#include <fs.h>

EFI_STATUS init_efi_fs(EFI_SYSTEM_TABLE* SystemTable, EFI_SIMPLE_FILE_SYSTEM_PROTOCOL** FileSystemProtocol) {
    EFI_STATUS status = EFI_SUCCESS; 
    EFI_GUID gEfiSimpleFileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: Loading EFI filesystem...\r\n");
    #endif
    status = SystemTable->BootServices->LocateProtocol(
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            (void**)FileSystemProtocol);
    if (EFI_ERROR(status)) {
        efi_print(SystemTable, L"Fatal: Error locating Simple File System Protocol\r\n");
        return status;
    }
    #ifdef DEBUG
        efi_print(SystemTable, L"Debug: EFI filesystem loaded\r\n");
    #endif
    return status;
}
