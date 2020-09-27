#define QaLib ftime1NameSpace

#include <time.h>
#include <sys\timeb.h>

#undef QA_USE_THREADS   // due to use of putenv() threads can't be used.
#include "qalibrary.h"

/*@ ftime

Test Cases:
------------
  1-3. set timezone and call function.  make simple checks for correctness.
  
@*/

#define Y1993 725328000L  /* approx. number of seconds between 1970 & 1993 */
#define Y1994 756864000L  /* approx. number of seconds between 1970 & 1994 */
#define Y1995 788400000L  /* approx. number of seconds between 1970 & 1995 */
#define Y1997 851472000L  /* approx. number of seconds between 1970 & 1997 */
#define Y1999 914544000L
#define Y2036 2081376000L
#define Y2037 2145904712L   /* actual number of seconds between 1970 & 12/31/2037 - 12:35 p.m. */

#define YMIN  Y1994       /* Range (in seconds) for time tests */
#define YMAX  Y2037

_TCHAR *envstr[] = 
{  
    _T("TZ=PST8PDT"),   /* Pacific Standard Time */
    _T("TZ=MST7MDT"),   /* Mountain Standard Time */
    _T("TZ=EST5EDT")    /* Eastern Standard Time */
};

int zones[] = { 480, 420, 300 };

template <class TT, class TF> bool TC1(TF timeFunc, QA_THREAD_PARAM)
{
    bool    retVal = true;
    TT      times;

    if (!QATCheck(_tputenv(envstr[0]) == 0))
        return false;

    _tzset();
    timeFunc(&times);
    retVal &= QATCheck(times.time > YMIN);
    retVal &= QATCheck(times.time < YMAX);
    retVal &= QATCheck(times.millitm < 999);
    retVal &= QATCheckE(times.timezone, zones[0]);          

    return retVal;
}

template <class TT, class TF> bool TC2(TF timeFunc, QA_THREAD_PARAM)
{
    bool    retVal = true;
    TT      times;

    if (!QATCheck(_tputenv(envstr[1]) == 0))
        return false;

    _tzset();
    timeFunc(&times);
    retVal &= QATCheck(times.time > YMIN);
    retVal &= QATCheck(times.time < YMAX);
    retVal &= QATCheck(times.millitm < 999);
    retVal &= QATCheckE(times.timezone, zones[1]);          

    return retVal;
}

template <class TT, class TF> bool TC3(TF timeFunc, QA_THREAD_PARAM)
{
    bool    retVal = true;
    TT      times;

    if (!QATCheck(_tputenv(envstr[2]) == 0))
        return false;

    _tzset();
    timeFunc(&times);
    retVal &= QATCheck(times.time > YMIN);
    retVal &= QATCheck(times.time < YMAX);
    retVal &= QATCheck(times.millitm < 999);
    retVal &= QATCheckE(times.timezone, zones[2]);          

    return retVal;
}


static unsigned __stdcall ftime1FTime1(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(1)

        QATTrace1(_T("ftime()"));
        QATCheck(TC1<struct _timeb>(_ftime, QA_THREAD_PARAM_NAME));

#if ( _MSC_VER > 1200)
        QATTrace1(_T("ftime64()"));
        QATCheck(TC1<struct __timeb64>(_ftime64, QA_THREAD_PARAM_NAME));
#endif /* (_MSC_VER > 1200) */

    QA_END_TC(1)
    return 0;
}

static unsigned __stdcall ftime1FTime2(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(2)

        QATTrace1(_T("ftime()"));
        QATCheck(TC2<struct _timeb>(_ftime, QA_THREAD_PARAM_NAME));

#if ( _MSC_VER > 1200)        
		QATTrace1(_T("ftime64()"));
        QATCheck(TC2<struct __timeb64>(_ftime64, QA_THREAD_PARAM_NAME));
#endif /* (_MSC_VER > 1200) */

    QA_END_TC(2)
    return 0;
}

static unsigned __stdcall ftime1FTime3(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(3)

        QATTrace1(_T("ftime()"));
        QATCheck(TC3<struct _timeb>(_ftime, QA_THREAD_PARAM_NAME));
		
#if ( _MSC_VER > 1200)
        QATTrace1(_T("ftime64()"));
        QATCheck(TC3<struct __timeb64>(_ftime64, QA_THREAD_PARAM_NAME));
#endif /* (_MSC_VER > 1200) */

    QA_END_TC(3)
    return 0;
}

TestFunc ftime1TestFuncList[] =
{
    ftime1FTime1,
    ftime1FTime2,
    ftime1FTime3
};


int ftime1Entry()
{
    int nTests = (sizeof(ftime1TestFuncList)/sizeof(ftime1TestFuncList[0]));
    int iTest = 0;
    bool bRunTerm = false;

    if (ftime1TestFuncList[0] == 0)
    {
        if (nTests == 1) return 3; /* error, not enough data */
        /* if the 0th element is zero, then 1st element is a */
        /* thread kickoff function */
        iTest = 2;
        if (ftime1TestFuncList[1] != 0)
            ftime1TestFuncList[1](0);
    }

    if (nTests >= 4)
    {
        if (ftime1TestFuncList[2] == 0)
        {
            bRunTerm = true;
            iTest = 4;
        }
    }

    for (; iTest < nTests; iTest++)
        if(ftime1TestFuncList[iTest] != 0)
            ftime1TestFuncList[iTest](0);
    if (bRunTerm)
    {
        if (!ftime1TestFuncList[3])
            DbgPrint("Term function NULL!\n");
        else
            ftime1TestFuncList[3](0);
    }
    int ret = GetTestFailure() ? 1 : 0;
    ReportTest(false);
    ClearFailures();
    return ret;
}

