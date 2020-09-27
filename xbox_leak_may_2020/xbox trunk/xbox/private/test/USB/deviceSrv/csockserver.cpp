/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    CSockServer.cpp

Abstract:

    This class will allow the creation of the Winsock Server object that
    can send and receive data within it's own thread.

Author:

    Jim Helm (jimhelm)
    Josh Poley (jpoley)

Environment:

    XBox

*/
#include "CSockServer.h"

void HandleRequest(SOCKET sock);

namespace DeviceServer {

CSockServer::CSockServer()
    {
    m_port = 20;
    m_socket = NULL;
    m_remotesocket = NULL;
    m_socktype = SOCK_STREAM;
    m_uTimeoutSec = 60;

    XNetAddRef();

    WSADATA wsaData;
    unsigned short version = MAKEWORD(2, 2);
    WSAStartup(version, &wsaData);
    }

CSockServer::~CSockServer()
    {
    CloseSocket(m_socket);
    WSACleanup();
    XNetRelease();
    }

int CSockServer::BindSocket()
    {
    int nReturnVal=0;						// Return Value for BindSocket()

    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_port = htons(GetPort());
    m_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    nReturnVal = bind(m_socket, (LPSOCKADDR)&m_sockaddr, sizeof(SOCKADDR));
    if(SOCKET_ERROR == nReturnVal)
        {
        //nReturnVal = WSAGetLastError();
        DebugPrint("BindSocket: WS Last Error : %d\n", WSAGetLastError());
        }

    return (nReturnVal);
    }


// Close our Socket
int CSockServer::CloseSocket(SOCKET s)
    {
    Lock();

    // Shutdown the socket (will fail if not connected)
    shutdown(s, SD_BOTH);

    // Close the socket
    closesocket(s);
    Unlock();

    return 0;
    }


// Get our Socket
int CSockServer::GetSocket(int af, int socktype, int protocol)
    {
    int nReturnVal=0;		// Return value for GetSocket()

    m_socket = socket(af, socktype, protocol);
    if(INVALID_SOCKET == m_socket)
        {
        //nReturnVal=WSAGetLastError();
        DebugPrint("GetSocket Last WS Error: %s\n", WSAGetLastError());
        }

    return nReturnVal;
    }


// Create our server socket
int CSockServer::OpenServerSocket(int socktype)
    {
    int nReturnVal;
    
    if(socktype == SOCK_DGRAM) nReturnVal = GetSocket(AF_INET, socktype, 0);
    else nReturnVal = GetSocket(AF_INET, socktype, IPPROTO_TCP);

    return nReturnVal;
    }

BOOL CSockServer::IsDataAvailable(SOCKET s)
    {
    TIMEVAL timeout = { m_uTimeoutSec, 0 };
    FD_SET bucket;
    bucket.fd_count = 1;
    bucket.fd_array[0] = s;

    int err = select(0, &bucket, NULL, NULL, &timeout);
    if(err == 0 || err == SOCKET_ERROR)
        {
        return FALSE;
        }

    return TRUE;
    }

// Thread code for our Winsock Server
DWORD CSockServer::ThreadFunct(void)
    {
	DWORD nReturnVal=0;				// Return Value of ThreadFunct()
    DWORD count = 0;

	while(!GetExitFlag())
        {
        if(m_socktype == SOCK_STREAM)
            {
            if(SOCKET_ERROR == listen(m_socket, 1))
                {
                DebugPrint("DEVICESRV: listen failed: ec=%lu\n", WSAGetLastError());
                break;
                }
            else
                {
                //DebugPrint("DEVICESRV: Waiting for a connection on port %u... (%u)\n", GetPort(), ++count);
                m_remotesocket = accept(m_socket, NULL, NULL);
                if(INVALID_SOCKET == m_remotesocket)
                    {
                    DebugPrint("DEVICESRV: connection failed: ec=%lu\n", WSAGetLastError());
                    continue;
                    }
                else
                    {
                    // turn off the time wait delay after closing the socket
                    linger noTimeWait = { true, 0 };
                    setsockopt(m_remotesocket, SOL_SOCKET, SO_LINGER, (char*)&noTimeWait, sizeof(noTimeWait));

                    Lock();
                    HandleRequest(m_remotesocket);
                    Unlock();

                    Sleep(25);
                    CloseSocket(m_remotesocket);
                    }
                }
            }
        }

    return nReturnVal;
    }

} // namespace DeviceServer