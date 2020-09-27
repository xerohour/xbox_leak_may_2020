// TipDlg.h : header file
//

#include "tipmgr.h"
#include "tipctrl.h"
#include "totd.h"

/////////////////////////////////////////////////////////////////////////////
// CTipDlg dialog

class CTipDlg : public C3dDialog
{
// Construction
public:
	CTipDlg( CTipManager & ATipManager, CWnd* pParent = NULL );

// Dialog Data
	//{{AFX_DATA(CTipDlg)
	enum { IDD = IDD_TIPOFTHEDAY };
	CButton	m_BNext;
	BOOL	m_bShowTipsAtStartup;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTipManager & m_Tip;
	CTipCtrl m_TipCtrl;
	CRect m_rectFrame;
	int m_margin;
	CString	m_sTip;

	// Generated message map functions
	//{{AFX_MSG(CTipDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTipNext();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

