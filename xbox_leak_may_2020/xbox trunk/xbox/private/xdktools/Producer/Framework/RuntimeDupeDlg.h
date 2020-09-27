#if !defined(AFX_RUNTIMEDUPEDLG_H__9EA7A742_707F_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_RUNTIMEDUPEDLG_H__9EA7A742_707F_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RuntimeDupeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRuntimeDupeDlg dialog

class CRuntimeDupeDlg : public CDialog
{
// Construction
public:
	CRuntimeDupeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRuntimeDupeDlg)
	enum { IDD = IDD_RUNTIMEDUPES };
	CStatic	m_staticPrompt;
	CButton	m_btnOK;
	CListBox	m_listbxFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuntimeDupeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Member variables
public:
	CFileNode*	m_pFileNode;

protected:
	CProject*	m_pProject;

// Implementation
protected:
	void AddToList( CFileNode* pFileNode ); 

	// Generated message map functions
	//{{AFX_MSG(CRuntimeDupeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelChangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RUNTIMEDUPEDLG_H__9EA7A742_707F_11D1_89AE_00A0C9054129__INCLUDED_)
