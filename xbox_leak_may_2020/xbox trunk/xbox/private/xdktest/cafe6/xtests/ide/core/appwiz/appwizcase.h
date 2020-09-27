///////////////////////////////////////////////////////////////////////////////
//	AppWizcase.h
//

#ifndef __APPWIZCASE_H__
#define __APPWIZCASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\IdeTestBase.h"
#include "AppWizsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CDisAsmIDETest class

class CAppWizTest : public CIdeTestBase
{
	DECLARE_TEST(CAppWizTest, CAppWizSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

// Test Cases
	void CreateWin32App(void);
	void CreateWin32ConsoleApp(void);
	void CreateWin32Dll(void);
	void CreateWin32StaticLib(void);
	void CreateXboxGame(void);
	void CreateXboxStaticLib(void);
	void CreateAndBuildXboxStaticLib(void);
	void UseStaticLibDebug(void);
	void UseStaticLibRelease(void);

	void SafeCloseProject();

private:
	int m_TestNumber;
	int m_TotalNumberOfTests;
	CString m_strLogInfo;
};

#endif //__APPWIZCASE_H__
