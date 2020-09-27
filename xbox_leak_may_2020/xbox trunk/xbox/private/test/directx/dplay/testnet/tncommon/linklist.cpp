//==================================================================================
// Includes
//==================================================================================
#include <windows.h>


#include "main.h"
#include "cppobjhelp.h"
#include "linklist.h"
#ifdef DEBUG
#ifndef NO_TNCOMMON_DEBUG_SPEW
#include "debugprint.h"
#include "stack.h"
#endif // ! NO_TNCOMMON_DEBUG_SPEW
#endif // DEBUG





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLITEM::LLITEM()"
//==================================================================================
// LLITEM constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the LLITEM object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
LLITEM::LLITEM(void)
{
/*
#ifdef DEBUG
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	LPVOID	lpvCaller;
	LPVOID	lpvCallersCaller;


	TNStackGetCallersAddress(&lpvCaller, &lpvCallersCaller);

	DPL(9, "this = %x, caller = %X, caller's caller = %X",
		3, this, lpvCaller, lpvCallersCaller);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
#endif // DEBUG
*/

	// We start off with a refcount of 0, but technically it should be 1, because
	// somebody clearly has to be holding on to this new item, as in:
	//		pItem = new (LLITEM);
	// which means someone has a reference to it.  We'll assume that the owner will
	// increment the refcount if he plans to hold on to the object and not put it
	// on a list or anything (which increments the refcount automatically).

	this->m_dwRefCount = 0;
	this->m_pPrev = NULL;
	this->m_pNext = NULL;
	this->m_fAlias = FALSE;
#ifdef DEBUG
	this->m_fDeleted = FALSE;
#endif //DEBUG
} // LLITEM::LLITEM
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLITEM::~LLITEM()"
//==================================================================================
// LLITEM destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the LLITEM object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
LLITEM::~LLITEM(void)
{
#ifdef DEBUG
	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	PVOID	pvCaller;
	PVOID	pvCallersCaller;


	TNStackGetCallersAddress(&pvCaller, &pvCallersCaller);

	DPL(9, "Deleting item %x%s  caller = %X, caller's caller = %X",
		4, this,
		(this->fAlias ? " (It's an alias)." : "."),
		pvCaller,
		pvCallersCaller);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/

	if (this->m_dwRefCount > 0)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Deleting object %x with m_dwRefCount of %u!", 2, this, this->m_dwRefCount);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (some list still refers to us)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Object %x already marked as deleted?!", 1, this);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (this item has been deleted before)

	this->m_fDeleted = TRUE;

	this->m_pPrev = NULL;
	this->m_pNext = NULL;
#endif //DEBUG
} // LLITEM::~LLITEM
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLALIAS::LLALIAS()"
//==================================================================================
// LLALIAS constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the LLALIAS object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
LLALIAS::LLALIAS(void)
{
	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(0, "this = %x", 1, this);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/

	this->m_fAlias = TRUE;

	this->m_pTarget = NULL;
} // LLALIAS::LLALIAS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLALIAS::~LLALIAS()"
//==================================================================================
// LLALIAS destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the LLALIAS object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
LLALIAS::~LLALIAS(void)
{
	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(0, "this = %x", 1, this);
	DPL(0, "Deleting alias: %x    target: %x", 2, this, this->m_pTarget);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/


	/*
#ifdef DEBUG
	if (this->m_pTarget == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Alias' target is NULL!  How did that happen?", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return;
	} // end if (our count appears to be screwed up)
#endif //DEBUG
	*/


	// We don't want to implement this I don't think.
	/*
	this->m_pTarget->m_dwRefCount--;
	if (this->m_pTarget->m_dwRefCount <= 0)
	{
		/-
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Killing aliases target.  %x", 1, this->m_pTarget);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		-/

		delete (this->m_pTarget);
	} // end if (this is the last item referring to the object)

	this->m_pTarget = NULL;
	*/
} // LLALIAS::~LLALIAS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::LLIST()"
//==================================================================================
// LLIST constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the LLIST object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
LLIST::LLIST(void)
{
	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(9, "this = %x", 1, this);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/

	InitializeCriticalSection(&(this->m_cs));

	this->m_pFirst = NULL;
	this->m_pLast = NULL;
	this->m_iCount = 0;

#ifdef DEBUG
	this->m_fDeleted = FALSE;
#endif //DEBUG
} // LLIST::LLIST
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::~LLIST()"
//==================================================================================
// LLIST destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the LLIST object and any memory it may have allocated
//				during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
LLIST::~LLIST(void)
{
	HRESULT		hr;


	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(9, "this = %x", 1, this);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/


#ifdef DEBUG
	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Object %x already marked as deleted?!", 1, this);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (this item has been deleted before)
#endif //DEBUG


	hr = this->RemoveAll();
	if (hr != S_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Removing all items failed!  %e", 1, hr);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end if (removing all items failed)


#ifdef DEBUG
	this->m_fDeleted = TRUE;
#endif //DEBUG

	
	DeleteCriticalSection(&this->m_cs);
} // LLIST::~LLIST
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::EnterCritSection()"
//==================================================================================
// LLIST::EnterCritSection
//----------------------------------------------------------------------------------
//
// Description: Enters this object's critical section.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void LLIST::EnterCritSection(void)
{

#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return;
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return;
	} // end if (using deleted object)
#endif // DEBUG


	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(0, "Entering LLIST critical section (%x).", 1, &this->cs);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/

	EnterCriticalSection(&(this->m_cs));
} // LLIST::EnterCritSection
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::LeaveCritSection()"
//==================================================================================
// LLIST::LeaveCritSection
//----------------------------------------------------------------------------------
//
// Description: Leaves this object's critical section.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void LLIST::LeaveCritSection(void)
{

#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return;
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return;
	} // end if (using deleted object)
