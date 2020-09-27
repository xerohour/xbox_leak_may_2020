/*****************************************************************************
 *                                                                           *
 *                                 ESCAPE.H                                  *
 *                                                                           *
 *    A file to gather all the miscellaneous Escape() codes implemented in   *
 *    the GDI driver.                                                        *
 *                                                                           *
 *                 Copyright (C) 1999, Nvidia Coporation                     *
 *                                                                           *
 *                                                                           *
 * Any escape that begins NV_ESC_XXX or GN_ESC_XXX can be changed since      *
 * these are private escapes between different driver components. Any other  *
 * escapes are system defined values and shouldn't be changed.               *
 *****************************************************************************/

#ifndef _ESCAPE_H_
#define _ESCAPE_H_

//*****************************************************************************
// Pulled from hwspec/code/hwcntrl.asm

#define NV_ESC_DVD_MACROVISION_CMD              0x6979

#define     MACROVISION_OFF                     0
#define     MACROVISION_AGC_ONLY                1
#define     MACROVISION_TWO_STRIPES_PLUS_AGC    2
#define     MACROVISION_FOUR_STRIPES_PLUS_AGC   3

//*****************************************************************************
// Hide these for WINNT miniport builds to avoid conflict with nv4_ref.h

#ifndef WINNT
#define     NV_PRAMDAC_TEST_CONTROL             0x680608
#define     NV_PRAMDAC_CHECKSUM                 0x68060C
#endif

    typedef struct _tagMACROVISION_INFO {
        unsigned long   dwMacrovisionNavigatorID;
        unsigned long   dwMacrovisionData;
    }   MACROVISION_INFO;


//*****************************************************************************
// Pulled from hwspec/code/hwcntrl.asm
#define NV_ESC_INTEL_DIAG                       0x6E77      // 'nv' + 1

#define     IDIAG_OPEN_INTERFACE                0
#define     IDIAG_CLOSE_INTERFACE               1
#define     IDIAG_GET_ERROR_CODE                2
#define     IDIAG_GET_CRC                       3
#define     IDIAG_GET_DRIVER_INFO               4
#define     IDIAG_GET_MEMORY_INFO               5
#define     IDIAG_GET_CLOCK_RATES               6
#define     IDIAG_GET_PCI_CONFIG                7
#define     IDIAG_GET_BUS_CONFIG                8

#define     IDIAG_ERROR_NONE                    0
#define     IDIAG_ERROR_GENERIC                 1
#define     IDIAG_ERROR_INVALID_PARAM           2
#define     IDIAG_ERROR_NOT_SUPPORTED           3


//*****************************************************************************
// Pulled from tools/sdk/win32/include/winerror.h
#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS                    0L
#endif
#define ERROR_INVALID_FUNCTION           1L
#define ERROR_INVALID_DATA               13L
#define ERROR_BAD_COMMAND                22L
#define ERROR_INVALID_PARAMETER          87L


//*****************************************************************************
// Added to enable alpha blended color cursors
#define NV_ESC_GET_CURSOR_ALPHA             0x6E78      // 'nv' + 2
#define NV_ESC_SET_CURSOR_ALPHA             0x6E79      // 'nv' + 3

//*****************************************************************************
// Added to test mode switching by OpenGL or anything else that uses the counter
#define NV_ESC_INC_MODE_SWITCH              0x6E7A      // 'nv' + 4

//*****************************************************************************
// Pulled from common/win9x/inc/GNPDEV.H
// The generic non-Windows defined escapes we support are:
// For GETPDEVICE, the first DWORD pointed to by lpOut will get a linear
// address of the PDEVICE and the second DWORD gets a 16:16 ptr to the
// PDEVICE.
#define GN_ESC_GET_PDEVICE                  0x6E7B
#define GETPDEVICE                          GN_ESC_GET_PDEVICE

