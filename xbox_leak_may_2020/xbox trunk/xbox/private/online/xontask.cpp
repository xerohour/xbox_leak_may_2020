/*++

Copyright (c) Microsoft Corporation

Description:
    Module implementing Xbox Online Task pump APIs

Module Name:

    xontask.c

--*/

#include "xonp.h"
#include "xonver.h"

//
// DEBUG ONLY: Define the valid signature XONT
//
#if DBG
#define XONLINETASK_SIGNATURE   ((DWORD)('TNOX'))
#endif

//
// Helper function to initialize the task context
//
VOID CXo::TaskInitializeContext(PXONLINETASK_CONTEXT pContext)
{
	Assert(pContext != NULL);

	// Clear eveyrhting
	ZeroMemory(pContext, sizeof(XONLINETASK_CONTEXT));

#if DBG 
	pContext->dwSignature = XONLINETASK_SIGNATURE;
	pContext->dwThreadId = 0;
#endif
}

//
// Helper function to verify a task context
//
void CXo::TaskVerifyContext(PXONLINETASK_CONTEXT pContext)
{
    Assert(pContext != NULL);
    Assert(pContext->dwSignature == XONLINETASK_SIGNATURE);
}

//
// Continuation function to execute the task associated with the handle
//
HRESULT CXo::XOnlineTaskContinue(XONLINETASK_HANDLE hTask)
{
    XoEnter("XOnlineTaskContinue");
    XoCheck(hTask != NULL);

	HRESULT hr;
	PXONLINETASK_CONTEXT pContext = (PXONLINETASK_CONTEXT)hTask;

#if DBG 
	TaskVerifyContext(pContext);

    DWORD dwThreadID = GetCurrentThreadId();
    DWORD dwPrevThreadID;
    dwPrevThreadID = InterlockedCompareExchange(
                (PLONG)&pContext->dwThreadId,
                dwThreadID,
                0);

    AssertSz(dwPrevThreadID != dwThreadID, "Recursively calling the same task");
    AssertSz(dwPrevThreadID == 0, "Another thread is working on this task");
#endif

    hr = (this->*(pContext->pfnContinue))(hTask);

#if DBG
    pContext->dwThreadId = 0;
#endif

    return(XoLeave(hr));
}

//
// Function to release a task handle, and all its task-allocated resources
//
HRESULT CXo::XOnlineTaskClose(XONLINETASK_HANDLE hTask)
{
    XoEnter("XOnlineTaskClose");
    XoCheck(hTask != NULL);

	PXONLINETASK_CONTEXT pContext = (PXONLINETASK_CONTEXT)hTask;

#if DBG 
	TaskVerifyContext(pContext);

    AssertSz(pContext->dwThreadId == 0, "Trying to close a task that another thread is still working on");
#endif

    (this->*(pContext->pfnClose))(hTask);

    return(XoLeave(S_OK));
}


