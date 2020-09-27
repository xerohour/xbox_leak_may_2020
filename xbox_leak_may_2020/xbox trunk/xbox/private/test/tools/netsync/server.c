/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  server.c

Abstract:

  This module handles the netsync server logic

Author:

  Steven Kehrli (steveke) 15-Nov-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



#ifdef _XBOX

#pragma data_seg( ".SYNCEXP$A" )
NETSYNC_EXPORT_TABLE_DIRECTORY __netsync_export_table_directory_start[] = { NULL };

#pragma data_seg( ".SYNCEXP$Z" )
NETSYNC_EXPORT_TABLE_DIRECTORY __netsync_export_table_directory_end[] = { NULL };

#pragma data_seg()

#pragma comment(linker, "/merge:.SYNCEXP=.export")

#endif // _XBOX



using namespace NetsyncNamespace;

namespace NetsyncNamespace {

// g_hNetsyncServerInfoMutex is the object to synchronize access to the netsync server info
HANDLE               g_hNetsyncServerInfoMutex;
// g_NetsyncServerInfo is the netsync server info
NETSYNC_SERVER_INFO  g_NetsyncServerInfo = {
                                             INVALID_HANDLE_VALUE, // hMemObject
                                             0,                    // dwRefCount
                                             NULL,                 // pNetsyncObject
                                             NULL,                 // hThread
                                             0,                    // nServerClientCount
                                             0,                    // bySessionCount
                                             NULL,                 // pSessionDlls
                                             FALSE,                // bStopServer
                                             NULL,                 // hStopEvent
                                             { FALSE },            // bPortInUse[NETSYNC_PORT_SECTIONS]
                                             { FALSE },            // bPortUsed[NETSYNC_PORT_SECTIONS]
                                            };



#ifdef _XBOX

FARPROC
FindProcAddress(
    IN HMODULE  hModule,
    IN LPSTR    lpszFunction_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Finds the address of the specified function

Arguments:

  hModule - Pointer to the module's NETSYNC_EXPORT_TABLE_DIRECTORY
  lpszFunction_A - Pointer to a null-terminated string (ANSI) that specifies the function name

Return Value:

  FARPROC:
    If the function succeeds, the return value is a pointer the function address
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pTableEntry is a pointer to a table entry
    PNETSYNC_EXPORT_TABLE_ENTRY  pTableEntry = ((PNETSYNC_EXPORT_TABLE_DIRECTORY) hModule)->pTableEntry;
    // pFunction is a pointer to the function address
    FARPROC                      pFunction = NULL;
    // dwErrorCode is the last error code
    DWORD                        dwErrorCode = ERROR_SUCCESS;



    // Search each table entry for the specified function
    while ((NULL != pTableEntry->lpszFunction_A) && (NULL != pTableEntry->pFunction)) {
        if (0 == strcmp(lpszFunction_A, pTableEntry->lpszFunction_A)) {
            pFunction = pTableEntry->pFunction;

            break;
        }

        pTableEntry++;
    }

    // Specified function was not found
    if (NULL == pFunction) {
        dwErrorCode = ERROR_PROC_NOT_FOUND;
    }

    if (ERROR_SUCCESS != dwErrorCode) {
        SetLastError(dwErrorCode);
    }

    return pFunction;
}



PNETSYNC_EXPORT_TABLE_DIRECTORY
FindModuleAddress(
    IN HANDLE  hMemObject,
    IN LPWSTR  lpszDllName_W
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Finds the specified dll and calls its entry point

Arguments:

  hMemObject - Handle to the memory object
  lpszDllName_W - Pointer to a null-terminated string (UNICODE) that specifies the dll name

Return Value:

  PNETSYNC_EXPORT_TABLE_DIRECTORY:
    If the function succeeds, the return value is a pointer to its NETSYNC_EXPORT_TABLE_DIRECTORY.
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pTableDir is a pointer to a table directory
    PNETSYNC_EXPORT_TABLE_DIRECTORY   pTableDir = __netsync_export_table_directory_start;
    // pLastTableDir is a pointer to the last table directory
    PNETSYNC_EXPORT_TABLE_DIRECTORY   pLastTableDir = __netsync_export_table_directory_end;
    // pDllMain is a pointer to the dll entry point
    LPDLLMAIN_PROC                    pDllMain = NULL;

    // lpszModules is a pointer to the list of modules
    LPWSTR                            lpszModules = NULL;
    // pDot3 is a pointer to the dot 3 portion of the dll name
    PWCHAR                            pDot3 = NULL;

    // dwErrorCode is the last error code
    DWORD                             dwErrorCode = ERROR_SUCCESS;



    // Strip ".dll" or ".lib" from lpszDllName_W
    pDot3 = wcsrchr(lpszDllName_W, L'.');
    if (NULL != pDot3) {
        if ((lstrcmpi(L".dll", pDot3)) || (lstrcmpi(L".lib", pDot3))) {
            *pDot3 = L'\0';
        }
        else {
            pDot3 = NULL;
        }
    }

    // Get the modules
    lpszModules = GetIniSection(hMemObject, L"syncsrv");

    // Find the module
    if ((NULL == lpszModules) || (FALSE == FindString(lpszModules, lpszDllName_W))) {
        dwErrorCode = ERROR_MOD_NOT_FOUND;

        goto ExitFunc;
    }

    // Search each table directory for the specified dll
    while (pTableDir < pLastTableDir) {
        if ((NULL != pTableDir->lpszModuleName) && (0 == lstrcmp(lpszDllName_W, pTableDir->lpszModuleName))) {
            pDllMain = (LPDLLMAIN_PROC) FindProcAddress((HMODULE) pTableDir, "DllMain");
            if (NULL != pDllMain) {
                pDllMain(NULL, DLL_PROCESS_ATTACH, NULL);
            }

            break;
        }

        pTableDir++;
    }

    // Specified dll was not found
    if (pTableDir == pLastTableDir) {
        dwErrorCode = ERROR_MOD_NOT_FOUND;
    }

ExitFunc:
    if (NULL != pDot3) {
        *pDot3 = L'.';
    }

    if (NULL != lpszModules) {
        xMemFree(hMemObject, lpszModules);
    }

    if (ERROR_SUCCESS != dwErrorCode) {
        SetLastError(dwErrorCode);
    }

    return (ERROR_SUCCESS == dwErrorCode) ? pTableDir : NULL;
}

#endif



PNETSYNC_SESSION_DLL
FindSessionDll(
    IN HANDLE                hMemObject,
    IN PNETSYNC_SESSION_DLL  *pSessionDlls,
    IN LPWSTR                lpszDllName_W
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Finds the specified dll name in the list of session dlls

Arguments:

  hMemObject - Handle to the memory object
  pSessionDlls - Pointer to the head of the list of session dlls
  lpszDllName_W - Pointer to a null-terminated string (UNICODE) that specifies the dll name

Return Value:

  PNETSYNC_SESSION_DLL:
    If the dll is found, the return value is a pointer to the session dll within the list.
    If the dll is not found, the return value is NULL.

------------------------------------------------------------------------------*/
{
    // pSessionDll is a pointer to the session dll
    PNETSYNC_SESSION_DLL  pSessionDll = NULL;
    // iErrorCode is the last error code
    int                   iErrorCode = ERROR_SUCCESS;



    for (pSessionDll = *pSessionDlls; NULL != pSessionDll; pSessionDll = pSessionDll->pNextSessionDll) {
        // Check the dll name
        if (0 == lstrcmp(lpszDllName_W, pSessionDll->lpszDllName_W)) {
            break;
        }
    }

    if (NULL != pSessionDll) {
        return pSessionDll;
    }

    // Allocate the memory for the new session dll
    pSessionDll = (PNETSYNC_SESSION_DLL) xMemAlloc(hMemObject, sizeof(NETSYNC_SESSION_DLL) + (lstrlen(lpszDllName_W) + 1) * sizeof(WCHAR));
    if (NULL == pSessionDll) {
        iErrorCode = GetLastError();

        goto FunctionFailed;
    }

    // Load the dll
#ifdef _XBOX
    pSessionDll->hModule = (HMODULE) FindModuleAddress(hMemObject, lpszDllName_W);
#else
    pSessionDll->hModule = LoadLibrary(lpszDllName_W);
#endif
    if (NULL == pSessionDll->hModule) {
        iErrorCode = GetLastError();

        goto FunctionFailed;
    }

    // Copy the dll name
    pSessionDll->lpszDllName_W = (LPWSTR) ((UINT_PTR) pSessionDll + sizeof(NETSYNC_SESSION_DLL));
    lstrcpy(pSessionDll->lpszDllName_W, lpszDllName_W);

    // Set the pointer to the next session dll to the head of the session dll list
    pSessionDll->pNextSessionDll = *pSessionDlls;

    // Set the head of the session dll list to the new session dll
    *pSessionDlls = pSessionDll;

    return pSessionDll;

FunctionFailed:
    // Free the new session dll, if necessary
    if (NULL != pSessionDll) {
        xMemFree(hMemObject, pSessionDll);
    }

    // Set the last error code
    SetLastError(iErrorCode);

    return NULL;
}



PNETSYNC_SESSION_INFO
FindSessionInfo(
    IN HANDLE                 hMemObject,
    IN PNETSYNC_SESSION_INFO  *pSessionInfos,
    IN HMODULE                hModule,
    IN DWORD                  dwSessionType,
    IN BYTE                   byMinClientCount,
    IN BYTE                   byMaxClientCount,
    IN u_short                nMinPortCount,
    IN LPSTR                  lpszMainFunction_A,
    IN LPSTR                  lpszStartFunction_A,
    IN LPSTR                  lpszStopFunction_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Finds the specified session info in the list of session infos

Arguments:

  hMemObject - Handle to the memory object
  pSessionInfos - Pointer to the head of the list of session infos
  hModule - Handle to the module of the dll
  dwSessionType - Specifies the session type
  byMinClientCount - Specifies the minimum client count for the session
  byMaxClientCount - Specifies the maximum client count for the session
  nMinPortCount - Specifies the minimum port count for the session
  lpszMainFunction_A - Pointer to a null-terminated string (ANSI) that specifies the main function name
  lpszStartFunction_A - Pointer to a null-terminated string (ANSI) that specifies the start function name
  lpszStopFunction_A - Pointer to a null-terminated string (ANSI) that specifies the stop function name

Return Value:

  PNETSYNC_SESSION_INFO:
    If the dll is found, the return value is a pointer to the session info within the list.
    If the dll is not found, the return value is NULL.

------------------------------------------------------------------------------*/
{
    // pSessionInfo is a pointer to the session info
    PNETSYNC_SESSION_INFO  pSessionInfo;
    // dwBufferSize is the size of the new session info buffer
    DWORD                  dwBufferSize = sizeof(NETSYNC_SESSION_INFO);
    // dwOffset is the offset into the new session info buffer
    DWORD                  dwOffset = sizeof(NETSYNC_SESSION_INFO);
    // iErrorCode is the last error code
    int                    iErrorCode = ERROR_SUCCESS;



    for (pSessionInfo = *pSessionInfos; NULL != pSessionInfo; pSessionInfo = pSessionInfo->pNextSessionInfo) {
        // Check the session type
        if (dwSessionType != pSessionInfo->dwSessionType) {
            continue;
        }

        // Check the function name
        if (0 != strcmp(lpszMainFunction_A, pSessionInfo->lpszMainFunction_A)) {
            continue;
        }

        if (NETSYNC_SESSION_CALLBACK == dwSessionType) {
            // Check the start function name
            if (0 != strcmp(lpszStartFunction_A, pSessionInfo->lpszStartFunction_A)) {
                continue;
            }

            // Check the stop function name
            if (0 != strcmp(lpszStopFunction_A, pSessionInfo->lpszStopFunction_A)) {
                continue;
            }
        }

        break;
    }

    if (NULL != pSessionInfo) {
        return pSessionInfo;
    }

    // Determine the memory required by the new session info
    dwBufferSize += (strlen(lpszMainFunction_A) + 1);

    if (NETSYNC_SESSION_CALLBACK == dwSessionType) {
        dwBufferSize += (strlen(lpszStartFunction_A) + 1);
        dwBufferSize += (strlen(lpszStopFunction_A) + 1);
    }

    // Allocate the memory for the new session info
    pSessionInfo = (PNETSYNC_SESSION_INFO) xMemAlloc(hMemObject, dwBufferSize);
    if (NULL == pSessionInfo) {
        iErrorCode = GetLastError();

        goto FunctionFailed;
    }

    // Load the functions
    if (NETSYNC_SESSION_THREAD == dwSessionType) {
#ifdef _XBOX
        pSessionInfo->pThreadFunction = (PNETSYNC_THREAD_FUNCTION) FindProcAddress(hModule, lpszMainFunction_A);
#else
        pSessionInfo->pThreadFunction = (PNETSYNC_THREAD_FUNCTION) GetProcAddress(hModule, lpszMainFunction_A);
#endif
        if (NULL == pSessionInfo->pThreadFunction) {
            iErrorCode = GetLastError();

            goto FunctionFailed;
        }
    }
    else {
#ifdef _XBOX
        pSessionInfo->pCallbackFunction = (PNETSYNC_CALLBACK_FUNCTION) FindProcAddress(hModule, lpszMainFunction_A);
#else
        pSessionInfo->pCallbackFunction = (PNETSYNC_CALLBACK_FUNCTION) GetProcAddress(hModule, lpszMainFunction_A);
#endif
        if (NULL == pSessionInfo->pCallbackFunction) {
            iErrorCode = GetLastError();

            goto FunctionFailed;
        }

#ifdef _XBOX
        pSessionInfo->pStartFunction = (PNETSYNC_START_FUNCTION) FindProcAddress(hModule, lpszStartFunction_A);
#else
        pSessionInfo->pStartFunction = (PNETSYNC_START_FUNCTION) GetProcAddress(hModule, lpszStartFunction_A);
#endif
        if (NULL == pSessionInfo->pStartFunction) {
            iErrorCode = GetLastError();

            goto FunctionFailed;
        }

#ifdef _XBOX
        pSessionInfo->pStopFunction = (PNETSYNC_STOP_FUNCTION) FindProcAddress(hModule, lpszStopFunction_A);
#else
        pSessionInfo->pStopFunction = (PNETSYNC_STOP_FUNCTION) GetProcAddress(hModule, lpszStopFunction_A);
#endif
        if (NULL == pSessionInfo->pStopFunction) {
            iErrorCode = GetLastError();

            goto FunctionFailed;
        }
    }

    // Copy the session type
    pSessionInfo->dwSessionType = dwSessionType;

    // Copy the client info
    pSessionInfo->byMinClientCount = byMinClientCount;
    pSessionInfo->byMaxClientCount = byMaxClientCount;

    if (pSessionInfo->byMinClientCount < 1) {
        pSessionInfo->byMinClientCount = 1;
    }

    if (pSessionInfo->byMaxClientCount < pSessionInfo->byMinClientCount) {
        pSessionInfo->byMaxClientCount = pSessionInfo->byMinClientCount;
    }

    // Copy the port info
    pSessionInfo->nMinPortCount = nMinPortCount;

    // Copy the function names
    pSessionInfo->lpszMainFunction_A = (LPSTR) ((UINT_PTR) pSessionInfo + dwOffset);
    strcpy(pSessionInfo->lpszMainFunction_A, lpszMainFunction_A);
    dwOffset += (strlen(pSessionInfo->lpszMainFunction_A) + 1);

    if (NETSYNC_SESSION_CALLBACK == dwSessionType) {
        pSessionInfo->lpszStartFunction_A = (LPSTR) ((UINT_PTR) pSessionInfo + dwOffset);
        strcpy(pSessionInfo->lpszStartFunction_A, lpszStartFunction_A);
        dwOffset += (strlen(pSessionInfo->lpszStartFunction_A) + 1);

        pSessionInfo->lpszStopFunction_A = (LPSTR) ((UINT_PTR) pSessionInfo + dwOffset);
        strcpy(pSessionInfo->lpszStopFunction_A, lpszStopFunction_A);
        dwOffset += (strlen(pSessionInfo->lpszStopFunction_A) + 1);
    }

    // Set the pointer to the next session info to the head of the session info list
    pSessionInfo->pNextSessionInfo = *pSessionInfos;

    // Set the head of the session info list to the new session info
    *pSessionInfos = pSessionInfo;

    return pSessionInfo;

FunctionFailed:
    // Free the new session info, if necessary
    if (NULL != pSessionInfo) {
        xMemFree(hMemObject, pSessionInfo);
    }

    // Set the last error code
    SetLastError(iErrorCode);

    return NULL;
}



BOOL
DoesSessionInfoExist(
    IN  HANDLE                    hMemObject,
    IN  PNETSYNC_SESSION_MESSAGE  pSessionMessage,
    OUT BYTE                      *bySessionClientCount
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Checks the list of session infos for the specified session info

Arguments:

  hMemObject - Handle to the memory object
  pSessionMessage - Pointer to a session message
  bySessionCount - Pointer to the number of clients in the session

Return Value:

  BOOL:
    If the specified session is found, the return value is nonzero.
    If the specified session is not found or cannot be loaded, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pSessionDll is a pointer to the session dll
    PNETSYNC_SESSION_DLL   pSessionDll = NULL;
    // pSessionInfo is a pointer to the session info
    PNETSYNC_SESSION_INFO  pSessionInfo = NULL;
    // lpszDllName_W is a pointer to szDllName_W within the session message
    LPWSTR                 lpszDllName_W = pSessionMessage->szDllName_W;
    // lpszMainFunction_A is a pointer to szMainFunction_A within the session message
    LPSTR                  lpszMainFunction_A = pSessionMessage->szMainFunction_A;
    // lpszStartFunction_A is a pointer to szStartFunction_A within the session message
    LPSTR                  lpszStartFunction_A = NULL;
    // lpszStopFunction_A is a pointer to szStopFunction_A within the session message
    LPSTR                  lpszStopFunction_A = NULL;
    // iErrorCode is the last error code
    int                    iErrorCode = ERROR_SUCCESS;



    // Wait for the netsync server mutex
    WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);

    if (TRUE == g_NetsyncServerInfo.bStopServer) {
        // Server is stopping
        iErrorCode = ERROR_SERVER_DISABLED;

        goto ExitFunc;
    }

    if (NETSYNC_SESSION_CALLBACK == pSessionMessage->dwSessionType) {
        // Get the pointers to the function names within the session message
        lpszStartFunction_A = pSessionMessage->szStartFunction_A;
        lpszStopFunction_A = pSessionMessage->szStopFunction_A;
    }

    // Check if the session dll is loaded
    pSessionDll = FindSessionDll(hMemObject, &g_NetsyncServerInfo.pSessionDlls, lpszDllName_W);
    if (NULL == pSessionDll) {
        iErrorCode = GetLastError();

        goto ExitFunc;
    }

    // Check if the session info is loaded
    pSessionInfo = FindSessionInfo(hMemObject, &pSessionDll->pSessionInfos, pSessionDll->hModule, pSessionMessage->dwSessionType, pSessionMessage->byMinClientCount, pSessionMessage->byMaxClientCount, pSessionMessage->nMinPortCount, lpszMainFunction_A, lpszStartFunction_A, lpszStopFunction_A);
    if (NULL == pSessionInfo) {
        iErrorCode = GetLastError();

        goto ExitFunc;
    }

    // Copy the session client count
    if (NULL == pSessionInfo->pActiveSession) {
        *bySessionClientCount = 0;
    }
    else {
        *bySessionClientCount = pSessionInfo->pActiveSession->byClientCount;
    }

ExitFunc:
    // Release the netsync server mutex
    ReleaseMutex(g_hNetsyncServerInfoMutex);

    if (ERROR_SUCCESS != iErrorCode) {
        // Set the last error code
        SetLastError(iErrorCode);
    }

    return (ERROR_SUCCESS == iErrorCode);
}



BYTE
AllocatePortSections(
    IN  u_short  nMinPortCount,
    OUT u_short  *LowPort,
    OUT u_short  *HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Marks a number of port sections as allocated.

Arguments:

  nMinPortCount - Specifies the minimum port count
  LowPort - Pointer to the low port allocated
  HighPort - Pointer to the high port allocated

Return Value:

  BYTE:
    If the port sections are allocated, the return value is the first section allocated.
    If the port sections are not allocated, the return value is 0.

------------------------------------------------------------------------------*/
{
    // byPortSection is the base port section allocated
    BYTE  byPortSection = 0;

    // bySectionCount is the section count to allocate
    BYTE  bySectionCount = 0;
    // byCurrentSection is a counter to enumerate each section in the port range
    BYTE  byCurrentSection = 0;
    // byNumSections is a counter to enumerate the block of sections
    BYTE  byNumSections = 0;



    // Determine the section count to allocate
    bySectionCount = ((nMinPortCount + 1) / NETSYNC_PORT_SECTION_SIZE) + ((0 != ((nMinPortCount + 1) % NETSYNC_PORT_SECTION_SIZE)) ? 1 : 0);

    // Find a free block of port sections
    for (byCurrentSection = 0; (byCurrentSection + bySectionCount - 1) < NETSYNC_PORT_SECTIONS; byCurrentSection++) {
        for (byNumSections = 0; byNumSections < bySectionCount; byNumSections++) {
            if ((TRUE == g_NetsyncServerInfo.bPortInUse[byCurrentSection + byNumSections]) || (TRUE == g_NetsyncServerInfo.bPortUsed[byCurrentSection + byNumSections])) {
                break;
            }
        }

        if (byNumSections == bySectionCount) {
            break;
        }
    }

    if (byCurrentSection < NETSYNC_PORT_SECTIONS) {
        // Mark the block of port sections as allocated
        for (byNumSections = 0; byNumSections < bySectionCount; byNumSections++) {
            g_NetsyncServerInfo.bPortInUse[byCurrentSection + byNumSections] = TRUE;
            g_NetsyncServerInfo.bPortUsed[byCurrentSection + byNumSections] = TRUE;
        }

        // Set the first section allocated
        byPortSection = byCurrentSection;

        // Set the low and high port allocated
        *LowPort = (byPortSection * NETSYNC_PORT_SECTION_SIZE) + NETSYNC_PORT_LOW_RANGE;
        *HighPort = ((byPortSection + bySectionCount) * NETSYNC_PORT_SECTION_SIZE) + NETSYNC_PORT_LOW_RANGE - 1;
    }

    return byPortSection;
}



VOID
ReleasePortSections(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Marks port sections as not recently used.

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // byPortSection is a counter to enumeration each port section
    BYTE  byPortSection = 0;



    // Enumerate each port section
    for (byPortSection = 0; byPortSection < NETSYNC_PORT_SECTIONS; byPortSection++) {
        if ((FALSE == g_NetsyncServerInfo.bPortInUse[byPortSection]) && (TRUE == g_NetsyncServerInfo.bPortUsed[byPortSection])) {
            g_NetsyncServerInfo.bPortUsed[byPortSection] = FALSE;
        }
    }
}



VOID
FreePortSections(
    IN BYTE     byPortSection,
    IN u_short  nMinPortCount
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Marks a number of port sections as free.

Arguments:

  byPortSection - Specifies the first section to be freed
  nMinPortCount - Specifies the minimum port count

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // bySectionCount is the section count to free
    BYTE  bySectionCount = 0;
    // byNumSections is a counter to enumerate the block of sections
    BYTE  byNumSections = 0;



    // Determine the section count to free
    bySectionCount = ((nMinPortCount + 1) / NETSYNC_PORT_SECTION_SIZE) + ((0 != ((nMinPortCount + 1) % NETSYNC_PORT_SECTION_SIZE)) ? 1 : 0);

    // Free the block of port sections
    for (byNumSections = 0; byNumSections < bySectionCount; byNumSections++) {
        g_NetsyncServerInfo.bPortInUse[byPortSection + byNumSections] = FALSE;
    }
}



VOID
RemoveSessionFromList(
    IN PNETSYNC_SESSION  pSession
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Removes the specified session from the list of sessions

Arguments:

  pSession - Pointer to the session

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // pPrevSession is a pointer to the previous session in the list
    PNETSYNC_SESSION  pPrevSession = NULL;
    // pCurrentSession is a pointer to the current session in the list
    PNETSYNC_SESSION  pCurrentSession = NULL;
    // pNextSession is a pointer to the next session in the list
    PNETSYNC_SESSION  pNextSession = NULL;



    if (pSession->pSessionInfo->pActiveSession == pSession) {
        // The specified session is the active session

        // Set the active session to NULL
        pSession->pSessionInfo->pActiveSession = NULL;
    }
    else {
        // The specified session is in the list of running sessions

        // Set the current session to the head of the list of running sessions
        pCurrentSession = pSession->pSessionInfo->pRunningSessions;

        while (NULL != pCurrentSession) {
            // Set the next session to the next session
            pNextSession = pCurrentSession->pNextSession;

            if (pCurrentSession == pSession) {
                break;
            }

            // Set the previous session to the current session
            pPrevSession = pCurrentSession;

            // Set the current session to the next session
            pCurrentSession = pNextSession;
        }

        if (NULL != pCurrentSession) {
            // The specified session was found
            if (NULL == pPrevSession) {
                // The specified session is the head of the list of running sessions

                // Set the head of the list of running sessions to the next session in the list
                pSession->pSessionInfo->pRunningSessions = pNextSession;
            }
            else {
                // The specified session is in the list of running sessions

                // Set the previous session pointer to the next session to the next session
                pPrevSession->pNextSession = pNextSession;
            }
        }
    }
}



VOID
FreeSession(
    IN PNETSYNC_SESSION  pSession
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Frees the specified session from the list of sessions

Arguments:

  pSession - Pointer to the session

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // hMemObject is a handle to the memory object
    HANDLE           hMemObject = NULL;
    // pNetsyncObject is handle to the netsync object
    PNETSYNC_OBJECT  pNetsyncObject = NULL;



    // Wait for the netsync server mutex
    WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);

    // Get the memory object
    hMemObject = pSession->pNetsyncObject->hMemObject;

    // Get the netsync object
    pNetsyncObject = pSession->pNetsyncObject;

    // Decrement the server client count
    g_NetsyncServerInfo.nServerClientCount -= pSession->byClientCount;

    // Decrement the session count
    g_NetsyncServerInfo.bySessionCount--;

    if ((0 == g_NetsyncServerInfo.bySessionCount) && (TRUE == g_NetsyncServerInfo.bStopServer)) {
        // Set the stop event
        SetEvent(g_NetsyncServerInfo.hStopEvent);
    }

    // Free the allocated port sections
    FreePortSections(pSession->byPortSection, pSession->pSessionInfo->nMinPortCount);

    // Free the session
    xMemFree(hMemObject, pSession);

    // Free the NETSYNC_OBJECT
    CloseNetsyncObject(pNetsyncObject);

    // Release the netsync server mutex
    ReleaseMutex(g_hNetsyncServerInfoMutex);
}



VOID
RemoveClientFromList(
    IN PNETSYNC_SESSION  pSession,
    IN u_long            ClientInAddr
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Removes the client from the list

Arguments:

  pSession - Pointer to the specified session
  ClientInAddr - ip address of the new client

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // byCurrentClient is a counter to enumerate each client in the list
    BYTE  byCurrentClient = 0;
    // byOffset is the lookahead offset
    BYTE  byOffset = 0;



    for (byCurrentClient = 0; (byCurrentClient + byOffset) < pSession->byClientCount; byCurrentClient++) {
        // Check if the current client is the specified client
        if (ClientInAddr == pSession->ClientInAddrs[byCurrentClient]) {
            byOffset = 1;
        }

        // Copy the client address
        pSession->ClientInAddrs[byCurrentClient] = pSession->ClientInAddrs[byCurrentClient + byOffset];
        CopyMemory(&pSession->ClientXnAddrs[byCurrentClient], &pSession->ClientXnAddrs[byCurrentClient + byOffset], sizeof(pSession->ClientXnAddrs[byCurrentClient]));
    }

    // Decrement the client count
    pSession->byClientCount--;

    // Decrement the server client count
    g_NetsyncServerInfo.nServerClientCount--;

    if ((NULL == pSession->pSessionInfo->pActiveSession) || ((SESSION_RUNNING == pSession->pSessionInfo->pActiveSession->dwSessionStatus) && (pSession->byClientCount < pSession->pSessionInfo->pActiveSession->byClientCount))) {
        // This session now has fewer clients than the active session so make it the active session

        if (NULL != pSession->pSessionInfo->pActiveSession) {
            // Move the active session to the running list
            pSession->pSessionInfo->pActiveSession->pNextSession = pSession->pSessionInfo->pRunningSessions;
            pSession->pSessionInfo->pRunningSessions = pSession->pSessionInfo->pActiveSession;
            pSession->pSessionInfo->pActiveSession = NULL;
        }

        // Remove session from running list
        RemoveSessionFromList(pSession);

        // Set the active session to this session
        pSession->pSessionInfo->pActiveSession = pSession;
    }
}



VOID
RemoveClientFromSession(
    IN PNETSYNC_SESSION  pSession,
    IN u_long            ClientInAddr
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Removes the client from the specified session

Arguments:

  pSession - Pointer to the specified session
  ClientInAddr - ip address of the new client

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // byCurrentClient is a counter to enumerate each client in the list
    BYTE  byCurrentClient = 0;
    // byOffset is the lookahead offset
    BYTE  byOffset = 0;



    // Wait for the netsync server mutex
    WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);

    // Enter the session critical section
    EnterCriticalSection(&pSession->cs);

    // Remove the client
    RemoveClientFromList(pSession, ClientInAddr);

    // Leave the session critical section
    LeaveCriticalSection(&pSession->cs);

    // Release the netsync server mutex
    ReleaseMutex(g_hNetsyncServerInfoMutex);
}



VOID
SendConnectReply(
    IN HANDLE   hNetsyncObject,
    IN DWORD    dwErrorCode,
    IN BYTE     byClientCount,
    IN u_long   *ClientInAddrs,
    IN u_short  LowPort,
    IN u_short  HighPort
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sends a netsync connect reply to an array of clients

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT
  dwErrorCode - Specifies the error code of the connect
  byClientCount - Specifies the number of clients in the array
  ClientInAddrs - Pointer to an array of client ip addresses
  LowPort - Low port allocated for the session
  HighPort - High port allocated for the session

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // ConnectSessionReply is a connect session reply
    NETSYNC_CONNECT_REPLY  ConnectSessionReply;
    // byCurrentClient is a counter to enumerate each client address in the session
    BYTE                   byCurrentClient;



    // Setup the connect session reply
    ZeroMemory(&ConnectSessionReply, sizeof(ConnectSessionReply));
    ConnectSessionReply.dwMessageId = NETSYNC_REPLY_CONNECT;
    ConnectSessionReply.dwErrorCode = dwErrorCode;
    ConnectSessionReply.LowPort = LowPort;
    ConnectSessionReply.HighPort = HighPort;

    for (byCurrentClient = 0; byCurrentClient < byClientCount; byCurrentClient++) {
        // Send the connect session reply
        DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Send Connect Reply To: 0x%08x\n", ClientInAddrs[byCurrentClient]);
        NetsyncSendServerMessage(hNetsyncObject, ClientInAddrs[byCurrentClient], 0, TRUE, sizeof(ConnectSessionReply), (char *) &ConnectSessionReply);
    }
}



VOID
SendStopMessageToPort(
    IN HANDLE   hNetsyncObject,
    IN BOOL     bNetsyncPort,
    IN BYTE     byClientCount,
    IN u_long   *ClientInAddrs
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sends a netsync stop message to an array of clients

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT
  bNetsyncPort - Specifies if destination port is netsync port
  byClientCount - Specifies the number of clients in the array
  ClientInAddrs - Pointer to an array of client addresses

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT          pNetsyncObject = (PNETSYNC_OBJECT) hNetsyncObject;
    // GenericMessage is the stop session message
    NETSYNC_GENERIC_MESSAGE  GenericMessage;
    // byCurrentClient is a counter to enumerate each client address in the session
    BYTE                     byCurrentClient;



    GenericMessage.dwMessageId = NETSYNC_MSG_STOPSESSION;

    for (byCurrentClient = 0; byCurrentClient < byClientCount; byCurrentClient++) {
        // Send the stop session message
        NetsyncSendServerMessage(hNetsyncObject, ClientInAddrs[byCurrentClient], (TRUE == bNetsyncPort) ? 0 : pNetsyncObject->NetsyncPort, TRUE, sizeof(GenericMessage), (char *) &GenericMessage);
    }
}



DWORD
SessionThread(
    IN LPVOID  lpv
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Thread to handle the session logic

Arguments:

  lpv - Pointer to the session

------------------------------------------------------------------------------*/
{
    // pSesssion is a pointer to the session
    PNETSYNC_SESSION       pSession = (PNETSYNC_SESSION) lpv;
    // lpContext is a pointer to a context structure
    LPVOID                 lpContext = NULL;
    // FromInAddr is the source ip address
    u_long                 FromInAddr;
    // FromXnAddr is the source xnet address
    XNADDR                 FromXnAddr;
    // dwMessageType is the type of receive message
    DWORD                  dwMessageType;
    // dwMessageSize is the size of the receive message
    DWORD                  dwMessageSize;
    // pMessage is a pointer to the received message
    char                   *pMessage;
    // bExceptionOccurred indicates if an exception occurred
    BOOL                   bExceptionOccurred = FALSE;
    // bStopSession indicates if the stop session message was received
    BOOL                   bStopSession = FALSE;
    // bCallbackFunction is the return value from pCallbackFunction
    BOOL                   bCallbackFunction = FALSE;



    // Wait for the netsync server mutex
    WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);

    // Enter the session critical section
    EnterCriticalSection(&pSession->cs);

    if (TRUE == pSession->bStopSession) {
        goto ExitSession;
    }

    // Release the netsync server mutex
    ReleaseMutex(g_hNetsyncServerInfoMutex);

    // Send the connect session replies
    SendConnectReply(pSession->pNetsyncObject, ERROR_SUCCESS, pSession->byClientCount, pSession->ClientInAddrs, pSession->LowPort, pSession->HighPort);

    if (NETSYNC_SESSION_CALLBACK == pSession->pSessionInfo->dwSessionType) {
        DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread() Started\n  Session Id: 0x%08x\n  Start Function: %s\n  Main Function: %s\n  Stop Function: %s\n", pSession->hThread, pSession->pSessionInfo->lpszStartFunction_A, pSession->pSessionInfo->lpszMainFunction_A, pSession->pSessionInfo->lpszStopFunction_A);
    }
    else {
        DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread() Started\n  Session Id: 0x%08x\n  Main Function: %s\n", pSession->hThread, pSession->pSessionInfo->lpszMainFunction_A);
    }

    // Check the session type
    if (NETSYNC_SESSION_CALLBACK == pSession->pSessionInfo->dwSessionType) {
        __try {
            DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Start Function - Call %s\n", pSession->pSessionInfo->lpszStartFunction_A);

            // Call the start function
            lpContext = pSession->pSessionInfo->pStartFunction((HANDLE) pSession, pSession->pNetsyncObject, pSession->byClientCount, pSession->ClientInAddrs, pSession->ClientXnAddrs, pSession->LowPort, pSession->HighPort);

            DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Start Function - %s Return\n", pSession->pSessionInfo->lpszStartFunction_A);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            // An exception occurred
            bExceptionOccurred = TRUE;

            DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Start Function - %s Caused An Exception: 0x%08x\n", pSession->pSessionInfo->lpszStartFunction_A, GetExceptionCode());
        }
    }

    // Leave the session critical section
    LeaveCriticalSection(&pSession->cs);

    // Check if an exception occurred
    if (FALSE == bExceptionOccurred) {
        // Check the session type
        if (NETSYNC_SESSION_THREAD == pSession->pSessionInfo->dwSessionType) {
            __try {
                DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Main Function - Call %s\n", pSession->pSessionInfo->lpszMainFunction_A);

                // Call the main thread function
                pSession->pSessionInfo->pThreadFunction(pSession->pNetsyncObject, pSession->byClientCount, pSession->ClientInAddrs, pSession->ClientXnAddrs, pSession->LowPort, pSession->HighPort);

                DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Main Function - %s Return\n", pSession->pSessionInfo->lpszMainFunction_A);
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                // An exception occurred
                bExceptionOccurred = TRUE;

                DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Main Function - %s Caused An Exception: 0x%08x\n", pSession->pSessionInfo->lpszMainFunction_A, GetExceptionCode());
            }
        }
        else {
            do {
                if (WAIT_OBJECT_0 == NetsyncReceiveMessage(pSession->pNetsyncObject, INFINITE, &dwMessageType, &FromInAddr, &FromXnAddr, &dwMessageSize, &pMessage)) {
                    if (NETSYNC_MSGTYPE_SERVER == dwMessageType) {
                        // Check if the message indicates a client joined the session
                        if ((NETSYNC_MSG_ADDCLIENT == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) || (NETSYNC_MSG_DUPLICATECLIENT == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId)) {
                            FromInAddr = ((PNETSYNC_CLIENT_MESSAGE) pMessage)->FromInAddr;
                            CopyMemory(&FromXnAddr, &((PNETSYNC_CLIENT_MESSAGE) pMessage)->FromXnAddr, sizeof(FromXnAddr));

                            if (NETSYNC_MSG_ADDCLIENT == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) {
                                DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): NETSYNC_MSG_ADDCLIENT - From: 0x%08x\n", FromInAddr);
                            }
                            else {
                                DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): NETSYNC_MSG_DUPLICATECLIENT - From: 0x%08x\n", FromInAddr);
                            }

                            // Send the connect session reply
                            SendConnectReply(pSession->pNetsyncObject, ERROR_SUCCESS, 1, &FromInAddr, pSession->LowPort, pSession->HighPort);
                        }
                        // Check if the message indicates a client left the session
                        else if (NETSYNC_MSG_DELETECLIENT == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) {
                            // Remove the client from the session
                            RemoveClientFromSession(pSession, FromInAddr);

                            DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): NETSYNC_MSG_DELETECLIENT - From: 0x%08x\n", FromInAddr);
                        }
                        // Check if the message indicates the session has stopped
                        else if (NETSYNC_MSG_STOPSESSION == ((PNETSYNC_GENERIC_MESSAGE) pMessage)->dwMessageId) {
                            DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): NETSYNC_MSG_STOPSESSION\n");
                        }
                    }

                    __try {
                        DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Main Function - Call %s\n", pSession->pSessionInfo->lpszMainFunction_A);

                        // Call the main callback function
                        bCallbackFunction = pSession->pSessionInfo->pCallbackFunction((HANDLE) pSession, pSession->pNetsyncObject, FromInAddr, &FromXnAddr, dwMessageType, dwMessageSize, pMessage, lpContext);

                        DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Main Function - %s Returned %s\n", pSession->pSessionInfo->lpszMainFunction_A, (TRUE == bCallbackFunction) ? "TRUE" : "FALSE");
                    }
                    __except (EXCEPTION_EXECUTE_HANDLER) {
                        // An exception occurred
                        bExceptionOccurred = TRUE;

                        DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Main Function - %s Caused An Exception %s\n", pSession->pSessionInfo->lpszMainFunction_A, GetExceptionCode());
                    }

                    // Free the message
                    NetsyncFreeMessage(pMessage);
                }
            } while ((TRUE != bExceptionOccurred) && (FALSE != bCallbackFunction));
        }
    }

    // Check if an exception occurred
    if (FALSE == bExceptionOccurred) {
        // Check the session type
        if (NETSYNC_SESSION_CALLBACK == pSession->pSessionInfo->dwSessionType) {
            __try {
                DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Stop Function - Call %s\n", pSession->pSessionInfo->lpszStopFunction_A);

                // Call the stop function
                pSession->pSessionInfo->pStopFunction((HANDLE) pSession, pSession->pNetsyncObject, lpContext);

                DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Stop Function - %s Return\n", pSession->pSessionInfo->lpszStopFunction_A);
            }
            __except (EXCEPTION_EXECUTE_HANDLER) {
                // An exception occurred
                bExceptionOccurred = TRUE;

                DebugMessage(NETSYNC_DEBUG_SESSION, "SessionThread(): Stop Function - %s Caused An Exception %s\n", pSession->pSessionInfo->lpszStopFunction_A, GetExceptionCode());
            }
        }
    }

    // Wait for the netsync server mutex
    WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);

    // Enter the session critical section
    EnterCriticalSection(&pSession->cs);

