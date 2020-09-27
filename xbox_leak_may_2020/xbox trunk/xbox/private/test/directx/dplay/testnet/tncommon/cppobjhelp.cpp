//==================================================================================
// Includes
//==================================================================================
#include <windows.h>


#include "main.h"
#include "cppobjhelp.h"
#ifdef DEBUG
#include "linklist.h"
#include "sprintf.h"
#ifndef NO_TNCOMMON_DEBUG_SPEW
#include "debugprint.h"
#endif // ! NO_TNCOMMON_DEBUG_SPEW
#ifndef NO_CPPOBJHELP_LEAKCHECK
#include "symbols.h"
#include "stack.h"
#include "strutils.h"
#include "linkedstr.h"
#include "fileutils.h"
#endif // ! NO_CPPOBJHELP_LEAKCHECK
#endif // DEBUG




#ifdef DEBUG
#ifndef NO_CPPOBJHELP_LEAKCHECK


//==================================================================================
// Defines
//==================================================================================
#define MEMORYTRACKING_ARRAYSIZEINCREMENT	50

#define MEMORYTRACKING_COOKIE				0x66666666
#define MEMORYTRACKING_FREEDCOOKIE			0x6A6A6A6A




//==================================================================================
// Structures
//==================================================================================
typedef struct tagMEMORYTRACKINGHEADER
{
	DWORD			dwCookie; // cookie to make sure the memory is good
	PVOID			pvCallersAddress; // address of the allocating function
	PVOID			pvCallersCaller; // address of allocating function's caller
	unsigned int	uiSize; // size of the memory
} MEMORYTRACKINGHEADER, * PMEMORYTRACKINGHEADER;




//==================================================================================
// Globals
//==================================================================================
DWORD					g_dwCPPObjInits = 0;
CRITICAL_SECTION		g_csMem;
DWORD					g_dwNumOutstandingObjects = 0;
DWORD					g_dwTotalOutstandingBytes = 0;
PMEMORYTRACKINGHEADER*	g_paAllocations = NULL;
DWORD					g_dwAllocationsCount = 0;





