#ifndef IBESET_H
#define IBESET_H

#include <stdarg.h>
#include <list>
#include <map>

#include "hdeA64.h"
#include <localUtil.h>

#include <opcOperand.h>

class instSet
{
private:
    std::map<uint32_t, size_t*> varTable;
    std::list<cOperand*> instPatternList;
public:
    size_t** checkOperand(uint32_t newOperand);
    void addNewInst(cOperand* newInstruction);
    int findPattern(uint32_t* startAddress, size_t sizeSearch, uint32_t** resultAddr);
    size_t getVar(uint32_t key);
    // opcOperandVar* useKey(uint32_t key);
};

#endif