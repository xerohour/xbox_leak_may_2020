//==================================================================================
// Includes
//==================================================================================
#include "xsledge.h"

//==================================================================================
// Defines
//==================================================================================
#undef MIN
#undef MAX
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_CLIENT_CONNS    50
#define MAX_LINE 80
#define MAX_SERV_CONNS      50


//==================================================================================
// Globals
//==================================================================================
CHAR *CONN_TYPES_Str[] =
{
    "TCP",
    "UDP",
};

SLEDGE_PARMS g_SledgeParms =
{
    SERV_TCP,
    SERV_UDP,
    SERV_TCP_ADDR,
    SERV_UDP_ADDR,
    SERV_RECV_LEN,
    CLIENT_TCP_CONNS,
    CLIENT_TCP_CYCLES,
    CLIENT_UDP_CONNS,
    CLIENT_UDP_CYCLES,
    MIN_PACKET_LEN,
    MAX_PACKET_LEN,
    MIN_PACKET_LEN,
    MAX_PACKET_LEN,
    PACKETS_PER_CONN,
    CLIENT_PERCENT_SLEEPY_RECVS,
    SERV_PERCENT_SLEEPY_RECVS,
    UDP_SLEEP_BEFORE_NEXT,
    IN_FLIGHT_CLOSES
};

// States that sockets can be monitored for
typedef enum _SOCKET_STATE
{
    READABLE,
    WRITEABLE,
    EXCEPTION
} SOCKET_STATE;

CRITICAL_SECTION    g_csSerializeAccess;
CRITICAL_SECTION    g_csSledgeCritSection;
DWORD               g_dwTotalPacketsIn = 0, g_dwTotalBytesIn = 0, g_dwLastTime;
HANDLE              g_hCreateThreadsEvent;
CLIENT_CONNS        g_ccClientConns[MAX_CLIENT_CONNS];
SERV_CONNS          g_scServConns  [MAX_SERV_CONNS];
INT                 g_iServConns = 0, g_iClientConns = 0;
TCHAR               g_lpszCfgFileName[MAX_LINE];
BOOL                g_fIsServer = FALSE, g_fExit = FALSE, g_bFinished = FALSE;
BOOL                g_fVerbose = FALSE, g_fNonBlocking = FALSE;
PACKET_TYPE         g_ptTxDataType = CONSTANT;
SOCKET              g_sTCPServer = INVALID_SOCKET, g_sUDPServer = INVALID_SOCKET;
HANDLE              g_hLog = INVALID_HANDLE_VALUE;

//==================================================================================
// Functions
//==================================================================================
void WINAPI SledgeLog(HANDLE hLog, DWORD dwLogLevel, LPSTR szFormat, ...)
{
    va_list pArgs;
    va_start(pArgs, szFormat);

    if(hLog != INVALID_HANDLE_VALUE)
        xLog_va(hLog, dwLogLevel, szFormat, pArgs);

    va_end(pArgs);
}

//==================================================================================
// DllMain
//----------------------------------------------------------------------------------
//
// Description: DLL entry
//
// Arguments:
//  HINSTANCE   hInstance       Handle to module
//  DWORD       dwReason        Indicates the reason for calling the function
//  LPVOID      lpContext       reserved
// Returns:
//  TRUE on success
//==================================================================================
BOOL WINAPI DllMain(IN HINSTANCE hInstance, IN DWORD     dwReason, IN LPVOID    lpContext)
{
    // We'll initialize/delete the global critical section here
    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
        XnetInitialize(NULL, TRUE);
        InitializeCriticalSection(&g_csSerializeAccess);
        InitializeCriticalSection(&g_csSledgeCritSection);
        break;
    case DLL_PROCESS_DETACH:
        XnetCleanup();
        DeleteCriticalSection(&g_csSerializeAccess);
        DeleteCriticalSection(&g_csSledgeCritSection);
        break;
    default:
        break;
    }

    return TRUE;
}

//==================================================================================
// HardcodeTestSettings
//----------------------------------------------------------------------------------
//
// Description: Hardcodes globals for this test
//
// Arguments:
//  none
//
// Returns:
//  none
//==================================================================================
void HardcodeTestSettings(void)
{
    DWORD dwIPAddr = 0;
    g_fVerbose = TRUE;
    g_fNonBlocking = FALSE;
    g_ptTxDataType = CONSTANT;
    g_SledgeParms.InFlightCloses = FALSE;
    g_fIsServer = FALSE;
    g_SledgeParms.ServUDP = FALSE;
    g_SledgeParms.ServTCP = FALSE;
    g_SledgeParms.PacketsPerConn = 500;
    g_SledgeParms.ClientTCPConns = 2;
    g_SledgeParms.ClientTCPCycles = 10000;
    g_SledgeParms.TCPMaxPacketLen = MAX_PACKET_LEN;
    g_SledgeParms.TCPMinPacketLen = MIN_PACKET_LEN;
    dwIPAddr = inet_addr("157.56.10.105");
    g_SledgeParms.ServTCPAddr = htonl(dwIPAddr);
    g_SledgeParms.ClientUDPConns = 0;
    g_SledgeParms.ClientSleepyRecvs = 0;

    if(g_fIsServer)
    {
        g_SledgeParms.ClientTCPConns    = 0;
        g_SledgeParms.ClientUDPConns    = 0;
    }
    else
    {
        g_SledgeParms.ServTCP           = FALSE;
        g_SledgeParms.ServUDP           = FALSE;
    }

    g_iClientConns = g_SledgeParms.ClientTCPConns + g_SledgeParms.ClientUDPConns;

    return;
}

//==================================================================================
// IsSocketReady
//----------------------------------------------------------------------------------
//
// Description: Creates a listening socket and accepts a connection
//
// Arguments:
//  SOCKET          Socket              Socket to monitor for a given state
//  SOCKET_STATE    SocketState         State to monitor for
// Returns:
//  Returns TRUE if the activity has occured, FALSE otherwise
//==================================================================================
BOOL IsSocketReady(SOCKET Socket, SOCKET_STATE SocketState, DWORD dwSecondsToWait)
{
    fd_set SocketCollection, *ReadCollection, *WriteCollection, *ExceptCollection;
    timeval TimeOut = {0,0}, *pTimeOut = NULL;

    // If we have a bad socket, just report FALSE
    if(Socket == INVALID_SOCKET)
        return FALSE;

    if(dwSecondsToWait != TIME_INFINITY)
    {
        TimeOut.tv_sec = dwSecondsToWait;
        pTimeOut = &TimeOut;
    }

    FD_ZERO(&SocketCollection);
    FD_SET(Socket, &SocketCollection);

    // Depending on the activity to monitor for, setup the parameters to select
    switch(SocketState)
    {
    case READABLE:
        ReadCollection = &SocketCollection;
        WriteCollection = NULL;
        ExceptCollection = NULL;
        break;
    case WRITEABLE:
        ReadCollection = NULL;
        WriteCollection = &SocketCollection;
        ExceptCollection = NULL;
        break;
    default:
        ReadCollection = NULL;
        WriteCollection = NULL;
        ExceptCollection = &SocketCollection;
        break;
    }

    if((select(1, ReadCollection, WriteCollection, ExceptCollection, pTimeOut)) != 1)
        return FALSE;
    else
        return TRUE;
}


//==================================================================================
// RetrieveTestSettings
//----------------------------------------------------------------------------------
//
// Description: Reads profile variables for the given app into globals for this test
//
// Arguments:
//  none
//
// Returns:
//  none
//==================================================================================
void RetrieveTestSettings(void)
{
    INT     nTemp = 0;
    CHAR    szTemp[MAX_STRING_SIZE];
    DWORD   dwIPAddr = 0;

    // Are we verbose?
    nTemp = GetProfileIntA("xsledge", "Verbose", 0);
    g_fVerbose = (nTemp ? TRUE : FALSE);

    // Use non-blocking socket calls?
    nTemp = GetProfileIntA("xsledge", "NonBlockingSockets", 0);
    g_fNonBlocking = (nTemp ? TRUE : FALSE);

    // What class of data to put in the packets?
    memset(szTemp, 0, sizeof(szTemp));
    GetProfileStringA("xsledge", "DataType", "Constant", szTemp, MAX_STRING_SIZE * sizeof(CHAR));
    if(!strcmp(szTemp, "Constant"))
        g_ptTxDataType = CONSTANT;
    else if(!strcmp(szTemp, "Increment"))
        g_ptTxDataType = INCREMENT;
    else
        g_ptTxDataType = RANDOM;

    // Allow in-flight closes?
    nTemp = GetProfileIntA("xsledge", "InFlightCloses", IN_FLIGHT_CLOSES);
    g_SledgeParms.InFlightCloses = (nTemp ? TRUE : FALSE);

    // Use UDP?
    nTemp = GetProfileIntA("xsledge", "UDPServer", 0);
    g_SledgeParms.ServUDP = (nTemp ? TRUE : FALSE);
    g_fIsServer = (g_SledgeParms.ServUDP ? TRUE : g_fIsServer);

    // Use TCP?
    nTemp = GetProfileIntA("xsledge", "TCPServer", 0);
    g_SledgeParms.ServTCP = (nTemp ? TRUE : FALSE);
    g_fIsServer = (g_SledgeParms.ServTCP ? TRUE : g_fIsServer);

    if(g_fIsServer)
    {
        // What receive length?
        nTemp = GetProfileIntA("xsledge", "ReceiveLength", SERV_RECV_LEN);
        g_SledgeParms.ServRecvLen = nTemp;

        // What percent sleepy receives?
        nTemp = GetProfileIntA("xsledge", "SleepyReceives", SERV_PERCENT_SLEEPY_RECVS);
        g_SledgeParms.ServSleepyRecvs = nTemp;
    }
    else
    {
        // How many packets per connection?
        nTemp = GetProfileIntA("xsledge", "PacketsPerConn", PACKETS_PER_CONN);
        g_SledgeParms.PacketsPerConn = nTemp;

        // How many simultaneous TCP connections?
        nTemp = GetProfileIntA("xsledge", "TCPConnections", CLIENT_TCP_CONNS);
        g_SledgeParms.ClientTCPConns = nTemp;

        // How many simultaneous TCP connections?
        nTemp = GetProfileIntA("xsledge", "TCPCycles", CLIENT_TCP_CYCLES);
        g_SledgeParms.ClientTCPCycles = nTemp;

        // Max size for packets on TCP connections?
        nTemp = GetProfileIntA("xsledge", "TCPMaxSize", MAX_PACKET_LEN);
        g_SledgeParms.TCPMaxPacketLen = nTemp;

        // Min size for packets on TCP connections?
        nTemp = GetProfileIntA("xsledge", "TCPMinSize", MIN_PACKET_LEN);
        g_SledgeParms.TCPMinPacketLen = nTemp;

        // What address for the TCP connections?
        memset(szTemp, 0, sizeof(szTemp));
        GetProfileStringA("xsledge", "TCPAddress", "127.0.0.1", szTemp, MAX_STRING_SIZE * sizeof(CHAR));
        if((dwIPAddr = inet_addr(szTemp)) == -1L)
        {
            // If we were given a bad address, then just go with the address we know works
            dwIPAddr = inet_addr("127.0.0.1");
        }
        g_SledgeParms.ServTCPAddr = htonl(dwIPAddr);

        // How many simultaneous UDP connections?
        nTemp = GetProfileIntA("xsledge", "UDPConnections", CLIENT_UDP_CONNS);
        g_SledgeParms.ClientUDPConns = nTemp;

        // How many simultaneous UDP connections?
        nTemp = GetProfileIntA("xsledge", "UDPCycles", CLIENT_UDP_CYCLES);
        g_SledgeParms.ClientUDPCycles = nTemp;

        // Max size for packets on UDP connections?
        nTemp = GetProfileIntA("xsledge", "UDPMaxSize", MAX_PACKET_LEN);
        g_SledgeParms.UDPMaxPacketLen = nTemp;

        // Min size for packets on UDP connections?
        nTemp = GetProfileIntA("xsledge", "UDPMinSize", MIN_PACKET_LEN);
        g_SledgeParms.UDPMinPacketLen = nTemp;

        // Sleep between UDP sends?
        nTemp = GetProfileIntA("xsledge", "UDPSleep", UDP_SLEEP_BEFORE_NEXT);
        g_SledgeParms.UDPSleepBeforeNext = nTemp;

        // What address for the UDP connections?
        memset(szTemp, 0, sizeof(szTemp));
        GetProfileStringA("xsledge", "UDPAddress", "127.0.0.1", szTemp, MAX_STRING_SIZE * sizeof(CHAR));
        if((dwIPAddr = inet_addr(szTemp)) == -1L)
        {
            // If we were given a bad address, then just go with the address we know works
            dwIPAddr = inet_addr("127.0.0.1");
        }
        g_SledgeParms.ServUDPAddr = htonl(dwIPAddr);

        // What percent sleepy receives?
        nTemp = GetProfileIntA("xsledge", "SleepyReceives", CLIENT_PERCENT_SLEEPY_RECVS);
        g_SledgeParms.ClientSleepyRecvs = nTemp;
    }

    if(g_fIsServer)
    {
        g_SledgeParms.ClientTCPConns    = 0;
        g_SledgeParms.ClientUDPConns    = 0;
    }
    else
    {
        g_SledgeParms.ServTCP           = FALSE;
        g_SledgeParms.ServUDP           = FALSE;
    }

    g_iClientConns = g_SledgeParms.ClientTCPConns + g_SledgeParms.ClientUDPConns;

    return;
}

//==================================================================================
// VerifyValidParams
//----------------------------------------------------------------------------------
//
// Description: Verifies that we are running with valid test parameters
//
// Arguments:
//  none
//
// Returns:
//  TRUE if the parameters are OK, FALSE otherwise
//==================================================================================
BOOL VerifyValidParams()
{
    if(g_iClientConns > MAX_CLIENT_CONNS)
    {
        SledgeLog(g_hLog, XLL_WARN, "Max connections (%d) exceeded", MAX_CLIENT_CONNS);
        return FALSE;
    }

    if (g_SledgeParms.TCPMaxPacketLen > PACKET_MAX_LEN)
    {
        SledgeLog(g_hLog, XLL_WARN, "TCPMaxPacketLen (%u) > PACKET_MAX_LEN (%u)", g_SledgeParms.TCPMaxPacketLen, PACKET_MAX_LEN);
        return FALSE;
    }

    if  (g_SledgeParms.ServRecvLen > PACKET_MAX_LEN)
    {
        SledgeLog(g_hLog, XLL_WARN, "ServRecvLen (%u) > PACKET_MAX_LEN (%u)", g_SledgeParms.ServRecvLen, PACKET_MAX_LEN);
        return FALSE;
    }

    return TRUE;
}

