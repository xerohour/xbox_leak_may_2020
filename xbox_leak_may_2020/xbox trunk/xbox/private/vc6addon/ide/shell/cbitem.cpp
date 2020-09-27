#include "stdafx.h"
#include "shlmenu.h"

#include "barglob.h"
#include "shlbar.h"

#include <winuser.h> // Required to get menu font with NONCLIENTMETRICS 
//OEMRESOURCE must be defined in stdafx.h to get OBM_* bitmaps.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Statics
//
CBitmap CBMenuItem::s_bmpCheckMark;
CSize CBMenuItem::s_sizeCheckMark ;

CBitmap CBMenuItem::s_bmpArrow ;
CSize CBMenuItem::s_sizeArrow ;
CSize CBMenuItem::s_sizeCmdGlyph;

//
//
//
const int c_iSeparatorHeight = 8 ;

//
//
//
void CBMenuItem::Release()
{
   delete this;
}


//
//
//
CBMenuItem::CBMenuItem()
:  m_bSelected(FALSE),
   m_bChecked(FALSE),
   m_bGrayed(FALSE),
   m_bTransient(FALSE),
   m_bDefault(FALSE),
   m_pMenuWnd(NULL),
   m_nCmdID(0),
   m_iItemType(MIT_Unknown),
   m_pMenuPopup(NULL),
   m_bExpanded(FALSE),
   m_bVisible(TRUE),
   m_hbmCmdGlyph(NULL), 
   m_hbmOriginalGlyph(NULL), 
   m_bCustomGlyph(FALSE),
   m_bCustomText(FALSE),
   m_bNeedToCalculateSizes(TRUE),
   m_bTextModified(FALSE),
   m_pCTE(NULL)
{

   for (int i = 0 ; i < c_iNumFields ; i++)
   {
      m_rectFields[i].SetRectEmpty() ;
   }

   Updated(TRUE) ;
}


//
//
//
CBMenuItem::~CBMenuItem()
{
      if (m_pMenuPopup != NULL)
      {
         ASSERT(m_iItemType == MIT_Popup) ;
		 if(m_pMenuPopup &&
			 !m_pMenuPopup->IsDropDown())
		 {
			delete m_pMenuPopup ;
		 }
      }

      if (m_hbmCmdGlyph != NULL)
      {
		  ::DeleteObject(m_hbmCmdGlyph);
      }

      if (m_hbmOriginalGlyph != NULL)
      {
		  ::DeleteObject(m_hbmOriginalGlyph);
      }
}


//
// Sync -	Forces a recalc of the size of the item.
//			Required because the cmd gylphs may have changed sizes.
//			
//
void CBMenuItem::Sync()
{

	  //
	  // TODO: These lines are in three places. Collapse.
      if ((m_iItemType == MIT_Separator) || //Separator
      (!m_bVisible || m_strName.IsEmpty()))
	  {
		  //Sizes are not recalculated if this item is a separator, or there
		  // isn't a name.
		  return ;
	  }


	 // Recalc the size.
	  CalcSizes(NULL) ;

	  // Get the bitmap glyph again.
	  UpdateGlyph() ;
}

