//
// targetdlg.h
//
// Defines CTargetDlg class, target dialog for project target manipulation
// Implementation is in : targetdlg.cpp
// 
// History:
// Date				Who					What
// 12/14/93			ignatius			created
//////////////////////////////////////////////////////////////////

#ifndef _TARGETDLG_H_
#define _TARGETDLG_H_

#include "resource.h"
#include "prjconfg.h"
#include <dlgbase.h>	// C3dDialog

typedef enum ActionType {Add, Rename, Delete};

class CAction : public CObject
{
// Operations
public:
	CAction() {m_fRedundant = FALSE;}
	virtual BOOL Perform( CProject *pProject ) = 0;
	ActionType m_act_type;
	BOOL m_fRedundant;
};

class CNewAction : public CAction
{
// Construction
public:
	CNewAction( LPCTSTR lpszName, LPCTSTR lpszType, BOOL bDebug, LPCTSTR lpszClone,
				LPCTSTR lpszCopyFiles, LPCTSTR lpszMirrorFiles ) : CAction()
	{
		m_strName = lpszName; m_strType = lpszType;
		m_bDebug = bDebug; m_strCloneFrom = lpszClone;
		m_strCopyFilesFrom = lpszCopyFiles; m_strMirrorFilesFrom = lpszMirrorFiles;
		m_act_type = Add;
	}

// Operations
public:
	virtual BOOL Perform( CProject *pProject );

// Attributes
public:
	CString m_strName;
	CString m_strType;
	BOOL		m_bDebug;
	CString m_strCloneFrom;
	CString m_strCopyFilesFrom;
	CString m_strMirrorFilesFrom;
};

class CRenameAction : public CAction
{
// Construction
public:
	CRenameAction( LPCTSTR lpszOldName, LPCTSTR lpszName ) : CAction()
	{
		m_strFrom = lpszOldName;
		m_strTo   = lpszName;
		m_act_type = Rename;
	}

// Operations
public:
	virtual BOOL Perform( CProject *pProject );

// Attributes
public:
	CString m_strFrom;
	CString m_strTo;
};

class CDeleteAction : public CAction
{
// Construction
public:
	CDeleteAction( LPCTSTR lpszName, LPCTSTR lpszDecoratedName ) : CAction()
	{
		m_strName = lpszName;
		m_strDecoratedName = lpszDecoratedName;
		m_act_type = Delete;
	}

// Operations
public:
	virtual BOOL Perform( CProject *pProject );

// Attributes
public:
	CString m_strName;
	CString m_strDecoratedName;
};

// limit the number of characters in the target
#define MAX_TARGET_SIZE		128

/////////////////////////////////////////////////////////////////
// CTargetDlg class & CSelectTargetDlg class
// definition for Target Dialogs
//////////////////////////////////////////////////////////////////

class CNewTargetDlg;
class CProjectTarget;

class CBaseTargetDlg : public C3dDialog
{
//  Construction 
public:
	CBaseTargetDlg (UINT nIDTemplate, CWnd * pParent)
		: C3dDialog (nIDTemplate, pParent) { 
		m_plstFilterTargs = NULL; 
		m_pProject = NULL;
		m_pTarget = NULL;
	}
	virtual ~CBaseTargetDlg();
 	 
// Dialog Data
	//{{AFC_DATA(CBaseTargetDlg)
	CListBox m_TargetList;
	CWnd m_Platform;
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBaseTargetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_MSG(CBaseTargetDlg)
	virtual BOOL OnInitDialog();
	virtual int DoModal(CString * pstrConfigName = NULL);
	afx_msg VOID UpdateTypeDisplay();
	afx_msg void OnOK();
	//}}AFX_MSG

protected:
	CProject *m_pProject;
	CProjectTarget *m_pTarget;
 	CObList m_ProjectTargets;
	CPtrList m_lstAvailableTargets;

	virtual BOOL ShowTarget(LPCTSTR lpszName);
	BOOL TargetFound(LPCTSTR lpszName);
	BOOL IsDebug(ConfigurationRecord *pConfig);

	CStringList * m_plstFilterTargs;
};

class CSelectTargetDlg : public CBaseTargetDlg
{
//	Construction
public:
	CSelectTargetDlg (CWnd * pParent = NULL, UINT nIDTemplate = CSelectTargetDlg::IDD, long hId = (long)-1, 
					 CStringList * plstFilterTargs = NULL, BOOL fShowSamePlatform = TRUE)
		: CBaseTargetDlg (nIDTemplate, pParent)
		{
			m_plstFilterTargs = plstFilterTargs;
			m_fShowSamePlatform = fShowSamePlatform;
            SetHelpID(hId);
		 } 
	virtual ~CSelectTargetDlg();

// Dialog Data
	//{{AFC_DATA(CSelectTargetDlg)
	enum {IDD = IDD_SELECT_TARGET};
 	//}}AFX_DATA

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_MSG(CSelectTargetDlg)
	afx_msg void OnOK();
	afx_msg void OnCancel();																		  
	//}}AFX_MSG	

	virtual BOOL OnInitDialog();

	CString m_strSelectedTarget;

protected:
	virtual BOOL ShowTarget(LPCTSTR lpszName);

private:
	BOOL m_fShowSamePlatform;	// Do we filter to just projects of the same platform
								// as the active project
};

#endif // _TARGETDLG_H_
