#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>

#include <krw_util.h>

#include "spare_vmlinux.h"
#include "parseAndKern.h"

int kern_img::live_kern_addr(void* target_kernel_address, size_t size_kernel_buf, void** out_live_addr)
{
    int result = -1;
    void* newKernelAddress = 0;

    if (live_kernel == false)
    {
        *out_live_addr = target_kernel_address;
        result = 0;
        goto finish;
    }

    newKernelAddress = calloc(size_kernel_buf, 1);
    SAFE_BAIL(newKernelAddress == 0);
    SAFE_BAIL(kRead(target_kernel_address, size_kernel_buf, (size_t)target_kernel_address) == -1);

    result = 0;
    goto finish;
fail:
    SAFE_FREE(newKernelAddress);
finish:
    return result;
}

int kern_img::kernel_search(instSet* getB, void* img_var, size_t img_var_sz, uint32_t** out_img_off)
{
    int result = -1;
    void* img_var_local = 0;

    SAFE_BAIL(live_kern_addr(img_var, img_var_sz, &img_var_local) == -1);
    SAFE_BAIL(getB->findPattern((uint32_t*)img_var_local, img_var_sz, out_img_off) == -1);

    result = 0;
fail:
    return result;
}

int kern_img::grab_sinittext()
{
    int result = -1;
    hde_t tempInst = {0};
    uint32_t* binBegMap = 0;
    
    SAFE_BAIL(live_kern_addr((void*)binBegin, sizeof(*binBegin), (void**)&binBegMap) == -1);
    SAFE_BAIL(parseInst(*binBegMap, &tempInst) == -1);
    _sinittext = (uint32_t*)(tempInst.immLarge + (size_t)binBegin);

    result = 0;
fail:
    return result;
}

void kern_img::insert_section(std::string sec_name, uint16_t sh_type, uint64_t sh_flags,
    uint64_t sh_addr, uint64_t sh_offset, uint64_t sh_size, uint16_t sh_link,
    uint16_t sh_info, uint64_t sh_addralign, uint64_t sh_entsize)
{
    Elf64_Shdr* newShdr = 0;
    Elf64_Phdr* newPhdr = 0;
    Elf64_Word p_type = 0;
    Elf64_Xword p_align = 0;
    Elf64_Word p_flags = 0;
    
    if (
        (sec_name == ".symtab") ||
        (sec_name == ".strtab") ||
        (sec_name == ".shstrtab")
        )
    {

    }
    else
    {
        sh_addr = R_KA(RESOLVE_REL(sh_addr));
        sh_offset = RESOLVE_REL(sh_offset);
        p_type = PT_LOAD;
        p_align = 0x10000;

        sh_flags = SHF_ALLOC;
        p_flags = PF_R;
        // found a text, add executable flag
        if (sec_name.find("text") != std::string::npos)
        {
            sh_flags |= SHF_EXECINSTR | SHF_WRITE;
            p_flags |= PF_X | PF_W;
        }
        else
        {
            sh_flags |= SHF_WRITE;
            p_flags |= PF_W;
        }
    }

    newShdr = new Elf64_Shdr{
        0,
        sh_type,
        sh_flags,
        sh_addr,
        sh_offset,
        sh_size,
        sh_link,
        sh_info,
        sh_addralign,
        sh_entsize
        };

    sect_list.push_back({sec_name, newShdr});

    if (p_type == PT_LOAD)
    {
        newPhdr = new Elf64_Phdr{
            p_type,
            p_flags,
            sh_offset,
            sh_addr,
            sh_addr,
            sh_size,
            sh_size,
            p_align
        };

        prog_list.push_back({sec_name, newPhdr});
    }
}

int kern_img::grab_primary_switch()
{
    int result = -1;
    instSet getB;
    uint32_t* primSwitchBAddr = 0;
    size_t primSwitchOff = 0;

    getB.addNewInst(cOperand::createB<saveVar_t>(getB.checkOperand(0)));
    SAFE_BAIL(kernel_search(&getB, _sinittext, PAGE_SIZE, &primSwitchBAddr) == -1);

    getB.getVar(0, &primSwitchOff);
    __primary_switch = (uint32_t*)(primSwitchOff + (size_t)primSwitchBAddr);
    
    result = 0;
fail:
    return result;
}

int kern_img::grab_primary_switched()
{
    int result = -1;
    instSet getB;
    uint32_t* primSwitchedLdrAddr = 0;
    size_t primSwitchOff = 0;
    uint32_t* create_page_tablesAddr = 0;
    size_t create_page_tablesOff = 0;

// the old way,was find a particular branch, resolve the math for an ldr and
// afterwards add it to the found address. changed to resolving an adjacent
// routine and looking for a prologue.
    // size_t tmpMath = 0;

    // operand 1 is the immediate19, operand 2 is the register
    // getB.addNewInst(cOperand::createLDRL<saveVar_t, saveVar_t>(getB.checkOperand(0), getB.checkOperand(1)));
    // SAFE_BAIL(getB.findPattern(__primary_switch_g, PAGE_SIZE, &primSwitchedLdrAddr) == -1);

    // getB.getVar(1, &primSwitchOff);

    // tmpMath = *(size_t*)(primSwitchOff + (size_t)primSwitchedLdrAddr);
    // __primary_switched_g = (uint32_t*)(tmpMath + (size_t)binBegin);

    getB.addNewInst(new cOperand(ARM64_ISB_OP));
    getB.addNewInst(cOperand::createBL<saveVar_t>(getB.checkOperand(0)));
    SAFE_BAIL(kernel_search(&getB, __primary_switch, PAGE_SIZE, &create_page_tablesAddr) == -1);

    getB.getVar(0, &create_page_tablesOff);
    __create_page_tables = (uint32_t*)(create_page_tablesOff + (size_t)create_page_tablesAddr + sizeof(uint32_t));
    
    getB.clearInternals();
    getB.addNewInst(cOperand::createADRP<saveVar_t, saveVar_t>(getB.checkOperand(0), getB.checkOperand(1)));
    getB.addNewInst(cOperand::createASI<size_t, saveVar_t, size_t>(SP, getB.checkOperand(0), 4));
    SAFE_BAIL(kernel_search(&getB, __create_page_tables, PAGE_SIZE, &__primary_switched) == -1);

    result = 0;
fail:
    return result;
}

size_t kern_img::resolveRel(size_t rebase)
{
    return rebase - (size_t)binBegin;
}

int kern_img::grab_start_kernel_g()
{
    int result = -1;
    instSet getB;
    size_t start_kernelOff = 0;

    getB.addNewInst(cOperand::createB<saveVar_t>(getB.checkOperand(0)));
    SAFE_BAIL(kernel_search(&getB, __primary_switched, PAGE_SIZE, &start_kernel) == -1);

    getB.getVar(0, &start_kernelOff);
    start_kernel = (uint32_t*)(start_kernelOff + (size_t)start_kernel);

finish:
    result = 0;
fail:
    return result;
}

