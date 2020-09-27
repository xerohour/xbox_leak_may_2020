// File that contains XMTA Host communication routines via TCP/IP

/*

OVERVIEW
--------
All communications between the UUT and the host are initiated by the UUT.  Some messages
from the UUT to the host do not require a response (ex., status messages).  Other messages
from the UUT to the host do require a response (ex. a message that requests that the
host capture an audio buffer from an audio card installed in the host and return the results
to the UUT).  This application demonstrates a simple protocol that allows communication
between the UUT and the host.  This implementation uses TCP/IP through Windows sockets.  A
UUT opens a single socket connection with the host when the test suite first initializes, and
uses the single socket for communication until the test suite completes.  The UUT can not
re-open a socket to the host once it is closed: if a socket closes in the middle of the test
suite (typically only happens if the host crashes, the network crashes, or the UUT crashes)
then the test suite must be restarted with a new socket.  In some situations, a host
may communicate with more than one UUT at a time (like in a run-in systems test envirnment);
in this situation, each socket connection represents a different UUT.

FUNCTION IMPLEMENTATION
-----------------------
For XBox test applications, a single socket is opened by the main() routine before any test
routines are executed.  Communication with the host is accomplished through a single function
call:

int iSendHost(DWORD dwMID, char *pcBufferToHost, DWORD dwBufferToHostLength, CHostResponse& CHR, DWORD *pdwBytesReceivedFromHost, DWORD *pdwErrorCodeFromHost, DWORD dwTimeout)

(in)  DWORD dwMID - the Message ID of the message being sent to the host
(in)  char *pcBufferToHost - pointer to a buffer containing the data/message to send to the host
(in)  DWORD dwBufferToHostLength - the number of bytes in the pcBufferToHost to send to the host
(in/out) CHostResponse& CHR - reference to a CHostResponse structure that contains the pointer to the host response buffer
(out) DWORD *pdwBytesReceivedFromHost - the number of bytes actually returned in the buffer from the host
(out) DWORD *pdwErrorCodeFromHost - the partial error code returned from the host (0 is there was no error)
(in)  DWORD dwTimeout - the maximum number of seconds that the calling routine is willing to wait for a response from the host

returns: EVERYTHINGS_OK if the communication was successful
         some other error if communication was not successful

The iSendHost routine allocates a response buffer for messages that need a response from
the host.  In order to prevent memory leaks, the CHostResponse class is used to manage the
pointer to the response buffer.  A test that calls iSendHost and expects a response simply
needs to create a local instance of CHostResponse and pass it to iSendHost.  When the local
CHostResponse instance passes out of scope, the response buffer associated with it will be
deleted automatically.  If you decide to assign the response buffer pointer to another local
pointer to analyze the response data form the host
(for example, pMyStruct = (PMY_STRUCT)CHR.pvBufferFromHost), be sure to only make the
assignment AFTER the call to iSendHost, and be sure to only access the response buffer
when the CHostResponse instance is in scope.  Declaring the CHostResponse instance at
the top of the test can eliminate scope worries (it won't go out of scope until the test
exits).  The following code is an example of good usage of iSendHost.

An example of how a test can implement a call to iSendHost that requires a response
IMPLEMENT_TESTUNLOCKED (AUDIO, external_analog_loopback, 5)
{
	CHostResponse CHR;
	char *send_buf = NULL;
	DWORD dwMID, dwBufferToHostLength, dwActualBytesReceived, dwErrorCodeFromHost;
	DWORD dwTimeout = 3000; // 5 minute timeout
	int iError;

	.
	.
	.
	// Set up the send buffer (send_buf) here as well as dwMID and dwBufferToHostLength

	
	if ((iError = g_host.iSendHost(dwMID, send_buf, dwBufferToHostLength, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, dwTimeout)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
		}
		else
		{
			// Process a communication error
		}
	}

	else if (CHR.pvBufferFromHost == NULL) // Don't access the response buffer if it is NULL
	{
		// The response buffer is NULL, so take appropriate action (like an error message)
	}
	else // The response is not NULL, and is not an error message, so process it
	{
		// Analyze the response data from the host
	}

	.
	.
	.
}

For messages where no response is needed, such as general Status messages or keep-alive
messages, etc.  The following version of iSendHost can be used instead:

int iSendHost(DWORD dwMID, char *pcBufferToHost, DWORD dwBufferToHostLength)

The parameters are the same as those in the long version of the iSendHost function.

Other functions can be written to wrap this function and make it easier to use for specific
message types.  For example, a routine could wrap up a Status message type so that tests
can send status messages using a much simpler function interface.

TCP/IP DATA PACKET FORMAT
-------------------------
The actual format that the socket communication routines use for data transmissions is rather
simple.  For UUT-to-host messages, the format is as follows:

Offset  0:  4 bytes - Total Length of Message
Offset  4:  4 bytes - Unique message number for this socket
Offset  8:  4 bytes - Message ID (MID) that indicates what type of message this is
Offset 12:  4 bytes - DWORD checksum of the above 3 DWORDs; this helps ensure the integrity of the header
Offset 16:  X bytes - Data (the actual message contents)

Total Length of Message is the length of the entire message, including itself.
This item is needed to inform the host exactly how big the message is.  Because TCP/IP is a
stream-based protocol instead of a message-based protocol, this length parameter is vital to
inform the host how big each message is (which then allows the host to determine where each message
starts and stops).

Unique message number for this socket is an identifier that is created by the sending
routines on the UUT that is unique for that message on that socket connection.  This
identifier is passed back from the host in the response message so that the UUT knows
which message to the host pairs up with which response from the host.

Message ID is the unique ID assigned to this message type.  Each message type whithin a
test architecture has a unique message ID, so that the host can determine what DLL,
service, or utility on the host can process the message.  Currently, message IDs are registered
in the MID.H file in the UHC 2000 project in SourceSafe. 

Data is the actual message that is sent to the host.  The Data blob will itself have a format that
tells the DLL, service, or utility on the host that processes the message what to do.
The format of the data blob, though, is of no significance to the communication routines
on the UUT and host; only the supporting routines and utilities that actually process
the contents of the data blob need to know its structure.

For Host-to-UUT messages, the format is as follows:

Offset  0:  4 bytes - Total Length of Message
Offset  4:  4 bytes - Unique message number for this socket
Offset  8:  4 bytes - Returned Error Code from Host
Offset 12:  X bytes - Data (the actual message contents)

Total Length of Message is the length of the entire message, including itself.
This item is needed to inform the UUT exactly how big the message is.  Because TCP/IP is a
stream-based protocol instead of a message-based protocol, this length parameter is vital to
inform the UUT how big each message is (which then allows the UUT to determine where each message
starts and stops).

Unique message number for this socket is an identifier that is created by the sending
routines on the UUT that is unique for that message on that socket connection.  This
identifier is passed back from the host in the response message so that the UUT knows
which message to the host pairs up with which response from the host.

Returned Error Code from Host is a partial error code (only values 0, and 0xF00 through 0xFFF
are allowed to be returned) that indicates what error happened on the host.  If this value
is zero, then no error happened and the Data buffer contains valid returned data.  If this
value is non-zero, then this value can be combined with the module number and test number
by the test that receives this response to form a complete error code; the data buffer
contains a unicode error message that describes the nature of the error (which can be
displayed in an error reporting routine by the calling test, if desired).  Please note
that the returned error codes are not unique across a test architecture.  For example, many
host utilities may use the error code 0xF30 to represent different errors, but the error
code is unique within a single utility.

Data is the actual response message that is sent to the UUT.  The Data blob will itself
have a format that provides the UUT with useful response information.  The format of the
data blob, though, is of no significance to the communication routines on the UUT and
host; only the supporting routines that actually process the contents of the data blob
need to know its structure.

*/

#include "stdafx.h"
#include "host.h"

// NTSTATUS
typedef LONG NTSTATUS;
/*lint -save -e624 */  // Don't complain about different typedefs.
typedef NTSTATUS *PNTSTATUS;
/*lint -restore */  // Resume checking for different typedefs.
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) 
#define NTHALAPI     DECLSPEC_IMPORT

// SMBus functions.
extern "C"
{
NTHALAPI
NTSTATUS
NTAPI
HalReadSMBusValue(
	IN UCHAR SlaveAddress,
	IN UCHAR CommandCode,
	IN BOOLEAN ReadWordValue,
	OUT ULONG *DataValue
	);

NTHALAPI
NTSTATUS 
NTAPI
HalWriteSMBusValue(
	IN UCHAR SlaveAddress,
	IN UCHAR CommandCode,
	IN BOOLEAN WriteWordValue,
	IN ULONG DataValue
	);
}
#if !defined( STAND_ALONE_MODE )
int CHost::iSendToHost(SOCKET sock, char *pcAppDataToSend, int iAppDataToSendLength)
{
	int iNumLeft = 0, iSendIndex = 0, iNumSent;

	iNumLeft = iAppDataToSendLength;
	iSendIndex = 0;
	while (iNumLeft > 0)
	{
		iNumSent = send(sock, &pcAppDataToSend[iSendIndex], iNumLeft, 0);

		if (iNumSent == SOCKET_ERROR)
		{
//			printf("Could not send data to the port\n");
			return (COULD_NOT_SEND_DATA_TO_PORT);
		}
		iNumLeft = iNumLeft - iNumSent;
		iSendIndex = iSendIndex + iNumSent;
	}

	return (EVERYTHINGS_OK);
}

