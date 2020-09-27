#include "stdafx.h"
#include "menuprxy.h"
#include "barglob.h"
#include "bardockx.h"
#include "prxycust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CMenuPopupProxy
// This class implements the bar proxy on top of a CBMenuPopup.
void CMenuPopupProxy::AttachMenu(CBMenuPopup* pPopup)
{
	m_pPopup = pPopup;
}

CWnd* CMenuPopupProxy::GetWindow()
{
	return m_pPopup;
}

BOOL CMenuPopupProxy::IsInDialog()
{
	return FALSE;
}

UINT CMenuPopupProxy::GetExpansionConstraint(const CRect &expand, ORIENTATION *orBar /*= NULL*/)
{
	if (orBar != NULL)
		*orBar = orVert;
	
	return HTRIGHT;
}

int CMenuPopupProxy::GetCount() const
{
	return m_pPopup->GetMenuItemCount();
}

DWORD CMenuPopupProxy::GetBarStyle() const
{
	return 0;
}

BOOL CMenuPopupProxy::HasButtonExtra() const
{
	return TRUE;
}

const CSize CMenuPopupProxy::GetButtonSize() const
{
	return CCustomBar::GetDefaultButtonSize(globalData.GetLargeMenuButtons());
}

const CSize CMenuPopupProxy::GetImageSize() const
{
	return CCustomBar::GetDefaultBitmapSize(globalData.GetLargeMenuButtons());
}

HBITMAP CMenuPopupProxy::GetImageWell(int nIndex)
{
	return m_pPopup->GetMenuItem(nIndex)->m_hbmCmdGlyph;
}

int CMenuPopupProxy::GetImageIndex(int nIndex) const
{
	return 0;
}

UINT CMenuPopupProxy::GetButtonStyle(int nIndex) const
{
	CBMenuItem* pItem = m_pPopup->GetMenuItem(nIndex);
	ASSERT(pItem != NULL);

	UINT nStyle=0;
	if(pItem->m_hbmCmdGlyph != NULL)
	{
		nStyle |= TBBS_GLYPH;
	}
	switch (pItem->GetItemType())
	{
	case CBMenuItem::MIT_Command:
		nStyle|= TBBS_TEXTUAL;
		break;

	case CBMenuItem::MIT_Popup:
		nStyle |= TBBS_MENU | TBBS_TEXTUAL;
		break;

	case CBMenuItem::MIT_Separator:
		nStyle = TBBS_SEPARATOR;
		break;
	}
	if(pItem->m_bCustomGlyph)
	{
		nStyle|=TBBS_CUSTOMBITMAP;
	}
	if(pItem->m_bCustomText)
	{
		nStyle|=TBBS_CUSTOMTEXT;
	}

	return nStyle;
}

BOOL CMenuPopupProxy::IsStaticItem(int nIndex)
{
	return FALSE;
}

// May be called for an invalid index
BOOL CMenuPopupProxy::IsGapItem(int nIndex)
{
	if(nIndex<0 || nIndex>=GetCount())
	{
		return FALSE;
	}
	else
	{
		return m_pPopup->GetMenuItem(nIndex)->GetItemType() == CBMenuItem::MIT_Separator;
	}
}

const CString& CMenuPopupProxy::GetButtonText(int nIndex) const
{
	return m_pPopup->GetMenuItem(nIndex)->m_strName;
}

HWND CMenuPopupProxy::GetButtonControl(int nIndex) const
{
	return NULL;
}

HBITMAP CMenuPopupProxy::GetButtonImage(int nIndex) const
{
	return NULL;
}

void CMenuPopupProxy::SetButtonControl(int nIndex, HWND hControl)
{
	// deliberate nop
}

void CMenuPopupProxy::GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const
{
	nID = m_pPopup->GetMenuItem(nIndex)->GetCmdID();
	nStyle = GetButtonStyle(nIndex);
	iImage = GetImageIndex(nIndex);
}

void CMenuPopupProxy::GetButtonInfoEx(int nIndex, HWND& hControl, CString& rText, HBITMAP& hImage) const
{
	hControl = GetButtonControl(nIndex);
	rText = GetButtonText(nIndex);
	hImage = GetButtonImage(nIndex);
}

BOOL CMenuPopupProxy::HasExInfo(int nIndex) const
{
	return TRUE; // always has text
}

