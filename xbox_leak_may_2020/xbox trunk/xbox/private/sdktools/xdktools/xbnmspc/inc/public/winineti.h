
#if !defined(_WININETEX_)
#define _WININETEX_

#if defined(__cplusplus)
extern "C" {
#endif

#define MAX_CACHE_ENTRY_INFO_SIZE       4096
#define INTERNET_FLAG_UNUSED_1          0x00020000
#define INTERNET_FLAG_BGUPDATE          0x00000008
#define INTERNET_FLAG_UNUSED_4          0x00000004

//
// INTERNET_PREFETCH_STATUS -
//

typedef struct {

    //
    // dwStatus - status of download. See INTERNET_PREFETCH_ flags
    //

    DWORD dwStatus;

    //
    // dwSize - size of file downloaded so far
    //

    DWORD dwSize;
} INTERNET_PREFETCH_STATUS, * LPINTERNET_PREFETCH_STATUS;

//
// INTERNET_PREFETCH_STATUS - dwStatus values
//

#define INTERNET_PREFETCH_PROGRESS  0
#define INTERNET_PREFETCH_COMPLETE  1
#define INTERNET_PREFETCH_ABORTED   2

//
// INTERNET_DIAGNOSTIC_SOCKET_INFO - info about the socket in use
// (diagnostic purposes only, hence internal)
//

typedef struct {
    DWORD_PTR Socket;
    DWORD     SourcePort;
    DWORD     DestPort;
    DWORD     Flags;
} INTERNET_DIAGNOSTIC_SOCKET_INFO, * LPINTERNET_DIAGNOSTIC_SOCKET_INFO;

//
// INTERNET_DIAGNOSTIC_SOCKET_INFO.Flags definitions
//

#define IDSI_FLAG_KEEP_ALIVE    0x00000001  // set if from keep-alive pool
#define IDSI_FLAG_SECURE        0x00000002  // set if secure connection
#define IDSI_FLAG_PROXY         0x00000004  // set if using proxy
#define IDSI_FLAG_TUNNEL        0x00000008  // set if tunnelling through proxy

#define INTERNET_PER_CONN_AUTOCONFIG_SECONDARY_URL      6
#define INTERNET_PER_CONN_AUTOCONFIG_RELOAD_DELAY_MINS  7
#define INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_TIME   8
#define INTERNET_PER_CONN_AUTOCONFIG_LAST_DETECT_URL    9

#define INTERNET_ONLINE_OFFLINE_INFO    INTERNET_CONNECTED_INFO
#define LPINTERNET_ONLINE_OFFLINE_INFO  LPINTERNET_CONNECTED_INFO
#define dwOfflineState                  dwConnectedState


#define ISO_FORCE_OFFLINE       ISO_FORCE_DISCONNECTED


#ifdef __WINCRYPT_H__
#ifdef ALGIDDEF
//
// INTERNET_SECURITY_INFO - contains information about certificate
// and encryption settings for a connection.
//

#define INTERNET_SECURITY_INFO_DEFINED

typedef struct {

    //
    // dwSize - Size of INTERNET_SECURITY_INFO structure.
    //

    DWORD dwSize;


    //
    // pCertificate - Cert context pointing to leaf of certificate chain.
    //

    PCCERT_CONTEXT pCertificate;

    //
    // Start SecPkgContext_ConnectionInfo
    // The following members must match those
    // of the SecPkgContext_ConnectionInfo
    // sspi structure (schnlsp.h)
    //


    //
    // dwProtocol - Protocol that this connection was made with
    //  (PCT, SSL2, SSL3, etc)
    //

    DWORD dwProtocol;

    //
    // aiCipher - Cipher that this connection as made with
    //

    ALG_ID aiCipher;

    //
    // dwCipherStrength - Strength (in bits) that this connection
    //  was made with;
    //

    DWORD dwCipherStrength;

    //
    // aiHash - Hash that this connection as made with
    //

    ALG_ID aiHash;

    //
    // dwHashStrength - Strength (in bits) that this connection
    //  was made with;
    //

    DWORD dwHashStrength;

    //
    // aiExch - Key Exchange type that this connection as made with
    //

    ALG_ID aiExch;

    //
    // dwExchStrength - Strength (in bits) that this connection
    //  was made with;
    //

    DWORD dwExchStrength;


} INTERNET_SECURITY_INFO, * LPINTERNET_SECURITY_INFO;


typedef struct {
    //
    // dwSize - size of INTERNET_SECURITY_CONNECTION_INFO
    //
    DWORD dwSize;

    // fSecure - Is this a secure connection.
    BOOL fSecure;

    //
    // dwProtocol - Protocol that this connection was made with
    //  (PCT, SSL2, SSL3, etc)
    //

    DWORD dwProtocol;

    //
    // aiCipher - Cipher that this connection as made with
    //

    ALG_ID aiCipher;

    //
    // dwCipherStrength - Strength (in bits) that this connection
    //  was made with;
    //

    DWORD dwCipherStrength;

    //
    // aiHash - Hash that this connection as made with
    //

    ALG_ID aiHash;

    //
    // dwHashStrength - Strength (in bits) that this connection
    //  was made with;
    //

    DWORD dwHashStrength;

    //
    // aiExch - Key Exchange type that this connection as made with
    //

    ALG_ID aiExch;

    //
    // dwExchStrength - Strength (in bits) that this connection
    //  was made with;
    //

    DWORD dwExchStrength;

} INTERNET_SECURITY_CONNECTION_INFO , * LPINTERNET_SECURITY_CONNECTION_INFO;


INTERNETAPI
BOOL
WINAPI
InternetAlgIdToStringA(
    IN ALG_ID                         ai,
    IN LPSTR                        lpstr,
    IN OUT LPDWORD                    lpdwBufferLength,
    IN DWORD                          dwReserved
    );
INTERNETAPI
BOOL
WINAPI
InternetAlgIdToStringW(
    IN ALG_ID                         ai,
    IN LPWSTR                        lpstr,
    IN OUT LPDWORD                    lpdwBufferLength,
    IN DWORD                          dwReserved
    );
#ifdef UNICODE
#define InternetAlgIdToString  InternetAlgIdToStringW
#else
#define InternetAlgIdToString  InternetAlgIdToStringA
#endif // !UNICODE

INTERNETAPI
BOOL
WINAPI
InternetSecurityProtocolToStringA(
    IN DWORD                          dwProtocol,
    IN LPSTR                        lpstr,
    IN OUT LPDWORD                    lpdwBufferLength,
    IN DWORD                          dwReserved
    );
INTERNETAPI
BOOL
WINAPI
InternetSecurityProtocolToStringW(
    IN DWORD                          dwProtocol,
    IN LPWSTR                        lpstr,
    IN OUT LPDWORD                    lpdwBufferLength,
    IN DWORD                          dwReserved
    );
#ifdef UNICODE
#define InternetSecurityProtocolToString  InternetSecurityProtocolToStringW
#else
#define InternetSecurityProtocolToString  InternetSecurityProtocolToStringA
#endif // !UNICODE

#endif // ALGIDDEF
#endif // __WINCRYPT_H__

#ifdef INTERNET_SECURITY_INFO_DEFINED

INTERNETAPI
DWORD
WINAPI
ShowSecurityInfo(
    IN HWND                            hWndParent,
    IN LPINTERNET_SECURITY_INFO        pSecurityInfo
    );
#endif // INTERNET_SECURITY_INFO_DEFINED



INTERNETAPI
DWORD
WINAPI
ShowX509EncodedCertificate(
    IN HWND    hWndParent,
    IN LPBYTE  lpCert,
    IN DWORD   cbCert
    );

INTERNETAPI
DWORD
WINAPI
ShowClientAuthCerts(
    IN HWND hWndParent
    );

INTERNETAPI
DWORD
WINAPI
ParseX509EncodedCertificateForListBoxEntry(
    IN LPBYTE  lpCert,
    IN DWORD   cbCert,
    OUT LPSTR  lpszListBoxEntry,
    IN LPDWORD lpdwListBoxEntry
    );

//
// This is a private API for Trident.  It displays
// security info based on a URL
//

INTERNETAPI
BOOL
WINAPI
InternetShowSecurityInfoByURLA(
    IN       LPSTR    lpszURL,
    IN       HWND     hwndParent
    );

INTERNETAPI
BOOL
WINAPI
InternetShowSecurityInfoByURLW(
    IN       LPWSTR    lpszURL,
    IN       HWND     hwndParent
    );

#ifdef UNICODE
#define InternetShowSecurityInfoByURL  InternetShowSecurityInfoByURLW
#else
#ifdef _WINX32_
#define InternetShowSecurityInfoByURL  InternetShowSecurityInfoByURLA
#else
INTERNETAPI
BOOL
WINAPI
InternetShowSecurityInfoByURL(
    IN       LPSTR    lpszURL,
    IN       HWND     hwndParent
    );
#endif // _WINX32_
#endif // !UNICODE

//Fortezza related exports. not public

// The commands that InternetFortezzaCommand supports.

typedef enum {
    FORTCMD_LOGON                   = 1,
    FORTCMD_LOGOFF                  = 2,
    FORTCMD_CHG_PERSONALITY         = 3,
} FORTCMD;

    
INTERNETAPI
BOOL
WINAPI
InternetFortezzaCommand(DWORD dwCommand, HWND hwnd, DWORD_PTR dwReserved);


typedef enum {
    FORTSTAT_INSTALLED          = 0x00000001,
    FORTSTAT_LOGGEDON           = 0x00000002,
}   FORTSTAT ; 

INTERNETAPI
BOOL
WINAPI
InternetQueryFortezzaStatus(DWORD *pdwStatus, DWORD_PTR dwReserved);



BOOLAPI
InternetDebugGetLocalTime(
    OUT SYSTEMTIME * pstLocalTime,
    OUT DWORD      * pdwReserved
    );

#define INTERNET_SERVICE_URL    0
//
// InternetConnectUrl() - a macro which allows you to specify an URL instead of
// the component parts to InternetConnect(). If any API which uses the returned
// connect handle specifies a NULL path then the URL-path part of the URL
// specified in InternetConnectUrl() will be used
//

#define InternetConnectUrl(hInternet, lpszUrl, dwFlags, dwContext) \
    InternetConnect(hInternet,                      \
                    lpszUrl,                        \
                    INTERNET_INVALID_PORT_NUMBER,   \
                    NULL,                           \
                    NULL,                           \
                    INTERNET_SERVICE_URL,           \
                    dwFlags,                        \
                    dwContext                       \
                    )

INTERNETAPI
BOOL
WINAPI
InternetWriteFileExA(
    IN HINTERNET hFile,
    IN LPINTERNET_BUFFERSA lpBuffersIn,
    IN DWORD dwFlags,
    IN DWORD_PTR dwContext
    );
INTERNETAPI
BOOL
WINAPI
InternetWriteFileExW(
    IN HINTERNET hFile,
    IN LPINTERNET_BUFFERSW lpBuffersIn,
    IN DWORD dwFlags,
    IN DWORD_PTR dwContext
    );
#ifdef UNICODE
#define InternetWriteFileEx  InternetWriteFileExW
#else
#define InternetWriteFileEx  InternetWriteFileExA
#endif // !UNICODE

#define INTERNET_OPTION_CONTEXT_VALUE_OLD       10
#define INTERNET_OPTION_NET_SPEED               61
// Pass in pointer to INTERNET_SECURITY_CONNECTION_INFO to be filled in.
#define INTERNET_OPTION_SECURITY_CONNECTION_INFO  66
#define INTERNET_OPTION_DIAGNOSTIC_SOCKET_INFO  67
#define INTERNET_OPTION_DETECT_POST_SEND        71
#define INTERNET_OPTION_DISABLE_NTLM_PREAUTH    72
#define INTERNET_LAST_OPTION_INTERNAL           INTERNET_OPTION_IGNORE_OFFLINE


#define INTERNET_OPTION_OFFLINE_TIMEOUT INTERNET_OPTION_DISCONNECTED_TIMEOUT
#define INTERNET_OPTION_LINE_STATE      INTERNET_OPTION_CONNECTED_STATE


#define INTERNET_STATE_ONLINE       INTERNET_STATE_CONNECTED
#define INTERNET_STATE_OFFLINE      INTERNET_STATE_DISCONNECTED
#define INTERNET_STATE_OFFLINE_USER INTERNET_STATE_DISCONNECTED_BY_USER
#define INTERNET_LINE_STATE_MASK    (INTERNET_STATE_ONLINE | INTERNET_STATE_OFFLINE)
#define INTERNET_BUSY_STATE_MASK    (INTERNET_STATE_IDLE | INTERNET_STATE_BUSY)



//
// the following are used with InternetSetOption(..., INTERNET_OPTION_CALLBACK_FILTER, ...)
// to filter out unrequired callbacks. INTERNET_STATUS_REQUEST_COMPLETE cannot
// be filtered out
//

#define INTERNET_STATUS_FILTER_RESOLVING        0x00000001
#define INTERNET_STATUS_FILTER_RESOLVED         0x00000002
#define INTERNET_STATUS_FILTER_CONNECTING       0x00000004
#define INTERNET_STATUS_FILTER_CONNECTED        0x00000008
#define INTERNET_STATUS_FILTER_SENDING          0x00000010
#define INTERNET_STATUS_FILTER_SENT             0x00000020
#define INTERNET_STATUS_FILTER_RECEIVING        0x00000040
#define INTERNET_STATUS_FILTER_RECEIVED         0x00000080
#define INTERNET_STATUS_FILTER_CLOSING          0x00000100
#define INTERNET_STATUS_FILTER_CLOSED           0x00000200
#define INTERNET_STATUS_FILTER_HANDLE_CREATED   0x00000400
#define INTERNET_STATUS_FILTER_HANDLE_CLOSING   0x00000800
#define INTERNET_STATUS_FILTER_PREFETCH         0x00001000
#define INTERNET_STATUS_FILTER_REDIRECT         0x00002000
#define INTERNET_STATUS_FILTER_STATE_CHANGE     0x00004000


//
// Note that adding any HTTP_QUERY_* codes here must be followed
//   by an equivlent line in wininet\http\hashgen\hashgen.cpp
//   please see that file for further information regarding
//   the addition of new HTTP headers
//


// These are not part of HTTP 1.1 yet. We will propose these to the
// HTTP extensions working group. These are required for the client-caps support
// we are doing in conjuntion with IIS.

typedef struct _INTERNET_COOKIE {
    DWORD cbSize;
    LPSTR pszName;
    LPSTR pszData;
    LPSTR pszDomain;
    LPSTR pszPath;
    FILETIME *pftExpires;
    DWORD dwFlags;
    LPSTR pszUrl;
} INTERNET_COOKIE, *PINTERNET_COOKIE;

#define INTERNET_COOKIE_IS_SECURE   0x01
#define INTERNET_COOKIE_IS_SESSION  0x02

//
// DAV Detection
//
BOOLAPI
HttpCheckDavComplianceA(
    IN LPCSTR lpszUrl,
    IN LPCSTR lpszComplianceToken,
    IN OUT LPBOOL lpfFound,
    IN HWND hWnd,
    IN LPVOID lpvReserved
    );
//
// DAV Detection
//
BOOLAPI
HttpCheckDavComplianceW(
    IN LPCWSTR lpszUrl,
    IN LPCWSTR lpszComplianceToken,
    IN OUT LPBOOL lpfFound,
    IN HWND hWnd,
    IN LPVOID lpvReserved
    );
#ifdef UNICODE
#define HttpCheckDavCompliance  HttpCheckDavComplianceW
#else
#define HttpCheckDavCompliance  HttpCheckDavComplianceA
#endif // !UNICODE

BOOLAPI
HttpCheckCachedDavStatusA(
    IN LPCSTR lpszUrl,
    IN OUT LPDWORD lpdwStatus
    );
BOOLAPI
HttpCheckCachedDavStatusW(
    IN LPCWSTR lpszUrl,
    IN OUT LPDWORD lpdwStatus
    );
#ifdef UNICODE
#define HttpCheckCachedDavStatus  HttpCheckCachedDavStatusW
#else
#define HttpCheckCachedDavStatus  HttpCheckCachedDavStatusA
#endif // !UNICODE

BOOLAPI
HttpCheckDavCollectionA(
    IN LPCSTR lpszUrl,
    IN OUT LPBOOL lpfFound,
    IN HWND hWnd,
    IN LPVOID lpvReserved
    );
BOOLAPI
HttpCheckDavCollectionW(
    IN LPCWSTR lpszUrl,
    IN OUT LPBOOL lpfFound,
    IN HWND hWnd,
    IN LPVOID lpvReserved
    );
#ifdef UNICODE
#define HttpCheckDavCollection  HttpCheckDavCollectionW
#else
#define HttpCheckDavCollection  HttpCheckDavCollectionA
#endif // !UNICODE

// DAV detection defines
#define DAV_LEVEL1_STATUS               0x00000001
#define DAV_COLLECTION_STATUS           0x00004000
#define DAV_DETECTION_REQUIRED          0x00008000
#define ERROR_INTERNET_NO_NEW_CONTAINERS        (INTERNET_ERROR_BASE + 51)

#define ERROR_INTERNET_OFFLINE  ERROR_INTERNET_DISCONNECTED

//
// internal error codes that are used to communicate specific information inside
// of Wininet but which are meaningless at the interface
//

#define INTERNET_INTERNAL_ERROR_BASE            (INTERNET_ERROR_BASE + 900)

#define ERROR_INTERNET_INTERNAL_SOCKET_ERROR    (INTERNET_INTERNAL_ERROR_BASE + 1)
#define ERROR_INTERNET_CONNECTION_AVAILABLE     (INTERNET_INTERNAL_ERROR_BASE + 2)
#define ERROR_INTERNET_NO_KNOWN_SERVERS         (INTERNET_INTERNAL_ERROR_BASE + 3)
#define ERROR_INTERNET_PING_FAILED              (INTERNET_INTERNAL_ERROR_BASE + 4)
#define ERROR_INTERNET_NO_PING_SUPPORT          (INTERNET_INTERNAL_ERROR_BASE + 5)
#define ERROR_INTERNET_CACHE_SUCCESS            (INTERNET_INTERNAL_ERROR_BASE + 6)
#define HTTP_1_1_CACHE_ENTRY            0x00000040
#define STATIC_CACHE_ENTRY              0x00000080
#define MUST_REVALIDATE_CACHE_ENTRY     0x00000100
#define PENDING_DELETE_CACHE_ENTRY      0x00400000
#define OTHER_USER_CACHE_ENTRY          0x00800000
#define POST_RESPONSE_CACHE_ENTRY       0x04000000
#define INSTALLED_CACHE_ENTRY           0x10000000
#define POST_CHECK_CACHE_ENTRY          0x20000000

// We include some entry types even if app doesn't specifically ask for them.
#define INCLUDE_BY_DEFAULT_CACHE_ENTRY \
    (HTTP_1_1_CACHE_ENTRY | MUST_REVALIDATE_CACHE_ENTRY)


//
// Well known sticky group ID
//
#define CACHEGROUP_ID_BUILTIN_STICKY       0x1000000000000007

//
// INTERNET_CACHE_CONFIG_PATH_ENTRY
//

typedef struct _INTERNET_CACHE_CONFIG_PATH_ENTRYA {
    CHAR   CachePath[MAX_PATH];
    DWORD dwCacheSize;  // in KBytes
} INTERNET_CACHE_CONFIG_PATH_ENTRYA, * LPINTERNET_CACHE_CONFIG_PATH_ENTRYA;
typedef struct _INTERNET_CACHE_CONFIG_PATH_ENTRYW {
    WCHAR  CachePath[MAX_PATH];
    DWORD dwCacheSize;  // in KBytes
} INTERNET_CACHE_CONFIG_PATH_ENTRYW, * LPINTERNET_CACHE_CONFIG_PATH_ENTRYW;
#ifdef UNICODE
typedef INTERNET_CACHE_CONFIG_PATH_ENTRYW INTERNET_CACHE_CONFIG_PATH_ENTRY;
typedef LPINTERNET_CACHE_CONFIG_PATH_ENTRYW LPINTERNET_CACHE_CONFIG_PATH_ENTRY;
#else
typedef INTERNET_CACHE_CONFIG_PATH_ENTRYA INTERNET_CACHE_CONFIG_PATH_ENTRY;
typedef LPINTERNET_CACHE_CONFIG_PATH_ENTRYA LPINTERNET_CACHE_CONFIG_PATH_ENTRY;
#endif // UNICODE

//
// INTERNET_CACHE_CONFIG_INFO
//

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4201)

