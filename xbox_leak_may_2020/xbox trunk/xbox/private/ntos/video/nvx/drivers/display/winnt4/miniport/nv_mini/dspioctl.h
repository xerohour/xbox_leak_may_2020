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

#define IOCTL_VIDEO_GET_DEVICE_REF \
        CTL_CODE (FILE_DEVICE_VIDEO, 2065, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_ALLOC_AGP_MEM \
        CTL_CODE (FILE_DEVICE_VIDEO, 2066, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_FREE_AGP_MEM  \
        CTL_CODE (FILE_DEVICE_VIDEO, 2067, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_ENABLE_PIXMIX_INDEX \
                CTL_CODE (FILE_DEVICE_VIDEO, 2068, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_VPE_COMMAND \
        CTL_CODE (FILE_DEVICE_VIDEO, 2069, METHOD_BUFFERED, FILE_ANY_ACCESS)

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
#define IOCTL_VIDEO_GET_SYSTEM_MEMORY_INFO \
        CTL_CODE (FILE_DEVICE_VIDEO, 2082, METHOD_BUFFERED, FILE_ANY_ACCESS)
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

typedef struct _GET_NUMBER_DACS
{
    ULONG ulNumberDacsOnBoard;          // Number of heads on the board.
    ULONG ulNumberDacsConnected;        // Number of heads connected to any device.
    ULONG ulAllDeviceMask;              // Bitmask for all connectors in the board.
    ULONG ulConnectedDeviceMask;        // Bitmask for all connected devices in the system.
    ULONG ulDeviceMask[NV_NO_DACS];     // The bitmask of device connected to the head.
                                       // defined in nvcm.h
    ULONG ulDeviceType[NV_NO_DACS];     // The type of device connected to the head viz., CRT, TV, FlatPanel
                                       // defined in nvcm.h
    ULONG ulTVFormat[NV_NO_DACS];       // The TV type: defined in nvcm.h
    ULONG ulHeadDeviceOptions[NV_NO_DACS]; // A matrix of possible device options for each head.

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

#if _WIN32_WINNT >= 0x0500
//
// System Memory Information structure
//
typedef struct _NV_SYSMEM_INFO
    {
    ULONGLONG SystemMemorySize;      // Physical memory size in bytes
    ULONGLONG AgpAllocationLimit;    // Max recommended size in bytes for committed AGP meme
    } *PNV_SYSMEM_INFO, NV_SYSMEM_INFO;
#endif // _WIN32_WINNT >= 0x0500


typedef struct _RESOLUTION_INFO
{
    ULONG ulDisplayWidth; 			// width  
    ULONG ulDisplayHeight;  		// Height
    ULONG ulDisplayRefresh;  		// refresh rate 
    ULONG ulDisplayPixelDepth; // Color Depth
} RESOLUTION_INFO;

typedef struct _HEAD_RESOLUTION_INFO
{
    ULONG ulDisplayWidth; 			// width  
    ULONG ulDisplayHeight;  		// Height
    ULONG ulDisplayRefresh;  		// refresh rate 
    ULONG ulDisplayPixelDepth;      // Color Depth
    ULONG ulHead;                     // Whether 0 or 1. This is the physical head number
    ULONG ulDeviceMask;             // Bitmask of Device.
    ULONG ulDeviceType;             // Device type of the head.
    ULONG ulTVFormat;
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

