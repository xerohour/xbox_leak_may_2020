#ifndef __COMMON_H_IN_ONLINE_CLIENT__
#define __COMMON_H_IN_ONLINE_CLIENT__

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



//==================================================================================
// Defines
//==================================================================================
#define HARNESS_TITLE_ID				0xA7049955
#define SLEEPINTERVAL                   50
#define ALLOWEDLOGONINTERVAL           	30000
#define ALLOWEDPUMPINGINTERVAL          30000

#define	SY_NULL							0
#define SY_NONNULL						1

#define SY_SUCCESS 						1
#define SY_FAIL 						-1
#define SY_TIMEOUT 						-2
#define SY_STOP_WHOLE_TEST	    		-3
#define SY_FAIL_THIS_TESTCASE			-1


#define SY_EXCEPTION_STOP_WHOLE_TEST	    0xe0000001
#define SY_EXCEPTION_FAIL_THIS_TESTCASE		0xe0000002

#define SY_EXCEPTION_MEM_ALLOC_FAIL			0xe0000011
#define SY_EXCEPTION_GET_USERS_FAIL			0xe0000012
#define SY_EXCEPTION_NO_USERS				0xe0000013
#define SY_EXCEPTION_LOGON_FAIL				0xe0000014
#define SY_EXCEPTION_LOGON_PUMP_FAIL		0xe0000015
#define SY_EXCEPTION_LOGON_PUMP_TIMEOUT		0xe0000016
#define SY_EXCEPTION_LOGONTASK_RESULT_WRONG	0xe0000017



#define CLEANUP_TASK(hTask)				if(hTask) \
										{ \
											XOnlineTaskClose(hTask); \
											hTask = NULL; \
										}
#define START_TESTCASE(hTestLog,TestName)	    xStartVariation(hTestLog, TestName);
#define END_TESTCASE(HTestLog)				    xEndVariation(hTestLog);

#define PASS_TESTCASE(hTestLog,PassMsg)		    xLog(hTestLog, XLL_PASS, PassMsg);
#define FAIL_TESTCASE(hTestLog,FailMsg)		    xLog(hTestLog, XLL_FAIL, FailMsg);
//#define FAIL_TESTCASE_AND_RAISE_EXCEPT(FailMsg)		    xLog(hTestLog, XLL_FAIL, FailMsg);\
//                                        RaiseException(SY_EXCEPTION_FAIL_THIS_TESTCASE,0,0,NULL);
#define PUMP_CS_AND_EXIT_IF_FAILED(hTestLog, hLogonTask)\
	{\
		HRESULT hrPump;\
		hrPump = PumpLogonTask(hTestLog, hLogonTask, &hrPump);\
		if(FAILED(hrPump))\
			RaiseException(SY_EXCEPTION_STOP_WHOLE_TEST, 0,0, NULL);\
	}
#define RaiseExceptionIfNecessary(iResult) \
		    if((iResult) == SY_FAIL_THIS_TESTCASE || (iResult) == SY_TIMEOUT) \
				RaiseException(SY_EXCEPTION_FAIL_THIS_TESTCASE, 0,0, NULL); \
		    else if((iResult) == SY_EXCEPTION_STOP_WHOLE_TEST) \
		    	RaiseException(SY_EXCEPTION_STOP_WHOLE_TEST, 0, 0, NULL);


__forceinline void DbgBreak()
{
    _asm int 3;
}

#define Verify(x)       do { if (!(x)) DbgBreak(); } while (0)

//==================================================================================
// Functions
//==================================================================================
BOOL LogonForTest(HANDLE hTestLog, HANDLE hLogonEvent, PXONLINETASK_HANDLE phLogonTask, 
							DWORD* pServices, DWORD cServices, DWORD dwMaxWaitInterval);
LONG LogonForTestExceptionFilter(DWORD dwExceptionCode);
INT PumpTaskUntilTimeout(HANDLE hTestLog, XONLINETASK_HANDLE hTask, HANDLE hEvent, DWORD dwMaxWaitInterval, 
									DWORD *pdwPollCounter, BOOL bCloseOnComplete, HRESULT *hrReturn);
BOOL IsTimeExpired(DWORD	dwStartTime, DWORD dwAllowedInterval);
BOOL PumpLogonTask(HANDLE  hTestLog, XONLINETASK_HANDLE hLogonTask, HRESULT* phrReturn);

//==================================================================================
// Global Variables
//==================================================================================
extern HANDLE g_hTestLog;



#endif // __COMMON_H_IN_ONLINE_CLIENT__
