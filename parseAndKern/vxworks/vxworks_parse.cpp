#include "../kern_static.h"
#include "vxworksdefs.h"
#include <stdlib.h>
#include <string.h>
#include <string>
#include <elf.h>

#include "vxworksdefs.h"

#define VARTEMPLIFY(FUNC, KERNOBJ, ...) \
    { \
        if (KERNOBJ->get_bitness() == 64) \
        { \
            return FUNC ## _p ELFBIT64(KERNOBJ, __VA_ARGS__); \
        } \
        else \
        { \
            return FUNC ## _p ELFBIT32(KERNOBJ, __VA_ARGS__); \
        } \
    }

#define VARTEMPLIFYSING(FUNC, KERNOBJ) \
    { \
        if (KERNOBJ->get_bitness() == 64) \
        { \
            return FUNC ## _p ELFBIT64(KERNOBJ); \
        } \
        else \
        { \
            return FUNC ## _p ELFBIT32(KERNOBJ); \
        } \
    }

#if !defined(__linux__)
inline uint32_t __bswap_32(uint32_t value)
{
    return (((value & 0x000000FF) << 24) |
            ((value & 0x0000FF00) <<  8) |
            ((value & 0x00FF0000) >>  8) |
            ((value & 0xFF000000) >> 24));
}
#endif

ELFBIT
int parseandgetoffsets_p(kern_static* kern_target)
{
    Elf_Shdr* vxsymtab = 0;
    Elf_Shdr* strsect = 0;
    vxworks6_5_symbol_t* symind = 0;
    int refsymcount = 0;
    Elf_Sym* newsymtab = 0;
    int strtab_index = 0;

    vxsymtab = (Elf_Shdr*)kern_target->find_sect("symbols");
    strsect = (Elf_Shdr*)kern_target->find_sect(".strtab", &strtab_index);
    symind = (vxworks6_5_symbol_t*)(vxsymtab->sh_offset + kern_target->get_binbegin());
    refsymcount = vxsymtab->sh_size / (sizeof(vxworks6_5_symbol_t));
    newsymtab = (Elf_Sym*)malloc(refsymcount * sizeof(Elf_Sym));
    vxsymtab->sh_link = strtab_index;

    for (int i = 0; i < refsymcount; i++)
    {
        int functype = 0;
        // function
        if (symind->symtype == __bswap_32(0x500))
        {
            functype = STT_FUNC;
        }
        // constant
        else if (symind->symtype == __bswap_32(0x300))
        {
            functype = STT_NOTYPE;
        }
        // data
        else if (symind->symtype == __bswap_32(0x900))
        {
            functype = STT_OBJECT;
        }
        // external
        else if (symind->symtype == __bswap_32(0x1100))
        {
            functype = STT_NOTYPE;
        }
        
        newsymtab[i].st_name = __bswap_32(symind->name_address) - strsect->sh_addr;
        newsymtab[i].st_value = __bswap_32(symind->symval);
        newsymtab[i].st_info = functype;
        // kern_target->ref_elfsymbol((const char*)symind->name_address, functype, symind->symval);
    }
    kern_target->setsymtab((void*)newsymtab, refsymcount * sizeof(Elf_Sym));

    return 0;
}

int parseandgetoffsets(kern_static* kern_target)
VARTEMPLIFYSING(parseandgetoffsets, kern_target)

int parseandgetoffsets_extc(void* kern_target)
{
    return parseandgetoffsets((kern_static*)kern_target);
}
