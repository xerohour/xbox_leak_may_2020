///////////////////////////////////////////////////////////////////////////////
//	LFNCASES.H
//
//	Created by :			Date :
//		MikePie					3/29/95
//
//	Description :
//		Declaration of the CTBTestCases class
//

#ifndef __LFNCASES_H__
#define __LFNCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CLfnTestCases class

class CLfnTestCases : public CTest
{
	DECLARE_TEST(CLfnTestCases, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:

// Test Cases
protected:
	BOOL DetectLanguage(void);
	BOOL DeleteOldFile(void);
	BOOL CreateLongFilename(void);  
	BOOL OpenLongFilename(void);  
	BOOL DeleteNewFile(void);
};

#endif //__LFNCASES_H__
