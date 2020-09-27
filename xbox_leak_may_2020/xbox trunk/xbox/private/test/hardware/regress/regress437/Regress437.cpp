/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    regress437.cpp

Abstract:

    test app that performs a power cycle

	Tests the feature where the SMC can control the poweroff time of the
	system.

--*/


///////////////////////////////////////////////////////
/*
	So here’s how you program the off time.  
	I added a new write register 0x22.  
	The default value is 25 (25 * 20ms = 500ms.) 
	The time the system will be off when you use SMC reg 0x2  
	to cycle power will be the value in reg 0x22 multiplied 
	by 20ms.  After the system cycles power the value in the register 
	will be reset back to the default of 25 (500ms)  I’m assuming that 
	reg 0x2 and the power switch won’t be used at the same time.  If they 
	are, the timing could be screwy.  Note: Since I decrement then compare 
	in the code if you write the value of zero to the register it will 
	turn into -1 and the time off will be very long.
*/ 



#include <ntos.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddscsi.h>
#include <scsi.h>
#include <align.h>
#include <stdio.h>
#include <stdlib.h>
#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>
#include <fscache.h>
#include <xbinput.h>



#define FILENAME "z:\\resets.txt"



// SMC Functions

extern "C"
    {
    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalReadSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN ReadWordValue,
        OUT ULONG *DataValue
        );

    DECLSPEC_IMPORT
    LONG
    WINAPI
    HalWriteSMBusValue(
        IN UCHAR SlaveAddress,
        IN UCHAR CommandCode,
        IN BOOLEAN WriteWordValue,
        IN ULONG DataValue
        );
    }

DWORD WriteSMC(unsigned char addr, unsigned char value)
    {
    return HalWriteSMBusValue(0x20, addr, FALSE, value);
    }

DWORD ReadSMC(unsigned char addr)
    {
    DWORD value = 0xCCCCCCCC;
    HalReadSMBusValue(0x21, addr, FALSE, &value);
    return value;
    }




void DebugPrint(LPSTR format, ...)
    {
    va_list args;
    va_start(args, format);

    CHAR szBuffer[1024];

    sprintf(szBuffer, format, args);
    OutputDebugStringA(szBuffer);

    va_end(args);
    }


// performs SMC power cycle for the
// specified amount of milliseconds (minimum of 20 ms)
//
VOID SMCPowerCycle( DWORD milliseconds )
{
	if( milliseconds < 60 )
		milliseconds = 60;

	WriteSMC( 0x22, (UCHAR)(milliseconds / 20) );
	WriteSMC( 0x02, 0x40 );
}


void Menu()
{
	printf( "   X button - sleep for 60 ms\n");
	printf( "   Y button - sleep for 100 ms\n");
	printf( "   A button - sleep for 500 ms\n");
	printf( "   B button - sleep for 1s\n" );
	printf( "   Black button - sleep for 5s\n" );
	printf( "   White button - sleep for default time ( 500ms )\n");
}



void __cdecl main()
{
	XBGAMEPAD* pGamepads = NULL;
	

	
	//DebugBreak();


	// create console
	xCreateConsole( NULL );
	xSetBackgroundImage( NULL );
	xSetFont( 15.0, 18.0, NULL );


	// print menu
	Menu();

	// create gamepads
	XInitDevices(0, NULL);
	XBInput_CreateGamepads( &pGamepads );

	// poll for gamepad input
	//    X button - sleep for 20 ms
	//    Y button - sleep for 100 ms
	//    A button - sleep for 500 ms
	//    B button - sleep for 1s
	//    Black button - sleep for 5s
	//    White button - sleep for default time ( 500ms )

	while ( true )
	{
		XBInput_GetInput( pGamepads );
		for( int i = 0; i < 4; i++ )
		{
			if( pGamepads[i].hDevice )
			{
				if(  pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
				{
					//sleep for 25 ms
					SMCPowerCycle( 20 );
				}
				if (pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ] )
				{
					//sleep for 100 ms
					SMCPowerCycle( 100 );
				}
				if (pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
				{
					//sleep for 500 ms
					SMCPowerCycle( 500 );
				}
				if (pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
				{
					//sleep for 1s
					SMCPowerCycle( 1000 );
				}
				if (pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_BLACK ] )
				{
					// sleep for 5s
					SMCPowerCycle( 5000 );
				}

				if (pGamepads[i].bPressedAnalogButtons[ XINPUT_GAMEPAD_WHITE ] )
				{
					// sleep for default time (500ms)
					WriteSMC( 0x02, 0x40 );
				}


				
			}
		}
		
		Sleep( 50 );
	}



	
	

	Sleep( INFINITE );
}
