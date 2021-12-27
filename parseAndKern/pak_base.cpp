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

int kern_img::base_inits()
{
    int result = -1;
    instSet getB;
    uint32_t* text_start = 0;

    uint32_t nonzeroLook = 0;
    FINISH_IF((check_sect(".head.text", NULL) == 0) &&
        (check_sect(".text", NULL) == 0) && (check_sect(".init.text", NULL) == 0));

    insert_section(".head.text", SHT_PROGBITS, 0, (size_t)binBegin, (size_t)binBegin, 0, 0, 0, 4, 0);
    
    // SO originally i searched for the first sub sp operation. HOWEVER it seems like on different
    // devices and kernels the first routine may not even start with a sub, but rather an stp.
    // if this is the case.... well gonna be harder to detect. so another option is either looking for
    // page, or first nonzero word after 0x40, gonna stick with the latter.

    // getB.addNewInst(cOperand::createASI<size_t, size_t, saveVar_t>(SP, SP, getB.checkOperand(0)));
    // SAFE_BAIL(kernel_search(&getB, binBegin, PAGE_SIZE * 4, &text_start) == -1);

    SAFE_BAIL(kernel_search_seq(binBegin, PAGE_SIZE * 2, (uint8_t*)&nonzeroLook, sizeof(nonzeroLook),
        0x40, sizeof(nonzeroLook), false, (void**)&text_start) == -1);
    find_sect(".head.text")->sh_size = (size_t)text_start - (size_t)binBegin;
    insert_section(".text", SHT_PROGBITS, 0, (size_t)text_start, (size_t)text_start, 0, 0, 0, 2048, 0);

    insert_section(".init.text", SHT_PROGBITS, 0, (size_t)_sinittext, (size_t)_sinittext, 0, 0, 0, 8, 0);

finish:
    result = 0;
fail:
    return result;
}

int kern_img::base_ksymtab_strings()
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
        live_kern_addr((void*)(paramSec->sh_offset - ksymstrAssumeSize), ksymstrAssumeSize, &ksymstrBuf);
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
    insert_section("__ksymtab_strings", SHT_PROGBITS, 0, ksymtabstr_tmp, ksymtabstr_tmp, paramSec_start - ksymtabstr_tmp, 0, 0, 1, 0);
finish:
    result = 0;
fail:
    SAFE_FREE(ksymstrBuf);
    return result;
}

int kern_img::base_kcrctab()
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
    insert_section("__kcrctab", SHT_PROGBITS, 0, (size_t)crcIter, (size_t)crcIter, 0, 0, 0, 1, 0);
    find_sect("__kcrctab")->sh_size = UNRESOLVE_REL(find_sect("__ksymtab_strings")->sh_offset) - (size_t)crcIter;
    ksyms_count = crcCount;
finish:
    result = 0;
fail:
    return result;
}

