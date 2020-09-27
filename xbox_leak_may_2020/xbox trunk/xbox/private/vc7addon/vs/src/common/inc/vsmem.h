//---------------------------------------------------------------------------
// Microsoft Visual Studio
//
// Microsoft Confidential
// Copyright (C) 1994 - 2000 Microsoft Corporation. 
// All Rights Reserved.
//
// VsMem.H
//
// Common memory allocation routines
//---------------------------------------------------------------------------
#ifndef _INC_VSMEM_H
#define _INC_VSMEM_H

PVOID WINAPI VSRetAlloc (SIZE_T cb);
PVOID WINAPI VSRetAllocZero (SIZE_T cb);
PVOID WINAPI VSRetRealloc (PVOID pv, SIZE_T cb);
PVOID WINAPI VSRetReallocZero (PVOID pv, SIZE_T cb);
BOOL  WINAPI VSRetFree (PVOID pv);
SIZE_T WINAPI VSRetSize (PVOID pv);

#if DEBUG

//=----------------------------------------------------------------------=
// Debug allocation routines.

#include "vsassert.h"

// We don't want people to be using non-debug allocators.  So, we generate
// compile errors when they do
//
#define calloc(num, size)       0; VSCASSERT(0, Should_be_using_debug_allocators)
#define malloc(a)               0; VSCASSERT(0, Should_be_using_debug_allocators)
#define realloc(a, b)           0; VSCASSERT(0, Should_be_using_debug_allocators)
#define free(a)                    VSCASSERT(0, Should_be_using_debug_allocators)
#define HeapAlloc(h, f, b)      0; VSCASSERT(0, Should_be_using_debug_allocators)
#define HeapReAlloc(h, f, m, b) 0; VSCASSERT(0, Should_be_using_debug_allocators)
#define HeapFree(h, f, m)          VSCASSERT(0, Should_be_using_debug_allocators)

#define VSAlloc(cb)           VsDebAlloc(0, cb)
#define VSAllocZero(cb)       VsDebAlloc(HEAP_ZERO_MEMORY, cb)
#define VSRealloc(pv, cb)     VsDebRealloc(pv, 0, cb)
#define VSReallocZero(pv, cb) VsDebRealloc(pv, HEAP_ZERO_MEMORY, cb)
#define VSFree(pv)            VsDebFree(pv)
#define VSSize(pv)            VsDebSize(pv)

#define VSHeapCreate(flags, name)       VsDebHeapCreate(flags, name)
#define VSHeapDestroy(heap, fLeakCheck) VsDebHeapDestroy(heap, fLeakCheck)
#define VSHeapAlloc(heap, cb)           VsDebHeapAlloc(heap, 0, cb)
#define VSHeapAllocZero(heap, cb)       VsDebHeapAlloc(heap, HEAP_ZERO_MEMORY, cb)
#define VSHeapRealloc(heap, pv, cb)     VsDebHeapRealloc(heap, pv, 0, cb)
#define VSHeapReallocZero(heap, pv, cb) VsDebHeapRealloc(heap, pv, HEAP_ZERO_MEMORY, cb)
#define VSHeapFree(heap, pv)            VsDebHeapFree(heap, pv)
#define VSHeapSize(heap, pv)            VsDebHeapSize(heap, pv)

#define VSOleAlloc(cb)        VsDebOleAlloc(cb)
#define VSOleFree(pv)         VsDebOleFree(pv)
#define VSOleRealloc(pv, cb)  VsDebOleRealloc(pv, cb)

#define CoTaskMemAlloc(cb)        VsDebOleAlloc(cb)
#define CoTaskMemRealloc(pv, cb)  VsDebOleRealloc(pv, cb)
#define CoTaskMemFree(pv)         VsDebOleFree(pv)

#define SysAllocString(str)             VsDebSysAllocString((str))
#define SysAllocStringByteLen(str, cb)  VsDebSysAllocStringByteLen((str), (cb))
#define SysAllocStringLen(str, cch)     VsDebSysAllocStringLen((str), (cch))

// Wrappers for ComDlg functions...
#define GetOpenFileNameA	VsGetOpenFileNameA
#define GetSaveFileNameA	VsGetSaveFileNameA
#define GetFileTitleA		VsFileTitleA
#define ChooseColorA		VsChooseColorA
#define FindTextA		VsFindTextA
#define ReplaceTextA		VsReplaceTextA
#define ChooseFontA		VsChooseFontA
#define PrintDlgA		VsPrintDlgA
#define CommDlgExtendedError	VsCommDlgExtendedError
#define PageSetupDlgA		VsPageSetupDlgA

