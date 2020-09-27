//------------------------------------------------------------------------------
// XBox Online
//
// File:		XSGTest.cpp
//
// Copyright:	Copyright (c) 2001 Microsoft Corporation
//
// Contents:	Functional test cases for the security gateway
//
// History:	01/02/2002	johnblac		Created
//
//------------------------------------------------------------------------------

#include "StdAfx.h"
#include "XSGTest.h"
#include "messages.h"

// Globals
DWORD g_dwSrcAddress = 0;
DWORD g_dwDestAddress = 0;
UINT g_uiFailedCases = 0;
UINT g_uiPassedCases = 0;
UINT g_nLogResults = 0;

BYTE g_rgbSrcMAC[6];
BYTE g_rgbDestMAC[6];
BYTE g_rgbHexTable[103];
FILE *g_pOutput;

// s1 is a UDP socket descriptor
SOCKET               g_sUDP1 = INVALID_SOCKET;
// sTCPMS is a TCP socket descriptor for the monitoring server
SOCKET               g_sTCP1 = INVALID_SOCKET;

WCHAR g_wszSG_IP[16];		// String version of SG IP address
WCHAR g_wszTester[25];		// Tester Name
WCHAR g_wszBuild[12];		// Build name
IN_ADDR g_ipSGIP;			// SG IP address
u_short g_usPort = 3074;		// SG Port
WCHAR g_wszSep[] = L".";		// IP address seperator
int g_nMilestone = -1;		// Milestone
int g_nMachineType = 2;		// Type of machine running the test


