//
// CTargetCombo
//
// Implementation of the customizable target combo
//
// [colint]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "targctrl.h"	// our local header

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern void SizeComboToContent(CComboBox* pCombo, int nMax = 0);

CObList CTargetControl::s_listControls;

BEGIN_MESSAGE_MAP(CTargetControl, CWnd)
    //{{AFX_MSG_MAP(CTargetControl)
	ON_CBN_SELCHANGE(ID_TARGET_COMBO, OnSelChange)
	ON_CBN_DROPDOWN(ID_TARGET_COMBO, OnDropList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTargetControl::CTargetControl()
{
	s_listControls.AddHead(this);
}

CTargetControl::~CTargetControl()
{
	CObject * pObj;

	POSITION pos = s_listControls.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = s_listControls.GetAt(pos);
		if (pObj == this)
		{
			s_listControls.RemoveAt(pos);
			break;
		}
		s_listControls.GetNext(pos);
	}
}
 
BOOL CTargetControl::Create(DWORD dwStyle, const RECT& rect, CWnd * pParentWnd, UINT nID)
{
	if (!CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	m_pTargetCombo = new CTargetCombo;

	CRect rectCombo(0, 0, 160, 120);
  	if (!m_pTargetCombo->Create(WS_TABSTOP | WS_VISIBLE | WS_VSCROLL |
		CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS,
		rectCombo, this, ID_TARGET_COMBO))
 	{
		TRACE("Failed to create target combo.\n");
		delete m_pTargetCombo;
		return FALSE;
	}

	return TRUE;
}

LRESULT CTargetControl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG lResult;

	if (DkPreHandleMessage(GetSafeHwnd(), message, wParam, lParam, &lResult))
		return lResult;

	return CWnd::WindowProc(message, wParam, lParam);
}

void CTargetControl::PostNcDestroy()
{
	delete this;
}

void CTargetControl::OnSelChange()
{
	ASSERT(m_pTargetCombo);

	int nIndex = m_pTargetCombo->GetCurSel();
	ASSERT(nIndex != CB_ERR);

	CString strTarget;
	m_pTargetCombo->GetLBText(nIndex, strTarget);

    // Set the active config - note this will cause
    // a notification, which will eventually reach us
    // at which point we set the new selection up

	HBLDTARGET hTarget = g_BldSysIFace.GetTarget(strTarget, NO_BUILDER);
	ASSERT(hTarget);
	HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);
	ASSERT(hBld != NO_BUILDER);
	g_BldSysIFace.SetActiveTarget(hTarget, hBld);
}

void CTargetControl::OnDropList()
{
}


CObList CConfigControl::s_listControls;

BEGIN_MESSAGE_MAP(CConfigControl, CWnd)
    //{{AFX_MSG_MAP(CConfigControl)
	ON_CBN_SELCHANGE(ID_CONFIG_COMBO, OnSelChange)
	ON_CBN_DROPDOWN(ID_CONFIG_COMBO, OnDropList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CConfigControl::CConfigControl()
{
	s_listControls.AddHead(this);
}

CConfigControl::~CConfigControl()
{
	CObject * pObj;

	POSITION pos = s_listControls.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = s_listControls.GetAt(pos);
		if (pObj == this)
		{
			s_listControls.RemoveAt(pos);
			break;
		}
		s_listControls.GetNext(pos);
	}
}
 
BOOL CConfigControl::Create(DWORD dwStyle, const RECT& rect, CWnd * pParentWnd, UINT nID)
{
	if (!CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	m_pConfigCombo = new CConfigCombo;

	CRect rectCombo(0, 0, 100, 120);
  	if (!m_pConfigCombo->Create(WS_TABSTOP | WS_VISIBLE | WS_VSCROLL |
		CBS_DROPDOWNLIST | CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS,
		rectCombo, this, ID_CONFIG_COMBO))
 	{
		TRACE("Failed to create target combo.\n");
		delete m_pConfigCombo;
		return FALSE;
	}

	return TRUE;
}

LRESULT CConfigControl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG lResult;

	if (DkPreHandleMessage(GetSafeHwnd(), message, wParam, lParam, &lResult))
		return lResult;

	return CWnd::WindowProc(message, wParam, lParam);
}

