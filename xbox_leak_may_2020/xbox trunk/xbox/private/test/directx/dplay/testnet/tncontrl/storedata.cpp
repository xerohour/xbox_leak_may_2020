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

#include "storedata.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNStoredData::CTNStoredData()"
//==================================================================================
// CTNStoredData constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNStoredData object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNStoredData::CTNStoredData(void):
	m_dwLocation(0),
	m_iTesterNum(-1)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNStoredData));
	} // CTNStoredData::CTNStoredData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNStoredData::~CTNStoredData()"
//==================================================================================
// CTNStoredData destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNStoredData object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNStoredData::~CTNStoredData(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNStoredData::~CTNStoredData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNStoredData::AddString()"
//==================================================================================
// CTNStoredData::AddString
//----------------------------------------------------------------------------------
//
// Description: Adds the given string to this stored data.
//
// Arguments:
//	char* szString	String to add.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNStoredData::AddString(char* szString)
{
	return (this->m_strings.AddString(szString));
} // CTNStoredData::AddString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNStoredDataList::CTNStoredDataList()"
//==================================================================================
// CTNStoredDataList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNStoredDataList object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNStoredDataList::CTNStoredDataList(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNStoredDataList));
} // CTNStoredDataList::CTNStoredDataList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNStoredDataList::~CTNStoredDataList()"
//==================================================================================
// CTNStoredDataList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNStoredDataList object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNStoredDataList::~CTNStoredDataList(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNStoredDataList::~CTNStoredDataList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNStoredDataList::GetScriptInputDataStrings()"
//==================================================================================
// CTNStoredDataList::GetScriptInputDataStrings
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the list of strings the user added under the
//				InputData section in the script that loaded this test, or NULL if
//				this test was not instantiated by a script.
//
// Arguments: None.
//
// Returns: Pointer to script string list, or NULL if doesn't exist.
//==================================================================================
PLSTRINGLIST CTNStoredDataList::GetScriptInputDataStrings(void)
{
	PTNSTOREDDATA	pStoredData = NULL;


	// Get the first stored data item
	pStoredData = (PTNSTOREDDATA) this->GetItem(0);

	// If it doesn't exist, this wasn't instantiated by a file
	if (pStoredData == NULL)
		return (NULL);

	// If the data is from a script section, then this is what we want.  Note that
	// we only look at the first item because only one script section can be
	// associated with a test (if you tried to put another section with the same
	// case ID, it would create another instance of the test).
	if (pStoredData->m_dwLocation == TNTSDL_SCRIPTSECTION)
		return (&(pStoredData->m_strings));

	return (NULL);
} // CTNStoredDataList::GetScriptInputDataStrings
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNStoredDataList::GetReportStringsForTester()"
//==================================================================================
// CTNStoredDataList::GetReportStringsForTester
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the list of strings that were written out
//				(using the module's WriteData function) for the iIndex-th report
//				that matches the report type and tester number.
//				For example, passing (TNTSDL_REPORTASSIGN, 0, 0) will return the
//				first tester 0 assignment report (if it exists).  Passing
//				(TNTSDL_REPORTFAILURE, 0, 2) will skip the first 2 tester 0
//				failure reports and return the third (if it exists).
//
// Arguments:
//	DWORD dwReportType		ID of the report type to search for (TNTSDL_REPORTxxx).
//	int iTesterNum			Tester number that generated the report.
//	int iIndex				How many matching reports to skip.
//
// Returns: Pointer to script string list, or NULL if doesn't exist.
//==================================================================================
PLSTRINGLIST CTNStoredDataList::GetReportStringsForTester(DWORD dwReportType,
														int iTesterNum,
														int iIndex)
{
	int				i;
	PTNSTOREDDATA	pStoredData = NULL;
	int				iSkipped = 0;


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		// Get the stored data item
		pStoredData = (PTNSTOREDDATA) this->GetItem(i);
		if (pStoredData == NULL)
		{
			DPL(0, "Couldn't get item %i!", 1, i);
			this->LeaveCritSection();
			return (NULL);
		} // end if (couldn't get that item)

		// Check to see if the report type and tester number match.
		if ((pStoredData->m_dwLocation == dwReportType) &&
			(pStoredData->m_iTesterNum == iTesterNum))
		{
			// Okay, we found a matching item, but does the user actually want
			// this one?
			if (iSkipped >= iIndex)
			{
				this->LeaveCritSection();
				return (&(pStoredData->m_strings));
			} // end if (shouldn't skip this one)

			// User doesn't, so skip it and look for the next match
			iSkipped++;
		} // end if (the report type and tester number match)
	} // end for (each item associated with this test)
	this->LeaveCritSection();

	return (NULL);
} // CTNStoredDataList::GetReportStringsForTester
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

