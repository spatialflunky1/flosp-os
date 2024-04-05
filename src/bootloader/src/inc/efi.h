#ifndef EFI_H
#define EFI_H

#ifndef IN
    #define IN
    #define OUT
    #define OPTIONAL
#endif

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
    { 0x964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b } }
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    { 0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } }
#define EFI_ACPI_20_TABLE_GUID \
    { 0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81 } }

#define EFI_ERROR(a) (a < 0)
#define EFIERR(a)    (EFI_STATUS)(0x8000000000000000 | a)

#define EFI_PAGE_MASK  0xFFF
#define EFI_PAGE_SHIFT 12
#define EFI_SIZE_TO_PAGES(a)  \
    ( ((a) >> EFI_PAGE_SHIFT) + ((a) & EFI_PAGE_MASK ? 1 : 0) )

#define EFI_SUCCESS                      0
#define EFI_LOAD_ERROR            EFIERR(1)
#define EFI_INVALID_PARAMETER     EFIERR(2)
#define EFI_UNSUPPORTED           EFIERR(3)
#define EFI_BUFFER_TOO_SMALL      EFIERR(5)
#define EFI_NOT_FOUND            EFIERR(14)
#define EFI_INCOMPATIBLE_VERSION EFIERR(25)

// Open Modes
#define EFI_FILE_MODE_READ 0x0000000000000001
// File Attributes
#define EFI_FILE_READ_ONLY 0x0000000000000001

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001

//
// Datatypes
//
#define NULL ((void*)0)
#define UINT64_MAX_DEC_DIGITS 20
#define UINT64_MAX_HEX_DIGITS 16

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;
typedef unsigned int __attribute__((__mode__(__DI__)))  UINT64;

typedef signed char  INT8;
typedef signed short INT16;
typedef signed int   INT32;
typedef signed int __attribute__((__mode__(__DI__)))  INT64;

typedef UINT16 CHAR16;
typedef UINT8  bool;

typedef UINT64         EFI_STATUS;
typedef UINT64         EFI_PHYSICAL_ADDRESS;
typedef UINT64         EFI_VIRTUAL_ADDRESS;
typedef void*          EFI_HANDLE;
typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8  Data4[8];
} EFI_GUID;

//
// Forward Declarations
//
struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
struct EFI_FILE_PROTOCOL;
struct EFI_MEMORY_DESCRIPTOR;
struct EFI_GRAPHICS_OUTPUT_PROTOCOL;
struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;
struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

//
// EFI Memory Types
//
typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef enum {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
} EFI_ALLOCATE_TYPE;

//
// EFI handle search type
//
typedef enum {
    AllHandles,
    ByRegisterNotify,
    ByProtocol
} EFI_LOCATE_SEARCH_TYPE;

//
// EFI graphics
//
typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMask
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef enum {
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

//
// EFI Table Header
//
typedef struct {
    UINT64 Signature;  // Table signature
    UINT32 Revision;   // EFI spec
                       // Upper 16 bits: Major Revision
                       // Lower 16 bits: Minor Revision
                       // Format:  (Major).(Minor upper decimal).(Minor lower decimal)
    UINT32 HeaderSize; // Size of the table
    UINT32 CRC32;      // Cyclic Redundancy Check 32 (checksum)
    UINT32 Reserved;   // Set 0
} EFI_TABLE_HEADER;


//
// EFI Function Declarations
//
typedef 
EFI_STATUS 
(*EFI_TEXT_RESET) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN bool   ExtendedVerification);

typedef
EFI_STATUS
(*EFI_TEXT_STRING) (
        IN struct  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN CHAR16* String);

typedef
EFI_STATUS
(*EFI_TEXT_TEST_STRING) (
        IN struct  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN CHAR16* String);

typedef
EFI_STATUS
(*EFI_TEXT_QUERY_MODE) (
        IN struct   EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN UINT64   ModeNumber, 
        OUT UINT64* Columns,
        OUT UINT64* Rows);

