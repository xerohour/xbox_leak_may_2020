//
// miscdlg.h
//
// Defines CRemoteTargetDlg, and CCallingExeDlg classes, these are
// dialogs to obtain information that we don't have but need to complete
// a requested operation (e.g. updating remote target, or debugging)
//
// Implementation is in : miscdlg.cpp
//
// History:
// Date				Who			What
// 03/23/94			colint			created
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MISCDLG_H_
#define _MISCDLG_H_

#include "resource.h"
#include "prjconfg.h"
#include <dlgbase.h>		// C3dDialog

///////////////////////////////////////////////////////////////////////////////
// CRemoteTargetDlg class
// definition for remote target dialog
///////////////////////////////////////////////////////////////////////////////

class CRemoteTargetDlg : public C3dDialog
{
// Construction
public:
	CRemoteTargetDlg (BOOL bBuild = FALSE, CWnd * pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CRemoteTargetDlg)
	enum { IDD = IDD_REMOTE_TARG_NAME };
	//}}AFX_DATA

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_MSG(CRemoteTargetDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	void OnNameChange();
	//}}AFX_MSG

// private data
private:
	BOOL m_bBuild;
};

///////////////////////////////////////////////////////////////////////////////
// CCallingExeDlg class
// definition for remote target dialog
///////////////////////////////////////////////////////////////////////////////

class CCallingExeDlg : public C3dDialog
{
// Construction
public:
	CCallingExeDlg (BOOL bExecute = FALSE, CWnd * pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CCallingExeDlg)
	enum { IDD = IDD_EXE_FOR_DEBUG };
	//}}AFX_DATA

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_MSG(CCallingExeDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	void OnNameChange();
	void OnDebugBrowse();
	void OnUseWebBrowser();
	void OnUseTestContainer();
	//}}AFX_MSG

// private data
private:
	BOOL m_bExecute;
	CMenuBtn m_btnExeFinder;
};

class CDbgBrowseDlg: public C3dFileDialog
{
// Construction
public:
	CDbgBrowseDlg();
	~CDbgBrowseDlg();

// Implementation
	int DoModal();
	CString GetFileName();

// private data
private:
	TCHAR m_szFileBuff[_MAX_PATH];
};


///////////////////////////////////////////////////////////////////////////////
// CJavaDebugInfoDlg class
// definition for Java debug info dialog
///////////////////////////////////////////////////////////////////////////////

class CJavaDebugInfoDlg: public C3dDialog
{
// Construction
public:
	CJavaDebugInfoDlg(BOOL bExecute = FALSE, CWnd * pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CJavaDebugInfoDlg)
	enum { IDD = IDD_JAVA_DEBUG_INFO };
	CButton m_btnBrowser;
	CButton m_btnStandalone;
	CEdit m_editClassFileName;
	CEdit m_editBrowser;
	CEdit m_editStandalone;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJavaDebugInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_MSG(CJavaDebugInfoDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	void OnChangeDebugUsing();
	void OnChange();
	//}}AFX_MSG

// private data
private:
	BOOL m_bExecute;
};

///////////////////////////////////////////////////////////////////////////////
// CPlatformsDlg class
// definition for platforms dialog
///////////////////////////////////////////////////////////////////////////////

class CPlatformsDlg : public C3dDialog
{
// Construction
public:
	CPlatformsDlg (CStringList * plstPlatforms, CWnd * pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CPlatformsDlg)
	enum { IDD = IDD_PLATFORMS };
	//}}AFX_DATA

	CStringList	* m_plstPlatforms;

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_MSG(CPlatformsDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

// private data
private:
	CCheckList m_lbPlatforms;
};

#endif // _MISCDLG_H_
