#include <stdint.h>
#include "hdeA64.h"

int parseLIinst(uint32_t pc, hde_t* instTemp)
{
    instTemp->immr = GET_ARM64_OP(pc, IMMR);
    instTemp->imms = GET_ARM64_OP(pc, IMMS);
    instTemp->rd = GET_ARM64_OP(pc, RT);
    instTemp->rn = GET_ARM64_OP(pc, RN);
    return 0;
}

// load store immediate with 9 bit immediate, a 5 bit source reg and 5 bit dest reg
int parseRRImm9(uint32_t pc, hde_t* instTemp)
{
    instTemp->imm9 = GET_ARM64_OP(pc, IMM9);
    instTemp->rn = GET_ARM64_OP(pc, RN);
    instTemp->rt = GET_ARM64_OP(pc, RT);
    return 0;
}

int parseRRImm12(uint32_t pc, hde_t* instTemp)
{
    instTemp->imm12 = GET_ARM64_OP(pc, IMM12);
    instTemp->rn = GET_ARM64_OP(pc, RN);
    instTemp->rt = GET_ARM64_OP(pc, RT);
    return 0;
}

int parseImm19(uint32_t pc, hde_t* pcParsed)
{
    pcParsed->imm19 = GET_ARM64_OP(pc, IMM19);
    pcParsed->rd = GET_ARM64_OP(pc, RD);
    return 0;
}

int parseImm26(uint32_t pc, hde_t* pcParsed)
{
    pcParsed->imm26 = GET_ARM64_OP(pc, IMM26);
    return 0;
}

int parseByEnc(uint32_t pc, hde_t* instTemp)
{
    int result = -1;

    if CASE_ARM64_ENC(pc, INSTCODE, LS_ENC)
    {
        if ENCODE_FILTER(pc, instTemp, LS, OP0, RI, RI)
        {
            if ENCODE_FILTER(pc, instTemp, LS, OP2, IMM, IMM)
            {
                if ENCODE_FILTER(pc, instTemp, LS, OP3, LS, IMM)
                {
                    if (ENCODE_FILTER_NO_SET(pc, LS, OP4, IMM, FIX) || ENCODE_FILTER_NO_SET(pc, LS, OP4, IMM, UI))
                    {
                        instTemp->LS.OP4 = ENC_GET_FIELDTYPE(pc, LS, OP4, IMM);
                        parseRRImm9(pc, instTemp);
                    }
                }
            }
            else if ENCODE_FILTER(pc, instTemp, LS, OP2, IMM, UIMM)
            {
                parseRRImm12(pc, instTemp);
            }
            instTemp->immLarge = instTemp->immLarge << (2 + GET_ARM64_OP(pc, RRI_OPTSZ));
        }
    }
    else if CASE_ARM64_ENC(pc, INSTCODE, BR_ENC)
    {
        instTemp->encode = GET_ARM64_ENC(pc, INSTCODE, BR_ENC);
        if ENCODE_FILTER(pc, instTemp, BR, OP0, CBR, B)
        {
            parseImm26(pc, instTemp);
        }
    }
    else if CASE_ARM64_ENC(pc, INSTCODE, DPIMM_ENC)
    {
        instTemp->encode = GET_ARM64_ENC(pc, INSTCODE, DPIMM_ENC);
        // guaranteed adrp case, just calculate the final immediate right away.
        if ENCODE_FILTER(pc, instTemp, DPIMM, OP0, PC, PC)
        {
            parseImm19(pc, instTemp);
            instTemp->immLarge = instTemp->imm19 << 14;
            instTemp->immLarge |= GET_ARM64_OP(pc, RI_IMMLO) << 12;
            if (pc & 0x00800000)
            {
                instTemp->immLarge = 0xFFFFFFE000000000 | instTemp->immLarge;
            }
        }
        else if ENCODE_FILTER(pc, instTemp, DPIMM, OP0, GEN, LI)
        {
            parseLIinst(pc, instTemp);
        }        
    }

fail:
    return result;
}