// helper function during creation
// This routine assumes that hbmSource can be usurped by us, unless bMustCopy is true
void CBMenuItem::SetGlyph(HBITMAP hbmSource, int nIndex, BOOL bMustCopy, BOOL bCustom)
{
	CSize sizeCmdBitmap = CCustomBar::GetDefaultBitmapSize(globalData.GetLargeMenuButtons());

	BOOL bScale=FALSE;

	m_bCustomGlyph=bCustom;

	// altho bmpData is only inited inside this if, we assert later on to ensure no bad usage.
	BITMAP bmpData;

	// FUTURE: Could use GLOBAL_DATA::DuplicateBitmapSegment
	if(hbmSource)
	{
		::GetObject(hbmSource, sizeof(BITMAP), &bmpData);

		if(	sizeCmdBitmap.cy!=bmpData.bmHeight)
		{
			bScale=TRUE;
		}
	}

	// Delete the bitmap if it already exists.
	if (m_hbmCmdGlyph != NULL)
	{
		::DeleteObject(m_hbmCmdGlyph);
	}

	if(!bScale && !bMustCopy)
	{
		m_hbmCmdGlyph=hbmSource;
	}
	else
	{
		ASSERT(hbmSource);
		// need to either copy or scale
		CClientDC dc(AfxGetMainWnd() /*Window not yet created pMenuWnd*/) ;
		CDC dcSrc ;                // Set up the source bitmap/dc.
		dcSrc.CreateCompatibleDC(&dc) ;
		CBitmap* pbmpSrc = CBitmap::FromHandle(hbmSource);
		CBitmap* pOldBmpSrc = (CBitmap*)dcSrc.SelectObject(pbmpSrc) ;

		CDC dcDest ;            // Set up the dest bitmap/dc.
		dcDest.CreateCompatibleDC(&dc) ;

		if(bScale)
		{
			// Delete the bitmap if it already exists.
			if (m_hbmOriginalGlyph != NULL)
			{
				::DeleteObject(m_hbmOriginalGlyph);
			}

			if(!bMustCopy)
			{
				m_hbmOriginalGlyph=hbmSource;
			}
			else
			{
				// duplicate hbmSource
				m_hbmOriginalGlyph=GLOBAL_DATA::DuplicateBitmap(AfxGetMainWnd(), hbmSource);
			}
		}
		
		// Create a new bitmap.
		m_hbmCmdGlyph=::CreateBitmap(sizeCmdBitmap.cx, 
								sizeCmdBitmap.cy,
								dc.GetDeviceCaps(PLANES),
								dc.GetDeviceCaps(BITSPIXEL), NULL);
		CBitmap* pOldBmpDest = (CBitmap*)dcDest.SelectObject(CBitmap::FromHandle(m_hbmCmdGlyph)) ;

		if(bScale)
		{
			GLOBAL_DATA::ScaleBitmap(dcDest, sizeCmdBitmap, CPoint(sizeCmdBitmap.cx*nIndex, 0), hbmSource,  CSize(bmpData.bmWidth, bmpData.bmHeight));
		}
		else
		{
			// Blt to destination
			dcDest.BitBlt(0, 0, sizeCmdBitmap.cx, sizeCmdBitmap.cy,
						  &dcSrc, sizeCmdBitmap.cx * nIndex, 0, SRCCOPY);
		}

		// Cleanup.
		dcSrc.SelectObject(pOldBmpSrc) ;
		dcDest.SelectObject(pOldBmpDest);
	}
}

//
//
//
void CBMenuItem::UpdateGlyph()
{
	// MSL: Don't try to get glyph if it's a submenu, or if the glyph isn't system provided
	if(m_nCmdID!=0 && !m_bCustomGlyph)
	{
		int nGlyph;
		HBITMAP hbmWell;

		if(theApp.GetCommandBitmap(m_nCmdID, &hbmWell, &nGlyph, globalData.GetLargeMenuButtons(), m_pCTE))
		{
			SetGlyph(hbmWell, nGlyph, TRUE, FALSE);
		}
		else
		{
			// remove the glyph. Should never happen, but maybe the package changed its mind?
			SetGlyph(NULL, 0, FALSE, FALSE);
		}
	}
}

// removed cached sizes
void CBMenuItem::FlushSizeCache()
{
	s_sizeCmdGlyph=CSize(0,0);
	if (s_bmpCheckMark.m_hObject != NULL)
	{
		s_bmpCheckMark.DeleteObject();
	}
	if (s_bmpArrow.m_hObject != NULL)
	{
		s_bmpArrow.DeleteObject();
	}
}
// and recalculate them
void CBMenuItem::EnsureSizeCache()
{
   // Cache the checkmark bitmap
   BITMAP bm ;
   if (s_bmpCheckMark.m_hObject == NULL)
   {
      VERIFY(s_bmpCheckMark.LoadOEMBitmap(OBM_CHECK)) ;
      VERIFY(s_bmpCheckMark.GetObject(sizeof(BITMAP),(void*)&bm)) ;
      s_sizeCheckMark.cx = bm.bmWidth;
      s_sizeCheckMark.cy = bm.bmHeight;
   }
   ASSERT_VALID(&s_bmpCheckMark) ;

   if (s_bmpArrow.m_hObject == NULL)
   {
      VERIFY(s_bmpArrow.LoadOEMBitmap(OBM_MNARROW)) ;
      VERIFY(s_bmpArrow.GetObject(sizeof(BITMAP),(void*)&bm)) ;
      s_sizeArrow.cx = bm.bmWidth;
      s_sizeArrow.cy = bm.bmHeight;
   }
   ASSERT_VALID(&s_bmpArrow);

   if(s_sizeCmdGlyph.cx==0)
   {
	   s_sizeCmdGlyph=CCustomBar::GetDefaultBitmapSize(globalData.GetLargeMenuButtons());
	   // two for the 3d effect, plus one for pressing offset
	   s_sizeCmdGlyph.cx+=3;
	   s_sizeCmdGlyph.cy+=3;
   }
}

