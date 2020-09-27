///////////////////////////////////////////////////////////////////////////////
//	Remove.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CRemoveingCodeTest Class
//

#ifndef __Remove_H__
#define __Remove_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\ecset.h"
#include "sniffsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CRemovingCodeTest class

class CRemovingCodeTest : public CECTestSet
{
	DECLARE_DYNAMIC(CRemovingCodeTest)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run();
	CRemovingCodeTest(CSubSuite* pSubSuite, LPCSTR szName, int nExpectedCompares, LPCSTR szListFilename = NULL);

// Test Cases
protected:

	BOOL RemoveFunction();
	BOOL RemoveMemberFunction();
	BOOL RemoveFunctionCall();
	BOOL RemoveGlobal();
	BOOL RemoveDataMember();
	BOOL RemoveStaticVar();
	BOOL RemoveLocalVar();
	BOOL RemoveLocalObject();
	BOOL RemoveOperations();
	BOOL RemoveLoop();

};

///////////////////////////////////////////////////////////////////////////////
//	CRemovingCodeTestEXE class

class CRemovingCodeTestEXE : public CRemovingCodeTest
{
	DECLARE_TEST(CRemovingCodeTestEXE, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

///////////////////////////////////////////////////////////////////////////////
//	CRemovingCodeTestEXT class

class CRemovingCodeTestEXT : public CRemovingCodeTest
{
	DECLARE_TEST(CRemovingCodeTestEXT, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

///////////////////////////////////////////////////////////////////////////////
//	CRemovingCodeTestINT class

class CRemovingCodeTestINT : public CRemovingCodeTest
{
	DECLARE_TEST(CRemovingCodeTestINT, CSniffSubSuite)

// Operations
public:
	virtual void Run();

// Test Cases
protected:


};

#endif //__Remove_H__
