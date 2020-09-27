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

 /********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVHEAP.H                                                          *
*   Heap allocation stuff                                                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler        bertrem     16Jul99     created             *
*       Ben de Waal                         27Jul99     add CHeap class     *
*                                                                           *
\***************************************************************************/

#ifndef _NVHEAP_H_
#define _NVHEAP_H_

#ifndef __cplusplus
#error C++ compiler required
#endif

extern "C"
{

extern NVOS11_PARAMETERS HeapParams;

// define to disable tiled memory
//#define HEAP_DISABLE_TILING
//#define CHEAP_SANITY_CHECK      // check integrity

#define HEAP_ALLOC_DEPTH_WIDTH_HEIGHT        1
#define HEAP_ALLOC_SIZE                      2
#define HEAP_FREE                            3
#define HEAP_PURGE                           4
#define HEAP_INFO                            5
#define HEAP_ALLOC_TILED_PITCH_HEIGHT        6

#define TYPE_IMAGE                           0
#define TYPE_DEPTH                           1
#define TYPE_TEXTURE                         2
#define TYPE_OVERLAY                         3
#define TYPE_FONT                            4
#define TYPE_CURSOR                          5
#define TYPE_DMA                             6
#define TYPE_INSTANCE                        7
#define TYPE_VERTEX                          2  // same as texture for now. differentiate later
#define MEM_TYPE_PRIMARY                     8
#define MEM_TYPE_IMAGE_TILED                 9
#define TYPE_DEPTH_COMPR16                   10
#define TYPE_DEPTH_COMPR32                   11

#define STATUS_SUCCESS                       (0x00000000)
#define STATUS_ERROR_INSUFFICIENT_RESOURCES  (0x00000001)
#define STATUS_ERROR_INVALID_FUNCTION        (0x00000002)
#define STATUS_ERROR_INVALID_OWNER           (0x00000003)

//
// Heap manager interface.
//
//extern void __stdcall NvIoControl(DWORD, PVOID);

#define DX_HEAP_ID  'NVDX'

#ifndef WINNT

// parameter values
#define NVHEAP_PURGE()                                          \
{                                                               \
    HeapParams.hRoot = pDriverData->dwRootHandle;               \
    HeapParams.hObjectParent = NV_WIN_DEVICE;                   \
    HeapParams.function = HEAP_PURGE;                           \
    HeapParams.owner    = DX_HEAP_ID;                           \
    HeapParams.status = NvRmArchHeap((ULONG) HeapParams.hRoot, (ULONG) HeapParams.hObjectParent, (ULONG) HeapParams.function, (ULONG) HeapParams.owner,HeapParams.type,HeapParams.height,HeapParams.size, (ULONG *)&HeapParams.pitch, &HeapParams.offset, &HeapParams.address, &HeapParams.limit, &HeapParams.free, &HeapParams.total); \
}

#define NVHEAP_INFO()                                           \
{                                                               \
    HeapParams.hRoot = pDriverData->dwRootHandle;               \
    HeapParams.hObjectParent = NV_WIN_DEVICE;                   \
    HeapParams.function = HEAP_INFO;                            \
    HeapParams.owner    = DX_HEAP_ID;                           \
    HeapParams.status = NvRmArchHeap((ULONG) HeapParams.hRoot, (ULONG) HeapParams.hObjectParent, (ULONG) HeapParams.function, (ULONG) HeapParams.owner,HeapParams.type,HeapParams.height,HeapParams.size, (ULONG *)&HeapParams.pitch, &HeapParams.offset, &HeapParams.address, &HeapParams.limit, &HeapParams.free, &HeapParams.total); \
    if (pDriverData && !pDriverData->DDrawVideoSurfaceCount) {  \
        pDriverData->VideoHeapOverhead = HeapParams.total - HeapParams.free; \
    }                                                           \
}

#define NVHEAP_ALLOC(sts,pvm,sz,tp)                             \
{                                                               \
    HeapParams.hRoot = pDriverData->dwRootHandle;               \
    HeapParams.hObjectParent = NV_WIN_DEVICE;                   \
    HeapParams.function = HEAP_ALLOC_SIZE;                      \
    HeapParams.owner    = DX_HEAP_ID;                           \
    HeapParams.type     = (tp);                                 \
    HeapParams.size     = (sz);                                 \
    HeapParams.status = NvRmArchHeap((ULONG) HeapParams.hRoot, (ULONG) HeapParams.hObjectParent, (ULONG) HeapParams.function, (ULONG) HeapParams.owner,HeapParams.type,HeapParams.height,HeapParams.size, (ULONG *)&HeapParams.pitch, &HeapParams.offset, &HeapParams.address, &HeapParams.limit, &HeapParams.free, &HeapParams.total); \
    (sts) = HeapParams.status;                                  \
    (pvm) = (unsigned long)(HeapParams.address);                \
    pDriverData->VideoHeapFree = HeapParams.free;               \
}

#define NVHEAP_ALLOC_B(sts,pvm,sz,tp)                           \
{                                                               \
    HeapParams.hRoot = pDriverData->dwRootHandle;               \
    HeapParams.hObjectParent = NV_WIN_DEVICE;                   \
    HeapParams.function = HEAP_ALLOC_SIZE;                      \
    HeapParams.owner    = DX_HEAP_ID;                           \
    HeapParams.type     = (tp);                                 \
    HeapParams.size     = (sz);                                 \
    HeapParams.status = NvRmArchHeap((ULONG) HeapParams.hRoot, (ULONG) HeapParams.hObjectParent, (ULONG) HeapParams.function, (ULONG) HeapParams.owner,HeapParams.type,HeapParams.height,HeapParams.size, (ULONG *)&HeapParams.pitch, &HeapParams.offset, &HeapParams.address, &HeapParams.limit, &HeapParams.free, &HeapParams.total); \
    (sts) = HeapParams.status;                                  \
    (pvm) = (unsigned long)(HeapParams.address);                \
    pDriverData->VideoHeapFree = HeapParams.free;               \
}

#define NVHEAP_ALLOC_C(sts,pvm,sz,tp)                           \
{                                                               \
    HeapParams.hRoot = pDriverData->dwRootHandle;               \
    HeapParams.hObjectParent = NV_WIN_DEVICE;                   \
    HeapParams.function = HEAP_ALLOC_SIZE;                      \
    HeapParams.owner    = DX_HEAP_ID;                           \
    HeapParams.type     = (tp);                                 \
    HeapParams.size     = (sz);                                 \
    HeapParams.status = NvRmArchHeap((ULONG) HeapParams.hRoot, (ULONG) HeapParams.hObjectParent, (ULONG) HeapParams.function, (ULONG) HeapParams.owner,HeapParams.type,HeapParams.height,HeapParams.size, (ULONG *)&HeapParams.pitch, &HeapParams.offset, &HeapParams.address, &HeapParams.limit, &HeapParams.free, &HeapParams.total); \
    (sts) = HeapParams.status;                                  \
    (pvm) = (unsigned long)(HeapParams.address);                \
    pDriverData->VideoHeapFree = HeapParams.free;               \
}

#ifndef HEAP_DISABLE_TILING
#define NVHEAP_ALLOC_TILED(sts,pvm,pch,ht,tp)                   \
{                                                               \
    HeapParams.hRoot = pDriverData->dwRootHandle;               \
    HeapParams.hObjectParent = NV_WIN_DEVICE;                   \
    HeapParams.function = HEAP_ALLOC_TILED_PITCH_HEIGHT;        \
    HeapParams.owner    = DX_HEAP_ID;                           \
    HeapParams.type     = (tp);                                 \
    HeapParams.pitch    = (pch);                                \
    HeapParams.height   = (ht);                                 \
    HeapParams.status = NvRmArchHeap((ULONG) HeapParams.hRoot, (ULONG) HeapParams.hObjectParent, (ULONG) HeapParams.function, (ULONG) HeapParams.owner,HeapParams.type,HeapParams.height,HeapParams.size, (ULONG *)&HeapParams.pitch, &HeapParams.offset, &HeapParams.address, &HeapParams.limit, &HeapParams.free, &HeapParams.total); \
    (sts) = HeapParams.status;                                  \
    (pvm) = (unsigned long)(HeapParams.address);                \
    (pch) = HeapParams.pitch;                                   \
    pDriverData->VideoHeapFree = HeapParams.free;               \
}
#else
#define NVHEAP_ALLOC_TILED(sts,pvm,pch,ht,tp) NVHEAP_ALLOC(sts,pvm,pch,ht,tp)
#endif

#define NVHEAP_FREE(pvm)                                        \
{                                                               \
    HeapParams.hRoot = pDriverData->dwRootHandle;               \
    HeapParams.hObjectParent = NV_WIN_DEVICE;                   \
    HeapParams.function = HEAP_FREE;                            \
    HeapParams.owner    = DX_HEAP_ID;                           \
    HeapParams.offset   = (U032)(pvm) - pDriverData->BaseAddress;\
    HeapParams.status = NvRmArchHeap((ULONG) HeapParams.hRoot, (ULONG) HeapParams.hObjectParent, (ULONG) HeapParams.function, (ULONG) HeapParams.owner,HeapParams.type,HeapParams.height,HeapParams.size, (ULONG *)&HeapParams.pitch, &HeapParams.offset, &HeapParams.address, &HeapParams.limit, &HeapParams.free, &HeapParams.total); \
    pDriverData->VideoHeapFree = HeapParams.free;               \
}
#else   // WINNT

#define NVHEAP_PURGE() // NT never does a purge!

#define NVHEAP_INFO()                                           \
{                                                               \
    PVOID pParms = (PVOID) &HeapParams;                         \
    DWORD cbReturned;                                           \
                                                                \
    HeapParams.hRoot = ppdev->hClient;                          \
    HeapParams.hObjectParent = ppdev->hDevice;                  \
    HeapParams.function = HEAP_INFO;                            \
    HeapParams.owner    = 'NVDD';                               \
    EngDeviceIoControl(                                         \
        ppdev->hDriver,                                         \
        (DWORD)IOCTL_NV01_ARCH_HEAP,                            \
        (&pParms),                                              \
        sizeof(PVOID),                                          \
        pParms,                                                 \
        sizeof(NVOS11_PARAMETERS),                              \
        &cbReturned                                             \
    );                                                          \
    if (pDriverData) {                                          \
        if (!pDriverData->DDrawVideoSurfaceCount)               \
        {                                                       \
            pDriverData->VideoHeapOverhead = HeapParams.total - \
            HeapParams.free - ppdev->cbGdiHeap;                 \
        }                                                       \
        ppdev->VideoHeapTotal = HeapParams.total;               \
        ppdev->VideoHeapFree = HeapParams.free;                 \
    }                                                           \
    HeapParams.address = 0;                                     \
}


#define NVHEAP_ALLOC(sts,pvm,sz,tp)                             \
{                                                               \
    NVOS11_PARAMETERS   HeapParams;                             \
    PVOID pParms = (PVOID) &HeapParams;                         \
    DWORD cbReturned;                                           \
                                                                \
    if (pDriverData && !pDriverData->DDrawVideoSurfaceCount && !ppdev->cbGdiHeap)    \
    {                                                           \
        ppdev->cbGdiHeap = ppdev->VideoHeapTotal -              \
            ppdev->VideoHeapFree;                               \
    }                                                           \
                                                                \
    HeapParams.hRoot = ppdev->hClient;                          \
    HeapParams.hObjectParent = ppdev->hDevice;                  \
    HeapParams.function = HEAP_ALLOC_SIZE;                      \
    HeapParams.owner    = 'NVDD';                               \
    HeapParams.type     = (tp);                                 \
    HeapParams.size     = (sz);                                 \
    EngDeviceIoControl(                                         \
        ppdev->hDriver,                                         \
        (DWORD)IOCTL_NV01_ARCH_HEAP,                            \
        (&pParms),                                              \
        sizeof(PVOID),                                          \
        pParms,                                                 \
        sizeof(NVOS11_PARAMETERS),                              \
        &cbReturned                                             \
    );                                                          \
    (sts) = HeapParams.status;                                  \
    (pvm) = (unsigned long)(HeapParams.offset);                 \
                                                                \
    if (sts && ppdev->cbGdiHeap)                                \
    {                                                           \
        bMoveAllDfbsFromOffscreenToDibs(ppdev);                 \
        HeapParams.hRoot = ppdev->hClient;                      \
        HeapParams.hObjectParent = ppdev->hDevice;              \
        HeapParams.function = HEAP_ALLOC_SIZE;                  \
        HeapParams.owner    = 'NVDD';                           \
        HeapParams.type     = (tp);                             \
        HeapParams.size     = (sz);                             \
        EngDeviceIoControl(                                     \
            ppdev->hDriver,                                     \
            (DWORD)IOCTL_NV01_ARCH_HEAP,                        \
            (&pParms),                                          \
            sizeof(PVOID),                                      \
            pParms,                                             \
            sizeof(NVOS11_PARAMETERS),                          \
            &cbReturned                                         \
        );                                                      \
        (sts) = HeapParams.status;                              \
        (pvm) = (unsigned long)(HeapParams.offset);             \
        pDriverData->VideoHeapFree =                            \
            ppdev->VideoHeapFree;                               \
    }                                                           \
                                                                \
    if (!(sts)) {                                               \
        ppdev->VideoHeapFree = HeapParams.free;                 \
        pDriverData->VideoHeapFree =                            \
            HeapParams.free + ppdev->cbGdiHeap;                 \
    }                                                           \
}

#define NVHEAP_ALLOC_B(sts,pvm,sz,tp)                           \
{                                                               \
    NVOS11_PARAMETERS   HeapParams;                             \
    PVOID pParms = (PVOID) &HeapParams;                         \
    DWORD cbReturned;                                           \
                                                                \
    if (pDriverData && !pDriverData->DDrawVideoSurfaceCount && !ppdev->cbGdiHeap)    \
    {                                                           \
        ppdev->cbGdiHeap = ppdev->VideoHeapTotal -              \
            ppdev->VideoHeapFree;                               \
    }                                                           \
                                                                \
    HeapParams.hRoot = ppdev->hClient;                          \
    HeapParams.hObjectParent = ppdev->hDevice;                  \
    HeapParams.function = HEAP_ALLOC_SIZE;                      \
    HeapParams.owner    = 'NVDD';                               \
    HeapParams.type     = (tp);                                 \
    HeapParams.size     = (sz);                                 \
    EngDeviceIoControl(                                         \
        ppdev->hDriver,                                         \
        (DWORD)IOCTL_NV01_ARCH_HEAP,                            \
        (&pParms),                                              \
        sizeof(PVOID),                                          \
        pParms,                                                 \
        sizeof(NVOS11_PARAMETERS),                              \
        &cbReturned                                             \
    );                                                          \
    (sts) = HeapParams.status;                                  \
    (pvm) = (unsigned long)(HeapParams.offset);                 \
                                                                \
    if (sts && ppdev->cbGdiHeap)                                \
    {                                                           \
        bMoveAllDfbsFromOffscreenToDibs(ppdev);                 \
        HeapParams.hRoot = ppdev->hClient;                      \
        HeapParams.hObjectParent = ppdev->hDevice;              \
        HeapParams.function = HEAP_ALLOC_SIZE;                  \
        HeapParams.owner    = 'NVDD';                           \
        HeapParams.type     = (tp);                             \
        HeapParams.size     = (sz);                             \
        EngDeviceIoControl(                                     \
            ppdev->hDriver,                                     \
            (DWORD)IOCTL_NV01_ARCH_HEAP,                        \
            (&pParms),                                          \
            sizeof(PVOID),                                      \
            pParms,                                             \
            sizeof(NVOS11_PARAMETERS),                          \
            &cbReturned                                         \
        );                                                      \
        (sts) = HeapParams.status;                              \
        (pvm) = (unsigned long)(HeapParams.offset);             \
        pDriverData->VideoHeapFree =                            \
            ppdev->VideoHeapFree;                               \
    }                                                           \
                                                                \
    if (!(sts)) {                                               \
        ppdev->VideoHeapFree = HeapParams.free;                 \
        pDriverData->VideoHeapFree =                            \
            HeapParams.free + ppdev->cbGdiHeap;                 \
    }                                                           \
}

#define NVHEAP_ALLOC_C(sts,pvm,sz,tp)                           \
{                                                               \
    NVOS11_PARAMETERS   HeapParams;                             \
    PVOID pParms = (PVOID) &HeapParams;                         \
    DWORD cbReturned;                                           \
                                                                \
    if (pDriverData && !pDriverData->DDrawVideoSurfaceCount && !ppdev->cbGdiHeap)    \
    {                                                           \
        ppdev->cbGdiHeap = ppdev->VideoHeapTotal -              \
            ppdev->VideoHeapFree;                               \
    }                                                           \
                                                                \
    HeapParams.hRoot = ppdev->hClient;                          \
    HeapParams.hObjectParent = ppdev->hDevice;                  \
    HeapParams.function = HEAP_ALLOC_SIZE;                      \
    HeapParams.owner    = 'NVDD';                               \
    HeapParams.type     = (tp);                                 \
    HeapParams.size     = (sz);                                 \
    EngDeviceIoControl(                                         \
        ppdev->hDriver,                                         \
        (DWORD)IOCTL_NV01_ARCH_HEAP,                            \
        (&pParms),                                              \
        sizeof(PVOID),                                          \
        pParms,                                                 \
        sizeof(NVOS11_PARAMETERS),                              \
        &cbReturned                                             \
    );                                                          \
    (sts) = HeapParams.status;                                  \
    (pvm) = (unsigned long)(HeapParams.offset);                 \
                                                                \
    if (sts && ppdev->cbGdiHeap)                                \
    {                                                           \
        bMoveAllDfbsFromOffscreenToDibs(ppdev);                 \
        HeapParams.hRoot = ppdev->hClient;                      \
        HeapParams.hObjectParent = ppdev->hDevice;              \
        HeapParams.function = HEAP_ALLOC_SIZE;                  \
        HeapParams.owner    = 'NVDD';                           \
        HeapParams.type     = (tp);                             \
        HeapParams.size     = (sz);                             \
        EngDeviceIoControl(                                     \
            ppdev->hDriver,                                     \
            (DWORD)IOCTL_NV01_ARCH_HEAP,                        \
            (&pParms),                                          \
            sizeof(PVOID),                                      \
            pParms,                                             \
            sizeof(NVOS11_PARAMETERS),                          \
            &cbReturned                                         \
        );                                                      \
        (sts) = HeapParams.status;                              \
        (pvm) = (unsigned long)(HeapParams.offset);             \
        pDriverData->VideoHeapFree =                            \
            ppdev->VideoHeapFree;                               \
    }                                                           \
                                                                \
    if (!(sts)) {                                               \
        ppdev->VideoHeapFree = HeapParams.free;                 \
        pDriverData->VideoHeapFree =                            \
            HeapParams.free + ppdev->cbGdiHeap;                 \
    }                                                           \
}

#ifndef HEAP_DISABLE_TILING
#define NVHEAP_ALLOC_TILED(sts,pvm,pch,ht,tp)                   \
{                                                               \
    NVOS11_PARAMETERS   HeapParams;                             \
    PVOID pParms = (PVOID) &HeapParams;                         \
    DWORD cbReturned;                                           \
                                                                \
    if (pDriverData && !pDriverData->DDrawVideoSurfaceCount && !ppdev->cbGdiHeap)    \
    {                                                           \
        ppdev->cbGdiHeap = ppdev->VideoHeapTotal -              \
            ppdev->VideoHeapFree;                               \
    }                                                           \
                                                                \
    HeapParams.hRoot = ppdev->hClient;                          \
    HeapParams.hObjectParent = ppdev->hDevice;                  \
    HeapParams.function = HEAP_ALLOC_TILED_PITCH_HEIGHT;        \
    HeapParams.owner    = 'NVDD';                               \
    HeapParams.type     = (tp);                                 \
    HeapParams.pitch    = (pch);                                \
    HeapParams.height   = (ht);                                 \
    EngDeviceIoControl(                                         \
        ppdev->hDriver,                                         \
        (DWORD)IOCTL_NV01_ARCH_HEAP,                            \
        (&pParms),                                              \
        sizeof(PVOID),                                          \
        pParms,                                                 \
        sizeof(NVOS11_PARAMETERS),                              \
        &cbReturned                                             \
    );                                                          \
    (sts) = HeapParams.status;                                  \
    (pvm) = ((DWORD) HeapParams.address - (DWORD) ppdev->pjFrameBufbase);  \
                                                                \
    if (sts && ppdev->cbGdiHeap)                                \
    {                                                           \
        bMoveAllDfbsFromOffscreenToDibs(ppdev);                 \
        HeapParams.hRoot = ppdev->hClient;                      \
        HeapParams.hObjectParent = ppdev->hDevice;              \
        HeapParams.function = HEAP_ALLOC_TILED_PITCH_HEIGHT;    \
        HeapParams.owner    = 'NVDD';                           \
        HeapParams.type     = (tp);                             \
        HeapParams.pitch    = (pch);                            \
        HeapParams.height   = (ht);                             \
        EngDeviceIoControl(                                     \
            ppdev->hDriver,                                     \
            (DWORD)IOCTL_NV01_ARCH_HEAP,                        \
            (&pParms),                                          \
            sizeof(PVOID),                                      \
            pParms,                                             \
            sizeof(NVOS11_PARAMETERS),                          \
            &cbReturned                                         \
        );                                                      \
        (sts) = HeapParams.status;                              \
        (pvm) = ((DWORD) HeapParams.address - (DWORD) ppdev->pjFrameBufbase);  \
        pDriverData->VideoHeapFree =                            \
            ppdev->VideoHeapFree;                               \
    }                                                           \
                                                                \
    if (!(sts)) {                                               \
        (pch) = HeapParams.pitch;                               \
        ppdev->VideoHeapFree = HeapParams.free;                 \
        pDriverData->VideoHeapFree =                            \
            HeapParams.free + ppdev->cbGdiHeap;                 \
    }                                                           \
}

#else // HEAP_DISABLE_TILING
#define NVHEAP_ALLOC_TILED(sts,pvm,pch,ht,tp) NVHEAP_ALLOC(sts,pvm,pch*ht,tp)
#endif

#define NVHEAP_FREE(pvm)                                        \
{                                                               \
    NVOS11_PARAMETERS HeapParams;                               \
    PVOID pParms = (PVOID) &HeapParams;                         \
    DWORD cbReturned;                                           \
    HeapParams.hRoot = ppdev->hClient;                          \
    HeapParams.hObjectParent = ppdev->hDevice;                  \
    HeapParams.function = HEAP_FREE;                            \
    HeapParams.owner    = 'NVDD';                               \
    HeapParams.offset   = (U032)(pvm);                          \
    EngDeviceIoControl(                                         \
        ppdev->hDriver,                                         \
        (DWORD)IOCTL_NV01_ARCH_HEAP,                            \
        (&pParms),                                              \
        sizeof(PVOID),                                          \
        pParms,                                                 \
        sizeof(NVOS11_PARAMETERS),                              \
        &cbReturned                                             \
    );                                                          \
    if (!HeapParams.status) {                                   \
        ppdev->VideoHeapFree = HeapParams.free;                 \
        pDriverData->VideoHeapFree =                            \
            HeapParams.free + ppdev->cbGdiHeap;                 \
    }                                                           \
}

#endif  // WINNT

#define AGP_HEAP 0

}

