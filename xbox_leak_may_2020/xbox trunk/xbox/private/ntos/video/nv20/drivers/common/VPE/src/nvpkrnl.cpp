 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
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
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
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

/*
==============================================================================

    NVIDIA Corporation, (C) Copyright
    1996,1997,1998,1999. All rights reserved.

    nvpkrnl.cpp

    Video Port kernel interface (Miniport/MiniVDD)

    Feb 10, 1999

==============================================================================
*/
#ifdef NVPE

#include "nvpkrnl.h"

// NVPE inludes
#include "nvpesrvc.h"  // shared structure and definitions (miniport/DDraw)

// Win9x and NT differences
#ifdef WINNT

#define     NVP_CLIENT              (ppdev->hClient)
#define     NVP_DRIVER_HANDLE       (pDriverData->nvpeState.hVPEKernel)      // (pDriverData->ppdev->hDriver)
#define     NVP_VPE_CHANNEL_PTR     (pDriverData->NvDevFlatVPE)
#define     NVP_OVERLAY_CHANNEL_PTR (pDriverData->NvDevVideoFlatPio)
#define     NVP_FB_BASE_ADDRESS     (ppdev->pjFrameBufbase)
#define     NVP_FB_LENGTH           (ppdev->cbFrameBuf)
#define     PNVP_DDRAW_SURFACE      PDD_SURFACE_INT     // LPDDRAWI_DDRAWSURFACE_INT for Win9x
#define     NVP_OVERLAY_SUBCHANNEL  7 //DD_SPARE            // NV_DD_SPARE for Win9x
#define     NVP_GET_SURFACE_OFFSET(x) ((U032)(x))
#define     NVP_GET_SURFACE_POINTER(x) ((U032)(x) + (U032)(ppdev->pjFrameBufbase)) // on win2k, pVidMem are offsets not linear pointers !!!

#else
// Win9x version

#define     IOCTL_VIDEO_VPE_COMMAND 0x300cafe0
#define     VPE_EVENT               "VPE_Event"
#define     NVP_CLIENT              pDriverData->dwRootHandle //(pDriverData->ppdev->hClient)
#define     NVP_DRIVER_HANDLE       (pDriverData->nvpeState.hVPEKernel)     //(pDriverData->ppdev->hDriver)
#define     NVP_VPE_CHANNEL_PTR     (pDriverData->NvDevFlatVPE)
#define     NVP_OVERLAY_CHANNEL_PTR (pDriverData->NvDevVideoFlatPio)
#define     NVP_FB_BASE_ADDRESS     pDriverData->VideoHeapBase   //(pDriverData->ppdev->pjScreen)
#define     NVP_FB_LENGTH           (pDriverData->VideoHeapEnd - pDriverData->VideoHeapBase) //(pDriverData->ppdev->cbFrameBuf)
#define     PNVP_DDRAW_SURFACE      LPDDRAWI_DDRAWSURFACE_INT//PDD_SURFACE_INT     // LPDDRAWI_DDRAWSURFACE_INT for Win9x
#define     NVP_OVERLAY_SUBCHANNEL  7 //DD_SPARE            // NV_DD_SPARE for Win9x
#define     NVP_GET_SURFACE_OFFSET(x)  ((U032)(x) - (U032)(pDriverData->BaseAddress))
#define     NVP_GET_SURFACE_POINTER(x) ((U032)(x))

#endif //WINNT

// local prototypes
NVP_STATUS NVPGetImageDim(
    LPDDVIDEOPORTDESC pVPDesc,
    LPDDVIDEOPORTINFO pVideoInfo,
    PNVP_CMD_OBJECT pnvCmdObj,
    BOOL bIsVideoSurf);
NVP_STATUS NVPRegSurface(
    GLOBALDATA *pDriverData,
    PNVP_DDRAW_SURFACE pDDSurface, //LPDDRAWI_DDRAWSURFACE_INT *pDDSurface, // lpInput->lplpDDSurface,
    PNVP_CMD_OBJECT pnvCmdObj,
    DWORD dwSurfaceId);





NVP_STATUS NVPCreateEvents(GLOBALDATA *pDriverData, PNVP_CMD_OBJECT pnvInitObj);
NVP_STATUS NVPDestroyEvents(GLOBALDATA *pDriverData);
NVP_STATUS NVPCallKernel(HANDLE hDevice, PNVP_CMD_OBJECT pnvInCmdObj, PNVP_CMD_OBJECT pnvOutCmdObj);
NVP_STATUS NVPEGetHandleToKernel(GLOBALDATA *pDriverData, HANDLE hDevice);
NVP_STATUS NVPDumpVPSurfInfo(LPDDHAL_UPDATEVPORTDATA lpInput, NVP_CMD_OBJECT *pnvCmdObj);
NVP_STATUS NVPDumpVPConnectInfo(DWORD dwVPConnectionFlags);

#ifdef VPEFSMIRROR
// [XW] added for FS Mirror
NVP_STATUS NVPFsRegSurface(
    GLOBALDATA *pDriverData,
    PNVP_CMD_OBJECT pnvCmdObj,
    DWORD dwSurfaceId,
    DWORD dwSurfPitch,
    DWORD dwSurfOffset,
    DWORD dwSurfHeight,
    DWORD dwSurfWidth
    );

/*
==============================================================================

    NVPFsRegSurface

    Description:  Interface to the miniport... register surfaces etc.


    Parameters:


    Note:       Xun Wang September 25, 2000

==============================================================================
*/

NVP_STATUS NVPFsRegSurface(
    GLOBALDATA *pDriverData,
    PNVP_CMD_OBJECT pnvCmdObj,
    DWORD dwSurfaceId,
    DWORD dwSurfPitch,
    DWORD dwSurfOffset,
    DWORD dwSurfHeight,
    DWORD dwSurfWidth
    )
{

    // register the fullscreen mirror surface in the kernel.
    dwSurfOffset = (U032)(pDriverData->BaseAddress+dwSurfOffset);

    pnvCmdObj->pSurfVidMem = NVP_GET_SURFACE_POINTER(dwSurfOffset);
    pnvCmdObj->dwSurfPitch = dwSurfPitch;
    pnvCmdObj->dwSurfWidth = dwSurfWidth;
    pnvCmdObj->dwSurfHeight = dwSurfHeight;

    pnvCmdObj->dwSurfOffset = NVP_GET_SURFACE_OFFSET(dwSurfOffset);

    pnvCmdObj->dwSurfaceId = dwSurfaceId;
    pnvCmdObj->dwSrvId = NVP_SRV_REG_FSMIRROR_SURFACE;

     // call vpe kernel to register surface
    NVPCallKernel(NVP_DRIVER_HANDLE, pnvCmdObj, NULL);

    return NVP_STATUS_SUCCESS;

}
/*
==============================================================================

    NVPFsMirrorEnable

    Description:  Interface to the miniport... register surfaces etc.


    Parameters:


    Note:       Xun Wang September 25, 2000

==============================================================================
*/
NVP_STATUS NVPFsMirrorEnable(GLOBALDATA *pDriverData)
{
    NVP_CMD_OBJECT nvInObj;
    int i;

    DPF("nvpkrnl.cpp: NVPFsMirrorEnable()\n");

    if (!pDriverData->nvpeState.bVPEInitialized) {
        return NVP_STATUS_SUCCESS;
    }

    if (pDriverData->nvpeState.bFsMirrorEnabled) {
        NVPFsMirrorDisable(pDriverData);
    }

//    if (NVPEGetHandleToKernel(pDriverData, NULL) != NVP_STATUS_SUCCESS) {
//        return NVP_STATUS_FAILURE;
//    }

    // NVP_SRV_ENABLE_FSMIRROR to send down the other information
    nvInObj.dwPortId      = 0;    // use 0 for now !
    nvInObj.dwSrvId = NVP_SRV_ENABLE_FSMIRROR;
    nvInObj.dwSize = sizeof(NVP_CMD_OBJECT);
    nvInObj.regOverlayMode2 = pDriverData->vpp.regOverlayMode2;
    nvInObj.dwOverlayFSHead = pDriverData->vpp.dwOverlayFSHead;
    nvInObj.dwDesktopState = pDriverData->dwDesktopState;

    // figure out which device to put the overlay on (copied from VPP)
    DWORD dwFSMirrorDevice = (pDriverData->vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK) - 1;
    if (pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE && dwFSMirrorDevice != 0xFFFFFFFF) {
        if (dwFSMirrorDevice >= 14) {
            dwFSMirrorDevice = 1;
        }
        nvInObj.dwOverlayFSOvlHead = dwFSMirrorDevice ^ 1;
        } else {
#if (IS_WINNT4 || IS_WINNT5)
            nvInObj.dwOverlayFSOvlHead = (ppdev->ulDeviceDisplay[0]);
#else
            nvInObj.dwOverlayFSOvlHead = (pDXShare->dwHeadNumber);
#endif
        }

    // call vpe kernel to enable FS Mirror
    NVPCallKernel(NVP_DRIVER_HANDLE, &nvInObj, NULL);

    // NVP_SRV_REG_FSMIRROR_SURFACE  to register surfaces

    for (i = 0; i < (int)(pDriverData->vpp.dwOverlayFSNumSurfaces); i++)
    {
        NVPFsRegSurface(pDriverData,
                        &nvInObj,
                        i,
                        pDriverData->vpp.dwOverlayFSPitch,
                        pDriverData->vpp.dwOverlayFSOffset[i],
                        pDriverData->vpp.dwOverlayFSHeight,
                        pDriverData->vpp.dwOverlayFSWidth
                        );
    }

    //CloseHandle(NVP_DRIVER_HANDLE);
    //NVP_DRIVER_HANDLE = 0;

    pDriverData->nvpeState.bFsMirrorEnabled = TRUE;

    return NVP_STATUS_SUCCESS;
}

