///////////////////////////////////////////////////////////////////////////////
//	expor1.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the Cexpor1Test class

#ifndef __expor1_H__
#define __expor1_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "exporsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CExportMakefileTest1 class

class CExportMakefileTest1 : public CProjectTestSet
{
	DECLARE_TEST(CExportMakefileTest1, CExportMakefileSubSuite)

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

#endif //__expor1_H__
