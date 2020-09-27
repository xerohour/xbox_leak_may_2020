#include <ntos.h>
#include <ntdddisk.h>
#include <xtl.h>
#include <xtestlib.h>
#include <dxconio.h>

// Globals
HANDLE ghGamePads[4];
BOOL gbPause = FALSE;
BOOL gbQuit = FALSE;


void
DumpBuffer(PUCHAR Buffer, ULONG BufferSize) {
	ULONG i, j;

	for(i = 0; i < BufferSize/24; i++) {
		for(j = 0; j < 24; j++) {
			xDebugStringA("%.2x ", Buffer[i*20 + j]);
		}
		xDebugStringA("\n");
	}
}

void
InitGamePads() {
	DWORD i;
	XDEVICE_PREALLOC_TYPE xdpt;

	// Initialize gamepads
	xdpt.DeviceType = XDEVICE_TYPE_GAMEPAD;
	xdpt.dwPreallocCount = 4;

	XInitDevices(1, &xdpt);

	// Get a mask of all currently available devices
    DWORD dwDeviceMask = XGetDevices(XDEVICE_TYPE_GAMEPAD);

    // Open the devices
	for(i = 0; i < 4; i++){
		// Initialize game pad array
		ghGamePads[i] = NULL;

        if( dwDeviceMask & (1<<i) ) {
            // Get a handle to the device
            ghGamePads[i] = XInputOpen( XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL );
			xDebugStringA("Added Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
        }
	}
}


void
ReadGamePads() {
	DWORD dwInsertions, dwRemovals;
	DWORD b, i, Buttons = 0;
	XINPUT_STATE State;
	BOOL AnalogButtons[8];

	// Get gamepad insertions and removals
	XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals);
	for(i = 0; i < 4; i++) {
		// Handle removed devices.
		if(dwRemovals & (1<<i) ) {
			XInputClose( ghGamePads[i]);
			xDebugStringA("HD RW: Removed Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
			ghGamePads[i] = NULL;
		}

		// Handle inserted devices
		if(dwInsertions & (1<<i)) {
			ghGamePads[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL);
			xDebugStringA("HD RW: Added Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
		}
	}

	// Init button variables
	Buttons = 0;
	for(b = 0; b < 8; b++) {
		AnalogButtons[b] = FALSE;
	}

	// Get button presses
	for(i = 0; i < 4; i++) {
		if(ghGamePads[i] != NULL) {
			XInputGetState(ghGamePads[i], &State);

			// Digital buttons
			Buttons |= State.Gamepad.wButtons;

			// Analog buttons (threshold = 25/256)
			for(b = 0; b < 8; b++) {
				AnalogButtons[b] |= (State.Gamepad.bAnalogButtons[b] > 25);
			}
		}
	}

	// Check button presses
	// Back button pauses app
	if((Buttons & XINPUT_GAMEPAD_BACK) > 0) {
		if(!gbPause) {
			xDebugStringA("HD RW: Application Paused\n");
			gbPause = TRUE;
		}
	}

	// Start button unpauses app
	if((Buttons & XINPUT_GAMEPAD_START) > 0 ) {
		if(gbPause) {
			xDebugStringA("HD RW: Application NOT Paused\n");
			gbPause = FALSE;
		}
	}

	// Both triggers plus Black button quits app (from Samples)
	if(AnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] & AnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] & AnalogButtons[XINPUT_GAMEPAD_BLACK]) {
		if(!gbQuit) {
			xDebugStringA("HD RW: Application Quiting...\n");
			gbQuit = TRUE;
		}
	}
}


