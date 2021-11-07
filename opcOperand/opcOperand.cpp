#include "opcOperand.h"

bool opcOperand::checkHelper(opcOperand* targCompare)
{
    if (getValue() != targCompare->getValue())
    {
        return false;
    }
    return true;
}

bool opcOperandFix::verifyValue(opcOperand* targCompare)
{
    return checkHelper(targCompare);
}

bool opcOperandVar::verifyValue(opcOperand* targCompare)
{
    if (initialized == true)
    {
        return checkHelper(targCompare);
    }
    else
    {
        value = targCompare->getValue();
        initialized = true;
    }
    return true;
}
