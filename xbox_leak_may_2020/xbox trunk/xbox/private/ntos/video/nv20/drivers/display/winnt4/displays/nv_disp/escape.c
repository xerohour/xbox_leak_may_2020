//***************************** Module Header **********************************
//
// Module Name: escape.c
//
// Escape handler for drivers and other escapes.
//
// Copyright (c) 1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"

#include "excpt.h"
#include "nv32.h"
#include "nvos.h"
#include "nvntioctl.h"
//#include "nvntddctl.h"
//#include "nvosnt.h"
#include "nvsubch.h"
#include "nvapi.h"
#include "nvReg.h"
#if (NVARCH >= 0x4)
#if _WIN32_WINNT >= 0x0500
#include "nvPriv.h"
#endif
#endif

#include "wksappsupport.h"

#include "nv32.h"
#include "dmamacro.h"

#include "nvsubch.h"
#include "nvalpha.h"
#include "nvapi.h"

#include "oglDD.h"
#include "oglOverlay.h"
#include "oglsync.h"
#ifdef NV_MAIN_MULTIMON
#include "oglutils.h"
#endif
#include "pixelfmt.h"

// To get the NV11 twinview definitions
#include "escape.h"
#include "nvcm.h"
extern BOOLEAN   TwinViewDataOK(PPDEV ppdev, NVTWINVIEWDATA *pData);
extern void PrintTwinViewData (char *Msg, ULONG ulDebugLevel, NVTWINVIEWDATA *pData, PPDEV ppdev);
extern BOOLEAN bGetPhysicalVirtualModeList(PPDEV ppdev, ULONG Head, ULONG DeviceMask, ULONG DeviceType, ULONG TVFormat,
        MODE_ENTRY *PhysModeList, MODE_ENTRY *VirtModeList, ULONG *NumPhysModes, ULONG *NumVirtModes);
extern void InduceAndUpdateDeviceScanning (PPDEV ppdev);

extern void   ProcessSynchronousFlags(PPDEV ppdev, NVTWINVIEWDATA *pData);
extern VOID WriteRegistryTwinViewInfo (PPDEV ppdev, NVTWINVIEWDATA *pData);

extern VOID SetInduceRegValue(PPDEV ppdev, ULONG ulVal);
extern NVTWINVIEWDATA * pFindSaveSettings(PPDEV ppdev, QUERY_SAVE_SETTINGS * pQuery, NVTWINVIEWDATA * pTwinViewData);
extern VOID vWriteSaveSettings(PPDEV ppdev);
BOOLEAN bHandleNT4HotKey(PPDEV ppdev, GET_DEVICE_MASK_DATA_TYPE *pInfo);

#if 0 // defined in common\win9x\inc\escape.h
// - Added escapes for output device set/get
#define NV_ESC_SET_DISPLAY_DEVICE_TYPE  0x6E84
#define NV_ESC_GET_DISPLAY_DEVICE_TYPE  0x6E85
// - Added to provide the path to the driver reg area (hard coded under winnt4)
#define NV_ESC_GET_LOCAL_REGISTRY_PATH      0x6E7F
// - Added to provide id of the board and type
#define NV_ESC_IS_NVIDIA_BOARD_AND_DRIVER   0x6E80
// - Added escape to validate a mode for a device
#define NV_ESC_VALIDATE_DEVICE_MODE         0x6E83
// - From nvVer.h, the escape value for driver verification
#endif
#define NV_ESCAPE_ID                'nv'
#define NV_ESCAPE_ID_RETURN         0x3031564E  // "NV10"


#if 0
//
// Since we are including escape.h now, we don't need to define the structure here. Otherwise we get a redefinition error.
//

// - Data struct for NV_ESC_SET_DISPLAY_DEVICE_TYPE / NV_ESC_GET_DISPLAY_DEVICE_TYPE
typedef struct  tagSETGETDEVTYPE
{
    unsigned long   dwSuccessFlag;
    unsigned long   dwDeviceType;
    unsigned long   dwTvFormat;
} SETGETDEVTYPE, *PSETGETDEVTYPE;
#else
typedef SETGETDEVTYPE *PSETGETDEVTYPE;
#endif // 0



// - Data struct for NV_ESC_VALIDATE_DEVICE_MODE
typedef struct  tagVALMODEEXTR
{
    unsigned long   dwHRes;
    unsigned long   dwVRes;
    unsigned long   dwBpp;
    unsigned long   dwRefresh;
    unsigned long   dwDeviceType;
} VALMODEXTR;

// - Added function protos for support functions added
BOOL  nvSetDisplayDevice(PDEV *ppdev, PSETGETDEVTYPE pvIn);
BOOL  nvGetDisplayDevice(PDEV *ppdev, PSETGETDEVTYPE pvIn);
BOOL  __cdecl nvDoValidateDeviceModeEsc(PDEV *ppdev, VALMODEXTR *pvIn, DWORD *pvOut);
BOOL  nvGetDisplayDeviceClass(PDEV *ppdev, ULONG *deviceclass);


//
// Before calling RM IOCTL routines use csFifo mutex.  This is also
// used before kicking off push buffers.  This should remove any chance
// that a software interrupt can get into the resource manager at the
// same time as a user IOCTL on MP systems.
//
#define NVDMAPUSHMODESWITCHCOUNT 0x6986

#define CHECK_PDEV_NULL(_STR) \
    if (ppdev == NULL || pvIn == NULL || pvOut == NULL) \
        { \
        DISPDBG((0, "DrvEscape - %s NULL error!", _STR)); \
        return(FALSE); \
        }
#define CHECK_IN_OUT_SIZE(_STR, _INSIZE, _OUTSIZE) \
    if (_INSIZE != cjIn) \
        { \
        DISPDBG((0, "DrvEscape - %s input size error! needed:%d != cjIn:%d", _STR, _INSIZE, cjIn)); \
        return(FALSE); \
        } \
    if (_OUTSIZE != cjOut) \
        { \
        DISPDBG((0, "DrvEscape - %s output size error! needed:%d != cjOut:%d ", _STR, _OUTSIZE, cjOut)); \
        return(FALSE); \
        }

#define OGL_ESCAPE_FAIL_TEST(_PPDEV) ((_PPDEV)->oglMutexPtr == NULL)

static ULONG DoOpenGLEscapeCommand(PPDEV ppdev, PVOID in, PVOID out);

#ifdef NV3
// WARNING: This structure is defined in the client side.
// Don't change it without changing the OpenGL clients too.
typedef struct Nv3DmaPushInfo {
    DWORD dwChID;
    DWORD dwTlbPtBase;
    DWORD dwAddressSpace;
    DWORD get;
    DWORD put;
    DWORD localModeSwitchCount;
    DWORD hClient;
    DWORD hDevice;
} Nv3DmaPushInfo;

extern ULONG NV3_DmaPushGo(PDEV *ppdev, ULONG chID, ULONG tlbPtBase, ULONG busAddressSpace, ULONG getOffset, ULONG putOffset, ULONG hClient, ULONG hDevice);
#endif

static ULONG OglCheckForClientDll(PDEV *ppdev);

//******************************************************************************
//
//  Function:   DrvEscape(SURFOBJ *, ULONG, ULONG, VOID *, ULONG cjOut, VOID *pvOut)
//
//  Routine Description:
//
//      Driver escape entry point.  This function should return TRUE for any
//      supported escapes in response to QUERYESCSUPPORT, and FALSE for any
//      others.  All supported escapes are called from this routine.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


