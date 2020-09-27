///////////////////////////////////////////////////////////////////////////////
//      UAPPWIZ.H
//
//      Created by :                    Date :
//              DavidGa                                 10/27/93
//
//      Description :
//              Declaration of the UIAppWizard class
//

#ifndef __UIAPPWIZ_H__
#define __UIAPPWIZ_H__

#include "..\sym\appwz.h"
#include "uprojwiz.h"

#include "prjxprt.h"

#ifndef __UPROJWIZ_H__ 
	#error include 'uprojwiz.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//      UIAppWizard class

enum { UIAW_INVALID,                    // it doesn't look like anything of App Wizard's
		UIAW_NEWPROJ,           // New Project dialog
		UIAW_APPTYPE,           // SDI/MDI/Dialog choices
		UIAW_DLGOPTIONS,        // Dialog Project options
		UIAW_DLGFEATURES,       // Dialog Features
		UIAW_DLGCLASSES,        // Dialog Class Names
		UIAW_DBOPTIONS,                 // Database options
		UIAW_OLEOPTIONS,        // OLE options
		UIAW_FEATURES,          // Features
		UIAW_PROJOPTIONS,       // Project options
		UIAW_CLASSES,           // Class Names
		UIAW_CONFIRM,           // Confirmation
		UIAW_PROGRESS,          // Progress dialog
		UIAW_DBSOURCES,                 // Data Sources dialog
		UIAW_MESSAGEBOX };      // Any App Wizard dialog with an OK button

enum { UIAW_APP_SDI = APPWZ_IDC_PTRADIO,
		UIAW_APP_MDI = APPWZ_IDC_PTMDI,
		UIAW_APP_FORM = APPWZ_IDC_PTDLG };

enum { UIAW_DB_NONE = APPWZ_IDC_DB_RADIO,
		UIAW_DB_HEADER = APPWZ_IDC_DB_HEADER,
		UIAW_DB_VIEWONLY = APPWZ_IDC_DB_SIMPLE,
		UIAW_DB_VIEWFILE = APPWZ_IDC_DB_DOCVIEW };

enum { UIAW_OLE_NONE = APPWZ_IDC_NO_INPLACE,
		UIAW_OLE_CONTAINER = APPWZ_IDC_CONTAINER,
		UIAW_OLE_MINI_SERVER = APPWZ_IDC_MSERVER,
		UIAW_OLE_FULL_SERVER = APPWZ_IDC_FSERVER,
		UIAW_OLE_CONTAINER_SERVER = APPWZ_IDC_CSERVER,
		UIAW_OLE_YES_AUTO = APPWZ_IDC_RADIO1,
		UIAW_OLE_NO_AUTO = APPWZ_IDC_AUTOMATION_RADIO };

enum { UIAW_PROJ_COMMENTS = APPWZ_IDCD_POVERBOSE,
		UIAW_PROJ_NOCOMMENTS = APPWZ_IDC_RADIO2,
// REVIEW(briancr): I don't think this is supported for Olympus
//              UIAW_PROJ_EXTERNAL = APPWZ_IDCD_POEXTMAK,
		UIAW_PROJ_INTERNAL = APPWZ_IDC_RADIO3,
		UIAW_PROJ_MFCDLL = APPWZ_IDCD_PODLL,
		UIAW_PROJ_MFCSTATIC = APPWZ_IDCD_PONOTDLL };

// BEGIN_CLASS_HELP
// ClassName: UIAppWizard
// BaseClass: UIProjectWizard
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIAppWizard : public UIProjectWizard
{
	UIWND_COPY_CTOR(UIAppWizard, UIProjectWizard);
	virtual void OnUpdate(void) ;
	operator HWND ( ) {return HWnd();}
	operator BOOL ( ) {return (BOOL) HWnd();} ;
		// Data
public:
	int OleInstalled  ;
	BOOL ODBCInstalled ;
	//Pages in the Wizard.
	WORD pg1, pg2, pg3,pg4, pg5, pg6, confirm;

protected:
// Utilities
public:
	inline virtual CString ExpectedTitle(void) const
		{       return ExpectedTitle(-1); }
	virtual CString ExpectedTitle(int nPage) const;
	
	virtual void Initialize() ;
	virtual HWND Create(void);
									   
	void SetAppType(UINT id);
	void SetDBOptions(UINT id);
	void SetOLEOptions(UINT id);
	void SetOLEOptions2(UINT id);
	void SetProjOptions(UINT id);
	void SetFeatures(UINT id);

	void SelectClass(int n);
	void SelectClass(LPCSTR sz);

	CString GetClassName(void);
	CString GetBaseClass(void);
	CString GetSourceFile(void);
	CString GetHeaderFile(void);

// Helper functions
public:
	virtual int GetPage(HWND hwnd = NULL);
	virtual void ExpectPage(int nPage);
	virtual BOOL IsValid(void);

};

#endif //__UIAPPWIZ_H__
