/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    USBSimulator.cpp

Abstract:

    This object manages information on remote simulators, and allows a device
    class to attach to a given simulator.

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#include "..\inc\USBSimulator.h"
#include <malloc.h>

static const char* const recordingFormatExt[] = { "log", "utg", "xml" };
Recorder recorder; // global recording object

/*****************************************************************************

Routine Description:

    Default Constructor/Destructor

Arguments:

    none

Return Value:

    none

*****************************************************************************/
USBSimulator::USBSimulator()
    {
    WSADATA wsaData;
    unsigned short version = MAKEWORD(2, 2);
    WSAStartup(version, &wsaData);

    for(size_t i=0; i<USBSIM_MAX_SIMULATORS; i++)
        {
        ip[i] = 0;
        }
    record = false;
    }

USBSimulator::USBSimulator(bool startRecording, int recordingFormat)
    {
    WSADATA wsaData;
    unsigned short version = MAKEWORD(2, 2);
    WSAStartup(version, &wsaData);

    for(size_t i=0; i<USBSIM_MAX_SIMULATORS; i++)
        {
        ip[i] = 0;
        }
    record = startRecording;
    recorder.SetFormat(recordingFormat);

    if(record)
        {
        recorder.LogPrint("");
        recorder.LogPrint("");
        recorder.LogPrint("******************************************************************************");
        recorder.LogPrint("* Started Logfile");
        recorder.LogPrint("******************************************************************************");
        }
    }

USBSimulator::~USBSimulator()
    {
    WSACleanup();
    }


/*****************************************************************************

Routine Description:
    
    SetIP

    Sets the IP addrss on a given simulator

Arguments:

    char simulator  - the simulator to set the IP of
    char *ip        - Ipv4 textual representation of the IP address (a.b.c.d)

Return Value:

    DWORD - binary version of the IP address

*****************************************************************************/
DWORD USBSimulator::SetIP(char simulator, char *ipAddr)
    {
    return ip[simulator] = inet_addr(ipAddr);
    }


/*****************************************************************************

Routine Description:
    
    FindSimulators

    Creates a list of IP Addresses for simulator devices that are connected
    through the network.

Arguments:

    none

Return Value:

    DWORD - the number of simulators that responded to the query

Notes:


*****************************************************************************/
DWORD USBSimulator::FindSimulators(void)
    {
    DWORD count=0;
    SOCKET sock=0;
    int err=0;
    BOOL val=TRUE;
    SOCKADDR_IN dest;
    int addrsize=sizeof(SOCKADDR_IN);
    char buffer[128];
    SimPacketHeader ipQuery = { SIM_CMD_IPQUERY, 0, 0, 0 };
    SimPacket *response = (SimPacket*)buffer;

    // broadcast ip query
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    err = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&val, sizeof(BOOL));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(0);
    dest.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock, (SOCKADDR *)&dest, sizeof(SOCKADDR_IN));
    if(err == SOCKET_ERROR)
        {
        return 0;
        }
    dest.sin_port = htons(SIM_NETPORT_IPQUERY);
    dest.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    err = sendto(sock, (char*)&ipQuery, sizeof(SimPacketHeader), 0, (SOCKADDR*)&dest, addrsize);
    if(err == SOCKET_ERROR)
        {
        return 0;
        }

    TIMEVAL timeout;
    FD_SET bucket;

    for(unsigned i=0; i<USBSIM_MAX_SIMULATORS; i++)
        {
        bucket.fd_count = 1;
        bucket.fd_array[0] = sock;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        // wait for responses
        err = select(0, &bucket, NULL, NULL, &timeout);
        if(err == 0 || err == SOCKET_ERROR) break; // we hit timeout so bail

        // grab the response
        err = recvfrom(sock, buffer, 128, 0, (SOCKADDR*)&dest, &addrsize);
        if(err == 0) break; // no more data

        DebugPrint("Found Client [%u.%u.%u.%u]:\n", dest.sin_addr.S_un.S_un_b.s_b1, dest.sin_addr.S_un.S_un_b.s_b2, dest.sin_addr.S_un.S_un_b.s_b3, dest.sin_addr.S_un.S_un_b.s_b4);

        if(response->command != SIM_CMD_IPQUERY)
            {
            // error
            }
        else
            {
            ip[count] = dest.sin_addr.S_un.S_addr;
            ++count;
            }
        }

    // null out the unused spots
    for(i=count; i<USBSIM_MAX_SIMULATORS; i++)
        {
        ip[i] = 0;
        }

    // clean up
    closesocket(sock);

    return count;
    }