// Creation
//
//
//
BOOL CBMenuItem::Create(CBMenuPopup* pMenuWnd, UINT nCmdID, CString strName)
{
   ASSERT(pMenuWnd) ;
   ASSERT_VALID(pMenuWnd) ;
  // ASSERT(nCmdID != 0) ;

   // cache command table entry pointer
   if(nCmdID!=0)
   {
	   m_pCTE=theCmdCache.GetCommandEntry(nCmdID);
   }

   ParseOutAccelerator(strName) ;

   m_nCmdID = nCmdID ;
   m_pMenuWnd = pMenuWnd ;
   m_iItemType = MIT_Command ;

   // Get the glyph for this command
   UpdateGlyph() ;

   EnsureSizeCache();

   // copy transient flag from menu, or from command table
   m_bTransient=(CBMenuPopup::s_bCreateTransients || (m_pCTE && (m_pCTE->flags & CT_DYNAMIC)!=0));

   return TRUE ;
}

//
// Create a Popup
//
BOOL CBMenuItem::CreatePopup(CBMenuPopup* pMenuWnd, 
                        CBMenuPopup* pMenuPopup, 
                        CString strName)
{
   if (Create(pMenuWnd, (UINT)NULL, strName))
   {
      m_iItemType = MIT_Popup ;
      m_pMenuPopup = pMenuPopup ;
      return TRUE ;
   }
   return FALSE ;

}

//
// Create a Popup
//
BOOL CBMenuItem::CreatePopup(CBMenuPopup* pMenuWnd, 
                        UINT nCmdID, 
                        CString strName)
{
   if (Create(pMenuWnd, nCmdID, strName))
   {
      m_iItemType = MIT_Popup ;
      m_pMenuPopup = NULL ;
      return TRUE ;
   }
   return FALSE ;

}

// Creation for a menu item whose type is determined by the global command table
// This routine assumes that hbmCustom can be usurped by us
BOOL CBMenuItem::CreateDynamic(CBMenuPopup* pMenuWnd, UINT nCmdID, CString strName,  BOOL bShowGlyph, HBITMAP hbmCustom)
{
	m_pCTE=theCmdCache.GetCommandEntry(nCmdID);
	
	// If the caller didn't provide a name, use the one in the command table. Menu entries have to have a name.
	if(	strName.IsEmpty() &&
		m_pCTE)
	{
		LPCTSTR pText=NULL;
		theCmdCache.GetCommandString(nCmdID, STRING_MENUTEXT, &pText);
		strName=pText;
	}

	if(	m_pCTE &&
		m_pCTE->flags & CT_MENU)
	{
		if(!CreatePopup(pMenuWnd, nCmdID, strName))
		{
			return FALSE;
		}
	}
	else
	{
		if(!Create(pMenuWnd, nCmdID, strName))
		{
			return FALSE;
		}
	}
	if(!bShowGlyph)
	{
		// Delete the bitmap if it already exists.
		if (m_hbmCmdGlyph != NULL)
		{
			::DeleteObject(m_hbmCmdGlyph);
		}
	}
	else
	{
		if(hbmCustom)
		{
			SetGlyph(hbmCustom, 0,FALSE, TRUE);
		}
	}
	return TRUE;
}

//
//
//
BOOL CBMenuItem::CreateSeparator()
{
   m_iItemType = MIT_Separator ; 
   return TRUE ;
}


//
// Attributes
//
//
//

void CBMenuItem::GetWidths(CDC* pdc, /*out*/ int** pWidths)
{
   if ((m_iItemType == MIT_Separator) || //Separator
       (!m_bVisible || m_strName.IsEmpty()))
   {	  
	   // TODO : Optomize this out.
      for (int i = 0 ; i < c_iNumFields ; i++)
      {
         m_iFieldWidths[i] = 0 ;
      }
   }
   else if (m_bNeedToCalculateSizes)
   {
	 CalcSizes(pdc) ;
   }
   *pWidths = m_iFieldWidths;
}

int CBMenuItem::GetHeight(CDC* pdc)
{
	if(!m_bVisible)
	{
		m_iHeight=0;
	}
	else if (m_iItemType == MIT_Separator)
	{
		m_iHeight = c_iSeparatorHeight ; //::GetSystemMetrics(SM_CYMENU) ; 
	}
	else if (m_strName.IsEmpty())
	{
		m_iHeight=0;
	}
	else if (m_bNeedToCalculateSizes)
	{
		CalcSizes(pdc) ;
	}
	return m_iHeight ;
}

