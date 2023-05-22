#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>

// #include <ibeSet.h>
#include <localUtil.h>
#include <localUtil_cpp.h>
#include <kern_img.h>
#include <kernel_block.h>

#include "spare_vmlinux.h"
#include "kern_static.h"

void* kern_static::find_prog(std::string lookupKey)
{
    return vector_pair_key_find<std::string, void*>(&prog_list, lookupKey);
}

void* kern_static::find_sect(std::string lookupKey)
{
    return vector_pair_key_find<std::string, void*>(&sect_list, lookupKey);
}

int kern_static::check_sect(std::string sect_name, void* sect_out)
{
    int result = -1;
    int index = 0;

    index = vector_pair_ind<std::string, void*>(&sect_list, sect_name);
    SAFE_BAIL(index == -1);
    // SAFE_BAIL(sect_list.find(sect_name) == sect_list.end());

    if (sect_out != 0)
    {
        // *sect_out = &sect_list[sect_name];
        *(void**)sect_out = sect_list[index].second;
    }

    result = 0;
fail:
    return result;
}

ELFBIT
int kern_static::gen_shstrtab_p(std::string** out_shstrtab, uint16_t* numSects, uint16_t* shstrtab_index)
{
    std::string shstrtabTmp = "\0";
    uint16_t sect_iter = 0;
    uint16_t str_index = 0;
    std::vector<std::pair<std::string, Elf_Shdr*>>* sect_list_l = (std::vector<std::pair<std::string, Elf_Shdr*>>*)&sect_list;
    
    for (auto i = sect_list_l->begin(); i != sect_list_l->end(); i++)
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

int kern_static::gen_shstrtab(std::string** out_shstrtab, uint16_t* numSects, uint16_t* shstrtab_index)
TEMPLIFY(gen_shstrtab, out_shstrtab, numSects, shstrtab_index)

int kern_static::base_new_shstrtab()
{
    char strtabRef[] = ".shstrtab";
    std::string* shtstrtab_tmp = 0;

    gen_shstrtab(&shtstrtab_tmp, NULL, NULL);
    insert_section(strtabRef, kern_sz, shtstrtab_tmp->size() + sizeof(strtabRef));
    return 0;
}

