/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  xlogging.c

Abstract:

  This module is the Xbox logging library

Author:

  Steven Kehrli (steveke) 19-Mar-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace xLogNamespace;

namespace xLogNamespace {

// Global variables
// g_hLogInfoMutex is the object to synchronize access to the xlog info
HANDLE     g_hLogInfoMutex = NULL;
// g_LogInfo is the xlog info
XLOG_INFO  g_LogInfo = {
                          INVALID_HANDLE_VALUE,  // hMemObject
                          0,                     // dwRefCount
                          NULL,                  // pWebPostLogFile
                          NULL,                  // lpszHostName_A
                          NULL,                  // lpszDiskModel
                          NULL,                  // lpszDiskSerial
                         };



HANDLE
xCreateLog_X(
    IN  PXLOG_OBJECT_INFO_A  pLogObjectInfo_A,
    IN  PXLOG_OBJECT_INFO_W  pLogObjectInfo_W
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Creates a lob object:
    Creates an XLOG_OBJECT
    Creates a log file, if specified
    Logs and web posts the system configuration, if specified
    Logs and web posts the system state, if specified

Arguments:

  pLogObjectInfo_A - Pointer to the ANSI xlog object info
  pLogObjectInfo_W - Pointer to the UNICODE xlog object info

Return Value:

  HANDLE:
    If the function succeeds, the return value is a handle to the XLOG_OBJECT
    If the function fails, the return value is INVALID_HANDLE_VALUE

------------------------------------------------------------------------------*/
{
    // lpszLogFileName_A is a pointer to the log file name
    LPSTR             lpszLogFileName_A = NULL;
    // lpszHttpServer_A is a pointer to the http server name
    LPSTR             lpszHttpServer_A = NULL;
    // lpszConfigPage_A is a pointer to the http server config page
    LPSTR             lpszConfigPage_A = NULL;
    // lpszStatePage_A is a pointer to the http server state page
    LPSTR             lpszStatePage_A = NULL;
    // lpszLogPage_A is a pointer to the http server log page
    LPSTR             lpszLogPage_A = NULL;
    // lpszWebPostFileName_A is a pointer to the web post log file
    LPSTR             lpszWebPostFileName_A = NULL;
    // sConsoleSocket specifies the console socket
    SOCKET            sConsoleSocket = INVALID_SOCKET;
    // dwLogLevel specifies the log level
    DWORD             dwLogLevel = 0;
    // dwLogOptions specifies the log options
    DWORD             dwLogOptions = 0;

    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT      pLogObject = NULL;
    // dwBufferSize is the size of the XLOG_OBJECT buffer, in bytes
    DWORD             dwBufferSize = sizeof(XLOG_OBJECT);
    // uipOffset is an offset into the XLOG_OBJECT buffer for the strings
    UINT_PTR          uipOffset = sizeof(XLOG_OBJECT);

    // pCurrentWebPostLogFile is a pointer to the current web post log file in the list of web post log files
    PWEBPOST_LOGFILE  pCurrentWebPostLogFile = NULL;

#ifdef _XBOX
    // nResult is the int status code returned by a function
    INT               nResult = 0;
#else
    // RpcStatus is the rpc status code returned by a function
    RPC_STATUS        RpcStatus = RPC_S_OK;
    // OSVersionInfo is the version info
    OSVERSIONINFO     OSVersionInfo;
#endif

    // szDebugString is the debug string
    CHAR              szDebugString[61];
    // dwErrorCode is the last error code
    DWORD             dwErrorCode = ERROR_SUCCESS;



    // Create the xlog info mutex
    g_hLogInfoMutex = CreateMutexA(NULL, FALSE, "xLogInfoMutex");

    // Wait for the xlog info mutex
    WaitForSingleObject(g_hLogInfoMutex, INFINITE);

    // Increment the xLog ref count
    g_LogInfo.dwRefCount++;

    if (1 == g_LogInfo.dwRefCount) {
        // Create the memory object
        g_LogInfo.hMemObject = xMemCreate();
        if (INVALID_HANDLE_VALUE == g_LogInfo.hMemObject) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionFailed;
        }

#ifdef _XBOX
        // Get the disk model and serial number
        g_LogInfo.lpszDiskModel = (LPSTR) xMemAlloc(g_LogInfo.hMemObject, HalDiskModelNumber->Length + 1);

        if (NULL == g_LogInfo.lpszDiskModel) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionFailed;
        }

        strncpy(g_LogInfo.lpszDiskModel, HalDiskModelNumber->Buffer, HalDiskModelNumber->Length);

        g_LogInfo.lpszDiskSerial = (LPSTR) xMemAlloc(g_LogInfo.hMemObject, HalDiskSerialNumber->Length + 1);

        if (NULL == g_LogInfo.lpszDiskSerial) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionFailed;
        }

        strncpy(g_LogInfo.lpszDiskSerial, HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length);
#endif
        // Get the host name and host addr
        g_LogInfo.lpszHostName_A = GetHostName(g_LogInfo.hMemObject);

        if (NULL == g_LogInfo.lpszHostName_A) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionFailed;
        }
    }

    // Get the xlog object info
    if (NULL == pLogObjectInfo_A) {
        if (NULL != pLogObjectInfo_W->lpszLogFileName_W) {
            lpszLogFileName_A = UnicodeToMultiByteString(g_LogInfo.hMemObject, pLogObjectInfo_W->lpszLogFileName_W);
            if (NULL == lpszLogFileName_A) {
                // Get the last error code
                dwErrorCode = GetLastError();

                goto FunctionFailed;
            }
        }

        if (NULL != pLogObjectInfo_W->lpszHttpServer_W) {
            lpszHttpServer_A = UnicodeToMultiByteString(g_LogInfo.hMemObject, pLogObjectInfo_W->lpszHttpServer_W);
            if (NULL == lpszHttpServer_A) {
                // Get the last error code
                dwErrorCode = GetLastError();

                goto FunctionFailed;
            }

            if (NULL != pLogObjectInfo_W->lpszConfigPage_W) {
                lpszConfigPage_A = UnicodeToMultiByteString(g_LogInfo.hMemObject, pLogObjectInfo_W->lpszConfigPage_W);
                if (NULL == lpszConfigPage_A) {
                    // Get the last error code
                    dwErrorCode = GetLastError();

                    goto FunctionFailed;
                }
            }

            if (NULL != pLogObjectInfo_W->lpszStatePage_W) {
                lpszStatePage_A = UnicodeToMultiByteString(g_LogInfo.hMemObject, pLogObjectInfo_W->lpszStatePage_W);
                if (NULL == lpszStatePage_A) {
                    // Get the last error code
                    dwErrorCode = GetLastError();

                    goto FunctionFailed;
                }
            }

            if (NULL != pLogObjectInfo_W->lpszLogPage_W) {
                lpszLogPage_A = UnicodeToMultiByteString(g_LogInfo.hMemObject, pLogObjectInfo_W->lpszLogPage_W);
                if (NULL == lpszLogPage_A) {
                    // Get the last error code
                    dwErrorCode = GetLastError();

                    goto FunctionFailed;
                }
            }

            if (NULL != pLogObjectInfo_W->lpszWebPostFileName_W) {
                lpszWebPostFileName_A = UnicodeToMultiByteString(g_LogInfo.hMemObject, pLogObjectInfo_W->lpszWebPostFileName_W);
                if (NULL == lpszWebPostFileName_A) {
                    // Get the last error code
                    dwErrorCode = GetLastError();

                    goto FunctionFailed;
                }
            }
            else {
                lpszWebPostFileName_A = WEBPOST_LOGFILE_NAME;
            }
        }

        sConsoleSocket = pLogObjectInfo_W->sConsoleSocket;
        dwLogLevel = pLogObjectInfo_W->dwLogLevel;
        dwLogOptions = pLogObjectInfo_W->dwLogOptions;
    }
    else {
        lpszLogFileName_A = pLogObjectInfo_A->lpszLogFileName_A;
        if (NULL != pLogObjectInfo_A->lpszHttpServer_A) {
            lpszHttpServer_A = pLogObjectInfo_A->lpszHttpServer_A;
            lpszConfigPage_A = pLogObjectInfo_A->lpszConfigPage_A;
            lpszStatePage_A = pLogObjectInfo_A->lpszStatePage_A;
            lpszLogPage_A = pLogObjectInfo_A->lpszLogPage_A;
            lpszWebPostFileName_A = pLogObjectInfo_A->lpszWebPostFileName_A;
            if (NULL == lpszWebPostFileName_A) {
                lpszWebPostFileName_A = WEBPOST_LOGFILE_NAME;
            }
        }
        sConsoleSocket = pLogObjectInfo_A->sConsoleSocket;
        dwLogLevel = pLogObjectInfo_A->dwLogLevel;
        dwLogOptions = pLogObjectInfo_A->dwLogOptions;
    }

    // Increase the size of the XLOG_OBJECT buffer to store the strings

    // Critical Sections
    // pcs critical section
    dwBufferSize += sizeof(CRITICAL_SECTION);

    // pWebSocketcs critical section
    dwBufferSize += sizeof(CRITICAL_SECTION);

    // pConsoleSocketcs critical section
    dwBufferSize += sizeof(CRITICAL_SECTION);

    // lpszLogFileName
    if (NULL != lpszLogFileName_A) {
        dwBufferSize += strlen(lpszLogFileName_A) + 1;
    }

    // lpszHttpServer
    if (NULL != lpszHttpServer_A) {
        dwBufferSize += strlen(lpszHttpServer_A) + 1;

        // lpszConfigPage
        if (NULL != lpszConfigPage_A) {
            dwBufferSize += strlen(lpszConfigPage_A) + 1;
        }

        // lpszStatePage
        if (NULL != lpszStatePage_A) {
            dwBufferSize += strlen(lpszStatePage_A) + 1;
        }

        // lpszLogPage
        if (NULL != lpszLogPage_A) {
            dwBufferSize += strlen(lpszLogPage_A) + 1;
        }

        // lpszWebPostFileName
        if (NULL != lpszWebPostFileName_A) {
            dwBufferSize += strlen(lpszWebPostFileName_A) + 1;
        }
    }

    // Allocate the memory for the XLOG_OBJECT buffer
    pLogObject = (PXLOG_OBJECT) xMemAlloc(g_LogInfo.hMemObject, dwBufferSize);
    if (NULL == pLogObject) {
        // Get the last error code
        dwErrorCode = GetLastError();

        goto FunctionFailed;
    }

    // Initialize the XLOG_OBJECT
    pLogObject->hLogFile = INVALID_HANDLE_VALUE;
    pLogObject->hWebPostFile = INVALID_HANDLE_VALUE;
    pLogObject->sConsoleSocket = INVALID_SOCKET;

    // Set the memory object
    pLogObject->hMemObject = g_LogInfo.hMemObject;

    // Initialize the critical sections
    // pcs critical section
    pLogObject->pcs = (CRITICAL_SECTION *) ((UINT_PTR) pLogObject + uipOffset);
    uipOffset += sizeof(CRITICAL_SECTION);
    InitializeCriticalSection(pLogObject->pcs);

    // pConsoleSocket critical section
    pLogObject->pConsoleSocketcs = (CRITICAL_SECTION *) ((UINT_PTR) pLogObject + uipOffset);
    uipOffset += sizeof(CRITICAL_SECTION);
    InitializeCriticalSection(pLogObject->pConsoleSocketcs);

#ifdef _XBOX
    // Increment the net stack reference count
    if (FALSE == AddRefNet()) {
        // Get the last error code
        dwErrorCode = GetLastError();

        goto FunctionFailed;
    }

    // Create the UUID
    nResult = XNetRandom((BYTE *) &pLogObject->ObjectUUID, sizeof(pLogObject->ObjectUUID));
    if (0 != nResult) {
        // Get the last error code
        dwErrorCode = nResult;
    }

    // Terminate the net stack
    ReleaseNet();

    if (0 != nResult) {
        goto FunctionFailed;
    }
#else
    // Create the UUID
    RpcStatus = UuidCreate(&pLogObject->ObjectUUID);
    if (RPC_S_OK != RpcStatus) {
        // Set the last error code
        dwErrorCode = (DWORD) RpcStatus;

        goto FunctionFailed;
    }
#endif

    // Get the UNICODE string representation of the object guid
    sprintf(pLogObject->szObjectUUID_A, "{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", pLogObject->ObjectUUID.Data1, pLogObject->ObjectUUID.Data2, pLogObject->ObjectUUID.Data3, pLogObject->ObjectUUID.Data4[0], pLogObject->ObjectUUID.Data4[1], pLogObject->ObjectUUID.Data4[2], pLogObject->ObjectUUID.Data4[3], pLogObject->ObjectUUID.Data4[4], pLogObject->ObjectUUID.Data4[5], pLogObject->ObjectUUID.Data4[6], pLogObject->ObjectUUID.Data4[7]);

    // Copy the host name
    strncpy(pLogObject->szHostName_A, g_LogInfo.lpszHostName_A, BOX_ID_LENGTH);

