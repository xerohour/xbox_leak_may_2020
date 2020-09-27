//==================================================================================
// Includes
//==================================================================================
#include <windows.h>


#include "main.h"
#include "cppobjhelp.h"
#include "linklist.h"
#ifndef NO_TNCOMMON_DEBUG_SPEW
#include "debugprint.h"
#endif // ! NO_TNCOMMON_DEBUG_SPEW
#include "strutils.h"

#include "linkedstr.h"






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLString::CLString(none)"
//==================================================================================
// CLString overloaded constructor
// No parameters version
//----------------------------------------------------------------------------------
//
// Description: Creates the CLString object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CLString::CLString(void)
{
	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(0, "this = %x", 1, this);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/

	this->m_pszString = NULL;
} // CLString::CLString(none)
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLString::CLString(string)"
//==================================================================================
// CLString overloaded constructor
// String version
//----------------------------------------------------------------------------------
//
// Description: Creates the CLString object.  Initializes the data structures, and
//				sets the string value to the one passed in.
//
// Arguments:
//	char* pszString	String to initialize value to.
//
// Returns: None (just the object).
//==================================================================================
CLString::CLString(char* pszString):
	m_pszString(NULL)
{
	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(9, "this = %x, param = %s", 2, this, pszString);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/

	if (pszString != NULL)
	{
		this->m_pszString = (char*) HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											(strlen(pszString) + 1));
		if (this->m_pszString != NULL)
		{
			strcpy(this->m_pszString, pszString);
		} // end if (allocated memory)
	} // end if (there's a valid string to use)
} // CLString::CLString(string)
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLString::~CLString()"
//==================================================================================
// CLString destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CLString object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CLString::~CLString(void)
{
	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(9, "this = %x", 1, this);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/

	if (this->m_pszString != NULL)
	{
		HeapFree(GetProcessHeap(), 0, this->m_pszString);
		this->m_pszString = NULL;
	} // end if (we allocated a string)
} // CLString::~CLString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLString::SetString()"
//==================================================================================
// CLString::SetString
//----------------------------------------------------------------------------------
//
// Description: Sets this object's value to the passed in string, replacing any
//				existing value.
//
// Arguments:
//	char* pszString	Pointer to string to set, or NULL to clear.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CLString::SetString(char* pszString)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)
#endif // DEBUG


	// If there was a value before, dump it.
	if (this->m_pszString != NULL)
	{
		HeapFree(GetProcessHeap(), 0, this->m_pszString);
		this->m_pszString = NULL;
	} // end if (we allocated a string before)


	// If there's a new value, set it.
	if (pszString != NULL)
	{
		this->m_pszString = (char*) HeapAlloc(GetProcessHeap(),
											HEAP_ZERO_MEMORY,
											(strlen(pszString) + 1));
		if (this->m_pszString == NULL)
		{
			return (E_OUTOFMEMORY);
		} // end if (couldn't allocate memory)

		strcpy(this->m_pszString, pszString);
	} // end if (there was an item to replace it)

	return (S_OK);
} // CLString::SetString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLString::GetString()"
//==================================================================================
// CLString::GetString
//----------------------------------------------------------------------------------
//
// Description: Returns pointer to this object's value.
//
// Arguments: None.
//
// Returns: Pointer to value if it has been set, NULL otherwise.
//==================================================================================
char* CLString::GetString(void)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got a bad object)
#endif // DEBUG


	return (this->m_pszString);
} // CLString::GetString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLString::ConcatenateString()"
//==================================================================================
// CLString::ConcatenateString
//----------------------------------------------------------------------------------
//
// Description: Adds the string to the end of this item's existing value, if any.
//
// Arguments:
//	char* pszString	Pointer to string to set, or NULL to clear.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CLString::ConcatenateString(char* pszString)
{
	char*	pszOldString;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)
