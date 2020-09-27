///////////////////////////////////////////////////////////////////////////////
//  ODBCDLGS.H
//
//  Created by :            Date :
//      IvanL              6/17/94
//
//  Description :
//      Declaration of the UISqldialogs class
//

#ifndef __ODBCDLGS_H__
#define __ODBCDLGS_H__

#include "..\..\udialog.h"
#include "..\sym\clswiz.h"
#include "prjxprt.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif

#define ERROR_ERROR -1

///////////////////////////////////////////////////////////////////////////////
//  UISqldialogs class 

 
typedef enum {DAO_SOURCE =CLSWIZ_IDC_DAO , ODBC_SOURCE= CLSWIZ_IDC_ODBC} DBSOURCES ;
typedef enum {SNAPSHOT_TYPE = CLSWIZ_IDC_SNAPSHOT, DYNASET_TYPE =CLSWIZ_IDC_DYNASET , TABLES_TYPE = CLSWIZ_IDC_TABLE} REC_SET_TYPE ;

// BEGIN_CLASS_HELP
// ClassName: UISqlDataSources
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UISqlDataSources : public UIDialog 
	{
    UIWND_COPY_CTOR (UISqlDataSources, UIDialog) ; 

// Private data
    private:
			int m_ActiveSource ;
// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return ("Data Options"); }// REVIEW : LOCALIZATION 
			
			HWND Display(void) ;
			int DataSource(DBSOURCES source);
			int RecordSetType(REC_SET_TYPE Type);
			int DetectDirtyCol(int Yes = 1) ;
			int BindAllCol(int Yes = 1) ;
			int  SetODBCName(CString ds) ;
			int  SetODBCName(int ds) ;
			int  SetDAOName(CString ds) ;
			int Close(int OK = 1) ;
   } ;

// BEGIN_CLASS_HELP
// ClassName: UISqlServerLogIn
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UISqlServerLogIn : public UIDialog 
	{
    UIWND_COPY_CTOR (UISqlServerLogIn, UIDialog) ; 

// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return ("SQL Server Login"); }		// Options

			int ID(CString ID) ;
			int Pw(CString pw) ;        
			int Close(int OK = 1) ;             
 } ; 

 // BEGIN_CLASS_HELP
// ClassName: UISelectTables
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UISelectTables : public UIDialog 
	{
    UIWND_COPY_CTOR (UISelectTables, UIDialog) ; 

// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return ("Select Database Tables"); }		// Options

			int Name(CString tbName) ;
			int Name(int index = 1) ;
			int Close(int OK = 1) ;             
 } ; 

// BEGIN_CLASS_HELP
// ClassName: UIDataBaseDlgs
// BaseClass: None
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIDataBaseDlgs
{
	CString m_DBasePath ;

	public:
	 UISqlDataSources Sources ;
	 UISqlServerLogIn login ;
	 UISelectTables	  tables ;
	int WaitForSources() ;
	 
	int WaitForLogin();
	int WaitForTables() ;
	void SetDataBasePath(CString Path)
	{
		m_DBasePath = Path ;
	}
	CString GetDataBasePath()
	{
		return m_DBasePath ;
	}


	 
} ;

// BEGIN_CLASS_HELP
// ClassName: UISelectRecordSet
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UISelectRecordSet : public UIDialog 
	{
    UIWND_COPY_CTOR (UISelectRecordSet, UIDialog) ; 

// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return ("Select a Record Set"); }		// Options

			int Class(CString Name) ;
			UIDataBaseDlgs *New() ;
			int Close(int OK = 1) ;
			             
 } ; 



#endif //__ODBCDLGS_H__          

