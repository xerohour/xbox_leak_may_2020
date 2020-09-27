/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    IsocDevice.cpp

Abstract:

    USB Device child class that handles Hawk enumeration and communication

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#include "..\inc\USBSimulator.h"

static const char* const DeviceName = "Isoc Device";

/*****************************************************************************

Routine Description:

    Default Constructor/Destructor

Arguments:

    none

Return Value:

    none

*****************************************************************************/
IsocDevice::IsocDevice()
    {
    for(unsigned i=0; i<10; i++)
        enumerateState[i] = 0;

    bufferSize = 9*1024;
    receiveBuffer = new char[bufferSize];
    }
IsocDevice::~IsocDevice()
    {
    delete[] receiveBuffer;
    }


/*****************************************************************************

Routine Description:

    GetName

    Returns a textual description of this device, used for logging

Arguments:

    none

Return Value:

    char*   ptr to a global string

*****************************************************************************/
const char* IsocDevice::GetName(void)
    {
    return DeviceName;
    }


/*****************************************************************************

Routine Description:

    EndpointConfig

    Configures up the Endpoint settings on the simulator

Arguments:

    config      0 - disable
                1 - normal state
                2 - repeat state
                

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

*****************************************************************************/
DWORD IsocDevice::EndpointConfig(unsigned config)
    {
    DWORD error = USBSIM_ERROR_OK;
    unsigned i;

    if(record) recorder.LogPrint("Configuring endpoints.");
    switch(config)
        {
        case 0:
            error = SetupEndpoint(0, SIM_ENDPOINT_SETUP_TYPE_DISABLED, SIM_ENDPOINT_SETUP_SIZE_NOISOC64, SIM_ENDPOINT_SETUP_AUTO_DISABLED);
            for(i=1; i<3; i++)
                {
		        Sleep(USBSIM_UDP_DELAY);
                error = SetupEndpoint(i, SIM_ENDPOINT_SETUP_TYPE_DISABLED, SIM_ENDPOINT_SETUP_SIZE_NOISOC64, SIM_ENDPOINT_SETUP_AUTO_DISABLED);
                }
            break;
        case 1:
            dataToggle = true;
            for(i=0; i<10; i++)
                enumerateState[i] = 0;
            error = SetupEndpoint(0, SIM_ENDPOINT_SETUP_TYPE_CONTROL, SIM_ENDPOINT_SETUP_SIZE_NOISOC64, SIM_ENDPOINT_SETUP_AUTO_DISABLED);
            for(i=1; i<3; i++)
                {
		        Sleep(USBSIM_UDP_DELAY);
                error = SetupEndpoint(i, SIM_ENDPOINT_SETUP_TYPE_ISOC, SIM_ENDPOINT_SETUP_SIZE_ISOC64, SIM_ENDPOINT_SETUP_AUTO_DISABLED);
                }
            break;
        case 2:
            error = SetupEndpoint(0, SIM_ENDPOINT_SETUP_TYPE_CONTROL, SIM_ENDPOINT_SETUP_SIZE_NOISOC64, SIM_ENDPOINT_SETUP_AUTO_ENABLED);
            for(i=1; i<3; i++)
                {
		        Sleep(USBSIM_UDP_DELAY);
                error = SetupEndpoint(i, SIM_ENDPOINT_SETUP_TYPE_ISOC, SIM_ENDPOINT_SETUP_SIZE_ISOC64, SIM_ENDPOINT_SETUP_AUTO_ENABLED);
                }
            break;
        default: 
            break;
        }

    return error;
    }


/*****************************************************************************

Routine Description:

    Receive

    Handles all incomming packets

Arguments:

    none

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

Notes:


*****************************************************************************/
DWORD IsocDevice::Receive(void)
    {
    SimPacket *packet = (SimPacket*)receiveBuffer;
    USBPacket *usb = (USBPacket*)packet->data;

    // USB request
    if(packet->command == SIM_CMD_USBDATA)
        {
        if(usb->pid == USB_PID_SETUP) return HandleSETUP();
        else if(usb->pid == USB_PID_OUT) return HandleOUT();
        else if(usb->pid == USB_PID_IN) return HandleIN();
        else
            {
            DebugPrint("Unexpected USB packet:\n");
            PrintPacket(packet);
            }
        }
    else if(packet->command == SIM_CMD_USBEXTRADATA)
        {
        if(packet->subcommand == SIM_SUBCMD_USBEXTRADATA_ACKNAK)
            {
            if(usb->pid == USB_PID_NAK)
                {
                DebugPrint("DETECTED MISSING IN - Responding to the NAK\n");
                return HandleIN();
                }
            return USBSIM_ERROR_OK;
            }
        else if(packet->subcommand == SIM_SUBCMD_USBEXTRADATA_RESET)
            {
            Lock();
            for(unsigned i=0; i<9; i++)
                {
                enumerateState[i] = 0;
                }
            Unlock();

            DebugPrint("Reset packet:\n");
            PrintPacket(packet);
            }
        }

    // other packets
    else
        {
        DebugPrint("Unexpected packet:\n");
        PrintPacket(packet);
        }

    return USBSIM_ERROR_OK;
    }


