// PChannelList.cpp : implementation file
//

#include "stdafx.h"
#include "bandeditordll.h"
#include "Band.h"
#include "BandCtl.h"
#include "BandDlg.h"
#include "PChannelList.h"
#include "PChannelName.h"
#include "PChannelPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPChannelList

CPChannelList::CPChannelList()
{
	m_pBandDlg = NULL;
}

CPChannelList::~CPChannelList()
{
}


BEGIN_MESSAGE_MAP(CPChannelList, CListBox)
	//{{AFX_MSG_MAP(CPChannelList)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPChannelList message handlers

void CPChannelList::SetBandDialog(CBandDlg* pBandDlg)
{
	m_pBandDlg = pBandDlg;
}


void CPChannelList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the Instrument
	CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)lpDrawItemStruct->itemData; 
	if(!pInstrumentItem)
		return;
	ASSERT( pInstrumentItem);
	CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
	ASSERT(pInstrument);

	CString sDrawString;
	CString sInstrumentName = CBandDlg::GetPatchName(m_pBandDlg->GetBand()->m_pComponent, pInstrument);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// if we're drawing a real item and the item is to be completely
	// draw or drawn as if it is selected, then ...

	if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
	{
		bool bSelected = false; // Flag to mark item selection
		bool bDisabled = false; // Flag to check for ignore flags
		
		CFont  boldFont;
		CFont* pNormalFont = GetFont();

		LOGFONT lfBold;
		pNormalFont->GetLogFont(&lfBold); 

		lfBold.lfWeight = 600;
		boldFont.CreateFontIndirect(&lfBold);

		// set up the color and the background color

		COLORREF disabledUnselectedTextColor = ::GetSysColor(COLOR_GRAYTEXT);
		COLORREF disabledSelectedTextColor = ::GetSysColor(COLOR_3DHILIGHT);
		COLORREF disabledTextColor = disabledUnselectedTextColor;

		COLORREF newTextColor = ::GetSysColor(COLOR_WINDOWTEXT) ;
		COLORREF oldTextColor = pDC->SetTextColor(newTextColor);

		COLORREF newBkColor = ::GetSysColor(COLOR_WINDOW);
		COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

		if (newTextColor == newBkColor)
			newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

		// if the item is selected, we need to paint a selected background
		// draw it!
		CRect rect = CRect(lpDrawItemStruct->rcItem);

		if ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0 || pInstrumentItem->IsSelected())
		{
			bSelected = true;

			disabledTextColor = disabledSelectedTextColor;
			if(GetFocus() == this)
				pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
			else
				pDC->SetBkColor(::GetSysColor(COLOR_INACTIVECAPTION));

			newTextColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			pDC->SetTextColor(newTextColor);
		}
		
		if(bSelected)
		{
			CBrush brush;
			if(GetFocus() == this)
				brush.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
			else
				brush.CreateSolidBrush(::GetSysColor(COLOR_INACTIVECAPTION));

			CBrush* pOldBrush = pDC->SelectObject(&brush);
			pDC->FillRect(&rect, &brush);
			pDC->SelectObject(pOldBrush);
		}
		else
		{
			CBrush brush;
			brush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
			CBrush* pOldBrush = pDC->SelectObject(&brush);
			pDC->FillRect(&rect, &brush);
			pDC->SelectObject(pOldBrush);
		}


		// PCh#
		sDrawString.Format("%d", pInstrument->dwPChannel+1);
		DrawListItemText(pDC, lpDrawItemStruct, 0, 5, rect, sDrawString, bDisabled, pNormalFont, &boldFont);	

		// PCh Name
		sDrawString = GetPChannelName(pInstrument);
		DrawListItemText(pDC, lpDrawItemStruct, 5, 21, rect, sDrawString, bDisabled, pNormalFont, &boldFont);	

		// Instrument Name
		if(!TestIfIgnore(pInstrument, DMUS_IO_INST_PATCH))
		{
			bDisabled = true;
			pDC->SetTextColor(disabledTextColor);
		}
		else
		{
			bDisabled = false;
			pDC->SetTextColor(newTextColor);
		}
		DrawListItemText(pDC, lpDrawItemStruct, 21, 37, rect, sInstrumentName, bDisabled, pNormalFont, &boldFont);	

		// Octave
		int nOctave = (pInstrument->nTranspose)/12;
		
		if(nOctave > 0)
			sDrawString.Format("+%d", nOctave);
		else
			sDrawString.Format("%d", nOctave);

		if(!TestIfIgnore(pInstrument, DMUS_IO_INST_TRANSPOSE))
		{
			bDisabled = true;
			pDC->SetTextColor(disabledTextColor);
		}
		else
		{
			bDisabled = false;
			pDC->SetTextColor(newTextColor);
		}
		DrawListItemText(pDC, lpDrawItemStruct, 37, 41, rect, sDrawString, bDisabled, pNormalFont, &boldFont);	

		// Tranpose
		sDrawString = CBandDlg::GetInterval(pInstrument->nTranspose - nOctave*12);
		if(!TestIfIgnore(pInstrument, DMUS_IO_INST_TRANSPOSE))
		{
			bDisabled = true;
			pDC->SetTextColor(disabledTextColor);
		}
		else
		{
			bDisabled = false;
			pDC->SetTextColor(newTextColor);
		}
		DrawListItemText(pDC, lpDrawItemStruct, 41, 48, rect, sDrawString, bDisabled, pNormalFont, &boldFont);	

		
		// Find out where this priority range falls
		int nCount = 0;
		while(pInstrument->dwChannelPriority < dwaPriorityLevels[nCount] && nCount < PRIORITY_LEVELS)
			nCount++;

		sDrawString.LoadString(arrPriorityLevelNames[nCount]);
		if(!TestIfIgnore(pInstrument, DMUS_IO_INST_CHANNEL_PRIORITY))
		{
			bDisabled = true;
			pDC->SetTextColor(disabledTextColor);
		}
		else
		{
			bDisabled = false;
			pDC->SetTextColor(newTextColor);
		}
		DrawListItemText(pDC, lpDrawItemStruct, 47, 55, rect, sDrawString, bDisabled, pNormalFont, &boldFont);	

		// restore the old objects in the DC

		pDC->SetTextColor(oldTextColor);
		pDC->SetBkColor(oldBkColor);
	}

	// if the item is focused, draw the focus rectangle

	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(&(lpDrawItemStruct->rcItem));
}

