/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    UsbManager.cpp

Abstract:


Author:

    Josh Poley (jpoley)

Environment:

    Xbox

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

#include "usbpch.h"
#include "usbmanager.h"

// Threshold for analog buttons
#define XINPUT_ANALOGBUTTONTHRESHOLD 32

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
    DWORD insertions;
    // DWORD insertions2;
    unsigned i, j;
    unsigned port;
    // unsigned slot;

    //Initialize core peripheral port support
    XInitDevices( 0, NULL );

    pollingParameters.fAutoPoll       = 1;
    pollingParameters.fInterruptOut   = 0;
    pollingParameters.ReservedMBZ1    = 0;
    pollingParameters.bInputInterval  = 8;
    pollingParameters.bOutputInterval = 8;
    pollingParameters.ReservedMBZ2    = 0;

    if( ARRAYSIZE( XIDMasks ) != XGetPortCount() )
    {
        XDBGWRN( USBM_APP_TITLE_NAME_A, "USBManager::USBManager():XIDMasks != XGetPortCount!!" );
        XDBGWRN( USBM_APP_TITLE_NAME_A, "USBManager::USBManager():Will break in to debugger in 60 seconds..." );

        Sleep( 60000 );
        BREAK_INTO_DEBUGGER;
    }

    // initialize
    for( i = 0; i < XGetPortCount(); ++i )
    {
        for( j = 0; j < SLOT_MAX; ++j )
        {
            devices[i][j] = NULL;
            packetNum[i][j] = 1234;
        }
    }

    // look for game pads
    insertions = XGetDevices( XDEVICE_TYPE_GAMEPAD );
    for( i = 0; i < XGetPortCount(); ++i )
    {
        port = i % XGetPortCount();
        if( insertions & XIDMasks[i] )
        {
            XDBGTRC( USBM_APP_TITLE_NAME_A, "USBManager::USBManager():GAMEPAD found on port - '%d'", port );

            devices[i][0] = new DeviceDuke(port, 0, &pollingParameters);

            // Make sure there wasn't a problem inserting the Duke
            if( !((DeviceDuke*)devices[i][0])->m_hDuke )
            {
                delete (DeviceDuke*)devices[i][0];
                devices[i][0] = NULL;
            }
        }
    }

    // look for remote control
    insertions = XGetDevices( XDEVICE_TYPE_IR_REMOTE );
    for( i = 0; i < XGetPortCount(); ++i )
    {
        port = i % XGetPortCount();
        if( insertions & XIDMasks[i] )
        {
            XDBGTRC( USBM_APP_TITLE_NAME_A, "USBManager::USBManager():REMOTE found on port - '%d'", port );
            devices[i][0] = new DeviceRemote( port, 0 );

            // Make sure there wasn't a problem inserting the Remote
            if( !((DeviceRemote*)devices[i][0])->m_hRemote )
            {
                XDBGWRN( USBM_APP_TITLE_NAME_A, "USBManager::USBManager():REMOTE insert problem on port - '%d'", port );

                delete (DeviceRemote*)devices[i][0];
                devices[i][0] = NULL;
            }
        }
    }

    // Initialize our Thread Handles (for our Rumble Thread)
    for( i = 0; i < 4; i++ )
    {
        m_hThreads[i] = NULL;
    }

    ZeroMemory( &m_RumbleInfo[0], sizeof( m_RumbleInfo[0] ) * 4 );
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
                if( XDEVICE_TYPE_GAMEPAD == devices[i][j]->m_type )
                {
                    XDBGTRC( USBM_APP_TITLE_NAME_A, "USBManager::~USBManager():GAMEPAD being deleted from port - '%d'", i );

                    delete (DeviceDuke*)devices[i][j];
                }
                else if( XDEVICE_TYPE_IR_REMOTE == devices[i][j]->m_type )
                {
                    XDBGTRC( USBM_APP_TITLE_NAME_A, "USBManager::~USBManager():REMOTE being deleted from port - '%d'", i );

                    delete (DeviceRemote*)devices[i][j];
                }

                devices[i][j] = NULL;
            }
        }
    }
}


