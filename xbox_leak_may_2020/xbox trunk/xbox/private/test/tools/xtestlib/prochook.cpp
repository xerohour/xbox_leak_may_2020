/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    prochook.cpp

Abstract:

    Routines to hook imported functions

Author:

    Sakphong Chanbai (schanbai) 05-Jul-2000

Environment:

    Xbox

Revision History:

--*/


#include "precomp.h"
#include "prochook.h"
#include "xeutils.h"

#define MAKEPTR(cast, ptr, addValue) (cast)((DWORD)(ptr)+(DWORD)(addValue))

#ifdef __cplusplus
extern "C" {
#endif


PROC WINAPI HookImportedFunction(
    HMODULE hFromModule,        // Module to intercept calls from
    LPCSTR pszFunctionModule,   // Module to intercept calls to
    LPCSTR pszFunctionName,     // Function to intercept calls to
    PROC pfnNewProc             // New function (replace old function)
    )
{
    // Get the module handle
    HMODULE hModule = (HMODULE)XeModuleHeaderFromModuleName( "xapi.dll" );

    if ( !hModule ) {
        return NULL;
    }

    // Get the function address
    PROC pfn = GetProcAddress( hModule, pszFunctionName );

    if ( !pfn ) {
        return NULL;
    }

    // Hook it
    return HookImportedFunctionEx(
                hFromModule,
                pszFunctionModule,
                pfn,
                pfnNewProc
                );
}


PROC WINAPI HookImportedFunctionEx(
    HMODULE hFromModule,        // Module to intercept calls from
    LPCSTR pszFunctionModule,   // Module to intercept calls to
    PROC pfnProc,               // Function to intercept calls to
    PROC pfnNewProc             // New function (replace old function)
    )
{
    if ( IsBadReadPtr(hFromModule, sizeof(XIMAGE_MODULE_HEADER)) ) {
        return NULL;
    }

    return HookPEImportedFunctionEx(
                ((PXIMAGE_MODULE_HEADER)hFromModule)->ImageBase,
                pszFunctionModule,
                pfnProc,
                pfnNewProc
                );
}


PROC WINAPI HookPEImportedFunctionEx(
    ULONG_PTR BaseAddress,      // Module to intercept calls from
    LPCSTR pszFunctionModule,   // Module to intercept calls to
    PROC pfnProc,               // Function to intercept calls to
    PROC pfnNewProc             // New function (replace old function)
    )
{
    PROC pfnOriginalProc;
    PIMAGE_DOS_HEADER pDosHeader;
    PIMAGE_NT_HEADERS pNTHeader;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
    PIMAGE_THUNK_DATA pThunk;

    // Verify that a valid pfn was passed
    if ( IsBadCodePtr(pfnNewProc) ) {
        return NULL;
    }

    // First, verify the function passed to use are valid
    if ( IsBadCodePtr(pfnProc) ) {
        return NULL;
    }

    pfnOriginalProc = pfnProc;
    pDosHeader = (PIMAGE_DOS_HEADER)BaseAddress;

    // Test to make sure we're looking at a module image (the MZ header)
    if ( IsBadReadPtr(pDosHeader, sizeof(IMAGE_DOS_SIGNATURE)) ) {
        return NULL;
    }

    if ( pDosHeader->e_magic != IMAGE_DOS_SIGNATURE ) {
        return NULL;
    }

    // The MZ header has a pointer to PE signature
    pNTHeader = MAKEPTR(PIMAGE_NT_HEADERS, pDosHeader, pDosHeader->e_lfanew);

    // More tests to make sure we're looking at a "PE" image
    if ( IsBadReadPtr(pNTHeader, sizeof(IMAGE_NT_HEADERS)) ) {
        return NULL;
    }

    //
    // We now have a valid pointer to the module's PE header
    // Now go get a pointer to its imports section
    //
    pImportDesc = MAKEPTR(
        PIMAGE_IMPORT_DESCRIPTOR, 
        pDosHeader, 
        pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
        );

    // Bail out if the RVA of the imports section is 0 (it doesn't exist)
    if ( pImportDesc == (PIMAGE_IMPORT_DESCRIPTOR)pNTHeader ) {
        return NULL;
    }

    //
    // Iterate through the array of imported module descriptor
    // Looking for the module whose name matches the pszFunctionModule parameter
    //
    while ( pImportDesc->Name ) {
        LPSTR pszModName = MAKEPTR( PSTR, pDosHeader, pImportDesc->Name );

        if ( _stricmp(pszModName, pszFunctionModule) == 0 ) {
            break;
        }

        // Advance to next imported module descriptor
        pImportDesc++;
    }

    // Bail out if we didn't find the import module descriptor
    if ( pImportDesc->Name == 0 ) {
        return NULL;
    }

    // Get a pointer to the found Module's import address table (IAT)
    pThunk = MAKEPTR( PIMAGE_THUNK_DATA, pDosHeader, pImportDesc->FirstThunk );

    //
    // Blast through the table of import addresses and looking for the one
    // that matches the address we got back from GetProcAddress
    //
    while ( pThunk->u1.Function )
    {
        if ( pThunk->u1.Function == (ULONG_PTR)pfnOriginalProc ) {
            // We found it! Overwrite the original address with the
            // address of interception function

            MEMORY_BASIC_INFORMATION mbi;
            DWORD oldFlags, newFlags;
            DWORD dwScratch;

            // Get original protection attribute
            VirtualQuery(&pThunk->u1.Function, &mbi, sizeof(mbi));
            oldFlags = mbi.Protect;
            newFlags = mbi.Protect;

            // Tweak to writeable
            newFlags &= ~PAGE_READONLY;
            newFlags &= ~PAGE_EXECUTE_READ;
            newFlags |= PAGE_READWRITE;

            // Modify the page's protection attributes
            if ( !VirtualProtect(&pThunk->u1.Function, sizeof(PDWORD), newFlags, &dwScratch) ) {
                //BUGBUG: handle errror here
                //DWORD dwError = GetLastError();
            }

            BOOL fFaulted = FALSE;

            __try
            {
                pThunk->u1.Function = (ULONG_PTR)pfnNewProc;
            }
            __except(1)
            {
                fFaulted = TRUE;
            }

            VirtualProtect(&pThunk->u1.Function, sizeof(PDWORD), oldFlags, &dwScratch);

            if (!fFaulted) {
                return pfnOriginalProc;
            } else {
                return NULL;
            }
        }

        // Advance to next imported function table
        pThunk++;
    }

    return NULL;
}


#ifdef __cplusplus
}
#endif