//==================================================================================
// StartNonClientThreads
//----------------------------------------------------------------------------------
//
// Description: Starts all non-client threads (hatchet, display and server threads)
//
// Arguments:
//  none
//
// Returns:
//  TRUE if successful, FALSE otherwise
//==================================================================================
BOOL StartNonClientThreads()
{
    HANDLE  hThread;
    DWORD   dwThreadId;

    //Start the DisplayThread
    if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) DisplayThread, NULL, 0, &dwThreadId)) == NULL)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge: CreateThread(DisplayThread) failed %d", GetLastError());
        return FALSE;
    }

    //If necessary, start the HatchetThread
    if (g_SledgeParms.InFlightCloses)
    {
        if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) HatchetThread, NULL, 0, &dwThreadId)) == NULL)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge: CreateThread(Hatchet) failed %d", GetLastError());
            return FALSE;
        }

        CloseHandle(hThread);
    }

    //If necessary, start the TCPServThread
    if (g_SledgeParms.ServTCP)
    {
        if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) TCPServThread, NULL, 0, &dwThreadId)) == NULL)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: CreateThread(TCPServThread) failed %d", CONN_TYPES_Str[CONN_TCP], GetLastError());
            return FALSE;
        }

        CloseHandle(hThread);
    }

    //If necessary, start the UDPServThread
    if (g_SledgeParms.ServUDP)
    {
        if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) UDPServThread, NULL, 0, &dwThreadId)) == NULL)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: CreateThread(UDPServThread) failed %d", CONN_TYPES_Str[CONN_UDP], GetLastError());
            return FALSE;
        }

        CloseHandle(hThread);
    }

    return TRUE;
}

//==================================================================================
// IsSledgeComplete
//----------------------------------------------------------------------------------
//
// Description: Determines if the current test is complete
//
// Arguments:
//  none
//
// Returns:
//  TRUE if successful, FALSE otherwise
//==================================================================================
BOOL IsSledgeComplete()
{
    INT iConn;

    EnterCriticalSection(&g_csSledgeCritSection);

    // If any sockets haven't reached their total cycles or any are still connected, the test isn' over
    for (iConn = 0; iConn < g_iClientConns; iConn++)
    {
        if (g_ccClientConns[iConn].Cycle != g_ccClientConns[iConn].Cycles || g_ccClientConns[iConn].Connected)
        {
            LeaveCriticalSection(&g_csSledgeCritSection);
            return FALSE;
        }
    }

    // If this is a server test, then the test isn't over
    if (g_SledgeParms.ServTCP || g_SledgeParms.ServUDP)
    {
        LeaveCriticalSection(&g_csSledgeCritSection);
        return FALSE;
    }

    LeaveCriticalSection(&g_csSledgeCritSection);
    return TRUE;
}

//==================================================================================
// StartTest
//----------------------------------------------------------------------------------
//
// Description: Test entry point
//
// Arguments:
//  HANDLE      hLog            Handle to logging subsystem
// Returns:
//  none
//==================================================================================
VOID WINAPI StartTest(IN HANDLE  hLog)
{
    INT     iConn;
    HANDLE  hThread;
    DWORD   dwThreadId;
    WSADATA WSAData;
    INT     nRet = 0;


    // Since we have to be thread safe, serialize entry for now
    EnterCriticalSection(&g_csSerializeAccess);

    g_hLog = hLog;

    // Set the component and subcomponent
    xSetComponent(g_hLog, "Network(S)", "Winsock");

    // Init winsock
    if(nRet = WSAStartup(MAKEWORD(2, 2), &WSAData))
    {
        SledgeLog(g_hLog, XLL_FAIL, "Couldn't init winsock: %d", nRet);
        goto ExitMain;
    }

    SledgeLog(g_hLog, XLL_INFO, "Winsock layer initialized");

    // Get the test settings from the INI file
//  RetrieveTestSettings();
    HardcodeTestSettings();

    // Output the parameters for this test
    SledgeLog(g_hLog, XLL_INFO, "********************************************************");
    SledgeLog(g_hLog, XLL_INFO, "<< Sledge Parameters >>");
    SledgeLog(g_hLog, XLL_INFO, "********************************************************");
    if (g_fIsServer)
    {
        CHAR  szProtocols[15] = "";

        SledgeLog(g_hLog, XLL_INFO, "Running as server...");
        if (g_SledgeParms.ServTCP)
            strcat(szProtocols, "TCP ");
        if (g_SledgeParms.ServUDP)
            strcat(szProtocols, "UDP ");
        SledgeLog(g_hLog, XLL_INFO, "Protocols:            %s", szProtocols);
        SledgeLog(g_hLog, XLL_INFO, "ReceiveSize:          %d", g_SledgeParms.ServRecvLen);
        SledgeLog(g_hLog, XLL_INFO, "Hatchet Status:       %s", g_SledgeParms.InFlightCloses ? "ENABLED" : "DISABLED");
    }
    else
    {
        SledgeLog(g_hLog, XLL_INFO, "Running as client...");
        SledgeLog(g_hLog, XLL_INFO, "Packets per cycle:    %d", g_SledgeParms.PacketsPerConn);
        SledgeLog(g_hLog, XLL_INFO, "Hatchet Status:       %s", g_SledgeParms.InFlightCloses ? "ENABLED" : "DISABLED");
        if(g_SledgeParms.ClientTCPConns)
        {
            SledgeLog(g_hLog, XLL_INFO, "TCP connections:      %d",
                g_SledgeParms.ClientTCPConns);
            SledgeLog(g_hLog, XLL_INFO, "Cycles to run:        %d",
                g_SledgeParms.ClientTCPCycles);
            SledgeLog(g_hLog, XLL_INFO, "Packet Size:          Min = %d  Max = %d",
                g_SledgeParms.TCPMinPacketLen,
                g_SledgeParms.TCPMaxPacketLen);
            SledgeLog(g_hLog, XLL_INFO, "Server Address:       0x%8x",
                g_SledgeParms.ServTCPAddr);
        }
        if(g_SledgeParms.ClientUDPConns)
        {
            SledgeLog(g_hLog, XLL_INFO, "UDP connections:      %d",
                g_SledgeParms.ClientUDPConns);
            SledgeLog(g_hLog, XLL_INFO, "Cycles to run:        %d",
                g_SledgeParms.ClientUDPCycles);
            SledgeLog(g_hLog, XLL_INFO, "Packet Size:          Min = %d  Max = %d",
                g_SledgeParms.UDPMinPacketLen,
                g_SledgeParms.UDPMaxPacketLen);
            SledgeLog(g_hLog, XLL_INFO, "Server Address:       0x%8x",
                g_SledgeParms.ServUDPAddr);
        }
    }
    SledgeLog(g_hLog, XLL_INFO, "********************************************************");

    // Verify that the test parameters are valid
    if(!VerifyValidParams())
    {
        SledgeLog(g_hLog, XLL_FAIL, "Test parameters aren't valid");
        goto ExitMain;
    }

    if ((g_hCreateThreadsEvent = CreateEvent(NULL, FALSE, TRUE, NULL)) == NULL)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge: CreateEvent(g_hCreateThreadsEvent) failed %d", GetLastError());
        goto ExitMain;
    }

    // Initialize server connection structures
    for (iConn = 0; iConn < MAX_SERV_CONNS; iConn++)
    {
        memset(&g_scServConns[iConn], 0, sizeof(SERV_CONNS));
        g_scServConns[iConn].ConnType         = CONN_IDLE;
        g_scServConns[iConn].Index            = iConn;
        g_scServConns[iConn].PacketLenMin     = 9999;
        g_scServConns[iConn].SendInProgress   = 0;
        g_scServConns[iConn].RecvInProgress   = 0;
    }

    if(g_iClientConns)
        SledgeLog(g_hLog, XLL_INFO, "%d connection(s) to initialize", g_iClientConns);

    // Initialize client connection structures
    for (iConn = 0; iConn < g_iClientConns; iConn++)
    {
        memset(&g_ccClientConns[iConn], 0, sizeof(CLIENT_CONNS));
        g_ccClientConns[iConn].Index          = iConn;
        g_ccClientConns[iConn].Connected      = FALSE;
        g_ccClientConns[iConn].SendThreadDown = FALSE;
        g_ccClientConns[iConn].RecvThreadDown = FALSE;
        g_ccClientConns[iConn].PacketLenMin   = 9999;
        g_ccClientConns[iConn].NextChar       = 'a';
        g_ccClientConns[iConn].TotalLatePackets = 0;
        g_ccClientConns[iConn].TotalLostPackets = 0;
        g_ccClientConns[iConn].TotalBadPackets = 0;
        g_ccClientConns[iConn].fCSInitialized = TRUE;
        InitializeCriticalSection(&(g_ccClientConns[iConn].csSendRecv));

        if (iConn < g_SledgeParms.ClientUDPConns)
        {
            g_ccClientConns[iConn].ConnType = CONN_UDP;
            g_ccClientConns[iConn].Cycles   = g_SledgeParms.ClientUDPCycles;
        }
        else
        {
            g_ccClientConns[iConn].ConnType = CONN_TCP;
            g_ccClientConns[iConn].Cycles   = g_SledgeParms.ClientTCPCycles;
        }


        if ((g_ccClientConns[iConn].hPacketsOutEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d]: CreateEvent(hPacketsOutEvent) failed %d", iConn, GetLastError());
            goto ExitMain;
        }
    }

    // Start all non-client threads
    if(!StartNonClientThreads())
    {
        goto ExitMain;
    }

    while (1)
    {
        // Wait for a signal that we need to restart a thread
        WaitForSingleObject(g_hCreateThreadsEvent, INFINITE);

        // See if the global test exit condition has been triggered
        if (g_fExit)
        {
            SledgeLog(g_hLog, XLL_FAIL, "Global exit triggered.");
            g_bFinished = TRUE;
            goto ExitMain;
        }

        // If the test is complete, then exit
        if(IsSledgeComplete())
        {
            SledgeLog(g_hLog, XLL_INFO, "Sledge: exiting normally");
            g_bFinished = TRUE;
            goto ExitMain;
        }

        EnterCriticalSection(&g_csSledgeCritSection);

//      SledgeLog(g_hLog, XLL_INFO, "Spawning client threads.");

        // Scan for sockets that need to be restarted
        for (iConn = 0; iConn < g_iClientConns; iConn++)
        {
            if ((g_ccClientConns[iConn].Cycle < g_ccClientConns[iConn].Cycles) &&
                ! g_ccClientConns[iConn].Connected)
            {
                // Reset the connection parameters for each socket that needs to be restarted
                g_ccClientConns[iConn].Cycle = (g_ccClientConns[iConn].Cycle + 1) % INFINITE_CYCLES;
                g_ccClientConns[iConn].PacketLenMin    = 9999;
                g_ccClientConns[iConn].PacketLenMax    = 0;
                g_ccClientConns[iConn].TotalPacketsOut = 0;
                g_ccClientConns[iConn].TotalBytesOut   = 0;
                g_ccClientConns[iConn].DeltaBytesOut   = 0;
                g_ccClientConns[iConn].TotalPacketsIn  = 0;
                g_ccClientConns[iConn].TotalBytesIn    = 0;
                g_ccClientConns[iConn].DeltaBytesIn    = 0;
                g_ccClientConns[iConn].PacketsOutHead  = 0;
                g_ccClientConns[iConn].PacketsOutTail  = 0;
                g_ccClientConns[iConn].PacketsOutCount = 0;
                g_ccClientConns[iConn].SendThreadDown  = FALSE;
                g_ccClientConns[iConn].RecvThreadDown  = FALSE;
                g_ccClientConns[iConn].SleepyRecv      = FALSE;
                g_ccClientConns[iConn].InSend          = FALSE;
                g_ccClientConns[iConn].InRecv          = FALSE;
                g_ccClientConns[iConn].InClose         = FALSE;
                g_ccClientConns[iConn].NextChar        = 'a';
                g_ccClientConns[iConn].SendInProgress = 0;
                g_ccClientConns[iConn].RecvInProgress = 0;

                // Connect the socket
                OpenConn(&g_ccClientConns[iConn]);

                // Spawn the receive thread for the socket
                if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ClientRecvThread, &g_ccClientConns[iConn], 0, &dwThreadId)) == NULL)
                {
                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: CreateThread(ClientRecvThread) failed %d",
                        iConn,
                        CONN_TYPES_Str[g_ccClientConns[iConn].ConnType],
                        GetLastError());

                    LeaveCriticalSection(&g_csSledgeCritSection);
                    goto ExitMain;
                }

                CloseHandle(hThread);

                // Spawn the send thread for the socket
                if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ClientSendThread, &g_ccClientConns[iConn], 0, &dwThreadId)) == NULL)
                {
                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: CreateThread(ClientSendThread) failed %d",
                        iConn,
                        CONN_TYPES_Str[g_ccClientConns[iConn].ConnType],
                        GetLastError());

                    LeaveCriticalSection(&g_csSledgeCritSection);
                    goto ExitMain;
                }

                CloseHandle(hThread);
            }
        }

        LeaveCriticalSection(&g_csSledgeCritSection);
    }