typedef
EFI_STATUS
(*EFI_TEXT_SET_MODE) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This, 
        IN UINT64 ModeNumber);

typedef
EFI_STATUS
(*EFI_TEXT_SET_ATTRIBUTE) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN UINT64 Attribute);

typedef
EFI_STATUS
(*EFI_TEXT_CLEAR_SCREEN) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This);

typedef
EFI_STATUS
(*EFI_TEXT_SET_CURSOR_POSITION) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN UINT64 Column,
        IN UINT64 Row);

typedef
EFI_STATUS
(*EFI_TEXT_ENABLE_CURSOR) (
        IN struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
        IN bool   Visible);

typedef 
EFI_STATUS
(*EFI_EXIT_BOOT_SERVICES) (
        IN EFI_HANDLE ImageHandle,
        IN UINT64     MemoryMapKey);

typedef 
EFI_STATUS
(*EFI_SET_WATCHDOG_TIMER) (
        IN UINT64  Timeout,
        IN UINT64  WatchdogCode,
        IN UINT64  DataSize,
        IN CHAR16* WatchdogData);

typedef 
EFI_STATUS
(*EFI_OPEN_PROTOCOL) (
        IN EFI_HANDLE Handle,
        IN EFI_GUID*  Protocol,
        OUT void**    Interface OPTIONAL,
        IN EFI_HANDLE AgentHandle,
        IN EFI_HANDLE ControllerHandle,
        IN UINT32     Attributes);

typedef
EFI_STATUS
(*EFI_LOCATE_HANDLE_BUFFER) (
        IN EFI_LOCATE_SEARCH_TYPE SearchType,
        IN EFI_GUID*              Protocol  OPTIONAL,
        IN void*                  SearchKey OPTIONAL,
        OUT UINT64*               NoHandles,
        OUT EFI_HANDLE**          Buffer);

typedef
EFI_STATUS
(*EFI_LOCATE_PROTOCOL) (
        IN  EFI_GUID* Protocol,
        IN  void*     Registration OPTIONAL,
        OUT void**    Interface);

typedef 
EFI_STATUS
(*EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME) (
        IN  struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* This,
        OUT struct EFI_FILE_PROTOCOL**              Root);

typedef
EFI_STATUS
(*EFI_FILE_OPEN) (
        IN struct EFI_FILE_PROTOCOL*   This,
        OUT struct EFI_FILE_PROTOCOL** NewHandle,
        IN CHAR16*                     FileName,
        IN UINT64                      OpenMode,
        IN UINT64                      Attributes);

typedef
EFI_STATUS
(*EFI_FILE_CLOSE) (
        IN struct EFI_FILE_PROTOCOL* This);

typedef
EFI_STATUS
(*EFI_FILE_READ) (
        IN struct EFI_FILE_PROTOCOL* This,
        IN OUT UINT64*               BufferSize,
        OUT void*                    Buffer);

typedef
EFI_STATUS
(*EFI_FILE_GET_POSITION) (
        IN struct EFI_FILE_PROTOCOL* This,
        OUT UINT64*                  Position);

typedef
EFI_STATUS
(*EFI_FILE_SET_POSITION) (
        IN struct EFI_FILE_PROTOCOL* This,
        IN UINT64                    Position
        );

typedef 
EFI_STATUS
(*EFI_ALLOCATE_PAGES) (
        IN EFI_ALLOCATE_TYPE         Type,
        IN EFI_MEMORY_TYPE           MemoryType,
        IN UINT64                    Pages,
        IN OUT EFI_PHYSICAL_ADDRESS* Memory);

typedef 
EFI_STATUS
(*EFI_ALLOCATE_POOL) (
        IN EFI_MEMORY_TYPE PoolType,
        IN UINT64          Size,
        OUT void**         Buffer);

typedef
EFI_STATUS
(*EFI_GET_MEMORY_MAP) (
        IN OUT UINT64*                    MemoryMapSize,
        OUT struct EFI_MEMORY_DESCRIPTOR* MemoryMap,
        OUT UINT64*                       MapKey,
        OUT UINT64*                       DescriptorSize,
        OUT UINT32*                       DescriptorVersion);

