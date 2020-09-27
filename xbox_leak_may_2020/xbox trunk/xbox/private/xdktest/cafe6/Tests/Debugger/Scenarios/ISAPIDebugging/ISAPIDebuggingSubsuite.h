///////////////////////////////////////////////////////////////////////////////
//	ISAPIDebuggingSubsuite.h
//
//	Created by: MichMa		Date: 11/20/97
//
//	Description:
//		Declaration of CISAPIDebuggingSubsuite

#ifndef __ISAPIDEBUGGINGSUBSUITE_H__
#define __ISAPIDEBUGGINGSUBSUITE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CISAPIDebuggingSubsuite class

class CISAPIDebuggingSubsuite : public CIDESubSuite

{
	DECLARE_SUBSUITE(CISAPIDebuggingSubsuite)
	DECLARE_TESTLIST();

// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
};

#endif //__ISAPIDEBUGGINGSUBSUITE_H__
