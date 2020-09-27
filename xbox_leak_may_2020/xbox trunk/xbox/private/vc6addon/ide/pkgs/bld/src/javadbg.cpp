//
// CDebugJavaGeneralPage
//		The general debugger options page for Java projects. This page
//		lets the user enter the class file to debug, the HTML page
//		containing the applet, and/or the parameters to the applet/application.
//
// CParamGridWnd
//		The grid for entering applet parameters.
//				   

#include "stdafx.h"

#include "javadbg.h"
#include "htmlfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//----------------------------------------------------------------
// CDebugJavaGeneralPage: the page to handle the general options
//----------------------------------------------------------------

BEGIN_IDE_CONTROL_MAP(CDebugJavaGeneralPage, IDDP_JAVA_DEBUG_GENERAL, IDS_DEBUG_OPTIONS)
	MAP_EDIT(IDC_JAVA_CALLING_PROG, P_Java_ClassFileName)
	MAP_RADIO(IDC_JAVA_USE_BROWSER, IDC_JAVA_USE_STANDALONE, 1, P_Java_DebugUsing)
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CDebugJavaGeneralPage, CDebugPageTab)
	//{{AFX_MSG_MAP(CDebugJavaGeneralPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// CDebugJavaGeneralPage::OnInitDialog
BOOL CDebugJavaGeneralPage::OnInitDialog()
{
	if ( !COptionMiniPage::OnInitDialog() ) {
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------
// CDebugJavaGeneralPage::InitPage
void CDebugJavaGeneralPage::InitPage()
{
	COptionMiniPage::InitPage();
}

//----------------------------------------------------------------
// CDebugJavaGeneralPage::CommitPage
void CDebugJavaGeneralPage::CommitPage()
{
	// REVIEW(briancr): do we need to do anything here?
}

//----------------------------------------------------------------
// CDebugJavaGeneralPage::OnPropChange
BOOL CDebugJavaGeneralPage::OnPropChange(UINT idProp)
{
	// we want to enable/disable properties based on property changes

	switch (idProp) {
		case P_Java_ClassFileName: {

			OnChangeClassFileName();

			break;
		}
		case P_Java_DebugUsing: {

			OnChangeDebugUsing();

			break;
		}
	}

	// call our base-class
	return CDebugPageTab::OnPropChange(idProp);
}

//----------------------------------------------------------------
// CDebugJavaGeneralPage::Validate
BOOL CDebugJavaGeneralPage::Validate()
{
	return CDebugPageTab::Validate();
}

void CDebugJavaGeneralPage::OnChangeClassFileName()
{
	// we don't allow the user to enter a path or .class extension
	// (other extensions are allowed because we can't tell the
	// difference between the extension in java.lang.String and
	// an invalid extension)

	// get the class name
	CString strClassName;
	m_pSlob->GetStrProp(P_Java_ClassFileName, strClassName);

	// split the class name into parts
	// (and eliminate any path that's entered)
	TCHAR szFileName[_MAX_PATH];
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(strClassName, NULL, NULL, szFileName, szExt);

	// remove the extension if it's .class
	CString strNewClassName;
	if (_tcscmp(szExt, _T(".class")) == 0) {

		strNewClassName = CString(szFileName);
	}
	else {

		strNewClassName = CString(szFileName) + CString(szExt);
	}

	// only set the class name prop if the new class name is different
	if (strNewClassName != strClassName) {

		// set the class name prop
		m_pSlob->SetStrProp(P_Java_ClassFileName, strNewClassName);
	}
}

void CDebugJavaGeneralPage::OnChangeDebugUsing()
{
	// when the user changes debug using (selects browser or stand-alone),
	// we need to set other properties

	// if debug using browser:
	//   - if P_Java_ParamSource is unknown:
	//       - if P_Java_HTMLPage contains a valid page:
	//           - set P_Java_ParamSource to Java_ParamSource_HTMLPage
	//             (get the params from the HTML page)
	//       - if P_Java_HTMLPage does not contain a page:
	//           - set P_Java_ParamSource to Java_ParamSource_User
	//             (get the params from the user)

	// get debug using
	int nDebugUsing = Java_DebugUsing_Unknown;
	GPT gptDebugUsing = m_pSlob->GetIntProp(P_Java_DebugUsing, nDebugUsing);

	if (gptDebugUsing != ambiguous && nDebugUsing == Java_DebugUsing_Browser) {

		// get the param source
		int nParamSource = Java_ParamSource_Unknown;
		GPT gptParamSource = m_pSlob->GetIntProp(P_Java_ParamSource, nParamSource);

		// only change P_Java_ParamSource if it's not ambiguous and is unknown
		if (gptParamSource != ambiguous && nParamSource == Java_ParamSource_Unknown) {

			// first, set the param source to be the HTML page
			m_pSlob->SetIntProp(P_Java_ParamSource, Java_ParamSource_HTMLPage);

			// get the HTML page
			CString strHTMLPage;
			m_pSlob->GetStrProp(P_Java_HTMLPage, strHTMLPage);

			// if the HTML page is empty, set the param source to be the param grid (user)
			if (strHTMLPage.IsEmpty()) {

				// set the param source
				m_pSlob->SetIntProp(P_Java_ParamSource, Java_ParamSource_User);
			}
		}
	}
}

//----------------------------------------------------------------
// CDebugJavaBrowserPage: the page to handle the browser options
//----------------------------------------------------------------

BEGIN_IDE_CONTROL_MAP(CDebugJavaBrowserPage, IDDP_JAVA_DEBUG_BROWSER, IDS_JAVA_PAGE_BROWSER)
	MAP_EDIT(IDC_JAVA_BROWSER, P_Java_Browser)
	MAP_RADIO(IDC_JAVA_USE_HTML_PARAMS, IDC_JAVA_USE_USER_PARAMS, 1, P_Java_ParamSource)
	MAP_EDIT(IDC_JAVA_HTML_PAGE, P_Java_HTMLPage)
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CDebugJavaBrowserPage, CDebugPageTab)
	//{{AFX_MSG_MAP(CDebugJavaBrowserPage)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_CONTROL(GN_CHANGE, IDC_JAVA_PARAMS, OnChangeParamGrid)
END_MESSAGE_MAP()

//----------------------------------------------------------------
// CDebugJavaBrowserPage::OnInitDialog
BOOL CDebugJavaBrowserPage::OnInitDialog()
{
	if ( !CDebugPageTab::OnInitDialog() ) {
		return FALSE;
	}

	// init grid
	m_pgridParams = new CParamGridWnd;
	BOOL fRet = m_pgridParams->ReplaceControl(this, IDC_JAVA_PARAMS_HOLDER, IDC_JAVA_PARAMS,
		WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_TABSTOP, 0, GRIDWND_TB_ALL, TRUE);

	return TRUE;
}

//----------------------------------------------------------------
// CDebugJavaBrowserPage::OnDestroy
void CDebugJavaBrowserPage::OnDestroy()
{
	delete m_pgridParams;
}

//----------------------------------------------------------------
// CDebugJavaBrowserPage::InitPage
void CDebugJavaBrowserPage::InitPage()
{
	CDebugPageTab::InitPage();

	// Show the grid
	m_pgridParams->ShowWindow(SW_SHOW);
}

//----------------------------------------------------------------
// CDebugJavaBrowserPage::CommitPage
void CDebugJavaBrowserPage::CommitPage()
{
	// REVIEW(briancr): do we need to do anything here?
}

//----------------------------------------------------------------
// CDebugJavaBrowserPage::OnPropChange
BOOL CDebugJavaBrowserPage::OnPropChange(UINT idProp)
{
	// we want to enable/disable properties based on property changes

	switch (idProp) {
		case P_Java_Browser: {

			OnChangeBrowser();

			break;
		}
		case P_Java_HTMLPage: {

			UpdateParamGrid();

			break;
		}
		// user has changed the param source
		case P_Java_ParamSource: {

			OnChangeParamSource();

			break;
		}
	}

	// call our base-class
	return CDebugPageTab::OnPropChange(idProp);
}

//----------------------------------------------------------------
// CDebugJavaBrowserPage::Validate
BOOL CDebugJavaBrowserPage::Validate()
{
	if (m_pgridParams->AcceptControl())
	{
		OnChangeParamGrid();
	}
	return CDebugPageTab::Validate();
}

//----------------------------------------------------------------
// CDebugJavaBrowserPage::OnChangeBrowser
void CDebugJavaBrowserPage::OnChangeBrowser()
{
	// remove any quotes around the browser name

	// get the browser's name
	CString strBrowser;
	if (m_pSlob->GetStrProp(P_Java_Browser, strBrowser) == valid) {

		CString strNewBrowser = strBrowser;

		// trim white space
		strNewBrowser.TrimLeft();
		strNewBrowser.TrimRight();

		// only look for quotes if the string is not empty
		if (!strNewBrowser.IsEmpty()) {

			// is the first character a quote?
			if (strNewBrowser[0] == _T('\"')) {

				// trim it
				strNewBrowser = strNewBrowser.Mid(1);
			}
			
			// is the last character a quote?
			int nLen = strNewBrowser.GetLength();
			if (strNewBrowser[nLen-1] == _T('\"')) {

				// trim it
				strNewBrowser = strNewBrowser.Left(nLen-1);
			}

			// only set the browser property if it's different
			if (strNewBrowser != strBrowser) {

				m_pSlob->SetStrProp(P_Java_Browser, strNewBrowser);
			}
		}
	}
}

//----------------------------------------------------------------
// CDebugJavaBrowserPage::OnChangeParamGrid
void CDebugJavaBrowserPage::OnChangeParamGrid()
{
	// we'll only set the arg properties if the param source is not ambiguous
	// and the current args are not ambiguous
	// this prevents the configuration b's args from overwriting configuration a's args

	// get the param source
	int nParamSource = Java_ParamSource_Unknown;
	GPT gptParamSource = m_pSlob->GetIntProp(P_Java_ParamSource, nParamSource);
	if (gptParamSource != ambiguous && nParamSource != Java_ParamSource_Unknown) {

		// get the args to find out if they're ambiguous
		CString strCurrentHTMLArgs;
		if (m_pSlob->GetStrProp(P_Java_HTMLArgs, strCurrentHTMLArgs) != ambiguous) {

			// get the args from the grid
			CString strHTMLArgs = m_pgridParams->GetParamsAsHTML();

			// set the HTML style args
			m_pSlob->SetStrProp(P_Java_HTMLArgs, strHTMLArgs);
		}
	}
}

//----------------------------------------------------------------
// CDebugJavaBrowserPage::OnChangeParamSource
void CDebugJavaBrowserPage::OnChangeParamSource()
{
	// enable/disable controls based on the param source

	// if the param source is unknown (only initially):
	//   - disable HTML page
	//   - disable param grid
	// if the param source is HTML page:
	//   - enable HTML page
	//   - enable and read-only param grid
	//   - read the HTML page and fill the grid
	// if the param source is user:
	//   - disable HTML page
	//   - enable param grid

	// get the param source
	int nParamSource = Java_ParamSource_Unknown;
	GPT gptParamSource = m_pSlob->GetIntProp(P_Java_ParamSource, nParamSource);

	// enable/disable HTML page
	CDebugPageTab::OnPropChange(P_Java_HTMLPage);
	CWnd* pControl = (CWnd*)GetDlgItem(IDC_JAVA_HTML_PAGE_TXT);
	ASSERT(pControl != NULL);
	pControl->EnableWindow(gptParamSource != ambiguous && nParamSource == Java_ParamSource_HTMLPage);

	// enable/disable param grid
	m_pgridParams->EnableWindow(gptParamSource != ambiguous && nParamSource != Java_ParamSource_Unknown);
	m_pgridParams->SetReadOnly(gptParamSource != ambiguous && nParamSource == Java_ParamSource_HTMLPage);
	m_pgridParams->Invalidate();

	// update the param grid
	UpdateParamGrid();
}

//----------------------------------------------------------------
// CDebugJavaBrowserPage::UpdateParamGrid
void CDebugJavaBrowserPage::UpdateParamGrid()
{
	// get the param source
	int nParamSource = Java_ParamSource_Unknown;
	GPT gptParamSource = m_pSlob->GetIntProp(P_Java_ParamSource, nParamSource);

	CString strParams;

	// only read the params from the HTML page, if the HTML page is the param source
	if (gptParamSource != ambiguous && nParamSource == Java_ParamSource_HTMLPage) {

		// read the params from the HTML page and fill the grid

		// get the HTML page
		CString strHTMLPage;
		m_pSlob->GetStrProp(P_Java_HTMLPage, strHTMLPage);
		if (!strHTMLPage.IsEmpty()) {

			// get the class file name
			CString strClassFileName;
			m_pSlob->GetStrProp(P_Java_ClassFileName, strClassFileName);

			// is there a class file specified?
			if (!strClassFileName.IsEmpty()) {

				// attempt to read params from the page
				CPath pathClass;
				pathClass.Create(strClassFileName);
				strClassFileName = pathClass.GetFileName();

				// open the HTML file
				CHTMLPageFile fileHTML;
				if (fileHTML.Open(strHTMLPage, CFile::modeRead)) {

					// read params from page
					fileHTML.ReadParamBlock(strClassFileName, strParams);

					fileHTML.Close();
				}
			}
		}
	}
	// otherwise fill the param grid from the props
	else if (gptParamSource != ambiguous && nParamSource == Java_ParamSource_User) {

		// get the params
		m_pSlob->GetStrProp(P_Java_HTMLArgs, strParams);
	}

	// fill the param grid
	m_pgridParams->SetParamsFromHTML(strParams);
}

//----------------------------------------------------------------
// CDebugJavaStandalonePage: the page to handle the stand-alone options
//----------------------------------------------------------------

BEGIN_IDE_CONTROL_MAP(CDebugJavaStandalonePage, IDDP_JAVA_DEBUG_STANDALONE, IDS_JAVA_PAGE_STANDALONE)
	MAP_EDIT(IDC_JAVA_STANDALONE, P_Java_Standalone)
	MAP_EDIT(IDC_JAVA_STANDALONE_ARGS, P_Java_StandaloneArgs)
	MAP_EDIT(IDC_JAVA_PROG_ARGS, P_Java_Args)
#ifdef STANDALONE_APPLET
	MAP_RADIO(IDC_JAVA_STANDALONE_APPLET, IDC_JAVA_STANDALONE_APP, 1, P_Java_DebugStandalone)
#endif
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CDebugJavaStandalonePage, CDebugPageTab)
	//{{AFX_MSG_MAP(CDebugJavaStandalonePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// CDebugJavaStandalonePage::OnInitDialog
