// scriptpr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ScriptPropSheet

class ScriptPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(ScriptPropSheet)

// Construction
public:
	ScriptPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	ScriptPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScriptPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~ScriptPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(ScriptPropSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
