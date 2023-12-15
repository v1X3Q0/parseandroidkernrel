#pragma once

ELFBIT
int kern_static::gen_vmlinux_sz_p(size_t *outSz, size_t headOffset)
{
    size_t szTemp = 0;
    std::string *shstrtab_tmp;
    std::vector<std::pair<std::string, Elf_Shdr*>>* sect_list_l = (std::vector<std::pair<std::string, Elf_Shdr*>>*)&sect_list;

    // we are gonna consider that the header size SHOULD just be a page, however
    // under the condition that maybe we got a dummy thicc header, we allow it
    // to be passed in.
    // szTemp += headOffset;
    szTemp += sizeof(Elf_Ehdr) + sizeof(Elf_Phdr) * prog_list.size();

    // we are using the whole parsed kernel image as input, so add that.
    szTemp += kern_sz;

    // these two don't have to be true, but if they are they need to be added.
    // add sizes for new string table
    szTemp += newstrtabsz;
    // add sizes for symtab
    szTemp += newsymtabsz;

    // generate shstrtab
    gen_shstrtab(szTemp, &shstrtab_tmp, NULL, NULL);

    // adding the shstrtab to the image whole
    szTemp += shstrtab_tmp->size();

    // adding the section list to the array, plus 1 for the null section
    szTemp += ((sect_list_l->size() + 1) * sizeof(Elf_Shdr));

    *outSz = szTemp;
    SAFE_DEL(shstrtab_tmp);
    return 0;
}

ELFBIT
void kern_static::elfConstruction_p()
{
    int result = -1;
    Elf_Ehdr *vmlinuxBase = 0;
    char *kernimgBase = 0;
    size_t vmlinux_sz;
    std::string vmlinux_dir_made;

    kernel_symbol *ksymBase = 0;
    Elf_Phdr *phdrBase = 0;

    std::string *shstrtab_tmp;
    void *vmlinux_iter = 0;

    void* symtab = 0;
    size_t symtabsz = 0;

    // handle symtable
    // parsedKernimg->gen_strtab();
    gen_symtab(&symtab, &symtabsz);

    // alloc outfile
    gen_vmlinux_sz(&vmlinux_sz, PAGE_SIZE);

    if ((vmlinux_sz % PAGE_MASK4K) != 0)
    {
        vmlinux_sz = (vmlinux_sz + PAGE_SIZE4K) & ~PAGE_MASK4K;
    }
    result = posix_memalign((void **)&vmlinuxBase, PAGE_SIZE4K, vmlinux_sz);
    SAFE_BAIL(vmlinuxBase == 0);

    // write elf header to the new vmlinux
    elfHeadConstruction(vmlinuxBase);
    vmlinux_iter = (void *)((size_t)vmlinuxBase + sizeof(Elf_Ehdr));

    // write the new program header to the new vmlinux
    phdrBase = (Elf_Phdr *)vmlinux_iter;
    for (int i = 0; i < prog_list.size(); i++)
    {
        // TODO this seemed to be a line for setting the offset before it
        // is written to the target program header. I never figured out what
        // its value was supposed to be, but its something.
        // ((Elf_Phdr*)prog_list[i].second)->p_offset + sizeof(Elf_Ehdr) + sizeof(Elf_Phdr) * vmlinuxBase->e_phnum;
        memcpy(vmlinux_iter, (Elf_Phdr *)prog_list[i].second, sizeof(Elf_Phdr));
        vmlinux_iter = (void*)((size_t)vmlinux_iter + sizeof(Elf_Phdr));
    }
    // insert_phdr(PT_LOAD, PF_X | PF_W | PF_R, PAGE_SIZE4K, ANDROID_KERNBASE, ANDROID_KERNBASE,
    //     parsedKernimg->get_kernimg_sz(), parsedKernimg->get_kernimg_sz(), 0x10000);
    // patch_and_write_phdr((Elf64_Phdr*)vmlinux_iter, &g_phArray);
    // vmlinux_iter = (void *)((size_t)vmlinuxBase + PAGE_SIZE);

    // write the kernel image itself to the new vmlinux
    kernimgBase = (char *)vmlinux_iter;
    memcpy(vmlinux_iter, (void *)get_binbegin(), get_kernimg_sz());
    vmlinux_iter = (void *)((size_t)vmlinux_iter + get_kernimg_sz());

    // if there is a symbol table, update it
    memcpy(vmlinux_iter, symtab, symtabsz);

    // write the new shstrtab to vmlinux
    gen_shstrtab((size_t)NULL, &shstrtab_tmp, &vmlinuxBase->e_shnum, &vmlinuxBase->e_shstrndx);
    memcpy(vmlinux_iter, shstrtab_tmp->data(), shstrtab_tmp->size());
    vmlinux_iter = (void *)((size_t)vmlinux_iter + shstrtab_tmp->size());

    // patch the section header and write it to the binary, adjusting for
    // the program header and the elf header
    vmlinuxBase->e_shoff = ((size_t)vmlinux_iter - (size_t)vmlinuxBase);
    patch_and_write(vmlinuxBase, (Elf_Shdr *)vmlinux_iter, phdrBase, (size_t)kernimgBase - (size_t)vmlinuxBase);
fail:
    SAFE_DEL(shstrtab_tmp);
    SAFE_FREE(vmlinuxBase);
}

