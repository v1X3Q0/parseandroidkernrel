#ifndef PARSEANDKERN_H
#define PARSEANDKERN_H

#pragma once

#include <map>
#include <vector>
#include <string>
#include <elf.h>
#include <string.h>
#include <iostream>
#include <dlfcn.h>

#include <localUtil.h>
// #include <ibeSet.h>
#include <drv_share.h>
#include <localUtil_cpp.h>

// #include <jsmn/jsmn.h>
// #include <json.h>
// #include <jsonUtil.h>

#include "spare_vmlinux.h"
#include <kern_img.h>

// template <typename Elf_Shdr>
// bool cmp_Shdr(std::pair<std::string, Elf_Shdr*>& a,
//          std::pair<std::string, Elf_Shdr*>& b);

// template <typename Elf_Phdr>
// bool cmp_Phdr(std::pair<std::string, Elf_Phdr*>& a,
//          std::pair<std::string, Elf_Phdr*>& b);

template <typename Elf_Shdr>
bool cmp_Shdr(std::pair<std::string, Elf_Shdr*>& a,
         std::pair<std::string, Elf_Shdr*>& b)
{
    return a.second->sh_offset < b.second->sh_offset;
}

template <typename Elf_Phdr>
bool cmp_Phdr(std::pair<std::string, Elf_Phdr*>& a,
         std::pair<std::string, Elf_Phdr*>& b)
{
    return a.second->p_offset < b.second->p_offset;
}

#define ELFBIT     template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr, \
            typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>

#define TEMPLIFY(FUNC, ...) \
    { \
        if (bitness == 64) \
        { \
            return FUNC ## _p<uint64_t, Elf64_Ehdr, Elf64_Shdr, Elf64_Phdr, Elf64_Xword, Elf64_Word>(__VA_ARGS__); \
        } \
        else \
        { \
            return FUNC ## _p<uint32_t, Elf32_Ehdr, Elf32_Shdr, Elf32_Phdr, Elf32_Xword, Elf32_Word>(__VA_ARGS__); \
        } \
    }


class kern_static : public kernel_linux
{
public:
    // have to extend these, they are for every kernel. the ksym
    // dlsym is for getting the kernel symbol location, really
    // extending that is just in case a symbol can't or shouldn't
    // be cached, for instance if a symbol table has been found.
    int ksym_dlsym(const char *newString, uint64_t *out_address)
    {
        int result = -1;
        uint64_t symtmp = 0;
        named_kmap_t *mh_base = 0;
        std::map<std::string, uint64_t>::iterator findres;

        // if we have found it dynamically it will be here
        FINISH_IF(kern_sym_fetch(newString, &symtmp) == 0);
        FINISH_IF(kstruct_offset(newString, &symtmp) == 0);

        // if we have not found it dynamically, then check kallsyms cache
        // findres = kallsym_cache.find(newString);
        // SAFE_BAIL(findres == kallsym_cache.end());
        // symtmp = findres->second;

        goto fail;
    finish:
        result = 0;
        if (out_address != 0)
        {
            *out_address = symtmp;
        }
    fail:
        return result;
    }

    // get the index of a kstr in the ksymstr table.
    int findKindInKstr(const char *newString, int *index);
    int gen_shstrtab(std::string **out_shstrtab, uint16_t *numSects, uint16_t *shstrtab_index);

    // wrappers
    int gen_vmlinux_sz(size_t *outSz, size_t headOffset);
    void elfConstruction();
    void elfHeadConstruction(void* elfHead);
    int patch_and_write(void *vmlinux_base, void *vmlinux_cur, void *phBase, size_t offset);
    int parseAndGetGlobals();
    void insert_section(std::string sec_name, uint64_t sh_offset, uint64_t sh_size);
    void insert_elfsegment(std::string sec_name, int permissions, uint64_t Virtual, uint64_t Physical, uint64_t FileOffset,  uint64_t sh_size);
    void insert_elfsection(std::string sec_name, uint64_t Virtual, uint64_t FileOffset,  uint64_t sh_size);
    int kcrc_index(std::string symbol, uint32_t* kcrc);
private:
    // private constructors for internal use only
    using kernel_linux::kernel_linux;
    int populate_kcrc_map();

    ELFBIT
    void elfHeadConstruction_p(void* elfHead);
    // patch the out binary, section table base at vmlinux_cur and ph base at phBase
    // int patch_and_write(Elf_Ehdr* vmlinux_base, Elf_Shdr* vmlinux_cur, Elf_Phdr* phBase, size_t offset);
    ELFBIT
    int patch_and_write_p(void *vmlinux_base, void *vmlinux_cur, void *phBase, size_t offset);
    // couple of helpers for finding stuff
    void *find_prog(std::string lookupKey);
    void *find_sect(std::string lookupKey);
    int check_sect(std::string sect_name, void* sect_out);

    ELFBIT
    int gen_vmlinux_sz_p(size_t *outSz, size_t headOffset);
    ELFBIT
    void elfConstruction_p();
    ELFBIT
    int parseAndGetGlobals_p();
    // insertion function to be used, should be the only interface for adding new values
    // void insert_section(std::string sec_name, uint16_t sh_type, uint64_t sh_flags,
    //     uint64_t sh_addr, uint64_t sh_offset, uint64_t sh_size, uint16_t sh_link,
    //     uint16_t sh_info, uint64_t sh_addralign, uint64_t sh_entsize);
    // void insert_section(std::string sec_name, uint64_t sh_offset, uint64_t sh_size);
    ELFBIT
    void insert_section_p(std::string sec_name, uint64_t sh_offset, uint64_t sh_size);
    ELFBIT
    void insert_elfsection_p(std::string sec_name, uint64_t Virtual, uint64_t FileOffset,  uint64_t sh_size);
    ELFBIT
    void insert_elfsegment_p(std::string sec_name, int permissions, uint64_t Virtual, uint64_t Physical, uint64_t FileOffset,  uint64_t sh_size);
    ELFBIT
    int kcrc_index_p(std::string symbol, uint32_t* kcrc);
    ELFBIT
    int gen_shstrtab_p(std::string **out_shstrtab, uint16_t *numSects, uint16_t *shstrtab_index);

    // finding sections in the binary
    int base_new_shstrtab();

    // section list, second arg is an Elf_Shdr*
    std::vector<std::pair<std::string, void*>> sect_list;
    std::vector<const char *> kstrtab_sorted;
    std::vector<const char *> kstrtab_gpl_sorted;

    // program header list, second arg is an Elf_Phdr*
    std::vector<std::pair<std::string, void*>> prog_list;
};

#include "kern_static.hpp"

#endif