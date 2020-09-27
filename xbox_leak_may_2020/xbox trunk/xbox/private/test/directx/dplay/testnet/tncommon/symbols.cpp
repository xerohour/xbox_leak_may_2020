//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <imagehlp.h>

#include <stdlib.h>	// NT BUILD needs this for _pgmptr


#include "main.h"
#include "debugprint.h"
#include "cppobjhelp.h"
#include "linklist.h"
#include "linkedstr.h"
#include "fileutils.h"
#include "strutils.h"
#include "sprintf.h"
#include "symbols.h"






//==================================================================================
// Defines
//==================================================================================
#ifndef REGKEY_TESTNET
#define REGKEY_TESTNET						"SOFTWARE\\Microsoft\\DirectPlay\\TestNet"
#endif // REGKEY_TESTNET

#define REGVALUE_DISABLESYMBOLRESOLUTION	"DisableSymbolResolution"







//==================================================================================
// Typedefs for IMAGEHLP.DLL functions so that we can use them with GetProcAddress
//==================================================================================
typedef BOOL (__stdcall * SYMINITIALIZEPROC)			(HANDLE, LPSTR, BOOL);

//typedef DWORD (__stdcall * SYMGETOPTIONSPROC)			(void);

typedef DWORD (__stdcall * SYMSETOPTIONSPROC)			(DWORD);

typedef BOOL (__stdcall * SYMLOADMODULEPROC)			(HANDLE, HANDLE, LPSTR,
														LPSTR, DWORD_PTR, DWORD);

typedef BOOL (__stdcall * SYMCLEANUPPROC)				(HANDLE);

#ifdef _WIN64
typedef BOOL (__stdcall * SYMGETSYMFROMADDR64PROC)		(HANDLE, DWORD_PTR, PDWORD_PTR,
														PIMAGEHLP_SYMBOL64);
#else // _WIN64
typedef BOOL (__stdcall * SYMGETSYMFROMADDRPROC)		(HANDLE, DWORD_PTR, PDWORD_PTR,
														PIMAGEHLP_SYMBOL);
#endif // _WIN64

//typedef BOOL (__stdcall * SYMGETSEARCHPATHPROC)			(HANDLE, LPSTR, DWORD);

//typedef BOOL (__stdcall * SYMSETSEARCHPATHPROC)			(HANDLE, LPSTR);

typedef PLOADED_IMAGE (__stdcall * IMAGELOADPROC)		(LPSTR, LPSTR);

typedef BOOL (__stdcall * IMAGEUNLOADPROC)				(PLOADED_IMAGE);

typedef BOOL (__stdcall * STACKWALKPROC)				(DWORD, HANDLE, HANDLE,
														LPSTACKFRAME, LPVOID,
														PREAD_PROCESS_MEMORY_ROUTINE,
														PFUNCTION_TABLE_ACCESS_ROUTINE,
														PGET_MODULE_BASE_ROUTINE,
														PTRANSLATE_ADDRESS_ROUTINE);

typedef LPVOID (__stdcall * SYMFUNCTIONTABLEACCESSPROC)	(HANDLE, DWORD_PTR);

typedef DWORD_PTR (__stdcall * SYMGETMODULEBASEPROC)		(HANDLE, DWORD_PTR);

typedef void (__stdcall * RTLGETCALLERSADDRESSPROC)		(LPVOID*, LPVOID*);




//==================================================================================
// These are some definitions from ntrtl.h, for future reference
//==================================================================================
/*
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_M_ALPHA) || defined(_M_AXP64) || defined(_M_IA64)
PVOID
_ReturnAddress (
    VOID
    );

#pragma intrinsic(_ReturnAddress)

#define RtlGetCallersAddress(CallersAddress, CallersCaller) \
    *CallersAddress = (PVOID)_ReturnAddress(); \
    *CallersCaller = NULL;
#else
NTSYSAPI
VOID
NTAPI
RtlGetCallersAddress(
    OUT PVOID *CallersAddress,
    OUT PVOID *CallersCaller
    );
#endif

#ifdef __cplusplus
}
#endif
*/





//==================================================================================
// Globals
//==================================================================================
DWORD						g_dwTNSymInits = 0;
BOOL						g_fTNSymSomeFuncsUnavailable = FALSE;
HANDLE						g_hTNSymProcess = NULL;
#ifndef USE_STATIC_DLL_LINKS
HMODULE						g_hImageHlpDLL = NULL;
HMODULE						g_hNTDLL = NULL;
#endif // USE_STATIC_DLL_LINKS
CLStringList				g_symbolsloaded;

