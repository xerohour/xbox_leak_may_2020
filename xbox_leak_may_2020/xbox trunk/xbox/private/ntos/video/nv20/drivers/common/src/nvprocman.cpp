 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2001 NVIDIA, Corporation.  All rights reserved.      *|
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
#include "nvprecomp.h"
#pragma hdrstop

// Local function
BOOL pmDeleteProcessPDD       (GLOBALDATA *pDriverData, DWORD processID);

// NB: This file is broken into 3 parts.  One for Win9x, Win2k, and WinNT4.
//     All functions are implemented in each section.

#if IS_WIN9X
// Event Services (Set,Reset,Wait)
//
HRESULT NvResetEvent(HDRVEVENT hEvent)
{
    return ResetEvent(hEvent);
}

HRESULT NvSetEvent(HDRVEVENT hEvent)
{
    return SetEvent(hEvent);
}

HRESULT NvWaitEvent(HDRVEVENT hEvent, DWORD timeout)
{
    return WaitForSingleObject(hEvent,timeout);
}

// ------------------------------------------------------------------------------------------
// ConvertRing3EventToRing0
//      Converts a ring3 event handle to a ring0 event handle that RM can understand
HANDLE ConvertRing3EventToRing0 ( HANDLE hEventRing3 )
{
    HANDLE  hEventRing0;

#ifdef IKOS
    return (NULL);
#endif

    if ( hEventRing3 == NULL || global.pfOpenVxDHandle == NULL)
        return (NULL);

    /*
     * Create a ring0 event
     */
    hEventRing0 = (*global.pfOpenVxDHandle)(hEventRing3);

    return(hEventRing0);
}

// ------------------------------------------------------------------------------------------
// CloseRing0Handle
//      Closes the ring0 event handle
BOOL CloseRing0Handle( HANDLE hEventRing0 )
{
    HINSTANCE hKernel32Dll;
    HANDLE  (WINAPI *pfCloseVxDHandle)(HANDLE);

    if (hEventRing0 == NULL) return (FALSE);

    hKernel32Dll = LoadLibrary("kernel32.dll");
    if (!hKernel32Dll)
        return (FALSE);

    pfCloseVxDHandle = (HANDLE (WINAPI *)(HANDLE))GetProcAddress(hKernel32Dll, "CloseVxDHandle");
    if (!pfCloseVxDHandle) {
        FreeLibrary(hKernel32Dll);
        return (FALSE);
    }

    (*pfCloseVxDHandle)(hEventRing0);
    FreeLibrary(hKernel32Dll);

    return (TRUE);
}


// -------------------------------------------------------------------------
// pmGetProcess
//      Returns a pointer to the PROCESSINFO structure of the associated
//      process ID.  Returns NULL if not found.
LPPROCESSINFO pmGetProcess(GLOBALDATA *pDriverData, DWORD processID)
{
LPPROCESSINFO lpProcInfo;
static DWORD state=0;
DWORD mask;

    switch (pDXShare->dwHeadNumber) {
    case 1:  mask = 0x00000002; break;
    case 0:
    default: mask = 0x00000001; break;
    }

    // to prevent infinite recursion
    if ((pDXShare->dwHeadNumber == 0 && (state & 0x00000001)) ||
        (pDXShare->dwHeadNumber == 1 && (state & 0x00000002))) {
        return NULL;
    }

    for (lpProcInfo=pDriverData->lpProcessInfoHead; lpProcInfo!=NULL; lpProcInfo=lpProcInfo->lpNext) {
        if (lpProcInfo->dwProcessID == processID)
            return lpProcInfo;
    }

    state |= mask;
    pmAddProcess(pDriverData, processID);
    state &= ~mask;
    return pmGetProcess(pDriverData, processID);
}

