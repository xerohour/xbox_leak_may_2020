/*
 * nvRmtApiNt.c
 *
 * NVidia resource manager API implementation for Windows NT.
 *
 * Copyright (c) 1997, Nvidia Corporation.  All rights reserved.
 * Portions Copyright (c) 1996, 1997, Silicon Graphics Inc.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

#include <windows.h>
#include <nvEscDef.h>
#include "nvos.h"
#include "nv32.h"
#include "nvRmInt.h"
#include "files.h"
#include "nvmisc.h"
#include "nvcm.h"
#include "nvReg.h"

/*
 * pragmas
 */
#pragma warning(disable : 4101 4035)

/*
 * NvRmOpen - open connection to resource manager
 */

HANDLE NvRmOpenWinNt
(
    VOID
)
{
    HDC   hDC;
    int   ret;
    DWORD in, out;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return (HANDLE)-1;
    }

    in  = (DWORD)0;
    ret = ExtEscape(hDC,
                    NV_ESC_RM_OPEN,
                    sizeof(DWORD), (LPCSTR)&in,
                    sizeof(DWORD), (LPSTR)&out);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return (HANDLE)-1;
    }

    ReleaseDC(0, hDC);
    return (out == (DWORD)INVALID_HANDLE_VALUE) ? (HANDLE)-1 : (HANDLE) 0;
}

/*
 * NvRmClose - close connection with resource manager
 */
VOID NvRmCloseWinNt
(
    VOID
)
{
    HDC   hDC;
    int   ret;
    DWORD in, out;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return;
    }

    in  = (DWORD)0;
    ret = ExtEscape(hDC,
                    NV_ESC_RM_CLOSE,
                    sizeof(DWORD), (LPCSTR)&in,
                    sizeof(DWORD), (LPSTR)&out);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return;
    }

    ReleaseDC(0, hDC);
    return;
}

/*
 * NvRmAlloc - allocate arbitrary objects
 */
ULONG NvRmAllocWinNt
(
    ULONG hClient, 
    ULONG hParent, 
    ULONG hObject, 
    ULONG hClass, 
    PVOID pAllocParms
)
{
    HDC   hDC;
    int   ret;
    NVOS21_PARAMETERS inParms, outParms;

    // set input parameters
    inParms.hRoot                 = (NvV32)hClient;
    inParms.hObjectParent         = (NvV32)hParent;
    inParms.hObjectNew            = (NvV32)hObject;
    inParms.hClass                = (NvV32)hClass;
    inParms.pAllocParms           = (NvP64)pAllocParms;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS21_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_ALLOC,
                    sizeof(NVOS21_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS21_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS21_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ReleaseDC(0, hDC);
    return outParms.status;
}

/*
 * NvAllocRoot - allocate root of device tree
 */
ULONG NvRmAllocRootWinNt
(
    ULONG *phClient
)
{
    HDC   hDC;
    int   ret;
    NVOS01_PARAMETERS inParms, outParms;

    *phClient = 0;

    inParms.hClass = NV01_ROOT;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS01_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_ALLOC_ROOT,
                    sizeof(NVOS01_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS01_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS01_STATUS_ERROR_OPERATING_SYSTEM);
    }

    *phClient = (ULONG)outParms.hObjectNew;

    ReleaseDC(0, hDC);
    return outParms.status;
}

/*
 * NvAllocDevice - allocate a device.
 */
