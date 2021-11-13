#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>

uint32_t* binBegin = 0;
uint32_t* _sinittext_g = 0;
uint32_t* __primary_switch_g = 0;
uint32_t* __primary_switched_g = 0;
uint32_t* __create_page_tables_g = 0;
uint32_t* start_kernel_g = 0;

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
    size_t tmpMath = 0;
    size_t primSwitchOff = 0;
    uint32_t* create_page_tablesAddr = 0;
    size_t create_page_tablesOff = 0;

// the old way,was find a particular branch, resolve the math for an ldr and
// afterwards add it to the found address. changed to resolving an adjacent
// routine and looking for a prologue.

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

    getB.clearInstructions();
    getB.addNewInst(cOperand::createADRP<saveVar_t*, saveVar_t*>(getB.checkOperand(0), getB.checkOperand(1)));
    getB.addNewInst(cOperand::createASI<size_t, saveVar_t*, size_t>(SP, getB.checkOperand(0), 4));
    SAFE_BAIL(getB.findPattern(__create_page_tables_g, PAGE_SIZE, &__primary_switched_g) == -1);

    result = 0;
fail:
    return result;
}

// int grab_start_kernel_g()
// {
//     int result = -1;
//     instSet getB;
//     opcOperandVar* startkernOff;
//     uint32_t* startkernBAddr = 0;

//     getB.insertInst_t<immediateVar>(ARM64_B_OP, NULL_INST, 0);
//     startkernBAddr = getB.ffindRegexInst(__primary_switched_g, PAGE_SIZE);
//     SAFE_BAIL(startkernBAddr == 0);

//     getB.varValueForKey(0, &startkernOff);
//     start_kernel_g = (uint32_t*)(startkernOff->getVal() + (size_t)startkernBAddr);
    
//     result = 0;
// fail:
//     return result;
// }

int parseAndGetGlobals()
{
    int result = -1;

    SAFE_BAIL(grab_sinittext() == -1);
    SAFE_BAIL(grab_primary_switch() == -1);
    SAFE_BAIL(grab_primary_switched() == -1);

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