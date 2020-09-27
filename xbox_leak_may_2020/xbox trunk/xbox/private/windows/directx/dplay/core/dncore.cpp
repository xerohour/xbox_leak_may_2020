/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dncore.cpp
 *  Content:    
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *  ====       ==      ======
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dncorei.h"

extern DWORD g_dwGlobalObjectCount;
extern DIRECTNETOBJECT *g_DPlayInterfaces[16];
extern DNCRITICAL_SECTION g_csDPlayInterfaces;

#undef DPF_MODNAME
#define DPF_MODNAME "DIRECTNETOBJECT::AddRef"
ULONG DIRECTNETOBJECT::AddRef(void)
{
    ULONG                   ulRefCount;

    DNASSERT(m_ulRefCount);

    ulRefCount = ++m_ulRefCount;

    return ulRefCount;
}

#undef DPF_MODNAME
#define DPF_MODNAME "DIRECTNETOBJECT::Release"
ULONG DIRECTNETOBJECT::Release(void)
{
    ULONG                   ulRefCount;

    DNASSERT(m_ulRefCount);
    
    if(!(ulRefCount = --m_ulRefCount))
    {
        delete this;
    }

    return ulRefCount;
}

#undef DPF_MODNAME
#define DPF_MODNAME "DIRECTNETOBJECT::DIRECTNETOBJECT"
DIRECTNETOBJECT::DIRECTNETOBJECT( DWORD dwDirectPlayType )
{
	DIRECTNETOBJECT *pdnObject = this;

	//
	//	Signature
	//
	pdnObject->Sig[0] = 'D';
	pdnObject->Sig[1] = 'N';
	pdnObject->Sig[2] = 'E';
	pdnObject->Sig[3] = 'T';

	//
	//	Set allocatable elements to NULL to simplify free'ing later on
	//
	pdnObject->dwLockCount = 0;
	pdnObject->hLockEvent = NULL;
	pdnObject->pdnProtocolData = NULL;
#ifndef DPLAY_DOWORK
	pdnObject->hWorkerEvent = NULL;
	pdnObject->hWorkerThread = NULL;
#endif
	pdnObject->pListenParent = NULL;
	pdnObject->pConnectParent = NULL;
	pdnObject->pvConnectData = NULL;
	pdnObject->dwConnectDataSize = 0;
	pdnObject->pIDP8ADevice = NULL;
	pdnObject->pIDP8AEnum = NULL;
	pdnObject->m_pFPOOLAsyncOp = NULL;
	pdnObject->m_pFPOOLConnection = NULL;
	pdnObject->m_pFPOOLGroupConnection = NULL;
	pdnObject->m_pFPOOLGroupMember = NULL;
	pdnObject->m_pFPOOLNameTableEntry = NULL;
	pdnObject->m_pFPOOLNameTableOp = NULL;
	pdnObject->m_pFPOOLPendingDeletion = NULL;
	pdnObject->m_pFPOOLQueuedMsg = NULL;
	pdnObject->m_pFPOOLRefCountBuffer = NULL;
	pdnObject->m_pFPOOLSyncEvent = NULL;
	pdnObject->m_pFPOOLWorkerJob = NULL;
	pdnObject->m_pFPOOLMemoryBlockTiny = NULL;
	pdnObject->m_pFPOOLMemoryBlockSmall = NULL;
	pdnObject->m_pFPOOLMemoryBlockMedium = NULL;
	pdnObject->m_pFPOOLMemoryBlockLarge = NULL;
	pdnObject->m_pFPOOLMemoryBlockHuge = NULL;
	pdnObject->pNewHost = NULL;
	pdnObject->pConnectAddress = NULL;
	pdnObject->nTargets = 0;
	pdnObject->nTargetListLen = 0;
	pdnObject->pTargetList = NULL;
	pdnObject->nExpandedTargets = 0;       
	pdnObject->nExpandedTargetListLen = 0;
	pdnObject->pExpandedTargetList = NULL;
	pdnObject->m_ulRefCount = 1;
	pdnObject->dwFlags = dwDirectPlayType;
	
#ifdef ENABLE_DPLAY_VOICE
	// Voice Additions
    pdnObject->lpDxVoiceNotifyClient = NULL;	
    pdnObject->lpDxVoiceNotifyServer = NULL;
	pdnObject->pTargetList = NULL;
	pdnObject->pExpandedTargetList = NULL;
#endif
}


