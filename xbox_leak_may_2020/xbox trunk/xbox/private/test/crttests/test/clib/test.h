/* Test.h -- testing include file.
 *
 *              Copyright (C) Microsoft Corp. 1993
 *
 *
 *  This include file contains macros and functions for reporting
 *  test failures.
 *
 *  Required constants:
 *
 *      test     -- char array, name of test.
 *      perfect  -- int, test success/failure flag;
 *          0 => success, 1 => failure
 *
 *
 *  Macros/functions defined are:
 *
 *  check( e )      -- confirm that expression e is zero.
 *  checke( e1, e2 )    -- confirm that expressions e1 and e2 are equal.
 *  checkne( e1, e2 )   -- confirm that expressions e1 and e2 are NOT equal.
 *  check32( e )        -- confirm that expression e is zero.
 *  checke32( e1, e2 )  -- confirm that expressions e1 and e2 are equal.
 *  checkne32( e1, e2 ) -- confirm that expressions e1 and e2 are NOT equal.
 *  checkNULL( p )      -- confirm that pointer p is NULL.
 *  checknNULL( p )     -- confirm that pointer p is NOT NULL.
 *  fail( n )       -- report failure, test number specified.
 *  faill(  )       -- report failure, no test number specified.
 *  finish()        -- summarize test pass/fail status.
 *  WinLogString( p, ... )  -- Redefines printf to TextOut() and file write
 *  WinFLogString ( f, ... ) -- Redefines printf to TextOut() and file write, ignored fptr
 *
 *
 *  Modifications
 *  M000   22-Mar-89  waltcr
 *  - added #includes for stdio, and stdlib.  removed defines of cdecl and near.
 *  M001   06-Apr-89  waltcr
 *  - doscalls doesn't exist anymore. now use os2.h.  change DOSSLEEP to DosSleep
 *  M001   06-Apr-89  waltcr
 *  - define REGVARS macro to help find more optimization bugs.
 *  M002   20-May-89  waltcr
 *  - convert function declerations to new style function declerations.
 *  M003   06-Jun-89  waltcr
 *  - define INCL_NOPM so when os2.h is included, pm headers are not included.
 *  M004   27-Mar-90  mattheww
 *  - define far etc. to nothing if using c386 compiler
 *  M005   29-Mar-90  waltcr
 *  - merge in CXX test.h - change finish() to macro, define TEST_H.
 *  M006   02-Apr-90  mrw
 *  - remove MTHREAD if's and put them into seperate THREAD.H
 *  M007   10-Apr-90  chauv
 *  - changed most "int" variables to "unsigned long" variables.
 *  - changed those "%d" associated with "int" variables to "%lu" in printf().
 *  M008  12-Apr-90   mattheww
 *  - move c386 specific defines to a seperate include file c386.h (undo M004)
 *  - put printf back in check_func (mistakenly deleted before)
 *  M009  31-May-90  brucemc
 *  - added CALLCONV as macro for _cdecl vs _stdcall prototypes.
 *  M010  06-Jun-90  chauv
 *  - changed all "unsigned long" back to "int" and duplicate these
 *  - functions for unsigned long adding "32" to function name.
 *  M011  20-Jun-90  mattheww
 *  - fixed format spec for printing line number in faill_func()
 *  M012  28-Dec-90  alans
 *  - undefined fail() when __IOSTREAM_H is defined due to conflict with
 *  - ios::fail() member function.
 *  M013  16-May-91  tomca
 *  - c7/386 defines _M_I386 only (not M_I386)
 *  M014  01-Aug-91  bos
 *  - Modified printf statements for Failure output to confirm with
 *  - standard C error output.  This allows PWB and M to track a test's
 *  - runtime errors in the build results window.
 *  M015  01-Aug-91  xiangjun
 *  - remove "#include <callcon.h>" because of the usage change of "stdcall"
 *  - and "#define CALLCONV" to make the tests still valid.
 *  M016  23-Aug-91  georgech
 *  - remove _threadid duplicated in thread.h as far
 *  M017  23-Aug-91  bos
 *  - removed extraneous #endif after _threadid definition comment
 *  M018  10-Jan-92  alans
 *  - a couple of the 32-bit macros were missing the unsigned long cast
 *  M019  13-Jan-92  alans
 *  - the 32-bit functions protoed the __LINE__ as an unsigned long but
 *  - it is an int always.
 *  M020  15-Jan-92  alans
 *  - added checkNULL and checknNULL for checking pointers
 *  M021  16-Feb-92  bos
 *  - moved some declarations and macros outside the extern "C"
 *  M022  22-Jan-92  v-stevek
 *  - included <ntport.h> to support nt
 *  M023  04-Jun-92  patlo
 *  - if _WINDOWS is defined, redefined printf, main to run tests as
 *  - win-16 apps
 *  M024  06-Jul-92  patlo
 *  - Split into test.h and test.c
 *  M025  13-Jul-92  Patlo
 *  - Added errdefs to Win16 code
 *  M026  16-Jul-92  patlo
 *  - Merged test.c back into test.h, put all WinTest code in wintest.h,
 *  - added conditional compilation of wintest.h
 *  M027  12-Aug-92  judithla
 *  - added check for defines of WIN32S or WINNT to add proper includes
 *  M028  09-nov-92 tomca
 *  - include if NT_TEST, _M_I386 or _M_M68K is defined. no more c386.h
 *  M029  30-Dec-1992 mattheww
 *  - For NT, define has been changed to _M_IX86; check for this to determine whether
 *    or not to include 
 *  M030  18-Feb-1993 a-aarond
 *  - If TEST_NT is defined, do not include ntport.h or c32.h.
 *  M031  25-Mar-1993 a-jasonw
 *  - If _M_MRX000 is defined (when using Centaur) include c32.h.
 *  M032  12-Aug-1994 a-timke
 *  - Removed define of CALLCONV. This is no longer needed but caused
 *  - compiler errors in oleauto.h.
 *  M033  23-Jan-1994 a-timke
 *  - __IOSTREAM_H header flag was changed to __INC_IOS (in ios.h)
 *  M034  17-Apr-1995 a-timke
 *  - Support for TEST_WIN32S switch.
 *  M035  03-Apr-1995 mmalone
 *  - Added code to block the Debug CRT report message box from openning.
 *    writes to stdout instead.
 *  M035  07-July-1995 mmalone
 *  - Added check for QA_NO_DBGCRT_HOOK, if defined will not install a report
 *    hook.  Added 2 global variables:
 *       reportHookCalled  : set to 1 if the report hook has been called.
 *       dbgCrtCont        : set to 1 if you want to return from report hook
 *                           instead of exiting.
 *
 * M036   24-Oct-1995 mmalone
 *  - added include for skiptest.h (added) so failing tests can be turned
 *    on/off and kept track of more easily.
 * M037   29-Jan-1998 mmalone
 *  - added _WIN64 (i64) versions of checkNULL and checknNULL.  
 * M038   29-Jan-1998 mmalone
 *  - added 64 bit version of check_func().
 * M039	Nov-30-98 anitag
 *	- added _Win64 to include c32.h to tests for P7
 *
 */


