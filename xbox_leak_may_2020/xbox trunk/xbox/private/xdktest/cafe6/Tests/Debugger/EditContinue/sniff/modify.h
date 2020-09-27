///////////////////////////////////////////////////////////////////////////////
//	Modify.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CModifyingCodeTest Class
//

#ifndef __Modify_H__
#define __Modify_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\ecset.h"
#include "sniffsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CModifyingCodeTest class

class CModifyingCodeTest : public CECTestSet
{
//	DECLARE_TEST(CModifyingCodeTest, CSniffSubSuite)
	DECLARE_DYNAMIC(CModifyingCodeTest)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();
	CModifyingCodeTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

// Test Cases
protected:

	BOOL ModifyFunctionDeclaration();
	BOOL ModifyFunctionName();
	BOOL ModifyMemberFunctionDeclaration();
	BOOL ModifyFunctionDefinition();
	BOOL ReplaceFunctionCall();
	BOOL ModifyFunctionCall();
	BOOL ReplaceMemberFunctionCall();
	BOOL ModifyMemberFunctionCall();
	BOOL ModifyGlobal();
	BOOL ModifyDataMember();
	BOOL ModifyStaticVar();
	BOOL ModifyLocalVar();
	BOOL ModifyLocalFunctionPointer();
	BOOL ModifyLocalPointer();
	BOOL ModifyLocalObject();
	BOOL ModifyOperations();
	BOOL ModifyLoop();

};

///////////////////////////////////////////////////////////////////////////////
//	CModifyingCodeTestEXE class

class CModifyingCodeTestEXE : public CModifyingCodeTest
{
	DECLARE_TEST(CModifyingCodeTestEXE, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

///////////////////////////////////////////////////////////////////////////////
//	CModifyingCodeTestEXT class

class CModifyingCodeTestEXT : public CModifyingCodeTest
{
	DECLARE_TEST(CModifyingCodeTestEXT, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

///////////////////////////////////////////////////////////////////////////////
//	CModifyingCodeTestINT class

class CModifyingCodeTestINT : public CModifyingCodeTest
{
	DECLARE_TEST(CModifyingCodeTestINT, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

#endif //__Modify_H__