ExitSession:
    // Remove the session from the list of sessions
    RemoveSessionFromList(pSession);

    // Release the netsync server mutex
    ReleaseMutex(g_hNetsyncServerInfoMutex);

    // Send the stop session messages
    SendStopMessageToPort(pSession->pNetsyncObject, pSession->bStopSession, pSession->byClientCount, pSession->ClientInAddrs);

    // Leave the session critical section
    LeaveCriticalSection(&pSession->cs);

    // Free the session
    FreeSession(pSession);

    return 0;
}



PNETSYNC_SESSION
StartSession(
    IN HANDLE                 hMemObject,
    IN PNETSYNC_SESSION_INFO  pSessionInfo
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Starts the specified session in a suspended thread.  The thread is resumed when there are sufficient clients in the session.

Arguments:

  hMemObject - Handle to the memory object
  pSessionInfo - Pointer to the session info

Return Value:

  PNETSYNC_SESSION:
    If the function succeeds, the return value is a pointer to the new NETSYNC_SESSION struct.
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is a pointer to the new netsync object
    PNETSYNC_OBJECT   pNetsyncObject = NULL;
    // pSession is a pointer to the new session
    PNETSYNC_SESSION  pSession = NULL;

    // byPortSection is the base port section allocated for the session
    BYTE              byPortSection = 0;
    // LowPort is the low port allocated for the session
    u_short           LowPort = 0;
    // HighPort is the high port allocated for the session
    u_short           HighPort = 0;

    // iErrorCode is the last error code
    int               iErrorCode = ERROR_SUCCESS;



    // Allocate the port range for the session
    byPortSection = AllocatePortSections(pSessionInfo->nMinPortCount, &LowPort, &HighPort);
    if (0 == byPortSection) {
        // Release port sections
        ReleasePortSections();

        // Allocate the port range for the session
        byPortSection = AllocatePortSections(pSessionInfo->nMinPortCount, &LowPort, &HighPort);
        if (0 == byPortSection) {
            iErrorCode = ERROR_NOT_ENOUGH_QUOTA;

            goto FunctionFailed;
        }
    }

    // Create the netsync object
    pNetsyncObject = CreateNetsyncObject(hMemObject, 0, LowPort);
    if (NULL == pNetsyncObject) {
        iErrorCode = GetLastError();

        goto FunctionFailed;
    }

    // Allocate the memory for the new session
    pSession = (PNETSYNC_SESSION) xMemAlloc(pNetsyncObject->hMemObject, sizeof(NETSYNC_SESSION) + pSessionInfo->byMaxClientCount * (sizeof(u_long) + sizeof(XNADDR)));
    if (NULL == pSession) {
        iErrorCode = GetLastError();

        goto FunctionFailed;
    }

    // Set the netsync object
    pSession->pNetsyncObject = pNetsyncObject;

#ifdef _XBOX
    // Copy the xnet key id and key
    CopyMemory(&pSession->pNetsyncObject->XnKid, &g_NetsyncServerInfo.pNetsyncObject->XnKid, sizeof(pSession->pNetsyncObject->XnKid));
    CopyMemory(&pSession->pNetsyncObject->XnKey, &g_NetsyncServerInfo.pNetsyncObject->XnKey, sizeof(pSession->pNetsyncObject->XnKey));
#endif

    // Initialize the critical section
    InitializeCriticalSection(&pSession->cs);

    // Set the pointer to the session info
    pSession->pSessionInfo = pSessionInfo;

    // Set the initial client count
    pSession->byClientCount = 0;

    // Set the initial client addresses
    pSession->ClientInAddrs = (u_long *) ((UINT_PTR) pSession + sizeof(NETSYNC_SESSION));
    pSession->ClientXnAddrs = (XNADDR *) ((UINT_PTR) pSession + sizeof(u_long) * pSessionInfo->byMaxClientCount + sizeof(NETSYNC_SESSION));

    // Set the ports
    pSession->byPortSection = byPortSection;
    pSession->LowPort = LowPort + 1;
    pSession->HighPort = HighPort;

    // Create the session thread
    pSession->hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SessionThread, pSession, CREATE_SUSPENDED, NULL);
    if (NULL == pSession->hThread) {
        iErrorCode = GetLastError();

        goto FunctionFailed;
    }

    return pSession;

