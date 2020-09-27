 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

 

 /***************************************************************************\
|*                                                                           *|
|*                         NV Architecture Interface                         *|
|*                                                                           *|
|*  <nvos.h>  defines the Operating System function and ioctl interfaces to  *|
|*  NVIDIA's Unified Media Architecture (TM).                                *|
|*                                                                           *|
 \***************************************************************************/


#ifndef NVOS_INCLUDED
#define NVOS_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <nvtypes.h>


 /***************************************************************************\
|*                              NV OS Functions                              *|
 \***************************************************************************/


/* macro NvOpen() */
/* values */
#define NVOPEN_STATUS_SUCCESS                            (0x00000000)
#define NVOPEN_STATUS_ERROR_OPERATING_SYSTEM             (0x00000001)

/* macro for NT Mini-Port driver */
#ifdef NTMINIPORT
#include <ntddk.h>
#include <windef.h>
typedef struct                                                                \
{                                                                              \
    PFILE_OBJECT   pFileObject;                                                \
    PDEVICE_OBJECT pDeviceObject;                                              \
} NVDESCRIPTOR;
#endif


/* function NvClose() */
#define NVCLOSE_ERROR_OK                                 (0x0000)
#define NVCLOSE_ERROR_UNSUCCESSFUL                       (0x0001)



 /***************************************************************************\
|*                               NV OS Ioctls                                *|
 \***************************************************************************/


/* ioctl Nv1GetDeviceOsName() */
#define NV0001_ERROR_OK                                  (0x0000)
#define NV0001_ERROR_NONEXISTENT_DEVICE                  (0x0001)
#define NV0001_ERROR_INSUFFICIENT_RESOURCES              (0x0002)
typedef struct 
{
	U016 devInstance;
	U032 nameBuffer;
	U016 maxNameLength;
	U016 nvStatus;
} NV1_GET_DEVICE_OS_NAME_STRUCT;


/* ioctl Nv1FreeMemory() */
#define NV000F_ERROR_OK                                  (0x0000)
#define NV000F_ERROR_INVALID_ADDRESS                     (0x0001)
typedef struct 
{
	U032 pMemory;
	U016 nvStatus;
} NV1_FREE_MEMORY_STRUCT;

/* ioctl Nv1FreeContextDma() */
#define NV0003_ERROR_OK                                  (0x0000)
#define NV0003_ERROR_INVALID_CONTEXT                     (0x0001)
typedef struct 
{
	U032 id;
	U016 nvStatus;
} NV1_FREE_CONTEXT_DMA_STRUCT;

/* ioctl Nv3FreeChannelPio() */
#define NV0009_ERROR_OK                                  (0x0000)
#define NV0009_ERROR_INVALID_CHANNEL                     (0x0001)
typedef struct 
{
	U016 devInstance;
	U032 pChannel;
	U016 nvStatus;
} NV3_FREE_CHANNEL_PIO_STRUCT;

/* ioctl Nv1FreeObject() */
#define NVFFF1_ERROR_OK                                  (0x0000)
#define NVFFF1_ERROR_INVALID_CONTEXT                     (0x0001)
typedef struct 
{
	U032 id;
	U016 nvStatus;
} NV1_FREE_OBJECT_STRUCT;

/* ioctl Nv4FreeChannelDma() */
typedef struct 
{
	U016 devInstance;
	U032 pChannel;
	U016 nvStatus;
} NV3_FREE_CHANNEL_DMA_STRUCT;

/* ioctl Nv4FreeChannelDma() */
typedef struct 
{
	U016 devInstance;
	U032 pChannel;
	U016 nvStatus;
} NV4_FREE_CHANNEL_DMA_STRUCT;

typedef struct 
{
	U032 hCallback;
	U016 nvStatus;
} NV1_FREE_CALLBACK_STRUCT;

typedef struct 
{
	U032 hMessage;
	U016 nvStatus;
} NV1_FREE_WIN32_MESSAGE_STRUCT;


/* ioctl Nv1AllocMemory() */
#define NV000E_ERROR_OK                                  (0x0000)
#define NV000E_ERROR_UNKNOWN_TYPE                        (0x0001)
#define NV000E_ERROR_UNSUCCESSFUL                        (0x0002)
typedef struct 
{
	U032 name;
	U032 pMemory;
	U032 limit;
	U016 nvStatus;
} NV1_ALLOC_MEMORY_STRUCT;