BOOL CDebugJavaStandalonePage::OnInitDialog()
{
	if ( !CDebugPageTab::OnInitDialog() ) {
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------
// CDebugJavaStandalonePage::InitPage
void CDebugJavaStandalonePage::InitPage()
{
	CDebugPageTab::InitPage();
}

//----------------------------------------------------------------
// CDebugJavaStandalonePage::CommitPage
void CDebugJavaStandalonePage::CommitPage()
{
	// REVIEW(briancr): do we need to do anything here?
}

//----------------------------------------------------------------
// CDebugJavaStandalonePage::OnPropChange
BOOL CDebugJavaStandalonePage::OnPropChange(UINT idProp)
{
	// we want to enable/disable properties based on property changes

	switch (idProp) {
		case P_Java_Standalone: {

			OnChangeStandalone();

			break;
		}
	}

	// call our base-class
	return CDebugPageTab::OnPropChange(idProp);
}

//----------------------------------------------------------------
// CDebugJavaStandalonePage::Validate
BOOL CDebugJavaStandalonePage::Validate()
{
	return CDebugPageTab::Validate();
}

//----------------------------------------------------------------
// CDebugJavaStandalonePage::OnChangeStandalone
void CDebugJavaStandalonePage::OnChangeStandalone()
{
	// remove any quotes around the stand-alone interpreter name

	// get the interpreter's name
	CString strStandalone;
	if (m_pSlob->GetStrProp(P_Java_Standalone, strStandalone) == valid) {

		CString strNewStandalone = strStandalone;

		// trim white space
		strNewStandalone.TrimLeft();
		strNewStandalone.TrimRight();

		// only look for quotes if the string is not empty
		if (!strNewStandalone.IsEmpty()) {

			// is the first character a quote?
			if (strNewStandalone[0] == _T('\"')) {

				// trim it
				strNewStandalone = strNewStandalone.Mid(1);
			}
			
			// is the last character a quote?
			int nLen = strNewStandalone.GetLength();
			if (strNewStandalone[nLen-1] == _T('\"')) {

				// trim it
				strNewStandalone = strNewStandalone.Left(nLen-1);
			}

			// only set the browser property if it's different
			if (strNewStandalone != strStandalone) {

				m_pSlob->SetStrProp(P_Java_Standalone, strNewStandalone);
			}
		}
	}

#ifdef STANDALONE_APPLET
	// update P_Java_DebugStandalone
	CDebugPageTab::OnPropChange(P_Java_DebugStandalone);
#endif
}

//----------------------------------------------------------------
// CDebugJavaAddlClasses: page for handling additional classes
//----------------------------------------------------------------

BEGIN_IDE_CONTROL_MAP(CDebugJavaAddlClassesPage, IDDP_JAVA_DEBUG_ADDL_CLASSES, IDS_JAVA_DEBUG_ADDL_CLASSES)
	MAP_CHECK(IDC_LOAD_DLL_PROMPT, P_PromptForDlls)
END_IDE_CONTROL_MAP()

//----------------------------------------------------------------
// CDebugJavaAddlClassesPage::OnInitDialog
BOOL CDebugJavaAddlClassesPage::OnInitDialog()
{
	CRect rc;
	HBLDTARGET hTarget;
	CString str;

	if ( !COptionMiniPage::OnInitDialog() ) {
		return FALSE;
	}

	// Get the correct location for the grid
	CWnd *pReplace = GetDlgItem(IDC_GRID_HOLDER);
	ASSERT(pReplace != NULL);

	// Init GridList
	g_BldSysIFace.InitTargetEnum();
	while ((hTarget = g_BldSysIFace.GetNextTarget(str)) != NO_TARGET)
	{
		GRIDINFO *pGI = new GRIDINFO;

		pGI->hTarget = hTarget;
		pGI->pGrid = new CAddlClassesGridWnd;
		BOOL fRet = pGI->pGrid->ReplaceControl(this, IDC_GRID_HOLDER, IDC_DLLS_LIST,
			WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_TABSTOP, 0, GRIDWND_TB_NEWDEL, FALSE);
		
		pGI->pGrid->AddRow(new CAddlClassesGridRow((CAddlClassesGridWnd*)pGI->pGrid));

		pGI->pGrid->ShowWindow(SW_HIDE);
		m_GridList.AddTail(pGI);
	}

	// HACK - If the project file is an exe, we won't get any targets
	if (m_GridList.IsEmpty())
	{
		ASSERT( ((CProxySlob *)m_pSlob)->IsSingle() );

		GRIDINFO *pGI = new GRIDINFO;
		hTarget = g_BldSysIFace.GetTarget(((CProxySlob *)m_pSlob)->GetBaseConfig()->GetConfigurationName(), ACTIVE_BUILDER);

		pGI->hTarget = hTarget;
		pGI->pGrid = new CAddlClassesGridWnd;
		
		BOOL fRet = pGI->pGrid->ReplaceControl(this, IDC_GRID_HOLDER, IDC_DLLS_LIST,
			WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_TABSTOP, 0, GRIDWND_TB_NEWDEL, FALSE);
		
		pGI->pGrid->AddRow(new CAddlClassesGridRow((CAddlClassesGridWnd*)pGI->pGrid));

		pGI->pGrid->ShowWindow(SW_HIDE);
		m_GridList.AddTail(pGI);
	}

	m_pGridActive = ((GRIDINFO *)m_GridList.GetHead())->pGrid;

	// Load info from src package (which got it from the vcp file)
	InitGrids();

	// and kill the holder window
	pReplace->DestroyWindow();

	return TRUE;
}

#if 0 // can't browse for .class files
//----------------------------------------------------------------
// CDebugJavaAddlClassesPage:: OnBrowse
void CDebugJavaAddlClassesPage::OnBrowse()
{
	// set up browse dialog
	CFileDialog dlg(TRUE);

	// browse dialog title
	CString strDlgTitle;
	VERIFY(strDlgTitle.LoadString(IDS_JAVA_BROWSE_CLASS));
	dlg.m_ofn.lpstrTitle = strDlgTitle;

	// browse dialog attributes
    dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON;

	// browse dialog filters
	CString strClassFilter;
	VERIFY(strClassFilter.LoadString(IDS_JAVA_FILTER_CLASS));
	CString strFilter;
	AppendFilterSuffix(strFilter, dlg.m_ofn, strClassFilter);
	dlg.m_ofn.lpstrFilter = strFilter;
	dlg.m_ofn.nFilterIndex = 1;

	// bring up the browse dialog
	if (dlg.DoModal() == IDOK) {

		// get the name of the class specified
		CString strLocalClass = dlg.GetPathName();

		// fill in the grid
		CEdit* pEdit = (CEdit*)m_pGridActive->GetDlgItem(1);
		ASSERT(pEdit != NULL);
		pEdit->SetWindowText(strLocalClass);
		pEdit->SetSel(0, -1);
		pEdit->SetFocus();
	}
}
#endif

//----------------------------------------------------------------
// CDebugJavaAddlClassesPage::InitGrids
void CDebugJavaAddlClassesPage::InitGrids()
{
	if (g_VPROJIdeInterface.Initialize()) 	// make sure we are initialised!
	{
		POSITION pos = m_GridList.GetHeadPosition();
		while (pos != NULL)
		{
			GRIDINFO *pGI = (GRIDINFO *)m_GridList.GetNext(pos);

			// Add blank rows to the grid for the src package to fill in.			
			int cRows;
			g_VPROJIdeInterface.GetDLLInfo()->GetDLLCount((ULONG)pGI->hTarget, &cRows);
			for (int i=0; i < cRows; i++)
			{
				pGI->pGrid->AddRow(new CAddlClassesGridRow((CAddlClassesGridWnd*)(pGI->pGrid)));
			}

			CPtrList listDLLs;
			// get the DLL info...
			g_VPROJIdeInterface.GetDLLInfo()->InitDLLList((ULONG)pGI->hTarget, listDLLs);

			int iRow = 0;
			POSITION pos = listDLLs.GetHeadPosition();
			while (pos != NULL)
			{
				DLLREC *pDLLRec = (DLLREC *)listDLLs.GetNext(pos);
				// The last row will always be a new row
				CDLLGridRow *pRow = (CDLLGridRow *)pGI->pGrid->GetRow(iRow++);
				pRow->m_fPreload		= pDLLRec->fPreload;
				pRow->m_strLocalName	= pDLLRec->strLocalName;
				pRow->m_strRemoteName	= pDLLRec->strRemoteName;
			}
		}
	}
}

//----------------------------------------------------------------
// CDebugJavaAddlClassesPage::InitPage
void CDebugJavaAddlClassesPage::InitPage()
{
	// call the base class's InitPage
	CDebugAdditionalDllPage::InitPage();

	// set the local name column to be a fixed full width
	// (i.e. hide remote name column)

	// figure out the width minus the preload column
	CRect rectCaptionRow;
	m_pGridActive->GetCaptionRow()->GetRect(rectCaptionRow);
	int cxName = rectCaptionRow.Width() - m_pGridActive->GetColumnWidth(COL_PRELOAD);

	// set the width of the local name column and fix it
	m_pGridActive->SetColumnWidth(COL_LOCALNAME, cxName);
	m_pGridActive->GetCaptionRow()->SetColumnFixedWidth(COL_LOCALNAME);

	// set the width of the remote name column
	m_pGridActive->SetColumnWidth(COL_REMOTENAME, 0);

	m_pGridActive->ResetSize();
	m_pGridActive->Invalidate(FALSE); // INEFFICIENT
	m_pGridActive->ResetScrollBars();
}

//----------------------------------------------------------------
// Java: grid control window for handling parameters
//----------------------------------------------------------------

IMPLEMENT_DYNAMIC(CParamGridWnd, CGridControlWnd)

BEGIN_MESSAGE_MAP(CParamGridWnd, CGridControlWnd)
	//{{AFX_MSG_MAP(CGridControlWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_BN_CLICKED(ID_GRD_NEW, OnGridNew)
	ON_BN_CLICKED(ID_GRD_DELETE, OnGridDelete)
	ON_BN_CLICKED(ID_GRD_MOVEUP, OnGridMoveUp)
	ON_BN_CLICKED(ID_GRD_MOVEDOWN, OnGridMoveDown)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_SYSKEYDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// CParamGridWnd::CParamGridWnd
CParamGridWnd::CParamGridWnd(int nColumns, BOOL bDisplayCaption)
: CGridControlWnd(nColumns, bDisplayCaption),
  m_bReadOnly(FALSE)
{
	// no sorting needed for params
	EnableSort(FALSE);
	EnableAutoSort(FALSE);

	// no multi selection
	EnableMultiSelect(FALSE);

	// enable drag n drop
	m_bEnableDragRow = TRUE;

	// set up read-only grid colors
	// (toggled by changing m_bSysDefaultColors)
	m_colorWindowText = GetSysColor(COLOR_WINDOWTEXT);
	m_colorWindow = GetSysColor(COLOR_BTNFACE);
	m_colorHighlight = GetSysColor(COLOR_HIGHLIGHT);
	m_colorHighlightText = GetSysColor(COLOR_HIGHLIGHTTEXT);

	// add one row (the add row)
	AddRow(new CParamGridRow(this));
}

//----------------------------------------------------------------
// CParamGridWnd::~CParamGridWnd
CParamGridWnd::~CParamGridWnd()
{
	// CGridWnd's dtor destroys the rows
}

//----------------------------------------------------------------
// CParamGridWnd::OnGridNew
afx_msg void CParamGridWnd::OnGridNew()
{
	// only process this event if we're not read-only
	if (!m_bReadOnly) {
		// activate the new row
		CGridRow* pSelRow = GetTailRow();
		if (pSelRow != NULL) {
			Select(pSelRow);
			// Auto-activate the edit box in the Name column
			ActivateNextControl();
		}	

		CGridControlWnd::OnGridNew();
	}
}

//----------------------------------------------------------------
// CParamGridWnd::OnGridDelete
afx_msg void CParamGridWnd::OnGridDelete()
{
	// only process this event if we're not read-only
	if (!m_bReadOnly) {
		if (m_pActiveElement == NULL) {
			DoRowDelete();
		}
		CGridControlWnd::OnGridDelete();
	}
}

//----------------------------------------------------------------
// CParamGridWnd::OnGridMoveUp
afx_msg void CParamGridWnd::OnGridMoveUp()
{
	// only process this event if we're not read-only
	if (!m_bReadOnly) {
		// if either of these asserts fire, this function needs to be
		// fixed to deal with multiple selection
		ASSERT(m_selection.GetCount() == 1);

		DoRowMove(GetRowIndex((CParamGridRow*)GetRowAt(m_posCurRow)) - 1);
	}
}

//----------------------------------------------------------------
// CParamGridWnd::OnGridMoveDown
afx_msg void CParamGridWnd::OnGridMoveDown()
{
	// only process this event if we're not read-only
	if (!m_bReadOnly) {
		// if either of these asserts fire, this function needs to be
		// fixed to deal with multiple selection
		ASSERT(m_selection.GetCount() == 1);

		DoRowMove(GetRowIndex((CParamGridRow*)GetRowAt(m_posCurRow)) + 1);
	}
}

//----------------------------------------------------------------
// CParamGridWnd::OnCreate
afx_msg int CParamGridWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGridControlWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the grid's captions
	CString strColCaption;
	VERIFY(strColCaption.LoadString(IDS_JAVA_PARAM_COL_NAME));
	SetColumnCaption(CParamGridRow::GRID_COL_NAME, strColCaption);
	VERIFY(strColCaption.LoadString(IDS_JAVA_PARAM_COL_VALUE));
	SetColumnCaption(CParamGridRow::GRID_COL_VALUE, strColCaption);

	// set the column width
	CRect rectGrid;
	GetCaptionRow()->GetRect(rectGrid);
	SetColumnWidth(CParamGridRow::GRID_COL_NAME, rectGrid.Width() / 2);
	SetColumnWidth(CParamGridRow::GRID_COL_VALUE, rectGrid.Width() / 2);

	// select the first row
	Select(GetRow(0));

	return 0;
}

//----------------------------------------------------------------
// CParamGridWnd::OnDestroy
afx_msg void CParamGridWnd::OnDestroy()
{
	CGridControlWnd::OnDestroy();
}

//----------------------------------------------------------------
// CParamGridWnd::OnPaint
void CParamGridWnd::OnPaint() 
{
	// do normal painting if we're enabled
	if (IsWindowEnabled()) {

		// if we're read-only, adjust the colors before painting
		if (m_bReadOnly) {
			// we set up the read-only colors at construction time
			// now we'll just use m_bSysDefaultColors to toggle
			// between the system colors and our read-only colors
			m_bSysDefaultColors = FALSE;
		}

		// paint
		CGridControlWnd::OnPaint();

		// if we're read-only, toggle back to system colors
		if (m_bReadOnly) {
			m_bSysDefaultColors = TRUE;
		}
	}
	else {

		// we're disabled or read-only

		CPaintDC dc(this);
		CGridRow* pRow;
		CRect rowRect;
		GetClientRect(rowRect);

		// Draw the captions first
		pRow = GetCaptionRow();
		rowRect.bottom = rowRect.top + pRow->GetRowHeight();
		if (dc.m_ps.rcPaint.top < rowRect.bottom)
		{
			CFont* pOldFont = dc.SelectObject(GetCaptionFont());
			pRow->Draw(&dc, rowRect);
			dc.SelectObject(pOldFont);
		}
	
		// Setup clip rect around the actual grid (client less captions)
		rowRect.top = rowRect.bottom;
		rowRect.bottom = dc.m_ps.rcPaint.bottom;

		dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
		dc.ExtTextOut(0, 0, ETO_OPAQUE, rowRect, NULL, 0, NULL);
	}
}

//----------------------------------------------------------------
// CParamGridWnd::DoDragDrop
void CParamGridWnd::DoDragDrop(CPoint point)
{
	// figure out where we're dropping the selected row(s)

	// convert the point to an element
	CGridElement* pElement = ElementFromPoint(point);
	// if the point doesn't convert, then drop after the last row
	if (pElement == NULL) {
		CGridRow* pRow = (CGridRow*)m_rows.GetTail();
		if (pRow == NULL)
			return;

		pElement = pRow->ElementFromPoint(point);
	}

	// at this point, the element must not be NULL
	ASSERT(pElement != NULL);

	// get the row from the element
	CGridRow* pRow = pElement->GetRow();

	// we can't insert in the caption row
	if (pRow == GetCaptionRow())
		return;

	// move the selected row(s) to the new position
	DoRowMove(GetRowIndex((CParamGridRow*)pRow));
}

//----------------------------------------------------------------
// CParamGridWnd::OnGetDlgCode
afx_msg UINT CParamGridWnd::OnGetDlgCode()
{
	return CGridControlWnd::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTCHARS;
}

//----------------------------------------------------------------
// CParamGridWnd::ProcessKeyboard
BOOL CParamGridWnd::ProcessKeyboard(MSG* pMsg, BOOL bPreTrans /*= FALSE*/)
{
	// determine the type of message
	switch (pMsg->message) {
		case WM_KEYDOWN: {
			// determine the key pressed
			switch (pMsg->wParam) {
				case VK_TAB: {
					// control-tab is mdi window switching
					if (GetKeyState(VK_CONTROL) < 0)
						break;

					// if there's no active element and we're in a dialog, don't do anything
					if (m_pActiveElement == NULL && m_bParentIsDialog)
						break;

					// we're finished with the current control
					if (!AcceptControl(FALSE))
						return TRUE;

					// tab to the next (prev) control
					if (GetKeyState(VK_SHIFT) < 0)
						ActivatePrevControl();
					else
						ActivateNextControl();

					return TRUE;
				}
				case VK_RETURN: {
					// if there's an active element, then accept the changes,
					// and select the next row
					if (m_pActiveElement != NULL && (GetKeyState(VK_CONTROL) >= 0)) {
						AcceptControl();
						// get the next row
						POSITION posNextRow = m_posCurRow;
						GetNextRow(posNextRow);
						// select the next row
						if (posNextRow != NULL) {
							Select(GetRowAt(posNextRow));
						}
						
						return TRUE;
					}
					break;
				}
				case VK_UP: {
					// Alt+Up moves the row up one
					if (GetKeyState(VK_MENU) < 0) {
						OnGridMoveUp();
						return TRUE;
					}
					break;
				}
				case VK_DOWN: {
					// Alt+Down moves the row down one
					if (GetKeyState(VK_MENU) < 0) {
						OnGridMoveDown();
						return TRUE;
					}
					break;
				}
			}

			break;
		}
	}

	return CGridControlWnd::ProcessKeyboard(pMsg, bPreTrans);
}

//----------------------------------------------------------------
// CParamGridWnd::OnCmdMsg
BOOL CParamGridWnd::OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
	return CGridControlWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//----------------------------------------------------------------
// CParamGridWnd::OnKillFocus
void CParamGridWnd::OnKillFocus(CWnd* pNewWnd)
{
	// call the base class
	CGridControlWnd::OnKillFocus(pNewWnd);

	// send grid change notification
	GridChange();
}

//----------------------------------------------------------------
// CParamGridWnd::DoRowMove
void CParamGridWnd::DoRowMove(int nToIndex)
{
	// the index to move the selected rows to must be valid
	if (nToIndex >= GetRowCount() - 1)			// -1 because of new row
		nToIndex = GetRowCount() - 2;			// -2 because of new row

	if (nToIndex < 0)
		return;

	// Notify the grid window of these moves
	// the grid can cancel any or all of the row moves
	CParamGridRow** pMoveRows = new CParamGridRow*[m_selection.GetCount()];
	int cMove = 0;

	// go through all selected rows and notify the grid of the move
	int nDest = nToIndex;
	for (POSITION posSel = GetHeadSelPosition(); posSel != NULL; ) {

		// get the row to move
		CParamGridRow* pSelRow = (CParamGridRow*)GetNextSel(posSel);

		// don't move the new row
		if (pSelRow->IsNewRow()) {
			Deselect(pSelRow);
		}
		else {

			// get the index of the row to move
			int nSrcIndex = GetRowIndex(pSelRow);

			// notify the grid that this row is moving; if the grid
			// returns FALSE, don't move the row
			if (!OnRowMove(nSrcIndex, nDest++)) {
				Deselect(pSelRow);
				nDest--;
			}
			else {
				// store the row in our array of rows to move
				pMoveRows[cMove++] = pSelRow;
			}
		}
	}

	// remove the selected rows
	for (posSel = GetHeadSelPosition(); posSel != NULL; ) {

		// get the row to remove
		CParamGridRow* pSelRow = (CParamGridRow*)GetNextSel(posSel);

		// Remove the row from the grid
		POSITION pos = m_rows.Find(pSelRow);
		ASSERT(pos != NULL);

		// update m_posTopVisibleRow if the row we're about to delete is the
		// top visible row
		if (m_posTopVisibleRow == pos) {

			// make the previous row the top visible row
			GetPrevRow(m_posTopVisibleRow);

			// we don't want to make the caption row the visible row
			if (m_posTopVisibleRow != NULL && (GetRowAt(m_posTopVisibleRow) == GetCaptionRow())) {
				m_posTopVisibleRow = NULL;
				m_cyScroll = 0;
			}
		}

		// remove the row
		m_rows.RemoveAt(pos);
	}

	// move each row from our temp list into the grid's list of rows

	// figure the starting row to insert after
	POSITION posTarget = m_rows.FindIndex(nToIndex);
	ASSERT(posTarget != NULL);
	for (int iCur = 0; iCur < cMove; iCur++) {
		posTarget = m_rows.InsertAfter(posTarget, pMoveRows[iCur]);
	}

	// delete the array of moved rows
	delete [] pMoveRows;

	// repaint
	AdjustTopVisible();
	ResetSize(); // INEFFICIENT
	Invalidate(FALSE); // INEFFICIENT
	ResetScrollBars();

	// scroll the first row in the selection into view
	// get the first row in the selection list
	POSITION posSelHead = GetHeadSelPosition();
	CGridRow* pSelRow = (posSelHead == NULL) ? NULL : (CGridRow*) GetNextSel(posSelHead);
	if (pSelRow != NULL) {
		// scroll this row into view
		ScrollIntoView(pSelRow);
	}

	// if no rows are selected, select the "new" row
	if (m_selection.IsEmpty()) {
		CParamGridRow* pRow = GetTailRow();
		if (pRow != NULL)
			Select(pRow);
	}

	// send grid change notification
	GridChange();
}

//----------------------------------------------------------------
// CParamGridWnd::OnRowMove
BOOL CParamGridWnd::OnRowMove(int nSrcIndex, int nDestIndex)
{
	return TRUE;
}

//----------------------------------------------------------------
// CParamGridWnd::DoRowDelete
void CParamGridWnd::DoRowDelete()
{
	// don't try to delete anything with an empty selection list
	if (m_selection.IsEmpty())
		return;

	// figure out the row to select after the deletion is done: it should be
	// the row after the first element in the selection list
	POSITION posNewSel = NULL;

	// go through the selection list and delete all selected rows
	for (POSITION posSel = GetHeadSelPosition(); posSel != NULL; ) {

		// get the selected row
		CParamGridRow* pSelRow = (CParamGridRow*)GetNextSel(posSel);

		// if we're deleting the last selected row, the selected row should
		// be the row following the last selected node
		if (posSel == NULL) {
			posNewSel = m_rows.Find(pSelRow);
			m_rows.GetNext(posNewSel);
		}

		// don't delete the last row (the add row)
		if (!pSelRow->IsNewRow()) {

			// delete the row
			RemoveRow(pSelRow);
			delete pSelRow;
		}
		else {

			// deselect the last row, since you can't delete it
			Deselect(pSelRow);
		}
	}

	// Redraw
	Invalidate(FALSE);
	ResetScrollBars();

	// if there's no row to select, default to the last row in the grid
	if (posNewSel == NULL)
		posNewSel = GetTailRowPosition();

	// Set the selection
	CGridControlRow* pSelRow = (CGridControlRow*)GetRowAt(posNewSel);
	// this row should not be the caption row
	ASSERT((CGridRow*)pSelRow != (CGridRow*)GetCaptionRow());

	SetActiveElement(pSelRow);
	Select(pSelRow);

	// send grid change notification
	GridChange();
}

//----------------------------------------------------------------
// CParamGridWnd::GetCurSel
int CParamGridWnd::GetCurSel()
{
	// we should have exactly one item selected
	if (m_selection.GetCount() != 1)
		return -1;
	return GetRowIndex((CParamGridRow*)GetRowAt(m_posCurRow));
}

//----------------------------------------------------------------
// CParamGridWnd::GetRowIndex
int CParamGridWnd::GetRowIndex(const CParamGridRow* pRow) const
{
	// iterate through the rows until the one given is found
	int nIndex = 0;
	for (POSITION pos = GetHeadRowPosition(); pos != NULL; nIndex++) {
		if ((CParamGridRow*)GetNextRow(pos) == pRow)
			return nIndex;
	}

	return -1;
}

//----------------------------------------------------------------
// CParamGridWnd::SetReadOnly
inline void CParamGridWnd::SetReadOnly(BOOL bReadOnly)
{
	m_bReadOnly = bReadOnly;

	if (m_bReadOnly) {
		// no selection for a read-only grid
		// go through selection list and deselect everything
		for (POSITION posSel = GetHeadSelPosition(); posSel != NULL; ) {
			CParamGridRow* pSelRow = (CParamGridRow*)GetNextSel(posSel);
			Deselect(pSelRow);
		}
	}
}

//----------------------------------------------------------------
// CParamGridWnd::IsReadOnly
inline BOOL CParamGridWnd::IsReadOnly()
{
	return m_bReadOnly;
}

//----------------------------------------------------------------
// CParamGridWnd::GetTailRow
inline CParamGridRow* CParamGridWnd::GetTailRow() const
{
	if (m_rows.IsEmpty())
		return NULL;
	return (CParamGridRow*)m_rows.GetTail();
}

//----------------------------------------------------------------
// CParamGridWnd::GetTailRowPosition
inline POSITION CParamGridWnd::GetTailRowPosition() const
{
	return m_rows.GetTailPosition();
}

//----------------------------------------------------------------
// CParamGridWnd::GetTailSelPosition
inline POSITION CParamGridWnd::GetTailSelPosition() const
{
	return m_selection.GetTailPosition();
}

//----------------------------------------------------------------
// CParamGridWnd::SetParamsFromHTML
inline void CParamGridWnd::SetParamsFromHTML(LPCSTR pszHTMLParams)
{
	if (m_strHTMLParams != CString(pszHTMLParams)) {
		m_strHTMLParams = pszHTMLParams;
		SetGridFromHTML(m_strHTMLParams);
		CParamGridRow* pRow = (CParamGridRow*)GetRowAt(GetTailRowPosition());
		SetActiveElement(pRow);
		Select(pRow);
	}

	// send grid change notification
	GridChange();
}

//----------------------------------------------------------------
// CParamGridWnd::GetParamsAsHTML
inline CString CParamGridWnd::GetParamsAsHTML()
{
	m_strHTMLParams = GetHTMLFromGrid();
	return m_strHTMLParams;
}

//----------------------------------------------------------------
// CParamGridWnd::GetParamsAsCmdLine
inline CString CParamGridWnd::GetParamsAsCmdLine()
{
	return GetCmdLineFromGrid();
}

//----------------------------------------------------------------
// CParamGridWnd::DeleteAllRows
void CParamGridWnd::DeleteAllRows()
{
	// delete all rows (even the new row)

	for (POSITION posRow = GetHeadRowPosition(); posRow != NULL; ) {
		// get the row to delete
		CParamGridRow* pRow = (CParamGridRow*)GetNextRow(posRow);
		RemoveRow(pRow, FALSE);
	}
}

//----------------------------------------------------------------
// CParamGridWnd::SetGridFromHTML
void CParamGridWnd::SetGridFromHTML(LPCSTR pszParams)
{
	// this function expects the parameter string to be in the
	// HTML applet parameter format:
	// <param name="param name" value="param value">

	// delete everything in the grid
	DeleteAllRows();

	CString strParams = pszParams;
	CString strName;
	CString strValue;

	// read each param line from the string until there are no more
	BOOL bMore = TRUE;
	while (bMore) {

		// get a param line (<param ...>)
		bMore = GetParamLine(strParams, strName, strValue);

		// if either the name or value has a value, enter it in the grid
		if (!strName.IsEmpty() || !strValue.IsEmpty()) {

			// create a row for this param
			CParamGridRow* pNewRow = new CParamGridRow(this);
			pNewRow->SetColumnText(CParamGridRow::GRID_COL_NAME, strName);
			pNewRow->SetColumnText(CParamGridRow::GRID_COL_VALUE, strValue);

			// add this row
			AddRow(pNewRow);
		}
	}

	// add the new row
	AddRow(new CParamGridRow(this));
}

//----------------------------------------------------------------
// CParamGridWnd::GetHTMLFromGrid
CString CParamGridWnd::GetHTMLFromGrid()
{
	CString strParams;
	CString strFormat;
	strFormat.LoadString(IDS_JAVA_PARAMS_AS_HTML);

	// go through each row in the grid and generate params for it
	for (POSITION posRow = GetHeadRowPosition(); posRow != NULL; ) {

		CParamGridRow* pRow = (CParamGridRow*)GetNextRow(posRow);

		// don't generate params for the new row
		if (!pRow->IsNewRow()) {

			// get name and value for the row
			CString strName;
			CString strValue;
			pRow->GetColumnText(CParamGridRow::GRID_COL_NAME, strName);
			pRow->GetColumnText(CParamGridRow::GRID_COL_VALUE, strValue);

			// generate params
			CString strParam;
			strParam.Format(strFormat, strName, strValue);

			strParams += strParam;
		}
	}

	return strParams;
}

//----------------------------------------------------------------
// CParamGridWnd::GetCmdLineFromGrid
CString CParamGridWnd::GetCmdLineFromGrid()
{
	CString strParams;

	// go through each row in the grid and generate a command line from it
	for (POSITION posRow = GetHeadRowPosition(); posRow != NULL; ) {

		CParamGridRow* pRow = (CParamGridRow*)GetNextRow(posRow);

		// don't generate any cmd line args for the new row
		if (!pRow->IsNewRow()) {

			// get the name and value for the row
			CString strName;
			CString strValue;
			pRow->GetColumnText(CParamGridRow::GRID_COL_NAME, strName);
			pRow->GetColumnText(CParamGridRow::GRID_COL_VALUE, strValue);
			// trim white space from name and value
			strName.TrimLeft();
			strName.TrimRight();
			strValue.TrimLeft();
			strValue.TrimRight();

			// append the name
			strParams += strName;

			// if name and value are both not empty, put an equal
			// sign between them, so params are name=value format
			if (!strName.IsEmpty() && !strValue.IsEmpty()) {
				strParams += _T("=");
			}

			// append the value
			strParams += strValue;

			// add a space to separate params
			strParams += _T(" ");
		}
	}

	// trim trailing white space
	strParams.TrimRight();

	return strParams;
}

//----------------------------------------------------------------
// CParamGridWnd::GridChange
void CParamGridWnd::GridChange()
{
	// send a GN_CHANGE message to the parent
	WPARAM wParam = (WPARAM)MAKELONG(GetDlgCtrlID(), GN_CHANGE);
	ASSERT(LOWORD(wParam) != 0);
	LPARAM lParam = (LPARAM)GetSafeHwnd();
	ASSERT(lParam != 0);
	CWnd* pParent = GetParent();
	ASSERT(pParent != NULL);

	pParent->SendMessage(WM_COMMAND, wParam, lParam);
}

//----------------------------------------------------------------
// CParamGridWnd::GetParamLine
BOOL CParamGridWnd::GetParamLine(CString& strParams, CString& strName, CString& strValue)
{
	strName.Empty();
	strValue.Empty();

	CString strNameToken;
	CString strValueToken;

	// the param line is in the format:
	// <param name="name" value="value">

	// sets of tokens to skip while extracting the name and value
	TCHAR* pszSkipTokens1[] = { _T("<"), _T("param"), _T("name"), _T("="), NULL, _T("\""), NULL };
	TCHAR* pszSkipTokens2[] = { _T("value"), _T("="), NULL, _T("\""), NULL };
	TCHAR* pszSkipTokens3[] = { _T(">"), NULL };

	// skip all initial tokens
	if (SkipTokens(strParams, pszSkipTokens1)) {

		// get the name
		if (GetQuotedString(strParams, strNameToken)) {

			// skip all tokens between name and value
			if (SkipTokens(strParams, pszSkipTokens2)) {

				// get the value
				if (GetQuotedString(strParams, strValueToken)) {

					// skip all ending tokens
					if (SkipTokens(strParams, pszSkipTokens3)) {

						strName = strNameToken;
						strValue = strValueToken;

						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

//----------------------------------------------------------------
// CParamGridWnd::SkipTokens
BOOL CParamGridWnd::SkipTokens(CString& strString, TCHAR* pszSkipTokens[])
{
	// skip all tokens given
	for (int i = 0; pszSkipTokens[i] != NULL; i++) {

		// remove any leading white space
		strString.TrimLeft();

		// get the length of the expected token
		int nTokenLen = _tcslen(pszSkipTokens[i]);
		// extract what we expect to be the token
		CString strToken = strString.Left(nTokenLen);
		// lower case the token
		strToken.MakeLower();
		// is this the token we expect?
		if (strToken != pszSkipTokens[i]) {

			// token not found
			return FALSE;
		}

		// skip the token
		strString = strString.Mid(nTokenLen);
	}

	return TRUE;
}

//----------------------------------------------------------------
// CParamGridWnd::GetQuotedString
BOOL CParamGridWnd::GetQuotedString(CString& strString, CString& strValue)
{
	int nEndName = -1;

	// the string may or may not really be quoted
	if (strString[0] == _T('\"')) {

		// find ending quote
		// + 2 to include beginning and ending quote
		nEndName = strString.Mid(1).Find(_T('\"')) + 2;
		// if we didn't find the end of the value, assume
		// the end of the string is the end of the value
		if (nEndName < 2)
			nEndName = strString.GetLength();
	}
	else {
		// name is not quoted, find ending white space
		nEndName = strString.FindOneOf(_T(" \r\n\t>"));
		// if we didn't find the end of the value, assume
		// the end of the string is the end of the value
		if (nEndName == -1)
			nEndName = strString.GetLength();
	}

	// copy the value out of the string
	strValue = strString.Left(nEndName);

	// discard the value from the string
	strString = strString.Mid(nEndName);

	return TRUE;
}

void CParamGridWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// only process this event if we're not read-only
	if (!m_bReadOnly)
		CGridControlWnd::OnRButtonDown(nFlags, point);
}

void CParamGridWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// only process this event if we're not read-only
	if (!m_bReadOnly)
		CGridControlWnd::OnRButtonUp(nFlags, point);
}

void CParamGridWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// only process this event if we're not read-only
	if (!m_bReadOnly)
		CGridControlWnd::OnLButtonDown(nFlags, point);
}

void CParamGridWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// only process this event if we're not read-only
	if (!m_bReadOnly)
		CGridControlWnd::OnLButtonDblClk(nFlags, point);
}

void CParamGridWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// only process this event if we're not read-only
	if (!m_bReadOnly)
		CGridControlWnd::OnLButtonUp(nFlags, point);
}

void CParamGridWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// only process this event if we're not read-only
	if (!m_bReadOnly)
		CGridControlWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CParamGridWnd::OnSize(UINT nType, int cx, int cy)
{
	// override OnSize to fix bug in CGridWnd::OnSize
	// the bug is that when SetActiveElement in CGridWnd::OnSize
	// is called, the column should be specified as m_nCurColumn.
	// Rather than make this change for all grids, which is slightly
	// risky, we'll just do it for the param grid

	CGridElement* pActiveElement = m_pActiveElement;

	SetActiveElement(NULL);
	
	// call the base class
	CGridControlWnd::OnSize(nType, cx, cy);

	if (pActiveElement != NULL)
		SetActiveElement(pActiveElement, m_nCurColumn);
}

void CParamGridWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// only process this event if we're not read-only
	if (!m_bReadOnly)
		CGridControlWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CParamGridWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// only process this event if we're not read-only
	if (!m_bReadOnly)
		CGridControlWnd::OnMouseMove(nFlags, point);
}

BOOL CParamGridWnd::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	// only process this event if we're not read-only
	if (!m_bReadOnly)
		return CGridControlWnd::OnToolTipText(nID, pNMHDR, pResult);
	return FALSE;
}

