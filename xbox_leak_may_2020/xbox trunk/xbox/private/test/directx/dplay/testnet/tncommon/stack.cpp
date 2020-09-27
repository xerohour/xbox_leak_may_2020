//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <imagehlp.h>

#include "main.h"
#include "debugprint.h"
#include "cppobjhelp.h"
#include "linklist.h"
#include "strutils.h"
#include "symbols.h"
#include "stack.h"






//==================================================================================
// Structures
//==================================================================================
typedef struct tagGETSTACKTHREADCONTEXT
{
	HANDLE				hThreadToUse; // handle to thread to get the stack from
	PTNGETSTACKDATA		pData; // pointer to the parameters/options the caller specified
} GETSTACKTHREADCONTEXT, * PGETSTACKTHREADCONTEXT;

//BUGBUG let user get this (or some variant)?
typedef struct tagTNCALLSTACKENTRY
{
	PVOID	pvReturnAddress; // pointer to calling function
	PVOID	pvFunctionAddress; // pointer to called function
	PVOID	pvParam1; // first parameter to function (if applicable)
	PVOID	pvParam2; // second parameter to function (if applicable)
	PVOID	pvParam3; // third parameter to function (if applicable)
	PVOID	pvParam4; // fourth parameter to function (if applicable)
} TNCALLSTACKENTRY, * PTNCALLSTACKENTRY;





//==================================================================================
// Function prototypes so the below globals will work
//==================================================================================
typedef BOOL (__stdcall * STACKWALKPROC)				(DWORD, HANDLE, HANDLE,
														LPSTACKFRAME, LPVOID,
														PREAD_PROCESS_MEMORY_ROUTINE,
														PFUNCTION_TABLE_ACCESS_ROUTINE,
														PGET_MODULE_BASE_ROUTINE,
														PTRANSLATE_ADDRESS_ROUTINE);

typedef LPVOID (__stdcall * SYMFUNCTIONTABLEACCESSPROC)	(HANDLE, DWORD_PTR);

typedef DWORD_PTR (__stdcall * SYMGETMODULEBASEPROC)	(HANDLE, DWORD_PTR);





//==================================================================================
// "Secret" External Globals from symbols.cpp
//
// These are defined here so that we can access the IMAGEHLP.DLL and NTDLL.DLL
// functionality without having to load and initialize it here as well, etc.  Plus
// we can hook into the symbol stuff better (why would you even bother trying to
// dump the stack without symbols anyway?).
// Note that this means TNSymInitialize must be called in order for these functions
// to work.
//==================================================================================
extern DWORD						g_dwTNSymInits;
extern BOOL							g_fTNSymSomeFuncsUnavailable;
extern HANDLE						g_hTNSymProcess;
extern STACKWALKPROC				g_pfnStackWalk;
extern SYMFUNCTIONTABLEACCESSPROC	g_pfnSymFunctionTableAccess;
extern SYMGETMODULEBASEPROC			g_pfnSymGetModuleBase;
extern RTLGETCALLERSADDRESSPROC		g_pfnRtlGetCallersAddress;



//==================================================================================
// Local Prototypes
//==================================================================================
HRESULT PrintCallStack(LPCONTEXT lpThreadContext, HANDLE hThread,
					PVOID pvIgnoreCallsAboveFunction);

#ifdef _X86_
HRESULT PrintRawIntelStack(LPCONTEXT lpThreadContext, DWORD dwNumEntries);
#endif // _X86_

void Win9xFakeRtlGetCallersAddress(PVOID* ppvCallersAddress,
									PVOID* ppvCallersCaller);

DWORD WINAPI GetStackThreadProc(LPVOID lpvParameter);







