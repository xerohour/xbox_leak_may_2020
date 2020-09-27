/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    http.cxx

Abstract:

    HTTP client functions

Revision History:

    08/08/2000 davidx
        Created it.

--*/

#include "precomp.h"
#include <stdio.h>
#include "httptime.h"

//
// Default HTTP string constants
//
const WCHAR WininetImpl::HttpDefaultVerbStr[] = L"GET";
const WCHAR WininetImpl::HttpDefaultVersionStr[] = HTTP_VERSION;
const WCHAR WininetImpl::HttpDefaultObjectStr[] = L"/";
static const WCHAR HttpSchemeStr[] = L"http://";


InternetObject*
InternetObject::Create(
    const WCHAR* userAgent,
    const WCHAR* proxyServer
    )

/*++

Routine Description:

    Instantiate a new internet object

Arguments:

    userAgent - Specifies the user agent
    proxyServer - Specifies the proxy server name

Return Value:

    Pointer to the newly created internet object
    NULL if there is an error

--*/

{
    static const WCHAR defaultUserAgent[] = L"XBox HTTP Client Library";
    InternetObject* inetobj;
    WSADATA wsadata;
    INT err;

    // Instantiate the object
    inetobj = new InternetObject();
    if (!inetobj) goto failed;

    // Startup winsock
    err = WSAStartup(WINSOCK_VERSION, &wsadata);
    if (err != NO_ERROR) {
        SetLastError(err);
        goto failed;
    }
    inetobj->wsastartup = TRUE;

    // Save user-agent string
    if (!userAgent) userAgent = defaultUserAgent;
    inetobj->userAgent = strdupWtoA(userAgent);
    if (!inetobj->userAgent) goto failed;

    // Save proxy server name and resolve its IP address
    if (proxyServer) {
        inetobj->proxyServer = strdupWtoA(proxyServer);
        if (!inetobj->proxyServer) goto failed;

        inetobj->proxyServerAddr = ResolveHostAddr(inetobj->proxyServer);
        if (inetobj->proxyServerAddr.s_addr == 0) goto failed;
    }

    return inetobj;

failed:
    WARNING_("InternetOpen failed: %d", GetLastError());
    delete inetobj;
    return NULL;
}


ConnectObject*
ConnectObject::Create(
    InternetObject* inetobj,
    const WCHAR* serverName,
    INTERNET_PORT serverPort,
    const WCHAR* username,
    const WCHAR* password,
    DWORD_PTR appContext
    )

/*++

Routine Description:

    Instantiate an HTTP connection object

Arguments:

    inetobj - Points to an InternetObject
    serverName - Server host name
    username - Username
    password - Password
    appContext - App-specific context value

Return Value:

    Pointer to the newly created connection object
    NULL if there is an error

--*/

{
    ConnectObject* connobj;

    connobj = new ConnectObject();
    if (!connobj) goto failed;

    // Keep a reference to the internet object
    inetobj->AddRef();
    connobj->inetobj = inetobj;
    connobj->appContext = appContext;
    connobj->serverPort = serverPort;

    // Save server name and resolve its IP address
    connobj->serverName = strdupWtoA(serverName);
    if (!connobj->serverName) goto failed;

    if (inetobj->GetProxyServer()) {
        connobj->serverAddr = inetobj->GetProxyServerAddr();
    } else {
        connobj->serverAddr = ResolveHostAddr(connobj->serverName);
        if (connobj->serverAddr.s_addr == 0) goto failed;
    }

    // Save username and password information
    if (username) {
        connobj->username = strdupWtoA(username);
        if (!connobj->username) goto failed;
    }

    if (password) {
        connobj->password = strdupWtoA(password);
        if (!connobj->password) goto failed;
    }

    // Successful return
    return connobj;

failed:
    WARNING_("InternetConnect failed: %d", GetLastError());
    delete connobj;
    return NULL;
}


RequestObject*
RequestObject::Create(
    ConnectObject* connobj,
    const WCHAR* verb,
    const WCHAR* objectName,
    const WCHAR* httpVer,
    const WCHAR* referer OPTIONAL,
    const WCHAR* acceptTypes[] OPTIONAL,
    DWORD_PTR appContext
    )

