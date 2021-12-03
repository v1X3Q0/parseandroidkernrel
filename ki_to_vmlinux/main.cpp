#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <vector>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>
#include <parseAndKern.h>

int usage(const char* name)
{
    fprintf(stderr, "Usage: %s [-v vmlinux] [-k kernel_image]*\n",
        name);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    const char *newDriver = 0;
    const char* vmlinux_targ = 0;
    const char* kernimg_targ = 0;
    const char* vendorimg_path = 0;
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
    std::vector<Elf64_Shdr> sections;

    std::string vmlinux_header = "";
    const char* monitor = 0;

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
        default: /* '?' */
            usage(argv[0]);
        }
    }

    if (optind < argc)
    {

    }

    block_grab(kernimg_targ, (void**)&kernimgBase, NULL);
    
    parsedKernimg = new kern_img((uint32_t*)kernimgBase);

    vmlinux_header += 0x7f;
    monitor = vmlinux_header.data();
    

    return 0;
}