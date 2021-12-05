#ifndef PARSEANDKERN_H
#define PARSEANDKERN_H

#include <map>
#include <string>
#include <elf.h>
#include "spare_vmlinux.h"

class kern_img
{
public:
    kern_img(uint32_t* binBegin_a) : binBegin(binBegin_a) { parseAndGetGlobals(); };
    kern_img(const char* kern_file) { block_grab(kern_file, (void**)&binBegin, &kern_sz); };

    int findKindInKstr(const char* newString, int* index);
    int parseAndGetGlobals();
    void insert_section(std::string sec_name, uint16_t sh_type, uint64_t sh_flags,
        uint64_t sh_addr, uint64_t sh_offset, uint64_t sh_size, uint16_t sh_link,
        uint16_t sh_info, uint64_t sh_addralign, uint64_t sh_entsize);
    int check_sect(std::string sect_name, Elf64_Shdr** sect_out);
    int gen_vmlinux_sz(size_t* outSz, size_t headOffset);
    std::string gen_shstrtab();
    int patch_and_write(void* vmlinux_cur, size_t offset);

    size_t get_kernimg_sz() { return kern_sz; };
    kernel_symbol* get_ksymtab() { return (kernel_symbol*)sect_list["__ksymtab"].sh_addr; };
    size_t get_ksyms_count() { return ksyms_count; };
    uint32_t* get_kcrctab() { return (uint32_t*)sect_list["__kcrctab"].sh_addr; };
private:
    int grab_sinittext();
    int grab_primary_switch();
    int grab_primary_switched();
    int grab_start_kernel_g();

    int base_modverparam();
    int base_ksymtab_strings();
    int base_kcrctab();
    int base_ksymtab();
    int base_modver();
    int base_ex_table();
    int base_inits();

    std::map<std::string, Elf64_Shdr> sect_list;
    uint32_t* binBegin;
    size_t kern_sz;

    uint32_t* _sinittext;
    uint32_t* __primary_switch;
    uint32_t* __primary_switched;
    uint32_t* __create_page_tables;
    uint32_t* start_kernel;

    // size_t __modver;
    // size_t __param;
    // const char* __ksymtab_strings_g;
    // uint32_t* __kcrctab;
    // kernel_symbol* __ksymtab;
   
    size_t ksyms_count;
};

#endif