//----------------------------------------------------------------
// CParamGridRow: row for handling parameters
//----------------------------------------------------------------

//----------------------------------------------------------------
// CParamGridRow::CParamGridRow
CParamGridRow::CParamGridRow(CParamGridWnd* pGrid)
: CGridControlRow(pGrid)
{
}

//----------------------------------------------------------------
// CParamGridRow::~CParamGridRow
CParamGridRow::~CParamGridRow()
{
}

//----------------------------------------------------------------
// CParamGridRow::DrawCell
// Need to override DrawCell in order to draw the focus rectangle in the
// name column of the last row (the "new" row)
void CParamGridRow::DrawCell(CDC *pDC, const CRect& cellRect, int nColumn)
{
	// let the base class actually draw the cell
	CGridControlRow::DrawCell(pDC, cellRect, nColumn);

	// only draw the focus rect if the grid is not read-only
	if (!((CParamGridWnd*)GetGrid())->IsReadOnly()) {
		// Draw a focus rect in the name column of the new row
		if (IsNewRow() && nColumn == GRID_COL_NAME) {
			// adjust the focus rect for margins
			CRect rectText = cellRect;
			AdjustForMargins(rectText, nColumn);

			pDC->DrawFocusRect(rectText);
		}
	}
}

//----------------------------------------------------------------
// CParamGridRow::OnActivate
void CParamGridRow::OnActivate(BOOL bActivate, int nColumn)
{
	// are we activating a column?
	if (bActivate && nColumn >= 0) {

		// create an edit control in the column
		CreateControl(edit, nColumn, ES_AUTOHSCROLL);
	}
	else {

		// deactivating: remove the control from the column
		CreateControl(none, -1);
	}
}

