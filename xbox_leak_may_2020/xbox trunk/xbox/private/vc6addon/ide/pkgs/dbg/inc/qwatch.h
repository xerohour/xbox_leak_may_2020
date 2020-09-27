// ..\inc\qwatch.h : header file
//

// Forward declaration
class CTM;
class CQuickW;
class CQWGrid;

// Defined in qwatch.cpp, these are private to the quick watch implementation.
class CQWGridEdit;
class CQWGridRow;

enum expr_status { expr_valid, expr_invalid, expr_illegal};

/////////////////////////////////////////////////////////////////////////////
// CQWGrid window

class CQWGrid : public CDbgGridWnd
{
// Construction
public:
	CQWGrid();
	typedef CDbgGridWnd CBaseClass;


// Attributes
public:
	CQuickW	*m_pDlg;
	BOOL m_bIsModified;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQWGrid)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQWGrid();

	expr_status AddNewExpression(const CString&);

	virtual BOOL ProcessKeyboard(MSG *pMsg, BOOL bPreTrans = FALSE);
	virtual LRESULT OnUpdateDebug(WPARAM, LPARAM);

	// Generated message map functions
protected:
	//{{AFX_MSG(CQWGrid)
	afx_msg UINT OnGetDlgCode();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CQuickW dialog

class CQuickW : public C3dDialog
{
// Construction
public:
	CQuickW(CWnd* pParent = NULL);   // standard constructor
	typedef C3dDialog CBaseClass ;

	CQWGrid m_qwGrid;
	CString  m_strInit;

	expr_status ReplaceQWExpr(const CString&);

	// Helper functions.
	inline CTM * GetCurTM( );
	inline BOOL IsCurRowEditable();
	CString StripWhiteSpace(const CString& str);

	void ChangeDefButton(UINT nID); // Changes the default push-button, correctly.
	void UpdateControls(UINT nForceId = 0 );		// enable-disable buttons etc.

	// grid control sends this notification when a new row 
	// within the grid control gets selected.
	void OnSelChanged(CQWGridRow *pRow);

// Dialog Data
	//{{AFX_DATA(CQuickW)
	enum { IDD = IDD_QUICKWATCH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuickW)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bIsValidExpr;
	
	// Generated message map functions
	//{{AFX_MSG(CQuickW)
	virtual BOOL OnInitDialog();
	afx_msg void OnQuickwNewexpr();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCBEditChange();
	afx_msg void OnCBSetFocus();
	afx_msg void OnCBSelChange();
	afx_msg void OnQuickwAdd();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// static members.
public:
	static CStringArray s_rgstr;	
};

/////////////////////////////////////////////////////////////////////////////
