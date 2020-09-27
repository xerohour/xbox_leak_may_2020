#define QaLib ctime1NameSpace

#include <stdio.h>
#include <time.h>
#include <string.h>

#define QA_NTHREADS 1
#include "qalibrary.h"

/*@ ctime

ctime(), wctime(), _ctime64(), _wctime64().

Will need to set the system time for boundary and Y2K testing.

Test Cases:
------------
 1. date < 1/1/1970.
 2. string size == 25.
 3. that 24hr clock is used.
 4. all fields have the following format: (3s3s2s2:2:2s4\n\0).
 5. time is adjusted to local time zone.
 6. use of static buffer does not fail when threads are used.
 7. shared access to static buffer with asctime, localtime, and gmtime 
    acts as expected (overwrites but is thread safe).
    
General time related TCs: 
 8. YK2 issue:
    8a. 2000
    8b. 2001
    ?? others ??
 9. boundary:
    9a. 2038 (32bit)
    9b. 2039 (32bit) - fail.
    9c. 2999 (64bit)
    9d. 3000 (64bit) - fail.
10. "edge" conditions:
    10a. 2100, 2200, 2300, 2500 is NOT a leap year.
    10b. 2000 (32bit also), 2400, 2800 is a leap year.
    
@*/


// globals.
//
// leave room so strlen won't crash if greater than 26.
#define BUFF_SIZE           128 
#define NUM_MONTHS          12
#define NUM_DAYS            7
#define OFFSET_YR           1994
#define INITIAL_TEST_TIME   757411200
#define EXPECTED_SIZE       25

_TCHAR *short_months[12]= 
{
    _T("Jan"),
    _T("Feb"),
    _T("Mar"),
    _T("Apr"),
    _T("May"),
    _T("Jun"),
    _T("Jul"),
    _T("Aug"),
    _T("Sep"),
    _T("Oct"),
    _T("Nov"),
    _T("Dec")
};

_TCHAR *short_days[7] = 
{
    _T("Sat"),
    _T("Sun"),
    _T("Mon"),
    _T("Tue"),
    _T("Wed"),
    _T("Thu"),
    _T("Fri")
};

bool    VerifyFormat(const _TCHAR* s)
{
    _TCHAR  szDOW[4];
    _TCHAR  szMon[4];
    _TCHAR  szDay[3];
    _TCHAR  szHour[3];
    _TCHAR  szMin[3];
    _TCHAR  szSec[3];
    _TCHAR  szYear[5];
    bool    ok = false;
    bool    retval = true;

    if (!QACheckE(_tcslen(s), (size_t)EXPECTED_SIZE))
        return false;

    _stscanf(s, _T("%3s %3s %2s %2s:%2s:%2s %4s"), 
             szDOW, szMon, szDay, szHour, szMin, szSec, szYear);

    // check day of week
    //
    for (int i = 0; i < 7; i++)
        if (_tcscmp(szDOW, short_days[i]) == 0)
        {
            ok = true;
            break;
        }
    if (!QACheck(ok))
        QALog << _T("Day of week incorrect. \"") << szDOW 
              << _T("\" not found. index = ") << i << QAEndl;
    retval &= ok;
    ok = false;

    // check month
    //
    for (i = 0; i < 12; i++)
        if (_tcscmp(szMon, short_months[i]) == 0)
        {
            ok = true;
            break;
        }
    if (!QACheck(ok))
        QALog << _T("Month incorrect. \"") << szMon
              << _T("\" not found. index = ") << i << QAEndl;
    retval &= ok;
    ok = false;

    // check day
    //
    retval &= QACheckE(_tcslen(szDay), (size_t)2);
    retval &= QACheck(_ttoi(szDay) >= 0 && _ttoi(szDay) <= 31);

    // check hour
    //
    retval &= QACheckE(_tcslen(szHour), (size_t)2);
    retval &= QACheck(_ttoi(szHour) >= 0 && _ttoi(szHour) <= 24);

    // check minute
    //
    retval &= QACheckE(_tcslen(szMin), (size_t)2);
    retval &= QACheck(_ttoi(szMin) >= 0 && _ttoi(szMin) <= 60);

    // check seconds
    //
    retval &= QACheckE(_tcslen(szSec), (size_t)2);
    retval &= QACheck(_ttoi(szSec) >= 0 && _ttoi(szSec) <= 60);

    // check year
    //
    retval &= QACheckE(_tcslen(szYear), (size_t)4);
    retval &= QACheck(_ttoi(szYear) >= 1970 && _ttoi(szYear) <= 2999);

    return retval;
}

