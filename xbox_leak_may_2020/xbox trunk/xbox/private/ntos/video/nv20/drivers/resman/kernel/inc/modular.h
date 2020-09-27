 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/***************************** Modular includes *****************************\
*                                                                           *
* Module: MODULAR.H                                                         *
*   This module contains structure and prototype definitions used in the    *
*   modular class architechure.                                             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#ifndef _MODULAR_H_
#define _MODULAR_H_

#include <gr.h>

//
// Define modular class structures.
//
typedef struct _def_video_from_mem_object    VIDEOFROMMEMOBJECT,    *PVIDEOFROMMEMOBJECT;
typedef struct _def_video_decoder_object     VIDEODECODEROBJECT,    *PVIDEODECODEROBJECT;
typedef struct _def_video_decompressor_object VIDEODECOMPRESSOROBJECT, *PVIDEODECOMPRESSOROBJECT;
typedef struct _def_parallel_bus_object      PARALLELBUSOBJECT,     *PPARALLELBUSOBJECT;
typedef struct _def_monitor_bus_object       MONITORBUSOBJECT,      *PMONITORBUSOBJECT;
typedef struct _def_serial_bus_object        SERIALBUSOBJECT,       *PSERIALBUSOBJECT;
typedef struct _def_video_overlay_object     VIDEO_OVERLAY_OBJECT,  *PVIDEO_OVERLAY_OBJECT;

// Define function pointers
typedef V032     (*POVERLAYSERVICEPROC)(PHWINFO, POBJECT);

//
// CLASS 63 NV_VIDEO_FROM_MEMORY
//
struct _def_video_from_mem_object
{
    BUFFEROBJECT            BufferObj;      // ALWAYS keep this first!!!!
    COMMONOBJECT            Common;
    //
    // State in this structure is shared between the HAL and RM.
    //
    VIDEOFROMMEMHALOBJECT   HalObject;
    VBLANKNOTIFY            VBlankNotify[2];
    U032                    BufferToggle;
    U032                    InitState;
};

//
// VIDEO_LUT_CURSOR_DAC class object.
//
typedef struct _def_video_lut_cursor_dac_object
{
    COMMONOBJECT                     Common;

    //
    // This state is shared between the RM and the HAL.
    //
    VIDEO_LUT_CURSOR_DAC_HAL_OBJECT  HalObject;

    //
    // This state is (currently) RM only.
    //
    struct {
        PDMAOBJECT  SemaphoreCtxDma;
        U032        Offset;
        U032        *ReleaseAddr;
        U032        ReleaseValue;
        U032        ReleaseTrigger;
        VBLANKCALLBACK ReleaseCallback;
    } Semaphore;

    struct {
        PDMAOBJECT   ImageCtxDma;
        U032         Offset;                       // Byte offset (from ImageContextDma) of top left pixel
        U032         Pitch;                        
        U032         NotifyAction;                   
        U032         NotifyTrigger;
        S016         DeltaX;
        S016         DeltaY;
        U032         MultiSync;
        VBLANKCALLBACK ImageCallback;
    }                                Image[2];
    U032                             CompletedFlipCount;
    struct {
        PDMAOBJECT     LUTCtxDma;
        U032           Offset;                       // Byte offset (from LUTContextDma)
        U032           NotifyAction;                   
        U032           NotifyTrigger;
        VBLANKCALLBACK LUTCallback;
    }                                LUT[2];
    struct {
        PDMAOBJECT     CursorCtxDma;
        U032           Offset;                       // Byte offset (from CursorContextDma) of top left pixel
        U032           Width;
        U032           Height;
        U032           ColorFormat;
        U032           NotifyAction;                   
        U032           NotifyTrigger;
        VBLANKCALLBACK CursorImageCallback;
    }                                CursorImage[2];
    U032                             CursorPoint;     // y_x
    VBLANKCALLBACK                   CursorPointCallback;
    U032                             ScanRegion;      // U015_U016 stop_start 
    U032                             Polarity;        // polarity bit (blit in/out of range)
    struct {
#if 0
        U032       VisibleImageWidth;    
        U032       VisibleImageHeight;   
        U032       HorizontalBlankStart;
        U032       HorizontalBlankWidth;
        U032       HorizontalSyncStart;
        U032       HorizontalSyncWidth;
        U032       VerticalBlankStart; 
        U032       VerticalBlankHeight; 
        U032       VerticalSyncStart; 
        U032       VerticalSyncHeight; 
        U032       TotalWidth; 
        U032       TotalHeight;     
        U032       PixelClock;
        U032       ColorFormat;                  
        U032       PixelDepth;
        struct {
            unsigned int DoubleScanMode         : 1;
            unsigned int InterlacedMode         : 1;
            unsigned int HorizontalSyncPolarity : 1;
            unsigned int VerticalSyncPolarity   : 1;
            unsigned int unusedBits1            : 12;
            unsigned int unusedBits2            : 16;
        }          Format;
#endif
        U032       NotifyAction;                   
        U032       NotifyTrigger;
    }                                Dac[2];
    
    struct {
        U032    NotifyAction;
        U032    NotifyTrigger;
        U032    HotPlugEventDescriptor;
    }HotPlug;
    
    U032    DisplayType;                                // may be CRT, TV, DFP
    U032    TVStandard;                                 // variations of NTSC and PAL
    U032    RefreshRate;
} VIDEO_LUT_CURSOR_DAC_OBJECT, *PVIDEO_LUT_CURSOR_DAC_OBJECT;

//
// CLASS 047 VIDEO_OVERLAY
//
/* typedef */ struct _def_video_overlay_object
{
    COMMONOBJECT                    Common;
    //
    // State in this structure is shared between the HAL and RM.
    //
    U032                            InitState;
    VIDEO_OVERLAY_HAL_OBJECT        HalObject;
    //
    // This per-buffer state is RM only.
    //
    struct {
        PDMAOBJECT                  OverlayCtxDma;
        U032                        NotifyAction;                   
        VBLANKCALLBACK              OverlayCallback;
    } Overlay[2];
};

