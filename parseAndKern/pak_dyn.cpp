#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>

#include <krw_util.h>

#include "spare_vmlinux.h"
#include "parseAndKern.h"

// only for live kernel definitions
#ifdef LIVE_KERNEL
int kern_img::live_kern_addr(void* target_kernel_address, size_t size_kernel_buf, void** out_live_addr)
{
    int result = -1;
    void* newKernelAddress = 0;

    newKernelAddress = calloc(size_kernel_buf, 1);
    SAFE_BAIL(newKernelAddress == 0);
    SAFE_BAIL(kRead(newKernelAddress, size_kernel_buf, (size_t)target_kernel_address) == -1);

    *out_live_addr = newKernelAddress;

    result = 0;
    goto finish;
fail:
    SAFE_FREE(newKernelAddress);
finish:
    return result;
}
#else
int kern_img::live_kern_addr(void* target_kernel_address, size_t size_kernel_buf, void** out_live_addr)
{
    int result = -1;
    *out_live_addr = target_kernel_address;
    result = 0;
    return result;
}
#endif

int kern_img::kernel_search_seq(void* img_var, size_t img_var_sz, uint8_t* byte_search, size_t search_sz,
    size_t offset, size_t step, bool match, void** out_img_off)
{
    int result = -1;
    uint8_t* kern_copy = 0;
    uint8_t* iterPoint = 0;

    SAFE_BAIL(live_kern_addr(img_var, img_var_sz, (void**)&kern_copy) == -1);
    iterPoint = (uint8_t*)((size_t)kern_copy + offset);

    for (int kern_index = offset; kern_index < img_var_sz; kern_index += step, iterPoint += step)
    {
        // principal behind this is we can search for, for instance, a nonzero block using this same
        // routine. so  if 0 == 0) == true, then it will keep going. but if non == 0) ~ false, then
        // all good
        if ((memcmp(byte_search, iterPoint, search_sz) == 0) == match)
        {
            *out_img_off = iterPoint;
            goto found;
        }
    }
    goto fail;
found:
    if (live_kernel == true)
    {
        *out_img_off = (void**)((size_t)iterPoint - (size_t)kern_copy + (size_t)img_var);
    }
    result = 0;
fail:
    SAFE_LIVE_FREE(kern_copy);
    return result;
}

// kinda for both?
int kern_img::kernel_search(instSet* getB, void* img_var, size_t img_var_sz, uint32_t** out_img_off)
{
    int result = -1;
    void* img_var_local = 0;

    SAFE_BAIL(live_kern_addr(img_var, img_var_sz, &img_var_local) == -1);
    SAFE_BAIL(getB->findPattern((uint32_t*)img_var_local, img_var_sz, out_img_off) == -1);
    if (live_kernel == true)
    {
        *out_img_off = (uint32_t*)((size_t)*out_img_off - (size_t)img_var_local + (size_t)binBegin);
    }

    result = 0;
fail:
    SAFE_LIVE_FREE(img_var_local);
    return result;
}

// routine to be used for dynamic use, the relocation table will fill these up,
// maybe someday i can see how they are filled in static use as well.
int kern_img::base_ksymtab_kcrctab_ksymtabstrings()
{
#define TARGET_KSYMTAB_SEARCH_STR followStr
    int result = -1;
    Elf64_Shdr* head_text_shdr = 0;
    Elf64_Shdr* text_shdr = 0;
    Elf64_Shdr* init_text_shdr = 0;
    char* kBuffer = 0;
    size_t searchSz = 0;
    char searchStr[] = "module.sig_enforce";
    char followStr[] = "nomodule";
    symsearch* tmpSymSearch = 0;
    uint16_t poststr_block = 0;

    size_t ksymtab_base = 0;
    size_t ksymtab_gpl_base = 0;
    size_t kcrctab_base = 0;
    size_t kcrctab_gpl_base = 0;
    size_t ksymtab_strings_base = 0;

    // check if all 3 already exists
    FINISH_IF((check_sect("__ksymtab", NULL) == 0) &&
        (check_sect("__kcrctab", NULL) == 0) &&
        (check_sect("__ksymtab_strings", NULL) == 0)
        );

    SAFE_BAIL(check_sect(".head.text", &head_text_shdr) == -1);
    SAFE_BAIL(check_sect(".init.text", &init_text_shdr) == -1);
    SAFE_BAIL(check_sect(".text", &text_shdr) == -1);
    
    // if not, begin the search! brute force for our string, with an upper bound
    // limit of the .init.text section. Once we get there we have to stop
    // reading or kernel panic.

    // skip a section by starting at the .text, though if we can't guarantee
    // alignment.... may have to do .head.text, which should only be an
    // additional page or so.
    searchSz = init_text_shdr->sh_offset - head_text_shdr->sh_offset;
    SAFE_BAIL(kernel_search_seq((void*)head_text_shdr->sh_addr, searchSz, (uint8_t*)followStr, sizeof(followStr), 0, 1, true, (void**)&kBuffer) == -1);

    kBuffer = (char*)((size_t)kBuffer + sizeof(TARGET_KSYMTAB_SEARCH_STR));
    BIT_PAD(kBuffer, char*, 8);

    SAFE_BAIL(live_kern_addr(kBuffer, sizeof(symsearch) * 3, (void**)&tmpSymSearch) == -1);

    // index 0 and 1 are each the ksymtab and ksymtab_gpl respectively
    // index 2 bases the kcrc, but its end is the same as its entry. The end of it is the 
    // crcgpl, which is referenced by the gpl ksymtab
    ksymtab_base = (size_t)tmpSymSearch[0].start;
    ksymtab_gpl_base = (size_t)tmpSymSearch[1].start;
    kcrctab_base = (size_t)tmpSymSearch[2].start;
    kcrctab_gpl_base = (size_t)tmpSymSearch[1].crcs;
    ksymtab_strings_base = (size_t)tmpSymSearch[2].crcs;

    insert_section("__ksymtab", SHT_PROGBITS, 0, ksymtab_base, ksymtab_base, ksymtab_gpl_base - ksymtab_base, 0, 0, 8, 0);
    insert_section("__ksymtab_gpl", SHT_PROGBITS, 0, ksymtab_gpl_base, ksymtab_gpl_base, kcrctab_base - ksymtab_gpl_base, 0, 0, 8, 0);
    insert_section("__kcrctab", SHT_PROGBITS, 0, kcrctab_base, kcrctab_base, kcrctab_gpl_base - kcrctab_base, 0, 0, 1, 0);
    insert_section("__kcrctab_gpl", SHT_PROGBITS, 0, kcrctab_gpl_base, kcrctab_gpl_base, ksymtab_strings_base - kcrctab_gpl_base, 0, 0, 1, 0);

    SAFE_BAIL(kernel_search_seq((void*)ksymtab_strings_base, init_text_shdr->sh_addr - ksymtab_strings_base, (uint8_t*)&poststr_block, sizeof(poststr_block), 0, 1, true, (void**)&kBuffer) == -1);
    insert_section("__ksymtab_strings", SHT_PROGBITS, 0, ksymtab_strings_base, ksymtab_strings_base, (size_t)kBuffer - ksymtab_strings_base + 1, 0, 0, 1, 0);

    ksyms_count = (kcrctab_base - ksymtab_base) / sizeof(kernel_symbol);
finish:
    result = 0;
fail:
    SAFE_LIVE_FREE(tmpSymSearch)
    return result;
}

