/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    UsbManager.h

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    07-20-2000  Created

Notes:
    What's inside the HANDLE from XInputOpen (HANDLE == _XID_OPEN_DEVICE*):
        PXID_DEVICE_NODE    XidNode;
        PXID_OPEN_DEVICE    NextOpenDevice;
        LONG                OutstandingPoll;
        ULONG               PacketNumber;
        PVOID               InterruptInEndpointHandle;
        PVOID               InterruptOutEndpointHandle;
        LONG                OpenCount;
        UCHAR               Report[XID_MAXIMUM_REPORT_SIZE-2];
        UCHAR               ReportForUrb[XID_MAXIMUM_REPORT_SIZE];
        URB                 Urb;
        URB_CLOSE_ENDPOINT  CloseUrb;
        UCHAR               MdlBuffer[sizeof(MDL)+sizeof(PFN_NUMBER)*2];
        UCHAR               ClosePending:1;
        UCHAR               EndpointsOpen:1;
        UCHAR               EndpointsPendingClose:1;
        UCHAR               DeletePending:1;
        UCHAR               AutoPoll:1;
        UCHAR               Pad:3;

*/

#ifndef _USBMANAGER_H_
#define _USBMANAGER_H_

#include "stdafx.h"
#include <xbox.h>

namespace MUTests {


enum {
    SLOT_CONTROLLER,
    SLOT_TOP,
    SLOT_BOTTOM,
    SLOT_MAX
    };


class USBDevice
    {
    public:
        PXPP_DEVICE_TYPE type; // XDEVICE type

    public:
        USBDevice() { type = NULL; }
        ~USBDevice() { type = NULL; }

    public:
        virtual void Insert(unsigned port, unsigned slot) = 0;
        virtual void Remove(void) = 0;
    };

class DeviceDuke : public USBDevice
    {
    public:
        HANDLE duke;
        XINPUT_POLLING_PARAMETERS *pollingParameters;

    public:
        DeviceDuke() { type = XDEVICE_TYPE_GAMEPAD; pollingParameters = NULL; duke = NULL; }
        DeviceDuke(unsigned port, unsigned slot, XINPUT_POLLING_PARAMETERS *p) { type = XDEVICE_TYPE_GAMEPAD; pollingParameters = p; duke = NULL; Insert(port, slot); }
        ~DeviceDuke() { if(duke) Remove(); }

    public:
        void Insert(unsigned port, unsigned slot) { /*DebugPrint("USBDevice: Insert duke\n");*/ duke = XInputOpen(XDEVICE_TYPE_GAMEPAD, port, 0, pollingParameters); }
        void Remove(void) { /*DebugPrint("USBDevice: Remove duke\n");*/ XInputClose(duke); duke = 0; }
    };

class DeviceMU : public USBDevice
    {
    public:
        char drive;

    public:
        DeviceMU() { type = XDEVICE_TYPE_MEMORY_UNIT; drive = 0; }
        DeviceMU(unsigned port, unsigned slot) { type = XDEVICE_TYPE_MEMORY_UNIT; drive = 0; Insert(port, slot); }
        ~DeviceMU() { if(drive) Remove(); }

    public:
        void Insert(unsigned port, unsigned slot) { /*DebugPrint("USBDevice: Insert MU\n");*/ XMountMU(port, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT, &drive); }
        void Remove(void) { /*DebugPrint("USBDevice: Remove MU\n");*/ XUnmountMU(XMUPortFromDriveLetter(drive), XMUSlotFromDriveLetter(drive)); drive = 0; }
    };

class DeviceHawk : public USBDevice
    {
    public:
        XMediaObject *microphone;
        XMediaObject *headphone;

    public:
        DeviceHawk() { type = XDEVICE_TYPE_VOICE_MICROPHONE; microphone = headphone = NULL; }
        DeviceHawk(unsigned port, unsigned slot) { type = XDEVICE_TYPE_VOICE_MICROPHONE; microphone = headphone = NULL; Insert(port, slot); }
        ~DeviceHawk() { if(microphone || headphone) Remove(); }

    public:
        void Insert(unsigned port, unsigned slot) { /*DebugPrint("USBDevice: Insert HAWK\n"); XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, port, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT, NULL, &microphone); XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, port, slot==1?XDEVICE_TOP_SLOT:XDEVICE_BOTTOM_SLOT, NULL, &headphone); */}
        void Remove(void) { 
            /*DebugPrint("USBDevice: Remove HAWK\n");*/
            if(microphone) { DebugPrint("microphone->Release()..."); microphone->Release(); DebugPrint("...microphone->Release()\n"); } 
            if(headphone)  { DebugPrint("headphone->Release()...");  headphone->Release();  DebugPrint("...headphone->Release()\n"); } 
            microphone = headphone = NULL; 
            /*DebugPrint("USBDevice: Remove HAWK\n");*/ }
    };

class USBManager
    {
    public:
        bool hotplugs;
        USBDevice *devices[XGetPortCount()][SLOT_MAX];
        DWORD packetNum[XGetPortCount()][SLOT_MAX];
        XINPUT_POLLING_PARAMETERS pollingParameters;

    public:
        USBManager();
        ~USBManager();

    public:
        void DumpControllerState(SOCKET sock, unsigned port, unsigned slot);
        void ProcessInput(void);
        void ProcessInput(SOCKET sock);
        void CheckForHotplugs(void);

    };

} // namespace MUTests

#endif // _USBMANAGER_H_