#endif // DEBUG


	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(0, "Leaving LLIST critical section (%x).", 1, &this->cs);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/

	LeaveCriticalSection(&(this->m_cs));
} // LLIST::LeaveCritSection
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::Add()"
//==================================================================================
// LLIST::Add
//----------------------------------------------------------------------------------
//
// Description: Adds the passed item to the end of the list.  Cannot be NULL.
//
// Arguments:
//	PLLITEM pNewItem	Pointer to object to add.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT LLIST::Add(PLLITEM pNewItem)
{
	HRESULT		hr = S_OK;
	PLLALIAS	pAlias = NULL;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (using deleted object)
#endif // DEBUG

	if (pNewItem == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed a NULL pointer!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_INVALID_PARAMETER);
	} // end if (we got passed a NULL pointer)

#ifdef DEBUG
	if (pNewItem->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "About to add an item (%x) which has already been deleted!  DEBUGBREAK()-ing.",
			1, pNewItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (we got passed a NULL pointer)

	if (pNewItem->m_fAlias)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed an alias object!  How did that happen?", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_INVALID_PARAMETER);
	} // end if (we got passed an alias)

	if (pNewItem->m_dwRefCount == 0xFFFFFFFF)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "pNewItem (%x) refcount is bad!", 1, pNewItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (ERROR_BAD_ENVIRONMENT);
	}
#endif //DEBUG

	pNewItem->m_dwRefCount++; // this list will refer to the object

	// If someone else already has a reference to the object, we're going to
	// make an alias to it.
	if (pNewItem->m_dwRefCount > 1)
	{
		pAlias = new (LLALIAS);
		if (pAlias == NULL)
			return (E_OUTOFMEMORY);

		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Adding alias %x pointing to %x, refcount is %u.",
			3, pAlias, pNewItem, pNewItem->m_dwRefCount);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/

		pAlias->m_pTarget = pNewItem;
		pNewItem->m_dwRefCount++; // this alias now refers to the object
		pNewItem = pAlias;
	} // end if (this object has been referred to by other lists or aliases already)
#ifdef DEBUG
	else
	{
		if ((pNewItem->m_pPrev != NULL) || (pNewItem->m_pNext != NULL))
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Item to add (%x) had no references but it had a prev (%x) or next (%x) pointer!?",
				3, pNewItem, pNewItem->m_pPrev, pNewItem->m_pNext);

			DEBUGBREAK();
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			this->LeaveCritSection();
			return (ERROR_BAD_ENVIRONMENT);
		} // end if (new item but has list pointers)

		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Adding original item %x.", 1, pNewItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/
	} // end else (this object is new)
#endif // DEBUG

	this->EnterCritSection();

	if (this->m_iCount > 0)
	{
#ifdef DEBUG
		if ((this->m_pFirst == NULL) || (this->m_pLast == NULL))
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "List count (%i) is bad, first = %x, last = %x!  DEBUGBREAK()-ing.",
				3, this->m_iCount, this->m_pFirst, this->m_pLast);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (our count appears to be screwed up)
#endif //DEBUG

		this->m_pLast->m_pNext = pNewItem;
		pNewItem->m_pPrev = this->m_pLast;
		pNewItem->m_pNext = NULL;
		this->m_pLast = pNewItem;

		this->m_iCount++;
	} // end if (our count says we've added items already)
	else
	{
#ifdef DEBUG
		if ((this->m_pFirst != NULL) || (this->m_pLast != NULL))
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "List count of 0 is bad, first = %x, last = %x!  DEBUGBREAK()-ing.",
				2, this->m_pFirst, this->m_pLast);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (our count appears to be screwed up)
#endif //DEBUG

		this->m_pFirst = pNewItem;
		this->m_pLast = pNewItem;

		this->m_iCount = 1;
	} // end if (our count says we don't have any items)


#ifdef DEBUG
DONE:
#endif //DEBUG

	this->LeaveCritSection();

	return (hr);
} // LLIST::Add
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::AddAfter()"
//==================================================================================
// LLIST::AddAfter
//----------------------------------------------------------------------------------
//
// Description: Adds the object passed in after the item pAfterItem.  If NULL is
//				passed in as the object to AddAfter, the item is placed at the
//				beginning of the list.
//
// Arguments:
//	PLLITEM pNewItem		Pointer to object to add.
//	PLLITEM pAfterItem		Pointer to object to add after, or NULL for beginning.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT LLIST::AddAfter(PLLITEM pNewItem, PLLITEM pAfterItem)
{
	HRESULT		hr = S_OK;
	PLLALIAS	pAlias = NULL;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (using deleted object)
#endif // DEBUG


	if (pNewItem == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed a NULL object to add!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_INVALID_PARAMETER);
	} // end if (got passed nothing)


