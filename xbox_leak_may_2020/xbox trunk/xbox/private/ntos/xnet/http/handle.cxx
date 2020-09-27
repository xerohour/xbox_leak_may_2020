/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    handle.cxx

Abstract:

    Functions for managing various types of WININET handles

Revision History:

    08/08/2000 davidx
        Created it.

--*/

#include "precomp.h"

HINTERNET WINAPI
InternetOpen(
    IN LPCWSTR lpszAgent,
    IN DWORD dwAccessType,
    IN LPCWSTR lpszProxy OPTIONAL,
    IN LPCWSTR lpszProxyBypass OPTIONAL,
    IN DWORD dwFlags
    )

/*++

Routine Description:

    Initializes an application's use of the Wininet functions.

Arguments:

    lpszAgent - Name of the user agent
    dwAccessType - Type of access required
    lpszProxy - Host name of the proxy server
    lpszProxyBypass - Must be NULL
    dwFlags - Must be 0

Return Value:

    Handle to the Wininet object
    NULL if there is an error

--*/

{
    // Validate function parameters:
    // - INTERNET_OPEN_TYPE_PRECONFIG and
    //   INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY
    //   are treated the same way as INTERNET_OPEN_TYPE_DIRECT
    // - we don't support proxy bypass,
    //   either everything go through the proxy
    //   or nothing go through the proxy
    // - flag parameter must be 0

    switch (dwAccessType) {
    case INTERNET_OPEN_TYPE_DIRECT:
    case INTERNET_OPEN_TYPE_PRECONFIG:
    case INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY:
        if (lpszProxy) {
            WARNING_("Proxy server ignored for direct connection");
            lpszProxy = NULL;
        }
        break;
    
    case INTERNET_OPEN_TYPE_PROXY:
        if (lpszProxy) break;

        // Fall through

    default:
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (lpszProxyBypass || dwFlags) {
        WARNING_("Unsupported InternetOpen parameters");
        SetLastError(ERROR_NOT_SUPPORTED);
        return NULL;
    }

    return InternetObject::Create(lpszAgent, lpszProxy);
}


BOOL WINAPI
InternetCloseHandle(
    IN HINTERNET hInternet
    )

/*++

Routine Description:

    Closes a single Internet handle.

Arguments:

    hInternet - Specifies the handle to be closed

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    BaseObject* obj = BaseObject::Lock(hInternet, OBJTYPE_NONE);
    if (!obj) return FALSE;

    obj->Unlock();
    obj->Release();
    return TRUE;
}


HINTERNET WINAPI
InternetConnect(
    IN HINTERNET hInternet,
    IN LPCWSTR lpszServerName,
    IN INTERNET_PORT nServerPort,
    IN LPCWSTR lpszUserName OPTIONAL,
    IN LPCWSTR lpszPassword OPTIONAL,
    IN DWORD dwService,
    IN DWORD dwFlags,
    IN DWORD_PTR dwContext
    )

/*++

Routine Description:

    Opens a HTTP session for a given site.

Arguments:

    hInternet - Handle returned by InternetOpen
    lpszServerName - Server host name
    nServerPort - Server port number
    lpszUserName - Username
    lpszPassword - Password
    dwService - Must be INTERNET_SERVICE_HTTP
    dwFlags - Must be 0
    dwContext - App-specific context value

Return Value:

    Handle to the connection object
    NULL if there is an error

--*/

{
    InternetObject* inetobj;
    ConnectObject* connobj;

    // Just assert to save code in free build
    ASSERT(lpszServerName != NULL);
    
    if (dwService != INTERNET_SERVICE_HTTP) {
        WARNING_("Unsupported InternetConnect parameters");
        SetLastError(ERROR_NOT_SUPPORTED);
        return NULL;
    }

    if (dwFlags != 0) {
        WARNING_("Flags to InternetConnect ignored: 0x%x", dwFlags);
    }
    
    inetobj = LOCK_INTERNET_OBJECT(hInternet);
    if (!inetobj) return NULL;

    // No need to hold the lock to inetobj
    // because it doesn't have any volatile fields.
    // Just incrementing its refcount is enough.
    inetobj->AddRef();
    inetobj->Unlock();

    connobj = ConnectObject::Create(
                inetobj,
                lpszServerName,
                nServerPort,
                lpszUserName,
                lpszPassword,
                dwContext);

    inetobj->Release();
    return connobj;
}


HINTERNET WINAPI
HttpOpenRequest(
    IN HINTERNET hConnect,
    IN LPCWSTR lpszVerb,
    IN LPCWSTR lpszObjectName,
    IN LPCWSTR lpszVersion,
    IN LPCWSTR lpszReferrer OPTIONAL,
    IN LPCWSTR* lplpszAcceptTypes OPTIONAL,
    IN DWORD dwFlags,
    IN DWORD_PTR dwContext
    )

/*++

Routine Description:

    Creates an HTTP request handle.

Arguments:

    hConnect - Handle to the HTTP connection object
    lpszVerb - Verb to use for the request
    lpszObjectName - Object name
    lpszVersion - HTTP version number
    lpszReferrer - Referer URL
    lplpszAcceptTypes - Media types accepted by the client
    dwFlags - Must be 0
    dwContext - App-specific context value

Return Value:

    Handle to the request object
    NULL if there is an error

--*/

{
    ConnectObject* connobj;
    RequestObject* reqobj;

    if (dwFlags != 0) {
        WARNING_("Unsupported HttpOpenRequest parameters");
        SetLastError(ERROR_NOT_SUPPORTED);
        return NULL;
    }

    connobj = LOCK_CONNECT_OBJECT(hConnect);
    if (!connobj) return NULL;

    connobj->AddRef();
    connobj->Unlock();

    if (!lpszVerb) lpszVerb = HttpDefaultVerbStr;
    if (!lpszVersion) lpszVersion = HttpDefaultVersionStr;
    if (!lpszObjectName) lpszObjectName = HttpDefaultObjectStr;

    reqobj = RequestObject::Create(
                connobj,
                lpszVerb,
                lpszObjectName,
                lpszVersion,
                lpszReferrer,
                lplpszAcceptTypes,
                dwContext);

    connobj->Release();
    return reqobj;
}


HINTERNET WINAPI
InternetOpenUrl(
    IN HINTERNET hInternet,
    IN LPCWSTR lpszUrl,
    IN LPCWSTR lpszHeaders OPTIONAL,
    IN DWORD dwHeadersLength,
    IN DWORD dwFlags,
    IN DWORD_PTR dwContext
    )

/*++

Routine Description:

    Begins reading a complete HTTP URL

Arguments:

    hInternet - Points to the internet object returned by InternetOpen
    lpszUrl - URL string
    lpszHeaders - Extra HTTP request headers
    dwHeadersLength - Extra header length
    dwFlags - Must be 0
    dwContext - App-specific context value

Return Value:

    Handle to the HTTP request object
    NULL if there is an error

--*/

{
    WCHAR* serverName = NULL;
    WCHAR* objectName = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    INTERNET_PORT serverPort;

    //
    // Parse the HTTP URL to extra the server name and the object name
    //
    if (!HttpParseUrl(lpszUrl, &serverName, &serverPort, &objectName))
        return NULL;

    //
    // Create a connection handle
    //
    hConnect = InternetConnect(
                    hInternet,
                    serverName,
                    serverPort,
                    NULL,
                    NULL,
                    INTERNET_SERVICE_HTTP,
                    0,
                    0);

    if (hInternet) {
        //
        // Create a request handle
        //
        hRequest = HttpOpenRequest(
                        hConnect,
                        NULL,
                        objectName,
                        NULL,
                        NULL,
                        NULL,
                        0,
                        0);

        //
        // Send out the HTTP request to the server
        //
        if (hRequest &&
            !HttpSendRequest(hRequest, lpszHeaders, dwHeadersLength, NULL, 0)) {
            InternetCloseHandle(hRequest);
            hRequest = NULL;
        }
    }

    if (hConnect) { InternetCloseHandle(hConnect); }
    Free(serverName);
    Free(objectName);

    if (!hRequest) {
        WARNING_("InternetOpenUrl failed: %d", GetLastError());
    }
    return hRequest;
}


BOOL WINAPI
HttpSendRequest(
    IN HINTERNET hRequest,
    IN LPCWSTR lpszHeaders OPTIONAL,
    IN DWORD dwHeadersLength,
    IN LPVOID lpOptional OPTIONAL,
    IN DWORD dwOptionalLength
    )

/*++

Routine Description:

    Sends the specified request to the HTTP server.

Arguments:

    hRequest - Handle to an HTTP request object
    lpszHeaders - Extra HTTP request headers
    dwHeadersLength - Extra header length
    lpOptional - Optional data sent in the HTTP request
    dwOptionalLength - Optional data length

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    RequestObject* reqobj;
    BOOL result;

    ASSERT(lpszHeaders || !dwHeadersLength);
    ASSERT(lpOptional || !dwOptionalLength);

    reqobj = LOCK_REQUEST_OBJECT(hRequest);
    if (!reqobj) return FALSE;

    result = reqobj->SendRequest(
                lpszHeaders,
                dwHeadersLength,
                lpOptional,
                dwOptionalLength);

    reqobj->Unlock();
    return result;
}


BOOL WINAPI
InternetQueryDataAvailable(
    IN HINTERNET hRequest,
    OUT LPDWORD lpdwNumberOfBytesAvailable,
    IN DWORD dwFlags,
    IN DWORD_PTR dwContext
    )

/*++

Routine Description:

    Query the amount of response data currently available for reading

Arguments:

    hRequest - Handle to the HTTP request object
    lpdwNumberOfBytesAvailable - Returns the number of bytes available
    dwFlags - Must be 0
    dwContext - Ignored

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    RequestObject* reqobj;
    BOOL result;
    
    ASSERT(lpdwNumberOfBytesAvailable != NULL);

    if (dwFlags != 0) {
        WARNING_("Unsupported InternetQueryDataAvailable parameters");
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }
    
    reqobj = LOCK_REQUEST_OBJECT(hRequest);
    if (!reqobj) return FALSE;

    result = reqobj->QueryDataAvailable(lpdwNumberOfBytesAvailable);
    reqobj->Unlock();

    return result;
}


BOOL WINAPI
InternetReadFile(
    IN HINTERNET hRequest,
    IN LPVOID lpBuffer,
    IN DWORD dwNumberOfBytesToRead,
    OUT LPDWORD lpdwNumberOfBytesRead
    )

/*++

Routine Description:

    Read HTTP response data

Arguments:

    hRequest - Handle to the HTTP request
    lpBuffer - Output data buffer
    dwNumberOfBytesToRead - Output buffer size
    lpdwNumberOfBytesRead - Returns the number of bytes actually read

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    RequestObject* reqobj;
    BOOL result;

    ASSERT(lpBuffer && lpdwNumberOfBytesRead);

    reqobj = LOCK_REQUEST_OBJECT(hRequest);
    if (!reqobj) return FALSE;

    result = reqobj->ReadData((CHAR*) lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);
    reqobj->Unlock();

    return result;
}


BOOL WINAPI
HttpQueryInfo(
    IN HINTERNET hRequest,
    IN DWORD dwInfoLevel,
    IN LPVOID lpvBuffer,
    IN LPDWORD lpdwBufferLength,
    IN OUT LPDWORD lpdwIndex
    )

/*++

Routine Description:

    Retrieves header information associated with an HTTP request

Arguments:

    hRequest - Handle to the HTTP request
    dwInfoLevel - Specifies what attribute to retrieve
    lpvBuffer - Output data buffer
    lpdwBufferLength - Output data buffer size
        On entry, it contains the buffer size in number of WCHARs
        On return, it contains either the actual output data in number
        of WCHARs (not including the null terminator) or the actual
        number of bytes needed if the output buffer is too small
    lpdwIndex - 0-based index specifying which occurrence if of interest

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    RequestObject* reqobj;
    BOOL result;

    ASSERT(lpdwBufferLength);
    ASSERT(lpvBuffer || *lpdwBufferLength == 0);

    reqobj = LOCK_REQUEST_OBJECT(hRequest);
    if (!reqobj) return FALSE;

    result = reqobj->QueryRespInfo(
                        dwInfoLevel,
                        (WCHAR*) lpvBuffer,
                        lpdwBufferLength,
                        lpdwIndex);

    reqobj->Unlock();
    return result;
}

