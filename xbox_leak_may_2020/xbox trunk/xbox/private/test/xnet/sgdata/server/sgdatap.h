/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  sgdatap.h

Abstract:

  This module defines private data for sgdata_nt.exe

Author:

  Steven Kehrli (steveke) 24-Jan-2002

------------------------------------------------------------------------------*/

#pragma once



namespace SGDataNamespace {



// Ports

#define PORT_SESSION         65473
#define PORT_UDP             65474
#define PORT_TCP             65474



// Values

#define TIMER_TIMEOUT        15000       // 15 Seconds
#define ACTIVITY_TIMEOUT     1200000000  // 2 Minutes

#define SGDATA_CLASS_NAME    L"SGDataClassName"

#define SGDATA_LOGFILE_NAME  L"sgdata.csv"



// Structures

typedef struct _TEST_OBJECT {
    CRITICAL_SECTION     *pcs;                       // Pointer to the object to synchronize access to the test objects
    HWND                 hWnd;                       // Handle to the parent window
    HANDLE               hMemObject;                 // Handle to the memory object
    HANDLE               hFile;                      // Handle to the log file
    WCHAR                RemoteName[22];             // Specifies the remote name
    SOCKADDR_IN          Session_remotename;         // Specifies the session remote name
    SOCKADDR_IN          Test_remotename;            // Specifies the test remote name
    int                  type;                       // Specifies the type of socket
    SOCKET               sSocket;                    // Specifies the socket descriptor
    u_short              sin_port;                   // Specifies the port
    DWORD                dwUDPBytes;                 // Specifies the number of UDP bytes
    DWORD                dwTCPBytes;                 // Specifies the number of TCP bytes
    SYSTEMTIME           FirstActivityTime;          // Specifies the first time of activity
    SYSTEMTIME           LastActivityTime;           // Specifies the last time of activity
    HANDLE               hStopEvent;                 // Handle to the stop event
    HANDLE               hRecvEvent;                 // Handle to the recv event
    HANDLE               hTestThread;                // Handle to the test thread
    struct _TEST_OBJECT  *pPrevTestObject;           // Pointer to the prev test object
    struct _TEST_OBJECT  *pNextTestObject;           // Pointer to the next test object
} TEST_OBJECT, *PTEST_OBJECT;



typedef struct _SESSION_OBJECT {
    CRITICAL_SECTION     *pcs;                       // Pointer to the object to synchronize access to the test objects
    HWND                 hWnd;                       // Handle to the parent window
    HANDLE               hMemObject;                 // Handle to the memory object
    HANDLE               hFile;                      // Handle to the log file
    SOCKET               sSessionSocket;             // Specifies the session socket descriptor
    u_short              sin_SessionPort;            // Specifies the session port
    SOCKET               sUDPSocket;                 // Specifies the UDP socket descriptor
    u_short              sin_UDPPort;                // Specifies the UDP port
    SOCKET               sTCPSocket;                 // Specifies the TCP socket descriptor
    u_short              sin_TCPPort;                // Specifies the TCP port
    HANDLE               hStopEvent;                 // Handle to the stop event
    HANDLE               hRecvEvent;                 // Handle to the recv event
    HANDLE               hSessionThread;             // Handle to the session thread
    PTEST_OBJECT         pTestObject;                // Pointer to the list of test objects
} SESSION_OBJECT, *PSESSION_OBJECT;



// Dialog messages

#define UM_DIALOG_INITIALIZE  (WM_USER + 1)
#define UM_UPDATE_TEST        (WM_USER + 10)
#define UM_DELETE_TEST        (WM_USER + 11)



// The following enum is used to identify the column indexes

enum eColumnIndex {
    eColumnRemoteName = 0,
    eColumnUDPBytes,
    eColumnTCPBytes,
    eColumnFirstActivityTime,
    eColumnLastActivityTime,
    eColumnIllegal
};



// The following enum is used to identify the sort method

enum eSortMethod {
    eSortRemoteNameAscending = 0,
    eSortRemoteNameDescending,
    eSortUDPBytesAscending,
    eSortUDPBytesDescending,
    eSortTCPBytesAscending,
    eSortTCPBytesDescending,
    eSortFirstActivityTimeAscending,
    eSortFirstActivityTimeDescending,
    eSortLastActivityTimeAscending,
    eSortLastActivityTimeDescending,
    eSortIllegal
};

} // namespace SGDataNamespace
