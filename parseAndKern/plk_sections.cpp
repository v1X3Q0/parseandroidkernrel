#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ibeSet.h>
#include <localUtil.h>
#include <kernel_block.h>

#include <krw_util.h>

#include "spare_vmlinux.h"
#include "kern_static.h"

#ifdef LIVE_KERNEL
#include <hdeA64.h>
#else
#include <capstone/capstone.h>
#endif

template <typename size_b, typename Elf_Ehdr, typename Elf_Shdr,
    typename Elf_Phdr, typename Elf_Xword, typename Elf_Word>
int kern_static<size_b, Elf_Ehdr, Elf_Shdr, Elf_Phdr, Elf_Xword, Elf_Word>::base_new_shstrtab()
{
    char strtabRef[] = ".shstrtab";
    std::string* shtstrtab_tmp = 0;

    gen_shstrtab(&shtstrtab_tmp, NULL, NULL);
    insert_section(strtabRef, kern_sz, shtstrtab_tmp->size() + sizeof(strtabRef));
    return 0;
}
