#include <stdint.h>
#include <stdarg.h>
#include <string.h>
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
    if (pcParsed->imm26 & 0x02000000)
    {
        pcParsed->immLarge = (pcParsed->immLarge | 0xfffffffffc000000);
    }
    return 0;
}

int parseByEnc(uint32_t pc, hde_t* instTemp)
{
    int result = -1;
    int lsl_value = 0;

    SAFE_BAIL(instTemp == 0);
    memset(instTemp, 0, sizeof(hde_t));

    // if ((((pc & ARM64_INSTCODE_MASK) >> ARM64_INSTCODE_SHIFT) & ARM64_LS_ENC_MASK) == ARM64_LS_ENC)

    if CASE_ARM64_ENC(pc, INSTCODE, LS_ENC)
    {
        // instTemp->encode = GET_ARM64_ENC(pc, INSTCODE, LS_ENC);
        instTemp->encode = E_LS;
        if ENCODE_FILTER(FT, pc, instTemp, LS, OP0, RI, RI)
        {
            lsl_value = 0;
            if ENCODE_FILTER(FT, pc, instTemp, LS, OP2, IMM, IMM)
            {
                if ENCODE_FILTER(FT, pc, instTemp, LS, OP3, LS, IMM)
                {
                    if (ENCODE_FILTER_NO_SET(pc, LS, OP4, IMM, FIX) || ENCODE_FILTER_NO_SET(pc, LS, OP4, IMM, UI))
                    {
                        instTemp->LS.OP4 = ENC_GET_FIELDTYPE(pc, LS, OP4, IMM);
                        parseRRImm9(pc, instTemp);
                    }
                }
            }
            else if ENCODE_FILTER(FT, pc, instTemp, LS, OP2, IMM, UIMM)
            {
                parseRRImm12(pc, instTemp);
            }
        }
        if ENCODE_FILTER(FT, pc, instTemp, LS, OP0, RI, RL)
        {
            lsl_value = 1;
            if ENCODE_FILTER(FT, pc, instTemp, LS, OP2, IMM, IMM)
            {
                parseImm19(pc, instTemp);
            }
        }
        instTemp->immLarge = instTemp->immLarge << (lsl_value + GET_ARM64_OP(pc, RRI_OPTSZ));
    }
    else if CASE_ARM64_ENC(pc, INSTCODE, BR_ENC)
    {
        // instTemp->encode = GET_ARM64_ENC(pc, INSTCODE, BR_ENC);
        instTemp->encode = E_BR;
        if ENCODE_FILTER(FG, pc, instTemp, BR, OP0, CBR, B)
        {
            parseImm26(pc, instTemp);
        }
        instTemp->immLarge <<= 2;
    }
    else if CASE_ARM64_ENC(pc, INSTCODE, DPIMM_ENC)
    {
        // instTemp->encode = GET_ARM64_ENC(pc, INSTCODE, DPIMM_ENC);
        instTemp->encode = E_DPIMM;
        // guaranteed adrp case, just calculate the final immediate right away.
        if ENCODE_FILTER(FT, pc, instTemp, DPIMM, OP0, PC, PC)
        {
            parseImm19(pc, instTemp);
            instTemp->immLarge = instTemp->imm19 << 14;
            instTemp->immLarge |= GET_ARM64_OP(pc, RI_IMMLO) << 12;
            if (pc & 0x00800000)
            {
                instTemp->immLarge = 0xFFFFFFE000000000 | instTemp->immLarge;
            }
        }
        else if ENCODE_FILTER(FT, pc, instTemp, DPIMM, OP0, GEN, LI)
        {
            parseLIinst(pc, instTemp);
        }        
    }
    else
    {
        goto fail;
    }

    result = 0;
fail:
    return result;
}

// uint32_t opSet(ENCODE_E encoding, int nargs, ...)
// {
//     va_list args;
//     HDEA64_OPCODE localOP;

//     va_start(args, nargs);
//     for (int i = 0; i < nargs; ++i)
//     {
//         localOP.ENCODING.OPNUM = va_arg(args, uint32_t);
//     }
//     va_end(args);
    
// }