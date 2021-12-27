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

    insert_section("__ksymtab", SHT_PROGBITS, 0, (size_t)tmpSymSearch[0].start, (size_t)tmpSymSearch[0].start, (size_t)tmpSymSearch[0].stop - (size_t)tmpSymSearch[0].start, 0, 0, 8, 0);
    insert_section("__kcrctab", SHT_PROGBITS, 0, (size_t)tmpSymSearch[1].start, (size_t)tmpSymSearch[1].start, (size_t)tmpSymSearch[1].stop - (size_t)tmpSymSearch[1].start, 0, 0, 1, 0);
    insert_section("__ksymtab_strings", SHT_PROGBITS, 0, (size_t)tmpSymSearch[2].start, (size_t)tmpSymSearch[2].start, (size_t)tmpSymSearch[2].stop - (size_t)tmpSymSearch[2].start, 0, 0, 1, 0);

finish:
    result = 0;
fail:
    SAFE_FREE(kBuffer);
    SAFE_LIVE_FREE(tmpSymSearch)
    return result;
}

