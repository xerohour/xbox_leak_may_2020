///////////////////////////////////////////////////////////////////////////////
//	CLWSNAP.H
//
//	Created by :			Date :
//		Ivanl				4/5/94
//
//	Description :
//		Declaration of the Classwizard cases
//

#ifndef __CLWCASES_H__
#define __CLWCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	ClassWizardTestCases class

class ClassWizardTestCases : public CIDETest
{
	DECLARE_TEST(ClassWizardTestCases, CSniffDriver);

// Attributes
public:
// Operations
public:
	virtual int Run(void);

// Data
protected:

// Test Cases
protected:
	void ClassWizard( );
	void ControlWizard();
};

#endif //__CLWCASES_H__
