//===========================================================================
// memptr.cpp
//
//  Copyright (C) Microsoft Corporation, 1997 - 1999
//
// Mempory and pointer functions.
//
// Functions:
//  helpMalloc()
//  helpFree()
//  helpIsValidPtr()
//  helpGetBogusPointer()
//  helpFreeBogusPointer()
//
// History:
//  08/21/1997 - davidkl - created
//  09/12/1997 - davidkl - renamed to memptr.cpp.  moved to sep project
//	10/02/1997 - brianhar - fixed error in dihelpIsValidPtr()
//===========================================================================

#include "globals.h"

//---------------------------------------------------------------------------

//===========================================================================
// helpMalloc
//
// Wraps malloc() to make life a little easier/safer.
//
// Parameters:
//
// Returns: HRESULT
//
// History:
//	03/31/1997 - davidkl - created
//  08/22/1997 - davidkl - snaked from didevsim
//  09/12/1997 - davidkl - changed failure code to outofmemory
//===========================================================================
HRESULT _stdcall helpMalloc(void **ppv, UINT uSize)
{
	BYTE **ppb = (BYTE **)ppv;

	// validate pv
	if(!helpIsValidPtr(ppv, 1, FALSE))
	{
		return E_POINTER;
	}

	// validate uSize
	if(!uSize)
	{
		return E_INVALIDARG;
	}

	// allocate the requested memory block
	*ppb = (BYTE *)malloc(uSize);
	
    if(NULL == *ppb)
    {
        return E_OUTOFMEMORY;
    }

	#ifdef DEBUG
	// pre-fill the memory with junk
	memset(*ppb, MEMJUNK, uSize);
	#endif

	// done
	return S_OK;
		
} //*** end helpMalloc()


//===========================================================================
// helpFree
//
// Wraps free() to make life a little easier/safer.
//
// Parameters:
//
// Returns:
//
// History:
//	03/31/1997 - davidkl - created
//  08/22/1997 - davidkl - snaked from didevsim
//===========================================================================
HRESULT _stdcall helpFree(void *pv)
{
	// validate pv
	if(!helpIsValidPtr(pv, 1, FALSE))
	{
		return E_POINTER;
	}

	#ifdef DEBUG
	// fill the memory block with junk
	memset(pv, MEMJUNK, _msize(pv));
	#endif

	// free the block
	free(pv);
	
	// if we get here, all is well
	return S_OK;

} //*** end helpFree()


//===========================================================================
// helpIsValidPtr
//
// Checks to see if a pointer is valid.
//
// Parameters:
//	[in]  LPVOID ptr	    - pointer to validate
//	[in]  UINT   cb			- number of bytes that ptr points to
//	[in]  BOOL   fAllowNull - should we consider NULL a valid pointer?
//
// Returns: BOOL
//
// History:
//	02/25/1997 - davidkl - created
//  08/22/1997 - davidkl - snaked from didevsim.
//	10/02/1997 - brianhar - fixed inverted logic problem
//===========================================================================
BOOL _stdcall helpIsValidPtr(LPVOID ptr, UINT cb, BOOL fAllowNull)
{

    if(IsBadReadPtr(ptr, cb) ||
        IsBadWritePtr(ptr, cb))
    {
		if (fAllowNull && (ptr==NULL))	
		{
			return TRUE;	// Only way out from here is having a NULL when NULL is OK
		}
		return FALSE;
    }

    // if we get here, we have a valid pointer
    return TRUE;

} //*** end helpIsValidPtr()


//===========================================================================
// helpIsValidWritePtr
//
// Checks to see if a pointer is a valid write ptr.
//
// Parameters:
//	[in]  LPVOID ptr	    - pointer to validate
//	[in]  UINT   cb			- number of bytes that ptr points to
//	[in]  BOOL   fAllowNull - should we consider NULL a valid pointer?
//
// Returns: BOOL
//
// History:
//	02/25/1997 - davidkl - created
//  08/22/1997 - davidkl - snaked from didevsim.
//	10/02/1997 - brianhar - fixed inverted logic problem
//  03/08/2000 - danhaff - copied from helpIsValidPtr
//===========================================================================
BOOL _stdcall helpIsValidWritePtr(LPVOID ptr, UINT cb, BOOL fAllowNull)
{

    if (IsBadWritePtr(ptr, cb))
    {
		if (fAllowNull && (ptr==NULL))	
		{
			return TRUE;	// Only way out from here is having a NULL when NULL is OK
		}
		return FALSE;
    }

    // if we get here, we have a valid pointer
    return TRUE;

} //*** end helpIsValidPtr()



//===========================================================================
// helpIsValidReadPtr
//
// Checks to see if a pointer is a valid write ptr.
//
// Parameters:
//	[in]  LPVOID ptr	    - pointer to validate
//	[in]  UINT   cb			- number of bytes that ptr points to
//	[in]  BOOL   fAllowNull - should we consider NULL a valid pointer?
//
// Returns: BOOL
//
// History:
//	02/25/1997 - davidkl - created
//  08/22/1997 - davidkl - snaked from didevsim.
//	10/02/1997 - brianhar - fixed inverted logic problem
//  03/08/2000 - danhaff - copied from helpIsValidPtr
//===========================================================================
BOOL _stdcall helpIsValidReadPtr(LPVOID ptr, UINT cb, BOOL fAllowNull)
{

    if (IsBadReadPtr(ptr, cb))
    {
		if (fAllowNull && (ptr==NULL))	
		{
			return TRUE;	// Only way out from here is having a NULL when NULL is OK
		}
		return FALSE;
    }

    // if we get here, we have a valid pointer
    return TRUE;

} //*** end helpIsValidPtr()



//===========================================================================
// helpGetBogusPointer
//
// Creates a bad pointer.
//
// Parameters: none
//
// Returns: LPVOID
//
// History:
//  09/12/1997 - davidkl - taken from tdinput sources
//===========================================================================
LPVOID _stdcall helpGetBogusPointer(void)
{

	// allocate some memory and mark it not accessable
	return VirtualAlloc(NULL, 8, MEM_COMMIT, PAGE_NOACCESS);

} //*** end helpGetBogusPointer()


//===========================================================================
// helpFreeBogusPointer
//
// Frees bad pointer created by helpGetBogusPointer.
//
// Parameters:
//  LPVOID lpv  - pointer to free
//
// Returns: nothing
//
// History:
//  09/12/1997 - davidkl - taken from tdinput sources
//  03/23/1998 - a-kellyc - Added VirtualProtect function call
//===========================================================================
void _stdcall helpFreeBogusPointer(LPVOID lpv)
{
	DWORD   dwOld;

	if (lpv)
	{
		// set the access rights back to read/write
		if (VirtualProtect(lpv, 8, PAGE_READWRITE, &dwOld))
		{
			// release memory pointed to by lpv
			VirtualFree(lpv, 0, MEM_RELEASE);
		}
	}

} //*** end helpFreeBogusPointer()


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================


//===========================================================================
//===========================================================================