ExitMain:

    // Need to clean up the TCP server socket if it exists
    if(g_sTCPServer != INVALID_SOCKET)
    {
        SledgeLog(g_hLog, XLL_INFO, "Closing TCP server socket.");
        if(closesocket(g_sTCPServer) == SOCKET_ERROR)
        {
            SledgeLog(g_hLog, XLL_INFO, "Error closing TCP server socket");
        }

    }

    // Need to clean up the UDP server socket if it exists
    if(g_sUDPServer != INVALID_SOCKET)
    {
        SledgeLog(g_hLog, XLL_INFO, "Closing UDP server socket.");
        if(closesocket(g_sUDPServer) == SOCKET_ERROR)
        {
            SledgeLog(g_hLog, XLL_INFO, "Error closing UDP server socket");
        }

    }

    // Make sure we've cleaned up all of our sockets and critical sections
    for (iConn = 0; iConn < g_iClientConns; iConn++)
    {
        if(g_ccClientConns[iConn].Sock != INVALID_SOCKET)
        {
            SledgeLog(g_hLog, XLL_INFO, "Closing client socket 0x%8x", g_ccClientConns[iConn].Sock);
            closesocket(g_ccClientConns[iConn].Sock);
            g_ccClientConns[iConn].Sock = INVALID_SOCKET;
        }

        if(g_ccClientConns[iConn].fCSInitialized)
            DeleteCriticalSection(&(g_ccClientConns[iConn].csSendRecv));
    }

    // Clean up winsock
    SledgeLog(g_hLog, XLL_INFO, "Cleaning up Winsock layer", g_ccClientConns[iConn].Sock);
    WSACleanup();

    g_hLog = INVALID_HANDLE_VALUE;

    // Allow other threads to enter now
    LeaveCriticalSection(&g_csSerializeAccess);

}

//==================================================================================
// EndTest
//----------------------------------------------------------------------------------
//
// Description: Test exit point
//
// Arguments:
//  none
// Returns:
//  none
//==================================================================================
VOID WINAPI EndTest()
{
    // We already cleaned up everything at the end of StartTest
}

//==================================================================================
// IsValidForHatchet
//----------------------------------------------------------------------------------
//
// Description: Helper function that determines if connection can be killed
//
// Arguments:
//  CLIENT_CONNS    *pClient
// Returns:
//  TRUE if the client can be killed, FALSE otherwise
//==================================================================================
BOOL IsValidForHatchet(CLIENT_CONNS *pClient)
{
    //If less than half the packets have been sent, then we can't kill
    if(pClient->TotalPacketsOut <= (unsigned) g_SledgeParms.PacketsPerConn / 2)
        return FALSE;

    //If the connection is already broken, then we can't kill
    if(pClient->Connected)
        return FALSE;

    //If either thread is down, then we can't kill
    if(pClient->SendThreadDown || pClient->RecvThreadDown)
        return FALSE;

    return TRUE;
}

//==================================================================================
// HatchetThread
//----------------------------------------------------------------------------------
//
// Description: Thread that kills connections if InFlightCloses is turned on
//
// Arguments:
//  LPVOID      *pParm          Just needed because this is a thread proc
// Returns:
//  always 0
//==================================================================================
DWORD WINAPI HatchetThread(LPVOID *pParm)
{
    int iConn;

    SledgeLog(g_hLog, XLL_INFO, "Hatchet thread started");

    while(1)
    {
        Sleep(1000);

        EnterCriticalSection(&g_csSledgeCritSection);

        //Look for hatchet victims
        for (iConn = 0; iConn < g_iClientConns; iConn++)
        {
            switch(g_ccClientConns[iConn].ConnType)
            {
            case CONN_TCP:
            case CONN_UDP:
                //If we find a valid victim, kill it
                if(IsValidForHatchet(&(g_ccClientConns[iConn])))
                {
                    SledgeLog(g_hLog, XLL_INFO, "Sledge[%d,%s]: in-flight closesocket()",
                        g_ccClientConns[iConn].Index,
                        CONN_TYPES_Str[g_ccClientConns[iConn].ConnType]);

                    g_ccClientConns[iConn].InClose = TRUE;

                    if (closesocket(g_ccClientConns[iConn].Sock) == SOCKET_ERROR)
                        g_ccClientConns[iConn].InClose = FALSE;

                    g_ccClientConns[iConn].InClose = FALSE;
                }
                break;
            }
        }
        LeaveCriticalSection(&g_csSledgeCritSection);
    }
    return(0);
}

//==================================================================================
// DisplayThread
//----------------------------------------------------------------------------------
//
// Description: Thread that displays status information about test programs
//
// Arguments:
//  LPVOID      *pParm          Just needed because this is a thread proc
// Returns:
//  always 0
//==================================================================================
DWORD WINAPI DisplayThread(LPVOID *pParm)
{
    int   iConn;

    ULONG DeltaTime;
    ULONG TotalPackets;
    ULONG TotalBytes;
    ULONG TotalBytesPerSec;

    CHAR  *pStr;

    DWORD ElapsedTime, ElapsedSeconds, ElapsedMinutes, ElapsedHours;
    DWORD StartTime = g_dwLastTime = GetTickCount();

    SledgeLog(g_hLog, XLL_INFO, "Display thread started");

    while(1)
    {

        if(g_fVerbose)
        {
            Sleep(10000);

            DeltaTime = GetTickCount() - g_dwLastTime;
            g_dwLastTime += DeltaTime;
            DeltaTime = DeltaTime / 1000;

            if (g_bFinished == TRUE)
            {
                return (0);
            }

            ElapsedTime = g_dwLastTime - StartTime;
            ElapsedSeconds = (ElapsedTime / 1000) % 60;
            ElapsedMinutes = (ElapsedTime / 60000) % 60;
            ElapsedHours = ElapsedTime / 3600000;
            SledgeLog(g_hLog, XLL_INFO, "Running for %dh:%02dm:%02ds", ElapsedHours, ElapsedMinutes, ElapsedSeconds);

            if (g_iServConns > 0)
            {
                TotalPackets = TotalBytes = TotalBytesPerSec = 0;

                SledgeLog(g_hLog, XLL_INFO, " Server            Packet Size");
                SledgeLog(g_hLog, XLL_INFO, "Connection        Min  Max  Avg   Packets      Bytes    Bytes/s");
                SledgeLog(g_hLog, XLL_INFO, "----------       ---- ---- ---- ----------- ----------- -------");

                for (iConn = 0; iConn < MAX_SERV_CONNS; iConn++)
                {
                    if (g_scServConns[iConn].ConnType != CONN_IDLE)
                    {
                        if (g_scServConns[iConn].SleepyRecv)
                            pStr = "zzzz";
                        else
                            pStr = "loop";

                        g_scServConns[iConn].SleepyRecv = FALSE;

                        SledgeLog(g_hLog, XLL_INFO, "%3d %6s       %4d %4d %4d %11d %11d %7d (%s) %c%c%c",
                            g_scServConns[iConn].Index,
                            CONN_TYPES_Str[g_scServConns[iConn].ConnType],
                            g_scServConns[iConn].PacketLenMin,
                            g_scServConns[iConn].PacketLenMax,
                            (g_scServConns[iConn].TotalPacketsLoop > 0) ?
                            g_scServConns[iConn].TotalBytesLoop /
                            g_scServConns[iConn].TotalPacketsLoop :
                        0,
                            g_scServConns[iConn].TotalPacketsLoop,
                            g_scServConns[iConn].TotalBytesLoop,
                            (g_scServConns[iConn].DeltaBytesLoop + DeltaTime -1)
                            / DeltaTime,
                            pStr,
                            (g_scServConns[iConn].InSend)  ? 'S' : ' ',
                            (g_scServConns[iConn].InRecv)  ? 'R' : ' ',
                            (g_scServConns[iConn].InClose) ? 'C' : ' ');

                        TotalPackets += g_scServConns[iConn].TotalPacketsLoop;
                        TotalBytes += g_scServConns[iConn].TotalBytesLoop;
                        TotalBytesPerSec += (g_scServConns[iConn].DeltaBytesLoop +
                            DeltaTime -1)
                            / DeltaTime;
                        g_scServConns[iConn].DeltaBytesLoop = 0;
                    }
                }

                SledgeLog(g_hLog, XLL_INFO, "Totals                          %11d %11d %7d (loop)",
                    TotalPackets, TotalBytes, TotalBytesPerSec);
                SledgeLog(g_hLog, XLL_INFO, "          %12d     %12d",
                    g_scServConns[iConn].RecvInProgress,
                    g_scServConns[iConn].SendInProgress);
            }

            if (g_iClientConns > 0)
            {
                TotalPackets = TotalBytes = TotalBytesPerSec = 0;
                SledgeLog(g_hLog, XLL_INFO, " Client            Packet Size");
                SledgeLog(g_hLog, XLL_INFO, "Connection Cycle  Min  Max  Avg   Packets      Bytes    Bytes/s");
                SledgeLog(g_hLog, XLL_INFO, "---------- ----- ---- ---- ---- ----------- ----------- -------");

                for (iConn = 0; iConn < g_iClientConns; iConn++)
                {
                    if (g_ccClientConns[iConn].ConnType != CONN_IDLE)
                    {
                        if (g_ccClientConns[iConn].SleepyRecv)
                            pStr = "zzzz";
                        else
                            pStr = "recv";

                        g_ccClientConns[iConn].SleepyRecv = FALSE;

                        SledgeLog(g_hLog, XLL_INFO, "%3d %6s %5d %4d %4d %4d %11d %11d %7d (send) %c%c%c",
                            g_ccClientConns[iConn].Index,
                            CONN_TYPES_Str[g_ccClientConns[iConn].ConnType],
                            g_ccClientConns[iConn].Cycle,
                            g_ccClientConns[iConn].PacketLenMin,
                            g_ccClientConns[iConn].PacketLenMax,
                            (g_ccClientConns[iConn].TotalPacketsOut > 0) ?
                            g_ccClientConns[iConn].TotalBytesOut /
                            g_ccClientConns[iConn].TotalPacketsOut : 0,
                            g_ccClientConns[iConn].TotalPacketsOut,
                            g_ccClientConns[iConn].TotalBytesOut,
                            (g_ccClientConns[iConn].DeltaBytesOut+DeltaTime-1) / DeltaTime,
                            (g_ccClientConns[iConn].InSend)  ? 'S' : ' ',
                            (g_ccClientConns[iConn].InRecv)  ? 'R' : ' ',
                            (g_ccClientConns[iConn].InClose) ? 'C' : ' ');

                        SledgeLog(g_hLog, XLL_INFO, "                                %11d %11d %7d (%s)",
                            g_ccClientConns[iConn].TotalPacketsIn,
                            g_ccClientConns[iConn].TotalBytesIn,
                            (g_ccClientConns[iConn].DeltaBytesIn+DeltaTime-1) / DeltaTime,
                            pStr);

                SledgeLog(g_hLog, XLL_INFO, "In Progress Errors");
                SledgeLog(g_hLog, XLL_INFO, "          Recv             Send");
                SledgeLog(g_hLog, XLL_INFO, "          ------------     ------------");
                SledgeLog(g_hLog, XLL_INFO, "          %12d     %12d",
                    g_ccClientConns[iConn].RecvInProgress,
                    g_ccClientConns[iConn].SendInProgress);
                SledgeLog(g_hLog, XLL_INFO, "CurrentSend");
                SledgeLog(g_hLog, XLL_INFO, "          Recv             Send");
                SledgeLog(g_hLog, XLL_INFO, "          ------------     ------------");
                SledgeLog(g_hLog, XLL_INFO, "          %12d     %12d",
                    g_ccClientConns[iConn].RecvInProgress,
                    g_ccClientConns[iConn].SendInProgress);

                        TotalPackets += g_ccClientConns[iConn].TotalPacketsOut;
                        TotalPackets += g_ccClientConns[iConn].TotalPacketsIn;
                        TotalBytes += g_ccClientConns[iConn].TotalBytesOut;
                        TotalBytes += g_ccClientConns[iConn].TotalBytesIn;
                        TotalBytesPerSec += (g_ccClientConns[iConn].DeltaBytesOut +
                            DeltaTime -1) / DeltaTime;
                        TotalBytesPerSec += (g_ccClientConns[iConn].DeltaBytesIn +
                            DeltaTime -1) / DeltaTime;

                        g_ccClientConns[iConn].DeltaBytesOut = 0;

                        g_ccClientConns[iConn].DeltaBytesIn = 0;
                    }
                }

                SledgeLog(g_hLog, XLL_INFO, "Totals                          %11d %11d %7d (client)",
                    TotalPackets, TotalBytes, TotalBytesPerSec);
                SledgeLog(g_hLog, XLL_INFO, "          Lost Packets     Late Packets     Bad Packets ");
                SledgeLog(g_hLog, XLL_INFO, "          ------------     ------------     ------------");
                SledgeLog(g_hLog, XLL_INFO, "          %12d     %12d     %12d\r\n",
                    g_ccClientConns[iConn].TotalLostPackets,
                    g_ccClientConns[iConn].TotalLatePackets,
                    g_ccClientConns[iConn].TotalBadPackets);
            }

        }
        else //if(!g_fVerbose)
        {
            Sleep(30000);

            SledgeLog(g_hLog, XLL_INFO, "Received %d packets, containing %d bytes total",
                g_dwTotalPacketsIn, g_dwTotalBytesIn);
        }


    }

    return(0);
}

//==================================================================================
// WaitForWriteableSocket
//----------------------------------------------------------------------------------
//
// Description: Used to determine when a non-blocking socket is writeable
//
// Arguments:
//  SOCKET      sockServ        Socket to examine for writeability
// Returns:
//  TRUE if the socket is writeable, otherwise FALSE
//==================================================================================
BOOL WaitForWriteableSocket(SOCKET sockServ)
{
    FD_SET  writefds;
    INT     iRet;

    if(sockServ == INVALID_SOCKET)
        return FALSE;

    // Clear all the FD_SET structures
    FD_ZERO(&writefds);
    FD_SET(sockServ, &writefds);

    // Do a blocking select
    if((iRet = select( 0, NULL, &writefds, NULL, NULL)) == SOCKET_ERROR)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[]: select() error %s", GetLastErrorText());
        return FALSE;
    }

    // If select returned but didn't fill any of the FD_SET structures, something is wrong
    if(!iRet)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[]: select() didn't return any sockets");
        return FALSE;
    }

    // If the socket isn't ready for writing, then something else must have happened... This is a failure.
    if(!FD_ISSET(sockServ, &writefds))
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[]: writing socket failed\r\n");
        return FALSE;
    }

    // There is pending data or a pending connection
    return TRUE;
}

