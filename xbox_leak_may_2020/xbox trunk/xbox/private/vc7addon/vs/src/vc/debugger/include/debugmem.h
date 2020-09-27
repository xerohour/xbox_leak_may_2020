#ifndef _debugmem_h
#define _debugmem_h

// redefine the old OSD macros to use the debug CRT heap

#include <malloc.h>
#include <crtdbg.h>

#ifdef _CRTDBG_MAP_ALLOC
#error Dont define _CRTDBG_MAP_ALLOC when using debugmem.h
#endif

// filenames point to DLLs which get unloaded by the time we exit, so we
// have to dupe them, but efficiently
extern const char * SafeDupeFilename( const char *x );

#define MHAlloc(x)   (_malloc_dbg(x,_NORMAL_BLOCK,SafeDupeFilename(__FILE__),__LINE__))
#define MHRealloc(a,b) (_realloc_dbg(a,b,_NORMAL_BLOCK,SafeDupeFilename(__FILE__),__LINE__))
#define MHFree(y)    (_free_dbg(y,_NORMAL_BLOCK))

#define MHAllocHuge(x)   (_malloc_dbg(x,_NORMAL_BLOCK,SafeDupeFilename(__FILE__),__LINE__))
#define MHFreeHuge(y)    (_free_dbg(y,_NORMAL_BLOCK))

#define MMAlloc(x)   (_malloc_dbg(x,_NORMAL_BLOCK,SafeDupeFilename(__FILE__),__LINE__))
#define MMFree(y)    (_free_dbg(y,_NORMAL_BLOCK))
#define	MMLock(a)	(LPVOID)(a)
#define	MMUnlock(a)

// exactly like _CRTDBG_MAP_ALLOC but wraps filename

#if defined(_DEBUG)

#if !defined(DONT_REDEFINE_MALLOC_ET_ALL)

#define   malloc(s)         _malloc_dbg(s, _NORMAL_BLOCK, SafeDupeFilename(__FILE__), __LINE__)
#define   calloc(c, s)      _calloc_dbg(c, s, _NORMAL_BLOCK, SafeDupeFilename(__FILE__), __LINE__)
#define   realloc(p, s)     _realloc_dbg(p, s, _NORMAL_BLOCK, SafeDupeFilename(__FILE__), __LINE__)
#define   _expand(p, s)     _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   free(p)           _free_dbg(p, _NORMAL_BLOCK)
#define   _msize(p)         _msize_dbg(p, _NORMAL_BLOCK)

#endif

#include <util\critleak.h>

#if defined(__cplusplus) && !defined(DONT_REDEFINE_DEBUG_NEW)

void* 
__cdecl operator new(
	size_t s, 
	const char *filename, 
	int nLine
	);

void 
__cdecl operator delete(
	void *p, 
	LPCSTR lpszFileName, 
	int nLine
	);

void* __cdecl 
operator new[] (
	size_t, 
	const char *, 
	int
	);

void __cdecl 
operator delete[](
	void *, 
	const char *, 
	int
	);

#define DEBUG_NEW new(__FILE__, __LINE__)

#if !defined(DONT_AUTODEFINE_DEBUG_NEW)
#define	new DEBUG_NEW
#endif

#endif // __cplusplus
#endif // _DEBUG

#endif
