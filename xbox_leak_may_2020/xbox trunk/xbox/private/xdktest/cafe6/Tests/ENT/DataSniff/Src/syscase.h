///////////////////////////////////////////////////////////////////////////////
//	SYSCASES.H
//
//	Created by :			Date :
//		RickKr					1/25/94
//
//	Description :
//		Declaration of the CSysTestCases class
//

#ifndef __SYSCASES_H__
#define __SYSCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CConnectTest: Connect to the SQL server Data Source

class CConnectTest: public CTest
{
	DECLARE_TEST(CConnectTest, CDataSubSuite)

public:
	virtual void Run(void);
	virtual	BOOL RunAsDependent(void);
};



#endif // SYSCASES_H__