//----------------------------------------------------------------
// CParamGridRow::OnAccept
BOOL CParamGridRow::OnAccept(CWnd* pControlWnd)
{
	CParamGridWnd* pGrid = GetParamGrid();
	int nCol = pGrid->m_nControlColumn;

	// store the text the user entered into the column
	CString str;
	if (pControlWnd != NULL) {
		switch (pGrid->m_nControlColumn) {
			case GRID_COL_NAME:
				pControlWnd->GetWindowText(str);
				QuoteString(str);
				m_strName = str;
				break;

			case GRID_COL_VALUE:
				pControlWnd->GetWindowText(str);
				QuoteString(str);
				m_strValue = str;
				break;

			default:
				// should only be name or value columns
				ASSERT(FALSE);
				break;
		}
	}

	// If the user has deleted the name and value, remove the row
	// (but don't ever delete the last row - it's the "new" row)
	if (!IsNewRow() && (m_strName.IsEmpty() && m_strValue.IsEmpty())) {

		// only the current row will be selected, so DoDelete will
		// just delete one row
		GetParamGrid()->DoRowDelete();
		return TRUE;
	}

	// if the user has entered something in the last row (the "new" row),
	// create a new "new" row
	if (IsNewRow() && !str.IsEmpty()) {

		pGrid->AddRow(new CParamGridRow(pGrid));
		// Make sure the new row is added at the tail
		ASSERT(pGrid->GetTailRow() != this);
	}

	return TRUE;
}

