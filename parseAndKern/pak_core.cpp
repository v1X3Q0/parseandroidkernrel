#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>
#include <localUtil_cpp.h>
#include <kern_img.h>
#include <kernel_block.h>

#include <sv_gpl.h>

#include "spare_vmlinux.h"
#include "parseAndKern.h"

bool cmp_Shdr(std::pair<std::string, Elf64_Shdr*>& a,
         std::pair<std::string, Elf64_Shdr*>& b)
{
    return a.second->sh_offset < b.second->sh_offset;
}

bool cmp_Phdr(std::pair<std::string, Elf64_Phdr*>& a,
         std::pair<std::string, Elf64_Phdr*>& b)
{
    return a.second->p_offset < b.second->p_offset;
}

bool strcmp_bool(const char* first, const char* second)
{
    if (strcmp(first, second) == 0)
        return true;
    return false;
}

int kern_static::parseAndGetGlobals()
{
    int result = -1;
    int snprintfInd = 0;
    uint32_t snprintfCrc = 0;

    SAFE_BAIL(grab_sinittext() == -1);
    SAFE_BAIL(grab_primary_switch() == -1);
    SAFE_BAIL(grab_primary_switched() == -1);
    SAFE_BAIL(grab_start_kernel_g() == -1);

    SAFE_BAIL(base_inits() == -1);
    SAFE_BAIL(base_modverparam() == -1);
    SAFE_BAIL(base_ksymtab_strings() == -1);
    SAFE_BAIL(base_kcrctab() == -1);
    SAFE_BAIL(base_ksymtab() == -1);
    SAFE_BAIL(base_ex_table() == -1);
    SAFE_BAIL(base_new_shstrtab() == -1);

    SAFE_BAIL(populate_kcrc_map() == -1);

    vector_pair_sort<std::string, Elf64_Shdr*>(&sect_list, cmp_Shdr);
    vector_pair_sort<std::string, Elf64_Phdr*>(&prog_list, cmp_Phdr);

    // SAFE_BAIL(findKindInKstr("printk", &snprintfInd) == -1);
    // snprintfCrc = get_kcrctab()[snprintfInd];

    result = 0;
fail:
    return result;
}

Elf64_Phdr* kern_static::find_prog(std::string lookupKey)
{
    return vector_pair_key_find<std::string, Elf64_Phdr*>(&prog_list, lookupKey);
}

int kern_static::populate_kcrc_map()
{
    int result = -1;
    Elf64_Shdr* kstrtab_sec = 0;
    const char* kstr_iter = 0;

    SAFE_BAIL(check_sect("__ksymtab_string", &kstrtab_sec) == -1);
    kstr_iter = (const char*)UNRESOLVE_REL(kstrtab_sec->sh_offset);

    for (int i = 0; i < ksyms_count + 5; i++)
    {
        SAFE_PAIL(ksym_map.find(kstr_iter) == ksym_map.end(), "couldn't find the symbol %s in our sym repo\n", kstr_iter)
        if (ksym_map[kstr_iter] == NONGPL_SYM)
        {
            kstrtab_sorted.push_back(kstr_iter);
        }
        else
        {
            kstrtab_gpl_sorted.push_back(kstr_iter);
        }
        kstr_iter = strlen(kstr_iter) + kstr_iter + 1;
        if (*(uint16_t*)(kstr_iter - 1) == 0)
        {
            break;
        }
    }

    std::sort(kstrtab_sorted.begin(), kstrtab_sorted.end(), strcmp_bool);    
    std::sort(kstrtab_gpl_sorted.begin(), kstrtab_gpl_sorted.end(), strcmp_bool);


    result = 0;
fail:
    return result;
}

int kern_static::kcrc_index(std::string symbol, uint32_t* kcrc)
{
    int result = -1;
    Elf64_Shdr* kcrctab_sec = 0;
    uint32_t* kcrctab_base = 0;
    SAFE_BAIL(check_sect("__kcrctab", &kcrctab_sec) == -1);

    kcrctab_base = (uint32_t*)UNRESOLVE_REL(kcrctab_sec->sh_offset);

    for (int i = 0; i < kstrtab_sorted.size(); i++)
    {
        if (symbol == kstrtab_sorted[i])
        {
            *kcrc = kcrctab_base[i];
            goto finish;
        }
    }
    for (int i = 0; i < kstrtab_gpl_sorted.size(); i++)
    {
        if (symbol == kstrtab_gpl_sorted[i])
        {
            *kcrc = kcrctab_base[kstrtab_sorted.size() + i];
            goto finish;
        }
    }
    goto fail;

finish:
    result = 0;
fail:
    return result;
}

Elf64_Shdr* kern_static::find_sect(std::string lookupKey)
{
    return vector_pair_key_find<std::string, Elf64_Shdr*>(&sect_list, lookupKey);
}

int kern_static::check_sect(std::string sect_name, Elf64_Shdr** sect_out)
{
    int result = -1;
    int index = 0;

    index = vector_pair_ind<std::string, Elf64_Shdr*>(&sect_list, sect_name);
    SAFE_BAIL(index == -1);
    // SAFE_BAIL(sect_list.find(sect_name) == sect_list.end());

    if (sect_out != 0)
    {
        // *sect_out = &sect_list[sect_name];
        *sect_out = sect_list[index].second;
    }

    result = 0;
fail:
    return result;
}

