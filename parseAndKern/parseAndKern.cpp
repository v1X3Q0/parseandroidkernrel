#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>
#include "spare_vmlinux.h"
#include "parseAndKern.h"

int kern_img::grab_sinittext()
{
    int result = -1;
    hde_t tempInst = {0};
    SAFE_BAIL(parseInst(*binBegin, &tempInst) == -1);
    _sinittext = (uint32_t*)(tempInst.immLarge + (size_t)binBegin);

    result = 0;
fail:
    return result;
}

void kern_img::insert_section(std::string sec_name, uint16_t sh_type, uint64_t sh_flags,
    uint64_t sh_addr, uint64_t sh_offset, uint64_t sh_size, uint16_t sh_link,
    uint16_t sh_info, uint64_t sh_addralign, uint64_t sh_entsize)
{
    sect_list[sec_name] = { 0, sh_type, sh_flags, sh_addr, sh_offset, sh_size, sh_link,
        sh_info, sh_addralign, sh_entsize };    
}

int kern_img::base_inits()
{
    int result = -1;
    instSet getB;
    uint32_t* text_start = 0;

    FINISH_IF((check_sect(".head.text", NULL) == 0) &&
        (check_sect(".text", NULL) == 0) && (check_sect(".init.text", NULL) == 0));

    insert_section(".head.text", 0, 0, RESOLVE_REL(binBegin), RESOLVE_REL(binBegin), 0, 0, 0, 8, 0);
    
    getB.addNewInst(cOperand::createASI<size_t, size_t, saveVar_t*>(SP, SP, getB.checkOperand(0)));
    SAFE_BAIL(getB.findPattern(binBegin, PAGE_SIZE * 4, &text_start) == -1);
    sect_list[".head.text"].sh_size = (size_t)text_start - (size_t)binBegin;
    insert_section(".text", 0, 0, RESOLVE_REL(text_start), RESOLVE_REL(text_start), 0, 0, 0, 8, 0);

    insert_section(".init.text", 0, 0, RESOLVE_REL(_sinittext), RESOLVE_REL(_sinittext), 0, 0, 0, 8, 0);

finish:
    result = 0;
fail:
    return result;
}

int kern_img::grab_primary_switch()
{
    int result = -1;
    instSet getB;
    uint32_t* primSwitchBAddr = 0;
    size_t primSwitchOff = 0;

    getB.addNewInst(cOperand::createB<saveVar_t*>(getB.checkOperand(0)));
    SAFE_BAIL(getB.findPattern(_sinittext, PAGE_SIZE, &primSwitchBAddr) == -1);

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
    // getB.addNewInst(cOperand::createLDRL<saveVar_t*, saveVar_t*>(getB.checkOperand(0), getB.checkOperand(1)));
    // SAFE_BAIL(getB.findPattern(__primary_switch_g, PAGE_SIZE, &primSwitchedLdrAddr) == -1);

    // getB.getVar(1, &primSwitchOff);

    // tmpMath = *(size_t*)(primSwitchOff + (size_t)primSwitchedLdrAddr);
    // __primary_switched_g = (uint32_t*)(tmpMath + (size_t)binBegin);

    getB.addNewInst(new cOperand(ARM64_ISB_OP));
    getB.addNewInst(cOperand::createBL<saveVar_t*>(getB.checkOperand(0)));
    SAFE_BAIL(getB.findPattern(__primary_switch, PAGE_SIZE, &create_page_tablesAddr) == -1);

    getB.getVar(0, &create_page_tablesOff);
    __create_page_tables = (uint32_t*)(create_page_tablesOff + (size_t)create_page_tablesAddr + sizeof(uint32_t));

    getB.clearInternals();
    getB.addNewInst(cOperand::createADRP<saveVar_t*, saveVar_t*>(getB.checkOperand(0), getB.checkOperand(1)));
    getB.addNewInst(cOperand::createASI<size_t, saveVar_t*, size_t>(SP, getB.checkOperand(0), 4));
    SAFE_BAIL(getB.findPattern(__create_page_tables, PAGE_SIZE, &__primary_switched) == -1);

    result = 0;
fail:
    return result;
}

