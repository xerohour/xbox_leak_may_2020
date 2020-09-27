 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  NVIDIA, Corp. of Sunnyvale,  California owns     *|
|*     copyrights, patents, and has design patents pending on the design     *|
|*     and  interface  of the NV chips.   Users and  possessors  of this     *|
|*     source code are hereby granted a nonexclusive, royalty-free copy-     *|
|*     right  and design patent license  to use this code  in individual     *|
|*     and commercial software.                                              *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright  1993-1998  NVIDIA,  Corporation.   NVIDIA  has  design     *|
|*     patents and patents pending in the U.S. and foreign countries.        *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF THIS SOURCE CODE FOR ANY PURPOSE. IT IS PROVIDED "AS IS" WITH-     *|
|*     OUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPORATION     *|
|*     DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOURCE CODE, INCLUD-     *|
|*     ING ALL IMPLIED WARRANTIES  OF MERCHANTABILITY  AND FITNESS FOR A     *|
|*     PARTICULAR  PURPOSE.  IN NO EVENT  SHALL NVIDIA,  CORPORATION  BE     *|
|*     LIABLE FOR ANY SPECIAL,  INDIRECT,  INCIDENTAL,  OR CONSEQUENTIAL     *|
|*     DAMAGES, OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,     *|
|*     DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR     *|
|*     OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION  WITH THE     *|
|*     USE OR PERFORMANCE OF THIS SOURCE CODE.                               *|
|*                                                                           *|
|*     RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the     *|
|*     Government is subject  to restrictions  as set forth  in subpara-     *|
|*     graph (c) (1) (ii) of the Rights  in Technical Data  and Computer     *|
|*     Software  clause  at DFARS  52.227-7013 and in similar clauses in     *|
|*     the FAR and NASA FAR Supplement.                                      *|
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

// local defines here
#define FILE_DEVICE_NV 		0x00008000
#define NV_IOCTL_FCT_BASE 	0x00000800

// Use these defines if you are opening the RM vxd or the minivdd using the
// CreateFile inetrface.
// NV3 Resource Manager VXDs
#define NV3_PRIMARY_MINIVDD     "\\\\.\\NV3.VXD"
#define NV3_SECONDARY_MINIVDD   "\\\\.\\NV3MINI2.VXD"
#define NV3_RESMAN_VXD          "\\\\.\\NV3RM.VXD"
// NV4/NV10 Resource Manager VXDs
#define NVX_PRIMARY_MINIVDD     "\\\\.\\NVMINI.VXD"
#define NVX_SECONDARY_MINIVDD   "\\\\.\\NVMINI2.VXD"
#define NVX_RESMAN_VXD          "\\\\.\\NVCORE.VXD"

// Use this define for the resman vxd name if you use the VXDLDR_LoadDevice
// interface.
#define VXDLDR_NV3_RESMAN_VXD_NAME "NV3RM.VXD"
#define VXDLDR_NVX_RESMAN_VXD_NAME "NVCORE.VXD"
#define VXDLDR_NVA_RESMAN_VXD_NAME "NVACORE.VXD"

#if defined(WIN32)
#if !defined(NV_APIENTRY)
#define NV_APIENTRY __stdcall
#ifndef APIENTRY
#define APIENTRY    NV_APIENTRY          // APIENTRY deprecated; use NV_APIENTRY
#endif
#define CAPI        NV_APIENTRY          // CAPI deprecated; use NV_APIENTRY
#endif
#if !defined(NV_FASTCALL)
#define NV_FASTCALL __fastcall
//#define FASTCALL    NV_FASTCALL          // FASTCALL deprecated; use NV_FASTCALL
#endif
#if !defined(NV_INLINE)
#define NV_INLINE __inline
#define INLINE      NV_INLINE            // INLINE deprecated; use NV_INLINE
#endif
#if !defined(NV_CDECLCALL)
#define NV_CDECLCALL __cdecl
#define CDECLCALL   NV_CDECLCALL         // CDECLCALL deprecated; use NV_CDECLCALL
#endif
#if !defined(NV_STDCALL)
#define NV_STDCALL __stdcall
#define STDCALL     NV_STDCALL           // STDCALL deprecated; use NV_STDCALL
#endif

#else /* ! defined(WIN32) */

#define NV_APIENTRY
#define NV_FASTCALL
#define NV_CDECLCALL
#define NV_STDCALL 
#ifdef __GNUC__
#define NV_INLINE     __inline__
#elif defined (macintosh)
#define NV_INLINE inline
#else
#define NV_INLINE
#endif

