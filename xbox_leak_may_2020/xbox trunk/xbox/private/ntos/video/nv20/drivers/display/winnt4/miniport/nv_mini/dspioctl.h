#ifndef _DSPIOCTL_H
#define _DSPIOCTL_H
//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     dspioctl.h
//
// Abstract:
//
// Environment:
//
//     Kernel mode
//
// Revision History:
//
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include "nvMultiMon.h"
//******************************************************************************
// Number of dacs
//******************************************************************************
#define NV_NO_DACS                  2

//******************************************************************************
// Private IOCTL info - Used by all components of the display driver
//******************************************************************************

#define IOCTL_VIDEO_GET_VIDEO_CARD_INFO \
        CTL_CODE (FILE_DEVICE_VIDEO, 2048, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_RESTORE_DISPLAY_PATCH    \
        CTL_CODE (FILE_DEVICE_VIDEO, 2050, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_GET_OGL_STENCIL_SWITCH    \
        CTL_CODE (FILE_DEVICE_VIDEO, 2051, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_MAP_IO_PORT_FOR_ALI    \
        CTL_CODE (FILE_DEVICE_VIDEO, 2052, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_UNMAP_IO_PORT_FOR_ALI    \
        CTL_CODE (FILE_DEVICE_VIDEO, 2053, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_GET_VBLANK_FLAG    \
        CTL_CODE (FILE_DEVICE_VIDEO, 2054, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_MAP_NV_PFIFO_USER \
        CTL_CODE (FILE_DEVICE_VIDEO, 2055, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_ENABLE_WRITE_COMBINING \
        CTL_CODE (FILE_DEVICE_VIDEO, 2056, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_GET_TV_CURSOR_ADJUST_INFO \
        CTL_CODE (FILE_DEVICE_VIDEO, 2057, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_GET_IF09_VIDEO_SUPPORT \
        CTL_CODE (FILE_DEVICE_VIDEO, 2058, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SYSMEM_SHARE_MEMORY \
        CTL_CODE (FILE_DEVICE_VIDEO, 2059, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SYSMEM_UNSHARE_MEMORY \
        CTL_CODE (FILE_DEVICE_VIDEO, 2060, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_LOCK_DOWN_MEMORY \
        CTL_CODE (FILE_DEVICE_VIDEO, 2061, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_UNLOCK_MEMORY \
        CTL_CODE (FILE_DEVICE_VIDEO, 2062, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_MAP_TO_USER \
        CTL_CODE (FILE_DEVICE_VIDEO, 2063, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_UNMAP_FROM_USER \
        CTL_CODE (FILE_DEVICE_VIDEO, 2064, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_GET_COMMON_DEVICE_INFO \
        CTL_CODE (FILE_DEVICE_VIDEO, 2065, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_ALLOC_AGP_MEM \
        CTL_CODE (FILE_DEVICE_VIDEO, 2066, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_FREE_AGP_MEM  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2067, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_ENABLE_PIXMIX_INDEX \
        CTL_CODE (FILE_DEVICE_VIDEO, 2068, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_VPE_COMMAND \
        CTL_CODE (FILE_DEVICE_VIDEO, 2069, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_GET_REGISTRY_PATH  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2070, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_REGISTRY_VAL  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2071, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_REGISTRY_VALUE  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2072, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_REGISTRY_BINARY_VAL  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2073, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SET_DISPLAY_DEVICE  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2075, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_GET_DISPLAY_DEVICE  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2076, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VALIDATE_DEVICE_MODE  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2077, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_IS_NVIDIA_BOARD_AND_DRIVER  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2078, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_GET_GAMMA_CLUT  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2079, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_MAP_PHYS_ADDR  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2080, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_UNMAP_PHYS_ADDR  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2081, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_START_TIMER_FOR_NV10_CURSOR \
        CTL_CODE (FILE_DEVICE_VIDEO, 2083, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_STOP_TIMER_FOR_NV10_CURSOR \
        CTL_CODE (FILE_DEVICE_VIDEO, 2084, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_TIMER_UPDATE_XY_FOR_NV10_CURSOR \
        CTL_CODE (FILE_DEVICE_VIDEO, 2085, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_DESKTOP_INFO_MODE \
        CTL_CODE (FILE_DEVICE_VIDEO, 2086, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_GET_TIMING_DAC \
        CTL_CODE (FILE_DEVICE_VIDEO, 2087, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_PREMODE_SET \
        CTL_CODE (FILE_DEVICE_VIDEO, 2088, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_POSTMODE_SET \
        CTL_CODE (FILE_DEVICE_VIDEO, 2089, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_GET_TWINVIEW_STATE \
        CTL_CODE (FILE_DEVICE_VIDEO, 2090, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_TWINVIEW_STATE \
        CTL_CODE (FILE_DEVICE_VIDEO, 2091, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_GET_NUMBER_DACS \
        CTL_CODE (FILE_DEVICE_VIDEO, 2092, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_INDUCE_HEAD_DEVICE_SCANNING \
        CTL_CODE (FILE_DEVICE_VIDEO, 2093, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SAVE_GAMMA_VALUES \
        CTL_CODE (FILE_DEVICE_VIDEO, 2094, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_RESTORE_GAMMA_VALUES \
        CTL_CODE (FILE_DEVICE_VIDEO, 2095, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_PCI_SLOT  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2096, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_GET_HEAD_PHYSICAL_MODE_LIST  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2097, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_CLEAR_FRAMEBUFFER  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2098, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_GET_TV_ENCODER_TYPE  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2099, METHOD_BUFFERED, FILE_ANY_ACCESS)
// handle calls for stereo glasses
#define IOCTL_VIDEO_STEREO_GLASSES_SWITCH \
        CTL_CODE (FILE_DEVICE_VIDEO, 2100, METHOD_BUFFERED, FILE_ANY_ACCESS)
// handle calls for multi device support for NT 4
#define IOCTL_VIDEO_QUERY_NUM_OF_DEVICE_OBJECTS  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2101, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_GET_DEVICE_OBJECTS  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2102, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_INDUCE_TIMING_OVERRIDE \
        CTL_CODE (FILE_DEVICE_VIDEO, 3000, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_UNMAP_NV_PFIFO_USER \
        CTL_CODE (FILE_DEVICE_VIDEO, 3001, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define IOCTL_VIDEO_ZW_CREATE_FILE \
        CTL_CODE (FILE_DEVICE_VIDEO, 3002, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_ZW_READ_FILE \
        CTL_CODE (FILE_DEVICE_VIDEO, 3003, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_ZW_WRITE_FILE \
        CTL_CODE (FILE_DEVICE_VIDEO, 3004, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_ZW_CLOSE_HANDLE \
        CTL_CODE (FILE_DEVICE_VIDEO, 3005, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_ZW_GET_FILE_IO_FUNCS \
        CTL_CODE (FILE_DEVICE_VIDEO, 3006, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_CONTROL_DEVICE \
        CTL_CODE (FILE_DEVICE_VIDEO, 3007, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SET_UNIQUENESS_VGA \
        CTL_CODE (FILE_DEVICE_VIDEO, 3008, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SIGNAL_NVSVC_MODESET_EVENT \
        CTL_CODE (FILE_DEVICE_VIDEO, 3010, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_NVSVC_STATUS \
        CTL_CODE (FILE_DEVICE_VIDEO, 3011, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_NVSVC_MODESET_DEVMASK \
        CTL_CODE (FILE_DEVICE_VIDEO, 3012, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SIGNAL_NVCPL_HOTKEY_EVENT \
        CTL_CODE (FILE_DEVICE_VIDEO, 3013, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_CONVERT_RING3_HANDLE \
        CTL_CODE (FILE_DEVICE_VIDEO, 3014, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_MAP_NV_PBUS_USER \
        CTL_CODE (FILE_DEVICE_VIDEO, 3015, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_UNMAP_NV_PBUS_USER \
        CTL_CODE (FILE_DEVICE_VIDEO, 3016, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SHOULD_AGP_BE_ENABLED \
        CTL_CODE (FILE_DEVICE_VIDEO, 3017, METHOD_BUFFERED, FILE_ANY_ACCESS)


typedef struct _GET_NUMBER_DACS
{
    ULONG ulNumberDacsOnBoard;          // Number of heads on the board.
    ULONG ulNumberDacsConnected;        // Number of heads connected to any device.
    ULONG ulAllDeviceMask;              // Bitmask for all connectors in the board.
    ULONG ulConnectedDeviceMask;        // Bitmask for all connected devices in the system.
    ULONG ulDeviceDisplay[NV_NO_DACS];  // 0,1 or 1,0 depending on the primary head being used in std/span modes.
    ULONG ulDeviceMask[NV_NO_DACS];     // The bitmask of device connected to the head.
                                        // defined in nvcm.h
    ULONG ulDeviceType[NV_NO_DACS];     // The type of device connected to the head viz., CRT, TV, FlatPanel
                                        // defined in nvcm.h
    ULONG ulTVFormat[NV_NO_DACS];       // The TV type: defined in nvcm.h
    ULONG ulHeadDeviceOptions[NV_NO_DACS]; // A matrix of possible device options for each head.
    //
    // Following are the registry keys that get sent back upto the display driver
    //
    ULONG Mobile;                       // 0: desktop system; 1: Dell laptop; 2: Toshiba laptop
    ULONG UseHWSelectedDevice;          // BIOS devices take precedence over windows devices.
    ULONG UseHotKeySameMode;            // On hot key switches, keeps the same desktop mode (resolution,depth).
                                        // This needs PAN_SCAN_SELECTION_ENABLE_ALL to have been turned on.
                                        // This field is obsolete now since no OEM is using this feature curently.
    ULONG PanScanSelection;             // absent or 0: pan-scan in only secondary monitor of clone mode: PAN_SCAN_SELECTION_CLONE_SECONDARY_ONLY
                                        // 1: Enable pan-scan in all modes: PAN_SCAN_SELECTION_ENABLE_ALL
                                        // 2: Disable pan-scan in all modes:PAN_SCAN_SELECTION_DISABLE_ALL 
    ULONG GDIModePruning;
    ULONG ulDFPSupports1400x1050;       // If the builtin DFP of the laptop supports 1400x1050 or not.
                                        // Toshiba wants us to export this mode only if natively supported by DFP.
    ULONG ulHotKeyPending;              // 1 if miniport has a hotkey pending to be done by the display driver.
                                        // Then the display will ignore the BOOT_HEADS.
    ULONG ulUniquenessVGA;              // The unique integer ID for the ppdev
                                        // for which the last IOCTL_RESET_DEVICE 
                                        // was performed.
    ULONG ACPISystem;                   // TRUE if we are on a ACPI system (such as laptops).

    

} GET_NUMBER_DACS;

typedef struct _HEAD_MODE_LIST_INFO
{
    ULONG ulHead;
    ULONG ulEnableDDC;
    ULONG ulDeviceMask;
    ULONG ulDeviceType;
    ULONG ulTVType;
    ULONG ulNumVirtualModes;
    MODE_ENTRY *pVirtualModeList;
    MODE_ENTRY *pPhysicalModeList;    // Caller should have allocated this memory. It is assumed that this
                                      // PhysicalModeList is atleast as big as the VirtualModeList.
} HEAD_MODE_LIST_INFO;

typedef struct _NV_DESKTOP_INFO
{
    ULONG ulDesktopMode;                // 0x0000 -> Single Monitor
    ULONG ulDesktopModeIndex;           // Display driver set the requested mode index  
    ULONG ulDesktopWidth;               // Miniport driver return the width based on the mode index
    ULONG ulDesktopHeight;              // Miniport driver return the height based on the mode index
    ULONG ulDisplayWidth[NV_NO_DACS];   // Display driver set these values; 
    ULONG ulDisplayHeight[NV_NO_DACS];  // control panel should give it to display driver
    ULONG ulDisplayRefresh[NV_NO_DACS]; // refresh rate 
    ULONG ulDisplayPixelDepth[NV_NO_DACS]; // Color Depth
    ULONG ulNumberDacsOnBoard;          // Number of DACs on the board
    ULONG ulNumberDacsConnected;        // Number of DACs which have a device (CRT/DFP/TV) connected to them
    ULONG ulNumberDacsActive;           // Number of DACs which are currently active. For example, in the Normal
                                        // mode, this value will be 1.
    ULONG ulAllDeviceMask;              // Bitmask of all output connectors in the board.
    ULONG ulConnectedDeviceMask;        // Bitmask for all connected devices in the system.
    ULONG ulDeviceMask[NV_NO_DACS];     // The bitmask of device connected to the head.
                                        // defined in nvcm.h
    ULONG ulDeviceType[NV_NO_DACS];     // The type of device connected to the head viz., CRT, TV, FlatPanel
                                        // defined in nvcm.h
    ULONG ulTVFormat[NV_NO_DACS];       // The TV type: defined in nvcm.h
    ULONG ulDeviceDisplay[NV_NO_DACS];  // Dynamically switch device; ulDeviceDisplay[0] will be primary; head 0 or head 1
    ULONG ulHeadDeviceOptions[NV_NO_DACS];    // A matrix of possible device options for each head. Defined in nvMultiMon.h.
                                                // This info is used mainly by the NV control panel and gets scanned in after each
                                                // modeset since this matrix can be changed by the RM after a modeset.
                                                // This is a static entity as opposed to ulDeviceType[NV_NO_DACS] which specifies
                                                // the currently selected device type for this head.
    ULONG ulTwinView_State;             // standard, clone or span.

} NV_DESKTOP_INFO;

typedef struct _NV_CURSOR_INFO
{
    LONG    lCursorX;
    LONG    lCursorY;
    LONG    lCursorWidth;
    LONG    lCursorHeight;
    LONG    lScreenStride;
    LONG    lScreenPixelDepth;
    ULONG   ulHwCursorWidth;
    ULONG   ulOriginalImageMemOffset;
    ULONG   ulCursorMemOffset;
} NV_CURSOR_INFO;

typedef struct _NV_SYSMEM_SHARE_MEMORY
    {
    PVOID pSystemMdl;           // MDL for locked down memory
    PVOID userVirtualAddress;   // user virtual address
    PVOID ddVirtualAddress;     // display driver virtual address
    ULONG physicalAddress;      // physical address of memory
    ULONG byteLength;           // byte length of data to lock down
    ULONG bDontModifyMDL;       // if TRUE, MDL will not be allocated nor freed
    } NV_SYSMEM_SHARE_MEMORY;


typedef struct _NV_LOCK_DOWN_MEMORY
    {
    PVOID virtualAddress; // display driver virtual address
    ULONG byteLength;     // byte length of data to lock down
    PVOID pMdl;           // MDL for locked down memory
    } NV_LOCK_DOWN_MEMORY;

typedef struct _NV_UNLOCK_MEMORY
    {
    PVOID pMdl;           // MDL for memory to unlock
    } NV_UNLOCK_MEMORY;

//
// AGP memory block node structure.
//
typedef struct _AGP_MEM
    {
    ULONG   hContext;       // Addr context of process
    ULONG   cb;             // Size of memory region in bytes
    PVOID   VirtualAddress; // Based address of Virtual AGP buffer
    } *PAGP_MEM, AGP_MEM;

//
// Registry set structure
//
typedef struct _NV_REGISTRY_STRUCT
    {
    PWSTR keyName;          // Name of key in unicode string format
    PVOID keyVal;           // pointer to key value
    ULONG keyValSize;       // Size of Key Value;
    } *PNV_REGISTRY_STRUCT, NV_REGISTRY_STRUCT;


//
// Zw File I/O support
// We don't want to export the use of Zw functions for release builds
// define NV_ZW_FILE_IO_ENABLED to get the Zw functions enabled.
// 

//#define NV_ZW_FILE_IO_ENABLED

typedef struct _NV_ZW_IO_FILE_OUT
    {
    PVOID pvFileHandle;
    PVOID pvIoStatusBlock;
    PVOID pvBuffer;
    } *PNV_ZW_IO_FILE_OUT, NV_ZW_IO_FILE_OUT;

typedef struct _ZW_UNICODE_STRING
    {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
    } ZW_UNICODE_STRING, *PZW_UNICODE_STRING;

typedef struct _ZW_OBJECT_ATTRIBUTES
    {
    ULONG              Length;
    PVOID              RootDirectory;             // Handle
    PZW_UNICODE_STRING ObjectName;
    ULONG              Attributes;
    PVOID              SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID              SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
    } ZW_OBJECT_ATTRIBUTES, *PZW_OBJECT_ATTRIBUTES;

typedef struct _NV_ZW_CREATE_FILE_IN
    {
    ULONG                ulDesiredAccess;
    ZW_OBJECT_ATTRIBUTES ObjectAttributes;
    LARGE_INTEGER        liAllocationSize;
    ULONG                ulFileAttributes;
    ULONG                ulShareAccess;
    ULONG                ulCreateDisposition;
    ULONG                ulCreateOptions;
    PVOID                pvEaBuffer;
    ULONG                ulEaLength;

    ZW_UNICODE_STRING uiObjectName;
    WCHAR pwcObjectBuffer[257];
    } *PNV_ZW_CREATE_FILE_IN, NV_ZW_CREATE_FILE_IN;

typedef struct _NV_ZW_RW_FILE_IN
    {
    PVOID         pvFileHandle;
    PVOID         pvEvent;
    PVOID         pvApcRoutine;
    PVOID         pvApcContext;
    PVOID         pvBuffer;
    ULONG         ulLength;
    LARGE_INTEGER liByteOffset;
    PVOID         pvKey;
    } *PNV_ZW_RW_FILE_IN, NV_ZW_RW_FILE_IN;

typedef struct _NV_ZW_DEVICE_IO_CONTROL_FILE
    {
    PVOID hDevice;
    ULONG ulIoControlCode;
    PVOID pvInputBuffer;
    ULONG ulInputBufferSize;
    } *PNV_ZW_DEVICE_IO_CONTROL_FILE, NV_ZW_DEVICE_IO_CONTROL_FILE;

typedef ULONG (*LPNVZWCREATEFILEFUNCTION) ( PVOID FileHandle,
                                            ULONG DesiredAccess,
                                            PVOID ObjectAttributes,
                                            PVOID IoStatusBlock,
                                            PVOID AllocationSize,
                                            ULONG FileAttributes,
                                            ULONG ShareAccess,
                                            ULONG CreateDisposition,
                                            ULONG CreateOptions,
                                            PVOID EaBuffer,
                                            ULONG EaLength );


typedef ULONG (*LPNVZWWRITEFILEFUNCTION) ( PVOID  FileHandle,
                                           PVOID  Event,
                                           PVOID  ApcRoutine,
                                           PVOID  ApcContext,
                                           PVOID  IoStatusBlock,
                                           PVOID  Buffer,
                                           ULONG  Length,
                                           PVOID  ByteOffset,
                                           PVOID  Key );

typedef ULONG (*LPNVZWREADFILEFUNCTION)  ( PVOID  FileHandle,
                                           PVOID  Event,
                                           PVOID  ApcRoutine,
                                           PVOID  ApcContext,
                                           PVOID  IoStatusBlock,
                                           PVOID  Buffer,
                                           ULONG  Length,
                                           PVOID  ByteOffset,
                                           PVOID  Key );

typedef ULONG (*LPNVZWCLOSEFUNCTION) ( PVOID Handle );

typedef ULONG (*LPNVZWDEVICEIOCONTROLFILEFUNCTION) ( PVOID  FileHandle,
                                                     PVOID  Event,
                                                     PVOID  ApcRoutine,
                                                     PVOID  ApcContext,
                                                     PVOID  IoStatusBlock,
                                                     ULONG  IoControlCode,
                                                     PVOID  InputBuffer,
                                                     ULONG  InputBufferLength,
                                                     PVOID  OutputBuffer,
                                                     ULONG  OutputBufferLength );

typedef struct _NV_ZW_FILE_IO_FUNCS
    {
    LPNVZWCREATEFILEFUNCTION pvZwCreateFile;
    LPNVZWWRITEFILEFUNCTION pvZwWriteFile;
    LPNVZWREADFILEFUNCTION pvZwReadFile;
    LPNVZWCLOSEFUNCTION pvZwClose;
    LPNVZWDEVICEIOCONTROLFILEFUNCTION pvZwDeviceIoControlFile;
    } *PNV_ZW_FILE_IO_FUNCS, NV_ZW_FILE_IO_FUNCS;

//
// OS Version Information Support
// (Some header information stolen from NTDDK.H)
//

typedef struct _NV_UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR  Buffer;
#endif // MIDL_PASS
} NV_UNICODE_STRING;
typedef NV_UNICODE_STRING *PNV_UNICODE_STRING;

typedef struct _NV_OS_VERSION_INFO
    {
    ULONG  dwMajorVersion;
    ULONG  dwMinorVersion;
    ULONG  dwBuildNumber;
    ULONG  dwBuildType;
    } *PNV_OS_VERSION_INFO, NV_OS_VERSION_INFO;

#if (_WIN32_WINNT >= 0x0500)
//
// System Memory Information structure
//
typedef struct _NV_SYSMEM_INFO
    {
    ULONGLONG SystemMemorySize;      // Physical memory size in bytes
    ULONGLONG AgpAllocationLimit;    // Max recommended size in bytes for committed AGP meme
    } *PNV_SYSMEM_INFO, NV_SYSMEM_INFO;
#endif // #if (_WIN32_WINNT >= 0x0500)


typedef struct _NV_COMMON_DEVICE_INFO
    {
    ULONG  ulDeviceReference;
    ULONG  ulEnableDualView;
    ULONG  ulDualViewSecondary;

    NV_OS_VERSION_INFO OsVersionInfo;
#if (_WIN32_WINNT >= 0x0500)
    NV_SYSMEM_INFO     SysMemInfo;
#endif
    } *PNV_COMMON_DEVICE_INFO, NV_COMMON_DEVICE_INFO;

typedef struct _RESOLUTION_INFO
{
    ULONG ulDisplayWidth;           // width  
    ULONG ulDisplayHeight;          // Height
    ULONG ulDisplayRefresh;         // refresh rate 
    ULONG ulDisplayPixelDepth;      // Color Depth
} RESOLUTION_INFO;

//
// Possible values for ulOption in the HEAD_RESOLUTION_INFO structure
// Currently only one flag is being used.
//
#define HEAD_RESOLUTION_OPTION_DONT_BACKOFF 1

typedef struct _HEAD_RESOLUTION_INFO
{
    ULONG ulDisplayWidth;           // width  
    ULONG ulDisplayHeight;          // Height
    ULONG ulDisplayRefresh;         // refresh rate 
    ULONG ulDisplayPixelDepth;      // Color Depth
    ULONG ulHead;                     // Whether 0 or 1. This is the physical head number
    ULONG ulDeviceMask;             // Bitmask of Device.
    ULONG ulDeviceType;             // Device type of the head.
    ULONG ulTVFormat;
    ULONG ulOption;                 // if DONT_BACKOFF is set, then if the requested mode is not supported in
                                    // the EDID, ignores the EDID and returns the timing without backing off.
} HEAD_RESOLUTION_INFO;


typedef struct _dac_timing_values
{  
    unsigned int HorizontalVisible; 
    unsigned int VerticalVisible;
    unsigned int Refresh;
    unsigned int HorizontalTotal; 
    unsigned int HorizontalBlankStart; 
    unsigned int HorizontalRetraceStart; 
    unsigned int HorizontalRetraceEnd; 
    unsigned int HorizontalBlankEnd;
    unsigned int VerticalTotal; 
    unsigned int VerticalBlankStart; 
    unsigned int VerticalRetraceStart; 
    unsigned int VerticalRetraceEnd; 
    unsigned int VerticalBlankEnd;
    unsigned int PixelClock;
    unsigned int HSyncpolarity; 
    unsigned int VSyncpolarity; 
} DAC_TIMING_VALUES;

//
// Possible values for the registry entry "GDIModePruning".
//
// Absent or 0: Driver always provides EDID mode pruning protection.
// 1:   For desktop or laptop systems, for CRT, for the
//      standard mode device and the primary device in clone mode and
//      both devices in XP DualView mode, lets the GDI handle EDID 
//      mode pruning via "Hide Modes" monitor checkbox.
// 2:   For desktop systems only, for CRT or DFP, for the
//      standard mode device and the primary device in clone mode and
//      both devices in XP DualView mode, lets the GDI handle EDID 
//      mode pruning via "Hide Modes" monitor checkbox.
//
#define GDI_MODE_PRUNING_NONE                       0
#define GDI_MODE_PRUNING_DESKTOP_LAPTOP_CRT         1
#define GDI_MODE_PRUNING_DESKTOP_CRT_DFP            2



//
// structures used forr NT4 multi device layer
//
typedef struct _QUERY_NUM_OF_DEVICE_OBJECTS
{
    union
    {
        IN  HANDLE hDriver;
        IN  PVOID  pvDevObj;
        OUT ULONG  ulNumDevObj;
    };
} QUERY_NUM_OF_DEVICE_OBJECTS;

typedef struct _GET_DEVICE_OBJECTS
{
    union
    {
        IN  HANDLE hDriver;
        IN  PVOID  pvDevObj;
    };
    IN  ULONG  ulNumDevObj;
    union
    {
        OUT  HANDLE ahDriver[1];
        OUT  PVOID  apvDevObj[1];
    };
    
} GET_DEVICE_OBJECTS;

#endif // _DSPIOCTL_H
