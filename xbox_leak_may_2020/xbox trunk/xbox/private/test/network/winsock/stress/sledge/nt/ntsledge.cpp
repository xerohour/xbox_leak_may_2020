//****************
//*** Includes ***
//****************
#include "ntsledge.h"

#include "stdio.h"
#include "stdlib.h"

//***************
//*** Defines ***
//***************
#undef MIN
#undef MAX
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_CLIENT_CONNS    50
#define MAX_LINE 80
#define MAX_SERV_CONNS      50

//***************
//*** Globals ***
//***************
LPCTSTR	cszThisFile = TEXT("sledge.cpp");

TCHAR *CONN_TYPES_Str[] =
{
    TEXT("TCP"),
		TEXT("UDP"),
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

CRITICAL_SECTION g_csSledgeCritSection;
DWORD g_dwTotalPacketsIn = 0, g_dwTotalBytesIn = 0, g_dwLastTime;
HANDLE g_hCreateThreadsEvent;
CLIENT_CONNS g_ccClientConns[MAX_CLIENT_CONNS];
SERV_CONNS g_scServConns  [MAX_SERV_CONNS];
INT g_iServConns = 0, g_iClientConns = 0;
TCHAR g_lpszCfgFileName[MAX_LINE];
BOOL g_fIsServer = FALSE, g_fExit = FALSE, g_bFinished = FALSE, g_fVerbose = FALSE, g_fNonBlocking = FALSE;
PACKET_TYPE g_ptTxDataType = CONSTANT;

//*****************
//*** Functions ***
//*****************

//**********************************************
//*** TRACE - Function to print debug output ***
//**********************************************
void WINAPI TRACE(LPCTSTR szFormat, ...) {
	TCHAR szBuffer[1024] = TEXT("");
	va_list pArgs; 
	va_start(pArgs, szFormat);
	
	wvsprintf(szBuffer, szFormat, pArgs);
	
	va_end(pArgs);
#ifdef UNDER_CE
	OutputDebugString(szBuffer);
#else
	_tprintf(szBuffer);
#endif
}

//*****************************************************
//*** ParseCommandLine - Parse command line options ***
//*****************************************************
BOOL ParseCommandLine(INT argc, TCHAR *argv[])
{
	INT	i;
	LPTSTR	szCmdLine = NULL, szCurr = NULL, szTemp = NULL;
	DWORD	dwCmdLineSize = 0;
	struct hostent *hostp;
	DWORD dwIPAddr = 0;
	CHAR Address[40];
	BOOL fEOL = FALSE;

	// Determine size of fake command line...
	for(i = 0; i < argc; i++)
	{
		dwCmdLineSize += ((_tcslen(argv[i]) + 1) * sizeof(TCHAR));
	}

	szCmdLine = (LPTSTR) LocalAlloc(LPTR, dwCmdLineSize);
	memset(szCmdLine, 0, dwCmdLineSize);

	// Copy command line components into fake command line
	for(i = 0; i < argc; i++)
	{
		// if this is the first argument, we start with cpy
		if(!i)
		{
			_tcscpy(szCmdLine, argv[i]);
			_tcscat(szCmdLine, TEXT(" "));
		}
		// if this is the last argument, we don't need a space at the end
		else if(i == argc - 1)
		{
			_tcscat(szCmdLine, argv[i]);
		}
		// for all the "middle" arguments, we need to use cat and put a space at the end
		else
		{
			_tcscat(szCmdLine, argv[i]);
			_tcscat(szCmdLine, TEXT(" "));
		}		
	}

	szCurr = szCmdLine;
	
	while(*szCurr)
    {
		if ((*szCurr == TEXT('-')) || (*szCurr == TEXT('/')))
        {
			szCurr++;
			switch (*szCurr)
            {
			case TEXT('v'):
			case TEXT('V'):
				g_fVerbose = TRUE;
				TRACE(TEXT("Setting to verbose mode...\r\n"));
				break;
			case TEXT('n'):
			case TEXT('N'):
				g_fNonBlocking = TRUE;
				TRACE(TEXT("Setting to non-blocking mode...\r\n"));
				break;
			case TEXT('d'):
			case TEXT('D'):
				szCurr++;
				switch (*szCurr)
				{
				case TEXT('\0'):
					break;
				case TEXT('c'):
				case TEXT('C'):
					g_ptTxDataType = CONSTANT;
					TRACE(TEXT("Setting to packet type to CONSTANT...\r\n"));
					break;
				case TEXT('i'):
				case TEXT('I'):
					g_ptTxDataType = INCREMENT;
					TRACE(TEXT("Setting to packet type to INCREMENT...\r\n"));
					break;
				case TEXT('r'):
				case TEXT('R'):
					g_ptTxDataType = RANDOM;
					TRACE(TEXT("Setting to packet type to RANDOM...\r\n"));
					break;
				default:
					break;
				}
				break;
			case TEXT('s'):
			case TEXT('S'):
				szCurr++;
				g_fIsServer = TRUE;
				switch (*szCurr)
				{
				case TEXT('\0') :
					break;
				case TEXT('t') :
				case TEXT('T') :
					g_SledgeParms.ServTCP = TRUE;
					TRACE(TEXT("TCP Server Mode...\r\n"));
					break;
				case TEXT('u') :
				case TEXT('U') :
					g_SledgeParms.ServUDP = TRUE;
					TRACE(TEXT("UDP Server Mode...\r\n"));
					break;
				case TEXT('l') :
				case TEXT('L') :
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
						
					g_SledgeParms.ServRecvLen = _ttol(szCurr);
					TRACE(TEXT("Setting ServRecvLen to %d\r\n"),
						g_SledgeParms.ServRecvLen);
						
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				case TEXT('s') :
				case TEXT('S') :
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
						
					g_SledgeParms.ServSleepyRecvs = _ttol(szCurr);
					TRACE(TEXT("Setting Server sleepy receives to %d\r\n"),
						g_SledgeParms.ServSleepyRecvs);
						
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				default :
					usage();
					LocalFree(szCmdLine);
					return FALSE;
				}
				break;
			case TEXT('f'):
			case TEXT('F'):
				g_SledgeParms.InFlightCloses = TRUE;
				TRACE(TEXT("InFlight closes enabled\r\n"));
				break;
			case TEXT('p'):
			case TEXT('P'):
				szCurr++;
				while(*szCurr == TEXT(' '))
					szCurr++;
						
				g_SledgeParms.PacketsPerConn = _ttol (szCurr);
				TRACE(TEXT("Setting Packets Per Connection to %d\r\n"), g_SledgeParms.PacketsPerConn);
						
				while(*szCurr && (*szCurr != TEXT(' ')))
					szCurr++;
				break;
			case TEXT('t'):
			case TEXT('T'):
				szCurr++;
				switch(*szCurr)
				{
				case TEXT('c'):
				case TEXT('C'):
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
						
					g_SledgeParms.ClientTCPConns = _ttol (szCurr);
					TRACE(TEXT("Setting TCP Connections to %d\r\n"),
						g_SledgeParms.ClientTCPConns);
							
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				case TEXT('l'):
				case TEXT('L'):
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
						
					g_SledgeParms.ClientTCPCycles = _ttol (szCurr);
					TRACE(TEXT("Setting TCP Cycles to %d\r\n"), g_SledgeParms.ClientTCPCycles);
						
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				case TEXT('a'):
				case TEXT('A'):
					dwIPAddr = 0;
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
						
					if(!*szCurr || (*szCurr == TEXT('/')) || (*szCurr == TEXT('-')))
					{
						usage();
						LocalFree(szCmdLine);
						return FALSE;
					}
						for(szTemp = szCurr; (*szTemp && (*szTemp != TEXT(' '))); szTemp++);
					if(*szTemp)
						*szTemp = 0;
					else
						fEOL = TRUE;

#ifdef UNICODE
					wcstombs (Address, szCurr, _tcslen(szCurr)+1);
#else
					strcpy (Address, szCurr);
#endif
					if ((dwIPAddr = inet_addr(Address)) == -1L)
					{
						TRACE(TEXT("Host=0x%8x\r\n"), Address);
						if ((hostp = gethostbyname (Address)) != NULL)
						{
							dwIPAddr = *((long *)hostp->h_addr);
						}
					}
					if (dwIPAddr == 0)
					{
						TRACE(TEXT("Error setting dwIPAddr to '%a'\r\n"), Address);
					}
					TRACE(TEXT("TCP Addr=%hs(0x%X)\r\n"), Address, dwIPAddr);
					g_SledgeParms.ServTCPAddr = htonl(dwIPAddr);
					
					szCurr = szTemp;
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				case TEXT('m'):
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
						
					g_SledgeParms.TCPMinPacketLen = _ttol (szCurr);
					TRACE(TEXT("Setting TCP packets minimum length to %d\r\n"), g_SledgeParms.TCPMinPacketLen);
					
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				case TEXT('M'):
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
						
					g_SledgeParms.TCPMaxPacketLen = _ttol (szCurr);
					TRACE(TEXT("Setting TCP packets maximum length to %d\r\n"), g_SledgeParms.TCPMaxPacketLen);
						
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
					
				default :
					usage();
					LocalFree(szCmdLine);
					return FALSE;
			}
			break;
			case TEXT('u'):
			case TEXT('U'):
				szCurr++;
				switch (*szCurr)
				{
				case TEXT('c'):
				case TEXT('C'):
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
							
					g_SledgeParms.ClientUDPConns = _ttol (szCurr);
					if(g_SledgeParms.ClientUDPConns > 1)
						g_SledgeParms.ClientUDPConns = 1;
					TRACE(TEXT("Setting UDP Connections to %d\r\n"), g_SledgeParms.ClientUDPConns);
						
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				case TEXT('l'):
				case TEXT('L'):
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
						
					g_SledgeParms.ClientUDPCycles = _ttol (szCurr);
					TRACE(TEXT("Setting UDP Cycles to %d\r\n"), g_SledgeParms.ClientUDPCycles);
						
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				case TEXT('a'):
				case TEXT('A'):
					dwIPAddr = 0;
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
							
					if(!*szCurr || (*szCurr == TEXT('/')) || (*szCurr == TEXT('-')))
					{
						usage();
						LocalFree(szCmdLine);
						return FALSE;
					}
					
					for(szTemp = szCurr; (*szTemp && (*szTemp != TEXT(' '))); szTemp++);
					if(*szTemp)
						*szTemp = 0;
					else
						fEOL = TRUE;

#ifdef UNICODE
					wcstombs (Address, szCurr, _tcslen(szCurr)+1);
#else								
					strcpy (Address, szCurr);
#endif
					if ((dwIPAddr = inet_addr(Address)) == -1L)
					{
						TRACE(TEXT("Host='%a'\r\n"), Address);
						if ((hostp = gethostbyname (Address)) != NULL)
						{
							dwIPAddr = *((long *)hostp->h_addr);
						}
					}
					if (dwIPAddr == 0)
					{
						TRACE(TEXT("Error setting dwIPAddr to '%a'\r\n"), Address);
					}
					TRACE(TEXT("UDP Addr=%hs(0x%X)\r\n"),
						Address, dwIPAddr);
					g_SledgeParms.ServUDPAddr = htonl(dwIPAddr);
							
					szCurr = szTemp;
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				case TEXT('m'):
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
								
					g_SledgeParms.UDPMinPacketLen = _ttol (szCurr);
					TRACE(TEXT("Setting UDP packets minimum length to %d\r\n"),
						g_SledgeParms.UDPMinPacketLen);
								
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				case TEXT('M'):
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
							
					g_SledgeParms.UDPMaxPacketLen = _ttol (szCurr);
					TRACE(TEXT("Setting UDP packets maximum length to %d\r\n"),
						g_SledgeParms.UDPMaxPacketLen);
							
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				case TEXT('s'):
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
							
					g_SledgeParms.UDPSleepBeforeNext = _ttol (szCurr);
					TRACE(TEXT("Setting UDP sleeping time between datagrams to %d\r\n"),
						g_SledgeParms.UDPMaxPacketLen);
								
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;
				default :
					usage();
					LocalFree(szCmdLine);
					return FALSE;
				}
				break;
			case TEXT('c'):
			case TEXT('C'):
				szCurr++;
				switch (*szCurr)
				{
				case TEXT('s') :
				case TEXT('S') :
					szCurr++;
					while(*szCurr == TEXT(' '))
						szCurr++;
								
					g_SledgeParms.ClientSleepyRecvs = _ttol(szCurr);
					TRACE(TEXT("Setting Client sleepy receives to %d\r\n"),
						g_SledgeParms.ClientSleepyRecvs);
									
					while(*szCurr && (*szCurr != TEXT(' ')))
						szCurr++;
					break;                    
				default :
					usage();
					LocalFree(szCmdLine);
					return FALSE;
				}
				break;
			default :
				TRACE(TEXT("Bad argument '%c'\r\n"), *szCurr);
				usage();
				LocalFree(szCmdLine);
				return FALSE;
			}
		}

		if(fEOL)
			break;

		szCurr++;
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
	
	LocalFree(szCmdLine);

	return TRUE;
}

//****************************************************
//*** VerifyValidParams - Verify valid test params ***
//****************************************************
BOOL VerifyValidParams()
{
	if(g_iClientConns > MAX_CLIENT_CONNS)
	{
		TRACE(TEXT("Max connections (%d) exceeded\r\n"), MAX_CLIENT_CONNS);
		return FALSE;
	}
	
    if (g_SledgeParms.TCPMaxPacketLen > PACKET_MAX_LEN)
    {
		TRACE(TEXT("TCPMaxPacketLen (%u) > PACKET_MAX_LEN (%u)\r\n"), g_SledgeParms.TCPMaxPacketLen, PACKET_MAX_LEN);
        return FALSE;
    }
	
	if  (g_SledgeParms.ServRecvLen > PACKET_MAX_LEN)
	{
		TRACE(TEXT("ServRecvLen (%u) > PACKET_MAX_LEN (%u)\r\n"), g_SledgeParms.ServRecvLen, PACKET_MAX_LEN);
        return FALSE;
	}

	return TRUE;
}

//********************************************************
//*** StartNonClientThreads - Start non-client threads ***
//********************************************************
BOOL StartNonClientThreads()
{
    HANDLE  hThread;
    DWORD   dwThreadId;

	//Start the DisplayThread
	if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) DisplayThread, NULL, 0, &dwThreadId)) == NULL)
	{
		TRACE(TEXT("Sledge: CreateThread(DisplayThread) failed %d\r\n"), GetLastError());
		return FALSE;
	}
	
	//If necessary, start the HatchetThread
    if (g_SledgeParms.InFlightCloses)
    {
        if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) HatchetThread, NULL, 0, &dwThreadId)) == NULL)
        {
            TRACE(TEXT("Sledge: CreateThread(Hatchet) failed %d\r\n"), GetLastError());
            return FALSE;
        }
		
        CloseHandle(hThread);
    }

	//If necessary, start the TCPServThread
    if (g_SledgeParms.ServTCP)
    {
        if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) TCPServThread, NULL, 0, &dwThreadId)) == NULL)
        {
            TRACE(TEXT("Sledge[%s]: CreateThread(TCPServThread) failed %d\r\n"), CONN_TYPES_Str[CONN_TCP], GetLastError());
            return FALSE;
        }
		
        CloseHandle(hThread);
    }
	
	//If necessary, start the UDPServThread
    if (g_SledgeParms.ServUDP)
    {
        if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) UDPServThread, NULL, 0, &dwThreadId)) == NULL)
        {
            TRACE(TEXT("Sledge[%s]: CreateThread(UDPServThread) failed %d\r\n"), CONN_TYPES_Str[CONN_UDP], GetLastError());
            return FALSE;
        }
		
        CloseHandle(hThread);
    }

	return TRUE;
}