//
// CalcSizes - Internal function used to cache the sizes of everything.
//
// If you do not have a CDC* pass NULL and CalcSizes will get its own DC.
//
void CBMenuItem::CalcSizes(CDC* pdc) 
{
	// In most cases, there will not be a window around
	// when we call this function. However, we still need a DC.
	// First, check to see if we do have a window. If not, use
	// the MainWnd.

	BOOL bDeleteDC = FALSE ;
	if (pdc == NULL)
	{
		CWnd* pWnd;
		if (m_pMenuWnd->GetSafeHwnd() == NULL)
		{
			pWnd = AfxGetMainWnd() ;
		}
		else
		{
			pWnd = m_pMenuWnd ;
		}
		pdc = new CClientDC(pWnd) ;
		bDeleteDC = TRUE ;
	}

   //
   // Calculate the size required by the menu string and the accelerator string.
   //
	
	// Select the Correct Font.
	CFont *pFont=globalData.GetMenuFont(m_bDefault);

  CFont* pOldFont = (CFont*)pdc->SelectObject(pFont) ;
  ASSERT(pOldFont) ;
  ASSERT_VALID(pOldFont) ;

  // Get the bounding box for the text by using DrawText.
  CRect rectName ;
  rectName.SetRectEmpty() ;
  pdc->DrawText( m_strName, &rectName, DT_EXTERNALLEADING | DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_SINGLELINE /*| DT_VCENTER*/);

  ASSERT(!rectName.IsRectEmpty()) ;
  ASSERT(!rectName.IsRectNull()) ;

  // Get the bounding box for the Accelerator.
  CRect rectAccel ;
  rectAccel.SetRectEmpty() ;
  if (!m_strAccel.IsEmpty())
  {
      pdc->DrawText( m_strAccel, &rectAccel, DT_EXTERNALLEADING | DT_CALCRECT | DT_LEFT | DT_EXPANDTABS | DT_SINGLELINE /*| DT_VCENTER*/);
      ASSERT(!rectAccel.IsRectEmpty()) ;
      ASSERT(!rectAccel.IsRectNull()) ;
  }

  // Cleanup
  if (pOldFont) pdc->SelectObject(pOldFont) ;


  //
  // We now have all the information we need to calculate the widths.
  //

   // Width for e_FieldAll
   m_iFieldWidths[e_FieldAll] = c_iFieldWidthUnknown ;

   // Width for e_FieldButton
   m_iFieldWidths[e_FieldButton] = s_sizeCmdGlyph.cx+1; // add one here, so that there's a 1 pixel gap between the 3d line and the blue area.

   // Width for the e_FieldName - Command Name
   m_iFieldWidths[e_FieldName] = rectName.Width() ;

   // Width for the e_FieldAccel 
   m_iFieldWidths[e_FieldAccel] = rectAccel.Width() ;


   // Width for e_FieldArrow - allow it to center in a broader area.
   m_iFieldWidths[e_FieldArrow] = s_sizeArrow.cx*2;

   // Width for e_FieldGap which is the widht of widest char.
  TEXTMETRIC tm ;
  pdc->GetOutputTextMetrics(&tm);
  m_iFieldWidths[e_FieldGap] = tm.tmMaxCharWidth;

  //
  // Now do the height
  //
  m_iHeight = ::GetSystemMetrics(SM_CYMENU) ;
  m_iHeight = max(m_iHeight, max(s_sizeCheckMark.cy,s_sizeArrow.cy)) ;
  m_iHeight = max(m_iHeight, max(s_sizeCmdGlyph.cy, rectName.Height())) ; // TODO: Vertical padding

  // Mark that we do not have to calculated the sizes now.
  m_bNeedToCalculateSizes = FALSE ;

  if (bDeleteDC)
  {
	  delete pdc ;
  }
}

//
// Operation
//
//
//
extern BOOL DrawButtonGlyph(CDC* pDC,
							int x, int y,
							CPoint ptOffset,
							int iImage,
							UINT nStyle,
							const SIZE& sizeButton,
							const SIZE& sizeImage,
							HDC hdcGlyphs,
							HDC hdcMono);
extern void CreateCmdBarButtonMask(int iImage,
								   CPoint ptOffset,
								   BOOL bHilite,
								   BOOL bShadow,
								   const SIZE& sizeButton,
								   const SIZE& sizeImage,
								   HDC hdcGlyphs,
								   HDC hdcMono);