//-----------------------------------------------------------------------------
//
// atoh
//
// Converts and even length WCHAR string to hex.  The hex value is placed in the destination
// buffer.  The caller is responsible for making the destination buffer long enough.
//
BOOL atoh(BYTE* pDestHex, WCHAR* pSrcChar, UINT uiLen)
{
	if(!pDestHex || !pSrcChar || (uiLen %2 != 0))
	{
		return FALSE;
	}

	BYTE bTempVal = 0;
	
	for(UINT i = 0; i < uiLen/2; i++)
	{
		bTempVal = 16*g_rgbHexTable[*pSrcChar];
		pSrcChar++;
		bTempVal += g_rgbHexTable[*pSrcChar];
		*pDestHex = bTempVal;
		pSrcChar++;
		pDestHex++;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
//
// E
//
// Checks the error code and increments the appropriate counter.
//
void E(HRESULT hr) 
{ 
	if (FAILED(hr)) 
	{ 
		g_uiFailedCases++; 
	}
	else
	{
		g_uiPassedCases++;
	}
}


WCHAR* FindOneOf
//-----------------------------------------------------------------------------
//
// FindOneOf
//
// Modified version of ATL code that looks for an instance of string p1 in string p2 using only\
// WCHARs.
//
( 
	WCHAR* p1, 
	WCHAR* p2
)
{
    while (p1 != NULL && *p1 != NULL)
    {
        WCHAR* p = p2;
        while (p != NULL && *p != NULL)
        {
            if (*p1 == *p)
                return CharNext(p1);
            p = CharNext(p);
        }
        p1 = CharNext(p1);
    }
    return NULL;
}


//-----------------------------------------------------------------------------
//
// FillData
//
// Takes a BYTE pointer to a buffer with a given size and maximum length of the buffer.  The 
// buffer is filled with 'bogus' data.
//
HRESULT FillData(BYTE* pBuff, DWORD wSize, WORD wBufferLength)
{
	if(!pBuff || wSize > wBufferLength)
	{
		return E_FAIL;
	}

	ZeroMemory(pBuff, wBufferLength);
	WORD wCount = 0;
	while(wCount < wSize)
	{
		switch(wCount % 3)
		{
		case 0:
			*pBuff = 0xC0;
			break;
		case 1:
			*pBuff = 0xFF;
			break;
		case 2:
			*pBuff = 0xEE;
			break;
		default:
			return E_UNEXPECTED;
		}
		wCount++;
		pBuff++;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
//
// FillData
//
// Takes a BYTE pointer to a buffer with a given size and maximum length of the buffer.  The 
// buffer is filled with 'bogus' data.
//
HRESULT WriteBadFrame
(
	// [in/out] Destination buffer to contain the hex values of the input string
	BYTE* pBuff,
	//[in] Character string of hex values to be translated into memory
	char* pbHexString,
	// [in] Size of pbHexString
	DWORD wHexStringSize, 
	// [in] size of buffer pBuff points to
	WORD wBufferLength
)
{
	if(!pBuff || wHexStringSize > wBufferLength)
	{
		return E_FAIL;
	}

	ZeroMemory(pBuff, wBufferLength);
	WORD wCount = 0;
	BYTE bTempVal = 0;
	
	while(wCount < wHexStringSize)
	{
		bTempVal = 16*g_rgbHexTable[*pbHexString];
		pbHexString++;
		bTempVal += g_rgbHexTable[*pbHexString];
		*pBuff = bTempVal;
		pBuff++;
		pbHexString++;
		wCount++;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//
// GetChecksum
//
// Takes a pointer to an IP header cast to a WORD* type and calculates a checksum value for 
// that header.  This checksum is returned.
//
WORD GetChecksum(WORD * pBuff, UINT uiSize)
{
	WORD wChecksum = 0;
	WORD wPrevChecksum = 0;
	WORD wOverflow = 0;

	for(UINT ui = 0; ui < uiSize; ui++)
	{
		wChecksum += htons(*pBuff);
		pBuff++;
		
		if(wChecksum < wPrevChecksum)
			wOverflow++;

		wPrevChecksum = wChecksum;
	}

	return htons((~wChecksum)-wOverflow);
}

/*
///////////////////////////////////////
//    GetLocalMachineAddresses
//
bool GetLocalMachineAddress()
{ 
	XNADDR xThisXbox;
	static struct in_addr myaddr;

	// Get host name or token
	XNetGetTitleXnAddr(&xThisXbox);
	memcpy(&myaddr, &xThisXbox.ina, sizeof(myaddr));

	char szHostName[ k_dw2048const ];
	if( gethostname( szHostName, k_dw2048const ) != 0 )
	{
		printf( "gethostname failed! Error = %d\n", WSAGetLastError() );
		return( false );
	}
 
	// Get list of host addresses
	HOSTENT* pHostEnt = gethostbyname( szHostName );
	if( NULL == pHostEnt )
	{
		printf( "gethostbyname failed! Error = %d\n", WSAGetLastError() );
		return( false );
	}
	else
	{
		g_dwSrcAddress = htonl(*(DWORD*)(*(DWORD*)pHostEnt->h_addr_list));
	}

	return( true );
}
*/

//-----------------------------------------------------------------------------
//
// GetNewNounce
//
// This assignes the current tick count to the first 4 bytes of the array and a rand to the last 4 
// bytes.  It is a vein attempt to generate a semi-random number.
//
void GetNewNounce(
	// [in/out] Array to contain the random bits
	BYTE rgbNonce[8]
)
{
	*(DWORD*)(rgbNonce) = GetTickCount();
	*(DWORD*)(rgbNonce + sizeof(DWORD)) = (DWORD)rand();
	// TODO: replace this hack by a real random nounce
}


//-----------------------------------------------------------------------------
//
// Initialize Network
//
// This function initializes winsock and creates a socket for UDP communications.
//
HRESULT InitializeNetworkUDP(
	// [in/out] socket for communications
	SOCKET* ps1, 
	// [in/out] Socket Address for which machine to connect to
	SOCKADDR_IN* psXSGAddress
)
{
	HRESULT hr = S_OK;
	// iLastError is the last error code
	int                  iLastError = ERROR_SUCCESS;	
	// nBytes is the number of bytes sent or received on the socket
	int                  nBytes;

	ZeroMemory(psXSGAddress, sizeof(*psXSGAddress)); // BUGBUG
	
	// Create a UDP socket
	*ps1 = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == *ps1) 
	{
		// Get the last error code
		iLastError = GetLastError();
		hr = E_FAIL;
		goto FunctionExit1;
	}

	u_long ulNonblock;
	ulNonblock = 1;
	iLastError = ioctlsocket(*ps1, FIONBIO, &ulNonblock);
	if(SOCKET_ERROR == iLastError)
	{
		printf("ioctlsocket failed for s1\n");
		hr = E_FAIL;
		goto FunctionExit1;
	}

	// Set the receive timeout value to 5 sec
	if (SOCKET_ERROR == setsockopt(*ps1, SOL_SOCKET, SO_RCVTIMEO, (char *) &k_uiSocketTimeout, sizeof(k_uiSocketTimeout))) 
	{
		// Get the last error code
		iLastError = WSAGetLastError();
		hr = E_FAIL;
		goto FunctionExit1;
	}

	// Set the send timeout value to 5 sec
	if (SOCKET_ERROR == setsockopt(*ps1, SOL_SOCKET, SO_SNDTIMEO, (char *) &k_uiSocketTimeout, sizeof(k_uiSocketTimeout))) 
	{
		// Get the last error code
		iLastError = WSAGetLastError();
		hr = E_FAIL;
		goto FunctionExit1;
	}

	// Assign the port and IP
	psXSGAddress->sin_port = htons(g_usPort);
	psXSGAddress->sin_family = AF_INET;
	psXSGAddress->sin_addr.S_un.S_un_b.s_b1 = g_ipSGIP.S_un.S_un_b.s_b1;
	psXSGAddress->sin_addr.S_un.S_un_b.s_b2 = g_ipSGIP.S_un.S_un_b.s_b2;
	psXSGAddress->sin_addr.S_un.S_un_b.s_b3 = g_ipSGIP.S_un.S_un_b.s_b3;
	psXSGAddress->sin_addr.S_un.S_un_b.s_b4 = g_ipSGIP.S_un.S_un_b.s_b4;

FunctionExit1:
	return hr;
}


//-----------------------------------------------------------------------------
//
// InitGlobalArray
//
// This fills out a global array used for converting ASCII to hex.
//
void InitGlobalArray()
{
	ZeroMemory(g_rgbHexTable, sizeof(g_rgbHexTable));
	g_rgbHexTable[48] = 0x0;
	g_rgbHexTable[49] = 0x1;
	g_rgbHexTable[50] = 0x2;
	g_rgbHexTable[51] = 0x3;
	g_rgbHexTable[52] = 0x4;
	g_rgbHexTable[53] = 0x5;
	g_rgbHexTable[54] = 0x6;
	g_rgbHexTable[55] = 0x7;
	g_rgbHexTable[56] = 0x8;
	g_rgbHexTable[57] = 0x9;	
	g_rgbHexTable[65] = 0xA;
	g_rgbHexTable[66] = 0xB;
	g_rgbHexTable[67] = 0xC;
	g_rgbHexTable[68] = 0xD;
	g_rgbHexTable[69] = 0xE;
	g_rgbHexTable[70] = 0xF;
	g_rgbHexTable[97] = 0xA;
	g_rgbHexTable[98] = 0xB;
	g_rgbHexTable[99] = 0xC;
	g_rgbHexTable[100] = 0xD;
	g_rgbHexTable[101] = 0xE;
	g_rgbHexTable[102] = 0xF;
}


//-----------------------------------------------------------------------------
//
// Initialize NetworkTCP
//
// This function initializes winsock and creates a socket for UDP communications.
//
HRESULT InitializeNetworkTCP(
	// [in/out] socket for TCP communications
	SOCKET* ps1, 
	// [in/out] Socket Address for which machine to connect to
	SOCKADDR_IN* psXSGAddress,
	// [in] Port to open this socket on
	u_short uPort
)
{
	// iLastError is the last error code
	int                  iLastError = ERROR_SUCCESS;
	// iResult is the result of a function
	int                  iResult = 0;
    
	// LowPort is the low bound of the netsync port range
	u_short              LowPort = 0;
	// HighPort is the high bound of the netsync port range
	u_short              HighPort = 0;

	// readfds is the set of sockets to check for a pending connection
	fd_set               readfds;
	// iTimeout is the send and receive timeout value for the socket
	int                  iTimeout;
	// portname is the local address associated with a socket
	SOCKADDR_IN          portname = { AF_INET, 0, 0, {'\0'} };
	// toname is the recipient address information structure
	SOCKADDR_IN          toname = { AF_INET, 0, 0, {'\0'} };
	// ReceiveBuffer is the receive buffer
	char                 ReceiveBuffer[k_dw2048const];
	// nBytes is the number of bytes sent or received on the socket
	int                  nBytes;
	// Number of backlog requests we accept
	int			iBackLog = 8;
	// No delay flag
	bool bNoDelay = TRUE;
	
	char BVTBuffer[k_dw2048const] = "C0FFEEC0FFEE";
	
	// Create a TCP socket
	*ps1 = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == *ps1) 
	{
		// Get the last error code
		iLastError = GetLastError();
		goto FunctionExit4;
	}

	// Set the receive timeout value to 5 sec
	iTimeout = 5000;
	if (SOCKET_ERROR == setsockopt(*ps1, SOL_SOCKET, SO_RCVTIMEO, (char *) &iTimeout, sizeof(iTimeout))) 
	{
		// Get the last error code
		iLastError = WSAGetLastError();
		goto FunctionExit4;
	}

	// Set the send timeout value to 5 sec
	iTimeout = 5000;
	if (SOCKET_ERROR == setsockopt(*ps1, SOL_SOCKET, SO_SNDTIMEO, (char *) &iTimeout, sizeof(iTimeout))) 
	{
		// Get the last error code
		iLastError = WSAGetLastError();
		goto FunctionExit4;
	}

	if (SOCKET_ERROR == setsockopt(*ps1, IPPROTO_TCP, TCP_NODELAY, (char *)&bNoDelay, sizeof(bNoDelay))) 
	{
		// Get the last error code
		iLastError = WSAGetLastError();
		goto FunctionExit4;
	}

	// Bind TCP socket 1
	portname.sin_port = htons(uPort);
	if (SOCKET_ERROR == bind(*ps1, (SOCKADDR *) &portname, sizeof(portname))) 
	{
		// Get the last error code
		iLastError = WSAGetLastError();
		goto FunctionExit4;
	}

	// Initialize connect
	toname.sin_port = htons(uPort);
	toname.sin_addr.S_un.S_un_b = psXSGAddress->sin_addr.S_un.S_un_b;

	// Listen on TCP socket 1
	if (SOCKET_ERROR == listen(*ps1, iBackLog) )
	{
		// Get the last error code
		iLastError = WSAGetLastError();
	}

FunctionExit4:
/*	// Close the TCP socket
	if (0 != closesocket(*ps1)) 
	{
		// Get the last error code
		iLastError = WSAGetLastError();
	}
*/

	return (ERROR_SUCCESS == iLastError);
}


DWORD ReadIni
//-----------------------------------------------------------------------------
//
// ReadIni
// 
// This reads the data source information from the ini file provided on the 
// command line.  The key names are the same as what DLP uses, so you should
// be able to simply cut and paste datasources from any of your own config.ini 
// files to the ini file used for this program.
//
(
	WCHAR* ThisIniFile
	// [in] ThisIniFile - argument string pointing to the ini file
)
{
	WCHAR* pwszSrc;
	WCHAR wszDigit[4];
	WCHAR wszMAC[13];
	ZeroMemory(wszDigit, sizeof(wszDigit));
	ZeroMemory(wszMAC, sizeof(wszMAC));	
	WCHAR* pwszDest = wszDigit;

	// Loads each test cases parameters
	g_usPort = (u_short)GetPrivateProfileInt(L"TestParameters", L"SGPort", 3074, ThisIniFile);
	g_nMachineType = GetPrivateProfileInt(L"TestParameters", L"LabOrPersonalSG", 2, ThisIniFile);
	g_nMilestone = GetPrivateProfileInt(L"TestParameters", L"Milestone", 10, ThisIniFile);
	g_nLogResults = GetPrivateProfileInt(L"TestParameters", L"LogResults", 0, ThisIniFile);
	DWORD dwReturn = GetPrivateProfileString(L"TestParameters", L"SGIP", L"127.0.0.1", &g_wszSG_IP[0], NUMELEM(g_wszSG_IP), ThisIniFile);
	dwReturn = GetPrivateProfileString(L"TestParameters", L"Tester", L"Lester", &g_wszTester[0], NUMELEM(g_wszTester), ThisIniFile);
	dwReturn = GetPrivateProfileString(L"TestParameters", L"Build", L"4033.000000", &g_wszBuild[0], NUMELEM(g_wszBuild), ThisIniFile);
	dwReturn = GetPrivateProfileString(L"TestParameters", L"DestinationMAC", L"000000000000", &wszMAC[0], NUMELEM(wszMAC), ThisIniFile);
	atoh(g_rgbDestMAC, wszMAC, 12);	
	pwszSrc = g_wszSG_IP;
	pwszDest = wcstok(pwszSrc, g_wszSep);
	g_ipSGIP.S_un.S_un_b.s_b1 = (unsigned char)_wtoi( pwszDest );
	pwszDest = wcstok(NULL, g_wszSep);	
	g_ipSGIP.S_un.S_un_b.s_b2 = (unsigned char)_wtoi( pwszDest );
	pwszDest = wcstok(NULL, g_wszSep);		
	g_ipSGIP.S_un.S_un_b.s_b3 = (unsigned char)_wtoi( pwszDest );
	pwszDest = wcstok(NULL, g_wszSep);		
	g_ipSGIP.S_un.S_un_b.s_b4 = (unsigned char)_wtoi( pwszDest );

	// restores the global variable
	dwReturn = GetPrivateProfileString(L"TestParameters", L"SGIP", L"127.0.0.1", &g_wszSG_IP[0], NUMELEM(g_wszSG_IP), ThisIniFile);

	// Set the destination address
	g_dwDestAddress = g_ipSGIP.S_un.S_un_b.s_b1*16777216 +
					  g_ipSGIP.S_un.S_un_b.s_b2*65536 +
					  g_ipSGIP.S_un.S_un_b.s_b3*256 +
					  g_ipSGIP.S_un.S_un_b.s_b4;

	return dwReturn;
}


//-----------------------------------------------------------------------------
//
// ReceiveFromCS
//
// This function listens for incomming UDP packets
//
HRESULT ReceiveFromCS(
	// [in] socket to listen on
	SOCKET sSocket,
	// [out] buffer to store incomming data (from the CS) in
	BYTE* pBuffer,
	// [in] Length of above buffer
	DWORD dwLength,
	// [out] number of bytes received.
	DWORD* pdwBytesRecvd
)
{
    HRESULT hr = S_OK;

    // Prepare to probe the inbound socket for readability
    
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sSocket, &readfds);

    fd_set exceptfds;
    FD_ZERO(&exceptfds);
    FD_SET(sSocket, &exceptfds);
    
    TIMEVAL sTimeout = {0, 0}; 
	int iLastError = 0;

    // Probe the inbound socket. We don't want it to block so
    // timeout is set to zero.
    
    int iRes = select(0, &readfds, NULL, &exceptfds, &sTimeout);

    if(iRes > 0)
    {
        // One or more sockets were flagged. Let's check
        // the sets. Check for exceptions first.
                
        if(FD_ISSET(sSocket, &exceptfds))
        {
            printf("socket (recv) has exception\n");
            hr = E_FAIL;
        }
        else
        {
            // Socket is ready for reading

            ASSERT(FD_ISSET(sSocket, &readfds));
            
            // Read data from CS

            sockaddr_in sSourceAddr;
            int iSizeSourceAddr = sizeof(sSourceAddr);
                        
            int iBytesRecvd = recvfrom(sSocket, (char*) pBuffer, 
                dwLength, 0, (sockaddr*) &sSourceAddr, &iSizeSourceAddr);

            if(iBytesRecvd != SOCKET_ERROR)
            {
				iLastError = WSAGetLastError();
                if(pdwBytesRecvd)
                {
                    *pdwBytesRecvd = (DWORD) iBytesRecvd;
                }
            }
            else
            {
                iLastError = WSAGetLastError();
                hr = E_FAIL;
            }
        }
    }
    else
    {
        // No data
        hr = S_FALSE;
    }

    return hr;
}


//-----------------------------------------------------------------------------
//
// RunTestCases
//
// All of the test cases are run from here
//
HRESULT RunTestCases(
	// [in/out] Socket Address for which machine to connect to
	SOCKADDR_IN* psXSGAddress
)
{
	HRESULT hr = S_OK;

	// SG Validity Tests
	printf("...Starting SG tests...\n");
	SGValidity(&g_sUDP1, psXSGAddress, 532);

	// SG Encryption Tests
	SGEncryption(&g_sUDP1, psXSGAddress, 500);
	
	// SG Routing tests
	SGRouting(&g_sUDP1, psXSGAddress, 435);
	
	//SG Key Order tests
	SGKeyOrder(&g_sUDP1, psXSGAddress, 477);

	//SG Ticket validity tests
	SGTicketValidity(&g_sUDP1, psXSGAddress, 478);
	
	return hr;
}


//-----------------------------------------------------------------------------
//
// SendToCS
//
// This function sends data in the buffer argument to the CS via a UDP packet or packets.
//
HRESULT SendToCS(
	// [in] socket to communicate on
	SOCKET	sSocket,
	// [in] address of CS to talk to
	SOCKADDR_IN sAddr,
	// [in] buffer of data to send
	const BYTE* pBuffer,
	// [in] number of bytes in above buffer
	DWORD cchBuffer
)
{
    HRESULT hr = S_OK;
    
    // Prepare to probe socket for writeability
    
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sSocket, &writefds);

    fd_set exceptfds;
    FD_ZERO(&exceptfds);
    FD_SET(sSocket, &exceptfds);
    
    TIMEVAL sTimeout = {0, 0}; 

    // Probe the outbound socket. We don't want it to block so
    // timeout is set to zero.
    
    int iRes = select(0, NULL, &writefds, &exceptfds, &sTimeout);

    if(iRes > 0)
    {
        // One or more sockets were flagged. Let's check
        // the sets. Check for exceptions first.
                
        if(FD_ISSET(sSocket, &exceptfds))
        {
            printf("socket has exception\n");
            hr = E_FAIL;
        }
        else
        {
            // Socket is ready for writting

            ASSERT(FD_ISSET(sSocket, &writefds));
            
            // Send data to CS
            
            int iBytesSent = sendto(sSocket, (const char*) pBuffer, 
                cchBuffer, 0, (const sockaddr*) &sAddr, sizeof(sAddr));

            if(SOCKET_ERROR == iBytesSent)
            {
                printf("sendto failed to send %d bytes, error=%d\n", iBytesSent, WSAGetLastError());
                hr = E_FAIL;
            }
        }
    }
    else
    {
        // Socket is not ready
        hr = E_FAIL;
    }

    return hr;
}


//-----------------------------------------------------------------------------
//
// SGEncryption
//
// Runs the test cases that check for well encrypted packets
//
HRESULT SGEncryption(
	// [in/out] socket for communications
	SOCKET* pSocket, 
	// [in/out] Socket Address for which machine to connect to
	SOCKADDR_IN* psXSGAddress,
	// [in] Test Number
	int nTestNum
)
{
	return S_OK;
}


//-----------------------------------------------------------------------------
//
// SGKeyOrder
//
// Runs the test cases that check for proper key order and polite failure for 
// improper key order
//
HRESULT SGKeyOrder(
	// [in/out] socket for communications
	SOCKET* pSocket, 
	// [in/out] Socket Address for which machine to connect to
	SOCKADDR_IN* psXSGAddress,
	// [in] Test Number
	int nTestNum
)
{
	ENET_HEADER sEnetHdr;
	IP_HEADER sIPHdr;
	UDP_HEADER sUDPHdr;
	CKeyExXbToSgInit XBtoSGInit;	// First header
	CKeyExHdr DHKeyExHdr;	// Second header
	CKeyExKerbApReq KerbApReq;	// Third header
	WORD wSizeOfHeaders = sizeof(sEnetHdr)+sizeof(sIPHdr)+sizeof(sUDPHdr);
	BYTE* rgbFrameBuffer = new BYTE[k_w1522const];
	BYTE* rgbDataBuffer = new BYTE[k_w1522const-wSizeOfHeaders];	
	WORD wDataLen = 0;
	DWORD dwSPI = 0;

	// Set up a valid Init Structure
	XBtoSGInit._dwSpiInit = 0x99887766;
	GetNewNounce(XBtoSGInit._abNonceInit);

	// Set up a valid KerbApReq structure
//	KerbApReq._ab

	// SG_VALID_KEY_ORDER
	SetKeysToValidState(&XBtoSGInit, &DHKeyExHdr, &KerbApReq);
	// Copy the SPI
	memcpy(rgbDataBuffer, &dwSPI, sizeof(dwSPI));
	// Copy the first KeyEx Header - a XBtoSGInit
	wDataLen += sizeof(dwSPI);
	memcpy(rgbDataBuffer+wDataLen, (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	// Copy the second KeyEx Header
	wDataLen += sizeof(XBtoSGInit);
	memcpy(rgbDataBuffer+wDataLen, (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));
	// Create a 96 byte key
	wDataLen += sizeof(DHKeyExHdr);	
	FillData(rgbDataBuffer+wDataLen, k_dwDHKeySize, k_w1522const-wSizeOfHeaders);
	// Copy the third KeyEx Header
	wDataLen +=	k_dwDHKeySize;
	memcpy(rgbDataBuffer+wDataLen, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	// Copy the third matching structure
	wDataLen += sizeof(KerbApReq);
	// Copy the whole canoly to the frame buffer
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
	CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr), &sUDPHdr, sizeof(sUDPHdr));
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_VALID_KEY_ORDER\n");
	}

	// SG_SEND_MULTIPLE_KEYEX
	// Even a DH key of all 0's is still valid
	ZeroMemory(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr), k_dwDHKeySize);
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_SEND_MULTIPLE_SEQ_NUM\n");
	}	

//--------------------------
// BEGIN 1ST KEY TESTS
//--------------------------

	// SG_INVALID_1ST_1
	XBtoSGInit._wType = KEYEX_TYPE_XBTOXB_INIT;	// NULL case
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1ST_1\n");
	}
	
	// SG_INVALID_1ST_2
	XBtoSGInit._wType = KEYEX_TYPE_XBTOXB_RESP;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1ST_2\n");
	}

	// SG_INVALID_1ST_3
	XBtoSGInit._wType = KEYEX_TYPE_SGTOXB_RESP;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1ST_3\n");
	}

	// SG_INVALID_1ST_4
	XBtoSGInit._wType = KEYEX_TYPE_DH_GX;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1ST_4\n");
	}

	// SG_INVALID_1ST_5
	XBtoSGInit._wType = KEYEX_TYPE_HMAC_SHA;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1ST_5\n");
	}

	// SG_INVALID_1ST_6
	XBtoSGInit._wType = KEYEX_TYPE_KERB_APREQ;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1ST_6\n");
	}

	// SG_INVALID_1ST_7
	XBtoSGInit._wType = KEYEX_TYPE_KERB_APREP;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1ST_7\n");
	}

	// SG_INVALID_1ST_8
	XBtoSGInit._wType = KEYEX_TYPE_NULL_APREQ;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1ST_8\n");
	}

	// SG_INVALID_1ST_9
	XBtoSGInit._wType = KEYEX_TYPE_NULL_APREP;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1ST_9\n");
	}

	// SG_INVALID_1ST_10
	XBtoSGInit._wType = 10;		// Not in range
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1ST_10\n");
	}

	// SG_INVALID_1STKEY_LARGE
	XBtoSGInit._wType = KEYEX_TYPE_XBTOSG_INIT;
	XBtoSGInit._cbEnt = sizeof(CKeyExXbToSgInit)+1;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1STKEY_LARGE\n");
	}
	
	// SG_INVALID_1STKEY_SMALL
	XBtoSGInit._cbEnt = sizeof(CKeyExXbToSgInit)-1;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1STKEY_SMALL\n");
	}
	
	// SG_INVALID_1STKEY_ZERO
	XBtoSGInit._cbEnt = 0;
	memcpy(rgbDataBuffer+sizeof(dwSPI), (BYTE*)&XBtoSGInit, sizeof(XBtoSGInit));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_1STKEY_ZERO\n");
	}
	
