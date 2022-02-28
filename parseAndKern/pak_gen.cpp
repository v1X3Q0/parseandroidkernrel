#include <string.h>
#include <stdlib.h>
#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>
#include "spare_vmlinux.h"
#include "parseAndKern.h"

int kern_static::gen_shstrtab(std::string** out_shstrtab, uint16_t* numSects, uint16_t* shstrtab_index)
{
    std::string shstrtabTmp = '\0';
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

int kern_static::gen_vmlinux_sz(size_t* outSz, size_t headOffset)
{
    size_t szTemp = 0;
    std::string* shstrtab_tmp;

    // we are gonna consider that the header size SHOULD just be a page, however
    // under the condition that maybe we got a dummy thicc header, we allow it
    // to be passed in.
    szTemp += headOffset;

    // we are using the whole parsed kernel image as input, so add that.
    szTemp += kern_sz;
    
    // generate shstrtab
    gen_shstrtab(&shstrtab_tmp, NULL, NULL);

    // adding the shstrtab to the image whole
    szTemp += shstrtab_tmp->size();

    // adding the section list to the array, plus 1 for the null section
    szTemp += ((sect_list.size() + 1) * sizeof(Elf_Shdr));

    *outSz = szTemp;
    return 0;
}

