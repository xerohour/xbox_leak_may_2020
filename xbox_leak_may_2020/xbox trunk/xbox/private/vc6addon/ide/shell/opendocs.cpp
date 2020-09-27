// opendocs.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop
#include "opendocs.h"
#include "util.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenDocsDlg dialog


COpenDocsDlg::COpenDocsDlg(CWnd* pParent /*=NULL*/)
	: C3dDialog(COpenDocsDlg::IDD, pParent),
	  m_bPrintable(FALSE),
	  m_bSaveable(FALSE)
{
	//{{AFX_DATA_INIT(COpenDocsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COpenDocsDlg::DoDataExchange(CDataExchange* pDX)
{
	C3dDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenDocsDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenDocsDlg, C3dDialog)
	//{{AFX_MSG_MAP(COpenDocsDlg)
	ON_BN_CLICKED(IDC_OPEN_DOCUMENTS_CLOSE, OnOpenDocumentsCloseDoc)
	ON_BN_CLICKED(IDC_OPEN_DOCUMENTS_ACTIVATE, OnOpenDocumentsActivate)
#ifdef PRINT_DOC_LIST
	ON_BN_CLICKED(IDC_OPEN_DOCUMENTS_PRINT, OnOpenDocumentsPrint)
#endif
	ON_BN_CLICKED(IDC_OPEN_DOCUMENTS_SAVE, OnOpenDocumentsSave)
	ON_LBN_DBLCLK(IDC_OPEN_DOCUMENTS_LIST, OnOpenDocumentsActivate)
	ON_BN_CLICKED(IDC_OPEN_DOCUMENTS_TILE_HORZ, OnOpenDocumentsTileHorz)
	ON_BN_CLICKED(IDC_OPEN_DOCUMENTS_TILE_VERT, OnOpenDocumentsTileVert)
	ON_LBN_SELCHANGE(IDC_OPEN_DOCUMENTS_LIST, OnSelChangeDocList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenDocsDlg virtual function overrides
BOOL COpenDocsDlg::OnInitDialog() 
{
	C3dDialog::OnInitDialog();
	FillListBox();

	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_OPEN_DOCUMENTS_LIST);
	int cElements = pListBox->GetCount();

	pListBox->SetFocus();

	SelectActiveChild();
	
	OnSelChangeDocList();

	UpdateUI();

	return FALSE;
}

// From Orion 6115, the buttons and labels in this dialogue have been reorganised.

// In 4.x, the buttons were labelled 
//		Activate (IDOK), Cancel (IDCANCEL), Save, Close Window
// Cancel was a misnomer, because it wasn't possible to cancel actions in the dialog
// This has been fixed for 5.0 such that the buttons are now as follows:
//		Activate (ID_...ACTIVATE), OK (IDOK), Save, Close Window.
// This means that the new ok button is the old cancel button, and the new ACTIVATE 
// button is the old ok button. There is no cancel button.

// This also helps the context help to work better. martynl 29Apr96

void COpenDocsDlg::OnOpenDocumentsActivate()
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_OPEN_DOCUMENTS_LIST);
	int cElements = pListBox->GetSelCount();
	int nSelection;

	// can only activate a window if at least one item is selected
	if (cElements > 0) {
		// activate the first selected item
		pListBox->GetSelItems(1, &nSelection);

		// get the pointer to the frame
		CPartFrame* pFrame;
		pFrame = (CPartFrame*)pListBox->GetItemData(nSelection);
		ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));

		// activate the window
		// olympus 15235
		// now use theApp.SetActiveView() rather than MDIActivate()
		// get a pointer to the active view in the frame
		CPartView* pView = (CPartView*)pFrame->GetActiveView();
		ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));
		// olympus 15840
		// we need to unlock the worker AND set the active docker to NULL
		// note that instead of doing these two lines of code,
		// we could also do a theApp.ReactivateView() after the DoModal call
		// that invokes this dialog (if OK was pressed)
		CDockWorker::LockWorker(FALSE);
		CDockWorker::s_pActiveDocker = NULL;
		theApp.SetActiveView(pView->m_hWnd);
	}

	C3dDialog::OnOK();			
}

