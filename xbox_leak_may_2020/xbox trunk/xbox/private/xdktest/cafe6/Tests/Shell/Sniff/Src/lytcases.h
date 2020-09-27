///////////////////////////////////////////////////////////////////////////////
//	LYTCASES.H
//
//	Created by :			Date :
//		Enriquep					10/25/93
//
//	Description :
//		Declaration of the CLytTestCases class
//

#ifndef __LYTCASES_H__
#define __LYTCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CCusTestCases class

class CLytTestCases : public CTest
{
	DECLARE_TEST(CLytTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:
	CString m_strCWD;		// the test's current working directory

// Test Cases
protected:
	BOOL DesignModeLyt(void);
	BOOL DebugModeLyt(void);
};


///////////////////////////////////////////////////////////////////////////////
// CTabTestCases class

class CTabTestCases : public CTest
{
	DECLARE_TEST(CTabTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);
};

#endif //__LYTCASES_H__