#ifdef DEBUG
	if (pNewItem->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "About to add an item (%x) which has already been deleted!  DEBUGBREAK()-ing.",
			1, pNewItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (we got passed a NULL pointer)

	if ((pAfterItem != NULL) && (pAfterItem->m_fDeleted))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "About to AddAfter an item (%x) which has already been deleted!  DEBUGBREAK()-ing.",
			1,  pAfterItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (we got passed a NULL pointer)

	if (pNewItem->m_fAlias)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed an alias object to add (%x)!  How did that happen?",
			1, pNewItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_INVALID_PARAMETER);
	} // end if (we got passed an alias)

	if ((pAfterItem != NULL) && (pAfterItem->m_fAlias))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed an alias object to add after (%x)!  How did that happen?",
			1, pAfterItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_INVALID_PARAMETER);
	} // end if (we got passed an alias)

	if (pNewItem == pAfterItem)
	{
		this->LeaveCritSection();
		return (ERROR_INVALID_PARAMETER);
	} // end if (trying to add item after itself)
#endif // DEBUG


	this->EnterCritSection();

	if (this->m_iCount <= 0) // if there aren't any items
	{
		if (pAfterItem == NULL) // not adding after anything as expected
			hr = this->Add(pNewItem); // let the normal Add's logic handle it
		else // where is the caller getting the item to add after?
			hr = ERROR_INVALID_PARAMETER;

		this->LeaveCritSection();
		return (hr);
	}

	// If we're adding at the end, that's just like calling plain old Add().
	if (pAfterItem == this->m_pLast)
	{
		hr = this->Add(pNewItem); // it's just a normal Add
		this->LeaveCritSection();
		return (hr);
	} // end if (we're adding at the end)


#ifdef DEBUG
	if (pNewItem->m_dwRefCount == 0xFFFFFFFF)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "pNewItem (%x) refcount is bad!", 1, pNewItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (bogus refcount)
#endif //DEBUG

	pNewItem->m_dwRefCount++; // this list will refer to the object

	if (pNewItem->m_dwRefCount > 1)
	{
		pAlias = new (LLALIAS);
		if (pAlias == NULL)
		{
			this->LeaveCritSection();
			return (E_OUTOFMEMORY);
		} // end if (out of memory)

		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Adding alias %x pointing to %x, refcount is %u.",
			3, pAlias, pNewItem, pNewItem->m_dwRefCount);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/

		pAlias->m_pTarget = pNewItem;
		pNewItem->m_dwRefCount++; // this alias now refers to the object
		pNewItem = pAlias;
	} // end if (this object has been referred to by other lists or aliases already)
#ifdef DEBUG
	else
	{
		if ((pNewItem->m_pPrev != NULL) || (pNewItem->m_pNext != NULL))
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Item to add (%x) had no references but it had a prev (%x) or next (%x) pointer!?",
				3, pNewItem, pNewItem->m_pPrev, pNewItem->m_pNext);

			DEBUGBREAK();
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			this->LeaveCritSection();
			return (ERROR_BAD_ENVIRONMENT);
		} // end if (new item but has list pointers)

		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Adding original item %x.", 1, pNewItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/
	} // end else (this object is new)
#endif // DEBUG


	// If we're not adding after something
	if (pAfterItem == NULL)
	{
#ifdef DEBUG
		if ((this->m_pFirst == NULL) || (this->m_pFirst->m_pPrev != NULL))
		{
			this->LeaveCritSection();
			return (ERROR_BAD_ENVIRONMENT);
		} // end if (something got screwed)
#endif //DEBUG

		pNewItem->m_pNext = this->m_pFirst;
		pNewItem->m_pPrev = NULL;

		this->m_pFirst->m_pPrev = pNewItem;
		this->m_pFirst = pNewItem;

		this->m_iCount++;

		this->LeaveCritSection();
		return (S_OK);
	} // end if (we're not adding after something)

#ifdef DEBUG
	if (pAfterItem->m_pNext == NULL)
	{
		this->LeaveCritSection();
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (something got screwed)
#endif //DEBUG

	pNewItem->m_pNext = pAfterItem->m_pNext;
	pNewItem->m_pPrev = pAfterItem;
	pAfterItem->m_pNext->m_pPrev = pNewItem;
	pAfterItem->m_pNext = pNewItem;
	this->m_iCount++;

	this->LeaveCritSection();
	return (S_OK);
} // LLIST::AddAfter
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::InsertBeforeIndex()"
//==================================================================================
// LLIST::InsertBeforeIndex
//----------------------------------------------------------------------------------
//
// Description: Adds the object passed in before the item at the given index.
//
// Arguments:
//	PLLITEM pNewItem	Pointer to object to add.
//	int iPos			Index to insert object before.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT LLIST::InsertBeforeIndex(PLLITEM pNewItem, int iPos)
{
	HRESULT		hr;
	PLLALIAS	pAlias = NULL;
	PLLITEM		pBeforeItem = NULL;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (using deleted object)
#endif // DEBUG


	this->EnterCritSection();

	// If we're putting the first item into the list
	if ((iPos == 0) && (this->m_iCount == 0))
	{
		// Let Add do the work for us
		hr = this->Add(pNewItem);
		this->LeaveCritSection();
		return (hr);
	} // end if (we're adding the first item)

#ifdef DEBUG
	if ((iPos < 0) || (iPos >= this->m_iCount))
	{
		this->LeaveCritSection();

		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Index is out of range!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_INVALID_PARAMETER);
	} // end if (index is out of range)


	if (pNewItem == NULL)
	{
		this->LeaveCritSection();

		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed a NULL object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_INVALID_PARAMETER);
	} // end if (passed nothing)

	if (pNewItem->m_fAlias)
	{
		this->LeaveCritSection();

		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed an alias object!  How did that happen?", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_INVALID_PARAMETER);
	} // end if (we got passed an alias)

	if (pNewItem->m_dwRefCount == 0xFFFFFFFF)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "pNewItem (%x) refcount is bad!", 1, pNewItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (ERROR_BAD_ENVIRONMENT);
	}