void
CloseGamePads() {
	DWORD i;

	for(i = 0; i < 4; i++ ) {
		if(ghGamePads[i] != NULL) {
			XInputClose(ghGamePads[i]);
			xDebugStringA("HD RW: Removed Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
			ghGamePads[i] = NULL;
		}
	}
}


void
__cdecl main() {
	HANDLE hFile;

	DWORD BytesRead;
	BOOL  bRO, bWO;

	ULONG FileSize, TimeDuration;
	LONG BufferSize;

	LARGE_INTEGER CurrentTime, EndTime, Frequency;
	LARGE_INTEGER BytesToMove;

	PUCHAR Buffer;

	// Initialize Console Output
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);

	xDebugStringA("HD RW: Press the 'Back' button to Pause the application.\n");
	xDebugStringA("HD RW: Press the 'Start' button to Resume the application.\n");
	xDebugStringA("HD RW: Press both triggers and the 'Black' button to quit the application.\n");

	// Initialize Game Pads
	InitGamePads();

	// Allocate buffer
	BufferSize = 512;
	Buffer = (PUCHAR)GlobalAlloc(GPTR, BufferSize);
	if(Buffer == NULL) {
		xDebugStringA("GlobalAlloc(Buffer) FAILED LastError = %d\n", GetLastError());
		Sleep(INFINITE);
	}

	// Get file size from testini.ini
	FileSize = GetProfileIntA("hdrw", "FileSize", 8);
	xDebugStringA("HD RW: File Size = %u MB\n", FileSize);

	// Get read only flag from testini.ini
	if(GetProfileIntA("hdrw", "ReadOnly", 1) == 1) {
		bRO = TRUE;
	} else {
		bRO = FALSE;
	}
	
	// Get write only flag from testini.ini
	if(GetProfileIntA("hdrw", "WriteOnly", 0) == 1) {
		bWO = TRUE;
	} else {
		bWO = FALSE;
	}

	if(bRO && bWO) {
		xDebugStringA("HD RW: Read/Write\n");
	} else if(bRO && !bWO) {
		xDebugStringA("HD RW: Read Only\n");
	} else if(!bRO && bWO) {
		xDebugStringA("HD RW: Write Only\n");
	} else {
		xDebugStringA("HD RW: Nothing\n");
	}

	// Get time to run from testini.ini
	TimeDuration = GetProfileIntA("hdrw", "TimeDuration", 0);
	if(TimeDuration == 0) {
		xDebugStringA("HD RW: Time Duration = Run Forever\n");
	} else {
		xDebugStringA("HD RW: Time Duration = %u secs\n", TimeDuration);
	}
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&CurrentTime);
	EndTime.QuadPart = CurrentTime.QuadPart + Frequency.QuadPart*TimeDuration;

	// Open file
	hFile = CreateFile("t:\\hdrwtest.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_DELETE_ON_CLOSE, NULL);
	if(hFile == INVALID_HANDLE_VALUE) { 
		xDebugStringA("CreateFile(%s) FAILED LastError = %d\n", "t:\\hdrwtest.txt", GetLastError());
		Sleep(INFINITE);
	}
	
	// Size the file
	BytesToMove.QuadPart = FileSize*1024*1024;
	if(!SetFilePointerEx(hFile, BytesToMove, NULL, FILE_BEGIN)) {
		xDebugStringA("SetFilePointerEx(%u bytes) FAILED LastError = %d\n", BytesToMove.QuadPart, GetLastError());
		Sleep(INFINITE);
	}

	if(!SetEndOfFile(hFile)) {
		xDebugStringA("SetEndOfFile FAILED LastError = %d\n", GetLastError());
		Sleep(INFINITE);
	}

	while(!gbQuit) {
		if(!gbPause) {
			if(bRO) {
				// Read at beginning of file
				BytesToMove.QuadPart = 0;
				if(!SetFilePointerEx(hFile, BytesToMove, NULL, FILE_BEGIN)) {
					xDebugStringA("SetFilePointerEx(%u bytes) FAILED LastError = %d\n", BytesToMove.QuadPart, GetLastError());
					Sleep(INFINITE);
				}
				if(!ReadFile(hFile, Buffer, BufferSize, &BytesRead, NULL)) {
					xDebugStringA("ReadFile() FAILED LastError = %d\n", GetLastError());
					Sleep(INFINITE);
				}

				// Read at end of file
				BytesToMove.QuadPart = -BufferSize;
				if(!SetFilePointerEx(hFile, BytesToMove, NULL, FILE_END)) {
					xDebugStringA("SetFilePointerEx(%u bytes) FAILED LastError = %d\n", BytesToMove.QuadPart, GetLastError());
					Sleep(INFINITE);
				}
				if(!ReadFile(hFile, Buffer, BufferSize, &BytesRead, NULL)) {
					xDebugStringA("ReadFile() FAILED LastError = %d\n", GetLastError());
					Sleep(INFINITE);
				}
			}

			if(bWO) {
				// Read at beginning of file
				BytesToMove.QuadPart = 0;
				if(!SetFilePointerEx(hFile, BytesToMove, NULL, FILE_BEGIN)) {
					xDebugStringA("SetFilePointerEx(%u bytes) FAILED LastError = %d\n", BytesToMove.QuadPart, GetLastError());
					Sleep(INFINITE);
				}
				if(!WriteFile(hFile, Buffer, BufferSize, &BytesRead, NULL)) {
					xDebugStringA("WriteFile() FAILED LastError = %d\n", GetLastError());
					Sleep(INFINITE);
				}

				// Write at end of file
				BytesToMove.QuadPart = -BufferSize;
				if(!SetFilePointerEx(hFile, BytesToMove, NULL, FILE_END)) {
					xDebugStringA("SetFilePointerEx(%u bytes) FAILED LastError = %d\n", BytesToMove.QuadPart, GetLastError());
					Sleep(INFINITE);
				}
				if(!WriteFile(hFile, Buffer, BufferSize, &BytesRead, NULL)) {
					xDebugStringA("WrtieFile() FAILED LastError = %d\n", GetLastError());
					Sleep(INFINITE);
				}
			}
		}

		// Get button presses
		ReadGamePads();

		// Check time duration
		if(TimeDuration != 0) {
			QueryPerformanceCounter(&CurrentTime);
			if(CurrentTime.QuadPart > EndTime.QuadPart) {
				break;
			}
		}
	}

	// Close device
	CloseHandle(hFile);

	// Close gamepads
	CloseGamePads();

	// Free buffers
	GlobalFree(Buffer);

	// Close console
	xReleaseConsole();

	// Reboot
	XLaunchNewImage(NULL, NULL);
}




