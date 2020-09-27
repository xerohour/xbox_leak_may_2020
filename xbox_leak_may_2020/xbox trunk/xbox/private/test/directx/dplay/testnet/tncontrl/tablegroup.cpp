//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\strutils.h"

#include "tncontrl.h"
#include "main.h"

#include "tablegroup.h"




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::CTNTestTableGroup()"
//==================================================================================
// CTNTestTableGroup constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTestTableGroup object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNTestTableGroup::CTNTestTableGroup(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNTestTableGroup));

	this->m_fGroup = TRUE;
} // CTNTestTableGroup::CTNTestTableGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::~CTNTestTableGroup()"
//==================================================================================
// CTNTestTableGroup destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTestTableGroup object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTestTableGroup::~CTNTestTableGroup(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNTestTableGroup::~CTNTestTableGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::CalculateChecksum()"
//==================================================================================
// CTNTestTableGroup::CalculateChecksum
//----------------------------------------------------------------------------------
//
// Description: Calculates the checksum for this group based on all the test cases
//				and sub groups it contains.
//
// Arguments:
//	LPCHECKSUM lpChecksum	Place to store checksum generated.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestTableGroup::CalculateChecksum(LPCHECKSUM lpChecksum)
{
	HRESULT				hr = S_OK;
	PTNTESTTABLEITEM	pItem = NULL;
	int					i;
	CHECKSUM			subChecksum;



	this->m_items.EnterCritSection();


	// Start checksum with the number of sub items, our name and ID.

	(*lpChecksum) = this->m_items.Count();

	if (this->m_pszName != NULL)
		(*lpChecksum) += GetChecksum(this->m_pszName, strlen(this->m_pszName));

	if (this->m_pszName != NULL)
		(*lpChecksum) += GetChecksum(this->m_pszID, strlen(this->m_pszID));


	// Loop through all the subitems and grab their checksum too.
	for(i = 0; i < this->m_items.Count(); i++)
	{
		pItem = (PTNTESTTABLEITEM) this->m_items.GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't get item %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get that item)

		hr = pItem->CalculateChecksum(&subChecksum);
		if (hr != S_OK)
		{
			DPL(0, "Calculating subitem %i (%x)'s checksum failed!",
				2, i, pItem);
			goto DONE;
		} // end if (calculating subgroup checksum failed)

		(*lpChecksum) += subChecksum;
	} // end for (each item in this group)


DONE:

	this->m_items.LeaveCritSection();

	return (hr);
} // CTNTestTableGroup::CalculateChecksum
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::GetNumberOfCases()"
//==================================================================================
// CTNTestTableGroup::GetNumberOfCases
//----------------------------------------------------------------------------------
//
// Description: Returns the number of test cases contained in this group.  The
//				hierarchy is treated as flat.
//
// Arguments:
//	int* piTotal	Pointer to place to store total.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestTableGroup::GetNumberOfCases(int* piTotal)
{
	HRESULT				hr;
	int					i;
	PTNTESTTABLEITEM	pItem = NULL;


	for(i = 0; i < this->m_items.Count(); i++)
	{
		pItem = (PTNTESTTABLEITEM) this->m_items.GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't retrieve item %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)

		if (pItem->m_fGroup)
		{
			hr = ((PTNTESTTABLEGROUP) pItem)->GetNumberOfCases(piTotal);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't get number of cases from group indexed %i!",
					1, i);
				return (hr);
			} // end if (an error occurred)
		} // end if (the item is a group)
		else
		{
			// Don't report the built-in test cases

			if (! (((PTNTESTTABLECASE) pItem)->m_dwOptionFlags & TNTCO_BUILTIN))
				(*piTotal)++;
		} // end else (the item is a case)
	} // end for (each item in the testtable)

	return (S_OK);
} // CTNTestTableGroup::GetNumberOfCases
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::GetCaseIndex()"
//==================================================================================
// CTNTestTableGroup::GetCaseIndex
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the case at the given index, searching all sub
//				groups, treating the hierarchy as flat.
//
// Arguments:
//	int* piNumItemsLeft			Pointer to item to use in determining when we've
//								reached the item.
//	PTNTESTTABLECASE* ppCase	Pointer to place to store result if found.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestTableGroup::GetCaseIndex(int* piNumItemsLeft, PTNTESTTABLECASE* ppCase)
{
	HRESULT				hr;
	int					i;
	PTNTESTTABLEITEM	pItem = NULL;


	for(i = 0; i < this->m_items.Count(); i++)
	{
		pItem = (PTNTESTTABLEITEM) this->m_items.GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't retrieve item %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)

		if (pItem->m_fGroup)
		{
			hr = ((PTNTESTTABLEGROUP) pItem)->GetCaseIndex(piNumItemsLeft,
															ppCase);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't get case from group indexed %i!", 1, i);
				return (hr);
			} // end if (an error occurred)

			// If the subgroup found it, then we're done too
			if ((*ppCase) != NULL)
				return (S_OK);
		} // end if (the item is a group)
		else
		{
			// Don't return built-in cases.
			if (! (((PTNTESTTABLECASE) pItem)->m_dwOptionFlags & TNTCO_BUILTIN))
			{
				if ((*piNumItemsLeft) == 0)
				{
					(*ppCase) = (PTNTESTTABLECASE) pItem;

					return (S_OK);
				} // end if (this is the item we want)

				// Otherwise, move on
				(*piNumItemsLeft)--;
			} // end if (it's not a built-in test case)
		} // end else (the item is a case)
	} // end for (each item in the testtable)


	// We didn't find it in this group, but that's not a fatal error.
	//(*ppCase) = NULL;

	return (S_OK);
} // CTNTestTableGroup::GetCaseIndex
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::NewGroup()"
//==================================================================================
// CTNTestTableGroup::NewGroup
//----------------------------------------------------------------------------------
//
// Description: Creates a new grouping with the given ID & name, and adds it to the
//				end of the list.
//				The strings must be constant (must point to valid memory for life
//				of object).
//
// Arguments:
//	char* pszID		Pointer to constant string ID of the group.
//	char* pszName	Pointer to constant string name of the group.
//
// Returns: Pointer to the new group or NULL if an error occurred.
//==================================================================================
PTNTESTTABLEGROUP CTNTestTableGroup::NewGroup(char* pszID, char* pszName)
{
	HRESULT				hr;
	PTNTESTTABLEGROUP	pNewGroup = NULL;


	if ((pszID == NULL) || (strlen(pszID) <= 0))
	{
		DPL(0, "Can't pass a NULL or empty ID string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (passed in invalid param)

	if ((pszName == NULL) || (strlen(pszName) <= 0))
	{
		DPL(0, "Can't pass a NULL or empty name string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (passed in invalid param)

	pNewGroup = new (CTNTestTableGroup);
	if (pNewGroup == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)

	// Save the string pointers.
	pNewGroup->m_pszID = pszID;
	pNewGroup->m_pszName = pszName;
	
	hr = this->m_items.Add(pNewGroup);
	if (hr != S_OK)
	{
		DPL(0, "Adding group failed!", 0);
		goto ERROR_EXIT;
	} // end if (add failed)

	return (pNewGroup);


ERROR_EXIT:

	if (pNewGroup != NULL)
	{
		delete (pNewGroup);
		pNewGroup = NULL;
	} // end if (have object)

	return (NULL);
} // CTNTestTableGroup::NewGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::NewSubGroup()"
//==================================================================================
// CTNTestTableGroup::NewSubGroup
//----------------------------------------------------------------------------------
//
// Description: Creates a new subgrouping with the given ID & name, and adds it to
//				the end of the list.
//				The strings must be constant (must point to valid memory for life
//				of object).
//
// Arguments:
//	char* pszID							Pointer to constant string ID of the group.
//	char* pszName						Pointer to constant string name of the
//										group.
//	PTNTESTTABLEGROUP* ppNewSubGroup	Place to store group created.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestTableGroup::NewSubGroup(char* pszID, char* pszName,
									PTNTESTTABLEGROUP* ppNewSubGroup)
{
	HRESULT		hr;


	(*ppNewSubGroup) = NULL;

	if ((pszID == NULL) || (strlen(pszID) <= 0))
	{
		DPL(0, "Can't pass a NULL or empty ID string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (passed in invalid param)

	if ((pszName == NULL) || (strlen(pszName) <= 0))
	{
		DPL(0, "Can't pass a NULL or empty name string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto ERROR_EXIT;
	} // end if (passed in invalid param)

	// Only the built-in group is allowed to pass some non-numeric characters.
	if (strcmp(pszID, "BuiltIn") != 0)
	{
		if (StringContainsNonChar(pszID, "0123456789.", TRUE, 0) >= 0)
		{
			// It's an invalid character)
			DPL(0, "Group ID cannot contain any characters other than numbers and periods (specified \"%s\")!",
				1, pszID);
			hr = ERROR_INVALID_PARAMETER;
			goto ERROR_EXIT;
		} // end if (non-numeric or period character)
	} // end if (not built-in group)

#pragma TODO(vanceo, "Validate uniqueness and location of ID")


	(*ppNewSubGroup) = new (CTNTestTableGroup);
	if ((*ppNewSubGroup) == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto ERROR_EXIT;
	} // end if (couldn't allocate memory)

	// Save the string pointers.
	(*ppNewSubGroup)->m_pszID = pszID;
	(*ppNewSubGroup)->m_pszName = pszName;
	
	hr = this->m_items.Add(*ppNewSubGroup);
	if (hr != S_OK)
	{
		DPL(0, "Adding group failed!", 0);
		goto ERROR_EXIT;
	} // end if (add failed)

	return (S_OK);


ERROR_EXIT:

	if ((*ppNewSubGroup) != NULL)
	{
		delete (*ppNewSubGroup);
		(*ppNewSubGroup) = NULL;
	} // end if (have object)

	return (hr);
} // CTNTestTableGroup::NewSubGroup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#ifndef _XBOX // no master supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::AddTest()"
//==================================================================================
// CTNTestTableGroup::AddTest
//----------------------------------------------------------------------------------
//
// Description: Adds the passed in test ID to the end of the list.
//				The strings must be constant (must point to valid memory for life
//				of object).
//
// Arguments:
//	char* pszCaseID				Pointer to constant string ID of the test.
//	char* pszName				Pointer to constant string name of the test.
//	int iNumMachines			Number of machines required for this test.
//	DWORD dwOptionFlags			Options for this test (TNTCO_xxx).
//	PTNTESTCASEPROCS pProcs		Pointer to structure containing a list of callback
//								functions associated with the new case.
//	DWORD dwNumGraphs			Number of graph items in the following array.
//	PTNGRAPHITEM paGraphs		Array of TNGRAPHITEMs.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestTableGroup::AddTest(char* pszCaseID, char* pszName, int iNumMachines,
						DWORD dwOptionFlags, PTNTESTCASEPROCS pProcs,
						DWORD dwNumGraphs, PTNGRAPHITEM paGraphs)
{
	HRESULT				hr;
	PTNTESTTABLECASE	pNewTest = NULL;


	if (iNumMachines < 0)
	{
		DPL(0, "Can't pass a negative number for number of machines required!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (invalid number of machines)

	if ((pszCaseID == NULL) || (strlen(pszCaseID) <= 0))
	{
		DPL(0, "Can't pass a NULL or empty case ID string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (passed in invalid param)

	if ((pszName == NULL) || (strlen(pszName) <= 0))
	{
		DPL(0, "Can't pass a NULL or empty name string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (passed in invalid param)

	// Built-in test cases are allowed to pass some non-numeric characters.  We'll
	// assume the developer knows what he's doing, so we'll just skip the checking
	// completely.
	if (! (dwOptionFlags & TNTCO_BUILTIN))
	{
		if (StringContainsNonChar(pszCaseID, "0123456789.", TRUE, 0) >= 0)
		{
			// It's an invalid character)
			DPL(0, "Test ID cannot contain any characters other than numbers and periods (specified \"%s\")!",
				1, pszCaseID);
			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (non-numeric or period character)
	} // end if (not built-in test case)

	//BUGBUG these aren't checked later on yet
	if (! (dwOptionFlags & (TNTCO_API | TNTCO_STRESS | TNTCO_POKE)))
	{
		DPL(0, "Invalid test to add (\"%s\"), need to specify availability in API, stress, or poke mode!",
			1, pszCaseID);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (test passed in invalid)

	pNewTest = new (CTNTestTableCase);
	if (pNewTest == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// Save the string pointers.
	pNewTest->m_pszID = pszCaseID;
	pNewTest->m_pszName = pszName;

	pNewTest->m_iNumMachines = iNumMachines;
	pNewTest->m_dwOptionFlags = dwOptionFlags;
	pNewTest->m_pfnCanRun = pProcs->pfnCanRun;
	pNewTest->m_pfnGetInputData = pProcs->pfnGetInputData;
	pNewTest->m_pfnExecCase = pProcs->pfnExecCase;
	pNewTest->m_pfnWriteData = pProcs->pfnWriteData;
	pNewTest->m_pfnFilterSuccess = pProcs->pfnFilterSuccess;
	pNewTest->m_dwNumGraphs = dwNumGraphs;
	pNewTest->m_paGraphs = paGraphs;

	//BUGBUG Generate a checksum for this item
	//pNewTest->m_dwChecksum = ?

	//DPL(0, "Adding test at %x to list at %x", 2, pNewTest, this);
	
	hr = this->m_items.Add(pNewTest);
	if (hr != S_OK)
	{
		DPL(0, "Adding test failed!", 0);
		goto DONE;
	} // end if (add failed)

	pNewTest = NULL; // forget about it so we don't free it below


DONE:

	if (pNewTest != NULL)
	{
		delete(pNewTest);
		pNewTest = NULL;
	} // end if (allocated object)

	return (hr);
} // CTNTestTableGroup::AddTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::AddTest()"
//==================================================================================
// CTNTestTableGroup::AddTest
//----------------------------------------------------------------------------------
//
// Description: Adds the passed in test to the end of the list.
//
// Arguments:
//	PTNADDTESTDATA pAddTestData		Pointer to structure containing information on
//									test case to add.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestTableGroup::AddTest(PTNADDTESTDATA pAddTestData)
{
	HRESULT				hr;
	PTNTESTTABLECASE	pNewTest = NULL;


	if (pAddTestData->dwSize != sizeof (TNADDTESTDATA))
	{
		DPL(0, "AddTestData structure size is not correct (%u != %u)!",
			2, pAddTestData->dwSize, sizeof (TNADDTESTDATA));
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (passed in invalid param)

	if ((pAddTestData->pszCaseID == NULL) || (strlen(pAddTestData->pszCaseID) <= 0))
	{
		DPL(0, "Can't pass a NULL or empty case ID string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (passed in invalid param)

	// Built-in test cases are allowed to pass some non-numeric characters.  We'll
	// assume the control layer developer knows what he's doing, so we'll just skip
	// the checking completely.
	if (! (pAddTestData->dwOptionFlags & TNTCO_BUILTIN))
	{
		if (StringContainsNonChar(pAddTestData->pszCaseID, "0123456789.", TRUE, 0) >= 0)
		{
			// It's an invalid character)
			DPL(0, "Test ID cannot contain any characters other than numbers and periods (specified \"%s\")!",
				1, pAddTestData->pszCaseID);
			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (non-numeric or period character)
	} // end if (not built-in test case)

#pragma TODO(vanceo, "Validate uniqueness and location of case ID")

	if ((pAddTestData->pszName == NULL) || (strlen(pAddTestData->pszName) <= 0))
	{
		DPL(0, "Can't pass a NULL or empty name string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (passed in invalid param)

	if ((pAddTestData->pszDescription == NULL) || (strlen(pAddTestData->pszDescription) <= 0))
	{
		DPL(0, "Can't pass a NULL or empty description string!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (passed in invalid param)

	if (pAddTestData->pszInputDataHelp == NULL)
	{
		if (pAddTestData->pfnGetInputData != NULL)
		{
			DPL(0, "Must pass an input data help string when using a GetInputData callback!", 0);
			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (callback)
	} // end if (input data help string)
	else
	{
		if (pAddTestData->pfnGetInputData == NULL)
		{
			DPL(0, "Can't pass an input data help string when no GetInputData callback!", 0);
			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (no callback)

		if (strlen(pAddTestData->pszInputDataHelp) <= 0)
		{
			DPL(0, "Can't pass an empty input data help string!", 0);
			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (empty string)
	} // end else (no input data help string)


	if (pAddTestData->iNumMachines == 0)
	{
		DPL(0, "Can't pass 0 for number of machines required!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (passed in invalid param)

	if (! (pAddTestData->dwOptionFlags & (TNTCO_API | TNTCO_STRESS | TNTCO_POKE | TNTCO_PICKY)))
	{
		DPL(0, "Need to specify availability in API, stress, poke, or picky mode!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (passed in invalid param)

	if (! (pAddTestData->dwOptionFlags & (TNTCO_TOPLEVELTEST | TNTCO_SUBTEST)))
	{
		DPL(0, "Need to specify either TOPLEVELTEST, SUBTEST or both!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (passed in invalid param)


	if (pAddTestData->pfnExecCase == NULL)
	{
		DPL(0, "Test case must have at least an ExecCase function!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (doesn't have exec case)



	pNewTest = new (CTNTestTableCase);
	if (pNewTest == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// Save the string pointers.
	pNewTest->m_pszID = pAddTestData->pszCaseID;
	pNewTest->m_pszName = pAddTestData->pszName;
	pNewTest->m_pszDescription = pAddTestData->pszDescription;
	pNewTest->m_pszInputDataHelp = pAddTestData->pszInputDataHelp;

	pNewTest->m_iNumMachines = pAddTestData->iNumMachines;
	pNewTest->m_dwOptionFlags = pAddTestData->dwOptionFlags;

	pNewTest->m_pfnCanRun = pAddTestData->pfnCanRun;
	pNewTest->m_pfnGetInputData = pAddTestData->pfnGetInputData;
	pNewTest->m_pfnExecCase = pAddTestData->pfnExecCase;
	pNewTest->m_pfnWriteData = pAddTestData->pfnWriteData;
	pNewTest->m_pfnFilterSuccess = pAddTestData->pfnFilterSuccess;

	pNewTest->m_paGraphs = pAddTestData->paGraphs;
	pNewTest->m_dwNumGraphs = pAddTestData->dwNumGraphs;


	//DPL(9, "Adding test at %x to list at %x", 2, pNewTest, this);
	
	hr = this->m_items.Add(pNewTest);
	if (hr != S_OK)
	{
		DPL(0, "Adding test failed!", 0);
		goto DONE;
	} // end if (add failed)

	pNewTest = NULL; // forget about it so we don't free it below


DONE:

	if (pNewTest != NULL)
	{
		delete(pNewTest);
		pNewTest = NULL;
	} // end if (allocated object)

	return (hr);
} // CTNTestTableGroup::AddTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::GetTest()"
//==================================================================================
// CTNTestTableGroup::GetTest
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the CTNTestTableCase object identified by the
//				passed in module ID.
//
// Arguments:
//	char* szID		Module defined ID of test to retrieve.
//
// Returns: Pointer to object or NULL if couldn't find it.
//==================================================================================
PTNTESTTABLECASE CTNTestTableGroup::GetTest(char* szID)
{
	int					i;
	PTNTESTTABLEITEM	pItem = NULL;


	this->m_items.EnterCritSection();
	for(i = 0; i < this->m_items.Count(); i++)
	{
		pItem = (PTNTESTTABLEITEM) this->m_items.GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't retrieve table item %i!", 1, i);
			this->m_items.LeaveCritSection();
			return (NULL);
		} // end if (couldn't get that item)

		if (pItem->m_fGroup)
		{
			// Try having the group look through its members for the test
			pItem = ((PTNTESTTABLEGROUP) pItem)->GetTest(szID);
			if (pItem != NULL)
			{
				this->m_items.LeaveCritSection();
				return ((PTNTESTTABLECASE) pItem);
			} // end if (the group found the module ID)
		} // end if (this is a group)
		else
		{
			if (strcmp(pItem->m_pszID, szID) == 0)
			{
				this->m_items.LeaveCritSection();
				return ((PTNTESTTABLECASE) pItem);
			} // end if (we found the module ID)
		} // end else (this is a test case)
	} // end for (each test)

	this->m_items.LeaveCritSection();

	return (NULL);
} // CTNTestTableGroup::GetTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#ifndef _XBOX // no file printing supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTestTableGroup::PrintToFile()"
//==================================================================================
// CTNTestTableGroup::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints the contents of this group to the passed in file.
//
// Arguments:
//	HANDLE hFile		File to print to.
//	DWORD dwOptions		Options on how to print.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNTestTableGroup::PrintToFile(HANDLE hFile, DWORD dwOptions)
{
	HRESULT				hr;
	int					i;
	PTNTESTTABLEITEM	pItem = NULL;


	if (dwOptions & TNTTPO_GROUPS)
	{
		if ((this->m_pszID != NULL) && (this->m_pszName != NULL))
		{
			//Ignoring errors for these
			FileWriteString(hFile, this->m_pszID);
			FileWriteString(hFile, "\t");
			FileWriteLine(hFile, this->m_pszName);
		} // end if (there's a valid ID and name)
	} // end if (groups should be printed)

	this->m_items.EnterCritSection();
	for(i = 0; i < this->m_items.Count(); i++)
	{
		pItem = (PTNTESTTABLEITEM) this->m_items.GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't retrieve table item %i!", 1, i);
			this->m_items.LeaveCritSection();
			return (E_FAIL);
		} // end if (couldn't get that item)

		hr = pItem->PrintToFile(hFile, dwOptions);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't print table item %i to file!", 1, i);
			this->m_items.LeaveCritSection();
			return (hr);
		} // end if (couldn't get that item)
	} // end for (each test)

	this->m_items.LeaveCritSection();

	return (S_OK);
} // CTNTestTableGroup::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX