///////////////////////////////////////////////////////////////////////////////
//	DisAcase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CDisAsmIDETest Class
//

#ifndef __DisAcase_H__
#define __DisAcase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\DbgTestBase.h"
#include "disasub.h"

///////////////////////////////////////////////////////////////////////////////
//	CDisAsmIDETest class

class CDisAsmIDETest : public CDbgTestBase
{
	DECLARE_TEST(CDisAsmIDETest, CDisAsmSubSuite)

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
	void SourceAnnotation(void);
	void GoToSymbol(void);
	void HitBreakPoint(void);
	void SwitchDAMtoSRCandBack(void);
	void TraceIntoCall(void);
	void SwitchSRCtoDAMMultiInstance(void);
	void VerifyDockingView(void);
	void GoToMaxMinAddress(void);
	void GoToInvalidAddress(void);
	void OpenDAMWhileChildIsRunning(void);
	void CodeBytes(void);
	void SetFocusToThread(void);
	void GoToOverloadedSymbol(void);
	void GoAfterScrollDisassemblyWndModified(void);
	void CheckAsmSourceAndDAMConsistency(void);
	void RedisplayingAfterEditingRegistersAndMemoryWindowsNoCVINFO(void);
	void ToolbarToDAMExpression(void);
	void StepIntoStepOver(void);

	void HitBreakPointNoCVINFO(void);
	void NavigateStackNoCVINFO(void);
	void PgUpDownCtrlHomeEndNoCVINFO(void);
	void SetNextStatementViaCtrlShiftF7NoCVINFO(void);

	void DebugJIT(void);

protected:
	BOOL GoAfterScrollDisassemblyWnd(void);
	void CheckThatIPisAtFuncWithArg(void);
	void MinimalTestResizeMoveWindow(void);
	void TestResizeMoveWindow(BOOL bIsDocked, BOOL bFullTest);
	void PgUpDownCtrlHomeEnd(void);
	void PopulateDamWindowText(void);


private:
	int m_TestNumber;
	int m_TotalNumberOfTests;
	CString m_strLogInfo;
};

#endif //__DisAcase_H__
