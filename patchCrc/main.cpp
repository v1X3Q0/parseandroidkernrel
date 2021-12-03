#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>
#include <parseAndKern.h>
#include "patchCrc.h"
#include "iterate_dir.h"

int usage(const char* name)
{
    fprintf(stderr, "Usage: %s [-v vmlinux] [-k kernel_image] [-m kernel_driver] [-i vendor_mod_path]*\n",
        name);
    exit(EXIT_FAILURE);

}

int main(int argc, char **argv)
{
    const char *newDriver = 0;
    const char* vmlinux_targ = 0;
    const char* kernimg_targ = 0;
    const char* vendorimg_path = 0;
    std::pair<std::string, unsigned long> tmpObj;
    std::map<std::string, unsigned long> versMap;
    std::map<std::string, unsigned long> vendor_crcs;
    std::vector<std::string> vendorimg_names;
    size_t drvSize = 0;
    char *drverBase = 0;
    char *vmlinuxBase = 0;
    char *kernimgBase = 0;

    kernel_symbol* ksymBase = 0;
    size_t ksymCount = 0;
    uint32_t* kcrcBase = 0;
    kern_img* parsedKernimg = 0;

    FILE* drvoutTmp = 0;
    int crcargBase = 0;

    int opt = 0;
    
    while ((opt = getopt(argc, argv, "v:k:m:i:")) != -1)
    {
        switch (opt)
        {
        case 'v':
            vmlinux_targ = optarg;
            break;
        case 'k':
            kernimg_targ = optarg;
            break;
        case 'm':
            newDriver = optarg;
            break;
        case 'i':
            vendorimg_path = optarg;
            break;
        default: /* '?' */
            usage(argv[0]);
        }
    }

    if (optind < argc)
    {
        crcargBase = optind % 2;
        for (int i = optind; i < argc; i++)
        {
            if ((i % 2) == crcargBase)
            {
                tmpObj.first = argv[i];
            }
            else
            {
                tmpObj.second = atoi(argv[i]);
                versMap.insert(tmpObj);
            }
        }
    }

    if (newDriver == 0)
    {
        usage(argv[0]);
    }

    if (vmlinux_targ != 0)
    {
        SAFE_BAIL(block_grab(vmlinux_targ, (void**)&vmlinuxBase, NULL) == -1);
        SAFE_BAIL(specSection(vmlinuxBase, "__ksymtab", (void**)&ksymBase, &ksymCount) == -1);
        ksymCount /= sizeof(kernel_symbol);
        SAFE_BAIL(specSection(vmlinuxBase, "__kcrctab", (void**)&kcrcBase, 0) == -1);
    }
    else if (kernimg_targ != 0)
    {
        SAFE_BAIL(vendorimg_path == 0);
        SAFE_BAIL(get_libmodules(vendorimg_path, &vendorimg_names) == -1);
        symvers_modules(&vendorimg_names, &vendor_crcs);
        SAFE_BAIL(block_grab(kernimg_targ, (void**)&kernimgBase, NULL) == -1);
        parsedKernimg = new kern_img((uint32_t*)kernimgBase);
        ksymBase = parsedKernimg->get_ksymtab();
        ksymCount = parsedKernimg->get_ksyms_count();
        kcrcBase = parsedKernimg->get_kcrctab();
    }

    SAFE_BAIL(newDriver == 0);
    SAFE_BAIL(block_grab(newDriver, (void**)&drverBase, &drvSize) == -1);

    populateVers(drverBase, &versMap);
    
    if ((ksymBase == 0) || (ksymBase->name == 0))
    {
        populateCrcMap(&versMap, &vendor_crcs);
    }
    else
    {
        populateCrcKsymtab(&versMap, ksymBase, ksymCount, kcrcBase);
    }

    patchVersion(drverBase, &versMap);

    drvoutTmp = fopen(newDriver, "w");
    fwrite(drverBase, 1, drvSize, drvoutTmp);
    printf("patched file %s\n", newDriver);
fail:
    SAFE_DEL(parsedKernimg);
    SAFE_FCLOSE(drvoutTmp);
    SAFE_FREE(drverBase);
    SAFE_FREE(vmlinuxBase);
    SAFE_FREE(kernimgBase);
    return 0;
}
