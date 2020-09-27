/******************************************************************************
*
*   Module:  nvapi.c
*
*   Description:
*       This module contains the functions that provide the NT display driver
*   access to the resource manager.
*
******************************************************************************/

#include "precomp.h"
#include <nv32.h>
#include <nvos.h>
#include <nvntioctl.h>
#include <nvapi.h>

HANDLE __cdecl NvOpen
(
    HANDLE hDriver
)
{
    DWORD status, cbReturned;

    // connect to the RM thru an IOCTL to the miniport
    status = EngDeviceIoControl(
        hDriver,
        (DWORD)NVAPI_IOCTL_OPEN,
        NULL,
        0,
        NULL,
        0,
        &cbReturned
    );

    return (status == 0) ? hDriver : NVARCH_INVALID_NVDESCRIPTOR;
}

VOID _cdecl NvClose
(
    HANDLE hDriver
)
{
    DWORD cbReturned;

    // disconnect from the RM thru an IOCTL to the miniport
    EngDeviceIoControl(
        hDriver,
        (DWORD)NVAPI_IOCTL_CLOSE,
        NULL,
        0,
        NULL,
        0,
        &cbReturned
    );
}

ULONG __cdecl NvAllocRoot
(
    HANDLE hDriver,
    ULONG  hClass,
    ULONG *phClient
)
{
    NVOS01_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hClass            = hClass;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_ALLOC_ROOT,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS01_PARAMETERS),
        &cbReturned
    );

    *phClient               = (ULONG)parms.hObjectNew;

    return (ULONG)parms.status;

}

ULONG __cdecl NvAllocDevice
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hDevice,
    ULONG hClass,
    PUCHAR szName
)
{
    NVOS06_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hObjectParent     = (NvV32)hClient;
    parms.hObjectNew        = (NvV32)hDevice;
    parms.hClass            = (NvV32)hClass;
    parms.szName            = (NvP64)szName;



    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_ALLOC_DEVICE,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS06_PARAMETERS),
        &cbReturned
    );

    return (ULONG)parms.status;

}

ULONG __cdecl NvAllocContextDma
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hDma,
    ULONG hClass,
    ULONG flags,
    PVOID base,
    ULONG limit
)
{
    NVOS03_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hObjectParent      = (NvV32)hClient;
    parms.hObjectNew         = (NvV32)hDma;
    parms.hClass             = (NvV32)hClass;
    parms.flags              = (NvV32)flags;
    parms.pBase              = (NvP64)base;
    parms.limit              = (NvU64)limit;

    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_ALLOC_CONTEXT_DMA,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS03_PARAMETERS),
        &cbReturned
    );

    return (ULONG)parms.status;

}

ULONG __cdecl NvAllocChannelPio
(
    HANDLE hDriver,
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
    PVOID pParms;
    DWORD cbReturned;

    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hDevice;
    parms.hObjectNew        = (NvV32)hChannel;
    parms.hClass            = (NvV32)hClass;
    parms.hObjectError      = (NvV32)hErrorCtx;
    parms.flags             = (NvV32)flags;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_ALLOC_CHANNEL_PIO,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS04_PARAMETERS),
        &cbReturned
    );

    *ppChannel = (PVOID)parms.pChannel;

    return (ULONG)parms.status;

}

ULONG __cdecl NvAllocChannelDma
(
    HANDLE hDriver,
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
    PVOID pParms;
    DWORD cbReturned;

    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hDevice;
    parms.hObjectNew        = (NvV32)hChannel;
    parms.hClass            = (NvV32)hClass;
    parms.hObjectError      = (NvV32)hErrorCtx;
    parms.hObjectBuffer     = (NvV32)hDataCtx;
    parms.offset            = (NvU32)offset;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV03_ALLOC_CHANNEL_DMA,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS07_PARAMETERS),
        &cbReturned
    );

    *ppChannel = (PVOID)parms.pControl;

    return (ULONG)parms.status;

}

ULONG __cdecl NvAllocMemory
(
    HANDLE hDriver,
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
    PVOID pParms;
    DWORD cbReturned;

    // set input parameters
    parms.hRoot                = (NvV32)hClient;
    parms.hObjectParent        = (NvV32)hParent;
    parms.hObjectNew           = (NvV32)hMemory;
    parms.hClass               = (NvV32)hClass;
    parms.flags                = (NvV32)flags;
    parms.pMemory              = (NvP64)*ppAddress;
    parms.pLimit               = (NvU64)*pLimit;

    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_ALLOC_MEMORY,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS02_PARAMETERS),
        &cbReturned
    );

    // retrieve output parameters
    *ppAddress = (PVOID)parms.pMemory;
    *pLimit    = (ULONG)parms.pLimit;

    return (ULONG)parms.status;

}