/*****************************************************************************
 *****************************************************************************
 *****************************************************************************
 ***  CInterProcessHeap ******************************************************
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************/
class CInterProcessHeap
{
    /*
     * internal flags
     */
protected:
    enum
    {
        FLAG_ALLOCATED      = 0x00000001,   // this block is allocated
        FLAG_BOUNDARY       = 0x00000002,   // start of a new block
        FLAG_16BIT          = 0x00000004,   // this block was allocated from 16 bit land

        ALIGN               = 64,           // byte alignment (power of 2)
    };

    /*
     * internal structures
     */
protected:
    struct HEADER
    {
        HEADER *pNext;          // next block
        HEADER *pNextFree;      // next free block
        HEADER *pPrev;          // prev block
        HEADER *pPrevFree;      // prev free block
        DWORD   dwFlags;        // state
        DWORD   dwUserStart;    // start of user memory
        DWORD   dwUserSize;     // size of user memory
        DWORD   dwRealSize;     // size of this block + headers
        DWORD   dwRMID;         // RM ID of mem block, only if alloc'd directly by OS
#ifdef DEBUG
        void   *pCaller;        // pointer to allocation requester
#endif
        // NOTE!!! the dwReserved entry must appear last in this structure
        DWORD   dwReserved;     // may be used by back pointer - depends on alignment
    };