/* ioctl Nv1AllocContextDma() */
#define NV0002_CONTEXT_DMA_LOCK_DOWN                     (0x0000)
#define NV0002_CONTEXT_DMA_IN_TRANSIT                    (0x0001)
#define NV0002_CONTEXT_DMA_CACHED                        (0x0000)
#define NV0002_CONTEXT_DMA_UNCACHED                      (0x0002)
#define NV0002_CONTEXT_DMA_WRITE_COMBINED                (0x0000)
#define NV0002_CONTEXT_DMA_NOT_WRITE_COMBINED            (0x0004)
#define NV0002_ERROR_OK                                  (0x0000)
#define NV0002_ERROR_CREATE                              (0x0001)
#define NV0002_ERROR_INVALID_CLASS                       (0x0002)
#define NV0002_ERROR_INVALID_ID                          (0x0003)
#define NV0002_ERROR_INVALID_LIMIT                       (0x0004)
typedef struct 
{
	U032 id;
	V032 class;
	U032 base;
	U032 limit;
	U016 flags;
	U016 nvStatus;
} NV1_ALLOC_CONTEXT_DMA_STRUCT;


/* ioctl Nv3AllocChannelPio() */
#define NV0008_ERROR_OK                                  (0x0000)
#define NV0008_ERROR_MAPPING                             (0x0001)
#define NV0008_ERROR_MAX_CHANNELS                        (0x0002)
#define NV0008_ERROR_INVALID_CONTEXT                     (0x0003)
#define NV0008_ERROR_UNUSABLE_CONTEXT                    (0x0004)
#define NV0008_ERROR_CONTEXT_ERROR_IN_USE                (0x0005)
typedef struct 
{
	U016 devInstance;
	U032 idDmaContextErrorToMemory;
	U032 pChannel;
	U016 nvStatus;
} NV3_ALLOC_CHANNEL_PIO_STRUCT;


/* ioctl NV1_ALLOC_OBJECT */
#define  NV1_ALLOC_OBJECT                                (0x00000005)
/* parameters */
typedef struct
{
  V032 hObject;
  V032 hChannel;
  V032 hClass;
  V032 status;
} NVOS05_PARAMETERS;
/* parameter values */
#define NVOS05_STATUS_SUCCESS                            (0x00000000)
#define NVOS05_STATUS_ERROR_OPERATING_SYSTEM             (0x00000001)
#define NVOS05_STATUS_ERROR_BAD_OBJECT                   (0x00000002)
#define NVOS05_STATUS_ERROR_BAD_CHANNEL                  (0x00000003)
#define NVOS05_STATUS_ERROR_BAD_CLASS                    (0x00000004)
#define NVOS05_STATUS_ERROR_INSUFFICIENT_RESOURCES       (0x00000005)

/* macro for NT Mini-Port drivers */
#ifdef NTMINIPORT
#define Nv1AllocObject(descriptor, pParameters)                               \
{                                                                             \
    NTSTATUS ioStatus;                                                        \
    PIRP pIrp;                                                                \
    pIrp = IoBuildDeviceIoControlRequest(                                     \
        (ULONG)NV1_ALLOC_OBJECT,                                              \
        (descriptor).pDeviceObject,                                           \
        (PVOID)(pParameters),                                                 \
        (ULONG)sizeof(NVOS05_PARAMETERS),                                     \
        (PVOID)(pParameters),                                                 \
        (ULONG)sizeof(NVOS05_PARAMETERS),                                     \
        FALSE,                                                                \
        NULL,                                                                 \
        NULL);                                                                \
    if (pIrp!=NULL)                                                           \
        ioStatus = IoCallDriver(                                              \
            (descriptor).pDeviceObject,                                       \
            pIrp);                                                            \
    if (pIrp==NULL || !NT_SUCCESS(ioStatus))                                  \
        (pParameters)->status = NVOS05_STATUS_ERROR_OPERATING_SYSTEM;          \
}
#endif

