// logview.h : header file
//
 
#ifndef _LOGVIEW_H
#define _LOGVIEW_H

/////////////////////////////////////////////////////////////////////////////
// CLogView view
typedef enum NodeTypes {INVALID = -1, SUITE,SUBSUITE,TESTCASE,DETAILS,SUMMARY,FAILURE,OTHER}  ;

class CLogView : public CTreeView
{
protected:
	CLogView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CLogView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

	// Tree control data and methods.
	int isFirstSuite , isFirstSubSuite,isFirstTestCase, isFirstDetails,isFirstSummary ;
	TV_ITEM tv_Item, m_stNode, m_substNode,m_tstCaseNode,m_detailNode,m_sumNode,m_otherNode  ;
	TV_INSERTSTRUCT * tv_Suite, * tv_SubSuite, * tv_Case, * tv_Details, *tv_Summary, *tv_Other ;
	HTREEITEM hSuite, hParentSubSuite, hParentSuite , hParentTest; 
	CImageList pImageList ;
public:
	void AddNewNode(NodeTypes iType,int nLines, CString szText) ;
	HTREEITEM GetParentSubSuite() {return hParentSubSuite; } ;
	HTREEITEM GetParentSuite() {return hParentSuite ; } ;
	HTREEITEM GetParentTest()   {return hParentTest ; } ;
	void ReportFailToTest() ;
	void ReportFailToSubSuite() ;
	void ReportFailToSuite() ;


// Implementation
protected:
	virtual ~CLogView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CLogView)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //_LOGVIEW_H
/////////////////////////////////////////////////////////////////////////////
