/*==========================================================================;
 *
 *  Copyright (C) 1999 NVidia Corporation. All Rights Reserved.
 *
 *  File:       dxshare.h
 *
 *  Content:    This file has a data structure that is shared between
 *              the display driver and the DirectX driver.
 *
 *      NOTES:  Due to the need to h2inc this file and also compiler
 *              structure packing parameters, please only use
 *              unsigned longs as members of this structure and
 *              do NOT have any substructures. The real data
 *              DirectX needs will go in its own private data
 *              structure that it allocates itself when the DirectX
 *              driver is laoded. This shared data structure is ONLY
 *              needed for the display driver to send over a few
 *              instance specific things to the DirectX driver
 *              and the DirectX driver to send back a few instance
 *              specific things to the display driver.
 *
 ***************************************************************************/

#ifndef _DXSHARE_H_
#define _DXSHARE_H_

// I don't think we will have more than this for a while
#define MAX_HEADS       4

typedef struct  tagDISPDRVDIRECTXCOMMON
{
    // The display driver fills these in for the DirectX driver to use
    unsigned long   pFlatDibeng;        // Flat ptr to Driver PDEVICE
    unsigned long   dwNVDeviceID;       // 1 based board instance
    unsigned long   dwDeviceVersion;    // NVidia chip/software version
    unsigned long   dwRootHandle;       // Root allocated from RM by driver
    unsigned long   dwScreenLinearBase; // Linear base address of framebuffer
    unsigned long   dwDeviceLinearBase; // Linear base address of chip
    unsigned long   dwDeviceHandle;     // VDD handle for this device
    unsigned long   dwHeadNumber;       // 0 or 1
                                        // For single head boards and
                                        // multi-head boards in clone mode
                                        // this will be 0.

    // There are some wierd DirectX apps that assume the mode table
    // is created in 16bit land before even calling the 32bit
    // DirectX driver. Hence, DirectX needs amode table around and
    // hanging out so that it can just grab. It doesn't have the
    // opportunity to build one before it needs it. This is a ptr to
    // MODEENTRYANDPITCH structures defined in modeext.h
    unsigned long   lpMasterModeList;

    // This is a 32:32 (FAR32) ptr to a callback function that DirectX
    // can invoke only when it is within a pfnInterfaceFunc routine
    // shown below. Pass in the pDispDriverHWLogDevCX as the context
    // data for this function call.
    unsigned long   dwDrvCallbackOffset;
    unsigned long   dwDrvCallbackSelector;
    unsigned long   pDispDriverHWLogDevCX;

    ////////////////////////////////////////////////////////////////////
    // The DirectX driver fills these in for the display driver to use.
    unsigned long   dwFlatDataSel;
    unsigned long   dwFlatCodeSel;

    // 32 Bit flat ptr to the DDHALInfo structure
    unsigned long   pHalInfo;

    // size of DDthe HALINFO structure in bytes
    unsigned long   dwHALInfoSize;

    // The hInstance of the DirectX driver passed to its DllMain proc
    unsigned long   hDirectXInstance;

    // cached values of the previous display mode (used to detect true mode changes)
    // do not use these to get the current mode information
    unsigned long   deWidth;
    unsigned long   deHeight;
    unsigned long   deBitsPixel;

    // 32bit flat ptr to a procedure which accepts the following:
    // eax - ptr to this structure DISPDRVDIRECTXCOMMON
    // ebx - one of the GDIDX_XXX equates below
    // Basically this procedure can be called by the display driver
    // whenever the driver wants to inform DirectX about something.
    // The interface is agreed upon solely by the display driver
    // and the DirectX driver. This procedure will be called directly
    // (with no thunking) from the display driver. Hence, it cannot
    // call any system routines, since although we are running in
    // 32bit flat land at the time we are executing it, we will really
    // be running as a sub-routine of the display driver! To return,
    // this procedure MUST execute a FAR32 return -- you must do this
    // in assembly -- the 32bit C compiler cannot do this for you!
    unsigned long   pfnInterfaceFunc;

    // DirectX context data ptr. The display driver has no idea
    // what this points to -- it is solely for DirectDraw's use.
    unsigned long   pDirectXData;
    unsigned long   pNvPrimarySurf;
    unsigned long   dwModeNumber;
    unsigned long   primaryBufferIndex;

    unsigned long   dwPhysicalHeadNumber;

}   DISPDRVDIRECTXCOMMON;

