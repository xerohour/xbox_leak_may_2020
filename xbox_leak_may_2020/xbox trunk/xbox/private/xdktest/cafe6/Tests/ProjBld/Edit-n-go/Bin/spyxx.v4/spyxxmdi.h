// spyxxmdi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpyMDIChildWnd frame

class CSpyMDIChildWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CSpyMDIChildWnd)
protected:
	CSpyMDIChildWnd();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpyMDIChildWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSpyMDIChildWnd();

	// Generated message map functions
	//{{AFX_MSG(CSpyMDIChildWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
