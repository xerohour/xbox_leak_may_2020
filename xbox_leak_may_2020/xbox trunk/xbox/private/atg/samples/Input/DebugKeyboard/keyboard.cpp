//-----------------------------------------------------------------------------
// File: Keyboard.cpp
//
// Desc: Handles debug keyboard.
//
// Hist: 06.11.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define DEBUG_KEYBOARD
#include <xtl.h>
#include <xkbd.h>
#include "keyboard.h"


static HANDLE g_hKeyboardDevice[4] = { 0 };

static BOOL                     g_bKeyboardInitialised  = FALSE;
static XINPUT_DEBUG_KEYSTROKE   g_keyboardStroke;




//-----------------------------------------------------------------------------
// Name: XBInput_InitDebugKeyboard()
// Desc: Initialise Debug Keyboard for use
//-----------------------------------------------------------------------------
HRESULT XBInput_InitDebugKeyboard()
{
    // Check that we are not already initialised and then initialise if necessary
    if( g_bKeyboardInitialised )
        return S_OK;

    XINPUT_DEBUG_KEYQUEUE_PARAMETERS keyboardSettings;
    keyboardSettings.dwFlags          = XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN|XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT;
    keyboardSettings.dwQueueSize      = 25;
    keyboardSettings.dwRepeatDelay    = 500;
    keyboardSettings.dwRepeatInterval = 50;

    if( ERROR_SUCCESS != XInputDebugInitKeyboardQueue( &keyboardSettings ) )
        return E_FAIL;

    g_bKeyboardInitialised = TRUE;

    // Now find the keyboard device, in this case we shall loop indefinitly, although
    // it would be better to monitor the time taken and to time out if necessary
    // in case the keyboard has been unplugged

    DWORD dwDeviceMask = XGetDevices( XDEVICE_TYPE_DEBUG_KEYBOARD );

    // Open the devices
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        if( dwDeviceMask & (1<<i) ) 
		{
			// Now open the device
			XINPUT_POLLING_PARAMETERS pollValues;
			pollValues.fAutoPoll       = TRUE;
			pollValues.fInterruptOut   = TRUE;
			pollValues.bInputInterval  = 32;  
			pollValues.bOutputInterval = 32;
			pollValues.ReservedMBZ1    = 0;
			pollValues.ReservedMBZ2    = 0;

			g_hKeyboardDevice[i] = XInputOpen( XDEVICE_TYPE_DEBUG_KEYBOARD, i, 
									           XDEVICE_NO_SLOT, &pollValues );
		}
	}
     
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: XBInput_GetKeyboardInput()
// Desc: Processes input from a debug keyboard
//-----------------------------------------------------------------------------
CHAR XBInput_GetKeyboardInput()
{
    // Get status about gamepad insertions and removals. Note that, in order to
    // not miss devices, we will check for removed device BEFORE checking for
    // insertions
    DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_DEBUG_KEYBOARD, &dwInsertions, &dwRemovals );

    // Loop through all ports
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        // Handle removed devices.
        if( dwRemovals & (1<<i) )
        {
            XInputClose( g_hKeyboardDevice[i] );
            g_hKeyboardDevice[i] = NULL;
        }

        // Handle inserted devices
        if( dwInsertions & (1<<i) )
        {
			// Now open the device
			XINPUT_POLLING_PARAMETERS pollValues;
			pollValues.fAutoPoll       = TRUE;
			pollValues.fInterruptOut   = TRUE;
			pollValues.bInputInterval  = 32;  
			pollValues.bOutputInterval = 32;
			pollValues.ReservedMBZ1    = 0;
			pollValues.ReservedMBZ2    = 0;

            // TCR 1-14 Device Types
            g_hKeyboardDevice[i] = XInputOpen( XDEVICE_TYPE_DEBUG_KEYBOARD, i, 
                                               XDEVICE_NO_SLOT, &pollValues );
        }

        // If we have a valid device, poll it's state and track button changes
        if( g_hKeyboardDevice[i] )
        {
		    if( ERROR_SUCCESS == XInputDebugGetKeystroke( &g_keyboardStroke ) )
				return g_keyboardStroke.Ascii;
        }
    }

	return '\0';
}




