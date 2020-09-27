//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"

#include "tncontrl.h"
#include "main.h"

#include "teststats.h"






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::CTNTestStats()"
//==================================================================================
// CTNTestStats constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTestStats object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNTestStats::CTNTestStats(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNTestStats));

	InitializeCriticalSection(&this->m_cs);
	this->m_dwSuccesses = 0;
	this->m_dwFailures = 0;
	this->m_dwWarnings = 0;
} // CTNTestStats::CTNTestStats
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::~CTNTestStats()"
//==================================================================================
// CTNTestStats destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTestStats object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTestStats::~CTNTestStats(void)
{
	//DPL(0, "this = %x", 1, this);

	DeleteCriticalSection(&this->m_cs);
} // CTNTestStats::~CTNTestStats
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::Lock()"
//==================================================================================
// CTNTestStats::Lock
//----------------------------------------------------------------------------------
//
// Description: Causes all other threads attempting to lock this object to wait
//				until the caller Unlocks this object.  Nested calls are okay, as
//				long as each is balanced.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void CTNTestStats::Lock(void)
{
	EnterCriticalSection(&this->m_cs);
} // CTNTestStats::Lock
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::Unlock()"
//==================================================================================
// CTNTestStats::Unlock
//----------------------------------------------------------------------------------
//
// Description: Allows other threads to lock this object again after a Lock.  Nested
//				calls are okay, as long as each is balanced.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void CTNTestStats::Unlock(void)
{
	LeaveCriticalSection(&this->m_cs);
} // CTNTestStats::Unlock
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::GetSuccesses()"
//==================================================================================
// CTNTestStats::GetSuccesses
//----------------------------------------------------------------------------------
//
// Description: Returns the number of successes this object has.  It is assumed the
//				caller has already locked the object as necessary.
//
// Arguments: None.
//
// Returns: The number of successes.
//==================================================================================
DWORD CTNTestStats::GetSuccesses(void)
{
	return (this->m_dwSuccesses);
} // CTNTestStats::GetSuccesses
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::GetFailures()"
//==================================================================================
// CTNTestStats::GetFailures
//----------------------------------------------------------------------------------
//
// Description: Returns the number of failures this object has.  It is assumed the
//				caller has already locked the object as necessary.
//
// Arguments: None.
//
// Returns: The number of successes.
//==================================================================================
DWORD CTNTestStats::GetFailures(void)
{
	return (this->m_dwFailures);
} // CTNTestStats::GetFailures
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::GetWarnings()"
//==================================================================================
// CTNTestStats::GetWarnings
//----------------------------------------------------------------------------------
//
// Description: Returns the number of warnings this object has.  It is assumed the
//				caller has already locked the object as necessary.
//
// Arguments: None.
//
// Returns: The number of successes.
//==================================================================================
DWORD CTNTestStats::GetWarnings(void)
{
	return (this->m_dwWarnings);
} // CTNTestStats::GetWarnings
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::GetTotalComplete()"
//==================================================================================
// CTNTestStats::GetTotalComplete
//----------------------------------------------------------------------------------
//
// Description: Returns the total number of successes and failures this object has.
//				It is assumed the caller has already locked the object as necessary.
//
// Arguments: None.
//
// Returns: The number of successes.
//==================================================================================
DWORD CTNTestStats::GetTotalComplete(void)
{
	return (this->m_dwSuccesses + this->m_dwFailures);
} // CTNTestStats::GetTotalComplete
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::IncrementSuccesses()"
//==================================================================================
// CTNTestStats::IncrementSuccesses
//----------------------------------------------------------------------------------
//
// Description: Increases number of successes this object has by one.  It is assumed
//				the caller has already locked the object as necessary.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void CTNTestStats::IncrementSuccesses(void)
{
	this->m_dwSuccesses++;
} // CTNTestStats::IncrementSuccesses
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::IncrementFailures()"
//==================================================================================
// CTNTestStats::IncrementFailures
//----------------------------------------------------------------------------------
//
// Description: Increases number of failures this object has by one.  It is assumed
//				the caller has already locked the object as necessary.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void CTNTestStats::IncrementFailures(void)
{
	this->m_dwFailures++;
} // CTNTestStats::IncrementFailures
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::IncrementWarnings()"
//==================================================================================
// CTNTestStats::IncrementWarnings
//----------------------------------------------------------------------------------
//
// Description: Increases number of warnings this object has by one.  It is assumed
//				the caller has already locked the object as necessary.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void CTNTestStats::IncrementWarnings(void)
{
	this->m_dwWarnings++;
} // CTNTestStats::IncrementWarnings
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestStats::Copy()"
//==================================================================================
// CTNTestStats::Copy
//----------------------------------------------------------------------------------
//
// Description: Copies the values of the stats object passed in to this object.
//
// Arguments:
//	PTNTESTSTATS pStatsToCopy	Stats object to copy values from.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestStats::Copy(PTNTESTSTATS pStatsToCopy)
{
	this->Lock();
	pStatsToCopy->Lock();

	this->m_dwSuccesses = pStatsToCopy->m_dwSuccesses;
	this->m_dwFailures = pStatsToCopy->m_dwFailures;
	this->m_dwWarnings = pStatsToCopy->m_dwWarnings;

	pStatsToCopy->Unlock();
	this->Unlock();

	return (S_OK);
} // CTNTestStats::Copy
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