ULONG __cdecl NvAllocObject
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hChannel,
    ULONG hObject,
    ULONG hClass
)
{
    NVOS05_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    // set input parameters
    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hChannel;
    parms.hObjectNew        = (NvV32)hObject;
    parms.hClass            = (NvV32)hClass;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_ALLOC_OBJECT,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS05_PARAMETERS),
        &cbReturned
    );

    return (ULONG)parms.status;

}
ULONG __cdecl NvAllocEvent
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hParent,
    ULONG hObject,
    ULONG hClass,
    ULONG index,
#ifdef _WIN64
    PVOID hEvent
#else
    ULONG hEvent
#endif
)
{
    NVOS10_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    // set input parameters
    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hParent;
    parms.hObjectNew        = (NvV32)hObject;
    parms.hClass            = (NvV32)hClass;
    parms.index             = (NvV32)index;
    parms.hEvent            = (NvU64)hEvent;

    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_ALLOC_EVENT,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS10_PARAMETERS),
        &cbReturned
    );

    return (ULONG)parms.status;

}

ULONG __cdecl NvAlloc
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hChannel,
    ULONG hObject,
    ULONG hClass,
    PVOID pAllocParms
)
{
    NVOS21_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    // set input parameters
    parms.hRoot                    = (NvV32)hClient;
    parms.hObjectParent            = (NvV32)hChannel;
    parms.hObjectNew               = (NvV32)hObject;
    parms.hClass                   = (NvV32)hClass;
    parms.pAllocParms              = (NvP64)pAllocParms;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV04_ALLOC,
        &pParms,
        sizeof(PVOID),
        &parms,
        sizeof(NVOS21_PARAMETERS),
        &cbReturned
    );

    return (ULONG)parms.status;

}

ULONG __cdecl NvFree
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hParent,
    ULONG hObject
)
{
    NVOS00_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hParent;
    parms.hObjectOld        = (NvV32)hObject;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_FREE,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS00_PARAMETERS),
        &cbReturned
    );

    return (ULONG)parms.status;

}

ULONG __cdecl NvGetDmaPushInfo
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hDevice,
    ULONG hChannel,
    ULONG retArray,
    ULONG hDmaContext
)
{
    ULONG parms[5];
    PVOID pParms;
    DWORD cbReturned;

    parms[0] = (NvV32)hClient;
    parms[1] = (NvV32)hDevice;
    parms[2] = (NvU32)hChannel;
    parms[3] = (NvU32)retArray;
    parms[4] = (NvU32)hDmaContext;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV03_DMA_FLOW_CONTROL, // USING THIS TO GET TO RM
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(parms),
        &cbReturned
    );

    return (ULONG)parms[4];

}

ULONG __cdecl NvDmaFlowControl
(
    HANDLE hDriver,
    ULONG hChannel,
    ULONG flags,
    ULONG put,
    ULONG get
)
{
    NVOS08_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hChannel          = (NvV32)hChannel;
    parms.flags             = (NvV32)flags;
    parms.put               = (NvU32)put;
    parms.get               = (NvU32)get;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV03_DMA_FLOW_CONTROL,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS08_PARAMETERS),
        &cbReturned
    );

    return (ULONG)parms.status;
}

ULONG __cdecl NvArchHeap
(
    HANDLE hDriver,
    PVOID  parms
)
{
    PVOID pParms;
    DWORD cbReturned;

    pParms = parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_ARCH_HEAP,
        (&pParms),
        sizeof(PVOID),
        parms,
        sizeof(NVOS11_PARAMETERS),
        &cbReturned
    );

    return ((NVOS11_PARAMETERS *)parms)->status;
}

/*
/*
ULONG NvDmaPushMutexState
(
    HANDLE hDriver,
    DWORD  mutexState
)
{
    NV3_DMA_PUSH_MUTEX_STATE_STRUCT parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.mutexState = (U032)mutexState;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)NVAPI_IOCTL_DMA_PUSH_MUTEX_STATE,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NV3_DMA_PUSH_MUTEX_STATE_STRUCT),
        &cbReturned
    );

    return (ULONG)parms.nvStatus;

}
*/

