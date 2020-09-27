//
// INTEL CONFIDENTIAL.  Copyright (c) 1997-2000 Intel Corp. All rights reserved.
//
#if !defined(_WTLIB_H_)
#define _WTLIB_H_

// Only the TESTLIBC library must define the TESTDLL symbol so it will
// export its DLL functions.  All other files that include WTLIB.H will
// try to link to the correct debug/release DLL and import its functions.
//
#if defined(TESTDLL)
	#define DLLEXPORT __declspec(dllexport)
#else  
	#define DLLEXPORT __declspec(dllimport)
	#if defined (WIN64)
		#if defined(_DEBUG)
			#pragma comment(lib,"wtlib64d.lib")
		#else
			#pragma comment(lib,"wtlib64c.lib")
		#endif
	#else
		#if defined(_DEBUG)
			#pragma comment(lib,"testlibd.lib")
		#else
			#pragma comment(lib,"testlibc.lib")
		#endif
	#endif
#endif

// All WinMTA modules and libraries should dynamically link to the C-runtime library.
// This check ensures the use of the correct compiler flags.
//
#if !defined(_MT) || !defined(_DLL)
	#if defined(_DEBUG)
		#error The /MDd compiler switch is required (Use debug multithreaded C-runtime DLL)
	#else
		#error The /MD compiler switch is required (Use release multithreaded C-runtime DLL)
	#endif
#endif

// If MFC is used, make sure it is the shared DLL.
//
#if defined(__AFXWIN_H__) && !defined(_AFXDLL)
	#error When MFC is used, it should be from the shared DLL, not the static library.
#endif

#define TESTLIB_VERSION "version_number=4.00 "

// return values for prompt failures
//  PROMPT_CANCEL and PROMPT_TIMEOUT are handled by the library
//  and do not need to be tested by the caller.
//
#define PROMPT_OK			1
#define PROMPT_CANCEL		0

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * service call prototypes
 */
DLLEXPORT BOOL APIENTRY DllMain(HINSTANCE hModule,DWORD dwReason,LPVOID /*lpReserved*/);
DLLEXPORT BOOL wt_initialize(int* ptestnum);
DLLEXPORT void wt_set_cleanup(void(*cleanup)());
DLLEXPORT void wt_state(int state);
DLLEXPORT void wt_progress(int progress);
DLLEXPORT void wt_done();

DLLEXPORT int  wt_advisory(LPCTSTR format_string,...);
DLLEXPORT int  wt_error(DWORD ErrorCode, LPCTSTR format_string,...);
DLLEXPORT int  wt_statistic(DWORD StatCode,LPCTSTR format_string,...);
DLLEXPORT int  wt_status(LPCTSTR format_string,...);
DLLEXPORT int  wt_info(LPCTSTR format_string,...);
DLLEXPORT int  wt_verbose(DWORD mask,LPCTSTR format_string,...);

DLLEXPORT BOOL wt_getint(LPCTSTR key,int* pvalue);
DLLEXPORT BOOL wt_getuint(LPCTSTR key,UINT* pvalue);
DLLEXPORT BOOL wt_getstring(LPCTSTR key,LPTSTR value,DWORD length);
DLLEXPORT BOOL wt_putint (LPCTSTR path, LPCTSTR key, int pvalue);
DLLEXPORT BOOL wt_putuint (LPCTSTR path, LPCTSTR key, UINT pvalue);
DLLEXPORT BOOL wt_putstring (LPCTSTR path, LPCTSTR key, LPCTSTR value);

DLLEXPORT BOOL wt_remove_group(LPCTSTR path);
DLLEXPORT BOOL wt_copy_group(LPCTSTR path,LPCTSTR name,LPTSTR newPath,int length);
DLLEXPORT BOOL wt_remove_subtest(LPCTSTR path,int testNumber);
DLLEXPORT BOOL wt_disable_subtest(LPCTSTR path,int testNumber);

DLLEXPORT int  wt_prompt_list(int *choice,int count,const LPCTSTR* list,LPCTSTR prompt);
DLLEXPORT int  wt_prompt_string(LPTSTR buffer, int length, LPCTSTR prompt);
DLLEXPORT int  wt_prompt_confirm(BOOL *yesno, LPCTSTR prompt);
DLLEXPORT int  wt_wait_keypress(UINT* scancode, UCHAR* keycode);

#ifdef __cplusplus
}
#endif

#endif /* _WTLIB_H_ */
