#ifndef PATCHCRC_H
#define PATCHCRC_H

#include <stdio.h>
#include <string>
#include <elf.h>
#include <map>
#include <vector>
#include <parseAndKern.h>

int findSec(char *elfBase, const char* sectionName, Elf64_Shdr** secHeadFound);
int specSection(char* elfBase, const char* sectionName, void** sectionBase, size_t* sectionSize);
int patchVersion(char *elfBase, std::map<std::string, unsigned long>* crcPairs);
int populateVers(char *elfBase, std::map<std::string, unsigned long>* crcPairs);
int populateCrcKsymtab(std::map<std::string, unsigned long>* crcPairs,
    kernel_symbol* ksymBase, size_t ksymCount, uint32_t* kcrcBase);
int populateKcrctab_ki(std::map<std::string, unsigned long>* crcPairs, kern_static* parsedKI);
int populateCrcMap(std::map<std::string, unsigned long>* crcPairs, std::map<std::string, unsigned long>* crcNet);
int symvers_modules(std::vector<std::string>* vendor_modules, std::map<std::string, unsigned long>* crcNet);

#endif