#endif // DEBUG


	// If there's a new value, set it.
	if (pszString != NULL)
	{
		// If there was a value before, we're going to concatenate.
		if (this->m_pszString != NULL)
		{
			pszOldString = this->m_pszString;

			this->m_pszString = (char*) HeapAlloc(GetProcessHeap(),
												HEAP_ZERO_MEMORY,
												(strlen(pszOldString) + strlen(pszString) + 1));
			if (this->m_pszString == NULL)
			{
				HeapFree(GetProcessHeap(), 0, pszOldString);
				pszOldString = NULL;
				return (E_OUTOFMEMORY);
			} // end if (couldn't allocate memory)

			strcpy(this->m_pszString, pszOldString);
			strcat(this->m_pszString, pszString);

			HeapFree(GetProcessHeap(), 0, pszOldString);
			pszOldString = NULL;
		} // end if (we allocated a string before)
		else
		{
			this->m_pszString = (char*) HeapAlloc(GetProcessHeap(),
												HEAP_ZERO_MEMORY,
												(strlen(pszString) + 1));
			if (this->m_pszString == NULL)
			{
				return (E_OUTOFMEMORY);
			} // end if (couldn't allocate memory)

			strcpy(this->m_pszString, pszString);
		} // end else (there wasn't a string before)
	} // end if (there was an item to replace it)

	return (S_OK);
} // CLString::ConcatenateString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLString::IsKey()"
//==================================================================================
// CLString::IsKey
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if this string begins with the key name specified,
//				case insensitive and ignoring whitespace, FALSE otherwise.
//
// Arguments:
//	char* szKeyName		Key to check if we match.
//
// Returns: TRUE if we matched the key, FALSE otherwise.
//==================================================================================
BOOL CLString::IsKey(char* szKeyName)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (FALSE);
	} // end if (we've got a bad object)
#endif // DEBUG


	return (StringIsKey(this->m_pszString, szKeyName));
} // CLString::IsKey
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLString::IsValueTrue()"
//==================================================================================
// CLString::IsValueTrue
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if this string's key value exists and is not all "0"'s,
//				"false" or "off", case-insensitive.  Otherwise this returns FALSE.
//
// Arguments: None.
//
// Returns: TRUE if we're anything but the textual representations of false.
//==================================================================================
BOOL CLString::IsValueTrue(void)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (FALSE);
	} // end if (we've got a bad object)
#endif // DEBUG


	return (StringIsValueTrue(this->m_pszString));
} // CLString::IsValueTrue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLString::GetKeyValue()"
//==================================================================================
// CLString::GetKeyValue
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to this string's key value if there is one, NULL
//				if not.
//
// Arguments: None.
//
// Returns: Pointer to key value, or NULL if none.
//==================================================================================
char* CLString::GetKeyValue(void)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (FALSE);
	} // end if (we've got a bad object)
#endif // DEBUG

	return (StringGetKeyValue(this->m_pszString));
} // CLString::GetKeyValue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLStringList::CLStringList()"
//==================================================================================
// CLStringList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CLStringList object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CLStringList::CLStringList(void)
{
	/-
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(0, "this = %x", 1, this);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	-/
} // CLStringList::CLStringList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLStringList::~CLStringList()"
//==================================================================================
// CLStringList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CLStringList object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CLStringList::~CLStringList(void)
{
	/-
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(0, "this = %x", 1, this);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	-/
} // CLStringList::~CLStringList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLStringList::AddString()"
//==================================================================================
// CLStringList::AddString
//----------------------------------------------------------------------------------
//
// Description: Adds the passed in string to the end of the list.
//
// Arguments:
//	char* pszString	Pointer to string to add.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CLStringList::AddString(char* pszString)
{
	HRESULT		hr;
	PLSTRING	pNewItem = NULL;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)
