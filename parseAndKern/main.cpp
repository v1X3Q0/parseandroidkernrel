#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <adrpParse/adrpParse.h>
#include <regExBin/regexInst.h>
#include <regExBin/instSet.h>
#include <localUtil/localUtil.h>

uint32_t* binBegin = 0;
uint32_t* _sinittext_g = 0;
uint32_t* __primary_switch_g = 0;
uint32_t* __primary_switched_g = 0;
uint32_t* start_kernel_g = 0;

#define ARM64_MOVBI_OP_MASK     0x7f800000
#define ARM64_MOVBI_OP_SHIFT    23
#define ARM64_MOVBI_OP          0x64

#define ARM64_REG_RD_MASK       0x0000001f
#define ARM64_REG_RD_SHIFT      0

#define GET_ARM64_BF(PC, INST, FIELD) \
    (PC & ARM64_ ## INST ## _ ## FIELD ## _MASK) >> ARM64_ ## INST ## _ ## FIELD ## _SHIFT

#define CASE_ARM64_OP(PC, INST) \
    GET_ARM64_BF(PC, INST, OP) == ARM64_ ## INST ## _OP

int parseInstBeta(uint32_t pc)
{
    GET_ARM64_BF(pc, MOVBI, OP);
    if (CASE_ARM64_OP(pc, MOVBI))
    {

    }
}

int grab_sinittext()
{
    int result = -1;
    instruction_t tempInst;
    SAFE_BAIL(parseInst(*binBegin, &tempInst) == -1);
    _sinittext_g = (uint32_t*)(tempInst.immediate + (size_t)binBegin);

    result = 0;
fail:
    return result;
}

int grab_primary_switch()
{
    int result = -1;
    instSet getB;
    opcOperandVar* primSwitchOff;
    uint32_t* primSwitchBAddr = 0;
    size_t tmpMath = 0;

    getB.insertInst_t<immediateVar>(ARM64_B_OP, NULL_INST, 0);
    primSwitchBAddr = getB.ffindRegexInst(_sinittext_g, PAGE_SIZE);
    SAFE_BAIL(primSwitchBAddr == 0);

    getB.varValueForKey(0, &primSwitchOff);
    tmpMath = *(size_t*)(primSwitchOff->getVal() + (size_t)primSwitchBAddr);
    __primary_switched_g = (uint32_t*)(tmpMath + (size_t)binBegin);
    
    result = 0;
fail:
    return result;
}

int grab_primary_switched()
{
    int result = -1;
    instSet getB;
    opcOperandVar* primSwitchedOff;
    uint32_t* primSwitchedLdrAddr = 0;

    getB.insertInst_t<immediateVar>(ARM64_LDR_OP, NULL_INST, 0);
    primSwitchedLdrAddr = getB.ffindRegexInst(__primary_switch_g, PAGE_SIZE);
    SAFE_BAIL(primSwitchedLdrAddr == 0);

    getB.varValueForKey(0, &primSwitchedOff);
    __primary_switch_g = (uint32_t*)(primSwitchedOff->getVal() + (size_t)primSwitchedLdrAddr);
    
    result = 0;
fail:
    return result;
}

int grab_start_kernel_g()
{
    int result = -1;
    instSet getB;
    opcOperandVar* startkernOff;
    uint32_t* startkernBAddr = 0;

    getB.insertInst_t<immediateVar>(ARM64_B_OP, NULL_INST, 0);
    startkernBAddr = getB.ffindRegexInst(__primary_switched_g, PAGE_SIZE);
    SAFE_BAIL(startkernBAddr == 0);

    getB.varValueForKey(0, &startkernOff);
    start_kernel_g = (uint32_t*)(startkernOff->getVal() + (size_t)startkernBAddr);
    
    result = 0;
fail:
    return result;
}

int main(int argc, char **argv)
{
    int flags = 0;
    int opt = 0;
    const char* kernName = 0;

    while ((opt = getopt(argc, argv, "k:")) != -1)
    {
        switch (opt)
        {
        case 'k':
            kernName = optarg;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-k kernelimage]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }


}