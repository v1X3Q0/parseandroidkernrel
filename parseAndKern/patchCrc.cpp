#include <elf.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>

#include <localUtil.h>
#include "parseAndKern.h"

int findSec(char *elfBase, const char* sectionName, Elf64_Shdr** secHeadFound)
{
    int result = -1;
    Elf64_Ehdr *foundHeader = (Elf64_Ehdr *)elfBase;
    Elf64_Shdr *foundSecTab = 0;
    const char *strTab = 0;

    SAFE_BAIL(*(uint32_t*)elfBase != 0x464c457f);
    foundSecTab = (Elf64_Shdr *)(foundHeader->e_shoff + elfBase);
    strTab = (const char *)(foundSecTab[foundHeader->e_shstrndx].sh_offset + elfBase);
    
    for (int i = 0; i < foundHeader->e_shnum; i++)
    {
        if (strcmp(strTab + foundSecTab[i].sh_name, sectionName) == 0)
        {
            *secHeadFound = &foundSecTab[i];
            result = 0;
            break;
        }
    }
    result = 0;
fail:
    return result;
}

int specSection(char* elfBase, const char* sectionName, void** sectionBase, size_t* sectionSize)
{
    int result = -1;
    Elf64_Shdr* modversTmp = 0;

    SAFE_BAIL(findSec(elfBase, sectionName, &modversTmp) == -1);
    if (sectionBase != 0)
    {
        *sectionBase = modversTmp->sh_offset + elfBase;
    }
    if (sectionSize != 0)
    {
        *sectionSize = modversTmp->sh_size;
    }
    result = 0;
fail:
    return result;
}

int patchVersion(char *elfBase, std::map<std::string, unsigned long>* crcPairs)
{
    int result = -1;
    size_t verSize = 0;
    modversion_info *versBase = 0;
    modversion_info *versIter = 0;
    Elf64_Shdr* shdrVers = 0;
    auto i = crcPairs->begin();

    SAFE_BAIL(specSection(elfBase, "__versions", (void**)&versBase, &verSize) == -1);

    for (; i != crcPairs->end(); i++)
    {
        versIter = versBase;
        for (int j = 0; j < verSize; j += sizeof(modversion_info), versIter++)
        {
            if (strcmp(versIter->name, i->first.data()) == 0)
            {
                versIter->crc = i->second;
            }
        }
    }

    result = -1;
fail:
    return result;
}

int populateVers(char *elfBase, std::map<std::string, unsigned long>* crcPairs)
{
    int result = -1;
    size_t verSize = 0;
    modversion_info *versBase = 0;
    modversion_info *versIter = 0;
    Elf64_Shdr* shdrVers = 0;

    SAFE_BAIL(specSection(elfBase, "__versions", (void**)&versBase, &verSize) == -1);

    versIter = versBase;
    for (int j = 0; j < verSize; j += sizeof(modversion_info), versIter++)
    {
        (*crcPairs)[std::string(versIter->name)] = versIter->crc;
    }
    result = 0;
fail:
    return result;
}

int populateCrcKsymtab(std::map<std::string, unsigned long>* crcPairs,
    kernel_symbol* ksymBase, size_t ksymCount, uint32_t* kcrcBase)
{
    int result = -1;
    int resultTmp = -1;

    for (auto i = crcPairs->begin(); i != crcPairs->end(); i++)
    {
        resultTmp = -1;
        for (size_t j = 0; j < ksymCount; j++)
        {
            if(strcmp(i->first.data(), ksymBase[j].name) == 0)
            {
                i->second = kcrcBase[j];
                resultTmp = 0;
            }
        }
        SAFE_BAIL(resultTmp == -1);
    }

    result = 0;
fail:
    return result;
}

int populateCrcMap(std::map<std::string, unsigned long>* crcPairs, std::map<std::string, unsigned long>* crcNet)
{
    for (auto i = crcPairs->begin(); i != crcPairs->end(); i++)
    {
        if (crcNet->find(i->first) == crcNet->end())
        {
            printf("WARNING: symbol %s not found\n", i->first.data());
        }
        else
        {
            (*crcPairs)[i->first] = i->second;
        }
    }
    return 0;
}

int grabElfFile(const char* fileTargName, void** allocBase, size_t* fSize)
{
    int result = -1;
    FILE* outFile = 0;
    size_t outfileSz = 0;
    size_t outfileSzPad = 0;

    outFile = fopen(fileTargName, "r");
    SAFE_BAIL(outFile == 0);
    
    fseek(outFile, 0, SEEK_END);
    outfileSz = ftell(outFile);
    fseek(outFile, 0, SEEK_SET);

    if ((outfileSz % PAGE_SIZE4K) != 0)
    {
        outfileSzPad = (outfileSz + PAGE_SIZE4K) & ~PAGE_MASK4K;
    }

    posix_memalign(allocBase, PAGE_SIZE4K, outfileSzPad);

    fread(*allocBase, 1, outfileSz, outFile);

    if (fSize != 0)
    {
        *fSize = outfileSz;
    }

    result = 0;
fail:
    SAFE_FCLOSE(outFile);
    return result;
}

int symvers_modules(std::vector<std::string>* vendor_modules, std::map<std::string, unsigned long>* crcNet)
{
    int result = -1;
    char* tmpModBase = 0;

    for (auto i = vendor_modules->begin(); i != vendor_modules->end(); i++)
    {
        SAFE_CONT(grabElfFile(i->data(), (void**)&tmpModBase, 0) == -1);
        populateVers(tmpModBase, crcNet);
        SAFE_FREE(tmpModBase);
    }

    result = 0;
fail:
    return result;
}