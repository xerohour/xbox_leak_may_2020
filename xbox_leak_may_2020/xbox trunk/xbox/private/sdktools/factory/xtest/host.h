// Header file for XMTA Host communication routines via TCP/IP

#if !defined(SENDHOSTH_INCLUDED)
#define SENDHOSTH_INCLUDED

#define HOST_COMMUNICATION_INITIALIZATION_TIMEOUT 200000.0 // In seconds. Set this really large (like 1000000.0) to effectively make this infinite

// This is the name of the machine that is acting as the host.  This will probably change to
// an IP address in a real test environment.
#define TEST_HOST_ADDRESS "142.142.1.1"
//
// ERROR MESSAGE DEFINITIONS
//
#define EVERYTHINGS_OK                           0x00
#define COULD_NOT_ALLOCATE_SEND_BUFFER           0x10 // Communication send errors
#define COULD_NOT_SEND_DATA_TO_PORT              0x11
#define BUFFER_TO_SEND_IS_BAD                    0x12
#define SEND_TIMEOUT                             0x13
#define COULD_NOT_ALLOCATE_RESPONSE_LIST_ENTRY   0x14
#define COULD_NOT_RECEIVE_DATA_FROM_PORT         0x20 // Communication reception errors
#define CONNECTION_CLOSED                        0x21
#define RECEIVE_BUFFER_OVERRUN                   0x22
#define COULD_NOT_ALLOCATE_RECEIVE_BUFFER        0x23
#define RECEIVE_TIMEOUT                          0x24
#define COULD_NOT_CREATE_RESPONSE_EVENT          0x25
#define SOCKET_VERSION_NOT_SUPPORTED             0x30 // Communication initialization errors
#define COULD_NOT_GET_SOCKET_HANDLE              0x31
#define COULD_NOT_GET_HOST_IP_ADDRESS            0x32
#define COULD_NOT_CONNECT_TO_SOCKET              0x33
#define COULD_NOT_RECEIVE_CONNECT_HANDSHAKE      0x34
#define COULD_NOT_SEND_CONNECT_HANDSHAKE         0x35
#define COULD_NOT_START_RECEIVE_THREAD           0x36
#define COULD_NOT_LOAD_SOCKET_LIBRARY            0x37
#define COULD_NOT_CREATE_SOCKET_EVENT            0x38
#define COULD_NOT_RESET_SOCKET_EVENT             0x39
#define HOST_ERROR_RETURNED                      0x3A
#define ABORT_RECEIVED                           0x3B
#define COULD_NOT_CREATE_ACK_EVENT               0x3C
#define ACK_TIMEOUT                              0x3D

#ifndef STAND_ALONE_MODE
#ifndef _XBOX

#pragma comment (lib, "ws2_32.lib") // Make sure the Winsock2 library is included
#include <winsock2.h>
#else
#include <winsockx.h>
// loading libraries this way doesn't seem to work in the XBOX game project environment right now
//#pragma comment (lib, "xnetd.lib") // Make sure the Winsock2 library is included
#endif
#endif

#include <stdlib.h>
#include <process.h>

#ifdef _XBOX
extern "C"
{
DWORD
NTAPI
XQueryValue(
    IN ULONG ulValueIndex,
    OUT PULONG pulType,
    OUT PVOID pValue,
    IN ULONG cbValueLength,
    IN OUT PULONG pcbResultLength
    );
}
#endif

typedef struct _HOST_RESPONSE_ENTRY {
	struct _HOST_RESPONSE_ENTRY *pNextEntry;  // in:  Next entry in the linked list of pending responses
	char *pcBufferFromHost;        // out: Pointer to the buffer that contains response data from the host
	DWORD dwID;                    // in:  Unique identifier for the list entry
	DWORD dwActualBytesReceived;   // out: The actual number of bytes being returned in the buffer
	DWORD dwReturnedErrorCode;     // out: The error code returned by the host
	int iStatus;                   // out: Status returned from the routine that receives the host response data
	HANDLE hEvent;                 // in:  Event to signal when the response is completely copied
} HOST_RESPONSE_ENTRY, *PHOST_RESPONSE_ENTRY;

class CHostResponse
{
	public:
	CHostResponse ()
	{
		pvBufferFromHost = NULL;
	};
	virtual ~CHostResponse ()
	{
		if (pvBufferFromHost != NULL)
		{
			delete [] pvBufferFromHost;
			pvBufferFromHost = NULL;
		}
	};

	// Function definitions

	// Variable definitions
	void *pvBufferFromHost;
};


// CConfigParam : A class to contain all local values newed during configuration parameter routines
class CHost
{
	public:
	CHost ()
	{
		m_uhcHwnd = NULL;
		pF = stdout;
		hWnd = NULL;
		pHostResponsesPendingList = NULL;
		gdwLocalID = 1;
		g_dwLocalIDToAck = 0;
		InitializeCriticalSection (&m_section);
		bSocketRestarting = false;
		m_strHostName = NULL;
		m_strSerialNumber = NULL;
#ifdef _XBOX
		Init ();
#endif
	}