/*
==============================================================================

    NVPFsMirrorDisable

    Description:  Interface function to the miniport.. cleanup


    Parameters:


    Note:       Xun Wang September 25, 2000

==============================================================================
*/
NVP_STATUS NVPFsMirrorDisable(GLOBALDATA *pDriverData)
{
    NVP_CMD_OBJECT nvInitObj;

    DPF("nvpkrnl.cpp: NVPFsMirrorDisable()\n");

    if (!pDriverData->nvpeState.bFsMirrorEnabled) {
        return NVP_STATUS_SUCCESS;
    }

    // fill up structure with data needed for uninitialization
    nvInitObj.dwSize = sizeof(NVP_CMD_OBJECT);
    nvInitObj.dwSrvId = NVP_SRV_DISABLE_FSMIRROR;
    nvInitObj.hClient = NVP_CLIENT;
    nvInitObj.pChannel = (U032) NVP_VPE_CHANNEL_PTR;    // pDriverData->NvDevFlatVPE
    nvInitObj.dwPortId = 0; // use 0 for now !
    nvInitObj.dwDeviceId = NV_WIN_DEVICE;

    // call vpe kernel to disable FS Mirror
    NVPCallKernel(NVP_DRIVER_HANDLE, &nvInitObj, NULL);

    pDriverData->nvpeState.bFsMirrorEnabled = FALSE;

    return NVP_STATUS_SUCCESS;
}

#endif