#ifdef _XBOX
    // Set the OS type
    pLogObject->dwPlatformId = OS_PLATFORM_XBOX;

    // Get the version info
    pLogObject->KernelDebug = !!(0x8000 & XboxKrnlVersion->Qfe);
    pLogObject->KernelVersion = XboxKrnlVersion->Build;

    pLogObject->XapiDebug = XeImageHeader()->XapiLibraryVersion->DebugBuild;
    pLogObject->XapiVersion = XeImageHeader()->XapiLibraryVersion->BuildVersion;
#else
    // Set the OS type
    pLogObject->dwPlatformId = OS_PLATFORM_NT;

    // Get the version info
    ZeroMemory(&OSVersionInfo, sizeof(OSVersionInfo));
    OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVersionInfo);
    if (FALSE == GetVersionEx(&OSVersionInfo)) {
        // Get the last error code
        dwErrorCode = GetLastError();

        goto FunctionFailed;
    }

    // Copy the version info
    pLogObject->dwMajorVersion = OSVersionInfo.dwMajorVersion;
    pLogObject->dwMinorVersion = OSVersionInfo.dwMinorVersion;
    pLogObject->dwBuildNumber = OSVersionInfo.dwBuildNumber;
#endif

    // Get the web server info, if specified
    if (NULL != lpszHttpServer_A) {
        // Get the web address
        pLogObject->HttpAddr = inet_addr(lpszHttpServer_A);
        if (INADDR_NONE == pLogObject->HttpAddr) {
            // Set the last error code
            dwErrorCode = ERROR_INVALID_PARAMETER;

            goto FunctionFailed;
        }
    }

    // Create the log file, if specified
    if (NULL != lpszLogFileName_A) {
        // Create the log file
        pLogObject->hLogFile = CreateFileA(lpszLogFileName_A, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, (0 != (dwLogOptions & XLO_REFRESH)) ? CREATE_ALWAYS : OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        // The last error code is always set by CreateFile
        dwErrorCode = GetLastError();

        if (INVALID_HANDLE_VALUE == pLogObject->hLogFile) {
            goto FunctionFailed;
        }

        if ((0 == (dwLogOptions & XLO_REFRESH)) && (ERROR_ALREADY_EXISTS == dwErrorCode)) {
            // File already exists, so set the file pointer to the end of the file
            SetFilePointer(pLogObject->hLogFile, 0, NULL, FILE_END);
        }
    }

    // Create the web post log file, if specified
    if (NULL != lpszWebPostFileName_A) {
        // Set the current web post log file to the head of the web post log file list
        pCurrentWebPostLogFile = g_LogInfo.pWebPostLogFile;

        while (NULL != pCurrentWebPostLogFile) {
            if (0 == _stricmp(lpszWebPostFileName_A, pCurrentWebPostLogFile->szWebPostFileName_A)) {
                break;
            }

            // Set the current web post log file to the next web post log file in the list
            pCurrentWebPostLogFile = pCurrentWebPostLogFile->pNextWebPostLogFile;
        }

        if (NULL == pCurrentWebPostLogFile) {
            // The web post log file was not found

            // Create the web post log file
            pLogObject->hWebPostFile = CreateFileA(lpszWebPostFileName_A, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (INVALID_HANDLE_VALUE == pLogObject->hWebPostFile) {
                // Get the last error code
                dwErrorCode = GetLastError();

                goto FunctionFailed;
            }

            // Create the web post log file element
            pCurrentWebPostLogFile = (PWEBPOST_LOGFILE) xMemAlloc(g_LogInfo.hMemObject, sizeof(WEBPOST_LOGFILE));
            if (NULL == pCurrentWebPostLogFile) {
                // Get the last error code
                dwErrorCode = GetLastError();

                // Flush the file buffers to prevent corruption
                FlushFileBuffers(pLogObject->hWebPostFile);

                // Close the log file
                CloseHandle(pLogObject->hWebPostFile);
                pLogObject->hWebPostFile = INVALID_HANDLE_VALUE;

                // Delete the web post log file
                DeleteFileA(lpszWebPostFileName_A);

                goto FunctionFailed;
            }

            // Copy the web post log file name
            strncpy(pCurrentWebPostLogFile->szWebPostFileName_A, lpszWebPostFileName_A, sizeof(pCurrentWebPostLogFile->szWebPostFileName_A));

            // Set the ref count
            pCurrentWebPostLogFile->dwRefCount = 1;

            // Set the handle to the web post log file
            pCurrentWebPostLogFile->hFile = pLogObject->hWebPostFile;

            // Set the pointer to the next web post log file
            pCurrentWebPostLogFile->pNextWebPostLogFile = g_LogInfo.pWebPostLogFile;
            if (NULL != g_LogInfo.pWebPostLogFile) {
                g_LogInfo.pWebPostLogFile->pPrevWebPostLogFile = pCurrentWebPostLogFile;
            }

            // Set the head of the web post log file list to the current web post log file
            g_LogInfo.pWebPostLogFile = pCurrentWebPostLogFile;
        }
        else {
            // The web post log file was found

            // Increment the ref count
            pCurrentWebPostLogFile->dwRefCount++;

            // Get the handle to the web post log file
            pLogObject->hWebPostFile = pCurrentWebPostLogFile->hFile;
        }
    }

    // Set the XLOG_OBJECT members

    // lpszLogFileName
    if (NULL != lpszLogFileName_A) {
        pLogObject->lpszLogFileName_A = (LPSTR) ((UINT_PTR) pLogObject + uipOffset);
        strcpy(pLogObject->lpszLogFileName_A, lpszLogFileName_A);
        pLogObject->dwLogFileNameLen = strlen(pLogObject->lpszLogFileName_A) + 1;
        uipOffset += pLogObject->dwLogFileNameLen;
    }

    // lpszHttpServer
    if (NULL != lpszHttpServer_A) {
        pLogObject->lpszHttpServer_A = (LPSTR) ((UINT_PTR) pLogObject + uipOffset);
        strcpy(pLogObject->lpszHttpServer_A, lpszHttpServer_A);
        pLogObject->dwHttpServerLen = strlen(pLogObject->lpszHttpServer_A) + 1;
        uipOffset += pLogObject->dwHttpServerLen;

        // lpszConfigPage
        if (NULL != lpszConfigPage_A) {
            pLogObject->lpszConfigPage_A = (LPSTR) ((UINT_PTR) pLogObject + uipOffset);
            strcpy(pLogObject->lpszConfigPage_A, lpszConfigPage_A);
            pLogObject->dwConfigPageLen = strlen(pLogObject->lpszConfigPage_A) + 1;
            uipOffset += pLogObject->dwConfigPageLen;
        }

        // lpszStatePage
        if (NULL != lpszStatePage_A) {
            pLogObject->lpszStatePage_A = (LPSTR) ((UINT_PTR) pLogObject + uipOffset);
            strcpy(pLogObject->lpszStatePage_A, lpszStatePage_A);
            pLogObject->dwStatePageLen = strlen(pLogObject->lpszStatePage_A) + 1;
            uipOffset += pLogObject->dwStatePageLen;
        }

        // lpszLogPage
        if (NULL != lpszLogPage_A) {
            pLogObject->lpszLogPage_A = (LPSTR) ((UINT_PTR) pLogObject + uipOffset);
            strcpy(pLogObject->lpszLogPage_A, lpszLogPage_A);
            pLogObject->dwLogPageLen = strlen(pLogObject->lpszLogPage_A) + 1;
            uipOffset += pLogObject->dwLogPageLen;
        }

        // lpszWebPostFileName
        if (NULL != lpszWebPostFileName_A) {
            pLogObject->lpszWebPostFileName_A = (LPSTR) ((UINT_PTR) pLogObject + uipOffset);
            strcpy(pLogObject->lpszWebPostFileName_A, lpszWebPostFileName_A);
            pLogObject->dwWebPostFileNameLen = strlen(pLogObject->lpszWebPostFileName_A) + 1;
            uipOffset += pLogObject->dwWebPostFileNameLen;
        }
    }

    // sConsoleSocket
    pLogObject->sConsoleSocket = sConsoleSocket;

    // dwLogLevel
    pLogObject->dwLogLevel = dwLogLevel;

    // dwLogOptions
    pLogObject->dwLogOptions = dwLogOptions;

    // Log the start banner
    if (FALSE == LogBanner(pLogObject, "xLog Started", NULL)) {
        // Get the last error code
        dwErrorCode = GetLastError();

        goto FunctionFailed;
    }

    // Log the system configuration, if specified
    if (0 != (dwLogOptions & XLO_CONFIG)) {
        if (FALSE == xLogConfiguration(pLogObject, NULL)) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionFailed;
        }
    }

    // Web post the system configuration, if specified
    if (NULL != pLogObject->lpszConfigPage_A) {
        xPostConfiguration(pLogObject);
    }

    // Log the system state, if specified
    if (0 != (dwLogOptions & XLO_STATE)) {
        if (FALSE == xLogResourceStatus(pLogObject, NULL)) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionFailed;
        }
    }

    // Log a blank line
    if (FALSE == FormatAndWriteString(pLogObject, "\r\n")) {
        // Get the last error code
        dwErrorCode = GetLastError();

        goto FunctionFailed;
    }

    if (INVALID_HANDLE_VALUE != pLogObject->hLogFile) {
        // Flush the file buffers to prevent corruption
        FlushFileBuffers(pLogObject->hLogFile);
    }

    // Get the xlog object info
    if (NULL != pLogObjectInfo_W) {
        if (NULL != lpszLogFileName_A) {
            xMemFree(g_LogInfo.hMemObject, lpszLogFileName_A);
        }

        if (NULL != lpszHttpServer_A) {
            xMemFree(g_LogInfo.hMemObject, lpszHttpServer_A);
        }

        if (NULL != lpszConfigPage_A) {
            xMemFree(g_LogInfo.hMemObject, lpszConfigPage_A);
        }

        if (NULL != lpszStatePage_A) {
            xMemFree(g_LogInfo.hMemObject, lpszStatePage_A);
        }

        if (NULL != lpszLogPage_A) {
            xMemFree(g_LogInfo.hMemObject, lpszLogPage_A);
        }

        if ((NULL != pLogObjectInfo_W->lpszWebPostFileName_W) && (NULL != lpszWebPostFileName_A)) {
            xMemFree(g_LogInfo.hMemObject, lpszWebPostFileName_A);
        }
    }

    // Release the xlog info mutex
    ReleaseMutex(g_hLogInfoMutex);

    return pLogObject;

FunctionFailed:
    if (NULL != pLogObject) {
        // Decrement the ref count of the web post log file, if necessary
        if (INVALID_HANDLE_VALUE != pLogObject->hWebPostFile) {
            ReleaseWebPostLogFile(pLogObject->lpszWebPostFileName_A);
        }

        // Close the log file, if necessary
        if (INVALID_HANDLE_VALUE != pLogObject->hLogFile) {
            // Flush the file buffers to prevent corruption
            FlushFileBuffers(pLogObject->hLogFile);

            // Close the log file
            CloseHandle(pLogObject->hLogFile);
        }

        // Delete the critical sections
        DeleteCriticalSection(pLogObject->pConsoleSocketcs);
        DeleteCriticalSection(pLogObject->pcs);

        // Free the XLOG_OBJECT buffer
        xMemFree(g_LogInfo.hMemObject, pLogObject);
    }

    // Free the xlog object info, if necessary
    if (NULL == pLogObjectInfo_A) {
        if (NULL != lpszLogFileName_A) {
            xMemFree(g_LogInfo.hMemObject, lpszLogFileName_A);
        }

        if (NULL != lpszHttpServer_A) {
            xMemFree(g_LogInfo.hMemObject, lpszHttpServer_A);
        }

        if (NULL != lpszConfigPage_A) {
            xMemFree(g_LogInfo.hMemObject, lpszConfigPage_A);
        }

        if (NULL != lpszStatePage_A) {
            xMemFree(g_LogInfo.hMemObject, lpszStatePage_A);
        }

        if (NULL != lpszLogPage_A) {
            xMemFree(g_LogInfo.hMemObject, lpszLogPage_A);
        }

        if (NULL != lpszWebPostFileName_A) {
            xMemFree(g_LogInfo.hMemObject, lpszWebPostFileName_A);
        }
    }

    if (1 == g_LogInfo.dwRefCount) {
#ifdef _XBOX
        // Free the disk model and serial number
        if (NULL != g_LogInfo.lpszDiskModel) {
            xMemFree(g_LogInfo.hMemObject, g_LogInfo.lpszDiskModel);
            g_LogInfo.lpszDiskModel = NULL;
        }

        if (NULL != g_LogInfo.lpszDiskSerial) {
            xMemFree(g_LogInfo.hMemObject, g_LogInfo.lpszDiskSerial);
            g_LogInfo.lpszDiskSerial = NULL;
        }
#endif
        // Free the host name
        if (NULL != g_LogInfo.lpszHostName_A) {
            xMemFree(g_LogInfo.hMemObject, g_LogInfo.lpszHostName_A);
            g_LogInfo.lpszHostName_A = NULL;
        }

        // Close the memory object
        if (INVALID_HANDLE_VALUE != g_LogInfo.hMemObject) {
            xMemClose(g_LogInfo.hMemObject);
            g_LogInfo.hMemObject = INVALID_HANDLE_VALUE;
        }
    }

    // Decrement the xLog ref count
    g_LogInfo.dwRefCount--;

    // Release the xlog info mutex
    ReleaseMutex(g_hLogInfoMutex);

    // Set the last error code
    SetLastError(dwErrorCode);

    return INVALID_HANDLE_VALUE;
}

} // namespace xLogNamespace



