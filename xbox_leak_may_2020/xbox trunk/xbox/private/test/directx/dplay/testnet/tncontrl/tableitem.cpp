//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"

#include "tncontrl.h"
#include "main.h"

#include "tableitem.h"






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableItem::CTNTestTableItem()"
//==================================================================================
// CTNTestTableItem constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTestTableItem object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNTestTableItem::CTNTestTableItem(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNTestTableItem));

	this->m_pszID = NULL;
	this->m_pszName = NULL;
	this->m_fGroup = FALSE;
} // CTNTestTableItem::CTNTestTableItem
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableItem::~CTNTestTableItem()"
//==================================================================================
// CTNTestTableItem destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTestTableItem object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTestTableItem::~CTNTestTableItem(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNTestTableItem::~CTNTestTableItem
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
