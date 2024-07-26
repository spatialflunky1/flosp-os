#ifndef LOADER_H
#define LOADER_H

#include <efi.h>
#include <video.h>
#include <elf.h>

int check_file_exists(CHAR16* KernelFilename, EFI_SYSTEM_TABLE* SystemTable);

EFI_STATUS load_kernel(
        EFI_FILE_PROTOCOL* RootFileSystem, 
        CHAR16* KernelFilename, 
        EFI_PHYSICAL_ADDRESS* KernelEntryPoint, 
        EFI_SYSTEM_TABLE* SystemTable);

EFI_STATUS load_segment(
                EFI_SYSTEM_TABLE* SystemTable,
                EFI_FILE_PROTOCOL* KernelImage,
                EFI_PHYSICAL_ADDRESS SegmentOffset,
                UINT64 SegmentFileSize,
                UINT64 SegmentMemorySize,
                EFI_PHYSICAL_ADDRESS SegmentPhysicalAddress);

EFI_STATUS load_program_segments(
        EFI_SYSTEM_TABLE* SystemTable,
        EFI_FILE_PROTOCOL* KernelImage,
        void* KernelHeader,
        void* KernelProgramHeaders);

#endif
