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
    01-13-2000  Modified   JNH

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

#include "stdafx.h"
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
    XDBGWRN( APP_TITLE_NAME_A, "USBManager::USBManager()" );
    
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
        XDBGWRN( APP_TITLE_NAME_A, "USBManager::USBManager():XIDMasks != XGetPortCount!" );
        XDBGWRN( APP_TITLE_NAME_A, "USBManager::USBManager():Will break in to debugger in 60 seconds..." );

        Sleep( 60000 );
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

        memset( &m_DukeState[i], 0, sizeof( m_DukeState[i] ) );
    }

    // look for game pads
    insertions = XGetDevices(XDEVICE_TYPE_GAMEPAD);
    for(i=0; i<XGetPortCount(); i++)
    {
        port = i%XGetPortCount();
        if(insertions & XIDMasks[i])
        {
            devices[i][0] = new DeviceDuke(port, 0, &pollingParameters); 
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
                if( XDEVICE_TYPE_GAMEPAD == devices[i][j]->type )
                {
                    delete (DeviceDuke*)devices[i][j];
                }

                devices[i][j] = NULL;
            }
        }
    }
}


void USBManager::CheckForHotplugs( void )
{
    DWORD addDuke, removeDuke;
    unsigned slot, port;
    unsigned i, j;

    // check for game controllers (insert & removal)
    XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &addDuke, &removeDuke );

    for( i=0; i < XGetPortCount(); i++ )
    {
        port = i % XGetPortCount();
        
        if( removeDuke & XIDMasks[i] )
        {
            delete (DeviceDuke*)devices[i][0];
            devices[i][0] = NULL;
        }

        if( addDuke & XIDMasks[i] )
        {
            devices[i][0] = new DeviceDuke( port, 0, &pollingParameters );
        }
    }
}


// Returns the state of a controller at the specified port
XINPUT_STATE* USBManager::GetControllerState( int port )
{
    if( ( port < 0 ) || ( port >= NUM_XBOX_PORTS ) )
    {
        XDBGWRN( APP_TITLE_NAME_A, "USBManager::GetControllerState():Invalid port requested!! - '%d'", port );
        return NULL;
    }
    
    return &m_DukeState[port];
}


bool USBManager::IsControlPressed( int port, enum CONTROLS control, bool* bFirstPress /*=NULL*/)
{
    // Make sure we have a duke at the current location, otherwise return false
    if( port != PORT_ANY )
    {
        if( devices[port][SLOT_CONTROLLER] == NULL )
        {
            return false;
        }
    }

    // Check to see if the user wants to find out for ANY port
    if( port == PORT_ANY )
    {
        for( unsigned int locPort = 0; locPort < NUM_XBOX_PORTS; ++locPort )
        {
            // If there is no Duke at the current port, skip over it
            if( devices[locPort][SLOT_CONTROLLER] == NULL )
            {
                continue;
            }

            if( m_ControllerState.bControlPressed[locPort][control] )
            {
                // If the user gave us a valid address let's return
                // whether or not it's the first press of the item
                if( bFirstPress)
                {
                    // If the previous X value was also greater than the deadZone, return
                    // false for bFirstPress
                    if( m_PrevControllerState.bControlPressed[locPort][control] )
                    {
                        *bFirstPress = false;
                    }
                    else // This must be the first press
                    {
                        *bFirstPress = true;
                    }
                }

                // Return the value of the control
                // This is always true
                return m_ControllerState.bControlPressed[locPort][control];
            }
        }
    }
    else // Only check one port
    {
        if( m_ControllerState.bControlPressed[port][control] )
        {
            // If the user gave us a valid address let's return
            // whether or not it's the first press of the item
            if( bFirstPress)
            {
                // If the previous X value was also greater than the deadZone, return
                // false for bFirstPress
                if( m_PrevControllerState.bControlPressed[port][control] )
                {
                    *bFirstPress = false;
                }
                else // This must be the first press
                {
                    *bFirstPress = true;
                }
            }

            // Return the value of the control
            // This is always true
            return m_ControllerState.bControlPressed[port][control];
        }
    }

    // If the button wasn't pressed, let's return false
    return false;
}