/*****************************************************************************

Routine Description:

    Plug

    Inserts a new device into a "port" on a simulator. Internally we connect
    to the simulator on the proper UDP port, then initialize the device and
    start it's worker thread.

Arguments:

    int port            [in] 1-4
    char simulator      [in] 0 based index of which simulator to attach to
    USBDevice *vdevice  [in] pointer to a new USBDevice class (or child)

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

Notes:

    The device must not be currently attached to any other simulator!

*****************************************************************************/
DWORD USBSimulator::Plug(int port, char simulator, USBDevice *vdevice)
    {
    SOCKADDR_IN dest;

    if(!vdevice) return USBSIM_ERROR_INVALID_DEVICE;
    if(vdevice->sock != INVALID_SOCKET) return USBSIM_ERROR_CONNECTED;

    if(simulator > USBSIM_MAX_SIMULATORS || simulator < 0 || ip[simulator] == 0) return USBSIM_ERROR_INVALID_SIMULATOR;
    if(port < 0 || port > USBSIM_MAX_USBPORTS) return USBSIM_ERROR_INVALID_USB_PORT;

    port += SIM_NETPORT_BASE;

    // connect to the simulator
    vdevice->sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(vdevice->sock == INVALID_SOCKET) return USBSIM_ERROR_CONNECT_FAILED;
    dest.sin_family = AF_INET;
    dest.sin_port = htons((unsigned short)(port+SIM_NETPORT_CLIENT_OFFSET));
    dest.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(vdevice->sock, (SOCKADDR *)&dest, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) return USBSIM_ERROR_CONNECT_FAILED;

    // initialize settings in the device
    vdevice->record = record;
    vdevice->usbPort = (char)(port-SIM_NETPORT_BASE);
    vdevice->simIP = ip[simulator];
    vdevice->destPort = htons((unsigned short)port);
    if(record) recorder.LogPrint("Inserted device (%s) on port %d of simulator %u.%u.%u.%u", vdevice->GetName(), vdevice->usbPort, ((unsigned char*)&vdevice->simIP)[0], ((unsigned char*)&vdevice->simIP)[1], ((unsigned char*)&vdevice->simIP)[2], ((unsigned char*)&vdevice->simIP)[3]);

    // send any device specific setup commands
    vdevice->EndpointConfig(1);

    // Start the USB Thread that listens for commands and responds to them
    vdevice->Run();

    return USBSIM_ERROR_OK;
    }


/*****************************************************************************

Routine Description:

    Unplug

    This removes the device from the simulator and then signals the device's 
    worker thread to exit.

Arguments:

    USBDevice *vdevice  [in] pointer to a device plugged in using Plug()

Return Value:

    DWORD   USBSIM error code (see "USBSimulator.h")

*****************************************************************************/
DWORD USBSimulator::Unplug(USBDevice *vdevice)
    {
    if(!vdevice) return USBSIM_ERROR_INVALID_DEVICE;
    if(vdevice->sock == INVALID_SOCKET) return USBSIM_ERROR_NOTCONNECTED;

    // tell the thread to shut down
    shutdown(vdevice->sock, SD_RECEIVE);
    vdevice->SoftBreak(500);

    recorder.LogPrint("Removed device (%s) from port %d of simulator %u.%u.%u.%u", vdevice->GetName(), vdevice->usbPort, ((unsigned char*)&vdevice->simIP)[0], ((unsigned char*)&vdevice->simIP)[1], ((unsigned char*)&vdevice->simIP)[2], ((unsigned char*)&vdevice->simIP)[3]);

    return USBSIM_ERROR_OK;
    }