//==================================================================================
// WaitForReadableSocket
//----------------------------------------------------------------------------------
//
// Description: Used to determine when a non-blocking socket is readable
//
// Arguments:
//  SOCKET      sockServ        Socket to examine for readability
// Returns:
//  TRUE if the socket is writeable, otherwise FALSE
//==================================================================================
BOOL WaitForReadableSocket(SOCKET sockServ)
{
    FD_SET  readfds;
    INT     iRet;

    if(sockServ == INVALID_SOCKET)
        return FALSE;

    // Clear all the FD_SET structures
    FD_ZERO(&readfds);
    FD_SET(sockServ, &readfds);

    // Do a blocking select
    if((iRet = select( 0, &readfds, NULL, NULL, NULL)) == SOCKET_ERROR)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[]: select() error %s", GetLastErrorText());
        return FALSE;
    }

    // If select returned but didn't fill any of the FD_SET structures, something is wrong
    if(!iRet)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[]: select() didn't return any sockets\r\n");
        return FALSE;
    }

    // If the socket isn't ready for reading, then something else must have happened... This is a failure.
    if(!FD_ISSET(sockServ, &readfds))
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[]: reading socket failed\r\n");
        return FALSE;
    }

    // There is pending data or a pending connection
    return TRUE;
}

//==================================================================================
// TCPServThread
//----------------------------------------------------------------------------------
//
// Description: Thread that handles incoming connetions from TCP client tests
//
// Arguments:
//  LPVOID      *Parm           Just needed because this is a thread proc
// Returns:
//  always 0
//==================================================================================
DWORD WINAPI TCPServThread(LPVOID *pParm)
{
    SOCKADDR_IN ServSockAddr = { AF_INET };
    SOCKADDR_IN PeerSockAddr;
    int         sizeofSockAddr;

    SOCKET      NewSock = INVALID_SOCKET;

    HANDLE      hThread;
    DWORD       dwThreadId;
    ULONG       uNonBlockParam = TRUE;

    int         iConn;

    SledgeLog(g_hLog, XLL_INFO, "TCP server thread started");

    // Set the local socket parameters
    memset(&ServSockAddr, 0, sizeof(struct sockaddr));
    ServSockAddr.sin_port        = htons(TCP_SERV_PORT);
    ServSockAddr.sin_addr.s_addr = INADDR_ANY;
    ServSockAddr.sin_family = AF_INET;

    // Create the socket
    if ((g_sTCPServer = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: socket() failed %s\r\n", CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
        SignalTestExit(0);
        goto DONE;
    }

    // If necessary, set the socket for non-blocking IO
    if(g_fNonBlocking)
    {
        if(ioctlsocket(g_sTCPServer, FIONBIO, &uNonBlockParam) == SOCKET_ERROR)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: ioctlsocket() error %s while setting to non-blocking",
                CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
            SignalTestExit(0);
            goto DONE;
        }
    }

    // Bind the socket to the local address
    if (bind(g_sTCPServer, (const struct sockaddr *) &ServSockAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: bind() error %s", CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
        SignalTestExit(0);
        goto DONE;
    }

    // Set the socket for accepting incoming connections
    if (listen(g_sTCPServer, SERV_BACKLOG) == SOCKET_ERROR)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: listen() error %s\r\n", CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
        SignalTestExit(0);
        goto DONE;
    }

    while(1)
    {
        // Initialize the incoming address information
        sizeofSockAddr = sizeof(SOCKADDR_IN);
        memset(&PeerSockAddr, 0, sizeof (struct sockaddr));
        PeerSockAddr.sin_family = AF_INET;

        // Accept the pending connection
        if ((NewSock = accept(g_sTCPServer, (struct sockaddr *) &PeerSockAddr, &sizeofSockAddr)) == INVALID_SOCKET)
        {
            if(g_fNonBlocking)
            {
                // If we are using a non-blocking socket and the socket would have blocked, wait for incoming connections
                if(WSAGetLastError() == WSAEWOULDBLOCK)
                {
                    if(!WaitForReadableSocket(g_sTCPServer))
                    {
                        SignalTestExit(0);
                        goto DONE;
                    }

                    if ((NewSock = accept(g_sTCPServer, (struct sockaddr *) &PeerSockAddr, &sizeofSockAddr)) == INVALID_SOCKET)
                    {
                        SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: accept() error %s (after select returned success!)",
                            CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
                        SignalTestExit(0);
                        goto DONE;
                    }

                }
            }
            else
            {
                SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: accept() error %s", CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
                SignalTestExit(0);
                goto DONE;
            }
        }

        EnterCriticalSection(&g_csSledgeCritSection);

        for (iConn = 0; iConn < MAX_SERV_CONNS; iConn++)
        {
            if (g_scServConns[iConn].ConnType == CONN_IDLE)
                break;
        }

        if (g_scServConns[iConn].ConnType != CONN_IDLE)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: g_scServConns[iConn].ConnType != CONN_IDLE");
            SignalTestExit(0);
            goto DONE;
        }

        g_iServConns++;

        g_scServConns[iConn].ConnType         = CONN_TCP;
        g_scServConns[iConn].Sock             = NewSock;
        g_scServConns[iConn].PacketLenMin     = 9999;
        g_scServConns[iConn].PacketLenMax     = 0;
        g_scServConns[iConn].TotalPacketsLoop = 0;
        g_scServConns[iConn].TotalBytesLoop   = 0;
        g_scServConns[iConn].DeltaBytesLoop   = 0;
        g_scServConns[iConn].SleepyRecv       = FALSE;
        g_scServConns[iConn].InSend           = FALSE;
        g_scServConns[iConn].InRecv           = FALSE;
        g_scServConns[iConn].InClose          = FALSE;
        g_scServConns[iConn].SendInProgress   = 0;
        g_scServConns[iConn].RecvInProgress   = 0;

        LeaveCriticalSection(&g_csSledgeCritSection);

        if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ServRecvThread, &g_scServConns[iConn], 0, &dwThreadId)) == NULL)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: CreateThread(ServRecvThread) failed %d",
                iConn,
                CONN_TYPES_Str[g_scServConns[iConn].ConnType],
                GetLastError());

            SignalTestExit(0);
            goto DONE;
        }

        CloseHandle(hThread);
    }

DONE:

    return(0);
}

//==================================================================================
// UDPServThread
//----------------------------------------------------------------------------------
//
// Description: Thread that handles incoming connections from UDP client tests
//
// Arguments:
//  LPVOID      *Parm           Just needed because this is a thread proc
// Returns:
//  always 0
//==================================================================================
DWORD WINAPI UDPServThread(LPVOID *pParm)
{
    SOCKADDR_IN ServSockAddr = { AF_INET };

    HANDLE      hThread;
    DWORD       dwThreadId;
    ULONG       uNonBlockParam = TRUE;

    int         iConn;

    SledgeLog(g_hLog, XLL_INFO, "UDP server thread started");

    // Set the local socket parameters
    memset(&ServSockAddr, 0, sizeof(struct sockaddr));
    ServSockAddr.sin_port        = htons(UDP_SERV_PORT);
    ServSockAddr.sin_addr.s_addr = INADDR_ANY;
    ServSockAddr.sin_family = AF_INET;

    // Create the socket
    if ((g_sUDPServer = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: socket() failed %s", CONN_TYPES_Str[CONN_UDP], GetLastErrorText());
        SignalTestExit(0);
        goto DONE;
    }

    // If necessary, set the socket for non-blocking IO
    if(g_fNonBlocking)
    {
        if(ioctlsocket(g_sUDPServer, FIONBIO, &uNonBlockParam) == SOCKET_ERROR)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: ioctlsocket() error %s while setting to non-blocking",
                CONN_TYPES_Str[CONN_UDP], GetLastErrorText());
            SignalTestExit(0);
            goto DONE;
        }
    }

    // Bind the socket to the local address
    if (bind(g_sUDPServer, (const struct sockaddr *) &ServSockAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: bind() error %s", CONN_TYPES_Str[CONN_UDP], GetLastErrorText());
        SignalTestExit(0);
        goto DONE;
    }

    EnterCriticalSection(&g_csSledgeCritSection);

    for (iConn = 0; iConn < MAX_SERV_CONNS; iConn++)
    {
        if (g_scServConns[iConn].ConnType == CONN_IDLE)
            break;
    }

    if (g_scServConns[iConn].ConnType != CONN_IDLE)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: g_scServConns[iConn].ConnType != CONN_IDLE");
        SignalTestExit(0);
        goto DONE;
    }

    g_iServConns++;

    g_scServConns[iConn].ConnType         = CONN_UDP;
    g_scServConns[iConn].Sock             = g_sUDPServer;
    g_scServConns[iConn].PacketLenMin     = 9999;
    g_scServConns[iConn].PacketLenMax     = 0;
    g_scServConns[iConn].TotalPacketsLoop = 0;
    g_scServConns[iConn].TotalBytesLoop   = 0;
    g_scServConns[iConn].DeltaBytesLoop   = 0;
    g_scServConns[iConn].SleepyRecv       = FALSE;
    g_scServConns[iConn].InSend           = FALSE;
    g_scServConns[iConn].InRecv           = FALSE;
    g_scServConns[iConn].InClose          = FALSE;
    g_scServConns[iConn].SendInProgress   = 0;
    g_scServConns[iConn].RecvInProgress   = 0;

    LeaveCriticalSection(&g_csSledgeCritSection);

    if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ServRecvThread, &g_scServConns[iConn], 0, &dwThreadId)) == NULL)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: CreateThread(ServRecvThread) failed %d",
            iConn, CONN_TYPES_Str[g_scServConns[iConn].ConnType],
            GetLastError());

        SignalTestExit(0);
        goto DONE;
    }

    CloseHandle(hThread);

DONE:

    return(0);
}