void CBMenuItem::Draw(CDC* pdc, CRect rectFields[c_iNumFields]) 
{
	ASSERT_VALID(pdc) ;
	if (rectFields != NULL)
	{
		// DrawText Might Modify pRect...so save it.
		// Also need it for HitTest
		for(int i = 0 ; i < c_iNumFields ; i++)
		{
			m_rectFields[i] = rectFields[i];
		}
	}
	
	// Don't draw if its not visible.
	if(!m_bVisible || m_rectFields[e_FieldAll].IsRectEmpty() || m_rectFields[e_FieldAll].IsRectNull())
	{
		return;
	}
	
	// Separators are drawn with a different function.
	if (m_iItemType == MIT_Separator)
	{
		DrawSeparator(pdc, &m_rectFields[e_FieldAll]) ;
		return ;
	}
	
	
	// 
	COLORREF oldTextColor;
	// Normal and gray items have the same background color.
	COLORREF oldBkColor = pdc->SetBkColor(globalData.clrBtnFace);
	
	// grayed items can be drawn selected because the keyboard interface allows it.
	if (m_bSelected )
	{
		// The menu item is not drawn selected when it is gray.
		oldBkColor = pdc->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT)) ;
		if(m_bGrayed)
		{
			oldTextColor = pdc->SetTextColor(::GetSysColor(COLOR_INACTIVECAPTIONTEXT)) ; // !!! reconsider? 
		}
		else
		{
			oldTextColor = pdc->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT)) ;
		}
	}
	else
	{
		// Normal items have black text. Grayed items don't
		oldTextColor = pdc->SetTextColor(globalData.clrBtnText) ;
	}
	
	//
	// Handle whole rectangle.
	//
	
	// Paint everything except the button area.
	CRect rectRightFields(m_rectFields[e_FieldAll]);
	rectRightFields.left += m_rectFields[e_FieldButton].Width();
	pdc->ExtTextOut(0,0,ETO_OPAQUE, &rectRightFields, NULL, 0, NULL) ;

	//
	// Paint the Text.
	//
	
	// Select the Font appropriate to the type of item
	CFont *pFont=globalData.GetMenuFont(m_bDefault);

	CFont* pOldFont = (CFont*)pdc->SelectObject(pFont) ;
	ASSERT_VALID(pOldFont) ;
	
	// Create a rectangle where text is painted.
	CRect rectText(m_rectFields[e_FieldName]);
	CRect rectAccel(m_rectFields[e_FieldAccel]);
	
	//   int iOldBkMode = pdc->SetBkMode(TRANSPARENT) ;
	
	UINT MenuTextFlags = GLOBAL_DATA::e_mtoLeftAlign ;
	if (m_bGrayed && !m_bSelected)
	{
		MenuTextFlags |= GLOBAL_DATA::e_mtoDisabled ;
	}
	
	// Comand Text
	globalData.MenuTextOut(pdc, &rectText, m_strName, MenuTextFlags) ;
	
	// Accelerator Text
	globalData.MenuTextOut(pdc, &rectAccel, m_strAccel, 
							MenuTextFlags | GLOBAL_DATA::e_mtoRightAlign) ;
	
	//
	// Setup for blting various bitmaps.
	//
	
	// Set up a memory DC
	CDC dcMem;
	dcMem.CreateCompatibleDC(pdc) ;
	CDC dcMono ;
	dcMono.CreateCompatibleDC(pdc);
	
	// Create an alias for m_rectFields[e_FieldButton];
	CRect* pRectButton = &m_rectFields[e_FieldButton] ;
	
	CBitmap bmMono;
	bmMono.CreateBitmap(pRectButton->Width(), pRectButton->Height() , 1, 1, NULL);
	CBitmap* pOldMonoBm = static_cast<CBitmap*>(dcMono.SelectObject(&bmMono));
	
	// Arrow for the popup...
	if (m_iItemType == MIT_Popup)
	{
		// Draw the Arrow
		CBitmap* pOldBitmap = (CBitmap*)dcMem.SelectObject(&s_bmpArrow) ;
		
		CRect rectArrow(m_rectFields[e_FieldArrow]);
		
		int left = rectArrow.left + (rectArrow.Width()-s_sizeArrow.cx)/2 ;
		int top  = rectArrow.top + (rectArrow.Height()-s_sizeArrow.cy)/2 ; //Center Bitmap

		if (m_bGrayed && !m_bSelected)
		{
			// Draw the arrow diabled with a shadow.
			DrawButtonGlyph(pdc, left, top,
							CPoint(0,0), 0, TBBS_DISABLED, 
							pRectButton->Size(), s_sizeArrow, 
							dcMem.m_hDC, dcMono.m_hDC);
		}
		else
		{
			// Put down the arrow...
			pdc->BitBlt(left, top, 										
						s_sizeArrow.cx, s_sizeArrow.cy, 
						&dcMem, 0, 0, SRCCOPY) ;

		}
		// Cleanup
		dcMem.SelectObject(pOldBitmap) ;
	}
	
	// Paint the background for the glyph area.
	pdc->FillSolidRect(&m_rectFields[e_FieldButton], globalData.clrBtnFace);
	
	// Draw the cmd glyph
	if (m_hbmCmdGlyph != NULL)
	{
		CRect rectGlyph=*pRectButton;

		// if it's taller than the width of the glyph, make it square
		if(rectGlyph.Height() > s_sizeCmdGlyph.cy)
		{
			rectGlyph.OffsetRect(0,(m_iHeight-s_sizeCmdGlyph.cy)/2);

			rectGlyph.bottom-=m_iHeight-s_sizeCmdGlyph.cy;
		}

		CBitmap* pOldBitmap = (CBitmap*)dcMem.SelectObject(CBitmap::FromHandle(m_hbmCmdGlyph)) ;
		
		if (m_bChecked ||
			(m_bSelected && !m_bGrayed))
		{
			COLORREF clrTopLeft;
			COLORREF clrBotRight;

			if(!m_bChecked)
			{
				// raised if unchecked
				clrTopLeft=globalData.clrBtnHilite;
				clrBotRight=globalData.clrBtnShadow;
			}
			else
			{
				// depressed if checked
				clrBotRight=globalData.clrBtnHilite;
				clrTopLeft=globalData.clrBtnShadow;
			}

			// We do not raise the bitmap if it is disabled.
			pdc->Draw3dRect(rectGlyph.left, rectGlyph.top,
							rectGlyph.Width(), rectGlyph.Height(),
							clrTopLeft, clrBotRight);
		}
		
		UINT nStyle=0;
		CPoint ptOffset(0,0);
		if(m_bGrayed) 
		{	
			nStyle |= TBBS_DISABLED;
		}
		
		if(m_bChecked) 
		{
			nStyle |= TBBS_CHECKED;
			if(m_bSelected)
			{
				nStyle|=TBBS_EXTRUDED;
			}
			ptOffset.x=1;
			ptOffset.y=1;
		}
		
		DrawButtonGlyph(pdc,	rectGlyph.left + 1, rectGlyph.top + 1, 
								ptOffset, 0, nStyle, rectGlyph.Size(),s_sizeCmdGlyph, dcMem.m_hDC, dcMono.m_hDC);
		
		dcMem.SelectObject(pOldBitmap) ;
	}
	else if (m_bChecked)
	{
		// Draw the CheckMark
		CBitmap* pOldBitmap = (CBitmap*)dcMem.SelectObject(&s_bmpCheckMark) ;
		CRect rcCheck(	pRectButton->left + (pRectButton->Width() - s_sizeCheckMark.cx) / 2 - 1,
						pRectButton->top + (pRectButton->Height() - s_sizeCheckMark.cy) / 2 - 1,
						0, 0);
		rcCheck.right = rcCheck.left + s_sizeCheckMark.cx + 2;
		rcCheck.bottom = rcCheck.top + s_sizeCheckMark.cy + 2;
		
		// Draw the border.
		pdc->Draw3dRect(rcCheck, globalData.clrBtnShadow, globalData.clrBtnHilite);
		rcCheck.InflateRect(-1, -1);
		
		if (!m_bSelected)
		{
			COLORREF crTxt = pdc->SetTextColor(globalData.clrBtnHilite);
			COLORREF crBk = pdc->SetBkColor(globalData.clrBtnFace);
			pdc->FillRect(rcCheck, CDC::GetHalftoneBrush());
			
			pdc->SetTextColor(crTxt);
			pdc->SetBkColor(crBk);
		}
		
		COLORREF crTxt = pdc->SetTextColor(RGB(0,0,0));
		COLORREF crBk = pdc->SetBkColor(RGB(255,255,255));
		
		// This works because the original bitmap is mono.
		pdc->BitBlt(rcCheck.left, rcCheck.top, s_sizeCheckMark.cx, s_sizeCheckMark.cy, &dcMem, 0, 0, SRCAND) ; 
		
		pdc->SetTextColor(crTxt);
		pdc->SetBkColor(crBk);
		
		dcMem.SelectObject(pOldBitmap) ;
	}
	else
	{
		if(m_bSelected)
		{
			pdc->FillSolidRect(pRectButton, ::GetSysColor(COLOR_HIGHLIGHT));
		}
	}
	
	// Cleanup 
	if (pOldFont) pdc->SelectObject(pOldFont) ;
	
	pdc->SetBkColor(oldBkColor);
	if (!m_bGrayed)
	{
		pdc->SetTextColor(oldTextColor) ;
	}
	if (pOldMonoBm)
		dcMono.SelectObject(pOldMonoBm);
}