typedef struct _INTERNET_CACHE_CONFIG_INFOA {
    DWORD dwStructSize;
    DWORD dwContainer;
    DWORD dwQuota;
    DWORD dwReserved4;
    BOOL  fPerUser;
    DWORD dwSyncMode;
    DWORD dwNumCachePaths;
    union 
    { 
        struct 
        {
            CHAR   CachePath[MAX_PATH];
            DWORD dwCacheSize;
        };
        INTERNET_CACHE_CONFIG_PATH_ENTRYA CachePaths[ANYSIZE_ARRAY];
    };
    DWORD dwNormalUsage;
    DWORD dwExemptUsage;
} INTERNET_CACHE_CONFIG_INFOA, * LPINTERNET_CACHE_CONFIG_INFOA;
typedef struct _INTERNET_CACHE_CONFIG_INFOW {
    DWORD dwStructSize;
    DWORD dwContainer;
    DWORD dwQuota;
    DWORD dwReserved4;
    BOOL  fPerUser;
    DWORD dwSyncMode;
    DWORD dwNumCachePaths;
    union 
    { 
        struct 
        {
            WCHAR  CachePath[MAX_PATH];
            DWORD dwCacheSize;
        };
        INTERNET_CACHE_CONFIG_PATH_ENTRYW CachePaths[ANYSIZE_ARRAY];
    };
    DWORD dwNormalUsage;
    DWORD dwExemptUsage;
} INTERNET_CACHE_CONFIG_INFOW, * LPINTERNET_CACHE_CONFIG_INFOW;
#ifdef UNICODE
typedef INTERNET_CACHE_CONFIG_INFOW INTERNET_CACHE_CONFIG_INFO;
typedef LPINTERNET_CACHE_CONFIG_INFOW LPINTERNET_CACHE_CONFIG_INFO;
#else
typedef INTERNET_CACHE_CONFIG_INFOA INTERNET_CACHE_CONFIG_INFO;
typedef LPINTERNET_CACHE_CONFIG_INFOA LPINTERNET_CACHE_CONFIG_INFO;
#endif // UNICODE

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4201)
#endif