// Helper test functions.  These are template functions to test both 32bit
// and 64bit.
//
template <class TT, class TF> bool TC1(TF tf, TT tt = 0)
{
    _TCHAR  buff[ BUFF_SIZE ];
    int     number;
    TT      test_time = INITIAL_TEST_TIME;
    bool    ret = true;

    for (int loop1 = 0; loop1 <= 20; loop1++)
    {
        QATTrace2(_T("loop count: "), loop1);
        // Decimal year
        _tcsncpy(buff, tf(&test_time), BUFF_SIZE);
        _stscanf(buff, _T("%*s%*s%*s%*s%d"), &number);
            
        ret &= QACheckE(number, (OFFSET_YR + loop1));
        ret &= QACheck(VerifyFormat(buff));

        test_time += 31622400;
    }

    return ret;
}

template <class TT, class TF> bool TC2(TF tf, TT tt = 0)
{
    _TCHAR  buff[ BUFF_SIZE ];
    _TCHAR  string[20];
    TT      test_time = INITIAL_TEST_TIME;
    bool    ret = true;

    // Test months
    //
    for (int loop1 = 0; loop1 < NUM_MONTHS; loop1++)
    {
        _tcsncpy( buff, tf(&test_time), BUFF_SIZE);
        _stscanf( buff, _T("%*s%s"), string);

        ret &= QACheckE(_tcslen(buff), (size_t)25);
        ret &= QACheckE(string, short_months[loop1]);

        test_time += 2707200;
    }

    return ret;
}

template <class TT, class TF> bool TC3(TF tf, TT tt = 0)
{
    _TCHAR  buff[ BUFF_SIZE ];
    _TCHAR  string[20];
    TT      test_time = INITIAL_TEST_TIME;
    bool    ret = true;

    /* Test days */

    for ( int loop1 = 0; loop1 < NUM_DAYS; loop1++ )
    {

        /* Day as an abbreviation */

        _tcsncpy( buff, tf( &test_time ), BUFF_SIZE );
        _stscanf( buff, _T("%s"), string );

        ret &= QACheckE(_tcslen(buff), (size_t)25);
        ret &= QACheckE(string, short_days[loop1]);

        test_time += 86400;
    }

    return ret;
}

template <class TT, class TF> bool TC4(TF tf, TT tt = 0)
{
    _TCHAR  buff[ BUFF_SIZE ];
    _TCHAR  string[20];
    int     number;
    TT      test_time = INITIAL_TEST_TIME;
    bool    ret = true;

    /* Hour */

    for ( int loop1 = 0; loop1 < 24; loop1++ )
    {
        _tcsncpy( buff, tf ( &test_time ), BUFF_SIZE );
        _stscanf( buff, _T("%[^:]"), string );
        _stscanf( string, _T("%*s%*s%*s%d"), &number );

        ret &= QACheckE(_tcslen(buff), (size_t)25);
        ret &= QACheckE(number, loop1);

        test_time += 3600;
    }

    return ret;
}

template <class TT, class TF> bool TC5(TF tf, TT tt = 0)
{
    _TCHAR  buff[ BUFF_SIZE ];
    _TCHAR  string[20];
    int     number;
    TT      test_time = INITIAL_TEST_TIME;
    bool    ret = true;

    for (int loop1 = 0; loop1 < 60; loop1++)
    {

        _tcsncpy( buff, tf( &test_time ), BUFF_SIZE );
        _stscanf ( buff, _T("%*s%*s%*s%s"), string );
        _stscanf ( string, _T("%*d:%d:%*d"), &number );

        ret &= QACheckE(_tcslen(buff), (size_t)25);
        ret &= QACheckE(number, loop1);

        test_time += 60;
    }

    return ret;
}

template <class TT, class TF> bool TC6(TF tf, TT tt = 0)
{
    _TCHAR  buff[ BUFF_SIZE ];
    _TCHAR  string[20];
    int     number;
    TT      test_time = INITIAL_TEST_TIME;
    bool    ret = true;

    for (int loop1 = 0; loop1 < 60; loop1++)
    {

        _tcsncpy( buff, tf( &test_time ), BUFF_SIZE );
        _stscanf( buff, _T("%*s%*s%*s%s"), string );
        _stscanf( string, _T("%*d:%*d:%d"), &number );

        ret &= QACheckE(_tcslen(buff), (size_t)25);
        ret &= QACheckE(number, loop1);

        test_time++;

    }

    return ret;
}

template <class TT, class TF> bool TC7(TF tf, TT tt = 0)
{
    TT      test_time = (TT)-1;
    return QACheck(tf(&test_time) == NULL);
}

template <class TT, class TF> bool TC8(TF tf, TT tt = 0)
{
    _TCHAR* pchResult;
    TT      nTime = 0x2e19bd9c;
    bool    retval = true;
    
    _tzset();
    pchResult = tf(&nTime);
    retval &= QACheck(pchResult != NULL);
    retval &= QACheckE(pchResult, _T("Tue Jul 05 13:07:56 1994\n"));
    return retval;
}