/*
==============================================================================

    NVPIsVPEEnabled

    Description:    On Win2k: call vpe kernel which to check the regitry
                              to see if VPPEEnable=0 is set in regsitry !
                    On Win9x: read VPEEnable key registry here.

                    Both platforms: pass shared flag "pOvrRing0FlipFlag"
                                    to kernel

   Parameters:      DWORD hDevice:  - handle to the kernel device
                                    - if handle is NULL, then try to connect here

   Note:            this function is called at DDraw initialization time !

==============================================================================
*/
BOOL __stdcall NVPIsVPEEnabled(
    GLOBALDATA *pDriverData,
    DWORD hDevice)
{
    NVP_CMD_OBJECT nvInObj, nvOutObj;
    BOOL bEnableVPE;
#ifndef WINNT
    int i, j;
    char regStr[256];
    const char ddSubKey[] = NV4_REG_DIRECT_DRAW_SUBKEY;
    HKEY hKey;
#endif

    if (NVPEGetHandleToKernel(pDriverData, (HANDLE)hDevice) != NVP_STATUS_SUCCESS)
        return FALSE;

    // fill up structure with initialization data
    nvOutObj.dwSize         = sizeof(NVP_CMD_OBJECT);
    nvOutObj.dwPortId       = 0;

    nvInObj.dwSize          = sizeof(NVP_CMD_OBJECT);
    nvInObj.dwSrvId         = NVP_SRV_IS_VPE_ENABLED;
    nvInObj.dwPortId        = 0;

#ifndef WINNT
    nvInObj.pOvrRing0FlipFlag = (DWORD)&(pDriverData->bRing0FlippingFlag);

    // call vpe kernel
    NVPCallKernel(NVP_DRIVER_HANDLE, &nvInObj, NULL);

    CloseHandle(NVP_DRIVER_HANDLE);
    NVP_DRIVER_HANDLE = 0;

    // Grab the local registry path and append the DirectDraw subkey to the string
    // without using the C runtime library.
    MyExtEscape(pDXShare->dwHeadNumber, NV_ESC_GET_LOCAL_REGISTRY_PATH, 0, NULL, 256, regStr);

    for (i = 4; i < 255; i++)
    {
        if (regStr[i] == '\0')
            break;
    }
    regStr[i] = '\\';
    j = 0;
    while (ddSubKey[j] != '\0' && (i+j+1) < 256)
    {
        regStr[i+j+1] = ddSubKey[j++];
    }
    regStr[i+j+1] = '\0';

    // VPE enabled by default
    bEnableVPE = 1;

    if (RegOpenKeyEx((HKEY)(*((LPDWORD)&regStr[0])), &regStr[4], 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        DWORD   dwSize = sizeof(long);
        DWORD   dwType = REG_DWORD;
        long    lValue;

        //check for VPE enable bit
        if (RegQueryValueEx(hKey, NV4_REG_VPE_ENABLE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS)
        {
            if (!lValue)
                bEnableVPE = FALSE;
        }
        RegCloseKey(hKey);
    }

#else
    nvInObj.pOvrRing0FlipFlag = 0;

    // win2k: call vpe kernel to read registry !
    NVPCallKernel(NVP_DRIVER_HANDLE, &nvInObj, &nvOutObj);

    bEnableVPE = ((nvOutObj.dwPortId == 1) ? TRUE : FALSE);
#endif // !WINNT

    return bEnableVPE;
}

/*
==============================================================================

    NVPEGetHandleToKernel

    Description:    get a handle to minivdd(Win9x) or miniport(Win2k)

==============================================================================
*/
NVP_STATUS NVPEGetHandleToKernel(
    GLOBALDATA *pDriverData,
    HANDLE hDevice)
{
#ifndef WINNT
    HANDLE hVxdHandle = INVALID_HANDLE_VALUE;
#endif // !WINNT

    // forced to use the device handle passed to us !
    if (hDevice)
        NVP_DRIVER_HANDLE = hDevice;
    else
    {
#ifndef WINNT
        if (!NVP_DRIVER_HANDLE)
        {
            // Check whether we are primary or secondary display
            DWORD dwSuccess, dwDeviceHandle;
            dwSuccess = NvRmConfigGet(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                                      NV_CFG_DEVICE_HANDLE, &dwDeviceHandle);

            if (dwSuccess != NVOS_CGE_STATUS_SUCCESS)
            {
                // Unable to know if we are primary or secondary
                // We must fail the VPE since loading the wrong VDD would cause a BSOD.
                DPF("ERROR: VPE is unable to know if we are primary or secondary display\n");
                return NVP_STATUS_FAILURE;
            }

            if (dwDeviceHandle == 1)
            {
                // We are primary display
                hVxdHandle = CreateFile(NVX_PRIMARY_MINIVDD, 0,0,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
            }
            else
            {
                // We are secondary display
                // NO VPE support yet
                DPF("ERROR: VPE not supported on second display\n");
                return NVP_STATUS_FAILURE;
            }

            if (hVxdHandle != INVALID_HANDLE_VALUE)
            {
                NVP_DRIVER_HANDLE = hVxdHandle;
            }
            else
            {
                DPF("ERROR: unable to connect to MiniVDD!\n");
                return NVP_STATUS_FAILURE;
            }
       }
#else
        NVP_DRIVER_HANDLE = ppdev->hDriver;
#endif // WINNT
    }

    return NVP_STATUS_SUCCESS;
}

/*
==============================================================================

    NVPInitialize

    Description:    pass initialization and other VPE data to kernel

==============================================================================
*/
NVP_STATUS NVPInitialize
(
    GLOBALDATA *pDriverData,
    DWORD dwVPConnectionFlags,
    LPDDRAWI_DIRECTDRAW_GBL pDDGbl
)
{
    NVP_CMD_OBJECT nvInitObj;

    DPF("nvpkrnl.cpp: NVPInitialize()\n");

    NVPDumpVPConnectInfo(dwVPConnectionFlags);      //Dump connection info

    // First time call into HAL: Reset NV and and create some objects
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        DPF ("NVPInitialize: reset NV\n");
        if (!nvEnable32 (pDDGbl)) {
            return NVP_STATUS_FAILURE;
        }
    }

    //If VPE is already initialized, uninitialize it first before continuing
    //pDriverData->nvpeState.bVPEInitialized = FALSE;
    if(pDriverData->nvpeState.bVPEInitialized){
        NVPUninitialize(pDriverData);
    }

    if (NVPEGetHandleToKernel(pDriverData, NULL) != NVP_STATUS_SUCCESS) {
        return NVP_STATUS_FAILURE;
    }

#ifdef NV3
    NvChannel *pVPChannel;

    // Use Pio channel for NV3
    if (NVP_VPE_CHANNEL_PTR == NULL)
    {
        // allocate timer object
        if (NvRmAllocChannelPio(
                NVP_CLIENT,     //pDriverData->dwRootHandle,
                NV_WIN_DEVICE,
                NVP_VPE_CHANNEL,
                NV03_CHANNEL_PIO,
                0,
                (PVOID)&(NVP_VPE_CHANNEL_PTR),
                ALLOC_CHL_PIO_FLAGS_FIFO_IGNORE_RUNOUT) != 0)
        {
            DPF("ERROR: NVPInitialize() failed to allocate VPE PIO channel !\n");
            return NVP_STATUS_FAILURE;
        }
    }
    nvInitObj.dwChipId          = pDriverData->NvDeviceVersion; //NVP_CHIP_VER_03;
#else
    nvInitObj.dwChipId          = pDriverData->NvDeviceVersion; //NVP_CHIP_VER_04;
#endif

    // fill up structure with initialization data
    nvInitObj.dwSize            = sizeof(NVP_CMD_OBJECT);
    nvInitObj.dwSrvId           = NVP_SRV_INITIALIZE;
    nvInitObj.hClient           = NVP_CLIENT;
    nvInitObj.pChannel          = (U032) NVP_VPE_CHANNEL_PTR;   // pDriverData->NvDevFlatVPE
    nvInitObj.pOvrChannel       = (U032) NVP_OVERLAY_CHANNEL_PTR;
    nvInitObj.dwOvrSubChannel   = NVP_OVERLAY_SUBCHANNEL;       // DD_SPARE; // NV_DD_SPARE;
    //nvInitObj.dwOverlayObjectID = NV_DD_YUV422_VIDEO_FROM_MEMORY;
    //nvInitObj.dwScalerObjectID = NV_DD_VIDEO_SCALER;
    //nvInitObj.dwDVDObjectID = NV_DD_DVD_SUBPICTURE;

    nvInitObj.dwOverlayObjectID = NV_VIDEO_OVERLAY; //0xDD003F90;
    nvInitObj.dwScalerObjectID = NV_VIDEO_OVERLAY; //0xDD003FA0;
    nvInitObj.dwDVDObjectID = NV_VIDEO_DVD_SUBPICTURE;
    nvInitObj.dwDVDObjectCtxDma = NV_DD_PIO_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nvInitObj.pNvPioFlipOverlayNotifierFlat = (U032)pDriverData->pPioFlipOverlayNotifierFlat;

    nvInitObj.dwPortId      = 0;    // use 0 for now !
    nvInitObj.dwDeviceId    = NV_WIN_DEVICE;
    nvInitObj.pFrameBuffer  = (U032) NVP_FB_BASE_ADDRESS;
    nvInitObj.dwFBLen       = NVP_FB_LENGTH;
    nvInitObj.dwVPTypeFlags = dwVPConnectionFlags;

    nvInitObj.MultiMon        = 0;
#if IS_WINNT5
    if (ppdev->TwinView_State == NVTWINVIEW_STATE_CLONE) {
        nvInitObj.MultiMon = 1;
    }
#elif IS_WIN9X
    if (pDriverData->dwDesktopState == NVTWINVIEW_STATE_DUALVIEW ||
        pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE) {
        nvInitObj.MultiMon = 1;
    }
#endif
    nvInitObj.regOverlayMode  = pDriverData->vpp.regOverlayMode;
    nvInitObj.regOverlayMode2 = pDriverData->vpp.regOverlayMode2;
    nvInitObj.regOverlayMode3 = pDriverData->vpp.regOverlayMode3;

    NVPCreateEvents(pDriverData, &nvInitObj);

/*
    dwVal = (DWORD)('e' | ('p' << 8) | ('v' << 16) | ('n' << 24));
    pVppDispatch = (PNVP_VPP_DISPATCH) EngAllocMem(FL_ZERO_MEMORY, sizeof(NVP_VPP_DISPATCH), dwVal);
    nvInitObj.pVppDispatch = (ULONG)pVppDispatch;

    nvCreateThread(&hThreadHandle, ThreadProc);
*/
    // call vpe kernel to initialize media port
    if (NVPCallKernel(NVP_DRIVER_HANDLE, &nvInitObj, NULL) == NVP_STATUS_SUCCESS)
        pDriverData->nvpeState.bVPEInitialized = TRUE;

    return NVP_STATUS_SUCCESS;
}

/*
==============================================================================

    NVPUninitialize

    Description:    clean up

==============================================================================
*/
NVP_STATUS NVPUninitialize(GLOBALDATA *pDriverData)
{
    NVP_CMD_OBJECT nvInitObj;

    DPF("nvpkrnl.cpp: NVPUninitialize()\n");

   // already un-initialized !
    if (pDriverData->nvpeState.bVPEInitialized == FALSE)
    {
        DPF("NVPUninitialize: already unintialized !\n");
        return NVP_STATUS_SUCCESS;
    }

#ifdef VPEFSMIRROR
    if (pDriverData->nvpeState.bFsMirrorEnabled) {
        NVPFsMirrorDisable(pDriverData);
    }
#endif

    // fill up structure with data needed for uninitialization
    nvInitObj.dwSize = sizeof(NVP_CMD_OBJECT);
    nvInitObj.dwSrvId = NVP_SRV_UNINITIALIZE;
    nvInitObj.hClient = NVP_CLIENT;
    nvInitObj.pChannel = (U032) NVP_VPE_CHANNEL_PTR;    // pDriverData->NvDevFlatVPE
    nvInitObj.dwPortId = 0; // use 0 for now !
    nvInitObj.dwDeviceId = NV_WIN_DEVICE;

    // call vpe kernel to uninitialize media port
    NVPCallKernel(NVP_DRIVER_HANDLE, &nvInitObj, NULL);

    // free VPE pio channel
    if (NVP_VPE_CHANNEL_PTR)
    {
        NvRmFree(NVP_CLIENT, NV_WIN_DEVICE, NVP_VPE_CHANNEL);
        NVP_VPE_CHANNEL_PTR = 0;
    }

    NVPDestroyEvents(pDriverData);

#ifndef WINNT
    CloseHandle(NVP_DRIVER_HANDLE);
    NVP_DRIVER_HANDLE = 0;
#endif


    // reset flag
    pDriverData->nvpeState.bVPEInitialized = FALSE;

/*
    if (pVppDispatch)
    {
        EngFreeMem(pVppDispatch);
        pVppDispatch = NULL;
    }

    if (hThreadHandle)
    {
        nvTerminateThread();
        hThreadHandle = NULL;
    }
*/
    return NVP_STATUS_SUCCESS;
}

/*
==============================================================================

    NVPStartVideo

    Description:    tells the VP to start grabbing

==============================================================================
*/
NVP_STATUS NVPStartVideo(
    LPDDHAL_UPDATEVPORTDATA lpInput)
{
    GLOBALDATA *pDriverData;
    NVP_CMD_OBJECT nvCmdObj;
    DWORD dwNumVideoSurfaces, dwNumVBISurfaces;
    DWORD i;

    DPF("nvpkrnl.cpp: NVPStartVideo()\n");

    nvSetDriverDataPtrFromDDGbl (lpInput->lpDD->lpGbl);

    // fill up structure with data needed for uninitialization
    nvCmdObj.dwSize = sizeof(NVP_CMD_OBJECT);
    nvCmdObj.hClient = NVP_CLIENT;
    nvCmdObj.pChannel = (U032) NVP_VPE_CHANNEL_PTR; // pDriverData->NvDevFlatVPE
    nvCmdObj.dwPortId = 0;  // use 0 for now !
    nvCmdObj.dwDeviceId = NV_WIN_DEVICE;

    // keep a copy of the VP Info DDraw flags
    nvCmdObj.dwVPInfoFlags = lpInput->lpVideoInfo->dwVPFlags;

    // determine image dimensions
    NVPGetImageDim(&(lpInput->lpVideoPort->ddvpDesc), lpInput->lpVideoInfo, &nvCmdObj, (lpInput->lplpDDVBISurface == NULL));

    nvCmdObj.bSurfVBI = FALSE;
    nvCmdObj.bSurfUpdate = FALSE;

    dwNumVideoSurfaces = lpInput->dwNumAutoflip;
    dwNumVBISurfaces = lpInput->dwNumVBIAutoflip;

    // in case of manual flip, register one surface for now... the others will be passed to us in FlipVideoPort
    if (!(lpInput->lpVideoInfo->dwVPFlags & DDVP_AUTOFLIP))
    {
        dwNumVideoSurfaces = (lpInput->lplpDDSurface) ? 1 : 0;
        dwNumVBISurfaces = (lpInput->lplpDDVBISurface) ? 1 : 0;
    }

    // register Video surfaces
    for (i = 0; i < dwNumVideoSurfaces; i++)
    {
        NVPRegSurface(pDriverData, lpInput->lplpDDSurface[i], &nvCmdObj, i);
    }

    // any VBI surfaces ?!
    nvCmdObj.bSurfVBI = (dwNumVBISurfaces != 0);
    for (i = 0; i < dwNumVBISurfaces; i++)
    {
        NVPRegSurface(pDriverData, lpInput->lplpDDVBISurface[i], &nvCmdObj, i);
    }

    // Need better explanation of this. @mjl@

    // channel pointer might have been updated or initialized later than expected
    nvCmdObj.pOvrChannel = (U032) NVP_OVERLAY_CHANNEL_PTR;
    // extra overlay offset (used for prescaling on NV4 and NV5 !)
    for (i = 0; i < pDriverData->vpp.extraNumSurfaces; i++)
    {
        nvCmdObj.aExtra422OverlayOffset[i] = pDriverData->vpp.extraOverlayOffset[i];
    }
    nvCmdObj.dwExtra422NumSurfaces = pDriverData->vpp.extraNumSurfaces;
    nvCmdObj.dwExtra422Index = pDriverData->vpp.extraIndex;
    nvCmdObj.dwExtra422Pitch = pDriverData->vpp.extraPitch;

    // call Miniport to start VP
    nvCmdObj.dwSrvId = NVP_SRV_START_VIDEO;

    // before starting media port, wait for overlay engine to complete operation 35msec x 2 fields ... this situation might occur during a mode switch
    NVPWaitForSync(pDriverData, NVP_EVENT_OVERLAY1, 35);
    NVPWaitForSync(pDriverData, NVP_EVENT_OVERLAY2, 35);

    // call vpe kernel to start media port
    NVPCallKernel(NVP_DRIVER_HANDLE, &nvCmdObj, NULL);

    NvReleaseSemaphore(pDriverData);

    NVPDumpVPSurfInfo(lpInput, &nvCmdObj);

    return NVP_STATUS_SUCCESS;
}

/*
==============================================================================

    NVPUpdateVideo

    Description:    updates VP settings

==============================================================================
*/
NVP_STATUS NVPUpdateVideo(
    LPDDHAL_UPDATEVPORTDATA lpInput)
{
    GLOBALDATA *pDriverData;
    NVP_CMD_OBJECT nvCmdObj;
    DWORD dwNumVideoSurfaces, dwNumVBISurfaces;
    DWORD i;

    DPF("nvpkrnl.cpp: NVPUpdateVideo()\n");

    nvSetDriverDataPtrFromDDGbl (lpInput->lpDD->lpGbl);

    // stop video first !
//    NVPStopVideo(pDriverData);

    // fill up structure with data needed for uninitialization
    nvCmdObj.dwSize = sizeof(NVP_CMD_OBJECT);
    nvCmdObj.hClient = NVP_CLIENT;
    nvCmdObj.pChannel = (U032) NVP_VPE_CHANNEL_PTR; // pDriverData->NvDevFlatVPE
    nvCmdObj.dwPortId = 0;  // use 0 for now !
    nvCmdObj.dwDeviceId = NV_WIN_DEVICE;

    // keep a copy of the VP Info DDraw flags
    nvCmdObj.dwVPInfoFlags = lpInput->lpVideoInfo->dwVPFlags;

    // determine image dimensions
    NVPGetImageDim(&(lpInput->lpVideoPort->ddvpDesc), lpInput->lpVideoInfo, &nvCmdObj, (lpInput->lplpDDVBISurface == NULL));

    nvCmdObj.bSurfVBI = FALSE;
    nvCmdObj.bSurfUpdate = TRUE;

    dwNumVideoSurfaces = lpInput->dwNumAutoflip;
    dwNumVBISurfaces = lpInput->dwNumVBIAutoflip;

    // in case of manual flip, register one surface for now... the others will be passed to us in FlipVideoPort
    if (!(lpInput->lpVideoInfo->dwVPFlags & DDVP_AUTOFLIP))
    {
        dwNumVideoSurfaces = (lpInput->lplpDDSurface) ? 1 : 0;
        dwNumVBISurfaces = (lpInput->lplpDDVBISurface) ? 1 : 0;
    }

    // register Video surfaces
    for (i = 0; i < dwNumVideoSurfaces; i++)
    {
        NVPRegSurface(pDriverData, lpInput->lplpDDSurface[i], &nvCmdObj, i);
    }

    // any VBI surfaces ?!
    nvCmdObj.bSurfVBI = (dwNumVBISurfaces != 0);
    for (i = 0; i < dwNumVBISurfaces; i++)
    {
        NVPRegSurface(pDriverData, lpInput->lplpDDVBISurface[i], &nvCmdObj, i);
    }

    // Need better explanation of this. @mjl@

    // channel pointer might have been updated or initialized later than expected
    nvCmdObj.pOvrChannel = (U032) NVP_OVERLAY_CHANNEL_PTR;
    // extra overlay offset (used for prescaling on NV4 and NV5 !)
    for (i = 0; i < pDriverData->vpp.extraNumSurfaces; i++)
    {
        nvCmdObj.aExtra422OverlayOffset[i] = pDriverData->vpp.extraOverlayOffset[i];
    }
    nvCmdObj.dwExtra422NumSurfaces = pDriverData->vpp.extraNumSurfaces;
    nvCmdObj.dwExtra422Index = pDriverData->vpp.extraIndex;
    nvCmdObj.dwExtra422Pitch = pDriverData->vpp.extraPitch;

    // call Miniport to update VP
    nvCmdObj.dwSrvId = NVP_SRV_UPDATE_VIDEO;

    // call vpe kernel to update video port data/state
    NVPCallKernel(NVP_DRIVER_HANDLE, &nvCmdObj, NULL);

    NvReleaseSemaphore(pDriverData);

    NVPDumpVPSurfInfo(lpInput, &nvCmdObj);

    return NVP_STATUS_SUCCESS;
}

/*
==============================================================================

    NVPStopVideo

    Description:    tells the VP to stop grabbing video

==============================================================================
*/
NVP_STATUS NVPStopVideo (GLOBALDATA *pDriverData, LPDDRAWI_DIRECTDRAW_GBL pDDGbl)
{
    NVP_CMD_OBJECT nvCmdObj;

    DPF("nvpkrnl.cpp: NVPStopVideo()\n");

    if(!pDriverData->nvpeState.bVPEInitialized){
        return  NVP_STATUS_SUCCESS;
    }

    // case where stopping VPE right after a mode switch... Reset NV and and create some objects
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED)
    {
        DPF ("NVPStopVideo: reset NV !\n");
        if (!nvEnable32 (pDDGbl)) {
            return NVP_STATUS_FAILURE;   
        }
        //// this flag must be set here after a mode switch because a new pDriverData got created and the old
        // flag/state is lost ! (this flag might be reset in NVPUninitialize() later)
        pDriverData->nvpeState.bVPEInitialized = TRUE;
    }

    // fill up structure with data needed for uninitialization
    nvCmdObj.dwSize = sizeof(NVP_CMD_OBJECT);
    nvCmdObj.dwSrvId = NVP_SRV_STOP_VIDEO;
    nvCmdObj.hClient = NVP_CLIENT;
    nvCmdObj.pChannel = (U032) NVP_VPE_CHANNEL_PTR; // pDriverData->NvDevFlatVPE
    nvCmdObj.dwPortId = 0;  // use 0 for now !
    nvCmdObj.dwDeviceId = NV_WIN_DEVICE;

    // call vpe kernel to stop media port
    NVPCallKernel(NVP_DRIVER_HANDLE, &nvCmdObj, NULL);

    // wait for overlay engine to complete operation 35msec x 2 fields
    NVPWaitForSync(pDriverData, NVP_EVENT_OVERLAY1, 35);
    NVPWaitForSync(pDriverData, NVP_EVENT_OVERLAY2, 35);

    DPF("NVPStopVideo: end function !\n");

    return NVP_STATUS_SUCCESS;
}

/*
==============================================================================

    NVPGetImageDim

    Description:    determines the dimension of captured video field (crop,
                    vbi height...)

==============================================================================
*/
NVP_STATUS NVPGetImageDim(
    LPDDVIDEOPORTDESC pVPDesc,
    LPDDVIDEOPORTINFO pVideoInfo,
    PNVP_CMD_OBJECT pnvCmdObj,
    BOOL bIsVideoOnlySurface)
{
    if (bIsVideoOnlySurface)
    {
        // this indicates that there are NO VBI surfaces associated with this video port update.
        // therefore we should send all the line to the Image surface
        pnvCmdObj->dwImageStartLine = 0;    // send all the data after line 0

        if (pVideoInfo->dwVPFlags & DDVP_CROP)
        {
            if(pVideoInfo->rCrop.top > (signed long) pnvCmdObj->dwImageStartLine)
                pnvCmdObj->dwImageStartLine = pVideoInfo->rCrop.top;
        }

    }
    else
    {
#define NVP_VBIVALIDDATALINE    1
        // there are some VBI surfaces associated with this video port,
        // so update VBI height and start line accordingly
        pnvCmdObj->dwVBIStartLine = NVP_VBIVALIDDATALINE;

        pnvCmdObj->dwVBIHeight = pVideoInfo->dwVBIHeight;

        pnvCmdObj->dwImageStartLine = 1 + pnvCmdObj->dwVBIHeight + pnvCmdObj->dwVBIStartLine;

        // Image start line should be one line after the vbi end

        if (pVideoInfo->dwVPFlags & DDVP_CROP)
        {
            if (pVideoInfo->rCrop.top > (signed long) pnvCmdObj->dwImageStartLine)
                pnvCmdObj->dwImageStartLine = pVideoInfo->rCrop.top;
        }
    }

    // check for cropping
    if (pVideoInfo->dwVPFlags & DDVP_CROP)
    {
        pnvCmdObj->dwInHeight = (DWORD) (pVideoInfo->rCrop.bottom - pVideoInfo->rCrop.top);
        pnvCmdObj->dwInWidth  = (DWORD) (pVideoInfo->rCrop.right - pVideoInfo->rCrop.left);
    }
    else
    {
        pnvCmdObj->dwInHeight = pVPDesc->dwFieldHeight;
        pnvCmdObj->dwInWidth  = pVPDesc->dwFieldWidth;
    }

    // in the case where DDVP_IGNOREVBIXCROP flag is set we must substract the VBIHeight from InHeight
    if (pVideoInfo->dwVPFlags & DDVP_IGNOREVBIXCROP)
	    pnvCmdObj->dwInHeight -= (pVideoInfo->dwVBIHeight+1);	// -1=empirical data shows this is needed
																// to normalize to vid size

    // check prescaling
    if (pVideoInfo->dwVPFlags & DDVP_PRESCALE)
    {
        pnvCmdObj->dwPreHeight = pVideoInfo->dwPrescaleHeight;
        pnvCmdObj->dwPreWidth  = pVideoInfo->dwPrescaleWidth;

    }
    else
    {
        pnvCmdObj->dwPreHeight = pnvCmdObj->dwInHeight;
        pnvCmdObj->dwPreWidth = pnvCmdObj->dwInWidth;
    }

    pnvCmdObj->dwOriginX = pVideoInfo->dwOriginX;
    pnvCmdObj->dwOriginY = pVideoInfo->dwOriginY;

    return NVP_STATUS_SUCCESS;
}

/*
==============================================================================

    NVPRegSurface

    Description:    registers a new surface in miniport

==============================================================================
*/
NVP_STATUS NVPRegSurface(
    GLOBALDATA *pDriverData,
    PNVP_DDRAW_SURFACE pDDSurface,  // lpInput->lplpDDSurface,
    PNVP_CMD_OBJECT pnvCmdObj,
    DWORD dwSurfaceId)
{
    DWORD dwVal;

    // surface memory pointer and pitch
    pnvCmdObj->pSurfVidMem = NVP_GET_SURFACE_POINTER(pDDSurface->lpLcl->lpGbl->fpVidMem);
    pnvCmdObj->dwSurfPitch = pDDSurface->lpLcl->lpGbl->lPitch;

    // compute the actual offset
    dwVal = (pnvCmdObj->dwOriginX)*2 + (pnvCmdObj->dwOriginY)*(pnvCmdObj->dwSurfPitch);
    pnvCmdObj->dwSurfOffset = NVP_GET_SURFACE_OFFSET(pDDSurface->lpLcl->lpGbl->fpVidMem) + dwVal;

    pnvCmdObj->dwSrvId = NVP_SRV_REG_SURFACE;
    pnvCmdObj->dwSurfaceId = dwSurfaceId;

    // call vpe kernel to register surface
    NVPCallKernel(NVP_DRIVER_HANDLE, pnvCmdObj, NULL);

    return NVP_STATUS_SUCCESS;
}


/*
==============================================================================

    NVPUpdateOverlay

    Description:    update overlay data in miniport

==============================================================================
*/
DWORD __stdcall NVPUpdateOverlay(GLOBALDATA *pDriverData, LPDDRAWI_DIRECTDRAW_GBL pDDGbl)
{
    NVP_CMD_OBJECT nvCmdObj;
    DWORD          i;

    // this is a case where a VPE object wasn't created (ex: STB TV app) but KMVT service OverlayFlip() is called !
    if ((pDriverData->bRing0FlippingFlag) && !(pDriverData->nvpeState.bVPEInitialized))
    {
        if (NVPInitialize(pDriverData, 0, pDDGbl) != NVP_STATUS_SUCCESS)
            return 0;
    }

    if(!(pDriverData->nvpeState.bVPEInitialized)){
        return 0;
    }

    // copy overlay data from DriverInfo
    nvCmdObj.dwOverlayFormat       = pDriverData->vpp.dwOverlayFormat;
    nvCmdObj.dwOverlaySrcX         = pDriverData->vpp.dwOverlaySrcX;
    nvCmdObj.dwOverlaySrcY         = pDriverData->vpp.dwOverlaySrcY;
    nvCmdObj.dwOverlaySrcPitch     = pDriverData->vpp.dwOverlaySrcPitch;
    nvCmdObj.dwOverlaySrcSize      = pDriverData->vpp.dwOverlaySrcSize;
    nvCmdObj.dwOverlaySrcWidth     = pDriverData->vpp.dwOverlaySrcWidth;
    nvCmdObj.dwOverlaySrcHeight    = pDriverData->vpp.dwOverlaySrcHeight;
    nvCmdObj.dwOverlayDstWidth     = pDriverData->vpp.dwOverlayDstWidth;
    nvCmdObj.dwOverlayDstHeight    = pDriverData->vpp.dwOverlayDstHeight;
    nvCmdObj.dwOverlayDstX         = pDriverData->vpp.dwOverlayDstX;
    nvCmdObj.dwOverlayDstY         = pDriverData->vpp.dwOverlayDstY;
    nvCmdObj.dwOverlayDeltaX       = pDriverData->vpp.dwOverlayDeltaX;
    nvCmdObj.dwOverlayDeltaY       = pDriverData->vpp.dwOverlayDeltaY;
    nvCmdObj.dwOverlayColorKey     = pDriverData->vpp.dwOverlayColorKey;
    nvCmdObj.dwOverlayMode         = pDriverData->vpp.dwOverlayMode;
    nvCmdObj.dwOverlayMaxDownScale = pDriverData->vpp.dwOverlayMaxDownScale;

    // Need better explanation of this. @mjl@

    // extra overlay offset (used for prescaling on NV4 and NV5 !)
    for (i = 0; i < pDriverData->vpp.extraNumSurfaces; i++)
    {
        nvCmdObj.aExtra422OverlayOffset[i] = pDriverData->vpp.extraOverlayOffset[i];
    }
    nvCmdObj.dwExtra422NumSurfaces = pDriverData->vpp.extraNumSurfaces;
    nvCmdObj.dwExtra422Index = pDriverData->vpp.extraIndex;
    nvCmdObj.dwExtra422Pitch = pDriverData->vpp.extraPitch;

    nvCmdObj.dwSize = sizeof(NVP_CMD_OBJECT);
    nvCmdObj.dwSrvId = NVP_SRV_UPDATE_OVERLAY;
    nvCmdObj.dwPortId = 0;  // use 0 for now !

    // call vpe kernel to register this surface
    NVPCallKernel(NVP_DRIVER_HANDLE, &nvCmdObj, NULL);

    if ((pDriverData->vpp.dwOverlayDstWidth == 0) && (pDriverData->vpp.dwOverlayDstHeight == 0))
    {
        // wait for overlay engine to complete operation (35msec)
        NVPWaitForSync(pDriverData, NVP_EVENT_OVERLAY1, 35);
        NVPWaitForSync(pDriverData, NVP_EVENT_OVERLAY2, 35);
    }

    return 0;
}

/*
==============================================================================

    NVPWaitForSync

    Description:    waits on vpe events. They are set by video kernel when
                    notification is issued after an even or odd field is
                    displayed or when captured (or engine stopped)

==============================================================================
*/
NVP_STATUS NVPWaitForSync(
    GLOBALDATA *pDriverData,
    DWORD dwIndex, // event list index NVP_EVENT_MEDIAPORT, NVP_EVENT_OVERLAY1, NVP_EVENT_OVERLAY2
    DWORD dwTimeOut) // in msec
{
#ifdef WINNT
    LARGE_INTEGER llTimeOut;

    if (pDriverData->nvpeState.hNVPSyncEvent[dwIndex] == NULL)
        return NVP_STATUS_SUCCESS;

    if (dwTimeOut)
    {
        llTimeOut.LowPart = -((int)dwTimeOut) * 10000;   // measured in units of 100 ns in NT
        llTimeOut.HighPart = -1;                       // negative means relative to current time
    }
    else
    {
        llTimeOut.LowPart = 0;
        llTimeOut.HighPart = 0;
    }

    // note: DDK docs are wrong, it returns an error code of 0x102 if timed-out and 0 on success, not a TRUE/FALSE result
    return (NVP_STATUS)(EngWaitForSingleObject((PEVENT) (pDriverData->nvpeState.hNVPSyncEvent[dwIndex]), &llTimeOut));

#else   //WINNT
    NVP_STATUS dwStatus;

    if (pDriverData->nvpeState.hNVPSyncEvent[dwIndex] == NULL)
        return NVP_STATUS_SUCCESS;

    dwStatus = (NVP_STATUS)(WaitForSingleObject(pDriverData->nvpeState.hNVPSyncEvent[dwIndex], dwTimeOut));
    ResetEvent(pDriverData->nvpeState.hNVPSyncEvent[dwIndex]);

    return dwStatus;
#endif  //WINNT
}

/*
==============================================================================

    NVPCreateEvents

    Description:    creates VPE events. They are set by video kernel when
                    notification is issued after an even or odd field is
                    captured (at end of mediaport vsync !)

==============================================================================
*/
NVP_STATUS NVPCreateEvents(
    GLOBALDATA *pDriverData,
    PNVP_CMD_OBJECT pnvInitObj)
{
    int i;

    // media port and overlay synchronization event
    for (i = 0; i < NVP_EVENT_MAX; i++)
    {
#ifndef WINNT
        if (!(pDriverData->nvpeState.hNVPSyncEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL)))
#else
        if (!EngCreateEvent((PEVENT *)&(pDriverData->nvpeState.hNVPSyncEvent[i])))
#endif
        {

            DPF("ERROR: NVPCreateEvents() failed to create sync events !\n");
            return NVP_STATUS_FAILURE;
        }

#ifndef WINNT
        // under Win9x, convert win32 event to ring0 handle
        pDriverData->nvpeState.hNVPSyncEvent0[i] = ConvertRing3EventToRing0((HANDLE) (pDriverData->nvpeState.hNVPSyncEvent[i]));
        pnvInitObj->hNVPSyncEvent[i] = (U032)(pDriverData->nvpeState.hNVPSyncEvent0[i]);
#else
        // under Win2k, you must de-reference the event handle in order to be used by miniport KeSetEvent !
        pnvInitObj->hNVPSyncEvent[i] = (U032) (*((LPDWORD)(pDriverData->nvpeState.hNVPSyncEvent[i])));
#endif
    }

    return NVP_STATUS_SUCCESS;
}