// -------------------------------------------------------------------------
// pmAddProcess
//      Adds a process to the linked list, creates events and attaches them
//      to their respective objects.  Returns TRUE if succeeded.
BOOL pmAddProcess(GLOBALDATA *pDriverData, DWORD processID)
{
LPPROCESSINFO lpProcInfo;
SECURITY_ATTRIBUTES sec = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

    if (pmGetProcess(pDriverData, processID) != NULL)
        return FALSE;

    lpProcInfo = (LPPROCESSINFO) AllocIPM(sizeof(PROCESSINFO));
    if (lpProcInfo == NULL)
        return FALSE;

    lpProcInfo->lpNext          = pDriverData->lpProcessInfoHead;
    lpProcInfo->dwProcessID     = processID;

    // under Win9x, events are created once under the parent process
    // all subsequent processes open the same user event

    if (pDriverData->lpProcessInfoHead == 0) {
        // first one, create and attach events
        // Idle event
        lpProcInfo->hIdleSyncEvent3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_IDLE);
        lpProcInfo->hIdleSyncEvent0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hIdleSyncEvent3);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_DD_ROP_GDI_RECT_AND_TEXT,
                       NV_DD_EVENT_IDLE_SYNC, NV01_EVENT_WIN32_EVENT,
                       NV04A_NOTIFIERS_NOTIFY, (void*) lpProcInfo->hIdleSyncEvent0);
        NvSetEvent(lpProcInfo->hIdleSyncEvent3);

        // Timer event
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0004_TIMER) {
            lpProcInfo->hTimer3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_TIMER);
            lpProcInfo->hTimer0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hTimer3);
            NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_TIMER_IID,
                           NV_DD_EVENT_TIMER, NV01_EVENT_WIN32_EVENT,
                           NV004_NOTIFIERS_SET_ALARM_NOTIFY, (void*) lpProcInfo->hTimer0);
            NvSetEvent(lpProcInfo->hTimer3);
        }

        // Overlay flip events
        lpProcInfo->hOvlFlipEvenEvent3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_OVL_FLIP_EVEN);
        lpProcInfo->hOvlFlipEvenEvent0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hOvlFlipEvenEvent3);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_OVERLAY_IID,
                       NV_DD_EVENT_OVL_FLIP1, NV01_EVENT_WIN32_EVENT,
                       NV07A_NOTIFIERS_SET_OVERLAY(0), (void*) lpProcInfo->hOvlFlipEvenEvent0);
        NvSetEvent(lpProcInfo->hOvlFlipEvenEvent3);

        lpProcInfo->hOvlFlipOddEvent3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_OVL_FLIP_ODD);
        lpProcInfo->hOvlFlipOddEvent0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hOvlFlipOddEvent3);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_OVERLAY_IID,
                       NV_DD_EVENT_OVL_FLIP2, NV01_EVENT_WIN32_EVENT,
                       NV07A_NOTIFIERS_SET_OVERLAY(1), (void*) lpProcInfo->hOvlFlipOddEvent0);
        NvSetEvent(lpProcInfo->hOvlFlipOddEvent3);

        // DMA to Video memory event
        lpProcInfo->hDMABlitToVid3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_DMA_TO_VID);
        lpProcInfo->hDMABlitToVid0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hDMABlitToVid3);
        NvRmAllocEvent(pDriverData->dwRootHandle, pDriverData->vpp.m_obj_a[DMABLT_TO_VID_NDX].classIID,
                       NV_DD_EVENT_DMABLT_TO_VID, NV01_EVENT_WIN32_EVENT,
                       NV039_NOTIFIERS_BUFFER_NOTIFY, (void*) lpProcInfo->hDMABlitToVid0);
        NvSetEvent(lpProcInfo->hDMABlitToVid3);

        // Format conversion to YUV422 event
        lpProcInfo->hConvert3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_CONVERT);
        lpProcInfo->hConvert0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hConvert3);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_V2V_FORMAT_Y_IID,
                       NV_DD_EVENT_CONVERT_SYNC, NV01_EVENT_WIN32_EVENT,
                       NV039_NOTIFIERS_BUFFER_NOTIFY, (void*) lpProcInfo->hConvert0);
        NvSetEvent(lpProcInfo->hConvert3);

        // DVD subpicture
        lpProcInfo->hSubPicture3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_SUBPICTURE);
        lpProcInfo->hSubPicture0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hSubPicture3);
        NvRmAllocEvent(pDriverData->dwRootHandle, pDriverData->vpp.m_obj_a[DVDPICT_NDX].classIID,
                       NV_DD_EVENT_DVD_SUBPICTURE, NV01_EVENT_WIN32_EVENT,
                       NV038_NOTIFIERS_NOTIFY, (void*) lpProcInfo->hSubPicture0);
        NvSetEvent(lpProcInfo->hSubPicture3);

        // Colour control events
        lpProcInfo->hColourControl3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_COLOURCONTROL);
        lpProcInfo->hColourControl0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hColourControl3);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_V2OSH_FORMAT_IID,
                       NV_DD_EVENT_VIDEO_TO_SYSTEM_SYNC, NV01_EVENT_WIN32_EVENT,
                       NV039_NOTIFIERS_BUFFER_NOTIFY, (void*) lpProcInfo->hColourControl0);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_OSH2V_FORMAT_IID,
                       NV_DD_EVENT_SYSTEM_TO_VIDEO_SYNC, NV01_EVENT_WIN32_EVENT,
                       NV039_NOTIFIERS_BUFFER_NOTIFY, (void*) lpProcInfo->hColourControl0);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_ALPHA_BLIT_IID,
                       NV_DD_EVENT_ALPHA_BLIT_SYNC, NV01_EVENT_WIN32_EVENT,
                       NV05F_NOTIFIERS_NOTIFY, (void*) lpProcInfo->hColourControl0);
        NvSetEvent(lpProcInfo->hColourControl3);

        // Temporal filter event
        lpProcInfo->hTFilter3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_TFILTER);
        lpProcInfo->hTFilter0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hTFilter3);
        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_20) {
            NvRmAllocEvent(pDriverData->dwRootHandle, D3D_KELVIN_PRIMITIVE,
                           NV_DD_EVENT_COMBINER_SYNC, NV01_EVENT_WIN32_EVENT,
                           NV097_NOTIFIERS_NOTIFY, (void*) lpProcInfo->hTFilter0);
        } else if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
            NvRmAllocEvent(pDriverData->dwRootHandle, D3D_CELSIUS_PRIMITIVE,
                           NV_DD_EVENT_COMBINER_SYNC, NV01_EVENT_WIN32_EVENT,
                           NV056_NOTIFIERS_NOTIFY, (void*) lpProcInfo->hTFilter0);
        } else {
            NvRmAllocEvent(pDriverData->dwRootHandle, D3D_DX6_MULTI_TEXTURE_TRIANGLE,
                           NV_DD_EVENT_COMBINER_SYNC, NV01_EVENT_WIN32_EVENT,
                           NV055_NOTIFIERS_NOTIFY, (void*) lpProcInfo->hTFilter0);
        }
        NvSetEvent(lpProcInfo->hTFilter3);

        // Skip this on NV4/NV5
        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10)
        {
            // Deinterlace filter event
            lpProcInfo->hDFilter3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_DFILTER);
            lpProcInfo->hDFilter0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hDFilter3);
            if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_20) {
                NvRmAllocEvent(pDriverData->dwRootHandle, D3D_KELVIN_PRIMITIVE,
                               NV_DD_EVENT_DFILTER_SYNC, NV01_EVENT_WIN32_EVENT,
                               NV097_NOTIFIERS_NOTIFY, (void*) lpProcInfo->hDFilter0);
            } else if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                NvRmAllocEvent(pDriverData->dwRootHandle, D3D_CELSIUS_PRIMITIVE,
                               NV_DD_EVENT_DFILTER_SYNC, NV01_EVENT_WIN32_EVENT,
                               NV056_NOTIFIERS_NOTIFY, (void*) lpProcInfo->hDFilter0);
            }
            NvSetEvent(lpProcInfo->hDFilter3);
        }

        // Full screen mirror event
        lpProcInfo->hFSMirror3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_FSMIRROR);
        lpProcInfo->hFSMirror0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hFSMirror3);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_SCALED_IMAGE1_IID,
                       NV_DD_EVENT_FSMIRROR, NV01_EVENT_WIN32_EVENT,
                       NV089_NOTIFIERS_NOTIFY, (void*) lpProcInfo->hFSMirror0);
        NvSetEvent(lpProcInfo->hFSMirror3);

        // 4CC blt event
        lpProcInfo->hFourCCBlt3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_FOURCCBLT);
        lpProcInfo->hFourCCBlt0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hFourCCBlt3);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_SCALED_IMAGE2_IID,
                       NV_DD_EVENT_FOURCCBLT, NV01_EVENT_WIN32_EVENT,
                       NV089_NOTIFIERS_NOTIFY, (void*) lpProcInfo->hFourCCBlt0);
        NvSetEvent(lpProcInfo->hFourCCBlt0);

        // Primary flip events
        lpProcInfo->hPrimary0FlipEven3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_PRIMARY0_FLIP_EVEN);
        lpProcInfo->hPrimary0FlipEven0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hPrimary0FlipEven3);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_DD_VIDEO_LUT_CURSOR_DAC,
                       NV_DD_EVENT_PRIMARY0_FLIP1, NV01_EVENT_WIN32_EVENT,
                       NV067_NOTIFIERS_SET_IMAGE(0), (void*) lpProcInfo->hPrimary0FlipEven0);
        NvSetEvent(lpProcInfo->hPrimary0FlipEven3);

        lpProcInfo->hPrimary0FlipOdd3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_PRIMARY0_FLIP_ODD);
        lpProcInfo->hPrimary0FlipOdd0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hPrimary0FlipOdd3);
        NvRmAllocEvent(pDriverData->dwRootHandle, NV_DD_VIDEO_LUT_CURSOR_DAC,
                       NV_DD_EVENT_PRIMARY0_FLIP2, NV01_EVENT_WIN32_EVENT,
                       NV067_NOTIFIERS_SET_IMAGE(1), (void*) lpProcInfo->hPrimary0FlipOdd0);
        NvSetEvent(lpProcInfo->hPrimary0FlipOdd3);

        if (pDriverData->dwHeads > 1)
        {
            lpProcInfo->hPrimary1FlipEven3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_PRIMARY1_FLIP_EVEN);
            lpProcInfo->hPrimary1FlipEven0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hPrimary1FlipEven3);
            NvRmAllocEvent(pDriverData->dwRootHandle, NV_DD_VIDEO_LUT_CURSOR_DAC + 1,
                           NV_DD_EVENT_PRIMARY1_FLIP1, NV01_EVENT_WIN32_EVENT,
                           NV067_NOTIFIERS_SET_IMAGE(0), (void*) lpProcInfo->hPrimary1FlipEven0);
            NvSetEvent(lpProcInfo->hPrimary1FlipEven3);

            lpProcInfo->hPrimary1FlipOdd3 = CreateEvent(&sec, TRUE, TRUE, EVENTNAME_PRIMARY1_FLIP_ODD);
            lpProcInfo->hPrimary1FlipOdd0 = ConvertRing3EventToRing0((HANDLE)lpProcInfo->hPrimary1FlipOdd3);
            NvRmAllocEvent(pDriverData->dwRootHandle, NV_DD_VIDEO_LUT_CURSOR_DAC + 1,
                           NV_DD_EVENT_PRIMARY1_FLIP2, NV01_EVENT_WIN32_EVENT,
                           NV067_NOTIFIERS_SET_IMAGE(1), (void*) lpProcInfo->hPrimary1FlipOdd0);
            NvSetEvent(lpProcInfo->hPrimary1FlipOdd3);
        }

    } else {
        lpProcInfo->hIdleSyncEvent3      = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_IDLE);
        lpProcInfo->hIdleSyncEvent0      = 0;
        lpProcInfo->hTimer3              = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_TIMER);
        lpProcInfo->hTimer0              = 0;
        lpProcInfo->hOvlFlipEvenEvent3   = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_OVL_FLIP_EVEN);
        lpProcInfo->hOvlFlipEvenEvent0   = 0;
        lpProcInfo->hOvlFlipOddEvent3    = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_OVL_FLIP_ODD);
        lpProcInfo->hOvlFlipOddEvent0    = 0;
        lpProcInfo->hDMABlitToVid3       = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_DMA_TO_VID);
        lpProcInfo->hDMABlitToVid0       = 0;
        lpProcInfo->hConvert3            = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_CONVERT);
        lpProcInfo->hConvert0            = 0;
        lpProcInfo->hSubPicture3         = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_SUBPICTURE);
        lpProcInfo->hSubPicture0         = 0;
        lpProcInfo->hColourControl3      = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_COLOURCONTROL);
        lpProcInfo->hColourControl0      = 0;
        lpProcInfo->hTFilter3            = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_TFILTER);
        lpProcInfo->hTFilter0            = 0;
        // Skip this on NV4/NV5
        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10)
        {
            lpProcInfo->hDFilter3            = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_DFILTER);
            lpProcInfo->hDFilter0            = 0;
        }
        lpProcInfo->hFSMirror3           = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_FSMIRROR);
        lpProcInfo->hFSMirror0           = 0;
        lpProcInfo->hFourCCBlt0          = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_FOURCCBLT);
        lpProcInfo->hFourCCBlt3          = 0;
        lpProcInfo->hPrimary0FlipEven3   = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_PRIMARY0_FLIP_EVEN);
        lpProcInfo->hPrimary0FlipEven0   = 0;
        lpProcInfo->hPrimary0FlipOdd3    = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_PRIMARY0_FLIP_ODD);
        lpProcInfo->hPrimary0FlipOdd0    = 0;
        if (pDriverData->dwHeads > 1)
        {
            lpProcInfo->hPrimary1FlipEven3   = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_PRIMARY1_FLIP_EVEN);
            lpProcInfo->hPrimary1FlipEven0   = 0;
            lpProcInfo->hPrimary1FlipOdd3    = OpenEvent(EVENT_ALL_ACCESS, TRUE, EVENTNAME_PRIMARY1_FLIP_ODD);
            lpProcInfo->hPrimary1FlipOdd0    = 0;
        }
    }

    pDriverData->lpProcessInfoHead = lpProcInfo;
    return TRUE;
}