void USBManager::CheckForHotplugs( void )
{
    DWORD addDuke, removeDuke;
    DWORD addRemote, removeRemote;
    // unsigned slot;
    unsigned port;
    unsigned i;

    // check for game controllers (insert & removal)
    XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &addDuke, &removeDuke );

    for( i=0; i < XGetPortCount(); i++ )
    {
        port = i % XGetPortCount();

        if( removeDuke & XIDMasks[i] )
        {
            XDBGTRC( USBM_APP_TITLE_NAME_A, "USBManager::USBManager():GAMEPAD removed from port - '%d'", port );

            delete (DeviceDuke*)devices[i][0];
            devices[i][0] = NULL;
        }

        if( addDuke & XIDMasks[i] )
        {
            XDBGTRC( USBM_APP_TITLE_NAME_A, "USBManager::USBManager():GAMEPAD added to port - '%d'", port );

            devices[i][0] = new DeviceDuke( port, 0, &pollingParameters );

            // Make sure there wasn't a problem inserting the Duke
            if( !((DeviceDuke*)devices[i][0])->m_hDuke )
            {
                delete (DeviceDuke*)devices[i][0];
                devices[i][0] = NULL;
            }
        }
    }

    // Check for Remote Control (insert & removal)
    XGetDeviceChanges( XDEVICE_TYPE_IR_REMOTE, &addRemote, &removeRemote );

    for( i=0; i < XGetPortCount(); i++ )
    {
        port = i % XGetPortCount();

        if( removeRemote & XIDMasks[i] )
        {
            XDBGTRC( USBM_APP_TITLE_NAME_A, "USBManager::USBManager():REMOTE removed from port - '%d'", port );

            delete (DeviceRemote*)devices[i][0];
            devices[i][0] = NULL;
        }

        if( addRemote & XIDMasks[i] )
        {
            XDBGTRC( USBM_APP_TITLE_NAME_A, "USBManager::USBManager():REMOTE found on port - '%d'", i );

            devices[i][0] = new DeviceRemote( port, 0 );

            // Make sure there wasn't a problem inserting the Duke
            if( !((DeviceRemote*)devices[i][0])->m_hRemote )
            {
                XDBGWRN( USBM_APP_TITLE_NAME_A, "USBManager::USBManager():REMOTE insert problem on port - '%d'", i );

                delete (DeviceRemote*)devices[i][0];
                devices[i][0] = NULL;
            }
        }
    }
}

BOOL USBManager::IsControlPressed( int port, enum CONTROLS control, BOOL* bFirstPress /*=NULL*/)
{
    // Make sure we have a duke at the current location, otherwise return FALSE
    if( port != PORT_ANY )
    {
        if( ( devices[port][SLOT_CONTROLLER] == NULL ) || ( devices[port][SLOT_CONTROLLER]->m_type != XDEVICE_TYPE_GAMEPAD ) )
        {
            return FALSE;
        }

        if( m_ControllerState.bControlPressed[port][control] )
        {
            // If the user gave us a valid address let's return
            // whether or not it's the first press of the item
            if( bFirstPress)
            {
                // If the previous X value was also greater than the deadZone, return
                // FALSE for bFirstPress
                if( m_PrevControllerState.bControlPressed[port][control] )
                {
                    *bFirstPress = FALSE;
                }
                else // This must be the first press
                {
                    *bFirstPress = TRUE;
                }
            }

            // Return the value of the control
            // This is always TRUE
            return m_ControllerState.bControlPressed[port][control];
        }
    }
    else    // Check to see if the user wants to find out for ANY port
    {
        for( unsigned int locPort = 0; locPort < XGetPortCount(); ++locPort )
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
                    // FALSE for bFirstPress
                    if( m_PrevControllerState.bControlPressed[locPort][control] )
                    {
                        *bFirstPress = FALSE;
                    }
                    else // This must be the first press
                    {
                        *bFirstPress = TRUE;
                    }
                }

                // Return the value of the control
                // This is always TRUE
                return m_ControllerState.bControlPressed[locPort][control];
            }
        }
    }

    // If the button wasn't pressed, let's return FALSE
    return FALSE;
}