#ifndef _XBOX // We don't support GetModuleFileName and this function ultimately depends on it
#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNStackGetStackForCurrentThread()"
//==================================================================================
// TNStackGetStackForCurrentThread
//----------------------------------------------------------------------------------
//
// Description: Retrieves the stack for the current thread.
//
// Arguments:
//	PTNGETSTACKDATA pGetStackData		Pointer to parameter block for options on
//										how to get the stack.  See its definition.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNStackGetStackForCurrentThread(PTNGETSTACKDATA pGetStackData)
{
	HRESULT					hr;
	GETSTACKTHREADCONTEXT	gstc;
	HANDLE					hGetStackThread = NULL;
	DWORD					dwThreadID;


	ZeroMemory(&gstc, sizeof (GETSTACKTHREADCONTEXT));

	DPL(9, "==> (%X)", 1, pGetStackData);

	if (g_dwTNSymInits == 0) 
	{
		DPL(0, "TNSymInitialize was not successfully called!", 0);
		hr = TNSYMERR_UNINITIALIZED;
		goto DONE;
	} // end if (sym not initialized)

	if (pGetStackData->dwSize != sizeof (TNGETSTACKDATA))
	{
		DPL(0, "GetStackData passed in is not correct size (%i != %i)!",
			2, pGetStackData->dwSize, sizeof (TNGETSTACKDATA));
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (parameter block is wacky)
	

	// We need to convert the pseudo handle that GetCurrentThread() returns
	// into a real handle, because if we try to use the pseudo handle in
	// the GetStack thread, the OS will actually resolve it to be that
	// thread instead of this one.
	if (! DuplicateHandle(g_hTNSymProcess, GetCurrentThread(),
						g_hTNSymProcess, &gstc.hThreadToUse,
						0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		hr = GetLastError();
		DPL(0, "Duplicating the current thread handle failed!", 0);
		goto DONE;
	} // end if (duplicating handle failed)

	gstc.pData = pGetStackData;


	hGetStackThread = CreateThread(NULL, 0, GetStackThreadProc, &gstc, 0, &dwThreadID);
	if (hGetStackThread == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create GetStack thread!", 0);
		goto DONE;
	} // end if (couldn't create thread)


	// Wait for the thread to complete
	hr = WaitForSingleObject(hGetStackThread, INFINITE);
	switch (hr)
	{
		case WAIT_OBJECT_0:
			// Thread is done as expected
			if (! GetExitCodeThread(hGetStackThread, (LPDWORD) (&hr)))
			{
				hr = GetLastError();
				DPL(0, "Couldn't get thread's exit code!", 0);
				goto DONE;
			} // end if (couldn't get thread's exit code)
		  break;

		/*
		case WAIT_TIMEOUT:
			// Thread is taking a really long time to die or something
			// is hosed...
		  break;
		*/

		default:
			// Some other wierd return
			DPL(0, "WaitForSingleObject returned unexpected value!", 0);
			goto DONE;
		  break;
	} // end switch (on wait result)


DONE:

	if (hGetStackThread != NULL)
	{
		CloseHandle(hGetStackThread);
		hGetStackThread = NULL;
	} // end if (have handle)

	if (gstc.hThreadToUse != NULL)
	{
		CloseHandle(gstc.hThreadToUse);
		gstc.hThreadToUse = NULL;
	} // end if (have handle)


	DPL(9, "<== %e", 1, hr);

	return (hr);
} // TNStackGetStackForCurrentThread
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNStackGetRGCAProc()"
//==================================================================================
// TNStackGetRGCAProc
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments: None.
//
// Returns: Pointer to procedure to use.
//==================================================================================
RTLGETCALLERSADDRESSPROC TNStackGetRGCAProc(void)
{
	if (g_pfnRtlGetCallersAddress != NULL)
		return (g_pfnRtlGetCallersAddress);

	return (Win9xFakeRtlGetCallersAddress);
} // TNStackGetRGCAProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#ifndef _XBOX // We don't support GetModuleFileName and this function ultimately depends on it
#undef DEBUG_SECTION
#define DEBUG_SECTION	"PrintCallStack()"
//==================================================================================
// PrintCallStack
//----------------------------------------------------------------------------------
//
// Description: Walks the call stack and prints out all functions found.  If the
//				IgnoreCalls pointer is not NULL and a call to the function specified
//				by that address is found on the stack, it and all calls above it in
//				the stack are not printed.
//
// Arguments:
//	LPCONTEXT lpThreadContext			Pointer to the context of a suspended thread
//										whose call stack you wish to dump.
//	HANDLE hThread						Handle to the suspended thread.
//	PVOID pvIgnoreCallsAboveFunction	Address user wants to ignore on callstack if
//										not NULL.  Also all calls above this on the
//										stack are ignored as well.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT PrintCallStack(LPCONTEXT lpThreadContext, HANDLE hThread,
					PVOID pvIgnoreCallsAboveFunction)
{
	HRESULT				hr;
	STACKFRAME			stackframe;
	CONTEXT				tc;
	DWORD				dwTemp = 0;
	TNCALLSTACKENTRY	callstack[32];
	DWORD				dwNumItems = 0;
	DWORD				dwSkipFunctionEntry = 0xFFFFFFFF;
	char				szSymbol[256];


	ZeroMemory(&stackframe, sizeof (STACKFRAME));
	ZeroMemory(&tc, sizeof (CONTEXT));
	ZeroMemory(&callstack, sizeof (TNCALLSTACKENTRY));

#ifdef _X86_
	// Initialize the STACKFRAME structure for the first call.  This is only
	// necessary for Intel CPUs, and isn't mentioned in the documentation.
	stackframe.AddrPC.Offset = lpThreadContext->Eip;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = lpThreadContext->Esp;
	stackframe.AddrStack.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = lpThreadContext->Ebp;
	stackframe.AddrFrame.Mode = AddrModeFlat;
#endif // _X86_


	do
	{
		if (!g_pfnStackWalk(IMAGE_FILE_MACHINE_I386,
							g_hTNSymProcess,
							hThread,
							&stackframe,
							&tc,
							NULL,
							g_pfnSymFunctionTableAccess,
							g_pfnSymGetModuleBase,
							NULL))
		{
			hr = GetLastError();
			if ((hr == ERROR_MOD_NOT_FOUND) ||
				(hr == ERROR_INVALID_ADDRESS) ||
				(hr == ERROR_PARTIAL_COPY))
			{
				/*
				DPL(0, "Stopped walking stack because of expected error (%x).  Non-fatal.",
					1, hr);
				*/
				hr = S_OK;
				break;
			} // end if (expected error)

			DPL(0, "StackWalk returned an unexpected failure!", 0);
			goto DONE;
		} // end if (stack walk failed)


		if (stackframe.AddrFrame.Offset == 0)
		{
			DPL(0, "Stack frame is goofy!", 0);
			hr = E_FAIL;
			goto DONE;
		} // end if (stack frame is goofy)

		dwNumItems++;

		// If the user wants to ignore items, we haven't found the appropriate
		// function yet, and this is the function, mark it for skipping.
		if ((pvIgnoreCallsAboveFunction != NULL) &&
			(dwSkipFunctionEntry == 0xFFFFFFFF) &&
			(((LPVOID) (stackframe.AddrPC.Offset)) == pvIgnoreCallsAboveFunction))
		{
			/*
			DPL(9, "Found function to skip (%X) at stack level %u.",
				2, lpvIgnoreCallsAboveFunction, dwTemp);
			*/
			dwSkipFunctionEntry = dwTemp;
		} // end if (we found the skip function)

		callstack[dwTemp].pvReturnAddress = (LPVOID) (stackframe.AddrReturn.Offset);
		callstack[dwTemp].pvFunctionAddress = (LPVOID) (stackframe.AddrPC.Offset);
		callstack[dwTemp].pvParam1 = (LPVOID) stackframe.Params[0];
		callstack[dwTemp].pvParam2 = (LPVOID) stackframe.Params[1];
		callstack[dwTemp].pvParam3 = (LPVOID) stackframe.Params[2];
		callstack[dwTemp].pvParam4 = (LPVOID) stackframe.Params[3];

		dwTemp++;
		if (dwTemp > 32)
		{
			DPL(0, "32 stack items limit reached.", 0);
			break;
		} // end if (hit item limit)
	} // end do (while not done)
	while (true);

	// We're going to print out what we found.  Spit out the header
	DPL(9, "----- Dumping call stack -----", 0);
	DPL(9, "ReturnAddr\t   Module!SymbolName + offset   \t    Params", 0);
	DPL(9, "----------\t--------------------------------\t---------------------", 0);

	// If we didn't find (or didn't look for) a function to ignore, then start
	// printing at item zero (the first one)..  Otherwise, start at the function's
	// depth. 
	if (dwSkipFunctionEntry == 0xFFFFFFFF)
	{
		if (pvIgnoreCallsAboveFunction != NULL)
		{
			DPL(9, "WARNING: Didn't find function expected to skip on call stack. %X",
				1, pvIgnoreCallsAboveFunction);
		} // end if (were expecting to find function)

		dwSkipFunctionEntry = 0;
	} // end if (didn't find skip function)
	else
	{
		/*
		DPL(9, "... (%u functions)", 1, dwSkipFunctionEntry - 1);
		*/
	} // end else (found skip function))

	for(dwTemp = dwSkipFunctionEntry; dwTemp < dwNumItems; dwTemp++)
	{
		hr = TNSymResolve(callstack[dwTemp].pvFunctionAddress, szSymbol, 256);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't resolve symbol for %x!", 1, callstack[dwTemp].pvFunctionAddress);
			goto DONE;
		} // end if (couldn't resolve symbol)

		DPL(9, "%x\t%s\t(%x, %x, %x, %x)",
			6, callstack[dwTemp].pvReturnAddress, szSymbol,
			callstack[dwTemp].pvParam1, callstack[dwTemp].pvParam2,
			callstack[dwTemp].pvParam3, callstack[dwTemp].pvParam4);
	} // end for (each item to print)

	DPL(9, "----- End call stack dump -----", 0);


DONE:

	return (hr);
} // PrintCallStack
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX

