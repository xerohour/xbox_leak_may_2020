/////////////////////////////////////////////////////////////////////////////
// envpage.h
//
// email	date		change
// briancr	11/04/94	created
//
// copyright 1994 Microsoft

// Interface of the CEnvPage class

#include "resource.h"
#include "settings.h"

/////////////////////////////////////////////////////////////////////////////
// CEnvPage dialog

class CEnvPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CEnvPage)

// Construction
public:
	CEnvPage(CSettings* pSettings = NULL);
	~CEnvPage();

// Dialog Data
	//{{AFX_DATA(CEnvPage)
	enum { IDD = IDD_ToolsetEnvironment };
	CListBox	m_lbEnv;
	CComboBox	m_cbEnvvars;
	CString		m_strEnvvar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEnvPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnSetActive(void);
	virtual void OnOK(void);

// operations (internal)
protected:
	BOOL FillEnvvarList(void);
	BOOL SetEnvvar(void);

// data
protected:
	CSettings* m_pSettings;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEnvPage)
	afx_msg void OnEnvvarEdit();
	afx_msg void OnEnvvarsAdd();
	afx_msg void OnEnvvarsMoveUp();
	afx_msg void OnEnvvarsRemove();
	afx_msg void OnEnvvarsMoveDown();
	afx_msg void OnSelchangeEnvvar();
	afx_msg void OnSelchangeEnvvars();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CAddDlg dialog

class CAddDlg : public CDialog
{
// Construction
public:
	CAddDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddDlg)
	enum { IDD = IDD_AddDlg };
	CString	m_strNewValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
