class CTestApp
	base; CWinApp; public
	mf; none; CTestApp; public
	mf; BOOL; InitInstance; public
	mf; afx_msg void; OnAppAbout; public
***
class CTestCntrItem
	base; COleClientItem
	mf; none; CTestCntrItem; public
	mf; CTestDoc*; GetDocument; public
	mf; CTestView*; GetActiveView; public
	mf; void; OnChange; public
	mf; void; OnGetItemPosition; protected
	mf; void; OnDeactivateUI; protected
	mf; BOOL; OnChangeItemPosition; protected
	mf; none; ~CTestCntrItem; public
	mf; void; AssertValid; public
	mf; void; Dump; public
	mf; void; Serialize; public
***
class CTestDoc
	base; COleDocument; public
	mf; none; CTestDoc; public
	mf; none; ~CTestDoc; public
	mf; BOOL; OnNewDocument; public
	mf; void; Serialize; public
	mf; void; AssertValid; public
	mf; void; Dump; public
***
class CTestView
	base; CView; public
	mf; none; CTestView; public
	mf; none; ~CTestView; public
	mf; CTestDoc*; GetDocument; public
	mf; void; OnDraw; public
	mf; BOOL; PreCreateWindow; public
	mf; BOOL; OnPreparePrinting; protected
	mf; void; OnBeginPrinting; protected
	mf; void; OnEndPrinting; protected
	mf; BOOL; IsSelected; protected
	mf; void; AssertValid; public
	mf; void; Dump; public
	mf; afx_msg void; OnSetFocus; protected
	mf; afx_msg void; OnSize; protected
	mf; afx_msg void; OnInsertObject; protected
	mf; afx_msg void; OnCancelEditCntr; protected
	md; CTestCntrItem*; m_pSelection; public
***
