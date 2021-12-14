#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <lz4.h>
#include <lz4frame.h>
#include <lz4hc.h>

#include <localUtil.h>

int usage(const char* name)
{
    fprintf(stderr, "Usage: %s [-i lz4_in_name]\n",
        name);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    LZ4F_dctx *curCtx;
    LZ4F_frameInfo_t curFrame;
    int opt = 0;
    const char* lz4_in_name = 0;
    char* lz4_in_raw = 0;
    size_t lz4_head_sz = 0;
    size_t lz4_in_sz = 0;
    LZ4F_errorCode_t errorCode = 0;
    uint32_t* cur_decomp_sz = 0;
    void* post_lz4 = 0;
    int i = 0;

    while ((opt = getopt(argc, argv, "i:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            lz4_in_name = optarg;
            break;
        default: /* '?' */
            usage(argv[0]);
        }
    }

    block_grab(lz4_in_name, (void**)&lz4_in_raw, &lz4_in_sz);
    lz4_head_sz = lz4_in_sz;

    errorCode = LZ4F_createDecompressionContext(&curCtx, LZ4F_VERSION);
    SAFE_BAIL(LZ4F_isError(errorCode));

    LZ4F_getFrameInfo(curCtx, &curFrame, lz4_in_raw, &lz4_head_sz);

    cur_decomp_sz = (uint32_t*)((size_t)lz4_in_raw + lz4_head_sz);
    if (*cur_decomp_sz != 0)
    {
        printf("found first block data at %p with value 0x%x\n", cur_decomp_sz, *cur_decomp_sz);
    }

    while (*cur_decomp_sz != 0)
    {
        cur_decomp_sz = (uint32_t*)((size_t)cur_decomp_sz + (size_t)*cur_decomp_sz + sizeof(uint32_t));
        printf("found block data at %p with value 0x%x\n", cur_decomp_sz, *cur_decomp_sz);
    }

    cur_decomp_sz += 2;
    post_lz4 = (void*)cur_decomp_sz;
    i = ((size_t)cur_decomp_sz - (size_t)lz4_in_raw);

    for (; i < lz4_in_sz; i++)
    {
        if (lz4_in_raw[i] != 0)
        {
            printf("found nonzero block at offset 0x%x", i);
            break;
        }
    }

fail:
    return 0;
}