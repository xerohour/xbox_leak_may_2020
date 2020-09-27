#if !defined(__QALIB_MACRO_H__)
#define __QALIB_MACRO_H__

#include "qa_nowarns.h"

//
// macros
//

// set unicode macros
//
#if defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#endif

// if building QALIB dll then turn everything on.
//
#if defined(QA_BUILD_QALIB_DLL)
#define QA_USE_THREADS
#endif 

#ifndef QA_NO_STD_PREFIX
#undef STD
#define STD std::
#else
#undef STD
#define STD
namespace std {}
using namespace std;
#endif 

// may want to do typedefs for those not in the lib.
// check out iosfwd header.
//!: basic_*fstream, basic_ios, locale

#if defined(_UNICODE) || defined(UNICODE)
#define _tios           STD wios
#define _tstreambuf     STD wstreambuf
#define _tistream       STD wistream
#define _tostream       STD wostream
#define _tiostream      STD wiostream
#define _tstringbuf     STD wstringbuf
#define _tistringstream STD wistringstream
#define _tostringstream STD wostringstream
#define _tstringstream  STD wstringstream
#define _tfilebuf       STD wfilebuf
#define _tifstream      STD wifstream
#define _tofstream      STD wofstream
#define _tfstream       STD wfstream
#define _tstring        STD wstring
#define _tcout          STD wcout
#define _tcerr          STD wcerr
#define _tcin           STD wcin
#define _tclog          STD wclog
#else
#define _tios           STD ios
#define _tstreambuf     STD streambuf
#define _tistream       STD istream
#define _tostream       STD ostream
#define _tiostream      STD iostream
#define _tstringbuf     STD stringbuf
#define _tistringstream STD istringstream
#define _tostringstream STD ostringstream
#define _tstringstream  STD stringstream
#define _tfilebuf       STD filebuf
#define _tifstream      STD ifstream
#define _tofstream      STD ofstream
#define _tfstream       STD fstream
#define _tstring        STD string
#define _tcout          STD cout
#define _tcerr          STD cerr
#define _tcin           STD cin
#define _tclog          STD clog
#endif 

#define _QT

#define INDENT1 _QT("  ")      // first level of indentation, used for printing.
#define INDENT2 _QT("    ")    // second level of indentation, used for printing.

#if defined(QA_BUILD_QALIB_DLL)
#define QADllExport __declspec( dllexport )
#define QADllImport QADllExport                 //!__declspec( dllimport )
#elif defined(QA_USE_QALIB_DLL)
#define QADllExport __declspec( dllexport )
#define QADllImport __declspec( dllimport )
#else
#define QADllImport
#define QADllExport
#endif

// user versions of the above, for tests that are built as DLLs.
//
#if defined(QA_BUILD_DLL)
#define QAUDllExport __declspec( dllexport )
#define QAUDllImport QAUDllExport
#elif defined(QA_USE_DLL)
#define QAUDllExport __declspec( dllexport )
#define QAUDllImport __declspec( dllimport )
#else
#define QAUDllImport
#define QAUDllExport
#endif

// second set of the above
//
#if defined(QA_BUILD_DLL2)
#define QAUDllExport2 __declspec( dllexport )
#define QAUDllImport2 QAUDllExport2
#elif defined(QA_USE_DLL2)
#define QAUDllExport2 __declspec( dllexport )
#define QAUDllImport2 __declspec( dllimport )
#else
#define QAUDllImport2
#define QAUDllExport2
#endif


#define QA_DECL __stdcall           // used for exported global functions.

// set OS convienence macros.
//
#if defined(QA_OS_PMAC) || defined(QA_OS_68K) || defined(_MAC)
#define QA_OS_MAC
#endif

#if defined(QA_OS_WIN32S) || defined(QA_OS_WIN95) || defined(QA_OS_NT)
#define QA_WIN32
#endif

#if defined(QA_OS_WIN16) || defined(QA_WIN32)
#define QA_WIN
#endif

// set other macros.
//
#if defined(_MT)
#define QA_MT_LIBS
#endif

