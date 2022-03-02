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

int kern_static::base_new_shstrtab()
{
    char strtabRef[] = ".shstrtab";
    std::string* shtstrtab_tmp = 0;

    gen_shstrtab(&shtstrtab_tmp, NULL, NULL);
    insert_section(strtabRef, kern_sz, shtstrtab_tmp->size() + sizeof(strtabRef));
    return 0;
}