//
//
//
//
//
//
void CBMenuItem::DrawSeparator(CDC* pdc, const CRect* pRect) 
{
   // Fill Rectangle
   pdc->FillSolidRect(pRect->left, pRect->top, pRect->Width()+1, pRect->Height()+1, globalData.clrBtnFace) ; // Does not fill right/bottom edge.

   // Dark Line on Top
   int y = pRect->top + pRect->Height()/2 ;
   pdc->FillSolidRect(pRect->left+2, y, pRect->Width()-4+1, 1, globalData.clrBtnShadow) ;
   // Hilite on bottom
   y++;
   pdc->FillSolidRect(pRect->left+2, y, pRect->Width()-4+1, 1, globalData.clrBtnHilite) ;
}
 

//
// Tests whether a given item is hit, given a scroll offset
//
BOOL CBMenuItem::HitTest(CPoint pt, int)
{
   return m_rectFields[e_FieldAll].PtInRect(pt);
}

//
//
//
UINT CBMenuItem::Click()
{
   if (m_iItemType == MIT_Separator)
   {
      return 0 ;
   }

   if (m_bGrayed)
      return 0;

   if (m_iItemType == MIT_Command)
   {
      if (m_nCmdID != 0)
      {
		   // send command
		   return m_nCmdID;
      }
   }
   /*
   else (m_iItemType == MIT_Popup)
   {
   }
   */
   return 0 ;
}

