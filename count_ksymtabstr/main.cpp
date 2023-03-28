#include <iostream>
#include <localUtil.h>
#include <getopt.h>

void usage(char* callername)
{
    fprintf(stderr, "Usage: %s filename ksymstrtab\n",
            callername);
    exit(EXIT_FAILURE);
}

int stringcount(const char* vaddr, size_t* strcount_out, size_t* strtabend_out)
{
    const char* vaddr_iter = 0;
    int restemp = 0;
    size_t strcount = 0;
    int result = -1;

    vaddr_iter = vaddr;
    while (1)
    {
        restemp = strnlenu(vaddr_iter, 100);
        if (restemp <= 0)
        {
            printf("%d\n", restemp);
            break;
        }
        vaddr_iter = (const char*)((size_t)vaddr_iter + restemp + 2);
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
    int opt = 0;

    while ((opt = getopt(argc, argv, "df")) != -1)
    {
        switch (opt)
        {
        case 'h':
            usage(argv[0]);
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

    block_grab(argv[1], &allocBase, &fSize);

    ksymstrtab_addr = (size_t)allocBase + ksymstrtab_addr;
    stringcount((const char*)ksymstrtab_addr, &strcount, &strtabend);
    strtabend = strtabend - (size_t)allocBase;

    printf("table has %d strings, and ends at file off 0x%lx\n", strcount, strtabend);

    SAFE_FREE(allocBase);
}