// -------------------------------------------------------------------------
// pmDeleteProcess
//      Deletes a process from the info list.  Destroy all associated
//      events.  Returns TRUE if succeeded
BOOL pmDeleteProcess(DWORD processID)
{
    return pmDeleteProcessPDD(pDriverData, processID);
}

BOOL pmDeleteProcessPDD(GLOBALDATA *pDriverData, DWORD processID)
{
    LPPROCESSINFO lpProcInfo, lpPrevProcInfo;

    for (lpProcInfo=pDriverData->lpProcessInfoHead; lpProcInfo!=NULL; lpProcInfo=lpProcInfo->lpNext) {
        if (lpProcInfo->dwProcessID == processID) {
            if (lpProcInfo == pDriverData->lpProcessInfoHead) {
                pDriverData->lpProcessInfoHead = lpProcInfo->lpNext;
            } else {
                lpPrevProcInfo->lpNext = lpProcInfo->lpNext;
            }

            /*  Rely on process termination to kill these, closing them seems to screw up Windows for unknown reasons
            if (lpProcInfo->hIdleSyncEvent3)
                WAITFORSINGLEOBJECT((HANDLE)lpProcInfo->hIdleSyncEvent3, 100);
            if (lpProcInfo->hIdleSyncEvent0) {
                CloseRing0Handle((HANDLE)lpProcInfo->hIdleSyncEvent0);
                lpProcInfo->hIdleSyncEvent0 = 0;
            }
            if (lpProcInfo->hIdleSyncEvent3) {
                CLOSE_HANDLE((HANDLE)lpProcInfo->hIdleSyncEvent3);
                lpProcInfo->hIdleSyncEvent3 = 0;
            }
            */

            FreeIPM(lpProcInfo);
            return TRUE;
        } else {
            lpPrevProcInfo = lpProcInfo;
        }
    }
    return FALSE;
}

