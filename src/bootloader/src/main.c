#include <efi.h>
#include <fs.h>
#include <video.h>
#include <loader.h>

#define KERNEL_FILEPATH L"\\kernel.elf"

static EFI_STATUS get_memory_map(
        EFI_SYSTEM_TABLE* SystemTable,
        void**            MemoryMap,
        UINT64*           MemoryMapSize,
        UINT64*           MemoryMapKey,
        UINT64*           DescriptorSize,
        UINT32*           DescriptorVersion) 
{
    EFI_STATUS status = EFI_SUCCESS;
    
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Allocating memory map\r\n");
    #endif
    status = SystemTable->BootServices->GetMemoryMap(
            MemoryMapSize, 
            *MemoryMap, 
            MemoryMapKey, 
            DescriptorSize, 
            DescriptorVersion);
    if (EFI_ERROR(status)) {
        // Always fails first attempt
        if (status != EFI_BUFFER_TOO_SMALL) {
            efi_print(SystemTable, L"Fatal: Error getting memory map size\r\n");
            return status;
        }
    }
    // Up to 2 new descriptors may be created in the process of allocating the new pool memory
    *MemoryMapSize += 2 * (*DescriptorSize);
    status = SystemTable->BootServices->AllocatePool(
            EfiLoaderData,
            *MemoryMapSize,
            (void**)MemoryMap);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error allocating memory map buffer\r\n");
        return status;
    }
    // Get memory map after buffer allocation
    status = SystemTable->BootServices->GetMemoryMap(
            MemoryMapSize,
            *MemoryMap,
            MemoryMapKey,
            DescriptorSize,
            DescriptorVersion);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error getting memory map\r\n");
        return status;
    }

    return status;
}

