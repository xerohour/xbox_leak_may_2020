/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    wininetp.h

Abstract:

    Private header file for lightweight HTTP client implementation

Revision History:

    08/08/2000 davidx
        Created it.

--*/

#ifndef _WININETP_H
#define _WININETP_H


//
// Header information common to all handle types
//
class BaseObject {

protected:

    LONG lock;
        // Object lock

    #define OBJLOCK_CLOSED  '-NIH'
    #define OBJLOCK_ACTIVE  '+NIH'
    #define OBJLOCK_BUSY    '*NIH'

    LONG refcount;
        // reference count

    INT type;
        // Object type

    #define OBJTYPE_NONE        0
    #define OBJTYPE_INTERNET    1
    #define OBJTYPE_CONNECT     2
    #define OBJTYPE_REQUEST     3

protected:
    
    BaseObject(INT type) {
        lock = OBJLOCK_ACTIVE;
        this->type = type;
        refcount = 1;
    }

    virtual ~BaseObject() {
        // Mark the object as freed for simple protection
        // against apps that reuse a closed handle.
        lock = OBJLOCK_CLOSED;
    }

public:

    //
    // Lock an object handle
    //
    static BaseObject* Lock(HINTERNET handle, INT type) {
        BaseObject* obj = (BaseObject*) handle;
        INT err = ERROR_INVALID_HANDLE;
        LONG lock;

        // NOTE: We don't allow multiple threads to access the same
        // handle simultaneously. If the app tries to do that, the first
        // thread will succeed and other threads will get an error return.

        if (obj) {
            lock = InterlockedCompareExchange(&obj->lock, OBJLOCK_BUSY, OBJLOCK_ACTIVE);
            if (lock == OBJLOCK_ACTIVE) {
                if (obj->type == type || type == OBJTYPE_NONE) return obj;
                obj->Unlock();
            }

            if (lock == OBJLOCK_BUSY)
                err = ERROR_BUSY;
        }

        WARNING_("Invalid Wininet handle: %x, %d", obj, err);
        SetLastError(err);
        return NULL;
    }

    //
    // Unlock an object
    //
    VOID Unlock() {
        // NOTE: we assume this is an atomic operation
        lock = OBJLOCK_ACTIVE;
    }

    //
    // Increment / decrement the reference count of a handle
    //
    LONG AddRef() {
        return InterlockedIncrement(&refcount);
    }

    LONG Release() {
        LONG count = InterlockedDecrement(&refcount);
        if (count == 0) delete this;
        return count;
    }
};


//
// Object returned by InternetOpen call (OBJTYPE_INTERNET)
//
class InternetObject : public BaseObject {

public:
    // Instantiation
    static InternetObject*
    Create(
        const WCHAR* userAgent,
        const WCHAR* proxyServer
        );

    CHAR* GetUserAgent() { return userAgent; }
    CHAR* GetProxyServer() { return proxyServer; }
    struct in_addr GetProxyServerAddr() { return proxyServerAddr; }

protected:

    InternetObject() : BaseObject(OBJTYPE_INTERNET) {
        userAgent = proxyServer = NULL;
        wsastartup = FALSE;
    }

    ~InternetObject() {
        Free(userAgent);
        Free(proxyServer);
        if (wsastartup) {
            WSACleanup();
        }
    }

    CHAR* userAgent;
        // User agent

    CHAR* proxyServer;
        // Pointer to the proxy server name
        // NULL if we're not using proxy

    struct in_addr proxyServerAddr;
        // Proxy server IP address (0 if no proxy is used)

    BOOL wsastartup;
        // Whether we called WSAStartup successfully
};


//
// Special separators
//
enum {
    CR = 0xd,
    LF = 0xa,
    TAB = '\t',
    SPACE = ' ',
    COMMA = ',',
    SLASH = '/',
    COLON = ':',
    DASH = '-'
};


//
// Buffers for holding HTTP request/response header information
//
class HdrBuf {

public:

    UINT size;
        // Number of bytes currently in the buffer

    UINT maxsize;
        // Max buffer size

    BYTE* data;
        // Pointer to data buffer

    //
    // Constructor / destructor
    //
    HdrBuf() {
        size = maxsize = 0;
        data = NULL;
    }
    ~HdrBuf() { Dispose(); }

    //
    // Amount of space left in this buffer
    //
    UINT SpaceLeft() { return (maxsize-size); }

    //
    // Reserve additional space in the buffer
    //
    BOOL ReserveSpace(UINT bytesNeeded) {
        return (bytesNeeded <= SpaceLeft()) ||
               Alloc(size + bytesNeeded);
    }

