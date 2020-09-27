#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tchar.h>

//These flags are OR'ed for the 3rd parameter to triage
#define FLAG_NO_SYMBOLS (DWORD)1        //It tells triage not to try to fix symbols
                                    

// For people who want to dynamically link
typedef PCHAR (*TRIAGEACALL)(PCHAR, PCHAR, DWORD, PCHAR);
typedef PWCHAR (*TRIAGEWCALL)(PWCHAR, PWCHAR, DWORD, PWCHAR);

// Lets make life easier for them
#ifdef UNICODE
    typedef PWCHAR (*TRIAGECALL)(PWCHAR, PWCHAR, DWORD, PWCHAR);
#else
    typedef PCHAR (*TRIAGECALL)(PCHAR, PCHAR, DWORD, PCHAR);
#endif

// This should be called to release any memory that triage allocated on your behalf.(ie. return buffer)
typedef VOID  (*TRIAGECLEANUP)();

/*
#ifndef MYLIBAPI
    #define MYLIBAPI __declspec(dllimport)
#else
    #define MYLIBAPI __declspec(dllexport)
#endif
*/

// The actual prototype
// Parameter    1 - Debugger Name.  Do not include the '\\'.  Just csdbg not \\csdbg.
//              2 - NamedPipe name.  This is the second parameter on the remote line.
//              3 - A bit mask.  The only bit defined now is FLAG_NO_SYMBOLS.  See Above
//              4 - An alternate ini file name.  Currently triage.ini is the default.  NULL is OK.
// Returns - A buffer containing the formatted failure infomation.  This should be freed using TriageCleanup()


#if defined(__cplusplus)
extern "C"
{
	__declspec( dllexport ) PCHAR TriageA(PCHAR, PCHAR, DWORD, PCHAR);
	__declspec( dllexport ) PWCHAR TriageW(PWCHAR, PWCHAR, DWORD, PWCHAR);
	__declspec( dllexport ) VOID TriageCleanup();
//	MYLIBAPI PCHAR TriageA(PCHAR, PCHAR, DWORD, PCHAR);
//	MYLIBAPI PWCHAR TriageW(PWCHAR, PWCHAR, DWORD, PWCHAR);
//	MYLIBAPI VOID TriageCleanup();
}
#else 
	__declspec( dllexport ) PCHAR TriageA(PCHAR, PCHAR, DWORD, PCHAR);
	__declspec( dllexport ) PWCHAR TriageW(PWCHAR, PWCHAR, DWORD, PWCHAR);
	__declspec( dllexport ) VOID TriageCleanup();
//	MYLIBAPI PCHAR TriageA(PCHAR, PCHAR, DWORD, PCHAR);
//	MYLIBAPI PWCHAR TriageW(PWCHAR, PWCHAR, DWORD, PWCHAR);
//	MYLIBAPI VOID TriageCleanup();
#endif

// Let them be flexible
#ifdef UNICODE
#define Triage  TriageW
#else
#define Triage  TriageA
#endif