FunctionFailed:
    // Free the new session, if necessary
    if (NULL != pSession) {
        xMemFree(pNetsyncObject->hMemObject, pSession);
    }

    // Close the netsync object, if necessary
    if (NULL != pNetsyncObject) {
        CloseNetsyncObject(pNetsyncObject);
    }

    // Free the port range, if necessary
    if (0 != byPortSection) {
        FreePortSections(byPortSection, pSessionInfo->nMinPortCount);
    }

    // Set the last error code
    SetLastError(iErrorCode);

    return NULL;
}



BOOL
AddClientToSession(
    IN HANDLE                    hMemObject,
    IN u_long                    ClientInAddr,
    IN XNADDR                    *ClientXnAddr,
    IN PNETSYNC_SESSION_MESSAGE  pSessionMessage
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Adds the client to the specified session

Arguments:

  hNetsyncObject - Handle to the NETSYNC_OBJECT
  ClientIpAddr - ip address of the new client
  ClientXnAddr - xnet address of the new client
  pSessionMessage - Pointer to a session message

Return Value:

  BOOL:
    If the client is added, the return value is nonzero.
    If the client is not added, the return value is 0.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pSessionDll is a pointer to the session dll
    PNETSYNC_SESSION_DLL    pSessionDll = NULL;
    // pSessionInfo is a pointer to the session info
    PNETSYNC_SESSION_INFO   pSessionInfo = NULL;
    // lpszDllName_W is a pointer to szDllName_W within the session message
    LPWSTR                  lpszDllName_W = pSessionMessage->szDllName_W;
    // lpszMainFunction_A is a pointer to szMainFunction_A within the session message
    LPSTR                   lpszMainFunction_A = pSessionMessage->szMainFunction_A;
    // lpszStartFunction_A is a pointer to szStartFunction_A within the session message
    LPSTR                   lpszStartFunction_A = NULL;
    // lpszStopFunction_A is a pointer to szStopFunction_A within the session message
    LPSTR                   lpszStopFunction_A = NULL;
    // bNewClient specifies if the client is new or a duplicate
    BOOL                    bNewClient = TRUE;
    // byCurrentClient is a counter to enumerate each client in the list
    BYTE                    byCurrentClient = 0;
    // ClientMessage is the client message
    NETSYNC_CLIENT_MESSAGE  ClientMessage;
    // iErrorCode is the last error code
    int                     iErrorCode = ERROR_SUCCESS;



    // Wait for the netsync server mutex
    WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);

    if (TRUE == g_NetsyncServerInfo.bStopServer) {
        // Server is stopping
        iErrorCode = ERROR_SERVER_DISABLED;

        goto ExitFunc;
    }

    if (NETSYNC_SESSION_CALLBACK == pSessionMessage->dwSessionType) {
        // Get the pointers to the function names within the session message
        lpszStartFunction_A = pSessionMessage->szStartFunction_A;
        lpszStopFunction_A = pSessionMessage->szStopFunction_A;
    }

    // Check if the session dll is loaded
    pSessionDll = FindSessionDll(hMemObject, &g_NetsyncServerInfo.pSessionDlls, lpszDllName_W);
    if (NULL == pSessionDll) {
        iErrorCode = GetLastError();

        goto ExitFunc;
    }

    // Check if the session info is loaded
    pSessionInfo = FindSessionInfo(hMemObject, &pSessionDll->pSessionInfos, pSessionDll->hModule, pSessionMessage->dwSessionType, pSessionMessage->byMinClientCount, pSessionMessage->byMaxClientCount, pSessionMessage->nMinPortCount, lpszMainFunction_A, lpszStartFunction_A, lpszStopFunction_A);
    if (NULL == pSessionInfo) {
        iErrorCode = GetLastError();

        goto ExitFunc;
    }

    if (NULL == pSessionInfo->pActiveSession) {
        // Start the session
        pSessionInfo->pActiveSession = StartSession(hMemObject, pSessionInfo);
        if (NULL == pSessionInfo->pActiveSession) {
            iErrorCode = GetLastError();

            goto ExitFunc;
        }

        // Update the session status
        pSessionInfo->pActiveSession->dwSessionStatus = SESSION_PENDING;

        // Increment the session count
        g_NetsyncServerInfo.bySessionCount++;
    }

    // Enter the session critical section
    EnterCriticalSection(&pSessionInfo->pActiveSession->cs);

    // Check if the client is a duplicate
    for (byCurrentClient = 0; byCurrentClient < pSessionInfo->pActiveSession->byClientCount; byCurrentClient++) {
        // Check if the current client is the specified client
        if (ClientInAddr == pSessionInfo->pActiveSession->ClientInAddrs[byCurrentClient]) {
            break;
        }
    }
    bNewClient = (byCurrentClient == pSessionInfo->pActiveSession->byClientCount);

    if (TRUE == bNewClient) {
        DebugMessage(NETSYNC_DEBUG_SERVER, "AddClientToSession(): New Client 0x%08x\n", ClientInAddr);

        // Increment the client count
        pSessionInfo->pActiveSession->byClientCount++;

        // Copy the new client address
        pSessionInfo->pActiveSession->ClientInAddrs[pSessionInfo->pActiveSession->byClientCount - 1] = ClientInAddr;
        CopyMemory(&pSessionInfo->pActiveSession->ClientXnAddrs[pSessionInfo->pActiveSession->byClientCount - 1], ClientXnAddr, sizeof(pSessionInfo->pActiveSession->ClientXnAddrs[pSessionInfo->pActiveSession->byClientCount - 1]));

        // Increment the server client count
        g_NetsyncServerInfo.nServerClientCount++;
    }
    else {
        DebugMessage(NETSYNC_DEBUG_SERVER, "AddClientToSession(): Duplicate Client 0x%08x\n", ClientInAddr);
    }

    if (SESSION_RUNNING == pSessionInfo->pActiveSession->dwSessionStatus) {
        if (TRUE == bNewClient) {
            // Setup the add client message
            ClientMessage.dwMessageId = NETSYNC_MSG_ADDCLIENT;
        }
        else {
            // Setup the duplicate client message
            ClientMessage.dwMessageId = NETSYNC_MSG_DUPLICATECLIENT;
        }
        ClientMessage.FromInAddr = ClientInAddr;
        CopyMemory(&ClientMessage.FromXnAddr, ClientXnAddr, sizeof(ClientMessage.FromXnAddr));

        // Send the add client message
        NetsyncSendServerMessage(pSessionInfo->pActiveSession->pNetsyncObject, htonl(INADDR_LOOPBACK), pSessionInfo->pActiveSession->pNetsyncObject->NetsyncPort, TRUE, sizeof(ClientMessage), (char *) &ClientMessage);
    }
    else if (pSessionInfo->byMinClientCount == pSessionInfo->pActiveSession->byClientCount) {
        // Update the session status
        pSessionInfo->pActiveSession->dwSessionStatus = SESSION_RUNNING;

        // Resume the session thread
        ResumeThread(pSessionInfo->pActiveSession->hThread);
    }

    // Leave the session critical section
    LeaveCriticalSection(&pSessionInfo->pActiveSession->cs);

    if (pSessionInfo->byMaxClientCount == pSessionInfo->pActiveSession->byClientCount) {
        // Session has the maximum number of clients, so move the active session to the list of running sessions
        pSessionInfo->pActiveSession->pNextSession = pSessionInfo->pRunningSessions;
        pSessionInfo->pRunningSessions = pSessionInfo->pActiveSession;
        pSessionInfo->pActiveSession = NULL;
    }

