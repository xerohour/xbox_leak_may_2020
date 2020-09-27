//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"

#include "tncontrl.h"
#include "main.h"

#include "comm.h"
#include "reachcheck.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReachCheck::CTNReachCheck()"
//==================================================================================
// CTNReachCheck constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNReachCheck object.  Initializes the data structures.
//
// Arguments:
//	DWORD dwID				ID of reach check.
//	DWORD dwMethod			Method of communication being checked.
//	PVOID pvMethodData		Pointer to method data being used, if any.
//	DWORD dwMethodDataSize	Size of method data, if any.
//
// Returns: None (just the object).
//==================================================================================
CTNReachCheck::CTNReachCheck(DWORD dwID, DWORD dwMethod, PVOID pvMethodData,
							DWORD dwMethodDataSize)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNReachCheck));


	this->m_dwID = dwID;

	this->m_dwMethod = dwMethod;

	this->m_pvMethodData = NULL;
	this->m_dwMethodDataSize = dwMethodDataSize;

	if (pvMethodData != NULL)
	{
		this->m_pvMethodData = LocalAlloc(LPTR, dwMethodDataSize);
		if (this->m_pvMethodData != NULL)
		{
			CopyMemory(this->m_pvMethodData, pvMethodData, dwMethodDataSize);
			this->m_dwMethodDataSize = dwMethodDataSize;
		} // end if (method data)
	} // end if (method data)

	this->m_hEvent = NULL;

	this->m_fResult = FALSE;
	this->m_hresult = E_FAIL;

	this->m_pPeriodicSend = NULL;

	this->m_pvAddress = NULL;
	this->m_dwAddressSize = 0;

	this->m_pCtrlComm = NULL;
	this->m_pvCommData = NULL;
} // CTNReachCheck::CTNReachCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReachCheck::~CTNReachCheck()"
//==================================================================================
// CTNReachCheck destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNReachCheck object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNReachCheck::~CTNReachCheck(void)
{
	//DPL(0, "this = %x", 1, this);


	if (this->m_pvMethodData != NULL)
	{
		LocalFree(this->m_pvMethodData);
		this->m_pvMethodData = NULL;
	} // end if (allocated output data)

	if (this->m_hEvent != NULL)
	{
		CloseHandle(this->m_hEvent);
		this->m_hEvent = NULL;
	} // end if (have event)

	if (this->m_pPeriodicSend != NULL)
	{
		this->m_pPeriodicSend->m_dwRefCount--;
		if (this->m_pPeriodicSend->m_dwRefCount == 0)
		{
			DPL(7, "Deleting periodic send object %x.", 1, this->m_pPeriodicSend);

			// If there's a context, we know it's allocated memory.  Free it.
			if (this->m_pPeriodicSend->m_pvUserContext != NULL)
			{
				LocalFree(this->m_pPeriodicSend->m_pvUserContext);
				this->m_pPeriodicSend->m_pvUserContext = NULL;
			} // end if (there's a user context)

			delete (this->m_pPeriodicSend);
		} // end if (last reference)
		else
		{
			DPL(7, "Not deleting periodic send object %x, its refcount is %u.",
				2, this->m_pPeriodicSend, this->m_pPeriodicSend->m_dwRefCount);
		} // end if (shouldn't delete object)
		this->m_pPeriodicSend = NULL;
	} // end if (there's a slave object)

	if (this->m_pvAddress != NULL)
	{
		LocalFree(this->m_pvAddress);
		this->m_pvAddress = NULL;
	} // end if (allocated output data)

	if (this->m_pCtrlComm != NULL)
	{
		this->m_pCtrlComm->m_dwRefCount--;
		if (this->m_pCtrlComm->m_dwRefCount == 0)
		{
			DPL(7, "Deleting control comm object %x.", 1, this->m_pCtrlComm);
			delete (this->m_pCtrlComm);
		} // end if (last reference)
		else
		{
			DPL(7, "Not deleting control comm object %x, its refcount is %u.",
				2, this->m_pCtrlComm, this->m_pCtrlComm->m_dwRefCount);
		} // end if (shouldn't delete object)
		this->m_pCtrlComm = NULL;
	} // end if (there's a slave object)
} // CTNReachCheck::~CTNReachCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReachChecksList::GetCheck()"
//==================================================================================
// CTNReachChecksList::GetCheck
//----------------------------------------------------------------------------------
//
// Description: Returns the CTNReachCheck item with the specified ID, or NULL if not
//				found.
//
// Arguments:
//	DWORD dwID	ID of check to search for.
//
// Returns: Pointer to CTNReachCheck or NULL if not found.
//==================================================================================
PTNREACHCHECK CTNReachChecksList::GetCheck(DWORD dwID)
{
	int				i;
	PTNREACHCHECK	pItem = NULL;


#ifdef DEBUG
	if (this == NULL)
	{
		DPL(0, "CTNReachChecksList pointer is NULL!", 0);

		DEBUGBREAK();

		return (NULL);
	} // end if (got passed invalid object)
#endif // DEBUG

	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNREACHCHECK) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get item %i!", 1, i);
			return (NULL);
		} // end if (couldn't get that item)

		if (pItem->m_dwID == dwID)
		{
			this->LeaveCritSection();
			return (pItem);
		} // end if (found the ID the caller was looking for)
	} // end for (each item in the list)
	this->LeaveCritSection();

	return (NULL);
} // CTNReachChecksList::GetCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReachCheckTarget::CTNReachCheckTarget()"
//==================================================================================
// CTNReachCheckTarget constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNReachCheckTarget object.  Initializes the data
//				structures.
//
// Arguments:
//	PTNCTRLMACHINEID pSlaveID	Pointer to ID of slave this pertains to.
//
// Returns: None (just the object).
//==================================================================================
CTNReachCheckTarget::CTNReachCheckTarget(PTNCTRLMACHINEID pSlaveID)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNReachCheckTarget));

	CopyMemory(&(this->m_id), pSlaveID, sizeof (TNCTRLMACHINEID));
} // CTNReachCheckTarget::CTNReachCheckTarget
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReachCheckTarget::~CTNReachCheckTarget()"
//==================================================================================
// CTNReachCheckTarget destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNReachCheckTarget object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNReachCheckTarget::~CTNReachCheckTarget(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNReachCheckTarget::~CTNReachCheckTarget
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNReachCheckTargetsList::GetTarget()"
//==================================================================================
// CTNReachCheckTargetsList::GetTarget
//----------------------------------------------------------------------------------
//
// Description: Returns the CTNReachCheckTarget item for the specified slave, or
//				NULL if not found.
//
// Arguments:
//	PTNCTRLMACHINEID pSlaveID	ID of slave to search for.
//
// Returns: Pointer to CTNReachCheckTarget or NULL if not found.
//==================================================================================
PTNREACHCHECKTARGET CTNReachCheckTargetsList::GetTarget(PTNCTRLMACHINEID pSlaveID)
{
	int						i;
	PTNREACHCHECKTARGET		pItem = NULL;


#ifdef DEBUG
	if (this == NULL)
	{
		DPL(0, "CTNReachCheckTargetsList pointer is NULL!", 0);

		DEBUGBREAK();

		return (NULL);
	} // end if (got passed invalid object)
#endif // DEBUG

	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNREACHCHECKTARGET) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get item %i!", 1, i);
			return (NULL);
		} // end if (couldn't get that item)

		if (memcmp(&(pItem->m_id), pSlaveID, sizeof (TNCTRLMACHINEID)) == 0)
		{
			this->LeaveCritSection();
			return (pItem);
		} // end if (found the slave the caller was looking for)
	} // end for (each item in the list)
	this->LeaveCritSection();

	return (NULL);
} // CTNReachCheckTargetsList::GetTarget
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
