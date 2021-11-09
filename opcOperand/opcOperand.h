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

class cOperand
{
private:
// if the variable has been fixed
    uint32_t fixvar_set;
    hde_t parsedOpcode;
public:
// initiate it to an initialized fixed variable, that's it.
    cOperand() : fixvar_set(e_rd | e_rn | e_imms | e_immr | e_immLarge) {};
// constructor for fixed operands
    cOperand(uint32_t targOp);
    bool checkHelper(cOperand* targCompare);

    // case for adding a fixed size_t 
    // template <typename T>
    void fixvar_add(size_t* targetVar, size_t hde_member, val_set_t e_index);

    // case for adding a variable size_t, inwhich we just are adding a * to a **
    // template<typename T>
    void fixvar_add(size_t* targetVar, size_t** var_member, val_set_t e_index);

    template <typename fv_rd, typename fv_rn, typename fv_imms, typename fv_immr, typename fv_immLarge>
    static cOperand* insertToGlob(uint32_t opcode, fv_rd rd, fv_rn rn, fv_imms imms, fv_immr immr, fv_immLarge immLarge)
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
};

#endif