#include <stdio.h>                                                   /* M000 */
#include <stdlib.h>
/* M009 */
/* M015
#include <callcon.h>
*/


/*M032# CALLCONV*/


/* M005*/
#ifndef TEST_H
#define TEST_H

/* M026 */
/* M034 */
/* not needed for xbox
#ifdef _WINDOWS
# include <wintest.h>
#elif defined(WIN32S) || defined(TEST_WIN32S) || defined(WINNT)
# define _WINDOWS
# include <wintest.h>
#endif
*/

#include <xtl.h>
int DbgPrint(const char *, ...);
#define QA_NO_DBGCRT_HOOK

#define T(x) x
#define V(x) x
#define PDV(x) x
#define starthread()  perfect = 0

/* M035 */
#if defined(_DEBUG) && !defined(QA_NO_DBGCRT_HOOK)
#include <crtdbg.h>

static int oldRetVal = 0;
static int reportHookCalled = 0; /* 1 if report hook called */
static int dbgCrtCont = 0;       /* 0 == exit, 1 = continue program. */

static void logDbgCrtExit(void)
{
  exit(99);
}

extern char test[];
static int logDbgCrtReport(int nRptType, char *szMsg, int* retval)
{
  oldRetVal = 0;
  reportHookCalled = 1;
  if (dbgCrtCont)
    return 1;
  DbgPrint("\nDebug CRT Report: %s", szMsg);
  DbgPrint("%s: ----- FAILED -----\n", test);
  logDbgCrtExit();
  return 0;    /* never gets called, but compiler complains if not here.*/
}

