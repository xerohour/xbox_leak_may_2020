//==================================================================================
// Includes
//==================================================================================
#define INCL_WINSOCK_API_TYPEDEFS 1 // includes winsock2 fn proto's, for getprocaddress
#include <windows.h>
#ifndef _XBOX
#include <winsock2.h>
#else
#include <winsockx.h>		// Needed for XNet initialization
#include <stdio.h>			// Needed for ANSI/Unicode conversion
#include <stdlib.h>			// Needed for ANSI/Unicode conversion
#endif // XBOX


#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\strutils.h"

#include "tncontrl.h"
#include "main.h"

#include "sendq.h"
#include "comm.h"
#include "commtcp.h"






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CtrlCommTCPLoadOptimal()"
//==================================================================================
// CtrlCommTCPLoadOptimal
//----------------------------------------------------------------------------------
//
// Description: Creates the WinSock TCP/IP optimal WinSock version control method
//				object.
//
// Arguments:
//	PCONTROLCOMMINITPBLOCK pParams		Data coming in to the function.
//	PTNCTRLCOMM* ppCTNCtrlComm			Pointer to pointer to hold object created.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CtrlCommTCPLoadOptimal(PCONTROLCOMMINITPBLOCK pParams,
								PTNCTRLCOMM* ppCTNCtrlComm)
{
	PTNCTRLCOMMTCP		pObj = NULL;


	// Create the object.
	// TRUE = we can use winsock 2 functionality if available
	pObj = new (CTNCtrlCommTCP)(pParams, TRUE);
	if (pObj == NULL)
		return (E_OUTOFMEMORY);

	pObj->m_dwRefCount++; // caller is using it

	// Return our new object
	(*ppCTNCtrlComm) = pObj;

	return (S_OK);
} // CtrlCommTCPLoadOptimal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CtrlCommTCPLoadWinSock1()"
//==================================================================================
// CtrlCommTCPLoadWinSock1
//----------------------------------------------------------------------------------
//
// Description: Creates the WinSock TCP/IP forced non WinSock version 2
//				functionality control method object.
//
// Arguments:
//	PCONTROLCOMMINITPBLOCK pParams		Data coming in to the function.
//	PTNCTRLCOMM* ppTNCtrlComm			Pointer to pointer to hold object created.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CtrlCommTCPLoadWinSock1(PCONTROLCOMMINITPBLOCK pParams,
								PTNCTRLCOMM* ppTNCtrlComm)
{
	PTNCTRLCOMMTCP	pObj = NULL;


	// Create the object
	// FALSE = we won't use winsock 2 functionality ever
	pObj = new (CTNCtrlCommTCP)(pParams, FALSE);
	if (pObj == NULL)
		return (E_OUTOFMEMORY);

	pObj->m_dwRefCount++; // caller is using it

	// Return our new object
	(*ppTNCtrlComm) = pObj;

	return (S_OK);
} // CtrlCommTCPLoadWinSock1
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CtrlCommTCPLoadWinSock2OneToOne()"
//==================================================================================
// CtrlCommTCPLoadWinSock2OneToOne
//----------------------------------------------------------------------------------
//
// Description: Creates the WinSock TCP/IP optimal WinSock version control method
//				object.
//
// Arguments:
//	PCONTROLCOMMINITPBLOCK pParams		Data coming in to the function.
//	PTNCTRLCOMM* ppTNCtrlComm			Pointer to pointer to hold object created.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CtrlCommTCPLoadWinSock2OneToOne(PCONTROLCOMMINITPBLOCK pParams,
										PTNCTRLCOMM* ppTNCtrlComm)
{
	PTNCTRLCOMMTCP	pObj = NULL;


	// Create the object.
	// TRUE = we can use winsock 2 functionality if available
	pObj = new (CTNCtrlCommTCP)(pParams, TRUE);
	if (pObj == NULL)
		return (E_OUTOFMEMORY);

	pObj->m_dwRefCount++; // caller is using it

	// Return our new object
	(*ppTNCtrlComm) = pObj;

	return (S_OK);
} // CtrlCommTCPLoadWinSock2OneToOne
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLinkedSocket::CLinkedSocket()"
//==================================================================================
// CLinkedSocket constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CLinkedSocket, using the passed in parameters.
//
// Arguments:
//	SOCKET sNew				Socket to add.
//	SOCKADDR_IN address		The address of the socket being added.
//	BOOL fTCP				Whether the socket is TCP or not.
//	BOOL fListen			If the socket is TCP, whether its a listen socket or not.
//	BOOL fBroadcast			If the socket is UDP, whether its capable of sending
//							broadcasts or not.
//	HANDLE hWinSock2Event	Handle to event associated with socket, if available.
//
// Returns: None (just the object).
//==================================================================================
CLinkedSocket::CLinkedSocket(SOCKET sNew, SOCKADDR_IN address, BOOL fTCP, BOOL fListen,
							BOOL fBroadcast, HANDLE hWinSock2Event):
	m_socket(sNew),
	m_address(address),
	m_pCommData(NULL),
	m_fTCP(fTCP),
	m_fListen(fListen),
	m_fBroadcast(fBroadcast),
	m_hWS2Event(hWinSock2Event),
	m_dwFoundPriorToReachCheck(0)
{
	DPL(9, "this = %x, sizeof (this) = %i", 2, this, sizeof (CLinkedSocket));

	if ((fTCP) && (fListen))
		this->m_fListen = TRUE;

	if ((! fTCP) && (fBroadcast))
		this->m_fBroadcast = TRUE;
} // CLinkedSocket::CLinkedSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLinkedSocket::~CLinkedSocket()"
//==================================================================================
// CLinkedSocket destructor
//----------------------------------------------------------------------------------
//
// Description: Releases data associated with the CLinkedSocket object.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CLinkedSocket::~CLinkedSocket(void)
{
	HRESULT		hr;


	DPL(9, "this = %x", 1, this);

	hr = this->CloseSocket();
	if (hr != S_OK)
	{
		DPL(0, "Closing the socket failed!  %e", 1, hr);
	} // end if (failed closing the socket)
} // CLinkedSocket::~CLinkedSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLinkedSocket::GetSocket()"
//==================================================================================
// CLinkedSocket::GetSocket
//----------------------------------------------------------------------------------
//
// Description: Retrieves this socket.
//
// Arguments: None.
//
// Returns: Pointer to this socket.
//==================================================================================
SOCKET* CLinkedSocket::GetSocket(void)
{
	return (&(this->m_socket));
} // CLinkedSocket::GetSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLinkedSocket::GetAddress()"
//==================================================================================
// CLinkedSocket::GetAddress
//----------------------------------------------------------------------------------
//
// Description: Retrieves this socket's address.
//
// Arguments: None.
//
// Returns: Pointer to this item's socket address.
//==================================================================================
SOCKADDR_IN* CLinkedSocket::GetAddress(void)
{
	return (&(this->m_address));
} // CLinkedSocket::GetAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLinkedSocket::GetEvent()"
//==================================================================================
// CLinkedSocket::GetEvent
//----------------------------------------------------------------------------------
//
// Description: Retrieves this socket's event.
//
// Arguments: None.
//
// Returns: Handle of this socket's event.
//==================================================================================
HANDLE CLinkedSocket::GetEvent(void)
{
	return (this->m_hWS2Event);
} // CLinkedSocket::GetEvent
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLinkedSocket::IsTCPListen()"
//==================================================================================
// CLinkedSocket::IsTCPListen
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if this is a TCP socket that is listening, FALSE
//				otherwise.
//
// Arguments: None.
//
// Returns: TRUE if TCP and listen, FALSE otherwise.
//==================================================================================
BOOL CLinkedSocket::IsTCPListen(void)
{
	return ((this->m_fTCP) && (this->m_fListen));
} // CLinkedSocket::IsTCPListen
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLinkedSocket::IsUDP()"
//==================================================================================
// CLinkedSocket::IsUDP
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if this is a UDP socket, FALSE otherwise.
//
// Arguments: None.
//
// Returns: TRUE if UDP, FALSE otherwise.
//==================================================================================
BOOL CLinkedSocket::IsUDP(void)
{
	return (!(this->m_fTCP));
} // CLinkedSocket::IsUDP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLinkedSocket::IsUDPBroadcast()"
//==================================================================================
// CLinkedSocket::IsUDPBroadcast
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if this is a UDP socket capable of sending to the
//				broadcast address, FALSE otherwise.
//
// Arguments: None.
//
// Returns: TRUE if UDP, FALSE otherwise.
//==================================================================================
BOOL CLinkedSocket::IsUDPBroadcast(void)
{
	return ((! this->m_fTCP) && (this->m_fBroadcast));
} // CLinkedSocket::IsUDPBroadcast
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLinkedSocket::CloseSocket()"
//==================================================================================
// CLinkedSocket::CloseSocket
//----------------------------------------------------------------------------------
//
// Description: Closes the socket gracefully.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CLinkedSocket::CloseSocket(void)
{
	HRESULT		hr;


	// Silently return if the socket is bad
	if (this->m_socket == INVALID_SOCKET)
		return (S_OK);

	if ((this->m_fTCP) && (! this->m_fListen))
	{
		DPL(3, "Closing connection to %o", 1, &(this->m_address));

		// Disable sends, tell other side we want to shutdown.
		if (shutdown(this->m_socket, SD_SEND) == SOCKET_ERROR)
		{
			hr = WSAGetLastError();
			DPL(0, "Shutting down sends on stream socket failed!", 0);
			return (hr);
		} // end if (shutdown failed)

		/*
		// Read any pending data
		while (1)
		{
			hr = recv(this->m_socket, lpBuffer, iBufferSize, 0);
			if (hr == SOCKET_ERROR)
			{
				DPL(0, "Failed to receive stream data!  %e", 1, WSAGetLastError());
				break;
			} // end if (failed to receive data)
			else if (hr == 0)
			{
				break;
			} // end if (connection was closed)
		} // end while (haven't received all data)
		*/

		// Let other side know we are finished.
		if (shutdown(this->m_socket, SD_BOTH) == SOCKET_ERROR)
		{
			DPL(0, "Shutting down stream socket failed!", 0);
			return (WSAGetLastError());
		} // end if (shutdown failed)
	} // end if (this is a TCP socket)

	if (closesocket(this->m_socket) != 0)
	{
		hr = WSAGetLastError();
		DPL(0, "Failed to close socket!", 0);
		return (hr);
	} // end if (failed to close socket)
	this->m_socket = INVALID_SOCKET;

	if (this->m_pCommData != NULL)
	{
		this->m_pCommData->fDropped = TRUE;
		/*
		if (! SetEvent(this->m_lpCommData->hStatusEvent))
		{
			DPL(0, "Couldn't set status event!", 0);
		} // end if (failed to set status event)
		*/
	} // end if (this socket was bound by the control layer)

	return (S_OK);
} // CLinkedSocket::CloseSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLSocketsList::CLSocketsList()"
//==================================================================================
// CLSocketsList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CLSocketsList object.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CLSocketsList::CLSocketsList(void)
{
	HRESULT		hr;


	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CLSocketsList));

	this->m_hListChangedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (this->m_hListChangedEvent == NULL)
	{
		hr = GetLastError();
		DPL(0, "Couldn't create list changed event!  %e", 1, hr);
	} // end if (couldn't create event)
} // CLSocketsList::CLSocketsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLSocketsList::~CLSocketsList()"
//==================================================================================
// CLSocketsList destructor
//----------------------------------------------------------------------------------
//
// Description: Releases data associated with the CLSocketsList object.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CLSocketsList::~CLSocketsList(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_hListChangedEvent != NULL)
	{
		CloseHandle(this->m_hListChangedEvent);
		this->m_hListChangedEvent = NULL;
	} // end if (have event)
} // CLSocketsList::~CLSocketsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLSocketsList::AddSocket()"
//==================================================================================
// CLSocketsList::AddSocket
//----------------------------------------------------------------------------------
//
// Description: Adds the passed in socket to this list and sets the list changed
//				event if applicable.
//
// Arguments:
//	SOCKET sNew					Socket to add.
//	SOCKADDR_IN address			The address of the socket being added.
//	BOOL fTCP					Whether the socket is TCP or not.
//	BOOL fListen				If the socket is TCP, whether its a listen socket or
//								not.
//	BOOL fBroadcast				If the socket is UDP, whether its capable of sending
//								broadcasts or not.
//	HANDLE hWinSock2Event		Handle to event to associate with socket, or NULL to
//								not associate anything.
//	PLINKEDSOCKET* ppLSocket	Place to store pointer to socket object created, or
//								 NULL if not wanted.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CLSocketsList::AddSocket(SOCKET sNew, SOCKADDR_IN address, BOOL fTCP,
								BOOL fListen, BOOL fBroadcast, HANDLE hWinSock2Event,
								PLINKEDSOCKET* ppLSocket)
{
	HRESULT			hr;
	PLINKEDSOCKET	pNewItem = NULL;


	pNewItem = new (CLinkedSocket)(sNew, address, fTCP, fListen, fBroadcast,
									hWinSock2Event);
	if (pNewItem == NULL)
		return (E_OUTOFMEMORY);

	this->EnterCritSection();

	hr = this->Add(pNewItem);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add linked socket to list!", 0);
	} // end if (couldn't add item to list)
	else if (hWinSock2Event)
	{
		if (! SetEvent(this->m_hListChangedEvent))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set list changed event (%x)!",
				1, this->m_hListChangedEvent);
		} // end if (couldn't set event)
	} // end else if (could add item to list and using WinSock2)

	/*
	DPL(9, "Added %s socket at index %i.",
		2, (fTCP ? "TCP" : "UDP"), (this->Count() - 1));
	*/

	if (ppLSocket != NULL)
		(*ppLSocket) = pNewItem;

	this->LeaveCritSection();

	return (hr);
} // CLSocketsList::AddSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLSocketsList::AddNewConnectionFromSocket()"
//==================================================================================
// CLSocketsList::AddNewConnectionFromSocket
//----------------------------------------------------------------------------------
//
// Description: Accepts the next connection available on the socket passed in, and
//				and adds that connection to this list.  If hWinSock2Event and
//				lpfnWSAEventSelect are not NULL, the event will be associated with
//				the new socket (using that proc).
//
// Arguments:
//	SOCKET* pListenSocket					Socket that is ready to accept a
//											connection.
//	HANDLE hWinSock2Event					Event to associate with socket, if any.
//	LPFN_WSAEVENTSELECT lpfnWSAEventSelect	Pointer to WinSock2 procedure to call.
//	BOOL fSocketOwnsEvent					TRUE if the new socket should own (and
//											thus, know about) the WinSock 2 event.
//	PLINKEDSOCKET* ppLSocket				Place to store pointer to socket object
//											created, or NULL if not wanted.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CLSocketsList::AddNewConnectionFromSocket(SOCKET* pListenSocket,
												HANDLE hWinSock2Event,
												LPFN_WSAEVENTSELECT lpfnWSAEventSelect,
												BOOL fSocketOwnsEvent,
												PLINKEDSOCKET* ppLSocket)
{
	HRESULT			hr;
	SOCKET			sNew;
	SOCKADDR_IN		fromSockAddress;
	int				iFromSockAddressSize = sizeof (SOCKADDR_IN);
	HANDLE			hEvent = NULL;


	sNew = accept((*pListenSocket), (SOCKADDR*) (&fromSockAddress), &iFromSockAddressSize);
	if (sNew == INVALID_SOCKET)
	{
		hr = WSAGetLastError();

		// WinSock2 seems to be setting our events twice, so trying to handle the
		// item again returns WSAEWOULDBLOCK.
		if (hr != WSAEWOULDBLOCK)
		{
			DPL(0, "Stream socket accept failed!", 0);
		} // end if (not special case)
		return (hr);
	} // end if (accept failed)

	if ((hWinSock2Event != NULL) && (lpfnWSAEventSelect != NULL))
	{
		hr = lpfnWSAEventSelect(sNew, hWinSock2Event, FD_CLOSE | FD_READ);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't associate receive event with new socket!", 0);
			return (hr);
		} // end if (WSAEventSelect failed)
	} // end if (we're using WinSock2 functionality)

	hr = this->AddSocket(sNew, fromSockAddress, TRUE, FALSE, FALSE,
						((fSocketOwnsEvent) ? hWinSock2Event : NULL),
						ppLSocket);
	if (hr != S_OK)
	{
		DPL(0, "Failed to add accepted socket!  Closing it.", 0);
		closesocket(sNew); // ignoring error
		return (hr);
	} // end if (failed to add socket)


	DPL(5, "Successfully received connection from %o", 1, &fromSockAddress);

	return (S_OK);
} // CLSocketsList::AddNewConnectionFromSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CLSocketsList::GetLSocketByAddress()"
//==================================================================================
// CLSocketsList::GetLSocketByAddress
//----------------------------------------------------------------------------------
//
// Description: If TCP, this searches the list of sockets for one connected to the
//				specified address and port.  If UDP, then the first UDP socket found
//				using a matching port is returned.  If the address is the broadcast
//				address, only broadcast capable sockets are returned.
//				If pAddress is NULL, then the first socket of the given type is
//				returned.
//
// Arguments:
//	SOCKADDR_IN* pAddress	Address to search for.
//	BOOL fTCP				Whether to look for a TCP socket or not.
//
// Returns: Pointer to the socket found or NULL if doesn't exist.
//==================================================================================
PLINKEDSOCKET CLSocketsList::GetLSocketByAddress(SOCKADDR_IN* pAddress, BOOL fTCP)
{
	BOOL				fLocal = FALSE;
	BOOL				fUDPBroadcast = FALSE;
	int					i;
	PLINKEDSOCKET		pItem = NULL;



	// UDP sockets are connectionless and therefore always only have port
	// information.
	if (! fTCP)
	{
		fLocal = TRUE;
	} // end if (not TCP socket)

	// If there's a socket, note if it specifies the special unknown or broadcast
	// addresses.
	if (pAddress)
	{
		unsigned long*		pulIPaddr;


		pulIPaddr = (unsigned long*) (&pAddress->sin_addr.S_un);

		if ((*pulIPaddr) == INADDR_ANY)
		{
			fLocal = TRUE;
		} // end if (unspecified address)


		if ((*pulIPaddr) == INADDR_BROADCAST)
		{
#ifdef DEBUG
			if (fTCP)
			{
				DPL(0, "Looking for TCP connection to the broadcast address!?  DEBUGBREAK()-ing.", 0);
				DEBUGBREAK();
			} // end if (looking for TCP socket)
#endif // DEBUG

			fUDPBroadcast = TRUE;
		} // end if (address is the broadcast address)

#ifdef DEBUG
		if (pAddress->sin_port == 0)
		{
			DPL(0, "Looking for %s socket with an unspecified port!?  DEBUGBREAK()-ing.",
				1, ((fTCP) ? "TCP" : "UDP"));
			DEBUGBREAK();
		} // end if (no port)
#endif // DEBUG
	} // end if (there's an address)


	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PLINKEDSOCKET) this->GetItem(i);
		if (pItem == NULL)
		{
			DPL(0, "Failed to get socket %i!", 1, i);
			this->LeaveCritSection();
			return (NULL);
		} // end if (couldn't get that item)


		// Skip sockets of the wrong type.
		if (((fTCP) && (pItem->IsUDP())) ||
			((! fTCP) && (! pItem->IsUDP())))
		{
			continue;
		} // end if (socket of wrong type)


		// If the socket needs to be broadcast capable, and this isn't, skip it.
		if ((fUDPBroadcast) && (! pItem->IsUDPBroadcast()))
			continue;


		if (pAddress)
		{
			SOCKADDR_IN*	pCompareAddress;


			pCompareAddress = pItem->GetAddress();

			// If it's a local address, we only care about the port.
			if (fLocal)
			{
				// Check if it's using the right port.
				if (pCompareAddress->sin_port != pAddress->sin_port)
					continue;

				// If we're looking for a local TCP socket, make sure this
				// one is a TCP listen socket.
				if ((fTCP) && (! pItem->IsTCPListen()))
					continue;
			} // end if (local)
			else
			{
				// Check if it's a connection to the right address.
				if (memcmp(pCompareAddress, pAddress, sizeof (SOCKADDR_IN)) != 0)
					continue;
			} // end else (not local)
		} // end if (there's an address to compare)


		// If we're here, it means we found an item to use.  We're done.
		this->LeaveCritSection();
		return (pItem);
	} // end for (each item in this list)
	this->LeaveCritSection();

	return (NULL);
} // CLSocketsList::GetLSocketByAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::CTNCtrlCommTCP()"
//==================================================================================
// CTNCtrlCommTCP constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNCtrlCommTCP object, using the passed in parameters.
//
// Arguments:
//	PCONTROLCOMMINITPBLOCK pParams	How to create the object.
//	BOOL fWinSock2					Whether WinSock2 can be looked for.
//
// Returns: None (just the object).
//==================================================================================
CTNCtrlCommTCP::CTNCtrlCommTCP(PCONTROLCOMMINITPBLOCK pParams, BOOL fWinSock2):
	m_fWinSock2(fWinSock2)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNCtrlCommTCP));

	this->m_dwMethodID = pParams->dwControlMethodID;

	this->m_hWinSock2DLL = NULL;

	// Assume we're not going to use a Send thread.  See below.
	this->m_lpSendThreadProc = NULL;

	// Assume we're using WinSock 1 for now, this will get overwritten if we in
	// fact load WinSock 2.
	this->m_lpReceiveThreadProc = WS1ReceiveThreadProc;

	this->m_fWinSockStarted = FALSE;

	// If we're in a special mode, we need to delay our socket setup, otherwise,
	// go ahead and startup the appropriate ctrl method stuff
	this->m_fCtrlMethodMode = FALSE;
	if (pParams->dwFlags == 0)
	{
		this->m_fCtrlMethodMode = TRUE;
		this->m_lpSendThreadProc = WSSendThreadProc;
	} // end if (we're in normal mode)


	this->m_fWinSock2 = fWinSock2;
	this->m_hAllSocketsReceiveEvent = NULL;
	this->m_lpfnWSAEventSelect = NULL;
	this->m_lpfnWSAWaitForMultipleEvents = NULL;
	this->m_lpfnWSAResetEvent = NULL;

	this->m_pControlObj = pParams->pControlLayerObj;
	this->m_fMaster = pParams->fMaster;
} // CTNCtrlCommTCP::CTNCtrlCommTCP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::~CTNCtrlCommTCP()"
//==================================================================================
// CTNCtrlCommTCP destructor
//----------------------------------------------------------------------------------
//
// Description: Releases data associated with the CTNCtrlCommTCP object.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNCtrlCommTCP::~CTNCtrlCommTCP(void)
{
	HRESULT		hr;


	//DPL(0, "this = %x", 1, this);

	hr = this->Release();
	if (hr != S_OK)
	{
		DPL(0, "Releasing object failed!", 0);
	} // end if (releasing object failed)
} // CTNCtrlCommTCP::~CTNCtrlCommTCP
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::Initialize()"
//==================================================================================
// CTNCtrlCommTCP::Initialize
//----------------------------------------------------------------------------------
//
// Description: Initializes the UDP and TCP sockets necessary for communication.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::Initialize(void)
{
	HRESULT		hr = S_OK;
	WSADATA		wsaData;

	
	// Init WinSock.  First look for version 2.2, but fall back to version 1.1
	// if that's not available.
    hr = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (hr == WSAVERNOTSUPPORTED)
	{
		// There is a bug in winsock 1.1.  if you've called WSAStartup 1x in a
		// process, then if any subsequent call asks for a version # > then that
		// returned to the first call, you get WSAEVERNOTSUPPORTED.  We'll just
		// revert to 1.1 functionality.
	    hr = WSAStartup(MAKEWORD(1, 1), &wsaData);
	} // end if (version not supported)
	if (hr != 0) 
	{
		DPL(0, "Could not start WinSock!", 0);
		return (hr);
	} // end if (error opening WinSock)

	this->m_fWinSockStarted = TRUE;

	if (this->m_fWinSock2)
	{
		if (LOBYTE(wsaData.wVersion) >= 2)
		{
			DPL(1, "Found WinSock version %i.%i, using WinSock 2 functionality.",
				2, LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));

			hr = this->InitWinSock2();
			if (hr != S_OK)
			{
				DPL(0, "Couldn't initialize WinSock 2!", 0);
				return (hr);
			} // end if (failed to init winsock2)
		} // end if (we have WinSock 2 functionality)
		else
		{
			DPL(1, "Found WinSock version %i.%i, not able to use WinSock 2 functionality.",
				2, LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
			this->m_fWinSock2 = FALSE;
		} // end if (we don't have WinSock 2 functionality)
	} // end if (can use WinSock 2 if we found it)
	else
	{
		DPL(1, "Found WinSock version %i.%i; forcing to non-WinSock 2 functionality.",
			2, LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
	} // end else (we can't use WinSock 2, no matter what)


	// If we're in control method mode and we're a master we need a UDP socket that
	// can receive on a well known port.  If we're a slave we need any old UPD socket,
	// but it must be broadcast capable.

	if (this->m_fCtrlMethodMode)
	{
		if (this->m_fMaster)
			hr = this->CreateUDPSocket(CTRLCOMM_TCPIP_PORT, FALSE, NULL);
		else 
			hr = this->CreateUDPSocket(0, TRUE, NULL);

		if (hr != S_OK)
		{
			DPL(0, "Couldn't create appropriate UDP socket!", 0);
			return (hr);
		} // end if (couldn't create UDP socket)
	} // end if (in control method mode)


	// If we're in control method mode and we're the master, we need a listening
	// TCP socket.
	if ((this->m_fCtrlMethodMode) && (this->m_fMaster))
	{
		hr = this->CreateTCPListenSocket(CTRLCOMM_TCPIP_PORT, NULL);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't create appropriate TCP socket!", 0);
			return (hr);
		} // end if (couldn't create TCP socket)
	} // end if (in control method mode and master)


	hr = this->StartThreads();
	if (hr != S_OK)
	{
		DPL(0, "Failed to start threads!", 0);
		return (hr);
	} // end if (failed to start threads)

	return (S_OK);
} // CTNCtrlCommTCP::Initialize
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::Release()"
//==================================================================================
// CTNCtrlCommTCP::Release
//----------------------------------------------------------------------------------
//
// Description: Releases data associated with the WinSock TCP/IP control method.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::Release(void)
{
	HRESULT		hr;


	// Make sure everything is sent.
	hr = this->FlushSendQueue();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't flush the send queue!", 0);
		return (hr);
	} // end if (couldn't flush send queue)

	hr = this->KillThreads();
	if (hr != S_OK)
	{
		DPL(0, "Failed to kill threads!", 0);
		return (hr);
	} // end if (failed to kill threads)

	hr = this->m_socketlist.RemoveAll();
	if (hr != S_OK)
	{
		DPL(0, "Failed to remove all linked sockets!", 0);
		return (hr);
	} // end if (removeall failed)

	if (this->m_fWinSockStarted)
	{
		DPL(1, "Cleaning up WinSock.", 0);
		WSACleanup();
		this->m_fWinSockStarted = FALSE;
	} // end if (WinSock was started)

	if (this->m_hWinSock2DLL)
	{
		if (! FreeLibrary(this->m_hWinSock2DLL))
		{
			hr = GetLastError();
			DPL(0, "Could not free WinSock 2 library!", 0);
		} // end if (free library failed)

		this->m_hWinSock2DLL = NULL;
	} // end if (we have the WinSock2 DLL loaded)

	if (this->m_hAllSocketsReceiveEvent != NULL)
	{
		CloseHandle(this->m_hAllSocketsReceiveEvent);
		this->m_hAllSocketsReceiveEvent = NULL;
	} // end if (have receive event)

	return (hr);
} // CTNCtrlCommTCP::Release
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::GetBroadcastAddress()"
//==================================================================================
// CTNCtrlCommTCP::GetBroadcastAddress
//----------------------------------------------------------------------------------
//
// Description: Returns the address data necessary to broadcast data.
//				If the pointer to store the results in is NULL, then no memory
//				is copied, but the buffer size is still set to the amount of memory
//				required.
//
// Arguments:
//	PVOID pvModifierData		Additional data used to modify address, or NULL to
//								use defaults.
//	DWORD dwModifierDataSize	Size of additional data used to modify address.
//	PVOID pvAddress				Place to store the resulting address.
//	DWORD* pdwAddressSize		Pointer to hold the size of the buffer.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CTNCtrlCommTCP::GetBroadcastAddress(PVOID pvModifierData,
											DWORD dwModifierDataSize,
											PVOID pvAddress,
											DWORD* pdwAddressSize)
{
	HRESULT						hr = S_OK;
	SOCKADDR_IN*				pSocketAddress = (SOCKADDR_IN*) pvAddress;
	unsigned long*				pulIPaddr;

	
	if ((pvModifierData != NULL) && (dwModifierDataSize != sizeof (WORD)))
	{
		DPL(0, "Unexpected modifier data (size %u != %u)!",
			2, dwModifierDataSize, sizeof (WORD));
		return (ERROR_INVALID_PARAMETER);
	} // end if (unexpected modifier)

	(*pdwAddressSize) = sizeof (SOCKADDR_IN);

	if (pvAddress == NULL)
		return (S_OK);

	// Fill in address structure
	pSocketAddress->sin_family = AF_INET;
	if (pvModifierData == NULL)
	{
		pSocketAddress->sin_port = BYTEREVERSEWORD(CTRLCOMM_TCPIP_PORT);
	} // end if (no modifier data)
	else
	{
		pSocketAddress->sin_port = BYTEREVERSEWORD(*((WORD*) pvModifierData));
	} // end else (modifier data)

	pulIPaddr = (unsigned long*) (&pSocketAddress->sin_addr.S_un);
	(*pulIPaddr) = INADDR_BROADCAST;

	return (S_OK);
} // CTNCtrlCommTCP::GetBroadcastAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::ConvertStringToAddress()"
//==================================================================================
// CTNCtrlCommTCP::ConvertStringToAddress
//----------------------------------------------------------------------------------
//
// Description: Returns the usable address information the specified string
//				corresponds to.
//				If the pointer to store the results in is NULL, then no memory
//				is copied, but the buffer size is still set to the amount of memory
//				required.
//
// Arguments:
//	char* szString				String to convert.
//	PVOID pvModifierData		Additional data used to modify address, or NULL to
//								use defaults.
//	DWORD dwModifierDataSize	Size of additional data used to modify address.
//	PVOID pvAddress				Place to store the resulting address.
//	DWORD* pdwAddressSize		Pointer to hold the size of the buffer.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CTNCtrlCommTCP::ConvertStringToAddress(char* szString,
												PVOID pvModifierData,
												DWORD dwModifierDataSize,
												PVOID pvAddress,
												DWORD* pdwAddressSize)
{
	HRESULT				hr;
	SOCKADDR_IN*		pSocketAddress = (SOCKADDR_IN*) pvAddress;
	unsigned long*		pulIPaddr;
	PHOSTENT			phostent = NULL;
	int					i;
	IN_ADDR				hostaddr;

	
	if (szString == NULL)
	{
		DPL(0, "String to convert is not valid!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (string is invalid)

	if ((pvModifierData != NULL) && (dwModifierDataSize != sizeof (WORD)))
	{
		DPL(0, "Unexpected modifier data (size %u != %u)!",
			2, dwModifierDataSize, sizeof (WORD));
		return (ERROR_INVALID_PARAMETER);
	} // end if (unexpected modifier)

	(*pdwAddressSize) = sizeof (SOCKADDR_IN);

	if (pvAddress == NULL)
		return (S_OK);


	// Fill in address structure

	pSocketAddress->sin_family = AF_INET;
	if (pvModifierData == NULL)
	{
		pSocketAddress->sin_port = BYTEREVERSEWORD(CTRLCOMM_TCPIP_PORT);
	} // end if (no modifier data)
	else
	{
		pSocketAddress->sin_port = BYTEREVERSEWORD(*((WORD*) pvModifierData));
	} // end else (modifier data)

	pulIPaddr = (unsigned long*) (&(pSocketAddress->sin_addr.S_un));

	// This next call looks up the IP address for the string specified
	(*pulIPaddr) = inet_addr(szString);

	if ((*pulIPaddr) == INADDR_NONE)
	{
		phostent = gethostbyname(szString);
		if (phostent == NULL)
		{
			hr = WSAGetLastError();
			DPL(0, "Couldn't convert \"%s\" to IP address!",
				1, szString);

			return (hr);
		} // end if (couldn't convert the string any way)

		// BUGBUG there may be multiple ways to reach a dude, we should handle that 

		i = 0;
		while (phostent->h_addr_list[i] != NULL)
			i++;

		if (i < 1)
		{
			DPL(0, "Couldn't find any valid IP addresses for \"%s\"!",
				1, szString);
			return (E_FAIL);
		} // end if (didn't get any valid addresses)

		if (i > 1)
		{
			DPL(0, "WARNING: \"%s\" has multiple (%i) IP addresses!  Using first one.",
				2, szString, i);
			//return (E_FAIL);
		} // end if (didn't get any valid addresses)

		CopyMemory(&hostaddr, phostent->h_addr_list[0], sizeof (IN_ADDR));
		(*pulIPaddr) = hostaddr.S_un.S_addr;
	} // end if (we couldn't convert it that way)


	return (S_OK);
} // CTNCtrlCommTCP::ConvertStringToAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::GetSelfAddressAsString()"
//==================================================================================
// CTNCtrlCommTCP::GetSelfAddressAsString
//----------------------------------------------------------------------------------
//
// Description: Returns the string representation of this machine's address.
//				If the pointer to store the results in is NULL, then no memory
//				is copied, but the string size is still set to the amount of memory
//				required, including NULL termination.
//
// Arguments:
//	char* pszString			Pointer to buffer to store results in.
//	DWORD* pdwAddressSize	Pointer to hold the size of the buffer.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CTNCtrlCommTCP::GetSelfAddressAsString(char* pszString,
												DWORD* pdwStringSize)
{
	HRESULT			hr;
	char			szTemp[MAX_COMPUTERNAME_SIZE];
	DWORD			dwTemp = MAX_COMPUTERNAME_SIZE;
	PHOSTENT		phostent = NULL;
	int				i;
#ifdef _XBOX // Damn ANSI conversion
	WCHAR			szWideString[16];
#endif

	(*pdwStringSize) = 16;

	if (pszString == NULL)
		return (ERROR_BUFFER_TOO_SMALL);

#ifndef _XBOX // GetComputerName not supported
	if (! GetComputerName(szTemp, &dwTemp))
	{
		hr = GetLastError();
		DPL(0, "Couldn't get this computer's name!", 0);
		return (hr);
	} // end if (couldn't get this computer's name)
#else // ! XBOX
	dwTemp = MAX_COMPUTERNAME_SIZE;
	gethostname(szTemp, dwTemp);
#endif // XBOX

	phostent = gethostbyname(szTemp);
	if (phostent == NULL)
	{
		hr = WSAGetLastError();
		DPL(0, "Couldn't convert our name \"%s\" to IP address!",
			1, szTemp);

		return (hr);
	} // end if (couldn't convert the string any way)

	// BUGBUG there may be multiple ways to reach a dude, we should handle that 

	i = 0;
	while (phostent->h_addr_list[i] != NULL)
		i++;

	if (i < 1)
	{
		DPL(0, "Couldn't find any valid IP addresses for ourselves (\"%s\")!",
			1, szTemp);
		return (E_FAIL);
	} // end if (didn't get any valid addresses)

	if (i > 1)
	{
		DPL(0, "WARNING: We (\"%s\") have multiple (%i) IP addresses!  Using first one.",
			2, szTemp, i);
		//return (E_FAIL);
	} // end if (didn't get any valid addresses)

#ifndef _XBOX // Damn ANSI conversion
	wsprintf(pszString, "%i.%i.%i.%i",
			((LPBYTE) ((IN_ADDR*) (phostent->h_addr_list[0])))[0],
			((LPBYTE) ((IN_ADDR*) (phostent->h_addr_list[0])))[1],
			((LPBYTE) ((IN_ADDR*) (phostent->h_addr_list[0])))[2],
			((LPBYTE) ((IN_ADDR*) (phostent->h_addr_list[0])))[3]);
#else // ! XBOX
	// Use swprintf and then convert the result ot ANSI
	swprintf(szWideString, L"%i.%i.%i.%i",
			((LPBYTE) ((IN_ADDR*) (phostent->h_addr_list[0])))[0],
			((LPBYTE) ((IN_ADDR*) (phostent->h_addr_list[0])))[1],
			((LPBYTE) ((IN_ADDR*) (phostent->h_addr_list[0])))[2],
			((LPBYTE) ((IN_ADDR*) (phostent->h_addr_list[0])))[3]);
	wcstombs(pszString, szWideString, wcslen(szWideString));
	pszString[wcslen(szWideString)] = 0;
#endif // XBOX

	return (S_OK);
} // CTNCtrlCommTCP::GetSelfAddressAsString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::BindDataToAddress()"
//==================================================================================
// CTNCtrlCommTCP::BindDataToAddress
//----------------------------------------------------------------------------------
//
// Description: Binds the machine data with the address, particularly the event,
//				so if a connection drops, we can inform the control layer object
//				using us.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::BindDataToAddress(PCOMMDATA pCommData)
{
	SOCKADDR_IN*	pSockAddr = NULL;
	PLINKEDSOCKET	pLSocket = NULL;


	pSockAddr = (SOCKADDR_IN*) pCommData->pvAddress;
	if (pCommData->dwAddressSize != sizeof (SOCKADDR_IN))
	{
		DPL(0, "Got passed unexpected address data!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (we got passed a wacky address)

	
#pragma TODO(vanceo, "Handle UDP case gracefully.")
	pLSocket = this->m_socketlist.GetLSocketByAddress(pSockAddr, TRUE);
	if (pLSocket == NULL)
	{
		DPL(0, "Couldn't find socket in list!", 0);
		return (E_FAIL);
	} // end if (couldn't find item in list)

	pLSocket->m_pCommData = pCommData;

	return (S_OK);
} // CTNCtrlCommTCP::BindDataToAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::UnbindDataFromAddress()"
//==================================================================================
// CTNCtrlCommTCP::UnbindDataFromAddress
//----------------------------------------------------------------------------------
//
// Description: Unbinds the machine data from the address.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::UnbindDataFromAddress(PCOMMDATA pCommData)
{
	HRESULT			hr;
	SOCKADDR_IN*	pSockAddr = NULL;
	PLINKEDSOCKET	pLSocket = NULL;


	
	if (pCommData->dwAddressSize != sizeof (SOCKADDR_IN))
	{
		DPL(0, "Got passed unexpected address data!", 0);
		return (ERROR_INVALID_PARAMETER);
	} // end if (we got passed a wacky address)

	pSockAddr = (SOCKADDR_IN*) pCommData->pvAddress;


	this->m_socketlist.EnterCritSection();

	// Assume its TCP...
	pLSocket = this->m_socketlist.GetLSocketByAddress(pSockAddr, TRUE);
	if (pLSocket == NULL)
	{
		// If it's not TCP, see if it's UDP.
		pLSocket = this->m_socketlist.GetLSocketByAddress(pSockAddr, FALSE);
		if (pLSocket == NULL)
		{
			DPL(0, "Couldn't find socket in list!", 0);
			this->m_socketlist.LeaveCritSection();
			return (E_FAIL);
		} // end if (couldn't find UDP socket in list)

		DPL(2, "No need to unbind UDP sockets (leaving %o alone).",
			1, pLSocket->GetAddress());

		this->m_socketlist.LeaveCritSection();
		return (S_OK);
	} // end if (couldn't find TCP socket in list)

	// If there's no data bound to this socket yet, it means the user just wants
	// to disconnect.  Don't print the warning in that case.  Otherwise, note if
	// somehow the bound data doesn't match what the user passed in.
	if ((pLSocket->m_pCommData != NULL) &&
		(pLSocket->m_pCommData != pCommData))
	{
		DPL(0, "WARNING: Unbinding different data (%x != %x)!",
			2, pLSocket->m_pCommData, pCommData);
	} // end if (the socket was bound but data wasn't right)

	// The user wants to shut the connection down.
	hr = pLSocket->CloseSocket();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't close the socket!", 0);
		this->m_socketlist.LeaveCritSection();
		return (hr);
	} // end if (failed closing the socket)

	// Pull it off the list
	hr = this->m_socketlist.RemoveFirstReference(pLSocket);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't remove socket from list!", 0);
		this->m_socketlist.LeaveCritSection();
		return (E_FAIL);;
	} // end if (couldn't find socket in list)

	if (pLSocket->m_dwRefCount != 0)
	{
		DPL(0, "WARNING: Still an outstanding reference to linked socket %x!",
			1, pLSocket);
	} // end if (not last reference to object)
	else
	{
		delete (pLSocket);
		pLSocket = NULL;
	} // end else (that was the last reference)

	if (this->m_fWinSock2)
	{
		if (! SetEvent(this->m_socketlist.m_hListChangedEvent))
		{
			hr = GetLastError();
			DPL(0, "Couldn't set socket list changed event (%x)!",
				1, this->m_socketlist.m_hListChangedEvent);
		} // end if (couldn't notify thread that the list changed)
	} // end if (we're using WinSock2)

	this->m_socketlist.LeaveCritSection();

	// Just to make sure we know he's gone.
	pCommData->fDropped = TRUE;

	return (hr);
} // CTNCtrlCommTCP::UnbindDataFromAddress
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::FlushSendQueue()"
//==================================================================================
// CTNCtrlCommTCP::FlushSendQueue
//----------------------------------------------------------------------------------
//
// Description: Waits for all outgoing data to be sent.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::FlushSendQueue(void)
{
	HRESULT		hr;
	int			i = 0;


	DPL(9, "==>", 0);

#pragma BUGBUG(vanceo, "Improve this whole function (dumping destroys reliability)")

	// Give the send queue some time to empty.
	while (this->m_sendqueue.Count() > 0)
	{
		i++;
		if (i > 20) // 10 seconds total
		{
			DPL(0, "Aborting flush, 10 seconds reached, %i items in queue.",
				1, this->m_sendqueue.Count());
			break;
		} // end if (waited maximum amount)

		Sleep(500); // sleep for half a second
	} // end while (the send queue isn't empty)


	// If there's still some items left, dump them.
	hr = this->m_sendqueue.RemoveAll();


	// HACKHACK: This extra sleep hopefully allows the send thread to get the
	//			 current message its sending (if there were one) out the door.
	//			 It's not on the queue, so the above while won't cover this.
	Sleep(500);


	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNCtrlCommTCP::FlushSendQueue
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::StartAcceptingReachCheck()"
//==================================================================================
// CTNCtrlCommTCP::StartAcceptingReachCheck
//----------------------------------------------------------------------------------
//
// Description: Starts allowing incoming IP/NAT detection data for the given reach
//				check object.
//
// Arguments:
//	PTNREACHCHECK pReachCheck	Reach check being performed.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::StartAcceptingReachCheck(PTNREACHCHECK pReachCheck)
{
	HRESULT			hr = S_OK;
	SOCKADDR_IN		sockaddrin;
	unsigned long*	pulIPaddr = NULL;
	PLINKEDSOCKET	pLSocket = NULL;
	BOOL			fAddedToList = FALSE;
	BOOL			fInCritSect = FALSE;


	DPL(9, "==> (%x)", 1, pReachCheck);


	// The method data is a port, so make sure that's actually what we got.
	if (pReachCheck->m_dwMethodDataSize != sizeof (WORD))
	{
		DPL(0, "Reach check method data is not expected size (%u != %u)!",
			2, pReachCheck->m_dwMethodDataSize, sizeof (WORD));
		hr = E_FAIL;
		goto ERROR_EXIT;
	} // end if (data is wrong)

	// Make sure we're not being called to accept for a method we don't support.
	if ((pReachCheck->m_dwMethod != TNRCM_UDPBROADCAST) &&
		(pReachCheck->m_dwMethod != TNRCM_UDP) &&
		(pReachCheck->m_dwMethod != TNRCM_TCP))
	{
		DPL(0, "This control comm object doesn't support reach check method %u!",
			1, pReachCheck->m_dwMethod);
		hr = E_FAIL;
		goto ERROR_EXIT;
	} // end if (wrong method)


	// Add this item (or an alias to it actually) to our internal list.
	hr = this->m_reachchecks.Add(pReachCheck);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add alias to reach check to list!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't add alias)

	fAddedToList = TRUE;


	ZeroMemory(&sockaddrin, sizeof (SOCKADDR_IN));
	sockaddrin.sin_family = AF_INET;
	pulIPaddr = (unsigned long*) (&(sockaddrin.sin_addr.S_un));
	(*pulIPaddr) = INADDR_ANY;
	sockaddrin.sin_port = BYTEREVERSEWORD(*((WORD*) pReachCheck->m_pvMethodData));


	DPL(1, "Trying to use %s sockets on port %u.",
		2, ((pReachCheck->m_dwMethod == TNRCM_TCP) ? "TCP" : "UDP"),
		*((WORD*) pReachCheck->m_pvMethodData));


	this->m_socketlist.EnterCritSection();
	fInCritSect = TRUE;

	
	// Find or add an appropriate socket on the given port.
	if (pReachCheck->m_dwMethod == TNRCM_TCP)
	{
		pLSocket = this->m_socketlist.GetLSocketByAddress(&sockaddrin, TRUE);
		if (pLSocket == NULL)
		{
			DPL(1, "Couldn't find TCP socket on port %u, will try to create one.",
				1, *((WORD*) pReachCheck->m_pvMethodData));
				
			hr = this->CreateTCPListenSocket(*((WORD*) pReachCheck->m_pvMethodData),
											&pLSocket);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't create TCP listen socket on port %u!",
					1, *((WORD*) pReachCheck->m_pvMethodData));
				goto ERROR_EXIT;
			} // end if (couldn't create TCP socket)
		} // end if (didn't find a TCP socket already)
		else
		{
			pLSocket->m_dwFoundPriorToReachCheck++;
		} // end else (found TCP socket)
	} // end if (TCP reach check)
	else
	{
		pLSocket = this->m_socketlist.GetLSocketByAddress(&sockaddrin, FALSE);
		if (pLSocket == NULL)
		{
			DPL(1, "Couldn't find UDP socket on port %u, will try to create one.",
				1, *((WORD*) pReachCheck->m_pvMethodData));
				
			//BUGBUG maybe allow using port 0, if so retrieve the port actually used
			hr = this->CreateUDPSocket(*((WORD*) pReachCheck->m_pvMethodData), FALSE,
										&pLSocket);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't create UDP socket on port %u!",
					1, *((WORD*) pReachCheck->m_pvMethodData));
				goto ERROR_EXIT;
			} // end if (couldn't create UDP socket)
		} // end if (didn't find a UDP socket already)
		else
		{
			pLSocket->m_dwFoundPriorToReachCheck++;
		} // end else (found UDP socket)
	} // end else (UDP or UDP broadcast reach check)


	// Add a ref for the object, and attach it to the reach check.
	pLSocket->m_dwRefCount++;
	pReachCheck->m_pvCommData = pLSocket;


	fInCritSect = FALSE;
	this->m_socketlist.LeaveCritSection();


	DPL(9, "<== S_OK", 0);

	return (S_OK);