//--------------------------
// BEGIN 2ND KEY TESTS
//--------------------------

	// SG_INVALID_2NDKEY_1
	SetKeysToValidState(&XBtoSGInit, &DHKeyExHdr, &KerbApReq);
	DHKeyExHdr._wType = KEYEX_TYPE_XBTOXB_INIT;	// NULL case
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_1\n");
	}

	// SG_INVALID_2NDKEY_2
	DHKeyExHdr._wType = KEYEX_TYPE_XBTOXB_RESP;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_2\n");
	}

	// SG_INVALID_2NDKEY_3
	DHKeyExHdr._wType = KEYEX_TYPE_XBTOSG_INIT;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_3\n");
	}

	// SG_INVALID_2NDKEY_4
	DHKeyExHdr._wType = KEYEX_TYPE_SGTOXB_RESP;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_4\n");
	}

	// SG_INVALID_2NDKEY_5
	DHKeyExHdr._wType = KEYEX_TYPE_HMAC_SHA;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_5\n");
	}

	// SG_INVALID_2NDKEY_6
	DHKeyExHdr._wType = KEYEX_TYPE_KERB_APREQ;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_6\n");
	}

	// SG_INVALID_2NDKEY_7
	DHKeyExHdr._wType = KEYEX_TYPE_KERB_APREP;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_7\n");
	}

	// SG_INVALID_2NDKEY_8
	DHKeyExHdr._wType = KEYEX_TYPE_NULL_APREQ;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_8\n");
	}

	// SG_INVALID_2NDKEY_9
	DHKeyExHdr._wType = KEYEX_TYPE_NULL_APREP;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_9\n");
	}

	// SG_INVALID_2NDKEY_10
	DHKeyExHdr._wType = 10;		// Not in range
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_10\n");
	}

	// SG_INVALID_2NDKEY_LARGE
	DHKeyExHdr._wType = KEYEX_TYPE_DH_GX;
	DHKeyExHdr._cbEnt = k_dwDHKeySize+1;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_LARGE\n");
	}
	
	// SG_INVALID_2NDKEY_SMALL
	DHKeyExHdr._cbEnt = k_dwDHKeySize-1;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));	
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_SMALL\n");
	}
	
	// SG_INVALID_2NDKEY_ZERO
	DHKeyExHdr._cbEnt = 0;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit), (BYTE*)&DHKeyExHdr, sizeof(DHKeyExHdr));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}

