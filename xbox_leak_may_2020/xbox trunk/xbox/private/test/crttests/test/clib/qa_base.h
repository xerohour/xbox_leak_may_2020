#if !defined(__QALIB_BASE_H__)
#define __QALIB_BASE_H__

#include <string.h>
#include <tchar.h> 
#include <math.h>
#include <stdlib.h>

#if defined(_DEBUG)
# include <crtdbg.h>
#endif //defined(_DEBUG)

#include "qa_macro.h"

#if defined(_DEBUG)
// windows must be included AFTER qa_macro.h for UNICODE runs!
#include <xtl.h>
#endif //defined(_DEBUG)

#include "qa_macro.h"
#if defined(QA_USE_IOS_LOG) || defined(QA_BUILD_QALIB_DLL)
#include <iostream>
#endif
#if defined(QA_USE_STDIO_LOG) || defined(QA_BUILD_QALIB_DLL)
#include <stdio.h>
#endif 

#include "qa_thread.h"

#if defined(QA_USE_STRINGTABLE)
#include "qa_str_table.h"
#endif

// declare Windows API SetErrorMode to turn off GPF/AV dialog.
//
#if !defined(_WINDOWS_)
extern "C" __declspec(dllimport) unsigned __stdcall SetErrorMode(unsigned uMode);

#ifndef QA_SETERRORMODE
// these values are the same as the #defines used in the #else branch for Win2K
#define QA_SETERRORMODE (0x0001 | 0x8000 | 0x0002)
#endif // QA_SETERRORMODE

#else // defined(_WINDOWS_)

#ifndef QA_SETERRORMODE
#define QA_SETERRORMODE (SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX | SEM_NOGPFAULTERRORBOX)
#endif // QA_SETERRORMODE

#endif 

extern "C" { int __cdecl DbgPrint(const char*, ...); }

namespace QaLib 
{

//
///////////////////////////////////////////////////////////////////////////
// Global Typedefs.
///////////////////////////////////////////////////////////////////////////
//

typedef QAUDllImport unsigned (__stdcall* TestFunc)(void*);

//
///////////////////////////////////////////////////////////////////////////
// Classes.
///////////////////////////////////////////////////////////////////////////
//

//!: build all flavors for qalib dll, need to include the correct version of GetLog
// in user code.
// macros (QALog, QAEndl) should not be defined when building DLL.

struct QADllImport CEndl {}; // basically tells the log class to print a new line and
                             // flush buffers.

// used to print diagnostic text and default information like line # and file.
class QADllImport CDiagText
{
public:
    CDiagText(const char* tzText, int line, const char* tzFile, unsigned tid)
    : m_tzText(tzText), m_line(line), m_tzFile(tzFile), m_tid(tid) {}

    const char*     m_tzText;
    int             m_line;
    const char*     m_tzFile;
    unsigned        m_tid;
};

//
// Log classes.  There are currently (2/17/97) 2 classes.  One sends output
// to stdout via the iostreams (std::cout) and the other class uses printf.
// The interface is similar to ostream except manipulators can't be used.
//
#if defined(QA_USE_IOS_LOG) || defined(QA_BUILD_QALIB_DLL)

class QADllImport CIOStreamLog
{
public:
    // lock and unlock
    CIOStreamLog() 
    {
        QA_LOCK1;
        m_os = &_tcout;
    }

    ~CIOStreamLog()
    {
        QA_UNLOCK1;
    }
#ifdef _WIN64					//it is the testharness utility anyway. LX: 11101999	
    CIOStreamLog& operator << (size_t _X) { 	//This fix is ad hoc, but so is the design
	 DbgPrint(_QT("%I64i"), _X); 
	return *this; 
	}
#endif