size_t kern_img::resolveRel(size_t rebase)
{
    return rebase - (size_t)binBegin;
}

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
    getB.addNewInst(cOperand::createADRP<saveVar_t*, saveVar_t*>(getB.checkOperand(0), getB.checkOperand(1)));
    getB.addNewInst(cOperand::createADRP<saveVar_t*, saveVar_t*>(getB.checkOperand(2), getB.checkOperand(3)));
    getB.addNewInst(cOperand::createASI<saveVar_t*, saveVar_t*, saveVar_t*>(getB.checkOperand(0), getB.checkOperand(0), getB.checkOperand(4)));
    getB.addNewInst(cOperand::createASI<saveVar_t*, saveVar_t*, saveVar_t*>(getB.checkOperand(2), getB.checkOperand(2), getB.checkOperand(5)));
    getB.addNewInst(cOperand::createLI<saveVar_t*, size_t, size_t, size_t>(getB.checkOperand(6), X31,  0x39, 0x3));

    SAFE_BAIL(getB.findPattern(start_kernel, PAGE_SIZE, &modverAddr) == -1);

    getB.getVar(3, &modverOff);
    modvertmp = modverOff + ((size_t)(modverAddr + sizeof(uint32_t)) & ~PAGE_MASK);
    getB.getVar(5, &modverOff);
    modvertmp += modverOff;
    insert_section("__modver", 0, 0, RESOLVE_REL(modvertmp), RESOLVE_REL(modvertmp), 0, 0, 0, 8, 0);

    getB.getVar(1, &modverOff);
    paramtmp = modverOff + ((size_t)(modverAddr) & ~PAGE_MASK);
    getB.getVar(4, &modverOff);
    paramtmp += modverOff;
    insert_section("__param", 0, 0, RESOLVE_REL(paramtmp), RESOLVE_REL(paramtmp), modvertmp - paramtmp, 0, 0, 8, 0);

    base_ex_table();
    sect_list["__modver"].sh_size = UNRESOLVE_REL(sect_list["__ex_table"].sh_addr) - modvertmp;

finish:
    result = 0;
fail:
    return result;
}

