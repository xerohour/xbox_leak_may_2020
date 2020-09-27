// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: ddCapture.h
//      macros used by a capture driver (this is a local extension
//      to the shared file nvCapture.h)
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        24May00         created
//
// **************************************************************************

#ifndef _DDCAPTURE_H
#define _DDCAPTURE_H

#ifdef CAPTURE

//////////////////////////////////////////////////////////////////////////////
// override object creation
//
inline ULONG __capture_NvRmAllocObject (ULONG hClient, ULONG hChannel, ULONG hObject, ULONG hClass)
{
    if (!global.b16BitCode)
    {
        if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
            CAPTURE_OBJECT_CREATE create;
            create.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            create.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            create.dwExtensionID        = CAPTURE_XID_OBJECT_CREATE;
            create.dwClassID            = hClass;
            create.dwHandle             = hObject;
            captureLog (&create,sizeof(create));
        }
    }
    return NvRmAllocObject (hClient, hChannel, hObject, hClass);
}
#define NvRmAllocObject __capture_NvRmAllocObject   // alias to CAPTURE create

//////////////////////////////////////////////////////////////////////////////
// override second flavor of object creation
//
inline ULONG __capture_NvRmAlloc (ULONG hClient, ULONG hParent, ULONG hObject, ULONG hClass, PVOID pAllocParms)
{
    if (!global.b16BitCode)
    {
        if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
            CAPTURE_OBJECT_CREATE create;
            create.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            create.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            create.dwExtensionID        = CAPTURE_XID_OBJECT_CREATE;
            create.dwClassID            = hClass;
            create.dwHandle             = hObject;
            captureLog (&create,sizeof(create));
        }
    }
    return NvRmAlloc (hClient, hParent, hObject, hClass, pAllocParms);
}
#define NvRmAlloc __capture_NvRmAlloc

//////////////////////////////////////////////////////////////////////////////
// override context DMA creation
//
inline ULONG __capture_NvRmAllocContextDma (ULONG hClient, ULONG hDma, ULONG hClass, ULONG flags, PVOID base, ULONG limit)
{
    if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
        CAPTURE_CTXDMA_CREATE create;
        create.dwJmpCommand         = CAPTURE_JMP_COMMAND;
        create.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
        create.dwExtensionID        = CAPTURE_XID_CTXDMA_CREATE;
        create.dwHandle             = hDma;
        create.dwSize               = limit + 1;
        create.dwBaseAddress        = (DWORD)(base);

        if ((DWORD(base) >= pDriverData->BaseAddress)
         && (DWORD(base) < (pDriverData->BaseAddress + pDriverData->TotalVRAM)))
        {
            create.dwType = CAPTURE_CTXDMA_TYPE_VID;
        }
        else if (pDriverData->GARTLinearBase
              && (DWORD(base) >= pDriverData->GARTLinearBase)
              && (DWORD(base) <  (pDriverData->GARTLinearBase + pDriverData->regMaxAGPLimit + 1)))
        {
            create.dwType = CAPTURE_CTXDMA_TYPE_AGP;
        }
        else
        {
            create.dwType = CAPTURE_CTXDMA_TYPE_PCI;
        }
        captureLog (&create,sizeof(create));
    }
    return NvRmAllocContextDma (hClient, hDma, hClass, flags, base, limit);
}
#define NvRmAllocContextDma __capture_NvRmAllocContextDma

//////////////////////////////////////////////////////////////////////////////
// override object / context dma deletion
//
inline ULONG __capture_NvRmFree (ULONG hClient, ULONG hChannel, ULONG hObject)
{
    if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
        CAPTURE_OBJECT_FREE free;
        free.dwJmpCommand         = CAPTURE_JMP_COMMAND;
        free.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
        free.dwExtensionID        = CAPTURE_XID_OBJECT_FREE;
        free.dwHandle             = hObject;
        captureLog (&free,sizeof(free));
    }
    return NvRmFree (hClient, hChannel, hObject);
}
#define NvRmFree __capture_NvRmFree

#endif  // CAPTURE

#endif  // _DDCAPTURE_H