#endif // DEBUG


	//pNewItem = new (CLString)(pszString);
	pNewItem = new (CLString);
	if (pNewItem == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't create object)

	hr = pNewItem->SetString(pszString);
	if (hr != S_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't set new string's contents!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (couldn't add item to list)

	hr = this->Add(pNewItem);
	if (hr != S_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't add new string item to list!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (couldn't add item to list)

	pNewItem = NULL; // forget about it so it doesn't get freed below


DONE:

	if (pNewItem != NULL)
	{
		delete (pNewItem);
		pNewItem = NULL;
	} // end if (have leftover object)

	return (hr);
} // CLStringList::AddString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLStringList::GetIndexedString()"
//==================================================================================
// CLStringList::GetIndexedString
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to the string at the given index in the list, or
//				NULL if an error occurred.
//
// Arguments:
//	int iPos	Index of string to retrieve.
//
// Returns: Pointer to string at index, NULL if an error occurred.
//==================================================================================
char* CLStringList::GetIndexedString(int iPos)
{
	PLSTRING	pItem = NULL;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got a bad object)
#endif // DEBUG


	pItem = (PLSTRING) this->GetItem(iPos);
	if (pItem == NULL)
		return (NULL);

	return (pItem->GetString());
} // CLStringList::GetIndexedString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLStringList::GetStringIndex()"
//==================================================================================
// CLStringList::GetStringIndex
//----------------------------------------------------------------------------------
//
// Description: Finds the first instance of the specified string and returns its
//				index in the list.  The search is started at the specified index
//				(which is zero-based, so pass 0 to start searching from the
//				beginning).
//				If the is key not found, then -1 is returned.
//
// Arguments:
//	char* szString		String to search for.
//	int iStartPos		Index to begin search from, usually 0 to start at beginning.
//	BOOL fMatchCase		Whether case has to match or not.
//
// Returns: Index of the string, or -1 if not found or error occurred.
//==================================================================================
int CLStringList::GetStringIndex(char* szString, int iStartPos, BOOL fMatchCase)
{
	PLSTRING	pItem = NULL;
	int			i;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)

	if (szString == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Passed in NULL string to search for!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got bad parameters)
#endif // DEBUG

	this->EnterCritSection();

#ifdef DEBUG
	if ((iStartPos < 0) || (iStartPos >= this->Count()))
	{
		this->LeaveCritSection();
		return (-1);
	} // end if (the starting position is out of range)
#endif // DEBUG

	for(i = iStartPos; i < this->Count(); i++)
	{
		pItem = (PLSTRING) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't get string item %i!", 1, i);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (-1);
		} // end if (couldn't get an item)

		// If it's an empty string, skip it.
		if (pItem->GetString() == NULL)
			continue;

		if (fMatchCase)
		{
			if (strcmp(pItem->GetString(), szString) == 0)
			{
				this->LeaveCritSection();
				return (i);
			} // end if (found the right item)
		} // end if (case is important)
		else
		{
			if (StringCmpNoCase(pItem->GetString(), szString))
			{
				this->LeaveCritSection();
				return (i);
			} // end if (found the right item)
		} // end else (case doesn't matter)
	} // end for (each string in the list)
	this->LeaveCritSection();

	return (-1);
} // CLStringList::GetStringIndex
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLStringList::GetStringObject()"
//==================================================================================
// CLStringList::GetStringObject
//----------------------------------------------------------------------------------
//
// Description: Finds the first instance of the specified string and returns a
//				pointer to the object.  The search is started at the specified index
//				(which is zero-based, so pass 0 to start searching from the
//				beginning).
//				If the is key not found, then NULL is returned.
//
// Arguments:
//	char* szString		String to search for.
//	int iStartPos		Index to begin search from, usually 0 to start at beginning.
//	BOOL fMatchCase		Whether case has to match or not.
//
// Returns: Pointer to the CLString object, or NULL if not found or error occurred.
//==================================================================================
PLSTRING CLStringList::GetStringObject(char* szString, int iStartPos, BOOL fMatchCase)
{
	PLSTRING	pItem = NULL;
	int			i;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got a bad object)

	if (szString == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Passed in NULL string to search for!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got bad parameters)
#endif // DEBUG


	this->EnterCritSection();

#ifdef DEBUG
	if ((iStartPos < 0) || (iStartPos >= this->Count()))
	{
		this->LeaveCritSection();
		return (NULL);
	} // end if (the starting position is out of range)
