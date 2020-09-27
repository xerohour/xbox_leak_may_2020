/*
 * nvRmApi9x.c
 *
 * NVidia resource manager API for Windows 9x.
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
 * Global variables local to the scope of this file.
 */
/*
 * Although Win32 applications have a flat address space, on the X86
 * architecture they still have a single selector that you must supply as
 *    contextDmaToMemory.SetDmaSpecifier.address[1].
 */
int GetFlatDataSelector(void)
{
    DWORD Selector;

    _asm xor eax, eax
    _asm mov ax, ds
    _asm mov Selector, eax
    return (Selector);
}

/*
 * NvRmOpen - open connection to resource manager
 */
HANDLE NvRmOpenWin9x
(
    VOID
)
{
}

/*
 * NvRmClose - close connection with resource manager
 */
VOID NvRmCloseWin9x
(
	HANDLE IOCTL_hDevice
)
{
}

/*
 * NvRmAlloc - allocate arbitrary objects
 */
ULONG NvRmAllocWin9x
(
    ULONG hClient, 
    ULONG hParent, 
    ULONG hObject, 
    ULONG hClass, 
    PVOID pAllocParms
)
{
    NVOS21_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    // set input parameters
    parms.hRoot                 = (NvV32)hClient;
    parms.hObjectParent         = (NvV32)hParent;
    parms.hObjectNew            = (NvV32)hObject;
    parms.hClass                = (NvV32)hClass;
    parms.pAllocParms.selector  = GetFlatDataSelector();
    parms.pAllocParms.offset    = (NvU32)pAllocParms;

    t_arch.function    = (DWORD)NVRM_ARCH_NV04ALLOC;
    t_arch.pParameters = (DWORD)&parms;
	NvIoControl(t_arch.function,(VOID *) &parms);
    
    return (ULONG)parms.status;
}

/*
 * NvAllocRoot - allocate root of device tree
 */
ULONG NvRmAllocRootWin9x
(
    ULONG *phClient
)
{
    NVOS01_PARAMETERS parms;
    NvIoctlArch       t_arch;
	HANDLE            IOCTL_hDevice;

    parms.hClass = NV01_ROOT;

    t_arch.function     = (DWORD)NVRM_ARCH_NV01ALLOCROOT;
    t_arch.pParameters  = (DWORD)&parms;

    NvIoControl(t_arch.function,(VOID *) &parms);

    *phClient = (ULONG)parms.hObjectNew;

    return (ULONG)parms.status;
}

/*
 * NvAllocDevice - allocate a device.
 */
ULONG NvRmAllocDeviceWin9x
(
    ULONG hClient,
    ULONG hDevice,
    ULONG hClass,
    PUCHAR szName
)
{
    NVOS06_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hObjectParent = hClient;
    parms.hObjectNew    = hDevice;
    parms.hClass        = hClass;
    parms.szName        = (NvV32)szName;

    t_arch.function     = (DWORD)NVRM_ARCH_NV01ALLOCDEVICE;
    t_arch.pParameters  = (DWORD)&parms;
    
    NvIoControl(t_arch.function,(VOID *) &parms);

    return((DWORD)parms.status);
}

/*
 * NvAllocContextDma - allocate and lock down memory using the resource manager.
 */
ULONG NvRmAllocContextDmaWin9x
(
    ULONG hClient,
    ULONG hDma,
    ULONG hClass,
    ULONG flags,
    PVOID base,
    ULONG limit
)
{
    NVOS03_PARAMETERS parms;
    NvIoctlArch t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hObjectParent  = hClient;
    parms.hObjectNew     = hDma;
    parms.hClass         = hClass;
    parms.flags          = flags;
    parms.pBase.selector = GetFlatDataSelector();
    parms.pBase.offset   = (DWORD)base;
    parms.limit.low      = limit;

    t_arch.function    = (DWORD)NVRM_ARCH_NV01ALLOCCONTEXTDMA;
    t_arch.pParameters = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    return((DWORD)parms.status);
}

/*
 * AllocChannelPio - allocate a PIO channel by calling RM.
 */
ULONG NvRmAllocChannelPioWin9x
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
    NVOS04_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hRoot         = hClient;
    parms.hObjectParent = hDevice;
    parms.hObjectNew    = hChannel;
    parms.hClass        = hClass;
    parms.hObjectError  = hErrorCtx;
    parms.flags         = flags;

    t_arch.function    = (DWORD)NVRM_ARCH_NV01ALLOCCHANNELPIO;
    t_arch.pParameters = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    *ppChannel = (PVOID)parms.pChannel.offset;

    return((DWORD) parms.status);
}

/*
 * NvAllocChannelDma - allocate a DMA push channel using RM.
 */