//----------------------------------------------------------------
// CParamGridRow::ResetSize
void CParamGridRow::ResetSize(CDC* pDC)
{
	// get the text metrics
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	// set the row height
	m_cyHeight = tm.tmHeight + tm.tmExternalLeading + 3;
}

//----------------------------------------------------------------
// CParamGridRow::GetColumnText
void CParamGridRow::GetColumnText(int nColumn, CString& str)
{
	// get the text for given column
	switch (nColumn) {
		case GRID_COL_NAME:
			str = m_strName;
			break;
		case GRID_COL_VALUE:
			str = m_strValue;
			break;
		default:
			// should only be name or value columns
			ASSERT(FALSE);
	}
}

//----------------------------------------------------------------
// CParamGridRow::SetColumnText
void CParamGridRow::SetColumnText(int nColumn, LPCSTR pszValue)
{
	// set the text for the given column
	switch (nColumn) {
		case GRID_COL_NAME:
			m_strName = pszValue;
			break;
		case GRID_COL_VALUE:
			m_strValue = pszValue;
			break;
		default:
			// should only be name or value columns
			ASSERT(FALSE);
	}
}

//----------------------------------------------------------------
// CParamGridRow::GetParamGrid
inline CParamGridWnd* CParamGridRow::GetParamGrid()
{
	return (CParamGridWnd*)GetGrid();
}

