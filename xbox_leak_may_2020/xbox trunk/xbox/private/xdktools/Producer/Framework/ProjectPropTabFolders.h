#if !defined(AFX_PROJECTPROPTABFOLDERS_H__AE0BAF04_A119_11D0_89AD_00A0C9054129__INCLUDED_)
#define AFX_PROJECTPROPTABFOLDERS_H__AE0BAF04_A119_11D0_89AD_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ProjectPropTabFolders.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CProjectPropTabFolders dialog

class CProjectPropTabFolders : public CPropertyPage
{
// Construction
public:
	CProjectPropTabFolders( CProjectPropPageManager* pPageManager );
	virtual ~CProjectPropTabFolders();
	void SetProject( CProject* pProject );

// Dialog Data
	//{{AFX_DATA(CProjectPropTabFolders)
	enum { IDD = IDD_TAB_PROJECT_FOLDERS };
	CEdit	m_editFolder;
	CListBox	m_lstbxFolders;
	CStatic	m_staticPrompt;
	CButton	m_btnBrowse;
	//}}AFX_DATA

// Attributes
protected:
	CProject*					m_pProject;
	CProjectPropPageManager*	m_pPageManager;
	CString						m_strOrigFolderText;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProjectPropTabFolders)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableControls( BOOL fEnable );
	void UpdateFolder( LPCTSTR szNewFolder );
	void GetDocName( CString& strName );
	void FillFolderListBox();

	// Generated message map functions
	//{{AFX_MSG(CProjectPropTabFolders)
	afx_msg void OnBrowse();
	afx_msg void OnSelChangeFolderList();
	afx_msg void OnKillFocusFolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTPROPTABFOLDERS_H__AE0BAF04_A119_11D0_89AD_00A0C9054129__INCLUDED_)
