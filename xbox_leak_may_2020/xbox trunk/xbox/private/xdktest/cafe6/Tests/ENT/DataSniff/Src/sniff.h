///////////////////////////////////////////////////////////////////////////////
//	SNIFF.H
//
//	Created by :			Date :
//		TomWh					11/1/93
//
//	Description :
//		Declaration of the CSniffDriver class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#define PROJNAME "Data Base Proj"
#define PROJDIR "Data Base Proj\\"

// Stuff needed for Data View verification.

// BEGIN_CLASS_HELP
// ClassName: CObjectInfo
// BaseClass: none
// Category: Project
// END_CLASS_HELP

struct CObjectInfo
{
	enum TYPE { TABLE, VIEW, DIAGRAM, PROC, TRIGGER, ADHOC, NEWPROC};
//	enum ACCESS { PUBLIC, PRIVATE, PROTECTED, GLOBAL };

	CString		m_strName;			// Name of member.
	CString		m_strParams;		// Parameters 
	// actually params for PROC/NEWPROC, or table owner for TRIGGER)
	CString		m_strOwner;			//owner of this object
	TYPE		m_Type; 			// Type TABLE, PROC, ...

	~CObjectInfo();

    void ClearAll();

    LPCSTR GetName() const { ASSERT(!m_strName.IsEmpty()); return (LPCSTR)m_strName; }
	CString GetTypeName() const {
		if(m_Type==TABLE) 
			return GetLocString(IDSS_TABLE); //Table
		if(m_Type==VIEW) 
			return GetLocString(IDSS_VIEW); //View
		if(m_Type==DIAGRAM) 
			return GetLocString(IDSS_DIAGRAM); //Database Diagram
		if(m_Type==PROC || m_Type==NEWPROC) 
			return GetLocString(IDSS_STOREDPROC); //Stored Procedure
		if(m_Type==TRIGGER) 
			return GetLocString(IDSS_TRIGGER); //Trigger
		if(m_Type==ADHOC) 
			return CString(""); 
		return CString(""); 
	}

};

// BEGIN_CLASS_HELP
// ClassName: CClassInfo
// BaseClass: none
// Category: Project
// END_CLASS_HELP
struct CDataInfo 
{
    CString m_strDataBase;					// Name of database.
    CString m_strDSN;				// Name of datasource.
	CString m_strServer;				// Server name
	CString m_strUser;					// user name
	CString m_strPassWord;				// Password for user
	CTypedPtrList<CPtrList, CObjectInfo*> m_listMembers;
	void GetDataInfo(CString &DirName,CString &FileName);
	~CDataInfo();

    void ClearAll();
    LPCSTR GetName() const { return (LPCSTR)m_strDataBase; }
};


///////////////////////////////////////////////////////////////////////////////
// CSysSubSuite class

class CDataSubSuite : public CIDESubSuite
{
	DECLARE_SUBSUITE(CDataSubSuite)

	DECLARE_TESTLIST()

// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
	virtual void CleanUp(void);
protected:
	BOOL RegisterAll();
// data
public:
	CDataInfo m_DataInfo;		//database in project's DAataPana
	COProject m_prj;			//project being open
	int m_projOpen;	// 0 closed, 1 open, -1 open with no pwd validated
	CString m_Computername;

//operations
	void OpenDataProject();
	BOOL ForcePassword(CDataInfo *pDataInfo);
	int CheckObjectProp(CObjectInfo* pMember);


	BOOL SelectDatabase(CString strDBName);
	BOOL RefreshDatabase(CString strDBName);
	void ExpandProcedureNode(CString strDBName);
	void ExpandTableNode(CString strDBName);
	void ExpandDiagramsNode(CString strDBName);
	void ExpandViewNode(CString strDBName);
	void ExpandTable(CString strTableName);
	void CollapseObjects(BOOL isTrigger);
	void ExpandDatabase(CString strDBName);
//utilities preparing the database
	int CDataSubSuite::CleanAndInsertTable(LPCSTR sqlTableName);
	int CDataSubSuite::CopySQLFile(CString &m_Source,CString &m_Dest,LPCSTR FName);
};


#endif //__SNIFF_H__
