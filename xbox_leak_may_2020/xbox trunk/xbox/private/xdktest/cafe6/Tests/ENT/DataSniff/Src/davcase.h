///////////////////////////////////////////////////////////////////////////////
//	DAVCASES.H
//
//	Created by :			Date :
//              ChrisKoz                                  9/9/97
//
//	Description :
//		Declaration of the Davinci Designers tools test cases
//

#ifndef __DAVCASES_H__
#define __DAVCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CDavinciTest

class CDavinciTest: public CTest
{
	DECLARE_TEST(CDavinciTest, CDataSubSuite)

public:
	virtual void Run(void);
	virtual	BOOL RunAsDependent(void);

	void DataBaseDesignerTest(LPCSTR szTable);
	void ViewDesignerTest(LPCSTR szTable,int nrows,BOOL bIsView);
//	void QueryDesignerTest(LPCSTR szTable,int nrows);

//utility functions
private:
	LPCSTR GetDavinciPathFromReg(void);
};



#endif // DAVCASES_H__
