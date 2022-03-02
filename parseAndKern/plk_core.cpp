#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>

#include <ibeSet.h>
#include <localUtil.h>
#include <localUtil_cpp.h>
#include <kern_img.h>
#include <kernel_block.h>

#include "spare_vmlinux.h"
#include "kern_static.h"

#ifdef LIVE_KERNEL
#include <hdeA64.h>
#else
#include <capstone/capstone.h>
#endif

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

    // parse the json and determine which sorters/generators i need to run

    vector_pair_sort<std::string, Elf64_Shdr*>(&sect_list, cmp_Shdr);
    vector_pair_sort<std::string, Elf64_Phdr*>(&prog_list, cmp_Phdr);

    result = 0;
fail:
    return result;
}

Elf64_Phdr* kern_static::find_prog(std::string lookupKey)
{
    return vector_pair_key_find<std::string, Elf64_Phdr*>(&prog_list, lookupKey);
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

