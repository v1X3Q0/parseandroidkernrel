#ifndef PATCHCRC_H
#define PATCHCRC_H

#include <stdio.h>
#include <string>
#include <elf.h>
#include <map>
#include "parseAndKern.h"

int findSec(char *elfBase, const char* sectionName, Elf64_Shdr** secHeadFound);
int specSection(char* elfBase, const char* sectionName, void** sectionBase, size_t* sectionSize);
int patchVersion(char *elfBase, std::map<std::string, unsigned long>* crcPairs);
int grabElfFile(const char* fileTargName, void** allocBase, size_t* fSize);
int populateVers(char *elfBase, std::map<std::string, unsigned long>* crcPairs);
int populateCrc(std::map<std::string, unsigned long>* crcPairs, kernel_symbol* ksymBase, size_t ksymCount, uint32_t* kcrcBase);

#endif