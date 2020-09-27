//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\sprintf.h"

#include "tncontrl.h"
#include "main.h"

#include "sendq.h"
#include "comm.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlComm::CTNCtrlComm()"
//==================================================================================
// CTNCtrlComm constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNCtrlComm object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNCtrlComm::CTNCtrlComm(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNCtrlComm));

	this->m_hSendThread = NULL;
	this->m_hReceiveThread = NULL;

	this->m_dwMethodID = 0;
	this->m_fMaster = FALSE;
	this->m_lpSendThreadProc = NULL;
	this->m_lpReceiveThreadProc = NULL;

	this->m_dwRefCount = 0;
	this->m_pControlObj = NULL;
	this->m_hKillSendThreadEvent = NULL;
	this->m_hKillReceiveThreadEvent = NULL;
} // CTNCtrlComm::CTNCtrlComm
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlComm::~CTNCtrlComm()"
//==================================================================================
// CTNCtrlComm destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNCtrlComm object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNCtrlComm::~CTNCtrlComm(void)
{
	HRESULT		hr;


	//DPL(0, "this = %x", 1, this);

#ifdef DEBUG
	if (this->m_dwRefCount != 0)
	{
		DPL(0, "Deleting ctrl comm object %x when refcount is not 0 (its %u)!  DEBUGBREAK()-ing",
			2, this, this->m_dwRefCount);

		DEBUGBREAK();
	} // end if (not last refcount)
#endif // DEBUG


	// Empty the send queue
	hr = this->m_sendqueue.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "Failed remove all the data to send from the queue!  %e", 1, hr);
	} // end if (couldn't empty send queue)

	hr = this->KillThreads();
	if (hr != S_OK)
	{
		DPL(0, "Failed to kill threads!  %e", 1, hr);
	} // end if (failed to kill threads)
} // CTNCtrlComm::~CTNCtrlComm
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlComm::StartThreads()"
//==================================================================================
// CTNCtrlComm::StartThreads
//----------------------------------------------------------------------------------
//
// Description: Creates the events and threads the control layer needs.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlComm::StartThreads(void)
{
	DWORD	dwThreadID;


	if (this->m_lpSendThreadProc != NULL)
	{
		// Create send thread and associated manual reset event
		this->m_hKillSendThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (this->m_hKillSendThreadEvent == NULL)
			return (GetLastError());

		this->m_hSendThread = CreateThread(NULL, 0, this->m_lpSendThreadProc,
										this, 0, &dwThreadID);
		if (this->m_hSendThread == NULL)
			return (GetLastError());
	} // end if (the send thread can be created)

	if (this->m_lpReceiveThreadProc != NULL)
	{
		// Create receive thread and associated manual reset event
		this->m_hKillReceiveThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (this->m_hKillReceiveThreadEvent == NULL)
			return (GetLastError());

		this->m_hReceiveThread = CreateThread(NULL, 0, this->m_lpReceiveThreadProc,
											this, 0, &dwThreadID);
		if (this->m_hReceiveThread == NULL)
			return (GetLastError());
	} // end if (the receive thread can be created)

	return (S_OK);
} // CTNCtrlComm::StartThreads
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlComm::KillThreads()"
//==================================================================================
// CTNCtrlComm::KillThreads
//----------------------------------------------------------------------------------
//
// Description: Shuts down any threads and releases the events the control layer
//				used.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlComm::KillThreads(void)
{
	// Close all items associated with the send thread
	if (this->m_hSendThread != NULL)
	{
		if (this->m_hKillSendThreadEvent == NULL) // ack, we won't be able to tell it to die
			return (E_FAIL);

		if (! SetEvent(this->m_hKillSendThreadEvent))
			return (E_FAIL);

		switch (WaitForSingleObject(this->m_hSendThread, TIMEOUT_DIE_CONTROLSENDTHREAD))
		{
			case WAIT_OBJECT_0:
				// what we want
			  break;
			case WAIT_TIMEOUT:
				DPL(0, "Waited %i ms for control send thread to die but it didn't!",
					1, TIMEOUT_DIE_CONTROLSENDTHREAD);


#ifndef DONT_BREAK_ON_KILLTHREAD_TIMEOUT
				DEBUGBREAK();
#endif // DONT_BREAK_ON_KILLTHREAD_TIMEOUT

			  break;
			default:
				DPL(0, "Got unexpected return code from WaitForSingleObject on the control send thread!", 0);
			  break;
		} // end switch (on result of waiting for thread to die)

		CloseHandle(this->m_hSendThread);
		this->m_hSendThread = NULL;
	} // end if (the send thread exists)

	if (this->m_hKillSendThreadEvent != NULL)
	{
		CloseHandle(this->m_hKillSendThreadEvent);
		this->m_hKillSendThreadEvent = NULL;
	} // end if (have event)

	// Close all items associated with the receive thread
	if (this->m_hReceiveThread != NULL)
	{
		if (this->m_hKillReceiveThreadEvent == NULL) // ack, we won't be able to tell it to die
			return (E_FAIL);

		if (! SetEvent(this->m_hKillReceiveThreadEvent))
			return (E_FAIL);

		switch (WaitForSingleObject(this->m_hReceiveThread, TIMEOUT_DIE_CONTROLRECEIVETHREAD))
		{
			case WAIT_OBJECT_0:
				// what we want
			  break;
			case WAIT_TIMEOUT:
				DPL(0, "Waited %i ms for control receive thread to die but it didn't!",
					1, TIMEOUT_DIE_CONTROLRECEIVETHREAD);


#ifndef DONT_BREAK_ON_KILLTHREAD_TIMEOUT
				DEBUGBREAK();
#endif // DONT_BREAK_ON_KILLTHREAD_TIMEOUT

			  break;
			default:
				DPL(0, "Got unexpected return code from WaitForSingleObject on the control receive thread!", 0);
			  break;
		} // end switch (on result of waiting for thread to die)

		CloseHandle(this->m_hReceiveThread);
		this->m_hReceiveThread = NULL;
	} // end if (the receive thread exists)

	if (this->m_hKillReceiveThreadEvent != NULL)
	{
		CloseHandle(this->m_hKillReceiveThreadEvent);
		this->m_hKillReceiveThreadEvent = NULL;
	} // end if (have event)

	return (S_OK);
} // CTNCtrlComm::KillThreads
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