int kern_img::grab_start_kernel_g()
{
    int result = -1;
    instSet getB;
    size_t start_kernelOff = 0;

    getB.addNewInst(cOperand::createB<saveVar_t*>(getB.checkOperand(0)));
    SAFE_BAIL(getB.findPattern(__primary_switched, PAGE_SIZE, &start_kernel) == -1);

    getB.getVar(0, &start_kernelOff);
    start_kernel = (uint32_t*)(start_kernelOff + (size_t)start_kernel);

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
    // make sure we don't search memory forever
    int ksymAssumeSize = 0x40000;
    int dbgCounter = 0;
    const char targSymName[] = "system_state";
    size_t targSymLen = strlen(targSymName);
    const char* targSymEnd = (const char*)((size_t)targSymName + targSymLen - 1);
    Elf64_Shdr* paramSec = 0;
    size_t ksymtabstr_tmp = 0;

    // check if kcrc already exists
    FINISH_IF(check_sect("__ksymtab_strings", NULL) == 0);

    // grab the base that i need
    SAFE_BAIL(check_sect("__param", &paramSec) == -1);

    offsetTmp = rfindnn((const char*)UNRESOLVE_REL(paramSec->sh_addr), DEFAULT_SEARCH_SIZE);
    SAFE_BAIL(offsetTmp == -1);
    curStr = (const char*)(UNRESOLVE_REL(paramSec->sh_addr) - offsetTmp);
    dbgCounter = offsetTmp;

    while (dbgCounter < ksymAssumeSize)
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
    insert_section("__ksymtab_strings", 0, 0, RESOLVE_REL(ksymtabstr_tmp), RESOLVE_REL(ksymtabstr_tmp), UNRESOLVE_REL(paramSec->sh_addr) - ksymtabstr_tmp, 0, 0, 8, 0);
finish:
    result = 0;
fail:
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

    crcIter = (uint32_t*)(UNRESOLVE_REL(ksymtabStr->sh_addr) - sizeof(uint32_t));

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
    insert_section("__kcrctab", 0, 0, RESOLVE_REL(crcIter), RESOLVE_REL(crcIter), 0, 0, 0, 8, 0);
    sect_list["__kcrctab"].sh_size = UNRESOLVE_REL(sect_list["__ksymtab_strings"].sh_addr) - (size_t)crcIter;
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
    ksymtabTmp = (UNRESOLVE_REL(crcSec->sh_addr) - sizeof(kernel_symbol) * ksyms_count);
    insert_section("__ksymtab", 0, 0, RESOLVE_REL(ksymtabTmp), RESOLVE_REL(ksymtabTmp), 0, 0, 0, 8, 0);

    // instSet getB;
    // size_t start_kernelOff = 0;
    // uint32_t* modverAddr = 0;
    // size_t modverOff = 0;

    // getB.addNewInst(cOperand::createMWI<size_t, size_t>(1, 0x80c0));
    // getB.addNewInst(cOperand::createB<saveVar_t*>(getB.checkOperand(0)));
    // SAFE_BAIL(getB.findPattern(__primary_switched_g, PAGE_SIZE, &start_kernel_g) == -1);

    // getB.getVar(0, &start_kernelOff);
    // start_kernel_g = (uint32_t*)(start_kernelOff + (size_t)start_kernel_g);

    // getB.clearInternals();
    // getB.addNewInst(cOperand::createADRP<saveVar_t*, saveVar_t*>(getB.checkOperand(0), getB.checkOperand(1)));
    // getB.addNewInst(cOperand::createADRP<saveVar_t*, saveVar_t*>(getB.checkOperand(2), getB.checkOperand(3)));
    // getB.addNewInst(cOperand::createASI<saveVar_t*, saveVar_t*, saveVar_t*>(getB.checkOperand(0), getB.checkOperand(0), getB.checkOperand(4)));
    // getB.addNewInst(cOperand::createASI<saveVar_t*, saveVar_t*, saveVar_t*>(getB.checkOperand(2), getB.checkOperand(2), getB.checkOperand(5)));
    // getB.addNewInst(cOperand::createLI<saveVar_t*, size_t, size_t, size_t>(getB.checkOperand(6), X31,  0x39, 0x3));

finish:
    result = 0;
fail:
    return result;
}

int kern_img::gen_shstrtab(std::string** out_shstrtab, uint16_t* numSects, uint16_t* shstrtab_index)
{
    std::string shstrtabTmp = "";
    shstrtabTmp += '\0';
    uint16_t sect_iter = 0;
    uint16_t str_index = 0;
    
    for (auto i = sect_list.begin(); i != sect_list.end(); i++)
    {
        i->second.sh_name = shstrtabTmp.size();
        shstrtabTmp = shstrtabTmp + i->first + '\0';
        if (i->first == ".shstrtab")
        {
            str_index = sect_iter;
        }
        sect_iter++;
    }

finish:
    if (out_shstrtab != 0)
    {
        *out_shstrtab = new std::string(shstrtabTmp);
    }
    if (numSects != 0)
    {
        *numSects = sect_iter;
    }
    if (shstrtab_index != 0)
    {
        *shstrtab_index = str_index;
    }

    return 0;
}

int kern_img::gen_vmlinux_sz(size_t* outSz, size_t headOffset)
{
    size_t szTemp = 0;
    std::string* shstrtab_tmp;

    // szTemp += sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);
    // szTemp += PAGE_SIZE;
    szTemp += headOffset;
    szTemp += kern_sz;
    
    // generate shstrtab
    gen_shstrtab(&shstrtab_tmp, NULL, NULL);
    szTemp += shstrtab_tmp->size();
    szTemp += (sect_list.size() * sizeof(Elf64_Shdr));

    *outSz = szTemp;
    return 0;
}

int kern_img::findKindInKstr(const char* newString, int* index)
{
    const char* strIter = 0;
    Elf64_Shdr* ksymstrSec = 0;
    int result = -1;
    int i = 0;

    SAFE_BAIL(check_sect("__ksymtab_strings", &ksymstrSec) == -1);
    strIter = (const char*)UNRESOLVE_REL(ksymstrSec->sh_addr);

    for (; i < ksyms_count; i++)
    {
        if (strcmp(newString, strIter) == 0)
        {
            goto finish_eval;
        }
        if (*(uint16_t*)(strIter + strlen(strIter)) == 0)
        {
            break;
        }
        strIter = strIter + strlen(strIter) + 1;
    }

    goto fail;
finish_eval:
    *index = i;
finish:
    result = 0;
fail:
    return result;
}