// -------------------------------------------------------------------------
// pmDeleteAllProcesses
//      Deletes all processes
void pmDeleteAllProcesses(GLOBALDATA *pDriverData)
{
    LPPROCESSINFO lpProcInfo;

    // tells the ring0 code to not run any more!
    pDriverData->vpp.regRing0ColourCtlInterlockFlags = pDriverData->vpp.regRing0ColourCtlInterlockFlags & 0xFFFFFFFE;
    {
        DWORD aCount = 16;
        while((pDriverData->vpp.regRing0ColourCtlInterlockFlags & 0x02) && aCount-- ) {
            Sleep(1);  // waits 1 millisecond
        }
    }

    while (pDriverData->lpProcessInfoHead != 0) {
        lpProcInfo = pDriverData->lpProcessInfoHead;
        pmDeleteProcessPDD(pDriverData, lpProcInfo->dwProcessID);
    }
}

#elif IS_WINNT5

// Event Services (Set,Reset,Wait)
//
HRESULT NvResetEvent(HDRVEVENT hEvent)
{
    LARGE_INTEGER timeOut;
    timeOut.LowPart = -2000000;
    timeOut.HighPart = -1;
    return EngWaitForSingleObject(hEvent, &timeOut);
}

HRESULT NvSetEvent(HDRVEVENT hEvent)
{
    return EngSetEvent(hEvent);
}

HRESULT NvWaitEvent(HDRVEVENT hEvent, DWORD timeout)
{
    HRESULT ddrval;
    LARGE_INTEGER bigTimeOut;
    bigTimeOut.LowPart = -((int)timeout) * 10000;   // measured in units of 100 fs in NT
    bigTimeOut.HighPart = (timeout == 0) ? 0:(-1);  // negative means relative to current time

    // note: DDK docs are wrong, it returns an error code, not a TRUE/FALSE result
    ddrval = EngWaitForSingleObject(hEvent, &bigTimeOut);
    EngSetEvent(hEvent);
    return ddrval;
}

LPPROCESSINFO pmGetProcess(GLOBALDATA *pDriverData, DWORD processID)
{
	if (pDriverData->lpProcessInfoHead == NULL) {
		pmAddProcess(pDriverData, processID);
	}

    return(pDriverData->lpProcessInfoHead);
}
   
