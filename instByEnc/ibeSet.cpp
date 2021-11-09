#include <vector>
#include <list>

#include <localUtil.h>
#include "ibeSet.h"
#include <opcOperand.h>


void instSet::addNewInst(cOperand* newInstruction)
{
    instPatternList.push_back(newInstruction);   
}

size_t** instSet::checkOperand(uint32_t newOperand)
{
    size_t** result = 0;
    auto it = varTable.begin();
    it = varTable.find(newOperand);
    if (it == varTable.end())
    {
        varTable[newOperand] = new size_t;
    }
    result = &varTable[newOperand];
    return result;
}

size_t instSet::getVar(uint32_t key)
{
    size_t value = 0;
    auto i = varTable.begin();

    i = varTable.find(key);
    SAFE_BAIL(i == varTable.end());

    value = *(i->second);
fail:
    return value;
}

bool equalInstSet(std::list<cOperand*>* list1, std::list<cOperand*>* list2)
{
    auto it1 = list1->begin();
    auto it2 = list2->begin();

    for (; it1 != list1->end(); it1++)
    {
        if ((*it1)->checkHelper((*it2)) == false)
        {
            return false;
        }
    }
    return true;
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

        SAFE_DEL(instSlide.front());
        instSlide.pop_front();
        instSlide.push_back(new cOperand(*slideEnd));
    }

fail:
    return result;
}
