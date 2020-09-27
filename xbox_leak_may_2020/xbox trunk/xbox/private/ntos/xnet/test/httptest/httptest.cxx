// HTTP Client Library test program

#include "precomp.h"

INT testFlag = 1;
INT err;

#define BUFSIZE 4096
CHAR buf[BUFSIZE];
LPCWSTR acceptTypes[] = { L"*/*", NULL };

//
// Use our own new/delete operators
//
VOID* __cdecl operator new(size_t size) {
    return MAlloc(size);
}

VOID __cdecl operator delete(VOID* p) {
    Free(p);
}


//
// Dump the HTTP response headers
//
#define HTTPQUERYINFO(_infolevel, _buf, _len, _barf) \
        len = (_len); \
        ok = HttpQueryInfo(hRequest, _infolevel, &(_buf), &len, NULL); \
        if (!ok && _barf) { \
            WARNFAIL(HttpQueryInfo); return; \
        }


VOID
DumpResponseHeaders(
    HINTERNET hRequest
    )
{
    #define _FIELD(x) { x, #x }
    static const struct {
        DWORD index;
        CHAR* name;
    } infoLevels[] = {
        _FIELD(HTTP_QUERY_CONTENT_TYPE),
        _FIELD(HTTP_QUERY_CONTENT_LENGTH),
        _FIELD(HTTP_QUERY_DATE),
        _FIELD(HTTP_QUERY_EXPIRES),
        _FIELD(HTTP_QUERY_LAST_MODIFIED),
        _FIELD(HTTP_QUERY_VERSION),
        _FIELD(HTTP_QUERY_STATUS_CODE),
        _FIELD(HTTP_QUERY_STATUS_TEXT),
        _FIELD(HTTP_QUERY_ACCEPT),
    };

    DWORD statusCode;
    DWORD len;
    WCHAR buf[1024];
    UINT i, count;
    BOOL ok;

    HTTPQUERYINFO(
        HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,
        statusCode,
        sizeof(statusCode),
        TRUE);
    DbgPrint("Response status code: %d\n", statusCode);

    switch (testFlag) {
    case 1: {
        SYSTEMTIME systime;
        HTTPQUERYINFO(
            HTTP_QUERY_DATE|HTTP_QUERY_FLAG_SYSTEMTIME,
            systime,
            sizeof(systime),
            TRUE);

        DbgPrint("Date: %04d/%02d/%02d %d %02d:%02d:%02d\n",
            systime.wYear,
            systime.wMonth,
            systime.wDay,
            systime.wDayOfWeek,
            systime.wHour,
            systime.wMinute,
            systime.wSecond);
        }

    case 2: {
        
        i = (testFlag == 1) ?
                HTTP_QUERY_RAW_HEADERS :
                HTTP_QUERY_RAW_HEADERS_CRLF;

        len = 0;
        if (HttpQueryInfo(hRequest, i, NULL, &len, NULL) ||
            GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            WARNFAIL(HttpQueryInfo); return;
        }

        WCHAR* hdrbuf = (WCHAR*) MAlloc(len);
        if (!hdrbuf) {
            WARNFAIL(MAlloc); return;
        }

        len /= sizeof(WCHAR);
        if (!HttpQueryInfo(hRequest, i, hdrbuf, &len, NULL)) {
            WARNFAIL(HttpQueryInfo);
            Free(hdrbuf);
            return;
        }

        WCHAR* p = hdrbuf;
        if (i == HTTP_QUERY_RAW_HEADERS_CRLF) {
            while (*p) {
                if (p[0] == L'\r' && p[1] == L'\n') {
                    *p++ = L' ';
                    *p++ = 0;
                } else
                    p++;
            }
            p = hdrbuf;
        }

        while (*p) {
            DbgPrint("%ws\n", p);
            p += wcslen(p) + 1;
        }

        Free(hdrbuf);
        }
        break;

    case 3:

        count = ARRAYCOUNT(infoLevels);
        for (i=0; i < count; i++) {
            HTTPQUERYINFO(infoLevels[i].index, buf, ARRAYCOUNT(buf), FALSE);
            DbgPrint("%3s: ", infoLevels[i].name);
            if (ok) {
                DbgPrint("%ws\n", buf);
            } else {
                DbgPrint("*** error - %d\n", GetLastError());
            }
        }

        wcscpy(buf, L"Server");
        HTTPQUERYINFO(HTTP_QUERY_CUSTOM, buf, ARRAYCOUNT(buf), FALSE);
        DbgPrint("Server: ");
        if (ok) {
            DbgPrint("%ws\n", buf);
        } else {
            DbgPrint("*** error - %d\n", GetLastError());
        }
        break;
    }
}


