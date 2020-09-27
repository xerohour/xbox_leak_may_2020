/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  netsyncp.h

Abstract:

  This module contains the private definitions for netsync.dll

Author:

  Steven Kehrli (steveke) 15-Nov-2000

------------------------------------------------------------------------------*/

#pragma once



namespace NetsyncNamespace {

// NETSYNC_OBJECT_TYPE identifies a NETSYNC_OBJECT
#define NETSYNC_OBJECT_TYPE             0x083E546B

// NETSYNC_PORT_LOW_RANGE is the low edge of the port range
#define NETSYNC_PORT_LOW_RANGE          49152
// NETSYNC_PORT_HIGH_RANGE is the high edge of the port range
#define NETSYNC_PORT_HIGH_RANGE         65471
// NETSYNC_PORT_SECTION_SIZE is the number of ports per section in the port range
#define NETSYNC_PORT_SECTION_SIZE       64
// NETSYNC_PORT_SECTIONS is the number of port sections in the port range
#define NETSYNC_PORT_SECTIONS           255

typedef struct _NETSYNC_MESSAGE_HEADER {
    DWORD                         dwMessageId;                         // Specifies the message id
    DWORD                         dwMessageSize;                       // Specifies the message size, in bytes
    XNADDR                        FromXnAddr;                          // Specifies the sender xnet address
    ULONGLONG                     LowSN;                               // Specifies the low field of the sequence number
    ULONGLONG                     HighSN;                              // Specifies the high field of the sequence number
    HANDLE                        hACKEvent;                           // Handle to the ACK event
} NETSYNC_MESSAGE_HEADER, *PNETSYNC_MESSAGE_HEADER;

typedef struct _NETSYNC_MESSAGE_QUEUE {
    DWORD                         dwMessageSize;                       // Specifies the size of the message
    u_long                        FromInAddr;                          // Specifies the sender ip address
    XNADDR                        FromXnAddr;                          // Specifies the sender xnet address
    ULONGLONG                     LowSN;                               // Specifies the low field of the sequence number
    ULONGLONG                     HighSN;                              // Specifies the high field of the sequence number
    char                          *pReceivedMessage;                   // Pointer to the message
    struct _NETSYNC_MESSAGE_QUEUE *pNextQueuedMessage;                 // Pointer to the next message in the queue
} NETSYNC_MESSAGE_QUEUE, *PNETSYNC_MESSAGE_QUEUE;

typedef struct _NETSYNC_CLIENT_OBJECT {
    u_long                        ClientAddr;                          // Specifies the client address
    CRITICAL_SECTION              csSendSN;                            // Object to synchronize access to the next send sequence number
    ULONGLONG                     LowSendSN;                           // Specifies the low field of the send sequence number
    ULONGLONG                     HighSendSN;                          // Specifies the high field of the send sequence number
    CRITICAL_SECTION              csAckSN;                             // Object to synchronize access to the next ack sequence number
    ULONGLONG                     LowAckSN;                            // Specifies the low field of the ack sequence number
    ULONGLONG                     HighAckSN;                           // Specifies the high field of the ack sequence number
    CRITICAL_SECTION              csReceiveSN;                         // Object to synchronize access to the next receive sequence number
    ULONGLONG                     LowReceiveSN;                        // Specifies the low field of the receive sequence number
    ULONGLONG                     HighReceiveSN;                       // Specifies the high field of the receive sequence number
    struct _NETSYNC_CLIENT_OBJECT *pNextClient;                        // Pointer to the next client in the list
} NETSYNC_CLIENT_OBJECT, *PNETSYNC_CLIENT_OBJECT;

typedef struct _NETSYNC_OBJECT {
    DWORD                         dwObjectType;                        // Specifies the object type
    HANDLE                        hMemObject;                          // Specified the memory object for this object
    UUID                          ObjectUUID;                          // Specifies the unique identifier of this object
    u_long                        InAddr;                              // Specifies the ip address
    XNADDR                        XnAddr;                              // Specifies the xnet address
    XNKID                         XnKid;                               // Specifies the xnet session key id
    XNKEY                         XnKey;                               // Specifies the xnet session key
    BOOL                          bXnKey;                              // Specifies if xnet session key id and key are registered
    u_long                        ServerAddr;                          // Specifies the server address
    CRITICAL_SECTION              csClientList;                        // Object to synchronize access to the client list
    PNETSYNC_CLIENT_OBJECT        pClientList;                         // Pointer to the head of the client list
    CRITICAL_SECTION              csSocket;                            // Object to synchronize access to the socket
    SOCKET                        sSocket;                             // A socket descriptor to communicate with all clients in the session
    u_short                       NetsyncPort;                         // Specifies the port used by the socket for netsync communication
    HANDLE                        hReceiveEvent;                       // Handle to an event to signal receipt of a message
    HANDLE                        hReceiveThread;                      // Handle to the receive thread
    CRITICAL_SECTION              csServerQueue;                       // Object to synchronize access to the server message queue
    HANDLE                        hServerSemaphore;                    // Object to synchronize access to the server message queue
    PNETSYNC_MESSAGE_QUEUE        pHeadServerQueue;                    // Pointer to the head of the server message queue
    PNETSYNC_MESSAGE_QUEUE        pTailServerQueue;                    // Pointer to the tail of the server message queue
    CRITICAL_SECTION              csClientQueue;                       // Object to synchronize access to the client message queue
    HANDLE                        hClientSemaphore;                    // Object to synchronize access to the client message queue
    PNETSYNC_MESSAGE_QUEUE        pHeadClientQueue;                    // Pointer to the head of the client message queue
    PNETSYNC_MESSAGE_QUEUE        pTailClientQueue;                    // Pointer to the head of the client message queue
} NETSYNC_OBJECT, *PNETSYNC_OBJECT;

typedef struct _NETSYNC_SESSION {
    CRITICAL_SECTION              cs;                                  // Object to synchronize access to this session
    DWORD                         dwSessionStatus;                     // Specifies the session status (pending or running)
    BOOL                          bStopSession;                        // Specifies if the session thread should exit
    PNETSYNC_OBJECT               pNetsyncObject;                      // Pointer to the NETSYNC_OBJECT
    HANDLE                        hThread;                             // Handle to the session thread
    struct _NETSYNC_SESSION_INFO  *pSessionInfo;                       // Pointer to the session info
    BYTE                          byClientCount;                       // Specifies the number of clients in the session
    u_long                        *ClientInAddrs;                      // Pointer to an array of ip addresses of the clients in the session
    XNADDR                        *ClientXnAddrs;                      // Pointer to an array of xnet addresses of the clients in the session
    BYTE                          byPortSection;                       // Specifies the base port section allocated for the session
    u_short                       LowPort;                             // Specifies the low port allocated for the session
    u_short                       HighPort;                            // Specifies the high port allocated for the session
    struct _NETSYNC_SESSION       *pNextSession;                       // Pointer to the next session in the list
} NETSYNC_SESSION, *PNETSYNC_SESSION;

typedef struct _NETSYNC_SESSION_INFO {
    DWORD                         dwSessionType;                       // Specifies the session type
    BYTE                          byMinClientCount;                    // Specifies the minimum client count for a session
    BYTE                          byMaxClientCount;                    // Specifies the maximum client count for a session
    u_short                       nMinPortCount;                       // Specifies the minimum port count for a session
    LPSTR                         lpszMainFunction_A;                  // Pointer to a null-terminated string (ANSI) that specifies the main function name
    PNETSYNC_THREAD_FUNCTION      pThreadFunction;                     // Pointer to the main thread function
    PNETSYNC_CALLBACK_FUNCTION    pCallbackFunction;                   // Pointer to the main callback function
    LPSTR                         lpszStartFunction_A;                 // Pointer to a null-terminated string (ANSI) that specifies the start function name
    PNETSYNC_START_FUNCTION       pStartFunction;                      // Pointer to the start function
    LPSTR                         lpszStopFunction_A;                  // Pointer to a null-terminated string (ANSI) that specifies the stop function name
    PNETSYNC_STOP_FUNCTION        pStopFunction;                       // Pointer to the stop function
    PNETSYNC_SESSION              pActiveSession;                      // Pointer to the active session
    PNETSYNC_SESSION              pRunningSessions;                    // Pointer to the list of running sessions
    struct _NETSYNC_SESSION_INFO  *pNextSessionInfo;                   // Pointer to the next session info in the list
} NETSYNC_SESSION_INFO, *PNETSYNC_SESSION_INFO;

typedef struct _NETSYNC_SESSION_DLL {
    LPWSTR                        lpszDllName_W;                       // Pointer to a null-terminated string (UNICODE) that specifies the dll name
    HMODULE                       hModule;                             // Handle to the dll module
    PNETSYNC_SESSION_INFO         pSessionInfos;                       // Pointer to the list of session infos for this dll
    struct _NETSYNC_SESSION_DLL   *pNextSessionDll;                    // Pointer to the next session dll in the list
} NETSYNC_SESSION_DLL, *PNETSYNC_SESSION_DLL;

typedef struct _NETSYNC_SERVER_INFO {
    HANDLE                        hMemObject;                          // Specified the memory object for this object
    DWORD                         dwRefCount;                          // Specifies the reference count of this object
    PNETSYNC_OBJECT               pNetsyncObject;                      // Pointer to the NETSYNC_OBJECT
    HANDLE                        hThread;                             // Handle to the server thread
    u_short                       nServerClientCount;                  // Specifies the number of clients connected to this server
    BYTE                          bySessionCount;                      // Specifies the number of sessions running on the server
    PNETSYNC_SESSION_DLL          pSessionDlls;                        // Pointer to the list of session dlls for this server
    BOOL                          bStopServer;                         // Specifies if the server is stopping
    HANDLE                        hStopEvent;                          // Handle to the stop event
    BOOL                          bPortInUse[NETSYNC_PORT_SECTIONS];   // An array of values that specifies whether the port section is in use
    BOOL                          bPortUsed[NETSYNC_PORT_SECTIONS];    // An array of values that specifies whether the port section has been used recently
} NETSYNC_SERVER_INFO, *PNETSYNC_SERVER_INFO;

typedef struct _NETSYNC_CLIENT_INFO {
    HANDLE                        hMemObject;                          // Specified the memory object for this object
    DWORD                         dwRefCount;                          // Specifies the reference count of this object
} NETSYNC_CLIENT_INFO, *PNETSYNC_CLIENT_INFO;

typedef struct _NETSYNC_SERVER {
    u_long                        InAddr;                              // Specifies the ip address
    XNADDR                        XnAddr;                              // Specifies the xnet address
    XNKID                         XnKid;                               // Specifies the xnet session key id
    XNKEY                         XnKey;                               // Specifies the xnet session key
    BYTE                          bySessionClientCount;                // Specifies the number of clients in the session
    u_short                       nServerClientCount;                  // Specifies the number of clients connected to the server
    BYTE                          bySessionCount;                      // Specifies the number of sessions running on the server
    struct _NETSYNC_SERVER        *pNextServer;                        // Pointer to the next server in the list
} NETSYNC_SERVER, *PNETSYNC_SERVER;

typedef struct _NETSYNC_SESSION_MESSAGE {
    DWORD                         dwMessageId;                         // Specifies the message id
    UUID                          FromObjectUUID;                      // Specifies the unique identifier of the sender
    DWORD                         dwSessionType;                       // Specifies the session type
    BYTE                          byMinClientCount;                    // Specifies the minimum client count for a session
    BYTE                          byMaxClientCount;                    // Specifies the maximum client count for a session
    u_short                       nMinPortCount;                       // Specifies the minimum port count for a session
    WCHAR                         szDllName_W[128];                    // Specifies the dll name
    CHAR                          szMainFunction_A[128];               // Specifies the main function name
    CHAR                          szStartFunction_A[128];              // Specifies the start function name
    CHAR                          szStopFunction_A[128];               // Specifies the stop function name
} NETSYNC_SESSION_MESSAGE, *PNETSYNC_SESSION_MESSAGE;

typedef struct _NETSYNC_ENUM_REPLY {
    DWORD                         dwMessageId;                         // Specifies the message id
    UUID                          FromObjectUUID;                      // Specifies the unique identifier of the sender
    u_long                        InAddr;                              // Specifies the ip address
    XNKID                         XnKid;                               // Specifies the xnet session key id
    XNKEY                         XnKey;                               // Specifies the xnet session key
    BYTE                          bySessionClientCount;                // Specifies the number of clients in the session
    u_short                       nServerClientCount;                  // Specifies the number of clients connected to the server
    BYTE                          bySessionCount;                      // Specifies the number of sessions running on the server
} NETSYNC_ENUM_REPLY, *PNETSYNC_ENUM_REPLY;

typedef struct _NETSYNC_CONNECT_REPLY {
    DWORD                         dwMessageId;                         // Specifies the message id
    DWORD                         dwErrorCode;                         // Specifies the error code if the connect failed
    u_short                       LowPort;                             // Specifies the low port allocated for the session
    u_short                       HighPort;                            // Specifies the high port allocated for the session
} NETSYNC_CONNECT_REPLY, *PNETSYNC_CONNECT_REPLY;

typedef struct _NETSYNC_CLIENT_MESSAGE {
    DWORD                         dwMessageId;                         // Specifies the message id
    u_long                        FromInAddr;                          // Specifies the source ip address
    XNADDR                        FromXnAddr;                          // Specifies the source xnet address
} NETSYNC_CLIENT_MESSAGE, *PNETSYNC_CLIENT_MESSAGE;



// NETSYNC_PORT is the socket port for initial communication between client and server
#define NETSYNC_PORT                    65472

// SESSION_PENDING indicates the session is pending
#define SESSION_PENDING                 0
// SESSION_RUNNING indicates the session is running
#define SESSION_RUNNING                 1

// NETSYNC_MSG_ACK is the message id to indicate an ack
#define NETSYNC_MSG_ACK                 0x11

// NETSYNC_MSG_ENUM is the message id of a NETSYNC_SESSION_MSG struct
#define NETSYNC_MSG_ENUM                0x21
// NETSYNC_REPLY_ENUM is the message id of the NETSYNC_ENUM_REPLY struct
#define NETSYNC_REPLY_ENUM              0x22
// NETSYNC_MSG_CONNECT is the message id of a NETSYNC_SESSION_MSG struct
#define NETSYNC_MSG_CONNECT             0x23
// NETSYNC_REPLY_CONNECT is the message id of the NETSYNC_CONNECT_REPLY struct
#define NETSYNC_REPLY_CONNECT           0x24
// NETSYNC_MSG_STOPRECEIVE is the message id to indicate the receive thread is to exit
#define NETSYNC_MSG_STOPRECEIVE         0x25



// Function prototypes

typedef BOOL (WINAPI *LPDLLMAIN_PROC) (HINSTANCE, DWORD, PVOID);

PNETSYNC_OBJECT
CreateNetsyncObject(
    IN HANDLE   hMemObject,
    IN u_long   ServerAddr,
    IN u_short  NetsyncPort
);

VOID
CloseNetsyncObject(
    IN PNETSYNC_OBJECT  pNetsyncObject
);

PNETSYNC_CLIENT_OBJECT
FindClientObject(
    IN PNETSYNC_OBJECT  pNetsyncObject,
    IN u_long           ClientAddr
);

DWORD
NetsyncSendMessageToPort(
    IN HANDLE   hNetsyncObject,
    IN u_long   ToAddr,
    IN u_short  ToPort,
    IN BOOL     bGuarantee,
    IN DWORD    dwMessageId,
    IN DWORD    dwMessageSize,
    IN char     *pMessage
);

DWORD
NetsyncSendServerMessage(
    IN HANDLE   hNetsyncObject,
    IN u_long   ToAddr,
    IN u_short  ToPort,
    IN BOOL     bGuarantee,
    IN DWORD    dwMessageSize,
    IN char     *pMessage
);

DWORD
ReceiveThread(
    IN LPVOID  lpv
);

} // namespace Netsyncnamespace