#undef DPF_MODNAME
#define DPF_MODNAME "DIRECTNETOBJECT::ObjectInit"
HRESULT DIRECTNETOBJECT::ObjectInit( void )
{
	DIRECTNETOBJECT *pdnObject = this;

	// Initialize Critical Section
	if (!DNInitializeCriticalSection(&(pdnObject->csDirectNetObject)))
	{
		DPFERR("DNInitializeCriticalSection() failed");
		return(E_OUTOFMEMORY);
	}

	if (!DNInitializeCriticalSection(&(pdnObject->csServiceProviders)))
	{
		DPFERR("DNInitializeCriticalSection() failed");
		return(E_OUTOFMEMORY);
	}

	if (!DNInitializeCriticalSection(&(pdnObject->csNameTableOpList)))
	{
		DPFERR("DNInitializeCriticalSection() failed");
		return(E_OUTOFMEMORY);
	}

	if (!DNInitializeCriticalSection(&(pdnObject->csAsyncOperations)))
	{
		DPFERR("DNInitializeCriticalSection() failed");
		return(E_OUTOFMEMORY);
	}

#ifdef ENABLE_DPLAY_VOICE
	if (!DNInitializeCriticalSection(&(pdnObject->csVoice)))
	{
		DPFERR("DNInitializeCriticalSection() failed");
		return(E_OUTOFMEMORY);
	}
#endif

	if (!DNInitializeCriticalSection(&(pdnObject->csWorkerQueue)))
	{
		DPFERR("DNInitializeCriticalSection(worker queue) failed");
		return(E_OUTOFMEMORY);
	}

	if (!DNInitializeCriticalSection(&(pdnObject->csActiveList)))
	{
		DPFERR("DNInitializeCriticalSection(csActiveList) failed");
		return(E_OUTOFMEMORY);
	}

	if (!DNInitializeCriticalSection(&(pdnObject->csConnectionList)))
	{
		DPFERR("DNInitializeCriticalSection(csConnectionList) failed");
		return(E_OUTOFMEMORY);
	}

    DN_InitSPCapsList( pdnObject );

	// Initialize Fixed Pool for AsyncOps
	pdnObject->m_pFPOOLAsyncOp = new CLockedContextClassFixedPool< CAsyncOp >;
	DNASSERT(pdnObject->m_pFPOOLAsyncOp != NULL);
	pdnObject->m_pFPOOLAsyncOp->Initialize(	CAsyncOp::FPMAlloc,
											CAsyncOp::FPMInitialize,
											CAsyncOp::FPMRelease,
											CAsyncOp::FPMDealloc );

	// Initialize Fixed Pool for RefCountBuffers
	pdnObject->m_pFPOOLRefCountBuffer = new CLockedContextClassFixedPool< CRefCountBuffer >;
	DNASSERT(pdnObject->m_pFPOOLRefCountBuffer != NULL);
	pdnObject->m_pFPOOLRefCountBuffer->Initialize(	CRefCountBuffer::FPMAlloc,
													CRefCountBuffer::FPMInitialize,
													CRefCountBuffer::FPMRelease,
													CRefCountBuffer::FPMDealloc );

	// Initialize Fixed Pool for SyncEvents
	pdnObject->m_pFPOOLSyncEvent = new CLockedContextClassFixedPool< CSyncEvent >;
	DNASSERT(pdnObject->m_pFPOOLSyncEvent != NULL);
	pdnObject->m_pFPOOLSyncEvent->Initialize(	CSyncEvent::FPMAlloc,
												CSyncEvent::FPMInitialize,
												CSyncEvent::FPMRelease,
												CSyncEvent::FPMDealloc );

	// Initialize Fixed Pool for Connections
	pdnObject->m_pFPOOLConnection = new CLockedContextClassFixedPool< CConnection >;
	DNASSERT(pdnObject->m_pFPOOLConnection != NULL);
	pdnObject->m_pFPOOLConnection->Initialize(	CConnection::FPMAlloc,
												CConnection::FPMInitialize,
												CConnection::FPMRelease,
												CConnection::FPMDealloc );

	// Initialize Fixed Pool for Group Connections
	pdnObject->m_pFPOOLGroupConnection = new CLockedContextClassFixedPool< CGroupConnection >;
	DNASSERT(pdnObject->m_pFPOOLGroupConnection != NULL);
	pdnObject->m_pFPOOLGroupConnection->Initialize(	CGroupConnection::FPMAlloc,
													CGroupConnection::FPMInitialize,
													CGroupConnection::FPMRelease,
													CGroupConnection::FPMDealloc );

	// Initialize Fixed Pool for Group Members
	pdnObject->m_pFPOOLGroupMember = new CLockedContextClassFixedPool< CGroupMember >;
	DNASSERT(pdnObject->m_pFPOOLGroupMember != NULL);
	pdnObject->m_pFPOOLGroupMember->Initialize(	CGroupMember::FPMAlloc,
												CGroupMember::FPMInitialize,
												CGroupMember::FPMRelease,
												CGroupMember::FPMDealloc );

	// Initialize Fixed Pool for NameTable Entries
	pdnObject->m_pFPOOLNameTableEntry = new CLockedContextClassFixedPool< CNameTableEntry >;
	DNASSERT(pdnObject->m_pFPOOLNameTableEntry != NULL);
	pdnObject->m_pFPOOLNameTableEntry->Initialize(	CNameTableEntry::FPMAlloc,
													CNameTableEntry::FPMInitialize,
													CNameTableEntry::FPMRelease,
													CNameTableEntry::FPMDealloc );

	// Initialize Fixed Pool for NameTable Ops
	pdnObject->m_pFPOOLNameTableOp = new CLockedContextClassFixedPool< CNameTableOp >;
	DNASSERT(pdnObject->m_pFPOOLNameTableOp != NULL);
	pdnObject->m_pFPOOLNameTableOp->Initialize(	CNameTableOp::FPMAlloc,
												CNameTableOp::FPMInitialize,
												CNameTableOp::FPMRelease,
												CNameTableOp::FPMDealloc );

	// Initialize Fixed Pool for NameTable Pending Deletions
	pdnObject->m_pFPOOLPendingDeletion = new CLockedContextClassFixedPool< CPendingDeletion >;
	DNASSERT(pdnObject->m_pFPOOLPendingDeletion != NULL);
	pdnObject->m_pFPOOLPendingDeletion->Initialize(	CPendingDeletion::FPMAlloc,
													CPendingDeletion::FPMInitialize,
													CPendingDeletion::FPMRelease,
													CPendingDeletion::FPMDealloc );

	// Initialize Fixed Pool for Queued Messages
	pdnObject->m_pFPOOLQueuedMsg = new CLockedContextClassFixedPool< CQueuedMsg >;
	DNASSERT(pdnObject->m_pFPOOLQueuedMsg != NULL);
	pdnObject->m_pFPOOLQueuedMsg->Initialize(	CQueuedMsg::FPMAlloc,
												CQueuedMsg::FPMInitialize,
												CQueuedMsg::FPMRelease,
												CQueuedMsg::FPMDealloc );

	// Initialize Fixed Pool for Worker Thread Jobs
	pdnObject->m_pFPOOLWorkerJob = new CLockedContextClassFixedPool< CWorkerJob >;
	DNASSERT(pdnObject->m_pFPOOLWorkerJob != NULL);
	pdnObject->m_pFPOOLWorkerJob->Initialize(	CWorkerJob::FPMAlloc,
												CWorkerJob::FPMInitialize,
												CWorkerJob::FPMRelease,
												CWorkerJob::FPMDealloc );

	// Initialize Fixed Pool for memory blocks
	pdnObject->m_pFPOOLMemoryBlockTiny = new CLockedContextClassFixedPool< CMemoryBlockTiny >;
	DNASSERT(pdnObject->m_pFPOOLMemoryBlockTiny != NULL);
	pdnObject->m_pFPOOLMemoryBlockTiny->Initialize(	CMemoryBlockTiny::FPMAlloc,
													CMemoryBlockTiny::FPMInitialize,
													CMemoryBlockTiny::FPMRelease,
													CMemoryBlockTiny::FPMDealloc );

	pdnObject->m_pFPOOLMemoryBlockSmall = new CLockedContextClassFixedPool< CMemoryBlockSmall >;
	DNASSERT(pdnObject->m_pFPOOLMemoryBlockSmall != NULL);
	pdnObject->m_pFPOOLMemoryBlockSmall->Initialize(CMemoryBlockSmall::FPMAlloc,
													CMemoryBlockSmall::FPMInitialize,
													CMemoryBlockSmall::FPMRelease,
													CMemoryBlockSmall::FPMDealloc );

	pdnObject->m_pFPOOLMemoryBlockMedium = new CLockedContextClassFixedPool< CMemoryBlockMedium >;
	DNASSERT(pdnObject->m_pFPOOLMemoryBlockMedium != NULL);
	pdnObject->m_pFPOOLMemoryBlockMedium->Initialize(CMemoryBlockMedium::FPMAlloc,
													CMemoryBlockMedium::FPMInitialize,
													CMemoryBlockMedium::FPMRelease,
													CMemoryBlockMedium::FPMDealloc );

	pdnObject->m_pFPOOLMemoryBlockLarge = new CLockedContextClassFixedPool< CMemoryBlockLarge >;
	DNASSERT(pdnObject->m_pFPOOLMemoryBlockLarge != NULL);
	pdnObject->m_pFPOOLMemoryBlockLarge->Initialize(CMemoryBlockLarge::FPMAlloc,
													CMemoryBlockLarge::FPMInitialize,
													CMemoryBlockLarge::FPMRelease,
													CMemoryBlockLarge::FPMDealloc );

	pdnObject->m_pFPOOLMemoryBlockHuge = new CLockedContextClassFixedPool< CMemoryBlockHuge >;
	DNASSERT(pdnObject->m_pFPOOLMemoryBlockHuge != NULL);
	pdnObject->m_pFPOOLMemoryBlockHuge->Initialize(	CMemoryBlockHuge::FPMAlloc,
													CMemoryBlockHuge::FPMInitialize,
													CMemoryBlockHuge::FPMRelease,
													CMemoryBlockHuge::FPMDealloc );

	//
	//	Create Protocol Object
	//
	if ((pdnObject->pdnProtocolData = reinterpret_cast<PProtocolData>(DNMalloc(sizeof(ProtocolData)))) == NULL)
	{
		DPFERR("DNMalloc() failed");
		return(E_OUTOFMEMORY);
	}
	pdnObject->hProtocolShutdownEvent = NULL;
	pdnObject->lProtocolRefCount = 0;

	// Initialize SP List
	pdnObject->m_bilinkServiceProviders.Initialize();

	//
	//	Initialize AsyncOp List
	//
	pdnObject->m_bilinkAsyncOps.Initialize();

	//
	//	Initialize outstanding CConection list
	//
	pdnObject->m_bilinkConnections.Initialize();

	//
	//	Initialize pending deletion list
	//
	pdnObject->m_bilinkPendingDeletions.Initialize();

	//
	//	Initialize active AsyncOp list
	//
	pdnObject->m_bilinkActiveList.Initialize();

	//
	//	Initialize worker thread job list
	//
	pdnObject->m_bilinkWorkerJobs.Initialize();

	//
	//	Initialize indicated connection list
	//
	pdnObject->m_bilinkIndicated.Initialize();

#ifdef	DEBUG
	// Debug counts
	if (!DNInitializeCriticalSection(&(pdnObject->csDebugCount)))
	{
		DPFERR("DNInitializeCriticalSection(Debug Count) failed");
		return(E_OUTOFMEMORY);
	}
	pdnObject->dwWorkerJobCount = 0;

	pdnObject->bDebugUse = FALSE;
#endif

#ifdef DPLAY_DOWORK
	pdnObject->m_hWorkerCloseEvent = NULL;
#else
	//
	//	Create worker thread event
	//
	if ((pdnObject->hWorkerEvent = CreateEvent(NULL,TRUE,FALSE,NULL)) == NULL)
	{
		DPFERR("Could not create worker thread event");
		return(DPNERR_OUTOFMEMORY);
	}
#endif

	//
	//	Create lock event
	//
	if ((pdnObject->hLockEvent = CreateEvent(NULL,TRUE,FALSE,NULL)) == NULL)
	{
		DPFERR("Unable to create lock event");
		return(DPNERR_OUTOFMEMORY);
	}

	return S_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "DIRECTNETOBJECT::~DIRECTNETOBJECT"
DIRECTNETOBJECT::~DIRECTNETOBJECT( void )
{
	DIRECTNETOBJECT *pdnObject = this;

	//
	//	No outstanding listens
	//
	DNASSERT(pdnObject->pListenParent == NULL);

	//
	//	No outstanding connect
	//
	DNASSERT(pdnObject->pConnectParent == NULL);

	//
	//	Host migration target
	//
	DNASSERT(pdnObject->pNewHost == NULL);

	//
	//	Protocol shutdown event
	//
	DNASSERT(pdnObject->hProtocolShutdownEvent == NULL);

	//
	//	Worker close event
	//
	DNASSERT(pdnObject->m_hWorkerCloseEvent == NULL);

	//
	//	Lock event
	//
	CloseHandle(pdnObject->hLockEvent);

#ifndef DPLAY_DOWORK
	// Worker Thread Queue
	if (pdnObject->hWorkerEvent)
		CloseHandle(pdnObject->hWorkerEvent);
#endif
	DNDeleteCriticalSection(&pdnObject->csWorkerQueue);

#ifdef	DEBUG
	// Debug counts
	DPF(2,"Worker Job FPM Outstanding: [%ld]",pdnObject->dwWorkerJobCount);
	DNDeleteCriticalSection(&pdnObject->csDebugCount);
#endif

    DN_FreeSPCapsList( pdnObject );

	// Protocol
	if (pdnObject->pdnProtocolData != NULL)
		DNFree(pdnObject->pdnProtocolData);

	// Active AsyncOp List Critical Section
	DNDeleteCriticalSection(&pdnObject->csActiveList);

	// NameTable operation list Critical Section
	DNDeleteCriticalSection(&pdnObject->csNameTableOpList);

	// Async Ops Critical Section
	DNDeleteCriticalSection(&pdnObject->csAsyncOperations);

	// Connection Critical Section
	DNDeleteCriticalSection(&pdnObject->csConnectionList);

#ifdef ENABLE_DPLAY_VOICE
	// Voice Critical Section
	DNDeleteCriticalSection(&pdnObject->csVoice);
#endif

	//
	// Deinitialize and delete fixed pools
	//
	if (pdnObject->m_pFPOOLAsyncOp)
	{
		pdnObject->m_pFPOOLAsyncOp->Deinitialize();
		delete pdnObject->m_pFPOOLAsyncOp;
		pdnObject->m_pFPOOLAsyncOp = NULL;
	}

	if (pdnObject->m_pFPOOLRefCountBuffer)
	{
		pdnObject->m_pFPOOLRefCountBuffer->Deinitialize();
		delete pdnObject->m_pFPOOLRefCountBuffer;
		pdnObject->m_pFPOOLRefCountBuffer = NULL;
	}

	if (pdnObject->m_pFPOOLSyncEvent)
	{
		pdnObject->m_pFPOOLSyncEvent->Deinitialize();
		delete pdnObject->m_pFPOOLSyncEvent;
		pdnObject->m_pFPOOLSyncEvent = NULL;
	}

	if (pdnObject->m_pFPOOLConnection)
	{
		pdnObject->m_pFPOOLConnection->Deinitialize();
		delete pdnObject->m_pFPOOLConnection;
		pdnObject->m_pFPOOLConnection = NULL;
	}

	if (pdnObject->m_pFPOOLGroupConnection)
	{
		pdnObject->m_pFPOOLGroupConnection->Deinitialize();
		delete pdnObject->m_pFPOOLGroupConnection;
		pdnObject->m_pFPOOLGroupConnection = NULL;
	}

	if (pdnObject->m_pFPOOLGroupMember)
	{
		pdnObject->m_pFPOOLGroupMember->Deinitialize();
		delete pdnObject->m_pFPOOLGroupMember;
		pdnObject->m_pFPOOLGroupMember = NULL;
	}

	if (pdnObject->m_pFPOOLNameTableEntry)
	{
		pdnObject->m_pFPOOLNameTableEntry->Deinitialize();
		delete pdnObject->m_pFPOOLNameTableEntry;
		pdnObject->m_pFPOOLNameTableEntry = NULL;
	}

	if (pdnObject->m_pFPOOLNameTableOp)
	{
		pdnObject->m_pFPOOLNameTableOp->Deinitialize();
		delete pdnObject->m_pFPOOLNameTableOp;
		pdnObject->m_pFPOOLNameTableOp = NULL;
	}

	if (pdnObject->m_pFPOOLPendingDeletion)
	{
		pdnObject->m_pFPOOLPendingDeletion->Deinitialize();
		delete pdnObject->m_pFPOOLPendingDeletion;
		pdnObject->m_pFPOOLPendingDeletion = NULL;
	}

	if (pdnObject->m_pFPOOLQueuedMsg)
	{
		pdnObject->m_pFPOOLQueuedMsg->Deinitialize();
		delete pdnObject->m_pFPOOLQueuedMsg;
		pdnObject->m_pFPOOLQueuedMsg = NULL;
	}

	if (pdnObject->m_pFPOOLWorkerJob)
	{
		pdnObject->m_pFPOOLWorkerJob->Deinitialize();
		delete pdnObject->m_pFPOOLWorkerJob;
		pdnObject->m_pFPOOLWorkerJob = NULL;
	}

	if (pdnObject->m_pFPOOLMemoryBlockTiny)
	{
		pdnObject->m_pFPOOLMemoryBlockTiny->Deinitialize();
		delete pdnObject->m_pFPOOLMemoryBlockTiny;
		pdnObject->m_pFPOOLMemoryBlockTiny = NULL;
	}

	if (pdnObject->m_pFPOOLMemoryBlockSmall)
	{
		pdnObject->m_pFPOOLMemoryBlockSmall->Deinitialize();
		delete pdnObject->m_pFPOOLMemoryBlockSmall;
		pdnObject->m_pFPOOLMemoryBlockSmall = NULL;
	}

	if (pdnObject->m_pFPOOLMemoryBlockMedium)
	{
		pdnObject->m_pFPOOLMemoryBlockMedium->Deinitialize();
		delete pdnObject->m_pFPOOLMemoryBlockMedium;
		pdnObject->m_pFPOOLMemoryBlockMedium = NULL;
	}

	if (pdnObject->m_pFPOOLMemoryBlockLarge)
	{
		pdnObject->m_pFPOOLMemoryBlockLarge->Deinitialize();
		delete pdnObject->m_pFPOOLMemoryBlockLarge;
		pdnObject->m_pFPOOLMemoryBlockLarge = NULL;
	}

	if (pdnObject->m_pFPOOLMemoryBlockHuge)
	{
		pdnObject->m_pFPOOLMemoryBlockHuge->Deinitialize();
		delete pdnObject->m_pFPOOLMemoryBlockHuge;
		pdnObject->m_pFPOOLMemoryBlockHuge = NULL;
	}

	// Delete DirectNet critical section
	DNDeleteCriticalSection(&pdnObject->csDirectNetObject);

	// Enum listen address
	if (pdnObject->pIDP8AEnum != NULL)
	{
		pdnObject->pIDP8AEnum->Release();
		pdnObject->pIDP8AEnum = NULL;
	}

	DNEnterCriticalSection(&g_csDPlayInterfaces);
    for (DWORD i = 0; i < 16; i += 1)
    {
        if (g_DPlayInterfaces[i] == this)
        {
            g_DPlayInterfaces[i] = NULL;
            break;
        }
    }
	DNLeaveCriticalSection(&g_csDPlayInterfaces);

    g_dwGlobalObjectCount -= 1;
}