ULONG NvRmAllocDeviceWinNt
(
    ULONG hClient,
    ULONG hDevice,
    ULONG hClass,
    PUCHAR szName
)
{
    HDC   hDC;
    int   ret;
    NVOS06_PARAMETERS inParms, outParms;

    inParms.hObjectParent   = (NvV32)hClient;
    inParms.hObjectNew      = (NvV32)hDevice;
    inParms.hClass          = (NvV32)hClass;
    inParms.szName          = (NvV32)szName;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS06_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_ALLOC_DEVICE,
                    sizeof(NVOS06_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS06_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS06_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ReleaseDC(0, hDC);
    return outParms.status;
}

/*
 * NvAllocContextDma - allocate and lock down memory using the resource manager.
 */
ULONG NvRmAllocContextDmaWinNt
(
    ULONG hClient,
    ULONG hDma,
    ULONG hClass,
    ULONG flags,
    PVOID base,
    ULONG limit
)
{
    HDC   hDC;
    int   ret;
    NVOS03_PARAMETERS inParms, outParms;

    inParms.hObjectParent   = (NvV32)hClient;
    inParms.hObjectNew      = (NvV32)hDma;
    inParms.hClass          = (NvV32)hClass;
    inParms.flags           = (NvV32)flags;
    inParms.pBase           = (NvP64)base;
    inParms.limit           = (NvU64)limit;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS03_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_ALLOC_CONTEXT_DMA,
                    sizeof(NVOS03_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS03_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS03_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ReleaseDC(0, hDC);
    return outParms.status;
}

/*
 * AllocChannelPio - allocate a PIO channel by calling RM.
 */
ULONG NvRmAllocChannelPioWinNt
(
    ULONG hClient,
    ULONG hDevice,
    ULONG hChannel,
    ULONG hClass,
    ULONG hErrorCtx,
    PVOID *ppChannel,
    ULONG flags
)
{
    HDC   hDC;
    int   ret;
    NVOS04_PARAMETERS inParms, outParms;

    inParms.hRoot           = (NvV32)hClient;
    inParms.hObjectParent   = (NvV32)hDevice;
    inParms.hObjectNew      = (NvV32)hChannel;
    inParms.hClass          = (NvV32)hClass;
    inParms.hObjectError    = (NvV32)hErrorCtx;
    inParms.flags           = (NvV32)flags;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS04_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_ALLOC_CHANNEL_PIO,
                    sizeof(NVOS04_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS04_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS04_STATUS_ERROR_OPERATING_SYSTEM);
    }

    *ppChannel = (PVOID)outParms.pChannel;

    ReleaseDC(0, hDC);
    return outParms.status;
}

/*
 * NvAllocChannelDma - allocate a DMA push channel using RM.
 */
ULONG NvRmAllocChannelDmaWinNt
(
    ULONG hClient,
    ULONG hDevice,
    ULONG hChannel,
    ULONG hClass,
    ULONG hErrorCtx,
    ULONG hDataCtx,
    ULONG offset,
    PVOID *ppChannel
)
{
    HDC   hDC;
    int   ret;
    NVOS07_PARAMETERS inParms, outParms;

    inParms.hRoot           = (NvV32)hClient;
    inParms.hObjectParent   = (NvV32)hDevice;
    inParms.hObjectNew      = (NvV32)hChannel;
    inParms.hClass          = (NvV32)hClass;
    inParms.hObjectError    = (NvV32)hErrorCtx;
    inParms.hObjectBuffer   = (NvV32)hDataCtx;
    inParms.offset          = (NvV32)offset;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS07_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_ALLOC_CHANNEL_DMA,
                    sizeof(NVOS07_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS07_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS07_STATUS_ERROR_OPERATING_SYSTEM);
    }

    *ppChannel = (PVOID)outParms.pControl;

    ReleaseDC(0, hDC);
    return outParms.status;
}

/*
 * NvAllocMemory - allocate and lock down a mess 'o memory using RM.
 */
ULONG NvRmAllocMemoryWinNt
(
    ULONG hClient,
    ULONG hParent,
    ULONG hMemory,
    ULONG hClass,
    ULONG flags,
    PVOID *ppAddress,
    ULONG *pLimit
)
{
    HDC   hDC;
    int   ret;
    NVOS02_PARAMETERS inParms, outParms;

    // set input parameters
    inParms.hRoot               = (NvV32)hClient;
    inParms.hObjectParent       = (NvV32)hParent;
    inParms.hObjectNew          = (NvV32)hMemory;
    inParms.hClass              = (NvV32)hClass;
    inParms.flags               = (NvV32)flags;
    inParms.pMemory             = (NvP64)*ppAddress;
    inParms.pLimit              = (NvU64)*pLimit;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS02_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_ALLOC_MEMORY,
                    sizeof(NVOS02_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS02_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS02_STATUS_ERROR_OPERATING_SYSTEM);
    }

    // retrieve output parameters
    *ppAddress = (PVOID)outParms.pMemory;
    *pLimit    = (ULONG)outParms.pLimit;

    ReleaseDC(0, hDC);
    return outParms.status;
}

/*
 * NvAllocObject - allocate a object using RM.
 */