//
// Dump the HTTP response message in response to a request
//
VOID
DumpGetResponse(
    HINTERNET hRequest
    )
{
    DumpResponseHeaders(hRequest);

    DWORD size, total = 0;

    do {
        if (!InternetReadFile(hRequest, buf, BUFSIZE, &size)) {
            WARNFAIL(InternetReadFile); return;
        }
        total += size;
    } while (size != 0);

    DbgPrint("Response data: %d bytes\n\n", total);
}


//
// HTTP GET without using a proxy server
//
VOID
GetTestDirect()
{
    HINTERNET hInternet;
    HINTERNET hRequest;

    hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        WARNFAIL(InternetOpen); return;
    }

    hRequest = InternetOpenUrl(hInternet, L"http://davidx3", NULL, 0, 0, 0);
    if (!hRequest) {
        WARNFAIL(InternetOpenUrl);
    } else {
        DumpGetResponse(hRequest);
        InternetCloseHandle(hRequest);
    }

    InternetCloseHandle(hInternet);
}


//
// HTTP GET through a proxy server
//
VOID
GetTestProxied()
{
    HINTERNET hInternet;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;

    hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PROXY, L"itgproxy", NULL, 0);
    if (!hInternet) {
        WARNFAIL(InternetOpen); return;
    }

    // Use InternetConnect/HttpOpenRequest/HttpSendRequest
    // instead of InternetOpenUrl for this test

    hConnect = InternetConnect(
                    hInternet,
                    L"www.xbox.com",
                    INTERNET_DEFAULT_HTTP_PORT,
                    NULL,
                    NULL,
                    INTERNET_SERVICE_HTTP,
                    0,
                    0);

    if (!hConnect) {
        WARNFAIL(InternetConnect); goto exit;
    }

    hRequest = HttpOpenRequest(
                    hConnect,
                    L"GET",
                    L"/xbox/flash/home.asp",
                    NULL,
                    NULL,
                    acceptTypes,
                    0,
                    0);

    if (!hRequest) {
        WARNFAIL(HttpOpenRequest); goto exit;
    }

    if (HttpSendRequest(hRequest, NULL, 0, NULL, 0)) {
        DumpGetResponse(hRequest);
    }

exit:
    if (hRequest) { InternetCloseHandle(hRequest); }
    if (hConnect) { InternetCloseHandle(hConnect); }
    InternetCloseHandle(hInternet);
}


//
// HTTP POST without using a proxy server
//
VOID
PostTestDirect()
{
    static const WCHAR extraHeaders[] =
        L"From: foo@bar.com\r\n"
        L"Content-Type: application/x-www-form-urlencoded\r\n";

    HINTERNET hInternet;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;

    hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        WARNFAIL(InternetOpen); return;
    }

    hConnect = InternetConnect(
                    hInternet,
                    L"davidx3",
                    INTERNET_DEFAULT_HTTP_PORT,
                    NULL,
                    NULL,
                    INTERNET_SERVICE_HTTP,
                    0,
                    0);

    if (!hConnect) {
        WARNFAIL(InternetConnect); goto exit;
    }

    hRequest = HttpOpenRequest(
                    hConnect,
                    L"POST",
                    L"/test",
                    NULL,
                    NULL,
                    acceptTypes,
                    0,
                    0);

    if (!hRequest) {
        WARNFAIL(HttpOpenRequest); goto exit;
    }

    // Send additional header fields
    // and send the content of a file as optional data

    if (HttpSendRequest(
            hRequest,
            extraHeaders,
            -1, 
            L"T:\\test",
            -1)) {
        DumpGetResponse(hRequest);
    }

exit:
    if (hRequest) { InternetCloseHandle(hRequest); }
    if (hConnect) { InternetCloseHandle(hConnect); }
    InternetCloseHandle(hInternet);
}


void __cdecl main()
{
    DbgPrint("Loading XBox network stack...\n");
    err = XnetInitialize(NULL, TRUE);
    if (err != NO_ERROR) {
        WARNFAIL(XnetInitialize);
        BREAK_INTO_DEBUGGER
    }

    DbgPrint("*** Test started, press 'g' to continue...\n");

    while (TRUE) {
        DbgPrint("*** To quit, press CTRL-C and type: ed %x 0;g\n", &testFlag);
        BREAK_INTO_DEBUGGER

        if (testFlag <= 0) break;
        switch (testFlag) {
        case 1:
            GetTestDirect();
            break;

        case 2:
            GetTestProxied();
            break;

        case 3:
            PostTestDirect();
            break;
        }
    }

    XnetCleanup();

    if (testFlag == 0)
        HalReturnToFirmware(HalRebootRoutine);
    else
        Sleep(INFINITE);
}

