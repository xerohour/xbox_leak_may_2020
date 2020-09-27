/*
 * nvRmApi.c
 *
 * NVidia resource manager API.
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
 * Function defined in the scope of this file.
 */
static DWORD GetWindowsType(void);

/*
 ********************************************************************************
 * The following functions are the external, exported API to the resource manager.
 ********************************************************************************
/*
 * NvRmOpen - open connection to resource manager
 */
HANDLE __stdcall NvRmOpen
(
    VOID
)
{
}

/*
 * NvRmClose - close connection with resource manager
 */
void __stdcall NvRmClose
(
    VOID
)
{
}

/*
 * NvAlloc - allocate any arbitrary object
 */
ULONG __stdcall NvRmAlloc
(
    ULONG hClient, 
    ULONG hParent, 
    ULONG hObject, 
    ULONG hClass, 
    PVOID pAllocParms
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmAllocWin9x(hClient, hParent, hObject, hClass, pAllocParms));
    else
        return(NvRmAllocWinNt(hClient, hParent, hObject, hClass, pAllocParms));
}

/*
 * NvAllocRoot - allocate root of device tree
 */
ULONG __stdcall NvRmAllocRoot
(
    ULONG *phClient
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmAllocRootWin9x(phClient));
    else
        return(NvRmAllocRootWinNt(phClient));
}

/*
 * NvAllocDevice - allocate a device.
 */
ULONG __stdcall NvRmAllocDevice
(
    ULONG hClient,
    ULONG hDevice,
    ULONG hClass,
    PUCHAR szName
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmAllocDeviceWin9x(hClient, hDevice, hClass, szName));
    else
        return(NvRmAllocDeviceWinNt(hClient, hDevice, hClass, szName));
}

/*
 * NvAllocContextDma - allocate and lock down memory using the resource manager.
 */
ULONG __stdcall NvRmAllocContextDma
(
    ULONG hClient,
    ULONG hDma,
    ULONG hClass,
    ULONG flags,
    PVOID base,
    ULONG limit
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmAllocContextDmaWin9x(hClient, hDma, hClass, flags, base, limit));
    else
        return(NvRmAllocContextDmaWinNt(hClient, hDma, hClass, flags, base, limit));
}

/*
 * AllocChannelPio - allocate a PIO channel by calling RM.
 */
ULONG __stdcall NvRmAllocChannelPio
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
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmAllocChannelPioWin9x(hClient, hDevice, hChannel, hClass, hErrorCtx, ppChannel, flags));
    else
        return(NvRmAllocChannelPioWinNt(hClient, hDevice, hChannel, hClass, hErrorCtx, ppChannel, flags));
}

/*
 * NvAllocChannelDma - allocate a DMA push channel using RM.
 */
ULONG __stdcall NvRmAllocChannelDma
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
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmAllocChannelDmaWin9x(hClient, hDevice, hChannel, hClass, hErrorCtx, hDataCtx, offset, ppChannel));
    else
        return(NvRmAllocChannelDmaWinNt(hClient, hDevice, hChannel, hClass, hErrorCtx, hDataCtx, offset, ppChannel));
}

/*
 * NvAllocMemory - allocate and lock down a mess 'o memory using RM.
 */
ULONG __stdcall NvRmAllocMemory
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
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmAllocMemoryWin9x(hClient, hParent, hMemory, hClass, flags, ppAddress, pLimit));
    else
        return(NvRmAllocMemoryWinNt(hClient, hParent, hMemory, hClass, flags, ppAddress, pLimit));
}

/*
 * NvAllocObject - allocate a object using RM.
 */
ULONG __stdcall NvRmAllocObject
(
    ULONG hClient,
    ULONG hChannel,
    ULONG hObject,
    ULONG hClass
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmAllocObjectWin9x(hClient, hChannel, hObject, hClass));
    else
        return(NvRmAllocObjectWinNt(hClient, hChannel, hObject, hClass));
}

/*
 * NvFree - free a "thing" allocated using RM.
 */
ULONG __stdcall NvRmFree
(
    ULONG hClient,
    ULONG hParent,
    ULONG hObject
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmFreeWin9x(hClient, hParent, hObject));
    else
        return(NvRmFreeWinNt(hClient, hParent, hObject));
}

/*
 * NV3GetDmaPutInfo - These functions get the channel ID, TLB PT base and address space
 * from the resource manager.  This is a backdoor to program the DMA push registers.
 */
