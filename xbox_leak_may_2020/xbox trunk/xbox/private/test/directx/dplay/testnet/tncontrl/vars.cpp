//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"

#include "tncontrl.h"
#include "main.h"

#include "vars.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOutputVar::CTNOutputVar()"
//==================================================================================
// CTNOutputVar constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNOutputVar object.  Initializes the data structures.
//
// Arguments:
//	char* szName		Name of this output variable.
//	char* szType		Type of the value for this output variable.
//	PVOID pvValue		Pointer to value of this output variable.
//	DWORD dwValueSize	Size of value of this output variable.
//
// Returns: None (just the object).
//==================================================================================
CTNOutputVar::CTNOutputVar(char* szName, char* szType, PVOID pvValue,
						DWORD dwValueSize):
	CLString(szName),
	m_pszType(NULL),
	m_pvValue(pvValue),
	m_dwValueSize(dwValueSize)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNOutputVar));

	this->m_pszType = (char*) LocalAlloc(LPTR, (strlen(szType) + 1));
	if (this->m_pszType != NULL)
	{
		strcpy(this->m_pszType, szType);
	} // end if (allocated memory)
} // CTNOutputVar::CTNOutputVar
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOutputVar::~CTNOutputVar()"
//==================================================================================
// CTNOutputVar destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNOutputVar object.  Frees any memory it may have
//				allocated.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNOutputVar::~CTNOutputVar(void)
{
	//DPL(0, "this = %x", 2, this, sizeof (CTNOutputVar));

	if (this->m_pszType != NULL)
	{
		LocalFree(this->m_pszType);
		this->m_pszType = NULL;
	} // end if (allocated a type string)
} // CTNOutputVar::CTNOutputVar
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOutputVarsList::CTNOutputVarsList()"
//==================================================================================
// CTNOutputVarsList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNOutputVarsList object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNOutputVarsList::CTNOutputVarsList(void):
	m_pvOutputDataStart(NULL)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNOutputVarsList));
} // CTNOutputVarsList::CTNOutputVarsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOutputVarsList::AddVariable()"
//==================================================================================
// CTNOutputVarsList::AddVariable
//----------------------------------------------------------------------------------
//
// Description: Creates a new variable with the given name (or overwrites the old
//				one if one existed for this result).  Its value is set to the
//				pointer passed in, which must point to some data within the output
//				data buffer.
//
// Arguments:
//	char* szName				Name of variable to create and set.
//	char* szType				Type of variable to create.
//	PVOID pvValueLocation		Pointer to variable's value.  This must be inside
//								the output data buffer.
//	DWORD dwValueSize			Size of variable's value.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNOutputVarsList::AddVariable(char* szName, char* szType,
										PVOID pvValueLocation,
										DWORD dwValueSize)
{
	HRESULT			hr;
	PTNOUTPUTVAR	pVar = NULL;


	if (this == NULL)
	{
		DPL(0, "Result pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)


	this->EnterCritSection();


	// If we have a variable with this name already, remove it.
	pVar = (PTNOUTPUTVAR) this->GetStringObject(szName, 0, FALSE);
	if (pVar != NULL)
	{
		hr = this->RemoveFirstReference(pVar);
		if (hr != S_OK)
		{
			DPL(0, "Removing reference to %x failed!", 1, pVar);
			goto DONE;
		} // end if (adding variable failed)

		if (pVar->m_dwRefCount == 0)
		{
			delete (pVar);
			pVar = NULL;
		} // end if (can delete item)
		else
		{
			DPL(0, "WARNING: Can't delete item %x, it's refcount is %u!",
				2, pVar, pVar->m_dwRefCount);
		} // end else (can't delete item)
	} // end if (we found a variable with this name already)


	// Create the new variable
	pVar = new (CTNOutputVar)(szName, szType, pvValueLocation, dwValueSize);
	if (pVar == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	hr = this->Add(pVar);
	if (hr != S_OK)
	{
		DPL(0, "Adding item failed!", 0);
		delete (pVar);
		pVar = NULL;
		//goto DONE;
	} // end if (adding variable failed)


DONE:

	this->LeaveCritSection();

	return (hr);
} // CTNOutputVarsList::AddVariable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOutputVarsList::GetVariable()"
//==================================================================================
// CTNOutputVarsList::GetVariable
//----------------------------------------------------------------------------------
//
// Description: Searches for a variable with the given name and type, and sets the
//				pointer and size to the variables value pointer and size.
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
HRESULT CTNOutputVarsList::GetVariable(char* szName, char* szType,
										PVOID* ppvValueLocation,
										DWORD* pdwValueSize)
{
	HRESULT			hr = S_OK;
	PTNOUTPUTVAR	pItem = NULL;


	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)

	
	this->EnterCritSection();

	pItem = (PTNOUTPUTVAR) this->GetStringObject(szName, 0, FALSE);
	if (pItem == NULL)
	{
		//DPL(0, "Couldn't get variable with name \"%s\"!", 1, szName);
		hr = ERROR_NOT_FOUND;
		goto DONE;
	} // end if (couldn't get that item)

	if (strcmp(pItem->m_pszType, szType) != 0)
	{
		/*
		DPL(0, "Found variable \"%s\", but type is wrong (\"%s\" != \"%s\")!",
			3, szName, lpItem->m_lpszType, szType);
		*/
		hr = ERROR_INVALID_DATA;
		goto DONE;
	} // end if (variable type is wrong)


	(*ppvValueLocation) = pItem->m_pvValue;
	(*pdwValueSize) = pItem->m_dwValueSize;

DONE:

	this->LeaveCritSection();

	return (hr);
} // CTNOutputVarsList::GetVariable
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOutputVarsList::GetFirstVariableOfType()"
//==================================================================================
// CTNOutputVarsList::GetFirstVariableOfType
//----------------------------------------------------------------------------------
//
// Description: Searches for the first variable of the given type, and sets the
//				pointer and size to the variables value pointer and size.
//				If no variable of that type is found, ERROR_NOT_FOUND is returned.
//				If ppszName is not NULL, then it is set to point to the name of
//				the variable found.
//
// Arguments:
//	char* szType				Type of variable to retrieve.
//	char** ppszName				Place to store pointer to name of variable.
//	PVOID* ppvValueLocation		Pointer to set to point to variable's value.
//	DWORD* pdwValueSize			Pointer to set to size of variable's value.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNOutputVarsList::GetFirstVariableOfType(char* szType, char** ppszName,
												PVOID* ppvValueLocation,
												DWORD* pdwValueSize)
{
	HRESULT			hr;
	int				i;
	PTNOUTPUTVAR	pItem = NULL;


	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (got passed invalid object)

	
	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNOUTPUTVAR) this->GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Couldn't get variable %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get that item)

		if (strcmp(pItem->m_pszType, szType) == 0)
		{
			if (ppszName != NULL)
				(*ppszName) = pItem->GetString();

			(*ppvValueLocation) = pItem->m_pvValue;
			(*pdwValueSize) = pItem->m_dwValueSize;

			hr = S_OK;
			goto DONE;
		} // end if (found matching type)
	} // end for (each item)

	//DPL(0, "Couldn't find variable of type \"%s\"!", 1, szType);
	hr = ERROR_NOT_FOUND;