ERROR_EXIT:

	if (fAddedToList)
	{
		// Ignoring error
		this->m_reachchecks.RemoveFirstReference(pReachCheck);
		if (pReachCheck->m_dwRefCount == 0)
		{
			DPL(0, "WARNING: Deleting search %x because reference count is 0!?",
				1, pReachCheck);

			DEBUGBREAK();

			delete (pReachCheck);
			pReachCheck = NULL;
		} // end if (should delete search now)

		fAddedToList = FALSE;
	} // end if (added search to list)

	if (fInCritSect)
	{
		this->m_socketlist.LeaveCritSection();
		fInCritSect = FALSE;
	} // end if (in critical section)


	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNCtrlCommTCP::StartAcceptingReachCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::StopAcceptingReachCheck()"
//==================================================================================
// CTNCtrlCommTCP::StopAcceptingReachCheck
//----------------------------------------------------------------------------------
//
// Description: Stops allowing incoming IP/NAT detection data for the given
//				reach check object.
//
// Arguments:
//	PTNREACHCHECK pReachCheck	Reach check being performed.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::StopAcceptingReachCheck(PTNREACHCHECK pReachCheck)
{
	HRESULT		hr;


	DPL(9, "==> (%x)", 1, pReachCheck);

	hr = this->InternalCloseReachCheck(pReachCheck);

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNCtrlCommTCP::StopAcceptingReachCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::PrepareToReachCheck()"
//==================================================================================
// CTNCtrlCommTCP::PrepareToReachCheck
//----------------------------------------------------------------------------------
//
// Description: Prepares necessary items to allow sending IP/NAT detection data for
//				the given reach check object.
//
// Arguments:
//	PTNREACHCHECK pReachCheck	Reach check being performed.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::PrepareToReachCheck(PTNREACHCHECK pReachCheck)
{
	HRESULT			hr = S_OK;
	SOCKADDR_IN		sockaddrin;
	unsigned long*	pulIPaddr = NULL;
	PLINKEDSOCKET	pLSocket = NULL;
	BOOL			fAddedToList = FALSE;
	BOOL			fInCritSect = FALSE;


	DPL(9, "==> (%x)", 1, pReachCheck);


	// The method data is a port, so make sure that's actually what we got.
	if (pReachCheck->m_dwMethodDataSize != sizeof (WORD))
	{
		DPL(0, "Reach check method data is not expected size (%u != %u)!",
			2, pReachCheck->m_dwMethodDataSize, sizeof (WORD));
		hr = E_FAIL;
		goto ERROR_EXIT;
	} // end if (data is wrong)

	// Make sure we're not being called to accept for a method we don't support.
	if ((pReachCheck->m_dwMethod != TNRCM_UDPBROADCAST) &&
		(pReachCheck->m_dwMethod != TNRCM_UDP) &&
		(pReachCheck->m_dwMethod != TNRCM_TCP))
	{
		DPL(0, "This control comm object doesn't support reach check method %u!",
			1, pReachCheck->m_dwMethod);
		hr = E_FAIL;
		goto ERROR_EXIT;
	} // end if (wrong method)


	// Add this item (or an alias to it actually) to our internal list.
	hr = this->m_reachchecks.Add(pReachCheck);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add alias to reach check to list!", 0);
		goto ERROR_EXIT;
	} // end if (couldn't add alias)

	fAddedToList = TRUE;


	ZeroMemory(&sockaddrin, sizeof (SOCKADDR_IN));
	sockaddrin.sin_family = AF_INET;
	pulIPaddr = (unsigned long*) (&(sockaddrin.sin_addr.S_un));
	(*pulIPaddr) = INADDR_ANY;
	sockaddrin.sin_port = BYTEREVERSEWORD(*((WORD*) pReachCheck->m_pvMethodData));


	DPL(1, "Trying to use %s sockets on port %u.",
		2, ((pReachCheck->m_dwMethod == TNRCM_TCP) ? "TCP" : "UDP"),
		*((WORD*) pReachCheck->m_pvMethodData));


	this->m_socketlist.EnterCritSection();
	fInCritSect = TRUE;


	// Find or add an appropriate socket, UDP only.  TCP will make the various
	// connections on the fly.  We don't need to remember the sockets we used,
	// because as long as the other side shuts the connections down when they're
	// done with it, our side will notice that (and the fact that the socket
	// isn't being used by anybody anymore) and remove it from our list, too.
	// UDP has to have a socket to receive on as well as send to using the
	// specified port, so we make sure we have one capable of doing that.
	if (pReachCheck->m_dwMethod != TNRCM_TCP)
	{
		// Make sure there's a UDP socket that's using that port.  We'll use
		// it both for sending and for listening for replies.
		pLSocket = this->m_socketlist.GetLSocketByAddress(&sockaddrin,
														FALSE);
		if (pLSocket == NULL)
		{
			hr = this->CreateUDPSocket(*((WORD*) pReachCheck->m_pvMethodData),
										((pReachCheck->m_dwMethod == TNRCM_UDPBROADCAST) ? TRUE : FALSE),
										&pLSocket);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't create UDP socket on port %u!",
					1, *((WORD*) pReachCheck->m_pvMethodData));
				goto ERROR_EXIT;
			} // end if (couldn't create UDP socket)
		} // end if (didn't find a UDP socket already)
		else
		{
			// If it's the UDP broadcast method but this socket isn't capable
			// of broadcasting, change it.
			if ((pReachCheck->m_dwMethod == TNRCM_UDPBROADCAST) &&
				(! pLSocket->IsUDPBroadcast()))
			{
				BOOL	fTemp;
				

				if (setsockopt(*(pLSocket->GetSocket()), SOL_SOCKET, SO_BROADCAST, (char*) &fTemp, sizeof (BOOL)) != 0)
				{
					hr = WSAGetLastError();
					DPL(0, "Failed to set datagram socket (port %u) as broadcast capable!",
						1, *((WORD*) pReachCheck->m_pvMethodData));
					goto ERROR_EXIT;
				} // end if (failed to set datagram socket options)
			} // end if (it's not broadcast capable)

			pLSocket->m_dwFoundPriorToReachCheck++;
		} // end else (found UDP socket)


		// Add a ref for the object, and attach it to the reach check.
		pLSocket->m_dwRefCount++;
		pReachCheck->m_pvCommData = pLSocket;
	} // end if (not TCP reach check method)


	fInCritSect = FALSE;
	this->m_socketlist.LeaveCritSection();


	DPL(9, "<== S_OK", 0);

	return (S_OK);