#endif  /* ! WIN32 */

#define NV_CAPI	NV_APIENTRY


 /***************************************************************************\
|*                              NV OS Functions                              *|
 \***************************************************************************/

/*
	Note:
		This version of the architecture has been changed to allow the 
		RM to return a client handle that will subsequently used to
		identify the client.  NvAllocRoot() returns the handle.  All
		other functions must specify this client handle. 

*/
/* macro NV01_FREE */
#define  NV01_FREE                                                 (0x00000000)

/* parameter values */
#define NVOS00_STATUS_SUCCESS                                      (0x00000000)
#define NVOS00_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS00_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS00_STATUS_ERROR_BAD_OBJECT_OLD                         (0x00000003)
#define NVOS00_STATUS_ERROR_OBJECT_IN_USE                          (0x00000004)
#define NVOS00_STATUS_ERROR_OBJECT_HAS_CHILDERN                    (0x00000005)


/* NT ioctl data structure */
typedef struct
{
  NvV32 hRoot;
  NvV32 hObjectParent;
  NvV32 hObjectOld;
  NvV32 status;
} NVOS00_PARAMETERS;

/* macro NV01_ALLOC_ROOT */
#define  NV01_ALLOC_ROOT                                           (0x00000001)

/* status values */
#define NVOS01_STATUS_SUCCESS                                      (0x00000000)
#define NVOS01_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS01_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS01_STATUS_ERROR_BAD_OBJECT_NEW                         (0x00000003)
#define NVOS01_STATUS_ERROR_BAD_CLASS                              (0x00000004)
#define NVOS01_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000005)

/* parameters */
typedef struct
{
    NvV32 hObjectNew;
    NvV32 hClass;
    NvV32 status;
} NVOS01_PARAMETERS;

/* valid hClass values. */
#define  NV01_ROOT                                                 (0x00000000)
#define  NV01_ROOT_USER                                            (0x00000041)

/* macro NV01_ALLOC_MEMORY */
#define  NV01_ALLOC_MEMORY                                         (0x00000002)

/* parameter values */
#define NVOS02_FLAGS_PHYSICALITY                                   7:4
#define NVOS02_FLAGS_PHYSICALITY_CONTIGUOUS                        (0x00000000)
#define NVOS02_FLAGS_PHYSICALITY_NONCONTIGUOUS                     (0x00000001)
#define NVOS02_FLAGS_LOCATION                                      11:8
#define NVOS02_FLAGS_LOCATION_PCI                                  (0x00000000)
#define NVOS02_FLAGS_LOCATION_AGP                                  (0x00000001)
#define NVOS02_FLAGS_COHERENCY                                     15:12
#define NVOS02_FLAGS_COHERENCY_UNCACHED                            (0x00000000)
#define NVOS02_FLAGS_COHERENCY_CACHED                              (0x00000001)
#define NVOS02_FLAGS_COHERENCY_WRITE_COMBINE                       (0x00000002)
#define NVOS02_FLAGS_COHERENCY_WRITE_THROUGH                       (0x00000003)
#define NVOS02_FLAGS_COHERENCY_WRITE_PROTECT                       (0x00000004)
#define NVOS02_FLAGS_COHERENCY_WRITE_BACK                          (0x00000005)
#define NVOS02_STATUS_SUCCESS                                      (0x00000000)
#define NVOS02_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS02_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS02_STATUS_ERROR_BAD_OBJECT_NEW                         (0x00000003)
#define NVOS02_STATUS_ERROR_BAD_CLASS                              (0x00000004)
#define NVOS02_STATUS_ERROR_BAD_FLAGS                              (0x00000005)
#define NVOS02_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000006)

/* parameters */
typedef struct 
{
  NvV32 hRoot;
  NvV32 hObjectParent;
  NvV32 hObjectNew;
  NvV32 hClass;
  NvV32 flags;
  NvP64 pMemory;
  NvU64 pLimit;
  NvV32 status;
} NVOS02_PARAMETERS;

/* macro NV01_ALLOC_CONTEXT_DMA */
#define  NV01_ALLOC_CONTEXT_DMA                                    (0x00000003)

