///////////////////////////////////////////////////////////////////////////////
//	WCHCASES.H
//
//	Created by :			Date :
//		MichMa					10/19/94
//
//	Description :
//		Declaration of the CWatchCases class
//

#ifndef __WCHCASES_H__
#define __WCHCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "dbgset.h"

///////////////////////////////////////////////////////////////////////////////
//	CWatchCases class

class CWatchCases : public CDebugTestSet
	{
		DECLARE_TEST(CWatchCases, CCORESubSuite)

	// Data
	private:
		COFile file;
		CODebug dbg;
		COBreakpoints bp;
		COStack stack;
		UIMemory mem;
		UIWatch watch;
		UIVariables var;

	// Operations (test cases)
	public:
		virtual void PreRun(void);
		virtual void Run();
	};

#endif //__WCHCASES_H__
