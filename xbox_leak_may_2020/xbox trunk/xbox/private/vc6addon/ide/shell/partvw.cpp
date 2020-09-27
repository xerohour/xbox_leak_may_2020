// partvw.cpp : implementation of the CPartView class
//

#include "stdafx.h"
#include "main.h"

#include "imeutil.h"
#include "find.h"
#include "remi.h"
#include "resource.h"
#include "bardockx.h"

#include "toolexpt.h"

IMPLEMENT_DYNCREATE(CPartView, CSlobWnd)
IMPLEMENT_DYNCREATE(CDockablePartView, CPartView)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPartView

BEGIN_MESSAGE_MAP(CPartView, CSlobWnd)
	//{{AFX_MSG_MAP(CPartView)
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FIND_DLG, OnEditFind)
	ON_COMMAND(ID_EDIT_FIND_NEXT, OnEditFindNext)
	ON_COMMAND(ID_EDIT_FIND_PREV, OnEditFindPrev)
	ON_COMMAND(IDM_EDIT_TOGGLE_CASE, OnToggleFindCase)
	ON_COMMAND(IDM_EDIT_TOGGLE_RE, OnToggleFindRE)
	ON_COMMAND(IDM_EDIT_TOGGLE_WORD, OnToggleFindWord)
	ON_COMMAND(IDM_EDIT_TOGGLE_SEARCH_ALL_DOCS, OnToggleSearchAllDocs)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateFindReplace)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_DLG, OnUpdateFindReplace)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_NEXT, OnUpdateFindReplace)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_PREV, OnUpdateFindReplace)

	ON_UPDATE_COMMAND_UI(IDM_EDIT_TOGGLE_CASE, OnUpdateToggleMatchCase)
	ON_UPDATE_COMMAND_UI(IDM_EDIT_TOGGLE_WORD, OnUpdateToggleMatchWord)
	ON_UPDATE_COMMAND_UI(IDM_EDIT_TOGGLE_RE, OnUpdateToggleMatchRegEx)
	ON_UPDATE_COMMAND_UI(IDM_EDIT_TOGGLE_SEARCH_ALL_DOCS, OnUpdateToggleSearchAllDocs)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartView construction/destruction

CPartView::CPartView()
{
	m_pPacket = NULL;
}

CPartView::~CPartView()
{
}

CPartTemplate* CPartView::GetDocTemplate()
{
	CPartDoc* pDoc = GetDocument();
	if (pDoc == NULL)
		return NULL;

	return (CPartTemplate*) pDoc->GetDocTemplate();
}

int CPartView::GetAssociatedFiles(CStringArray & saFiles, BOOL /* bSelected = TRUE */)
{
	const CDocument * pDoc = GetDocument();
	if ((pDoc == NULL) || (pDoc->GetPathName().IsEmpty()))
		return 0;

	CPath path;
	if (!path.Create(pDoc->GetPathName()))
	{
		return 0;
	}

	saFiles.Add((LPCTSTR)path);
	return 1;
}

// Tries to position the dialog outside the view window.
// Returns TRUE if it could reposition the dialog
// Note: CTextView's  does more than this, but this might get the dialog out of the way at startup