ULONG NvRmAllocObjectWinNt
(
    ULONG hClient,
    ULONG hChannel,
    ULONG hObject,
    ULONG hClass
)
{
    HDC   hDC;
    int   ret;
    NVOS05_PARAMETERS inParms, outParms;

    // set input parameters
    inParms.hRoot           = (NvV32)hClient;
    inParms.hObjectParent   = (NvV32)hChannel;
    inParms.hObjectNew      = (NvV32)hObject;
    inParms.hClass          = (NvV32)hClass;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS05_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_ALLOC_OBJECT,
                    sizeof(NVOS05_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS05_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS05_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ReleaseDC(0, hDC);
    return outParms.status;
}

/*
 * NvFree - free a "thing" allocated using RM.
 */
ULONG NvRmFreeWinNt
(
    ULONG hClient,
    ULONG hParent,
    ULONG hObject
)
{
    HDC   hDC;
    int   ret;
    NVOS00_PARAMETERS inParms, outParms;

    inParms.hRoot           = (NvV32)hClient;
    inParms.hObjectParent   = (NvV32)hParent;
    inParms.hObjectOld      = (NvV32)hObject;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS00_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_FREE,
                    sizeof(NVOS00_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS00_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS00_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ReleaseDC(0, hDC);
    return(outParms.status);
}

/*
 * NVGetDmaPutInfo - These functions get the channel ID, TLB PT base and address space
 * from the resource manager.  This is a backdoor to program the DMA push registers.
 */
ULONG Nv3RmGetDmaPushInfoWinNt
(
    ULONG hClient,  // this is client NOT channel
    ULONG hDevice,  // this is device NOT flags
    ULONG hChannel, // this is channel NOT put
    ULONG hDmaContext, // this is the DMA context for the push buffer
    ULONG retArray
)
{
    HDC   hDC;
    int   ret;
    ULONG inParms[5], outParms[5];

    inParms[0] = (NvV32)hClient;
    inParms[1] = (NvV32)hDevice;
    inParms[2] = (NvV32)hChannel;
    inParms[3] = (NvV32)retArray;
    inParms[4] = (NvV32)hDmaContext;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS08_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_DMA_PUSH_INFO,
                    sizeof(inParms), (LPCSTR)&inParms,
                    sizeof(outParms), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS08_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ReleaseDC(0, hDC);
    return(outParms[4]);
}

ULONG NvRmAllocEventWinNt
(
    ULONG hClient,
    ULONG hObjectParent,
    ULONG hObjectNew,
    ULONG hClass,
    ULONG index,
    ULONG data
)
{
    // This is not supported yet.
    return NVOS10_STATUS_ERROR_OPERATING_SYSTEM;
}

ULONG NvRmArchHeapWinNt
(
    ULONG hClient,
    ULONG hParent,
    ULONG function,
    ULONG owner,
    ULONG type,
    ULONG height,
    ULONG size,
    ULONG *pitch,
    ULONG *offset,
    ULONG *address,
    ULONG *limit,
    ULONG *free,
    ULONG *total
)
{
    HDC   hDC;
    int   ret;
    NVOS11_PARAMETERS inParms, outParms;

    inParms.hRoot         = (NvV32)hClient;
    inParms.hObjectParent = (NvV32)hParent;
    inParms.function      = (NvU32)function;
    inParms.owner         = (NvU32)owner;
    inParms.type          = (NvU32)type;
    inParms.pitch         = (NvS32)((pitch) ? *pitch : 0x0);
    inParms.height        = (NvU32)height;
    inParms.size          = (NvU32)size;
    inParms.offset        = (NvU32)*offset;


    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS11_STATUS_ERROR_INSUFFICIENT_RESOURCES);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_ARCH_HEAP,
                    sizeof(NVOS11_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS11_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS11_STATUS_ERROR_INSUFFICIENT_RESOURCES);
    }

    if (pitch)
        *pitch   = (ULONG)outParms.pitch;
    *offset  = (ULONG)outParms.offset;
    *address = (ULONG)outParms.address;
    *limit   = (ULONG)outParms.limit;
    *free    = (ULONG)outParms.free;
    *total   = (ULONG)outParms.total;

    ReleaseDC(0, hDC);
    return outParms.status;
}

ULONG NvRmConfigGetWinNt
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    ULONG *pValue
)
{
    HDC   hDC;
    int   ret;
    NVOS13_PARAMETERS inParms, outParms;

    inParms.hClient         = (NvV32)hClient;
    inParms.hDevice         = (NvV32)hDevice;
    inParms.index           = (NvV32)index;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS13_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_CONFIG_GET,
                    sizeof(NVOS13_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS13_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS13_STATUS_ERROR_OPERATING_SYSTEM);
    }

    *pValue = (ULONG)outParms.value;

    ReleaseDC(0, hDC);
    return outParms.status;
}

ULONG NvRmConfigSetWinNt
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    ULONG newValue,
    ULONG *pOldValue
)
{
    HDC   hDC;
    int   ret;
    NVOS14_PARAMETERS inParms, outParms;

    inParms.hClient         = (NvV32)hClient;
    inParms.hDevice         = (NvV32)hDevice;
    inParms.index           = (NvV32)index;
    inParms.newValue        = (NvV32)newValue;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS14_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_CONFIG_SET,
                    sizeof(NVOS14_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS14_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS14_STATUS_ERROR_OPERATING_SYSTEM);
    }

    *pOldValue = (ULONG)outParms.oldValue;

    ReleaseDC(0, hDC);
    return outParms.status;
}

