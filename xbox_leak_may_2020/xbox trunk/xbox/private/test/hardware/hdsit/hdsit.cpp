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
			xDebugStringA("HD SIT: Removed Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
			ghGamePads[i] = NULL;
		}

		// Handle inserted devices
		if(dwInsertions & (1<<i)) {
			ghGamePads[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL);
			xDebugStringA("HD SIT: Added Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
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
			xDebugStringA("HD SIT: Application Paused\n");
			gbPause = TRUE;
		}
	}

	// Start button unpauses app
	if((Buttons & XINPUT_GAMEPAD_START) > 0 ) {
		if(gbPause) {
			xDebugStringA("HD SIT: Application NOT Paused\n");
			gbPause = FALSE;
		}
	}

	// Both triggers plus Black button quits app (from Samples)
	if(AnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] & AnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] & AnalogButtons[XINPUT_GAMEPAD_BLACK]) {
		if(!gbQuit) {
			xDebugStringA("HD SIT: Application Quiting...\n");
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
			xDebugStringA("HD SIT: Removed Gamepad: Port: %ld Handle: %#x\n", i , ghGamePads[i]);
			ghGamePads[i] = NULL;
		}
	}
}


void
__cdecl main() {
	HANDLE hFile, hPattern;

	OBJECT_STRING objectName;
	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK ioStatusBlock;
	NTSTATUS status;
	DWORD BytesReturned, BytesRead;
	BOOL bReturn, bRO, bWO;
	DISK_GEOMETRY DiskGeometry;

	ULONG Cylinder, Head, Sector, SectorsToRead, BufferSize, i;
	LARGE_INTEGER LBA, ByteOffset;
	CHAR PatternFile[MAX_PATH], cString[MAX_PATH];
	UCHAR Pattern[8];
	PUCHAR OrigBuffer, WriteBuffer, ReadBuffer;

	// Initialize Console Output
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);

	xDebugStringA("HD SIT: Press the 'Back' button to Pause the application.\n");
	xDebugStringA("HD SIT: Press the 'Start' button to Resume the application.\n");
	xDebugStringA("HD SIT: Press both triggers and the 'Black' button to quit the application.\n");

	// Initialize Game Pads
	InitGamePads();

	// Open device
	RtlInitObjectString(&objectName, "\\Device\\Harddisk0\\Partition0");
	InitializeObjectAttributes(&objectAttributes, &objectName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = NtOpenFile(&hFile, 
							GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE,
							&objectAttributes,
							&ioStatusBlock,
							0,
							FILE_SYNCHRONOUS_IO_ALERT | FILE_NO_INTERMEDIATE_BUFFERING);

	if(!NT_SUCCESS(status)) {
		xDebugStringA("NtOpenFile FAILED Status = %d\n", status);
		Sleep(INFINITE);
	}

	// get geometry
	memset(&DiskGeometry, 0, sizeof(DiskGeometry));
	bReturn = DeviceIoControl(hFile, IOCTL_DISK_GET_DRIVE_GEOMETRY,
								NULL, 0, 
								&DiskGeometry, sizeof(DiskGeometry), 
								&BytesReturned, NULL);

	if(!bReturn) {
		xDebugStringA("IOCTL_DISK_GET_DRIVE_GEOMETRY FAILED LastError = %d\n", GetLastError());
		Sleep(INFINITE);
	}

	xDebugStringA("Disk Geometry: Cylinders = %u\n", DiskGeometry.Cylinders.QuadPart);
	xDebugStringA("Disk Geometry: TracksPerCylinder = %u\n", DiskGeometry.TracksPerCylinder);
	xDebugStringA("Disk Geometry: SectorsPerTrack = %u\n", DiskGeometry.SectorsPerTrack);
	xDebugStringA("Disk Geometry: BytesPerSector = %u\n", DiskGeometry.BytesPerSector);

	// Get location from ini
	Cylinder =		GetProfileIntA("hdsit", "Cylinder", (INT)DiskGeometry.Cylinders.QuadPart - 1);
	Head =			GetProfileIntA("hdsit", "Head", DiskGeometry.TracksPerCylinder - 1);
	Sector =		GetProfileIntA("hdsit", "Sector", DiskGeometry.SectorsPerTrack - 1);
	SectorsToRead = GetProfileIntA("hdsit", "SectorsToRead", 1);

	// LBA.QuadPart = DiskGeometry.SectorsPerTrack*(DiskGeometry.Cylinders.QuadPart*(Head)  + Cylinder) + Sector - 1;
	LBA.QuadPart = DiskGeometry.SectorsPerTrack*(DiskGeometry.TracksPerCylinder*Cylinder + Head) + Sector - 1;
	BufferSize = SectorsToRead*DiskGeometry.BytesPerSector;
	ByteOffset.QuadPart = LBA.QuadPart*DiskGeometry.BytesPerSector;

	xDebugStringA("Test Location: Cylinder = %u\n", Cylinder);
	xDebugStringA("Test Location: Head = %u\n", Head);
	xDebugStringA("Test Location: Sector = %u\n", Sector);
	xDebugStringA("Test Location: LBA = %u\n", LBA.QuadPart);
	xDebugStringA("Test Location: BufferSize = %u\n", BufferSize);
	xDebugStringA("Test Location: ByteOffset = %u\n", ByteOffset.QuadPart);

	// Allocate buffers
	OrigBuffer = (PUCHAR)GlobalAlloc(GPTR, BufferSize);
	if(OrigBuffer == NULL) {
		xDebugStringA("GlobalAlloc(OrigBuffer) FAILED LastError = %d\n", GetLastError());
		Sleep(INFINITE);
	}

	ReadBuffer = (PUCHAR)GlobalAlloc(GPTR, BufferSize);
	if(ReadBuffer == NULL) {
		xDebugStringA("GlobalAlloc(ReadBuffer) FAILED LastError = %d\n", GetLastError());
		Sleep(INFINITE);
	}
	
	WriteBuffer = (PUCHAR)GlobalAlloc(GPTR, BufferSize);
	if(WriteBuffer == NULL) {
		xDebugStringA("GlobalAlloc(WriteBuffer) FAILED LastError = %d\n", GetLastError());
		Sleep(INFINITE);
	}

	// Get read only flag from testini.ini
	if(GetProfileIntA("hdsit", "ReadOnly", 1) == 1) {
		bRO = TRUE;
	} else {
		bRO = FALSE;
	}

	// Get write only flag from testini.ini
	if(GetProfileIntA("hdsit", "WriteOnly", 0) == 1) {
		bWO = TRUE;
	} else {
		bWO = FALSE;
	}

	if(bRO && bWO) {
		xDebugStringA("TestLocation: Read/Write\n");
	} else if(bRO && !bWO) {
		xDebugStringA("TestLocation: Read Only\n");
	} else if(!bRO && bWO) {
		xDebugStringA("TestLocation: Write Only\n");
	} else {
		xDebugStringA("TestLocation: Nothing\n");
	}

	// Get test pattern from testini or from file
	GetProfileStringA("hdsit", "PatternFile", "NoFile", PatternFile, MAX_PATH);
	IniRemoveStringCommentA(PatternFile);
	if(lstrcmpA(PatternFile, "NoFile") == 0) {
		// No pattern file, get pattern bytes from testini.ini
		for(i = 0; i < 8; i++) {
			wsprintfA(cString, "PatternByte%u", i);
			Pattern[i] = (UCHAR)GetProfileIntA("hdsit", cString, i);
			xDebugStringA("Test Pattern Byte %u: %u\n", i, Pattern[i]);
		}
	
		// Fill WriteBuffer with pattern
		for(i = 0; i < BufferSize; i++) {
			WriteBuffer[i] = Pattern[i%8];
		}

	} else {
		// Get pattern file
		xDebugStringA("Test Location: Test Pattern File = %s\n", PatternFile);
		hPattern = CreateFile(PatternFile, GENERIC_READ, 0, NULL, 
			OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

		if(hPattern == INVALID_HANDLE_VALUE) { 
			xDebugStringA("CreateFile(%s) FAILED LastError = %d\n", PatternFile, GetLastError());
			Sleep(INFINITE);
		}

		// Read pattern
		if(!ReadFile(hPattern, WriteBuffer, BufferSize, &BytesRead, NULL)) {
			xDebugStringA("ReadFile(%s) FAILED LastError = %d\n", PatternFile, GetLastError());
			Sleep(INFINITE);
		}

		CloseHandle(hPattern);
	}

	// xDebugStringA("Pattern to Write (in Hex) at ByteOffset %u\n",  ByteOffset.QuadPart);
	// DumpBuffer(WriteBuffer, BufferSize);

	// Read current current contents at location
	xDebugStringA("Reading Original Contents at ByteOffset %u\n", ByteOffset.QuadPart);
	status = NtReadFile(hFile, NULL, NULL, NULL, &ioStatusBlock, OrigBuffer, BufferSize, &ByteOffset);

	if(!NT_SUCCESS(status)) {
		xDebugStringA("NtReadFile(OrigBuffer %u Bytes at Offset %u) FAILED Status = %d\n", 
			BufferSize, ByteOffset.QuadPart, status);
		Sleep(INFINITE);
	}

	// xDebugStringA("Original Contents (in Hex) at ByteOffset %u\n",  ByteOffset.QuadPart);
	// DumpBuffer(OrigBuffer, BufferSize);

	// Write pattern
	xDebugStringA("Write Pattern at ByteOffset %u\n", ByteOffset.QuadPart);
	status = NtWriteFile(hFile, NULL, NULL, NULL, &ioStatusBlock, WriteBuffer, BufferSize, &ByteOffset);

	if(!NT_SUCCESS(status)) {
		xDebugStringA("NtWriteFile(WriteBuffer %u Bytes at Offset %u) FAILED Status = %d\n", 
			BufferSize, ByteOffset.QuadPart, status);
		Sleep(INFINITE);
	}

	while(!gbQuit) {
		if(!gbPause) {
			if(bRO) {
				// Read location
				status = NtReadFile(hFile, NULL, NULL, NULL, &ioStatusBlock, ReadBuffer, BufferSize, &ByteOffset);

				if(!NT_SUCCESS(status)) {
					xDebugStringA("NtReadFile(ReadBuffer %u Bytes at Offset %u) FAILED Status = %d\n", 
						BufferSize, LBA.QuadPart, status);
				Sleep(INFINITE);
				}
			}

			if(bWO) {
				// Write location
				status = NtWriteFile(hFile, NULL, NULL, NULL, &ioStatusBlock, WriteBuffer, BufferSize, &ByteOffset);

				if(!NT_SUCCESS(status)) {
					xDebugStringA("NtWriteFile(WriteBuffer %u Bytes at Offset %u) FAILED Status = %d\n", 
						BufferSize, ByteOffset.QuadPart, status);
					Sleep(INFINITE);
				}
			}
		}

		// Get button presses
		ReadGamePads();
	}

	// Write original contents back to loaction
	xDebugStringA("Write Original Contents at ByteOffset %u\n", ByteOffset.QuadPart);
	status = NtWriteFile(hFile, NULL, NULL, NULL, &ioStatusBlock, OrigBuffer, BufferSize, &ByteOffset);

	if(!NT_SUCCESS(status)) {
		xDebugStringA("NtWriteFile(OrigBuffer %u Bytes at Offset %u) FAILED Status = %d\n", 
			BufferSize, ByteOffset.QuadPart, status);
		Sleep(INFINITE);
	}

	// Close device
	CloseHandle(hFile);

	// Close gamepads
	CloseGamePads();

	// Free buffers
	GlobalFree(OrigBuffer);
	GlobalFree(ReadBuffer);
	GlobalFree(WriteBuffer);

	// Close console
	xReleaseConsole();

	// Reboot
	XLaunchNewImage(NULL, NULL);
}