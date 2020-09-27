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
    DWORD insertions, insertions2;
    unsigned i, j;
    unsigned port, slot;

    //Initialize core peripheral port support
    XInitDevices(0,NULL);

    pollingParameters.fAutoPoll = 1;
    pollingParameters.fInterruptOut = 0;
    pollingParameters.ReservedMBZ1 = 0;
    pollingParameters.bInputInterval = 8;  
    pollingParameters.bOutputInterval = 8;
    pollingParameters.ReservedMBZ2 = 0;

    // make sure the usb schema has not changed on us
    // ASSERT(ARRAYSIZE(XIDMasks) == XGetPortCount());
    if( ARRAYSIZE( XIDMasks ) != XGetPortCount() )
    {
        DebugPrint( "USBManager::USBManager(): XIDMasks != XGetPortCount!\n" );
        BREAK_INTO_DEBUGGER;
    }
    
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
            devices[i][0] = new DeviceDuke(port, 0, &pollingParameters); 
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
            devices[i][slot] = new DeviceMU(port, slot);
        }
    }

    
    // check for hawks
 //   insertions = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
  //  insertions2 = XGetDevices(XDEVICE_TYPE_VOICE_HEADPHONE);
 //   if(insertions == insertions2)
 //   {
        //DebugPrint("USBMGR: OK!\n");
 //   }
 //   else
 //  {
        //DebugPrint("USBMGR: CRAP!\n");
  //  }
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
            //    else if(XDEVICE_TYPE_VOICE_MICROPHONE == devices[i][j]->type) delete (DeviceHawk*)devices[i][j];
                devices[i][j] = NULL;
            }
        }
    }
}


void USBManager::CheckForHotplugs( void )
{
    DWORD addDuke, removeDuke;
    DWORD addMU, removeMU;
    DWORD addHawk, removeHawk, addHawk2, removeHawk2;
    unsigned slot, port;
    unsigned i, j;

    // check for mu removals
    XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, &addMU, &removeMU );
    for( i = 0; i < ARRAYSIZE( XMUMasks ); i++ )
    {
        slot = i < XGetPortCount() ? 1 : 2;
        port = i % XGetPortCount();
        
        if( removeMU & XMUMasks[i] )
        {
            //DebugPrint("***USBMGR: Removed XMU [%d][%d]\n", port, slot);
            delete (DeviceMU*)devices[i][slot];
            devices[i][slot] = NULL;
        }
    }

    // check for xhawk removals
  //  XGetDeviceChanges( XDEVICE_TYPE_VOICE_MICROPHONE, &addHawk, &removeHawk );
  //  XGetDeviceChanges( XDEVICE_TYPE_VOICE_HEADPHONE, &addHawk2, &removeHawk2 );
    
  //  if( ( removeHawk == removeHawk ) && ( addHawk == addHawk2 ) )
   // {
        //DebugPrint("USBMGR: OK!\n");
  //  }
   // else
   // {
        //DebugPrint("USBMGR: CRAP!\n");
   // }

    for(i = 0; i < ARRAYSIZE( XMUMasks ); i++ )
    {
        slot = i < XGetPortCount() ? 1 : 2;
        port = i % XGetPortCount();
        
       // if( removeHawk & XMUMasks[i] )
      //  {
            //DebugPrint("***USBMGR: Removed HAWK [%d][%d]\n", port, slot);
        //    delete (DeviceHawk*)devices[port][slot];
        //    devices[port][slot] = NULL;
        //}
    }

    // check for game controllers (insert & removal)
    XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &addDuke, &removeDuke );

    //DebugPrint("USBMGR: XID: %X %X\n", removals, insertions);
    for( i=0; i < XGetPortCount(); i++ )
    {
        port = i % XGetPortCount();
        
        if( removeDuke & XIDMasks[i] )
        {
            //DebugPrint("USBMGR: Removed XID [%d][0]\n", i);
            delete (DeviceDuke*)devices[i][0];
            devices[i][0] = NULL;
            /*
            // close any devices in slots
            if(devices[i][1] != INVALID_HANDLE_VALUE)
            {
                XUnmountMU(XMUPortFromDriveLetter((WCHAR)devices[i][1]), XMUSlotFromDriveLetter((WCHAR)devices[i][1]));
                devices[i][1] = INVALID_HANDLE_VALUE;
            }
            if(devices[i][2] != INVALID_HANDLE_VALUE)
            {
                XUnmountMU(XMUPortFromDriveLetter((WCHAR)devices[i][2]), XMUSlotFromDriveLetter((WCHAR)devices[i][2]));
                devices[i][2] = INVALID_HANDLE_VALUE;
            }
                */
        }
        if( addDuke & XIDMasks[i] )
        {
            //DebugPrint("USBMGR: Inserted XID [%d][0]\n", i);
            devices[i][0] = new DeviceDuke( port, 0, &pollingParameters );
        }
    }

    // check for mu insertions
    for( i = 0; i < ARRAYSIZE( XMUMasks ); i++ )
    {
        slot = i < XGetPortCount() ? 1 : 2;
        port = i % XGetPortCount();
        
        if( addMU & XMUMasks[i] )
        {
            //DebugPrint("USBMGR: Inserted XMU [%d][%d] = %wc:\\\n", port, slot, drive);
            devices[i][slot] = new DeviceMU( port, slot );
        }
    }

    // check for xhawk insertions
    for( i = 0; i < ARRAYSIZE( XMUMasks ); i++ )
    {
        slot = i < XGetPortCount() ? 1 : 2;
        port = i % XGetPortCount();
        
      //  if( addHawk & XMUMasks[i] )
      //  {
            //DebugPrint("USBMGR: Inserted XHAWK [%d][%d]\n", port, slot);
      //      devices[i][slot] = new DeviceHawk(port, slot); 
      //  }
    }
}

