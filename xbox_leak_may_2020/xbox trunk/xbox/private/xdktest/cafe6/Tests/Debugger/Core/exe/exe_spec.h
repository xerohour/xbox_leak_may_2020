///////////////////////////////////////////////////////////////////////////////
//	exe_spec.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the Cexe_specialIDETest Class
//

#ifndef __exe_spec_H__
#define __exe_spec_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\dbgset.h"
#include "exesub.h"

///////////////////////////////////////////////////////////////////////////////
//	CexeIDETest class

class Cexe_specialIDETest : public CDebugTestSet
{
	DECLARE_TEST(Cexe_specialIDETest, CexeSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:

	BOOL GoExeDoesNotExist();
	BOOL ExitWhileDebugging();

	BOOL CannotFindDll();
	BOOL StdInOutRedirection();
	BOOL NoSystemCodeStepping();
	BOOL TraceIntoWndProc();

// Utilities
protected:
	BOOL CheckGeneralNetworkFailure();

};

#endif //__exe_spec_H__