BOOLAPI
IsUrlCacheEntryExpiredA(
    IN      LPCSTR        lpszUrlName,
    IN      DWORD           dwFlags,
    IN OUT  FILETIME*       pftLastModified
    );
BOOLAPI
IsUrlCacheEntryExpiredW(
    IN      LPCWSTR        lpszUrlName,
    IN      DWORD           dwFlags,
    IN OUT  FILETIME*       pftLastModified
    );
#ifdef UNICODE
#define IsUrlCacheEntryExpired  IsUrlCacheEntryExpiredW
#else
#define IsUrlCacheEntryExpired  IsUrlCacheEntryExpiredA
#endif // !UNICODE


#define INTERNET_CACHE_FLAG_ALLOW_COLLISIONS     0x00000100
#define INTERNET_CACHE_FLAG_INSTALLED_ENTRY      0x00000200
#define INTERNET_CACHE_FLAG_ENTRY_OR_MAPPING     0x00000400
#define INTERNET_CACHE_FLAG_ADD_FILENAME_ONLY    0x00000800
#define INTERNET_CACHE_FLAG_GET_STRUCT_ONLY      0x00001000
#define CACHE_ENTRY_MODIFY_DATA_FC  0x80000000

// Flags for CreateContainer

#define INTERNET_CACHE_CONTAINER_NOSUBDIRS (0x1)
#define INTERNET_CACHE_CONTAINER_AUTODELETE (0x2)
#define INTERNET_CACHE_CONTAINER_RESERVED1 (0x4)
#define INTERNET_CACHE_CONTAINER_NODESKTOPINIT (0x8)
#define INTERNET_CACHE_CONTAINER_MAP_ENABLED (0x10)

