//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#ifdef _XBOX // Damn ANSI conversion
#include <stdio.h>
#include <stdlib.h>
#endif

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\sprintf.h"

#include "tncontrl.h"
#include "main.h"

#include "faultsim.h"



//==================================================================================
// Defines
//==================================================================================
#ifndef _XBOX // Damn ANSI conversion
#define FAULTSIM_PREFIX_MUTEX		"TN FaultSim mutex "
#else
#define FAULTSIM_PREFIX_MUTEX		L"TN FaultSim mutex "
#endif






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSim::CTNFaultSim()"
//==================================================================================
// CTNFaultSim constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNFaultSim object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNFaultSim::CTNFaultSim(void):
	m_dwID(0),
	m_fInitialized(FALSE),
	m_hExclusiveLock(NULL)
{
	DPL(9, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNFaultSim));
} // CTNFaultSim::CTNFaultSim
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSim::~CTNFaultSim()"
//==================================================================================
// CTNFaultSim destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNFaultSim object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNFaultSim::~CTNFaultSim(void)
{
	HRESULT		hr;


	DPL(9, "this = %x", 1, this);

	if (this->m_fInitialized)
	{
		DPL(0, "WARNING: Fault sim object %x is still initialized on delete!",
			1, this);

		hr = this->Release();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't release fault sim object %x!", 2, this, hr);
		} // end if (couldn't release object)
	} // end if (still initialized)

	// Just in case we somehow have the lock still.
	if (this->m_hExclusiveLock != NULL)
	{
		ReleaseMutex(this->m_hExclusiveLock);
		CloseHandle(this->m_hExclusiveLock);
		this->m_hExclusiveLock = NULL;
	} // end if (have mutex handle)
} // CTNFaultSim::~CTNFaultSim
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSim::Initialize()"
//==================================================================================
// CTNFaultSim::Initialize
//----------------------------------------------------------------------------------
//
// Description: Sets up this object, and prepares it for use.
//
// Arguments:
//	LPVOID lpvInitData		Pointer to data to use when initializing.
//	DWORD dwInitDataSize	Size of data to use when initializing.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSim::Initialize(LPVOID lpvInitData, DWORD dwInitDataSize)
{
	HRESULT		hr = S_OK;


	DPL(9, "==>", 0);

	if (this->m_fInitialized)
	{
		DPL(0, "FaultSim already initialized!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (not initialized)


DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNFaultSim::Initialize
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSim::Release()"
//==================================================================================
// CTNFaultSim::Release
//----------------------------------------------------------------------------------
//
// Description: Releases this object after a successful call to Initialize.  Must be
//				done prior to deletion.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSim::Release(void)
{
	HRESULT		hr = S_OK;


	DPL(9, "==>", 0);

	if (! this->m_fInitialized)
	{
		DPL(0, "FaultSim not initialized!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (not initialized)


	// Drop the lock, if we have it.
	if (this->m_hExclusiveLock != NULL)
	{
		ReleaseMutex(this->m_hExclusiveLock);
		CloseHandle(this->m_hExclusiveLock);
		this->m_hExclusiveLock = NULL;
	} // end if (have mutex handle)


DONE:

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNFaultSim::Release
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSim::EnsureExclusiveUse()"
//==================================================================================
// CTNFaultSim::EnsureExclusiveUse
//----------------------------------------------------------------------------------
//
// Description: Prevents other threads and processes from using this type of fault
//				simulator simultaneously.
//				This may block until a previous fault simulator relenquishes its
//				exclusive control.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSim::EnsureExclusiveUse(void)
{
	HRESULT		hr;
	char		szName[256];


#ifndef _XBOX // Damn ANSI conversion
	wsprintf(szName, FAULTSIM_PREFIX_MUTEX "%u", this->m_dwID);
	this->m_hExclusiveLock = CreateMutex(NULL, FALSE, szName);
#else // ! XBOX
	WCHAR		szWideName[256];
	// Use swprintf and then convert the result ot ANSI
	swprintf(szWideName, FAULTSIM_PREFIX_MUTEX L"%u", this->m_dwID);
	this->m_hExclusiveLock = CreateMutex(NULL, FALSE, szWideName);
#endif // XBOX
	if (this->m_hExclusiveLock == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get fault sim exclusive lock \"%s\"!", 1, szName);
		goto ERROR_EXIT;
	} // end if (couldn't create mutex)


	// Now grab the mutex lock

#pragma BUGBUG(vanceo, "Get user cancel here")
	hr = WaitForSingleObjectEx(this->m_hExclusiveLock, INFINITE, TRUE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// Got the lock, we're cool
		  break;

		default:
			// Got some unexpected error
			DPL(0, "Got unexpected result from WaitForSingleObjectEx!", 0);
			goto ERROR_EXIT;
		  break;
	} // end switch (on wait result)

	return (S_OK);


ERROR_EXIT:


	if (this->m_hExclusiveLock != NULL)
	{
		CloseHandle(this->m_hExclusiveLock);
		this->m_hExclusiveLock = NULL;
	} // end if (have mutex handle)

	return (hr);
} // CTNFaultSim::EnsureExclusiveUse
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNFaultSim::RelenquishExclusiveUse()"
//==================================================================================
// CTNFaultSim::RelenquishExclusiveUse
//----------------------------------------------------------------------------------
//
// Description: Allows other threads and processes to use this type of fault
//				simulator after previously gaining exclusive control.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNFaultSim::RelenquishExclusiveUse(void)
{
	HRESULT		hr = S_OK;


	if (this->m_hExclusiveLock == NULL)
	{
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (don't have lock)

	ReleaseMutex(this->m_hExclusiveLock);
	CloseHandle(this->m_hExclusiveLock);
	this->m_hExclusiveLock = NULL;


DONE:

	return (hr);
} // CTNFaultSim::RelenquishExclusiveUse
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