#if defined(_DEBUG)
#define QA_DEBUG
#endif

// Log macros.
//
// If building the QLIB DLL use STDIO log.
#if (!defined(QA_USE_IOS_LOG) && !defined(QA_USE_STDIO_LOG)) || defined(QA_BUILD_QALIB_DLL)
#undef QA_USE_IOS_LOG
#undef QA_USE_STDIO_LOG
#define QA_USE_STDIO_LOG
#endif 

#define QAEndl CEndl()

#if defined(QA_USE_IOS_LOG)
#define QALog CIOStreamLog()
#define QA_LOG(x)   CIOStreamLog x
#endif 

#if defined(QA_USE_STDIO_LOG)
#define QALog CStdioLog()
#define QA_LOG(x)   CStdioLog x
#endif 

// Validation macros. 
//
#define QA_LF  __LINE__,_QT(__FILE__)

#define QACheck(x)      QaLib::Check((x), QA_LF, QA_STR(x))
#define QACheckE(x,y)   QaLib::CheckE((x),(y), QA_LF)
#define QACheckNE(x,y)  QaLib::CheckNE((x),(y), QA_LF)
#define QAFail(x)       QaLib::Fail(QA_LF, x)
#define QAFail0()       QaLib::Fail(QA_LF)

#if defined(QA_TEST_DEBUG)

#ifdef QA_SIMPLE_QALIB
#define QATrace(x)      QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0)
#define QATrace0()      QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0)
#define QATrace1(x)     QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0 
#define QATrace2(a,b)   QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0)
#define QATrace3(a,b,c) QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0)
#define QATrace4(a,b,c,d)   QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0)
#define QATrace5(a,b,c,d,e) QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0)
#define QATrace6(a,b,c,d,e,f)   QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0)
#define QATrace7(a,b,c,d,e,f,g) QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0)
#define QATrace8(a,b,c,d,e,f,g,h)   QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0)
#else
#define QATrace(x)      QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) << (x) << QAEndl
#define QATrace0()      QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) << QAEndl
#define QATrace1(x)     QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) \
<< (x) << QAEndl

#define QATrace2(a,b)   QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) \
<< (a) << (b) << QAEndl

#define QATrace3(a,b,c) QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) \
<< (a) << (b) << (c) << QAEndl

#define QATrace4(a,b,c,d)   QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) \
<< (a) << (b) << (c) << (d) << QAEndl

#define QATrace5(a,b,c,d,e) QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) \
<< (a) << (b) << (c) << (d) << (e) << QAEndl

#define QATrace6(a,b,c,d,e,f)   QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) \
<< (a) << (b) << (c) << (d) << (e) << (f) << QAEndl

#define QATrace7(a,b,c,d,e,f,g) QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) \
<< (a) << (b) << (c) << (d) << (e) << (f) << (g) << QAEndl

#define QATrace8(a,b,c,d,e,f,g,h)   QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) \
<< (a) << (b) << (c) << (d) << (e) << (f) << (g) << (h) << QAEndl
#endif

#else

#define QATrace(x)
#define QATrace0()
#define QATrace1(x)
#define QATrace2(a,b)
#define QATrace3(a,b,c)
#define QATrace4(a,b,c,d)
#define QATrace5(a,b,c,d,e)
#define QATrace6(a,b,c,d,e,f)
#define QATrace7(a,b,c,d,e,f,g)
#define QATrace8(a,b,c,d,e,f,g,h)
#endif 

// thread specific.
//
#define QATCheck(x)      QaLib::Check((x), QA_LF, QA_STR(x), QA_TID)
#define QATCheckE(x,y)   QaLib::CheckE((x),(y), QA_LF, QA_TID)
#define QATCheckNE(x,y)  QaLib::CheckNE((x),(y), QA_LF, QA_TID)
#define QATFail(x)       QaLib::Fail(QA_LF, x, QA_TID)
#define QATFail2()       QaLib::Fail(QA_LF, _QT(""), QA_TID)

#if defined(QA_TEST_DEBUG)

