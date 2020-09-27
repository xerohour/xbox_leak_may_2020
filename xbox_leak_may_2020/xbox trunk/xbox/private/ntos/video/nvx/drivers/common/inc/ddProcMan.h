/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: ddProcMan.h                                                       *
*    Definition file for:                                                   *
*       ddpm.c                                                              *
*    Suggested new name:                                                    *
*       ddProcMan.c                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Paul van der Kouwe 06/16/99 - created.                              *
*                                                                           *
\***************************************************************************/

#ifndef __DDPROCMAN_H_
#define __DDPROCMAN_H_

#include "CompileControl.h"

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
#define EVENTNAME_PRIMARY0_FLIP_EVEN    "NV_Primary0FlipEven"
#define EVENTNAME_PRIMARY0_FLIP_ODD     "NV_Primary0FlipOdd"
#define EVENTNAME_PRIMARY1_FLIP_EVEN    "NV_Primary1FlipEven"
#define EVENTNAME_PRIMARY1_FLIP_ODD     "NV_Primary1FlipOdd"
#define EVENTNAME_TIMER                 "NV_Timer"

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
BOOL          pmDeleteProcessPDD       (GLOBALDATA *pDriverData, DWORD processID);
void          pmDeleteAllProcesses     (GLOBALDATA *pDriverData);

HRESULT NvResetEvent(HDRVEVENT hEvent);
HRESULT NvSetEvent(HDRVEVENT hEvent);
HRESULT NvWaitEvent(HDRVEVENT hEvent, DWORD timeout);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __DDPROCMAN_H_
