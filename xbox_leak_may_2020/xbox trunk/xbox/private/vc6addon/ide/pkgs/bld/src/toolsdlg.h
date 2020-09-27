//
//
// CToolsDlg, CCustomBuildPage
//
// Tools dialog for adding,editing and deleting
// tools across platforms. Can be per-builder.
//
// Custom Build page for Project.Settings.
//
// [matthewt]
//

#ifndef _INCLUDE_TOOLSDLG_H
#define _INCLUDE_TOOLSDLG_H

#include <utilctrl.h>	// menu buttons

/*
R.I.P. for v4.0 with VBA?
//----------------------------------------------------------------
// CToolsDlg
// Allows adding, editing and deleting of tools.
//----------------------------------------------------------------

class CToolsDlg : public C3dDialog
{
	DECLARE_DYNAMIC(CToolsDlg)

public:
	CToolsDlg(CProject * pProject, CWnd * pwndParent = NULL) :
		C3dDialog(IDD_CUSTOM_TOOLS, pwndParent)
	{
		m_pProject = pProject;	// the builder in question
		m_fGlobal = TRUE;		// global tools shown by default
	}

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual void OnOK(); 

	//{{AFX_MSG(CToolsDlg)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnDestroy();

	afx_msg void OnNewTool();
	afx_msg void OnEditTool();
	afx_msg void OnDeleteTool();
	afx_msg void OnSelectTool();
	afx_msg void OnCheckGlobal();
	//}}AFX_MSG

private:
	// fill the custom tools list
	void FillToolsList();

	// local copies of custom tools lists
	CPtrList m_lstGblTools;		// global custom tools list
	CPtrList m_lstBldTools;		// builder-specific tools list

	// currently selected tool
	CCustomTool * m_pToolFocus;

	// builder in question
	CProject * m_pProject;

	// global?
	BOOL m_fGlobal;
};

// CEditToolDlg
// Allows edigin of a tool.

class CDirMacroMenu : public CMenuBtn
{
public:
	CDirMacroMenu();
};

class CFileMacroMenu : public CMenuBtn
{
public:
	CFileMacroMenu();
};

class CEditToolDlg : public C3dDialog
{
	DECLARE_DYNAMIC(CEditToolDlg)

public:
	CEditToolDlg(CCustomTool * pTool, CWnd * pwndParent = NULL);

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM, LPARAM);

	//{{AFX_MSG(CEditToolDlg)
	afx_msg void OnBrowse();
	//}}AFX_MSG

	//{{AFX_VIRTUAL(CEditToolDlg)
	virtual void DoDataExchange(CDataExchange * pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

private:
	// our data source
	CCustomTool * m_pTool;

	// our macro buttons
	CDirMacroMenu m_btnDirMacro;
	CFileMacroMenu m_btnFileMacro;
};
*/

//----------------------------------------------------------------
// primitive custom build 'tool macros'
//----------------------------------------------------------------

#define IDMACRO_INPTDIR		IDM_BLDTOOL_DIRMACRO1
#define IDMACRO_INTDIR		IDM_BLDTOOL_DIRMACRO2 
#define IDMACRO_OUTDIR		IDM_BLDTOOL_DIRMACRO3
#define IDMACRO_WKSPDIR		IDM_BLDTOOL_DIRMACRO4
#define IDMACRO_PROJDIR		IDM_BLDTOOL_DIRMACRO5
#define IDMACRO_TARGDIR		IDM_BLDTOOL_DIRMACRO6
#define IDMACRO_IDEDIR		IDM_BLDTOOL_DIRMACRO7
#define IDMACRO_REMOTEDIR	IDM_BLDTOOL_DIRMACRO8

