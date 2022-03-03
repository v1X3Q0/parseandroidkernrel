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

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
int kern_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::parseAndGetGlobals()
{
    int result = -1;

    // parse the json and determine which sorters/generators i need to run

    vector_pair_sort<std::string, Elf_Shdr*>(&sect_list, cmp_Shdr<Elf_Shdr>);
    vector_pair_sort<std::string, Elf_Phdr*>(&prog_list, cmp_Phdr<Elf_Phdr>);

    result = 0;
fail:
    return result;
}

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
Elf_Phdr* kern_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::find_prog(std::string lookupKey)
{
    return vector_pair_key_find<std::string, Elf64_Phdr*>(&prog_list, lookupKey);
}

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
Elf_Shdr* kern_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::find_sect(std::string lookupKey)
{
    return vector_pair_key_find<std::string, Elf_Shdr*>(&sect_list, lookupKey);
}

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
int kern_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::check_sect(std::string sect_name, Elf_Shdr** sect_out)
{
    int result = -1;
    int index = 0;

    index = vector_pair_ind<std::string, Elf_Shdr*>(&sect_list, sect_name);
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

kern_img* allocate_static_kernel(const char* kern_filename, uint32_t bitness)
{
    kern_img* new_stat = 0;

    if (bitness == 32)
    {
        new_stat = kernel_block::allocate_kern_img<kern_static<uint32_t, Elf32_Ehdr, Elf32_Shdr, Elf32_Phdr, Elf32_Xword, Elf32_Word>>(kern_filename);
    }
    else if (bitness == 64)
    {
        new_stat = kernel_block::allocate_kern_img<kern_static<uint64_t, Elf64_Ehdr, Elf64_Shdr, Elf64_Phdr, Elf64_Xword, Elf64_Word>>(kern_filename);
    }
    return new_stat;
}