/*++

Routine Description:

    Instantiate a new HTTP request object

Arguments:

    connobj - Points to the HTTP connection object
    verb - Verb for the HTTP request
    objectName - Name of the object in question
    httpVer - HTTP version string
    referer - Referer URL
    acceptTypes - Media types accepted by the client
    appContext - App-specific context value

Return Value:

    Pointer to the newly created HTTP request object
    NULL if there is an error

--*/

{
    RequestObject* reqobj;

    reqobj = new RequestObject();
    if (!reqobj) return NULL;

    // Keep a reference to the parent connection object
    connobj->AddRef();
    reqobj->connobj = connobj;
    reqobj->appContext = appContext;

    //
    // Assemble default HTTP request headers
    //
    InternetObject* inetobj = connobj->GetInternetObject();
    HdrBuf* hdrbuf = &reqobj->reqhdrs;
    INT ok;

    // Request-line
    ok = hdrbuf->AppendUnicodeString(verb, SPACE);
    if (inetobj->GetProxyServer()) {
        ok &= hdrbuf->AppendUnicodeString(HttpSchemeStr, 0);
        ok &= hdrbuf->AppendAsciiString(connobj->GetServerName(), 0);
    }
    ok &= hdrbuf->AppendUnicodeString(objectName, SPACE);
    ok &= hdrbuf->AppendUnicodeString(httpVer, LF);

    // User-agent
    ok &= hdrbuf->AppendAsciiString("User-Agent", COLON);
    ok &= hdrbuf->AppendAsciiString(inetobj->GetUserAgent(), LF);

    // Accept
    //  note: this is actually an HTTP/1.1 header field
    if (acceptTypes && *acceptTypes) {
        const WCHAR* lasttype = *acceptTypes++;
        ok &= hdrbuf->AppendAsciiString("Accept", COLON);
        while (*acceptTypes) {
            ok &= hdrbuf->AppendUnicodeString(lasttype, COMMA);
            lasttype = *acceptTypes++;
        }
        ok &= hdrbuf->AppendUnicodeString(lasttype, LF);
    }

    // Referer
    if (referer) {
        ok &= hdrbuf->AppendAsciiString("Referer", COLON);
        ok &= hdrbuf->AppendUnicodeString(referer, LF);
    }

    if (!ok) {
        WARNING_("Failed to assembly default HTTP request headers");
        delete reqobj;
        return NULL;
    }
    return reqobj;
}


BOOL
RequestObject::Connect()

/*++

Routine Description:

    Establish a TCP connection to the HTTP server

Arguments:

    NONE

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    struct sockaddr_in serveraddr;

    // Do nothing if already connected
    if (IsConnected()) return TRUE;

    // Create the socket and connect to the HTTP server
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        return FALSE;

    connobj->GetServerAddr(&serveraddr);
    if (_connect(sock, &serveraddr) != NO_ERROR) {
        Disconnect();
        return FALSE;
    }

    // NOTE: we could set receive timeout option here...
    return TRUE;
}


static BOOL
DoSendData(
    SOCKET s,
    WSABUF* bufs,
    UINT bufcnt
    )

/*++

Routine Description:

    Send data out of a TCP connection

Arguments:

    s - Socket handle
    bufs - Data buffers
    bufcnt - Number of data buffers

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    INT err;
    DWORD sent;

    while (bufcnt) {
        err = WSASend(s, bufs, bufcnt, &sent, 0, NULL, NULL);
        if (err != NO_ERROR) return FALSE;

        // Only partial amount of data was sent:
        //  we need to update the send buffers and 
        //  then call WSASend again
        while (sent) {
            DWORD n = min(sent, bufs->len);

            if ((bufs->len -= n) == 0) {
                bufs++, bufcnt--;
            } else
                bufs->buf += n;
            sent -= n;
        }
    }

    return TRUE;
}


static HANDLE
DoOpenFile(
    const WCHAR* filename,
    UINT* fileSize
    )

/*++

Routine Description:

    Open a file for reading and get the file size

Arguments:

    filename - Specifies the filename
    fileSize - Return the file size

Return Value:

    Handle to the open file
    INVALID_HANDLE_VALUE if there is an error

--*/

