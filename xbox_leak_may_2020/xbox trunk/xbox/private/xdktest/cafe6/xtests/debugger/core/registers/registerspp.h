///////////////////////////////////////////////////////////////////////////////
//	RegistersPP.h
//
//	Created by:			
//		dverma
//
//	Description:
//		VC6 Processor Pack testcases.
//

#ifndef __RegistersPP_H__
#define __RegistersPP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\DbgTestBase.h"
#include "regsub.h"
//#include "eesub.h"	//	for watch window access

///////////////////////////////////////////////////////////////////////////////
//	CRegistersPP class

class CRegistersPP : public CDbgTestBase

{
	DECLARE_TEST(CRegistersPP, CRegistersSubSuite)

// Attributes
public:
	// the base name of the localized directories and files we will use.
	CString m_strProjBase;
	// the location of the unlocalized sources, under which the projects we will use will be located.
	CString m_strSrcDir;
	// the locations of the dll and exe projects, and their localized sources, that we will use for this test.
	CString m_strExeDir;

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

// Test Cases	

	void VerifyUnsupportedRegistersNotPresent(void);
	void TestMMXreg(void);
	//xbox void Test3dNowReg(void);
	void TestXMMreg(void);
	//xbox void TestWNIreg(void);
	void testRegVal(char *varname, char *vartype, char *value, char *message);
	void VerifyRegArray(CString Arr[],int Num_items);

private:
	int m_TestNumber;
	int m_TotalNumberOfTests;
	CString m_strLogInfo;
	//xbox char bSystemIsWin9X;
};

#endif //__RegistersPP_H__