HANDLE
WINAPI
xCreateLog_W(
    IN  LPWSTR              lpszLogFileName_W,
    IN  PWEB_SERVER_INFO_W  pWebServerInfo_W,
    IN  SOCKET              sConsoleSocket,
    IN  DWORD               dwLogLevel,
    IN  DWORD               dwLogOptions
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  UNICODE version of xCreateLog()

Arguments:

  lpszLogFileName_W - Pointer to a null-terminated string (UNICODE) that specifies the filename to which the log output will be directed.  This member is NULL if no log file will be created.
  pWebServerInfo - Pointer to the web server information (http server and web pages)
  sConsoleSocket - Socket descriptor used for directing output to a remote console.
  dwLogLevel - Specifies the logging level for this log object.  It can be any combination of the XLL_ #defines.
  dwLogOptions - Specifies the logging options for this log object.  It can be any combination of the XLO_ #defines.

Return Value:

  HANDLE:
    If the function succeeds, the return value is a handle to the XLOG_OBJECT
    If the function fails, the return value is INVALID_HANDLE_VALUE

------------------------------------------------------------------------------*/
{
    // xLogObjectInfo_W is the UNICODE xlog object info
    XLOG_OBJECT_INFO_W  xLogObjectInfo_W;



    // Initialize the xlog object info
    ZeroMemory(&xLogObjectInfo_W, sizeof(xLogObjectInfo_W));

    // Set the xlog object info
    xLogObjectInfo_W.lpszLogFileName_W = lpszLogFileName_W;

    if (NULL != pWebServerInfo_W) {
        xLogObjectInfo_W.lpszHttpServer_W = pWebServerInfo_W->lpszHttpServer;
        xLogObjectInfo_W.lpszConfigPage_W = pWebServerInfo_W->lpszConfigPage;
        xLogObjectInfo_W.lpszStatePage_W = pWebServerInfo_W->lpszStatePage;
        xLogObjectInfo_W.lpszLogPage_W = pWebServerInfo_W->lpszLogPage;
        xLogObjectInfo_W.lpszWebPostFileName_W = pWebServerInfo_W->lpszWebPostFileName;
    }

    xLogObjectInfo_W.sConsoleSocket = sConsoleSocket;
    xLogObjectInfo_W.dwLogLevel = dwLogLevel;
    xLogObjectInfo_W.dwLogOptions = dwLogOptions;

    // Create the log object
    return xCreateLog_X(NULL, &xLogObjectInfo_W);
}



HANDLE
WINAPI
xCreateLog_A(
    IN  LPSTR               lpszLogFileName_A,
    IN  PWEB_SERVER_INFO_A  pWebServerInfo_A,
    IN  SOCKET              sConsoleSocket,
    IN  DWORD               dwLogLevel,
    IN  DWORD               dwLogOptions
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  ANSI version of xCreateLog()

Arguments:

  lpszLogFileName_A - Pointer to a null-terminated string (ANSI) that specifies the filename to which the log output will be directed.  This member is NULL if no log file will be created.
  pWebServerInfo - Pointer to the web server information (http server and web pages)
  sConsoleSocket - Socket descriptor used for directing output to a remote console.
  dwLogLevel - Specifies the logging level for this log object.  It can be any combination of the XLL_ #defines.
  dwLogOptions - Specifies the logging options for this log object.  It can be any combination of the XLO_ #defines.

Return Value:

  HANDLE:
    If the function succeeds, the return value is a handle to the XLOG_OBJECT
    If the function fails, the return value is INVALID_HANDLE_VALUE

------------------------------------------------------------------------------*/
{
    // xLogObjectInfo_A is the ANSI xlog object info
    XLOG_OBJECT_INFO_A  xLogObjectInfo_A;



    // Initialize the xlog object info
    ZeroMemory(&xLogObjectInfo_A, sizeof(xLogObjectInfo_A));

    // Set the xlog object info
    xLogObjectInfo_A.lpszLogFileName_A = lpszLogFileName_A;

    if (NULL != pWebServerInfo_A) {
        xLogObjectInfo_A.lpszHttpServer_A = pWebServerInfo_A->lpszHttpServer;
        xLogObjectInfo_A.lpszConfigPage_A = pWebServerInfo_A->lpszConfigPage;
        xLogObjectInfo_A.lpszStatePage_A = pWebServerInfo_A->lpszStatePage;
        xLogObjectInfo_A.lpszLogPage_A = pWebServerInfo_A->lpszLogPage;
        xLogObjectInfo_A.lpszWebPostFileName_A = pWebServerInfo_A->lpszWebPostFileName;
    }

    xLogObjectInfo_A.sConsoleSocket = sConsoleSocket;
    xLogObjectInfo_A.dwLogLevel = dwLogLevel;
    xLogObjectInfo_A.dwLogOptions = dwLogOptions;

    // Create the log object
    return xCreateLog_X(&xLogObjectInfo_A, NULL);
}



HANDLE
WINAPI
xSetLogSocket(
    IN  HANDLE  hLog,
    IN  SOCKET  sConsoleSocket
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Changes the console socket

Arguments:

  hLog - Handle to the XLOG_OBJECT
  sConsoleSocket - Socket descriptor used for directing output to a remote console.

Return Value:

  HANDLE:
    The return value is the handle to the changed XLOG_OBJECT

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT  pLogObject = (PXLOG_OBJECT) hLog;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        return hLog;
    }

    // Wait for access to the console socket critical section
    EnterCriticalSection(pLogObject->pConsoleSocketcs);

    // Change the console socket
    pLogObject->sConsoleSocket = sConsoleSocket;

    // Release access to the console socket critical section
    LeaveCriticalSection(pLogObject->pConsoleSocketcs);

    return pLogObject;
}



BOOL
WINAPI
xCloseLog(
    IN  HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Closes a lob object:
    Logs the variation results
    Logs and web posts the system state, if specified
    Closes the log file, if necessary
    Frees the XLOG_OBJECT

Arguments:

  hLog - Handle to the XLOG_OBJECT

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT        pLogObject = (PXLOG_OBJECT) hLog;
    // pNextThreadInfo is a pointer to the next thread info in the list
    PTHREAD_INFO        pNextThreadInfo = NULL;
    // pNextVariationElement is a pointer to the next variation element in the lsit
    PVARIATION_ELEMENT  pNextVariationElement = NULL;
    // pNextWebPostLogFile is a pointer to the next web post log file in the list
    PWEBPOST_LOGFILE    pNextWebPostLogFile = NULL;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Create the xlog info mutex
    g_hLogInfoMutex = CreateMutexA(NULL, FALSE, "xLogInfoMutex");

    // Wait for the xlog info mutex
    WaitForSingleObject(g_hLogInfoMutex, INFINITE);

    // Wait for access to the XLOG_OBJECT
    EnterCriticalSection(pLogObject->pcs);

    while (NULL != pLogObject->pThreadInfo) {
        // Set the pointer to the next thread info in the list
        pNextThreadInfo = pLogObject->pThreadInfo->pNextThreadInfo;
        if (NULL != pNextThreadInfo) {
            pNextThreadInfo->pPrevThreadInfo = NULL;
        }

        // Free the variation info
        ASSERT(NULL == pLogObject->pThreadInfo->pVariationList);
        while (NULL != pLogObject->pThreadInfo->pVariationList) {
            // Set the pointer to the next variation element in the list
            pNextVariationElement = pLogObject->pThreadInfo->pVariationList->pNextVariationElement;
            if (NULL != pNextVariationElement) {
                pNextVariationElement->pPrevVariationElement = NULL;
            }

            // Free the variation element
            xMemFree(pLogObject->hMemObject, pLogObject->pThreadInfo->pVariationList);

            // Set the pointer to the next variation element in the list
            pLogObject->pThreadInfo->pVariationList = pNextVariationElement;
        }

        // Free the function info
        if (NULL != pLogObject->pThreadInfo->pFunctionInfo) {
            xMemFree(pLogObject->hMemObject, pLogObject->pThreadInfo->pFunctionInfo);
            pLogObject->pThreadInfo->pFunctionInfo = NULL;
        }

        // Free the current thread info
        xMemFree(pLogObject->hMemObject, pLogObject->pThreadInfo);

        // Set the pointer to the next thread info in the list
        pLogObject->pThreadInfo = pNextThreadInfo;
    }

    // Release access to the XLOG_OBJECT
    LeaveCriticalSection(pLogObject->pcs);

    // Log the variation results
    xLogResults(hLog, NULL);

    // Log the system state, if specified
    if (0 != (pLogObject->dwLogOptions & XLO_STATE)) {
        xLogResourceStatus(hLog, NULL);
    }

    // Log the end banner
    LogBanner(hLog, "xLog Ended", NULL);

    // Decrement the ref count of the web post log file, if necessary
    if (INVALID_HANDLE_VALUE != pLogObject->hWebPostFile) {
        ReleaseWebPostLogFile(pLogObject->lpszWebPostFileName_A);
    }

    // Close the log file, if necessary
    if (INVALID_HANDLE_VALUE != pLogObject->hLogFile) {
        // Flush the file buffers to prevent corruption
        FlushFileBuffers(pLogObject->hLogFile);

        // Close the log file
        CloseHandle(pLogObject->hLogFile);
    }

    // Delete the critical sections
    DeleteCriticalSection(pLogObject->pConsoleSocketcs);
    DeleteCriticalSection(pLogObject->pcs);

    // Free the XLOG_OBJECT buffer
    xMemFree(g_LogInfo.hMemObject, pLogObject);

    // Decrement the xLog ref count
    g_LogInfo.dwRefCount--;

    if (0 == g_LogInfo.dwRefCount) {
        while (NULL != g_LogInfo.pWebPostLogFile) {
            // Assert the web post log file ref count is 0
            ASSERT(0 == g_LogInfo.pWebPostLogFile->dwRefCount);

            // Get the next web post log file in the list
            pNextWebPostLogFile = g_LogInfo.pWebPostLogFile->pNextWebPostLogFile;
            if (NULL != pNextWebPostLogFile) {
                pNextWebPostLogFile->pPrevWebPostLogFile = NULL;
            }

            // Flush the file buffers to prevent corruption
            FlushFileBuffers(g_LogInfo.pWebPostLogFile->hFile);

            // Upload the current web post log file
            if (TRUE == UploadWebPostLogFile(g_LogInfo.pWebPostLogFile->hFile)) {
                // Close the web post log file
                CloseHandle(g_LogInfo.pWebPostLogFile->hFile);

                // Delete the web post log file
                DeleteFileA(g_LogInfo.pWebPostLogFile->szWebPostFileName_A);
            }
            else {
                // Close the web post log file
                CloseHandle(g_LogInfo.pWebPostLogFile->hFile);
            }

            // Free the current web post log file
            xMemFree(g_LogInfo.hMemObject, g_LogInfo.pWebPostLogFile);

            // Set the pointer to the next web post log file in the list
            g_LogInfo.pWebPostLogFile = pNextWebPostLogFile;
        }

#ifdef _XBOX
        // Free the disk model and serial number
        if (NULL != g_LogInfo.lpszDiskModel) {
            xMemFree(g_LogInfo.hMemObject, g_LogInfo.lpszDiskModel);
            g_LogInfo.lpszDiskModel = NULL;
        }

        if (NULL != g_LogInfo.lpszDiskSerial) {
            xMemFree(g_LogInfo.hMemObject, g_LogInfo.lpszDiskSerial);
            g_LogInfo.lpszDiskSerial = NULL;
        }
#endif
        // Free the host name
        if (NULL != g_LogInfo.lpszHostName_A) {
            xMemFree(g_LogInfo.hMemObject, g_LogInfo.lpszHostName_A);
            g_LogInfo.lpszHostName_A = NULL;
        }

        // Close the memory object
        if (INVALID_HANDLE_VALUE != g_LogInfo.hMemObject) {
            xMemClose(g_LogInfo.hMemObject);
            g_LogInfo.hMemObject = INVALID_HANDLE_VALUE;
        }
    }

    // Release the xlog info mutex
    ReleaseMutex(g_hLogInfoMutex);

    return TRUE;
}



namespace xLogNamespace {

VOID
ReleaseWebPostLogFile(
    IN  LPSTR   lpszWebPostFileName_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Decrements the reference count of a web post log file

Arguments:

  lpszWebPostFileName_A - Pointer to a null-terminated string (ANSI) that specifies the web post log file

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // pCurrentWebPostLogFile is a pointer to the current web post log file in the list
    PWEBPOST_LOGFILE  pCurrentWebPostLogFile = NULL;



    // Set the pointer to the head of the web post log file list
    pCurrentWebPostLogFile = g_LogInfo.pWebPostLogFile;

    while (NULL != pCurrentWebPostLogFile) {
        if (0 == _stricmp(lpszWebPostFileName_A, pCurrentWebPostLogFile->szWebPostFileName_A)) {
            // Decrement the ref count
            pCurrentWebPostLogFile->dwRefCount--;

            break;
        }

        // Set the pointer to the next web post log file in the list
        pCurrentWebPostLogFile = pCurrentWebPostLogFile->pNextWebPostLogFile;
    }
}



BOOL
UploadWebPostLogFile(
    IN  HANDLE  hFile
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Posts the results cached in the file specified by hFile

Arguments:

  hFile - Handle to the web post log file

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // bNetStackInitialized specifies if the net stack is initialized
    BOOL         bNetStackInitialized = FALSE;
    // szWebPostLen_A is the web post length
    CHAR         szWebPostLen_A[6];
    // szHttpAddr_A is the string representation of the http server host address
    CHAR         szHttpAddr_A[13];
    // szWebPost_A is the web post
    CHAR         szWebPost_A[MAX_WEBPOST_LENGTH];
    // HttpAddr is the host address of the http server, in network byte order.
    u_long       HttpAddr;
    // dwByte is a counter to enumerate each byte of the http server host address, from high to low
    DWORD        dwByte;
    // pWebSockets is a pointer to the list of web sockets
    PWEB_SOCKET  pWebSockets = NULL;
    // pCurrentWebSocket is a pointer to the current web socket in the list
    PWEB_SOCKET  pCurrentWebSocket = NULL;
    // dwBytesRead is the number of bytes read from the file
    DWORD        dwBytesRead;
    // dwBytesToRead is the number of bytes to read from the file
    DWORD        dwBytesToRead;
    // dwErrorCode is the last error code
    DWORD        dwErrorCode = ERROR_SUCCESS;



    // Increment the net stack reference count
    bNetStackInitialized = AddRefNet();
    if (FALSE == bNetStackInitialized) {
        // Get the last error code
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Set the file pointer to the end of the file
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

    while (TRUE) {
        // Get the web post length
        ZeroMemory(szWebPostLen_A, sizeof(szWebPostLen_A));
        if (FALSE == ReadFile(hFile, szWebPostLen_A, 5, &dwBytesRead, NULL)) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionExit;
        }

        if (0 == dwBytesRead) {
            break;
        }

        // Calculate the length of the web post
        dwBytesToRead = ((szWebPostLen_A[0] - '0') * 100) + ((szWebPostLen_A[1] - '0') * 10) + (szWebPostLen_A[2] - '0') + 2;

        // Get the http server host address
        ZeroMemory(szHttpAddr_A, sizeof(szHttpAddr_A));
        if (FALSE == ReadFile(hFile, szHttpAddr_A, 12, &dwBytesRead, NULL)) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionExit;
        }

        // Calculate the host address of the http server
        for (HttpAddr = 0, dwByte = 0; dwByte < 8; dwByte++) {
            if (('a' <= szHttpAddr_A[2 + dwByte]) && ('f' >= szHttpAddr_A[2 + dwByte])) {
                HttpAddr = (HttpAddr << 4) + 10 + (szHttpAddr_A[2 + dwByte] - 'a');
            }
            else if (('A' <= szHttpAddr_A[2 + dwByte]) && ('F' >= szHttpAddr_A[2 + dwByte])) {
                HttpAddr = (HttpAddr << 4) + 10 + (szHttpAddr_A[2 + dwByte] - 'A');
            }
            else {
                HttpAddr = (HttpAddr << 4) + (szHttpAddr_A[2 + dwByte] - '0');
            }
        }

        // Get the web post
        ZeroMemory(szWebPost_A, sizeof(szWebPost_A));
        if (FALSE == ReadFile(hFile, szWebPost_A, dwBytesToRead, &dwBytesRead, NULL)) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionExit;
        }
        szWebPost_A[dwBytesToRead - 1] = '\0';
        szWebPost_A[dwBytesToRead - 2] = '\0';

        // Check if a connection to the http server exists
        pCurrentWebSocket = pWebSockets;

        while (NULL != pCurrentWebSocket) {
            if (HttpAddr == pCurrentWebSocket->HttpAddr) {
                break;
            }

            // Set the current web socket to the next web socket in the list
            pCurrentWebSocket = pCurrentWebSocket->pNextWebSocket;
        }

        if (NULL == pCurrentWebSocket) {
            // A connection to the http server does not exist

            // Create the web socket
            pCurrentWebSocket = (PWEB_SOCKET) xMemAlloc(g_LogInfo.hMemObject, sizeof(WEB_SOCKET));
            if (NULL == pCurrentWebSocket) {
                // Get the last error code
                dwErrorCode = GetLastError();

                goto FunctionExit;
            }

            // Add the new web socket to the head of the list
            pCurrentWebSocket->pNextWebSocket = pWebSockets;
            if (NULL != pWebSockets) {
                pWebSockets->pPrevWebSocket = pCurrentWebSocket;
            }

            // Set the head of the web socket list to the current web socket
            pWebSockets = pCurrentWebSocket;

            // Set the host address of the http server
            pCurrentWebSocket->HttpAddr = HttpAddr;

            // Set the web socket
            pCurrentWebSocket->sWebSocket = INVALID_SOCKET;
        }

        if (INVALID_SOCKET == pCurrentWebSocket->sWebSocket) {
            // Connect to the web server
            pCurrentWebSocket->sWebSocket = ConnectWebServer(HttpAddr);
        }

        // Send the web post
        if ((INVALID_SOCKET == pCurrentWebSocket->sWebSocket) || (FALSE == PostString(pCurrentWebSocket->sWebSocket, szWebPost_A, strlen(szWebPost_A)))) {
            // Get the last error code
            dwErrorCode = GetLastError();

            shutdown(pCurrentWebSocket->sWebSocket, SD_BOTH);
            closesocket(pCurrentWebSocket->sWebSocket);
            pCurrentWebSocket->sWebSocket = INVALID_SOCKET;
        }
    }

FunctionExit:
    // Close the web sockets
    while (NULL != pWebSockets) {
        pCurrentWebSocket = pWebSockets;
        pWebSockets = pWebSockets->pNextWebSocket;
        if (NULL != pWebSockets) {
            pWebSockets->pPrevWebSocket = NULL;
        }

        // Close the current web socket
        shutdown(pCurrentWebSocket->sWebSocket, SD_BOTH);
        closesocket(pCurrentWebSocket->sWebSocket);

        // Free the current web socket
        xMemFree(g_LogInfo.hMemObject, pCurrentWebSocket);
    }

    // Decrement the net stack reference count, if necessary
    if (TRUE == bNetStackInitialized) {
        ReleaseNet();
    }

    if (ERROR_SUCCESS != dwErrorCode) {
        // Set the last error code
        SetLastError(dwErrorCode);
    }

    return (ERROR_SUCCESS == dwErrorCode);
}

} // namespace xLogNamespace



