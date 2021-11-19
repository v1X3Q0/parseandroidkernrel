#ifndef HDEA64_H
#define HDEA64_H

#include <stdint.h>
#include <stdio.h>

#define X0 0
#define X1 1
#define X2 2
#define X3 3
#define X4 4
#define X5 5
#define X6 6
#define X7 7
#define X8 8
#define X9 9
#define X10 10
#define X11 11
#define X12 12
#define X13 13
#define X14 14
#define X15 15
#define X16 16
#define X17 17
#define X18 18
#define X19 19
#define X20 20
#define X21 21
#define X22 22
#define X23 23
#define X24 24
#define X25 25
#define X26 26
#define X27 27
#define X28 28
#define X29 29
#define X30 30
#define X31 31
#define SP X31

#define ARM64_IMMR_MASK      0x003f0000
#define ARM64_IMMR_SHIFT     16
#define ARM64_IMMS_MASK      0x0000fc00
#define ARM64_IMMS_SHIFT     10

#define ARM64_IMM9_MASK         0x001ff000
#define ARM64_IMM9_SHIFT        12
#define ARM64_IMM12_MASK        0x003ffc00
#define ARM64_IMM12_SHIFT       10
#define ARM64_IMM16_MASK        0x001fffe0
#define ARM64_IMM16_SHIFT       5
#define ARM64_IMM19_MASK        0x00ffffe0
#define ARM64_IMM19_SHIFT       5
#define ARM64_IMM26_MASK        0x03ffffff
#define ARM64_IMM26_SHIFT       0
#define ARM64_RT_MASK           0x0000001f
#define ARM64_RT_SHIFT          0
#define ARM64_RD_MASK           ARM64_RT_MASK
#define ARM64_RD_SHIFT          ARM64_RT_SHIFT
#define ARM64_RN_MASK           0x000003e0
#define ARM64_RN_SHIFT          5
#define ARM64_RM_MASK           0x001f0000
#define ARM64_RM_SHIFT          16

#define ARM64_RI_IMMLO_MASK     0x60000000
#define ARM64_RI_IMMLO_SHIFT    29
#define ARM64_RRI_OPTSZ_MASK    0xc0000000
#define ARM64_RRI_OPTSZ_SHIFT   30

#define ARM64_INSTCODE_MASK     0x1e000000
#define ARM64_INSTCODE_SHIFT    25

// inst encode masks
#define ARM64_BR_ENC_MASK       0xe
#define ARM64_BR_ENC            0xa

#define ARM64_LS_ENC_MASK       0x5
#define ARM64_LS_ENC            0x4

#define ARM64_DPIMM_ENC_MASK    0xe
#define ARM64_DPIMM_ENC         0x8

#define ARM64_DPREG_ENC_MASK    0x7
#define ARM64_DPREG_ENC         0x5

#define ARM64_DPREG_OP0_MASK    0x40000000
#define ARM64_DPREG_OP0_SHIFT   30
#define ARM64_DPREG_OP0_DPNS_MASK   0x1
#define ARM64_DPREG_OP0_2SRC    0x0
#define ARM64_DPREG_OP0_1SRC    0x1

#define ARM64_DPREG_OP1_MASK    0x10000000
#define ARM64_DPREG_OP1_SHIFT   28
// shift or extend instructions have a 0, the rest are 1
#define ARM64_DPREG_OP1_GEN_MASK    0x1
#define ARM64_DPREG_OP1_SER     0x0
#define ARM64_DPREG_OP1_NSER    0x1

#define ARM64_DPREG_OP2_MASK    0x01e00000
#define ARM64_DPREG_OP2_SHIFT   21
#define ARM64_DPREG_OP2_GEN_MASK    0xf
// these are paired with the generic data processing instructions
#define ARM64_DPREG_OP2_DP      0x6
// noticed that the instructions covered here are arithmetic,
// they just don't seem dependent on the register that the
// shift or extend instructions, SER, did
#define ARM64_DPREG_OP2_ANS     0x0
#define ARM64_DPREG_OP2_LSR_MASK    0x8
#define ARM64_DPREG_OP2_LSR     0x0
#define ARM64_DPREG_OP2_ASSE_MASK   0x9
#define ARM64_DPREG_OP2_ASS     0x8
#define ARM64_DPREG_OP2_ASE     0x9