/*****************************************************************************

Routine Description:

    HandleSETUP

    Handles all USB SETUP packets

Arguments:

    none

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

Notes:


*****************************************************************************/
DWORD IsocDevice::HandleSETUP(void)
    {
    DWORD error = USBSIM_ERROR_OK;
    SOCKADDR_IN dest;
    SimPacket *packet = (SimPacket*)receiveBuffer;
    USBPacket *usb = (USBPacket*)packet->data;
    dest.sin_family = AF_INET;
    dest.sin_port = destPort;
    dest.sin_addr.s_addr = simIP;
    bool notdone;

    error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_DATA0);
    if(error != USBSIM_ERROR_OK) return error;

    //
    // Get Device Descriptor
    //
    if(memcmp(usb->data, "\x80\x06\x00\x01\x00\x00\x08\x00", 8) == 0)
        {
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 9;
            memcpy(packet->data, "\xD2\x12\x01\x10\x01\x00\x00\x00\x08", packet->dataSize);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            Lock();
            ++enumerateState[0];
            Unlock();
            }
        }

    //
    // Set Address
    //
    else if(memcmp(usb->data, "\x00\x05", 2) == 0 && memcmp(usb->data+3, "\x00\x00\x00\x00\x00", 5) == 0)
        {
        unsigned char addr = usb->data[2];
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 1;
            memcpy(packet->data, "\xD2", packet->dataSize);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));

            packet->command = SIM_CMD_SETUP;
            packet->subcommand = SIM_SUBCMD_SETUP_USBADDRESS;
            packet->dataSize = 1;
            packet->data[0] = addr;
            Sleep(USBSIM_UDP_DELAY);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            Lock();
            ++enumerateState[1];
            Unlock();
            }
        }

    //
    // Get Configuration Descriptor
    //
    else if(memcmp(usb->data, "\x80\x06\x00\x02\x00\x00\x50\x00", 8) == 0)
        {
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 9;
            memcpy(packet->data, "\xD2\x09\x02\x2D\x00\x02\x01\x00\x80", packet->dataSize);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            Lock();
            ++enumerateState[2];
            Unlock();
            }
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 9;
            memcpy(packet->data, "\xC3\x32\x09\x04\x00\x00\x01\x78\x02", packet->dataSize);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            Lock();
            ++enumerateState[3];
            Unlock();
            }
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 9;
            memcpy(packet->data, "\xD2\x00\x00\x09\x05\x04\x05\x32\x00", packet->dataSize);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            Lock();
            ++enumerateState[4];
            Unlock();
            }
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 9;
            memcpy(packet->data, "\xC3\x01\x00\x00\x09\x04\x01\x00\x01", packet->dataSize);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            Lock();
            ++enumerateState[5];
            Unlock();
            }
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 9;
            memcpy(packet->data, "\xD2\x78\x02\x00\x00\x09\x05\x85\x05", packet->dataSize);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            Lock();
            ++enumerateState[6];
            Unlock();
            }
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 6;
            memcpy(packet->data, "\xC3\x32\x00\x01\x00\x00", packet->dataSize);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            Lock();
            ++enumerateState[7];
            Unlock();
            }
        }

    //
    // Set Configuration
    //
    else if(memcmp(usb->data, "\x00\x09\x01\x00\x00\x00\x00\x00", 8) == 0)
        {
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 1;
            memcpy(packet->data, "\xD2", packet->dataSize);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            Lock();
            ++enumerateState[8];
            Unlock();
            }
        }

    //
    // Unknown SETUP/DATA0 packet
    //
    else
        {
        DebugPrint("Unexpected SETUP/DATA0 packet:\n");
        PrintPacket(packet);
		
        notdone = true;
        while(notdone == true)
            {
            error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
            if(error == USBSIM_ERROR_OK)
                {
                if(packet->param != 0)
                    {	// its not control pipe IN
                    HandleIN();
                    }
                else
                    {	// its on the control endpoint, handle it
                    packet->command = SIM_CMD_USBDATA;
                    packet->subcommand = SIM_SUBCMD_USBDATA;
                    packet->dataSize = 1;
                    memcpy(packet->data, "\xD2", packet->dataSize);
                    sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
                    notdone = false;
                    }
                }
            }
        }

    return error;
    }


/*****************************************************************************

Routine Description:

    HandleOUT

    Handles all USB OUT packets

Arguments:

    none

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

Notes:


*****************************************************************************/
DWORD IsocDevice::HandleOUT(void)
    {
    DWORD error = USBSIM_ERROR_OK;
    SOCKADDR_IN dest;
    SimPacket *packet = (SimPacket*)receiveBuffer;
    dest.sin_family = AF_INET;
    dest.sin_port = destPort;
    dest.sin_addr.s_addr = simIP;

    error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, -1);
    if(error != USBSIM_ERROR_OK) return error;

    //
    // 
    //
    if(packet->dataSize > 1) // Unknown OUT/DATA0 packet
        {
        DebugPrint("Unexpected OUT/DATA0 packet:\n");
        PrintPacket(packet);

        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 1;
            memcpy(packet->data, "\xD2", packet->dataSize);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        }

    return error;
    }


/*****************************************************************************

Routine Description:

    HandleIN

    Handles all USB IN packets

Arguments:

    none

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

Notes:


*****************************************************************************/
DWORD IsocDevice::HandleIN(void)
    {
    DWORD error = USBSIM_ERROR_OK;
    SOCKADDR_IN dest;
    SimPacket *packet = (SimPacket*)receiveBuffer;
    dest.sin_family = AF_INET;
    dest.sin_port = destPort;
    dest.sin_addr.s_addr = simIP;

    packet->command = SIM_CMD_USBDATA;
    packet->subcommand = SIM_SUBCMD_USBDATA;
    packet->dataSize = 1;
    memcpy(packet->data, "\xD2", packet->dataSize);
    Sleep(USBSIM_UDP_DELAY);
    sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));

    return error;
    }


/*****************************************************************************

Routine Description:


Arguments:


Return Value:


Notes:


*****************************************************************************/
bool IsocDevice::IsEnumerated(void)
    {
    bool enumed = true;
    Lock();
    for(unsigned i=0; i<9; i++)
        {
        if(enumerateState[i] == 0)
            {
            enumed = false;
            }
        }
    Unlock();

    return enumed;
    }