//
//
//
void CBMenuItem::Check(BOOL bChecked)
{
   ASSERT( m_iItemType != MIT_Separator) ;
   m_bChecked = bChecked;
}

///
//
//
UINT CBMenuItem::Enable(BOOL bEnable)
{ 
   UINT uResult = m_bGrayed ? MF_GRAYED : MF_ENABLED ;
   m_bGrayed = !bEnable;
   return uResult ;
}

//
//
//
BOOL CBMenuItem::Show(BOOL bShow)
{ 
   BOOL bResult = m_bVisible ;
   m_bVisible = bShow;
   return bResult ;
}

//
//
//
void CBMenuItem::SetText(LPCTSTR lpszText) 
{
   ASSERT( lpszText != NULL) ;
   if (lpszText != NULL)
   {
      // Separate the Accerlerator from the command name.
      ParseOutAccelerator(lpszText) ;

	  // Recalc the size.
	  CalcSizes(NULL) ;
	}
}

//
// This modifies the accelerator, and optionally recalcs. Normally this is called when
// the menus are being rebuilt during change of focus, in which case recalc is done at the end
// in one go.
//
void CBMenuItem::SetAccelerator(LPCTSTR lpszAccel, BOOL bRecalc) 
{
	if (lpszAccel != NULL)
	{
		m_strAccel = lpszAccel;
	}
	else
	{
		m_strAccel.Empty();
	}

	if(bRecalc)
	{
		// Recalc the size.
		CalcSizes(NULL);
	}
}

//
//
//
CBMenuPopup* CBMenuItem::Expand(BOOL bExpand) 
{
	if (m_iItemType != MIT_Popup)
	  return NULL ;

	if (m_bGrayed)
	  return NULL ;

	m_bExpanded = bExpand;
	if (m_bExpanded) 
	{
		// rectFields is only initialsed after a menu has been painted. So we must paint before we expand!
		if(m_pMenuWnd &&
			!m_pMenuWnd->IsPainted())
		{
			// do any pending update now
			m_pMenuWnd->UpdateWindow();
		}

		CPoint pt(m_rectFields[e_FieldAll].right, m_rectFields[e_FieldAll].top) ;
		m_pMenuWnd->ClientToScreen(&pt) ;
		if(m_pMenuPopup==NULL)
		{
			m_pMenuPopup=theCmdCache.GetMenu(m_nCmdID);
		}

		if(m_pMenuPopup)
		{	
			// submenu should avoid its parent item
			// Bug m_pMenuPopup->m_rectAvoid=m_rectFields[e_FieldAll];

			m_pMenuPopup->Create(m_pMenuWnd, pt, TRUE, &m_rectFields[e_FieldAll]) ;
			CBMenuPopup *pParent=(CBMenuPopup *)m_pMenuWnd;
			m_pMenuPopup->m_bDropDown=pParent->m_bDropDown;
		}
		return m_pMenuPopup;
	}
	else
	{
		if(m_pMenuPopup)
		{
			m_pMenuPopup->DestroyWindow() ; 

			// depointer if required.
			if(	m_pCTE &&
				m_pCTE->flags & CT_MENU)
			{
				m_pMenuPopup=NULL;
			}
		}
		return NULL ;
	}
}