//==================================================================================
// ServRecvThread
//----------------------------------------------------------------------------------
//
// Description: Thread that handles receiving and echoing data back to client tests
//
// Arguments:
//  LPVOID      *Parm           Pointer to the SERV_CONNS struct for this server
// Returns:
//  always 0
//==================================================================================
DWORD WINAPI ServRecvThread(LPVOID *pParm)
{
    SERV_CONNS *pConn = (SERV_CONNS *) pParm;

    char *Packet;
    ULONG  BytesRead, BytesSent;
    ULONG  TotalBytesSent;
    int PeerLen = sizeof(SOCKADDR_IN);

    BOOL ExpectedErrorExit = FALSE;
    BOOL EOFRcvd = FALSE;
    SOCKADDR_IN   PeerSockAddr;

    // Allocate and initialize the packet buffer
    Packet = (char *) LocalAlloc(LPTR, PACKET_MAX_LEN);
    memset(&PeerSockAddr, 0, sizeof(SOCKADDR_IN));
    if (NULL == Packet)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: unable to allocate packet", pConn->Index, CONN_TYPES_Str[pConn->ConnType]);

        SignalTestExit(0);
        goto ExitServRecvThread;
    }

    while(1)
    {
        switch(pConn->ConnType)
        {
        case CONN_TCP:
            // Randomly determine if we should do a sleepy receive
            if (RandomRange(0, 99) < (ULONG) g_SledgeParms.ServSleepyRecvs)
            {
                pConn->SleepyRecv = TRUE;
                Sleep(SLEEP_BEFORE_RECV_MS);
            }

            pConn->InRecv = TRUE;

            if ((BytesRead = recv(pConn->Sock, (char *) Packet, g_SledgeParms.ServRecvLen, 0)) == SOCKET_ERROR)
            {
                //If this is a non-blocking socket and the recv completes asynchornously, then this isn't an error
                if(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
                {

                    if(WaitForReadableSocket(pConn->Sock))
                    {

                        if((BytesRead = recv(pConn->Sock,(char *) Packet,g_SledgeParms.ServRecvLen,0)) != SOCKET_ERROR)
                        {
                            goto TCPReadOK;
                        }
                    }
                }

                // If there was another blocking operation, we have to go back and try again
                if(WSAGetLastError() == WSAEINPROGRESS)
                {
                    if(!pConn->InSend)
                        SledgeLog(g_hLog, XLL_WARN, "Socket is already blocking, but we aren't sending: RecvInProgress %d, SendInProgress %d"
                        ,pConn->RecvInProgress, pConn->SendInProgress);

                    pConn->RecvInProgress = pConn->RecvInProgress + 1;
                    pConn->InRecv = FALSE;
                    Sleep(100);
                    continue;
                }


                pConn->InRecv = FALSE;

                // If we are using in flight closes
                if (g_SledgeParms.InFlightCloses)
                {
                    // If this wasn't caused by the HatchetThread, then exit
                    if (GetLastError() != WSAECONNRESET && GetLastError() != WSAECONNABORTED && GetLastError() != WSAEINVAL)
                    {
                        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: recv() failed %s",
                            pConn->Index,
                            CONN_TYPES_Str[pConn->ConnType],
                            GetLastErrorText());

                        SignalTestExit(0);
                        goto ExitServRecvThread;
                    }
                    // If this was caused by the HatchetThread, handle it the error gracefully
                    else
                    {
                        SledgeLog(g_hLog, XLL_INFO, "Sledge[%d,%s]: ServRecvThread exiting on expected error %s",
                            pConn->Index,
                            CONN_TYPES_Str[pConn->ConnType],
                            GetLastErrorText());

                        ExpectedErrorExit = TRUE;
                        goto ExitServRecvThread;
                    }
                }
                else
                {
                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: recv() failed %s",
                        pConn->Index,
                        CONN_TYPES_Str[pConn->ConnType],
                        GetLastErrorText());

                    SignalTestExit(0);
                    goto ExitServRecvThread;
                }
            }

            // The recv succeeded
TCPReadOK:
            pConn->InRecv = FALSE;

            if (BytesRead == 0)
            {
                ExpectedErrorExit = FALSE;
                SledgeLog(g_hLog, XLL_INFO, "Sledge[%d,%s]: ServRecvThread recv returned 0 bytes", pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
                goto ExitServRecvThread;
            }

            if (Packet[BytesRead - 1] == 0x1A)
            {
                EOFRcvd = TRUE;
                BytesRead--;
            }

            TotalBytesSent = 0;
            while(TotalBytesSent < BytesRead)
            {
                BytesSent = 0;
                pConn->InSend = TRUE;

                if ((BytesSent = send(pConn->Sock, (char *) Packet + TotalBytesSent, BytesRead - TotalBytesSent, 0)) == SOCKET_ERROR)
                {
                    if(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
                    {

                        Sleep(100);
                        continue;
                    }

                    // If there was another blocking operation, we have to go back and try again
                    if(WSAGetLastError() == WSAEINPROGRESS)
                    {
                        if(!pConn->InRecv)
                            SledgeLog(g_hLog, XLL_WARN, "Socket is already blocking, but we aren't recving: RecvInProgress %d, SendInProgress %d"
                            ,pConn->RecvInProgress, pConn->SendInProgress);

                        pConn->SendInProgress = pConn->SendInProgress + 1;
                        pConn->InSend = FALSE;
                        Sleep(100);
                        continue;
                    }

                    pConn->InSend = FALSE;

                    // If we are using in flight closes
                    if (g_SledgeParms.InFlightCloses)
                    {
                        // If this wasn't caused by the HatchetThread, then exit
                        if (GetLastError() != WSAECONNRESET &&
                            GetLastError() != WSAEDISCON &&
                            GetLastError() != WSAECONNABORTED &&
                            GetLastError() != WSAEINVAL)
                        {
                            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: send() failed %s",
                                pConn->Index,
                                CONN_TYPES_Str[pConn->ConnType],
                                GetLastErrorText());

                            SignalTestExit(0);
                            goto ExitServRecvThread;
                        }
                        // If this was caused by the HatchetThread, handle it the error gracefully
                        else
                        {
                            SledgeLog(g_hLog, XLL_INFO, "Sledge[%d,%s]: ServRecvThread exiting on expected error %s",
                                pConn->Index,
                                CONN_TYPES_Str[pConn->ConnType],
                                GetLastErrorText());

                            ExpectedErrorExit = TRUE;
                            goto ExitServRecvThread;
                        }
                    }
                    else
                    {
                        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: send() failed %s",
                            pConn->Index,
                            CONN_TYPES_Str[pConn->ConnType],
                            GetLastErrorText());

                        SignalTestExit(0);
                        goto ExitServRecvThread;
                    }
                }

                pConn->InSend = FALSE;
                TotalBytesSent += BytesSent;
            }

            if (TotalBytesSent != BytesRead)
            {
                SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: TotalBytesSent != BytesRead", pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
                SignalTestExit(0);
                goto ExitServRecvThread;
            }


            if (EOFRcvd)
            {
                ExpectedErrorExit = FALSE;
                goto ExitServRecvThread;
            }

            EnterCriticalSection(&g_csSledgeCritSection);

            pConn->PacketLenMin = MIN(pConn->PacketLenMin, (int) BytesRead);
            pConn->PacketLenMax = MAX(pConn->PacketLenMax, (int) BytesRead);

            pConn->TotalPacketsLoop++;
            pConn->TotalBytesLoop += BytesRead;
            pConn->DeltaBytesLoop += BytesRead;

            LeaveCriticalSection(&g_csSledgeCritSection);
            break;

            //Added UDP support... - TBJ 7/29/98
          case CONN_UDP:
              if (RandomRange(0, 99) < SERV_PERCENT_SLEEPY_RECVS)
              {
                  pConn->SleepyRecv = TRUE;

                  Sleep(SLEEP_BEFORE_RECV_MS);
              }


RECV:

              pConn->InRecv = TRUE;

              //Since we are dealing with datagrams that can be of any size, we
              //ignore the server receive length since this might cause us to
              //cut datagrams into pieces and lose data... instead we set the
              //receive length to the maximum value... we also capture the address
              //of the socket which sent this data
              if ((BytesRead =
                  recvfrom(pConn->Sock, Packet, PACKET_MAX_LEN, 0, (struct sockaddr *) &PeerSockAddr, &PeerLen))
                  == SOCKET_ERROR)
              {
                  //If this is a non-blocking socket and the recvfrom completes asynchronously, then this isn't an error
                  if(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
                  {
                      if(WaitForReadableSocket(pConn->Sock))
                      {
                          if((BytesRead =
                              recvfrom(pConn->Sock, Packet, PACKET_MAX_LEN, 0, (struct sockaddr *) &PeerSockAddr, &PeerLen))
                              != SOCKET_ERROR)
                          {
                              goto UDPReadOK;
                          }
                      }
                  }

                  // If there was another blocking operation, we have to go back and try again
                  if(WSAGetLastError() == WSAEINPROGRESS)
                  {
                      if(!pConn->InSend)
                          SledgeLog(g_hLog, XLL_WARN, "Socket is already blocking, but we aren't sending: RecvInProgress %d, SendInProgress %d"
                          ,pConn->RecvInProgress, pConn->SendInProgress);

                      pConn->RecvInProgress = pConn->RecvInProgress + 1;
                      pConn->InRecv = FALSE;
                      Sleep(100);
                      goto RECV;
                  }

                  pConn->InRecv = FALSE;

                  if (g_SledgeParms.InFlightCloses)
                  {
                      if (GetLastError() != WSAECONNRESET &&
                          GetLastError() != WSAECONNABORTED &&
                          GetLastError() != WSAEINVAL)
                      {
                          SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: recv() failed %s",
                              pConn->Index,
                              CONN_TYPES_Str[pConn->ConnType],
                              GetLastErrorText());

                          SignalTestExit(0);
                          goto ExitServRecvThread;
                      }
                      else
                      {
                          ExpectedErrorExit = TRUE;
                          goto ExitServRecvThread;
                      }
                  }
                  else
                  {
                      SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: recv() failed %s",
                          pConn->Index,
                          CONN_TYPES_Str[pConn->ConnType],
                          GetLastErrorText());

                      SignalTestExit(0);
                      goto ExitServRecvThread;
                  }
              }

              // The recvfrom succeeded
UDPReadOK:
              pConn->InRecv = FALSE;

              if (BytesRead == 0)
              {
                  ExpectedErrorExit = FALSE;
                  goto ExitServRecvThread;
              }

              if (Packet[BytesRead - 1] == 0x1A)
              {
                  EOFRcvd = TRUE;
                  BytesRead--;
              }

              // DumpMem(Packet, BytesRead);

SEND:
              pConn->InSend = TRUE;

              if ((BytesSent = sendto(pConn->Sock, Packet, BytesRead, 0,
                  (const struct sockaddr *) &PeerSockAddr, PeerLen))
                  == SOCKET_ERROR)
              {

                    //Wait for 100ms between sendto attempts...
                  while(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
                  {
                      if ((BytesSent =
                          sendto(pConn->Sock,Packet,BytesRead,0,(const struct sockaddr *)&PeerSockAddr,PeerLen))
                          != SOCKET_ERROR)
                      {
                          if(BytesSent < BytesRead)
                          {
                              SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: sendto() sent a UDP fragment!",
                                  pConn->Index,
                                  CONN_TYPES_Str[pConn->ConnType]);

                              SignalTestExit(0);
                              goto ExitServRecvThread;
                          }

                          goto UDPWriteOK;
                      }

                      Sleep(100);
                  }

                  // If there was another blocking operation, we have to go back and try again
                  if(WSAGetLastError() == WSAEINPROGRESS)
                  {
                      if(!pConn->InRecv)
                        SledgeLog(g_hLog, XLL_WARN, "Socket is already blocking, but we aren't recving: RecvInProgress %d, SendInProgress %d"
                        ,pConn->RecvInProgress, pConn->SendInProgress);

                      pConn->SendInProgress = pConn->SendInProgress + 1;
                      pConn->InSend = FALSE;
                      Sleep(100);
                      goto SEND;
                  }

                  pConn->InSend = FALSE;

                  if (g_SledgeParms.InFlightCloses)
                  {
                      if (GetLastError() != WSAECONNRESET &&
                          GetLastError() != WSAEDISCON &&
                          GetLastError() != WSAECONNABORTED &&
                          GetLastError() != WSAEINVAL)
                      {
                          SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: send() failed %s",
                              pConn->Index,
                              CONN_TYPES_Str[pConn->ConnType],
                              GetLastErrorText());

                          SignalTestExit(0);
                          goto ExitServRecvThread;
                      }
                      else
                      {
                          ExpectedErrorExit = TRUE;
                          goto ExitServRecvThread;
                      }
                  }
                  else
                  {
                      SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: send() failed %s",
                          pConn->Index,
                          CONN_TYPES_Str[pConn->ConnType],
                          GetLastErrorText());

                      SignalTestExit(0);
                      goto ExitServRecvThread;
                  }
              }

// The sendto succeeded
UDPWriteOK:

              pConn->InSend = FALSE;

              if (BytesSent != BytesRead)
              {
                  SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: BytesSent != BytesRead", pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
                  SignalTestExit(0);
                  goto ExitServRecvThread;
              }


              if (EOFRcvd)
              {
                  ExpectedErrorExit = FALSE;
                  goto ExitServRecvThread;
              }

              EnterCriticalSection(&g_csSledgeCritSection);

              pConn->PacketLenMin = (int) MIN((ULONG) pConn->PacketLenMin, BytesRead);
              pConn->PacketLenMax = (int) MAX((ULONG) pConn->PacketLenMax, BytesRead);

              if((pConn->TotalBytesLoop + BytesRead) >= ROLLOVER_LIMIT)
              {
                  pConn->TotalPacketsLoop = 1;
                  pConn->TotalBytesLoop = BytesRead;
                  pConn->DeltaBytesLoop = BytesRead;
              }
              else
              {
                  pConn->TotalPacketsLoop++;
                  pConn->TotalBytesLoop += BytesRead;
                  pConn->DeltaBytesLoop += BytesRead;
              }

              LeaveCriticalSection(&g_csSledgeCritSection);
              break;
        }
    }

ExitServRecvThread:

    pConn->InClose = TRUE;

    if (closesocket(pConn->Sock) == SOCKET_ERROR)
    {
        pConn->InClose = FALSE;

        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: closesocket() error %s",
            pConn->Index,
            CONN_TYPES_Str[pConn->ConnType],
            GetLastErrorText());

        SignalTestExit(0);

    }

    pConn->InClose = FALSE;

    EnterCriticalSection(&g_csSledgeCritSection);

    g_iServConns--;

    pConn->ConnType = CONN_IDLE;

    LeaveCriticalSection(&g_csSledgeCritSection);

    LocalFree (Packet);

    return(0);
}

//==================================================================================
// ClientSendThread
//----------------------------------------------------------------------------------
//
// Description: Thread that handles sending data to the server
//
// Arguments:
//  LPVOID      *Parm           Pointer to the CLIENT_CONNS struct for this client
// Returns:
//  always 0
//==================================================================================
DWORD WINAPI ClientSendThread(LPVOID *pParm)
{
    CLIENT_CONNS *pConn = (CLIENT_CONNS *) pParm;
    BOOL  SocketClosed  =  FALSE;
    BYTE *Packet;

    int   Reps, Len;

//  SledgeLog(g_hLog, XLL_INFO, "Client send thread started");

    Packet = (BYTE *) LocalAlloc(LPTR, PACKET_MAX_LEN);

    if (NULL == Packet)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: unable to allocate packet",
            pConn->Index, CONN_TYPES_Str[pConn->ConnType]);

        SignalTestExit(0);
        goto DONE;
    }

    for (Reps = 0; Reps < g_SledgeParms.PacketsPerConn && ! SocketClosed; Reps++)
    {
        EnterCriticalSection(&g_csSledgeCritSection);

        if(pConn->ConnType != CONN_UDP)
            Len = RandomRange(g_SledgeParms.TCPMinPacketLen, g_SledgeParms.TCPMaxPacketLen);
        else
            Len = RandomRange(g_SledgeParms.UDPMinPacketLen, g_SledgeParms.UDPMaxPacketLen);

        pConn->PacketsOut[pConn->PacketsOutHead].Len = Len;

        pConn->PacketsOut[pConn->PacketsOutHead].CheckSum =
            BuildTestPacket(pConn, Len, Packet, Reps);

        pConn->PacketsOut[pConn->PacketsOutHead].Index = Reps;

        LeaveCriticalSection(&g_csSledgeCritSection);

//      SledgeLog(g_hLog, XLL_INFO, "Generated packet %d containing %d bytes", Reps, Len);

        switch(pConn->ConnType)
        {
        case CONN_TCP:
            // If we don't do this... we could flood the other side
            while(pConn->TotalBytesOut >= (pConn->TotalBytesIn + 20000))
                Sleep(100);

            if (SendPacketSTREAM(pConn, Len, Packet) != 0)
                SocketClosed = TRUE;
            break;
        case CONN_UDP:
            if (SendPacketDGRAM(pConn, Len, Packet) != 0)
                SocketClosed = TRUE;
            break;
        default:
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: invalid connection type",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
            SignalTestExit(0);
            goto DONE;
        }

        if (!SocketClosed)
        {
            EnterCriticalSection(&g_csSledgeCritSection);

            pConn->PacketLenMin = MIN(pConn->PacketLenMin, Len);
            pConn->PacketLenMax = MAX(pConn->PacketLenMax, Len);

            pConn->TotalPacketsOut++;
            pConn->TotalBytesOut += Len;
            pConn->DeltaBytesOut += Len;

            pConn->PacketsOutHead = (pConn->PacketsOutHead + 1) %
                MAX_PACKETS_IN_FLIGHT;
            pConn->PacketsOutCount++;

            if (pConn->PacketsOutHead == pConn->PacketsOutTail)
            {
                SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: pConn->PacketsOutHead == pConn->PacketsOutTail",
                    pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
                SignalTestExit(0);
                LeaveCriticalSection(&g_csSledgeCritSection);
                goto DONE;
            }

            LeaveCriticalSection(&g_csSledgeCritSection);
        }

        if((pConn->ConnType == CONN_UDP) && (Reps < g_SledgeParms.PacketsPerConn))
            Sleep(g_SledgeParms.UDPSleepBeforeNext);

//      SledgeLog(g_hLog, XLL_INFO, "Signalling receive thread");

        if (SetEvent(pConn->hPacketsOutEvent) != TRUE)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: SetEvent(hPacketsOutEvent) failed %d",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                GetLastError());
        }

    }

    if (SocketClosed)
    {
        EnterCriticalSection(&g_csSledgeCritSection);

        if (pConn->RecvThreadDown)
            CloseConn(pConn);

        pConn->SendThreadDown = TRUE;

        pConn->PacketsOut[pConn->PacketsOutHead].Len = -1;

        pConn->PacketsOutHead = (pConn->PacketsOutHead + 1) %
            MAX_PACKETS_IN_FLIGHT;

        // UDP shouldn't increment the number of packets out on send thread exit
        if(pConn->ConnType != CONN_UDP)
            pConn->PacketsOutCount++;

        if (pConn->PacketsOutHead == pConn->PacketsOutTail)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: pConn->PacketsOutHead == pConn->PacketsOutTail",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
            SignalTestExit(0);
            LeaveCriticalSection(&g_csSledgeCritSection);
            goto DONE;
        }

        LeaveCriticalSection(&g_csSledgeCritSection);

        if (SetEvent(pConn->hPacketsOutEvent) != TRUE)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: SetEvent(hPacketsOutEvent) failed %d",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                GetLastError());
        }

        SetEvent(g_hCreateThreadsEvent);
    }
    else
    {
        switch(pConn->ConnType)
        {
        case CONN_TCP:
            Packet[0] = 0x1A;
            if (SendPacketSTREAM(pConn, 1, Packet) != 0)
                SocketClosed = TRUE;
            break;
        case CONN_UDP:
            break;
        default:
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: invalid connection type",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
            SignalTestExit(0);
            goto DONE;
        }

        EnterCriticalSection(&g_csSledgeCritSection);

        pConn->SendThreadDown = TRUE;

        pConn->PacketsOut[pConn->PacketsOutHead].Len = -1;

        pConn->PacketsOutHead = (pConn->PacketsOutHead + 1) %
            MAX_PACKETS_IN_FLIGHT;

        // UDP shouldn't increment the number of packets out on send thread exit
        if(pConn->ConnType != CONN_UDP)
            pConn->PacketsOutCount++;

        if (pConn->PacketsOutHead == pConn->PacketsOutTail)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: pConn->PacketsOutHead == pConn->PacketsOutTail",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
            SignalTestExit(0);
            LeaveCriticalSection(&g_csSledgeCritSection);
            goto DONE;
        }

        LeaveCriticalSection(&g_csSledgeCritSection);

        SledgeLog(g_hLog, XLL_INFO, "Send thread complete. RecvInProgress %d, SendInProgress %d",
            pConn->RecvInProgress, pConn->SendInProgress);

        if (SetEvent(pConn->hPacketsOutEvent) != TRUE)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: SetEvent(hPacketsOutEvent) failed %d",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                GetLastError());
        }

    }