#define ARM64_DPREG_OP3_MASK    0x0000fc00
#define ARM64_DPREG_OP3_SHIFT   10
#define ARM64_DPREG_OP3_GEN_MASK    0x3f
#define ARM64_DPREG_OP3_ASC     0x0

// data processing immediate group
#define ARM64_DPIMM_OP0_MASK    0x03800000
#define ARM64_DPIMM_OP0_SHIFT   23

#define ARM64_DPIMM_OP0_PC_MASK 0x6
// pc relative address, adrp
#define ARM64_DPIMM_OP0_PC      0x0
#define ARM64_DPIMM_OP0_ADRP    0x1
#define ARM64_DPIMM_OP0_GEN_MASK 0x7
// logical immediate
#define ARM64_DPIMM_OP0_LI      0x4
// add subtract immediate
#define ARM64_DPIMM_OP0_ASI     0x2
// move wide immediate
#define ARM64_DPIMM_OP0_MWI     0x5

// branch operations group
#define ARM64_BR_OP0_MASK       0xe0000000
#define ARM64_BR_OP0_SHIFT      29
// conditional branch without flags
#define ARM64_BR_OP0_CBR_MASK   0x3
#define ARM64_BR_OP0_B          0x0
#define ARM64_BR_OP0_BL         0x4
#define ARM64_BR_OP0_ETC_MASK   0x7
#define ARM64_BR_OP0_SYS        0x6

#define ARM64_BR_OP1_MASK       0x03fff000
#define ARM64_BR_OP1_SHIFT      12
#define ARM64_BR_OP1_ETC_MASK   0x3fff
#define ARM64_BR_OP1_BAR        0x1033

#define ARM64_BR_OP2_MASK       0x0000001f
#define ARM64_BR_OP2_SHIFT      0

// load store operations group
#define ARM64_LS_OP0_MASK       0xf0000000
#define ARM64_LS_OP0_SHIFT      28
#define ARM64_LS_OP0_RI_MASK    0x3
#define ARM64_LS_OP0_RR         0x2
#define ARM64_LS_OP0_RI         0x3
#define ARM64_LS_OP0_RL         0x1

#define ARM64_LS_OP1_MASK       0x04000000
#define ARM64_LS_OP1_SHIFT      26
#define ARM64_LS_OP1_NULL       0

#define ARM64_LS_OP2_MASK       0x01800000
#define ARM64_LS_OP2_SHIFT      23
#define ARM64_LS_OP2_IMM_MASK   0x2
#define ARM64_LS_OP2_IMM        0x0
#define ARM64_LS_OP2_UIMM       0x2

#define ARM64_LS_OP3_MASK       0x003f0000
#define ARM64_LS_OP3_SHIFT      16
#define ARM64_LS_OP3_LS_MASK    0x20
#define ARM64_LS_OP3_IMM        0x00


#define ARM64_LS_OP4_MASK       0x00000c00
#define ARM64_LS_OP4_SHIFT      10
#define ARM64_LS_OP4_IMM_MASK   0x3
#define ARM64_LS_OP4_PRE        0x3
#define ARM64_LS_OP4_POST       0x1
#define ARM64_LS_OP4_FIX        0x1
#define ARM64_LS_OP4_UI         0x0

#define ARM64_NOP_OP        0xd503201f
#define ARM64_ISB_OP        0xd5033fdf

