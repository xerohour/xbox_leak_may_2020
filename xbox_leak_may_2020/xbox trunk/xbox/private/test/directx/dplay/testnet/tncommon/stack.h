#ifndef __TNSTACK__
#define __TNSTACK__





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







//==================================================================================
// External Structures
//==================================================================================
typedef struct tagTNGETSTACKDATA
{
	DWORD	dwSize; // size of this structure, must be filled in prior to calling the function

	BOOL	fCallStack; // whether to display the call stack (using "StackWalk()") 
	PVOID	pvIgnoreCallsAboveFunction; // pointer to function that represents the top most valid function to list

	DWORD	dwRawStackDepth; // how many entries to display off the raw stack, or 0 for none
} TNGETSTACKDATA, * PTNGETSTACKDATA;




//==================================================================================
// Typedef for macro
//==================================================================================
typedef void (__stdcall * RTLGETCALLERSADDRESSPROC)		(PVOID*, PVOID*);




//==================================================================================
// External Prototypes
//==================================================================================
DLLEXPORT HRESULT TNStackGetStackForCurrentThread(PTNGETSTACKDATA pGetStackData);

DLLEXPORT RTLGETCALLERSADDRESSPROC TNStackGetRGCAProc(void); // see symbols.h for definition





//==================================================================================
// External Macro
//==================================================================================
#define TNStackGetCallersAddress(ppvCallersAddress, ppvCallersCaller)\
											{\
												RTLGETCALLERSADDRESSPROC	pfnRtlGetCallersAddress;\
												\
												pfnRtlGetCallersAddress = TNStackGetRGCAProc();\
												pfnRtlGetCallersAddress(ppvCallersAddress, ppvCallersCaller);\
											}




#endif // __TNSTACK__