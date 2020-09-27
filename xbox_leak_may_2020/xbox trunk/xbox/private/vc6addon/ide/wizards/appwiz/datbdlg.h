// datbdlg.h : header file
//

#ifndef DATBDLG_H
#define DATBDLG_H

#include "seqdlg.h"

#ifndef VS_PACKAGE
#include <clwzapi.h>
#endif	// VS_PACKAGE

/////////////////////////////////////////////////////////////////////////////
// CMyButton window

class CMyButton : public CButton
{
// Construction
public:
	CMyButton();

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CMyButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyButton)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CDBDlg dialog

class CDBDlg : public CSeqDlg
{
// Construction
public:
	CDBDlg(CWnd* pParent = NULL);	// standard constructor
	virtual BOOL CanDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void WriteValues();
	virtual UINT ChooseBitmap();

// Dialog Data
	//{{AFX_DATA(CDBDlg)
	enum { IDD = IDD_DB_OPTIONS };
	int		m_nDBOptions;
	//}}AFX_DATA

// Implementation
protected:
#ifndef VS_PACKAGE
	LPCLASSWIZARD m_lpWiz;
#endif	// VS_PACKAGE
	CMyButton m_btnDataSource;
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void ReadValues();
	void UpdateDatasrcText();	
	void DisableAll();
	void RunDataSource(CDialog* dlg);
	void MakeUserString(CString& str);

	// Generated message map functions
	//{{AFX_MSG(CDBDlg)
	afx_msg void OnClickedDataSource();
	afx_msg BOOL OnClickedDBOption(UINT nID);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
