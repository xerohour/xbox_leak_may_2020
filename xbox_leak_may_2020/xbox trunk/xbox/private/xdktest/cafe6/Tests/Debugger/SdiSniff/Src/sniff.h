///////////////////////////////////////////////////////////////////////////////
//	SNIFF.H
//
//	Created by :			Date :
//		Chriskoz			9/5/96
//
//	Description :
//		Declaration of the CSniffDriver class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define MAX_BPTABLE 30
#define MAX_SRVTABLE 5 

///////////////////////////////////////////////////////////////////////////////
// CSniffDriver class

class BPInfo{
public:
	CString Proc;			//form of TABLE;TRIGGER if this is trigger
	CString Stmt;			//statement begining
	int		ServerInd;		//index of a server in servers table
	int		HitCount;		//how many times the BP is supposed to be hit
};

class CSniffDriver : public CIDESubSuite
{
	DECLARE_SUBSUITE(CSniffDriver)

	DECLARE_TESTLIST()

// Data members
public:
	CString m_AppName;			//name of the workspace to be open
	CString m_AppCmd;			//command arguments for the app
	COProject m_prj;			//project being open
	int		 m_projOpen;		//==0 closed, ==1 open ==-1 open, not pwd validated
	struct serverinfo
	{
		CString  m_DSN;				//data source name
		CString  m_Server;			//server name
		CString  m_User;			//user name
		CString  m_Psw;				//password
		CString  m_DataBase;		//database (might be different for same servers)
		CString  m_SpScripts;		//list of (blank separ) scripts to exec on setup
	}m_svrtable[MAX_SRVTABLE];
	int m_Servers;
	BPInfo BPTable[MAX_BPTABLE];
	int m_BPs;
// Member functions
BOOL RegisterAll();
void GetDataInfo(CString &bpTableFile);



// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
	virtual void CleanUp(void);

// utility functions
	BOOL OpenProject(void);
	void CollapseObjects();
	void ExpandTable(CString &Server,CString &DBName,LPCSTR strTableName);
	BOOL SelectDatabase(CString &Server,CString &strDBName);
	int ExpandDatabase(CString &Server,CString &strDBName);
	void ExpandProcedureNode(CString &Server,CString &strDBName);
	void ExpandTableNode(CString &Server,CString &strDBName);

};

#endif //__SNIFF_H__