// These flags indicate what type of Interface call this is
// When calling pfnInterfaceFunc from the display driver, if ebx =
// GDIDX_PRE_SETINFOINIT, then you should fill in the 16bit portion of
// the DDHALINFO structure. Basically the "16 bit portion" is the
// vmiData, FourCC, ModeList, and Callback List fields.
#define GDIDX_PRE_SETINFOINIT           0x01
#define GDIDX_ENTER_FULL_SCREEN_DOS     0x02
#define GDIDX_EXIT_FULL_SCREEN_DOS      0x03
#define GDIDX_PRE_MODESET_NOTIFY        0x04
#define GDIDX_POST_MODESET_NOTIFY       0x05
#define GDIDX_DIRECTX_OS_VERSION        0x06
#define GDIDX_DRIVER_UNLOADING          0x07
#define GDIDX_EARLY_DX_CONNECT          0x08

// This is a DCI escape call that I made up. It is not part of the
// standard DCI escape set. but it should be! It tells the display
// driver that the DirectX driver is going down.
#define DDNVCREATEDRIVEROBJECT      98
#define DDNVDESTROYDRIVEROBJECT     99

// DirectX needs to understand the standard part of the PDEVICE
// which is the DIBENGING structure. Unfortunately, including
// dibeng.inc won't build so I just have the structure here.
typedef struct  _tagDIBENGINEHDR1
{
    unsigned short      deType;
    unsigned short      deWidth;
    unsigned short      deHeight;
    unsigned short      deWidthBytes;
    unsigned char       dePlanes;
    unsigned char       deBitsPixel;
    unsigned long       deReserved1;
    unsigned long       deDeltaScan;
    unsigned long       delpPDevice;
    unsigned long       deBitsOffset;
    unsigned short      deBitsSelector;
    unsigned short      deFlags;
    unsigned short      deVersion;
    unsigned long       deBitmapInfo;
    unsigned long       deBeginAccess;
    unsigned long       deEndAccess;
    unsigned long       deDriverReserved;
}   DIBENGINEHDR1;

// When certain events happen in 16bit, there is a special call
// made into the DX driver via the pfnInterfaceFunc ptr in the
// DISPDRVDIRECTXCOMMON structure. This is not a thunked call
// so while executing the code in these calls, the DX driver is
// running as a sub-routine of the display driver and it cannot
// make system calls. To make calls into the OS to alloc memory,
// etc, DX must call back to the display driver and have it make
// the system calls on its behalf. To do this the DX driver
// calls the function pointed at by the dwDrvCallbackOffset and
// dwDrvCallbackSelector fields in the DISPDRVDIRECTXCOMMON
// structure. This DrvCallback routine has the following interface.
// int FAR32 PASCAL DrvCallbackFunc(DWORD pDispDriverHWLogDevCX,
// DWORD dwFunction, void *pIn, void *pOut) and can perform a
// variety of services based upon the dwFunction argument.
// These are listed below.

// For the alloc memory service, set dwFunction to this equate
// pIn should point to the following structure.
// The objectID should be a uniqueID for the object that the
// RM will associate with the memory block. This objectID will
// need to get passed back in to free the memory block.
// dwAllocFlags are things like NVOS02_FLAGS_PHYSICALITY_CONTIGUOUS
// specified in nvos.h
// pOut should point to a dword which will get filled in with
// a ptr to a memory block of the size requested or NULL if the
// allocation was unsuccessful.
#define NV_DRVSERVICE_ALLOC_MEMORY          0x01
typedef struct  _DRVSERVICE_ALLOC_MEMORY
{
    unsigned long   dwObjectID;
    unsigned long   dwAllocFlags;
    unsigned long   dwNumBytes;
}   DRVSERVICE_ALLOC_MEMORY;

// For the free memory service, set dwFunction to this equate
// Then pIn should point to the following struct. The service
// always returns TRUE. The pOut argument is not used.
#define NV_DRVSERVICE_FREE_MEMORY           0x02
typedef struct  _DRVSERVICE_FREE_MEMORY
{
    unsigned long   dwObjectID;
}   DRVSERVICE_FREE_MEMORY;

// These two services should be called with the lpIn argument equal
// to the lpDibeng. lpDibeng should be a field in the HALINFO table.
// It is a 16:16 ptr to the DIBENGINE structure.
// These two services may actually be called even if the DX driver
// is not running as a subroutine of the display driver because
// these services make no system calls, so it does not matter what
// "space" the owner is running in.
#define NV_DRVSERVICE_BEGINACCESS           0x03
#define NV_DRVSERVICE_ENDACCESS             0x04

#endif  // _DXSHARE_H_