//
// CLASS 04D NV_EXTERNAL_VIDEO_DECODER
//
struct _def_video_decoder_object
{
    COMMONOBJECT    Common;
    VIDEODECODERHALOBJECT   HalObject;
    PDMAOBJECT      ContextDmaImage[2];
    PDMAOBJECT      ContextDmaVbi[2];
    U032            LastEventProcessed;
    U032            LastVbiBufferProcessed;
    U032            LastImageBufferProcessed;
};

//
// CLASS 04E NV_EXTERNAL_VIDEO_DECOMPRESSOR
//
struct _def_video_decompressor_object
{
    COMMONOBJECT    Common;
    VIDEODECOMPRESSORHALOBJECT  HalObject;
    PDMAOBJECT      ContextDmaData[2];
    PDMAOBJECT      ContextDmaImage[2];
};

//
// CLASS 04F NV_EXTERNAL_PARALLEL_BUS
//
struct _def_parallel_bus_object
{
    COMMONOBJECT    Common;
    PARALLELBUSHALOBJECT    HalObject;
    U032            BusType;
    U032            InterruptNotifyPending;
};
//
// CLASS 050 NV_EXTERNAL_MONITOR_BUS
//
struct _def_monitor_bus_object
{
    COMMONOBJECT    Common;
    U032            BusType;
    struct {
        U016        DataSize;
        U016        AddressSize;
        U032        Address;
        U032        Data;
    } Write;
    struct {
        U016        DataSize;
        U016        AddressSize;
        U032        Address;
        U032        Notify;
    } Read;
    U032            InterruptNotifyPending;
};

//
// CLASS 051 NV_EXTERNAL_SERIAL_BUS
//
struct _def_serial_bus_object
{
    COMMONOBJECT    Common;
    U032            BusType;
    struct {
        U016        DataSize;
        U016        AddressSize;
        U032        Address;
        U032        Data;
    } Write;
    struct {
        U016        DataSize;
        U016        AddressSize;
        U032        Address;
        U032        Notify;
    } Read;
    U032            DefaultPortID;
    U032            InterruptNotifyPending;
};

//
// NVDE Object
//
#define NVDE_MAX_EVENTS                             32

#define NVDE_FLAG_PGRAPH_FIFO_ACCESS_ENABLED        4
#define NVDE_FLAG_CACHES_REASSIGN_ENABLED           2
#define NVDE_FLAG_CACHE1_PULL0_ACCESS_ENABLED       1

typedef struct _dev_nvde_event
{
    U032 code;
    U032 flags;
    U032 param[6];
} NVDEEVENT, *PNVDEEVENT;

typedef struct _def_nvde_object
{
    COMMONOBJECT Common;    // see gr.h
    U064 EventHandle;       // passed in as part of the alloc routine
    U032 EventClass;

    U032      EventBufferCount;
    NVDEEVENT EventBuffer[NVDE_MAX_EVENTS];
} NVDEOBJECT, *PNVDEOBJECT;

RM_STATUS NvdeAlloc(U032, VOID *);
RM_STATUS NvdeFree(U032, U032);
V032      NvdeAppendEventForDebugger (PNVDEOBJECT,U032,U032,U032,U032,U032,U032,U032,U032);

//
// Define modular class function prototypes.
//