#ifndef _XBOX // We don't support GetModuleFileName and this function ultimately depends on it
#ifdef _X86_

#undef DEBUG_SECTION
#define DEBUG_SECTION	"PrintRawIntelStack()"
//==================================================================================
// PrintRawIntelStack
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	LPCONTEXT lpThreadContext	Pointer to the context of a suspended thread whose
//								raw stack you wish to dump.
//	DWORD dwNumEntries			Number of DWORDs from the stack that should be
//								dumped.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT PrintRawIntelStack(LPCONTEXT lpThreadContext, DWORD dwNumEntries)
{
	HRESULT		hr;
	DWORD*		pdwPointer;
	DWORD		dwTemp = 0;
	char		szSymbol[1024];



	DPL(9, "----- Dumping stack -----", 0);

	pdwPointer = (DWORD*) (lpThreadContext->Esp);
	do
	{
		if (IsBadReadPtr(pdwPointer, sizeof (DWORD)))
		{
			Sleep(25);
			DPL(0, "Memory not valid at %x, breaking.", 1, pdwPointer);
			break;
		} // end if (the memory is not valid)

		hr = TNSymResolve((LPVOID) (*pdwPointer), szSymbol, 1024);
		if (hr != S_OK)
		{
			Sleep(25);
			DPL(0, "Couldn't resolve symbol for %x!", 1, (*pdwPointer));
			goto DONE;
		} // end if (couldn't resolve symbol)

		DPL(9, "%x:\t%s", 2, pdwPointer, szSymbol);

		pdwPointer++;

		dwTemp++;
		if (dwTemp > dwNumEntries)
		{
			Sleep(25);
			DPL(9, "%i dump limit.", 1, dwNumEntries);
			break;
		} // end if (we've reached the limit)
	} // end do (while not done)
	while (true);

	Sleep(25);
	DPL(9, "----- End stack dump -----", 0);


DONE:

	return (hr);
} // PrintRawIntelStack
#undef DEBUG_SECTION
#define DEBUG_SECTION	""


