#ifndef OPCOPERAND_H
#define OPCOPERAND_H

#include <stdint.h>
#include <hdeA64.h>
// #include <ibeSet.h>

#define CMPASSIGN_REG(LOCAL, TARGET, INDEX) \
    if (fixvar_set & e_ ## INDEX) \
    { \
        SAFE_BAIL(LOCAL.INDEX != TARGET->LOCAL.INDEX); \
    } \
    else \
    { \
        LOCAL.INDEX = TARGET->LOCAL.INDEX; \
        fixvar_set ^= e_  ## INDEX; \
    }

#define FIXVAR_ADD(INDEX, OPERAND) \
    OPERAND->fixvar_add(&OPERAND->parsedOpcode.INDEX, INDEX, e_ ## INDEX);
    // fixvar_add<typeof(hde_t::INDEX)>(&OPERAND->parsedOpcode.INDEX, INDEX, e_ ## INDEX);

class cOperand;

typedef struct
{
    val_set_t val_set;
    cOperand* regRand;
} saveVar_t;

class cOperand
{
private:
// if the variable has been fixed
    uint32_t fixvar_unmod;
    uint32_t fixvar_set;
    hde_t parsedOpcode;
public:
// initiate it to an initialized fixed variable, that's it.
    cOperand();
// constructor for fixed operands
    cOperand(uint32_t targOp);
    bool checkHelper(cOperand* targCompare);

    int getOpComp(val_set_t val_set, size_t* component);

    // case for adding a fixed size_t 
    // template <typename T>
    void fixvar_add(size_t* targetVar, size_t hde_member, val_set_t e_index);

    // case for adding a variable size_t, inwhich we just are adding a * to a **
    // template<typename T>
    void fixvar_add(size_t* targetVar, saveVar_t* var_member, val_set_t e_index);

    void clearVars();

    template <typename fv_rd, typename fv_rn, typename fv_imms, typename fv_immr, typename fv_immLarge>
    static cOperand* insertToGlob(uint64_t opcode, fv_rd rd, fv_rn rn, fv_imms imms, fv_immr immr, fv_immLarge immLarge)
    {
        cOperand* outOp = new cOperand;
        outOp->parsedOpcode.opcode = opcode;
        FIXVAR_ADD(rd, outOp);
        FIXVAR_ADD(rn, outOp);
        FIXVAR_ADD(imms, outOp);
        FIXVAR_ADD(immr, outOp);
        FIXVAR_ADD(immLarge, outOp);
        return outOp;
    }

    template <typename fv_immLarge>
    static cOperand* createB(fv_immLarge immLarge)
    {
        hdea64_opcode lop = {0};
        ENCODE_OP0_INST(lop, BR, B);

        return insertToGlob<size_t, size_t, size_t, size_t, fv_immLarge>(
            lop.opcode, 0, 0, 0, 0, immLarge);
    }

    template <typename fv_immLarge>
    static cOperand* createBL(fv_immLarge immLarge)
    {
        hdea64_opcode lop = {0};
        ENCODE_OP0_INST(lop, BR, BL);

        return insertToGlob<size_t, size_t, size_t, size_t, fv_immLarge>(
            lop.opcode, 0, 0, 0, 0, immLarge);
    }

    // create load register literal
    template <typename fv_rt, typename fv_imm19>
    static cOperand* createLDRL(fv_rt rt, fv_imm19 imm19)
    {
        hdea64_opcode lop = {0};
        ENCODE_OP2_INST(lop, LS, RL, NULL, IMM);

        return insertToGlob<fv_rt, size_t, size_t, size_t, fv_imm19>(
            lop.opcode, rt, 0, 0, 0, imm19);
    }

    template <typename fv_rd, typename fv_rn, typename fv_imm12>
    static cOperand* createASI(fv_rd rd, fv_rn rn, fv_imm12 imm12)
    {
        hdea64_opcode lop = {0};
        ENCODE_OP0_INST(lop, DPIMM, ASI);

        return insertToGlob<fv_rd, fv_rn, size_t, size_t, fv_imm12>(
            lop.opcode, rd, rn, 0, 0, imm12);
    }

    template <typename fv_rd, typename fv_immLarge>
    static cOperand* createADRP(fv_rd rd, fv_immLarge immLarge)
    {
        hdea64_opcode lop = {0};
        ENCODE_OP0_INST(lop, DPIMM, PC);

        return insertToGlob<fv_rd, size_t, size_t, size_t, fv_immLarge>(
            lop.opcode, rd, 0, 0, 0, immLarge);
    }

    // logical immediate, which in the end looks like a move
    template <typename fv_rd, typename fv_rn, typename fv_imms, typename fv_immr>
    static cOperand* createLI(fv_rd rd, fv_rn rn, fv_imms imms, fv_immr immr)
    {
        hdea64_opcode lop = {0};
        ENCODE_OP0_INST(lop, DPIMM, LI);

        return insertToGlob<fv_rd, fv_rn, fv_imms, fv_immr, size_t>(
            lop.opcode, rd, rn, imms, immr, 0);
    }

    // mov with a wide immediate
    template <typename fv_rd, typename fv_imm16>
    static cOperand* createMWI(fv_rd rd, fv_imm16 imm16)
    {
        hdea64_opcode lop = {0};
        ENCODE_OP0_INST(lop, DPIMM, MWI);

        return insertToGlob<fv_rd, size_t, size_t, size_t, imm16>(
            lop.opcode, rd, 0, 0, 0, imm16);
    }
};

#endif