BOOLAPI
CreateUrlCacheContainerA(
     IN LPCSTR Name,
     IN LPCSTR lpCachePrefix,
     LPCSTR lpszCachePath,
     IN DWORD KBCacheLimit,
     IN DWORD dwContainerType,
     IN DWORD dwOptions,
     IN OUT LPVOID pvBuffer,
     IN OUT LPDWORD cbBuffer
     );
BOOLAPI
CreateUrlCacheContainerW(
     IN LPCWSTR Name,
     IN LPCWSTR lpCachePrefix,
     LPCWSTR lpszCachePath,
     IN DWORD KBCacheLimit,
     IN DWORD dwContainerType,
     IN DWORD dwOptions,
     IN OUT LPVOID pvBuffer,
     IN OUT LPDWORD cbBuffer
     );
#ifdef UNICODE
#define CreateUrlCacheContainer  CreateUrlCacheContainerW
#else
#define CreateUrlCacheContainer  CreateUrlCacheContainerA
#endif // !UNICODE

BOOLAPI
DeleteUrlCacheContainerA(
     IN LPCSTR Name,
     IN DWORD dwOptions
     );
BOOLAPI
DeleteUrlCacheContainerW(
     IN LPCWSTR Name,
     IN DWORD dwOptions
     );
#ifdef UNICODE
#define DeleteUrlCacheContainer  DeleteUrlCacheContainerW
#else
#define DeleteUrlCacheContainer  DeleteUrlCacheContainerA
#endif // !UNICODE