    //
    // Append data to the end of this buffer
    //
    INT Append(const VOID* buf, UINT buflen) {
        // If the data buffer is too small, grow it first
        if (!ReserveSpace(buflen)) return 0;

        CopyMem(data+size, buf, buflen);
        size += buflen;
        return 1;
    }

    //
    // Append ASCII string to the buffer
    //
    INT AppendAsciiString(const CHAR* str, CHAR sep) {
        UINT len = strlen(str);

        if (!ReserveSpace(len+2)) return 0;
        CopyMem(data+size, str, len);
        size += len;

        AppendSeparator(sep);
        return 1;
    }

    //
    // Append Unicode string to the buffer
    //
    INT AppendUnicodeString(const WCHAR* str, CHAR sep) {
        UINT len = wcslen(str);

        if (!ReserveSpace(len+2)) return 0;

        BYTE* p = data + size;
        size += len;
        while (len--)
            *p++ = (BYTE) *str++;
        
        AppendSeparator(sep);
        return 1;
    }

    //
    // Free the data buffer
    //
    VOID Dispose() {
        Free(data);
        data = NULL;
        size = maxsize = 0;
    }

private:

    enum { ALLOCINCR = 1024 };
        // Grow the buffer in 1KB blocks

    //
    // Allocate data memory for this buffer
    //
    BOOL Alloc(UINT newMaxsize) {
        // Check if the buffer is already big enough
        if (maxsize >= newMaxsize) return TRUE;

        // Allocate new data buffer
        newMaxsize = (newMaxsize+ALLOCINCR-1) & ~(ALLOCINCR-1);
        BYTE* newData = (BYTE*) MAlloc(newMaxsize);
        if (!newData) return FALSE;

        // Copy the content of the existing data buffer
        if (size) {
            CopyMem(newData, data, size);
        }
        Free(data);
        data = newData;
        maxsize = newMaxsize;
        return TRUE;
    }

    //
    // Append separator to the buffer
    //  The buffer has at least 2 bytes of available space
    //
    VOID AppendSeparator(CHAR sep) {
        switch (sep) {
        case 0:
            break;
        case LF:
            data[size++] = CR;
            data[size++] = LF;
            break;
        case COMMA:
            data[size++] = COMMA;
            data[size++] = SPACE;
            break;
        case COLON:
            data[size++] = COLON;
            data[size++] = SPACE;
            break;
        default:
            data[size++] = sep;
            break;
        }
    }
};


//
// Object returned by InternetConnect call (OBJTYPE_CONNECT)
//
class ConnectObject : public BaseObject {
public:

    // Instantiate a new HTTP connection object
    static ConnectObject*
    Create(
        InternetObject* inetobj,
        const WCHAR* serverName,
        INTERNET_PORT serverPort,
        const WCHAR* username,
        const WCHAR* password,
        DWORD_PTR appContext
        );

    // Return the server address and port number
    // for this connection object
    VOID GetServerAddr(struct sockaddr_in* serveraddr) {
        ZeroMem(serveraddr, sizeof(*serveraddr));
        serveraddr->sin_family = AF_INET;
        serveraddr->sin_port = HTONS(serverPort);
        serveraddr->sin_addr = serverAddr;
    }

    CHAR* GetServerName() { return serverName; }
    CHAR* GetUserName() { return username; }
    InternetObject* GetInternetObject() { return inetobj; }

protected:

    ConnectObject() : BaseObject(OBJTYPE_CONNECT) {
        inetobj = NULL;
        serverName = username = password = NULL;
    }

    ~ConnectObject() {
        if (inetobj) inetobj->Release();
        Free(serverName);
        Free(username);
        Free(password);
    }

    InternetObject* inetobj;
        // Reference to the internet object

    CHAR* serverName;
    struct in_addr serverAddr;
    INTERNET_PORT serverPort;
        // Server host name and IP address and port number
    
    CHAR* username;
    CHAR* password;
        // Username and password
    
    DWORD_PTR appContext;
        // App-specific context value
};


//
// Object returned by InternetOpenUrl and InternetOpenRequest calls (OBJTYPE_REQUEST)
//
class RequestObject : public BaseObject {

public:

    // Instantiate a new HTTP request object
    static RequestObject*
    Create(
        ConnectObject* connobj,
        const WCHAR* verb,
        const WCHAR* objectName,
        const WCHAR* httpVer,
        const WCHAR* referer,
        const WCHAR* acceptTypes[],
        DWORD_PTR appContext
        );

    // Send out the HTTP request
    BOOL
    SendRequest(
        const WCHAR* headers,
        UINT headerLength,
        const VOID* optionalData,
        UINT optionalLength
        );

    // Query response header information
    BOOL
    QueryRespInfo(
        DWORD infoLevel,
        WCHAR* buffer,
        DWORD* buflen,
        DWORD* hdrindex
        );

