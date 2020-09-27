///////////////////////////////////////////////////////////////////////////////
//	INTCASES.H
//
//	Created by :			Date :
//		DougT					8/13/93
//
//	Description :
//		Declaration of the CIntTestCases class
//

#ifndef __INTCASES_H__
#define __INTCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CIntTestCases class

class CIntTestCases : public CTest
{
	DECLARE_TEST(CIntTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Test Cases
protected:
	BOOL IntOpenErrorLadenRC(void);
};

#endif //__INTCASES_H__