//--------------------------
// BEGIN 3RD KEY TESTS
//--------------------------

	// SG_INVALID_3RDKEY_TYPE
	SetKeysToValidState(&XBtoSGInit, &DHKeyExHdr, &KerbApReq);
	KerbApReq._wType = KEYEX_TYPE_XBTOXB_INIT;	// NULL case
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}
	
	KerbApReq._wType = KEYEX_TYPE_XBTOXB_RESP;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}
	
	KerbApReq._wType = KEYEX_TYPE_XBTOSG_INIT;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}

	KerbApReq._wType = KEYEX_TYPE_SGTOXB_RESP;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}

	KerbApReq._wType = KEYEX_TYPE_DH_GX;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}
	
	KerbApReq._wType = KEYEX_TYPE_HMAC_SHA;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}
	
	KerbApReq._wType = KEYEX_TYPE_KERB_APREP;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}
	
	KerbApReq._wType = KEYEX_TYPE_NULL_APREQ;	// Should fail in secure mode
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}
	
	KerbApReq._wType = KEYEX_TYPE_NULL_APREP;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}

	KerbApReq._wType = 10;		// Not in range
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}

	// SG_INVALID_3RDKEY_LARGE
	KerbApReq._wType = KEYEX_TYPE_KERB_APREQ;
	KerbApReq._cbEnt = sizeof(CKeyExKerbApReq)+1;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}

	// SG_INVALID_3RDKEY_SMALL
	KerbApReq._cbEnt = sizeof(CKeyExKerbApReq)-1;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}
	
	// SG_INVALID_3RDKEY_ZERO
	KerbApReq._cbEnt = 0;
	memcpy(rgbDataBuffer+sizeof(dwSPI)+sizeof(XBtoSGInit)+sizeof(DHKeyExHdr)+k_dwDHKeySize, (BYTE*)&KerbApReq, sizeof(KerbApReq));
	memcpy(rgbFrameBuffer+wSizeOfHeaders, rgbDataBuffer, wDataLen);
	if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
	{
		printf("VLanXmit failed SG_INVALID_2NDKEY_ZERO\n");
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
//
// SetKeysToValidState
//
// Sets the key exchange headers to a defualt successful configuration in the correct order.
//
void SetKeysToValidState(CKeyExHdr* pXBtoSGInit, CKeyExHdr* pDHKeyExHdr, CKeyExHdr* pKerbApReq)
{
	pXBtoSGInit->_wType = KEYEX_TYPE_XBTOSG_INIT;
	pXBtoSGInit->_cbEnt = sizeof(CKeyExXbToSgInit);
	pDHKeyExHdr->_wType = KEYEX_TYPE_DH_GX;
	pDHKeyExHdr->_cbEnt = 96;	// DH key is 96 bytes
	pKerbApReq->_wType = KEYEX_TYPE_KERB_APREQ;
	pKerbApReq->_cbEnt = sizeof(CKeyExKerbApReq);
}


//-----------------------------------------------------------------------------
//
// SGRouting
//
// Runs the test cases that check for well routed packets
//
HRESULT SGRouting(
	// [in/out] socket for communications
	SOCKET* pSocket, 
	// [in/out] Socket Address for which machine to connect to
	SOCKADDR_IN* psXSGAddress,
	// [in] Test Number
	int nTestNum
)
{
	return S_OK;
}


//-----------------------------------------------------------------------------
//
// SGTicketValidity
//
// Runs the test cases that check for valid tickets coming back from the SG and no tickets
// for invalid keys.
//
HRESULT SGTicketValidity(
	// [in/out] socket for communications
	SOCKET* pSocket, 
	// [in/out] Socket Address for which machine to connect to
	SOCKADDR_IN* psXSGAddress,
	// [in] Test Number
	int nTestNum
)
{
	return S_OK;
}


//-----------------------------------------------------------------------------
//
// SGValidity
//
// Runs the test cases that check packet validity and data integrity
//
HRESULT SGValidity(
	// [in/out] socket for communications
	SOCKET* pSocket, 
	// [in/out] Socket Address for which machine to connect to
	SOCKADDR_IN* psXSGAddress,
	// [in] Test Number
	int nTestNum
)
{
	ENET_HEADER sEnetHdr;
	IP_HEADER sIPHdr;
	UDP_HEADER sUDPHdr;
	WORD wSizeOfHeaders = sizeof(sEnetHdr)+sizeof(sIPHdr)+sizeof(sUDPHdr);
	BYTE* rgbFrameBuffer = new BYTE[k_w1522const];
	BYTE* rgbFrameBuffer2 = new BYTE[1618];
	BYTE* rgbDataBuffer = new BYTE[k_w1522const-wSizeOfHeaders];	
	WORD wDataLen = 0;
	void * pvArg = NULL;
	
	// One time call - sets the Virtual MAC address for using VLan functions
	VLanAttach("Client@Nic/0:001122334455", g_rgbSrcMAC, pvArg);

	//WriteBadFrame(rgbFrameBuffer2, "0002B34DA7040030F2715CC508004500031B77B700003D11417C9D380D79C0A856450C020C0203077CC5000000000200180000000000002D3900631E320423FDB0550000000004006400D8F446716D0234DB182CE1300BCC850D11CFB9984EE659FB331788B5E6D3CC2098B5E551FE4A48C2939D238ED6A78655E1C1D28D8535495C3B008B61504C5C2DC721E9E2A470D8DC3426954858DB6212B9E01ADA93EF4C8344E06637AF34870F06007F026E82027730820273A003020105A10302010EA20703050020000000A38201B2618201AE308201AAA003020105A10A1B0878626F782E636F6DA2163014A003020102A10D300B1B0273671B057369746531A382017D30820179A003020117A28201700482016CBECC1D3C9C35D2B37D854A0DF06AEB2917533B34C8E239BD1E7E51A3978C6879C0BE398C7E6CD441918CBD522A08C354F54589DD9CC5DED52E1DD1E62E8EC83533CF946C41D3589CB1705C0A2500D1466AE71FE30489538C9FCC3799E5B558A6EAB083A9D66429935AA62A21661D98C102F727E90D7BDE3B7A23AA129CF87F147230CA12727C68E80ACCF3451952C1CCBD4DB3902958187B6ACB610B30E0F06486F6762841D7C173FCED4D3FBD71DB0117165BC6A08D1D3B281745364EA011471C37AF0012255D33CD0E6C477A7A11D4880DCE3CF58DCFA1DC2136C5B16276FD630F2FA2A16A365F8F02C54C750C959C88D32F6A0AB91CB5A4313EEFFDF73A5F80C4C5EE143949D95D36728720B7B08A19E7D74A4C3A4C57AA87CE6E1929458E2201CA7D6BCD9EFF491B97681FFE874FBAE83E01684985105CF5DCF24B2DBAEC77B517F90CF195E7457FE3B29DA2A631A9F8CBF5F6712A2DFCE79C6B81E2F80D09DE0414A2B14E665636A946A481A73081A4A003020117A2819C048199BA64DAA2A9833C95C57F7F92AB484D30D5CDB43F9D719F96FDD91050A92E984E9A3B736A447FBCAC88BD379F15A9CCC1D7792A4780CE384BE3126F78E939E36AD942F57E6EF431B86342A1FAFC2F093F71147DE5CE4AFC8042596ADAA108F2ABF469FC36EFA79EEFCCF212BAB7F10394844B60EFA418BB3FC1EED221F5FF501AE724AE75D8BA320000CCFFB4DA3FA1B62CB21B6CA9046CAF68", 1618, 1618);
	WriteBadFrame(rgbFrameBuffer2, "0002B34DA70400112233445508004500031B77B700003D11417C9D380D79C0A856450C020C0203077CC5000000000200180000000000002D3900631E320423FDB0550000000004006400D8F446716D0234DB182CE1300BCC850D11CFB9984EE659FB331788B5E6D3CC2098B5E551FE4A48C2939D238ED6A78655E1C1D28D8535495C3B008B61504C5C2DC721E9E2A470D8DC3426954858DB6212B9E01ADA93EF4C8344E06637AF34870F06007F026E82027730820273A003020105A10302010EA20703050020000000A38201B2618201AE308201AAA003020105A10A1B0878626F782E636F6DA2163014A003020102A10D300B1B0273671B057369746531A382017D30820179A003020117A28201700482016CBECC1D3C9C35D2B37D854A0DF06AEB2917533B34C8E239BD1E7E51A3978C6879C0BE398C7E6CD441918CBD522A08C354F54589DD9CC5DED52E1DD1E62E8EC83533CF946C41D3589CB1705C0A2500D1466AE71FE30489538C9FCC3799E5B558A6EAB083A9D66429935AA62A21661D98C102F727E90D7BDE3B7A23AA129CF87F147230CA12727C68E80ACCF3451952C1CCBD4DB3902958187B6ACB610B30E0F06486F6762841D7C173FCED4D3FBD71DB0117165BC6A08D1D3B281745364EA011471C37AF0012255D33CD0E6C477A7A11D4880DCE3CF58DCFA1DC2136C5B16276FD630F2FA2A16A365F8F02C54C750C959C88D32F6A0AB91CB5A4313EEFFDF73A5F80C4C5EE143949D95D36728720B7B08A19E7D74A4C3A4C57AA87CE6E1929458E2201CA7D6BCD9EFF491B97681FFE874FBAE83E01684985105CF5DCF24B2DBAEC77B517F90CF195E7457FE3B29DA2A631A9F8CBF5F6712A2DFCE79C6B81E2F80D09DE0414A2B14E665636A946A481A73081A4A003020117A2819C048199BA64DAA2A9833C95C57F7F92AB484D30D5CDB43F9D719F96FDD91050A92E984E9A3B736A447FBCAC88BD379F15A9CCC1D7792A4780CE384BE3126F78E939E36AD942F57E6EF431B86342A1FAFC2F093F71147DE5CE4AFC8042596ADAA108F2ABF469FC36EFA79EEFCCF212BAB7F10394844B60EFA418BB3FC1EED221F5FF501AE724AE75D8BA320000CCFFB4DA3FA1B62CB21B6CA9046CAF68", 1618, 1618);
	if (!VLanXmit((BYTE *)rgbFrameBuffer2, 809))
	{
		printf("VLanXmit failed SG_MORE_THAN_ONE\n");
	}
		
	// SG_MORE_THAN_ONE
	// SG_BUFFER_OVERFLOW	
		wDataLen = 1480;	// 1472 is MAX UDP buffer size
		FillData(rgbDataBuffer, wDataLen, k_w1522const-wSizeOfHeaders);
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr), &sUDPHdr, sizeof(sUDPHdr));	
		memcpy(rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)+sizeof(sUDPHdr), rgbDataBuffer, wDataLen);
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
		{
			printf("VLanXmit failed SG_MORE_THAN_ONE\n");
		}

	// SG_BUFFER_MAX
		wDataLen = 1472;	// 1472 is MAX UDP buffer size
		FillData(rgbDataBuffer, wDataLen, k_w1522const-wSizeOfHeaders);
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr), &sUDPHdr, sizeof(sUDPHdr));	
		memcpy(rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)+sizeof(sUDPHdr), rgbDataBuffer, wDataLen);	
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
		{
			printf("VLanXmit failed SG_BUFFER_MAX\n");
		}
		
	// SG_EMPTY
		wDataLen = 0;
		FillData(rgbDataBuffer, wDataLen, k_w1522const-wSizeOfHeaders);
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr), &sUDPHdr, sizeof(sUDPHdr));
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);		
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
		{
			printf("VLanXmit failed on SG_EMPTY\n");
		}

	// SG_SMALL_FRAME
		wDataLen = 12;
		FillData(rgbDataBuffer, wDataLen, k_w1522const-wSizeOfHeaders);
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr)-1, &sIPHdr, sizeof(sIPHdr), &sUDPHdr, sizeof(sUDPHdr));
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)-1+sizeof(sIPHdr)+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);		
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen-1))
		{
			printf("VLanXmit failed on SG_SMALL_FRAME\n");
		}

	// SG_LARGE_FRAME
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr)+1, &sIPHdr, sizeof(sIPHdr), &sUDPHdr, sizeof(sUDPHdr));
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)+1+sizeof(sIPHdr)+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);		
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen+1))
		{
			printf("VLanXmit failed on SG_LARGE_FRAME\n");
		}

	// SG_IP_HEADER_TOO_SMALL
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr)-1, &sUDPHdr, sizeof(sUDPHdr));
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)-1+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);		
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen-1))
		{
			printf("VLanXmit failed on SG_IP_HEADER_TOO_SMALL\n");
		}

	// SG_IP_HEADER_TOO_LARGE
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr)+1, &sUDPHdr, sizeof(sUDPHdr));
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)+1+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen+1))
		{
			printf("VLanXmit failed on SG_IP_HEADER_TOO_LARGE\n");
		}

	// SG_IP_SIZE_GT_IP_PACKET_SIZE  *ADDME
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		sIPHdr.wLength++;
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr)+1, &sUDPHdr, sizeof(sUDPHdr));
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)+1+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
		{
			printf("VLanXmit failed on SG_IP_SIZE_GT_IP_PACKET_SIZE\n");
		}

	// SG_IP_SIZE_GT_FRAME_SIZE
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		sIPHdr.wLength += sizeof(sEnetHdr)+1;
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr)+1, &sUDPHdr, sizeof(sUDPHdr));
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)+1+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen+1))
		{
			printf("VLanXmit failed on SG_IP_SIZE_GT_FRAME_SIZE\n");
		}

	// SG_IGNORE_IP_HEADER_OPTIONS
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		sIPHdr.bVersion = 0x46;	// This translates into version 4, header length 24 (6 32-bit words = 6*4 bytes = 24)
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr)+1, &sUDPHdr, sizeof(sUDPHdr));
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)+1+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen+1))
		{
			printf("VLanXmit failed on SG_IGNORE_IP_HEADER_OPTIONS\n");
		}

	// SG_IGNORE_SMALL_IP_FRAME *ADDME
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		sIPHdr.bVersion = 0x44;	// This translates into version 4, header length 16 (4 32-bit words = 4*4 bytes = 16)
		CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr)+1, &sUDPHdr, sizeof(sUDPHdr));
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)+1+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);
		if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen+1))
		{
			printf("VLanXmit failed on SG_IGNORE_SMALL_IP_FRAME\n");
		}		

	// SG_IGNORE_BROADCAST - sets the Ethernet destination MAC to all FF -> Broadcast
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		for(int i = 0; i < sizeof(sEnetHdr.rgbDestMAC); i++)
		{
			sEnetHdr.rgbDestMAC[i] = 0xFF;
		}
		for(WORD wFrameType = 0x0; wFrameType < 0xFF00; wFrameType++)
		{
			if(wFrameType != 0x0806)	// Only 0x0806 is ARP, everything else should be ignored
			{
				sEnetHdr.wEnetType = htons(wFrameType);
				CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr), &sUDPHdr, sizeof(sUDPHdr));
				if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
				{
					printf("VLanXmit failed on SG_IGNORE_BROADCAST - %d", wFrameType);
				}
			}
		}	

	// SG_IP_VERSION_NOT_4
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);
		for(BYTE bVer = 0x0; bVer < 7; bVer++)
		{
			if(bVer != 4)	// Only 0x0800 is IP, everything else should be ignored
			{
				sIPHdr.bVersion = bVer*16;		// The x16 shifts the bits 4 places to the left - see the IP frame header for details
											// Only the 4 high order bits determine the version
				CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr), &sUDPHdr, sizeof(sUDPHdr));
				if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
				{
					printf("VLanXmit failed on SG_IP_VERSION_NOT_4 - %d", bVer);
				}
			}
		}		

	// SG_BAD_FRAME
	// SG_IGNORE_UNICAST
		SetNetworkHeaders(&sEnetHdr, &sIPHdr, &sUDPHdr, wDataLen);
		memcpy((BYTE *)(&rgbFrameBuffer+sizeof(sEnetHdr)+sizeof(sIPHdr)+sizeof(sUDPHdr)), (BYTE *)&rgbDataBuffer, wDataLen);
		for(wFrameType = 0x0; wFrameType < 0xFF00; wFrameType++)
		{
			if(wFrameType != 0x0800)	// Only 0x0800 is IP, everything else should be ignored
			{
				sEnetHdr.wEnetType = htons(wFrameType);
				CopyHeaders(rgbFrameBuffer, &sEnetHdr, sizeof(sEnetHdr), &sIPHdr, sizeof(sIPHdr), &sUDPHdr, sizeof(sUDPHdr));
				if (!VLanXmit((BYTE *)rgbFrameBuffer, wSizeOfHeaders+wDataLen))
				{
					printf("VLanXmit failed on SG_BAD_FRAME - %d", wFrameType);
				}
			}
		}

	delete [] rgbFrameBuffer;
	delete [] rgbFrameBuffer2;
	delete [] rgbDataBuffer;
		
	return S_OK;
}


