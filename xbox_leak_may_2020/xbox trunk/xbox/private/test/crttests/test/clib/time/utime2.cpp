#define QaLib utime2NameSpace

#include <stdio.h>
#include <direct.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\utime.h>
#include <sys\timeb.h>
#include <io.h>
#include <errno.h>

#undef QA_USE_THREADS   // theads make no sense with this test.
#include "qalibrary.h"

/*@ utime

TestCases:
-----------
  1. utime() should fail gracefully when no more file handles.  errno should
     be set to EMFILE.
  
@*/

#define MAXHANDLES 2048
int handles[MAXHANDLES];

_TCHAR readonlyfile[] = _T("utime2.tst");

//
// testcases
//
template <class TF> bool TC1(TF utimeFunc, QA_THREAD_PARAM)
{
    bool retVal = true;

    int fhandle;
    int fh = 0;
    int chk1,chk2,chk3;

    if (!QATCheck((fhandle = _topen(readonlyfile, O_CREAT | O_RDWR, S_IREAD)) != -1))
        return false;
    QACheck(_close(fhandle) == 0);

/* loop until no more file handles are availiable, then
 * make sure utime() fails eloquently:
 *
 * use the file readonlyfile, which happens to be a handy file.
*/
    errno = 0;
    while ((fh < MAXHANDLES ) &&
           errno != EMFILE    &&
           (-1 != (handles[fh++] = _topen(readonlyfile, O_RDONLY))))
        ; /* loop until done */

    retVal &= QATCheck(fh > 150);   // should have opened at least this many.

    chk1 = errno == EMFILE;

    /* no more file handles.. */

    errno = 0;
    chk2 = -1 == utimeFunc(readonlyfile, NULL);

#if !defined(_MAC)
    chk3 = errno == EMFILE;
#else
    chk3 = errno == EACCES;   /* _utime() for mac never sets errno to EMFILE
                                 according to porting doc. */
#endif

    while (--fh >= 0)
    {
        if (handles[fh] == -1)
            continue;
        if (!QATCheck(_close(handles[fh]) == 0))
        {
            QALog << _T("    ** fh == ") << fh << QAEndl;
            retVal = false;
        }
    }

    /* now report if errors */
    retVal &= QATCheck(chk1 != 0);
    retVal &= QATCheck(chk2 != 0);
    retVal &= QATCheck(chk3 != 0);

    retVal &= QATCheck(_tchmod(readonlyfile, S_IWRITE) == 0);
    retVal &= QATCheck(_tunlink(readonlyfile) == 0);

    return retVal;
}

static unsigned __stdcall utime2UTime1(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(1)

        QATTrace1(_T("*** utime() ***"));
        QATCheck(TC1(_tutime, QA_THREAD_PARAM_NAME));

#if (_MSC_VER > 1200)
        QATTrace1(_T("*** utime64() ***"));
        QATCheck(TC1(_tutime64, QA_THREAD_PARAM_NAME));
#endif /* (_MSC_VER > 1200) */

    QA_END_TC(1)
    return 0;
}

TestFunc utime2TestFuncList[] =
{
    utime2UTime1
};


int utime2Entry()
{
    int nTests = (sizeof(utime2TestFuncList)/sizeof(utime2TestFuncList[0]));
    int iTest = 0;
    bool bRunTerm = false;

    if (utime2TestFuncList[0] == 0)
    {
        if (nTests == 1) return 3; /* error, not enough data */
        /* if the 0th element is zero, then 1st element is a */
        /* thread kickoff function */
        iTest = 2;
        if (utime2TestFuncList[1] != 0)
            utime2TestFuncList[1](0);
    }

    if (nTests >= 4)
    {
        if (utime2TestFuncList[2] == 0)
        {
            bRunTerm = true;
            iTest = 4;
        }
    }

    for (; iTest < nTests; iTest++)
        if(utime2TestFuncList[iTest] != 0)
            utime2TestFuncList[iTest](0);
    if (bRunTerm)
    {
        if (!utime2TestFuncList[3])
            DbgPrint("Term function NULL!\n");
        else
            utime2TestFuncList[3](0);
    }
    int ret = GetTestFailure() ? 1 : 0;
    ReportTest(false);
    ClearFailures();
    return ret;
}