ULONG __cdecl NvConfigVersion
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hDevice,
    ULONG *pVersion
)
{
    NVOS12_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hClient           = (NvV32)hClient;
    parms.hDevice           = (NvV32)hDevice;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_CONFIG_VERSION,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS12_PARAMETERS),
        &cbReturned
    );

    *pVersion = (ULONG)parms.version;

    return (ULONG)parms.status;

}

ULONG __cdecl NvConfigGet
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    ULONG *pValue
)
{
    NVOS13_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hClient           = (NvV32)hClient;
    parms.hDevice           = (NvV32)hDevice;
    parms.index             = (NvV32)index;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_CONFIG_GET,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS13_PARAMETERS),
        &cbReturned
    );

    *pValue = (ULONG)parms.value;

    return (ULONG)parms.status;

}

ULONG __cdecl NvConfigSet
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    ULONG newValue,
    ULONG *pOldValue
)
{
    NVOS14_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hClient           = (NvV32)hClient;
    parms.hDevice           = (NvV32)hDevice;
    parms.index             = (NvV32)index;
    parms.newValue          = (NvV32)newValue;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_CONFIG_SET,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS14_PARAMETERS),
        &cbReturned
    );

    *pOldValue = (ULONG)parms.oldValue;

    return (ULONG)parms.status;

}

ULONG __cdecl NvConfigGetEx
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    PVOID paramStructPtr,
    ULONG paramSize
)
{
    NVOS_CONFIG_GET_EX_PARAMS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hClient           = (NvV32)hClient;
    parms.hDevice           = (NvV32)hDevice;
    parms.index             = (NvV32)index;
    parms.paramStructPtr    = (NvP64)paramStructPtr;
    parms.paramSize         = (NvU32)paramSize;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV04_CONFIG_GET_EX,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS_CONFIG_GET_EX_PARAMS),
        &cbReturned
    );

    return (ULONG)parms.status;

}

ULONG __cdecl NvConfigSetEx
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    PVOID paramStructPtr,
    ULONG paramSize
)
{
    NVOS_CONFIG_SET_EX_PARAMS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hClient           = (NvV32)hClient;
    parms.hDevice           = (NvV32)hDevice;
    parms.index             = (NvV32)index;
    parms.paramStructPtr    = (NvP64)paramStructPtr;
    parms.paramSize         = (NvU32)paramSize;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV04_CONFIG_SET_EX,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS_CONFIG_SET_EX_PARAMS),
        &cbReturned
    );

    return (ULONG)parms.status;

}

ULONG __cdecl NvConfigUpdate
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hDevice,
    ULONG flags
)
{
    NVOS15_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hClient           = (NvV32)hClient;
    parms.hDevice           = (NvV32)hDevice;
    parms.flags             = (NvV32)flags;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_CONFIG_UPDATE,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS15_PARAMETERS),
        &cbReturned
    );

    return (ULONG)parms.status;

}

ULONG NvRmDebugControl
(
    HANDLE hDriver,
    ULONG hRoot,
    ULONG command,
    PVOID pArgs
)
{
    NVOS20_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hRoot              = (NvV32)hRoot;
    parms.command            = command;
    parms.pArgs              = (NvP64)((ULONG_PTR)hRoot);

    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV01_DEBUG_CONTROL,
        (&pParms),                      // input buffer
        sizeof(PVOID),                  // input buffer size
        (&parms),                       // output buffer
        sizeof(NVOS20_PARAMETERS),      // output buffer size
        &cbReturned
    );

    return (ULONG)parms.status;
}

ULONG __cdecl NvDirectMethodCall
(
    HANDLE hDriver,
    ULONG hClient,
    ULONG hChannel,
    ULONG hObject,
    ULONG method,
    ULONG data
)
{
    NVOS1D_PARAMETERS parms;
    PVOID pParms;
    DWORD cbReturned;

    parms.hRoot             = (NvV32)hClient;
    parms.hObjectParent     = (NvV32)hChannel;
    parms.hObjectOld        = (NvV32)hObject;
    parms.method            = (NvV32)method;
    parms.data              = (NvV32)data;
    pParms = &parms;

    EngDeviceIoControl(
        hDriver,
        (DWORD)IOCTL_NV04_DIRECT_METHOD_CALL,
        (&pParms),
        sizeof(PVOID),
        (&parms),
        sizeof(NVOS1D_PARAMETERS),
        &cbReturned
    );

    return (ULONG)parms.status;

}

// end of nvapi.c


