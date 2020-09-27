//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"

#include "tncontrl.h"
#include "main.h"

#include "syncdata.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncData::CTNSyncData()"
//==================================================================================
// CTNSyncData constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNSyncData object.  Initializes the data
//				structures.
//
// Arguments:
//	DWORD dwRequestID	Request ID that created this item.
//	char* pszSyncName	Name of sync operation this object belongs to, or NULL if
//						not important.
//	int iTesterNum		Tester number this sync represents.
//
// Returns: None (just the object).
//==================================================================================
CTNSyncData::CTNSyncData(DWORD dwRequestID, char* pszSyncName, int iTesterNum):
	m_dwRequestID(dwRequestID),
	m_pszSyncName(NULL),
	m_iTesterNum(iTesterNum),
	m_pSyncList(NULL),

	m_pvData(NULL),
	m_dwDataSize(0)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNSyncData));


	if (pszSyncName != NULL)
	{
		this->m_pszSyncName = (char*) LocalAlloc(LPTR, (strlen(pszSyncName) + 1));
		if (this->m_pszSyncName != NULL)
		{
			strcpy(this->m_pszSyncName, pszSyncName);
		} // end if (allocated memory)
	} // end if (there's a sync name to use)
} // CTNSyncData::CTNSyncData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncData::~CTNSyncData()"
//==================================================================================
// CTNSyncData destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNSyncData object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNSyncData::~CTNSyncData(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_pszSyncName != NULL)
	{
		LocalFree(this->m_pszSyncName);
		this->m_pszSyncName = NULL;
	} // end if (there's a sync name)

	if (this->m_pvData != NULL)
	{
		LocalFree(this->m_pvData);
		this->m_pvData = NULL;
	} // end if (there's sync data)

	if (this->m_pSyncList != NULL)
	{
		delete (this->m_pSyncList);
		this->m_pSyncList = NULL;
	} // end if (there's a sync list)
} // CTNSyncData::~CTNSyncData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncData::SetData()"
//==================================================================================
// CTNSyncData::SetData
//----------------------------------------------------------------------------------
//
// Description: Sets this object's data to a copy based on the pointers passed in.
//				If data already existed, it is freed to make room.  If NULL is
//				passed, no data is copied in its place.
//
// Arguments:
//	PVOID pvNewData			Pointer to data to set.
//	DWORD dwNewDataSize		Size of data to set.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNSyncData::SetData(PVOID pvNewData, DWORD dwNewDataSize)
{
	if (this->m_pvData != NULL)
		LocalFree(this->m_pvData);

	this->m_dwDataSize = 0;

	if (pvNewData != NULL)
	{
		this->m_pvData = LocalAlloc(LPTR, dwNewDataSize);
		if (this->m_pvData == NULL)
			return (E_OUTOFMEMORY);
	} // end if (there's actually data)

	this->m_dwDataSize = dwNewDataSize;

	if (pvNewData != NULL)
		CopyMemory(this->m_pvData, pvNewData, dwNewDataSize);

	return (S_OK);
} // CTNSyncData::SetData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncData::GetData()"
//==================================================================================
// CTNSyncData::GetData
//----------------------------------------------------------------------------------
//
// Description: Retrieves pointers to the data this object holds.  Note: this is
//				not a copy of the data.
//
// Arguments:
//	PVOID* ppvData			Pointer to set to point at data.
//	DWORD* pdwDataSize		Pointer to fill in with size of data.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNSyncData::GetData(PVOID* ppvData, DWORD* pdwDataSize)
{
	(*ppvData) = this->m_pvData;
	(*pdwDataSize) = this->m_dwDataSize;

	return (S_OK);
} // CTNSyncData::GetData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncData::DoSyncCheck()"
//==================================================================================
// CTNSyncData::DoSyncCheck
//----------------------------------------------------------------------------------
//
// Description: Checks if all of this item's targets are recursively in sync.
//				The passed in list gets added to each time an item has been checked,
//				so after the top-most check has completed, if the pfInSync is TRUE,
//				then the list holds all the items that are now in sync.
//
// Arguments:
//	PTNSYNCDATALIST pCheckedList	Pointer to list that will be filled with items
//									that have been checked.
//	BOOL* pfInSync					Pointer to boolean to set to TRUE if all targets
//									are in sync.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNSyncData::DoSyncCheck(PTNSYNCDATALIST pCheckedList, BOOL* pfInSync)
{
	HRESULT			hr;
	int				i;
	PTNSYNCDATA		pTarget = NULL;



	(*pfInSync) = FALSE;


	// We're checking ourselves, so add us to the list.
	hr = pCheckedList->Add(this);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add tester %i to our temporary checked list!",
			1, this->m_iTesterNum);
		return (hr);
	} // end if (couldn't add this item to our temp list)


	// Loop through each target.  If it's a placeholder, then we're not in sync so
	// stop and percolate that back through the call stack.  Otherwise, recursively
	// check if the target is in sync, unless it's been checked already.

	for(i = 0; i < this->m_pSyncList->Count(); i++)
	{
		pTarget = (PTNSYNCDATA) this->m_pSyncList->GetItem(i);
		if (pTarget == NULL)
		{
			DPL(0, "Couldn't get target at index %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)


		// If the target is a placeholder, we're done.
		if (pTarget->m_pSyncList == NULL)
			return (S_OK);


		// If the target has not been checked already, check it
		if (pCheckedList->GetFirstIndex(pTarget) < 0)
		{
			hr = pTarget->DoSyncCheck(pCheckedList, pfInSync);
			if (hr != S_OK)
			{
				DPL(0, "Checking target tester index %i failed!", 1, i);
				return (hr);
			} // end if (couldn't add this item to our temp list)

			if (! (*pfInSync))
				return (S_OK);
		} // end if (the item has not been checked)

	} // end for (each target)

	// If we got here, then all of the items are in sync.
	(*pfInSync) = TRUE;

	return (S_OK);
} // CTNSyncData::DoSyncCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncDataList::CTNSyncDataList()"
//==================================================================================
// CTNSyncDataList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNSyncDataList object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNSyncDataList::CTNSyncDataList(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNSyncDataList));
} // CTNSyncDataList::CTNSyncDataList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncDataList::~CTNSyncDataList()"
//==================================================================================
// CTNSyncDataList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNSyncDataList object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNSyncDataList::~CTNSyncDataList(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNSyncDataList::~CTNSyncDataList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncDataList::GetDataFromTester()"
//==================================================================================
// CTNSyncDataList::GetDataFromTester
//----------------------------------------------------------------------------------
//
// Description: Retrieves pointers to the data sent by the specified tester.  Note:
//				this is not a copy of the data.
//				If either of the pointers passed in are NULL, then this function
//				returns S_OK if the tester has data in the list, or E_FAIL if not.
//
// Arguments:
//	int iTesterNum			Tester from which to retrieve data.
//	PVOID* ppvData			Pointer to set to point at data.
//	DWORD* pdwDataSize		Pointer to fill in with size of data.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNSyncDataList::GetDataFromTester(int iTesterNum, PVOID* ppvData,
											DWORD* pdwDataSize)
{
	HRESULT			hr;
	int				i;
	PTNSYNCDATA		pItem = NULL;


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNSYNCDATA) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get item %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)

		if (pItem->m_iTesterNum == iTesterNum)
		{
			// If the pointers passed in are NULL, the caller just wants to know
			// if the tester already has data in the list.
			if ((ppvData == NULL) || (pdwDataSize == NULL))
			{
				this->LeaveCritSection();
				return (S_OK);
			} // end if (just trying to find out if the tester exists)

			hr = pItem->GetData(ppvData, pdwDataSize);
			this->LeaveCritSection();
			return (hr);
		} // end if (found the tester caller is looking for)
	} // end for (each data item in the list)
	this->LeaveCritSection();


	DPL(0, "Couldn't find data from tester %i!", 1, iTesterNum);

	return (E_FAIL);
} // CTNSyncDataList::GetDataFromTester
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncDataList::GetItemForTester(int)"
//==================================================================================
// Overloaded function
// CTNSyncDataList::GetItemForTester
// Integer version
//----------------------------------------------------------------------------------
//
// Description: Searches the list, and returns a pointer to the first data for the
//				specified tester for any sync operation, NULL if no more exist.
//
// Arguments:
//	int iTesterNum		Tester to search for.
//
// Returns: Pointer to data for tester if in the list, NULL otherwise.
//==================================================================================
PTNSYNCDATA CTNSyncDataList::GetItemForTester(int iTesterNum)
{
	int				i;
	PTNSYNCDATA		pItem = NULL;


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNSYNCDATA) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get item %i!", 1, i);
			return (NULL);
		} // end if (couldn't get that item)

		if (pItem->m_iTesterNum == iTesterNum)
		{
			this->LeaveCritSection();
			return (pItem);
		} // end if (found the tester caller is looking for)
	} // end for (each data item in the list)
	this->LeaveCritSection();


	// This is commented out because sometimes we get called just to find out
	// if the item exists or not.
	//DPL(0, "Couldn't find first item for tester %i!", 1, iTesterNum);

	return (NULL);
} //CTNSyncDataList::GetItemForTester(int)
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncDataList::GetItemForTester(char*, int)"
//==================================================================================
// Overloaded function
// CTNSyncDataList::GetItemForTester
// Name and integer version
//----------------------------------------------------------------------------------
//
// Description: Searches the list, and returns a pointer to the data for the passed
//				tester in the given sync operation if one was found, NULL otherwise.
//
// Arguments:
//	char* szSyncName	Name of sync operation.
//	int iTesterNum		Tester to search for.
//
// Returns: Pointer to data for tester if in the list, NULL otherwise.
//==================================================================================
PTNSYNCDATA CTNSyncDataList::GetItemForTester(char* szSyncName, int iTesterNum)
{
	int				i;
	PTNSYNCDATA		pItem = NULL;


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNSYNCDATA) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get item %i!", 1, i);
			return (NULL);
		} // end if (couldn't get that item)

		if (pItem->m_pszSyncName == NULL)
		{
			DPL(0, "Sync object %i name wasn't set up!?", 1, i);
			DEBUGBREAK();
			return (NULL);
		} // end if (the name wasn't set up)

		if ((pItem->m_iTesterNum == iTesterNum) &&
			(strcmp(pItem->m_pszSyncName, szSyncName) == 0))
		{
			this->LeaveCritSection();
			return (pItem);
		} // end if (found the tester caller is looking for)
	} // end for (each data item in the list)
	this->LeaveCritSection();


	// This is commented out because sometimes we get called just to find out
	// if the item exists or not.
	//DPL(0, "Couldn't find \"%s\" item for tester %i!", 2, szSyncName, iTesterNum);

	return (NULL);
} // CTNSyncDataList::GetItemForTester(char*, int)
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncDataList::AddAndDoSyncCheck()"
//==================================================================================
// CTNSyncDataList::AddAndDoSyncCheck
//----------------------------------------------------------------------------------
//
// Description: Adds the given tester and the specified targets to this list of
//				people trying to sync.
//				This also updates the relationships among the syncers, so that any
//				testers that become in sync as a result of this addition will be
//				accumulated in the passed in list (which should start as empty).
//				The boolean at pfInSync will be set to TRUE on function exit if
//				everyone in the affected list is in sync and should be notified.
//
// Arguments:
//	DWORD dwRequestID					ID used to identify the request generating
//										this object.
//	char* szSyncName					Unique string name of sync operation.
//	int iTesterNum						Tester who is now trying to sync.
//	int iNumTargets						Number of targets in the following array.
//	int* aiTargetList					Array of targets that the tester is trying to
//										sync with.
//	PVOID pvData						Optional pointer to data to send to targets.
//	DWORD dwDataSize					Size of data to send to targets.
//	PTNSYNCDATALIST pAffectedList		Pointer to list that accumulates all the
//										related testers.
//	BOOL* pfInSync						Pointer to boolean that will be set to TRUE
//										if all items in the affected list are in
//										sync, FALSE otherwise.
//	
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNSyncDataList::AddAndDoSyncCheck(DWORD dwRequestID, char* szSyncName,
											int iTesterNum, int iNumTargets,
											int* aiTargetList, PVOID pvData,
											DWORD dwDataSize,
											PTNSYNCDATALIST pAffectedList,
											BOOL* pfInSync)
{
	HRESULT			hr = S_OK;
	PTNSYNCDATA		pNewItem = NULL;
	int				i;
	PTNSYNCDATA		pTarget = NULL;



	this->EnterCritSection();


	(*pfInSync) = FALSE;

	if (iTesterNum < 0)
	{
		DPL(0, "Tester %i is not valid!", 1, iTesterNum);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (the previous tester wasn't a place holder)

	
	pNewItem = this->GetItemForTester(szSyncName, iTesterNum);
	if (pNewItem != NULL)
	{
		// If there's no sync list for this existing object, then it's just a
		// placeholder and it's okay.  But if it does exist, then something's wacky.

		if (pNewItem->m_pSyncList != NULL)
		{
			DPL(0, "Tester %i already exists in the \"%s\" list and it's not a placeholder!",
				2, iTesterNum, szSyncName);
			hr = ERROR_ALREADY_EXISTS;
			goto DONE;
		} // end if (the previous tester wasn't a place holder)

		DPL(0, "Converting sync \"%s\" placeholder for tester %i into real object.",
			2, szSyncName, iTesterNum);

		pNewItem->m_dwRequestID = dwRequestID;
	} // end if (the tester already was in the list)
	else
	{
		// It didn't already exist, so we can make a new one.

		pNewItem = new (CTNSyncData)(dwRequestID, szSyncName, iTesterNum);
		if (pNewItem == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		hr = this->Add(pNewItem);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add the item to the list!", 0);
			delete (pNewItem);
			pNewItem = NULL;
			goto DONE;
		} // end if (adding data item failed)
	} // end else (it didn't already exist)


	// Start our sync list
	pNewItem->m_pSyncList = new (CTNSyncDataList);
	if (pNewItem->m_pSyncList == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)


	// Well of course the item we're adding is affected...
	hr = pAffectedList->Add(pNewItem);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add the item to the affected list!", 0);
		goto DONE;
	} // end if (adding item failed)


	// Assume we're in sync for starters
	(*pfInSync) = TRUE;

	// Loop through each target, and add a pointer to it (or at least a pointer to a
	// placeholder for it) to our target list.

	for(i = 0; i < iNumTargets; i++)
	{
        //  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
		if (((UNALIGNED int*)aiTargetList)[i] == iTesterNum)
		{
			DPL(0, "\"%s\" tester %i is trying to sync with himself!",
				2, szSyncName, iTesterNum);
			hr = ERROR_INVALID_DATA;
			goto DONE;
		} // end if (the guy is trying to sync with himself)

		pTarget = this->GetItemForTester(szSyncName, ((UNALIGNED int*)aiTargetList)[i]);
		if (pTarget == NULL)
		{
			// We'll have to create a placeholder.  This means we're not in sync.
			(*pfInSync) = FALSE;

			DPL(0, "\"%s\" target tester number %i doesn't exist.  Adding placeholder.",
				2, szSyncName, ((UNALIGNED int*)aiTargetList)[i]);
			

			// Note we create a place holder in this main list here and put a
			// reference to it in the syncer's target list below.
			
			pTarget = new (CTNSyncData)(0, szSyncName, ((UNALIGNED int*)aiTargetList)[i]);
			if (pTarget == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate object)

			hr = this->Add(pTarget);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add the target item to the list!", 0);
				delete (pTarget);
				pTarget = NULL;
				goto DONE;
			} // end if (adding data item failed)

		} // end if (the tester wasn't in the list)
		else
		{
			// The target already existed, check to see if it's a placeholder.
			// If it is a placeholder, we're not in sync.  If it's not, and we're
			// in sync so far, and we haven't checked it yet, then do the
			// recursive sync check.

			if (pTarget->m_pSyncList == NULL)
				(*pfInSync) = FALSE;
			else if ((*pfInSync))
			{
				if (pAffectedList->GetFirstIndex(pTarget) < 0)
				{
					hr = pTarget->DoSyncCheck(pAffectedList, pfInSync);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't perform sync check for target tester %i!",
							1, ((UNALIGNED int*)aiTargetList)[i]);
						goto DONE;
					} // end if (couldn't add placeholder)
				} // end if (we haven't checked this target yet)

			} // end else if (not a placeholder and still in sync so far)
		} // end else (the tester already existed)


		// Now that we have a pointer to the target, or at the very least a pointer
		// to his placeholder, we can add it to this syncer's list of targets.  See
		// the above notes.
		
		hr = pNewItem->m_pSyncList->Add(pTarget);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add target tester %i to \"%s\" list!", 2, ((UNALIGNED int*)aiTargetList)[i],
				szSyncName);
			goto DONE;
		} // end if (adding target failed)

	} // end for (each target)


	hr = pNewItem->SetData(pvData, dwDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't set tester %i's data!", 1, iTesterNum);
		goto DONE;
	} // end if (setting item's data failed)