void CConfigControl::PostNcDestroy()
{
	delete this;
}

void CConfigControl::OnSelChange()
{
	ASSERT(m_pConfigCombo);

	int nIndex = m_pConfigCombo->GetCurSel();
	ASSERT(nIndex != CB_ERR);

	CString strConfig;
	m_pConfigCombo->GetLBText(nIndex, strConfig);

    // Set the active config - note this will cause
    // a notification, which will eventually reach us
    // at which point we set the new selection up

	// HBLDTARGET hTarget = g_BldSysIFace.GetTarget(strTarget, NO_BUILDER);
	// ASSERT(hTarget);
	// HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);
	// ASSERT(hBld != NO_BUILDER);
	// g_BldSysIFace.SetActiveTarget(hTarget, hBld);
}

void CConfigControl::OnDropList()
{
}

// *
// * Target Combo implementation
// *


BOOL CBuildCombo::Create(DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	// get the height of this font 'M'
	CClientDC dc(pParentWnd);	// use our parent's
	CFont * pFontOld = dc.SelectObject(GetStdFont(0));
	m_cyItem = dc.GetTextExtent("M", 1).cy;
	dc.SelectObject(pFontOld);

	if (!CDockCombo::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	// set extended UI and also our 'thin' font
	SetExtendedUI();
	SetFont(GetStdFont(0));

	SubclassCtl3d();

	RefreshTargets();

	return TRUE;
}

void CBuildCombo::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// height of box?
	if (lpMeasureItemStruct->itemID == -1)
		lpMeasureItemStruct->itemHeight = m_cyItem + 1;	// Was +2, but that was too big.
	else
		lpMeasureItemStruct->itemHeight = m_cyItem;
}

void CBuildCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	if (lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))
	{
		BOOL fIsDisabled = !IsWindowEnabled();
		BOOL fDrawSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0;
		BOOL fHasFocus = ::GetFocus() == lpDrawItemStruct->hwndItem;
		COLORREF rgbText, rgbBackground;

		// draw the item text
		if (fHasFocus && fDrawSelected && !fIsDisabled)
		{
			rgbText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
			rgbBackground = ::GetSysColor(COLOR_HIGHLIGHT);
		}
		else
		{
			// when not highlighted do we want to gray? 
			// if == 0 then 'disabled'
			BOOL fGrayText = fIsDisabled || !lpDrawItemStruct->itemData;

			rgbBackground = ::GetSysColor(COLOR_WINDOW);
			rgbText = ::GetSysColor(fGrayText ? COLOR_GRAYTEXT : COLOR_WINDOWTEXT);

			// make sure we don't have the same color as the background when showing disabled
			if (fGrayText && rgbText == ::GetSysColor(COLOR_WINDOW))
				rgbText = ::GetSysColor(COLOR_BTNSHADOW);
		}

		// set the colors in the DC
		(void) pDC->SetTextColor(rgbText); (void) pDC->SetBkColor(rgbBackground);
		
		// string to draw?
		CString str;
		
		// itemID can be -1 when the combobox is empty
		if (lpDrawItemStruct->itemID != -1)
			GetLBText(lpDrawItemStruct->itemID, str);

		// do the text drawn centred vertically (if we need to)

		if (!str.IsEmpty())
			pDC->ExtTextOut(lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, ETO_OPAQUE | ETO_CLIPPED,
							&lpDrawItemStruct->rcItem, str, str.GetLength(), NULL);
		
		// selection to draw?
		if (fDrawSelected)
		{
			if (fHasFocus)
			{
				pDC->SetTextColor(rgbText);
				pDC->SetBkColor(rgbBackground);
			}
			else
			{
				// highlight is a frame when we don't have the focus...
				CBrush brush;
				if (brush.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)))
					pDC->FrameRect(&lpDrawItemStruct->rcItem, &brush);
			}
		}
	}
	
	// draw the focus rectangle?
	if (lpDrawItemStruct->itemAction == ODA_FOCUS)
		pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
}