BOOL
WINAPI
xSetOwnerAlias(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszOwnerAlias_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sets the owner alias of the log object

Arguments:

  hLog - Handle to the XLOG_OBJECT
  lpszOwnerAlias_A - Pointer to a null-terminated string (ANSI) that specifies the owner alias

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT  pLogObject = (PXLOG_OBJECT) hLog;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Initialize the owner alias
    ZeroMemory(pLogObject->szOwnerAlias_A, sizeof(pLogObject->szOwnerAlias_A));

    // Copy the owner alias
    if (NULL != lpszOwnerAlias_A) {
        strncpy(pLogObject->szOwnerAlias_A, lpszOwnerAlias_A, OWNER_ALIAS_LENGTH);
    }

    return TRUE;
}



BOOL
WINAPI
xSetComponent(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszComponentName_A,
    IN  LPSTR   lpszSubcomponentName_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sets the component and subcomponent used in logging for the thread

Arguments:

  hLog - Handle to the XLOG_OBJECT
  lpszComponentName_A - Pointer to a null-terminated string (ANSI) that specifies the component name
  lpszSubcomponentName_A - Pointer to a null-terminated string (ANSI) that specifies the subcomponent name

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT    pLogObject = (PXLOG_OBJECT) hLog;
    // dwThreadId is the id of the current thread
    DWORD           dwThreadId = GetCurrentThreadId();
    // pCurrentThreadInfo a pointer to the thread info for the current thread
    PTHREAD_INFO    pCurrentThreadInfo = NULL;
    // bThreadInfoAllocated indicates the thread info was allocated
    BOOL            bThreadInfoAllocated = FALSE;
    // lpszComma is a pointer to a comma in the component name or subcomponent name
    LPSTR           lpszComma = NULL;
    // dwErrorCode is the last error code
    DWORD           dwErrorCode = ERROR_SUCCESS;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Assert parameters are both NULL or both not NULL
    ASSERT(((NULL == lpszComponentName_A) && (NULL == lpszSubcomponentName_A)) || ((NULL != lpszComponentName_A) && (NULL != lpszSubcomponentName_A)));

    // Wait for access to the XLOG_OBJECT
    EnterCriticalSection(pLogObject->pcs);

    // Get the current thread info
    pCurrentThreadInfo = pLogObject->pThreadInfo;

    while (NULL != pCurrentThreadInfo) {
        if (dwThreadId == pCurrentThreadInfo->dwThreadId) {
            break;
        }

        // Set the current thread info to the next thread info in the list
        pCurrentThreadInfo = pCurrentThreadInfo->pNextThreadInfo;
    }

    if ((NULL == lpszComponentName_A) && (NULL == lpszSubcomponentName_A)) {
        if (NULL != pCurrentThreadInfo) {
            if (NULL != pCurrentThreadInfo->pFunctionInfo) {
                // Free the memory for the function info
                xMemFree(pLogObject->hMemObject, pCurrentThreadInfo->pFunctionInfo);
                pCurrentThreadInfo->pFunctionInfo = NULL;
            }

            if (NULL == pCurrentThreadInfo->pVariationList) {
                // Remove the thread info from the list
                if (NULL != pCurrentThreadInfo->pPrevThreadInfo) {
                    pCurrentThreadInfo->pPrevThreadInfo->pNextThreadInfo = pCurrentThreadInfo->pNextThreadInfo;
                }
                else {
                    pLogObject->pThreadInfo = pCurrentThreadInfo->pNextThreadInfo;
                }

                if (NULL != pCurrentThreadInfo->pNextThreadInfo) {
                    pCurrentThreadInfo->pNextThreadInfo->pPrevThreadInfo = pCurrentThreadInfo->pPrevThreadInfo;
                }

                // Free the memory for the thread info
                xMemFree(pLogObject->hMemObject, pCurrentThreadInfo);                
            }
        }

        goto FunctionExit;
    }

    if (NULL == pCurrentThreadInfo) {
        // Allocate the memory for the thread info
        pCurrentThreadInfo = (PTHREAD_INFO) xMemAlloc(pLogObject->hMemObject, sizeof(THREAD_INFO));
        if (NULL == pCurrentThreadInfo) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionExit;
        }

        bThreadInfoAllocated = TRUE;

        // Set the thread id
        pCurrentThreadInfo->dwThreadId = dwThreadId;

        // Add the thread info to the list
        pCurrentThreadInfo->pNextThreadInfo = pLogObject->pThreadInfo;
        if (NULL != pLogObject->pThreadInfo) {
            pLogObject->pThreadInfo->pPrevThreadInfo = pCurrentThreadInfo;
        }
        pLogObject->pThreadInfo = pCurrentThreadInfo;
    }

    if (NULL == pCurrentThreadInfo->pFunctionInfo) {
        // Allocate the memory for the function info
        pCurrentThreadInfo->pFunctionInfo = (PFUNCTION_INFO) xMemAlloc(pLogObject->hMemObject, sizeof(FUNCTION_INFO));
        if (NULL == pCurrentThreadInfo->pFunctionInfo) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionExit;
        }

        // Set the pointer to the component name
        pCurrentThreadInfo->pFunctionInfo->lpszComponentName_A = pCurrentThreadInfo->pFunctionInfo->szComponentName_A;
        // Set the pointer to the subcomponent name
        pCurrentThreadInfo->pFunctionInfo->lpszSubcomponentName_A = pCurrentThreadInfo->pFunctionInfo->szSubcomponentName_A;
    }

    // Reset the component name
    ZeroMemory(pCurrentThreadInfo->pFunctionInfo->szComponentName_A, COMPONENT_NAME_LENGTH + 1);

    // Copy the component name
    strncpy(pCurrentThreadInfo->pFunctionInfo->szComponentName_A, lpszComponentName_A, COMPONENT_NAME_LENGTH);

    // Parse the component name for commas and replace with spaces
    lpszComma = pCurrentThreadInfo->pFunctionInfo->szComponentName_A;
    while (NULL != (lpszComma = strchr(lpszComma, ','))) {
        *lpszComma = ' ';
    }

    // Reset the subcomponent name
    ZeroMemory(pCurrentThreadInfo->pFunctionInfo->szSubcomponentName_A, SUBCOMPONENT_NAME_LENGTH + 1);

    // Copy the subcomponent name
    strncpy(pCurrentThreadInfo->pFunctionInfo->szSubcomponentName_A, lpszSubcomponentName_A, SUBCOMPONENT_NAME_LENGTH);

    // Parse the subcomponent name for commas and replace with spaces
    lpszComma = pCurrentThreadInfo->pFunctionInfo->szSubcomponentName_A;
    while (NULL != (lpszComma = strchr(lpszComma, ','))) {
        *lpszComma = ' ';
    }

    // Reset the function name
    ZeroMemory(pCurrentThreadInfo->pFunctionInfo->szFunctionName_A, FUNCTION_NAME_LENGTH + 1);
    pCurrentThreadInfo->pFunctionInfo->lpszFunctionName_A = NULL;

