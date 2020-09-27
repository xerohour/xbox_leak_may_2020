/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    USBDevice.cpp

Abstract:

    This class defines the core functionality for every "device". All devices
    are children of this object.

    Defined in "USBSimulator.h"

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#include "..\inc\USBSimulator.h"


/*****************************************************************************

Routine Description:

    Default Constructor/Destructor

Arguments:

    none

Return Value:

    none

*****************************************************************************/
USBDevice::USBDevice()
    {
    simIP = 0;
    usbPort = 0;
    destPort = 0;
    sock = INVALID_SOCKET;
    record = false;
    receiveBuffer = 0;
    bufferSize = 0;
    dataToggle = false;

    timeout.tv_sec = 0;
    timeout.tv_usec = 250000;
    }

USBDevice::~USBDevice()
    {
    // make sure the thread stopped, and wait for it if it hasn't
    DWORD threadTimeout = 500;
    while(SoftBreak(threadTimeout) != TRUE)
        {
        threadTimeout *= 2;
        if(threadTimeout >= 100000)
            {
            // the thread did not respond so do the evil and terminate it
            HardBreak();
            }
        }

    // make sure the user USBSimulator.Unplug()s the device before destroying it
    _ASSERT(sock == INVALID_SOCKET);
    }




/*****************************************************************************

Routine Description:

    recv

    Override Winsock's recv to include logging

Arguments:

    see documentation on 'recv' 

Return Value:

    see documentation on 'recv' 

*****************************************************************************/
int USBDevice::recv(SOCKET s, char *buf, int len, int flags)
    {
    int ret = ::recv(s, buf, len, flags);

    if(record)
        {
        DWORD err = GetLastError();
        if(ret == SOCKET_ERROR) recorder.LogPrint("recv error: %u\r\n", err);
        else recorder.LogPacket(buf, len, (DWORD)this, false);
        SetLastError(err);
        }

    return ret;
    }


/*****************************************************************************

Routine Description:

    send

    Override Winsock's send to include logging

Arguments:

    see documentation on 'send' 

Return Value:

    see documentation on 'send' 

*****************************************************************************/
int USBDevice::send(SOCKET s, char *buf, int len, int flags)
    {
    if(record) recorder.LogPacket(buf, len, (DWORD)this, true);

    return ::send(s, buf, len, flags);
    }


/*****************************************************************************

Routine Description:

    recvfrom

    Override Winsock's recvfrom to include logging

Arguments:

    see documentation on 'recvfrom'

Return Value:

    see documentation on 'recvfrom'

*****************************************************************************/
int USBDevice::recvfrom(SOCKET s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen)
    {
    int ret = ::recvfrom(s, buf, len, flags, from, fromlen);

    if(record)
        {
        DWORD err = GetLastError();
        if(ret == SOCKET_ERROR) recorder.LogPrint("recvfrom error: %u\r\n", err);
        else recorder.LogPacket(buf, len, (DWORD)this, false);
        SetLastError(err);
        }

    return ret;
    }


/*****************************************************************************

Routine Description:

    sendto

    Override Winsock's sendto to include logging

Arguments:

    see documentation on 'sendto'

Return Value:

    see documentation on 'sendto'

*****************************************************************************/
int USBDevice::sendto(SOCKET s, char *buf, int len, int flags, struct sockaddr *to, int tolen)
    {
    int ret = ::sendto(s, buf, len, flags, to, tolen);

    if(record)
        {
        DWORD err = GetLastError();
        recorder.LogPacket(buf, len, (DWORD)this, true);
        SetLastError(err);
        }

    return ret;
    }


/*****************************************************************************

Routine Description:

    IsDataAvailable

    Checks to see if there is new data in the receive buffer

Arguments:

    none

Return Value:

    0 or SOCKET_ERROR - no data is available
    1 - data is available to recv

*****************************************************************************/
int USBDevice::IsDataAvailable(void)
    {
    FD_SET bucket;

    bucket.fd_count = 1;
    bucket.fd_array[0] = sock;

    return select(0, &bucket, NULL, NULL, &timeout);
    }