int CMenuPopupProxy::InsertButton(int nIndex, TRANSFER_TBBUTTON& trans, BOOL bReplace, GAP gap)
{
	// first remove any submenus which could get confusingly orphaned
	m_pPopup->HideSubPopup();

	UINT nFlags = MF_BYPOSITION;
	CString strCommand;

	// load the appropriate bitmap, if required
	trans.LoadCommandBitmap(globalData.GetLargeMenuButtons());

	LPCTSTR pszText;

	if (trans.nStyle & TBBS_CUSTOMTEXT)
	{
		pszText = trans.extra.GetLabel();
	}
	else
	{
		theCmdCache.GetCommandString(trans.nID, STRING_MENUTEXT, &pszText);
		if (pszText == NULL || pszText[0] == _T('\0'))
		{
			theCmdCache.GetCommandString(trans.nID, STRING_COMMAND, &pszText);
		// (bobz, paranoia:) we do not want to hold onto the return string from GetCommandString
		// (could be dynamic string held in single global CString). So even though current code
		// would be ok - (InsertMenu below copies the string), I am copying to a local CString
		// to ensure no weird future problems

		strCommand = pszText;
		pszText = (LPCTSTR)strCommand;
		}
	}

	CBMenuPopup* pPopup = NULL;
	if (trans.nStyle & TBBS_MENU)
	{
		pPopup = theCmdCache.GetMenu(trans.nID);
		nFlags |= MF_POPUP;
	}
	else if (trans.nStyle & TBBS_CHECKED)
	{
		nFlags |= MF_CHECKED;
	}
	else if (trans.nStyle & TBBS_SEPARATOR)
	{
		nFlags |= MF_SEPARATOR;
	}

	// Menu code doesn't want to insert at end, so use -1 to append instead
	if(nIndex==m_pPopup->m_nMaxIndex)
	{
		nIndex=-1;
	}
	BOOL bInserted=m_pPopup->InsertMenu(nIndex, nFlags, trans.nID, pszText, pPopup);

	// if we inserted ok, add the accelerator to the end
	if(bInserted)
	{
		if(nIndex==-1)
		{
			nIndex=m_pPopup->GetMenuItemCount()-1;
		}

		CBMenuItem *pItem=m_pPopup->GetMenuItem(nIndex);
		if(pItem &&
			pItem->m_nCmdID!=0)
		{
			// find out short cut
			CString strKey;
			if (GetCmdKeyString(pItem->m_nCmdID, strKey))
			{
				// update in item
				pItem->SetAccelerator(strKey, TRUE);
			}
		}
	}

	m_pPopup->m_bDirty=TRUE;

	return nIndex;
}

