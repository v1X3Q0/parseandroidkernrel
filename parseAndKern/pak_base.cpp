#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>
#include <kernel_block.h>

#include <krw_util.h>

#include "spare_vmlinux.h"
#include "parseAndKern.h"

void kern_static::insert_section(std::string sec_name, uint64_t sh_offset, uint64_t sh_size)
{
    Elf64_Shdr* newShdr = 0;
    Elf64_Phdr* newPhdr = 0;
    Elf64_Word p_type = 0;
    Elf64_Xword p_align = 0;
    Elf64_Word p_flags = 0;

    uint16_t sh_type = SHT_PROGBITS;
    uint64_t sh_flags = 0;
    uint64_t sh_addr = sh_offset;
    // uint64_t sh_offset = 0;
    // uint64_t sh_size = 0;
    uint16_t sh_link = 0;
    uint16_t sh_info = 0;
    uint64_t sh_addralign = 0;
    uint64_t sh_entsize = 0;

    if (sec_name == ".head.text")
    {
        sh_addralign = sizeof(int);
    }
    else if (sec_name == ".text")
    {
        sh_addralign = 2048;
    }
    else if (sec_name == "__ksymtab")
    {
        sh_addralign = 8;
    }
    else if (sec_name == "__kcrctab")
    {
        sh_addralign = 1;
    }
    else if (sec_name == "__ksymtab_strings")
    {
        sh_addralign = 1;
    }
    else if (sec_name == "__param")
    {
        sh_addralign = 8;
    }
    else if (sec_name == "__modver")
    {
        sh_addralign = 8;
    }
    else if (sec_name == "__ex_table")
    {
        sh_addralign = 8;
    }
    else if (sec_name == ".init.text")
    {
        sh_addralign = 8;
    }
    
    if (
        (sec_name == ".symtab") ||
        (sec_name == ".strtab") ||
        (sec_name == ".shstrtab")
        )
    {

    }
    else
    {
        if (live_kernel == false)
        {
            sh_addr = R_KA(RESOLVE_REL(sh_addr));
        }
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

int kern_static::base_ksymtab_strings()
{
    int result = -1;
    const char* curStr = 0;
    const char* prevStr = 0;
    size_t offsetTmp = 0;
    int dbgCounter = 0;
    const char targSymName[] = "system_state";
    size_t targSymLen = strlen(targSymName);
    const char* targSymEnd = (const char*)((size_t)targSymName + targSymLen - 1);
    Elf64_Shdr* paramSec = 0;
    size_t ksymtabstr_tmp = 0;

    // make sure we don't search memory forever
    const size_t ksymstrAssumeSize = 0x40000;
    size_t paramSec_start = 0;
    void* ksymstrBuf = 0;

    // check if kcrc already exists
    FINISH_IF(check_sect("__ksymtab_strings", NULL) == 0);

    // grab the base that i need
    SAFE_BAIL(check_sect("__param", &paramSec) == -1);

#ifdef LIVE_KERNEL
#else
#endif
    if (live_kernel == true)
    {
        live_kern_addr(paramSec->sh_offset - ksymstrAssumeSize, ksymstrAssumeSize, &ksymstrBuf);
        paramSec_start = (size_t)ksymstrBuf + ksymstrAssumeSize;
    }
    else if (live_kernel == false)
    {
        paramSec_start = UNRESOLVE_REL(paramSec->sh_offset);
    }

    offsetTmp = rfindnn((const char*)paramSec_start, DEFAULT_SEARCH_SIZE);
    SAFE_BAIL(offsetTmp == -1);
    curStr = (const char*)(paramSec_start - offsetTmp);
    dbgCounter = offsetTmp;

    while (dbgCounter < ksymstrAssumeSize)
    {
        ksyms_count++;
        // strncmp("static_key_initialized", cmpStr, DEFAULT_SEARCH_SIZE)
        if (rstrncmp(curStr, targSymEnd, targSymLen) == 0)
        {
            goto finish_eval;
        }
        offsetTmp = rstrnlen(curStr, DEFAULT_SEARCH_SIZE);
        SAFE_BAIL(offsetTmp == -1);
        // you're at the last non null character, subtract the offset to get to the next
        // null character. add 1 so that you're string begins at the next non null character.
        curStr = curStr - offsetTmp;
        prevStr = curStr + 1;

        // printf("%s\n", prevStr);

        // subtract 1 so you can start at the next non null character, the last non null char.
        curStr -= 1;
        dbgCounter += offsetTmp;
    }
    goto fail;

finish_eval:
    ksymtabstr_tmp = (size_t)(curStr - targSymLen + 1);
    insert_section("__ksymtab_strings", ksymtabstr_tmp, paramSec_start - ksymtabstr_tmp);
finish:
    result = 0;
fail:
    SAFE_FREE(ksymstrBuf);
    return result;
}

int kern_static::base_kcrctab()
{
    int result = -1;
    size_t crcCount = 0;
    uint32_t* crcIter = 0;
    Elf64_Shdr* ksymtabStr = 0;

    // check if kcrc already exists
    FINISH_IF(check_sect("__kcrctab", NULL) == 0);

    // grab the base that i need
    SAFE_BAIL(check_sect("__ksymtab_strings", &ksymtabStr) == -1);

    crcIter = (uint32_t*)(UNRESOLVE_REL(ksymtabStr->sh_offset) - sizeof(uint32_t));

    while (true)
    {
        if (*crcIter == *(crcIter - 2))
        {
            goto finish_eval;
        }
        crcCount++;
        crcIter--;
    }
    goto fail;

finish_eval:
    crcIter++;
    insert_section("__kcrctab", (size_t)crcIter, 0);
    find_sect("__kcrctab")->sh_size = UNRESOLVE_REL(find_sect("__ksymtab_strings")->sh_offset) - (size_t)crcIter;
    ksyms_count = crcCount;
finish:
    result = 0;
fail:
    return result;
}

int kern_static::base_ex_table()
{
    int result = -1;
    void** modverIter = 0;
    Elf64_Shdr* modverSec = 0;
    Elf64_Shdr* inittextSec = 0;
    size_t ex_tableSz = 0;

    // check if ex_table already exists
    FINISH_IF(check_sect("__ex_table", NULL) == 0);

    // dependencies
    SAFE_BAIL(check_sect("__modver", &modverSec) == -1);
    SAFE_BAIL(check_sect(".init.text", &inittextSec) == -1);

    modverIter = (void**)UNRESOLVE_REL(modverSec->sh_offset);

    while ((*modverIter) == 0)
    {
        modverIter++;
    }

    ex_tableSz = UNRESOLVE_REL(inittextSec->sh_offset) - (uint64_t)modverIter;

    insert_section("__ex_table", (size_t)modverIter, ex_tableSz);

finish:
    result = 0;
fail:
    return result;
}

int kern_static::base_new_shstrtab()
{
    char strtabRef[] = ".shstrtab";
    std::string* shtstrtab_tmp = 0;

    gen_shstrtab(&shtstrtab_tmp, NULL, NULL);
    insert_section(strtabRef, kern_sz, shtstrtab_tmp->size() + sizeof(strtabRef));
    return 0;
}

int kern_static::base_modver()
{
    int result = -1;
    kernel_param* paramIter = 0;
    Elf64_Shdr* paramSec = 0;

    // check if modver already exists
    FINISH_IF(check_sect("__modver", NULL) == 0);

    // grab the base that i need
    SAFE_BAIL(check_sect("__param", &paramSec) == -1);

    paramIter = (kernel_param*)UNRESOLVE_REL(paramSec->sh_offset);

    while (paramIter->perm)
    {
        paramIter++;
    }

    insert_section("__modver", (size_t)paramIter, 0);
    base_ex_table();
    find_sect("__modver")->sh_size = UNRESOLVE_REL(find_sect("__ex_table")->sh_offset) - (size_t)paramIter;

finish:
    result = 0;
fail:
    return result;
}

// this is a modverparam search, that only works if you havev the symbol
// start_kernel, which means that the .init.text section must be readable. It
// seems that for live kernels this isn't the case, so we will only use this
// routine for static analysis of kernels.
int kern_static::base_modverparam()
{
    int result = -1;
    instSet getB;
    uint32_t* modverAddr = 0;
    size_t modverOff = 0;
    size_t modvertmp = 0;
    size_t paramtmp = 0;

    // check if we already have what we are looking for
    FINISH_IF((check_sect("__modver", NULL) == 0) && (check_sect("__param", NULL) == 0));

    getB.clearInternals();
    getB.addNewInst(cOperand::createADRP<saveVar_t, saveVar_t>(getB.checkOperand(0), getB.checkOperand(1)));
    getB.addNewInst(cOperand::createADRP<saveVar_t, saveVar_t>(getB.checkOperand(2), getB.checkOperand(3)));
    getB.addNewInst(cOperand::createASI<saveVar_t, saveVar_t, saveVar_t>(getB.checkOperand(0), getB.checkOperand(0), getB.checkOperand(4)));
    getB.addNewInst(cOperand::createASI<saveVar_t, saveVar_t, saveVar_t>(getB.checkOperand(2), getB.checkOperand(2), getB.checkOperand(5)));
    getB.addNewInst(cOperand::createLI<saveVar_t, size_t, size_t, size_t>(getB.checkOperand(6), X31,  0x39, 0x3));

    SAFE_BAIL(kernel_search(&getB, KSYM_V(start_kernel), PAGE_SIZE, (void**)&modverAddr) == -1);

    getB.getVar(3, &modverOff);
    modvertmp = modverOff + ((size_t)(modverAddr + sizeof(uint32_t)) & ~PAGE_MASK);
    getB.getVar(5, &modverOff);
    modvertmp += modverOff;
    insert_section("__modver", (size_t)modvertmp, 0);

    getB.getVar(1, &modverOff);
    paramtmp = modverOff + ((size_t)(modverAddr) & ~PAGE_MASK);
    getB.getVar(4, &modverOff);
    paramtmp += modverOff;
    insert_section("__param", (size_t)paramtmp, modvertmp - paramtmp);

    SAFE_BAIL(base_ex_table() == -1);
    find_sect("__modver")->sh_size = UNRESOLVE_REL(find_sect("__ex_table")->sh_offset) - modvertmp;

finish:
    result = 0;
fail:
    return result;
}

// since don't have start_kernel, probably not gonna work for static either.
int kern_static::base_init_data()
{
    int result = -1;
    instSet getB;
    uint32_t* init_data_off = 0;
    size_t init_data_l = 0;
    size_t init_data_final = 0;

    getB.addNewInst(cOperand::createADRP<saveVar_t, saveVar_t>(
        getB.checkOperand(0), getB.checkOperand(1)));
    getB.addNewInst(cOperand::createLDRB<saveVar_t, saveVar_t, saveVar_t>(
        getB.checkOperand(2), getB.checkOperand(0), getB.checkOperand(4)));
    SAFE_BAIL(kernel_search(&getB, KSYM_V(start_kernel), PAGE_SIZE, (void**)&init_data_off) == -1);

    getB.getVar(0, &init_data_l);
    getB.getVar(4, &init_data_final);
    init_data_final += init_data_l;

    init_data_final = (size_t)(((size_t)init_data_off & ~PAGE_MASK) + (size_t)init_data_final);

finish:
    result = 0;
fail:
    return result;
}