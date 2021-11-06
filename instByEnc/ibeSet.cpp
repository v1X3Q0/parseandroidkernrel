#include "ibeSet.h"

int instSet::findPattern(uint32_t* startAddress, size_t sizeSearch, uint32_t* resultAddr)
{
    uint32_t* endAddress = (uint32_t*)((size_t)startAddress + sizeSearch);
    uint32_t* curAddr = startAddress;
    regexInst* tempInst;

    for (curAddr = startAddress; curAddr < endAddress; curAddr++)
    {
        regexInst::parseAndCreate(*curAddr, &tempInst);
    }
}