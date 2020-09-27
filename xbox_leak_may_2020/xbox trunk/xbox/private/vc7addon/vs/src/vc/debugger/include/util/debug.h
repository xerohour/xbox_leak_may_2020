/*++

Copyright (c) 1996 Microsoft Coporation

Module Name: debug.h

Abstract:

    Debugging macros and functions.

Author:

    Matthew D Hendel (math)

--*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//
// All asserts go through our assert
//

#ifdef ASSERT
#undef ASSERT
#endif

#ifdef assert
#undef assert
#endif

#ifdef VERIFY
#undef VERIFY
#endif

#ifdef verify
#undef verify
#endif

#define ASSERT(_expr)			Assert(_expr)
#define assert(_expr)			Assert(_expr)
#define VERIFY(_expr)			Verify(_expr)
#define verify(_expr)			Verify(_expr)

    //
    // For util headers
    //
#define dbgutilASSERT(_expr)	Assert(_expr)
#define dbgutilVERIFY(_expr)	Verify(_expr)

#define DbgPrint _DbgPrint

				
void
__stdcall
DbgPrint(
	const char*,
	...
	);

unsigned long
__stdcall
AssertFailed(
	const char* cond,
	const char* file,
	unsigned long line,
	unsigned int*   pIgnoreAssert
	
	);

	
unsigned long
AssertFailHr(
	const char* cond,
	const char* file,
	unsigned long line,
	HRESULT hr
	);

#ifdef DEBUG


// Debug only.

#ifdef i386

	//
    // int 3 is a little nicer than DebugBreak since it doesn't throw you
    // into mixed mode
	//

#define DbgBreakPoint()				\
				do {                \
					_asm int 3      \
				} while (0)

#else

#define DbgBreakPoint()     DebugBreak()

#endif // i386


#define Assert(_arg)\
			do {															\
				if (! (_arg) ) {											\
					static unsigned int __ignore_assert = 0;							\
					if (__ignore_assert == 0 &&								\
						AssertFailed (#_arg, __FILE__, __LINE__, &__ignore_assert)) {			\
						DbgBreakPoint ();									\
					}														\
				}															\
			} while (0)


#define AssertWithMsg(_arg,msg)\
			do {														    \
				if (! (_arg) ) {											\
					static unsigned int __ignore_assert = 0;							\
					if (__ignore_assert == 0 &&								\
						AssertFailed (msg, __FILE__, __LINE__, &__ignore_assert)) {	        \
						DbgBreakPoint ();									\
					}														\
				}															\
			} while (0)

#define AssertHr(_arg,_hr)\
			do {															\
				if ( !(_arg) ) {											\
					static unsigned int __ignore_assert = 0;							\
					if (__ignore_assert == 0 &&								\
						AssertFailHr (#_arg, __FILE__, __LINE__, _hr)) {	\
						DbgBreakPoint ();									\
					}														\
				}															\
			} while (0)
			

#define Verify(_arg)	Assert(_arg)
#if defined(KdPrint)
#undef  KdPrint
#endif
#define KdPrint(_arg)   DbgPrint _arg



VOID
WINAPI
_DbgCheckHeap(
	);
	
#define DbgCheckHeap _DbgCheckHeap

#else   // DEBUG

#define Verify(_arg)    (_arg)

//
// these go away in retail

#if defined(KdPrint)
#undef  KdPrint
#endif

#define KdPrint(_arg) do {} while (0)
#define Assert(_arg) do {} while (0)
#define AssertHr(_arg,_hr) do {} while (0)
#define DbgBreakPoint() do {} while (0)
#define DbgCheckHeap() do {} while (0)

#endif // DEBUG


#ifdef __cplusplus

__inline void
AssertWritePtr(
	void* ptr,
	UINT cb = 1
	)
{
	Assert (ptr == NULL || !IsBadWritePtr (ptr, cb));
}

__inline void
AssertReadPtr(
	void* ptr,
	UINT cb = 1
	)
{
	Assert (ptr == NULL || !IsBadReadPtr (ptr, cb));
}

#endif // __cplusplus


/*++ macro CT_ASSERT

Macro Description:

	CT_ASSERT is a compile-time assert. The expression given as the parameter
	must be able to be evaluated at compile time. Generally this will be a
	sizeof or offsetof expression.

	If this assert fails, the compile time error:

		error C2118: negative subscript or subscript is too large

	will be generated.

--*/

#pragma warning (disable:4094)
	
#define CT_ASSERT(_cond)						\
    struct {									\
	        int _unused [(_cond) ? 1 : -1];		\
	};											\


BOOL
WINAPI
IsCalledFrom(
	UINT_PTR CallerAddress,
	HMODULE hModule
	);


//
// This is a compiler intrinsic function which return the return address of
// the caller. Very usefule for tracking memory leaks and the like.
//

extern "C" void*
_ReturnAddress();

#pragma intrinsic (_ReturnAddress)

#if defined (i386)
//
// This is a retail assert, not to be shipped.  It should be removed before RC
//
#define RASSERT(expr) 							\
do {											\
	if (!(expr)) { 								\
		_asm int 3 							\
	}											\
} while (0)
#endif

	
#ifdef __cplusplus
};
#endif