BOOL CPartView::SetDialogPos(BOOL fFirstTime, HWND hDlg)
{
	if (fFirstTime)
	{
		// try moving dialog completely outside of view window
		CRect rectWindow, rectClient;
		RECT rectDlg;
		int x, y, xDlg, yDlg;
		int cxScreen = GetSystemMetrics(SM_CXSCREEN);
		int cyScreen = GetSystemMetrics(SM_CYSCREEN);


		// get rectangle for dialog box
		::GetWindowRect(hDlg, &rectDlg);
		xDlg = rectDlg.right - rectDlg.left;
		yDlg = rectDlg.bottom - rectDlg.top;

		// get window rectangle in screen coordinates.
		GetWindowRect(rectWindow);
		// get window rectangle in client coordinates (excludes scroll bars, etc).
		GetClientRect(rectClient);

		// adjust screen coordinate rectangle.Allows dialog to overlay scroll bars or title
		rectWindow.bottom = rectWindow.top + rectClient.bottom;
		rectWindow.right = rectWindow.left + rectClient.right;

		// forcing repositioning, attempt to position dialog
		// outside of editor window client window area.

		// if the dialog can be repositioned outside window rectangle, do it.
		if (yDlg < rectWindow.top)	// above
		{
			x = (cxScreen - xDlg) / 2;
			y = rectWindow.top - yDlg;
			::MoveWindow(hDlg, x, y, xDlg, yDlg, TRUE);
			return TRUE;
		}
		else if (yDlg < (cyScreen - rectWindow.bottom + 2))  // below
		{
			x = (cxScreen - xDlg) / 2;
			y = rectWindow.bottom + 2;
			::MoveWindow(hDlg, x, y, xDlg, yDlg, TRUE);
			return TRUE;
		}
		else if (xDlg < rectWindow.left)	// left
		{
			x = rectWindow.left - xDlg;
			y = (cyScreen - yDlg) / 2;
			::MoveWindow(hDlg, x, y, xDlg, yDlg, TRUE);
			return TRUE;
		}
		else if (xDlg < (cxScreen - rectWindow.right + 2))	// right
		{
			x = rectWindow.right + 2;
			y = (cyScreen - yDlg) / 2;
			::MoveWindow(hDlg, x, y, xDlg, yDlg, TRUE);
			return TRUE;
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPartView diagnostics

#ifdef _DEBUG
void CPartView::AssertValid() const
{
	CSlobWnd::AssertValid();
}

void CPartView::Dump(CDumpContext& dc) const
{
	CSlobWnd::Dump(dc);
}

CPartDoc* CPartView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument == NULL ||
		m_pDocument->IsKindOf(RUNTIME_CLASS(CPartDoc)));
	return (CPartDoc*) m_pDocument;
}
#endif //_DEBUG


BOOL CPartView::CanClose()
{
	return TRUE;
}

void CPartView::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch(nID & 0xFFF0)
	{
		case SC_MOUSEMENU:
			CBMenuPopup::SetSystemClick(GetMessageTime(), CPoint(LOWORD(lParam), HIWORD(lParam)));
		case SC_KEYMENU:
			if(	((nID & 0xFFF0)==SC_MOUSEMENU || lParam=='-') &&
				CASBar::s_pMenuBar->GetSafeHwnd()!=NULL &&
				Menu::IsShowingCmdBars())
			{
				CASBar::s_pMenuBar->KeyDown(VK_SUBTRACT,0,0);
				return;
			}
			break;

		// why do this? Because it happens when you single click on an icon before the double
		// click to deminimize the icon
		case SC_MOVE:
			CBMenuPopup::SetSystemClick(GetMessageTime(), CPoint(LOWORD(lParam), HIWORD(lParam)));
			break;

	}
	CSlobWnd::OnSysCommand(nID, lParam);
}

void CPartView::OnLoadWorkspace()
{
	// Override this to load your init-data from the workspace...
}

void CPartView::OnFindNext(BOOL* pbSuccess /*=NULL*/)
{	// Combo box search
	if(findReplace.regExpr)
	{
		findReplace.CompileRegEx(GetRegExprSyntax());

		if(findReplace.pat == NULL) // Problem in the regular expression
		{
			ErrorBox(IDS_REGEXPR_INVALID, findReplace.findWhat);
			if (pbSuccess != NULL)
				*pbSuccess = FALSE;
			return;
		}
	}
	
	BOOL bFound = DoFind();

	if (pbSuccess != NULL)
		*pbSuccess = bFound;
}