/* macro for Win32 drivers */
#ifdef WIN32
#include <winbase.h>
#define Nv1AllocObject(descriptor, pParameters)                               \
{                                                                             \
    BOOL ioStatus;                                                            \
    DWORD bytesReturned;                                                      \
    ioStatus = DeviceIoControl(                                               \
        (HANDLE)(descriptor),                                                 \
        (DWORD)NV1_ALLOC_OBJECT,                                              \
        (LPVOID)(pPararmeters));                                              \
        (DWORD)sizeof(NVOS05_PARAMETERS),                                     \
        (LPVOID)(pParameters),                                                \
        (DWORD)sizeof(NVOS05_PARAMETERS),                                     \
        (LPDWORD)&bytesReturned,                                              \
        NULL);                                                                \
    if (ioStatus == FALSE)                                                    \
        (pParameters)->status = NVOS05_STATUS_ERROR_OPERATING_SYSTEM;          \
}
#endif

/* macro for Sun Solaris 1.0 and 2.0 drivers */
#if defined(SUNOS41) || defined(SYSV)
#ifdef SYSV
#include <unistd.h>
#endif
#include <sys/ioctl.h>
#define Nv1AllocObject(descriptor, pParameters)                               \
{                                                                             \
    int ioStatus;                                                             \
    ioStatus = ioctl(                                                         \
        (descriptor),                                                         \
        _IOWR(F, NV1_ALLOC_OBJECT, NVOS05_PARAMETERS),                        \
        (caddr_t)(pPararmeters));                                             \
    if (ioStatus == -1)                                                       \
        (pParameters)->status = NVOS05_STATUS_ERROR_OPERATING_SYSTEM;          \
}
#endif


typedef struct 
{
	U032 name;
	U032 hCallback;
	U016 nvStatus;
} NV1_ALLOC_CALLBACK_STRUCT;

typedef struct 
{
	U032 name;
	U032 hMessage;
	U016 nvStatus;
} NV1_ALLOC_WIN32_MESSAGE_STRUCT;


/* ioctl NV3_ALLOC_CHANNEL_DMA */
#define  NV3_ALLOC_CHANNEL_DMA                           (0x00000007)
/* parameters */
typedef struct
{
  V032 hObject;
  V032 hDevice;
  V032 hClass;
  V032 hError;
  V032 hBuffer;
  U032 offset;
  P064 pChannel;
  V032 status;
} NVOS07_PARAMETERS;
/* parameter values */
#define NVOS07_STATUS_SUCCESS                            (0x00000000)
#define NVOS07_STATUS_ERROR_OPERATING_SYSTEM             (0x00000001)
#define NVOS07_STATUS_ERROR_BAD_OBJECT                   (0x00000002)
#define NVOS07_STATUS_ERROR_BAD_DEVICE                   (0x00000003)
#define NVOS07_STATUS_ERROR_BAD_CLASS                    (0x00000004)
#define NVOS07_STATUS_ERROR_BAD_ERROR                    (0x00000005)
#define NVOS07_STATUS_ERROR_BAD_BUFFER                   (0x00000006)
#define NVOS07_STATUS_ERROR_BAD_OFFSET                   (0x00000007)
#define NVOS07_STATUS_ERROR_INSUFFICIENT_RESOURCES       (0x00000008)

/* macro for NT Mini-Port drivers */
#ifdef NTMINIPORT
#define Nv3AllocChannelDma(descriptor, pParameters)                           \
{                                                                             \
    NTSTATUS ioStatus;                                                        \
    PIRP pIrp;                                                                \
    pIrp = IoBuildDeviceIoControlRequest(                                     \
        (ULONG)NV3_IOCTL_ALLOC_CHANNEL_DMA,                                         \
        (descriptor).pDeviceObject,                                           \
        (PVOID)(pParameters),                                                 \
        (ULONG)sizeof(NVOS07_PARAMETERS),                                     \
        (PVOID)(pParameters),                                                 \
        (ULONG)sizeof(NVOS07_PARAMETERS),                                     \
        FALSE,                                                                \
        NULL,                                                                 \
        NULL);                                                                \
    if (pIrp!=NULL)                                                           \
        ioStatus = IoCallDriver(                                              \
            (descriptor).pDeviceObject,                                       \
            pIrp);                                                            \
    if (pIrp==NULL || !NT_SUCCESS(ioStatus))                                  \
        (pParameters)->status = NVOS07_STATUS_ERROR_OPERATING_SYSTEM;          \
}
#endif

