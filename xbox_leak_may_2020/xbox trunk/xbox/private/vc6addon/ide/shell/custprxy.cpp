#include "stdafx.h"

#include "custprxy.h"

#include "barglob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CCustomBarProxy
// This class implements the bar proxy on top of a CCustomBar.
void CCustomBarProxy::AttachBar(CCustomBar* pBar)
	{ m_pBar = pBar; }
CWnd* CCustomBarProxy::GetWindow()
	{ return m_pBar; }
void CCustomBarProxy::OnSelChange(int nIndex)
	{ m_pBar->OnSelChange(nIndex); }
BOOL CCustomBarProxy::IsInDialog()
	{ return m_pBar->IsInDialog(); }
BOOL CCustomBarProxy::IsStaticItem(int nIndex)
	{ return m_pBar->IsStaticItem(nIndex); }
BOOL CCustomBarProxy::IsGapItem(int nIndex)
	{ return m_pBar->IsGap(nIndex); }
void CCustomBarProxy::OnDeleteSelection()
	{ m_pBar->OnDeleteSelection(); }
void CCustomBarProxy::DeleteButton(int nIndex, BOOL bMoving /*= FALSE*/)
	{ m_pBar->DeleteButton(nIndex, bMoving); }
int CCustomBarProxy::InsertButton(int nIndex, TRANSFER_TBBUTTON& trans, BOOL bReplace, GAP gap)
	{ return m_pBar->InsertButton(nIndex, trans, bReplace, gap); }
void CCustomBarProxy::RecalcLayout(UINT nHTSize /*= HTRIGHT*/)
	{ m_pBar->RecalcLayout(nHTSize); }
void CCustomBarProxy::GetItemRect(int nIndex, LPRECT lpRect) const
	{ m_pBar->GetItemRect(nIndex, lpRect, GetOrient()); }
UINT CCustomBarProxy::GetButtonStyle(int nIndex) const
	{ return m_pBar->GetButtonStyle(nIndex); }
void CCustomBarProxy::GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const
	{ m_pBar->GetButtonInfo(nIndex, nID, nStyle, iImage); }
UINT CCustomBarProxy::GetExpansionConstraint(const CRect &expand, ORIENTATION *orBar /*= NULL*/)
	{ return m_pBar->GetExpansionConstraint(expand, orBar); }
const CSize CCustomBarProxy::GetButtonSize() const
	{ return m_pBar->m_sizeButton; }
const CSize CCustomBarProxy::GetImageSize() const
	{ return m_pBar->m_sizeImage; }
int CCustomBarProxy::GetCount() const
	{ return m_pBar->m_nCount; }
DWORD CCustomBarProxy::GetBarStyle() const
	{ return m_pBar->GetBarStyle(); }
int CCustomBarProxy::IndexFromPoint(const CPoint& pt, CRect *pButtonRect) const
	{ return m_pBar->IndexFromPoint(pt, pButtonRect); }

int CCustomBarProxy::HitTest(CPoint point) const
{
	return m_pBar->HitTest(point);
}

CToolBarProxy* CCustomBarProxy::NewBar(UINT nID, const CPoint& pt)
{ 
	CCustomBar *pBar=m_pBar->NewBar(nID, pt);

	if(pBar) 
	{
		return pBar->GetProxy();
	}
	else
	{
		return NULL;
	}
}

void CCustomBarProxy::OnBarActivate()
	{ m_pBar->OnBarActivate(); }
void CCustomBarProxy::OnButtonSize()
	{ m_pBar->OnButtonSize(); }
void CCustomBarProxy::OnButtonMove(int nIndex)
	{ if (GetButtonStyle(nIndex) & TBBS_MENU) m_pBar->ExpandItem(nIndex, FALSE); }
void CCustomBarProxy::GetButtonTransferInfo(int nIndex, DWORD& dwData, TRANSDATA_CALLBACK* lplpDataCallback) const
	{ m_pBar->GetButtonTransferInfo(nIndex, dwData, lplpDataCallback); }
HBITMAP CCustomBarProxy::GetImageWell(int nIndex)
	{ return m_pBar->m_hbmImageWell; }
int CCustomBarProxy::GetImageIndex(int nIndex) const
	{ return m_pBar->_GetButtonPtr(nIndex)->iImage; }
BOOL CCustomBarProxy::CanChooseAppearance()
	{ return m_pBar->CanChooseAppearance(); }
BOOL CCustomBarProxy::CanResetImage()
	{ return m_pBar->CanResetImage(); }
BOOL CCustomBarProxy::CanPasteImage()
	{ return m_pBar->CanPasteImage(); }
void CCustomBarProxy::PasteButtonImage()
	{ m_pBar->PasteButtonImage(); }
void CCustomBarProxy::ResetButtonImage()
	{ m_pBar->ResetButtonImage(); }
void* CCustomBarProxy::_GetRealObject()
	{ return m_pBar; }
