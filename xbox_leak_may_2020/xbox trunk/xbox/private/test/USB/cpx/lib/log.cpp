/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    log.cpp

Abstract:

    logging functions for CPX

Author:

    Jason Gould (a-jasgou) May 2000

--*/

#include <stdio.h>
#include <xtl.h>
#include <stdarg.h>
#include <time.h>
#include "log.h"

DWORD gPosition = 0;

extern "C" ULONG DebugPrint(PCHAR Format, ...);	//prints stuff to the debugger computer

//*********Global Log variables******************************************************
#ifdef _UseCreateFile_
	static HANDLE f;			//or the handle, if we use CreateFile
#else
	static FILE * f;			//the file pointer used in logging
#endif

static CHAR gFileName[256] = "t:\\cpxlog.log";

/*************************************************************************************
Function:   logInit
Purpose:	Begin Logging, using specified or default file
Params:     file --- pointer to the string that holds the name of the file to log to
			If file is NULL, the file C:\winnt\xboxtest\cpxlog.log" is used instead.
Return:     zero if it failed, non-zero if it worked
*************************************************************************************/
UINT logInit(
	OPTIONAL IN CHAR * file)
{
	DebugPrint("File = 0x%08x\n", file);
	if(file == NULL){
		sprintf(gFileName, "t:\\cpxlog.log");
	} else {
		sprintf(gFileName, "%s", file);
	}

	DebugPrint("LogInit... opening %s.\n", gFileName);

	
	#ifdef _UseCreateFile_
		BOOL TryAgain = TRUE;
tryagain:
		f = CreateFile(gFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL); 

		if(f==INVALID_HANDLE_VALUE)
		{
			int err = GetLastError();
            DebugPrint("CreateFile couldn't open '%s'! GetLastError: %d\n", gFileName, err);
			if(err == 1392) {// weird / corrupted file that happens a lot...
				if(!DeleteFile(gFileName)) {
					DebugPrint("DeleteFile(\"%s\") failed, GetLastError: %d\n", gFileName, GetLastError());
				}
				if(TryAgain) {
					DebugPrint("Trying again...");
					TryAgain = FALSE;
					goto tryagain;
				}
			}

			return 0;
		} else {
			DebugPrint("Createfile opened '%s' correctly!\n", gFileName);
			if(!~SetFilePointer(f, 0, 0, FILE_END))	{
				DebugPrint("SetFilePointer failed! Error %d\n", GetLastError());
			}
		}
	#else
		//	f = fopen(file, "a+");
		f = fopen(gFileName, "a+");
		if(f == NULL)
		{
			DebugPrint("fopen Couldn't open '%s'!  GetLastError: %d\n", gFileName, GetLastError());
			return 0;
		} else {
			DebugPrint("fopen opened '%s' correctly!\n", gFileName);
		}
	#endif
	CHAR timbuf[64];
	_strtime(timbuf);
	DebugPrint("Control Panel X\n");
	DebugPrint("\n--- Beginning log at %s", timbuf);
	_strdate(timbuf);
	DebugPrint(" on %s ---\n\n", timbuf);
	//DebugPrint("]Test...\n");
	return (UINT)f;
}


/*************************************************************************************
Function:   logDeinit
Purpose:	End logging, close the log file, etc.
Params:     none
Return:     none
*************************************************************************************/
VOID logDeinit()
{
	CHAR timbuf[64];
	_strtime(timbuf);
	DebugPrint("\n --- Closing log at %s", timbuf);
	_strdate(timbuf);
	DebugPrint(" on %s ---\n\n", timbuf);
	#ifdef _UseCreateFile_
		CloseHandle(f);
	#else
		fclose(f);
	#endif
}


/* we shouldn't need this now...
#ifdef UNICODE
#define _vftprintf(stream, format, argptr) vfwprintf(stream, format, argptr)
#else
#define _vftprintf(stream, format, argptr) vfprintf(stream, format, argptr)
#endif
*/


/*************************************************************************************
Function:   logLog
Purpose:	Log something!
Params:     str --- the WIDE_CHAR formatted string to log
			... --- the optional params for the formatted string
Notes:		If the first char of str is ']', logLog will print the date and time to 
			log file
Return:     none
*************************************************************************************/
unsigned long __stdcall w(void* buf)
{
	#ifdef _UseCreateFile_
		WriteFile(f, ((CHAR*)buf)+4, *(int*)buf * 2, (DWORD*)buf, 0);
	#else
		fwprintf(f, ((CHAR*)buf)+4);
	#endif

	delete[] buf;
	return 0;
}

UINT logLog(const CHAR * str, ...)
{
	CHAR* buf = new CHAR[1024];
	DWORD length;

	va_list v;
	va_start(v, str);

	if(!f) {
		logInit(NULL);
	}

	*(int*)buf = vsprintf(buf+4, str, v);

	CreateThread(0,0,w,buf,0,0);

	va_end (v);
	return 0;
}

/*************************************************************************************
Function:   logLogAndDump
Purpose:	Log something, and dump it to the debugger
Params:     str --- the WIDE_CHAR formatted string to log
			... --- the optional params for the formatted string
Notes:		If the first char of str is ']', logLog will print the date and time to 
			log file (and not the ']',
Return:     none
*************************************************************************************/
UINT logLogAndDump(const CHAR * str, ...)
{
	CHAR buf[1024];
	DWORD length;

	va_list v;
	va_start(v, str);

	if(!f) {
		logInit(NULL);
	}

	if(str[0] == ']') {		//secret code... display data & time also
		CHAR timbuf[64];
		_strdate(timbuf);
		logLog("%ls ", timbuf);
		_strtime(timbuf);
		logLog("%ls ", timbuf);
		str++;
	} 

	length = vsprintf(buf, str, v);
		DebugPrint("%s", buf);

	#ifdef _UseCreateFile_
		WriteFile(f, buf, length * 2, &length, 0);
	#else
		fprintf(f, buf);
	#endif

	va_end (v);
	return 0;
}


/*************************************************************************************
Function:   logDumpToDebug
Purpose:	dump the log file to the debugger, clear the log file
Params:     none 
Return:     none
*************************************************************************************/
void logDumpToDebug()
{
	CHAR buf[256];
	logDeinit();

	#ifdef _UseCreateFile_
		DWORD length;
		f = CreateFile(gFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL); 
		DebugPrint("\n\n\n");
		do {
			ReadFile(f, buf, sizeof(buf) - 2, &length, 0);
			buf[length / sizeof(buf[0])] = '\0';
			DebugPrint("%s", buf);
		} while (length);
		DebugPrint("\n\n\n");
		CloseHandle(f);
		f=NULL;
//	logInit(NULL);	
	#else //we're trying fopen instead...
		f = fopen(gFileName, "r");
		DebugPrint("\n\n\n");
		
		while(!feof(f)) {
			fgets(buf, 255, f);
			DebugPrint("%s", buf);
		}
	
		DebugPrint("\n\n\n");
		fclose(f);
		f=NULL;
//	logInit(NULL);	
	#endif

}