FunctionExit:
    if (ERROR_SUCCESS != dwErrorCode) {
        if (TRUE == bThreadInfoAllocated) {
            // Remove the thread info from the list
            pLogObject->pThreadInfo = pLogObject->pThreadInfo->pNextThreadInfo;
            if (NULL != pLogObject->pThreadInfo) {
                pLogObject->pThreadInfo->pPrevThreadInfo = NULL;
            }

            // Free the thread info
            xMemFree(pLogObject->hMemObject, pCurrentThreadInfo);
        }
    }

    // Release access to the XLOG_OBJECT
    LeaveCriticalSection(pLogObject->pcs);

    if (ERROR_SUCCESS != dwErrorCode) {
        // Set the last error code
        SetLastError(dwErrorCode);
    }

    return (ERROR_SUCCESS == dwErrorCode);
}



BOOL
WINAPI
xSetFunctionName(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszFunctionName_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sets the function name used in logging for the thread

Arguments:

  hLog - Handle to the XLOG_OBJECT
  lpszFunctionName_A - Pointer to a null-terminated string (ANSI) that specifies the function name

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT    pLogObject = (PXLOG_OBJECT) hLog;
    // dwThreadId is the id of the current thread
    DWORD           dwThreadId = GetCurrentThreadId();
    // pCurrentThreadInfo a pointer to the thread info for the current thread
    PTHREAD_INFO    pCurrentThreadInfo = NULL;
    // lpszComma is a pointer to a comma in the component name or subcomponent name
    LPSTR           lpszComma = NULL;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Assert lpszFunctionName_A is not NULL
    ASSERT(NULL != lpszFunctionName_A);

    // Wait for access to the XLOG_OBJECT
    EnterCriticalSection(pLogObject->pcs);

    // Get the current thread info
    pCurrentThreadInfo = pLogObject->pThreadInfo;

    while (NULL != pCurrentThreadInfo) {
        if (dwThreadId == pCurrentThreadInfo->dwThreadId) {
            break;
        }

        // Set the current thread info to the next thread info in the list
        pCurrentThreadInfo = pCurrentThreadInfo->pNextThreadInfo;
    }

    // Assert pCurrentThreadInfo is not NULL
    ASSERT(NULL != pCurrentThreadInfo);
    // Assert pFunctionInfo is not NULL
    ASSERT(NULL != pCurrentThreadInfo->pFunctionInfo);
    // Assert component name is not NULL
    ASSERT(NULL != pCurrentThreadInfo->pFunctionInfo->lpszComponentName_A);
    // Assert subcomponent name is not NULL
    ASSERT(NULL != pCurrentThreadInfo->pFunctionInfo->lpszSubcomponentName_A);

    // Reset the function name
    ZeroMemory(pCurrentThreadInfo->pFunctionInfo->szFunctionName_A, FUNCTION_NAME_LENGTH + 1);

    // Copy the function name
    strncpy(pCurrentThreadInfo->pFunctionInfo->szFunctionName_A, lpszFunctionName_A, FUNCTION_NAME_LENGTH);

    // Parse the function name for commas and replace with spaces
    lpszComma = pCurrentThreadInfo->pFunctionInfo->szFunctionName_A;
    while (NULL != (lpszComma = strchr(lpszComma, ','))) {
        *lpszComma = ' ';
    }

    // Set the pointer to the function name
    pCurrentThreadInfo->pFunctionInfo->lpszFunctionName_A = pCurrentThreadInfo->pFunctionInfo->szFunctionName_A;

    // Release access to the XLOG_OBJECT
    LeaveCriticalSection(pLogObject->pcs);

    return TRUE;
}



BOOL
WINAPI
xStartVariationEx(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszVariationName_A,
    IN  int     nVariationTime
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Starts a variation logging sequence for the thread:
    Creates a new variation element
    Adds the new variation element to the head of the variation list
    Copies the variation name into the variation element
    Increments the dwVariationsTotal member in the XLOG_OBJECT
    Logs the variation name
    Caches current times

Arguments:

  hLog - Handle to the XLOG_OBJECT
  lpszVariationName_A - Pointer to a null-terminated string (ANSI) that specifies the variation name
  nVariationTime - Specifies if variation times are logged

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT        pLogObject = (PXLOG_OBJECT) hLog;
    // dwThreadId is the id of the current thread
    DWORD               dwThreadId = GetCurrentThreadId();
    // pCurrentThreadInfo a pointer to the thread info for the current thread
    PTHREAD_INFO        pCurrentThreadInfo = NULL;
    // bThreadInfoAllocated indicates the thread info was allocated
    BOOL                bThreadInfoAllocated = FALSE;
    // pVariationElement is a pointer to the new variation element
    PVARIATION_ELEMENT  pVariationElement = NULL;
    // lpszComma is a pointer to a comma in the variation name
    LPSTR               lpszComma = NULL;
    // bReturnCode is the return code of this function
    BOOL                bReturnCode = TRUE;
    // dwErrorCode is the last error code
    DWORD               dwErrorCode = ERROR_SUCCESS;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Assert lpszVariationName_A is not NULL
    ASSERT(NULL != lpszVariationName_A);

    // Wait for access to the XLOG_OBJECT
    EnterCriticalSection(pLogObject->pcs);

    // Get the current thread info
    pCurrentThreadInfo = pLogObject->pThreadInfo;

    while (NULL != pCurrentThreadInfo) {
        if (dwThreadId == pCurrentThreadInfo->dwThreadId) {
            break;
        }

        // Set the current thread info to the next thread info in the list
        pCurrentThreadInfo = pCurrentThreadInfo->pNextThreadInfo;
    }

    if (NULL == pCurrentThreadInfo) {
        // Allocate the memory for the thread info
        pCurrentThreadInfo = (PTHREAD_INFO) xMemAlloc(pLogObject->hMemObject, sizeof(THREAD_INFO));
        if (NULL == pCurrentThreadInfo) {
            // Get the last error code
            dwErrorCode = GetLastError();

            goto FunctionFailed;
        }

        bThreadInfoAllocated = TRUE;

        // Set the thread id
        pCurrentThreadInfo->dwThreadId = dwThreadId;

        // Add the thread info to the list
        pCurrentThreadInfo->pNextThreadInfo = pLogObject->pThreadInfo;
        if (NULL != pLogObject->pThreadInfo) {
            pLogObject->pThreadInfo->pPrevThreadInfo = pCurrentThreadInfo;
        }
        pLogObject->pThreadInfo = pCurrentThreadInfo;
    }

    // Allocate the memory for a variation element
    pVariationElement = (PVARIATION_ELEMENT) xMemAlloc(pLogObject->hMemObject, sizeof(VARIATION_ELEMENT));
    if (NULL == pVariationElement) {
        // Get the last error code
        dwErrorCode = GetLastError();

        goto FunctionFailed;
    }

    // Add the variation element to the list
    pVariationElement->pNextVariationElement = pCurrentThreadInfo->pVariationList;
    if (NULL != pCurrentThreadInfo->pVariationList) {
        pCurrentThreadInfo->pVariationList->pPrevVariationElement = pVariationElement;
    }
    pCurrentThreadInfo->pVariationList = pVariationElement;

    // Copy the variation name
    strncpy(pVariationElement->szVariationName_A, lpszVariationName_A, VARIATION_NAME_LENGTH);

    // Parse the variation name for commas and replace with spaces
    lpszComma = pVariationElement->szVariationName_A;
    while (NULL != (lpszComma = strchr(lpszComma, ','))) {
        *lpszComma = ' ';
    }

    // Set the pointer to the variation name
    pVariationElement->lpszVariationName_A = pVariationElement->szVariationName_A;

    // Increment the dwVariationsTotal member
    pLogObject->dwVariationsTotal++;

    // Release access to the XLOG_OBJECT
    LeaveCriticalSection(pLogObject->pcs);

    // Log the variation name
    bReturnCode = xLog(hLog, XLL_VARIATION_START, "Variation Started: %s", lpszVariationName_A);

#ifdef _XBOX
    pVariationElement->bVariationTime = FALSE;
    if (0 == nVariationTime) {
        pVariationElement->bVariationTime = (0 != (XLL_VARIATION_TIME & pLogObject->dwLogLevel));
    }
    else if (0 > nVariationTime) {
        pVariationElement->bVariationTime = FALSE;
    }
    else if (0 < nVariationTime) {
        pVariationElement->bVariationTime = TRUE;
    }

    if (TRUE == pVariationElement->bVariationTime) {
        // Get the current times
        GetSystemTimeAsFileTime((FILETIME *) &pVariationElement->InitialTime);
        QueryCpuTimesInformation((LARGE_INTEGER *) &pVariationElement->InitialKernelTime, (LARGE_INTEGER *) &pVariationElement->InitialIdleTime, (LARGE_INTEGER *) &pVariationElement->InitialDpcTime, (LARGE_INTEGER *) &pVariationElement->InitialInterruptTime, NULL);
    }
#else
    pVariationElement->bVariationTime = FALSE;
#endif

    return bReturnCode;

FunctionFailed:
    if (NULL != pVariationElement) {
        // Remove the variation element from the list
        pCurrentThreadInfo->pVariationList = pVariationElement->pNextVariationElement;
        if (NULL != pCurrentThreadInfo->pVariationList) {
            pCurrentThreadInfo->pVariationList->pPrevVariationElement = NULL;
        }

        // Free the variation element
        xMemFree(pLogObject->hMemObject, pVariationElement);
    }

    if (TRUE == bThreadInfoAllocated) {
        // Remove the thread info from the list
        pLogObject->pThreadInfo = pLogObject->pThreadInfo->pNextThreadInfo;
        if (NULL != pLogObject->pThreadInfo) {
            pLogObject->pThreadInfo->pPrevThreadInfo = NULL;
        }

        // Free the thread info
        xMemFree(pLogObject->hMemObject, pCurrentThreadInfo);
    }

    // Release access to the XLOG_OBJECT
    LeaveCriticalSection(pLogObject->pcs);

    // Set the last error code
    SetLastError(dwErrorCode);

    return FALSE;
}



BOOL
WINAPI
xStartVariation(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszVariationName_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Passthrough to xStartVariationEx

Arguments:

  hLog - Handle to the XLOG_OBJECT
  lpszVariationName_A - Pointer to a null-terminated string (ANSI) that specifies the variation name

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Call xStartVariationEx
    return xStartVariationEx(hLog, lpszVariationName_A, 0);
}



