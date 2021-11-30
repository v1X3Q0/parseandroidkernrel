#ifndef PARSEANDKERN_H
#define PARSEANDKERN_H

#define MAX_PARAM_PREFIX_LEN (64 - sizeof(unsigned long))
#define MODULE_NAME_LEN MAX_PARAM_PREFIX_LEN
typedef struct
{
    unsigned long crc;
    char name[MODULE_NAME_LEN];
} modversion_info;

typedef struct
{
	unsigned long value;
	const char *name;
} kernel_symbol;

class kern_img
{
public:
    kern_img(uint32_t* binBegin_a) : binBegin(binBegin_a) { parseAndGetGlobals(); };
    int grab_sinittext();
    int grab_primary_switch();
    int grab_primary_switched();
    int grab_start_kernel_g();
    int base_ksymtab_strings();
    int base_kcrctab();
    int base_ksymtab();
    int findKindInKstr(const char* newString, int* index);
    int parseAndGetGlobals();

    kernel_symbol* get_ksymtab() { return __ksymtab; };
    size_t get_ksyms_count() { return ksyms_count; };
    uint32_t* get_kcrctab() { return __kcrctab; };
private:
    uint32_t* binBegin;
    uint32_t* _sinittext;
    uint32_t* __primary_switch;
    uint32_t* __primary_switched;
    uint32_t* __create_page_tables;
    uint32_t* start_kernel;
    size_t __modver;
    size_t __param;
    const char* __ksymtab_strings_g;
    uint32_t* __kcrctab;
    kernel_symbol* __ksymtab;
    size_t ksyms_count;
};

#endif