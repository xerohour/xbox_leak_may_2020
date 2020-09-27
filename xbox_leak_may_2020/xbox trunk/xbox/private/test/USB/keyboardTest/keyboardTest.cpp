//
// "winerror.h"
//

#define DEBUG_KEYBOARD

#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>

void DebugPrint(char* format, ...)
    {
    va_list args;
    va_start(args, format);

    char szBuffer[1024];

    vsprintf(szBuffer, format, args);
    OutputDebugStringA(szBuffer);

    va_end(args);
    }



void main( void )
    {
    XINPUT_POLLING_PARAMETERS xpp = { TRUE, TRUE, 0, 32, 32, 0 };
    XINPUT_DEBUG_KEYQUEUE_PARAMETERS xdkp = {
        XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN     |
            XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT  |
            XINPUT_DEBUG_KEYQUEUE_FLAG_ASCII_ONLY,
            50,
            500,
            70 };
        XDEVICE_PREALLOC_TYPE xdpt[] = {
            {XDEVICE_TYPE_GAMEPAD, 4},
            {XDEVICE_TYPE_MEMORY_UNIT, 2},
            {XDEVICE_TYPE_DEBUG_KEYBOARD, 1} };
            XINPUT_DEBUG_KEYSTROKE ks;
            DWORD dwIns, dwRem, dwPort = 0;
            CHAR szOut[2] = "X";
            HANDLE hKeyboard;
            
            
    // Initialize the peripherals.
    XInitDevices( sizeof(xdpt) / sizeof(XDEVICE_PREALLOC_TYPE), xdpt );
    
    // Create the keyboard queue.
    if( ERROR_SUCCESS == XInputDebugInitKeyboardQueue( &xdkp ) )
        {
        // Wait for the device to show up.
        DebugPrint("Waiting for Keyboard...\n");
        dwIns = XGetDevices( XDEVICE_TYPE_DEBUG_KEYBOARD );
        while( !dwIns )
            {
            XGetDeviceChanges( XDEVICE_TYPE_DEBUG_KEYBOARD, &dwIns, &dwRem );
            }
        
        // Convert the bitmask into a port number.
        while( !(dwIns & (1 << dwPort)) )
            {
            dwPort++;
            }
        
        // Open the keyboard device.
        DebugPrint("Opening Keyboard...\n");
        if( hKeyboard = XInputOpen(
            XDEVICE_TYPE_DEBUG_KEYBOARD,
            dwPort,
            XDEVICE_NO_SLOT,
            &xpp ) )
            {
            // Loop waiting for keyboard input.
            DebugPrint("You can press keys now");
            while( TRUE )
                {       
                if( ERROR_SUCCESS == XInputDebugGetKeystroke( &ks ) )
                    {
                    *szOut = ks.Ascii;
                    OutputDebugString( szOut );
                    }
                }       
            }   
        }    
    
    // Don't return from main.
    XLaunchNewImage(NULL, NULL);
    }


