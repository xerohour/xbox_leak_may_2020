// findtool.h : header file
//


class CFiltersWindowDlgTab;
class CFindWindowDlg;
class CSearchWindowDlg;
class CSearchMessageDlg;
class CSearchWindowDlgTab;
class CSearchMessageDlgTab;

/////////////////////////////////////////////////////////////////////////////
// CFindToolIcon

class CFindToolIcon : public CStatic
{
// Construction
public:
	CFindToolIcon() {}

// Attributes
public:

// Operations
public:
	void Initialize(CFiltersWindowDlgTab* pFiltersWindowDlgTab);
	void Initialize(CFindWindowDlg* pFindWindowDlg);
	void Initialize(CSearchWindowDlg* pSearchWindowDlg);
	void Initialize(CSearchMessageDlg* pSearchMessageDlg);

// Implementation
public:
	virtual ~CFindToolIcon() {}

protected:
	void DrawWindowFrame(HWND hwnd);
	void StartFlash(HWND hwnd);
	void StopFlash();

	CFiltersWindowDlgTab* m_pFiltersWindowDlgTab;
	CFindWindowDlg* m_pFindWindowDlg;
	CSearchWindowDlg* m_pSearchWindowDlg;
	CSearchMessageDlg* m_pSearchMessageDlg;
	HWND m_hwndFlash;
	HWND m_hwndSubjectLast;
	BOOL m_fHilite;

	// Generated message map functions
protected:
	//{{AFX_MSG(CFindToolIcon)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