// Will return TRUE if any button or thumbstick has changed since last poll
BOOL USBManager::MovementDetected( int port, int deadZone /*=0*/ )
{
    if( port != PORT_ANY )
    {
        if( devices[port][SLOT_CONTROLLER] == NULL )
        {
            return FALSE;
        }

        ZeroMemory( &m_state, sizeof( m_state ) );

        // Get the state of the current controller
        XInputGetState( ( (DeviceDuke*)devices[port][0])->m_hDuke, &m_state);

        // Check the buttons
        for( unsigned button = 0; button < BUTTON_MAX; ++button)
        {
            if( IsButtonPressed( port, (enum BUTTONS)button ) )
            {
                return TRUE;
            }
        }

        // Check the controls
        for( unsigned control = 0; control < CONTROL_MAX; ++control)
        {
            if( IsControlPressed( port, (enum CONTROLS)control ) )
            {
                return TRUE;
            }
        }

        // Check the Joysticks
        for( unsigned joystick = 0; joystick < JOYSTICK_MAX; ++joystick )
        {
            if( 0 != GetJoystickX( port, (enum JOYSTICK)joystick, deadZone ) )
            {
                return TRUE;
            }

            if( 0 != GetJoystickY( port, (enum JOYSTICK)joystick, deadZone ) )
            {
                return TRUE;
            }
        }

        // Check the remote
        for( unsigned remote = 0; remote < REMOTE_BUTTON_MAX; ++remote )
        {
            if( IsRemoteButtonPressed( port, (enum REMOTE_BUTTONS)remote ) )
            {
                return TRUE;
            }
        }
    }
    else    // Check to see if the user wants to find out for ANY port
    {
        ZeroMemory( &m_state, sizeof( m_state ) );

        for( unsigned int x = 0; x < XGetPortCount(); ++x )
        {
            if( devices[x][0] == NULL )
            {
                continue;
            }

            if( devices[x][0]->m_type == XDEVICE_TYPE_GAMEPAD )
            {
                // Get the state of the current controller
                XInputGetState( ( (DeviceDuke*)devices[x][0] )->m_hDuke, &m_state );

                // Check the buttons
                for( unsigned button = 0; button < BUTTON_MAX; ++button )
                {
                    if( IsButtonPressed( x, (enum BUTTONS)button ) )
                    {
                        return TRUE;
                    }
                }

                // Check the controls
                for( unsigned control = 0; control < CONTROL_MAX; ++control )
                {
                    if( IsControlPressed( x, (enum CONTROLS)control ) )
                    {
                        return TRUE;
                    }
                }

                // Check the Joysticks
                for( unsigned joystick = 0; joystick < JOYSTICK_MAX; ++joystick )
                {
                    if( 0 != GetJoystickX( x, (enum JOYSTICK)joystick, deadZone ) )
                    {
                        return TRUE;
                    }

                    if( 0 != GetJoystickY( x, (enum JOYSTICK)joystick, deadZone ) )
                    {
                        return TRUE;
                    }
                }
            }
            else if( devices[x][0]->m_type == XDEVICE_TYPE_IR_REMOTE )
            {
                // Get the state of the current remote
                XInputGetState( ( (DeviceRemote*)devices[x][0] )->m_hRemote, &m_state );

                // Check the remote
                for( unsigned remote = 0; remote < REMOTE_BUTTON_MAX; ++remote )
                {
                    if( IsRemoteButtonPressed( x, (enum REMOTE_BUTTONS)remote ) )
                    {
                        return TRUE;
                    }
                }
            }
        }
    }

    return FALSE;
}