DWORD
WINAPI
xEndVariation(
    IN  HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Ends a variation logging sequence for the thread:
    Gets the variation list from TLS
    Gets the variation result from the head of the variation list
    Increments the appropriate variation result member in the XLOG_OBJECT
    Logs and web posts the variation name and result
    Sets the new head of the variation list in TLS
    Frees the old head of the variation list

Arguments:

  hLog - Handle to the XLOG_OBJECT

Return Value:

  DWORD:
    The dwVariationResult value from TLS.

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT        pLogObject = (PXLOG_OBJECT) hLog;
    // dwThreadId is the id of the current thread
    DWORD               dwThreadId = GetCurrentThreadId();
    // pCurrentThreadInfo a pointer to the thread info for the current thread
    PTHREAD_INFO        pCurrentThreadInfo = NULL;
    // pVariationElement is a pointer to the variation element
    PVARIATION_ELEMENT  pVariationElement = NULL;
    // dwVariationResult is the variation result
    DWORD               dwVariationResult = 0;

    // lpszComponentName_A is a pointer to the component name
    LPSTR               lpszComponentName_A = NO_COMPONENT_STRING;
    // lpszSubcomponentName_A is a pointer to the subcomponent name
    LPSTR               lpszSubcomponentName_A = NO_SUBCOMPONENT_STRING;
    // lpszFunctionName_A is a pointer to the function name
    LPSTR               lpszFunctionName_A = NO_FUNCTION_STRING;
    // lpszVariationName_A is a pointer to the variation name
    LPSTR               lpszVariationName_A = NO_VARIATION_STRING;

#ifdef _XBOX
    // CurrentTime is the current time
    ULONGLONG           CurrentTime;
    // CurrentSeconds is the current time (seconds)
    ULONGLONG           CurrentSeconds;
    // CurrentMilliSeconds is the current time (milli-seconds)
    ULONGLONG           CurrentMilliSeconds;
    // CurrentMicroSeconds is the current time (micro-seconds)
    ULONGLONG           CurrentMicroSeconds;

    // CurrentKernelTime is the current kernel time
    ULONGLONG           CurrentKernelTime;
    // CurrentKernelSeconds is the current kernel time (seconds)
    ULONGLONG           CurrentKernelSeconds;
    // CurrentKernelMilliSeconds is the current kernel time (milli-seconds)
    ULONGLONG           CurrentKernelMilliSeconds;
    // CurrentKernelMicroSeconds is the current kernel time (micro-seconds)
    ULONGLONG           CurrentKernelMicroSeconds;

    // CurrentIdleTime is the current idle time
    ULONGLONG           CurrentIdleTime;
    // CurrentIdleSeconds is the current idle time (seconds)
    ULONGLONG           CurrentIdleSeconds;
    // CurrentIdleMilliSeconds is the current idle time (milli-seconds)
    ULONGLONG           CurrentIdleMilliSeconds;
    // CurrentIdleMicroSeconds is the current idle time (micro-seconds)
    ULONGLONG           CurrentIdleMicroSeconds;

    // CurrentDpcTime is the current dpc time
    ULONGLONG           CurrentDpcTime;
    // CurrentDpcSeconds is the current dpc time (seconds)
    ULONGLONG           CurrentDpcSeconds;
    // CurrentDpcMilliSeconds is the current dpc time (milli-seconds)
    ULONGLONG           CurrentDpcMilliSeconds;
    // CurrentDpcMicroSeconds is the current dpc time (micro-seconds)
    ULONGLONG           CurrentDpcMicroSeconds;

    // CurrentInterruptTime is the current interrupt time
    ULONGLONG           CurrentInterruptTime;
    // CurrentInterruptSeconds is the current interrupt time (seconds)
    ULONGLONG           CurrentInterruptSeconds;
    // CurrentInterruptMilliSeconds is the current interrupt time (milli-seconds)
    ULONGLONG           CurrentInterruptMilliSeconds;
    // CurrentInterruptMicroSeconds is the current interrupt time (micro-seconds)
    ULONGLONG           CurrentInterruptMicroSeconds;



    // Get the current times
    QueryCpuTimesInformation((LARGE_INTEGER *) &CurrentKernelTime, (LARGE_INTEGER *) &CurrentIdleTime, (LARGE_INTEGER *) &CurrentDpcTime, (LARGE_INTEGER *) &CurrentInterruptTime, NULL);
    GetSystemTimeAsFileTime((FILETIME *) &CurrentTime);
#endif

    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Wait for access to the XLOG_OBJECT
    EnterCriticalSection(pLogObject->pcs);

    // Get the current thread info
    pCurrentThreadInfo = pLogObject->pThreadInfo;

    while (NULL != pCurrentThreadInfo) {
        if (dwThreadId == pCurrentThreadInfo->dwThreadId) {
            break;
        }

        // Set the current thread info to the next thread info in the list
        pCurrentThreadInfo = pCurrentThreadInfo->pNextThreadInfo;
    }

    // Assert pCurrentThreadInfo is not NULL
    ASSERT(NULL != pCurrentThreadInfo);
    // Assert pVariationList is not NULL
    ASSERT(NULL != pCurrentThreadInfo->pVariationList);
    // Assert variation name is not NULL
    ASSERT(NULL != pCurrentThreadInfo->pVariationList->lpszVariationName_A);
    // Assert variation result is not 0
    ASSERT(0 != pCurrentThreadInfo->pVariationList->dwVariationResult);

    // Get the variation element
    pVariationElement = pCurrentThreadInfo->pVariationList;

#ifdef _XBOX
    if (TRUE == pVariationElement->bVariationTime) {
        // Calculate and log the times
        CurrentTime = CurrentTime - pVariationElement->InitialTime;
        CurrentMicroSeconds = CurrentTime / 10;
        CurrentMilliSeconds = CurrentTime / 10000;
        CurrentSeconds = CurrentTime / 10000000;
        CurrentMilliSeconds -= (CurrentSeconds * 1000);
        CurrentMicroSeconds -= ((CurrentSeconds * 1000 * 1000) + (CurrentMilliSeconds * 1000));
        xLog(hLog, XLL_VARIATION_TIME, "Variation Time - %I64u.%03I64u.%03I64u Seconds", CurrentSeconds, CurrentMilliSeconds, CurrentMicroSeconds);

        CurrentKernelTime = CurrentKernelTime - pVariationElement->InitialKernelTime;
        CurrentKernelMicroSeconds = CurrentKernelTime / 10;
        CurrentKernelMilliSeconds = CurrentKernelTime / 10000;
        CurrentKernelSeconds = CurrentKernelTime / 10000000;
        CurrentKernelMilliSeconds -= (CurrentKernelSeconds * 1000);
        CurrentKernelMicroSeconds -= ((CurrentKernelSeconds * 1000 * 1000) + (CurrentKernelMilliSeconds * 1000));
        xLog(hLog, XLL_VARIATION_TIME, "Kernel Time - %I64u.%03I64u.%03I64u Seconds", CurrentKernelSeconds, CurrentKernelMilliSeconds, CurrentKernelMicroSeconds);

        CurrentIdleTime = CurrentIdleTime - pVariationElement->InitialIdleTime;
        CurrentIdleMicroSeconds = CurrentIdleTime / 10;
        CurrentIdleMilliSeconds = CurrentIdleTime / 10000;
        CurrentIdleSeconds = CurrentIdleTime / 10000000;
        CurrentIdleMilliSeconds -= (CurrentIdleSeconds * 1000);
        CurrentIdleMicroSeconds -= ((CurrentIdleSeconds * 1000 * 1000) + (CurrentIdleMilliSeconds * 1000));
        xLog(hLog, XLL_VARIATION_TIME, "Idle Time - %I64u.%03I64u.%03I64u Seconds", CurrentIdleSeconds, CurrentIdleMilliSeconds, CurrentIdleMicroSeconds);

        CurrentDpcTime = CurrentDpcTime - pVariationElement->InitialDpcTime;
        CurrentDpcMicroSeconds = CurrentDpcTime / 10;
        CurrentDpcMilliSeconds = CurrentDpcTime / 10000;
        CurrentDpcSeconds = CurrentDpcTime / 10000000;
        CurrentDpcMilliSeconds -= (CurrentDpcSeconds * 1000);
        CurrentDpcMicroSeconds -= ((CurrentDpcSeconds * 1000 * 1000) + (CurrentDpcMilliSeconds * 1000));
        xLog(hLog, XLL_VARIATION_TIME, "Dpc Time - %I64u.%03I64u.%03I64u Seconds", CurrentDpcSeconds, CurrentDpcMilliSeconds, CurrentDpcMicroSeconds);

        CurrentInterruptTime = CurrentInterruptTime - pVariationElement->InitialInterruptTime;
        CurrentInterruptMicroSeconds = CurrentInterruptTime / 10;
        CurrentInterruptMilliSeconds = CurrentInterruptTime / 10000;
        CurrentInterruptSeconds = CurrentInterruptTime / 10000000;
        CurrentInterruptMilliSeconds -= (CurrentInterruptSeconds * 1000);
        CurrentInterruptMicroSeconds -= ((CurrentInterruptSeconds * 1000 * 1000) + (CurrentInterruptMilliSeconds * 1000));
        xLog(hLog, XLL_VARIATION_TIME, "Interrupt Time - %I64u.%03I64u.%03I64u Seconds", CurrentInterruptSeconds, CurrentInterruptMilliSeconds, CurrentInterruptMicroSeconds);
    }
#endif

    // Get the variation result
    dwVariationResult = pVariationElement->dwVariationResult;

    // Increment the variation result
    IncrementVariationResult(hLog, dwVariationResult, FALSE);

    // Release access to the XLOG_OBJECT
    LeaveCriticalSection(pLogObject->pcs);

    // Log the variation name
    xLog(hLog, XLL_VARIATION_END, "Variation Ended: %s", pVariationElement->lpszVariationName_A);

    // Wait for access to the XLOG_OBJECT
    EnterCriticalSection(pLogObject->pcs);

    // Get the current thread info
    pCurrentThreadInfo = pLogObject->pThreadInfo;

    while (NULL != pCurrentThreadInfo) {
        if (dwThreadId == pCurrentThreadInfo->dwThreadId) {
            break;
        }

        // Set the current thread info to the next thread info in the list
        pCurrentThreadInfo = pCurrentThreadInfo->pNextThreadInfo;
    }

    if (NULL != pCurrentThreadInfo->pFunctionInfo) {
        // Set the component name
        lpszComponentName_A = pCurrentThreadInfo->pFunctionInfo->lpszComponentName_A;
        // Set the subcomponent name
        lpszSubcomponentName_A = pCurrentThreadInfo->pFunctionInfo->lpszSubcomponentName_A;

        if (NULL != pCurrentThreadInfo->pFunctionInfo->lpszFunctionName_A) {
            // Set the function name
            lpszFunctionName_A = pCurrentThreadInfo->pFunctionInfo->lpszFunctionName_A;
        }
    }

    // Set the variation name
    lpszVariationName_A = pVariationElement->lpszVariationName_A;

    // Post the result
    PostResult(hLog, dwVariationResult, lpszComponentName_A, lpszSubcomponentName_A, lpszFunctionName_A, lpszVariationName_A, pVariationElement->lpszLogString_A);

    // Remove the variation element from the list
    pCurrentThreadInfo->pVariationList = pVariationElement->pNextVariationElement;
    if (NULL != pCurrentThreadInfo->pVariationList) {
        pCurrentThreadInfo->pVariationList->pPrevVariationElement = NULL;
    }

    if ((NULL == pCurrentThreadInfo->pFunctionInfo) && (NULL == pCurrentThreadInfo->pVariationList)) {
        // Remove the thread info from the list
        if (NULL != pCurrentThreadInfo->pPrevThreadInfo) {
            pCurrentThreadInfo->pPrevThreadInfo->pNextThreadInfo = pCurrentThreadInfo->pNextThreadInfo;
        }
        else {
            pLogObject->pThreadInfo = pCurrentThreadInfo->pNextThreadInfo;
        }

        if (NULL != pCurrentThreadInfo->pNextThreadInfo) {
            pCurrentThreadInfo->pNextThreadInfo->pPrevThreadInfo = pCurrentThreadInfo->pPrevThreadInfo;
        }

        // Free the memory for the thread info
        xMemFree(pLogObject->hMemObject, pCurrentThreadInfo);                
    }

    // Release access to the XLOG_OBJECT
    LeaveCriticalSection(pLogObject->pcs);

    // Free the variation element
    xMemFree(pLogObject->hMemObject, pVariationElement);

    return dwVariationResult;
}



