///////////////////////////////////////////////////////////////////////////////
//	localscases.h
//
//	Created by:	MichMa		Date: 10/22/97
//
//	Description:
//		Declaration of the CECLocals3Cases Class
//

#ifndef __LOCALSCASES_H__
#define __LOCALSCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\ecset.h"
#include "localssub.h"


class CECLocal : public CLocal

{
public:

	CString m_strVerifyValue;
	CString m_strVerifyType;
	BOOL m_bUsesCtorDtor;
	EC_EDIT_TYPE m_ecEditType;
	CStringArray m_strCodeLines;
	CECLocal(void){}
	
	CECLocal(CECLocal &ECLocalSrc)
	
	{
		m_strType = ECLocalSrc.m_strType;
		m_strName = ECLocalSrc.m_strName;
		m_strValue = ECLocalSrc.m_strValue;
		m_ExprState = ECLocalSrc.m_ExprState;
		m_strVerifyValue = ECLocalSrc.m_strVerifyValue;
		m_strVerifyType = ECLocalSrc.m_strVerifyType;
		m_bUsesCtorDtor = ECLocalSrc.m_bUsesCtorDtor;
		m_ecEditType = ECLocalSrc.m_ecEditType;

		for(int iCodeLine = 0; iCodeLine < ECLocalSrc.m_strCodeLines.GetSize(); iCodeLine++)
			m_strCodeLines.Add(ECLocalSrc.m_strCodeLines[iCodeLine]);
	}

	const CECLocal& operator=(const CECLocal& ECLocalSrc)
	
	{
		m_strType = ECLocalSrc.m_strType;
		m_strName = ECLocalSrc.m_strName;
		m_strValue = ECLocalSrc.m_strValue;
		m_ExprState = ECLocalSrc.m_ExprState;
		m_strVerifyValue = ECLocalSrc.m_strVerifyValue;
		m_strVerifyType = ECLocalSrc.m_strVerifyType;
		m_bUsesCtorDtor = ECLocalSrc.m_bUsesCtorDtor;
		m_ecEditType = ECLocalSrc.m_ecEditType;

		for(int iCodeLine = 0; iCodeLine < ECLocalSrc.m_strCodeLines.GetSize(); iCodeLine++)
			m_strCodeLines.Add(ECLocalSrc.m_strCodeLines[iCodeLine]);

		return *this;	
	}
};


class CFunc

{
public:

	CString m_strName;
	CString m_strFile;
	CString m_strChild;
	CArray<CECLocal, CECLocal> m_Locals;
	CFunc(void){}
	
	CFunc(CFunc &FuncSrc)
	
	{
		m_strName = FuncSrc.m_strName;
		m_strFile = FuncSrc.m_strFile;
		m_strChild = FuncSrc.m_strChild;

		for(int iLocal = 0; iLocal < FuncSrc.m_Locals.GetSize(); iLocal++)
			m_Locals.Add(FuncSrc.m_Locals[iLocal]);
	}

	const CFunc& operator=(const CFunc &FuncSrc)
	
	{
		m_strName = FuncSrc.m_strName;
		m_strFile = FuncSrc.m_strFile;
		m_strChild = FuncSrc.m_strChild;

		for(int iLocal = 0; iLocal < FuncSrc.m_Locals.GetSize(); iLocal++)
			m_Locals.Add(FuncSrc.m_Locals[iLocal]);

		return *this;	
	}
};


class CAddTest

{
public:

	BOOL m_bTestDone;
	CECLocal m_Local;
	CAddTest(void){}

	CAddTest(CAddTest &AddTestSrc)
	{
		m_bTestDone = AddTestSrc.m_bTestDone;
		m_Local = AddTestSrc.m_Local;
	}

	const CAddTest& operator=(const CAddTest &AddTestSrc)
	{
		m_bTestDone = AddTestSrc.m_bTestDone;
		m_Local = AddTestSrc.m_Local;
		return *this;
	}
};


class CValueTest

{
public:

	BOOL m_bTestDone;
	CECLocal m_Local;
	BOOL m_bAddAssignment;
	CValueTest(void){}