#define startest()  _CrtSetReportHook((_CRT_REPORT_HOOK)logDbgCrtReport); perfect = 0
#else
#define startest()  perfect = 0
#endif





/* This macro is used to confirm its argument is zero.
*/
#ifdef _WIN64
#define check( a ) check_func64( (__int64)(a), __LINE__ )
#else
#define check( a ) check_func( (int)(a), __LINE__ )
#endif 

/* This macro is used to confirm its arguments are equal.
*/
#ifdef _WIN64
#define checke( a, b ) checke_func64( (__int64)(a), (__int64)(b), __LINE__ )
#else
#define checke( a, b ) checke_func( (int)(a), (int)(b), __LINE__ )
#endif 

#define checke32( a, b ) checke_func32( (unsigned long)(a), (unsigned long)(b), __LINE__ )    /* M010 */

/* This macro is used to confirm its arguments are NOT equal.
*/
#ifdef _WIN64
#define checkne( a, b ) checkne_func64( (__int64)(a), (__int64)(b), __LINE__ )
#else
#define checkne( a, b ) checkne_func( (int)(a), (int)(b), __LINE__ )
#endif 
/* M018 */
/* M019 */
#define checkne32( a, b ) checkne_func32( (unsigned long)(a), (unsigned long)(b), __LINE__ )  /* M010 */


/* This macro is used to report failures of tests that are explicitely
 * numbered.
*/
#if !defined(QA_NO_FAIL_MACRO)
#define fail( n ) fail_func( (n), __LINE__ )
#endif
/* M018 */
#define fail32( n ) fail_func32( ((unsigned long)n), __LINE__ )    /* M010 */


/* This macro is used to report failures of tests that are NOT explicitely
 * numbered.
*/
#define faill( ) faill_func( __LINE__ )
#define faill32( ) faill_func32( __LINE__ )         /* M010 */

/* M037 */
#if !defined(_WIN64)

/* M020 */
/* This macro is used to confirm that its argument is a NULL pointer.
*/
#define checkNULL( a ) if ( sizeof( a ) == 4 ) \
               check_func32( (unsigned long)(a), __LINE__ ); \
               else \
               check_func( (unsigned int)(a),  __LINE__ )

/* M020 */
/* This macro is used to confirm that its argument is NOT a NULL pointer.
*/
#define checknNULL( a ) if ( sizeof( a ) == 4 ) \
                check_func32( (unsigned long)!(a), __LINE__ ); \
            else \
                check_func( (unsigned int)!(a), __LINE__ )

#else 

#define checkNULL( a ) check_func64( (unsigned __int64)(a), __LINE__ )
#define checknNULL( a ) check_func64( (unsigned __int64)!(a), __LINE__ )

#endif 

/* Report test results
**
*/

#define finish() \
    if( !V(perfect) ) DbgPrint("%s: ***** PASSED *****\n",test); \
    else DbgPrint("%s: ----- FAILED -----\n",test); \
    return (int)( V(perfect) ? 1 : 0 )\


