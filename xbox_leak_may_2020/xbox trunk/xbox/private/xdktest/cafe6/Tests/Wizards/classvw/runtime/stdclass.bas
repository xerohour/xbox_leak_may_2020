class CAboutDlg
	base; CDialog; public
	mf; none; CAboutDlg; public
	mf; void; DoDataExchange; protected
***
class CChildFrame
	base; CMDIChildWnd; public
	mf; none; CChildFrame; public
	mf; none; ~CChildFrame; public
	mf; BOOL; PreCreateWindow; public
	mf; void; AssertValid; public
	mf; void; Dump; public
***
class CMainFrame
	base; CMDIFrameWnd; public
	mf; none; CMainFrame; public
	mf; BOOL; PreCreateWindow; public
	mf; none; ~CMainFrame; public
	mf; void; AssertValid; public
	mf; void; Dump; public
	mf; afx_msg int; OnCreate; protected
	md; CStatusBar; m_wndStatusBar; protected
	md; CToolBar; m_wndToolBar; protected
***
global theApp
***