/*****************************************************************************

Routine Description:

    GetPacket

    Looks for a new data in the receive buffer, and reads just enough for 1
    packet.

Arguments:

    none

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

Notes:

    If the packet is a usb packet (SIM_CMD_USBDATA) AND its an IN packet, then
    we HAVE to look at the next packet. If the next packet is an ACK then the
    application has already responded to the IN and we DO NOT return it (but
    we do return the ACK). Otherwise (next packet was a NAK) we return the IN 
    and leave the NAK in the receive buffer.

*****************************************************************************/
DWORD USBDevice::GetPacket(void)
    {
    SOCKADDR_IN dest;
    int addressSize = sizeof(SOCKADDR);
    dest.sin_family = AF_INET;
    dest.sin_port = destPort;
    dest.sin_addr.s_addr = simIP;
    SimPacket *packet = (SimPacket*)receiveBuffer;
    int err = 0;
    unsigned long size = 0;

    for(unsigned i=0; i<USBSIM_RECV_RETRYS; i++)
        {
        err = IsDataAvailable();
        if(err == SOCKET_ERROR) return USBSIM_ERROR_SOCKET_ERROR;
        else if(err == 1)
            {
            // get the size of the packet
            ioctlsocket(sock, FIONREAD, &size);

            // assert if our receiveBuffer is not big enough!
            _ASSERT(size < bufferSize);

            // read the packet header & data
            addressSize = sizeof(SOCKADDR);
            err = recvfrom(sock, (char*)packet, size, 0, (SOCKADDR*)&dest, &addressSize);
            if(err == 0 || (err == SOCKET_ERROR && WSAGetLastError() != WSAEMSGSIZE)) return USBSIM_ERROR_SOCKET_ERROR;


            // If we get an IN, we need to determin if we have to deal with it
            // or if its already been handled. To do this, we look at the next 
            // packet for a NAK or an ACK.
            if(packet->command == SIM_CMD_USBDATA && packet->data[0] == USB_PID_IN)
                {
                err = IsDataAvailable();
                if(err != 1)
                    {
                    DebugPrint("NO ACK/NAK FOLLOWING IN!\n");
                    // assume we need to respond to it
                    }
                else
                    {
                    // get the size of the packet
                    ioctlsocket(sock, FIONREAD, &size);

                    SimPacket *packet2 = (SimPacket*)(receiveBuffer+sizeof(SimPacketHeader)+packet->dataSize);
                    addressSize = sizeof(SOCKADDR);
                    err = recvfrom(sock, (char*)packet2, size, 0, (SOCKADDR*)&dest, &addressSize);
                    if(!(err == 0 || (err == SOCKET_ERROR && GetLastError() != WSAEMSGSIZE)))
                        {
                        if(packet2->command == SIM_CMD_USBEXTRADATA && packet2->data[0] == USB_PID_ACK)
                            {
                            // eat the IN (its just info - no need to respond to it), return the ack
                            memmove(packet, packet2, sizeof(SimPacketHeader)+packet2->dataSize);
                            }
                        }
                    }
                // otherwise, IN not followed by an ACK, return the IN
                }

            return USBSIM_ERROR_OK;
            }

        if(GetExitFlag()) return USBSIM_ERROR_USER_ABORT;
        }

    return USBSIM_ERROR_TIMEOUT;
    }


/*****************************************************************************

Routine Description:

    WaitForUSBPacket

    Calls GetPacket(), if the returned packet meets the specified requirements 
    we will return that packet, otherwise we will wait around for it.

Arguments:

    int command     [in] packet type (-1 for ignore this filter)
    int subcommand  [in] packet sub type (-1 for ignore this filter)
    int pid         [in] specific value for data[0] (-1 for ignore this filter)

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

*****************************************************************************/
DWORD USBDevice::WaitForUSBPacket(int command, int subcommand, int pid)
    {
    DWORD err = 0;
    SimPacket *packet = (SimPacket*)receiveBuffer;
    USBPacket *usb = (USBPacket*)packet->data;

    for(int i=0; i<USBSIM_RECV_RETRYS; i++)
        {
        if(GetExitFlag()) return USBSIM_ERROR_USER_ABORT;
        err = GetPacket();
        if(err == USBSIM_ERROR_TIMEOUT) continue;
        else if(err != USBSIM_ERROR_OK) return err;

        if(command != -1) if(packet->command != command) continue;
        if(subcommand != -1) if(packet->subcommand != subcommand) continue;
        if(pid != -1) if(usb->pid != pid) continue;
        return USBSIM_ERROR_OK;
        }

    return USBSIM_ERROR_TIMEOUT;
    }


/*****************************************************************************

Routine Description:

    Plug

    Inserts a new device into a "port" on a simulator. Internally we connect
    to the simulator on the proper UDP port, then initialize the device and
    start it's worker thread.

Arguments:

    none

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

Notes:

    The device must not be currently attached to any other simulator!

*****************************************************************************/
DWORD USBDevice::Plug(void)
    {
    SOCKADDR_IN dest;

    if(sock != INVALID_SOCKET) return USBSIM_ERROR_CONNECTED;

    if(usbPort < 0 || usbPort > USBSIM_MAX_USBPORTS) return USBSIM_ERROR_INVALID_USB_PORT;

    // connect to the simulator
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == INVALID_SOCKET)
        {
        if(GetLastError() == 10093)
            {
            WSADATA wsaData;
            unsigned short version = MAKEWORD(2, 2);
            WSAStartup(version, &wsaData);
            if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) return USBSIM_ERROR_CONNECT_FAILED;
            }
        else return USBSIM_ERROR_CONNECT_FAILED;
        }
    dest.sin_family = AF_INET;
    dest.sin_port = htons((unsigned short)(usbPort+SIM_NETPORT_BASE+SIM_NETPORT_CLIENT_OFFSET));
    dest.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock, (SOCKADDR *)&dest, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) return USBSIM_ERROR_CONNECT_FAILED;

    // initialize settings in the device
    if(record) recorder.LogPrint("Inserted device (%s) on port %d of simulator %u.%u.%u.%u", GetName(), usbPort, ((unsigned char*)&simIP)[0], ((unsigned char*)&simIP)[1], ((unsigned char*)&simIP)[2], ((unsigned char*)&simIP)[3]);

    // send any device specific setup commands
    EndpointConfig(1);

    // Start the USB Thread that listens for commands and responds to them
    Run();

    Sleep(250);

    return USBSIM_ERROR_OK;
    }


