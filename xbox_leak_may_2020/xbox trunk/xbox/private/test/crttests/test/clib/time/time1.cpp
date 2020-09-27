#define QaLib time1NameSpace

#include <time.h>
#include "qalibrary.h"

/*@ time

Test Cases:
------------
  1. just check that reported time is within reasonable range.
  2. same as #1 but pass NULL for argument.

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

template <class TT, class TF> bool TC1(TF timeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    bool        retVal = true;

    TT          time1 = 0;
    TT          time2 = timeFunc(&time2);

    retVal &= QATCheck(time2 > YMIN);
    retVal &= QATCheck(time2 < YMAX);
    retVal &= QATCheckNE(time2, time1);

    return retVal;
}

template <class TT, class TF> bool TC2(TF timeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    bool        retVal = true;

    TT          time1 = 0;
    TT          time2 = timeFunc(0);

    retVal &= QATCheck(time2 > YMIN);
    retVal &= QATCheck(time2 < YMAX);
    retVal &= QATCheckNE(time2, time1);

    return retVal;
}

static unsigned __stdcall time1Time1(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(1)

        QATTrace1(_T("mktime()"));
        QATCheck(TC1<time_t>(time, QA_THREAD_PARAM_NAME));
		
#if ( _MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
        QATCheck(TC1<__time64_t>(_time64, QA_THREAD_PARAM_NAME));
#endif /* (_MSC_VER > 1200) */

    QA_END_TC(1)

    QA_BEGIN_TC(2)

        QATTrace1(_T("mktime()"));
        QATCheck(TC2<time_t>(time, QA_THREAD_PARAM_NAME));
		
#if ( _MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
        QATCheck(TC2<__time64_t>(_time64, QA_THREAD_PARAM_NAME));
#endif /* (_MSC_VER > 1200) */

    QA_END_TC(2)

    return 0;
}

TestFunc time1TestFuncList[] =
{
    time1Time1
};


int time1Entry()
{
    int nTests = (sizeof(time1TestFuncList)/sizeof(time1TestFuncList[0]));
    int iTest = 0;
    bool bRunTerm = false;

    if (time1TestFuncList[0] == 0)
    {
        if (nTests == 1) return 3; /* error, not enough data */
        /* if the 0th element is zero, then 1st element is a */
        /* thread kickoff function */
        iTest = 2;
        if (time1TestFuncList[1] != 0)
            time1TestFuncList[1](0);
    }

    if (nTests >= 4)
    {
        if (time1TestFuncList[2] == 0)
        {
            bRunTerm = true;
            iTest = 4;
        }
    }

    for (; iTest < nTests; iTest++)
        if(time1TestFuncList[iTest] != 0)
            time1TestFuncList[iTest](0);
    if (bRunTerm)
    {
        if (!time1TestFuncList[3])
            DbgPrint("Term function NULL!\n");
        else
            time1TestFuncList[3](0);
    }
    int ret = GetTestFailure() ? 1 : 0;
    ReportTest(false);
    ClearFailures();
    return ret;
}

