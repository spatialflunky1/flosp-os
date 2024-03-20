#ifndef FS_H
#define FS_H

#include <efi.h>
#include <video.h>

EFI_STATUS init_efi_fs(EFI_SYSTEM_TABLE* SystemTable, EFI_SIMPLE_FILE_SYSTEM_PROTOCOL** FileSystemProtocol);

#endif