BOOL CPartView::DoFind()
{
	BOOL bFound = FindString(&findReplace);

	if(!bFound && !findReplace.loop) // If we are searching through all documents
	{								// then open the next view and search it
		// get a pointer to the main frame
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT(pMainFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));

		DWORD dwNextWin = findReplace.goUp ? GW_HWNDPREV : GW_HWNDNEXT;
		DWORD dwFirstWin = findReplace.goUp ? GW_HWNDLAST : GW_HWNDFIRST;
		CPartFrame* pMDIActiveChild = (CPartFrame*)pMainFrame->MDIGetActive();
		CPartFrame* pMDIChild = pMDIActiveChild;
		while( TRUE )
		{
			pMDIChild = (CPartFrame *)pMDIChild->GetWindow(dwNextWin);
			if(pMDIChild == NULL) // we got to the end of the list
				pMDIChild = (CPartFrame *)pMDIActiveChild->GetWindow(dwFirstWin);

			if(pMDIChild == pMDIActiveChild) // we completed the loop
				break;

			if(!IsValidMDICycleMember(pMDIChild))
				continue;

			CPartView *pView = (CPartView *)pMDIChild->GetActiveView();
			ASSERT(pView->IsKindOf(RUNTIME_CLASS(CPartView)));
			ASSERT(pView != this); // we should never see the active view in this loop
			DWORD dwFlags, dwFlags2;
			if(!GetFindReplaceFlags(&dwFlags, &dwFlags2) || (dwFlags & FDS_SEARCHALL) == 0)
				continue;	// the view does not support this type of search

			findReplace.startFromCursor = FALSE;
			bFound = pView->FindString(&findReplace);
			findReplace.startFromCursor = TRUE;
			if(bFound)
			{
				CDockWorker::LockWorker(FALSE);
				CDockWorker::s_pActiveDocker = NULL;
				theApp.SetActiveView(pView->m_hWnd);

				InvalidateRect(0); // test

				if(!findReplace.goUp) // move previous window to the bottom of
								// the list, only if we're going down
					pMDIActiveChild->SetWindowPos(&wndBottom, 0,0,0,0, 
						SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

				break;	// exits the while loop.
			}
		}
		if(!bFound) // If not found if other views, look in initial view
		{
			findReplace.loop = TRUE;
			bFound = FindString(&findReplace);
			findReplace.loop = FALSE;
		}
	}

	if(!bFound)
	{
		CString str;
		if(findReplace.regExpr)
			MsgText(str, IDS_REG_EXPR_NOT_FOUND, findReplace.findWhat);
		else
			MsgText(str, IDS_STRING_NOT_FOUND, findReplace.findWhat);
		MsgBox(Error, str);
	}

	return bFound;
}


void CPartView::OnEditFind()
{
	DWORD dwFlags, dwFlags2;
	if(!GetFindReplaceFlags(&dwFlags, &dwFlags2))
		dwFlags = FDS_WHOLEWORD | FDS_MATCHCASE | FDS_REGEXP;  // default value

	CFindStringDlg dlg(dwFlags, theApp.m_pMainWnd);
	CString str;
	if(GetFindString(str))
		dlg.m_strFindString = str;
	dlg.DoModal();
}

void CPartView::OnEditFindNext()
{
	// If we don't have the input focus, then a find control might have it.
	// We have to grab the focus since the find control's  WM_KILLFOCUS handler
	// puts its current text into <findReplace.findWhat>.
  if (GetFocus() != this)
		SetFocus();

	findReplace.goUp = FALSE;
	if(findReplace.findWhat[0]!='\0')  // if the search string is not empty
		OnFindNext();
}

void CPartView::OnEditFindPrev()
{
	// If we don't have the input focus, then a find control might have it.
	// We have to grab the focus since the find control's  WM_KILLFOCUS handler
	// puts its current text into <findReplace.findWhat>.
	if (GetFocus() != this)
		SetFocus();

	findReplace.goUp = TRUE;
	if(findReplace.findWhat[0]!='\0')  // if the search string is not empty
		OnFindNext();
}

void CPartView::OnToggleSearchAllDocs(void)
{
	// note: findReplace.loop has the opposite semantics of search all docs
	findReplace.loop = !findReplace.loop;

	// Have two separate messages to ease translation:
	SetPrompt(findReplace.loop ? STA_SearchAllDocsOff : STA_SearchAllDocsOn);
}

void CPartView::OnToggleFindCase(void)
{
	findReplace.matchCase = !findReplace.matchCase;

	// Have two separate messages to ease translation:
//	StatusText(findReplace.matchCase ? STA_MatchCaseOn : STA_MatchCaseOff, STATUS_INFOTEXT, FALSE);
	SetPrompt(findReplace.matchCase ? STA_MatchCaseOn : STA_MatchCaseOff);
}

void CPartView::OnToggleFindRE(void)
{
	findReplace.regExpr = !findReplace.regExpr;

	// Have two separate messages to ease translation:
//	StatusText(findReplace.regExpr ? STA_MatchREOn : STA_MatchREOff, STATUS_INFOTEXT, FALSE);
	SetPrompt(findReplace.regExpr ? STA_MatchREOn : STA_MatchREOff);
}

void CPartView::OnToggleFindWord(void)
{
	findReplace.wholeWord = !findReplace.wholeWord;

	// Have two separate messages to ease translation:
//	StatusText(findReplace.wholeWord ? STA_MatchWordOn : STA_MatchWordOff, STATUS_INFOTEXT, FALSE);
	SetPrompt(findReplace.wholeWord ? STA_MatchWordOn : STA_MatchWordOff);
}