ULONG DrvEscape(SURFOBJ *pso, ULONG iEsc,
                ULONG cjIn, VOID *pvIn,
                ULONG cjOut, VOID *pvOut)

    {
    ULONG   retVal;
    ULONG   oglSupported;
    ULONG   returnedDataLength;
    ULONG   cbReturned;
    PDEV    *ppdev = (PDEV *)pso->dhpdev;

#ifdef _WIN32_WINNT // >= NT 4.x

    //**************************************************************************
    //
    // On 64 bit systems, the following switch statement is for escape codes
    // that are handled identically on both 32 and 64 bit clients.
    // This primarily means the RM calls where the incoming parameter
    // structure is deliberately sized the same for both models.
    //
    // When in doubt, add new escapes to the second switch statement below.
    //
    //**************************************************************************
    switch(iEsc)
    {
    case NV_ESC_RM_OPEN:
        {
        CHECK_PDEV_NULL("NV_OPEN");
        CHECK_IN_OUT_SIZE("NV_OPEN", sizeof(DWORD), sizeof(DWORD));

        ppdev->pfnAcquireOglMutex(ppdev);
        (*(HANDLE *)pvOut) = NvOpen(ppdev->hDriver);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_CLOSE:
        {
        CHECK_PDEV_NULL("NV_CLOSE");
        CHECK_IN_OUT_SIZE("NV_CLOSE", sizeof(DWORD), sizeof(DWORD));

        ppdev->pfnAcquireOglMutex(ppdev);
        NvClose(ppdev->hDriver);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_ALLOC_ROOT:
        {
        NVOS01_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("ALLOC_ROOT");
        CHECK_IN_OUT_SIZE("ALLOC_ROOT", sizeof(NVOS01_PARAMETERS), sizeof(NVOS01_PARAMETERS));

        inParms  = (NVOS01_PARAMETERS *)pvIn;
        outParms = (NVOS01_PARAMETERS *)pvOut;
        inParms->hClass  = NV01_ROOT_USER;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvAllocRoot(ppdev->hDriver, NV01_ROOT_USER, &inParms->hObjectNew);
        ppdev->pfnReleaseOglMutex(ppdev);

        outParms->hObjectNew = inParms->hObjectNew;

        return(TRUE);
        }
        break;
    case NV_ESC_RM_ALLOC_DEVICE:
        {
        NVOS06_PARAMETERS *inParms, *outParms;
        union
        {
            ULONG osName;
            char devName[NV_DEVICE_NAME_LENGTH_MAX+1];
        } nameBuffer;

        CHECK_PDEV_NULL("ALLOC_DEVICE");
        CHECK_IN_OUT_SIZE("ALLOC_DEVICE", sizeof(NVOS06_PARAMETERS), sizeof(NVOS06_PARAMETERS));

        inParms  = (NVOS06_PARAMETERS *)pvIn;
        outParms = (NVOS06_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);

        // escape the device class to the device reference
        nameBuffer.osName = ppdev->ulDeviceReference;

        outParms->status = NvAllocDevice(ppdev->hDriver,
                                         inParms->hObjectParent,
                                         inParms->hObjectNew,
                                         NV03_DEVICE_XX, // inParms->hClass,
                                         (unsigned char *)nameBuffer.devName);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_ALLOC_CONTEXT_DMA:
        {
        NVOS03_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("ALLOC_CONTEXT_DMA");
        CHECK_IN_OUT_SIZE("ALLOC_CONTEXT_DMA", sizeof(NVOS03_PARAMETERS), sizeof(NVOS03_PARAMETERS));

        inParms  = (NVOS03_PARAMETERS *)pvIn;
        outParms = (NVOS03_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvAllocContextDma(ppdev->hDriver,
                                             inParms->hObjectParent,
                                             inParms->hObjectNew,
                                             inParms->hClass,
                                             inParms->flags,
                                             (PVOID)inParms->pBase,
                                             (ULONG)inParms->limit);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_ALLOC_CHANNEL_PIO:
        {
        NVOS04_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("ALLOC_CHANNEL_PIO");
        CHECK_IN_OUT_SIZE("ALLOC_CHANNEL_PIO", sizeof(NVOS04_PARAMETERS), sizeof(NVOS04_PARAMETERS));

        inParms  = (NVOS04_PARAMETERS *)pvIn;
        outParms = (NVOS04_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvAllocChannelPio(ppdev->hDriver,
                                             inParms->hRoot,
                                             inParms->hObjectParent,
                                             inParms->hObjectNew,
                                             inParms->hClass,
                                             inParms->hObjectError,
                                             (PVOID)&inParms->pChannel,
                                             inParms->flags);
        ppdev->pfnReleaseOglMutex(ppdev);

        outParms->pChannel = inParms->pChannel;

        return(TRUE);
        }
        break;
    case NV_ESC_RM_ALLOC_CHANNEL_DMA:
        {
        NVOS07_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("ALLOC_CHANNEL_DMA");
        CHECK_IN_OUT_SIZE("ALLOC_CHANNEL_DMA", sizeof(NVOS07_PARAMETERS), sizeof(NVOS07_PARAMETERS));

        inParms  = (NVOS07_PARAMETERS *)pvIn;
        outParms = (NVOS07_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvAllocChannelDma(ppdev->hDriver,
                                             inParms->hRoot,
                                             inParms->hObjectParent,
                                             inParms->hObjectNew,
                                             inParms->hClass,
                                             inParms->hObjectError,
                                             inParms->hObjectBuffer,
                                             inParms->offset,
                                             (PVOID)&inParms->pControl);
        ppdev->pfnReleaseOglMutex(ppdev);

        outParms->pControl = inParms->pControl;

        return(TRUE);
        }
        break;
    case NV_ESC_RM_ALLOC_MEMORY:
        {
        NVOS02_PARAMETERS *inParms, *outParms;
        PVOID memory;
        ULONG limit;

        CHECK_PDEV_NULL("ALLOC_MEMORY");
        CHECK_IN_OUT_SIZE("ALLOC_MEMORY", sizeof(NVOS02_PARAMETERS), sizeof(NVOS02_PARAMETERS));

        inParms  = (NVOS02_PARAMETERS *)pvIn;
        outParms = (NVOS02_PARAMETERS *)pvOut;

        memory = (PVOID)inParms->pMemory; 
        limit = (ULONG)inParms->pLimit; 

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvAllocMemory(ppdev->hDriver,
                                         inParms->hRoot,
                                         inParms->hObjectParent,
                                         inParms->hObjectNew,
                                         inParms->hClass,
                                         inParms->flags,
                                         &memory,
                                         &limit);
        ppdev->pfnReleaseOglMutex(ppdev);

        outParms->pMemory = (NvP64)memory;
        outParms->pLimit  = (NvU64)limit;

        return(TRUE);
        }
        break;
    case NV_ESC_RM_ALLOC:
        {
        NVOS21_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("ALLOC");
        CHECK_IN_OUT_SIZE("ALLOC", sizeof(NVOS21_PARAMETERS), sizeof(NVOS21_PARAMETERS));

        inParms  = (NVOS21_PARAMETERS *)pvIn;
        outParms = (NVOS21_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvAlloc(ppdev->hDriver,
                                   inParms->hRoot,
                                   inParms->hObjectParent,
                                   inParms->hObjectNew,
                                   inParms->hClass,
                                   (PVOID)inParms->pAllocParms);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_ALLOC_OBJECT:
        {
        NVOS05_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("ALLOC_OBJECT");
        CHECK_IN_OUT_SIZE("ALLOC_OBJECT", sizeof(NVOS05_PARAMETERS), sizeof(NVOS05_PARAMETERS));

        inParms  = (NVOS05_PARAMETERS *)pvIn;
        outParms = (NVOS05_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvAllocObject(ppdev->hDriver,
                                             inParms->hRoot,
                                             inParms->hObjectParent,
                                             inParms->hObjectNew,
                                             inParms->hClass);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_ALLOC_EVENT:
        {
        NVOS10_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("ALLOC_EVENT");
        CHECK_IN_OUT_SIZE("ALLOC_EVENT", sizeof(NVOS10_PARAMETERS), sizeof(NVOS10_PARAMETERS));

        inParms  = (NVOS10_PARAMETERS *)pvIn;
        outParms = (NVOS10_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvAllocEvent(ppdev->hDriver,
                                            inParms->hRoot,
                                            inParms->hObjectParent,
                                            inParms->hObjectNew,
                                            inParms->hClass,
                                            inParms->index,
#ifdef _WIN64
                                            (PVOID) inParms->hEvent);
#else
                                            (ULONG) inParms->hEvent);
#endif
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_FREE:
        {
        NVOS00_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("NV_FREE");
        CHECK_IN_OUT_SIZE("NV_FREE", sizeof(NVOS00_PARAMETERS), sizeof(NVOS00_PARAMETERS));

        inParms  = (NVOS00_PARAMETERS *)pvIn;
        outParms = (NVOS00_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvFree(ppdev->hDriver,
                                  inParms->hRoot,
                                  inParms->hObjectParent,
                                  inParms->hObjectOld);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_ARCH_HEAP:
        {
        NVOS11_PARAMETERS *inParms, *outParms;
        ULONG             lPitch;
        FLATPTR           fpVidMem;

        CHECK_PDEV_NULL("RM_ARCH_HEAP");
        CHECK_IN_OUT_SIZE("RM_ARCH_HEAP", sizeof(NVOS11_PARAMETERS), sizeof(NVOS11_PARAMETERS));

        inParms  = (NVOS11_PARAMETERS *)pvIn;
        outParms = (NVOS11_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);

        // copy input data to set it up
        *outParms = *inParms;

#ifdef RM_HEAPMGR
        outParms->status = NvArchHeap(ppdev->hDriver, inParms);
        outParms->offset = inParms->offset;
        outParms->pitch  = inParms->pitch;
#else
        // TODO: track heap allocations and cleanup on process
        // termination in DestroyContextClientInfo.
        if (inParms->function == NVOS11_HEAP_ALLOC_SIZE)
            {
            lPitch = (ULONG)inParms->pitch;
            lPitch = (lPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
            NVHEAP_ALLOC(fpVidMem, lPitch * inParms->height, TYPE_IMAGE);
            inParms->offset = (ULONG)fpVidMem;
            if (inParms->offset)
                {
                outParms->offset = inParms->offset;
                outParms->pitch  = inParms->pitch;
                outParms->status = NVOS11_STATUS_SUCCESS;
                }
            else
                {
                outParms->offset = inParms->offset;
                outParms->pitch  = inParms->pitch;
                outParms->status = NVOS11_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                }
            }
        else
            {
            NVHEAP_FREE(inParms->offset);
            outParms->status = NVOS11_STATUS_SUCCESS;
            }
#endif

        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_CONFIG_VERSION:
        {
        NVOS12_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("CONFIG_VERSION");
        CHECK_IN_OUT_SIZE("CONFIG_VERSION", sizeof(NVOS12_PARAMETERS), sizeof(NVOS12_PARAMETERS));

        inParms  = (NVOS12_PARAMETERS *)pvIn;
        outParms = (NVOS12_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvConfigVersion (ppdev->hDriver,
                                            inParms->hClient,
                                            inParms->hDevice,
                                            &outParms->version);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_CONFIG_GET:
        {
        NVOS13_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("CONFIG_GET");
        CHECK_IN_OUT_SIZE("CONFIG_GET", sizeof(NVOS13_PARAMETERS), sizeof(NVOS13_PARAMETERS));

        inParms  = (NVOS13_PARAMETERS *)pvIn;
        outParms = (NVOS13_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvConfigGet (ppdev->hDriver,
                                        inParms->hClient,
                                        inParms->hDevice,
                                        inParms->index,
                                        &outParms->value);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_CONFIG_SET:
        {
        NVOS14_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("CONFIG_SET");
        CHECK_IN_OUT_SIZE("CONFIG_SET", sizeof(NVOS14_PARAMETERS), sizeof(NVOS14_PARAMETERS));

        inParms  = (NVOS14_PARAMETERS *)pvIn;
        outParms = (NVOS14_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvConfigSet (ppdev->hDriver,
                                        inParms->hClient,
                                        inParms->hDevice,
                                        inParms->index,
                                        inParms->newValue,
                                        &outParms->oldValue);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_CONFIG_GET_EX:
        {
        NVOS_CONFIG_GET_EX_PARAMS *inParms, *outParms;

        CHECK_PDEV_NULL("CONFIG_GET_EX");
        CHECK_IN_OUT_SIZE("CONFIG_GET_EX", sizeof(NVOS_CONFIG_GET_EX_PARAMS), sizeof(NVOS_CONFIG_GET_EX_PARAMS));

        inParms  = (NVOS_CONFIG_GET_EX_PARAMS *)pvIn;
        outParms = (NVOS_CONFIG_GET_EX_PARAMS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvConfigGetEx (ppdev->hDriver,
                                          inParms->hClient,
                                          inParms->hDevice,
                                          inParms->index,
                                          (ULONG *)inParms->paramStructPtr,
                                          inParms->paramSize);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_CONFIG_SET_EX:
        {
        NVOS_CONFIG_SET_EX_PARAMS *inParms, *outParms;

        CHECK_PDEV_NULL("CONFIG_SET_EX");
        CHECK_IN_OUT_SIZE("CONFIG_SET_EX", sizeof(NVOS_CONFIG_SET_EX_PARAMS), sizeof(NVOS_CONFIG_SET_EX_PARAMS));

        inParms  = (NVOS_CONFIG_SET_EX_PARAMS *)pvIn;
        outParms = (NVOS_CONFIG_SET_EX_PARAMS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvConfigSetEx (ppdev->hDriver,
                                          inParms->hClient,
                                          inParms->hDevice,
                                          inParms->index,
                                          (ULONG *)inParms->paramStructPtr,
                                          inParms->paramSize);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_CONFIG_UPDATE:
        {
        NVOS15_PARAMETERS *inParms, *outParms;

        CHECK_PDEV_NULL("CONFIG_UPDATE");
        CHECK_IN_OUT_SIZE("CONFIG_UPDATE", sizeof(NVOS15_PARAMETERS), sizeof(NVOS15_PARAMETERS));

        inParms  = (NVOS15_PARAMETERS *)pvIn;
        outParms = (NVOS15_PARAMETERS *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms->status = NvConfigUpdate (ppdev->hDriver,
                                           inParms->hClient,
                                           inParms->hDevice,
                                           inParms->flags);
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;
    case NV_ESC_RM_DEBUG_CONTROL:
        {
        NVOS20_PARAMETERS *parms;
        NVOS20_PARAMETERS *outParms;
        parms = (NVOS20_PARAMETERS *) pvIn;
        outParms = (NVOS20_PARAMETERS *)pvOut;

        outParms->status = NvRmDebugControl(ppdev->hDriver,
                                            parms->hRoot,
                                            parms->command,
                                            (PVOID)(parms->pArgs));
        return (ULONG)TRUE;
        }
        break;
    case ESC_NV_CONVERT_RING3_TO_RING0:
        {
        DWORD status, cbReturned;
        HANDLE* input[] = {pvIn, pvOut};

        CHECK_PDEV_NULL("NV_OPEN");
        CHECK_IN_OUT_SIZE("NV_CONVERT_RING3_TO_RING0", sizeof(HANDLE), sizeof(HANDLE));        

        if (*(HANDLE*)pvIn == NULL)
        {
            *(HANDLE*)pvOut = NULL;
            return(TRUE);
        }

        // the miniport will convert the handle
        status = EngDeviceIoControl(
            ppdev->hDriver,
            IOCTL_CONVERT_RING3_HANDLE,
            input,
            2 * sizeof(HANDLE*),
            input,
            2 * sizeof(HANDLE*),
            &cbReturned
        );

        if (status)
        {
            *(HANDLE*)pvOut = NULL;            
        }

        return(TRUE);
        }
        break;
    case QUERYESCSUPPORT:
        {
        // QUERYESCSUPPORT is sent to the driver to ask do you support
        // this pvIn points to the function code to query
        switch(*(ULONG *)pvIn)
            {
            case OPENGL_GETINFO:
                if (OGL_ESCAPE_FAIL_TEST(ppdev))
                    {
                    return((ULONG)FALSE);
                    }
#ifdef NV3
                oglSupported = (ppdev->cBitsPerPel == 16);
#else
                oglSupported = ((ppdev->cBitsPerPel == 16) ||
                                (ppdev->cBitsPerPel == 32));
#endif // NV3

#ifdef _WIN64
                oglSupported = oglSupported && (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC);
#endif
                if (oglSupported)
                    return((ULONG)TRUE); // return non-zero if supported
                return((ULONG)FALSE);
            break;
            case OPENGL_CMD:
                return((ULONG)TRUE); // supported
            break;
            default:
            break;
            }
        break;
        }
    case OPENGL_GETINFO:
        {
        if (OGL_ESCAPE_FAIL_TEST(ppdev))
            {
            return((ULONG)FALSE);
            }

        if (OglSupportEnabled(ppdev) == (ULONG)FALSE)
            {
            return((ULONG)FALSE);
            }
#ifdef NV3
        oglSupported = (ppdev->cBitsPerPel == 16);
#else
        oglSupported = ((ppdev->cBitsPerPel == 16) ||
                        (ppdev->cBitsPerPel == 32));
#endif // NV3

#ifdef _WIN64
        oglSupported = oglSupported && (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC); 
#endif
        if (oglSupported) {
            if (*((ULONG *)pvIn) == OPENGL_GETINFO_DRVNAME) {
#ifdef NV3
                if (wcslen(OPENGL_KEY_NAME_NV3) <= MAX_PATH) {
                    (void)wcscpy((wchar_t *)((LP_OPENGL_INFO) pvOut)->awch, OPENGL_KEY_NAME_NV3);
                    ((LP_OPENGL_INFO) pvOut)->ulVersion       = OPENGL_ICD_VER;
                    ((LP_OPENGL_INFO) pvOut)->ulDriverVersion = OPENGL_DRIVER_VER;

                    return((ULONG)TRUE);
                }
#else                
                if (wcslen(OPENGL_KEY_NAME_NV4) <= MAX_PATH) {
                    (void)wcscpy((wchar_t *)((LP_OPENGL_INFO) pvOut)->awch, OPENGL_KEY_NAME_NV4);
                    ((LP_OPENGL_INFO) pvOut)->ulVersion       = OPENGL_ICD_VER;
                    ((LP_OPENGL_INFO) pvOut)->ulDriverVersion = OPENGL_DRIVER_VER;

                    return((ULONG)TRUE);
                }
#endif            
            }
        }
        return((ULONG)FALSE);
        }
        break;
    case OPENGL_CMD:
      { 
            NV_OPENGL_COMMAND *pNvOpenGLCmd = (NV_OPENGL_COMMAND *)pvIn;
            PPDEV              ppdevTemp    = ppdev;

            if (   (NULL == pNvOpenGLCmd)
                || (cjIn < sizeof(NV_OPENGL_COMMAND)) )
            {
                DISPDBG((0, "DrvEscape - OPENGL_CMD input param error! needed:%d != cjIn:%d, pvIn:0x%p", sizeof(NV_OPENGL_COMMAND), cjIn, pNvOpenGLCmd));
                return(FALSE);
            }

#if defined(_WIN64)
            ASSERTDD(pNvOpenGLCmd->NVClientModel == NV_ESC_IS_64BIT_CLIENT || pNvOpenGLCmd->NVClientModel == NV_ESC_IS_32BIT_CLIENT, "NVClientModel not preset !!!");
#else
            ASSERTDD(pNvOpenGLCmd->NVClientModel == NV_ESC_IS_32BIT_CLIENT, "ICD Client is not matching 64 bit client on 32 bit server or NVClientModel not preset !!!");
#endif
            // this call has to be send to the right device in multimon configuration
            // we use the device origin for that purpose
#ifdef NV_MAIN_MULTIMON
            if( bOglRetrieveValidPDEVForOrigin( &ppdevTemp, pNvOpenGLCmd->devOriginLeft, pNvOpenGLCmd->devOriginTop ) )
#else
            if( NULL != ppdevTemp )
#endif
            {
                //************************************************************************** 
                // Need to do smart sync between OpenGL and GDI depending on 
                // clip intersection.
                //**************************************************************************
                bOglSyncOglIfGdiRenderedToWindow(ppdevTemp);

                return ((ULONG)DoOpenGLEscapeCommand(ppdevTemp, (PVOID)pvIn, (PVOID)pvOut));
            }
            else
            {
                ASSERT(FALSE);
                return((ULONG)FALSE);
            }
            
        }
        break;
    case WNDOBJ_SETUP:
        {
        NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;
        NV_WNDOBJ_SETUP *pNvWndObj;
        LONG status;
#if defined(_WIN64)
        NV_WNDOBJ_SETUP    nvWndObj;
        NV_WNDOBJ_SETUP32 *pNvWndObj32;

        if (cjIn == sizeof(NV_WNDOBJ_SETUP32)) {
            pNvWndObj32 = (NV_WNDOBJ_SETUP32 *)pvIn;
            pNvWndObj = &nvWndObj;
            try
                {
                    NvCopyStructIn(pNvWndObj32, pNvWndObj, "IPPII");
                }
            except(EXCEPTION_EXECUTE_HANDLER)
                {
                status = GetExceptionCode();
                DISPDBG((1, "WNDOBJ_SETUP: exception = 0x%lx", status));
                return((ULONG)FALSE);
                }
 
        } else
#endif
        pNvWndObj = (NV_WNDOBJ_SETUP *)pvIn;

        if((pNvWndObj != NULL ) && (pNvWndObj->NVSignature == NV_SIGNATURE))
            {
            try
                {
                ppdev->numDevices = pNvWndObj->numDevices;
                OglAcquireGlobalMutex();
                clientDrawableInfo = CreateDrawableClientInfo(pso,
                    pNvWndObj->hWnd,
                    pNvWndObj->hDC,
                    &pNvWndObj->pfd);
               ((PVOID *)pvOut)[0] = clientDrawableInfo;
                OglReleaseGlobalMutex();
                }
            except(EXCEPTION_EXECUTE_HANDLER)
                {
                status = GetExceptionCode();
                DISPDBG((1, "WNDOBJ_SETUP: exception = 0x%lx", status));
                return((ULONG)FALSE);
                }
            return((ULONG)TRUE);
            }
        else
            {
            return((ULONG)FALSE);
            }
        }
        break;
   // - Added to provide the path to the driver reg area (hard coded under winnt4)
   case NV_ESC_GET_LOCAL_REGISTRY_PATH:
      {
         // - The first four spaces in the string are required to remain comp with w9x behavior
#if (_WIN32_WINNT >= 0x0500)
        UCHAR drv_path[128];
#else
        UCHAR drv_path[] = "    SYSTEM\\CurrentControlSet\\Services\\nv4\\Device0";
#endif
         if(ppdev == NULL ||
            pvOut == NULL || cjOut < sizeof(drv_path) ) { // - Remember to change this to X 2 when unicode

            DISPDBG((2, "DrvEscape - can't NV_ESC_GET_LOCAL_REGISTRY_PATH"));
            return(FALSE);
            }

#if (_WIN32_WINNT >= 0x0500)
        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_GET_REGISTRY_PATH,
                               NULL,
                               0,
                               (LPVOID) (drv_path),
                               sizeof(drv_path),
                               &returnedDataLength))
        {
            return(FALSE);
        }
        if (returnedDataLength == 0)
        {
            return(FALSE);
        }
#endif

        // - Do a simple memcpy for now, till unicode later
        {
            int i;
            for(i=0;drv_path[i]!=0 && i<127;i++)
               ((UCHAR*)pvOut)[i] = drv_path[i];
            ((UCHAR*)pvOut)[i] = 0;
        }

        return(TRUE);
      }
      break;


    default:
        break;
    }

    //**************************************************************************
    //
    // DISABLED UNTIL IMPLEMENTATION IS COMPLETE
    //
    // On 64 bit systems, call DrvEscape32() for escape codes that need
    // special handling (pointer conversion) for 32 bit clients.
    //
    //
    //**************************************************************************
    if (iEsc & NV_ESC_64_BASE)
        iEsc &= ~NV_ESC_64_BASE;
#if 0
    else
        return(DrvEscape32(pso, iEsc, cjIn, pvIn, cjOut, pvOut));
#endif

    //**************************************************************************
    //
    // The following switch statement handles the bulk of the cases where the
    // incoming parameter data differs in size from 32 and 64 bit clients.
    //
    //**************************************************************************

    switch(iEsc)
    {
    case NV_ESC_PRIMARY_INFO:
        {
        NV_PRIMARY_INFO *primaryInfoIn = (NV_PRIMARY_INFO *)pvIn;
        NV_PRIMARY_INFO *primaryInfoOut = (NV_PRIMARY_INFO *)pvOut;
        LONG status;
        unsigned int numActiveDACs, dacMask, activeDACs, d;

        try
            {
            if (primaryInfoIn && primaryInfoOut && primaryInfoIn->ulNVSignature == NV_SIGNATURE)
                {
                primaryInfoOut->ulWidth         = ppdev->cxScreen;
                primaryInfoOut->ulHeight        = ppdev->cyScreen;
                primaryInfoOut->ulDepth         = ppdev->cjPelSize << 3;
                primaryInfoOut->ulPitch         = ppdev->lDelta;
                primaryInfoOut->ulPrimaryOffset = ppdev->ulPrimarySurfaceOffset;
                primaryInfoOut->ulNumDACs       = ppdev->ulNumberDacsOnBoard;
                primaryInfoOut->ulNumActiveDACs = ppdev->ulNumberDacsActive;
                primaryInfoOut->ulPanningPtr    = NULL;
                primaryInfoOut->ulReturnCode    = TRUE;

                // Compute a bitmap of active DACs by looping over all displays
                // until we've checked them all, or found ulNumberDacsActive
                // unique DACs--whichever comes first.
                for (d = activeDACs = numActiveDACs = 0; (d < NV_NO_DACS) &&
                     (numActiveDACs < ppdev->ulNumberDacsActive); d++)
                    {
                    if (ppdev->ulDeviceDisplay[d] < NV_NO_DACS)
                        {
                        dacMask = 1 << ppdev->ulDeviceDisplay[d];
                        if (!(dacMask & activeDACs))
                            {
                            numActiveDACs++;
                            activeDACs |= dacMask;
                            }
                        }
                    }
                primaryInfoOut->ulActiveDACs = activeDACs;
                }
            else
                {
                return (ULONG)FALSE;
                }
            }
        except(EXCEPTION_EXECUTE_HANDLER)
            {
            status = GetExceptionCode();
            DISPDBG((1, "NV_ESC_PRIMARY_INFO: exception = 0x%lx", status));
            return (ULONG)FALSE;
            }
        }
        return (ULONG)TRUE;
    case NVDMAPUSHMODESWITCHCOUNT:
        {
        if (OGL_ESCAPE_FAIL_TEST(ppdev))
            {
            return((ULONG)FALSE);
            }

        if (pvOut != NULL)
            {
            ((ULONG *)pvOut)[0] = 0; // ppdev->dwGlobalModeSwitchCount;
            return((ULONG)TRUE);
            }
        return((ULONG)FALSE);
        }
        break;
    case ESC_NV_OPENGL_ESCAPE:
        {
        return((ULONG)DoOpenGLEscapeCommand(ppdev, (PVOID)pvIn, (PVOID)pvOut));
        }
        break;
    case NV_OPENGL_SET_ENABLE:
        {
        // return the current mode switch counter to the caller and
        // the shared mutex lock pointer.
        ((ULONG *)pvOut)[0] = ppdev->dwGlobalModeSwitchCount;
        return((ULONG)TRUE); // return non-zero if supported
        }
        break;
    case ESC_NV_OPENGL_DMA_PUSH_GO:
        {

            if (ppdev == NULL || pvOut == NULL || pvIn == NULL) {
                return((ULONG)FALSE);
            } else {
#ifdef NV3
                Nv3DmaPushInfo *dmaPushInfo;
                dmaPushInfo = (Nv3DmaPushInfo *)((Nv3DmaPushInfo *)pvIn);

                if ((long)dmaPushInfo->put - (long)dmaPushInfo->get <= 0) {
                    ((ULONG *)pvOut)[1] = ppdev->dwGlobalModeSwitchCount;

                    return((ULONG)FALSE);
                }

                if (ppdev->cBitsPerPel == 16 &&
                    dmaPushInfo->localModeSwitchCount == ppdev->dwGlobalModeSwitchCount)
                    {
                    // on NV3 this is a DMA push kickoff escape function
                    ((ULONG *)pvOut)[0] = (ULONG)NV3_DmaPushGo(ppdev,
                                                               dmaPushInfo->dwChID,
                                                               dmaPushInfo->dwTlbPtBase,
                                                               dmaPushInfo->dwAddressSpace,
                                                               dmaPushInfo->get,
                                                               dmaPushInfo->put,
                                                               dmaPushInfo->hClient,
                                                               dmaPushInfo->hDevice);
                }
#else                
                // on NV4 this is an engine sync escape function
                // force a channel sync when GDI gets called again for rendering
                ppdev->oglLastChannel = -1;  // FORCE IT TO SYNC...
                ppdev->NVFreeCount = 0;
                ppdev->pfnWaitForChannelSwitch(ppdev);
#endif // NV3
            
                ((ULONG *)pvOut)[1] = ppdev->dwGlobalModeSwitchCount;

                DISPDBG((2, "DrvEscape - SUCCESS!"));
                return((ULONG)TRUE);
            }
        }
        break;
    case ESC_SET_GAMMA_RAMP:
        {
        if (ppdev == NULL || pvIn == NULL || cjIn != sizeof(GAMMARAMP) )
            {
            DISPDBG((2, "DrvEscape - can't set gamma ramp"));
            return(FALSE);
            }
        nvSetGammaRamp(ppdev, (PUSHORT) pvIn);
        return(TRUE);
        }
        break;
    case ESC_GET_GAMMA_RAMP:
        {
        if (ppdev == NULL || pvOut == NULL || cjOut != sizeof(GAMMARAMP) ) 
            {
            DISPDBG((2, "DrvEscape - can't get gamma ramp"));
            return(FALSE);
            }
        nvGetGammaRamp(ppdev, (PUSHORT) pvOut);
        return(TRUE);
        }
        break;
    case ESC_SET_GAMMA_RAMP_MULTI:
        {
        if (ppdev == NULL || pvIn == NULL || cjIn != sizeof(GAMMARAMP_MULTI) )
            {
            DISPDBG((2, "DrvEscape - can't set gamma ramp multi"));
            return(FALSE);
            }
        nvSetHeadGammaRamp(ppdev, ((PGAMMARAMP_MULTI)pvIn)->wHead, (PUSHORT)&(((PGAMMARAMP_MULTI)pvIn)->grGammaRamp));
        return(TRUE);
        }
        break;
    case ESC_GET_GAMMA_RAMP_MULTI:
        {
        if (ppdev == NULL || pvOut == NULL || cjOut != sizeof(GAMMARAMP_MULTI) ||
                             pvIn == NULL  || cjIn != sizeof(GAMMARAMP_MULTI) )
            {
            DISPDBG((2, "DrvEscape - can't get gamma ramp multi"));
            return(FALSE);
            }
        nvGetHeadGammaRamp(ppdev, ((PGAMMARAMP_MULTI)pvIn)->wHead, (PUSHORT)&(((PGAMMARAMP_MULTI)pvOut)->grGammaRamp));
        return(TRUE);
        }
        break;
    case NV_ESC_RM_DMA_PUSH_INFO:
        {
        // TODO: define a structure for this call...
        ULONG *inParms, *outParms;

        CHECK_PDEV_NULL("RM_DMA_PUSH_INFO");
        CHECK_IN_OUT_SIZE("RM_DMA_PUSH_INFO", sizeof(NVOS08_PARAMETERS), sizeof(NVOS08_PARAMETERS));

        inParms  = (ULONG *)pvIn;
        outParms = (ULONG *)pvOut;

        ppdev->pfnAcquireOglMutex(ppdev);
        outParms[4] = NvGetDmaPushInfo (ppdev->hDriver,
                                        inParms[0],  // hClient
                                        inParms[1],  // hDevice
                                        inParms[2],  // hChannel
                                        inParms[3],  // a pointer to a DWORD array
                                        inParms[4]); // hDmaContext
        ppdev->pfnReleaseOglMutex(ppdev);

        return(TRUE);
        }
        break;

    case ESC_NV_QUERY_PCI_SLOT:
    {
        ULONG * pulPciSlot;

        pulPciSlot = (ULONG *) pvOut;

        if(EngDeviceIoControl(ppdev->hDriver,
                       IOCTL_VIDEO_QUERY_PCI_SLOT,
                       NULL,
                       0,
                       (LPVOID) (pulPciSlot),
                       sizeof(ULONG),
                       &returnedDataLength))
        {
            DISPDBG((2, "DrvEscape - can't get PCI number"));
            return(FALSE);
        }
        
        return(TRUE);
    }    
    break;

    case ESC_NV_QUERY_HEAD_REFRESH_RATE:
    {
        ULONG * pulHead;
        ULONG * pulRefreshRate;

        pulHead = (ULONG *) pvIn;
        pulRefreshRate = (ULONG *) pvOut;

        if( (pulHead == NULL) || (*pulHead > 1) )
        {
            DISPDBG((2, "DrvEscape - Invaild Head number"));
            return(FALSE);
        }

        if( ppdev == NULL || pulRefreshRate == NULL)
        {
            DISPDBG((2, "DrvEscape - NULL pointer for refresh rate"));
            return(FALSE);
        }

        *pulRefreshRate = ppdev->ulRefreshRate[*pulHead];

        return(TRUE);
    }    
    break;


    case ESC_NV_QUERYSET_REGISTRY_KEY:
        {
            __PNVRegKeyInfo inParms, outParms;
            WCHAR tmpStrBuf[NV_MAX_REG_KEYNAME_LEN];
            ULONG  inpStrLen, outStrLen;
            ULONG ii, returnedDataLen;
            NV_REGISTRY_STRUCT regStruct;
            NV_REGISTRY_STRUCT regStructInput, regStructOutput;
            DWORD retStatus;
            
            CHECK_PDEV_NULL("QUERYSET_REGISTRY_KEY");
            CHECK_IN_OUT_SIZE("QUERYSET_REGISTRY_KEY", sizeof(__NVRegKeyInfo), sizeof(__NVRegKeyInfo));

            inParms = (__PNVRegKeyInfo) pvIn;


            if (!inParms->keyName)
                return (FALSE);

            inpStrLen = strlen(inParms->keyName);

            if ((inpStrLen+1) > NV_MAX_REG_KEYNAME_LEN)
                return (FALSE);
            /*
            for (ii = 0; ii < inpStrLen; ii++)
                tmpStrBuf[ii] = inParms->keyName[ii];
            tmpStrBuf[ii] = 0;
            */

            EngMultiByteToUnicodeN((LPWSTR)tmpStrBuf, NV_MAX_REG_KEYNAME_LEN*sizeof(WCHAR), &outStrLen,
                                   inParms->keyName, (inpStrLen+1)*sizeof(CHAR));



            switch(inParms->type)
            {
            case NV_QUERY_REGISTRY_BINARY_KEY:
                outParms = (__PNVRegKeyInfo) pvOut;
                *outParms = *inParms;

                regStructInput.keyName = (PWSTR)tmpStrBuf;
                regStructInput.keyVal = outParms->keyVal;
                regStructInput.keyValSize = outParms->keyValMaxSize;

                retStatus = EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_QUERY_REGISTRY_BINARY_VAL,
                               (LPVOID)&regStructInput,
                               sizeof(NV_REGISTRY_STRUCT),
                               (LPVOID) (outParms->keyVal),
                               outParms->keyValMaxSize,
                               &(returnedDataLen));

                outParms->retStatus = retStatus;

                // Copy the returned registry value size into outParams
                outParms->keyValSize = returnedDataLen;
                // The KeyVal has already been copied by the miniport

                break;

            case NV_QUERY_REGISTRY_KEY:

                outParms = (__PNVRegKeyInfo) pvOut;
                *outParms = *inParms;

                retStatus = EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_QUERY_REGISTRY_VAL,
                               (LPVOID)tmpStrBuf,
                               outStrLen,
                               (LPVOID) (outParms->keyVal),
                               outParms->keyValMaxSize,
                               &(outParms->keyValSize));

                outParms->retStatus = retStatus;

                break;

            case NV_SET_REGISTRY_KEY:

                outParms = (__PNVRegKeyInfo) pvOut;
                *outParms = *inParms;

                regStruct.keyName = (PWSTR)tmpStrBuf;
                regStruct.keyVal = inParms->keyVal;
                regStruct.keyValSize = inParms->keyValSize;

                retStatus = EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_SET_REGISTRY_VALUE,
                               (LPVOID)(&regStruct),
                               sizeof(NV_REGISTRY_STRUCT),
                               (LPVOID) NULL,
                               0,
                               &returnedDataLen);

                outParms->retStatus = retStatus;

                break;

            default:
                break;
            }
            return (TRUE);
        }
        break;

    case ESC_NV_DESKMGR_ZOOMBLIT:
#ifdef _WIN32_WINNT
        {
            __NVDMEscapeParams* pParams = (__NVDMEscapeParams*) pvIn;

            // Validate parameters.
            if (!(pParams && (cjIn >= (sizeof(__NVDMEscapeParams) +
                                      (pParams->clips - 1) * sizeof(RECTL)))))
                return (ULONG) FALSE;
            // If we don't have appropriate scaled-image HW support, fail.
            if (!(ppdev->CurrentClass.ScaledImageFromMemory &&
                  ppdev->pfnStretchCopy))
                return (ULONG) FALSE;

            // If performing a blit out of system memory...
            if (pParams->flags & NVDM_ESC_BLIT_SYSMEM) {
                // Not supported at the moment.
                return (ULONG) TRUE;
            }
            // Otherwise do a vidmem-to-vidmem blit.
            return (ULONG)
                ppdev->pfnStretchCopy(ppdev,
                                      pParams->flags & NVDM_ESC_BLIT_SMOOTH,
                                      pParams->scale,
                                      &pParams->srcRect, &pParams->dstRect,
                                      pParams->clips, pParams->clipRects);
        }
#endif  //#ifdef _WIN32_WINNT
        return (ULONG) TRUE;

#if (NVARCH >= 0x4) && (_WIN32_WINNT >= 0x0500) && !defined(_WIN64)
    case NV_ESC_DDPRIV_DMABLIT:
       {
           ULONG i;
           if (cjOut >= sizeof(NVS2VERROR)) {
               ((LPNVS2VDATATYPE)pvIn)->dwDevice = ppdev->ulDeviceReference;
               *((NVS2VERROR *)pvOut) = nvMoComp(pvIn);
               return (TRUE);
           }
       }
       break;

   case NV_ESC_DDPRIV_OVERLAYCONTROL:
       {
           ULONG i;
           if (cjOut >= sizeof(NVOCERROR)) {
               ((LPNVOCDATATYPE)pvIn)->dwDevice = ppdev->ulDeviceReference;
               *((NVOCERROR *)pvOut) = nvOverlayControl(pvIn);
               return (TRUE);
           }
       }
       break;
#endif

    case NV_ESC_SETGET_TWINVIEW_DATA:
        {

            NVTWINVIEWDATA  * nvDataIn;
            NVTWINVIEWDATA  * nvDataOut;
            ULONG GetFlag;  // 1 means a "Get" kind of action and 0 means a "Set" kind of action
            ULONG i, j;

            DISPDBG((1, "Enter NV_ESC_SETGET_TWINVIEW_DATA"));

            if (ppdev == NULL || pvIn == NULL || pvOut == NULL) {
                DISPDBG((0, "DrvEscape - cant NV_ESC_SETGET_TWINVIEW_DATA: ppdev: 0x%x, pvIn: 0x%x, pvOut: 0x%x", ppdev, pvIn, pvOut));
                return(FALSE);
            }
            //
            // Need to add some defensive code in case the Control Panel doesn't match the driver. If this
            // happens, the structure size can be different and cause lots of problems, especially when doing
            // the memcpy below. Note that I'm not checking for cjIn/cjOut to be equal the NVTWINVIEWDATA
            // because there is a case during initialization where this isn't true - they are about 4 times
            // larger. Need to check with Control Panel guys, and so this is at least a first pass to make
            // sure the memcpy below won't get a page fault.
            //
            if ((cjIn < sizeof(NVTWINVIEWDATA)) || (cjOut < sizeof(NVTWINVIEWDATA)))
            {
                DISPDBG((1, "DrvEscape - NV_ESC_SETGET_TWINVIEW_DATA error: cjIn: 0x%x, cjOut: 0x%x", cjIn, cjOut));
                return(FALSE);
            }
            nvDataIn = (NVTWINVIEWDATA *)pvIn;
            nvDataOut = (NVTWINVIEWDATA *)pvOut;

            switch (nvDataIn->dwFlag) {
                case NVTWINVIEW_FLAG_GET: 
                     GetFlag = 1;
                     // Do a copy of the pvIn to pvOut.
                     // This is to handle the case where the OS zeroes out the pvOut before calling the display driver.
                     memcpy(nvDataOut, nvDataIn, sizeof(NVTWINVIEWDATA));
                     break;
                case NVTWINVIEW_FLAG_SET: 
                     GetFlag = 0;
                     break;
                default:
                     DISPDBG((0," oops! dwFlag is unknown: 0x%x\n",nvDataIn->dwFlag));
                     nvDataOut->dwSuccess = FALSE;
                     return(TRUE);
                     break;
            }
            nvDataOut->dwSuccess = TRUE;

            switch (nvDataIn->dwAction) {
                case NVTWINVIEW_ACTION_SETGET_PHYSICAL_VIRTUAL_MODE:
                    if (GetFlag)
                    {
                        SET_RECT(&nvDataOut->DeskTopRectl, 0, 0, ppdev->cxScreen, ppdev->cyScreen);
                        nvDataOut->dwState = ppdev->TwinView_State;
                        nvDataOut->dwOrientation = ppdev->TwinView_Orientation;
                        nvDataOut->dwAllDeviceMask = ppdev->ulAllDeviceMask;
                        nvDataOut->dwConnectedDeviceMask = ppdev->ulConnectedDeviceMask;
                        for (i=0; i < ppdev->ulNumberDacsOnBoard; i++) {
                            nvDataOut->dwDeviceDisplay[i] = ppdev->ulDeviceDisplay[i];
                        }
                    }
                    else
                    {
                       BOOLEAN DataOK;
                        DataOK = TwinViewDataOK(ppdev, nvDataIn);
                        if (!DataOK) 
                        {
                            DISPDBG((0,"Oops! The panel did not pass us sensible data. So return Failure\n"));
                            nvDataOut->dwSuccess = FALSE;
                            return(TRUE);
                        }
                        //
                        // Now write the TwinViewInfo structure to the registry.
                        //
                        PrintTwinViewData ("SET_PHYSICAL_VIRTUAL_MODE:Info sent by nvPanel", 1, nvDataIn, ppdev);
                        WriteRegistryTwinViewInfo(ppdev, nvDataIn);

                        // Some of the flags are synchronous. They take effect immediately.
                        // Handle the synchronous EnablePanScan flag.
                        ProcessSynchronousFlags(ppdev, nvDataIn);


                        return(TRUE);
                    }

                    for (i=0; i < ppdev->ulNumberDacsOnBoard; i++) {
                        if (GetFlag) {
                            SET_RECT(&nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].VirtualRectl,ppdev->rclDisplayDesktop[i].left,
                                ppdev->rclDisplayDesktop[i].top, ppdev->rclDisplayDesktop[i].right,ppdev->rclDisplayDesktop[i].bottom);
                            SET_RECT(&nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].PhysicalRectl,ppdev->rclCurrentDisplay[i].left,
                                ppdev->rclCurrentDisplay[i].top,ppdev->rclCurrentDisplay[i].right,ppdev->rclCurrentDisplay[i].bottom);

                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwDeviceMask = ppdev->ulDeviceMask[i];

                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwDeviceType = ppdev->ulDeviceType[i];
                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwTVFormat = ppdev->ulTVFormat[i];
                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwBpp = ppdev->cBitsPerPel; // same for both devices
                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwRefresh = ppdev->ulRefreshRate[i];
                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwNumPhysModes = ppdev->ulNumPhysModes[i];
                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwNumVirtualModes = ppdev->ulNumVirtualModes[i];

                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwTimingOverRide = ppdev->ulTimingOverRide[i];
                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwEnableDDC = ppdev->ulEnableDDC[i];
                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwEnablePanScan = ppdev->ulEnablePanScan[i];
                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwFreezePanScan = ppdev->ulFreezePanScan[i];
                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwFreezeOriginX = ppdev->ulFreezeOriginX[i];
                            nvDataOut->NVTWINVIEWUNION.nvtwdevdata[i].dwFreezeOriginY = ppdev->ulFreezeOriginY[i];

                        } else {
                            DISPDBG((0,"OOps! Logic error in display driver. Should not be here. file: %s, line: %d\n",__FILE__,__LINE__));
                        }
                    } // for
                    if (GetFlag) {
                        //
                        // Print the info we are sending back to the panel
                        //
                        PrintTwinViewData ("GET_PHYSICAL_VIRTUAL_MODE:Info returned to nvPanel", 2, nvDataOut, ppdev);
                    }
                    break;

                case NVTWINVIEW_ACTION_SETGET_PHYSICAL_VIRTUAL_MODELIST:
                    if (GetFlag) {
                        for (i=0; i < ppdev->ulNumberDacsOnBoard; i++) {
                            if (!bGetPhysicalVirtualModeList(ppdev, i, 

                                    nvDataOut->NVTWINVIEWUNION.nvtwModeListData[i].dwDeviceMask,

                                    nvDataOut->NVTWINVIEWUNION.nvtwModeListData[i].dwDeviceType,
                                    nvDataOut->NVTWINVIEWUNION.nvtwModeListData[i].dwTVFormat,
                                    nvDataOut->NVTWINVIEWUNION.nvtwModeListData[i].PhysModeEntries,
                                    nvDataOut->NVTWINVIEWUNION.nvtwModeListData[i].VirtualModeEntries,
                                    &nvDataOut->NVTWINVIEWUNION.nvtwModeListData[i].dwNumPhysModeEntries,
                                    &nvDataOut->NVTWINVIEWUNION.nvtwModeListData[i].dwNumVirtualModeEntries))
                            {
                                nvDataOut->dwSuccess = FALSE;
                                return(TRUE);
                            }
                        }
                    } 
                    else 
                    {
                        // There is no necessity for this path. The panel should never need to tell
                        // the driver to set the physmodeList for a head.
                    }
                    break;

                default: DISPDBG((0,"Invalid unsupported dwAction: 0x%x\n",nvDataIn->dwAction));
                     nvDataOut->dwSuccess = FALSE;
                     return(TRUE);
                    break;
            }
            return(TRUE);
        }


    case NV_ESC_SET_DISPLAY_DEVICE_TYPE:
        {
        if (ppdev == NULL || pvIn == NULL || cjIn != sizeof(SETGETDEVTYPE))
            {
            DISPDBG((2, "DrvEscape - cant NV_ESC_SET_DISPLAY_DEVICE_TYPE"));
            return(FALSE);
            }
        DISPDBG((2, "DrvEscape - NV_ESC_SET_DISPLAY_DEVICE_TYPE"));
        if(!nvSetDisplayDevice(ppdev, (PSETGETDEVTYPE) pvIn)) {
           return(FALSE);
        }
        return(TRUE);
        }
        break;


    case NV_ESC_GET_DISPLAY_DEVICE_TYPE:
        {
        if (ppdev == NULL || pvOut == NULL || cjOut != sizeof(SETGETDEVTYPE))
            {
            DISPDBG((2, "DrvEscape - cant NV_ESC_GET_DISPLAY_DEVICE_TYPE"));
            return(FALSE);
            }
        DISPDBG((2, "DrvEscape - NV_ESC_GET_DISPLAY_DEVICE_TYPE"));
        if(!nvGetDisplayDevice(ppdev, (PSETGETDEVTYPE) pvOut)) {
            return(FALSE);
        }
        return(TRUE);
        }
        break;

    case NV_ESC_GET_DEVICE_SCAN_DATA:
        {
        GET_DEVICE_SCAN_DATA_TYPE * pInfo;
        ULONG i;
        if (ppdev == NULL || pvOut == NULL || cjOut != sizeof(GET_DEVICE_SCAN_DATA_TYPE))
            {
            DISPDBG((0, "DrvEscape - cant NV_ESC_GET_DEVICE_SCAN_DATA"));
            return(FALSE);
            }
        DISPDBG((1, "DrvEscape - NV_ESC_GET_DEVICE_SCAN_DATA"));
        pInfo = (GET_DEVICE_SCAN_DATA_TYPE *)pvOut;
        pInfo->ulNumberDacsOnBoard = ppdev->ulNumberDacsOnBoard;
        pInfo->ulNumberDacsConnected = ppdev->ulNumberDacsConnected;
        pInfo->ulNumberDacsActive = ppdev->ulNumberDacsActive;
        for (i=0; i < ppdev->ulNumberDacsOnBoard; i++)
        {
            pInfo->ulHeadDeviceOptions[i] = ppdev->ulHeadDeviceOptions[i];
        }
        
        return(TRUE);
        }
        break;

    case NV_ESC_GET_DEVICE_MASK:
        {
        GET_DEVICE_MASK_DATA_TYPE * pInfo;
        ULONG ulPrimaryHead;
        ULONG ReturnedDataLength;
        if (ppdev == NULL || pvOut == NULL || cjOut != sizeof(GET_DEVICE_MASK_DATA_TYPE))
            {
            DISPDBG((0, "DrvEscape - cant NV_ESC_GET_DEVICE_MASK"));
            return(FALSE);
            }
        DISPDBG((1, "DrvEscape - NV_ESC_GET_DEVICE_MASK, ppdev->ulInduceModeChangeDeviceMask: 0x%x",
                 ppdev->ulInduceModeChangeDeviceMask));
        pInfo = (GET_DEVICE_MASK_DATA_TYPE *)pvOut;
        //
        // Initialize the return data.
        //
        pInfo->ulDeviceMask = 0;

        //
        // A sanity check. We enable this modeswitch only if the NVSVC has started.
        // We should not run into this situation since we would never be here unless NVSVC had responded to
        // the signal on the named event. A sanity check however should not hurt.
        //
        if (ppdev->ulNVSVCStatus != NVSVC_STATUS_STARTED)
        {
            DISPDBG((1,"Something wrong. NVSVC is not running."));
            return(TRUE);
        }


#if _WIN32_WINNT < 0x500
        //
        // See if we got here as a result of a NT4.0 hotkey switch, such as on gateway laptops.
        // I can put a further check here for ACPI system. Do this later.
        //
        if (bHandleNT4HotKey(ppdev, pInfo))
        {
            //
            // There was a hotkey pending. We processed it and now we can return.
            //
            return(TRUE);
        }
#endif

        //
        // We are not here because of a hotkey switch.
        // Ergo, this is a device switch initiated by the display driver.
        // So return the device mask to be used by the control panel.
        //
        pInfo->ulDeviceMask = ppdev->ulInduceModeChangeDeviceMask;
        DISPDBG((1, "Info being returned: ulDeviceMask: 0x%x",pInfo->ulDeviceMask));

        //
        // The ulInduceModeChangeDeviceMask has been consumed by the panel. So clear it now so that subsequent
        // modesets will not cause StopImage method to show up. Also, let miniport know too.
        // Note: it is better to do it here rather than on entry into InitMultiMon(). Reason is the hibernation
        // case where GDI callsDrvAssertMode(TRUE) twice and by the time control panel makes the GET_DEVICE_MASK
        // escape call, the second DrvAssertMode(TRUE) would have sneaked in and cleared the 
        // ulInduceModeChangeDeviceMask before the control panel had a chance to consume it.
        //
        ppdev->ulInduceModeChangeDeviceMask = 0;
        if (EngDeviceIoControl(ppdev->hDriver,
                        IOCTL_VIDEO_SET_NVSVC_MODESET_DEVMASK,
                        &ppdev->ulInduceModeChangeDeviceMask,
                        sizeof(ULONG),
                        NULL,
                        0,
                        &ReturnedDataLength))
        {
            DISPDBG((1, "InitMultiMon() - IOCTL_VIDEO_SIGNAL_MODESET_DEVMASK failed"));
        }   
        return(TRUE);
        }
        break;

    case NV_ESC_INDUCE_DEVICE_SCAN:
        {
            if (ppdev == NULL)
               {
               DISPDBG((0, "DrvEscape - cant NV_ESC_INDUCE_DEVICE_SCAN"));
               return(FALSE);
               }
            DISPDBG((1, "DrvEscape - NV_ESC_INDUCE_DEVICE_SCAN"));
            InduceAndUpdateDeviceScanning(ppdev);

            return(TRUE);
        }
        break;

    case NV_ESC_SET_NVSVC_STATUS:
        {
        ULONG ulNVSVCStatus;
        ULONG returnedDataLength;

        if (ppdev == NULL || pvIn == NULL || cjIn != sizeof(ULONG))
            {
            DISPDBG((0, "DrvEscape - cant NV_ESC_SET_NVSVC_STATUS"));
            return(FALSE);
            }
        DISPDBG((1, "DrvEscape - NV_ESC_SET_NVSVC_STATUS"));
        ulNVSVCStatus = *(ULONG *)pvIn;
        if (ulNVSVCStatus != NVSVC_STATUS_STARTED && ulNVSVCStatus != NVSVC_STATUS_STOPPED &&
            ulNVSVCStatus != NVSVC_STATUS_PAUSED)
        {
            //
            // Invalid data passed to us by NVSVC
            //
            DISPDBG((0, "DrvEscape - Invalid NVSVC_STATUS data passed in by NVSVC: 0x%x",ulNVSVCStatus));
            return(FALSE);
        }
        DISPDBG((1, "NVSVCStatus being set: 0x%x\n",ulNVSVCStatus));
        //
        // Store it in the pdev.
        //
        ppdev->ulNVSVCStatus = ulNVSVCStatus;
        //
        // Let the miniport know of this latest status also.
        //
        if(EngDeviceIoControl(ppdev->hDriver,
                       IOCTL_VIDEO_SET_NVSVC_STATUS,
                       &ulNVSVCStatus,
                       sizeof(ULONG),
                       NULL,
                       0,
                       &returnedDataLength))
        {
            DISPDBG((0, "DrvEscape - can't do SET_NVSVC_STATUS"));
            return(FALSE);
        }

        return(TRUE);
        }
        break;

    case NV_ESC_QUERY_SAVE_SETTINGS:
        {

            QUERY_SAVE_SETTINGS  * nvDataIn;
            NVTWINVIEWDATA  * nvDataOut;
            NVTWINVIEWDATA  * pFind;

            DISPDBG((1, "Enter NV_ESC_QUERY_SAVE_SETTINGS"));

            if (ppdev == NULL || pvIn == NULL || pvOut == NULL) {
                DISPDBG((0, "DrvEscape - cant NV_ESC_QUERY_SAVE_SETTINGS: ppdev: 0x%x, pvIn: 0x%x, pvOut: 0x%x", ppdev, pvIn, pvOut));
                return(FALSE);
            }
            //
            // Need to add some defensive code in case the Control Panel doesn't match the driver. 
            //
            if ((cjIn < sizeof(QUERY_SAVE_SETTINGS)) || (cjOut < sizeof(NVTWINVIEWDATA)))
            {
                DISPDBG((1, "DrvEscape - NV_ESC_QUERY_SAVE_SETTINGS error: cjIn: 0x%x, cjOut: 0x%x", cjIn, cjOut));
                return(FALSE);
            }
            nvDataIn = (QUERY_SAVE_SETTINGS *)pvIn;
            nvDataOut = (NVTWINVIEWDATA *)pvOut;
            
            DISPDBG((1, "Query: State: 0x%x, Orientation: 0x%x, mapping array:(%d, %d), deviceMask: (0x%x, 0x%x)",
                nvDataIn->ulState,nvDataIn->ulOrientation, nvDataIn->ulDeviceDisplay[0], nvDataIn->ulDeviceDisplay[1],
                nvDataIn->ulDeviceMask[0],nvDataIn->ulDeviceMask[1]));

            //
            // Find the saved settings slot for the requested device combo
            //
            pFind = pFindSaveSettings(ppdev, nvDataIn, NULL);
            if (pFind == NULL)
            {
                DISPDBG((1,"Something wrong. pFindSaveSettings() returned failure"));
                nvDataOut->dwSuccess = FALSE;
                return(TRUE);
            }
        
            //
            // We found the saved settings. Now return the data to the panel
            //
            memcpy(nvDataOut, pFind, sizeof(NVTWINVIEWDATA));
            nvDataOut->dwSuccess = TRUE;
            //
            // Print the info we are sending back to the panel
            //
            PrintTwinViewData ("QUERY_SAVE_SETTINGS:Info returned to nvPanel", 1, nvDataOut, ppdev);
            return(TRUE);
        }
        break;

    case NV_ESC_FORCE_SAVE_SETTINGS:
        {
            NVTWINVIEWDATA  * nvDataIn;
            NVTWINVIEWDATA  * pFind;

            DISPDBG((1, "Enter NV_ESC_QUERY_SAVE_SETTINGS"));

            if (ppdev == NULL || pvIn == NULL) {
                DISPDBG((0, "DrvEscape - cant NV_ESC_FORCE_SAVE_SETTINGS: ppdev: 0x%x, pvIn: 0x%x", ppdev, pvIn));
                return(FALSE);
            }
            //
            // Need to add some defensive code in case the Control Panel doesn't match the driver. 
            //
            if (cjIn < sizeof(NVTWINVIEWDATA))
            {
                DISPDBG((1, "DrvEscape - NV_ESC_FORCE_SAVE_SETTINGS error: cjIn: 0x%x", cjIn));
                return(FALSE);
            }
            nvDataIn = (NVTWINVIEWDATA *)pvIn;
            PrintTwinViewData("DataInput", 1, nvDataIn, ppdev);

            //
            // Find the saved settings slot for the requested device combo
            //
            pFind = pFindSaveSettings(ppdev, NULL, nvDataIn);
            if (pFind == NULL)
            {
                DISPDBG((1,"Something wrong. pFindSaveSettings() returned failure"));
                // How do I pass an error value back to control panel here?
                return(TRUE);
            }
        
            //
            // We found the saved settings. Now Copy the passed in data into the slot.
            //
            *pFind = *nvDataIn;

            //
            // Write the table back to the registry.
            //
            vWriteSaveSettings(ppdev);
            return(TRUE);
        }
        break;

    case NV_ESC_DISPLAY_ICON:
        {
        DISPLAY_ICON_DATA_TYPE * pInfo;
        if (ppdev == NULL || pvIn == NULL || cjIn != sizeof(DISPLAY_ICON_DATA_TYPE))
            {
            DISPDBG((0, "DrvEscape - cant NV_ESC_DISPLAY_ICON"));
            return(FALSE);
            }
        DISPDBG((1, "DrvEscape - NV_ESC_DISPLAY_ICON"));
        pInfo = (DISPLAY_ICON_DATA_TYPE *)pvIn;
        return(TRUE);
        }
        break;

    case NV_ESC_HIDE_ICON:
        {
            if (ppdev == NULL)
            {
                DISPDBG((0, "DrvEscape - cant NV_ESC_HIDE_ICON:"));
                return(FALSE);
            }
            DISPDBG((1, "DrvEscape - NV_ESC_HIDE_ICON"));
            return(TRUE);
        }
        break;

    case NV_ESC_DISABLE_HW_CURSOR:
        {
            if (ppdev == NULL || pvIn == NULL)
            {
                DISPDBG((0, "DrvEscape - Incorrect param to disable HW cursor"));
                return(FALSE);
            }
            if(*(ULONG *)pvIn == FALSE)
            {
                ppdev->flCaps &= ~CAPS_SW_POINTER;
                DISPDBG((1, "DrvEscape - enable HW cursor"));
            }
            else
            {
                ppdev->flCaps |= CAPS_SW_POINTER;
                DISPDBG((1, "DrvEscape - disable HW cursor"));
            }
            return(TRUE);
        }
        break;

   case NV_ESC_VALIDATE_DEVICE_MODE:
      {
        if (ppdev == NULL ||
            pvIn  == NULL || cjIn  != sizeof(VALMODEXTR) ||
            pvOut == NULL || cjOut != sizeof(DWORD))
            {
            DISPDBG((2, "DrvEscape - cant NV_ESC_VALIDATE_DEVICE_MODE"));
            return(FALSE);
            }
        DISPDBG((2, "DrvEscape - NV_ESC_VALIDATE_DEVICE_MODE"));

        if(!nvDoValidateDeviceModeEsc(ppdev, (VALMODEXTR *)pvIn, (DWORD *) pvOut)) {
           return(FALSE);
        }
        return(TRUE);
      }
      break;


   // - Added to provide id of the board and type
   case NV_ESC_IS_NVIDIA_BOARD_AND_DRIVER:
      {
        if (ppdev == NULL ||
            pvOut == NULL || cjOut != 2 * sizeof(DWORD)) // Two DWORDs
            {
            DISPDBG((2, "DrvEscape - cant NV_ESC_IS_NVIDIA_BOARD_AND_DRIVER"));
            return(FALSE);
            }
        DISPDBG((2, "DrvEscape - NV_ESC_IS_NVIDIA_BOARD_AND_DRIVER"));

        // - Store the ESC value back in the 1st DWORD
        ((DWORD *)pvOut)[0] = NV_ESC_IS_NVIDIA_BOARD_AND_DRIVER;

        // - Store the nvidia device class in the second DWORD
        if(!nvGetDisplayDeviceClass(ppdev, &((DWORD *)pvOut)[1])) {
           return(FALSE);
        }

        return(TRUE);
      }
      break;


#ifdef   NV_ESCAPE_ID
   // - Added to positive id of nvidia drivers
   case NV_ESCAPE_ID:
      {
        if (ppdev == NULL ||
            pvOut == NULL || cjOut != 1 * sizeof(DWORD))
            {
            DISPDBG((2, "DrvEscape - cant NV_ESCAPE_ID"));
            return(FALSE);
            }
        DISPDBG((2, "DrvEscape - NV_ESCAPE_ID"));

        // - Store the NV_ESCAPE_ID_RETURN value in the 1st DWORD
        ((DWORD *)pvOut)[0] = NV_ESCAPE_ID_RETURN;
        return(TRUE);
      }
      break;
#endif

   case NV_ESC_RM_I2C_ACCESS:
      {

         NVOS_I2C_ACCESS_PARAMS *inParms, *outParms;
         ULONG ReturnedDataLength;

         CHECK_PDEV_NULL("I2C_ACCESS");
         CHECK_IN_OUT_SIZE("I2C_ACCESS", sizeof(NVOS_I2C_ACCESS_PARAMS), sizeof(NVOS_I2C_ACCESS_PARAMS));

         inParms  = (NVOS_I2C_ACCESS_PARAMS *)pvIn;
         outParms = (NVOS_I2C_ACCESS_PARAMS *)pvOut;

         if( EngDeviceIoControl( ppdev->hDriver,
                                IOCTL_NV04_I2C_ACCESS,
                                pvIn,
                                sizeof(NVOS_I2C_ACCESS_PARAMS),
                                pvOut,
                                sizeof(NVOS_I2C_ACCESS_PARAMS),
                                &ReturnedDataLength) ) {

            // The IOCTL failed, so return fail
            return(FALSE);
         }

         // Make sure we got what we wanted, at least size wise
         if(ReturnedDataLength != sizeof(NVOS_I2C_ACCESS_PARAMS))
            return(FALSE);

         return(TRUE);
      }
      break;

      case ESC_NV_MAP_USER:
        {
        // Setup the parms
        NV_SYSMEM_SHARE_MEMORY *sysmemIn;

        CHECK_PDEV_NULL("MAP_USER");

        sysmemIn = (NV_SYSMEM_SHARE_MEMORY *)pvIn;

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_MAP_TO_USER,
                               sysmemIn,
                               sizeof(NV_SYSMEM_SHARE_MEMORY),
                               (NV_SYSMEM_SHARE_MEMORY *)pvOut,
                               sizeof(NV_SYSMEM_SHARE_MEMORY),
                               &returnedDataLength))
            {
            DISPDBG((2, "DrvEscape - can't map address"));
            return(FALSE);
            }
            // Add some clean-up registering code.

        return(TRUE);
        }
        break;

    case ESC_NV_UNMAP_USER:
        {
        NV_SYSMEM_SHARE_MEMORY sysmemShareMemoryIn, sysmemShareMemoryOut;

        CHECK_PDEV_NULL("UNMAP_PFIFO_USER");

        sysmemShareMemoryIn = *(NV_SYSMEM_SHARE_MEMORY *)pvIn;
        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_UNMAP_FROM_USER,
                               &sysmemShareMemoryIn,
                               sizeof(NV_SYSMEM_SHARE_MEMORY),
                               (PULONG) &sysmemShareMemoryOut,
                               sizeof(NV_SYSMEM_SHARE_MEMORY),
                               &returnedDataLength))
            {
            DISPDBG((2, "DrvEscape - can't unmap NV PFIFO"));
            return(FALSE);
            }
        return(TRUE);
        }
        break;

#ifdef USE_WKS_ELSA_TOOLS
    case ESC_ELSA_TOOLS:
    {
        // Only support the interface on a QUADRO piece
        if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC)
        {
            return ((ULONG)lEscapeTools(pso, cjIn, pvIn, cjOut, pvOut));
        }
        // not supported on this device -> defaults to FALSE
        break;
    }
#endif //USE_WKS_ELSA_TOOLS

    case ESC_NV_WKS_APP_SUPPORT:
    {
        return ((ULONG)lWksAppSupport(pso, cjIn, pvIn, cjOut, pvOut));
        break;
    }

    default:
        break;
    }

#endif  // >= NT 4.x
    return (ULONG)FALSE;
    }

static ULONG DoOpenGLEscapeCommand(PPDEV ppdev, PVOID pvIn, PVOID pvOut)
{
    NV_OPENGL_COMMAND *inCommand;
    ULONG             *outPtr = NULL;
    LONG              status;
    ULONG             ulRet = FALSE;

    ASSERT(ppdev);

    inCommand = (NV_OPENGL_COMMAND *)pvIn;
    try
        {
        ppdev->numDevices = inCommand->numDevices;
        }
    except(EXCEPTION_EXECUTE_HANDLER)
        {
        status = GetExceptionCode();
        DISPDBG((1, "DoOpenGLEscapeCommand: exception = 0x%lx", status));
        return(1 + (ULONG)FALSE);
        }

#if defined(_WIN64)
    if (!EngSaveFloatingPointState(ppdev->fpState, ppdev->fpStateSize))
        {
        DISPDBG((1, "EngSaveFloatingPointState failed"));
        return(1 + (ULONG)FALSE);
        }
#endif

    switch(inCommand->command)
    {
#if (NVARCH >= 0x4)
        case ESC_NV_OPENGL_COMMAND:
            // Call back to shared library to execute a command
            if (NV_OglOperation(ppdev,
                                inCommand->hWnd,
                                inCommand->hDC,
                                (__GLNVoperation*)inCommand->other)) {
                ulRet = ((ULONG)TRUE);
            } else {
                ulRet = ((ULONG)FALSE);
            }
            break;
        case ESC_NV_OPENGL_SYNC_CHANNEL:
            // force a channel sync when GDI gets called again for rendering
            ppdev->oglLastChannel = -1;  // FORCE IT TO SYNC...
            ppdev->NVFreeCount = 0;
            ppdev->pfnWaitForChannelSwitch(ppdev);
            ulRet = ((ULONG)TRUE);
            break;
        case ESC_NV_OPENGL_SHARED_LIBRARY_SIZES:
            {
            extern int sizeOfState;
            extern int sizeOfDrawablePrivate;
            int *sizes = (int *)inCommand->other;

                if (sizes[0] == sizeOfState && // __GLNVstate
                    sizes[1] == sizeOfDrawablePrivate)  // __GLdrawablePrivate
                    {
                    ulRet = ((ULONG)TRUE);
                    break;
                    }
                else
                    {
                    ulRet = ((ULONG)FALSE);
                    break;
                    }
                ulRet = ((ULONG)FALSE);
            }
            break;
        case ESC_NV_OPENGL_SUPPORT:
            if (OglSupportEnabled(ppdev) == (ULONG)TRUE)
                {
                if (inCommand->NVSignature == NV_SIGNATURE)
                    {
                    ulRet = ((ULONG)TRUE);
                    break;
                    }
                else
                    {
                    ulRet = ((ULONG)FALSE);
                    break;
                    }
                }
            ulRet = ((ULONG)FALSE);
            break;
        case ESC_NV_OPENGL_REGISTER_RM_CLIENT:
            OglAcquireGlobalMutex();
            OglRegisterRmClient(ppdev, PtrToUlong(inCommand->other));
            OglReleaseGlobalMutex();
            ulRet = ((ULONG)TRUE);
            break;
        case ESC_NV_OPENGL_UNREGISTER_RM_CLIENT:
            OglAcquireGlobalMutex();
            OglUnRegisterRmClient(ppdev, PtrToUlong(inCommand->other));
            OglReleaseGlobalMutex();
            ulRet = ((ULONG)TRUE);
            break;
        case ESC_NV_OPENGL_DESTROY_DRAWABLE:
            OglAcquireGlobalMutex();
            DestroyDrawableClientInfo(ppdev, inCommand->hWnd, inCommand->hDC, TRUE);
            OglReleaseGlobalMutex();
            ulRet = ((ULONG)TRUE);
            break;
        case ESC_NV_OPENGL_START_STATE_CLIENT_TRACKING:
            {
            OglAcquireGlobalMutex();
            ulRet = CreateContextClientInfo(ppdev, inCommand->hWnd, inCommand->hClient);
            OglReleaseGlobalMutex();
            }
            break;
        case ESC_NV_OPENGL_STOP_STATE_CLIENT_TRACKING:
            OglAcquireGlobalMutex();
            DestroyContextClientInfo(ppdev, inCommand->hClient, TRUE);
            OglReleaseGlobalMutex();
            ulRet = ((ULONG)TRUE);
            break;
        case ESC_NV_OPENGL_ALLOC_SHARED_MEMORY:
            {
            void *globalData, *pfifoAddress, *pbusAddress;

            OglAcquireGlobalMutex();
            CreateSharedClientInfo(ppdev, inCommand->hClient, inCommand->processHandle, &globalData, &pfifoAddress, &pbusAddress);
            ((PVOID *)pvOut)[0] = globalData;
            ((PVOID *)pvOut)[1] = pfifoAddress;
            ((PVOID *)pvOut)[2] = pbusAddress;
            OglReleaseGlobalMutex();
            ulRet = ((ULONG)TRUE);
            }
            break;
        case ESC_NV_OPENGL_FREE_SHARED_MEMORY:
            OglAcquireGlobalMutex();
            DestroySharedClientInfo(ppdev, inCommand->hClient, inCommand->processHandle, inCommand->globalData, TRUE);
            OglReleaseGlobalMutex();
            ulRet = ((ULONG)TRUE);
            break;
        case ESC_NV_OPENGL_GET_DISPLAY_PITCH:
            ((ULONG *)pvOut)[0] = (ULONG)ppdev->lDelta;
            break;
        case ESC_NV4_OPENGL_SWAP_BUFFERS:
            {
            __GLNVswapInfo *swapInfo = (__GLNVswapInfo *)inCommand->other;

            ppdev->pfnAcquireOglMutex(ppdev);
            ulRet = NV4_OglSwapBuffers(ppdev,
                                       inCommand->hWnd,
                                       inCommand->hDC,
                                       swapInfo->hClient,
                                       swapInfo);
            ppdev->pfnReleaseOglMutex(ppdev);
            break;
            }
        case ESC_NV_OPENGL_SWAP_BUFFERS:
            {
            __GLNVswapInfo *swapInfo = (__GLNVswapInfo *)inCommand->other;

            ppdev->pfnAcquireOglMutex(ppdev);
            ulRet = NV_OglSwapBuffers(ppdev,
                                      inCommand->hWnd,
                                      inCommand->hDC,
                                      swapInfo->hClient,
                                      swapInfo);
            ppdev->pfnReleaseOglMutex(ppdev);
            break;
            }
        case ESC_NV_OPENGL_INIT_STATE:
            {
            NV_OglInitState(ppdev, inCommand->other);
            ulRet = ((ULONG)TRUE);
            break;
            }

        case ESC_NV_OPENGL_OVERLAY_MERGEBLIT:
        {
            __GLNVoverlayMergeBlitInfo *mergeInfo = (__GLNVoverlayMergeBlitInfo *)inCommand->other;

            ppdev->pfnAcquireOglMutex(ppdev);
            ulRet = NV_OglOverlayMergeBlit(ppdev,
                                           inCommand->hWnd,
                                           inCommand->hDC,
                                           mergeInfo);
            ppdev->pfnReleaseOglMutex(ppdev);
            break;
        }

        // this is used by W2K in DrvDescribePixelFormat
        case ESC_NV_OPENGL_PFD_CHECK:
        {
            ULONG *pulPfdCheckFlags = pvOut;           
            *pulPfdCheckFlags = 0;

            ulRet = bOglPfdCheckFlags(ppdev, pulPfdCheckFlags) ? 1 : (ULONG)-1;
            break;
        }
#endif // (NVARCH >= 0x4)
        case ESC_NV_OPENGL_PURGE_DEVICE_BITMAP:
            {
            ((ULONG *)pvOut)[0] = (ULONG)0; // means nothing got punted
            if (ppdev->cbGdiHeap)   // contains #bytes allocated by GDI
                {
                bMoveAllDfbsFromOffscreenToDibs(ppdev);
                ((ULONG *)pvOut)[0] = (ULONG)1; // means something got punted
                }
            ulRet = (ULONG) TRUE;
            break;
            }
        case ESC_NV4_OPENGL_FLUSH:
            {
            __GLNVflushInfo *flushInfo = (__GLNVflushInfo *)inCommand->other;

            ppdev->pfnAcquireOglMutex(ppdev);
            ulRet = NV4_OglFlushClipped(ppdev,
                                        inCommand->hWnd,
                                        inCommand->hDC,
                                        flushInfo->hClient,
                                        flushInfo);
            ppdev->pfnReleaseOglMutex(ppdev);
            break;
            }
        case ESC_NV_OPENGL_FLUSH:
            {
#if (NVARCH >= 0x4)
            __GLNVflushInfo *flushInfo = (__GLNVflushInfo *)inCommand->other;
#endif // (NVARCH >= 0x4)

            ulRet = (ULONG) FALSE;

#if (NVARCH >= 0x4)
            ppdev->pfnAcquireOglMutex(ppdev);
            ulRet = NV_OglFlushClipped(ppdev,
                                       inCommand->hWnd,
                                       inCommand->hDC,
                                       flushInfo->hClient,
                                       flushInfo);
            ppdev->pfnReleaseOglMutex(ppdev);
#endif // (NVARCH >= 0x4)
            break;
            }
        case ESC_NV_OPENGL_DMA_PUSH_GO_CMD:
            {
                if (ppdev == NULL || pvOut == NULL || pvIn == NULL) {
                    ulRet = ((ULONG)FALSE);
                    break;
                } else {
#ifdef NV3
                    Nv3DmaPushInfo *dmaPushInfo;
                    dmaPushInfo = (Nv3DmaPushInfo *)((Nv3DmaPushInfo *)pvIn);

                    if ((long)dmaPushInfo->put - (long)dmaPushInfo->get <= 0) {
                        ((ULONG *)pvOut)[1] = ppdev->dwGlobalModeSwitchCount;

                        ulRet = ((ULONG)FALSE);
                        break;
                    }

                    if (ppdev->cBitsPerPel == 16 &&
                        dmaPushInfo->localModeSwitchCount == ppdev->dwGlobalModeSwitchCount)
                        {
                        // on NV3 this is a DMA push kickoff escape function
                        ((ULONG *)pvOut)[0] = (ULONG)NV3_DmaPushGo(ppdev,
                            dmaPushInfo->dwChID,
                            dmaPushInfo->dwTlbPtBase,
                            dmaPushInfo->dwAddressSpace,
                            dmaPushInfo->get,
                            dmaPushInfo->put,
                            dmaPushInfo->hClient,
                            dmaPushInfo->hDevice);
                    }
#else                    
                    // on NV4 this is an engine sync escape function
                    // force a channel sync when GDI gets called again for rendering
                    ppdev->oglLastChannel = -1;  // FORCE IT TO SYNC...
                    ppdev->NVFreeCount = 0;
                    ppdev->pfnWaitForChannelSwitch(ppdev);
#endif // NV3
                }
                ((ULONG *)pvOut)[1] = ppdev->dwGlobalModeSwitchCount;

                DISPDBG((2, "DrvEscape - SUCCESS!"));
            }
            ulRet = ((ULONG)TRUE);
            break;
        case ESC_NV_OPENGL_SCREEN_TO_SCREEN_SRCCOPY:
            {
            ulRet = ((ULONG)FALSE);
            }
            break;
        case ESC_NV_OPENGL_SUPPORT_ENABLED:
            {
            // This is a toggle...
            if (ppdev->bSupportOgl == (ULONG)TRUE)
                ppdev->bSupportOgl = (ULONG)FALSE;
            else
                ppdev->bSupportOgl = (ULONG)TRUE;
            ((ULONG *)pvOut)[0] = ppdev->bSupportOgl; // return current value
            ulRet = ((ULONG)TRUE);
            }
            break;
        case ESC_NV_OPENGL_SET_DAC_BASE:
            ppdev->pfnAcquireOglMutex(ppdev);

            NvRestoreDacs(ppdev);

            ppdev->pfnReleaseOglMutex(ppdev);

            ulRet = ((ULONG)TRUE);
            break;
        case ESC_NV_OPENGL_CPUBLIT:
            {
                __GLNvCpuBltData * oglNvCpuBlitData;

                oglNvCpuBlitData = (__GLNvCpuBltData *)inCommand->other;

                ppdev->pfnAcquireOglMutex(ppdev);
                ulRet = OglCpuBlit(ppdev, oglNvCpuBlitData);

                ppdev->pfnReleaseOglMutex(ppdev);
            }
            break;
        case ESC_NV_OPENGL_ALLOC_UNIFIED_SURFACES:
            if (!OglAllocUnifiedSurfaces(ppdev, inCommand->other))
            {
                DISPDBG((0, "ERROR in DrvEscape: ESC_NV_OPENGL_ALLOC_UNIFIED_SURFACES-OglAllocUnifiedSurfaces failed!"));
            }
            ulRet = (ppdev->bOglSingleBackDepthSupported &&
                     ppdev->bOglSingleBackDepthCreated);
            break;
        case ESC_NV_OPENGL_FREE_UNIFIED_SURFACES:
            OglFreeUnifiedSurfaces(ppdev);
            ulRet = (ppdev->bOglSingleBackDepthSupported &&
                     !ppdev->bOglSingleBackDepthCreated);
            break;
        case ESC_NV_OPENGL_GET_SURFACE_INFO:
            {
                NvU32 fbInfo;
                outPtr = (ULONG *)pvOut;
                
                fbInfo = OglGetUnifiedSurfaceInfo(ppdev, inCommand->other);
                try
                {
                    *outPtr = (ULONG)fbInfo;
                }
                except(EXCEPTION_EXECUTE_HANDLER)
                {
                    status = GetExceptionCode();
                    DISPDBG((1, "ESC_NV_OPENGL_GET_SURFACE_INFO: ppdev: exception = 0x%lx", status));
                }
                if (fbInfo) {
                    ulRet = ((ULONG)TRUE);
                } else {
                    ulRet = ((ULONG)FALSE);
                }
                break;
            }
        case ESC_NV_OPENGL_DID_STATUS_CHANGE:
            outPtr = (ULONG *)pvOut;
            try
                {
                NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
                NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;

                clientList = OglFindClientInfoFromHWndHDC(ppdev, inCommand->hWnd, inCommand->hDC);
                if (clientList)
                    {
                    clientInfo = &clientList->clientInfo;
                    }
                if (clientInfo)
                    {
                    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;

                        clientDrawableInfo = clientInfo->clientDrawableInfo;
                        if (clientDrawableInfo->clipChangedCount != PtrToUlong(inCommand->other))

                        {
                        *outPtr = 1;
                        }
                    else
                        {
                        *outPtr = 0;
                        }
                    }
                else
                    {
                    *outPtr = -1; // client not found
                    }
                }
            except(EXCEPTION_EXECUTE_HANDLER)
                {
                status = GetExceptionCode();
                DISPDBG((1, "ESC_NV_OPENGL_DID_STATUS_CHANGE: ppdev: exception = 0x%lx", status));
                }
            ulRet = ((ULONG)TRUE);
            break;
        case ESC_NV_OPENGL_GET_CLIP_LIST_COUNT:
            outPtr = (ULONG *)pvOut;
            try
                {
                NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
                NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;

                clientList = OglFindClientInfoFromHWndHDC(ppdev, inCommand->hWnd, inCommand->hDC);
                if (clientList)
                    {
                    clientInfo = &clientList->clientInfo;
                    }
                if (clientInfo)
                    {
                    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;

                    clientDrawableInfo = clientInfo->clientDrawableInfo;
                    *outPtr = clientDrawableInfo->numClipRects;
                    }
                else
                    {
                    *outPtr = -1; // client not found
                    }
                }
            except(EXCEPTION_EXECUTE_HANDLER)
                {
                status = GetExceptionCode();
                DISPDBG((1, "ESC_NV_OPENGL_GET_CLIP_LIST_COUNT: ppdev: exception = 0x%lx", status));
                }
            ulRet = ((ULONG)TRUE);
            break;
        case ESC_NV_OPENGL_GET_CLIP_LIST:
            outPtr = (ULONG *)pvOut;
            try
                {
                NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;

                clientList = OglFindClientInfoFromHWndHDC(ppdev, inCommand->hWnd, inCommand->hDC);
                if (clientList)
                    {
                    NV_OPENGL_CLIP_LIST_DATA *nvClipList;
                    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;
                    int       i, rgnDataSize;
                    LPRGNDATA rgnData = NULL;
                    RECTL     *rectList = NULL, *rect = NULL;
                    static RECTL nullRect = { 0,0,0,0 };

                    clientDrawableInfo = clientList->clientInfo.clientDrawableInfo;
                    nvClipList = (NV_OPENGL_CLIP_LIST_DATA *)inCommand->other;

                    rect        = (RECTL *)nvClipList->rect; // bounds rectangle
                    rgnDataSize = sizeof(RGNDATAHEADER) + (clientDrawableInfo->numClipRects * sizeof(RECT));
                    rgnData     = (LPRGNDATA)nvClipList->rgnData;

                    if (rgnDataSize > nvClipList->rgnDataSize)
                        {
                        *outPtr = rgnDataSize;  // Insufficient space--return space needed.
                        }
                    else
                        {
                        rgnDataSize           = nvClipList->rgnDataSize;
                        rgnData->rdh.dwSize   = sizeof(RGNDATAHEADER);
                        rgnData->rdh.iType    = RDH_RECTANGLES;
                        rgnData->rdh.nCount   = clientDrawableInfo->numClipRects;
                        rgnData->rdh.nRgnSize = rgnDataSize;

                        rgnData->rdh.rcBound = clientDrawableInfo->rect;
                        rectList = (RECTL *)&rgnData->rdh.rcBound;
                        if (rectList->left >= rectList->right || rectList->top >= rectList->bottom)
                            {
                            // NULL clip list
                            rgnData->rdh.nCount = 0;
                            *rectList = nullRect; // rgnData->rdh.rcBound set to NULL rectangle
                            }

                        rectList = (RECTL *)&rgnData->Buffer;
                        if ((int)rgnData->rdh.nCount <= 0)
                            {
                            rectList[0] = nullRect;
                            }
                        else
                            {
                            for (i = 0; i < (int)rgnData->rdh.nCount; i++)
                                {
                                rectList[i] = clientDrawableInfo->copyWindowRect[i];
                                if (rectList[i].left >= rectList[i].right ||
                                    rectList[i].top >= rectList[i].bottom)
                                    {
                                    rectList[i] = nullRect;
                                    }
                                }
                            }

                        nvClipList->clipChangedCount = clientDrawableInfo->clipChangedCount;
                        *outPtr = 1;
                        }
                    }
                else
                    {
                    *outPtr = -1; // client not found
                    }
                }
            except(EXCEPTION_EXECUTE_HANDLER)
                {
                status = GetExceptionCode();
                DISPDBG((1, "ESC_NV_OPENGL_GET_CLIP_LIST: ppdev: exception = 0x%lx", status));
                }
            ulRet = ((ULONG)TRUE);
            break;

        case ESC_NV_OPENGL_SET_LAYER_PALETTE:
            ulRet = OglSetLayerPalette(inCommand->other);
            break;

        case ESC_NV_OPENGL_GET_LAYER_PALETTE:
            ulRet = OglGetLayerPalette(inCommand->other);
            break;

        default:
            ulRet = ((ULONG)FALSE);
            break;
    }

#if defined(_WIN64)
    EngRestoreFloatingPointState(ppdev->fpState);
#endif

    // Offset the return value by +1 so that the ESC_NV_OPENGL_ESCAPE escape handler
    // always returns >0, so a zero return from ExtEscape() will indicate a "hard"
    // GDI error.
    return(ulRet + 1);
}


//*************************************************************************
// OglSupportEnabled
// 
// returns TRUE if hw accelerated OpenGL is supported and allowed.
//*************************************************************************
ULONG OglSupportEnabled(PDEV *ppdev)
{
    if (   (ppdev->bSupportOgl == (ULONG)TRUE)         // sw enable/disable
        && (NULL != ppdev->pOglServer)                 // need ogl server data
        && OglCheckForClientDll(ppdev) == (ULONG)TRUE) // need ICD dll
        {
        return((ULONG)TRUE);
        }
    return((ULONG)FALSE);
}

/*
 * This function checks to see if the OpenGL driver is on
 * the system.  If it is then we can return TRUE.  If is
 * isn't then we return FALSE.  This code was added for NT5
 * because Microsoft reported a bug that we claimed to run
 * OpenGL but the client side DLL didn't exist.  But
 * Microsoft wouldn't let us ship the DLL.  This code became
 * old when Microsoft decided to let NVidia ship the OpenGL
 * client side DLL.
 */
static ULONG OglCheckForClientDll(PDEV *ppdev)
{
#if 0
    return((ULONG)TRUE);
#else
    unsigned char oglDllName[MAX_PATH + 1];
    HANDLE        oglDllHandle;

#ifdef NV3
    (void)wcscpy((wchar_t *)oglDllName, OGL_CLIENT_DLL_NAME_NV3);
#else    
    (void)wcscpy((wchar_t *)oglDllName, OGL_CLIENT_DLL_NAME_NV4);
#endif // NV3
    
    oglDllHandle = EngLoadModule((unsigned short *)&oglDllName[0]);
    if (oglDllHandle)
        {
        EngFreeModule(oglDllHandle);
        DISPDBG((2, "OglCheckForClientDll - SUCCESS!"));
        return((ULONG)TRUE);
        }
    DISPDBG((0, "OglCheckForClientDll - FAILED!"));
    return((ULONG)FALSE);
#endif
}

/******************************************************************************
 * This function does a generic CPU blit.
 * It is currently used in buffer region extension.
 * It can be optimized later by figuring out whether the blit was
 * from Mem To Screen OR Screen To Mem OR Mem To Mem.
 ******************************************************************************/
ULONG
OglCpuBlit(PDEV *ppdev, __GLNvCpuBltData *oglNVCpuBltData)
{
    BYTE *src, *dst;
    ULONG  srcStep, dstStep;
    ULONG  height, width;

    if (oglNVCpuBltData->useClipList == 0L)
    {
        src = (BYTE *)(oglNVCpuBltData->pSrcBase)
            + (oglNVCpuBltData->dwSrcTop * oglNVCpuBltData->dwSrcByteWidth)
            + (oglNVCpuBltData->dwSrcLeft * oglNVCpuBltData->dwSrcElementSize);
        dst = (BYTE *)(oglNVCpuBltData->pDstBase)
            + (oglNVCpuBltData->dwDstTop * oglNVCpuBltData->dwDstByteWidth)
            + (oglNVCpuBltData->dwDstLeft * oglNVCpuBltData->dwDstElementSize);

        srcStep = oglNVCpuBltData->dwSrcByteWidth;
        dstStep = oglNVCpuBltData->dwDstByteWidth;

        height = oglNVCpuBltData->dwHeight;
        width = oglNVCpuBltData->dwWidth * oglNVCpuBltData->dwDstElementSize;

        while (height--) {
            memcpy(dst, src, width);
            src += srcStep;
            dst += dstStep;
        }
    }
    else
    {
        /* Intersect rectangle
           (oglNVCpuBltData->dwSrcTop,
            oglNVCpuBltData->dwDstLeft,
            oglNVCpuBltData->dwSrcTop+oglNVCpuBltData->dwHeight,
            oglNVCpuBltData->dwSrcLeft+oglNVCpuBltData->dwWidth)
            with clip list using CombineRectList
            and blit each rectangle separately.
         */
    }
    oglNVCpuBltData->retStatus = TRUE;
    return ((ULONG)TRUE);
}



BOOL  nvSetDisplayDevice(PDEV *ppdev, PSETGETDEVTYPE pvIn)
{

   ULONG ReturnedDataLength;

   // - Attempt to perform the IOCTL to SET the display device
   if (
       EngDeviceIoControl(
            ppdev->hDriver,
            IOCTL_VIDEO_SET_DISPLAY_DEVICE,
            pvIn,
            sizeof(SETGETDEVTYPE),
            NULL,
            0,
            &ReturnedDataLength
      )
   ) {

      // - That call burned, so return fail
      return(FALSE);
   }

   return(TRUE);

}

BOOL  nvGetDisplayDevice(PDEV *ppdev, PSETGETDEVTYPE pvIn)
{

   ULONG ReturnedDataLength;

   // - Attempt to perform the IOCTL to GET the display device
   if (
       EngDeviceIoControl(
            ppdev->hDriver,
            IOCTL_VIDEO_GET_DISPLAY_DEVICE,
            NULL,
            0,
            pvIn,
            sizeof(SETGETDEVTYPE),
            &ReturnedDataLength
      )
   ) {

      // - That call burned, so return fail
      return(FALSE);
   }

   // - Make sure we got what we wanted, at least size wise
   if(ReturnedDataLength != sizeof(SETGETDEVTYPE))
      return(FALSE);

   return(TRUE);

}


BOOL __cdecl nvDoValidateDeviceModeEsc(PDEV *ppdev, VALMODEXTR *pvIn, DWORD *pvOut)
{

   ULONG ReturnedDataLength;

   // Assume that the mode is not valid just to be safe
   *(DWORD *)pvOut = FALSE;

   // - Attempt to perform the IOCTL to validate the device display mode
   if (
       EngDeviceIoControl(
            ppdev->hDriver,
            IOCTL_VALIDATE_DEVICE_MODE,
            pvIn,
            sizeof(VALMODEXTR),
            pvOut,
            sizeof(DWORD),
            &ReturnedDataLength
      )
   ) {

      DISPDBG((1, "DrvEscape - nvDoValidateDeviceModeEsc FAILED"));
      // - That call burned, so return fail
      return(FALSE);
   }

   // - Make sure we got what we wanted, at least size wise
   if(ReturnedDataLength != sizeof(DWORD))
      return(FALSE);

   DISPDBG((1, "DrvEscape - nvDoValidateDeviceModeEsc PASSED"));

   return(TRUE);

}


BOOL  nvGetDisplayDeviceClass(PDEV *ppdev, ULONG *deviceclass)
{

   ULONG ReturnedDataLength;

   // - Attempt to perform the IOCTL to GET the display device
   if (
       EngDeviceIoControl(
            ppdev->hDriver,
            IOCTL_VIDEO_IS_NVIDIA_BOARD_AND_DRIVER,
            NULL,
            0,
            deviceclass,
            sizeof(ULONG),
            &ReturnedDataLength
      )
   ) {

      // - That call burned, so return fail
      return(FALSE);
   }

   // - Make sure we got what we wanted, at least size wise
   if(ReturnedDataLength != sizeof(ULONG))
      return(FALSE);

   return(TRUE);

}

//
// Handles the hotkeys under NT4.0 for laptops.
// Return Value: FALSE if there was no hotkey pending. TRUE if a hotkey was pending.
// Modifies the 'ulDeviceMask' field with the hotkey device mask if a hotkey was pending. 
// 
//
BOOLEAN bHandleNT4HotKey(PPDEV ppdev, GET_DEVICE_MASK_DATA_TYPE *pInfo)
{
        NV_CFGEX_GET_HOTKEY_EVENT_PARAMS nvHotKeyEvent, nvTmp;
        ULONG ulDeviceMask;
        ULONG i, status;
        UCHAR statusByte;

        //
        // See if a NT4.0 style hotkey request is pending.
        //
        nvHotKeyEvent.status = 0;
        nvHotKeyEvent.event =  0;
        if (NvConfigGetEx (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                 NV_CFGEX_GET_HOTKEY_EVENT, &nvHotKeyEvent, sizeof(NV_CFGEX_GET_HOTKEY_EVENT_PARAMS))) 
        {
            DISPDBG((1, "bHandleNT4HotKey() - Cannot get HOTKEY_EVENT from RM. What to do??"));
            return(FALSE);
        }
        
        //
        // check to see if a hotkey event is really pending
        //
        if (nvHotKeyEvent.event == NV_HOTKEY_EVENT_DISPLAY_ENABLE)
        {
            //
            // Atleast one HK event is there. Extract the device mask.
            //
            ulDeviceMask = 0;
            status = nvHotKeyEvent.status;
            DISPDBG((1,"special device word from RM to be parsed: 0x%x",nvHotKeyEvent.status));
            for (i=0; i < 2; i++)
            {   
                statusByte = (UCHAR)((status >> (i * 8)) & 0xff);
                switch (statusByte)
                {
                case NV_HOTKEY_STATUS_DISPLAY_ENABLE_LCD:
                    ulDeviceMask |= BITMASK_DFP0;
                    break;
                case NV_HOTKEY_STATUS_DISPLAY_ENABLE_CRT:
                    ulDeviceMask |= BITMASK_CRT0;
                    break;
                case NV_HOTKEY_STATUS_DISPLAY_ENABLE_TV:
                    ulDeviceMask |= BITMASK_TV0;
                    break;
                }
            }
            DISPDBG((1, "ulDeviceMask constructed: 0x%x",ulDeviceMask));
            pInfo->ulDeviceMask = ulDeviceMask;
            
            //
            // We now need to drain the event queue of the RM of any other 
            // HKs. This can happen if the user presses Fn+F5 faster than the driver can process.
            // Just as a safety measure, do it only say 50 times before bailing out.
            //
            nvTmp.event = NV_HOTKEY_EVENT_DISPLAY_ENABLE;
            i = 0;
            while (nvTmp.event != NV_HOTKEY_EVENT_NONE_PENDING && i < 50)
            {
                if (NvConfigGetEx (ppdev->hDriver, ppdev->hClient, ppdev->hDevice,
                 NV_CFGEX_GET_HOTKEY_EVENT, &nvTmp, sizeof(NV_CFGEX_GET_HOTKEY_EVENT_PARAMS))) 
                {
                    DISPDBG((1, "bHandleNT4HotKey() - Cannot get tmp HOTKEY_EVENT from RM. What to do??"));
                    break;
                }
                i++;
            }
            DISPDBG((1,"bHandleNT4HotKey(): Successful hotkey event processed")); 
            return(TRUE);
        }
        else
        {
            //
            // Either hotkey is not supported or is not pending.
            //
            DISPDBG((1,"bHandleNT4HotKey(): no hotkey event pending: 0x%x", nvHotKeyEvent.event)); 
            return(FALSE);
        }
}
