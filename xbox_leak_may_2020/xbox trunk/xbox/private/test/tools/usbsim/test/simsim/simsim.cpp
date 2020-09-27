// simsim.cpp : Defines the entry point for the console application.
//

#include "CSockServer.h"
#include "..\..\inc\SimPacket.h"
#include <stdio.h>

DWORD SendPacket(SOCKET sock, unsigned __int8 command, unsigned __int8 subcommand, unsigned __int8 param, unsigned __int8 dataSize, char *data);
BOOL IsDataAvailable(SOCKET s);
DWORD clientAddress;
unsigned short clientPort;
bool clientConnected = false;

int main(int argc, char* argv[])
    {
    CSockServer bcastSrv;
    bcastSrv.Init();
    bcastSrv.SetPort(SIM_NETPORT_IPQUERY);
    bcastSrv.m_socktype = SOCK_DGRAM;
    bcastSrv.OpenServerSocket(bcastSrv.m_socktype);
    bcastSrv.BindSocket();
    bcastSrv.Run();

    CSockServer deviceSrv;
    deviceSrv.Init();
    deviceSrv.SetPort(SIM_NETPORT_USB1);
    deviceSrv.m_socktype = SOCK_DGRAM;
    deviceSrv.OpenServerSocket(deviceSrv.m_socktype);
    deviceSrv.BindSocket();
    deviceSrv.Run();

    printf("Running the server. Press Enter to quit.\n");
    getchar();

    bcastSrv.CloseSocket(NULL);
    deviceSrv.CloseSocket(NULL);

    if(!bcastSrv.SoftBreak(2000)) bcastSrv.HardBreak();
    if(!deviceSrv.SoftBreak(2000)) deviceSrv.HardBreak();

    return 0;
    }


void HandleDukeStuff(SOCKET sock)
    {
    char buffer[1024];
    SimPacket *packet = (SimPacket*)buffer;
    int bytes = 0;
    printf("Device Connected on port %d. Sending Enumeration packets...\n", SIM_NETPORT_USB1);
    clientConnected = true;

    // Get Device Descriptor
    printf(" Get Device Descriptor\n");
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\xB4\x00\x00");
    printf("   SETUP (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 9, "\xC3\x80\x06\x00\x01\x00\x00\xff\x00");
    printf("   DATA0 (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x4B"); // ACK

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x00\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x00\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x00\x00"); // repeat the IN
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x4B"); // ACK

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x87\x00\x00");
    printf("   OUT   (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 1, "\xD2");
    printf("   DATA1 (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x4B"); // ACK

    // Set Address
    printf(" Set Address\n");
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\xB4\x00\x00");
    printf("   SETUP (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 9, "\xC3\x00\x05\x01\x00\x00\x00\x00\x00");
    printf("   DATA0 (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x00\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK

    // Get Configuration Descriptor
    printf(" Get Configuration Descriptor\n");
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\xB4\x02\x00");
    printf("   SETUP (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 9, "\xC3\x80\x06\x00\x02\x00\x00\x50\x00");
    printf("   DATA0 (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x02\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x02\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x02\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x02\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x02\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x02\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK

    // Set Configuration
    printf(" Set Configuration\n");
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\xB4\x02\x00");
    printf("   SETUP (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 9, "\xC3\x00\x09\x01\x00\x00\x00\x00\x00");
    printf("   DATA0 (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x02\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK

    // Get XID Descriptor
    printf(" Get XID Descriptor\n");
    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\xB4\x02\x00");
    printf("   SETUP (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 9, "\xC3\xC1\x06\x00\x41\x00\x00\x08\x00");
    printf("   DATA0 (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());

    bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x02\x00");
    printf("   IN    (%2d bytes) (ec: %u)\n", bytes, WSAGetLastError());
    bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK

    printf(" Device on port %d should be enumerated by now. Sending INs on EP 2...\n", SIM_NETPORT_USB1);
    SOCKADDR_IN dest;
    dest.sin_family = AF_INET;
    dest.sin_port = clientPort;
    dest.sin_addr.s_addr = clientAddress;
    do
        {
        bytes = SendPacket(sock, SIM_CMD_USBDATA, 0, 0, 3, "\x96\x02\x02");
        bytes = SendPacket(sock, SIM_CMD_USBEXTRADATA, 1, 0, 1, "\x5A"); // NAK
        if(bytes == 0 || bytes == SOCKET_ERROR) break;
        if(IsDataAvailable(sock))
            {
            int addressSize = sizeof(SOCKADDR);
            bytes = recvfrom(sock, buffer, 1024, 0, (SOCKADDR*)&dest, &addressSize);
            if(packet->command == SIM_CMD_SETUP && packet->subcommand == SIM_SUBCMD_SETUP_CONNECT && packet->data[0] == 0)
                {
                printf("Device Closed.\n");
                break;
                }
            if(bytes == 0 || bytes == SOCKET_ERROR)
                {
                printf("Socket Error.\n");
                break;
                }
            }
        } while(1);

    clientConnected = false;
    }


DWORD SendPacket(SOCKET sock, unsigned __int8 command, unsigned __int8 subcommand, unsigned __int8 param, unsigned __int8 dataSize, char *data)
    {
    SOCKADDR_IN dest;
    dest.sin_family = AF_INET;
    dest.sin_port = clientPort;
    dest.sin_addr.s_addr = clientAddress;

    char buffer[1024];
    SimPacket *packet = (SimPacket*)buffer;
    packet->command = command;
    packet->subcommand = subcommand;
    packet->param = param;
    packet->dataSize = dataSize;
    memcpy(packet->data, data, packet->dataSize);
    return sendto(sock, buffer, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR));
    }


BOOL IsDataAvailable(SOCKET s)
    {
    TIMEVAL timeout;
    FD_SET bucket;
    bucket.fd_count = 1;
    bucket.fd_array[0] = s;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;

    int err = select(0, &bucket, NULL, NULL, &timeout);
    if(err == 0 || err == SOCKET_ERROR)
        {
        return FALSE;
        }

    return TRUE;
    }
