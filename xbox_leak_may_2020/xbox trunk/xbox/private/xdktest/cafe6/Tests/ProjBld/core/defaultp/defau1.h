///////////////////////////////////////////////////////////////////////////////
//	defau1.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the Cdefau1Test class

#ifndef __defau1_H__
#define __defau1_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "defausub.h"

///////////////////////////////////////////////////////////////////////////////
//	CDefaultProjectTest1 class

class CDefaultProjectTest1 : public CProjectTestSet
{
	DECLARE_TEST(CDefaultProjectTest1, CDefaultPrjSubSuite)

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

#endif //__defau1_H__
