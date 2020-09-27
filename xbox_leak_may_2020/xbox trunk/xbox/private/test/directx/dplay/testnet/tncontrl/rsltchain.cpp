//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"

#include "tncontrl.h"
#include "main.h"

#include "rsltchain.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::CTNResult()"
//==================================================================================
// CTNResult constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNResult object.  Initializes the data structures.
//
// Arguments:
//	PTNTESTINSTANCES pTest		Pointer to test this is a result for.
//
// Returns: None (just the object).
//==================================================================================
CTNResult::CTNResult(PTNTESTINSTANCES pTest)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNResult));

	pTest->m_dwRefCount++;
	this->m_pTest = pTest;

	this->m_hresult = S_OK;
	this->m_fComplete = FALSE;
	this->m_fSuccess = FALSE;
	this->m_pvOutputData = NULL;
	this->m_dwOutputDataSize = 0;
	this->m_fUnderDontSaveTest = FALSE;
} // CTNResult::CTNResult
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::~CTNResult()"
//==================================================================================
// CTNResult destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNResult object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNResult::~CTNResult(void)
{
	//DPL(0, "this = %x", 1, this);


	this->m_pTest->m_dwRefCount--;
	if (this->m_pTest->m_dwRefCount == 0)
	{
		DPL(7, "Deleting test %x.", 1, this->m_pTest);
		delete (this->m_pTest);
	} // end if (last reference)
	else
	{
		DPL(7, "Not deleting test %x, its refcount is %u.",
			2, this->m_pTest, this->m_pTest->m_dwRefCount);
	} // end else (not last reference)
	this->m_pTest = NULL;

	if (this->m_pvOutputData != NULL)
	{
		LocalFree(this->m_pvOutputData);
		this->m_pvOutputData = NULL;
	} // end if (allocated output data)
} // CTNResult::~CTNResult
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::SetResultCodeAndBools()"
//==================================================================================
// CTNResult::SetResultCodeAndBools
//----------------------------------------------------------------------------------
//
// Description: Sets this object's values.
//
// Arguments:
//	HRESULT hresult			Success or error code.
//	BOOL fTestComplete		Does this result mean that no more execution will be
//							performed on the test (either because it completed or it
//							encountered a fatal error)?
//	BOOL fSuccess			Is this result a success (note this does not
//							necessarily mean hresult is 0)?
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNResult::SetResultCodeAndBools(HRESULT hresult, BOOL fTestComplete,
										BOOL fSuccess)
{
	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)

	this->m_hresult = hresult;
	this->m_fComplete = fTestComplete;
	this->m_fSuccess = fSuccess;

	return (S_OK);
} // CTNResult::SetResultCodeAndBools
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::CreateOutputDataBuffer()"
//==================================================================================
// CTNResult::CreateOutputDataBuffer
//----------------------------------------------------------------------------------
//
// Description: Creates an output buffer of the specified size.  If data already
//				existed, it is freed to make room, and all output variables are
//				destroyed.  If 0 is passed in, no buffer is allocated.
//
// Arguments:
//	DWORD dwBufferSize	Size of buffer to create.
//
// Returns: Pointer to buffer, or NULL if an error occurred.
//==================================================================================
PVOID CTNResult::CreateOutputDataBuffer(DWORD dwBufferSize)
{
	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (NULL);
	} // end if (got passed invalid object)


	// Free anything that existed before.
	this->DestroyOutputDataBufferAndVars();


	if (dwBufferSize > 0)
	{
		this->m_pvOutputData = LocalAlloc(LPTR, dwBufferSize);
		if (this->m_pvOutputData != NULL)
		{
			this->m_dwOutputDataSize = dwBufferSize;
			this->m_vars.m_pvOutputDataStart = this->m_pvOutputData;
		} // end if (allocated memory)
	} // end if (there's a new buffer to create)

	return (this->m_pvOutputData);
} // CTNResult::CreateOutputDataBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::DestroyOutputDataBufferAndVars()"
//==================================================================================
// CTNResult::DestroyOutputDataBufferAndVars
//----------------------------------------------------------------------------------
//
// Description: Frees the output buffer if it existed, and all output variables are
//				destroyed.
//
// Arguments:  None.
//
// Returns: None.
//==================================================================================
void CTNResult::DestroyOutputDataBufferAndVars(void)
{
#ifdef DEBUG
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!  DEBUGBREAK()-ing.", 0);
		DEBUGBREAK();
	} // end if (got passed invalid object)

	DPL(9, "(%x)==>", 1, this);