/*
==============================================================================

    NVPDestroyEvents

    Description:    destroys VPE events

==============================================================================
*/
NVP_STATUS NVPDestroyEvents(GLOBALDATA *pDriverData)
{
    int i;

    // close media port and overlay synchronization event
    for (i = 0; i < NVP_EVENT_MAX; i++)
    {
#ifndef WINNT
        CloseHandle(pDriverData->nvpeState.hNVPSyncEvent[i]);
        CloseRing0Handle(pDriverData->nvpeState.hNVPSyncEvent0[i]);
        pDriverData->nvpeState.hNVPSyncEvent0[i] = NULL;

#else
        HDRVEVENT pevent;
        if(pevent = (PEVENT)pDriverData->nvpeState.hNVPSyncEvent[i])
        {
            EngDeleteEvent(pevent);
        }

#endif
        pDriverData->nvpeState.hNVPSyncEvent[i] = NULL;
    }

    return NVP_STATUS_SUCCESS;
}


/*
==============================================================================

    NVPFlipVideoPort

    Description:    asks kernel to start capturing now to the specified
                    target surface

==============================================================================
*/
NVP_STATUS NVPFlipVideoPort(
    LPDDHAL_FLIPVPORTDATA lpInput)
{
    GLOBALDATA *pDriverData;
    NVP_CMD_OBJECT nvCmdObj;
    DWORD dwVal;

    nvSetDriverDataPtrFromDDGbl (lpInput->lpDD->lpGbl);

    // determine image dimensions
    NVPGetImageDim(&(lpInput->lpVideoPort->ddvpDesc), &(lpInput->lpVideoPort->ddvpInfo), &nvCmdObj, TRUE);

    // surface memory pointer and pitch
    nvCmdObj.pSurfVidMem = NVP_GET_SURFACE_POINTER(lpInput->lpSurfTarg->lpGbl->fpVidMem);
    nvCmdObj.dwSurfPitch = lpInput->lpSurfTarg->lpGbl->lPitch;

    // compute the actual offset
    dwVal = (nvCmdObj.dwOriginX)*2 + (nvCmdObj.dwOriginY)*(nvCmdObj.dwSurfPitch);
    nvCmdObj.dwSurfOffset = NVP_GET_SURFACE_OFFSET(lpInput->lpSurfTarg->lpGbl->fpVidMem) + dwVal;

    nvCmdObj.dwSize = sizeof(NVP_CMD_OBJECT);
    nvCmdObj.dwPortId = 0;  // use 0 for now !
    nvCmdObj.dwSrvId = NVP_SRV_FLIP_VIDEOPORT;
    nvCmdObj.dwSurfaceId = NVP_UNDEF; // this value tells the kernel to lookup the surface Id in its registered surfaces list

    // flip to the target surface
    NVPCallKernel(NVP_DRIVER_HANDLE, &nvCmdObj, NULL);

    NvReleaseSemaphore(pDriverData);

    return NVP_STATUS_SUCCESS;
}