/////////////////////////////////////////////////////////////////////////////
// COpenDocsDlg message handlers
void COpenDocsDlg::OnOpenDocumentsCloseDoc() 
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_OPEN_DOCUMENTS_LIST);
	int cElements = pListBox->GetSelCount();
	int iElement;

	if (cElements > 0)
	{
		CWaitCursor hourglass;
		(void)hourglass;

		// get the selected documents
		int *rgiSelections = new int[cElements];
		pListBox->GetSelItems(cElements, rgiSelections);
		
		// store the HWND for each frame
		// because if the .rc file is closed before its editors,
		// the HWNDs for the editors will no longer be valid
		HWND *aHWnds = new HWND[cElements];
		for (iElement = 0; iElement < cElements; iElement++) {
			// get the pointer to the frame
			CPartFrame* pFrame;
			pFrame = (CPartFrame*)pListBox->GetItemData(rgiSelections[iElement]);
			ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));
			aHWnds[iElement] = pFrame->m_hWnd;
		}
		// iterate through the selected documents
		for (iElement = 0; iElement < cElements; iElement++) {
			// make sure the HWND for the frame is still valid before doing anything
			if (::IsWindow(aHWnds[iElement])) {
				// get the pointer to the frame
				CPartFrame* pFrame;
				pFrame = (CPartFrame*)pListBox->GetItemData(rgiSelections[iElement]);
				ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));

				pFrame->SendMessage(WM_CLOSE);
			}
		}

		// get a pointer to the main frame
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

		// refresh the MDI window
		pMainFrame->SendMessage(WM_MDIREFRESHMENU);

		FillListBox();

		int iSel = min(rgiSelections[0], pListBox->GetCount() - 1);
		pListBox->SetCaretIndex(iSel);
		pListBox->SetSel(iSel);

		UpdateUI();

		delete [] aHWnds;
		delete [] rgiSelections;
	}
}

#ifdef PRINT_DOC_LIST
void COpenDocsDlg::OnOpenDocumentsPrint() 
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_OPEN_DOCUMENTS_LIST);
	int cElements = pListBox->GetSelCount();

	if (cElements > 0)
	{
		// get the selected windows
		int *rgiSelections = new int[cElements];
		pListBox->GetSelItems(cElements, rgiSelections);

		// iterate through the selected windows
		for (int iElement = 0; iElement < cElements; iElement++) {
			// get the pointer to the frame
			CPartFrame* pFrame;
			pFrame = (CPartFrame*)pListBox->GetItemData(rgiSelections[iElement]);
			ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));

			// get a pointer to the active view in the frame
			CPartView* pView = (CPartView*)pFrame->GetActiveView();
			ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));
			// get a pointer to the view's document
			CPartDoc* pDoc = pView->GetDocument();

			// we only allow printing text documents, so make sure it's a text doc before printing
			if (IsViewPrintable(pView)) {
				// print the doc
// FUTURE: when ID_FILE_PRINT_DIRECT is working, we'll just send
// an ID_FILE_PRINT_DIRECT message to each view
#ifdef PRINT_DIRECT
				::SendMessage(pView->m_hWnd, WM_COMMAND, ID_FILE_PRINT, 0);
#else
/*
				ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTextDoc)));
				printAll = TRUE;
				::Print(AfxGetApp()->m_pMainWnd->m_hWnd, (CTextDoc*)pDoc);
*/
#endif
			}
		}

		delete [] rgiSelections;

		UpdateUI();
	}
}
#endif