// Definition from gnu-efi
static INT64 compare_efi_guid(EFI_GUID* guid1_ptr, EFI_GUID* guid2_ptr) {
    INT32 *g1, *g2, r;
    
    g1 = (INT32*) guid1_ptr;
    g2 = (INT32*) guid2_ptr;

    r  = g1[0] - g2[0];
    r |= g1[1] - g2[1];
    r |= g1[2] - g2[2];
    r |= g1[3] - g2[3];

    return r;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    // Bootloader status
    EFI_STATUS                       status                 = EFI_SUCCESS;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystemProtocol     = NULL;
    EFI_FILE_PROTOCOL*               RootFileSystem         = NULL;
    EFI_PHYSICAL_ADDRESS*            KernelEntryPoint       = NULL;
    KERNEL_BOOT_INFO                 BootInfo;
    EFI_MEMORY_DESCRIPTOR*           MemoryMap              = NULL;
    UefiGraphicsService              GraphicsService;
    EFI_GRAPHICS_OUTPUT_PROTOCOL*    GraphicsOutputProtocol = NULL;
    UINT64                           MemoryMapSize          = 0;
    UINT64                           MemoryMapKey           = 0;
    UINT64                           DescriptorSize         = 0;
    UINT32                           DescriptorVersion      = 0;
    // Kernel entry function pointer
    void (*kernel_entry)(KERNEL_BOOT_INFO* BootInfo);

    efi_print(SystemTable, L"Booting Flosp...\r\n");
 
    //
    // Disable watchdog timer (5 min inactivity timeout -> reboot)
    //
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Disabling watchdog\r\n");
    #endif
    status = SystemTable->BootServices->SetWatchdogTimer(0,0,0,NULL);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error disabling watchdog timer\r\n");
        return status;
    }

    //
    // Initialize Graphics Output Protocol
    //
    status = graphics_init(SystemTable, &GraphicsService);
    if (EFI_ERROR(status)) {
        if (status != EFI_NOT_FOUND) {
            efi_print(SystemTable, L"Fatal: Error initializing graphics service\r\n");
            return status;
        }
        #ifdef DEBUG
        else {
            efi_print(SystemTable, L"Debug: Unable to locate graphics device\r\n");
        }
        #endif
        return status;
    }

    // Open the graphics output protocol from the active console's handle
    status = SystemTable->BootServices->OpenProtocol(
            GraphicsService.handle_buffer[0],
            &gEfiGraphicsOutputProtocolGuid,
            (void**)&GraphicsOutputProtocol,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Failed to open graphics output protocol on the current console\r\n");
        while(1);
    }
    
    if (status == EFI_NOT_STARTED || !GraphicsOutputProtocol->Mode) {
        status = set_graphics_mode(
                SystemTable, 
                GraphicsOutputProtocol, 
                TARGET_SCREEN_WIDTH, 
                TARGET_SCREEN_HEIGHT, 
                TARGET_SCREEN_PIXEL_FORMAT);
        if (EFI_ERROR(status)) {
            // Error printed within function call above
            return status;
        }
    }

    if ((void*)GraphicsOutputProtocol->Mode->FrameBufferBase == NULL) {
        efi_print(SystemTable, L"Fatal: Framebuffer address is NULL");
        while(1);
    }

    // Save graphics info in BootInfo
    BootInfo.VideoModeInfo.FramebufferPointer =
        (void*)GraphicsOutputProtocol->Mode->FrameBufferBase;

    BootInfo.VideoModeInfo.HorizontalResolution =
        GraphicsOutputProtocol->Mode->Info->HorizontalResolution;

    BootInfo.VideoModeInfo.VerticalResolution = 
        GraphicsOutputProtocol->Mode->Info->VerticalResolution;

    BootInfo.VideoModeInfo.PixelsPerScanline = 
        GraphicsOutputProtocol->Mode->Info->PixelsPerScanline;

    SystemTable->BootServices->AllocatePool(
        EfiLoaderData,
        BootInfo.VideoModeInfo.VerticalResolution * BootInfo.VideoModeInfo.PixelsPerScanline * 4,
        &BootInfo.VideoModeInfo.BackbufferPointer
    );

    //
    // ACPI XSDP Table
    //
    BootInfo.XSDPTable = NULL;
    EFI_GUID ACPI2GUID = EFI_ACPI_20_TABLE_GUID;
    EFI_CONFIGURATION_TABLE* tempTable = SystemTable->ConfigurationTable;
    for (UINT16 i = 0; i < SystemTable->NumberOfTableEntries; i++) {
        if (compare_efi_guid(&tempTable[i].VendorGuid, &ACPI2GUID)) {
            if (((char*)tempTable->VendorTable)[0] == 'R' &&
                ((char*)tempTable->VendorTable)[1] == 'S' &&
                ((char*)tempTable->VendorTable)[2] == 'D' &&
                ((char*)tempTable->VendorTable)[3] == ' ' &&
                ((char*)tempTable->VendorTable)[4] == 'P' &&
                ((char*)tempTable->VendorTable)[5] == 'T' &&
                ((char*)tempTable->VendorTable)[6] == 'R' &&
                ((char*)tempTable->VendorTable)[7] == ' ') {
                BootInfo.XSDPTable = tempTable->VendorTable;
            }
        }
        tempTable++;
    }
    if (BootInfo.XSDPTable == NULL) {
        efi_print(SystemTable, L"Fatal: Unable to locate ACPI v2+ XSDP\r\n");
        return EFI_NOT_FOUND;
    }

    //
    // Initialize Simple File System Protocol
    //
    status = init_efi_fs(SystemTable, &FileSystemProtocol);
    if (EFI_ERROR(status)) {
        // error printed inside initialization function
        return status;
    }

    //
    // Open EFI Boot Volume
    //
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Opening EFI boot volume\r\n");
    #endif
    status = FileSystemProtocol->OpenVolume(
            (struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*)FileSystemProtocol, 
            (struct EFI_FILE_PROTOCOL**)&RootFileSystem);
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Root file system address: ");
        efi_printhex(SystemTable, (UINT64)RootFileSystem, 1);
        efi_print(SystemTable, L"\r\n");
    #endif
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error opening EFI boot volume\r\n");
        return status;
    }
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: EFI boot volume opened\r\n");
    #endif

    //
    // Load Kernel
    //
    status = load_kernel(RootFileSystem, KERNEL_FILEPATH, KernelEntryPoint, SystemTable);
    if (EFI_ERROR(status)) {
        // error printed inside kernel loader function
        return status;
    }
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Kernel located in memory at entry point: ");
        efi_printhex(SystemTable, *KernelEntryPoint, 1);
        efi_print(SystemTable, L"\r\n");
    #endif

    //
    // Exit EFI
    //
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Saving memory map\r\n");
    #endif
    status = get_memory_map(
            SystemTable,
            (void**)&MemoryMap,
            &MemoryMapSize,
            &MemoryMapKey,
            &DescriptorSize,
            &DescriptorVersion);
    if (EFI_ERROR(status)) {
        // Error printed within function call above
        return status;
    }
    // Save memory map in BootInfo
    BootInfo.MemoryMap = MemoryMap;
    BootInfo.MemoryMapSize = MemoryMapSize;
    BootInfo.MemoryMapDescriptorSize = DescriptorSize;
    
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Exiting EFI boot services\r\n");
    #endif
    status = SystemTable->BootServices->ExitBootServices(ImageHandle, MemoryMapKey);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error exiting EFI boot services\r\n");
        return status;
    }

    #ifdef DEBUG
        print_video_mode_info(SystemTable, &BootInfo.VideoModeInfo);
    #endif
    //
    // Enter kernel
    //
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Entering kernel\r\n");
    #endif
    kernel_entry = (void (*)(KERNEL_BOOT_INFO*))*KernelEntryPoint;
    /*
     * BootInfo address is passed through rdi as the kernel uses the System V calling convention
     */
    __asm__ volatile ("mov %0, %%rdi" :: "r"(&BootInfo));
    kernel_entry(NULL);
    
    return EFI_LOAD_ERROR;
}
