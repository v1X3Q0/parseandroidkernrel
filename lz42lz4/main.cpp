#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <vector>
#include <string>

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

typedef struct 
{
    void* out_buf;
    size_t alloc_sz;
    size_t out_sz;
} decomp_save;

int main(int argc, char **argv)
{
    LZ4F_dctx *curCtx;
    LZ4F_frameInfo_t curFrame;
    int opt = 0;
    const char* lz4_in_name = 0;
    char* lz4_in_raw = 0;
    size_t lz4_head_sz = 0;
    size_t lz4_in_sz = 0;
    
    uint32_t* cur_decomp_sz = 0;
    size_t lz4_tmp_sz = 0;
    void* post_lz4 = 0;
    
    size_t curDecomp = 0;
    void* tmp_dst_point = 0;
    void* tmp_src_point = 0;
    size_t tmp_dst_sz = 0;
    LZ4F_decompressOptions_t tmp_decomp_opt = { 0 };
    LZ4F_errorCode_t errorCode = 0;
    int i = 0;

    std::string out_file_str;
    const char* dec_out_name = 0;
    FILE* out_file;
    std::vector<decomp_save*> decomp_array;

    while ((opt = getopt(argc, argv, "i:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            lz4_in_name = optarg;
            break;
        case 'o':
            dec_out_name = optarg;
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
        curDecomp = 0;
        lz4_tmp_sz = *cur_decomp_sz;
        tmp_dst_point = calloc(lz4_tmp_sz, 1);
        tmp_src_point = cur_decomp_sz;
        decomp_array.push_back(new decomp_save{tmp_dst_point, *cur_decomp_sz, 0});
        
        while (curDecomp < *cur_decomp_sz)
        {
            tmp_dst_sz = *cur_decomp_sz;
            errorCode = LZ4F_decompress(curCtx, tmp_dst_point, &tmp_dst_sz, tmp_src_point, &lz4_tmp_sz, &tmp_decomp_opt);
            SAFE_BAIL(LZ4F_isError(errorCode));
            
            curDecomp += lz4_tmp_sz;
            tmp_src_point = (void*)((size_t)lz4_tmp_sz + (size_t)(tmp_src_point));
            tmp_dst_point = (void*)((size_t)tmp_dst_sz + (size_t)(tmp_dst_point));
            // lz4_tmp_sz = errorCode;
            lz4_tmp_sz = *cur_decomp_sz - lz4_tmp_sz;
        }

        cur_decomp_sz = (uint32_t*)((size_t)cur_decomp_sz + (size_t)*cur_decomp_sz + sizeof(uint32_t));
        printf("found block data at %p with value 0x%x\n", cur_decomp_sz, *cur_decomp_sz);
    }

    // verify end of payload
    cur_decomp_sz += 2;
    post_lz4 = (void*)cur_decomp_sz;
    i = ((size_t)cur_decomp_sz - (size_t)lz4_in_raw);

    // write payload to file
    if (dec_out_name == 0)
    {
        out_file_str = lz4_in_name;
        out_file_str += ".raw";
        dec_out_name = out_file_str.data();
    }
    out_file = fopen(dec_out_name, "r");
    for (i = 0; i < decomp_array.size(); i++)
    {
        fwrite(decomp_array[i]->out_buf, 1, decomp_array[i]->out_sz, out_file);    
    }

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