// Will return true if any button or thumbstick has changed since last poll
bool USBManager::MovementDetected( int port, int deadZone /*=0*/ )
{
    if( port != PORT_ANY )
    {
        if( devices[port][SLOT_CONTROLLER] == NULL )
        {
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
            if( devices[x][0] == NULL )
            {
                continue;
            }

            // Get the state of the current controller
            XInputGetState( ( (DeviceDuke*)devices[x][0] )->duke, &state );

            // Check the buttons
            for( unsigned button = 0; button < BUTTON_MAX; ++button )
            {
                if( IsButtonPressed( x, (enum BUTTONS)button, TRUE_ON_REPEAT ) )
                {
                    return true;
                }
            }

            // Check the controls
            for( unsigned control = 0; control < CONTROL_MAX; ++control )
            {
                if( IsControlPressed( x, (enum CONTROLS)control, TRUE_ON_REPEAT ) )
                {
                    return true;
                }
            }

            // Check the Joysticks
            for( unsigned joystick = 0; joystick < JOYSTICK_MAX; ++joystick )
            {
                if( 0 != GetJoystickX( x, (enum JOYSTICK)joystick, TRUE_ON_REPEAT ) )
                {
                    return true;
                }

                if( 0 != GetJoystickY( x, (enum JOYSTICK)joystick, TRUE_ON_REPEAT ) )
                {
                    return true;
                }
            }
        }
    }
    else // Only one port
    {
        // Get the state of the current controller
        XInputGetState( ( (DeviceDuke*)devices[port][0])->duke, &state);

        // Check the buttons
        for( unsigned button = 0; button < BUTTON_MAX; ++button)
        {
            if( IsButtonPressed( port, (enum BUTTONS)button, TRUE_ON_REPEAT ) )
            {
                return true;
            }
        }

        // Check the controls
        for( unsigned control = 0; control < CONTROL_MAX; ++control)
        {
            if( IsControlPressed( port, (enum CONTROLS)control, TRUE_ON_REPEAT ) )
            {
                return true;
            }
        }

        // Check the Joysticks
        for( unsigned joystick = 0; joystick < JOYSTICK_MAX; ++joystick )
        {
            if( 0 != GetJoystickX( port, (enum JOYSTICK)joystick, TRUE_ON_REPEAT ) )
            {
                return true;
            }

            if( 0 != GetJoystickY( port, (enum JOYSTICK)joystick, TRUE_ON_REPEAT ) )
            {
                return true;
            }
        }
    }

    return false;
}


// Will return the X value of the joystick passed in
int USBManager::GetJoystickX( int port, enum JOYSTICK joystick, int deadZone /*=0*/, bool* bFirstPress /*=NULL*/ )
{
    // Make sure we have a duke at the current location, otherwise return 0
    if( port != PORT_ANY )
    {
        if( devices[port][SLOT_CONTROLLER] == NULL )
        {
            return 0;
        }
    }

    int nReturnVal = 0;

    // Check to see if the user wants to find out for ANY port
    if( port == PORT_ANY )
    {
        for( unsigned int locPort = 0; locPort < NUM_XBOX_PORTS; ++locPort )
        {
            // If there is no Duke in this port, skip to the next port
            if( devices[locPort][SLOT_CONTROLLER] == NULL )
            {
                continue;
            }

            // If the current stick is moved, process the request and return the value!
            if( abs( m_ControllerState.sThumbX[locPort][joystick] ) > deadZone )
            {
                // Determine if the user wishes to know if this is the first time the joystick
                // pressed, or if it's a repeat press
                if( bFirstPress)
                {
                    // If the previous Y value was also greater than the deadZone, return
                    // false for bFirstPress
                    if( abs( m_PrevControllerState.sThumbX[locPort][joystick] ) > deadZone )
                    {
                        *bFirstPress = false;
                    }
                    else // This must be the first press
                    {
                        *bFirstPress = true;
                    }
                }

                // Return the value we got from the joystick
                return m_ControllerState.sThumbX[locPort][joystick];
            }
        }
    }
    else    // Check one port only
    {
        // If the current stick is moved, process the request and return the value!
        if( abs( m_ControllerState.sThumbX[port][joystick] ) > deadZone )
        {
            // Determine if the user wishes to know if this is the first time the joystick
            // pressed, or if it's a repeat press
            if( bFirstPress)
            {
                // If the previous X value was also greater than the deadZone, return
                // false for bFirstPress
                if( abs( m_PrevControllerState.sThumbX[port][joystick] ) > deadZone )
                {
                    *bFirstPress = false;
                }
                else // This must be the first press
                {
                    *bFirstPress = true;
                }
            }

            // Return the value we got from the joystick
            return m_ControllerState.sThumbX[port][joystick];
        }
    }

    // Return 0 if the joystick was not pushed past the dead zone
    return 0;
}


// Will return the Y value of the joystick passed in
int USBManager::GetJoystickY( int port, enum JOYSTICK joystick, int deadZone /*=0*/, bool* bFirstPress /*=NULL*/ )
{
    // Make sure we have a duke at the current location, otherwise return 0
    if( port != PORT_ANY )
    {
        if( devices[port][SLOT_CONTROLLER] == NULL )
        {
            return 0;
        }
    }

    int nReturnVal = 0;

    // Check to see if the user wants to find out for ANY port
    if( port == PORT_ANY )
    {
        for( unsigned int locPort = 0; locPort < NUM_XBOX_PORTS; ++locPort )
        {
            // If there is no Duke in this port, skip to the next port
            if( devices[locPort][SLOT_CONTROLLER] == NULL )
            {
                continue;
            }

            // If the current stick is moved, process the request and return the value!
            if( abs( m_ControllerState.sThumbY[locPort][joystick] ) > deadZone )
            {
                // Determine if the user wishes to know if this is the first time the joystick
                // pressed, or if it's a repeat press
                if( bFirstPress)
                {
                    // If the previous Y value was also greater than the deadZone, return
                    // false for bFirstPress
                    if( abs( m_PrevControllerState.sThumbY[locPort][joystick] ) > deadZone )
                    {
                        *bFirstPress = false;
                    }
                    else // This must be the first press
                    {
                        *bFirstPress = true;
                    }
                }

                // Return the value we got from the joystick
                return m_ControllerState.sThumbY[locPort][joystick];
            }
        }
    }
    else    // Check one port only
    {
        // If the current stick is moved, process the request and return the value!
        if( abs( m_ControllerState.sThumbY[port][joystick] ) > deadZone )
        {
            // Determine if the user wishes to know if this is the first time the joystick
            // pressed, or if it's a repeat press
            if( bFirstPress)
            {
                // If the previous Y value was also greater than the deadZone, return
                // false for bFirstPress
                if( abs( m_PrevControllerState.sThumbY[port][joystick] ) > deadZone )
                {
                    *bFirstPress = false;
                }
                else // This must be the first press
                {
                    *bFirstPress = true;
                }
            }

            // Return the value we got from the joystick
            return m_ControllerState.sThumbY[port][joystick];
        }
    }

    // Return 0 if the joystick was not pushed past the dead zone
    return 0;
}


