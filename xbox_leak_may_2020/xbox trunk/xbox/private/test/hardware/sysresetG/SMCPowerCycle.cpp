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



#define FILENAME "t:\\resets.txt"



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




void DebugPrint(LPWSTR format, ...)
    {
    va_list args;
    va_start(args, format);

    WCHAR szBuffer[1024];

    wsprintf(szBuffer, format, args);
    OutputDebugString(szBuffer);

    va_end(args);
    }



DWORD ReadFromFile( LPSTR lpFilename ,DWORD * pdwArray, DWORD dwSize)
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
		result = ReadFile( hFile, (LPVOID)pdwArray, dwSize, &dwBytesRead, NULL );
	}


	// close the file
	CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE;

	return result;
}





VOID WriteToFile( LPSTR lpFilename, DWORD * pdwArray, DWORD dwSize )
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


	WriteFile( hFile, (LPCVOID)pdwArray, dwSize, &dwBytesWritten, NULL );
	

	// close the file
	CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE;

}


enum {
	eResets,
	eGamePads,
	eMUs,
	eMikes,
	ePhones
};

USHORT GetSlort(DWORD SlotDiffs)
{
	USHORT Slot = 0;
	USHORT Port = 0;
	USHORT Slort;

	if(SlotDiffs & 0xff00)
	{
		Slot = 1;
	}
	SlotDiffs >>= 16 * Slot;
	Slort = Slot << 8; // put slot number in upper byte
	for(int i = 0; i<4; i++)
	{
		if(SlotDiffs & 1)
		{
			Port = (USHORT) i;
			break;
		}
		SlotDiffs >>= 1;
	}
	// build response
	Slort |= Port;
	return Slort;
}

void __cdecl main()
{
	HANDLE hLog;				// handle to the log file
	DWORD dwResets = 0;				// counter
//	XBGAMEPAD* pGamepads = NULL;
	DWORD dwGamepads,dwMUs,dwMicrophones,dwHeadphone;
	DWORD dwArray[5];
	DWORD dwResult;

	//DebugBreak();

	memset(dwArray,0x00,sizeof(dwArray));

	// create console
	xCreateConsole( NULL );
	xSetBackgroundImage( NULL );
	xSetFont( 15.0, 18.0, NULL );


	// create gamepads
	XInitDevices(0, NULL);
//	XBInput_CreateGamepads( &pGamepads );
	Sleep(2000); // allow enumeration time
	dwGamepads = XGetDevices(XDEVICE_TYPE_GAMEPAD);
	dwMUs = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);
	dwMicrophones = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
	dwHeadphone = XGetDevices(XDEVICE_TYPE_VOICE_HEADPHONE);
	



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
	dwResult = ReadFromFile( FILENAME, dwArray, sizeof(dwArray) );
	if(dwResult)
	{
		dwArray[eResets]++; // increment cycle counter
		USHORT Slort;
		DWORD SlotDiffs ;
		int iPort , iSlot;
		SlotDiffs = iPort = iSlot = 0;
		if(dwArray[eGamePads] != dwGamepads)
		{
			DWORD diffs = dwArray[eGamePads] ^ dwGamepads;
			printf("     Diff found during reboot %d\n",dwArray[eResets]);

			int port = 0;
			for(int i = 0; i<4; i++)
			{
				if(diffs & 1)
				{
					port = i;
					break;
				}
				diffs >>= 1;
			}
			printf("\n     Change in Port %d\n",port);
			printf("  Resetting reboot count\n" );
			DeleteFile( FILENAME );
			Sleep( INFINITE );
		} else if(dwArray[eMUs] != dwMUs)
		{
			
			SlotDiffs = dwArray[eMUs] ^ dwMUs;
			Slort = GetSlort(SlotDiffs);
			printf("    Change in Memory Unit\n");
			printf("        Port %d\n",Slort & 0xff);
			printf("        Slot %d\n", (Slort & 0xff00) >> 16);
			DeleteFile(FILENAME);
			Sleep(INFINITE);

		} else if(dwArray[eMikes] != dwMicrophones)
		{
			SlotDiffs = dwArray[eMikes] ^ dwMicrophones;
			Slort = GetSlort(SlotDiffs);
			printf("    Change in Microphone\n");
			printf("        Port %d\n",Slort & 0xff);
			printf("        Slot %d\n", (Slort & 0xff00) >> 16);
			DeleteFile(FILENAME);
			Sleep(INFINITE);

		} else if(dwArray[ePhones] != dwHeadphone)
		{
			SlotDiffs = dwArray[ePhones] ^ dwHeadphone;
			Slort = GetSlort(SlotDiffs);
			printf("    Change in Headphone\n");
			printf("        Port %d\n",Slort & 0xff);
			printf("        Slot %d\n", (Slort & 0xff00) >> 16);
			DeleteFile(FILENAME);
			Sleep(INFINITE);
		}
		
		// inc resets, compare and rewrite
	} else 
	{
		// initialize and write
		dwArray[eResets] = 0;
		dwArray[eGamePads] = dwGamepads;
		dwArray[eMUs] = dwMUs;
		dwArray[eMikes] = dwMicrophones;
		dwArray[ePhones] = dwHeadphone;
	}

	WriteToFile( FILENAME, dwArray,sizeof(dwArray) );
	printf(" SysReset # %d\n", dwArray[eResets] );
	xLog( hLog, XLL_INFO, "SYSReset #: %d", dwResets );

	Sleep(1000); // allow enough time for cycle time printout to be seen by user

// now recycle power	

	WriteSMC( 0x02, 0x40 );

	Sleep( INFINITE );
}
