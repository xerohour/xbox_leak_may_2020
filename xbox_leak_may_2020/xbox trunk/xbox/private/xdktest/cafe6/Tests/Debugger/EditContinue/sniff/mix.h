///////////////////////////////////////////////////////////////////////////////
//	Mix.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CMixTest Class
//

#ifndef __Mix_H__
#define __Mix_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\ecset.h"
#include "sniffsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CMixTest class

class CMixTest : public CECTestSet
{
//	DECLARE_TEST(CMixTest, CSniffSubSuite)
	DECLARE_DYNAMIC(CMixTest)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();
	CMixTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

// Test Cases
protected:

	BOOL EditHeaderExe();
	BOOL EditIncludeCpp();
	BOOL ECStopDebuggingLink();
	BOOL SyntaxError();

};

///////////////////////////////////////////////////////////////////////////////
//	CMixTestEXE class

class CMixTestEXE : public CMixTest
{
	DECLARE_TEST(CMixTestEXE, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

///////////////////////////////////////////////////////////////////////////////
//	CMixTestEXT class

class CMixTestEXT : public CMixTest
{
	DECLARE_TEST(CMixTestEXT, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

///////////////////////////////////////////////////////////////////////////////
//	CMixTestINT class

class CMixTestINT : public CMixTest
{
	DECLARE_TEST(CMixTestINT, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

#endif //__Mix_H__