//----------------------------------------------------------------
// CParamGridRow::IsNewRow
inline BOOL CParamGridRow::IsNewRow()
{
	return (GetParamGrid()->GetTailRow() == this);
}

//----------------------------------------------------------------
// CParamGridRow::QuoteString
void CParamGridRow::QuoteString(CString& str)
{
	// if the given string contains white space and is not quoted,
	// put quotes around it

	// search the string for white space
	if (str.FindOneOf(_T(" \t\n\r")) != -1) {

		// is the string already quoted?
		int nLen = str.GetLength();
		if (str[0] != _T('\"') || str[nLen-1] != _T('\"')) {

			// put quotes around the string
			str = _T("\"") + str + _T("\"");
		}
	}
}

//----------------------------------------------------------------
// CAddlClassesGridWnd: grid control window for handling addl classes
//----------------------------------------------------------------

// NOTE: there are really only two columns for the additional classes grid.
// Having three makes the derivation from CDLLGridWnd much simpler. We'll just
// fix the size of the second column so you never see the third.
CAddlClassesGridWnd::CAddlClassesGridWnd(int nColumns /*= 3*/, BOOL bDisplayCaption /*= TRUE*/)
: CDLLGridWnd(nColumns, bDisplayCaption)
{
	// No browsing for classes.
	m_bEnableEllipse = FALSE;
}

