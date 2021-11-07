#ifndef OPCOPERAND_H
#define OPCOPERAND_H

#include <stdint.h>

class opcOperand
{
protected:
    uint32_t value;
public:
    uint32_t getValue();
    virtual bool verifyValue(opcOperand* targCompare) = 0;
    bool checkHelper(opcOperand* targCompare);
};

class opcOperandFix : protected opcOperand
{
public:
    opcOperandFix(uint32_t value_a) { value = value_a; };
    bool verifyValue(opcOperand* targCompare);
};

class opcOperandVar : protected opcOperand
{
private:
    uint32_t key;
    bool initialized;
public:
    opcOperandVar(uint32_t key_a) : key(key_a) { };
    bool verifyValue(opcOperand* targCompare);
};

typedef opcOperandFix registerFix;
typedef opcOperandFix immediateFix;
typedef opcOperandVar registerVar;
typedef opcOperandVar immediateVar;

#endif