	virtual ~CHost ()
	{
#ifndef STAND_ALONE_MODE
		DisconnectFromHost ();
#endif
		DeleteCriticalSection (&m_section);
		CloseIniFile();
		if ((pF != NULL) && (pF != stdout)) fclose(pF);
	};

#ifndef STAND_ALONE_MODE
	// Function definitions
	void Init (char *strHostName = NULL, LPCTSTR strSerialNumber = NULL);
	int iSendToHost(SOCKET sock, char *pcAppDataToSend, int iAppDataToSendLength);
	int iReceive(SOCKET sock, char *pcReceiveBuffer, DWORD dwSizeOfReceiveBuffer, DWORD *pdwActualSize, double local_timeout_value, LARGE_INTEGER *pliLocalTimeout);
	int iInitHostCommunication(char *strHostName, LPCTSTR strSerialNumber, double dblTimeout, SOCKET *psock);
	void DisconnectFromHost();
#endif

	int iSendHost(DWORD dwMID, char *pcBufferToHost, DWORD dwBufferToHostLength, CHostResponse& CHR, DWORD *pdwBytesReceivedFromHost, DWORD *pdwErrorCodeFromHost, DWORD dwTimeout);
	int iSendHost (DWORD dwMID, void *pcBufferToHost, DWORD dwBufferToHostLength)
	{
		CHostResponse CHRTemp;
		return iSendHost (dwMID, (char *)pcBufferToHost, dwBufferToHostLength, CHRTemp, NULL, NULL, 600); // Wait for up to 10 minutes on any send
	}
	void vDeleteHostResponseListEntry(DWORD dwID);
	static void vReceiveFromHostThreadEntry(void *pthis);
	void vReceiveFromHostThread(void);
	static void vAlivePrintThreadEntry(void *pthis);
	void vAlivePrintThread(void);
	void MicroSecondDelay(DWORD dwDelayTime);
	void StartTimer(LARGE_INTEGER *StartCount);
	int TimedOut(double dblDelayTime, LARGE_INTEGER *StartCount);
	void SeedRand(int iSeed);
	DWORD GetHostDword (DWORD mid, DWORD alternate);

	// Variable definitions
#ifndef STAND_ALONE_MODE
	SOCKET sock;  // This needs to be global because more than one thread uses it
#endif
	PHOST_RESPONSE_ENTRY pHostResponsesPendingList; // This is the starting point of a linked list of pending responses from the host computer
	CRITICAL_SECTION HostResponseEntryListCriticalSection, SendToHostCriticalSection, SocketAccessCriticalSection;
	DWORD gdwLocalID;
	HANDLE hAckReceivedEvent;
	DWORD g_dwLocalIDToAck;
	unsigned long ulHost;
	bool bSocketRestarting;
	char *m_strHostName;
	LPTSTR m_strSerialNumber;

	CRITICAL_SECTION m_section;

	LPTSTR OpenIniFile (LPCTSTR filename);
	void CloseIniFile ();
	LPTSTR m_start;
	FILE *pF;
	HINSTANCE hWnd;

	HWND m_uhcHwnd;
	void SetUhcHwnd (LPCTSTR str);
	HWND GetUhcHwnd () {return m_uhcHwnd;}
};

#ifdef _XBOX
//
// gethostbyname and gethostbyaddr are not publicly exposed.
// They're declared as private here for us by internal system libraries.
// Notice that their names are obscured.
//

#ifndef _WINSOCKP_H
#define _WINSOCKP_H

#define gethostbyname _wspapi001
#define gethostbyaddr _wspapi002

#ifdef __cplusplus
extern "C" {
#endif

#if INCL_WINSOCK_API_PROTOTYPES
WINSOCK_API_LINKAGE
struct hostent FAR *
WSAAPI
gethostbyaddr(
    IN const char FAR * addr,
    IN int len,
    IN int type
    );
#endif // INCL_WINSOCK_API_PROTOTYPES

#if INCL_WINSOCK_API_TYPEDEFS
typedef
struct hostent FAR *
(WSAAPI * LPFN_GETHOSTBYADDR)(
    IN const char FAR * addr,
    IN int len,
    IN int type
    );
#endif // INCL_WINSOCK_API_TYPEDEFS

#if INCL_WINSOCK_API_PROTOTYPES
WINSOCK_API_LINKAGE
struct hostent FAR *
WSAAPI
gethostbyname(
    IN const char FAR * name
    );
#endif // INCL_WINSOCK_API_PROTOTYPES

#if INCL_WINSOCK_API_TYPEDEFS
typedef
struct hostent FAR *
(WSAAPI * LPFN_GETHOSTBYNAME)(
    IN const char FAR * name
    );
#endif // INCL_WINSOCK_API_TYPEDEFS

#ifdef __cplusplus
}
#endif
#endif // !_WINSOCKP_H

#endif


#endif