// This routine spin-waits for a reception of data.  If the data is not received by the specified time, then
// the routine will return with a TIMEOUT failure.  Because this spin-waits, it should only be used
// for initialization, not for regular communication.
int CHost::iReceive(SOCKET sock, char *pcReceiveBuffer, DWORD dwSizeOfReceiveBuffer, DWORD *pdwActualSize, double local_timeout_value, LARGE_INTEGER *pliLocalTimeout)
{
	DWORD dwNumSentToHere = 4;
	int iNumLeft, iReceiveIndex;
	DWORD dwNumReceivedThisTime, dwFlags;
	int iStatus = EVERYTHINGS_OK;
	WSABUF DataBuf;
	WSAOVERLAPPED wsaRecvOverlapped;
	int iReceiveResult;
	int iError, iWTemp = 0;
	WSAEVENT WSAEvent = WSA_INVALID_EVENT;

	iNumLeft = sizeof(DWORD);
	iReceiveIndex = 0;
	if ((WSAEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
		return (COULD_NOT_CREATE_SOCKET_EVENT);
	while ((iNumLeft > 0) && (!TimedOut(local_timeout_value, pliLocalTimeout))) // Receive the DWORD message length indicator
	{
		DataBuf.len = iNumLeft;
		DataBuf.buf = ((char *)&dwNumSentToHere) + iReceiveIndex;
		dwFlags = 0;
		wsaRecvOverlapped.hEvent = WSAEvent;
		if (!WSAResetEvent(WSAEvent))
		{
			WSACloseEvent(WSAEvent);
			return (COULD_NOT_RESET_SOCKET_EVENT);
		}
		iReceiveResult = WSARecv(sock, &DataBuf, 1, &dwNumReceivedThisTime, &dwFlags, &wsaRecvOverlapped, NULL);
//		iNumReceivedThisTime = recv(sock, ((char *)&dwNumSentToHere) + iReceiveIndex, iNumLeft, 0);
		if (iReceiveResult == 0)
		{
			if (dwNumReceivedThisTime == 0)
			{
//				printf("Connection closed by client\n");
				WSACloseEvent(WSAEvent);
				return (CONNECTION_CLOSED);
			}
		}
		else if (iReceiveResult == SOCKET_ERROR)
		{
			if ((iWTemp = WSAGetLastError()) == WSA_IO_PENDING) // The reception is pending, so wait for it
			{
				while ((!WSAGetOverlappedResult(sock, &wsaRecvOverlapped, &dwNumReceivedThisTime, FALSE, &dwFlags)) && (!TimedOut(local_timeout_value, pliLocalTimeout)))
				{
					iError = WSAGetLastError();
					if ((iError != WSA_IO_INCOMPLETE) && (iError != WSA_IO_PENDING))// The reception is complete but there was an error
					{
/*
						switch (iError)
						{
							case WSANOTINITIALISED:
								printf("The socket wan't initialized\n");
								break;
							case WSAENETDOWN:
								printf("The network is down!\n");
								break;
							case WSAENOTSOCK:
								printf("This is not a valid socket\n");
								break;
							case WSA_INVALID_HANDLE:
								printf("The event handle is bad\n");
								break;
							case WSA_INVALID_PARAMETER:
								printf("One or more of the parameters is bad\n");
								break;
							case WSA_IO_INCOMPLETE:
								printf("This isn't a real failure, the transfer just isn't complete yet\n");
								break;
							case WSAEFAULT:
								printf("A parameter fault has happened\n");
								break;
							case WSA_IO_PENDING:
								printf("I/O operation is in progress\n");
								break;
							default:
								printf("Unknown error, %8.8lx\n", (DWORD)iError);
						}
*/
//						printf("Could not receive data from the client inside overlapped result routine size\n");
						WSACloseEvent(WSAEvent);
						return (COULD_NOT_RECEIVE_DATA_FROM_PORT);
					}
				}
			}
			else
			{
//				printf("Could not receive data from the client\n");
				WSACloseEvent(WSAEvent);
				return (COULD_NOT_RECEIVE_DATA_FROM_PORT);
			}
		}
		else // Some other bad error happened, so abort
		{
//			printf("Could not receive data from the client\n");
			WSACloseEvent(WSAEvent);
			return (COULD_NOT_RECEIVE_DATA_FROM_PORT);
		}
		iNumLeft = iNumLeft - (int)dwNumReceivedThisTime;
		iReceiveIndex = iReceiveIndex + dwNumReceivedThisTime;
	}
	// Don't need an extra timeout check here because it will happen below the next while() loop
	if (dwNumSentToHere - 4 > dwSizeOfReceiveBuffer)
	{
		dwNumSentToHere = dwSizeOfReceiveBuffer + 4; // Do this so we don't overflow the receive buffer
//		printf("Receive buffer overrun\n");
		iStatus = RECEIVE_BUFFER_OVERRUN; // Don't return yet; actually try to receive the data
	}

	
	
	// Now receive the actual message
	iNumLeft = (int)(dwNumSentToHere-4);
	*pdwActualSize = 0; // Indicate that 0 bytes have been received so far
	while ((iNumLeft > 0) && (!TimedOut(local_timeout_value, pliLocalTimeout))) // Receive the DWORD message length indicator
	{
		DataBuf.len = iNumLeft;
		DataBuf.buf = &pcReceiveBuffer[*pdwActualSize];
		dwFlags = 0;
		wsaRecvOverlapped.hEvent = WSAEvent;
		if (!WSAResetEvent(WSAEvent))
		{
			WSACloseEvent(WSAEvent);
			return (COULD_NOT_RESET_SOCKET_EVENT);
		}
		iReceiveResult = WSARecv(sock, &DataBuf, 1, &dwNumReceivedThisTime, &dwFlags, &wsaRecvOverlapped, NULL);
//		iNumReceivedThisTime = recv(sock, &pcReceiveBuffer[*pdwActualSize], iNumLeft, 0);
		if (iReceiveResult == 0)
		{
			if (dwNumReceivedThisTime == 0)
			{
//				printf("Connection closed by client\n");
				WSACloseEvent(WSAEvent);
				return (CONNECTION_CLOSED);
			}
		}
		else if (iReceiveResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING) // The reception is pending, so wait for it
			{
				while ((!WSAGetOverlappedResult(sock, &wsaRecvOverlapped, &dwNumReceivedThisTime, FALSE, &dwFlags)) && (!TimedOut(local_timeout_value, pliLocalTimeout)))
				{
					iError = WSAGetLastError();
					if ((iError != WSA_IO_INCOMPLETE) && (iError != WSA_IO_PENDING))// The reception is complete but there was an error
					{
//						printf("Could not receive data from the client inside overlapped result routine data\n");
						WSACloseEvent(WSAEvent);
						return (COULD_NOT_RECEIVE_DATA_FROM_PORT);
					}
				}
			}
			else
			{
//				printf("Could not receive data from the client\n");
				WSACloseEvent(WSAEvent);
				return (COULD_NOT_RECEIVE_DATA_FROM_PORT);
			}
		}
		else // Some other bad error happened, so abort
		{
//			printf("Could not receive data from the client\n");
			WSACloseEvent(WSAEvent);
			return (COULD_NOT_RECEIVE_DATA_FROM_PORT);
		}

		iNumLeft = iNumLeft - (int)dwNumReceivedThisTime;
		*pdwActualSize = *pdwActualSize + dwNumReceivedThisTime;
	}
	if (TimedOut(local_timeout_value, pliLocalTimeout))
	{
//		printf("Timed out waiting to receive data from the host\n");
		WSACloseEvent(WSAEvent);
		return (RECEIVE_TIMEOUT);
	}

	WSACloseEvent(WSAEvent);
	return (iStatus);
}



// Try to establish a connection with the host for up to dblTimeout seconds.  The
// returned socket is to be used for all future communication with the host.
int CHost::iInitHostCommunication(char *strHostName, LPCTSTR strSerialNumber, double dblTimeout, SOCKET *psock)
{
	UNREFERENCED_PARAMETER(dblTimeout);
	#define SOCKET_VERSION_TO_USE                0x0202
#ifdef RNR_MODE
	#define FIRST_SOCKET_PORT_TO_TRY             2001
#else
	#define FIRST_SOCKET_PORT_TO_TRY             2000
#endif
	#define LOCAL_TIMEOUT_VALUE                  50.0 // This is the max number of seconds the send/receive handshake should take
	#define MAX_RANDOM_RETRY_DELAY               3 // This is the max number of seconds to delay between socket connection retries
	#define DEFAULT_HOST_ADDRESS                 0x01018e8e 

	WSADATA wsaData;
//	HOSTENT *phent = NULL;
	struct sockaddr_in tcpaddr;
	int port = FIRST_SOCKET_PORT_TO_TRY;
	DWORD dwReceivedBufferSize;
	const DWORD MSG_HEADER_SIZE = 16;
	DWORD dwConnectSendBuffer[30], dwConnectReceiveBuffer[3];
//	LARGE_INTEGER liGlobalTimeout;
	LARGE_INTEGER liLocalTimeout;
//	int iTimedOut;
	int iStatus = EVERYTHINGS_OK;
	DWORD dwSleepTime;
	unsigned long ulReceiveFromHostThread;
	int iRT1 = 0, iS1 = 0;
//	char cHostName[100];
//	unsigned long ulMyAddr;
	DWORD dwMaxRetryDelay = MAX_RANDOM_RETRY_DELAY;
#if defined(_XBOX)
UNREFERENCED_PARAMETER(strHostName);

	// DWORD dwProtocolBufferSize;
	// LPWSAPROTOCOL_INFO pWSAProtocolInfoBuffer = NULL;
	// int iNumProtocolBuffers = 0;
	
	int XNetStartupError;

	XNetStartupParams xnsp;
	XNADDR xnaddr;

	memset(&xnsp, 0, sizeof(xnsp));
	xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
	xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
	if ((XNetStartupError = XNetStartup(&xnsp)) != NO_ERROR)
	{
		return (COULD_NOT_LOAD_SOCKET_LIBRARY);
	}
	memset(&xnaddr, 0, sizeof(xnaddr));
	/*xnaddr.bSizeOfStruct = sizeof(XNADDR);*/	// Volt - exists no more
	// Wait for the IP address to be assigned
	while ((XNetGetTitleXnAddr(&xnaddr) & (XNET_GET_XNADDR_DHCP | XNET_GET_XNADDR_GATEWAY)) == 0)
		Sleep(50);
/*
	int iSocketInit;
	if ((iSocketInit = XnetInitialize(NULL, TRUE)) != NO_ERROR)
	{
		return (COULD_NOT_LOAD_SOCKET_LIBRARY);
	}
*/
#endif

 	if (WSAStartup(SOCKET_VERSION_TO_USE, &wsaData) != 0)
	{
		// Could not find a usable WinSock DLL.
//		printf("Could not use at least socket version %x\n", SOCKET_VERSION_TO_USE);
		return (SOCKET_VERSION_NOT_SUPPORTED);
	}
 
	// Confirm that the WinSock DLL supports 2.2.  Note that if the DLL supports
	// versions greater than 2.2 in addition to 2.2, it will still return
	// 2.2 in wVersion since that is the version we requested
 	if ( wsaData.wVersion != SOCKET_VERSION_TO_USE)
	{
		// Tell the user that we could not find a usable WinSock DLL.
//		printf("Could not use at least socket version %x\n", SOCKET_VERSION_TO_USE);
		return (SOCKET_VERSION_NOT_SUPPORTED);
	}
#if defined(_XBOX)
	// The routine below will find the server address and restrict it to one of the following values:
	// unsigned long ulHostAddresses[] = {0x01018e8e, 0x01108e8e, 0x01208e8e, 0x01308e8e, 0x01408e8e, 0x01508e8e};
	DWORD dwMyAddress = (DWORD)xnaddr.ina.S_un.S_addr;
	if ((dwMyAddress & 0x0000FFFF) != 0x00008e8e) // Make sure it is 142.142.x.x
		ulHost = DEFAULT_HOST_ADDRESS;
	else
	{
		if ((dwMyAddress & 0x00F00000) > 0x00500000) // 142.142.80 through 95.x is the highest address allowed
			ulHost = DEFAULT_HOST_ADDRESS;
		else
		{
			ulHost = (dwMyAddress & 0x00F0FFFF) | 0x01000000; // Form the address of the host
			if ((dwMyAddress & 0x00F00000) == 0x00000000) // 142.142.1 through 15.x has a host address of 142.142.1.0 (not 142.142.0.0)
				ulHost = ulHost | 0x00010000;
		}
	}
#else
	// Lets try looking up the host IP address using the host name
	//
	HOSTENT *phent = gethostbyname(strHostName);
	if (phent)
	{
		ulHost = *(unsigned long *)(phent->h_addr_list[0]);
	}
#endif 
	// The WinSock DLL is acceptable. Proceed.

	SeedRand(0);  // Seed the random number function here because we may need it to generate a random delay below

	// Now we know what the host IP address is, and we are connected
	// Try to handshake with the host
	for (;;)
	{
		for (;;) // Connect a socket again
		{
			if ((*psock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) != INVALID_SOCKET)
			{
				tcpaddr.sin_family = AF_INET;
				tcpaddr.sin_port = htons((unsigned short)port);
				tcpaddr.sin_addr.s_addr = ulHost;
				// Connect to the destination port.  This could timeout for around 50 seconds if the host
				// is not on the network.  This is a long time, but not too problematic because the UUT won't
				// have much to do if it can't communicate with the host anyway.  If the host is on the network
				// but not listening, then this will timeout in just a couple of seconds.
				if (connect(*psock, (SOCKADDR *)&tcpaddr, sizeof(tcpaddr)) == 0)
				{
					break; // We connected, so we know that this is the host
				}
				else
				{
					iStatus = COULD_NOT_CONNECT_TO_SOCKET;
				}
				closesocket(*psock);
			}
			else
			{
				iStatus =  COULD_NOT_GET_SOCKET_HANDLE;
			}
		}

		// Build the connection request message
		// 
		if (strSerialNumber)
		{
			DWORD length = (_tcslen (strSerialNumber) + 1) * sizeof (_TCHAR);

			dwConnectSendBuffer[0] = MSG_HEADER_SIZE + length;
			dwConnectSendBuffer[1] = (DWORD)rand();
			dwConnectSendBuffer[2] = MID_CONNECT_BY_UNIQUE_ID;

			memcpy (&dwConnectSendBuffer[4], strSerialNumber, length);
		}
		else
		{
#ifdef RNR_MODE
			BYTE SerialNumber[13];
			ULONG Size, Type, Error;
			TCHAR tSerialNumber[13];

			Error = XQueryValue(0x100, &Type, SerialNumber, sizeof SerialNumber, &Size);
			if(Error != NO_ERROR)  // Set the serial number to all 9s
			{
				memset(SerialNumber, '9', sizeof SerialNumber);
			}
			SerialNumber[12] = 0;
			if (sizeof(TCHAR) == sizeof(wchar_t))
				_stprintf(tSerialNumber, _T("%S"), SerialNumber);
			else
				_stprintf(tSerialNumber, _T("%s"), SerialNumber);

			dwConnectSendBuffer[0] = MSG_HEADER_SIZE + ((_tcslen(tSerialNumber) + 1) * sizeof(TCHAR));
			dwConnectSendBuffer[1] = (DWORD)rand();
			dwConnectSendBuffer[2] = MID_CONNECT_BY_UNIQUE_ID;

			memcpy (&dwConnectSendBuffer[4], tSerialNumber, ((_tcslen(tSerialNumber) + 1) * sizeof(TCHAR)));
#else
			dwConnectSendBuffer[0] = MSG_HEADER_SIZE;
			dwConnectSendBuffer[1] = (DWORD)rand();
			dwConnectSendBuffer[2] = MID_CONNECT_BY_MAC_ADDRESS;
#endif
		}
		dwConnectSendBuffer[3] = dwConnectSendBuffer[0] + dwConnectSendBuffer[1] + dwConnectSendBuffer[2];

		iStatus = EVERYTHINGS_OK;
		StartTimer(&liLocalTimeout); // Get the starting time for the loop
		if ((iS1 = iSendToHost(*psock, (char *)dwConnectSendBuffer, dwConnectSendBuffer[0])) == EVERYTHINGS_OK)
		{
			dwConnectReceiveBuffer[0] = dwConnectReceiveBuffer[1] = 0xFF; // Just set the buffer to non-zero values so we know if it is completely filled by the response
			if ((iRT1 = iReceive(*psock, (char *)dwConnectReceiveBuffer, sizeof(dwConnectReceiveBuffer), &dwReceivedBufferSize, LOCAL_TIMEOUT_VALUE, &liLocalTimeout)) == EVERYTHINGS_OK)
			{
				if (dwConnectReceiveBuffer[0] == dwConnectSendBuffer[1]) // Received the ACK
				{
					dwConnectReceiveBuffer[0] = dwConnectReceiveBuffer[1] = 0xFF; // Just set the buffer to non-zero values so we know if it is completely filled by the response
					if ((iRT1 = iReceive(*psock, (char *)dwConnectReceiveBuffer, sizeof(dwConnectReceiveBuffer), &dwReceivedBufferSize, LOCAL_TIMEOUT_VALUE, &liLocalTimeout)) == EVERYTHINGS_OK)
					{
						if ((dwConnectReceiveBuffer[0] == dwConnectSendBuffer[1]) && (dwConnectReceiveBuffer[1] == 0)) // Received proper response
							break;
					}
					else
					{
						iStatus = COULD_NOT_RECEIVE_CONNECT_HANDSHAKE;
					}
				}
				else
				{
					iStatus = COULD_NOT_RECEIVE_CONNECT_HANDSHAKE;
				}
			}
			else
			{
				iStatus = COULD_NOT_RECEIVE_CONNECT_HANDSHAKE;
			}
		}
		else
		{
			iStatus = COULD_NOT_SEND_CONNECT_HANDSHAKE;
		}
		// We did not get the correct handshake, so tear down the socket and start over
		closesocket(*psock);
		dwSleepTime = (DWORD)((((double)((unsigned int)rand()))/(double)RAND_MAX) * (double)(dwMaxRetryDelay-1) * (double)1000);
		dwSleepTime = dwSleepTime + 1000;
		Sleep(dwSleepTime); // Sleep a random amount of time
		if (dwMaxRetryDelay < 20)
			dwMaxRetryDelay += 2;
	}

	// We must have had a successful handshake.

	if (iStatus == EVERYTHINGS_OK)
	{
		// Do whatever global initialization required by vReceiveFromHostThread here
		InitializeCriticalSection(&HostResponseEntryListCriticalSection); // Initialize the critical section before it is used
		InitializeCriticalSection(&SendToHostCriticalSection);
		InitializeCriticalSection(&SocketAccessCriticalSection);
		// Create a manual-reset event.  Non-signaled to start with
		if ((hAckReceivedEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
			iStatus =  COULD_NOT_CREATE_ACK_EVENT;
		else
		{
			pHostResponsesPendingList = NULL;
			gdwLocalID = 1;
			g_dwLocalIDToAck = 0; // Zero is a reserved value indicating that no ACK is expected
			// Start the thread that receives results from the host
			ulReceiveFromHostThread = _beginthread(&vReceiveFromHostThreadEntry, 0, this);  // Start the thread; no parameters need to be passed here
			if (ulReceiveFromHostThread == -1) // Couldn't start the thread
				iStatus = COULD_NOT_START_RECEIVE_THREAD;
/*
// Starts a thread that blinks the LED green or orange once every second
			unsigned long ulAlivePrintThread = _beginthread(&vAlivePrintThreadEntry, 0, this);  // Start the thread; no parameters need to be passed here
			if (ulAlivePrintThread == -1) // Couldn't start the thread
				iStatus = COULD_NOT_START_RECEIVE_THREAD;
*/
		}
	}
	return (iStatus);
}


// This routine will perform communication with the host computer.  Any function can use this routine to
// pass a command to the host computer and receive a response.
int CHost::iSendHost(DWORD dwMID, char *pcBufferToHost, DWORD dwBufferToHostLength, CHostResponse& CHR, DWORD *pdwBytesReceivedFromHost, DWORD *pdwErrorCodeFromHost, DWORD dwTimeout)
{
	int iStatus = EVERYTHINGS_OK;
	DWORD dwNumToSend = 0, dwNumLeft = 0, dwSendIndex = 0, dwNumSent;
	char *pcSendBuffer = NULL;
	double dblTimeout = (double)dwTimeout;
	LARGE_INTEGER liTimeout;
	PHOST_RESPONSE_ENTRY pListTemp = NULL;
	DWORD dwLocalID;
	bool bExpectResponse;
	CTestObj *test = GET_TESTOBJ_PTR;


	while(bSocketRestarting)
		Sleep(1000);
	// Set up returned variables to a "no response" status initially
	if (CHR.pvBufferFromHost != NULL) // Make sure the response buffer is deleted if it had contained something
	{
		delete [] CHR.pvBufferFromHost;
		CHR.pvBufferFromHost = NULL;
	}
	if (pdwBytesReceivedFromHost != NULL)
		*pdwBytesReceivedFromHost = 0;
	if (pdwErrorCodeFromHost != NULL)
		*pdwErrorCodeFromHost = 0;

	// Determine whether a response is expected/possible to return to the caller or not.
	if ((pdwBytesReceivedFromHost == NULL) || (pdwErrorCodeFromHost == NULL) || ((dwMID & 0x40000000) == 0))
		bExpectResponse = false;
	else
		bExpectResponse = true;

	StartTimer(&liTimeout); // Start the timeout timer right away

	// This should be the only code that ever accesses gdwLocalID so that we can guarantee its uniqueness across multiple
	// threads.  Using HostResponseEntryListCriticalSection is cheating; this really should have its own critical section,
	// but it seems silly to create another global critical section just for this.
	// Copy gdwLocalID to a stack-based variable so that future accesses to the value from the stack-based variable
	// will return the correct value, even if other threads have incremented the global variable in the mean time
	EnterCriticalSection(&HostResponseEntryListCriticalSection); // Prevent other threads from accessing the list
	gdwLocalID ++; // Increment the message ID to make sure it is a new value
	dwLocalID = gdwLocalID;
	LeaveCriticalSection(&HostResponseEntryListCriticalSection);

	// Create the full message to send (total length, ID, message ID, data)
	if ((pcSendBuffer = new char[dwBufferToHostLength + 16]) == NULL) // Allocate a buffer to send the data
	{
//		printf("Send: Could not allocate memory for a send buffer\n");
		return (COULD_NOT_ALLOCATE_SEND_BUFFER);
	}
	dwNumToSend = dwBufferToHostLength + 16; // Add space to contain message size, ID, MID, and header checksum
	if (dwBufferToHostLength > 0)
		memcpy(pcSendBuffer + 16, pcBufferToHost, dwBufferToHostLength); // Copy the data into the send buffer
//	dwMID = dwMID | MID_NO_ACK_EXPECTED; // Disabling QQQACKs
	*((DWORD *)(&pcSendBuffer[0])) = dwNumToSend; // The length field includes the length of everything that is sent
	*((DWORD *)(&pcSendBuffer[4])) = dwLocalID; // The ID for this specific message to the host
	*((DWORD *)(&pcSendBuffer[8])) = dwMID; // The message ID of this message to the host
	*((DWORD *)(&pcSendBuffer[12])) = dwNumToSend + dwLocalID + dwMID; // Checksum of the header

	if (bExpectResponse) // There should be a response from the host, so add an entry into the HostResponsesPendingList
	{
		// Allocate a new list entry here
		if ((pListTemp = new HOST_RESPONSE_ENTRY) == NULL)
		{
			if (pcSendBuffer != NULL)
			{
				delete [] pcSendBuffer;
				pcSendBuffer = NULL;
			}
//			printf("Send: Could not allocate memory for a pending host response list entry\n");
			return (COULD_NOT_ALLOCATE_RESPONSE_LIST_ENTRY);
		}
		pListTemp->pcBufferFromHost = NULL;
		pListTemp->dwID = dwLocalID;
		pListTemp->dwActualBytesReceived = 0;
		pListTemp->iStatus = EVERYTHINGS_OK;
		pListTemp->dwReturnedErrorCode = 0;
		// Create an event and report an error if creation fails
		// Create an event for signaling when the response from the host has been captured
		// This handle cannot be inherited by a child process; Auto-reset the state to non-signaled Initial state is non-signaled
		if ((pListTemp->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		{
			if (pcSendBuffer != NULL)
			{
				delete [] pcSendBuffer;
				pcSendBuffer = NULL;
			}
			if (pListTemp != NULL)
			{
				delete pListTemp;
				pListTemp = NULL;
			}
//			printf("Send: Could not create an event for signalling response reception from the host\n");
			return (COULD_NOT_CREATE_RESPONSE_EVENT);
		}
		
		EnterCriticalSection(&HostResponseEntryListCriticalSection); // Prevent other threads from accessing the list

		// Add the new list entry to the front of the list
		pListTemp->pNextEntry = pHostResponsesPendingList;
		pHostResponsesPendingList = pListTemp;

		LeaveCriticalSection(&HostResponseEntryListCriticalSection);
	}

	// Now that the response entry has been made, if applicable, send the message to the host
	dwNumLeft = dwNumToSend;
	dwSendIndex = 0;
	EnterCriticalSection(&SendToHostCriticalSection); // Make sure we send the whole message to the host without being interrupted by other threads' sends.
	g_dwLocalIDToAck = dwLocalID; // This is the Local ID to wait for an ACK
	ResetEvent(hAckReceivedEvent);
	while (dwNumLeft > 0) // Send the data
	{
		// Only allow a timeout if we haven't sent any data successfully yet.
		// Once we have started sending the message, we cannot stop until complete or
		// the communication with the host will go out of sync because TCP is a stream
		// protocol, not a message protocol.

//		do not check the timeout before sending
//		if (dwNumLeft == dwNumToSend)
//		{
//			if (TimedOut(dblTimeout, &liTimeout))
//				break;
			// Probably don't want to check for timeouts here because if a test aborts, we want
			// to be able to send any abort or error messages associated with the abort to the host
//			if (test != NULL)
//			{
//				if (test->CheckAbort(HERE))
//					break;
//			}
//		}
		// Since send() is blocking, we may be stuck here much longer than the specified timout
		// but that shouldn't matter too much because if we timeout due to a network problem
		// then we have nothing better to do then wait around because we can't communicate
		// our problem to the world anyway.
		EnterCriticalSection(&SocketAccessCriticalSection); // Make sure we don't use the socket at the same time as the receive thread
		dwNumSent = (DWORD)send(sock, &pcSendBuffer[dwSendIndex], (int)dwNumLeft, 0);
		LeaveCriticalSection(&SocketAccessCriticalSection);
		if (dwNumSent == SOCKET_ERROR)
		{
			int iE = WSAGetLastError();
			if (pcSendBuffer != NULL)
			{
				delete [] pcSendBuffer;
				pcSendBuffer = NULL;
			}
			LeaveCriticalSection(&SendToHostCriticalSection);
			vDeleteHostResponseListEntry(dwLocalID);
			g_dwLocalIDToAck = 0;
			if ((iE != WSAEINPROGRESS) && (iE != WSAENOBUFS))
			{
				SOCKET lsock = INVALID_SOCKET;
				struct sockaddr_in tcpaddr;
				int port = FIRST_SOCKET_PORT_TO_TRY;
				DWORD dwConnectSendBuffer[16], dwConnectReceiveBuffer[3];
				LARGE_INTEGER liLocalTimeout;
				int iRT1 = 0, iS1 = 0;
				DWORD MSG_HEADER_SIZE = 16;
				DWORD dwReceivedBufferSize;
				DWORD dwSleepTime;
				DWORD dwLocalMaxRetryDelay = MAX_RANDOM_RETRY_DELAY;

				bSocketRestarting = true;  // Block all other threads from accessing this routine
				Sleep(2000);  // Wait awhile for any threads to exit that might be in here
				// The socket is gone.  Reconnect and send an error at least
				shutdown (sock, SD_BOTH);
				closesocket(sock);
				sock = INVALID_SOCKET;

				// This is a bad socket error, so open a new socket to the host and complain
				for(;;) // Try to talk to the host again forever
				{
					WSADATA wsaData;

					dwSleepTime = (DWORD)((((double)((unsigned int)rand()))/(double)RAND_MAX) * (double)(dwLocalMaxRetryDelay-1) * (double)1000);
					dwSleepTime = dwSleepTime + 1000;
					Sleep(dwSleepTime); // Sleep a random amount of time
					if (dwLocalMaxRetryDelay < 10)
						dwLocalMaxRetryDelay += 2;
					if (lsock != INVALID_SOCKET)
						closesocket(lsock);
					lsock = INVALID_SOCKET;
					WSACleanup();
#ifdef _XBOX
					int XNetStartupError;
					XNetStartupParams xnsp;
					XNADDR xnaddr;
					DWORD dwWaitForIP;

					XNetCleanup();
					memset(&xnsp, 0, sizeof(xnsp));
					xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
					xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
					while ((XNetStartupError = XNetStartup(&xnsp)) != NO_ERROR)
					{
						Sleep(3000);
					}
					memset(&xnaddr, 0, sizeof(xnaddr));
					/*xnaddr.bSizeOfStruct = sizeof(XNADDR); */	// Volt - exists no more
					// Wait for the IP address to be assigned
					dwWaitForIP = 80;
					while ((XNetGetTitleXnAddr(&xnaddr) & (XNET_GET_XNADDR_DHCP | XNET_GET_XNADDR_GATEWAY)) == 0)
					{
						dwWaitForIP--;
						if (dwWaitForIP == 0)
							break;
						Sleep(50);
					}
					if (dwWaitForIP == 0) // No address was issued
						continue;
#endif
					if (WSAStartup(SOCKET_VERSION_TO_USE, &wsaData) != 0)
					{
						// Could not find a usable WinSock DLL.
						continue;
					}
#ifdef _XBOX
					// The routine below will find the server address and restrict it to one of the following values:
					// unsigned long ulHostAddresses[] = {0x01018e8e, 0x01108e8e, 0x01208e8e, 0x01308e8e, 0x01408e8e, 0x01508e8e};
					DWORD dwMyAddress = (DWORD)xnaddr.ina.S_un.S_addr;
					if ((dwMyAddress & 0x0000FFFF) != 0x00008e8e) // Make sure it is 142.142.x.x
						ulHost = DEFAULT_HOST_ADDRESS;
					else
					{
						if ((dwMyAddress & 0x00F00000) > 0x00500000) // 142.142.80 through 95.x is the highest address allowed
							ulHost = DEFAULT_HOST_ADDRESS;
						else
						{
							ulHost = (dwMyAddress & 0x00F0FFFF) | 0x01000000; // Form the address of the host
							if ((dwMyAddress & 0x00F00000) == 0x00000000) // 142.142.1 through 15.x has a host address of 142.142.1.0 (not 142.142.0.0)
								ulHost = ulHost | 0x00010000;
						}
					}
#else
					// Lets try looking up the host IP address using the host name
					//
					HOSTENT *phent = gethostbyname(m_strHostName);
					if (phent)
					{
						ulHost = *(unsigned long *)(phent->h_addr_list[0]);
					}
#endif
					if ((lsock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) != INVALID_SOCKET)
					{
						tcpaddr.sin_family = AF_INET;
						tcpaddr.sin_port = htons((unsigned short)port);
						tcpaddr.sin_addr.s_addr = ulHost;
						// Connect to the destination port.  This could timeout for around 50 seconds if the host
						// is not on the network.  This is a long time, but not too problematic because the UUT won't
						// have much to do if it can't communicate with the host anyway.  If the host is on the network
						// but not listening, then this will timeout in just a couple of seconds.
						if (connect(lsock, (SOCKADDR *)&tcpaddr, sizeof(tcpaddr)) == 0)
						{
							// Build the connection request message
							// 
#ifdef RNR_MODE
							BYTE SerialNumber[13];
							ULONG Size, Type, Error;
							TCHAR tSerialNumber[13];

							Error = XQueryValue(0x100, &Type, SerialNumber, sizeof SerialNumber, &Size);
							if(Error != NO_ERROR)  // Set the serial number to all 9s
							{
								memset(SerialNumber, '9', sizeof SerialNumber);
							}
							SerialNumber[12] = 0;
							if (sizeof(TCHAR) == sizeof(wchar_t))
								_stprintf(tSerialNumber, _T("%S"), SerialNumber);
							else
								_stprintf(tSerialNumber, _T("%s"), SerialNumber);

							dwConnectSendBuffer[0] = MSG_HEADER_SIZE + ((_tcslen(tSerialNumber) + 1) * sizeof(TCHAR));
							dwConnectSendBuffer[1] = (DWORD)rand();
							dwConnectSendBuffer[2] = MID_CONNECT_BY_UNIQUE_ID;

							memcpy (&dwConnectSendBuffer[4], tSerialNumber, ((_tcslen(tSerialNumber) + 1) * sizeof(TCHAR)));
#else
							dwConnectSendBuffer[0] = MSG_HEADER_SIZE;
							dwConnectSendBuffer[1] = (DWORD)rand();
							dwConnectSendBuffer[2] = MID_CONNECT_BY_MAC_ADDRESS;
#endif
							dwConnectSendBuffer[3] = dwConnectSendBuffer[0] + dwConnectSendBuffer[1] + dwConnectSendBuffer[2];

							iStatus = EVERYTHINGS_OK;
							StartTimer(&liLocalTimeout); // Get the starting time for the loop
							if ((iS1 = iSendToHost(lsock, (char *)dwConnectSendBuffer, dwConnectSendBuffer[0])) == EVERYTHINGS_OK)
							{
								dwConnectReceiveBuffer[0] = dwConnectReceiveBuffer[1] = 0xFF; // Just set the buffer to non-zero values so we know if it is completely filled by the response
								if ((iRT1 = iReceive(lsock, (char *)dwConnectReceiveBuffer, sizeof(dwConnectReceiveBuffer), &dwReceivedBufferSize, LOCAL_TIMEOUT_VALUE, &liLocalTimeout)) == EVERYTHINGS_OK)
								{
									if (dwConnectReceiveBuffer[0] == dwConnectSendBuffer[1]) // Received the ACK
									{
										dwConnectReceiveBuffer[0] = dwConnectReceiveBuffer[1] = 0xFF; // Just set the buffer to non-zero values so we know if it is completely filled by the response
										if ((iRT1 = iReceive(lsock, (char *)dwConnectReceiveBuffer, sizeof(dwConnectReceiveBuffer), &dwReceivedBufferSize, LOCAL_TIMEOUT_VALUE, &liLocalTimeout)) == EVERYTHINGS_OK)
										{
											if ((dwConnectReceiveBuffer[0] == dwConnectSendBuffer[1]) && (dwConnectReceiveBuffer[1] == 0)) // Received proper response
											{
												// We have connected and had a handshake.  Now send an error message
												TCHAR SendPrivateError[100];
												int iCurrent;
												CHostResponse CHRTemp;
												DWORD *pdwSendPrivateError = (DWORD *)SendPrivateError;
												
												pdwSendPrivateError[1] = (DWORD)rand();
												pdwSendPrivateError[2] = MID_UUT_ERROR;

												iCurrent = 16/sizeof(TCHAR);
												_stprintf(&SendPrivateError[iCurrent], _T("%s"), _T("SendHost.Main"));
												iCurrent += _tcslen(&SendPrivateError[iCurrent]) + 1;
												*((DWORD *)&SendPrivateError[iCurrent]) = 0x2E; // Socket dropped error code
												iCurrent += (4/sizeof(TCHAR));
												_stprintf(&SendPrivateError[iCurrent], _T("%s"), _T("The socket connection to the host was dropped"));
												iCurrent += _tcslen(&SendPrivateError[iCurrent]) + 1;

												pdwSendPrivateError[0] = iCurrent*sizeof(TCHAR);
												pdwSendPrivateError[3] = pdwSendPrivateError[0] + pdwSendPrivateError[1] + pdwSendPrivateError[2];

												iStatus = EVERYTHINGS_OK;
												StartTimer(&liLocalTimeout); // Get the starting time for the loop
												if ((iS1 = iSendToHost(lsock, (char *)pdwSendPrivateError, pdwSendPrivateError[0])) == EVERYTHINGS_OK)
												{
													dwConnectReceiveBuffer[0] = dwConnectReceiveBuffer[1] = 0xFF; // Just set the buffer to non-zero values so we know if it is completely filled by the response
													if ((iRT1 = iReceive(lsock, (char *)dwConnectReceiveBuffer, sizeof(dwConnectReceiveBuffer), &dwReceivedBufferSize, LOCAL_TIMEOUT_VALUE, &liLocalTimeout)) == EVERYTHINGS_OK)
													{
														if (dwConnectReceiveBuffer[0] == pdwSendPrivateError[1]) // Received the ACK
														{
															// Error message sent correctly, now send a testing ended message
															pdwSendPrivateError[0] = 16;
															pdwSendPrivateError[1] = (DWORD)rand();
															pdwSendPrivateError[2] = MID_UUT_ENDING;
															pdwSendPrivateError[3] = pdwSendPrivateError[0] + pdwSendPrivateError[1] + pdwSendPrivateError[2];
															if ((iS1 = iSendToHost(lsock, (char *)pdwSendPrivateError, pdwSendPrivateError[0])) == EVERYTHINGS_OK)
															{
																dwConnectReceiveBuffer[0] = dwConnectReceiveBuffer[1] = 0xFF; // Just set the buffer to non-zero values so we know if it is completely filled by the response
																if ((iRT1 = iReceive(lsock, (char *)dwConnectReceiveBuffer, sizeof(dwConnectReceiveBuffer), &dwReceivedBufferSize, LOCAL_TIMEOUT_VALUE, &liLocalTimeout)) == EVERYTHINGS_OK)
																{
																	if (dwConnectReceiveBuffer[0] == pdwSendPrivateError[1]) // Received the ACK
																	{
																		// Testing Ended message sent correctly
																		break;
																	}
																	else
																	{
																		iStatus = COULD_NOT_RECEIVE_CONNECT_HANDSHAKE;
																	}
																}
																else
																{
																	iStatus = COULD_NOT_RECEIVE_CONNECT_HANDSHAKE;
																}
															}
														}
														else
														{
															iStatus = COULD_NOT_RECEIVE_CONNECT_HANDSHAKE;
														}
													}
													else
													{
														iStatus = COULD_NOT_RECEIVE_CONNECT_HANDSHAKE;
													}
												}
											}
										}
										else
										{
											iStatus = COULD_NOT_RECEIVE_CONNECT_HANDSHAKE;
										}
									}
									else
									{
										iStatus = COULD_NOT_RECEIVE_CONNECT_HANDSHAKE;
									}
								}
								else
								{
									iStatus = COULD_NOT_RECEIVE_CONNECT_HANDSHAKE;
								}
							}
							// We did not get the correct handshake, so tear down the socket and start over
						}
						else
						{
							iStatus = COULD_NOT_CONNECT_TO_SOCKET;
						}
					}
				} // end for(;;)
				// We only get here if we successfully sent the error and ended messages
				for (;;){}
			}
			return (COULD_NOT_SEND_DATA_TO_PORT);
		}
		dwNumLeft = dwNumLeft - dwNumSent;
		dwSendIndex = dwSendIndex + dwNumSent;
	}
// QQQAKC
///*
	while (!(TimedOut(dblTimeout, &liTimeout)))
	{
		if (WaitForSingleObject(hAckReceivedEvent, 1000) == WAIT_OBJECT_0)
			break;
		if (test != NULL)
		{
			if (test->CheckAbort(HERE))
				break;
		}
	}
//*/
	if (TimedOut(dblTimeout, &liTimeout))
	{
		g_dwLocalIDToAck = 0;

		vDeleteHostResponseListEntry(dwLocalID);
		LeaveCriticalSection(&SendToHostCriticalSection);
		return (ACK_TIMEOUT);
	}

	LeaveCriticalSection(&SendToHostCriticalSection);
	delete [] pcSendBuffer;
	pcSendBuffer = NULL;
	if (TimedOut(dblTimeout, &liTimeout))
	{
		vDeleteHostResponseListEntry(dwLocalID);
//		printf("Send: Timeout out trying to send data to the host\n");
		return (SEND_TIMEOUT);
	}
	if (test != NULL)
	{
		if (test->CheckAbort(HERE))
		{
			vDeleteHostResponseListEntry(dwLocalID);
			return (ABORT_RECEIVED);
		}
	}

	// Wait for a reply from the host only if applicable
	if (bExpectResponse)
	{
		// Wait for a response or a timeout condition
		// It is possible that this whole routine may take 2x iTimeOut if we wait almost
		// iTimeout seconds above and then iTimeout seconds here too.  That is not too big a deal
		// though and avoids having to do the time math here.
		while (!(TimedOut(dblTimeout, &liTimeout)))
		{
			if (WaitForSingleObject(pListTemp->hEvent, 1000) == WAIT_OBJECT_0)
				break;
			if (test != NULL)
			{
				if (test->CheckAbort(HERE))
					break;
			}
		}
		if (TimedOut(dblTimeout, &liTimeout))
		{
			vDeleteHostResponseListEntry(dwLocalID);
			return (RECEIVE_TIMEOUT);
		}
		if (test != NULL)
		{
			if (test->CheckAbort(HERE))
			{
				vDeleteHostResponseListEntry(dwLocalID);
				return (ABORT_RECEIVED);
			}
		}

		// The data is back, so let's parse it
		// pListTemp should still be a valid pointer to our list entry (even though its position in the list may have moved around)
		// Use the critical section just in case the receive routine gets another response that matches this ID (shouldn't happen)
		// and starts to muck with our data while we are parsing it out
		EnterCriticalSection(&HostResponseEntryListCriticalSection); // Prevent other threads from accessing the list

		CHR.pvBufferFromHost = pListTemp->pcBufferFromHost;
		*pdwBytesReceivedFromHost = pListTemp->dwActualBytesReceived;
		*pdwErrorCodeFromHost = pListTemp->dwReturnedErrorCode;
		iStatus = pListTemp->iStatus;

		LeaveCriticalSection(&HostResponseEntryListCriticalSection);

		vDeleteHostResponseListEntry(dwLocalID); // Close the event and remove this entry from the list
	}

	return (iStatus);
}

// Delete an entry from the Host Response List, if it exists
// WARNING: Do not access this routine from within the HostResponseEntryListCriticalSection.  Deadlock will result
void CHost::vDeleteHostResponseListEntry(DWORD dwID)
{
	PHOST_RESPONSE_ENTRY pListTemp = NULL, pPreviousListEntryTemp = NULL;

	EnterCriticalSection(&HostResponseEntryListCriticalSection); // Prevent other threads from accessing the list
	pPreviousListEntryTemp = pHostResponsesPendingList;
	pListTemp = pHostResponsesPendingList;
	if (pListTemp != NULL) // There is at least one entry in the list
	{
		while (pListTemp != NULL)
		{
			if (pListTemp->dwID == dwID)
			{
				if (pListTemp == pHostResponsesPendingList) // This is the first entry in the list
				{
					pHostResponsesPendingList = pHostResponsesPendingList->pNextEntry;
				}
				else // This is not the first entry in the list
				{
					pPreviousListEntryTemp->pNextEntry = pListTemp->pNextEntry;
				}
				pListTemp->pNextEntry = NULL;
				// This is how to close out an Event (whether or not it was set)
				CloseHandle(pListTemp->hEvent);
				delete pListTemp;
				break;
			}
			pPreviousListEntryTemp = pListTemp;
			pListTemp = pListTemp->pNextEntry;
		}
	}
	LeaveCriticalSection(&HostResponseEntryListCriticalSection);
	return;
}


// The entry point to the new thread that will allow access to the real vReceiveFromHostThread routine
void CHost::vReceiveFromHostThreadEntry(void *pthis)
{
	CHost *pHost = (CHost *)pthis;

	pHost->vReceiveFromHostThread();
}
// This thread receives all communication from the host after initial communication has
// been established.  Whenever a function sends a command to the host, this thread will
// capture and pass back the respose.
void CHost::vReceiveFromHostThread(void)
{
	DWORD dwNumSentToHere = 4;
	int iForever = 1;
	int iNumLeft, iReceiveIndex;
	DWORD dwNumReceivedThisTime;
	char *pcLocalBuffer;
	char *pcCopyBuffer;
	int iStatus;
	PHOST_RESPONSE_ENTRY pListTemp = NULL;
	int iActualBytesReceived; // Used to hold information about the received data
	DWORD dwID;                // Used to hold the message ID of the received data
	DWORD dwReturnedErrorCode;
	WSAEVENT WSAEvent = WSA_INVALID_EVENT;
	WSABUF DataBuf;
	WSAOVERLAPPED wsaRecvOverlapped;
	int iReceiveResult, iWTemp, iError;
	DWORD dwFlags;
	BOOL bResult;
	char pcDefaultBuffer[100]; // This buffer is use to receive useful data if the buffer allocation fails
	char pcScratchBuffer[100]; // This buffer is use to receive scratch data if the buffer allocation fails


	if ((WSAEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
		return;
	while (iForever == 1) // This will be in a forever while() loop	
	{
		pcLocalBuffer = NULL;
		iStatus = EVERYTHINGS_OK;
		iNumLeft = sizeof(DWORD);
		iReceiveIndex = 0;
		while (iNumLeft > 0) // Receive the DWORD message length indicator
		{
			DataBuf.len = iNumLeft;
			DataBuf.buf = ((char *)&dwNumSentToHere) + iReceiveIndex;
			dwFlags = 0;
			wsaRecvOverlapped.hEvent = WSAEvent;
			WSAResetEvent(WSAEvent);

			EnterCriticalSection(&SocketAccessCriticalSection); // Make sure we don't use the socket at the same time as the receive thread
			iReceiveResult = WSARecv(sock, &DataBuf, 1, &dwNumReceivedThisTime, &dwFlags, &wsaRecvOverlapped, NULL);
			LeaveCriticalSection(&SocketAccessCriticalSection);
			if (iReceiveResult == 0)
			{
				if (dwNumReceivedThisTime == 0) // The socket closed, so we can't do anything
				{
					WSACloseEvent(WSAEvent);
					return;
				}
			}
			else if (iReceiveResult == SOCKET_ERROR)
			{
				if ((iWTemp = WSAGetLastError()) == WSA_IO_PENDING) // The reception is pending, so wait for it
				{
					WSAWaitForMultipleEvents(1, &WSAEvent, FALSE, WSA_INFINITE, FALSE); // Wait for the data to be received
					for(;;)
					{
						EnterCriticalSection(&SocketAccessCriticalSection); // Make sure we don't use the socket at the same time as the receive thread
						bResult = WSAGetOverlappedResult(sock, &wsaRecvOverlapped, &dwNumReceivedThisTime, FALSE, &dwFlags);
						LeaveCriticalSection(&SocketAccessCriticalSection);
						if (bResult)
							break;
						iError = WSAGetLastError();
						if ((iError != WSA_IO_INCOMPLETE) && (iError != WSA_IO_PENDING))// The reception is complete but there was an error
						{
							WSACloseEvent(WSAEvent);
							return;
						}
						Sleep(5); // Don't hog all of the CPU time
					}
				}
				else // Could not receive data from the client for some reason, so exit
				{
					WSACloseEvent(WSAEvent);
					return;
				}
			}
			else // Some other bad error happened, so abort
			{
				WSACloseEvent(WSAEvent);
				return;
			}
			iNumLeft = iNumLeft - (int)dwNumReceivedThisTime;
			iReceiveIndex = iReceiveIndex + (int)dwNumReceivedThisTime;
		}

		if ((pcLocalBuffer = new char[dwNumSentToHere - 4]) == NULL) // Allocate a buffer to receive the data
		{
			pcCopyBuffer = pcDefaultBuffer;
			if ((int)(dwNumSentToHere-4) > sizeof(pcDefaultBuffer)) // Ensure we bring in the right amount of data
				iNumLeft = sizeof(pcDefaultBuffer);
			else
				iNumLeft = (int)(dwNumSentToHere-4);
			iStatus = COULD_NOT_ALLOCATE_RECEIVE_BUFFER;
		}
		else
		{
			pcCopyBuffer = pcLocalBuffer;
			iNumLeft = (int)(dwNumSentToHere-4);
		}

		iActualBytesReceived = iNumLeft; // This is the actual number of bytes that will be received
		// Now receive the actual message
		iReceiveIndex = 0; // Indicate that 0 bytes have been received so far
		while (iNumLeft > 0) // Receive the full message
		{
			DataBuf.len = iNumLeft;
			DataBuf.buf = &pcCopyBuffer[iReceiveIndex];
			dwFlags = 0;
			wsaRecvOverlapped.hEvent = WSAEvent;
			WSAResetEvent(WSAEvent);
			EnterCriticalSection(&SocketAccessCriticalSection); // Make sure we don't use the socket at the same time as the receive thread
			iReceiveResult = WSARecv(sock, &DataBuf, 1, &dwNumReceivedThisTime, &dwFlags, &wsaRecvOverlapped, NULL);
			LeaveCriticalSection(&SocketAccessCriticalSection);
			if (iReceiveResult == 0)
			{
				if (dwNumReceivedThisTime == 0) // The socket closed, so we can't do anything
				{
					WSACloseEvent(WSAEvent);
					return;
				}
			}
			else if (iReceiveResult == SOCKET_ERROR)
			{
				if ((iWTemp = WSAGetLastError()) == WSA_IO_PENDING) // The reception is pending, so wait for it
				{
					WSAWaitForMultipleEvents(1, &WSAEvent, FALSE, WSA_INFINITE, FALSE); // Wait for the data to be received
					for(;;)
					{
						EnterCriticalSection(&SocketAccessCriticalSection); // Make sure we don't use the socket at the same time as the receive thread
						bResult = WSAGetOverlappedResult(sock, &wsaRecvOverlapped, &dwNumReceivedThisTime, FALSE, &dwFlags);
						LeaveCriticalSection(&SocketAccessCriticalSection);
						if (bResult)
							break;
						iError = WSAGetLastError();
						if ((iError != WSA_IO_INCOMPLETE) && (iError != WSA_IO_PENDING))// The reception is complete but there was an error
						{
							WSACloseEvent(WSAEvent);
							return;
						}
						Sleep(5); // Don't hog all of the CPU time
					}
				}
				else // Could not receive data from the client for some reason, so exit
				{
					WSACloseEvent(WSAEvent);
					return;
				}
			}
			else // Some other bad error happened, so abort
			{
				WSACloseEvent(WSAEvent);
				return;
			}

			iNumLeft = iNumLeft - (int)dwNumReceivedThisTime;
			iReceiveIndex = iReceiveIndex + (int)dwNumReceivedThisTime;
		}

		// If the buffer allocation failed above, read in the rest of the data and throw it away (we have the important stuff already)
		if ((iStatus == COULD_NOT_ALLOCATE_RECEIVE_BUFFER) && ((int)(dwNumSentToHere-4) > sizeof(pcDefaultBuffer)))
		{
			// There is still some data to read, so let's get to it.
			iNumLeft = (int)(dwNumSentToHere - 4) - sizeof(pcDefaultBuffer);
			while (iNumLeft > 0)
			{
				DataBuf.len = (iNumLeft < sizeof(pcScratchBuffer)) ? iNumLeft : sizeof(pcScratchBuffer);
				DataBuf.buf = &pcScratchBuffer[0];
				dwFlags = 0;
				wsaRecvOverlapped.hEvent = WSAEvent;
				WSAResetEvent(WSAEvent);
				EnterCriticalSection(&SocketAccessCriticalSection); // Make sure we don't use the socket at the same time as the receive thread
				iReceiveResult = WSARecv(sock, &DataBuf, 1, &dwNumReceivedThisTime, &dwFlags, &wsaRecvOverlapped, NULL);
				LeaveCriticalSection(&SocketAccessCriticalSection);
				if (iReceiveResult == 0)
				{
					if (dwNumReceivedThisTime == 0) // The socket closed, so we can't do anything
					{
						WSACloseEvent(WSAEvent);
						return;
					}
				}
				else if (iReceiveResult == SOCKET_ERROR)
				{
					if ((iWTemp = WSAGetLastError()) == WSA_IO_PENDING) // The reception is pending, so wait for it
					{
						WSAWaitForMultipleEvents(1, &WSAEvent, FALSE, WSA_INFINITE, FALSE); // Wait for the data to be received
						for(;;)
						{
							EnterCriticalSection(&SocketAccessCriticalSection); // Make sure we don't use the socket at the same time as the receive thread
							bResult = WSAGetOverlappedResult(sock, &wsaRecvOverlapped, &dwNumReceivedThisTime, FALSE, &dwFlags);
							LeaveCriticalSection(&SocketAccessCriticalSection);
							if (bResult)
								break;
							iError = WSAGetLastError();
							if ((iError != WSA_IO_INCOMPLETE) && (iError != WSA_IO_PENDING))// The reception is complete but there was an error
							{
								WSACloseEvent(WSAEvent);
								return;
							}
							Sleep(5); // Don't hog all of the CPU time
						}
					}
					else // Could not receive data from the client for some reason, so exit
					{
						WSACloseEvent(WSAEvent);
						return;
					}
				}
				else // Some other bad error happened, so abort
				{
					WSACloseEvent(WSAEvent);
					return;
				}

				iNumLeft = iNumLeft - (int)dwNumReceivedThisTime;
			}
		}

		// At this point, all of the data for this command has been received
		// iStatus should indicate EVERYTHINGS_OK, or COULD_NOT_ALLOCATE_RECEIVE_BUFFER
		// From here below is the only code that knows the format of the returned data (ID, Host error code, and data)
		dwID = *(DWORD *)&pcCopyBuffer[0];  // ID value returned from host
		dwReturnedErrorCode = (*(DWORD *)&pcCopyBuffer[4]);  // Error Code value from the host
		iActualBytesReceived = iActualBytesReceived - 12; // Subtract the space used by the error code and the ID to get the number of data bytes

		pListTemp = NULL; // Do this so that the buffer delete below will happen if the response is an ACK or not in the list at all
		if (dwID == g_dwLocalIDToAck) // If this was just a command ACK, process it here
//		if (dwID == dwID+10)  // Always false, just to disable QQQACKs
		{
			g_dwLocalIDToAck = 0;
			SetEvent(hAckReceivedEvent);
		}
		else // This wasn't an ACK, so maybe it was a real response
		{
			EnterCriticalSection(&HostResponseEntryListCriticalSection); // Prevent other threads from accessing the list
			pListTemp = pHostResponsesPendingList;
			while (pListTemp != NULL)
			{
				if (pListTemp->dwID == dwID)
					break;
				pListTemp = pListTemp->pNextEntry;
			}
			if (pListTemp != NULL) // We found a match!
			{
				// Point the destination buffer pointer to the one we allocated
				pListTemp->iStatus = iStatus;  // Relay the status back to the thread that is waiting for this response
				pListTemp->pcBufferFromHost = pcLocalBuffer; // The pointer to the local buffer, or NULL
				if (pcLocalBuffer == NULL)
					pListTemp->dwActualBytesReceived = 0;
				else
				{
					pListTemp->dwActualBytesReceived = (DWORD)iActualBytesReceived;
					memcpy(&pcCopyBuffer[0], &pcCopyBuffer[12], pListTemp->dwActualBytesReceived); // Shift the reponse data to the start of the buffer
					pListTemp->dwReturnedErrorCode = dwReturnedErrorCode;
					if ((pListTemp->iStatus == EVERYTHINGS_OK) && (dwReturnedErrorCode != 0)) // If the host returned an error, but iStatus is still OK, change iStatus to indicate that there is a host error
						pListTemp->iStatus = HOST_ERROR_RETURNED;
				}
				SetEvent(pListTemp->hEvent); // Trigger the event to wake up the thread that is waiting for this response
			}
			LeaveCriticalSection(&HostResponseEntryListCriticalSection); // Relinquish access to the list
		}
		// If there is no match, discard the data and do nothing
		if ((pListTemp == NULL) && (pcLocalBuffer != NULL)) // If a buffer was allocated, but not sent back to a calling thread, delete the allocated buffer
		{
			delete [] pcLocalBuffer;
			pcLocalBuffer = NULL;
		}
	}
	return;
}

void CHost::vAlivePrintThreadEntry(void *pthis)
{
	CHost *pHost = (CHost *)pthis;

	pHost->vAlivePrintThread();
}
void CHost::vAlivePrintThread(void)
{
#ifdef _XBOX
	DWORD dwValueToWrite = 0;

	// Set LED Override to use the requested LED Behavior
	HalWriteSMBusValue(0x20, 0x07, FALSE, 0x01);
	for (;;)
	{
		if (dwValueToWrite == 0)
		{
			// Set LED Off
			// Orange
			HalWriteSMBusValue(0x20, 0x08, FALSE, 0xFF);
			dwValueToWrite = 1;
		}
		else
		{
			// Green
			HalWriteSMBusValue(0x20, 0x08, FALSE, 0x0F);
			dwValueToWrite = 0;
		}
		Sleep(1000);
	}
#endif
}


// StartTimer is really just a reuse of QueryPerformanceCounter
void CHost::StartTimer(LARGE_INTEGER *StartCount)
{
	QueryPerformanceCounter(StartCount);
	return;
}

// Returns TRUE if the specified amount of time has elapsed, FALSE otherwise.  This should only
// be use to perform periodic checks on a timer for a timeout condition
int CHost::TimedOut(double dblDelayTime, LARGE_INTEGER *StartCount)
{
// Assume that a high-perfmance counter is supported (it is on all Pentiums and higher)
	LARGE_INTEGER PerformanceFrequency, CurrentCount;
	double dblFrequency;

	QueryPerformanceFrequency(&PerformanceFrequency);
	dblFrequency = (double)(PerformanceFrequency.QuadPart);
	QueryPerformanceCounter(&CurrentCount);

	if ((((double)(CurrentCount.QuadPart - StartCount->QuadPart))/dblFrequency) < dblDelayTime)
		return (FALSE);
	return (TRUE);
}

// Delay for a specified number of Microseconds.  This should only be used for very small
// delays because it uses lots of CPU time.  Use Sleep() when the delay is in the millisecond
// or longer range and can be a bit longer than specified.
void CHost::MicroSecondDelay(DWORD dwDelayTime)
{
// Assume that a high-perfmance counter is supported (it is on all Pentiums and higher)
	LARGE_INTEGER PerformanceFrequency, StartCount, CurrentCount;
	double dblFrequency;

	QueryPerformanceFrequency(&PerformanceFrequency);
	dblFrequency = (double)(PerformanceFrequency.QuadPart) / 1000000; // Microsecond resolution
	QueryPerformanceCounter(&StartCount);
	CurrentCount.QuadPart = StartCount.QuadPart;

	while ((((double)(CurrentCount.QuadPart - StartCount.QuadPart))/dblFrequency) < (double)dwDelayTime)
		QueryPerformanceCounter(&CurrentCount);
	return;
}

// This function will seed the random number generator rand() with either the value passed in
// or the current time (if the value passed in is 0)
void CHost::SeedRand(int iSeed)
{
	SYSTEMTIME systimeSeed;
	FILETIME filetimeSeed;

	if (iSeed == 0)
	{
		GetSystemTime(&systimeSeed);
		SystemTimeToFileTime(&systimeSeed, &filetimeSeed);
		iSeed = (int)filetimeSeed.dwLowDateTime;
	}
	srand(iSeed);
	return;
}

LPTSTR CHost::OpenIniFile (LPCTSTR filename)
{
	#define INI_HOST_TIMEOUT 15*60 // 15 minutes timeout

	CHostResponse CHR;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;
	int iError;
// The file name may have path information that we don't want.  Clean up XMTA.CPP

	if ((iError = g_host.iSendHost(MID_GET_FILE, (char *)filename, (wcslen(filename) + 1) * sizeof(WCHAR), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, INI_HOST_TIMEOUT)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			g_error.err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
			return(NULL);
		}
		else
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			g_error.err_HostCommunicationError(iError);
			return(NULL);
		}
	}
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		g_error.err_INIBufferIsEmpty();
		return(NULL);
	}
	// The response is not NULL, and is not an error message, so process it

	// We are reading an ASCII file and converting it to UNICODE.
	// The following algorithm is designed to use a single buffer
	// for reading and conversion.  To do this we create a buffer large
	// enough to hold the converted file, then write the original
	// data to the upper half of the file.
	//
	DWORD fileSize = dwActualBytesReceived;
	m_start = new _TCHAR [fileSize + 2];
	BYTE *m_startBYTE = (BYTE *)m_start;
	char *middle = (char *) &m_start [(fileSize + 2)/2];

	// Copy the file contents to the allocated buffer
	memcpy(middle, CHR.pvBufferFromHost, dwActualBytesReceived);  // Copy the ini file contents
	
	for (DWORD q = 0; q < dwActualBytesReceived; q++)
	{
		m_startBYTE[q+q] = middle[q];
		m_startBYTE[q+q+1] = 0;
	}
/*
	int len = MultiByteToWideChar (CP_ACP, 0, middle, dwActualBytesReceived, m_start, fileSize + 1);
	if (len == 0)
	{
		g_error.ReportUnicodeConversionFailed (filename, (LPCTSTR) CGetLastError());
		return NULL;
	}
*/
	m_start [fileSize] = _T('\0');
	LPTSTR current = m_start;

	// Skip over initial unicode designator character
	//
	if (0xfffe == (int) *current)
		return NULL; // This code represents little endian format
	if (0xfeff == (int) *current)
		current++;

	return current;
}

DWORD CHost::GetHostDword (DWORD mid, DWORD alternate)
{
	CHostResponse CHR;

	const DWORD FIVEMINUTES = 30000;
	DWORD result, bytes, status;
	iSendHost (mid, NULL, 0, CHR, &bytes, &status, FIVEMINUTES);
	if ((CHR.pvBufferFromHost == NULL) || (bytes < 4) || (status != 0)) // Don't access the response buffer if it does not at least contain a DWORD
		result = alternate;
	else
		result = *((DWORD *)(CHR.pvBufferFromHost));
	return result;
}

void CHost::Init (char *strHostName, LPCTSTR strSerialNumber)
{
	// Only in remote mode
	sock = INVALID_SOCKET;
	if (iInitHostCommunication(strHostName, strSerialNumber,
		HOST_COMMUNICATION_INITIALIZATION_TIMEOUT, &sock) != EVERYTHINGS_OK)
	{
		DisconnectFromHost ();
	}
}

void CHost::DisconnectFromHost()
{
	if ((sock != NULL) && (sock != INVALID_SOCKET))
	{
		shutdown (sock, SD_BOTH);
		closesocket(sock);
		WSACleanup();
		sock = INVALID_SOCKET;
	}			
}

#endif // STAND_ALONE_MODE