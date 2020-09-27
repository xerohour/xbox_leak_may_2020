/*
 * nvrmint.h
 *
 * NVidia resource manager API header file for internal builds.
 *
 * Copyright (c) 1997, Nvidia Corporation.  All rights reserved.
 * Portions Copyright (c) 1996, 1997, Silicon Graphics Inc.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/*
 * _DWTYPE is returned from GetVersionEx(&osVer) and it is stored in dwWinType
 * in the __GLNVstate structure.
 */
#define NV_WIN_IS_9X(_DWTYPE) (_DWTYPE == VER_PLATFORM_WIN32_WINDOWS)
#define NV_WIN_IS_NT(_DWTYPE) (_DWTYPE == VER_PLATFORM_WIN32_NT)

typedef HANDLE NVDESCRIPTOR;
#define NV_DEVICE_NAME_LENGTH_MAX 127
#define NVARCH_INVALID_NVDESCRIPTOR 0
#define NVARCH_INVALID_CLIENT_HANDLE 0


/*
 * Control codes
 */
#define NVRM_IOCTL_BASE                 0x10
#define NVRM_IOCTL_NV_ARCH              (NVRM_IOCTL_BASE + 5)

#define NVRM_ARCH_NV01FREE              0
#define NVRM_ARCH_NV01ALLOCROOT         1
#define NVRM_ARCH_NV01ALLOCMEMORY       2
#define NVRM_ARCH_NV01ALLOCCONTEXTDMA   3
#define NVRM_ARCH_NV01ALLOCCHANNELPIO   4
#define NVRM_ARCH_NV01ALLOCOBJECT       5
#define NVRM_ARCH_NV01ALLOCDEVICE       6
#define NVRM_ARCH_NV03ALLOCCHANNELDMA   7
#define NVRM_ARCH_NV03DMAFLOWCONTROL    8
#define NVRM_ARCH_NV01INTERRUPT         9
#define NVRM_ARCH_NV01ALLOCEVENT        10
#define NVRM_ARCH_NV01HEAP              11
#define NVRM_ARCH_NV01CONFIGVERSION     12
#define NVRM_ARCH_NV01CONFIGGET         13
#define NVRM_ARCH_NV01CONFIGSET         14
#define NVRM_ARCH_NV01CONFIGUPDATE      15
#define NVRM_ARCH_RING0CALLBACK         16
#define NVRM_ARCH_NV04CONFIGGETEX       17
#define NVRM_ARCH_NV04CONFIGSETEX       18
#define NVRM_ARCH_NV04I2CACCESS         19
#define NVRM_ARCH_NV01DEBUGCONTROL      20
#define NVRM_ARCH_NV04ALLOC             21
// audio stuff
#define NVRM_ARCH_UNIFIEDFREE           30

/*
 * Win 9x data structure.
 */
typedef struct
{
    ULONG function;
    ULONG pParameters;
} NvIoctlArch, *PNvIoctlArch;

typedef struct
{
    ULONG p0;
    ULONG p1;
    ULONG p2;
    ULONG p3;
    ULONG p4;
    ULONG p5;
    ULONG p6;
} NVWATCHAPI_PARAMETERS;

ULONG   NvAlloc             (HANDLE, ULONG, ULONG, ULONG, ULONG, PVOID);
ULONG   NvAllocRoot         (HANDLE, ULONG*);
ULONG   NvAllocDevice       (HANDLE, ULONG, ULONG, ULONG, PUCHAR);
ULONG   NvAllocContextDma   (HANDLE, ULONG, ULONG, ULONG, ULONG, PVOID, ULONG);
ULONG   NvAllocChannelPio   (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID*, ULONG);
ULONG   NvAllocChannelDma   (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID*);
ULONG   NvAllocMemory       (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, PVOID*, ULONG*);
ULONG   NvAllocObject       (HANDLE, ULONG, ULONG, ULONG, ULONG);
ULONG   NvFree              (HANDLE, ULONG, ULONG, ULONG);
ULONG   NvDmaFlowControl    (HANDLE, ULONG, ULONG, ULONG, ULONG);
ULONG   NvAllocEvent        (HANDLE, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);
ULONG   NvWatchApiFrontEnd  (ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);
//ULONG     NvDmaPushMutexState (HANDLE, DWORD);

/*
 * Internal, platform dependent implementation of API routines
 */

HANDLE NvRmOpenWin9x(VOID);
HANDLE NvRmOpenWinNt(VOID);

VOID NvRmCloseWin9x(HANDLE);
VOID NvRmCloseWinNt(VOID);

ULONG NvRmAllocWin9x(ULONG hClient, ULONG hParent, ULONG hObject, ULONG hClass, PVOID pAllocParms);
ULONG NvRmAllocWinNt(ULONG hClient, ULONG hParent, ULONG hObject, ULONG hClass, PVOID pAllocParms);

ULONG NvRmAllocRootWin9x(ULONG *phClient);
ULONG NvRmAllocRootWinNt(ULONG *phClient);

ULONG NvRmAllocDeviceWin9x(ULONG hClient, ULONG hDevice, ULONG hClass, PUCHAR szName);
ULONG NvRmAllocDeviceWinNt(ULONG hClient, ULONG hDevice, ULONG hClass, PUCHAR szName);

