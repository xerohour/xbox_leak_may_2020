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

#include "testfromfiles.h"






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestFromFile::CTNTestFromFile()"
//==================================================================================
// CTNTestFromFile constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTestFromFile object.  Initializes the data
//				structures.  This object will own the pointers passed in, so the
//				caller should forget about them.
//
// Arguments:
//	DWORD dwUniqueID					Unique test ID to assign to this new test.
//	PTNTESTTABLECASE pCase				Pointer to the case this will instantiate.
//	char* pszInstanceID					String ID of test instance.
//	char* pszFriendlyName				Optional friendly name for test.
//	int iNumMachines					Number of testers for this instance.
//	DWORD dwPermutationMode				Permutations of this test to run.
//	DWORD dwOptions						Options for this new test.
//	PTNSTOREDDATA pStoredData			Pointer to a stored data item.
//	PLSTRINGLIST pTesterAssignments		Pointer to list of tester assignment strings
//										for test.
//	DWORD dwRepsRemaining				Number of repetitions this test should be
//										run.
//
// Returns: None (just the object).
//==================================================================================
CTNTestFromFile::CTNTestFromFile(DWORD dwUniqueID,
								PTNTESTTABLECASE pCase,
								char* pszInstanceID,
								char* pszFriendlyName,
								int iNumMachines,
								DWORD dwPermutationMode,
								//DWORD dwOptions,
								PTNSTOREDDATA pStoredData,
								PLSTRINGLIST pTesterAssignments,
								DWORD dwRepsRemaining):
	m_dwUniqueID(dwUniqueID),
	m_pCase(pCase),
	m_pszInstanceID(pszInstanceID),
	m_pszFriendlyName(pszFriendlyName),
	m_iNumMachines(iNumMachines),
	m_dwPermutationMode(dwPermutationMode),
	//m_dwOptions(dwOptions),
	m_pStoredDataList(NULL),
	m_pTesterAssignments(pTesterAssignments),
	m_dwRepsRemaining(dwRepsRemaining)
{
	HRESULT		hr;

	
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNTestFromFile));


	// Note we're not addref-ing the case object.  We assume it will exist for our
	// entire lifespan.


#ifdef DEBUG
	if (pszInstanceID == NULL)
	{
		DPL(0, "No instance ID was specified!", 0);
		DEBUGBREAK();
	} // end if (there's an instance ID)

	switch (dwPermutationMode)
	{
		case TNTPM_ONCE:
		case TNTPM_ALLSLAVES:
		case TNTPM_ALLSLAVESASTESTERS0ANDNON0:
		case TNTPM_ALLSLAVESASALLTESTERS:
		case TNTPM_ALLPOSSIBLE:
			// Those are valid modes
		  break;

		default:
			DPL(0, "Unrecognized permutation mode %u!", 1, dwPermutationMode);
			DEBUGBREAK();
		  break;
	} // end switch (on permutation mode)
#endif // DEBUG

	if (pStoredData != NULL)
	{
		this->m_pStoredDataList = new (CTNStoredDataList);
		if (this->m_pStoredDataList != NULL)
		{
			hr = this->m_pStoredDataList->Add(pStoredData);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add stored data object to list!  %e", 1, hr);
			} // end if (couldn't add stored data object)
		} // end if (allocated object)
	} // end if (there's stored data)
} // CTNTestFromFile::CTNTestFromFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestFromFile::~CTNTestFromFile()"
//==================================================================================
// CTNTestFromFile destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTestFromFile object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTestFromFile::~CTNTestFromFile(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_pszInstanceID != NULL)
	{
		LocalFree(this->m_pszInstanceID);
		this->m_pszInstanceID = NULL;
	} // end if (allocated a sub ID)

	if (this->m_pszFriendlyName != NULL)
	{
		LocalFree(this->m_pszFriendlyName);
		this->m_pszFriendlyName = NULL;
	} // end if (allocated a friendly name)

	if (this->m_pStoredDataList != NULL)
	{
		delete (this->m_pStoredDataList);
		this->m_pStoredDataList = NULL;
	} // end if (have stored data)

	if (this->m_pTesterAssignments != NULL)
	{
		delete (this->m_pTesterAssignments);
		this->m_pTesterAssignments = NULL;
	} // end if (have tester assignments)
} // CTNTestFromFile::~CTNTestFromFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestFromFilesList::GetTest()"
//==================================================================================
// CTNTestFromFilesList::GetTest
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the CTNTestFromFile object identified by the
//				passed in unique ID if it exists, NULL otherwise.
//
// Arguments:
//	DWORD dwUniqueID	ID of test to retrieve.
//
// Returns: Pointer to object or NULL if couldn't find it.
//==================================================================================
PTNTESTFROMFILE CTNTestFromFilesList::GetTest(DWORD dwUniqueID)
{
	PTNTESTFROMFILE		pItem = NULL;
	int					i;


	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNTESTFROMFILE) this->GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't retrieve test %i!", 1, i);
			goto DONE;
		} // end if (couldn't get that item)

		// If we found the item, we're done
		if (pItem->m_dwUniqueID == dwUniqueID)
		{
			goto DONE;
		} // end if (we found the unique ID)
	} // end for (each test)

	pItem = NULL;


DONE:

	this->LeaveCritSection();

	return (pItem);
} // CTNTestFromFilesList::GetTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