int kern_img::check_sect(std::string sect_name, Elf64_Shdr** sect_out)
{
    int result = -1;

    SAFE_BAIL(sect_list.find(sect_name) == sect_list.end());

    if (sect_out != 0)
    {
        *sect_out = &sect_list[sect_name];
    }

    result = 0;
fail:
    return result;
}

int kern_img::base_modver()
{
    int result = -1;
    kernel_param* paramIter = 0;
    Elf64_Shdr* paramSec = 0;

    // check if modver already exists
    SAFE_BAIL(check_sect("__modver", NULL) == -1);

    // grab the base that i need
    SAFE_BAIL(check_sect("__param", &paramSec) == -1);

    paramIter = (kernel_param*)UNRESOLVE_REL(paramSec->sh_addr);

    while (paramIter->perm)
    {
        paramIter++;
    }

    insert_section("__modver", 0, 0, RESOLVE_REL(paramIter), RESOLVE_REL(paramIter), 0, 0, 0, 8, 0);
    base_ex_table();
    sect_list["__modver"].sh_size = UNRESOLVE_REL(sect_list["__ex_table"].sh_addr) - (size_t)paramIter;

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

    FINISH_IF(check_sect("__ex_table", NULL) == 0);

    SAFE_BAIL(check_sect("__modver", &modverSec) == -1);
    SAFE_BAIL(check_sect(".init.text", &inittextSec) == -1);

    modverIter = (void**)UNRESOLVE_REL(modverSec->sh_addr);

    while (*modverIter)
    {
        modverIter++;
    }

    ex_tableSz = UNRESOLVE_REL(inittextSec->sh_addr) - (uint64_t)modverIter;

    insert_section("__ex_table", 0, 0, RESOLVE_REL(modverIter), RESOLVE_REL(modverIter), ex_tableSz, 0, 0, 8, 0);

finish:
    result = 0;
fail:
    return result;
}

int kern_img::patch_and_write(void* vmlinux_cur, size_t offset)
{
    int result = -1;

    for (auto i = sect_list.begin(); i != sect_list.end(); i++)
    {
        i->second.sh_offset += offset;
        memcpy(vmlinux_cur, &i->second, sizeof(Elf64_Shdr));
        vmlinux_cur = (void*)((size_t)vmlinux_cur + sizeof(Elf64_Shdr));
    }

    result = 0;
    return result;
}

int kern_img::base_new_shstrtab()
{
    char strtabRef[] = ".shstrtab";
    std::string* shtstrtab_tmp = 0;

    gen_shstrtab(&shtstrtab_tmp, NULL, NULL);
    insert_section(strtabRef, SHT_STRTAB, 0, kern_sz, kern_sz, shtstrtab_tmp->size() + sizeof(strtabRef), 0, 0, 1, 0);
    return 0;
}

int kern_img::parseAndGetGlobals()
{
    int result = -1;
    int snprintfInd = 0;
    uint32_t snprintfCrc = 0;

    SAFE_BAIL(grab_sinittext() == -1);
    SAFE_BAIL(grab_primary_switch() == -1);
    SAFE_BAIL(grab_primary_switched() == -1);
    SAFE_BAIL(grab_start_kernel_g() == -1);

    SAFE_BAIL(base_inits() == -1);
    SAFE_BAIL(base_modverparam() == -1);
    SAFE_BAIL(base_ksymtab_strings() == -1);
    SAFE_BAIL(base_kcrctab() == -1);
    SAFE_BAIL(base_ksymtab() == -1);
    SAFE_BAIL(base_ex_table() == -1);
    SAFE_BAIL(base_new_shstrtab() == -1);

    SAFE_BAIL(findKindInKstr("printk", &snprintfInd) == -1);
    snprintfCrc = get_kcrctab()[snprintfInd];

    result = 0;
fail:
    return result;
}