ULONG NvRmAllocChannelDmaWin9x
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
    NVOS07_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hRoot         = (NvV32)hClient;
    parms.hObjectParent = (NvV32)hDevice;
    parms.hObjectNew    = (NvV32)hChannel;
    parms.hClass        = (NvV32)hClass;
    parms.hObjectError  = (NvV32)hErrorCtx;
    parms.hObjectBuffer = (NvV32)hDataCtx;
    parms.offset        = (NvV32)offset;

    t_arch.function    = (DWORD)NVRM_ARCH_NV03ALLOCCHANNELDMA;
    t_arch.pParameters = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    *ppChannel = (PVOID)parms.pControl.offset;

    return((DWORD)parms.status);
}

/*
 * NvAllocMemory - allocate and lock down a mess 'o memory using RM.
 */
ULONG NvRmAllocMemoryWin9x
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
    NVOS02_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    // set input parameters
    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hParent;
    parms.hObjectNew        = (NvV32)hMemory;
    parms.hClass            = (NvV32)hClass;
    parms.flags             = (NvV32)flags;
    parms.pMemory.selector  = GetFlatDataSelector();
    parms.pMemory.offset    = (NvU32)*ppAddress;
    parms.pLimit.high       = 0x00000000;
    parms.pLimit.low        = (NvU32)*pLimit;

    t_arch.function    = (DWORD)NVRM_ARCH_NV01ALLOCMEMORY;
    t_arch.pParameters = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    // retrieve output parameters
    *ppAddress = (PVOID)parms.pMemory.offset;
    *pLimit    = (ULONG)parms.pLimit.low;

    return (ULONG)parms.status;
}

/*
 * NvAllocObject - allocate a object using RM.
 */
ULONG NvRmAllocObjectWin9x
(
    ULONG hClient,
    ULONG hChannel,
    ULONG hObject,
    ULONG hClass
)
{
    NVOS05_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    // set input parameters
    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hChannel;
    parms.hObjectNew        = (NvV32)hObject;
    parms.hClass            = (NvV32)hClass;

    t_arch.function    = (DWORD)NVRM_ARCH_NV01ALLOCOBJECT;
    t_arch.pParameters = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    return (ULONG)parms.status;
}

/*
 * NvFree - free a "thing" allocated using RM.
 */
ULONG NvRmFreeWin9x
(
    ULONG hClient,
    ULONG hParent,
    ULONG hObject
)
{
    NVOS00_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hRoot         = hClient;
    parms.hObjectParent = hParent;
    parms.hObjectOld    = hObject;

    t_arch.function    = (DWORD)NVRM_ARCH_NV01FREE;
    t_arch.pParameters = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    return((DWORD)parms.status);
}

/*
 * NVGetDmaPutInfo - These functions get the channel ID, TLB PT base and address space
 * from the resource manager.  This is a backdoor to program the DMA push registers.
 */
ULONG Nv3RmGetDmaPushInfoWin9x
(
    ULONG hClient,  // this is client NOT channel
    ULONG hDevice,  // this is device NOT flags
    ULONG hChannel, // this is channel NOT put
    ULONG hDmaContext, // this is the DMA context for the push buffer
    ULONG retArray
)
{
    ULONG       parms[5];
    NvIoctlArch t_arch;
    HANDLE            IOCTL_hDevice;

    parms[0] = (NvV32)hClient;
    parms[1] = (NvV32)hDevice;
    parms[2] = (NvV32)hChannel;
    parms[3] = (NvV32)retArray;
    parms[4] = (NvV32)hDmaContext;

    t_arch.function    = (DWORD)NVRM_ARCH_NV03DMAFLOWCONTROL;
    t_arch.pParameters = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    return(parms[4]);
}

ULONG NvRmAllocEventWin9x
(
    ULONG hClient,
    ULONG hObjectParent,
    ULONG hObjectNew,
    ULONG hClass,
    ULONG index,
    ULONG data
)
{
    NVOS10_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hRoot         = hClient;
    parms.hObjectParent = hObjectParent;
    parms.hObjectNew    = hObjectNew;
    parms.hClass        = hClass;
    parms.index         = index;
    parms.hEvent.low    = data;

    t_arch.function    = (DWORD)NVRM_ARCH_NV01ALLOCEVENT;
    t_arch.pParameters = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    return((DWORD)parms.status);

}

ULONG NvRmArchHeapWin9x
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
    NVOS11_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hRoot         = hClient;
    parms.hObjectParent = hParent;
    parms.function      = function;
    parms.owner         = owner;
    parms.type          = type;
    parms.pitch         = (pitch) ? *pitch : 0x0;
    parms.height        = height;
    parms.size          = size;
    parms.offset        = *offset;
    t_arch.function    = (DWORD)NVRM_ARCH_NV01HEAP;
    t_arch.pParameters = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    if (pitch)
        *pitch   = parms.pitch;
    *offset  = parms.offset;
    *address = parms.address;
    *limit   = parms.limit;
    *total   = parms.total;
    *free    = parms.free;
    return((DWORD)parms.status);

}