DONE:

    LocalFree (Packet);

    return(0);
}

//==================================================================================
// ClientRecvThread
//----------------------------------------------------------------------------------
//
// Description: Thread that handles receiving data from the server
//
// Arguments:
//  LPVOID      *Parm           Pointer to the CLIENT_CONNS struct for this client
// Returns:
//  always 0
//==================================================================================
DWORD WINAPI ClientRecvThread(LPVOID *pParm)
{
    CLIENT_CONNS *pConn = (CLIENT_CONNS *) pParm;
    BOOL  SocketClosed  =  FALSE, TimedOut;

    int   Result = 0;

    BYTE *Packet, PacketNumber;

    int   CurrentPacketsOutCount, LastPacketExpected, MaxPacketExpected;
    int   CurrentPacketExpected, SkippedPackets;
    int   Reps, Len, Offset;

//  SledgeLog(g_hLog, XLL_INFO, "Client recv thread started");

    Packet = (BYTE *) LocalAlloc(LPTR, PACKET_MAX_LEN);

    if (NULL == Packet)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: unable to allocate packet",
            pConn->Index, CONN_TYPES_Str[pConn->ConnType]);

        SignalTestExit(0);
        goto DONE;
    }

    while(1)
    {
//      SledgeLog(g_hLog, XLL_INFO, "Waiting for send thread to signal");

        WaitForSingleObject(pConn->hPacketsOutEvent, INFINITE);

//      SledgeLog(g_hLog, XLL_INFO, "Send thread signaled");

        EnterCriticalSection(&g_csSledgeCritSection);

        CurrentPacketsOutCount = pConn->PacketsOutCount;
        MaxPacketExpected = g_SledgeParms.PacketsPerConn - 1;
        CurrentPacketExpected = MIN(pConn->PacketsOut[pConn->PacketsOutTail].Index, MaxPacketExpected);
        LastPacketExpected = MIN(CurrentPacketExpected + CurrentPacketsOutCount - 1, MaxPacketExpected);


        LeaveCriticalSection(&g_csSledgeCritSection);

        TimedOut = FALSE;

        //Added UDP Support...
        if(pConn->ConnType == CONN_UDP)
        {

            for(Reps = 0; (Reps < CurrentPacketsOutCount) && (!TimedOut);)
            {
                if (RandomRange(0, 99) < (ULONG) g_SledgeParms.ClientSleepyRecvs)
                {
                    pConn->SleepyRecv = TRUE;

                    Sleep(SLEEP_BEFORE_RECV_MS);
                }

                Len = PACKET_MAX_LEN;

                Result = RecvPacketDGRAM(pConn, &Len, Packet);
                if(Result == DEATH_BY_HATCHET)
                {
                    SocketClosed = TRUE;
                    goto ExitClientRecvThread;
                }
                else if (Result == RECEIVE_TIMED_OUT)
                {
                    TimedOut = TRUE;

                    //Since we waited a fair ammount of time to receive a
                    //packet, go ahead and assume that it is lost... increment
                    //the tail pointer by the number of packets that we were
                    //waiting for and decrement the number of packets that
                    //are currently listed as "out"

                    EnterCriticalSection(&g_csSledgeCritSection);
                    pConn->PacketsOutTail = (pConn->PacketsOutTail + (CurrentPacketsOutCount - Reps))
                        % MAX_PACKETS_IN_FLIGHT;
                    pConn->PacketsOutCount -= (CurrentPacketsOutCount - Reps);
                    pConn->TotalLostPackets += (CurrentPacketsOutCount - Reps);
                    LeaveCriticalSection(&g_csSledgeCritSection);

                    //All the packets for this cycle haven't been
                    //received yet...
                    continue;
                }

//              SledgeLog(g_hLog, XLL_INFO, "Received packet %d containing %d bytes", Packet[0], Len);

                //Otherwise, we received a packet... since this is an unguaranteed
                //protocol, we can't count on in-order delivery, or reliable delivery
                //We must determine which packet this is by reading the first byte...
                PacketNumber = Packet[0];

                //We weren't expecting this packet, we'd already assumed it was lost
                //We won't log it as a successfully received packet... instead we'll
                //log it as late... then continue
                if(PacketNumber < CurrentPacketExpected)
                {
                    EnterCriticalSection(&g_csSledgeCritSection);

                    pConn->TotalLatePackets++;
                    pConn->TotalLostPackets--;

                    LeaveCriticalSection(&g_csSledgeCritSection);
                    continue;
                }
                // This packet has an invalid packet number... drop it
                // and continue
                else if (PacketNumber > MaxPacketExpected)
                    continue;

                EnterCriticalSection(&g_csSledgeCritSection);

                //If the packet number of the packet we received has a higher number
                //than the one we were expecting, we assume that any packets between
                //the one expected and the one received were lost and we skip them.
                SkippedPackets = 0;

                for(Offset = 0; Offset < (PacketNumber - CurrentPacketExpected); Offset++)
                {
                    SkippedPackets++;
                    pConn->PacketsOutCount--;

                    //In the rare event that we received a packet before the sending
                    //thread incremented its head marker, leave the critical section
                    //and give it some time...
                    while(((pConn->PacketsOutTail + 1) % MAX_PACKETS_IN_FLIGHT) == pConn->PacketsOutHead)
                    {
                        LeaveCriticalSection(&g_csSledgeCritSection);
                        Sleep(1000);
                        EnterCriticalSection(&g_csSledgeCritSection);
                    }
                    pConn->PacketsOutTail = (pConn->PacketsOutTail + 1)
                        % MAX_PACKETS_IN_FLIGHT;
                    pConn->TotalLostPackets++;

                    //Since we were expecting this packet,
                    Reps++;
                }
                CurrentPacketExpected += SkippedPackets;

                //Now that any skipped packets have been accounted for, update
                //our info for the packet we just received
                //This packet just doesn't add up...
                if (CheckSum(Len, Packet) !=
                    pConn->PacketsOut[pConn->PacketsOutTail].CheckSum)
                {
                    pConn->TotalBadPackets++;

                    SledgeLog(g_hLog, XLL_WARN, "Received packet failed checksum");
                    LeaveCriticalSection(&g_csSledgeCritSection);
                    continue;
                }

//              SledgeLog(g_hLog, XLL_INFO, "Received packet passed checksum");

                CurrentPacketExpected++;
                pConn->PacketsOutCount--;
                pConn->PacketsOutTail = (pConn->PacketsOutTail + 1)
                    % MAX_PACKETS_IN_FLIGHT;
                Reps++;
                g_dwTotalPacketsIn++;
                g_dwTotalBytesIn += Len;
                pConn->TotalPacketsIn++;
                pConn->TotalBytesIn += Len;
                pConn->DeltaBytesIn += Len;

                LeaveCriticalSection(&g_csSledgeCritSection);

                if(PacketNumber >= MaxPacketExpected)
                    goto ExitClientRecvThread;
            }

            //Since this was the last packet expected for this cycle,
            //end the receive thread...
            if(LastPacketExpected >= MaxPacketExpected)
                goto ExitClientRecvThread;
        }
        else
        {

            for (Reps = 0; Reps < CurrentPacketsOutCount; Reps++)
            {

                EnterCriticalSection(&g_csSledgeCritSection);

                Len = pConn->PacketsOut[pConn->PacketsOutTail].Len;

                LeaveCriticalSection(&g_csSledgeCritSection);

                if (RandomRange(0, 99) < (ULONG) g_SledgeParms.ClientSleepyRecvs)
                {
                    pConn->SleepyRecv = TRUE;

                    Sleep(SLEEP_BEFORE_RECV_MS);
                }

                switch(pConn->ConnType)
                {
                case CONN_TCP:
                    Result = RecvPacketSTREAM(pConn, Len, Packet, (Len == -1));
                    if ( Result == DEATH_BY_HATCHET)
                    {
                        SocketClosed = TRUE;
                        goto ExitClientRecvThread;
                    }

                    if (Len == -1)
                        goto ExitClientRecvThread;
                    break;
                default:
                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: invalid connection type", pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
                    SignalTestExit(0);
                    goto DONE;
                }

//              SledgeLog(g_hLog, XLL_INFO, "Received packet %d containing %d bytes", Packet[0], Len);

                if (CheckSum(Len, Packet) != pConn->PacketsOut[pConn->PacketsOutTail].CheckSum)
                {
                    OutputDebugString(L"Checksum failed.\n");
                    __asm int 3;

                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: CheckSum failed, packet len %d",
                        pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                        Len);

                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: CheckSum expected %d, received %d",
                        pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                        pConn->PacketsOut[pConn->PacketsOutTail].CheckSum,
                        CheckSum(Len, Packet));

                    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

                    SledgeLog(g_hLog, XLL_WARN, " Client            Packet Size");
                    SledgeLog(g_hLog, XLL_WARN, "Connection Cycle  Min  Max  Avg   Packets      Bytes    Bytes/s");
                    SledgeLog(g_hLog, XLL_WARN, "---------- ----- ---- ---- ---- ----------- ----------- -------");

                    SledgeLog(g_hLog, XLL_WARN, "%3d %6s %5d %4d %4d %4d %11d %11d %7d (send)",
                        pConn->Index,
                        CONN_TYPES_Str[pConn->ConnType],
                        pConn->Cycle,
                        pConn->PacketLenMin,
                        pConn->PacketLenMax,
                        (pConn->TotalPacketsOut > 0) ?
                        pConn->TotalBytesOut /
                        pConn->TotalPacketsOut :
                    0,
                        pConn->TotalPacketsOut,
                        pConn->TotalBytesOut,
                        0);

                    SledgeLog(g_hLog, XLL_WARN, "                                %11d %11d %7d (recv)",
                        pConn->TotalPacketsIn,
                        pConn->TotalBytesIn,
                        0);
                    SignalTestExit(0);
                    goto DONE;
                }

//              SledgeLog(g_hLog, XLL_INFO, "Received packet passed checksum");

                EnterCriticalSection(&g_csSledgeCritSection);

                g_dwTotalPacketsIn++;
                g_dwTotalBytesIn += Len;

                pConn->TotalPacketsIn++;
                pConn->TotalBytesIn += Len;
                pConn->DeltaBytesIn += Len;

                pConn->PacketsOutTail = (pConn->PacketsOutTail + 1)
                    % MAX_PACKETS_IN_FLIGHT;
                pConn->PacketsOutCount--;

                LeaveCriticalSection(&g_csSledgeCritSection);
            }
        }
    }

ExitClientRecvThread:

    SledgeLog(g_hLog, XLL_INFO, "Recv thread complete.");

    if (SocketClosed)
    {
        EnterCriticalSection(&g_csSledgeCritSection);

        if (pConn->SendThreadDown)
            CloseConn(pConn);

        pConn->RecvThreadDown = TRUE;

        LeaveCriticalSection(&g_csSledgeCritSection);
    }
    else
    {
        //Without this, it is possible for the recv thread to overtake
        //the send thread and kill the socket before the send thread
        //exits...
        while(! pConn->SendThreadDown)
            Sleep(1000);

        EnterCriticalSection(&g_csSledgeCritSection);

        pConn->RecvThreadDown = TRUE;

        pConn->PacketsOutTail = (pConn->PacketsOutTail + 1)
            % MAX_PACKETS_IN_FLIGHT;
        pConn->PacketsOutCount--;

        CloseConn(pConn);

        LeaveCriticalSection(&g_csSledgeCritSection);
    }

    SetEvent(g_hCreateThreadsEvent);

DONE:

    LocalFree (Packet);

    return(0);
}

