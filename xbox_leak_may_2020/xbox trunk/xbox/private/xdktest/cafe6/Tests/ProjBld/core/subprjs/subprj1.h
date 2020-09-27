///////////////////////////////////////////////////////////////////////////////
//	subprj1.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the Csubprj1Test class

#ifndef __subprj1_H__
#define __subprj1_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "sprjsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CSubprojectsTest1 class

class CSubprojectsTest1 : public CProjectTestSet
{
	DECLARE_TEST(CSubprojectsTest1, CSubprojectsSubSuite)

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

#endif //__subprj1_H__