CString CPChannelList::GetPChannelName(CDMInstrument* pInstrument)
{
	ASSERT(m_pBandDlg);

	// Get the project that this band belongs to...
	CBand* pBand  = m_pBandDlg->GetBand();
	IDMUSProdProject* pIProject = NULL;
	IDMUSProdNode* pIDocRootNode = NULL;

	if(FAILED(pBand->GetDocRootNode(&pIDocRootNode)))
		return ("");


	pBand->m_pComponent->m_pIFramework->FindProject(pIDocRootNode, &pIProject);

	ASSERT(pIProject);
	
	IDMUSProdPChannelName* pIPChannelName = NULL;

	WCHAR pszPChannelName[DMUS_MAX_NAME];

	if(pIProject->QueryInterface(IID_IDMUSProdPChannelName, (void**)&pIPChannelName) == S_OK)
		pIPChannelName->GetPChannelName(pInstrument->dwPChannel, pszPChannelName);
	else
		wcscpy(pszPChannelName, (wchar_t*)"");

	if(pIPChannelName)
		RELEASE(pIPChannelName);
	if(pIProject)
		RELEASE(pIProject);
	if(pIDocRootNode)
		RELEASE(pIDocRootNode);

	return CString(pszPChannelName);
}
	

void CPChannelList::DrawListItemText(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct, 
									  int nLeftOffset, int nRightOffset, 
									  CRect& originalRect, CString& sText, 
									  bool bDisabled, CFont* pNormalFont, CFont* pBoldFont)
{
	CFont* pOldFont = pDC->SelectObject(pBoldFont);
	
	CRect tempRect;
	TEXTMETRIC textMetrics;
	pDC->GetTextMetrics(&textMetrics);

	tempRect.SetRect(originalRect.left + textMetrics.tmAveCharWidth * nLeftOffset, originalRect.top, 
					 originalRect.left + textMetrics.tmAveCharWidth * nRightOffset, originalRect.bottom);

	if(bDisabled)
		pDC->SelectObject(pNormalFont);

	pDC->ExtTextOut(tempRect.left,
		tempRect.top, ETO_OPAQUE,
		&(tempRect),
		sText,
		lstrlen(sText), NULL);

	// The default font is bold...
	// The TextMetrics are always calculated on the default font 
	// for correct placement of strings
	pDC->SelectObject(pOldFont);
}

