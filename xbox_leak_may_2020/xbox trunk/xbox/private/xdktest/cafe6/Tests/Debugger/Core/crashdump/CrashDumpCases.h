///////////////////////////////////////////////////////////////////////////////
//	CrashDumpCases.h
//
//	Created by: MichMa		Date: 9/30/97
//
//	Description :
//		Declaration of the CCrashDumpCases class
//

#ifndef __CRASHDUMPCASES_H__
#define __CRASHDUMPCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\dbgset.h"
#include "CrashDumpSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CCrashDumpCases class

class CCrashDumpCases : public CDebugTestSet

{
	DECLARE_TEST(CCrashDumpCases, CCrashDumpSubsuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);

	// Test cases
	private:
};

#endif //__CRASHDUMPCASES_H__