BOOL pmAddProcess(GLOBALDATA *pDriverData, DWORD processID)
{
    LPPROCESSINFO lpProcInfo;

    lpProcInfo = &(pDriverData->procInfo);

    lpProcInfo->lpNext      = NULL;
    lpProcInfo->dwProcessID = processID;

    // under WIN2k, all driver events are shared among all processes
    // cannot use standard user event services, must use EngEvent services
    // set the ring3 event member to equal the ring0 event to make thing easier to macrotize

    // idle event
    if (lpProcInfo->hIdleSyncEvent0 == NULL) {
        EngCreateEvent(&lpProcInfo->hIdleSyncEvent0);
        lpProcInfo->hIdleSyncEvent3 = lpProcInfo->hIdleSyncEvent0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_DD_ROP_GDI_RECT_AND_TEXT,
                           NV_DD_EVENT_IDLE_SYNC, NV01_EVENT_WIN32_EVENT,
                           NV04A_NOTIFIERS_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hIdleSyncEvent0)))) {
            EngDeleteEvent(pDriverData->procInfo.hIdleSyncEvent0);
            lpProcInfo->hIdleSyncEvent0 = NULL;
            lpProcInfo->hIdleSyncEvent3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hIdleSyncEvent3);
        }
    }

    // timer event
    if (lpProcInfo->hTimer0 == NULL &&
       (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0004_TIMER)) {
        EngCreateEvent(&lpProcInfo->hTimer0);
        lpProcInfo->hTimer3 = lpProcInfo->hTimer0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_TIMER_IID,
                           NV_DD_EVENT_TIMER, NV01_EVENT_WIN32_EVENT,
                           NV004_NOTIFIERS_SET_ALARM_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hTimer0)))) {
            EngDeleteEvent(pDriverData->procInfo.hTimer0);
            lpProcInfo->hTimer0 = NULL;
            lpProcInfo->hTimer3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hTimer3);
        }
    }

    // DMA to Video memory event
    if (lpProcInfo->hDMABlitToVid0 == NULL) {
        EngCreateEvent(&lpProcInfo->hDMABlitToVid0);
        lpProcInfo->hDMABlitToVid3 = lpProcInfo->hDMABlitToVid0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, pDriverData->vpp.m_obj_a[DMABLT_TO_VID_NDX].classIID,
                           NV_DD_EVENT_DMABLT_TO_VID, NV01_EVENT_WIN32_EVENT,
                           NV039_NOTIFIERS_BUFFER_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hDMABlitToVid0)))) {
            EngDeleteEvent(pDriverData->procInfo.hDMABlitToVid0);
            lpProcInfo->hDMABlitToVid0 = NULL;
            lpProcInfo->hDMABlitToVid3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hDMABlitToVid3);
        }
    }

    // Full screen mirror event
    if (lpProcInfo->hFSMirror0 == NULL) {
        EngCreateEvent(&lpProcInfo->hFSMirror0);
        lpProcInfo->hFSMirror3 = lpProcInfo->hFSMirror0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_SCALED_IMAGE1_IID,
                           NV_DD_EVENT_FSMIRROR, NV01_EVENT_WIN32_EVENT,
                           NV089_NOTIFIERS_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hFSMirror0)))) {
            EngDeleteEvent(pDriverData->procInfo.hFSMirror0);
            lpProcInfo->hFSMirror0 = NULL;
            lpProcInfo->hFSMirror3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hFSMirror3);
        }
    }

    // FourCCBlt event
    if (lpProcInfo->hFourCCBlt0 == NULL) {
        EngCreateEvent(&lpProcInfo->hFourCCBlt0);
        lpProcInfo->hFourCCBlt3 = lpProcInfo->hFourCCBlt0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_SCALED_IMAGE2_IID,
                           NV_DD_EVENT_FOURCCBLT, NV01_EVENT_WIN32_EVENT,
                           NV089_NOTIFIERS_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hFourCCBlt0)))) {
            EngDeleteEvent(pDriverData->procInfo.hFourCCBlt0);
            lpProcInfo->hFourCCBlt0 = NULL;
            lpProcInfo->hFourCCBlt3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hFourCCBlt3);
        }
    }

    // don't try to create overlay events if an overlay doesn't exist
    if (pDriverData->vpp.dwOverlaySurfaces == 0)
    {
        return TRUE;
    }

    // Overlay flip events
    if (lpProcInfo->hOvlFlipEvenEvent0 == NULL) {
        EngCreateEvent(&lpProcInfo->hOvlFlipEvenEvent0);
        lpProcInfo->hOvlFlipEvenEvent3 = lpProcInfo->hOvlFlipEvenEvent0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_OVERLAY_IID,
                           NV_DD_EVENT_OVL_FLIP1, NV01_EVENT_WIN32_EVENT,
                           NV07A_NOTIFIERS_SET_OVERLAY(0), (void*) (*((LPDWORD)lpProcInfo->hOvlFlipEvenEvent0)))) {
            // error because overlay not created yet, so do this later
            EngDeleteEvent(pDriverData->procInfo.hOvlFlipEvenEvent0);
            lpProcInfo->hOvlFlipEvenEvent0 = NULL;
            lpProcInfo->hOvlFlipEvenEvent3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hOvlFlipEvenEvent3);
        }
    }

    if (lpProcInfo->hOvlFlipOddEvent0 == NULL) {
        EngCreateEvent(&lpProcInfo->hOvlFlipOddEvent0);
        lpProcInfo->hOvlFlipOddEvent3 = lpProcInfo->hOvlFlipOddEvent0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_OVERLAY_IID,
                           NV_DD_EVENT_OVL_FLIP2, NV01_EVENT_WIN32_EVENT,
                           NV07A_NOTIFIERS_SET_OVERLAY(1), (void*) (*((LPDWORD)lpProcInfo->hOvlFlipOddEvent0)))) {
            // error because overlay not created yet, so do this later
            EngDeleteEvent(pDriverData->procInfo.hOvlFlipOddEvent0);
            lpProcInfo->hOvlFlipOddEvent0 = NULL;
            lpProcInfo->hOvlFlipOddEvent3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hOvlFlipOddEvent3);
        }
    }

    // Format conversion to YUV422 event
    if (lpProcInfo->hConvert0 == NULL) {
        EngCreateEvent(&lpProcInfo->hConvert0);
        lpProcInfo->hConvert3 = lpProcInfo->hConvert0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_V2V_FORMAT_Y_IID,
                           NV_DD_EVENT_CONVERT_SYNC, NV01_EVENT_WIN32_EVENT,
                           NV039_NOTIFIERS_BUFFER_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hConvert0)))) {
            EngDeleteEvent(pDriverData->procInfo.hConvert0);
            lpProcInfo->hConvert0 = NULL;
            lpProcInfo->hConvert3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hConvert3);
        }
    }

    // DVD subpicture
    if (lpProcInfo->hSubPicture0 == NULL) {
        EngCreateEvent(&lpProcInfo->hSubPicture0);
        lpProcInfo->hSubPicture3 = lpProcInfo->hSubPicture0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, pDriverData->vpp.m_obj_a[DVDPICT_NDX].classIID,
                           NV_DD_EVENT_DVD_SUBPICTURE, NV01_EVENT_WIN32_EVENT,
                           NV038_NOTIFIERS_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hSubPicture0)))) {
            EngDeleteEvent(pDriverData->procInfo.hSubPicture0);
            lpProcInfo->hSubPicture0 = NULL;
            lpProcInfo->hSubPicture3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hSubPicture3);
        }
    }

    // Colour control events
    if (lpProcInfo->hColourControl0 == NULL) {
        EngCreateEvent(&lpProcInfo->hColourControl0);
        lpProcInfo->hColourControl3 = lpProcInfo->hColourControl0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_V2OSH_FORMAT_IID,
                           NV_DD_EVENT_VIDEO_TO_SYSTEM_SYNC, NV01_EVENT_WIN32_EVENT,
                           NV039_NOTIFIERS_BUFFER_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hColourControl0))) ||
            NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_OSH2V_FORMAT_IID,
                           NV_DD_EVENT_SYSTEM_TO_VIDEO_SYNC, NV01_EVENT_WIN32_EVENT,
                           NV039_NOTIFIERS_BUFFER_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hColourControl0))) ||
            NvRmAllocEvent(pDriverData->dwRootHandle, NV_VPP_ALPHA_BLIT_IID,
                           NV_DD_EVENT_ALPHA_BLIT_SYNC, NV01_EVENT_WIN32_EVENT,
                           NV05F_NOTIFIERS_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hColourControl0)))) {
            EngDeleteEvent(pDriverData->procInfo.hColourControl0);
            lpProcInfo->hColourControl0 = NULL;
            lpProcInfo->hColourControl3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hColourControl3);
        }
    }

    // Temporal filter event
    if (lpProcInfo->hTFilter0 == NULL) {
        DWORD result;
        EngCreateEvent(&lpProcInfo->hTFilter0);
        lpProcInfo->hTFilter3 = lpProcInfo->hTFilter0;
        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_20) {
            result = NvRmAllocEvent(pDriverData->dwRootHandle, D3D_KELVIN_PRIMITIVE,
                                    NV_DD_EVENT_COMBINER_SYNC, NV01_EVENT_WIN32_EVENT,
                                    NV097_NOTIFIERS_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hTFilter0)));
        } else if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
            result = NvRmAllocEvent(pDriverData->dwRootHandle, D3D_CELSIUS_PRIMITIVE,
                                    NV_DD_EVENT_COMBINER_SYNC, NV01_EVENT_WIN32_EVENT,
                                    NV056_NOTIFIERS_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hTFilter0)));
        } else {
            result = NvRmAllocEvent(pDriverData->dwRootHandle, D3D_DX6_MULTI_TEXTURE_TRIANGLE,
                                    NV_DD_EVENT_COMBINER_SYNC, NV01_EVENT_WIN32_EVENT,
                                    NV055_NOTIFIERS_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hTFilter0)));
        }
        if (result) {
            EngDeleteEvent(pDriverData->procInfo.hTFilter0);
            lpProcInfo->hTFilter0 = NULL;
            lpProcInfo->hTFilter3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hTFilter3);
        }
    }

    // Skip this on NV4/NV5
    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10)
    {
        // Deinterlace filter event
        if (lpProcInfo->hDFilter0 == NULL) {
            DWORD result;
            EngCreateEvent(&lpProcInfo->hDFilter0);
            lpProcInfo->hDFilter3 = lpProcInfo->hDFilter0;
            if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_20) {
                result = NvRmAllocEvent(pDriverData->dwRootHandle, D3D_KELVIN_PRIMITIVE,
                                        NV_DD_EVENT_DFILTER_SYNC, NV01_EVENT_WIN32_EVENT,
                                        NV097_NOTIFIERS_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hDFilter0)));
            } else {
                result = NvRmAllocEvent(pDriverData->dwRootHandle, D3D_CELSIUS_PRIMITIVE,
                                        NV_DD_EVENT_DFILTER_SYNC, NV01_EVENT_WIN32_EVENT,
                                        NV056_NOTIFIERS_NOTIFY, (void*) (*((LPDWORD)lpProcInfo->hDFilter0)));
            }
            if (result) {
                EngDeleteEvent(pDriverData->procInfo.hDFilter0);
                lpProcInfo->hDFilter0 = NULL;
                lpProcInfo->hDFilter3 = NULL;
            } else {
                EngSetEvent(lpProcInfo->hDFilter3);
            }
        }
    }

    // Primary flip events
    if (lpProcInfo->hPrimary0FlipEven0 == NULL) {
        EngCreateEvent(&lpProcInfo->hPrimary0FlipEven0);
        lpProcInfo->hPrimary0FlipEven3 = lpProcInfo->hPrimary0FlipEven0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_DD_VIDEO_LUT_CURSOR_DAC,
                           NV_DD_EVENT_PRIMARY0_FLIP1, NV01_EVENT_WIN32_EVENT,
                           NV067_NOTIFIERS_SET_IMAGE(0), (void*) (*((LPDWORD)lpProcInfo->hPrimary0FlipEven0)))) {
            EngDeleteEvent(pDriverData->procInfo.hPrimary0FlipEven0);
            lpProcInfo->hPrimary0FlipEven0 = NULL;
            lpProcInfo->hPrimary0FlipEven3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hPrimary0FlipEven3);
        }
    }

    if (lpProcInfo->hPrimary0FlipOdd0 == NULL) {
        EngCreateEvent(&lpProcInfo->hPrimary0FlipOdd0);
        lpProcInfo->hPrimary0FlipOdd3 = lpProcInfo->hPrimary0FlipOdd0;
        if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_DD_VIDEO_LUT_CURSOR_DAC,
                           NV_DD_EVENT_PRIMARY0_FLIP2, NV01_EVENT_WIN32_EVENT,
                           NV067_NOTIFIERS_SET_IMAGE(1), (void*) (*((LPDWORD)lpProcInfo->hPrimary0FlipOdd0)))) {
            EngDeleteEvent(pDriverData->procInfo.hPrimary0FlipOdd0);
            lpProcInfo->hPrimary0FlipOdd0 = NULL;
            lpProcInfo->hPrimary0FlipOdd3 = NULL;
        } else {
            EngSetEvent(lpProcInfo->hPrimary0FlipOdd3);
        }
    }

    // Conditionally create events for primary on 2nd head
    if (pDriverData->dwHeads > 1)
    {
        if (lpProcInfo->hPrimary1FlipEven0 == NULL) {
            EngCreateEvent(&lpProcInfo->hPrimary1FlipEven0);
            lpProcInfo->hPrimary1FlipEven3 = lpProcInfo->hPrimary1FlipEven0;
            if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_DD_VIDEO_LUT_CURSOR_DAC + 1,
                               NV_DD_EVENT_PRIMARY1_FLIP1, NV01_EVENT_WIN32_EVENT,
                               NV067_NOTIFIERS_SET_IMAGE(0), (void*) (*((LPDWORD)lpProcInfo->hPrimary1FlipEven0)))) {
                EngDeleteEvent(pDriverData->procInfo.hPrimary1FlipEven0);
                lpProcInfo->hPrimary1FlipEven0 = NULL;
                lpProcInfo->hPrimary1FlipEven3 = NULL;
            } else {
                EngSetEvent(lpProcInfo->hPrimary1FlipEven3);
            }
        }

        if (lpProcInfo->hPrimary1FlipOdd0 == NULL) {
            EngCreateEvent(&lpProcInfo->hPrimary1FlipOdd0);
            lpProcInfo->hPrimary1FlipOdd3 = lpProcInfo->hPrimary1FlipOdd0;
            if (NvRmAllocEvent(pDriverData->dwRootHandle, NV_DD_VIDEO_LUT_CURSOR_DAC + 1,
                               NV_DD_EVENT_PRIMARY1_FLIP2, NV01_EVENT_WIN32_EVENT,
                               NV067_NOTIFIERS_SET_IMAGE(1), (void*) (*((LPDWORD)lpProcInfo->hPrimary1FlipOdd0)))) {
                EngDeleteEvent(pDriverData->procInfo.hPrimary1FlipOdd0);
                lpProcInfo->hPrimary1FlipOdd0 = NULL;
                lpProcInfo->hPrimary1FlipOdd3 = NULL;
            } else {
                EngSetEvent(lpProcInfo->hPrimary1FlipOdd3);
            }
        }
    }

    pDriverData->lpProcessInfoHead = lpProcInfo;
    return TRUE;
}

