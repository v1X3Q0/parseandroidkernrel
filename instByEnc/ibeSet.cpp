#include <vector>
#include <list>

#include <localUtil.h>
#include "ibeSet.h"
#include <opcOperand.h>


void instSet::addNewInst(cOperand* newInstruction)
{
    instPatternList.push_back(newInstruction);   
}

void instSet::clearInstructions()
{
    for (int i = 0; i < instPatternList.size(); i++)
    {
        SAFE_DEL(instPatternList.front());
        instPatternList.pop_front();
    }
}

saveVar_t* instSet::checkOperand(uint32_t newOperand)
{
    saveVar_t* result = 0;
    auto it = varTable.begin();
    it = varTable.find(newOperand);
    if (it == varTable.end())
    {
        result = (saveVar_t*)calloc(1, sizeof(saveVar_t));
        varTable[newOperand] = result;
    }
    result = varTable[newOperand];
    return result;
}

saveVar_t* instSet::addOperand(uint32_t newOperand, val_set_t val_set, cOperand* regRand)
{
    saveVar_t* result = 0;
    auto it = varTable.begin();
    it = varTable.find(newOperand);
    if (it == varTable.end())
    {
        result = (saveVar_t*)calloc(1, sizeof(saveVar_t));
        result->val_set = val_set;
        result->regRand = regRand;
        varTable[newOperand] = result;
    }
    result = varTable[newOperand];
    return result;
}

int instSet::getVar(uint32_t key, size_t* value)
{
    int result = -1;
    auto i = varTable.begin();

    SAFE_BAIL(value == 0);
    i = varTable.find(key);
    SAFE_BAIL(i == varTable.end());

    SAFE_BAIL(i->second->regRand->getOpComp(i->second->val_set, value) == -1);

    result = 0;
fail:
    return result;
}

bool equalInstSet(std::list<cOperand*>* list1, std::list<cOperand*>* list2)
{
    auto it1 = list1->begin();
    auto it2 = list2->begin();

    for (; it1 != list1->end(); it1++, it2++)
    {
        if ((*it1)->checkHelper((*it2)) == false)
        {
            return false;
        }
    }
    return true;
}

void instSet::clearVars()
{
    for (auto i = varTable.begin(); i != varTable.end(); i++)
    {
        i->second->regRand->clearVars();
    }
}

int instSet::findPattern(uint32_t* startAddress, size_t sizeSearch, uint32_t** resultAddr)
{
    int result = -1;
    uint32_t* endAddress = (uint32_t*)((size_t)startAddress + sizeSearch);
    uint32_t* curAddr = startAddress;
    uint32_t* slideEnd = curAddr + instPatternList.size();
    std::list<cOperand*> instSlide;

    for (int i = 0; i < instPatternList.size(); curAddr++, i++)
    {
        instSlide.push_back(new cOperand(*curAddr));
    }

    for (curAddr = startAddress; slideEnd < endAddress; curAddr++, slideEnd++)
    {
        if (equalInstSet(&instPatternList, &instSlide) == true)
        {
            result = 0;
            *resultAddr = curAddr;
            break;
        }

        clearVars();
        SAFE_DEL(instSlide.front());
        instSlide.pop_front();
        instSlide.push_back(new cOperand(*slideEnd));
    }

    for (int i = 0; i < instSlide.size(); i++)
    {
        SAFE_DEL(instSlide.front());
        instSlide.pop_front();
    }

fail:
    return result;
}
