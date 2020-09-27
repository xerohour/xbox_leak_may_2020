///////////////////////////////////////////////////////////////////////////////
//	UAPPWIZ.H
//
//	Created by :			Date :
//		Ivanl				10/14/94
//
//	Description :
//		Declaration of the UIProjectWizard class; Any wizard which creates a project
//

#ifndef __UPROJWIZ_H__
#define __UPROJWIZ_H__

#include "..\..\udialog.h"
#include "..\sym\vproj.h"
#include "..\shl\newfldlg.h"
#include "Strings.h"
#include "prjxprt.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif

#include <afxtempl.h>
///////////////////////////////////////////////////////////////////////////////
//	UIProjectWizard class

// The following enumeration lists the types of projects that can currently
// be opened by the IDE. Each item in this enum corresponds to a string ID,
// which in-turn identifies a string that matches a project type.  There are
// actually two sets here, one for the COProject class and one for the 
// UIProjectWizard.  This was done so as to not break any tests, by removing
// any of the previously defined symbols.  The two sets exists here rather 
// than split across two headers for easier maintenance.  However, if a 
// restructuring of CAFE ever occurs, this should definitely be reconsidered. 
enum PROJTYPE 
{		
		UIAW_PT_APPWIZ = IDSS_PROJTYPE_APPWIZ_APP,
		UIAW_PT_APPWIZ_DLL = IDSS_PROJTYPE_APPWIZ_DLL,
		UIAW_PT_OCX = IDSS_PROJTYPE_OLE_CONTROLWIZ,		
		UIAW_PT_APP = IDSS_PROJTYPE_APPLICATION,
		UIAW_PT_DLL = IDSS_PROJTYPE_DLL,
		UIAW_PT_CONSOLE = IDSS_PROJTYPE_CONSOLE,
		UIAW_PT_STATIC_LIBRARY = IDSS_PROJTYPE_STATIC_LIB,
		UIAW_PT_QUICKWIN = IDSS_PROJTYPE_QUICKWIN,
		UIAW_PT_STD_GRAPH = IDSS_PROJTYPE_STD_GRAPH,
		UIAW_PT_GENPROJ = IDSS_PROJTYPE_GENPROJECT,
		UIAW_PT_EXT_TARG = IDSS_PROJTYPE_EXT_TARG,
		UIAW_PT_CUST_APPWIZ = IDSS_PROJTYPE_CUST_APPWIZ,
		UIAW_PT_JAVA_PROJECT = IDSS_PROJTYPE_JAVA_PROJECT,
		UIAW_PT_JAVA_APPWIZ = IDSS_PROJTYPE_JAVA_APPWIZ,
		UIAW_PT_MAKEFILE = IDSS_PROJTYPE_EXT_TARG,
		UIAW_PT_XBOX = IDSS_PROJTYPE_XBOX_GAME,
		UIAW_PT_XBOX_LIB = IDSS_PROJTYPE_XBOX_LIB,

		APPWIZ_EXE = IDSS_PROJTYPE_APPWIZ_APP,
		APPWIZ_DLL = IDSS_PROJTYPE_APPWIZ_DLL,
		APPWIZ_OCX = IDSS_PROJTYPE_OLE_CONTROLWIZ,			
		EXE = IDSS_PROJTYPE_APPLICATION,		
		DLL = IDSS_PROJTYPE_DLL,		
		CONSOLE_APP = IDSS_PROJTYPE_CONSOLE,	
		STATIC_LIBRARY = IDSS_PROJTYPE_STATIC_LIB,
		DATABASE_PROJECT = IDSS_PROJTYPE_DATABASE,
		ACTIVEX_CONTROL = IDSS_PROJTYPE_OLE_CONTROLWIZ,
		GENERIC_PROJECT = IDSS_PROJTYPE_GENPROJECT,
		ATL_COM = IDSS_PROJTYPE_ATL_COM_WIZ,
		ISAPI_EXTENSION = IDSS_PROJTYPE_ISAPI_EXTENSION,
		XBOX_PROJECT = IDSS_PROJTYPE_XBOX_GAME,
		XBOX_LIB_PROJECT = IDSS_PROJTYPE_XBOX_LIB
};

// for inserting projects.
enum PROJ_HIERARCHY {TOP_LEVEL_PROJ, SUB_PROJ};

// for creating projects.
enum PROJ_WORKSPACE {CREATE_NEW_WORKSPACE, ADD_TO_CURRENT_WORKSPACE};

// BEGIN_CLASS_HELP
// ClassName: UIProjectWizard
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIProjectWizard : public UIDialog
{
	UIWND_COPY_CTOR(UIProjectWizard, UIDialog);
	virtual void OnUpdate(void);

// Data
public:
BOOL IsValid() ;
UINewFileDlg m_FlNewDlg ;

protected:
	int m_nPage;
//	CTypedPtrMap<CMapPtrToPtr, WORD, CString> m_CurrPage ;
	CMap<WORD, WORD &, CString, LPCSTR> m_CurrPage ;
	CPtrArray m_pgArr ;


// Utilities
public:
	inline virtual CString ExpectedTitle(void) const
		{	return ExpectedTitle(-1); }
	virtual CString ExpectedTitle(int nPage) const;

	virtual void Initialize() { };
	int NextPage(void);
	int NextPage(int);	//Specify a specific control ID For WaitWndWithCtrl
	int PrevPage(void);
	int Finish(void);
	int Finish(int);	//Specify a specific control ID For WaitWndWithCtrl
	virtual HWND Cancel(void);

	void SetProjType(PROJTYPE);
	void SetProjType(CString Name); // Name as it shows up in the combobox.
	void SetName(LPCSTR szName);
	void SetHierarchy(PROJ_HIERARCHY phProjHier);
	void SetTopLevelProj(LPCSTR szProjName);
	void SetLocation(LPCSTR szPath); // simpler, more up-to-date version of SetDir().
	void SetDir(LPCSTR szName);
	void SetSubDir(LPCSTR szName);
	void SetPlatform(int iPlatform) ;
	void SetAddToWorkspace(BOOL AddToWorkspace);
	virtual HWND Create(void);
	virtual BOOL ConfirmCreate(void);
	  
	// Helper functions
public:
	virtual WORD GetCurrentPage() ;
	virtual WORD GoToPage(int ID) ;
	virtual int GetPage(HWND hwnd = NULL);
	virtual int UpdatePage(void);
	virtual void ExpectPage(int nPage);
	HWND GetSubDialog(void);
	LPCSTR GetLabel(UINT id);
};

#endif //__UPROJWIZ_H__
