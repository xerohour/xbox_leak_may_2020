///////////////////////////////////////////////////////////////////////////////
//	autorec.h
//
//	Created by : MichMa		Date: 9-17-96			
//		VCBU QA
//
//	Description :
//		Declaration of the CRecordingAutomationIDETest Class
//

#ifndef __AUTOREC_H__
#define __AUTOREC_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\dbgset.h"
#include "autosub.h"

///////////////////////////////////////////////////////////////////////////////
//	CRecordingAutomationIDETest class

class CRecordingAutomationIDETest : public CDebugTestSet
	{
		DECLARE_TEST(CRecordingAutomationIDETest, CAutomationSubSuite)

	// Operations
	public:
		virtual void PreRun(void);
		virtual void Run(void);
	};

#endif //__AUTOREC_H__
