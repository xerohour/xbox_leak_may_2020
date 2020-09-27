#ifndef __XAUTHBVT_H__
#define __XAUTHBVT_H__

//==================================================================================
// Includes
//==================================================================================
#include <xtl.h>
#include <winsockx.h>
#include <stdio.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xonlinep.h>
#include <xnetref.h>

namespace XAuthBVTNamespace {

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

//==================================================================================
// Prototypes 
//==================================================================================
// Harness functions
BOOL WINAPI XAuthBVTDllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext);
VOID WINAPI XAuthBVTStartTest(IN HANDLE hLog);
VOID WINAPI XAuthBVTEndTest();

} // namespace XAuthBVTNamespace

#endif // __XAuthBVT_H__
