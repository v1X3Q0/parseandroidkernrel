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
#include <drv_share.h>

int usage(const char* name)
{
    fprintf(stderr, "Usage: %s [-v vmlinux] [-k kernel_image]*\n",
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
    elfHead->e_phentsize = sizeof(Elf64_Phdr);
    elfHead->e_shentsize = sizeof(Elf64_Shdr);
}

void progHeadConstruction(Elf64_Phdr* phHead)
{
    memset(phHead, 0, sizeof(Elf64_Phdr));
    phHead->p_type = PT_LOAD;
    phHead->p_offset = PAGE_SIZE;
    phHead->p_vaddr = ANDROID_KERNBASE;
    phHead->p_paddr = ANDROID_KERNBASE;
    phHead-> p_flags = PF_X | PF_W | PF_R;
    phHead->p_align = 0x10000;
}

int main(int argc, char **argv)
{
    const char* vmlinux_targ = 0;
    const char* kernimg_targ = 0;
    char *vmlinuxBase = 0;
    char *kernimgBase = 0;
    size_t vmlinux_sz;

    kernel_symbol* ksymBase = 0;
    size_t ksymCount = 0;
    uint32_t* kcrcBase = 0;
    kern_img* parsedKernimg = 0;
    
    std::string shstrtab_tmp;
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
        default: /* '?' */
            usage(argv[0]);
        }
    }

    parsedKernimg = new kern_img(kernimg_targ);

    // alloc outfile
    parsedKernimg->gen_vmlinux_sz(&vmlinux_sz, PAGE_SIZE);
    posix_memalign((void**)&vmlinuxBase, PAGE_SIZE4K, vmlinux_sz);

    // write elf header to the new vmlinux
    elfConstruction((Elf64_Ehdr*)vmlinuxBase);
    vmlinux_iter = (void*)(vmlinuxBase + sizeof(Elf64_Ehdr));

    // write the new program header to the new vmlinux
    progHeadConstruction((Elf64_Phdr*)vmlinux_iter);
    vmlinux_iter = vmlinuxBase + PAGE_SIZE;

    // write the kernel image itself to the new vmlinux
    kernimgBase = (char*)vmlinux_iter;
    memcpy(vmlinux_iter, kernimg_targ, parsedKernimg->get_kernimg_sz());
    vmlinux_iter = (void*)(vmlinux_iter + parsedKernimg->get_kernimg_sz());
    
    // write the new shstrtab to vmlinux
    shstrtab_tmp = parsedKernimg->gen_shstrtab();
    memcpy(vmlinux_iter, shstrtab_tmp.data(), shstrtab_tmp.size());
    vmlinux_iter = (void*)(vmlinux_iter + shstrtab_tmp.size());

    // patch the section header and write it to the binary, adjusting for
    // the program header and the elf header
    parsedKernimg->patch_and_write(vmlinux_iter, vmlinuxBase - kernimgBase);


    return 0;
}