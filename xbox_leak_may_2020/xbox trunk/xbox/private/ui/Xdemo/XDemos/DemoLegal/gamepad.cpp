#include "gamepad.h"

XBGAMEPAD g_Gamepads[4];

HRESULT InitInput(void)
{
	XDEVICE_PREALLOC_TYPE deviceTypes[] =
	{
	   {XDEVICE_TYPE_GAMEPAD, 4},
	   {XDEVICE_TYPE_MEMORY_UNIT, 2}
	};
	XInitDevices(sizeof(deviceTypes) / sizeof(XDEVICE_PREALLOC_TYPE), deviceTypes );

   // Get a mask of all currently available devices
    DWORD dwDeviceMask = XGetDevices( XDEVICE_TYPE_GAMEPAD );

    // Open the devices
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        ZeroMemory( &g_Gamepads[i], sizeof(XBGAMEPAD) );
        if( dwDeviceMask & (1<<i) ) 
        {
            // Get a handle to the device
            g_Gamepads[i].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, 
                                                XDEVICE_NO_SLOT, NULL );

            // Store capabilites of the device
            XInputGetCapabilities( g_Gamepads[i].hDevice, &g_Gamepads[i].caps );
        }
    }

    return S_OK;
}

//
// Poll controllers for input, if we detect any, we should return true;
//
bool PollControllers(void)
{
	// check to make sure a controller is unplugged
    DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals );

    // Loop through all gamepads
    for( DWORD i = 0; i < XGetPortCount(); i++ )
    {
        // Handle removed devices.
        g_Gamepads[i].bRemoved = ( dwRemovals & (1<<i) ) ? TRUE : FALSE;
        if( g_Gamepads[i].bRemoved )
        {
            XInputClose( g_Gamepads[i].hDevice );
            g_Gamepads[i].hDevice = NULL;
            g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed  = 0;
            g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed = 0;
        }

        // Handle inserted devices
        g_Gamepads[i].bInserted = ( dwInsertions & (1<<i) ) ? TRUE : FALSE;
        if( g_Gamepads[i].bInserted ) 
        {
            g_Gamepads[i].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, 
                                               XDEVICE_NO_SLOT, NULL );
            XInputGetCapabilities( g_Gamepads[i].hDevice, &g_Gamepads[i].caps );
        }

        // If we have a valid device, poll it's state and track button changes
        if( g_Gamepads[i].hDevice )
        {
            // Read the input state
            XINPUT_STATE xiState;
            XInputGetState( g_Gamepads[i].hDevice, &xiState );

            // Copy gamepad to local structure
            memcpy( &g_Gamepads[i], &xiState.Gamepad, sizeof(XINPUT_STATE) );

            // Put Xbox device input for the gamepad into our custom format
            FLOAT fX1 = (g_Gamepads[i].sThumbLX+0.5f)/32767.5f;
            g_Gamepads[i].fX1 = ( fX1 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fX1)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            FLOAT fY1 = (g_Gamepads[i].sThumbLY+0.5f)/32767.5f;
            g_Gamepads[i].fY1 = ( fY1 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fY1)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            FLOAT fX2 = (g_Gamepads[i].sThumbRX+0.5f)/32767.5f;
            g_Gamepads[i].fX2 = ( fX2 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fX2)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            FLOAT fY2 = (g_Gamepads[i].sThumbRY+0.5f)/32767.5f;
            g_Gamepads[i].fY2 = ( fY2 >= 0.0f ? 1.0f : -1.0f ) *
                               max( 0.0f, (fabsf(fY2)-XBINPUT_DEADZONE)/(1.0f-XBINPUT_DEADZONE) );

            // Get the boolean buttons that have been pressed since the last
            // call. Each button is represented by one bit.
            g_Gamepads[i].wPressedButtons = ( g_Gamepads[i].wLastButtons ^ g_Gamepads[i].wButtons ) & g_Gamepads[i].wButtons;
            g_Gamepads[i].wLastButtons    = g_Gamepads[i].wButtons;

            // Get the analog buttons that have been pressed or released since
            // the last call.
            for( DWORD b = 0; b < 8; b++ )
            {
                // Turn the 8-bit polled value into a boolean value
                BOOL bPressed = ( g_Gamepads[i].bAnalogButtons[b] > 0 );

                if( bPressed )
                    g_Gamepads[i].bPressedAnalogButtons[b] = !g_Gamepads[i].bLastAnalogButtons[b];
                else
                    g_Gamepads[i].bPressedAnalogButtons[b] = FALSE;
                
                // Store the current state for the next time
                g_Gamepads[i].bLastAnalogButtons[b] = bPressed;
            }
        }
    }

	//
	// For all the joysticks
	//
	for(int n = 0; n < 4; n++)
	{
		if(g_Gamepads[n].wButtons & XINPUT_GAMEPAD_DPAD_UP)			return true;
		if(g_Gamepads[n].wButtons & XINPUT_GAMEPAD_DPAD_DOWN)		return true;
		if(g_Gamepads[n].wButtons & XINPUT_GAMEPAD_DPAD_LEFT)		return true;
		if(g_Gamepads[n].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)		return true;
		if(g_Gamepads[n].wButtons & XINPUT_GAMEPAD_BACK)			return true;
		if(g_Gamepads[n].wButtons & XINPUT_GAMEPAD_START)			return true;
		if(g_Gamepads[n].bAnalogButtons[XINPUT_GAMEPAD_A] > XBINPUT_DEADZONE)	return true;
		if(g_Gamepads[n].bAnalogButtons[XINPUT_GAMEPAD_B] > XBINPUT_DEADZONE)	return true;
		if(g_Gamepads[n].bAnalogButtons[XINPUT_GAMEPAD_X] > XBINPUT_DEADZONE)	return true;
		if(g_Gamepads[n].bAnalogButtons[XINPUT_GAMEPAD_Y] > XBINPUT_DEADZONE)	return true;
		if(g_Gamepads[n].bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > XBINPUT_DEADZONE)	return true;
		if(g_Gamepads[n].bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > XBINPUT_DEADZONE)	return true;

		if(g_Gamepads[n].fX1 > 0.0f) return true;
		if(g_Gamepads[n].fX1 < 0.0f) return true;
		if(g_Gamepads[n].fY1 > 0.0f) return true;
		if(g_Gamepads[n].fY1 < 0.0f) return true;
	}

	return false;
}