void COpenDocsDlg::OnOpenDocumentsSave() 
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_OPEN_DOCUMENTS_LIST);
	int cElements = pListBox->GetSelCount();

	if (cElements > 0)
	{
		CWaitCursor waitCursor;

		// get the selected documents
		int *rgiSelections = new int[cElements];
		pListBox->GetSelItems(cElements, rgiSelections);

		// iterate through the selected documents
		for (int iElement = 0; iElement < cElements; iElement++) {
			// get the pointer to the frame
			CPartFrame* pFrame;
			pFrame = (CPartFrame*)pListBox->GetItemData(rgiSelections[iElement]);
			ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));

			// only save frame's that are saveable
			if (IsFrameSaveable(pFrame)) {
				// get a pointer to the active view in the frame
				CPartView* pView = (CPartView*)pFrame->GetActiveView();
				ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));
				// get a pointer to the view's document
				CPartDoc* pDoc = pView->GetDocument();

				// only bother to save if the document's been modified
				if (pDoc->IsModified()) {
					// Olympus:14702
					// store the current directory and restore it after saving
					// in case the directory is changed during Save As
					TCHAR szDir[MAX_PATH];
					DWORD dwResult = GetCurrentDirectory(MAX_PATH-1, szDir);

					// If the Doc is untitled, then call DoSave (NULL) which
					// will bring a a file name dialog.  This is just like OnFileSave
					// The caller is responsible for restoring the origninal view:
					if (pDoc->GetPathName().IsEmpty() && pDoc->GetFirstViewPosition() != NULL) {
						pDoc->DoSave(NULL);
					}
					else {
						pDoc->DoSave(pDoc->GetPathName());
					}

					// as long as we got a valid directory before saving, restore it
					if (dwResult) {
						// set the directory back to its original
						SetCurrentDirectory(szDir);

						// get a pointer to the main frame
						CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
						ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

						// update all window titles
						pMainFrame->OnDirChange();
					}
				}
			}
		}

		FillListBox();

		// restore selection
		for (iElement = 0; iElement < cElements; iElement++) {
			pListBox->SetSel(rgiSelections[iElement], TRUE);
		}

		delete[] rgiSelections;

		UpdateUI();
	}
}

BOOL COpenDocsDlg::MinimizeUnselected(void)
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_OPEN_DOCUMENTS_LIST);
	int cSelElements = pListBox->GetSelCount();
	int cElements=pListBox->GetCount();

	if (cSelElements > 1)
	{
		CPartFrame* pFrame;
		pFrame = (CPartFrame*)pListBox->GetItemData(0);

		CWnd *pMDIClient=pFrame->GetParent();

		pMDIClient->SetRedraw(FALSE);

		// iterate through the selected documents
		for (int iElement = 0; iElement < cElements; iElement++)
		{
			// get the pointer to the frame
			CPartFrame* pFrame;
			pFrame = (CPartFrame*)pListBox->GetItemData(iElement);
			// is the item selected
			BOOL bSel=pListBox->GetSel(iElement);

			// if not, minimize the child
			if(!bSel)
			{
				if(!pFrame->IsIconic())
				{
					pFrame->CloseWindow();
				}
			}
			else
			{
				if(pFrame->IsIconic())
				{
					pFrame->OpenIcon();
				}
			}
		}

		pMDIClient->SetRedraw(TRUE);

		CRect rectClient;
		pMDIClient->GetWindowRect(&rectClient);
		::InvalidateRect(NULL, rectClient, TRUE);

		return TRUE;
	}
	else
	{
		// really, the button should be disabled if this branch can be reached
		ASSERT(FALSE);
		return FALSE;
	}
}

void COpenDocsDlg::OnOpenDocumentsTileHorz() 
{
	if(MinimizeUnselected())
	{
		// tile all the remaining (selected) ones
		CMainFrame *pMainFrame=(CMainFrame *)AfxGetMainWnd();
		pMainFrame->MDITile(MDITILE_HORIZONTAL);
	}
}

void COpenDocsDlg::OnOpenDocumentsTileVert() 
{
	if(MinimizeUnselected())
	{
		// tile all the remaining (selected) ones
		CMainFrame *pMainFrame=(CMainFrame *)AfxGetMainWnd();
		pMainFrame->MDITile(MDITILE_VERTICAL);
	}
}