//==================================================================================
// OpenConn
//----------------------------------------------------------------------------------
//
// Description: Establish a connection to the server
//
// Arguments:
//  CLIENT_CONNS    *pConn          Pointer to the CLIENT_CONNS struct for this client
// Returns:
//  none
//==================================================================================
void OpenConn(CLIENT_CONNS *pConn)
{

    SOCKADDR_IN     DstAddrIP  = { AF_INET }, LclAddrIP = { AF_INET };
    ULONG           uNonBlockParam = TRUE;

    int           RepeatCount = 0;

    switch(pConn->ConnType)
    {
    case CONN_TCP:
        DstAddrIP.sin_port        = htons(TCP_SERV_PORT);
        DstAddrIP.sin_addr.s_addr = htonl(g_SledgeParms.ServTCPAddr);

//      SledgeLog(g_hLog, XLL_INFO, "Creating TCP socket");

        if ((pConn->Sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: socket() failed %s",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                GetLastErrorText());

            SignalTestExit(0);
            return;
        }

        // If necessary, set the socket for non-blocking IO
        if(g_fNonBlocking)
        {
//          SledgeLog(g_hLog, XLL_INFO, "Set socket as non-blocking");
            if(ioctlsocket(pConn->Sock, FIONBIO, &uNonBlockParam) == SOCKET_ERROR)
            {
                SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: ioctlsocket() error %s while setting to non-blocking",
                    CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
                SignalTestExit(0);
                return;
            }
        }

        while (1)
        {
//          SledgeLog(g_hLog, XLL_INFO, "Connecting socket");
            if (connect(pConn->Sock, (const struct sockaddr *) &DstAddrIP,  sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
            {
//              SledgeLog(g_hLog, XLL_INFO, "Connect returned an error");

                // If this test is non-blocking, then wait for the connection to complete
                if(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
                {
                    if(!WaitForWriteableSocket(pConn->Sock))
                    {
                        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: Failed waiting for connection to complete",
                            pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
                        SignalTestExit(0);
                        return;
                    }
                    else
                        break;
                }

                if (WSAGetLastError() == WSAECONNREFUSED)
                {
                    if (RepeatCount++ == CLIENT_TCP_CONNECT_RETRIES)
                    {
                        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: connect(0x%02X%02X 0x%02X%02X 0x%02X%02X%02X%02X) failed %s (%d times)",
                            pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                            ((BYTE *) &DstAddrIP)[0], ((BYTE *) &DstAddrIP)[1],
                            ((BYTE *) &DstAddrIP)[2], ((BYTE *) &DstAddrIP)[3],
                            ((BYTE *) &DstAddrIP)[4], ((BYTE *) &DstAddrIP)[5],
                            ((BYTE *) &DstAddrIP)[6], ((BYTE *) &DstAddrIP)[7],
                            GetLastErrorText(),
                            CLIENT_TCP_CONNECT_RETRIES);

                        SignalTestExit(0);
                        return;
                    }
                }
                else
                {
                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: connect(0x%02X%02X 0x%02X%02X 0x%02X%02X%02X%02X) failed %s",
                        pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                        ((BYTE *) &DstAddrIP)[0], ((BYTE *) &DstAddrIP)[1],
                        ((BYTE *) &DstAddrIP)[2], ((BYTE *) &DstAddrIP)[3],
                        ((BYTE *) &DstAddrIP)[4], ((BYTE *) &DstAddrIP)[5],
                        ((BYTE *) &DstAddrIP)[6], ((BYTE *) &DstAddrIP)[7],
                        GetLastErrorText());

                    SignalTestExit(0);
                    return;
                }
                Sleep(1000);
            }
            else
            {
//              SledgeLog(g_hLog, XLL_INFO, "Connect succeeded");
                break;
            }
        }

        break;

    case CONN_UDP:
        LclAddrIP.sin_family      = AF_INET;
        LclAddrIP.sin_port        = htons(UDP_SERV_PORT);
        LclAddrIP.sin_addr.s_addr = INADDR_ANY;

        if ((pConn->Sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: socket() failed %s",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                GetLastErrorText());

            SignalTestExit(0);
            return;
        }

        // If necessary, set the socket for non-blocking IO
        if(g_fNonBlocking)
        {
            if(ioctlsocket(pConn->Sock, FIONBIO, &uNonBlockParam) == SOCKET_ERROR)
            {
                SledgeLog(g_hLog, XLL_WARN, "Sledge[%s]: ioctlsocket() error %s while setting to non-blocking",
                    CONN_TYPES_Str[CONN_UDP], GetLastErrorText());
                SignalTestExit(0);
                return;
            }
        }

        if (bind(pConn->Sock, (const struct sockaddr *) &LclAddrIP,
            sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: bind() failed %s",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                GetLastErrorText());

            SignalTestExit(0);
            return;
        }
        break;
    default:
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: invalid connection type",
            pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
        SignalTestExit(0);
        return;

    }

    pConn->Connected = TRUE;
}

//==================================================================================
// CloseConn
//----------------------------------------------------------------------------------
//
// Description: Close connection to the server
//
// Arguments:
//  CLIENT_CONNS    *pConn          Pointer to the CLIENT_CONNS struct for this client
// Returns:
//  none
//==================================================================================
void CloseConn(CLIENT_CONNS *pConn)
{
    switch(pConn->ConnType)
    {
    case CONN_TCP:
    case CONN_UDP:
        pConn->InClose = TRUE;

        if (closesocket(pConn->Sock) == SOCKET_ERROR)
        {
            pConn->InClose = FALSE;

            if (g_SledgeParms.InFlightCloses)
            {
                if (GetLastError() != WSAENOTSOCK)
                {
                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: closesocket() failed %s",
                        pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                        GetLastErrorText());

                    SignalTestExit(0);
                    return;
                }
            }
            else
            {
                SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: closesocket() failed %s",
                    pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                    GetLastErrorText());

                SignalTestExit(0);
                return;
            }
        }

        pConn->Sock = INVALID_SOCKET;

        pConn->InClose = FALSE;
        break;
    default:
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: invalid connection type",
            pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
        SignalTestExit(0);
        return;
    }

    pConn->Connected = FALSE;
}

//==================================================================================
// RecvPacketSTREAM
//----------------------------------------------------------------------------------
//
// Description: Receive packet from a reliable data source (e.g. TCP connection)
//
// Arguments:
//  CLIENT_CONNS    *pConn          Pointer to the CLIENT_CONNS struct for this client
//  int             Len             Length of the passed in buffer
//  BYTE            Buff[]          Buffer to be filled with the data
//  BOOL            ExpectingClose  Bool indicating whether a socket close is expected
// Returns:
//  0 for success, error code otherwise
//==================================================================================
int RecvPacketSTREAM(CLIENT_CONNS *pConn, int Len, BYTE Buff[],
                 BOOL ExpectingClose)
{
    int    TotalBytesRead = 0;
    ULONG  BytesRead;
    DWORD  dwLastError = 0;

    if (ExpectingClose)
        Len = 1;

    while (TotalBytesRead < Len)
    {

//      SledgeLog(g_hLog, XLL_INFO, "Calling recv() with %d bytes", Len - TotalBytesRead);

        EnterCriticalSection(&(pConn->csSendRecv));
        pConn->InRecv = TRUE;

        if ((BytesRead = recv(pConn->Sock, (char *) Buff + TotalBytesRead,
            Len - TotalBytesRead, 0)) == SOCKET_ERROR)
        {
            pConn->InRecv = FALSE;
            LeaveCriticalSection(&(pConn->csSendRecv));
//          SledgeLog(g_hLog, XLL_INFO, "recv() returned an error");

            dwLastError = WSAGetLastError();

            // If this is a non-blocking test, wait for pending data before calling recv again
            if(g_fNonBlocking && (dwLastError == WSAEWOULDBLOCK))
            {
                Sleep(0);
                continue;
            }

            OutputDebugString(L"recv() failed\n");
            __asm int 3;

            // If there was another blocking operation, we have to go back and try again
            if(dwLastError == WSAEINPROGRESS)
            {
                if(!pConn->InSend)
                    SledgeLog(g_hLog, XLL_WARN, "Socket is already blocking, but we aren't sending: RecvInProgress %d, SendInProgress %d"
                    ,pConn->RecvInProgress, pConn->SendInProgress);

                pConn->RecvInProgress = pConn->RecvInProgress + 1;
                Sleep(100);
                continue;
            }

            if (g_SledgeParms.InFlightCloses)
            {
                if (GetLastError() != WSAENOTSOCK     &&
                    GetLastError() != WSAECONNRESET   &&
                    GetLastError() != WSAECONNABORTED &&
                    GetLastError() != WSAESHUTDOWN    &&
                    GetLastError() != WSAENOTCONN     &&
                    GetLastError() != WSAEINVAL       &&
                    GetLastError() != WSAEDISCON      &&
                    GetLastError() != WSAEINTR)
                {

                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: recv() failed %s(%u)",
                        pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                        GetLastErrorText(), dwLastError);

                    SignalTestExit(0);
                    return (-1);
                }
                else
                    return(DEATH_BY_HATCHET);
            }
            else
            {
                SOCKADDR_IN addrLocal;
                INT         addrLen;

                addrLen = sizeof(SOCKADDR_IN);
                getsockname(pConn->Sock, (struct sockaddr *) &addrLocal, &addrLen);

                SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: recv() failed %s(%u) on port 0x%x on cycle %d, %d packets out, %d packets in",
                    pConn->Index, CONN_TYPES_Str[pConn->ConnType], GetLastErrorText(), WSAGetLastError(),
                    ntohs(addrLocal.sin_port), pConn->Cycle, pConn->TotalPacketsOut, pConn->TotalPacketsIn);

                SignalTestExit(0);
                return (-1);
            }
        }

        pConn->InRecv = FALSE;
        LeaveCriticalSection(&(pConn->csSendRecv));

//      SledgeLog(g_hLog, XLL_INFO, "recv() returned success");

        if (ExpectingClose && BytesRead == 0)
            return(0);

        if ((! ExpectingClose) && BytesRead == 0)
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: recv() unexpected socket close",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType]);

            SignalTestExit(0);
            return (-1);
        }

        TotalBytesRead += BytesRead;
    }

    return(0);
}

//==================================================================================
// RecvPacketDGRAM
//----------------------------------------------------------------------------------
//
// Description: Receive packet from datagram data source (e.g. UDP connection)
//
// Arguments:
//  CLIENT_CONNS    *pConn          Pointer to the CLIENT_CONNS struct for this client
//  int             Len             Length of the passed in buffer
//  BYTE            Buff[]          Buffer to be filled with the data
// Returns:
//  0 for success, error code otherwise
//==================================================================================
int RecvPacketDGRAM(CLIENT_CONNS *pConn, int *Len, BYTE Buff[])
{
    BOOL         Loop = FALSE;
    ULONG        BytesRead;

RECV:
    EnterCriticalSection(&(pConn->csSendRecv));
    pConn->InRecv = TRUE;

    if ((BytesRead = recvfrom(pConn->Sock, (char *) Buff, *Len, 0, NULL, NULL)) == SOCKET_ERROR)
    {
        pConn->InRecv = FALSE;
        LeaveCriticalSection(&(pConn->csSendRecv));

        // If this is a non-blocking test, wait for pending data before calling recv again
        if(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
        {
            if(!WaitForReadableSocket(pConn->Sock))
            {
                SignalTestExit(0);
                return (-1);
            }

            EnterCriticalSection(&(pConn->csSendRecv));
            pConn->InRecv = TRUE;
            if ((BytesRead = recvfrom(pConn->Sock, (char *) Buff, *Len, 0, NULL, NULL)) != SOCKET_ERROR)
            {
                pConn->InRecv = FALSE;
                LeaveCriticalSection(&(pConn->csSendRecv));
                goto UDPReadOK;
            }
            pConn->InRecv = FALSE;
            LeaveCriticalSection(&(pConn->csSendRecv));
        }

        // If there was another blocking operation, we have to go back and try again
        if(WSAGetLastError() == WSAEINPROGRESS)
        {
            if(!pConn->InSend)
                SledgeLog(g_hLog, XLL_WARN, "Socket is already blocking, but we aren't sending: RecvInProgress %d, SendInProgress %d"
                ,pConn->RecvInProgress, pConn->SendInProgress);

            pConn->RecvInProgress = pConn->RecvInProgress + 1;
            Sleep(100);
            goto RECV;
        }

        if (g_SledgeParms.InFlightCloses)
        {
            if (GetLastError() != WSAENOTSOCK     &&
                GetLastError() != WSAECONNRESET   &&
                GetLastError() != WSAECONNABORTED &&
                GetLastError() != WSAESHUTDOWN    &&
                GetLastError() != WSAENOTCONN     &&
                GetLastError() != WSAEINVAL       &&
                GetLastError() != WSAEDISCON      &&
                GetLastError() != WSAEINTR)
            {
                SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: recv() failed %s",
                    pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                    GetLastErrorText());

                SignalTestExit(0);
                return (-1);
            }
            else
            {

                return(DEATH_BY_HATCHET);
            }
        }
        else
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: recv() failed %s",
                pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                GetLastErrorText());

            SignalTestExit(0);
            return (-1);
        }
    }

    pConn->InRecv = FALSE;
    LeaveCriticalSection(&(pConn->csSendRecv));

// The recvfrom succeeded
UDPReadOK:

    pConn->InRecv = FALSE;

    if (BytesRead == 0)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: recv() unexpected socket close",
            pConn->Index, CONN_TYPES_Str[pConn->ConnType]);

        SignalTestExit(0);
        return (-1);
    }

    *Len = BytesRead;

    return(0);
}

