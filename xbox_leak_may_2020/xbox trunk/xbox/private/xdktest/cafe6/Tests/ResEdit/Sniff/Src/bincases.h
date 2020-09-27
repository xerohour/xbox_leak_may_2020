///////////////////////////////////////////////////////////////////////////////
//	BINCASES.H
//
//	Created by :			Date :
//		ChrisSh					8/13/93
//
//	Description :
//		Declaration of the CBinTestCases class
//

#ifndef __BINCASES_H__
#define __BINCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CBinTestCases class

class CBinTestCases : public CTest
{
	DECLARE_TEST(CBinTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Test Cases
protected:
	BOOL CreateBinaryEditor(void);
	BOOL EnterBinaryData(void);
	BOOL SaveOpen(void);
};

#endif //__BINCASES_H__