//********************************************************************
//*** IsSledgeComplete - Determine if the current test is complete ***
//********************************************************************
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

//*********************************************
//*** WinMain - Entry point for application ***
//*********************************************
int _cdecl _tmain(INT argc, TCHAR *argvW[])
{
	INT		iConn;
    WORD    WSAVerReq = MAKEWORD(1,1);
    WSADATA WSAData;
	
    BOOL    fSuccess = TRUE;
	HANDLE	hThread;
	DWORD	dwThreadId;
	
	// Initialize the winsock layer
    if (WSAStartup(WSAVerReq, &WSAData) != 0)
    {
		TRACE(TEXT("Sledge: WSAStartup() failed\r\n"));
        return FALSE;
    }

	// Parse the command line
	if(!ParseCommandLine(argc, argvW))
	{
		fSuccess = FALSE;
		goto ExitMain;
	}

	// Verify that the test parameters are valid
	if(!VerifyValidParams())
	{
		fSuccess = FALSE;
		goto ExitMain;
	}

	InitializeCriticalSection(&g_csSledgeCritSection);
	
    if ((g_hCreateThreadsEvent = CreateEvent(NULL, FALSE, TRUE, NULL)) == NULL) 
    {	
        TRACE(TEXT("Sledge: CreateEvent(g_hCreateThreadsEvent) failed %d\r\n"), GetLastError());
        return FALSE;
    }
    
	// Initialize server connection structures
    for (iConn = 0; iConn < MAX_SERV_CONNS; iConn++)
    {
        memset(&g_scServConns[iConn], 0, sizeof(SERV_CONNS));
        g_scServConns[iConn].ConnType         = CONN_IDLE;
        g_scServConns[iConn].Index            = iConn;
        g_scServConns[iConn].PacketLenMin     = 9999;
    }

	if(g_iClientConns)
		TRACE(TEXT("%d connection(s) to initialize\r\n"), g_iClientConns);
	
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
            TRACE(TEXT("Sledge[%d]: CreateEvent(hPacketsOutEvent) failed %d\r\n"), iConn, GetLastError());
            return FALSE;
        }
    }

	// Start all non-client threads
	if(!StartNonClientThreads())
	{
		fSuccess = FALSE;
		goto ExitMain;
	}
	
    while (1)
    {
		// Wait for a signal that we need to restart a thread
        WaitForSingleObject(g_hCreateThreadsEvent, INFINITE);
		
		// See if the global test exit condition has been triggered
        if (g_fExit)
		{
			g_bFinished = TRUE;
            goto ExitMain;
		}
		
		// If the test is complete, then exit
		if(IsSledgeComplete())
		{
			TRACE(TEXT("Sledge: exiting normally\r\n"));
			g_bFinished = TRUE;
			goto ExitMain;
		}
		
		EnterCriticalSection(&g_csSledgeCritSection);

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
				
				// Connect the socket
				OpenConn(&g_ccClientConns[iConn]);
				
				// Spawn the receive thread for the socket
				if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ClientRecvThread, &g_ccClientConns[iConn], 0, &dwThreadId)) == NULL)
				{
					TRACE(TEXT("Sledge[%d,%s]: CreateThread(ClientRecvThread) failed %d\r\n"),
						iConn,
						CONN_TYPES_Str[g_ccClientConns[iConn].ConnType],
						GetLastError());
					
					return FALSE;
				}
				
				CloseHandle(hThread);
				
				// Spawn the send thread for the socket
				if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ClientSendThread, &g_ccClientConns[iConn], 0, &dwThreadId)) == NULL)
				{
					TRACE(TEXT("Sledge[%d,%s]: CreateThread(ClientSendThread) failed %d\r\n"),
						iConn, 
						CONN_TYPES_Str[g_ccClientConns[iConn].ConnType],
						GetLastError());
					
					return FALSE;
				}
				
				CloseHandle(hThread);
			}
		}
		
		LeaveCriticalSection(&g_csSledgeCritSection);
    }
	
