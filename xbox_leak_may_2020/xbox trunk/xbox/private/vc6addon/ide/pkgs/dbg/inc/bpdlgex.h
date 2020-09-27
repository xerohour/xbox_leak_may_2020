//
// bpdlg.h
//
// class declarations for tabbed breakpoint dialog
//

#ifndef INCLUDE_BPDLG_H
#define INCLUDE_BPDLG_H

#include <dlgbase.h>
#include <utilctrl.h>
#include <shlmenu.h>

#define IDC_BP_LIST  		2000
#define IDC_BP_GOTOCODE		2001
#define IDC_BP_REMOVE 		2002
#define IDC_BP_REMOVE_ALL	2003

#define LOCATION_TAB		0
#define DATA_TAB			1
#define MESSAGES_TAB		2


class CLocationTab : public CDlgTab
{
protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CLocationTab)

	friend class CBPDlgEx;
	
// Construction
public:
	CLocationTab();

	// Dialog Data
	//{{AFX_DATA(CLocationTab)
	enum { IDD = IDD_BPTAB_LOCATION };
	CEdit	m_edLocation;
	//}}AFX_DATA

	CMenuBtn m_butLocMenu;
	CString m_strCurAddr;
	CString m_strCurLine;
	CString m_strCurFile;	// used with current line when disasm wnd is active
	CString m_strCurFunc;

	BOOL m_fInitialized;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocationTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CLocationTab)
	afx_msg void OnChangeLocation();
	afx_msg void OnCondition();
	afx_msg void OnCurrentAddr();
	afx_msg void OnCurrentLine();
	afx_msg void OnCurrentFunc();
	afx_msg void OnAdvanced();
	//}}AFX_MSG

// Implementation
protected:

	virtual BOOL OnInitDialog();
	virtual BOOL ValidateTab();
	void 		 InitializeLocationMenu();
	void		 SetBPFields();
	virtual BOOL PreTranslateMessage(MSG *pmsg);
};

class CDataTabEx : public CDlgTab
{
protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CDataTabEx)

	friend class CBPDlgEx;
	
// Construction
public:
	CDataTabEx();

	// Dialog Data
	//{{AFX_DATA(CDataTabEx)
	enum { IDD = IDD_BPTAB_DATA1 };
	//CEdit	m_edLocation;
	CEdit	m_edExpr;
	//CStatic	m_txtLength;
	CEdit	m_edLength;
	//}}AFX_DATA

	CMenuBtn m_butExprMenu;
	BOOL m_fInitialized;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataTabEx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CDataTabEx)
	afx_msg void OnChangeExpression();
	afx_msg void OnChangeLength();
	afx_msg void OnChangeLocation();
	afx_msg void OnAdvanced();
	//afx_msg void OnClickedChanges();
	//afx_msg void OnClickedTrue();
	//}}AFX_MSG
	
	virtual BOOL 	OnInitDialog();
	virtual BOOL	ValidateTab();
	void			SetBPFields();
	virtual BOOL	PreTranslateMessage(MSG *pmsg);
};

class CMessagesTabEx : public CDlgTab
{
protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CMessagesTabEx)

	friend class CBPDlgEx;
	
// Construction
public:
	CMessagesTabEx();

	// Dialog Data
	//{{AFX_DATA(CMessagesTabEx)
	enum { IDD = IDD_BPTAB_MESSAGES };
	CComboBox	m_lstWndProc;
	CComboBox	m_lstMessage;
	//}}AFX_DATA

	BOOL m_fInitialized;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessagesTabEx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMessagesTabEx)
	afx_msg void OnDropdownMessage();
	afx_msg void OnDropdownWndproc();
	afx_msg void OnChangeWndProc();
	afx_msg void OnChangeMessage();
	afx_msg void OnSelchangeMessage();
	afx_msg void OnSelchangeWndproc();
	//}}AFX_MSG
	virtual BOOL 	OnInitDialog();
	virtual BOOL	ValidateTab();
	void			SetBPFields();
	virtual BOOL	PreTranslateMessage(MSG *pmsg);
};

class CBPListEx : public CCheckList
{
protected:

	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
	virtual void SetCheck(int nItem, int nCheck);
	virtual int  GetCheck(int nItem);

protected:
	afx_msg void OnLButtonDown(UINT, CPoint);
};

class CBPDlgEx : public CTabbedDialog
{
	DECLARE_DYNAMIC(CBPDlgEx)
	CBPDlgEx();
	~CBPDlgEx();

public:

