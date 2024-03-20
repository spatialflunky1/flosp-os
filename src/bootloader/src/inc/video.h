#ifndef VIDEO_H
#define VIDEO_H

#include <efi.h>

#define TARGET_SCREEN_WIDTH 800
#define TARGET_SCREEN_HEIGHT 600
#define TARGET_SCREEN_PIXEL_FORMAT PixelBlueGreenRedReserved8BitPerColor

static EFI_GUID gEfiGraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

typedef struct {
    EFI_HANDLE* handle_buffer;
    UINT64      handle_count;
} UefiGraphicsService;

void efi_print(EFI_SYSTEM_TABLE* SystemTable, CHAR16* s);
void efi_printnum(EFI_SYSTEM_TABLE* SystemTable, UINT64 n);
// zero_x: when set to 1 print 0x before the number
void efi_printhex(EFI_SYSTEM_TABLE* SystemTable, UINT64 h, UINT8 zero_x);
EFI_STATUS graphics_init(EFI_SYSTEM_TABLE* SystemTable, UefiGraphicsService* GraphicsService);
EFI_STATUS find_graphics_mode(
        EFI_SYSTEM_TABLE* SystemTable, 
        EFI_GRAPHICS_OUTPUT_PROTOCOL* Protocol,
        UINT32 TargetWidth,
        UINT32 TargetHeight,
        EFI_GRAPHICS_PIXEL_FORMAT TargetPixelFormat,
        UINT64* VideoMode);
EFI_STATUS set_graphics_mode(
        EFI_SYSTEM_TABLE* SystemTable, 
        EFI_GRAPHICS_OUTPUT_PROTOCOL* Protocol, 
        UINT32 TargetWidth,
        UINT32 TargetHeight,
        EFI_GRAPHICS_PIXEL_FORMAT TargetPixelFormat);
void print_video_mode_info(EFI_SYSTEM_TABLE* SystemTable, KERNEL_BOOT_VIDEO_MODE_INFO* VideoModeInfo);

#endif
