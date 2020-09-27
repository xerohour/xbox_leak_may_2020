/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Cancel.cpp
 *  Content:    DirectNet Cancel Operations
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  04/07/00	mjn		Created
 *	04/08/00	mjn		Added DNCancelEnum(), DNCancelSend()
 *	04/11/00	mjn		DNCancelEnum() uses CAsyncOp
 *	04/17/00	mjn		DNCancelSend() uses CAsyncOp
 *	04/25/00	mjn		Added DNCancelConnect()
 *	07/05/00	mjn		Added code to handle invalid async ops
 *	07/08/00	mjn		Fixed CAsyncOp to contain m_bilinkParent
 *	08/05/00	mjn		Added DNCancelChildren(),DNCancelActiveCommands(),DNCanCancelCommand()
 *				mjn		Removed DNCancelEnum(),DNCancelListen(),DNCancelSend(),DNCancelConnect()
 *	08/07/00	mjn		Added DNCancelRequestCommands()
 *	08/22/00	mjn		Remove cancelled receive buffers from the active list in DNDoCancelCommand()
 *	09/02/00	mjn		Cancel active commands in reverse order (to prevent out of order messages at protocol level)
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dncorei.h"


//	DNCanCancelCommand
//
//	This will determine if an operation is cancelable based on the selection flags

#undef DPF_MODNAME
#define DPF_MODNAME "DNCanCancelCommand"

BOOL DNCanCancelCommand(CAsyncOp *const pAsyncOp,
						const DWORD dwFlags)
{
	BOOL	fReturnVal;

	DPF(8,"Parameters: pAsyncOp [0x%p], dwFlags [0x%lx]",pAsyncOp,dwFlags);

	DNASSERT(pAsyncOp != NULL);

	fReturnVal = FALSE;
	switch(pAsyncOp->GetOpType())
	{
		case ASYNC_OP_CONNECT:
			{
				if (dwFlags & DN_CANCEL_FLAG_CONNECT)
				{
					fReturnVal = TRUE;
				}
				break;
			}
		case ASYNC_OP_DISCONNECT:
			{
				if (dwFlags & DN_CANCEL_FLAG_DISCONNECT)
				{
					fReturnVal = TRUE;
				}
				break;
			}
		case ASYNC_OP_ENUM_QUERY:
			{
				if (dwFlags & DN_CANCEL_FLAG_ENUM_QUERY)
				{
					fReturnVal = TRUE;
				}
				break;
			}
		case ASYNC_OP_ENUM_RESPONSE:
			{
				if (dwFlags & DN_CANCEL_FLAG_ENUM_RESPONSE)
				{
					fReturnVal = TRUE;
				}
				break;
			}
		case ASYNC_OP_LISTEN:
			{
				if (dwFlags & DN_CANCEL_FLAG_LISTEN)
				{
					fReturnVal = TRUE;
				}
				break;
			}
		case ASYNC_OP_SEND:
			{
				if (pAsyncOp->IsInternal())
				{
					if (dwFlags & DN_CANCEL_FLAG_INTERNAL_SEND)
					{
						fReturnVal = TRUE;
					}
				}
				else
				{
					if (dwFlags & DN_CANCEL_FLAG_USER_SEND)
					{
						fReturnVal = TRUE;
					}
				}
				break;
			}
		case ASYNC_OP_RECEIVE_BUFFER:
			{
				if (dwFlags & DN_CANCEL_FLAG_RECEIVE_BUFFER)
				{
					fReturnVal = TRUE;
				}
				break;
			}
		case ASYNC_OP_REQUEST:
			{
				break;
			}
		default:
			{
				break;
			}
	}
	DPF(8,"Returning: [%ld]",fReturnVal);
	return(fReturnVal);
}


//	DNDoCancelCommand
//
//	This will attempt to cancel a given operation based on its OpType

#undef DPF_MODNAME
#define DPF_MODNAME "DNDoCancelCommand"

