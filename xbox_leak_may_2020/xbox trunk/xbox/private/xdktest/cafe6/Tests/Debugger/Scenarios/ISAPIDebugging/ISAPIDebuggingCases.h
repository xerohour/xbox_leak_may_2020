///////////////////////////////////////////////////////////////////////////////
//	ISAPIDebuggingCases.h
//
//	Created by: MichMa		Date: 11/20/97
//
//	Description:
//		Declaration of CISAPIDebuggingCases

#ifndef __ISAPIDEBUGGINGCASES_H__
#define __ISAPIDEBUGGINGCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ISAPIDebuggingSubsuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CISAPIDebuggingCases class

class CISAPIDebuggingCases : public CTest

{
	DECLARE_TEST(CISAPIDebuggingCases, CISAPIDebuggingSubsuite)

public:
	// Operations
	virtual void Run(void);
};

#endif //__ISAPIDEBUGGINGCASES_H__