#define QATTrace(x)     QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),QA_TID) << (x) << QAEndl
#define QATTrace0()     QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),0) << QAEndl
#define QATTrace1(x)    QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),QA_TID) \
<< (x) << QAEndl

#define QATTrace2(a,b)  QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),QA_TID) \
<< (a) << (b) << QAEndl

#define QATTrace3(a,b,c) QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),QA_TID) \
<< (a) << (b) << (c) << QAEndl

#define QATTrace4(a,b,c,d)  QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),QA_TID) \
<< (a) << (b) << (c) << (d) << QAEndl

#define QATTrace5(a,b,c,d,e)    QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),QA_TID) \
<< (a) << (b) << (c) << (d) << (e) << QAEndl

#define QATTrace6(a,b,c,d,e,f)  QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),QA_TID) \
<< (a) << (b) << (c) << (d) << (e) << (f) << QAEndl

#define QATTrace7(a,b,c,d,e,f,g)    QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),QA_TID) \
<< (a) << (b) << (c) << (d) << (e) << (f) << (g) << QAEndl

#define QATTrace8(a,b,c,d,e,f,g,h)  QALog << QA_TRACE_TEXT(__LINE__, _QT(__FILE__),QA_TID) \
<< (a) << (b) << (c) << (d) << (e) << (f) << (g) << (h) << QAEndl

#else

#define QATTrace(x)
#define QATTrace0()
#define QATTrace1(x)
#define QATTrace2(a,b)
#define QATTrace3(a,b,c)
#define QATTrace4(a,b,c,d)
#define QATTrace5(a,b,c,d,e)
#define QATTrace6(a,b,c,d,e,f)
#define QATTrace7(a,b,c,d,e,f,g)
#define QATTrace8(a,b,c,d,e,f,g,h)
#endif 


// Helper macros.
//
#define QA_STR(x) (_QT(#x))
#define QA_EVAL_DESC(x) STR(x)
#define QA_EVAL(x) x

// Testcase macros.
//

#if defined(QA_ENABLE_TC_EH)
#define QA_BEGIN_TC_EH try {
#define QA_END_TC_EH(x) } catch(...) { QaLib::Fail(QA_LF, _QT("TC Exception"), x); }
#else
#define QA_BEGIN_TC_EH
#define QA_END_TC_EH(x)
#endif 