// Will return the X value of the joystick passed in
int USBManager::GetJoystickX( int port, enum JOYSTICK joystick, int deadZone /*=0*/, BOOL* bFirstPress /*=NULL*/ )
{
    // Make sure we have a duke at the current location, otherwise return 0
    if( port != PORT_ANY )
    {
        if( ( devices[port][SLOT_CONTROLLER] == NULL ) || ( devices[port][SLOT_CONTROLLER]->m_type != XDEVICE_TYPE_GAMEPAD ) )
        {
            return 0;
        }

        // If the current stick is moved, process the request and return the value!
        if( abs( m_ControllerState.sThumbX[port][joystick] ) > deadZone )
        {
            // Determine if the user wishes to know if this is the first time the joystick
            // pressed, or if it's a repeat press
            if( bFirstPress)
            {
                // If the previous X value was also greater than the deadZone, return
                // FALSE for bFirstPress
                if( abs( m_PrevControllerState.sThumbX[port][joystick] ) > deadZone )
                {
                    *bFirstPress = FALSE;
                }
                else // This must be the first press
                {
                    *bFirstPress = TRUE;
                }
            }

            // Return the value we got from the joystick
            return m_ControllerState.sThumbX[port][joystick];
        }
    }
    else    // Check to see if the user wants to find out for ANY port
    {
        for( unsigned int locPort = 0; locPort < XGetPortCount(); ++locPort )
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
                    // FALSE for bFirstPress
                    if( abs( m_PrevControllerState.sThumbX[locPort][joystick] ) > deadZone )
                    {
                        *bFirstPress = FALSE;
                    }
                    else // This must be the first press
                    {
                        *bFirstPress = TRUE;
                    }
                }

                // Return the value we got from the joystick
                return m_ControllerState.sThumbX[locPort][joystick];
            }
        }
    }

    // Return 0 if the joystick was not pushed past the dead zone
    return 0;
}


// Will return the Y value of the joystick passed in
int USBManager::GetJoystickY( int port, enum JOYSTICK joystick, int deadZone /*=0*/, BOOL* bFirstPress /*=NULL*/ )
{
    // Make sure we have a duke at the current location, otherwise return 0
    if( port != PORT_ANY )
    {
        if( ( devices[port][SLOT_CONTROLLER] == NULL ) || ( devices[port][SLOT_CONTROLLER]->m_type != XDEVICE_TYPE_GAMEPAD ) )
        {
            return 0;
        }

        // If the current stick is moved, process the request and return the value!
        if( abs( m_ControllerState.sThumbY[port][joystick] ) > deadZone )
        {
            // Determine if the user wishes to know if this is the first time the joystick
            // pressed, or if it's a repeat press
            if( bFirstPress)
            {
                // If the previous Y value was also greater than the deadZone, return
                // FALSE for bFirstPress
                if( abs( m_PrevControllerState.sThumbY[port][joystick] ) > deadZone )
                {
                    *bFirstPress = FALSE;
                }
                else // This must be the first press
                {
                    *bFirstPress = TRUE;
                }
            }

            // Return the value we got from the joystick
            return m_ControllerState.sThumbY[port][joystick];
        }
    }
    else    // Check to see if the user wants to find out for ANY port
    {
        for( unsigned int locPort = 0; locPort < XGetPortCount(); ++locPort )
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
                    // FALSE for bFirstPress
                    if( abs( m_PrevControllerState.sThumbY[locPort][joystick] ) > deadZone )
                    {
                        *bFirstPress = FALSE;
                    }
                    else // This must be the first press
                    {
                        *bFirstPress = TRUE;
                    }
                }

                // Return the value we got from the joystick
                return m_ControllerState.sThumbY[locPort][joystick];
            }
        }
    }

    // Return 0 if the joystick was not pushed past the dead zone
    return 0;
}


