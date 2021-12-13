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

int kern_img::patch_and_write(Elf64_Ehdr* vmlinux_base, Elf64_Shdr* vmlinux_cur, Elf64_Phdr* phBase, size_t offset)
{
    int result = -1;
    Elf64_Shdr nullSec = { 0 };
    Elf64_Phdr* phdrTemp = 0;
    int phdrCount = 0;
    memcpy(vmlinux_cur, &nullSec, sizeof(Elf64_Shdr));
    vmlinux_cur++;
    auto j = sect_list.begin();
    j++;

    for (auto i = sect_list.begin(); i != sect_list.end(); i++)
    {
        // fix the offset and size of the target segment
        i->second->sh_offset += offset;
        if ((i->second->sh_size == 0) && (j != sect_list.end()))
        {
            i->second->sh_size = (j->second->sh_offset + offset) - i->second->sh_offset;
        }
        // push the changes and the segment header
        memcpy(vmlinux_cur, i->second, sizeof(Elf64_Shdr));

        // fix the program header if it has a match
        phdrTemp = find_prog(i->first);
        if (phdrTemp != 0)
        {
            phdrTemp->p_offset = i->second->sh_offset;
            phdrTemp->p_filesz = phdrTemp->p_memsz = i->second->sh_size;
            memcpy(phBase, phdrTemp, sizeof(Elf64_Phdr));
            phdrCount++;
            phBase++;
        }

        // increment the section header iterator that will be used for copying
        vmlinux_cur++;
        // increment the next iterator that will be used for the size calc
        if (j != sect_list.end())
        {
            j++;
        }
    }

    vmlinux_base->e_phnum = phdrCount;

    result = 0;
    return result;
}

int kern_img::gen_shstrtab(std::string** out_shstrtab, uint16_t* numSects, uint16_t* shstrtab_index)
{
    std::string shstrtabTmp = "";
    shstrtabTmp += '\0';
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

int kern_img::gen_vmlinux_sz(size_t* outSz, size_t headOffset)
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
    szTemp += ((sect_list.size() + 1) * sizeof(Elf64_Shdr));

    *outSz = szTemp;
    return 0;
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

