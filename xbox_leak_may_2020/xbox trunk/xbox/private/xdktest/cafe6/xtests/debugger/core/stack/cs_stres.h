///////////////////////////////////////////////////////////////////////////////
//	cs_stres.H
//
//	Created by :			Date :
//		YefimS				01/06/94
//
//	Description :
//		Declaration of the Cstack_stressIDETest class
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __cs_stres_H__
#define __cs_stres_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "..\..\dbgtestbase.h"

///////////////////////////////////////////////////////////////////////////////
//	Cstack_stressIDETest class

class Cstack_stressIDETest : public CDbgTestBase
{
	DECLARE_TEST(Cstack_stressIDETest, CstackSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:
	BOOL CheckLocals(   );
	BOOL Recursion(   );

// Utilities
protected:
	void VerifyStack(int nParam, LPCSTR strLine);
	void VerifyEndOfStack(int level);
	void RecCycle(int nParam);
	BOOL ShortRecCycle(int nParam);
	BOOL EndOfStackIsCorrect(int level);
	BOOL StackIsCorrect(int nParam, LPCSTR strLine);
};

#endif //__cs_stres_H__
