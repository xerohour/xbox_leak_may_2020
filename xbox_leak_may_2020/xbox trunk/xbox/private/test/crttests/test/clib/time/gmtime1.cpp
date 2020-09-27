#define QaLib gmtime1NameSpace

#include <string.h>
#include <time.h>

#include "qalibrary.h"

/*@ gmtime

Test Cases:
------------
  1. 'timer' == -1.  Should FAIL.
  2. 'timer' == 0.  Should PASS - 1/1/1970.
  
@*/

// Helper test functions.  These are template functions to test both 32bit
// and 64bit.
//
template <class TT, class TF> bool TC1(TF timeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    bool        retVal = true;
    struct tm*  ptm;
    TT          timer;

    timer = -1;
    ptm = timeFunc(&timer);
    QATCheck(ptm == 0);

    return retVal;
}

template <class TT, class TF> bool TC2(TF timeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    bool        retVal = true;
    struct tm*  ptm;
    TT          timer;

    timer = 0;
    ptm = timeFunc(&timer);
    if (QATCheck(ptm != 0))
    {
        retVal &= QATCheckE(ptm->tm_mon, 0);
        retVal &= QATCheckE(ptm->tm_mday, 1);
        retVal &= QATCheckE(ptm->tm_year, 70);
    }
    else
        retVal = false;

    return retVal;
}

static unsigned __stdcall gmtime1GMTime1(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(1)

        QATCheck(TC1<time_t>(gmtime, QA_THREAD_PARAM_NAME));

#if (_MSC_VER > 1200)	
		QATCheck(TC1<__time64_t>(_gmtime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */


    QA_END_TC(1)
    return 0;
}

static unsigned __stdcall gmtime1GMTime2(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(2)

        QATCheck(TC2<time_t>(gmtime, QA_THREAD_PARAM_NAME));

#if (_MSC_VER > 1200)
		QATCheck(TC2<__time64_t>(_gmtime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(2)
    return 0;
}

TestFunc gmtime1TestFuncList[] =
{
    gmtime1GMTime1,
    gmtime1GMTime2
};


int gmtime1Entry()
{
    int nTests = (sizeof(gmtime1TestFuncList)/sizeof(gmtime1TestFuncList[0]));
    int iTest = 0;
    bool bRunTerm = false;

    if (gmtime1TestFuncList[0] == 0)
    {
        if (nTests == 1) return 3; /* error, not enough data */
        /* if the 0th element is zero, then 1st element is a */
        /* thread kickoff function */
        iTest = 2;
        if (gmtime1TestFuncList[1] != 0)
            gmtime1TestFuncList[1](0);
    }

    if (nTests >= 4)
    {
        if (gmtime1TestFuncList[2] == 0)
        {
            bRunTerm = true;
            iTest = 4;
        }
    }

    for (; iTest < nTests; iTest++)
        if(gmtime1TestFuncList[iTest] != 0)
            gmtime1TestFuncList[iTest](0);
    if (bRunTerm)
    {
        if (!gmtime1TestFuncList[3])
            DbgPrint("Term function NULL!\n");
        else
            gmtime1TestFuncList[3](0);
    }
    int ret = GetTestFailure() ? 1 : 0;
    ReportTest(false);
    ClearFailures();
    return ret;
}

