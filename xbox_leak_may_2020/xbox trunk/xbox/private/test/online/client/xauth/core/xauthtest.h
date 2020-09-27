#ifndef __XAUTHTEST_H__
#define __XAUTHTEST_H__

//==================================================================================
// Includes
//==================================================================================
#include <xtl.h>
#include <xdbg.h>
#include <winsockx.h>
#include <stdio.h>
#include <xlog.h>

#include <xonline.h>
#include <xnetref.h>

namespace XAuthTestNamespace {

//==================================================================================
// Defines
//==================================================================================
//==================================================================================
// Defines
//==================================================================================
#define FAIL_TESTCASE(FailureMessage)	xLog(hLog, XLL_FAIL, FailureMessage); \
										break
#define PASS_TESTCASE(PassMessage)		xLog(hLog, XLL_PASS, PassMessage);
#define START_TESTCASE(TestCaseName)	while(TRUE) \
										{ \
											xStartVariation(hLog, TestCaseName)
#define END_TESTCASE()						break; \
										}; \
										xEndVariation(hLog)
#define CLEANUP_TASK(hTask)				if(hTask) \
										{ \
											DWORD dwCleanupPollCount = 0, dwCleanupWorkFlags = 0; \
											XOnlineTaskCancel(hTask, &dwCleanupWorkFlags); \
											while(!XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwCleanupWorkFlags)) \
											{ \
												XOnlineTaskContinue(hTask, 0, &dwCleanupWorkFlags); \
												++dwCleanupPollCount; \
											} \
											XOnlineTaskCloseHandle(hTask); \
											hTask = NULL; \
										}

//==================================================================================
// Typedefs
//==================================================================================

//==================================================================================
// Structures
//==================================================================================

//==================================================================================
// Prototypes 
//==================================================================================
// Harness functions
BOOL WINAPI XAuthTestDllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext);
VOID WINAPI XAuthTestStartTest(IN HANDLE hLog);
VOID WINAPI XAuthTestEndTest();

// Helper functions

// Local test cases

} // namespace XAuthTestNamespace

#endif // __XAuthTest_H__