//-----------------------------------------------------------------------------
//
// CopyHeaders
//
//  This copies all of the headers into the prgbFrameBuffer in order with the given sizes for each
//  header.
//
void CopyHeaders(BYTE* prgbFrameBuffer, ENET_HEADER* pEnetHdr, UINT uiEnetSize, IP_HEADER* pIPHdr, UINT uiIPSize, UDP_HEADER* pUDPHdr, UINT uiUDPSize)
{
	memcpy(prgbFrameBuffer, (BYTE *)pEnetHdr, uiEnetSize);
	memcpy((prgbFrameBuffer+uiEnetSize), pIPHdr, uiIPSize);
	memcpy((prgbFrameBuffer+uiEnetSize+uiIPSize), pUDPHdr, uiUDPSize);
}

//-----------------------------------------------------------------------------
//
// SetNetworkHeaders
//
// This sets all of the Ethernet, IP, and UDP headers to a preset constant before fiddling with
// their contents before a test.
//
void SetNetworkHeaders(ENET_HEADER* pEnetHdr, IP_HEADER* pIPHdr, UDP_HEADER* pUDPHdr, WORD wDataLen)
{
	// Set Ethernet header
	memcpy(pEnetHdr->rgbDestMAC, g_rgbDestMAC, sizeof(g_rgbDestMAC));
	memcpy(pEnetHdr->rgbSrcMAC, g_rgbSrcMAC, sizeof(g_rgbSrcMAC));
	pEnetHdr->wEnetType = htons(0x0800);

	// Set IP header
	pIPHdr->bVersion = 0x45;
	pIPHdr->bTypeOfService = 0x00;
	// WARNING: Could overflow
	pIPHdr->wLength = htons(sizeof(IP_HEADER) + sizeof(UDP_HEADER) + wDataLen);
	pIPHdr->wIdentification = htons(0xBEEF);
	pIPHdr->wFlagsAndOffset = 0;
	pIPHdr->bTTL = 2;
	pIPHdr->bProtocol = 0x11;	// UDP
	pIPHdr->wChecksum = 0;
	pIPHdr->dwSrcAddress = htonl(g_dwSrcAddress);
	pIPHdr->dwDestAddress = htonl(g_dwDestAddress);
	pIPHdr->wChecksum = GetChecksum((WORD *)pIPHdr, sizeof(IP_HEADER)/2);

	// Set UDP header
	pUDPHdr->wSrcPort = htons(g_usPort);
	pUDPHdr->wDestPort = htons(g_usPort);
	pUDPHdr->wChecksum = 0;	// The SG ignores UDP checksums
	pUDPHdr->wLength = htons(sizeof(UDP_HEADER) + wDataLen);
}