//
// INTERNET_CACHE_ENTRY_INFO -
//


typedef struct _INTERNET_CACHE_CONTAINER_INFOA {
    DWORD dwCacheVersion;       // version of software
    LPSTR   lpszName;             // embedded pointer to the container name string.
    LPSTR   lpszCachePrefix;      // embedded pointer to the container URL prefix
    LPSTR   lpszVolumeLabel;      // embedded pointer to the container volume label if any.
    LPSTR   lpszVolumeTitle;      // embedded pointer to the container volume title if any.
} INTERNET_CACHE_CONTAINER_INFOA, * LPINTERNET_CACHE_CONTAINER_INFOA;
typedef struct _INTERNET_CACHE_CONTAINER_INFOW {
    DWORD dwCacheVersion;       // version of software
    LPWSTR  lpszName;             // embedded pointer to the container name string.
    LPWSTR  lpszCachePrefix;      // embedded pointer to the container URL prefix
    LPWSTR  lpszVolumeLabel;      // embedded pointer to the container volume label if any.
    LPWSTR  lpszVolumeTitle;      // embedded pointer to the container volume title if any.
} INTERNET_CACHE_CONTAINER_INFOW, * LPINTERNET_CACHE_CONTAINER_INFOW;
#ifdef UNICODE
typedef INTERNET_CACHE_CONTAINER_INFOW INTERNET_CACHE_CONTAINER_INFO;
typedef LPINTERNET_CACHE_CONTAINER_INFOW LPINTERNET_CACHE_CONTAINER_INFO;
#else
typedef INTERNET_CACHE_CONTAINER_INFOA INTERNET_CACHE_CONTAINER_INFO;
typedef LPINTERNET_CACHE_CONTAINER_INFOA LPINTERNET_CACHE_CONTAINER_INFO;
#endif // UNICODE

//  FindFirstContainer options
#define CACHE_FIND_CONTAINER_RETURN_NOCHANGE (0x1)

INTERNETAPI
HANDLE
WINAPI
FindFirstUrlCacheContainerA(
    IN OUT LPDWORD pdwModified,
    OUT LPINTERNET_CACHE_CONTAINER_INFOA lpContainerInfo,
    IN OUT LPDWORD lpdwContainerInfoBufferSize,
    IN DWORD dwOptions
    );
INTERNETAPI
HANDLE
WINAPI
FindFirstUrlCacheContainerW(
    IN OUT LPDWORD pdwModified,
    OUT LPINTERNET_CACHE_CONTAINER_INFOW lpContainerInfo,
    IN OUT LPDWORD lpdwContainerInfoBufferSize,
    IN DWORD dwOptions
    );
#ifdef UNICODE
#define FindFirstUrlCacheContainer  FindFirstUrlCacheContainerW
#else
#define FindFirstUrlCacheContainer  FindFirstUrlCacheContainerA
#endif // !UNICODE

BOOLAPI
FindNextUrlCacheContainerA(
    IN HANDLE hEnumHandle,
    OUT LPINTERNET_CACHE_CONTAINER_INFOA lpContainerInfo,
    IN OUT LPDWORD lpdwContainerInfoBufferSize
    );
BOOLAPI
FindNextUrlCacheContainerW(
    IN HANDLE hEnumHandle,
    OUT LPINTERNET_CACHE_CONTAINER_INFOW lpContainerInfo,
    IN OUT LPDWORD lpdwContainerInfoBufferSize
    );
#ifdef UNICODE
#define FindNextUrlCacheContainer  FindNextUrlCacheContainerW
#else
#define FindNextUrlCacheContainer  FindNextUrlCacheContainerA
#endif // !UNICODE


typedef enum {
    WININET_SYNC_MODE_NEVER=0,
    WININET_SYNC_MODE_ON_EXPIRY, // bogus
    WININET_SYNC_MODE_ONCE_PER_SESSION,
    WININET_SYNC_MODE_ALWAYS,
    WININET_SYNC_MODE_AUTOMATIC,
    WININET_SYNC_MODE_DEFAULT = WININET_SYNC_MODE_AUTOMATIC
} WININET_SYNC_MODE;


BOOLAPI
FreeUrlCacheSpaceA(
    IN LPCSTR lpszCachePath,
    IN DWORD dwSize,
    IN DWORD dwFilter
    );
BOOLAPI
FreeUrlCacheSpaceW(
    IN LPCWSTR lpszCachePath,
    IN DWORD dwSize,
    IN DWORD dwFilter
    );
