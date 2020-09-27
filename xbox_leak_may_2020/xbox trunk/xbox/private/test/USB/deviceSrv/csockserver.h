/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    CSockServer.h

Abstract:

    This class will allow the creation of the Winsock Server object that
    can send and receive data within it's own thread.

Author:

    Jim Helm (jimhelm)
    Josh Poley (jpoley)

Environment:

    XBox


*/
#ifndef _CSOCKSERVER_H_
#define _CSOCKSERVER_H_

#include "deviceSrv.h"
#include <winsockx.h>
#include <xnetref.h>
#include "CWorkerThread.h"

namespace DeviceServer {

class CSockServer : public CWorkerThread
    {
    private:
        SOCKET m_socket;                                        // Socket used to "listen" for connections
        SOCKET m_remotesocket;                                  // Socket returned by "accepted" connections
        sockaddr_in m_sockaddr;                                 // Our Sock Address structure
        USHORT m_port;                                          // Port of our current server
        unsigned m_uTimeoutSec;                                 // idle timeout length

        int GetSocket(int af=AF_INET,
                    int socktype=SOCK_STREAM,
                    int protocol=IPPROTO_TCP);                  // Get our socket

    public:
        int m_socktype;

    public:
        CSockServer();                                          // Constructor
        ~CSockServer();                                         // Destructor

    public:
        virtual DWORD ThreadFunct(void);                        // Thread function to handle accepting connections

    public:
        int BindSocket();                                       // Bind or Name our socket
        int OpenServerSocket(int socktype);                     // Create the socket for our server
        int CloseSocket(SOCKET s);                              // Close our socket
        BOOL IsDataAvailable(SOCKET s);

        USHORT GetPort(void) { Lock(); USHORT usTemp = m_port; Unlock(); return (usTemp); }
        void SetPort(USHORT usNewPort) { Lock(); m_port = usNewPort; Unlock(); }
        unsigned GetTimeout(void) { Lock(); unsigned usTemp = m_uTimeoutSec; Unlock(); return (usTemp); }
        void SetTimeout(unsigned timeout) { Lock(); m_uTimeoutSec = timeout; Unlock(); }
    };

} // namespace DeviceServer 

#endif // _CSOCKSERVER_H_