void CPartView::OnUpdateFindReplace(CCmdUI* pCmdUI)
{
	DWORD dwFlags, dwFlags2;
	if(!GetFindReplaceFlags(&dwFlags, &dwFlags2) || dwFlags == 0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CPartView::OnUpdateToggleMatchCase(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(findReplace.matchCase);
	OnUpdateFindReplace(pCmdUI);
}

void CPartView::OnUpdateToggleMatchWord(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(findReplace.wholeWord);
	OnUpdateFindReplace(pCmdUI);
}

void CPartView::OnUpdateToggleMatchRegEx(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(findReplace.regExpr);
	OnUpdateFindReplace(pCmdUI);
}

void CPartView::OnUpdateToggleSearchAllDocs(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!findReplace.loop); // 'loop' has opposite semantics
	OnUpdateFindReplace(pCmdUI);
}

BOOL CPartView::FindString(FINDREPLACEDISK *pfr)
{
	return FALSE;
}
BOOL CPartView::ReplaceString(FINDREPLACEDISK *pfr, BOOL bReplaceAll)
{
	return FALSE;
}
BOOL CPartView::GetFindSelectionRect(CRect* prc)
{
	return FALSE;
}
BOOL CPartView::GetFindString(CString& rstr)
{
	return FALSE;
}
BOOL CPartView::GetFindReplaceFlags(DWORD *pdwFlagsFind, DWORD *pdwFlagsReplace)
{
	return FALSE;
}
BOOL CPartView::MarkAll(FINDREPLACEDISK *pfr)
{
	return FALSE;
}

BOOL CPartView::GetSelectionInterface(GUID* pguid, IUnknown** piUnk)
{
	return FALSE;
}

UINT NEAR WM_VIEWACTIVATE = RegisterMessage("ViewActivate");

void CPartView::OnActivateView(BOOL bActivate, CView* pActivateView, 
	CView* pDeactiveView)
{
	if (CPartFrame::IsWorkspaceLocked())
		return;

	SendMessage(WM_VIEWACTIVATE, (WPARAM)bActivate, 
		(LPARAM)(bActivate ? pDeactiveView : pActivateView));

	if (bActivate)
		ShowSprites();
	else
		HideSprites();

	if (!bActivate || UsesIME())
		theIME.EnableIME( TRUE );
	else
		theIME.EnableIME( FALSE );

	CSlobWnd::OnActivateView(bActivate, pActivateView, pDeactiveView);

	// try to activate the wizard bar

	LPWIZARDBAR pWBIFace =	g_IdeInterface.GetWizBarIFace();  // released on package exit

	if (pWBIFace != NULL)
	{
		HRESULT hr;

		hr = pWBIFace->FExistWizBar();
		if (hr == S_OK)
			pWBIFace->ActivateWizBar(bActivate); 
	}
}


BOOL CPartView::PreTranslateMessage(MSG* pMsg)
{
	if (CSlobWnd::PreTranslateMessage(pMsg))
		return TRUE;

	CKeymap* pKeymap = GetKeymap();
	if (pKeymap != NULL)
	{
		if (pKeymap->TranslateMessage(GetParentFrame(), pMsg))
			return TRUE;
	}

	return FALSE;
}

void CPartView::RecalculateIME()
{
	// Override to position the IME after the mainfrm is moved or sized
}

#ifndef NO_VBA

// GetSelectionObject -- select the specified Range (if present); then return an object
//		(currently also a Range) representing the current section.
LPDISPATCH
CPartView::GetSelectionObject(LPDISPATCH pdisp)
{
	return NULL;	// generic version does nothing; overrides do all the work
}

#endif	// NO_VBA

/////////////////////////////////////////////////////////////////////////////
// CDockablePartView


CDockablePartView::CDockablePartView()
{
}

CDockablePartView::~CDockablePartView()
{
}


BEGIN_MESSAGE_MAP(CDockablePartView, CPartView)
	//{{AFX_MSG_MAP(CDockablePartView)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDockablePartView diagnostics

#ifdef _DEBUG
void CDockablePartView::AssertValid() const
{
	CPartView::AssertValid();
}

void CDockablePartView::Dump(CDumpContext& dc) const
{
	CPartView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDockablePartView message handlers

void CDockablePartView::OnFileClose() 
{
	CFrameWnd* pFrame = GetParentFrame();
	if (pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildDock)))
		pFrame->SendMessage(WM_CLOSE);		
}

void CDockablePartView::OnUpdateFileClose(CCmdUI* pCmdUI) 
{
	CFrameWnd* pFrame = GetParentFrame();
	pCmdUI->Enable(pFrame != NULL &&
		pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildDock)));
}

