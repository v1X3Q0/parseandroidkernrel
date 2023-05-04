#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <vector>
#include <libgen.h>
// #include <linux/limits.h>

#include <localUtil.h>
#include <kern_static.h>
#include <drv_share.h>

#include <kernel_block.h>

int usage(const char* name)
{
    fprintf(stderr, "Usage: %s [-o vmlinux_out] [-k kernel_image] [-m bitness] [-a arch] [-f family] [-d device]\n",
        name);
    exit(EXIT_FAILURE);
}

int pull_target_parameters(const char* target_a);

void progHeadConstruction(Elf64_Phdr* phHead, size_t imageSz)
{
    memset(phHead, 0, sizeof(Elf64_Phdr));
    phHead->p_type = PT_LOAD;
    phHead->p_flags = PF_X | PF_W | PF_R;
    phHead->p_offset = PAGE_SIZE4K;
    phHead->p_vaddr = ANDROID_KERNBASE;
    phHead->p_paddr = ANDROID_KERNBASE;
    phHead->p_filesz = imageSz;
    phHead->p_memsz = imageSz;
    phHead->p_align = 0x10000;
}

int main(int argc, char **argv)
{
    int result = -1;
    const char* vmlinux_targ = 0;
    const char* kernimg_targ = 0;
    Elf64_Ehdr* vmlinuxBase = 0;
    char *kernimgBase = 0;
    size_t vmlinux_sz;
    char vmlinux_dir_copy[PATH_MAX] = { 0 };
    std::string vmlinux_dir_made;
    FILE* out_vmlinux = 0;

    kernel_symbol* ksymBase = 0;
    size_t ksymCount = 0;
    uint32_t* kcrcBase = 0;
    kernel_linux* parsedKernimg = 0;
    Elf64_Phdr* phdrBase = 0;

    const char* target_family = 0;
    const char* target_device = 0;
    const char* target_arch = 0;
    const char* target_version = 0;
    const char* target_config = 0;

    size_t bitness_local = 64;
    
    std::string* shstrtab_tmp;
    void* vmlinux_iter = 0;

    int opt = 0;

    const char* monitor = 0;

    while ((opt = getopt(argc, argv, "o:k:m:a:f:d:")) != -1)
    {
        switch (opt)
        {
        case 'o':
            vmlinux_targ = optarg;
            break;
        case 'k':
            kernimg_targ = optarg;
            break;
        case 'm':
            bitness_local = atoi(optarg);
            break;
        case 'a':
            target_arch = optarg;
            break;
        case 'f':
            target_family = optarg;
            break;
        case 'd':
            target_device = optarg;
            break;
        case 'j':
            target_config = optarg;
            break;
        case 'v':
            target_version = optarg;
            break;
        default: /* '?' */
            usage(argv[0]);
        }
    }

    if (kernimg_targ == 0)
    {
        usage(argv[0]);
    }

    if ((bitness_local != 64) && (bitness_local != 32))
    {
        usage(argv[0]);
    }

    if (vmlinux_targ == 0)
    {
        strcpy(vmlinux_dir_copy, kernimg_targ);
        dirname(vmlinux_dir_copy);
        vmlinux_dir_made = vmlinux_dir_copy;
        vmlinux_dir_made += "/vmlinux";
        vmlinux_targ = vmlinux_dir_made.data();
    }

    if (target_config != 0)
    {
        pull_target_parameters(target_config);
    }

    parsedKernimg = allocate_static_kernel(kernimg_targ,
        bitness_local);
    SAFE_BAIL(parsedKernimg == 0);

    out_vmlinux = fopen(vmlinux_targ, "w");
    SAFE_BAIL(out_vmlinux == 0);
    fwrite(vmlinuxBase, 1, vmlinux_sz, out_vmlinux);

    printf("created new vmlinux %s\n", vmlinux_targ);

    result = 0;
fail:
    SAFE_FCLOSE(out_vmlinux);
    // SAFE_DEL(parsedKernimg);
    SAFE_FREE(vmlinuxBase);
    return result;
}