// Will return TRUE if a remote button is depressed, or FALSE if it is not
BOOL USBManager::IsRemoteButtonPressed( int port, enum REMOTE_BUTTONS button, BOOL* bFirstPress /*=NULL*/ )
{
    // Make sure we have a remote at the current location, otherwise return FALSE
    if( port != PORT_ANY )
    {
        // Check to make sure a remote control device is plugged in to this port
        if( ( devices[port][SLOT_CONTROLLER] == NULL ) || ( devices[port][SLOT_CONTROLLER]->m_type != XDEVICE_TYPE_IR_REMOTE ) )
        {
            return FALSE;
        }

        if( m_ControllerState.m_bRemoteButtonPress[port][button] )
        {
            // If the user gave us the address of a BOOLean variable, we should return to them
            // whether or not this is the first time the button was pressed, or if it's a repeat
            // press
            if( bFirstPress )
            {
                // If in the previous state the button was pressed, return 'FALSE' for bFirstPress
                if( m_PrevControllerState.m_bRemoteButtonPress[port][button] )
                {
                    *bFirstPress = FALSE;
                }
                else // Otherwise, if the Prev value was FALSE, we need to notify the user it's the first press
                {
                    *bFirstPress = TRUE;
                }
            }
        }

        // Return the value of the button
        // This can be 'TRUE' or 'FALSE'
        return m_ControllerState.m_bRemoteButtonPress[port][button];
    }
    else    // Check to see if the user wants to find out for ANY port
    {
        for( unsigned int locPort = 0; locPort < XGetPortCount(); ++locPort )
        {
            // If there is no Duke in this port, skip to the next port
            if( ( devices[locPort][SLOT_CONTROLLER] == NULL ) || ( devices[locPort][SLOT_CONTROLLER]->m_type != XDEVICE_TYPE_IR_REMOTE ) )
            {
                continue;
            }

            // If the button is depressed, determine if the user wishes to know
            // if it's a repeat press
            if( m_ControllerState.m_bRemoteButtonPress[locPort][button] )
            {
                // If the user gave us the address of a BOOLean variable, we should return to them
                // whether or not this is the first time the button was pressed, or if it's a repeat
                // press
                if( bFirstPress )
                {
                    // If in the previous state the button was pressed, return 'FALSE' for bFirstPress
                    if( m_PrevControllerState.m_bRemoteButtonPress[locPort][button] )
                    {
                        *bFirstPress = FALSE;
                    }
                    else // Otherwise, if the Prev value was FALSE, we need to notify the user it's the first press
                    {
                        *bFirstPress = TRUE;
                    }
                }

                // Since a button was pressed, return early out of our loop
                return TRUE;
            }
        }
    }

    // If no button was pressed, let's return FALSE;
    return FALSE;
}


// Will return TRUE if a button is depressed, or FALSE if it is not
BOOL USBManager::IsButtonPressed( int port, enum BUTTONS button, int nThreshold /*=0*/, BOOL* bFirstPress /*=NULL*/ )
{
    // Make sure we have a duke at the current location, otherwise return FALSE
    if( port != PORT_ANY )
    {
        if( ( devices[port][SLOT_CONTROLLER] == NULL ) || ( devices[port][SLOT_CONTROLLER]->m_type != XDEVICE_TYPE_GAMEPAD ) )
        {
            return FALSE;
        }

        if( m_ControllerState.nButtonPress[port][button] > nThreshold )
        {
            // If the user gave us the address of a BOOLean variable, we should return to them
            // whether or not this is the first time the button was pressed, or if it's a repeat
            // press
            if( bFirstPress )
            {
                // If in the previous state the button was pressed, return 'FALSE' for bFirstPress
                if( m_PrevControllerState.nButtonPress[port][button] > nThreshold )
                {
                    *bFirstPress = FALSE;
                }
                else // Otherwise, if the Prev value was FALSE, we need to notify the user it's the first press
                {
                    *bFirstPress = TRUE;
                }
            }
        }

        // Return the value of the button
        // This can be 'TRUE' or 'FALSE'
        return (m_ControllerState.nButtonPress[port][button] > nThreshold);
    }
    else    // Check to see if the user wants to find out for ANY port
    {
        for( unsigned int locPort = 0; locPort < XGetPortCount(); ++locPort )
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
                // If the user gave us the address of a BOOLean variable, we should return to them
                // whether or not this is the first time the button was pressed, or if it's a repeat
                // press
                if( bFirstPress )
                {
                    // If in the previous state the button was pressed, return 'FALSE' for bFirstPress
                    if( m_PrevControllerState.nButtonPress[locPort][button] > nThreshold )
                    {
                        *bFirstPress = FALSE;
                    }
                    else // Otherwise, if the Prev value was FALSE, we need to notify the user it's the first press
                    {
                        *bFirstPress = TRUE;
                    }
                }

                // Since a button was pressed, return early out of our loop
                return TRUE;
            }
        }
    }

    // If no button was pressed, let's return FALSE;
    return FALSE;
}


