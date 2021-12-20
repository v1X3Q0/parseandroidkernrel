#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <libgen.h>
#include <linux/limits.h>
#include <string.h>

#include <lz4.h>
#include <lz4frame.h>
#include <lz4hc.h>

#include <localUtil.h>

#define SAFE_LZ4FREE(x) \
    if (x) \
    { \
        LZ4F_freeDecompressionContext(x); \
        x = 0; \
    }


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

int iterate_lz4(char* lz4_in_raw, size_t lz4_in_sz, size_t* lz4_out_sz)
{
    int result = -1;
    LZ4F_dctx *curCtx = 0;
    LZ4F_frameInfo_t curFrame;
    size_t lz4_head_sz = lz4_in_sz;
    void* post_lz4 = 0;

    uint32_t* cur_decomp_sz = 0;
    
    LZ4F_errorCode_t errorCode = 0;
    int i = 0;

    errorCode = LZ4F_createDecompressionContext(&curCtx, LZ4F_VERSION);
    SAFE_BAIL(LZ4F_isError(errorCode));

    errorCode = LZ4F_getFrameInfo(curCtx, &curFrame, lz4_in_raw, &lz4_head_sz);
    SAFE_BAIL(LZ4F_isError(errorCode));

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

    // verify end of payload
    cur_decomp_sz += 2;
    post_lz4 = (void*)cur_decomp_sz;
    i = ((size_t)cur_decomp_sz - (size_t)lz4_in_raw);

    if (i == lz4_in_sz)
    {
        printf("input lz4 has no data post final checksum\n");
        goto fail;
    }

    *lz4_out_sz = i;
    // for (; i < lz4_in_sz; i++)
    // {
    //     if (lz4_in_raw[i] != 0)
    //     {
    //         printf("found nonzero block at offset 0x%x\n", i);
    //         break;
    //     }
    // }

    result = 0;
fail:
    SAFE_LZ4FREE(curCtx);
    return result;
}

int iterate_decomp_lz4(void* lz4_in_raw, size_t lz4_in_sz, void** out_payload, size_t* out_sz)
{
    int result = -1;
    LZ4F_dctx *curCtx = 0;
    LZ4F_frameInfo_t curFrame;
    size_t lz4_head_sz = lz4_in_sz;
    
    uint32_t* cur_decomp_sz = 0;
    size_t lz4_tmp_sz = 0;
    
    size_t curDecomp = 0;
    void* tmp_dst_point = 0;
    void* tmp_src_point = 0;
    size_t tmp_dst_sz = 0;
    LZ4F_decompressOptions_t tmp_decomp_opt = { 0 };
    LZ4F_errorCode_t errorCode = 0;

    std::vector<decomp_save*> decomp_array;

    errorCode = LZ4F_createDecompressionContext(&curCtx, LZ4F_VERSION);
    SAFE_BAIL(LZ4F_isError(errorCode));

    errorCode = LZ4F_getFrameInfo(curCtx, &curFrame, lz4_in_raw, &lz4_head_sz);
    SAFE_BAIL(LZ4F_isError(errorCode));

    cur_decomp_sz = (uint32_t*)((size_t)lz4_in_raw + lz4_head_sz);
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
    }

    result = 0;
fail:
    SAFE_LZ4FREE(curCtx);
    return result;
}

int main(int argc, char **argv)
{
    LZ4F_dctx *curCtx = 0;
    LZ4F_frameInfo_t curFrame;
    int opt = 0;
    const char* lz4_in_name = 0;
    char* lz4_in_raw = 0;
    char* lz4_out_raw = 0;
    size_t lz4_head_sz = 0;
    size_t lz4_in_sz = 0;
    size_t lz4_out_sz = 0;
    
    int lz4_rip = 0;
    int dec_it = 0;

    char dup_path[PATH_MAX];
    std::string out_file_suffix = "";
    std::string out_file_str;
    const char* out_file_name = 0;
    FILE* out_file;

    while ((opt = getopt(argc, argv, "dri:o:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            lz4_in_name = optarg;
            break;
        case 'o':
            out_file_name = optarg;
            break;
        case 'r':
            lz4_rip = 1;
            break;
        case 'd':
            dec_it = 1;
            break;
        default: /* '?' */
            usage(argv[0]);
        }
    }

    SAFE_BAIL(block_grab(lz4_in_name, (void**)&lz4_in_raw, &lz4_in_sz) == -1);
    lz4_head_sz = lz4_in_sz;

    if (lz4_rip)
    {
        SAFE_BAIL(iterate_lz4(lz4_in_raw, lz4_in_sz, &lz4_out_sz) == -1);
        lz4_out_raw = lz4_in_raw;
        out_file_suffix = ".orig";
        
        strcpy(dup_path, lz4_in_name);
        dirname(dup_path);
        out_file_str = (const char*)dup_path;
        out_file_str += "/excess_dump.raw";
        out_file = fopen(out_file_str.data(), "w");
        fwrite((lz4_out_raw + lz4_out_sz), 1, lz4_in_sz - lz4_out_sz, out_file);
        SAFE_FCLOSE(out_file);
    }
    else if (dec_it)
    {
        SAFE_BAIL(iterate_decomp_lz4(lz4_in_raw, lz4_in_sz, (void**)&lz4_out_raw, &lz4_out_sz) == -1);
        out_file_suffix = ".raw";
    }

    if (out_file_name == 0)
    {
        out_file_str = lz4_in_name;
        out_file_str += out_file_suffix;
        out_file_name = out_file_str.data();
    }        

    // write payload to file
    out_file = fopen(out_file_name, "w");
    fwrite(lz4_out_raw, 1, lz4_out_sz, out_file);

fail:
    SAFE_FREE(lz4_in_raw);
    SAFE_FCLOSE(out_file);
    return 0;
}