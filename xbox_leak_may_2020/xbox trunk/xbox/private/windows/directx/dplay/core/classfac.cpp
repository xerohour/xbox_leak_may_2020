/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ClassFac.cpp
 *  Content:    DNET COM class factory
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/21/99	mjn		Created
 *	12/23/99	mjn		Fixed Host and AllPlayers short-cut pointer use
 *	12/28/99	mjn		Moved Async Op stuff to Async.h
 *	01/06/00	mjn		Moved NameTable stuff to NameTable.h
 *	01/08/00	mjn		Fixed DN_APPLICATION_DESC in DIRECTNETOBJECT
 *	01/13/00	mjn		Added CFixedPools and CRefCountBuffers
 *	01/14/00	mjn		Removed pvUserContext from DN_NAMETABLE_ENTRY
 *	01/15/00	mjn		Replaced DN_COUNT_BUFFER with CRefCountBuffer
 *	01/16/00	mjn		Removed User message fixed pool
 *  01/18/00	mjn		Fixed bug in ref count.
 *	01/19/00	mjn		Replaced DN_SYNC_EVENT with CSyncEvent
 *	01/19/00	mjn		Initialize structures for NameTable Operation List
 *	01/25/00	mjn		Added NameTable pending operation list
 *	01/31/00	mjn		Added Internal FPM's for RefCountBuffers
 *  03/17/00    rmt     Added calls to init/free SP Caps cache
 *	03/23/00	mjn		Implemented RegisterLobby()
 *  04/04/00	rmt		Enabled "Enable Parameter Validation" flag on object by default
 *	04/09/00	mjn		Added support for CAsyncOp
 *	04/11/00	mjn		Added DIRECTNETOBJECT bilink for CAsyncOps
 *	04/26/00	mjn		Removed DN_ASYNC_OP and related functions
 *	04/28/00	mjn		Code cleanup - removed hsAsyncHandles,blAsyncOperations
 *	05/04/00	mjn		Cleaned up and made multi-thread safe
 *  05/23/00    RichGr  IA64: Substituted %p format specifier whereever
 *                      %x was being used to format pointers.  %p is 32-bit
 *                      in a 32-bit build, and 64-bit in a 64-bit build.
 *  06/09/00    rmt     Updates to split CLSID and allow whistler compat
 *  06/09/00    rmt     Updates to split CLSID and allow whistler compat and support external create funcs
 *	06/20/00	mjn		Fixed QueryInterface bug
 *  06/27/00	rmt		Fixed bug which was causing interfaces to always be created as peer interfaces
 *  07/05/00	rmt		Bug #38478 - Could QI for peer interfaces from client object
 *						(All interfaces could be queried from all types of objects).
 *				mjn		Initialize pConnect element of DIRECNETOBJECT to NULL
 *	07/07/00	mjn		Added pNewHost for DirectNetObject
 *	07/08/00	mjn		Call DN_Close when object is about to be free'd
 *  07/09/00	rmt		Added code to free interface set by RegisterLobby (if there is one)
 *	07/17/00	mjn		Add signature to DirectNetObject
 *  07/21/00    RichGr  IA64: Use %p format specifier for 32/64-bit pointers.
 *	07/26/00	mjn		DN_QueryInterface returns E_POINTER if NULL destination pointer specified
 *	07/28/00	mjn		Added m_bilinkConnections to DirectNetObject
 *	07/30/00	mjn		Added CPendingDeletion
 *	07/31/00	mjn		Added CQueuedMsg
 *	08/05/00	mjn		Added m_bilinkActiveList and csActiveList
 *	08/06/00	mjn		Added CWorkerJob
 *	08/23/00	mjn		Added CNameTableOp
 *	09/04/00	mjn		Added CApplicationDesc
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dncorei.h"

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

typedef	STDMETHODIMP IUnknownQueryInterface( IUnknown *pInterface, REFIID riid, LPVOID *ppvObj );
typedef	STDMETHODIMP_(ULONG)	IUnknownAddRef( IUnknown *pInterface );
typedef	STDMETHODIMP_(ULONG)	IUnknownRelease( IUnknown *pInterface );

//
// VTable for IUnknown interface
//
IUnknownVtbl  DN_UnknownVtbl =
{
	(IUnknownQueryInterface*)	DN_QueryInterface,
	(IUnknownAddRef*)			DN_AddRef,
	(IUnknownRelease*)			DN_Release
};


//**********************************************************************
// Variable definitions
//**********************************************************************

//
// Globals
//
extern	DWORD	GdwHLocks;
extern	DWORD	GdwHObjects;


//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************

#undef DPF_MODNAME
#define DPF_MODNAME "DN_CreateInterface"

