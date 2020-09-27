/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  sgdata.c

Abstract:

  This modules tests SG Data Validation - library

Author:

  Steven Kehrli (steveke) 5-Feb-2002

------------------------------------------------------------------------------*/

#include "precomp.h"



using namespace SGDataNamespace;

namespace SGDataNamespace {



ULONG
AddRefNet(
    IN HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Initializes the online subsystem

Arguments:

  hLog - Handle to the xLog log object

Return Value:

  ULONG:
    If the function succeeds, the return value is a non-zero
    If the function fails, the return value is 0.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // iLastError is the last error code
    int     iLastError = ERROR_SUCCESS;
    // uXOnlineRef is the reference count of xonline
    ULONG   uXOnlineRef = 0;
    // XnAddr is the title xnet address
    XNADDR  XnAddr;
    // dwResult is the result of XNetGetTitleXnAddr
    DWORD   dwResult = 0;



    // Start the variation
    xStartVariation(hLog, "SG Data Net Stack");

    // Initialize the online subsystem
    uXOnlineRef = XOnlineAddRef();
    if (0 == uXOnlineRef) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "XOnlineAddRef failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    while (0 == (dwResult = XNetGetTitleXnAddr(&XnAddr))) {
        Sleep(1000);
    }

    xLog(hLog, XLL_PASS, "Net Stack passed");

FunctionExit:
    // End the variation
    xEndVariation(hLog);

    if (ERROR_SUCCESS != iLastError) {
        SetLastError(iLastError);
    }

    return uXOnlineRef;
}



VOID
ReleaseNet(
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Terminates the online subsystem

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Terminate the online subsystem
    XOnlineRelease();
}