#undef DEBUG_SECTION
#define DEBUG_SECTION	"InitCPPObjHelp()"
//==================================================================================
// InitCPPObjHelp
//----------------------------------------------------------------------------------
//
// Description: Prepares this file for use.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void InitCPPObjHelp(void)
{
	g_dwCPPObjInits++;

	if (g_dwCPPObjInits == 1)
	{
		InitializeCriticalSection(&g_csMem);

		g_paAllocations = (PMEMORYTRACKINGHEADER*) HeapAlloc(GetProcessHeap(),
															HEAP_ZERO_MEMORY,
															MEMORYTRACKING_ARRAYSIZEINCREMENT * sizeof (PMEMORYTRACKINGHEADER));
		if (g_paAllocations == NULL)
		{
			DeleteCriticalSection(&g_csMem);
			return;
		} // end if (couldn't allocate memory)

		g_dwAllocationsCount = MEMORYTRACKING_ARRAYSIZEINCREMENT;
	} // end if (first initialization)
} // InitCPPObjHelp
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CheckForCPPObjLeaks()"
//==================================================================================
// CheckForCPPObjLeaks
//----------------------------------------------------------------------------------
//
// Description: Prints out all currently outstanding objects.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void CheckForCPPObjLeaks(void)
{
#ifndef _XBOX // no GetModuleFileName supported, so don't implement this now

	HRESULT		hr;
	DWORD		dwTemp;
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	char		szTemp[1024];
	char		szCallersAddress[1024];
	char		szCallersCaller[1024];
	DWORD_PTR	dwOffset;

#ifndef NO_TNCOMMON_DEBUG_SPEW
	// Make sure we can debug print
	TNDebugInitialize();
#endif // ! NO_TNCOMMON_DEBUG_SPEW


	if (g_dwCPPObjInits == 0)
	{
#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "CPP object help not initialized!", 0);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end if (not initted)


	// Check for any items we've leaked
	if (g_dwNumOutstandingObjects > 0)
	{
		hr = FileCreateAndOpenFile("leaks.txt", FALSE, TRUE, FALSE, &hFile);
		if (hr != S_OK)
		{
#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't create and open \"leaks.txt\" file!  %e", 1, hr);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
		} // end if (couldn't open file)


		if (hFile != INVALID_HANDLE_VALUE)
		{
			FileWriteLine(hFile, "//==================================================================================");
			StringGetCurrentDateStr(szTemp);
			FileSprintfWriteLine(hFile, "// Leaks for process %x/%u at %s",
								3, GetCurrentProcessId(), GetCurrentProcessId(),
								szTemp);
			FileWriteLine(hFile, "//==================================================================================");

			FileSprintfWriteLine(hFile, "Leaked %u C++ object(s) for a total of %u bytes!",
								2, g_dwNumOutstandingObjects,
								(g_dwTotalOutstandingBytes - (g_dwNumOutstandingObjects * sizeof (MEMORYTRACKINGHEADER))));
			FileSprintfWriteLine(hFile, "(%u bytes including memory tracking headers)!",
								1, g_dwTotalOutstandingBytes);
		} // end if (have open file)

#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Leaked %u C++ object(s) for a total of %u bytes!",
			2, g_dwNumOutstandingObjects,
			(g_dwTotalOutstandingBytes - (g_dwNumOutstandingObjects * sizeof (MEMORYTRACKINGHEADER))));
		DPL(0, "(%u bytes including memory tracking headers)!",
			1, g_dwTotalOutstandingBytes);
#endif // ! NO_TNCOMMON_DEBUG_SPEW


		for(dwTemp = 0; dwTemp < g_dwAllocationsCount; dwTemp++)
		{
			if (g_paAllocations[dwTemp] != NULL)
			{
				// Unfortunately, in-app symbol resolution doesn't seem to work
				// when the process is shutting down (plus TNSymCleanup should
				// have already been called), so we can't use %Xs.  However, we
				// can just retrieve a module name and offset, so we'll do that.
				ZeroMemory(szCallersAddress, 1024);
				ZeroMemory(szCallersCaller, 1024);

				hr = TNSymGetModuleNameAndOffsetForPointer(g_paAllocations[dwTemp]->pvCallersAddress,
															szCallersAddress,
															1024,
															&dwOffset);
				if (hr != S_OK)
				{
#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(0, "Couldn't get module name and offset for caller's address %x!  %e",
						2, g_paAllocations[dwTemp]->pvCallersAddress,
						hr);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
				} // end if (couldn't resolve name)
				else
				{
					wsprintf(szTemp, " + 0x" SPRINTF_PTR " (0x" SPRINTF_PTR ")",
							dwOffset,
							g_paAllocations[dwTemp]->pvCallersAddress);
					strcat(szCallersAddress, szTemp);
				} // end else (could get name)


				hr = TNSymGetModuleNameAndOffsetForPointer(g_paAllocations[dwTemp]->pvCallersCaller,
															szCallersCaller,
															1024,
															&dwOffset);
				if (hr != S_OK)
				{
#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(0, "Couldn't get module name and offset for caller's caller %x!  %e",
						2, g_paAllocations[dwTemp]->pvCallersCaller,
						hr);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
				} // end if (couldn't resolve name)
				else
				{
					wsprintf(szTemp, " + 0x" SPRINTF_PTR " (0x" SPRINTF_PTR ")",
							dwOffset,
							g_paAllocations[dwTemp]->pvCallersCaller);
					strcat(szCallersCaller, szTemp);
				} // end else (could get name)


				if (hFile != INVALID_HANDLE_VALUE)
				{
					FileSprintfWriteLine(hFile,
										"Leaked C++ object at %x, %u bytes (+ %u byte header)!",
										3, ((LPBYTE) (g_paAllocations[dwTemp])) + sizeof (MEMORYTRACKINGHEADER),
										g_paAllocations[dwTemp]->uiSize,
										sizeof (MEMORYTRACKINGHEADER));

					FileSprintfWriteLine(hFile, "   Allocated by %s",
										1, szCallersAddress);
					FileSprintfWriteLine(hFile, "   Which was called by %s",
										1, szCallersCaller);
				} // end if (have open file)

#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Leaked C++ object at %x, %u bytes (+ %u byte header)!",
					3, ((LPBYTE) (g_paAllocations[dwTemp])) + sizeof (MEMORYTRACKINGHEADER),
					g_paAllocations[dwTemp]->uiSize,
					sizeof (MEMORYTRACKINGHEADER));

				DPL(0, "   Allocated by %s",
					1, szCallersAddress);
				DPL(0, "   Which was called by %s",
					1, szCallersCaller);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
			} // end if (still an allocation here)
		} // end for (each allocation slot)

		if (hFile != INVALID_HANDLE_VALUE)
		{
			// Add extra space in case the file is appended to.
			FileWriteLine(hFile, "");

			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		} // end if (have open file)
	} // end if (anything left outstanding)


