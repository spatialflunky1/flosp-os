#include <elf.h>

EFI_STATUS read_elf_identity(EFI_SYSTEM_TABLE* SystemTable, EFI_FILE_PROTOCOL* KernelImage, UINT8** ELFIDBuffer) {
    EFI_STATUS status = EFI_SUCCESS;
    UINT64 BufferSize = EI_NIDENT;

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Reading kernel ELF identity\r\n");
    #endif

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Resetting kernel file pointer to beginning of file\r\n");
    #endif
    status = KernelImage->SetPosition(
            (struct EFI_FILE_PROTOCOL*)KernelImage, 
            0);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error resetting file pointer position\r\n");
        return status;
    }
    efi_print(SystemTable, L"Test print\r\n");

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Allocating buffer for ELF identity\r\n");
    #endif
    status = SystemTable->BootServices->AllocatePool(
            EfiLoaderData, 
            EI_NIDENT, 
            (void**)ELFIDBuffer);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error allocating memory for ELF identity buffer\r\n");
        return status;
    }

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Reading kernel ELF identity\r\n");
    #endif
    status = KernelImage->Read(
            (struct EFI_FILE_PROTOCOL*)KernelImage, 
            &BufferSize, 
            (void*)*ELFIDBuffer);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error reading kernel ELF identity\r\n");
        return status;
    }
    return status;
}

EFI_STATUS validate_elf_identity(EFI_SYSTEM_TABLE* SystemTable, UINT8* ELFIDBuffer) {
    EFI_STATUS status = EFI_SUCCESS;

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Validating kernel ELF identity\r\n");
    #endif
    // Magic portion of header should contain 0x7F followed by "ELF" in ASCII
    if (
            (ELFIDBuffer[EI_MAG0] != 0x7F) || 
            (ELFIDBuffer[EI_MAG1] != 'E')  ||
            (ELFIDBuffer[EI_MAG2] != 'L')  ||
            (ELFIDBuffer[EI_MAG3] != 'F')
            ) {
        efi_print(SystemTable, L"Fatal: Invalid kernel ELF header\r\n");
        return status;
    }
    // Check for 64 bit kernel
    if (ELFIDBuffer[EI_CLASS] == ELF_FILE_CLASS_64) {
        #ifdef DEBUG
            efi_print(SystemTable, L"Debug: Identified kernel as x86_64\r\n");
        #endif
    }
    else {
        efi_print(SystemTable, L"Fatal: Invalid kernel file\r\n");
        return EFI_UNSUPPORTED;
    }
    // Check that the ELF executable kernel is LSB format
    if (ELFIDBuffer[EI_DATA] != 1) {
        efi_print(SystemTable, L"Fatal: Only an LSB ELF kernel is supported\r\n");
        return EFI_INCOMPATIBLE_VERSION;
    }
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Kernel ELF identity validated\r\n");
    #endif
    return status;
}

EFI_STATUS read_elf_file(EFI_SYSTEM_TABLE* SystemTable, EFI_FILE_PROTOCOL* KernelImage, void** KernelHeader, void** KernelProgramHeaders) {
    EFI_STATUS status = EFI_SUCCESS;
    UINT64 BufferSize = sizeof(Elf64_Ehdr);
    UINT64 ProgramHeadersOffset = 0;

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Reading kernel ELF file\r\n");
    #endif
    
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Resetting kernel file pointer to beginning of file\r\n");
    #endif
    status = KernelImage->SetPosition((struct EFI_FILE_PROTOCOL*)KernelImage,0);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error resetting file pointer position\r\n");
        return status;
    }

    ///
    /// Read Kernel ELF Header
    ///
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Allocating buffer for kernel ELF header\r\n");
    #endif
    status = SystemTable->BootServices->AllocatePool(
            EfiLoaderData, 
            BufferSize, 
            KernelHeader);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error allocating memory for kernel ELF header\r\n");
        return status;
    }

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Reading kernel ELF header\r\n");
    #endif

    status = KernelImage->Read(
            (struct EFI_FILE_PROTOCOL*)KernelImage, 
            &BufferSize, 
            *KernelHeader);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error reading kernel ELF header\r\n");
        return status;
    }

    ///
    /// Read Kernel ELF Program Headers
    ///
    ProgramHeadersOffset = ((Elf64_Ehdr*)*KernelHeader)->e_phoff;
    BufferSize = sizeof(Elf64_Phdr) * ((Elf64_Ehdr*)*KernelHeader)->e_phnum;
    
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Setting kernel file pointer to program header offset\r\n");
    #endif
    status = KernelImage->SetPosition((struct EFI_FILE_PROTOCOL*)KernelImage, ProgramHeadersOffset);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error setting kernel file pointer position\r\n");
        return status;
    }

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Allocating buffer for kernel ELF program headers\r\n");
    #endif
    status = SystemTable->BootServices->AllocatePool(
            EfiLoaderData,
            BufferSize,
            KernelProgramHeaders);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error allocating memory for kernel ELF program headers\r\n");
        return status;
    }

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Reading kernel ELF program headers\r\n");
    #endif
    status = KernelImage->Read(
            (struct EFI_FILE_PROTOCOL*)KernelImage, 
            &BufferSize, 
            *KernelProgramHeaders);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error reading kernel ELF program headers\r\n");
        return status;
    }

    return status;
}

