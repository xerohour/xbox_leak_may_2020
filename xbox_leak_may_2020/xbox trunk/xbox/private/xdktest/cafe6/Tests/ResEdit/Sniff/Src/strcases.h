///////////////////////////////////////////////////////////////////////////////
//	STRCASES.H
//
//	Created by :			Date :
//		ChrisSh					8/13/93
//
//	Description :
//		Declaration of the CStrTestCases class
//

#ifndef __STRCASES_H__
#define __STRCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CStrTestCases class

class CStrTestCases : public CTest
{
	DECLARE_TEST(CStrTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:

// Test Cases
protected:
	BOOL CreateStringTable(void);
	BOOL NewString(void);
	BOOL SaveOpen(void);
};

#endif //__STRCASES_H__
