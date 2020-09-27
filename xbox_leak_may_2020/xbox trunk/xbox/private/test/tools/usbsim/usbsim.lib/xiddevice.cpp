/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    XIDDevice.cpp

Abstract:

    USB Device child class that handles XID enumeration and reporting

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#include "..\inc\USBSimulator.h"

static const char* const DeviceName = "XID Device";
const XIDInputReport defaultXIDReport = { 0, sizeof(XIDInputReport), 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0 };


/*****************************************************************************

Routine Description:

    Default Constructor/Destructor

Arguments:

    none

Return Value:

    none

*****************************************************************************/
XIDDevice::XIDDevice()
    {
    dataToggle = true;
    for(unsigned i=0; i<10; i++)
        enumerateState[i] = 0;
    bufferSize = 1024;
    receiveBuffer = new char[bufferSize];

    xidPacketLen = sizeof(XIDInputReport);
    inCapabilitiesLen = sizeof(XIDInputReport);
    outCapabilitiesLen = 6;
    xidDescriptorLen = 8;

    xidPacket = new unsigned __int8[xidPacketLen];
    inCapabilities = new unsigned __int8[inCapabilitiesLen];
    outCapabilities = new unsigned __int8[outCapabilitiesLen];
    xidDescriptor = new unsigned __int8[xidDescriptorLen];

    memcpy(deviceDescriptor, "\x12\x01\x10\x01\x00\x00\x00\x40", 8);
    memcpy(configurationDescriptor, "\x09\x02\x20\x00\x01\x01\x00\x80\x00\x09\x04\x00\x00\x02\x58\x00\x00\x00\x07\x05\x82\x03\x20\x00\x04\x07\x05\x03\x03\x20\x00\x04", 32);

    memcpy(xidPacket, &defaultXIDReport, xidPacketLen);
    memcpy(inCapabilities, "\x00\x14\xff\x00\xff\xff\xff\xff\xff\xff\xff\xff\x00\xff\x00\xff\x00\xff\x00\xff", inCapabilitiesLen);
    memcpy(outCapabilities, "\x00\x06\xFF\xFF\xFF\xFF", outCapabilitiesLen);
    memcpy(xidDescriptor, "\x08\x42\x00\x01\x01\x01\x14\x06", xidDescriptorLen);
    }