// The lpIn argument should point to 2 DWORDS.  The first DWORD is the
// linear base address the newly allcated seelctor should get. The
// second DWORD specifies the limit of the selector. The return value
// is a 16bit WORD that is placed where lpOut points to. The 16bit value
// is the newly allocated slector. It is 0 if the routine failed.
#define GN_ESC_ALLOC_SELECTOR_AND_MAP       0x6E7C

// The first 16 bit WORD pointed to by lpIn has the selector to free.
#define GN_ESC_UNMAP_AND_FREE_SELECTOR      0x6E7D

// If lpIn is NULL, then this is a query type call and lpOut points to
// a dword that returns the number of modes in the registry list. If lpIn
// is not NULL, then it points to one of the following three values
// (defined in modeext.h) MODETYPE_MODEENTRYNORR, MODETYPE_MODEENTRY,
// or MODETYPE_MODEENTRYANDPITCH. lpOut points to an array of
// MAX_MODEENTRYS structures of the format specified by the lpIn
// parameter (i.e. either MODEENTRYNORR, MODEENTRY, or MODEENTRYANDPITCH.)
// The list of modes will be placed into this lpOut buffer. If the
// routine fails, the return code is 0; if it succeeeds, it returns 1.
#define NV_ESC_GET_REGISTRY_MODE_LIST       0x6E7E

// This escape gets the local base path in the registry to be used
// by driver components wishing to write board specific info into the
// registry. How to use the string returned by this escape is discussed
// in ....drivers\common\nv4\inc\nvreg.h. The lpOut parameter points
// to a buffer that is at least 128 characters in length.
// The first DWORD in lpOut is one one of the predefined registry keys
// like HKEY_LOCAL_MACHINE or HKEY_CURRENT_USER, etc. Starting right
// after the first dword is a string which is the registry path.
// NOTE: You have to issue the escape to the correct display driver in
// a multi-mon scenario. How do you know which one? There is code in
// the DirectX driver which shows how to get the correct display HDC.
#define NV_ESC_GET_LOCAL_REGISTRY_PATH      0x6E7F

// This escape indicates that we are running on an NVidia board/driver
// combination. It returns this escape value as a dword pointed to by
// lpOut argument of the escape call. The second dword pointed to by
// the lpOut argument is the NVidia device number that the display
// driver is talking to -- this is a 1 based number. The third dword
// pointed to by lpOut is a physical CRTC index. Don't use this right
// now -- it is just for testing purposes.
#define NV_ESC_IS_NVIDIA_BOARD_AND_DRIVER   0x6E80

// This escape will fill in the following structure into the memory
// area pointed to by the lpOut argument to the escape call.
#define NV_ESC_GET_HARDWARE_POINTERS        0x6E81

typedef struct  tagHARDWAREPOINTERS
{
    unsigned long   lp16FrameBuffer;        // 16:16 ptr to frame buffer
    unsigned long   lp32FrameBuffer;        // flat  ptr to frame buffer
    unsigned long   lp16Chip;               // 16:16 ptr to chip space
    unsigned long   lp32Chip;               // flat  ptr to chip space
} HARDWAREPOINTERS;

// This escape will get a flat ptr to the shared DirectX data. It
// is returned as the first dword of the lpOut parameter.
#define NV_ESC_GET_FLAT_DIRECTX_SHARED_DATA 0x6E82

// This escape will validate a mode on a given display device.
// The lpIn ptr will point to the following structure.  The
// Escape will always return TRUE. lpOut points to a dword. If
// the requested mode can be set on the specified device then
// the dword pointed to by lpOut will be set to 1. If the mode
// cannot be set on the device, the dword will be 0. The
// dwDeviceType field in the structure can be one of the
// MONITOR_TYPE_XXXX constants in nvcm.h.
#define NV_ESC_VALIDATE_DEVICE_MODE         0x6E83

typedef struct  tagVALMODEEXT
{
    unsigned long   dwHRes;
    unsigned long   dwVRes;
    unsigned long   dwBpp;
    unsigned long   dwDeviceType;
} VALMODEXT;