typedef 
EFI_STATUS
(*EFI_FREE_POOL) (
        IN void* Buffer);

typedef 
EFI_STATUS
(*EFI_COPY_MEM) (
        IN void*  Destination,
        IN void*  Source,
        IN UINT64 Length);

typedef
EFI_STATUS
(*EFI_SET_MEM) (
        IN void*  Buffer,
        IN UINT64 Size,
        IN UINT8  Value);

typedef 
EFI_STATUS
(*EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE) (
        IN struct EFI_GRAPHICS_OUTPUT_PROTOCOL*           This,
        IN UINT32                                         ModeNumber,
        OUT UINT64*                                       SizeOfInfo,
        OUT struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION** Info);

typedef
EFI_STATUS
(*EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE) (
        IN struct EFI_GRAPHICS_OUTPUT_PROTOCOL* This,
        IN UINT32                               ModeNumber);

typedef
EFI_STATUS
(*EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT) (
        IN struct EFI_GRAPHICS_OUTPUT_PROTOCOL*        This,
        IN OUT struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL*   BltBuffer,
        IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION           BltOperation,
        IN UINT64                                      SourceX,
        IN UINT64                                      SourceY,
        IN UINT64                                      DestinationX,
        IN UINT64                                      DestinationY,
        IN UINT64                                      Width,
        IN UINT64                                      Height,
        IN UINT64                                      Delta OPTIONAL);

//
// EFI Tables
//
typedef struct {
    INT32 MaxMode;
    // Current settings
    INT32 Mode;
    INT32 Attribute;
    INT32 CursorColumn;
    INT32 CursorRow;
    bool  CursorVisible;
} SIMPLE_TEXT_OUTPUT_MODE;