    /*
     * internal members
     */
protected:
    static BOOL m_bInitialized;

protected:
    HEADER *m_pBase;            // 1st block
    HEADER *m_pBaseFree;        // 1st free block
    DWORD   m_dwRMID;
#ifndef WINNT
    DWORD   m_dwRootHandle;     // root handle
    DWORD   m_dwDeviceHandle;   // device handle
#endif

    /*
     * internal helpers
     */
protected:
    inline BOOL isBoundary (HEADER *p) const { return p->dwFlags & FLAG_BOUNDARY;     }
    inline BOOL isFree     (HEADER *p) const { return !(p->dwFlags & FLAG_ALLOCATED); }
    inline BOOL isAlloc    (HEADER *p) const { return p->dwFlags & FLAG_ALLOCATED;    }

    inline void AddToList(HEADER *pHeader) {
        pHeader->pPrev = NULL;
        pHeader->pNext = m_pBase;
        if (m_pBase) m_pBase->pPrev = pHeader;
        m_pBase = pHeader;
    }
    inline void RemoveFromList(HEADER *pHeader) {
        if (pHeader->pNext)     pHeader->pNext->pPrev = pHeader->pPrev;
        if (pHeader->pPrev)     pHeader->pPrev->pNext = pHeader->pNext;
        if (m_pBase == pHeader) m_pBase = pHeader->pNext;
    }
    inline void AddToFreeList(HEADER *pHeader) {
        pHeader->pPrevFree = NULL;
        pHeader->pNextFree = m_pBaseFree;
        if (m_pBaseFree) m_pBaseFree->pPrevFree = pHeader;
        m_pBaseFree = pHeader;
    }
    inline void RemoveFromFreeList(HEADER *pHeader) {
        if (pHeader->pNextFree)     pHeader->pNextFree->pPrevFree = pHeader->pPrevFree;
        if (pHeader->pPrevFree)     pHeader->pPrevFree->pNextFree = pHeader->pNextFree;
        if (m_pBaseFree == pHeader) m_pBaseFree = pHeader->pNextFree;
    }
    inline void CInterProcessHeap::freeBlock(HEADER *pHeader);

#ifdef CHEAP_SANITY_CHECK
           void dbgTestIntegrity (void);
#else
    inline void dbgTestIntegrity (void) const {}
#endif
    HEADER* create            (DWORD dwSize);
    void combineFreeBlocks (void);
    void releaseFreeBlocks (void);