bool USBManager::IsControlPressed( int port, enum CONTROLS control, BOOL bFalseOnRepeat /*=false*/)
{
    // Make sure we have a duke at the current location, otherwise return false
    if( port != PORT_ANY )
    {
        if( devices[port][0] == NULL )
        {
            // DebugPrint( "USBManager::IsDPadPressed(): Requested info for non-existing Duke!\n" );
            return false;
        }
    }

    XINPUT_STATE state;
    memset( &state, 0, sizeof( XINPUT_STATE ) );

    // Check to see if the user wants to find out for ANY port
    if( port == PORT_ANY )
    {
        for( unsigned int x = 0; x < NUM_XBOX_PORTS; ++x )
        {
            // If there is no Duke at the current port, skip over it
            if( devices[x][0] == NULL )
            {
                continue;
            }

            XInputGetState( ( (DeviceDuke*)devices[x][0] )->duke, &state );

            if( state.Gamepad.wButtons & ( 1 << control ) )
            {
                // A button is pressed, check to see if it's a repeated press or a first time press,
                // if the user cares about it
                if( bFalseOnRepeat && ( true == g_ControllerState.bControlPressed[x][control] ) )
                {
                    return false;
                }
                else
                {
                    g_ControllerState.bControlPressed[x][control] = true;
                    return true;
                }
            }
            else
            {
                g_ControllerState.bControlPressed[x][control] = false;
            }

            // Clear out our state variable
            memset( &state, 0, sizeof( XINPUT_STATE ) );
        }
    }
    else // Only check one port
    {
        XInputGetState( ( (DeviceDuke*)devices[port][0] )->duke, &state );

        if( state.Gamepad.wButtons & ( 1 << control ) )
        {
            // A button is pressed, check to see if it's a repeated press or a first time press,
            // if the user cares about it
            if( bFalseOnRepeat && ( true == g_ControllerState.bControlPressed[port][control] ) )
            {
                return false;
            }
            else
            {
                g_ControllerState.bControlPressed[port][control] = true;
                return true;
            }
        }
        else
        {
            g_ControllerState.bControlPressed[port][control] = false;
        }
    }

    // If the button wasn't pressed, let's return false
    return false;
}

// Will return TRUE if a button is depressed, or FALSE if it is not
bool USBManager::IsButtonPressed( int port, enum BUTTONS button, BOOL bFalseOnRepeat /*=false*/ )
{
    // Make sure we have a duke at the current location, otherwise return false
    if( port != PORT_ANY )
    {
        if( devices[port][0] == NULL )
        {
            // DebugPrint( "USBManager::IsButtonPressed(): Requested info for non-existing Duke!\n" );
            return false;
        }
    }

    XINPUT_STATE state;
    memset( &state, 0, sizeof( XINPUT_STATE ) );

    // Check to see if the user wants to find out for ANY port
    if( port == PORT_ANY )
    {
        for( unsigned int x = 0; x < NUM_XBOX_PORTS; ++x )
        {
            // If there is no Duke in this port, skip to the next port
            if( devices[x][0] == NULL )
            {
                continue;
            }

            XInputGetState( ( (DeviceDuke*)devices[x][0] )->duke, &state );

            if( state.Gamepad.bAnalogButtons[button] )
            {
                // A button is pressed, check to see if it's a repeated press or a first time press,
                // if the user cares about it
                if( bFalseOnRepeat && ( true == g_ControllerState.bButtonPressed[x][button] ) )
                {
                    return false;
                }
                else
                {
                    g_ControllerState.bButtonPressed[x][button] = true;
                    return true;
                }
            }
            else
            {
                g_ControllerState.bButtonPressed[x][button] = false;
            }

            // Clear out our state variable
            memset( &state, 0, sizeof( XINPUT_STATE ) );
        }
    }
    else    // Check one port only
    {
        // Get the state of the Duke at the specified port
        XInputGetState( ( (DeviceDuke*)devices[port][0] )->duke, &state );

        if( state.Gamepad.bAnalogButtons[button] )
        {
            // A button is pressed, check to see if it's a repeated press or a first time press,
            // if the user cares about it
            if( bFalseOnRepeat && ( true == g_ControllerState.bButtonPressed[port][button] ) )
            {
                return false;
            }
            else
            {
                g_ControllerState.bButtonPressed[port][button] = true;
                return true;
            }
        }
        else
        {
            g_ControllerState.bButtonPressed[port][button] = false;
        }
    }

    // If the button wasn't pressed, let's return false
    return false;
}

void USBManager::ProcessInput( void )
{
    CheckForHotplugs();
}