#define QA_BEGIN_TC2(x)  { QaLib::CAutoTC auto_tc(100 * x + QA_TID); \
{ QA_MEM_CHECK_OBJECT; { QATTrace2(_QT("Testcase: "), (x)); QA_BEGIN_TC_EH

#define QA_END_TC2(x)    QA_END_TC_EH(x) }}}

// Doesn't use memcheck object.
//
#ifdef QA_SIMPLE_QALIB
#define QA_BEGIN_TC(x)  { { QATTrace2(_QT("Testcase: "), x); QA_BEGIN_TC_EH
#else
#define QA_BEGIN_TC(x)  { QaLib::CAutoTC auto_tc(100 * x + QA_TID); \
{ QATTrace2(_QT("Testcase: "), x); QA_BEGIN_TC_EH
#endif // QA_SIMPLE_QALIB

#define QA_END_TC(x)    QA_END_TC_EH(x) }}

// Convenance macro for Init and Terminate.
//
#define QA_INIT QaLib::Init(_QT(__FILE__))
#define QA_TERM return QaLib::Terminate()

//
///////////////////////////////////////////////////////////////////////////////
// thread support.
///////////////////////////////////////////////////////////////////////////////
//
#define QA_THREAD_PARAM_NAME    qaThreadParam
#define QA_THREAD_PARAM         void* QA_THREAD_PARAM_NAME = 0

#define QA_THREAD_DECL(x) QAUDllExport unsigned __stdcall x (QA_THREAD_PARAM)

// thread function decl.
//

#if (defined(QA_MT_MIX) || defined(QA_MT_SEQ)) && !defined(QA_USE_THREADS)
#define QA_USE_THREADS
#endif

#if defined(QA_USE_THREADS) && (!defined(QA_MT_MIX) && !defined(QA_MT_SEQ))
#define QA_MT_MIX
#endif 

#if defined(QA_USE_THREADS)
#define QA_TID (QA_THREAD_PARAM_NAME == 0 ? 0 : (*(unsigned*)QA_THREAD_PARAM_NAME))
#else
#define QA_TID 0
#endif 

//
//QA_MT_SEQ   // (sequential) fire off all threads for a test before going on.
//QA_MT_MIX   // (mixed) fire off a thread for each test before going on.
//
// timeout interval. 10 min. for default.
//
#if !defined(QA_MT_TIMEOUT)
#define QA_MT_TIMEOUT   600000L
#endif

// for number of threads.  5 is the default.
//
#if defined(QA_USE_THREADS) && !defined(QA_NTHREADS)
#define QA_NTHREADS 5
#endif

// boilerplate code for thread execution.
//
#if defined(QA_MT_MIX) 
#define QA_DEF_RUNOBJ_TYPE QaLib::CRunMTMix
#elif defined(QA_MT_SEQ)
#define QA_DEF_RUNOBJ_TYPE QaLib::CRunMTSeq
#else
#define QA_DEF_RUNOBJ_TYPE QaLib::CRunNoMT
#endif

#define QA_DEF_TESTMGR(x,y) QaLib::CTestMgr<x> testMgr(y);

#define QA_RUN_CODE(rot,testl,s) \
rot runObj(testl, (s)); \
QA_DEF_TESTMGR(rot, runObj)

//
// Default main implementation macros.
//
#if !defined(QA_MT_LIBS) && (defined(QA_MT_SEQ) || defined(QA_MT_MIX))
#define QA_DEF_RUN \
    QALog << _QT("Running with QA_USE_THREADS but without _MT. Skipping") << QAEndl; exit(2); 
#define QA_DEF_RUN3(rot,x) \
    QALog << _QT("Running with QA_USE_THREADS but without _MT. Skipping") << QAEndl; exit(2); 
#else
#define QA_DEF_RUN \
    QA_RUN_CODE(QA_DEF_RUNOBJ_TYPE, TestFuncList, \
                sizeof(TestFuncList)/sizeof(TestFuncList[0])) \
    return testMgr.Run(_QT(__FILE__));
#define QA_DEF_RUN3(rot,x) \
    QA_RUN_CODE(rot,TestFuncList, \
                sizeof(TestFuncList)/sizeof(TestFuncList[0])) \
    return testMgr.Run(_QT(__FILE__), x);
#endif

#if !defined (QA_USE_WINMAIN)
#define QA_MAIN \
    int  _tmain (int argc, char* argv[])

#else

#define QA_MAIN \
    WINAPI _tWinMain( \
          HINSTANCE hInstance, \
          HINSTANCE hPrevInstance, \
          LPTSTR lpCmdLine, \
          int nShowCmd \
          )

#endif

// use these macros if not using QA_DEF_RUN, that is, you need to do custom 
// test run code.
//
#define QA_BEGIN_TEST_MAIN QA_MAIN { QA_INIT;
#define QA_END_TEST_MAIN QA_TERM; }

#define QA_DEF_TESTLIST static QaLib::TestFunc TestFuncList[] =

#ifdef QA_SIMPLE_QALIB

#define QA_DEF_TEST_MAIN QA_MAIN {\
	int nTests = (sizeof(TestFuncList)/sizeof(TestFuncList[0]));\
	int iTest = 0;\
	bool bRunTerm = false;\
\
	if (TestFuncList[0] == 0){\
		if (nTests == 1) return 3; \
		iTest = 2;\
		if (TestFuncList[1] != 0)\
			TestFuncList[1](0);\
	};\
	if (nTests >= 4)\
	{\
		if (TestFuncList[2] == 0){\
			bRunTerm = true;\
			iTest = 4;\
		};\
	};\
	for (; iTest < nTests; iTest++)\
		if(TestFuncList[iTest] != 0)\
			TestFuncList[iTest](0);\
	if (bRunTerm) {\
		if (!TestFuncList[3]) {\
			_tprintf(_QT("Term function NULL!\n"));\
		} else {\
			TestFuncList[3](0);\
		}\
	}\
	int ret = GetTestFailure() ? 1 : 0;\
	ReportTest(false);\
	ClearFailures();\
	return ret;\
}

// locale versions of above.
//
#define QA_DEF_RUN2 QA_DEF_RUN3(QA_DEF_RUNOBJ_TYPE,QA_DEF_LC_TABLE_NAME)
#define QA_DEF_TEST_MAIN2 QA_MAIN { QA_DEF_RUN3(QA_DEF_RUNOBJ_TYPE,QA_DEF_LC_TABLE_NAME) }
#define QA_DEF_TEST_MAIN3(x) QA_MAIN { QA_DEF_RUN3(QA_DEF_RUNOBJ_TYPE,x) }

#else
#define QA_DEF_TEST_MAIN QA_MAIN { /* ::SetErrorMode(QA_SETERRORMODE); */ QA_DEF_RUN }

// locale versions of above.
//
#define QA_DEF_RUN2 QA_DEF_RUN3(QA_DEF_RUNOBJ_TYPE,QA_DEF_LC_TABLE_NAME)
#define QA_DEF_TEST_MAIN2 QA_MAIN { /* ::SetErrorMode(QA_SETERRORMODE); */ QA_DEF_RUN3(QA_DEF_RUNOBJ_TYPE,QA_DEF_LC_TABLE_NAME) }
#define QA_DEF_TEST_MAIN3(x) QA_MAIN { /* ::SetErrorMode(QA_SETERRORMODE); */ QA_DEF_RUN3(QA_DEF_RUNOBJ_TYPE,x) }
#endif // QA_SIMPLE_QALIB

//
// Misc. macros.
//
// macro for string literals when passed to Check* or Compare functions.
//
#define QA_SL(x)  ((const char*) _QT(x))   // cast string literals to char*.
#define _SL(x)  QA_SL(x)
#define _STR(x) (const char*)(x)          // cast string variables to char*.

#if defined(_MT)
#define QA_TLS __declspec(thread)           // for TLS variables (global).
#else
#define QA_TLS
#endif

//
// Memcheck macros
//
#if defined(_DEBUG)
#include <crtdbg.h>

#if !defined(QA_DBG_OUTPUT_DEBUGWIN)

#define  QA_SEND_TO_STDOUT() \
     { _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE ); \
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT ); \
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE ); \
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT ); \
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE ); \
    _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT ); };