void print_elf_info(EFI_SYSTEM_TABLE* SystemTable, void* KernelHeader, void* KernelProgramHeaders) {
    Elf64_Ehdr* header = (Elf64_Ehdr*)KernelHeader;
    efi_print(SystemTable, L"Debug: Kernel ELF header info:\r\n");
    
    // Magic
    efi_print(SystemTable, L"  Magic: ");
    for (int i = 0; i <= 3; i++) {
        efi_printhex(SystemTable, header->e_ident[i], 0);
        efi_print(SystemTable, L" ");
    }
    efi_print(SystemTable, L"\r\n");
    // Class
    efi_print(SystemTable, L"  Class: ");
    efi_printhex(SystemTable, header->e_ident[EI_CLASS], 1);
    efi_print(SystemTable, L"\r\n");
    // Data
    efi_print(SystemTable, L"  Data: ");
    efi_printhex(SystemTable, header->e_ident[EI_DATA], 1); 
    efi_print(SystemTable, L"\r\n");
    // Version
    efi_print(SystemTable, L"  Version: ");
    efi_printhex(SystemTable, header->e_ident[EI_VERSION], 1);
    efi_print(SystemTable, L"\r\n");
    // OS/ABI
    efi_print(SystemTable, L"  OS/ABI: ");
    efi_printhex(SystemTable, header->e_ident[EI_OSABI], 1);
    efi_print(SystemTable, L"\r\n");
    // ABI Version
    efi_print(SystemTable, L"  ABI Version: ");
    efi_printhex(SystemTable, header->e_ident[EI_ABIVERSION], 1);
    efi_print(SystemTable, L"\r\n");
    // Type
    efi_print(SystemTable, L"  Type: ");
    efi_printhex(SystemTable, header->e_type, 1);
    efi_print(SystemTable, L"\r\n");
    // Machine
    efi_print(SystemTable, L"  Machine: ");
    efi_printhex(SystemTable, header->e_machine, 1);
    efi_print(SystemTable, L"\r\n");
    // Version
    efi_print(SystemTable, L"  Version: ");
    efi_printhex(SystemTable, header->e_version, 1);
    efi_print(SystemTable, L"\r\n");
    // Entry point address
    efi_print(SystemTable, L"  Entry point address: ");
    efi_printhex(SystemTable, header->e_entry, 1);
    efi_print(SystemTable, L"\r\n");
    // Start of program headers
    efi_print(SystemTable, L"  Start of program headers: ");
    efi_printhex(SystemTable, header->e_phoff, 1);
    efi_print(SystemTable, L"\r\n");
    // Start of section headers
    efi_print(SystemTable, L"  Start of section headers: ");
    efi_printhex(SystemTable, header->e_shoff, 1);
    efi_print(SystemTable, L"\r\n");
    // Flags
    efi_print(SystemTable, L"  Flags: ");
    efi_printhex(SystemTable, header->e_flags, 1);
    efi_print(SystemTable, L"\r\n");
    // Size of this header
    efi_print(SystemTable, L"  Size of this header: ");
    efi_printhex(SystemTable, header->e_ehsize, 1);
    efi_print(SystemTable, L"\r\n");
    // Size of program headers
    efi_print(SystemTable, L"  Size of program headers: ");
    efi_printhex(SystemTable, header->e_phentsize, 1);
    efi_print(SystemTable, L"\r\n");
    // Number of program headers
    efi_print(SystemTable, L"  Number of program headers: ");
    efi_printnum(SystemTable, header->e_phnum);
    efi_print(SystemTable, L"\r\n");
    // Size of section headers
    efi_print(SystemTable, L"  Size of section headers: ");
    efi_printhex(SystemTable, header->e_shentsize, 1);
    efi_print(SystemTable, L"\r\n");
    // Number of section headers
    efi_print(SystemTable, L"  Number of section headers: ");
    efi_printnum(SystemTable, header->e_shnum);
    efi_print(SystemTable, L"\r\n");
    // Section header string table index
    efi_print(SystemTable, L"  Section header string table index: ");
    efi_printhex(SystemTable, header->e_shstrndx, 1);
    efi_print(SystemTable, L"\r\n");
}
