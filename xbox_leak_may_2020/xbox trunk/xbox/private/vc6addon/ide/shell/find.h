#ifndef __DLGBASE_H__
#include <dlgbase.h>	// C3dDialog
#endif

#ifndef __MENUBTN_H__
#include <utilctrl.h>
#endif

void SaveFindReplaceStrings();
void LoadFindReplaceStrings();

/////////////////////////////////////////////////////////////////////////////
// CButtonEdit window
// use CButtonEdit to associate an edit field and a menu button item:
// the edit field will remember the last selection and cursor position

class CButtonEdit : public CEdit
{
// Construction
public:
	CButtonEdit();

// Attributes
public:
	DWORD m_dwLastSel; // last selection on KillFocus

// Methods
public:
	void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE);

// Operations
protected:
	afx_msg void OnKillFocus(CWnd *pNewWnd);

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CFindStringDlg dialog

class CFindStringDlg : public C3dDialog
{
// Construction
public:
	CFindStringDlg(LONG lStyle, CWnd* pParent = NULL);
	~CFindStringDlg();

// Dialog Data
	//{{AFX_DATA(CFindStringDlg)
//	enum { IDD = IDD_FIND };
	BOOL	m_bMatchCase;
	BOOL	m_bRegExp;
	BOOL	m_bWholeWord;
	int		m_nDirection;
	BOOL	m_bSearchAllDocs;
	CString	m_strFindString;
	CButton	m_btnFindNext;
	//}}AFX_DATA

	CMenuBtn m_btnSpecial;
	CButtonEdit m_comboEdit;

	LONG m_lStyle;
	BOOL m_bInitString;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CFindStringDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedFindNext();
	afx_msg void OnClickedMarkAll();
	afx_msg void OnClickedSearchAll();
	afx_msg void OnEditChangeFindWhat();
	afx_msg void OnSelChangeFindWhat();
	afx_msg void OnKillfocusFindWhat();
	afx_msg void OnSpecialItem(UINT nId);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Helper functions
	void UpdateState();
	void UpdateButtons(BOOL fEnable);
};

