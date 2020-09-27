//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\stack.h"

#include "tncontrl.h"
#include "main.h"

#include "sendq.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSendData::CTNSendData()"
//==================================================================================
// CTNSendData constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNSendData object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNSendData::CTNSendData(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNSendData));

	this->m_fGuaranteed = FALSE;
	this->m_pvAddress = NULL;
	this->m_dwAddressSize = 0;
	this->m_pvData = NULL;
	this->m_dwDataSize = 0;
	this->m_pvSubmittersAddress = NULL;
} // CTNSendData::CTNSendData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSendData::~CTNSendData()"
//==================================================================================
// CTNSendData destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNSendData object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNSendData::~CTNSendData(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_pvAddress != NULL)
	{
		LocalFree(this->m_pvAddress);
		this->m_pvAddress = NULL;
	} // end if (an address was allocated)

	if (this->m_pvData != NULL)
	{
		LocalFree(this->m_pvData);
		this->m_pvData = NULL;
	} // end if (data was allocated)
} // CTNSendData::~CTNSendData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSendDataQueue::CTNSendDataQueue()"
//==================================================================================
// CTNSendDataQueue constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNSendDataQueue object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNSendDataQueue::CTNSendDataQueue(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNSendDataQueue));

	// Auto reset event
	this->m_hSendDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (this->m_hSendDataEvent == NULL)
	{
		HRESULT		hr;


		hr = GetLastError();
		DPL(0, "Failed to create send data event!  %e", 1, hr);
	} // end if (failed to create event)
} // CTNSendDataQueue::CTNSendDataQueue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSendDataQueue::~CTNSendDataQueue()"
//==================================================================================
// CTNSendDataQueue destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNSendDataQueue object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNSendDataQueue::~CTNSendDataQueue(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_hSendDataEvent != NULL)
	{
		CloseHandle(this->m_hSendDataEvent);
		this->m_hSendDataEvent = NULL;
	} // end if (have event)
} // CTNSendDataQueue::~CTNSendDataQueue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSendDataQueue::AddSendData()"
//==================================================================================
// CTNSendDataQueue::AddSendData
//----------------------------------------------------------------------------------
//
// Description: Adds the passed item onto the queue and sets the event.
//
// Arguments:
//	PTNSENDDATA pNewSendData	Item to add.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNSendDataQueue::AddSendData(PTNSENDDATA pNewSendData)
{
	HRESULT		hr;
	PVOID		pvCallersCaller;


	// Save the address of the function calling us for debugging purposes.
	TNStackGetCallersAddress(&(pNewSendData->m_pvSubmittersAddress),
							&pvCallersCaller);

	/*
	DPL(9, "Data %x submitted by %X, %X",
		3, pNewSendData->m_pvData, pNewSendData->m_pvSubmittersAddress, pvCallersCaller);
	*/

	hr = this->Add(pNewSendData);
	if (hr != S_OK)
	{
		DPL(0, "Failed to add send data item!", 0);
		return (hr);
	} // end if (failed to add item)

	if (! SetEvent(this->m_hSendDataEvent))
	{
		hr = GetLastError();

		if (hr == S_OK)
			hr = E_FAIL;

		DPL(0, "Couldn't set send data event %x!", 1, this->m_hSendDataEvent);

		return (hr);
	} // end if (couldn't set event)

	return (S_OK);
} // CTNSendDataQueue::AddSendData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSendDataQueue::RemoveAllItemsTo()"
//==================================================================================
// CTNSendDataQueue::RemoveAllItemsTo
//----------------------------------------------------------------------------------
//
// Description: Removes all items still in the queue that are to be sent to the
//				given address.
//
// Arguments:
//	PVOID pvAddress			Pointer to address memory to compare.
//	DWORD dwAddressSize		Size of address memory to compare.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNSendDataQueue::RemoveAllItemsTo(PVOID pvAddress, DWORD dwAddressSize)
{
	HRESULT			hr = S_OK;
	int				i;
	PTNSENDDATA		pSendData;


	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pSendData = (PTNSENDDATA) this->GetItem(i);
		if (pSendData == NULL)
		{
			DPL(0, "Failed to get send item %i!", 1, i);
			return (E_FAIL);
		} // end if (failed to get first item)		
		
		if ((pSendData->m_dwAddressSize == dwAddressSize) &&
			(memcmp(pSendData->m_pvAddress, pvAddress, dwAddressSize) == 0))
		{
			DPL(0, "WARNING: Removing item %i (%x, size %u) from send queue!",
				3, i, pvAddress, dwAddressSize);

			// Ignore error
			hr = this->Remove(i);
			if (hr != S_OK)
			{
				DPL(0, "Failed removing item %i!", 1, i);
				goto DONE;
			} // end if (failed removing item)

			// Move our index back, so we don't get out of sync with the list.
			i--;
		} // end if (they're going to the same place)
	} // end for (each item)


DONE:

	this->LeaveCritSection();

	return (hr);
} // CTNSendDataQueue::RemoveAllItemsTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

