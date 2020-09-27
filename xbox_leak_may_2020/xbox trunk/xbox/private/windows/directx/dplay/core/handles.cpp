/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Handles.cpp
 *  Content:    Handle manager
 *
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  08/19/99	mjn		Created
 *	03/23/00	mjn		Revised to ensure 64-bit compliance
 *	03/24/00	mjn		Added H_Enum
 *  08/05/00    RichGr  IA64: Use %p format specifier in DPFs for 32/64-bit pointers and handles.
 *
 ***************************************************************************/



#include "dncorei.h"


#define	INC_SERIAL_COUNT(a)		if (++((a)->dwSerial) == 0)	((a)->dwSerial)++
#define	INDEX_MASK				0x000fffff
#define	SERIAL_MASK				0xfff00000
#define SERIAL_SHIFT				20
#define	GET_INDEX(h)			(h & INDEX_MASK)
#define	GET_SERIAL(h)			((h & SERIAL_MASK) >> SERIAL_SHIFT)
#define	MAKE_HANDLE(i,s)		((i & INDEX_MASK) | ((s << SERIAL_SHIFT) & SERIAL_MASK))
#define	VERIFY_HANDLE(p,h)		(((h & INDEX_MASK) < (p)->dwNumHandles) && ((p)->HandleArray[(h & INDEX_MASK)].dwSerial) && ((h & SERIAL_MASK) == ((p)->HandleArray[(h & INDEX_MASK)].dwSerial << SERIAL_SHIFT)))


#undef DPF_MODNAME
#define DPF_MODNAME "H_Grow"

