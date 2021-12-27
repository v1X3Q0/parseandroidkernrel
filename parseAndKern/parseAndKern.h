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

#define RESOLVE_REL(x) \
    ((size_t)x - (size_t)binBegin)

#define R_KA(x) \
    ((size_t)x + (size_t)ANDROID_KERNBASE)

#define UNRESOLVE_REL(x) \
    ((size_t)x + (size_t)binBegin)

#define SAFE_LIVE_FREE(x) \
    if (live_kernel == true) \
    { \
        SAFE_FREE(x); \
    }

class kern_img
{
public:
    // to be used for actual construction
    static kern_img* allocate_kern_img(const char* kern_file)
    {
        kern_img* result = 0;
        void* binBegin = 0;
        size_t kernSz = 0;

        SAFE_BAIL(block_grab(kern_file, &binBegin, &kernSz) == -1);

        result = new kern_img((uint32_t*)binBegin, kernSz);
        SAFE_BAIL(result->parseAndGetGlobals() == -1);

        goto finish;
    fail:
        SAFE_DEL(result);
    finish:
        return result;
    };

    static kern_img* grab_live_kernel(void* kern_base)
    {
        kern_img* result = 0;

        result = new kern_img((uint32_t*)kern_base);
        SAFE_BAIL(result->parseAndGetGlobals() == -1);

        goto finish;
    fail:
        SAFE_DEL(result);
    finish:
        return result;
    }

    // couple of helpers for finding stuff
    Elf64_Phdr* find_prog(std::string lookupKey);
    Elf64_Shdr* find_sect(std::string lookupKey);

    size_t resolveRel(size_t rebase);
    int findKindInKstr(const char* newString, int* index);
    int parseAndGetGlobals();

    // under the condition that we have a live kernel, translation routine.
    int live_kern_addr(void* target_kernel_address, size_t size_kernel_buf, void** out_live_addr);
    int kernel_search(instSet* getB, void* img_var, size_t img_var_sz, uint32_t** out_img_off);
    int kernel_search_seq(void* img_var, size_t img_var_sz, uint8_t* byte_search, size_t search_sz,
        size_t offset, size_t step, bool match, void** out_img_off);
    
    // insertion function to be used, should be the only interface for adding new values
    void insert_section(std::string sec_name, uint16_t sh_type, uint64_t sh_flags,
        uint64_t sh_addr, uint64_t sh_offset, uint64_t sh_size, uint16_t sh_link,
        uint16_t sh_info, uint64_t sh_addralign, uint64_t sh_entsize);
    int check_sect(std::string sect_name, Elf64_Shdr** sect_out);
    int gen_vmlinux_sz(size_t* outSz, size_t headOffset);
    int gen_shstrtab(std::string** out_shstrtab, uint16_t* numSects, uint16_t* shstrtab_index);

    // patch the out binary, section table base at vmlinux_cur and ph base at phBase
    int patch_and_write(Elf64_Ehdr* vmlinux_base, Elf64_Shdr* vmlinux_cur, Elf64_Phdr* phBase, size_t offset);

    size_t get_kernimg_sz() { return kern_sz; };
    uint32_t* get_binbegin() { return binBegin; };
    kernel_symbol* get_ksymtab() { return (kernel_symbol*)UNRESOLVE_REL(find_sect("__ksymtab")->sh_offset); };
    size_t get_ksyms_count() { return ksyms_count; };
    uint32_t* get_kcrctab() { return (uint32_t*)UNRESOLVE_REL(find_sect("__kcrctab")->sh_offset); };
private:
    bool live_kernel;
    // private constructors for internal use only
    kern_img(uint32_t* binBegin_a) : binBegin(binBegin_a), live_kernel(true) {};
    kern_img(uint32_t* binBegin_a, size_t kern_sz_a) : binBegin(binBegin_a), kern_sz(kern_sz_a), live_kernel(false) {};
    kern_img(const char* kern_file);

    // grabbing global variables for use
    int grab_sinittext();
    int grab_primary_switch();
    int grab_primary_switched();
    int grab_start_kernel_g();

    // finding sections in the binary
    int base_modverparam();
    int base_ksymtab_strings();
    int base_kcrctab();
    int base_ksymtab();
    int base_modver();
    int base_ex_table();
    int base_inits();
    int base_new_shstrtab();
    int base_init_data();

    int base_ksymtab_kcrctab_ksymtabstrings();

    std::vector<std::pair<std::string, Elf64_Shdr*>> sect_list;
    std::vector<std::pair<std::string, Elf64_Phdr*>> prog_list;
    // beginning of file list
    uint32_t* binBegin;
    // input file size
    size_t kern_sz;

    // tracked globals
    uint32_t* _sinittext;
    uint32_t* __primary_switch;
    uint32_t* __primary_switched;
    uint32_t* __create_page_tables;
    uint32_t* start_kernel;
   
    size_t ksyms_count;
};

#endif