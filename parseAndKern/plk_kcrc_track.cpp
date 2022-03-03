#include <algorithm>
#include <functional>
#include <sv_gpl.h>
#include "kern_static.h"

bool strcmp_bool(const char* first, const char* second)
{
    if (strcmp(first, second) == 0)
        return true;
    return false;
}

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
int kern_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::populate_kcrc_map()
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

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
int kern_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::kcrc_index(std::string symbol, uint32_t* kcrc)
{
    int result = -1;
    Elf_Shdr* kcrctab_sec = 0;
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

