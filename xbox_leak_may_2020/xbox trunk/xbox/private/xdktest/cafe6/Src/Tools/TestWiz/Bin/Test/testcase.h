///////////////////////////////////////////////////////////////////////////////
//	testcase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the TestTestSet Class
//

#ifndef __CLWCASES_H__
#define __CLWCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
//	TestTestSet class

class TestTestSet : public CTestSet
{
public:
	TestTestSet();

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

	virtual BOOL TestCase1( );
};

#endif //__CLWCASES_H__
