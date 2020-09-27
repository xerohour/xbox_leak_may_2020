/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    Utils.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#include "..\inc\USBSimulator.h"

// USB PID names based on the lower 4 bits of the PID value 
static const char * const usbPIDStrings[] = { 
    "MDATA",
    "DATA2",
    "DATA1",
    "DATA0",
    "SETUP",
    "SOF",
    "IN",
    "OUT",
    "STALL",
    "NYET",
    "NAK",
    "ACK",
    "ERR",
    "PING",
    "SPLIT",
    "RESERVED"
    };

const char* const GetPIDName(unsigned char pid)
    {
    return usbPIDStrings[pid&0x0F];
    }

void DebugPrint(char* format, ...)
    {
    va_list args;
    va_start(args, format);

    char szBuffer[1024];

    vsprintf(szBuffer, format, args);
    OutputDebugStringA(szBuffer);

    va_end(args);
    }

void PrintPacket(SimPacket *packet)
    {
    DebugPrint("SimPacket:\n  command:    %d\n  subcommand: %d\n  datasize:   %d\n  data:       ", packet->command, packet->subcommand, packet->dataSize);
    for(unsigned i=0; i<packet->dataSize; i++)
        DebugPrint("%02X ", packet->data[i]);
    DebugPrint("\n");
    }

