/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  util.c

Abstract:

  This module contains the utility functions for xlog.dll

Author:

  Steven Kehrli (steveke) 13-Mar-2000

------------------------------------------------------------------------------*/

#include "precomp.h"



namespace xLogNamespace {

LPWSTR
MultiByteToUnicodeString(
    IN  HANDLE  hMemObject,
    IN  LPSTR   lpszString_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Converts a multi-byte string to a UNICODE string

Arguments:

  hMemObject - Handle to the memory object for memory allocation
  lpszString_A - Pointer to a null-terminated multi-byte string

Return Value:

  LPWSTR:
    If the function succeeds, the return value is a pointer to the null-terminated UNICODE string
    If the function fails, the return value is NULL

------------------------------------------------------------------------------*/
{
    // lpszString_W is the UNICODE equivalent of lpszString_A
    LPWSTR    lpszString_W = NULL;
    // dwBufferSize is the size of the UNICODE string
    DWORD     dwBufferSize = 0;

    // NtStatus is the nt status code returned by RtlMultiByteToUnicodeString()
    NTSTATUS  NtStatus = STATUS_SUCCESS;
    // dwErrorCode is the win32 error code represented by NtStatus
    DWORD     dwErrorCode = ERROR_SUCCESS;



    // Get the size of the lpszString_W string
    NtStatus = RtlMultiByteToUnicodeSize(&dwBufferSize, lpszString_A, strlen(lpszString_A) + 1);

    if (!NT_SUCCESS(NtStatus)) {
        // Get the last error code
        dwErrorCode = RtlNtStatusToDosError(NtStatus);

        goto FunctionFailed0;
    }

    // Allocate the memory for the lpszString_W string
    lpszString_W = (LPWSTR) xMemAlloc(hMemObject, dwBufferSize);

    if (NULL == lpszString_W) {
        // Get the last error code
        dwErrorCode = GetLastError();

        goto FunctionFailed0;
    }

    // Get the lpszString_W string
    NtStatus = RtlMultiByteToUnicodeN(lpszString_W, dwBufferSize, NULL, lpszString_A, strlen(lpszString_A) + 1);

    if (!NT_SUCCESS(NtStatus)) {
        // Get the last error code
        dwErrorCode = RtlNtStatusToDosError(NtStatus);

        goto FunctionFailed1;
    }

    return lpszString_W;

FunctionFailed1:
    // Free the lpszString_W string
    xMemFree(hMemObject, lpszString_W);

FunctionFailed0:
    // Set the last error code
    SetLastError(dwErrorCode);

    return NULL;
}



LPSTR
UnicodeToMultiByteString(
    IN  HANDLE  hMemObject,
    IN  LPWSTR  lpszString_W
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Converts a UNICODE string to a multi-byte string

Arguments:

  hMemObject - Handle to the memory object for memory allocation
  lpszString_W - Pointer to a null-terminated UNICODE string

Return Value:

  LPSTR:
    If the function succeeds, the return value is a pointer to a null-terminated multi-byted string
    If the function fails, the return value is NULL

------------------------------------------------------------------------------*/
{
    // lpszString_A is the multi-byte equivalent of lpszString_W
    LPSTR     lpszString_A = NULL;
    // dwBufferSize is the size of the multi-byte string
    DWORD     dwBufferSize = 0;

    // NtStatus is the nt status code returned by RtlMultiByteToUnicodeString()
    NTSTATUS  NtStatus = STATUS_SUCCESS;
    // dwErrorCode is the win32 error code represented by NtStatus
    DWORD     dwErrorCode = ERROR_SUCCESS;



    // Get the size of the lpszString_A string
    NtStatus = RtlUnicodeToMultiByteSize(&dwBufferSize, lpszString_W, (wcslen(lpszString_W) + 1) * sizeof(WCHAR));

    if (!NT_SUCCESS(NtStatus)) {
        // Get the last error code
        dwErrorCode = RtlNtStatusToDosError(NtStatus);

        goto FunctionFailed0;
    }

    // Allocate the memory for the lpszString_A string
    lpszString_A = (LPSTR) xMemAlloc(hMemObject, dwBufferSize);

    if (NULL == lpszString_A) {
        // Get the last error code
        dwErrorCode = GetLastError();

        goto FunctionFailed0;
    }

    // Get the lpszString_A string
    NtStatus = RtlUnicodeToMultiByteN(lpszString_A, dwBufferSize, NULL, lpszString_W, (wcslen(lpszString_W) + 1) * sizeof(WCHAR));

    if (!NT_SUCCESS(NtStatus)) {
        // Get the last error code
        dwErrorCode = RtlNtStatusToDosError(NtStatus);

        goto FunctionFailed1;
    }

    return lpszString_A;

FunctionFailed1:
    // Free the lpszString_A string
    xMemFree(hMemObject, lpszString_A);

FunctionFailed0:
    // Set the last error code
    SetLastError(dwErrorCode);

    return NULL;
}



BOOL
FormatAndWriteString(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszFormatString_A,
    IN  ...
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Formats and writes a string to the log file

Arguments:

  hLog - Handle to the XLOG_OBJECT
  lpszFormatString_A - Pointer to a null-terminated string (ANSI) that consists of format-control specifications

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT  pLogObject = (PXLOG_OBJECT) hLog;
    // varg_ptr is a pointer to the variable argument list
    va_list       varg_ptr = NULL;
    // szOutputString_A is the line of log output
    CHAR          szOutputString_A[1025] = {'\0'};
    // dwBytesWritten is the number of bytes written to the file
    DWORD         dwBytesWritten = 0;
    // bReturnValue is the return value of this function
    BOOL          bReturnValue = TRUE;



    // Get the variable argument list
    va_start(varg_ptr, lpszFormatString_A);
    _vsnprintf(szOutputString_A, 1024, lpszFormatString_A, varg_ptr);

#ifdef _XBOX
    // Wait for access to the console socket critical section
    EnterCriticalSection(pLogObject->pConsoleSocketcs);

    // Echo the line to the remote console, if specified
    if (INVALID_SOCKET != pLogObject->sConsoleSocket) {
        if (SOCKET_ERROR == send(pLogObject->sConsoleSocket, szOutputString_A, strlen(szOutputString_A), 0)) {
            goto FunctionExit;
        }
    }

    // Release access to the console socket critical section
    LeaveCriticalSection(pLogObject->pConsoleSocketcs);
#endif

    // Write the line to the log file, if specified
    if (INVALID_HANDLE_VALUE != pLogObject->hLogFile) {
        if (FALSE == WriteFile(pLogObject->hLogFile, szOutputString_A, strlen(szOutputString_A), &dwBytesWritten, NULL)) {
            goto FunctionExit;
        }
    }

    // Echo the line to the debugger, if specified
    if (0 != (XLO_DEBUG & pLogObject->dwLogOptions)) {
        OutputDebugStringA(szOutputString_A);
    }

    bReturnValue = TRUE;

FunctionExit:
    return bReturnValue;
}



BOOL
LogBanner(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszTitleString_A,
    IN  LPSTR   lpszLogString_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Logs a banner

Arguments:

  hLog - Handle to the XLOG_OBJECT
  lpszTitleString_A - Pointer to a null-terminated string (ANSI) that specifies the title string
  lpszLogString_A - Pointer to a null-terminated string (ANSI) that specifies the log string

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // LocalTime is the current local time in hours, minutes, seconds, etc.
    SYSTEMTIME  LocalTime;



    // Get the current time
    GetLocalTime(&LocalTime);

    // Write the banner to the log file
    if (NULL == lpszLogString_A) {
        return FormatAndWriteString(hLog, "\r\n**********\r\n%s,%02d/%02d/%04d,%02d:%02d:%02d\r\n**********\r\n", lpszTitleString_A, LocalTime.wMonth, LocalTime.wDay, LocalTime.wYear, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);
    }

    return FormatAndWriteString(hLog, "\r\n**********\r\n%s,%02d/%02d/%04d,%02d:%02d:%02d\r\n%s**********\r\n", lpszTitleString_A, LocalTime.wMonth, LocalTime.wDay, LocalTime.wYear, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond, lpszLogString_A);
}



SOCKET
ConnectWebServer(
    IN  u_long  HttpAddr
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Connects to the web server

Arguments:

  HttpAddr - Address of the web server

Return Value:

  SOCKET:
    If the function succeeds, the return value is a valid socket descriptor.
    If the function fails, the return value is INVALID_SOCKET.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // sWebSocket is the web socket
    SOCKET        sWebSocket = INVALID_SOCKET;
    // webname is the address and port of the web server
    SOCKADDR_IN   webname = {AF_INET, 0, 0, 0};
    // szDebugString is the debug string
    CHAR          szDebugString[61];
    // dwErrorCode is the win32 error code
    DWORD         dwErrorCode = ERROR_SUCCESS;


    
    // Setup the web address and port
    webname.sin_port = htons(80);
    webname.sin_addr.s_addr = HttpAddr;

    // Create the connection
    sWebSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == sWebSocket) {
        // Get the last error code
        dwErrorCode = WSAGetLastError();

        sprintf(szDebugString, "xLog: Cannot create the web socket - ec = %u\r\n", dwErrorCode);
        OutputDebugStringA(szDebugString);
    }
    // Connect to the web server
    else if (SOCKET_ERROR == connect(sWebSocket, (SOCKADDR *) &webname, sizeof(webname))) {
        // Get the last error code
        dwErrorCode = WSAGetLastError();

        sprintf(szDebugString, "xLog: Cannot connect to the web server - ec = %u\r\n", dwErrorCode);
        OutputDebugStringA(szDebugString);

        closesocket(sWebSocket);
        sWebSocket = INVALID_SOCKET;
    }

    if (ERROR_SUCCESS != dwErrorCode) {
        SetLastError(dwErrorCode);
    }

    return sWebSocket;
}



BOOL
PostString(
    IN  SOCKET  sWebSocket,
    IN  LPSTR   szString_A,
    IN  DWORD   dwStringLen
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Sends a string to the web server

Arguments:

  sWebSocket - Web socket descriptor
  szString_A - Pointer to a null-terminated string (ANSI)
  dwStringLen - Specifies the length of the string

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // webname is the address and port of the web server
    SOCKADDR_IN   webname = {AF_INET, 0, 0, 0};
    // dwErrorCode is the win32 error code
    DWORD         dwErrorCode = ERROR_SUCCESS;



    // Send the web post
    if (SOCKET_ERROR == send(sWebSocket, szString_A, dwStringLen, 0)) {
        // Get the last error code
        dwErrorCode = WSAGetLastError();

        closesocket(sWebSocket);
    }
    // Receive the web post
    else if (SOCKET_ERROR == recv(sWebSocket, szString_A, dwStringLen, 0)) {
        // Get the last error code
        dwErrorCode = WSAGetLastError();

        closesocket(sWebSocket);
    }
    
    if (ERROR_SUCCESS != dwErrorCode) {
        // Set the last error code
        SetLastError(dwErrorCode);
    }

    return (ERROR_SUCCESS == dwErrorCode);
}



VOID
PostResult(
    IN  HANDLE  hLog,
    IN  DWORD   dwVariationResult,
    IN  LPSTR   lpszComponentName_A,
    IN  LPSTR   lpszSubcomponentName_A,
    IN  LPSTR   lpszFunctionName_A,
    IN  LPSTR   lpszVariationName_A,
    IN  LPSTR   lpszLogString_A
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Posts a string to the web server

Arguments:

  hLog - Handle to the XLOG_OBJECT
  dwVariationResult - Specifies the logging level for this line of output.  It can be one of the XLL_ #defines.
  lpszComponentName_A - Pointer to a null-terminated string (ANSI) that specifies the component name
  lpszSubcomponentName_A - Pointer to a null-terminated string (ANSI) that specifies the subcomponent name
  lpszFunctionName_A - Pointer to a null-terminated string (ANSI) that specifies the function name
  lpszVariationName_A - Pointer to a null-terminated string (ANSI) that specifies the variation name
  lpszLogString_A - Pointer to a null-terminated string (ANSI)

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT  pLogObject = (PXLOG_OBJECT) hLog;
    
    // szWebPostLogData_A is the line of web post data
    CHAR          szWebPostLogData_A[WEBPOST_LOGDATA_LENGTH] = {'\0'};
    // szWebPostLog_A is the web post
    CHAR          szWebPostLog_A[WEBPOST_LOG_LENGTH] = {'\0'};
    // szWebPostEntry_A is the web post entry for the file
    CHAR          szWebPostEntry_A[5 + 12 + WEBPOST_LOG_LENGTH + 3] = {'\0'};
    // dwBytesWritten is the number of bytes written to the file
    DWORD         dwBytesWritten = 0;

    // szDebugString is the debug string
    CHAR          szDebugString[61];



    if (NULL != pLogObject->lpszLogPage_A) {
        // Format the web post data
        sprintf(szWebPostLogData_A, WEBPOST_LOGDATA_FORMAT_A, pLogObject->szHostName_A, pLogObject->szObjectUUID_A, pLogObject->szOwnerAlias_A, GetCurrentThreadId(), dwVariationResult, lpszComponentName_A, lpszSubcomponentName_A, lpszFunctionName_A, lpszVariationName_A, lpszLogString_A);

        // Format the web post
        sprintf(szWebPostLog_A, WEBPOST_FORMAT_A, pLogObject->lpszLogPage_A, pLogObject->lpszHttpServer_A, strlen(szWebPostLogData_A), szWebPostLogData_A);

        if (INVALID_HANDLE_VALUE != pLogObject->hWebPostFile) {
            // Format the web post entry for the file
            sprintf(szWebPostEntry_A, "%03u\r\n0x%08x\r\n%s\r\n", strlen(szWebPostLog_A), pLogObject->HttpAddr, szWebPostLog_A);

            // Write the web post entry to the file
            WriteFile(pLogObject->hWebPostFile, szWebPostEntry_A, strlen(szWebPostEntry_A), &dwBytesWritten, NULL);

            // Flush the file buffers to prevent corruption
            FlushFileBuffers(pLogObject->hWebPostFile);
        }
    }
}



VOID
IncrementVariationResult(
    IN  HANDLE  hLog,
    IN  DWORD   dwVariationResult,
    IN  BOOL    bIncrementTotal
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Increments the variation result counters

Arguments:

  hLog - Handle to the XLOG_OBJECT
  dwVariationResult - Specifies the variation result.  It can be one of the XLL_ #defines.
  bIncrementTotal - Specifies if the total variations counter should be incremented.

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // pLogObject is the pointer to the XLOG_OBJECT
    PXLOG_OBJECT  pLogObject = (PXLOG_OBJECT) hLog;



    if (TRUE == bIncrementTotal) {
        // Increment the dwVariationsTotal member
        pLogObject->dwVariationsTotal++;
    }

    switch (dwVariationResult) {

    case XLL_EXCEPTION:
        // Increment the variations excepted counter
        pLogObject->dwVariationsException++;
        break;

    case XLL_BREAK:
        // Increment the variations broken counter
        pLogObject->dwVariationsBreak++;
        break;

    case XLL_FAIL:
        // Increment the variations failed counter
        pLogObject->dwVariationsFail++;
        break;

    case XLL_WARN:
        // Increment the variations warned counter
        pLogObject->dwVariationsWarning++;
        break;

    case XLL_BLOCK:
        // Increment the variations blocked counter
        pLogObject->dwVariationsBlock++;
        break;

    case XLL_PASS:
        // Increment the variations passed counter
        pLogObject->dwVariationsPass++;
        break;

    default:
        // Increment the variations unknown counter
        pLogObject->dwVariationsUnknown++;
        break;
    }
}



BOOL
AddRefNet(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Increments the reference count for the net stack

Return Value:

  BOOL:
    If the function succeeds, the return value is nonzero.
    If the function fails, the return value is zero.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // WSAData is the details of the Winsock implementation
    WSADATA            WSAData;
    // dwErrorCode is the last error code
    DWORD              dwErrorCode = ERROR_SUCCESS;



#ifdef _XBOX
    // Initialize the net stack
    dwErrorCode = XNetAddRef();
    if (0 == dwErrorCode) {
        goto FunctionFailed0;
    }
#endif

    // Initialize Winsock
    ZeroMemory(&WSAData, sizeof(WSAData));
    dwErrorCode = WSAStartup(MAKEWORD(2, 2), &WSAData);
    if (0 != dwErrorCode) {
        goto FunctionFailed1;
    }

    return TRUE;

FunctionFailed1:
#ifdef _XBOX
    // Terminate the net stack
    XNetRelease();

FunctionFailed0:
#endif
    return FALSE;
}



VOID
ReleaseNet(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Decrements the reference count for the net stack

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Terminate Winsock
    WSACleanup();

#ifdef _XBOX
    // Terminate the net stack
    XNetRelease();
#endif
}



LPSTR
GetHostName(
    IN HANDLE  hMemObject
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets the host name

Arguments:

  hMemObject - Handle to the memory object for memory allocation

Return Value:

  LPSTR:
    If the function succeeds, the return value is a pointer to the host name.
    If the function fails, the return value is NULL.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // lpszHostName is a pointer to the host name
    LPSTR  lpszHostName = NULL;
#ifdef _XBOX
    // HostNameLen is the length of the host name buffer
    DWORD  dwHostNameLen = 18;

    // MacAddr is the mac address
    BYTE   MacAddr[6];
    // dwType is the type
    DWORD  dwType = 0;
    // dwSize is the size
    DWORD  dwSize = 0;
#else
    // HostNameLen is the length of the host name buffer
    DWORD  dwHostNameLen = MAX_COMPUTERNAME_LENGTH + 1;
#endif

    // dwErrorCode is the last error code
    DWORD  dwErrorCode = ERROR_SUCCESS;



    // Allocate the memory for the host name
    lpszHostName = (LPSTR) xMemAlloc(hMemObject, dwHostNameLen);
    if (NULL == lpszHostName) {
        goto FunctionExit;
    }

#ifdef _XBOX
    // Get the mac address
    dwErrorCode = XQueryValue(XC_FACTORY_ETHERNET_ADDR, &dwType, MacAddr, sizeof(MacAddr), &dwSize);
    if (ERROR_SUCCESS != dwErrorCode) {
        goto FunctionExit;
    }

    // Copy the standard host name
    sprintf(lpszHostName, "%02x-%02x-%02x-%02x-%02x-%02x", MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]);
#else
    if (FALSE == GetComputerNameA(lpszHostName, &dwHostNameLen)) {
        dwErrorCode = GetLastError();
        goto FunctionExit;
    }
#endif

FunctionExit:
    if (ERROR_SUCCESS != dwErrorCode) {
        if (NULL != lpszHostName) {
            // Free the host name
            xMemFree(hMemObject, lpszHostName);
            lpszHostName = NULL;
        }

        // Set the last error code
        SetLastError(dwErrorCode);
    }

    return lpszHostName;
}

} // namespace xLogNamespace