//----------------------------------------------------------------
// CAddlClassesGridRow: grid row for handling addl classes
//----------------------------------------------------------------

CAddlClassesGridRow::CAddlClassesGridRow(CAddlClassesGridWnd* pGridWnd, CAddlClassesGridRow* pParent /*= NULL*/)
: CDLLGridRow(pGridWnd, pParent)
{
}

void CAddlClassesGridRow::GetColumnText(int nColumn, CString& rStr)
{
	switch (nColumn) {
		case COL_LOCALNAME:
			rStr = m_strLocalName;
			// Remove path and .class extension.
			if (!rStr.IsEmpty()) {
				CString strClassExt;
				VERIFY(strClassExt.LoadString(IDS_JAVA_CLASS_EXT));
				TCHAR szFileName[_MAX_FNAME];
				TCHAR szExt[_MAX_EXT];
				_tsplitpath(rStr, NULL, NULL, szFileName, szExt);
				CString strExt = CString(szExt);
				strExt.MakeLower();
				if (strExt == strClassExt) {
					*szExt = _T('\0');
				}
				rStr = CString(szFileName) + CString(szExt);
			}
			break;

		default:
			CDLLGridRow::GetColumnText(nColumn, rStr);
			break;
	}		
}

void CAddlClassesGridRow::OnActivate(BOOL bActivate, int nColumn)
{
	if (bActivate) {
		switch (nColumn) {
			case COL_LOCALNAME:
				GetDLLGrid()->m_bEnableEllipse = FALSE;
				CreateControl(edit, nColumn, ES_AUTOHSCROLL);
				break;

			default:
				CDLLGridRow::OnActivate(bActivate, nColumn);
				break;
		}
	}
	else {
		CDLLGridRow::OnActivate(bActivate, nColumn);
	}
}