void CMenuPopupProxy::DeleteButton(int nIndex, BOOL bMoving /*= FALSE*/)
{
	// first remove any submenus which could get confusingly orphaned
	m_pPopup->HideSubPopup();

	m_pPopup->DeleteMenu(nIndex, MF_BYPOSITION);
	
	// TRUE if the item before the one deleted was a separator
	BOOL bPrevItemSeparator=FALSE;

	// TRUE if the first visible item before the one deleted was a separator
	BOOL bPrevVisibleItemSeparator=FALSE;

	// Index of the previous visible item, if it was a separator.
	int iPrevVisibleItem=-1;
	
	// TRUE if we've found the first visible item before the one deleted
	BOOL bPrevVisibleFound=FALSE;

	// first check previous/next item so deletion is done first
	if(nIndex-1>=0)
	{
		CBMenuItem *pItem=m_pPopup->GetMenuItem(nIndex-1);
		if(	pItem &&
			pItem->GetItemType()==CBMenuItem::MIT_Separator)
		{
			bPrevItemSeparator=TRUE;
		}
	}
	else
	{
		// If we deleted the top item, then the previous item and visible item (empty space) are equivalent to a separator
		bPrevItemSeparator=TRUE;
	}

	// First handle next item possible deletion
	if(bPrevItemSeparator)
	{
		int iDelete=-1;

		if(nIndex<m_pPopup->GetMenuItemCount())
		{
			CBMenuItem *pItem=m_pPopup->GetMenuItem(nIndex);
			if(	pItem &&
				pItem->GetItemType()==CBMenuItem::MIT_Separator)
			{
				iDelete=nIndex;
			}
		}
		else
		{
			// we are at the end of the menu, which is effectively a separator, so we need to delete the previous item
			if(nIndex-1>=0)
			{
				iDelete=nIndex-1;
				--nIndex;
			}
		}

		if(iDelete!=-1)
		{
			m_pPopup->DeleteMenu(iDelete, MF_BYPOSITION);
		}
	}

	// find previous visible item
	for(int iItem=nIndex-1;iItem>=0 && !bPrevVisibleFound;--iItem)
	{
		CBMenuItem *pItem=m_pPopup->GetMenuItem(iItem);
		if(pItem &&
			pItem->IsVisible())
		{
			bPrevVisibleFound=TRUE;
			if(pItem->GetItemType()==CBMenuItem::MIT_Separator)
			{
				iPrevVisibleItem=iItem;
				bPrevVisibleItemSeparator=TRUE;
			}
		}
	}

	// no previous visible implies that it was a separator
	if(!bPrevVisibleFound)
	{
		bPrevVisibleItemSeparator=TRUE;
	}

	// if there was a separator before, we need to delete/hide
	if(bPrevVisibleItemSeparator)
	{
		int nItems=m_pPopup->GetMenuItemCount();

		// TRUE if we've found the first visible item before the one deleted
		BOOL bNextVisibleFound=FALSE;

		for(int iItem=nIndex;iItem<nItems && !bNextVisibleFound;++iItem)
		{
			// If the next visible item is a separator, we need to hide it.
			CBMenuItem *pItem=m_pPopup->GetMenuItem(iItem);
			if(	pItem &&
				pItem->IsVisible())
			{
				bNextVisibleFound=TRUE;
				if(pItem->GetItemType()==CBMenuItem::MIT_Separator)
				{
					pItem->SetVisible(FALSE);
				}
			}

		}

		// If no subsequent visible item was found, then the previous visible separator is now the last item in the menu, and
		// needs to be hidden
		if(!bNextVisibleFound)
		{
			if(iPrevVisibleItem>=0)
			{
				CBMenuItem *pItem=m_pPopup->GetMenuItem(iPrevVisibleItem);
				if(pItem)
				{
					pItem->SetVisible(FALSE);
				}
			}
		}
	}

	m_pPopup->CalcMenuSize();

	m_pPopup->m_bDirty=TRUE;
}

void CMenuPopupProxy::RecalcLayout(UINT nHTSize /*= HTRIGHT*/)
{
}

void CMenuPopupProxy::GetItemRect(int nIndex, LPRECT lpRect) const
{
	::CopyRect(lpRect, &m_pPopup->GetMenuItem(nIndex)->m_rectFields[e_FieldAll]);
}

int CMenuPopupProxy::IndexFromPoint(const CPoint& pt, CRect *pButtonRect) const
{
	CRect rcMenu;

	m_pPopup->GetClientRect(&rcMenu);

	CRect rcItem(0,0,0,0); // rect for the current item
	CRect rcOld;  // rect for the previous item
	CRect rcReturn;

	int nIndexReturn=-2;

	int yCheckCenter=pt.y;
	int yCenter=-1; // current y center
	int yOldCenter; // previous y center
	
	int nCount=GetCount();

	int nLastVisible=-1;
	CRect rcLastVisible;
	rcLastVisible.left=rcMenu.left;
	rcLastVisible.right=rcMenu.right;
	rcLastVisible.bottom=rcLastVisible.top=((rcMenu.bottom-rcMenu.top)/2)+rcMenu.top;
	BOOL bFirstSeen=FALSE;

	for (int i = 0; i < nCount; i++)
	{
		// Always skip visible items
		CBMenuItem *pItem=m_pPopup->GetMenuItem(i);
		if(!pItem->IsVisible())
		{
			continue;
		}

		rcOld=rcItem;
		yOldCenter=yCenter;

		GetItemRect(i, &rcItem);

		if(!bFirstSeen)
		{
			// make old same as item first time around, so button -1 returns rect correctly
			rcOld=rcItem;
			rcLastVisible=rcItem;

			// check for just above first button
			if(yCenter>=yCheckCenter)
			{
				nIndexReturn=-1;
				rcReturn=rcItem;
			}

			bFirstSeen=TRUE;
		}

		yCenter=rcItem.top+rcItem.Height()/2;

		// if the center of the check rect has just passed us, then return which ever one is closer
		if(	yCenter>=yCheckCenter && 
			yOldCenter<=yCheckCenter)
		{
			if(	yCenter-yCheckCenter <
				yCheckCenter-yOldCenter)
			{
				// current is nearer
				nIndexReturn=i;
				rcReturn=rcItem;
			}
			else
			{
				// previous was nearer
				nIndexReturn=nLastVisible;
				rcReturn=rcLastVisible;
			}
			break;
		}

		nLastVisible=i;
		rcLastVisible=rcItem;
	}
	if(nIndexReturn==-2)
	{
		nIndexReturn=nLastVisible;
		rcReturn=rcLastVisible;
	}
	if(pButtonRect)
	{
		*pButtonRect=rcReturn;
	}
	return nIndexReturn;
}

