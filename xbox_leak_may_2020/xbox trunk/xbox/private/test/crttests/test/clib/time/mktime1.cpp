#define QaLib mktime1NameSpace

#include <time.h>
#include "qalibrary.h"

/*@ mktime

Test Cases:
-----------
 1-5. timeptr points to a tm struct in which all values are in correct      
      range, and the time represented is before midnight 1/1/1970 or after   
      midnight 2/5/2036 (32bit) OR 12/31/2999 (64bit).  The return value from 
      mktime() should be (time_t)-1.
     
 6. timeptr points to a tm struct in which all values are in correct        
    range, and the time represented is after midnight 1/1/1970 and before  
    midnight 2/5/2036.  Verify that the struct's values have not been      
    changed.   Verify that the return value represents the same time as the 
    struct.                                     
                                         
 7-8. timeptr points to a tm struct in which all values but tm_wday and      
      tm_yday are in correct range, and the time represented is after       
      midnight 1/1/70. Verify that the values for tm_wday and/or tm_yday    
      are in range.                                 
                                         
 9. timeptr points to a tm struct in which some, possibly all, fields are  
    out-of-range.  If -1 is not returned, check that the structure is      
    complete and normalized (in range).        
    
10. Regression test from Plum Hall C library test suite. 

11. 32bit/64bit leap year recognition for year 2000. 

12-17. 64bit leap year recognition.
       12-15: not a leap year: 2100, 2200, 2300, 2500.
       16-17: leap year: 2400, 2800.
 
@*/

// macros
//
#define NOT_EQUAL 1
#define EQUAL     0

/*
 *  Structures for tests of type 1 above.
 */

struct tm A[] = {  { 12, 32, 15, 20,  4, 50, 0,  0, -1 },
           { 59, 59, 15, 31, 11, 69, 0,  0,  0 },
           {  0,  0,  0,  1,  0,  0, 0,  0,  0 },
           { 34, 45,  8, 21,  7, 29, 0,  0, -1 } };

// 32bit version.
struct tm A1[] = {  
    { 21, 21, 21, 21,  2,151, 0,     0, -1 },
    {  0,  0,  0,  6,  1,138, 0,     0, -1 }
};

// 64bit version.
struct tm A2[] = {  
    { 21, 21, 21, 21,  2,151, 0,     0, 0 },
    {  0,  0,  0,  6,  1,138, 0,     0, 0 }
};


/*
 *  Structures for tests of type 2 above.  These structures are also used
 *  when testing type 3.
 */

struct tm B[] = { { 12, 32, 15, 20,  1, 98, 5,  50,  0 },
          {  1,  1,  1, 28,  6, 80, 1, 209,  1 },
          { 59, 59, 23,  8,  8,130, 0, 250,  1 },
          { 29, 38,  8, 19, 10,110, 5, 322,  0 } };


/*
 *  Structures for tests of type 3 above.
 */

struct tm C[] = {  { 53, 22,  5, 10,  8,125, 0,  0, -1 },
           { 59, 59, 23, 31, 11, 99, 0,  0, -1 },
           { 50, 40, 20, 30, 10,100, 0,  0, -1 },
           {  0,  0,  0,  1,  0, 70, 0,  0, -1 },
           { 28, 36, 18, 24,  5,104, 0,  0, -1 } };


/*
 *  Structures for tests of type 4 above.
 */

struct tm D[] = {  { 95, 22, -5, 10,  8,125, 9,  0, -1 },
           { 12, 12, 12, 12, 12, 99, 0, 12, -1 },
           {  0,  0,  0,  1,  0,170, 0,  0, -1 },
           { 45, 66,  9, 27,  7, 89, 0,  0, -1 },
           {  9, 41, 20, 66, 11,100, 1, 66, -1 },
           { 33, 55, 11,  1, -4,111, 0,  0, -1 },
           { 99,-21, 33,  9, 20, 91, 5,  5, -1 },
           {-86,  1, 42, 18,  1, 76, 0,  0, -1 },
           { 28, 36, 18,-24,  5,104,-6, -6, -1 } };


void copyStruct(struct tm *src, struct tm *dest);
int  compareStruct(struct tm *t1, struct tm *t2);


// Helper test functions.  These are template functions to test both 32bit
// and 64bit.
//