/* parameter values */
#define NVOS03_FLAGS_ACCESS                                        3:0
#define NVOS03_FLAGS_ACCESS_READ_WRITE                             (0x00000000)
#define NVOS03_FLAGS_ACCESS_READ_ONLY                              (0x00000001)
#define NVOS03_FLAGS_ACCESS_WRITE_ONLY                             (0x00000002)
#define NVOS03_FLAGS_PHYSICALITY                                   7:4
#define NVOS03_FLAGS_PHYSICALITY_CONTIGUOUS                        (0x00000000)
#define NVOS03_FLAGS_PHYSICALITY_NONCONTIGUOUS                     (0x00000001)
#define NVOS03_FLAGS_LOCKED                                        11:8
#define NVOS03_FLAGS_LOCKED_ALWAYS                                 (0x00000000)
#define NVOS03_FLAGS_LOCKED_IN_TRANSIT                             (0x00000001)
#define NVOS03_FLAGS_COHERENCY                                     31:12
#define NVOS03_FLAGS_COHERENCY_UNCACHED                            (0x00000000)
#define NVOS03_FLAGS_COHERENCY_CACHED                              (0x00000001)
#define NVOS03_STATUS_SUCCESS                                      (0x00000000)
#define NVOS03_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS03_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS03_STATUS_ERROR_BAD_OBJECT_NEW                         (0x00000003)
#define NVOS03_STATUS_ERROR_BAD_CLASS                              (0x00000004)
#define NVOS03_STATUS_ERROR_BAD_FLAGS                              (0x00000005)
#define NVOS03_STATUS_ERROR_BAD_BASE                               (0x00000006)
#define NVOS03_STATUS_ERROR_BAD_LIMIT                              (0x00000007)
#define NVOS03_STATUS_ERROR_PROTECTION_FAULT                       (0x00000008)
#define NVOS03_STATUS_ERROR_MULTIPLE_MEMORY_TYPES                  (0x00000009)
#define NVOS03_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x0000000A)

/* parameters */
typedef struct
{
    NvV32 hObjectParent;
    NvV32 hObjectNew;
    NvV32 hClass;
    NvV32 flags;
    NvP64 pBase;
    NvU64 limit;
    NvV32 status;
} NVOS03_PARAMETERS;

/* macro NV01_ALLOC_CHANNEL_PIO */
#define  NV01_ALLOC_CHANNEL_PIO                                    (0x00000004)

/* parameter values */
#define NVOS04_FLAGS_FIFO_RUNOUT_IGNORE                            (0x00000000)
#define NVOS04_FLAGS_FIFO_RUNOUT_ERROR                             (0x00000001)
#define NVOS04_STATUS_SUCCESS                                      (0x00000000)
#define NVOS04_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS04_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS04_STATUS_ERROR_BAD_OBJECT_NEW                         (0x00000003)
#define NVOS04_STATUS_ERROR_BAD_CLASS                              (0x00000004)
#define NVOS04_STATUS_ERROR_BAD_OBJECT_ERROR                       (0x00000005)
#define NVOS04_STATUS_ERROR_BAD_FLAGS                              (0x00000006)
#define NVOS04_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000007)

/* parameters */
typedef struct
{
    NvV32 hRoot;
    NvV32 hObjectParent;
    NvV32 hObjectNew;
    NvV32 hClass;
    NvV32 hObjectError;
    NvP64 pChannel;
    NvV32 flags;
    NvV32 status;
} NVOS04_PARAMETERS;

/* macro NV01_ALLOC_OBJECT */
#define  NV01_ALLOC_OBJECT                                         (0x00000005)

/* parameter values */
#define NVOS05_STATUS_SUCCESS                                      (0x00000000)
#define NVOS05_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS05_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS05_STATUS_ERROR_BAD_OBJECT_NEW                         (0x00000003)
#define NVOS05_STATUS_ERROR_BAD_CLASS                              (0x00000004)
#define NVOS05_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000005)

/* parameters */
typedef struct
{
  NvV32 hRoot;
  NvV32 hObjectParent;
  NvV32 hObjectNew;
  NvV32 hClass;
  NvV32 status;
} NVOS05_PARAMETERS;

// ***** this has been changed to allow a device name string to be returned
/* macro NV01_ALLOC_DEVICE */
#define  NV01_ALLOC_DEVICE                                         (0x00000006)

/* parameter values */
#define NVOS06_STATUS_SUCCESS                                      (0x00000000)
#define NVOS06_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS06_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS06_STATUS_ERROR_BAD_OBJECT_NEW                         (0x00000003)
#define NVOS06_STATUS_ERROR_BAD_CLASS                              (0x00000004)
#define NVOS06_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000005)