ExitFunc:
    // Release the netsync server mutex
    ReleaseMutex(g_hNetsyncServerInfoMutex);

    if (ERROR_SUCCESS != iErrorCode) {
        // Set the last error code
        SetLastError(iErrorCode);
    }

    return (ERROR_SUCCESS == iErrorCode);
}



DWORD
ServerThread(
    IN LPVOID  lpv
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Thread to handle the server logic

Arguments:

  lpv - Handle of the NETSYNC_OBJECT

------------------------------------------------------------------------------*/
{
    // pNetsyncObject is the pointer to the NETSYNC_OBJECT
    PNETSYNC_OBJECT           pNetsyncObject = (PNETSYNC_OBJECT) lpv;

    // FromInAddr is the source ip address
    u_long                    FromInAddr;
    // FromXnAddr is the source xnet address
    XNADDR                    FromXnAddr;
    // dwMessageSize is the size of the receive message
    DWORD                     dwMessageSize;
    // pMessage is a pointer to a received message, either query or connect
    char                      *pMessage;
    // pGenericMessage is a pointer to a generic message
    PNETSYNC_GENERIC_MESSAGE  pGenericMessage;
    // pSessionMessage is a pointer to a session message, either query or connect
    PNETSYNC_SESSION_MESSAGE  pSessionMessage;
    // EnumSessionReply is a enum session reply
    NETSYNC_ENUM_REPLY        EnumSessionReply;
    // ConnectSessionReply is a connect session reply
    NETSYNC_CONNECT_REPLY     ConnectSessionReply;



    while (WAIT_OBJECT_0 == NetsyncReceiveServerMessage(pNetsyncObject, INFINITE, &FromInAddr, &FromXnAddr, &dwMessageSize, &pMessage)) {
        pGenericMessage = (PNETSYNC_GENERIC_MESSAGE) pMessage;
        pSessionMessage = (PNETSYNC_SESSION_MESSAGE) pMessage;

        if (NETSYNC_MSG_STOPSESSION == pGenericMessage->dwMessageId) {
            // Free the message
            NetsyncFreeMessage(pMessage);

            break;
        }

        // Handle the message
        switch (pGenericMessage->dwMessageId) {

        case NETSYNC_MSG_ENUM:
            DebugMessage(NETSYNC_DEBUG_SERVER, "ServerThread(): NETSYNC_MSG_ENUM From 0x%08x\n", FromInAddr);

            ZeroMemory(&EnumSessionReply, sizeof(EnumSessionReply));

            // Check if the session info exists
            if (TRUE == DoesSessionInfoExist(pNetsyncObject->hMemObject, pSessionMessage, &EnumSessionReply.bySessionClientCount)) {
                DebugMessage(NETSYNC_DEBUG_SERVER, "ServerThread(): NETSYNC_MSG_ENUM Enum Succeeded\n");

                // Wait for the netsync server mutex
                WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);

                // Setup the enum session reply
                EnumSessionReply.dwMessageId = NETSYNC_REPLY_ENUM;
                CopyMemory(&EnumSessionReply.FromObjectUUID, &pSessionMessage->FromObjectUUID, sizeof(EnumSessionReply.FromObjectUUID));
                EnumSessionReply.nServerClientCount = g_NetsyncServerInfo.nServerClientCount;
                EnumSessionReply.bySessionCount = g_NetsyncServerInfo.bySessionCount;
                EnumSessionReply.InAddr = pNetsyncObject->InAddr;
                CopyMemory(&EnumSessionReply.XnKid, &pNetsyncObject->XnKid, sizeof(EnumSessionReply.XnKid));
                CopyMemory(&EnumSessionReply.XnKey, &pNetsyncObject->XnKey, sizeof(EnumSessionReply.XnKey));

                // Release the netsync server mutex
                ReleaseMutex(g_hNetsyncServerInfoMutex);

                // Send the enum session reply
                NetsyncSendServerMessage(pNetsyncObject, htonl(INADDR_BROADCAST), 0, FALSE, sizeof(EnumSessionReply), (char *) &EnumSessionReply);
            }
            else {
                DebugMessage(NETSYNC_DEBUG_SERVER, "ServerThread(): NETSYNC_MSG_ENUM Enum Failed\n");
            }

            break;

        case NETSYNC_MSG_CONNECT:
            DebugMessage(NETSYNC_DEBUG_SERVER, "ServerThread(): NETSYNC_MSG_CONNECT From 0x%08x\n", FromInAddr);

            // Add the client to the specified session
            if (FALSE == AddClientToSession(pNetsyncObject->hMemObject, FromInAddr, &FromXnAddr, pSessionMessage)) {
                DebugMessage(NETSYNC_DEBUG_SERVER, "ServerThread(): NETSYNC_MSG_CONNECT Connect Failed - ec: 0x%08x\n", GetLastError());

                // Setup the connect session reply
                ZeroMemory(&ConnectSessionReply, sizeof(ConnectSessionReply));
                ConnectSessionReply.dwMessageId = NETSYNC_REPLY_CONNECT;
                ConnectSessionReply.dwErrorCode = GetLastError();

                // Send the connect session reply
                NetsyncSendServerMessage(pNetsyncObject, FromInAddr, 0, TRUE, sizeof(ConnectSessionReply), (char *) &ConnectSessionReply);
            }
            else {
                DebugMessage(NETSYNC_DEBUG_SERVER, "ServerThread(): NETSYNC_MSG_CONNECT Connect Succeeded\n");
            }

            break;

        default:
            break;
        }

        // Free the message
        NetsyncFreeMessage(pMessage);
    }

    return 0;
}

} // namespace NetsyncNamespace