int kern_img::base_ksymtab()
{
    // here is asspull city.... gonna look for a hella regex. in execution, the routine
    // _request_firmware has a call to kmem_cache_alloc_trace(kmalloc_caches[0][7], 0x14080C0u, 0x20uLL);
    // where args 2 and 3 are the gfp flags and size. because i believe them to be measurable enough,
    // as well as arguments, lets give them a looksie....

    int result = -1;
    size_t ksymtabTmp = 0;
    Elf64_Shdr* crcSec = 0;

    // check if ksymtab already exists
    FINISH_IF(check_sect("__ksymtab", NULL) == 0);

    // grab the base that i need
    SAFE_BAIL(check_sect("__kcrctab", &crcSec) == -1);

    SAFE_BAIL(ksyms_count == 0);
    ksymtabTmp = (UNRESOLVE_REL(crcSec->sh_offset) - sizeof(kernel_symbol) * ksyms_count);
    insert_section("__ksymtab", SHT_PROGBITS, 0, ksymtabTmp, ksymtabTmp, 0, 0, 0, 8, 0);

    // instSet getB;
    // size_t start_kernelOff = 0;
    // uint32_t* modverAddr = 0;
    // size_t modverOff = 0;

    // getB.addNewInst(cOperand::createMWI<size_t, size_t>(1, 0x80c0));
    // getB.addNewInst(cOperand::createB<saveVar_t>(getB.checkOperand(0)));
    // SAFE_BAIL(getB.findPattern(__primary_switched_g, PAGE_SIZE, &start_kernel_g) == -1);

    // getB.getVar(0, &start_kernelOff);
    // start_kernel_g = (uint32_t*)(start_kernelOff + (size_t)start_kernel_g);

    // getB.clearInternals();
    // getB.addNewInst(cOperand::createADRP<saveVar_t, saveVar_t>(getB.checkOperand(0), getB.checkOperand(1)));
    // getB.addNewInst(cOperand::createADRP<saveVar_t, saveVar_t>(getB.checkOperand(2), getB.checkOperand(3)));
    // getB.addNewInst(cOperand::createASI<saveVar_t, saveVar_t, saveVar_t>(getB.checkOperand(0), getB.checkOperand(0), getB.checkOperand(4)));
    // getB.addNewInst(cOperand::createASI<saveVar_t, saveVar_t, saveVar_t>(getB.checkOperand(2), getB.checkOperand(2), getB.checkOperand(5)));
    // getB.addNewInst(cOperand::createLI<saveVar_t, size_t, size_t, size_t>(getB.checkOperand(6), X31,  0x39, 0x3));

finish:
    result = 0;
fail:
    return result;
}

int kern_img::base_ex_table()
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

    insert_section("__ex_table", SHT_PROGBITS, 0, (size_t)modverIter, (size_t)modverIter, ex_tableSz, 0, 0, 8, 0);

finish:
    result = 0;
fail:
    return result;
}

int kern_img::base_new_shstrtab()
{
    char strtabRef[] = ".shstrtab";
    std::string* shtstrtab_tmp = 0;

    gen_shstrtab(&shtstrtab_tmp, NULL, NULL);
    insert_section(strtabRef, SHT_STRTAB, 0, 0, kern_sz, shtstrtab_tmp->size() + sizeof(strtabRef), 0, 0, 1, 0);
    return 0;
}

int kern_img::base_modver()
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

    insert_section("__modver", SHT_PROGBITS, 0, (size_t)paramIter, (size_t)paramIter, 0, 0, 0, 8, 0);
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
int kern_img::base_modverparam()
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

    SAFE_BAIL(kernel_search(&getB, start_kernel, PAGE_SIZE, &modverAddr) == -1);

    getB.getVar(3, &modverOff);
    modvertmp = modverOff + ((size_t)(modverAddr + sizeof(uint32_t)) & ~PAGE_MASK);
    getB.getVar(5, &modverOff);
    modvertmp += modverOff;
    insert_section("__modver", SHT_PROGBITS, 0, (size_t)modvertmp, (size_t)modvertmp, 0, 0, 0, 8, 0);

    getB.getVar(1, &modverOff);
    paramtmp = modverOff + ((size_t)(modverAddr) & ~PAGE_MASK);
    getB.getVar(4, &modverOff);
    paramtmp += modverOff;
    insert_section("__param", SHT_PROGBITS, 0, (size_t)paramtmp, (size_t)paramtmp, modvertmp - paramtmp, 0, 0, 8, 0);

    SAFE_BAIL(base_ex_table() == -1);
    find_sect("__modver")->sh_size = UNRESOLVE_REL(find_sect("__ex_table")->sh_offset) - modvertmp;

finish:
    result = 0;
fail:
    return result;
}

// since don't have start_kernel, probably not gonna work for static either.
int kern_img::base_init_data()
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
    SAFE_BAIL(kernel_search(&getB, start_kernel, PAGE_SIZE, &init_data_off) == -1);

    getB.getVar(0, &init_data_l);
    getB.getVar(4, &init_data_final);
    init_data_final += init_data_l;

    init_data_final = (size_t)(((size_t)init_data_off & ~PAGE_MASK) + (size_t)init_data_final);

finish:
    result = 0;
fail:
    return result;
}