/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    SimPacket.h

Abstract:

    Defines the packet structures and values for the commands and subcomands
    Defines USB values

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/
#ifndef _SIMPACKET_H_
#define _SIMPACKET_H_

//
// TCP / UDP Ports
//
#define SIM_NETPORT_CLIENT_OFFSET 10
enum _SIM_NET_PORTS
    {
    SIM_NETPORT_IPQUERY = 200,
    SIM_NETPORT_BASE = 200,
    SIM_NETPORT_USB1,
    SIM_NETPORT_USB2,
    SIM_NETPORT_USB3,
    SIM_NETPORT_USB4,
    SIM_NETPORT_MAX
    };

//
// Network Packet Commands & Subcommands
//
enum _SIM_COMMANDS
    {
    SIM_CMD_USBDATA = 1,
    SIM_CMD_USBEXTRADATA,
    SIM_CMD_IPQUERY,
    SIM_CMD_STATUS,
    SIM_CMD_SETUP,
    SIM_CMD_MAX
    };
static const char * const commandStrings[SIM_CMD_MAX] = {"", "USB", "USB", "IPQUERY", "STATUS", "SETUP" };
static const char * const subcommandStrings[SIM_CMD_MAX][7] = { 
    {"", "", "", "", "", "", "" },
    {"DATA", "", "", "", "", "", "" },
    {"", "ACK/NAK", "OTHER", "RESET", "", "", "" },
    {"IPQUERY", "", "", "", "", "", "" },
    {"", "VERSION", "MACADDR", "GETCONNECTED", "", "", "" },
    {"", "ENDPOINT", "USBADDR", "CONNECT/DISCONNECT", "RESET", "SET GPIO", "GET GPIO" }
    };


// SIM CMD USBDATA
enum _SIM_USBDATA_SUBCOMMANDS
    {
    SIM_SUBCMD_USBDATA = 0
    };

// SIM CMD USBEXTRADATA
enum _SIM_USBEXTRADATA_SUBCOMMANDS
    {
    SIM_SUBCMD_USBEXTRADATA_ACKNAK = 1,
    SIM_SUBCMD_USBEXTRADATA_OTHER,
    SIM_SUBCMD_USBEXTRADATA_RESET
    };

// SIM CMD IPQUERY
enum _SIM_IPQUERY_SUBCOMMANDS
    {
    SIM_SUBCMD_IPQUERY = 0
    };

// SIM CMD STATUS
enum _SIM_STATUS_SUBCOMMANDS
    {
    SIM_SUBCMD_STATUS_VERSION = 1,
    SIM_SUBCMD_STATUS_MACADDR,
    SIM_SUBCMD_STATUS_CONNECTED
    };

// SIM CMD SETUP
enum _SIM_SETUP_SUBCOMMANDS
    {
    SIM_SUBCMD_SETUP_ENDPOINT = 1,
    SIM_SUBCMD_SETUP_USBADDRESS,
    SIM_SUBCMD_SETUP_CONNECT,
    SIM_SUBCMD_SETUP_RESET,
    SIM_SUBCMD_SETUP_SET_GPIO,
    SIM_SUBCMD_SETUP_GET_GPIO
    };

enum _SIM_USBEXTRADATA_RESET_VALUES
    {
    SIM_USB_PORT_RESUME =   0xFD,
    SIM_USB_PORT_SUSPEND =  0xFE,
    SIM_USB_PORT_RESET =    0xFF
    };

//
// Values for the EndpointSetup structure
//
enum _SIM_ENDPOINT_SETUP_TYPES
    {
    SIM_ENDPOINT_SETUP_TYPE_DISABLED = 0,
    SIM_ENDPOINT_SETUP_TYPE_ISOC,
    SIM_ENDPOINT_SETUP_TYPE_NOISOC,
    SIM_ENDPOINT_SETUP_TYPE_CONTROL
    };
enum _SIM_ENDPOINT_SETUP_SIZE
    {
    SIM_ENDPOINT_SETUP_SIZE_ISOC64 = 0,
    SIM_ENDPOINT_SETUP_SIZE_ISOC256,
    SIM_ENDPOINT_SETUP_SIZE_ISOC512,
    SIM_ENDPOINT_SETUP_SIZE_ISOC1024,

    SIM_ENDPOINT_SETUP_SIZE_NOISOC16 = 0,
    SIM_ENDPOINT_SETUP_SIZE_NOISOC64,
    SIM_ENDPOINT_SETUP_SIZE_NOISOC8,
    SIM_ENDPOINT_SETUP_SIZE_NOISOC32
    };
enum _SIM_ENDPOINT_SETUP_AUTO_REPEAT
    {
    SIM_ENDPOINT_SETUP_AUTO_DISABLED = 0,
    SIM_ENDPOINT_SETUP_AUTO_ENABLED
    };


//
// Network Packet structures
//
#pragma warning(push)
#pragma warning(disable:4200) // A structure or union contained an array with zero size
#pragma pack(push, 1) // align structure members on a byte boundry

struct SimPacketHeader
    {
    unsigned __int8  command;
    unsigned __int8  subcommand;
    unsigned __int8  param;
    unsigned __int16 dataSize;
    };

struct SimPacket : public SimPacketHeader
    {
    unsigned char data[0];
    };

template <class DataType>
struct SimPacketTyped
    {
    SimPacketHeader header;
    DataType data;
    };


//
// Specific "data" structs used in the network communication
//
struct USBPacket
    {
    unsigned __int8 pid;
    unsigned char data[0];
    };

struct EndpointSetup
    {
    unsigned __int8 fifoSize;
    unsigned __int8 type;
    unsigned __int8 autoRepeat;
    };

#pragma pack(pop)
#pragma warning(pop)






#endif //_SIMPACKET_H_