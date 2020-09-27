///////////////////////////////////////////////////////////////////////////////
//	BreakpointsCases.h
//
//	Created by:	MichMa		Date: 3/12/98
//
//	Description:
//		Declaration of the CECBreakpointsCases Class
//

#ifndef __BREAKPOINTSCASES_H__
#define __BREAKPOINTSCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\ecset.h"
#include "BreakpointsSubsuite.h"


class CLocationTest 

{
public:

	BOOL m_bTestDone;
	EC_EDIT_TYPE m_ecEditType;
	CLocationTest(void){}

	CLocationTest(CLocationTest &LocationTestSrc)
	{
		m_bTestDone = LocationTestSrc.m_bTestDone;
		m_ecEditType = LocationTestSrc.m_ecEditType;
	}

	const CLocationTest& operator=(const CLocationTest &LocationTestSrc)
	{
		m_bTestDone = LocationTestSrc.m_bTestDone;
		m_ecEditType = LocationTestSrc.m_ecEditType;
		return *this;
	}
};


class CDataTest 

{
public:

	BOOL m_bTestDone;
	BOOL m_bConditional;
	EC_EDIT_TYPE m_ecEditType;
	CDataTest(void){}

	CDataTest(CDataTest &DataTestSrc)
	{
		m_bTestDone = DataTestSrc.m_bTestDone;
		m_ecEditType = DataTestSrc.m_ecEditType;
		m_bConditional = DataTestSrc.m_bConditional;
	}

	const CDataTest& operator=(const CDataTest &DataTestSrc)
	{
		m_bTestDone = DataTestSrc.m_bTestDone;
		m_ecEditType = DataTestSrc.m_ecEditType;
		m_bConditional = DataTestSrc.m_bConditional;
		return *this;
	}
};


///////////////////////////////////////////////////////////////////////////////
//	CECBreakpointsCases class

class CECBreakpointsCases : public CECTestSet

{
	DECLARE_TEST(CECBreakpointsCases, CECBreakpointsSubsuite)

private:
	// Data
	CString m_strProjBase;
	CString m_strSrcFullPath;
	CString m_strProjDirFullPath;

public:

	// Operations
	virtual void PreRun(void);
	virtual void Run();

	// Utilities
	void InitTests(void);
	BOOL GetBackToStartOfWinMain(void);
	void ApplyCodeChanges(void);

	// location test functions.
	BOOL DoTest(CLocationTest &LocationTest);
	void AddCodeToSource(EC_EDIT_TYPE ecEditType);

	// data test functions.
	BOOL DoTest(CDataTest &DataTest);
	void AddLocalToSource(CDataTest &DataTest);
	BOOL SetDataBreakpoint(CDataTest &DataTest);
};

#endif //__BREAKPOINTSCASES_H__