#ifdef UNICODE
#define FreeUrlCacheSpace  FreeUrlCacheSpaceW
#else
#define FreeUrlCacheSpace  FreeUrlCacheSpaceA
#endif // !UNICODE

//
// config APIs.
//

#define CACHE_CONFIG_FORCE_CLEANUP_FC           0x00000020
#define CACHE_CONFIG_DISK_CACHE_PATHS_FC        0x00000040
#define CACHE_CONFIG_SYNC_MODE_FC               0x00000080
#define CACHE_CONFIG_CONTENT_PATHS_FC           0x00000100
#define CACHE_CONFIG_COOKIES_PATHS_FC           0x00000200
#define CACHE_CONFIG_HISTORY_PATHS_FC           0x00000400
#define CACHE_CONFIG_QUOTA_FC                   0x00000800
#define CACHE_CONFIG_USER_MODE_FC               0x00001000
#define CACHE_CONFIG_CONTENT_USAGE_FC           0x00002000
#define CACHE_CONFIG_STICKY_CONTENT_USAGE_FC    0x00004000

BOOLAPI
GetUrlCacheConfigInfoA(
    OUT LPINTERNET_CACHE_CONFIG_INFOA lpCacheConfigInfo,
    IN OUT LPDWORD lpdwCacheConfigInfoBufferSize,
    IN DWORD dwFieldControl
    );
BOOLAPI
GetUrlCacheConfigInfoW(
    OUT LPINTERNET_CACHE_CONFIG_INFOW lpCacheConfigInfo,
    IN OUT LPDWORD lpdwCacheConfigInfoBufferSize,
    IN DWORD dwFieldControl
    );
#ifdef UNICODE
#define GetUrlCacheConfigInfo  GetUrlCacheConfigInfoW
#else
#define GetUrlCacheConfigInfo  GetUrlCacheConfigInfoA
#endif // !UNICODE

BOOLAPI
SetUrlCacheConfigInfoA(
    IN LPINTERNET_CACHE_CONFIG_INFOA lpCacheConfigInfo,
    IN DWORD dwFieldControl
    );
BOOLAPI
SetUrlCacheConfigInfoW(
    IN LPINTERNET_CACHE_CONFIG_INFOW lpCacheConfigInfo,
    IN DWORD dwFieldControl
    );
#ifdef UNICODE
#define SetUrlCacheConfigInfo  SetUrlCacheConfigInfoW
#else
#define SetUrlCacheConfigInfo  SetUrlCacheConfigInfoA
#endif // !UNICODE

INTERNETAPI
DWORD
WINAPI
RunOnceUrlCache(
        HWND    hwnd,
        HINSTANCE hinst,
        LPSTR   lpszCmd,
        int     nCmdShow);

INTERNETAPI
DWORD
WINAPI
DeleteIE3Cache(
        HWND    hwnd,
        HINSTANCE hinst,
        LPSTR   lpszCmd,
        int     nCmdShow);

BOOLAPI
UpdateUrlCacheContentPath(LPSTR szNewPath);

// Cache header data defines.

#define CACHE_HEADER_DATA_CURRENT_SETTINGS_VERSION   0
#define CACHE_HEADER_DATA_CONLIST_CHANGE_COUNT       1
#define CACHE_HEADER_DATA_COOKIE_CHANGE_COUNT        2


#define CACHE_HEADER_DATA_NOTIFICATION_HWND          3
#define CACHE_HEADER_DATA_NOTIFICATION_MESG          4
#define CACHE_HEADER_DATA_ROOTGROUP_OFFSET           5
#define CACHE_HEADER_DATA_GID_LOW                    6
#define CACHE_HEADER_DATA_GID_HIGH                   7

// beta logging stats
#define CACHE_HEADER_DATA_CACHE_NOT_EXPIRED         8
#define CACHE_HEADER_DATA_CACHE_NOT_MODIFIED        9
#define CACHE_HEADER_DATA_CACHE_MODIFIED            10
#define CACHE_HEADER_DATA_CACHE_RESUMED             11
#define CACHE_HEADER_DATA_CACHE_NOT_RESUMED         12
#define CACHE_HEADER_DATA_CACHE_MISS                13
#define CACHE_HEADER_DATA_DOWNLOAD_PARTIAL          14
#define CACHE_HEADER_DATA_DOWNLOAD_ABORTED          15
#define CACHE_HEADER_DATA_DOWNLOAD_CACHED           16
#define CACHE_HEADER_DATA_DOWNLOAD_NOT_CACHED       17
#define CACHE_HEADER_DATA_DOWNLOAD_NO_FILE          18
#define CACHE_HEADER_DATA_DOWNLOAD_FILE_NEEDED      19
#define CACHE_HEADER_DATA_DOWNLOAD_FILE_NOT_NEEDED  20

// retail data
#define CACHE_HEADER_DATA_NOTIFICATION_FILTER       21
#define CACHE_HEADER_DATA_ROOT_LEAK_OFFSET          22

// more beta logging stats
#define CACHE_HEADER_DATA_SYNCSTATE_IMAGE           23
#define CACHE_HEADER_DATA_SYNCSTATE_VOLATILE        24
#define CACHE_HEADER_DATA_SYNCSTATE_IMAGE_STATIC    25
#define CACHE_HEADER_DATA_SYNCSTATE_STATIC_VOLATILE 26

// retail data
#define CACHE_HEADER_DATA_ROOT_GROUPLIST_OFFSET     27 // offset to group list
#define CACHE_HEADER_DATA_ROOT_FIXUP_OFFSET         28 // offset to fixup list
#define CACHE_HEADER_DATA_ROOT_FIXUP_COUNT          29 // num of fixup items
#define CACHE_HEADER_DATA_ROOT_FIXUP_TRIGGER        30 // threshhold to fix up
#define CACHE_HEADER_DATA_HIGH_VERSION_STRING       31 // highest entry ver


#define CACHE_HEADER_DATA_LAST                      31

// options for cache notification filter
#define CACHE_NOTIFY_ADD_URL                        0x00000001
#define CACHE_NOTIFY_DELETE_URL                     0x00000002
#define CACHE_NOTIFY_UPDATE_URL                     0x00000004
#define CACHE_NOTIFY_DELETE_ALL                     0x00000008
#define CACHE_NOTIFY_URL_SET_STICKY                 0x00000010
#define CACHE_NOTIFY_URL_UNSET_STICKY               0x00000020
#define CACHE_NOTIFY_SET_ONLINE                     0x00000100
#define CACHE_NOTIFY_SET_OFFLINE                    0x00000200

#define CACHE_NOTIFY_FILTER_CHANGED                 0x10000000

BOOL
RegisterUrlCacheNotification(
    IN  HWND        hWnd,
    IN  UINT        uMsg,
    IN  GROUPID     gid,
    IN  DWORD       dwOpsFilter,
    IN  DWORD       dwReserved
    );



BOOL
GetUrlCacheHeaderData(IN DWORD nIdx, OUT LPDWORD lpdwData);

BOOL
SetUrlCacheHeaderData(IN DWORD nIdx, IN  DWORD  dwData);

BOOL
IncrementUrlCacheHeaderData(IN DWORD nIdx, OUT LPDWORD lpdwData);

BOOL
LoadUrlCacheContent();

BOOL
GetUrlCacheContainerInfoA(
    IN LPSTR lpszUrlName,
    OUT LPINTERNET_CACHE_CONTAINER_INFOA lpContainerInfo,
    IN OUT LPDWORD lpdwContainerInfoBufferSize,
    IN DWORD dwOptions
    );
BOOL
GetUrlCacheContainerInfoW(
    IN LPWSTR lpszUrlName,
    OUT LPINTERNET_CACHE_CONTAINER_INFOW lpContainerInfo,
    IN OUT LPDWORD lpdwContainerInfoBufferSize,
    IN DWORD dwOptions
    );
#ifdef UNICODE
#define GetUrlCacheContainerInfo  GetUrlCacheContainerInfoW
#else
#define GetUrlCacheContainerInfo  GetUrlCacheContainerInfoA
#endif // !UNICODE


//
// Autodial APIs
//

INTERNETAPI
DWORD
WINAPI
InternetDialA(
    IN HWND     hwndParent,
    IN LPSTR   lpszConnectoid,
    IN DWORD    dwFlags,
    OUT DWORD_PTR *lpdwConnection,
    IN DWORD    dwReserved
    );

INTERNETAPI
DWORD
WINAPI
InternetDialW(
    IN HWND     hwndParent,
    IN LPWSTR   lpszConnectoid,
    IN DWORD    dwFlags,
    OUT DWORD_PTR *lpdwConnection,
    IN DWORD    dwReserved
    );

#ifdef UNICODE
#define InternetDial  InternetDialW
#else
#ifdef _WINX32_
#define InternetDial  InternetDialA
#else
INTERNETAPI
DWORD
WINAPI
InternetDial(
    IN HWND     hwndParent,
    IN LPSTR   lpszConnectoid,
    IN DWORD    dwFlags,
    OUT LPDWORD lpdwConnection,
    IN DWORD    dwReserved
    );
#endif // _WINX32_
#endif // !UNICODE

// Flags for InternetDial - must not conflict with InternetAutodial flags
//                          as they are valid here also.
#define INTERNET_DIAL_FORCE_PROMPT     0x2000
#define INTERNET_DIAL_SHOW_OFFLINE     0x4000
#define INTERNET_DIAL_UNATTENDED       0x8000

INTERNETAPI
DWORD
WINAPI
InternetHangUp(
    IN DWORD    dwConnection,
    IN DWORD    dwReserved);

#define INTERENT_GOONLINE_REFRESH 0x00000001
#define INTERENT_GOONLINE_MASK 0x00000001

INTERNETAPI
BOOL
WINAPI
InternetGoOnlineA(
    IN LPSTR   lpszURL,
    IN HWND     hwndParent,
    IN DWORD    dwFlags
    );

INTERNETAPI
BOOL
WINAPI
InternetGoOnlineW(
    IN LPWSTR   lpszURL,
    IN HWND     hwndParent,
    IN DWORD    dwFlags
    );

#ifdef UNICODE
#define InternetGoOnline  InternetGoOnlineW
#else
#ifdef _WINX32_
#define InternetGoOnline  InternetGoOnlineA
#else
INTERNETAPI
BOOL
WINAPI
InternetGoOnline(
    IN LPSTR   lpszURL,
    IN HWND     hwndParent,
    IN DWORD    dwFlags
    );
#endif // _WINX32_
#endif // !UNICODE

INTERNETAPI
BOOL
WINAPI
InternetAutodial(
    IN DWORD    dwFlags,
    IN HWND     hwndParent);

// Flags for InternetAutodial
#define INTERNET_AUTODIAL_FORCE_ONLINE          1
#define INTERNET_AUTODIAL_FORCE_UNATTENDED      2
#define INTERNET_AUTODIAL_FAILIFSECURITYCHECK   4

#define INTERNET_AUTODIAL_FLAGS_MASK (INTERNET_AUTODIAL_FORCE_ONLINE | INTERNET_AUTODIAL_FORCE_UNATTENDED | INTERNET_AUTODIAL_FAILIFSECURITYCHECK)
INTERNETAPI
BOOL
WINAPI
InternetAutodialHangup(
    IN DWORD    dwReserved);

INTERNETAPI
BOOL
WINAPI
InternetGetConnectedState(
    OUT LPDWORD  lpdwFlags,
    IN DWORD    dwReserved);

INTERNETAPI
BOOL
WINAPI
InternetGetConnectedStateExA(
    OUT LPDWORD lpdwFlags,
    OUT LPSTR  lpszConnectionName,
    IN DWORD    dwNameLen,
    IN DWORD    dwReserved
    );

INTERNETAPI
BOOL
WINAPI
InternetGetConnectedStateExW(
    OUT LPDWORD lpdwFlags,
    OUT LPWSTR  lpszConnectionName,
    IN DWORD    dwNameLen,
    IN DWORD    dwReserved
    );

#ifdef UNUSED
INTERNETAPI
BOOL
WINAPI
InternetInitializeAutoProxyDll(
    DWORD dwReserved
    );