bool CPChannelList::TestIfIgnore(CDMInstrument* pInstrument, DWORD dwTestFlag)
{
	if(pInstrument->dwFlags & dwTestFlag)
		return true;
	else
		return false;
}



void CPChannelList::OnMouseMove(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWnd* pWnd = GetFocus();

	if(m_pBandDlg->m_pBandCtrl && m_pBandDlg->m_pBandCtrl->IsChild(pWnd) == false)
		return;

	SelectPChannelFromPoint(point, false);

	CListBox::OnMouseMove(nFlags, point);
}


void CPChannelList::SelectPChannelFromPoint(CPoint point, bool bSelect)
{
	// Do nothing if items are selected
	int nSelectionCount = GetSelCount();
	
	ASSERT(m_pBandDlg);

	//	GetThe item nearest to this point
	BOOL bOutside;
	UINT nIndex = ItemFromPoint(point, bOutside);

	if(!bOutside)
	{

		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)GetItemDataPtr(nIndex); 
		ASSERT( pInstrumentItem);
		if(pInstrumentItem == NULL)
			return;
		
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);
		if(pInstrument == NULL)
			return;

		if(bSelect)
		{
			pInstrumentItem->SetSelected(true);
			SetSel(nIndex);
			m_pBandDlg->EnableGridButtonAndUpdateStatus(pInstrument->dwPChannel, true);
			m_pBandDlg->UpdateCommonPropertiesObject();
			m_pBandDlg->RefreshPChannelPropertyPage();
		}
		else if(nSelectionCount == 0)
			m_pBandDlg->DisplayStatus(pInstrument->dwPChannel);

	}
}


int CPChannelList::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{
	CInstrumentListItem* pInstrumentItem1 = (CInstrumentListItem*) lpCompareItemStruct->itemData1;
	CInstrumentListItem* pInstrumentItem2 = (CInstrumentListItem*) lpCompareItemStruct->itemData2;

	// Any of the instruments are NULL we return.... 
	if(pInstrumentItem1 == NULL || pInstrumentItem2 == NULL || lpCompareItemStruct->itemID1 > 999 || lpCompareItemStruct->itemID2 > 999)
		return 0;

	CDMInstrument* pInstrument1 = pInstrumentItem1->GetInstrument();
	CDMInstrument* pInstrument2 = pInstrumentItem2->GetInstrument();

	ASSERT(pInstrument1);
	ASSERT(pInstrument2);


	if(pInstrument1->dwPChannel < pInstrument2->dwPChannel)
		return -1; // return -1 = item 1 sorts before item 2

	else if(pInstrument1->dwPChannel > pInstrument2->dwPChannel)
		return 1;  // return 1 = item 1 sorts after item 2

	else 
		return 0;  // return 0 = item 1 and item 2 sort the same
}

void CPChannelList::OnRButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	//	GetThe item nearest to this point
	BOOL bOutside;
	UINT nIndex = ItemFromPoint(point, bOutside);
	
	if(bOutside)
		return;

	if(!(nFlags & MK_CONTROL) && GetSel(nIndex) == 0)
	{
		m_pBandDlg->GetBand()->UnselectAllInstruments();
		m_pBandDlg->UpdateGridControls();
		m_pBandDlg->UpdateCommonPropertiesObject();
		m_pBandDlg->RefreshPChannelPropertyPage();
		Invalidate();
		SetSel(-1, FALSE);
	}

	SelectPChannelFromPoint(point, true);
	
	CListBox::OnRButtonDown(nFlags, point);
}

void CPChannelList::OnKillFocus(CWnd* pNewWnd) 
{
	CListBox::OnKillFocus(pNewWnd);
	
	Invalidate();
}


void CPChannelList::OnSetFocus(CWnd* pOldWnd) 
{
	CListBox::OnSetFocus(pOldWnd);
	
	Invalidate();
}

void CPChannelList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == VK_INSERT)
	{
		m_pBandDlg->InsertPChannel();
	}
	
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
