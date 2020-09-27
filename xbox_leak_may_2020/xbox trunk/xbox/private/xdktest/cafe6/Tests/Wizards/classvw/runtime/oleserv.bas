class CTestApp
	base; CWinApp; public
	mf; none; CTestApp; public
	mf; BOOL; InitInstance; public
	mf; afx_msg void; OnAppAbout; public
	md; COleTemplateServer; m_server; public
***
class CInPlaceFrame
	base; COleIPFrameWnd; pubic
	mf; none; CInPlaceFrame; public
	mf; BOOL; OnCreateControlBars; public
	mf; BOOL; PreCreateWindow; public
	mf; none; ~CInPlaceFrame; public
	mf; void; AssertValid; public
	mf; void; Dump; public
	md; CToolBar; m_wndToolBar; protected
	md; COleResizeBar; m_wndResizeBar; protected
	md; COleDropTarget; m_dropTarget; protected
	mf; afx_msg int; OnCreate; protected
***
class CTestDoc
	base; COleServerDoc; public
	mf; none; CTestDoc; public
	mf; CTestSrvrItem*; GetEmbeddedItem
	mf; COleServerItem*; OnGetEmbeddedItem; protected
	mf; none; ~CTestDoc; public
	mf; BOOL; OnNewDocument; public
	mf; void; Serialize; public
	mf; void; AssertValid; public
	mf; void; Dump; public
***
class CTestSrvrItem 
	base; COleServerItem; public
	mf; none; CTestSrvrItem; public
	mf; CTestDoc*; GetDocument; public
	mf; BOOL; OnDraw; public
	mf; BOOL; OnGetExtent; public
	mf; none; ~CTestSrvrItem; public
	mf; void; AssertValid; public
	mf; void; Dump; public
	mf; void; Serialize; public
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
	mf; void; AssertValid; public
	mf; void; Dump; public
	mf; afx_msg void; OnCancelEditSrvr; protected
***