/* M005*/
#if defined( __cplusplus )
extern "C" {
#endif

/* M022 */
#if defined(TEST_NT)

/* M030 */
/*
#include <ntport.h>
*/

#undef M_I386
#undef _M_I386

#endif


extern char Result[ 64 ] ;
extern unsigned Synchronize ;
extern int  T(perfect);


/* M008 */
/* M028 */
/* M029 */
/* M030 */
/* M031 */
/* #if defined(M_IX86) || defined(_M_IX86) || defined(_M_M68K) || defined(_M_MRX000) || defined(_M_MPPC) || defined(_M_PPC) || defined(_WIN64) */
#ifndef TEST_NT
// not needed for xbox
// #include <c32.h>
#endif
/* #endif */

#define REGVARS   register int _r1 = 1;  register int _r2 = 2        /* M002 */

/* extern int * _threadid ;     M016 */

/* the use of "far pascal" was conflicting with use of -Za switch, so */
/* replaced prototype of DOSSLEEP with following three lines, 7-6-88  */

static void  check_func(int  a,int  l);
static void  checke_func(int  a,int  b,int  l);
static void  checkne_func(int  a,int  b,int  l);
static void  fail_func(int  n,int  l);
static void  faill_func(int  l);


/* M010 */
static void  check_func32(unsigned long  a,int  l);
static void  checke_func32(unsigned long  a,unsigned long  b,int  l);
static void  checkne_func32(unsigned long  a,unsigned long  b,int  l);
static void  fail_func32(unsigned long  n,int  l);
static void  faill_func32(int  l);

#if defined(_WIN64)
static void  check_func64(unsigned __int64  a, int  l);
static void  checke_func64(unsigned __int64  a, unsigned __int64 b,int  l);
static void  checkne_func64(unsigned __int64  a, unsigned __int64 b,int  l);
#endif 

/* M012 */
/* M033 */
#if defined(__IOSTREAM_H) || defined(_INC_IOSTREAM) || defined(_INC_IOS) || defined(_IOSTREAM_)
 #undef fail
#endif

/**************************************************************/
/*****  SPLIT HERE IF YOU WANT TEST.H AND TEST.C SEPARATE *****/
/**************************************************************/

extern char test[];
static void check_func( int a, int l )
{
    if( !a ) return;
    DbgPrint("%s(%d): Failure: --- %d != 0\n",test, l, a );
    V(perfect) = 1;
}

/* M010 */
/* M019 */
static void check_func32( unsigned long a, int l )
{
    if( !a ) return;
    DbgPrint("%s(%d): Failure: --- %lu != 0\n",test, l, a );
    V(perfect) = 1;
}

/* M038 */
#if defined(_WIN64)
static void check_func64( unsigned __int64 a, int l )
{
    if( !a ) return;
    DbgPrint("%s(%d): Failure: --- %I64u != 0\n",test, l, a );
    V(perfect) = 1;
}

static void checke_func64(unsigned __int64 a, unsigned __int64 b, int l )
{
    if( a == b ) return;
    DbgPrint("%s(%d): Failure: --- %I64u != %I64u\n",test, l, a, b );
    V(perfect) = 1;
}

static void checkne_func64( unsigned __int64 a, unsigned __int64 b, int l )
{
    if( a != b ) return;
    DbgPrint("%s(%d): Failure: --- %I64u == %I64u\n",test, l, a, b );
    V(perfect) = 1;
}
#endif 


static void checke_func(int a, int b, int l )
{
    if( a == b ) return;
    DbgPrint("%s(%d): Failure: --- %d != %d\n",test, l, a, b );
    V(perfect) = 1;
}

/* M010 */
/* M019 */
static void checke_func32(unsigned long a, unsigned long b, int l )
{
    if( a == b ) return;
    DbgPrint("%s(%d): Failure: --- %lu != %lu\n",test, l, a, b );
    V(perfect) = 1;
}

static void checkne_func( int a, int b, int l )
{
    if( a != b ) return;
    DbgPrint("%s(%d): Failure: --- %d == %d\n",test, l, a, b );
    V(perfect) = 1;
}

/* M010 */
/* M019 */
static void checkne_func32( unsigned long a, unsigned long b, int l )
{
    if( a != b ) return;
    DbgPrint("%s(%d): Failure: --- %lu == %lu\n",test, l, a, b );
    V(perfect) = 1;
}



static void fail_func( int n, int l )
{
    DbgPrint("%s(%d): Failure: --- test %d\n",test, l, n );
    V(perfect) = 1;
}

/* M010 */
/* M019 */
static void fail_func32( unsigned long n, int l )
{
    DbgPrint("%s(%d): Failure: --- test %lu\n",test, l, n );
    V(perfect) = 1;
}



static void faill_func( int l )
{
    DbgPrint("%s(%d): Failure:\n",test, l);           /* M011 */
    V(perfect) = 1;
}

/* M010 */
/* M019 */
static void faill_func32( int l )
{
    DbgPrint("%s(%d): Failure:\n",test, l);
    V(perfect) = 1;
}


#if defined( __cplusplus )
};
#endif

/*M036*/
// xbox
// #include "skiptest.h"

#ifdef __cplusplus
#if !defined(QA_DISABLE_USING_NAMESPACE)
namespace std {}
using namespace std;
#endif
#endif

// xbox - redefine any lingering printf's
#pragma message("pointing printf to DbgPrint")
#define printf DbgPrint

// not supported in xbox, so undefine this
#define _putenv(x) 

#endif /* #ifndef TEST_H */