{
    // BUGBUG
    // Since CreateFileW API is gone, we need to strip down
    // the Unicode string to ANSI string. Eventually we should
    // change the HttpSendRequest API to have it pass in
    // ANSI string.

    XDBGWRN("HTTP",
        "HttpSendRequest: lpOptional parameter is treated as Unicode filename - %ws",
        filename);
    
    CHAR *p, buf[MAX_PATH];
    ASSERT(wcslen(filename) < MAX_PATH);

    p = buf;
    while ((*p++ = (CHAR) *filename++) != 0)
        ;

    HANDLE file;

    //
    // Open the file for reading
    //
    file = CreateFileA(
                buf,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,
                NULL);

    if (file != INVALID_HANDLE_VALUE) {
        //
        // Get file size
        //
        *fileSize = GetFileSize(file, NULL);
        if (*fileSize == 0xffffffff) {
            CloseHandle(file);
            file = INVALID_HANDLE_VALUE;
        }
    }

    return file;
}


static BOOL
DoSendFile(
    SOCKET s,
    HANDLE file,
    UINT filesize
    )

/*++

Routine Description:

    Send the content of a file out of a TCP connection

Arguments:

    s - Specifies the socket handle
    file - Specifies the open file handle
    filesize - Total file size

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

#define SENDFILE_BLKSIZE 4096

{
    CHAR* buf = NULL;
    WSABUF wsabuf;
    UINT bufsize;
    
    bufsize = min(filesize, SENDFILE_BLKSIZE);
    buf = (CHAR*) MAlloc(bufsize);
    if (!buf) return FALSE;

    while (filesize) {
        DWORD count = min(filesize, bufsize);
        DWORD bytesRead;

        // Read the next chunk of data from the file
        if (!ReadFile(file, buf, count, &bytesRead, NULL) ||
            count != bytesRead)
            break;
        
        // Send it out
        wsabuf.buf = buf;
        wsabuf.len = count;
        if (!DoSendData(s, &wsabuf, 1)) break;

        filesize -= count;
    }

    Free(buf);
    return (filesize == 0);
}


BOOL
RequestObject::SendRequest(
    const WCHAR* headers,
    UINT headerLength,
    const VOID* optionalData,
    UINT optionalLength
    )

/*++

Routine Description:

    Send the request to the HTTP server

Arguments:

    headers - Points to extra HTTP request headers
    headerLength - Extra header length
    optionalData - Points to optional data sent along the HTTP request
    optionalLength - Optional data length

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    // Establish a TCP connection to the server
    if (!Connect()) return FALSE;

    // Free any existing response information
    ResetRespInfo();

    HdrBuf hdrend;
    CHAR* extrahdrs = NULL;
    INT ok = 1;
    HANDLE file = INVALID_HANDLE_VALUE;

    //
    // Artificial loop for error handling
    //
    do {
        CHAR lenstr[16];

        // Content-Length header field
        if (optionalLength == 0xffffffff) {
            file = DoOpenFile((const WCHAR*) optionalData, &optionalLength);
            if (file == INVALID_HANDLE_VALUE) {
                WARNING_("Couldn't open file: %ws", optionalData);
                ok = 0;
                break;
            }
        }

        ok &= hdrend.AppendAsciiString("Content-Length", COLON);
        sprintf(lenstr, "%u", optionalLength);
        ok &= hdrend.AppendAsciiString(lenstr, LF);

        // Empty line
        ok &= hdrend.AppendAsciiString("", LF);
        if (!ok) break;

        // Assemble send buffers
        WSABUF bufs[4];
        UINT bufcnt = 0;

        bufs[bufcnt].len = reqhdrs.size;
        bufs[bufcnt++].buf = (CHAR*) reqhdrs.data;

        if (headerLength == 0xffffffff) {
            headerLength = wcslen(headers);
        }

        if (headerLength) {
            extrahdrs = strdupWtoA(headers, headerLength);
            if (!extrahdrs) break;

            bufs[bufcnt].len = headerLength;
            bufs[bufcnt++].buf = extrahdrs;
        }

        bufs[bufcnt].len = hdrend.size;
        bufs[bufcnt++].buf = (CHAR*) hdrend.data;

        if (optionalLength && file == INVALID_HANDLE_VALUE) {
            bufs[bufcnt].len = optionalLength;
            bufs[bufcnt++].buf = (CHAR*) optionalData;
        }

        // Send request data
        //  note: we need to use a wrapper function here
        //  and cann't call WSASend directly because
        //  WSASend can return success only after
        //  sending partial amount of data.
        ok = DoSendData(sock, bufs, bufcnt) &&
             (file == INVALID_HANDLE_VALUE ||
              DoSendFile(sock, file, optionalLength));
    } while (FALSE);

    if (file == INVALID_HANDLE_VALUE) {
        CloseHandle(file);
    }
    Free(extrahdrs);

    if (!ok) {
        Disconnect();
    }
    return ok;
}


BOOL
RequestObject::QueryDataAvailable(
    DWORD* bytesAvailable
    )

/*++

Routine Description:

    Query the amount of HTTP response data available

Arguments:

    bytesAvailable - Returns the number of bytes available

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    //
    // Make sure we're connected and
    // we have already read the HTTP response headers
    //
    if (!ReadRespHdrs(FALSE)) return FALSE;

    // We didn't finish reading the response headers
    if (!IsRespHdrsOk()) {
        *bytesAvailable = 0;
        return TRUE;
    }

    // Check if there is any data to be read
    ULONG avail;
    INT err = ioctlsocket(sock, FIONREAD, &avail);

    if (err != NO_ERROR) return FALSE;
    *bytesAvailable = peeklen + avail;
    if (*bytesAvailable)
        return TRUE;

    // If there is no data, check if the connection is closed
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    struct timeval timeout = { 0, 0 };

    err = select(1, &fds, NULL, NULL, &timeout);
    switch (err) {
    case 0:
        break;

    case SOCKET_ERROR:
        return FALSE;
    
    default:
        ASSERT(err == 1);
        err = ioctlsocket(sock, FIONREAD, &avail);
        if (err != NO_ERROR) return FALSE;

        // Connection has been closed
        if (avail == 0) {
            SetLastError(ERROR_HANDLE_EOF);
            return FALSE;
        }

        *bytesAvailable = avail;
        break;
    }

    return TRUE;
}


BOOL
RequestObject::ReadData(
    CHAR* buffer,
    UINT bufferSize,
    DWORD* bytesRead
    )

/*++

Routine Description:

    Read HTTP response data

Arguments:

    buffer - Output data buffer
    bufferSize - Output buffer size
    bytesRead - Returns the number of bytes actually read

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    *bytesRead = 0;

    //
    // Make sure we're connected and
    // we have already read the HTTP response headers
    //
    if (!ReadRespHdrs(TRUE)) return FALSE;

    //
    // If we have read too much data while parsing
    // the response headers, return it here.
    //
    UINT n;
    if (peeklen) {
        n = min(bufferSize, peeklen);
        CopyMem(buffer, peekdata, n);
        buffer += n;
        peekdata += n;
        peeklen -= n;

        *bytesRead += n;
        readcnt += n;
        bufferSize -= n;
    }

    while (bufferSize) {
        n = bufferSize;
        if (contentlen != 0xffffffff) {
            //
            // If Content-Length header field is not present,
            // we'll keep on reading until the server closes
            // the connection.
            //
            // Otherwise, we only read the specified amount of
            // data (and ignore any extra data at the end).
            //
            if (readcnt >= contentlen) break;
            if (n > contentlen - readcnt)
                n = contentlen - readcnt;
        }

        INT count = recv(sock, buffer, n, 0);
        if (count < 0) return FALSE;

        // Stop if connection is closed
        if (count == 0) break;

        n = count;
        *bytesRead += n;
        readcnt += n;
        buffer += n;
        bufferSize -= n;
    }

    return TRUE;
}


BOOL
RequestObject::ReadRespHdrs(
    BOOL wait
    )

/*++

Routine Description:

    Read HTTP response headers

Arguments:

    wait - Whether to wait for the header data

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    //
    // Check if we're currently connected
    //
    if (!IsConnected()) {
        SetLastError(ERROR_NOT_CONNECTED);
        return FALSE;
    }

    //
    // Check if we have already read the response headers
    //
    if (IsRespHdrsOk()) return TRUE;

    do {
        //
        // Reallocate memory buffer if necessary
        //
        if (resphdrs.SpaceLeft() < 512) {
            BYTE* olddata = resphdrs.data;
            if (!resphdrs.ReserveSpace(512)) goto failed;

            UINT offset = resphdrs.data - olddata;
            peekdata += offset;
            for (UINT i=0; i < resphdrLinecnt; i++)
                resphdrLines[i] += offset;
        }
        
        UINT n = resphdrs.SpaceLeft();
        INT result;
        if (!wait) {
            ULONG avail;
            result = ioctlsocket(sock, FIONREAD, &avail);
            if (result == SOCKET_ERROR) goto failed;
            if (avail == 0) break;
            if (n > avail) n = avail;
        }

        result = recv(sock, (CHAR*) resphdrs.data + resphdrs.size, n, 0);
        if (result == SOCKET_ERROR) goto failed;

        resphdrs.size += result;
        ParseRespHdrs(result == 0);
        if (result == 0) {
            //
            // Server has closed the connection
            //
            if (peeklen != 0) goto failed;
            resphdrsOk = TRUE;
        }
    } while (!IsRespHdrsOk());
    return TRUE;

failed:
    Disconnect();
    WARNING_("Failed to read HTTP response headers");
    SetLastError(ERROR_INVALID_DATA);
    return FALSE;
}


VOID
RequestObject::ParseRespHdrs(
    BOOL eof
    )

/*++

Routine Description:

    Parse HTTP response header fields

Arguments:

    eof - Whether the server connection has been closed

Return Value:

    NONE

--*/

