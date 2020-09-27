///////////////////////////////////////////////////////////////////////////////
//	SQLDebugCase.H
//
//	Created by :			Date :
//		CHRISKOZ					5/21/97
//
//	Description :
//		Declaration of the CSQLDebugCase class

#ifndef __SQLDebugCase_H__
#define __SQLDebugCase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\sysset.h"
#include "SQLDebugSuite.h"

///////////////////////////////////////////////////////////////////////////////
//	CSQLDebugCase class

class CSQLDebugCase : public CSystemTestSet
{
	DECLARE_TEST(CSQLDebugCase, CSQLDebugSuite)

// Operations
public:
	virtual void PreRun(void);
	virtual void Run(void);
	
	void CreateFileDSN(LPCSTR DSN,LPCSTR ServerName, LPCSTR DBName, LPCSTR User, LPCSTR Pwd);
	void CreateDBProjectInCurrentWksp(LPCSTR ServerName,LPCSTR DBName, LPCSTR User, LPCSTR Pwd);
	void InsertStoredProcs(LPCSTR ServerName,LPCSTR DBName, LPCSTR User, LPCSTR Pwd);
	void DebugSP(LPCSTR Server,LPCSTR strDBName, LPCSTR User, LPCSTR Pwd);
	void CopySQLFile(CString &m_Source,CString &m_Dest, LPCSTR User, LPCSTR FName);
	BOOL StoredProcParam(LPCSTR Params =NULL);

private:
	COProject m_prj;
	CString m_SQLProjectName;
	CString m_DSN;
	CString m_User;
	CString m_Pwd;
	CString m_Computername;
};

#endif //__SQLDebugCase_H__

//utility functions
void CopySQLFile(CString &m_Source,CString &m_Dest,LPCSTR FName);
void NewSP(LPCSTR filename);


void CollapseObjects();
void ExpandTable(LPCSTR Server,LPCSTR DBName,LPCSTR strTableName);
BOOL SelectDatabase(LPCSTR Server,LPCSTR strDBName);
int ExpandDatabase(LPCSTR Server,LPCSTR strDBName);
void ExpandProcedureNode(LPCSTR Server,LPCSTR strDBName);
void ExpandTableNode(LPCSTR Server,LPCSTR strDBName);