namespace xLogNamespace {

BOOL
LogString(
    IN  HANDLE  hLog,
    IN  DWORD   dwLogLevel,
    IN  LPSTR   lpszLogString_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Writes the line of output to the log file.

Arguments:

  hLog - Handle to the XLOG_OBJECT
  dwLogLevel - Specifies the logging level for this line of output.  It can be one of the XLL_ #defines.
  lpszLogString_A - Pointer to a null-terminated string (ANSI) that specifies the log string

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT        pLogObject = (PXLOG_OBJECT) hLog;
    // dwThreadId is the id of the current thread
    DWORD               dwThreadId = GetCurrentThreadId();
    // pCurrentThreadInfo a pointer to the thread info for the current thread
    PTHREAD_INFO        pCurrentThreadInfo = NULL;
    // pVariationElement is a pointer to the variation element
    PVARIATION_ELEMENT  pVariationElement = NULL;

    // lpszComponentName_A is a pointer to the component name
    LPSTR               lpszComponentName_A = NO_COMPONENT_STRING;
    // lpszSubcomponentName_A is a pointer to the subcomponent name
    LPSTR               lpszSubcomponentName_A = NO_SUBCOMPONENT_STRING;
    // lpszFunctionName_A is a pointer to the function name
    LPSTR               lpszFunctionName_A = NO_FUNCTION_STRING;
    // lpszVariationName_A is a pointer to the variation name
    LPSTR               lpszVariationName_A = NO_VARIATION_STRING;

    // LocalTime is the current local time in hours, minutes, seconds, etc.
    SYSTEMTIME          LocalTime = {0, 0, 0, 0, 0, 0, 0, 0};
    // lpszLogLevel is a pointer to the string representation of the log level
    LPSTR               lpszLogLevel_A = NULL;
    // szOutputString is the line of log output
    CHAR                szOutputString_A[LOG_OUTPUT_LENGTH] = {'\0'};

    // dwErrorCode is the last error code
    DWORD               dwErrorCode = ERROR_SUCCESS;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Wait for access to the XLOG_OBJECT
    EnterCriticalSection(pLogObject->pcs);

    // Get the current thread info
    pCurrentThreadInfo = pLogObject->pThreadInfo;

    while (NULL != pCurrentThreadInfo) {
        if (dwThreadId == pCurrentThreadInfo->dwThreadId) {
            break;
        }

        // Set the current thread info to the next thread info in the list
        pCurrentThreadInfo = pCurrentThreadInfo->pNextThreadInfo;
    }

    // Get the variation element
    pVariationElement = (NULL == pCurrentThreadInfo) ? NULL : pCurrentThreadInfo->pVariationList;

    if (NULL != pVariationElement) {
        // Check if the variation result should be updated
        if ((0 == pVariationElement->dwVariationResult) || (dwLogLevel <= pVariationElement->dwVariationResult)) {
            // Update the variation result
            pVariationElement->dwVariationResult = dwLogLevel;

            // Reset the log string
            ZeroMemory(pVariationElement->szLogString_A, LOG_STRING_LENGTH + 1);

            // Copy the log string
            strncpy(pVariationElement->szLogString_A, lpszLogString_A, LOG_STRING_LENGTH);

            // Set the pointer to the log string
            pVariationElement->lpszLogString_A = pVariationElement->szLogString_A;
        }
    }
    else {
        // Increment the variation result
        IncrementVariationResult(hLog, dwLogLevel, TRUE);
    }

    if ((NULL != pCurrentThreadInfo) && (NULL != pCurrentThreadInfo->pFunctionInfo)) {
        // Set the component name
        lpszComponentName_A = pCurrentThreadInfo->pFunctionInfo->lpszComponentName_A;
        // Set the subcomponent name
        lpszSubcomponentName_A = pCurrentThreadInfo->pFunctionInfo->lpszSubcomponentName_A;

        if (NULL != pCurrentThreadInfo->pFunctionInfo->lpszFunctionName_A) {
            // Set the function name
            lpszFunctionName_A = pCurrentThreadInfo->pFunctionInfo->lpszFunctionName_A;
        }
    }

    if (NULL != pVariationElement) {
        // Set the variation name
        lpszVariationName_A = pVariationElement->lpszVariationName_A;
    }

    // Post the result, if necessary
    if (NULL == pVariationElement) {
        // Post the result
        PostResult(hLog, dwLogLevel, lpszComponentName_A, lpszSubcomponentName_A, lpszFunctionName_A, lpszVariationName_A, lpszLogString_A);
    }

    // Check if the line of output should be logged and/or echoed to the debugger
    if ((0 == (dwLogLevel & pLogObject->dwLogLevel)) && (XLL_VARIATION_TIME != dwLogLevel)) {
        goto FunctionExit;
    }

    // Get the current time
    GetLocalTime(&LocalTime);

    // Get the string representation of the log level
    switch (dwLogLevel) {

    case XLL_EXCEPTION:
        lpszLogLevel_A = "EXCEPTION";
        break;

    case XLL_BREAK:
        lpszLogLevel_A = "BREAK";
        break;

    case XLL_FAIL:
        lpszLogLevel_A = "FAIL";
        break;

    case XLL_WARN:
        lpszLogLevel_A = "WARN";
        break;

    case XLL_BLOCK:
        lpszLogLevel_A = "BLOCK";
        break;

    case XLL_PASS:
        lpszLogLevel_A = "PASS";
        break;

    case XLL_INFO:
        lpszLogLevel_A = "INFO";
        break;

    case XLL_VARIATION_START:
        lpszLogLevel_A = "VARIATION START";
        break;

    case XLL_VARIATION_END:
        // Get the variation result
        switch (pVariationElement->dwVariationResult) {

        case XLL_EXCEPTION:
            lpszLogLevel_A = "VARIATION END: EXCEPTION";
            break;

        case XLL_BREAK:
            lpszLogLevel_A = "VARIATION END: BREAK";
            break;

        case XLL_FAIL:
            lpszLogLevel_A = "VARIATION END: FAIL";
            break;

        case XLL_WARN:
            lpszLogLevel_A = "VARIATION END: WARN";
            break;

        case XLL_BLOCK:
            lpszLogLevel_A = "VARIATION END: BLOCK";
            break;

        case XLL_PASS:
            lpszLogLevel_A = "VARIATION END: PASS";
            break;

        default:
            lpszLogLevel_A = "VARIATION END: UNKNOWN";
            break;
        }

        break;

    case XLL_VARIATION_TIME:
        lpszLogLevel_A = "VARIATION TIME";
        break;

    default:
        lpszLogLevel_A = "UNKNOWN";
        break;
    }

    // Format the line of output
    sprintf(szOutputString_A, "%04x,%02d:%02d:%02d,%s,%s,%s,%s,%s,%s\r\n", dwThreadId, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond, lpszLogLevel_A, lpszComponentName_A, lpszSubcomponentName_A, lpszFunctionName_A, lpszVariationName_A, lpszLogString_A);

    // Log the line of output
    if (FALSE == FormatAndWriteString(hLog, szOutputString_A)) {
        // Get the last error code
        dwErrorCode = GetLastError();
    }

    if (INVALID_HANDLE_VALUE != pLogObject->hLogFile) {
        // Flush the file buffers to prevent corruption
        FlushFileBuffers(pLogObject->hLogFile);
    }

    if (XLL_BREAK == dwLogLevel) {
        // Echo the line to the debuffer if not already done so
        if (0 == (XLO_DEBUG & pLogObject->dwLogOptions)) {
            OutputDebugStringA(szOutputString_A);
        }

        // Break into the debugger, if specified
        DebugBreak();
    }

FunctionExit:
    // Release access to the XLOG_OBJECT
    LeaveCriticalSection(pLogObject->pcs);

    return (ERROR_SUCCESS == dwErrorCode);
}

} // namespace xLogNamespace



BOOL
WINAPI
xLog_va(
    IN  HANDLE   hLog,
    IN  DWORD    dwLogLevel,
    IN  LPSTR    lpszFormatString_A,
    IN  va_list  varg_ptr
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Passthrough to LogString:
    Gets the function info from TLS
    Gets the variation list from TLS
    Sets the component name, subcomponent name, function name, and variation name to default values
    Formats the log string
    If variation list is not NULL:
      Updates the variation result and log string if dwLogLevel is more severe
    Checks if dwLogLevel is severe enough to log
    If function info is not NULL:
      Gets the component name, subcomponent name, and function name
    If variation list is not NULL:
      Gets the variation name
    Calls LogString

Arguments:

  hLog - Handle to the XLOG_OBJECT
  dwLogLevel - Specifies the logging level for this line of output.  It can be one of the XLL_ #defines.
  lpszFormatString_A - Pointer to a null-terminated string (ANSI) that consists of format-control specifications
  varg_ptr - Pointer to the variable argument list

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // szLogString is the string in the line of log output
    CHAR   szLogString_A[LOG_STRING_LENGTH + 1] = {'\0'};
    // lpszComma is a pointer to a comma in the log string
    LPSTR  lpszComma = NULL;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Format the log string
    _vsnprintf(szLogString_A, LOG_STRING_LENGTH, lpszFormatString_A, varg_ptr);

    // Parse the log string for commas and replace with spaces
    lpszComma = szLogString_A;
    while (NULL != (lpszComma = strchr(lpszComma, ','))) {
        *lpszComma = ' ';
    }

    // Call LogString
    return LogString(hLog, dwLogLevel, szLogString_A);
}



BOOL
WINAPI
xLog(
    IN  HANDLE  hLog,
    IN  DWORD   dwLogLevel,
    IN  LPSTR   lpszFormatString_A,
    IN  ...
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Passthrough to xLog_va:
    Gets the variable argument list
    Call xLog_va

Arguments:

  hLog - Handle to the XLOG_OBJECT
  dwLogLevel - Specifies the logging level for this line of output.  It can be one of the XLL_ #defines.
  lpszFormatString_A - Pointer to a null-terminated string (ANSI) that consists of format-control specifications

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // varg_ptr is a pointer to the variable argument list
    va_list  varg_ptr = NULL;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Get the variable argument list
    va_start(varg_ptr, lpszFormatString_A);

    // Call xLog_va
    return xLog_va(hLog, dwLogLevel, lpszFormatString_A, varg_ptr);
}



