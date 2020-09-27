class CTestDoc
	base; CDocument; public
	mf; none; CTestDoc; public
	mf; none; ~CTestDoc; public
	mf; BOOL; OnNewDocument; public
	mf; void; Serialize; public
	mf; void; AssertValid; public
	mf; void; Dump; public
***
class CTestView
	mf; none; CTestView; public
	mf; none; ~CTestView; public
	mf; CTestDoc*; GetDocument; public
	mf; void; OnDraw; public
	mf; BOOL; PreCreateWindow; public
	mf; BOOL; OnPreparePrinting; protected
	mf; void; OnBeginPrinting; protected
	mf; void; OnEndPrinting; protected
	mf; void; AssertValid; public
	mf; void; Dump; public
***
class CTestApp
	base; CWinApp; public
	mf; none; CTestApp; public
	mf; BOOL; InitInstance; public
	mf; afx_msg void; OnAppAbout; public
***