BOOL
WINAPI
NetsyncCreateServer(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Increments the server ref count and creates a netsync server, if necessary

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero
    If the function fails, the return value is 0.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // iErrorCode is the last error code
    int  iErrorCode = ERROR_SUCCESS;



    DebugMessage(NETSYNC_DEBUG_SERVER, "NetsyncCreateServer()\n");

    // Create the netsync server mutex
    g_hNetsyncServerInfoMutex = CreateMutexA(NULL, FALSE, "NetsyncServerMutex");

    // Wait for the netsync server mutex
    WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);

    if (0 == g_NetsyncServerInfo.dwRefCount) {
        // Create the memory object
        g_NetsyncServerInfo.hMemObject = xMemCreate();
        if (INVALID_HANDLE_VALUE == g_NetsyncServerInfo.hMemObject) {
            iErrorCode = GetLastError();

            goto FunctionFailed;
        }

        // Create the NETSYNC_OBJECT
        g_NetsyncServerInfo.pNetsyncObject = CreateNetsyncObject(g_NetsyncServerInfo.hMemObject, 0, NETSYNC_PORT);
        if (NULL == g_NetsyncServerInfo.pNetsyncObject) {
            iErrorCode = GetLastError();

            goto FunctionFailed;
        }

#ifdef _XBOX
        // Create and register the xnet key id and key
        XNetCreateKey(&g_NetsyncServerInfo.pNetsyncObject->XnKid, &g_NetsyncServerInfo.pNetsyncObject->XnKey);
        XNetRegisterKey(&g_NetsyncServerInfo.pNetsyncObject->XnKid, &g_NetsyncServerInfo.pNetsyncObject->XnKey);
        g_NetsyncServerInfo.pNetsyncObject->bXnKey = TRUE;
#endif

        g_NetsyncServerInfo.hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == g_NetsyncServerInfo.hStopEvent) {
            iErrorCode = GetLastError();

            goto FunctionFailed;
        }

        // Create the server thread
        g_NetsyncServerInfo.hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerThread, g_NetsyncServerInfo.pNetsyncObject, 0, NULL);
        if (NULL == g_NetsyncServerInfo.hThread) {
            iErrorCode = GetLastError();

            goto FunctionFailed;
        }
    }

    // Increment the reference count
    g_NetsyncServerInfo.dwRefCount++;

    // Release the netsync server mutex
    ReleaseMutex(g_hNetsyncServerInfoMutex);

    return TRUE;

