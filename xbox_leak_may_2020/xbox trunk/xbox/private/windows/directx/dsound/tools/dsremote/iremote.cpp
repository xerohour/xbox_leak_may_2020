/***************************************************************************
 *
 *  Copyright (C) 1/8/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       iremote.cpp
 *  Content:    IRemote interface impelementation.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/8/2002   dereks  Created.
 *
 ****************************************************************************/

#include "dsremi.h"

using namespace DSREMOTE;

#ifdef DEBUG

static HRESULT __WSATranslateError(DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, LPCSTR pszCalled)
{
    DWORD                   dwResult    = WSAGetLastError();
    HRESULT                 hr;

    CDebug::SetContext(DPF_FLAGS_DEFAULT, dwLevel, pszFile, nLine, pszFunction, DPF_LIBRARY);

    if(dwResult)
    {
        CDebug::Print("%s failed with error %lu", pszCalled, dwResult);
        hr = HRESULT_FROM_WIN32(dwResult);
    }
    else
    {
        CDebug::Print("%s failed with an unknown error", pszCalled);
        hr = E_FAIL;
    }

    return hr;
}

#define WSATranslateError(dwLevel, func) \
    __WSATranslateError(dwLevel, __FILE__, __LINE__, DPF_FNAME, #func)


static HRESULT __XBDMTranslateError(DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, HRESULT hr, LPCSTR pszCalled)
{
    if(XBDM_NOERR == hr)
    {
        hr = S_OK;
    }
    else
    {
        CDebug::SetContext(DPF_FLAGS_DEFAULT, dwLevel, pszFile, nLine, pszFunction, DPF_LIBRARY);

        if(FAILED(hr))
        {
            CDebug::Print("%s failed with error %lx", pszCalled, hr);
        }
        else
        {
            CDebug::Print("%s failed with an unknown error", hr);
            hr = E_FAIL;
        }
    }

    return hr;
}

#define XBDMTranslateError(dwLevel, result, func) \
    __XBDMTranslateError(dwLevel, __FILE__, __LINE__, DPF_FNAME, result, #func)

#else // DEBUG

static HRESULT __WSATranslateError(void)
{
    DWORD                   dwResult    = WSAGetLastError();
    HRESULT                 hr;

    if(dwResult)
    {
        hr = HRESULT_FROM_WIN32(dwResult);
    }
    else
    {
        hr = E_FAIL;
    }

    return hr;
}

#define WSATranslateError(dwLevel, func) \
    __WSATranslateError()


static HRESULT __XBDMTranslateError(HRESULT hr)
{
    if(XBDM_NOERR == hr)
    {
        hr = S_OK;
    }
    else if(SUCCEEDED(hr))
    {
        hr = E_FAIL;
    }

    return hr;
}

#define XBDMTranslateError(dwLevel, result, func) \
    __XBDMTranslateError(result)

#endif // DEBUG


/****************************************************************************
 *
 *  CreateRemoteConnection
 *
 *  Description:
 *      Creates a remote transmission object.
 *
 *  Arguments:
 *      DWORD [in]: port assignment.
 *      LPCDSREMOTECALLBACK [in]: callback object.
 *      LPVOID [in]: callback context.
 *      LPREMOTECONNECTION * [out]: remote object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CreateRemoteConnection"

HRESULT
CreateRemoteConnection
(
    DWORD                   dwPort,
    LPCDSREMOTECALLBACK     pCallback,
    LPVOID                  pvContext,
    LPREMOTECONNECTION *    ppRemote
)
{
    CRemoteConnection *     pRemote;
    HRESULT                 hr;

    hr = HRFROMP(pRemote = NEW(CRemoteConnection));

    if(SUCCEEDED(hr))
    {
        hr = pRemote->Initialize(dwPort, pCallback, pvContext);
    }

    if(SUCCEEDED(hr))
    {
        *ppRemote = pRemote;
    }
    else
    {
        RELEASE(pRemote);
    }

    return hr;
}


/****************************************************************************
 *
 *  CRemoteConnection
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::CRemoteConnection"

CRemoteConnection::CRemoteConnection
(
    void
)
{
    m_socket = INVALID_SOCKET;
    m_dwState = DSREMOTE_STATE_IDLE;

    InitializeListHead(&m_lstHandlers);
    InitializeListHead(&m_lstStitched);
}


/****************************************************************************
 *
 *  ~CRemoteConnection
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::~CRemoteConnection"

CRemoteConnection::~CRemoteConnection
(
    void
)
{
    PLIST_ENTRY             pleEntry;
    CStitchedPacket *       pStitchedPacket;

    Disconnect(0);

    WSACleanup();

#ifdef _XBOX

    XNetCleanup();

#endif // _XBOX

    while((pleEntry = m_lstStitched.Flink) != &m_lstStitched)
    {
        pStitchedPacket = CONTAINING_RECORD(pleEntry, CStitchedPacket, m_leStitched);

        MEMFREE(pStitchedPacket->m_pvPacketData);
        DELETE(pStitchedPacket);
    }
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      DWORD [in]: port assignment.
 *      LPCDSREMOTECALLBACK [in]: callback object.
 *      LPVOID [in]: callback context.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::Initialize"

HRESULT
CRemoteConnection::Initialize
(
    DWORD                   dwPort,
    LPCDSREMOTECALLBACK     pCallback,
    LPVOID                  pvContext
)
{
    HRESULT                 hr          = S_OK;
    DWORD                   dwResult;
    WSADATA                 wsadata;
    SOCKADDR_IN             sa;
    
    //
    // Save data members
    //

    m_dwPort = dwPort;
    
    m_StatusCallback.Initialize(pCallback, pvContext);

    //
    // Initialize the network
    //

#ifdef _XBOX

    XNetStartupParams xnsp;

    ZeroMemory(&xnsp, sizeof(xnsp));

    xnsp.cfgSizeOfStruct = sizeof(xnsp);
    xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY | XNET_STARTUP_BYPASS_ENCRYPTION;
    
    if(dwResult = XNetStartup(&xnsp))
    {
        DPF_ERROR("XNetStartup failed with %lu", dwResult);
        hr = HRESULT_FROM_WIN32(dwResult);
    }

#endif // _XBOX

    if(SUCCEEDED(hr))
    {
        if(dwResult = WSAStartup(MAKEWORD(2, 2), &wsadata))
        {
            DPF_ERROR("WSAStartup failed with %lu", dwResult);
            hr = HRESULT_FROM_WIN32(dwResult);
        }
    }

    //
    // Register default handlers
    //

    if(SUCCEEDED(hr))
    {
        hr = m_FileIoHandler.Register(this);
    }

    //
    // Listen for a connection
    //

    if(SUCCEEDED(hr))
    {
        hr = Listen();
    }

    return hr;
}


/****************************************************************************
 *
 *  Listen
 *
 *  Description:
 *      Listens for a connection.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::Listen"

HRESULT
CRemoteConnection::Listen
(
    void
)
{
    SOCKADDR_IN             sa;
    HRESULT                 hr;

    if(DSREMOTE_STATE_LISTENING != m_dwState)
    {
        //
        // If we're connected, disconnect
        //
        
        if(DSREMOTE_STATE_CONNECTED == m_dwState)
        {
            DPF_WARNING("Already connected");
            Disconnect(0);
        }

        //
        // Create the socket
        //

        hr = CreateSocket(&m_socket);

        //
        // Bind the socket to the local address
        //

        if(SUCCEEDED(hr))
        {
            CreateAddress(NULL, m_dwPort, &sa);

            if(bind(m_socket, (const sockaddr *)&sa, sizeof(sa)))
            {
                hr = WSATranslateError(DPFLVL_ERROR, bind);
            }
        }

        //
        // Listen for a connection
        //

        if(SUCCEEDED(hr))
        {
            if(listen(m_socket, SOMAXCONN))
            {
                hr = WSATranslateError(DPFLVL_ERROR, listen);
            }
        }        

        if(SUCCEEDED(hr))
        {
            DPF_INFO("Listening for connection on port %lu", m_dwPort);

            m_dwState = DSREMOTE_STATE_LISTENING;
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  CreateAddress
 *
 *  Description:
 *      Creates an IP address.
 *
 *  Arguments:
 *      LPCSTR [in]: host address.
 *      DWORD [in]: port number.
 *      SOCKADDR_IN * [out]: address.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::CreateAddress"

void 
CRemoteConnection::CreateAddress
(
    LPCSTR                  pszAddress, 
    DWORD                   dwPort, 
    SOCKADDR_IN *           pAddr
)
{
    pAddr->sin_family = AF_INET;
    pAddr->sin_addr.s_addr = pszAddress ? inet_addr(pszAddress) : INADDR_ANY;
    pAddr->sin_port = htons((USHORT)dwPort);
}


/****************************************************************************
 *
 *  CreateSocket
 *
 *  Description:
 *      Creates a local socket.
 *
 *  Arguments:
 *      SOCKET * [out]: socket.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::CreateSocket"

HRESULT
CRemoteConnection::CreateSocket
(
    SOCKET *                psock
)
{
    HRESULT                 hr          = S_OK;
    SOCKET                  sock;
    
    if(INVALID_SOCKET == (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
    {
        hr = WSATranslateError(DPFLVL_ERROR, socket);
    }

    if(SUCCEEDED(hr))
    {
        *psock = sock;
    }

    return hr;
}


/****************************************************************************
 *
 *  Connect
 *
 *  Description:
 *      Connects to a remote session or listens for one.
 *
 *  Arguments:
 *      LPCSTR [in]: host address or NULL to listen for a connection.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::Connect"

HRESULT
CRemoteConnection::Connect
(
    LPCSTR                  pszHost,
    DWORD                   dwFlags
)
{
    HRESULT                 hr              = S_OK;
    CHAR                    szHost[0x100];
    SOCKADDR_IN             sa;

    //
    // If we're already connected, disconnect
    //

    if(DSREMOTE_STATE_CONNECTED == m_dwState)
    {
        DPF_WARNING("Already connected");
        Disconnect(0);
    }

    //
    // If the DNSLOOKUP flag was specified, the host name is an Xbox name,
    // not an address.  We'll do the conversion for them.
    //
    // If the DNSLOOKUP flag was specified, but no host was, the caller wants
    // to connect to the default Xbox.
    //
    // If the host name is NULL *and* DNSLOOKUP wasn't specified, the caller
    // wants to block until someone else connects to this session.
    //

    if(dwFlags & DSREMOTE_CONNECT_DNSLOOKUP)
    {
        hr = GetHostAddress(pszHost, szHost);

        if(SUCCEEDED(hr))
        {
            pszHost = szHost;
        }
    }

    //
    // Connect to the remote host
    //

    if(pszHost)
    {
        hr = CreateConnection(&m_socket, pszHost, m_dwPort, &sa);
    }
    else
    {
        hr = AcceptConnection(&m_socket, &sa);
    }

    //
    // Call the callback
    //

    if(SUCCEEDED(hr))
    {
        OnConnect(sa);
    }

    return hr;
}


/****************************************************************************
 *
 *  GetHostAddress
 *
 *  Description:
 *      Looks up an Xbox's address.
 *
 *  Arguments:
 *      LPCSTR [in]: host name.
 *      LPSTR [out]: host address.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::GetHostAddress"

HRESULT
CRemoteConnection::GetHostAddress
(
    LPCSTR                  pszHost,
    LPSTR                   pszAddress
)
{

#ifdef _XBOX

    DPF_ERROR("DNSLOOKUP is not supported by the Xbox version of this library");
    
    return E_INVALIDARG;

#else // _XBOX

    HRESULT                 hr              = S_OK;
    CHAR                    szHost[0x100];
    DWORD                   dwSize;
    DWORD                   dwAddress;

    if(!pszHost)
    {
        dwSize = NUMELMS(szHost);
        
        hr = XBDMTranslateError(DPFLVL_ERROR, DmGetXboxName(szHost, &dwSize), DmGetXboxName);

        if(SUCCEEDED(hr))
        {
            pszHost = szHost;
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = XBDMTranslateError(DPFLVL_ERROR, DmSetXboxNameNoRegister(pszHost), XBDMTranslateError);
    }

    if(SUCCEEDED(hr))
    {
        hr = XBDMTranslateError(DPFLVL_ERROR, DmGetAltAddress(&dwAddress), DmGetAltAddress);
    }

    if(SUCCEEDED(hr))
    {
        sprintf(pszAddress, "%lu.%lu.%lu.%lu", ((LPBYTE)&dwAddress)[3], ((LPBYTE)&dwAddress)[2], ((LPBYTE)&dwAddress)[1], ((LPBYTE)&dwAddress)[0]);
    }

    return hr;

#endif // _XBOX

}


/****************************************************************************
 *
 *  CreateConnection
 *
 *  Description:
 *      Connects to a remote session or listens for one.
 *
 *  Arguments:
 *      SOCKET * [in/out]: socket.
 *      LPCSTR [in]: host address.
 *      DWORD [in]: host port.
 *      SOCKADDR_IN * [out]: host address.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::CreateConnection"

HRESULT
CRemoteConnection::CreateConnection
(
    SOCKET *                pSocket,
    LPCSTR                  pszHost,
    DWORD                   dwPort,
    SOCKADDR_IN *           pAddr
)
{
    SOCKET                  sock;
    SOCKADDR_IN             sa;
    HRESULT                 hr;

    CreateAddress(pszHost, dwPort, &sa);

    //
    // Create the new socket
    //

    hr = CreateSocket(&sock);

    //
    // Make the remote connection
    //

    if(SUCCEEDED(hr))
    {
        DPF_INFO("Attempting connection to %lu.%lu.%lu.%lu:%lu on socket %x", sa.sin_addr.S_un.S_un_b.s_b1, sa.sin_addr.S_un.S_un_b.s_b2, sa.sin_addr.S_un.S_un_b.s_b3, sa.sin_addr.S_un.S_un_b.s_b4, dwPort, sock);

        if(connect(sock, (const sockaddr *)&sa, sizeof(sa)))
        {
            hr = WSATranslateError(DPFLVL_WARNING, connect);
        }
    }

    //
    // Close supplied socket and return the new one
    //

    if(SUCCEEDED(hr))
    {
        CloseSocket(pSocket);

        *pSocket = sock;
    }

    //
    // Return the address
    //

    if(SUCCEEDED(hr) && pAddr)
    {
        *pAddr = sa;
    }

    return hr;
}


/****************************************************************************
 *
 *  AcceptConnection
 *
 *  Description:
 *      Accepts an incoming connection.
 *
 *  Arguments:
 *      SOCKET * [in/out]: socket.  The socket is assumed to be in a listen
 *                         state on entry.  If a connection is made, the
 *                         orginal socket will be closed and a new one 
 *                         returned.
 *      SOCKADDR_IN [out]: address of the connected entity.
 *      
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::AcceptConnection"

HRESULT
CRemoteConnection::AcceptConnection
(
    SOCKET *                pSocket,
    SOCKADDR_IN *           pAddr
)
{
    int                     nSize   = sizeof(*pAddr);
    HRESULT                 hr      = S_OK;
    SOCKET                  sock;

    //
    // Wait for the connection
    //

    if(INVALID_SOCKET == (sock = accept(*pSocket, (sockaddr *)pAddr, pAddr ? &nSize : NULL)))
    {
        hr = WSATranslateError(DPFLVL_WARNING, accept);
    }

    //
    // Close the listen socket and return the new one
    //

    if(SUCCEEDED(hr))
    {
        CloseSocket(pSocket);

        *pSocket = sock;
    }

    return hr;
}


/****************************************************************************
 *
 *  GetSocketStatus
 *
 *  Description:
 *      Checks a socket for pending read or write data.
 *
 *  Arguments:
 *      SOCKET  [in]: socket.
 *      DWORD [in]: access flags (GENERIC_READ | GENERIC_WRITE).
 *      DWORD [in]: timeout, in ms.
 *
 *  Returns:  
 *      BOOL: TRUE if the socket is ready.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::GetSocketStatus"

BOOL
CRemoteConnection::GetSocketStatus
(
    SOCKET                  sock,
    DWORD                   dwAccess,
    DWORD                   dwTimeout
)
{
    TIMEVAL                 timeval;
    fd_set                  fdsRead;
    fd_set                  fdsWrite;
    DWORD                   dwResult;
    BOOL                    fReady;

    ASSERT(dwAccess & (GENERIC_READ | GENERIC_WRITE));

    timeval.tv_sec = 0;
    timeval.tv_usec = dwTimeout;

    FD_ZERO(&fdsRead);
    FD_ZERO(&fdsWrite);

    if(dwAccess & GENERIC_READ)
    {
        FD_SET(sock, &fdsRead);
    }

    if(dwAccess & GENERIC_WRITE)
    {
        FD_SET(sock, &fdsWrite);
    }

    dwResult = select(0, &fdsRead, &fdsWrite, NULL, (INFINITE == dwTimeout) ? NULL : &timeval);

    switch(dwResult)
    {
        case 1:
        case 2:
            fReady = TRUE;
            break;

        case SOCKET_ERROR:
        case 0:
            fReady = FALSE;
            break;

        default:
            ASSERTMSG("Unexpected result from select");
            fReady = FALSE;
            break;
    }

    return fReady;
}


/****************************************************************************
 *
 *  DoWork
 *
 *  Description:
 *      Worker function.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::DoWork"

void
CRemoteConnection::DoWork
(
    void
)
{
    LPVOID                  pvPacketData;
    DSRPACKETHEADER         PacketHeader;
    SOCKADDR_IN             sa;
    HRESULT                 hr;

    if(INVALID_SOCKET != m_socket)
    {
        //
        // Check for pending read data
        //

        if(GetSocketStatus(m_socket, GENERIC_READ, 0))
        {
            if(DSREMOTE_STATE_LISTENING == m_dwState)
            {
                //
                // Accept an incoming connection
                //

                hr = AcceptConnection(&m_socket, &sa);
                
                if(SUCCEEDED(hr))
                {
                    OnConnect(sa);
                }
            }
            else if(DSREMOTE_STATE_CONNECTED == m_dwState)
            {
                //
                // Receive and dispatch the next message
                //

                hr = ReceiveMessage(&PacketHeader, &pvPacketData);

                if(SUCCEEDED(hr) && pvPacketData)
                {
                    DispatchMessage(&PacketHeader, &pvPacketData);
                }
            }
        }
    }
}
        

/****************************************************************************
 *
 *  OnConnect
 *
 *  Description:
 *      Called when a connection is made.
 *
 *  Arguments:
 *      const SOCKADDR_IN & [in]: connection address.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::OnConnect"

void
CRemoteConnection::OnConnect
(
    const SOCKADDR_IN &     sa
)
{
    CHAR                    szAddress[0x100];

    //
    // Flag the object as connected
    //

    m_dwState = DSREMOTE_STATE_CONNECTED;

    //
    // Call the callback
    //

    sprintf(szAddress, "%lu.%lu.%lu.%lu", sa.sin_addr.S_un.S_un_b.s_b1, sa.sin_addr.S_un.S_un_b.s_b2, sa.sin_addr.S_un.S_un_b.s_b3, sa.sin_addr.S_un.S_un_b.s_b4);

    DPF_INFO("Connected to %s", szAddress);

    m_StatusCallback.Connect(szAddress);
}


/****************************************************************************
 *
 *  Disconnect
 *
 *  Description:
 *      Disconnects a connected socket.
 *
 *  Arguments:
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::Disconnect"

HRESULT
CRemoteConnection::Disconnect
(
    DWORD                   dwFlags
)
{
    HRESULT                 hr  = S_OK;
    
    if(DSREMOTE_STATE_IDLE != m_dwState)
    {
        CloseSocket(&m_socket);

        m_dwState = DSREMOTE_STATE_IDLE;

        DPF_INFO("Disconnected");

        m_StatusCallback.Disconnect();
    }

    if(dwFlags & DSREMOTE_DISCONNECT_LISTEN)
    {
        hr = Listen();
    }

    return hr;
}


/****************************************************************************
 *
 *  CloseSocket
 *
 *  Description:
 *      Closes an open socket.
 *
 *  Arguments:
 *      SOCKET * [in/out]: socket.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::CloseSocket"

void 
CRemoteConnection::CloseSocket
(
    SOCKET *                psock
)
{
    ASSERT(psock);

    if(INVALID_SOCKET != *psock)
    {
        shutdown(*psock, SD_BOTH);
        closesocket(*psock);

        *psock = INVALID_SOCKET;
    }
}


/****************************************************************************
 *
 *  RegisterMsgHandler
 *
 *  Description:
 *      Registers a message handler.
 *
 *  Arguments:
 *      DWORD [in]: message major type.
 *      LPCDSREMOTEMSGHANDLER [in]: handler object.
 *      LPVOID [in]: handler context.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::RegisterMsgHandler"

HRESULT
CRemoteConnection::RegisterMsgHandler
(
    DWORD                   dwMajorType,
    LPCDSREMOTEMSGHANDLER   pCallback,
    LPVOID                  pvContext
)
{
    HRESULT                 hr          = S_OK;
    CRemoteMsgHandler *     pHandler;
    LIST_ENTRY *            pleEntry;

    for(pleEntry = m_lstHandlers.Flink; (pleEntry != &m_lstHandlers) && SUCCEEDED(hr); pleEntry = pleEntry->Flink)
    {
        pHandler = CONTAINING_RECORD(pleEntry, CRemoteMsgHandler, m_leHandlers);

        if(dwMajorType == pHandler->m_dwMajorType)
        {
            DPF_ERROR("Type %lu already registered", dwMajorType);
            hr = E_INVALIDARG;
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = HRFROMP(pHandler = NEW(CRemoteMsgHandler));
    }

    if(SUCCEEDED(hr))
    {
        pHandler->Initialize(dwMajorType, pCallback, pvContext);
    }

    if(SUCCEEDED(hr))
    {
        InsertTailList(&m_lstHandlers, &pHandler->m_leHandlers);
    }

    return hr;
}


/****************************************************************************
 *
 *  UnregisterMsgHandler
 *
 *  Description:
 *      Unregisters a message handler.
 *
 *  Arguments:
 *      DWORD [in]: message major type.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::UnregisterMsgHandler"

HRESULT
CRemoteConnection::UnregisterMsgHandler
(
    DWORD                   dwMajorType
)
{
    HRESULT                 hr          = E_INVALIDARG;
    CRemoteMsgHandler *     pHandler;
    LIST_ENTRY *            pleEntry;

    for(pleEntry = m_lstHandlers.Flink; pleEntry != &m_lstHandlers; pleEntry = pleEntry->Flink)
    {
        pHandler = CONTAINING_RECORD(pleEntry, CRemoteMsgHandler, m_leHandlers);

        if(dwMajorType == pHandler->m_dwMajorType)
        {
            RemoveEntryList(pleEntry);

            DELETE(pHandler);

            hr = S_OK;
            
            break;
        }
    }

    if(FAILED(hr))
    {
        DPF_ERROR("Handler not registered");
    }

    return hr;
}


/****************************************************************************
 *
 *  Send
 *
 *  Description:
 *      Transmits data across the connection.
 *
 *  Arguments:
 *      LPCDSRPACKET [in]: packet header.
 *      DWORD [in]: packet data buffer count.
 *      LPCDSRBUFFER [in]: packet data buffers.
 *      LPDSRBUFFER [in/out]: response data buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::Send"

HRESULT
CRemoteConnection::Send
(
    LPCDSRPACKET            pPacketHeader, 
    DWORD                   dwPacketBufferCount, 
    LPCDSRBUFFER            paPacketBuffers, 
    LPDSRBUFFER             pResponseBuffer
)
{
    HRESULT                 hr              = S_OK;
    DSRPACKETHEADER         PacketHeader;
    DSRPACKETHEADER         ResponseHeader;
    LPVOID                  pvResponseData;
    DWORD                   dwSent;
    BOOL                    fDispatch;

    if(!pPacketHeader)
    {
        DPF_ERROR("No packet descriptor supplied");
        hr = E_INVALIDARG;
    }

    if(SUCCEEDED(hr) && HIWORD(pPacketHeader->dwFlags))
    {
        DPF_ERROR("The high word of the packet flags is reserved for internal use");
        hr = E_INVALIDARG;
    }
    
    if(SUCCEEDED(hr))
    {
        if(dwPacketBufferCount && !paPacketBuffers)
        {
            DPF_ERROR("Buffer count is non-zero, but paPacketBuffers is NULL");
            hr = E_INVALIDARG;
        }
        else if(!dwPacketBufferCount && paPacketBuffers)
        {
            DPF_ERROR("Buffer count is zero, but paPacketBuffers is non-NULL");
            hr = E_INVALIDARG;
        }
    }

    if(SUCCEEDED(hr) && pResponseBuffer)
    {
        if(pResponseBuffer->pvData && !pResponseBuffer->dwSize)
        {
            DPF_ERROR("Valid response buffer supplied, but dwSize is 0");
            hr = E_INVALIDARG;
        }
        else if(!pResponseBuffer->pvData && pResponseBuffer->dwSize)
        {
            DPF_ERROR("No response buffer supplied, but dwSize is non-zero");
            hr = E_INVALIDARG;
        }
    }

    if(SUCCEEDED(hr) && (DSREMOTE_STATE_CONNECTED != m_dwState))
    {
        DPF_ERROR("Not connected");
        hr = E_FAIL;
    }

    //
    // Send the data.  If an empty response structure was supplied, the caller
    // just wants to make sure the message was handled, but doesn't care what
    // data the handler wants to send back.
    //
    // Note that dwSize, dwChecksum, dwStitchSize and dwStitchOffset are filled
    // in by SendMessage.
    //

    if(SUCCEEDED(hr))
    {
        PacketHeader.dwSignature = DSREMOTE_PACKET_SIGNATURE;
        PacketHeader.dwSequence = m_dwSequence++;
        PacketHeader.dwMajorType = pPacketHeader->dwMajorType;
        PacketHeader.dwMinorType = pPacketHeader->dwMinorType;
        PacketHeader.dwFlags = pPacketHeader->dwFlags;

        if(pResponseBuffer)
        {
            PacketHeader.dwFlags |= DSRPACKETF_ACKREQ;
            PacketHeader.dwResponseSize = pResponseBuffer->dwSize;
        }
        else
        {
            PacketHeader.dwResponseSize = 0;
        }

        hr = SendMessage(&PacketHeader, dwPacketBufferCount, paPacketBuffers);
    }

    //
    // Wait for the response packet
    //

    if(SUCCEEDED(hr) && pResponseBuffer)
    {
        while(SUCCEEDED(hr))
        {
            hr = ReceiveMessage(&ResponseHeader, &pvResponseData);

            if(SUCCEEDED(hr))
            {
                if((ResponseHeader.dwSequence == PacketHeader.dwSequence) && (ResponseHeader.dwFlags & DSRPACKETF_ACK))
                {
                    ASSERT(ResponseHeader.dwMajorType == PacketHeader.dwMajorType);
                    ASSERT(ResponseHeader.dwMinorType == PacketHeader.dwMinorType);
                    ASSERT(ResponseHeader.dwSize <= PacketHeader.dwResponseSize);
                    ASSERT(!ResponseHeader.dwResponseSize);

                    if(pResponseBuffer->dwSize = min(PacketHeader.dwResponseSize, ResponseHeader.dwSize))
                    {
                        ASSERT(pvResponseData);

                        CopyMemory(pResponseBuffer->pvData, pvResponseData, pResponseBuffer->dwSize);
                    }

                    MEMFREE(pvResponseData);

                    break;
                }
                else if(pvResponseData)
                {
                    hr = DispatchMessage(&ResponseHeader, &pvResponseData);
                }
            }
        }
    }

    return hr;
}


/****************************************************************************
 *
 *  SendMessage
 *
 *  Description:
 *      Transmits data across the connection.
 *
 *  Arguments:
 *      LPCDSRPACKETHEADER [in]: packet header.
 *      DWORD [in]: packet data buffer count.
 *      LPCDSRBUFFER [in]: packet data buffers.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::SendMessage"

HRESULT
CRemoteConnection::SendMessage
(
    LPCDSRPACKETHEADER      pPacketHeader,
    DWORD                   dwBufferCount,
    LPCDSRBUFFER            paBuffers
)
{
    LPDSRPACKETHEADER       pBuffer;
    DWORD                   dwTotalPacketSize;
    DWORD                   dwBufferSize;
    DWORD                   dwPacketDataSize;
    DWORD                   dwPacketCount;
    DWORD                   dwBufferIndex;
    DWORD                   dwSrcOffset;
    DWORD                   dwDstOffset;
    DWORD                   dwCopy;
    DWORD                   dwSent;
    HRESULT                 hr;
    DWORD                   i;

    ASSERT(DSREMOTE_STATE_CONNECTED == m_dwState);

    //
    // Determine the total outgoing message size
    //

    for(i = 0, dwTotalPacketSize = 0; i < dwBufferCount; i++)
    {
        dwTotalPacketSize += paBuffers[i].dwSize;
    }

    //
    // Coalesce the data into a single buffer.  If the message size exceeds
    // the maximum, send the message as multiple packets.
    //

    if(dwTotalPacketSize > DSREMOTE_OPTIMAL_PACKET_SIZE)
    {
        dwPacketDataSize = DSREMOTE_OPTIMAL_PACKET_SIZE;
    }
    else
    {
        dwPacketDataSize = dwTotalPacketSize;
    }

    dwPacketCount = (dwTotalPacketSize + dwPacketDataSize - 1) / dwPacketDataSize;
    dwBufferSize = dwPacketDataSize + sizeof(*pPacketHeader);
    
    hr = HRFROMP(pBuffer = (LPDSRPACKETHEADER)MEMALLOC_NOINIT(BYTE, dwBufferSize));

    if(SUCCEEDED(hr))
    {
        *pBuffer = *pPacketHeader;

        pBuffer->dwSize = dwTotalPacketSize;
        pBuffer->dwStitchOffset = 0;
        pBuffer->dwStitchSize = dwPacketDataSize;
    }
        
    //
    // Send the data out over the wire
    //

    for(i = 0, dwBufferIndex = 0, dwSrcOffset = 0; (i < dwPacketCount) && SUCCEEDED(hr); i++)
    {
        //
        // If this is the last packet, fix the size
        //
        
        if(i + 1 == dwPacketCount)
        {
            pBuffer->dwStitchSize = dwTotalPacketSize - (dwPacketDataSize * (dwPacketCount - 1));
        }
        
        //
        // Copy the packet data into the outgoing buffer
        //
        
        dwDstOffset = 0;
        
        while(dwDstOffset < pBuffer->dwStitchSize)
        {
            if(dwSrcOffset >= paBuffers[dwBufferIndex].dwSize)
            {
                dwBufferIndex++;
                dwSrcOffset = 0;
            }

            dwCopy = min(pBuffer->dwStitchSize - dwDstOffset, paBuffers[dwBufferIndex].dwSize - dwSrcOffset);
            
            CopyMemory((LPBYTE)(pBuffer + 1) + dwDstOffset, (LPBYTE)paBuffers[dwBufferIndex].pvData + dwSrcOffset, dwCopy);

            dwSrcOffset += dwCopy;
            dwDstOffset += dwCopy;
        }

        //
        // Create a checksum
        //

        pBuffer->dwChecksum = CreateChecksum(pBuffer + 1, pBuffer->dwStitchSize);

        //
        // Transmit the data
        //

        DPF_INFO("Sending %lu bytes (stitch offset %lu)", pBuffer->dwStitchSize, pBuffer->dwStitchOffset);

        hr = send(pBuffer, sizeof(*pBuffer) + pBuffer->dwStitchSize);

        //
        // Increment the stitch offset
        //

        pBuffer->dwStitchOffset += pBuffer->dwStitchSize;
    }

    //
    // Clean up
    //

    MEMFREE(pBuffer);

    return hr;
}


/****************************************************************************
 *
 *  ReceiveMessage
 *
 *  Description:
 *      Receives data across a connection.
 *
 *  Arguments:
 *      LPDSRPACKETHEADER [out]: packet header.
 *      LPVOID * [out]: packet data.  The caller is responsible for freeing
 *                      this buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::ReceiveMessage"

HRESULT
CRemoteConnection::ReceiveMessage
(
    LPDSRPACKETHEADER       pPacketHeader,
    LPVOID *                ppvPacketData
)
{
    LPVOID                  pvPacketData    = NULL;
    LPVOID                  pvResponseData  = NULL;
    CStitchedPacket *       pStitchedPacket = NULL;
    HRESULT                 hr              = S_OK;
    PLIST_ENTRY             pleEntry;
    CRemoteMsgHandler *     pHandler;
    BOOL                    fResponse;
    BOOL                    fComplete;
    DWORD                   dwChecksum;

    ASSERT(DSREMOTE_STATE_CONNECTED == m_dwState);

    //
    // Receive just the packet header.
    //
    // We actually expect to get an error here.  Most of the time, there
    // should be more data following the packet, so we should get WSAEMSGSIZE
    //

    hr = recv(&pPacketHeader->dwSignature, sizeof(pPacketHeader->dwSignature));

    if(SUCCEEDED(hr) && (DSREMOTE_PACKET_SIGNATURE != pPacketHeader->dwSignature))
    {
        DPF_WARNING("Invalid packet signature");
        hr = E_FAIL;
    }

    if(SUCCEEDED(hr))
    {
        hr = recv((LPBYTE)pPacketHeader + sizeof(pPacketHeader->dwSignature), sizeof(*pPacketHeader) - sizeof(pPacketHeader->dwSignature));
    }
        
    //
    // If any data follows the packet header, receive it now
    //

    if(SUCCEEDED(hr) && pPacketHeader->dwSize)
    {
        //
        // Get the receive buffer, either from the stitched packet object or
        // by allocating one.
        //
        
        if(pPacketHeader->dwStitchSize < pPacketHeader->dwSize)
        {
            hr = CreateStitchedPacket(pPacketHeader, &pStitchedPacket);

            if(SUCCEEDED(hr))
            {
                pvPacketData = pStitchedPacket->m_pvPacketData;
            }
        }
        else
        {
            ASSERT(pPacketHeader->dwStitchSize == pPacketHeader->dwSize);

            hr = HRFROMP(pvPacketData = MEMALLOC_NOINIT(BYTE, pPacketHeader->dwSize));
        }

        //
        // Receive the packet data
        //

        if(SUCCEEDED(hr))
        {
            ASSERT(pPacketHeader->dwStitchOffset + pPacketHeader->dwStitchSize <= pPacketHeader->dwSize);
            ASSERT(pPacketHeader->dwStitchSize);

            hr = recv((LPBYTE)pvPacketData + pPacketHeader->dwStitchOffset, pPacketHeader->dwStitchSize);
        }

        //
        // Check the checksum
        //

        if(SUCCEEDED(hr))
        {
            dwChecksum = CreateChecksum((LPBYTE)pvPacketData + pPacketHeader->dwStitchOffset, pPacketHeader->dwStitchSize);

            if(dwChecksum != pPacketHeader->dwChecksum)
            {
                DPF_ERROR("Checksum error");
                hr = E_FAIL;
            }
        }

        //
        // Increment the amount of data we've received.  If we've received
        // all of it, we can return the data to the caller and free the
        // stitched packet object.  If not, return NULL to the calling 
        // function.
        //

        if(SUCCEEDED(hr))
        {
            DPF_INFO("Received %lu bytes (stitch offset %lu)", pPacketHeader->dwStitchSize, pPacketHeader->dwStitchOffset);

            if(pStitchedPacket)
            {
                ASSERT(pStitchedPacket->m_dwReceived + pPacketHeader->dwStitchSize <= pPacketHeader->dwSize);
            
                pStitchedPacket->m_dwReceived += pPacketHeader->dwStitchSize;

                if(fComplete = (pStitchedPacket->m_dwReceived >= pPacketHeader->dwSize))
                {
                    DELETE(pStitchedPacket);
                }
                else
                {
                    pvPacketData = NULL;
                }
            }
            else
            {
                fComplete = TRUE;
            }
        }
    }

    //
    // Success
    //

    if(SUCCEEDED(hr))
    {
        *ppvPacketData = pvPacketData;
    }
    else
    {
        MEMFREE(pvPacketData);
    }

    return hr;
}


/****************************************************************************
 *
 *  DispatchMessage
 *
 *  Description:
 *      Dispatches a message to the appropriate handler.
 *
 *  Arguments:
 *      LPCDSRPACKETHEADER [in]: packet header.
 *      LPVOID * [in/out]: packet data.  This buffer will be freed during
 *                         this call.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::DispatchMessage"

HRESULT
CRemoteConnection::DispatchMessage
(
    LPCDSRPACKETHEADER      pPacketHeader, 
    LPVOID *                ppvPacketData
)
{
    HRESULT                 hr              = S_OK;
    PLIST_ENTRY             pleEntry;
    CRemoteMsgHandler *     pHandler;
    DSRPACKET               Packet;
    DSRBUFFER               PacketBuffer;
    DSRBUFFER               Response;
    DSRPACKETHEADER         ResponseHeader;

    ASSERT(!(pPacketHeader->dwFlags & DSRPACKETF_ACK));
    ASSERT(ppvPacketData && *ppvPacketData);

    //
    // Set up the external version of the packet header
    //

    Packet.dwMajorType = pPacketHeader->dwMajorType;
    Packet.dwMinorType = pPacketHeader->dwMinorType;
    Packet.dwFlags = LOWORD(pPacketHeader->dwFlags);

    PacketBuffer.dwSize = pPacketHeader->dwSize;
    PacketBuffer.pvData = *ppvPacketData;

    //
    // Set up and allocate the response data
    //

    if((pPacketHeader->dwFlags & DSRPACKETF_ACKREQ) && pPacketHeader->dwResponseSize)
    {
        Response.dwSize = pPacketHeader->dwResponseSize;

        hr = HRFROMP(Response.pvData = MEMALLOC_NOINIT(BYTE, Response.dwSize));
    }
    else
    {
        Response.dwSize = 0;
        Response.pvData = NULL;
    }

    //
    // Find the correct handler function
    //

    if(SUCCEEDED(hr))
    {
        for(pleEntry = m_lstHandlers.Flink; pleEntry != &m_lstHandlers; pleEntry = pleEntry->Flink)
        {
            pHandler = CONTAINING_RECORD(pleEntry, CRemoteMsgHandler, m_leHandlers);

            if(pPacketHeader->dwMajorType == pHandler->m_dwMajorType)
            {
                break;
            }
        }

        if(pleEntry == &m_lstHandlers)
        {
            DPF_ERROR("No handler found for major type %lu", pPacketHeader->dwMajorType);
            hr = E_FAIL;
        }
    }

    //
    // Call the handler
    //

    if(SUCCEEDED(hr))
    {
        if(!pHandler->Receive(&Packet, &PacketBuffer, &Response))
        {
            Response.dwSize = 0;
        }
    }

    //
    // Regardless of whether the message was handled or not, we'll send an ack
    // back to the caller.  If the message was handled and response data was
    // supplied, we'll include that with the ack packet.
    //

    if(SUCCEEDED(hr) && (pPacketHeader->dwFlags & DSRPACKETF_ACKREQ))
    {
        ResponseHeader = *pPacketHeader;

        ResponseHeader.dwFlags |= DSRPACKETF_ACK;
        ResponseHeader.dwSize = Response.dwSize;
        ResponseHeader.dwResponseSize = 0;
        
        hr = SendMessage(&ResponseHeader, 1, &Response);
    }

    //
    // Clean up
    //

    MEMFREE(Response.pvData);
    MEMFREE(*ppvPacketData);

    return hr;
}


/****************************************************************************
 *
 *  send
 *
 *  Description:
 *      Wrapper around WinSock's send function.
 *
 *  Arguments:
 *      LPCVOID [in]: data.
 *      DWORD [in]: size, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::send"

HRESULT
CRemoteConnection::send
(
    LPCVOID                 pvBuffer,
    DWORD                   dwBufferSize
)
{
    HRESULT                 hr      = S_OK;
    DWORD                   dwSent;

    ASSERT(DSREMOTE_STATE_CONNECTED == m_dwState);

    if((dwSent = ::send(m_socket, (const char *)pvBuffer, dwBufferSize, 0)) != dwBufferSize)
    {
        if(SOCKET_ERROR == dwSent)
        {
            hr = WSATranslateError(DPFLVL_WARNING, send);
        }
        else
        {
            DPF_WARNING("send returned no error, but not all the data was sent (%lu != %lu)", dwSent, dwBufferSize);
            hr = E_FAIL;
        }
    }

    if(FAILED(hr))
    {
        Disconnect(DSREMOTE_DISCONNECT_LISTEN);
    }

    return hr;
}


/****************************************************************************
 *
 *  recv
 *
 *  Description:
 *      Wrapper around WinSock's recv function.
 *
 *  Arguments:
 *      LPVOID [in]: data.
 *      DWORD [in]: size, in bytes.
 *      LPDWORD [out]: amount of data received, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::recv"

HRESULT
CRemoteConnection::recv
(
    LPVOID                  pvBuffer,
    DWORD                   dwBufferSize,
    LPDWORD                 pdwReceived
)
{
    DWORD                   dwTotalReceived = 0;
    HRESULT                 hr              = S_OK;
    DWORD                   dwReceived;

    ASSERT(DSREMOTE_STATE_CONNECTED == m_dwState);

    while(SUCCEEDED(hr) && (dwTotalReceived < dwBufferSize))
    {
        if((dwReceived = ::recv(m_socket, (char *)pvBuffer + dwTotalReceived, dwBufferSize - dwTotalReceived, 0)) != dwBufferSize - dwTotalReceived)
        {
            if(SOCKET_ERROR == dwReceived)
            {
                if(!pdwReceived || (WSAEMSGSIZE != WSAGetLastError()))
                {
                    hr = WSATranslateError(DPFLVL_WARNING, recv);
                }
            }
            else if(!dwReceived)
            {
                DPF_WARNING("recv returned no error, but not all the data was received (%lu != %lu)", dwReceived, dwBufferSize);
                hr = E_FAIL;
            }
        }

        if(SUCCEEDED(hr))
        {
            dwTotalReceived += dwReceived;
        }
    }

    if(SUCCEEDED(hr) && pdwReceived)
    {
        *pdwReceived = dwTotalReceived;
    }
    
    if(FAILED(hr))
    {
        Disconnect(DSREMOTE_DISCONNECT_LISTEN);
    }

    return hr;
}


/****************************************************************************
 *
 *  CreateStitchedPacket
 *
 *  Description:
 *      Creates a stitched packet object.
 *
 *  Arguments:
 *      LPCDSRPACKET [in]: packet header.
 *      CStitchedPacket ** [out]: stitched packet object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::CreateStitchedPacket"

HRESULT
CRemoteConnection::CreateStitchedPacket
(
    LPCDSRPACKETHEADER      pPacketHeader,
    CStitchedPacket **      ppStitchedPacket
)
{
    HRESULT                 hr              = S_OK;
    CStitchedPacket *       pStitchedPacket;
    PLIST_ENTRY             pleEntry;
    
    //
    // See if a packet is already in the list
    //

    for(pleEntry = m_lstStitched.Flink; pleEntry != &m_lstStitched; pleEntry = pleEntry->Flink)
    {
        pStitchedPacket = CONTAINING_RECORD(pleEntry, CStitchedPacket, m_leStitched);

        if(pStitchedPacket->m_PacketHeader.dwSequence == pPacketHeader->dwSequence)
        {
            ASSERT(pPacketHeader->dwMajorType == pStitchedPacket->m_PacketHeader.dwMajorType);
            ASSERT(pPacketHeader->dwMinorType == pStitchedPacket->m_PacketHeader.dwMinorType);
            ASSERT(pPacketHeader->dwFlags == pStitchedPacket->m_PacketHeader.dwFlags);
            ASSERT(pPacketHeader->dwSize == pStitchedPacket->m_PacketHeader.dwSize);
            ASSERT(pPacketHeader->dwResponseSize == pStitchedPacket->m_PacketHeader.dwResponseSize);
            
            break;
        }
    }

    if(pleEntry == &m_lstStitched)
    {
        hr = HRFROMP(pStitchedPacket = NEW(CStitchedPacket));

        if(SUCCEEDED(hr))
        {
            hr = pStitchedPacket->Initialize(pPacketHeader, &m_lstStitched);
        }
    }

    if(SUCCEEDED(hr))
    {
        *ppStitchedPacket = pStitchedPacket;
    }

    return hr;
}


/****************************************************************************
 *
 *  CreateChecksum
 *
 *  Description:
 *      Creates a 32-bit data checksum.
 *
 *  Arguments:
 *      LPCVOID [in]: data buffer.
 *      DWORD [in]: buffer size, in bytes.
 *
 *  Returns:  
 *      DWORD: checksum.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CRemoteConnection::CreateChecksum"

#pragma warning(disable:4035)

DWORD __fastcall
CRemoteConnection::CreateChecksum
(
    LPCVOID                 pvBuffer,
    DWORD                   dwBufferSize
)
{
    __asm 
    {
	    xor		eax, eax
        xor     ebx, ebx
        shr     edx, 2
        test    edx, edx
        jz      L2
L1:     add     eax, [ecx]
        adc     ebx, 0
        add     ecx, 4
        dec     edx
        jnz     L1
L2:     add     eax, ebx
        adc     eax, 0
    }
}

#pragma warning(default:4035)


/****************************************************************************
 *
 *  CStitchedPacket
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CStitchedPacket::CStitchedPacket"

CStitchedPacket::CStitchedPacket
(
    void
)
{
    InitializeListHead(&m_leStitched);
}


/****************************************************************************
 *
 *  ~CStitchedPacket
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CStitchedPacket::~CStitchedPacket"

CStitchedPacket::~CStitchedPacket
(
    void
)
{
    RemoveEntryList(&m_leStitched);
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      LPCDSRPACKETHEADER [in]: packet data.
 *      PLIST_ENTRY [in/out]: stitched message list.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CStitchedPacket::Initialize"

HRESULT
CStitchedPacket::Initialize
(
    LPCDSRPACKETHEADER      pPacketHeader,
    PLIST_ENTRY             plstStitched
)
{
    HRESULT                 hr;

    hr = HRFROMP(m_pvPacketData = MEMALLOC_NOINIT(BYTE, pPacketHeader->dwSize));

    if(SUCCEEDED(hr))
    {
        m_PacketHeader = *pPacketHeader;

        InsertTailList(plstStitched, &m_leStitched);
    }

    return hr;
}


