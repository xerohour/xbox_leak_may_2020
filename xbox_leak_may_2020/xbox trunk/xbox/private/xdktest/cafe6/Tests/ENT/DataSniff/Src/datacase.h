///////////////////////////////////////////////////////////////////////////////
//	DATACASE.H
//
//	Created by :			Date :
//		ChrisKoz					2/02/96
//
//	Description :
//		Declaration of classes for DataPane Verification
//

#ifndef __DATACASE_H__
#define __DATACASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"



///////////////////////////////////////////////////////////////////////////////
//	CDataCases class
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// CTablesTest: Check Pubs DB tables on the SQL server

class CTablesTest: public CTest
{
	DECLARE_TEST(CTablesTest, CDataSubSuite)

public:
	virtual void Run(void);

	void TestTableObjects();
	int OpenGridView(LPCSTR pszName,CString strType,CString &Params);
	int OpenDiagView(LPCSTR pszName,CString strType,CString &Params);
};



#endif //__DATACASE_H__