#endif //DEBUG


	pNewItem->m_dwRefCount++; // this list will refer to the object

	if (pNewItem->m_dwRefCount > 1)
	{
		pAlias = new (LLALIAS);
		if (pAlias == NULL)
		{
			this->LeaveCritSection();
			return (E_OUTOFMEMORY);
		} // end if (out of memory)

		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Adding alias: %x   pointing to: %x", 2, pAlias, pNewItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/

		pAlias->m_pTarget = pNewItem;
		pNewItem->m_dwRefCount++; // this alias now refers to the object
		pNewItem = pAlias;
	} // end if (this object has been referred to by other lists or aliases already)
	/*
	else
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Adding original item.  %x", 1, pNewItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end else (this object is new)
	*/


#ifdef DEBUG
	if (this->m_pFirst == NULL)
	{
		this->LeaveCritSection();
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (something got screwed)
#endif //DEBUG

	// Inserting at beginning of list
	if (iPos == 0)
	{
		pNewItem->m_pNext = this->m_pFirst;
		pNewItem->m_pPrev = NULL;
		this->m_pFirst->m_pPrev = pNewItem;
		this->m_pFirst = pNewItem;
		this->m_iCount++;

		this->LeaveCritSection();
		return (S_OK);
	} // end if (inserting at beginning)


	pBeforeItem = this->m_pFirst;
	while (iPos > 0)
	{
#ifdef DEBUG
		if (pBeforeItem->m_pNext == NULL)
		{
			this->LeaveCritSection();
			return (ERROR_BAD_ENVIRONMENT);
		} // end if (something got screwed)
#endif //DEBUG

		pBeforeItem = pBeforeItem->m_pNext;
		iPos--;
	} // end while (we haven't reached the index specified)

	pNewItem->m_pNext = pBeforeItem;
	pNewItem->m_pPrev = pBeforeItem->m_pPrev;
	pNewItem->m_pPrev->m_pNext = pNewItem;
	pBeforeItem->m_pPrev = pNewItem;
	this->m_iCount++;

	this->LeaveCritSection();

	return (S_OK);
} // LLIST::InsertBeforeIndex
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::GetItem()"
//==================================================================================
// LLIST::GetItem
//----------------------------------------------------------------------------------
//
// Description: Returns the pointer to the item specified, NULL if there was a
//				problem.  The index is zero based.
//
// Arguments:
//	int iPos	Position to retrieve item from.
//
// Returns: Pointer to object at index, or NULL if there was a problem.
//==================================================================================
PLLITEM LLIST::GetItem(int iPos)
{
	PLLITEM		pCurrent = NULL;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (using deleted object)
#endif // DEBUG


	this->EnterCritSection();

	if ((this->m_iCount <= 0) || (iPos < 0) || (iPos >= this->m_iCount))
	{
		this->LeaveCritSection();
		return (NULL);
	} // end if (no items or the number to get is out of range)


#ifdef DEBUG
	if (this->m_pFirst == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "List count of %i is bogus because first pointer is NULL!",
			1, this->m_iCount);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();

		this->LeaveCritSection();
		return (NULL);
	} // end if (didn't get valid object)
#endif // DEBUG


	pCurrent = this->m_pFirst;
	while (iPos > 0)
	{
#ifdef DEBUG
		if (pCurrent->m_pNext == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Object %x has no next pointer but there should be at least %i more items (%i total in list)!",
				3, pCurrent, iPos, this->m_iCount);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			this->LeaveCritSection();
			return (NULL);
		} // end if (our count got screwed)
#endif // DEBUG

		pCurrent = pCurrent->m_pNext;
		iPos--;
	} // end while (we've still got more to traverse)


	if (pCurrent->m_fAlias)
	{
#ifdef DEBUG
		if (pCurrent == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Alias points to null object!  How did that happen?  DEBUGBREAK()-ing.", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();
		} // end if (something got screwed)

		if (pCurrent->m_fDeleted)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Using alias (%x) which has already been deleted!  DEBUGBREAK()-ing.",
				1, pCurrent);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();
		} // end if (item is deleted)
#endif //DEBUG

		pCurrent = ((PLLALIAS) pCurrent)->m_pTarget;
	} // end if (it's an alias)


#ifdef DEBUG
	if (pCurrent->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "About to return item (%x) which has already been deleted!  DEBUGBREAK()-ing.",
			1, pCurrent);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (item is deleted)
