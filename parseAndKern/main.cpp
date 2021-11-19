#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>

uint32_t* binBegin = 0;
uint32_t* _sinittext_g = 0;
uint32_t* __primary_switch_g = 0;
uint32_t* __primary_switched_g = 0;
uint32_t* __create_page_tables_g = 0;
uint32_t* start_kernel_g = 0;
size_t __modver_g;
size_t __param_g;
const char* __ksymtab_strings_g = 0;
uint32_t* __kcrctab_g = 0;
size_t ksyms_count = 0;

#define ARM64_MOVBI_OP_MASK     0x7f800000
#define ARM64_MOVBI_OP_SHIFT    23
#define ARM64_MOVBI_OP          0x64

#define ARM64_REG_RD_MASK       0x0000001f
#define ARM64_REG_RD_SHIFT      0

int grab_sinittext()
{
    int result = -1;
    hde_t tempInst = {0};
    SAFE_BAIL(parseInst(*binBegin, &tempInst) == -1);
    _sinittext_g = (uint32_t*)(tempInst.immLarge + (size_t)binBegin);

    result = 0;
fail:
    return result;
}

int grab_primary_switch()
{
    int result = -1;
    instSet getB;
    uint32_t* primSwitchBAddr = 0;
    size_t primSwitchOff = 0;

    getB.addNewInst(cOperand::createB<saveVar_t*>(getB.checkOperand(0)));
    SAFE_BAIL(getB.findPattern(_sinittext_g, PAGE_SIZE, &primSwitchBAddr) == -1);

    getB.getVar(0, &primSwitchOff);
    __primary_switch_g = (uint32_t*)(primSwitchOff + (size_t)primSwitchBAddr);
    
    result = 0;
fail:
    return result;
}

int grab_primary_switched()
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
    SAFE_BAIL(getB.findPattern(__primary_switch_g, PAGE_SIZE, &create_page_tablesAddr) == -1);

    getB.getVar(0, &create_page_tablesOff);
    __create_page_tables_g = (uint32_t*)(create_page_tablesOff + (size_t)create_page_tablesAddr + sizeof(uint32_t));

    getB.clearInternals();
    getB.addNewInst(cOperand::createADRP<saveVar_t*, saveVar_t*>(getB.checkOperand(0), getB.checkOperand(1)));
    getB.addNewInst(cOperand::createASI<size_t, saveVar_t*, size_t>(SP, getB.checkOperand(0), 4));
    SAFE_BAIL(getB.findPattern(__create_page_tables_g, PAGE_SIZE, &__primary_switched_g) == -1);

    result = 0;
fail:
    return result;
}

int grab_start_kernel_g()
{
    int result = -1;
    instSet getB;
    size_t start_kernelOff = 0;
    uint32_t* modverAddr = 0;
    size_t modverOff = 0;

    getB.addNewInst(cOperand::createB<saveVar_t*>(getB.checkOperand(0)));
    SAFE_BAIL(getB.findPattern(__primary_switched_g, PAGE_SIZE, &start_kernel_g) == -1);

    getB.getVar(0, &start_kernelOff);
    start_kernel_g = (uint32_t*)(start_kernelOff + (size_t)start_kernel_g);

    getB.clearInternals();
    getB.addNewInst(cOperand::createADRP<saveVar_t*, saveVar_t*>(getB.checkOperand(0), getB.checkOperand(1)));
    getB.addNewInst(cOperand::createADRP<saveVar_t*, saveVar_t*>(getB.checkOperand(2), getB.checkOperand(3)));
    getB.addNewInst(cOperand::createASI<saveVar_t*, saveVar_t*, saveVar_t*>(getB.checkOperand(0), getB.checkOperand(0), getB.checkOperand(4)));
    getB.addNewInst(cOperand::createASI<saveVar_t*, saveVar_t*, saveVar_t*>(getB.checkOperand(2), getB.checkOperand(2), getB.checkOperand(5)));
    getB.addNewInst(cOperand::createLI<saveVar_t*, size_t, size_t, size_t>(getB.checkOperand(6), X31,  0x39, 0x3));

    SAFE_BAIL(getB.findPattern(start_kernel_g, PAGE_SIZE, &modverAddr) == -1);

    getB.getVar(3, &modverOff);    
    __modver_g = modverOff + ((size_t)(modverAddr + sizeof(uint32_t)) & ~PAGE_MASK);
    getB.getVar(5, &modverOff);
    __modver_g += modverOff;

    getB.getVar(1, &modverOff);
    __param_g = modverOff + ((size_t)(modverAddr) & ~PAGE_MASK);
    getB.getVar(4, &modverOff);
    __param_g += modverOff;

    result = 0;
fail:
    return result;
}

