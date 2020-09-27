#if !defined(AFX_FILENEWDLG_H__0A8494DE_8B10_4113_A5FA_17D6F08CD8F8__INCLUDED_)
#define AFX_FILENEWDLG_H__0A8494DE_8B10_4113_A5FA_17D6F08CD8F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileNewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileNewDlg dialog

class CFileNewDlg : public CDialog
{
// Construction
public:
	CFileNewDlg(CWnd* pParent = NULL);   // standard constructor
	~CFileNewDlg();

// Dialog Data
	//{{AFX_DATA(CFileNewDlg)
	enum { IDD = IDD_FILE_NEW };
	CButton	m_checkUseDefaultNames;
	CEdit	m_editName;
	CListBox	m_lstbxTypes;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileNewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillTypeListBox();

	// Generated message map functions
	//{{AFX_MSG(CFileNewDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckUseDefaultNames();
	afx_msg void OnDoubleClickedCheckUseDefaultNames();
	afx_msg void OnDblClkLstbxTypes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString			m_strTempName;

public:
	int				m_nNbrSelItems;
	CDocTemplate**	m_apTemplate;
	CString			m_strDefaultName;	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILENEWDLG_H__0A8494DE_8B10_4113_A5FA_17D6F08CD8F8__INCLUDED_)