#endif // _X86_
#endif // ! XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"Win9xFakeRtlGetCallersAddress()"
//==================================================================================
// Win9xFakeRtlGetCallersAddress
//----------------------------------------------------------------------------------
//
// Description: Dummy function that doesn't actually retrieve return addresses.
//
// Arguments:
//	PVOID* ppvCallersAddress	Place to store caller's return address.
//	PVOID* ppvCallersCaller		Place to store caller's caller's return address.
//
// Returns: None.
//==================================================================================
void Win9xFakeRtlGetCallersAddress(PVOID* ppvCallersAddress,
									PVOID* ppvCallersCaller)
{
	(*ppvCallersAddress) = NULL;
	(*ppvCallersCaller) = NULL;
} // Win9xFakeRtlGetCallersAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#ifndef _XBOX // this function doesn't seem to be used, and we don't support GetModuleFileName
#undef DEBUG_SECTION
#define DEBUG_SECTION	"GetStackThreadProc()"
//==================================================================================
// GetStackThreadProc
//----------------------------------------------------------------------------------
//
// Description: Short lived thread that suspends the specified thread, dumps its
//				stack, and the starts it again.
//
// Arguments:
//	LPVOID lpvParameter		Parameter.  Cast as pointer to a GETSTACKTHREADCONTEXT
//							structure.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
DWORD WINAPI GetStackThreadProc(LPVOID lpvParameter)
{
	HRESULT						hr = S_OK;
	PGETSTACKTHREADCONTEXT		pContext = (PGETSTACKTHREADCONTEXT) lpvParameter;
	BOOL						fThreadSuspended = FALSE;
	CONTEXT						tc;


	if ((pContext == NULL) || (pContext->pData == NULL))
	{
		DPL(0, "Somehow we got started without a context or options!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (context or options don't exist)

	SuspendThread(pContext->hThreadToUse);

	fThreadSuspended = TRUE;

	ZeroMemory(&tc, sizeof (CONTEXT));
	tc.ContextFlags = CONTEXT_CONTROL;
	if (! GetThreadContext(pContext->hThreadToUse, &tc))
	{
		hr = GetLastError();
		DPL(0, "GetThreadContext failed!", 0);
		goto DONE;
	} // end if (get thread context failed)

	// If the caller wants the callstack printed, and symbol functions
	// are available, do it.
	if ((pContext->pData->fCallStack) && (! g_fTNSymSomeFuncsUnavailable))
	{
		hr = PrintCallStack(&tc, pContext->hThreadToUse,
							pContext->pData->pvIgnoreCallsAboveFunction);
		if (hr != S_OK)
		{
			DPL(0, "Printing call stack failed!  %e", 1, hr);
			goto DONE;
		} // end if (printing stack failed)
	} // end if (we should print the call stack)

	if (pContext->pData->dwRawStackDepth > 0)
	{
#ifdef _X86_
		hr = PrintRawIntelStack(&tc, pContext->pData->dwRawStackDepth);
		if (hr != S_OK)
		{
			DPL(0, "Printing raw Intel stack failed!  %e", 1, hr);
			goto DONE;
		} // end if (printing stack failed)
#else
		DPL(0, "Can't print raw stack for non-Intel machines at this time!  Ignoring.", 0);
#endif // ! _X86_
	} // end if (we should print the raw stack depth)


DONE:

	if (fThreadSuspended)
	{
		ResumeThread(pContext->hThreadToUse);
		fThreadSuspended = FALSE;
	} // end if (suspended thread)

	return (hr);
} // GetStackThreadProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX
