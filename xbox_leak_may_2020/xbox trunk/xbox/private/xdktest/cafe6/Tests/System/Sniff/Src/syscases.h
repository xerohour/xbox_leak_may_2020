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
// CSysAppWizTest: SYS AppWizard test

class CSysAppWizTest: public CTest
{
	DECLARE_TEST(CSysAppWizTest, CSysSubSuite)

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
// CSysClassesTest: SYS Verify Classes

class CSysClassesTest: public CTest
{
	DECLARE_TEST(CSysClassesTest, CSysSubSuite)

public:
	virtual void Run(void);
};

///////////////////////////////////////////////////////////////////////////////
// CSysEditTest: SYS Edit test

class CSysEditTest: public CTest
{
	DECLARE_TEST(CSysEditTest, CSysSubSuite)

public:
	virtual void Run(void);
// support functions
protected:
	BOOL OpenSource(void);
	BOOL SearchSource(void);
	void EditSourceAndContinue(void);
// ide support
private:
	COSource m_src;
};

///////////////////////////////////////////////////////////////////////////////
// CSysResTest: SYS resource test

class CSysResTest: public CTest
{
	DECLARE_TEST(CSysResTest, CSysSubSuite)

// overrides
public:
	virtual void Run(void);

// support functions
protected:
	BOOL OpenResScript(void);
	BOOL AddDialog(void);
};

///////////////////////////////////////////////////////////////////////////////
// CSysBuildTest: SYS build test

class CSysBuildTest: public CTest
{
	DECLARE_TEST(CSysBuildTest, CSysSubSuite)

// overrides
public:
	virtual void Run(void);

	virtual BOOL RunAsDependent(void);

// support functions
protected:
	BOOL OpenProject(void);
	BOOL BuildProject(void);
};

///////////////////////////////////////////////////////////////////////////////
// CSysDebugPrjTest

class CSysDebugPrjTest: public CTest
{
	DECLARE_TEST(CSysDebugPrjTest, CSysSubSuite)

// overrides
public:
	virtual void Run(void);

// support functions
protected:
	BOOL ENCCases(void);
	void VerifyENCRelink(void);
	BOOL OpenProject(void);
	BOOL SetBreakpoint(void);
	BOOL RunToBreakpoint(void);
	BOOL StepOut(void);
	BOOL GoToDefinition(void);
	BOOL StepToCursor(void);
	BOOL StepOver(void);
	BOOL Restart(void);
	BOOL StepIntoMFC(void);
	BOOL StopDebugging(void);
	BOOL ClearAllBreakpoints(void);
};

///////////////////////////////////////////////////////////////////////////////
// CSysDebugExeTest

class CSysDebugExeTest: public CTest
{
	DECLARE_TEST(CSysDebugExeTest, CSysSubSuite)

// overrides
public:
	virtual void Run(void);
};

#endif // SYSCASES_H__