#define IsLWS(c) ((c) == SPACE || (c) == TAB)

{
    BYTE* start = peekdata;
    BYTE* end = resphdrs.data + (resphdrs.size-1);

    while (TRUE) {
        //
        // Find the end of the next line
        //
        BYTE* p = start;
        BYTE* q;

    findeol:
        while (p < end) {
            if (p[0] == CR || p[1] == LF) break;
            p++;
        }
        if (p >= end) break;

        if (p == start) {
            //
            // Empty line - end of header section
            //
            start += 2;
            resphdrsOk = TRUE;
            break;
        }

        //
        // Handle line continuations
        //
        q = p + 2;
        if (q > end) {
            if (!eof) break;
        } else if (IsLWS(*q)) {
            do {
                q++;
            } while (q <= end && IsLWS(*q));
            if (q > end) break;

            UINT movecnt = (end-q) + 1;
            *p++ = SPACE;
            MoveMem(p, q, movecnt);

            end -= (q - p);
            goto findeol;
        }

        //
        // Strip trailing whitespaces
        //
        do {
            *p-- = 0;
        } while (p >= start && IsLWS(*p));

        if (resphdrLinecnt < MAXRESPHDRS) {
            resphdrLines[resphdrLinecnt++] = start;
        } else {
            WARNING_("Too many response header fields");
        }

        start = q;

        //
        // Look for Content-Length: field
        //
        CHAR* str;
        if (MatchHeaderField(start, "Content-Length", &str)) {
            NTSTATUS status;
            ULONG val;
            status = RtlCharToInteger(str, 10, &val);
            if (NT_SUCCESS(status)) contentlen = val;
        }
    }

    peekdata = start;
    peeklen = (end - start) + 1;
}