// all should fail.
//
template <class TT, class TF> bool TC1(TF timeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    struct tm   Temp;

    for (int i = 0; i < sizeof(A)/sizeof(A[0]); i++)
    {
        copyStruct(&Temp, &A[i]);

        if (!QATCheckE(timeFunc(&Temp), (TT)-1))
        {
            QATFail(i);
            return false;
        }
    }

    return true;
}

template <class TT, class TF1, class TF2> 
bool TC6(TF1 mktimeFunc, TF2 localtimeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    TT          result;
    struct tm   Temp;
    struct tm*  ptr;
    bool        retVal = true;

    for (int i = 0; i < sizeof(B)/sizeof(B[0]); i++)
    {
        copyStruct( &Temp, &B[i] );
    
        result = mktimeFunc(&Temp);
        if ( result == (TT)-1 )
            QATFail( 10 + i );
        else
        {
            if ( compareStruct( &Temp, &B[i] ) == NOT_EQUAL )
                QATFail( 20 + i );
    
            ptr = localtimeFunc( &result );
            if ( ptr == NULL )
                QATFail( 30 + i );
            else if ( compareStruct( ptr, &B[i] ) == NOT_EQUAL )
                QATFail( 40 + i );
            else
            {
                retVal &= true;
                continue;
            }
        }
        retVal = false;
    }

    return retVal;
}

template <class TT, class TF> bool TC7(TF timeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    TT          result;
    struct tm   Temp;
    bool        retVal = true;

    for (int i = 0; i < sizeof(C)/sizeof(C[0]); i++)
    {
        copyStruct( &Temp, &C[i] );
    
        result = timeFunc( &Temp );
        if ( result == (TT)-1 )
        {
            QATFail( 10 + i );
            retVal = false;
        }
        else
        {
            if ( (Temp.tm_wday < 0) || (Temp.tm_wday > 6) )
            {
                QATFail( 20 + i );
                retVal = false;
            }
    
            if ( (Temp.tm_yday < 0) || (Temp.tm_yday > 365) )
            {
                QATFail( 30 + i );
                retVal = false;
            }
        }
    }

    return retVal;
}

template <class TT, class TF> bool TC8(TF timeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    TT          result;
    struct tm   Temp;
    bool        retVal = true;

    for (int i = 0; i < sizeof(B)/sizeof(B[0]); i++)
    {
        copyStruct( &Temp, &B[i] );
    
        result = timeFunc( &Temp );
        if ( result == (TT)-1 )
        {
            QATFail( 10 + i );
            retVal = false;
        }
        else
        {
            if ( Temp.tm_wday != B[i].tm_wday )
            {
                QATFail( 20 + i );
                retVal = false;
            }
    
            if ( Temp.tm_yday != B[i].tm_yday )
            {
                QATFail( 30 + i );
                retVal = false;
            }
        }
    }

    return retVal;
}

template <class TT, class TF> bool TC9(TF timeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    TT          result;
    struct tm   Temp;
    bool        retVal = true;

    for (int i = 0; i < sizeof(D)/sizeof(D[0]); i++)
    {
        copyStruct( &Temp, &D[i] );
    
        result = timeFunc( &Temp );
        if ( result != (TT)-1 )
        {
            if ( (Temp.tm_sec < 0) || (Temp.tm_sec > 59) )
            {
                QATFail( 10 + i );
                retVal = false;
            }
    
            if ( (Temp.tm_min < 0) || (Temp.tm_min > 59) )
            {
                QATFail( 20 + i );
                retVal = false;
            }
    
            if ( (Temp.tm_hour < 0) || (Temp.tm_hour > 23) )
            {
                QATFail( 30 + i );
                retVal = false;
            }
    
            if ( (Temp.tm_mon < 0) || (Temp.tm_mon > 11) )
            {
                QATFail( 40 + i );
                retVal = false;
            }
    
            if ( (Temp.tm_year < 70) || (Temp.tm_wday > 1099) )
            {
                QATFail( 50 + i );
                retVal = false;
            }
    
            if ( (Temp.tm_wday < 0) || (Temp.tm_wday > 6) )
            {
                QATFail( 60 + i );
                retVal = false;
            }
    
            if ( (Temp.tm_wday < 0) || (Temp.tm_wday > 6) )
            {
                QATFail( 70 + i );
                retVal = false;
            }
    
            if ( (Temp.tm_yday < 0) || (Temp.tm_yday > 365) )
            {
                QATFail( 80 + i );
                retVal = false;
            }
        }
    }

    return retVal;
}

