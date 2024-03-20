#include <video.h>

void efi_print(EFI_SYSTEM_TABLE* SystemTable, CHAR16* s) {
    SystemTable->ConOut->OutputString(
            (struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*)SystemTable->ConOut, 
            s);
}

void efi_printnum(EFI_SYSTEM_TABLE* SystemTable, UINT64 n) {
    CHAR16 StringBuffer[UINT64_MAX_DEC_DIGITS];
    INT8   stack_top = -1;
    // Append the digits to the string buffer
    if (n == 0) {
        stack_top++;
        StringBuffer[stack_top] = L'0';
    }
    while (n != 0) {
        stack_top++;
        StringBuffer[stack_top] = (CHAR16)((n%10)+48);
        n /= 10;
    }

    // Reverse digits into the correct order in the StringBuffer
    INT8 tmp_index = stack_top;
    CHAR16 tmp;
    for (; tmp_index > (stack_top/2); tmp_index--) {
        tmp = StringBuffer[stack_top - tmp_index];
        StringBuffer[stack_top - tmp_index] = StringBuffer[tmp_index];
        StringBuffer[tmp_index] = tmp;
    }
    // Add the null byte
    StringBuffer[stack_top+1] = L'\0';
    // Print the created string 
    efi_print(SystemTable, StringBuffer);
}

void efi_printhex(EFI_SYSTEM_TABLE* SystemTable, UINT64 h, UINT8 zero_x) {
    CHAR16 StringBuffer[UINT64_MAX_HEX_DIGITS];
    INT8   stack_top = -1;
    // Append the hex digits to the string buffer
    if (h == 0) {
        stack_top++;
        StringBuffer[stack_top] = L'0';
    }
    UINT16 tmp0;
    while(h != 0) {
        stack_top++;
        tmp0 = h % 16;
        if (tmp0 >= 10) {
            StringBuffer[stack_top] = (CHAR16)(tmp0+55);
        }
        else {
            StringBuffer[stack_top] = (CHAR16)(tmp0+48);
        }
        h /= 16;
    }

    // Reverse digits into the correct order in the StringBuffer
    INT8 tmp_index = stack_top;
    CHAR16 tmp1;
    for (; tmp_index > (stack_top/2); tmp_index--) {
        tmp1 = StringBuffer[stack_top - tmp_index];
        StringBuffer[stack_top - tmp_index] = StringBuffer[tmp_index];
        StringBuffer[tmp_index] = tmp1;
    }
    // Add the null byte
    StringBuffer[stack_top+1] = L'\0';
    
    // Print the created string
    if (zero_x) {
        efi_print(SystemTable, L"0x");
    }
    efi_print(SystemTable, StringBuffer);
}

EFI_STATUS graphics_init(EFI_SYSTEM_TABLE* SystemTable, UefiGraphicsService* GraphicsService) {
    EFI_STATUS status = EFI_SUCCESS; 
    
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Initializing the graphics output protocol service\r\n");
    #endif
    // Get graphics service handle buffer
    status = SystemTable->BootServices->LocateHandleBuffer(
            ByProtocol,
            &gEfiGraphicsOutputProtocolGuid,
            NULL,
            &GraphicsService->handle_count,
            &GraphicsService->handle_buffer);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error locating graphics output protocol handle buffer\r\n");
        return status;
    }

    return status;
}

EFI_STATUS find_graphics_mode(
        EFI_SYSTEM_TABLE* SystemTable, 
        EFI_GRAPHICS_OUTPUT_PROTOCOL* Protocol,
        UINT32 TargetWidth,
        UINT32 TargetHeight,
        EFI_GRAPHICS_PIXEL_FORMAT TargetPixelFormat,
        UINT64* GraphicsMode)
{
    EFI_STATUS                            status = EFI_SUCCESS;
    UINT64                                ModeInfoSize;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* ModeInfo;
    
    for (UINT32 i = 0; i < Protocol->Mode->MaxMode; i++) {
        #ifdef DEBUG
            efi_print(SystemTable, L"Debug: Testing video mode number: ");
            efi_printnum(SystemTable, i);
            efi_print(SystemTable, L"\r\n");
        #endif
        status = Protocol->QueryMode(
                (struct EFI_GRAPHICS_OUTPUT_PROTOCOL*)Protocol, 
                i, 
                &ModeInfoSize, 
                (struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION**)&ModeInfo);
        if (EFI_ERROR(status)) {
            efi_print(SystemTable, L"Fatal: Error querying video mode number: ");
            efi_printnum(SystemTable, i);
            efi_print(SystemTable, L"\r\n");
        }

        if (ModeInfo->HorizontalResolution == TargetWidth &&
            ModeInfo->VerticalResolution   == TargetHeight &&
            ModeInfo->PixelFormat          == TargetPixelFormat)
        {
            #ifdef DEBUG
                efi_print(SystemTable, L"Debug: Matching video mode detected\r\n");
            #endif
            *GraphicsMode = i;
            return status;
        }
    }

    return EFI_UNSUPPORTED;
}

EFI_STATUS set_graphics_mode(
        EFI_SYSTEM_TABLE* SystemTable, 
        EFI_GRAPHICS_OUTPUT_PROTOCOL* Protocol, 
        UINT32 TargetWidth,
        UINT32 TargetHeight,
        EFI_GRAPHICS_PIXEL_FORMAT TargetPixelFormat) 
{
    EFI_STATUS status = EFI_SUCCESS;
    UINT64 GraphicsModeNum = 0;

    // Locate graphics mode that matches target specifications
    status = find_graphics_mode(
            SystemTable, 
            Protocol, 
            TargetWidth, 
            TargetHeight, 
            TargetPixelFormat, 
            &GraphicsModeNum);
    if (EFI_ERROR(status)) {
        // Error printed within function call above
        return status;
    }

    // Set graphics mode
    status = Protocol->SetMode((struct EFI_GRAPHICS_OUTPUT_PROTOCOL*)Protocol, GraphicsModeNum);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error setting graphics mode\r\n");
        return status;
    }

    return status;
}

void print_video_mode_info(EFI_SYSTEM_TABLE* SystemTable, KERNEL_BOOT_VIDEO_MODE_INFO* VideoModeInfo) {
    efi_print(SystemTable, L"Debug: Current video mode info:\r\n");
    // Framebuffer Address
    efi_print(SystemTable, L"    Framebuffer Address: ");
    efi_printhex(SystemTable, (UINT64)VideoModeInfo->FramebufferPointer, 1);
    efi_print(SystemTable, L"\r\n");
    // Horizontal Resolution
    efi_print(SystemTable, L"    Horizontal Resolution: ");
    efi_printnum(SystemTable, VideoModeInfo->HorizontalResolution);
    efi_print(SystemTable, L"\r\n");
    // Vertical Resolution
    efi_print(SystemTable, L"    Vertical Resolution: ");
    efi_printnum(SystemTable, VideoModeInfo->VerticalResolution);
    efi_print(SystemTable, L"\r\n");
    // Pixels Per Scanline
    efi_print(SystemTable, L"    Pixels Per Scanline: ");
    efi_printnum(SystemTable, VideoModeInfo->PixelsPerScanline);
    efi_print(SystemTable, L"\r\n");
}
