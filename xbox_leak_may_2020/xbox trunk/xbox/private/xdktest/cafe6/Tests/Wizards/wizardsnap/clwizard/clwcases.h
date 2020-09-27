///////////////////////////////////////////////////////////////////////////////
//	XXXCASES.H
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

///////////////////////////////////////////////////////////////////////////////
//	XxxTestCases class

class ClassWizardTestCases : public CTestSet
{
public:
	ClassWizardTestCases();

// Attributes
public:
	DECLARE_GET_NAME();
	DECLARE_GET_NUM_TESTS();

// Operations
public:
	virtual int Run(void* pData = NULL);

// Data
protected:

// Test Cases
protected:
	BOOL ClassWizard( PDESCH );
	BOOL ControlWizard( PDESCH );
};

#endif //__CLWCASES_H__