// Will accecpt a port and rumble the Controller for a certain amount of time
void USBManager::Rumble( int port, WORD wForce, int nMilliseconds )
{
    if( PORT_ANY == port )
    //
    // Send the command to call controllers plugged in
    //
    {
        for( unsigned int locPort = 0; locPort < XGetPortCount(); ++locPort )
        {
            if( ( devices[locPort][SLOT_CONTROLLER] != NULL ) && ( devices[locPort][SLOT_CONTROLLER]->m_type == XDEVICE_TYPE_GAMEPAD ) )
            {
                // Check to make sure our thread isn't already running
                DWORD dwExitCode;
                GetExitCodeThread( m_hThreads[locPort], &dwExitCode );
                if( STILL_ACTIVE != dwExitCode )
                {
                    // Insert the information in to our Structure
                    m_RumbleInfo[locPort].nPort = locPort;
                    m_RumbleInfo[locPort].wForce = wForce;
                    m_RumbleInfo[locPort].nMilliseconds = nMilliseconds;
                    m_RumbleInfo[locPort].pUSBManager = this;

                    // Create the thread
                    m_hThreads[locPort] = CreateThread( NULL,
                                                        0,
                                                        RumbleThread,
                                                        (LPVOID)&m_RumbleInfo[locPort],
                                                        0, // CREATE_SUSPENDED,
                                                        NULL );
                }
            }
        }
    }
    else
    //
    // Send the command to the specified controller if it exists
    //
    {
        // Make sure the controller is plugged in
        if( ( devices[port][SLOT_CONTROLLER] == NULL ) || ( devices[port][SLOT_CONTROLLER]->m_type != XDEVICE_TYPE_GAMEPAD ) )
        {
            return;
        }

        // Check to make sure our thread isn't already running
        DWORD dwExitCode;
        GetExitCodeThread( m_hThreads[port], &dwExitCode );
        if( STILL_ACTIVE != dwExitCode )
        {
            // Insert the information in to our Structure
            m_RumbleInfo[port].nPort = port;
            m_RumbleInfo[port].wForce = wForce;
            m_RumbleInfo[port].nMilliseconds = nMilliseconds;
            m_RumbleInfo[port].pUSBManager = this;

            // Create the thread
            m_hThreads[port] = CreateThread( NULL,
                                             0,
                                             RumbleThread,
                                             (LPVOID)&m_RumbleInfo[port],
                                             0, // CREATE_SUSPENDED,
                                             NULL );
        }
    }
}


