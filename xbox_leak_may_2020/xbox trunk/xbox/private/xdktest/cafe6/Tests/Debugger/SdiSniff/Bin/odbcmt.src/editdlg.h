// EditSPDlg.h : header file
//
#include "ODBCDoc.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CEditSPDlg dialog

class CEditSPDlg : public CDialog
{
// Construction
public:
	CEditSPDlg(CODBCMTDoc *pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditSPDlg)
	enum { IDD = IDD_EDITSP };
	CComboBox	m_DSNCombo;
	CEdit	m_SPEdit;
	UINT	m_Delay;
	UINT	m_NoThreads;
	CString	m_SPName;
	//}}AFX_DATA

	CODBCMTDoc *m_pDoc;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditSPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditSPDlg)
	afx_msg void OnApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDsn();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
