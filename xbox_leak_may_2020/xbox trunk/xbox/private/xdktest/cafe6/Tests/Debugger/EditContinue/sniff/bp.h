///////////////////////////////////////////////////////////////////////////////
//	BP.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CBreakpointsTest Class
//

#ifndef __BP_H__
#define __BP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\ecset.h"
#include "sniffsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CBreakpointsTest class

class CBreakpointsTest : public CECTestSet
{
//	DECLARE_TEST(CBreakpointsTest, CSniffSubSuite)
	DECLARE_DYNAMIC(CBreakpointsTest)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();
	CBreakpointsTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

// Test Cases
protected:

	BOOL BPAtNewCodeLine();
	BOOL BPOnNewLocal();
	BOOL BPOnNewLocalWithCodition();

};

///////////////////////////////////////////////////////////////////////////////
//	CBreakpointsTestEXE class

class CBreakpointsTestEXE : public CBreakpointsTest
{
	DECLARE_TEST(CBreakpointsTestEXE, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

///////////////////////////////////////////////////////////////////////////////
//	CBreakpointsTestEXT class

class CBreakpointsTestEXT : public CBreakpointsTest
{
	DECLARE_TEST(CBreakpointsTestEXT, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

///////////////////////////////////////////////////////////////////////////////
//	CBreakpointsTestINT class

class CBreakpointsTestINT : public CBreakpointsTest
{
	DECLARE_TEST(CBreakpointsTestINT, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

#endif //__BP_H__