ULONG NvRmAllocContextDmaWin9x(ULONG hClient, ULONG hDma, ULONG hClass, ULONG flags, PVOID base, ULONG limit);
ULONG NvRmAllocContextDmaWinNt(ULONG hClient, ULONG hDma, ULONG hClass, ULONG flags, PVOID base, ULONG limit);

ULONG NvRmAllocChannelPioWin9x(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hClass, ULONG hErrorCtx, PVOID *ppChannel, ULONG flags);
ULONG NvRmAllocChannelPioWinNt(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hClass, ULONG hErrorCtx, PVOID *ppChannel, ULONG flags);

ULONG NvRmAllocChannelDmaWin9x(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hClass, ULONG hErrorCtx, ULONG hDataCtx, ULONG offset, PVOID *ppChannel);
ULONG NvRmAllocChannelDmaWinNt(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hClass, ULONG hErrorCtx, ULONG hDataCtx, ULONG offset, PVOID *ppChannel);

ULONG NvRmAllocMemoryWin9x(ULONG hClient, ULONG hParent, ULONG hMemory, ULONG hClass, ULONG flags, PVOID *ppAddress, ULONG *pLimit);
ULONG NvRmAllocMemoryWinNt(ULONG hClient, ULONG hParent, ULONG hMemory, ULONG hClass, ULONG flags, PVOID *ppAddress, ULONG *pLimit);

ULONG NvRmAllocObjectWin9x(ULONG hClient, ULONG hChannel, ULONG hObject, ULONG hClass);
ULONG NvRmAllocObjectWinNt(ULONG hClient, ULONG hChannel, ULONG hObject, ULONG hClass);

ULONG NvRmFreeWin9x(ULONG hClient, ULONG hParent, ULONG hObject);
ULONG NvRmFreeWinNt(ULONG hClient, ULONG hParent, ULONG hObject);

ULONG Nv3RmGetDmaPushInfoWin9x(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hDmaContext, ULONG get);
ULONG Nv3RmGetDmaPushInfoWinNt(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hDmaContext, ULONG get);

ULONG NvRmAllocEventWin9x(ULONG hClient, ULONG hObjectParent, ULONG hObjectNew, ULONG hClass, ULONG index, ULONG data);
ULONG NvRmAllocEventWinNt(ULONG hClient, ULONG hObjectParent, ULONG hObjectNew, ULONG hClass, ULONG index, ULONG data);

ULONG NvRmArchHeapWin9x(ULONG hClient, ULONG hParent, ULONG function, ULONG owner, ULONG type,
                               ULONG height, ULONG size, ULONG *pitch, ULONG *offset, ULONG *address, ULONG *limit, ULONG *free, ULONG *total);
ULONG NvRmArchHeapWinNt(ULONG hClient, ULONG hParent, ULONG function, ULONG owner, ULONG type,
                               ULONG height, ULONG size, ULONG *pitch, ULONG *offset, ULONG *address, ULONG *limit, ULONG *free, ULONG *total);
ULONG NvRmConfigGetWin9x(ULONG hClient, ULONG hDevice, ULONG index, ULONG *pValue);
ULONG NvRmConfigGetWinNt(ULONG hClient, ULONG hDevice, ULONG index, ULONG *pValue);
ULONG NvRmConfigSetWin9x(ULONG hClient, ULONG hDevice, ULONG index, ULONG newValue, ULONG *pOldValue);
ULONG NvRmConfigSetWinNt(ULONG hClient, ULONG hDevice, ULONG index, ULONG newValue, ULONG *pOldValue);

ULONG NvRmConfigGetExWin9x(ULONG hClient, ULONG hDevice, ULONG index, VOID *paramStructPtr, ULONG paramSize);
ULONG NvRmConfigGetExWinNt(ULONG hClient, ULONG hDevice, ULONG index, VOID *paramStructPtr, ULONG paramSize);
ULONG NvRmConfigSetExWin9x(ULONG hClient, ULONG hDevice, ULONG index, VOID *paramStructPtr, ULONG paramSize);
ULONG NvRmConfigSetExWinNt(ULONG hClient, ULONG hDevice, ULONG index, VOID *paramStructPtr, ULONG paramSize);

ULONG NvRmInterruptExWin9x(ULONG hClient, ULONG device);
ULONG NvRmInterruptExWinNt(ULONG hClient, ULONG device);

// Unpublished hook for driver to call a ring0 function.
ULONG NvRmRing0CallbackWin9x(ULONG hClient, ULONG device, ULONG procAddr, ULONG param1, ULONG param2);
ULONG NvRmRing0CallbackWinNt(ULONG hClient, ULONG device, ULONG procAddr, ULONG param1, ULONG param2);

ULONG NvRmInterruptWin9x(ULONG, ULONG);
ULONG NvRmInterruptWinNt(ULONG, ULONG);

// Unpublished I2C access routine
ULONG NvRmI2CAccessWin9x(ULONG hClient, ULONG device, VOID *paramStructPtr );
ULONG NvRmI2CAccessWinNt(ULONG hClient, ULONG device, VOID *paramStructPtr );

// Unpublished debug control
ULONG NvRmDebugControlWin9x( ULONG hRoot, ULONG command, VOID *pArgs);
ULONG NvRmDebugControlWinNt( ULONG hRoot, ULONG command, VOID *pArgs);
