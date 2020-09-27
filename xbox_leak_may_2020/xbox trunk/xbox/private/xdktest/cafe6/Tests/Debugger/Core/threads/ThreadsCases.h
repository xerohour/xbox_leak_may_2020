///////////////////////////////////////////////////////////////////////////////
//	ThreadsCases.h
//
//	Created by:			Date:
//		MichMa				2/5/98
//
//	Description :
//		Declaration of the CThreadsCases Class
//

#ifndef __THREADSCASES_H__
#define __THREADSCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\dbgset.h"
#include "ThreadsSubsuite.h"


///////////////////////////////////////////////////////////////////////////////
//	CThreadsCases class

class CThreadsCases : public CDebugTestSet

{
	DECLARE_TEST(CThreadsCases, CThreadsSubsuite)

private:
	// Data
	int m_iDefaultThreadPri;

public:

	// Operations
	virtual void PreRun(void);
	virtual void Run(void);

	// Utils
	void InitTestState(void);
	LPCSTR GetThreadIDStr(LPCSTR szThreadIDVar);

	// Test Cases
	void VerifyThreadInfoForMultipleThreads(void);
	void ChangeFocusToDifferentThreadAndBack(void);
	void RunMultithreadedAppToTermination(void);
	void HitLocationBPInThreadSetBeforeThreadCreated(void);
	void HitLocationBPInThreadSetAfterThreadCreated(void);
	void HitGlobalDataBPInThreadSetWhileFocusedOnAnotherThread(void);
	void HitGlobalDataBPInThreadSetWhileFocusedOnSameThread(void);
	void VerifyMemoryDumpOfLocalFromSecondaryThread(void);
	void NavigateStackInSecondaryThread(void);
	void StopDebuggingFromSecondaryThread(void);
	void SuspendAndResumeThread(void);
	void SetFocusToThreadAndStep(void);
	void GoWhenThreadWithFocusIsSuspended(void);
};

#endif //__THREADSCASES_H__