ELFBIT
void kern_static::elfHeadConstruction_p(void* elfHead_a)
{
    Elf_Ehdr* elfHead = (Elf_Ehdr*)elfHead_a;
    memset(elfHead, 0, sizeof(Elf_Ehdr));
    memcpy(&elfHead->e_ident[EI_MAG0], ELFMAG, sizeof(uint32_t));
    if (bitness == 32)
    {
        elfHead->e_ident[EI_CLASS] = ELFCLASS32;
    }
    else if (bitness == 64)
    {
        elfHead->e_ident[EI_CLASS] = ELFCLASS64;
    }

    // EM_AARCH64
    elfHead->e_machine = architecture;
    // ELFDATA2LSB;
    elfHead->e_ident[EI_DATA] = endianess;

    elfHead->e_ident[EI_VERSION] = EV_CURRENT;

    elfHead->e_type = ET_DYN;
    elfHead->e_version = EV_CURRENT;
    elfHead->e_phoff = sizeof(Elf_Ehdr);
    elfHead->e_flags = 0x602;
    elfHead->e_ehsize = sizeof(Elf_Ehdr);
    elfHead->e_phentsize = sizeof(Elf_Phdr);
    elfHead->e_phnum = prog_list.size();
    elfHead->e_shentsize = sizeof(Elf_Shdr);

    elfHead->e_shnum = sect_list.size();
    elfHead->e_shstrndx = sect_list.size() - 1;

    elfHead->e_entry = 0;
    elfHead->e_shoff = kern_sz + sizeof(Elf_Ehdr) + sizeof(Elf_Phdr);
}

// patch the out binary, section table base at vmlinux_cur and ph base at phBase
// int patch_and_write(Elf_Ehdr* vmlinux_base, Elf_Shdr* vmlinux_cur, Elf_Phdr* phBase, size_t offset);
ELFBIT
int kern_static::patch_and_write_p(void* vmlinux_base_a, void* vmlinux_cur_a, void* phBase_a, size_t offset)
{
    Elf_Ehdr* vmlinux_base = (Elf_Ehdr*)vmlinux_base_a;
    Elf_Shdr* vmlinux_cur = (Elf_Shdr*)vmlinux_cur_a;
    Elf_Phdr* phBase = (Elf_Phdr*)phBase_a;
    int result = -1;
    Elf_Phdr *phdrTemp = 0;
    int phdrCount = 0;
    std::vector<std::pair<std::string, Elf_Shdr*>>* sect_list_l = (std::vector<std::pair<std::string, Elf_Shdr*>>*)&sect_list;
    int sectlast = 0;

    memset(vmlinux_cur, 0, sizeof(Elf_Shdr));
    vmlinux_cur++;
    auto j = sect_list_l->begin();
    // skip null?
    size_t strlast = 0;
    size_t strlastsz = 0;
    j++;

    strlast = ((Elf_Shdr*)(find_sect(".shstrtab")))->sh_offset;
    if (newstrtab != 0)
    {
        ((Elf_Shdr*)(find_sect(".strtab")))->sh_offset = strlast - newstrtabsz;
        strlast = strlast - newstrtabsz;
    }
    if (newsymtab != 0)
    {
        ((Elf_Shdr*)(find_sect(".symtab")))->sh_offset = strlast - newstrtabsz;
    }

    for (auto i = sect_list_l->begin(); i != sect_list_l->end(); i++)
    {
        // fix the offset and size of the target segment
        i->second->sh_offset += offset;
        if ((i->second->sh_size == 0) && (j != sect_list_l->end()))
        {
            i->second->sh_size = (j->second->sh_offset + offset) - i->second->sh_offset;
        }

        // for this one I don't think I have to do anything, since it was appended correctly
        if (i->first == ".shstrtab")
        {

        }
        // meanwhile for the symbol table that was added, we have to readjust for the offset
        else if ((i->first == ".symtab") && (newsymtab != 0))
        {
            ((Elf_Shdr*)(find_sect(".symtab")))->sh_offset = ((Elf_Shdr*)(find_sect(".shstrtab")))->sh_offset - newsymtabsz;
        }
        // push the changes and the segment header
        memcpy(vmlinux_cur, i->second, sizeof(Elf_Shdr));

        // fix the program header if it has a match
        phdrTemp = (Elf_Phdr*)find_prog(i->first);
        if (phdrTemp != 0)
        {
            phdrTemp->p_offset = i->second->sh_offset;
            phdrTemp->p_filesz = phdrTemp->p_memsz = i->second->sh_size;
            memcpy(phBase, phdrTemp, sizeof(Elf_Phdr));
            phdrCount++;
            phBase++;
        }

        // increment the section header iterator that will be used for copying
        vmlinux_cur++;
        // increment the next iterator that will be used for the size calc
        if (j != sect_list_l->end())
        {
            j++;
        }
    }

    vmlinux_base->e_phnum = phdrCount;

    result = 0;
    return result;
}