XIDDevice::~XIDDevice()
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
const char* XIDDevice::GetName(void)
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
DWORD XIDDevice::EndpointConfig(unsigned config)
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
                error = SetupEndpoint(i, SIM_ENDPOINT_SETUP_TYPE_NOISOC, SIM_ENDPOINT_SETUP_SIZE_NOISOC64, SIM_ENDPOINT_SETUP_AUTO_DISABLED);
                }
            break;
        case 2:
            error = SetupEndpoint(0, SIM_ENDPOINT_SETUP_TYPE_CONTROL, SIM_ENDPOINT_SETUP_SIZE_NOISOC64, SIM_ENDPOINT_SETUP_AUTO_ENABLED);
            for(i=1; i<3; i++)
                {
		        Sleep(USBSIM_UDP_DELAY);
                error = SetupEndpoint(i, SIM_ENDPOINT_SETUP_TYPE_NOISOC, SIM_ENDPOINT_SETUP_SIZE_NOISOC64, SIM_ENDPOINT_SETUP_AUTO_ENABLED);
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
DWORD XIDDevice::Receive(void)
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
            //DebugPrint("Unexpected USB packet:\n");
            //PrintPacket(packet);
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
            for(unsigned i=0; i<5; i++)
                {
                enumerateState[i] = 0;
                }
            Unlock();

            DebugPrint("Reset packet:\n");
            PrintPacket(packet);
            }
        else
            {
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
DWORD XIDDevice::HandleSETUP(void)
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
            packet->data[0] = 0xD2;
            Lock();
            memcpy(packet->data+1, deviceDescriptor, packet->dataSize-1);
            ++enumerateState[0];
            Unlock();
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
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
            Lock();
            ++enumerateState[1];
            Unlock();
            Sleep(USBSIM_UDP_DELAY);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        }

    //
    // Get Configuration Descriptor
    //
    else if(memcmp(usb->data, "\x80\x06\x00\x02\x00\x00", 6) == 0)
        {
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 33;
            packet->data[0] = 0xD2;
            Lock();
            memcpy(packet->data+1, configurationDescriptor, packet->dataSize-1);
            ++enumerateState[2];
            Unlock();
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
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
            ++enumerateState[3];
            Unlock();
            }
        }

    //
    // XID Descriptor
    //
    else if(memcmp(usb->data, "\xC1\x06\x00\x42\x00\x00\x10\x00", 8) == 0)
        {
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = (unsigned __int16)(xidDescriptorLen + 1);
            packet->data[0] = 0xD2;
            Lock();
            memcpy(packet->data+1, xidDescriptor, packet->dataSize-1);
            ++enumerateState[4];
            Unlock();
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        }


    //
    // XID Capabilities
    //
    else if(memcmp(usb->data, "\xC1\x01\x00", 3) == 0)
        {
        unsigned char capsize = usb->data[6];
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = (unsigned __int16)(capsize + 1);
            packet->data[0] = 0xD2;
            if(capsize == 0x06) // get capabilities for the motors
                {
                Lock();
                memcpy(packet->data+1, outCapabilities, packet->dataSize-1);
                Unlock();
                }
            else if(capsize == 0x14) // button report
                {
                Lock();
                memcpy(packet->data+1, inCapabilities, packet->dataSize-1);
                Unlock();
                }
            else
                {
                DebugPrint("Unexpected SETUP/DATA0 packet (XID Capabilities):\n");
                PrintPacket(packet);
                }
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        }


    //
    // 
    //
    else if(memcmp(usb->data, "\xA1\x01\x00\x01\x00\x00\x14\x00", 8) == 0)
        {
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            dataToggle = false;
            HandleIN();
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
DWORD XIDDevice::HandleOUT(void)
    {
    DWORD error = USBSIM_ERROR_OK;
    //SimPacket *packet = (SimPacket*)receiveBuffer;

    //error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, -1);
    //if(error != USBSIM_ERROR_OK) return error;

    //DebugPrint("Unexpected OUT packet:\n");
    //PrintPacket(packet);

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
DWORD XIDDevice::HandleIN(void)
    {
    DWORD error = USBSIM_ERROR_OK;
    SOCKADDR_IN dest;
    SimPacket *packet = (SimPacket*)receiveBuffer;
    dest.sin_family = AF_INET;
    dest.sin_port = destPort;
    dest.sin_addr.s_addr = simIP;

    packet->command = SIM_CMD_USBDATA;
    packet->subcommand = SIM_SUBCMD_USBDATA;
    packet->dataSize = (unsigned __int16)(xidPacketLen + 1);

    Lock();
    memcpy(packet->data+1, xidPacket, xidPacketLen);
    Unlock();

    packet->data[0] = (unsigned char)(dataToggle ? USB_PID_DATA0 : USB_PID_DATA1);
    dataToggle = !dataToggle;
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
DWORD XIDDevice::SetInputReport(XIDInputReport *xid)
    {
    Lock();
    if(xid)
        memcpy(xidPacket, xid, sizeof(XIDInputReport));
    else
        memcpy(xidPacket, &defaultXIDReport, sizeof(XIDInputReport));
    Unlock();

    return 0;
    }
DWORD XIDDevice::SetInputReport(unsigned __int8  reportID, unsigned __int8  size, unsigned __int16 buttons, unsigned __int8 *analogButtons, unsigned __int16 thumbLX, unsigned __int16 thumbLY, unsigned __int16 thumbRX, unsigned __int16 thumbRY)
    {
    XIDInputReport report = { reportID, size, buttons, {0, 0, 0, 0, 0, 0, 0, 0}, thumbLX, thumbLY, thumbRX, thumbRY };

    for(unsigned i=0; i<8; i++)
        report.analogButtons[i] = analogButtons[i];

    return SetInputReport(&report);
    }


/*****************************************************************************

Routine Description:


Arguments:


Return Value:


Notes:


*****************************************************************************/
bool XIDDevice::IsEnumerated(void)
    {
    Lock();
    bool enumed = true;
    for(unsigned i=0; i<5; i++)
        {
        if(enumerateState[i] == 0)
            {
            enumed = false;
            }
        }
    Unlock();

    return enumed;
    }