/*****************************************************************************

Routine Description:

    Unplug

    This removes the device from the simulator and then signals the device's 
    worker thread to exit.

Arguments:

    none

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

*****************************************************************************/
DWORD USBDevice::Unplug(void)
    {
    if(sock == INVALID_SOCKET) return USBSIM_ERROR_NOTCONNECTED;

    // tell the thread to shut down
    shutdown(sock, SD_RECEIVE);
    SoftBreak(500);

    if(record) recorder.LogPrint("Removed device (%s) from port %d of simulator %u.%u.%u.%u", GetName(), usbPort, ((unsigned char*)&simIP)[0], ((unsigned char*)&simIP)[1], ((unsigned char*)&simIP)[2], ((unsigned char*)&simIP)[3]);

    return USBSIM_ERROR_OK;
    }

    
/*****************************************************************************

Routine Description:

    SetupEndpoint

    Sends a SIM_SUBCMD_SETUP_ENDPOINT packet with the specified settings for
    a given endpoint number.

Arguments:

    unsigned endpoint   [in] 0-7 specifies which endpoint we are setting
    unsigned type       [in] the type (see enum _SIM_ENDPOINT_SETUP_TYPES)
    unsigned fifoSize   [in] the size (see enum _SIM_ENDPOINT_SETUP_SIZE)
    unsigned autoRepeat [in] auto repeat (see _SIM_ENDPOINT_SETUP_AUTO_REPEAT)

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

Notes:

    The enum definitions are in "SimPacket.h"

    See document "621F01" for descriptions of the endpoint settings

*****************************************************************************/
DWORD USBDevice::SetupEndpoint(unsigned endpoint, unsigned type, unsigned fifoSize, unsigned autoRepeat)
    {
    SOCKADDR_IN dest;
    if(sock == INVALID_SOCKET) return USBSIM_ERROR_NOTCONNECTED;

    SimPacketTyped<EndpointSetup> setup;
    setup.header.command = SIM_CMD_SETUP;
    setup.header.subcommand = SIM_SUBCMD_SETUP_ENDPOINT;
    setup.header.param = (unsigned __int8)endpoint;
    setup.header.dataSize = sizeof(EndpointSetup);
    setup.data.fifoSize = (unsigned __int8)fifoSize;
    setup.data.type = (unsigned __int8)type;
    setup.data.autoRepeat = (unsigned __int8)autoRepeat;

    dest.sin_family = AF_INET;
    dest.sin_port = destPort;
    dest.sin_addr.s_addr = simIP;
    sendto(sock, (char*)&setup, sizeof(setup), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));

    return USBSIM_ERROR_OK;
    }


/*****************************************************************************

Routine Description:

    ThreadFunct

    The is the worker thread function for every device. The thread will exit 
    when this function returns.

Arguments:

    none

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

Notes:

    See CWorkerThread.h & CWorkerThread.cpp for more information on this
    member function.

*****************************************************************************/
DWORD USBDevice::ThreadFunct(void)
    {
    DWORD error = 0;

    // send the "connect" command
    SimPacketTyped<unsigned __int8[1]> connectCmd = { SIM_CMD_SETUP, SIM_SUBCMD_SETUP_CONNECT, 0, 1, 1 };
    SOCKADDR_IN dest;
    dest.sin_family = AF_INET;
    dest.sin_port = destPort;
    dest.sin_addr.s_addr = simIP;
    if(sendto(sock, (char*)&connectCmd, sizeof(connectCmd), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
        {
        return USBSIM_ERROR_SEND_FAILURE;
        }

    while(! GetExitFlag())
        {
        // check for available data
        error = GetPacket();
        if(error == USBSIM_ERROR_TIMEOUT) continue;
        else if(error != USBSIM_ERROR_OK) break;

        // Process the data
        Receive();
        }

    // send the "disconnect" command
    SimPacketTyped<unsigned __int8[1]> disconnectCmd = { SIM_CMD_SETUP, SIM_SUBCMD_SETUP_CONNECT, 0, 1, 0 };
    dest.sin_family = AF_INET;
    dest.sin_port = destPort;
    dest.sin_addr.s_addr = simIP;
    Sleep(USBSIM_UDP_DELAY);
    sendto(sock, (char*)&disconnectCmd, sizeof(disconnectCmd), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));

    // close socket
    shutdown(sock, SD_SEND);
    closesocket(sock);
    sock = INVALID_SOCKET;

    return error;
    }
