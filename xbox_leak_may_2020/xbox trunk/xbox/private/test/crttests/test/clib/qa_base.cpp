#if !defined(__QALIB_BASE_CPP__)
#define __QALIB_BASE_CPP__

#if !defined(QA_USE_QALIB_DLL)

#include <float.h>
#include <limits.h>
#ifndef _XTL_
#include <xtl.h>
#endif

#include "qalib.h"

namespace QaLib 
{

//
// variables/objects.
// 
static const int      g_MaxTestIdSize = 256*sizeof(char);  // max chars in test id.

static bool           g_bTestFailed = 0;          // flag to indicate test failed.
static bool           g_bTCFailed = 0;            // flag to indicate testcase failed.
static char          g_tzTestId[g_MaxTestIdSize+1];  // buffer for test id.

static double         g_DDelta = (double)0.0;
static long double    g_LDDelta = (long double)0.0;
static float          g_FDelta = (float)0.0;
//!: may be used later if output is to be sent to a file.
//static _TCHAR*        g_fileName = _QT("CON");     //!: using this will not 
                                                    //   allow redirects.

//
// Reporting results.
//

////////////////////////////////////////
// ReportTest - Send text to log object indicating results (pass or fail) of test.
//              Reset test state.
//
void
ReportTest(bool bClearFail)
{
#ifdef QA_SIMPLE_QALIB
    if (g_bTestFailed)
	    DbgPrint(_QT("\n** %d Failed **\n"), g_tzTestId);
    else
	    DbgPrint(_QT("** %d Passed **\n"), g_tzTestId);
#else
    if (g_bTestFailed)
        QALog << _QT("\n** ") << g_tzTestId << _QT(" Failed **\n") << QAEndl;
    else
        QALog << _QT("** ") << g_tzTestId << _QT(" Passed **\n") << QAEndl;
#endif
    if (bClearFail)
        ClearFailures();
}

////////////////////////////////////////
// ReportTC - Print text to stdout if testcase failed.  Reset testcase state.
//
void
ReportTC(unsigned p_tc, bool bClearFail)
{
    if (g_bTCFailed)
    {
        SetTestFailure();
#ifdef QA_SIMPLE_QALIB
	DbgPrint(_QT("\n\t** TC %d Failed **\n"), p_tc);
#else
	QALog << _QT("\n") << INDENT1 << _QT("** TC ") << p_tc << _QT(" Failed **\n") << QAEndl;
#endif
    }
    if (bClearFail)
        SetTCFailure(false);
}


//
// Math functions.
//

////////////////////////////////////////
// Pow - Calc x^y.
//
// Returns: x to the power of y.
// 
double
Pow(
    double p_d,     // > operand.
    unsigned p_u)   // > power.
{
    double p = 1.0;
    
    while (p_u-- > 0)
        p *= p_d;
    return p;
}


//
// Compare functions, used with CheckE() and CheckNE().
//

////////////////////////////////////////
// Compare - Compare 2 doubles.  compare is not exact, but includes a delta.
//
// Returns: true if equal, false otherwise.
//
bool
Compare(
    double p_val1,  // > src double.
    double p_val2)  // > double to compare to.
{
    if (g_DDelta == 0.0) 
        SetDDelta(0.5 / Pow(10.0, DBL_DIG - 1));

    if (p_val1 == p_val2)
        return true;

    if ((p_val1 + g_DDelta) > p_val2 && 
        (p_val1 - g_DDelta) < p_val2)
        return true;
    return false;
}

////////////////////////////////////////
// Compare - Compare 2 floats.  compare is not exact, but includes a delta.
//
// Returns: true if equal, false otherwise.
//
bool
Compare(
    float p_val1,   // > src float.          
    float p_val2)   // > float to compare to.
{
    
    if (g_FDelta == 0.0) 
        SetFDelta((float) (0.5 / Pow(10.0, FLT_DIG - 1)));

    if (p_val1 == p_val2)
        return true;

    if ((p_val1 + g_FDelta) > p_val2 && 
        (p_val1 - g_FDelta) < p_val2)
        return true;
    return false;
}

////////////////////////////////////////
// Compare - Compare 2 doubles.  compare is not exact, but includes a delta.
//
// Returns: true if equal, false otherwise.
//
bool
Compare(
    long double p_val1,   // > src long double.          
    long double p_val2)   // > long double to compare to.
{
    if (g_LDDelta == 0.0L) 
        SetLDDelta(0.5L / Pow(10.0, DBL_DIG - 1));

    if (p_val1 == p_val2)
        return true;
    if ((p_val1 + g_LDDelta) > p_val2 && 
        (p_val1 - g_LDDelta) < p_val2)
        return true;
    return false;
}

//
// Test validation functions.
//

////////////////////////////////////////
// Check
//
bool
Check(bool b, int line, const char* p_tzFile, const char* p_tzExp, unsigned tid)
{
    if (b)
        return true;
#ifdef QA_SIMPLE_QALIB
    QA_ERROR_TEXT(line, p_tzFile, tid);
    DbgPrint(p_tzExp);
    DbgPrint(_QT("\n"));
#else
    QALog << QA_ERROR_TEXT(line, p_tzFile, tid) << p_tzExp 
          << (*p_tzExp != 0 ? _QT(".") : p_tzExp) << QAEndl;
#endif
    SetTCFailure();
    return false;
}

////////////////////////////////////////
// Fail - Prints error message and sets error flags.
//
void
Fail(int line, const char* p_tzFile, const char* p_tzMsg, unsigned tid)
{
#ifdef QA_SIMPLE_QALIB
    QA_ERROR_TEXT(line, p_tzFile, tid);
    DbgPrint(p_tzMsg);
    DbgPrint(_QT("\n"));
#else
    QALog << QA_ERROR_TEXT(line, p_tzFile, tid) << p_tzMsg
          << (*p_tzMsg != 0 ? _QT(".") : p_tzMsg) << QAEndl;
#endif
    SetTCFailure();
}

void
Fail(int line, const char* p_tzFile, int e, unsigned tid)
{
    char  buf[10];
    _itoa(e, buf, 10); //_itot(e, buf, 10);
    Fail(line, p_tzFile, buf, tid);
}

void
Trace(int line, const char* p_tzFile, const char* p_tzMsg, unsigned tid)
{
#ifdef QA_SIMPLE_QALIB
    QA_ERROR_TEXT(line, p_tzFile, tid);
    DbgPrint(p_tzMsg);
    DbgPrint(_QT("\n"));
#else
    QALog << QA_TRACE_TEXT(line, p_tzFile, tid) << p_tzMsg 
          << (*p_tzMsg != 0 ? _QT(".") : p_tzMsg) << QAEndl;
#endif
}

//
// Initialization and termination.
//

////////////////////////////////////////
// Init - stores test id and prints test header.  Also sets a debug report
// hook. 
//

// iainb:06/05/2000 We were using QALog here to print out the debug hook reports
// this is not good if the debug report comes AFTER the globals have been destructed,
// as it will if it's a CRT memory leak report. The problem is with the global critical
// section used in the object QALog creates' constructor (i.e. via QA_LOCK).
static int __cdecl DbgReportHook(int nRptType, char *szMsg, int *retVal)
{
#pragma message("xbox -- STD_ERROR_HANDLE undefined: " __FILE__)
  /*
  HANDLE hStdErr = ::GetStdHandle(STD_ERROR_HANDLE);
  DWORD nBytesWritten;
  ::WriteFile(hStdErr, szMsg, (DWORD)strlen(szMsg), &nBytesWritten, NULL);
  */
  *retVal = 1;
  return 0;
}

static int __cdecl DbgReportHookTerm(int nRptType, char *szMsg, int *retVal)
{
#if !defined(QA_SHOW_MEM_LEAKS)

#pragma message("xbox -- STD_ERROR_HANDLE undefined: " __FILE__)
  /*
  HANDLE hStdErr = ::GetStdHandle(STD_ERROR_HANDLE);
  DWORD nBytesWritten;
  ::WriteFile(hStdErr, szMsg, (DWORD)strlen(szMsg), &nBytesWritten, NULL);
  */
  *retVal = 1;
  return 0;
#else
  return 0;
#endif 
}

void
Init(const char* p_testid, bool bBanner)
{
    /* SetErrorMode(2); */
    QA_DISPLAY_MEM_LEAKS_STDOUT();
#ifdef _DEBUG
    _CrtSetReportHook(DbgReportHook);
#endif
    QA_AUTO_LOCK2;
    strcpy(g_tzTestId, p_testid);
    ClearFailures();
    if (bBanner)
#ifdef QA_SIMPLE_QALIB
	DbgPrint(_QT("Test Id: %d\n\n"), g_tzTestId);
#else
	    QALog << _QT("Test Id: ") << g_tzTestId << _QT("\n") << QAEndl;
#endif
}

////////////////////////////////////////
// Terminate - optionally print test footer.
//
// Returns: 1 if test failed, 0 otherwise.
//
int
Terminate(bool bReportTest)   // > indicates whether test footer should be printed.
{
    int ret = GetTestFailure() ? 1 : 0;
    if (bReportTest)
        ReportTest(false);
    ClearFailures();
#ifdef _DEBUG
    _CrtSetReportHook(DbgReportHookTerm);
#endif
    return ret;
}

//
// Accsessor functions.
//

void SetTestFailure(bool b) 
{
    QA_AUTO_LOCK2;
    g_bTestFailed = b;
}

bool GetTestFailure() 
{
    return g_bTestFailed;
}

void SetTCFailure(bool b) 
{
    QA_AUTO_LOCK2;
    g_bTCFailed = b;
    if (b)
        g_bTestFailed = b;
}

bool GetTCFailure() 
{
    return g_bTCFailed;
}

void ClearFailures()
{
    SetTestFailure(false);
    SetTCFailure(false);
}

//
// Set/Get delta value for float/double compare funtions.
//

void SetDDelta(double d) 
{
    QA_AUTO_LOCK2;
    g_DDelta = d;
}

double GetDDelta() 
{
    return g_DDelta;
}

void SetLDDelta(long double d)
{
    QA_AUTO_LOCK2;
    g_LDDelta = d;
}

long double GetLDDelta() 
{
    return g_LDDelta;
}

void SetFDelta(float d) 
{
    QA_AUTO_LOCK2;
    g_FDelta = d;
}

float GetFDelta() 
{
    return g_FDelta;
}

//
// Accessor functions.
//
int GetMaxTestIdSize()
{
    return  g_MaxTestIdSize;
}

char* GetTestId()
{
    return g_tzTestId;
}

void SetTestId(const char* szId)
{
    QA_AUTO_LOCK2;
    strcpy(g_tzTestId, szId);
}

} // QaLib

#endif // !QA_USE_QALIB_DLL
#endif // __QALIB_BASE_CPP__