BOOL CBMenuItem::ModifyItem(UINT nNewCmdID,
                           LPCTSTR lpszNewItem, 
                           CBMenuPopup* pNewPopup, 
                           int iNewItemType)
{
   if (iNewItemType != MIT_Unknown)
   {
      for (int i = 0 ; i < c_iNumFields ; i++)
      {
         m_rectFields[e_FieldAll].SetRectEmpty() ;
      }

      // Change basic type of menu item:
      switch (iNewItemType)
      {
      case MIT_Separator:
         {
            ASSERT(m_iItemType != MIT_Separator) ;
            m_strName.Empty() ;
            m_bSelected = FALSE ;
            m_nCmdID = 0 ;
            m_bChecked = FALSE ;
            m_bGrayed = FALSE ;
            m_bExpanded = FALSE ;
            if (m_pMenuPopup != NULL)
            {
               ASSERT(m_iItemType == MIT_Popup) ;
               delete m_pMenuPopup ;
            }
            m_iItemType = MIT_Separator;
         }
         break;
      case MIT_Popup:
         {
            ASSERT(m_iItemType != MIT_Popup) ;
            ASSERT(m_pMenuPopup == NULL) ;
            m_nCmdID = 0 ;
            //?? m_bChecked = FALSE ;
            m_bSelected = FALSE ; // This could cause problems...
            m_bExpanded = FALSE;
            // Wait for next section: m_pMenuPopup = pNewPopup ;
            m_iItemType = MIT_Popup;
         }
         break;
      case MIT_Command:
         {
            ASSERT(m_iItemType != MIT_Command) ;
            m_nCmdID = 0 ;
            m_bChecked = FALSE ;
            m_bGrayed = FALSE ;
            m_bExpanded = FALSE ;
            if (m_pMenuPopup != NULL)
            {
               ASSERT(m_iItemType == MIT_Popup) ;
               delete m_pMenuPopup ;
            }
            m_iItemType = MIT_Command;
         }
         break;
      default:
         ASSERT(0);
         return FALSE;
      }
   }

   //
   // Now make changes to the type itself.
   // Keep in mind that the above case falls through.
   //
   switch (m_iItemType)
   {
   case MIT_Separator:
      {
         // Ignore the following:
         // nNewCmdID
         // lpszNewItem
         // pSubPopup
      }
      break;
   case MIT_Popup:
      {
         if (pNewPopup != NULL)
         {
            if (m_pMenuPopup != NULL)
            {
               delete m_pMenuPopup ;
            }
            m_pMenuPopup = pNewPopup ;
            m_bExpanded = FALSE ;
         }

		 SetText(lpszNewItem) ;
         // Ignore the following:
         // nNewCmdID
      }
      break;
   case MIT_Command:
      {
         if (nNewCmdID != 0)
         {
            m_nCmdID = nNewCmdID ;
         }

		 SetText(lpszNewItem) ;
         // Ignore the following:
         // pSubPopup
      }
      break;
   default:
      ASSERT(0);
      return FALSE ;
   }

   return TRUE ;
}

//
// ParseOutAccelerator 
//
// This function removes the accerator from the menu string.
// I was going to change the code so that the accerator was not added,
// However, That wouldn't handle CmdUIUpdate functions updating the text.
//

void CBMenuItem::ParseOutAccelerator(CString strWhole)
{
  int index = strWhole.Find(L'\t') ;
	
  if (index < 0)
  {
	  m_strName = strWhole ;
  }
  else
  {
	  m_strName = strWhole.Left(index) ;
	  m_strAccel = strWhole.Mid(index+1) ;
  }

};

//
// Posts a WM_MENUSELECT message so that the menu will show the status bar
// text.
//
// This function implements only a subset of the proper WM_MENUSELCT code.
// It never sets the following flags:
//			MF_OWNERDRAW
//			MF_SYSMENU
//			MF_BITMAP
//			MF_HILITE
//			MF_DISABLED
//
// This menu also doesn't send out the handle to the popup.
//
void CBMenuItem::Select(BOOL bSelect)
{

	m_bSelected = bSelect;

	if (bSelect)
	{
		// Only send a menu select if we are selecting the menu.

		UINT flags = 0 ;	
		if (m_bChecked)
		{
			flags |= MF_DISABLED ;
		}

		if (m_bGrayed)
		{
			flags |= MF_GRAYED ;
		}

		if (m_iItemType == MIT_Popup)
		{
			flags |= MF_POPUP ;
		}

		// Send a WM_MENUSELECT command to the main window. This message results
		// in the menu prompt displaying in the status bar. Notice the NULL
		// for the last parameter. This is only needed if this menu is the
		// system menu.
		AfxGetMainWnd()->SendMessage(WM_MENUSELECT, MAKELONG(m_nCmdID, flags), NULL) ;
	}
}

void CBMenuItem::SetDefault(BOOL bDefault)
{ 
	if(m_iItemType==MIT_Command)
	{
		// flush cache
		m_bNeedToCalculateSizes=TRUE; 
		m_bDefault=bDefault; 

		if(m_pMenuWnd)
		{
			// recalc whole of menu. Could be extraneous, but probably not since this is primarily used in context menu.
			m_pMenuWnd->CalcMenuSize();
		}
	}
}