	void OnOK();
	void OnCancel();

	virtual void 		GetMargins(CRect & rect);
	virtual void 		CreateButtons();
	virtual void 		MoveButtons(int nLeft, int nCenter);
	virtual CButton*	GetButtonFromIndex(int index);
	virtual void		OnSelectTab(int nTab);
	virtual BOOL 		PreTranslateMessage(MSG *pmsg);

	friend class CLocationTab;
	friend class CDataTabEx;
	friend class CMessagesTabEx;
	//friend class CAdvancedTab;
	friend class CBPListEx;

protected:
	DECLARE_MESSAGE_MAP()

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBPDlgEx)
	protected:
	//}}AFX_VIRTUAL

	// ClassWizard generated message map functions	
	//{{AFX_MSG(CBPDlgEx)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnChangedSel();
	afx_msg	void OnRemoveBP();
	afx_msg	void OnToggleBP();
	afx_msg void OnGotoCode();
	afx_msg	void OnRemoveAllBP();
	//}}AFX_MSG

	CLocationTab* 	m_pdlgLocation;
	CDataTabEx*		m_pdlgData;
	CMessagesTabEx* m_pdlgMessages;
	//CAdvancedTab* m_pdlgAdvanced;

	CStatic			m_lblBP;		// breakpoints label
	CBPListEx		m_lstBP;		// breakpoints list
	CButton			m_butGotoCode;	// goto source button
	CButton			m_butRemove;	// remove breakpoint button
	CButton			m_butRemoveAll;	// remove all breakpoints button
	
	BREAKPOINTTYPES	m_bptypeCur;	// current breakpoint type
	BOOL			m_fCurBPDirty;	// current breakpoint has been modified
	BOOL			m_fSelfChange;	// ignore changes, dialog is manipulating itself
	BOOL			m_fInsertingNew;// true if a new BP is being inserted
	int				m_iItemChanging;// the index of the breakpoint being added or edited

	char 			m_szLocation[cbBpCmdMax];
	char 			m_szExpression[cbBpCmdMax];
	char 			m_szLength[20];
	char 			m_szPassCnt[20];
	char			m_szMessage[128];
	//BOOL			m_fChanges;
	BOOL			m_fExpTrue;
	int 			m_iedLocStart;
	int 			m_iedLocEnd;
	int 			m_iedExpStart;
	int 			m_iedExpEnd;

	int				m_itabPrev;

private:
	PBREAKPOINTNODE GetBreakPoint(UINT wItemID);
	void 			FillBPList();
	int  			GetBPNodeFromControls(PBREAKPOINTNODE pBreakpoint);
	void 			AcceptChanges();
	void 			RejectChanges();
	BOOL 			AddReplaceBP();
	void			ClearBPFields();
	void 			SetBPFields();
	void 			UpdateCurrentBP(BREAKPOINTTYPES bptype);
	void 			UpdateListString(char *buf);
	LRESULT 		OnInitMoveFocus(WPARAM wParam, LPARAM lParam);
	BOOL			FCurrentBPIsEmpty();
	BREAKPOINTTYPES	GetCurrentBPType();
	void			HandleBPError(int ierr, BOOL fQuiet);
};

///////////////////////////////////////////////////////////////////////////////
// CBPCondDlg class
///////////////////////////////////////////////////////////////////////////////

class CBPCondDlg : public C3dDialog
{
	DECLARE_MESSAGE_MAP()

// Construction
public:
	CBPCondDlg (CWnd * pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CCallingExeDlg)
	enum { IDD = IDD_BP_DLG_CONDITION };
	//}}AFX_DATA

	//{{AFX_MSG(CCallingExeDlg)
	afx_msg void OnChangeExpression();
	//}}AFX_MSG

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	BOOL	m_fBreakWhenExprTrue;
	CString m_strExpr;
	CString m_strLength;
	CString m_strSkip;
};

///////////////////////////////////////////////////////////////////////////////
// CBPContextDlg class
///////////////////////////////////////////////////////////////////////////////

class CBPContextDlg : public C3dDialog
{
// Construction
public:
	CBPContextDlg (CWnd * pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CCallingExeDlg)
	enum { IDD = IDD_BP_DLG_CONTEXT };
	//}}AFX_DATA

	//{{AFX_MSG(CCallingExeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG

	CString m_strlblWhere;
	CString m_strWhere;
	CString m_strFunc;
	CString m_strSource;
	CString m_strModule;
};

#endif // _INCLUDE_BPDLG_H
