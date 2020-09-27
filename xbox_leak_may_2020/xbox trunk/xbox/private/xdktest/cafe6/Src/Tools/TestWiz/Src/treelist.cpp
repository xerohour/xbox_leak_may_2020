// treelist.cpp : Ownerdraw listbox 

#include "stdafx.h"
#include "treelist.h"
/////////////////////////////////////////////////////////////////////////////
// Custom Listbox 
#define XBITMAP 30
#define YBITMAP 20

extern TEXTMETRIC tm ;
extern int cxChar, cxCaps, cyChar ;

// Pens and Brushes we need to show selections.
extern CBrush brSelRect ;
extern CBrush brUnSelRect ;
extern CPen   penUnSelRect ;
extern CPen   penSelRect ;
extern CBitmap bmapUnCheck, bmapCheck, bmapTriangle;								 

////////////////////////////////////////////////////////////////////////////

#define COLOR_ITEM_HEIGHT   20

void CAreasLB::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	// all items are of fixed size
	// must use LBS_OWNERDRAWVARIABLE for this to work
	lpMIS->itemHeight = COLOR_ITEM_HEIGHT;
}

void CAreasLB::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->itemID >= 0) // Only if we have an item in the listbox 
	 {
	 	CDC* cdc = CDC::FromHandle(lpDrawItemStruct->hDC);
		CBitmap  *pbmap, *poldbmap;
		RECT sel ;

	 	CString cs ;
		CString &refcs = cs ;
	 	CDC compDC ;
		compDC.CreateCompatibleDC(cdc);

		GetText(lpDrawItemStruct->itemID,refcs) ;
	
		sel = lpDrawItemStruct->rcItem ;
		sel.left = sel.left ; // + XBITMAP ;

		if( ODA_DRAWENTIRE == lpDrawItemStruct->itemAction)
		{ //createcompatibleDC

		  	CDC compDC ;
			compDC.CreateCompatibleDC(cdc);

			pbmap = CBitmap::FromHandle((HBITMAP)GetItemData(lpDrawItemStruct->itemID)) ;
		 	poldbmap = compDC.SelectObject(pbmap) ;
			//Draw the bitmap
			cdc->BitBlt(lpDrawItemStruct->rcItem.left ,lpDrawItemStruct->rcItem.top,
						lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left,
						lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top ,
						&compDC,0,0,SRCCOPY);
			//Draw the text 
		   	cdc->TextOut(XBITMAP,lpDrawItemStruct->rcItem.top,refcs ) ;
			cdc->SelectObject(poldbmap) ;
			compDC.DeleteDC() ;
			
		}

		// If selectced 
		if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
			(lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
		{
			// Draw text with highlight color and background
			cdc->SelectObject(&brSelRect) ; 
  			cdc->SelectObject(&penSelRect) ;
  			cdc->Rectangle(&sel) ;
		
			cdc->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT)) ;
			cdc->SetBkColor(GetSysColor(COLOR_HIGHLIGHT)) ;
	     	
			// Bitmap 
			CDC compDC ;
			compDC.CreateCompatibleDC(cdc);

			pbmap = CBitmap::FromHandle((HBITMAP)GetItemData(lpDrawItemStruct->itemID)) ;
		 	poldbmap = compDC.SelectObject(pbmap) ;
			cdc->BitBlt(lpDrawItemStruct->rcItem.left ,lpDrawItemStruct->rcItem.top,
						lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left,
						lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top ,
						&compDC,0,0,SRCCOPY);
		
		   	cdc->TextOut(XBITMAP, lpDrawItemStruct->rcItem.top, refcs) ;
					
			//restore text colors
			cdc->SetTextColor(GetSysColor(COLOR_WINDOWTEXT)) ;
			cdc->SetBkColor(GetSysColor(COLOR_WINDOW)) ;
			//restore objects
			cdc->SelectObject(poldbmap) ;
			compDC.DeleteDC() ;


	   	}  
	 	
		if (!(lpDrawItemStruct->itemState & ODS_SELECTED) &&
			(lpDrawItemStruct->itemAction & ODA_SELECT))
		{
			// Item has been de-selected -- remove frame
			cdc->SelectObject(&brUnSelRect) ; 
			cdc->SelectObject(&penUnSelRect) ;
			cdc->Rectangle(&sel) ;

			// Draw bitmap
			pbmap = CBitmap::FromHandle((HBITMAP)GetItemData(lpDrawItemStruct->itemID)) ;
			poldbmap = compDC.SelectObject(pbmap) ;
			cdc->BitBlt(lpDrawItemStruct->rcItem.left ,lpDrawItemStruct->rcItem.top,
						lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left,
						lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top ,
						&compDC,0,0,SRCCOPY);
						
			cdc->SetTextColor(GetSysColor(COLOR_WINDOWTEXT)) ;
			cdc->SetBkColor(GetSysColor(COLOR_WINDOW)) ;
	     	
			cdc->TextOut(XBITMAP,lpDrawItemStruct->rcItem.top, refcs) ;
			//restore objects
			cdc->SelectObject(poldbmap) ;
			compDC.DeleteDC() ;

		}  
	 }
}

// Not yet implemented, still has the sample code from the color listbox.
int CAreasLB::CompareItem(LPCOMPAREITEMSTRUCT lpCIS)
{
	COLORREF cr1 = (COLORREF)lpCIS->itemData1;
	COLORREF cr2 = (COLORREF)lpCIS->itemData2;
	if (cr1 == cr2)
		return 0;       // exact match

	// first do an intensity sort, lower intensities go first
	int intensity1 = GetRValue(cr1) + GetGValue(cr1) + GetBValue(cr1);
	int intensity2 = GetRValue(cr2) + GetGValue(cr2) + GetBValue(cr2);
	if (intensity1 < intensity2)
		return -1;      // lower intensity goes first
	else if (intensity1 > intensity2)
		return 1;       // higher intensity goes second

	// if same intensity, sort by color (blues first, reds last)
	if (GetBValue(cr1) > GetBValue(cr2))
		return -1;
	else if (GetGValue(cr1) > GetGValue(cr2))
		return -1;
	else if (GetRValue(cr1) > GetRValue(cr2))
		return -1;
	else
		return 1;
}

/////////////////////////////////////////////////////////////////////////
