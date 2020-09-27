///////////////////////////////////////////////////////////////////////////////
//	DISASMPP.H
//
//	Created by:			
//		dverma
//
//	Description:
//		VC6 Processor Pack testcases.
//

#ifndef __DisAsmPP_H__
#define __DisAsmPP_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\..\DbgTestBase.h"
#include "disasub.h"

///////////////////////////////////////////////////////////////////////////////
//	CDisAsmPP class

class CDisAsmPP : public CDbgTestBase
{
	DECLARE_TEST(CDisAsmPP, CDisAsmSubSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);

// Test Cases
	void TestMMX(void);
	void TestKatmai(void);

protected:
	BOOL GoAfterScrollDisassemblyWnd(void);
	void CheckThatIPisAtFuncWithArg(void);
	void MinimalTestResizeMoveWindow(void);
	void TestResizeMoveWindow(BOOL bIsDocked, BOOL bFullTest);
	void PgUpDownCtrlHomeEnd(void);


private:
	int m_TestNumber;
	int m_TotalNumberOfTests;
	CString m_strLogInfo;
};

#endif //__DisAsmPP_H__
