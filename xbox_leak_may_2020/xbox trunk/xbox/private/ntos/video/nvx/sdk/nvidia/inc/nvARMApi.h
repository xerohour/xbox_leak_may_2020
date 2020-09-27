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

/*
* nvARmApi.h
*
* NVidia resource manager API header file exported to drivers. (as of now, only
* the audio drivers use it.
*
*/

#if !defined _NVARMAPIH_
#define _NVARMAPIH_

#if defined __cplusplus
extern "C" {
#endif //__cplusplus
    
#include <nvos.h>


// exported functions - OS independent to get to the core

// to be called before anything can be done with the RM
NvU32	NVARM_LoadCore(NvU32 *pDeviceHandle);
void	NVARM_UnloadCore(NvU32 uDeviceHandle);

NvU32	NVARM_Alloc(NvU32 uDeviceHandle, NVOS21_PARAMETERS *pIn);
NvU32	NVARM_Free(NvU32 uDeviceHandle, NV_UNIFIED_FREE *pIn);
    
    
#if defined WIN9XVXD		// change this to whatever the macro is
    
// some imports from resman files... wonder why it's not in common

// this structure is exactly the same as NV_IOCTL_ARCH_STRUCT,
// name's changed to prevent redfinition.
typedef struct
{
    NvU32 nvarchFunction;
    NvU32 nvarchParameters;
} NVA_IOCTL_ARCH_STRUCT;
    
#if !defined NVRM_IOCTL_NV_ARCH
#define NVRM_IOCTL_NV_ARCH				21
#endif
    
#define AVxDCall(service) \
    _asm _emit 0xcd \
    _asm _emit 0x20 \
    _asm _emit (service & 0xff) \
    _asm _emit (service >> 8) & 0xff \
    _asm _emit (service >> 16) & 0xff \
    _asm _emit (service >> 24) & 0xff \
    
NvU32 static __inline
IOCTLCall(NvU32 pDevInfoParam, DIOCPARAMETERS *pParams)
{
    NvU32 dw;
    struct DeviceInfo  *pDevInfo;
    struct VxD_Desc_Block *pDDB;
    
    pDevInfo = (struct DeviceInfo *)pDevInfoParam;
    pDDB = (struct VxD_Desc_Block *)pDevInfo->DI_DDB;
    
    // W32_DEVICEIOCONTROL = 0x23
    _asm mov eax, 0x23
    _asm mov ecx, pDDB
    _asm mov esi, pParams
    AVxDCall(0x00010147);		// Directed_Sys_Control
    _asm mov dw, eax
    return(dw);
}
    
inline NvU32
NVARM_LoadCore(NvU32 *ppDevInfo)
{
    NvU32	dlResult;
    NvU8	*pName = (NvU8*)VXDLDR_NVA_RESMAN_VXD_NAME;
    struct DeviceInfo  *pDevInfo;
    
    _asm	mov	edx, pName
    _asm	mov	eax, 1
    AVxDCall(0x00270001);		// VXDLDR_LoadDevice
    _asm	mov	dlResult, eax
    _asm	jc	LoadDeviceError
    // DDB returned in eax
    _asm	mov	pDevInfo, edx
    
    *ppDevInfo = (NvU32)pDevInfo;
    dlResult = 0;
    
LoadDeviceError:
    return	(dlResult);
}
    
inline VOID
NVARM_UnloadCore(NvU32 pDevInfoParam)
{	
    struct DeviceInfo *pDevInfo;
    
    pDevInfo = (struct DeviceInfo *)pDevInfoParam;
    
    NvU8	*pName = (NvU8 *)pDevInfo->DI_ModuleName;
    NvU32	deviceID = pDevInfo->DI_DeviceID;
    
    _asm	mov	ebx, deviceID
    _asm	mov	edx, pName
    AVxDCall(0x00270002);	// VXDLDR_UnloadDevice
}

inline NvU32
NVARM_Alloc(NvU32 pDevInfo, NVOS21_PARAMETERS *pIn)
{
    DIOCPARAMETERS ioctlParams;
    NVA_IOCTL_ARCH_STRUCT	params;
    
    params.nvarchFunction = NV04_ALLOC;
    params.nvarchParameters = (NvU32)pIn;
    
    ioctlParams.dwIoControlCode = NVRM_IOCTL_NV_ARCH;
    ioctlParams.lpvInBuffer = (NvU32)&params;
    ioctlParams.cbInBuffer = sizeof(NVA_IOCTL_ARCH_STRUCT);
    ioctlParams.lpvOutBuffer = NULL;
    ioctlParams.cbOutBuffer = 0;
    
    return IOCTLCall(pDevInfo, &ioctlParams);
}

inline NvU32
NVARM_Free(NvU32 pDevInfo, NV_UNIFIED_FREE *pIn)
{
    DIOCPARAMETERS ioctlParams;
    NVA_IOCTL_ARCH_STRUCT	params;
    
    params.nvarchFunction = NV04_UNIFIED_FREE;
    params.nvarchParameters = (NvU32)pIn;
    
    ioctlParams.dwIoControlCode = NVRM_IOCTL_NV_ARCH;
    ioctlParams.lpvInBuffer = (NvU32)&params;
    ioctlParams.cbInBuffer = sizeof(NVA_IOCTL_ARCH_STRUCT);
    ioctlParams.lpvOutBuffer = NULL;
    ioctlParams.cbOutBuffer = 0;
    
    return IOCTLCall(pDevInfo, &ioctlParams);
    
}
    
#endif
    
#if defined WDM

// list of imported functions from the resource manager
// the user still needs to link to the .lib

__declspec(dllimport) NvU32 rmAuDispatch(U032 uFunction, PVOID pIn);

inline NvU32
NVARM_LoadCore(NvU32 *ppDevInfo)
{
    // nothing to do really - except maybe load the driver??

    // I was returning '0', but the client thinks '0' is an invalid value
    *ppDevInfo = 0xF;
    return 0;
}

inline VOID
NVARM_UnloadCore(NvU32 pDevInfoParam)
{
    return;
}

inline NvU32
NVARM_Alloc(NvU32 pDevInfo, NVOS21_PARAMETERS *pIn)
{
    return rmAuDispatch(NV04_ALLOC, (VOID *)pIn);
}

inline NvU32
NVARM_Free(NvU32 pDevInfo, NV_UNIFIED_FREE *pIn)
{
    return rmAuDispatch(NV04_UNIFIED_FREE, (VOID *)pIn);
}

#endif		// WDM

#if defined __cplusplus
}
#endif //__cplusplus

#endif // _NVARMAPIH_