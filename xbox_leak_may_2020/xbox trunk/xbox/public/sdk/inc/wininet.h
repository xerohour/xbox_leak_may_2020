/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    wininet.h

Abstract:

    Lightweight HTTP client library for XBox

--*/

#ifndef _WININET_H
#define _WININET_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef HANDLE HINTERNET;
typedef WORD INTERNET_PORT;

//
// Initializes an application's use of the Win32114 Internet functions.
//
HINTERNET WINAPI
InternetOpen(
    IN LPCWSTR lpszAgent,
    IN DWORD dwAccessType,
    IN LPCWSTR lpszProxy OPTIONAL,
    IN LPCWSTR lpszProxyBypass OPTIONAL,
    IN DWORD dwFlags
    );

// Constant values for dwAccessType parameter

#define INTERNET_OPEN_TYPE_PRECONFIG                    0   // use registry configuration
#define INTERNET_OPEN_TYPE_DIRECT                       1   // direct to net
#define INTERNET_OPEN_TYPE_PROXY                        3   // via named proxy
#define INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY  4   // prevent using java/script/INS

//
// Closes a single Internet handle.
//
BOOL WINAPI
InternetCloseHandle(
    IN HINTERNET hInternet
    );

//
// Begins reading a complete HTTP URL. 
//
HINTERNET WINAPI
InternetOpenUrl(
    IN HINTERNET hInternet,
    IN LPCWSTR lpszUrl,
    IN LPCWSTR lpszHeaders OPTIONAL,
    IN DWORD dwHeadersLength,
    IN DWORD dwFlags,
    IN DWORD_PTR dwContext
    );

//
// Opens a HTTP session for a given site.
//
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
    );

// Constant values for nServerPort parameter

#define INTERNET_INVALID_PORT_NUMBER    0
#define INTERNET_DEFAULT_FTP_PORT       21
#define INTERNET_DEFAULT_GOPHER_PORT    70
#define INTERNET_DEFAULT_HTTP_PORT      80
#define INTERNET_DEFAULT_HTTPS_PORT     443
#define INTERNET_DEFAULT_SOCKS_PORT     1080

// Constant values for dwService parameter

#define INTERNET_SERVICE_FTP    1
#define INTERNET_SERVICE_GOPHER 2
#define INTERNET_SERVICE_HTTP   3

//
// Creates an HTTP request handle.
//
HINTERNET WINAPI
HttpOpenRequest(
    IN HINTERNET hConnect,
    IN LPCWSTR lpszVerb,
    IN LPCWSTR lpszObjectName,
    IN LPCWSTR lpszVersion,
    IN LPCWSTR lpszReferrer OPTIONAL,
    IN LPCWSTR FAR * lplpszAcceptTypes OPTIONAL,
    IN DWORD dwFlags,
    IN DWORD_PTR dwContext
    );

//
// Sends the specified request to the HTTP server.
//
BOOL WINAPI
HttpSendRequest(
    IN HINTERNET hRequest,
    IN LPCWSTR lpszHeaders OPTIONAL,
    IN DWORD dwHeadersLength,
    IN LPVOID lpOptional OPTIONAL,
    IN DWORD dwOptionalLength
    );

//
// Queries the server to determine the amount of data available.
//
BOOL WINAPI
InternetQueryDataAvailable(
    IN HINTERNET hRequest,
    OUT LPDWORD lpdwNumberOfBytesAvailable,
    IN DWORD dwFlags,
    IN DWORD_PTR dwContext
    );

//
// Reads data from a handle opened by the InternetOpenUrl or HttpOpenRequest function.
//
BOOL WINAPI
InternetReadFile(
    IN HINTERNET hRequest,
    IN LPVOID lpBuffer,
    IN DWORD dwNumberOfBytesToRead,
    OUT LPDWORD lpdwNumberOfBytesRead
    );

//
// Retrieves header information associated with an HTTP request
//
BOOL WINAPI
HttpQueryInfo(
    IN HINTERNET hRequest,
    IN DWORD dwInfoLevel,
    IN LPVOID lpvBuffer,
    IN LPDWORD lpdwBufferLength,
    IN OUT LPDWORD lpdwIndex
    );

//
// HttpQueryInfo info levels. Generally, there is one info level
// for each potential RFC822/HTTP/MIME header that an HTTP server
// may send as part of a request response.
//
// The HTTP_QUERY_RAW_HEADERS info level is provided for clients
// that choose to perform their own header parsing.
//

#define HTTP_QUERY_CONTENT_TYPE     1
#define HTTP_QUERY_CONTENT_LENGTH   5
#define HTTP_QUERY_DATE             9
#define HTTP_QUERY_EXPIRES          10
#define HTTP_QUERY_LAST_MODIFIED    11
#define HTTP_QUERY_VERSION          18  // special: part of status line
#define HTTP_QUERY_STATUS_CODE      19  // special: part of status line
#define HTTP_QUERY_STATUS_TEXT      20  // special: part of status line
#define HTTP_QUERY_RAW_HEADERS      21  // special: all headers as MULTI_SZ
#define HTTP_QUERY_RAW_HEADERS_CRLF 22  // special: all headers
#define HTTP_QUERY_ACCEPT           24

//
// HTTP_QUERY_CUSTOM - if this special value is supplied as the dwInfoLevel
// parameter of HttpQueryInfo() then the lpBuffer parameter contains the name
// of the header we are to query
//

#define HTTP_QUERY_CUSTOM 65535

//
// HTTP_QUERY_FLAG_NUMBER - if this bit is set in the dwInfoLevel parameter of
// HttpQueryInfo(), then the value of the header will be converted to a number
// before being returned to the caller, if applicable
//

#define HTTP_QUERY_FLAG_NUMBER 0x20000000

//
// HTTP_QUERY_FLAG_SYSTEMTIME - if this bit is set in the dwInfoLevel parameter
// of HttpQueryInfo() AND the header being queried contains date information,
// e.g. the "Expires:" header then lpBuffer will contain a SYSTEMTIME structure
// containing the date and time information converted from the header string
//

#define HTTP_QUERY_FLAG_SYSTEMTIME 0x40000000

//
// the default major/minor HTTP version numbers
//

#define HTTP_MAJOR_VERSION      1
#define HTTP_MINOR_VERSION      0

#define HTTP_VERSION            TEXT("HTTP/1.0")

#if defined(__cplusplus)
}
#endif

#endif // !_WININET_H