// Will return TRUE if a button is depressed, or FALSE if it is not
bool USBManager::IsButtonPressed( int port, enum BUTTONS button, int nThreshold /*=0*/, bool* bFirstPress /*=NULL*/ )
{
    // Make sure we have a duke at the current location, otherwise return false
    if( port != PORT_ANY )
    {
        if( devices[port][SLOT_CONTROLLER] == NULL )
        {
            return false;
        }
    }

    // Check to see if the user wants to find out for ANY port
    if( port == PORT_ANY )
    {
        for( unsigned int locPort = 0; locPort < NUM_XBOX_PORTS; ++locPort )
        {
            // If there is no Duke in this port, skip to the next port
            if( devices[locPort][SLOT_CONTROLLER] == NULL )
            {
                continue;
            }

            // If the button is depressed, determine if the user wishes to know
            // if it's a repeat press
            if( m_ControllerState.nButtonPress[locPort][button] > nThreshold )
            {
                // If the user gave us the address of a boolean variable, we should return to them
                // whether or not this is the first time the button was pressed, or if it's a repeat
                // press
                if( bFirstPress )
                {
                    // If in the previous state the button was pressed, return 'false' for bFirstPress
                    if( m_PrevControllerState.nButtonPress[locPort][button] > nThreshold )
                    {
                        *bFirstPress = false;
                    }
                    else // Otherwise, if the Prev value was false, we need to notify the user it's the first press
                    {
                        *bFirstPress = true;
                    }
                }

                // Since a button was pressed, return early out of our loop
                return true;
            }
        }
    }
    else    // Check one port only
    {
        if( m_ControllerState.nButtonPress[port][button] > nThreshold )
        {
            // If the user gave us the address of a boolean variable, we should return to them
            // whether or not this is the first time the button was pressed, or if it's a repeat
            // press
            if( bFirstPress )
            {
                // If in the previous state the button was pressed, return 'false' for bFirstPress
                if( m_PrevControllerState.nButtonPress[port][button] > nThreshold )
                {
                    *bFirstPress = false;
                }
                else // Otherwise, if the Prev value was false, we need to notify the user it's the first press
                {
                    *bFirstPress = true;
                }
            }
        }

        // Return the value of the button
        // This can be 'true' or 'false'
        return !!m_ControllerState.nButtonPress[port][button];
    }

    // If no button was pressed, let's return false;
    return false;
}

void USBManager::ProcessInput( void )
{
    // Store the previous state
    m_PrevControllerState = m_ControllerState;

    // Check to see if any devices were added or removed
    CheckForHotplugs();

    for( unsigned int port = 0; port < NUM_XBOX_PORTS; ++port )
    {
        memset( &m_DukeState[port], 0, sizeof( m_DukeState[port] ) );

        // Make sure a duke is plugged in
        if( devices[port][SLOT_CONTROLLER] == NULL )
        {
            continue;
        }

        XInputGetState( ( (DeviceDuke*)devices[port][SLOT_CONTROLLER] )->duke, &m_DukeState[port] );

        // Packet Number
        m_ControllerState.dwControllerState[port] = m_DukeState[port].dwPacketNumber;

        // Check the buttons
        for( unsigned button = 0; button < BUTTON_MAX; ++button)
        {
            m_ControllerState.nButtonPress[port][button] = m_DukeState[port].Gamepad.bAnalogButtons[button];
        }

        // Check the controls
        for( unsigned control = 0; control < CONTROL_MAX; ++control)
        {
            m_ControllerState.bControlPressed[port][control] = !!( m_DukeState[port].Gamepad.wButtons & ( 1 << control ) );
        }

        // Check the Joysticks
        for( unsigned joystick = 0; joystick < JOYSTICK_MAX; ++joystick )
        {
            m_ControllerState.sThumbX[port][joystick] = m_DukeState[port].Gamepad.sThumbLX;
            m_ControllerState.sThumbY[port][joystick] = m_DukeState[port].Gamepad.sThumbLY;
        }
    }
}
