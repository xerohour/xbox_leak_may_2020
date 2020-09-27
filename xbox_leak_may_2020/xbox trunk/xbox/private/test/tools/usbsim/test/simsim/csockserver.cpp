// CSockServer.cpp
//
// This class will allow the creation of the Winsock Server object that
// can send and receive data within it's own thread.
//
// It is derived from a Thread class created from Josh Poley
//
// CSockServer is a modification of a class created by Jim Helm
///////////////////////////////////////////////////////////////////////
#include "CSockServer.h"

#include "..\..\inc\SimPacket.h"
void HandleDukeStuff(SOCKET sock);
#include <stdio.h>

// Bind or Name our socket
int CSockServer::BindSocket()
    {
    int nReturnVal=0;						// Return Value for BindSocket()

    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_port = htons(GetPort());
    m_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); // (u_long)0x9D370D57

    nReturnVal = bind(m_socket, (LPSOCKADDR)&m_sockaddr, sizeof(SOCKADDR));
    if(SOCKET_ERROR == nReturnVal)
        {
        //nReturnVal = GetLastError();
        //Trace("Bind failed!\n");
        //Trace("WS Last Error : %d\n", GetLastWSError());
        printf("Bind failed!\n");
        }

    return (nReturnVal);
    }


// Close our Socket
int CSockServer::CloseSocket(SOCKET s)
    {
    int nReturnVal=0;		// Return Value of CloseSocket()

    Lock();
    if(NULL == s)
        {
        s = m_socket;
        }

    // Shutdown the socket (will fail if not connected)
    nReturnVal = shutdown(s, SD_BOTH);
    if(SOCKET_ERROR == nReturnVal)
        {
        //nReturnVal = GetLastError();
        //Trace("Shutdown(s) failed!\n");
        //Trace("WS Last Error : %s\n", GetLastWSError());
        }

    // Close the socket
    nReturnVal = closesocket(s);
    if(SOCKET_ERROR == nReturnVal)
        {
        //nReturnVal = GetLastError();
        //Trace("closesocket(s) failed!\n");
        //Trace("WS Last Error : %s\n", GetLastWSError());
        }
    Unlock();

    return (nReturnVal);
    }

// Constructor
CSockServer::CSockServer()
    {
    parent = NULL;
    m_socket = NULL;
    m_socktype = SOCK_DGRAM;
    m_key[0] = '\0';
    }

// Destructor
CSockServer::~CSockServer()
    {
    CloseSocket(m_socket);
    }


// Get our Socket
int CSockServer::GetSocket(int af, int socktype, int protocol)
    {
    int nReturnVal=0;		// Return value for GetSocket()

    m_socket = socket(af, socktype, protocol);
    if(INVALID_SOCKET == m_socket)
        {
        //nReturnVal=WSAGetLastError();
        printf("socket failed: Last WS Error: %u\n", WSAGetLastError ());
        }

    return nReturnVal;
    }

// Initialize our Windows Socket
// Returns 0 if successful, non-zero if there was a problem
int CSockServer::Init(WORD wVersionRequested)
    {
    int nReturnVal=0;			// Return value of Init()

    nReturnVal = WSAStartup(wVersionRequested, &m_wsadata);

    return nReturnVal;
    }

// Create our server socket
int CSockServer::OpenServerSocket(int socktype)
    {
    int nReturnVal;			// Return value for OpenServerSocket()
    if(socktype == SOCK_DGRAM) nReturnVal = GetSocket(AF_INET, socktype, 0);
    else nReturnVal = GetSocket(AF_INET, socktype, IPPROTO_TCP);
    if(WSANOTINITIALISED == nReturnVal)
        {
        if(0 == Init())
            {
            nReturnVal = GetSocket();
            }
        }
    return nReturnVal;
    }

