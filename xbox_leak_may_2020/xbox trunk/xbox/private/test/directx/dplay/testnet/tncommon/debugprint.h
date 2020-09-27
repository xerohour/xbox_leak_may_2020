#ifndef __DEBUGPRINT__
#define __DEBUGPRINT__




//==================================================================================
// Defines
//==================================================================================
#ifdef LOCAL_TNCOMMON

	// define LOCAL_TNCOMMON when including this code directly into your project
	#ifdef DLLEXPORT
		#undef DLLEXPORT
	#endif // DLLEXPORT defined
	#define DLLEXPORT

#else // ! LOCAL_TNCOMMON

	#ifdef TNCOMMON_EXPORTS

		// define TNCOMMON_EXPORTS only when building the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllexport)

	#else // ! TNCOMMON_EXPORTS

		// default behavior is to import the functions from the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllimport)

	#endif // ! TNCOMMON_EXPORTS
#endif // ! LOCAL_TNCOMMON

#ifndef DEBUG
	#ifdef _DEBUG
		#define DEBUG
	#endif // _DEBUG
#endif // DEBUG not defined



// Initialize these to empty strings
#ifndef DEBUG_MODULE
#define DEBUG_MODULE	""
#endif // not DEBUG_MODULE

#ifndef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // not DEBUG_SECTION




//==================================================================================
// Debugging help
//==================================================================================
// You can dump this string from a debugger to get some useful information and
// tips for debugging.
extern char		g_szDebugHelp[];

#ifdef DEBUG
// This points the dynamically allocated debug log help.
extern char*	g_pszDebugLogHelp;
#endif // DEBUG




//==================================================================================
// Useful macros based on some DNet code (which was taken from code by ToddLa)
//==================================================================================
//
// Macros that generate compile time messages.  Use these with #pragma:
//
//	#pragma TODO(vanceo, "Fix this later")
//	#pragma BUGBUG(vanceo, "Busted!")
//	#pragma WAITFORDEVFIX(vanceo, millen, 666, "DPLAY: Broken")
//	#pragma PRINTVALUE(DPERR_SOMETHING)
//
// To turn them off, define TODO_OFF, BUGBUG_OFF, WAITFORDEVFIX_OFF, PRINTVALUE_OFF
// in your project preprocessor defines.
//
//
// If we're building under VC, (you should define BUILDENV_VISUALC), these expand to
// look like:
//
//	D:\directory\file.cpp(101) : BUGBUG: vanceo: Busted!
//
// in your output window, and you should be able to double click on it to jump
// directly to that location (line 101 of D:\directory\file.cpp).
//
// If we're building under the NT build environment, these expand to look like:
//
//	BUGBUG: vanceo: D:\directory\file.cpp(101) : Busted!
//
// because (at least right now) the build process thinks that a failure occurred if
// a message beginning with a filename and line number is printed.  It used to work
// just fine, but who knows.
//

#ifdef BUILDENV_VISUALC
#define __TODO(user, msgstr, n)								message(__FILE__ "(" #n ") : TODO: " #user ": " msgstr)
#define __BUGBUG(user, msgstr, n)							message(__FILE__ "(" #n ") : BUGBUG: " #user ": " msgstr)
#define __WAITFORDEVFIX(user, bugdb, bugnum, titlestr, n)	message(__FILE__ "(" #n ") : WAITFORDEVFIX: " #user ": " #bugdb " bug #" #bugnum" : " titlestr)
#define __PRINTVALUE(itemnamestr, itemvaluestr, n)			message(__FILE__ "(" #n ") : PRINTVALUE: " itemnamestr " = " itemvaluestr)
#else // ! BUILDENV_VISUALC
#define __TODO(user, msgstr, n)								message("TODO: " #user ": " __FILE__ "(" #n ") : " msgstr)
#define __BUGBUG(user, msgstr, n)							message("BUGBUG: " #user ": " __FILE__ "(" #n ") : " msgstr)
#define __WAITFORDEVFIX(user, bugdb, bugnum, titlestr, n)	message("WAITFORDEVFIX: " #user ": " __FILE__ "(" #n ") : " #bugdb " bug #" #bugnum" : " titlestr)
#define __PRINTVALUE(itemnamestr, itemvaluestr, n)			message("PRINTVALUE: " __FILE__ "(" #n ") : " itemnamestr " = " itemvaluestr)
#endif // ! BUILDENV_VISUALC