void CBuildCombo::EnableItem(int iItem, BOOL fEnabled)
{
	SetItemData(iItem, fEnabled ? 1 : 0);
	if (iItem == GetCurSel())
	  	// redraw this selection
		Invalidate(FALSE);
}

void CBuildCombo::PostNcDestroy()
{
	delete this;
}


//
// CTarget Combos
//

CObList CTargetCombo::s_listCombos;

CTargetCombo::CTargetCombo()
{
	s_listCombos.AddHead(this);
}

CTargetCombo::~CTargetCombo()
{
	CObject * pObj;

	POSITION pos = s_listCombos.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = s_listCombos.GetAt(pos);
		if (pObj == this)
		{
			s_listCombos.RemoveAt(pos);
			break;
		}
		s_listCombos.GetNext(pos);
	}
}

void CTargetCombo::UpdateView()
{
 	CProject * pProject = g_pActiveProject;
	ASSERT(pProject);
	ConfigurationRecord * pcr = pProject->GetActiveConfig();

	CString strName;
	pcr->GetProjectName(strName);

	int index = FindStringExact(-1, strName );
	if (index != CB_ERR && index != GetCurSel())
		SetCurSel(index);
}

void CTargetCombo::RefreshTargets()
{
	CProject * pProject = g_pActiveProject;
	SetRedraw(FALSE);

	EnableWindow(pProject != (CProject *)NULL);
	if (pProject == (CProject *)NULL || !IsWindowEnabled())
	{
		SetRedraw(TRUE);
		return;	// done!
	}

	// fill in target list box
	// enumerate all possible configuration and put them in the listbox
	g_theProjectEnum.FillComboBox(this /* Target List Box */, TRUE /* ResetContent*/);

/*	CString strProjectName;
 *	VERIFY(pProject->GetStrProp(P_ProjItemName, strProjectName));
 */

	ConfigurationRecord * pcr = pProject->GetActiveConfig();
	CString strProjectName;
	pcr->GetProjectName(strProjectName);

	int index = FindStringExact(-1, (const TCHAR *)strProjectName);
	if (index != CB_ERR)
		SetCurSel(index);

	SetRedraw(TRUE);
}

BOOL CTargetCombo::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * lResult)
{
	BOOL bRet = FALSE;

	if (message == WM_COMMAND)
	{
		if (HIWORD(wParam) == CBN_SELENDOK)
		{
			int nIndex = GetCurSel();
			ASSERT(nIndex != CB_ERR);

			CString strTarget;
			GetLBText(nIndex, strTarget);

			/*
			HBLDTARGET hTarget = g_BldSysIFace.GetTarget(strTarget, NO_BUILDER);
			ASSERT(hTarget);
			HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);
			ASSERT(hBld != NO_BUILDER);
			g_BldSysIFace.SetActiveTarget(hTarget, hBld);
			*/
			HBUILDER hBld = g_BldSysIFace.GetBuilderFromName(strTarget);
			g_BldSysIFace.SetActiveBuilder(hBld);


			CObList * plstCombos = GetComboList();
			POSITION pos = plstCombos->GetHeadPosition();
			while (pos != NULL)
			{
				CTargetCombo * pCombo = (CTargetCombo *)plstCombos->GetNext(pos);
				if( pCombo != this )
					pCombo->UpdateView();
			}

			bRet = TRUE;		
		}
		else if (HIWORD(wParam) == CBN_DROPDOWN)
		{
			SizeComboToContent(this);

			bRet = TRUE;
		}
	}

	if (CDockCombo::OnChildNotify(message, wParam, lParam, lResult))
		bRet = TRUE;

	return bRet;
}