int CMenuPopupProxy::HitTest(CPoint point) const
{
	int nHit=m_pPopup->HitTest(point);

	if(nHit==CBMenuPopup::HT_HITSELECTEDITEM)
	{
		nHit=m_pPopup->m_nSelectedMenuItem;
	}
	if(nHit==CBMenuPopup::HT_MISSED)
	{
		nHit=-1;
	}

	return nHit;
}

CToolBarProxy* CMenuPopupProxy::NewBar(UINT nID, const CPoint& pt)
{
	CASBar* pBar = new CASBar;
	CSize sizeButton=CCustomBar::GetDefaultButtonSize(theApp.m_bLargeToolBars);

	CRect rectInit( pt.x-sizeButton.cx/2,
					pt.y-sizeButton.cy/2,
					pt.x+sizeButton.cx/2,
					pt.y+sizeButton.cy/2);
	if (!pBar->Create(((CMainFrame*) AfxGetMainWnd())->m_pManager,
		nID, rectInit))
	{
		// auto delete inside this creation function
		return NULL;
	}
	
	return pBar->GetProxy();
}

void CMenuPopupProxy::GetButtonTransferInfo(int nIndex, DWORD& dwData, TRANSDATA_CALLBACK* lplpDataCallback) const
{
	*lplpDataCallback = NULL;
}

BOOL CMenuPopupProxy::CanChooseAppearance()
{
	return TRUE;
}

BOOL CMenuPopupProxy::CanPasteImage()
{
	return TRUE;
}

BOOL CMenuPopupProxy::CanResetImage()
{ 
	if(CASBar::s_pCustomizer->m_nSelectIndex!=-1)
	{
		CBMenuItem *pItem=m_pPopup->GetMenuItem(CASBar::s_pCustomizer->m_nSelectIndex);
		if(pItem)
		{
			return pItem->m_bCustomGlyph;
		}
	}
	
	return FALSE;
}

void CMenuPopupProxy::PasteButtonImage()
{
	AddBitmap(CASBar::s_pCustomizer->m_nSelectIndex, cBitmapSourceClipboard, NULL);
}

void CMenuPopupProxy::ResetButtonImage()
{
	if(CASBar::s_pCustomizer->m_nSelectIndex!=-1)
	{
		CBMenuItem *pItem=m_pPopup->GetMenuItem(CASBar::s_pCustomizer->m_nSelectIndex);
		if(pItem)
		{
			// no longer custom
			pItem->m_bCustomGlyph=FALSE;

			// get glyph back from package, or clear it
			pItem->UpdateGlyph();

			// redraw the item
			m_pPopup->InvalidateItem(CASBar::s_pCustomizer->m_nSelectIndex);
		}
	}
}

void CMenuPopupProxy::OnSelChange(int nIndex)
{
	// Does nothing, by design.
}

void CMenuPopupProxy::OnDeleteSelection()
	{ }
void CMenuPopupProxy::OnBarActivate()
	{ }
void CMenuPopupProxy::OnButtonSize()
	{ }
void CMenuPopupProxy::OnButtonMove(int nIndex)
	{ }
void* CMenuPopupProxy::_GetRealObject()
	{ return m_pPopup; }
ORIENTATION CMenuPopupProxy::GetOrient() const
	{ return orVert; }

// adds text to the button
void CMenuPopupProxy::AddText(int nIndex) // default menu text
{
	LPCSTR pText;

	theCmdCache.GetCommandString(m_pPopup->GetMenuItem(nIndex)->GetCmdID(), STRING_COMMAND, &pText);

	CString text(pText);

	AddText(nIndex, pText, FALSE);
	m_pPopup->m_bDirty=TRUE;
}