ULONG __stdcall Nv3RmGetDmaPushInfo
(
    ULONG hClient,
    ULONG hDevice,
    ULONG hChannel,
    ULONG hDmaContext,
    ULONG retArray
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(Nv3RmGetDmaPushInfoWin9x(hClient, hDevice, hChannel, hDmaContext, retArray));
    else
        return(Nv3RmGetDmaPushInfoWinNt(hClient, hDevice, hChannel, hDmaContext, retArray));
}

ULONG __stdcall NvRmAllocEvent
(
    ULONG hClient,
    ULONG hObjectParent,
    ULONG hObjectNew,
    ULONG hClass,
    ULONG index,
    ULONG data
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmAllocEventWin9x(hClient, hObjectParent, hObjectNew, hClass, index, data));
    else
        return(NvRmAllocEventWinNt(hClient, hObjectParent, hObjectNew, hClass, index, data));
}


ULONG __stdcall NvRmArchHeap
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
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmArchHeapWin9x(hClient, hParent, function, owner, type,
                                   height, size, pitch, offset, address, limit, free, total));
    else
        return(NvRmArchHeapWinNt(hClient, hParent, function, owner, type,
                                   height, size, pitch, offset, address, limit, free, total));
}

ULONG  __stdcall NvRmConfigVersion
(
    ULONG hClient,
    ULONG hDevice,
    ULONG *pVersion
)
{
    return NVOS12_STATUS_SUCCESS;
}

ULONG  __stdcall NvRmConfigGet
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    ULONG *pValue
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmConfigGetWin9x(hClient, hDevice, index, pValue));
    else
        return(NvRmConfigGetWinNt(hClient, hDevice, index, pValue));
}

ULONG  __stdcall NvRmConfigSet
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    ULONG newValue,
    ULONG *pOldValue
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmConfigSetWin9x(hClient, hDevice, index, newValue, pOldValue));
    else
        return(NvRmConfigSetWinNt(hClient, hDevice, index, newValue, pOldValue));
}

ULONG  __stdcall NvRmConfigUpdate
(
    ULONG hClient,
    ULONG hDevice,
    ULONG flags
)
{
    return NVOS15_STATUS_SUCCESS;
}

ULONG  __stdcall NvRmConfigGetEx
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    VOID  *paramStructPtr,
    ULONG paramSize
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmConfigGetExWin9x(hClient, hDevice, index, paramStructPtr, paramSize));
    else
        return(NvRmConfigGetExWinNt(hClient, hDevice, index, paramStructPtr, paramSize));
}

ULONG  __stdcall NvRmConfigSetEx
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    VOID  *paramStructPtr,
    ULONG paramSize
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmConfigSetExWin9x(hClient, hDevice, index, paramStructPtr, paramSize));
    else
        return(NvRmConfigSetExWinNt(hClient, hDevice, index, paramStructPtr, paramSize));
}

ULONG  __stdcall NvRmInterrupt
(
    ULONG hClient,
    ULONG hDevice
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmInterruptWin9x(hClient, hDevice));
    else
        return(NvRmInterruptWinNt(hClient, hDevice));
}

ULONG  __stdcall NvRmRing0Callback
(
    ULONG hClient,
    ULONG hDevice,
    ULONG procAddr,
    ULONG param1,
    ULONG param2
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmRing0CallbackWin9x(hClient, hDevice, procAddr, param1, param2));
    else
        return(NvRmRing0CallbackWinNt(hClient, hDevice, procAddr, param1, param2));
}

ULONG  __stdcall NvRmI2CAccess
(
    ULONG hClient,
    ULONG hDevice,
    VOID  *paramStructPtr
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return(NvRmI2CAccessWin9x(hClient, hDevice, paramStructPtr ));
    else
        return(NvRmI2CAccessWinNt(hClient, hDevice, paramStructPtr ));
}

ULONG  __stdcall NvRmDebugControl
(
    ULONG hRoot,
    ULONG command,
    VOID *pArgs
)
{
    if (NV_WIN_IS_9X(GetWindowsType()))
        return NvRmDebugControlWin9x(hRoot, command, pArgs);
    else
        return NvRmDebugControlWinNt(hRoot, command, pArgs);
}

/*
 * GetWindowsType - Use Win32 to get windows type (9x or NT)
 */
static DWORD
GetWindowsType
(
    void
)
{
    OSVERSIONINFO osVer; /* for GetVersionEx() */

    /*
     * Get Windows type: 9x or NT
     */
    memset(&osVer, 0, sizeof(osVer));
    osVer.dwOSVersionInfoSize = sizeof(osVer);
    (void)GetVersionEx(&osVer);
    return(osVer.dwPlatformId);
}

// end of nvRmApi.c