ELFBIT
int kern_static::parseAndGetGlobals_p()
{
    int result = -1;
    std::vector<std::pair<std::string, Elf_Phdr*>>* prog_list_l = (std::vector<std::pair<std::string, Elf_Phdr*>>*)&prog_list;
    std::vector<std::pair<std::string, Elf_Shdr*>>* sect_list_l = (std::vector<std::pair<std::string, Elf_Shdr*>>*)&sect_list;

    vector_pair_sort<std::string, Elf_Shdr*>(sect_list_l, cmp_Shdr<Elf_Shdr>);
    vector_pair_sort<std::string, Elf_Phdr*>(prog_list_l, cmp_Phdr<Elf_Phdr>);

    result = 0;
fail:
    return result;
}

// default insertion function to be used, should be the only interface for adding new values
// void insert_section(std::string sec_name, uint16_t sh_type, uint64_t sh_flags,
//     uint64_t sh_addr, uint64_t sh_offset, uint64_t sh_size, uint16_t sh_link,
//     uint16_t sh_info, uint64_t sh_addralign, uint64_t sh_entsize);
// void insert_section(std::string sec_name, uint64_t sh_offset, uint64_t sh_size);
ELFBIT
void kern_static::insert_section_p(std::string sec_name, uint64_t sh_offset, uint64_t sh_size)
{
    Elf_Shdr *newShdr = 0;
    Elf_Phdr *newPhdr = 0;
    Elf_Word p_type = 0;
    Elf_Xword p_align = 0;
    Elf_Word p_flags = 0;

    uint16_t sh_type = SHT_PROGBITS;
    size_b sh_flags = 0;
    size_b sh_addr = sh_offset;
    // uint64_t sh_offset = 0;
    // uint64_t sh_size = 0;
    uint16_t sh_link = 0;
    uint16_t sh_info = 0;
    size_b sh_addralign = 0;
    size_b sh_entsize = 0;
    std::vector<std::pair<std::string, Elf_Shdr*>>* sect_list_l = (std::vector<std::pair<std::string, Elf_Shdr*>>*)&sect_list;
    std::vector<std::pair<std::string, Elf_Phdr*>>* prog_list_l = (std::vector<std::pair<std::string, Elf_Phdr*>>*)&prog_list;

    if (
        (sec_name != ".symtab") &&
        (sec_name != ".strtab") &&
        (sec_name != ".shstrtab"))
    {
        if (live_kernel == false)
        {
            sh_addr = R_KA(RESOLVE_REL(sh_addr));
        }
        sh_offset = RESOLVE_REL(sh_offset);
        p_type = PT_LOAD;
        p_align = 0x10000;

        sh_flags = SHF_ALLOC;
        p_flags = PF_R;
        // found a text, add executable flag
    }

    newShdr = new Elf_Shdr{
        0,
        sh_type,
        sh_flags,
        sh_addr,
        (size_b)sh_offset,
        (size_b)sh_size,
        sh_link,
        sh_info,
        sh_addralign,
        sh_entsize};

    sect_list_l->push_back({sec_name, newShdr});

    if (p_type == PT_LOAD)
    {
        newPhdr = new Elf_Phdr{
            p_type,
            p_flags,
            (size_b)sh_offset,
            sh_addr,
            sh_addr,
            (size_b)sh_size,
            (size_b)sh_size,
            (size_b)p_align};

        prog_list_l->push_back({sec_name, newPhdr});
    }
}