DONE:

	this->LeaveCritSection();

	return (hr);
} // CTNSyncDataList::AddAndDoSyncCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncDataList::AddDataFromTester()"
//==================================================================================
// CTNSyncDataList::AddDataFromTester
//----------------------------------------------------------------------------------
//
// Description: Sets this object's data to a copy based on the pointers passed in.
//				If data already existed, it is freed to make room.
//
// Arguments:
//	int iTesterNum			Tester whose data we're adding.
//	PVOID pvNewData			Pointer to data to set.
//	DWORD dwNewDataSize		Size of data to set.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNSyncDataList::AddDataFromTester(int iTesterNum, 	PVOID pvNewData,
											DWORD dwNewDataSize)
{
	HRESULT			hr = S_OK;
	PTNSYNCDATA		pItem = NULL;


	if ((pvNewData == NULL) || (dwNewDataSize == 0))
	{
		DPL(0, "Attempting to add NULL data!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (the tester already has data in the list)

	this->EnterCritSection();

	// I removed this check because we now put things on the front of the list
	// (like a stack), so a pointer to the old data will still be valid, but any
	// time you try to request data from this tester you'll get the most recent
	// one.
#if 0
	if (this->GetDataFromTester(iTesterNum, NULL, NULL) == S_OK)
	{
		DPL(0, "Tester %i already has data in the list!",
			1, iTesterNum);
		hr = ERROR_ALREADY_EXISTS;
		goto DONE;
	} // end if (the tester already has data in the list)
#endif


	pItem = new (CTNSyncData)(0, NULL, iTesterNum);
	if (pItem == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	hr = pItem->SetData(pvNewData, dwNewDataSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't set the item's data!", 0);
		goto DONE;
	} // end if (setting data failed)


	// See above note
#if 0
	hr = this->Add(pItem);
#else
	hr = this->AddAfter(pItem, NULL); // put it at the front of the list
#endif

	if (hr != S_OK)
	{
		DPL(0, "Couldn't add the item to the list!", 0);
		goto DONE;
	} // end if (adding data item failed)

	pItem = NULL; // forget about it so we don't free it below


DONE:

	this->LeaveCritSection();

	if (pItem != NULL)
		delete (pItem);

	return (hr);
} // CTNSyncDataList::AddDataFromTester
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncDataList::PackDataForTesterIntoBuffer()"
//==================================================================================
// CTNSyncDataList::PackDataForTesterIntoBuffer
//----------------------------------------------------------------------------------
//
// Description: Flattens all the data in this list for the specified tester into a
//				contiguous buffer for easy transmission.
//				If the pointer to store the results in is NULL, then no memory
//				is copied, but the buffer size is still set to the amount of memory
//				required.
//				If the tester number to retrieve is -1, all items are stuffed.
//
// Arguments:
//	int iTesterNum			Tester for which to retrieve data, or -1 for all.
//	PVOID pvBuffer			Pointer to buffer to use.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to fill in with
//							buffer size required.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNSyncDataList::PackDataForTesterIntoBuffer(int iTesterNum,
													PVOID pvBuffer,
													DWORD* pdwBufferSize)
{
	HRESULT			hr;
	LPBYTE			lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int				i;
	PTNSYNCDATA		pItem = NULL;
	PVOID			pvItemData = NULL;
	DWORD			dwItemDataSize = 0;



	(*pdwBufferSize) = 0;

	if (this->Count() <= 0)
	{
		return (S_OK);
	} // end if (there aren't any items in this list)

	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNSYNCDATA) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get item %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)


		// If it's the specified tester, he's obviously not going to be sending
		// data to himself, so skip it.
		if (pItem->m_iTesterNum == iTesterNum)
		{
			continue;
		} // end if (tester number matters and this isn't it)

		hr = pItem->GetData(&pvItemData, &dwItemDataSize);
		if (hr != S_OK)
		{
			this->LeaveCritSection();
			DPL(0, "Get item %i's data failed!", 1, i);
			return (hr);
		} // end if (couldn't get that item)

		// If there is no data, then skip it.
		if ((pvItemData == NULL) || (dwItemDataSize <= 0))
			continue;

		// If we haven't added any items yet, we need to make room for the header
		// that says how many items we have.
		if ((*pdwBufferSize) == 0)
		{
			(*pdwBufferSize) = sizeof (int); // number of items in the list

			// If there's a buffer, intialize the header.
			if (pvBuffer != NULL)
			{
				*((int*) pvBuffer) = 0;
				lpCurrent += sizeof (int); // move the pointer
			} // end if (there's a buffer to copy to)
		} // end if (this is the first item)

		(*pdwBufferSize) += sizeof (int)		// tester number
							+ sizeof (DWORD)	// size of item data
							+ dwItemDataSize;	// item data

		if (pvBuffer != NULL)
		{
			// Adjust the header
			(*((int*) pvBuffer))++;

			// Copy the item to the current location
			CopyAndMoveDestPointer(lpCurrent, &(pItem->m_iTesterNum), sizeof (int))
			CopyAndMoveDestPointer(lpCurrent, &dwItemDataSize, sizeof (DWORD))
			CopyAndMoveDestPointer(lpCurrent, pvItemData, dwItemDataSize)
		} // end if (there's a buffer to copy to)
	} // end for (each data item in the list)
	this->LeaveCritSection();

	if (pvBuffer == NULL)
		return (ERROR_BUFFER_TOO_SMALL);

	return (S_OK);
} // CTNSyncDataList::PackDataForTesterIntoBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSyncDataList::UnpackFromBuffer()"
//==================================================================================
// CTNSyncDataList::UnpackFromBuffer
//----------------------------------------------------------------------------------
//
// Description: Populates this list with the contents previously stored in a
//				contiguous buffer.
//
// Arguments:
//	PVOID pvBuffer			Pointer to buffer to use.
//	DWORD dwBufferSize		Size of buffer.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNSyncDataList::UnpackFromBuffer(PVOID pvBuffer, DWORD dwBufferSize)
{
	HRESULT				hr;
	LPBYTE				lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int					iNumItems = 0;
	int					i;
	int					iTesterNum = -1;
	DWORD				dwDataSize = 0;


	this->EnterCritSection();

	CopyAndMoveSrcPointer(&iNumItems, lpCurrent, sizeof (int));

	for(i = 0; i < iNumItems; i++)
	{
		CopyAndMoveSrcPointer(&iTesterNum, lpCurrent, sizeof (int));
		CopyAndMoveSrcPointer(&dwDataSize, lpCurrent, sizeof (DWORD));

		hr = this->AddDataFromTester(iTesterNum, lpCurrent, dwDataSize);
		if (hr != S_OK)
		{
			this->LeaveCritSection();
			DPL(0, "Adding data item %i (from tester %i) failed!",
				2, i, iTesterNum);
			return (hr);
		} // end if (couldn't get that item)

		lpCurrent += dwDataSize; // we have to manually move the pointer
	} // end for (each data item in the list)
	this->LeaveCritSection();

	return (S_OK);
} // CTNSyncDataList::UnpackFromBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

