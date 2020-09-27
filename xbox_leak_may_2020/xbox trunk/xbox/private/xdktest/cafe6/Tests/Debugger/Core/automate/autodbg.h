///////////////////////////////////////////////////////////////////////////////
//	autodbg.h
//
//	Created by : MichMa		Date: 9-17-96			
//		VCBU QA
//
//	Description :
//		Declaration of the CAutomationIDETest Class
//

#ifndef __AUTODBG_H__
#define __AUTODBG_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\dbgset.h"
#include "autosub.h"

///////////////////////////////////////////////////////////////////////////////
//	CDebuggerAutomationIDETest class

class CDebuggerAutomationIDETest : public CDebugTestSet
	{
		DECLARE_TEST(CDebuggerAutomationIDETest, CAutomationSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__AUTODBG_H__