DONE:

#ifndef NO_TNCOMMON_DEBUG_SPEW
	TNDebugCleanup();
#endif // ! NO_TNCOMMON_DEBUG_SPEW
#else // ! XBOX
#pragma TODO(tristanj, "CheckForCPPObjLeaks isn't doing anything")
#endif // XBOX
} // CheckForCPPObjLeaks
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CleanupCPPObjHelp()"
//==================================================================================
// CleanupCPPObjHelp
//----------------------------------------------------------------------------------
//
// Description: Cleans up after this file.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void CleanupCPPObjHelp(void)
{
	if (g_dwCPPObjInits == 1)
	{
		CheckForCPPObjLeaks();


		HeapFree(GetProcessHeap(), 0, g_paAllocations);
		g_paAllocations = NULL;

		g_dwAllocationsCount = 0;
		g_dwNumOutstandingObjects = 0;

		DeleteCriticalSection(&g_csMem);
	} // end if (last deinitialize)

	g_dwCPPObjInits--;
} // CleanupCPPObjHelp
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! NO_CPPOBJHELP_LEAKCHECK
#endif // DEBUG





#undef DEBUG_SECTION
#define DEBUG_SECTION	"DLLSafeNew()"
//==================================================================================
// DLLSafeNew
//----------------------------------------------------------------------------------
//
// Description: Allocates memory of the specified size from the process heap.
//
// Arguments:
//	unsigned int cb		Size of memory block to allocate.
//
// Returns: Pointer to the memory allocated, or NULL if an error occurred.
//==================================================================================
void* DLLSafeNew(unsigned int cb)
{
	DWORD					dwSize;
	void*					pvResult;
#ifdef DEBUG
#ifndef NO_CPPOBJHELP_LEAKCHECK
	PMEMORYTRACKINGHEADER	pHeader = NULL;


	if (g_dwCPPObjInits == 0)
	{
#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "CPP object help not initialized!", 0);
#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (NULL);
	} // end if (not initialized)

	EnterCriticalSection(&g_csMem);
#endif // ! NO_CPPOBJHELP_LEAKCHECK
#endif // DEBUG


	dwSize = cb;
#ifdef DEBUG
#ifndef NO_CPPOBJHELP_LEAKCHECK
	dwSize += sizeof (MEMORYTRACKINGHEADER);
#endif // ! NO_CPPOBJHELP_LEAKCHECK
#endif // DEBUG

	pvResult = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);

