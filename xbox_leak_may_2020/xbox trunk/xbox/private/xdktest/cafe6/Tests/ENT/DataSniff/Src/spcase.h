///////////////////////////////////////////////////////////////////////////////
//	SRCCASES.H
//
//	Created by :			Date :
//		DavidGa					4/11/94
//
//	Description :
//		Declaration of the CSrcTestCases class
//

#ifndef __SRCCASES_H__
#define __SRCCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
//	CSourceTestCases class

class CSPTest : public CTest
{
	DECLARE_TEST(CSPTest, CDataSubSuite)

// Data
public:

// Operations
public:
	virtual void Run(void);

// Test Cases
	void TestSPTrigObjects();
	int OpenSourceView(LPCSTR pszName,CString strType,CDataInfo *DataInfo);
public:
};

#endif //__SRCCASES_H__
