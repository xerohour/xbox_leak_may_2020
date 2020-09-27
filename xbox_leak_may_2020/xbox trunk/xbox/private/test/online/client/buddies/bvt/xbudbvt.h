#ifndef __XBUDBVT_H__
#define __XBUDBVT_H__

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

namespace XBudBVTNamespace {

//==================================================================================
// Defines
//==================================================================================
#define ONFIX	1
#define SY_NEED_POPULATE_USERS	1
#define SY_NEED_CREATE_USERS	0
#define	SY_BLOCKING_IMPLEMENTED	0

#define	SY_RESULT			int
#define SY_SUCCESS			1
#define SY_STOPTESTCASE		0
#define SY_STOPWHOLETEST	-1


#define FRIEND_HANDLE_PUMP_COUNT         25
#define HARNESS_TITLE_ID				0xA7049955
#define	TIMELIMIT	                    100
#define SLEEPTIME                       50
#define ALLOWEDPUMPINGTIME              30000

#define CLEANUP_TASK(hTask)				if(hTask) \
										{ \
											XOnlineTaskClose(hTask); \
											hTask = NULL; \
										}
#define START_TESTCASE(TestName)	    xStartVariation(hLog, TestName);
#define END_TESTCASE				    PUMP_CS_AND_EXIT_IF_FAILED\
									    xEndVariation(hLog);

#define PASS_TESTCASE(PassMsg)		    xLog(hLog, XLL_PASS, PassMsg);
#define FAIL_TESTCASE(FailMsg)		    xLog(hLog, XLL_FAIL, FailMsg);
#define PUMP_CS_AND_EXIT_IF_FAILED		nResult = PumpLogonTask(hLog, hLogonTask, &hReturn);\
                                		if(SY_STOPWHOLETEST == nResult)\
                                		{\
                                			xEndVariation(hLog);\
                                			goto Exit;\
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
BOOL WINAPI XBudBVTDllMain(IN HINSTANCE hInstance, IN DWORD dwReason, IN LPVOID lpContext);
VOID WINAPI XBudBVTStartTest(IN HANDLE hLog);
VOID WINAPI XBudBVTEndTest();

} // namespace XBudBVTNamespace

#endif // __XBudBVT_H__
