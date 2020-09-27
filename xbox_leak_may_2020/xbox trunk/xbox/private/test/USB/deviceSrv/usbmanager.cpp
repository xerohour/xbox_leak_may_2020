/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    UsbManager.cpp

Abstract:

    

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    07-20-2000  Created

Notes:
    What's inside the HANDLE from XInputOpen (HANDLE == XID_OPEN_DEVICE*):
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

#include "deviceSrv.h"
#include "usbmanager.h"

DWORD XIDMasks[] = {
    XDEVICE_PORT0_MASK,
    XDEVICE_PORT1_MASK,
    XDEVICE_PORT2_MASK,
    XDEVICE_PORT3_MASK
    };
DWORD XMUMasks[] = {
    XDEVICE_PORT0_TOP_MASK,
    XDEVICE_PORT1_TOP_MASK,
    XDEVICE_PORT2_TOP_MASK,
    XDEVICE_PORT3_TOP_MASK,
    XDEVICE_PORT0_BOTTOM_MASK,
    XDEVICE_PORT1_BOTTOM_MASK,
    XDEVICE_PORT2_BOTTOM_MASK,
    XDEVICE_PORT3_BOTTOM_MASK
    };


USBManager::USBManager()
    {
    hotplugs = true;
    DWORD insertions, insertions2;
    unsigned i, j;
    unsigned port, slot;

    pollingParameters.fAutoPoll = 1;
    pollingParameters.fInterruptOut = 0;
    pollingParameters.ReservedMBZ1 = 0;
    pollingParameters.bInputInterval = 8;  
    pollingParameters.bOutputInterval = 8;
    pollingParameters.ReservedMBZ2 = 0;

    // initialize
    for(i=0; i<XGetPortCount(); i++)
        {
        for(j=0; j<SLOT_MAX; j++)
            {
            devices[i][j] = NULL;
            packetNum[i][j] = 1234;
            }
        }

    // look for game pads
    insertions = XGetDevices(XDEVICE_TYPE_GAMEPAD);
    //DebugPrint("USBMGR: XID: - %X\n", insertions);
    for(i=0; i<XGetPortCount(); i++)
        {
        port = i%XGetPortCount();
        if(insertions & XIDMasks[i])
            {
            //DebugPrint("USBMGR: Inserted XID %d\n", i);
            devices[port][0] = new DeviceDuke(port, 0, &pollingParameters); 
            }
        }

    // check for mus
    insertions = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);
    //DebugPrint("USBMGR: XMU: - %X\n", insertions);
    for(i=0; i<ARRAYSIZE(XMUMasks); i++)
        {
        slot = i<XGetPortCount()?1:2;
        port = i%XGetPortCount();
        if(insertions & XMUMasks[i])
            {
            //DebugPrint("USBMGR: Inserted XMU %d (ec: %lu)\n", port, err);
            devices[port][slot] = new DeviceMU(port, slot);
            }
        }

    
    // check for hawks
    insertions = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
    insertions2 = XGetDevices(XDEVICE_TYPE_VOICE_HEADPHONE);
    if(insertions == insertions2)
        {
        //DebugPrint("USBMGR: OK!\n");
        }
    else
        {
        //DebugPrint("USBMGR: CRAP!\n");
        }
    //DebugPrint("USBMGR: XHAWK: %X %X\n", removals, insertions);
    for(i=0; i<ARRAYSIZE(XMUMasks); i++)
        {
        slot = i<XGetPortCount()?1:2;
        port = i%XGetPortCount();
        if(insertions & XMUMasks[i])
            {
            //DebugPrint("USBMGR: Inserted XHAWK [%d][%d]\n", port, slot);
            devices[port][slot] = new DeviceHawk(port, slot);
            }
        }
    }