BOOL CAddlClassesGridRow::OnAccept(CWnd *pControlWnd)
{
	CString str;
	CAddlClassesGridWnd *pGrid = (CAddlClassesGridWnd*)GetDLLGrid();
	int nCol = pGrid->m_nControlColumn;

	if (pControlWnd != NULL)
	{
		switch ( pGrid->m_nControlColumn ) 
		{
			case COL_PRELOAD:
				GetDLLGrid()->SetFocus();
				break;

			case COL_LOCALNAME:
				pControlWnd->GetWindowText(str);
				m_strLocalName = str;
				break;

			case COL_REMOTENAME:
				pControlWnd->GetWindowText(str);
				m_strRemoteName = str;
				break;

			default:
				ASSERT(FALSE);
				break;
		}
	}

	// If the user has deleted both the local and remote name, remove the row
	// (but don't ever delete the last row - it's the "new" row)
	if (this != pGrid->GetLastRow() &&
		(nCol == COL_LOCALNAME || nCol == COL_REMOTENAME) &&
		(m_strRemoteName.IsEmpty() && m_strLocalName.IsEmpty()) 
	) {

		// only the current row will be selected
		GetDLLGrid()->DoDelete();
		return TRUE;
	}

	if (this == pGrid->GetLastRow()
			&& (nCol == COL_LOCALNAME || nCol == COL_REMOTENAME)
			&& !str.IsEmpty() ) {

		pGrid->AddRow(new CAddlClassesGridRow((CAddlClassesGridWnd*)pGrid));
		// Make sure the new row is added at the tail
		ASSERT( pGrid->GetLastRow() != this );
	}

	return TRUE;
}																		 

