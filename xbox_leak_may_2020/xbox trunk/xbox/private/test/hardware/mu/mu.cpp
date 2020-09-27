/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    mu.cpp

Abstract:

    Continuously read and write all attached MUs

Environment:

    Xbox

Revision History:

--*/
#include <stdio.h>
#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xlogconio.h>


#if !defined(HARNESSLIB)
BOOL g_RunForever = TRUE;
BOOL g_Pause = FALSE;
#else
extern BOOL g_RunForever;
extern BOOL g_Pause;
#endif

// Globals
CHAR gMUs[4][2];			// Array of MU Drive Letters [Port][Slot]
CHAR *gWData, *gRData;		// Read and Write Buffers
SIZE_T gFileSize;			// Size of read and write buffers


VOID
InitWData(HANDLE LogHandle) {
	UCHAR Value;
	int RandomValue;
	SIZE_T i;
	
	RandomValue = GetProfileInt(TEXT("mu"), TEXT("RandomValue"), 1);
	Value = (CHAR)GetProfileInt(TEXT("mu"), TEXT("Value"), 0);

	// Initialize write buffer
	if(RandomValue == 1) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Data Value = RANDOM");
		for(i = 0; i < gFileSize; i++) {
			gWData[i] = (CHAR)rand()%256;
		}
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Data Value = %#x", Value);
		for(i = 0; i < gFileSize; i++) {
			gWData[i] = Value;
		}
	}
}


VOID
DeletAllFiles(HANDLE LogHandle, CHAR Drive) {
    char strFile[100];
    WIN32_FIND_DATA wfd;
    HANDLE hFind;

	wsprintfA(strFile, "%c:\\*", Drive);

    // Start the find and check for failure.
    hFind = FindFirstFile( strFile, &wfd );
	if(INVALID_HANDLE_VALUE == hFind) {
        return;
    } else {
        // Delete all files on mu
        do {
			if(lstrcmpA(wfd.cFileName, "TitleMeta.xbx") != 0) {
				wsprintfA(strFile, "%c:\\%s", Drive, wfd.cFileName);
				if(!DeleteFile(strFile)) {
					xLog(gbConsoleOut, LogHandle, XLL_FAIL,
							"DeleteFile(%s) FAILED LastError = %u", strFile, GetLastError());
				}
			}
        } while(FindNextFile( hFind, &wfd ));

        // Close the find handle.
        FindClose(hFind);
	}
}
    

VOID
MUWrite(HANDLE LogHandle, CHAR Drive) {
	CHAR File[100];
	HANDLE hFile;
	DWORD Bytes;

	xSetFunctionName( LogHandle, "MUWrite" );

	// Open file
	sprintf(File, "%c:\\testfile.dat", Drive);
	hFile = CreateFile(File, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"CreateFile(%s) returned INVALID_HANDLE_VALUE, Error Code = %u", File, GetLastError());
		return;
	}

	// Write to file
	if(!WriteFile(hFile, gWData, gFileSize, &Bytes, NULL)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"WriteFile failed Error Code = %u", GetLastError());
		CloseHandle(hFile);
		return;
	}

	// Close file
	if(!CloseHandle(hFile)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"CloseHande(%s) Error Code = %u", File, GetLastError());
		return;
	}

	xLog(gbConsoleOut, LogHandle, XLL_PASS, "MUWrite %c:\\ PASSED", Drive);
}


VOID
MURead(HANDLE LogHandle, CHAR Drive) {
	CHAR File[100];
	HANDLE hFile;
	DWORD Bytes;

	xSetFunctionName( LogHandle, "MURead" );

	// Create file name
	sprintf(File, "%c:\\testfile.dat", Drive);
	
	// Open file
	hFile = CreateFile(File, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if(hFile == INVALID_HANDLE_VALUE) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"CreateFile(%s) returned INVALID_HANDLE_VALUE, Error Code = %u", File, GetLastError());
		return;
	}

	// Read from file
	if(!ReadFile(hFile, gRData, gFileSize, &Bytes, NULL)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"ReadFile failed Error Code = %u", GetLastError());
		CloseHandle(hFile);
		return;
	}

	// Close file
	if(!CloseHandle(hFile)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"CloseHande(%s) Error Code = %u", File, GetLastError());
		return;
	}
	
	// Check Data
	if(memcmp(gWData, gRData, gFileSize) != 0) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"Read data does not match Write data - gWData[0] = %#x gRData[0] = %#x", gWData[0], gRData[0]);
	}

	xLog(gbConsoleOut, LogHandle, XLL_PASS, "MURead %c:\\ PASSED", Drive);
}


