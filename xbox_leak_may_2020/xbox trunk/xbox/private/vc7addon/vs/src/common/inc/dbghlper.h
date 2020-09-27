
// Note: this exists in both v5x.pro and v7 LANGAPIs - both must match
// [created 6/12/97 by apennell]

#ifndef _INC_DBGHLPER_H
#define _INC_DBGHLPER_H

// these prototypes might end up in a public header file someday

EXTERN_C BOOL WINAPI SetThreadName( DWORD dwThread, LPCSTR szThreadName, DWORD dwFlags );


////////////////////////////////////////////////////////////////////////////
// this is the private data used by the runtime and read by the debugger  //
////////////////////////////////////////////////////////////////////////////

// change the enum value whenever an underlying struct changes
enum EXCEPTION_DEBUGGER_ENUM
{
	EXCEPTION_DEBUGGER_NAME_THREAD	=	0x1000,
	EXCEPTION_DEBUGGER_PROBE		=	0x1001,
	EXCEPTION_DEBUGGER_RUNTIMECHECK	=	0x1002,
	EXCEPTION_DEBUGGER_FIBER		=	0x1003,

	EXCEPTION_DEBUGGER_MAX	= 0x1003				// largest value this debugger understands
};

// must be convertible to DWORDs for use by RaiseException
typedef struct tagEXCEPTION_VISUALCPP_DEBUG_INFO
{
	DWORD	dwType;						// one of the enums from above
	union
	{
		struct
		{
			LPCSTR	szName;				// pointer to name (in user addr space)
			DWORD	dwThreadID;			// thread ID (-1=caller thread)
			DWORD	dwFlags;			// reserved for future use (eg User thread, System thread)
		} SetName;

		struct
		{
			DWORD	dwLevelRequired;	// 0 = do you understand this private exception, else max value of enum
			PBYTE	pbDebuggerPresent;	// debugger puts a non-zero value in this address if there
		} DebuggerProbe;

		struct
		{
			DWORD	dwRuntimeNumber;		// the type of the runtime check
			BOOL	bRealBug;				// TRUE if never a false-positive
			PVOID	pvReturnAddress;		// caller puts a return address in here
			PBYTE	pbDebuggerPresent;		// debugger puts a non-zero value in this address if handled it
			LPCWSTR	pwRuntimeMessage;		// pointer to Unicode message (or NULL)
		} RuntimeError;

		struct
		{
			DWORD	dwType;					// 0=ConvertThreadToFiber, 1=CreateFiber, 2=DeleteFiber
			PVOID	pvFiber;				// pointer to fiber
			PVOID	pvStartRoutine;			// pointer to FIBER_START_ROUTINE (CreateFiber only)
		} FiberInfo;
	};
} EXCEPTION_VISUALCPP_DEBUG_INFO;

#endif