DONE:

	this->LeaveCritSection();

	return (hr);
} // CTNOutputVarsList::GetFirstVariableOfType
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOutputVarsList::PackIntoBuffer()"
//==================================================================================
// CTNOutputVarsList::PackIntoBuffer
//----------------------------------------------------------------------------------
//
// Description: Flattens this list into a contiguous buffer for easy transmission.
//				If the pointer to store the results in is NULL, then no memory
//				is copied, but the buffer size is still set to the amount of memory
//				required, and ERROR_BUFFER_TOO_SMALL is returned.
//
// Arguments:
//	PVOID pvBuffer			Pointer to buffer to use.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to fill in with
//							buffer size required.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNOutputVarsList::PackIntoBuffer(PVOID pvBuffer, DWORD* pdwBufferSize)
{
	LPBYTE			lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int				i;
	PTNOUTPUTVAR	pVar = NULL;
	DWORD			dwNameSize;
	DWORD			dwTypeSize;
	DWORD		    dwValueOffset;



	(*pdwBufferSize) = 0;


	this->EnterCritSection();

	i = this->Count();

	if (i <= 0)
	{
		this->LeaveCritSection();
		return (S_OK);
	} // end if (there aren't any items in this list)


	if (pvBuffer == NULL)
	{
		(*pdwBufferSize) += sizeof (int); // number of items in the list
	} // end if (there isn't a buffer)
	else
	{
		// Copy the number of items in.
		CopyAndMoveDestPointer(lpCurrent, &i, sizeof (int))
	} // end else (there is a buffer)

	for(i = 0; i < this->Count(); i++)
	{
		pVar = (PTNOUTPUTVAR) this->GetItem(i);
		if (pVar == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get output variable %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)

		// String sizes include NULL termination.
		dwNameSize = strlen(pVar->GetString()) + 1;
		dwTypeSize = strlen(pVar->m_pszType) + 1;

		if (pvBuffer == NULL)
		{
			(*pdwBufferSize) += dwNameSize			// actual name string
								+ dwTypeSize		// actual type string
								+ sizeof (DWORD)	// dwValueOffset
								+ sizeof (DWORD);	// dwValueSize
		} // end if (don't have a buffer)
		else
		{
			dwValueOffset = (DWORD)(((LPBYTE) pVar->m_pvValue) - ((LPBYTE) this->m_pvOutputDataStart));

			CopyAndMoveDestPointer(lpCurrent, pVar->GetString(), dwNameSize);
			CopyAndMoveDestPointer(lpCurrent, pVar->m_pszType, dwTypeSize);
			CopyAndMoveDestPointer(lpCurrent, &(dwValueOffset), sizeof (DWORD));
			CopyAndMoveDestPointer(lpCurrent, &(pVar->m_dwValueSize), sizeof (DWORD));
		} // end else (there's a buffer)
	} // end for (each variable)

	this->LeaveCritSection();


	// If the caller was just retrieving the size, return BUFFER_TOO_SMALL
	if (pvBuffer == NULL)
	{
		return (ERROR_BUFFER_TOO_SMALL);
	} // end if (just retrieving size)

	return (S_OK);
} // CTNOutputVarsList::PackIntoBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOutputVarsList::UnpackFromBuffer()"
//==================================================================================
// CTNOutputVarsList::UnpackFromBuffer
//----------------------------------------------------------------------------------
//
// Description: Populates this list with the contents previously stored in a
//				contiguous buffer.
//
// Arguments:
//	PVOID pvBuffer		Pointer to buffer to use.
//	DWORD dwBufferSize	Size of buffer.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNOutputVarsList::UnpackFromBuffer(PVOID pvBuffer, DWORD dwBufferSize)
{
	HRESULT			hr;
	LPBYTE			lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int				iNumItems = 0;
	int				i;
	PTNOUTPUTVAR	pVar = NULL;
	char*			pszName;
	char*			pszType;
	DWORD			dwValueOffset;



	this->EnterCritSection();

	CopyAndMoveSrcPointer(&iNumItems, lpCurrent, sizeof (int));

	for(i = 0; i < iNumItems; i++)
	{
		pszName = (char*) lpCurrent;
		lpCurrent += strlen(pszName) + 1;
		pszType = (char*) lpCurrent;
		lpCurrent += strlen(pszType) + 1;
        //  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
		dwValueOffset = *((UNALIGNED DWORD*) lpCurrent);
		lpCurrent += sizeof (DWORD);

		pVar = new (CTNOutputVar)(pszName, pszType,
									((LPBYTE) this->m_pvOutputDataStart) + dwValueOffset,
									*((UNALIGNED DWORD*) lpCurrent));
		if (pVar == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		// Move past that last DWORD and on to the next item
		lpCurrent += sizeof (DWORD);			


		hr = this->Add(pVar);
		if (hr != S_OK)
		{
			DPL(0, "Adding variable %i failed!", 1, i);
			goto DONE;
		} // end if (couldn't add item)

		pVar = NULL; // forget about it so we don't free it below
	} // end for (each binary location list)


DONE:

	this->LeaveCritSection();

	if (pVar != NULL)
	{
		delete (pVar);
		pVar = NULL;
	} // end if (have leftover object)

	return (hr);
} // CTNOutputVarsList::UnpackFromBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