/* macro for Win32 drivers */
#ifdef WIN32
#include <winbase.h>
#define Nv3AllocChannelDma(descriptor, pParameters)                           \
{                                                                             \
    BOOL ioStatus;                                                            \
    DWORD bytesReturned;                                                      \
    ioStatus = DeviceIoControl(                                               \
        (HANDLE)(descriptor),                                                 \
        (DWORD)NV3_ALLOC_CHANNEL_DMA,                                         \
        (LPVOID)(pPararmeters));                                              \
        (DWORD)sizeof(NVOS07_PARAMETERS),                                     \
        (LPVOID)(pParameters),                                                \
        (DWORD)sizeof(NVOS07_PARAMETERS),                                     \
        (LPDWORD)&bytesReturned,                                              \
        NULL);                                                                \
    if (ioStatus == FALSE)                                                    \
        (pParameters)->status = NVOS07_STATUS_ERROR_OPERATING_SYSTEM;          \
}
#endif

/* macro for Sun Solaris 1.0 and 2.0 drivers */
#if defined(SUNOS41) || defined(SYSV)
#ifdef SYSV
#include <unistd.h>
#endif
#include <sys/ioctl.h>
#define Nv3AllocChannelDma(descriptor, pParameters)                           \
{                                                                             \
    int ioStatus;                                                             \
    ioStatus = ioctl(                                                         \
        (descriptor),                                                         \
        _IOWR(F, NV3_ALLOC_CHANNEL_DMA, NVOS07_PARAMETERS),                   \
        (caddr_t)(pPararmeters));                                             \
    if (ioStatus == -1)                                                       \
        (pParameters)->status = NVOS07_STATUS_ERROR_OPERATING_SYSTEM;          \
}
#endif


/* ioctl NV3_DMA_FLOW_CONTROL */
#define  NV3_DMA_FLOW_CONTROL                            (0x00000008)
/* parameters */
typedef struct
{
  V032 hChannel;
  V032 flags;
  U032 put;
  U032 get;
  V032 status;
} NVOS08_PARAMETERS;
/* parameter values */
#define NVOS08_FLAGS_READ_GET                            (0x00000000)
#define NVOS08_FLAGS_WRITE_PUT_READ_GET                  (0x00000001)
#define NVOS08_FLAGS_SET_JUMP_WRITE_PUT_READ_GET         (0x00000002)
#define NVOS08_STATUS_SUCCESS                            (0x00000000)
#define NVOS08_STATUS_ERROR_OPERATING_SYSTEM             (0x00000001)
#define NVOS08_STATUS_ERROR_BAD_CHANNEL                  (0x00000002)
#define NVOS08_STATUS_ERROR_BAD_FLAGS                    (0x00000003)
#define NVOS08_STATUS_ERROR_BAD_JUMP                     (0x00000004)
#define NVOS08_STATUS_ERROR_BAD_PUT                      (0x00000005)
#define NVOS08_STATUS_ERROR_BAD_GET                      (0x00000006)

/* macro for NT Mini-Port drivers */
#ifdef NTMINIPORT
#define Nv3DmaFlowControl(descriptor, pParameters)                            \
{                                                                             \
    NTSTATUS ioStatus;                                                        \
    PIRP pIrp;                                                                \
    pIrp = IoBuildDeviceIoControlRequest(                                     \
        (ULONG)NV3_IOCTL_DMA_FLOW_CONTROL,                                          \
        (descriptor).pDeviceObject,                                           \
        (PVOID)(pParameters),                                                 \
        (ULONG)sizeof(NVOS08_PARAMETERS),                                     \
        (PVOID)(pParameters),                                                 \
        (ULONG)sizeof(NVOS08_PARAMETERS),                                     \
        FALSE,                                                                \
        NULL,                                                                 \
        NULL);                                                                \
    if (pIrp!=NULL)                                                           \
        ioStatus = IoCallDriver(                                              \
            (descriptor).pDeviceObject,                                       \
            pIrp);                                                            \
    if (pIrp==NULL || !NT_SUCCESS(ioStatus))                                  \
        (pParameters)->status = NVOS08_STATUS_ERROR_OPERATING_SYSTEM;          \
}
#endif