/* parameters */
typedef struct
{
    NvV32 hObjectParent;
    NvV32 hObjectNew;
    NvV32 hClass;
#if _WIN32_WINNT >= 0x0400
    NvP64 szName;
#else
    NvV32 szName;
#endif
    NvV32 status;
} NVOS06_PARAMETERS;

/* macro NV03_ALLOC_CHANNEL_DMA */
#define  NV03_ALLOC_CHANNEL_DMA                                    (0x00000007)

/* parameter values */
#define NVOS07_STATUS_SUCCESS                                      (0x00000000)
#define NVOS07_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS07_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS07_STATUS_ERROR_BAD_OBJECT_NEW                         (0x00000003)
#define NVOS07_STATUS_ERROR_BAD_CLASS                              (0x00000004)
#define NVOS07_STATUS_ERROR_BAD_OBJECT_ERROR                       (0x00000005)
#define NVOS07_STATUS_ERROR_BAD_OBJECT_BUFFER                      (0x00000006)
#define NVOS07_STATUS_ERROR_BAD_OFFSET                             (0x00000007)
#define NVOS07_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000008)

/* parameters */
typedef struct
{
  NvV32 hRoot;
  NvV32 hObjectParent;
  NvV32 hObjectNew;
  NvV32 hClass;
  NvV32 hObjectError;
  NvV32 hObjectBuffer;
  NvU32 offset;
  NvP64 pControl;
  NvV32 status;
} NVOS07_PARAMETERS;

/* macro NV03_DMA_FLOW_CONTROL */
#define  NV03_DMA_FLOW_CONTROL                                     (0x00000008)

/* parameter values */
#define NVOS08_FLAGS_PUT                                           3:0
#define NVOS08_FLAGS_PUT_IGNORE                                    (0x00000000)
#define NVOS08_FLAGS_PUT_UPDATE                                    (0x00000001)
#define NVOS08_FLAGS_JUMP                                          31:4
#define NVOS08_FLAGS_JUMP_IGNORE                                   (0x00000000)
#define NVOS08_FLAGS_JUMP_UPDATE                                   (0x00000001)
#define NVOS08_STATUS_SUCCESS                                      (0x00000000)
#define NVOS08_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS08_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS08_STATUS_ERROR_BAD_FLAGS                              (0x00000003)
#define NVOS08_STATUS_ERROR_BAD_PUT                                (0x00000004)

/* parameters */
typedef struct
{
  NvV32 hChannel;
  NvV32 flags;
  NvU32 put;
  NvU32 get;
  NvV32 status;
} NVOS08_PARAMETERS;

/* macro NV01_INTERRUPT */
#define  NV01_INTERRUPT                                            (0x00000009)

/* parameter values */
#define NVOS09_STATUS_SUCCESS                                      (0x00000000)
#define NVOS09_STATUS_ERROR_BAD_CLIENT                             (0x00000001)
#define NVOS09_STATUS_ERROR_BAD_DEVICE                             (0x00000002)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 status;
} NVOS09_PARAMETERS;

/* macro NV01_ALLOC_EVENT */
#define  NV01_ALLOC_EVENT                                          (0x0000000A)

/* parameter values */
#define NVOS10_STATUS_SUCCESS                                      (0x00000000)
#define NVOS10_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS10_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS10_STATUS_ERROR_BAD_OBJECT_NEW                         (0x00000003)
#define NVOS10_STATUS_ERROR_BAD_CLASS                              (0x00000004)
#define NVOS10_STATUS_ERROR_BAD_INDEX                              (0x00000005)
#define NVOS10_STATUS_ERROR_BAD_EVENT                              (0x00000006)
#define NVOS10_STATUS_ERROR_OBJECT_IN_USE                          (0x00000007)

/* parameters */
typedef struct
{
  NvV32 hRoot;
  NvV32 hObjectParent;
  NvV32 hObjectNew;
  NvV32 hClass;
  NvV32 index;
  NvU64 hEvent;
  NvV32 status;
} NVOS10_PARAMETERS;

/* Valid values for hClass in Nv01AllocEvent */
/* Note that NV01_EVENT_OS_EVENT is same as NV01_EVENT_WIN32_EVENT */
/* TODO: delete the WIN32 name */
#define  NV01_EVENT_KERNEL_CALLBACK                                (0x00000078)
#define  NV01_EVENT_OS_EVENT                                       (0x00000079)
#define  NV01_EVENT_WIN32_EVENT                                    NV01_EVENT_OS_EVENT