// This escape is used to inform the display driver what the
// desired output device should be on the next modeset. The
// lpIn parameter should point to the following structure.
// The dwSuccessFlag is ignored for this escape (can be 0.) The
// lpOut parameter should point to a dword which will be set
// to a 1 if the display driver successfully handles the escape
// and 0 if the display driver could not handle the call.
// The escape call itself will always return non-0 if the
// display driver recieved the escape.
// The dwDeviceType holds one of the MONITOR_TYPE_XXXX constants
// defined in nvcm.h. If dwDeviceType is either the NTSC or PAL
// type, then dwTvFormat will be one of the specific NTSC or PAL
// formats specified in nvcm.h like NTSC_M, NTSC_J, PAL_A etc.
#define NV_ESC_SET_DISPLAY_DEVICE_TYPE      0x6E84

// This escape is used to get from the display driver what the
// current output device actually is. The lpOut parameter should
// point to the following structure. Upon returning, the dwSuccessFlag
// will be non-0 if the display driver successfully filled in the
// dwDeviceType and dwTvFormat fields with the current device type
// and tv format respectively. Otherwise, it will hold FALSE.
// The escape call itself will always return non-0 if the
// display driver recieved the escape.
// The dwDeviceType will hold one of the MONITOR_TYPE_XXXX constants
// defined in nvcm.h. If dwDeviceType is either the NTSC or PAL
// type, then dwTvFormat will be one of the specific NTSC or PAL
// formats specified in nvcm.h like NTSC_M, NTSC_J, PAL_A etc.
#define NV_ESC_GET_DISPLAY_DEVICE_TYPE      0x6E85

typedef struct  tagSETGETDEVTYPE
{
    unsigned long   dwSuccessFlag;
    unsigned long   dwDeviceType;
    unsigned long   dwTvFormat;
} SETGETDEVTYPE;


// Escape used to get the current refresh rate from the display driver

#define NV_ESC_GET_REFRESH_INFO             0x6E86

// lpIn = NULL
// lpOut points to DWORD to receive the current refresh rate as follows:
// (-1) = Optimal
//  0   = Adapter Default
// Any other value represents the actual refresh rate in use


// This escape will allow 32 bit code to call SetCursor() directly,
// bypassing GDI and the goofyness inherent in it.  16 bit code can
// find the SetCursor entry point (ordinal 102 in the display driver).
//
// lpIn = pointer to a genuine cursor structure
//
// Note: this allows 32x32, 64x64 (and 32x64 & 64x32) cursors if the
// hardware permits it.  Also, in debug builds, pixel depths of 33 and
// 34 will expose the ARGB and premultiplied ARGB (where available).

#define GN_ESC_SET_CURSOR                   0x6E87

#ifdef WINNT
// lpIn = pointer to input structure to nvMoComp
// lpOut = pointer to a DWORD return code
#define NV_ESC_DDPRIV_DMABLIT               0x6E88

// lpIn = pointer to input structure to nvOverlayControl
// lpOut = pointer to a DWORD return code
#define NV_ESC_DDPRIV_OVERLAYCONTROL        0x6E89
#endif // WINNT

// lpIn pointer to a 32bit value that has the amount of memory
//      to allocate
// lpOut pointer to a 32bit value that is the flat ptr to the
//      block of memory that is at least as large as requested.
// The escape itself always returns TRUE. If the allocation is
// successful, lpOut will return pointing to a non-NULL value.
// NULL value indicates that the alloc failed. The alloc escape
// always allocates CACHED memory above 2Gig (SHARED memory).