namespace xLogNamespace {

BOOL
xPostConfiguration(
    IN  HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Web posts the system configuration

Arguments:

  hLog - Handle to the XLOG_OBJECT

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT  pLogObject = (PXLOG_OBJECT) hLog;

    // szWebPostConfigData_A is the line of web post data
    CHAR          szWebPostConfigData_A[WEBPOST_CONFIGDATA_LENGTH] = {'\0'};
    // szWebPostConfig_A is the web post
    CHAR          szWebPostConfig_A[WEBPOST_CONFIG_LENGTH] = {'\0'};
    // szWebPostLen_A is the web post length
    CHAR          szWebPostLen_A[6] = {'\0'};
    // szHttpAddr_A is the string representation of the http server host address
    CHAR          szHttpAddr_A[13] = {'\0'};
    // dwBytesWritten is the number of bytes written to the file
    DWORD         dwBytesWritten = 0;

    // szDebugString is the debug string
    CHAR          szDebugString[61];

    // bReturnCode is the return code of this function
    BOOL          bReturnCode = TRUE;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    if (OS_PLATFORM_XBOX == pLogObject->dwPlatformId) {
        // Format the web post data
        sprintf(szWebPostConfigData_A, WEBPOST_CONFIGDATA_FORMAT_XBOX_A, pLogObject->szHostName_A, pLogObject->szObjectUUID_A, 0, 0, pLogObject->XapiVersion, (TRUE == pLogObject->XapiDebug) ? 1 : 0, 0, 0, pLogObject->KernelVersion, (TRUE == pLogObject->KernelDebug) ? 1 : 0);
    }
    else if (OS_PLATFORM_NT == pLogObject->dwPlatformId) {
        // Format the web post data
        sprintf(szWebPostConfigData_A, WEBPOST_CONFIGDATA_FORMAT_NT_A, pLogObject->szHostName_A, pLogObject->szObjectUUID_A, pLogObject->dwMajorVersion, pLogObject->dwMinorVersion, pLogObject->dwBuildNumber, 0);
    }
    else {
        // Format the web post data
        sprintf(szWebPostConfigData_A, WEBPOST_CONFIGDATA_FORMAT_UNKNOWN_A, pLogObject->szHostName_A, pLogObject->szObjectUUID_A);
    }

    // Format the web post
    sprintf(szWebPostConfig_A, WEBPOST_FORMAT_A, pLogObject->lpszConfigPage_A, pLogObject->lpszHttpServer_A, strlen(szWebPostConfigData_A), szWebPostConfigData_A);

    if (INVALID_HANDLE_VALUE != pLogObject->hWebPostFile) {
        // Format the web post length
        sprintf(szWebPostLen_A, "%03u\r\n", strlen(szWebPostConfig_A));

        // Format the http server host address
        sprintf(szHttpAddr_A, "0x%08x\r\n", pLogObject->HttpAddr);

        // Write the web post to the file
        bReturnCode &= WriteFile(pLogObject->hWebPostFile, szWebPostLen_A, strlen(szWebPostLen_A), &dwBytesWritten, NULL);
        bReturnCode &= WriteFile(pLogObject->hWebPostFile, szHttpAddr_A, strlen(szHttpAddr_A), &dwBytesWritten, NULL);
        bReturnCode &= WriteFile(pLogObject->hWebPostFile, szWebPostConfig_A, strlen(szWebPostConfig_A), &dwBytesWritten, NULL);
        bReturnCode &= WriteFile(pLogObject->hWebPostFile, "\r\n", strlen("\r\n"), &dwBytesWritten, NULL);

        // Flush the file buffers to prevent corruption
        FlushFileBuffers(pLogObject->hWebPostFile);
    }

    return bReturnCode;
}



BOOL
WINAPI
xLogConfiguration(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszLogString_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Logs the system configuration

Arguments:

  hLog - Handle to the XLOG_OBJECT
  lpszLogString_A - Pointer to a null-terminated string (ANSI) that specifies the log string

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT    pLogObject = (PXLOG_OBJECT) hLog;

    // szDrives is the list of valid drives for the system
    CHAR            szDrives[] = "A:\\\0B:\\\0C:\\\0D:\\\0E:\\\0F:\\\0G:\\\0H:\\\0I:\\\0J:\\\0K:\\\0L:\\\0M:\\\0N:\\\0O:\\\0P:\\\0Q:\\\0R:\\\0S:\\\0T:\\\0U:\\\0V:\\\0W:\\\0X:\\\0Y:\\\0Z:\\";

    // dwSerialNumber is the serial number of the drive
    DWORD           dwSerialNumber;
    // AvailableBytes is the available bytes on the drive
    ULARGE_INTEGER  AvailableBytes;
    // TotalBytes is the total bytes on the drive
    ULARGE_INTEGER  TotalBytes;
    // dwNumDrives is the number of valid drives for the system
    DWORD           dwNumDrives = 0;
    // dwCurrentDrive is a counter to enumerate each drive
    DWORD           dwCurrentDrive = 0;

    // bReturnCode is the return code of this function
    BOOL            bReturnCode = TRUE;
    // dwErrorCode is the last error code
    DWORD           dwErrorCode = ERROR_SUCCESS;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Get the valid drives
    dwNumDrives = (sizeof(szDrives) / sizeof(szDrives[0])) / 4;
    if (0 == dwNumDrives) {
        // Get the last error code
        dwErrorCode = GetLastError();

        bReturnCode = FALSE;

        goto FunctionExit0;
    }

    // Log the info banner
    bReturnCode &= LogBanner(hLog, "Machine & Version Information", lpszLogString_A);
    bReturnCode &= FormatAndWriteString(hLog, "Host Name,%s\r\n", pLogObject->szHostName_A);

    // Log the version info
    if (OS_PLATFORM_XBOX == pLogObject->dwPlatformId) {
        bReturnCode &= FormatAndWriteString(hLog, "OS Platform,Xbox\r\n");
        bReturnCode &= FormatAndWriteString(hLog, "Kernel Version,%u %s\r\n", pLogObject->KernelVersion, (TRUE == pLogObject->KernelDebug) ? "DEBUG" : "FREE");
        bReturnCode &= FormatAndWriteString(hLog, "Xapi Version,%u %s\r\n", pLogObject->XapiVersion, (TRUE == pLogObject->XapiDebug) ? "DEBUG" : "FREE");
    }
    else if (OS_PLATFORM_NT == pLogObject->dwPlatformId) {
        bReturnCode &= FormatAndWriteString(hLog, "OS Platform,Nt\r\n");
        bReturnCode &= FormatAndWriteString(hLog, "Major Version,%u\r\n", pLogObject->dwMajorVersion);
        bReturnCode &= FormatAndWriteString(hLog, "Minor Version,%u\r\n", pLogObject->dwMinorVersion);
        bReturnCode &= FormatAndWriteString(hLog, "Build Number,%u\r\n", pLogObject->dwBuildNumber);
    }
    else {
        bReturnCode &= FormatAndWriteString(hLog, "OS Platform,Unknown\r\n");
    }
    bReturnCode &= FormatAndWriteString(hLog, "**********\r\n");

    // Log the drive info banner
    bReturnCode &= LogBanner(hLog, "Drive Information", lpszLogString_A);
#ifdef _XBOX
    bReturnCode &= FormatAndWriteString(hLog, "Disk Model Number,%s\r\n", g_LogInfo.lpszDiskModel);
    bReturnCode &= FormatAndWriteString(hLog, "Disk Serial Number,%s\r\n\r\n", g_LogInfo.lpszDiskSerial);
#endif
    bReturnCode &= FormatAndWriteString(hLog, "Drive Name,Serial Number,Total Bytes,Free Bytes\r\n");

#ifndef _XBOX
    // Set the system error mode
    SetErrorMode(SEM_FAILCRITICALERRORS);
#endif

    // Log the drive info
    for (dwCurrentDrive = 0; dwCurrentDrive < dwNumDrives; dwCurrentDrive++) {
        // Get the serial number
        dwSerialNumber = 0;
        if (GetVolumeInformationA(&szDrives[dwCurrentDrive * 4], NULL, 0, &dwSerialNumber, NULL, NULL, NULL, 0)) {
            // Get the drive space
            AvailableBytes.QuadPart = 0;
            TotalBytes.QuadPart = 0;
            GetDiskFreeSpaceExA(&szDrives[dwCurrentDrive * 4], &AvailableBytes, &TotalBytes, NULL);

            bReturnCode &= FormatAndWriteString(hLog, "%s,%04X-%04X,%I64u MB,%I64u MB\r\n", &szDrives[dwCurrentDrive * 4], HIWORD(dwSerialNumber), LOWORD(dwSerialNumber), TotalBytes.QuadPart / (1024 * 1024), AvailableBytes.QuadPart / (1024 * 1024));
        }
    }
    bReturnCode &= FormatAndWriteString(hLog, "**********\r\n");

#ifndef _XBOX
    // Reset the system error mode
    SetErrorMode(0);
#endif

FunctionExit0:
    if (ERROR_SUCCESS != dwErrorCode) {
        // Set the last error code
        SetLastError(dwErrorCode);
    }

    return bReturnCode;
}



BOOL
WINAPI
xLogResourceStatus(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszLogString_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Logs the system resource status

Arguments:

  hLog - Handle to the XLOG_OBJECT
  lpszLogString_A - Pointer to a null-terminated string (ANSI) that specifies the log string

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT   pLogObject = (PXLOG_OBJECT) hLog;

#ifdef _XBOX
    // mmStats are the memory manager stats
    MM_STATISTICS  mmStats;
    // psStats are the process manager stats
    PS_STATISTICS  psStats;
#else
    // vmStats are the virtual memory stats
    VM_COUNTERS    vmStats;
    // dwHandleCount is the handle count
    DWORD          dwHandleCount = 0;
#endif

    // bReturnCode is the return code of this function
    BOOL           bReturnCode = TRUE;
    // dwErrorCode is the last error code
    DWORD          dwErrorCode = ERROR_SUCCESS;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

#ifdef _XBOX
    // Initialize the mm stats
    ZeroMemory(&mmStats, sizeof(mmStats));
    mmStats.Length = sizeof(mmStats);

    // Initialize the ps stats
    ZeroMemory(&psStats, sizeof(psStats));
    psStats.Length = sizeof(psStats);

    // Query the mm stats
    MmQueryStatistics(&mmStats);

    // Query the ps stats
    PsQueryStatistics(&psStats);
#else
    // Initialize the vm stats
    ZeroMemory(&vmStats, sizeof(vmStats));

    // Query the stats
    NtQueryInformationProcess(NtCurrentProcess(), ProcessVmCounters, &vmStats, sizeof(vmStats), NULL);

    // Query the handle count
    NtQueryInformationProcess(NtCurrentProcess(), ProcessHandleCount, &dwHandleCount, sizeof(dwHandleCount), NULL);
#endif

    // Log the resource status banner
    bReturnCode &= LogBanner(hLog, "Resource Status", lpszLogString_A);

#ifdef _XBOX
    // Log the resource status
    bReturnCode &= FormatAndWriteString(hLog, "Total Memory,%u\r\n", mmStats.TotalPhysicalPages * PAGE_SIZE);
    bReturnCode &= FormatAndWriteString(hLog, "Available Memory,%u\r\n", mmStats.AvailablePages * PAGE_SIZE);
    bReturnCode &= FormatAndWriteString(hLog, "Cache,%u\r\n", mmStats.CachePagesCommitted * PAGE_SIZE);
    bReturnCode &= FormatAndWriteString(hLog, "Pool,%u\r\n", mmStats.PoolPagesCommitted * PAGE_SIZE);
    bReturnCode &= FormatAndWriteString(hLog, "Stack,%u\r\n", mmStats.StackPagesCommitted * PAGE_SIZE);
    bReturnCode &= FormatAndWriteString(hLog, "Image,%u\r\n", mmStats.ImagePagesCommitted * PAGE_SIZE);
    bReturnCode &= FormatAndWriteString(hLog, "Handle Count,%u\r\n", psStats.HandleCount);
#else
    // Log the resource status
    bReturnCode &= FormatAndWriteString(hLog, "Working Set,%u\r\n", vmStats.WorkingSetSize / 1024);
    bReturnCode &= FormatAndWriteString(hLog, "Virtual Bytes,%u\r\n", vmStats.VirtualSize / 1024);
    bReturnCode &= FormatAndWriteString(hLog, "Paged Pool,%u\r\n", vmStats.QuotaPagedPoolUsage / 1024);
    bReturnCode &= FormatAndWriteString(hLog, "Non-Paged Pool,%u\r\n", vmStats.QuotaNonPagedPoolUsage / 1024);
    bReturnCode &= FormatAndWriteString(hLog, "Pagefile,%u\r\n", vmStats.PagefileUsage / 1024);
    bReturnCode &= FormatAndWriteString(hLog, "Handle Count,%u\r\n", dwHandleCount);
#endif
    bReturnCode &= FormatAndWriteString(hLog, "**********\r\n");

    if (INVALID_HANDLE_VALUE != pLogObject->hLogFile) {
        // Flush the file buffers to prevent corruption
        FlushFileBuffers(pLogObject->hLogFile);
    }

    if (ERROR_SUCCESS != dwErrorCode) {
        // Set the last error code
        SetLastError(dwErrorCode);
    }

    return bReturnCode;
}



BOOL
WINAPI
xLogResults(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszLogString_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Logs the variation results of the XLOG_OBJECT

Arguments:

  hLog - Handle to the XLOG_OBJECT
  lpszLogString_A - Pointer to a null-terminated string (ANSI) that specifies the log string

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT  pLogObject = (PXLOG_OBJECT) hLog;
    // bReturnCode is the return code of this function
    BOOL          bReturnCode = TRUE;



    if ((INVALID_HANDLE_VALUE == hLog) || (NULL == hLog)) {
        // Set the last error code
        SetLastError(ERROR_INVALID_PARAMETER);

        return FALSE;
    }

    // Wait for access to the XLOG_OBJECT
    EnterCriticalSection(pLogObject->pcs);

    // Log the results banner
    bReturnCode &= LogBanner(hLog, "Results", lpszLogString_A);

    // Log the results
    bReturnCode &= FormatAndWriteString(hLog, "Total Variations,%u\r\n", pLogObject->dwVariationsTotal);
    bReturnCode &= FormatAndWriteString(hLog, "Exceptions,%u\r\n", pLogObject->dwVariationsException);
    bReturnCode &= FormatAndWriteString(hLog, "Break,%u\r\n", pLogObject->dwVariationsBreak);
    bReturnCode &= FormatAndWriteString(hLog, "Failures,%u\r\n", pLogObject->dwVariationsFail);
    bReturnCode &= FormatAndWriteString(hLog, "Warnings,%u\r\n", pLogObject->dwVariationsWarning);
    bReturnCode &= FormatAndWriteString(hLog, "Blocked,%u\r\n", pLogObject->dwVariationsBlock);
    bReturnCode &= FormatAndWriteString(hLog, "Passed,%u\r\n", pLogObject->dwVariationsPass);
    bReturnCode &= FormatAndWriteString(hLog, "Unknown,%u\r\n", pLogObject->dwVariationsUnknown);
    bReturnCode &= FormatAndWriteString(hLog, "**********\r\n");

    if (INVALID_HANDLE_VALUE != pLogObject->hLogFile) {
        // Flush the file buffers to prevent corruption
        FlushFileBuffers(pLogObject->hLogFile);
    }

    // Release access to the XLOG_OBJECT
    LeaveCriticalSection(pLogObject->pcs);

    return bReturnCode;
}

} // namespace xLogNamespace