void COpenDocsDlg::OnSelChangeDocList() 
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_OPEN_DOCUMENTS_LIST);
	int cElements = pListBox->GetSelCount();

	m_bPrintable = FALSE;
	m_bSaveable = FALSE;

	if (cElements > 0) {
		// get the selected documents
		int *rgiSelections = new int[cElements];
		pListBox->GetSelItems(cElements, rgiSelections);

		// iterate through the selected documents
		for (int iElement = 0; iElement < cElements; iElement++) {
			// get the pointer to the frame
			CPartFrame* pFrame;
			pFrame = (CPartFrame*)pListBox->GetItemData(rgiSelections[iElement]);
			ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));

			// get a pointer to the active view in the frame
			CPartView* pView = (CPartView*)pFrame->GetActiveView();
			ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));

			// if the document's printable, set the flag
			if (IsViewPrintable(pView)) {
				m_bPrintable = TRUE;
			}
			// if the view's saveable, set the flag
			if (IsFrameSaveable(pFrame)) {
				m_bSaveable = TRUE;
			}
		}

		delete[] rgiSelections;
	}

	UpdateUI();
}


// Helper used by other helper (InsertStringIntoListBox) below.  Inserts
//  a string into a range (inclusive), sorted within that range.
static int InsertStringInListBoxRange(CListBox* pListBox, LPCTSTR szEntry,
									  int nMin, int nMax)
{
	CString strIter;
	for (int i=nMin; i <= nMax; i++)
	{
		pListBox->GetText(i, strIter);

		// Search 'til we first get after szEntry in alphabetical order
		if (_tcsicmp(strIter, szEntry) < 0)
			continue;

		break;		// i is now the place for szEntry to be
	}

	// Either the loop went to the end or we broke.  Either way,
	//  i is where szEntry should be inserted.
	int nRet = pListBox->InsertString(i, szEntry);
	ASSERT (nRet == i);
	return nRet;
}

// Helper used in FillListBox below.  We want to organize the open docs
//  list so that all files in the CWD are shown w/out paths, but alphabetized
//  amongst themselves at the top.  Then, underneath them are all other docs
//  w/ their full paths, alphabetized amongst themselves.  We don't want docs
//  from the first set being sorted with the others, since that makes the
//  dialog less usable.
static int InsertStringIntoListBox(CListBox* pListBox, LPCTSTR szEntry,
									BOOL bFullPath, int& nFirstFullPath)
{
	if (bFullPath)
	{
		// Insert sorted at bottom
		int nMax = pListBox->GetCount() - 1;
		return InsertStringInListBoxRange(pListBox, szEntry, nFirstFullPath, nMax);
	}
	else
	{
		// Insert sorted at top
		int nRet = InsertStringInListBoxRange(pListBox, szEntry, 0, nFirstFullPath-1);
		nFirstFullPath++;		// This pushes down the first fullpath entry
		return nRet;
	}
}

// Gets the number n in the title "MyDoc:n"
static LPCTSTR GetWndNum(LPCTSTR szTitle)
{
	LPCTSTR szRet = _tcsrchr(szTitle, _T(':'));
	if (szRet == NULL)
		return NULL;

	TCHAR* pch = _tcsinc(szRet);
	while (*pch != NULL)
	{
		if (!_istdigit(*pch) && *pch != '*' && !_istspace(*pch))
			return NULL;
		pch = _tcsinc(pch);
	}
	return szRet;
}

