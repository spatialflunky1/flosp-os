#include <loader.h>

EFI_STATUS load_kernel(EFI_FILE_PROTOCOL* RootFileSystem, CHAR16* KernelFilename, EFI_PHYSICAL_ADDRESS* KernelEntryPoint, EFI_SYSTEM_TABLE* SystemTable) {
    EFI_STATUS status = EFI_SUCCESS;
    EFI_FILE_PROTOCOL* KernelImage;
    UINT8* ELFIDBuffer = NULL;
    void* KernelHeader = NULL;
    void* KernelProgramHeaders = NULL;

    #ifdef INFO
        efi_print(SystemTable, L"Info: Loading kernel\r\n");
    #endif
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Opening kernel file\r\n");
    #endif
    status = RootFileSystem->Open(
            (struct EFI_FILE_PROTOCOL*)RootFileSystem, 
            (struct EFI_FILE_PROTOCOL**)&KernelImage, 
            KernelFilename, 
            EFI_FILE_MODE_READ, 
            EFI_FILE_READ_ONLY);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error opening kernel file\r\n");
        return status;
    }

    // Read the ELF identity from the kernel file
    status = read_elf_identity(SystemTable, KernelImage, &ELFIDBuffer);
    if (EFI_ERROR(status)) {
        // Error printed within function call above
        return status;
    }

    // Validate the ELF identity from the kernel file
    status = validate_elf_identity(SystemTable, ELFIDBuffer);
    if (EFI_ERROR(status)) {
        // Error printed within function call above
        return status;
    }

    // Free the ELF identity buffer, allocated in read_elf_identity()
    status = SystemTable->BootServices->FreePool(ELFIDBuffer);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Unable to free kernel ELF identity buffer\r\n");
        return status;
    }

    // Read the ELF header and ELF program headers from the kernel file
    status = read_elf_file(SystemTable, KernelImage, &KernelHeader, &KernelProgramHeaders);
    if (EFI_ERROR(status)) {
        // Error printed within function call above
        return status;
    }
    *KernelEntryPoint = ((Elf64_Ehdr*)KernelHeader)->e_entry;

    // Debug: print the kernel headers
    #ifdef DEBUG
        print_elf_info(SystemTable, KernelHeader, KernelProgramHeaders);
    #endif

    // Load the kernel memory segments
    status = load_program_segments(SystemTable, KernelImage, KernelHeader, KernelProgramHeaders);
    if (EFI_ERROR(status)) {
        // Error printed within function call above
        return status;
    }

    //
    // Cleanup
    //
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Closing kernel file\r\n");
    #endif
    status = KernelImage->Close((struct EFI_FILE_PROTOCOL*)KernelImage);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error closing kernel file\r\n");
        return status;
    }

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Freeing kernel ELF header buffer\r\n");
    #endif
    status = SystemTable->BootServices->FreePool(KernelHeader);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error freeing kernel ELF header buffer\r\n");
        return status;
    }

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Freeing kernel ELF program headers buffer\r\n");
    #endif
    status = SystemTable->BootServices->FreePool(KernelProgramHeaders);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error freeing kernel ELF program headers buffer\r\n");
        return status;
    }

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Kernel loaded\r\n");
    #endif
    return status;
}

