///////////////////////////////////////////////////////////////////////////////
//	conv1.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the CConversionTest1 class

#ifndef __conv1_H__
#define __conv1_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "convsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CConversionTest1 class

class CConversionTest1 : public CProjectTestSet
{
	DECLARE_TEST(CConversionTest1, CConversionSubSuite)

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

#endif //__conv1_H__