void USBManager::ProcessInput( void )
{
    // Store the previous state
    m_PrevControllerState = m_ControllerState;

    // Check to see if any devices were added or removed
    CheckForHotplugs();

    for( unsigned int port = 0; port < XGetPortCount(); ++port )
    {
        // Make sure a duke is plugged in
        if( devices[port][SLOT_CONTROLLER] == NULL )
        {
            continue;
        }

        if( devices[port][SLOT_CONTROLLER]->m_type == XDEVICE_TYPE_GAMEPAD )
        {
            ZeroMemory( &m_state, sizeof( m_state ) );

            XInputGetState( ( (DeviceDuke*)devices[port][SLOT_CONTROLLER] )->m_hDuke, &m_state );

            // Packet Number
            m_ControllerState.dwControllerState[port] = m_state.dwPacketNumber;

            // Check the buttons
            for( unsigned button = 0; button < BUTTON_MAX; ++button)
            {
                m_ControllerState.nButtonPress[port][button] = m_state.Gamepad.bAnalogButtons[button];
            }

            // Check the controls
            for( unsigned control = 0; control < CONTROL_MAX; ++control)
            {
                m_ControllerState.bControlPressed[port][control] = !!( m_state.Gamepad.wButtons & ( 1 << control ) );
            }

            // Check the Joysticks
            m_ControllerState.sThumbX[port][JOYSTICK_LEFT] = m_state.Gamepad.sThumbLX;
            m_ControllerState.sThumbY[port][JOYSTICK_LEFT] = m_state.Gamepad.sThumbLY;
            m_ControllerState.sThumbX[port][JOYSTICK_RIGHT] = m_state.Gamepad.sThumbRX;
            m_ControllerState.sThumbY[port][JOYSTICK_RIGHT] = m_state.Gamepad.sThumbRY;

            // Check the remote control
            for( unsigned remote = 0; remote < REMOTE_BUTTON_MAX; ++remote )
            {
                m_ControllerState.m_bRemoteButtonPress[port][remote] = FALSE;
            }
        }
        else if( devices[port][SLOT_CONTROLLER]->m_type == XDEVICE_TYPE_IR_REMOTE )
        {
            XINPUT_STATE_INTERNAL state;
            ZeroMemory( &state, sizeof( state ) );

            XInputGetState( ( (DeviceRemote*)devices[port][SLOT_CONTROLLER] )->m_hRemote, (PXINPUT_STATE)&state );

            // Packet Number
            m_ControllerState.dwControllerState[port] = state.dwPacketNumber;

            // Check the buttons
            for( unsigned button = 0; button < BUTTON_MAX; ++button)
            {
                m_ControllerState.nButtonPress[port][button] = FALSE;
            }

            // Check the controls
            for( unsigned control = 0; control < CONTROL_MAX; ++control)
            {
                m_ControllerState.bControlPressed[port][control] = FALSE;
            }

            // Check the Joysticks
            for( unsigned joystick = 0; joystick < JOYSTICK_MAX; ++joystick )
            {
                m_ControllerState.sThumbX[port][joystick] = 0;
                m_ControllerState.sThumbY[port][joystick] = 0;
            }

            // Check the remote control
            for( unsigned remote = 0; remote < REMOTE_BUTTON_MAX; ++remote )
            {
                // BUGBUG: The remote always reports the button is being pressed, even if it is not
                // If this can be fixed, we can change this code for the better
                if( ( state.IrRemote.wKeyCode == g_aRemoteButtons[remote] ) && ( REMOTE_BUTTON_REPEAT_THRESHOLD < state.IrRemote.wTimeDelta ) )
                {
                    m_ControllerState.m_bRemoteButtonPress[port][remote] = TRUE;
                }
                else
                {
                    m_ControllerState.m_bRemoteButtonPress[port][remote] = FALSE;
                }
            }
        }
    }
}


DWORD WINAPI RumbleThread( LPVOID lpParameter )
{
    PRUMBLE_INFO pRumbleInfo = (PRUMBLE_INFO)lpParameter;

    // Send the Rumble Command
    XINPUT_FEEDBACK Feedback;
    ZeroMemory( &Feedback, sizeof( Feedback ) );

    Feedback.Rumble.wLeftMotorSpeed = pRumbleInfo->wForce;
    Feedback.Rumble.wRightMotorSpeed = pRumbleInfo->wForce;

    XInputSetState( ((DeviceDuke*)(pRumbleInfo->pUSBManager->devices[pRumbleInfo->nPort][SLOT_CONTROLLER]))->m_hDuke, &Feedback );

    Sleep( pRumbleInfo->nMilliseconds );

    int nSleepTime = 0;
    while( ERROR_IO_PENDING == Feedback.Header.dwStatus )
    {
        nSleepTime += 100;
        Sleep( 100 );
    }

    Feedback.Rumble.wLeftMotorSpeed = 0;
    Feedback.Rumble.wRightMotorSpeed = 0;

    // Make sure the controller is still inserted, and stop the rumble
    if( pRumbleInfo->pUSBManager->devices[pRumbleInfo->nPort][SLOT_CONTROLLER] )
    {
        XInputSetState( ((DeviceDuke*)(pRumbleInfo->pUSBManager->devices[pRumbleInfo->nPort][SLOT_CONTROLLER]))->m_hDuke, &Feedback );

        while( ERROR_IO_PENDING == Feedback.Header.dwStatus )
        {
            Sleep( 100 );
        };
    }

    return 0;
}