//
// There's some definite ugly reuse of args in NV01_ARCH_HEAP for the various
// functions. We'd like to rename field names when adding funcs, so the args
// go in the appropriate places.
//
// For example, adding HEAP_INFO_FREE_BLOCKS, should take offset/offset2 args,
// but because it's not a union and some of the drivers refer to their own typedef
// for the struct, we reuse field names that don't apply to the func.
//

/* funct NV03_HEAP */
#define  NV01_ARCH_HEAP                                            (0x0000000B)
/* parameters */
typedef struct
{
  NvV32 hRoot;
  NvV32 hObjectParent;
  NvU32 function;
  NvU32 owner;
  NvU32 type;       /* holds 16bits of flags and 16bits of type */
  NvU32 depth;
  NvU32 width;
  NvU32 height;
  NvS32 pitch;
  NvU32 offset;     // for NVOS11_HEAP_INFO_FREE_BLOCKS, offset to be considered freed
  NvU32 size;       // for NVOS11_HEAP_INFO, size of largest free block
#if _WIN32_WINNT >= 0x0400
  NvP64 address;    // for NVOS11_HEAP_INFO, base address of heap
#else
  NvU32 address;    // for NVOS11_HEAP_INFO, base address of heap
#endif
  NvU32 limit;      // for NVOS11_HEAP_INFO_FREE_BLOCKS, offset to be considered freed
  NvU32 total;
  NvU32 free;
  NvV32 status;
} NVOS11_PARAMETERS;
/* parameter values */
#define NVOS11_HEAP_ALLOC_DEPTH_WIDTH_HEIGHT        1
#define NVOS11_HEAP_ALLOC_SIZE                      2
#define NVOS11_HEAP_FREE                            3
#define NVOS11_HEAP_PURGE                           4
#define NVOS11_HEAP_INFO                            5
#define NVOS11_HEAP_ALLOC_TILED_PITCH_HEIGHT        6
#define NVOS11_HEAP_DESTROY                         7
#define NVOS11_HEAP_INFO_FREE_BLOCKS                8
#define NVOS11_TYPE_IMAGE                           0
#define NVOS11_TYPE_DEPTH                           1
#define NVOS11_TYPE_TEXTURE                         2
#define NVOS11_TYPE_VIDEO_OVERLAY                   3
#define NVOS11_TYPE_FONT                            4
#define NVOS11_TYPE_CURSOR                          5
#define NVOS11_TYPE_DMA                             6
#define NVOS11_TYPE_INSTANCE                        7
#define NVOS11_TYPE_PRIMARY                         8
#define NVOS11_TYPE_IMAGE_TILED                     9
#define NVOS11_TYPE_DEPTH_COMPR16                   10
#define NVOS11_TYPE_DEPTH_COMPR32                   11
#define NVOS11_TYPE_FLAGS                                31:16
#define NVOS11_TYPE_FLAGS_IGNORE_BANK_PLACEMENT     0x00000001
#define NVOS11_TYPE_FLAGS_FORCE_MEM_GROWS_UP        0x00000002
#define NVOS11_TYPE_FLAGS_FORCE_MEM_GROWS_DOWN      0x00000004
#define NVOS11_INVALID_BLOCK_FREE_OFFSET            0xFFFFFFFF
#define NVOS11_STATUS_SUCCESS                       (0x00000000)
#define NVOS11_STATUS_ERROR_INSUFFICIENT_RESOURCES  (0x00000001)
#define NVOS11_STATUS_ERROR_INVALID_FUNCTION        (0x00000002)
#define NVOS11_STATUS_ERROR_INVALID_OWNER           (0x00000003)
#define NVOS11_STATUS_ERROR_INVALID_HEAP            (0x00000004)

/* function OS0C */
#define  NV01_CONFIG_VERSION                                       (0x0000000C)

/* parameter values */
#define NVOS12_STATUS_SUCCESS                                      (0x00000000)
#define NVOS12_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS12_STATUS_ERROR_BAD_CLIENT                             (0x00000002)
#define NVOS12_STATUS_ERROR_BAD_DEVICE                             (0x00000003)
#define NVOS12_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000005)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 version;
  NvV32 status;
} NVOS12_PARAMETERS;

/* function OS0D */
#define  NV01_CONFIG_GET                                           (0x0000000D)

