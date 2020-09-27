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

#include "slaveinfo.h"
#include "slavelist.h"







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNSlavesList::GetSlaveByID()"
//==================================================================================
// CTNSlavesList::GetSlaveByID
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the CTNSlaveInfo object identified by the
//				passed in ID.
//
// Arguments:
//	PTNCTRLMACHINEID pID	Pointer to machine to look up.
//
// Returns: Pointer to object or NULL if couldn't find it.
//==================================================================================
PTNSLAVEINFO CTNSlavesList::GetSlaveByID(PTNCTRLMACHINEID pID)
{
	int				i;
	PTNSLAVEINFO	pItem = NULL;


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNSLAVEINFO) this->GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't retrieve tester %i!", 1, i);
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
} // CTNSlavesList::GetSlaveByID
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
