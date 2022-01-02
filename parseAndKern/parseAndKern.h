#ifndef PARSEANDKERN_H
#define PARSEANDKERN_H

#include <map>
#include <vector>
#include <string>
#include <elf.h>

#include <localUtil.h>
#include <ibeSet.h>
#include <drv_share.h>
#include "spare_vmlinux.h"
#include <kern_img.h>

class kern_static : public kern_img
{
public:
    // couple of helpers for finding stuff
    Elf64_Phdr* find_prog(std::string lookupKey);

    // have to extend these, they are for every kernel. the ksym
    // dlsym is for getting the kernel symbol location, really
    // extending that is just in case a symbol can't or shouldn't
    // be cached, for instance if a symbol table has been found.
    int ksym_dlsym(const char* newString, size_t* out_address);
    int parseAndGetGlobals();
    
    // get the index of a kstr in the ksymstr table.
    int findKindInKstr(const char* newString, int* index);

    // insertion function to be used, should be the only interface for adding new values
    // void insert_section(std::string sec_name, uint16_t sh_type, uint64_t sh_flags,
    //     uint64_t sh_addr, uint64_t sh_offset, uint64_t sh_size, uint16_t sh_link,
    //     uint16_t sh_info, uint64_t sh_addralign, uint64_t sh_entsize);
    void insert_section(std::string sec_name, uint64_t sh_offset, uint64_t sh_size);

    int gen_vmlinux_sz(size_t* outSz, size_t headOffset);
    int gen_shstrtab(std::string** out_shstrtab, uint16_t* numSects, uint16_t* shstrtab_index);

    // patch the out binary, section table base at vmlinux_cur and ph base at phBase
    int patch_and_write(Elf64_Ehdr* vmlinux_base, Elf64_Shdr* vmlinux_cur, Elf64_Phdr* phBase, size_t offset);

    int kcrc_index(std::string symbol, uint32_t* kcrc);

private:
    using kern_img::kern_img;
    int populate_kcrc_map();

    // grabbing global variables for use
    int grab_primary_switch();
    int grab_primary_switched();
    int grab_start_kernel_g();

    int grab_task_struct_offs();

    // finding sections in the binary
    int base_modverparam();
    int base_ksymtab_strings();
    int base_kcrctab();
    int base_modver();
    int base_ex_table();
    int base_new_shstrtab();
    int base_init_data();

    std::vector<const char*> kstrtab_sorted;
    std::vector<const char*> kstrtab_gpl_sorted;

    std::vector<std::pair<std::string, Elf64_Phdr*>> prog_list;
};

#endif