#endif //DEBUG

	this->LeaveCritSection();

	return (pCurrent);
} // LLIST::GetItem
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::GetFirstIndex()"
//==================================================================================
// LLIST::GetFirstIndex
//----------------------------------------------------------------------------------
//
// Description: Returns the index of the first instance of item in the list, or -1
//				if the item was not in the list.
//
// Arguments:
//	PLLITEM pItem	Item to locate.
//
// Returns: The zero based index of the item in the list, or -1 if not found.
//==================================================================================
int LLIST::GetFirstIndex(PLLITEM pItem)
{
	PLLITEM		pCurrent = NULL;
	int			iPos = 0;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (using deleted object)
#endif // DEBUG


	this->EnterCritSection();

	if (this->m_iCount <= 0)
	{
		this->LeaveCritSection();
		return (-1);
	} // end if (no items)

#ifdef DEBUG
	if (pItem->m_fAlias)
	{
		this->LeaveCritSection();

		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed an alias!  How did that happen?", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (-1);
	} // end if (we were given an alias)
#endif //DEBUG


	pCurrent = this->m_pFirst;
	while (pCurrent != NULL)
	{
		if (pCurrent->m_fAlias)
		{
			if (((PLLALIAS) pCurrent)->m_pTarget == pItem)
				break;
		} // end if (we're looking at an alias)
		else
		{
			if (pCurrent == pItem)
				break;
		} // end if (we're not looking at an alias)

		pCurrent = pCurrent->m_pNext;
		iPos++;
	} // end while (we've still got more to traverse)

	this->LeaveCritSection();

	if (pCurrent == NULL)
	{
		return (-1);
	} // end if (we didn't find the object)

	return (iPos);
} // LLIST::GetFirstIndex
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::GetNextItem()"
//==================================================================================
// LLIST::GetNextItem
//----------------------------------------------------------------------------------
//
// Description: Returns the next item in the list after the one specified.  If NULL
//				is passed, then the first item is returned.
//				NOTE: The list lock must be held across calls to this function!
//				NOTE: This list must be composed of entirely original items (no
//					  aliases).
//
// Arguments:
//	PLLITEM pPreviousItem	Item to start from, or NULL to start at beginning.
//
// Returns: The next item in the list, or NULL if none.
//==================================================================================
PLLITEM LLIST::GetNextItem(PLLITEM pPreviousItem)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (using deleted object)

	if ((pPreviousItem != NULL) && (pPreviousItem->m_fAlias))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Using LLIST::GetNextItem requires that this list not contain any aliases!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got a bad object)
#endif // DEBUG


	if (pPreviousItem == NULL)
		return (this->m_pFirst);

	return (pPreviousItem->m_pNext);
} // LLIST::GetNextItem
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::GetPrevItem()"
//==================================================================================
// LLIST::GetPrevItem
//----------------------------------------------------------------------------------
//
// Description: Returns the item found in the list immediately preceding the one
//				specified.  If NULL is passed, then the last item is returned.
//				NOTE: The list lock must be held across calls to this function!
//				NOTE: This list must be composed of entirely original items (no
//					  aliases).
//
// Arguments:
//	PLLITEM pNextItem	Item to work back from, or NULL to start at end.
//
// Returns: The previous item in the list, or NULL if none.
//==================================================================================
PLLITEM LLIST::GetPrevItem(PLLITEM pNextItem)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got a bad object)

	if ((pNextItem != NULL) && (pNextItem->m_fAlias))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Using LLIST::GetPrevItem requires that this list not contain any aliases!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got a bad object)
#endif // DEBUG


	if (pNextItem == NULL)
		return (this->m_pLast);

	return (pNextItem->m_pPrev);
} // LLIST::GetPrevItem
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::RemoveFirstReference()"
//==================================================================================
// LLIST::RemoveFirstReference
//----------------------------------------------------------------------------------
//
// Description: Finds the first occurrence of the passed in object and pulls it
//				off the list (without freeing the memory for the object).  Note that
//				the object's refcount will decremented by function exit, and it is
//				up to the caller to delete it or increment the refcount as
//				necessary.
//
// Arguments:
//	PLLITEM pItem	Item to pull from list.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT LLIST::RemoveFirstReference(PLLITEM pItem)
{
	HRESULT		hr = S_OK;
	PLLITEM		pCurrent = NULL;
	PLLALIAS	pCurrentAsAlias = NULL;
	PLLITEM		pPrev = NULL;
	PLLITEM		pNext = NULL;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (using deleted object)
#endif // DEBUG


	this->EnterCritSection();

	if (this->m_iCount <= 0)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: No items in list (count = %i)!", 1, this->m_iCount);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (there aren't any items)


	pCurrent = this->m_pFirst;
	pCurrentAsAlias = (PLLALIAS) pCurrent;

	// Keep trying while it's either:
	// not an alias and it isn't the object, or
	// it is an alias, but it doesn't point to the object.
	while (! (((! pCurrent->m_fAlias) && (pCurrent == pItem)) ||
			((pCurrent->m_fAlias) && (pCurrentAsAlias->m_pTarget == pItem))) )
	{
		if (pCurrent->m_pNext == NULL)
			break; // get out of here, it's not in the list
		pCurrent = pCurrent->m_pNext;
		pCurrentAsAlias = (PLLALIAS) pCurrent;
	} // end while (we haven't found that item yet)

	// If we didn't successfully find it in the list
	if (! (((! pCurrent->m_fAlias) && (pCurrent == pItem)) ||
		((pCurrent->m_fAlias) && (pCurrentAsAlias->m_pTarget == pItem))) )
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Didn't find item %x in list!", 1, pItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (couldn't find it in the list)


#ifdef DEBUG
	if (pCurrent->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "About to remove reference to item (%x) which has already been deleted!  DEBUGBREAK()-ing.",
			1, pCurrent);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (the item has already been deleted)