void CMenuPopupProxy::AddText(int nIndex, const CString &text, BOOL bCustom)
{
	CBMenuItem *pItem=m_pPopup->GetMenuItem(nIndex);
	pItem->SetText(text);
	pItem->m_bCustomText=bCustom;
	m_pPopup->CalcMenuSize();
	m_pPopup->m_bDirty=TRUE;
}

void CMenuPopupProxy::AddBitmap(int nIndex, BITMAPSOURCE bsSource, HBITMAP hbmCustom)
{
	CBMenuItem *pItem=m_pPopup->GetMenuItem(nIndex);
	switch(bsSource)
	{
		case cBitmapSourcePackage:
			pItem->UpdateGlyph();
			break;

		case cBitmapSourceClipboard:
			{
				// we need to create a bitmap of the right size
				HBITMAP hbmPaste=NULL;
				
				CSize sizeCmdGlyph = CCustomBar::GetDefaultBitmapSize(globalData.GetLargeMenuButtons());
				HWND hwnd = this->GetWindow()->GetSafeHwnd();
				HDC hdc = ::GetDC(hwnd);
				hbmPaste=CreateCompatibleBitmap(hdc, sizeCmdGlyph.cx, sizeCmdGlyph.cy);
				::ReleaseDC(hwnd, hdc);
				GLOBAL_DATA::PasteGlyph(this->GetWindow(), hbmPaste, 0, sizeCmdGlyph);

				pItem->SetGlyph(hbmPaste, 0, FALSE, TRUE);
			}
			break;

		case cBitmapSourceCustom:
			pItem->SetGlyph(hbmCustom, 0, TRUE, TRUE);
			break;
	}
	m_pPopup->m_bDirty=TRUE;
	m_pPopup->InvalidateItem(nIndex);
}

// remove the glyph from a glyph+label button
void CMenuPopupProxy::RemoveBitmap(int nIndex)
{
	CBMenuItem *pItem=m_pPopup->GetMenuItem(nIndex);
	pItem->SetGlyph(NULL,0,FALSE, FALSE);
	m_pPopup->m_bDirty=TRUE;
	m_pPopup->InvalidateItem(nIndex);
}

// call this when the button has been modified to mark the bar dirty and possibly redraw (if bSized==TRUE)
void CMenuPopupProxy::ButtonModified(int nIndex, BOOL bSized)
{
	m_pPopup->m_bDirty=TRUE;
	if(bSized)
	{
		m_pPopup->CalcMenuSize();
	}
}

// Removes a separator before a button
void CMenuPopupProxy::RemoveSeparator(int iButton)
{
	m_pPopup->DeleteMenu(iButton-1,MF_BYPOSITION);
	m_pPopup->m_bDirty=TRUE;
}

// Inserts a separator before a button
void CMenuPopupProxy::InsertSeparator(int iButton)
{
	m_pPopup->InsertMenu(iButton, MF_SEPARATOR, 0, NULL, NULL);
	m_pPopup->m_bDirty=TRUE;
}

// returns if the specified button can be shown with text
APPEARANCEOPTION CMenuPopupProxy::CanText(int iButton)
{
	return cAppearanceCompulsory;
}

// returns if the specified button can be shown with a glyph
APPEARANCEOPTION CMenuPopupProxy::CanGlyph(int iButton)
{
	CBMenuItem *pItem=m_pPopup->GetMenuItem(iButton);
	ASSERT(pItem != NULL);

	if(pItem->GetItemType()==CBMenuItem::MIT_Popup)
	{
		return cAppearanceForbidden;
	}
	else
	{
		return cAppearanceOptional;
	}
}

// returns any limitations on the appearance of a given button style 
APPEARANCEOPTION CMenuPopupProxy::CanText(UINT nStyle)
{
	return cAppearanceCompulsory;
}

// returns any limitations on the appearance of a given button style
APPEARANCEOPTION CMenuPopupProxy::CanGlyph(UINT nStyle)
{
	if(	(nStyle & (TBBS_SEPARATOR|TBBS_MENU)!=0))
	{
		return cAppearanceForbidden;
	}
	else
	{
		return cAppearanceOptional;
	}
}

// returns the appearance of a button
APPEARANCE CMenuPopupProxy::GetAppearance(int iButton)
{
	CBMenuItem *pItem=m_pPopup->GetMenuItem(iButton);
	if(pItem->m_hbmCmdGlyph)
	{
		return cAppearanceImageText;
	}
	else
	{
		return cAppearanceTextOnly;
	}
}