#define _TODO(user, msgstr, n)								__TODO(user, msgstr, n)
#define _BUGBUG(user, msgstr, n)							__BUGBUG(user, msgstr, n)
#define _WAITFORDEVFIX(user, bugdb, bugnum, titlestr, n)	__WAITFORDEVFIX(user, bugdb, bugnum, titlestr, n)
#define _PRINTVALUE(itemstr, item, n)						__PRINTVALUE(itemstr, #item, n)


#ifdef TODO_OFF
#define TODO(user, msgstr)
#else
#define TODO(user, msgstr)									_TODO(user, msgstr, __LINE__)
#endif

#ifdef BUGBUG_OFF
#define BUGBUG(user, msgstr)
#else
#define BUGBUG(user, msgstr)								_BUGBUG(user, msgstr, __LINE__)
#endif

#ifdef WAITFORDEVFIX_OFF
#define WAITFORDEVFIX(user, bugdb, bugnum, titlestr)
#else
#define WAITFORDEVFIX(user, bugdb, bugnum, titlestr)		_WAITFORDEVFIX(user, bugdb, bugnum, titlestr, __LINE__)
#endif

#ifdef PRINTVALUE_OFF
#define PRINTVALUE(item)
#else
#define PRINTVALUE(item)					_PRINTVALUE(#item, item, __LINE__)
#endif



// If debug is specified, these will be functions.  Otherwise, these will simply
// become no-ops.
#ifdef DEBUG


//==================================================================================
// External prototypes when DEBUG is specified
//==================================================================================
DLLEXPORT void TNDebugInitialize(void);

DLLEXPORT void TNDebugCleanup(void);


DLLEXPORT void TNDebugTakeLock(void);

DLLEXPORT void TNDebugDropLock(void);


DLLEXPORT void TNDebugSetDebuggerLogLevel(int iLevelMin, int iLevelMax);


DLLEXPORT void TNDebugStartFileLog(LPTSTR lptszFilepath, LPTSTR lptszHeaderString,
									BOOL fAppend, DWORD dwStartNewFileSize);

DLLEXPORT HRESULT TNDebugGetCurrentFileLogPath(LPTSTR lptszFilepath, DWORD* pdwFilepathSize);

DLLEXPORT void TNDebugEndFileLog(void);

DLLEXPORT void TNDebugSetFileLogLevel(int iLevelMin, int iLevelMax);


DLLEXPORT void TNDebugStartMemoryLog(DWORD dwNumEntries);

DLLEXPORT void TNDebugEndMemoryLog(void);

DLLEXPORT void TNDebugSetMemoryLogLevel(int iLevelMin, int iLevelMax);

DLLEXPORT void TNDebugDumpMemoryLogToFile(LPTSTR lptszFilepath, LPTSTR lptszHeaderString,
										BOOL fAppend);

DLLEXPORT void TNDebugDumpMemoryLogToString(char* szString, DWORD dwStringSize);

DLLEXPORT void TNDebugDisplayNewMemLogEntries(BOOL fToDebuggerIfOn, BOOL fToFileIfOn);


DLLEXPORT void TNDebugInitializeMemLogSharing(LPTSTR tszSharingKey);

DLLEXPORT void TNDebugCleanupMemLogSharing(void);


DLLEXPORT void TNDebugConnectDPlayLogging(void);

DLLEXPORT void TNDebugDisconnectDPlayLogging(void);

DLLEXPORT void TNDebugGrabNewDPlayEntries(void);