#endif // DEBUG

	for(i = iStartPos; i < this->Count(); i++)
	{
		pItem = (PLSTRING) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't get string item %i!", 1, i);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (NULL);
		} // end if (couldn't get an item)

		// If it's an empty string, skip it.
		if (pItem->GetString() == NULL)
			continue;

		if (fMatchCase)
		{
			if (strcmp(pItem->GetString(), szString) == 0)
			{
				this->LeaveCritSection();
				return (pItem);
			} // end if (found the right item)
		} // end if (case is important)
		else
		{
			if (StringCmpNoCase(pItem->GetString(), szString))
			{
				this->LeaveCritSection();
				return (pItem);
			} // end if (found the right item)
		} // end else (case doesn't matter)
	} // end for (each string in the list)
	this->LeaveCritSection();

	return (NULL);
} // CLStringList::GetStringObject
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLStringList::GetValueOfKey()"
//==================================================================================
// CLStringList::GetValueOfKey
//----------------------------------------------------------------------------------
//
// Description: Searches all of the strings in the list for the first instance of
//				the specified key (case insensitive) and returns a pointer to its
//				value if found.  If the is key not found, then NULL is returned.
//
// Arguments:
//	char* szKeyName			The name of the key to search for.
//
// Returns: Pointer to value of key, or NULL if not found.
//==================================================================================
char* CLStringList::GetValueOfKey(char* szKeyName)
{
	PLSTRING	pItem = NULL;
	int			i;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got a bad object)
#endif // DEBUG


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PLSTRING) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();

			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't get string item %i!", 1, i);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (NULL);
		} // end if (couldn't get an item)

		if (pItem->IsKey(szKeyName))
		{
			this->LeaveCritSection();
			return (pItem->GetKeyValue());
		} // end if (found the right item)
	} // end for (each string in the list)
	this->LeaveCritSection();

	return (NULL);
} // CLStringList::GetValueOfKey
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLStringList::IsKeyTrue()"
//==================================================================================
// CLStringList::IsKeyTrue
//----------------------------------------------------------------------------------
//
// Description: Searches all of the strings in the list for the first instance of
//				the specified key (case insensitive) and returns TRUE if its value
//				is not all "0"'s, is "false" or "off", case-insensitive.  If the
//				key was not found or its value was one of the above, FALSE is
//				returned.
//
// Arguments:
//	char* szKeyName		The name of the key to search check.
//
// Returns: TRUE if the string is anything but the textual representations of FALSE.
//==================================================================================
BOOL CLStringList::IsKeyTrue(char* szKeyName)
{
	PLSTRING	pItem = NULL;
	int			i;
	BOOL		fResult = FALSE;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (FALSE);
	} // end if (we've got a bad object)
#endif // DEBUG


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PLSTRING) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();

			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't get string item %i!", 1, i);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (FALSE);
		} // end if (couldn't get an item)

		if (pItem->IsKey(szKeyName))
		{
			this->LeaveCritSection();
			return (pItem->IsValueTrue());
		} // end if (found the item)
	} // end for (each string in the list)
	this->LeaveCritSection();

	return (FALSE);
} // CLStringList::IsKeyTrue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLStringList::ContainsMatchingWildcardForString()"
//==================================================================================
// CLStringList::ContainsMatchingWildcardForString
//----------------------------------------------------------------------------------
//
// Description: Treats all of the strings in the list as pattern matches and checks
//				to see if the passed in string fits at least one of them.  Multiple
//				items in this list are treated as ORed, i.e. the first rule that
//				matches stops searching and causes TRUE to be returned.  For FALSE
//				to be returned, none of the strings can match.
//
// Arguments:
//	char* szString		Pointer to string to store resulting value in.
//	BOOL fMatchCase		Whether case is important or not.
//
// Returns: TRUE if the string matches an item in the list, FALSE otherwise.
//==================================================================================
BOOL CLStringList::ContainsMatchingWildcardForString(char* szString, BOOL fMatchCase)
{
	PLSTRING	pItem = NULL;
	int			i;
	BOOL		fResult = FALSE;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (FALSE);
	} // end if (we've got a bad object)
#endif // DEBUG


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PLSTRING) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();

			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't get string item %i!", 1, i);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			return (FALSE);
		} // end if (couldn't get an item)

		if (StringMatchesWithWildcard(pItem->GetString(), szString, fMatchCase))
		{
			this->LeaveCritSection();
			return (TRUE);
		} // end if (matched the item)
	} // end for (each string in the list)

	this->LeaveCritSection();

	return (FALSE);
} // CLStringList::ContainsMatchingWildcardForString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""

