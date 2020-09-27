/////////////////////////////////////////////////////////////////////////////
// idepage.h
//
// email	date		change
// briancr	11/10/94	created
//
// copyright 1994 Microsoft

// Interface of the CIDEPage class

#include "resource.h"
#include "settings.h"
#include "target.h"
#include "idetarg.h"

/////////////////////////////////////////////////////////////////////////////
// CIDEPage dialog

class CIDEPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CIDEPage)

// Construction
public:
	CIDEPage(CSettings* pSettings = NULL, CIDE* pIDE = NULL);
	~CIDEPage();

// Dialog Data
	//{{AFX_DATA(CIDEPage)
	enum { IDD = IDD_TargetIDE };
	int		m_bClose;
	int		m_bDebugProcess;
	int		m_bIgnoreASSERT;
	int		m_bBreakASSERT;
	int		m_bNewApp;
	int		m_bUseKey;
	int		m_bPersist;
	CString	m_strCmdLine;
	CString	m_strFilename;
	CString	m_strKey;
	CString	m_strWorkDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIDEPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnSetActive(void);
	virtual void OnOK(void);

// data
protected:
	CSettings* m_pSettings;
	CIDE* m_pIDE;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CIDEPage)
	afx_msg void OnIDEUseKey();
	afx_msg void OnIDEConnections();
	afx_msg void OnIDEDebugProcess();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
