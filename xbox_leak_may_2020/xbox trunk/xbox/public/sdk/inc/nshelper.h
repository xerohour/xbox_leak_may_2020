/*++

Copyright (c) 1998-1999  Microsoft Corporation

Module Name:

    nshelper.h

Abstract:
    This file contains definitions which are needed by all NetSh helper DLLs
    which allow other DLLs to register sub-contexts under them.

--*/

#ifndef _NSHELPER_H_
#define _NSHELPER_H_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NS_DLL_TABLE_ENTRY
{
    //
    // Name of the DLL servicing the context
    //

    WCHAR                   pwszDLLName[MAX_NAME_LEN]; // Corresponding DLL

    //
    // Registry value used for this DLL
    //

    WCHAR                   pwszValueName[MAX_NAME_LEN];

    //
    // TRUE if loaded
    //

    BOOL                    bLoaded;                   // In memory or not

    //
    // Handle to DLL instance if loaded
    //

    HANDLE                  hDll;                      // DLL handle if loaded

    //
    // Function to stop this DLL
    //

    PNS_DLL_STOP_FN         pfnStopFn;

}NS_DLL_TABLE_ENTRY,*PNS_DLL_TABLE_ENTRY;

typedef struct _NS_HELPER_TABLE_ENTRY
{
    //
    // GUID associated with the helper
    //

    GUID                    guidHelper;

    //
    // GUID associated with the helper
    //

    GUID                    guidParent;

    //
    // Function (if any) to use to register sub-contexts
    //

    PVOID                   pfnRegisterContext;

    //
    // Function (if any) to use to register sub-contexts
    //

    PNS_DEREG_ALL_CONTEXTS_FN pfnDeregisterAllContexts;

    //
    // Function to start this helper
    //

    PNS_HELPER_START_FN     pfnStart;

    //
    // Function to stop this helper
    //

    PNS_HELPER_STOP_FN      pfnStop;

    //
    // Index of the DLL implementing the helper
    //

    DWORD                   dwDllIndex;

    //
    // TRUE if started
    //

    BOOL                    bStarted;
}NS_HELPER_TABLE_ENTRY,*PNS_HELPER_TABLE_ENTRY;

//
// This structure is a generic version of the attributes
// structure for a context.  Typically, a helper which
// supports subcontexts will define its own context attributes
// structure which is the same form as this structure except
// that the types of the functions may vary.  See the
// attributes structures in ipmontr.h and ipxmontr.h as examples.
//

typedef struct _NS_GENERIC_CONTEXT_ATTRIBUTES
{
#ifdef __cplusplus
    NS_BASE_CONTEXT_ATTRIBUTES bca;
#else
    NS_BASE_CONTEXT_ATTRIBUTES;
#endif

    PVOID                    pfnEntryFn;
    PNS_CONTEXT_COMMIT_FN    pfnCommitFn;
    PVOID                    pfnDumpFn;
    PNS_CONTEXT_CONNECT_FN   pfnConnectFn;
    PVOID                    pfnSubEntryFn;

}NS_GENERIC_CONTEXT_ATTRIBUTES, *PNS_GENERIC_CONTEXT_ATTRIBUTES;

#ifdef __cplusplus
}
#endif

#endif // _NETSH_H_
