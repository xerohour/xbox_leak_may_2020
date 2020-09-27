#define QaLib utime1NameSpace

#include <stdio.h>
#include <direct.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\utime.h>
#include <sys\timeb.h>
#include <io.h>
#include <errno.h>

#define QA_NTHREADS 1
#include "qalibrary.h"

/*@ utime

TestCases:
-----------
  1. Attempt to modify the time of a directory.  Should fail and set errno to
     EACCES.
  2. Attempt to modify the time of a read-only file.  Should fail and set
     errno to EACCES.
  3. utime() should fail if given a 'bad' file name. errno should be set to
     on of the following: ENOENT, EINVAL or EACCES.
  4. Modify the time for 'normal' file.  Should succeed.
  5. When second argument is 0 the current time should be used.
  
@*/

static _TCHAR readonlyfile[] = _T("e_utime.ro");
_TCHAR testfile1[] = _T("utime1.tst");
_TCHAR testfile2[] = _T("utime2.tst");

_TCHAR* invalid_file_names[] = {
    _T("noprog.non"),
    _T("nodir.nod\\filename.non"),
    _T("\\rootdir\\nofile.non"),
    _T("..\\nofile.non"),
    _T("nodrive:\\file.non"),
    _T("toomany\\\\\\\\\\\\separaters"),
    _T("...........\\toomany\\dots"),
    _T("manybad<<<>>>>|||||^^^chars")
};

/*  The following unsigned long array holds the values used to test _utime().
 *  These values are in seconds from 00:00:00 Jan 1, 1970.
 *
 *  It is undocumented as of 2/12/93, but any time value greater than 1980
 *  is supposed to be valid.
 */

unsigned long try_times[] =
{
    0xFFFFFFFEL,    /* valid     - maximum value */
    3294636800L,    /* valid     - approx. mid May 2074 */
     994967294L,    /* valid     - approx. mid Jun 2001 */
      88965439L     /* valid     - approx. mid Sep 1972 */
};

//
// testcases
//
template <class TF> bool TC1(TF utimeFunc, QA_THREAD_PARAM)
{
    bool retVal = false;

    if (QATCheck(_tmkdir(_T("foobar.qa")) == 0))
    {
        errno = 0;
        retVal = QATCheckE(utimeFunc(_T("foobar.qa"), 0), -1);
        retVal &= QATCheckE(errno, EACCES);
        retVal &= QATCheck(_trmdir(_T("foobar.qa")) == 0);
    }

    return retVal;
}

template <class TF> bool TC2(TF utimeFunc, QA_THREAD_PARAM)
{
    bool retVal = false;

    int  fhandle = _topen(readonlyfile, O_CREAT | O_RDWR, S_IREAD);
    if (QATCheckNE(fhandle, -1))
    {
        QATCheck(_close(fhandle) == 0);
        errno = 0;
        retVal = QATCheckE(utimeFunc(readonlyfile, 0), -1);
        retVal &= QATCheckE(errno, EACCES);
        QATCheck(_tchmod(readonlyfile, S_IWRITE) == 0);
        QATCheck(_tunlink(readonlyfile) == 0);
    }

    return retVal;
}

template <class UTIMBUF, class TF> bool TC3(TF utimeFunc, QA_THREAD_PARAM)
{
    bool retVal = true;

    for (_TCHAR** test_name = invalid_file_names;
         test_name < &invalid_file_names[sizeof( invalid_file_names )/sizeof( invalid_file_names[0] ) ];
         test_name++)
    {
        UTIMBUF thistimebuf;
        errno = 0;
        if (QATCheck(utimeFunc(*test_name, &thistimebuf) == -1))
        {
            retVal &= QATCheck(errno == ENOENT || errno == EINVAL);
        }
        else
            retVal = false;
    }

    return retVal;
}

template <class T, class UTIMBUF, class STAT, class TF1, class TF2> 
bool TC4(TF1 utimeFunc, TF2 statFunc, QA_THREAD_PARAM)
{
    bool    retVal = false;
    UTIMBUF utimes;
    STAT    stimes;
    FILE*   fp;

    if(QATCheck((fp = _tfopen(testfile1, _T("w+"))) != 0))
    {
        retVal = QATCheck(_fputts(_T("Where's the beef?"), fp) != EOF);
        retVal &= QATCheck(fclose(fp) == 0);
    
        for(int i = 0; i < (sizeof(try_times)/sizeof(try_times[0])); i++)
        {
            utimes.modtime = (T)try_times[i];
            utimes.actime = (T)try_times[i];
        
            if(utimeFunc(testfile1, &utimes) == -1 )
            {
                 retVal &= QATCheckE(errno, EINVAL);
            }
            else
            {
                retVal &= QATCheck(statFunc(testfile1, &stimes) == 0);

                // Added this quick fix for NTFS drives when the OS is setting DST.
                // When we have time we should change to check for NTFS partition
                // and if automatic Daylight Savings Time adjustments are turned on.
                // And only then we should add 3600 (1 hour) for DST.

                if (stimes.st_mtime != (T)try_times[i])
                    stimes.st_mtime += 3600; // adjust for dst

                retVal &= QATCheckE(stimes.st_mtime, (T)try_times[i]);
            }
        }
        _tunlink(testfile1);
    }

    return retVal;
}

