///////////////////////////////////////////////////////////////////////////////
//	cspecial.H
//
//	Created by :			Date :
//		YefimS					01/06/94
//
//	Description :
//		Declaration of the Cstack2IDETest class
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CSPECIAL_H__
#define __CSPECIAL_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "..\..\dbgtestbase.h"

///////////////////////////////////////////////////////////////////////////////
//	Cstack2IDETest class

class Cstack_specialIDETest : public CDbgTestBase
{
	DECLARE_TEST(Cstack_specialIDETest, CstackSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:
	BOOL MainSequence(   );
	BOOL CheckParamList(   );
	BOOL VarParam(   );
	BOOL Recursion(   );
	BOOL Options(   );
	BOOL AdvancedParam(   );

// Utilities
protected:
	BOOL ShortRecCycle(int nParam);
	BOOL EndOfStackIsCorrect(int level);
	BOOL StackIsCorrect(int nParam, LPCSTR strLine);
};

#endif //__CSPECIAL_H__