ExitMain:
	
	Sleep(5000);
	
	return fSuccess;
	
}            

//*******************************************************************************************
//*** IsValidForHatchet - Helper function for HatchetThread that determines if connection ***
//***    can be killed                                                                    ***
//*******************************************************************************************
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

//************************************************************************************
//*** HatchetThread - Thread that kills connections if InFlightCloses is turned on ***
//************************************************************************************
DWORD WINAPI HatchetThread(LPVOID *pParm)
{
    int iConn;
    
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
                    TRACE(TEXT("Sledge[%d,%s]: in-flight closesocket()\r\n"),
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

//***********************************************************************************
//*** DisplayThread - Thread that displays status information about test progress ***
//***********************************************************************************
DWORD WINAPI DisplayThread(LPVOID *pParm)
{
    int   iConn;
    
    ULONG DeltaTime;
	ULONG TotalPackets;
	ULONG TotalBytes;
	ULONG TotalBytesPerSec;
	
    TCHAR *pStr;
	
	DWORD ElapsedTime, ElapsedSeconds, ElapsedMinutes, ElapsedHours;
    DWORD StartTime = g_dwLastTime = GetTickCount();
	
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
			TRACE(TEXT("Running for %dh:%02dm:%02ds\r\n"), ElapsedHours, ElapsedMinutes, ElapsedSeconds);
			
			if (g_iServConns > 0)
			{
				TotalPackets = TotalBytes = TotalBytesPerSec = 0;
				
				TRACE(TEXT(" Server            Packet Size\r\n"));
				TRACE(TEXT("Connection        Min  Max  Avg   Packets      Bytes    Bytes/s\r\n"));
				TRACE(TEXT("----------       ---- ---- ---- ----------- ----------- -------\r\n"));
				
				for (iConn = 0; iConn < MAX_SERV_CONNS; iConn++)
				{
					if (g_scServConns[iConn].ConnType != CONN_IDLE)
					{
						if (g_scServConns[iConn].SleepyRecv)
							pStr = TEXT("zzzz");
						else
							pStr = TEXT("loop");
						
						g_scServConns[iConn].SleepyRecv = FALSE;
					
						TRACE(TEXT("%3d %6s       %4d %4d %4d %11d %11d %7d (%s) %c%c%c\r\n"),
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
							(g_scServConns[iConn].InSend)  ? TEXT('S') : TEXT(' '),
							(g_scServConns[iConn].InRecv)  ? TEXT('R') : TEXT(' '),
							(g_scServConns[iConn].InClose) ? TEXT('C') : TEXT(' '));
						
						TotalPackets += g_scServConns[iConn].TotalPacketsLoop;
						TotalBytes += g_scServConns[iConn].TotalBytesLoop;
						TotalBytesPerSec += (g_scServConns[iConn].DeltaBytesLoop +
							DeltaTime -1)
							/ DeltaTime;
						g_scServConns[iConn].DeltaBytesLoop = 0;
					}        
				}
				
				TRACE(TEXT("Totals                          %11d ")
					TEXT("%11d %7d (loop)\r\n"),
					TotalPackets, TotalBytes, TotalBytesPerSec);
			}
			
			if (g_iClientConns > 0)
			{
				TotalPackets = TotalBytes = TotalBytesPerSec = 0;
				TRACE(TEXT(" Client            Packet Size\r\n"));
				TRACE(TEXT("Connection Cycle  Min  Max  Avg   Packets      Bytes    Bytes/s\r\n"));
				TRACE(TEXT("---------- ----- ---- ---- ---- ----------- ----------- -------\r\n"));
				
				for (iConn = 0; iConn < g_iClientConns; iConn++)
				{
					if (g_ccClientConns[iConn].ConnType != CONN_IDLE)
					{
						if (g_ccClientConns[iConn].SleepyRecv)
							pStr = TEXT("zzzz");
						else
							pStr = TEXT("recv");
						
						g_ccClientConns[iConn].SleepyRecv = FALSE;
						
						TRACE(TEXT("%3d %6s %5d %4d %4d %4d %11d %11d %7d (send) %c%c%c\r\n")
							TEXT("                                          %11d %11d %7d (%s)\r\n"),
							g_ccClientConns[iConn].Index,
							CONN_TYPES_Str[g_ccClientConns[iConn].ConnType],
							g_ccClientConns[iConn].Cycle,
							g_ccClientConns[iConn].PacketLenMin,
							g_ccClientConns[iConn].PacketLenMax,
							(g_ccClientConns[iConn].TotalPacketsOut > 0) ?
							g_ccClientConns[iConn].TotalBytesOut / 
							g_ccClientConns[iConn].TotalPacketsOut :
						0,
							g_ccClientConns[iConn].TotalPacketsOut,
							g_ccClientConns[iConn].TotalBytesOut,
							(g_ccClientConns[iConn].DeltaBytesOut+DeltaTime-1) /
							DeltaTime,
							(g_ccClientConns[iConn].InSend)  ? TEXT('S') : TEXT(' '),
							(g_ccClientConns[iConn].InRecv)  ? TEXT('R') : TEXT(' '),
							(g_ccClientConns[iConn].InClose) ? TEXT('C') : TEXT(' '),
							g_ccClientConns[iConn].TotalPacketsIn,
							g_ccClientConns[iConn].TotalBytesIn,
							(g_ccClientConns[iConn].DeltaBytesIn+DeltaTime-1) /
							DeltaTime,
							pStr);
						
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
				
				TRACE(TEXT("Totals                          %11d %11d ")
					TEXT("%7d (client)\r\n"),
					TotalPackets, TotalBytes, TotalBytesPerSec);
				TRACE(TEXT("Lost Packets     Late Packets     Bad Packets \r\n")
					TEXT("          ------------     ------------     ------------\r\n")
					TEXT("          %12d     %12d     %12d\r\n"),
					g_ccClientConns[iConn].TotalLostPackets,
					g_ccClientConns[iConn].TotalLatePackets,
					g_ccClientConns[iConn].TotalBadPackets);
			}
			
		}
		else //if(!g_fVerbose)
		{
			Sleep(30000);
			
			TRACE(TEXT("Received %d packets, containing %d bytes total\r\n"),
				g_dwTotalPacketsIn, g_dwTotalBytesIn);
		}
		
		
    }
	
    return(0);
}
//**************************************************************************************
//*** WaitForWriteableSocket - Call select on the socket to see when it is writeable ***
//**************************************************************************************
BOOL WaitForWriteableSocket(SOCKET sockServ)
{
	FD_SET	writefds;
	INT		iRet;

	if(sockServ == INVALID_SOCKET)
		return FALSE;

	// Clear all the FD_SET structures
	FD_ZERO(&writefds);
	FD_SET(sockServ, &writefds);

//	TRACE(TEXT("Calling select... Looking for writeability\r\n"));

	// Do a blocking select
	if((iRet = select( 0, NULL, &writefds, NULL, NULL)) == SOCKET_ERROR)
	{
        TRACE(TEXT("Sledge[]: select() error %s\r\n"), GetLastErrorText());
        return FALSE;
	}

	// If select returned but didn't fill any of the FD_SET structures, something is wrong
	if(!iRet)
	{
        TRACE(TEXT("Sledge[]: select() didn't return any sockets\r\n"));
        return FALSE;
	}
	
	// If the socket isn't ready for writing, then something else must have happened... This is a failure.
	if(!FD_ISSET(sockServ, &writefds))
	{
        TRACE(TEXT("Sledge[]: writing socket failed\r\n"));
        return FALSE;
	}

//	TRACE(TEXT("Socket is writeable!\r\n"));

	// There is pending data or a pending connection
	return TRUE;
}

//************************************************************************************
//*** WaitForReadableSocket - Call select on the socket to see when it is readable ***
//************************************************************************************
BOOL WaitForReadableSocket(SOCKET sockServ)
{
	FD_SET	readfds;
	INT		iRet;

	if(sockServ == INVALID_SOCKET)
		return FALSE;

	// Clear all the FD_SET structures
	FD_ZERO(&readfds);
	FD_SET(sockServ, &readfds);

//	TRACE(TEXT("Calling select... Looking for readability\r\n"));

	// Do a blocking select
	if((iRet = select( 0, &readfds, NULL, NULL, NULL)) == SOCKET_ERROR)
	{
        TRACE(TEXT("Sledge[]: select() error %s\r\n"), GetLastErrorText());
        return FALSE;
	}

	// If select returned but didn't fill any of the FD_SET structures, something is wrong
	if(!iRet)
	{
        TRACE(TEXT("Sledge[]: select() didn't return any sockets\r\n"));
        return FALSE;
	}
	
	// If the socket isn't ready for reading, then something else must have happened... This is a failure.
	if(!FD_ISSET(sockServ, &readfds))
	{
        TRACE(TEXT("Sledge[]: reading socket failed\r\n"));
        return FALSE;
	}

//	TRACE(TEXT("Socket is readable!\r\n"));

	// There is pending data or a pending connection
	return TRUE;
}

//**************************************************************************************
//*** TCPServThread - Thread that handles incoming connections from TCP client tests ***
//**************************************************************************************
DWORD WINAPI TCPServThread(LPVOID *pParm)
{
    SOCKADDR_IN ServSockAddr = { AF_INET };
    SOCKADDR_IN PeerSockAddr;
    int         sizeofSockAddr;
	
    SOCKET      ServSock;
    SOCKET      NewSock;
	
    HANDLE      hThread;
    DWORD       dwThreadId;
	ULONG		uNonBlockParam = TRUE;
	
    int         iConn;
	
	// Set the local socket parameters
    memset(&ServSockAddr, 0, sizeof(struct sockaddr));
    ServSockAddr.sin_port        = htons(TCP_SERV_PORT);
    ServSockAddr.sin_addr.s_addr = INADDR_ANY;
    ServSockAddr.sin_family = AF_INET;
	
	// Create the socket
    if ((ServSock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        TRACE(TEXT("Sledge[%s]: socket() failed %s\r\n"), CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
        ExitProcess(0);
    }

	// If necessary, set the socket for non-blocking IO
	if(g_fNonBlocking)
	{
//		TRACE(TEXT("Setting server socket for non-blocking IO\r\n"));
		if(ioctlsocket(ServSock, FIONBIO, &uNonBlockParam) == SOCKET_ERROR)
		{
			TRACE(TEXT("Sledge[%s]: ioctlsocket() error %s while setting to non-blocking\r\n"),
				CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
			ExitProcess(0);
		}
	}
    
	// Bind the socket to the local address
    if (bind(ServSock, (const struct sockaddr *) &ServSockAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        TRACE(TEXT("Sledge[%s]: bind() error %s\r\n"), CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
        ExitProcess(0);
    }
	
	// Set the socket for accepting incoming connections
    if (listen(ServSock, SERV_BACKLOG) == SOCKET_ERROR)
    {
        TRACE(TEXT("Sledge[%s]: listen() error %s\r\n"), CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
        ExitProcess(0);
    }
	
    while(1)
    {
		// Initialize the incoming address information
        sizeofSockAddr = sizeof(SOCKADDR_IN);
        memset(&PeerSockAddr, 0, sizeof (struct sockaddr));
        PeerSockAddr.sin_family = AF_INET;

		// Accept the pending connection
        if ((NewSock = accept(ServSock, (struct sockaddr *) &PeerSockAddr, &sizeofSockAddr)) == INVALID_SOCKET)
        {
			if(g_fNonBlocking)
			{
				// If we are using a non-blocking socket and the socket would have blocked, wait for incoming connections
				if(WSAGetLastError() == WSAEWOULDBLOCK)
				{
//					TRACE(TEXT("Server thread would have blocked.  Wait for incoming connection\r\n"));
					if(!WaitForReadableSocket(ServSock))
						ExitProcess(0);

//					TRACE(TEXT("Select indicated that there is a pending connection.  Calling accept\r\n"));
					if ((NewSock = accept(ServSock, (struct sockaddr *) &PeerSockAddr, &sizeofSockAddr)) == INVALID_SOCKET)
					{
						TRACE(TEXT("Sledge[%s]: accept() error %s (after select returned success!)\r\n"),
							CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
						ExitProcess(0);
					}

				}
			}
			else
			{
				TRACE(TEXT("Sledge[%s]: accept() error %s\r\n"), CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
				ExitProcess(0);
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
            TRACE(TEXT("Sledge[%s]: g_scServConns[iConn].ConnType != CONN_IDLE\r\n"));
            ExitProcess(0);
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
		
        LeaveCriticalSection(&g_csSledgeCritSection);
        
        if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ServRecvThread, &g_scServConns[iConn], 0, &dwThreadId)) == NULL)
        {
            TRACE(TEXT("Sledge[%d,%s]: CreateThread(ServRecvThread) failed %d\r\n"),
				iConn, 
				CONN_TYPES_Str[g_scServConns[iConn].ConnType],
				GetLastError());
			
            ExitProcess(0);
        }
		
        CloseHandle(hThread);
    }        
	
    return(0);
}

//**************************************************************************************
//*** UDPServThread - Thread that handles incoming connections from UDP client tests ***
//**************************************************************************************
DWORD WINAPI UDPServThread(LPVOID *pParm)
{
    SOCKADDR_IN ServSockAddr = { AF_INET };
	
    SOCKET      ServSock;
	
    HANDLE      hThread;
    DWORD       dwThreadId;
	ULONG		uNonBlockParam = TRUE;
	
    int         iConn;
	
	// Set the local socket parameters
    memset(&ServSockAddr, 0, sizeof(struct sockaddr));
    ServSockAddr.sin_port        = htons(UDP_SERV_PORT);
    ServSockAddr.sin_addr.s_addr = INADDR_ANY;
    ServSockAddr.sin_family = AF_INET;
	
	// Create the socket
    if ((ServSock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        TRACE(TEXT("Sledge[%s]: socket() failed %s\r\n"), CONN_TYPES_Str[CONN_UDP], GetLastErrorText());
        ExitProcess(0);
    }
	
	// If necessary, set the socket for non-blocking IO
	if(g_fNonBlocking)
	{
//		TRACE(TEXT("Setting server socket for non-blocking IO\r\n"));

		if(ioctlsocket(ServSock, FIONBIO, &uNonBlockParam) == SOCKET_ERROR)
		{
			TRACE(TEXT("Sledge[%s]: ioctlsocket() error %s while setting to non-blocking\r\n"),
				CONN_TYPES_Str[CONN_UDP], GetLastErrorText());
			ExitProcess(0);
		}
	}

	// Bind the socket to the local address
    if (bind(ServSock, (const struct sockaddr *) &ServSockAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        TRACE(TEXT("Sledge[%s]: bind() error %s\r\n"), CONN_TYPES_Str[CONN_UDP], GetLastErrorText());
        ExitProcess(0);
    }
	
	EnterCriticalSection(&g_csSledgeCritSection);
	
	for (iConn = 0; iConn < MAX_SERV_CONNS; iConn++)        
	{
		if (g_scServConns[iConn].ConnType == CONN_IDLE)
			break;
	}
	
	if (g_scServConns[iConn].ConnType != CONN_IDLE)
	{
		TRACE(TEXT("Sledge[%s]: g_scServConns[iConn].ConnType != CONN_IDLE\r\n"));
		ExitProcess(0);
	}
	
	g_iServConns++;
	
	g_scServConns[iConn].ConnType         = CONN_UDP;
	g_scServConns[iConn].Sock             = ServSock;
	g_scServConns[iConn].PacketLenMin     = 9999;
	g_scServConns[iConn].PacketLenMax     = 0;
	g_scServConns[iConn].TotalPacketsLoop = 0;
	g_scServConns[iConn].TotalBytesLoop   = 0;
	g_scServConns[iConn].DeltaBytesLoop   = 0;
	g_scServConns[iConn].SleepyRecv       = FALSE;
	g_scServConns[iConn].InSend           = FALSE;
	g_scServConns[iConn].InRecv           = FALSE;
	g_scServConns[iConn].InClose          = FALSE;
	
	LeaveCriticalSection(&g_csSledgeCritSection);
	
	if ((hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ServRecvThread, &g_scServConns[iConn], 0, &dwThreadId)) == NULL)
	{
		TRACE(TEXT("Sledge[%d,%s]: CreateThread(ServRecvThread) failed %d\r\n"),
			iConn, CONN_TYPES_Str[g_scServConns[iConn].ConnType],
			GetLastError());
		
		ExitProcess(0);
	}
	
	CloseHandle(hThread);
	
    return(0);
}

//********************************************************************************************
//*** ServRecvThread - Thread that handles receiving and echoing data back to client tests ***
//********************************************************************************************
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
		TRACE(TEXT("Sledge[%d,%s]: unable to allocate packet\r\n"), pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
		
        ExitProcess(0);
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
			
            if ((BytesRead = recv(pConn->Sock, (char *) Packet,	g_SledgeParms.ServRecvLen, 0)) == SOCKET_ERROR)
            {
				//If this is a non-blocking socket and the recv completes asynchornously, then this isn't an error
				if(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
				{
//					TRACE(TEXT("Server thread would have blocked.  Wait for incoming data\r\n"));

					if(WaitForReadableSocket(pConn->Sock))
					{	

//						TRACE(TEXT("Select indicated that there is pending data.  Calling recv\r\n"));
						if((BytesRead = recv(pConn->Sock,(char *) Packet,g_SledgeParms.ServRecvLen,0)) != SOCKET_ERROR)
						{
							goto TCPReadOK;
						}
					}
				}
				
                pConn->InRecv = FALSE;
				
				// If we are using in flight closes
                if (g_SledgeParms.InFlightCloses)
                {
					// If this wasn't caused by the HatchetThread, then exit
                    if (GetLastError() != WSAECONNRESET && GetLastError() != WSAECONNABORTED && GetLastError() != WSAEINVAL)
                    {
                        TRACE(TEXT("Sledge[%d,%s]: recv() failed %s\r\n"),
							pConn->Index, 
							CONN_TYPES_Str[pConn->ConnType], 
							GetLastErrorText());
						
                        ExitProcess(0);
                    }
					// If this was caused by the HatchetThread, handle it the error gracefully
                    else
                    {   
                        TRACE(TEXT("Sledge[%d,%s]: ServRecvThread exiting on expected error %s\r\n"),
							pConn->Index, 
							CONN_TYPES_Str[pConn->ConnType], 
							GetLastErrorText());
						
                        ExpectedErrorExit = TRUE;
                        goto ExitServRecvThread;
                    }
                }
                else
                {
                    TRACE(TEXT("Sledge[%d,%s]: recv() failed %s\r\n"),
						pConn->Index, 
						CONN_TYPES_Str[pConn->ConnType], 
						GetLastErrorText());
					
                    ExitProcess(0);
                }
            }
			
			// The recv succeeded
TCPReadOK:
            pConn->InRecv = FALSE;
			
            if (BytesRead == 0)
            {   
                ExpectedErrorExit = FALSE;
                TRACE(TEXT("Sledge[%d,%s]: ServRecvThread recv returned 0 bytes\r\n"), pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
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
//						TRACE(TEXT("Server thread would have blocked.  Wait for buffer to empty.\r\n"));

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
							TRACE(TEXT("Sledge[%d,%s]: send() failed %s\r\n"), 
								pConn->Index, 
								CONN_TYPES_Str[pConn->ConnType], 
								GetLastErrorText());
							
							ExitProcess(0);
						}
						// If this was caused by the HatchetThread, handle it the error gracefully
						else
						{   
							TRACE(TEXT("Sledge[%d,%s]: ServRecvThread exiting on expected error %s\r\n"),
								pConn->Index, 
								CONN_TYPES_Str[pConn->ConnType], 
								GetLastErrorText());
							
							ExpectedErrorExit = TRUE;
							goto ExitServRecvThread;
						}
					}
					else
					{
						TRACE(TEXT("Sledge[%d,%s]: send() failed %s\r\n"),
							pConn->Index, 
							CONN_TYPES_Str[pConn->ConnType], 
							GetLastErrorText());
						
						ExitProcess(0);
					}
				}
				
				pConn->InSend = FALSE;
				TotalBytesSent += BytesSent;
			}

            if (TotalBytesSent != BytesRead)
            {
                TRACE(TEXT("Sledge[%d,%s]: TotalBytesSent != BytesRead\r\n"), pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
                ExitProcess(0);
            }
			
			
            if (EOFRcvd)
            {   
//                TRACE(TEXT("Sledge[%d,%s]: EOFRcvd\r\n"), pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
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
//					  TRACE(TEXT("Server thread would have blocked.  Wait for incoming data\r\n"));

					  if(WaitForReadableSocket(pConn->Sock))
					  {	
//						  TRACE(TEXT("Select indicated that there is pending data.  Calling recvfrom\r\n"));

						  if((BytesRead =
							  recvfrom(pConn->Sock, Packet, PACKET_MAX_LEN, 0, (struct sockaddr *) &PeerSockAddr, &PeerLen))
							  != SOCKET_ERROR)
						  {
							  goto UDPReadOK;
						  }
					  }
				  }
				  
				  pConn->InRecv = FALSE;
				  
				  if (g_SledgeParms.InFlightCloses)
				  {
					  if (GetLastError() != WSAECONNRESET &&
						  GetLastError() != WSAECONNABORTED &&
						  GetLastError() != WSAEINVAL)
					  {
						  TRACE(TEXT("Sledge[%d,%s]: recv() failed %s\r\n"),
							  pConn->Index,
							  CONN_TYPES_Str[pConn->ConnType],
							  GetLastErrorText());
						  
						  ExitProcess(0);
					  }
					  else
					  {   
						  ExpectedErrorExit = TRUE;
						  goto ExitServRecvThread;
					  }
				  }
				  else
				  {
					  TRACE(TEXT("Sledge[%d,%s]: recv() failed %s\r\n"),
						  pConn->Index,
						  CONN_TYPES_Str[pConn->ConnType], 
						  GetLastErrorText());
					  
					  ExitProcess(0);
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
			  
			  pConn->InSend = TRUE;
			  
			  if ((BytesSent = sendto(pConn->Sock, Packet, BytesRead, 0,
				  (const struct sockaddr *) &PeerSockAddr, PeerLen)) 
				  == SOCKET_ERROR)
			  {
				  
			  		//Wait for 100ms between sendto attempts...
				  while(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
				  {
//					  TRACE(TEXT("Server thread would have blocked.  Wait for buffer to empty\r\n"));

					  if ((BytesSent =
						  sendto(pConn->Sock,Packet,BytesRead,0,(const struct sockaddr *)&PeerSockAddr,PeerLen))
						  != SOCKET_ERROR)
					  {
						  if(BytesSent < BytesRead)
						  {
							  TRACE(TEXT("Sledge[%d,%s]: sendto() sent a UDP fragment!\r\n"),
								  pConn->Index, 
								  CONN_TYPES_Str[pConn->ConnType]);
							  
							  ExitProcess(0);
						  }

						  goto UDPWriteOK;
					  }
					  
					  Sleep(100);
				  }
				  
				  pConn->InSend = FALSE;
				  
				  if (g_SledgeParms.InFlightCloses)
				  {
					  if (GetLastError() != WSAECONNRESET &&
						  GetLastError() != WSAEDISCON &&
						  GetLastError() != WSAECONNABORTED &&
						  GetLastError() != WSAEINVAL)
					  {
						  TRACE(TEXT("Sledge[%d,%s]: send() failed %s\r\n"),
							  pConn->Index, 
							  CONN_TYPES_Str[pConn->ConnType], 
							  GetLastErrorText());
						  
						  ExitProcess(0);
					  }
					  else
					  {   
						  ExpectedErrorExit = TRUE;
						  goto ExitServRecvThread;
					  }
				  }
				  else
				  {
					  TRACE(TEXT("Sledge[%d,%s]: send() failed %s\r\n"),
						  pConn->Index,
						  CONN_TYPES_Str[pConn->ConnType], 
						  GetLastErrorText());
					  
					  ExitProcess(0);
				  }
			  }

// The sendto succeeded
UDPWriteOK:

			  pConn->InSend = FALSE;
			  
			  if (BytesSent != BytesRead)
			  {
				  TRACE(TEXT("Sledge[%d,%s]: BytesSent != BytesRead\r\n"), pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
				  ExitProcess(0);
			  }
			  
			  
			  if (EOFRcvd)
			  {   
//				  TRACE(TEXT("Sledge[%d,%s]: EOFRcvd\r\n"), pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
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
        
        TRACE(TEXT("Sledge[%d,%s]: closesocket() error %s\r\n"),
			pConn->Index, 
			CONN_TYPES_Str[pConn->ConnType],
			GetLastErrorText());
		
        ExitProcess(0);
		
    }
	
    pConn->InClose = FALSE;
	
    EnterCriticalSection(&g_csSledgeCritSection);
	
    g_iServConns--;
	
    pConn->ConnType = CONN_IDLE;
	
    LeaveCriticalSection(&g_csSledgeCritSection);
	
	LocalFree (Packet);
	
    return(0);
}

//*************************************************************************
//*** ClientSendThread - Thread that handles sending data to the server ***
//*************************************************************************
DWORD WINAPI ClientSendThread(LPVOID *pParm)
{
    CLIENT_CONNS *pConn = (CLIENT_CONNS *) pParm;
    BOOL  SocketClosed  =  FALSE;
    BYTE *Packet;
	
    int   Reps, Len; 
	
	Packet = (BYTE *) LocalAlloc(LPTR, PACKET_MAX_LEN);
	
	if (NULL == Packet)
	{
		TRACE(TEXT("Sledge[%d,%s]: unable to allocate packet\r\n"),
			pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
		
        ExitProcess(0);
    }
	
	for	(Reps = 0; Reps < g_SledgeParms.PacketsPerConn && ! SocketClosed; Reps++)
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
		
        switch(pConn->ConnType)
        {
		case CONN_TCP:
            if (SendPacketSTREAM(pConn, Len, Packet) != 0)
                SocketClosed = TRUE;
			break;
		case CONN_UDP:
            if (SendPacketDGRAM(pConn, Len, Packet) != 0)
                SocketClosed = TRUE;
            break;
		default:
			TRACE(TEXT("Sledge[%d,%s]: invalid connection type\r\n"),
				pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
			ExitProcess(0);
			break;
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
				TRACE(TEXT("Sledge[%d,%s]: pConn->PacketsOutHead == pConn->PacketsOutTail\r\n"), 
					pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
                ExitProcess(0);
            }
            
            LeaveCriticalSection(&g_csSledgeCritSection);
        }
		
		if((pConn->ConnType == CONN_UDP) && (Reps < g_SledgeParms.PacketsPerConn))
			Sleep(g_SledgeParms.UDPSleepBeforeNext);
		
		if (SetEvent(pConn->hPacketsOutEvent) != TRUE)
		{	
            TRACE(TEXT("Sledge[%d,%s]: SetEvent(hPacketsOutEvent) failed %d\r\n"), 
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
			TRACE(TEXT("Sledge[%d,%s]: pConn->PacketsOutHead == pConn->PacketsOutTail\r\n"),
				pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
            ExitProcess(0);
        }
		
        LeaveCriticalSection(&g_csSledgeCritSection);
		
        if (SetEvent(pConn->hPacketsOutEvent) != TRUE)
        {	
            TRACE(TEXT("Sledge[%d,%s]: SetEvent(hPacketsOutEvent) failed %d\r\n"), 
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
			TRACE(TEXT("Sledge[%d,%s]: invalid connection type\r\n"),
				pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
			ExitProcess(0);
			break;
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
			TRACE(TEXT("Sledge[%d,%s]: pConn->PacketsOutHead == pConn->PacketsOutTail\r\n"), 
				pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
			ExitProcess(0);
		}
		
        LeaveCriticalSection(&g_csSledgeCritSection);
		
        if (SetEvent(pConn->hPacketsOutEvent) != TRUE)
        {	
            TRACE(TEXT("Sledge[%d,%s]: SetEvent(hPacketsOutEvent) failed %d\r\n"), 
				pConn->Index, CONN_TYPES_Str[pConn->ConnType],
				GetLastError());
        }
		
	}
	
	LocalFree (Packet);
	
	return(0);
}

//*****************************************************************************
//*** ClientRecvThread - Thread that handles receiving data from the server ***
//*****************************************************************************
DWORD WINAPI ClientRecvThread(LPVOID *pParm)
{
    CLIENT_CONNS *pConn = (CLIENT_CONNS *) pParm;
    BOOL  SocketClosed  =  FALSE, TimedOut;
	
	int   Result = 0;
	
    BYTE *Packet, PacketNumber;
	
	int	  CurrentPacketsOutCount, LastPacketExpected, MaxPacketExpected;
	int   CurrentPacketExpected, SkippedPackets;
    int   Reps, Len, Offset;
	
	Packet = (BYTE *) LocalAlloc(LPTR, PACKET_MAX_LEN);
	
	if (NULL == Packet)
	{
		TRACE(TEXT("Sledge[%d,%s]: unable to allocate packet\r\n"),
			pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
		
        ExitProcess(0);
    }
	
	while(1)
	{
		WaitForSingleObject(pConn->hPacketsOutEvent, INFINITE);
		
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
					
					LeaveCriticalSection(&g_csSledgeCritSection);
					continue;
				}
				
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
					TRACE(TEXT("Sledge[%d,%s]: invalid connection type\r\n"), pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
					ExitProcess(0);
					break;
				}
				
				if (CheckSum(Len, Packet) != pConn->PacketsOut[pConn->PacketsOutTail].CheckSum)
				{
					TRACE(TEXT("Sledge[%d,%s]: CheckSum failed, packet len %d\r\n"), 
						pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
						Len);
					
					TRACE(TEXT("Sledge[%d,%s]: CheckSum expected %d, received %d\r\n"), 
						pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
						pConn->PacketsOut[pConn->PacketsOutTail].CheckSum,
						CheckSum(Len, Packet));
					
					SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
					
					TRACE(TEXT(" Client            Packet Size\r\n"));
					TRACE(TEXT("Connection Cycle  Min  Max  Avg   Packets      Bytes    Bytes/s\r\n"));
					TRACE(TEXT("---------- ----- ---- ---- ---- ----------- ----------- -------\r\n"));
					
					TRACE(TEXT("%3d %6s %5d %4d %4d %4d %11d %11d %7d (send)\r\n"),
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
                    
					TRACE(TEXT("                                %11d %11d %7d (recv)\r\n"),
						pConn->TotalPacketsIn,
						pConn->TotalBytesIn,
						0);
					ExitProcess(0);
				}
				
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
	
	LocalFree (Packet);
	
	return(0);
}

//*******************************************************
//*** OpenConn - Establish a connection to the server ***
//*******************************************************
void OpenConn(CLIENT_CONNS *pConn)
{
	
    SOCKADDR_IN		DstAddrIP  = { AF_INET }, LclAddrIP = { AF_INET };
	ULONG			uNonBlockParam = TRUE;
	
    int           RepeatCount = 0;
  
    switch(pConn->ConnType)
    {		
	case CONN_TCP:        
        DstAddrIP.sin_port        = htons(TCP_SERV_PORT);
        DstAddrIP.sin_addr.s_addr = htonl(g_SledgeParms.ServTCPAddr);
		
        if ((pConn->Sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        {
            TRACE(TEXT("Sledge[%d,%s]: socket() failed %s\r\n"),
				pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
				GetLastErrorText());
			
            ExitProcess(0);
        }
		
		// If necessary, set the socket for non-blocking IO
		if(g_fNonBlocking)
		{
//			TRACE(TEXT("Setting client socket for non-blocking IO\r\n"));

			if(ioctlsocket(pConn->Sock, FIONBIO, &uNonBlockParam) == SOCKET_ERROR)
			{
				TRACE(TEXT("Sledge[%s]: ioctlsocket() error %s while setting to non-blocking\r\n"),
					CONN_TYPES_Str[CONN_TCP], GetLastErrorText());
				ExitProcess(0);
			}
		}

        while (1)
        { 
            if (connect(pConn->Sock, (const struct sockaddr *) &DstAddrIP,  sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
            {
				// If this test is non-blocking, then wait for the connection to complete
				if(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
				{
//					TRACE(TEXT("Client thread would have blocked.  Wait for connection to complete\r\n"));

					if(!WaitForWriteableSocket(pConn->Sock))
					{
						TRACE(TEXT("Sledge[%d,%s]: Failed waiting for connection to complete\r\n"),
							pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
						ExitProcess(0);
					}
					else
						break;
				}

                if (WSAGetLastError() == WSAECONNREFUSED)
                {
                    if (RepeatCount++ == CLIENT_TCP_CONNECT_RETRIES)
                    {
                        TRACE(TEXT("Sledge[%d,%s]: connect(0x%02X%02X 0x%02X%02X 0x%02X%02X%02X%02X) failed %s (%d times)\r\n"), 
							pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
							((BYTE *) &DstAddrIP)[0], ((BYTE *) &DstAddrIP)[1],
							((BYTE *) &DstAddrIP)[2], ((BYTE *) &DstAddrIP)[3],
							((BYTE *) &DstAddrIP)[4], ((BYTE *) &DstAddrIP)[5],
							((BYTE *) &DstAddrIP)[6], ((BYTE *) &DstAddrIP)[7],
							GetLastErrorText(),
							CLIENT_TCP_CONNECT_RETRIES);
						
                        ExitProcess(0);
                    }
                }
                else
                {
                    TRACE(TEXT("Sledge[%d,%s]: connect(0x%02X%02X 0x%02X%02X 0x%02X%02X%02X%02X) failed %s\r\n"), 
						pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
						((BYTE *) &DstAddrIP)[0], ((BYTE *) &DstAddrIP)[1],
						((BYTE *) &DstAddrIP)[2], ((BYTE *) &DstAddrIP)[3],
						((BYTE *) &DstAddrIP)[4], ((BYTE *) &DstAddrIP)[5],
						((BYTE *) &DstAddrIP)[6], ((BYTE *) &DstAddrIP)[7],
						GetLastErrorText());
					
                    ExitProcess(0);
                }
				Sleep(1000);
            }
            else
                break;
        }
		
        break;
		
	case CONN_UDP:
		LclAddrIP.sin_family      = AF_INET;
		LclAddrIP.sin_port        = htons(UDP_SERV_PORT);
		LclAddrIP.sin_addr.s_addr = INADDR_ANY;
		
        if ((pConn->Sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
        {
            TRACE(TEXT("Sledge[%d,%s]: socket() failed %s\r\n"),
				pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
				GetLastErrorText());
			
            ExitProcess(0);
        }

		// If necessary, set the socket for non-blocking IO
		if(g_fNonBlocking)
		{	
//			TRACE(TEXT("Setting client socket for non-blocking IO\r\n"));

			if(ioctlsocket(pConn->Sock, FIONBIO, &uNonBlockParam) == SOCKET_ERROR)
			{
				TRACE(TEXT("Sledge[%s]: ioctlsocket() error %s while setting to non-blocking\r\n"),
					CONN_TYPES_Str[CONN_UDP], GetLastErrorText());
				ExitProcess(0);
			}
		}

		if (bind(pConn->Sock, (const struct sockaddr *) &LclAddrIP, 
			sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
            TRACE(TEXT("Sledge[%d,%s]: bind() failed %s\r\n"),
				pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
				GetLastErrorText());
			
            ExitProcess(0);
		}
        break;	
	default:
		TRACE(TEXT("Sledge[%d,%s]: invalid connection type\r\n"),
			pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
		ExitProcess(0);
		break;
		
    }
	
    pConn->Connected = TRUE;
}

//**********************************************
//*** CloseConn - Close connection to server ***
//**********************************************
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
                    TRACE(TEXT("Sledge[%d,%s]: closesocket() failed %s\r\n"),
						pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
						GetLastErrorText());
					
                    ExitProcess(0);
                }
            }
            else
            {
                TRACE(TEXT("Sledge[%d,%s]: closesocket() failed %s\r\n"),
					pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
					GetLastErrorText());
				
                ExitProcess(0);
            }
        }
        
        pConn->InClose = FALSE;
        break;
	default:
		TRACE(TEXT("Sledge[%d,%s]: invalid connection type\r\n"),
			pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
		ExitProcess(0);
		break;
    }
	
    pConn->Connected = FALSE;
}

//*****************************************************************************************
//*** RecvPacketSTREAM - Receive packet from reliable data source (e.g. TCP connection) ***
//*****************************************************************************************
int RecvPacketSTREAM(CLIENT_CONNS *pConn, int Len, BYTE Buff[],
                 BOOL ExpectingClose)
{
    BOOL   Loop = FALSE;
	int    TotalBytesRead = 0;
	ULONG  BytesRead;
	
    if (ExpectingClose)
        Len = 1;
    
	while (TotalBytesRead < Len)
	{
		
        Loop = TRUE;
		
		pConn->InRecv = TRUE;
		
        if ((BytesRead = recv(pConn->Sock, (char *) Buff + TotalBytesRead,
			Len - TotalBytesRead, 0)) == SOCKET_ERROR)
        {
			// If this is a non-blocking test, wait for pending data before calling recv again
			if(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
			{
//				TRACE(TEXT("Client thread would have blocked.  Wait for pending data\r\n"));
				if(!WaitForReadableSocket(pConn->Sock))
					ExitProcess(0);
				
//				TRACE(TEXT("Select indicated that there is pending data.  Calling recv\r\n"));
				if ((BytesRead = recv(pConn->Sock,(char *) Buff + TotalBytesRead,Len - TotalBytesRead, 0)) != SOCKET_ERROR)
					goto TCPReadOK;
			}

            pConn->InRecv = FALSE;
            
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
					
                    TRACE(TEXT("Sledge[%d,%s]: recv() failed %s(%u)\r\n"),
						pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
						GetLastErrorText(), WSAGetLastError());
					
                    ExitProcess(0);
                }
                else
                    return(DEATH_BY_HATCHET);
            }
            else
            {
				SOCKADDR_IN	addrLocal;
				INT			addrLen;
				
				addrLen = sizeof(SOCKADDR_IN);
				getsockname(pConn->Sock, (struct sockaddr *) &addrLocal, &addrLen);
				
				TRACE(TEXT("Sledge[%d,%s]: recv() failed %s(%u) on port 0x%x on cycle %d, %d packets out, %d packets in\r\n"),
					pConn->Index, CONN_TYPES_Str[pConn->ConnType], GetLastErrorText(), WSAGetLastError(),
					ntohs(addrLocal.sin_port), pConn->Cycle, pConn->TotalPacketsOut, pConn->TotalPacketsIn);
				
                ExitProcess(0);
            }
        }
		
// The recv succeeded
TCPReadOK:
        pConn->InRecv = FALSE;
		
        if (ExpectingClose && BytesRead == 0)
            return(0);
		
        if ((! ExpectingClose) && BytesRead == 0)
        {
            TRACE(TEXT("Sledge[%d,%s]: recv() unexpected socket close\r\n"),
				pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
			
            ExitProcess(0);
        }
		
		TotalBytesRead += BytesRead;
	}
	
    return(0);
}

//*****************************************************************************************
//*** RecvPacketSTREAM - Receive packet from datagram data source (e.g. UDP connection) ***
//*****************************************************************************************
int RecvPacketDGRAM(CLIENT_CONNS *pConn, int *Len, BYTE Buff[])
{
    BOOL         Loop = FALSE;
	ULONG        BytesRead;
	
	pConn->InRecv = TRUE;
	
	if ((BytesRead = recvfrom(pConn->Sock, (char *) Buff, *Len, 0, NULL, NULL)) == SOCKET_ERROR)
	{
		// If this is a non-blocking test, wait for pending data before calling recv again
		if(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
		{
//			TRACE(TEXT("Client thread would have blocked.  Wait for pending data\r\n"));
			if(!WaitForReadableSocket(pConn->Sock))
				ExitProcess(0);
			
//			TRACE(TEXT("Select indicated that there is pending data.  Calling recvfrom\r\n"));
			if ((BytesRead = recvfrom(pConn->Sock, (char *) Buff, *Len, 0, NULL, NULL)) != SOCKET_ERROR)
				goto UDPReadOK;
		}
		
		pConn->InRecv = FALSE;
		
//		TRACE(TEXT("recvfrom generated an error.\r\n"));
		
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
				TRACE(TEXT("Sledge[%d,%s]: recv() failed %s\r\n"),
					pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
					GetLastErrorText());
				
				ExitProcess(0);
			}
			else
			{
				
				return(DEATH_BY_HATCHET);
			}
		}
		else
		{
			TRACE(TEXT("Sledge[%d,%s]: recv() failed %s\r\n"),
				pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
				GetLastErrorText());
			
			ExitProcess(0);
		}
	}

// The recvfrom succeeded
UDPReadOK:

	pConn->InRecv = FALSE;
	
	if (BytesRead == 0)
	{
		TRACE(TEXT("Sledge[%d,%s]: recv() unexpected socket close\r\n"),
			pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
		
		ExitProcess(0);
	}
	
	*Len = BytesRead;
	
    return(0);
}

//****************************************************************************************
//*** SendPacketSTREAM - Send packet via reliable communications (e.g. TCP connection) ***
//****************************************************************************************
int SendPacketSTREAM(CLIENT_CONNS *pConn, int Len, BYTE Buff[])
{
	ULONG  BytesSent, TotalBytesSent;

	if(Len <= 0)
		return (0);
	
	TotalBytesSent = 0;
	while(TotalBytesSent < (ULONG) Len)
	{
		pConn->InSend = TRUE;
		
		if ((BytesSent = send(pConn->Sock, (char *) Buff + TotalBytesSent, Len - TotalBytesSent, 0)) == SOCKET_ERROR)
		{
			// If this is a non-blocking test, wait for 100ms before calling send again
			if(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
			{
//				TRACE(TEXT("Client thread would have blocked.  Wait for buffer to empty\r\n"));

				pConn->InSend = FALSE;

				Sleep(100);
				continue;
			}
			
			pConn->InSend = FALSE;
			
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
					TRACE(TEXT("Sledge[%d,%s]: send() failed %s\r\n"),
						pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
						GetLastErrorText());
					
					ExitProcess(0);
				}
				else
					return(GetLastError());
			}
			else
			{
				SOCKADDR_IN	addrLocal;
				INT			addrLen;
				
				addrLen = sizeof(SOCKADDR_IN);
				getsockname(pConn->Sock, (struct sockaddr *) &addrLocal, &addrLen);
				
				TRACE(TEXT("Sledge[%d,%s]: send() failed %s on port 0x%x on cycle %d, %d packets out, %d packets in\r\n"),
					pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
					GetLastErrorText(), ntohs(addrLocal.sin_port), pConn->Cycle, pConn->TotalPacketsOut, pConn->TotalPacketsIn);
				
				ExitProcess(0);
				
			}
		}
		
		TotalBytesSent += BytesSent;
		pConn->InSend = FALSE;
	}
	
    if ((int) TotalBytesSent != Len)
    {
		TRACE(TEXT("Sledge[%d,%s]: TotalBytesSent (%u) != Len (%u)\r\n"), pConn->Index, CONN_TYPES_Str[pConn->ConnType],
			TotalBytesSent, Len);
        ExitProcess(0);
    }
	
    return(0);
}

//*****************************************************************************************
//*** SendPacketDGRAM - Send packet via unreliable communications (e.g. UDP connection) ***
//*****************************************************************************************
int SendPacketDGRAM(CLIENT_CONNS *pConn, int Len, BYTE Buff[])
{
	ULONG         BytesSent;
	SOCKADDR_IN   DstAddrIP = { AF_INET };
	
    pConn->InSend = TRUE;
	
    DstAddrIP.sin_port        = htons(UDP_SERV_PORT);
    DstAddrIP.sin_addr.s_addr = htonl(g_SledgeParms.ServUDPAddr);
	
	if(Len < 0)
		return (0);

    if ((BytesSent = sendto(pConn->Sock, (char *) Buff, Len, 0,
		(const struct sockaddr *) &DstAddrIP, sizeof(SOCKADDR_IN))) == SOCKET_ERROR)
    {
		//Wait for 100ms between sendto attempts...
		while(g_fNonBlocking && (WSAGetLastError() == WSAEWOULDBLOCK))
		{
//			TRACE(TEXT("Client thread would have blocked.  Wait for pending data\r\n"));

			if ((BytesSent =
				sendto(pConn->Sock, (char *) Buff, Len, 0, (const struct sockaddr *) &DstAddrIP, sizeof(SOCKADDR_IN)))
				== SOCKET_ERROR)
			{
				if(BytesSent < (ULONG) Len)
				{
					TRACE(TEXT("Sledge[%d,%s]: sendto() sent a UDP fragment!\r\n"),
						pConn->Index, 
						CONN_TYPES_Str[pConn->ConnType]);
					
					ExitProcess(0);
				}
				
				goto UDPWriteOK;
			}
			
			Sleep(100);
		}

//		TRACE(TEXT("sendto generated an error.\r\n"), Len);
		
        pConn->InSend = FALSE;
        
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
                TRACE(TEXT("Sledge[%d,%s]: send() failed %s\r\n"),
					pConn->Index, CONN_TYPES_Str[pConn->ConnType], 
					GetLastErrorText());
				
                ExitProcess(0);
            }
            else
			{
                return(GetLastError());
			}
        }
        else
        {
            TRACE(TEXT("Sledge[%d,%s]: send() failed %s\r\n"),
				pConn->Index, 
				CONN_TYPES_Str[pConn->ConnType], 
				GetLastErrorText());
			
            ExitProcess(0);
        }
    }

// The sendto succeeded
UDPWriteOK:

    pConn->InSend = FALSE;
	
    if ((int) BytesSent != Len)
    {
        TRACE(TEXT("Sledge[%d,%s]: BytesSent != Len\r\n"),
			pConn->Index, CONN_TYPES_Str[pConn->ConnType]);
		
        ExitProcess(0);
    }
	
	
    return(0);
}

//*****************************************************
//*** RandomRange - Generate a pseudo-random number ***
//*****************************************************
ULONG RandomRange(ULONG Min, ULONG Max)
{
#if defined UNDER_CE
    return((Random() % (Max - Min + 1)) + Min);
#else
    return((GetTickCount() % (Max - Min + 1)) + Min);
#endif    
} 

//**********************************************************************
//*** BuildTestPacket - Build a packet of a particular class of data ***
//**********************************************************************
ULONG BuildTestPacket(CLIENT_CONNS *pConn, int Len, BYTE Buff[], int nPacketNum)
{
    ULONG CheckSum = 0;
    int   i = 0;
	
	Buff[i] = (BYTE) nPacketNum;
	CheckSum += Buff[i];
	i++;
	
    for (; i < Len; i++)
    {
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

//***********************************************
//*** CheckSum - Calculate checksum over data ***
//***********************************************
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

//*************************************
//*** ExitProcess - End sledge test ***
//*************************************
#ifdef UNDER_CE
void ExitProcess(UINT ExitCode)
{
    g_fExit = TRUE;
    SetEvent(g_hCreateThreadsEvent);
}
#endif

//****************************************************************
//*** GetLastErrorText - Returns text of the last error's name ***
//****************************************************************
TCHAR * GetLastErrorText()
{
    switch (WSAGetLastError())
    {
	case WSAEINTR:
        return (TEXT("WSAEINTR"));
        break;
		
	case WSAEBADF:
        return(TEXT("WSAEBADF"));
        break;
		
	case WSAEACCES:
        return(TEXT("WSAEACCES"));
        break;
		
	case WSAEFAULT:
        return(TEXT("WSAEFAULT"));
        break;
		
	case WSAEINVAL:
        return(TEXT("WSAEINVAL"));
        break;
        
	case WSAEMFILE:
        return(TEXT("WSAEMFILE"));
        break;
		
	case WSAEWOULDBLOCK:
        return(TEXT("WSAEWOULDBLOCK"));
        break;
		
	case WSAEINPROGRESS:
        return(TEXT("WSAEINPROGRESS"));
        break;
		
	case WSAEALREADY:
        return(TEXT("WSAEALREADY"));
        break;
		
	case WSAENOTSOCK:
        return(TEXT("WSAENOTSOCK"));
        break;
		
	case WSAEDESTADDRREQ:
        return(TEXT("WSAEDESTADDRREQ"));
        break;
		
	case WSAEMSGSIZE:
        return(TEXT("WSAEMSGSIZE"));
        break;
		
	case WSAEPROTOTYPE:
        return(TEXT("WSAEPROTOTYPE"));
        break;
		
	case WSAENOPROTOOPT:
        return(TEXT("WSAENOPROTOOPT"));
        break;
		
	case WSAEPROTONOSUPPORT:
        return(TEXT("WSAEPROTONOSUPPORT"));
        break;
		
	case WSAESOCKTNOSUPPORT:
        return(TEXT("WSAESOCKTNOSUPPORT"));
        break;
		
	case WSAEOPNOTSUPP:
        return(TEXT("WSAEOPNOTSUPP"));
        break;
		
	case WSAEPFNOSUPPORT:
        return(TEXT("WSAEPFNOSUPPORT"));
        break;
		
	case WSAEAFNOSUPPORT:
        return(TEXT("WSAEAFNOSUPPORT"));
        break;
		
	case WSAEADDRINUSE:
        return(TEXT("WSAEADDRINUSE"));
        break;
		
	case WSAEADDRNOTAVAIL:
        return(TEXT("WSAEADDRNOTAVAIL"));
        break;
		
	case WSAENETDOWN:
        return(TEXT("WSAENETDOWN"));
        break;
		
	case WSAENETUNREACH:
        return(TEXT("WSAENETUNREACH"));
        break;
		
	case WSAENETRESET:
        return(TEXT("WSAENETRESET"));
        break;
		
	case WSAECONNABORTED:
        return(TEXT("WSAECONNABORTED"));
        break;
		
	case WSAECONNRESET:
        return(TEXT("WSAECONNRESET"));
        break;
		
	case WSAENOBUFS:
        return(TEXT("WSAENOBUFS"));
        break;
		
	case WSAEISCONN:
        return(TEXT("WSAEISCONN"));
        break;
		
	case WSAENOTCONN:
        return(TEXT("WSAENOTCONN"));
        break;
		
	case WSAESHUTDOWN:
        return(TEXT("WSAESHUTDOWN"));
        break;
		
	case WSAETOOMANYREFS:
        return(TEXT("WSAETOOMANYREFS"));
        break;
		
	case WSAETIMEDOUT:
        return(TEXT("WSAETIMEDOUT"));
        break;
		
	case WSAECONNREFUSED:
        return(TEXT("WSAECONNREFUSED"));
        break;
		
	case WSAELOOP:
        return(TEXT("WSAELOOP"));
        break;
		
	case WSAENAMETOOLONG:
        return(TEXT("WSAENAMETOOLONG"));
        break;
		
	case WSAEHOSTDOWN:
        return(TEXT("WSAEHOSTDOWN"));
        break;
		
	case WSAEHOSTUNREACH:
        return(TEXT("WSAEHOSTUNREACH"));
        break;
		
	case WSAENOTEMPTY:
        return(TEXT("WSAENOTEMPTY"));
        break;
		
	case WSAEPROCLIM:
        return(TEXT("WSAEPROCLIM"));
        break;
		
	case WSAEUSERS:
        return(TEXT("WSAEUSERS"));
        break;
		
	case WSAEDQUOT:
        return(TEXT("WSAEDQUOT"));
        break;
		
	case WSAESTALE:
        return(TEXT("WSAESTALE"));
        break;
		
	case WSAEREMOTE:
        return(TEXT("WSAEREMOTE"));
        break;
		
	case WSAEDISCON:
        return(TEXT("WSAEDISCON"));
        break;
		
	case WSASYSNOTREADY:
        return(TEXT("WSASYSNOTREADY"));
        break;
		
	case WSAVERNOTSUPPORTED:
        return(TEXT("WSAVERNOTSUPPORTED"));
        break;
		
	case WSANOTINITIALISED:
        return(TEXT("WSANOTINITIALISED"));
        break;
		
        /*
		case WSAHOST:
        return(TEXT("WSAHOST"));
        break;
		
		  case WSATRY:
		  return(TEXT("WSATRY"));
		  break;
		  
			case WSANO:
			return(TEXT("WSANO"));
			break;
        */
		
	default:
        return(TEXT("Unknown Error"));
    }
}
void
usage ()
{
	TRACE(TEXT("Sledge [-s] [-pNUM] [Many others (see below)]\r\n"));
	TRACE(TEXT("\t-n     : Non-blocking Mode (default blocking)\r\n"));
	TRACE(TEXT("\t-s     : Server Mode (default client mode)\r\n"));
	TRACE(TEXT("\t-st    : Server TCP (default %s)\r\n"),
		SERV_TCP ? TEXT("TRUE") : TEXT("FALSE"));
	TRACE(TEXT("\t-su    : Server UDP (default %s)\r\n"),
		SERV_UDP ? TEXT("TRUE") : TEXT("FALSE"));
	TRACE(TEXT("\t-slNUM : Server receive length (default %d)\r\n"),
		SERV_RECV_LEN);
	TRACE(TEXT("\t-pNUM  : Set packets per connection to NUM (default %d)\r\n"),
		PACKETS_PER_CONN);
	TRACE(TEXT("\t-mNUM  : Set minimum packet len to NUM (default %d)\r\n"),
		MIN_PACKET_LEN);
	TRACE(TEXT("\t-MNUM  : Set maximum packet len to NUM (default %d)\r\n"),
		MAX_PACKET_LEN);
	TRACE(TEXT("\t-f     : Turn on in-flight closes\r\n"));
	TRACE(TEXT("\t-tcNUM : Set TCP Connections to NUM (default %d)\r\n"),
		CLIENT_TCP_CONNS);
	TRACE(TEXT("\t-tlNUM : Set TCP Cycles to NUM (default %d)\r\n"),
		CLIENT_TCP_CYCLES);
	TRACE(TEXT("\t-taADDR : Set TCP Addr to ADDR (default 0x%X)\r\n"),
		SERV_TCP_ADDR);
	TRACE(TEXT("\t\t(example -ta192.100.1.101 or -tappp_peer\r\n"));
	
    TRACE(TEXT("\t-ssNum : Set percent server sleepy receives (default %d)\r\n"), SERV_PERCENT_SLEEPY_RECVS);
	
    TRACE(TEXT("\t-csNum : Set percent client sleepy receives (default %d)\r\n"), CLIENT_PERCENT_SLEEPY_RECVS);
}

// Print out the parms before starting SLEDGE
void Printg_SledgeParms()
{
	
	TRACE(TEXT("<< Sledge Parameters >>\r\n"));
	if (g_SledgeParms.ServTCP)
    {
		TRACE(TEXT("Running TCP\r\n"));
    }
	if (g_SledgeParms.ServUDP)
    {
		TRACE(TEXT("Running UDP\r\n"));
    }
	
	TRACE(TEXT("TCP Packet Size : Min = %d\t Max = %d\r\n"), 
	       g_SledgeParms.TCPMinPacketLen, 
		   g_SledgeParms.TCPMaxPacketLen);
	TRACE(TEXT("UDP Packet Size : Min = %d\t Max = %d\r\n"), 
	       g_SledgeParms.UDPMinPacketLen, 
		   g_SledgeParms.UDPMaxPacketLen);
	TRACE(TEXT("SERVER -> TCP ADDR = %x && UDP ADDR = %x\r\n"),
		g_SledgeParms.ServTCPAddr,
		g_SledgeParms.ServUDPAddr);
	
	TRACE(TEXT("TCP Connections = %d \t TCP Cycles = %d\r\n"), 
	       g_SledgeParms.ClientTCPConns,
		   g_SledgeParms.ClientTCPCycles);
	TRACE(TEXT("UDP Connections = %d \t UDP Cycles = %d\r\n"), 
	       g_SledgeParms.ClientUDPConns,
		   g_SledgeParms.ClientUDPCycles);
	TRACE(TEXT("ServRecvLen = %d\r\n"), g_SledgeParms.ServRecvLen);
	TRACE(TEXT("Packets Per Connection = %d\r\n"), g_SledgeParms.PacketsPerConn);
	TRACE(TEXT("In Flight Closes = %d\r\n"), g_SledgeParms.InFlightCloses);
	
}
