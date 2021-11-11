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
    std::map<uint32_t, saveVar_t*> varTable;
    std::list<cOperand*> instPatternList;
public:
    saveVar_t* checkOperand(uint32_t newOperand);
    saveVar_t* addOperand(uint32_t newOperand, val_set_t val_set, cOperand* regRand);
    void addNewInst(cOperand* newInstruction);
    int findPattern(uint32_t* startAddress, size_t sizeSearch, uint32_t** resultAddr);
    int getVar(uint32_t key, size_t* value);
    void clearVars();
    // opcOperandVar* useKey(uint32_t key);
};

#endif