/* parameter values */
#define NVOS13_STATUS_SUCCESS                                      (0x00000000)
#define NVOS13_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS13_STATUS_ERROR_BAD_CLIENT                             (0x00000002)
#define NVOS13_STATUS_ERROR_BAD_DEVICE                             (0x00000003)
#define NVOS13_STATUS_ERROR_BAD_INDEX                              (0x00000004)
#define NVOS13_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000005)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 index;
  NvV32 value;
  NvV32 status;
} NVOS13_PARAMETERS;

/* function OS0E */
#define  NV01_CONFIG_SET                                           (0x0000000E)

/* parameter values */
#define NVOS14_STATUS_SUCCESS                                      (0x00000000)
#define NVOS14_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS14_STATUS_ERROR_BAD_CLIENT                             (0x00000002)
#define NVOS14_STATUS_ERROR_BAD_DEVICE                             (0x00000003)
#define NVOS14_STATUS_ERROR_BAD_INDEX                              (0x00000004)
#define NVOS14_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000005)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 index;
  NvV32 oldValue;
  NvV32 newValue;
  NvV32 status;
} NVOS14_PARAMETERS;

/* function OS0F */
#define  NV01_CONFIG_UPDATE                                        (0x0000000F)

/* parameter values */
#define NVOS15_STATUS_SUCCESS                                      (0x00000000)
#define NVOS15_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS15_STATUS_ERROR_BAD_CLIENT                             (0x00000002)
#define NVOS15_STATUS_ERROR_BAD_DEVICE                             (0x00000003)
#define NVOS15_STATUS_ERROR_BAD_FLAGS                              (0x00000004)
#define NVOS15_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000005)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 flags;
  NvV32 status;
} NVOS15_PARAMETERS;

/* function OS16 */
#define  NV04_RING0_CALLBACK                                        (0x00000010)

/* parameter values */
#define NVOS16_STATUS_SUCCESS                                       (0x00000000)

/* callback function prototype */
typedef NvU32 (*RING0CALLBACKPROC)(NvU32, NvU32);

/* parameters */
typedef struct
{
    NvV32             hClient;
    NvV32             hDevice;
#if _WIN32_WINNT >= 0x0400
    NvP64             functionPtr;
#else
    RING0CALLBACKPROC functionPtr;
#endif
    NvU32             param1;
    NvU32             param2;
    NvV32             status;
} NVRM_RING0CALLBACK_PARAMS;

/* function OS17 */
#define  NV04_CONFIG_GET_EX                                        (0x00000011)

/* parameter values */
#define NVOS_CGE_STATUS_SUCCESS                                    (0x00000000)
#define NVOS_CGE_STATUS_ERROR_OPERATING_SYSTEM                     (0x00000001)
#define NVOS_CGE_STATUS_ERROR_BAD_CLIENT                           (0x00000002)
#define NVOS_CGE_STATUS_ERROR_BAD_DEVICE                           (0x00000003)
#define NVOS_CGE_STATUS_ERROR_BAD_INDEX                            (0x00000004)
#define NVOS_CGE_STATUS_ERROR_INSUFFICIENT_RESOURCES               (0x00000005)
#define NVOS_CGE_STATUS_ERROR_BAD_PARAM_STRUCT                     (0x00000006)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 index;
#if _WIN32_WINNT >= 0x0400
  NvP64 paramStructPtr;
#else
  NvV32 paramStructPtr;
#endif
  NvU32 paramSize;
  NvV32 status;
} NVOS_CONFIG_GET_EX_PARAMS;

/* function OS18 */
#define  NV04_CONFIG_SET_EX                                        (0x00000012)

/* parameter values */
#define NVOS_CSE_STATUS_SUCCESS                                    (0x00000000)
#define NVOS_CSE_STATUS_ERROR_OPERATING_SYSTEM                     (0x00000001)
#define NVOS_CSE_STATUS_ERROR_BAD_CLIENT                           (0x00000002)
#define NVOS_CSE_STATUS_ERROR_BAD_DEVICE                           (0x00000003)
#define NVOS_CSE_STATUS_ERROR_BAD_INDEX                            (0x00000004)
#define NVOS_CSE_STATUS_ERROR_INSUFFICIENT_RESOURCES               (0x00000005)
#define NVOS_CSE_STATUS_ERROR_BAD_PARAM_STRUCT                     (0x00000006)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 index;
#if _WIN32_WINNT >= 0x0400
  NvP64 paramStructPtr;
#else
  NvV32 paramStructPtr;
