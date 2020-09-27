/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    xinput_remote.h

Abstract:

    Remote XInput protocol declarations

--*/

#ifndef _XINPUT_REMOTE_H
#define _XINPUT_REMOTE_H

//
// XIrm packet format
//
typedef struct _XIrmPacketHeader {
    // Packet type
    DWORD type;

    #define PKTTYPE_CLIENT_INIT     1
    #define PKTTYPE_SERVER_INIT     2
    #define PKTTYPE_DEVICE_CHANGES  3
    #define PKTTYPE_STATE_CHANGES   4
    #define PKTTYPE_SET_STATE       5
    #define PKTTYPE_ACK             6
    #define PKTTYPE_KEYSTROKE       7

    // Sender sequence number
    DWORD sendseq;

    // Acknowledgement sequence number
    DWORD ackseq;
} XIrmPacketHeader;

// for PKTTYPE_CLIENT_INIT
#define XIRM_MACHINE_NAME_MAXLEN 32
#define XIRM_CLIENT_INIT_INTERVAL 1000 // msecs
typedef struct _XIrmPacketClientInit {
    XIrmPacketHeader header;
    DWORD protocol_version;
    CHAR client_name[XIRM_MACHINE_NAME_MAXLEN];
} XIrmPacketClientInit;
        
// for PKTTYPE_DEVICE_CHANGES
typedef struct _XIrmPacketDeviceChanges {
    XIrmPacketHeader header;
    DWORD keyboard_device;
    DWORD active_devices;
    DWORD insertions;
    DWORD removals;
    XINPUT_CAPABILITIES devicecaps[1];
} XIrmPacketDeviceChanges;

// for PKTTYPE_STATE_CHANGES
typedef struct _XIrmPacketStateChanges {
    XIrmPacketHeader header;
    DWORD device_masks;
    XINPUT_STATE states[1];
} XIrmPacketStateChanges;

// for PKTTYPE_SET_STATE
typedef struct _XIrmPacketSetState {
    XIrmPacketHeader header;
    DWORD port;
    XINPUT_FEEDBACK feedback;
} XIrmPacketSetState;

// for PKTTYPE_KEYSTROKE
typedef struct _XIrmPacketKeystroke {
    XIrmPacketHeader header;
    XINPUT_DEBUG_KEYSTROKE keystroke;
} XIrmPacketKeystroke;


//
// Protocol version number
//
#define XIRM_PROTOCOL_VERSION 0x00010001

//
// UDP port numbers used by XInput remote
//
#define XIRM_SERVER_PORT 8873

//
// Send retransmission timeout (in msec)
//
#define XIRM_RETRANSMIT_TIMEOUT 500
#define XIRM_RETRANSMIT_RETRY 3

//
// Send window size, i.e. number of packets that can be sent
// before we must receive an acknowledgement
//
#define XIRM_SEND_WINDOW 8

//
// Number of packets that can be received before
// we must send out an acknowledgement
//
#define XIRM_DELAY_ACKS 4

//
// Maximum packet size
//
#define XIRM_MAX_PACKET_SIZE 1024

//
// Generate a random initial sequence number
//
__inline DWORD XIrmGetISN() {
    DWORD isn;
    FILETIME filetime;

    // Get the current CPU cycle count
    __asm {
        rdtsc
        mov isn, eax
    }

    // XOR with the current time
    GetSystemTimeAsFileTime(&filetime);
    return isn ^ filetime.dwLowDateTime;
}

//
// Max number of supported devices
//
#define XIRM_PORTMAX    4   // must be a power of 2
#define XIRM_PORTSHIFT  2

// Count the number of device mask bits
__inline INT XIrmCountDevices(DWORD device_masks) {
    INT count = 0;
    DWORD i = 1 << (XIRM_PORTMAX-1);
    do {
        if (device_masks & i) count++;
        i = i >> 1;
    } while (i);
    return count;
}

#endif // !_XINPUT_REMOTE_H