void COpenDocsDlg::FillListBox(void)
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_OPEN_DOCUMENTS_LIST);

	// Find the width of the longest line in the list box
	int cxMaxLineLen = 0;
	int cxCharWidth;
	TEXTMETRIC metrics;
	CDC *pDC;
	CFont *pFont;
	pDC = pListBox->GetDC();
	pFont = GetFont();
	pDC->SelectObject(pFont);
	pDC->GetTextMetrics(&metrics);
	cxCharWidth = metrics.tmMaxCharWidth;

	// Prevent flickering
	pListBox->LockWindowUpdate();

	pListBox->ResetContent();

	// iterate through all MDI windows except: FILTER_DEBUG | FILTER_PROJECT
	int nFilter = FILTER_PROJECT;
	CPartFrame* pMDIChild = theApp.MDITypedGetActive(nFilter);
	int nFirstFullPath = 0;
	while (pMDIChild != NULL)
	{
		CString strTitle;
		int nIndex;
		// get the window's title
		pMDIChild->GetWindowText(strTitle);
		BOOL bFullPath = (strTitle.Find(_T('\\')) != -1);
		if (bFullPath && strTitle.Find(_T('-')) == -1)
		{
			// if this is a non-resource editor window, use the path name instead of the frame title
			// because the frame title for non-resource editor windows is (potentially) truncated
			// but the path name for a resource editor window is just the name of the .rc file
			// is this a resource editor window (contains '-' in title)?
			// HACK: This was bad enough to scare people away from putting this code in
			//  toward the end of V4 (the hack to determine whether a window was a resource
			//  window (and thus to use its frame title instead of doc path as the listbox
			//  entry) based on the occurrence of a '-'.  But, eh, doesn't look THAT bad;
			//  plus, now the docs list will actually be useful since we can have
			//  non-abbreviated full paths in there.

			// don't use the GetWindowText title, use the path
			// get a pointer to the active view in the frame
			CPartView* pView = (CPartView*)pMDIChild->GetActiveView();
			ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));
			// get a pointer to the view's document
			CPartDoc* pDoc = pView->GetDocument();
			// get the doc's path name
			strTitle = pDoc->GetPathName();
			// if the path name is empty (i.e. new window), go back to the window title
			if (strTitle.IsEmpty()) 
				pMDIChild->GetWindowText(strTitle);

			// only add following niceties to titles derived from the path name;
			// all titles from the frame window will already have these niceties
			else 
			{
				// Append extra niceties.  First, ":n" for multiple windows
				CString strFrameTitle;
				pMDIChild->GetWindowText(strFrameTitle);
				LPCTSTR szWndNum;
				if ((szWndNum = GetWndNum(strFrameTitle)) != NULL)
				{
					// There's a ":n", so add it if it's not in the title already
					LPCTSTR szDocNum;
					if (((szDocNum = GetWndNum(strTitle)) == NULL)
						|| _tcscmp(szDocNum, szWndNum))
					{
						strTitle += szWndNum;
					}
				}

				// Next nicety: is the document dirty? (Don't do this if we appended
				//  a window number, since the '*' will be included in the text
				//  grabbed after the ':'.
				else if (pDoc->IsModified()) 
					strTitle += _T(" *");
			}
		}

		// add the window to the list
		nIndex = InsertStringIntoListBox(pListBox, strTitle, bFullPath, nFirstFullPath);
		if (nIndex >= 0)
		{
			// store a pointer to the frame
			pListBox->SetItemData(nIndex, (DWORD)pMDIChild);
			// save the new max line length
			cxMaxLineLen = max(cxMaxLineLen, (pDC->GetTextExtent(strTitle, strTitle.GetLength())).cx);
		}

		// get the next MDI window
		pMDIChild = theApp.MDITypedGetNext(pMDIChild, nFilter);
	}

	// Set the width of the list box in case we have a *long* pathname
	// add cxCharWidth because the length of the longest line is always a bit too short
	pListBox->SetHorizontalExtent(cxMaxLineLen + cxCharWidth);

	pListBox->UnlockWindowUpdate();

	pListBox->ReleaseDC(pDC);
}	

void COpenDocsDlg::UpdateUI(void)
{
	// get a pointer to the list box of open documents
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_OPEN_DOCUMENTS_LIST);
	int cElements = pListBox->GetCount();
	int cSelElements = pListBox->GetSelCount();

	// get pointers to the buttons
	CButton* pbtnActivate = (CButton*)GetDlgItem(IDC_OPEN_DOCUMENTS_ACTIVATE);
	CButton* pbtnClose = (CButton*)GetDlgItem(IDC_OPEN_DOCUMENTS_CLOSE);
#ifdef PRINT_DOC_LIST
	CButton* pbtnPrint = (CButton*)GetDlgItem(IDC_OPEN_DOCUMENTS_PRINT);
