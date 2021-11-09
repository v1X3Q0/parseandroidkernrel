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

    CMPASSIGN_REG(parsedOpcode, targCompare, rd);
    CMPASSIGN_REG(parsedOpcode, targCompare, rn);
    CMPASSIGN_REG(parsedOpcode, targCompare, imms);
    CMPASSIGN_REG(parsedOpcode, targCompare, immr);
    CMPASSIGN_REG(parsedOpcode, targCompare, immLarge);

    result = true;
fail:
    return false;
}

// case for adding a fixed size_t 
// template <typename T>
void cOperand::fixvar_add(size_t* targetVar, size_t hde_member, val_set_t e_index)
{
    *targetVar = (size_t)hde_member;
    fixvar_set |= e_index;
}

// case for adding a variable size_t, inwhich we just are adding a * to a **
// template<typename T>
void cOperand::fixvar_add(size_t* targetVar, size_t** var_member, val_set_t e_index)
{
    *var_member = targetVar;
    fixvar_set &= ~e_index;
}

