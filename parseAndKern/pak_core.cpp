#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>
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

int kern_img::parseAndGetGlobals()
{
    int result = -1;
    int snprintfInd = 0;
    uint32_t snprintfCrc = 0;

    SAFE_BAIL(grab_sinittext() == -1);

    if (live_kernel == false)
    {
        SAFE_BAIL(grab_primary_switch() == -1);
        SAFE_BAIL(grab_primary_switched() == -1);
        SAFE_BAIL(grab_start_kernel_g() == -1);
    }

    SAFE_BAIL(base_inits() == -1);
    if (live_kernel == false)
    {
        SAFE_BAIL(base_modverparam() == -1);
        SAFE_BAIL(base_ksymtab_strings() == -1);
        SAFE_BAIL(base_kcrctab() == -1);
        SAFE_BAIL(base_ksymtab() == -1);
        SAFE_BAIL(base_ex_table() == -1);
        SAFE_BAIL(base_new_shstrtab() == -1);
    }
    else if (live_kernel == true)
    {
        SAFE_BAIL(base_ksymtab_kcrctab_ksymtabstrings() == -1);
    }

    vector_pair_sort<std::string, Elf64_Shdr*>(&sect_list, cmp_Shdr);
    vector_pair_sort<std::string, Elf64_Phdr*>(&prog_list, cmp_Phdr);

    SAFE_BAIL(findKindInKstr("printk", &snprintfInd) == -1);
    snprintfCrc = get_kcrctab()[snprintfInd];

    result = 0;
fail:
    return result;
}

Elf64_Shdr* kern_img::find_sect(std::string lookupKey)
{
    return vector_pair_key_find<std::string, Elf64_Shdr*>(&sect_list, lookupKey);
}

Elf64_Phdr* kern_img::find_prog(std::string lookupKey)
{
    return vector_pair_key_find<std::string, Elf64_Phdr*>(&prog_list, lookupKey);
}

int kern_img::check_sect(std::string sect_name, Elf64_Shdr** sect_out)
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

int kern_img::findKindInKstr(const char* newString, int* index)
{
    const char* strIter = 0;
    Elf64_Shdr* ksymstrSec = 0;
    int result = -1;
    int i = 0;

    SAFE_BAIL(check_sect("__ksymtab_strings", &ksymstrSec) == -1);
    strIter = (const char*)UNRESOLVE_REL(ksymstrSec->sh_offset);

    for (; i < ksyms_count; i++)
    {
        if (strcmp(newString, strIter) == 0)
        {
            goto finish_eval;
        }
        if (*(uint16_t*)(strIter + strlen(strIter)) == 0)
        {
            break;
        }
        strIter = strIter + strlen(strIter) + 1;
    }

    goto fail;
finish_eval:
    *index = i;
finish:
    result = 0;
fail:
    return result;
}