ELFBIT
void kern_static::insert_elfsegment_p(std::string sec_name, int permissions, uint64_t Virtual, uint64_t Physical, uint64_t FileOffset,  uint64_t sh_size)
{
    Elf_Shdr *newShdr = 0;
    Elf_Phdr *newPhdr = 0;
    size_b sh_addr = 0;
    Elf_Word p_type = 0;
    Elf_Xword p_align = 0;
    Elf_Word p_flags = 0;
    std::vector<std::pair<std::string, Elf_Phdr*>>* prog_list_l = (std::vector<std::pair<std::string, Elf_Phdr*>>*)&prog_list;

    if (
        (sec_name != ".symtab") &&
        (sec_name != ".strtab") &&
        (sec_name != ".shstrtab"))
    {
        if (live_kernel == false)
        {
            sh_addr = R_KA(RESOLVE_REL(sh_addr));
        }
        // sh_offset = RESOLVE_REL(sh_offset);
        p_type = PT_LOAD;
        p_align = 0x10000;
        p_flags = permissions;
        // found a text, add executable flag
    }

    if (p_type == PT_LOAD)
    {
        newPhdr = new Elf_Phdr{
            p_type,
            p_flags,
            (size_b)FileOffset,
            (Elf32_Addr)Virtual,
            (Elf32_Addr)Physical,
            (size_b)sh_size,
            (size_b)sh_size,
            (size_b)p_align};

        prog_list_l->push_back({sec_name, newPhdr});
    }
}

ELFBIT
void kern_static::insert_elfsection_p(std::string sec_name, uint64_t Virtual, uint64_t FileOffset,  uint64_t sh_size)
{
    Elf_Shdr *newShdr = 0;
    Elf_Phdr *newPhdr = 0;

    uint16_t sh_type = SHT_PROGBITS;
    size_b sh_flags = 0;
    size_b sh_addr = 0;
    uint16_t sh_link = 0;
    uint16_t sh_info = 0;
    size_b sh_addralign = 0;
    size_b sh_entsize = 0;
    std::vector<std::pair<std::string, Elf_Shdr*>>* sect_list_l = (std::vector<std::pair<std::string, Elf_Shdr*>>*)&sect_list;

    if (
        (sec_name != ".symtab") &&
        (sec_name != ".strtab") &&
        (sec_name != ".shstrtab"))
    {
        if (live_kernel == false)
        {
            sh_addr = R_KA(RESOLVE_REL(sh_addr));
        }
        
        sh_flags = SHF_ALLOC;
    }

    newShdr = new Elf_Shdr{
        0,
        sh_type,
        sh_flags,
        (Elf32_Addr)Virtual,
        (size_b)FileOffset,
        (size_b)sh_size,
        sh_link,
        sh_info,
        sh_addralign,
        sh_entsize};

    sect_list_l->push_back({sec_name, newShdr});
}

ELFBIT
void kern_static::insert_elfsymbol_p(std::string symname, int symtype, uint64_t symval, uint64_t offset)
{

}

ELFBIT
void kern_static::ref_elfsymbol_p(const char* symname, int symtype, uint64_t symval)
{
    
}

ELFBIT
int kern_static::kcrc_index_p(std::string symbol, uint32_t* kcrc)
{
    int result = -1;
    Elf_Shdr *kcrctab_sec = 0;
    uint32_t *kcrctab_base = 0;
    SAFE_BAIL(check_sect("__kcrctab", (void*)&kcrctab_sec) == -1);

    kcrctab_base = (uint32_t *)UNRESOLVE_REL(kcrctab_sec->sh_offset);

    for (int i = 0; i < kstrtab_sorted.size(); i++)
    {
        if (symbol == kstrtab_sorted[i])
        {
            *kcrc = kcrctab_base[i];
            goto finish;
        }
    }
    for (int i = 0; i < kstrtab_gpl_sorted.size(); i++)
    {
        if (symbol == kstrtab_gpl_sorted[i])
        {
            *kcrc = kcrctab_base[kstrtab_sorted.size() + i];
            goto finish;
        }
    }
    goto fail;

finish:
    result = 0;
fail:
    return result;
}