#endif //DEBUG

	
	// If there was only one item in this list, reset the list and jump to
	// the object cleanup code.
	if (this->m_iCount == 1)
	{
		this->m_pFirst = NULL;
		this->m_pLast = NULL;
		this->m_iCount = 0;
		goto REMOVE_OBJECT;
	} // end if (there's only one item)

	pPrev = pCurrent->m_pPrev;
	pNext = pCurrent->m_pNext;

	if (pCurrent == this->m_pFirst)
	{
#ifdef DEBUG
		if (pNext == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "%x is first object, and there's %i items left in the list, but there's no next pointer!  DEBUGBREAK()-ing.",
				2, pCurrent, this->m_iCount);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (our count appears to be screwed up)
#endif //DEBUG

		pNext->m_pPrev = NULL;
		this->m_pFirst = pNext;
	} // end if (we're removing references from the first item)
	else if (pCurrent == this->m_pLast)
	{
#ifdef DEBUG
		if (pPrev == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "%x is the last object, and there's %i items left in the list, but there's no previous pointer!  DEBUGBREAK()-ing",
				2, pCurrent, this->m_iCount);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (our count appears to be screwed up)
#endif //DEBUG

		pPrev->m_pNext = NULL;
		this->m_pLast = pPrev;
	} // end else if (we're removing references from the last item)
	else
	{
#ifdef DEBUG
		if ((pPrev == NULL) || (pNext == NULL))
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "%x is not the first or last object, and there's %i items left in the list, but either the previous pointer (%x) or next pointer (%x) is NULL!  DEBUGBREAK()-ing.",
				4, pCurrent, this->m_iCount, pPrev, pNext);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (our count appears to be screwed up)
#endif //DEBUG

		pPrev->m_pNext = pNext;
		pNext->m_pPrev = pPrev;
	} // end else (we're deleting in the middle)
	
	this->m_iCount--; // we took it out of the chain, so decrement the counter


REMOVE_OBJECT:

	pCurrent->m_pPrev = NULL;
	pCurrent->m_pNext = NULL;

	if (pCurrent->m_fAlias)
	{
		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Deleting alias   %x", 1, pCurrentAsAlias);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/


		// Grab the target so we can work with it below.
		pCurrent = pCurrentAsAlias->m_pTarget;

		pCurrentAsAlias->m_pTarget->m_dwRefCount--; // alias is no longer pointing to it
		delete (pCurrentAsAlias); // remove this as an alias object
		pCurrentAsAlias = NULL;
	} // end if (it's an alias)


#ifdef DEBUG
	if (pCurrent->m_dwRefCount == 0)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Original object %x refcount is 0!?", 1, pCurrent);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (object refcount is bad)
#endif // DEBUG

	// We took it off the list, so decrement the reference count.  Note: we aren't
	// deleting it!
	pCurrent->m_dwRefCount--;

	if (pCurrent->m_dwRefCount == 0)
	{
		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Object %x not being deleted when refcount hit 0, possibly expected behavior.",
			1, pCurrent);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/
	} // end if (last reference to this object)
	/*
	else
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Object %x refcount is %u.",
			2, pCurrent, pCurrent->m_dwRefCount);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end else (not last reference)
	*/


DONE:

	this->LeaveCritSection();
	return (hr);
} // LLIST::RemoveFirstReference
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::Remove()"
//==================================================================================
// LLIST::Remove
//----------------------------------------------------------------------------------
//
// Description: Removes the item at the specified index.
//
// Arguments:
//	int iPos	Index of item to remove from list.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT LLIST::Remove(int iPos)
{
	HRESULT		hr = S_OK;
	PLLITEM		pCurrent = NULL;
	PLLITEM		pPrev = NULL;
	PLLITEM		pNext = NULL;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (using deleted object)
#endif // DEBUG


	this->EnterCritSection();

	if ((iPos < 0) || (this->m_iCount <= 0) || (iPos >= this->m_iCount))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Object to remove (%i) is out of range (list count == %i)!",
			2, iPos, this->m_iCount);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (no items or the number to get is too high )


	pCurrent = this->m_pFirst;

	while (iPos > 0)
	{
#ifdef DEBUG
		if (pCurrent->m_pNext == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Object %x has no next pointer but there should be at least %i more items (%i total in list)!",
				3, pCurrent, iPos, this->m_iCount);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (our count got screwed)
#endif //DEBUG

		pCurrent = pCurrent->m_pNext;
		iPos--;
	} // end while (we've still got more to traverse)



#ifdef DEBUG
	if (pCurrent->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "About to remove item (%x) which has already been deleted!  DEBUGBREAK()-ing.",
			1, pCurrent);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (the item has already been deleted)
#endif //DEBUG


	// If there was only one item in this list, reset the list and continue on to
	// deleting the object. 
	if (this->m_iCount == 1)
	{
		this->m_pFirst = NULL;
		this->m_pLast = NULL;
		this->m_iCount = 0;
		goto DELETE_OBJECT;
	} // end if (there's only one item)


	pPrev = pCurrent->m_pPrev;
	pNext = pCurrent->m_pNext;

	if (pCurrent == this->m_pFirst)
	{
#ifdef DEBUG
		if (pNext == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "%x is first object, and there's %i items left in the list, but there's no next pointer!  DEBUGBREAK()-ing.",
				2, pCurrent, this->m_iCount);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (our count appears to be screwed up)
#endif //DEBUG

		pNext->m_pPrev = NULL;
		this->m_pFirst = pNext;
	} // end if (we're removing the first item)
	else if (pCurrent == this->m_pLast)
	{
#ifdef DEBUG
		if (pPrev == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "%x is the last object, and there's %i items left in the list, but there's no previous pointer!  DEBUGBREAK()-ing.",
				2, pCurrent, this->m_iCount);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (our count appears to be screwed up)
#endif //DEBUG

		pPrev->m_pNext = NULL;
		this->m_pLast = pPrev;
	} // end else if (we're removing the last item)
	else
	{
#ifdef DEBUG
		if ((pPrev == NULL) || (pNext == NULL))
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "%x is not the first or last object, and there's %i items left in the list, but either the previous pointer (%x) or next pointer (%x) is NULL!  DEBUGBREAK()-ing.",
				4, pCurrent, this->m_iCount, pPrev, pNext);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (our count appears to be screwed up)