FunctionFailed:
    // Close the stop event, if necessary
    if (NULL != g_NetsyncServerInfo.hStopEvent) {
        CloseHandle(g_NetsyncServerInfo.hStopEvent);
        g_NetsyncServerInfo.hStopEvent = NULL;
    }

    // Free the NETSYNC_OBJECT, if necessary
    if (NULL != g_NetsyncServerInfo.pNetsyncObject) {
        CloseNetsyncObject(g_NetsyncServerInfo.pNetsyncObject);
        g_NetsyncServerInfo.pNetsyncObject = NULL;
    }

    // Close the memory object, if necessary
    if (INVALID_HANDLE_VALUE != g_NetsyncServerInfo.hMemObject) {
        xMemClose(g_NetsyncServerInfo.hMemObject);
        g_NetsyncServerInfo.hMemObject = INVALID_HANDLE_VALUE;
    }

    // Release the netsync server mutex
    ReleaseMutex(g_hNetsyncServerInfoMutex);

    // Set the last error code
    SetLastError(iErrorCode);

    return FALSE;
}



VOID
WINAPI
NetsyncCloseServer(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Decrements the server ref count and close the netsync server, if necessary

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // pCurrentSessionDll is a pointer to the current session dll in the list
    PNETSYNC_SESSION_DLL     pCurrentSessionDll = NULL;
    // pNextSessionDll is a pointer to the next session dll in the list
    PNETSYNC_SESSION_DLL     pNextSessionDll = NULL;
    // pCurrentSessionInfo is a pointer to the current session info in the list
    PNETSYNC_SESSION_INFO    pCurrentSessionInfo = NULL;
    // pNextSessionInfo is a pointer to the next session info in the list
    PNETSYNC_SESSION_INFO    pNextSessionInfo = NULL;
    // pCurrentSession is a pointer to a session within the list
    PNETSYNC_SESSION         pCurrentSession = NULL;
    // GenericMessage is the end session message
    NETSYNC_GENERIC_MESSAGE  GenericMessage;
    // pDllMain is a pointer to the dll entry point
    LPDLLMAIN_PROC           pDllMain = NULL;



    DebugMessage(NETSYNC_DEBUG_SERVER, "NetsyncCloseServer()\n");

    // Create the netsync server mutex
    g_hNetsyncServerInfoMutex = CreateMutexA(NULL, FALSE, "NetsyncServerMutex");

    // Wait for the netsync server mutex
    WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);

    // Ref count must be at least 1
    ASSERT(0 < g_NetsyncServerInfo.dwRefCount);

    // Decrement the reference count
    g_NetsyncServerInfo.dwRefCount--;

    if (0 == g_NetsyncServerInfo.dwRefCount) {
        DebugMessage(NETSYNC_DEBUG_SERVER, "NetsyncCloseServer(): Notify Server Thread Of Pending Stop\n");

        g_NetsyncServerInfo.bStopServer = TRUE;

        // Setup the end session message
        GenericMessage.dwMessageId = NETSYNC_MSG_STOPSESSION;

        // Send the end session message
        NetsyncSendServerMessage(g_NetsyncServerInfo.pNetsyncObject, htonl(INADDR_LOOPBACK), 0, TRUE, sizeof(GenericMessage), (char *) &GenericMessage);

        if (0 != g_NetsyncServerInfo.bySessionCount) {
            for (pCurrentSessionDll = g_NetsyncServerInfo.pSessionDlls; NULL != pCurrentSessionDll; pCurrentSessionDll = pCurrentSessionDll->pNextSessionDll) {
                for (pCurrentSessionInfo = pCurrentSessionDll->pSessionInfos; NULL != pCurrentSessionInfo; pCurrentSessionInfo = pCurrentSessionInfo->pNextSessionInfo) {
                    if (NULL != pCurrentSessionInfo->pActiveSession) {
                        // Enter the session critical section
                        EnterCriticalSection(&pCurrentSessionInfo->pActiveSession->cs);

                        if (SESSION_RUNNING == pCurrentSessionInfo->pActiveSession->dwSessionStatus) {
                            DebugMessage(NETSYNC_DEBUG_SERVER, "NetsyncCloseServer(): Notify Session 0x%08x Of Stop\n", pCurrentSessionInfo->pActiveSession->hThread);

                            // Send the end session message
                            NetsyncSendServerMessage(g_NetsyncServerInfo.pNetsyncObject, htonl(INADDR_LOOPBACK), pCurrentSessionInfo->pActiveSession->pNetsyncObject->NetsyncPort, TRUE, sizeof(GenericMessage), (char *) &GenericMessage);
                        }
                        else {
                            DebugMessage(NETSYNC_DEBUG_SERVER, "NetsyncCloseServer(): Notify Session 0x%08x Of Stop\n", pCurrentSessionInfo->pActiveSession->hThread);

                            pCurrentSessionInfo->pActiveSession->bStopSession = TRUE;

                            // Resume the session thread
                            ResumeThread(pCurrentSessionInfo->pActiveSession->hThread);
                        }

                        // Leave the session critical section
                        LeaveCriticalSection(&pCurrentSessionInfo->pActiveSession->cs);
                    }

                    for (pCurrentSession = pCurrentSessionInfo->pRunningSessions; NULL != pCurrentSession; pCurrentSession = pCurrentSession->pNextSession) {
                        DebugMessage(NETSYNC_DEBUG_SERVER, "NetsyncCloseServer(): Notify Session 0x%08x Of Stop\n", pCurrentSession->hThread);

                        // Send the end session message
                        NetsyncSendServerMessage(g_NetsyncServerInfo.pNetsyncObject, htonl(INADDR_LOOPBACK), pCurrentSession->pNetsyncObject->NetsyncPort, TRUE, sizeof(GenericMessage), (char *) &GenericMessage);
                    }
                }
            }

            // Release access to the netsync server mutex
            ReleaseMutex(g_hNetsyncServerInfoMutex);

            DebugMessage(NETSYNC_DEBUG_SERVER, "NetsyncCloseServer(): Wait For Session Threads To Exit\n");

            // Wait for the stop event to be signaled
            WaitForSingleObject(g_NetsyncServerInfo.hStopEvent, INFINITE);

            // Wait for the netsync server mutex
            WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);
        }

        // Wait for the server thread to exit
        WaitForSingleObject(g_NetsyncServerInfo.hThread, INFINITE);

        for (pCurrentSessionDll = g_NetsyncServerInfo.pSessionDlls; NULL != pCurrentSessionDll; pCurrentSessionDll = pNextSessionDll) {
            for (pCurrentSessionInfo = pCurrentSessionDll->pSessionInfos; NULL != pCurrentSessionInfo; pCurrentSessionInfo = pNextSessionInfo) {
                // Get the next session info in the list
                pNextSessionInfo = pCurrentSessionInfo->pNextSessionInfo;

                // Free the current session info
                xMemFree(g_NetsyncServerInfo.pNetsyncObject->hMemObject, pCurrentSessionInfo);
            }

            // Get the next session dll in the list
            pNextSessionDll = pCurrentSessionDll->pNextSessionDll;

            // Free the module
#ifdef _XBOX
            pDllMain = (LPDLLMAIN_PROC) FindProcAddress(pCurrentSessionDll->hModule, "DllMain");
            if (NULL != pDllMain) {
                pDllMain(NULL, DLL_PROCESS_DETACH, NULL);
            }
#else
            FreeLibrary(pCurrentSessionDll->hModule);
#endif

            // Free the current session dll
            xMemFree(g_NetsyncServerInfo.pNetsyncObject->hMemObject, pCurrentSessionDll);
        }

        // Reset the server info
        g_NetsyncServerInfo.pSessionDlls = NULL;
        g_NetsyncServerInfo.bStopServer = FALSE;

        // Close the stop event
        CloseHandle(g_NetsyncServerInfo.hStopEvent);
        g_NetsyncServerInfo.hStopEvent = NULL;

        // Free the NETSYNC_OBJECT
        CloseNetsyncObject(g_NetsyncServerInfo.pNetsyncObject);
        g_NetsyncServerInfo.pNetsyncObject = NULL;

        // Close the memory object
        xMemClose(g_NetsyncServerInfo.hMemObject);
        g_NetsyncServerInfo.hMemObject = INVALID_HANDLE_VALUE;
    }

    // Release the netsync server mutex
    ReleaseMutex(g_hNetsyncServerInfoMutex);
}



