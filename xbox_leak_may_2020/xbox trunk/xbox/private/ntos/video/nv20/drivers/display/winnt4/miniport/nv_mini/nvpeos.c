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

/*
==============================================================================

    NVIDIA Corporation, (C) Copyright 
    1993-1999. All rights reserved.

    nvpeos.c

    VPE kernel wrappers to OS or RM dependent services

    Dec 17, 1999

==============================================================================
*/
#ifdef NVPE

#ifdef _WIN32_WINNT
#include "miniport.h"
#include "ntddvdeo.h"
#include "video.h"
#endif

#include "nvos.h"
#include "nvpeos.h"

#ifdef DBG
U032 DbgPrint(U008 *pchFormat, ...);
#endif // DBG

U032 osNVPAllocDmaBuffer(
	void *pContext, 
	U032 dwSize, 
	void **pBuffer)
{
	return (U032)VideoPortAllocateBuffer(pContext, dwSize, pBuffer);
}

void osNVPReleaseDmaBuffer(
	void *pContext, 
	void *pBuffer)
{
	VideoPortReleaseBuffer(pContext, pBuffer);
}

void osNVPClearEvent(void *hEvent)
{
	KeClearEvent(hEvent);
}

U032 osNVPSetEvent(void *hEvent)
{
	return KeSetEvent(hEvent, NULL, FALSE);
}

U032 rmNVPAllocContextDma(
    U032 hClient,
    U032 hDma,
    U032 hClass,
    U032 flags,
    U032 selector,
    U032 base,
    U032 limit)
{
	return RmAllocContextDma(hClient, hDma, hClass, flags, selector, base, limit);
}

U032 rmNVPAllocChannelDma(
	U032 hClient,
	U032 hDevice,
	U032 hDmaChannel,
	U032 hClass,
	U032 hErrorContext,
	U032 hDataContext,
	U032 offset,
	U032 *ppControl)
{
	return RmAllocChannelDma(hClient, hDevice, hDmaChannel, hClass, hErrorContext, hDataContext, offset, ppControl);
}

U032 rmNVPAllocObject(
	U032 hClient,
	U032 hChannel,
	U032 hObject,
	U032 hClass)
{
	return RmAllocObject(hClient, hChannel, hObject, hClass);

}

U032 rmNVPAllocEvent(
	NVOS10_PARAMETERS *pEvent)
{
	Nv01AllocEvent(pEvent);
	return 0;
}

U032 rmNVPAlloc(
    U032 hClient,
    U032 hChannel,
    U032 hObject,
    U032 hClass,
    PVOID pAllocParms)
{

    return RmAlloc(hClient, hChannel, hObject, hClass, pAllocParms);

}

U032 rmNVPFree(
	U032 hClient,
	U032 hParent,
	U032 hObject)
{
	return RmFree(hClient, hParent, hObject);
}

#ifdef DBG

VOID osDbgPrintString
(
    int debuglevel,
    char* string
)
{
    if (debuglevel >= cur_debuglevel)
    {
        // print formatted string, for checked build
        DbgPrint(string);
    }
}

VOID osDbgPrintStringValue
(
    int debuglevel,
    char *string,
    int value
)
{
    if (debuglevel >= cur_debuglevel)
    {
        // print string and hex value, for checked build
        DbgPrint("%s 0x%x\n", string, value);

    }
}

#endif // DBG

#endif // NVPE