// Wrapper for CoCreateInstance (but if already defined to VBCoCreateInstance, don't.
// since VBCoCreateInstance does some useful but shell specific error checking)
#ifndef CoCreateInstance
#define CoCreateInstance	VsCoCreateInstance
#endif //ifndef CoCreateInstance

// We redefine new and delete to use the debug allocators, and make a 'new'
// macro to call the debug operator.  The standard runtime prototype for
// new is overloaded as well in vsmem.cpp just in case...
//
#undef new
void * _cdecl operator new(size_t size, LPSTR pszFile, UINT uLine);
#if _MSC_VER > 1200
void * _cdecl operator new[](size_t size, LPSTR pszFile, UINT uLine);
#endif
#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void *__cdecl operator new( size_t, void *_P) {return (_P); }
#if _MSC_VER > 1200
inline void *__cdecl operator new[]( size_t, void *_P) {return (_P); }
#endif
#endif

/*
    Want to define your own new operator for your class, without losing position
    info in leak detection?  Here's how:

    In your class header file:

            #undef new

            class CMyClassWithOverloadedNew
            {
            public:
                DECLARE_CLASS_NEW(iSize) { MyOwnAllocator.Allocate (iSize); }   // OR:
                DECLARE_CLASS_NEW(iSize);                                       // If not inlined (see below)
            };

            #define new vs_new

    In your implementation file:

            #undef new
            DEFINE_CLASS_NEW (CMyClassWithOverloadedNew, iSize)
            {
                DEBUGOUT ("%s(%d) : %d bytes allocated", pszFile, uLine);   // NOTE:  pszFile and uLine are only available ifdef _DEBUG!
                MyAllocator.Allocate (iSize);
            }
            #define new vs_new

    By doing this, you can define/declare operator new and get 'new' #define'd back to
    the appropriate thing so other uses of new get tracked by vsmem appropriately.  (Your
    allocation scheme will not be tracked by vsmem, of course.)

    Note that if your new overload takes its own parameters, you must undef new and
    leave it undefined.  You can then use 'vs_new' to new objects other than your
    class to get them tracked appropriately (otherwise, if they leak, vsmem.cpp(25) will
    be the offending line).

*/

#define DECLARE_CLASS_NEW(sizevar) void * _cdecl operator new (size_t sizevar, LPSTR pszFile, UINT uLine)
#define DEFINE_CLASS_NEW(c,sizevar) void * _cdecl c::operator new (size_t sizevar, LPSTR pszFile, UINT uLine)

#define vs_new new(__FILE__, __LINE__)
#define new vs_new

#else

//=----------------------------------------------------------------------=
// Retail allocation routines
//

#define VSAlloc(cb)       VSRetAlloc(cb)
#define VSAllocZero(cb)   VSRetAllocZero(cb)
#define VSRealloc(pv, cb) VSRetRealloc(pv, cb)
#define VSReallocZero(pv, cb) VSRetReallocZero(pv, cb)
#define VSFree(pv)        VSRetFree(pv)
#define VSSize(pv)        VSRetSize(pv)

#define VSHeapCreate(flags, name)       HeapCreate(flags, 0, 0)
#define VSHeapDestroy(heap, fLeakCheck) HeapDestroy(heap)
#define VSHeapAlloc(heap, cb)           HeapAlloc(heap, 0, cb)
#define VSHeapAllocZero(heap, cb)       HeapAlloc(heap, HEAP_ZERO_MEMORY, cb)
#define VSHeapRealloc(heap, pv, cb)     HeapReAlloc(heap, 0, pv, cb)
#define VSHeapReallocZero(heap, pv, cb) HeapReAlloc(heap, HEAP_ZERO_MEMORY, pv, cb)
#define VSHeapFree(heap, pv)            HeapFree(heap, 0, pv)
#define VSHeapSize(heap, pv)            HeapSize(heap, 0, pv)

#define VSOleAlloc(cb)        CoTaskMemAlloc(cb)
#define VSOleFree(pv)         CoTaskMemFree(pv)
#define VSOleRealloc(pv, cb)  CoTaskMemRealloc(pv, cb)

#define vs_new new
#define DECLARE_CLASS_NEW(sizevar) void * _cdecl operator new (size_t sizevar)
#define DEFINE_CLASS_NEW(c,sizevar) void * _cdecl c::operator new (size_t sizevar)

#endif // DEBUG

#endif // _INC_VSMEM_H