USBManager::~USBManager()
    {
    unsigned i, j;

    for(i=0; i<XGetPortCount(); i++)
        {
        for(j=0; j<SLOT_MAX; j++)
            {
            if(devices[i][j] != NULL)
                {
                if(XDEVICE_TYPE_GAMEPAD == devices[i][j]->type) delete (DeviceDuke*)devices[i][j];
                else if(XDEVICE_TYPE_MEMORY_UNIT == devices[i][j]->type) delete (DeviceMU*)devices[i][j];
                else if(XDEVICE_TYPE_VOICE_MICROPHONE == devices[i][j]->type) delete (DeviceHawk*)devices[i][j];
                devices[i][j] = NULL;
                }
            }
        }
    }


void USBManager::CheckForHotplugs(void)
    {
    DWORD addDuke, removeDuke;
    DWORD addKey, removeKey;
    DWORD addMU, removeMU;
    DWORD addHawk, removeHawk, addHawk2, removeHawk2;
    unsigned slot, port;
    unsigned i;

    if(!hotplugs) return;

    // check for mu removals
    XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &addMU, &removeMU);
    for(i=0; i<ARRAYSIZE(XMUMasks); i++)
        {
        slot = i<XGetPortCount()?1:2;
        port = i%XGetPortCount();
        if(removeMU & XMUMasks[i])
            {
            delete (DeviceMU*)devices[port][slot];
            devices[port][slot] = NULL;
            }
        }

#ifdef DEBUG_KEYBOARD
    // check for keyboard removals
    XGetDeviceChanges(XDEVICE_TYPE_DEBUG_KEYBOARD, &addKey, &removeKey);
    for(i=0; i<XGetPortCount(); i++)
        {
        port = i%XGetPortCount();
        if(removeKey & XIDMasks[i])
            {
            delete (DeviceKeyboard*)devices[port][0];
            devices[port][0] = NULL;
            }
        }
#endif // DEBUG_KEYBOARD

    // check for xhawk removals
    XGetDeviceChanges(XDEVICE_TYPE_VOICE_MICROPHONE, &addHawk, &removeHawk);
    XGetDeviceChanges(XDEVICE_TYPE_VOICE_HEADPHONE, &addHawk2, &removeHawk2);
    if(removeHawk == removeHawk && addHawk == addHawk2)
        {
        //DebugPrint("USBMGR: OK!\n");
        }
    else
        {
        //DebugPrint("USBMGR: CRAP!\n");
        }
    for(i=0; i<ARRAYSIZE(XMUMasks); i++)
        {
        slot = i<XGetPortCount()?1:2;
        port = i%XGetPortCount();
        if(removeHawk & XMUMasks[i])
            {
            delete (DeviceHawk*)devices[port][slot];
            devices[port][slot] = NULL;
            }
        }


    // check for game controllers (insert & removal)
    XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &addDuke, &removeDuke);
    for(i=0; i<XGetPortCount(); i++)
        {
        port = i%XGetPortCount();
        if(removeDuke & XIDMasks[i])
            {
            delete (DeviceDuke*)devices[port][0];
            devices[port][0] = NULL;
            }
        if(addDuke & XIDMasks[i])
            {
            try
                {
                devices[port][0] = new DeviceDuke(port, 0, &pollingParameters); 
                }
            catch(...)
                {
                delete devices[port][0];
                devices[port][0] = NULL;
                }
            }
        }

#ifdef DEBUG_KEYBOARD
    // check for keyboard insertions
    for(i=0; i<XGetPortCount(); i++)
        {
        port = i%XGetPortCount();
        if(addKey & XIDMasks[i])
            {
            try
                {
                devices[port][0] = new DeviceKeyboard(port, 0, &pollingParameters); 
                }
            catch(...)
                {
                delete devices[port][0];
                devices[port][0] = NULL;
                }
            }
        }