/*****************************************************************************

Routine Description:

    Recorder

    Constructor

Arguments:

    none

Return Value:

    none

*****************************************************************************/
Recorder::Recorder()
    {
    strcpy(filename, "usbsim");
    QueryPerformanceFrequency(&counterFrequency);
    counterFrequency.QuadPart /= 1000; // convert from seconds to ms
    QueryPerformanceCounter(&startTime);
    SetFormat(RECORDING_FORMAT_TEXT_LOG);
    }


/*****************************************************************************

Routine Description:

    SetFilename

    Sets the filename and adds the proper extension based on the current 
    format.

Arguments:

    none

Return Value:

    none

*****************************************************************************/
void Recorder::SetFilename(char *name)
    {
    strcpy(filename, name);
    sprintf(fullFilename, "%s.%s", filename, recordingFormatExt[style]);
    }


/*****************************************************************************

Routine Description:

    SetFormat

    Changes the format of the log file

Arguments:

    int formatType, one of the RECORDING_FORMATS enums

Return Value:

    none

*****************************************************************************/
void Recorder::SetFormat(int formatType)
    {
    if(formatType >= ARRAYSIZE(recordingFormatExt)) return;
    style = formatType;
    sprintf(fullFilename, "%s.%s", filename, recordingFormatExt[style]);
    }


/*****************************************************************************

Routine Description:

    LogPacket

    Takes a network packet, formats it and writes it to the log.

Arguments:

    char *data      [in] pointer to the network packet
    int len         [in] size of the packet
    DWORD deviceID  [in] identifier for the usb "device"
    bool outGoing   [in] true for send (to simulator), or false for recv

Return Value:

    none

*****************************************************************************/
void Recorder::LogPacket(char *data, int len, DWORD deviceID, bool outGoing)
    {
    SimPacket *packet = (SimPacket*)data;
    USBPacket *usb = (USBPacket*)packet->data;
    char *buffer = (char*)_alloca(len*4 + 1024); // allocate space on the stack
    char *ptr = buffer;
    bool usbData = false;

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    counter.QuadPart -= startTime.QuadPart;
    counter.QuadPart /= counterFrequency.QuadPart;

    if(packet->command == SIM_CMD_USBDATA || (packet->command==SIM_CMD_USBEXTRADATA && packet->subcommand!=SIM_SUBCMD_USBEXTRADATA_RESET)) usbData = true;

    //
    // RECORDING_FORMAT_TEXT_LOG
    //
    if(style == RECORDING_FORMAT_TEXT_LOG)
        {
        ptr += sprintf(ptr, "%08I64u %08X %s // (%s, %s%s%s)\n    %d, %d, %d, %d", counter.QuadPart, deviceID, (outGoing?"[TO SIM]:  ":"[FROM SIM]:"), commandStrings[packet->command%SIM_CMD_MAX], subcommandStrings[packet->command%SIM_CMD_MAX][packet->subcommand], (usbData?": ":""), (usbData?GetPIDName(usb->pid):""), packet->command, packet->subcommand, packet->param, packet->dataSize);
        if(packet->dataSize) ptr += sprintf(ptr, "\n   ");
        for(unsigned i=0; i<packet->dataSize; i++)
            ptr += sprintf(ptr, " %02X", packet->data[i]);
        }


    //
    // RECORDING_FORMAT_TEXT_XML
    //
    else if(style == RECORDING_FORMAT_TEXT_XML)
        {
        ptr += sprintf(ptr, "<PACKET TIME='%08I64u' DIRECTION='%s'>\n", counter.QuadPart, (outGoing?"TO SIM":"FROM SIM"));
        ptr += sprintf(ptr, "    <DESCRIPTION>%s, %s%s%s</DESCRIPTION>\n", commandStrings[packet->command%SIM_CMD_MAX], subcommandStrings[packet->command%SIM_CMD_MAX][packet->subcommand], (usbData?": ":""), (usbData?GetPIDName(usb->pid):""));
        ptr += sprintf(ptr, "    <DEVICEID>%08X</DEVICEID>\n", deviceID);
        ptr += sprintf(ptr, "    <HEADER>\n");
        ptr += sprintf(ptr, "        <COMMAND>    %2d </COMMAND>\n", packet->command);
        ptr += sprintf(ptr, "        <SUBCOMMAND> %2d </SUBCOMMAND>\n", packet->subcommand);
        ptr += sprintf(ptr, "        <ENDPOINT>   %2d </ENDPOINT>\n", packet->param);
        ptr += sprintf(ptr, "        <DATASIZE>   %2d </DATASIZE>\n", packet->dataSize);
        ptr += sprintf(ptr, "        </HEADER>\n");
        ptr += sprintf(ptr, "    <DATA>\n       ");
        for(unsigned i=0; i<packet->dataSize; i++)
            ptr += sprintf(ptr, " %02X", packet->data[i]);
        ptr += sprintf(ptr, "\n        </DATA>\n");
        ptr += sprintf(ptr, "    </PACKET>");
        }


    //
    // RECORDING_FORMAT_CHIEF_UTG
    //
    else if(style == RECORDING_FORMAT_CHIEF_UTG)
        {
        if(usbData)
            {
            ptr += sprintf(ptr, "pid=%s", GetPIDName(usb->pid));
            if(usb->pid == USB_PID_SETUP || usb->pid == USB_PID_IN || usb->pid == USB_PID_OUT)
                {
                ptr += sprintf(ptr, " addr=%u endp=%u ", usb->data[0], usb->data[1]);
                }
            else if(usb->pid == USB_PID_DATA0 || usb->pid == USB_PID_DATA1 || usb->pid == USB_PID_DATA2 || usb->pid == USB_PID_MDATA)
                {
                ptr += sprintf(ptr, "\n  data=(\n   ");
                for(unsigned i=1; i<packet->dataSize; i++)
                    ptr += sprintf(ptr, " %02X", packet->data[i]);
                ptr += sprintf(ptr, "\n    )\n  ");
                }
            else ptr += sprintf(ptr, " ");
            ptr += sprintf(ptr, "eop=3 idle=TO_EOF");
            }
        else
            {
            ptr += sprintf(ptr, "; Network Command: %d, %d, %d, %d", packet->command, packet->subcommand, packet->param, packet->dataSize);
            }
        }


    FILE *f = fopen(fullFilename, "a+");
    if(f)
        {
        fprintf(f, "%s\n", buffer);
        fclose(f);
        }
    }


