#ifndef _XBOX // ! no master supported
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

#include "masterinfo.h"
#include "masterlist.h"






/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMastersList::CTNMastersList()"
//==================================================================================
// CTNMastersList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNMastersList object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNMastersList::CTNMastersList(void)
{
	DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNMastersList));
}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMastersList::~CTNMastersList()"
//==================================================================================
// CTNMastersList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNMastersList object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNMastersList::~CTNMastersList(void)
{
	DPL(0, "this = %x", 1, this);
}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMastersList::GetMasterByID()"
//==================================================================================
// CTNMastersList::GetMasterByID
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the CTNMasterInfo object identified by the
//				passed in ID.
//
// Arguments:
//	PTNCTRLMACHINEID pID	Pointer to machine to look up.
//
// Returns: Pointer to object or NULL if couldn't find it.
//==================================================================================
PTNMASTERINFO CTNMastersList::GetMasterByID(PTNCTRLMACHINEID pID)
{
	int				i;
	PTNMASTERINFO	pItem = NULL;


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNMASTERINFO) this->GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't retrieve master %i!", 1, i);
			this->LeaveCritSection();
			return (NULL);
		} // end if (couldn't get that item)

		if (pItem->m_id.dwTime == pID->dwTime)
		{
			this->LeaveCritSection();
			return (pItem);
		} // end if (we found the tester)
	} // end for (each tester)

	this->LeaveCritSection();

	return (NULL);
}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX
