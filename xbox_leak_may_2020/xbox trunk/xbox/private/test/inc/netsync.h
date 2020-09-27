/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  netsync.h

Abstract:

  This module contains the definitions for netsync.dll

Author:

  Steven Kehrli (steveke) 21-Jul-2000

------------------------------------------------------------------------------*/

#pragma once



#ifdef _XBOX

#include <winsockx.h>

#else

#include <winsock2.h>
#include <wsockntp.h>

#endif



#ifdef __cplusplus
extern "C" {
#endif



// NETSYNC_MESSAGE_SIZE is the size limit, in bytes, of a netsync message
#define NETSYNC_MESSAGE_SIZE          1024



// Function Pointer Prototypes

// PNETSYNC_THREAD_FUNCTION is the main function prototype of a thread session
// NetsyncThreadFunction should return when the session is complete
typedef VOID (WINAPI *PNETSYNC_THREAD_FUNCTION) (
    IN HANDLE   hNetsyncObject,       // Handle to the netsync object for the session
    IN BYTE     byClientCount,        // Specifies the number of clients in the session
    IN u_long   *ClientInAddr,        // Pointer to the array of client ip addresses
    IN XNADDR   *ClientXnAddr,        // Pointer to the array of client xnet addresses
    IN u_short  LowPort,              // Specifies the low port number allocated for the session
    IN u_short  HighPort              // Specifies the high port number allocated for the session
);



// PNETSYNC_START_FUNCTION is the start function prototype
// NetsyncStartFunction is called once when the callback session starts and before NetsyncCallbackFunction
// NetsyncStartFunction should return LPVOID, a pointer to a context structure to be used by NetsyncCallbackFunction
typedef LPVOID (WINAPI *PNETSYNC_START_FUNCTION) (
    IN HANDLE   hSessionObject,       // Handle to the session object
    IN HANDLE   hNetsyncObject,       // Handle to the netsync object for the session
    IN BYTE     byClientCount,        // Specifies the number of clients in the session
    IN u_long   *ClientInAddr,        // Pointer to the array of client ip addresses
    IN XNADDR   *ClientXnAddr,        // Pointer to the array of client xnet addresses
    IN u_short  LowPort,              // Specifies the low port number allocated for the session
    IN u_short  HighPort              // Specifies the high port number allocated for the session
);

// PNETSYNC_CALLBACK_FUNCTION is the main function prototype of a callback session
// NetsyncCallbackFunction should return FALSE when the session is complete or TRUE to continue processing messages
typedef BOOL (WINAPI *PNETSYNC_CALLBACK_FUNCTION) (
    IN HANDLE   hSessionObject,       // Handle to the session object
    IN HANDLE   hNetsyncObject,       // Handle to the netsync object for the session
    IN u_long   FromInAddr,           // Specifies the sender ip address of the message
    IN XNADDR   *FromXnAddr,          // Pointer to the sender xnet address of the message
    IN DWORD    dwMessageType,        // Specifies the message type, either server (NETSYNC_MSGTYPE_SERVER) or client (NETSYNC_MSGTYPE_CLIENT)
    IN DWORD    dwMessageSize,        // Specifies the size of the message, in bytes
    IN char     *pMessage,            // Pointer to the message buffer
    IN LPVOID   lpContext             // Pointer to the context structure returned from NetsyncStartFunction
);

// PNETSYNC_STOP_FUNCTION is the stop function prototype
// NetsyncStopFunction is called once when the callback session ends and after NetsyncCallbackFunction
// NetsyncStopFunction should return when all session resources are freed, including the context structure
typedef VOID (WINAPI *PNETSYNC_STOP_FUNCTION) (
    IN HANDLE   hSessionObject,       // Handle to the session object
    IN HANDLE   hNetsyncObject,       // Handle to the netsync object for the session
    IN LPVOID   lpContext             // Pointer to the context structure returned from NetsyncStartFunction
);



// Session Types

// NETSYNC_SESSION_THREAD is a netsync session that runs in its own thread
#define NETSYNC_SESSION_THREAD        1
// NETSYNC_SESSION_CALLBACK is a netsync session that uses a message callback function
#define NETSYNC_SESSION_CALLBACK      2

typedef struct _NETSYNC_TYPE_THREAD {
    BYTE     byClientCount;           // Specifies the client count for a session
    u_short  nMinPortCount;           // Specifies the minimum port count for a session
    LPWSTR   lpszDllName_W;           // Pointer to a null-terminated string (UNICODE) that specifies the dll name for the netsync session
    LPSTR    lpszMainFunction_A;      // Pointer to a null-terminated string (ANSI) that specifies the main function name
} NETSYNC_TYPE_THREAD, *PNETSYNC_TYPE_THREAD;

typedef struct _NETSYNC_TYPE_CALLBACK {
    BYTE     byMinClientCount;        // Specifies the minimum client count for a session
    BYTE     byMaxClientCount;        // Specifies the maximum client count for a session
    u_short  nMinPortCount;           // Specifies the minimum port count for a session
    LPWSTR   lpszDllName_W;           // Pointer to a null-terminated string (UNICODE) that specifies the dll name for the netsync session
    LPSTR    lpszMainFunction_A;      // Pointer to a null-terminated string (ANSI) that specifies the main function name
    LPSTR    lpszStartFunction_A;     // Pointer to a null-terminated string (ANSI) that specifies the start function name
    LPSTR    lpszStopFunction_A;      // Pointer to a null-terminated string (ANSI) that specifies the stop function name
} NETSYNC_TYPE_CALLBACK, *PNETSYNC_TYPE_CALLBACK;



// Message Types

// NETSYNC_MSGTYPE_SERVER indicates a server message
#define NETSYNC_MSGTYPE_SERVER        0x01
// NETSYNC_MSGTYPE_CLIENT indicates a client message
#define NETSYNC_MSGTYPE_CLIENT        0x02

// NETSYNC_MSG_STOPSESSION is the message id to indicate the session has ended
#define NETSYNC_MSG_STOPSESSION       0x0F1
// NETSYNC_MSG_ADDCLIENT is the message id to indicate a client joined the session
#define NETSYNC_MSG_ADDCLIENT         0x0F2
// NETSYNC_MSG_DELETECLIENT is the message id to indicate a client left the session
#define NETSYNC_MSG_DELETECLIENT      0x0F3
// NETSYNC_MSG_DUPLICATECLIENT is the message id to indicate a client rejoined the session without first leaving the session
#define NETSYNC_MSG_DUPLICATECLIENT   0x0F4

// NETSYNC_MSG_USER is the base message id for user messages
#define NETSYNC_MSG_USER              0x800

typedef struct _NETSYNC_GENERIC_MESSAGE {
    DWORD  dwMessageId;  // Specifies the message id
} NETSYNC_GENERIC_MESSAGE, *PNETSYNC_GENERIC_MESSAGE;



// Function Prototypes

BOOL
WINAPI
NetsyncCreateServer(
);

VOID
WINAPI
NetsyncCloseServer(
);

VOID
WINAPI
NetsyncRemoveClientFromSession(
    IN HANDLE  hSessionObject,
    IN HANDLE  hNetsyncObject,
    IN u_long  ClientAddr
);

HANDLE
WINAPI
NetsyncCreateClient(
    IN  u_long   RemoteInAddr,
    IN  DWORD    dwSessionType,
    IN  LPVOID   pSessionType,
    OUT u_long   *ServerInAddr,
    OUT XNADDR   *ServerXnAddr,
    OUT u_short  *LowPort,
    OUT u_short  *HighPort
);

VOID
WINAPI
NetsyncCloseClient(
    IN HANDLE  hNetsyncObject
);

DWORD
WINAPI
NetsyncSendClientMessage(
    IN HANDLE  hNetsyncObject,
    IN u_long  ToAddr,
    IN DWORD   dwMessageSize,
    IN char    *pMessage
);

DWORD
WINAPI
NetsyncReceiveServerMessage(
    IN  HANDLE  hNetsyncObject,
    IN  DWORD   dwTimeout,
    OUT u_long  *FromInAddr,
    OUT XNADDR  *FromXnAddr,
    OUT DWORD   *dwMessageSize,
    OUT char *  *pMessage
);

DWORD
WINAPI
NetsyncReceiveClientMessage(
    IN  HANDLE  hNetsyncObject,
    IN  DWORD   dwTimeout,
    OUT u_long  *FromInAddr,
    OUT XNADDR  *FromXnAddr,
    OUT DWORD   *dwMessageSize,
    OUT char *  *pMessage
);

DWORD
WINAPI
NetsyncReceiveMessage(
    IN  HANDLE  hNetsyncObject,
    IN  DWORD   dwTimeout,
    OUT DWORD   *dwMessageType,
    OUT u_long  *FromInAddr,
    OUT XNADDR  *FromXnAddr,
    OUT DWORD   *dwMessageSize,
    OUT char *  *pMessage
);

BOOL
WINAPI
NetsyncFreeMessage(
    IN  char *  pMessage
);



// Export Table Definitions

#ifdef _XBOX

typedef struct _NETSYNC_EXPORT_TABLE_ENTRY  {
    LPSTR    lpszFunction_A;  // Pointer to a null-terminated string (ANSI) that specifies the function name
    FARPROC  pFunction;       // Pointer to the function address
} NETSYNC_EXPORT_TABLE_ENTRY, *PNETSYNC_EXPORT_TABLE_ENTRY;

typedef struct _NETSYNC_EXPORT_TABLE_DIRECTORY {
    LPWSTR                       lpszModuleName;  // Pointer to a null-terminated string (UNICODE) that specifies the dll name
    PNETSYNC_EXPORT_TABLE_ENTRY  pTableEntry;     // Pointer to the list of table entries for this dll
} NETSYNC_EXPORT_TABLE_DIRECTORY, *PNETSYNC_EXPORT_TABLE_DIRECTORY;

#ifdef __cplusplus
#define __EXTERN_C extern "C"
#else
#define __EXTERN_C
#endif

#define NETSYNC_EXPORT_SECTION_NAME ".SYNCEXP$N"

#define BEGIN_NETSYNC_EXPORT_TABLE(ModuleName) \
    __EXTERN_C NETSYNC_EXPORT_TABLE_ENTRY  ModuleName##_NetsyncExportTableEntry[] = {

#define NETSYNC_EXPORT_TABLE_ENTRY(FunctionName, Function) \
        { FunctionName, (FARPROC) Function },

#define END_NETSYNC_EXPORT_TABLE(ModuleName) \
        { NULL, NULL } \
    };

#define DECLARE_NETSYNC_EXPORT_DIRECTORY(ModuleName) \
    extern NETSYNC_EXPORT_TABLE_ENTRY          ModuleName##_NetsyncExportTableEntry[]; \
    __EXTERN_C NETSYNC_EXPORT_TABLE_DIRECTORY  ModuleName##_NetsyncExportTableDirectory = { \
        L#ModuleName, \
        ModuleName##_NetsyncExportTableEntry \
    };

#endif // XBOX



#ifdef __cplusplus
}
#endif