////////////////////////////////////////////////////////////////////////////////
// <function name> 
//  <description of what the function does>
//
static unsigned __stdcall ctime1CTime1(void* qaThreadParam = 0)
{
    // Test Year.
    //
    QA_BEGIN_TC(1)

        QATCheck(TC1<time_t>(_tctime));

#if (_MSC_VER > 1200)
        QATCheck(TC1<__time64_t>(_tctime64));
#endif /* (_MSC_VER > 1200) */

    QA_END_TC(1)
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// <function name> 
//  <description of what the function does>
//
static unsigned __stdcall ctime1CTime2(void* qaThreadParam = 0)
{
    // Test Month.
    //
    QA_BEGIN_TC(2)
    
        QATCheck(TC2<time_t>(_tctime));

#if (_MSC_VER > 1200)
        QATCheck(TC2<__time64_t>(_tctime64));
#endif /* (_MSC_VER > 1200) */

    QA_END_TC(2)
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// <function name> 
//  <description of what the function does>
//
static unsigned __stdcall ctime1CTime3(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(3)
    
        QATCheck(TC3<time_t>(_tctime));

#if (_MSC_VER > 1200)
	QATCheck(TC3<__time64_t>(_tctime64));
#endif /* (_MSC_VER > 1200) */

    QA_END_TC(3)
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// <function name> 
//  <description of what the function does>
//
static unsigned __stdcall ctime1CTime4(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(4)

        QATCheck(TC4<time_t>(_tctime));
	
#if (_MSC_VER > 1200)
        QATCheck(TC4<__time64_t>(_tctime64));
#endif /* (_MSC_VER > 1200) */
    
    QA_END_TC(4)
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// <function name> 
//  <description of what the function does>
//
static unsigned __stdcall ctime1CTime5(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(5)
    
        QATCheck(TC5<time_t>(_tctime));
	
#if (_MSC_VER > 1200)
        QATCheck(TC5<__time64_t>(_tctime64));
#endif /* (_MSC_VER > 1200) */
    
    QA_END_TC(5)
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// <function name> 
//  <description of what the function does>
//
static unsigned __stdcall ctime1CTime6(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(6)
    
        QATCheck(TC6<time_t>(_tctime));
	
#if (_MSC_VER > 1200)
        QATCheck(TC6<__time64_t>(_tctime64));
#endif /* (_MSC_VER > 1200) */
    
    QA_END_TC(6)
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// <function name> 
//  <description of what the function does>
//
static unsigned __stdcall ctime1CTime7(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(7)
    
        QATCheck(TC7<time_t>(_tctime));
	
#if (_MSC_VER > 1200)
        QATCheck(TC7<__time64_t>(_tctime64));
#endif /* (_MSC_VER > 1200) */
    
    QA_END_TC(7)
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// <function name> 
//  <description of what the function does>
//
static unsigned __stdcall ctime1CTime8(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(8)
    
        QATCheck(TC8<time_t>(_tctime));
	
#if (_MSC_VER > 1200)
        QATCheck(TC8<__time64_t>(_tctime64));
#endif /* (_MSC_VER > 1200) */
    
    QA_END_TC(8)
    return 0;
}

TestFunc ctime1TestFuncList[] =
{
    ctime1CTime1,
    ctime1CTime2,
    ctime1CTime3,
    ctime1CTime4,
    ctime1CTime5,
    ctime1CTime6,
    ctime1CTime7,
    ctime1CTime8
};

int ctime1Entry()
{
    int nTests = (sizeof(ctime1TestFuncList)/sizeof(ctime1TestFuncList[0]));
    int iTest = 0;
    bool bRunTerm = false;

    if (ctime1TestFuncList[0] == 0)
    {
        if (nTests == 1) return 3; /* error, not enough data */
        /* if the 0th element is zero, then 1st element is a */
        /* thread kickoff function */
        iTest = 2;
        if (ctime1TestFuncList[1] != 0)
            ctime1TestFuncList[1](0);
    }

    if (nTests >= 4)
    {
        if (ctime1TestFuncList[2] == 0)
        {
            bRunTerm = true;
            iTest = 4;
        }
    }

    for (; iTest < nTests; iTest++)
        if(ctime1TestFuncList[iTest] != 0)
            ctime1TestFuncList[iTest](0);
    if (bRunTerm)
    {
        if (!ctime1TestFuncList[3])
            DbgPrint("Term function NULL!\n");
        else
            ctime1TestFuncList[3](0);
    }
    int ret = GetTestFailure() ? 1 : 0;
    ReportTest(false);
    ClearFailures();
    return ret;
}