#endif //DEBUG

		pPrev->m_pNext = pNext;
		pNext->m_pPrev = pPrev;
	} // end else (we're deleting in the middle)
	
	this->m_iCount--; // we took it out of the chain, so decrement the counter



DELETE_OBJECT:

	if (pCurrent->m_fAlias)
	{
		PLLALIAS	pCurrentAsAlias;


		pCurrentAsAlias = (PLLALIAS) pCurrent;

#ifdef DEBUG
		if (pCurrentAsAlias->m_pTarget == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Alias points to null object!  How did that happen?  DEBUGBREAK()-ing.", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();

			hr = ERROR_BAD_ENVIRONMENT;
			goto DONE;
		} // end if (something got screwed)

		if (pCurrentAsAlias->m_pTarget->m_fDeleted)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "About to remove alias target (%x) which has already been deleted!  DEBUGBREAK()-ing.",
				1, pCurrentAsAlias->m_pTarget);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			DEBUGBREAK();
		} // end if (the item has already been deleted)
#endif //DEBUG

		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Deleting alias   %x", 1, pCurrentAsAlias);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/


		// Grab the target so we can possibly delete it as well in a second
		pCurrent = pCurrentAsAlias->m_pTarget;

		pCurrentAsAlias->m_pTarget->m_dwRefCount--; // alias is no longer pointing to it
		delete (pCurrentAsAlias); // remove this as an alias object
		pCurrentAsAlias = NULL;
	} // end if (it's an alias)


#ifdef DEBUG
	if (pCurrent->m_dwRefCount == 0)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Original item's refcount is zero!  It should be at least one!  DEBUGBREAK()-ing.", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();

		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (item's refcount is 0)
#endif //DEBUG


	// We took it off the list, so decrement the reference count.
	pCurrent->m_dwRefCount--;
	if (pCurrent->m_dwRefCount == 0)
	{
		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Deleting original   %x", 1, pCurrent);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/

		delete (pCurrent); // remove this as a normal object
		pCurrent = NULL;
	} // end if (this is the last reference to this object)
	/*
	else
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Not deleting object %x, its refcount is %u.", 
			2, pCurrent, pCurrent->m_dwRefCount);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end else (not last reference)
	*/


DONE:

	this->LeaveCritSection();

	return (hr);
} // LLIST::Remove
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::RemoveAll()"
//==================================================================================
// LLIST::RemoveAll
//----------------------------------------------------------------------------------
//
// Description: Removes all items in the list.
//
// Arguments: None.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT LLIST::RemoveAll(void)
{
	HRESULT		hr = S_OK;
	int			iNumItems = -1;
	int			i;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (using deleted object)
#endif // DEBUG


	this->EnterCritSection();

	iNumItems = this->m_iCount;

	for(i = 0; i < iNumItems; i++)
	{
		hr = this->Remove(0);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't remove first item (this->m_pFirst = %x, this->m_iCount = %i)",
				2, this->m_pFirst, this->m_iCount);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			break;
		} // end if (couldn't remove the first item)

	} // end for (each item in the list)

	this->LeaveCritSection();

	return (hr);
} // LLIST::RemoveAll
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::PopFirstItem()"
//==================================================================================
// LLIST::PopFirstItem
//----------------------------------------------------------------------------------
//
// Description: Pops the first item from the list and returns it.  If there are no
//				items, NULL is returned.  The item refcount is not reduced, because
//				even though it was taken off the list, the caller now has a
//				reference to it.
//
// Arguments: None.
//
// Returns: First item from list, or NULL if none.
//==================================================================================
PLLITEM LLIST::PopFirstItem(void)
{
	PLLITEM		pItem = NULL;



#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (using deleted object)
#endif // DEBUG


	this->EnterCritSection();

	if (this->m_pFirst == NULL)
		goto DONE;

	pItem = this->m_pFirst;
	this->m_pFirst = pItem->m_pNext;

	// Since it's not going to be in a list anymore, enforce the fact that it's
	// not part of a chain.
	pItem->m_pNext = NULL;
#ifdef DEBUG
	if (pItem->m_pPrev != NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Object %x is screwed up, it's previous pointer is %x (should be NULL)!",
			2, pItem, pItem->m_pPrev);

		DEBUGBREAK();
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end if (item previous pointer is not NULL)
#endif // DEBUG

	this->m_iCount--; // we took out an item

	// If there's another item after the one we're popping, remove the backward
	// link it had to the item we popped.
	if (this->m_pFirst != NULL)
	{
#ifdef DEBUG
		if (this->m_iCount == 0)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Count is screwed up (it's 0 but %x is a non-NULL pointer)!",
				1, this->m_pFirst);

			DEBUGBREAK();
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			goto DONE;
		} // end if (count is wrong)
#endif // DEBUG

		this->m_pFirst->m_pPrev = NULL;
	} // end if (there's another item)
	else
	{
#ifdef DEBUG
		if (this->m_iCount != 0)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Count is screwed up (it's %i but the first item pointer is NULL)!",
				1, this->m_iCount);

			DEBUGBREAK();
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			goto DONE;
		} // end if (count is wrong)
#endif // DEBUG

		// Make sure the last pointer is empty, too.
		this->m_pLast = NULL;
	} // end else (there aren't any more items)


	// If it was an alias instead of a real item, we can throw away the
	// alias.
	if (pItem->m_fAlias)
	{
		PLLALIAS	pAlias = NULL;


		pAlias = (PLLALIAS) pItem;
		pItem = pAlias->m_pTarget;

		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Deleting alias   %x", 1, pAlias);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/

		// Delete the alias, we don't need it.
		delete (pAlias);
		pAlias = NULL;

		pItem->m_dwRefCount--; // the alias is now gone.
	} // end if (the item is an alias)


