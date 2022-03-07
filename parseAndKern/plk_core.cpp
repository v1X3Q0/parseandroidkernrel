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
Elf_Phdr* kernel_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::find_prog(std::string lookupKey)
{
    return vector_pair_key_find<std::string, Elf64_Phdr*>(&prog_list, lookupKey);
}

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
Elf_Shdr* kernel_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::find_sect(std::string lookupKey)
{
    return vector_pair_key_find<std::string, Elf_Shdr*>(&sect_list, lookupKey);
}

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
int kernel_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::check_sect(std::string sect_name, Elf_Shdr** sect_out)
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

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
int kernel_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::gen_shstrtab(std::string** out_shstrtab, uint16_t* numSects, uint16_t* shstrtab_index)
{
    std::string shstrtabTmp = "\0";
    uint16_t sect_iter = 0;
    uint16_t str_index = 0;
    
    for (auto i = sect_list.begin(); i != sect_list.end(); i++)
    {
        i->second->sh_name = shstrtabTmp.size();
        shstrtabTmp = shstrtabTmp + i->first + '\0';
        if (i->first == ".shstrtab")
        {
            // add 1 for the null section
            str_index = sect_iter + 1;
        }
        sect_iter++;
    }

finish:
    if (out_shstrtab != 0)
    {
        *out_shstrtab = new std::string(shstrtabTmp);
    }
    if (numSects != 0)
    {
        *numSects = sect_iter + 1;
    }
    if (shstrtab_index != 0)
    {
        *shstrtab_index = str_index;
    }

    return 0;
}

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
int kernel_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::base_new_shstrtab()
{
    char strtabRef[] = ".shstrtab";
    std::string* shtstrtab_tmp = 0;

    gen_shstrtab(&shtstrtab_tmp, NULL, NULL);
    insert_section(strtabRef, kern_sz, shtstrtab_tmp->size() + sizeof(strtabRef));
    return 0;
}

kernel_linux* allocate_static_kernel(const char* kern_filename, uint32_t bitness)
{
    kernel_linux* h = 0;

    if (bitness == 32)
    {
        h = kernel_block::allocate_kern_img<kernel_static<uint32_t, Elf32_Ehdr, Elf32_Shdr, Elf32_Phdr, Elf32_Xword, Elf32_Word>>(kern_filename);
    }
    else if (bitness == 64)
    {
        h = kernel_block::allocate_kern_img<kernel_static<uint64_t, Elf64_Ehdr, Elf64_Shdr, Elf64_Phdr, Elf64_Xword, Elf64_Word>>(kern_filename);
    }
    
    return h;
}