SYMINITIALIZEPROC			g_pfnSymInitialize = NULL;
SYMSETOPTIONSPROC			g_pfnSymSetOptions = NULL;
SYMLOADMODULEPROC			g_pfnSymLoadModule = NULL;
SYMCLEANUPPROC				g_pfnSymCleanup = NULL;
#ifdef _WIN64
SYMGETSYMFROMADDR64PROC		g_pfnSymGetSymFromAddr64 = NULL;
#else // _WIN64
SYMGETSYMFROMADDRPROC		g_pfnSymGetSymFromAddr = NULL;
#endif // _WIN64
//SYMGETSEARCHPATHPROC		g_pfnSymGetSearchPath = NULL;
//SYMSETSEARCHPATHPROC		g_pfnSymSetSearchPath = NULL;
IMAGELOADPROC				g_pfnImageLoad = NULL;
IMAGEUNLOADPROC				g_pfnImageUnload = NULL;
STACKWALKPROC				g_pfnStackWalk = NULL;
SYMFUNCTIONTABLEACCESSPROC	g_pfnSymFunctionTableAccess = NULL;
SYMGETMODULEBASEPROC		g_pfnSymGetModuleBase = NULL;
RTLGETCALLERSADDRESSPROC	g_pfnRtlGetCallersAddress = NULL;





