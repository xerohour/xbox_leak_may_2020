///////////////////////////////////////////////////////////////////////////////
//	SQLSDI.H
//
//	Created by :			Updated :
//		ChrisKoz			9/15/96
//
//	Description :
//		Declaration of the CSQLSDICases class
//			Test cases for the SQL Debugger
//

#ifndef __SQLSDI_H__
#define __SQLSDI_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sniff.h"

///////////////////////////////////////////////////////////////////////////////
// CSQLSDICases class
class CAddDSNTest : public CTest
{
	DECLARE_TEST(CAddDSNTest, CSniffDriver)

// Operations
	
public:
	virtual void Run(void);
	int SetBP(BPInfo *pBPInfo,CString &Server,CString &DataBase);

// Utility Functions
protected:
	BOOL ActivateContextMenu();
	BOOL ContextMenuChoice(CString strChoice);
	BOOL ActivatePropertiesWindow();
	BOOL bFileComp( CString cstrFile0, CString cstrFile1);

// Data
protected:

// Test Cases
protected:
	int CheckNodeProperties(LPCSTR Name);
	int SetBPOnStmt(CString Name,CString Stmt);

};



class CBuildTest : public CTest
{
	DECLARE_TEST(CBuildTest, CSniffDriver)

// Operations
protected:
	BOOL VerifyBP(void);
	void SetDebugTarget(void);
	BOOL BuildProject(void);

public:
	virtual void Run(void);
	virtual BOOL CBuildTest::RunAsDependent(void);

};

class CInprocTest : public CTest
{
	DECLARE_TEST(CInprocTest, CSniffDriver)

// Operations
	
public:
	virtual void Run(void);
};

class CAppTest : public CTest
{
	DECLARE_TEST(CAppTest, CSniffDriver)

// data
protected:
	CODebug m_coDebug;
// Operations
	
public:
	virtual void Run(void);
//	BOOL Wait(int WAIT);
	BOOL HitBreakpoints();


};


#endif //__SQLSDI_H__