//
// CConfigCombo
//

CObList CConfigCombo::s_listCombos;

CConfigCombo::CConfigCombo()
{
	s_listCombos.AddHead(this);
}

CConfigCombo::~CConfigCombo()
{
	CObject * pObj;

	POSITION pos = s_listCombos.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = s_listCombos.GetAt(pos);
		if (pObj == this)
		{
			s_listCombos.RemoveAt(pos);
			break;
		}
		s_listCombos.GetNext(pos);
	}
}

void CConfigCombo::UpdateView()
{
 	CProject * pProject = g_pActiveProject;
	ASSERT(pProject);
	ConfigurationRecord * pcr = pProject->GetActiveConfig();
	CString strConfig;
	pcr->GetConfigurationDescription(strConfig);
	int index = FindStringExact(-1, (const TCHAR *)strConfig);
	if (index != CB_ERR && index != GetCurSel())
		SetCurSel(index);
}

void CConfigCombo::RefreshTargets()
{
	CProject * pProject = g_pActiveProject;
	SetRedraw(FALSE);
	ResetContent();
	EnableWindow(pProject != (CProject *)NULL);
	if (pProject == (CProject *)NULL || !IsWindowEnabled())
	{
		SetRedraw(TRUE);
		return;	// done!
	}

	// fill in target list box
	// enumerate all possible configuration and put them in the listbox
	pProject->m_ConfigEnum.FillComboBox(this /* Target List Box */, TRUE /* ResetContent*/);

	ConfigurationRecord * pcr = pProject->GetActiveConfig();
	if (pcr != NULL)
	{
		CString strConfig;
		pcr->GetConfigurationDescription(strConfig);
		int index = FindStringExact(-1, (const TCHAR *)strConfig);
		if (index != CB_ERR)
			SetCurSel(index);
	}
	SetRedraw(TRUE);
}

BOOL CConfigCombo::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * lResult)
{
	BOOL bRet = FALSE;

	if (message == WM_COMMAND)
	{
		if (HIWORD(wParam) == CBN_SELENDOK)
		{
			int nIndex = GetCurSel();
			ASSERT(nIndex != CB_ERR);
			CString strTarget;
			GetLBText(nIndex, strTarget);

			CString strProject;
			g_pActiveProject->GetActiveConfig()->GetProjectName(strProject);

			strTarget = strProject + " - " + strTarget;

			HBLDTARGET hTarget = g_BldSysIFace.GetTarget(strTarget, NO_BUILDER);
//			ASSERT(hTarget);

			/* do we need this ? */
			HBUILDER hBld = g_BldSysIFace.GetBuilder(hTarget);
//			ASSERT(hBld != NO_BUILDER);
			if( hBld == NO_BUILDER ){
				// put all the combos back as they were.
				CObList * plstCombos = GetComboList();
				POSITION pos = plstCombos->GetHeadPosition();
				while (pos != NULL)
				{
					CConfigCombo * pCombo = (CConfigCombo *)plstCombos->GetNext(pos);
					pCombo->UpdateView();
				}

				return bRet;
			}

			g_BldSysIFace.SetActiveTarget(hTarget, hBld);

			CObList * plstCombos = GetComboList();
			POSITION pos = plstCombos->GetHeadPosition();
			while (pos != NULL)
			{
				CConfigCombo * pCombo = (CConfigCombo *)plstCombos->GetNext(pos);
				if( pCombo != this )
					pCombo->UpdateView();
			}

			bRet = TRUE;		
		}
		else if (HIWORD(wParam) == CBN_DROPDOWN)
		{
			SizeComboToContent(this);

			bRet = TRUE;
		}
	}

	if (CDockCombo::OnChildNotify(message, wParam, lParam, lResult))
		bRet = TRUE;

	return bRet;
}