#endif
	CButton* pbtnSave = (CButton*)GetDlgItem(IDC_OPEN_DOCUMENTS_SAVE);
	CButton* pbtnTileHorz = (CButton*)GetDlgItem(IDC_OPEN_DOCUMENTS_TILE_HORZ);
	CButton* pbtnTileVert = (CButton*)GetDlgItem(IDC_OPEN_DOCUMENTS_TILE_VERT);

	// Make sure the buttons are properly enabled/disabled
	pbtnActivate->EnableWindow(cSelElements == 1);
	pbtnClose->EnableWindow(cElements > 0);
#ifdef PRINT_DOC_LIST
	pbtnPrint->EnableWindow(cElements > 0 && m_bPrintable);
#endif
	pbtnSave->EnableWindow(cElements > 0 && m_bSaveable);
	pbtnTileHorz->EnableWindow(cSelElements > 1);
	pbtnTileVert->EnableWindow(cSelElements > 1);

	// remove the default style from the buttons
	pbtnClose->SetButtonStyle(pbtnClose->GetButtonStyle() & ~BS_DEFPUSHBUTTON);
#ifdef PRINT_DOC_LIST
	pbtnPrint->SetButtonStyle(pbtnPrint->GetButtonStyle() & ~BS_DEFPUSHBUTTON);
#endif
	pbtnTileHorz->SetButtonStyle(pbtnTileHorz->GetButtonStyle() & ~BS_DEFPUSHBUTTON);
	pbtnTileVert->SetButtonStyle(pbtnTileVert->GetButtonStyle() & ~BS_DEFPUSHBUTTON);

	// make the OK button the default
	SetDefID(IDC_OPEN_DOCUMENTS_ACTIVATE);

	// set focus to the list box
	pListBox->SetFocus();
}

#ifdef PRINT_DIRECT
class CTestCmdUI: public CCmdUI
{
public: // re-implementations only
	virtual void Enable(BOOL bOn) { m_bEnableChanged = bOn; }
};
#endif

BOOL COpenDocsDlg::IsViewPrintable(CPartView* pView)
{
	// get a pointer to the view's document
	CPartDoc* pDoc = pView->GetDocument();

// FUTURE: when ID_FILE_PRINT_DIRECT is working, we'll query each
// doc/view to see it supports printing
#ifdef PRINT_DIRECT
	CTestCmdUI print;
	print.m_nID = ID_FILE_PRINT;

	// check if the view can print
	print.DoUpdate(pView, TRUE);
	if (print.m_bEnableChanged) {
		return TRUE;
	}
	// we probably only need to query the view, because I think the
	// OnUpdateCommandUI will be routed to the doc if the view doesn't
	// handle it
	// check if the doc can print
	print.DoUpdate(pDoc, TRUE);
	if (print.m_bEnableChanged) {
		return TRUE;
	}
	return FALSE;
#else
#ifdef PRINT_DOC_LIST
	return pDoc->IsKindOf(RUNTIME_CLASS(CTextDoc));
#else
	return FALSE;
#endif
#endif
}

BOOL COpenDocsDlg::IsFrameSaveable(CPartFrame* pFrame)
{
	return !pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildDock));
}

void COpenDocsDlg::SelectActiveChild(void)
{
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_OPEN_DOCUMENTS_LIST);
	int cElements = pListBox->GetCount();
	
	// get a pointer to the main frame
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

	// get the active child
	CMDIChildWnd* pActiveChild = pMainFrame->MDIGetActive();
	
	// deselect all items
	pListBox->SetSel(-1, FALSE);
	
	// select the active child 
	for (int iElement = 0; iElement < cElements; iElement++) {
		CPartFrame* pFrame = (CPartFrame*)pListBox->GetItemData(iElement);
		ASSERT(pFrame->IsKindOf(RUNTIME_CLASS(CPartFrame)));

		// is this the active child
		if (pFrame->m_hWnd == pActiveChild->m_hWnd) {
			pListBox->SetSel(iElement, TRUE);
		}
	}
}
