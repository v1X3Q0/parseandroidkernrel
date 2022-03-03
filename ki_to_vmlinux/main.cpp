#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <vector>
#include <libgen.h>
#include <linux/limits.h>

#include <ibeSet.h>
#include <localUtil.h>
#include <kern_static.h>
#include <drv_share.h>

#include <kernel_block.h>

#ifdef LIVE_KERNEL
#include <hdeA64.h>
#else
#include <capstone/capstone.h>
#endif

int usage(const char* name)
{
    fprintf(stderr, "Usage: %s [-v vmlinux] [-k kernel_image] [-m bitness]\n",
        name);
    exit(EXIT_FAILURE);
}

void elfConstruction(Elf64_Ehdr* elfHead)
{
    memset(elfHead, 0, sizeof(Elf64_Ehdr));
    memcpy(&elfHead->e_ident[EI_MAG0], ELFMAG, sizeof(uint32_t));
    elfHead->e_ident[EI_CLASS] = ELFCLASS64;
    elfHead->e_ident[EI_DATA] = ELFDATA2LSB;
    elfHead->e_ident[EI_VERSION] = EV_CURRENT;

    elfHead->e_type = ET_DYN;
    elfHead->e_machine = EM_AARCH64;
    elfHead->e_version = EV_CURRENT;
    elfHead->e_entry = ANDROID_KERNBASE;
    elfHead->e_phoff = sizeof(Elf64_Ehdr);
    // elfHead->e_shoff
    elfHead->e_flags = 0x602;
    elfHead->e_ehsize = sizeof(Elf64_Ehdr);
    elfHead->e_phentsize = sizeof(Elf64_Phdr);
    // elfHead->e_phnum
    elfHead->e_shentsize = sizeof(Elf64_Shdr);
    // elfHead->e_shnum
    // elfHead->e_shstrndx
}

std::vector<Elf64_Phdr> g_phArray;

void insert_phdr(Elf64_Word p_type, Elf64_Word p_flags, Elf64_Off p_offset,
    Elf64_Addr p_vaddr, Elf64_Addr p_paddr, Elf64_Xword p_filesz, Elf64_Xword p_memsz,
    Elf64_Xword p_align)
{
    g_phArray.push_back({p_type, p_flags, p_offset, p_vaddr, p_vaddr, p_filesz, p_memsz,
        p_align});
}

void patch_and_write_phdr(Elf64_Phdr* vmlinux_phBase, std::vector<Elf64_Phdr>* phArray)
{
    for (auto i = phArray->begin(); i != phArray->end(); i++)
    {
        memcpy(vmlinux_phBase, &(*i), sizeof(Elf64_Phdr));
    }
}

void progHeadConstruction(Elf64_Phdr* phHead, size_t imageSz)
{
    memset(phHead, 0, sizeof(Elf64_Phdr));
    phHead->p_type = PT_LOAD;
    phHead-> p_flags = PF_X | PF_W | PF_R;
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
    kern_img* parsedKernimg = 0;
    Elf64_Phdr* phdrBase = 0;

    size_t bitness_local = 64;
    
    std::string* shstrtab_tmp;
    void* vmlinux_iter = 0;

    int opt = 0;

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
        case 'm':
            bitness_local = atoi(optarg);
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

    parsedKernimg = allocate_static_kernel(kernimg_targ, bitness_local);
    SAFE_BAIL(parsedKernimg == 0);

    out_vmlinux = fopen(vmlinux_targ, "w");
    SAFE_BAIL(out_vmlinux == 0);
    fwrite(vmlinuxBase, 1, vmlinux_sz, out_vmlinux);

    printf("created new vmlinux %s\n", vmlinux_targ);

    result = 0;
fail:
    SAFE_FCLOSE(out_vmlinux);
    SAFE_DEL(parsedKernimg);
    SAFE_FREE(vmlinuxBase);
    return result;
}