BOOL
SGUsers(
    IN     HANDLE         hLog,
    IN     PXONLINE_USER  pOnlineUsers,
    IN OUT LPDWORD        lpdwNumUsers
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets the online users

Arguments:

  hLog - Handle to the xLog log object
  pOnlineUsers - Pointer to the array of online users
  dwNumUsers - Pointer to the number of online users

Return Value:

  TRUE on success

------------------------------------------------------------------------------*/
{
    // iLastError is the last error code
    int           iLastError = ERROR_SUCCESS;
    // hResult is the return code of a function
    HRESULT       hResult;

    // OnlineAllUsers is the array of all online user accounts
    XONLINE_USER  OnlineAllUsers[XONLINE_MAX_STORED_ONLINE_USERS];



    // Start the variation
    xStartVariation(hLog, "SG Data Users");

    // Initialize the users
    ZeroMemory(pOnlineUsers, *lpdwNumUsers * sizeof(XONLINE_USER));

    // Populate user accounts
    hResult = COnlineAccounts::Instance()->PopulateUserAccountsHD(NULL, *lpdwNumUsers);
    if (FAILED(hResult)) {
        iLastError = HRESULT_CODE(hResult);
        xLog(hLog, XLL_FAIL, "PopulateUserAccountsHD failed - hResult: 0x%08x - ec: %u", hResult, iLastError);

        goto FunctionExit;
    }

    // Get the user accounts
    ZeroMemory(&OnlineAllUsers, sizeof(OnlineAllUsers));
    hResult = XOnlineGetUsers(OnlineAllUsers, lpdwNumUsers);
    if (FAILED(hResult)) {
        iLastError = HRESULT_CODE(hResult);
        xLog(hLog, XLL_FAIL, "XOnlineGetUsers failed - hResult: 0x%08x - ec: %u", hResult, iLastError);

        goto FunctionExit;
    }

    if (0 == *lpdwNumUsers) {
        hResult = XONLINE_E_NO_USER;
        iLastError = HRESULT_CODE(hResult);
        xLog(hLog, XLL_FAIL, "No Users");

        goto FunctionExit;
    }

    // Copy the user accounts
    *lpdwNumUsers = __min(*lpdwNumUsers, XONLINE_MAX_LOGON_USERS);
    CopyMemory(pOnlineUsers, &OnlineAllUsers, *lpdwNumUsers * sizeof(XONLINE_USER));

    xLog(hLog, XLL_PASS, "Users passed");

FunctionExit:
    // End the variation
    xEndVariation(hLog);

    if (ERROR_SUCCESS != iLastError) {
        SetLastError(iLastError);
    }

    return (ERROR_SUCCESS == iLastError);
}



XONLINETASK_HANDLE
SGLogon(
    IN HANDLE        hLog,
    IN XONLINE_USER  *pOnlineUsers,
    IN DWORD         dwNumUsers
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Connects to the SG Data Validation Service

Arguments:

  hLog - Handle to the xLog log object
  pOnlineUsers - Pointer to the array of online users

Return Value:

  XONLINETASK_HANDLE: Online task handle

------------------------------------------------------------------------------*/
{
    // iLastError is the last error code
    int                 iLastError = ERROR_SUCCESS;
    // hResult is the return code of a function
    HRESULT             hResult;

    // OnlineLogonUsers is the array of all online user accounts
    XONLINE_USER        OnlineLogonUsers[XONLINE_MAX_LOGON_USERS];
    // dwOnlineServices is the array of online services
    DWORD               dwOnlineServices[] = { XONLINE_SG_DATA_ENUM_SERVICE, XONLINE_SG_DATA_TEST_SERVICE };
    // dwNumServices is the number of online services
    DWORD               dwNumServices = sizeof(dwOnlineServices) / sizeof(dwOnlineServices[0]);

    // hOnlineLogonEvent is the handle to the XOnlineLogon work event
    HANDLE              hOnlineLogonEvent = NULL;
    // OnlineLogonHandle is the pointer to the XOnlineLogon task handle
    XONLINETASK_HANDLE  OnlineLogonHandle = NULL;



    // Start the variation
    xStartVariation(hLog, "SG Data Logon");

    // Copy the users
    ZeroMemory(&OnlineLogonUsers, sizeof(OnlineLogonUsers));
    memcpy(&OnlineLogonUsers, pOnlineUsers, __min(dwNumUsers * sizeof(XONLINE_USER), sizeof(OnlineLogonUsers)));

    // Create the XOnlineLogon work event
    hOnlineLogonEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == hOnlineLogonEvent) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "hOnlineLogonEvent failed - ec: %u ", iLastError);

        goto FunctionExit;
    }

    // Logon to the services
    hResult = XOnlineLogon((PXONLINE_USER) &OnlineLogonUsers, dwOnlineServices, dwNumServices, hOnlineLogonEvent, &OnlineLogonHandle);
    if (FAILED(hResult)) {
        iLastError = HRESULT_CODE(hResult);
        xLog(hLog, XLL_FAIL, "XOnlineLogon failed - hResult: 0x%08x - ec: %u", hResult, iLastError);

        goto FunctionExit;
    }

    do {
        WaitForSingleObject(hOnlineLogonEvent, INFINITE);

        hResult = XOnlineTaskContinue(OnlineLogonHandle);
    } while (XONLINETASK_S_RUNNING == hResult);

    if (XONLINE_S_LOGON_CONNECTION_ESTABLISHED != hResult) {
        iLastError = HRESULT_CODE(hResult);
        xLog(hLog, XLL_FAIL, "XOnlineTaskContinue (Logon) failed - hResult: 0x%08x - ec: %u", hResult, iLastError);

        goto FunctionExit;
    }

    // Get the logon results
    hResult = XOnlineLogonTaskGetResults(OnlineLogonHandle);
    if (FAILED(hResult)) {
        iLastError = HRESULT_CODE(hResult);
        xLog(hLog, XLL_FAIL, "XOnlineLogonTaskGetResults failed - hResult: 0x%08x - ec: %u", hResult, iLastError);

        goto FunctionExit;
    }

    xLog(hLog, XLL_PASS, "Logon passed");