/*****************************************************************************

Routine Description:

    LogPrint

    Writes a formatted string (like printf) to the log.

Arguments:

    char* format    [in] format specifier
    ...             [in] arguments to the format

Return Value:

    unsigned - number of bytes written

*****************************************************************************/
unsigned Recorder::LogPrint(char* format, ...)
    {
    int bytes;
    char *buffer = (char*)_alloca(1024); // allocate space on the stack
    char *ptr = buffer;

    va_list args;
    va_start(args, format);

    if(style == RECORDING_FORMAT_TEXT_LOG)
        {
        }
    else if(style == RECORDING_FORMAT_TEXT_XML)
        {
        if(*format != '\0') ptr += sprintf(ptr, "<COMMENT><![CDATA[");
        }
    else if(style == RECORDING_FORMAT_CHIEF_UTG)
        {
        if(*format != '\0') ptr += sprintf(ptr, "; ");
        }

    // insert the formatted text
    bytes = vsprintf(ptr, format, args);
    if(bytes == -1) bytes = 0;
    ptr += bytes;

    if(style == RECORDING_FORMAT_TEXT_LOG)
        {
        }
    else if(style == RECORDING_FORMAT_TEXT_XML)
        {
        if(*format != '\0') ptr += sprintf(ptr, "]]></COMMENT>");
        }
    else if(style == RECORDING_FORMAT_CHIEF_UTG)
        {
        }

    FILE *f = fopen(fullFilename, "a+");
    if(f)
        {
        fprintf(f, "%s\n", buffer);
        fclose(f);
        }

    va_end(args);
    return (unsigned) bytes;
    }