/*
==============================================================================

    NVPCallKernel

    Description:    OS dependent functions used to call miniport (Win2k) or
                    minivdd (Win9x)

==============================================================================
*/
NVP_STATUS NVPCallKernel(
    HANDLE hDevice,
    PNVP_CMD_OBJECT pnvInCmdObj,
    PNVP_CMD_OBJECT pnvOutCmdObj)
{
    DWORD   dwCBRet, dwSizeOut = 0;

    if (pnvOutCmdObj != NULL)
       dwSizeOut = sizeof(NVP_CMD_OBJECT);

#ifndef WINNT

    if (DeviceIoControl(
            hDevice,
            IOCTL_VIDEO_VPE_COMMAND,
            pnvInCmdObj,
            sizeof(NVP_CMD_OBJECT),
            pnvOutCmdObj,
            dwSizeOut,
            &dwCBRet,
            NULL))
        return NVP_STATUS_SUCCESS;  //Win9x returns non-zero for success

#else

    if (!EngDeviceIoControl(
            hDevice,          //pDriverData->ppdev->hDriver
            IOCTL_VIDEO_VPE_COMMAND,
            pnvInCmdObj,
            sizeof(NVP_CMD_OBJECT),
            pnvOutCmdObj,
            dwSizeOut,
            &dwCBRet))
        return NVP_STATUS_SUCCESS;  //Win2k returns zero for success

#endif // !WINNT

    return NVP_STATUS_FAILURE;
}

