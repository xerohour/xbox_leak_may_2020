/* yvals.h values header for Microsoft C/C++ */
#pragma once
#ifndef _YVALS
#define _YVALS

#define _CPPLIB_VER	309

 #pragma warning(disable: 4514 4710)

#define _MT_LOCKS		_MT	/* nontrivial locks if multithreaded */

 #ifndef _HAS_EXCEPTIONS
  #define  _HAS_EXCEPTIONS  1	/* predefine as 0 to disable exceptions */
 #endif

 #define _GLOBAL_USING	1
 #define _HAS_MEMBER_TEMPLATES_REBIND	0
 #define _HAS_TEMPLATE_PARTIAL_ORDERING	0

#include <use_ansi.h>

#ifndef _VC6SP2
 #define _VC6SP2	0 /* define as 1 to fix linker errors with V6.0 SP2 */
#endif

/* Define _CRTIMP2 */
 #ifndef _CRTIMP2
   #if defined(_DLL) && !defined(_STATIC_CPPLIB)
    #define _CRTIMP2	__declspec(dllimport)
   #else   /* ndef _DLL && !STATIC_CPPLIB */
    #define _CRTIMP2
   #endif  /* _DLL && !STATIC_CPPLIB */
 #endif  /* _CRTIMP2 */

 #if defined(_DLL) && !defined(_STATIC_CPPLIB)
  #define _DLL_CPPLIB
 #endif


 #if (1300 <= _MSC_VER)
  #define _DEPRECATED	__declspec(deprecated)
 #else
  #define _DEPRECATED
 #endif

		/* NAMESPACE */
 #if defined(__cplusplus)
  #define _CSTD			::
  #define _STD			std::
  #define _STD_BEGIN	namespace std {
  #define _STD_END		};
//  #define _STD_USING
 #else
  #define _CSTD
  #define _STD
  #define _STD_BEGIN
  #define _STD_END
 #endif /* __cplusplus */

		/* match _STD_BEGIN/END if *.c compiled as C++ */
  #define _C_STD_BEGIN
  #define _C_STD_END

_STD_BEGIN
		/* TYPE bool */
 #if defined(__cplusplus)
typedef bool _Bool;
 #endif /* __cplusplus */

		/* INTEGER PROPERTIES */
#define _LONGLONG	__int64
#define _ULONGLONG	unsigned __int64
#define _GETLL(x)	_atoi64(x)
#define _GETULL(x)	_atoi64(x)
#define _LLONG_MAX	0x7fffffffffffffff

#define _MAX_EXP_DIG	8	/* for parsing numerics */
#define _MAX_INT_DIG	32
#define _MAX_SIG_DIG	36

		/* STDIO PROPERTIES */
#define _Filet _iobuf
 #ifndef _FPOS_T_DEFINED
  #define _FPOSOFF(fp)	((long)(fp))
 #endif /* _FPOS_T_DEFINED */
#define _IOBASE	_base
#define _IOPTR	_ptr
#define _IOCNT	_cnt

		/* NAMING PROPERTIES */
 #if defined(__cplusplus)
  #define _C_LIB_DECL extern "C" {
  #define _END_C_LIB_DECL }
 #else
  #define _C_LIB_DECL
  #define _END_C_LIB_DECL
 #endif /* __cplusplus */

 #if defined(__cplusplus)
		// LOCK MACROS
#define _LOCK_LOCALE	0
#define _LOCK_MALLOC	1
#define _LOCK_STREAM	2

		// CLASS _Lockit
class _CRTIMP2 _Lockit
	{	// lock while object in existence
public:
  #if _MT_LOCKS
	explicit _Lockit();	// set default lock
	explicit _Lockit(int);	// set the lock
	~_Lockit();	// clear the lock

private:
	_Lockit(const _Lockit&);				// not defined
	_Lockit& operator = (const _Lockit&);	// not defined

  #else /* _MT_LOCKS */
   #define _LOCKIT(x)
	explicit _Lockit()
		{	// do nothing
		}

	explicit _Lockit(int)
		{	// do nothing
		}

	~_Lockit()
		{	// do nothing
		}
  #endif /* _MT_LOCKS */
	};

class _CRTIMP2 _Mutex
	{	// lock under program control
public:
  #if _MT_LOCKS
	_Mutex();
	~_Mutex();
	void _Lock();
	void _Unlock();

private:
	_Mutex(const _Mutex&);				// not defined
	_Mutex& operator = (const _Mutex&);	// not defined
	void *_Mtx;
  #else /* _MT_LOCKS */
    void _Lock()
		{	// do nothing
		}

	void _Unlock()
		{	// do nothing
		}
  #endif /* _MT_LOCKS */
	};

class _Init_locks
	{	// initialize mutexes
public:
 #if _MT_LOCKS
	_Init_locks();
	~_Init_locks();
 #else /* _MT_LOCKS */
	_Init_locks()
		{	// do nothing
		}

	~_Init_locks()
		{	// do nothing
		}
 #endif /* _MT_LOCKS */ 
	};
 #endif /* __cplusplus */


		/* MISCELLANEOUS MACROS AND TYPES */
typedef int _Mbstatet;
#define _ATEXIT_T	void
#define _Mbstinit(x)	mbstate_t x = {0}
#define _Loc_atexit	_Loc_atexit

#define _cpp_max	max
#define _cpp_min	min
_STD_END
#endif /* _YVALS */

/*
* Copyright (c) 1992-2000 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V3.09:0009 */