int base_ksymtab_strings()
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

    offsetTmp = rfindnn((const char*)__param_g, DEFAULT_SEARCH_SIZE);
    SAFE_BAIL(offsetTmp == -1);
    curStr = (const char*)(__param_g - offsetTmp);
    dbgCounter = offsetTmp;

    while (dbgCounter < ksymAssumeSize)
    {
        ksyms_count++;
        // strncmp("static_key_initialized", cmpStr, DEFAULT_SEARCH_SIZE)
        if (rstrncmp(curStr, targSymEnd, targSymLen) == 0)
        {
            __ksymtab_strings_g = curStr - targSymLen + 1;
            result = 0;
            break;
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

fail:
    return result;
}

int base_kcrctab()
{
    int result = -1;
    uint32_t* crcIter = (uint32_t*)(__ksymtab_strings_g);
    size_t crcCount = 0;

    while (true)
    {
        crcCount++;
        if (*crcIter == *(crcIter - 2))
        {
            crcCount-=2;
            crcIter++;
            __kcrctab_g = crcIter;
            result = 0;
            break;
        }
        crcIter--;
    }

    return result;
}

int base_ksymtab()
{
    // here is asspull city.... gonna look for a hella regex. in execution, the routine
    // _request_firmware has a call to kmem_cache_alloc_trace(kmalloc_caches[0][7], 0x14080C0u, 0x20uLL);
    // where args 2 and 3 are the gfp flags and size. because i believe them to be measurable enough,
    // as well as arguments, lets give them a looksie....

    int result = -1;
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

    return result;
}

int findKindInKstr(const char* newString, int* index)
{
    const char* strIter = __ksymtab_strings_g;
    int result = -1;

    for (int i = 0; i < ksyms_count; i++)
    {
        if (strcmp(newString, strIter) == 0)
        {
            result = 0;
            *index = i;
            break;
        }
        if (*(uint16_t*)(strIter + strlen(strIter)) == 0)
        {
            break;
        }
        strIter = strIter + strlen(strIter) + 1;
    }
    return result;
}

int parseAndGetGlobals()
{
    int result = -1;
    int snprintfInd = 0;
    uint32_t snprintfCrc = 0;

    SAFE_BAIL(grab_sinittext() == -1);
    SAFE_BAIL(grab_primary_switch() == -1);
    SAFE_BAIL(grab_primary_switched() == -1);
    SAFE_BAIL(grab_start_kernel_g() == -1);
    SAFE_BAIL(base_ksymtab_strings() == -1);
    SAFE_BAIL(base_kcrctab() == -1);

    SAFE_BAIL(findKindInKstr("printk", &snprintfInd) == -1);
    snprintfCrc = __kcrctab_g[snprintfInd];

    result = 0;
fail:
    return result;
}

void usage(char** argv)
{
    fprintf(stderr, "Usage: %s [-k kernelimage]\n",
            argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    int flags = 0;
    int opt = 0;
    const char* kernName = 0;
    FILE * fp = 0;
    size_t fileSize = 0;

    while ((opt = getopt(argc, argv, "k:")) != -1)
    {
        switch (opt)
        {
        case 'k':
            kernName = optarg;
            break;
        default: /* '?' */
            usage(argv);
        }
    }

    if (kernName == 0)
    {
        usage(argv);
    }


    fp = fopen(kernName, "r");
    SAFE_BAIL(fp == 0);

    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    SAFE_BAIL(posix_memalign((void**)&binBegin, PAGE_SIZE, fileSize) != 0);

    SAFE_BAIL(fread(binBegin, 1, fileSize, fp) != fileSize);

    SAFE_BAIL(parseAndGetGlobals() == -1);

fail:
    SAFE_FCLOSE(fp);
    SAFE_FREE(binBegin);
    return 0;
}