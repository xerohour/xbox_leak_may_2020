///////////////////////////////////////////////////////////////////////////////
//	scn02.H
//
//	Created by :			Date :
//		YefimS					5/2/97
//
//	Description :
//		Declaration of the CDebugAppWizAppSubSuite Test classes

#ifndef __scn02_H__
#define __scn02_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "scn02sub.h"

///////////////////////////////////////////////////////////////////////////////
//	CEditBuildTest class

class CEditBuildTest : public CSystemTestSet
{
	DECLARE_TEST(CEditBuildTest, CDebugAppWizAppSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL AddMemberVar(void);
	BOOL Build(void);
	BOOL EditCode(void);
	BOOL ReBuild(void);

// Utilities

};

///////////////////////////////////////////////////////////////////////////////
//	CDebugTest class

class CDebugTest : public CSystemTestSet
{
	DECLARE_TEST(CDebugTest, CDebugAppWizAppSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

public:

// Test Cases
protected:
	BOOL BreakStep(void);
	BOOL LocalsAndWatch(void);
	BOOL CallStack(void);
	BOOL EditAndGo(void);
	BOOL Memory(void);
	BOOL DAM(void);
	BOOL Threads( void );

// Utilities

};

#endif //__scn02_H__
