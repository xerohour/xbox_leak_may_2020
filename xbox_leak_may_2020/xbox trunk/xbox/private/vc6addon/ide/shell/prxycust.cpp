#include "stdafx.h"

#include "prxycust.h"

#include "barglob.h"
#include "custbtn.h"
#include "menuprxy.h"
#include "shell.h"
#include "bardockx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

UINT DSM_COMMANDREMOVED = RegisterMessage("DevStudioCommandRemoved");

///////////////////////////////////////////////////////////////////////////////
//      CToolCustomizer
//              The tool customizer is responisble for managing the customizing of
//              toolbars, which constists of managing the selection, button moving,
//              and button sizing.
//

//      CToolCustomizer::CToolCustomizer
//              Construction.

CToolCustomizer::CToolCustomizer(CDockManager* pManager, CDlgTab* pDlg,
	const CObArray& aBars, BOOL bTemp /*=FALSE*/, BOOL bUndo /*=FALSE*/)
	: m_aBars(aBars)
{
	m_pManager = pManager;
	m_pDialog = pDlg;
	m_pSelectBar = NULL;
	m_nSelectIndex = 0;
	m_bTemp = bTemp;
	m_bUndo = bUndo;
	
	if (m_pDialog != NULL)
	{
		ASSERT_VALID(m_pManager);
		m_pManager->EnableManagerInput(FALSE);
		SetPropertyBrowserVisible(FALSE);
	}
}

//      CToolCustomizer::~CToolCustomizer
//              Destruction. Turn off the selection, update the visible command cache,
//              and re-enable input to dockable windows.