/*
==============================================================================

    NVPCleanUp

    Description:    basically, calls NVPUninitialize() and clears the ring0
                    flip flag set by kernel services

    Note:           this function is called from DestroySurface32() which
                    is NOT the "normal" path for uninitialization.
                    DestroyVideoPort32/NVPUninitialize is !

==============================================================================
*/
DWORD __stdcall NVPCleanUp(GLOBALDATA *pDriverData, LPDDRAWI_DIRECTDRAW_GBL pDDGbl)
{
#ifdef VPEFSMIRROR
    if (pDriverData->nvpeState.bVPEInitialized && pDriverData->nvpeState.bFsMirrorEnabled) {
        NVPFsMirrorDisable(pDriverData);
    }
#endif

//need to understand why the ifdef's are here to diff OSes.
#ifndef WINNT
    if ((pDriverData->bRing0FlippingFlag) && (pDriverData->nvpeState.bVPEInitialized)){
        //Dst width & height =0 will shutdown overlay
        //changing size values are not recommended but since these values
        //are updated when you call updateoverlay, I guess this is fine.
        //and we are destroying VP anyway...
        pDriverData->vpp.dwOverlayDstWidth = 0;
        pDriverData->vpp.dwOverlayDstHeight = 0;
        NVPUpdateOverlay(pDriverData, pDDGbl);
        NVPStopVideo(pDriverData, pDDGbl);
        NVPUninitialize(pDriverData);
    }

    pDriverData->bRing0FlippingFlag = 0;
#else
    if (pDriverData->nvpeState.bVPEInitialized){
        //Dst width & height =0 will shutdown overlay
        //changing size values are not recommended but since these values
        //are updated when you call updateoverlay, I guess this is fine.
        //and we are destroying VP anyway...
        pDriverData->vpp.dwOverlayDstWidth = 0;
        pDriverData->vpp.dwOverlayDstHeight = 0;
        NVPUpdateOverlay(pDriverData, pDDGbl);

		//bRing0FlippingFlag is only set when r0 lock is called (busmaster cards)
		//don't need to destroy VP otherwise (VP using non-over surf)
		if(pDriverData->bRing0FlippingFlag){
			NVPStopVideo(pDriverData, pDDGbl);
			NVPUninitialize(pDriverData);
		}
    }
#endif

    return 0;
}