#endif // DEBUG

	if (this->m_pvOutputData != NULL)
	{
#ifdef DEBUG
		DPL(9, "Destroying output data %x in result object %x.",
			2, this->m_pvOutputData, this);

		hr = this->m_vars.RemoveAll();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't remove all variables!  %e  DEBUGBREAK()-ing.",
				1, hr);
			DEBUGBREAK();
		} // end if (couldn't remove all variables)
#else // ! DEBUG
		this->m_vars.RemoveAll();
#endif // ! DEBUG

		this->m_vars.m_pvOutputDataStart = NULL;

		this->m_dwOutputDataSize = 0;
		LocalFree(this->m_pvOutputData);
		this->m_pvOutputData = NULL;
	} // end if (the output data buffer already existed)


	DPL(9, "(%x)<==", 1, this);
} // CTNResult::DestroyOutputDataBufferAndVars
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::GetOutputData()"
//==================================================================================
// CTNResult::GetOutputData
//----------------------------------------------------------------------------------
//
// Description: Retrieves pointers to the output data this object holds.  Note: this
//				is not a copy of the data.
//
// Arguments:
//	PVOID* ppvData			Pointer to set to point at data.
//	DWORD* pdwDataSize		Pointer to fill in with size of data.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNResult::GetOutputData(PVOID* ppvData, DWORD* pdwDataSize)
{
	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)

	(*ppvData) = this->m_pvOutputData;
	(*pdwDataSize) = this->m_dwOutputDataSize;

	return (S_OK);
} // CTNResult::GetOutputData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::SetOutputVariable()"
//==================================================================================
// CTNResult::SetOutputVariable
//----------------------------------------------------------------------------------
//
// Description: Creates a new variable with the given name and type (or overwrites
//				the old one if one existed for this result).  Its value is set to
//				the pointer passed in, which must point to some data within the
//				output data buffer.
//
// Arguments:
//	char* szName			Name of variable to create and set.
//	char* szType			Type of variable to create.
//	PVOID pvValueLocation	Pointer to variable's value.  This must be inside the
//							output data buffer.
//	DWORD dwValueSize		Size of variable's value.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNResult::SetOutputVariable(char* szName, char* szType,
									PVOID pvValueLocation,
									DWORD dwValueSize)
{
	HRESULT		hr;


	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)

	if (this->m_pvOutputData == NULL)
	{
		DPL(0, "Output data buffer doesn't exist yet!", 0);
		return (ERROR_NOT_READY);
	} // end if (got passed invalid object)

	if (((LPBYTE) pvValueLocation < (LPBYTE) this->m_pvOutputData) ||
		((LPBYTE) pvValueLocation >= (((LPBYTE) this->m_pvOutputData) + this->m_dwOutputDataSize)))
	{
		DPL(0, "Value pointer is out of range/not point somewhere in output data buffer (%x is not between %x and %x)!",
			3, pvValueLocation, this->m_pvOutputData,
			(((LPBYTE) this->m_pvOutputData) + this->m_dwOutputDataSize));
		return (ERROR_INVALID_PARAMETER);
	} // end if (got passed invalid object)

	hr = this->m_vars.AddVariable(szName, szType, pvValueLocation, dwValueSize);
	if (hr != S_OK)
	{
		DPL(0, "Adding variable failed!", 0);
	} // end if (adding variable failed)

	return (hr);
} // CTNResult::SetOutputVariable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::GetOutputVariable()"
//==================================================================================
// CTNResult::GetOutputVariable
//----------------------------------------------------------------------------------
//
// Description: Searches for a variable with the given name and type, and sets the
//				pointer and size to the variables value pointer and size.
//				Note: this is not a copy of the data.
//				If a variable with that name and type is not found, ERROR_NOT_FOUND
//				is returned.
//
// Arguments:
//	char* szName				Name of variable to retrieve.
//	char* szType				Type of variable to retrieve.
//	PVOID* ppvValueLocation		Pointer to set to point to variable's value.
//	DWORD* pdwValueSize			Pointer to set to size of variable's value.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNResult::GetOutputVariable(char* szName, char* szType,
									PVOID* ppvValueLocation,
									DWORD* pdwValueSize)
{
	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)

	return (this->m_vars.GetVariable(szName, szType, ppvValueLocation, pdwValueSize));
} // CTNResult::GetOutputVariable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::GetOutputVariableOfType()"
//==================================================================================
// CTNResult::GetOutputVariableOfType
//----------------------------------------------------------------------------------
//
// Description: Searches for the first variable of the given type, and sets the
//				pointer and size to the variables value pointer and size.  If
//				lplpszName is not NULL, then it is set to point to the name of the
//				variable found.
//				Note: this is not a copy of the data.
//				If a variable of that type is not found, ERROR_NOT_FOUND is
//				returned.
//
// Arguments:
//	char* szType				Type of variable to retrieve.
//	char** ppszName				Optional pointer to hold name of variable found.
//	PVOID* ppvValueLocation		Pointer to set to point to variable's value.
//	DWORD* pdwValueSize			Pointer to set to size of variable's value.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNResult::GetOutputVariableOfType(char* szType, char** ppszName,
											PVOID* ppvValueLocation,
											DWORD* pdwValueSize)
{
	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)

	return (this->m_vars.GetFirstVariableOfType(szType, ppszName,
												ppvValueLocation, pdwValueSize));
} // CTNResult::GetOutputVariableOfType
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::IsCase()"
//==================================================================================
// CTNResult::IsCase
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the test case associated with this result is the
//				one specified, FALSE otherwise.
//
// Arguments:
//	char* szID		Test case ID to check.
//
// Returns: TRUE if we're that case, FALSE otherwise.
//==================================================================================
BOOL CTNResult::IsCase(char* szID)
{
	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (FALSE);
	} // end if (got passed invalid object)

	if (strcmp(this->m_pTest->m_pCase->m_pszID, szID) == 0)
		return (TRUE);

	return (FALSE);
} // CTNResult::IsCase
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::IsSuccess()"
//==================================================================================
// CTNResult::IsSuccess
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if the test case associated with this result was a
//				success, FALSE otherwise.
//
// Arguments: None.
//
// Returns: TRUE if we're a success result, FALSE otherwise.
//==================================================================================
BOOL CTNResult::IsSuccess(void)
{
	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (FALSE);
	} // end if (got passed invalid object)

	return (this->m_fSuccess);
} // CTNResult::IsSuccess
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResult::GetResultCode()"
//==================================================================================
// CTNResult::GetResultCode
//----------------------------------------------------------------------------------
//
// Description: Returns the result code of the test case associated with this.
//
// Arguments: None.
//
// Returns: Result code for this object.
//==================================================================================
HRESULT CTNResult::GetResultCode(void)
{
	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)

	return (this->m_hresult);
} // CTNResult::GetResultCode
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResultsChain::NewResult()"
//==================================================================================
// CTNResultsChain::NewResult
//----------------------------------------------------------------------------------
//
// Description: Creates a new result associated with the given test and adds it to
//				the front of the list.
//
// Arguments:
//	PTNTESTINSTANCES pTest		Pointer to test to create result for.
//
// Returns: Pointer to test result or NULL if an error occurred.
//==================================================================================
PTNRESULT CTNResultsChain::NewResult(PTNTESTINSTANCES pTest)
{
	HRESULT		hr;
	PTNRESULT	pItem = NULL;


	if (this == NULL)
	{
		DPL(0, "ResultChain pointer is NULL!", 0);
		return (NULL);
	} // end if (got passed invalid object)

	pItem = new (CTNResult)(pTest);
	if (pItem == NULL)
	{
		return (NULL);
	} // end if (couldn't allocated object)

	hr = this->AddAfter(pItem, NULL);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add the item to the front of the list!  %e", 1, hr);
		delete (pItem);
		return (NULL);
	} // end if (adding the item failed)

	return (pItem);
} // CTNResultsChain::NewResult
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResultsChain::GetResultForTest()"
//==================================================================================
// CTNResultsChain::GetResultForTest
//----------------------------------------------------------------------------------
//
// Description: Returns the result associated with the given test.
//
// Arguments:
//	PTNTESTINSTANCES pTest		Pointer to test to look for matching result.
//
// Returns: Pointer to test result or NULL if an error occurred.
//==================================================================================
PTNRESULT CTNResultsChain::GetResultForTest(PTNTESTINSTANCES pTest)
{
	int			i;
	PTNRESULT	pItem = NULL;


	if (this == NULL)
	{
		DPL(0, "ResultChain pointer is NULL!", 0);
		return (NULL);
	} // end if (got passed invalid object)

	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNRESULT) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get item %i!", 1, i);
			return (NULL);
		} // end if (couldn't get item)

		if (pItem->m_pTest == pTest)
		{
			this->LeaveCritSection();
			return (pItem);
		} // end if (found item)
	} // end for (each item)
	this->LeaveCritSection();

	// Didn't find it, so return NULL.
	return (NULL);
} // CTNResultsChain::GetResultForTest
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResultsChain::GetMostRecentResult()"
//==================================================================================
// CTNResultsChain::GetMostRecentResult
//----------------------------------------------------------------------------------
//
// Description: Returns the most recently added result object in this list that
//				is associated with the specified ID.
//
// Arguments:
//	char* szID		ID to search for.
//
// Returns: Pointer to test result or NULL if an error occurred.
//==================================================================================
PTNRESULT CTNResultsChain::GetMostRecentResult(char* szID)
{
	int			i;
	PTNRESULT	pItem = NULL;


	if (this == NULL)
	{
		DPL(0, "ResultChain pointer is NULL!", 0);
		return (NULL);
	} // end if (got passed invalid object)

	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNRESULT) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get item %i!", 1, i);
			return (NULL);
		} // end if (couldn't get that item)

		if (pItem->IsCase(szID))
		{
			this->LeaveCritSection();
			return (pItem);
		} // end if (found an ID the caller was looking for)
	} // end for (each data item in the list)
	this->LeaveCritSection();

	//DPL(0, "Couldn't find an instance of ID %s!", 1, szID);

	return (NULL);
} // CTNResultsChain::GetMostRecentResult
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResultsChain::GetMostRecentOutputVar()"
//==================================================================================
// CTNResultsChain::GetMostRecentOutputVar
//----------------------------------------------------------------------------------
//
// Description: Searches for a variable with the given name and type, and sets the
//				pointer and size to the variables value pointer and size.
//				Note: this is not a copy of the data.
//				If a variable with that name is not found, ERROR_NOT_FOUND is
//				returned.
//
// Arguments:
//	char* szName				Name of variable to retrieve.
//	char* szType				Type of variable to retrieve.
//	PVOID* ppvValueLocation		Pointer to set to point to variable's value.
//	DWORD* pdwValueSize			Pointer to set to size of variable's value.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNResultsChain::GetMostRecentOutputVar(char* szName, char* szType,
												PVOID* ppvValueLocation,
												DWORD* pdwValueSize)
{
	HRESULT		hr;
	int			i;
	PTNRESULT	pResult;


	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)


	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pResult = (PTNRESULT) this->GetItem(i);
		if (pResult == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get result %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get item)

		hr = pResult->GetOutputVariable(szName, szType, ppvValueLocation,
										pdwValueSize);
		if (hr == S_OK)
		{
			// We found it, so we're done.
			this->LeaveCritSection();
			return (S_OK);
		} // end if (couldn't get item)

		if (hr != ERROR_NOT_FOUND)
		{
			// Got some wierd error
			this->LeaveCritSection();
			DPL(0, "Failed searching for output variable under result %i!", 1, i);
			return (hr);
		} // end if (couldn't get item)

		// Well it wasn't found yet, so keep looking
	} // end for (each result)

	this->LeaveCritSection();

	return (ERROR_NOT_FOUND);
} // CTNResultsChain::GetMostRecentOutputVar
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNResultsChain::GetMostRecentOutputVarOfType()"
//==================================================================================
// CTNResultsChain::GetMostRecentOutputVarOfType
//----------------------------------------------------------------------------------
//
// Description: Searches for the first variable of the given type, and sets the
//				pointer and size to the variables value pointer and size.  If
//				lplpszName is not NULL, then it is set to point to the name of the
//				variable found.
//				Note: this is not a copy of the data.
//				If a variable of that type is not found, ERROR_NOT_FOUND is
//				returned.
//
// Arguments:
//	char* szType				Type of variable to retrieve.
//	char** ppszName				Optional pointer to hold name of variable found.
//	PVOID* ppvValueLocation		Pointer to set to point to variable's value.
//	DWORD* pdwValueSize			Pointer to set to size of variable's value.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNResultsChain::GetMostRecentOutputVarOfType(char* szType, char** ppszName,
													PVOID* ppvValueLocation,
													DWORD* pdwValueSize)
{
	HRESULT		hr;
	int			i;
	PTNRESULT	pResult;


	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)


	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pResult = (PTNRESULT) this->GetItem(i);
		if (pResult == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get result %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get item)

		hr = pResult->GetOutputVariableOfType(szType, ppszName,
											ppvValueLocation,
											pdwValueSize);
		if (hr == S_OK)
		{
			// We found it, so we're done.
			this->LeaveCritSection();
			return (S_OK);
		} // end if (couldn't get item)

		if (hr != ERROR_NOT_FOUND)
		{
			// Got some wierd error
			this->LeaveCritSection();
			DPL(0, "Failed searching for output variable of type %s under result %i!",
				2, i, szType);
			return (hr);
		} // end if (couldn't get item)

		// Well it wasn't found yet, so keep looking
	} // end for (each result)

	this->LeaveCritSection();

	return (ERROR_NOT_FOUND);
} // CTNResultsChain::GetMostRecentOutputVarOfType
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
