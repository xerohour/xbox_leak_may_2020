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






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMasterInfo::CTNMasterInfo()"
//==================================================================================
// CTNMasterInfo constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNMasterInfo object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNMasterInfo::CTNMasterInfo(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNMasterInfo));

	ZeroMemory(&(this->m_id), sizeof (TNCTRLMACHINEID));
	this->m_dwControlVersion = 0;
	this->m_dwSessionID = 0;
	ZeroMemory(&(this->m_moduleID), sizeof (TNMODULEID));
	this->m_dwMode = 0;
	this->m_pszSessionFilter = NULL;
	ZeroMemory(&(this->m_commdata), sizeof (COMMDATA));
	this->m_pszAddress = NULL;
	this->m_iNumSlaves = 0;
	this->m_fJoinersAllowed = TRUE;
}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMasterInfo::~CTNMasterInfo()"
//==================================================================================
// CTNMasterInfo destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNMasterInfo object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNMasterInfo::~CTNMasterInfo(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_pszSessionFilter != NULL)
	{
		LocalFree(this->m_pszSessionFilter);
		this->m_pszSessionFilter = NULL;
	} // end if (allocated a session ID)

	if (this->m_commdata.pvAddress != NULL)
	{
		LocalFree(this->m_commdata.pvAddress);
		this->m_pszAddress = NULL;
	} // end if (we have a control comm address structure)

	if (this->m_pszAddress != NULL)
	{
		LocalFree(this->m_pszAddress);
		this->m_pszAddress = NULL;
	} // end if (allocated an address string)
}
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

#endif // ! XBOX