NVP_STATUS NVPDumpVPSurfInfo(LPDDHAL_UPDATEVPORTDATA lpInput, NVP_CMD_OBJECT *pnvCmdObj)
{

    int i;

    DPF("nvpkrnl.cpp: # Video surf          =0x%x", lpInput->dwNumAutoflip);
    if(lpInput->dwNumAutoflip > 0){
        for(i=0; i < (int)(lpInput->dwNumAutoflip); ++i){
            DPF("nvpkrnl.cpp:   pVidMem (%d)        =0x%x", i,
                NVP_GET_SURFACE_POINTER(((lpInput->lplpDDSurface)[i])->lpLcl->lpGbl->fpVidMem));
        }
        DPF("nvpkrnl.cpp:   Pitch               =0x%x (%d)",
                        (*(lpInput->lplpDDSurface))->lpLcl->lpGbl->lPitch,
                        (*(lpInput->lplpDDSurface))->lpLcl->lpGbl->lPitch);
        DPF("nvpkrnl.cpp:   Width               =0x%x (%d)", pnvCmdObj->dwInWidth, pnvCmdObj->dwInWidth);
        DPF("nvpkrnl.cpp:   Height              =0x%x (%d)", pnvCmdObj->dwInHeight, pnvCmdObj->dwInHeight);
        DPF("nvpkrnl.cpp:   PrescaleSizeX       =0x%x (%d)", pnvCmdObj->dwPreWidth, pnvCmdObj->dwPreWidth);
        DPF("nvpkrnl.cpp:   PrescaleSizeY       =0x%x (%d)", pnvCmdObj->dwPreHeight, pnvCmdObj->dwPreHeight);
        DPF("nvpkrnl.cpp:   Video Start Line    =0x%x (%d)", pnvCmdObj->dwImageStartLine, pnvCmdObj->dwImageStartLine);

        // print some stuff out about the flags we're lookin at:
        /*if( pMySurfaces->bInvertedFields ) {
            DPF("      field polarity               inverted");
        } else {
            DPF("      field polarity               normal");
        }*/
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_AUTOFLIP  ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_AUTOFLIP            on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_AUTOFLIP            off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_CONVERT       ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_CONVERT            on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_CONVERT            off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_CROP                  ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_CROP               on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_CROP               off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_INTERLEAVE            ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_INTERLEAVE         on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_INTERLEAVE         off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_MIRRORLEFTRIGHT       ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_MIRRORLEFTRIGHT    on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_MIRRORLEFTRIGHT    off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_MIRRORUPDOWN          ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_MIRRORUPDOWN       on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_MIRRORUPDOWN       off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_PRESCALE              ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_PRESCALE           on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_PRESCALE           off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_SKIPEVENFIELDS        ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_SKIPEVENFIELDS     on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_SKIPEVENFIELDS     off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_SKIPODDFIELDS         ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_SKIPODDFIELDS      on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_SKIPODDFIELDS      off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_SYNCMASTER                ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_SYNCMASTER         on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_SYNCMASTER         off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_VBICONVERT                ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_VBICONVERT         on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_VBICONVERT         off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_VBINOSCALE                ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_VBINOSCALE         on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_VBINOSCALE         off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_OVERRIDEBOBWEAVE      ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_OVERRIDEBOBWEAVE   on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_OVERRIDEBOBWEAVE   off");
        }
        if(lpInput->lpVideoInfo->dwVPFlags & DDVP_IGNOREVBIXCROP        ) {
            DPF("nvpkrnl.cpp:   FLAG DDVP_IGNOREVBIXCROP     on");
        } else {
            DPF("nvpkrnl.cpp:   FLAG DDVP_IGNOREVBIXCROP     off");
        }





    }

    DPF("nvpkrnl.cpp: # Vbi surf            =0x%x", lpInput->dwNumVBIAutoflip);
    if(lpInput->dwNumVBIAutoflip){
        for(i=0; i < (int)(lpInput->dwNumVBIAutoflip); ++i){
            DPF("nvpkrnl.cpp:   pVbiMem (%d)        =0x%x", i,
                NVP_GET_SURFACE_POINTER(((lpInput->lplpDDVBISurface)[i])->lpLcl->lpGbl->fpVidMem));
        }
        DPF("nvpkrnl.cpp:   Vbi Start Line      =0x%x (%d)", pnvCmdObj->dwVBIStartLine, pnvCmdObj->dwVBIStartLine);
        DPF("nvpkrnl.cpp:   Vbi Height          =0x%x (%d)", pnvCmdObj->dwVBIHeight, pnvCmdObj->dwVBIHeight);
        DPF("nvpkrnl.cpp:   Vbi Pitch           =0x%x (%d)",
                        (*(lpInput->lplpDDVBISurface))->lpLcl->lpGbl->lPitch,
                        (*(lpInput->lplpDDVBISurface))->lpLcl->lpGbl->lPitch);
    }

    return NVP_STATUS_SUCCESS;

}

