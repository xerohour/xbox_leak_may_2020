/*
	logs SMC Temps
	logs AMD Temps
*/



//#define WIN32_LEAN_AND_MEAN
#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <i2clib.h>



int __cdecl main(  )
{
	HANDLE		hLog;							// handle to the log
	WCHAR		lpszLogFileName[256];			// pointer to the log file name
	DWORD		dwSMCAirTemp;
	DWORD		dwSMCCPUTemp;
	DWORD		dwAMDAirTemp;
	DWORD		dwAMDCPUTemp;
	DWORD		dwFanSpeed;
	DWORD		dwQueryCount = 0;



	xCreateConsole( NULL );
	xSetBackgroundImage( NULL );
	xSetFont( 15.0, 18.0, NULL );

	GetPrivateProfileString( L"xLogTemp", L"LogFile", L"t:\\xLogTemp.txt", lpszLogFileName, 256, L"hwtestini.ini" );

	//
	// create the log
	//
	hLog = xCreateLog( lpszLogFileName,		// log file name
		               NULL,				// web server info
					   INVALID_SOCKET,		// socket info
					   XLL_LOGALL,			// log level
					   XLO_DEBUG );

	// check to make sure
	// the log was opened
	if( !hLog )
	{
		printf("Could not open log file (%s)\n", WinErrorSymbolicName(GetLastError()) );
		exit(1);
	}


	
	// set some logging options
	// this 
	xSetOwnerAlias( hLog, "sethmil" );		// set owner alias
	xSetComponent( hLog, "Console Thermal Test", "SMC" );
	xSetFunctionName( hLog, "Thermal Log" );
	xStartVariation( hLog, "15 Second Poll" );


	// write header to log file
	//
	xLog( hLog, XLL_INFO, "FAN_SPEED;SMC_AIR_TEMP;SMC_CPU_TEMP;AMD_AIR_TEMP;AMD_CPU_TEMP");
	printf( "LogTemp Running ...\n" );

	
	for( ; ; )  // while (true) 
	{
		Sleep( 2000 );
		

		// Get Fan Speed
		dwFanSpeed = ReadSMC( READFANSPEED );
		if( dwFanSpeed == 0xCCCCCCCC )
		{
			printf("Error getting SMC's fanspeed ... continuing\n");
			continue;
		}
		Sleep(10);
		

		// Get SMC's Air Temp
		dwSMCAirTemp = ReadSMC( AIRTEMP );
		if( dwSMCAirTemp == 0xCCCCCCCC )
		{
			printf("Error getting SMC's airtemp ... continuing\n");
			continue;
		}
		Sleep(10);
		

		// Get SMC's CPU Temp
		dwSMCCPUTemp = ReadSMC( CPUTEMP );
		if( dwSMCCPUTemp == 0xCCCCCCCC )
		{
			printf("Error getting SMC's cputemp ... continuing\n");
			continue;
		}
		Sleep(10);
		

		// Get AMD's Air Temp
		dwAMDAirTemp = ReadAMD( AMDAIRTEMP );
		if( dwAMDAirTemp == 0xCCCCCCCC )
		{
			printf("Error getting AMD's airtemp ... continuing\n");
			continue;
		}
		Sleep(10);
		

		// Get AMD's CPU Temp
		dwAMDCPUTemp = ReadAMD( AMDCPUTEMP );
		if( dwAMDCPUTemp == 0xCCCCCCCC )
		{
			printf("Error getting AMD's cputemp ... continuing\n");
			continue;
		}
		Sleep(10);
		


		// if there were no errors, we can now log the results
	
		xLog( hLog, XLL_INFO, "%d;%d;%d;%d;%d", 
													dwFanSpeed, 
													dwSMCAirTemp, 
													dwSMCCPUTemp, 
													dwAMDAirTemp, 
													dwAMDCPUTemp );

		dwQueryCount++;			// increment counter

		// print the results
		printf( "   Temp Query # %d\n", dwQueryCount );
		printf( "*************************\n" );
		printf( "    Fan Speed:    %d\n", dwFanSpeed );
		printf( "    SMC Air Temp: %d\n", dwSMCAirTemp );
		printf( "    SMC CPU Temp: %d\n", dwSMCCPUTemp );
		printf( "    AMD Air Temp: %d\n", dwAMDAirTemp );
		printf( "    AMD CPU Temp: %d\n", dwAMDCPUTemp );
		printf( "*************************\n" );
		printf( "\n" );

		
	}

}