ERROR_EXIT:

	if (fAddedToList)
	{
		// Ignoring error
		this->m_reachchecks.RemoveFirstReference(pReachCheck);
		if (pReachCheck->m_dwRefCount == 0)
		{
			DPL(0, "WARNING: Deleting search %x because reference count is 0!?",
				1, pReachCheck);

			DEBUGBREAK();

			delete (pReachCheck);
			pReachCheck = NULL;
		} // end if (should delete search now)

		fAddedToList = FALSE;
	} // end if (added search to list)

	if (fInCritSect)
	{
		this->m_socketlist.LeaveCritSection();
		fInCritSect = FALSE;
	} // end if (in critical section)


	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNCtrlCommTCP::PrepareToReachCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::CleanupReachCheck()"
//==================================================================================
// CTNCtrlCommTCP::CleanupReachCheck
//----------------------------------------------------------------------------------
//
// Description: Stops trying to send IP/NAT detection data for the given reach check
//				object.
//
// Arguments:
//	PTNREACHCHECK pReachCheck	Reach check being performed.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::CleanupReachCheck(PTNREACHCHECK pReachCheck)
{
	HRESULT		hr;


	DPL(9, "==> (%x)", 1, pReachCheck);

	hr = this->InternalCloseReachCheck(pReachCheck);

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNCtrlCommTCP::CleanupReachCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::SendFirstData()"
//==================================================================================
// CTNCtrlCommTCP::SendFirstData
//----------------------------------------------------------------------------------
//
// Description: Sends the first item in the send queue via UDP or TCP, depending on
//				the reliability setting.
//				NOTE: The sendqueue lock must be held with 1 and only 1 level of
//				recursion upon entering this function.  
//
// Arguments: None.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CTNCtrlCommTCP::SendFirstData(void)
{
	HRESULT			hr = S_OK;
	PTNSENDDATA		pSendData = NULL;


	// Pop the first item off the list
	pSendData = (PTNSENDDATA) this->m_sendqueue.PopFirstItem();
	if (pSendData == NULL)
	{
		DPL(0, "Failed to pop first item in queue!", 0);
		return (E_FAIL);
	} // end if (failed to get first item)

	// Drop lock now that we pulled the send off the queue
	this->m_sendqueue.LeaveCritSection();

	/*
	DPL(0, "Sending %i bytes of data at %x to %o", 3, pSendData->dwDataSize,
		pSendData->lpData, pSendData->m_pvAddress);
	*/

	if (! pSendData->m_fGuaranteed)
	{
		hr = this->SendUDPDataTo((SOCKADDR_IN*) (pSendData->m_pvAddress),
								pSendData->m_pvData, pSendData->m_dwDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Failed to send UDP data to %o!  %e  Ignoring.",
				2, hr, pSendData->m_pvAddress);
			hr = S_OK;
		} // end if (failed to send UDP message)
	} // end if (we don't have to send it guaranteed)
	else
	{
		hr = this->SendTCPDataTo((SOCKADDR_IN*) (pSendData->m_pvAddress),
								pSendData->m_pvData, pSendData->m_dwDataSize);
		if (hr != S_OK)
		{
			DPL(0, "Failed to send TCP data to %o, removing all items still queued up for him!",
				1, pSendData->m_pvAddress);

			// Ignore error
			this->m_sendqueue.RemoveAllItemsTo(pSendData->m_pvAddress,
												pSendData->m_dwDataSize);
		} // end if (failed to send TCP message)
	} // end else (we need to send it guaranteed)

	if (hr != S_OK)
	{
		DPL(0, "Failed sending %u bytes of %seliable data to %o submitted by %X!  %e",
			5, pSendData->m_dwDataSize,
			(pSendData->m_fGuaranteed ? "r" : "unr"),
			(SOCKADDR_IN*) (pSendData->m_pvAddress),
			pSendData->m_pvSubmittersAddress,
			hr);
	} // end if (there was a failure)

	pSendData->m_dwRefCount--; // subtract our refcount
	if (pSendData->m_dwRefCount == 0)
	{
		delete (pSendData);
		pSendData = NULL;
	} // end if (last reference to object)
	else
	{
		DPL(0, "WARNING: Still %u outstanding references to send data object %x!?",
			2, pSendData->m_dwRefCount, pSendData);
	} // end else (not last reference to object)


	// Take the lock again to return us to the state we were in when we entered.
	this->m_sendqueue.EnterCritSection();

	return (hr);
} // CTNCtrlCommTCP::SendFirstData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::CheckForReceiveData()"
//==================================================================================
// CTNCtrlCommTCP::CheckForReceiveData
//----------------------------------------------------------------------------------
//
// Description: Instantly checks for data on the receive socket and puts it into
//				the control message queue & sets the receive event if there is.
//
// Arguments: None.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CTNCtrlCommTCP::CheckForReceiveData(void)
{
	HRESULT			hr = S_OK;
	int				i;
	PLINKEDSOCKET	pLSocket = NULL;



	this->m_socketlist.EnterCritSection();
	for(i = 0; i < this->m_socketlist.Count(); i++)
	{
		pLSocket = (PLINKEDSOCKET) this->m_socketlist.GetItem(i);
		if (pLSocket == NULL)
		{
			DPL(0, "Failed to get socket %i in list!", 1, i);
			this->m_socketlist.LeaveCritSection();
			return (E_FAIL);
		} // end if (couldn't get item)

		if (*(pLSocket->GetSocket()) == INVALID_SOCKET)
		{
			DPL(0, "Skipping socket %i because it has been shutdown.", 1, i);
		} // end if (the socket is gone)
		else
		{
			// Note that this may drop the socket list lock.  That should be
			// okay, but at worst we might skip checking some sockets this
			// time around (only would happen if sockets got inserted before
			// this item, which should never happen since we always append
			// at the end of the list).
			hr = this->CheckAndHandleEventOnSocket(pLSocket);
			if (hr != S_OK)
			{
				DPL(0, "Failed to check for data or shutdown on socket %i!", 1, i);
				this->m_socketlist.LeaveCritSection();
				return (hr);
			} // end if (failed to check for socket event)
		} // end else (it's a valid socket)
	} // end for (each socket in the list)
	this->m_socketlist.LeaveCritSection();

	return (S_OK);
} // CTNCtrlCommTCP::CheckForReceiveData
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::InitWinSock2()"
//==================================================================================
// CTNCtrlCommTCP::InitWinSock2
//----------------------------------------------------------------------------------
//
// Description: Attempts to load and initialize the WinSock2 DLL.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::InitWinSock2(void)
{
	HRESULT		hr;


#ifndef _XBOX // Damn ANSI conversoin
	this->m_hWinSock2DLL = LoadLibrary("ws2_32.dll");
#else // ! XBOX
	this->m_hWinSock2DLL = LoadLibrary(L"ws2_32.dll");
#endif // XBOX
	if (! this->m_hWinSock2DLL) 
	{
		hr = GetLastError();
		DPL(0, "Could not load ws2_32.dll!", 0);
		goto ERROR_EXIT;
	} // end if (we couldn't load the WinSock2 DLL)

	// Get pointers to the entry points we need

	this->m_lpfnWSAEventSelect = (LPFN_WSAEVENTSELECT) GetProcAddress(this->m_hWinSock2DLL,
																"WSAEventSelect");
	if (! this->m_lpfnWSAEventSelect)
	{
		hr = GetLastError();
		DPL(0, "Could not find WinSock entry point \"WSAEventSelect\"!", 0);
		goto ERROR_EXIT;
	} // end if (we couldn't load the WinSock2 DLL)

	this->m_lpfnWSAWaitForMultipleEvents = (LPFN_WSAWAITFORMULTIPLEEVENTS)
											GetProcAddress(this->m_hWinSock2DLL,
															"WSAWaitForMultipleEvents");
	if(! this->m_lpfnWSAWaitForMultipleEvents)
	{
		hr = GetLastError();
		DPL(0, "Could not find WinSock entry point \"WSAWaitForMultipleEvents\"!", 0);
		goto ERROR_EXIT;
	} // end if (we couldn't load the WinSock2 DLL)

	this->m_lpfnWSAResetEvent = (LPFN_WSARESETEVENT) GetProcAddress(this->m_hWinSock2DLL,
																"WSAResetEvent");
	if (! this->m_lpfnWSAResetEvent)
	{
		hr = GetLastError();
		DPL(0, "Could not find WinSock entry point \"WSAResetEvent\"!", 0);
		goto ERROR_EXIT;
	} // end if (we couldn't load the WinSock2 DLL)

	// Override the WinSock 1 receive thread proc we specified.
	if (this->m_dwMethodID == TN_CTRLMETHOD_TCPIP_WINSOCK2_ONETOONE)
	{
		DPL(0, "WARNING: Using one-to-one mapping of sockets and events, connections can be starved!", 0);

		this->m_lpReceiveThreadProc = WS2OneToOneReceiveThreadProc;
	} // end if (should use one to one event/socket mapping)
	else
	{
		this->m_lpReceiveThreadProc = WS2ReceiveThreadProc;

		this->m_hAllSocketsReceiveEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (this->m_hAllSocketsReceiveEvent == NULL)
		{
			hr = GetLastError();
			DPL(0, "Couldn't create generic receive event!", 0);
			goto ERROR_EXIT;
		} // end if (couldn't create event)
	} // end else (shouldn't use one to one event/socket mapping)

	return (S_OK);	


ERROR_EXIT:

	if (this->m_hWinSock2DLL != NULL)
	{
		FreeLibrary(this->m_hWinSock2DLL);
		this->m_hWinSock2DLL = NULL;
	} // end if (have WinSock2 DLL)

	return (hr);
} // CTNCtrlCommTCP::InitWinSock2
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::CreateUDPSocket()"
//==================================================================================
// CTNCtrlCommTCP::CreateUDPSocket
//----------------------------------------------------------------------------------
//
// Description: Creates a new UDP socket with the passed in port.
//
// Arguments:
//	WORD wPort					Port to bind UDP socket to (0 for random/doesn't
//								matter).
//	BOOL fBroadcast				Whether to allow broadcasts on the socket or not.
//	PLINKEDSOCKET* ppLSocket	Place to store pointer to socket created, or NULL
//								if not wanted.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::CreateUDPSocket(WORD wPort, BOOL fBroadcast,
										PLINKEDSOCKET* ppLSocket)
{
	HRESULT			hr;
	SOCKET			sUDP = INVALID_SOCKET;
	SOCKADDR_IN		addr;
	unsigned long*	lpulIPaddr;
	HANDLE			hWinSock2Event = NULL;
	BOOL			fTemp = TRUE;			


	sUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sUDP == INVALID_SOCKET)
	{
		hr = WSAGetLastError();
		DPL(0, "Failed to create a datagram socket!", 0);
		return (hr);
	} // end if (failed to create a datagram socket)


	ZeroMemory(&addr, sizeof (SOCKADDR_IN));

	addr.sin_family = AF_INET;
	lpulIPaddr = (unsigned long*) (&addr.sin_addr.S_un);
	(*lpulIPaddr) = INADDR_ANY;
	addr.sin_port = BYTEREVERSEWORD(wPort);

	if (bind(sUDP, (SOCKADDR*) (&addr), sizeof (SOCKADDR_IN)) != 0)
	{
		hr = WSAGetLastError();
		DPL(0, "Failed to bind datagram socket on port %u!",
			1, wPort);
		return (hr);
	} // end if (failed to bind datagram socket)


	// If the port wasn't specified, retrieve what actually got bound.
	if (wPort == 0)
	{
		int		iSize;


		ZeroMemory(&addr, sizeof (SOCKADDR_IN));

		iSize = sizeof (SOCKADDR_IN);
		if (getsockname(sUDP, (SOCKADDR*) &addr, &iSize) != 0)
		{
			hr = WSAGetLastError();

			DPL(0, "Couldn't get information on just bound datagram socket!", 0);

			if (hr == S_OK)
				hr = E_FAIL;

			return (hr);
		} // end if (couldn't get information on bound socket)

		DPL(8, "Binding datagram socket to unspecified port got assigned as %o.",
			1, &addr);
	} // end if (got a zero port)


	if (fBroadcast)
	{
		if (setsockopt(sUDP, SOL_SOCKET, SO_BROADCAST, (char*) &fTemp, sizeof (BOOL)) != 0)
		{
			hr = WSAGetLastError();
			DPL(0, "Failed to set datagram socket options on port %u!",
				1, wPort);
			return (hr);
		} // end if (failed to set datagram socket options)
	} // end if (broadcasts are allowed on the socket)


	// If we're using WinSock 2, associate an event with this socket
	if (this->m_fWinSock2)
	{
		if (this->m_hAllSocketsReceiveEvent != NULL)
		{
			hWinSock2Event = this->m_hAllSocketsReceiveEvent;
		} // end if (there's a generic receive event)
		else
		{
			hWinSock2Event = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (hWinSock2Event == NULL)
			{
				hr = GetLastError();
				DPL(0, "Couldn't create socket event!", 0);
				return (hr);
			} // end if (couldn't create event)
		} // end else (there's no generic receive event)

		hr = this->m_lpfnWSAEventSelect(sUDP, hWinSock2Event, FD_READ);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't associate receive event with datagram socket on port %u!",
				1, wPort);
			return (hr);
		} // end if (WSAEventSelect failed)
	} // end if (we're using WinSock2 functionality)

	hr = this->m_socketlist.AddSocket(sUDP, addr, FALSE, FALSE, fBroadcast,
									hWinSock2Event, ppLSocket);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add new UDP socket to list!", 0);
	} // end if (couldn't add socket)

	return (hr);
} // CTNCtrlCommTCP::CreateUDPSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::CreateTCPListenSocket()"
//==================================================================================
// CTNCtrlCommTCP::CreateTCPListenSocket
//----------------------------------------------------------------------------------
//
// Description: Creates a new TCP listen socket with the passed in port.
//
// Arguments:
//	WORD wPort					Port to bind TCP socket to.
//	PLINKEDSOCKET* ppLSocket	Place to store pointer to socket created, or NULL
//								if not wanted.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::CreateTCPListenSocket(WORD wPort, PLINKEDSOCKET* ppLSocket)
{
	HRESULT			hr;
	SOCKET			sTCP = INVALID_SOCKET;
	SOCKADDR_IN		addr;
	unsigned long*	lpulIPaddr;
	HANDLE			hWinSock2Event = NULL;


	sTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sTCP == INVALID_SOCKET)
	{
		hr = WSAGetLastError();
		DPL(0, "Failed to create a stream socket!", 0);
		return (hr);
	} // end if (failed to create a stream socket)


	ZeroMemory(&addr, sizeof (SOCKADDR_IN));

	addr.sin_family = AF_INET;
	lpulIPaddr = (unsigned long*) (&addr.sin_addr.S_un);
	(*lpulIPaddr) = INADDR_ANY;
	addr.sin_port = BYTEREVERSEWORD(wPort);

	if (bind(sTCP, (SOCKADDR*) (&addr), sizeof (SOCKADDR_IN)) != 0)
	{
		hr = WSAGetLastError();
		DPL(0, "Failed to bind stream socket to port %u!",
			1, wPort);
		return (hr);
	} // end if (failed to bind stream socket)

	if (listen(sTCP, SOMAXCONN) != 0)
	{
		hr = WSAGetLastError();
		DPL(0, "Failed to start inbound stream socket listening on port %u!",
			1, wPort);
		return (hr);
	} // end if (failed to listen inbound stream socket)


	// If we're using WinSock 2, associate an event with this socket
	if (this->m_fWinSock2)
	{
		if (this->m_hAllSocketsReceiveEvent != NULL)
		{
			hWinSock2Event = this->m_hAllSocketsReceiveEvent;
		} // end if (there's a generic receive event)
		else
		{
			hWinSock2Event = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (hWinSock2Event == NULL)
			{
				hr = GetLastError();
				DPL(0, "Couldn't create socket event!", 0);
				return (hr);
			} // end if (couldn't create event)
		} // end else (there's no generic receive event)

		hr = this->m_lpfnWSAEventSelect(sTCP, hWinSock2Event, FD_ACCEPT | FD_READ);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't associate receive event with TCP listen socket on port %u!",
				1, wPort);
			return (hr);
		} // end if (WSAEventSelect failed)
	} // end if (we're using WinSock2 functionality)

	hr = this->m_socketlist.AddSocket(sTCP, addr, TRUE, TRUE, FALSE, hWinSock2Event,
									ppLSocket);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add new TCP listen socket to list!", 0);
	} // end if (couldn't add socket)

	return (hr);
} // CTNCtrlCommTCP::CreateTCPListenSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::CreateAndConnectTCPSocket()"
//==================================================================================
// CTNCtrlCommTCP::CreateAndConnectTCPSocket
//----------------------------------------------------------------------------------
//
// Description: Creates a new TCP socket and connects it to the passed in address.
//
// Arguments:
//	SOCKADDR_IN* pAddress		Address to establish connection to.
//	PLINKEDSOCKET* ppLSocket	Place to store pointer to socket created, or NULL
//								if not wanted.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::CreateAndConnectTCPSocket(SOCKADDR_IN* pAddress,
												PLINKEDSOCKET* ppLSocket)
{
	HRESULT			hr;
	SOCKET			sTCP = INVALID_SOCKET;
	SOCKADDR_IN		addr;
	unsigned long*	pulIPaddr = NULL;
	HANDLE			hWinSock2Event = NULL;

	
	sTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sTCP == INVALID_SOCKET)
	{
		hr = WSAGetLastError();
		DPL(0, "Failed to create a TCP send socket!", 0);
		return (hr);
	} // end if (failed to get a TCP socket)


	ZeroMemory(&addr, sizeof (SOCKADDR_IN));
	addr.sin_family = AF_INET;
	pulIPaddr = (unsigned long*) (&addr.sin_addr.S_un);
	(*pulIPaddr) = INADDR_ANY;
	addr.sin_port = 0;

	if (bind(sTCP, (SOCKADDR*) (&addr), sizeof (SOCKADDR_IN)) != 0)
	{
		hr = WSAGetLastError();
		DPL(0, "Failed to bind TCP socket to %o!", 1, (&addr));
		return (hr);
	} // end if (failed to bind TCP socket)


	DPL(7, "Attempting to establish new TCP connection to %o.",
		1, pAddress);

	if (connect(sTCP, (SOCKADDR*) pAddress, sizeof (SOCKADDR_IN)) != 0)
	{
		hr = WSAGetLastError();
		DPL(0, "Failed to connect TCP socket to %o!", 1, pAddress);
		return (hr);
	} // end if (failed to connect TCP socket)

	if (this->m_fWinSock2)
	{
		if (this->m_hAllSocketsReceiveEvent != NULL)
		{
			hWinSock2Event = this->m_hAllSocketsReceiveEvent;
		} // end if (there's a generic receive event)
		else
		{
			hWinSock2Event = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (hWinSock2Event == NULL)
			{
				hr = GetLastError();
				DPL(0, "Couldn't create socket event!", 0);
				return (hr);
			} // end if (couldn't create event)
		} // end else (there's no generic receive event)

		hr = this->m_lpfnWSAEventSelect(sTCP, hWinSock2Event, FD_CLOSE | FD_READ);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't associate receive event with new TCP socket connected to %o!",
				1, pAddress);
			return (hr);
		} // end if (WSAEventSelect failed)
	} // end if (we're using WinSock2 functionality)

	hr = this->m_socketlist.AddSocket(sTCP, (*pAddress), TRUE, FALSE, FALSE,
										hWinSock2Event, ppLSocket);
	if (hr != S_OK)
	{
		DPL(0, "Failed to add socket to list!", 0);
		return (hr);
	} // end if (failed to bind TCP socket)


