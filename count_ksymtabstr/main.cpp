#include <iostream>
#include <localUtil.h>
#include <getopt.h>

int STRINGALIGN = 0;

void usage(char* callername)
{
    fprintf(stderr, "Usage: %s [-n symtabl_sz] [-v] filename ksymstrtab\n",
            callername);
    fprintf(stderr, "-n size of symbol table\n"\
            "-v indicates the symbol table needs an 8 byte alignment, vxworks does this");
    exit(EXIT_FAILURE);
}

int stringcount(const char* vaddr, size_t* strcount_out, size_t* strtabend_out)
{
    const char* vaddr_iter = 0;
    int restemp = 0;
    size_t strcount = 0;
    int result = -1;
    int padtemp = 0;

    vaddr_iter = vaddr;
    while (1)
    {
        restemp = strnlenu(vaddr_iter, 100);
        if ((restemp <= 0) || (((*strtabend_out) != 0) && ((*strtabend_out) <= (size_t)vaddr_iter)))
        {
            printf("%d\n", restemp);
            break;
        }
        else if (STRINGALIGN != 0)
        {
            if (((restemp + 1) % STRINGALIGN) != 0)
            {
                restemp = restemp + (STRINGALIGN - (restemp % STRINGALIGN));
            }
            else
            {
                restemp++;
            }
        }
        else if (STRINGALIGN == 0)
        {
            restemp = restemp + 2;
        }
        vaddr_iter = (const char*)((size_t)vaddr_iter + restemp);
        strcount++;
    }
    
    SAFE_BAIL(strcount == 0);

    if (strcount_out != 0)
    {
        *strcount_out = strcount;
    }

    if (strtabend_out != 0)
    {
        *strtabend_out = (size_t)vaddr_iter;
    }

fail:
    return result;
}

int main(int argc, char** argv)
{
    void* allocBase = 0;
    size_t fSize = 0;
    size_t strtabend = 0;
    size_t strcount = 0;
    const char* in_file_name = 0;
    size_t ksymstrtab_addr = 0;
    size_t symtabsz = 0;
    int opt = 0;

    while ((opt = getopt(argc, argv, "vn:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            usage(argv[0]);
            break;
        case 'v':
            STRINGALIGN = 0x08;
            break;
        case 'n':
            symtabsz = strtoull(optarg, NULL, 0x10);
            break;
        default: /* '?' */
            usage(argv[0]);
        }
    }

    if (optind == argc)
    {
        usage(argv[0]);
    }
    else
    {
        in_file_name = argv[optind];
        optind++;
    }

    if (optind == argc)
    {
        usage(argv[0]);
    }
    else
    {
        ksymstrtab_addr = strtoull(argv[optind], NULL, 0x10);;
        optind++;
    }

    block_grab(in_file_name, &allocBase, &fSize);

    ksymstrtab_addr = (size_t)allocBase + ksymstrtab_addr;

    if (symtabsz != 0)
    {
        strtabend = ksymstrtab_addr + symtabsz;
    }

    stringcount((const char*)ksymstrtab_addr, &strcount, &strtabend);
    strtabend = strtabend - (size_t)allocBase;

    printf("table has %d strings, and ends at file off 0x%lx\n", strcount, strtabend);

    SAFE_FREE(allocBase);
}
