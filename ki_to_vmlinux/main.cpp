#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <vector>
#include <libgen.h>
#include <linux/limits.h>

#include <hdeA64.h>
#include <ibeSet.h>
#include <localUtil.h>
#include <parseAndKern.h>
#include <drv_share.h>

#include <kernel_block.h>

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
    kern_static* parsedKernimg = 0;
    Elf64_Phdr* phdrBase = 0;
    
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
        default: /* '?' */
            usage(argv[0]);
        }
    }

    if (kernimg_targ == 0)
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

    parsedKernimg = kernel_block::allocate_kern_img<kern_static>(kernimg_targ);
    SAFE_BAIL(parsedKernimg == 0);

    // alloc outfile
    parsedKernimg->gen_vmlinux_sz(&vmlinux_sz, PAGE_SIZE);

    if ((vmlinux_sz % PAGE_MASK4K) != 0)
    {
        vmlinux_sz = (vmlinux_sz + PAGE_SIZE4K) & ~PAGE_MASK4K;
    }
    result = posix_memalign((void**)&vmlinuxBase, PAGE_SIZE4K, vmlinux_sz);
    SAFE_BAIL(vmlinuxBase == 0);

    // write elf header to the new vmlinux
    elfConstruction(vmlinuxBase);
    vmlinux_iter = (void*)((size_t)vmlinuxBase + sizeof(Elf64_Ehdr));

    // write the new program header to the new vmlinux
    phdrBase = (Elf64_Phdr*)vmlinux_iter;
    // insert_phdr(PT_LOAD, PF_X | PF_W | PF_R, PAGE_SIZE4K, ANDROID_KERNBASE, ANDROID_KERNBASE,
    //     parsedKernimg->get_kernimg_sz(), parsedKernimg->get_kernimg_sz(), 0x10000);
    // patch_and_write_phdr((Elf64_Phdr*)vmlinux_iter, &g_phArray);
    vmlinux_iter = (void*)((size_t)vmlinuxBase + PAGE_SIZE);

    // write the kernel image itself to the new vmlinux
    kernimgBase = (char*)vmlinux_iter;
    memcpy(vmlinux_iter, (void*)parsedKernimg->get_binbegin(), parsedKernimg->get_kernimg_sz());
    vmlinux_iter = (void*)((size_t)vmlinux_iter + parsedKernimg->get_kernimg_sz());
    
    // write the new shstrtab to vmlinux
    parsedKernimg->gen_shstrtab(&shstrtab_tmp, &vmlinuxBase->e_shnum, &vmlinuxBase->e_shstrndx);
    memcpy(vmlinux_iter, shstrtab_tmp->data(), shstrtab_tmp->size());
    vmlinux_iter = (void*)((size_t)vmlinux_iter + shstrtab_tmp->size());

    // patch the section header and write it to the binary, adjusting for
    // the program header and the elf header
    vmlinuxBase->e_shoff = ((size_t)vmlinux_iter - (size_t)vmlinuxBase);
    parsedKernimg->patch_and_write(vmlinuxBase, (Elf64_Shdr*)vmlinux_iter, phdrBase, (size_t)kernimgBase - (size_t)vmlinuxBase);

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