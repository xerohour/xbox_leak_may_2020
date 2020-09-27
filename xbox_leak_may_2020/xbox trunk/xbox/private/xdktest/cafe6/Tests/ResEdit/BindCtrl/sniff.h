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

#define PROJNAME "Contain Proj"
#define PROJDIR "Contain Proj\\"

// Stuff needed for DataInfo holding.

// BEGIN_CLASS_HELP
// ClassName: CObjectInfo
// BaseClass: none
// Category: Project
// END_CLASS_HELP
struct CObjectInfo
{
	enum TYPE { TABLE, PROC, VIEW, TRIGGER};

	CString		m_strName;			// Name of the table.
	CString		m_strColumn;		// Return the column name,
									// data type for data.
	TYPE		m_Type; 			// stored proc or table or view?

	~CObjectInfo();

    void ClearAll();

    LPCSTR GetName() const { return (LPCSTR)m_strName; }
	LPCSTR GetColumn() const { return (LPCSTR)m_strColumn; }
	LPCSTR GetType() const {
		if(m_Type==TABLE) return "TABLE";
		if(m_Type==PROC) return "Return Type";
		if(m_Type==VIEW) return "VIEW";
		if(m_Type==TRIGGER) return "TRIGGER";
		return NULL; }

};

// BEGIN_CLASS_HELP
// ClassName: CClassInfo
// BaseClass: none
// Category: Project
// END_CLASS_HELP
struct CDataInfo 
{
    CString m_strConnect;				// Name of datasource.

	CString m_strServer;				// Server name
	CString m_strUser;					// user name
    CString m_strDataB;					// Name of database.
	CString m_strPassWord;				// Password for user
	CTypedPtrList<CPtrList, CObjectInfo*> m_listMembers;
	~CDataInfo();

    void ClearAll();
    LPCSTR GetName() const { return (LPCSTR)m_strDataB; }
};


///////////////////////////////////////////////////////////////////////////////
// CSysSubSuite class

class CBindSubSuite : public CIDESubSuite
{
	DECLARE_SUBSUITE(CBindSubSuite)

	DECLARE_TESTLIST()

// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
	virtual void CleanUp(void);
	void GetDataInfo(CString &strDataFile);
	int PropertySet(LPCSTR cntlrID,LPCSTR PropName,LPCSTR Value, int METHOD = METHOD_EDIT);
protected:
	BOOL RegisterAll();
// data
public:
	CDataInfo m_DataInfo[2];	//info abpout databases being accessed
	COProject m_prj;			//project being open
	COConnection *m_pconnec;
	BOOL m_projOpen;	//workaround bogus COProject::IsOpen()
//private:
	CString m_DefDialogId; //id of the dialog

};

#define m_DialogId (GetSubSuite()->m_DefDialogId)

BOOL SetFocusToResSym(LPCSTR SymName);

__declspec(selectany)  struct
{
	RECT pos;
	char ctrlName[128];
	char ctrlID[64];
}controldata[]={
	{{5,40,120,70},   "Microsoft DBCombo Control, version 5.0", "MY_COMBO1"},
	{{10,10,130,100}, "Microsoft DBList Control, version 5.0",  "MY_LIST1"},//bug in the DBList control - it does not resize itself
/*2*/{{20,130,150,150},"Microsoft RemoteData Control 2.0",       "MY_RDC"},
	{{160,70,280,100},"Microsoft DBCombo Control, version 5.0", "NEXT_COMBO"},
	{{160,100,280,120},"Microsoft DBCombo Control, version 5.0","LAST_COMBO"},
	{{160,130,270,150},"Microsoft Masked Edit Control, version 5.0","MY_MASKEDIT"},
/*6*/{{50,70,150,110}, "Microsoft RichText Control, version 5.0","MY_RICHEDIT"},
/*7*/{{10,155,300,300},"DBGrid  Control",						"MY_DBGrid"},
/*8*/{{135,5,300,70}, "Microsoft FlexGrid Control, version 5.0",  "MY_FlexGrid"},
};


#endif //__SNIFF_H__
