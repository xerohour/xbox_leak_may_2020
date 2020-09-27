///////////////////////////////////////////////////////////////////////////////
//	cs_base.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the Cstack_baseIDETest Class
//

#ifndef __cs_base_H__
#define __cs_base_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"

///////////////////////////////////////////////////////////////////////////////
//	CstackIDETest class

class Cstack_baseIDETest :public CDbgTestBase
{
	DECLARE_TEST(Cstack_baseIDETest, CstackSubSuite)

public:  // Operations
	virtual void PreRun(void);
	virtual void Run();

	// Test Cases
protected:
	void RebuildProject(LPCSTR projName,LPCSTR pConfig);
	BOOL GeneralNavigation();
	BOOL ChainDisplay();
	BOOL SteppingAndCallStackUpdating();
	BOOL CheckLocals();
	BOOL NavigationAndModifiedLocalGlobal();
};

#endif //__cs_base_H__
