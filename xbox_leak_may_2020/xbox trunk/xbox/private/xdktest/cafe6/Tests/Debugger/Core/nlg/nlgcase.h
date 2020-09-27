///////////////////////////////////////////////////////////////////////////////
//	NLGcase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CNLGIDETest Class
//

#ifndef __NLGcase_H__
#define __NLGcase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\dbgset.h"
#include "nlgsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CNLGIDETest class

class CNLGIDETest : public CDebugTestSet
{
	DECLARE_TEST(CNLGIDETest, CNLGSubSuite)

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
	BOOL EH_StepOverThrow( );
	BOOL EH_TryNoThrow( );
	BOOL EH_StepOverTry_StepOverCatch( );
	BOOL EH_StepOverReThrow( );
	BOOL EH_BreakInCatch( );
	BOOL EH_StepOverThrowFunc( );
	BOOL EH_StepOutThrowFunc( );
	BOOL EH_StepToCursorOverThrowIntoCatch( );
	BOOL EH_BreakInCtor( );
	BOOL EH_BreakInDtor( );

	// SEH
	BOOL SEH_TryNoException( );
	BOOL SEH_ExceptionContinueExecution( );
	BOOL SEH_ExceptionExecuteHandler( );
	BOOL SEH_TryFinallyNoException( );
	BOOL SEH_TryFinallyException( );
	BOOL SEH_BreakInFilter( );
	BOOL SEH_StepOver__try( );
	BOOL SEH_BreakInExcept( );
	BOOL SEH_BreakInFinally( );
	BOOL SEH_StepToCursorOver__tryIntoExcept( );
	BOOL SEH_StepToCursorOver__tryIntoFinally( );

	// Setjmp/Longjmp
	BOOL LJMP_StepOverLongJmp( );
	BOOL LJMP_StepOverFuncWithLongJmp( );
	BOOL LJMP_StepOutFuncWithLongJmp( );

// utilities

};

#endif //__NLGcase_H__
