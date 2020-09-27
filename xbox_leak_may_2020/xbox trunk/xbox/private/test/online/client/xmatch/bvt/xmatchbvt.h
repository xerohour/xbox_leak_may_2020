#ifndef __XMATCHBVT_H__
#define __XMATCHBVT_H__

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
#include "OnlineAccounts.h"

namespace XMatchBVTNamespace {

//==================================================================================
// Defines
//==================================================================================
#define ONFIX	1
#define SY_NEED_POPULATE_USERS	1

#define INT1 111222333444555666
#define STRING1 L"AAAAABBBBB"
#define BLOB1 "ZZZZZYYYYY"
#define INT2 0
#define STRING2 L"ccc"
#define BLOB2 "www"

#define GETSESSSP_INDEX 0x0000000A
#define GETSESSINT_INDEX 0x0000000B

#define GLOBALINT_ID1 X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000002
#define GLOBALSTR_ID1 X_ATTRIBUTE_DATATYPE_STRING | 0x00000001
#define GLOBALBLB_ID1 X_ATTRIBUTE_DATATYPE_BLOB | 0x00000001
#define GLOBALINT_ID2 X_ATTRIBUTE_DATATYPE_INTEGER | 0x00000003
#define GLOBALSTR_ID2 X_ATTRIBUTE_DATATYPE_STRING | 0x00000004
#define GLOBALBLB_ID2 X_ATTRIBUTE_DATATYPE_BLOB | 0x00000004

#define PUMP_CS_EVENT()					{\
											DWORD dwCurrentTime = GetTickCount();\
											hrDoWork2 = XOnlineTaskContinue(hLogon); \
											if(FAILED(hrDoWork2)) \
											{ \
												xLog(hLog, XLL_FAIL, "Lost connection with CS, exiting test!"); \
												goto Exit; \
											}\
										}
#define FAIL_TESTCASE(FailureMessage)	xLog(hLog, XLL_FAIL, FailureMessage); \
										break
#define PASS_TESTCASE(PassMessage)		xLog(hLog, XLL_PASS, PassMessage);
#define START_TESTCASE(TestCaseName)	while(TRUE) \
										{ \
											xStartVariation(hLog, TestCaseName)
#define END_TESTCASE()						break; \
										}; \
										PUMP_CS_EVENT()\
										xEndVariation(hLog)
#define CLEANUP_TASK(hTask)				if(hTask) \
										{ \
											XOnlineTaskClose(hTask); \
											hTask = NULL; \
										}

__forceinline void DbgBreak()
{
    _asm int 3;
}

#define Verify(x)       do { if (!(x)) DbgBreak(); } while (0)

//==================================================================================
// Prototypes 
//==================================================================================
// Harness functions
BOOL WINAPI XMatchBVTDllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext);
VOID WINAPI XMatchBVTStartTest(IN HANDLE hLog);
VOID WINAPI XMatchBVTEndTest();

} // namespace XMatchBVTNamespace

#endif // __XMatchBVT_H__