CToolCustomizer::~CToolCustomizer()
{
	SetSelection(NULL, 0);

	// if there's a dropped menu, dedrop it: !!! REVIEW: HACK
	if(	CASBar::s_pCustomizer==this &&
		CCustomBar::s_pDroppedBar!=NULL)
	{
		CCustomBar::s_pDroppedBar->ExpandItem(CCustomBar::s_nDroppedItem, FALSE);
	}

	if (m_pDialog != NULL)
	{
		ASSERT_VALID(m_pManager);
		m_pManager->EnableManagerInput(TRUE);

		// Re-enable windows in window menu.
		CMDIFrameWnd* pFrame = (CMDIFrameWnd*) m_pManager->m_pFrame;
		if (pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
			::SendMessage(pFrame->m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);

		BOOL bPopProp;
		theApp.m_theAppSlob.GetIntProp(P_PoppingProperties, bPopProp);

		SetPropertyBrowserVisible(!bPopProp);
	}
}

//      CToolCustomizer::SetSelection
//              Changes the selection (fuzzy rect around button), and stores the new
//              location.
void CToolCustomizer::SetSelection(CToolBarProxy* pBar, int nIndex)
{
	if (m_pSelectBar == NULL && m_nSelectIndex == -1)
		return; // Locked selection.
	if (m_pSelectBar == pBar && m_nSelectIndex == nIndex)
	return;

	if (m_pSelectBar != NULL && m_nSelectIndex!=-1)
	{
		CClientDC dcClient(m_pSelectBar->GetWindow());
		DrawSelection(&dcClient);
	}
	
	m_pSelectBar = pBar;
	m_nSelectIndex = nIndex;
	
	if (m_pSelectBar != NULL)
	{
		m_pSelectBar->OnSelChange(nIndex);

		CClientDC dcClient(m_pSelectBar->GetWindow());
		DrawSelection(&dcClient);
	}

	// In 4.x, there was some special-case code here to update the button description
	// whenever the selection was changed. In Orion, we want change notification in
	// another place too (the choose button dialog), so I'm sending a message
	// to the parent window, which can pick it up and perform any required action.
	// There are now 2 possible recipients; we only send it twice if they're different
	// martynl 2Apr96

	CWnd *pParent=NULL;
	if(m_pSelectBar)
	{
		pParent=pBar->GetWindow()->GetParent();

		if(pParent)
		{
			pParent->SendMessage(DSM_BARSELECTCHANGING, nIndex, (LPARAM)pBar);
		}
	}

	if( m_pDialog && 
		pParent->GetSafeHwnd()!=m_pDialog->GetSafeHwnd())
	{
		m_pDialog->SendMessage(DSM_BARSELECTCHANGING, nIndex, (LPARAM)pBar);
	}

	// Hide any destination that would be invalid for this selection.
	if(m_pSelectBar &&
		nIndex)
	{
		int nId=m_pSelectBar->GetButtonCmdID(nIndex);

		if(nId)
		{
			CTE *pCTE=theCmdCache.GetCommandEntry(nId);
			if(pCTE)
			{
				if(pCTE->flags & CT_MENU)
				{
					CASBar::HideInvalidDestination(nId);
				}
			}
		}
	}
}

//      CToolCustomizer::DeleteSelection
//              Deletes the selected button from it's toolbar.

BOOL CToolCustomizer::DeleteSelection(BOOL bDeferRemove)
{
	if (m_pSelectBar == NULL || m_pSelectBar->IsInDialog())
		return FALSE;

	BOOL bRemovePending=FALSE;

	if (m_bUndo)
		theUndoSlob.BeginUndo(IDS_UNDO_DELETE);
	
	if (m_pSelectBar->IsStaticItem(m_nSelectIndex))
	{
		m_pSelectBar->OnDeleteSelection();
	}
	else
	{
		CToolBarProxy* pBar = m_pSelectBar;
		int nIndex = m_nSelectIndex;
		UINT nIDWnd = (UINT) pBar->GetWindow()->GetDlgCtrlID();

		SetSelection(NULL, 0);

		if (pBar->GetCount() > 1 || nIDWnd < IDTB_CUSTOM_BASE)
		{
			UINT nCmdID=pBar->GetButtonCmdID(nIndex);

			pBar->DeleteButton(nIndex);
			pBar->OnDeleteSelection();
			pBar->RecalcLayout();

			// inform the parent dialog that a command has been removed
			if(	m_pDialog &&
				nCmdID>0)
			{
				m_pDialog->SendMessage(DSM_COMMANDREMOVED, nCmdID, 0);
			}
		}
		else if (m_pManager != NULL)
		{
			// if we're deleting the bar, lose the menu first
			if(	pBar &&		// should *never* be null, but safety first during the endgame
				CASBar::s_pDroppedBar==pBar->GetWindow())
			{
				CASBar::CancelMenu();
			}

			if(bDeferRemove)
			{
				bRemovePending=TRUE;
			}
			else
			{
				// Toolbars above the IDTB_CUSTOM_BASE range must belong to the shell.
				m_pManager->RemoveWnd(MAKEDOCKID(PACKAGE_SUSHI, nIDWnd));
			}
		}
	}

	if (m_bUndo)
		theUndoSlob.EndUndo();

	return bRemovePending;
}       

//      CToolCustomizer::DrawSelection
//              Draws the selection (fuzzy rect) on a given DC.

void CToolCustomizer::DrawSelection(CDC* pDC)
{
	CRect rect;
	
	m_pSelectBar->GetItemRect(m_nSelectIndex, rect);
	CBrush* pBrush = CDC::GetHalftoneBrush();
	if (pBrush != NULL)
		pBrush->UnrealizeObject();

	DrawTrackRect(pDC, &rect);
}

// A helper for DoButtonMove - creates a TRANSFER_TBBUTTON for a dragged command from the list
TRANSFER_TBBUTTON *CToolCustomizer::CreateCommandTransfer(int nCommand, CToolBarProxy *pBarDest )
{
	TRANSFER_TBBUTTON *pTrans=NULL;

	// copy from the all commands list
	CPackage *pPackage;

	// is there an available default appearance for this command?
	UINT nStyle=theApp.GetCommandBtnInfo(nCommand, &pPackage);
	BOOL bHasGlyph=theApp.HasCommandBitmap(nCommand);

	LPCTSTR pszMenuText;
	BOOL bFound=theCmdCache.GetCommandString(nCommand, STRING_MENUTEXT, &pszMenuText);
	BOOL bHasText=bFound && pszMenuText!=NULL && *pszMenuText!='\0';

	APPEARANCE apInitial=pBarDest->GetInitialAppearance(nStyle);
	if(	(apInitial==cAppearanceImageOnly && !bHasGlyph) ||
		(apInitial==cAppearanceTextOnly && !bHasText))
	{
		// user must choose a representation for the button
		CCustomButtonDlg *pCustom;

		CString buttonName;                     // name of the button such as DebugGo

		CAppToolGroups::GetAppToolGroups()->GetCommandName(nCommand,buttonName);
		CAppToolGroups::ReleaseAppToolGroups();

		pCustom=new CCustomButtonDlg(buttonName,pszMenuText, pBarDest->GetAppearance(), m_pManager, pBarDest->CanText(nStyle), pBarDest->CanGlyph(nStyle));
		if(pCustom->DoModal()==IDOK)
		{
			switch(pCustom->m_aAppearance)
			{
				default:
					ASSERT(FALSE);
					break;

				case cAppearanceImageOnly:
					// user chose one of the stock glyphs, so copy that
					ASSERT(pCustom->m_hbmCustomGlyph!=NULL);
					pTrans=new TRANSFER_TBBUTTON(nCommand, pCustom->m_hbmCustomGlyph, pBarDest->HasLargeButtons());
					break;

				case cAppearanceTextOnly: 
					// user entered a string label
					pTrans=new TRANSFER_TBBUTTON(nCommand, &(pCustom->m_buttonText), TRUE);
					break;

				case cAppearanceImageText:
					// user chose a stock glyph and a text label
					pTrans=new TRANSFER_TBBUTTON(nCommand, &(pCustom->m_buttonText), pCustom->m_hbmCustomGlyph, pBarDest->HasLargeButtons(), TRUE);
					break;
			}
		}
		else
		{
			// user cancelled dialog
			pTrans=NULL; 
		}
		delete pCustom;
	}
	else
	{
		// use default bitmap from package
		pTrans=new TRANSFER_TBBUTTON(nCommand);
	}

	return pTrans;
}

TRANSFER_TBBUTTON *CToolCustomizer::CreateSeparatorTransfer()
{
    return new TRANSFER_TBBUTTON(ID_SEPARATOR);
}

// determines, for a given point, what the drop button is
void CToolCustomizer::GetDropButton(CPoint pt, CToolBarProxy **ppBarDest, int *pnIndexDest, 
									CRect *pPrevious, ORIENTATION *porBar, UINT *pnHTSize)
{
	ASSERT(ppBarDest!=NULL);
	ASSERT(pnIndexDest!=NULL);
	ASSERT(pnHTSize!=NULL);
	ASSERT(porBar!=NULL);

	*ppBarDest=NULL; // no bar
	*pnIndexDest=-1; // dropped on dialog
	
	CRect rectTest;

	if (m_pDialog != NULL)
	{
		// check if it's inside the parent of this tab page, or below
		CWnd* pHit = CWnd::WindowFromPoint(pt);
		CWnd* pParent=m_pDialog->GetParent();
		if(	pParent==pHit ||
			pParent->IsChild(pHit))
		{
			return;
		}
	}

	POSITION pos=CBMenuPopup::s_listMenus.GetTailPosition();
	int nBarCount = m_aBars.GetSize();
	int iBar=0;
	BOOL bFound=FALSE;

	// Note: We must check menus before bars, because menus are parented to bars, so we need to
	// offer then the first chance to match.

	// loop over all bars and menus
	while(iBar<nBarCount || pos!=NULL)
	{
		// after menus, do bars
		if(pos!=NULL)
		{
			// loop menus
			CBMenuPopup *pMenu=CBMenuPopup::s_listMenus.GetPrev(pos);

			if(pMenu &&
				pMenu->GetSafeHwnd())
			{
				// only have a proxy when the menu is open
				*ppBarDest=pMenu->GetProxy();
			}
			else
			{
				*ppBarDest=NULL;
			}
		}
		else
		{
			// loop bars
			*ppBarDest = CToolBarProxy::GetBarProxy(m_aBars[iBar]);

			++iBar;
		}

		if(*ppBarDest==NULL)
		{
			continue;
		}

		// Two types of bar aren't checked: NODRAG bars (which should remain
		// immune at both ends), and bars which are uncreated (such as the phoney
		// one we create when dragging from the all commands list)
		if( (*ppBarDest)->GetWindow()->GetSafeHwnd()==NULL ||
			(*ppBarDest)->GetBarStyle() & CTBRS_NODRAG)
			continue;

		(*ppBarDest)->GetWindow()->GetWindowRect(rectTest);

		// First check if it's in the right boundaries
		if (rectTest.PtInRect(pt))
		{
			// Is this bar actually the owner of the point in question.
			CWnd* pWnd = CWnd::WindowFromPoint(pt);     
			while (pWnd != NULL && pWnd != (*ppBarDest)->GetWindow())
				pWnd = pWnd->GetParent();
				
			if (pWnd == (*ppBarDest)->GetWindow())
			{
				bFound=TRUE;
				break;
			}
		}
	}
	
	if (!bFound)
		*ppBarDest = NULL;

	if(*ppBarDest)
	{
		CPoint ptLocal=pt; // pt of button, adjusted to bar coordinates
		(*ppBarDest)->GetWindow()->ScreenToClient(&ptLocal);

		*pnHTSize=(*ppBarDest)->GetExpansionConstraint(CRect(ptLocal.x, ptLocal.y, ptLocal.x, ptLocal.y) ,porBar); // !!! maybe dicey
		
		*pnIndexDest = (*ppBarDest)->IndexFromPoint(ptLocal,pPrevious);
	}
	else
	{
		*pnHTSize=HTRIGHT;
		*pnIndexDest=0; // dropped in empty space
	}
}

//      CToolCustomizer::DoButtonMove
//              The guts of the tool customizer, handles the dragging of a button from
//              one location to another.

/* It is not valid for pBar to be NULL. But pBar may be a new-ed, uncreated 
CustomBar derivative, in which case, nIndex should be -1 and nCommand is the
id of the command being dragged from the all commands or all menus list. mtType
explains what kind of move is happening

This allows most of the code in this function to remain ignorant of whether there
is really a source toolbar or not. 

It's not valid to call any window-related function on a new-ed, uncreated bar. But we can
call pBar->NewBar(...), which is the reason we need the object.
*/
BOOL CToolCustomizer::DoButtonMove(CPoint pt, CToolBarProxy* pBar, int nIndex, BOOL *pbDeletePending, MOVETYPE mtType, int nCommand)
{
	CRect rectTest;
	BOOL bCopy;
	BOOL bLastButton=FALSE;

	// TRUE if the bar was created by the move, and so should be deleted if the move is cancelled
	BOOL bSynthesizedBar=FALSE;

	ASSERT(pBar!=NULL);
	if(pbDeletePending)
	{
		*pbDeletePending=FALSE;
	}
	
	if (mtType==cBar)
	{
		// if there's a source which isn't already the selection, make it the selection
		if(pBar != m_pSelectBar || nIndex != m_nSelectIndex)
		{
			SetSelection(pBar, nIndex);
		}
		
		// don't allow dragging from toolbars that don't want to.
		if(pBar->GetBarStyle() & CTBRS_NODRAG)
		{
			return FALSE;
		}
	}
	else
	{
		// when there's no original source for the button, clear the current selection,
		// to avoid confusion about what is being dragged
		SetSelection(NULL,0);
	}

	CToolBarProxy* pBarDest;
	int nIndexDest;
	int nCmdIDSrc=0;				// the command being dragged if it is a menu, or 0 if not.
	int nCmd=0;						// the command being dragged
	UINT nIDBar=0;					// window id of the source bar. Need to get in advance, in case the
									// bar goes away while dragging

	// determine command being dragged
	if(mtType==cBar)
	{
		ASSERT(pBar);
		nCmd=pBar->GetButtonCmdID(nIndex);
		nIDBar= (UINT) pBar->GetWindow()->GetDlgCtrlID();
	}
	else
	{	
		nCmd=nCommand;
	}

	if(nCmd!=0)
	{
		CTE *pCTE=theCmdCache.GetCommandEntry(nCmd);

		if(pCTE &&
			pCTE->flags & CT_MENU)
		{
			// only use source command id for menus.
			nCmdIDSrc=nCmd;
		}
	}
	
	ORIENTATION orBar;
	UINT nHTSize;
	CPoint ptInitial=pt;
	if (!TrackButtonMove(pt, bCopy, &pBarDest, &nIndexDest, &orBar, &nHTSize, nCmdIDSrc))
		return FALSE;

	// must be copy when nIndex is -1 (can't perform a move when
	// there's no source to perform a 'delete' on).
	ASSERT(nIndex!=-1 || bCopy);
	
	if (pBarDest == NULL)
	{
		if (!bCopy)
		{
			BOOL bDeletePending=DeleteSelection(TRUE);
			if(pbDeletePending)
			{
				*pbDeletePending=bDeletePending;
			}
			return TRUE;
		}
		else if (nIndexDest==0 &&       // drop was in empty space
				m_pManager != NULL) // and there's a dock manager present
		{
			// In V4,x. we passed in the command id of the new button here, and then returned.
			// This was ok, because there couldn't be any kind of custom button options. Now that
			// there can, we must treat a drag to an empty space the same way as any other.
			// martynl 08Apr96
			if ((pBarDest = pBar->NewBar((UINT)-1, pt)) == NULL)
			{
				return FALSE;
			}
			bSynthesizedBar=TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	if(!bCopy && pBar)
	{
		if(pBar->GetCount()==1)
		{
			bLastButton=TRUE;
		}
	}
	
	GAP gap = gapNil;
	BOOL bReplace = FALSE;
	
	if (nIndexDest == -1)
	{
		nIndexDest = 0;
	}
	else if (pBarDest->IsGapItem(nIndexDest))
	{
		bReplace = TRUE;
	}

	if (bCopy)
	{
		TRANSFER_TBBUTTON *pTrans=NULL;
		switch(mtType)
		{
			case cBar:
				pTrans=new TRANSFER_TBBUTTON(pBar, nIndex, FALSE);
				break;

			case cCommand:
				pTrans=CreateCommandTransfer(nCommand, pBarDest);
				break;

			default:
				ASSERT(FALSE);
				break;
		}

		if (pTrans==NULL)
		{
			if(bSynthesizedBar && pBarDest && pBarDest->GetWindow())
			{
				UINT nIDWnd = (UINT) pBarDest->GetWindow()->GetDlgCtrlID();

				if(m_pManager)
				{
					m_pManager->RemoveWnd(MAKEDOCKID(PACKAGE_SUSHI, nIDWnd));
				}
				pBarDest=NULL;
			}
			return FALSE;
		}
		
		SetSelection(NULL, 0);
		if (pBar->GetRealObject() != pBarDest->GetRealObject())
		{
			m_nSelectIndex = -1;    // Lock the selection.
			pBarDest->OnBarActivate();
			m_nSelectIndex = 0;
		}

		// Start recording after activation to assure correct window.
		if (m_bUndo)
			theUndoSlob.BeginUndo(IDS_UNDO_COPY);

		if(nIndex!=-1 && !bLastButton)
		{
			ASSERT_VALID(pBar->GetWindow());
		}

		pBarDest->InsertButton(nIndexDest, *pTrans, bReplace, gap);

		pBarDest->RecalcLayout(nHTSize);
		ASSERT_VALID(pBarDest->GetWindow());

		if(pTrans)
		{
			delete pTrans;
		}
	}
	else
	{
		ASSERT(mtType==cBar);

		if (pBarDest->GetRealObject() != pBar->GetRealObject())
		{
			if (pBar->IsStaticItem(nIndex))
				return FALSE; // Cannot move a static item to another toolbar.
		}
		else
		{
			if (bReplace)
			{
				// May want to preserve some gaps.
				if ((nIndexDest == nIndex + 1) &&
					pBar->IsGapItem(nIndex - 1))
				{
					gap = gapBefore;
				}
				else if ((nIndexDest == nIndex - 1) &&
					pBar->IsGapItem(nIndex + 1))
				{
					gap = gapAfter;
				}
			}
			else if (nIndexDest == nIndex ||
				nIndexDest == nIndex + 1)
			{
				// nudge time.
				nIndexDest = nIndex;
				CRect rectButton;
				pBar->GetItemRect(nIndexDest, rectButton);
				int nDiffRect = (orBar == orVert) ?
					pt.y - ptInitial.y : pt.x - ptInitial.x;

				if (nDiffRect > 0)
				{
					if (pBar->IsGapItem(nIndexDest - 1) || nIndex == 0)
						return FALSE;
					else
						gap = gapBefore;
				}
				else
				{
					if (pBar->IsGapItem(nIndexDest + 1) ||
						nIndex == pBarDest->GetCount() - 1)
						return FALSE;
					else
						gap = gapAfter;
				}
			}
		}
	    
	    // Here we actually do the button move by deleting the button from
	    // its old location, and inserting it in its new location.  Not always
	    // optimal, but seems fast enough.
	    //
		TRANSFER_TBBUTTON trans(pBar, nIndex, TRUE);

		SetSelection(NULL, 0);
		int nCountOld = pBar->GetCount();
		HWND hwnd = NULL;
		BOOL bRemoveBar = FALSE;

		if (m_bUndo)
			theUndoSlob.BeginUndo(IDS_UNDO_MOVE);
		
		pBar->DeleteButton(nIndex, TRUE);

		if(pBarDest==NULL)
		{
			UINT nCmdID=trans.nID;

			// inform the parent dialog that a command has been removed
			if(	m_pDialog &&
				nCmdID>0)
			{
				m_pDialog->SendMessage(DSM_COMMANDREMOVED, nCmdID, 0);
			}
		}
		
		if (pBar != pBarDest)
		{
			if (!(pBar->GetCount() > 0 || nIDBar < IDTB_CUSTOM_BASE))
				bRemoveBar = TRUE;
			else
				pBar->RecalcLayout();

			// Restart undo to assure correct window.
			if (m_bUndo)
			{
				theUndoSlob.EndUndo();
				theUndoSlob.Link();
			}

			m_nSelectIndex = -1;    // Lock the selection.
			pBarDest->OnBarActivate();
			m_nSelectIndex = 0;

			if (m_bUndo)
				theUndoSlob.BeginUndo(IDS_UNDO_MOVE);
		}
		else if (nIndex < nIndexDest)
		{
			nIndexDest -= nCountOld - pBar->GetCount();
		}

		if (nIndexDest < 0)
		{
			nIndexDest = 0;
			bReplace = FALSE;
		}
		
		// No gaps, if this is the first button.
		if (pBarDest->GetCount() == 0)
			gap = gapNil;
		
		// we must ensure pBar is valid before we insert button, because it might disappear as a result of the insert
		if(nIndex!=-1 && !bLastButton)
		{
			ASSERT_VALID(pBar->GetWindow());
		}

		pBarDest->InsertButton(nIndexDest, trans, bReplace, gap);
		pBarDest->RecalcLayout(nHTSize);

		if (bRemoveBar && m_pManager != NULL)
			m_pManager->RemoveWnd(MAKEDOCKID(PACKAGE_SUSHI, nIDBar));
	}
		
	if (!m_bTemp &&
		pBarDest->GetWindow()->GetSafeHwnd()!=NULL)
	{
		if (pBarDest->IsGapItem(nIndexDest))
			nIndexDest++;   // Make sure we don't select a separator.
	
		pBarDest->GetWindow()->UpdateWindow();
		int nNewSelection=nIndexDest;

		// could be past the end if we just deleted; move back if it's past the end, and we're not at 0
		if(nNewSelection>pBarDest->GetCount() - 1 &&
			pBarDest->GetCount()>0)
		{
			nNewSelection=pBarDest->GetCount() - 1;
		}
		ASSERT(nNewSelection>=0);
		ASSERT(nNewSelection<pBarDest->GetCount());

		SetSelection(pBarDest, nNewSelection);
	}

	if(pBarDest && pBarDest != pBar)
	{
		ASSERT_VALID(pBarDest->GetWindow());
	}

	if (theUndoSlob.IsRecording())
		theUndoSlob.EndUndo();

	return TRUE;
}

static inline int Brightness(COLORREF cr)
{
	return (3*GetRValue(cr) + 6*GetGValue(cr) + GetBValue(cr));
}

// This code, which is similar to office's GetRopToDrawFeedback (!) draws or undraws 
// the insertion point, given the rectangle of the preceding button, the orientation of the bar
// and an indication of whether it goes before or after
void CToolCustomizer::DrawDragFeedback(CDC *pDC, const CRect &rect, ORIENTATION or, BOOL bBefore)
{
	ASSERT(pDC != NULL);
	int ROP2;
	int ROP2Sav = pDC->GetROP2();
	CBrush brushBtn;
	brushBtn.CreateSolidBrush(globalData.clrBtnFace);
	const int cPoint = 8;
	POINT rgPoint[cPoint];
	POINT *ppt = rgPoint;

	CRect rectDest=rect;
	// make a 6 pixel width/height rectangle to draw into.
	if(or!=orVert)
	{
		// horizontal, so collapse width
		if(bBefore)
		{
			rectDest.right=rectDest.left;
		}
		else
		{
			rectDest.left=rectDest.right;
		}
		rectDest.left-=3;
		rectDest.right+=3;
	}
	else
	{
		if(bBefore)
		{
			rectDest.bottom=rectDest.top;
		}
		else
		{
			rectDest.top=rectDest.bottom;
		}
		rectDest.top-=3;
		rectDest.bottom+=3;
	}

	ROP2 = (Brightness(globalData.clrBtnFace) > 1100) ? R2_XORPEN : R2_NOTXORPEN;
	VERIFY(ROP2Sav == pDC->SetROP2(ROP2));
	int iPolyFillModeSav=pDC->GetPolyFillMode();
	VERIFY(iPolyFillModeSav == pDC->SetPolyFillMode(ALTERNATE));
	ASSERT((rectDest.Height() == 6) || (rectDest.Width()== 6));

#define SetAPoint(ppt, xx, yy) (ppt->x = (xx), ppt->y = (yy), ppt++)

	if (or==orVert)
		{
		SetAPoint(ppt, rectDest.left, rectDest.top-1);
		SetAPoint(ppt, rectDest.left+3, rectDest.top+2);
		SetAPoint(ppt, rectDest.right-3, rectDest.top+2);
		SetAPoint(ppt, rectDest.right, rectDest.top-1);
		SetAPoint(ppt, rectDest.right, rectDest.bottom);
		SetAPoint(ppt, rectDest.right-2, rectDest.bottom-2);
		SetAPoint(ppt, rectDest.left+2, rectDest.bottom-2);
		SetAPoint(ppt, rectDest.left, rectDest.bottom);
		}
	else
		{
		SetAPoint(ppt, rectDest.left, rectDest.top);
		SetAPoint(ppt, rectDest.right, rectDest.top);
		SetAPoint(ppt, rectDest.right-2, rectDest.top+2);
		SetAPoint(ppt, rectDest.right-2, rectDest.bottom-3);
		SetAPoint(ppt, rectDest.right+1, rectDest.bottom);
		SetAPoint(ppt, rectDest.left-1, rectDest.bottom);
		SetAPoint(ppt, rectDest.left+2, rectDest.bottom-3);
		SetAPoint(ppt, rectDest.left+2, rectDest.top+2);
		}

#undef SetAPoint

	CBrush *pbrSav = pDC->SelectObject(&brushBtn);
	CPen *pPenSav = pDC->SelectObject(CPen::FromHandle((HPEN)GetStockObject(NULL_PEN)));
	pDC->Polygon(rgPoint, cPoint);
	pDC->SelectObject(pbrSav);
	VERIFY(ROP2 == pDC->SetROP2(ROP2Sav));
	VERIFY(ALTERNATE == pDC->SetPolyFillMode(iPolyFillModeSav));
	pDC->SelectObject(pPenSav);
}


//      CToolCustomizer::TrackButtonMove
//              Tracks the button rectangle as the user drags it around the screen,
//              and also whether or not the operation is a copy or move.

//              This routine must now allow for the situation where there is no selected
//				button when it is called
BOOL CToolCustomizer::TrackButtonMove(CPoint &pt, BOOL& bCopy, CToolBarProxy **ppBarDest, int *pnIndexDest, ORIENTATION *porBar, UINT *pnHTSize, int nCmdIDSrc)
{
    BOOL bCancel = FALSE;
    BOOL bMoved = FALSE;
	BOOL bDestChanged=TRUE;
    
    BOOL bControl, bControlOld;
    CPoint ptOld;

	// We use a pointer to a DC so we can change DC as we change bar, because we only want to draw our
	// feedback inside the visible region of the destination bar.
	CLockDC *pDC=NULL;
    
	CWnd* pWndTrack=NULL;

	if(m_pDialog==NULL)
	{
		// Loop while the mouse is down.
		pWndTrack = CWnd::GetFocus(); // So we stay the active window.
		if (pWndTrack == NULL && m_pManager != NULL)
		{
			// Try to get a valid focused window.
			m_pManager->m_pFrame->SetFocus();
			if ((pWndTrack = CWnd::GetFocus()) == NULL)
				return FALSE;
		}
	}
	else
	{
		pWndTrack=m_pDialog;
	}

	pWndTrack->SetCapture();
	
    ptOld = pt;
	CPoint ptInitial=pt;
    bControlOld = -1; // neither false nor true

	CToolBarProxy *pBarDest=NULL;           // current destination
	CToolBarProxy *pBarDestOld=NULL;        // previous destination
	int nIndexDest=0;
	int nIndexDestOld=0;
	int nIndexExpand=-1;
	int nIndexExpandOld=-1;
	CToolBarProxy  *pBarExpandOld=NULL;
	CRect rectPreviousButton(0,0,0,0); // rectangle of the button before the insert position
	CRect rectPreviousButtonOld; // rectangle of the button before the insert position
	BOOL bUndraw=FALSE;
	BOOL bCanDrop=FALSE;		// TRUE if the current location would support a drop of the current item

	// We need to work out m_pSelectBar related stuff before we do the drag, because the drag could cause the selection to be lost
	// when a menu is auto-closed
	BOOL bNoSource=(m_pSelectBar==NULL);				// TRUE if the source bar was null to start with
	BOOL bSourceInDialog=(!bNoSource && m_pSelectBar->IsInDialog());	// TRUE if the source bar is inside a dialog
	BOOL bSourceStatic=(!bNoSource && !m_pSelectBar->IsStaticItem(m_nSelectIndex));
    
    while (!PollForMouseChange(pt, FALSE, bCancel, TRUE))
    {                 
		bControl = ((GetAsyncKeyState(VK_CONTROL) & ~1)  &&
					bSourceStatic) ||							// Cannot copy static item.
					bNoSource ||                                // Always copy from no source.
					bSourceInDialog;                            // Always copy from customize dialog.

		if (pt == ptOld &&
			bControl==bControlOld)
			continue;
		
		pBarDestOld=pBarDest;
		nIndexDestOld=nIndexDest;
		rectPreviousButtonOld=rectPreviousButton;

		GetDropButton(pt,&pBarDest, &nIndexDest, &rectPreviousButton, porBar, pnHTSize);

#if _DEBUG
		// These assertions can be pretty hard to be debug, but they are important. They indicate when we would have
		// done a bad paint, because we expect the control id and the rectangle to always change in sync.

		// To debug the problem, if it's in a toolbar, you can use the IFP_TRACE options I've added over there. They make it
		// much easier to see why a failure has occurred, since one normally needs to know how both sets of rectangles
		// were calculated.

		if(	pBarDest==pBarDestOld && pBarDest!=NULL)
		{
			// if the rect changed, the index must too
			if(nIndexDestOld==nIndexDest)
			{
				// same index, same rect
				ASSERT(rectPreviousButtonOld==rectPreviousButton);
			}
			else
			{
				int nFirstVisible=pBarDest->FindNextVisibleItem(-1, TRUE);
				if(nFirstVisible!=-1)
				{
					if( (nIndexDest==nFirstVisible && nIndexDestOld==-1) ||
						(nIndexDest==-1 && nIndexDestOld==nFirstVisible))
					{
						// index -1 and 0 must have same rect
						ASSERT(rectPreviousButtonOld==rectPreviousButton);
					}
					else
					{
						// different index, different rect
						ASSERT(rectPreviousButtonOld!=rectPreviousButton);
					}
				}
			}
		}
#endif

		// draw the insertion point
		if( pBarDest!=pBarDestOld ||
			nIndexDest!=nIndexDestOld)
		{
			if(pBarDestOld!=NULL)
			{
				// shouldn't get here unless we're undrawing, in which case DC should be set up already
				ASSERT(pDC!=NULL);

				ORIENTATION or=pBarDestOld->GetOrient();
				DrawDragFeedback(pDC, &rectPreviousButtonOld, or, nIndexDestOld==-1);
				bUndraw=FALSE;
			}

			// since we've just moved out of the old bar, lose it's DC
			if(pBarDestOld!=pBarDest)
			{
				if(pDC!=NULL)
				{
					delete pDC;
					pDC=NULL;
				}
			}

			if(pBarDest!=NULL)
			{
				if(pDC==NULL)
				{
					pDC=new CLockDC(pBarDest->GetWindow());
					ASSERT(pDC);
				}

				ORIENTATION or=pBarDest->GetOrient();
				DrawDragFeedback(pDC, &rectPreviousButton, or, nIndexDest==-1);
				bUndraw=TRUE;
			}
		}

		// determine what we should expand
		if(pBarDest!=NULL)
		{
			CPoint ptLocal=pt;
			pBarDest->GetWindow()->ScreenToClient(&ptLocal);
			nIndexExpand=pBarDest->HitTest(ptLocal);
		}
		else
		{
			nIndexExpand=-1;
		}

		// if there is a change in expansion, reflect it.
		if(nIndexExpand!=nIndexExpandOld ||
			pBarDest!=pBarExpandOld)
		{
			// expand the new
			if(	pBarDest!=NULL &&
				nIndexExpand!=-1)
			{
				if(pBarDest->ExpandDestination(nCmdIDSrc, nIndexExpand))
				{
					nIndexExpandOld=nIndexExpand;
					pBarExpandOld=pBarDest;
				}
			}
		}

		if (pBarDest!=pBarDestOld ||
			nIndexDest!=nIndexDestOld ||
			bControl != bControlOld)
		{
			int nIdCursor=IDCUR_BUTTON_COPY;
			if(pBarDest==NULL)
			{
				if(bControl)
				{
					nIdCursor=IDCUR_BUTTON_COPY;
				}
				else
				{
					nIdCursor=IDCUR_BUTTON_DELETE;
				}
			}
			else
			{
				if(bControl)
				{
					nIdCursor=IDCUR_BUTTON_COPY;
				}
				else
				{
					nIdCursor=IDCUR_BUTTON_MOVE;
				}
			}
			
			::SetCursor(theApp.LoadCursor(nIdCursor));
			bControlOld = bControl;
		}
	
		ptOld = pt;
    } // while
    
	// undraw insertion point
	if(bUndraw && pBarDest!=NULL)
	{
		ASSERT(pDC!=NULL);

		ORIENTATION or=pBarDest->GetOrient();
		DrawDragFeedback(pDC, &rectPreviousButton, or, nIndexDest==-1);

		if(pDC)
		{
			delete pDC;
			pDC=NULL;
		}
	}

	ASSERT(pDC==NULL);

    ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	if(pWndTrack->GetCapture()==pWndTrack)
	{
		ReleaseCapture();
	}

    if (bCancel || (abs(pt.x - ptInitial.x) < 2 &&
		abs(pt.y - ptInitial.y) < 2))     // avoid slip
	{
		*ppBarDest=NULL;
		*pnIndexDest=-1;
		return FALSE;
	}
    else
    {
		bCopy = bControl;
		*ppBarDest=pBarDest;
		if(pBarDest)
		{
			// reason for the +1, indexfrompoint returns the index of the button
			// after which we are pointing at, while the caller wants to know about 'before'.
			nIndexDest+=1;
		}
		*pnIndexDest=nIndexDest;	
		return TRUE;
    }
}

//      CToolCustomizer::DoButtonSize
//              Manages the sizing of a control in a toolbar.

void CToolCustomizer::DoButtonSize(CPoint pt, CToolBarProxy* pBar, int nIndex)
{
	CRect rect;
	
	if (pBar->GetRealObject() != m_pSelectBar->GetRealObject() || nIndex != m_nSelectIndex)
		SetSelection(pBar, nIndex);
			
	if (!TrackButtonSize(pt, rect))
		return;
				
	HWND hwnd = pBar->GetButtonControl(nIndex);

	CRect rectDrop = rect;
	::GetDroppedRect(hwnd, rectDrop);
	::SetWindowPos(hwnd, NULL, 0, 0,
		rect.Width(), rectDrop.Height(),
		SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	pBar->OnButtonSize();
	pBar->RecalcLayout(pBar->GetExpansionConstraint(rect));
}

//      CToolCustomizer::TrackButtonSize
//              Tracks the control's size rectangle as the user moves the mouse.
//              Assumes that sizing is only allowed on the right edge of the control.

BOOL CToolCustomizer::TrackButtonSize(CPoint ptStart, CRect& rectFinal)
{
    BOOL bCancel = FALSE;
    
    CRect rect, rectOld;
    CPoint pt, ptOld;

    m_pSelectBar->GetItemRect(m_nSelectIndex, rect);
    m_pSelectBar->GetWindow()->ClientToScreen(rect);
    rectFinal = rect;
    
	ASSERT(m_pSelectBar->GetButtonStyle(m_nSelectIndex) & TBBS_SIZABLE);
	ASSERT(m_pSelectBar->GetButtonStyle(m_nSelectIndex) & TBBS_HWND);
	
	HWND hwnd = m_pSelectBar->GetButtonControl(m_nSelectIndex);

	// Get min-max info.  We provide defaults, in case window doesn't handle.
    MINMAXINFO mmi;
	mmi.ptMaxTrackSize = CPoint(::GetSystemMetrics(SM_CXSCREEN), rect.Height());
    mmi.ptMinTrackSize = CPoint(m_pSelectBar->GetButtonSize().cx, rect.Height());
    
    ::SendMessage(hwnd, WM_GETMINMAXINFO, 0, (LPARAM) (LPVOID) &mmi);

	// Do not allow windows smaller than button width.
	mmi.ptMinTrackSize.x = max(mmi.ptMinTrackSize.x,
		m_pSelectBar->GetButtonSize().cx);
    
    // Need a client DC for the first erase, and final draw to avoid trash
    // on the screen DC.

    CDC dcClient;

    if (!dcClient.Attach(::GetDC(m_pSelectBar->GetWindow()->GetSafeHwnd())))
		return FALSE;
	
	rectOld = rect;
	m_pSelectBar->GetWindow()->ScreenToClient(rectOld);
	DrawTrackRect(&dcClient, &rectOld);

#if 1
	CLockDC dc(NULL);
#else
	CClientDC dc(NULL);
#endif

	DrawTrackRect(&dc, &rect);

	// Loop while the mouse is down.
    CWnd* pWndTrack = CWnd::GetFocus(); // So we stay the active window.
    if (pWndTrack == NULL && m_pManager != NULL)
	{
		// Try to get a valid focused window.
		m_pManager->m_pFrame->SetFocus();
		if ((pWndTrack = CWnd::GetFocus()) == NULL)
			return FALSE;
	}
	
    pWndTrack->SetCapture();
    ptOld = ptStart; //[der] Was pt, but pt is not initialized.
    
    while (!PollForMouseChange(pt, FALSE, bCancel))
    {                 
	if (pt.x == ptOld.x)
		continue;
	
	rectOld = rect;

	rect.right = rectFinal.right + pt.x - ptStart.x;
		rect.right = rect.left +
			min(max(rect.Width(), mmi.ptMinTrackSize.x),
				mmi.ptMaxTrackSize.x);
	
	// Move the drag rect on screen.
		DrawTrackRect(&dc, &rect, &rectOld);
	
	ptOld = pt;
    } // while
    
    // Turn off drag rectangle.
	DrawTrackRect(&dc, &rect);
	
    ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	if(pWndTrack->GetCapture()==pWndTrack)
	{
		ReleaseCapture();
	}

	BOOL bReturn = FALSE;
    if (!(bCancel || abs(rect.right - rectFinal.right) < 2))
	{
	    rectFinal = rect;
		bReturn = TRUE;
	}

	m_pSelectBar->GetWindow()->ScreenToClient(rectFinal);

	if (!bReturn)
		DrawTrackRect(&dcClient, &rectFinal);   //Redraw the selection.

	::ReleaseDC(m_pSelectBar->GetWindow()->GetSafeHwnd(), dcClient.Detach());
    return bReturn;
}

//      CToolCustomizer::DrawTrackRect
//              Handles drawing of track rects, which include the selection rect.
//              BrushOrg is set so that the selection rect will always paint the same.

void CToolCustomizer::DrawTrackRect(CDC* pdc, const CRect* prect,
	const CRect* prectLast /*=NULL*/)
{
	CSize size(g_mt.cxFrame - g_mt.cxBorder, g_mt.cyFrame - g_mt.cyBorder);

	COLORREF crBk = pdc->SetBkColor(RGB(0xFF, 0xFF, 0xFF));
	COLORREF crFG = pdc->SetTextColor(RGB(0, 0, 0));

	pdc->DrawDragRect(prect, size, prectLast, size);

	pdc->SetBkColor(crBk);
	pdc->SetTextColor(crFG);
}

BEGIN_POPUP_MENU(ButtonCustomize)
	MENU_ITEM(IDM_BUTTON_RESET)
	MENU_ITEM(IDM_BUTTON_DELETE)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_BUTTON_COPYIMAGE)
	MENU_ITEM(IDM_BUTTON_PASTEIMAGE)
	MENU_ITEM(IDM_BUTTON_RESETIMAGE)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_BUTTON_CHOOSEAPPEARANCE)
	MENU_ITEM(IDM_BUTTON_IMAGEONLY)
	MENU_ITEM(IDM_BUTTON_TEXTONLY)
	MENU_ITEM(IDM_BUTTON_IMAGETEXT)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_BUTTON_GROUPSTART)
END_POPUP_MENU()

POPDESC *CToolCustomizer::GetButtonMenu(void)
{
	return MENU_CONTEXT_POPUP(ButtonCustomize);
}

BOOL CToolCustomizer::DisplayButtonMenu(UINT nFlags, CPoint point)
{
	if (m_nSelectIndex != -1 &&
		m_pSelectBar!=NULL)
	{
		// find screen position
		::CBShowContextPopupMenu(MENU_CONTEXT_POPUP(ButtonCustomize), point, theApp.m_pMainWnd);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BEGIN_MESSAGE_MAP(CToolCustomizer, CCmdTarget)
	//{{AFX_MSG_MAP(CToolCustomizer)
	ON_COMMAND(IDM_BUTTON_COPYIMAGE, OnButtonCopyImage)
	ON_COMMAND(IDM_BUTTON_PASTEIMAGE, OnButtonPasteImage)
	ON_COMMAND(IDM_BUTTON_RESETIMAGE, OnButtonResetImage)
	ON_COMMAND(IDM_BUTTON_CHOOSEAPPEARANCE, OnButtonChooseAppearance)
	ON_COMMAND(IDM_BUTTON_RESET, OnButtonReset)
	ON_COMMAND(IDM_BUTTON_DELETE, OnButtonDelete)
	ON_COMMAND(IDM_BUTTON_IMAGEONLY, OnButtonImageOnly)
	ON_COMMAND(IDM_BUTTON_TEXTONLY, OnButtonTextOnly)
	ON_COMMAND(IDM_BUTTON_IMAGETEXT, OnButtonImageText)
	ON_COMMAND(IDM_BUTTON_GROUPSTART, OnButtonGroupStart)
	ON_UPDATE_COMMAND_UI(IDM_BUTTON_COPYIMAGE, OnUpdateButtonCopyImage)
	ON_UPDATE_COMMAND_UI(IDM_BUTTON_PASTEIMAGE, OnUpdateButtonPasteImage)
	ON_UPDATE_COMMAND_UI(IDM_BUTTON_RESETIMAGE, OnUpdateButtonResetImage)
	ON_UPDATE_COMMAND_UI(IDM_BUTTON_CHOOSEAPPEARANCE, OnUpdateButtonChooseAppearance)
	ON_UPDATE_COMMAND_UI(IDM_BUTTON_RESET, OnUpdateButtonReset)
	ON_UPDATE_COMMAND_UI(IDM_BUTTON_DELETE, OnUpdateButtonDelete)
	ON_UPDATE_COMMAND_UI(IDM_BUTTON_IMAGEONLY, OnUpdateButtonImageOnly)
	ON_UPDATE_COMMAND_UI(IDM_BUTTON_TEXTONLY, OnUpdateButtonTextOnly)
	ON_UPDATE_COMMAND_UI(IDM_BUTTON_IMAGETEXT, OnUpdateButtonImageText)
	ON_UPDATE_COMMAND_UI(IDM_BUTTON_GROUPSTART, OnUpdateButtonGroupStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

afx_msg void CToolCustomizer::OnButtonCopyImage()
{
	if (!m_pSelectBar->GetWindow()->OpenClipboard())
		return;
	
	EmptyClipboard();
	
	// copy the button
	HBITMAP hbmWell=m_pSelectBar->GetImageWell(m_nSelectIndex);
	int iImage=m_pSelectBar->GetImageIndex(m_nSelectIndex);
	CSize sizeImage=m_pSelectBar->GetImageSize();
	if(hbmWell!=NULL)
	{
		HBITMAP hbmClip=GLOBAL_DATA::DuplicateBitmap(m_pSelectBar->GetWindow(), // in this context
													hbmWell,                    // duplicate a portion of this well
													sizeImage, // size of portion
													CPoint(iImage*sizeImage.cx, 0)); // offset of portion
		
		// set the standard format (bitmap) data
		SetClipboardData(CF_BITMAP, hbmClip);
	}

	CloseClipboard();
}

afx_msg void CToolCustomizer::OnButtonPasteImage()
{
	m_pSelectBar->PasteButtonImage();
}

afx_msg void CToolCustomizer::OnButtonResetImage()
{
	m_pSelectBar->ResetButtonImage();
}

afx_msg void CToolCustomizer::OnButtonChooseAppearance()
{
	CCustomButtonDlg *pCustom;
	CString buttonName;                     // name of the button such as DebugGo
	CString buttonText;                     // label on toolbar, such as Start Program

	int nID=m_pSelectBar->GetButtonCmdID(m_nSelectIndex);
	CAppToolGroups::GetAppToolGroups()->GetCommandName(nID,buttonName);
	CAppToolGroups::ReleaseAppToolGroups();

	APPEARANCE aAppearance=m_pSelectBar->GetAppearance(m_nSelectIndex);

	if(aAppearance==cAppearanceImageOnly)
	{
		LPCSTR pText;
		theCmdCache.GetCommandString(nID, STRING_MENUTEXT, &pText);
		if(pText || *pText=='\0')
		{
			theCmdCache.GetCommandString(nID, STRING_COMMAND, &pText);
		}
		buttonText=pText;
	}
	else
	{
		buttonText = m_pSelectBar->GetButtonText(m_nSelectIndex);
	}

	// Get current glyph details
	HBITMAP hbmCurrent=NULL;
	int nIndexCurrent=m_pSelectBar->GetImageIndex(m_nSelectIndex);
	if(nIndexCurrent!=-1)
	{
		hbmCurrent=m_pSelectBar->GetImageWell(m_nSelectIndex);
	}

	pCustom=new CCustomButtonDlg(buttonName,buttonText, aAppearance, m_pManager, m_pSelectBar->CanText(m_nSelectIndex), m_pSelectBar->CanGlyph(m_nSelectIndex), hbmCurrent, nIndexCurrent);
	if(pCustom->DoModal()==IDOK)
	{
		ASSERT(m_pSelectBar);
		switch(pCustom->m_aAppearance)
		{
			default:
				ASSERT(FALSE);
				break;

			case cAppearanceTextOnly:
				m_pSelectBar->AddText(m_nSelectIndex, pCustom->m_buttonText, TRUE);
				if(aAppearance!=cAppearanceTextOnly)
				{
					m_pSelectBar->RemoveBitmap(m_nSelectIndex);
				}
				m_pSelectBar->ButtonModified(m_nSelectIndex, TRUE);
				break;

			case cAppearanceImageOnly:
				m_pSelectBar->AddBitmap(m_nSelectIndex, cBitmapSourceCustom, pCustom->m_hbmCustomGlyph);
				if(aAppearance!=cAppearanceImageOnly)
				{
					m_pSelectBar->RemoveText(m_nSelectIndex);
					m_pSelectBar->ButtonModified(m_nSelectIndex, TRUE);
				}
				break;

			case cAppearanceImageText:
				m_pSelectBar->AddText(m_nSelectIndex, pCustom->m_buttonText, TRUE);
				m_pSelectBar->AddBitmap(m_nSelectIndex, cBitmapSourceCustom, pCustom->m_hbmCustomGlyph);
				m_pSelectBar->ButtonModified(m_nSelectIndex, TRUE);
				break;
		}
	}
	delete pCustom;
}

afx_msg void CToolCustomizer::OnButtonReset()
{
	m_pSelectBar->ResetButtonImage();
	m_pSelectBar->ButtonModified(m_nSelectIndex, TRUE);

	// reset the attached menu, if appropriate
	int iImage;
	UINT nStyle;
	UINT nID;
	m_pSelectBar->GetButtonInfo(m_nSelectIndex, nID, nStyle, iImage);

	if(nStyle & TBBS_MENU)
	{
		CMainFrame *pFrame=(CMainFrame *)AfxGetMainWnd();

		pFrame->ResetMenu(nID);
	}
}

afx_msg void CToolCustomizer::OnButtonDelete()
{ 
	DeleteSelection();
}

afx_msg void CToolCustomizer::OnButtonImageOnly()
{
	int iImage;
	UINT nStyle;
	UINT nID;
	m_pSelectBar->GetButtonInfo(m_nSelectIndex, nID, nStyle, iImage);

	if((nStyle & (TBBS_TEXTUAL | TBBS_GLYPH))==TBBS_GLYPH)
	{
		return;
	}

	if((nStyle & TBBS_GLYPH)==0)
	{
		m_pSelectBar->AddBitmap(m_nSelectIndex);        
	}

	if(nStyle & TBBS_TEXTUAL)
	{
		m_pSelectBar->RemoveText(m_nSelectIndex);
	}
	m_pSelectBar->ButtonModified(m_nSelectIndex,TRUE);
}

afx_msg void CToolCustomizer::OnButtonTextOnly()
{
	int iImage;
	UINT nStyle;
	UINT nID;
	m_pSelectBar->GetButtonInfo(m_nSelectIndex, nID, nStyle, iImage);

	if((nStyle & (TBBS_TEXTUAL | TBBS_GLYPH))==TBBS_TEXTUAL)
	{
		return;
	}

	if((nStyle & TBBS_TEXTUAL)==0)
	{
		m_pSelectBar->AddText(m_nSelectIndex);
	}

	if(nStyle & TBBS_GLYPH)
	{
		m_pSelectBar->RemoveBitmap(m_nSelectIndex);
	}
	m_pSelectBar->ButtonModified(m_nSelectIndex,TRUE);
}

afx_msg void CToolCustomizer::OnButtonImageText()
{
	int iImage;
	UINT nStyle;
	UINT nID;
	m_pSelectBar->GetButtonInfo(m_nSelectIndex, nID, nStyle, iImage);

	if((nStyle & (TBBS_TEXTUAL | TBBS_GLYPH))==(TBBS_TEXTUAL | TBBS_GLYPH))
	{
		return;
	}

	if((nStyle & TBBS_TEXTUAL)==0)
	{
		m_pSelectBar->AddText(m_nSelectIndex);
	}

	if((nStyle & TBBS_GLYPH)==0)
	{
		m_pSelectBar->AddBitmap(m_nSelectIndex);
	}

	m_pSelectBar->ButtonModified(m_nSelectIndex,TRUE);
}

afx_msg void CToolCustomizer::OnButtonGroupStart()
{
	// can begin group on all but first item
	if(     ((m_pSelectBar!=NULL &&
		  m_pSelectBar->IsInDialog()) ||
		  m_nSelectIndex==0))
	{
		return;
	}
	else
	{
		// now work out whether we're already starting a group
		ASSERT(m_nSelectIndex>0);
		UINT nStyle=m_pSelectBar->GetButtonStyle(m_nSelectIndex-1);

		// remove selection to avoid it moving during deletion
		CToolBarProxy *pSelectBar=m_pSelectBar;
		int nSelectIndex=m_nSelectIndex;
		SetSelection(NULL,0);

		if(nStyle & TBBS_SEPARATOR)
		{
			// remove the separator
			pSelectBar->RemoveSeparator(nSelectIndex);
			SetSelection(m_pSelectBar, nSelectIndex-1);
		}
		else
		{
			// insert a separator
			pSelectBar->InsertSeparator(nSelectIndex);
			SetSelection(m_pSelectBar, nSelectIndex+1);
		}
	}
}

afx_msg void CToolCustomizer::OnUpdateButtonCopyImage(CCmdUI *pCmdUI)
{
	UINT nStyle=m_pSelectBar->GetButtonStyle(m_nSelectIndex);

	if((m_pSelectBar!=NULL &&
		m_pSelectBar->IsVisibleControl(m_nSelectIndex)) ||
	   (nStyle & (TBBS_GLYPH))==0)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

afx_msg void CToolCustomizer::OnUpdateButtonPasteImage(CCmdUI *pCmdUI)
{
	ASSERT(m_pSelectBar);
	pCmdUI->Enable(m_pSelectBar->CanPasteImage());
}


afx_msg void CToolCustomizer::OnUpdateButtonResetImage(CCmdUI *pCmdUI)
{
	ASSERT(m_pSelectBar);
	pCmdUI->Enable(m_pSelectBar->CanResetImage());
}

afx_msg void CToolCustomizer::OnUpdateButtonChooseAppearance(CCmdUI *pCmdUI)
{
	ASSERT(m_pSelectBar);
	pCmdUI->Enable(m_pSelectBar->CanChooseAppearance());
}

afx_msg void CToolCustomizer::OnUpdateButtonReset(CCmdUI *pCmdUI)
{
	ASSERT(m_pSelectBar);

	UINT nStyle=m_pSelectBar->GetButtonStyle(m_nSelectIndex);

	// we can always reset menus, but other things can only be reset in the same conditions as custom glyphs
	if(nStyle & TBBS_MENU)
	{
		if((m_pSelectBar!=NULL &&
			m_pSelectBar->IsInDialog()) ||
			m_pSelectBar->IsVisibleControl(m_nSelectIndex))
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(TRUE);
		}
	}
	else
	{
		// can always reset custom text buttons
		if(nStyle & TBBS_CUSTOMTEXT)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			OnUpdateButtonResetImage(pCmdUI);
		}
	}
}

afx_msg void CToolCustomizer::OnUpdateButtonDelete(CCmdUI *pCmdUI)
{
	ASSERT(m_pSelectBar);
	// everything can be deleted
	if( m_pSelectBar->IsInDialog())
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

afx_msg void CToolCustomizer::OnUpdateButtonImageOnly(CCmdUI *pCmdUI)
{
	ASSERT(m_pSelectBar);
	APPEARANCEOPTION aoText=m_pSelectBar->CanText(m_nSelectIndex);
	APPEARANCEOPTION aoImage=m_pSelectBar->CanGlyph(m_nSelectIndex);
	
	if(!m_pSelectBar->IsInDialog() &&
		GLOBAL_DATA::CanChooseImageOnly(aoText, aoImage))
	{
		// we can only use the menu to switch to this if we already have a glyph.
		int iImage;
		UINT nStyle;
		UINT nID;
		m_pSelectBar->GetButtonInfo(m_nSelectIndex, nID, nStyle, iImage);

		BOOL bHasGlyph=TRUE;
		if((nStyle & TBBS_GLYPH)==0)
		{
			bHasGlyph=theApp.HasCommandBitmap(nID);
		}

		if(bHasGlyph)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}

	if(m_pSelectBar->GetAppearance(m_nSelectIndex)==cAppearanceImageOnly)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

afx_msg void CToolCustomizer::OnUpdateButtonTextOnly(CCmdUI *pCmdUI)
{
	ASSERT(m_pSelectBar);
	APPEARANCEOPTION aoText=m_pSelectBar->CanText(m_nSelectIndex);
	APPEARANCEOPTION aoImage=m_pSelectBar->CanGlyph(m_nSelectIndex);
	
	if(!m_pSelectBar->IsInDialog() &&
		GLOBAL_DATA::CanChooseTextOnly(aoText, aoImage))
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}

	if(m_pSelectBar->GetAppearance(m_nSelectIndex)==cAppearanceTextOnly)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

afx_msg void CToolCustomizer::OnUpdateButtonImageText(CCmdUI *pCmdUI)
{
	ASSERT(m_pSelectBar);
	APPEARANCEOPTION aoText=m_pSelectBar->CanText(m_nSelectIndex);
	APPEARANCEOPTION aoImage=m_pSelectBar->CanGlyph(m_nSelectIndex);
	
	if(!m_pSelectBar->IsInDialog() &&
		GLOBAL_DATA::CanChooseImageText(aoText, aoImage))
	{
		int iImage;
		UINT nStyle;
		UINT nID;
		m_pSelectBar->GetButtonInfo(m_nSelectIndex, nID, nStyle, iImage);

		BOOL bHasGlyph=TRUE;
		if((nStyle & TBBS_GLYPH)==0)
		{
			bHasGlyph=theApp.HasCommandBitmap(nID);
		}

		if(bHasGlyph)
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	
	if(m_pSelectBar->GetAppearance(m_nSelectIndex)==cAppearanceImageText)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}
}

afx_msg void CToolCustomizer::OnUpdateButtonGroupStart(CCmdUI *pCmdUI)
{
	// can begin group on all but first item
	if(     ((m_pSelectBar!=NULL &&
		  m_pSelectBar->IsInDialog()) ||
		  m_nSelectIndex==0))
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);

		// now work out whether we're already starting a group
		ASSERT(m_nSelectIndex>0);

		UINT nStyle=m_pSelectBar->GetButtonStyle(m_nSelectIndex-1);

		if(nStyle & TBBS_SEPARATOR)
		{
			pCmdUI->SetCheck(TRUE);
		}
		else
		{
			pCmdUI->SetCheck(FALSE);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// CToolBarProxy::GetBarProxy
CToolBarProxy* CToolBarProxy::GetBarProxy(CObject* pBar)
{
	// FUTURE: Add CRuntimeClass pointers and conversion function
	// points to a list, and then this routine will just walk the list.

	if (pBar->IsKindOf(RUNTIME_CLASS(CCustomBar)))
		return static_cast<CCustomBar*>(pBar)->GetProxy();
	else if (pBar->IsKindOf(RUNTIME_CLASS(CBMenuPopup)))
		return static_cast<CBMenuPopup*>(pBar)->GetProxy();
	else
		return NULL;
}
