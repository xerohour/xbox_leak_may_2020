///////////////////////////////////////////////////////////////////////////////
//	Regcase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CRegistersIDETest Class
//

#ifndef __Regcase_H__
#define __Regcase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\DbgTestBase.h"
#include "regsub.h"

const int gl_constNumberUndoRedo = 1;

///////////////////////////////////////////////////////////////////////////////
//	CRegistersIDETest class

class CRegistersIDETest : public CDbgTestBase
{
	DECLARE_TEST(CRegistersIDETest, CRegistersSubSuite)

// Attributes
public:
	// the base name of the localized directories and files we will use.
	CString m_strProjBase;
	// the location of the unlocalized sources, under which the projects we will use will be located.
	CString m_strSrcDir;
	// the locations of the dll and exe projects, and their localized sources, that we will use for this test.
	CString m_strExeDir;

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

// Test Cases

	void VerifyEIP(void);
	void EIP_ESP_ChangeAfterStep(void);
	void VerifyFlags(void);
	void VerifyEAX(void);
	void EditRegisters(void);
	void ToggleFlags(void);
	void EditRegisterViaQuickWatch(void);
	void Registers_Dam_Watch_Memory(void);
	void Registers_Watch_Memory(void);
	void Change_EAX_Changes_RetValue(void);
	void DisplayIsAlwaysHex(void);
	void IntegrityWithSetFocusToThread(void);
	void IntegrityWithFunctionEvaluation(void);
	void BoundaryConditionsForEditingValues(void);
	void TypeInvalidValues(void);
	void FloatingPointException(void);
	void VerifyDockingView(void);

protected:
	void HelperFunctionVerifyEIP(char* szTestCaseName, char* szEIP);
	void MinimalTestResizeMoveWindow(void);
// TODO(michma - 3/2/00): this isn't working yet.
//	void TestResizeMoveWindow(BOOL bIsDocked, BOOL bFullTest);

private:
	char m_pszUndoRedoRegisters[gl_constNumberUndoRedo][128];
	int m_TestNumber;
	int m_TotalNumberOfTests;
	CString m_strLogInfo;
};

#endif //__Regcase_H__