ULONG NvRmConfigGetWin9x
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    ULONG *pValue
)
{
    NVOS13_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hClient       = (NvV32)hClient;
    parms.hDevice       = (NvV32)hDevice;
    parms.index         = (NvV32)index;

    t_arch.function     = (DWORD)NVRM_ARCH_NV01CONFIGGET;
    t_arch.pParameters  = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    *pValue = (ULONG)parms.value;

    return (ULONG)parms.status;
}

ULONG NvRmConfigSetWin9x
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    ULONG newValue,
    ULONG *pOldValue
)
{
    NVOS14_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hClient       = (NvV32)hClient;
    parms.hDevice       = (NvV32)hDevice;
    parms.index         = (NvV32)index;
    parms.newValue      = (NvV32)newValue;

    t_arch.function     = (DWORD)NVRM_ARCH_NV01CONFIGSET;
    t_arch.pParameters  = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    *pOldValue = (ULONG)parms.oldValue;

    return (ULONG)parms.status;
}

ULONG NvRmConfigGetExWin9x
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    VOID *paramStructPtr,
    ULONG paramSize
)
{
    NVOS_CONFIG_GET_EX_PARAMS parms;
    NvIoctlArch               t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hClient         = (NvV32)hClient;
    parms.hDevice         = (NvV32)hDevice;
    parms.index           = (NvV32)index;
    parms.paramStructPtr  = (NvV32)paramStructPtr;
    parms.paramSize       = (NvU32)paramSize;

    t_arch.function     = (DWORD)NVRM_ARCH_NV04CONFIGGETEX;
    t_arch.pParameters  = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    return (ULONG)parms.status;
}

ULONG NvRmConfigSetExWin9x
(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    VOID *paramStructPtr,
    ULONG paramSize
)
{
    NVOS_CONFIG_SET_EX_PARAMS parms;
    NvIoctlArch               t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hClient         = (NvV32)hClient;
    parms.hDevice         = (NvV32)hDevice;
    parms.index           = (NvV32)index;
    parms.paramStructPtr  = (NvV32)paramStructPtr;
    parms.paramSize       = (NvU32)paramSize;

    t_arch.function     = (DWORD)NVRM_ARCH_NV04CONFIGSETEX;
    t_arch.pParameters  = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    return (ULONG)parms.status;
}

ULONG NvRmInterruptWin9x
(
    ULONG hClient,
    ULONG hDevice
)
{
    NVOS09_PARAMETERS parms;
    NvIoctlArch       t_arch;
    HANDLE            IOCTL_hDevice;

    parms.hClient       = (NvV32)hClient;
    parms.hDevice       = (NvV32)hDevice;

    t_arch.function     = (DWORD)NVRM_ARCH_NV01INTERRUPT;
    t_arch.pParameters  = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    return (ULONG)parms.status;
}

ULONG NvRmRing0CallbackWin9x
(
    ULONG hClient,
    ULONG hDevice,
    ULONG procAddr,
    ULONG param1,
    ULONG param2
)
{
    NVRM_RING0CALLBACK_PARAMS parms;
    NvIoctlArch               t_arch;
    HANDLE                    IOCTL_hDevice;

    parms.hClient       = (NvV32)hClient;
    parms.hDevice       = (NvV32)hDevice;
    parms.functionPtr   = (RING0CALLBACKPROC)procAddr;
    parms.param1        = (NvU32)param1;
    parms.param2        = (NvU32)param2;

    t_arch.function     = (DWORD)NVRM_ARCH_RING0CALLBACK;
    t_arch.pParameters  = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    return (ULONG)parms.status;
}

ULONG NvRmI2CAccessWin9x
(
    ULONG hClient,
    ULONG hDevice,
    VOID *paramStructPtr
)
{
    NVOS_I2C_ACCESS_PARAMS    parms;
    NvIoctlArch               t_arch;
    HANDLE                    IOCTL_hDevice;

    parms.hClient         = (NvV32)hClient;
    parms.hDevice         = (NvV32)hDevice;
    parms.paramStructPtr  = (NvV32)paramStructPtr;

    t_arch.function     = (DWORD)NVRM_ARCH_NV04I2CACCESS;
    t_arch.pParameters  = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);
    
    return (ULONG)parms.status;
}

ULONG NvRmDebugControlWin9x
(
    ULONG hRoot,
    ULONG command,
    VOID *pArgs
)
{
    NVOS20_PARAMETERS         parms;
    NvIoctlArch               t_arch;

    parms.hRoot = hRoot;
    parms.command = command;
    parms.pArgs.selector = GetFlatDataSelector();
    parms.pArgs.offset = (DWORD) pArgs;

    t_arch.function     = (DWORD)NVRM_ARCH_NV01DEBUGCONTROL;
    t_arch.pParameters  = (DWORD)&parms;
    NvIoControl(t_arch.function,(VOID *) &parms);

    return (ULONG)parms.status;
}

// end of nvRmApi9x.c
