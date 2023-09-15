#include "kern_static.h"

int kern_static::gen_vmlinux_sz(size_t *outSz, size_t headOffset)
TEMPLIFY(gen_vmlinux_sz, outSz, headOffset)
void kern_static::elfConstruction()
TEMPLIFY(elfConstruction)
void kern_static::elfHeadConstruction(void* elfHead)
TEMPLIFY(elfHeadConstruction, elfHead)
int kern_static::patch_and_write(void *vmlinux_base, void *vmlinux_cur, void *phBase, size_t offset)
TEMPLIFY(patch_and_write, vmlinux_base, vmlinux_cur, phBase, offset)
int kern_static::parseAndGetGlobals()
TEMPLIFY(parseAndGetGlobals)
void kern_static::insert_section(std::string sec_name, uint64_t sh_offset, uint64_t sh_size)
TEMPLIFY(insert_section, sec_name, sh_offset, sh_size)
void kern_static::insert_elfsegment(std::string sec_name, int permissions, uint64_t Virtual, uint64_t Physical, uint64_t FileOffset, uint64_t sh_size)
TEMPLIFY(insert_elfsegment, sec_name, permissions, Virtual, Physical, FileOffset, sh_size)
void kern_static::insert_elfsection(std::string sec_name, uint64_t Virtual, uint64_t FileOffset, uint64_t sh_size)
TEMPLIFY(insert_elfsection, sec_name, Virtual, FileOffset, sh_size)
int kern_static::kcrc_index(std::string symbol, uint32_t *kcrc)
TEMPLIFY(kcrc_index, symbol, kcrc)

void kern_static::insert_elfsymbol(std::string symname, int symtype, uint64_t symval, uint64_t offset)
TEMPLIFY(insert_elfsymbol, symname, symtype, symval, offset)
void kern_static::ref_elfsymbol(const char* symname, int symtype, uint64_t symval)
TEMPLIFY(ref_elfsymbol, symname, symtype, symval)

void kern_static::setsymtab(void *symtabin, size_t szin)
{
    newsymtab = symtabin;
    newsymtabsz = szin;

    insert_elfsection(".symtab", 0, 0, szin);
}

int kern_static::gen_symtab(void **symtabout, size_t *szout)
{
    if (symtabout != 0)
    {
        *symtabout = newsymtab;
    }
    if (szout != 0)
    {
        *szout = newsymtabsz;
    }
    return 0;
}

void* kern_static::find_prog(std::string lookupKey, int* index_out)
{
    return vector_pair_key_find<std::string, void*>(&prog_list, lookupKey, index_out);
}

void* kern_static::find_sect(std::string lookupKey, int* index_out)
{
    return vector_pair_key_find<std::string, void*>(&sect_list, lookupKey, index_out);
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

kern_static::kern_static(void* kernimg, size_t kern_sz_a, int bitness_a,
    int arch_a, int endianess_a) :  kernel_linux((uint32_t*)kernimg, kern_sz_a, bitness_a)
{
    architecture = arch_a;
    endianess = endianess_a;
}