HRESULT DNDoCancelCommand(DIRECTNETOBJECT *const pdnObject,
						  CAsyncOp *const pAsyncOp)
{
	HRESULT		hResultCode;

	DPF(8,"Parameters: pAsyncOp [0x%p]",pAsyncOp);

	hResultCode = DPNERR_CANNOTCANCEL;

	switch(pAsyncOp->GetOpType())
	{
		case ASYNC_OP_CONNECT:
		case ASYNC_OP_ENUM_QUERY:
		case ASYNC_OP_ENUM_RESPONSE:
		case ASYNC_OP_LISTEN:
		case ASYNC_OP_SEND:
			{
				DNASSERT(pdnObject->pdnProtocolData != NULL );

				if (pAsyncOp->GetProtocolHandle() != NULL)
				{
					DPF(9,"Attempting to cancel AsyncOp [0x%p]",pAsyncOp);
					hResultCode = DNPCancelCommand(pdnObject->pdnProtocolData,pAsyncOp->GetProtocolHandle());
					DPF(9,"Result of cancelling AsyncOp [0x%p] was [0x%lx]",pAsyncOp,hResultCode);
				}
				break;
			}
		case ASYNC_OP_RECEIVE_BUFFER:
			{
				if (pAsyncOp->GetHandle() != 0)
				{
					if ((hResultCode = pdnObject->HandleTable.Destroy( pAsyncOp->GetHandle() )) == DPN_OK)
					{
						//
						//	Remove from active list
						//
						DNEnterCriticalSection(&pdnObject->csActiveList);
						pAsyncOp->m_bilinkActiveList.RemoveFromList();
						DNLeaveCriticalSection(&pdnObject->csActiveList);
					}
					else
					{
						hResultCode = DPNERR_CANNOTCANCEL;
					}
				}
				break;
			}
//		case ASYNC_OP_DISCONNECT:
		case ASYNC_OP_REQUEST:
		default:
			{
				DNASSERT(FALSE);
				break;
			}
	}

	DPF(8,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


//	DNCancelChildren
//
//	This will mark an operation as CANCELLED to prevent new children from attaching,
//	build a cancel list of any children, and recursively call itself to cancel those children.
//	At the bottom level, if there is a Protocol handle, we will actually call DNPCancelCommand() 

#undef DPF_MODNAME
#define DPF_MODNAME "DNCancelChildren"

HRESULT DNCancelChildren(DIRECTNETOBJECT *const pdnObject,
						 CAsyncOp *const pParent)
{
	HRESULT		hResultCode;
	CBilink		*pBilink;
	CAsyncOp	*pAsyncOp;
	CAsyncOp	**CancelList;
	DWORD		dwCount;
	DWORD		dwActual;

	DPF(6,"Parameters: pParent [0x%p]",pParent);

	DNASSERT(pdnObject != NULL);
	DNASSERT(pParent != NULL);

	pAsyncOp = NULL;
	CancelList = NULL;

	//
	//	Mark the parent as cancelled so that no new children can attach
	//
	pParent->Lock();
	if (pParent->IsCancelled() || pParent->IsComplete())
	{
		pParent->Unlock();
		hResultCode = DPN_OK;
		goto Exit;
	}
	pParent->SetCancelled();

	//
	//	Determine size of cancel list
	//
	dwCount = 0;
	pBilink = pParent->m_bilinkParent.GetNext();
	while (pBilink != &pParent->m_bilinkParent)
	{
		pAsyncOp = CONTAINING_OBJECT(pBilink,CAsyncOp,m_bilinkChildren);
		pAsyncOp->Lock();
		if (!pAsyncOp->IsCancelled() && !pAsyncOp->IsComplete())
		{
			dwCount++;
		}
		pAsyncOp->Unlock();
		pBilink = pBilink->GetNext();
	}
	DPF(7,"Number of cancellable children [%ld]",dwCount);

	//
	//	Create cancel list
	//
	if (dwCount > 0)
	{
		if ((CancelList = static_cast<CAsyncOp**>(DNMalloc(dwCount * sizeof(CAsyncOp*)))) == NULL)
		{
#pragma BUGBUG( minara, "Should we unflag the cancel ?" )
			pParent->Unlock();
			DPFERR("Could not allocate space for cancel list");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		dwActual = 0;
		pBilink = pParent->m_bilinkParent.GetNext();
		while (pBilink != &pParent->m_bilinkParent)
		{
			pAsyncOp = CONTAINING_OBJECT(pBilink,CAsyncOp,m_bilinkChildren);
			pAsyncOp->Lock();
			if (!pAsyncOp->IsCancelled() && !pAsyncOp->IsComplete())
			{
				DNASSERT(dwActual < dwCount);	// This list should NEVER grow !
				pAsyncOp->AddRef();
				CancelList[dwActual] = pAsyncOp;
				dwActual++;
			}
			pAsyncOp->Unlock();
			pBilink = pBilink->GetNext();
		}
		DPF(7,"Actual number of cancellable children [%ld]",dwActual);
	}
	pParent->Unlock();

	//
	//	Preset the return
	//
	hResultCode = DPN_OK;

	//
	//	Call ourselves with each of the children (if there are any)
	//	and clean up (release AsyncOp children and free list)
	//
	if (CancelList)
	{
		DWORD	dw;
		HRESULT	hr;

		for (dw = 0 ; dw < dwActual ; dw++ )
		{
			hr = DNCancelChildren(pdnObject,CancelList[dw]);
			if ((hr != DPN_OK) && (hResultCode == DPN_OK))
			{
				hResultCode = hr;
			}
			CancelList[dw]->Release();
			CancelList[dw] = NULL;
		}
		DNFree(CancelList);
		CancelList = NULL;
	}

	//
	//	Cancel this operation (if we can)
	//	This will only work for CONNECTs,DISCONNECTs,ENUM_QUERYs,ENUM_RESPONSEs,LISTENs,SENDs with a protocol handle
	//
	if (pParent->GetProtocolHandle() != NULL)
	{
		HRESULT	hr;

		hr = DNDoCancelCommand(pdnObject,pParent);
		if ((hr != DPN_OK) && (hResultCode == DPN_OK))
		{
			hResultCode = hr;
		}
	}

Exit:
	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (CancelList)
	{
		DNFree(CancelList);
		CancelList = NULL;
	}
	goto Exit;
}


//	DNCancelActiveCommands
//
//	This will attempt to cancel ALL operations in the active list.

#undef DPF_MODNAME
#define DPF_MODNAME "DNCancelActiveCommands"

HRESULT DNCancelActiveCommands(DIRECTNETOBJECT *const pdnObject,
							   const DWORD dwFlags)
{
	HRESULT		hResultCode;
	CAsyncOp	*pAsyncOp;
	CAsyncOp	**CancelList;
	CBilink		*pBilink;
	DWORD		dwCount;
	DWORD		dwActual;

	DPF(6,"Parameters: (none) ");

	DNASSERT(pdnObject != NULL);

	CancelList = NULL;

	//
	//	Prevent changes
	//
	DNEnterCriticalSection(&pdnObject->csActiveList);

	//
	//	Determine size of cancel list
	//
	dwCount = 0;
	pBilink = pdnObject->m_bilinkActiveList.GetPrev();
	while (pBilink != &pdnObject->m_bilinkActiveList)
	{
		pAsyncOp = CONTAINING_OBJECT(pBilink,CAsyncOp,m_bilinkActiveList);
		if (DNCanCancelCommand(pAsyncOp,dwFlags))
		{
			pAsyncOp->Lock();
			if (!pAsyncOp->IsCancelled() && !pAsyncOp->IsComplete())
			{
				dwCount++;
			}
			pAsyncOp->Unlock();
		}
		pBilink = pBilink->GetPrev();
	}
	DPF(7,"Number of cancellable ops [%ld]",dwCount);

	//
	//	Create cancel list
	//
	if (dwCount > 0)
	{
		if ((CancelList = static_cast<CAsyncOp**>(DNMalloc(dwCount * sizeof(CAsyncOp*)))) == NULL)
		{
			DNLeaveCriticalSection(&pdnObject->csActiveList);
			DPFERR("Could not allocate space for cancel list");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		dwActual = 0;
		pBilink = pdnObject->m_bilinkActiveList.GetPrev();
		while (pBilink != &pdnObject->m_bilinkActiveList)
		{
			pAsyncOp = CONTAINING_OBJECT(pBilink,CAsyncOp,m_bilinkActiveList);
			if (DNCanCancelCommand(pAsyncOp,dwFlags))
			{
				pAsyncOp->Lock();
				if (!pAsyncOp->IsCancelled() && !pAsyncOp->IsComplete())
				{
					DNASSERT(dwActual < dwCount);	// This list should NEVER grow !
					pAsyncOp->AddRef();
					CancelList[dwActual] = pAsyncOp;
					dwActual++;
				}
				pAsyncOp->Unlock();
			}
			pBilink = pBilink->GetPrev();
		}
		DPF(7,"Actual number of cancellable ops [%ld]",dwActual);
	}

	//
	//	Allow changes, though the list should not grow any more here
	//
	DNLeaveCriticalSection(&pdnObject->csActiveList);

	//
	//	Preset the return
	//
	hResultCode = DPN_OK;

	//
	//	Cancel each operation in the cancel list operation (if we can)
	//	This will only work for CONNECTs,DISCONNECTs,ENUM_QUERYs,ENUM_RESPONSEs,LISTENs,SENDs with a protocol handle
	//
	if (CancelList)
	{
		DWORD	dw;
		HRESULT	hr;

		for (dw = 0 ; dw < dwActual ; dw++ )
		{
			//
			//	Ensure operation has not already been cancelled
			//
			DNASSERT( CancelList[dw] != NULL );
			CancelList[dw]->Lock();
			if (CancelList[dw]->IsCancelled() || CancelList[dw]->IsComplete())
			{
				CancelList[dw]->Unlock();
				CancelList[dw]->Release();
				CancelList[dw] = NULL;
				continue;
			}
			CancelList[dw]->SetCancelled();
			CancelList[dw]->Unlock();

			//
			//	Perform the actual cancel
			//
			hr = DNDoCancelCommand(pdnObject,CancelList[dw]);
			if ((hr != DPN_OK) && (hResultCode == DPN_OK))
			{
				hResultCode = hr;
			}
			CancelList[dw]->Release();
			CancelList[dw] = NULL;
		}
		DNFree(CancelList);
		CancelList = NULL;
	}

Exit:
	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (CancelList)
	{
		DNFree(CancelList);
		CancelList = NULL;
	}
	goto Exit;
}


//	DNCancelRequestCommands
//
//	This will attempt to cancel REQUEST operations in the HandleTable.
//	Requests have handles which are matched up against responses.  Since these
//	typically have SEND children (which may have completed and thus vanished),
//	there is no guarantee these are not orphaned off in the HandleTable.
//	We will look through the HandleTable for them and cancel them.

#undef DPF_MODNAME
#define DPF_MODNAME "DNCancelRequestCommands"

HRESULT DNCancelRequestCommands(DIRECTNETOBJECT *const pdnObject)
{
	HRESULT		hResultCode;
	CAsyncOp	*pAsyncOp;
	DPNHANDLE	*HandleList;
	DWORD		dwCount;
	DWORD		dw;

	DPF(6,"Parameters: (none)");

	DNASSERT(pdnObject != NULL);

	pAsyncOp = NULL;
	HandleList = NULL;

	//
	//	Determine size of handle list
	//
	dwCount = 0;
	if ((hResultCode = pdnObject->HandleTable.Enum(NULL,&dwCount)) != DPNERR_BUFFERTOOSMALL)
	{
		DPFERR("Could not find any entries in the HandleTable");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}
	DPF(7,"Number of handles [%ld]",dwCount);

	//
	//	Create handle list
	//
	if ((HandleList = static_cast<DPNHANDLE*>(DNMalloc(dwCount * sizeof(DPNHANDLE)))) == NULL)
	{
		DPFERR("Could not allocate HandleList");
		hResultCode = DPNERR_OUTOFMEMORY;
		goto Failure;
	}
	if ((hResultCode = pdnObject->HandleTable.Enum(HandleList,&dwCount)) != DPN_OK)
	{
		DPFERR("Could not enum handles");
		DisplayDNError(0,hResultCode);
		goto Failure;
	}

	//
	//	Remove non-REQUEST operations as these might vanish later
	//
	for ( dw = 0 ; dw < dwCount ; dw++ )
	{
		if ((hResultCode = pdnObject->HandleTable.Find(HandleList[dw],&pAsyncOp)) != DPN_OK)
		{
			HandleList[dw] = 0;
			continue;
		}
		if (pAsyncOp->GetOpType() != ASYNC_OP_REQUEST)
		{
			HandleList[dw] = 0;
		}
		pAsyncOp->Release();
		pAsyncOp = NULL;
	}

	//
	//	Cancel REQUEST operations and remove from HandleTable
	//
	for ( dw = 0 ; dw < dwCount ; dw++ )
	{
		if (HandleList[dw] == 0)
		{
			continue;
		}
		if ((hResultCode = pdnObject->HandleTable.Find(HandleList[dw],&pAsyncOp)) != DPN_OK)
		{
			continue;
		}
		if (pAsyncOp->GetOpType() == ASYNC_OP_REQUEST)
		{
			pAsyncOp->Lock();
			pAsyncOp->SetResult( DPNERR_USERCANCEL );
			pAsyncOp->Unlock();
			pdnObject->HandleTable.Destroy( HandleList[dw] );
			pAsyncOp->Release();
			pAsyncOp = NULL;
		}
	}

	DNFree(HandleList);
	HandleList = NULL;

	hResultCode = DPN_OK;

Exit:
	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (HandleList != NULL)
	{
		DNFree(HandleList);
		HandleList = NULL;
	}
	goto Exit;
}