#define ENC_GET_FIELDGROUP(CUR_INST, ENC, FIELDGROUP) \
    ((pc & ARM64_ ## ENC ## _ ## FIELDGROUP ## _MASK) >> ARM64_ ## ENC ## _ ## FIELDGROUP ## _SHIFT)

#define ENC_GET_FIELDGROUP_SET(CUR_INST, DESTINST, ENC, FIELDGROUP) \
    (DESTINST->ENC.FIELDGROUP = ((pc & ARM64_ ## ENC ## _ ## FIELDGROUP ## _MASK) >> ARM64_ ## ENC ## _ ## FIELDGROUP ## _SHIFT))

#define ENC_GET_FIELDTYPE(CUR_INST, ENC, FIELDGROUP, FIELDTYPE) \
    (ENC_GET_FIELDGROUP(CUR_INST, ENC, FIELDGROUP) & ARM64_ ## ENC ## _ ## FIELDGROUP ## _ ## FIELDTYPE ## _MASK)

#define ENC_GET_FIELDTYPE_SET(CUR_INST, DESTINST, ENC, FIELDGROUP, FIELDTYPE) \
    (ENC_GET_FIELDGROUP_SET(CUR_INST, DESTINST, ENC, FIELDGROUP) & ARM64_ ## ENC ## _ ## FIELDGROUP ## _ ## FIELDTYPE ## _MASK)

#define ENCODE_FILTER_SETFT(CUR_INST, DESTINST, ENC, FIELDGROUP, FIELDTYPE) \
    (DESTINST->ENC.FIELDGROUP = ENC_GET_FIELDTYPE(CUR_INST, ENC, FIELDGROUP, FIELDTYPE))

#define ENCODE_FILTER_SETFG(CUR_INST, DESTINST, ENC, FIELDGROUP, FIELDTYPE) \
    ENC_GET_FIELDTYPE_SET(CUR_INST, DESTINST, ENC, FIELDGROUP, FIELDTYPE)

#define ENCODE_FILTER(FGT, CUR_INST, DESTINST, ENC, FIELDGROUP, FIELDTYPE, VALUE) \
    (ENCODE_FILTER_SET ## FGT(CUINST, DESTINST, ENC, FIELDGROUP, FIELDTYPE) == ARM64_ ## ENC ## _ ## FIELDGROUP ## _ ## VALUE)

#define ENCODE_FILTER_NO_SET(CUR_INST, ENC, FIELDGROUP, FIELDTYPE, VALUE) \
    (ENC_GET_FIELDTYPE(CUR_INST, ENC, FIELDGROUP, FIELDTYPE) == ARM64_ ## ENC ## _ ## FIELDGROUP ## _ ## VALUE)

#define GET_ARM64_OP(CUR_INST, PC_PART) \
    (((size_t)CUR_INST & ARM64_ ## PC_PART ## _MASK) >> ARM64_ ## PC_PART ## _SHIFT)

#define CASE_ARM64_IND(CUR_INST, ENCGRP, OP_TYPE, INDEX) \
    (GET_ARM64_OP(CUR_INST, ENCGRP ## _ ## OP_TYPE) == ARM64_ ## ENCGRP ## _ ## OP_TYPE ## _ ## INDEX)

#define CASE_ARM64_OP(CUR_INST, PC_PART, OP_TYPE) \
    CASE_ARM64_IND(CUR_INST, PC_PART, OP_TYPE, OP)

#define GET_ARM64_ENC(CUR_INST, PC_PART, ENC) \
    (((CUR_INST & ARM64_ ## PC_PART ## _MASK) >> ARM64_ ## PC_PART ## _SHIFT) & ARM64_ ## ENC ## _MASK)

#define CASE_ARM64_ENC(CUR_INST, PC_PART, OP_TYPE) \
    (GET_ARM64_ENC(CUR_INST, PC_PART, OP_TYPE) == ARM64_ ## OP_TYPE)


#define SAFE_BAIL(x) \
if (x) \
{ \
    goto fail; \
}

#define SAFE_DEL(x) \
    if (x) \
    { \
        delete x; \
        x = 0; \
    }

typedef enum
{
    e_rd=1,
    e_rn=1 << 1,
    e_imms=1 << 2,
    e_immr=1 << 3,
    e_immLarge=1 << 4
} val_set_t;

//      dpimm   br      ls      dpr     dps
// enc  100x    101x    x1x0    x101    x111
// op0  3       3       4              
// op1          14      1
// op2          5       2
// op3                  6
// op4                  2

#ifdef DYN_DISASM
#define UINT8_SZT size_t
#define UINT16_SZT size_t
#define UINT32_SZT size_t
#define SSZT_SZT size_t
#else
#define UINT8_SZT uint8_t
#define UINT16_SZT uint16_t
#define UINT32_SZT uint32_t
#define SSZT_SZT ssize_t
#endif

typedef enum
{
    E_UNCLASS=0,
    E_BR=1,
    E_LS,
    E_DPIMM,
    E_DPREG,
} ENCODE_E;

// NOTE:
// i TRIED to get away with using ENCODE_E encode : 4;, but it seems as though
// even when i try to bitfield my enumberation, it still gets casted to a 
// uint32_t. so just uint32_t and bitfielding will allow me to at least use the 
// size and preserve the value.

#define HDEA64_OPCODE \
    union \
    { \
        uint64_t opcode; \
        struct \
        { \
            ENCODE_E encode; \
            union \
            { \
                struct \
                { \
                    uint32_t OP0 : 3; \
                } DPIMM; \
                struct \
                { \
                    uint32_t OP0 : 1; \
                    uint32_t OP1 : 1; \
                    uint32_t OP2 : 4; \
                    uint32_t OP3 : 6; \
                } DPREG; \
                struct \
                { \
                    uint32_t OP0 : 3; \
                    uint32_t OP1 : 14; \
                    uint32_t OP2 : 5; \
                } BR; \
                struct \
                { \
                    uint32_t OP0 : 4; \
                    uint32_t OP1 : 1; \
                    uint32_t OP2 : 2; \
                    uint32_t OP3 : 6; \
                    uint32_t OP4 : 2; \
                } LS; \
                uint32_t unclass_val; \
            }; \
        }; \
    }

typedef HDEA64_OPCODE __attribute__ ((aligned (1))) __attribute__((packed, aligned(1))) hdea64_opcode;

typedef struct
{
    uint32_t VAL_SET;
    HDEA64_OPCODE;
    union
    {
        UINT8_SZT rd;
        UINT8_SZT rt;
    };
    UINT8_SZT rn;
    UINT8_SZT rm;
    union
    {
        UINT8_SZT imms;
        UINT8_SZT imm6;
    };
    
    UINT8_SZT immr;
    union
    {
        UINT16_SZT imm9;
        UINT16_SZT imm12;
        UINT16_SZT imm16;
        UINT32_SZT imm19;
        UINT32_SZT imm26;
        SSZT_SZT immLarge;
    };
} __attribute__ ((aligned (1))) __attribute__((packed, aligned(1))) hdeA64_t;

#define hde_t hdeA64_t

int parseByEnc(uint32_t pc, hde_t* instTemp);
#define parseInst parseByEnc

uint32_t opSet(ENCODE_E encoding, int nargs, ...);

#define ENCODE_OP0_INST(DSTOP, ENCODE, OP_0) \
    DSTOP.encode = E_ ## ENCODE; \
    DSTOP.ENCODE.OP0 = ARM64_ ## ENCODE ## _OP0_ ## OP_0;

#define ENCODE_OP4_INST(DSTOP, ENCODE, OP_0, OP_1, OP_2, OP_3, OP_4) \
    DSTOP.encode = E_ ## ENCODE; \
    DSTOP.ENCODE.OP0 = ARM64_ ## ENCODE ## _OP0_ ## OP_0; \
    DSTOP.ENCODE.OP1 = ARM64_ ## ENCODE ## _OP1_ ## OP_1; \
    DSTOP.ENCODE.OP2 = ARM64_ ## ENCODE ## _OP2_ ## OP_2; \
    DSTOP.ENCODE.OP3 = ARM64_ ## ENCODE ## _OP3_ ## OP_3; \
    DSTOP.ENCODE.OP4 = ARM64_ ## ENCODE ## _OP4_ ## OP_4;

#define ENCODE_OP2_INST(DSTOP, ENCODE, OP_0, OP_1, OP_2) \
    DSTOP.encode = E_ ## ENCODE; \
    DSTOP.ENCODE.OP0 = ARM64_ ## ENCODE ## _OP0_ ## OP_0; \
    DSTOP.ENCODE.OP1 = ARM64_ ## ENCODE ## _OP1_ ## OP_1; \
    DSTOP.ENCODE.OP2 = ARM64_ ## ENCODE ## _OP2_ ## OP_2;

#endif