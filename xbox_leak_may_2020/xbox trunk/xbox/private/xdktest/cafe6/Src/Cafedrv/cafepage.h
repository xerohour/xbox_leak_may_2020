/////////////////////////////////////////////////////////////////////////////
// cafepage.h
//
// email	date		change
// briancr	11/03/94	created
//
// copyright 1994 Microsoft

// Interface of the CCAFEPage class

#include "settings.h"

/////////////////////////////////////////////////////////////////////////////
// CCAFEPage dialog

class CCAFEPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCAFEPage)

// Construction
public:
	CCAFEPage(CSettings* pSettings = NULL);
	~CCAFEPage();

// Dialog Data
	//{{AFX_DATA(CCAFEPage)
	enum { IDD = IDD_CAFETab };
	int		m_bStdToolbar;
	int		m_bSubSuiteToolbar;
	int		m_bViewportToolbar;
	int		m_bStatusBar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCAFEPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnSetActive(void);
	virtual void OnOK(void);

// data
protected:
	CSettings* m_pSettings;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCAFEPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