HRESULT H_Grow(HANDLESTRUCT *const phs,
			   const DWORD dwIncSize)
{
	HRESULT		hResultCode = S_OK;
	DWORD		dw;

	DPF(9,"Parameters: phs [0x%p], dwIncSize [%ld]",phs,dwIncSize);

	if (dwIncSize == 0)
	{
		DPFERR("Must grow handles by at least 1");
		return(E_INVALIDARG);
	}

	// Grab CS
	DNEnterCriticalSection(&phs->dncs);

	if (phs->HandleArray == NULL || phs->dwNumHandles == 0)
	{
		if ((phs->HandleArray = static_cast<HANDLEELEMENT*>(DNMalloc((dwIncSize + phs->dwNumHandles) * sizeof(HANDLEELEMENT)))) == NULL)
		{
			DPFERR("Could not create handle array");
			hResultCode = E_OUTOFMEMORY;
			goto EXIT_H_Grow;
		}
	}
	else
	{
		if ((phs->HandleArray = static_cast<HANDLEELEMENT*>(DNRealloc(phs->HandleArray,(dwIncSize + phs->dwNumHandles) * sizeof(HANDLEELEMENT)))) == NULL)
		{
			DPFERR("Could not grow handle array");
			hResultCode = E_OUTOFMEMORY;
			goto EXIT_H_Grow;
		}
	}

	// Update Handle Structure
	phs->dwFirstFreeHandle = phs->dwNumHandles;
	phs->dwLastFreeHandle = phs->dwNumHandles + dwIncSize - 1;
	phs->dwNumFreeHandles = dwIncSize;
	phs->dwNumHandles += dwIncSize;

	// Setup free Handle Elements
	for (dw = 0 ; dw < dwIncSize - 1 ; dw++)
	{
		phs->HandleArray[phs->dwFirstFreeHandle + dw].dwSerial = 0;
		phs->HandleArray[phs->dwFirstFreeHandle + dw].Entry.dwIndex = phs->dwFirstFreeHandle + dw + 1;
	}
	phs->HandleArray[phs->dwFirstFreeHandle + dw].dwSerial = 0;
	phs->HandleArray[phs->dwFirstFreeHandle + dw].Entry.pvData = NULL;

EXIT_H_Grow:

	// Release CS
	DNLeaveCriticalSection(&phs->dncs);

	DPF(9,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "H_Initialize"

HRESULT	H_Initialize(HANDLESTRUCT *const phs,
					 const DWORD dwInitialNum)
{
	HRESULT		hResultCode = S_OK;

	DPF(9,"Parameters: phs [0x%p], dwInitialNum [%ld]",phs,dwInitialNum);

	if (dwInitialNum == 0)
	{
		DPFERR("Must initialize handles with at least 1");
		return(E_INVALIDARG);
	}

	if (!DNInitializeCriticalSection(&phs->dncs))
	{
		DPFERR("DNInitializeCriticalSection() failed");
		return(E_OUTOFMEMORY);
	}

	// Lock
	DNEnterCriticalSection(&phs->dncs);

	phs->HandleArray = NULL;
	phs->dwNumHandles = 0;
	phs->dwNumFreeHandles = 0;
	phs->dwFirstFreeHandle = 0;
	phs->dwLastFreeHandle = 0;
	phs->dwSerial = 1;

	if ((hResultCode = H_Grow(phs,dwInitialNum)) != S_OK)
	{
		DPFERR("H_Grow() failed");
		goto EXIT_H_Initialize;
	}

EXIT_H_Initialize:

	// Unlock
	DNLeaveCriticalSection(&phs->dncs);

	DPF(9,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "H_Terminate"

void H_Terminate(HANDLESTRUCT *const phs)
{
	DPF(9,"Parameters: phs [0x%p]",phs);

	if (phs->HandleArray != NULL)
		DNFree(phs->HandleArray);

	phs->dwNumHandles = 0;

	DNDeleteCriticalSection(&phs->dncs);

	DPF(9,"Returning");
}


#undef DPF_MODNAME
#define DPF_MODNAME "H_Create"

HRESULT H_Create(HANDLESTRUCT *const phs,
				 void *const pvData,
				 DWORD *const pHandle)
{
	DWORD	dwIndex;
	HRESULT	hResultCode = S_OK;

	DPF(9,"Parameters: phs [0x%p], pvData [0x%p], pHandle [0x%p]",phs,pvData,pHandle);

	if (pHandle == NULL)
	{
		DPFERR("Invalid handle pointer");
		return(E_INVALIDARG);
	}

	// Lock
	DNEnterCriticalSection(&phs->dncs);

	// If there are no free handles, double the handle array
	if (phs->dwNumFreeHandles == 0)
	{
		if ((hResultCode = H_Grow(phs,phs->dwNumHandles)) != S_OK)	// Double the size
		{
			DPFERR("H_Grow() failed");
			goto EXIT_H_Create;
		}
	}

	// Update internal handle pointers
	dwIndex = phs->dwFirstFreeHandle;
	phs->dwFirstFreeHandle = phs->HandleArray[dwIndex].Entry.dwIndex;
	phs->dwNumFreeHandles--;

	do
	{
		// Update handle information
		INC_SERIAL_COUNT(phs);
		phs->HandleArray[dwIndex].dwSerial = phs->dwSerial;
		phs->HandleArray[dwIndex].Entry.pvData = pvData;

		// Create user's handle
		*pHandle = MAKE_HANDLE(dwIndex,phs->dwSerial);
	} while (*pHandle == 0);		// Don't want 0 handle

	DPF(9,"Returning: *pHandle = [0x%lx]",*pHandle);

EXIT_H_Create:

	// Unlock
	DNLeaveCriticalSection(&phs->dncs);

	DPF(9,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "H_Destroy"

HRESULT	H_Destroy(HANDLESTRUCT *const phs,
				  const DWORD handle)
{
	DWORD	dwIndex;
	DWORD	dwSerial;
	HRESULT	hResultCode = S_OK;

	DPF(9,"Parameters: phs [0x%p], handle [0x%lx]",phs,handle);

	// Lock
	DNEnterCriticalSection(&phs->dncs);

	// Ensure valid handle supplied
	if (!VERIFY_HANDLE(phs,handle))
	{
		DPFERR("Invalid handle");
		hResultCode = E_INVALIDARG;
		goto EXIT_H_Destroy;
	}

	dwIndex = GET_INDEX(handle);
	dwSerial = GET_SERIAL(handle);

	// Add handle to end of free list
	if (phs->dwNumFreeHandles == 0)		// Only free handle
	{
		phs->dwFirstFreeHandle = phs->dwLastFreeHandle = dwIndex;
	}
	else									// Other handles, so add to end of list
	{
		phs->HandleArray[phs->dwLastFreeHandle].Entry.dwIndex = dwIndex;
		phs->dwLastFreeHandle = dwIndex;
	}

	// Clear out returned handle
	phs->HandleArray[dwIndex].dwSerial = 0;
	phs->HandleArray[dwIndex].Entry.pvData = NULL;

	// Update handle structure
	INC_SERIAL_COUNT(phs);
	phs->dwNumFreeHandles++;

EXIT_H_Destroy:

	// Unlock
	DNLeaveCriticalSection(&phs->dncs);

	DPF(9,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "H_Retrieve"

HRESULT H_Retrieve(HANDLESTRUCT *const phs,
				   const DWORD handle,
				   void **const ppvData)
{
	DWORD		dwIndex;
	DWORD		dwSerial;
	HRESULT		hResultCode = S_OK;

	DPF(9,"Parameters: phs [0x%p], handle [0x%lx], ppvData [0x%p]",phs,handle,ppvData);

	// Lock
	DNEnterCriticalSection(&phs->dncs);

	// Ensure valid handle supplied
	if (!VERIFY_HANDLE(phs,handle))
	{
//		DPFERR("Invalid handle");
		hResultCode = E_INVALIDARG;
		goto EXIT_H_Retrieve;
	}

	if (ppvData == NULL)
	{
		DPFERR("Invalid data return pointer");
		hResultCode = E_INVALIDARG;
		goto EXIT_H_Retrieve;
	}

	dwIndex = GET_INDEX(handle);
	dwSerial = GET_SERIAL(handle);

	*ppvData = phs->HandleArray[GET_INDEX(handle)].Entry.pvData;
	DPF(9,"Returning: *ppvData = [0x%p]",*ppvData);

EXIT_H_Retrieve:

	// Unlock
	DNLeaveCriticalSection(&phs->dncs);

	DPF(9,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "H_Enum"

HRESULT H_Enum(HANDLESTRUCT *const phs,
			   DWORD *const pdwNumHandles,
			   DWORD *const rgHandles)
{
	HRESULT		hResultCode;
	DWORD		dw;
	DWORD		dwNumHandles;
	DWORD		*pHandle;

	DPF(3,"Parameters: phs [0x%p], pdwNumHandles [0x%p], rgHandles [0x%p]",
			phs,pdwNumHandles,rgHandles);

	DNASSERT(phs != NULL);
	DNASSERT(pdwNumHandles != NULL);
	DNASSERT(rgHandles != NULL || *pdwNumHandles == 0);

	hResultCode = S_OK;
	dwNumHandles = 0;
	DNEnterCriticalSection(&phs->dncs);
	for (dw = 0 ; dw < phs->dwNumHandles ; dw++)
	{
		if (phs->HandleArray[dw].dwSerial != NULL)
		{
			DPF(5,"Found handle data at [%ld]",dw);
			dwNumHandles++;
		}
	}

	if (dwNumHandles)
	{
		if (dwNumHandles <= *pdwNumHandles)
		{
			pHandle = rgHandles;
			for (dw = 0 ; dw < phs->dwNumHandles ; dw++)
			{
				if (phs->HandleArray[dw].dwSerial != NULL)
				{
					*pHandle++ = MAKE_HANDLE(dw,phs->HandleArray[dw].dwSerial);
				}
			}
		}
		else
		{
			hResultCode = E_POINTER;
		}
	}
	*pdwNumHandles = dwNumHandles;

	DNLeaveCriticalSection(&phs->dncs);

	DPF(3,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}

