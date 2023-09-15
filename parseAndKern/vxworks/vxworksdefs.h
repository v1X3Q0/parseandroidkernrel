#pragma once
#include <stdint.h>

typedef struct
{
    uint32_t buf0;
    uint32_t name_address;
    uint32_t symval;
    uint32_t buf1;
    uint32_t symtype;
} vxworks6_5_symbol_t;

#ifdef __cplusplus
extern "C"
{
#endif
int parseandgetoffsets_extc(void* kern_target);
#ifdef __cplusplus
}
#endif
