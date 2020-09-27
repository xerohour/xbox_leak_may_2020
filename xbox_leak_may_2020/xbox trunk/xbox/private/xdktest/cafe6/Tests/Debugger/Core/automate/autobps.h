///////////////////////////////////////////////////////////////////////////////
//	autobps.h
//
//	Created by : MichMa		Date: 9-17-96			
//		VCBU QA
//
//	Description :
//		Declaration of the CBreakpointsAutomationIDETest Class
//

#ifndef __AUTOBPS_H__
#define __AUTOBPS_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\dbgset.h"
#include "autosub.h"

///////////////////////////////////////////////////////////////////////////////
//	CBreakpointsAutomationIDETest class

class CBreakpointsAutomationIDETest : public CDebugTestSet
	{
		DECLARE_TEST(CBreakpointsAutomationIDETest, CAutomationSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__AUTOBPS_H__
