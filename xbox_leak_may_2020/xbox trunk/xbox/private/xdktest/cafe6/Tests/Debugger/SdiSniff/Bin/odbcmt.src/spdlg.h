// SPDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewSPDlg dialog

#include "resource.h"

class CNewSPDlg : public CDialog
{
// Construction
public:
	CNewSPDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewSPDlg)
	enum { IDD = IDD_NEWSP };
	CEdit	m_SPEdit;
	UINT	m_NoThreads;
	UINT	m_Delay;
	CString	m_SPName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewSPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewSPDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
