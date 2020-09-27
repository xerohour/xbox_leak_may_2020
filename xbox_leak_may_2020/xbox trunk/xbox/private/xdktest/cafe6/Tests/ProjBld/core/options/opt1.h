///////////////////////////////////////////////////////////////////////////////
//	opt1.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the Copt1Test class

#ifndef __opt1_H__
#define __opt1_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "optsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CBuildOptionsTest1 class

class CBuildOptionsTest1 : public CProjectTestSet
{
	DECLARE_TEST(CBuildOptionsTest1, CBuildOptionsSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL Test1();

// Utilities

};

#endif //__opt1_H__
