/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    log.h

Abstract:

    function declarations for log.cpp

Author:

    Jason Gould (a-jasgou)   May 2000

Revision History:

--*/
#ifndef __log_h_
#define __log_h_

// #define _LogToDebug_ //define this to send all logged stuff to the debugger also
// #define _UseCreateFile_	//uncomment this to use CreateFile instead of fopen. *NOT RECOMMENDED, since fopen sometimes has bugs!*

#ifndef _Use_fopen_
	#define _UseCreateFile_
#endif

//#include "cpx.h"

UINT logInit(CHAR * file);
VOID logDeinit();


//logLog: used to log an event. It does formatted printing. 
//It automatically displays date & time, unless you place 
//']' at the beginning of str. 
//(']' won't be displayed if you do use it though.)
UINT logLog(const CHAR * str, ...); 
UINT logLogAndDump(const CHAR * str, ...);
void logDumpToDebug();

#endif