    //!mmalone: no int64 support in iostream yet so use stdio.
    CIOStreamLog& operator << (__int64 _X) { DbgPrint(_QT("%I64i"), _X); return *this; }
    CIOStreamLog& operator << (bool _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (short _X) { (*m_os) << _X; return *this; }
#if !defined(_UNICODE)
    CIOStreamLog& operator << (unsigned short _X) { (*m_os) << _X; return *this; }
#endif 
    CIOStreamLog& operator << (int _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (unsigned int _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (long _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (unsigned long _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (float _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (double _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (long double _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (const void* _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (char* _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (const char* _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (char _X) { (*m_os) << _X; return *this; }
    CIOStreamLog& operator << (const CEndl& _X) { (*m_os) << std::endl; return *this; }
    CIOStreamLog& operator << (const CDiagText& x)
    { 
        (*m_os) << x.m_tzText << x.m_tzFile << _QT(" (") << x.m_line << _QT("). ");
        if (x.m_tid)
            (*m_os) << _QT("tid(") << x.m_tid << _QT("). ");
        return *this;
    }
#if defined(_XSTRING_) || defined(QA_DEF_QALOG_STRING_OP)
    CIOStreamLog& operator << (const _tstring& x)
    { (*m_os) << x.c_str(); return *this; }
#endif

protected:
    std::basic_ostream<char>* m_os;
};

#endif

#if defined (QA_USE_STDIO_LOG) || defined(QA_BUILD_QALIB_DLL)

class QADllImport CStdioLog
{
public:
    // lock and unlock
    CStdioLog()
    {
        QA_LOCK1;
    }

    ~CStdioLog()
    {
        QA_UNLOCK1;
    }

#ifdef _WIN64					//it is the testharness utility anyway. LX: 11101999	
    CStdioLog& operator << (size_t _X) { 	//This fix is ad hoc, but so is the design
	 DbgPrint(_QT("%I64i"), _X); 
	return *this; 
	}
#endif

    CStdioLog& operator << (__int64 _X) { DbgPrint(_QT("%I64i"), _X); return *this; }
    CStdioLog& operator << (bool _X) { DbgPrint(_QT("%d"), _X ? 1 : 0); return *this; }
    CStdioLog& operator << (short _X) { DbgPrint(_QT("%hd"), _X); return *this; }
#if !defined(_UNICODE)
    CStdioLog& operator << (unsigned short _X) { DbgPrint(_QT("%hu"), _X); return *this; }
#endif 
    CStdioLog& operator << (int _X) { DbgPrint(_QT("%d"), _X); return *this; }
    CStdioLog& operator << (unsigned int _X) { DbgPrint(_QT("%u"), _X); return *this; }
    CStdioLog& operator << (long _X) { DbgPrint(_QT("%ld"), _X); return *this; }
    CStdioLog& operator << (unsigned long _X) { DbgPrint(_QT("%lu"), _X); return *this; }
    CStdioLog& operator << (float _X) { DbgPrint(_QT("%g"), (double)_X); return *this; }
    CStdioLog& operator << (double _X) { DbgPrint(_QT("%g"), _X); return *this; }
    CStdioLog& operator << (long double _X) { DbgPrint(_QT("%Lg"), _X); return *this; }
    CStdioLog& operator << (const void* _X) { DbgPrint(_QT("%p"), _X); return *this; }
    CStdioLog& operator << (char* _X) { DbgPrint(_QT("%s"), _X); return *this; }
    CStdioLog& operator << (const char* _X) { DbgPrint(_QT("%s"), _X); return *this; }
    CStdioLog& operator << (char _X) { DbgPrint(_QT("%c"), _X); return *this; }
    CStdioLog& operator << (const CEndl& _X) { DbgPrint(_QT("\n")); _flushall(); return *this; }
    CStdioLog& operator << (const CDiagText& x)
    { 
        DbgPrint(_QT("%s: %s (%d). "), x.m_tzText, x.m_tzFile, x.m_line); 
        if (x.m_tid)
            DbgPrint(_QT("tid(%d). "), x.m_tid);
        return *this; 
    }
#if defined(_XSTRING_) || defined(QA_DEF_QALOG_STRING_OP)
    CStdioLog& operator << (const _tstring& x)
    { DbgPrint(_QT("%s"), x.c_str()); return *this; }
#endif
};

#endif // QA_USE_IOS_LOG else QA_USE_STDIO_LOG

//
///////////////////////////////////////////////////////////////////////////
// Global Functions
///////////////////////////////////////////////////////////////////////////
//

//
// Accessor functions.
//

QADllImport int     GetMaxTestIdSize();
QADllImport char* GetTestId();
QADllImport void    SetTestId(const char* szId);


//
// Reporting results.
//

QADllImport void    ReportTest(bool bClearFail = true);
QADllImport void    ReportTC(unsigned p_tc, bool bClearFail = true);

//
// Double math functions, used with real versions Compare().
//

////////////////////////////////////////
// DAbs - calc. the absolute value of given double.
//
inline double DAbs(double p_d) {      // > double to take abs of.
    return (p_d >= 0.0) ? p_d : -p_d;
}

QADllImport double  Pow(double p_d, unsigned p_u);

//
// Compare functions, used with CheckE() and CheckNE().
//

////////////////////////////////////////
// Compare - template version, use == operator.
//

#if !defined(QA_NO_TMPL_CMP)
template <class T1, class T2> inline bool Compare(
    const T1& p_x,       // > first object to compare.
    const T2& p_y)       // > object to compare with.
{
    return p_x == p_y;
}
#endif

////////////////////////////////////////
// Compare - _TCHAR* version.
//

////////////////////////////////////////
// Compare - const _TCHAR* version.
//

inline bool Compare(
    const char* p_x,  // > src string.          
    const char* p_y)  // > string to compare to.
{
    if (p_x == NULL && p_y == NULL)
        return 1;
    if (p_x == NULL || p_y == NULL)
        return 0;
    return strcmp(p_x,p_y) == 0;
}

inline bool Compare(char* p_x,char* p_y) 
    { return Compare(_STR(p_x), _STR(p_y)); }

inline bool Compare(const char* p_x,char* p_y) 
    { return Compare(_STR(p_x), _STR(p_y)); }

inline bool Compare(char* p_x, const char* p_y) 
    { return Compare(_STR(p_x), _STR(p_y)); }


QADllImport bool     Compare(double p_val1, double p_val2);
QADllImport bool     Compare(float p_val1, float p_val2);
QADllImport bool     Compare(long double p_val1, long double p_val2);


//
// Test validation functions.
//

#ifdef QA_SIMPLE_QALIB
#define QA_ERROR_TEXT(l,f,id) DbgPrint(_QT("Error@line %d, file %s, id %d"), l, f, id);
#define QA_TRACE_TEXT(l,f,id) DbgPrint(_QT("Trace@line %d, file %s, id %d"), l, f, id);
#else
#define QA_ERROR_TEXT(l,f,id) CDiagText(_QT("Error"), l, f, id)
#define QA_TRACE_TEXT(l,f,id) CDiagText(_QT("Trace"), l, f, id)
#endif

QADllImport bool     Check(bool b, int line, const char* p_tzFile, 
                           const char* p_tzExp = _QT(""), 
                           unsigned tid = 0);

////////////////////////////////////////
// CheckE - compares 2 given values.  If not equal then print error message
//          and set error flags.
//
// Returns: true if equal, false otherwise.
//
template <class T1, class T2> bool
CheckE(const T1& p_x, const T2& p_y, int line, const char* p_tzFile, unsigned tid = 0)
{
    if (Compare(p_x,p_y))
        return true;
#ifdef QA_SIMPLE_QALIB   
    DbgPrint(_QT("CheckE failed line %d file: "), line);
    _putts(p_tzFile);
    DbgPrint(_QT("tid %d file: "), tid);
#else
    QALog << QA_ERROR_TEXT(line, p_tzFile, tid) << _QT("\"") << p_x 
          << _QT("\" != \"") << p_y << _QT("\". ") << QAEndl;
#endif// QA_SIMPLE_QALIB

    SetTCFailure();
    return false;
}

////////////////////////////////////////
// CheckNE - compares 2 given values.  If equal then print error message
//          and set error flags.
//
// Returns: 1 if not equal, 0 otherwise.
//
template <class T1, class T2> bool
CheckNE(const T1& p_x, const T2& p_y, int line, const char* p_tzFile, unsigned tid = 0)
{
    if (!Compare(p_x,p_y))
        return true;
    
#ifdef QA_SIMPLE_QALIB   
    DbgPrint(_QT("CheckE failed line %d file: "), line);
    _putts(p_tzFile);
    DbgPrint(_QT("tid %d file: "), tid);
#else
    QALog << QA_ERROR_TEXT(line, p_tzFile, tid) << _QT("\"") << p_x << _QT("\" == \"") 
          << p_y << _QT("\". ") << QAEndl;
#endif //QA_SIMPLE_QALIB
    SetTCFailure();
    return false;
}

QADllImport void    Fail(int line, const char* p_tzFile, const char* p_tzMsg = _QT(""), 
                         unsigned tid = 0);
QADllImport void    Fail(int line, const char* p_tzFile, int e, unsigned tid = 0);
QADllImport void    Trace(int line, const char* p_tzFile, const char* p_tzMsg, 
                          unsigned tid = 0);
//
// Initialization and termination.
//

QADllImport void    Init(const char* testid, bool banner = true);
QADllImport int     Terminate(bool bReportTest = true);

//
// Get/Set of error flags.
//
QADllImport void    SetTestFailure(bool b = true);
QADllImport bool    GetTestFailure();
QADllImport void    SetTCFailure(bool b = true);
QADllImport bool    GetTCFailure();
QADllImport void    ClearFailures();

//
// Set/Get delta value for float/double compare funtions.
//
QADllImport void    SetDDelta(double d);
QADllImport double  GetDDelta();
QADllImport void    SetLDDelta(long double d);
QADllImport long double GetLDDelta();
QADllImport void    SetFDelta(float d);
QADllImport float   GetFDelta();


//
///////////////////////////////////////////////////////////////////////////
// Classes.
///////////////////////////////////////////////////////////////////////////
//

//
// The following classes (CRunBase, etc.) are used with CTestMgr below
// to create a test manager appropriate to the threading style required.
// NOTE:  The test list can optionally contain initialize and terminate 
// enteries.  These functions are called before and after the tests have been
// run.  They may be called more than once in the case where a locale lists 
// is used.
// The terminate function is optional.  If it is defined then the initalize 
// entry must be specified.
// If the initialize function returns anything other than 0 then no tests
// are run and the terminate function is not called.
//

// Base class.  Keeps track of list and # of test functions.
//
class QADllImport CRunBase
{
public:
    CRunBase(TestFunc* funcs, int size)
    : m_nFuncs(size), m_pFuncs(funcs) {}

protected:
    int         m_nFuncs;       // # of test funtions.
    TestFunc*   m_pFuncs;       // list of test function pointers.

    int     Init() 
    { 
        if (!m_pFuncs[0])
            return m_pFuncs[1](0) == 0 ? !m_pFuncs[2] ? 4 : 2 : -1;
        return 0; 
    }

    void    Term() 
    {
        if (!m_pFuncs[0] && !m_pFuncs[2])
            m_pFuncs[3](0);
    }
};

#if defined(QA_USE_THREADS)

// Multi-threaded support base class.  Contains support for managing the threads
// created by the manager.
//
class QADllImport CRunMTBase : public CRunBase
{
public:
    CRunMTBase(TestFunc* funcs, int nFuncs)
        : CRunBase(funcs, nFuncs)
    {
        m_nThreads = QA_NTHREADS * m_nFuncs;
        m_threads = new HANDLE[m_nThreads];
        m_ids = new unsigned[QA_NTHREADS];
    }
    ~CRunMTBase()
    {
        delete[] m_ids;
        delete[] m_threads;
    }
    void PostRun()
    {
        DWORD ret = ::WaitForMultipleObjects(m_nThreads, m_threads, TRUE, QA_MT_TIMEOUT);
        if (ret == WAIT_TIMEOUT)
            QAFail(_QT("waiting for threads to finish timed out!"));
        else if (ret == WAIT_FAILED)
            QAFail(_QT("waiting for threads failed!"));
        for (int x = 0; x < m_nThreads; x++)
            CloseHandle(m_threads[x]);
    }

protected:
    int         m_nThreads;     // # of threads started.
    HANDLE*     m_threads;      // list of handles to threads.
    unsigned*   m_ids;          // list of thread ids.
};

struct QATestFuncRecord {
	TestFunc m_func;
	int m_test_id;
};

//#define QA_TIMES_TO_RERUN_TEST_THREAD 5000
#ifndef QA_TIMES_TO_RERUN_TEST_THREAD
#define QA_TIMES_TO_RERUN_TEST_THREAD 1
#endif

#if QA_TIMES_TO_RERUN_TEST_THREAD  > 1
static unsigned int QATestRunner(void *pv)
{
	struct QATestFuncRecord *pRun = (struct QATestFuncRecord *)pv;
	for (int k = 0; k < QA_TIMES_TO_RERUN_TEST_THREAD; k++)
	{
		(*pRun->m_func)((void*)&pRun->m_test_id);
	};
	return 0;
}
#endif

// Multi-thread version that "mixes" the order of threads relative to the
// test funtion order.  
//
class QADllImport CRunMTMix : public CRunMTBase
{
 public:
  CRunMTMix(TestFunc* funcs, int nFuncs)
        : CRunMTBase(funcs, nFuncs) {}
  int Run()
    {
#ifdef _MT
        int s = Init();
        if (s == -1)
	  return 1;
#if QA_TIMES_TO_RERUN_TEST_THREAD  > 1
	struct QATestFuncRecord *pTestRecords;
	pTestRecords = new struct QATestFuncRecord[QA_NTHREADS*m_nFuncs];
#endif

	m_nThreads = 0;

        for (int i = 0; i < QA_NTHREADS; i++)
	  for (int j = s; j < m_nFuncs; j++)
            {
	      unsigned threadID;
	      m_ids[i] = i;
#if QA_TIMES_TO_RERUN_TEST_THREAD  > 1
	      pTestRecords[j*m_nFuncs + i].m_func = m_pFuncs[j];
	      pTestRecords[j*m_nFuncs + i].m_test_id = i;
	      m_threads[m_nThreads] = 
		(HANDLE)_beginthreadex(0, 0, (TestFunc)&QATestRunner, (void*)&pTestRecords[j*m_nFuncs + i], 0, &threadID);
#else
	      m_threads[m_nThreads] = 
		(HANDLE)_beginthreadex(0, 0, m_pFuncs[j], m_ids+i, 0, &threadID);
#endif
	      if (m_threads[m_nThreads] == 0)
		{
		  QAFail(_QT("Could not spawn thread\n"));
		}
	      else
		m_nThreads++;
            }
        PostRun();
        Term();
#if QA_TIMES_TO_RERUN_TEST_THREAD  > 1
	delete[] pTestRecords;
#endif
#else // !defined(_MT)

#ifdef QA_SIMPLE_QALIB   
    DbgPrint(_QT(" Trying to execute MT test code without _MT libs!\nWe should never have gotten here.\n"));
#else
      QALog << _QT("Trying to execute MT test code without _MT libs!\nWe should never have gotten here.\n");
#endif //QA_SIMPLE_QALIB

      exit(2);
#endif
      return 0;
    }
};

// Multi-thread version that, for each test funtion, starts all threads for
// that function before moving on.
//
class QADllImport CRunMTSeq : public CRunMTBase
{
public:
    CRunMTSeq(TestFunc* funcs, int nFuncs)
        : CRunMTBase(funcs, nFuncs) {}

    int Run()
    {
#ifdef _MT
        int s = Init();
        if (s == -1)
            return 1;
	m_nThreads = 0;

        for (int j = s; j < m_nFuncs; j++)
            for (int i = 0; i < QA_NTHREADS; i++)
            {
                m_ids[i] = i;
                m_threads[m_nThreads] = 
                    (HANDLE)_beginthreadex(0, 0, m_pFuncs[j], m_ids+i, 0, 0);

		if ( m_threads[m_nThreads] )
		  m_nThreads++;
            }
        PostRun();
        Term();
#else // !defined(_MT)
#ifdef QA_SIMPLE_QALIB   
    DbgPrint(_QT("Trying to execute MT test code without _MT libs!\nWe should never have gotten here.\n"));
#else
      QALog << _QT("Trying to execute MT test code without _MT libs!\nWe should never have gotten here.\n");
#endif //QA_SIMPLE_QALIB
      exit(2);
#endif
        return 0;
    }
};

#endif // QA_USE_THREADS

// Single-thread version, just calls each test function in sequence.
//
class QADllImport CRunNoMT : public CRunBase
{
public:
    CRunNoMT(TestFunc* funcs, int nFuncs)
        : CRunBase(funcs, nFuncs) {}

    int Run()
    {
        int s = Init();
        if (s == -1)
            return 1;
        for (int i = s; i < m_nFuncs; i++)
            m_pFuncs[i](0);
        Term();
        return 0;
    }
};

// Creates an instance of the given helper class and delegates responsibility
// of running the tests to it.
//
template <class T> class CTestMgr
{
public:
    CTestMgr(T& runObj) : m_runObj(runObj) {}

#if defined(QA_USE_STRINGTABLE)
    int Run(const char* szTestId = 0,
            LocaleIdT* localeIds = s_defLocaleIds) 
#else
    int Run(const char* szTestId = 0)
#endif 
    { 
        QaLib::Init(szTestId);
#if defined(QA_USE_STRINGTABLE)
        for (int i = 0; localeIds[i]; i++)
        {
            QATrace2(_QT("Locale: "), locales[i]);
            CAutoLocale autoLocale(localeIds[i]);
            if (QACheck(autoLocale.Ok()))
                m_runObj.Run();
            else
#ifdef QA_SIMPLE_QALIB
		    DbgPrint("  Locale Not set: %s\n", localeIds[i]);
#else
                QALog << _QT("  Locale NOT set: ") << localeIds[i] << QAEndl;
#endif // QA_SIMPLE_QALIB
	}
#else
        m_runObj.Run();
#endif
        QA_TERM;
    }

    T&                  m_runObj;
#if defined(QA_USE_STRINGTABLE)
    static LocaleIdT    s_defLocaleIds[];
#endif 
};

#if defined(QA_USE_STRINGTABLE)
template <class T> LocaleIdT CTestMgr<T>::s_defLocaleIds[] = 
{ 
    QA_LC_NoChange, 
    0
};
#endif 

//
// memory check object, used in QA_BEGIN_TC.
//
#if defined(_DEBUG)
class QADllImport CMemCheck
{
public:
    CMemCheck(int line)
    : m_line(line)
    {
        QA_DISPLAY_MEM_LEAKS_STDOUT();
        _CrtMemCheckpoint(&m_memState);
    }
    ~CMemCheck()
    {
        _CrtMemState  cur;
        _CrtMemState  result;
        _CrtMemCheckpoint(&cur);
        
        if (_CrtMemDifference(&result, &m_memState, &cur ))
        {
            char buf[80];
            sprintf(buf, _QT("Error: Memory Leak in block (or testcase) starting on line: %d."), 
                      m_line);
#ifdef QA_SIMPLE_QALIB
	    DbgPrint("%s\n", buf);
#else
	    QALog << buf << QAEndl;
#endif // QA_SIMPLE_QALIB
	    SetTCFailure();
            _CrtMemDumpAllObjectsSince(&m_memState);
        }
    }

private:
    int             m_line;
    _CrtMemState    m_memState;
};

#else

class QADllImport CMemCheck
{
public:
    CMemCheck(int line) {}
    ~CMemCheck() {}
};

#endif //_DEBUG

//
// Automatically setup and cleanup a testcase.
//
class QADllImport CAutoTC
{
public:
    CAutoTC(long tc) 
        : m_tc(tc) { QaLib::SetTCFailure(false);}
    ~CAutoTC()
        { QaLib::ReportTC(m_tc); }
    long    m_tc;
};

} // QaLib

#endif // __QALIB_BASE_H__