BOOL CSockServer::IsDataAvailable(SOCKET s)
    {
    TIMEVAL timeout;
    FD_SET bucket;
    bucket.fd_count = 1;
    bucket.fd_array[0] = s;
    timeout.tv_sec = 60;
    timeout.tv_usec = 0;

    int err = select(0, &bucket, NULL, NULL, &timeout);
    if(err == 0 || err == SOCKET_ERROR)
        {
        return FALSE;
        }

    return TRUE;
    }

extern DWORD clientAddress;
extern unsigned short clientPort;
extern bool clientConnected;

// Thread code for our Winsock Server
DWORD CSockServer::ThreadFunct(void)
    {
	DWORD nReturnVal=0;				// Return Value of ThreadFunct()
    char buffer[1024];
    SimPacket *packet = (SimPacket*)buffer;
    int err;
    SOCKADDR_IN dest;
    int addrsize=sizeof(SOCKADDR);

	while(!GetExitFlag())
        {
        if(m_usPort == SIM_NETPORT_USB1)
            {
            TIMEVAL timeout;
            FD_SET bucket;
            bucket.fd_count = 1;
            bucket.fd_array[0] = m_socket;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            err = select(0, &bucket, NULL, NULL, &timeout);
            if(err == 0 || err == SOCKET_ERROR) continue;

            err = recvfrom(m_socket, buffer, 1024, 0, (SOCKADDR*)&dest, &addrsize);
            printf("Got Packet: %d, %d, %d, %d\n", packet->command, packet->subcommand, packet->param, packet->dataSize);
            if(err>0)
                {
                if(packet->command == SIM_CMD_SETUP && packet->subcommand == SIM_SUBCMD_SETUP_CONNECT)
                    {
                    if(packet->data[0] == 1)
                        {
                        clientPort = dest.sin_port;
                        clientAddress = dest.sin_addr.S_un.S_addr;
                        printf("Client Port: %d, IP: %s\n", ntohs(clientPort), inet_ntoa(dest.sin_addr));
                        HandleDukeStuff(m_socket);
                        }
                    else if(packet->data[0] == 1) printf("Device disconnected\n");
                    }
                else
                    {
                    Sleep(10);
                    }
                }
            else
                {
                Sleep(10);
                continue;
                }
            }
        else
            {
            TIMEVAL timeout;
            FD_SET bucket;
            bucket.fd_count = 1;
            bucket.fd_array[0] = m_socket;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            err = select(0, &bucket, NULL, NULL, &timeout);
            if(err == 0 || err == SOCKET_ERROR) continue;

            err = recvfrom(m_socket, buffer, 1024, 0, (SOCKADDR*)&dest, &addrsize);
            if(err>0)
                {
                buffer[err] = '\0';
                }
            else
                {
                Sleep(10);
                continue;
                }

            if(packet->command == SIM_CMD_IPQUERY)
                {
                printf("Received IP Query request, sending response\n");
                packet->subcommand = 0;
                packet->dataSize = 4;
                packet->data[0] = 1;
                packet->data[1] = 2;
                packet->data[2] = 3;
                packet->data[3] = 4;
                sendto(m_socket, buffer, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR));
                }
            else if(packet->command == SIM_CMD_STATUS && packet->subcommand == SIM_SUBCMD_STATUS_CONNECTED)
                {
                printf("Received Device Status command, sending response\n");
                packet->dataSize = 1;
                packet->data[0] = clientConnected ? 1 : 0;
                sendto(m_socket, buffer, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR));
                }
            else if(packet->command == SIM_CMD_STATUS && packet->subcommand == SIM_SUBCMD_STATUS_VERSION)
                {
                printf("Received Version command, sending response\n");
                strcpy((char*)packet->data, "SimSim v 1.1");
                packet->dataSize = strlen((char*)packet->data)+1;
                sendto(m_socket, buffer, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR));
                }
            else
                {
                printf("Received unknown command/subcommand: %u, %u\n", packet->command, packet->subcommand);
                }
            }
        }

    return nReturnVal;
    }