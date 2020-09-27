///////////////////////////////////////////////////////////////////////////////
//	BldXQuakecase.h
//

#ifndef __APPWIZCASE_H__
#define __APPWIZCASE_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\IdeTestBase.h"
#include "BldXQuakesub.h"

///////////////////////////////////////////////////////////////////////////////
//	CDisAsmIDETest class

class CBldXQuakeTest : public CIdeTestBase
{
	DECLARE_TEST(CBldXQuakeTest, CBldXQuakeSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

// Test Cases
	void BuildXQuake(void);

	void SafeCloseProject();

private:
	int m_TestNumber;
	int m_TotalNumberOfTests;
	CString m_strLogInfo;
};

#endif //__APPWIZCASE_H__