// This escape allocates memory through the RM.
// lpIn should point to the following structure.
// The objectID should be a uniqueID for the object that the
// RM will associate with the memory block. This objectID will
// need to get passed back in to free the memory block.
// dwAllocFlags are things like NVOS02_FLAGS_PHYSICALITY_CONTIGUOUS
// specified in nvos.h
// lpOut should point to a dword which will get filled in with
// a ptr to a memory block of the size requested or NULL if the
// allocation was unsuccessful.
// The Escape itself will always return TRUE.
#define NV_ESC_ALLOC_MEMORY                 0x6E8A
typedef struct  _ESC_ALLOC_MEMORY
{
    unsigned long   dwObjectID;
    unsigned long   dwAllocFlags;
    unsigned long   dwNumBytes;
}   ESC_ALLOC_MEMORY;


// This escape always returns TRUE.
// This escape frees memory allocated by a previous call to
// NV_ESC_ALLOC_MEMORY.
// lpIn should point to the following structure.
// lpOut is not used.
#define NV_ESC_FREE_MEMORY                  0x6E8B
typedef struct  _ESC_FREE_MEMORY
{
    unsigned long   dwObjectID;
}   ESC_FREE_MEMORY;


// This is identical to the NV_ESC_GET_REGISTRY_MODE_LIST escape,
// EXCEPT that the list only contains desktop modes. No DDRAW only
// modes are included.
#define NV_ESC_GET_REGISTRY_DESKTOP_MODE_LIST   0x6E8C


// This escape allocates shared memory.
// lpIn should point to an ESC_ALLOC_SHARED_MEMORY_IN structure.
// dwNumBytes should be the size in bytes of the desired
// memory block. dwID can be used to request a ptr to the
// memory block at a later date.
// lpOut should point to an ESC_ALLOC_MEMORY_OUT structure.
// pMemBlock will be a flat pointer to the memory block.
// The Escape itself will always return TRUE.
#define NV_ESC_ALLOC_SHARED_MEMORY  0x6E8D
typedef struct  _ESC_ALLOC_SHARED_MEMORY_IN
{
    unsigned long   dwNumBytes;
    unsigned long   dwID;
}   ESC_ALLOC_SHARED_MEMORY_IN;

typedef struct  _ESC_ALLOC_SHARED_MEMORY_OUT
{
    unsigned long   pMemBlock;
}   ESC_ALLOC_SHARED_MEMORY_OUT;

// This escape frees memory allocated by a previous call to
// NV_ESC_ALLOC_SHARED_MEMORY.
// lpIn should point to an _ESC_FREE_SHARED_MEMORY_IN structure.
// dwID should be an ID that was passed in to some prior ALLOC.
// lpOut should point to an _ESC_FREE_SHARED_MEMORY_OUT structure.
// If TRUE, the dwSuccess field indicates the the memory block
// assoictaed with the dwID passied in was freed.
// This escape always returns TRUE.
#define NV_ESC_FREE_SHARED_MEMORY   0x6E8E
typedef struct  _ESC_FREE_SHARED_MEMORY_IN
{
    unsigned long   dwID;
}   ESC_FREE_SHARED_MEMORY_IN;

typedef struct  _ESC_FREE_SHARED_MEMORY_OUT
{
    unsigned long   dwSuccess;
}   ESC_FREE_SHARED_MEMORY_OUT;

// This escape gets a ptr to a previsouly allocated shared
// memory block.
// lpIn should point to an _ESC_GET_SHARED_MEMORY_IN structure.
// dwID should be an ID that was passed in to some prior ALLOC.
// lpIn should point to an _ESC_GET_SHARED_MEMORY_OUT structure.
// This escape always returns TRUE.
#define NV_ESC_GET_SHARED_MEMORY    0x6E8F
typedef struct  _ESC_GET_SHARED_MEMORY_IN
{
    unsigned long   dwID;
}   ESC_GET_SHARED_MEMORY_IN;

typedef struct  _ESC_GET_SHARED_MEMORY_OUT
{
    unsigned long   pMemBlock;
}   ESC_GET_SHARED_MEMORY_OUT;