    // Query the amount of HTTP response data available
    BOOL QueryDataAvailable(DWORD* bytesAvailable);

    // Read HTTP response data
    BOOL ReadData(CHAR* buffer, UINT bufferSize, DWORD* bytesRead);

protected:

    RequestObject() : BaseObject(OBJTYPE_REQUEST) {
        connobj = NULL;
        sock = INVALID_SOCKET;
    }

    ~RequestObject() {
        Disconnect();
        if (connobj) connobj->Release();
    }

    BOOL IsConnected() { return (sock != INVALID_SOCKET); }
    BOOL Connect();
    VOID Disconnect() {
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
            sock = INVALID_SOCKET;
        }
        ResetRespInfo();
    }

    VOID ResetRespInfo() {
        resphdrs.Dispose();
        resphdrsOk = FALSE;
        resphdrLinecnt = 0;
        peekdata = resphdrs.data;
        peeklen = 0;
        contentlen = 0xffffffff;
        readcnt = 0;
    }

    BOOL ReadRespHdrs(BOOL wait);
    BOOL IsRespHdrsOk() { return resphdrsOk != 0; }
    VOID ParseRespHdrs(BOOL eof);
    BOOL MatchHeaderField(BYTE* data, const CHAR* fieldname, CHAR** fieldval);

    CHAR* ParseStatusLine(CHAR* buf, DWORD buflen, DWORD field);
    BOOL ReturnAllRespHdrs(WCHAR* buf, DWORD* buflen, DWORD field);

    ConnectObject* connobj;
        // Reference to the parent connection object
    
    DWORD_PTR appContext;
        // App-specific context value

    SOCKET sock;
        // Open socket handle

    HdrBuf reqhdrs;
        // HTTP request headers

    enum { MAXRESPHDRS = 32 };
        // Maximum number of HTTP response headers allowed

    HdrBuf resphdrs;
    BOOL resphdrsOk;
    BYTE* resphdrLines[MAXRESPHDRS];
    UINT resphdrLinecnt;
    BYTE* peekdata;
    UINT peeklen;
    UINT contentlen;
    UINT readcnt;
        // HTTP response information
};


//
// Macros for getting 
//
#define LOCK_INTERNET_OBJECT(_handle) \
        ((InternetObject*) BaseObject::Lock(_handle, OBJTYPE_INTERNET))

#define LOCK_CONNECT_OBJECT(_handle) \
        ((ConnectObject*) BaseObject::Lock(_handle, OBJTYPE_CONNECT))

#define LOCK_REQUEST_OBJECT(_handle) \
        ((RequestObject*) BaseObject::Lock(_handle, OBJTYPE_REQUEST))


//
// Wrap implementation-specific global functions
// and variable in our private namespace.
//
namespace WininetImpl {
    //
    // Resolve a host name to its IP address
    //  if the host name string is in dotted decimal form, 
    //  it's treated directly as an IP address string.
    //
    struct in_addr ResolveHostAddr(const CHAR* hostname);

    //
    // Crack an HTTP URL to extract various parts:
    //  serverName
    //  serverPort
    //  objectName
    //
    BOOL
    HttpParseUrl(
        const WCHAR* url,
        WCHAR** serverName,
        INTERNET_PORT* serverPort,
        WCHAR** objectName
        );


    //
    // Default HTTP string constants
    //
    extern const WCHAR HttpDefaultVerbStr[];
    extern const WCHAR HttpDefaultVersionStr[];
    extern const WCHAR HttpDefaultObjectStr[];

    //
    // Wrappers for Winsock APIs
    //
    inline _connect(SOCKET s, struct sockaddr_in* addr) {
        return connect(s, (struct sockaddr*) addr, sizeof(*addr));
    }

    //
    // Convert a Unicode string to an ASCII string
    //
    inline CHAR* strdupWtoA(const WCHAR* wstr, UINT count = 0xffffffff) {
        if (count == 0xffffffff) count = wcslen(wstr) + 1;
        CHAR* str = (CHAR*) MAlloc(count);
        CHAR* p;

        if ((p = str) != NULL) {
            // Just chop off the high byte of the Unicode value
            while (count--)
                *p++ = (CHAR) *wstr++;
        }
        return str;
    }

    inline VOID strcpyWtoA(CHAR* dst, const WCHAR* src) {
        while ((*dst++ = (CHAR) *src++) != 0) ;
    }

    inline VOID strcpyAtoW(WCHAR* dst, const CHAR* src) {
        while ((*dst++ = (UCHAR) *src++) != 0) ;
    }
}

using namespace WininetImpl;

#endif // !_WININETP_H