#define IDMACRO_WKSPBASE	IDM_BLDTOOL_FILEMACRO1
#define IDMACRO_TARGFILE	IDM_BLDTOOL_FILEMACRO2
#define IDMACRO_TARGBASE	IDM_BLDTOOL_FILEMACRO3
#define IDMACRO_INPTFILE	IDM_BLDTOOL_FILEMACRO4
#define IDMACRO_INPTBASE	IDM_BLDTOOL_FILEMACRO5
#define IDMACRO_REMOTEFILE	IDM_BLDTOOL_FILEMACRO6

#define IDMACRO_FIRST			IDMACRO_INPTDIR
#define IDMACRO_LAST			IDMACRO_REMOTEFILE

// map a macro id (IDM_) to the macro name
extern BOOL MapMacroIdToName(UINT idMacro, const TCHAR * & pchName);

// map a macro name to a macro id (IDM_)
extern BOOL MapMacroNameToId(const TCHAR * pchName, int & cchName, UINT & idMacro);

// set macro name usage
extern void SetMacroIdUsage(BOOL fUsage = TRUE, UINT idMacro = (UINT)-1);

// get macro name usage
extern BOOL GetMacroIdUsage(UINT idMacro);

// get the value of a macro
extern BOOL GetMacroValue(UINT idMacro, CString & strMacroValue, CActionSlob * pAction);

// expand a macro string
// if 'pstrOut' is (CString *)NULL the function wil just test the usage (syntax+semantics)
extern BOOL ExpandMacros(CString * pstrOut, const TCHAR * pchIn, CActionSlob * pAction, BOOL bNoEnvVars = FALSE );


//----------------------------------------------------------------
// our 'Custom Build' option property page
//----------------------------------------------------------------

class CCustomBuildPage : public CSlobPageTab
{
	DECLARE_DYNAMIC(CCustomBuildPage)
	DECLARE_IDE_CONTROL_MAP();

public:
	CCustomBuildPage();

	// set and get string props into a grid control 
	// in our case these are the build commands and output grid ctrls
	BOOL GetStrPropFromGrid(UINT idProp, UINT idGridCtl);
	BOOL SetStrPropToGrid(UINT idProp, UINT idGridCtl);

	// from CSlobPageTab
	virtual void InitPage();
	virtual BOOL Validate();
#if 0
	virtual void CommitTab();
#endif
    virtual BOOL ValidateTab ();

protected:
	BOOL OnPropChange(UINT idProp);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM, LPARAM);

	//{{AFX_MSG(CCustomBuildPage)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnUserDefinedDeps();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnActivateEditCmds();
	afx_msg void OnActivateEditOutput();
	//}}AFX_MSG

private:

	// string list grid for build command(s)
//	CStringListGridWnd_WithChange m_gridCmds;

	// string list grid for output file(s)s
//	CStringListGridWnd_WithChange m_gridOutput;

	CEdit m_editCmds;
	CEdit m_editOutput;

	// last list grid with the focus (NULL if none)
	// CStringListGridWnd * m_pgridLastFocus;
	CEdit * m_pEditLastFocus;

	// our macro buttons
	CMenuBtn m_btnDirMacro;
	CMenuBtn m_btnFileMacro;

	BOOL m_bChanged;
	BOOL m_bDontDeactivate;

	CStringArray m_strArryUserDeps;
};

//
// User-defined dependencies dialog.
//
class CUserDepsDlg: public C3dDialog
{
//  Construction 
public:
 	CUserDepsDlg ( CStringArray& strArryUserDeps, CWnd * pParent = NULL );

// Dialog Data
	//{{AFC_DATA(CUserDepsDlg)
	enum { IDD = IDD_USER_DEPS } ;
	//}}AFX_DATA


// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	BOOL CUserDepsDlg::OnCommand(WPARAM wParam, LPARAM lParam);

public:
	//{{AFX_MSG(CUserDepsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG

// private data
private:
	CStringListGridWnd_WithChange m_gridUserDeps;
	CStringArray& m_strArryUserDeps;
	BOOL m_bChanged;
};

#endif // _INCLUDE_TOOLSDLG_H