#ifndef _XBOX // no symbol initialization
#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNSymInitialize()"
//==================================================================================
// TNSymInitialize
//----------------------------------------------------------------------------------
//
// Description: Initializes the symbols functions for use.  Must be balanced by a
//				call to TNSymCleanup.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNSymInitialize(void)
{
	HRESULT			hr;
	OSVERSIONINFO	ovi;



	g_dwTNSymInits++;

	// If we've already been initialized, we're done.
	if (g_dwTNSymInits > 1)
	{
		return (S_OK);
	} // end if (already initted)


	// Convert the pseudo-handle that GetCurrentProcess returns into a real handle
	if (! DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(),
						GetCurrentProcess(), &g_hTNSymProcess, 0, FALSE,
						DUPLICATE_SAME_ACCESS))
	{
		hr = GetLastError();
		DPL(0, "Couldn't convert process pseudo-handle into real one!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't duplicate handle)


	// Check the registry setting for controlling symbol resolution.
	if (TNSymGetDisableSymResolutionSetting())
	{
		DPL(0, "Registry setting disables symbol resolution.", 0);
		g_fTNSymSomeFuncsUnavailable = TRUE;
		return (S_OK);
	} // end if (not using symbol resolution)



	// Check to see if we're running on an NT system.  If not, we can't use some
	// functionality.

	ZeroMemory(&ovi, sizeof (OSVERSIONINFO));
	ovi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	if (! GetVersionEx(&ovi))
	{
		hr = GetLastError();
		DPL(0, "Couldn't get OS version!", 0);
		goto ERROR_EXIT;
	} // end if (get os version)

	if (ovi.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		DPL(0, "WARNING: Symbol resolution unavailable on non NT based platforms (OS version = %i.%i.%i).",
			3, HIBYTE(HIWORD(ovi.dwBuildNumber)), LOBYTE(HIWORD(ovi.dwBuildNumber)),
			LOWORD(ovi.dwBuildNumber));

		g_fTNSymSomeFuncsUnavailable = TRUE;
		return (S_OK);
	} // end if (version 4 OS)
	

	g_symbolsloaded.RemoveAll();


#ifndef USE_STATIC_DLL_LINKS
	g_hImageHlpDLL = LoadLibrary("imagehlp.dll");
	if (g_hImageHlpDLL == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't load IMAGEHLP.DLL!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't load imagehlp)

	g_hNTDLL = LoadLibrary("ntdll.dll");
	if (g_hNTDLL == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't load NTDLL.DLL!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't load ntdll)


	// Grab pointers to the procedures we may use

	g_pfnSymInitialize = (SYMINITIALIZEPROC) GetProcAddress(g_hImageHlpDLL,
															"SymInitialize");
	if (g_pfnSymInitialize == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"SymInitialize\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)

	g_pfnSymSetOptions = (SYMSETOPTIONSPROC) GetProcAddress(g_hImageHlpDLL,
															"SymSetOptions");
	if (g_pfnSymSetOptions == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"SymSetOptions\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)

	g_pfnSymLoadModule = (SYMLOADMODULEPROC) GetProcAddress(g_hImageHlpDLL,
															"SymLoadModule");
	if (g_pfnSymLoadModule == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"SymLoadModule\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)

	g_pfnSymCleanup = (SYMCLEANUPPROC) GetProcAddress(g_hImageHlpDLL,
														"SymCleanup");
	if (g_pfnSymCleanup == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"SymCleanup\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)

#ifdef _WIN64
	g_pfnSymGetSymFromAddr64 = (SYMGETSYMFROMADDR64PROC) GetProcAddress(g_hImageHlpDLL,
																	"SymGetSymFromAddr64");
	if (g_pfnSymGetSymFromAddr64 == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"SymGetSymFromAddr64\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)
#else // _WIN64
	g_pfnSymGetSymFromAddr = (SYMGETSYMFROMADDRPROC) GetProcAddress(g_hImageHlpDLL,
																	"SymGetSymFromAddr");
	if (g_pfnSymGetSymFromAddr == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"SymGetSymFromAddr\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)
#endif // _WIN64

	/*
	g_pfnSymGetSearchPath = (SYMGETSEARCHPATHPROC) GetProcAddress(g_hImageHlpDLL,
																	"SymGetSearchPath");
	if (g_pfnSymGetSearchPath == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"SymGetSearchPath\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)

	g_pfnSymSetSearchPath = (SYMSETSEARCHPATHPROC) GetProcAddress(g_hImageHlpDLL,
																	"SymSetSearchPath");
	if (g_pfnSymSetSearchPath == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"SymSetSearchPath\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)
	*/

	g_pfnImageLoad = (IMAGELOADPROC) GetProcAddress(g_hImageHlpDLL,
													"ImageLoad");
	if (g_pfnImageLoad == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"ImageLoad\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)

	g_pfnImageUnload = (IMAGEUNLOADPROC) GetProcAddress(g_hImageHlpDLL,
														"ImageUnload");
	if (g_pfnImageUnload == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"ImageUnload\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)

	g_pfnStackWalk = (STACKWALKPROC) GetProcAddress(g_hImageHlpDLL,
													"StackWalk");
	if (g_pfnStackWalk == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"StackWalk\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)

	g_pfnSymFunctionTableAccess = (SYMFUNCTIONTABLEACCESSPROC) GetProcAddress(g_hImageHlpDLL,
																			"SymFunctionTableAccess");
	if (g_pfnSymFunctionTableAccess == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"SymFunctionTableAccess\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)

	g_pfnSymGetModuleBase = (SYMGETMODULEBASEPROC) GetProcAddress(g_hImageHlpDLL,
																	"SymGetModuleBase");
	if (g_pfnSymGetModuleBase == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"SymGetModuleBase\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)
	
	g_pfnRtlGetCallersAddress = (RTLGETCALLERSADDRESSPROC) GetProcAddress(g_hNTDLL,
																			"RtlGetCallersAddress");
	if (g_pfnRtlGetCallersAddress == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't get \"RtlGetCallersAddress\" function address!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't retrieve function)
#else // USE_STATIC_DLL_LINKS
	g_pfnSymInitialize = SymInitialize;
	g_pfnSymSetOptions = SymSetOptions;
	g_pfnSymLoadModule = SymLoadModule;
	g_pfnSymCleanup = SymCleanup;
	g_pfnSymGetSymFromAddr = SymGetSymFromAddr;
	/*
	g_pfnSymGetSearchPath = SymGetSearchPath;
	g_pfnSymSetSearchPath = SymSetSearchPath;
	*/
	g_pfnImageLoad = ImageLoad;
	g_pfnImageUnload = ImageUnload;
	g_pfnStackWalk = StackWalk;
	g_pfnSymFunctionTableAccess = SymFunctionTableAccess;
	g_pfnSymGetModuleBase = SymGetModuleBase;
	g_pfnRtlGetCallersAddress = RtlGetCallersAddress;
#endif // USE_STATIC_DLL_LINKS


	// Start IMAGEHLP.DLL cookin.  We say FALSE (don't invade process) because
	// setting it to TRUE never works.  #$%!   Who knows.............
	if (! g_pfnSymInitialize(g_hTNSymProcess, NULL, FALSE))
	{
		hr = GetLastError();
		DPL(0, "SymInitialize is a failure!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't initialize image help DLL)


	// We always want to defer loading the symbols.
	g_pfnSymSetOptions(SYMOPT_DEFERRED_LOADS);

	/*
	// We could change the symbol search path if we want...
	{
		char	szPath[1024];


		if (! g_pfnSymGetSearchPath(GetCurrentProcess(), szPath, 1024))
		{
			hr = GetLastError();
			DPL(0, "SymGetSearchPath is a failure!", 0);
			goto ERROR_EXIT;
		}
		
		DPL(0, "Symbol path currently = \"%s\"", 1, szPath);
	}
	*/


	// Now load the default symbols we want to get

	hr = TNSymLoadSymbolsForModules(_pgmptr);
	if (hr != S_OK)
	{
		DPL(0, "Loading symbols for current process (\"%s\") failed!", 1, _pgmptr);
		goto ERROR_EXIT;
	} // end if (couldn't load current process's symbols)

	hr = TNSymLoadSymbolsForModules("kernel32.dll;user32.dll;ntdll.dll");
	if (hr != S_OK)
	{
		DPL(0, "Couldn't load default symbols (\"kernel32.dll;user32.dll;ntdll.dll\")!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't load default DLL symbols)

	return (S_OK);


ERROR_EXIT:

	// Ignore errors, since we already have one
	TNSymCleanup();

	if (hr == S_OK)
	{
		DPL(0, "WARNING: Converting TNSymInitalize failure of S_OK to E_FAIL!", 0);
		hr = E_FAIL;
	} // end if (failure)

	return (hr);
} // TNSymInitialize
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX // no symbol initialization
#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNSymCleanup()"
//==================================================================================
// TNSymCleanup
//----------------------------------------------------------------------------------
//
// Description: Cleans up the symbol functions (balances out a TNSymInitialize
//				call).
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNSymCleanup(void)
{
	HRESULT		hr = S_OK;



	if (g_dwTNSymInits == 0)
	{
		DPL(0, "No TNSymInitialize to balance this TNSymCleanup!  DEBUGBREAK()-ing.", 0);
		DEBUGBREAK();
		return (E_FAIL);
	} // end if (no references)

	g_dwTNSymInits--;
	// If there are still more outstanding users, we're done.
	if (g_dwTNSymInits > 0)
		return (S_OK);


#ifndef USE_STATIC_DLL_LINKS
	if (g_hImageHlpDLL != NULL)
	{
		if (g_pfnSymCleanup != NULL)
		{
			// Ignore error
			g_pfnSymCleanup(g_hTNSymProcess);
		} // end if (we successfully called SymInitialize)

		FreeLibrary(g_hImageHlpDLL);
		g_hImageHlpDLL = NULL;
	} // end if (we loaded the DLL)

	if (g_hNTDLL != NULL)
	{
		FreeLibrary(g_hNTDLL);
		g_hNTDLL = NULL;
	} // end if (we loaded the DLL)
#else // USE_STATIC_DLL_LINKS
	if (g_pfnSymCleanup != NULL)
	{
		// Ignore error
		g_pfnSymCleanup(g_hTNSymProcess);
	} // end if (we successfully called SymInitialize)
#endif // USE_STATIC_DLL_LINKS

	g_pfnSymInitialize = NULL;
	g_pfnSymSetOptions = NULL;
	g_pfnSymLoadModule = NULL;
	g_pfnSymCleanup = NULL;
#ifdef _WIN64
	g_pfnSymGetSymFromAddr64 = NULL;
#else // _WIN64
	g_pfnSymGetSymFromAddr = NULL;
#endif // _WIN64
	/*
	g_pfnSymGetSearchPath = NULL;
	g_pfnSymSetSearchPath = NULL;
	*/
	g_pfnImageLoad = NULL;
	g_pfnImageUnload = NULL;
	g_pfnStackWalk = NULL;
	g_pfnSymFunctionTableAccess = NULL;
	g_pfnSymGetModuleBase = NULL;
	g_pfnRtlGetCallersAddress = NULL;


	if (g_hTNSymProcess != NULL)
	{
		CloseHandle(g_hTNSymProcess);
		g_hTNSymProcess = NULL;
	} // end if (we have a process handle)

	g_symbolsloaded.RemoveAll();

	return (S_OK);
} // TNSymCleanup
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNSymSetOptions()"
//==================================================================================
// TNSymSetOptions
//----------------------------------------------------------------------------------
//
// Description: Sets options for symbol resolution.
//
// Arguments:
//	BOOL fResolveUndecorated	Whether symbol names should be resolved as
//								undecorated or not (default is FALSE).
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNSymSetOptions(BOOL fResolveUndecorated)
{
	DWORD	dwOptions;


	if (g_fTNSymSomeFuncsUnavailable)
		return (S_OK);
		
	// We always want to defer loading the symbols.
	dwOptions = SYMOPT_DEFERRED_LOADS;

	if (fResolveUndecorated)
		dwOptions |= SYMOPT_UNDNAME;

	g_pfnSymSetOptions(dwOptions);

	return (S_OK);
} // TNSymSetOptions
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#ifndef _XBOX // not supporting symbol lookup
#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNSymResolve()"
//==================================================================================
// TNSymResolve
//----------------------------------------------------------------------------------
//
// Description: Attempts to resolve the given pointer to a symbol and/or offset. The
//				result is stored in the specified string.  If no symbol was found or
//				symbol resolution wasn't initialized, the value of the pointer (in
//				hexadecimal) is written to the string.
//
// Arguments:
//	PVOID pvPointer		Pointer to attempt to resolve.
//	char* szString		String to store result in.
//	DWORD dwStringSize	Size of string buffer, including room for NULL termination.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNSymResolve(PVOID pvPointer, char* szString, DWORD dwStringSize)
{
	HRESULT				hr = S_OK;
#ifdef _WIN64
	PIMAGEHLP_SYMBOL64	pSym64 = NULL;
#else // _WIN64
	PIMAGEHLP_SYMBOL	pSym = NULL;
#endif // _WIN64
	DWORD_PTR			dwOffset;
	char				szModuleName[1024];


	/*
	if (g_fTNSymSomeFuncsUnavailable)
	{
		wsprintf(szString, "0x" SPRINTF_PTR, lpvPointer);
		return (S_OK);
	} // end if (unavailable)
	*/

	if (g_dwTNSymInits == 0) 
	{
		DPL(0, "TNSymInitialize was not successfully called!", 0);
		hr = TNSYMERR_UNINITIALIZED;

		// We're still going to write the pointer's value, though.
		wsprintf(szString, "0x" SPRINTF_PTR, pvPointer);

		goto DONE;
	} // end if (sym not initialized)

	if (pvPointer != NULL)
	{
		ZeroMemory(szModuleName, 1024);

		hr = TNSymGetModuleNameAndOffsetForPointer(pvPointer, szModuleName,
													1024, &dwOffset);
		if ((hr != S_OK) || (strcmp(szModuleName, "") == 0))
		{
			/*
			DPL(0, "Failed getting module name for pointer %x!", 1, pvPointer);
			goto DONE;
			*/

			// Just print the raw pointer directly
			wsprintf(szString, "0x" SPRINTF_PTR, pvPointer);
			hr = S_OK;
		} // end if (couldn't get a module name for pointer)
		else
		{
			if (g_fTNSymSomeFuncsUnavailable)
			{
				wsprintf(szString, "%s + 0x" SPRINTF_PTR " (0x" SPRINTF_PTR ")", szModuleName,
						dwOffset, pvPointer);
			} // end if (symbol functions not available)
			else
			{
#ifdef _WIN64
				pSym64 = (PIMAGEHLP_SYMBOL64) LocalAlloc(LPTR, sizeof (IMAGEHLP_SYMBOL64) + dwStringSize);
				if (pSym64 == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)
				pSym64->SizeOfStruct = sizeof (IMAGEHLP_SYMBOL64) + dwStringSize;
				pSym64->MaxNameLength = dwStringSize - 1;

				if (g_pfnSymGetSymFromAddr64(g_hTNSymProcess, (DWORD_PTR) pvPointer, &dwOffset, pSym64))
				{
					wsprintf(szString, "%s!%s + 0x" SPRINTF_PTR " (0x" SPRINTF_PTR ")",
							szModuleName, pSym64->Name, dwOffset, pvPointer);
				} // end if (get the symbol was successful)
#else // _WIN64
				pSym = (PIMAGEHLP_SYMBOL) LocalAlloc(LPTR, sizeof (IMAGEHLP_SYMBOL) + dwStringSize);
				if (pSym == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)
				pSym->SizeOfStruct = sizeof (IMAGEHLP_SYMBOL) + dwStringSize;
				pSym->MaxNameLength = dwStringSize - 1;

				if (g_pfnSymGetSymFromAddr(g_hTNSymProcess, (DWORD_PTR) pvPointer, &dwOffset, pSym))
				{
					wsprintf(szString, "%s!%s + 0x" SPRINTF_PTR " (0x" SPRINTF_PTR ")",
							szModuleName, pSym->Name, dwOffset, pvPointer);
				} // end if (get the symbol was successful)
#endif // _WIN64
				else
				{
					hr = GetLastError();
					if ((hr == ERROR_MOD_NOT_FOUND) || (hr == ERROR_INVALID_ADDRESS))
					{
						wsprintf(szString, "%s + 0x" SPRINTF_PTR " (0x" SPRINTF_PTR ")", szModuleName,
								dwOffset, pvPointer);

						// It was an error we handled, so consider it "okay"
						hr = S_OK;
					} // end if (there just wasn't a symbol there)
					else
					{
						DPL(0, "SymGetSymFromAddr (%x) is a failure!", 1, pvPointer);
						goto DONE;
					} // end else (some other error)
				} // end else (failed to find a symbol)
			} // end else (symbol functions available)
		} // end else (successfully got module name)
	} // end if (the memory is not NULL)
	else
	{
		strcpy(szString, "0x00000000");
	} // end else (the memory is NULL)

	

DONE:

#ifdef _WIN64
	if (pSym64 != NULL)
	{
		LocalFree(pSym64);
		pSym64 = NULL;
	} // end if (have symbol object)
#else // _WIN64
	if (pSym != NULL)
	{
		LocalFree(pSym);
		pSym = NULL;
	} // end if (have symbol object)
#endif // _WIN64

	return (hr);
} // TNSymResolve
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#else // ! XBOX
//#pragma BUGBUG(tristanj, "May need to come up with an alternative for symbol lookup")
#endif // XBOX



#ifndef _XBOX // no symbol initialization
#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNSymLoadSymbolsForModules()"
//==================================================================================
// TNSymLoadSymbolsForModules
//----------------------------------------------------------------------------------
//
// Description: ?
//
// Arguments:
//	char* szModulesList		List of paths to modules to load symbols for, separated
//							by semicolons.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNSymLoadSymbolsForModules(char* szModulesList)
{
	HRESULT			hr = S_OK;
	char*			pszFields = NULL;
	char*			pszSymbol;
	DWORD			dwTemp;
	DWORD			dwNumFields;


	if (g_fTNSymSomeFuncsUnavailable)
		return (S_OK);
		
	if (g_dwTNSymInits == 0) 
	{
		DPL(0, "TNSymInitialize was not successfully called!", 0);
		hr = TNSYMERR_UNINITIALIZED;
		goto DONE;
	} // end if (sym not initialized)

	/*
	{
		char	szTemp[1024];


		if (g_pfnSymGetSearchPath(g_hTNSymProcess, szTemp, 1024))
		{
			DPL(0, "Sym search path = \"%s\"", 1, szTemp);
		}
	}
	*/

	dwTemp = strlen(szModulesList) + 1;
	pszFields = (char*) LocalAlloc(LPTR, dwTemp);
	if (pszFields == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	dwNumFields = StringSplitIntoFields(szModulesList, ";", pszFields, &dwTemp);

	for(dwTemp = 0; dwTemp < dwNumFields; dwTemp++)
	{
		pszSymbol = StringGetFieldPtr(pszFields, dwTemp);

		// If the symbol hasn't already been loaded, look for it
#pragma BUGBUG(vanceo, "make path tolerant")
		if (g_symbolsloaded.GetStringIndex(pszSymbol, 0, FALSE) < 0)
		{
			PLOADED_IMAGE	pLoadedImage;
			DWORD_PTR		dwModuleAddress;
			DWORD			dwModuleSize;


			DPL(0, "Loading symbols for \"%s\".", 1, pszSymbol);


			dwModuleAddress = (DWORD_PTR) GetModuleHandle(pszSymbol);

			// Load the module to retrieve its size.
			pLoadedImage = g_pfnImageLoad(pszSymbol, NULL);
			if (pLoadedImage == NULL)
			{
				hr = GetLastError();
				DPL(0, "Couldn't load \"%s\" image!", 1, pszSymbol);
				goto DONE;
			} // end if (couldn't load image)
			dwModuleSize = pLoadedImage->SizeOfImage;

			// Ignore error
			g_pfnImageUnload(pLoadedImage);
			pLoadedImage = NULL;


			// Try to load symbols the caller wants to add.
			if (! g_pfnSymLoadModule(g_hTNSymProcess, NULL, pszSymbol, NULL, dwModuleAddress, dwModuleSize))
			{
				hr = GetLastError();

				DPL(0, "Trying to load symbols for \"%s\" failed!  %e", 2, pszSymbol, hr);

				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end if (failed loading symbols using defaults)
			
			hr = g_symbolsloaded.AddString(pszSymbol);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add string \"%s\" to symbols loaded list!", 1, pszSymbol);
				goto DONE;
			} // end if (couldn't add string)
		} // end if ()
		else
		{
			DPL(0, "Already loaded symbols for \"%s\".", 1, pszSymbol);
		} // end else ()
	} // end for (each symbol to load)


DONE:

	if (pszFields != NULL)
	{
		LocalFree(pszFields);
		pszFields = NULL;
	} // end if (allocated string)

	return (hr);
} // TNSymLoadSymbolsForModules
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX // no GetModuleFileName supported, so don't implement this now
#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNSymGetModuleNameAndOffsetForPointer()"
//==================================================================================
// TNSymGetModuleNameAndOffsetForPointer
//----------------------------------------------------------------------------------
//
// Description: Retrieves the name of the module in which the pointer lies, plus its
//				offset from the start of that module and stores it into the string
//				buffer given.
//				This function does not require TNSymInitialize to have been called
//				in order to work.
//
// Arguments:
//	PVOID pvPointer		Pointer to attempt to resolve.
//	char* pszName		String to store result in.
//	DWORD dwNameSize	Size of string buffer, including room for NULL termination.
//	DWORD* pdwOffset	Size of string buffer, including room for NULL termination.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT TNSymGetModuleNameAndOffsetForPointer(PVOID pvPointer, char* pszName,
											DWORD dwNameSize, PDWORD_PTR pdwOffset)
{
	HRESULT						hr;
	MEMORY_BASIC_INFORMATION	mbi;



	ZeroMemory(&mbi, sizeof (MEMORY_BASIC_INFORMATION));
	if (VirtualQuery(pvPointer, &mbi, sizeof (MEMORY_BASIC_INFORMATION)) == 0)
	{
		hr = GetLastError();
		//DPL(0, "VirtualQuery failed!", 0);
		return (hr);
	} // end if (geting pointer information failed)

	if ((pszName != NULL) && (dwNameSize > 0))
	{
		if (mbi.AllocationBase > 0)
		{
			if (GetModuleFileName((HMODULE) mbi.AllocationBase, pszName, dwNameSize) == 0)
			{
				hr = GetLastError();

				// This is just from observation, is this right?
				if (mbi.AllocationBase == GetProcessHeap())
				{
					strcpy(pszName, "ProcessHeap");
					hr = S_OK;
				} // end if (it's part of the process heap)
				else
				{
					//DPL(1, "Failed getting module filename!  %e", 1, hr);

					if (hr == S_OK)
						hr = E_FAIL;

					return (hr);
				} // end else (it's not part of the process heap)
			} // end if (getting module filename failed)
			else
			{
				FileGetNameWithoutExtension(pszName, pszName, FALSE);
				_strupr(pszName);
			} // end else (successfully got module filename)
		} // end if (the memory is in a module)
		else
		{
			strcpy(pszName, "");
		} // end else (the memory is not in a module)
	} // end if (the caller wants the module name)

	if (pdwOffset != NULL)
	{
		(*pdwOffset) = ((LPBYTE) pvPointer) - ((LPBYTE) mbi.AllocationBase);
	} // end if (the caller wants the offset)

	return (S_OK);
} // TNSymGetModuleNameAndOffsetForPointer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNSymGetDisableSymResolutionSetting()"
//==================================================================================
// TNSymGetDisableSymResolutionSetting
//----------------------------------------------------------------------------------
//
// Description: Retrieves the current DisableSymResolution setting.
//
// Arguments: None.
//
// Returns: TRUE if DisableSymResolution is set, FALSE if not.
//==================================================================================
BOOL TNSymGetDisableSymResolutionSetting(void)
{
#ifndef _XBOX // no registry supported
	HRESULT		hr;
	HKEY		hKey = NULL;
	DWORD		dwType;
	DWORD		dwValue;
	DWORD		dwValueSize;

	// Open the registry setting for controlling symbol resolution.
	hr = RegOpenKey(HKEY_LOCAL_MACHINE, REGKEY_TESTNET, &hKey);
	if (hr != ERROR_SUCCESS)
	{
		DPL(1, "WARNING: Couldn't open key HKLM\\%s!  %e",
			2, REGKEY_TESTNET, hr);
		return (FALSE);
	} // end if (couldn't open key)


	dwValueSize = sizeof (DWORD);
	hr = RegQueryValueEx(hKey, REGVALUE_DISABLESYMBOLRESOLUTION, NULL,
						&dwType, (LPBYTE) &dwValue, &dwValueSize);
	if (hr != ERROR_SUCCESS)
	{
		DPL(1, "WARNING: Couldn't query HKLM\\%s value %s!  %e",
			3, REGKEY_TESTNET, REGVALUE_DISABLESYMBOLRESOLUTION, hr);

		// Close the key since we're bailing.
		RegCloseKey(hKey);
		hKey = NULL;

		return (FALSE);
	} // end if (couldn't query value)

	
	// Close the key, we're done with it.
	RegCloseKey(hKey);
	hKey = NULL;

	if (dwType != REG_DWORD)
	{
		DPL(1, "WARNING: HKLM\\%s value %s is not a DWORD!  %e",
			3, REGKEY_TESTNET, REGVALUE_DISABLESYMBOLRESOLUTION, hr);
		return (FALSE);
	} // end if (not a DWORD)

#else // ! XBOX
#pragma TODO(tristanj, "Hardcoded to always disable symbol resolution")
	DWORD		dwValue;
	dwValue = 1;
#endif // XBOX

	// Return the setting
	return ((dwValue != 0) ? TRUE : FALSE);
} // TNSymGetDisableSymResolutionSetting
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNSymSetDisableSymResolutionSetting()"
//==================================================================================
// TNSymSetDisableSymResolutionSetting
//----------------------------------------------------------------------------------
//
// Description: Stores the DisableSymResolution setting specified.
//
// Arguments:
//	BOOL fDisable	Set to TRUE to disable symbol resolution, or FALSE to leave
//					enabled.
//
// Returns: None.
//==================================================================================
void TNSymSetDisableSymResolutionSetting(BOOL fDisable)
{
#ifndef _XBOX // no registry supported
	HRESULT		hr;
	HKEY		hKey = NULL;
	DWORD		dwValue;


	// Open the registry setting for controlling symbol resolution.
	hr = RegCreateKey(HKEY_LOCAL_MACHINE, REGKEY_TESTNET, &hKey);
	if (hr != ERROR_SUCCESS)
	{
		DPL(1, "WARNING: Couldn't create key HKLM\\%s!  %e",
			2, REGKEY_TESTNET, hr);
		return;
	} // end if (couldn't open key)


	dwValue = (fDisable) ? 1 : 0;
	hr = RegSetValueEx(hKey, REGVALUE_DISABLESYMBOLRESOLUTION, 0, REG_DWORD,
						((BYTE*) (&dwValue)), (sizeof (DWORD)));
	if (hr != ERROR_SUCCESS)
	{
		DPL(1, "WARNING: Couldn't save HKLM\\%s value %s!  %e",
			3, REGKEY_TESTNET, REGVALUE_DISABLESYMBOLRESOLUTION, hr);

		// Continuing...
	} // end if (couldn't query value)

	
	// Close the key, we're done with it.
	RegCloseKey(hKey);
	hKey = NULL;
#else // ! XBOX
#pragma TODO(tristanj, "Hardcoded to ignore desired symbol resolution")
#endif // XBOX

} // TNSymSetDisableSymResolutionSetting
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
