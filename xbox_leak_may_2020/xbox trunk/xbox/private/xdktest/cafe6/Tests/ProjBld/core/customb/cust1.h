///////////////////////////////////////////////////////////////////////////////
//	cust1.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the Ccust1Test class

#ifndef __cust1_H__
#define __cust1_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "custsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CCustomBuildTest1 class

class CCustomBuildTest1 : public CProjectTestSet
{
	DECLARE_TEST(CCustomBuildTest1, CCustomBuildSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL BasicTest(void);
	BOOL SetCustomBuild(void);
	BOOL SetAllMacros(void);
	BOOL GetProjReady(void);

// Utilities

};

#endif //__cust1_H__