#endif // DEBUG_KEYBOARD

    // check for mu insertions
    for(i=0; i<ARRAYSIZE(XMUMasks); i++)
        {
        slot = i<XGetPortCount()?1:2;
        port = i%XGetPortCount();
        if(addMU & XMUMasks[i])
            {
            try
                {
                devices[port][slot] = new DeviceMU(port, slot); 
                }
            catch(...)
                {
                delete devices[port][slot];
                devices[port][slot] = NULL;
                }
            }
        }

    // check for xhawk insertions
    for(i=0; i<ARRAYSIZE(XMUMasks); i++)
        {
        slot = i<XGetPortCount()?1:2;
        port = i%XGetPortCount();
        if(addHawk & XMUMasks[i])
            {
            //DebugPrint("USBMGR: Inserted XHAWK [%d][%d]\n", port, slot);
            try
                {
                // TODO 
                devices[port][slot] = new DeviceHawk(port, slot); 
                }
            catch(...)
                {
                delete devices[port][0];
                devices[port][0] = NULL;
                }
            }
        }
    }


void USBManager::ProcessInput(void)
    {
    CheckForHotplugs();
    for(unsigned i=0; i<XGetPortCount(); i++)
        {
        DumpControllerState(INVALID_SOCKET, i, 0);
        }
    }


void USBManager::ProcessInput(SOCKET sock)
    {
    CheckForHotplugs();
    for(unsigned i=0; i<XGetPortCount(); i++)
        {
        DumpControllerState(sock, i, 0);
        }
    }


void USBManager::DumpControllerState(SOCKET sock, unsigned port, unsigned slot)
    {
    if(devices[port][slot] == NULL) return;

    XINPUT_STATE state;
    memset(&state, 0, sizeof(XINPUT_STATE));

    XInputGetState(((DeviceDuke*)devices[port][slot])->duke, &state);

    //DebugPrint("Duke: 0x%X, packet id: %u\r\n", ((DeviceDuke*)devices[port][slot])->duke, state.dwPacketNumber);

    if(packetNum[port][slot] != state.dwPacketNumber)
        {
        packetNum[port][slot] = state.dwPacketNumber;

        if(sock != INVALID_SOCKET)
            {
            SockPrint(sock, "Input [%u][%u] #%lu:\r\n"
                "   dButtons = %d%d%d%d %d%d%d%d %d%d%d%d%d%d%d%d\r\n"
                "   aButtons = (%02X %02X %02X %02X %02X %02X %02X %02X)\r\n"
                "   Thumb1   = (%d, %d)\r\n"
                "   Thumb2   = (%d, %d)\r\n"
                , port, slot,
                state.dwPacketNumber,
                !!(state.Gamepad.wButtons&(1<<0)),
                !!(state.Gamepad.wButtons&(1<<1)),
                !!(state.Gamepad.wButtons&(1<<2)),
                !!(state.Gamepad.wButtons&(1<<3)),
                !!(state.Gamepad.wButtons&(1<<4)),
                !!(state.Gamepad.wButtons&(1<<5)),
                !!(state.Gamepad.wButtons&(1<<6)),
                !!(state.Gamepad.wButtons&(1<<7)),
                !!(state.Gamepad.wButtons&(1<<8)),
                !!(state.Gamepad.wButtons&(1<<9)),
                !!(state.Gamepad.wButtons&(1<<10)),
                !!(state.Gamepad.wButtons&(1<<11)),
                !!(state.Gamepad.wButtons&(1<<12)),
                !!(state.Gamepad.wButtons&(1<<13)),
                !!(state.Gamepad.wButtons&(1<<14)),
                !!(state.Gamepad.wButtons&(1<<15)),
                state.Gamepad.bAnalogButtons[0],
                state.Gamepad.bAnalogButtons[1],
                state.Gamepad.bAnalogButtons[2],
                state.Gamepad.bAnalogButtons[3],
                state.Gamepad.bAnalogButtons[4],
                state.Gamepad.bAnalogButtons[5],
                state.Gamepad.bAnalogButtons[6],
                state.Gamepad.bAnalogButtons[7],
                state.Gamepad.sThumbLX,
                state.Gamepad.sThumbLY,
                state.Gamepad.sThumbRX,
                state.Gamepad.sThumbRY
                );
            }
        }
    }

