///////////////////////////////////////////////////////////////////////////////
//	bldatl.H
//
//	Created by :			Date :
//		YefimS					5/12/97
//
//	Description :
//		Declaration of the CBuildATLTest class

#ifndef __bldatl_H__
#define __bldatl_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "buildsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CBuildATLTest class

class CBuildATLTest : public CProjectTestSet
{
	DECLARE_TEST(CBuildATLTest, CBuildSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL BuildATLCOMAppWizardDll();
	BOOL BuildATLCOMAppWizardExe();
	BOOL BuildATLCOMAppWizardService();

// Utilities

};

#endif //__bldatl_H__
