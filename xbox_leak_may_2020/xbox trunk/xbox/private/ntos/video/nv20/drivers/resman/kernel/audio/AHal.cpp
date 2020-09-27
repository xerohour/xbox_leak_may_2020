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
* AHal.cpp
*
* Chip-independent functions exported by the RM kernel
*
*/

extern "C" 
{
#include <nvarm.h>
#include <aos.h>
#include <nv32.h>
#include <AHal.h>
#include <AudioHw.h>
}
#include "CHalBase.h"
#include "CObjBase.h"
#include "CHalVoiceProc.h"
#include "CHalMCP1.h"

RM_STATUS InitAudioHal(U008 uRevID, HWINFO_COMMON_FIELDS *pDev)
{
	switch(uRevID)
	{
	case REV_MCP1:
		pDev->pHalContext = (VOID *) new CHalMCP1(uRevID, pDev);
		break;

	default:
		pDev->pHalContext = NULL;
		break;
	}

	return (pDev->pHalContext)?RM_OK:RM_ERROR;
}

VOID DestroyAudioHal(HWINFO_COMMON_FIELDS *pDev)
{
	delete ((CHalBase *)pDev->pHalContext);
}

RM_STATUS AllocDevice(HWINFO_COMMON_FIELDS *pDev)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
	return pBase->Allocate();
}

RM_STATUS FreeDevice(HWINFO_COMMON_FIELDS *pDev)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
	return pBase->Free();
}

RM_STATUS DeviceAddClient(HWINFO_COMMON_FIELDS *pDev, VOID *pParam, U008 *pClientRef)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
    return pBase->AddClient(pParam, pClientRef);
}

RM_STATUS DeviceRemoveClient(HWINFO_COMMON_FIELDS *pDev, U008 uClientRef)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
    return pBase->DeleteClient(uClientRef);
}

RM_STATUS AllocObject(HWINFO_COMMON_FIELDS *pDev, VOID *pParam)
{
	APU_OBJECT	*pObjAllocParams = (APU_OBJECT *)pParam;
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
    return pBase->AllocateObject(pObjAllocParams->uObjectCode, (VOID *)pObjAllocParams->pParam);
}

RM_STATUS FreeObject(HWINFO_COMMON_FIELDS *pDev, VOID *pParam)
{
	APU_OBJECT	*pObjFreeParams = (APU_OBJECT *)pParam;
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
    return pBase->FreeObject(pObjFreeParams->uObjectCode, (VOID *)pObjFreeParams->pParam);
}

RM_STATUS AllocContextDma(HWINFO_COMMON_FIELDS *pDev, VOID *pParam)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
	return pBase->AllocateContextDma(pParam);
}

RM_STATUS FreeContextDma(HWINFO_COMMON_FIELDS *pDev, VOID *pParam)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
	return pBase->FreeContextDma(pParam);
}

RM_STATUS InterruptService(HWINFO_COMMON_FIELDS *pDev)
{
	CHalBase *pBase = (CHalBase *)pDev->pHalContext;
    if (pBase)
	    return pBase->InterruptService();

    return RM_ERROR;
}

// the following code is to make the driver compile in C++ 
void* _cdecl operator new(unsigned int n)
{
	void *p;
    aosAllocateMem((U032) n, ALIGN_DONT_CARE, &p, NULL);
    return p;
}

void _cdecl operator delete(void* p)
{
	if (p) 
		aosFreeMem(p);
}

extern "C" int _cdecl _purecall();

int _cdecl _purecall()
{
    // this better get printed!!
    aosDbgPrintString(DEBUGLEVEL_ERRORS + 5, "NVARM: _purecall - RUNTIME ERROR!  calling pure virtual function!!!\n");
	return 0;
}