const CString& CCustomBarProxy::GetButtonText(int nIndex) const
	{ if (!HasExInfo(nIndex)) return afxEmptyString; return m_pBar->GetButtonExtra(nIndex)->GetLabel(); }
HWND CCustomBarProxy::GetButtonControl(int nIndex) const
	{ if (!HasExInfo(nIndex)) return NULL; return m_pBar->GetButtonExtra(nIndex)->GetControl(); }
HBITMAP CCustomBarProxy::GetButtonImage(int nIndex) const
	{ if (!HasExInfo(nIndex)) return NULL; return m_pBar->GetButtonExtra(nIndex)->GetGlyph(); }
void CCustomBarProxy::SetButtonControl(int nIndex, HWND hControl)
	{ m_pBar->EditButtonExtra(nIndex)->SetControl(hControl); }
void CCustomBarProxy::GetButtonInfoEx(int nIndex, HWND& hControl, CString& rText, HBITMAP& hImage) const
	{ ASSERT(HasExInfo(nIndex)); hControl = GetButtonControl(nIndex); rText = GetButtonText(nIndex); hImage = GetButtonImage(nIndex); }
BOOL CCustomBarProxy::HasExInfo(int nIndex) const
	{ return m_pBar->HasButtonExtra(); }
ORIENTATION CCustomBarProxy::GetOrient() const
	{ return m_pBar->GetOrient(); }
// adds text to the button
void CCustomBarProxy::AddText(int nIndex) // default menu text
{
	m_pBar->AddText(nIndex);
}

void CCustomBarProxy::AddText(int nIndex, const CString &text, BOOL bCustom)
{
	m_pBar->AddText(nIndex, text, bCustom);
}

void CCustomBarProxy::AddBitmap(int nIndex, BITMAPSOURCE bsSource, HBITMAP hbmCustom)
{
	m_pBar->AddBitmap(nIndex, bsSource, hbmCustom);
}

// remove the label from a glyph+label button
void CCustomBarProxy::RemoveText(int nIndex)
{
	m_pBar->RemoveText(nIndex);
}

// remove the glyph from a glyph+label button
void CCustomBarProxy::RemoveBitmap(int nIndex)
{
	m_pBar->RemoveBitmap(nIndex);
}

// call this when the button has been modified to mark the bar dirty and possibly redraw (if bSized==TRUE)
void CCustomBarProxy::ButtonModified(int nIndex, BOOL bSized)
{
	m_pBar->ButtonModified(nIndex, bSized);
}

// Removes a separator before a button
void CCustomBarProxy::RemoveSeparator(int iButton)
{
	m_pBar->RemoveSeparator(iButton);
}

// Inserts a separator before a button
void CCustomBarProxy::InsertSeparator(int iButton)
{
	m_pBar->InsertSeparator(iButton);
}

// returns true if the item is a control, and is currently visible
BOOL CCustomBarProxy::IsVisibleControl(int iButton)
{
	return m_pBar->IsVisibleControl(iButton);
}

// returns if the specified button can be shown with text
APPEARANCEOPTION CCustomBarProxy::CanText(int iButton)
{
	UINT nStyle=GetButtonStyle(iButton);

	if(	(nStyle & (TBBS_SEPARATOR)!=0) ||
		IsVisibleControl(iButton))
	{
		return cAppearanceForbidden;
	}
	else
	{
		if(nStyle & TBBS_MENU)
		{
			return cAppearanceCompulsory;
		}
		else
		{
			return cAppearanceOptional;
		}
	}
}

// returns if the specified button can be shown with a glyph
APPEARANCEOPTION CCustomBarProxy::CanGlyph(int iButton)
{
	return CanGlyph(GetButtonStyle(iButton));
}

// returns any limitations on the appearance of a given button style 
APPEARANCEOPTION CCustomBarProxy::CanText(UINT nStyle)
{
	if(	(nStyle & (TBBS_SEPARATOR|TBBS_HWND)!=0))
	{
		return cAppearanceForbidden;
	}
	else
	{
		if(nStyle & TBBS_MENU)
		{
			return cAppearanceCompulsory;
		}
		else
		{
			return cAppearanceOptional;
		}
	}
}

// returns any limitations on the appearance of a given button style
APPEARANCEOPTION CCustomBarProxy::CanGlyph(UINT nStyle)
{
	if(	(nStyle & (TBBS_SEPARATOR|TBBS_MENU|TBBS_HWND)!=0))
	{
		return cAppearanceForbidden;
	}
	else
	{
		return cAppearanceOptional;
	}
}

// returns the appearance of a button
APPEARANCE CCustomBarProxy::GetAppearance(int iButton)
{
	UINT nStyle=GetButtonStyle(iButton);
	if(nStyle & TBBS_GLYPH)
	{
		if(nStyle & TBBS_TEXTUAL)
		{
			return cAppearanceImageText;
		}
		else
		{
			return cAppearanceImageOnly;
		}
	}
	else
	{
		return cAppearanceTextOnly;
	}
}