BOOL pmDeleteProcess(DWORD processID)
{
    return pmDeleteProcessPDD(pDriverData, processID);
}

BOOL pmDeleteProcessPDD(GLOBALDATA *pDriverData, DWORD processID)
{
    if (pDriverData->procInfo.hIdleSyncEvent0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_IDLE_SYNC);
        EngDeleteEvent(pDriverData->procInfo.hIdleSyncEvent0);
        pDriverData->procInfo.hIdleSyncEvent0 = NULL;
        pDriverData->procInfo.hIdleSyncEvent3 = NULL;
    }
    
    if (pDriverData->procInfo.hTimer0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_TIMER);
        EngDeleteEvent(pDriverData->procInfo.hTimer0);
        pDriverData->procInfo.hTimer0 = NULL;
        pDriverData->procInfo.hTimer3 = NULL;
    }
    
    if (pDriverData->procInfo.hOvlFlipEvenEvent0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_OVL_FLIP1);
        EngDeleteEvent(pDriverData->procInfo.hOvlFlipEvenEvent0);
        pDriverData->procInfo.hOvlFlipEvenEvent0 = NULL;
        pDriverData->procInfo.hOvlFlipEvenEvent3 = NULL;
    }
    
    if (pDriverData->procInfo.hOvlFlipOddEvent0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_OVL_FLIP2);
        EngDeleteEvent(pDriverData->procInfo.hOvlFlipOddEvent0);
        pDriverData->procInfo.hOvlFlipOddEvent0 = NULL;
        pDriverData->procInfo.hOvlFlipOddEvent3 = NULL;
    }
    
    if (pDriverData->procInfo.hDMABlitToVid0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_DMABLT_TO_VID);
        EngDeleteEvent(pDriverData->procInfo.hDMABlitToVid0);
        pDriverData->procInfo.hDMABlitToVid0 = NULL;
        pDriverData->procInfo.hDMABlitToVid3 = NULL;
    }
    
    if (pDriverData->procInfo.hConvert0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_CONVERT_SYNC);
        EngDeleteEvent(pDriverData->procInfo.hConvert0);
        pDriverData->procInfo.hConvert0 = NULL;
        pDriverData->procInfo.hConvert3 = NULL;
    }
    
    if (pDriverData->procInfo.hSubPicture0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_DVD_SUBPICTURE);
        EngDeleteEvent(pDriverData->procInfo.hSubPicture0);
        pDriverData->procInfo.hSubPicture0 = NULL;
        pDriverData->procInfo.hSubPicture3 = NULL;
    }
    
    if (pDriverData->procInfo.hColourControl0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_VIDEO_TO_SYSTEM_SYNC);
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_SYSTEM_TO_VIDEO_SYNC);
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_ALPHA_BLIT_SYNC);
        EngDeleteEvent(pDriverData->procInfo.hColourControl0);
        pDriverData->procInfo.hColourControl0 = NULL;
        pDriverData->procInfo.hColourControl3 = NULL;
    }
    
    if (pDriverData->procInfo.hTFilter0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_COMBINER_SYNC);
        EngDeleteEvent(pDriverData->procInfo.hTFilter0);
        pDriverData->procInfo.hTFilter0 = NULL;
        pDriverData->procInfo.hTFilter3 = NULL;
    }
    
    if (pDriverData->procInfo.hDFilter0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_DFILTER_SYNC);
        EngDeleteEvent(pDriverData->procInfo.hDFilter0);
        pDriverData->procInfo.hDFilter0 = NULL;
        pDriverData->procInfo.hDFilter3 = NULL;
    }
    
    if (pDriverData->procInfo.hFSMirror0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_FSMIRROR);
        EngDeleteEvent(pDriverData->procInfo.hFSMirror0);
        pDriverData->procInfo.hFSMirror0 = NULL;
        pDriverData->procInfo.hFSMirror3 = NULL;
    }

    if (pDriverData->procInfo.hFourCCBlt0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_FOURCCBLT);
        EngDeleteEvent(pDriverData->procInfo.hFourCCBlt0);
        pDriverData->procInfo.hFourCCBlt0 = NULL;
        pDriverData->procInfo.hFourCCBlt3 = NULL;
    }
    
    if (pDriverData->procInfo.hPrimary0FlipEven0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_PRIMARY0_FLIP1);
        EngDeleteEvent(pDriverData->procInfo.hPrimary0FlipEven0);
        pDriverData->procInfo.hPrimary0FlipEven0 = NULL;
        pDriverData->procInfo.hPrimary0FlipEven3 = NULL;
    }
    
    if (pDriverData->procInfo.hPrimary0FlipOdd0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_PRIMARY0_FLIP2);
        EngDeleteEvent(pDriverData->procInfo.hPrimary0FlipOdd0);
        pDriverData->procInfo.hPrimary0FlipOdd0 = NULL;
        pDriverData->procInfo.hPrimary0FlipOdd3 = NULL;
    }
    
    if (pDriverData->procInfo.hPrimary1FlipEven0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_PRIMARY1_FLIP1);
        EngDeleteEvent(pDriverData->procInfo.hPrimary1FlipEven0);
        pDriverData->procInfo.hPrimary1FlipEven0 = NULL;
        pDriverData->procInfo.hPrimary1FlipEven3 = NULL;
    }
    
    if (pDriverData->procInfo.hPrimary1FlipOdd0)
    {
        NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_EVENT_PRIMARY1_FLIP2);
        EngDeleteEvent(pDriverData->procInfo.hPrimary1FlipOdd0);
        pDriverData->procInfo.hPrimary1FlipOdd0 = NULL;
        pDriverData->procInfo.hPrimary1FlipOdd3 = NULL;
    }
    
    pDriverData->lpProcessInfoHead = NULL;
    return(TRUE);
}

