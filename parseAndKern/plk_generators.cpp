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

int kern_static::gen_shstrtab(std::string** out_shstrtab, uint16_t* numSects, uint16_t* shstrtab_index)
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

