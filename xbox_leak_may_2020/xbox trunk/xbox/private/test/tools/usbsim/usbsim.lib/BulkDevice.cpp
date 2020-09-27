/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    BulkDevice.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#include "..\inc\USBSimulator.h"

static const char* const DeviceName = "Bulk Device";

/*****************************************************************************

Routine Description:

    Default Constructor/Destructor

Arguments:

    none

Return Value:

    none

*****************************************************************************/
BulkDevice::BulkDevice()
    {
    for(unsigned i=0; i<10; i++)
        enumerateState[i] = 0;

    storage = NULL;
    SetCapacity(0x3EFF, 0x0200, 0); // 8megs

    bufferSize = 9*1024;
    receiveBuffer = new char[bufferSize];

    memcpy(deviceDescriptor, "\x12\x01\x10\x01\x00\x00\x00\x08", 8);
    memcpy(configurationDescriptor, "\x09\x02\x20\x00\x01\x01\x00\x80\x1E\x09\x04\x00\x00\x02\x08\x42\x50\x00\x07\x05\x81\x02\x40\x00\x00\x07\x05\x02\x02\x40\x00\x00", 32);
    }
BulkDevice::~BulkDevice()
    {
    delete[] receiveBuffer;
    delete[] storage;
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
const char* BulkDevice::GetName(void)
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
DWORD BulkDevice::EndpointConfig(unsigned config)
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
DWORD BulkDevice::Receive(void)
    {
    SimPacket *packet = (SimPacket*)receiveBuffer;
    USBPacket *usb = (USBPacket*)packet->data;

    // USB request
    if(packet->command == SIM_CMD_USBDATA)
        {
        if(usb->pid == USB_PID_SETUP) return HandleSETUP();
        else if(usb->pid == USB_PID_OUT) return HandleOUT();
        else if(usb->pid == USB_PID_IN) return HandleIN();
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
            for(unsigned i=0; i<8; i++)
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
DWORD BulkDevice::HandleSETUP(void)
    {
    DWORD error = USBSIM_ERROR_OK;
    SOCKADDR_IN dest;
    SimPacket *packet = (SimPacket*)receiveBuffer;
    USBPacket *usb = (USBPacket*)packet->data;
    dest.sin_family = AF_INET;
    dest.sin_port = destPort;
    dest.sin_addr.s_addr = simIP;
    bool notdone;


    enumerateState[1] = 1;
    enumerateState[7] = 1;

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
            //Lock();
            //++enumerateState[1];
            //Unlock();
            Sleep(USBSIM_UDP_DELAY);
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
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
            packet->data[0] = 0xD2;
            Lock();
            memcpy(packet->data+1, configurationDescriptor, packet->dataSize-1);
            ++enumerateState[2];
            Unlock();
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 9;
            packet->data[0] = 0xC3;
            Lock();
            memcpy(packet->data+1, configurationDescriptor+8, packet->dataSize-1);
            ++enumerateState[3];
            Unlock();
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 9;
            packet->data[0] = 0xD2;
            Lock();
            memcpy(packet->data+1, configurationDescriptor+16, packet->dataSize-1);
            ++enumerateState[4];
            Unlock();
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 9;
            packet->data[0] = 0xC3;
            Lock();
            memcpy(packet->data+1, configurationDescriptor+24, packet->dataSize-1);
            ++enumerateState[5];
            Unlock();
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 1;
            Lock();
            memcpy(packet->data, "\xD2", packet->dataSize);
            ++enumerateState[6];
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
            //Lock();
            //++enumerateState[7];
            //Unlock();
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
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
DWORD BulkDevice::HandleOUT(void)
    {
    DWORD error = USBSIM_ERROR_OK;
    SOCKADDR_IN dest;
    SimPacket *packet = (SimPacket*)receiveBuffer;
    SimPacketTyped<CSW> csw;
    dest.sin_family = AF_INET;
    dest.sin_port = destPort;
    dest.sin_addr.s_addr = simIP;

    error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, -1);
    if(error != USBSIM_ERROR_OK) return error;

    SimPacketTyped<CBW> *cbw = (SimPacketTyped<CBW>*)packet;

    //
    // Read Capacity
    //
    if(cbw->data.opCode == 0x25)
        {
        csw.data.sig = cbw->data.sig + 0x10000000;
        csw.data.tag = cbw->data.tag;
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            dataToggle = true;
            packet->command = SIM_CMD_USBDATA;
            packet->subcommand = SIM_SUBCMD_USBDATA;
            packet->dataSize = 9;
            Lock();
            memcpy(packet->data+1, &capacity, sizeof(MUCapacity));
            Unlock();
            packet->data[0] = (unsigned char)(dataToggle ? USB_PID_DATA0 : USB_PID_DATA1);
            dataToggle = !dataToggle;
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }

        // Send CSW
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            csw.data.dataLength = 0;
            csw.data.result = 0;
            csw.header.command = SIM_CMD_USBDATA;
            csw.header.subcommand = SIM_SUBCMD_USBDATA;
            csw.header.param = packet->param;
            csw.header.dataSize = 14;
            memcpy(cbw, &csw, sizeof(csw));
            packet->data[0] = (unsigned char)(dataToggle ? USB_PID_DATA0 : USB_PID_DATA1);
            dataToggle = !dataToggle;
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        }

    //
    // Read Block
    //
    else if(cbw->data.opCode == 0x28)
        {
        csw.data.sig = cbw->data.sig + 0x10000000;
        csw.data.tag = cbw->data.tag;
        DWORD address = (DWORD)cbw->data.address*(DWORD)capacity.blockSize;
        DWORD length = cbw->data.dataLength;
        DWORD index = 0;

        for(index = 0; index < length; index+=64)
            {
            error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
            if(error == USBSIM_ERROR_OK)
                {
                dataToggle = true;
                packet->command = SIM_CMD_USBDATA;
                packet->subcommand = SIM_SUBCMD_USBDATA;
                packet->dataSize = 65;
                Lock();
                memcpy(packet->data+1, storage+address+index, 64);
                Unlock();
                packet->data[0] = (unsigned char)(dataToggle ? USB_PID_DATA0 : USB_PID_DATA1);
                dataToggle = !dataToggle;
                sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
                }
            else break;
            }

        // Send CSW
        error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            csw.data.dataLength = 0;
            csw.data.result = 0;

            /*
            // TODO BUGBUG: DONT LEAVE THIS CODE IN!
            DebugPrint("Address: %u\n", address);
            if(address == 4096)
                {
                csw.data.result = 1;
                DebugPrint("Sending CSW FAIL***********************************\n");
                }
            // END TODO BUGBUG
            */

            csw.header.command = SIM_CMD_USBDATA;
            csw.header.subcommand = SIM_SUBCMD_USBDATA;
            csw.header.param = packet->param;
            csw.header.dataSize = 14;
            memcpy(cbw, &csw, sizeof(csw));
            packet->data[0] = (unsigned char)(dataToggle ? USB_PID_DATA0 : USB_PID_DATA1);
            dataToggle = !dataToggle;
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        }

    //
    // Write Block
    //
    else if(cbw->data.opCode == 0x2A)
        {
        csw.data.sig = cbw->data.sig + 0x10000000;
        csw.data.tag = cbw->data.tag;
        DWORD address = (DWORD)cbw->data.address*(DWORD)capacity.blockSize;
        DWORD length = cbw->data.dataLength;
        DWORD index = 0;
        unsigned __int8 result = 0;

        for(index = 0; index < length; )
            {
            error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, -1);
            if(error == USBSIM_ERROR_OK)
                {
                if(packet->data[0] == USB_PID_DATA0 || packet->data[0] == USB_PID_DATA1)
                    {
                    Lock();
                    memcpy(storage+address+index, packet->data+1, 64);
                    Unlock();
                    index += 64;
                    }
                else if(packet->data[0] == USB_PID_IN)
                    {
                    error = 1;
                    break;
                    }
                }
            else break;
            }

        // Send CSW
        if(!result) error = WaitForUSBPacket(SIM_CMD_USBDATA, SIM_SUBCMD_USBDATA, USB_PID_IN);
        if(error == USBSIM_ERROR_OK)
            {
            csw.data.dataLength = 0;
            csw.data.result = result;
            csw.header.command = SIM_CMD_USBDATA;
            csw.header.subcommand = SIM_SUBCMD_USBDATA;
            csw.header.param = packet->param;
            csw.header.dataSize = 14;
            memcpy(cbw, &csw, sizeof(csw));
            packet->data[0] = (unsigned char)(dataToggle ? USB_PID_DATA0 : USB_PID_DATA1);
            dataToggle = !dataToggle;
            sendto(sock, (char*)packet, packet->dataSize+sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, sizeof(SOCKADDR_IN));
            }
        }
    else if(packet->dataSize > 1) // Unknown OUT/DATA0 packet
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
DWORD BulkDevice::HandleIN(void)
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
bool BulkDevice::IsEnumerated(void)
    {
    bool enumed = true;
    //DebugPrint("BULK: ");
    Lock();
    for(unsigned i=0; i<8; i++)
        {
        if(enumerateState[i] == 0)
            {
            //DebugPrint(" %d", i);
            enumed = false;
            }
        }
    Unlock();
    //DebugPrint("\n");

    return enumed;
    }


/*****************************************************************************

Routine Description:

    SetCapacity

    Changes the default memory unit capacity

Arguments:

    DWORD           numBlocks
    unsigned short  blockSize
    unsigned short  logicalSize

Return Value:

    none

Notes:

    default size (8meg):
        numBlocks   = 0x3EFF
        blockSize   = 0x0200
        logicalSize = 0

*****************************************************************************/
void BulkDevice::SetCapacity(DWORD numBlocks, unsigned short blockSize, unsigned short logicalSize)
    {
    Lock();
    delete[] storage;

    capacity.numBlocks = numBlocks;
    capacity.blockSize = blockSize;
    capacity.logicalSize = logicalSize;
    storage = new unsigned __int8[numBlocks * blockSize];
    memset(storage, 0xFF, numBlocks * blockSize);
    Unlock();
    }


/*****************************************************************************

Routine Description:

    StoreMU

    Write the MU data to a file

Arguments:

    char *filename

Return Value:

    none

*****************************************************************************/
DWORD BulkDevice::StoreMU(char *filename)
    {
    FILE *f = fopen(filename, "wb");
    if(!f) return GetLastError();

    Lock();
    size_t err = fwrite(storage, (unsigned int)capacity.numBlocks * (unsigned int)capacity.blockSize, 1, f);
    Unlock();

    fclose(f);

    if(err != (unsigned int)capacity.numBlocks * (unsigned int)capacity.blockSize) return GetLastError();

    return 0;
    }


/*****************************************************************************

Routine Description:

    LoadMU

    Reads the MU data from a file

Arguments:

    char *filename

Return Value:

    none

*****************************************************************************/
DWORD BulkDevice::LoadMU(char *filename)
    {
    FILE *f = fopen(filename, "rb");
    if(!f) return GetLastError();

    Lock();
    size_t err = fread(storage, (unsigned int)capacity.numBlocks * (unsigned int)capacity.blockSize, 1, f);
    Unlock();

    fclose(f);

    if(err != (unsigned int)capacity.numBlocks * (unsigned int)capacity.blockSize) return GetLastError();

    return 0;
    }
