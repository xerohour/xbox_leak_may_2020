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
// CComWizTest: Build

class CContTest: public CTest
{
	DECLARE_TEST(CContTest, CBindSubSuite)

// overrides
public:
	virtual void Run(void);

	virtual BOOL RunAsDependent(void);

// support functions
protected:
	BOOL NewProject(void);
	BOOL SaveProject(void);
};

///////////////////////////////////////////////////////////////////////////////
// CBindClassesTest: SYS Verify the project classes

class CAddControlTest: public CTest
{
	DECLARE_TEST(CAddControlTest, CBindSubSuite)

public:
	virtual void Run(void);
	int CreateResource( LPCSTR szResName );
	int AddOleControl(LPCSTR Name,LPCSTR ID_Name,RECT *position=NULL);
};



#endif // SYSCASES_H__