#ifdef DEBUG
	{
		int		iSize;


		ZeroMemory(&addr, sizeof (SOCKADDR_IN));

		iSize = sizeof (SOCKADDR_IN);
		if (getsockname(sTCP, (SOCKADDR*) &addr, &iSize) != 0)
		{
			hr = WSAGetLastError();

			DPL(0, "Couldn't get information on just connected stream socket!", 0);

			if (hr == S_OK)
				hr = E_FAIL;

			return (hr);
		} // end if (couldn't get information on bound socket)

		DPL(8, "Successfully connected to %o using %o.",
			2, pAddress, &addr);
	} // end if (got a zero port)
#endif // DEBUG


	return (S_OK);
} // CTNCtrlCommTCP::CreateAndConnectTCPSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::CheckAndGetEventOnSocket()"
//==================================================================================
// CTNCtrlCommTCP::CheckAndGetEventOnSocket
//----------------------------------------------------------------------------------
//
// Description: Instantly checks (as opposed to blocking while waiting) for an event
//				on the passed linked socket.  If there is an event, it is handled.
//
// Arguments:
//	PLINKEDSOCKET pLSocket	Pointer to linked socket to check.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CTNCtrlCommTCP::CheckAndHandleEventOnSocket(PLINKEDSOCKET pLSocket)
{
	HRESULT		hr;
	FD_SET		readfds;
	TIMEVAL		timeout;


	FD_ZERO(&readfds);
	FD_SET(*(pLSocket->GetSocket()), &readfds);

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	hr = select(NULL, &readfds, NULL, NULL, &timeout);
	if (hr == SOCKET_ERROR)
	{
		DPL(0, "Failed to check for event on socket!", 0);
		hr = WSAGetLastError();

#if 1 // ! BOUNDSCHECKER
		// If WinSock is not bothering to tell us what the problem is, say it's
		// generic
		if (hr == S_OK)
			hr = E_FAIL;
		return (hr);
#endif // ! BOUNDSCHECKER
	} // end if (failed to check for event on socket)

	if (hr == 0)
	{
		/*
		DPL(9, "No event for socket (%x, %o, %s).",
			3, *(pLSocket->GetSocket()), pLSocket->GetAddress(),
			(pLSocket->IsUDP() ? "UDP" : "TCP"));
		*/

		return (S_OK);
	} // end else (timed out)


	DPL(9, "Event on socket (%x, %o, %s)",
		3, *(pLSocket->GetSocket()), pLSocket->GetAddress(),
		(pLSocket->IsUDP() ? "UDP" : "TCP"));

	return (this->HandleEventOnSocket(pLSocket));
} // CTNCtrlCommTCP::CheckAndHandleEventOnSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::HandleEventOnSocket()"
//==================================================================================
// CTNCtrlCommTCP::HandleEventOnSocket
//----------------------------------------------------------------------------------
//
// Description: Handles an event on a socket.  If it's a TCP port that is listening,
//				it accepts the incoming connection.  Otherwise, the incoming data
//				is received and the control layer's handler is called.
//				IMPORTANT: The socket list lock is assumed to be held, but we will
//				drop it while calling the HandleMessage callback to prevent
//				deadlocks.  Callers must make sure this is okay.
//
// Arguments:
//	PLINKEDSOCKET pLSocket		Pointer to linked socket to handle.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CTNCtrlCommTCP::HandleEventOnSocket(PLINKEDSOCKET pLSocket)
{
	HRESULT						hr;
	PCTRLCOMM_TCP_DATAHEADER	pDataWithHeader = NULL;
	int							iReadDataIndicatedSize = 0;
	int							iReadDataBufferSize = 0;
	int							iReadDataActualSize = 0;
	SOCKADDR_IN*				pFromSockAddress = NULL;
	int							iFromSockAddressSize = sizeof (SOCKADDR_IN);

	DPL(9, "(%x)==>(%x)", 2, this, pLSocket);

	// If it's a special listen socket, assume it's a connection event
	if (pLSocket->IsTCPListen())
	{
		HANDLE	hWinSock2Event;


		hWinSock2Event = NULL;
		if (this->m_fWinSock2)
		{
			if (this->m_hAllSocketsReceiveEvent != NULL)
			{
				hWinSock2Event = this->m_hAllSocketsReceiveEvent;
			} // end if (there's a generic receive event)
			else
			{
				hWinSock2Event = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (hWinSock2Event == NULL)
				{
					hr = GetLastError();
					DPL(0, "Couldn't create socket event!", 0);
					goto DONE;
				} // end if (couldn't create event)
			} // end else (there's no generic receive event)
		} // end if (WinSock 2)

		hr = this->m_socketlist.AddNewConnectionFromSocket(pLSocket->GetSocket(),
														hWinSock2Event,
														this->m_lpfnWSAEventSelect,
														((this->m_hAllSocketsReceiveEvent == NULL) ? TRUE : FALSE),
														NULL);
		if (hr != S_OK)
		{
			// WinSock seems to be setting our events twice, so trying to handle
			// this again returns WSAEWOULDBLOCK.
			if (hr == WSAEWOULDBLOCK)
			{
				DPL(7, "WARNING: Connection appears to already have been accepted!  Continuing.", 0);
				hr = S_OK;
			} // end if (the event was already consumed)
			else
			{
				DPL(0, "AddNewConnectionFromSocket failed!", 0);

				// Get rid of the event we created, if any.
				if ((this->m_fWinSock2) &&
					(this->m_hAllSocketsReceiveEvent == NULL))
				{
					CloseHandle(hWinSock2Event);
					hWinSock2Event = NULL;
				} // end if (there's a generic receive event)
			} // end else (unexpected error)
		} // end if (AddNewConnectionFromSocket failed)

		goto DONE;
	} // end if (this is a TCP listening socket)


	// Build a buffer that we can use to peek at the data on the socket.  We
	// assume it starts with a header, which we use to determine how much
	// data is following.  This is a denial-of-service attack susceptible
	// method, but who's going to do that to a measly test app?

	iReadDataBufferSize = sizeof (CTRLCOMM_TCP_DATAHEADER);
	pDataWithHeader = (PCTRLCOMM_TCP_DATAHEADER) LocalAlloc(LPTR, iReadDataBufferSize);
	if (pDataWithHeader == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// Now peek at the data, or check for a shutdown event.  If recv
	// returns zero and it's a TCP socket it means the connection has been
	// shutdown.  That case will get handled below in the TCP specific
	// handler.
	// Unfortunately, the Alpha version of WinSock (on NT4, at least) returns 
	// WSAENOTCONN when you try a recv on a datagram socket, so we have to do
	// separate calls for UDP vs. TCP.  Oh well, that's pretty minor.

	if (pLSocket->IsUDP())
	{
		DPL(9, "recvfrom(%x, %x, %i, MSG_PEEK, NULL, NULL)",
			3, *(pLSocket->GetSocket()), pDataWithHeader,
			iReadDataBufferSize);

#ifndef _XBOX // Xbox doesn't support MSG_PEEK
		hr = recvfrom(*(pLSocket->GetSocket()), (char*) pDataWithHeader,
					iReadDataBufferSize, MSG_PEEK, NULL, NULL);
#else // ! XBOX

		// Since we don't implement MSG_PEEK, determine size of packet before calling recvfrom
		DWORD	dwNextDGramSize = 0;

		// Determine size of next datagram
		ioctlsocket(*(pLSocket->GetSocket()), FIONREAD, &dwNextDGramSize);

		// We'll be reading the whole datagram instead of just header, need to reallocate
		LocalFree(pDataWithHeader);

		// Some sort of error occured, since we were told earlier that there was data on this socket
		if(dwNextDGramSize == 0)
		{
			OutputDebugString(L"No datagram was pending, but select said socket was readable!");
			hr = E_FAIL;
			goto DONE;
		} // no datagram data was pending

		iReadDataBufferSize = dwNextDGramSize;

		// Reallocate the buffer
		pDataWithHeader = (PCTRLCOMM_TCP_DATAHEADER) LocalAlloc(LPTR, iReadDataBufferSize);

		pFromSockAddress = (SOCKADDR_IN*) LocalAlloc(LPTR, iFromSockAddressSize);
		if (pFromSockAddress == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		hr = recvfrom(*(pLSocket->GetSocket()), (char*) pDataWithHeader,
					iReadDataBufferSize, 0,(SOCKADDR*) pFromSockAddress,
					&iFromSockAddressSize);

		// We're done... go to where we process the message
		goto RECVCOMPLETE;

#endif // XBOX
	} // end if (UDP)
	else
	{
#ifndef _XBOX // Xbox doesn't support MSG_PEEK
		hr = recv(*(pLSocket->GetSocket()), (char*) pDataWithHeader,
					iReadDataBufferSize, MSG_PEEK);
#else // ! XBOX
		hr = recv(*(pLSocket->GetSocket()), (char*) pDataWithHeader,
					iReadDataBufferSize, 0);
#endif // XBOX
	} // end else (not UDP)

	iReadDataIndicatedSize = hr;
	if (hr != SOCKET_ERROR)
		hr = S_OK;
	else
	{
		hr = WSAGetLastError();

#if 1 // BOUNDSCHECKER
		// Boundschecker is stealing the LastError for some reason.  Assume it's
		// WSAEMSGSIZE.
		if (hr == S_OK)
			hr = WSAEMSGSIZE;
#else
		// If WinSock is not bothering to tell us what the problem is, say it's
		// generic
		if (hr == S_OK)
			hr = E_FAIL;
#endif // ! BOUNDSCHECKER
	} // end else (there was an error)

	if ((hr != S_OK) &&
		(! ((pLSocket->IsUDP()) && (hr == WSAEMSGSIZE))) && // didn't read all of the datagram
		(! ((! pLSocket->IsUDP()) && (hr == WSAECONNRESET))) && // other end closed stream somewhat gracefully
		(! ((! pLSocket->IsUDP()) && (hr == WSAECONNABORTED)))) // other end closed stream crappily
	{
		// WinSock seems to be setting our events twice, so trying handle this again
		// returns WSAEWOULDBLOCK.
		if (hr == WSAEWOULDBLOCK)
		{
			//DPL(0, "WARNING: Data on socket appears to already have been received!  Continuing.", 0);
			hr = S_OK;
		} // end if (the event was already consumed)
		else if (hr == WSAECONNRESET)
		{
			// Windows 2000 throws you this notification via your receive path
			// (because the send path already indicated success) if the machine
			// on the other end actively refused (read: is up, but isn't listening
			// on that socket) a datagram you sent to it.  Datagrams are unreliable,
			// we know that, we don't require that it get there!
			DPL(1, "Received notification that a UDP datagram was refused (in peek).  Who cares!?", 0);
			hr = S_OK;
			goto DONE;
		} // end if (new Windows 2000 behavior)
		else
		{
			DPL(0, "Failed to peek at data on %s socket!",
				1, (pLSocket->IsUDP() ? "UDP" : "TCP"));
		} // end else (unexpected error)
		goto DONE;
	} // end if (failed to peek at data on socket)
	/*
	else
	{
		DPL(0, "Data peek %e", 1, hr);
	} // end else (didn't fail)
	*/

	// If we got valid data, or the error was WSAEMSGSIZE (which will always happen
	// for UDP data since we only read part of the message), get a new buffer to
	// hold the entire message, and create a buffer to store the return address as
	// well.
	if ((iReadDataIndicatedSize > 0) || (hr == WSAEMSGSIZE))
	{
#ifndef _XBOX // Xbox doesn't support MSG_PEEK
		iReadDataBufferSize = pDataWithHeader->dwSize;

		// Dump the old peek buffer to store the real one
		LocalFree(pDataWithHeader);
		pDataWithHeader = NULL;

		pDataWithHeader = (PCTRLCOMM_TCP_DATAHEADER) LocalAlloc(LPTR, iReadDataBufferSize);
		if (pDataWithHeader == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		pFromSockAddress = (SOCKADDR_IN*) LocalAlloc(LPTR, iFromSockAddressSize);
		if (pFromSockAddress == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
#else // ! XBOX
		PCTRLCOMM_TCP_DATAHEADER	pTempData = NULL;
		// Since we don't support MSG_PEEK, we'll have to copy the part of the buffer
		// we already read back into the final buffer.

		// Possibly fatal if we couldn't read the whole header... shouldn't happen
		if((iReadDataIndicatedSize > 0) && (iReadDataIndicatedSize < sizeof (CTRLCOMM_TCP_DATAHEADER)))
		{
			OutputDebugString(L"We didn't read the whole header!\r\n");
			hr = E_FAIL;
			goto DONE;
		}

		// Get the size of the whole packet
		iReadDataBufferSize = pDataWithHeader->dwSize;

		// Save the buffer that we already read
		pTempData = pDataWithHeader;
		pDataWithHeader = NULL;

		// Create the new buffer
		pDataWithHeader = (PCTRLCOMM_TCP_DATAHEADER) LocalAlloc(LPTR, iReadDataBufferSize);
		if (pDataWithHeader == NULL)
		{
			hr = E_OUTOFMEMORY;
			LocalFree(pTempData);
			goto DONE;
		} // end if (couldn't allocate memory)

		// Copy the old peek buffer to the new real one
		memcpy(pDataWithHeader, pTempData, sizeof (CTRLCOMM_TCP_DATAHEADER));

		// Dump the old peek buffer
		LocalFree(pTempData);

		// Create the buffer for the sock address
		pFromSockAddress = (SOCKADDR_IN*) LocalAlloc(LPTR, iFromSockAddressSize);
		if (pFromSockAddress == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

#endif // XBOX
	} // end if (we actually read data)

	if (pLSocket->IsUDP())
	{
#ifndef _XBOX // Xbox doesn't support MSG_PEEK
		// Now pull it off the queue

		DPL(9, "recvfrom(%x, %x, %i, 0, %x, %x)",
			5, *(pLSocket->GetSocket()), pDataWithHeader,
			iReadDataBufferSize, pFromSockAddress, iFromSockAddressSize);

		hr = recvfrom(*(pLSocket->GetSocket()), (char*) pDataWithHeader,
						iReadDataBufferSize, 0, (SOCKADDR*) pFromSockAddress,
						&iFromSockAddressSize);
#else // ! XBOX
RECVCOMPLETE:
#endif // XBOX

		iReadDataActualSize = hr;

		if (hr != SOCKET_ERROR)
			hr = S_OK;
		else
		{
			hr = WSAGetLastError();

			if (hr == WSAECONNRESET)
			{
				DPL(1, "Received notification that a UDP datagram was refused (in remove).  Who cares!?", 0);
				hr = S_OK;
				goto DONE;
			} // end if (got CONNRESET)

#if 1 // BOUNDSCHECKER
			// Boundschecker is stealing the LastError for some reason.  Assume it's
			// WSAEMSGSIZE.
			if (hr == S_OK)
				hr = WSAEMSGSIZE;
#else
			// If WinSock is not bothering to tell us what the problem is, say it's
			// generic
			if (hr == S_OK)
				hr = E_FAIL;
#endif // ! BOUNDSCHECKER
		} // end else (there was an error)

		if (hr != S_OK)
		{
			DPL(0, "Failed to retrieve data from datagram socket!", 0);
			goto DONE;
		} // end if (failed to peek at data on socket)

		//BUGBUG we may want to have a lower level reject mechanism (one that
		//		 throws out broadcasts from ourselves, but you have to be
		//		 careful in cases where we have multiple instances of this app
		//		 on the same machine.  In any case, it's not entirely necessary
		//		 because we ignore ENUMs at the next higher up level.
		/*
			// Check if it's just a reflection back to us (from a broadcast)
			if ((fromSockAddress.sin_addr.S_un.S_addr == lpAddress_SockAddr->socketAddress.sin_addr.S_un.S_addr) &&
				(fromSockAddress.sin_port == lpAddress_SockAddr->socketAddress.sin_port))
			{
				//DPL(0, "Message is from our address (%o), ignoring.",
				//	1, &fromAddress);
				hr = S_OK;
				goto DONE;
			} // end if (it's from us)
		*/
	} // end if (a datagram socket)
	else
	{
		if ((iReadDataIndicatedSize == 0) ||
			(hr == WSAECONNRESET) ||
			(hr == WSAECONNABORTED))
		{
			DPL(1, "Received shutdown socket event for connection to %o.",
				1, pLSocket->GetAddress());

			hr = pLSocket->CloseSocket();
			if (hr != S_OK)
			{
				DPL(0, "Couldn't close the socket!", 0);
				goto DONE;
			} // end if (failed closing the socket)

			if (pLSocket->m_pCommData == NULL)
			{
				// Pull it off the list
				hr = this->m_socketlist.RemoveFirstReference(pLSocket);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't remove socket from list!", 0);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't find socket in list)

				if (pLSocket->m_dwRefCount != 0)
				{
					DPL(0, "WARNING: Still an outstanding reference to linked socket %x!",
						1, pLSocket);
				} // end if (not last reference to object)
				else
				{
					delete (pLSocket);
					pLSocket = NULL;
				} // end else (that was the last reference)

				if (this->m_fWinSock2)
				{
					if (! SetEvent(this->m_socketlist.m_hListChangedEvent))
					{
						hr = GetLastError();
						DPL(0, "Couldn't set socket list changed event (%x)!",
							1, this->m_socketlist.m_hListChangedEvent);
						goto DONE;
					} // end if (couldn't notify thread that the list changed)
				} // end if (we're using WinSock2)
			} // end if (this socket isn't bound to anything yet)
			else
			{
				DPL(6, "Not removing socket from list because it is still bound.", 0);
			} // end else (the socket is bound)

			hr = S_OK;
			goto DONE;
		} // end if (it's a shutdown socket event)

#ifndef _XBOX // Xbox doesn't support MSG_PEEK
		// Now pull it off the queue
		hr = recv(*(pLSocket->GetSocket()), (char*) pDataWithHeader,
					iReadDataBufferSize, 0);
#else // ! XBOX
		// Read the second part of the packet (the part after the size field) into remaining part of the buffer
		hr = recv(*(pLSocket->GetSocket()), (char*) pDataWithHeader + sizeof(CTRLCOMM_TCP_DATAHEADER),
					iReadDataBufferSize - sizeof(CTRLCOMM_TCP_DATAHEADER), 0);
#endif // XBOX
		if (hr != SOCKET_ERROR)
		{

#ifndef _XBOX // Xbox doesn't support MSG_PEEK
			iReadDataActualSize = hr;
#else // ! XBOX
		// If we didn't fail, then adjust the number of bytes read to make it look like we read this packet
		// with one call instead of two.
		iReadDataActualSize = hr + sizeof(CTRLCOMM_TCP_DATAHEADER);
#endif // XBOX

#ifdef DEBUG
			if (iReadDataActualSize != iReadDataBufferSize)
			{
				DPL(0, "Didn't read blob same size as buffer (%i != %i)!  DEBUGBREAK()-ing.",
					2, iReadDataActualSize, iReadDataBufferSize);
				DEBUGBREAK();
			} // end if (didn't read expected)
#endif // DEBUG

			hr = S_OK;
		} // end if (no error)
		else
		{
			hr = WSAGetLastError();

			iReadDataActualSize = 0;

			#if 1 // ! BOUNDSCHECKER
			// If WinSock is not bothering to tell us what the problem is, say it's
			// generic
			if (hr == S_OK)
				hr = E_FAIL;
			#endif // ! BOUNDSCHECKER
		} // end else (there was an error)

		if (hr != S_OK)
		{
			DPL(0, "Failed to retrieve data from stream socket!", 0);
			goto DONE;
		} // end if (failed to peek at data on socket)

		// Copy the from address from the socket in the list
		CopyMemory(pFromSockAddress, pLSocket->GetAddress(), iFromSockAddressSize);
	} // end else (a stream socket)


	DPL(9, "Received %u bytes of data from %o", 2, iReadDataActualSize,
		pFromSockAddress);


	// Drop the socket list lock to prevent possible deadlocks
	this->m_socketlist.LeaveCritSection();

	//BUGBUG int to DWORD conversion
	hr = this->m_pControlObj->HandleMessage((LPVOID*) &pFromSockAddress, iFromSockAddressSize,
					((LPBYTE) pDataWithHeader) + sizeof (CTRLCOMM_TCP_DATAHEADER),
					iReadDataActualSize - sizeof (CTRLCOMM_TCP_DATAHEADER));

	// Retake the lock before continuing
	this->m_socketlist.EnterCritSection();

	if (hr != S_OK)
	{
		DPL(0, "Control object message handler failed!", 0);
		// Ignoring errors
		this->m_pControlObj->Log(TNLST_CRITICAL,
								"Control object message handler failed!  %e", 1, hr);
		goto DONE;
	} // end if (it handle message failed)


DONE:

	// This could be reset to NULL even if the memory wasn't explicitly freed here
	// if HandleMessage() turned around and used it in a send.  This is not a great
	// way to do this but it reduces CopyMemorys.
	if (pFromSockAddress != NULL)
	{
		LocalFree(pFromSockAddress);
		pFromSockAddress = NULL;
	} // end if (allocated a from address)

	if (pDataWithHeader != NULL)
	{
		LocalFree(pDataWithHeader);
		pFromSockAddress = NULL;
	} // end if (allocated a data with header buffer)


	DPL(9, "(%x)<== %e", 2, this, hr);

	return (hr);
} // CTNCtrlCommTCP::HandleEventOnSocket
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::InternalCloseReachCheck()"
//==================================================================================
// CTNCtrlCommTCP::InternalCloseReachCheck
//----------------------------------------------------------------------------------
//
// Description: Cleans up the given reach check object.
//
// Arguments:
//	PTNREACHCHECK pReachCheck	Reach check being closed.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNCtrlCommTCP::InternalCloseReachCheck(PTNREACHCHECK pReachCheck)
{
	HRESULT			hr;
	BOOL			fInCritSect = FALSE;
	PLINKEDSOCKET	pLSocket = NULL;



	DPL(9, "==> (%x)", 1, pReachCheck);


	if (pReachCheck->m_pvCommData == NULL)
	{
		DPL(2, "No socket used for reach check ID %u.",
			1, pReachCheck->m_dwID);
	} // end if (no socket)
	else
	{
		pLSocket = (PLINKEDSOCKET) pReachCheck->m_pvCommData;
		pReachCheck->m_pvCommData = NULL;


		// Remove all items still remaining to be sent.
		hr = this->m_sendqueue.RemoveAllItemsTo(pLSocket->GetAddress(),
												sizeof (SOCKADDR_IN));
		if (hr != S_OK)
		{
			DPL(0, "Couldn't remove all remaining items to be sent to %o!",
				1, pLSocket->GetAddress());
			goto DONE;
		} // end if (couldn't remove all items)


		this->m_socketlist.EnterCritSection();
		fInCritSect = TRUE;


		// If the socket only existed for this reach check, remove it.
		if (pLSocket->m_dwFoundPriorToReachCheck == 0)
		{
			DPL(2, "Removing %s socket %x used exclusively for reach check (current ID %u).",
				3, (pLSocket->IsUDP() ? "UDP" : "TCP"), *(pLSocket->GetSocket()),
				pReachCheck->m_dwID);

			hr = this->m_socketlist.RemoveFirstReference(pLSocket);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't remove original socket from list!", 0);
				goto DONE;
			} // end if (couldn't remove reference)

			pLSocket->m_dwRefCount--; // delete the comm data reference
			if (pLSocket->m_dwRefCount != 0)
			{
				DPL(0, "Can't delete socket %x (refcount is %u)!?",
					2, pLSocket, pLSocket->m_dwRefCount);

				DEBUGBREAK();
			} // end if (can't delete socket)

			// Deleting it will cause it to shut down.
			delete (pLSocket);
			pLSocket = NULL;
		} // end if (there's only one ref left)
		else
		{
			pLSocket->m_dwFoundPriorToReachCheck--;

			pLSocket->m_dwRefCount--; // delete the comm data reference
			if (pLSocket->m_dwRefCount == 0)
			{
				DPL(0, "Deleting socket %x!?", 1, pLSocket);

				DEBUGBREAK();

				delete (pLSocket);
			} // end if (last reference)
			else
			{
				DPL(0, "Not deleting %s socket %x, (object %x), found count is %u, refcount is %u, reach check ID is %u.",
					6, (pLSocket->IsUDP() ? "UDP" : "TCP"),
					*(pLSocket->GetSocket()),
					pLSocket,
					pLSocket->m_dwFoundPriorToReachCheck,
					pLSocket->m_dwRefCount,
					pReachCheck->m_dwID);
			} // end else (not last reference)

			pLSocket = NULL;
		} // end else (there's more refs left)


		fInCritSect = FALSE;
		this->m_socketlist.LeaveCritSection();
	} // end else (there is a socket)


	hr = this->m_reachchecks.RemoveFirstReference(pReachCheck);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't remove reach check from list!", 0);
		//goto DONE;
	} // end if (couldn't remove reference)

	if (pReachCheck->m_dwRefCount == 0)
	{
		DPL(0, "Deleting search %x because reference count is 0!?  DEBUGBREAK()-ing.",
			1, pReachCheck);

		DEBUGBREAK();

		delete (pReachCheck);
		pReachCheck = NULL;
	} // end if (should delete search now)


DONE:

	if (fInCritSect)
	{
		fInCritSect = FALSE;
		this->m_socketlist.LeaveCritSection();
	} // end if (in critical section)

	DPL(9, "<== %e", 1, hr);

	return (hr);
} // CTNCtrlCommTCP::InternalCloseReachCheck
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::SendUDPDataTo()"
//==================================================================================
// CTNCtrlCommTCP::SendUDPDataTo
//----------------------------------------------------------------------------------
//
// Description: Sends the passed in item via UDP to the passed in address.
//
// Arguments:
//	SOCKADDR_IN* pAddress	Pointer to address to use.
//	PVOID pvData			Pointer to data to send.
//	DWORD dwDataSize		Size of data to send.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CTNCtrlCommTCP::SendUDPDataTo(SOCKADDR_IN* pAddress, PVOID pvData,
										DWORD dwDataSize)
{
	HRESULT						hr = S_OK;
	PLINKEDSOCKET				pLSocket = NULL;
	PCTRLCOMM_TCP_DATAHEADER	pWrappedData = NULL;
	DWORD						dwWrappedDataSize = 0;

	
	DPL(8, "Sending %u bytes of data at %x to: %o", 3, dwDataSize, pvData, pAddress);


	// Look for a UDP socket using the given port.
	pLSocket = this->m_socketlist.GetLSocketByAddress(pAddress, FALSE);
	if (pLSocket == NULL)
	{
		SOCKADDR_IN*		pLocalAddress;


		// We didn't find one, so try using any old UDP socket.
		pLSocket = this->m_socketlist.GetLSocketByAddress(NULL, FALSE);
		if (pLSocket == NULL)
		{
			DPL(0, "Couldn't get any UDP sockets!", 0);
			return (E_FAIL);
		} // end if (a UDP socket wasn't found)

		pLocalAddress = pLSocket->GetAddress();

		DPL(8, "Sending with local socket on port %u (no UDP sockets on port %u).",
			2, BYTEREVERSEWORD(pLocalAddress->sin_port),
			BYTEREVERSEWORD(pAddress->sin_port));
	} // end if (a UDP socket wasn't using that port)


	// Tack on our header
	dwWrappedDataSize = sizeof (CTRLCOMM_TCP_DATAHEADER) +
						dwDataSize;
	pWrappedData = (PCTRLCOMM_TCP_DATAHEADER) LocalAlloc(LPTR, dwWrappedDataSize);
	if (pWrappedData == NULL)
		return (E_OUTOFMEMORY);

	pWrappedData->dwSize = dwWrappedDataSize;
	CopyMemory(((LPBYTE) pWrappedData) + sizeof (CTRLCOMM_TCP_DATAHEADER),
			pvData, dwDataSize);

	// Now send the whole shebang
	hr = sendto(*(pLSocket->GetSocket()), (char*) pWrappedData, dwWrappedDataSize,
				0, (SOCKADDR*) pAddress, sizeof (SOCKADDR_IN));
	if (hr == SOCKET_ERROR)
	{
		hr = WSAGetLastError();

		DPL(0, "Failed to send datagram to address (%o)!", 1, pAddress);

#if 1 // ! BOUNDSCHECKER
		// If WinSock is not bothering to tell us what the problem is, say it's generic
		if (hr == S_OK)
			hr = E_FAIL;
#endif // ! BOUNDSCHECKER
	} // end if (failed to send data)
	else
		hr = S_OK;

	if (pWrappedData != NULL)
	{
		LocalFree(pWrappedData);
		pWrappedData = NULL;
	} // end if (allocated memory)

	return (hr);
} // CTNCtrlCommTCP::SendUDPDataTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNCtrlCommTCP::SendTCPDataTo()"
//==================================================================================
// CTNCtrlCommTCP::SendTCPDataTo
//----------------------------------------------------------------------------------
//
// Description: Sends the passed in item via TCP to the passed in address.  It uses
//				a connection to the person if it already exists, or if one doesn't
//				but we're not a testmaster, it tries to create a connection.
//
// Arguments:
//	SOCKADDR_IN* pAddress	Pointer to address to use.
//	PVOID pvData			Pointer to data to send.
//	DWORD dwDataSize		Size of data to send.
//
// Returns: S_OK if succeeded, otherwise the failure code.
//==================================================================================
HRESULT CTNCtrlCommTCP::SendTCPDataTo(SOCKADDR_IN* pAddress, PVOID pvData,
										DWORD dwDataSize)
{
	HRESULT						hr = S_OK;
	PLINKEDSOCKET				pLSocket = NULL;
	PCTRLCOMM_TCP_DATAHEADER	pWrappedData = NULL;
	DWORD						dwWrappedDataSize = 0;
	DWORD						dwTemp;

	
	DPL(8, "Sending %u bytes of data at %x to: %o", 3, dwDataSize, pvData, pAddress);

	pLSocket = this->m_socketlist.GetLSocketByAddress(pAddress, TRUE);
	if (pLSocket == NULL)
	{
		//DPL(0, "No connection to that machine already exists.", 0);


#pragma TODO(vanceo, "Reconnect protection?")


		// Create a new socket and try to establish a new connection.

		hr = this->CreateAndConnectTCPSocket(pAddress, &pLSocket);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't create and connect TCP socket to %o!", 1, pAddress);
			return (hr);
		} // end if (couldn't create socket)
	} // end if (the connection doesn't exist)


	// Tack on our header
	dwWrappedDataSize = sizeof (CTRLCOMM_TCP_DATAHEADER) +
						dwDataSize;
	pWrappedData = (PCTRLCOMM_TCP_DATAHEADER) LocalAlloc(LPTR, dwWrappedDataSize);
	if (pWrappedData == NULL)
		return (E_OUTOFMEMORY);

	pWrappedData->dwSize = dwWrappedDataSize;
	CopyMemory(((LPBYTE) pWrappedData) + sizeof (CTRLCOMM_TCP_DATAHEADER),
			pvData, dwDataSize);

	// BUGBUG int and DWORD conversion
	dwTemp = send(*(pLSocket->GetSocket()), (char*) pWrappedData, dwWrappedDataSize, 0);
	if (dwTemp == SOCKET_ERROR)
	{
		hr = WSAGetLastError();
		DPL(0, "Send failed!", 0);
	} // end if (send failed)
	else
	{
		if (dwTemp != dwWrappedDataSize)
			DPL(0, "Sent (%u) different amount of data than expected (%u)",
				2, dwTemp, dwWrappedDataSize);
		hr = S_OK;
	} // end else (send succeeded)

	if (pWrappedData != NULL)
	{
		LocalFree(pWrappedData);
		pWrappedData = NULL;
	} // end if (allocated memory)

	return (hr);
} // CTNCtrlCommTCP::SendTCPDataTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"WSSendThreadProc()"
//==================================================================================
// WSSendThreadProc
//----------------------------------------------------------------------------------
//
// Description: Waits for data to send, and then sends it.
//
// Arguments:
//	LPVOID lpvParameter		Thread data.  Cast to a CTNCtrlCommTCP object.
//
// Returns: 0 if all goes well.
//==================================================================================
DWORD WINAPI WSSendThreadProc(LPVOID lpvParameter)
{
	HRESULT			hr;
	PTNCTRLCOMMTCP	pThisObj = (PTNCTRLCOMMTCP) lpvParameter;
	HANDLE			ahWaitObjects[3];


	DPL(1, "Starting up.", 0);

	if (pThisObj->m_hKillSendThreadEvent == NULL)
	{
		DPL(0, "Kill thread event doesn't exist!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (event doesn't exist)

	if (pThisObj->m_sendqueue.m_hSendDataEvent == NULL)
	{
		DPL(0, "Send data event doesn't exist!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (event doesn't exist)

	ahWaitObjects[0] = pThisObj->m_hKillSendThreadEvent;
	ahWaitObjects[1] = pThisObj->m_sendqueue.m_hSendDataEvent;
	ahWaitObjects[2] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array

	do
	{
		switch (WaitForMultipleObjects(2, ahWaitObjects, FALSE, INFINITE))
		{
			case WAIT_OBJECT_0:
				// Time to die
				hr = S_OK;
				goto DONE;
			  break;

			case WAIT_OBJECT_0 + 1:
				// There should be something in the queue

				pThisObj->m_sendqueue.EnterCritSection();

				if (pThisObj->m_sendqueue.Count() == 0)
				{
					DPL(0, "WARNING: Send queue event fired but there's nothing in the queue.", 0);
					pThisObj->m_sendqueue.LeaveCritSection();
				} // end if (there's nothing in the queue)
				else
				{
					hr = pThisObj->SendFirstData();
					if (hr != S_OK)
					{
						DPL(0, "WARNING: Control layer sending data failed!  %e", 1, hr);

						// Ignoring errors
						pThisObj->m_pControlObj->Log(TNLST_CRITICAL,
														"WARNING: Control layer sending data failed!  %e",
														1, hr);

						// but continue on...
						hr = S_OK;
					} // end if (sending data failed)

					// If there are still items left in the queue, refire our event
					if (pThisObj->m_sendqueue.Count() > 0)
					{
						if (! SetEvent(pThisObj->m_sendqueue.m_hSendDataEvent))
						{
							hr = GetLastError();
							DPL(0, "Couldn't refire send queue event (%x)!",
								1, pThisObj->m_sendqueue.m_hSendDataEvent);
							pThisObj->m_sendqueue.LeaveCritSection();
							goto DONE;
						} // end if (couldn't set event)
					} // end if (there are still items in the send queue)

					pThisObj->m_sendqueue.LeaveCritSection();
				} // end else (there's something in the send queue)
			  break;

			default:
				DPL(0, "Got unexpected return from WaitForMultipleObjects!", 0);
				hr = E_FAIL;
			  break;
		} // end while (we got new data to send)
	} // end do (while it's not time to die)
	while (hr == S_OK);

DONE:

	if (hr != S_OK)
	{
		DPL(0, "WinSock Send thread aborting!  DEBUGBREAK()-ing.  %e",
			1, hr);

		// Ignoring errors
		pThisObj->m_pControlObj->Log(TNLST_CRITICAL,
									"WinSock Send thread aborting!  DEBUGBREAK()-ing.  %e",
									1, hr);

		DEBUGBREAK();
	} // end if (quitting with a failure)

	DPL(1, "Exiting.", 0);

	return (hr);
} // WSSendThreadProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"WS1ReceiveThreadProc()"
//==================================================================================
// WS1ReceiveThreadProc
//----------------------------------------------------------------------------------
//
// Description: Waits for data to come in on the various sockets, then passes it up.
//
// Arguments:
//	LPVOID lpvParameter		Thread data.  Cast to a CTNCtrlCommTCP object.
//
// Returns: 0 if all goes well.
//==================================================================================
DWORD WINAPI WS1ReceiveThreadProc(LPVOID lpvParameter)
{
	HRESULT			hr = S_OK;
	PTNCTRLCOMMTCP	pThisObj = (PTNCTRLCOMMTCP) lpvParameter;

	DPL(1, "Starting up.", 0);

	if (pThisObj->m_hKillReceiveThreadEvent == NULL)
	{
		DPL(0, "Kill thread event doesn't exist!", 0);
		hr = E_FAIL;
		goto DONE;
	} // end if (event doesn't exist)

	// Without WinSock2 functionality, there's no way to wait for a kill thread event
	// while checking for a socket event, so we use this CPU intensive check.  Ugh.
	while (WaitForSingleObject(pThisObj->m_hKillReceiveThreadEvent, 0) == WAIT_TIMEOUT)
	{
		hr = pThisObj->CheckForReceiveData();
		if (hr != S_OK)
		{
			DPL(0, "CheckForReceiveData failed!", 0);
			goto DONE;
		} // end if (check for receive data failed)
	} // end while (we're not being told to stop)

DONE:

	if (hr != S_OK)
	{
		DPL(0, "WinSock WS1Receive thread aborting!  DEBUGBREAK()-ing.  %e",
			1, hr);

		// Ignoring errors
		pThisObj->m_pControlObj->Log(TNLST_CRITICAL,
									"WinSock WS1Receive thread aborting!  DEBUGBREAK()-ing.  %e",
									1, hr);

		DEBUGBREAK();
	} // end if (quitting with a failure)

	DPL(1, "Exiting.", 0);
	return (hr);
} // WS1ReceiveThreadProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"WS2ReceiveThreadProc()"
//==================================================================================
// WS2ReceiveThreadProc
//----------------------------------------------------------------------------------
//
// Description: Waits for data to come in on the various sockets, then passes it up.
//
// Arguments:
//	LPVOID lpvParameter		Thread data.  Cast to a CTNCtrlCommTCP object.
//
// Returns: 0 if all goes well.
//==================================================================================
DWORD WINAPI WS2ReceiveThreadProc(LPVOID lpvParameter)
{
	HRESULT			hr = S_OK;
	PTNCTRLCOMMTCP	pThisObj = (PTNCTRLCOMMTCP) lpvParameter;
	HANDLE			ahWaitObjects[3];


	DPL(1, "Starting up.", 0);

	if (pThisObj->m_hKillReceiveThreadEvent == NULL)
	{
		DPL(0, "Kill thread event doesn't exist!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (event doesn't exist)

	if (pThisObj->m_hAllSocketsReceiveEvent == NULL)
	{
		DPL(0, "All sockets recieve event doesn't exist!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (event doesn't exist)

	ahWaitObjects[0] = pThisObj->m_hKillReceiveThreadEvent;
	ahWaitObjects[1] = pThisObj->m_hAllSocketsReceiveEvent;
	ahWaitObjects[2] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array

	// With WinSock2 functionality we can wait until we either get a socket event
	// or a shutdown event, and not use CPU time in between.

	do
	{
		hr = WaitForMultipleObjects(2, ahWaitObjects, FALSE, INFINITE);
		switch (hr)
		{
			case WAIT_OBJECT_0:
				// We're being told to die.

				hr = S_OK;
				goto DONE;
			  break;

			case WAIT_OBJECT_0 + 1:
				// We've gotten a receive event.

				hr = pThisObj->CheckForReceiveData();
				if (hr != S_OK)
				{
					DPL(0, "CheckForReceiveData failed!", 0);
					goto DONE;
				} // end if (check for receive data failed)
			  break;

			default:
				// Something was goofy.

				DPL(0, "Got unexpected return from WaitForMultiple objects!", 0);
				goto DONE;
			  break;
		} // end switch (on wait result)
	} // end do
	while (TRUE); // loop until we are told to quit or get an error


DONE:

	if (hr != S_OK)
	{
		DPL(0, "WinSock WS2Receive thread aborting!  DEBUGBREAK()-ing.  %e",
			1, hr);

		// Ignoring errors
		pThisObj->m_pControlObj->Log(TNLST_CRITICAL,
									"WinSock WS2Receive thread aborting!  DEBUGBREAK()-ing.  %e",
									1, hr);

		DEBUGBREAK();
	} // end if (quitting with a failure)

	DPL(1, "Exiting.", 0);

	return (hr);
} // WS2ReceiveThreadProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"WS2OneToOneReceiveThreadProc()"
//==================================================================================
// WS2OneToOneReceiveThreadProc
//----------------------------------------------------------------------------------
//
// Description: Waits for data to come in on the various sockets, then passes it up.
//
// Arguments:
//	LPVOID lpvParameter		Thread data.  Cast to a CTNCtrlCommTCP object.
//
// Returns: 0 if all goes well.
//==================================================================================
DWORD WINAPI WS2OneToOneReceiveThreadProc(LPVOID lpvParameter)
{
	HRESULT				hr = S_OK;
	PTNCTRLCOMMTCP		pThisObj = (PTNCTRLCOMMTCP) lpvParameter;
	BOOL				fRebuildArray = TRUE;
	BOOL				fInCritSect = FALSE;
	HANDLE*				pahWaitObjects = NULL;
	int					iNumSockets = 0;
	int					i;
	PLINKEDSOCKET		pLSocket = NULL;


	DPL(1, "Starting up.", 0);

	if (pThisObj->m_hKillReceiveThreadEvent == NULL)
	{
		DPL(0, "Kill thread event doesn't exist!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (event doesn't exist)

	if (pThisObj->m_socketlist.m_hListChangedEvent == NULL)
	{
		DPL(0, "List changed event doesn't exist!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (event doesn't exist)

	// With WinSock2 functionality we can wait until we either get a socket event
	// or a shutdown event, and not use CPU time in between.

	do
	{
		if (fRebuildArray)
		{
			//DPL(0, "Rebuilding socket list array.", 0);

			fRebuildArray = FALSE;

			// If the array already existed, clear it out (closing references).
			if (pahWaitObjects != NULL)
			{
				for(i = 2; i < iNumSockets + 2; i++)
				{
					// Close our reference to the item, ignoring errors
					CloseHandle(pahWaitObjects[i]);
					pahWaitObjects[i] = NULL;
				} // end for (each item already in the array)

				LocalFree(pahWaitObjects);
				pahWaitObjects = NULL;
			} // end if (there was an array already)

			pThisObj->m_socketlist.EnterCritSection();
			fInCritSect = TRUE;

			// Reset the event just in case it got fired again or what not to
			// prevent needless rebuilding of the array.
			if (! ResetEvent(pThisObj->m_socketlist.m_hListChangedEvent))
			{
				hr = GetLastError();
				DPL(0, "Couldn't reset the list changed event (%x)!",
					1, pThisObj->m_socketlist.m_hListChangedEvent);
				goto DONE;
			} // end if (couldn't reset the event)

			iNumSockets = pThisObj->m_socketlist.Count();

			pahWaitObjects = (HANDLE*) LocalAlloc(LPTR, ((iNumSockets + 3) * sizeof (HANDLE)));
			if (pahWaitObjects == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)

			pahWaitObjects[0] = pThisObj->m_hKillReceiveThreadEvent;
			pahWaitObjects[1] = pThisObj->m_socketlist.m_hListChangedEvent;

			for(i = 0; i < iNumSockets; i++)
			{
				pLSocket = (PLINKEDSOCKET) pThisObj->m_socketlist.GetItem(i);
				if (pLSocket == NULL)
				{
					DPL(0, "Couldn't get socket %i!", 1, i);
					hr = E_FAIL;
					goto DONE;
				} // end if (couldn't get item)

				// We need a handle to the object, but we can't just use the
				// other handle because another thread might close it and we'd
				// have a bogus handle.
				if (! DuplicateHandle(GetCurrentProcess(), pLSocket->GetEvent(),
										GetCurrentProcess(), &(pahWaitObjects[i + 2]),
										0, FALSE, DUPLICATE_SAME_ACCESS))
				{
					hr = GetLastError();
					DPL(0, "Couldn't duplicate player event handle %i (%x)!",
						2, i, pLSocket->GetEvent());
					goto DONE;
				} // end if (couldn't duplicate handle)
			} // end for (each socket in the list)

			pahWaitObjects[iNumSockets + 2] = (HANDLE) 0xFFFFFFFF; // bug in Win95 Gold, this should force it to rewalk array
			
			fInCritSect = FALSE;
			pThisObj->m_socketlist.LeaveCritSection();
		} // end if (should rebuild array)


		DPL(9, "++ Waiting for event on %i sockets.", 1, iNumSockets);

		hr = WaitForMultipleObjects(iNumSockets + 2, pahWaitObjects, FALSE, INFINITE);

		DPL(9, "-- Got event %x", 1, hr);

		switch (hr)
		{
			case WAIT_OBJECT_0:
				// We're being told to die.

				hr = S_OK;
				goto DONE;
			  break;

			case WAIT_OBJECT_0 + 1:
				// We've gotten a list changed event.

				fRebuildArray = TRUE;
				hr = S_OK;
			  break;

			case WAIT_FAILED:
				// Something was goofy.

				hr = GetLastError();
				DPL(0, "Wait failed!", 0);
				goto DONE;
			  break;

			default:
				i = hr - WAIT_OBJECT_0 - 2; // get the index of the item that caused this

				pThisObj->m_socketlist.EnterCritSection();
				fInCritSect = TRUE;

				if (iNumSockets != pThisObj->m_socketlist.Count())
				{
					DPL(0, "List appears to have changed (%i != %i) but we haven't been told yet, forcing array rebuild and reactivating item %i!",
						3, iNumSockets, pThisObj->m_socketlist.Count(), i);
					fRebuildArray = TRUE;

					// Otherwise, refire the event that triggered this so we can retry,
					// if we can.
					if ((i >= 0) && (i < iNumSockets))
					{
						if (! SetEvent(pahWaitObjects[i + 2]))
						{
							hr = GetLastError();
							DPL(0, "WARNING: Couldn't refire event %x!  %e",
								2, pahWaitObjects[i + 2], hr);
						} // end if (couldn't refire event)
					} // end if (have a valid index to try refiring)
					else
					{
						DPL(0, "WARNING: Item %i was not a valid object before, not trying to refire anything!",
							1, i);
					} // end else (no object to refire)

					fInCritSect = FALSE;
					pThisObj->m_socketlist.LeaveCritSection();
					break;
				} // end if (list changed but we didn't see it yet)

				// Check to make sure it was a valid event
				if ((i < 0) || (i >= iNumSockets))
				{
					DPL(0, "Got unexpected return from WaitForMultipleObjects!", 0);
					hr = E_FAIL;
					goto DONE;
				} // end if (got wacky return)

				// Otherwise, grab the object that signalled
				pLSocket = (PLINKEDSOCKET) pThisObj->m_socketlist.GetItem(i);
				if (pLSocket == NULL)
				{
					//BUGBUG figure out why it's actually doing this
					/*
					DPL(0, "Couldn't get activated object item %i!", 1, i);
					hr = E_FAIL;
					goto DONE;
					*/
					DPL(0, "WARNING: Couldn't get activated object item %i, forcing array rebuild!", 1, i);
					fRebuildArray = TRUE;

					fInCritSect = FALSE;
					pThisObj->m_socketlist.LeaveCritSection();
					break;
				} // end if (couldn't get signalled socket)


				DPL(8, "Got event on socket %i", 1, i);

				if (*(pLSocket->GetSocket()) == INVALID_SOCKET)
				{
					DPL(0, "Skipping socket %i because it has been shutdown.", 1, i);
				} // end if (the socket is gone)
				else
				{
					// Note that this may drop the socket list lock.  That should be
					// okay since we don't use the items any more in this loop, and
					// if the user made a call that changed the array, the event
					// would be set, and we'll rebuild like usual.
					hr = pThisObj->HandleEventOnSocket(pLSocket);
					if (hr != S_OK)
					{
						DPL(0, "Failed to handle event on socket %i!", 1, i);
						goto DONE;
					} // end if (failed to check for socket event)
				} // end else (it's a valid socket)

				pLSocket = NULL; // for BoundsChecker's sake

				fInCritSect = FALSE;
				pThisObj->m_socketlist.LeaveCritSection();
			  break;
		} // end switch (on wait result)
	} // end do
	while (TRUE); // loop until we are told to quit or get an error

DONE:

	if (fInCritSect)
		pThisObj->m_socketlist.LeaveCritSection();

	if (pahWaitObjects != NULL)
	{
		for(i = 2; i < iNumSockets + 2; i++)
		{
			// Close our reference to the item, ignoring errors
			CloseHandle(pahWaitObjects[i]);
			pahWaitObjects[i] = NULL;
		} // end for (each item already in the array)

		LocalFree(pahWaitObjects);
		pahWaitObjects = NULL;
	} // end if (we had an object array)


	if (hr != S_OK)
	{
		DPL(0, "WinSock WS2OneToOneReceive thread aborting!  DEBUGBREAK()-ing.  %e",
			1, hr);

		// Ignoring errors
		pThisObj->m_pControlObj->Log(TNLST_CRITICAL,
									"WinSock WS2OneToOneReceive thread aborting!  DEBUGBREAK()-ing.  %e",
									1, hr);

		DEBUGBREAK();
	} // end if (quitting with a failure)

	DPL(1, "Exiting.", 0);

	return (hr);
} // WS2OneToOneReceiveThreadProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
