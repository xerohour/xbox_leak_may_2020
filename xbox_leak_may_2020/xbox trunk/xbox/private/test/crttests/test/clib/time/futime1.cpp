#define QaLib futime1NameSpace

#include <time.h>
#include <sys\stat.h>
#include <sys\timeb.h>
#include <sys\utime.h>
#include <fcntl.h>
#include <sys\types.h>
#include <io.h>
#include <errno.h>

// ALPHA64_UPDATE: 59
// ALPHA64_CHANGE: needed for the OS DST stuff
#include <windows.h>

#define QA_NTHREADS 1
#include "qalibrary.h"

/*@ futime

TestCases:
------------
  1. Opens file with different modes (binary and text).
     applies different times:
        o max
        o mid May 2074.
        o mid Jun 2001.
        o mid Sep 1972.
  2. Passes NULL as the second parameter, which should use the current time.
  
@*/

static const _TCHAR* testfile = _T("futime.tst");
static const _TCHAR* testfile2 = _T("futime2.tst");

/*
 *  Types of operation allowed for open file used for testing _futime().
 *  Note that the file must have write permision.  Also note that since
 *  _fstat() is used to verify _futime(), the file must have read access.
 *  So, ...
 */

int operations[] =
{
    _O_RDWR | _O_BINARY | _O_CREAT,
    _O_RDWR | _O_TRUNC | _O_APPEND | _O_TEXT | _O_CREAT
};

/*  The following unsigned long array holds the values used to test _utime().
 *  These values are in seconds from 00:00:00 Jan 1, 1970.
 *
 *  It is undocumented as of 2/12/93, but any time value greater than 1980
 *  is supposed to be valid.
 */

static unsigned long try_times[] =
{
    0xFFFFFFFEL,    /* valid    - maximum value */
    3294636800L,    /* valid    - approx. mid May 2074 */
     994967294L,    /* valid    - approx. mid Jun 2001 */
      88965439L     /* valid    - approx. mid Sep 1972 */
};

// ALPHA64_UPDATE: 60
// ALPHA64_CHANGE: helper functions for getting DST settings

// find out whether the "Adjust automatically to DST" is turned on
bool IsAdjustDSTEnabled (void) {
    TIME_ZONE_INFORMATION tzi;
    if(!GetTimeZoneInformation(&tzi)) {
        printf("could not find timezone information");
    }
    return (tzi.StandardBias != tzi.DaylightBias);
}

// find out whether we have DST right now
bool IsDSTNow(void) {
    _timeb tb;
    _ftime(&tb);
    return ((tb.dstflag != 0) ? true : false);
}

// is particular time DST
bool IsDST(time_t time) {
    tm* mytm;
    mytm = localtime(&time);
    return ((mytm->tm_isdst > 0) ? true : false);
}
// ALPHA64_UPDATE_END: 60

template <class T, class UTIMBUF, class STAT, class TF1, class TF2> 
bool TC1(TF1 futimeFunc, TF2 fstatFunc, QA_THREAD_PARAM)
{
    bool        retVal = true;
    
// ALPHA64_UPDATE: 81
// ALPHA64_CHANGE: DST functionality
    bool bAdjustDSTEnabled = IsAdjustDSTEnabled();
    bool bDSTNow = IsDSTNow();

    for (int n = 0; n < (sizeof(operations)/sizeof(operations[0])); n++)
    {
        UTIMBUF utimes;
        STAT    stimes;

        QATTrace2(_T("n == "), n);
        int     fhndl = _topen( testfile, operations[n] );
    
        if (QATCheck(fhndl != -1))
        {
            for(int i = 0; i < (sizeof(try_times)/sizeof(try_times[0])); i++)
            {
                QATTrace2(_T("i == "), i);
                utimes.modtime = try_times[i];
                utimes.actime = try_times[i];

                if(futimeFunc(fhndl, &utimes) == -1)
                {
                    QATCheckE(errno, EINVAL);
                }
                else
                {
                    if (QATCheck(fstatFunc(fhndl, &stimes ) == 0))
                    {

// ALPHA64_UPDATE: 82
// ALPHA64_CHANGE: adjust the time according OS settings, current time and test time
#ifdef _M_ALPHA64
                        // adjust time according to bAdjustDSTEnabled, bDSTNow and IsDST
                        // if !bAdjustDSTEnabled, we don't need to do anything
                        if (bAdjustDSTEnabled) {
                            if (bDSTNow && !IsDST(stimes.st_mtime)) stimes.st_mtime += 3600;
                            else if (!bDSTNow && IsDST(stimes.st_mtime)) stimes.st_mtime -= 3600;
                        }
#endif

                        QATCheckE(stimes.st_mtime, (T)try_times[i]);
                    }
                }
            }
            if (!QATCheck(_close( fhndl ) != -1))
                break;
        }
        _tsystem(_T("attrib -r futime.tst"));
        _tremove(testfile);
    }

    return retVal;
}

