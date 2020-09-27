// CSockServer.h
//
// This class will allow the creation of the Winsock Server object that
// can send and receive data within it's own thread.
//
// It is derived from a Thread class created from Josh Poley
//
// CSockServer is a modification of a class created by Jim Helm
///////////////////////////////////////////////////////////////////////
#ifndef _CSOCKSERVER_H_
#define _CSOCKSERVER_H_

#include <winsock2.h>
#include "CWorkerThread.h"

class CSockServer : public CWorkerThread
    {
    private:
        SOCKET m_socket;                                        // Socket used to "listen" for connections
        //SOCKET m_remotesocket;                                  // Socket returned by "accepted" connections
        WSADATA m_wsadata;                                      // Winsock data structure
        //LPHOSTENT m_lpHostEntry;                                // Pointer to a HOST Entry Structure
        struct sockaddr_in m_sockaddr;                          // Our Sock Address structure
        USHORT m_usPort;                                        // Port of our current server

        int GetSocket(int af=AF_INET,
                    int socktype=SOCK_STREAM,
                    int protocol=IPPROTO_TCP);                  // Get our socket
        //SOCKET GetRemoteSocket();                               // Returns the "accepted" socket from our server thread
        virtual DWORD ThreadFunct(void);                        // Thread function to handle accepting connections

    public:
        int m_socktype;
        HWND parent;
        char m_key[1024];

    public:
        CSockServer();                                          // Constructor
        ~CSockServer();                                         // Destructor
        //int GetHostByNameOrAddress(LPCSTR lpHostNameOrAddr);  // Populates the m_lpHostEntry structure
        int Init(WORD wVersionRequested=MAKEWORD(1,1));         // Initialize Windows Sockets
        int BindSocket();                                       // Bind or Name our socket
        int OpenServerSocket(int socktype);                     // Create the socket for our server
        void CloseOpenSockets();                                // Shutdown all of our open sockets
        int CloseSocket(SOCKET s);                              // Close our socket
        BOOL IsDataAvailable(SOCKET s);

        USHORT GetPort() { Lock(); USHORT usTemp = m_usPort; Unlock(); return (usTemp); }   // Returns the port number of the server
        void SetPort(USHORT usNewPort) { Lock(); m_usPort = usNewPort; Unlock(); }  // Sets the port number of the server
    };

#endif // _CSOCKSERVER_H_