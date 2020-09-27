#define QaLib localtime1NameSpace

#include <string.h>
#include <time.h>

#undef QA_USE_THREADS   // because of the putenv() calls this test can't 
                        // support threads.
#include "qalibrary.h"

/*@ localtime

Test Cases:
------------
  1. Timezone at 0 and timer at 0.  Should PASS.
  2. Timezone at 0 and timer at -1. Should Fail.
  3. Timezone at 8 and timer at 0.  Should PASS.
  4. Timezone at 8 and timer at -1. SHould FAIL.
  
@*/

// Helper test functions.  These are template functions to test both 32bit
// and 64bit.
//
template <class TT, class TF> bool TC1(TF tf, QA_THREAD_PARAM, TT tt = 0)
{
    TT          timer;
    struct tm*  ptm;
    bool        retVal = true;

    /* no putenv in xbox
    if (!QATCheck(_tputenv(_T("TZ=UCT+0")) == 0))
        return false;
    */

    _tzset();

    timer = 0;
    retVal &= QATCheck((ptm = tf(&timer)) != 0);
    retVal &= QATCheckE(_tcscmp(_tasctime(ptm), _T("Thu Jan 01 00:00:00 1970\n")), 0);
    return retVal;
}

template <class TT, class TF> bool TC2(TF tf, QA_THREAD_PARAM, TT tt = 0)
{
    TT          timer;
    struct tm*  ptm;
    bool        retVal = true;

    /* no putenv in xbox
    if (!QATCheck(_tputenv(_T("TZ=PDT+0")) == 0))
        return false;
    */

    _tzset();

    timer = -1;
    ptm = tf(&timer);
    QATCheck(ptm == 0);

    return retVal;
}

template <class TT, class TF> bool TC3(TF tf, QA_THREAD_PARAM, TT tt = 0)
{
    TT          timer;
    struct tm*  ptm;
    bool        retVal = true;

    /* no putenv in xbox
    if (!QATCheck((_tputenv(_T("TZ=PST+8")) == 0)))
        return false;
    */

    _tzset();

    timer = 0;
    QATCheck((ptm = tf(&timer)) != 0);
    //QATCheckE(_tcscmp(_tasctime(ptm), _T("Wed Dec 31 16:00:00 1969\n")), 0);
	//xbox can't set to a different timezone
    QATCheckE(_tcscmp(_tasctime(ptm), _T("Thu Jan 01 00:00:00 1970\n")), 0);

    return retVal;
}

template <class TT, class TF> bool TC4(TF tf, QA_THREAD_PARAM, TT tt = 0)
{
    TT          timer;
    struct tm*  ptm;
    bool        retVal = true;

    /* no putenv in xbox
    if (!QATCheck(_tputenv(_T("TZ=PST+8")) == 0))
        return false;
    */

    _tzset();

    timer = -1;
    QATCheck((ptm = tf(&timer)) == 0);

    return retVal;
}

static unsigned __stdcall localtime1LocalTime1(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(1)

        QATCheck(TC1<time_t>(localtime, QA_THREAD_PARAM_NAME));

#if (_MSC_VER > 1200)
        QATCheck(TC1<__time64_t>(_localtime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(1)
    return 0;
}

static unsigned __stdcall localtime1LocalTime2(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(2)

        QATCheck(TC2<time_t>(localtime, QA_THREAD_PARAM_NAME));

#if (_MSC_VER > 1200)
        QATCheck(TC2<__time64_t>(_localtime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(2)
    return 0;
}

static unsigned __stdcall localtime1LocalTime3(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(3)

        QATCheck(TC3<time_t>(localtime, QA_THREAD_PARAM_NAME));

#if (_MSC_VER > 1200)
        QATCheck(TC3<__time64_t>(_localtime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(3)
    return 0;
}

static unsigned __stdcall localtime1LocalTime4(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(4)

        QATCheck(TC4<time_t>(localtime, QA_THREAD_PARAM_NAME));

#if (_MSC_VER > 1200)
        QATCheck(TC4<__time64_t>(_localtime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(4)
    return 0;
}


TestFunc localtime1TestFuncList[] =
{
    localtime1LocalTime1,
    localtime1LocalTime2,
    localtime1LocalTime3,
    localtime1LocalTime4
};


int localtime1Entry()
{
    int nTests = (sizeof(localtime1TestFuncList)/sizeof(localtime1TestFuncList[0]));
    int iTest = 0;
    bool bRunTerm = false;

    if (localtime1TestFuncList[0] == 0)
    {
        if (nTests == 1) return 3; /* error, not enough data */
        /* if the 0th element is zero, then 1st element is a */
        /* thread kickoff function */
        iTest = 2;
        if (localtime1TestFuncList[1] != 0)
            localtime1TestFuncList[1](0);
    }

    if (nTests >= 4)
    {
        if (localtime1TestFuncList[2] == 0)
        {
            bRunTerm = true;
            iTest = 4;
        }
    }

    for (; iTest < nTests; iTest++)
        if(localtime1TestFuncList[iTest] != 0)
            localtime1TestFuncList[iTest](0);
    if (bRunTerm)
    {
        if (!localtime1TestFuncList[3])
            DbgPrint("Term function NULL!\n");
        else
            localtime1TestFuncList[3](0);
    }
    int ret = GetTestFailure() ? 1 : 0;
    ReportTest(false);
    ClearFailures();
    return ret;
}

