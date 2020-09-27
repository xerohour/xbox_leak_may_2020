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
#include "stdafx.h"
#include "CSockServer.h"
#include "launcher.h"
#include "launcherx.h"

namespace Launcher {

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
    if(NULL == s)
        {
        s = m_socket;
        }

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

	while(!GetExitFlag())
        {
        if(m_socktype == SOCK_STREAM)
            {
            if(SOCKET_ERROR == listen(m_socket, 1))
                {
                DebugPrint("LAUNCHER: listen failed: ec=%lu\n", WSAGetLastError());
                break;
                }
            else
                {
                DebugPrint("LAUNCHER: Waiting for a connection on port %u...\n", GetPort());
                m_remotesocket = accept(m_socket, NULL, NULL);
                if(INVALID_SOCKET == m_remotesocket)
                    {
                    DebugPrint("LAUNCHER: connection failed: ec=%lu\n", WSAGetLastError());
                    continue;
                    }
                else
                    {
                    Parser(m_remotesocket, this);
                    CloseSocket(m_remotesocket);
                    }
                }
            }
        else // socktype == SOCK_DGRAM (broadcast monitor)
            {
            /*
            // TODO this can become a broadcast to publish our IP address
            int err;
            char buffer[1024];
            SOCKADDR_IN dest;
            int addrsize=sizeof(SOCKADDR);
            TIMEVAL timeout;
            FD_SET bucket;
            bucket.fd_count = 1;
            bucket.fd_array[0] = m_socket;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
    
            err = select(0, &bucket, NULL, NULL, &timeout);
            if(err == 0 || err == SOCKET_ERROR) continue;

            err = recvfrom(m_socket, buffer, 1024, 0, (SOCKADDR*)&dest, &addrsize);
            if(err>0) buffer[err] = '\0';
            else continue;

            char hello[32]; hello[0] = '0';
            char thekey[1024]; thekey[0] = '0';
            sscanf(buffer, "%s %[^\n\r]", hello, thekey);

            sendto(m_socket, buffer, strlen(buffer), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR));
            */
            }
        }

    return nReturnVal;
    }

} // namespace Launcher