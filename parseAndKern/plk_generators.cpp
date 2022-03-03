#include <string.h>
#include <stdlib.h>
#include <ibeSet.h>
#include <localUtil.h>
#include "spare_vmlinux.h"
#include "kern_static.h"

#ifdef LIVE_KERNEL
#include <hdeA64.h>
#else
#include <capstone/capstone.h>
#endif

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
int kern_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::gen_shstrtab(std::string** out_shstrtab, uint16_t* numSects, uint16_t* shstrtab_index)
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
void kern_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::insert_section(std::string sec_name, uint64_t sh_offset,
    uint64_t sh_size)
{
    Elf_Shdr* newShdr = 0;
    Elf_Phdr* newPhdr = 0;
    Elf_Word p_type = 0;
    Elf_Xword p_align = 0;
    Elf_Word p_flags = 0;

    uint16_t sh_type = SHT_PROGBITS;
    size_b sh_flags = 0;
    size_b sh_addr = sh_offset;
    // uint64_t sh_offset = 0;
    // uint64_t sh_size = 0;
    uint16_t sh_link = 0;
    uint16_t sh_info = 0;
    size_b sh_addralign = 0;
    size_b sh_entsize = 0;
    
    if (
        (sec_name != ".symtab") &&
        (sec_name != ".strtab") &&
        (sec_name != ".shstrtab")
        )
    {
        if (live_kernel == false)
        {
            sh_addr = R_KA(RESOLVE_REL(sh_addr));
        }
        sh_offset = RESOLVE_REL(sh_offset);
        p_type = PT_LOAD;
        p_align = 0x10000;

        sh_flags = SHF_ALLOC;
        p_flags = PF_R;
        // found a text, add executable flag
    }

    newShdr = new Elf_Shdr{
        0,
        sh_type,
        sh_flags,
        sh_addr,
        sh_offset,
        sh_size,
        sh_link,
        sh_info,
        sh_addralign,
        sh_entsize
        };

    sect_list.push_back({sec_name, newShdr});

    if (p_type == PT_LOAD)
    {
        newPhdr = new Elf_Phdr{
            p_type,
            p_flags,
            sh_offset,
            sh_addr,
            sh_addr,
            sh_size,
            sh_size,
            p_align
        };

        prog_list.push_back({sec_name, newPhdr});
    }
}
