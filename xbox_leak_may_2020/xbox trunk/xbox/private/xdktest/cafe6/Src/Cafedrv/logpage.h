/////////////////////////////////////////////////////////////////////////////
// logpage.h
//
// email	date		change
// briancr	11/10/94	created
//
// copyright 1994 Microsoft

// Interface of the CLogPage class

#include "settings.h"

/////////////////////////////////////////////////////////////////////////////
// CLogPage dialog

class CLogPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CLogPage)

// Construction
public:
	CLogPage();
	~CLogPage();

// Dialog Data
	//{{AFX_DATA(CLogPage)
	enum { IDD = IDD_LogTab };
	int		m_nMaxFail;
	int		m_bPost;
	int		m_bCom;
	CString		m_strConnector;
	CString		m_strBaudRate;
	int		m_bComments;
	CString	m_strResultsFile;
	CString	m_strSummaryFile;
	CString	m_strWorkDir;
	int		m_bDebugOutput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLogPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnSetActive(void);

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLogPage)
	afx_msg void OnLogToCom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