DLLEXPORT BOOL TNDebugGetThreadBlockNote(DWORD dwThreadID, LPTSTR lptszModule,
										 LPTSTR lptszSection, LPTSTR lptszFile,
										 int* lpiLineNum, LPTSTR lptszInfoString,
										 DWORD* lpdwNoteAge);



//==================================================================================
// Macros when DEBUG is specified
//==================================================================================
// This is the main macro to use
#define DPL				TNInternalDebugPrepLog(DEBUG_MODULE, DEBUG_SECTION), \
						TNInternalDebugPrintLine

#define DNB(string)		TNInternalDebugNoteBlock(DEBUG_MODULE, DEBUG_SECTION, __FILE__, __LINE__, string)


#ifdef DEBUGBREAK_INT3
  #ifdef _X86_
    #define DEBUGBREAK()	{_asm int 3}
  #else // _X86_
    #pragma message("You can't use int 3 debug breaks on non-x86 builds, defaulting to DebugBreak()!")
    #define DEBUGBREAK()	DebugBreak()
  #endif // _X86_
#else
  #ifdef DEBUGBREAK_OS
    #define DEBUGBREAK()	DebugBreak()
  #else
    #define DEBUGBREAK()	*((DWORD*) (0xDEADDEED)) = 0xBAADD00D
  #endif // DEBUGBREAK_INT3
#endif // DEBUGBREAK_INT3



//==================================================================================
// More prototypes when DEBUG is specified, use macro instead
//==================================================================================
DLLEXPORT void TNInternalDebugPrepLog(LPTSTR lptszModule, LPTSTR lptszSection);

DLLEXPORT void TNInternalDebugPrintLine(int iLevel, LPTSTR lptszFormatString,
										DWORD dwNumParms, ... );

DLLEXPORT void TNInternalDebugNoteBlock(LPTSTR lptszModule, LPTSTR lptszSection,
										LPTSTR lptszFile, int iLineNum,
										LPTSTR lptszInfoString);



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#else // ! DEBUG
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


//==================================================================================
// External prototypes when DEBUG is not specified
//==================================================================================
#define TNDebugInitialize()

#define TNDebugCleanup()


#define TNDebugTakeLock()

#define TNDebugDropLock()

#define TNDebugSetDebuggerLogLevel(iLevelMin, iLevelMax)


#define TNDebugStartFileLog(lptszFilepath, lptszHeaderString, fAppend, dwStartNewFileSize)

// Note that the single = sign is that way on purpose
#define TNDebugGetCurrentFileLogPath(lptszFilepath, pdwFilepathSize)	(((*pdwFilepathSize) = 0) ? ERROR_BAD_ENVIRONMENT : ERROR_BAD_ENVIRONMENT)

#define TNDebugEndFileLog()

#define TNDebugSetFileLogLevel(iLevelMin, iLevelMax)


#define TNDebugStartMemoryLog(dwNumEntries)

#define TNDebugEndMemoryLog()

#define TNDebugSetMemoryLogLevel(iLevelMin, iLevelMax)

#define TNDebugDumpMemoryLogToFile(lptszFilepath, lpszHeaderString, fAppend)

#define TNDebugDumpMemoryLogToString(szString, dwStringSize)

#define TNDebugDisplayNewMemLogEntries(fToDebuggerIfOn, fToFileIfOn)


#define TNDebugInitializeMemLogSharing(tszSharingKey)

#define TNDebugCleanupMemLogSharing()


#define TNDebugConnectDPlayLogging()

#define TNDebugDisconnectDPlayLogging()

#define TNDebugGrabNewDPlayEntries()



#define TNDebugGetThreadBlockNote(dwThreadID, lptszModule, lptszSection, lptszFile,\
								lpiLineNum, lptszInfoString, lpdwNoteAge)				(FALSE)



//==================================================================================
// Macros when DEBUG is not specified
//==================================================================================
#define DPL
	
#define DNB(string)

#define DEBUGBREAK()

#endif // ! DEBUG

#ifdef _XBOX
DLLEXPORT void WINAPI DebugPrint (char *format, ...);
#endif

#endif // __DEBUGPRINT__