template <class TIMEB, class STAT, class TF1, class TF2, class TF3> 
bool TC5(TF1 utimeFunc, TF2 statFunc, TF3 ftimeFunc, QA_THREAD_PARAM)
{
    TIMEB   times;
    STAT    stimes;
    FILE*   fp;

    if (!QATCheck((fp = _tfopen(testfile2, _T("w+"))) != 0))
        return false;
    fclose(fp);

    if (QATCheck(utimeFunc(testfile2, NULL) == 0))
    {
        ftimeFunc(&times);
        if(QATCheck(statFunc(testfile2, &stimes) == 0))
        {
            /* Note this test allows for 2 seconds of difference which may not 
             * be enough if your machine is bogged down.  Two seconds is a long 
             * time, but rounding off could contribute to this also.
             */
            QATCheck(_tunlink(testfile2) == 0);
            return QATCheck(stimes.st_mtime - times.time <= 2);
        }
    }

    return false;
}

static unsigned __stdcall utime1UTime1(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(1)

        QATTrace1(_T("*** utime() ***"));
        QATCheck(TC1(_tutime, QA_THREAD_PARAM_NAME));

#if (_MSC_VER > 1200) 
        QATTrace1(_T("*** utime64() ***"));
        QATCheck(TC1(_tutime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(1)
    return 0;
}

static unsigned __stdcall utime1UTime2(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(2)

        QATTrace1(_T("*** utime() ***"));
        QATCheck(TC2(_tutime, QA_THREAD_PARAM_NAME));

#if (_MSC_VER > 1200) 
        QATTrace1(_T("*** utime64() ***"));
        QATCheck(TC2(_tutime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(2)
    return 0;
}

static unsigned __stdcall utime1UTime3(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(3)

        QATTrace1(_T("*** utime() ***"));
        QATCheck(TC3<struct _utimbuf>(_tutime, QA_THREAD_PARAM_NAME));

#if (_MSC_VER > 1200) 
        QATTrace1(_T("*** utime64() ***"));
        QATCheck(TC3<struct __utimbuf64>(_tutime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(3)
    return 0;
}

static unsigned __stdcall utime1UTime4(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(4)

        QATTrace1(_T("*** utime() ***"));
        if (!TC4<long, 
                 struct _utimbuf,
                 struct _stat>(_tutime, _tstat, QA_THREAD_PARAM_NAME))
            QATFail(10);

#if (_MSC_VER > 1200) 
        QATTrace1(_T("*** utime64() ***"));
        if (!TC4<__int64, 
                 struct __utimbuf64,
                 struct __stat64>(_tutime64, _tstat64, QA_THREAD_PARAM_NAME))
            QATFail(20);
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(4)
    return 0;
}

static unsigned __stdcall utime1UTime5(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(5)

        QATTrace1(_T("*** utime() ***"));
        if (!TC5<struct _timeb,
                 struct _stat>(_tutime, _tstat, _ftime, QA_THREAD_PARAM_NAME))
            QATFail(10);

#if (_MSC_VER > 1200) 
        QATTrace1(_T("*** utime64() ***"));
        if (!TC5<struct __timeb64,
                 struct __stat64>(_tutime64, _tstat64, _ftime64, QA_THREAD_PARAM_NAME))
            QATFail(20);
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(5)
    return 0;
}



TestFunc utime1TestFuncList[] =
{
    utime1UTime1,
    utime1UTime2,
    utime1UTime3,
    utime1UTime4,
    utime1UTime5
};


int utime1Entry()
{
    int nTests = (sizeof(utime1TestFuncList)/sizeof(utime1TestFuncList[0]));
    int iTest = 0;
    bool bRunTerm = false;

    if (utime1TestFuncList[0] == 0)
    {
        if (nTests == 1) return 3; /* error, not enough data */
        /* if the 0th element is zero, then 1st element is a */
        /* thread kickoff function */
        iTest = 2;
        if (utime1TestFuncList[1] != 0)
            utime1TestFuncList[1](0);
    }

    if (nTests >= 4)
    {
        if (utime1TestFuncList[2] == 0)
        {
            bRunTerm = true;
            iTest = 4;
        }
    }

    for (; iTest < nTests; iTest++)
        if(utime1TestFuncList[iTest] != 0)
            utime1TestFuncList[iTest](0);
    if (bRunTerm)
    {
        if (!utime1TestFuncList[3])
            DbgPrint("Term function NULL!\n");
        else
            utime1TestFuncList[3](0);
    }
    int ret = GetTestFailure() ? 1 : 0;
    ReportTest(false);
    ClearFailures();
    return ret;
}

