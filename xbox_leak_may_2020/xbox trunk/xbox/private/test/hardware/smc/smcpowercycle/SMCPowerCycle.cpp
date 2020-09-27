/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    smcpowercycle.cpp

Abstract:

    test app that performs a power cycle

	note:  the MFG boot loader, or a disk with the same image, must be in the drive

	note:  eventually, this should be expanded to check a whole lot of stuff
	after a powercycle, but for now it is concentrating on DVD.

--*/


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




// performs a soft reset on the dvd
//

BOOL
DVDSoftReset(HANDLE hDevice, UCHAR Command) {

 WCHAR buff[256];
 ATA_PASS_THROUGH AtaPassThrough;
 DWORD cbBytesReturned;
 BOOL bReturn;
 memset(&AtaPassThrough, 0, sizeof(ATA_PASS_THROUGH));
 AtaPassThrough.IdeReg.bCommandReg = Command;
 AtaPassThrough.IdeReg.bDriveHeadReg = 0xb0;      // Device 0
 AtaPassThrough.DataBuffer = NULL;
 bReturn = DeviceIoControl(hDevice, IOCTL_IDE_PASS_THROUGH,
        &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
        &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
        &cbBytesReturned, NULL);
 if(!bReturn) {
  //xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SetPowerMode FAILED StatusReg=%#x ErrorReg=%#x LastError = %d",
   //AtaPassThrough.IdeReg.bCommandReg, AtaPassThrough.IdeReg.bFeaturesReg, GetLastError());
  //HDPerf_LogAtaPassThrough(AtaPassThrough);

  wsprintf( buff, L"\nReset error: Last Error: %d\n\n", GetLastError() );
  OutputDebugString ( buff );
  return FALSE;
 } else {
  return TRUE;
 }
}



void DebugPrint(LPWSTR format, ...)
    {
    va_list args;
    va_start(args, format);

    WCHAR szBuffer[1024];

    wsprintf(szBuffer, format, args);
    OutputDebugString(szBuffer);

    va_end(args);
    }



DWORD ReadFromFile( LPSTR lpFilename )
{
	HANDLE hFile;
	DWORD result;
	DWORD dwBytesRead;

	// open the file for reading
	hFile = CreateFile(     lpFilename, 
							GENERIC_READ, 
							0,
							NULL, 
							OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL );


	// set reboots to 0 if file doesn't exist,
	// otherwise, read reboots from the file
	if( INVALID_HANDLE_VALUE == hFile )
	{
		result = 0;
	}
	else
	{
		// get current # of reboots
		ReadFile( hFile, (LPVOID)(&result), 4, &dwBytesRead, NULL );
	}


	// close the file
	CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE;

	return result;
}





VOID WriteToFile( LPSTR lpFilename, DWORD value )
{
	HANDLE hFile;
	DWORD dwBytesWritten;

	// open the file for writing
	hFile = CreateFile(		lpFilename, 
							GENERIC_WRITE, 
							0, 
							NULL,
							CREATE_ALWAYS, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL );


	WriteFile( hFile, (LPCVOID)(&value), 4, &dwBytesWritten, NULL );
	

	// close the file
	CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE;

}





