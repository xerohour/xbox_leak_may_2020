///////////////////////////////////////////////////////////////////////////////
//	extern1.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the Cextern1Test class

#ifndef __extern1_H__
#define __extern1_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "extersub.h"

///////////////////////////////////////////////////////////////////////////////
//	CExternalMakefileTest1 class

class CExternalMakefileTest1 : public CProjectTestSet
{
	DECLARE_TEST(CExternalMakefileTest1, CExternalMakefileSubSuite)

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

#endif //__extern1_H__
