#if !defined(AFX_DELETEFILEDLG_H__5E922342_3044_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_DELETEFILEDLG_H__5E922342_3044_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DeleteFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeleteFileDlg dialog

class CDeleteFileDlg : public CDialog
{
// Construction
public:
	CDeleteFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteFileDlg)
	enum { IDD = IDD_DELETE_FILE };
	CStatic	m_staticPrompt;
	CListBox	m_listFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Member variables
public:
	CFileNode*	m_pFileNode;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteFileDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEFILEDLG_H__5E922342_3044_11D1_89AE_00A0C9054129__INCLUDED_)