void __cdecl main()
{
	OBJECT_STRING objname;		// object name
	OBJECT_ATTRIBUTES oa;		// obj attributes
	IO_STATUS_BLOCK iosb;		// IO status block
	NTSTATUS status;			// status
	HANDLE hDevice;				// handle to the dvd
	HANDLE hLog;				// handle to the log file
	HANDLE hSearch;				// handle to a file search
	DWORD dwSMCTrayState;		// smc tray state
	WIN32_FIND_DATA fd;			// find find data
	DWORD dwResets = 0;				// counter
	XBGAMEPAD* pGamepads = NULL;
	DWORD dwPressedButtons = 0;
	DWORD dwPressedBlackButton = 0;

	BYTE buff[256];
	DWORD bytenum = 256;
	HANDLE hInFile;

	
	//DebugBreak();


	// create console
	xCreateConsole( NULL );
	xSetBackgroundImage( NULL );
	xSetFont( 15.0, 18.0, NULL );


	// create gamepads
	XInitDevices(0, NULL);
	XBInput_CreateGamepads( &pGamepads );


	// create log
	hLog = xCreateLog( L"t:\\SMCPowerCycle.log", NULL, INVALID_SOCKET, XLL_LOGALL, XLO_DEBUG );
	if( !hLog )
	{
		printf( "  Could not create Log\n");
		DebugBreak();
	}
	xSetOwnerAlias( hLog, "sethmil" );
	xSetComponent( hLog, "Sysreset", "SMC" );
	xSetFunctionName( hLog, "Sysreset" );
	xStartVariation( hLog, "Sysreset stress" );


	// get and increment the # of reboots
	dwResets = ReadFromFile( FILENAME );
	dwResets++;
	WriteToFile( FILENAME, dwResets );
	printf(" SysReset # %d\n", dwResets );
	xLog( hLog, XLL_INFO, "SYSReset #: %d", dwResets );


	// check for Gamepad input.
	// don't reboot if there is gamepad input
	Sleep( 500 );
	

	// if gamepad input, don't reboot, delete the reboot file
	if( dwPressedButtons )
	{
		printf("  Controller input detected\n");

		// reset count if black button was pressed
		if( dwPressedBlackButton )
		{
			printf("  Black button pressed\n");
			printf("  Resetting reboot count\n" );
			DeleteFile( FILENAME );
		}
		printf("\n  Stopped at %d resets\n", dwResets);
		printf("  Sleeping ..." );
		Sleep( INFINITE );
	}


	
	


	// test algorithm
	//	  pre test
	//          override reboot on tray open
	//
	//    . check for media detect
	//	  . Read a file from DVD
	//    . Open a file from DVD
	//    . Read a file from DVD
	//	  . perform a power cycle
	//
	
	// make sure we have media detect
	//
	dwSMCTrayState = (ReadSMC( 0x03 ) >> 4) & 0x0F ;
	if( dwSMCTrayState != 0x6 )
	{
		xLog( hLog, XLL_FAIL, "No Media Detect" );
	}

	

	// mount the DVD device
	//
	RtlInitObjectString(&objname, OTEXT("\\Device\\cdrom0"));
	InitializeObjectAttributes(&oa, &objname, OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = NtOpenFile(&hDevice, GENERIC_READ|SYNCHRONIZE, &oa, &iosb, 0, FILE_SYNCHRONOUS_IO_ALERT);
	

	// flush the file system cache
	FscInvalidateIdleBlocks();
	


	// open the DVDROM device
	//
	DebugPrint(L"opening dvd\n");
	hDevice = CreateFile("cdrom0:",	GENERIC_READ, 0, NULL, OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,NULL);
	if( hDevice == INVALID_HANDLE_VALUE )
	{
		DebugPrint( L"Could not open DVD after reset \n" );
		xLog( hLog, XLL_FAIL, "Could not open DVD after reset");
	}


	// search for 'VTS_01_1.VOB' on the mfg boot loader disk
	//
	DebugPrint(L"searghing for file\n");
	hSearch = FindFirstFile( "cdrom0:\\VIDEO_TS\\VTS_01_1.VOB", &fd );
	if( INVALID_HANDLE_VALUE == hSearch )
	{
		// fail
		xLog( hLog, XLL_FAIL, "Could not find cdrom:\\default.xbe" );
	}
	


	// open the file
	// from the DVD
	//
	DebugPrint(L"opening file\n");
	hInFile = CreateFile( "cdrom0:\\VIDEO_TS\\VTS_01_1.VOB", 
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL );
	if( INVALID_HANDLE_VALUE == hInFile )
	{
		//fail
		xLog( hLog, XLL_FAIL, "Could not open file cdrom:\\default.xbe" );
	}

	// read the first 1 megs from the file
	DebugPrint(L"reading file\n");
	bytenum = 256;
	int j = 0;
	for( j = 0; j < 4096 && ( bytenum == 256 ); j++ ) {
		if( !ReadFile( hInFile, buff, 256, &bytenum, NULL ) )
		{
			//fail
			xLog( hLog, XLL_FAIL, "Error reading default.xbe from DVD" );
			DebugPrint(L" \n\nHalt for failure\n\n");
			break;
		}
	}
	// check that the entire 1 meg was read
	if( j < 4096 || bytenum < 256 )
	{
		xLog( hLog, XLL_FAIL, "ERROR Reading entire 1 meg of data" );
		DebugBreak();
	}
		

	
	// log # of reboots
	//
	//xLog( hLog, XLL_INFO, "Reboot num: %d", dwResets );


	// close all the files
	CloseHandle( hSearch );
	CloseHandle( hInFile );
	CloseHandle( hDevice );
	//xCloseLog( hLog );
	


	// issue a power cycle
	Sleep( 500 );
	WriteSMC( 0x02, 0x40 );

	Sleep( INFINITE );
}