static	HRESULT DN_CreateInterface(OBJECT_DATA *pObject,
								   REFIID riid,
								   INTERFACE_LIST **const ppv)
{
	INTERFACE_LIST	*pIntNew;
	PVOID			lpVtbl;
	HRESULT			hResultCode;

    // 5/23/2000(RichGr): IA64: Use %p format specifier for 32/64-bit pointers.
	DPF(6,"Parameters: pObject [%p], ppv [%p]",pObject,ppv);

	DNASSERT(pObject != NULL);
	DNASSERT(ppv != NULL);

    DIRECTNETOBJECT *pdnObject = ((DIRECTNETOBJECT *)pObject->pvData);

		DPF(7,"riid = IID_IDirectPlay8Protocol");
		lpVtbl = &DN_ProtocolVtbl;

	if ((pIntNew = static_cast<INTERFACE_LIST*>(DNMalloc(sizeof(INTERFACE_LIST)))) == NULL)
	{
		DPFERR("Could not allocate interface");
		hResultCode = E_OUTOFMEMORY;
		goto Exit;
	}
	pIntNew->lpVtbl = lpVtbl;
	pIntNew->lRefCount = 0;
	pIntNew->pIntNext = NULL;
	pIntNew->pObject = pObject;

	*ppv = pIntNew;
    // 5/23/2000(RichGr): IA64: Use %p format specifier for 32/64-bit pointers.
	DPF(7,"*ppv = [0x%p]",*ppv);

	hResultCode = S_OK;

Exit:
    DPF(6,"Returning: hResultCode = [%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_FindInterface"

INTERFACE_LIST *DN_FindInterface(void *pInterface,
								 REFIID riid)
{
	INTERFACE_LIST	*pIntList;

    // 5/23/2000(RichGr): IA64: Use %p format specifier for 32/64-bit pointers.
	DPF(6,"Parameters: pInterface [%p]",pInterface);

	DNASSERT(pInterface != NULL);

	pIntList = (static_cast<INTERFACE_LIST*>(pInterface))->pObject->pIntList;	// Find first interface

	while (pIntList != NULL)
	{
			break;
	}

    // 5/23/2000(RichGr): IA64: Use %p format specifier for 32/64-bit pointers.
	DPF(6,"Returning: pIntList [0x%p]",pIntList);
	return(pIntList);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_QueryInterface"

STDMETHODIMP DN_QueryInterface(void *pInterface,
							   REFIID riid,
							   void **ppv)
{
	INTERFACE_LIST	*pIntList;
	INTERFACE_LIST	*pIntNew;
	HRESULT			hResultCode;

    // 5/23/2000(RichGr): IA64: Use %p format specifier for 32/64-bit pointers.
	DPF(2,"Parameters: pInterface [0x%p], ppv [0x%p]",pInterface,ppv);

	if (pInterface == NULL)
	{
		DPFERR("Invalid COM interface specified");
		hResultCode = E_INVALIDARG;
		goto Exit;
	}
	if (ppv == NULL)
	{
		DPFERR("Invalid target interface pointer specified");
		hResultCode = E_POINTER;
		goto Exit;
	}

	if ((pIntList = DN_FindInterface(pInterface,riid)) == NULL)
	{	// Interface must be created
		pIntList = (static_cast<INTERFACE_LIST*>(pInterface))->pObject->pIntList;
		if ((hResultCode = DN_CreateInterface(pIntList->pObject,riid,&pIntNew)) != S_OK)
		{
			goto Exit;
		}
		pIntNew->pIntNext = pIntList;
		pIntList->pObject->pIntList = pIntNew;
		pIntList = pIntNew;
	}
	if (pIntList->lRefCount == 0)		// New interface exposed
	{
		InterlockedIncrement( &pIntList->pObject->lRefCount );
	}
	InterlockedIncrement( &pIntList->lRefCount );
	*ppv = static_cast<void*>(pIntList);
    // 5/23/2000(RichGr): IA64: Use %p format specifier for 32/64-bit pointers.
	DPF(5,"*ppv = [0x%p]", *ppv);

	hResultCode = S_OK;

Exit:
	DPF(2,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_AddRef"

STDMETHODIMP_(ULONG) DN_AddRef(void *pInterface)
{
	INTERFACE_LIST	*pIntList;
	LONG			lRefCount;

    // 5/23/2000(RichGr): IA64: Use %p format specifier for 32/64-bit pointers.
	DPF(2,"Parameters: pInterface [0x%p]",pInterface);

	if (pInterface == NULL)
	{
		DPFERR("Invalid COM interface specified");
		lRefCount = 0;
		goto Exit;
	}

	pIntList = static_cast<INTERFACE_LIST*>(pInterface);
	lRefCount = InterlockedIncrement( &pIntList->lRefCount );
	DPF(5,"New lRefCount [%ld]",lRefCount);

Exit:
	DPF(2,"Returning: lRefCount [%ld]",lRefCount);
	return(lRefCount);
}


#undef DPF_MODNAME
#define DPF_MODNAME "DN_Release"

STDMETHODIMP_(ULONG) DN_Release(void *pInterface)
{
	INTERFACE_LIST	*pIntList;
	INTERFACE_LIST	*pIntCurrent;
	LONG			lRefCount;
	LONG			lObjRefCount;

    // 5/23/2000(RichGr): IA64: Use %p format specifier for 32/64-bit pointers.
	DPF(2,"Parameters: pInterface [%p]",pInterface);

	if (pInterface == NULL)
	{
		DPFERR("Invalid COM interface specified");
		lRefCount = 0;
		goto Exit;
	}

	pIntList = static_cast<INTERFACE_LIST*>(pInterface);
	lRefCount = InterlockedDecrement( &pIntList->lRefCount );
	DPF(5,"New lRefCount [%ld]",lRefCount);

	if (lRefCount == 0)
	{
		//
		//	Decrease object's interface count
		//
		lObjRefCount = InterlockedDecrement( &pIntList->pObject->lRefCount );

		//
		//	Free object and interfaces
		//
		if (lObjRefCount == 0)
		{
			pIntList = pIntList->pObject->pIntList;	// Get head of interface list
			DNFree(pIntList->pObject);

			// Free Interfaces
			DPF(5,"Free interfaces");
			while(pIntList != NULL)
			{
				pIntCurrent = pIntList;
				pIntList = pIntList->pIntNext;
				DNFree(pIntCurrent);
			}

			GdwHObjects--;
		}
	}

Exit:
	DPF(2,"Returning: lRefCount [%ld]",lRefCount);
	return(lRefCount);
}