//-----------------------------------------------------------------------------
//
// VLanRecv
//
// This is overridden just to get it to compile for using the VLan functions
//
void VLanRecv(unsigned char *,unsigned int,void *){}

void Usage()
//-----------------------------------------------------------------------------
//
// Usage
//
// Simply outputs the command line useage for this program.
//
{
	printf("XSGTest Help\n\n" );
	printf("XSGTest is a stand alone exe that runs a set of test cases against the the\n" );
	printf("security gateway specified in the ini file.\n" );
	printf("Usage: XSGTest ini_file\n" );
}


int __cdecl main(int argc, char * argv[])
//-----------------------------------------------------------------------------
//
// WinMain
//
// Initilizes global caches and pointers, waits until the test is done, cleans memory and quits.
//
{
	// Return code
	int iReturn = 0;
	HRESULT hr = S_OK;
	// sXSGAddress is the local address associated with a socket
	SOCKADDR_IN          sXSGAddress = { AF_INET, 0, 0, {'\0'} };
	WCHAR* pwszCmdLine = GetCommandLine();
	WCHAR wszTokens[] = L"-/";
	WCHAR* pwszToken = FindOneOf(pwszCmdLine, wszTokens);
	WCHAR wszProgramPath[MAX_PATH +1];
	WCHAR wszIniFile[MAX_PATH+1];
	WCHAR wszFile[MAX_PATH+1];
	WCHAR* pwch;

	// WSAData is the details of the Winsock implementation
	WSADATA              WSAData;
	// Integer for storing error codes
	int iLastError = 0;

	// Initialize Winsock
	iLastError = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (ERROR_SUCCESS != iLastError) 
	{
		goto Cleanup;
	}

	// Check the Winsock version
	if( LOBYTE(WSAData.wVersion) != 2 ||
        LOBYTE(WSAData.wVersion) != 2 )
	{
		hr = E_FAIL;
		goto Cleanup;
	}

	// Set up some global memory
	InitGlobalArray();

	// Open a file for logging errors
	if( (g_pOutput = fopen( "xsgtest.log", "w+" )) == NULL )
	{
		printf("Cannot open 'xsgtest.log' for output\n");
		goto Cleanup;
	}

	// Get wszFile to contain the first argument
	pwch = wcschr(pwszCmdLine, L' ');
	if(pwch != 0)
	{
		pwch++;
		wsprintf(wszFile, L"%s", pwch);
	}	
	else
	{
		Usage();
		return iReturn;
	}
	
	// Save executable path
	GetModuleFileName(NULL, wszProgramPath, MAX_PATH);
	// Strip the Spammer.exe
	pwch = wcsrchr(wszProgramPath, L'\\');
	if(pwch != 0)
	{
		pwch++;
		*pwch = L'\0';
	}

	// Put the ini file name argument on the end of the program path and store in wszIniFile
	wsprintf(wszIniFile, L"%s%s", wszProgramPath, wszFile);

	// Read INI file
	if(FAILED(ReadIni(wszIniFile)))
	{
		return iReturn;
	}

	// Initialize the VLan stuff
	if (!VLanInit())
	{
		printf("VLanInit failed\n");
		return(1);
	}
	
	// Set up network communications
	if(FAILED(InitializeNetworkUDP(&g_sUDP1, &sXSGAddress)))
	{
		return WSAGetLastError();
	}

	// TCP socket for Monitoring Server communications
	if(FAILED(InitializeNetworkTCP(&g_sTCP1, &sXSGAddress, 4002)))
	{
		return WSAGetLastError();
	}

	// Get the local machine address
//	GetLocalMachineAddress();

	RunTestCases(&sXSGAddress);

	printf("\nTotal succeeded test cases: %d\n", g_uiPassedCases);
	printf("Total failed test cases: %d\n", g_uiFailedCases);

Cleanup:

	VLanTerm();

	// Terminate Winsock
	if (0 != WSACleanup()) 
	{
		// Get the last error code
		iLastError = WSAGetLastError();
	}
	
	if( fclose( g_pOutput ) )
		printf( "The file 'data' was not closed\n" );

	return iReturn;
}