BOOL
RequestObject::MatchHeaderField(
    BYTE* data,
    const CHAR* fieldname,
    CHAR** fieldval
    )

/*++

Routine Description:

    Match a specified header field

Arguments:

    data - Points to the header field data
    fieldname - Specifies the name of the interested field
    fieldval - Return a pointer to the field value string

Return Value:

    TRUE if the name of the field matches the specified name
    FALSE otherwise

--*/

{
    INT len = strlen(fieldname);

    if (_strnicmp((CHAR*) data, fieldname, len) != 0 || data[len] != COLON)
        return FALSE;

    data += (len+1);
    while (*data && IsLWS(*data)) data++;
    *fieldval = (CHAR*) data;
    return TRUE;
}


BOOL
RequestObject::QueryRespInfo(
    DWORD infoLevel,
    WCHAR* buffer,
    DWORD* buflen,
    DWORD* hdrindex
    )

/*++

Routine Description:

    Retrieves header information associated with an HTTP request

Arguments:

    infoLevel - Specifies what attribute to retrieve
    buffer - Output data buffer
    buflen - Output data buffer size
        On entry, it contains the buffer size in number of WCHARs
        On return, it contains either the actual output data in number
        of WCHARs (not including the null terminator) or the actual
        number of bytes needed if the output buffer is too small
    hdrindex - 0-based index specifying which occurrence if of interest

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

#define HTTP_QUERY_FLAG_ALL \
        (HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_FLAG_SYSTEMTIME)

#define ReturnInsufficientBufferError(_bytesNeeded) { \
            *buflen = (_bytesNeeded); \
            SetLastError(ERROR_INSUFFICIENT_BUFFER); \
            return FALSE; \
        }

{
    static const struct {
        DWORD fieldindex;
        const CHAR* fieldname;
    } mapping[] = {
        { HTTP_QUERY_CONTENT_LENGTH, "Content-Length" },
        { HTTP_QUERY_CONTENT_TYPE, "Content-Type" },
        { HTTP_QUERY_DATE, "Date" },
        { HTTP_QUERY_EXPIRES, "Expires" },
        { HTTP_QUERY_LAST_MODIFIED, "Last-Modified" },
        { HTTP_QUERY_ACCEPT, "Accept" },
    };

    DWORD modifier = infoLevel & 0xffff0000;
    DWORD fieldIndex = infoLevel & 0xffff;
    CHAR tempbuf[64];
    const CHAR* fieldname;
    CHAR* fieldval = NULL;
    UINT i, count;
    DWORD occurrence = hdrindex ? *hdrindex : 0;

    //
    // Make sure we're connected and
    // we have already read the HTTP response headers
    //
    if (!ReadRespHdrs(TRUE)) return FALSE;

    if (modifier & ~HTTP_QUERY_FLAG_ALL)
        goto unsupported;

    switch (fieldIndex) {
    case HTTP_QUERY_VERSION:
    case HTTP_QUERY_STATUS_CODE:
    case HTTP_QUERY_STATUS_TEXT:
        //
        // Extract information from the status line
        //
        if (occurrence) goto unsupported;
        fieldval = ParseStatusLine(tempbuf, sizeof(tempbuf), fieldIndex);
        break;

    case HTTP_QUERY_RAW_HEADERS:
    case HTTP_QUERY_RAW_HEADERS_CRLF:
        //
        // Return all the header fields
        //
        if (modifier || occurrence) goto unsupported;
        return ReturnAllRespHdrs(buffer, buflen, fieldIndex);

    default:
        if (fieldIndex == HTTP_QUERY_CUSTOM) {
            //
            // Arbitrary field name
            //
            count = wcslen(buffer);
            if (count >= sizeof(tempbuf)) goto unsupported;
            strcpyWtoA(tempbuf, buffer);
            fieldname = tempbuf;
        } else {
            //
            // Map field index to field name
            //
            count = ARRAYCOUNT(mapping);
            for (i=0; i < count; i++) {
                if (fieldIndex == mapping[i].fieldindex) break;
            }

            if (i == count) goto unsupported;
            fieldname = mapping[i].fieldname;
        }

        //
        // Find the field with the specified name
        //
        for (i=count=0; i < resphdrLinecnt; i++) {
            if (MatchHeaderField(resphdrLines[i], fieldname, &fieldval)) {
                if (count == occurrence) break;
                count++;
            }
        }

        if (i == resphdrLinecnt)
            fieldval = NULL;
        break;
    }

    //
    // Check if the specified field is present
    //
    if (fieldval == NULL) {
        SetLastError(ERROR_NO_DATA);
        return FALSE;
    }

    if (modifier & HTTP_QUERY_FLAG_NUMBER) {
        //
        // Return the field value as an integer
        // NOTE: This is really confusing at the API level:
        //  should app pass in buffer length in # of WCHARs
        //  or should it be in # of bytes?
        if (*buflen < sizeof(DWORD)) {
            ReturnInsufficientBufferError(sizeof(DWORD));
        }

        NTSTATUS status;
        ULONG val;

        status = RtlCharToInteger(fieldval, 10, &val);
        if (!NT_SUCCESS(status)) {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }

        *((DWORD*) buffer) = val;
    } else if (modifier & HTTP_QUERY_FLAG_SYSTEMTIME) {
        //
        // Parse the HTTP date/time string
        //
        if (*buflen < sizeof(SYSTEMTIME)) {
            ReturnInsufficientBufferError(sizeof(SYSTEMTIME));
        }

        if (!HttpDateTime::Parse(fieldval, (SYSTEMTIME*) buffer))
            return FALSE;
    } else {
        //
        // Return the field value as Unicode string.
        // Check if the caller's buffer is large enough
        //
        count = strlen(fieldval);
        if (*buflen <= count) {
            ReturnInsufficientBufferError((count + 1) * sizeof(WCHAR));
        }

        *buflen = count;
        strcpyAtoW(buffer, fieldval);
    }

    if (hdrindex) *hdrindex = ++occurrence;
    return TRUE;

unsupported:
    WARNING_("HttpQueryInfo: unsupported parameter", infoLevel);
    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}


CHAR*
RequestObject::ParseStatusLine(
    CHAR* buf,
    DWORD buflen,
    DWORD field
    )

/*++

Routine Description:

    Extract the specified field from the HTTP response status line

Arguments:

    buf - Points to the output buffer
    buflen - Output buffer size
        These two parameters are only used for the version
        and status code fields.
    field - Specifies which field the caller is interested in

Return Value:

    Pointer to the string value for the specified field

--*/