FunctionExit:
    if (ERROR_SUCCESS != iLastError) {
        if (NULL != OnlineLogonHandle) {
            XOnlineTaskClose(OnlineLogonHandle);
            OnlineLogonHandle = NULL;
        }

    }

    if (NULL != hOnlineLogonEvent) {
        CloseHandle(hOnlineLogonEvent);
    }

    // End the variation
    xEndVariation(hLog);

    if (ERROR_SUCCESS != iLastError) {
        SetLastError(iLastError);
    }

    return OnlineLogonHandle;
}



SOCKET
CreateSocket(
    IN int     type,
    IN u_short sin_port
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Creates and binds a socket

Arguments:

  type - Specifies the type of socket (SOCK_DGRAM or SOCK_STREAM)
  sin_port - Specifies the bind port of socket

Return Value:

  SOCKET:
    If the function succeeds, the return value is a socket descriptor
    If the function fails, the return value is INVALID_SOCKET.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // sSocket is the new socket
    SOCKET       sSocket = INVALID_SOCKET;
    // iTimeout is the timeout value of the socket
    int          iTimeout;
    // localname is the local address
    SOCKADDR_IN  localname;
    // dwErrorCode is the last error code
    DWORD        dwErrorCode = ERROR_SUCCESS;



    // Create the socket
    sSocket = socket(AF_INET, type, 0);
    if (INVALID_SOCKET == sSocket) {
        // Get the last error code
        dwErrorCode = WSAGetLastError();

        goto FunctionExit;
    }

    // Set the socket timeouts
    iTimeout = 15000;
    if (SOCKET_ERROR == setsockopt(sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    iTimeout = 15000;
    if (SOCKET_ERROR == setsockopt(sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout))) {
        dwErrorCode = GetLastError();

        goto FunctionExit;
    }

    // Setup the local address
    ZeroMemory(&localname, sizeof(localname));
    localname.sin_family = AF_INET;
    localname.sin_port = htons(sin_port);

    // Bind the socket
    if (SOCKET_ERROR == bind(sSocket, (SOCKADDR *) &localname, sizeof(localname))) {
        // Get the last error code
        dwErrorCode = WSAGetLastError();

        goto FunctionExit;
    }

FunctionExit:
    if (ERROR_SUCCESS != dwErrorCode) {
        if (INVALID_SOCKET != sSocket) {
            closesocket(sSocket);
            sSocket = INVALID_SOCKET;
        }

        SetLastError(dwErrorCode);
    }

    return sSocket;
}



VOID
SGClose(
    IN PTEST_OBJECT  pTestObject
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Closes the test object

Arguments:

  pTestObject - Pointer to the test object

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // hMemObject is the handle to the memory object
    HANDLE  hMemObject = pTestObject->hMemObject;



    // Close the session socket
    if (INVALID_SOCKET != pTestObject->sSessionSocket) {
        shutdown(pTestObject->sSessionSocket, SD_BOTH);
        closesocket(pTestObject->sSessionSocket);
    }

    // Close the UDP socket
    if (INVALID_SOCKET != pTestObject->sUDPSocket) {
        shutdown(pTestObject->sUDPSocket, SD_BOTH);
        closesocket(pTestObject->sUDPSocket);
    }

    // Close the TCP socket
    if (INVALID_SOCKET != pTestObject->sTCPSocket) {
        shutdown(pTestObject->sTCPSocket, SD_BOTH);
        closesocket(pTestObject->sTCPSocket);
    }

    // Close the test object
    xMemFree(hMemObject, pTestObject);

    // Close the memory object
    xMemClose(hMemObject);
}



PTEST_OBJECT
SGOpen(
    IN HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Opens the test object

Arguments:

  hLog - Handle to the xLog log object

Return Value:

  PTEST_OBJECT - Pointer to the test object

------------------------------------------------------------------------------*/
{
    // iLastError is the last error code
    int           iLastError = ERROR_SUCCESS;

    // hMemObject is the handle to the memory object
    HANDLE        hMemObject = INVALID_HANDLE_VALUE;
    // pTestObject is the pointer to the test object
    PTEST_OBJECT  pTestObject = NULL;
    // sin_port is the port
    u_short       sin_port = 0;



    // Create the memory object
    hMemObject = xMemCreate();
    if (INVALID_HANDLE_VALUE == hMemObject) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "xMemCreate failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Create the test object
    pTestObject = (PTEST_OBJECT) xMemAlloc(hMemObject, sizeof(TEST_OBJECT));
    if (NULL == pTestObject) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "pTestObject failed - ec = %u", iLastError);

        // Close the memory object
        xMemClose(hMemObject);

        goto FunctionExit;
    }

    // Set the memory object
    pTestObject->hMemObject = hMemObject;

    // Generate a random port
    XNetRandom((BYTE *) &sin_port, sizeof(sin_port));

    // Create the session socket
    pTestObject->sSessionSocket = CreateSocket(SOCK_DGRAM, sin_port);
    if (INVALID_SOCKET == pTestObject->sSessionSocket) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "sSessionSocket failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Create the UDP socket
    pTestObject->sUDPSocket = CreateSocket(SOCK_DGRAM, sin_port + 1);
    if (INVALID_SOCKET == pTestObject->sUDPSocket) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "sUDPSocket failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Create the TCP socket
    pTestObject->sTCPSocket = CreateSocket(SOCK_STREAM, sin_port + 1);
    if (INVALID_SOCKET == pTestObject->sTCPSocket) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "sTCPSocket failed - ec: %u", iLastError);

        goto FunctionExit;
    }

