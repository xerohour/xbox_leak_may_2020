///////////////////////////////////////////////////////////////////////////////
//	Dbg_Stress_base.h
//
//	Created by :			
//		Xbox XDK Test
//
//	Description :
//		Declaration of the CDbgStressBaseTest Class
//

#ifndef __exe_base_H__
#define __exe_base_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\dbgtestbase.h"
#include "Dbg_Stress_Sub.h"

///////////////////////////////////////////////////////////////////////////////
//	CexeIDETest class

class CDbgStressStopAndGoTest : public CDbgTestBase
{
	DECLARE_TEST(CDbgStressStopAndGoTest, CDbgStressSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:

	BOOL StopAndGo();
};


class CDbgStressRestartAndGoTest : public CDbgTestBase
{
	DECLARE_TEST(CDbgStressRestartAndGoTest, CDbgStressSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:

	BOOL RestartAndGo();
};


class CDbgStressBreakAndGoTest : public CDbgTestBase
{
	DECLARE_TEST(CDbgStressBreakAndGoTest, CDbgStressSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:
	BOOL BreakAndGo();
};


class CDbgStressConditionalBPNonCountingTest : public CDbgTestBase
{
	DECLARE_TEST(CDbgStressConditionalBPNonCountingTest, CDbgStressSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:
	BOOL CondBPNonCounting();
};


class CDbgStressConditionalBPCountingTest : public CDbgTestBase
{
	DECLARE_TEST(CDbgStressConditionalBPCountingTest, CDbgStressSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:
	BOOL CondBPCounting();
};

class CDbgStressDataBPTest : public CDbgTestBase
{
	DECLARE_TEST(CDbgStressDataBPTest, CDbgStressSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();

// Test Cases
protected:
	BOOL DataBP();
};

#endif //__exe_base_H__