//
// NV04_VIDEO_LUT_CURSOR_DAC
//
RM_STATUS class046SetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046StopImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046StopLUT(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046StopCursorImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046StopDAC(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046SetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046SetImageCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046SetLUTCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046SetCursorCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046SetPanOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046GetOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046SetImageValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046SetLUTValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046SetCursorImageValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046SetCursorPoint(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class046SetDACValues(PHWINFO, POBJECT, PMETHOD, U032, U032);

//
// NV04_VIDEO_OVERLAY
//
RM_STATUS class047SetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class047StopOverlay(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class047SetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class047SetOverlayCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class047GetOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class047SetOverlayValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class047SetOverlayPointOutNow(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class047StopOverlayNow(PHWINFO, POBJECT, U032, V032, RM_STATUS);

//
// NV05_VIDEO_LUT_CURSOR_DAC
//
RM_STATUS class049SetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049StopImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049StopLUT(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049StopCursorImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049StopDAC(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049SetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049SetImageCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049SetLUTCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049SetCursorCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049SetPanOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049GetOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049SetImageValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049SetLUTValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049SetCursorImageValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049SetCursorPoint(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class049SetDACValues(PHWINFO, POBJECT, PMETHOD, U032, U032);

//
// NV_VIDEO_FROM_MEMORY
//
RM_STATUS class63SetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class63SetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class63StopTransfer(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class63SetVideoOutput(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class63SetImageCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class63ImageScan(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class63GetOffsetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class63VBlank(PHWINFO);

//
// NV_VIDEO_SCALER
//
RM_STATUS class64SetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class64SetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class64SetVideoOutput(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class64SetVideoInput(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class64SetDeltaDuDx(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class64SetDeltaDvDy(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class64SetPoint(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class64LoadContext(PHWINFO, PVIDEOSCALEROBJECT);

//
// NV_VIDEO_COLOR_KEY
//
RM_STATUS class65SetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class65SetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class65SetVideoOutput(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class65SetVideoInput(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class65SetColorFormat(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class65SetColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class65SetPoint(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class65SetSize(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class65LoadContext(PHWINFO, PVIDEOCOLORKEYOBJECT);
//
// NV_VIDEO_CAPTURE_TO_MEMORY
//
RM_STATUS class66SetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class66SetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class66StopTransfer(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class66SetVideoInput(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class66SetImageCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class66SetImageNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class66ImageCapture(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class66SetVbiCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class66SetVbiNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class66VbiCapture(PHWINFO, POBJECT, PMETHOD, U032, U032);

//
// NV10_VIDEO_LUT_CURSOR_DAC
//
RM_STATUS class067SetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067StopImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067StopLUT(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067StopCursorImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067StopDAC(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067SetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067SetImageCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067SetLUTCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067SetCursorCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067SetPanOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067GetOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067SetImageValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067SetLUTValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067SetCursorImageValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067SetCursorPoint(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class067SetDACValues(PHWINFO, POBJECT, PMETHOD, U032, U032);

//
// NV15_VIDEO_LUT_CURSOR_DAC
//
RM_STATUS class07CSetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CStopImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CStopLUT(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CStopCursorImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CStopDAC(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetImageCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetLUTCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetCursorCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetSemaphoreCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetSemaphoreOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetSemaphoreRelease(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetPanOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetOffsetRange(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CGetOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetImageValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetLUTValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetCursorImageValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetCursorPoint(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetDACValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07CSetHotPlug(PHWINFO, POBJECT, PMETHOD, U032, U032);
V032 HotPlugService(PHWINFO pDev);

//
// NV_VIDEO_SOURCE
//
RM_STATUS class74SetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class74SetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class74SetVideoOutput(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS osValidateVideoSource( char *, U032 , U032 );


//
// NV_CONTEXT_DMA_IN_MEMORY
//
RM_STATUS class61SetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class61SetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class61SetDmaSpecifier(PHWINFO, POBJECT, PMETHOD, U032, U032);


//////////////////////////////////////////////////////////////////////////////
// 
// CLASS012 - NV1_BETA_SOLID
//
// Method aliases
#define class012Notify              grHWMethod
#define class012SetContextDmaNotifies grHWMethod
#define class012SetBeta             grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS017 - NV1_IMAGE_SOLID
//
// Method aliases
#define class017Notify              grHWMethod
#define class017SetContextDmaNotifies grHWMethod
#define class017SetColorFormat      grHWMethod
#define class017SetColor            grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS018 - NV1_IMAGE_PATTERN
//
// Method aliases
#define class018Notify              grHWMethod
#define class018SetContextDmaNotifies grHWMethod
#define class018SetColorFormat      grHWMethod
#define class018SetMonochromeFormat grHWMethod
#define class018SetPatternShape     grHWMethod
#define class018SetColor0           grHWMethod
#define class018SetColor1           grHWMethod
#define class018SetPattern          grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS019 - NV1_IMAGE_BLACK_RECTANGLE
//
// Method aliases
#define class019Notify              grHWMethod
#define class019SetContextDmaNotifies grHWMethod
#define class019SetRectangle        grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS01C - NV1_RENDER_SOLID_LIN
//
RM_STATUS class01CSetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01CSetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01CSetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01CSetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01CSetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01CSetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class01CNotify              grHWMethod
#define class01CSetContextDmaNotifies grHWMethod
#define class01CSetColorFormat      grHWMethod
#define class01CColor               grHWMethod
#define class01CLin                 grHWMethod
#define class01CLin32               grHWMethod
#define class01CPolyLin             grHWMethod
#define class01CPolyLin32           grHWMethod
#define class01CColorPolyLin        grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS01D - NV1_RENDER_SOLID_TRIANGLE
//
RM_STATUS class01DSetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01DSetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01DSetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01DSetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01DSetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01DSetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class01DNotify              grHWMethod
#define class01DSetContextDmaNotifies grHWMethod
#define class01DSetColorFormat      grHWMethod
#define class01DColor               grHWMethod
#define class01DTriangle            grHWMethod
#define class01DTriangle32          grHWMethod
#define class01DTrimesh             grHWMethod
#define class01DTrimesh32           grHWMethod
#define class01DColorTriangle       grHWMethod
#define class01DColorTrimesh        grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS01E - NV1_RENDER_SOLID_RECTANGLE
//
RM_STATUS class01ESetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01ESetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01ESetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01ESetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01ESetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01ESetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class01ENotify              grHWMethod
#define class01ESetContextDmaNotifies grHWMethod
#define class01ESetColorFormat      grHWMethod
#define class01EColor               grHWMethod
#define class01ERectangle           grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS01F - NV1_IMAGE_BLIT
//
RM_STATUS class01FSetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01FSetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01FSetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01FSetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01FSetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01FSetContextSurfaceSource(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01FSetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class01FSetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class01FNotify              grHWMethod
#define class01FSetContextDmaNotifies grHWMethod
#define class01FPointIn             grHWMethod
#define class01FPointOut            grHWMethod
#define class01FSize                grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS021 - NV1_IMAGE_FROM_CPU
//
RM_STATUS class021SetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class021SetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class021SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class021SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class021SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class021SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class021SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class021Notify              grHWMethod
#define class021SetContextDmaNotifies grHWMethod
#define class021SetColorFormat      grHWMethod
#define class021Point               grHWMethod
#define class021Size                grHWMethod
#define class021SizeIn              grHWMethod
#define class021Color               grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS030 - NV1_NULL
//

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS036 - NV3_STRETCHED_IMAGE_FROM_CPU
//
RM_STATUS class036SetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class036SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class036SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class036SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class036SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class036SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class036Notify              grHWMethod
#define class036SetContextDmaNotifies grHWMethod
#define class036SetColorFormat      grHWMethod
#define class036SizeIn              grHWMethod
#define class036DeltaDxDu           grHWMethod
#define class036DeltaDyDv           grHWMethod
#define class036Clip                grHWMethod
#define class036Point12d4           grHWMethod
#define class036Color               grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS037 - NV3_SCALED_IMAGE_FROM_MEM
//
RM_STATUS class037SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class037SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class037SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class037SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class037SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class037Notify              grHWMethod
#define class037SetContextDmaNotifies grHWMethod
#define class037SetContextDmaImage  grHWMethod
#define class037SetColorFormat      grHWMethod
#define class037Clip                grHWMethod
#define class037RectangleOut        grHWMethod
#define class037DeltaDuDx           grHWMethod
#define class037DeltaDvDy           grHWMethod
#define class037ImageIn             grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS039 - NV3_MEMORY_TO_MEMORY_FORMAT
//
// Method aliases
#define class039Notify              grHWMethod
#define class039SetContextDmaNotifies grHWMethod
#define class039SetContextDmaBufferIn grHWMethod
#define class039SetContextDmaBufferOut grHWMethod
#define class039BufferTransfer      grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS048 - NV3_DX3_TEXTURED_TRIANGLE
//
RM_STATUS class048SetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class048SetContextSurfaceColor(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class048SetContextSurfaceZeta(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class048Notify              grHWMethod
#define class048SetContextDmaNotifies grHWMethod
#define class048SetTextureCtxDma    grHWMethod
#define class048TextureOffset       grHWMethod
#define class048TextureFormat       grHWMethod
#define class048Filter              grHWMethod
#define class048FogColor            grHWMethod
#define class048ControlOut0         grHWMethod
#define class048ControlOut1         grHWMethod
#define class048ControlPoint        grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS04B - NV3_GDI_RECTANGLE_TEXT
//
RM_STATUS class04BSetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04BSetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04BSetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04BSetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04BSetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class04BNotify              grHWMethod
#define class04BSetContextDmaNotifies grHWMethod
#define class04BSetColorFormat      grHWMethod
#define class04BSetMonochromeFormat grHWMethod
#define class04BColor1A             grHWMethod
#define class04BUnclippedRectangle  grHWMethod
#define class04BClipB               grHWMethod
#define class04BColor1B             grHWMethod
#define class04BClippedRectangle    grHWMethod
#define class04BClipC               grHWMethod
#define class04BColor1C             grHWMethod
#define class04BSizeC               grHWMethod
#define class04BPointC              grHWMethod
#define class04BMonochromeColor1C   grHWMethod
#define class04BClipD               grHWMethod
#define class04BColor1D             grHWMethod
#define class04BSizeInD             grHWMethod
#define class04BSizeOutD            grHWMethod
#define class04BPointD              grHWMethod
#define class04BMonochromeColor1D   grHWMethod
#define class04BClipE               grHWMethod
#define class04BColor0E             grHWMethod
#define class04BColor1E             grHWMethod
#define class04BSizeInE             grHWMethod
#define class04BSizeOutE            grHWMethod
#define class04BPointE              grHWMethod
#define class04BMonochromeColor01E  grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS04D - NV3_EXTERNAL_VIDEO_DECODER
//
RM_STATUS class04DNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DStopTransferVbi(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DStopTransferImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DSetContextDmaNotifies(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DSetContextDmaImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DSetContextDmaVbi(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DSetImageConfig(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DSetImageStartLine(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DSetImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DGetImageOffsetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DSetVbi(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04DGetVbiOffsetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);

RM_STATUS class04DService(PHWINFO);

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS04E - NV3_EXTERNAL_VIDEO_DECOMPRESSOR
//
RM_STATUS class04ENotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04EStopTransfer(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04ESetContextDmaNotifies(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04ESetContextDmaData(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04ESetContextDmaImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04EReadData(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04EWriteImage(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04EService(PHWINFO);

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS04F - NV3_EXTERNAL_PARALLEL_BUS
//
RM_STATUS class04FNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04FStopTransfer(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04FSetContextDmaNotifies(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04FWrite(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04FRead(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04FSetInterruptNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04FService(PHWINFO);

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS050 - NV3_EXTERNAL_MONITOR_BUS
//
RM_STATUS class050Create(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
RM_STATUS class050Destroy(PHWINFO, POBJECT);
RM_STATUS class050Notify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class050StopTransfer(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class050SetContextDmaNotifies(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class050Write(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class050Read(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class050SetInterruptNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS051 - NV3_EXTERNAL_SERIAL_BUS
//
RM_STATUS class051Create(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
RM_STATUS class051Destroy(PHWINFO, POBJECT);
RM_STATUS class051Notify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class051StopTransfer(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class051SetContextDmaNotifies(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class051Write(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class051Read(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class051SetInterruptNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS058 - NV3_CONTEXT_SURFACE_0
//
// Method aliases
#define class058Notify              grHWMethod
#define class058SetContextDmaNotifies grHWMethod
#define class058SetContextDmaImage  grHWMethod
#define class058SetColorFormat      grHWMethod
#define class058SetImagePitch       grHWMethod
#define class058SetImageOffset      grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS059 - NV3_CONTEXT_SURFACE_1
//
// Method aliases
#define class059Notify              grHWMethod
#define class059SetContextDmaNotifies grHWMethod
#define class059SetContextDmaImage  grHWMethod
#define class059SetColorFormat      grHWMethod
#define class059SetImagePitch       grHWMethod
#define class059SetImageOffset      grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS05A - NV3_CONTEXT_SURFACE_2
//
// Method aliases
#define class05ANotify              grHWMethod
#define class05ASetContextDmaNotifies grHWMethod
#define class05ASetContextDmaImage  grHWMethod
#define class05ASetColorFormat      grHWMethod
#define class05ASetImagePitch       grHWMethod
#define class05ASetImageOffset      grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS05B - NV3_CONTEXT_SURFACE_3
//
// Method aliases
#define class05BNotify              grHWMethod
#define class05BSetContextDmaNotifies grHWMethod
#define class05BSetContextDmaImage  grHWMethod
#define class05BSetColorFormat      grHWMethod
#define class05BSetImagePitch       grHWMethod
#define class05BSetImageOffset      grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS043 - NV3_CONTEXT_ROP
//
// Method aliases
#define class043Notify              grHWMethod
#define class043SetContextDmaNotifies grHWMethod
#define class043SetRop5             grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS038 - NV4_DVD_SUBPICTURE
//
// Method aliases
#define class038Notify              grHWMethod
#define class038SetContextDmaNotifies grHWMethod
#define class038SetContextDmaOverlay grHWMethod
#define class038SetContextDmaImageIn grHWMethod
#define class038SetContextDmaImageOut grHWMethod
#define class038ImageOutPoint       grHWMethod
#define class038ImageOutSize        grHWMethod
#define class038ImageOutFormat      grHWMethod
#define class038ImageOutOffset      grHWMethod
#define class038ImageInDeltaDuDx    grHWMethod
#define class038ImageInDeltaDvDy    grHWMethod
#define class038ImageInSize         grHWMethod
#define class038ImageInFormat       grHWMethod
#define class038ImageInOffset       grHWMethod
#define class038ImageInPoint        grHWMethod
#define class038OverlayDeltaDuDx    grHWMethod
#define class038OverlayDeltaDvDy    grHWMethod
#define class038OverlaySize         grHWMethod
#define class038OverlayFormat       grHWMethod
#define class038OverlayOffset       grHWMethod
#define class038OverlayPoint        grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS042 - NV4_CONTEXT_SURFACES_2D
//
// Method aliases
#define class042Notify              grHWMethod
#define class042SetContextDmaNotifies grHWMethod
#define class042SetContextDmaSource grHWMethod
#define class042SetContextDmaDestin grHWMethod
#define class042SetColorFormat      grHWMethod
#define class042SetPitch            grHWMethod
#define class042SetOffsetSource     grHWMethod
#define class042SetOffsetDestin     grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS044 - NV4_CONTEXT_PATTERN_METHODS
//
// Method aliases
#define class044Notify              grHWMethod
#define class044SetContextDmaNotifies grHWMethod
#define class044SetColorFormat      grHWMethod
#define class044SetMonochromeFormat grHWMethod
#define class044SetMonochromeShape  grHWMethod
#define class044SetPatternSelect    grHWMethod
#define class044SetMonochromeColor  grHWMethod
#define class044SetMonochromePattern grHWMethod
#define class044SetPatternY8        grHWMethod
#define class044SetPatternR5G6B5    grHWMethod
#define class044SetPatternX8R8G8B8  grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS04A - NV4_GDI_RECTANGLE_TEXT
//
RM_STATUS class04ASetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04ASetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04ASetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04ASetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04ASetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class04ASetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class04ANotify              grHWMethod
#define class04ASetContextDmaNotifies grHWMethod
#define class04ASetContextFonts     grHWMethod
#define class04ASetColorFormat      grHWMethod    
#define class04ASetMonochromeFormat grHWMethod
#define class04AColor1A             grHWMethod
#define class04AUnclippedRectangle  grHWMethod
#define class04AClipB               grHWMethod
#define class04AColor1B             grHWMethod
#define class04AClippedRectangle    grHWMethod
#define class04AClipC               grHWMethod
#define class04AColor1C             grHWMethod
#define class04ASizeC               grHWMethod
#define class04APointC              grHWMethod
#define class04AMonochromeColor1C   grHWMethod
#define class04AClipE               grHWMethod
#define class04AColor0E             grHWMethod
#define class04AColor1E             grHWMethod
#define class04ASizeInE             grHWMethod
#define class04ASizeOutE            grHWMethod
#define class04APointE              grHWMethod
#define class04AMonochromeColor01E  grHWMethod
#define class04AFontF               grHWMethod
#define class04AClipF               grHWMethod
#define class04AColor1F             grHWMethod
#define class04ACharacterColor1F    grHWMethod
#define class04AFontG               grHWMethod
#define class04AClipG               grHWMethod
#define class04AColor1G             grHWMethod
#define class04ACharacterColor1G    grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS052 - NV4_CONTEXT_SURFACE_SWIZZLED
//
// Method aliases
#define class052Notify              grHWMethod
#define class052SetContextDmaNotifies grHWMethod
#define class052SetContextDmaImage  grHWMethod
#define class052SetFormat           grHWMethod
#define class052SetOffset           grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS053 - NV4_CONTEXT_SURFACES_ARGB_ZS
//
RM_STATUS class053SetClipHorizontal(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class053SetClipVertical(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class053Notify              grHWMethod
#define class053SetContextDmaNotifies grHWMethod
#define class053SetContextDmaColor  grHWMethod
#define class053SetContextDmaZeta   grHWMethod
#define class053SetFormat           grHWMethod
#define class053SetClipSize         grHWMethod
#define class053SetPitch            grHWMethod
#define class053SetOffsetColor      grHWMethod
#define class053SetOffsetZeta       grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS054 - NV4_DX5_TEXTURED_TRIANGLE
//
// Method aliases
#define class054Notify              grHWMethod
#define class054SetContextDmaNotifies grHWMethod
#define class054SetContextDmaA      grHWMethod
#define class054SetContextDmaB      grHWMethod
#define class054SetContextSurface   grHWMethod
#define class054ColorKey            grHWMethod
#define class054Offset              grHWMethod
#define class054Format              grHWMethod
#define class054Filter              grHWMethod
#define class054Blend               grHWMethod
#define class054Control             grHWMethod
#define class054FogColor            grHWMethod
#define class054Tlvertex            grHWMethod
#define class054DrawPrimitive       grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS055 - NV4_DX6_MULTI_TEXTURE_TRIANGLE
//
// Method aliases
#define class055Notify              grHWMethod
#define class055SetContextDmaNotifies grHWMethod
#define class055SetContextDmaA      grHWMethod
#define class055SetContextDmaB      grHWMethod
#define class055SetContextSurface   grHWMethod
#define class055Offset              grHWMethod
#define class055Format              grHWMethod
#define class055Filter              grHWMethod
#define class055Combine0Alpha       grHWMethod
#define class055Combine0Color       grHWMethod
#define class055Combine1Alpha       grHWMethod
#define class055Combine1Color       grHWMethod
#define class055CombineFactor       grHWMethod
#define class055Blend               grHWMethod
#define class055Control             grHWMethod
#define class055FogColor            grHWMethod
#define class055Tlmtvertex          grHWMethod
#define class055DrawPrimitive       grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS056 - NV10_CELSIUS_PRIMITIVE
//
RM_STATUS class056GetState(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class056Notify              grHWMethod
#define class056SetWarningEnable    grHWMethod
#define class056WaitForIdle         grHWMethod
#define class056PMTrigger           grHWMethod
#define class056SetContextDmaNotifies grHWMethod
#define class056SetContextDmaA      grHWMethod
#define class056SetContextDmaB      grHWMethod
#define class056SetContextDmaVertex grHWMethod
#define class056SetContextDmaState  grHWMethod
#define class056SetContextDmaColor  grHWMethod
#define class056SetContextDmaZeta   grHWMethod
#define class056MethodCatcher       grHWMethod
#define class056InlineArray         grHWMethod
#define class056DebugInit           grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS057 - NV4_CONTEXT_COLOR_KEY
//
// Method aliases
#define class057Notify              grHWMethod
#define class057SetContextDmaNotifies grHWMethod
#define class057SetColorFormat      grHWMethod
#define class057SetColor            grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS05C - NV4_RENDER_SOLID_LIN
//
RM_STATUS class05CSetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05CSetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05CSetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05CSetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05CSetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05CSetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05CSetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class05CNotify              grHWMethod
#define class05CSetContextDmaNotifies grHWMethod
#define class05CSetColorFormat      grHWMethod
#define class05CColor               grHWMethod
#define class05CLin                 grHWMethod
#define class05CLin32               grHWMethod
#define class05CPolyLin             grHWMethod
#define class05CPolyLin32           grHWMethod
#define class05CColorPolyLin        grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS05D - NV4_RENDER_SOLID_TRIANGLE
//
RM_STATUS class05DSetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05DSetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05DSetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05DSetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05DSetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05DSetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05DSetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class05DNotify              grHWMethod
#define class05DSetContextDmaNotifies grHWMethod
#define class05DSetColorFormat      grHWMethod
#define class05DColor               grHWMethod
#define class05DTriangle            grHWMethod
#define class05DTriangle32          grHWMethod
#define class05DTrimesh             grHWMethod
#define class05DTrimesh32           grHWMethod
#define class05DColorTriangle       grHWMethod
#define class05DColorTrimesh        grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS05E - NV4_RENDER_SOLID_RECTANGLE
//
RM_STATUS class05ESetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05ESetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05ESetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05ESetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05ESetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05ESetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05ESetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class05ENotify              grHWMethod
#define class05ESetContextDmaNotifies grHWMethod
#define class05ESetColorFormat      grHWMethod
#define class05EColor               grHWMethod
#define class05ERectangle           grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS05F - NV4_IMAGE_BLIT
//
RM_STATUS class05FSetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05FSetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05FSetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05FSetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05FSetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05FSetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05FSetContextSurfaces2d(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class05FSetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class05FNotify              grHWMethod
#define class05FSetContextDmaNotifies grHWMethod
#define class05FControlPointIn      grHWMethod
#define class05FControlPointOut     grHWMethod
#define class05FSize                grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS060 - NV4_INDEXED_IMAGE_FROM_CPU
//
RM_STATUS class060SetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class060SetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class060SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class060SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class060SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class060SetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class060SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class060SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class060SetColorConversion(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class060Notify              grHWMethod
#define class060SetContextDmaNotifies grHWMethod
#define class060SetContextDmaLut    grHWMethod
#define class060SetColorFormat      grHWMethod
#define class060IndexFormat         grHWMethod
#define class060Point               grHWMethod
#define class060SizeOut             grHWMethod
#define class060SizeIn              grHWMethod
#define class060LutOffset           grHWMethod
#define class060Indices             grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS061 - NV4_IMAGE_FROM_CPU
//
RM_STATUS class061SetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class061SetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class061SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class061SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class061SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class061SetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class061SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class061SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class061Notify              grHWMethod
#define class061SetContextDmaNotifies grHWMethod
#define class061SetColorFormat      grHWMethod
#define class061Point               grHWMethod
#define class061SizeOut             grHWMethod
#define class061SizeIn              grHWMethod
#define class061Color               grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS062 - NV10_CONTEXT_SURFACES_2D
//
// Method aliases
#define class062Notify              grHWMethod
#define class062SetContextDmaNotifies grHWMethod
#define class062SetContextDmaSource grHWMethod
#define class062SetContextDmaDestin grHWMethod
#define class062SetColorFormat      grHWMethod
#define class062SetPitch            grHWMethod
#define class062SetOffsetSource     grHWMethod
#define class062SetOffsetDestin     grHWMethod

//////////////////////////////////////////////////////////////////////////////
//
// CLASS063 - NV05_SCALED_IMAGE_FROM_MEMORY
//
RM_STATUS class063SetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class063SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class063SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class063SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class063SetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class063SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class063SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class063SetColorConversion(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class063Notify              grHWMethod
#define class063SetContextDmaNotifies grHWMethod
#define class063SetContextDmaImage  grHWMethod
#define class063SetColorFormat      grHWMethod
#define class063Clip                grHWMethod
#define class063RectangleOut        grHWMethod
#define class063DeltaDuDx           grHWMethod
#define class063DeltaDvDy           grHWMethod
#define class063ImageIn             grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS064 - NV05_INDEXED_IMAGE_FROM_CPU
//
RM_STATUS class064SetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class064SetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class064SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class064SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class064SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class064SetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class064SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class064SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class064SetColorConversion(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class064Notify              grHWMethod
#define class064SetContextDmaNotifies grHWMethod
#define class064SetContextDmaLut    grHWMethod
#define class064SetColorFormat      grHWMethod
#define class064IndexFormat         grHWMethod
#define class064Point               grHWMethod
#define class064SizeOut             grHWMethod
#define class064SizeIn              grHWMethod
#define class064LutOffset           grHWMethod
#define class064Indices             grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS065 - NV5_IMAGE_FROM_CPU
//
RM_STATUS class065SetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class065SetContextClip(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class065SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class065SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class065SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class065SetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class065SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class065SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class065SetColorConversion(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class065Notify              grHWMethod
#define class065SetContextDmaNotifies grHWMethod
#define class065SetColorFormat      grHWMethod
#define class065Point               grHWMethod
#define class065SizeOut             grHWMethod
#define class065SizeIn              grHWMethod
#define class065Color               grHWMethod

//////////////////////////////////////////////////////////////////////////////
//
// CLASS066 - NV05_STRETCHED_IMAGE_FROM_CPU
//
RM_STATUS class066SetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class066SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class066SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class066SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class066SetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class066SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class066SetColorConversion(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class066SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class066Notify              grHWMethod
#define class066SetContextDmaNotifies grHWMethod
#define class066SetColorFormat      grHWMethod
#define class066SizeIn              grHWMethod
#define class066DeltaDxDu           grHWMethod
#define class066DeltaDyDv           grHWMethod
#define class066Clip                grHWMethod
#define class066Point12d4           grHWMethod
#define class066Color               grHWMethod

//////////////////////////////////////////////////////////////////////////////
//
// CLASS072 - NV4_CONTEXT_BETA
//
// Method aliases
#define class072Notify              grHWMethod
#define class072SetContextDmaNotifies grHWMethod
#define class072SetBetaFactor       grHWMethod

//////////////////////////////////////////////////////////////////////////////
//
// CLASS076 - NV4_STRETCHED_IMAGE_FROM_CPU
//
RM_STATUS class076SetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class076SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class076SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class076SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class076SetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class076SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class076SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class076Notify              grHWMethod
#define class076SetContextDmaNotifies grHWMethod
#define class076SetColorFormat      grHWMethod
#define class076SizeIn              grHWMethod
#define class076DeltaDxDu           grHWMethod
#define class076DeltaDyDv           grHWMethod
#define class076Clip                grHWMethod
#define class076Point12d4           grHWMethod
#define class076Color               grHWMethod

//////////////////////////////////////////////////////////////////////////////
//
// CLASS077 - NV4_SCALED_IMAGE_FROM_MEMORY
//
RM_STATUS class077SetContextColorKey(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class077SetContextPattern(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class077SetContextRop(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class077SetContextBeta1(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class077SetContextBeta4(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class077SetContextSurface(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class077SetOperation(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class077Notify              grHWMethod
#define class077SetContextDmaNotifies grHWMethod
#define class077SetContextDmaImage  grHWMethod
#define class077SetColorFormat      grHWMethod
#define class077Clip                grHWMethod
#define class077RectangleOut        grHWMethod
#define class077DeltaDuDx           grHWMethod
#define class077DeltaDvDy           grHWMethod
#define class077ImageIn             grHWMethod

//////////////////////////////////////////////////////////////////////////////
//
// CLASS07A - NV10_VIDEO_OVERLAY
//
RM_STATUS class07ACreate(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
RM_STATUS class07ADestroy(PHWINFO, POBJECT);
RM_STATUS class07ASetNotify(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07AStopOverlay(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07ASetNotifyCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07ASetOverlayCtxDma(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07AGetOffset(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07ASetOverlayValues(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07ASetOverlayPointOutNow(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07ASetOverlayLuminanceNow(PHWINFO, POBJECT, PMETHOD, U032, U032);
RM_STATUS class07ASetOverlayChrominanceNow(PHWINFO, POBJECT, PMETHOD, U032, U032);

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS07B - NV10_TEXTURE_FROM_CPU
//
// Method aliases
#define class07BNotify                grHWMethod
#define class07BSetContextDmaNotifies grHWMethod
#define class07BSetContextSurface     grHWMethod
#define class07BSetColorFormat        grHWMethod
#define class07BPoint                 grHWMethod
#define class07BSize                  grHWMethod
#define class07BClipHorizontal        grHWMethod
#define class07BClipVertical          grHWMethod
#define class07BColor                 grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS088 - NV10_DVD_SUBPICTURE
//
RM_STATUS grHWMethod(PHWINFO, POBJECT, PMETHOD, U032, U032);
// Method aliases
#define class088Notify              grHWMethod
#define class088SetContextDmaNotifies grHWMethod
#define class088SetContextDmaOverlay grHWMethod
#define class088SetContextDmaImageIn grHWMethod
#define class088SetContextDmaImageOut grHWMethod
#define class088ImageOutPoint       grHWMethod
#define class088ImageOutSize        grHWMethod
#define class088ImageOutFormat      grHWMethod
#define class088ImageOutOffset      grHWMethod
#define class088ImageInDeltaDuDx    grHWMethod
#define class088ImageInDeltaDvDy    grHWMethod
#define class088ImageInSize         grHWMethod
#define class088ImageInFormat       grHWMethod
#define class088ImageInOffset       grHWMethod
#define class088ImageInPoint        grHWMethod
#define class088OverlayDeltaDuDx    grHWMethod
#define class088OverlayDeltaDvDy    grHWMethod
#define class088OverlaySize         grHWMethod
#define class088OverlayFormat       grHWMethod
#define class088OverlayOffset       grHWMethod
#define class088OverlayPoint        grHWMethod

//////////////////////////////////////////////////////////////////////////////
//
// CLASS089 - NV10_SCALED_IMAGE_FROM_MEMORY
//
// Method aliases
#define class089Notify              grHWMethod
#define class089SetContextDmaNotifies grHWMethod
#define class089SetContextDmaImage  grHWMethod
#define class089SetContextPattern   grHWMethod
#define class089SetContextRop       grHWMethod
#define class089SetContextBeta1     grHWMethod
#define class089SetContextBeta4     grHWMethod
#define class089SetContextSurface   grHWMethod
#define class089SetColorConversion  grHWMethod
#define class089SetColorFormat      grHWMethod
#define class089SetOperation        grHWMethod
#define class089Clip                grHWMethod
#define class089RectangleOut        grHWMethod
#define class089DeltaDuDx           grHWMethod
#define class089DeltaDvDy           grHWMethod
#define class089ImageIn             grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS08A - NV10_IMAGE_FROM_CPU
//

// Method aliases
#define class08ANotify              grHWMethod
#define class08ASetContextDmaNotifies grHWMethod
#define class08ASetContextColorKey  grHWMethod
#define class08ASetContextClip      grHWMethod
#define class08ASetContextPattern   grHWMethod
#define class08ASetContextRop       grHWMethod
#define class08ASetContextBeta1     grHWMethod
#define class08ASetContextBeta4     grHWMethod
#define class08ASetContextSurface   grHWMethod
#define class08ASetColorConversion  grHWMethod
#define class08ASetOperation        grHWMethod
#define class08ASetColorFormat      grHWMethod
#define class08ASetColorFormat      grHWMethod
#define class08APoint               grHWMethod
#define class08ASizeOut             grHWMethod
#define class08ASizeIn              grHWMethod
#define class08AColor               grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS093 - NV10_CONTEXT_SURFACES_3D
//
// Method aliases
#define class093Notify              grHWMethod
#define class093SetContextDmaNotifies grHWMethod
#define class093SetContextDmaColor  grHWMethod
#define class093SetContextDmaZeta   grHWMethod
#define class093SetFormat           grHWMethod
#define class093SetClipSize         grHWMethod
#define class093SetPitch            grHWMethod
#define class093SetClipHorizontal   grHWMethod
#define class093SetClipVertical     grHWMethod
#define class093SetOffsetColor      grHWMethod
#define class093SetOffsetZeta       grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS094 - NV10_DX5_TEXTURED_TRIANGLE
//
// Method aliases
#define class094Notify              grHWMethod
#define class094SetContextDmaNotifies grHWMethod
#define class094SetContextDmaA      grHWMethod
#define class094SetContextDmaB      grHWMethod
#define class094SetContextSurface   grHWMethod
#define class094ColorKey            grHWMethod
#define class094Offset              grHWMethod
#define class094Format              grHWMethod
#define class094Filter              grHWMethod
#define class094Blend               grHWMethod
#define class094Control             grHWMethod
#define class094FogColor            grHWMethod
#define class094Tlvertex            grHWMethod
#define class094DrawPrimitive       grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS095 - NV10_DX6_MULTI_TEXTURE_TRIANGLE
//
// Method aliases
#define class095Notify              grHWMethod
#define class095SetContextDmaNotifies grHWMethod
#define class095SetContextDmaA      grHWMethod
#define class095SetContextDmaB      grHWMethod
#define class095SetContextSurface   grHWMethod
#define class095Offset              grHWMethod
#define class095Format              grHWMethod
#define class095Filter              grHWMethod
#define class095Combine0Alpha       grHWMethod
#define class095Combine0Color       grHWMethod
#define class095Combine1Alpha       grHWMethod
#define class095Combine1Color       grHWMethod
#define class095CombineFactor       grHWMethod
#define class095Blend               grHWMethod
#define class095Control             grHWMethod
#define class095FogColor            grHWMethod
#define class095Tlmtvertex          grHWMethod
#define class095DrawPrimitive       grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS096 - NV15_CELSIUS_PRIMITIVE / NV11_CELSIUS_PRIMITIVE
//
RM_STATUS class096GetState(PHWINFO, POBJECT, PMETHOD, U032, U032);

// Method aliases
#define class096Notify                grHWMethod
#define class096SetWarningEnable      grHWMethod
#define class096WaitForIdle           grHWMethod
#define class096SyncSetRead           grHWMethod
#define class096SyncSetWrite          grHWMethod
#define class096SyncSetModulo         grHWMethod
#define class096SyncIncrementWrite    grHWMethod
#define class096SyncStall             grHWMethod
#define class096PMTrigger             grHWMethod
#define class096SetContextDmaNotifies grHWMethod
#define class096SetContextDmaA        grHWMethod
#define class096SetContextDmaB        grHWMethod
#define class096SetContextDmaVertex   grHWMethod
#define class096SetContextDmaState    grHWMethod
#define class096SetContextDmaColor    grHWMethod
#define class096SetContextDmaZeta     grHWMethod
#define class096MethodCatcher         grHWMethod
#define class096InlineArray           grHWMethod
#define class096DebugInit             grHWMethod

//////////////////////////////////////////////////////////////////////////////
// 
// CLASS09F - NV15_IMAGE_BLIT
//
// Method aliases
#define class09FNotify                  grHWMethod
#define class09FSyncSetRead             grHWMethod
#define class09FSyncSetWrite            grHWMethod
#define class09FSyncSetModulo           grHWMethod
#define class09FSyncIncrementWrite      grHWMethod
#define class09FSyncStall               grHWMethod
#define class09FSyncIncrementReadRange  grHWMethod
#define class09FSetContextDmaNotifies   grHWMethod
#define class09FSetContextColorKey      grHWMethod
#define class09FSetContextClip          grHWMethod
#define class09FSetContextPattern       grHWMethod
#define class09FSetContextRop           grHWMethod
#define class09FSetContextBeta1         grHWMethod
#define class09FSetContextBeta4         grHWMethod
#define class09FSetContextSurfaces2d    grHWMethod
#define class09FSetOperation            grHWMethod
#define class09FControlPointIn          grHWMethod
#define class09FControlPointOut         grHWMethod
#define class09FSize                    grHWMethod

#endif // _MODULAR_H_