#ifdef DEBUG
	if (pItem->m_dwRefCount == 0)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Original object %x refcount is 0!?", 1, pItem);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		DEBUGBREAK();
	} // end if (object refcount is bad)
#endif // DEBUG


DONE:

	this->LeaveCritSection();

	return (pItem);
} // LLIST::PopFirstItem
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::Count()"
//==================================================================================
// LLIST::Count
//----------------------------------------------------------------------------------
//
// Description: Returns the number of items in the list.
//
// Arguments: None.
//
// Returns: Number of items in the list.
//==================================================================================
int LLIST::Count(void)
{
#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (-1);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (-1);
	} // end if (using deleted object)
#endif // DEBUG


	this->EnterCritSection();
	this->LeaveCritSection();
	return (this->m_iCount);
} // LLIST::Count
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::MoveAfter()"
//==================================================================================
// LLIST::MoveAfter
//----------------------------------------------------------------------------------
//
// Description: Moves the item at the first specified index to be placed after
//				the second index.  If the after index is greater than the number
//				items actually in the list, it is placed at the end.
//
// Arguments:
//	int iPos		Index of item to move.
//	int iAfterPos	Index of item to place after.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT LLIST::MoveAfter(int iPos, int iAfterPos)
{
	HRESULT		hr = S_OK;
	PLLITEM		pCurrent = NULL;
	PLLITEM		pAfter = NULL;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (using deleted object)
#endif // DEBUG


	this->EnterCritSection();

	if ((this->m_iCount <= 0) || (iPos >= this->m_iCount))
	{
		this->LeaveCritSection();
		return (ERROR_INVALID_PARAMETER);
	} // end if (no items or bad count)

	if (iAfterPos >= this->m_iCount)
		iAfterPos = this->m_iCount - 1;

	if (iPos == iAfterPos)
	{
		this->LeaveCritSection();
		return (S_OK);
	} // end if (we're moving it after itself)

	pCurrent = this->GetItem(iPos);
	pAfter = this->GetItem(iAfterPos);

	//BUGBUG this may not work if the item appears multiple times in the list
	hr = this->RemoveFirstReference(pCurrent);
	if (hr == S_OK)
		hr = this->AddAfter(pCurrent, pAfter);

	this->LeaveCritSection();
	return (hr);
} // LLIST::MoveAfter
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"LLIST::SwapPosition()"
//==================================================================================
// LLIST::SwapPosition
//----------------------------------------------------------------------------------
//
// Description: Switches the items given so that the first item is now where the
//				second used to be, and vice versa.
//
// Arguments:
//	int iPos1	Index of first item to swap.
//	int iPos2	Index of other item to swap.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT LLIST::SwapPosition(int iPos1, int iPos2)
{
	HRESULT		hr = S_OK;
	PLLITEM		pItem1 = NULL;
	PLLITEM		pItem2 = NULL;
	PLLITEM		pItem1Prev = NULL;
	PLLITEM		pItem2Prev = NULL;


#ifdef DEBUG
	if (this == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using invalid object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (we've got a bad object)

	if (this->m_fDeleted)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "ERROR: Using deleted object!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (using deleted object)
#endif // DEBUG


	this->EnterCritSection();

	if ((this->m_iCount <= 0) || (iPos1 >= this->m_iCount) || (iPos2 >= this->m_iCount))
	{
		this->LeaveCritSection();
		return (ERROR_INVALID_PARAMETER);
	} //end if (no items or bad indices)

	if (iPos1 == iPos2)
	{
		this->LeaveCritSection();
		return (S_OK);
	} // end if (they're the same item)

	pItem1 = this->GetItem(iPos1);
	if (pItem1 == NULL)
	{
		this->LeaveCritSection();
		return (E_FAIL);
	} // end if (failed to get the item)
	pItem2 = this->GetItem(iPos2);
	if (pItem2 == NULL)
	{
		this->LeaveCritSection();
		return (E_FAIL);
	} // end if (failed to get the item)

	pItem1Prev = pItem1->m_pPrev;
	pItem2Prev = pItem2->m_pPrev;

	if (pItem2Prev == pItem1)
	{
		//BUGBUG this may not work if the item appears multiple times in the list
		hr = this->RemoveFirstReference(pItem1);

		if (hr == S_OK)
			hr = this->AddAfter(pItem1, pItem2);
		
		this->LeaveCritSection();
		return (hr);
	} // end if (item 2 immediately follows item 1)

	if (pItem1Prev == pItem2)
	{
		//BUGBUG this may not work if the item appears multiple times in the list
		hr = this->RemoveFirstReference(pItem2);

		if (hr == S_OK)
			hr = this->AddAfter(pItem2, pItem1);
		
		this->LeaveCritSection();
		return (hr);
	} // end if (item 1 immediately follows item 2)

	//BUGBUG this may not work if the item appears multiple times in the list
	hr = this->RemoveFirstReference(pItem1);
	if (hr == S_OK)
		hr = this->RemoveFirstReference(pItem2);

	if (hr == S_OK)
		hr = this->AddAfter(pItem1, pItem2Prev);
	if (hr == S_OK)
		hr = this->AddAfter(pItem2, pItem1Prev);

	this->LeaveCritSection();
	return (hr);
} // LLIST::SwapPosition
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