{
    if (resphdrLinecnt == 0) return NULL;

    field = (field == HTTP_QUERY_VERSION) ? 0 :
            (field == HTTP_QUERY_STATUS_CODE) ? 1 : 2;

    CHAR* q = (CHAR*) resphdrLines[0];
    CHAR* p = NULL;
    DWORD i, n;

    for (i=n=0; i <= field; i++) {
        p = q;
        if (i < 2) {
            while (*q && !IsLWS(*q)) q++;
            n = q - p;
            while (*q && IsLWS(*q)) q++;
        } else {
            n = strlen(q);
            q += n;
        }
    }

    //
    // The request field is not present
    //
    if (n == 0) return NULL;

    //
    // For the status text field, return
    // a pointer to our internal data buffer
    //
    if (field == 2) return p;

    //
    // Copy version and status code value into the output buffer
    //
    if (buflen <= n) return NULL;
    CopyMem(buf, p, n);
    buf[n] = 0;
    return buf;
}


BOOL
RequestObject::ReturnAllRespHdrs(
    WCHAR* buf,
    DWORD* buflen,
    DWORD field
    )

/*++

Routine Description:

    Return all the HTTP response headers in one chunk

Arguments:

    buf - Points to the output buffer
    buflen - See comments for QueryRespInfo
    field - Specifies whether the header fields should be separated
        by the null character or the CRLF

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    if (resphdrLinecnt == 0) {
        SetLastError(ERROR_NO_DATA);
        return FALSE;
    }

    //
    // Figure out the size of output buffer we need
    //

    BOOL crlf = (field == HTTP_QUERY_RAW_HEADERS_CRLF);
    UINT i, count;

    for (i=count=0; i < resphdrLinecnt; i++)
        count += strlen((CHAR*) resphdrLines[i]);

    count += resphdrLinecnt * (crlf ? 2 : 1);
    if (*buflen <= count) {
        ReturnInsufficientBufferError((count + 1) * sizeof(WCHAR));
    }

    //
    // Copy all the header fields to the output buffer
    //

    *buflen = count;
    for (i=0; i < resphdrLinecnt; i++) {
        CHAR* str = (CHAR*) resphdrLines[i];
        strcpyAtoW(buf, str);
        count = strlen(str);
        buf += count;
        if (crlf) {
            *buf++ = CR;
            *buf++ = LF;
        } else
            *buf++ = 0;
    }

    *buf = 0;
    return TRUE;
}


BOOL
WininetImpl::HttpParseUrl(
    const WCHAR* url,
    WCHAR** serverName,
    INTERNET_PORT* serverPort,
    WCHAR** objectName
    )

/*++

Routine Description:

    Crack an HTTP URL to extract various parts:
        serverName
        serverPort
        objectName

Arguments:

    url - Specifies the input URL string
    serverName - Returns a pointer to the server name string
    serverPort - Returns the server port number
    objectName - Returns a pointer to the object name string

Return Value:

    TRUE if successful, FALSE if there is an error

--*/