#else
#define  QA_SEND_TO_STDOUT()
#endif

#define  SET_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))

#define QA_DISPLAY_MEM_LEAKS_STDOUT() \
    QA_SEND_TO_STDOUT(); \
    SET_CRT_DEBUG_FIELD(_CRTDBG_LEAK_CHECK_DF);

#define QA_MEM_CHECK_OBJECT QaLib::CMemCheck      memCheck(__LINE__)
#define QA_MEM_CHECK_OBJECT_GLOBAL QaLib::CMemCheck g_memCheck(__LINE__)

#else

#define QA_MEM_CHECK_OBJECT
#define QA_MEM_CHECK_OBJECT_GLOBAL

#define SET_CRT_DEBUG_FIELD(a)
#define CLEAR_CRT_DEBUG_FIELD(a)

#define QA_SEND_TO_STDOUT()

#define QA_DISPLAY_MEM_LEAKS_STDOUT()

#endif

// By default the using namespace statement is used.
//
#if !defined(QA_NO_USING_QALIB_NS)
namespace QaLib {}
using namespace QaLib;
#endif

// other useful macros.
//
#define QA_TEST_SERVER_SHARE _QT("\\\\vcuser\\exchange")
#define QA_TEST_SERVER_DIR _QT("crttest")

// length of array.
//
#define QALengthOf(x) (sizeof(x)/sizeof((x)[0]))

#endif // __QALIB_MACRO_H__
