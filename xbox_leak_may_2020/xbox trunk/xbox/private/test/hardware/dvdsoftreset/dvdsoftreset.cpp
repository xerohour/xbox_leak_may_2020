/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    dvdsoftreset.cpp

Abstract:

    test app that repeatedly sends an ATA device reset command
	to the dvd.

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

	BYTE buff[256];
	DWORD bytenum = 256;
	HANDLE hInFile;

	
	//DebugBreak();



	// initialize the log
	hLog = xCreateLog( L"t:\\DVDSoftReset.log", NULL, INVALID_SOCKET, XLL_LOGALL, XLO_DEBUG );
	if( INVALID_HANDLE_VALUE == hLog )
	{
		DebugPrint( L"\nCould not open log file\n" );
		DebugBreak();
	}
	xSetOwnerAlias( hLog, "sethmil" );
	xSetComponent( hLog, "DVD", "Soft Reset" );
	xSetFunctionName( hLog, "Soft Reset Stress" );
	xStartVariation( hLog, "DVD Soft Reset" );


	
	


	// test algorithm
	//    1.  Mount the drive
	//    2.  Send soft reset
	//    3.  Check media detect
	//    4.  Create cdrom:\\ file
	//    5.  Search for default.xbe in the DVD root directory
	//    6.  Read default.xbe from the drive
	//    Repeat.
	//
	while( true )
	{
		dwResets++;

		// mount the DVD device
		//
		RtlInitObjectString(&objname, OTEXT("\\Device\\cdrom0"));
		InitializeObjectAttributes(&oa, &objname, OBJ_CASE_INSENSITIVE, NULL, NULL);
		status = NtOpenFile(&hDevice, GENERIC_READ|SYNCHRONIZE, &oa, &iosb, 0, FILE_SYNCHRONOUS_IO_ALERT);
		
		// send soft reset to the DVD
		//
		DVDSoftReset( hDevice, 0x08 );
		DebugPrint(L"Reset the dvd\n");
		Sleep( 1000 );


		// flush the file system cache
		FscInvalidateIdleBlocks();
		


		// make sure we have media detect
		//
		dwSMCTrayState = (ReadSMC( 0x03 ) >> 4) & 0x0F ;
		if( dwSMCTrayState != 0x6 )
		{
			xLog( hLog, XLL_FAIL, "No Media Detect" );
		}



		// open the DVDROM device
		//
		hDevice = CreateFile("cdrom0:",	GENERIC_READ, 0, NULL, OPEN_EXISTING, 
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,NULL);
		if( hDevice == INVALID_HANDLE_VALUE )
		{
			DebugPrint( L"Could not open DVD after reset \n" );
			xLog( hLog, XLL_FAIL, "Could not open DVD after reset");
		}


		// search for 'default.xbe' in the root directory
		//
		hSearch = FindFirstFile( "cdrom0:\\default.xbe", &fd );
		if( INVALID_HANDLE_VALUE == hSearch )
		{
			// fail
			xLog( hLog, XLL_FAIL, "Could not find cdrom:\\default.xbe" );
		}
		


		// open default.xbe and read it 
		// from the DVD
		//
		hInFile = CreateFile( "cdrom0:\\default.xbe", 
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

		// read the file
		bytenum = 256;
		while( bytenum == 256 ){
			if( !ReadFile( hInFile, buff, 256, &bytenum, NULL ) )
			{
				//fail
				xLog( hLog, XLL_FAIL, "Error reading default.xbe from DVD" );
				DebugPrint(L" \n\nHalt for failure\n\n");
				DebugBreak();
				break;
			}
		}

		
		// log # of reboots
		//
		xLog( hLog, XLL_INFO, "Reboot num: %d", dwResets );


		// close all the files
		CloseHandle( hSearch );
		CloseHandle( hInFile );
		CloseHandle( hDevice );

	}


	// never gets here
	// DebugPrint( L"\n\nend\n\n");
	Sleep( INFINITE );



}