VOID
WINAPI
MUStartTest(HANDLE LogHandle) {
	DWORD i, j;
	DWORD dwDeviceMask, dwInsertions, dwRemovals;
	BOOL bRO, bWO, bRWOnlyOne, bOneDone;

	// Get read only flag from testini.ini
	if(GetProfileInt(TEXT("mu"), TEXT("RO"), 0)) {
		bRO = TRUE;
	} else {
		bRO = FALSE;
	}

	// Get write only flag from testini.ini
	if(GetProfileInt(TEXT("mu"), TEXT("WO"), 0)) {
		bWO = TRUE;
	} else {
		bWO = FALSE;
	}

	if(bRO && bWO) {
		xSetComponent( LogHandle, "EMC", "MU Read/Write" );
	} else if(bRO && !bWO) {
		xSetComponent( LogHandle, "EMC", "MU Read Only" );
	} else if(!bRO && bWO) {
		xSetComponent( LogHandle, "EMC", "MU Write Only" );
	} else {
		xSetComponent( LogHandle, "EMC", "MU Nothing" );
	}

	// Initialize configuration
	xSetOwnerAlias(LogHandle, "a-emebac");
    xSetFunctionName( LogHandle, "Initialization" );

	// Clear gMUs
	for(i=0; i<4; i++) {
		gMUs[i][XDEVICE_TOP_SLOT] = ' ';
		gMUs[i][XDEVICE_BOTTOM_SLOT] = ' ';
	}

	// Get FileSize from testini.ini
	gFileSize = GetProfileInt(TEXT("mu"), TEXT("FileSize"), 8192);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "FileSize = %u", gFileSize);

	// Get RWOnlyOne from testini.ini
	if(GetProfileInt(TEXT("mu"), TEXT("RWOnlyOne"), 0)) {
		bRWOnlyOne = TRUE;
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Using ONE MU only.");
	} else {
		bRWOnlyOne = FALSE;
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Using ALL MUs.");
	}
	
	// Allocate read and write buffers
	gRData = (CHAR *)VirtualAlloc(NULL, gFileSize, MEM_COMMIT, PAGE_READWRITE);
	if(gRData == NULL) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"gRData = VirtuallAlloc(%u) FAILED LastError = %u", gFileSize, GetLastError());
		return;
	}

	gWData = (CHAR *)VirtualAlloc(NULL, gFileSize, MEM_COMMIT, PAGE_READWRITE);
	if(gWData == NULL) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"gWData = VirtuallAlloc(%u) FAILED LastError = %u", gFileSize, GetLastError());
		return;
	}

	// Init write buffer
	InitWData(LogHandle);

	// Get a mask of all currently available devices
    dwDeviceMask = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);

    // Mount the MUs
    for( i=0; i<4; i++ ){
		// Top Slot
        if( dwDeviceMask & (1<<i) ) {
            // Mount MU
 			XMountMU(i, XDEVICE_TOP_SLOT, &gMUs[i][XDEVICE_TOP_SLOT]);
			xLog(gbConsoleOut, LogHandle, XLL_INFO, 
				"Added MU: Port: %ld Slot: %ld Drive: %c", i , XDEVICE_TOP_SLOT, gMUs[i][XDEVICE_TOP_SLOT]);
			DeletAllFiles(LogHandle, gMUs[i][XDEVICE_TOP_SLOT]);
			MUWrite(LogHandle, gMUs[i][XDEVICE_TOP_SLOT]);
        }

		// Bottom Slot
        if(dwDeviceMask & (1<<(i+16))) {
            // Mount MU
 			XMountMU(i, XDEVICE_BOTTOM_SLOT, &gMUs[i][XDEVICE_BOTTOM_SLOT]);
            xLog(gbConsoleOut, LogHandle, XLL_INFO, 
				"Added MU: Port: %ld Slot: %ld Drive: %c", i , XDEVICE_BOTTOM_SLOT, gMUs[i][XDEVICE_BOTTOM_SLOT]);
			DeletAllFiles(LogHandle, gMUs[i][XDEVICE_BOTTOM_SLOT]);
			MUWrite(LogHandle, gMUs[i][XDEVICE_BOTTOM_SLOT]);
        }
    }

	// Loop forever 
	xSetFunctionName( LogHandle, "Exercise MUs" );
	while (g_RunForever) {
		// Get gamepad insertions and removals
		XGetDeviceChanges( XDEVICE_TYPE_MEMORY_UNIT, &dwInsertions, &dwRemovals );
		// Loop through all gamepads
		for(i=0; i<4; i++) {
			// Handle removed devices
			// Top Slot
			if(dwRemovals & (1<<i)) {
				// UnMount MU
				XUnmountMU(i, XDEVICE_TOP_SLOT);
 				xLog(gbConsoleOut, LogHandle, XLL_INFO, 
					"Removed MU: Port: %ld Slot: %ld Drive: %c", i , XDEVICE_TOP_SLOT, gMUs[i][XDEVICE_TOP_SLOT]);
				gMUs[i][XDEVICE_TOP_SLOT] = ' ';
			}
			// Bottom Slot
			if(dwRemovals & (1<<(i+16))) {
				// UnMount MU
 				XUnmountMU(i, XDEVICE_BOTTOM_SLOT);
				xLog(gbConsoleOut, LogHandle, XLL_INFO, 
					"Removed MU: Port: %ld Slot: %ld Drive: %c", i , XDEVICE_BOTTOM_SLOT, gMUs[i][XDEVICE_BOTTOM_SLOT]);
				gMUs[i][XDEVICE_BOTTOM_SLOT] = ' ';
	        }

			// Handle inserted devices
			// Top Slot
	        if(dwInsertions & (1<<i) ) {
		        // Mount MU
 				XMountMU(i, XDEVICE_TOP_SLOT, &gMUs[i][XDEVICE_TOP_SLOT]);
	            xLog(gbConsoleOut, LogHandle, XLL_INFO, 
					"Added MU: Port: %ld Slot: %ld Drive: %c", i , XDEVICE_TOP_SLOT, gMUs[i][XDEVICE_TOP_SLOT]);
				DeletAllFiles(LogHandle, gMUs[i][XDEVICE_TOP_SLOT]);
				MUWrite(LogHandle, gMUs[i][XDEVICE_TOP_SLOT]);
			}
			// Bottom Slot
			if(dwInsertions & (1<<(i+16))) {
				// Mount MU
 				XMountMU(i, XDEVICE_BOTTOM_SLOT, &gMUs[i][XDEVICE_BOTTOM_SLOT]);
	            xLog(gbConsoleOut, LogHandle, XLL_INFO, 
					"Added MU: Port: %ld Slot: %ld Drive: %c", i , XDEVICE_BOTTOM_SLOT, gMUs[i][XDEVICE_BOTTOM_SLOT]);
				DeletAllFiles(LogHandle, gMUs[i][XDEVICE_BOTTOM_SLOT]);
				MUWrite(LogHandle, gMUs[i][XDEVICE_BOTTOM_SLOT]);
			}
		}

		// Read MUs if global Pause flag is not set
		if(!g_Pause) {
			bOneDone = FALSE;
			for(i=0; i<4; i++) {
				for(j=0; j<2; j++) {
					if((gMUs[i][j] != ' ') && !(bRWOnlyOne && bOneDone)) {
						if(bRO) {
							MURead(LogHandle, gMUs[i][j]);
						}
						if(bWO) {
							MUWrite(LogHandle, gMUs[i][j]);
						}
						bOneDone = TRUE;
					}
				}
			}
		} 
	}

	// Unmount MUs
	xSetFunctionName( LogHandle, "Unmount MUs" );
	for(i=0; i<4; i++) {
		// Top Slot
		if(gMUs[i][XDEVICE_TOP_SLOT] != ' ') {
			XUnmountMU(i, XDEVICE_TOP_SLOT);
			xLog(gbConsoleOut, LogHandle, XLL_INFO, 
				"Unmounted MU: Port: %ld Slot: %ld Drive: %c", i , XDEVICE_TOP_SLOT, gMUs[i][XDEVICE_TOP_SLOT]);
			gMUs[i][XDEVICE_TOP_SLOT] = ' ';

		}
		// Bottom Slot
		if(gMUs[i][XDEVICE_BOTTOM_SLOT] != ' ') {
			XUnmountMU(i, XDEVICE_BOTTOM_SLOT);
			xLog(gbConsoleOut, LogHandle, XLL_INFO, 
				"Unmounted MU: Port: %ld Slot: %ld Drive: %c", i , XDEVICE_BOTTOM_SLOT, gMUs[i][XDEVICE_BOTTOM_SLOT]);
			gMUs[i][XDEVICE_BOTTOM_SLOT] = ' ';
		}
	}

	// Free read and write buffers
	if(gRData != NULL) {
		if(!VirtualFree(gRData, 0, MEM_RELEASE)) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"VirtuallFree(gRData) FAILED LastError = %u", GetLastError());
		}
	}

	if(gWData != NULL) {
		if(!VirtualFree(gWData, 0, MEM_RELEASE)) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"VirtuallFree(gWData) FAILED LastError = %u", GetLastError());
		}
	}

}


VOID
WINAPI
MUEndTest(VOID) {
}


#if !defined(HARNESSLIB)
int __cdecl main() {
	HANDLE	LogHandle = NULL;

	// Do the initialization that the harness does
	// dxconio
	g_RunForever = TRUE;
	g_Pause = FALSE;

	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);
	gbConsoleOut = TRUE;
	
	// xlog
	LogHandle = xCreateLog(L"t:\\mu.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("MU: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		return 0;
    }

	// Start Test
	XInitDevices(0, NULL);
	MUStartTest(LogHandle);

	// End Test
	MUEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Do the de-initialize that the harness does
	// dxconio
	xReleaseConsole();

	// Return to launcher
	XLaunchNewImage(NULL, NULL);
}
#endif

//
// Export function pointers of StartTest and EndTest
//
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( mu )
#pragma data_seg()

BEGIN_EXPORT_TABLE( mu )
    EXPORT_TABLE_ENTRY( "StartTest", MUStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MUEndTest )
END_EXPORT_TABLE( mu )