void pmDeleteAllProcesses(GLOBALDATA *pDriverData)
{
    pmDeleteProcessPDD(pDriverData, NULL);
    return;
}

#elif IS_WINNT4

// Events are not available for use by the WinNT4 driver,
// but since the 98 and 2k drivers use them, we stub these
// functions so that a valid pointer to a process structure is 
// returned.
//
// That makes the calling code more uniform across platforms.
//

HRESULT NvResetEvent(HDRVEVENT hEvent)               { return STATUS_WAIT_0; }
HRESULT NvSetEvent(HDRVEVENT hEvent)                 { return STATUS_WAIT_0; }
HRESULT NvWaitEvent(HDRVEVENT hEvent, DWORD timeout) { return STATUS_WAIT_0; }


LPPROCESSINFO pmGetProcess(GLOBALDATA *pDriverData, DWORD processID)
{
	if (pDriverData->lpProcessInfoHead == NULL) {
		pmAddProcess(pDriverData, processID);
	}

    return(pDriverData->lpProcessInfoHead);
}
   
BOOL pmAddProcess(GLOBALDATA *pDriverData, DWORD processID)
{
    LPPROCESSINFO lpProcInfo;

    lpProcInfo = &(pDriverData->procInfo);

    // Make sure all event handles are NULL
    memset(lpProcInfo,0,sizeof(*lpProcInfo));

    lpProcInfo->lpNext      = NULL;
    lpProcInfo->dwProcessID = processID;

    pDriverData->lpProcessInfoHead = lpProcInfo;
    return TRUE;
}

BOOL pmDeleteProcess(DWORD processID)
{
    return TRUE;
}

void pmDeleteAllProcesses(GLOBALDATA *pDriverData)
{
    return;
}

#endif // OS determination

