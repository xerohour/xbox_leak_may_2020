#if !defined(AFX_TABFILERUNTIME_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_TABFILERUNTIME_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TabFileRuntime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabFileRuntime dialog

class CTabFileRuntime : public CPropertyPage
{
// Construction
public:
	CTabFileRuntime( CFilePropPageManager* pPageManager );
	~CTabFileRuntime();
	void SetFile( CFileNode* pFileNode );

// Dialog Data
	//{{AFX_DATA(CTabFileRuntime)
	enum { IDD = IDD_TAB_FILE_RUNTIME };
	CEdit	m_editRuntimeFile;
	CEdit	m_editRuntimeFileName;
	CStatic	m_staticRuntimeCreate;
	CEdit	m_editRuntimeDir;
	CButton	m_btnRuntimeBrowse;
	CStatic	m_staticRuntimeSize;
	CStatic	m_staticRuntimeModified;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTabFileRuntime)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
protected:
	CFileNode*				m_pFileNode;
	CFilePropPageManager*	m_pPageManager;

// Implementation
protected:
	void EnableControls( BOOL fEnable );
	void GetDefaultFolderText( CString& strDefaultText );

	// Generated message map functions
	//{{AFX_MSG(CTabFileRuntime)
	afx_msg void OnKillFocusRuntimeFileName();
	afx_msg void OnKillFocusRuntimeDir();
	afx_msg void OnRuntimeBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABFILERUNTIME_H__4B3A5402_6B29_11D1_89AE_00A0C9054129__INCLUDED_)
