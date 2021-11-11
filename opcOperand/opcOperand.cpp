#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "opcOperand.h"
#include <hdeA64.h>

cOperand::cOperand(uint32_t targOp)
{
    parseInst(targOp, &parsedOpcode);
}

bool cOperand::checkHelper(cOperand* targCompare)
{
    bool result = false;

    SAFE_BAIL(parsedOpcode.opcode != targCompare->parsedOpcode.opcode);
    CMPASSIGN_REG(parsedOpcode, targCompare, rd);
    CMPASSIGN_REG(parsedOpcode, targCompare, rn);
    CMPASSIGN_REG(parsedOpcode, targCompare, imms);
    CMPASSIGN_REG(parsedOpcode, targCompare, immr);
    CMPASSIGN_REG(parsedOpcode, targCompare, immLarge);

    result = true;
fail:
    return result;
}

void cOperand::clearVars()
{
    fixvar_set = fixvar_unmod;
}

int cOperand::getOpComp(val_set_t val_set, size_t* component)
{
#define EACH_CASE(INDEX) \
    case e_ ## INDEX: \
        *component = parsedOpcode.INDEX; \
        break;

    int result = -1;

    switch (val_set)
    {
    EACH_CASE(rd);
    EACH_CASE(rn);
    EACH_CASE(imms);
    EACH_CASE(immr);
    EACH_CASE(immLarge);
    default:
        goto fail;
    }
    result = 0;
fail:
    return result;
}

// case for adding a fixed size_t 
// template <typename T>
void cOperand::fixvar_add(size_t* targetVar, size_t hde_member, val_set_t e_index)
{
    *targetVar = (size_t)hde_member;
    fixvar_set |= e_index;
    fixvar_unmod = fixvar_set;
}

// case for adding a variable size_t, inwhich we just are adding a * to a **
// template<typename T>
void cOperand::fixvar_add(size_t* targetVar, saveVar_t* var_member, val_set_t e_index)
{
    var_member->regRand = this;
    var_member->val_set = e_index;
    fixvar_set &= ~e_index;
    fixvar_unmod = fixvar_set;
}

