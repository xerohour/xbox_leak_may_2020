//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"

#include "tncontrl.h"
#include "main.h"

#include "testinst.h"
#include "testlist.h"




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestMsList::CTNTestMsList()"
//==================================================================================
// CTNTestMsList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTestMsList object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNTestMsList::CTNTestMsList(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNTestMsList));
} // CTNTestMsList::CTNTestMsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestMsList::~CTNTestMsList()"
//==================================================================================
// CTNTestMsList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTestMsList object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTestMsList::~CTNTestMsList(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNTestMsList::~CTNTestMsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestMsList::GetTopLevelTest()"
//==================================================================================
// CTNTestMsList::GetTopLevelTest
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the top level CTNTestInstance object identified
//				by the passed in unique ID.
//
// Arguments:
//	DWORD dwUniqueTestID	Control defined ID of test to retrieve.
//
// Returns: Pointer to object or NULL if couldn't find it.
//==================================================================================
PTNTESTINSTANCEM CTNTestMsList::GetTopLevelTest(DWORD dwUniqueTestID)
{
	PTNTESTINSTANCEM	pItem = NULL;
	int					i;


	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNTESTINSTANCEM) this->GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't retrieve test %i!", 1, i);
			goto DONE;
		} // end if (couldn't get that item)

		// If we found the item, we're done
		if (pItem->m_dwUniqueID == dwUniqueTestID)
		{
			goto DONE;
		} // end if (we found the unique ID)
	} // end for (each test)

	pItem = NULL;
	goto DONE;


DONE:

	this->LeaveCritSection();

	return (pItem);
} // CTNTestMsList::GetTopLevelTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestMsList::AddTestInOrder()"
//==================================================================================
// CTNTestMsList::AddTestInOrder
//----------------------------------------------------------------------------------
//
// Description: Adds the given test instance object to the list, maintaining
//				numerical unique ID order from lowest to highest.
//
// Arguments:
//	PTNTESTINSTANCEM pTest	Pointer to test to add to list.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestMsList::AddTestInOrder(PTNTESTINSTANCEM pTest)
{
	HRESULT				hr;
	int					i;
	PTNTESTINSTANCEM	pCurrentItem = NULL;


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pCurrentItem = (PTNTESTINSTANCEM) this->GetItem(i);
		if (pCurrentItem == NULL)
		{
			DPL(0, "Couldn't retrieve test %i!", 1, i);
			this->LeaveCritSection();
			return (E_FAIL);
		} // end if (couldn't get that item)

		// If this item is numerically after the one we're adding, stop and insert
		// now.
		if (pCurrentItem->m_dwUniqueID > pTest->m_dwUniqueID)
		{
			hr = this->InsertBeforeIndex(pTest, i);
			this->LeaveCritSection();
			return (hr);
		} // end if (we found the unique ID)
	} // end for (each test)

	hr = this->Add(pTest);

	this->LeaveCritSection();

	return (hr);
} // CTNTestMsList::AddTestInOrder
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestSsList::CTNTestSsList()"
//==================================================================================
// CTNTestSsList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTestSsList object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNTestSsList::CTNTestSsList(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNTestSsList));
} // CTNTestSsList::CTNTestSsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestSsList::~CTNTestSsList()"
//==================================================================================
// CTNTestSsList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTestSsList object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTestSsList::~CTNTestSsList(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNTestSsList::~CTNTestSsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestSsList::GetTopLevelTest()"
//==================================================================================
// CTNTestSsList::GetTopLevelTest
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the top level CTNTestInstance object identified
//				by the passed in unique ID.
//
// Arguments:
//	DWORD dwUniqueTestID	Control defined ID of test to retrieve.
//
// Returns: Pointer to object or NULL if couldn't find it.
//==================================================================================
PTNTESTINSTANCES CTNTestSsList::GetTopLevelTest(DWORD dwUniqueTestID)
{
	PTNTESTINSTANCES	pItem = NULL;
	int					i;


	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNTESTINSTANCES) this->GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't retrieve test %i!", 1, i);
			goto DONE;
		} // end if (couldn't get that item)

		// If we found the item, we're done
		if (pItem->m_dwUniqueID == dwUniqueTestID)
		{
			goto DONE;
		} // end if (we found the unique ID)
	} // end for (each test)

	pItem = NULL;
	goto DONE;


DONE:

	this->LeaveCritSection();

	return (pItem);
} // CTNTestSsList::GetTopLevelTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestSsList::AddTestInOrder()"
//==================================================================================
// CTNTestSsList::AddTestInOrder
//----------------------------------------------------------------------------------
//
// Description: Adds the given test instance object to the list, maintaining
//				numerical unique ID order from lowest to highest.
//
// Arguments:
//	PTNTESTINSTANCES pTest	Pointer to test to add to list.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestSsList::AddTestInOrder(PTNTESTINSTANCES pTest)
{
	HRESULT				hr;
	int					i;
	PTNTESTINSTANCES	pCurrentItem = NULL;


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pCurrentItem = (PTNTESTINSTANCES) this->GetItem(i);
		if (pCurrentItem == NULL)
		{
			DPL(0, "Couldn't retrieve test %i!", 1, i);
			this->LeaveCritSection();
			return (E_FAIL);
		} // end if (couldn't get that item)

		// If this item is numerically after the one we're adding, stop and insert
		// now.
		if (pCurrentItem->m_dwUniqueID > pTest->m_dwUniqueID)
		{
			hr = this->InsertBeforeIndex(pTest, i);
			this->LeaveCritSection();
			return (hr);
		} // end if (we found the unique ID)
	} // end for (each test)

	hr = this->Add(pTest);

	this->LeaveCritSection();

	return (hr);
} // CTNTestSsList::AddTestInOrder
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/