typedef struct {
    EFI_TEXT_RESET               Reset;
    EFI_TEXT_STRING              OutputString;
    EFI_TEXT_TEST_STRING         TestString;
    EFI_TEXT_QUERY_MODE          QueryMode;
    EFI_TEXT_SET_MODE            SetMode;
    EFI_TEXT_SET_ATTRIBUTE       SetAttribute;
    EFI_TEXT_CLEAR_SCREEN        ClearScreen;
    EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
    EFI_TEXT_ENABLE_CURSOR       EnableCursor;
    SIMPLE_TEXT_OUTPUT_MODE*     Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct {
    UINT64                                      Revision;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

// All void* entries are unused by bootloader
typedef struct {
    UINT64                           Revision;
    EFI_FILE_OPEN                    Open;
    EFI_FILE_CLOSE                   Close;
    void*                            Delete;
    EFI_FILE_READ                    Read;
    void*                            Write;
    EFI_FILE_GET_POSITION            GetPosition;
    EFI_FILE_SET_POSITION            SetPosition;
    void*                            GetInfo;
    void*                            SetInfo;
    void*                            Flush;
    void*                            OpenEx;
    void*                            ReadEx;
    void*                            WriteEx;
    void*                            FlushEx;
} EFI_FILE_PROTOCOL;

// All void* service functions are unused by bootloader
typedef struct {
    EFI_TABLE_HEADER Hdr; // Table header
    // Task Priority Services
    void*                    RaiseTPL;
    void*                    RestoreTPL;
    // Memory Services
    EFI_ALLOCATE_PAGES       AllocatePages;
    void*                    FreePages;
    EFI_GET_MEMORY_MAP       GetMemoryMap;
    EFI_ALLOCATE_POOL        AllocatePool;
    EFI_FREE_POOL            FreePool;
    // Event & Timer Services
    void*                    CreateEvent;
    void*                    SetTimer;
    void*                    WaitForEvent;
    void*                    SignalEvent;
    void*                    CloseEvent;
    void*                    CheckEvent;
    // Protocol Handler Services
    void*                    InstallProtocolInterface;
    void*                    ReinstallProtocolInterface;
    void*                    UninstallProtocolInterface;
    void*                    HandleProtocol;
    void*                    Reserved;                   // Not a service function
    void*                    RegisterProtocolNotify;
    void*                    LocateHandle;
    void*                    LocateDevicePath;
    void*                    InstallConfigurationTable;
    // Image Services
    void*                    LoadImage;
    void*                    StartImage;
    void*                    Exit;
    void*                    UnloadImage;
    EFI_EXIT_BOOT_SERVICES   ExitBootServices;
    // Miscellaneous Services
    void*                    GetNextMonotonicCount;
    void*                    Stall;
    EFI_SET_WATCHDOG_TIMER   SetWatchdogTimer;
    // DriverSupport Services
    void*                    ConnectController;
    void*                    DisconnectController;
    // Open and Close Protocol Services
    EFI_OPEN_PROTOCOL        OpenProtocol;
    void*                    CloseProtocol;
    void*                    OpenProtocolInformation;
    // Library Services
    void*                    ProtocolsPerHandle;
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL      LocateProtocol;
    void*                    InstallMultipleProtocolInterfaces;
    void*                    UninstallMultipleProtocolInterfaces;
    // 32-bit CRC Services
    void*                    CalculateCrc32;
    // Miscellaneous Services
    EFI_COPY_MEM             CopyMem;
    EFI_SET_MEM              SetMem;
    void*                    CreateEventEx;
} EFI_BOOT_SERVICES;

typedef struct {
    EFI_GUID VendorGuid;
    void*    VendorTable;
} EFI_CONFIGURATION_TABLE;

// All void* entries are unused by bootloader
typedef struct {
    EFI_TABLE_HEADER                 Hdr;                  // Table header
    CHAR16*                          FirmwareVendor;       // Null-term string to identify the firmware vendor
    UINT32                           FirmwareRevision;     // Vendor specific firmware revision
    EFI_HANDLE                       ConsoleInHandle;      // Unused
    void*                            ConIn;                // Unused
    EFI_HANDLE                       ConsoleOutHandle;     // Handle for the active console output device
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;               // EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL interface pointer
    EFI_HANDLE                       StandardErrorHandle;  // Unused
    void*                            StdErr;               // Unused
    void*                            RuntimeServices;      // Unused
    EFI_BOOT_SERVICES*               BootServices;         // Unused
    UINT64                           NumberOfTableEntries; // Unused
    EFI_CONFIGURATION_TABLE*         ConfigurationTable;   // Unused
} EFI_SYSTEM_TABLE;

typedef struct {
    UINT32               Type;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS  VirtualStart;
    UINT64               NumberOfPages;
    UINT64               Attribute;
} EFI_MEMORY_DESCRIPTOR;

//
// Graphics
//
typedef struct {
    UINT32 RedMask;
    UINT32 GreenMask;
    UINT32 BlueMask;
    UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef struct {
    UINT32                    Version;
    UINT32                    HorizontalResolution;
    UINT32                    VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    EFI_PIXEL_BITMASK  PixelInformation;
    UINT32                    PixelsPerScanline;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    UINT32                                MaxMode;
    UINT32                                Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* Info;
    UINT64                                SizeOfInfo;
    EFI_PHYSICAL_ADDRESS                  FrameBufferBase;
    UINT64                                FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct {
    EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE QueryMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE   SetMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT        Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE*      Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

typedef struct {
    UINT8 Blue;
    UINT8 Green;
    UINT8 Red;
    UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

//
// Kernel
//

typedef struct {
    void*  FramebufferPointer;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
    UINT32 PixelsPerScanline;
} KERNEL_BOOT_VIDEO_MODE_INFO;

typedef struct {
    EFI_MEMORY_DESCRIPTOR*      MemoryMap;
    UINT64                      MemoryMapSize;
    UINT64                      MemoryMapDescriptorSize;
    KERNEL_BOOT_VIDEO_MODE_INFO VideoModeInfo;
    void*                       XSDPTable;
} KERNEL_BOOT_INFO;

#endif