#endif
    
#ifdef UNICODE
#define InternetGetConnectedStateEx  InternetGetConnectedStateExW
#else
#ifdef _WINX32_
#define InternetGetConnectedStateEx  InternetGetConnectedStateExA
#else
INTERNETAPI
BOOL
WINAPI
InternetGetConnectedStateEx(
    OUT LPDWORD lpdwFlags,
    IN LPSTR  lpszConnectionName,
    IN DWORD    dwNameLen,
    IN DWORD    dwReserved
    );
#endif // _WINX32_
#endif // !UNICODE

// Flags for InternetGetConnectedState and Ex
#define INTERNET_CONNECTION_MODEM           0x01
#define INTERNET_CONNECTION_LAN             0x02
#define INTERNET_CONNECTION_PROXY           0x04
#define INTERNET_CONNECTION_MODEM_BUSY      0x08  /* no longer used */
#define INTERNET_RAS_INSTALLED              0x10
#define INTERNET_CONNECTION_OFFLINE         0x20
#define INTERNET_CONNECTION_CONFIGURED      0x40

//
// Custom dial handler functions
//

// Custom dial handler prototype
typedef DWORD (CALLBACK * PFN_DIAL_HANDLER) (HWND, LPCSTR, DWORD, LPDWORD);

// Flags for custom dial handler
#define INTERNET_CUSTOMDIAL_CONNECT         0
#define INTERNET_CUSTOMDIAL_UNATTENDED      1
#define INTERNET_CUSTOMDIAL_DISCONNECT      2
#define INTERNET_CUSTOMDIAL_SHOWOFFLINE     4

// Custom dial handler supported functionality flags
#define INTERNET_CUSTOMDIAL_SAFE_FOR_UNATTENDED 1
#define INTERNET_CUSTOMDIAL_WILL_SUPPLY_STATE   2
#define INTERNET_CUSTOMDIAL_CAN_HANGUP          4

INTERNETAPI
BOOL
WINAPI
InternetSetDialStateA(
    IN LPCSTR lpszConnectoid,
    IN DWORD    dwState,
    IN DWORD    dwReserved
    );

INTERNETAPI
BOOL
WINAPI
InternetSetDialStateW(
    IN LPCWSTR lpszConnectoid,
    IN DWORD    dwState,
    IN DWORD    dwReserved
    );

#ifdef UNICODE
#define InternetSetDialState  InternetSetDialStateW
#else
#ifdef _WINX32_
#define InternetSetDialState  InternetSetDialStateA
#else
INTERNETAPI
BOOL
WINAPI
InternetSetDialState(
    IN LPCSTR lpszConnectoid,
    IN DWORD    dwState,
    IN DWORD    dwReserved
    );
#endif // _WINX32_
#endif // !UNICODE

// States for InternetSetDialState
#define INTERNET_DIALSTATE_DISCONNECTED     1

// Registry entries used by the dialing code
// All of these entries are in:
// HKCU\software\microsoft\windows\current version\internet settings

#define REGSTR_DIAL_AUTOCONNECT     "AutoConnect"


// Wininet events

#define REGSTR_PATH_INETEVENTS      "Software\\microsoft\\windows\\currentversion\\internet settings\\Events"

#define INETEVT_RAS_CONNECT         0x00000001
#define INETEVT_RAS_DISCONNECT      0x00000002
#define INETEVT_ONLINE              0x00000004
#define INETEVT_OFFLINE             0x00000008
#define INETEVT_LOGON               0x00000010
#define INETEVT_LOGOFF              0x00000020

INTERNETAPI
DWORD
WINAPI
InternetDispatchEvent(
    IN DWORD dwEvent,
    IN LPWSTR pwsEventDesc,
    IN DWORD dwEventData);



// Used by security manager.

INTERNETAPI
BOOL
WINAPI
IsHostInProxyBypassList(
    IN INTERNET_SCHEME tScheme,
    IN LPCSTR   lpszHost,
    IN DWORD    cchHost);

// Used by Shell to determine if anyone has loaded wininet yet
// Shell code calls OpenMutex with this name and if no mutex is
// obtained, we know that no copy of wininet has been loaded yet

#define WININET_STARTUP_MUTEX "WininetStartupMutex"


BOOL DoConnectoidsExist(void); // Returns TRUE if any RAS connectoids exist and FALSE otherwise

BOOL GetDiskInfoA(
    IN      PSTR pszPath, 
    IN OUT  PDWORD pdwClusterSize, 
    IN OUT  PDWORDLONG pdlAvail, 
    IN OUT  PDWORDLONG pdlTotal);

typedef BOOL (*CACHE_OPERATOR)(INTERNET_CACHE_ENTRY_INFO* pcei, PDWORD pcbcei, PVOID pOpData);

BOOL PerformOperationOverUrlCacheA(
    IN     PCSTR     pszUrlSearchPattern,
    IN     DWORD     dwFlags,
    IN     DWORD     dwFilter,
    IN     GROUPID   GroupId,
    OUT    PVOID     pReserved1,
    IN OUT PDWORD    pdwReserved2,
    IN     PVOID     pReserved3,
    IN       CACHE_OPERATOR op, 
    IN OUT PVOID     pOperatorData
    );

BOOL IsProfilesCapable();



//  in cookimp.cxx and cookexp.cxx
BOOL ImportCookieFileA( IN LPCSTR szFilename );
//  in cookimp.cxx and cookexp.cxx
BOOL ImportCookieFileW( IN LPCWSTR szFilename );
#ifdef UNICODE
#define ImportCookieFile  ImportCookieFileW
#else
#define ImportCookieFile  ImportCookieFileA
#endif // !UNICODE
BOOL ExportCookieFileA( IN LPCSTR szFilename, BOOL fAppend);
BOOL ExportCookieFileW( IN LPCWSTR szFilename, BOOL fAppend);
#ifdef UNICODE
#define ExportCookieFile  ExportCookieFileW
#else
#define ExportCookieFile  ExportCookieFileA
#endif // !UNICODE


#if defined(__cplusplus)
}
#endif

#endif // !define(_WININETEX_)
