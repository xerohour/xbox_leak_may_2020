///////////////////////////////////////////////////////////////////////////////
//	NLGcase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CNLGIDETest Class
//

#ifndef __NLGcase2_H__
#define __NLGcase2_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "nlgsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CNLGIDETest class

class CNLG2IDETest : public CDbgTestBase
{
	DECLARE_TEST(CNLG2IDETest, CNLGSubSuite)

// Attributes
public:

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

	BOOL IsUnhandledException(CString except_text);

// Test Cases
protected:
	// EH
	BOOL EH_StepIntoThrow( );
	BOOL EH_StepIntoReThrow( );
	BOOL EH_CatchInDLL( );
	BOOL EH_CatchFromDLL( );
	BOOL EH_ReThrowInCtor( );

	// SEH
	BOOL SEH_ExceptionContinueSearch( );
	BOOL SEH_StepIntoRaiseException( );

	// Setjmp/Longjmp
	BOOL LJMP_StepIntoLongJmp( );

// utilities

};

#endif //__NLGcase_H__