//*****************************************************************************
// Pulled from hwspec/code/hwcntrl.asm
#define NV_ESC_DMAPUSHMODESWITCHCOUNT       0x6E90

// Also found in OpenGL/nv/nvInit.c
#define NV_ESC_GETAGPBASE                   0x6E91
#define NV_ESC_GETACTIVECHANNELPTR          0x6E92


//*****************************************************************************
// Added for PGC (aka SLI) communications between multimonitor instances of the
// GDI driver. Can be also used to pull information about PGC drivers. The
// actual action is specified by the command field in the lpIn parameter.
#define NV_ESC_PGC_COMMAND                  0x6E93

//*****************************************************************************
//
// NV11 TwiNView escape for controlling dual CRTC functionality
//

#define NV_ESC_SETGET_TWINVIEW_DATA         0x6E94
#include "nvMultiMon.h" // Contains all definitions and structures used in the NV_ESC_SETGET_TWINVIEW_DATA escape call.

#define NV_ESC_ENABLE_DISABLE_SOFTWARE_CURSOR 0x6E95

// For both of these escapes the lpOut ptr points to a DWORD.
// For SET, the driver will get this dword and save it.
// For GET, the driver will place the cpl dword where lpOut points.
// The lpIn argument is not used.
#define NV_ESC_SET_CPL_DWORD                0x6E96
#define NV_ESC_GET_CPL_DWORD                0x6E97

#ifndef NV_SIGNATURE
// This value also is defined in driver.h of display driver
#define NV_SIGNATURE 0x11223344
#endif

#ifndef NV_ESC_PRIMARY_INFO
#define NV_ESC_PRIMARY_INFO 0x7104
typedef struct _NV_PRIMARY_INFO
{
    unsigned long ulNVSignature;   // IN
    unsigned long ulNumDevices;    // IN
    unsigned long ulHeadNum;       // IN
    unsigned long ulReturnCode;    // OUT from display driver
    unsigned long ulWidth;         // OUT from display driver
    unsigned long ulHeight;        // OUT from display driver
    unsigned long ulDepth;         // OUT from display driver
    unsigned long ulPitch;         // OUT from display driver
    unsigned long ulPrimaryOffset; // OUT from display driver
    unsigned long ulNumDACs;       // OUT from display driver (NT only)
    unsigned long ulNumActiveDACs; // OUT from display driver
    unsigned long ulActiveDACs;    // OUT from display driver (NT only)
    unsigned long ulPanningPtr;    // OUT from display driver (9x only)
    unsigned long ulFullscreenPtr; // OUT from display driver (9x only)
} NV_PRIMARY_INFO;
#endif

#ifndef ESC_NV_QUERY_HEAD_REFRESH_RATE
#define ESC_NV_QUERY_HEAD_REFRESH_RATE 0x7106
#endif

//*****************************************************************************
//
// PGC subcommands and corresponding structures
//
#ifndef WINNT
typedef struct _MYPGCCMD
{
    unsigned long dwCommand;
    unsigned long dwParam1;
    unsigned long dwParam2;
} MYPGCCMD;

#define PGC_CONNECT_GDI 1

#endif  // WINNT

//*****************************************************************************
//
// Formerly common/win9x/inc/OPENGL.H
//
#ifndef WINNT       // These are redefined in the NT tree
// From common/win9x/inc/opengl.h
// These escape values are fixed by Windows
#define OPENGL_CMD                          4352
#define OPENGL_GETINFO                      4353

#define     OPENGL_GETINFO_DRVNAME              0

#define     OPENGL_ICD_VER                      2
#define     OPENGL_DRIVER_VER                   0x10000     // Version 1.0000

#define     OPENGL_KEY_NAME                     "RIVATNT"

    typedef struct {
        unsigned long   ulVersion;
        unsigned long   ulDriverVersion;
        unsigned char   awch[ 129 ];
    } OPENGL_INFO;

#endif  // WINNT

#endif  // _ESCAPE_H_
