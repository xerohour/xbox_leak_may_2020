//
// QA added file to redirect CRT dbg output to stdout.
//
#if !defined(__LOGDBG_H__)
#define __LOGDBG_H__

#if defined(_DEBUG) && !defined(QA_NO_DBGCRT_HOOK)
#include <crtdbg.h>
#include <stdlib.h>
#include <stdio.h>

static int oldRetVal = 0;
static int reportHookCalled = 0; /* 1 if report hook called */
static int dbgCrtCont = 0;       /* 0 == exit, 1 = continue program. */

static void logDbgCrtExit(void)
{
  exit(99);
}

static int logDbgCrtReport(int nRptType, char *szMsg, int* retval)
{
  oldRetVal = 0;
  reportHookCalled = 1;
  if (dbgCrtCont)
    return 1;
  printf("\nDebug CRT Report: %s", szMsg);
  printf("%s: ----- FAILED -----\n", __FILE__);
  logDbgCrtExit();
  return 0;    /* never gets called, but compiler complains if not here.*/
}

#define InitDbgLog()  _CrtSetReportHook((_CRT_REPORT_HOOK)logDbgCrtReport)

#else

#define InitDbgLog()

#endif

#endif // __LOGDBG_H__
