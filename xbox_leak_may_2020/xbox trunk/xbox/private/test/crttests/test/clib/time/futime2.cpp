#define QaLib futime2NameSpace

#include <windows.h>
#include <time.h>
#include <sys\stat.h>
#include <sys\timeb.h>
#include <sys\utime.h>
#include <fcntl.h>
#include <sys\types.h>
#include <io.h>
#include <errno.h>

#define QA_NTHREADS 1
#include "qalibrary.h"

/*@ futime

TestCases:
-----------
  1. Check that futime() on a file opened as read only fails and that errno
     is set to EINVAL. 
  2. Check the futime() on a valid file returns success value and that
     the time values are as expected.
  
@*/

// global variables/macros.
//
#define k_cchFileSystemName 12

const _TCHAR* testfile = _T("futime.tst");
const _TCHAR* testfile2 = _T("futime2.tst");


/////////////////////////
//GetFileSystemType - Determine the file system of the current drive.
//  Returns: 0 if FAT
//           1 if NTFS
//          -1 if unknown
int GetFileSystemType()
{
    DWORD              dwMaximumComponentLength;
    DWORD              dwFileSystemFlags;
    char               szFileSystemNameBuffer[k_cchFileSystemName];
    
    if (GetVolumeInformation(NULL, NULL, 0,
                             NULL, &dwMaximumComponentLength, &dwFileSystemFlags,
                             szFileSystemNameBuffer, k_cchFileSystemName))
    {
        if (strcmp(szFileSystemNameBuffer, "FAT") == 0)
            return 0;
        if (strcmp(szFileSystemNameBuffer, "NTFS") == 0)
            return 1;
    }
    return -1;
}

bool DelFile(const _TCHAR* szFile)
{
  int rv;
  rv = _tchmod(szFile, _S_IREAD|_S_IWRITE);
  if (rv == -1) {
    printf("chmod failed errno %d %s %x\n", errno, strerror(errno), GetLastError());
  };
  
  rv = _tremove(szFile);
  if (rv == -1) {
    printf("remove failed errno %d %s %x\n", errno, strerror(errno), GetLastError());
  };
  return rv==0;
}

// testcases.
//
template <class UTIMBUF, class TF1, class TF2> 
bool TC1(TF1 futimeFunc, TF2 timeFunc, QA_THREAD_PARAM)
{
    bool            retVal = false;
    UTIMBUF         filetime;
    int             fh;

    filetime.actime = filetime.modtime = timeFunc(NULL);

    if (QATCheck((fh = _topen(testfile, _O_CREAT | _O_RDONLY)) != -1))
    {
        retVal = QATCheck(futimeFunc(fh, &filetime) != 0);
        retVal &= QATCheckE(errno, EINVAL);
        _close(fh);
        QATCheck(DelFile(testfile));
    }

    return retVal;
}
template <class UTIMBUF, class STAT,
          class TF1, class TF2, class TF3, class TF4> 
bool TC2(TF1 futimeFunc, TF2 fstatFunc, TF3 timeFunc, TF4 localtimeFunc,
         QA_THREAD_PARAM)
{
    bool            retVal = false;
    STAT            file_stat;
    int             fh;
    UTIMBUF         filetime;
    long            openMode = _O_RDWR | _O_TRUNC | _O_APPEND | _O_TEXT | _O_CREAT;

    if (QATCheck((fh = _topen(testfile2, openMode)) != -1))
    {
        retVal = true;

        filetime.actime = filetime.modtime = timeFunc(NULL);

        if (QATCheck(futimeFunc(fh, &filetime) == 0))
        {
            if (QATCheck(fstatFunc(fh, &file_stat) == 0))
            {
                if (GetFileSystemType() == 0)
                {
                    //FAT - Access times for hr, min, sec are not stored
                    //      for FAT file systems. Just compare year, month,
                    //      and day for the access times.
                    struct tm*      ptmTime;
                    struct tm       tmTimeSave;
                    
                    ptmTime = localtimeFunc(&filetime.actime);
                    memcpy(&tmTimeSave, ptmTime, sizeof(struct tm));
                    ptmTime = localtimeFunc(&file_stat.st_atime);
                    
                    if (tmTimeSave.tm_mday  != ptmTime->tm_mday
                     || tmTimeSave.tm_mon   != ptmTime->tm_mon
                     || tmTimeSave.tm_year  != ptmTime->tm_year
                     || tmTimeSave.tm_wday  != ptmTime->tm_wday
                     || tmTimeSave.tm_yday  != ptmTime->tm_yday
                     || tmTimeSave.tm_isdst != ptmTime->tm_isdst)
                        QATFail(10);
                   else
                        retVal = true;
                }
                else
                {
                   //NTFS
                    if(!(filetime.actime+filetime.actime % 2 == file_stat.st_atime
                      || filetime.actime == file_stat.st_atime)) 
                        QATFail(11);
                    else
                        retVal = true;
                }
            }
        }
        _close(fh);
        QATCheck(DelFile(testfile2));
    }

    return retVal;
}

static unsigned __stdcall futime2FUTime1(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(1)
    
        QATTrace1(_T("*** futime() ***"));
        if (!TC1<struct _utimbuf>(_futime, time, QA_THREAD_PARAM_NAME))
            QATFail(10);

#if (_MSC_VER > 1200)
        QATTrace1(_T("*** futime64() ***"));
        if (!TC1<struct __utimbuf64>(_futime64, _time64, QA_THREAD_PARAM_NAME))
            QATFail(11);
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(1)
    return 0;
}

static unsigned __stdcall futime2FUTime2(void* qaThreadParam = 0)
{
    QA_BEGIN_TC(2)
    
        QATTrace1(_T("*** futime() ***"));
        if (!TC2<struct _utimbuf, 
                 struct _stat>(_futime, _fstat, time, localtime, 
                               QA_THREAD_PARAM_NAME))
            QATFail(12);

#if (_MSC_VER > 1200)
        QATTrace1(_T("*** futime64() ***"));
        if (!TC2<struct __utimbuf64, 
                 struct __stat64>(_futime64, _fstat64, _time64, _localtime64,
                                  QA_THREAD_PARAM_NAME))
            QATFail(13);
#endif  /* (_MSC_VER > 1200) */
    
    QA_END_TC(2)
    return 0;
}

TestFunc futime2TestFuncList[] =
{
    futime2FUTime1,
    futime2FUTime2
};


int futime2Entry()
{
    int nTests = (sizeof(futime2TestFuncList)/sizeof(futime2TestFuncList[0]));
    int iTest = 0;
    bool bRunTerm = false;

    if (futime2TestFuncList[0] == 0)
    {
        if (nTests == 1) return 3; /* error, not enough data */
        /* if the 0th element is zero, then 1st element is a */
        /* thread kickoff function */
        iTest = 2;
        if (futime2TestFuncList[1] != 0)
            futime2TestFuncList[1](0);
    }

    if (nTests >= 4)
    {
        if (futime2TestFuncList[2] == 0)
        {
            bRunTerm = true;
            iTest = 4;
        }
    }

    for (; iTest < nTests; iTest++)
        if(futime2TestFuncList[iTest] != 0)
            futime2TestFuncList[iTest](0);
    if (bRunTerm)
    {
        if (!futime2TestFuncList[3])
            DbgPrint("Term function NULL!\n");
        else
            futime2TestFuncList[3](0);
    }
    int ret = GetTestFailure() ? 1 : 0;
    ReportTest(false);
    ClearFailures();
    return ret;
}