EFI_STATUS load_segment(EFI_SYSTEM_TABLE* SystemTable, EFI_FILE_PROTOCOL* KernelImage, EFI_PHYSICAL_ADDRESS SegmentOffset, UINT64 SegmentFileSize, UINT64 SegmentMemorySize, EFI_PHYSICAL_ADDRESS SegmentPhysicalAddress) {
    EFI_STATUS status = EFI_SUCCESS;
    UINT64 SegmentPageCount = EFI_SIZE_TO_PAGES(SegmentMemorySize);
    UINT64 BufferSize = 0;
    void* ProgramData = NULL;
    // Segment zero fill when size in memory is greater than the file size
    EFI_PHYSICAL_ADDRESS ZeroFillStart = 0;
    UINT64 ZeroFillCount = 0;

    // Set kernel file pointer to segment offset
    status = KernelImage->SetPosition((struct EFI_FILE_PROTOCOL*)KernelImage, SegmentOffset);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error setting kernel file pointer position\r\n");
        return status;
    }

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Allocating pages for ELF segment\r\n");
    #endif
    status = SystemTable->BootServices->AllocatePages(
            AllocateAddress,
            EfiLoaderData,
            SegmentPageCount,
            (EFI_PHYSICAL_ADDRESS*)&SegmentPhysicalAddress);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error allocating pages for ELF segment\r\n");
        return status;
    }

    if (SegmentFileSize > 0) {
        BufferSize = SegmentFileSize;

        #ifdef DEBUG
            efi_print(SystemTable, L"Debug: Allocating kernel program segment buffer\r\n");
        #endif
        status = SystemTable->BootServices->AllocatePool(
                EfiLoaderData, 
                BufferSize, 
                (void**)&ProgramData);
        if (EFI_ERROR(status)) {
            efi_print(SystemTable, L"Fatal: Error allocating kernel program segment buffer\r\n");
            return status;
        }

        #ifdef DEBUG
            efi_print(SystemTable, L"Debug: Reading segment data\r\n");
        #endif
        status = KernelImage->Read(
                (struct EFI_FILE_PROTOCOL*)KernelImage, 
                &BufferSize, 
                (void*)ProgramData);
        if (EFI_ERROR(status)) {
            efi_print(SystemTable, L"Fatal: Error reading segment data\r\n");
            return status;
        }

        #ifdef DEBUG
            efi_print(SystemTable, L"Debug: Copying segment data to memory\r\n");
        #endif
        status = SystemTable->BootServices->CopyMem(
                (void*)SegmentPhysicalAddress,
                ProgramData,
                SegmentFileSize);
        if (EFI_ERROR(status)) {
            efi_print(SystemTable, L"Fatal: Error copying segment data to memory\r\n");
            return status;
        }

        #ifdef DEBUG
            efi_print(SystemTable, L"Debug: Freeing kernel program segment buffer\r\n");
        #endif
        status = SystemTable->BootServices->FreePool(ProgramData);
        if (EFI_ERROR(status)) {
            efi_print(SystemTable, L"Fatal: Error freeing kernel program segment buffer\r\n");
            return status;
        }
    }

    // The ELF standard requires the segment to be zero filled if the size in memory
    // is larger than the file size (ELF standard page 34)
    ZeroFillStart = SegmentPhysicalAddress + SegmentFileSize;
    ZeroFillCount = SegmentMemorySize - SegmentFileSize;
    if (ZeroFillCount > 0) { 
        #ifdef DEBUG
            efi_print(SystemTable, L"Debug: Zero-filling extended data after program segment\r\n");
        #endif
        status = SystemTable->BootServices->SetMem((void*)ZeroFillStart, ZeroFillCount, 0);
        if (EFI_ERROR(status)) {
            efi_print(SystemTable, L"Fatal: Error zero-filling memory post data segment\r\n");
            return status;
        }
    }

    return status;
}

EFI_STATUS load_program_segments(EFI_SYSTEM_TABLE* SystemTable, EFI_FILE_PROTOCOL* KernelImage, void* KernelHeader, void* KernelProgramHeaders) {
    EFI_STATUS status = EFI_SUCCESS;
    UINT16 NumProgramHeaders = ((Elf64_Ehdr*)KernelHeader)->e_phnum;
    UINT16 NumSegmentsLoaded = 0;
    Elf64_Phdr* ProgramHeaders = (Elf64_Phdr*)KernelProgramHeaders;
    
    // Check for segments
    if (NumProgramHeaders == 0) {
        efi_print(SystemTable, L"Fatal: No program segments found within kernel file\r\n");
        return EFI_INVALID_PARAMETER;
    }

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Loading kernel segments\r\n");
    #endif
    for (int i = 0; i < NumProgramHeaders; i++) {
        if (ProgramHeaders[i].p_type == PT_LOAD) {
            status = load_segment(
                    SystemTable,
                    KernelImage,
                    ProgramHeaders[i].p_offset,
                    ProgramHeaders[i].p_filesz,
                    ProgramHeaders[i].p_memsz,
                    ProgramHeaders[i].p_paddr);
            if (EFI_ERROR(status)) {
                // Error printed within function call above
                return status;
            }
            NumSegmentsLoaded++;
        }
    }

    // Check if no loadable segments found
    if (NumSegmentsLoaded == 0) {
        efi_print(SystemTable, L"Fatal: No loadable program segments found within kernel file\r\n");
        return EFI_NOT_FOUND;
    }

    return status;
}
