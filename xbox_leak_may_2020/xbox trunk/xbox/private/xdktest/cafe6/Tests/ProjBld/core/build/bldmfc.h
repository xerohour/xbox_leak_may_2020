///////////////////////////////////////////////////////////////////////////////
//	bldmfc.H
//
//	Created by :			Date :
//		YefimS					5/12/97
//
//	Description :
//		Declaration of the CBuildMFCTest class

#ifndef __bldmfc_H__
#define __bldmfc_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\prjset.h"
#include "buildsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CBuildMFCTest class

class CBuildMFCTest : public CProjectTestSet
{
	DECLARE_TEST(CBuildMFCTest, CBuildSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL BuildMFCAppWizardExe();
	BOOL BuildMFCAppWizardDll();
	BOOL BuildMFCActiveXControlWizard();
	BOOL BuildMFCAppWizardExeCSHelp();
	BOOL BuildMFCAppWizardExeStaticMFC();

// Utilities

};

#endif //__bldmfc_H__