// returns the default appearance of a button
APPEARANCE CMenuPopupProxy::GetAppearance()
{
	return cAppearanceTextOnly;
}

// returns the appearance of a button of a given type when placed in this kind of container. If these display components
// are not available, the custom button dialog will appear
APPEARANCE CMenuPopupProxy::GetInitialAppearance(UINT nStyle)
{
	if(nStyle & TBBS_MENU)
	{
		return cAppearanceTextOnly;
	}
	else
	{
		return cAppearanceImageText;
	}
}

int CMenuPopupProxy::GetButtonCmdID(int iButton) 
{
	CBMenuItem *pItem=m_pPopup->GetMenuItem(iButton);
	return pItem->GetCmdID();
}

void CMenuPopupProxy::Empty(void) 
{
	m_pPopup->DeleteAll();
}

void CMenuPopupProxy::BatchBegin(int nCount)
{
	// deliberate do nothing, since we will add normally as we go along
}

void CMenuPopupProxy::BatchAdd(int nIndex, int nID, UINT nStyle, int nExtra, HBITMAP hbmGlyph, const CString &name)
{
	CBMenuItem *pItem=new CBMenuItem;
	BOOL bShowGlyph=(nStyle & TBBS_GLYPH)!=0;
	if(nStyle & TBBS_SEPARATOR)
	{
		pItem->CreateSeparator();
	}
	else
	{
		pItem->CreateDynamic(m_pPopup, nID, name, bShowGlyph, hbmGlyph);
	}
	m_pPopup->AddMenuItem(-1, pItem);
}

void CMenuPopupProxy::BatchEnd()
{
	// again nothing - all done in there
	m_pPopup->m_bDirty=TRUE;
}

// allocates or returns an individual glyph bitmap for a given button
HBITMAP CMenuPopupProxy::CreateSingleBitmap(int iButton)
{
	CBMenuItem *pItem=m_pPopup->GetMenuItem(iButton);
	if(pItem->m_hbmOriginalGlyph)
	{
		return pItem->m_hbmOriginalGlyph;
	}
	else
	{
		return pItem->m_hbmCmdGlyph;
	}
}

// always call this function if the one above returns non-null. Do not delete hbmSingle yourself, since it
// may still be in use elsewhere
void CMenuPopupProxy::DestroySingleBitmap(int iButton, HBITMAP hbmSingle)
{
	// never do anything
}

// This should only be called if nCmdIDSrc is a menu
BOOL CMenuPopupProxy::ExpandDestination(int nCmdIDSrc, int nIndexDest)
{
	// can't expand out of range
	if(nIndexDest<0 || nIndexDest>=m_pPopup->m_nMaxIndex)
	{
		return FALSE;
	}

	CBMenuItem *pItem=m_pPopup->GetMenuItem(nIndexDest);
	if(pItem->GetItemType()!=CBMenuItem::MIT_Popup)
	{
		// can't expand non-menus
		return FALSE;
	}

	if(pItem->GetCmdID()==nCmdIDSrc)
	{
		return FALSE;
	}

	CBMenuPopup *pMenu=theCmdCache.GetMenu(pItem->GetCmdID());
	if(	pMenu->m_hWnd!=NULL &&
		pMenu->IsWindowVisible())
	{
		// already expanded
		return FALSE;
	}

	if(nCmdIDSrc)
	{
		CBMenuPopup *pMenuSrc=theCmdCache.GetMenu(nCmdIDSrc);

		ASSERT(pMenuSrc!=NULL);
		
		if(pMenuSrc->ContainsMenu(pItem->GetCmdID()))
		{
			return FALSE;
		}
	}

	m_pPopup->ShowSubPopup(nIndexDest);

	// wait for menu to open
	do
	{
		theApp.PumpMessage();
	}
	while(!pMenu->IsPainted());

	return TRUE;
}

// returns the next visible item after the specified one. nIndex==-1 gets the first visible item
int CMenuPopupProxy::FindNextVisibleItem(int nIndex, BOOL bForward)
{
	return m_pPopup->FindNextVisibleItem(nIndex, bForward);
}

// TRUE if this bar is using large buttons
BOOL CMenuPopupProxy::HasLargeButtons() const
{
	return globalData.GetLargeMenuButtons();
}