template <class TT, class TF> bool TC10(TF timeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    struct tm   tm1;
    TT          time_t1, time_t2;
    bool        retVal = true;

    /* make sure that all of the required fields are there */
    tm1.tm_sec = 0;
    tm1.tm_min = 49;
    tm1.tm_hour = 20;
    tm1.tm_mday = 12;
    tm1.tm_mon = 8;
    tm1.tm_year = 76;
    tm1.tm_wday = -99;
    tm1.tm_yday = -99;
    tm1.tm_isdst = 1;

    /*
     * compute the difference between two times
     */
    time_t1 = timeFunc(&tm1);
    tm1.tm_sec = 5;
    time_t2 = timeFunc(&tm1);

    if( time_t1 == time_t2 )
    {
        _tprintf( _T("mktime failed\n") );
        retVal = false;
    }
    else
    {
        if(!QATCheckE((time_t1 - time_t2), (TT)-5))
        {
            _tprintf( _T("difftime failed\n") );
            retVal = false;
        }
    }
    return retVal;
}

template <class TT, class TF> bool TC11(TF timeFunc, QA_THREAD_PARAM, TT tt = 0)
{
    struct tm   tm1;
    bool        retVal = true;

    // set time to be 2/29/2000.
    //
    tm1.tm_sec = 0;
    tm1.tm_min = 0;
    tm1.tm_hour = 0;
    tm1.tm_mday = 29;
    tm1.tm_mon = 1;
    tm1.tm_year = 100;
    tm1.tm_wday = -1;
    tm1.tm_yday = -1;
    tm1.tm_isdst = 0;
    
    if (QATCheckNE(timeFunc(&tm1), (TT)-1))
    {
        QATCheckE(tm1.tm_mon, 1);
        QATCheckE(tm1.tm_mday, 29);
    }
    else
        retVal = false;

    return retVal;
}

