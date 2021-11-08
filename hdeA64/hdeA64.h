#ifndef HDEA64_H
#define HDEA64_H

#include <stdint.h>
#include <stdio.h>

#define ARM64_IMMR_MASK      0x003f0000
#define ARM64_IMMR_SHIFT     16
#define ARM64_IMMS_MASK      0x0000fc00
#define ARM64_IMMS_SHIFT     10

#define ARM64_IMM9_MASK         0x001ff000
#define ARM64_IMM9_SHIFT        12
#define ARM64_IMM12_MASK        0x003ffc00
#define ARM64_IMM12_SHIFT       10
#define ARM64_IMM19_MASK        0x00ffffe0
#define ARM64_IMM19_SHIFT       5
#define ARM64_IMM26_MASK        0x00ffffff
#define ARM64_IMM26_SHIFT       0
#define ARM64_RT_MASK           0x0000001f
#define ARM64_RT_SHIFT          0
#define ARM64_RD_MASK           ARM64_RT_MASK
#define ARM64_RD_SHIFT          ARM64_RT_SHIFT
#define ARM64_RN_MASK           0x000003e0
#define ARM64_RN_SHIFT          5

#define ARM64_RI_IMMLO_MASK     0x60000000
#define ARM64_RI_IMMLO_SHIFT    29
#define ARM64_RRI_OPTSZ_MASK    0x40000000
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

// data processing immediate group
#define ARM64_DPIMM_OP0_MASK    0x03800000
#define ARM64_DPIMM_OP0_SHIFT   23

#define ARM64_DPIMM_OP0_PC_MASK 0x6
#define ARM64_DPIMM_OP0_PC      0x0
#define ARM64_DPIMM_OP0_GEN_MASK 0x7
#define ARM64_DPIMM_OP0_LI      0x4

// branch operations group
#define ARM64_BR_OP0_MASK       0xe00000000
#define ARM64_BR_OP0_SHIFT      29
// conditional branch without flags
#define ARM64_BR_OP0_CBR_MASK   0x3
#define ARM64_BR_OP0_B          0x0
#define ARM64_BR_OP1_MASK       0x03fff000
#define ARM64_BR_OP1_SHIFT      12
#define ARM64_BR_OP2_MASK       0x0000001f
#define ARM64_BR_OP2_SHIFT      0

// load store operations group
#define ARM64_LS_OP0_MASK       0xf0000000
#define ARM64_LS_OP0_SHIFT      28
#define ARM64_LS_OP0_RI_MASK    0x3
#define ARM64_LS_OP0_RR         0x2
#define ARM64_LS_OP0_RI         0x3

#define ARM64_LS_OP1_MASK       0x04000000
#define ARM64_LS_OP1_SHIFT      26

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

#define ENC_GET_FIELDGROUP(CUR_INST, ENC, FIELDGROUP) \
    ((pc & ARM64_ ## ENC ## _ ## FIELDGROUP ## _MASK) >> ARM64_ ## ENC ## _ ## FIELDGROUP ## _SHIFT)

#define ENC_GET_FIELDTYPE(CUR_INST, ENC, FIELDGROUP, FIELDTYPE) \
    (ENC_GET_FIELDGROUP(CUR_INST, ENC, FIELDGROUP) & \
    ARM64_ ## ENC ## _ ## FIELDGROUP ## _ ## FIELDTYPE ## _MASK)

#define ENCODE_FILTER_SET(CUR_INST, DESTINST, ENC, FIELDGROUP, FIELDTYPE) \
    DESTINST->ENC.FIELDGROUP = ENC_GET_FIELDTYPE(CUR_INST, ENC, FIELDGROUP, FIELDTYPE)

#define ENCODE_FILTER(CUR_INST, DESTINST, ENC, FIELDGROUP, FIELDTYPE, VALUE) \
    (ENCODE_FILTER_SET(CUINST, DESTINST, ENC, FIELDGROUP, FIELDTYPE) == ARM64_ ## ENC ## _ ## FIELDGROUP ## _ ## VALUE)

#define ENCODE_FILTER_NO_SET(CUR_INST, ENC, FIELDGROUP, FIELDTYPE, VALUE) \
    (ENC_GET_FIELDTYPE(CUR_INST, ENC, FIELDGROUP, FIELDTYPE) == ARM64_ ## ENC ## _ ## FIELDGROUP ## _ ## VALUE)

#define GET_ARM64_OP(CUR_INST, PC_PART) \
    (((size_t)CUR_INST & ARM64_ ## PC_PART ## _MASK) >> ARM64_ ## PC_PART ## _SHIFT)

#define CASE_ARM64_IND(CUR_INST, ENCGRP, OP_TYPE, INDEX) \
    (GET_ARM64_OP(CUR_INST, ENCGRP ## _ ## OP_TYPE) == ARM64_ ## ENCGRP ## _ ## OP_TYPE ## _ ## INDEX)

#define CASE_ARM64_OP(CUR_INST, PC_PART, OP_TYPE) \
    CASE_ARM64_IND(CUR_INST, PC_PART, OP_TYPE, OP)

#define GET_ARM64_ENC(CUR_INST, PC_PART, ENC) \
    (((size_t)CUR_INST & ARM64_ ## PC_PART ## _MASK) >> ARM64_ ## PC_PART ## _SHIFT) & ARM64_ ## ENC ## _MASK

#define CASE_ARM64_ENC(CUR_INST, PC_PART, OP_TYPE) \
(GET_ARM64_ENC(CUR_INST, PC_PART, OP_TYPE) == ARM64_ ## OP_TYPE)


#define SAFE_BAIL(x) \
if (x) \
{ \
    goto fail; \
}

typedef enum
{
    RD=1,
    RN=1 << 1,
    IMMS=1 << 2,
    IMMR=1 << 3,
    IMMLARGE=1 << 4
} val_set_t;

//      dpimm   br      ls      dpr     dps
// enc  100x    101x    x1x0    x101    x111
// op0  3       3       4              
// op1          14      1
// op2          5       2
// op3                  6
// op4                  2

typedef struct
{
    uint32_t VAL_SET;
    union
    {
        uint32_t opcode;
        struct
        {
            unsigned int encode : 4;
            union
            {
                struct
                {
                    unsigned int OP0 : 3;
                } DPIMM;
                struct
                {
                    unsigned int OP0 : 3;
                    unsigned int OP1 : 14;
                    unsigned int OP2 : 5;
                } BR;
                struct
                {
                    unsigned int OP0 : 4;
                    unsigned int OP1 : 1;
                    unsigned int OP2 : 2;
                    unsigned int OP3 : 6;
                    unsigned int OP4 : 2;
                } LS;            
            };
        };
    };
    union
    {
        uint8_t rd;
        uint8_t rt;
    };
    uint8_t rn;
    uint8_t imms;
    uint8_t immr;
    union
    {
        uint16_t imm9;
        uint64_t imm12;
        uint32_t imm19;
        uint32_t imm26;
        ssize_t immLarge;
    };
} hde_t;

int parseByEnc(uint32_t pc, hde_t* instTemp);
#define parseInst parseByEnc

#endif