//      CDockablePartView::OnMouseActivate
//              The CView implementation of this function will cause asserts
//              as well as set the CMDIFrameWnd::m_pViewActive member, so we
//              need to override back to normal window behavior.

int CDockablePartView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}



static BOOL IsSeparator(LPCTSTR lpchWord, LPCTSTR lpchBorder);

BOOL FindInString( FINDREPLACEDISK *pfr, const char *szBuf,
		BOOL backwardsSearch, const char **pszStart, ULONG *pcbSize)
{
	int ichCur;
	char cLower = (char)_totlower(pfr->findWhat[0]);
	char cUpper = (char)_totupper(cLower);
	int cbFind = _tcslen(pfr->findWhat);
	int cbBuf = _tcslen(szBuf);
	BOOL fFound = FALSE;
	BOOL fWasEverFound = FALSE; // Is used for backwards searches.

  if (pszStart && *pszStart)
	{
		ichCur = (*pszStart) - szBuf;
	}
	else
		ichCur = 0;

	while(ichCur <= cbBuf) // DevStd #295: can be non-null, since BOL/EOL are valid search targets
	{
		int ichStart = ichCur;
		int ichEnd = cbBuf;

		if(pfr->regExpr)
		{
			fFound = RESearch(szBuf, (ULONG *)&ichStart, (ULONG *)&ichEnd,
								pfr->pat, pfr->findWhat, &(pfr->bWasLineMatch));
			if(fFound)
				ichCur = ichStart + _tclen (szBuf + ichStart); // Increments our cursor
		}
		else
		{
			fFound = FALSE;
			while( ichCur <= cbBuf - cbFind )
			{
				const char *pch;
				do
				{
					if((cLower == *(szBuf + ichCur)) || (cUpper == *(szBuf + ichCur)))
					{
						ichStart = ichCur;
						break;
					}
					ichCur += _tclen (szBuf + ichCur);	// Increments the cursor
				} while(ichCur <= cbBuf - cbFind );

				if( ichCur > cbBuf - cbFind ) // we did not find a match
					break;

				ichEnd = ichStart + cbFind;
				ichCur = ichStart + _tclen (szBuf + ichStart);	// Increments the cursor
				if(pfr->matchCase)
				{
					if( _tcsncmp(szBuf + ichStart, pfr->findWhat, cbFind) != 0 )
						continue;
				}
				else
				{
					if( _tcsnicmp(szBuf + ichStart, pfr->findWhat, cbFind) != 0 )
						continue;
				}
				fFound = TRUE;
				break;
			}

		}

		// if we've found a match, now make sure it's a standalone word
		if(fFound && pfr->wholeWord)
		{
			if( ((ichStart != 0) && 
				!IsSeparator(szBuf + ichStart, 
				_ftcsdec(szBuf, szBuf + ichStart))) ||		// character before
				((ichEnd < cbBuf) && 
				!IsSeparator (_ftcsdec(szBuf, szBuf + ichEnd),
				szBuf + ichEnd ) ))	// character after
			{
				fFound = FALSE;
				continue;
			}
		}

		if (fFound)
		{
			fWasEverFound = TRUE;
			if(pszStart != NULL)
				*pszStart = szBuf + ichStart;
			if(pcbSize != NULL)
				*pcbSize = ichEnd - ichStart;
			if(!backwardsSearch)	// If we're going down, we're finished
			{						// otherwize, carry on the search till the end
				break;				// to find the last occurence
			}
				
		}
		else break; // Not found
	}

	return fWasEverFound;
}


/*
**	FUNCTION:	IsSeparator
**
**	PURPOSE:	Given a character (either at the beginning or end of a "word")
**				and the character that borders it, determines if the border
**				character sufficiently defines a separator for the word.
**
**	NOTES:		The following rules are applied:
**
**			1.  If the border character is alphanumeric, then it
**			is only a valid separator if the word character is a DBC character.
**/
//To test if a character belong to the C/Pascal Alphanumeric set
#define CHARINALPHASET(c) (IsCharAlphaNumeric(c) || c == '_')

static BOOL IsSeparator (LPCTSTR lpchWord, LPCTSTR lpchBorder)
{
	if (CHARINALPHASET(*lpchBorder))
		{
		if (theApp.m_fOEM_MBCS)
			{
			if ((_ftclen(lpchWord) > 1 ) ||
					_ismbbkana(*(unsigned char *)lpchWord))
				return TRUE;
			else
				return FALSE;
			}
		else
			return FALSE;
		}
	else
		return TRUE;
}