static unsigned __stdcall mktime1MkTime1(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(1)

	if	(sizeof(time_t) == 4)
	{
        QATTrace1(_T("mktime()"));
        QATCheck(TC1<time_t>(mktime, QA_THREAD_PARAM_NAME));
	}

#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
        QATCheck(TC1<__time64_t>(_mktime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(1)

    QA_BEGIN_TC(2)

	if	(sizeof(time_t) == 4)
	{
        struct tm   temp = A1[0];

        QATTrace1(_T("mktime()"));
        QATCheckE(mktime(&temp), (time_t)-1);
	}
    
    QA_END_TC(2)

    QA_BEGIN_TC(3)

	if	(sizeof(time_t) == 4)
	{
        struct tm   temp = A1[1];
    
        QATTrace1(_T("mktime()"));
        QATCheckE(mktime(&temp), (time_t)-1);
	}
    
    QA_END_TC(3)

    QA_BEGIN_TC(4)

#if (_MSC_VER > 1200)

		struct tm   temp = A1[0];
    
        QATTrace1(_T("mktime64()"));
        QATCheckNE(_mktime64(&temp), (__time64_t)-1);
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(4)

    QA_BEGIN_TC(5)

#if (_MSC_VER > 1200)   

		struct tm   temp = A1[1];
 
        QATTrace1(_T("mktime64()"));
        QATCheckNE(_mktime64(&temp), (__time64_t)-1);
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(5)

    return 0;
}

static unsigned __stdcall mktime1MkTime2(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(6)

	if	(sizeof(time_t) == 4)
	{
        QATTrace1(_T("mktime()"));
        QATCheck(TC6<time_t>(mktime, localtime, QA_THREAD_PARAM_NAME));
	}

#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
        QATCheck(TC6<__time64_t>(_mktime64, _localtime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(6)
    return 0;
}

static unsigned __stdcall mktime1MkTime3(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(7)

	if	(sizeof(time_t) == 4)
	{
        QATTrace1(_T("mktime()"));
        QATCheck(TC7<time_t>(mktime, QA_THREAD_PARAM_NAME));
	}

#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
        QATCheck(TC7<__time64_t>(_mktime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(7)

    QA_BEGIN_TC(8)

	if	(sizeof(time_t) == 4)
	{
        QATTrace1(_T("mktime()"));
        QATCheck(TC8<time_t>(mktime, QA_THREAD_PARAM_NAME));
	}

#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
        QATCheck(TC8<__time64_t>(_mktime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(8)

    return 0;
}

static unsigned __stdcall mktime1MkTime4(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(9)

	if	(sizeof(time_t) == 4)
	{
        QATTrace1(_T("mktime()"));
        QATCheck(TC9<time_t>(mktime, QA_THREAD_PARAM_NAME));
	}
		
#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
        QATCheck(TC9<__time64_t>(_mktime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(9)
    return 0;
}

static unsigned __stdcall mktime1MkTime5(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(10)

	if	(sizeof(time_t) == 4)
	{
        QATTrace1(_T("mktime()"));
        QATCheck(TC10<time_t>(mktime, QA_THREAD_PARAM_NAME));
	}
		
#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
        QATCheck(TC10<__time64_t>(_mktime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(10)
    return 0;
}

static unsigned __stdcall mktime1MkTime6(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(11)

	if	(sizeof(time_t) == 4)
	{
        QATTrace1(_T("mktime()"));
        QATCheck(TC11<time_t>(mktime, QA_THREAD_PARAM_NAME));
	}
		
#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
        QATCheck(TC11<__time64_t>(_mktime64, QA_THREAD_PARAM_NAME));
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(11)
    return 0;
}

static unsigned __stdcall mktime1MkTime7(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(12)

#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
    
        struct tm   tm1;
    
        // set time to be 2/29/2100.
        //
        tm1.tm_sec = 0;
        tm1.tm_min = 0;
        tm1.tm_hour = 0;
        tm1.tm_mday = 29;
        tm1.tm_mon = 1;
        tm1.tm_year = 200;
        tm1.tm_wday = -1;
        tm1.tm_yday = -1;
        tm1.tm_isdst = 0;
        
        if (QATCheckNE(_mktime64(&tm1), (__time64_t)-1))
        {
            QATCheckE(tm1.tm_mon, 2);
            QATCheckE(tm1.tm_mday, 1);
        }
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(12)

    QA_BEGIN_TC(13)

#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
    
        struct tm   tm1;
    
        // set time to be 2/29/2200.
        //
        tm1.tm_sec = 0;
        tm1.tm_min = 0;
        tm1.tm_hour = 0;
        tm1.tm_mday = 29;
        tm1.tm_mon = 1;
        tm1.tm_year = 300;
        tm1.tm_wday = -1;
        tm1.tm_yday = -1;
        tm1.tm_isdst = 0;
        
        if (QATCheckNE(_mktime64(&tm1), (__time64_t)-1))
        {
            QATCheckE(tm1.tm_mon, 2);
            QATCheckE(tm1.tm_mday, 1);
        }
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(13)

    QA_BEGIN_TC(14)

#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
    
        struct tm   tm1;
    
        // set time to be 2/29/2300.
        //
        tm1.tm_sec = 0;
        tm1.tm_min = 0;
        tm1.tm_hour = 0;
        tm1.tm_mday = 29;
        tm1.tm_mon = 1;
        tm1.tm_year = 400;
        tm1.tm_wday = -1;
        tm1.tm_yday = -1;
        tm1.tm_isdst = 0;
        
        if (QATCheckNE(_mktime64(&tm1), (__time64_t)-1))
        {
            QATCheckE(tm1.tm_mon, 2);
            QATCheckE(tm1.tm_mday, 1);
        }
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(14)

    QA_BEGIN_TC(15)

#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
    
        struct tm   tm1;
    
        // set time to be 2/29/2500.
        //
        tm1.tm_sec = 0;
        tm1.tm_min = 0;
        tm1.tm_hour = 0;
        tm1.tm_mday = 29;
        tm1.tm_mon = 1;
        tm1.tm_year = 600;
        tm1.tm_wday = -1;
        tm1.tm_yday = -1;
        tm1.tm_isdst = 0;
        
        if (QATCheckNE(_mktime64(&tm1), (__time64_t)-1))
        {
            QATCheckE(tm1.tm_mon, 2);
            QATCheckE(tm1.tm_mday, 1);
        }
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(15)

    return 0;
}

static unsigned __stdcall mktime1MkTime8(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(16)

#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
    
        struct tm   tm1;
    
        // set time to be 2/29/2400.
        //
        tm1.tm_sec = 0;
        tm1.tm_min = 0;
        tm1.tm_hour = 0;
        tm1.tm_mday = 29;
        tm1.tm_mon = 1;
        tm1.tm_year = 500;
        tm1.tm_wday = -1;
        tm1.tm_yday = -1;
        tm1.tm_isdst = 0;
        
        if (QATCheckNE(_mktime64(&tm1), (__time64_t)-1))
        {
            QATCheckE(tm1.tm_mon, 1);
            QATCheckE(tm1.tm_mday, 29);
        }
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(16)

    QA_BEGIN_TC(17)

#if (_MSC_VER > 1200)
        QATTrace1(_T("mktime64()"));
    
        struct tm   tm1;
    
        // set time to be 2/29/2800.
        //
        tm1.tm_sec = 0;
        tm1.tm_min = 0;
        tm1.tm_hour = 0;
        tm1.tm_mday = 29;
        tm1.tm_mon = 1;
        tm1.tm_year = 900;
        tm1.tm_wday = -1;
        tm1.tm_yday = -1;
        tm1.tm_isdst = 0;
        
        if (QATCheckNE(_mktime64(&tm1), (__time64_t)-1))
        {
            QATCheckE(tm1.tm_mon, 1);
            QATCheckE(tm1.tm_mday, 29);
        }
#endif  /* (_MSC_VER > 1200) */

    QA_END_TC(17)

    return 0;
}

// helper functions.
//
/*
 *  copyStruct() takes two pointers to structures of type "tm" and
 *  copies the fields of src to the fields of dest.
 */

void copyStruct(struct tm *src, struct tm *dest)
{
    src->tm_sec   = dest->tm_sec;
    src->tm_min   = dest->tm_min;
    src->tm_hour  = dest->tm_hour;
    src->tm_mday  = dest->tm_mday;
    src->tm_mon   = dest->tm_mon;
    src->tm_year  = dest->tm_year;
    src->tm_wday  = dest->tm_wday;
    src->tm_yday  = dest->tm_yday;
    src->tm_isdst = dest->tm_isdst;
}


/*
 *  compareStruct() takes two pointers to structures of type "tm" and
 *  compares the fields of t1 to those of t2.
 */

int compareStruct(struct tm *t1, struct tm *t2)
{
    if ( t1->tm_sec   != t2->tm_sec   ||
         t1->tm_min   != t2->tm_min   ||
         t1->tm_hour  != t2->tm_hour  ||
         t1->tm_mday  != t2->tm_mday  ||
         t1->tm_mon   != t2->tm_mon   ||
         t1->tm_year  != t2->tm_year  ||
         t1->tm_wday  != t2->tm_wday  ||
         t1->tm_yday  != t2->tm_yday  ||
         t1->tm_isdst != t2->tm_isdst )
        return( NOT_EQUAL );
    else
        return( EQUAL );
}

TestFunc mktime1TestFuncList[] =
{
    mktime1MkTime1,
    mktime1MkTime2,
    mktime1MkTime3,
    mktime1MkTime4,
    mktime1MkTime5,
    mktime1MkTime6,
    mktime1MkTime7,
    mktime1MkTime8
};


int mktime1Entry()
{
    int nTests = (sizeof(mktime1TestFuncList)/sizeof(mktime1TestFuncList[0]));
    int iTest = 0;
    bool bRunTerm = false;

    if (mktime1TestFuncList[0] == 0)
    {
        if (nTests == 1) return 3; /* error, not enough data */
        /* if the 0th element is zero, then 1st element is a */
        /* thread kickoff function */
        iTest = 2;
        if (mktime1TestFuncList[1] != 0)
            mktime1TestFuncList[1](0);
    }

    if (nTests >= 4)
    {
        if (mktime1TestFuncList[2] == 0)
        {
            bRunTerm = true;
            iTest = 4;
        }
    }

    for (; iTest < nTests; iTest++)
        if(mktime1TestFuncList[iTest] != 0)
            mktime1TestFuncList[iTest](0);
    if (bRunTerm)
    {
        if (!mktime1TestFuncList[3])
            DbgPrint("Term function NULL!\n");
        else
            mktime1TestFuncList[3](0);
    }
    int ret = GetTestFailure() ? 1 : 0;
    ReportTest(false);
    ClearFailures();
    return ret;
}