#ifdef DEBUG
	/*
#ifndef NO_CPPOBJHELP_LEAKCHECK
#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(9, "%x (%x) = HeapAlloc(%x, HEAP_ZERO_MEMORY, %u (%u))",
		5, pvResult, (((LPBYTE) pvResult) + sizeof (MEMORYTRACKINGHEADER)),
		GetProcessHeap(), dwSize, cb);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
#endif // ! NO_CPPOBJHELP_LEAKCHECK
	*/

	if (pvResult == NULL)
	{
#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "HeapAlloc(%x, HEAP_ZERO_MEMORY, %u) failed!",
			2, GetProcessHeap(), dwSize);
#endif // ! NO_TNCOMMON_DEBUG_SPEW

#ifndef NO_CPPOBJHELP_LEAKCHECK
		LeaveCriticalSection(&g_csMem);
#endif // ! NO_CPPOBJHELP_LEAKCHECK

		return (NULL);
	} // end else (failed allocating memory)


#ifndef NO_CPPOBJHELP_LEAKCHECK
	pHeader = (PMEMORYTRACKINGHEADER) pvResult;
	pvResult = pHeader + 1;

	pHeader->dwCookie = MEMORYTRACKING_COOKIE;
	pHeader->uiSize = cb;
	TNStackGetCallersAddress(&(pHeader->pvCallersAddress),
							&(pHeader->pvCallersCaller));


	if (g_dwNumOutstandingObjects < g_dwAllocationsCount)
	{
		DWORD	dwTemp;


		// Look for the empty allocation slot
		for(dwTemp = 0; dwTemp < g_dwAllocationsCount; dwTemp++)
		{
			if (g_paAllocations[dwTemp] == NULL)
			{
				g_paAllocations[dwTemp] = pHeader;
				break;
			} // end if (found empty slot)
		} // end for (each slot)

#ifndef NO_TNCOMMON_DEBUG_SPEW
		if (dwTemp >= g_dwAllocationsCount)
		{
			DPL(0, "Didn't find empty slot in array!?", 0);
		} // end if (didn't find slot)
#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end if (there's an empty slot somewhere)
	else
	{
		void*	pvTemp;


		pvTemp = HeapAlloc(GetProcessHeap(),
							HEAP_ZERO_MEMORY,
							((g_dwAllocationsCount + MEMORYTRACKING_ARRAYSIZEINCREMENT) * sizeof (PMEMORYTRACKINGHEADER)));
		if (g_paAllocations == NULL)
		{
			HeapFree(GetProcessHeap(), 0, pvResult);
			pvResult = NULL;

			LeaveCriticalSection(&g_csMem);
			return (NULL);
		} // end if (couldn't allocate memory)

		CopyMemory(pvTemp, g_paAllocations,
				(g_dwAllocationsCount * sizeof (PMEMORYTRACKINGHEADER)));

		HeapFree(GetProcessHeap(), 0, g_paAllocations);
		g_paAllocations = (PMEMORYTRACKINGHEADER*) pvTemp;
		pvTemp = NULL;

		g_dwAllocationsCount += MEMORYTRACKING_ARRAYSIZEINCREMENT;

		g_paAllocations[g_dwNumOutstandingObjects] = pHeader;
	} // end if (need more room in allocation array)

	g_dwNumOutstandingObjects++;
	g_dwTotalOutstandingBytes += dwSize;


	/*
#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(9, "%u outstanding objects, %u bytes.",
		2, g_dwNumOutstandingObjects, g_dwTotalOutstandingBytes);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/


	LeaveCriticalSection(&g_csMem);
#endif // ! NO_CPPOBJHELP_LEAKCHECK
#endif // DEBUG

	return (pvResult);
} // DLLSafeNew
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"DLLSafeDelete()"
//==================================================================================
// DLLSafeDelete
//----------------------------------------------------------------------------------
//
// Description: Frees memory previously allocated from the process heap.
//
// Arguments:
//	void* p		Pointer to memory to free.
//
// Returns: None.
//==================================================================================
void DLLSafeDelete(void* p)
{
#ifdef DEBUG
#ifndef NO_CPPOBJHELP_LEAKCHECK
	PMEMORYTRACKINGHEADER	pHeader = NULL;
	DWORD					dwSize;
#endif // ! NO_CPPOBJHELP_LEAKCHECK
#ifndef NO_TNCOMMON_DEBUG_SPEW
	HRESULT					hr;
#endif // ! NO_TNCOMMON_DEBUG_SPEW


#ifndef NO_CPPOBJHELP_LEAKCHECK
	if (g_dwCPPObjInits == 0)
	{
#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "CPP object help not initialized!", 0);
#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return;
	} // end if (not initialized)

	EnterCriticalSection(&g_csMem);


	pHeader = (PMEMORYTRACKINGHEADER) (((LPBYTE) p) - sizeof (MEMORYTRACKINGHEADER));

	if (pHeader->dwCookie != MEMORYTRACKING_COOKIE)
	{
#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Memory tracking cookie for %x (allocated for %x) is wrong (%x != %x)!",
			4, pHeader, p, pHeader->dwCookie, MEMORYTRACKING_COOKIE);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end if (not cookie)
	else
	{
		// We're about to free it, so update it.
		pHeader->dwCookie = MEMORYTRACKING_FREEDCOOKIE;
	} // end else (is cookie)

	dwSize = pHeader->uiSize + sizeof (MEMORYTRACKINGHEADER);

	/*
#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(9, "HeapFree(%x, 0, %x)", 2, GetProcessHeap(), pHeader);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/
#endif // ! NO_CPPOBJHELP_LEAKCHECK
#endif // ! DEBUG


#if ((defined (DEBUG)) && (! defined (NO_CPPOBJHELP_LEAKCHECK)))
	if (! HeapFree(GetProcessHeap(), 0, pHeader))
#else // ! DEBUG || NO_CPPOBJHELP_LEAKCHECK
	if (! HeapFree(GetProcessHeap(), 0, p))
#endif // ! DEBUG || NO_CPPOBJHELP_LEAKCHECK

#ifdef DEBUG
	{
#ifndef NO_TNCOMMON_DEBUG_SPEW
		hr = GetLastError();

#ifdef NO_CPPOBJHELP_LEAKCHECK
		DPL(0, "Couldn't free item %x from process heap (%x)!  %e",
			3, p, GetProcessHeap(), hr);
#else // ! NO_CPPOBJHELP_LEAKCHECK
		DPL(0, "Couldn't free item %x (%x) from process heap (%x)!  %e",
			4, pHeader, p, GetProcessHeap(), hr);
#endif // ! NO_CPPOBJHELP_LEAKCHECK
#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end if (couldn't free item)
#ifndef NO_CPPOBJHELP_LEAKCHECK
	else
	{
		DWORD	dwTemp;


		// Loop through all our allocations for this one, and mark it as gone.
		for(dwTemp = 0; dwTemp < g_dwAllocationsCount; dwTemp++)
		{
			if (g_paAllocations[dwTemp] == pHeader)
			{
				g_paAllocations[dwTemp] = NULL; // it's gone now
				pHeader = NULL; // remember that we found it
				break; // stop looping
			} // end if (found pointer)
		} // end for (each allocation)

#ifndef NO_TNCOMMON_DEBUG_SPEW
		if (pHeader != NULL)
		{
			DPL(1, "WARNING: Couldn't find %x (%x) in memory tracking array!",
				2, pHeader, p);
		} // end if (didn't find item in array)
#endif // ! NO_TNCOMMON_DEBUG_SPEW


		g_dwNumOutstandingObjects--;
		g_dwTotalOutstandingBytes -= dwSize;

		/*
#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "%u remaining outstanding objects, %u bytes.",
			2, g_dwNumOutstandingObjects, g_dwTotalOutstandingBytes);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/
	} // end else (did free item)


	LeaveCriticalSection(&g_csMem);
#endif // ! NO_CPPOBJHELP_LEAKCHECK
#else // ! DEBUG
	{
	} // end if (couldn't free item)
#endif // ! DEBUG
} // DLLSafeDelete
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
