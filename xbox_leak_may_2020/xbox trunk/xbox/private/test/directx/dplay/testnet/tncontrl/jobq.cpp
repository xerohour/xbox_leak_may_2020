//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
//#include "..\tncommon\stack.h"

#include "tncontrl.h"
#include "main.h"

#include "slaveinfo.h"
#include "jobq.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNJob::CTNJob()"
//==================================================================================
// CTNJob constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNJob object.  Initializes the data
//				structures.
//
// Arguments:
//	DWORD dwOperation			Operation to this job represents.
//	DWORD dwTime				Time this job should be run.
//	PTNCTRLMACHINEID pSlaveID	ID of slave this job should be run on, if any.
//	PVOID pvContext				Job specific context for this operation.
//
// Returns: None (just the object).
//==================================================================================
CTNJob::CTNJob(DWORD dwOperation, DWORD dwTime, PTNCTRLMACHINEID pSlaveID, PVOID pvContext)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNJob));

	this->m_dwOperation = dwOperation;
	this->m_dwTime = dwTime;

	if (pSlaveID == NULL)
	{
		ZeroMemory(&(this->m_idSlave), sizeof (TNCTRLMACHINEID));
	} // end if (there's no ID)
	else
	{
		CopyMemory(&(this->m_idSlave), pSlaveID, sizeof (TNCTRLMACHINEID));
	} // end else (there's a slave ID)

	this->m_pvContext = pvContext;
} // CTNJob::CTNJob
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNJob::~CTNJob()"
//==================================================================================
// CTNJob destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNJob object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNJob::~CTNJob(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNJob::~CTNJob
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNJobQueue::CTNJobQueue()"
//==================================================================================
// CTNJobQueue constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNJobQueue object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNJobQueue::CTNJobQueue(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNJobQueue));

	// Auto reset event
	this->m_hNewJobEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (this->m_hNewJobEvent == NULL)
	{
		HRESULT		hr;


		hr = GetLastError();
		DPL(0, "Failed to create send data event!  %e", 1, hr);
	} // end if (failed to create event)
} // CTNJobQueue::CTNJobQueue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNJobQueue::~CTNJobQueue()"
//==================================================================================
// CTNJobQueue destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNJobQueue object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNJobQueue::~CTNJobQueue(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_hNewJobEvent != NULL)
	{
		CloseHandle(this->m_hNewJobEvent);
		this->m_hNewJobEvent = NULL;
	} // end if (have event)
} // CTNJobQueue::~CTNJobQueue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNJobQueue::AddJob()"
//==================================================================================
// CTNJobQueue::AddJob
//----------------------------------------------------------------------------------
//
// Description: Adds a new job for the thread to perform.
//
// Arguments:
//	DWORD dwOperation			ID of work to be performed.
//	DWORD dwDelay				How far into the future this operation should be
//								performed, or 0 for as soon as possible.
//	PTNCTRLMACHINEID pSlaveID	ID of slave to perform operation on, if any.
//	PVOID pvContext				Job specific context for this operation.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNJobQueue::AddJob(DWORD dwOperation, DWORD dwDelay,
							PTNCTRLMACHINEID pSlaveID, PVOID pvContext)
{
	HRESULT		hr;
	PTNJOB		pJob = NULL;
	BOOL		fInCritSect = FALSE;
	int			i;
	PTNJOB		pOtherJob = NULL;
	//PVOID		pvCallersCaller;


	/*
	// Save the address of the function calling us for debugging purposes.
	TNStackGetCallersAddress(&(pNewSendData->m_pvSubmittersAddress),
							&pvCallersCaller);

	DPL(9, "Data %x submitted by %X, %X",
		3, pNewSendData->m_pvData, pNewSendData->m_pvSubmittersAddress, pvCallersCaller);
	*/

	pJob = new (CTNJob)(dwOperation, ((dwDelay == 0) ? 0 : (GetTickCount() + dwDelay)),
						pSlaveID, pvContext);
	if (pJob == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	DPL(8, "Queueing job %x (op %u, to be run at %u).",
		3, pJob , pJob->m_dwOperation, pJob->m_dwTime);

	this->EnterCritSection();
	fInCritSect = TRUE;

	for(i = 0; i < this->Count(); i++)
	{
		pOtherJob = (PTNJOB) this->GetItem(i);
		if (pOtherJob == NULL)
		{
			DPL(0, "Couldn't get job %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		if (pOtherJob->m_dwTime > pJob->m_dwTime)
		{
			hr = this->InsertBeforeIndex(pJob, i);
			if (hr != S_OK)
			{
				DPL(0, "Failed to insert job before item %i!", 1, i);
				goto DONE;
			} // end if (failed to add item)

			pJob = NULL; // so we don't readd it to the list

			// We're done here
			break;
		} // end if (the current job is after the new job)
	} // end for (each job already in the queue)

	// If we didn't insert it into the list above, just add it to the end.
	if (pJob != NULL)
	{
		hr = this->Add(pJob);
		if (hr != S_OK)
		{
			DPL(0, "Failed to add job to end of list!", 0);
			goto DONE;
		} // end if (failed to add item)

		// Forget about it so we don't free it below, in case we didn't do it
		// above.
		pJob = NULL;
	} // end if (didn't add job above)


	if (! SetEvent(this->m_hNewJobEvent))
	{
		hr = GetLastError();

		if (hr == S_OK)
			hr = E_FAIL;

		DPL(0, "Couldn't set new job event %x!", 1, this->m_hNewJobEvent);

		goto DONE;
	} // end if (couldn't set event)


DONE:

	if (fInCritSect)
	{
		fInCritSect = FALSE;
		this->LeaveCritSection();
	} // end if (in critical section)

	if (pJob != NULL)
	{
		delete (pJob);
		pJob = NULL;
	} // end if (still have object)

	return (hr);
} // CTNJobQueue::AddJob
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

