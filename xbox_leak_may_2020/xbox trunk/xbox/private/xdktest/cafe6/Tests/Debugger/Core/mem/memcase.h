///////////////////////////////////////////////////////////////////////////////
//	memcase.h
//
//	Created by :			
//		VCBU QA
//
//	Description :
//		Declaration of the CmemIDETest Class
//

#ifndef __memcase_H__
#define __memcase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\DbgTestBase.h"
#include "memsub.h"

///////////////////////////////////////////////////////////////////////////////
//	CmemIDETest class

class CmemIDETest : public CDbgTestBase
{
	DECLARE_TEST(CmemIDETest, CmemSubSuite)

// Data
private:
	// the base name of the localized directories and files we will use.
	CString m_strProjBase;
	// the location of the unlocalized sources, under which the projects we will use will be located.
	CString m_strSrcDir;
	// the locations of the dll and exe projects, and their localized sources, that we will use for this test.
	CString m_strExeDir;

public:  // Operations
	virtual void PreRun(void);
	virtual void Run();

// Test cases
public:

	void GoToSymbol (void);
	void VerifyEditAllFormats(void);
	void EditInWatchVerifyInMemoryWindow(void);
	void VerifyColumns(void);
	void ReEvaluateExpression(void);
	void GoToOverloadedSymbol(void);
	void RestoringMWTroughtDebuggingSessions(void);
	void GoToMaxMinAddress(void);
	void GoToInvalidAddress(void);
	void EditEIP(void);
	void NavigatingPointer(void);
	void ContextMenuOnToolBar(void);
	void VerifyMultipleUndoRedo(void);
	void CycleThroughMemoryFormats(void);
	void PgUpDownCtrlHomeEnd(void);
	void VerifyDockingView(void);
	void VerifyEditing(void);
	void VerifyVCPPReg(void);
	void VerifyRegArray(CString Arr[],int Num_items);	

private:
	void InitializeGlobalsThatAreUsedInTheMemoryTests(void);
	void DoDifferentKeysInMemoryWindow(void);
	
// Old tests.  TODO (Dklem 07-30) remove them
	void GoAfterScrollMemoryWnd(void);
	void MemExpectBytes(void);
	void MemoryFormats(void);
	void TestResizeMoveWindow(BOOL bIsDocked, BOOL bFullTest);
	void MinimalTestResizeMoveWindow(void);

private:
	int m_TestNumber;
	int m_TotalNumberOfTests;
	CString m_strLogInfo;


};

#endif //__memcase_H__
