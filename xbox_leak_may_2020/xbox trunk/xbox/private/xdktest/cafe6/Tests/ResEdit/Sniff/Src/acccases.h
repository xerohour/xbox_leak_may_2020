///////////////////////////////////////////////////////////////////////////////
//	ACCCASES.H
//
//	Created by :			Date :
//		ChrisSh					8/13/93
//
//	Description :
//		Declaration of the CAccTestCases class
//

#ifndef __ACCCASES_H__
#define __ACCCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CAccTestCases class

class CAccTestCases : public CTest
{
	DECLARE_TEST(CAccTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:

// Test Cases
protected:
 	BOOL CreateAccelTable(void);
	BOOL NewAccel(void);
	BOOL SaveOpen(void);
};

#endif //__ACCCASES_H__