VOID
WINAPI
NetsyncRemoveClientFromSession(
    IN HANDLE  hSessionObject,
    IN HANDLE  hNetsyncObject,
    IN u_long  ClientInAddr
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Removes a client from the session and sends a netsync stop message to the client

Arguments:

  hSessionObject - Handle to the session
  hNetsyncObject - Handle to the NETSYNC_OBJECT
  ClientInAddr - Address of the new client

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // pCurrentSessionDll is a pointer to the current session dll in the list
    PNETSYNC_SESSION_DLL   pCurrentSessionDll = NULL;
    // pCurrentSessionInfo is a pointer to the current session info in the list
    PNETSYNC_SESSION_INFO  pCurrentSessionInfo = NULL;
    // pCurrentSession is a pointer to the current session in the list
    PNETSYNC_SESSION       pCurrentSession = NULL;
    // pSession is a pointer to the session
    PNETSYNC_SESSION       pSession = NULL;



    // Wait for the netsync server mutex
    WaitForSingleObject(g_hNetsyncServerInfoMutex, INFINITE);

    for (pCurrentSessionDll = g_NetsyncServerInfo.pSessionDlls; (NULL != pCurrentSessionDll) && (NULL == pSession); pCurrentSessionDll = pCurrentSessionDll->pNextSessionDll) {
        for (pCurrentSessionInfo = pCurrentSessionDll->pSessionInfos; (NULL != pCurrentSessionInfo) && (NULL == pSession); pCurrentSessionInfo = pCurrentSessionInfo->pNextSessionInfo) {
            if ((PNETSYNC_SESSION) hSessionObject == pCurrentSessionInfo->pActiveSession) {
                pSession = (PNETSYNC_SESSION) hSessionObject;
                break;
            }

            for (pCurrentSession = pCurrentSessionInfo->pRunningSessions; NULL != pCurrentSession; pCurrentSession = pCurrentSession->pNextSession) {
                if ((PNETSYNC_SESSION) hSessionObject == pCurrentSession) {
                    pSession = (PNETSYNC_SESSION) hSessionObject;
                    break;
                }
            }
        }
    }

    if (NULL != pSession) {
        // Enter the session critical section
        EnterCriticalSection(&pSession->cs);

        // Remove the client
        RemoveClientFromList(pSession, ClientInAddr);

        // Send the stop session messages
        SendStopMessageToPort(hNetsyncObject, FALSE, 1, &ClientInAddr);

        // Leave the session critical section
        LeaveCriticalSection(&pSession->cs);
    }

    // Release the netsync server mutex
    ReleaseMutex(g_hNetsyncServerInfoMutex);
}
