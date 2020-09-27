/////////////////////////////////////////////////////////////////////////////
// aplogdlg.h
//
// email	date		change
// briancr	02/06/95	created
//
// copyright 1994 Microsoft

// Interface of the CAppLogDlg class

/////////////////////////////////////////////////////////////////////////////
// CAppLogDlg dialog

#include "caferes.h"
#include "applog.h"

class CAppLogDlg : public CDialog
{
// Construction
public:
	CAppLogDlg(CAppLog* pAppLog = NULL, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCAFELogDlg)
	enum { IDD = IDD_AppLog };
	CString m_strAppLog;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCAFELogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnInitDialog(void);
	virtual void OnCopy(void);
	virtual void OnClear(void);

// data
protected:
	CAppLog* m_pAppLog;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCAFELogDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