//==================================================================================
// SendPacketSTREAM
//----------------------------------------------------------------------------------
//
// Description: Send packet via reliable communications (e.g. TCP connection)
//
// Arguments:
//  CLIENT_CONNS    *pConn          Pointer to the CLIENT_CONNS struct for this client
//  int             Len             Length of the passed in buffer
//  BYTE            Buff[]          Buffer containing data to send
// Returns:
//  0 for success, error code otherwise
//==================================================================================
int SendPacketSTREAM(CLIENT_CONNS *pConn, int Len, BYTE Buff[])
{
    ULONG  BytesSent, TotalBytesSent;
    DWORD  dwLastError = 0;

//  SledgeLog(g_hLog, XLL_INFO, "Entered SendPacketSTREAM");

    if(Len <= 0)
        return (0);

    TotalBytesSent = 0;
    while(TotalBytesSent < (ULONG) Len)
    {

//      SledgeLog(g_hLog, XLL_INFO, "Calling send() with %d bytes", Len - TotalBytesSent);

        EnterCriticalSection(&(pConn->csSendRecv));
        pConn->InSend = TRUE;

        if ((BytesSent = send(pConn->Sock, (char *) Buff + TotalBytesSent, Len - TotalBytesSent, 0)) == SOCKET_ERROR)
        {
//          SledgeLog(g_hLog, XLL_INFO, "send() returned an error");

            pConn->InSend = FALSE;
            LeaveCriticalSection(&(pConn->csSendRecv));

            dwLastError = WSAGetLastError();

            // If this is a non-blocking test, wait for 100ms before calling send again
            if(g_fNonBlocking && (dwLastError == WSAEWOULDBLOCK))
            {
                Sleep(0);
                continue;
            }

            OutputDebugString(L"send() failed\n");
            __asm int 3;

            // If there was another blocking operation, we have to go back and try again
            if(dwLastError == WSAEINPROGRESS)
            {
                if(!pConn->InRecv)
                    SledgeLog(g_hLog, XLL_WARN, "Socket is already blocking, but we aren't recving: RecvInProgress %d, SendInProgress %d"
                    ,pConn->RecvInProgress, pConn->SendInProgress);

                pConn->SendInProgress = pConn->SendInProgress + 1;
                Sleep(100);
                continue;
            }

            if (g_SledgeParms.InFlightCloses)
            {
                if (GetLastError() != WSAENOTSOCK     &&
                    GetLastError() != WSAECONNRESET   &&
                    GetLastError() != WSAECONNABORTED &&
                    GetLastError() != WSAESHUTDOWN    &&
                    GetLastError() != WSAENOTCONN     &&
                    GetLastError() != WSAEINVAL       &&
                    GetLastError() != WSAEDISCON      &&
                    GetLastError() != WSAEINTR)
                {
                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: send() failed %s",
                        pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                        GetLastErrorText());

                    SignalTestExit(0);
                    return (-1);
                }
                else
                    return(GetLastError());
            }
            else
            {
                SOCKADDR_IN addrLocal;
                INT         addrLen;

                addrLen = sizeof(SOCKADDR_IN);
                getsockname(pConn->Sock, (struct sockaddr *) &addrLocal, &addrLen);

                SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: send() failed %s on port 0x%x on cycle %d, %d packets out, %d packets in",
                    pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                    GetLastErrorText(), ntohs(addrLocal.sin_port), pConn->Cycle, pConn->TotalPacketsOut, pConn->TotalPacketsIn);

                SignalTestExit(0);
                return (-1);

            }
        }
        else
        {
            pConn->InSend = FALSE;
            LeaveCriticalSection(&(pConn->csSendRecv));
        }

//      SledgeLog(g_hLog, XLL_INFO, "send() returned success");

        TotalBytesSent += BytesSent;
    }

    if ((int) TotalBytesSent != Len)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: TotalBytesSent (%u) != Len (%u)", pConn->Index, CONN_TYPES_Str[pConn->ConnType],
            TotalBytesSent, Len);
        SignalTestExit(0);
        return (-1);
    }

    return(0);
}

//==================================================================================
// SendPacketDGRAM
//----------------------------------------------------------------------------------
//
// Description: Send packet via unreliable communications (e.g. UDP connection)
//
// Arguments:
//  CLIENT_CONNS    *pConn          Pointer to the CLIENT_CONNS struct for this client
//  int             Len             Length of the passed in buffer
//  BYTE            Buff[]          Buffer containing data to send
// Returns:
//  0 for success, error code otherwise
//==================================================================================
int SendPacketDGRAM(CLIENT_CONNS *pConn, int Len, BYTE Buff[])
{
    ULONG         BytesSent;
    SOCKADDR_IN   DstAddrIP = { AF_INET };

    DstAddrIP.sin_port        = htons(UDP_SERV_PORT);
    DstAddrIP.sin_addr.s_addr = htonl(g_SledgeParms.ServUDPAddr);

    if(Len < 0)
        return (0);

SEND:

    EnterCriticalSection(&(pConn->csSendRecv));
    pConn->InSend = TRUE;

    if ((BytesSent = sendto(pConn->Sock, (char *) Buff, Len, 0,
        (const struct sockaddr *) &DstAddrIP, sizeof(SOCKADDR_IN))) == SOCKET_ERROR)
    {
        pConn->InSend = FALSE;
        LeaveCriticalSection(&(pConn->csSendRecv));

        //Wait for 100ms between sendto attempts...
        while(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
        {
            EnterCriticalSection(&(pConn->csSendRecv));
            pConn->InSend = TRUE;

            if ((BytesSent =
                sendto(pConn->Sock, (char *) Buff, Len, 0, (const struct sockaddr *) &DstAddrIP, sizeof(SOCKADDR_IN)))
                == SOCKET_ERROR)
            {
                pConn->InSend = FALSE;
                LeaveCriticalSection(&(pConn->csSendRecv));
                if(BytesSent < (ULONG) Len)
                {
                    SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: sendto() sent a UDP fragment!",
                        pConn->Index,
                        CONN_TYPES_Str[pConn->ConnType]);

                    SignalTestExit(0);
                    return (-1);
                }

                goto UDPWriteOK;
            }

            pConn->InSend = FALSE;
            LeaveCriticalSection(&(pConn->csSendRecv));

            Sleep(100);
        }

        // If there was another blocking operation, we have to go back and try again
        if(WSAGetLastError() == WSAEINPROGRESS)
        {
            if(!pConn->InRecv)
                SledgeLog(g_hLog, XLL_WARN, "Socket is already blocking, but we aren't recving: RecvInProgress %d, SendInProgress %d"
                ,pConn->RecvInProgress, pConn->SendInProgress);

            pConn->SendInProgress = pConn->SendInProgress + 1;
            Sleep(100);
            goto SEND;
        }

        if (g_SledgeParms.InFlightCloses)
        {
            if (GetLastError() != WSAENOTSOCK     &&
                GetLastError() != WSAECONNRESET   &&
                GetLastError() != WSAECONNABORTED &&
                GetLastError() != WSAESHUTDOWN    &&
                GetLastError() != WSAENOTCONN     &&
                GetLastError() != WSAEINVAL       &&
                GetLastError() != WSAEDISCON      &&
                GetLastError() != WSAEINTR)
            {
                SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: send() failed %s",
                    pConn->Index, CONN_TYPES_Str[pConn->ConnType],
                    GetLastErrorText());

                SignalTestExit(0);
                return (-1);
            }
            else
            {
                return(GetLastError());
            }
        }
        else
        {
            SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: send() failed %s",
                pConn->Index,
                CONN_TYPES_Str[pConn->ConnType],
                GetLastErrorText());

            SignalTestExit(0);
            return (-1);
        }
    }

    pConn->InSend = FALSE;
    LeaveCriticalSection(&(pConn->csSendRecv));

// The sendto succeeded
UDPWriteOK:

    pConn->InSend = FALSE;

    if ((int) BytesSent != Len)
    {
        SledgeLog(g_hLog, XLL_WARN, "Sledge[%d,%s]: BytesSent != Len",
            pConn->Index, CONN_TYPES_Str[pConn->ConnType]);

        SignalTestExit(0);
        return (-1);
    }


    return(0);
}

//==================================================================================
// RandomRange
//----------------------------------------------------------------------------------
//
// Description: Generate a pseudo-random number
//
// Arguments:
//  ULONG           Min         Minimum random number desired
//  ULONG           Max         Maximum random number desired
// Returns:
//  ULONG containing the value of the generated random number
//==================================================================================
ULONG RandomRange(ULONG Min, ULONG Max)
{
    return((rand() % (Max - Min + 1)) + Min);
}

//==================================================================================
// BuildTestPacket
//----------------------------------------------------------------------------------
//
// Description: Generate a pseudo-random number
//
// Arguments:
//  CLIENT_CONNS    *pConn      Pointer to the clients CLIENT_CONNS struct
//  int             Len         Size of the buffer to fill
//  BYTE            Buff[]      Buffer to fill with data
//  int             nPacketNum  Number indicating the order this packet will be tx'd
// Returns:
//  ULONG containing the checksum of the generated packet
//==================================================================================
ULONG BuildTestPacket(CLIENT_CONNS *pConn, int Len, BYTE Buff[], int nPacketNum)
{
    ULONG CheckSum = 0;
    int   i = 0;

    Buff[i] = (BYTE) nPacketNum;
    CheckSum += Buff[i];
    i++;

    for (; i < Len; i++)
    {
        if(i == (Len - 1))
            Buff[i] = 0xAA;
        else
            Buff[i] = pConn->NextChar;

        if( g_ptTxDataType == INCREMENT )
        {
            if (pConn->NextChar == 'z')
                pConn->NextChar = 'a';
            else
                pConn->NextChar++;
        }
        else if ( g_ptTxDataType == RANDOM )
        {
            while((pConn->NextChar = (BYTE) (rand() & 0x000000ff)) == (BYTE) 0x1A);
        }


        CheckSum += Buff[i];
    }

    if ((pConn->NextChar >= 'z') || (pConn->NextChar < 'a'))
        pConn->NextChar = 'a';
    else
        pConn->NextChar++;


    return(CheckSum);
}

//==================================================================================
// BuildTestPacket
//----------------------------------------------------------------------------------
//
// Description: Calculate checksum over a buffer
//
// Arguments:
//  int             Len         Size of the buffer to calculate checksum over
//  BYTE            Buff[]      Buffer to calculate checksum over
// Returns:
//  ULONG containing the checksum of the buffer
//==================================================================================
ULONG CheckSum(int Len, BYTE Buff[])
{
    ULONG CheckSum = 0;
    int   i;

    for (i = 0; i < Len; i++)
    {
        CheckSum += Buff[i];
    }

    return(CheckSum);
}

//==================================================================================
// SignalTestExit
//----------------------------------------------------------------------------------
//
// Description: If we finish the test or encounter a critical error, signal to exit
//
// Arguments:
//  UINT            ExitCode    Code to return
// Returns:
//  none
//==================================================================================
void SignalTestExit(UINT ExitCode)
{
    g_fExit = TRUE;
    SetEvent(g_hCreateThreadsEvent);
}

//==================================================================================
// GetLastErrorText
//----------------------------------------------------------------------------------
//
// Description: Maps the last error to a string
//
// Arguments:
//  none
// Returns:
//  TCHAR string containing text representing the last error
//==================================================================================
CHAR * GetLastErrorText()
{
    switch (WSAGetLastError())
    {
    case WSAEINTR:
        return ("WSAEINTR");
        break;

    case WSAEBADF:
        return("WSAEBADF");
        break;

    case WSAEACCES:
        return("WSAEACCES");
        break;

    case WSAEFAULT:
        return("WSAEFAULT");
        break;

    case WSAEINVAL:
        return("WSAEINVAL");
        break;

    case WSAEMFILE:
        return("WSAEMFILE");
        break;

    case WSAEWOULDBLOCK:
        return("WSAEWOULDBLOCK");
        break;

    case WSAEINPROGRESS:
        return("WSAEINPROGRESS");
        break;

    case WSAEALREADY:
        return("WSAEALREADY");
        break;

    case WSAENOTSOCK:
        return("WSAENOTSOCK");
        break;

    case WSAEDESTADDRREQ:
        return("WSAEDESTADDRREQ");
        break;

    case WSAEMSGSIZE:
        return("WSAEMSGSIZE");
        break;

    case WSAEPROTOTYPE:
        return("WSAEPROTOTYPE");
        break;

    case WSAENOPROTOOPT:
        return("WSAENOPROTOOPT");
        break;

    case WSAEPROTONOSUPPORT:
        return("WSAEPROTONOSUPPORT");
        break;

    case WSAESOCKTNOSUPPORT:
        return("WSAESOCKTNOSUPPORT");
        break;

    case WSAEOPNOTSUPP:
        return("WSAEOPNOTSUPP");
        break;

    case WSAEPFNOSUPPORT:
        return("WSAEPFNOSUPPORT");
        break;

    case WSAEAFNOSUPPORT:
        return("WSAEAFNOSUPPORT");
        break;

    case WSAEADDRINUSE:
        return("WSAEADDRINUSE");
        break;

    case WSAEADDRNOTAVAIL:
        return("WSAEADDRNOTAVAIL");
        break;

    case WSAENETDOWN:
        return("WSAENETDOWN");
        break;

    case WSAENETUNREACH:
        return("WSAENETUNREACH");
        break;

    case WSAENETRESET:
        return("WSAENETRESET");
        break;

    case WSAECONNABORTED:
        return("WSAECONNABORTED");
        break;

    case WSAECONNRESET:
        return("WSAECONNRESET");
        break;

    case WSAENOBUFS:
        return("WSAENOBUFS");
        break;

    case WSAEISCONN:
        return("WSAEISCONN");
        break;

    case WSAENOTCONN:
        return("WSAENOTCONN");
        break;

    case WSAESHUTDOWN:
        return("WSAESHUTDOWN");
        break;

    case WSAETOOMANYREFS:
        return("WSAETOOMANYREFS");
        break;

    case WSAETIMEDOUT:
        return("WSAETIMEDOUT");
        break;

    case WSAECONNREFUSED:
        return("WSAECONNREFUSED");
        break;

    case WSAELOOP:
        return("WSAELOOP");
        break;

    case WSAENAMETOOLONG:
        return("WSAENAMETOOLONG");
        break;

    case WSAEHOSTDOWN:
        return("WSAEHOSTDOWN");
        break;

    case WSAEHOSTUNREACH:
        return("WSAEHOSTUNREACH");
        break;

    case WSAENOTEMPTY:
        return("WSAENOTEMPTY");
        break;

    case WSAEPROCLIM:
        return("WSAEPROCLIM");
        break;

    case WSAEUSERS:
        return("WSAEUSERS");
        break;

    case WSAEDQUOT:
        return("WSAEDQUOT");
        break;

    case WSAESTALE:
        return("WSAESTALE");
        break;

    case WSAEREMOTE:
        return("WSAEREMOTE");
        break;

    case WSAEDISCON:
        return("WSAEDISCON");
        break;

    case WSASYSNOTREADY:
        return("WSASYSNOTREADY");
        break;

    case WSAVERNOTSUPPORTED:
        return("WSAVERNOTSUPPORTED");
        break;

    case WSANOTINITIALISED:
        return("WSANOTINITIALISED");
        break;

    default:
        return("Unknown Error");
    }
}
