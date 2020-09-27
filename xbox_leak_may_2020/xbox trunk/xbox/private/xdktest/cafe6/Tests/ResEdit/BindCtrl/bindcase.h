///////////////////////////////////////////////////////////////////////////////
//	DATACASE.H
//
//	Created by :			Date :
//		ChrisKoz					2/02/96
//
//	Description :
//		Declaration of classes for DataPane Verification
//

#ifndef __BINDCASE_H__
#define __BINDCASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"



///////////////////////////////////////////////////////////////////////////////
//	CDataCases class
///////////////////////////////////////////////////////////////////////////////
// CBindBuildTest: Connect to the SQL server Data Source

class CBindBuildTest: public CTest
{
	DECLARE_TEST(CBindBuildTest, CBindSubSuite)

public:
	virtual void Run(void);
	virtual	BOOL RunAsDependent(void);
	BOOL OpenProject(void);
	BOOL BuildProject(void);
	void RunTestMode(void);
};

///////////////////////////////////////////////////////////////////////////////
// CPubsTablesTest: Check Pubs DB tables on the SQL server

class CPubsTablesTest: public CTest
{
	DECLARE_TEST(CPubsTablesTest, CBindSubSuite)

public:
	virtual void Run(void);
	int SetSource(LPCSTR RdcID,LPCSTR szConnect,LPCSTR szUser,LPCSTR szPWD,
		LPCSTR TableName);
};



#endif //__BINDCASE_H__
