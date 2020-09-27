///////////////////////////////////////////////////////////////////////////////
//	exe_dlls.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the Cexe_dllsIDETest Class
//

#ifndef __exe_dlls_H__
#define __exe_dlls_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "exesub.h"

///////////////////////////////////////////////////////////////////////////////
//	CexeIDETest class

class Cexe_dllsIDETest : public CDbgTestBase
{
	DECLARE_TEST(Cexe_dllsIDETest, CexeSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:

	BOOL DllStepTrace();
	BOOL DllStepOut();
	BOOL DllTraceOut();
	BOOL StepOverFuncInUnloadedDll();
	BOOL RestartFromWithinDll();
	BOOL GoFromWithinDll();

};

#endif //__exe_dlls_H__