    /*
     * public methods
     */
public:
    void  init    (void);
    void  destroy (void);
    void  checkHeapUsage (void);

#ifdef DEBUG
    void* alloc   (DWORD dwSize, void *pCaller = 0);
#else
    void* alloc   (DWORD dwSize);
#endif
    BOOL  realloc (void *pOldMem, DWORD dwNewSize, void **pNewMem);
    void  free    (void *pMemory);
};

//
// exports
//
extern CInterProcessHeap g_nvIPHeap;

inline void  InitIPM          (void)                { g_nvIPHeap.init();    }
inline void  DestroyIPM       (void)                { g_nvIPHeap.destroy(); }

inline void* AllocIPM         (DWORD dwSize)        { return g_nvIPHeap.alloc(dwSize); }
inline BOOL  ReallocIPM       (void *pOldMem, DWORD dwNewSize, void **ppNewMem) {
                                                      return g_nvIPHeap.realloc(pOldMem, dwNewSize, ppNewMem); }
inline void  FreeIPM          (void *pMemory)       { g_nvIPHeap.free(pMemory); }

//for interfacing with the vertex shader compilers
inline void* glLikeAllocIPM (void *bogus, size_t szSize) { return g_nvIPHeap.alloc((DWORD)szSize); }
inline void glLikeFreeIPM (void *bogus, void* pMemory) { g_nvIPHeap.free(pMemory); }

// all new & deletes use IPM
inline void* _cdecl operator new    (size_t nSize)  { return g_nvIPHeap.alloc(nSize); }
inline void  _cdecl operator delete (void *p)       { if (p) g_nvIPHeap.free(p);      }

// multi heap versions of new & delete
//inline void* _cdecl operator new    (size_t nSize, CInterProcessHeap* pHeap) { return pHeap->alloc(nSize); }
//inline void  _cdecl operator delete (void *p, CInterProcessHeap* pHeap)      { if (p) pHeap->free(p);      }

#endif  // _NVHEAP_H