#endif
  NvU32 paramSize;
  NvV32 status;
} NVOS_CONFIG_SET_EX_PARAMS;

/* function OS19 */
#define  NV04_I2C_ACCESS                                           (0x00000013)

/* parameter values */
#define NVOS_I2C_ACCESS_STATUS_SUCCESS                             (0x00000000)
#define NVOS_I2C_ACCESS_STATUS_ERROR_OPERATING_SYSTEM              (0x00000001)
#define NVOS_I2C_ACCESS_STATUS_ERROR_BAD_CLIENT                    (0x00000002)
#define NVOS_I2C_ACCESS_STATUS_ERROR_BAD_DEVICE                    (0x00000003)
#define NVOS_I2C_ACCESS_STATUS_ERROR_INSUFFICIENT_RESOURCES        (0x00000004)
#define NVOS_I2C_ACCESS_STATUS_ERROR_BAD_PARAM_STRUCT              (0x00000005)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
#if _WIN32_WINNT >= 0x0400
  NvP64 paramStructPtr;
#else
  NvV32 paramStructPtr;
#endif
  NvU32 paramSize;
  NvV32 status;
} NVOS_I2C_ACCESS_PARAMS;

/* function OS20 */
#define  NV01_DEBUG_CONTROL                                        (0x000000014)

/* parameter values */
#define NVOS20_STATUS_SUCCESS                                      (0x00000000)
#define NVOS20_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS20_STATUS_ERROR_BAD_DEVICE                             (0x00000002)
#define NVOS20_STATUS_ERROR_BAD_FLAGS                              (0x00000003)
#define NVOS20_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000004)

/* parameters */
typedef struct 
{
    NvV32 hRoot;
    NvV32 command;
    NvP64 pArgs;
    NvV32 status;
} NVOS20_PARAMETERS;

/* current values for command */
#define NVOS20_COMMAND_RM_FAILURE_ENABLE           0x0001
#define NVOS20_COMMAND_RM_FAILURE_DISABLE          0x0002

/* function OS21 */
#define  NV04_ALLOC                                                (0x00000015)

/* parameter values */
#define NVOS21_STATUS_SUCCESS                                      (0x00000000)
#define NVOS21_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS21_STATUS_ERROR_BAD_ROOT                               (0x00000002)
#define NVOS21_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000003)
#define NVOS21_STATUS_ERROR_BAD_OBJECT_NEW                         (0x00000004)
#define NVOS21_STATUS_ERROR_BAD_CLASS                              (0x00000005)
#define NVOS21_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000006)

/* parameters */
typedef struct
{
  NvV32 hRoot;
  NvV32 hObjectParent;
  NvV32 hObjectNew;
  NvV32 hClass;
  NvP64 pAllocParms;
  NvV32 status;
} NVOS21_PARAMETERS;

/* function OS22 */
#define  NV04_POWER_MANAGEMENT                                     (0x00000016)

/* parameter values */
#define NVOS22_STATUS_SUCCESS                                      (0x00000000)
#define NVOS22_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS22_STATUS_ERROR_BAD_ROOT                               (0x00000002)
#define NVOS22_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000003)
#define NVOS22_STATUS_ERROR_BAD_CLASS                              (0x00000005)
#define NVOS22_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000006)
#define NVOS22_STATUS_ERROR_NO_DEVICE                              (0x00000007)

/* parameters */
typedef struct
{
  NvV32 hRoot;
  NvV32 hDevice;
  NvV32 command;
  NvV32 head;
  void *params;
  NvV32 status;
} NVOS22_PARAMETERS;

/* Command values */

#define NVOS22_POWER_SLEEP               0x0001
#define NVOS22_POWER_WAKE                0x0002
#define NVOS22_POWER_WAKE_BIOS_HAS_RUN   0x0003
#define NVOS22_POWER_DOZE                0x0004
#define NVOS22_POWER_WAKE_TO_DOZE        0x0005
#define NVOS22_POWER_GET_LEVEL           0x0008

/* function OS21 */
#define  NV10_AGP_INIT                                             (0x000000015)
#define  NV10_AGP_TEARDOWN                                         (0x000000016)

/* parameters */
typedef struct 
{
  NvV32 hDevice;
  NvV32 config;
  NvV32 status;
} NVOS_AGP_PARAMS;