{
    ASSERT(url != NULL);
    *serverName = NULL;
    *objectName = NULL;
    *serverPort = INTERNET_DEFAULT_HTTP_PORT;

    //
    // Artificial loop for error handling
    //
    while (TRUE) {
        //
        // We only support http:// scheme
        //
        UINT len = wcslen(HttpSchemeStr);
        if (_wcsnicmp(url, HttpSchemeStr, len) != 0) break;

        //
        // Parse the server name
        //
        const WCHAR* server = url + len;
        const WCHAR* cp = server;

        while (*cp && *cp != COLON && *cp != SLASH) cp++;
        if ((len = cp - server) == 0) break;
        
        WCHAR* p = (WCHAR*) MAlloc((len+1) * sizeof(WCHAR));
        if (!p) break;

        CopyMem(p, server, len*sizeof(WCHAR));
        p[len] = 0;
        *serverName = p;

        //
        // Parse the server port number
        //
        if (*cp == COLON) {
            const WCHAR* port = ++cp;
            while (*cp && *cp != SLASH) cp++;
            if ((len = cp - port) == 0) break;

            UNICODE_STRING ustr;
            NTSTATUS status;
            ULONG val;

            ustr.Buffer = (WCHAR*) port;
            ustr.Length = (USHORT) (len * sizeof(WCHAR));
            ustr.MaximumLength = (USHORT) (ustr.Length + sizeof(WCHAR));
            status = RtlUnicodeStringToInteger(&ustr, 0, &val);

            if (!NT_SUCCESS(status) || val > 0xffff) break;
            *serverPort = (INTERNET_PORT) val;
        }

        //
        // Parse the object name
        //
        if (*cp == SLASH) {
            len = (wcslen(cp) + 1) * sizeof(WCHAR);
            p = (WCHAR*) MAlloc(len);
            if (!p) break;

            *objectName = p;
            CopyMem(p, cp, len);
        }
        return TRUE;
    }

    WARNING_("HttpParseUrl failed: %ws", url);
    SetLastError(ERROR_INVALID_PARAMETER);
    Free(*serverName);
    *serverName = NULL;
    return FALSE;
}


