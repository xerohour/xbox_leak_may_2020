///////////////////////////////////////////////////////////////////////////////
//      WORKSPC.H
//              Header file for Workspace options tab.
//

#ifndef __WORKSPC_H__
#define __WORKSPC_H__

#include "resource.h"
#include "utilctrl.h"

class CDockManager;
class CDockWorker;

#define WORKSPACE_VERSION 6

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceDlg dialog

//      CWorkspaceCheckList control.
//              Override CCheckList for check notification to the dialog.

class CWorkspaceCheckList : public CCheckList
{
public:
	virtual void SetCheck(int nItem, BOOL bCheck);
};

class CWorkspaceDlg : public CDlgTab
{
// Construction
public:
	CWorkspaceDlg(CDockManager* pManager, CWnd* pParent = NULL);

	void UpdateMDIState(int nItem);
	CDockWorker* GetWorker(int nIndex = LB_ERR);

	virtual void CommitTab();
	virtual BOOL ValidateTab();
	
// Dialog Data
	//{{AFX_DATA(CWorkspaceDlg)
	enum { IDD = IDDP_OPTIONS_WORKSPACE };
	BOOL    m_bAuto;
	BOOL    m_bStatusbar;
	BOOL    m_bAutosave;
	int         m_iInterval;
	BOOL    m_bClock;
	BOOL    m_bReloadProject;
	int             m_nFiles;
	int             m_nProjects;
	int             m_nWindows;
	BOOL    m_bWindowsSort;
	BOOL    m_bMRUSubmenu;
	BOOL	m_bTraditionalMenu;
	//}}AFX_DATA

	CWorkspaceCheckList m_lbViews;

	CDockManager* m_pManager;
	CObArray m_aViewWorkers;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CWorkspaceDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
#ifdef ENABLE_FILE_AUTOSAVES
	afx_msg void OnClickedAutosaveEnable();
#endif  // ENABLE_FILE_AUTOSAVES
	DECLARE_MESSAGE_MAP()
};

#endif  // __WORKSPC_H__
