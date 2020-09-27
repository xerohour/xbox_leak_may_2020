/***************************************************************************
 *
 *  Copyright (C) 1/8/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       iremote.h
 *  Content:    IRemoteConnection interface impelementation.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/8/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __IREMOTE_H__
#define __IREMOTE_H__

//
// Packet signature
//

#define DSREMOTE_PACKET_SIGNATURE   'srsd'

//
// Reserved packet flags (outgoing)
//

#define DSRPACKETF_ACKREQ           0x00010000

//
// Reserved packet flags (incoming)
//

#define DSRPACKETF_ACK              0x80000000

//
// Packet header
//

BEGIN_DEFINE_STRUCT()
    DWORD           dwSignature;        // Packet signature
    DWORD           dwSequence;         // Packet sequence number
    DWORD           dwMajorType;        // Packet major type
    DWORD           dwMinorType;        // Packet minor type
    DWORD           dwFlags;            // Packet flags
    DWORD           dwSize;             // Packet data buffer size
    DWORD           dwResponseSize;     // Response data size
    DWORD           dwStitchOffset;     // Stitched message data offset
    DWORD           dwStitchSize;       // Stitched message size
    DWORD           dwChecksum;         // Packet checksum
END_DEFINE_STRUCT(DSRPACKETHEADER);

#ifdef __cplusplus

//
// DSREMOTECALLBACK wrapper
//

namespace DSREMOTE
{
    class CRemoteCallback
    {
    private:
        DSREMOTECALLBACK        m_Callback;         // Callback object
        LPVOID                  m_pvContext;        // Context

    public:
        void Initialize(LPCDSREMOTECALLBACK pCallback, LPVOID pvContext);
        void Connect(LPCSTR pszRemoteAddress);
        void Disconnect(void);
    };

    __inline void CRemoteCallback::Initialize(LPCDSREMOTECALLBACK pCallback, LPVOID pvContext)
    {
        if(pCallback)
        {
            m_Callback = *pCallback;
        }
        else
        {
            ZeroMemory(&m_Callback, sizeof(m_Callback));
        }

        m_pvContext = pvContext;
    }

    __inline void CRemoteCallback::Connect(LPCSTR pszRemoteAddress)
    {
        if(m_Callback.Connect)
        {
            m_Callback.Connect(pszRemoteAddress, m_pvContext);
        }
    }

    __inline void CRemoteCallback::Disconnect(void)
    {
        if(m_Callback.Disconnect)
        {
            m_Callback.Disconnect(m_pvContext);
        }
    }
}

//
// IDSREMOTEMSGHANDLER wrapper
//

namespace DSREMOTE
{
    class CRemoteMsgHandler
    {
    public:
        LIST_ENTRY              m_leHandlers;       // List entry
        DWORD                   m_dwMajorType;      // Msg major type

    private:
        DSREMOTEMSGHANDLER      m_Callback;         // Callback object
        LPVOID                  m_pvContext;        // Context

    public:
        CRemoteMsgHandler(void);
        virtual ~CRemoteMsgHandler(void);

    public:
        void Initialize(DWORD dwMajorType, LPCDSREMOTEMSGHANDLER pCallback, LPVOID pvContext);
        BOOL Receive(LPCDSRPACKET pPacketHeader, LPCDSRBUFFER pPacketBuffer, LPDSRBUFFER pResponseBuffer);
    };

    __inline CRemoteMsgHandler::CRemoteMsgHandler(void)
    {
        InitializeListHead(&m_leHandlers);
    }

    __inline CRemoteMsgHandler::~CRemoteMsgHandler(void)
    {
        AssertValidEntryList(&m_leHandlers, ASSERT_NOT_IN_LIST);
    }

    __inline void CRemoteMsgHandler::Initialize(DWORD dwMajorType, LPCDSREMOTEMSGHANDLER pCallback, LPVOID pvContext)
    {
        m_dwMajorType = dwMajorType;
    
        if(pCallback)
        {
            m_Callback = *pCallback;
        }
        else
        {
            ZeroMemory(&m_Callback, sizeof(m_Callback));
        }

        m_pvContext = pvContext;
    }

    __inline BOOL CRemoteMsgHandler::Receive(LPCDSRPACKET pPacketHeader, LPCDSRBUFFER pPacketBuffer, LPDSRBUFFER pResponseBuffer)
    {
        BOOL                    fResponse;
    
        ASSERT(pPacketHeader);
        ASSERT(pPacketBuffer);
        ASSERT(pResponseBuffer);

        ASSERT(pPacketHeader->dwMajorType == m_dwMajorType);
    
        if(m_Callback.Receive)
        {
            fResponse = m_Callback.Receive(pPacketHeader, pPacketBuffer, pResponseBuffer, m_pvContext);
        }
        else
        {
            fResponse = FALSE;
        }
    
        return fResponse;
    }
}

//
// Stiched packet wrapper
//

namespace DSREMOTE
{
    class CStitchedPacket
    {
    public:
        LIST_ENTRY              m_leStitched;       // List entry
        DSRPACKETHEADER         m_PacketHeader;     // Packet data
        LPVOID                  m_pvPacketData;     // Data buffer
        DWORD                   m_dwReceived;       // Amount of data stitched so far

    public:
        CStitchedPacket(void);
        virtual ~CStitchedPacket(void);

    public:
        HRESULT Initialize(LPCDSRPACKETHEADER pPacketHeader, PLIST_ENTRY plstStitched);
    };
}

//
// IRemoteConnection implementation
//

namespace DSREMOTE
{
    class CRemoteConnection
        : public IRemoteConnection, public CRefCount
    {
    protected:
        DWORD                   m_dwPort;           // Port assignment
        CRemoteCallback         m_StatusCallback;   // Status callback object
        LIST_ENTRY              m_lstHandlers;      // Msg handlers
        LIST_ENTRY              m_lstStitched;      // Incomplete stiched messages
        SOCKET                  m_socket;           // Transmission socket
        DWORD                   m_dwState;          // Connection state
        DWORD                   m_dwSequence;       // Current outbound message sequence number

    private:
        CRemoteFileIoHandler    m_FileIoHandler;    // Standard file i/o handler

    public:
        CRemoteConnection(void);
        virtual ~CRemoteConnection(void);

    public:
        // Initialization
        HRESULT Initialize(DWORD dwPort, LPCDSREMOTECALLBACK pCallback, LPVOID pvContext);
    
        // IUnknown methods
        ULONG STDMETHODCALLTYPE AddRef(void);
        ULONG STDMETHODCALLTYPE Release(void);

        // IRemoteConnection methods
        HRESULT STDMETHODCALLTYPE Connect(LPCSTR pszHost, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE Disconnect(DWORD dwFlags);
        DWORD STDMETHODCALLTYPE GetState(void);
        HRESULT STDMETHODCALLTYPE Send(LPCDSRPACKET pPacketHeader, DWORD dwPacketBufferCount, LPCDSRBUFFER paPacketBuffers, LPDSRBUFFER pResponseBuffer);
        HRESULT STDMETHODCALLTYPE RegisterMsgHandler(DWORD dwMajorType, LPCDSREMOTEMSGHANDLER pHandler, LPVOID pvContext);
        HRESULT STDMETHODCALLTYPE UnregisterMsgHandler(DWORD dwMajorType);
        void STDMETHODCALLTYPE DoWork(void);

    protected:
        // Socket connection
        HRESULT Listen(void);

        // Data transmission
        HRESULT SendMessage(LPCDSRPACKETHEADER pPacketHeader, DWORD dwPacketBufferCount, LPCDSRBUFFER paPacketBuffers);
        HRESULT ReceiveMessage(LPDSRPACKETHEADER pPacketHeader, LPVOID *ppvPacketData);
        HRESULT DispatchMessage(LPCDSRPACKETHEADER pPacketHeader, LPVOID *ppvPacketData);

        // WinSock wrappers
        HRESULT send(LPCVOID pvData, DWORD dwSize);
        HRESULT recv(LPVOID pvData, DWORD dwSize, LPDWORD pdwReceived = NULL);

        // Stitched packets
        HRESULT CreateStitchedPacket(LPCDSRPACKETHEADER pPacketHeader, CStitchedPacket **ppPacket);

        // Handlers
        void OnConnect(const SOCKADDR_IN &sa);

    protected:
        // Socket creation
        static HRESULT CreateSocket(SOCKET *psock);
        static void CreateAddress(LPCSTR pszAddress, DWORD dwPort, SOCKADDR_IN *pAddr);
        static void CloseSocket(SOCKET *psock);

        // Socket connection
        static HRESULT CreateConnection(SOCKET *psock, LPCSTR pszHost, DWORD dwPort, SOCKADDR_IN *pAddr);
        static HRESULT AcceptConnection(SOCKET *psock, SOCKADDR_IN *pAddr);
        static BOOL GetSocketStatus(SOCKET sock, DWORD dwAccess, DWORD dwTimeout);

        // Xbox pseudo-DNS
        static HRESULT GetHostAddress(LPCSTR pszHost, LPSTR pszAddress);

        // Checksums
        static DWORD __fastcall CreateChecksum(LPCVOID pvBuffer, DWORD dwBufferSize);
    };

    __inline ULONG CRemoteConnection::AddRef(void)
    {
        return CRefCount::AddRef();
    }

    __inline ULONG CRemoteConnection::Release(void)
    {
        return CRefCount::Release();
    }

    __inline DWORD CRemoteConnection::GetState(void)
    {
        return m_dwState;
    }
}

#endif // __cplusplus

#endif // __IREMOTE_H__