//
// Semi-klugy way to determine if a hostname
// string is in dotted-decimal form
//
inline BOOL IsHostAddrString(const CHAR* hostname) {
    const CHAR* p = hostname;
    while (*p) {
        if ((*p < '0' || *p > '9') && *p != '.')
            return FALSE;
        p++;
    }
    return TRUE;
}

struct in_addr
WininetImpl::ResolveHostAddr(
    const CHAR* hostname
    )

/*++

Routine Description:

    Resolve a host name to its IP address

Arguments:

    hostname - Points to the hostname string

Return Value:

    IP address of the specified host
    0 if there is an error

--*/

{
    struct in_addr hostaddr;

    hostaddr.s_addr = 0;
    if (IsHostAddrString(hostname)) {
        //
        // The hostname is in dotted-decimal form:
        //  just convert it to IP address directly
        //
        LONG addr = inet_addr(hostname);
        if (addr != INADDR_NONE)
            hostaddr.s_addr = addr;
    } else {
        //
        // Use DNS to map hostname to IP address
        //
        struct hostent* hostent;
        LONG* paddr;

        hostent = gethostbyname(hostname);
        if (hostent) {
            ASSERT(hostent->h_addr_list);
            paddr = (LONG*) hostent->h_addr_list[0];
            if (paddr) hostaddr.s_addr = *paddr;
        }
    }

    return hostaddr;
}

