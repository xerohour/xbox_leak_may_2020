/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    main.cpp

Abstract:

    Entry point and initialization routines for test harness

Environment:

    Xbox

Revision History:

--*/
#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xlogconio.h>


extern "C"
DWORD
WINAPI
HarnessEntryPoint(VOID);


void __cdecl main() {
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch; 
	char SourceFile[MAX_PATH]; 
	char DestFile[MAX_PATH];
	BOOL bRet;

	char OutStr[256];
	
	// copy testini.ini - will not copy if t:\testini.ini exists
	CopyFile("d:\\testini.ini", "t:\\testini.ini", TRUE);


	// check for mmstress media in t:
	hSearch = FindFirstFile("t:\\media\\audio", &FileData);
	if(hSearch == INVALID_HANDLE_VALUE) {
		// media not found, copy from d:
		FindClose(hSearch);
		hSearch = FindFirstFile("d:\\mmstress\\media\\audio\\pcm\\*.wav", &FileData);
		if(hSearch == INVALID_HANDLE_VALUE) {
			xDebugStringA("No mmstress media files, mmstress will not run\n");
		} else {
			xDebugStringA("Copying mmstress media files...");

			// Create Directories
			bRet = CreateDirectory("t:\\media", NULL);
			if(!bRet) {
				wsprintfA(OutStr, "CreateDirectory(t:\\media) FAILED LastError = %d\n", GetLastError());
				xDebugStringA(OutStr);
				Sleep(INFINITE);
			}

			bRet = CreateDirectory("t:\\media\\audio", NULL);
			if(!bRet) {
				wsprintfA(OutStr, "CreateDirectory(t:\\media\\audio) FAILED LastError = %d\n", GetLastError());
				xDebugStringA(OutStr);
				Sleep(INFINITE);
			}

			bRet = CreateDirectory("t:\\media\\audio\\pcm", NULL);
			if(!bRet) {
				wsprintfA(OutStr, "CreateDirectory(t:\\media\\audio\\pcm) FAILED LastError = %d\n", GetLastError());
				xDebugStringA(OutStr);
				Sleep(INFINITE);
			}

			bRet = CreateDirectory("t:\\media\\audio\\wma", NULL);
			if(!bRet) {
				wsprintfA(OutStr, "CreateDirectory(t:\\media\\audio\\wma) FAILED LastError = %d\n", GetLastError());
				xDebugStringA(OutStr);
				Sleep(INFINITE);
			}


			// Copy pcm files
			do {
				// Source file name
				lstrcpyA(SourceFile, "d:\\mmstress\\media\\audio\\pcm\\");
				lstrcatA(SourceFile, FileData.cFileName);

				//lstrcpyA(OutStr, "Source: ");
				//lstrcatA(OutStr, SourceFile);
				//lstrcatA(OutStr, "\n");
				//xDebugStringA(OutStr);

				// Destination File
				lstrcpyA(DestFile, "t:\\media\\audio\\pcm\\");
				lstrcatA(DestFile, FileData.cFileName);

				//lstrcpyA(OutStr, "Destination: ");
				//lstrcatA(OutStr, DestFile);
				//lstrcatA(OutStr, "\n");
				//xDebugStringA(OutStr);

				bRet = CopyFile(SourceFile, DestFile, FALSE);
				if(!bRet) {
					wsprintfA(OutStr, "CopyFileFAILED: LastError = %d\n", GetLastError());
					OutputDebugStringA(OutStr);
					Sleep(INFINITE);
				}

			} while(FindNextFile(hSearch, &FileData));
			FindClose(hSearch);

			// Copy wma files
			hSearch = FindFirstFile("d:\\mmstress\\media\\audio\\wma\\*.wma", &FileData);
			do {
				// Source file name
				lstrcpyA(SourceFile, "d:\\mmstress\\media\\audio\\wma\\");
				lstrcatA(SourceFile, FileData.cFileName);

				//lstrcpyA(OutStr, "Source: ");
				//lstrcatA(OutStr, SourceFile);
				//lstrcatA(OutStr, "\n");
				//xDebugStringA(OutStr);

				// Destination File
				lstrcpyA(DestFile, "t:\\media\\audio\\wma\\");
				lstrcatA(DestFile, FileData.cFileName);
				
				//lstrcpyA(OutStr, "Destination: ");
				//lstrcatA(OutStr, DestFile);
				//lstrcatA(OutStr, "\n");
				//xDebugStringA(OutStr);

				bRet = CopyFile(SourceFile, DestFile, FALSE);
				if(!bRet) {
					wsprintfA(OutStr, "CopyFileFAILED: LastError = %d\n", GetLastError());
					OutputDebugStringA(OutStr);
					Sleep(INFINITE);
				}
			} while(FindNextFile(hSearch, &FileData));
			FindClose(hSearch);
			xDebugStringA("Done\n");
		}
	} else {
		FindClose(hSearch);
		xDebugStringA("mmstress media files present\n");
	}

   	// Determine console output from ini file
	gbConsoleOut = FALSE;

    // Call main routine of the tets harness library in harnesslib.lib
    HarnessEntryPoint();
}


//
// Tell the linker to include the following symbols so that XIU and XTU
// sections inside CRT will get built and startup/cleanup routines in
// xtestlib will get executed
//
#pragma comment( linker, "/include:_chessbrd_ExportTableDirectory" )
#pragma comment( linker, "/include:_clockwrk_ExportTableDirectory" )
#pragma comment( linker, "/include:_cpuid_ExportTableDirectory" )

#pragma comment( linker, "/include:_DolphinClassic_ExportTableDirectory" )
// #pragma comment( linker, "/include:_Dolphin_ExportTableDirectory" )
#pragma comment( linker, "/include:_dvdauth_ExportTableDirectory" )
#pragma comment( linker, "/include:_dvdecc_ExportTableDirectory" )
#pragma comment( linker, "/include:_dvdperf_ExportTableDirectory" )
#pragma comment( linker, "/include:_dvdservopage_ExportTableDirectory" )

#pragma comment( linker, "/include:_hawk_ExportTableDirectory" )
#pragma comment( linker, "/include:_hdperf_ExportTableDirectory" )
// #pragma comment( linker, "/include:_hdperf_ll_ExportTableDirectory" )
// #pragma comment( linker, "/include:_hdperf_sw_ExportTableDirectory" )

#pragma comment( linker, "/include:_idelog_ExportTableDirectory" )

// #pragma comment( linker, "/include:_floatpt_ExportTableDirectory" )

#pragma comment( linker, "/include:_launcher_ExportTableDirectory" )

#pragma comment( linker, "/include:_mempat_ExportTableDirectory" )
#pragma comment( linker, "/include:_memsize_ExportTableDirectory" )
#pragma comment( linker, "/include:_memtest_ExportTableDirectory" )
#pragma comment( linker, "/include:_memval_ExportTableDirectory" )
#pragma comment( linker, "/include:_misorbit_ExportTableDirectory" )
#pragma comment( linker, "/include:_mmStress_ExportTableDirectory" )
#pragma comment( linker, "/include:_msr_ExportTableDirectory" )
#pragma comment( linker, "/include:_mu_ExportTableDirectory" )

#pragma comment( linker, "/include:_netdata_ExportTableDirectory" )

#pragma comment( linker, "/include:_rumble_ExportTableDirectory" )

#pragma comment( linker, "/include:_sdstress_ExportTableDirectory" )
#pragma comment( linker, "/include:_speedtest_ExportTableDirectory" )

#pragma comment( linker, "/include:_xtestlib_startup" )
#pragma comment( linker, "/include:_xtestlib_cleanup" )




