///////////////////////////////////////////////////////////////////////////////
//	autobp.h
//
//	Created by : MichMa		Date: 9-17-96			
//		VCBU QA
//
//	Description :
//		Declaration of the CBreakpointAutomationIDETest Class
//

#ifndef __AUTOBP_H__
#define __AUTOBP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\dbgset.h"
#include "autosub.h"

///////////////////////////////////////////////////////////////////////////////
//	CBreakpointAutomationIDETest class

class CBreakpointAutomationIDETest : public CDebugTestSet
	{
		DECLARE_TEST(CBreakpointAutomationIDETest, CAutomationSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__AUTOBP_H__
