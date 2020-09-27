///////////////////////////////////////////////////////////////////////////////
//	cs_dlls.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CstackIDETest Class
//

#ifndef __cs_dlls_H__
#define __cs_dlls_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\dbgset.h"

///////////////////////////////////////////////////////////////////////////////
//	CstackIDETest class

class Cstack_dllsIDETest :public CDebugTestSet
{
	DECLARE_TEST(Cstack_dllsIDETest, CstackSubSuite)

public:  // Operations
	virtual void PreRun(void);
	virtual void Run();

	// Test Cases
protected:
	BOOL DllGeneralNavigation();
	BOOL DllStepCheckCallStack();
};

#endif //__cs_dlls_H__
