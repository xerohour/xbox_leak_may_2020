 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

#ifndef __NVPROCMAN_H_
#define __NVPROCMAN_H_

#include "CompileControl.h"

#ifndef NV3
#if IS_WINNT5
#include <dx95type.h>
#include "nvntd3d.h"
#endif

#ifndef NVQTWKVER // include DDKMMINI.H only if control panel not being built
#if !IS_WINNT4
#include "ddkmmini.h"
#endif
#endif // ifndef NVQTWKVER
#endif // NV3

#define EVENTNAME_IDLE                  "NV_Idle"
#define EVENTNAME_OVL_FLIP_EVEN         "NV_OvlFlipEven"
#define EVENTNAME_OVL_FLIP_ODD          "NV_OvlFlipOdd"
#define EVENTNAME_DMA_TO_VID            "NV_DMAToVid"
#define EVENTNAME_CONVERT               "NV_Convert"
#define EVENTNAME_SUBPICTURE            "NV_SubPicture"
#define EVENTNAME_PRESCALE              "NV_PreScale"
#define EVENTNAME_COLOURCONTROL         "NV_ColourControl"
#define EVENTNAME_TFILTER               "NV_TFilter"
#define EVENTNAME_DFILTER               "NV_DFilter"
#define EVENTNAME_DISPATCH_GO           "NV_DispatchGo"
#define EVENTNAME_DISPATCH_DONE         "NV_DispatchDone"
#define EVENTNAME_FSMIRROR              "NV_FSMirror"
#define EVENTNAME_FOURCCBLT             "NV_FourCCBlt"
#define EVENTNAME_PRIMARY0_FLIP_EVEN    "NV_Primary0FlipEven"
#define EVENTNAME_PRIMARY0_FLIP_ODD     "NV_Primary0FlipOdd"
#define EVENTNAME_PRIMARY1_FLIP_EVEN    "NV_Primary1FlipEven"
#define EVENTNAME_PRIMARY1_FLIP_ODD     "NV_Primary1FlipOdd"
#define EVENTNAME_TIMER                 "NV_Timer"
#define EVENTNAME_DXEXCLUSIVEMODE       "NV_DXExclusiveMode"

#if IS_WINNT5

#define CREATE_EVENT(pHandle) EngCreateEvent(pHandle)
#define DELETE_EVENT(handle) EngDeleteEvent(handle)
#define HDRVEVENT PEVENT
#define EVENT_ERROR

#elif IS_WINNT4

// TBD - mlavoie: resolve issues with events on NT4

// VERY unsure as to whether the ddraw portion of the display can call these.
//   the miniport can so we may need to find another way -mlavoie
// Also very unsure as to whether these 2 funcs correspond to create & delete
#define CREATE_EVENT(pHandle) KeInitializeEvent(pHandle,NotificationEvent,FALSE)
#define DELETE_EVENT(handle) KeResetEvent(&(handle))
//KeWaitForSingleObject
#define HDRVEVENT NvU32
#define EVENT_ERROR

#elif IS_WIN9X

#define HDRVEVENT HANDLE
#define EVENT_ERROR {   \
    pmDeleteProcess(lpProcInfo->dwProcessID);   \
    pmAddProcess(pDriverData, GetCurrentProcessId());        }

#else
#error Unhandled OS.
#endif

// process manager structure
typedef struct tagProcessInfo {
    unsigned long dwProcessID;
    HDRVEVENT     hIdleSyncEvent0;
    HDRVEVENT     hIdleSyncEvent3;
    HDRVEVENT     hOvlFlipEvenEvent0;
    HDRVEVENT     hOvlFlipEvenEvent3;
    HDRVEVENT     hOvlFlipOddEvent0;
    HDRVEVENT     hOvlFlipOddEvent3;
    HDRVEVENT     hDMABlitToVid0;
    HDRVEVENT     hDMABlitToVid3;
    HDRVEVENT     hConvert0;
    HDRVEVENT     hConvert3;
    HDRVEVENT     hSubPicture0;
    HDRVEVENT     hSubPicture3;
    HDRVEVENT     hColourControl0;
    HDRVEVENT     hColourControl3;
    HDRVEVENT     hTFilter0;
    HDRVEVENT     hTFilter3;
    HDRVEVENT     hDFilter0;
    HDRVEVENT     hDFilter3;
    HDRVEVENT     hFSMirror0;
    HDRVEVENT     hFSMirror3;
    HDRVEVENT     hFourCCBlt0;
    HDRVEVENT     hFourCCBlt3;
    HDRVEVENT     hPrimary0FlipEven0;
    HDRVEVENT     hPrimary0FlipEven3;
    HDRVEVENT     hPrimary0FlipOdd0;
    HDRVEVENT     hPrimary0FlipOdd3;
    HDRVEVENT     hPrimary1FlipEven0;
    HDRVEVENT     hPrimary1FlipEven3;
    HDRVEVENT     hPrimary1FlipOdd0;
    HDRVEVENT     hPrimary1FlipOdd3;
    HDRVEVENT     hTimer0;
    HDRVEVENT     hTimer3;
    struct tagProcessInfo *lpNext;
} PROCESSINFO, *LPPROCESSINFO;

#ifdef __cplusplus
extern "C" {
#endif

// public
#ifndef WINNT
HANDLE        ConvertRing3EventToRing0 (HDRVEVENT hEventRing3);
BOOL          CloseRing0Handle         (HDRVEVENT hEventRing0);
#endif // WINNT
typedef struct _GLOBALDATA GLOBALDATA;
LPPROCESSINFO pmGetProcess             (GLOBALDATA *pDriverData, DWORD processID);
BOOL          pmAddProcess             (GLOBALDATA *pDriverData, DWORD processID);
BOOL          pmDeleteProcess          (DWORD processID);
void          pmDeleteAllProcesses     (GLOBALDATA *pDriverData);

HRESULT NvResetEvent(HDRVEVENT hEvent);
HRESULT NvSetEvent(HDRVEVENT hEvent);
HRESULT NvWaitEvent(HDRVEVENT hEvent, DWORD timeout);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __NVPROCMAN_H_