FunctionExit:
    if (ERROR_SUCCESS != iLastError) {
        SGClose(pTestObject);
        pTestObject = NULL;
    }

    if (ERROR_SUCCESS != iLastError) {
        SetLastError(iLastError);
    }

    return pTestObject;
}



PTEST_OBJECT
SGConnect(
    IN HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Connects to the SG Data Validation Service

Arguments:

  hLog - Handle to the xLog log object

Return Value:

  SOCKET - Socket descriptor

------------------------------------------------------------------------------*/
{
    // iLastError is the last error code
    int                   iLastError = ERROR_SUCCESS;
    // hResult is the return code of a function
    HRESULT               hResult;
    // iResult is the return code of a function
    int                   iResult;

    // pTestObject is the pointer to the test object
    PTEST_OBJECT          pTestObject = NULL;

    // OnlineServiceInfo is the online service info
    XONLINE_SERVICE_INFO  OnlineServiceInfo;
    // SGEnumName is the host name of the sg enum service
    SOCKADDR_IN           SGEnumName;

    // SessionMessage is the session message
    SGDATA_MESSAGE        SGDataMessage;



    // Start the variation
    xStartVariation(hLog, "SG Data Connect");

    // Create the test object
    pTestObject = SGOpen(hLog);
    if (NULL == pTestObject) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "pTestObject failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Get the service info
    hResult = XOnlineGetServiceInfo(XONLINE_SG_DATA_ENUM_SERVICE, &OnlineServiceInfo);
    if (FAILED(hResult)) {
        iLastError = HRESULT_CODE(hResult);
        xLog(hLog, XLL_FAIL, "XOnlineGetServiceInfo failed - hResult: 0x%08x - ec: %u", hResult, iLastError);

        goto FunctionExit;
    }

    // Set the enum name
    ZeroMemory(&SGEnumName, sizeof(SGEnumName));
    SGEnumName.sin_family = AF_INET;
    SGEnumName.sin_addr.s_addr = OnlineServiceInfo.serviceIP.s_addr;
    SGEnumName.sin_port = htons(OnlineServiceInfo.wServicePort);

    // Connect the session socket
    iResult = connect(pTestObject->sSessionSocket, (SOCKADDR *) &SGEnumName, sizeof(SGEnumName));
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "connect failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Set the session message
    ZeroMemory(&SGDataMessage, sizeof(SGDataMessage));
    SGDataMessage.dwMessageId = SGDATA_ENUM_MSG;

    // Send the message
    iResult = send(pTestObject->sSessionSocket, (char *) &SGDataMessage, sizeof(SGDataMessage), 0);
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "send failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Receive the reply
    iResult = recv(pTestObject->sSessionSocket, (char *) &SGDataMessage, sizeof(SGDataMessage), 0);
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "recv failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Check the reply
    if (SGDATA_ENUM_REPLY != SGDataMessage.dwMessageId) {
        iLastError = SGDataMessage.dwErrorCode;
        xLog(hLog, XLL_FAIL, "Enum failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    xLog(hLog, XLL_PASS, "Enum passed");

    // Get the service info
    hResult = XOnlineGetServiceInfo(XONLINE_SG_DATA_TEST_SERVICE, &OnlineServiceInfo);
    if (FAILED(hResult)) {
        iLastError = HRESULT_CODE(hResult);
        xLog(hLog, XLL_FAIL, "XOnlineGetServiceInfo failed - hResult: 0x%08x - ec: %u", hResult, iLastError);

        goto FunctionExit;
    }

    // Set the enum name
    ZeroMemory(&SGEnumName, sizeof(SGEnumName));
    SGEnumName.sin_family = AF_INET;
    SGEnumName.sin_addr.s_addr = OnlineServiceInfo.serviceIP.s_addr;
    SGEnumName.sin_port = htons(OnlineServiceInfo.wServicePort);

    // Connect the UDP socket
    iResult = connect(pTestObject->sUDPSocket, (SOCKADDR *) &SGEnumName, sizeof(SGEnumName));
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "connect failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Connect the TCP socket
    iResult = connect(pTestObject->sTCPSocket, (SOCKADDR *) &SGEnumName, sizeof(SGEnumName));
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "connect failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Set the session message
    ZeroMemory(&SGDataMessage, sizeof(SGDataMessage));
    SGDataMessage.dwMessageId = SGDATA_CONNECT_MSG;

    // Send the message
    iResult = send(pTestObject->sSessionSocket, (char *) &SGDataMessage, sizeof(SGDataMessage), 0);
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "send failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Receive the reply
    iResult = recv(pTestObject->sSessionSocket, (char *) &SGDataMessage, sizeof(SGDataMessage), 0);
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "recv failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Check the reply
    if (SGDATA_CONNECT_SUCCESS != SGDataMessage.dwMessageId) {
        iLastError = SGDataMessage.dwErrorCode;
        xLog(hLog, XLL_FAIL, "Connect failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    xLog(hLog, XLL_PASS, "Connect passed");

FunctionExit:
    if (ERROR_SUCCESS != iLastError) {
        SGClose(pTestObject);
        pTestObject = NULL;
    }

    // End the variation
    xEndVariation(hLog);

    if (ERROR_SUCCESS != iLastError) {
        SetLastError(iLastError);
    }

    return pTestObject;
}



VOID
SGDisconnect(
    IN HANDLE        hLog,
    IN PTEST_OBJECT  pTestObject
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Disconnects from the SG Data Validation Service

Arguments:

  hLog - Handle to the xLog log object,
  pTestObject - Pointer to the test object

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // iLastError is the last error code
    int             iLastError = ERROR_SUCCESS;
    // iResult is the return code of a function
    int             iResult;

    // SGDataMessage is the session message
    SGDATA_MESSAGE  SGDataMessage;



    // Start the variation
    xStartVariation(hLog, "SG Data Disconnect");

    // Set the session message
    ZeroMemory(&SGDataMessage, sizeof(SGDataMessage));
    SGDataMessage.dwMessageId = SGDATA_DISCONNECT_MSG;

    // Send the message
    iResult = send(pTestObject->sSessionSocket, (char *) &SGDataMessage, sizeof(SGDataMessage), 0);
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "send failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Receive the reply
    iResult = recv(pTestObject->sSessionSocket, (char *) &SGDataMessage, sizeof(SGDataMessage), 0);
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "recv failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    xLog(hLog, XLL_PASS, "Disconnect passed");

FunctionExit:
    // Close the test object
    SGClose(pTestObject);

    // End the variation
    xEndVariation(hLog);

    if (ERROR_SUCCESS != iLastError) {
        SetLastError(iLastError);
    }
}



BOOL
SGTest(
    IN HANDLE        hLog,
    IN PTEST_OBJECT  pTestObject,
    IN int           type,
    IN int           len
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests SG Data Validation

Arguments:

  hLog - Handle to the xLog log object
  pTestObject - Pointer to the test object
  type - Specifies the type of socket (SOCK_DGRAM or SOCK_STREAM)
  len - Specifies the data length

Return Value:

  BOOL:
    If the function succeeds, the return value is a non-zero
    If the function fails, the return value is 0.  To get extended error information, call GetLastError().

------------------------------------------------------------------------------*/
{
    // iLastError is the last error code
    int     iLastError = ERROR_SUCCESS;
    // iResult is the return code of a function
    int     iResult;

    // sTestSocket is the test socket
    SOCKET  sTestSocket = INVALID_SOCKET;

    // Data Buffers
    CHAR    SendMessage[1500];
    CHAR    SendMaskMessage[1500];
    CHAR    RecvMessage[1500];
    CHAR    RecvMaskMessage[1500];


    // szVariationName is the variation name
    CHAR    szVariationName[LOG_STRING_LENGTH];



    // Set the variation name
    sprintf(szVariationName, "SG Test %s %d", (SOCK_DGRAM == type) ? "UDP" : "TCP", len);
    xStartVariation(hLog, szVariationName);

    // Get the test socket
    if (SOCK_DGRAM == type) {
        sTestSocket = pTestObject->sUDPSocket;
    }
    else {
        sTestSocket = pTestObject->sTCPSocket;
    }

    // Initialize data
    ZeroMemory(SendMessage, sizeof(SendMessage));
    ZeroMemory(SendMaskMessage, sizeof(SendMaskMessage));
    ZeroMemory(RecvMessage, sizeof(RecvMessage));
    ZeroMemory(RecvMaskMessage, sizeof(RecvMaskMessage));

    // Generate data
    XNetRandom((BYTE *) SendMessage, len);

    CopyMemory(SendMaskMessage, SendMessage, len);
    DataMask((BYTE *) SendMaskMessage, len);

    // Send data
    iResult = send(sTestSocket, SendMessage, len, 0);
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "send failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    // Recv data
    iResult = recv(sTestSocket, RecvMessage, sizeof(RecvMessage), 0);
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "recv failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    if (len != iResult) {
        iLastError = HRESULT_CODE(E_UNEXPECTED);
        xLog(hLog, XLL_FAIL, "Data len diff - EXPECTED: %u; RECEIVED: %u", len, iResult);
    }

    if (0 != memcmp(SendMessage, RecvMessage, len)) {
        iLastError = HRESULT_CODE(E_UNEXPECTED);
        xLog(hLog, XLL_FAIL, "Data diff");
    }

    // Recv data mask
    iResult = recv(sTestSocket, RecvMaskMessage, sizeof(RecvMaskMessage), 0);
    if (SOCKET_ERROR == iResult) {
        iLastError = GetLastError();
        xLog(hLog, XLL_FAIL, "recv mask failed - ec: %u", iLastError);

        goto FunctionExit;
    }

    if (len != iResult) {
        iLastError = HRESULT_CODE(E_UNEXPECTED);
        xLog(hLog, XLL_FAIL, "Data Mask len diff - EXPECTED: %u; RECEIVED: %u", len, iResult);
    }

    if (0 != memcmp(SendMaskMessage, RecvMaskMessage, len)) {
        iLastError = HRESULT_CODE(E_UNEXPECTED);
        xLog(hLog, XLL_FAIL, "Data Mask diff");
    }

    if (ERROR_SUCCESS == iLastError) {
        // Test succeeded
        xLog(hLog, XLL_PASS, "Test passed");
    }

FunctionExit:
    // End the variation
    xEndVariation(hLog);

    if (ERROR_SUCCESS != iLastError) {
        SetLastError(iLastError);
    }

    return (ERROR_SUCCESS == iLastError);
}



BOOL
SGDataTest(
    IN HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Tests SG Data Validation

Arguments:

  hLog - Handle to the xLog log object

Return Value:

  TRUE on success

------------------------------------------------------------------------------*/
{
    // iLastError is the last error code
    int                 iLastError = ERROR_SUCCESS;

    // uXOnlineRef is the reference count of xonline
    ULONG               uXOnlineRef = 0;
    // OnlineLogonUsers is the array of logon online user accounts
    XONLINE_USER        OnlineUsers[XONLINE_MAX_LOGON_USERS];
    // dwNumUsers is the number of online users
    DWORD               dwNumUsers = XONLINE_MAX_LOGON_USERS;
    // OnlineLogonHandle is the pointer to the XOnlineLogon task handle
    XONLINETASK_HANDLE  OnlineLogonHandle = NULL;

    // pTestObject is the pointer to the test object
    PTEST_OBJECT        pTestObject = NULL;



    // Set the component and subcomponent
    xSetComponent(hLog, "Online", "Security Gateway");

    // Set the function name
    xSetFunctionName(hLog, "SG Data");

    // Start the variation
    xStartVariation(hLog, "SG Data Test");

    // Initialize the online subsystem
    uXOnlineRef = AddRefNet(hLog);
    if (0 == uXOnlineRef) {
        iLastError = GetLastError();

        goto FunctionExit;
    }

    // Get the user accounts
    if (FALSE == SGUsers(hLog, OnlineUsers, &dwNumUsers)) {
        iLastError = GetLastError();

        goto FunctionExit;
    }

    // Logon
    OnlineLogonHandle = SGLogon(hLog, OnlineUsers, dwNumUsers);
    if (NULL == OnlineLogonHandle) {
        iLastError = GetLastError();

        goto FunctionExit;
    }

    // Connect session
    pTestObject = SGConnect(hLog);
    if (NULL == pTestObject) {
        iLastError = GetLastError();

        goto FunctionExit;
    }

    // Run test
    if (FALSE == SGTest(hLog, pTestObject, SOCK_DGRAM, BUFFER_10_LEN)) {
        iLastError = GetLastError();

        goto FunctionExit;
    }

    if (FALSE == SGTest(hLog, pTestObject, SOCK_DGRAM, BUFFER_UDP_LEN)) {
        iLastError = GetLastError();

        goto FunctionExit;
    }

    if (FALSE == SGTest(hLog, pTestObject, SOCK_STREAM, BUFFER_10_LEN)) {
        iLastError = GetLastError();

        goto FunctionExit;
    }

    if (FALSE == SGTest(hLog, pTestObject, SOCK_STREAM, BUFFER_TCP_LEN)) {
        iLastError = GetLastError();

        goto FunctionExit;
    }

FunctionExit:
    // Disconnect session
    if (NULL != pTestObject) {
        SGDisconnect(hLog, pTestObject);
    }

    // Close the XOnlineLogon task handle
    if (NULL != OnlineLogonHandle) {
        XOnlineTaskClose(OnlineLogonHandle);
    }

    // Terminate the online subsystem
    if (0 != uXOnlineRef) {
        ReleaseNet();
    }

    if (ERROR_SUCCESS == iLastError) {
        xLog(hLog, XLL_PASS, "SG Data Test passed");
    }
    else {
        xLog(hLog, XLL_FAIL, "SG Data Test failed - ec: %u", iLastError);
    }

    // End the variation
    xEndVariation(hLog);

    return (ERROR_SUCCESS == iLastError);
}

} // namespace SGDataNamespace
