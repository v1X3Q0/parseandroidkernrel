#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <localUtil.h>

#include "patchCrc.h"
#include "iterate_dir.h"
#include <kernel_block.h>
#include <kern_static.h>

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
    kernel_linux* parsedKernimg = 0;
    uint32_t bitness_local = sizeof(size_t) * 8;

    FILE* drvoutTmp = 0;
    int crcargBase = 0;

    int opt = 0;
    
    while ((opt = getopt(argc, argv, "v:k:n:m:i:")) != -1)
    {
        switch (opt)
        {
        case 'v':
            vmlinux_targ = optarg;
            break;
        case 'k':
            kernimg_targ = optarg;
            break;
        case 'n':
            newDriver = optarg;
            break;
        case 'i':
            vendorimg_path = optarg;
            break;
        case 'm':
            bitness_local = atoi(optarg);
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
        parsedKernimg = kernel_block::allocate_kern_img<kern_static>(kernimg_targ, bitness_local);
        SAFE_FAIL(parsedKernimg == 0, "kernel image  was invalid\n");
    }

    // if we have a vendor image path, then we should get all the vendor drivers
    // and rip their symbols out to use for our kcrc
    if (vendorimg_path != 0)
    {
        SAFE_FAIL(vendorimg_path == 0, "need a provided vendor image path\n");
        SAFE_FAIL(get_libmodules(vendorimg_path, &vendorimg_names) == -1, "provided image path invalid\n");
        symvers_modules(&vendorimg_names, &vendor_crcs);
    }

    SAFE_BAIL(newDriver == 0);
    SAFE_BAIL(block_grab(newDriver, (void**)&drverBase, &drvSize) == -1);

    populateVers(drverBase, &versMap);
    
    // assuming that we got a vendor image, then we should use it to fill out our kcrc's
    if ((ksymBase == 0) || (ksymBase->name == 0))
    {
        populateCrcMap(&versMap, &vendor_crcs);
    }
    // else if we have a parsed kernel image, use the sorted ksymtab_strings table to
    // perform our patches.
    else if (parsedKernimg != 0)
    {
        populateKcrctab_ki(&versMap, parsedKernimg);
    }
    // else, the other way was to iterate a populated table and fill. This however doesn't work
    else
    {
        populateCrcKsymtab(&versMap, ksymBase, ksymCount, kcrcBase);
    }

    patchVersion(drverBase, &versMap);

    drvoutTmp = fopen(newDriver, "w");
    fwrite(drverBase, 1, drvSize, drvoutTmp);
    printf("patched file %s\n", newDriver);
fail:
    // SAFE_DEL(parsedKernimg);
    SAFE_FCLOSE(drvoutTmp);
    SAFE_FREE(drverBase);
    SAFE_FREE(vmlinuxBase);
    SAFE_FREE(kernimgBase);
    return 0;
}