template <class T, class UTIMBUF, class TIMEB, class STAT, class TF1, class TF2, 
          class TF3> 
bool TC2(TF1 futimeFunc, TF2 fstatFunc, TF3 ftimeFunc, QA_THREAD_PARAM)
{
    bool    retVal = true;

    int     fhndl = _topen( testfile2, operations[0] );

    if (QATCheck(fhndl != -1))
    {
        TIMEB   times;
        STAT    stimes;

        /* check that when the 2nd arg is NULL, _futime sets the file's
         * modification and access time to the present time.
         */
    
        QATCheck(futimeFunc(fhndl, NULL) == 0);
        ftimeFunc(&times);
        if (QATCheck(fstatFunc( fhndl, &stimes ) == 0))
        {
            /* Note this test allows for 2 seconds of difference which may
             * not be enough if your machine is bogged down.  Two seconds is
             * a long time, but rounding off could contribute to this also.
             */
        
            QATCheck(stimes.st_mtime - times.time <= 2);
        }
    
        _close(fhndl);
        _tsystem(_T("attrib -r futime2.tst"));
        _tremove(testfile2);
    }

    return retVal;
}

static unsigned __stdcall futime1FUTime1(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(1)
    
#if (_MSC_VER > 1200) 
        QATTrace1(_T("*** futime64() ***"));
        if (!TC1<__int64,
                 struct __utimbuf64, 
                 struct __stat64>(_futime64, _fstat64, QA_THREAD_PARAM_NAME))
            QATFail(11);
// ALPHA64_UPDATE: 83
// ALPHA64_CHANGE: put the 32 bit code into the else clause
#else
        QATTrace1(_T("*** futime() ***"));
        if (!TC1<int,
                 struct _utimbuf, 
                 struct _stat>(_futime, _fstat, QA_THREAD_PARAM_NAME))
            QATFail(10);
#endif  /* (_MSC_VER > 1200) */
// ALPHA64_UPDATE_END: 83

    QA_END_TC(1)
    return 0;
}

static unsigned __stdcall futime1FUTime2(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(2)
    
#if (_MSC_VER > 1200) 
        QATTrace1(_T("*** futime64() ***"));
        if (!TC2<__int64,
                 struct __utimbuf64, 
                 struct __timeb64,
                 struct __stat64>(_futime64, _fstat64, _ftime64, QA_THREAD_PARAM_NAME))
            QATFail(13);
// ALPHA64_UPDATE: 84
// ALPHA64_CHANGE: put the 32 bit code into the else clause
#else
        QATTrace1(_T("*** futime() ***"));
        if (!TC2<int,
                 struct _utimbuf, 
                 struct _timeb,
                 struct _stat>(_futime, _fstat, _ftime, QA_THREAD_PARAM_NAME))
            QATFail(12);
#endif  /* (_MSC_VER > 1200) */
// ALPHA64_UPDATE_END: 84

    QA_END_TC(2)
    return 0;
}

TestFunc futime1TestFuncList[] =
{
    futime1FUTime1,
    futime1FUTime2
};


int futime1Entry()
{
    int nTests = (sizeof(futime1TestFuncList)/sizeof(futime1TestFuncList[0]));
    int iTest = 0;
    bool bRunTerm = false;

    if (futime1TestFuncList[0] == 0)
    {
        if (nTests == 1) return 3; /* error, not enough data */
        /* if the 0th element is zero, then 1st element is a */
        /* thread kickoff function */
        iTest = 2;
        if (futime1TestFuncList[1] != 0)
            futime1TestFuncList[1](0);
    }

    if (nTests >= 4)
    {
        if (futime1TestFuncList[2] == 0)
        {
            bRunTerm = true;
            iTest = 4;
        }
    }

    for (; iTest < nTests; iTest++)
        if(futime1TestFuncList[iTest] != 0)
            futime1TestFuncList[iTest](0);
    if (bRunTerm)
    {
        if (!futime1TestFuncList[3])
            DbgPrint("Term function NULL!\n");
        else
            futime1TestFuncList[3](0);
    }
    int ret = GetTestFailure() ? 1 : 0;
    ReportTest(false);
    ClearFailures();
    return ret;
}