/* parameter values */
#define NVOS_AGP_CONFIG_DISABLE_AGP                            	        (0x00000000)
#define NVOS_AGP_CONFIG_NVAGP                                           (0x00000001)
#define NVOS_AGP_CONFIG_AGPGART                                         (0x00000002)
#define NVOS_AGP_CONFIG_ANYAGP                                          (0x00000003)

#define NVOS_AGP_STATUS_SUCCESS                             		(0x00000000)
#define NVOS_AGP_STATUS_ERROR_OPERATING_SYSTEM              		(0x00000001)
#define NVOS_AGP_STATUS_ERROR_BAD_CLIENT                           	(0x00000002)
#define NVOS_AGP_STATUS_ERROR_BAD_DEVICE                           	(0x00000003)

/* function OS25 */
#define NV04_OSCONFIG_SET                                           (0x00000019)

/* parameter values */
#define NVOS25_STATUS_SUCCESS                                      (0x00000000)
#define NVOS25_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS25_STATUS_ERROR_BAD_CLIENT                             (0x00000002)
#define NVOS25_STATUS_ERROR_BAD_DEVICE                             (0x00000003)
#define NVOS25_STATUS_ERROR_BAD_INDEX                              (0x00000004)
#define NVOS25_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000005)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 index;
  NvV32 oldValue;
  NvV32 newValue;
  NvV32 status;
} NVOS25_PARAMETERS;

/* function OS26 */
#define NV04_OSCONFIG_GET                                           (0x0000001A)

/* parameter values */
#define NVOS26_STATUS_SUCCESS                                      (0x00000000)
#define NVOS26_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS26_STATUS_ERROR_BAD_CLIENT                             (0x00000002)
#define NVOS26_STATUS_ERROR_BAD_DEVICE                             (0x00000003)
#define NVOS26_STATUS_ERROR_BAD_INDEX                              (0x00000004)
#define NVOS26_STATUS_ERROR_INSUFFICIENT_RESOURCES                 (0x00000005)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 index;
  NvV32 value;
  NvV32 status;
} NVOS26_PARAMETERS;

/* function OS27 */
#define NV04_OSCONFIG_GET_EX                                        (0x0000001B)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 index;
#if _WIN32_WINNT >= 0x0400
  NvP64 paramStructPtr;
#else
  NvV32 paramStructPtr;
#endif
  NvU32 paramSize;
  NvV32 status;
} NVOS_OSCONFIG_GET_EX_PARAMS;

/* function OS28 */
#define NV04_OSCONFIG_SET_EX                                        (0x0000001C)

/* parameters */
typedef struct 
{
  NvV32 hClient;
  NvV32 hDevice;
  NvV32 index;
#if _WIN32_WINNT >= 0x0400
  NvP64 paramStructPtr;
#else
  NvV32 paramStructPtr;
#endif
  NvU32 paramSize;
  NvV32 status;
} NVOS_OSCONFIG_SET_EX_PARAMS;

/* macro NV04_DIRECT_METHOD_CALL */
#define NV04_DIRECT_METHOD_CALL                                    (0x0000001D)

/* parameter values */
#define NVOS1D_STATUS_SUCCESS                                      (0x00000000)
#define NVOS1D_STATUS_ERROR_OPERATING_SYSTEM                       (0x00000001)
#define NVOS1D_STATUS_ERROR_BAD_OBJECT_PARENT                      (0x00000002)
#define NVOS1D_STATUS_ERROR_BAD_OBJECT_OLD                         (0x00000003)
#define NVOS1D_STATUS_ERROR_BAD_METHOD                             (0x00000004)
#define NVOS1D_STATUS_ERROR_BAD_DATA                               (0x00000005)

/* NT ioctl data structure */
typedef struct
{
  NvV32 hRoot;
  NvV32 hObjectParent;
  NvV32 hObjectOld;
  NvV32 method;
  NvV32 data;
  NvV32 status;
} NVOS1D_PARAMETERS;

#define NV04_UNIFIED_FREE											(0x0000001E)

#define NVOS1E_STATUS_SUCCESS										(0x00000000)
#define NVOS1E_STATUS_ERROR_OPERATING_SYSTEM						(0x00000001)
#define NVOS1E_STATUS_ERROR_OBJECT_IN_USE							(0x00000004)

/* NT ioctl data structure */
typedef struct
{
	NvV32	hClient;
	NvV32	hClass;
	NvU32	uDeviceRef;
	NvP64	pParam;
	NvV32	status;
} NV_UNIFIED_FREE;


#ifdef __cplusplus
};
#endif
#endif /* NVOS_INCLUDED */

