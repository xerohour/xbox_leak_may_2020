///////////////////////////////////////////////////////////////////////////////
//	Add.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CAddingCodeTest Class
//

#ifndef __Add_H__
#define __Add_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\ecset.h"
#include "sniffsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CAddingCodeTest class

class CAddingCodeTest : public CECTestSet
{
//	DECLARE_TEST(CAddingCodeTest, CSniffSubSuite)
	DECLARE_DYNAMIC(CAddingCodeTest)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();
	CAddingCodeTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

// Test Cases
protected:

	BOOL AddFunction();
	BOOL AddMemberFunction();
	BOOL AddFunctionCall();
	BOOL AddGlobal();
	BOOL AddDataMember();
	BOOL AddStaticVar();
	BOOL AddLocalVar();
	BOOL AddLocalObject();
	BOOL AddOperations();
	BOOL AddLoop();

};

///////////////////////////////////////////////////////////////////////////////
//	CAddingCodeTestEXE class

class CAddingCodeTestEXE : public CAddingCodeTest
{
	DECLARE_TEST(CAddingCodeTestEXE, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

///////////////////////////////////////////////////////////////////////////////
//	CAddingCodeTestEXT class

class CAddingCodeTestEXT : public CAddingCodeTest
{
	DECLARE_TEST(CAddingCodeTestEXT, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

///////////////////////////////////////////////////////////////////////////////
//	CAddingCodeTestINT class

class CAddingCodeTestINT : public CAddingCodeTest
{
	DECLARE_TEST(CAddingCodeTestINT, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

#endif //__Add_H__