ULONG NvRmConfigGetExWinNt
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    VOID *paramStructPtr,
    ULONG paramSize
)
{
    HDC   hDC;
    int   ret;
    NVOS_CONFIG_GET_EX_PARAMS inParms, outParms;

    inParms.hClient         = (NvV32)hClient;
    inParms.hDevice         = (NvV32)hDevice;
    inParms.index           = (NvV32)index;
    inParms.paramStructPtr  = (NvV32)paramStructPtr;
    inParms.paramSize       = (NvU32)paramSize;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS_CGE_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_CONFIG_GET_EX,
                    sizeof(NVOS_CONFIG_GET_EX_PARAMS), (LPCSTR)&inParms,
                    sizeof(NVOS_CONFIG_GET_EX_PARAMS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS_CGE_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ReleaseDC(0, hDC);
    return outParms.status;
}

ULONG NvRmConfigSetExWinNt
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    VOID *paramStructPtr,
    ULONG paramSize
)
{
    HDC   hDC;
    int   ret;
    NVOS_CONFIG_SET_EX_PARAMS inParms, outParms;

    inParms.hClient         = (NvV32)hClient;
    inParms.hDevice         = (NvV32)hDevice;
    inParms.index           = (NvV32)index;
    inParms.paramStructPtr  = (NvV32)paramStructPtr;
    inParms.paramSize       = (NvU32)paramSize;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return(NVOS_CSE_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_CONFIG_SET_EX,
                    sizeof(NVOS_CONFIG_SET_EX_PARAMS), (LPCSTR)&inParms,
                    sizeof(NVOS_CONFIG_SET_EX_PARAMS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return(NVOS_CSE_STATUS_ERROR_OPERATING_SYSTEM);
    }

    ReleaseDC(0, hDC);
    return outParms.status;
}

ULONG NvRmInterruptWinNt
(
    ULONG hClient,
    ULONG hDevice
)
{
    return NVOS09_STATUS_SUCCESS;
}

ULONG NvRmRing0CallbackWinNt
(
    ULONG hClient,
    ULONG hDevice,
    ULONG procAddr,
    ULONG param1,
    ULONG param2
)
{
    return NVOS09_STATUS_SUCCESS;
}

ULONG NvRmI2CAccessWinNt
(
    ULONG hClient,
    ULONG hDevice,
    VOID *paramStructPtr
)
{
    NVOS_I2C_ACCESS_PARAMS inParams,outParams;
    HDC   hDC;
    int   ret;

    inParams.hClient         = (NvV32)hClient;
    inParams.hDevice         = (NvV32)hDevice;
    inParams.paramStructPtr  = (NvV32)paramStructPtr;
    inParams.paramSize       = (NvV32)sizeof(NVRM_I2C_ACCESS_CONTROL);
    inParams.status          = (NvV32)0;

    hDC = GetDC(0);
    if (hDC == NULL) {
       return NVOS_I2C_ACCESS_STATUS_ERROR_OPERATING_SYSTEM;
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_I2C_ACCESS,
                    sizeof(NVOS_I2C_ACCESS_PARAMS), (LPSTR)&inParams,
                    sizeof(NVOS_I2C_ACCESS_PARAMS), (LPSTR)&outParams);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return NVOS_I2C_ACCESS_STATUS_ERROR_OPERATING_SYSTEM;
    }
    ReleaseDC(0, hDC);

    return (ULONG)outParams.status;
    
}

ULONG NvRmDebugControlWinNt(
    ULONG hRoot,
    ULONG command,
    VOID *pArgs
)
{
    NVOS20_PARAMETERS inParms, outParms;
    HDC   hDC;
    int   ret;

    inParms.hRoot = hRoot;
    inParms.command = command;
    inParms.pArgs = (NvP64) pArgs;

    hDC = GetDC(0);
    if (hDC == NULL) {
        return NVOS20_STATUS_ERROR_OPERATING_SYSTEM;
    }

    ret = ExtEscape(hDC,
                    NV_ESC_RM_DEBUG_CONTROL,
                    sizeof(NVOS20_PARAMETERS), (LPCSTR)&inParms,
                    sizeof(NVOS20_PARAMETERS), (LPSTR)&outParms);

    if (ret <= 0) {
        ReleaseDC(0, hDC);
        return NVOS20_STATUS_ERROR_OPERATING_SYSTEM;
    }
    ReleaseDC(0, hDC);

    return outParms.status;
}

// end of nvRmApiNt.c