	CValueTest(CValueTest &ValueTestSrc)
	{
		m_bTestDone = ValueTestSrc.m_bTestDone;
		m_Local = ValueTestSrc.m_Local;
		m_bAddAssignment = ValueTestSrc.m_bAddAssignment;
	}

	const CValueTest& operator=(const CValueTest &ValueTestSrc)
	{
		m_bTestDone = ValueTestSrc.m_bTestDone;
		m_Local = ValueTestSrc.m_Local;
		m_bAddAssignment = ValueTestSrc.m_bAddAssignment;
		return *this;
	}
};


class CTypeTest

{
public:

	BOOL m_bTestDone;
	CECLocal m_Local;
	CString m_strNewType;
	CTypeTest(void){}

	CTypeTest(CTypeTest &TypeTestSrc)
	{
		m_bTestDone = TypeTestSrc.m_bTestDone;
		m_Local = TypeTestSrc.m_Local;
		m_strNewType = TypeTestSrc.m_strNewType;
	}

	const CTypeTest& operator=(const CTypeTest &TypeTestSrc)
	{
		m_bTestDone = TypeTestSrc.m_bTestDone;
		m_Local = TypeTestSrc.m_Local;
		m_strNewType = TypeTestSrc.m_strNewType;
		return *this;
	}
};


class CRemoveTest 

{
public:

	BOOL m_bTestDone;
	EC_EDIT_TYPE m_ecEditType;
	BOOL m_bLeaveReferences;
	CRemoveTest(void){}

	CRemoveTest(CRemoveTest &RemoveTestSrc)
	{
		m_bTestDone = RemoveTestSrc.m_bTestDone;
		m_ecEditType = RemoveTestSrc.m_ecEditType;
		m_bLeaveReferences = RemoveTestSrc.m_bLeaveReferences;
	}

	const CRemoveTest& operator=(const CRemoveTest &RemoveTestSrc)
	{
		m_bTestDone = RemoveTestSrc.m_bTestDone;
		m_ecEditType = RemoveTestSrc.m_ecEditType;
		m_bLeaveReferences = RemoveTestSrc.m_bLeaveReferences;
		return *this;
	}
};


///////////////////////////////////////////////////////////////////////////////
//	CECLocalsCases class

class CECLocalsCases : public CECTestSet

{
	DECLARE_TEST(CECLocalsCases, CECLocalsSubSuite)

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
	void InitFuncs(void);
	void InitTests(void);
	void VerifyFunctionLocals(CFunc &Func);
	void ApplyCodeChanges(void);
	BOOL GetBackToStartOfWinMain(void);
	BOOL RunToLastLineOfFunc(LPCSTR szFunc, EC_EDIT_TYPE ecEditType);
	int GetFunctionIndexFromName(LPCSTR szName);

	// Add test functions.
	BOOL DoTest(CAddTest &AddTest);
	int RandomlySelectFunctionToEdit(CAddTest &AddTest);
	void AddLocalToSource(CECLocal &Local);

	// Value test functions.
	BOOL DoTest(CValueTest &ValueTest);
	int RandomlySelectFunctionToEdit(CValueTest &ValueTest, int *piLocalToChange);
	void ReplaceLocalAssignmentInSource(CECLocal &Local);

	// Type test functions.
	BOOL DoTest(CTypeTest &TypeTest);
	int RandomlySelectFunctionToEdit(CTypeTest &TypeTest, int *piLocalToChange);
	void ReplaceLocalTypeInSource(CTypeTest &TypeTest);

	// Remove test functions.
	BOOL DoTest(CRemoveTest &RemoveTest);
	int RandomlySelectFunctionToEdit(CRemoveTest &RemoveTest);
	int FindLocalInLine(CString &strLine, CFunc &Func);
	void RemoveLocalsFromSource(CFunc &Func, CRemoveTest &RemoveTest);
	void RemoveLocalReferencesFromSource(CECLocal &Local);
};

#endif //__LOCALSCASES_H__