NVP_STATUS NVPDumpVPConnectInfo(DWORD dwVPConnectionFlags)
{
    //we currently do not use these
    /*
    // this is the second create
    DPF("  Size of the DDVIDEOPORTDESC structure     %d",lpInput->lpDDVideoPortDesc->dwSize);
    DPF("  Width of the video port field.            %d",lpInput->lpDDVideoPortDesc->dwFieldWidth);
    DPF("  Width of the VBI data.                    %d",lpInput->lpDDVideoPortDesc->dwVBIWidth);
    DPF("  Height of the video port field.           %d",lpInput->lpDDVideoPortDesc->dwFieldHeight);
    DPF("  Microseconds per video field.             %d",lpInput->lpDDVideoPortDesc->dwMicrosecondsPerField);
    DPF("  Maximum pixel rate per second.            %d",lpInput->lpDDVideoPortDesc->dwMaxPixelsPerSecond);
    DPF("  Video port ID (0 - (dwMaxVideoPorts -1)). %d",lpInput->lpDDVideoPortDesc->dwVideoPortID);
    DPF("  Reserved for future use - set to zero.    %d",lpInput->lpDDVideoPortDesc->dwReserved1);
    DPF("  Size of the DDVIDEOPORTDESC.dwSize        %d",lpInput->lpDDVideoPortDesc->VideoPortType.dwSize);
    DPF("              DDVIDEOPORTDESC.dwPortWidth   %d",lpInput->lpDDVideoPortDesc->VideoPortType.dwPortWidth);
#ifndef __cplusplus
    DPF("              DDVIDEOPORTDESC.guidTypeID    %d",lpInput->lpDDVideoPortDesc->VideoPortType.guidTypeID);
#endif
    DPF("              DDVIDEOPORTDESC.dwFlags       %d",lpInput->lpDDVideoPortDesc->VideoPortType.dwFlags);
    */
    DPF("nvpkrnl.cpp:VP Connection Info");
    if(dwVPConnectionFlags & DDVPCONNECT_INTERLACED ) {
        DPF("nvpkrnl.cpp:   DDVPCONNECT_INTERLACED          on");
    }else{
        DPF("nvpkrnl.cpp:   DDVPCONNECT_INTERLACED          off");
    }
    if(dwVPConnectionFlags & DDVPCONNECT_INVERTPOLARITY  ) {
        DPF("nvpkrnl.cpp:   DDVPCONNECT_INVERTPOLARITY      on");
    }else{
        DPF("nvpkrnl.cpp:   DDVPCONNECT_INVERTPOLARITY      off");
    }
    if(dwVPConnectionFlags & DDVPCONNECT_HALFLINE  ) {
        DPF("nvpkrnl.cpp:   DDVPCONNECT_HALFLINE            on");
    }else{
        DPF("nvpkrnl.cpp:   DDVPCONNECT_HALFLINE            off");
    }
    if(dwVPConnectionFlags & DDVPCONNECT_VACT ) {
        DPF("nvpkrnl.cpp:   DDVPCONNECT_VACT                on  -ERROR!!!!");
    }else{
        DPF("nvpkrnl.cpp:   DDVPCONNECT_VACT                off");
    }
    if(dwVPConnectionFlags & DDVPCONNECT_DOUBLECLOCK ) {
        DPF("nvpkrnl.cpp:   DDVPCONNECT_DOUBLECLOCK         on  -ERROR!!!!");
    }else{
        DPF("nvpkrnl.cpp:   DDVPCONNECT_DOUBLECLOCK         off");
    }


    return NVP_STATUS_SUCCESS;

}
//

void __stdcall NVPInitialState(
    GLOBALDATA *pDriverData)
{
    int i;

    pDriverData->nvpeState.bVPEInitialized = FALSE;
    pDriverData->nvpeState.bFsMirrorEnabled = FALSE;
    pDriverData->nvpeState.dwVPEState = 0;
    pDriverData->nvpeState.hVPEKernel = 0;
    for (i=0; i<NVPE_EVENT_MAX; i++) {
        pDriverData->nvpeState.hNVPSyncEvent[i] = 0;
        pDriverData->nvpeState.hNVPSyncEvent0[i] = 0;
    }

}


#endif // NVPE