// returns the default appearance of a button
APPEARANCE CCustomBarProxy::GetAppearance()
{
	return cAppearanceImageOnly;
}

// returns the appearance of a button of a given type when placed in this kind of container. If these display components
// are not available, the custom button dialog will appear
APPEARANCE CCustomBarProxy::GetInitialAppearance(UINT nStyle)
{
	if(nStyle & TBBS_MENU)
	{
		return cAppearanceTextOnly;
	}
	else
	{
		return cAppearanceImageOnly;
	}
}

int CCustomBarProxy::GetButtonCmdID(int nIndex) 
{
	return m_pBar->_GetButtonPtr(nIndex)->nID;
}

void CCustomBarProxy::Empty(void) 
{
	m_pBar->Empty();
}

void CCustomBarProxy::BatchBegin(int nCount)
{
	m_pBar->m_nCount=nCount;
	if (nCount > 0)
		m_pBar->m_pData = calloc(nCount, sizeof(AFX_TBBUTTON));
}

void CCustomBarProxy::BatchAdd(int iButton, int nID, UINT nStyle, int nExtra, HBITMAP hbmGlyph, const CString &text)
{
	AFX_TBBUTTON *pTBB=m_pBar->_GetButtonPtr(iButton);

	pTBB->nID = nID;
	pTBB->nStyle = nStyle;
	pTBB->iImage = nExtra;
	if(pTBB->nStyle & TBBS_CUSTOMTEXT)
	{
		ASSERT(!text.IsEmpty());
		m_pBar->EditButtonExtra(iButton)->SetLabel(text);
	}
	if(pTBB->nStyle & TBBS_CUSTOMBITMAP)
	{
		ASSERT(hbmGlyph!=NULL);
		m_pBar->EditButtonExtra(iButton)->SetGlyph(hbmGlyph);
	}

}

void CCustomBarProxy::BatchEnd()
{
	// do nothing
}

// allocates or returns an individual glyph bitmap for a given button
HBITMAP CCustomBarProxy::CreateSingleBitmap(int iButton)
{
	AFX_TBBUTTON *pTBB=m_pBar->_GetButtonPtr(iButton);

	ASSERT(pTBB->nStyle & TBBS_CUSTOMBITMAP);

	if(!m_pBar->HasButtonExtra() ||
		m_pBar->GetButtonExtra(iButton)->GetGlyph()==NULL)
	{
		// create it, because the master copy is in the well
		return GLOBAL_DATA::DuplicateBitmap(m_pBar, m_pBar->m_hbmImageWell, m_pBar->m_sizeImage, CPoint(pTBB->iImage *m_pBar->m_sizeImage.cx, 0));
	}
	else
	{
		// return the one we already have
		return m_pBar->GetButtonExtra(iButton)->GetGlyph();
	}
}

// always call this function if the one above returns non-null. Do not delete hbmSingle yourself, since it
// may still be in use elsewhere
void CCustomBarProxy::DestroySingleBitmap(int iButton, HBITMAP hbmSingle)
{
	// now we need to decide whether the bitmap was synthesised above, or whether it wasn't
	BITMAP bmpData;
	::GetObject(hbmSingle, sizeof(BITMAP), &bmpData);

	// if the bitmap is the same size as the glyphs in the current bar, then 
	// it was just temporarily created and can be deleted
	if(	bmpData.bmWidth == m_pBar->m_sizeImage.cx &&
		bmpData.bmHeight == m_pBar->m_sizeImage.cy)
	{
		::DeleteObject(hbmSingle);
	}
}

BOOL CCustomBarProxy::ExpandDestination(int nCmdIDSrc, int nIndexDest)
{
	// can't expand out of range
	if(nIndexDest<0 || nIndexDest>=m_pBar->m_nCount)
	{
		return FALSE;
	}

	AFX_TBBUTTON *pTBB=m_pBar->_GetButtonPtr(nIndexDest);
	if((pTBB-> nStyle & TBBS_MENU)==0)
	{
		// can't expand non-menus
		return FALSE;
	}

	if(pTBB->nID==nCmdIDSrc)
	{
		return FALSE;
	}

	CBMenuPopup *pMenu=theCmdCache.GetMenu(pTBB->nID);
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
		
		if(pMenuSrc->ContainsMenu(pTBB->nID))
		{
			return FALSE;
		}
	}

	// now expand the item
	m_pBar->ExpandItem(nIndexDest, TRUE);

	// wait for menu to open
	// REVIEW: Maybe able to ditch this if we can think of something better
	do
	{
		theApp.PumpMessage();
	}
	while(!pMenu->IsPainted());

	return TRUE;
}

// returns the next visible item after the specified one. nIndex==-1 gets the first visible item
int CCustomBarProxy::FindNextVisibleItem(int nIndex, BOOL bForward)
{
	return m_pBar->FindNextVisibleItem(nIndex, bForward);
}

// TRUE if this bar is using large buttons
BOOL CCustomBarProxy::HasLargeButtons() const
{
	return theApp.m_bLargeToolBars;
}
