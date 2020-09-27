#if !defined(AFX_DUPEFILEDLG_H__96FC19CA_87EF_11D3_B473_00105A2796DE__INCLUDED_)
#define AFX_DUPEFILEDLG_H__96FC19CA_87EF_11D3_B473_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DupeFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDupeFileDlg dialog

class CDupeFileDlg : public CDialog
{
// Construction
public:
	CDupeFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDupeFileDlg)
	enum { IDD = IDD_DUPLICATE_FILE };
	CButton	m_btnOKAll;
	CStatic	m_staticPrompt;
	CButton	m_radioUseExisting;
	CButton	m_radioUseEmbedded;
	CButton	m_radioKeepBoth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDupeFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

public:
	CString		m_strPrompt;
	CString		m_strPromptUseExisting;
	CString		m_strPromptUseEmbedded;
	CString		m_strPromptKeepBoth;

	// Generated message map functions
	//{{AFX_MSG(CDupeFileDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnOKAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUPEFILEDLG_H__96FC19CA_87EF_11D3_B473_00105A2796DE__INCLUDED_)
