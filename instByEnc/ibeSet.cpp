#include <vector>
#include <list>

#include <localUtil.h>
#include "ibeSet.h"
#include <opcOperand.h>

opcOperandVar* instSet::getVar(uint32_t key)
{
    opcOperandVar* value = 0;
    auto i = varTable.begin();

    i = varTable.find(key);
    SAFE_BAIL(i == varTable.end());

    value = i->second;
fail:
    return value;
}

opcOperandVar* instSet::useKey(uint32_t key)
{
    opcOperandVar* value = 0;
    auto i = varTable.begin();

    i = varTable.find(key);
    if (i == varTable.end())
    {
        value = new opcOperandVar(key);
    }
    else
    {
        value = varTable[key];
    }

fail:
    return value;
}

int instSet::findPattern(uint32_t* startAddress, size_t sizeSearch, uint32_t** resultAddr)
{
    int result = -1;
    uint32_t* endAddress = (uint32_t*)((size_t)startAddress + sizeSearch);
    uint32_t* curAddr = startAddress;
    uint32_t* slideEnd = curAddr + instPatternList.size();
    regexInst<registerFix, registerFix, immediateFix, immediateFix, immediateFix>* tempInst;
    std::list<regexInst<registerFix, registerFix, immediateFix, immediateFix, immediateFix>*> instSlide;

    for (int i = 0; i < instPatternList.size(); curAddr++, i++)
    {
        regexInst<registerFix, registerFix, immediateFix, immediateFix, immediateFix>::parseAndCreate(
            *curAddr, &tempInst);
        instSlide.push_back(tempInst);
    }

    for (curAddr = startAddress; slideEnd < endAddress; curAddr++, slideEnd++)
    {
        if (*this == instSlide)
        {
            *resultAddr = curAddr;
            result = 0;
            break;
        }
        
        instSlide.pop_front();
        regexInst<registerFix, registerFix, immediateFix, immediateFix, immediateFix>::parseAndCreate(
            *curAddr, &tempInst);
        instSlide.push_back(tempInst);
    }

fail:
    return result;
}