/* macro for Win32 drivers */
#ifdef WIN32
#include <winbase.h>
#define Nv3DmaFlowControl(descriptor, pParameters)                            \
{                                                                             \
    BOOL ioStatus;                                                            \
    DWORD bytesReturned;                                                      \
    ioStatus = DeviceIoControl(                                               \
        (HANDLE)(descriptor),                                                 \
        (DWORD)NV3_DMA_FLOW_CONTROL,                                          \
        (LPVOID)(pPararmeters));                                              \
        (DWORD)sizeof(NVOS08_PARAMETERS),                                     \
        (LPVOID)(pParameters),                                                \
        (DWORD)sizeof(NVOS08_PARAMETERS),                                     \
        (LPDWORD)&bytesReturned,                                              \
        NULL);                                                                \
    if (ioStatus == FALSE)                                                    \
        (pParameters)->status = NVOS08_STATUS_ERROR_OPERATING_SYSTEM;          \
}
#endif

/* macro for Sun Solaris 1.0 and 2.0 drivers */
#if defined(SUNOS41) || defined(SYSV)
#ifdef SYSV
#include <unistd.h>
#endif
#include <sys/ioctl.h>
#define Nv3DmaFlowControl(descriptor, pParameters)                            \
{                                                                             \
    int ioStatus;                                                             \
    ioStatus = ioctl(                                                         \
        (descriptor),                                                         \
        _IOWR(F, NV3_DMA_FLOW_CONTROL, NVOS08_PARAMETERS),                    \
        (caddr_t)(pPararmeters));                                             \
    if (ioStatus == -1)                                                       \
        (pParameters)->status = NVOS08_STATUS_ERROR_OPERATING_SYSTEM;          \
}
#endif

/* ioctl NV3_DMA_PUSH_MUTEX_STATE */
#define  NV3_DMA_PUSH_MUTEX_STATE                        (0x00000009)
/* parameters */
typedef struct
{
  U032 mutexState;
  U016 nvStatus;
} NV3_DMA_PUSH_MUTEX_STATE_STRUCT;
/* parameter values */
#define NV3_DMA_PUSH_MUTEX_STATE_FALSE                   (0x00000000)
#define NV3_DMA_PUSH_MUTEX_STATE_TRUE                    (0x00000000)
#define NV3_DMA_PUSH_MUTEX_STATE_STATUS_SUCCESS          (0x00000000)

/* only available from the mini-port driver */
/* macro for NT Mini-Port drivers */
#ifdef NTMINIPORT
#define Nv3DmaPushMutexState(descriptor, pParameters)                         \
{                                                                             \
    NTSTATUS ioStatus;                                                        \
    PIRP pIrp;                                                                \
    pIrp = IoBuildDeviceIoControlRequest(                                     \
        (ULONG)NV3_IOCTL_DMA_PUSH_MUTEX_STATE,                                \
        (descriptor).pDeviceObject,                                           \
        (PVOID)(pParameters),                                                 \
        (ULONG)sizeof(NV3_DMA_PUSH_MUTEX_STATE),                              \
        (PVOID)(pParameters),                                                 \
        (ULONG)sizeof(NV3_DMA_PUSH_MUTEX_STATE),                              \
        FALSE,                                                                \
        NULL,                                                                 \
        NULL);                                                                \
    if (pIrp!=NULL)                                                           \
        ioStatus = IoCallDriver(                                              \
            (descriptor).pDeviceObject,                                       \
            pIrp);                                                            \
    if (pIrp==NULL || !NT_SUCCESS(ioStatus))                                  \
        (pParameters)->status = NV3_DMA_PUSH_MUTEX_STATE_STATUS_SUCCESS;      \
}
#endif

/* ioctl OS09 */

/* ioctl OS0A */

/* ioctl OS0B */

/* ioctl OS0C */

/* ioctl OS0D */

/* ioctl OS0E */

/* ioctl OS0F */



#ifdef __cplusplus
};
#endif
#endif /* NVOS_INCLUDED */

