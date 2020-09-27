/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    rumble.cpp

Abstract:

    Cycle rumble motors 1sec, 50% duty, full on, full off

Environment:

    Xbox

Revision History:

--*/
#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xlogconio.h>

const WORD MOTOR_MIN = 0;
const WORD MOTOR_MAX = 65535; 
const DWORD MIN_TIME = 500;
const DWORD MAX_TIME = 500;

// Used by hawk and mu to pause activity
BOOL g_RunForever = TRUE;
BOOL g_Pause = FALSE;


//-----------------------------------------------------------------------------
// Name: struct XBGAMEPAD
// Desc: structure for holding Gamepad data
//-----------------------------------------------------------------------------
struct XBGAMEPAD
{
    // Device properties
    HANDLE     hDevice;
	XINPUT_FEEDBACK Feedback;
};

// Globals
XBGAMEPAD g_Gamepads[4];

VOID 
WINAPI
RumbleMotors(WORD Left, WORD Right) {
	WORD i;

	for(i=0; i<4; i++) {
		if(g_Gamepads[i].hDevice != NULL) {
			while(g_Gamepads[i].Feedback.Header.dwStatus == ERROR_IO_PENDING) {
				// Wait for I/O to complete
				Sleep(10);
			}
			// Set speed
			g_Gamepads[i].Feedback.Rumble.wLeftMotorSpeed = Left;
			g_Gamepads[i].Feedback.Rumble.wRightMotorSpeed = Right;
			XInputSetState(g_Gamepads[i].hDevice, &g_Gamepads[i].Feedback);
		}
	}
}


VOID
WINAPI
RumbleStartTest(HANDLE LogHandle) {
	DWORD dwInsertions, dwRemovals, SleepTime = MIN_TIME;
	XINPUT_STATE State;
	DWORD i, Buttons = 0;
	BOOL MotorMAXState = TRUE;

	xSetOwnerAlias(LogHandle, "a-emebac");
	xSetComponent(LogHandle, "EMC", "Rumble");
    xSetFunctionName(LogHandle, "Initialization" );

	// Initialize configuration
	// Get a mask of all currently available devices
    DWORD dwDeviceMask = XGetDevices( XDEVICE_TYPE_GAMEPAD );

    // Open the devices
	for( i=0; i<4; i++ ){
		// Initialize game pad array
		g_Gamepads[i].hDevice = NULL;
		memset(&(g_Gamepads[i].Feedback), 0, sizeof(XINPUT_FEEDBACK));

        if( dwDeviceMask & (1<<i) ) {
            // Get a handle to the device
            g_Gamepads[i].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL );
			xLog(gbConsoleOut, LogHandle, XLL_INFO, 
				"Added Gamepad: Port: %ld Handle: %#x", i , g_Gamepads[i].hDevice);
        }
	}

	// Loop forever toggling motor speed and wait times
	xSetFunctionName(LogHandle, "Toggle Motors");
	while (g_RunForever) {
		// Get gamepad insertions and removals
		XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals );
		for( i=0; i<4; i++ ) {
			// Handle removed devices.
			if( dwRemovals & (1<<i) ) {
				XInputClose( g_Gamepads[i].hDevice );
				xLog(gbConsoleOut, LogHandle, XLL_INFO, 
					"Removed Gamepad: Port: %ld Handle: %#x", i , g_Gamepads[i].hDevice);
				g_Gamepads[i].hDevice = NULL;
				memset(&(g_Gamepads[i].Feedback), 0, sizeof(XINPUT_FEEDBACK));
			}
			// Handle inserted devices
			if( dwInsertions & (1<<i) ) {
				g_Gamepads[i].hDevice = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL );
				xLog(gbConsoleOut, LogHandle, XLL_INFO, 
					"Added Gamepad: Port: %ld Handle: %#x", i , g_Gamepads[i].hDevice);
			}
		}

		// Set motor speed
		if(!g_Pause) {
			if(MotorMAXState) {
				RumbleMotors(MOTOR_MAX, MOTOR_MAX);
				SleepTime = MAX_TIME;
				MotorMAXState = FALSE;
			} else {
				RumbleMotors(MOTOR_MIN, MOTOR_MIN);
				SleepTime = MIN_TIME;
				MotorMAXState = TRUE;
			}
		}

		// Get button presses
		Buttons = 0;
		for( i=0; i<4; i++ ) {
			if(g_Gamepads[i].hDevice != NULL) {
				XInputGetState(g_Gamepads[i].hDevice, &State);
				Buttons = Buttons | State.Gamepad.wButtons;
			}
		}

		// Back button pauses app
		if((Buttons & XINPUT_GAMEPAD_BACK) > 0) {
			if(!g_Pause) {
				xLog(gbConsoleOut, LogHandle, XLL_INFO, 
					"Application Paused");
				OutputDebugStringA("Application Paused\n");
				// xDebugStringA("Application Paused\n");
				g_Pause = TRUE;
				SleepTime = 0;
				RumbleMotors(MOTOR_MIN, MOTOR_MIN);
			}
		}

		// Start button unpauses app
		if((Buttons & XINPUT_GAMEPAD_START) > 0 ) {
			if(g_Pause) {
				xLog(gbConsoleOut, LogHandle, XLL_INFO, 
					"Application NOT Paused");
				OutputDebugStringA("Application NOT Paused\n");
				// xDebugStringA("Application NOT Paused\n");
				g_Pause = FALSE;
			}
		}

		// Wait specified time
		Sleep(SleepTime);
	}	// while(g_RunForever)
	
	// Close all gamepads
	xSetFunctionName(LogHandle, "EndTest");
	for(i=0; i<4; i++ ) {
		if(g_Gamepads[i].hDevice != NULL) {
			while(g_Gamepads[i].Feedback.Header.dwStatus == ERROR_IO_PENDING) {
				// Wait for I/O to complete
			}
			// XInputClose( g_Gamepads[i].hDevice );
			xLog(gbConsoleOut, LogHandle, XLL_INFO, 
				"Removed Gamepad: Port: %ld Handle: %#x", i , g_Gamepads[i].hDevice);
			g_Gamepads[i].hDevice = NULL;
			memset(&(g_Gamepads[i].Feedback), 0, sizeof(XINPUT_FEEDBACK));
		}
	}
}


VOID
WINAPI
RumbleEndTest(VOID) {
}

#if !defined(HARNESSLIB)
int __cdecl main() {
	HANDLE	LogHandle = NULL;

	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);
	gbConsoleOut = TRUE;
	g_RunForever = TRUE;
	g_Pause = FALSE;

	// xlog
	LogHandle = xCreateLog_W(L"t:\\rumble.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("RUMBLE: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		return 0;
    }

	xDebugStringA("All attached Game Controllers will be exercised.\n");
	xDebugStringA("Press 'Back' to Pause\n");
	xDebugStringA("Press 'Start' to Resume\n");

	// Start Test
	XInitDevices(0, NULL);
	RumbleStartTest(LogHandle);

	// End Test
	RumbleEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Do the de-initialize that the harness does
	// dxconio
	xReleaseConsole();

	XLaunchNewImage(NULL, NULL);
}
#endif

//
// Export function pointers of StartTest and EndTest
//
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( rumble )
#pragma data_seg()

BEGIN_EXPORT_TABLE( rumble )
    EXPORT_TABLE_ENTRY( "StartTest", RumbleStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", RumbleEndTest )
END_EXPORT_TABLE( rumble )
