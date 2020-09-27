#include "stdafx.h"

#include <multinfo.h>
#include <proppage.h>
#include "resource.h"
#include "allpage2.h"

#include "utilauto.h"
#include <ocdesign.h>
#include <shldocs_.h>

#include <urlpdefs.h>

// IID_IForm needed for ISBug: 7249 - defined in forms3.h which wont compile
// in the devstudio tree
// {04598fc8-866c-11cf-ab7c-00aa00c08fcf}
DEFINE_GUID(IID_IForm, 
0x04598fc8, 0x866c, 0x11cf, 0xab, 0x7c, 0x0, 0xaa, 0x0, 0xc0, 0x8f, 0xcf);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// these two are taken from uuid.lib
EXTERN_C const GUID GUID_PathProperty;
EXTERN_C const GUID GUID_HasPathProperties;

// static data (begins with s_)
static LCID		s_lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
static CString	s_strTrue;
static CString	s_strFalse;
static UINT		s_uiLeadByte;

/////////////////////////////////////////////////////////////////////////////
// C3dFontDialog - FontDialog which hides the Color Check Box

BEGIN_MESSAGE_MAP(C3dFontDialog, CFontDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


C3dFontDialog::C3dFontDialog(
	LPLOGFONT	lplfInitial, 
	DWORD		dwFlags,
	CDC*		pdcPrinter, 
	CWnd*		pParentWnd) : 
CFontDialog( lplfInitial, (dwFlags & CF_ENABLETEMPLATE) ? 
	(dwFlags & ~CF_ENABLETEMPLATE) | CF_ENABLETEMPLATEHANDLE : dwFlags,
	pdcPrinter, pParentWnd )
{
}

BOOL C3dFontDialog::OnInitDialog()
{
	BOOL bRet = CFontDialog::OnInitDialog();

	CenterWindow();
	DWORD dwExStyle = (DWORD)GetWindowLong(m_hWnd, GWL_EXSTYLE);
	dwExStyle &= ~WS_EX_CONTEXTHELP;
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwExStyle);

	return bRet;
}

HBRUSH C3dFontDialog::OnCtlColor(
	CDC*	pDC,
	CWnd*	pWnd,
	UINT	nCtlColor)
{
	HBRUSH hbrush = CFontDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbrush;
}

int C3dFontDialog::DoModal()
{
	C3dDialogTemplate	dt;
	//PreModalWindow();
	if (m_cf.Flags & CF_ENABLETEMPLATEHANDLE)
	{
		VERIFY( dt.Load( m_cf.lpTemplateName ) );
		SetStdFont(dt);
		m_cf.hInstance = (HINSTANCE) dt.GetTemplate();
	}
	int		iRet = CFontDialog::DoModal();
	//PostModalWindow();

	return iRet;
}

// ================= static functions ====================================
// Some static functions that are placed here so that when they are used
// repeatedly, it would make sense to have the code in one place.
// Most of these are very small functions and are self-explanatory

static CString FormatEnumIntPropertyValue(long lCurrentValue, BSTR bstrValueDescription)
{
	CString strResult;

	if ((bstrValueDescription == NULL) || (bstrValueDescription[0] == L'\0'))
	{
		LPSTR pstrTemp = strResult.GetBuffer(30); // Should always be enough for a formatted long
		sprintf(pstrTemp, "%d", lCurrentValue);
		strResult.ReleaseBuffer();
	}
	else
	{
		LPSTR pstrTemp = strResult.GetBuffer(wcslen(bstrValueDescription) + 64);
		wsprintf(pstrTemp, _T("%d - %ws"), lCurrentValue, bstrValueDescription);
		strResult.ReleaseBuffer();
	}

	return strResult;
}

static int GetDefaultItemHeight(
	CWnd*	pWnd)
{
	int			iHeight = ALLPAGE_HEADER_HEIGHT;
	CDC*		pDC = pWnd->GetDC();
	TEXTMETRIC	tm;

	if (pDC)
	{
		pDC->GetTextMetrics(&tm);
		iHeight = tm.tmHeight + tm.tmExternalLeading + tm.tmInternalLeading;
		pWnd->ReleaseDC(pDC);
	}

	return iHeight;
}

static void LBAddStringAndSetItemData(
	CListBox*	pListBox,
	LPCTSTR		pszcString,
	DWORD		dwItemData)
{
	int		iIndex;

	iIndex = pListBox->AddString(pszcString);
	ASSERT(iIndex != LB_ERR);
	if (iIndex != LB_ERR)
		pListBox->SetItemData(iIndex, dwItemData);
}

static inline BOOL DoesControlTypeNeedAButton(
	CPropItem*	pItem)
{
	return (pItem &&
		((pItem->m_ctrlType == ctrl_ComboLimitToList) ||
		(pItem->m_ctrlType == ctrl_ComboNotLimitToList) ||
		(pItem->m_ctrlType == ctrl_BuilderPerPropertyBrowsing) ||
		(pItem->m_ctrlType == ctrl_BuilderProvidePropertyBuilder) ||
		(pItem->m_ctrlType == ctrl_BuilderWizardManager) ||
		(pItem->m_ctrlType == ctrl_BuilderWizardManagerIntrinsic) ||
		(pItem->m_ctrlType == ctrl_BuilderInternal)));
}

static inline BOOL IsControlTypeAComboBox(
	CPropItem*	pItem)
{
	return (pItem &&
		((pItem->m_ctrlType == ctrl_ComboLimitToList) ||
		(pItem->m_ctrlType == ctrl_ComboNotLimitToList)));
}

static inline BOOL GetPerPropertyBrowsing(
	IDispatch*				pDisp,
	IPerPropertyBrowsing**	ppPPB)
{

	return (SUCCEEDED(pDisp->QueryInterface(IID_IPerPropertyBrowsing,
		(void**)ppPPB)));
}

static inline long GetLongFromVariant(
	VARIANT*	pVarValue)
{
	return (V_VT(pVarValue) == VT_I4) ?	V_I4(pVarValue) : (long)(short)V_I2(pVarValue);
}

static inline BOOL IsEnumDataType(
	CPropItem*	pItem)
{
	return (pItem && ((pItem->m_dataType == data_EnumInt2) ||
		(pItem->m_dataType == data_EnumInt4) ||
		(pItem->m_dataType == data_EnumString)));
}

// ISBUG: 7249 !!!!!!!!!!!!!!!!!!!!!!!
// THIS CODE IS ASSOCIATED WITH THE CODE IN PROPPAGE.CPP (in LoadPages()).
// This function is provided to fix a bug in ISCTRLS.OCX.  The Stock Font,
// Color and Picture property pages provided by MFC does not work because,
// the ITypeInfos are not merged in ISCTRLS.OCX and so, the enumeration of
// properties does not work with these property pages, but they do work in the
// MS Stock Propperty Pages.    So we remove these property pages and let the user
// use the AllPage to edit those properties.
static inline HRESULT MapPropertyToPage(IPerPropertyBrowsing* pPPB, DISPID dispid, CLSID* pclsid)
{
	HRESULT hr = pPPB->MapPropertyToPage(dispid, pclsid);
	if(SUCCEEDED(hr))
	{
		// Map MFC Stock Prop Page to MS Stock Prop Page if in Layout/ObjEd Doc
		CFrameWnd*	pFrame = ((CFrameWnd*)AfxGetMainWnd())->GetActiveFrame();
		ASSERT(pFrame);
		if(pFrame)
		{
			CDocument*	pDoc = pFrame->GetActiveDocument();
			if(pDoc != NULL && pDoc->IsKindOf(RUNTIME_CLASS(CIPCompDoc)))
			{
				// Only the Layout & Object Editor docs contain forms96==IID_IForm.
				// QI for IForm.  If successful then we know we have a Layout ||
				// Object Editor Doc, since IID_IForm is Forms96 only.
				LPOLEOBJECT pOleObj = ((CIPCompDoc*)pDoc)->GetOleObject(); // Un-AddRefed
				if(pOleObj != NULL)
				{
					COleRef<IUnknown> srpForms96;
					if(SUCCEEDED(pOleObj->QueryInterface(IID_IForm, (void**)&srpForms96)))
					{
						if(IsEqualCLSID(*pclsid, CLSID_CFontPropPage) ||
							IsEqualCLSID(*pclsid, CLSID_CColorPropPage) ||
							IsEqualCLSID(*pclsid, CLSID_CPicturePropPage))
							hr = E_NOTIMPL;
					}
				}
			}
		}
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CPropItem

ULONG CPropItem::s_ulCountControls = 0;	// number of controls selected

CPropItem::CPropItem(
	LPCTSTR		pszcText,	// property name
	VARTYPE		vt,			// variant type
	DataType	dt,			// data type
	DISPID		dispid)		// dispid of the first control selected
{
	m_strText	= pszcText;
	m_ctrlType	= ctrl_Unknown;
	m_dataType	= dt;
	m_vt		= vt;
	m_fEdit		= FALSE;
	m_pEnumData	= NULL;
	m_iIndex	= -1;

	m_Dispid	= dispid;
	m_pDispid	= &m_Dispid;
	m_pvd		= NULL;
	m_pfd		= NULL;
	m_TypeInfo	= NULL;

	ZeroMemory(&unionValue, sizeof(unionValue));
	m_strProp.Empty();
}

// This methos deletes the enumdata, releases any interfaces
// if they are cached and also releases the vardesc and funcdesc pointers.
// Finally, if multiple controls were selected, the m_pDispid array is
// also deleted.
CPropItem::~CPropItem()
{
	if (m_pEnumData)
		delete m_pEnumData;

	int			i;
	int			iCount = m_TypeInfoArrayEnumInt.GetSize();
	ITypeInfo*	pTypeInfo;

	// release the ITypeInfo interfaces for enum int 2 and 4
	if (iCount > 0)
	{
		for (i = 0; i < iCount; ++i)
		{
			pTypeInfo = (ITypeInfo*)m_TypeInfoArrayEnumInt.GetAt(i);
			ASSERT(pTypeInfo);
			pTypeInfo->Release();
		}

		m_TypeInfoArrayEnumInt.RemoveAll();
	}

	// release the ITypeInfo interface (of the first control selected)
	if (m_TypeInfo)
	{
		if (m_pfd)
			m_TypeInfo->ReleaseFuncDesc(m_pfd);
		if (m_pvd)
			m_TypeInfo->ReleaseVarDesc(m_pvd);
		m_TypeInfo->Release();
		m_TypeInfo = NULL;
	}

	// if multiple controls were selected, delete the memory for dispid array
	if ((m_pDispid != &m_Dispid) && (s_ulCountControls > 1))
		delete [] m_pDispid;
}

// This method deletes the existing enum-data if any and allocates memory for
// the new enumdata.
void CPropItem::GetNewEnumData()
{
	// delete any old data if present
	if (m_pEnumData)
	{
		delete m_pEnumData;
		m_pEnumData = NULL;
	}

	m_pEnumData = new CEnumData;
}

// This method stores the DISPID when Multiple controls are selected.  For the
// first control, the m_Dispid member is used and no memory is allocated for the
// m_pDispid.  If multiple-controls are selected, then this method allocates
// memory for an array of dispids and then stores the given dispid at the
// appropriate index supplied by the caller (COleAllPage::IntersectToMap).
void CPropItem::SetDispid(
	ULONG	ulIndex,
	DISPID	dispid)
{
	ASSERT((ulIndex != 0) && (s_ulCountControls > 1) && (ulIndex <= s_ulCountControls));

	// if we have not allocated memory for the array, m_pDispid will be
	// pointing to &m_Dispid
	if (m_pDispid == &m_Dispid)
	{
		m_pDispid = new DISPID[s_ulCountControls];
		m_pDispid[0] = m_Dispid;
	}

	m_pDispid[ulIndex] = dispid;
}

/////////////////////////////////////////////////////////////////////////////
// CPropEdit

CPropEdit::CPropEdit()
{
	m_pItem	= NULL;
	m_pLb	= NULL;
	m_pBtn	= NULL;
}

// Handles the OnKeyDown notification message.
// The up/down arrow keys are passed on to the parent list box.
// When Delete Key is pressed for a picture property, the picture is
// deleted from the property.
// Again, the up/down arrow keys are used to move from one property to
// the next in the main property list box (the edit control's parent) and
// the left/right arrow key is used to select the previous/next item in
// the drop-down box if that is used to edit the currently selected property.
void CPropEdit::OnKeyDown(
	UINT	nChar,
	UINT	nRepCnt,
	UINT	nFlags)
{
	switch(nChar)
	{
		case VK_UP:		// pass these on to the parent list box
		case VK_DOWN:
		{
			CPropListBox*	pPropListBox;
			pPropListBox = (CPropListBox*)GetParent();
			ASSERT(pPropListBox);
			pPropListBox->SetFocus();
			pPropListBox->OnKeyDown(nChar, nRepCnt, nFlags);
		}
			break;
		case VK_LEFT:
			if (m_pLb)
			{
				m_pLb->SelectPrev();	// select previous item in list
				return;
			}
			break;
		case VK_RIGHT:
			if (m_pLb)
			{
				m_pLb->SelectNext();	// select next item in list
				return;
			}
			break;

		default:
		case VK_DELETE:
			if (m_pItem)
			{
				if (m_pItem->m_dataType == data_Font)
				{
					return;	// don't allow any typing for the font property
				}
				else if ((m_pItem->m_ctrlType == ctrl_ComboLimitToList) &&
					(nChar == VK_DELETE))
				{
					return;	// don't allow deleting of the combobox property
				}
				else if (m_pItem->m_dataType == data_Picture)
				{
					if (nChar == VK_DELETE)
					{
						// special processing for just the picture property
						// to set it to nothing when Del key is pressed.
						CPropListBox*	pPropListBox;
						pPropListBox = (CPropListBox*)GetParent();
						ASSERT(pPropListBox);
						if (pPropListBox->m_pPage)
							((COleAllPage*)pPropListBox->m_pPage)->SetPictureToNone(m_pItem);
					}
					return;
				}
			}
			break;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

// Handle OnChar notification message.  Prevents the user from typing
// anything if the property is either a font property or a picture property
void CPropEdit::OnChar(
	UINT	nChar,
	UINT	nRepCnt,
	UINT	nFlags) 
{
	if (m_pItem && 
		nChar!=3 /*^C*/ && //copy
		nChar!=24 /*^X*/ && //cut
		nChar !=22 /*^V*/) //paste
	{
		if (m_pLb && m_pItem->m_ctrlType == ctrl_ComboLimitToList)
		{
			// if we get a lead-byte character, just store it and return
			if (_istleadbyte(nChar))
			{
				s_uiLeadByte = nChar;
			}
			else
			{
				CString	strItem;
				LPTSTR	pszItem = strItem.GetBuffer(sizeof(TCHAR) * 3);

				// if we have stored a lead-byte, append the current character
				// to it and send this as a double-byte character to the listbox
				if (s_uiLeadByte)
				{
					*pszItem = (TCHAR)s_uiLeadByte;
					pszItem = _tcsinc(pszItem);

					s_uiLeadByte = NULL;
				}

				*pszItem = (TCHAR)nChar;
				pszItem = _tcsinc(pszItem);

				*pszItem = TCHAR('\0');
				strItem.ReleaseBuffer();

				// depending upon what character was typed, try to select
				// an item from the list
				m_pLb->SelectLimitToList(strItem);
			}

			return;
		}
		else if ((m_pItem->m_dataType == data_Font) ||
			(m_pItem->m_dataType == data_Picture))
		{
			// do not allow the user to type into the edit control for these
			// 2 data types
			return;
		}
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

UINT CPropEdit::OnGetDlgCode() 
{
	UINT nCodes = CEdit::OnGetDlgCode();
	return (nCodes | DLGC_WANTTAB);
}

// This method saves the property if it is modified
BOOL CPropEdit::SaveProperty()
{
	BOOL	bForceVisible = FALSE;

	if (m_pItem == NULL)
		return bForceVisible;

	BOOL	bModify = GetModify();

	if (bModify)
	{
		GetWindowText(m_pItem->m_strProp);
	}
	else
	{
		CString	strEdit;

		// IStudio  bug # 6021 & 5710
		// this is another HACK to make this work in IME modes
		// sometimes, the Modify flag is not set properly and we are
		// just trying to make sure that the text in the edit control
		// and the buffer are the same, if not, we set the modify flag
		// and save the property
		GetWindowText(strEdit);
		if (0 != m_pItem->m_strProp.Compare(strEdit))
		{
			bModify = TRUE;
			SetModify(bModify);
			m_pItem->m_strProp = strEdit;
		}
	}

	// if we have new text in the edit control, then update the property
	if (bModify)
	{
		CPropListBox*	pPropListBox = (CPropListBox*)GetParent();

		ASSERT(pPropListBox);
		bForceVisible = !(pPropListBox->UpdateProperty());
	}

	return bForceVisible;
}

// Saves the property if it is modified upon killfocus
void CPropEdit::OnKillFocus(
	CWnd*	pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	if ((m_pLb && pNewWnd == m_pLb) || (m_pBtn && pNewWnd == m_pBtn))
		return;

	BOOL	bForceVisible = SaveProperty();

	if (!bForceVisible)
		ShowWindow(SW_HIDE);

	if (m_pLb)
		m_pLb->SetWindowPos(&wndBottom, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
}

// To achieve VB-like behavior, when the user double-clicks a row, and if
// that row has a drop-down box to edit its property, this "cycles" through
// the selection and selects the next item in the list.  If the last item
// is selected, then this selects the first item (cycle).
// If the property does not have a drop-down listbox to edit the property,
// the double-click is just passed to the default class (edit).
void CPropEdit::OnLButtonDblClk(
	UINT	nFlags,
	CPoint	point)
{
	// selectnext item in the list (cycle if necessary)
	if (m_pLb && IsControlTypeAComboBox(m_pItem))
		m_pLb->SelectNextItemInList();
	else
		CEdit::OnLButtonDblClk(nFlags, point);
}

BEGIN_MESSAGE_MAP(CPropEdit, CEdit)
	//{{AFX_MSG_MAP(CPropEdit)
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropButton

CPropButton::CPropButton()
{
	m_lBtnWidth	= 0;
	m_lBtnHeight= 0;
	m_pItem		= NULL;
}

void CPropButton::SetBtnDimensions()
{
	if (!m_lBtnWidth && !m_lBtnHeight)
	{
		int		iHeight = GetDefaultItemHeight(this);

		if (iHeight > 0)
		{
			m_lBtnHeight = iHeight;
			m_lBtnWidth = m_lBtnHeight - ALLPAGE_GUTTER;
		}
	}
}

// Draws either a "drop-down" button or a "ellipsis" (...) button
// Uses the standard OBM_COMBO system bitmap for the former button
// and 3 small rectangles to achieve the ellipsis button for the
// latter.
void CPropButton::DrawItem(
	LPDRAWITEMSTRUCT	lpDIS) 
{
	if (!DoesControlTypeNeedAButton(m_pItem))
		return;

	CDC*		pDC = CDC::FromHandle(lpDIS->hDC);
	CRect		rcItem(lpDIS->rcItem);
	BOOL		bPushed = lpDIS->itemState & ODS_SELECTED;

	COLORREF	colorLt = ::GetSysColor(COLOR_3DHILIGHT);
	COLORREF	colorDk = ::GetSysColor(COLOR_3DDKSHADOW);

	// draw the outside rectangle
	if (bPushed)
		pDC->Draw3dRect(rcItem, colorDk, colorLt);
	else
		pDC->Draw3dRect(rcItem, colorLt, colorDk);

	rcItem.InflateRect(-1, -1);
	colorLt = ::GetSysColor(COLOR_3DLIGHT);
	colorDk = ::GetSysColor(COLOR_3DSHADOW);
	// draw the inside rectangle
	if (bPushed)
		pDC->Draw3dRect(rcItem, colorDk, colorLt);
	else
		pDC->Draw3dRect(rcItem, colorLt, colorDk);

	rcItem.InflateRect(-1, -1);
	pDC->FillSolidRect(rcItem, colorLt);	// fill the rectangle

	if (bPushed)
		rcItem.OffsetRect(1, 1);

	CPen	penBtnTxt(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
	CPen*	ppenOld = pDC->SelectObject(&penBtnTxt);

	if (IsControlTypeAComboBox(m_pItem))
	{
		CDC			bmpDC;
		CBitmap		bmp;
		CBitmap*	pbmpOld;
		BITMAP		bmpInfo;
		int			iShrinkW;
		int			iShrinkH;

		// load the combo bitmap from the system 
		bmp.LoadOEMBitmap(OBM_COMBO);
		bmp.GetBitmap(&bmpInfo);

		// center it on the rcItem rect
		iShrinkW = ((rcItem.Width() - bmpInfo.bmWidth) / 2);
		iShrinkH = ((rcItem.Height() - bmpInfo.bmHeight) / 2);
		rcItem.DeflateRect(iShrinkW, iShrinkH);

		// draw the bitmap
		bmpDC.CreateCompatibleDC(pDC);
		pbmpOld = bmpDC.SelectObject(&bmp);
		pDC->BitBlt(rcItem.left, rcItem.top, rcItem.right - rcItem.left,
			rcItem.bottom - rcItem.top, &bmpDC, 0, 0, SRCCOPY);
		bmpDC.SelectObject(pbmpOld);

		// free the gdi objects
		bmpDC.DeleteDC();
		bmp.DeleteObject();
	}
	else	// all the builders
	{
		CRect	rcDotMiddle;
		CRect	rcDotLeftRight;

		// draw the 3 dots (rectangles)
		// since all are on the same top and bottom plane, change only left, right
		// draw the middle dot
		rcDotMiddle.right = rcItem.left + (rcItem.Width() / 2) + 1;
		rcDotMiddle.left = rcDotMiddle.right - 2;
		rcDotMiddle.bottom = rcItem.bottom - (rcItem.Height() / 4) + 1;
		rcDotMiddle.top = rcDotMiddle.bottom - 2;
		colorDk = ::GetSysColor(COLOR_3DDKSHADOW);
		pDC->FillSolidRect(&rcDotMiddle, colorDk);

		rcDotLeftRight = rcDotMiddle;

		// draw the left dot
		rcDotLeftRight.right = rcDotMiddle.left - 2;
		rcDotLeftRight.left = rcDotLeftRight.right - 2;
		pDC->FillSolidRect(&rcDotLeftRight, colorDk);

		// draw the right dot
		rcDotLeftRight.left = rcDotMiddle.right + 2;
		rcDotLeftRight.right = rcDotLeftRight.left + 2;
		pDC->FillSolidRect(&rcDotLeftRight, colorDk);
	}

	pDC->SelectObject(ppenOld);
}

void CPropButton::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	SetBtnDimensions();
	lpMeasureItemStruct->itemWidth  = m_lBtnWidth;
	lpMeasureItemStruct->itemHeight = m_lBtnHeight;
}

BEGIN_MESSAGE_MAP(CPropButton, CButton)
	//{{AFX_MSG_MAP(CPropButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropList

CPropList::CPropList()
{
	m_pTxt = NULL;
	m_pPropListBox = NULL;
	m_iItemHeight = LB_ERR;
}

BEGIN_MESSAGE_MAP(CPropList, CListBox)
	//{{AFX_MSG_MAP(CPropList)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Populates the drop-down list box (this list box) with items
void CPropList::FillContents(
	CPropItem*	pItem)
{
	int		iCount;
	int		i;
	int		iIndex;

	ASSERT(pItem);
	ResetContent();
	switch(pItem->m_dataType)
	{
		case data_Bool:
			LBAddStringAndSetItemData(this, s_strTrue, TRUE);
			LBAddStringAndSetItemData(this, s_strFalse, FALSE);
			iCount = GetCount();
			for (i = 0; i < iCount; ++i)
			{
				if (pItem->unionValue.m_nValue == (int)GetItemData(i))
				{
					SetCurSel(i);
					i = iCount;	// break out of the loop
				}
			}
		break;

		case data_EnumInt2:
		case data_EnumInt4:
			ASSERT(pItem->m_pEnumData);
			iCount = pItem->m_pEnumData->m_straEnumStrings.GetSize();
			// just make sure that both the arrays are of same size
			ASSERT(iCount == pItem->m_pEnumData->m_dwaCookies.GetSize());
			for (i = 0; i < iCount; i++)
				LBAddStringAndSetItemData(this,
					pItem->m_pEnumData->m_straEnumStrings[i],
					pItem->m_pEnumData->m_dwaCookies[i]);

			iCount = GetCount();
			for (i = 0; i < iCount; i++)
			{
				if (pItem->unionValue.m_nValue == (int)GetItemData(i))
				{
					GetText(i, pItem->m_strProp);
					SetCurSel(i);
					i = iCount;	// break out of the loop
				}
			}
			break;

		case data_EnumString:
			if (pItem->m_pEnumData)
			{
				iCount = pItem->m_pEnumData->m_straEnumStrings.GetSize();
				// just make sure that both the arrays are of same size
				ASSERT(iCount == pItem->m_pEnumData->m_dwaCookies.GetSize());
				for (i = 0; i < iCount; i++)
					LBAddStringAndSetItemData(this,
						pItem->m_pEnumData->m_straEnumStrings[i],
						pItem->m_pEnumData->m_dwaCookies[i]);

				if (!pItem->m_strProp.IsEmpty())
				{
					i = FindStringExact(-1, pItem->m_strProp);
					if (LB_ERR != i)
						SetCurSel(i);
				}
			}
		break;
	}

	m_iItemHeight = GetItemHeight(0);
}

// This method positions the drop-down list box (this list box) below/above
// the edit control depending upon position of the edit control.
void CPropList::PositionToProperty(
	CRect*		pRect,
	CPropItem*	pItem)
{
	ASSERT(pRect && pItem);

	// calc the builder listbox size
	CRect	rcList;
	int		iHeightOfThisListBox;
	int		iScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	int		iYBorder = ::GetSystemMetrics(SM_CYBORDER);

	// should it go above or below the property item
	if (LB_ERR == m_iItemHeight)
		m_iItemHeight = pRect->Height() - 2;

	iHeightOfThisListBox = min((m_iItemHeight * GetCount()) + (2 * iYBorder),
		(iScreenHeight / 2));

	rcList.left	= pRect->left - 1;
	rcList.right = pRect->right - 1;
	if (iScreenHeight > (pRect->bottom + 1 + iHeightOfThisListBox))
	{
		rcList.top  = pRect->bottom + 1;
		rcList.bottom = iHeightOfThisListBox + rcList.top;
	}
	else
	{
		rcList.bottom = pRect->top - 1;
		rcList.top  = rcList.bottom - iHeightOfThisListBox;
	}

	MoveWindow(&rcList, FALSE);
}

// This method hides the drop-down list box (this list box) upon a selection
// change notification and then updates and saves the property change
void CPropList::OnSelchange() 
{
	// hide the drop-down list first
	SetWindowPos(&wndBottom, 0, 0, 0, 0,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
	// update the property change
	UpdateEditControl();
}

// This method updates the edit control with the current selection and
// also saves the change (updates the property)
void CPropList::UpdateEditControl()
{
	int		iIndex = GetCurSel();

	if (LB_ERR == iIndex)
		return;

	CString	str;

	GetText(iIndex, str);
	if (str.IsEmpty())
		return;
	
	ASSERT(m_pTxt);
	m_pTxt->SetWindowText(str);	// update edit control
	m_pTxt->m_pItem->m_strProp = str;	// update propitem data structure

	// if the data type happens to be of enumeration type,
	// store the dword value of enumeration in pItem->unionValue.m_dwValue
	// this will be used while setting the property
	if (m_pTxt->m_pItem->m_dataType == data_EnumInt2 ||
		m_pTxt->m_pItem->m_dataType == data_EnumInt4 ||
		m_pTxt->m_pItem->m_dataType == data_Bool)
		m_pTxt->m_pItem->unionValue.m_nValue = GetItemData(iIndex);
	else if (m_pTxt->m_pItem->m_dataType == data_EnumString)
		m_pTxt->m_pItem->unionValue.m_dwValue = GetItemData(iIndex);

	if (!IsControlTypeAComboBox(m_pTxt->m_pItem))
		m_pTxt->SetSel(0, -1);

	// get the parent window (main list box) and call updateproperty on it
	m_pTxt->SetModify(TRUE);	// set the modify flag
	ASSERT(m_pPropListBox);
	m_pPropListBox->UpdateProperty();// save the property
	m_pTxt->SetModify(FALSE);	// reset the modify flag

	m_pTxt->SetFocus();
}

// Selects the next item if one exists.
void CPropList::SelectNext()
{
	int		iIndex = GetCurSel();

	// if nothing is selected, this selects the first item
	if ((iIndex + 1) == GetCount())
		return;

	SetCurSel(++iIndex);
	UpdateEditControl();
}

// Selects the previous item if one exists.
void CPropList::SelectPrev()
{
	int		iIndex = GetCurSel();

	if (LB_ERR == iIndex || 0 == iIndex)
		return;

	SetCurSel(--iIndex);
	UpdateEditControl();
}

// This method selects the "next" item from the currently selected item
// that matches the given string.  This process cycles and selects from the
// beginning if the item is not found.
void CPropList::SelectLimitToList(
	LPCTSTR	pszcItem)
{
	int		iIndex = GetCurSel();
	int		iIndexNext = FindString(iIndex, pszcItem);

	// if we couldn't find an item starting with "nChar", then go back to
	// the previous item that was selected
	SetCurSel((LB_ERR != iIndexNext) ? iIndexNext : iIndex);
	UpdateEditControl();
}

// This method selects the "next" item in the list.  This also cycles and
// the first item if the currently selected item is the last item.
void CPropList::SelectNextItemInList()
{
	int		iIndex = GetCurSel();

	// if nothing is selected, this selects the first item
	if (++iIndex >= GetCount())
		iIndex = 0;

	SetCurSel(iIndex);
	UpdateEditControl();
}

/////////////////////////////////////////////////////////////////////////////
// CPropListBox

CPropListBox::CPropListBox()
{
	m_pTxt	= NULL;
	m_pBtn	= NULL;
	m_pLb	= NULL;
	m_pPage	= NULL;
	m_uiIndex = -1;
}

CPropListBox::~CPropListBox()
{
	delete m_pTxt;
	delete m_pBtn;
	delete m_pLb;
}

BEGIN_MESSAGE_MAP(CPropListBox, CListBox)
	//{{AFX_MSG_MAP(CPropListBox)
	ON_WM_SETFOCUS()
	ON_WM_GETDLGCODE()
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_WM_NCLBUTTONDOWN()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PROP_BUTTON, OnBtnClick)
END_MESSAGE_MAP()

void CPropListBox::OnDestroy()
{
	ResetContent();
	CListBox::OnDestroy();
}

// Basically draws the "grid", sets up the state for all the 3 child windows
// (edit, button and listbox).  Draw the selected row in hi-lite color.
// REVIEW: try to reduce the GDI objects (pen/brush) creation/deletion if possible
void CPropListBox::DrawItem(
	LPDRAWITEMSTRUCT lpDIS) 
{
	CPropItem*	pItem = (CPropItem *)(lpDIS->itemData);

	if (NULL == pItem)
		return;

	CDC*	pDC = CDC::FromHandle(lpDIS->hDC);
	CRect	rcItem(lpDIS->rcItem);
	CRect	rcTag(lpDIS->rcItem);
	CRect	rcProp(lpDIS->rcItem);
	int		iMidPoint;
	int		iColumn0Width;

	CWnd*	pWnd = GetParent();

	ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(COleAllPage)));
	iColumn0Width = ((COleAllPage*)pWnd)->GetColumnWidth(0);
	((COleAllPage*)pWnd)->SetHeaderInfo();	// to snap-back if necessary

	iMidPoint = rcItem.left + iColumn0Width - ALLPAGE_GUTTER;

	rcTag.right = rcTag.left + iMidPoint;
	rcProp.left = iMidPoint;
	if (lpDIS->itemState & ODS_SELECTED)
	{
		// output the tag text
		if (!pItem->m_strText.IsEmpty())
		{
			pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
			pDC->ExtTextOut(rcTag.left + ALLPAGE_GUTTER, rcTag.top,
				ETO_CLIPPED | ETO_OPAQUE, &rcTag, pItem->m_strText, NULL);
		}

		// Make sure that the text in the edit control is not newer than the
		// text in the CPropItem buffer and then update the text.
		// If not, leave the text alone.  Especially in the case of IME mode
		// where the first "Enter" key copies the text from the system buffer
		// into m_pTxt edit control, we get a repaint message and we do not
		// want to erase the new text with the old text.  (IS BuG# 6021)
		if (!m_pTxt->GetModify())
			m_pTxt->SetWindowText(pItem->m_strProp);

		m_uiIndex = lpDIS->itemID;	// store the index of item we are editing

		CRect	rcBtn = rcProp;

		if (!pItem->m_fEdit)
		{
			rcProp.left += ALLPAGE_GUTTER;
			rcProp.bottom -= 1;
			m_pLb->SetWindowPos(&wndBottom, 0, 0, 0, 0,
				SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
			switch (pItem->m_ctrlType)
			{
				case ctrl_Edit:						// just and edit
					rcProp.right -= 1;
					m_pTxt->MoveWindow(&rcProp, FALSE);

					// set the flag indicating that we have not modified the property yet
					m_pTxt->SetModify(FALSE);

					m_pTxt->m_pItem = pItem;
					m_pTxt->m_pLb = NULL;
					m_pTxt->m_pBtn = NULL;
					m_pBtn->m_pItem = NULL;
					m_pLb->m_pTxt = NULL;
					break;
				case ctrl_ComboLimitToList:			// edit and button
				case ctrl_ComboNotLimitToList:
					if (pItem->IsEnumDataDirty() && m_pPage)
						((COleAllPage*)m_pPage)->UpdateEnumData(pItem);

					m_pLb->FillContents(pItem);
					m_pTxt->m_pLb = m_pLb;
					m_pTxt->m_pBtn = m_pBtn;
					m_pLb->m_pTxt = m_pTxt;
					// OK to fall through
				case ctrl_BuilderPerPropertyBrowsing:
				case ctrl_BuilderProvidePropertyBuilder:
				case ctrl_BuilderWizardManager:
				case ctrl_BuilderWizardManagerIntrinsic:
				case ctrl_BuilderInternal:
					rcProp.right -= (m_pBtn->GetBtnWidth() + 1);
					rcBtn.left = rcBtn.right - m_pBtn->GetBtnWidth();
					rcBtn.bottom -= 1;
					m_pTxt->MoveWindow(&rcProp, FALSE);
					
					// set the flag indicating that we have not modified the property yet
					m_pTxt->SetModify(FALSE);

					if (!IsControlTypeAComboBox(pItem))
						m_pTxt->m_pLb = NULL;

					m_pTxt->m_pItem = pItem;
					m_pBtn->m_pItem = pItem;
					m_pBtn->MoveWindow(&rcBtn, FALSE);
					m_pBtn->ShowWindow(SW_SHOW);
					m_pBtn->InvalidateRect(NULL);
					break;
				default:
					ASSERT(FALSE);
			}

			pItem->m_fEdit = TRUE;
		}
		else
		{
			rcProp.left += ALLPAGE_GUTTER;
			rcProp.bottom -= 1;
			switch (pItem->m_ctrlType)
			{
				case ctrl_Edit:						// just and edit
					rcProp.right -= 1;
					m_pTxt->MoveWindow(&rcProp, FALSE);
					break;
				case ctrl_ComboLimitToList:			// edit and button
				case ctrl_ComboNotLimitToList:
					// OK to fall through
				case ctrl_BuilderPerPropertyBrowsing:
				case ctrl_BuilderProvidePropertyBuilder:
				case ctrl_BuilderWizardManager:
				case ctrl_BuilderWizardManagerIntrinsic:
				case ctrl_BuilderInternal:
					rcProp.right -= (m_pBtn->GetBtnWidth() + 1);
					m_pTxt->MoveWindow(&rcProp, FALSE);
					rcBtn.left = rcBtn.right - m_pBtn->GetBtnWidth();
					rcBtn.bottom -= 1;
					m_pBtn->MoveWindow(&rcBtn, FALSE);
					m_pBtn->ShowWindow(SW_SHOW);
					m_pBtn->InvalidateRect(NULL);
					break;
				default:
					ASSERT(FALSE);
			}
		}

		if (!m_pTxt->IsWindowVisible())
		{
			// draw the properties text
			CBrush	brush;

			brush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
			pDC->FillRect(&rcProp, &brush);
			brush.DeleteObject();
			if (!pItem->m_strProp.IsEmpty())
			{
				pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
				pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
				pDC->ExtTextOut(rcProp.left, rcProp.top,
					ETO_CLIPPED | ETO_OPAQUE, &rcProp, pItem->m_strProp, NULL);
			}
		}
	}
	else if (lpDIS->itemState & ODS_DISABLED)
	{
		// output the tag text
		pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->ExtTextOut(rcTag.left, rcTag.top, ETO_CLIPPED | ETO_OPAQUE, 
						&rcTag, pItem->m_strText, NULL);
		pItem->m_fEdit = FALSE;
	}
	else
	{
		// output the tag text
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->ExtTextOut(rcTag.left + ALLPAGE_GUTTER, rcTag.top,
			ETO_CLIPPED | ETO_OPAQUE, &rcTag, pItem->m_strText, NULL);

		if (pItem->m_fEdit)
		{
			m_pTxt->ShowWindow(SW_HIDE);
			m_pBtn->ShowWindow(SW_HIDE);
		}
		pItem->m_fEdit = FALSE;

		// draw the properties text
		CBrush brush;
		brush.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
		pDC->FillRect(&rcProp, &brush);
		brush.DeleteObject();
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->ExtTextOut(rcProp.left + ALLPAGE_GUTTER, rcProp.top,
			ETO_CLIPPED | ETO_OPAQUE, &rcProp, pItem->m_strProp, NULL);
	}
	
    // Create Grey Color Pen
    CPen	pen;
	pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_GRAYTEXT));
    CPen*	pPenOld = pDC->SelectObject(&pen);

	// draw the grid lines
	pDC->MoveTo(rcItem.left, rcItem.bottom - 1);
	pDC->LineTo(rcItem.right, rcItem.bottom - 1);
	pDC->MoveTo(iMidPoint, rcItem.bottom - 1);
	pDC->LineTo(iMidPoint, rcItem.top - 1);
	pDC->SelectObject(pPenOld);
	pen.DeleteObject();
}

// to achieve the sorting feature, we just compare the property names
// and the main property list is ordered by the property name.
int CPropListBox::CompareItem(
	LPCOMPAREITEMSTRUCT	lpCIS) 
{
	return _tcscmp(((CPropItem *)lpCIS->itemData1)->m_strText,
		((CPropItem *)lpCIS->itemData2)->m_strText);
}

void CPropListBox::MeasureItem(
	LPMEASUREITEMSTRUCT	lpMIS)
{
	lpMIS->itemHeight = GetDefaultItemHeight(this);
}

// This method creates all the 3 children (edit, button and listbox) that
// we need to achieve the "in-place" editing feature that the AllPage grid
// (main property listbox) provides.  Some sleight of hand is done here to
// achieve the drop-down listbox to function just-like a combobox drop-down.
// That is, this list box must be able to "drop" outside the parent window,
// and to do that this juggling is necessary.  Please read the comments below.
BOOL CPropListBox::CreateChildren()
{
	HWND	hWnd;
	CFont*	pFont = GetFont();

	m_pTxt	= new CPropEdit;
	m_pBtn  = new CPropButton;
	m_pLb	= new CPropList;

	hWnd = ::CreateWindow(_T("edit"), NULL,
		WS_CHILD | ES_LEFT | ES_AUTOHSCROLL | WS_CLIPSIBLINGS | WS_TABSTOP,
		0, 0, 0, 0, m_hWnd, (HMENU)IDC_PROP_EDIT, AfxGetInstanceHandle(), NULL);
	ASSERT(hWnd);
	m_pTxt->SubclassDlgItem(IDC_PROP_EDIT, this);
	m_pTxt->SetFont(pFont);

	hWnd = ::CreateWindow(_T("button"), NULL, WS_CHILD | BS_OWNERDRAW,
		0, 0, 0, 0, m_hWnd, (HMENU)IDC_PROP_BUTTON, AfxGetInstanceHandle(), NULL);
	ASSERT(hWnd);
	m_pBtn->SubclassDlgItem(IDC_PROP_BUTTON, this);
	m_pBtn->SetBtnDimensions();

	hWnd = ::CreateWindowEx(WS_EX_TOOLWINDOW, _T("listbox"), NULL,
		WS_CHILD | LBS_WANTKEYBOARDINPUT | WS_CLIPSIBLINGS | LBS_SORT |
		WS_VSCROLL | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
		0, 0, 0, 0, m_hWnd, (HMENU)IDC_PROP_LISTBOX, AfxGetInstanceHandle(), NULL);
	ASSERT(hWnd);
	m_pLb->SubclassDlgItem(IDC_PROP_LISTBOX, this);
	m_pLb->SetFont(pFont);
	m_pLb->SetParentListBoxPtr(this);

	// Convert the "drop-down" list box we just created into a pop-up window
	// so that it can extend outside the parent window.  We create this as
	// a child window initially so that the list box sends its notifications
	// to the parent "when it was created" and we want the parent to be
	// THIS main grid-listbox.
	// The ID must be set to zero because the control's ID
	// and the popup window's menu is stored in the same location.
	::SetWindowLong(hWnd, GWL_ID, 0L);
	::SetParent(hWnd, NULL);
	DWORD	dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
	::SetWindowLong(hWnd, GWL_STYLE, (dwStyle & ~WS_CHILD) | WS_POPUP);

	return TRUE;
}

UINT CPropListBox::OnGetDlgCode() 
{
	UINT nCodes = CListBox::OnGetDlgCode();
	return (nCodes | DLGC_WANTTAB);
}

// To handle the Tab order when the user uses the Tab key to traverse
// through the main property listbox, the edit control and the All property page
BOOL CPropListBox::PreTranslateMessage(
	MSG*	pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB)
	{
		if (this == GetFocus())
		{
			ShowEditControl();
			return TRUE;
		}
		return FALSE;
	}
	return CListBox::PreTranslateMessage(pMsg);
}

void CPropListBox::SetFocusAndCurSel()
{
	int		iIndex = GetCurSel();

	if (LB_ERR != iIndex)
	{
		m_pTxt->SetFocus();
		if (!IsControlTypeAComboBox((CPropItem*)GetItemData(iIndex)))
			m_pTxt->SetSel(0, -1);
	}
}

// Shows the edit control and displays selects the text
void CPropListBox::ShowEditControl()
{
	m_pTxt->ShowWindow(SW_SHOW);
	SetFocusAndCurSel();
}

// To achieve VB-like behavior, when the user double-clicks a row, and if
// that row has a drop-down box to edit its property, this "cycles" through
// the selection and selects the next item in the list.  If the last item
// is selected, then this selects the first item (cycle).
void CPropListBox::OnDblclk()
{
	int		iIndex = GetCurSel();

	if ((LB_ERR != iIndex) &&
		IsControlTypeAComboBox((CPropItem*)GetItemData(iIndex)))
		m_pLb->SelectNextItemInList();

	ShowEditControl();
}

// Every time the selection changes in the main property listbox, the
// edit control need to be shown in "that" row -- so a call to ShowWindow
// will do exactly that.
void CPropListBox::OnSelchange() 
{
	m_pTxt->ShowWindow(SW_SHOW);
}

// if the user starts typing and the focus in on the main property listbox
// this method passes on that character to the edit control if it is visible
void CPropListBox::OnChar(
	UINT	nChar,
	UINT	nRepCnt,
	UINT	nFlags) 
{
	if (m_pTxt->IsWindowVisible())
	{
		SetFocusAndCurSel();
		m_pTxt->OnChar(nChar, nRepCnt, nFlags);
		return;
	}

	CListBox::OnChar(nChar, nRepCnt, nFlags);
}

// Pass the right-left arrow keys to the edit control if it is visible
void CPropListBox::OnKeyDown(
	UINT	nChar,
	UINT	nRepCnt,
	UINT	nFlags) 
{
	switch (nChar)
	{
		case VK_LEFT:
		case VK_RIGHT:
			if (m_pTxt->IsWindowVisible())
			{
				SetFocusAndCurSel();
				m_pTxt->OnKeyDown(nChar, nRepCnt, nFlags);
				return;
			}
			// okay to fall through...
		case VK_UP:
		case VK_DOWN:
		case VK_HOME:
		case VK_END:
		case VK_PRIOR:	// page up
		case VK_NEXT:	// page down
		default:
			CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
	}
}

// When the builder (ellipsis) / drop-down (combo-box) is pressed,
// this method either invokes the builder / shows/hides the drop-down list box
void CPropListBox::OnBtnClick()
{
	switch(m_pBtn->m_pItem->m_ctrlType)
	{
		case ctrl_ComboLimitToList:
		case ctrl_ComboNotLimitToList:
			if (m_pLb->IsWindowVisible())
			{
				m_pLb->SetWindowPos(&wndBottom, 0, 0, 0, 0,
					SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
				SetFocus();
			}
			else
			{
				CRect	rcEdit;
				CRect	rcBtn;

				m_pTxt->GetWindowRect(rcEdit);
				m_pBtn->GetWindowRect(rcBtn);

				rcEdit.right = rcBtn.right;

				// bring the "drop-down" list box to the top
				m_pLb->PositionToProperty(&rcEdit, m_pBtn->m_pItem);
				m_pLb->SetWindowPos(&wndTopMost, 0, 0, 0, 0,
					SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			}
			break;

		case ctrl_BuilderPerPropertyBrowsing:
		case ctrl_BuilderProvidePropertyBuilder:
		case ctrl_BuilderWizardManager:
		case ctrl_BuilderWizardManagerIntrinsic:
		case ctrl_BuilderInternal:
			{
				CWnd*	pWnd = GetParent();

				ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(COleAllPage)));
				((COleAllPage*)pWnd)->ExecuteBuilder(m_pBtn->m_pItem);
			}
			break;

		default:
			ASSERT(FALSE);
			break;
	}
	
	ShowEditControl();
}

// if the non-client area gets a button down, hide the drop-down list box
void CPropListBox::OnNcLButtonDown(
	UINT	nHitTest,
	CPoint	point) 
{
	if (HTVSCROLL == nHitTest)
		SetFocus();
	
	m_pLb->SetWindowPos(&wndBottom, 0, 0, 0, 0,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
	CListBox::OnNcLButtonDown(nHitTest, point);
}

// This method is used to release all the cached data for that we have
// stored for every property (== every row in the grid)
void CPropListBox::ResetContent()
{
	// hide the edit, button and list controls first
	m_pTxt->ShowWindow(SW_HIDE);
	m_pBtn->ShowWindow(SW_HIDE);
	m_pLb->SetWindowPos(&wndBottom, 0, 0, 0, 0,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);

	// free data associated with the items
	m_pTxt->m_pItem = NULL;
	m_pTxt->m_pLb = NULL;
	m_pBtn->m_pItem = NULL;
	m_pLb->m_pTxt = NULL;

	int			iCount = GetCount();
	CPropItem*	pItem;

	for (int i = 0; i < iCount; ++i)
	{
		pItem = (CPropItem *)GetItemData(i);
		delete pItem;
	}

	CListBox::ResetContent();

	// set the number of items (controls) selected to 0
	// NOTE: THIS IS IMPORTANT, otherwise, caching of dispids will FAIL
	CPropItem::s_ulCountControls = 0;
}

// When the main property list box gets the focus, this method just
// makes sure that if the currently selected row needs a button (for the
// drop-down button or ellipsis) and makes it visible.
void CPropListBox::OnSetFocus(
	CWnd*	pOldWnd) 
{
	CListBox::OnSetFocus(pOldWnd);

	int		iIndex = GetCurSel();

	if (LB_ERR != iIndex)
	{
		if (m_pBtn &&
			DoesControlTypeNeedAButton((CPropItem*)GetItemData(iIndex)))
			m_pBtn->ShowWindow(SW_SHOW);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// COleAllPage

IMPLEMENT_DYNAMIC(COleAllPage, CSlobPage)

BEGIN_INTERFACE_MAP(COleAllPage, CSlobPage)
	INTERFACE_PART(COleAllPage, IID_IPropertyNotifySink, PropertyNotifySink)
END_INTERFACE_MAP()

BEGIN_IDE_CONTROL_MAP(COleAllPage, IDDP_ALLPROP_NEW, IDS_PROP_GENERAL) // was IDS_OC_ALL
	// All VB Styles Page controls are handled through the message map.
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(COleAllPage, CSlobPage)
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_NOTIFY(HDN_ENDTRACK, IDC_PROP_HEADER, OnEndTrackHeader)
	ON_NOTIFY(HDN_TRACK, IDC_PROP_HEADER, OnTrackHeader)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

COleAllPage::COleAllPage()
{
	m_strName.LoadString(IDS_PROP_ALL);

	m_lbProps.m_pPage	= this;
	m_ulCountControls	= 0;
	m_pSingleDisp		= NULL;
	m_pprgDisp			= NULL;
	m_bActive			= FALSE;
	m_bUpdatingProps	= FALSE;
	m_bUpdateLater		= FALSE;
	m_ihcHeight			= ALLPAGE_HEADER_HEIGHT;
	m_ihcCol0Width		= 0;
	m_ihcCol1Width		= 0;
	m_bSetHeader		= 0;
	m_ppTypeInfo		= NULL;
	m_pSingleTypeInfo	= NULL;
	m_ulTypeInfos		= 0;
}

COleAllPage::~COleAllPage()
{
	if (m_bActive)
		DestroyWindow();

	ReleaseInterfaces();		// free cached IDispatch interfaces
}

// This method releases all the cached interfaces and resets the member
// variables.
void COleAllPage::ReleaseInterfaces()
{
	FreeNotifySinkInfo();		// release all property notify sinks

	// release any cached interfaces
	for (ULONG i = 0; i < m_ulCountControls; ++i)
	{
		ASSERT(m_pprgDisp[i]);
		if(m_pprgDisp[i])
			m_pprgDisp[i]->Release();
	}

	// we've special cased the single selection case.  if we have more than
	// one disp, then we need to free up this heap
	if (m_ulCountControls > 1)
		AfxFreeTaskMem(m_pprgDisp);

	m_pprgDisp			= NULL;
	m_pSingleDisp		= NULL;
	m_ulCountControls	= 0;

	for (i = 0; i < m_ulTypeInfos; ++i)
	{
		ASSERT(m_ppTypeInfo[i]);
		if(m_ppTypeInfo[i])
			m_ppTypeInfo[i]->Release();
	}

	// we've special cased the single selection case.  if we have more than
	// one typeinfo, then we need to free up this heap
	if (m_ulTypeInfos > 1)
		AfxFreeTaskMem(m_ppTypeInfo);

	m_ppTypeInfo		= NULL;
	m_pSingleTypeInfo	= NULL;
	m_ulTypeInfos		= 0;
}

BOOL COleAllPage::PreTranslateMessage(MSG* pMsg)
{
#if 0
	// please do not remove this
	// Was here so that enter-key is pased on to edit control
	// so that it transfers the control to the grid-listbox
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (m_lbProps.m_pTxt == GetFocus())
		{
			// this causes the edit control to lose focus
			// and thus save the property and then give
			// focus to the list box
			m_lbProps.SetFocus();
			return TRUE;
		}
		return FALSE;
	}
#endif
	return CSlobPage::PreTranslateMessage(pMsg);
}

void COleAllPage::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	TRACE0("WM_COMMANDHELP is being eaten");
	return;
}

// Gets the required TypeInfo from the IProvideMultipleClassInfo interface
static HRESULT GetTypeInfoFromMultipleClassInfo(
	IDispatch*	pDisp,
	DISPID		dispid,
	ITypeInfo**	ppTypeInfo)
{
	ASSERT(pDisp && ppTypeInfo);
	*ppTypeInfo = NULL;
	
	HRESULT	hr;
	
	// see if the object provides multiple class infos
	COleRef<IProvideMultipleClassInfo>	srpPMCI;
	hr = pDisp->QueryInterface(IID_IProvideMultipleClassInfo, (void**)&srpPMCI);
	if (FAILED(hr))
		return hr;
	
	// Get count of Type Infos	
	ULONG	ulCountTypeInfos;
	hr = srpPMCI->GetMultiTypeInfoCount(&ulCountTypeInfos);
	if (FAILED(hr))
		ulCountTypeInfos = 1;

	// Variables used in for loop	
	COleRef<ITypeInfo>	srpTypeInfoStart;
	COleRef<ITypeInfo>	srpTypeInfo;
	LPTYPEATTR	ptaStart;
	LPTYPEATTR	pta;
	int			i;
	int			implTypeFlags;
	ULONG		ulCurTypeInfo;
	HREFTYPE	hrefType;
	
	for (ulCurTypeInfo = 0; ulCurTypeInfo < ulCountTypeInfos; ++ulCurTypeInfo)
	{
		pta = NULL;
		ptaStart = NULL;
		// since we are looping, make sure we release a
		// previously QI's interfaced
		srpTypeInfo.SRelease();
		srpTypeInfoStart.SRelease();

		// Get starting TypeInfo
		hr = srpPMCI->GetInfoOfIndex(ulCurTypeInfo,  MULTICLASSINFO_GETTYPEINFO,	
			&srpTypeInfoStart, NULL, NULL, NULL, NULL);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return hr;
			
		ASSERT(srpTypeInfoStart);
		VERIFY(srpTypeInfoStart->GetTypeAttr(&ptaStart) == S_OK);
		if (ptaStart->typekind == TKIND_DISPATCH ||
			ptaStart->typekind == TKIND_INTERFACE)
		{
			// since this is the correct typeinfo, get its interface pointer
			// and release the type attribute.
			srpTypeInfo = srpTypeInfoStart.Disown();
			pta = ptaStart;
		}
		else if (ptaStart->typekind == TKIND_COCLASS)
		{
			// go through and find out if we get a typekind of dispatch
			// if so, continue, otherwise, ignore this
			for (i = 0; i < ptaStart->cImplTypes; i++)
			{
				VERIFY(srpTypeInfoStart->GetImplTypeFlags(i, &implTypeFlags) == S_OK);
				VERIFY(srpTypeInfoStart->GetRefTypeOfImplType(i, &hrefType) == S_OK);
				VERIFY(srpTypeInfoStart->GetRefTypeInfo(hrefType, &srpTypeInfo) == S_OK);

				VERIFY(srpTypeInfo->GetTypeAttr(&pta) ==S_OK);
				if (pta->typekind != TKIND_DISPATCH)
				{
					srpTypeInfo->ReleaseTypeAttr(pta);
					srpTypeInfo.SRelease();
					pta = NULL;
					continue;
				}

				if (implTypeFlags & IMPLTYPEFLAG_FDEFAULT)
				{
					// Got our TypeInfo
					// release the start typeinfo and pta
					srpTypeInfoStart->ReleaseTypeAttr(ptaStart);
					break;
				}

				// release the type attribute since we do not need this
				srpTypeInfo->ReleaseTypeAttr(pta);
				srpTypeInfo.SRelease();
				pta = NULL;
			}
		}

		// new code above
		if (NULL == pta)
		{
			srpTypeInfoStart->ReleaseTypeAttr(ptaStart);
			continue;
		}
		
		// Finally have our TypeInfo
		ASSERT(srpTypeInfo);
		// GetDocumentation for DISPID
		hr = srpTypeInfo->GetDocumentation(dispid, NULL, NULL, NULL, NULL);
		if (SUCCEEDED(hr))
		{
			// Found a TypeInfo that contains the member
			*ppTypeInfo = srpTypeInfo.Disown();
			return hr;
		}
	}

	// didnt find TYPE info
	return E_FAIL;
}

// When the user presses the "F1" key or the "?" button, and if the AllPage
// is the active property page, this method is called.  This method tries
// to get the help file name and help context id from the ITypeInfo, or the
// ITypeLib.  If we get a help file name and not a help context id, the
// help finder dialog is displayed so that the user can select the topic
// himself/herself.
LRESULT COleAllPage::OnPageHelp(
	WPARAM	wParam,
	LPARAM	lParam)		// help context
{
	if (m_lbProps.m_uiIndex == -1)
		return E_UNEXPECTED;

	CPropItem*			pItem;

	pItem = (CPropItem*) m_lbProps.GetItemData(m_lbProps.m_uiIndex);
	if (NULL == pItem)
		return E_UNEXPECTED;

	COleRef<ITypeInfo>	srpTypeInfo;			// the type info we use to get props
	COleRef<ITypeLib>	srpTypeLib;

	UINT		uiIndex;
	BSTR		bstrHelpFile = NULL;
	ULONG		ulHelpContext = 0;
	DISPID		dispid;
	CString		strHelpFile;
	IDispatch*	pDisp;

	USES_CONVERSION;
	ASSERT(m_pprgDisp[0]);

	pDisp = m_pprgDisp[0];
	dispid = pItem->m_pDispid[0];
	// get the typeinfo
	if (SUCCEEDED(pDisp->GetTypeInfo(0, s_lcid, &srpTypeInfo)))
	{
		// try to get the help info from the typeinfo
		if (SUCCEEDED(srpTypeInfo->GetDocumentation(dispid, NULL, NULL,
			&ulHelpContext, &bstrHelpFile)))
			strHelpFile = OLE2T(bstrHelpFile);
		else
		{
			// Get another ITypeInfo from
			COleRef<ITypeInfo> srpOtherTypeInfo;
			if (SUCCEEDED(GetTypeInfoFromMultipleClassInfo(pDisp, 
				dispid, &srpOtherTypeInfo)))
			{
				ASSERT(srpOtherTypeInfo);
				if (SUCCEEDED(srpOtherTypeInfo->GetDocumentation(dispid,
						NULL, NULL, &ulHelpContext, &bstrHelpFile)))
					strHelpFile = OLE2T(bstrHelpFile);
			}
		}

		// if we did not get a help file, use the type library
		if (strHelpFile.IsEmpty() &&
			SUCCEEDED(srpTypeInfo->GetContainingTypeLib(&srpTypeLib, &uiIndex)) &&
			SUCCEEDED(srpTypeLib->GetDocumentation((int)uiIndex, NULL, NULL,
						&ulHelpContext, &bstrHelpFile)))
			strHelpFile = OLE2T(bstrHelpFile);

		// if we get the help info, use it
		if (!strHelpFile.IsEmpty())
		{
			// as suggested by Joseph Givens (from UE) and ScotG,
			// if we do not have proper context, use HELP_FINDER
			// instead of HELP_CONTEXT
			UINT	uiCommand = (ulHelpContext != 0) ? HELP_CONTEXT : HELP_FINDER;
			
			// remap certain control help to istudio help when being invoked
			// from the property browser's help button
			CFrameWnd *pFrame = ((CFrameWnd*)AfxGetMainWnd())->GetActiveFrame();
			if(pFrame)
			{
				CDocument *pDoc = pFrame->GetActiveDocument();
				if(pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CIPCompDoc)))
				{
					//	fm20.hlp	HELPID_FORMSFORM	-> istudio.hlp, HELPID_LAYOUTEDITOR
					//	fm20.hlp	HELPID_FORMSFORM	-> istudio.hlp, HELPID_OBJECTEDITOR
					//	fm20.hlp	id              	-> isfm20.hlp,  id
					//	ped.hlp		HELPID_HOTSPOT		-> isctrls.hlp,	HELPID_HOTSPOT
					//	ped.hlp		HELPID_IMAGE		-> isctrls.hlp,	HELPID_IMAGE
					//	ped.hlp		HELPID_LAYOUT		-> isctrls.hlp,	HELPID_LAYOUT
					//	ped.hlp		id          		-> isctrls.hlp,	id (SHOULD NOT HAPPEN)

					// convert the strings to lower case so that Find works fine
					strHelpFile.MakeLower();
					if(strHelpFile.Find(_T("fm20.hlp")) > -1)
						strHelpFile = _T("isfm20.hlp");
					else if(strHelpFile.Find(_T("ped.hlp")) > -1)
						strHelpFile = _T("isctrls.hlp");
				}
			}

			theApp.HelpOnApplication(ulHelpContext, uiCommand, strHelpFile);
		}
	}

	::SysFreeString(bstrHelpFile);
	return S_OK;
}

// Standard method.  Just calls the main property list box to create its
// children, set the header column widths, etc.,.  Also creates the header
// control.
BOOL COleAllPage::OnInitDialog() 
{
	VERIFY(m_lbProps.SubclassDlgItem(IDC_ALLPROP_PROPLIST, this));
	CSlobPage::OnInitDialog();
	
	m_lbProps.CreateChildren();
	CRect rect;
	m_lbProps.GetClientRect(&rect);
	m_lbProps.SetColumnWidth(rect.Width() / 4);
	m_lbProps.SetFont(GetStdFont(font_Normal));
	m_lbProps.ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);	// We need parent notifies.

	CRect	rcHeader = rect;
	int		iHeight = GetDefaultItemHeight(this);

	if (iHeight > 0)
		m_ihcHeight = iHeight;

	rcHeader.bottom = rcHeader.top + m_ihcHeight;
	m_hcProps.Create(HDS_BUTTONS | HDS_HORZ | WS_CHILD | WS_VISIBLE, //CCS_TOP | 
		rcHeader, this, IDC_PROP_HEADER);

	m_hcProps.SetFont(GetStdFont(font_Normal));

	HD_ITEM	hdi;
	CString	strProperty;
	CString	strValue;

	strProperty.LoadString(IDS_PROPERTYBROWSER_PROPERTY);
	strValue.LoadString(IDS_PROPERTYBROWSER_VALUE);

	LPSTR	pszCol1 = strProperty.GetBuffer(strProperty.GetLength() + 1);
	LPSTR	pszCol2 = strValue.GetBuffer(strValue.GetLength() + 1);

	m_ihcCol0Width = m_ihcCol1Width = rect.Width() / 2;

	ZeroMemory(&hdi, sizeof(hdi));
	hdi.mask = HDI_TEXT | HDI_WIDTH;
	hdi.cxy = m_ihcCol0Width;
	hdi.pszText = pszCol1;
	hdi.cchTextMax = _tclen(pszCol1);

	m_hcProps.InsertItem(0, &hdi);

	hdi.pszText = pszCol2;
	hdi.cchTextMax = _tclen(pszCol2);

	m_hcProps.InsertItem(1, &hdi);

	// load these strings once since we use them all over the place
	s_strTrue.LoadString(IDS_PROP_TRUE);
	s_strFalse.LoadString(IDS_PROP_FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL COleAllPage::Create(
	UINT	nIDSheet,
	CWnd*	pWndOwner)
{
	if (!CSlobPage::Create(nIDSheet, pWndOwner))
		return FALSE;

	// Save our minimum size, in case we get bigger.
	CRect	rectT;
	GetWindowRect(rectT);
	m_sizeMin = rectT.Size();

	m_bActive = TRUE;
	return TRUE;
}

// This method implements the "snap-back" effect.  When the m_bSetHeader
// flag is set, the 1st and 2nd columns are set to the previously
// determined sizes (in OnEndTrackHeader).
void COleAllPage::SetHeaderInfo()
{
	// we need to have this so that after the dragging of the divider
	// is done, we need to "snap-back" the divider to the right
	// position if the column 0 or 1 is too small.
	if (m_bSetHeader)
	{
		HD_ITEM	hdi;
		BOOL	bRepaint = FALSE;

		ZeroMemory(&hdi, sizeof(hdi));
		hdi.mask = HDI_WIDTH;

		m_hcProps.GetItem(0, &hdi);
		if (m_ihcCol0Width != hdi.cxy)
		{
			hdi.cxy = m_ihcCol0Width;
			m_hcProps.SetItem(0, &hdi);
			bRepaint = TRUE;
		}

		m_hcProps.GetItem(1, &hdi);
		if (m_ihcCol1Width != hdi.cxy)
		{
			hdi.cxy = m_ihcCol1Width;
			m_hcProps.SetItem(1, &hdi);
			bRepaint = TRUE;
		}

		if (bRepaint)
		{
			m_hcProps.InvalidateRect(NULL);
			m_hcProps.UpdateWindow();
		}

		m_bSetHeader = FALSE;
	}
}

// This handles the notification message when the header separation
// dragging stops.  We need to handle this so that we achieve the "snap-back"
// effect if the user makes the size of the first or second column too small.
// If the size of either column is < 1/5th the width of the header control,
// then the size of that column is set to 1/5th (and the other column is 4/5th).
// m_bSetHeader flag is set so that we can achieve the "snap-back" effect when
// the main property list box is redrawn (which calls SetHeaderInfo above)
void COleAllPage::OnEndTrackHeader(
	NMHDR*		pNMHDR,
	LRESULT*	pResult)
{
	HD_NOTIFY*	phdn = (HD_NOTIFY*) pNMHDR;
	HD_ITEM		hdi;
	CRect		rc;
	int			iFifth;

	*pResult = 0;

	m_hcProps.GetClientRect(&rc);
	iFifth = rc.Width() / 5;	// minimum column (0 or 1) width
	ZeroMemory(&hdi, sizeof(hdi));
	hdi.mask = HDI_WIDTH;

	// store the column widths
	if (phdn->iItem == 0)
	{
		m_ihcCol0Width = phdn->pitem->cxy;
		m_ihcCol1Width = rc.Width() - m_ihcCol0Width;
	}
	else if (phdn->iItem == 1)
	{
		m_ihcCol1Width = phdn->pitem->cxy;
		m_ihcCol0Width = rc.Width() - m_ihcCol1Width;
	}

	// recalculate column widths if either one is less than one fifth
	// the width of the entire header width
	// if so, set the flag so that we can resize ourselves later
	if (m_ihcCol0Width <= iFifth)
	{
		m_ihcCol0Width = iFifth;
		m_ihcCol1Width = rc.Width() - m_ihcCol0Width;
		m_bSetHeader = TRUE;
	}
	else if (m_ihcCol0Width >= (rc.Width() - iFifth))
	{
		m_ihcCol1Width = iFifth;
		m_ihcCol0Width = rc.Width() - m_ihcCol1Width;
		m_bSetHeader = TRUE;
	}

	if (phdn->iItem == 0)
	{
		hdi.cxy = m_ihcCol1Width;
		m_hcProps.SetItem(1, &hdi);	// set the other column width
	}
	else if (phdn->iItem == 1)
	{
		hdi.cxy = m_ihcCol0Width;
		m_hcProps.SetItem(0, &hdi);	// set the other column width
	}

	// repaint grid-listbox so that it can reflect the change in column widths
	m_lbProps.InvalidateRect(NULL);
}

// Handles the notification message when the Header separator is dragged
// There are 2 reasons for why we are handling this message.
// 1. We do not want the separator between the 2nd column and the 3rd (this
// column cannot be seen) to be dragged.
// 2. If the drop-down list box was visible when the dragging started, we
// want that to be hidden immediately.
void COleAllPage::OnTrackHeader(
	NMHDR*		pNMHDR,
	LRESULT*	pResult)
{
	HD_NOTIFY*	phdn = (HD_NOTIFY*) pNMHDR;

	// prevent the sizing of column 1
	// (dragging the divider between column 1 and 2)
	// because columns 0 and 1 can be resized by just dragging the
	// divider between columns 0 and 1
	if (phdn->iItem == 1)
		*pResult = TRUE;
	else
		*pResult = FALSE;

	// if the drop-down list box was visible, hide it
	if (m_lbProps.m_pLb->IsWindowVisible())
		m_lbProps.m_pLb->SetWindowPos(&wndBottom, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
}

// Helper which changes the current size of this property page.
void COleAllPage::ChangePageSize(
	CSize const&	sizeNew)
{
	CRect	rect;
	GetWindowRect(rect);
	CSize sizeOld = rect.Size();

	SetWindowPos(NULL, 0, 0, sizeNew.cx, sizeNew.cy,
				 SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	// make the list box fit inside the all-page property page
	HD_ITEM	hdi;

	m_ihcCol0Width = sizeNew.cx / 2;
	m_ihcCol1Width = sizeNew.cx - 1 - m_ihcCol0Width;	// -1 for visual hack

	ZeroMemory(&hdi, sizeof(hdi));
	hdi.mask = HDI_WIDTH;
	hdi.cxy = m_ihcCol0Width;
	m_hcProps.SetItem(0, &hdi);
	hdi.cxy = m_ihcCol1Width;
	m_hcProps.SetItem(1, &hdi);

	m_hcProps.SetWindowPos(NULL, 0, 0, sizeNew.cx - 1,
		m_ihcHeight, SWP_NOACTIVATE | SWP_NOZORDER);

	m_lbProps.SetWindowPos(NULL, 0, m_ihcHeight - 1, sizeNew.cx,
		sizeNew.cy - m_ihcHeight + 1, SWP_NOACTIVATE | SWP_NOZORDER);
}

// If the sheet asks how big we are, we assume that the contents of the sheet are
// changing.  We therefore shrink down to our minimum size ... if we are the current
// page, we will grow again when InitializePage is called.
CSize COleAllPage::GetPageSize()
{
	if (m_hWnd != NULL)		// are we initialized?
	{
		CRect rect;
		GetWindowRect(rect);
		if (rect.Size() != m_sizeMin)
			ChangePageSize(m_sizeMin);
	}
	return C3dPropertyPage::GetPageSize();
}

// When initialized, we grow to the maximum size that will fit on the sheet
// (i.e. the maximum size of any other property page).
void COleAllPage::InitializePage()
{
	ChangePageSize(m_pSheetWnd->m_sizePageMax);
	CSlobPage::InitializePage();	
}

// Initializes the property page and fills the property list (grid)
void COleAllPage::InitPage()
{
	COleRef<IDispatch>	srpDisp;

	ReleaseInterfaces();	// free cached IDispatch and ITypeInfo interfaces
	m_ulCountControls = COlePage::s_nObjectCurrent;
	if (1 == m_ulCountControls)
	{
		// ok, how about IDispatch for an all page
		if (SUCCEEDED(COlePage::s_pObjectCurrent[0]->QueryInterface(
			IID_IDispatch, (void**)&srpDisp)))
		{
			m_pSingleDisp = srpDisp.Disown();
			m_pprgDisp = &m_pSingleDisp;
		}
		else
		{
			m_ulCountControls = 0;
		}
	}
	else
	{
		m_pprgDisp = (IDispatch **)AfxAllocTaskMem(sizeof(IDispatch *) * m_ulCountControls);
		ASSERT(m_pprgDisp);
		if (NULL == m_pprgDisp)
			m_ulCountControls = 0;

		ULONG	j;
		for (ULONG i = 0; i < m_ulCountControls; ++i)
		{
			// ok, how about IDispatch for an all page
			if (SUCCEEDED(COlePage::s_pObjectCurrent[i]->QueryInterface(
				IID_IDispatch, (void**)&srpDisp)))
			{
				m_pprgDisp[i] = srpDisp.Disown();
			}
			else
			{
				// something went wrong, release all interfaces and
				// bail out of here
				for (j = 0; j < i; ++j)
					m_pprgDisp[j]->Release();

				AfxFreeTaskMem(m_pprgDisp);
				m_pprgDisp = NULL;
				m_ulCountControls = 0;			// will also terminate the for()
			}
		}
	}

	GetNotifySinkInfo();	// get all notify sinks
	FillPropList();
}

// This method implements the initializing of the property list (grid)
void COleAllPage::FillPropList()
{
	m_lbProps.SetRedraw(FALSE);	// to avoid flickering
	m_lbProps.ResetContent();	// delete all the items first

	int				iShowAllPropsList;
	int				iShowNoBrowseableStatic;
	BOOL			bRet = FALSE;
	CMapStringToPtr	map;

	// get the properties for the first control
	if (m_ulCountControls > 0)
	{
		// set the number of items (controls) selected
		// NOTE: THIS IS IMPORTANT, otherwise, caching of dispids will FAIL
		CPropItem::s_ulCountControls = m_ulCountControls;

		bRet = AddToMap(m_pprgDisp[0], &map);
	}

	// now intersect those properties with the other controls
	for (ULONG i = 1; bRet && (map.GetCount() > 0) && (i < m_ulCountControls); ++i)
		bRet = IntersectToMap(&map, i);

	// if no properties are in the list box hide the lb, show
	// the "no browseable properties" static
	if (map.GetCount() <= 0)
	{
		iShowAllPropsList = SW_HIDE; 
		iShowNoBrowseableStatic = SW_SHOW;
	}
	else
	{
		int			iIndex;
		CString		str;
		CPropItem*	pItem = NULL;
		POSITION	pos = map.GetStartPosition();

		while(NULL != pos)
		{
			map.GetNextAssoc(pos, str, (void*&)pItem);
			ASSERT(pItem && (str == pItem->m_strText));
			LBAddStringAndSetItemData(&m_lbProps, pItem->m_strText, (DWORD)pItem);
		}

		map.RemoveAll();
		iShowAllPropsList = SW_SHOW;
		iShowNoBrowseableStatic = SW_HIDE;
	}

	GetDlgItem(IDC_ALLPROP_PROPLIST)->ShowWindow(iShowAllPropsList);
	GetDlgItem(IDC_PROP_HEADER)->ShowWindow(iShowAllPropsList);
	GetDlgItem(IDC_ALLPROP_NOBROWSEABLE)->ShowWindow(iShowNoBrowseableStatic);

	m_lbProps.SetCurSel(0);
	m_lbProps.SetRedraw(TRUE);
	m_lbProps.Invalidate();
}

// Returns TRUE ifthe given ITypeInfo interface has GUID_HasPathProperties tag
// attached to it.
BOOL COleAllPage::DoesTypeInfoHasPathProperties(
	ITypeInfo*	srpTypeInfoStart)
{
	BOOL				bHasPathProps = FALSE;
	COleRef<ITypeInfo2>	srpTypeInfo2;

	if (SUCCEEDED(srpTypeInfoStart->QueryInterface(IID_ITypeInfo2, (void**)&srpTypeInfo2)))
	{
		VARIANT	varValue;

		// is the path property supported
		::VariantInit(&varValue);
		if (SUCCEEDED(srpTypeInfo2->GetCustData(GUID_HasPathProperties, &varValue))
			&& (varValue.vt != VT_EMPTY))
		{
			bHasPathProps = TRUE;
		}
		else
		{
			// find out if the top-level typeinfo supports path property
			COleRef<IPersist>	srpPersist;
			COleRef<ITypeInfo>	srpTypeInfo;
			COleRef<ITypeLib>	srpTypeLib;
			CLSID				clsid;
			
			if (SUCCEEDED(m_dispDriver.m_lpDispatch->QueryInterface(IID_IPersist,
					(void**)&srpPersist)) &&
				SUCCEEDED(srpPersist->GetClassID(&clsid)) &&
				SUCCEEDED (srpTypeInfoStart->GetContainingTypeLib(&srpTypeLib, 0)) &&
				SUCCEEDED(srpTypeLib->GetTypeInfoOfGuid(clsid, &srpTypeInfo)))
			{
				// release the current typeinfo2 interface since we are going
				// to reuse the same variable
				srpTypeInfo2.SRelease();
				::VariantInit(&varValue);
				// get the typeinfo2 from the new top-level typeinfo
				// and try to find out if that supports the path properties
				if (SUCCEEDED(srpTypeInfo->QueryInterface(IID_ITypeInfo2, (void**)&srpTypeInfo2)) &&
					SUCCEEDED(srpTypeInfo2->GetCustData(GUID_HasPathProperties, &varValue))
					&& (varValue.vt != VT_EMPTY))
					bHasPathProps = TRUE;
			}
		}
	}

	return bHasPathProps;
}

// This method goes through the ITypeInfo's of the first control selected
// and gathers all the properties that can be modified and stores all the
// necessary information needed to edit any property.  For single selection
// (of controls) this becomes the final set of properties, and for multiple
// selection, this is the initial set of properties which is used to compare
// with the set of properties that we get for the other controls selected
// and get the "common" set of properties for all the controls selected.
// This intersecting of properties is done in IntersectToMap method.
BOOL COleAllPage::AddToMap(
	IDispatch*			pDisp,
	CMapStringToPtr*	pMap)
{
	ASSERT(pDisp && pMap);
	if (NULL == pDisp || NULL == pMap)
		return FALSE;
	
	m_dispDriver.AttachDispatch(pDisp, FALSE);

	COleRef<IProvideMultipleClassInfo>	srpPMCI;

	int			i;
	int			implTypeFlags;
	BOOL		bHasPathProps;
	BOOL		bCacheDesc;
	ULONG		ulCurTypeInfo;
	ULONG		ulCountTypeInfos;
	HREFTYPE	hrefType;
	HREFTYPE	hrtFuncDesc;
	LPVARDESC	pvd;
	LPFUNCDESC	pfd;
	LPTYPEATTR	pta;
	LPTYPEATTR	ptaStart;
	CPropItem*	pItem;
	VARIANT		var;
	DataType	dt;
	CString		strNPropName;
	HRESULT		hr;

	// see if the object provides multiple class infos
	if (FAILED(pDisp->QueryInterface(IID_IProvideMultipleClassInfo, (void**)&srpPMCI)) ||
		FAILED(srpPMCI->GetMultiTypeInfoCount(&ulCountTypeInfos)))
		ulCountTypeInfos = 1;

	// if we have only one type info, do not allocate memory for an array
	m_ulTypeInfos = ulCountTypeInfos;
	if (1 == ulCountTypeInfos)
	{
		m_ppTypeInfo = &m_pSingleTypeInfo;
		m_ppTypeInfo[0] = NULL;
	}
	else
	{
		m_ppTypeInfo = (ITypeInfo**) AfxAllocTaskMem(sizeof(ITypeInfo*) * ulCountTypeInfos);
		ASSERT(m_ppTypeInfo);
		if (NULL == m_ppTypeInfo)
			return E_OUTOFMEMORY;

		// Zero out the array so that we can safely walk through later and release
		// pointers to typeinfos if we have to bail out of this function.
		for (ulCurTypeInfo = 0; ulCurTypeInfo < ulCountTypeInfos; ulCurTypeInfo++)
			m_ppTypeInfo[ulCurTypeInfo] = NULL;
	}

	// go through each typeinfo and get the properties that can be edited
	for (ulCurTypeInfo = 0; ulCurTypeInfo < ulCountTypeInfos; ++ulCurTypeInfo)
	{
		COleRef<ITypeInfo>	srpTypeInfoStart;	// use this to get the correct typeinfo
		COleRef<ITypeInfo>	srpTypeInfo;		// the type info we use to get props

		pta = NULL;
		ptaStart = NULL;
		if (srpPMCI)
		{
			// since we are looping, make sure we release a
			// previously QI's interfaced
			if (srpTypeInfo != NULL)
				srpTypeInfo.SRelease();
			if (srpTypeInfoStart != NULL)
				srpTypeInfoStart.SRelease();

			if (FAILED(srpPMCI->GetInfoOfIndex(ulCurTypeInfo, 
				MULTICLASSINFO_GETTYPEINFO,	&srpTypeInfoStart, 
				NULL, NULL, NULL, NULL)))
			{
				ASSERT(FALSE);

				for (ULONG i=0; i<ulCountTypeInfos; i++)
				{
					ITypeInfo *&rpITypeInfo = m_ppTypeInfo[i];

					if (rpITypeInfo != NULL)
					{
						rpITypeInfo->Release();
						rpITypeInfo = NULL;
					}
				}

				if (ulCountTypeInfos != 1)
					AfxFreeTaskMem(m_ppTypeInfo);

				m_ppTypeInfo = NULL;

				m_ulTypeInfos = 0;

				return FALSE;
			}
		}
		else
		{
			ASSERT(1 == ulCountTypeInfos);
			hr = m_dispDriver.m_lpDispatch->GetTypeInfo(0, s_lcid, &srpTypeInfoStart);
			if (FAILED(hr))
			{
				// Report an error; controls are supposed to be able to yield a
				// typeinfo since they must support IDispatch.  Tell the user to
				// complain to the control's author.
				TCHAR	szBuff[80];
				CString	strMessage;

				wsprintf(szBuff, _T("0x%08lx"), hr);
				AfxFormatString1(strMessage, IDS_NO_CONTROL_TYPEINFO, szBuff);
				AfxMessageBox(strMessage);

				for (ULONG i=0; i<ulCountTypeInfos; i++)
				{
					ITypeInfo *&rpITypeInfo = m_ppTypeInfo[i];

					if (rpITypeInfo != NULL)
					{
						rpITypeInfo->Release();
						rpITypeInfo = NULL;
					}
				}

				if (ulCountTypeInfos != 1)
					AfxFreeTaskMem(m_ppTypeInfo);

				m_ppTypeInfo = NULL;

				m_ulTypeInfos = 0;

				return FALSE;
			}
		}

		ASSERT(srpTypeInfoStart != NULL);

		bHasPathProps = DoesTypeInfoHasPathProperties(srpTypeInfoStart);

		// new code below to interpret sub typeinfo's
		LPTYPEATTR	ptaStart;

		VERIFY(srpTypeInfoStart->GetTypeAttr(&ptaStart) == S_OK);
		if (ptaStart->typekind == TKIND_DISPATCH ||
			ptaStart->typekind == TKIND_INTERFACE)
		{
			// since this is the correct typeinfo, get its interface pointer
			// and release the type attribute.
			srpTypeInfo = srpTypeInfoStart.Disown();
			pta = ptaStart;
		}
		else if (ptaStart->typekind == TKIND_COCLASS)
		{
			// go through and find out if we get a typekind of dispatch
			// if so, continue, otherwise, ignore this
			for (i = 0; i < ptaStart->cImplTypes; i++)
			{
				VERIFY(srpTypeInfoStart->GetImplTypeFlags(i, &implTypeFlags) == S_OK);
				VERIFY(srpTypeInfoStart->GetRefTypeOfImplType(i, &hrefType) == S_OK);
				VERIFY(srpTypeInfoStart->GetRefTypeInfo(hrefType, &srpTypeInfo) == S_OK);

				VERIFY(srpTypeInfo->GetTypeAttr(&pta) ==S_OK);
				if (pta->typekind != TKIND_DISPATCH)
				{
					srpTypeInfo->ReleaseTypeAttr(pta);
					srpTypeInfo.SRelease();
					pta = NULL;
					continue;
			//<=====
				}

				if (implTypeFlags & IMPLTYPEFLAG_FDEFAULT)
				{
					// release the start typeinfo and pta
					srpTypeInfoStart->ReleaseTypeAttr(ptaStart);
					break;
			//<=====
				}

				// release the type attribute since we do not need this
				srpTypeInfo->ReleaseTypeAttr(pta);
				srpTypeInfo.SRelease();
				pta = NULL;
			}
		}

		// new code above
		if (NULL == pta)
		{
			srpTypeInfoStart->ReleaseTypeAttr(ptaStart);
			continue;
		}

		//  Enumerate properties
		pvd = NULL;
		pfd = NULL;
		// handle the vardesc
		for (i = 0; i < pta->cVars; i++)
		{
			if (FAILED(srpTypeInfo->GetVarDesc(i, &pvd)))
				continue;

			ASSERT(pvd);
			dt = data_Unknown;
			bCacheDesc = FALSE;

			// hWnd parameter is not editable
			// make sure that the property is editable
			// also, finally to make sure, get the property value, which
			// fails if property is not editable
			if ((DISPID_HWND != pvd->memid) &&
				!IsPropTypeReadOnly(pvd, NULL, TRUE) &&
				GetPropValue(pDisp, pvd->memid, &var))
			{
				// get the property data type
				dt = GetPropType(pDisp, srpTypeInfo, pvd, NULL, bHasPathProps);
			}

			// do not allow editing of font and picture for multiple selection
			if ((dt != data_Unknown) && (m_ulCountControls > 1) &&
				((dt == data_Font) || (dt == data_Picture)))
				dt = data_Unknown;

			//if we made through to this place, get the property name
			if ((dt != data_Unknown) &&
				GetPropName(srpTypeInfo, pvd->memid, strNPropName))
			{
				pItem = new CPropItem(strNPropName, pvd->elemdescVar.tdesc.vt,
					dt, pvd->memid);
				if (GetPropDetails(pDisp, srpTypeInfo,
					pvd->elemdescVar.tdesc.hreftype, pvd->memid, pItem, TRUE, &var))
				{
					pMap->SetAt(strNPropName, (void*)pItem);
					// just save a copy of ITypeInfo, actuall addref'ed ITypeInfo
					// is stored in m_ppTypeInfo array in COleAllPage.
					// NOTE: <ignatius vc98:2910>
					// we still need to addref it since COleAllPage releases all the
					// m_ppTypeInfo stuff before the pItem is deleted.
					pItem->m_TypeInfo = srpTypeInfo;
					if (pItem->m_TypeInfo)
						pItem->m_TypeInfo->AddRef();
					pItem->m_pvd = pvd;	// cache vardesc for later use
					bCacheDesc = TRUE;	// this is released when pItem is destructed
				}
				else
				{
					delete pItem;
				}
			}

			::VariantClear(&var);
			if (!bCacheDesc)
				srpTypeInfo->ReleaseVarDesc(pvd);
		}

		// handle the funcdesc
		for (i = 0; i < pta->cFuncs; i++)
		{
			if (FAILED(srpTypeInfo->GetFuncDesc(i, &pfd)))
				continue;

			ASSERT(pfd);
			dt = data_Unknown;
			bCacheDesc = FALSE;

			// hWnd parameter is not editable
			// make sure that the property is editable
			// also, finally to make sure, get the property value, which
			// fails if property is not editable
			if ((DISPID_HWND != pfd->memid) &&
				!IsPropTypeReadOnly(NULL, pfd, TRUE) &&
				GetPropValue(pDisp, pfd->memid, &var))
			{
				// get the property data type
				dt = GetPropType(pDisp, srpTypeInfo, NULL, pfd, bHasPathProps);
			}

			// do not allow editing of font and picture for multiple selection
			if ((dt != data_Unknown) && (m_ulCountControls > 1) &&
				((dt == data_Font) || (dt == data_Picture)))
				dt = data_Unknown;

			//if we made through to this place, get the property name
			if ((dt != data_Unknown) &&
				GetPropName(srpTypeInfo, pfd->memid, strNPropName) &&
				!pMap->Lookup(strNPropName, (void*&)pItem))
			{
				pItem = new CPropItem(strNPropName,
					pfd->lprgelemdescParam->tdesc.vt, dt, pfd->memid);
				hrtFuncDesc = pfd->lprgelemdescParam->tdesc.hreftype;
				if ((pfd->lprgelemdescParam->tdesc.vt == VT_PTR) &&
					(pfd->lprgelemdescParam->tdesc.lptdesc->vt == VT_USERDEFINED))
					hrtFuncDesc = pfd->lprgelemdescParam->tdesc.lptdesc->hreftype;

				if (GetPropDetails(pDisp, srpTypeInfo, hrtFuncDesc, pfd->memid,
					pItem, TRUE, &var))
				{
					pMap->SetAt(strNPropName, (void*)pItem);
					// just save a copy of ITypeInfo, actuall addref'ed ITypeInfo
					// is stored in m_ppTypeInfo array in COleAllPage.
					// NOTE: <ignatius vc98:2910>
					// we still need to addref it since COleAllPage releases all the
					// m_ppTypeInfo stuff before the pItem is deleted.
					pItem->m_TypeInfo = srpTypeInfo;
					if (pItem->m_TypeInfo)
						pItem->m_TypeInfo->AddRef();
					pItem->m_pfd = pfd;	// cache funcdesc for later use
					bCacheDesc = TRUE;	// this is released when pItem is destructed
				}
				else
				{
					delete pItem;
				}
			}

			::VariantClear(&var);
			if (!bCacheDesc)
				srpTypeInfo->ReleaseFuncDesc(pfd);
		}

		srpTypeInfo->ReleaseTypeAttr(pta);

		// store the ITypeInfo in the array
		m_ppTypeInfo[ulCurTypeInfo] = srpTypeInfo.Disown();
	}

	return TRUE;
}

// "Intersects" the set of properties that can be edited.  AddToMap method
// gets the first set of properties from the first control -- and for the rest
// of the controls selected, this method, shortens the set of properties
// that are common to all the controls selected.  For example, if the "ForeColor"
// property exists in control A and not in control B, then it is eliminated
// from the set of properties here.
BOOL COleAllPage::IntersectToMap(
	CMapStringToPtr*	pMap,
	ULONG				ulIndex)
{
	IDispatch*	pDisp = m_pprgDisp[ulIndex];

	// we should have more than one control selected to get here
	ASSERT((m_ulCountControls > 1) && pDisp && pMap && pMap->GetCount());
	if (NULL == pDisp || NULL == pMap || m_ulCountControls <= 1 || 0 == pMap->GetCount())
		return FALSE;

	m_dispDriver.AttachDispatch(pDisp, FALSE);

	// see if the object provides multiple class infos
	COleRef<IProvideMultipleClassInfo>	srpPMCI;
    
	COleRef<ITypeInfo>	srpTypeInfo;
	COleRef<ITypeComp>	srpTypeComp;	
	COleRef<ITypeInfo>	srpTypeInfoBind;

	BOOL		bPropExists;
	ULONG		ulCurTypeInfo;
	ULONG		ulCountTypeInfos;
	POSITION	pos;
	CPropItem*	pItem;
	CString		strPropName;
	DESCKIND	descKind;
	BINDPTR		bindPtr;
	VARTYPE		vt;
	DataType	dt;
	CString		strNPropName;

	if (FAILED(pDisp->QueryInterface(
		IID_IProvideMultipleClassInfo, (void**)&srpPMCI)) ||
		FAILED(srpPMCI->GetMultiTypeInfoCount(&ulCountTypeInfos)))
		ulCountTypeInfos = 1;

	// walk the list box and see if these props exits in the
	// object we were given (represented by pDisp)
	USES_CONVERSION;
	pos = pMap->GetStartPosition();
	while(NULL != pos)
	{
		pMap->GetNextAssoc(pos, strPropName, (void*&)pItem);
		ASSERT(pItem && (strPropName == pItem->m_strText));

		bPropExists = FALSE;
		for (ulCurTypeInfo = 0; ulCurTypeInfo < ulCountTypeInfos; ++ulCurTypeInfo)
		{
			if (srpPMCI)
			{
				if (FAILED(srpPMCI->GetInfoOfIndex(ulCurTypeInfo, 
					MULTICLASSINFO_GETTYPEINFO,	&srpTypeInfo, 
					NULL, NULL, NULL, NULL)) ||
					FAILED(srpTypeInfo->GetTypeComp(&srpTypeComp)))
				{
					ASSERT(FALSE);
					return FALSE;
				}
			}
			else
			{
				ASSERT(1 == ulCountTypeInfos);
				if (FAILED(m_dispDriver.m_lpDispatch->GetTypeInfo(
					0, s_lcid, &srpTypeInfo)) ||
					FAILED(srpTypeInfo->GetTypeComp(&srpTypeComp)))
				{
					ASSERT(FALSE);
					return FALSE;
				}
			}

			ASSERT(srpTypeInfo && srpTypeComp);
			if (SUCCEEDED(srpTypeComp->Bind(T2OLE(strPropName), 
				::LHashValOfName(s_lcid, T2OLE(strPropName)),
				INVOKE_PROPERTYPUTREF | INVOKE_PROPERTYPUT, 
				&srpTypeInfoBind, &descKind, &bindPtr)))
			{
				if (DESCKIND_VARDESC == descKind)
				{
					if (!((bindPtr.lpvardesc->wVarFlags & VARFLAG_FREADONLY) ||
						pItem->m_vt != bindPtr.lpvardesc->elemdescVar.tdesc.vt ||
						!IntersectPropItemData(pDisp,
							bindPtr.lpvardesc->memid, pItem,
							srpTypeInfo, &bindPtr.lpvardesc->elemdescVar.tdesc)))
					{
						pItem->SetDispid(ulIndex, bindPtr.lpvardesc->memid);
						bPropExists = TRUE;
						ulCurTypeInfo = ulCountTypeInfos;	// break out of for loop
					}
				}
				else if (DESCKIND_FUNCDESC == descKind)
				{
					ASSERT(bindPtr.lpfuncdesc);
					dt = data_Unknown;
					// if property is not read-only, get the property data type
					if (!IsPropTypeReadOnly(NULL, bindPtr.lpfuncdesc, FALSE))
						dt = GetPropType(pDisp, srpTypeInfo, NULL,
							bindPtr.lpfuncdesc, FALSE);

					if ((dt == data_Unknown) ||
						!(GetPropName(srpTypeInfo, bindPtr.lpfuncdesc->memid,
							strNPropName)))
					{
						ulCurTypeInfo = ulCountTypeInfos;	// break out of for loop
					}
					else
					{
						if ((bindPtr.lpfuncdesc->invkind & 
							(INVOKE_PROPERTYPUT | INVOKE_PROPERTYPUTREF)) &&
							(1 == bindPtr.lpfuncdesc->cParams) &&
							(pItem->m_vt == bindPtr.lpfuncdesc->lprgelemdescParam[0].tdesc.vt) &&
							IntersectPropItemData(pDisp,
								bindPtr.lpfuncdesc->memid, pItem, srpTypeInfo,
								&bindPtr.lpfuncdesc->lprgelemdescParam[0].tdesc))
						{
							pItem->SetDispid(ulIndex, bindPtr.lpfuncdesc->memid);
							bPropExists = TRUE;
							ulCurTypeInfo = ulCountTypeInfos;	// break out of for loop
						}
					}
				}

				DeleteBindPtr(srpTypeInfo, &descKind, &bindPtr);
			}
			
			srpTypeInfo.SRelease();
			srpTypeComp.SRelease();
			srpTypeInfoBind.SRelease();
		}

		if (!bPropExists)
		{
			pMap->RemoveKey(pItem->m_strText);
			delete pItem;
		}
	}

	return TRUE;
}

void COleAllPage::DeleteBindPtr(
	ITypeInfo*	pTypeInfo,
	DESCKIND*	pDescKind,
	BINDPTR*	pBindPtr)
{
	ASSERT(pTypeInfo && pDescKind && pBindPtr);
	if (NULL == pTypeInfo || NULL== pDescKind || NULL == pBindPtr)
		return;

	switch(*pDescKind)
	{
		case DESCKIND_FUNCDESC:
			pTypeInfo->ReleaseFuncDesc(pBindPtr->lpfuncdesc);
			break;
		case DESCKIND_VARDESC:
			pTypeInfo->ReleaseVarDesc(pBindPtr->lpvardesc);
			break;
		case DESCKIND_TYPECOMP:
			pBindPtr->lptcomp->Release();
			break;
		case DESCKIND_IMPLICITAPPOBJ:
		case DESCKIND_NONE:
			break;
		default:
			ASSERT(FALSE);		// Bogus descKind
			break;
	}	
}

// Frees all the data/interfaces stored within the AllPage
// This is the complement function to InitPage
void COleAllPage::TermPage()
{
	m_lbProps.ResetContent();	// remove all items from the list box
	ReleaseInterfaces();		// free cached IDispatch and ITypeInfo interfaces
	CSlobPage::TermPage();		// call base class
}

// Validates and applies the property change to the set of selected controls
BOOL COleAllPage::Validate()
{
	// if the property has changed, then-only call apply
	if ((m_ulCountControls > 0) && m_pprgDisp && m_lbProps.m_pTxt->GetModify())
	{
		CPropItem*	pItem;

		// there is a situation that the modified text is in the edit control
		// but not updated to the pItem->m_strProp yet, so update it here
		if (m_lbProps.m_uiIndex != -1)
		{
			pItem = (CPropItem*) m_lbProps.GetItemData(m_lbProps.m_uiIndex);
			m_lbProps.m_pTxt->GetWindowText(pItem->m_strProp);
			// set the modify flag to false since we do not want to
			// save the same property twice
			m_lbProps.m_pTxt->SetModify(FALSE);
		}

		if (!Apply())
			return FALSE;
	}
		
	m_nValidateID = NULL;
	return TRUE;
}

BOOL COleAllPage::UndoPendingValidate()
{
//	TermPage();
	// for the time being read call this, later we need to change this to
	// load the single property
//	UpdateChangedProperties();
	return FALSE;
}

// The top level method to save the property for all the selected control(s)
BOOL COleAllPage::Apply()
{
	// if we are not displaying any properties (because there are none for one
	// of 2 reasons: 1. there are no properties, 2. there are no common
	// properties among the selected controls)
	// just return TRUE
	// The m_lbProps window will be not visible in this case
	if ((m_lbProps.GetCount() < 0) || (m_lbProps.m_uiIndex == -1))
		return TRUE;

	BOOL		bSet = FALSE;
	BOOL		fSuccess = TRUE;
	CPropItem*	pItem;

	ASSERT(m_pprgDisp);
	
	m_bUpdateLater = FALSE;
	// tell IPropertyNotifySink::OnChanged that we are updating properties
	// so that all refreshing happen later
	m_bUpdatingProps = TRUE;

	m_pSlob->BeginUndo(IDS_UNDO_OLE_PROPS);

	USES_CONVERSION;
	for (int i = 0; i < m_ulCountControls && fSuccess; i++)
	{
		ASSERT(NULL != m_pprgDisp[i]);
		ASSERT(m_lbProps.m_uiIndex != -1);
		if (m_lbProps.m_uiIndex != -1)
		{
			pItem = (CPropItem*) m_lbProps.GetItemData(m_lbProps.m_uiIndex);
			ASSERT(NULL != pItem);
			m_dispDriver.AttachDispatch(m_pprgDisp[i], FALSE);
			fSuccess = SUCCEEDED(ApplyProperty(pItem->m_pDispid[i], pItem,
				m_pprgDisp[i]));
		}
	}

	// check to see if we made it through the array
	bSet = (i == m_ulCountControls && fSuccess);
	// EndUndo takes a flag which is TRUE to abort the Undo Record, & FALSE 
	// to keep the undo record
	m_pSlob->EndUndo(!bSet);

	m_bUpdatingProps = FALSE;

	// just attach the first dispatch ptr
	m_dispDriver.AttachDispatch(m_pprgDisp[0], FALSE);

	if (m_bUpdateLater)
		UpdateChangedProperties();

	return bSet;
}

// Saves the general properties (not font or picture)
// Appropriate conversinos are done from the string that the user types to the
// correct data type.  If the property could not be saved for some reason, an
// error message is displayed (the control specified error message is displayed
// if the control specifies a particuar error message or a default "Invalid Property"
// message is displayed.
HRESULT COleAllPage::ApplyProperty(
	DISPID		dispid,
	CPropItem*	pItem,
	IDispatch*	pDisp)
{
	HRESULT	hr = S_OK;
	CString	strError;

	USES_CONVERSION;

	// Clear ErrorInfo object before we call SetProperty
	::SetErrorInfo(0L, NULL);
	
	// m_dispDriver must be set by the caller so that it can be used here
	try
	{
		switch(pItem->m_dataType)
		{
			default:
			case data_Unknown:
				ASSERT(FALSE);
				break;
			case data_Int2:		// maps to VT_I2
			case data_UInt2:
			{
				short	sValue;

				hr = VarI2FromStr(T2OLE(pItem->m_strProp), s_lcid,
					LOCALE_NOUSEROVERRIDE, &sValue);
				if (S_OK == hr)
					m_dispDriver.SetProperty(dispid, VT_I2, sValue);
			}
				break;
			case data_Int4:		// maps to VT_I4
			case data_UInt4:
			{
				long	lValue;

				hr = VarI4FromStr(T2OLE(pItem->m_strProp), s_lcid,
					LOCALE_NOUSEROVERRIDE, &lValue);
				if (S_OK == hr)
					m_dispDriver.SetProperty(dispid, VT_I4, lValue);
			}
				break;
			case data_Float:		// maps to VT_R4
			{
				float	fValue;

				hr = VarR4FromStr(T2OLE(pItem->m_strProp), s_lcid,
					LOCALE_NOUSEROVERRIDE, &fValue);
				if (S_OK == hr)
					m_dispDriver.SetProperty(dispid, VT_R4, fValue);
			}
				break;
			case data_Double:		// maps to VT_R8
			{
				double	dValue;

				hr = VarR8FromStr(T2OLE(pItem->m_strProp), s_lcid,
					LOCALE_NOUSEROVERRIDE, &dValue);
				if (S_OK == hr)
					m_dispDriver.SetProperty(dispid, VT_R8, dValue);
			}
				break;
			case data_Bool:		// maps to VT_BOOL
				m_dispDriver.SetProperty(dispid, VT_BOOL, (BOOL)pItem->unionValue.m_nValue);
				break;
			case data_Color:
			{
				long	lColor;
				LPTSTR	pszStopString;

				lColor = _tcstol(LPCTSTR(pItem->m_strProp), &pszStopString, 0);
				if ((*pszStopString != _T('\0')) ||
					(LONG_MAX == lColor) || (LONG_MIN == lColor))
					hr = E_FAIL;
				else
					m_dispDriver.SetProperty(dispid, VT_I4, lColor);
			}
				break;
			case data_Font:
			case data_Picture:
				// these things are handled automatically through
				// the builders or proppages
				break;
			case data_Path:		// works with VT_BSTR fine
			case data_String:	// maps to VT_BSTR
				m_dispDriver.SetProperty(dispid, VT_BSTR, pItem->m_strProp);
				break;
			case data_EnumString:
			{
				VARIANT	var;
				BYTE	rgbParams[2];
				WORD	wFlags;

				COleRef<IPerPropertyBrowsing>	srpPPB;
				
				if (!GetPerPropertyBrowsing(m_dispDriver.m_lpDispatch, &srpPPB))
					break;

				ASSERT(srpPPB);
				hr = srpPPB->GetPredefinedValue(dispid, pItem->unionValue.m_dwValue, &var);
				if (FAILED(hr))
					break;

				if (var.vt & VT_BYREF)
				{
					var.vt &= ~VT_BYREF;
					var.vt |= VT_MFCBYREF;
				}

				ASSERT(_tcslen(VTS_VARIANT) == 1);

				rgbParams[0] = *VTS_VARIANT;
				rgbParams[1] = 0;
				wFlags = (WORD)((var.vt == VT_DISPATCH) ?
					DISPATCH_PROPERTYPUTREF : DISPATCH_PROPERTYPUT);

				try
				{
					m_dispDriver.InvokeHelper(dispid, wFlags, VT_EMPTY, NULL,
						rgbParams, &var);
				}
				catch(COleDispatchException* e)
				{
					hr = e->m_scError;
					strError = e->m_strDescription;
				}
				catch(...)
				{
					hr = E_FAIL;
				}

				::VariantClear(&var);
			}
				break;
			case data_EnumInt2:
				m_dispDriver.SetProperty(dispid, VT_I2, pItem->unionValue.m_nValue);
				break;
			case data_EnumInt4:
				m_dispDriver.SetProperty(dispid, VT_I4, pItem->unionValue.m_nValue);
				break;
			case data_Date:
			{
				DATE	date;

				hr = VarDateFromStr(T2OLE(pItem->m_strProp), s_lcid,
					LOCALE_NOUSEROVERRIDE, &date);
				if (S_OK == hr)
				{
					VARIANT	var;

					m_dispDriver.SetProperty(dispid, VT_DATE, date);
					// we need to get the property again, since
					// the date property is "converted" sometimes
					// i.e., from "13/12/96" to "12/13/96" so we need
					// to display the correct value (just as in VB)
					if (GetPropValue(pDisp, dispid, &var))
						GetPropValueForOtherTypes(&var, pItem);
				}
			}
				break;
			case data_Currency:
			{
				CURRENCY	currency;
				hr = VarCyFromStr(T2OLE(pItem->m_strProp), s_lcid,
					LOCALE_NOUSEROVERRIDE, &currency);
				if (S_OK == hr)
					m_dispDriver.SetProperty(dispid, VT_CY, &currency);
#if 0
				VARIANT	varSrc;
				VARIANT	varDest;
				DISPID	dispidNamed = DISPID_PROPERTYPUT;
				DISPPARAMS	dp = {&varDest, &dispidNamed, 1, 1};

				::VariantInit(&varSrc);
				V_VT(&varSrc) = VT_BSTR;
				V_BSTR(&varSrc) = pItem->m_strProp.AllocSysString();
				hr = ::VariantChangeType(&varDest, &varSrc, VARIANT_NOVALUEPROP,
					VT_CY);
				if (S_OK == hr)
				{
					hr = m_dispDriver.m_lpDispatch->Invoke(dispid, IID_NULL, 0,
						INVOKE_PROPERTYPUT, &dp, NULL, NULL, NULL);

					::VariantClear(&varDest);
				}
#endif
			}
				break;
		}
	}
    catch(COleDispatchException* e)
	{
		hr = e->m_scError;
		strError = e->m_strDescription;
	}
	catch(...)
	{
		hr = E_FAIL;
	}

	if (S_OK != hr)
	{
		if (strError.IsEmpty())
			strError.LoadString(IDS_PROP_ERROR_INVPROP);

		// do not use AfxMessageBox, since specifying the default window
		// as the owner causes a mouse up to be lost (sridharc)
		::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), strError, NULL,
			MB_OK | MB_ICONEXCLAMATION);

		// since we need to revert back, set this flag so that whoever has
		// called this method can call UpdateChangedProperties later (if this
		// flag is set).
		m_bUpdateLater = TRUE;
		ForcePropertyBrowserToshow();
	}

	return hr;
}

void COleAllPage::ForcePropertyBrowserToshow()
{
	// The property browser gets an OnActivate((nState == WA_INACTIVE))
	// when an error messagebox is displayed, which causes the Property
	// Browser to go away when the OK on the MessageBox is pressed
	// Since, we do not need this behavior (as the user might want to correct
	// the situation by typing/selecting the correct value
	// Hence, the following flag (which governs the visibility of the property
	// browser is set to true here.  The ShowEditControl() brings the focus back to
	// the property browser (especially the edit control).
	g_PropSheetVisible = TRUE;
	m_lbProps.ShowEditControl();
}

// This method gets the picture type and displays "(None)", "(Icon)" or one
// of the other picture types based on the picture type.
BOOL COleAllPage::GetPictureType(
	IDispatch*	pPictDisp,
	CString&	strProp)
{
	COleRef<IPicture>	srpPicture;
	SHORT				sType;
	
	ASSERT(pPictDisp);
	if (SUCCEEDED(pPictDisp->QueryInterface(IID_IPicture, (void**)&srpPicture))
		&& SUCCEEDED(srpPicture->get_Type(&sType)))
	{
		// Set appropriate string for the picture type
		switch(sType)
		{
			default:
			case PICTYPE_NONE:
			case PICTYPE_UNINITIALIZED:
				strProp.LoadString(IDS_PICTURETYPENONE);
				break;
				
			case PICTYPE_BITMAP:
				strProp.LoadString(IDS_PICTURETYPEBITMAP);
				break;
				
			case PICTYPE_METAFILE:
				strProp.LoadString(IDS_PICTURETYPEMETAFILE);
				break;
				
			case PICTYPE_ICON:
				strProp.LoadString(IDS_PICTURETYPEICON);
				break;
				
			case PICTYPE_ENHMETAFILE:
				strProp.LoadString(IDS_PICTURETYPEENHMETAFILE);
				break;
		}

		return TRUE;
	}

	return FALSE;
}

// Just a silly method to determine if the builder is an intrinsic builder
BOOL COleAllPage::IsIntrinsicBuilder(
	GUID	guidBuilder)
{
	COleRef<IBuilderWizardManager>	srpBWM;

	return ((S_OK == theApp.GetService(SID_SBuilderWizardManager, 
		IID_IBuilderWizardManager, (void**)&srpBWM)) &&
		(S_OK == srpBWM->DoesBuilderExist(guidBuilder)));
}

// The algorighm for determining the builder type to edit the property for
// the given dispid is given below.
void COleAllPage::DetermineBuilderType(
	DISPID		dispid,
	CPropItem*	pItem)
{
	// ALL THIS IS DONE SO THAT WHEN WE NEED TO INVOKE A BUILDER, WE KNOW
	// WHAT TYPE IT IS.
	// Assume that we do not have a builder for this.
	// Get the IProvidePropertyBuilder interface from the dispatch interface
	// Call MapPropertyToBuilder to get the builder guid.
	// If we have a multiple selection and the above call returns a value
	//  specifying that it edits objects directly, then bail out.
	// Else if we have either a multiple selection or a single selection, but
	//  there is an internal builder which can be used, so set the control type
	//  value to that type and return.
	// Else if a standard builder is supported, just make sure that that builder
	//  exists by querying the BuilderWizardManager and then set its value to
	//  that control type.
	// If all the above fails, try to find out if a PerPropertyBrowsing interface
	//  exists and if so, set the control type to that value
	// If not supported by a standard builder or by PerPropertyBrowsing, and is of type
	//	Color, Font, or Picture, then set control type to Internal Builder which will use
	//  an internal builder for these properties

	HRESULT	hr;
	BOOL	bCtlTypeSet = FALSE;
	GUID	guidBuilder = GUID_NULL;

	pItem->m_ctrlType = ctrl_Unknown;	// initialize return value

	// look for an intrinsic builder first
	if (pItem->m_dataType == data_Color ||
		pItem->m_dataType == data_Font ||
		pItem->m_dataType == data_Picture ||
		pItem->m_dataType == data_Path)
	{
		switch(pItem->m_dataType)
		{
			case data_Color:
				guidBuilder = CATID_ColorBuilder;
				break;
			case data_Font:
				guidBuilder = CATID_FontBuilder;
				break;
			case data_Picture:
				guidBuilder = CATID_PictureBuilder;
				break;
			case data_Path:
				guidBuilder = CATID_URLBuilder;
				break;
		}
		if (IsIntrinsicBuilder(guidBuilder))
		{
			pItem->m_ctrlType = ctrl_BuilderWizardManagerIntrinsic;
			return;
		}
		else if (pItem->m_dataType != data_Path)
		{
			// No support for dataPath properties
			// No builder support so use internal Builder.
			// m_ctrlType will be reset if property can be mapped to a page
			// Internal Builder support should only be used if there are no
			// alternatives
			pItem->m_ctrlType = ctrl_BuilderInternal;
		}
	}

	int			iNumCtlsSupporingInternalBuilder = 0;
	int			iNumCtlsSupporingStandardBuilder = 0;
	int			iIndexInternalBuilder = -1;
	int			iIndexStandardBuilder = -1;
	ControlType	ctrlType;				// type of control required on grid

	ctrlType = ctrl_Unknown;	// this is a different control type

	for (ULONG i = 0; i < m_ulCountControls; i++)
	{
		ASSERT(m_pprgDisp[i]);
		ctrlType = GetControlBuilderType(dispid, m_pprgDisp[i]);
		if (ctrlType == ctrl_BuilderProvidePropertyBuilder)
		{
			// increment the number of controls supporting internal builders
			iNumCtlsSupporingInternalBuilder++;
			// if we don't already have an index (in m_pprgDisp), store it
			if (-1 == iIndexInternalBuilder)
				iIndexInternalBuilder = i;
		}
		else if (ctrlType == ctrl_BuilderWizardManager)
		{
			// increment the number of controls supporting internal builders
			iNumCtlsSupporingStandardBuilder++;
			// if we don't already have an index (in m_pprgDisp), store it
			if (-1 == iIndexStandardBuilder)
				iIndexStandardBuilder = i;
		}
	}

	// if at least one control supports either of the builders proceed
	// else find out if IPerPropertyBrowsing is supported
	if ((iNumCtlsSupporingInternalBuilder > 0) ||
		(iNumCtlsSupporingStandardBuilder > 0))
	{
		if (iNumCtlsSupporingStandardBuilder > 0)
		{
			pItem->m_iIndex = iIndexStandardBuilder;// store this for later use
			pItem->m_ctrlType = ctrl_BuilderWizardManager;
		}
		else
		{
			pItem->m_iIndex = iIndexInternalBuilder;// store this for later use
			pItem->m_ctrlType = ctrl_BuilderProvidePropertyBuilder;
		}

		bCtlTypeSet = TRUE;
	}

	if (!bCtlTypeSet)
	{
		BOOL	bContinue;
		CLSID	clsidFirst;	// clsid of the first control
		CLSID	clsidCurrent;	// clsid of the current control in loop
		COlePage*	pPage;

		// try to find if there is a property page with the properties dialog
		// that supports editing of this property
		// make sure that all the controls maps to the same property page
		// by comparing their CLSIDs

		// first get the clsid of the first control
		ASSERT(m_pprgDisp[0]);
		COleRef<IPerPropertyBrowsing>	srpPPB;

		// if we can't get the IPerPropertyBrowsing interface or if a property
		// page is not found, set continue flag to false
		bContinue = (GetPerPropertyBrowsing(m_pprgDisp[0], &srpPPB) &&
			(S_OK == MapPropertyToPage(srpPPB, dispid, &clsidFirst)));

		if (bContinue)
		{
			// make sure that the page exists
			pPage = (COlePage*)COlePage::GetPropPage(clsidFirst);
			bContinue = (pPage != NULL);
		}

		if (bContinue && (m_ulCountControls > 1))
		{
			for (ULONG i = 1; bContinue && (i < m_ulCountControls); ++i)
			{
				ASSERT(m_pprgDisp[i]);
				COleRef<IPerPropertyBrowsing>	srpPPBInLoop;

				if (GetPerPropertyBrowsing(m_pprgDisp[i], &srpPPBInLoop))
				{
					hr = MapPropertyToPage(srpPPBInLoop, dispid, &clsidCurrent);
					bContinue = (hr == S_OK) &&
						IsEqualCLSID(clsidCurrent, clsidFirst);
					// since we have verified that a prop-page exists for the
					// clsidFirst, and since clsidCurrent is the same as
					// clsidFirst, no need to check here again
				}
				else
				{
					bContinue = FALSE;
				}
			}
		}

		if (bContinue)
			pItem->m_ctrlType = ctrl_BuilderPerPropertyBrowsing;
	}
}

// Gets the builder type for the given control and dispid
// Basically, this method finds out if the control has a builder
// for ediiting this property and if so does it prefers to do it directly.
// Or does this contorl support editing the property via the 
// BuilderWizardManager interface
ControlType COleAllPage::GetControlBuilderType(
	DISPID		dispid,
	IDispatch*	pControlDisp)
{
	ASSERT(pControlDisp);

	HRESULT			hr;
	long			lCtlBldType = 0;
	BSTR			bstrGuidBuilder;
	VARIANT_BOOL	bRetVal;
	ControlType		ctrlType = ctrl_Unknown;

	COleRef<IProvidePropertyBuilder>	srpPPropBldr;

	hr = pControlDisp->QueryInterface(IID_IProvidePropertyBuilder,
		(void**)&srpPPropBldr);
	if (S_OK == hr)
	{
		lCtlBldType = 0;
		bstrGuidBuilder = ::SysAllocString(L"");
		bRetVal = FALSE;
		hr = srpPPropBldr->MapPropertyToBuilder(dispid, &lCtlBldType, &bstrGuidBuilder,
			&bRetVal);
		if (S_OK == hr && bRetVal)
		{
			// if this builder edits objects directly, and if we
			// have a multiple selection, we do not need this builder
			if (!((m_ulCountControls > 1) && (lCtlBldType & CTLBLDTYPE_FEDITSOBJDIRECTLY)))
			{
				if (lCtlBldType & CTLBLDTYPE_FINTERNALBUILDER)
				{
					ctrlType = ctrl_BuilderProvidePropertyBuilder;
				}
				else if (lCtlBldType & CTLBLDTYPE_FSTDPROPBUILDER)
				{
					COleRef<IBuilderWizardManager>	srpBWM;
					
					GUID	guidBuilder = GUID_NULL;

					hr = theApp.GetService(SID_SBuilderWizardManager, 
						IID_IBuilderWizardManager, (void**)&srpBWM);
					if (S_OK == hr &&
						SUCCEEDED(::CLSIDFromString(bstrGuidBuilder, &guidBuilder)))
					{
						hr = srpBWM->DoesBuilderExist(guidBuilder);
						if (S_OK == hr)
							ctrlType = ctrl_BuilderWizardManager;
					}
				}
				else
				{
					ASSERT(FALSE); // should never happen usually!
				}
			}
		}

		::SysFreeString(bstrGuidBuilder);
	}

	return ctrlType;
}

// This method sets up the dispparams and calls the builder
BOOL COleAllPage::CallGeneralPropertyBuilder(
	IDispatch*	pAppDisp,		// application dispatch ptr
	IDispatch*	pBuilderDisp,	// url builder dispatch ptr
	HWND		hwndOwner,		// hwnd of the owner
	DISPID		diExecute,		// dispatch method id
	VARIANT*	pvarValue)		// in/out value
{
	ASSERT(pAppDisp && pBuilderDisp && pvarValue);

#define	NUM_PARAMS_LOCAL_COLOR	4

	BOOL			bMethodReturn = FALSE;
	HRESULT			hr;

	// set up the dispparms.  They are:
	// 1. app object's IDispatch,
	// 2. hwndPromptOwner,
	// 3. IServiceProvider
	// 4. current color value (variant dword)
	// 5. return value (variant bool)

	UINT		cArgs = NUM_PARAMS_LOCAL_COLOR;
	DISPPARAMS	dp;

	dp.rgvarg = new VARIANTARG[cArgs];
	dp.cArgs = cArgs;
	dp.rgdispidNamedArgs = NULL;
	dp.cNamedArgs = 0;

	// since the machine pushes items on stack, store them in reverse order

	// param 1 (app object's IDispatch)
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_DISPATCH;
	dp.rgvarg[cArgs].pdispVal = pAppDisp;

	// param 2 (hwndPromptOwner)
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_I4;
	dp.rgvarg[cArgs].lVal = (long)hwndOwner;

	// param 3 (IServiceProvider interface)
	::VariantInit(&(dp.rgvarg[--cArgs]));
	IServiceProvider *pSP;
	theApp.m_pAutoApp->ExternalQueryInterface(
			&IID_IServiceProvider, (void**)&pSP);
	dp.rgvarg[cArgs].vt = VT_UNKNOWN;
	dp.rgvarg[cArgs].punkVal = (IUnknown*)(IServiceProvider*)pSP;

	// param 4 (current/new property value) 
	// this is an in/out param, therefore tag VT_BYREF
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_BYREF | VT_VARIANT;
	dp.rgvarg[cArgs].pvarVal = pvarValue;

	VARIANT	varReturn;

	::VariantInit(&varReturn);

	hr = pBuilderDisp->Invoke(diExecute, IID_NULL, s_lcid,
		DISPATCH_METHOD, &dp, &varReturn, NULL, NULL);
	ASSERT(SUCCEEDED(hr));
	if (SUCCEEDED(hr)) 
	{
		// Set return value
		ASSERT(VT_BOOL == V_VT(&varReturn));
		bMethodReturn = V_BOOL(&varReturn);
		::VariantClear(&varReturn);
	}

	// dp.rgvarg array is indexed from 0
	// do not clear the pAppDisp -- the caller needs it!
	::VariantClear(&(dp.rgvarg[1]));	// hwndOwner
	::VariantClear(&(dp.rgvarg[2]));	// IServiceProvider
	// do not clear the pvarValue -- the caller needs it!

	delete [] dp.rgvarg;

	return bMethodReturn;
}

// This method sets up the dispparams and calls the URL Builder
BOOL COleAllPage::CallURLBuilder(
	IDispatch*	pAppDisp,		// application dispatch ptr
	IDispatch*	pControlDisp,	// (first selected) control dispatch ptr
	IDispatch*	pBuilderDisp,	// url builder dispatch ptr
	HWND		hwndOwner,		// hwnd of the owner
	DISPID		diExecute,		// dispatch method id
	VARIANT*	pvarValue)		// in/out value
{
	ASSERT(pAppDisp && pControlDisp && pBuilderDisp && pvarValue);

#define	NUM_PARAMS_LOCAL_URL	10

	BOOL			bMethodReturn = FALSE;
	VARIANT_BOOL	bRet = FALSE;
	HRESULT			hr;

	// set up the dispparms.  They are:
	// 1. app object's IDispatch,
	// 2. hwndPromptOwner,
	// 3. service provider's IUnknown,
	// 4. current url value (variant (bstr))
	// 5. base url (bstr)
	// 6. additional filters (bstr)
	// 7. custom title (bstr)
	// 8. target frame value (variant (bstr))
	// 9. flags
	// 10. return value (variant bool)
	
	USES_CONVERSION;

	UINT		cArgs = NUM_PARAMS_LOCAL_URL;
	DISPPARAMS	dp;
	long		lFlags = URLP_EDITURLTITLE | URLP_ROOTRELATIVEURLTYPE;

	CString		strBaseURL;
	BSTR		bstrBaseURL;

	// get the base url if possible
	COleRef<IOleObject>		srpOleObject;

	// IT IS OKAY IF WE DO NOT GET THE BASE URL, JUST CONTINUE
	// get the client site from the IDispatch of the control
	hr = pControlDisp->QueryInterface(IID_IOleObject, (void**)&srpOleObject);
	if (S_OK == hr)
	{
		COleRef<IOleClientSite>	srpOleClientSite;

		hr = srpOleObject->GetClientSite(&srpOleClientSite);
		if (S_OK == hr)
		{
			COleRef<IMoniker>	srpMoniker;

			// get the moniker from the client site
			hr = srpOleClientSite->GetMoniker(OLEGETMONIKER_ONLYIFTHERE,
				OLEWHICHMK_CONTAINER, &srpMoniker);
			if (S_OK == hr)
			{
				COleRef<IBindCtx>	srpbc;

				hr = ::CreateBindCtx(0, &srpbc);
				if (S_OK == hr)
				{
					LPOLESTR	pszName;

					// get the base url from the moniker
					hr = srpMoniker->GetDisplayName(srpbc, 0, &pszName);
					if (S_OK == hr)
					{
						// copy the base url into a temp string
						strBaseURL = OLE2T(pszName);
						// free the memory
						::CoTaskMemFree((void*)pszName);
					}
				}
			}
		}
	}

	dp.rgvarg = new VARIANTARG[cArgs];
	dp.cArgs = cArgs;
	dp.rgdispidNamedArgs = NULL;
	dp.cNamedArgs = 0;

	// since the machine pushes items on stack, store them in reverse order

	// param 1 (app object's IDispatch)
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_DISPATCH;
	dp.rgvarg[cArgs].pdispVal = pAppDisp;

	// param 2 (hwndPromptOwner)
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_I4;
	dp.rgvarg[cArgs].lVal = (long)hwndOwner;

	// param 3 (service provider) 
	::VariantInit(&(dp.rgvarg[--cArgs]));	
	dp.rgvarg[cArgs].vt = VT_UNKNOWN;	// TODO$
	dp.rgvarg[cArgs].punkVal = NULL;	// for the time being pass NULL

	// param 4 (current/new url value) 
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_BYREF | VT_VARIANT;// in/out param, add tag VT_BYREF
	dp.rgvarg[cArgs].pvarVal = pvarValue;

	// param 5 (base url)
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_BSTR;
	if (strBaseURL.IsEmpty())
	{
		dp.rgvarg[cArgs].bstrVal = NULL;
	}
	else
	{
		bstrBaseURL = strBaseURL.AllocSysString();
		dp.rgvarg[cArgs].bstrVal = bstrBaseURL;
	}

	// param 6 (additional filters)
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_BSTR;
	dp.rgvarg[cArgs].bstrVal = NULL;

	// param 7 (custom title)
	// no custom title since we are editing the url
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_BSTR;
	dp.rgvarg[cArgs].bstrVal = NULL;

	// param 8 (target frame value) 
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_BYREF | VT_VARIANT;// in/out param, add tag VT_BYREF
	dp.rgvarg[cArgs].pvarVal = NULL;

	// param 9 (flags)
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_BYREF | VT_I4;	// in/out param, add tag VT_BYREF
	dp.rgvarg[cArgs].plVal = &lFlags;

	// param 10 (return value)
	::VariantInit(&(dp.rgvarg[--cArgs]));
	dp.rgvarg[cArgs].vt = VT_BYREF | VT_BOOL;// in/out param, add tag VT_BYREF
	dp.rgvarg[cArgs].pboolVal = &bRet;

	VARIANT	varReturn;

	::VariantInit(&varReturn);

	hr = pBuilderDisp->Invoke(diExecute, IID_NULL, s_lcid,
		DISPATCH_METHOD, &dp, &varReturn, NULL, NULL);
	ASSERT(SUCCEEDED(hr));
	if (SUCCEEDED(hr)) {
		// check the return value
		ASSERT(VT_I4 == V_VT(&varReturn));
		if (V_I4(&varReturn) == S_OK) {
			if (bRet)
				bMethodReturn = TRUE;
		}

		::VariantClear(&varReturn);
	}

	// dp.rgvarg array is indexed from 0
	// do not clear the pAppDisp -- the caller needs it!
	::VariantClear(&(dp.rgvarg[1]));	// hwndOwner
	// do not clear the IServiceProvider -- it is NULL for the time being
	// do not clear the pvarValue -- the caller needs it!
	::VariantClear(&(dp.rgvarg[4]));	// bstrBaseURL
	::VariantClear(&(dp.rgvarg[5]));	// additional filters
	::VariantClear(&(dp.rgvarg[6]));	// custom title
	::VariantClear(&(dp.rgvarg[7]));	// target frame value
	::VariantClear(&(dp.rgvarg[8]));	// flags
	::VariantClear(&(dp.rgvarg[9]));	// return value

	delete [] dp.rgvarg;

	return bMethodReturn;
}

// This method saves the modified property for all the selected controls
// It is only called when any of the Builders are executed to modify the property
// REVIEW:  Try to merge this and the Apply() method
HRESULT COleAllPage::SavePropertyForAllControls(
	VARIANT*	pvarValue,
	CPropItem*	pItem)
{
	HRESULT	hr = S_OK;
	WORD	wFlags;
	DISPID	dispid;
	CString	strError;

	COleDispatchDriver	dispDriver;

	m_bUpdateLater = FALSE;

	// tell IPropertyNotifySink::OnChanged that we are updating properties
	// so that all refreshing happen later
	m_bUpdatingProps = TRUE;

	m_pSlob->BeginUndo(IDS_UNDO_OLE_PROPS);

	if (pvarValue->vt & VT_BYREF)
	{
		pvarValue->vt &= ~VT_BYREF;
		pvarValue->vt |= VT_MFCBYREF;
	}
	
	wFlags = (WORD)((pvarValue->vt == VT_DISPATCH) ?
		DISPATCH_PROPERTYPUTREF : DISPATCH_PROPERTYPUT);

	try
	{
		USES_CONVERSION;

		// update the property of each control
		for (ULONG i= 0; i < m_ulCountControls; i++)
		{
			ASSERT(m_pprgDisp[i]);
			dispDriver.AttachDispatch(m_pprgDisp[i], FALSE);
			dispid = pItem->m_pDispid[i];

			switch(pItem->m_dataType)
			{
				case data_Path:
					ASSERT(pvarValue->vt == VT_BSTR);
					pItem->m_strProp = OLE2T(pvarValue->bstrVal);
					// ApplyProperty will clear the ErrorInfo
					ApplyProperty(dispid, pItem, m_pprgDisp[i]);
					break;
				case data_Color:
					// Clear ErrorInfo object before we call SetProperty
					::SetErrorInfo(0L, NULL);

					ASSERT(pvarValue->vt == VT_I4);
					dispDriver.SetProperty(dispid, pvarValue->vt, pvarValue->lVal);
					GetPropValueForOtherTypes(pvarValue, pItem);
					break;
				case data_Picture:
				{
					// Clear ErrorInfo object before we call SetProperty
					::SetErrorInfo(0L, NULL);

					ASSERT(pvarValue->vt == VT_DISPATCH);
					dispDriver.SetProperty(dispid, pvarValue->vt, V_DISPATCH(pvarValue));

					// get the new value and get the picture type
					VARIANT	varNewValue;
					if (GetPropValue(dispDriver.m_lpDispatch, dispid, &varNewValue))
					{
						GetPropValueForOtherTypes(&varNewValue, pItem);
						::VariantClear(&varNewValue);
					}
				}
					break;
				case data_Font:
					break;
				default:
					// ApplyProperty will clear the ErrorInfo
					ApplyProperty(dispid, pItem, m_pprgDisp[i]);
					break;
			}
		}	
	}
	catch(COleDispatchException* e)
	{
		hr = e->m_scError;
		strError = e->m_strDescription;
	}
	catch(...)
	{
		hr = E_FAIL;
	}

	if (S_OK != hr)
	{
		if (strError.IsEmpty())
			strError.LoadString(IDS_PROP_ERROR_INVPROP);

		// do not use AfxMessageBox, since specifying the default window
		// as the owner causes a mouse up to be lost (sridharc)
		::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), strError, NULL, MB_OK);
		ForcePropertyBrowserToshow();
	}

	// EndUndo takes a flag which is TRUE to abort the Undo Record, & FALSE 
	// to keep the undo record
	m_pSlob->EndUndo(!(S_OK == hr));

	m_bUpdatingProps = FALSE;

	// Some property was changed, so we need to update.
	UpdateChangedProperties();

	return hr;
}


//
//	Description:
// 	displays CColorDialog setting initial color to property's current value
//		returns selected color
//		converts from OLE_COLOR to COLORREF and back
//
//	Arguments:
//		OLE_COLOR olecolor - INOUT - in == current color
//									out == selected color
//
//	Return (BOOL): TRUE if color selected, FALSE otherwise
//
static BOOL InternalColorBuilder(OLE_COLOR& olecolor)
{
	COLORREF	clrref;

	// Translate OLE_COLOR to COLORREF
	if (SUCCEEDED(OleTranslateColor(olecolor, NULL, &clrref)))
	{
		// Display Color Dialog
		CColorDialog	colorDlg(clrref, 0, ::AfxGetMainWnd());
		if (colorDlg.DoModal() == IDOK)
		{
			// No translation needed from COLORREF to OLE_COLOR, because
			// COLORREF is a subset of OLE_COLOR.  And the Common Color
			// Dialog does not allow you to choose System Colors.
			// So we will lose system color info since we dont
			// have a way to select them.
			olecolor = colorDlg.GetColor();
			return TRUE;
		}
	}

	return FALSE;
}


//
//	Description:
//		Display CFontDialog setting initial font to property's current value
//		returns selected font.  Modifies IFontDisp properties to contain new
//		font info.
//
//	Arguments:
//		IFontDispatch IN - IDispatch for IFont
//
//	Return (BOOL): TRUE if font selected, FALSE otherwise
//
static BOOL InternalFontBuilder(
	IDispatch*	pIFontDisp)
{
	COleRef<IFont> srpFont;
	HRESULT hr = pIFontDisp->QueryInterface(IID_IFont, (void**)&srpFont);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return FALSE;

	HFONT hFont;  // owned by IFont so dont Delete	
	srpFont->get_hFont(&hFont);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return FALSE;
	
	// Create CFont from hFont
	// dont delete pFont.  It will be cleaned up during Idel Message processing
	CFont* pFont = CFont::FromHandle(hFont);
	ASSERT(pFont);
	if (pFont == NULL)
		return FALSE;

	// Get LOGFONT from CFont	
	LOGFONT logfont;
	if (!pFont->GetLogFont(&logfont))
	{
		ASSERT(!_T("CFont::CreateIndirect FAILED"));
		return FALSE;
	}
	
	// Display Font Dialog	
	C3dFontDialog FontDlg(&logfont, CF_SCREENFONTS | CF_ENABLETEMPLATE | CF_EFFECTS |
						CF_NOVECTORFONTS | CF_NOOEMFONTS, NULL, ::AfxGetMainWnd());

	FontDlg.m_cf.hInstance = ::AfxGetInstanceHandle();
	FontDlg.m_cf.lpTemplateName = MAKEINTRESOURCE(IDD_DIALOGFONT);
	
	if (FontDlg.DoModal() != IDOK)
		return FALSE;

	// Get Selected Font
	FontDlg.GetCurrentFont(&logfont);
	
	// Set Selected Font
	CString	strFontName = logfont.lfFaceName;
	BSTR	bstrFontName = strFontName.AllocSysString();

	VERIFY(SUCCEEDED(srpFont->put_Name(bstrFontName)));
	VERIFY(SUCCEEDED(srpFont->put_Weight((short)logfont.lfWeight)));
	VERIFY(SUCCEEDED(srpFont->put_Charset(logfont.lfCharSet)));
	VERIFY(SUCCEEDED(srpFont->put_Italic(logfont.lfItalic)));
	VERIFY(SUCCEEDED(srpFont->put_Underline(logfont.lfUnderline)));
	VERIFY(SUCCEEDED(srpFont->put_Strikethrough(logfont.lfStrikeOut)));

	// Get DC so I can get DeviceCaps
	CDC* pDC = ::AfxGetMainWnd()->GetDC();
	ASSERT(pDC);
	if (pDC == NULL)
		return FALSE;
	
	// Get ppi
	int ppi = pDC->GetDeviceCaps(LOGPIXELSY);
	::AfxGetMainWnd()->ReleaseDC(pDC);
	
	// calculate Font Size
	CY cySize;
	cySize.Lo = abs(logfont.lfHeight) * 720000 / ppi;
	cySize.Hi = 0;
	
	// Set new Font Size
	VERIFY(SUCCEEDED(srpFont->put_Size(cySize)));

	return TRUE;
}

//
//	Description:
//		Creates IPictureDisp from chosen file.  Uses OleLoadPicture to create the
//		IPictureDisp.  OleLoadPicture requires an IStream on the file.  Uses
//		CreateStreamOnHGlobal and copies file contents into stream.
//		Gets type from IPicture and sets appropriate Filter in File Open dialog
//
//	Arguments:
//		IPictureDisp** ppIPictureDisp: INOUT - in == IPictureDisp of current Picture
//												out == IPictureDisp of new picture
//
//	Return (BOOL): TRUE on SUCCESS, FALSE otherwise
//
static BOOL InternalPictureBuilder(
	IPictureDisp**	ppIPictureDisp)
{
	ASSERT(ppIPictureDisp);

	HRESULT hr;
	SHORT	sType = PICTYPE_NONE;
	if(NULL != *ppIPictureDisp)
	{
		// Get IPicture
		COleRef<IPicture> srpPicture;
		hr = (*ppIPictureDisp)->QueryInterface(IID_IPicture, (void**)&srpPicture);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return FALSE;

		// GetType
		hr = srpPicture->get_Type(&sType);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return FALSE;
	}
	
	// Set appropriate Filter & Dialog Title
	CString strFilter;
	CString strDialogTitle;
#if 0
	// to fix IS Bug # 7099
	switch(sType)
	{
		default:
		case PICTYPE_NONE:
		case PICTYPE_UNINITIALIZED:
			strFilter.LoadString(IDS_PICTUREFILTER);
			strDialogTitle.LoadString(IDS_PICTURETITLE);
			break;
			
		case PICTYPE_BITMAP:
			strFilter.LoadString(IDS_BITMAPFILTER);
			strDialogTitle.LoadString(IDS_BITMAPTITLE);
			break;
			
		case PICTYPE_METAFILE:
			strFilter.LoadString(IDS_METAFILEFILTER);
			strDialogTitle.LoadString(IDS_METAFILETITLE);
			break;
			
		case PICTYPE_ICON:
			strFilter.LoadString(IDS_ICONFILTER);
			strDialogTitle.LoadString(IDS_ICONTITLE);
			break;
			
		case PICTYPE_ENHMETAFILE:
			strFilter.LoadString(IDS_ENHMETAFILEFILTER);
			strDialogTitle.LoadString(IDS_ENHMETAFILETITLE);
			break;
	}	
#else	// 0
	strFilter.LoadString(IDS_PICTUREFILTER);
	strDialogTitle.LoadString(IDS_PICTURETITLE);
#endif	// 0
	
	// Choose file containing new picture
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, 
						strFilter, ::AfxGetMainWnd());
	fileDlg.m_ofn.lpstrTitle = strDialogTitle; // set title
	if (fileDlg.DoModal() != IDOK)
		return FALSE;
	
	// Need to copy file contents into a Stream
	// Create Stream
	COleRef<IStream> srpStream;
	hr = ::CreateStreamOnHGlobal(NULL, TRUE, &srpStream);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return FALSE;
		
	// Seek to start of stream
	LARGE_INTEGER li;
	LISet32(li, 0);
	srpStream->Seek(li, STREAM_SEEK_SET, NULL);
	// Copy file contents to stream
	try
	{
		BYTE	rgBuf[512];
		UINT	cbRead;
		ULONG	cbWritten;
		CFile 	file(fileDlg.GetPathName(), CFile::modeRead);
		
		// Copy bytes from File to Stream
		while(cbRead = file.Read(rgBuf, 512))
		{
			hr = srpStream->Write(rgBuf, cbRead, &cbWritten);
			ASSERT(SUCCEEDED(hr));
			ASSERT(cbRead == cbWritten);
			if (FAILED(hr) || cbRead != cbWritten)
				return FALSE;
		}
	}
	catch(...) 
	{
		return FALSE;
	}

	// Seek to start of stream before calling OleLoadPicture
	hr = srpStream->Seek(li, STREAM_SEEK_SET, NULL);
	ASSERT(SUCCEEDED(hr));

	IPictureDisp*	pIPictDispTemp;

	// Create IPictureDisp from IStream
	//hr = OleLoadPicture(srpStream, 0, TRUE, IID_IPictureDisp, (void**)ppIPictureDisp);
	hr = OleLoadPicture(srpStream, 0, TRUE, IID_IPictureDisp,
		(void**)&pIPictDispTemp);
	if (SUCCEEDED(hr))
	{
		// Releae the old IPictureDisp and update with the new one
		if(NULL != *ppIPictureDisp)
			(*ppIPictureDisp)->Release();

		// copy the new IPictureDisp in its place
		*ppIPictureDisp = pIPictDispTemp;
	}
	
	return SUCCEEDED(hr);
}

// Executes the PerPropertyBrowsing builder that basically "flips" the user
// to a different property page in the Property Browser so that the user
// can edit the property in that page.
HRESULT COleAllPage::ExecutePPBBuilder(
	IDispatch*	pDisp,
	DISPID		dispid)
{
	CLSID	clsid;

	COleRef<IPerPropertyBrowsing>	srpPPB;

	ASSERT(pDisp);
	if (pDisp && GetPerPropertyBrowsing(pDisp, &srpPPB) &&
		SUCCEEDED(MapPropertyToPage(srpPPB, dispid, &clsid)))
	{
		COlePage* pPage = (COlePage*)COlePage::GetPropPage(clsid);
		if (pPage)
		{
			CString strPage;
			CString	strGeneral;

			pPage->GetPageName(strPage);
			// HACK: Keep OLE controls from using "General" as Tab text.
			//		 Use "Control" instead.
			strGeneral.LoadString(IDS_PROP_GENERAL);
			if (strGeneral.CompareNoCase(strPage) == 0)
				strPage.LoadString(IDS_PROP_CONTROL);

			if (m_pSheetWnd->FlipToPage(strPage))
			{
				pPage->EditProperty(dispid);
			}
			else
			{
				CString	strFmt;
				CString	strError;

				strFmt.LoadString(IDS_E_CANNOTGOTOPAGE);
				strError.Format(strFmt, strPage);
				::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), strError, NULL,
					MB_OK | MB_ICONEXCLAMATION);
			}

			return S_OK;
		}
	}

	return E_FAIL;
}

// Executes the internal builders for color, font and picture
// properties (if not builder is supported)
HRESULT COleAllPage::ExecuteInternalBuilder(
	CPropItem*	pItem,
	VARIANT*	pVarValue)
{
	HRESULT	hr = E_NOINTERFACE;
	BOOL	bSaveProps = FALSE;

	switch(pItem->m_dataType)
	{
		case data_Color:
		{
			// check the return value
			if (V_VT(pVarValue) == VT_UI4)	// some controls use UI4 and
				V_VT(pVarValue) = VT_I4;	// COleDispDriver::SetProperty fails on that

			ASSERT(VT_I4 == V_VT(pVarValue));
			OLE_COLOR olecolor = V_I4(pVarValue);
			// varValue passed by reference
			if (InternalColorBuilder(olecolor))
			{
				V_I4(pVarValue) = olecolor;
				bSaveProps = TRUE;
			}
			break;
		}
		
		case data_Font:
			// check the return value
			ASSERT(VT_DISPATCH == V_VT(pVarValue));
			bSaveProps = InternalFontBuilder(V_DISPATCH(pVarValue));
			break;

		case data_Picture:
		{
			ASSERT(VT_DISPATCH == V_VT(pVarValue));
			IPictureDisp* pPictDisp = (IPictureDisp*)V_DISPATCH(pVarValue);
			if (InternalPictureBuilder(&pPictDisp))	// get new IPictureDisp
			{
				V_DISPATCH(pVarValue) = pPictDisp;	// set new IPictureDisp
				bSaveProps = TRUE;
			}
			break;
		}
		
		default:
			ASSERT(FALSE);	// should never happen!
			break;
	}
	
	if (bSaveProps)
		hr = SavePropertyForAllControls(pVarValue, pItem);

	return hr;
}

// Executes the intrinsic (color, font, picture or path) builders
HRESULT COleAllPage::ExecuteBWMIntrinsicBuilder(
	CPropItem*	pItem,
	VARIANT*	pVarValue)
{
	HRESULT		hr = E_NOINTERFACE;
	GUID		guidBuilder = GUID_NULL;
	DISPID		dispidExecute;
	LPOLESTR	szExecute = (L"Execute");

	switch(pItem->m_dataType)
	{
		case data_Color:
			guidBuilder = CATID_ColorBuilder;
			break;
		case data_Font:
			guidBuilder = CATID_FontBuilder;
			break;
		case data_Picture:
			guidBuilder = CATID_PictureBuilder;
			break;
		case data_Path:
			guidBuilder = CATID_URLBuilder;
			break;
		default:
			ASSERT(FALSE);	// should never happen!
			return hr;
			break;
	}

	COleRef<IDispatch>				srpAppDisp;
	COleRef<IBuilderWizardManager>	srpBWM;
	COleRef<IDispatch>				srpBuilderDisp;

	if (SUCCEEDED(theApp.GetService(SID_SBuilderWizardManager, 
		IID_IBuilderWizardManager, (void**)&srpBWM)) &&

		S_OK == srpBWM->GetBuilder(guidBuilder,
			BLDGETOPT_FAUTOMAPGUID,
			GetSafeHwnd(), &srpAppDisp, NULL, 
			IID_IDispatch, (IUnknown**)&srpBuilderDisp) &&

		SUCCEEDED(srpBuilderDisp->GetIDsOfNames(IID_NULL,
			&szExecute, 1, s_lcid, &dispidExecute)))
	{
		CWnd*	pMainWnd = AfxGetMainWnd();
		ASSERT(pMainWnd);
		HWND	hwndOwner = pMainWnd->GetSafeHwnd();

		// disable the main window
		srpBWM->EnableModeless(FALSE);
		switch(pItem->m_dataType)
		{
			case data_Color:
			case data_Font:
			case data_Picture:
				if (CallGeneralPropertyBuilder(srpAppDisp,
					srpBuilderDisp, hwndOwner, dispidExecute, pVarValue))
					hr = S_OK;
				break;
			case data_Path:
				if (CallURLBuilder(srpAppDisp, m_pprgDisp[0],
					srpBuilderDisp, hwndOwner, dispidExecute, pVarValue))
					hr = S_OK;
				break;
			default:
				ASSERT(FALSE);	// should never happen!
				break;
		}
		// enable the main window
		srpBWM->EnableModeless(TRUE);
	}

	if (S_OK == hr)
		hr = SavePropertyForAllControls(pVarValue, pItem);

	return hr;
}

// Executes the BuilderWizardManager and ProvidePropertyBuilder builders
HRESULT COleAllPage::ExecuteBWMAndPPBBuilder(
	CPropItem*	pItem,
	DISPID		dispid,
	VARIANT*	pVarValue)
{
	HRESULT				hr;
	long				lCtlBldType = 0;
	BSTR				bstrGuidBuilder;
	VARIANT_BOOL		bRetVal;
	COleDispatchDriver	dispDriver;

	COleRef<IProvidePropertyBuilder>	srpPPropBldr;

	dispDriver.m_lpDispatch = NULL;

	ASSERT((pItem->m_iIndex != -1) && (pItem->m_iIndex < m_ulCountControls) &&
		(NULL != m_pprgDisp[pItem->m_iIndex]));
	hr = m_pprgDisp[pItem->m_iIndex]->QueryInterface(
		IID_IProvidePropertyBuilder, (void**)&srpPPropBldr);
	if (S_OK == hr)
	{
		bstrGuidBuilder = ::SysAllocString(L"");
		bRetVal = FALSE;
		hr = srpPPropBldr->MapPropertyToBuilder(dispid, &lCtlBldType,
			&bstrGuidBuilder, &bRetVal);
		if (S_OK == hr && bRetVal)
			dispDriver.AttachDispatch(m_pprgDisp[pItem->m_iIndex], FALSE);
	}

	if (dispDriver.m_lpDispatch)
	{
		COleRef<IDispatch>	srpAppDisp;

		ASSERT(theApp.m_pAutoApp);
		hr = theApp.m_pAutoApp->ExternalQueryInterface(&IID_IDispatch,
			(void**)&srpAppDisp);
		ASSERT(SUCCEEDED(hr));
		if (S_OK == hr)
		{
			hr = E_NOINTERFACE;

			if (pItem->m_ctrlType == ctrl_BuilderProvidePropertyBuilder)
			{
				VARIANT_BOOL	bRetVal = FALSE;
				hr = srpPPropBldr->ExecuteBuilder(dispid, bstrGuidBuilder, srpAppDisp,
					(long)(HWND)GetSafeHwnd(), pVarValue, &bRetVal);
			}
			else if (pItem->m_ctrlType == ctrl_BuilderWizardManager)
			{
				COleRef<IDispatch>				srpAppDisp2;
				COleRef<IBuilderWizardManager>	srpBWM;
				COleRef<IDispatch>				srpBuilderDisp;
				
				GUID		guidBuilder = GUID_NULL;
				DISPID		dispidExecute;
				LPOLESTR	szExecute = (L"Execute");

				if (SUCCEEDED(theApp.GetService(SID_SBuilderWizardManager, 
					IID_IBuilderWizardManager, (void**)&srpBWM)) &&

					SUCCEEDED(::CLSIDFromString(bstrGuidBuilder, &guidBuilder)) &&
					
					S_OK == srpBWM->GetBuilder(guidBuilder,
						BLDGETOPT_FAUTOMAPGUID,
						GetSafeHwnd(), &srpAppDisp2, NULL, 
						IID_IDispatch, (IUnknown**)&srpBuilderDisp) &&

					SUCCEEDED(srpBuilderDisp->GetIDsOfNames(IID_NULL,
						&szExecute, 1, s_lcid, &dispidExecute)))
				{
					// setup the dispparms.  they are:
					// app object's IDispatch, hwndPromptOwner, 
					// current property value
					UINT		cArgs = 3;
					DISPPARAMS	dispparams;
					VARIANT		varReturn;

					dispparams.rgvarg = new VARIANTARG[cArgs];
					dispparams.cArgs = cArgs;

					dispparams.rgdispidNamedArgs = NULL;
					dispparams.cNamedArgs = 0;

					// param 1. app object
					::VariantInit(&(dispparams.rgvarg[--cArgs]));
					dispparams.rgvarg[cArgs].vt = VT_DISPATCH;
					// the srpAppDisp.Disown() transfers the addref'd pointer
					// from srpAppDisp to the variant.  VariantClear will
					// do the necessary release
					dispparams.rgvarg[cArgs].pdispVal = srpAppDisp2.Disown();

					// param 2. hwndPromptOwner
					::VariantInit(&(dispparams.rgvarg[--cArgs]));
					dispparams.rgvarg[cArgs].vt = VT_I4;
					dispparams.rgvarg[cArgs].lVal = (long)(HWND)GetSafeHwnd();

					// param 3. current property value
					::VariantInit(&(dispparams.rgvarg[--cArgs]));
					// since the current property value is an in/out 
					// parameter, it must also be tagged as a VT_BYREF
					dispparams.rgvarg[cArgs].vt = VT_BYREF|VT_VARIANT;
					dispparams.rgvarg[cArgs].pvarVal = pVarValue;

					::VariantInit(&varReturn);
					hr = srpBuilderDisp->Invoke(dispidExecute, IID_NULL, s_lcid, 
						DISPATCH_METHOD, &dispparams, &varReturn, NULL, NULL);
					if (SUCCEEDED(hr))
					{
						//determine the return value
						ASSERT(VT_BOOL == V_VT(&varReturn));
						bRetVal = V_BOOL(&varReturn);
						if (V_BOOL(&varReturn))
							hr = S_OK;
						else
							hr = S_FALSE;
						::VariantClear(&varReturn);
					}

					for (cArgs = 0; cArgs < 3; ++cArgs)
						::VariantClear(&(dispparams.rgvarg[cArgs]));
					delete [] dispparams.rgvarg;
				}
			}
			else
			{
				ASSERT(FALSE); // should never happen usually!
			}

			::SysFreeString(bstrGuidBuilder);

			if (S_OK == hr && !(lCtlBldType & CTLBLDTYPE_FEDITSOBJDIRECTLY))
				hr = SavePropertyForAllControls(pVarValue, pItem);
		}
	}

	return hr;
}

// This method is called when the user presses the ellipsis (...) button
// to execute the appropriate builder (top level method)
HRESULT COleAllPage::ExecuteBuilder(
	CPropItem*	pItem)
{
	ASSERT(pItem);
	if (NULL == pItem)
		return E_INVALIDARG;

	USES_CONVERSION;
	HRESULT		hr = E_FAIL;
	DISPID		dispid;

	m_dispDriver.AttachDispatch(m_pprgDisp[0], FALSE);
	switch (pItem->m_ctrlType)
	{
		case ctrl_BuilderPerPropertyBrowsing:
			hr = ExecutePPBBuilder(m_dispDriver.m_lpDispatch, pItem->m_pDispid[0]);
			break;
		case ctrl_BuilderWizardManager:
		case ctrl_BuilderProvidePropertyBuilder:
			ASSERT((pItem->m_iIndex != -1) && (pItem->m_iIndex < m_ulCountControls) &&
				(NULL != m_pprgDisp[pItem->m_iIndex]));
			if ((pItem->m_iIndex == -1) || (pItem->m_iIndex >= m_ulCountControls) ||
				(NULL == m_pprgDisp[pItem->m_iIndex]))
				return hr;
			// okay to fall through...
		case ctrl_BuilderWizardManagerIntrinsic:
		case ctrl_BuilderInternal:
		{
			VARIANT		varValue;
			IDispatch*	pDisp;
			DISPID		dispidCached;

			if ((pItem->m_ctrlType == ctrl_BuilderWizardManagerIntrinsic) ||
				(pItem->m_ctrlType == ctrl_BuilderInternal))
			{
				pDisp = m_pprgDisp[0];
				dispid = pItem->m_pDispid[0];
			}
			else
			{
				pDisp = m_pprgDisp[pItem->m_iIndex];
				dispid = pItem->m_pDispid[pItem->m_iIndex];
			}

			// get the current value of the property
			if (!GetPropValue(pDisp, dispid, &varValue))
				return hr;

			switch(pItem->m_ctrlType)
			{
				case ctrl_BuilderWizardManager:
				case ctrl_BuilderProvidePropertyBuilder:
					hr = ExecuteBWMAndPPBBuilder(pItem, dispid, &varValue);
					break;
				case ctrl_BuilderInternal:
					hr = ExecuteInternalBuilder(pItem, &varValue);
					break;
				case ctrl_BuilderWizardManagerIntrinsic:
					hr = ExecuteBWMIntrinsicBuilder(pItem, &varValue);
					break;
			}

			::VariantClear(&varValue);
		}
			break;
		default:
			ASSERT(FALSE);
			break;
	}

	return hr;
}

// When the user presses the "Del" key when the Picture property is selected,
// we "delete" the existing picture and set the property to NULL.
HRESULT COleAllPage::SetPictureToNone(
	CPropItem*	pItem)
{
	IDispatch*	pDispatch = m_pprgDisp[0];
	VARIANT		varValue;

	ASSERT(pDispatch && pItem);
	
	// get the current value of the property
	if (GetPropValue(pDispatch, pItem->m_pDispid[0], &varValue))
	{
		IPictureDisp*	pPictDisp;

		ASSERT(VT_DISPATCH == V_VT(&varValue));
		pPictDisp = (IPictureDisp*)V_DISPATCH(&varValue);
		// if we have a dispatch pointer, release it and set it
		// to NULL
		if (pPictDisp)
		{
			// release the current dispatch ptr
			pPictDisp->Release();

			// set the new value to NULL
			V_DISPATCH(&varValue) =	NULL;
			return SavePropertyForAllControls(&varValue, pItem);
		}

		return S_OK;
	}

	return E_FAIL;
}

// Called by the control(s) when a property is modified to indicate
// that the All property page must update/refresh its property values
// REVIEW: Currently, dispID is not used -- use this for optimization purposes
HRESULT COleAllPage::OnChanged(
	DISPID	dispID)
{
	// if we are not in the middle of changing properties for multi-select
	// or setting properties, handle the updating of properties
	if (!m_bUpdatingProps)
		UpdateChangedProperties();
	else
		m_bUpdateLater = TRUE;	// set flag so that we can be updated later

	return S_OK;
}

HRESULT COleAllPage::OnRequestEdit(
	DISPID	dispID)
{
	return S_OK;	// we always give permission to edit
}

STDMETHODIMP COleAllPage::XPropertyNotifySink::OnChanged(
	DISPID	dispID)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(COleAllPage, PropertyNotifySink)
	ASSERT_VALID(pThis);

	return pThis->OnChanged(dispID);
}

STDMETHODIMP COleAllPage::XPropertyNotifySink::OnRequestEdit(
	DISPID	dispID)
{
	// pass this off to our container class
	METHOD_PROLOGUE_EX(COleAllPage, PropertyNotifySink)
	ASSERT_VALID(pThis);

	return pThis->OnRequestEdit(dispID);
}

STDMETHODIMP_(ULONG) COleAllPage::XPropertyNotifySink::AddRef()
{
	// Delegate to our exported AddRef.

	METHOD_PROLOGUE_EX(COleAllPage, PropertyNotifySink)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) COleAllPage::XPropertyNotifySink::Release()
{
	// Delegate to our exported Release.

	METHOD_PROLOGUE_EX(COleAllPage, PropertyNotifySink)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP COleAllPage::XPropertyNotifySink::QueryInterface(
	REFIID		riid,
	LPVOID far*	ppvObj)
{
	// Delegate to our exported QueryInterface.

	METHOD_PROLOGUE_EX(COleAllPage, PropertyNotifySink)
	return (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
}

// Cache the IPropertyNotifySink interfaces
void COleAllPage::GetNotifySinkInfo()
{
	DWORD	dwCookie;

	// just check to make sure that we do not have any cookies
	ASSERT(0 == m_dwaCookies.GetSize());
	for (int i = 0; i < m_ulCountControls; ++i)
	{
		COleRef<IConnectionPointContainer>	srpCPC;
		COleRef<IConnectionPoint>			srpCP;

		// get the connectionpointcontainer from the dispatch ptr
		// then, get the connection point
		// and finally, get the propertynotifysink cookie
		ASSERT(m_pprgDisp[i]);
		if (SUCCEEDED(m_pprgDisp[i]->QueryInterface(
				IID_IConnectionPointContainer, (void**)&srpCPC)) &&
			SUCCEEDED(srpCPC->FindConnectionPoint(IID_IPropertyNotifySink,
				&srpCP)) &&
			SUCCEEDED(srpCP->Advise(&m_xPropertyNotifySink, &dwCookie)))
		{
			m_dwaCookies.Add(dwCookie);
		}
		else
		{
			// since we need to keep the cookie array matched with the
			// number of controls, just add a "0" if we failed to get
			// the proper notify sink
			m_dwaCookies.Add(0);
		}
	}
}

// Free the cached IPropertyNotifySink interfaces
void COleAllPage::FreeNotifySinkInfo()
{
	int		iSize = m_dwaCookies.GetSize();

	// the number of cookies must equal the number of dispatch ptrs
	ASSERT(iSize == m_ulCountControls);
	for (int i = 0; i < iSize; i++)
	{
		// if we have don't have a cookie for this, just continue
		if (0 == m_dwaCookies[i])
			continue;

		COleRef<IConnectionPointContainer>	srpCPC;
		COleRef<IConnectionPoint>			srpCP;

		// get the connectionpointcontainer from the dispatch ptr
		// then, get the connection point
		ASSERT(m_pprgDisp[i]);
		if (SUCCEEDED(m_pprgDisp[i]->QueryInterface(
				IID_IConnectionPointContainer, (void**)&srpCPC)) &&
			SUCCEEDED(srpCPC->FindConnectionPoint(IID_IPropertyNotifySink,
				&srpCP)))
		{
			// free the propertynotifysink cookie
			srpCP->Unadvise(m_dwaCookies[i]);
		}
	}

	m_dwaCookies.RemoveAll();
}

// Reads the flags from VarDesc or FuncDesc to see if the property is
// "tagged" as read-only, hidden or non-browseable 
BOOL COleAllPage::IsPropTypeReadOnly(
	LPVARDESC	pVarDesc,
	LPFUNCDESC	pFuncDesc,
	BOOL		bCheckForPutAndPutRef)
{
	BOOL		bReadOnly = TRUE;	// assume that the property is read-only
	
	// find out if the property is hidden/nonbrowsable/readonly
	if (pVarDesc)
	{
		// first find out if the property marked as read-only
		bReadOnly = (pVarDesc->wVarFlags & VARFLAG_FREADONLY) ||
			(pVarDesc->wVarFlags & VARFLAG_FHIDDEN) ||
			(pVarDesc->wVarFlags & VARFLAG_FNONBROWSABLE);
	}
	else //(pFuncDesc)
	{
		ASSERT(pFuncDesc);
		// first find out if the property marked as read-only
		if (bCheckForPutAndPutRef)
		{
			bReadOnly = 
				( !(pFuncDesc->invkind & (INVOKE_PROPERTYPUT | INVOKE_PROPERTYPUTREF))
				|| ((pFuncDesc->wFuncFlags & FUNCFLAG_FHIDDEN) ||
				(pFuncDesc->wFuncFlags & FUNCFLAG_FNONBROWSABLE)) ) ||
				(pFuncDesc->cParams != 1);
		}
		else
		{
			bReadOnly = ( (pFuncDesc->wFuncFlags & FUNCFLAG_FHIDDEN) ||
				(pFuncDesc->wFuncFlags & FUNCFLAG_FNONBROWSABLE) ) ||
				(pFuncDesc->cParams != 1);
		}
	}

	return bReadOnly;
}

// A top level method that determines the property type for a given property
// It is assumed that the property is editable (or IsPropTypeReadOnly method
// is already called and verified that the property can be edited/modified)
DataType COleAllPage::GetPropType(
	IDispatch*	pDisp,
	LPTYPEINFO	pTypeInfo,
	LPVARDESC	pVarDesc,
	LPFUNCDESC	pFuncDesc,
	BOOL		bHasPathProps)
{
	ASSERT(pDisp && pTypeInfo);

	DataType	dt = data_Unknown;
	BOOL		bReadOnly;
	DISPID		dispid;
	TYPEDESC*	pTypeDesc = NULL;
	
	// get the dispid and the typedesc
	if (pVarDesc)
	{
		dispid = pVarDesc->memid;
		pTypeDesc = &pVarDesc->elemdescVar.tdesc;
	}
	else //(pFuncDesc)
	{
		ASSERT(pFuncDesc);
		dispid = pFuncDesc->memid;
		pTypeDesc = &pFuncDesc->lprgelemdescParam->tdesc;
	}

	COleRef<IPerPropertyBrowsing>	srpPPB;

	// first find out if this of kind enum_strings
	if (GetPerPropertyBrowsing(pDisp, &srpPPB))
	{
		CALPOLESTR	calpOleStr;
		CADWORD		cadw;
		HRESULT		hr;

		// Note:
		// passing NULL for caloOleStr and cadw does work for some controls
		// and other controls (especially Forms3 controls returns E_POINTER
		// for such input).  Therefore, we have to get these counted arrays
		// and throw them away here to determine that the data type is of
		// type data_EnumString
		// REVIEW -- try to optimize this later
		try {
			hr = srpPPB->GetPredefinedStrings(dispid, &calpOleStr, &cadw);
		}
		catch(...)
		{
			hr = E_UNEXPECTED;
		}
		if (S_OK == hr)
		{
			ASSERT(calpOleStr.cElems == cadw.cElems);
			for (ULONG i = 0; i < calpOleStr.cElems; ++i)
				AfxFreeTaskMem(calpOleStr.pElems[i]);

			AfxFreeTaskMem(calpOleStr.pElems);
			AfxFreeTaskMem(cadw.pElems);

			return data_EnumString;
		}
	}
	// next find out if this a url path property
	else if (bHasPathProps && IsPropAPathProperty(pTypeInfo, dispid))
	{
		return data_Path;
	}

	return DataTypeFromVT(pTypeDesc, pTypeInfo);
}

// A recursive method used to determine the data type of a given property
DataType COleAllPage::DataTypeFromVT(
	TYPEDESC*	pTypeDesc,
	LPTYPEINFO	pTypeInfo)
{
	HREFTYPE	hRefType;
	DataType	dt = data_Unknown;

	switch (pTypeDesc->vt)
	{
		default:
			return MapDataType(pTypeDesc->vt);
			break;
		case VT_USERDEFINED:
			hRefType = pTypeDesc->hreftype;
			break;
		case VT_PTR:
			if (pTypeDesc->lptdesc->vt == VT_USERDEFINED)
			{
				hRefType = pTypeDesc->lptdesc->hreftype;
				pTypeDesc = pTypeDesc->lptdesc;
			}
			else if (pTypeDesc->lptdesc->vt == VT_VARIANT)
			{
				// TODO$ to be done later
				return dt;
			}
			else
			{
				// may be a ptr to some other type which we don't handle
				// return dt;
				// Note: (sridharc)
				// for the time being call this to handle controls created
				// with VB5 CCE (IS Bug 7371) (approved by ScotG)
				return MapDataType(pTypeDesc->lptdesc->vt);
			}
			break;
	}

	COleRef<ITypeInfo>	srpTypeInfoSub;
	LPTYPEATTR			ptaSub = NULL;
	ITypeInfo*			pTypeInfoSub = NULL;

	// get the reference type info and its type attribute
	if (dt == data_Unknown &&
		SUCCEEDED(pTypeInfo->GetRefTypeInfo(hRefType, &srpTypeInfoSub)) &&
		SUCCEEDED(srpTypeInfoSub->GetTypeAttr(&ptaSub)))
	{
		if (ptaSub->typekind == TKIND_ALIAS && ptaSub->tdescAlias.vt == VT_USERDEFINED)
		{
			// call this recursively to figure out the data type
			pTypeInfoSub = srpTypeInfoSub.Disown();
			dt = DataTypeFromVT(&(ptaSub->tdescAlias), pTypeInfoSub);
			goto PropTypeRecurseEnd;
		}
		else
		{
			switch (ptaSub->typekind)
			{
				case TKIND_ENUM:
					// for the time being set the enum type to enumI4,
					// when we actually fill the values, reset the datatype
					// to the correct data type (enumI2 or enumI4)
					dt = data_EnumInt4;
					break;
				case TKIND_ALIAS:
					ASSERT(ptaSub->tdescAlias.vt != VT_USERDEFINED);
					if (ptaSub->guid == GUID_COLOR)
						dt = data_Color;
					else
						dt = MapDataType(ptaSub->tdescAlias.vt);
					break;
				case TKIND_INTERFACE:
					if (ptaSub->guid == IID_IPicture)
						dt = data_Picture;
					else if (ptaSub->guid == IID_IFont)
						dt = data_Font;
					break;
				case TKIND_DISPATCH:
					if (ptaSub->guid == IID_IPictureDisp)
						dt = data_Picture;
					else if (ptaSub->guid == IID_IFontDisp)
						dt = data_Font;
					break;
				case TKIND_RECORD:
				case TKIND_MODULE:
				case TKIND_COCLASS:
				case TKIND_UNION:
				default:
					break;
			}
		}

PropTypeRecurseEnd:

		if (pTypeInfoSub)
		{
			pTypeInfoSub->ReleaseTypeAttr(ptaSub);
			pTypeInfoSub->Release();
		}
		else
		{
			srpTypeInfoSub->ReleaseTypeAttr(ptaSub);
		}
	}

	return dt;
}

// Checks to make sure if the given property is of type "data_Path" (URL Path)
BOOL COleAllPage::IsPropAPathProperty(
	LPTYPEINFO	pTypeInfo,
	DISPID		dispid)
{
	BOOL		bPathProperty = FALSE;
	LPTYPEATTR	pta;

	COleRef<ITypeInfo2>	srpTypeInfo2;

	if (FAILED(pTypeInfo->QueryInterface(IID_ITypeInfo2, (void**)&srpTypeInfo2)) ||
		FAILED(srpTypeInfo2->GetTypeAttr(&pta)))
		return bPathProperty;

	// the following is done since there is a limitation in
	// GetVarIndexOfMemId and GetFuncIndexOfMemId
	//hr = pTypeInfo2->GetVarIndexOfMemId(memid, &varMemID);
	HRESULT		hr;
	VARIANT		varValue;
	UINT		varMemID;
	LPVARDESC	pVarDesc;
	LPFUNCDESC	pFuncDesc;

	for (int i = 0; i < pta->cVars && !bPathProperty; ++i)
	{
		if (FAILED(srpTypeInfo2->GetVarDesc(i, &pVarDesc)))
			continue;

		if (pVarDesc->memid == dispid)
		{
			::VariantInit(&varValue);
			hr = srpTypeInfo2->GetVarCustData(i, GUID_PathProperty, &varValue);
			if ((S_OK == hr) && (varValue.vt != VT_EMPTY))
				bPathProperty = TRUE;

			::VariantClear(&varValue);
		}

		srpTypeInfo2->ReleaseVarDesc(pVarDesc);
	}

	for (i = 0; i < pta->cFuncs && !bPathProperty; ++i)
	{
		if (FAILED(srpTypeInfo2->GetFuncDesc(i, &pFuncDesc)))
			continue;

		if (pFuncDesc->memid == dispid)
		{
			::VariantInit(&varValue);
			hr = srpTypeInfo2->GetFuncCustData(i, GUID_PathProperty, &varValue);
			if ((S_OK == hr) && (varValue.vt != VT_EMPTY))
				bPathProperty = TRUE;

			::VariantClear(&varValue);
		}

		srpTypeInfo2->ReleaseFuncDesc(pFuncDesc);
	}

	srpTypeInfo2->ReleaseTypeAttr(pta);

	return bPathProperty;
}

// Simply "maps" the VARTYPE from the variant to our data type
DataType COleAllPage::MapDataType(
	VARTYPE	vt)
{
	DataType	dt = data_Unknown;

	switch (vt)
	{
		case VT_I2:
			dt = data_Int2;
			break;
		case VT_I4:
			dt = data_Int4;
			break;
		case VT_R4:
			dt = data_Float;
			break;
		case VT_R8:
			dt = data_Double;
			break;
		case VT_CY:
			dt = data_Currency;
			break;
		case VT_DATE:
			dt = data_Date;
			break;
		case VT_BSTR:
			dt = data_String;
			break;
		case VT_BOOL:
			dt = data_Bool;
			break;
		case VT_UI2:
			dt = data_Int2;
			break;
		case VT_UI4:
			dt = data_Int4;
			break;
		case VT_I8:
		case VT_UI8:
			break;
		case VT_INT:
		case VT_UINT:
			dt = data_Int4;
			break;
		default:
			break;
	}

	return dt;
}

// Gets the property name.  If the property name starts with an '_'
// character, it is assumed to be a read-only property
BOOL COleAllPage::GetPropName(
	LPTYPEINFO	pTypeInfo,
	DISPID		dispid,
	CString&	strPropName)
{
	BSTR	bstrName;
	UINT	cName;

	strPropName.Empty();	// initialize the out value
	if (S_OK != pTypeInfo->GetNames(dispid, &bstrName, 1, &cName))
		return FALSE;

	// if the property name has at least one character and
	// if it doesn't start with an underscore, include it
	USES_CONVERSION;
	if ((::SysStringLen(bstrName) > 0) && (*bstrName != (OLECHAR)_T('_')))
		strPropName = OLE2CT(bstrName);

	::SysFreeString(bstrName);

	return (!strPropName.IsEmpty());
}

// This method gets the "currently selected" enumerated string value
// this value is necessary to show the "existing" value for an enumerated
// int property (also used to select the item when the drop-down list is dropped-down)
BOOL COleAllPage::GetPropValueForEnumString(
	IDispatch*	pDisp,
	DISPID		dispid,
	CPropItem*	pItem,
	VARIANT*	pVarValue)
{
	BOOL	bRet;
	COleRef<IPerPropertyBrowsing>	srpPPB;

	bRet = GetPerPropertyBrowsing(pDisp, &srpPPB);
	if (bRet)
	{
		BSTR	bstr;
		HRESULT	hr;

		hr = srpPPB->GetDisplayString(dispid, &bstr);
		if (S_OK == hr)
		{
			USES_CONVERSION;
			pItem->m_strProp = OLE2CT(bstr);
			::SysFreeString(bstr);
		}
		else if (S_FALSE == hr)
		{
			// select the data from one of the items in the list
			BOOL bProceed = UpdateEnumStringData(pDisp, dispid, pItem, FALSE);
			if (bProceed)
			{
				long	lEnumVal;

				switch(pVarValue->vt)
				{
					case VT_I2:
						lEnumVal = (long)(short)V_I2(pVarValue);
						break;
					case VT_UI2:
						lEnumVal = (long)(unsigned short)V_UI2(pVarValue);
						break;
					case VT_I4:
						lEnumVal = V_I4(pVarValue);
						break;
					case VT_UI4:
						lEnumVal = (long)V_UI4(pVarValue);
						break;
					case VT_BSTR:
						pItem->m_strProp = pVarValue->bstrVal;
						bProceed = FALSE;
						break;
					default:
						bProceed = FALSE;
						break;
				}

				if (bProceed)
				{
					int	iCount = pItem->m_pEnumData->m_dwaCookies.GetSize();
					for (int i = 0; i < iCount; ++i)
					{
						if (pItem->m_pEnumData->m_dwaCookies[i] == lEnumVal)
						{
							pItem->m_strProp = pItem->m_pEnumData->m_straEnumStrings[i];
							i = iCount;	// break out of the loop
						}
					}

				}
			}
		}

		pItem->m_ctrlType = (S_FALSE == hr) ? ctrl_ComboNotLimitToList : ctrl_ComboLimitToList;
	}

	return bRet;
}

// This method gets the "currently selected" enumerated int value
// this value is necessary to show the "existing" value for an enumerated
// int property (also used to select the item when the drop-down list is dropped-down)
BOOL COleAllPage::GetPropValueForEnumInt(
	LPTYPEINFO	pTypeInfo,
	HREFTYPE	hrefType,
	long		lEnumValue,
	CPropItem*	pItem,
	BOOL		bSetAllProps)
{
	COleRef<ITypeInfo>	srpTypeInfoSub;
	LPTYPEATTR			ptaSub;
	BOOL				bRet = FALSE;

	// get the reference type info and its type attribute
	if (SUCCEEDED(pTypeInfo->GetRefTypeInfo(hrefType, &srpTypeInfoSub)) &&
		SUCCEEDED(srpTypeInfoSub->GetTypeAttr(&ptaSub)))
	{
		BSTR		bstrVal;
		LPVARDESC	pvdSub;
		long		lCurrentValue;

		ASSERT(ptaSub->typekind == TKIND_ENUM);	// make sure of the type
		for (int i = 0; i < ptaSub->cVars; ++i)
		{
			// if we do not get the vardesc, just continue
			if (FAILED(srpTypeInfoSub->GetVarDesc(i, &pvdSub)))
				continue;

			ASSERT(pvdSub->varkind == VAR_CONST);
			lCurrentValue = GetLongFromVariant(pvdSub->lpvarValue);
			if (lCurrentValue == lEnumValue)
			{
				if (SUCCEEDED(srpTypeInfoSub->GetDocumentation(
						pvdSub->memid, NULL, &bstrVal, NULL, NULL)))
				{
					pItem->m_strProp = FormatEnumIntPropertyValue(lCurrentValue, bstrVal);
					::SysFreeString(bstrVal);
					bRet = TRUE;
				}

				i = ptaSub->cVars;	// break out of the loop
			}

			srpTypeInfoSub->ReleaseVarDesc(pvdSub);
		}

		srpTypeInfoSub->ReleaseTypeAttr(ptaSub);

		if (bRet)
		{
			pItem->unionValue.m_nValue = lEnumValue;
			if (bSetAllProps)
			{
				// cache the ref type info for later use
				ITypeInfo*	pTypeInfoTemp = srpTypeInfoSub.Disown();
				pItem->m_TypeInfoArrayEnumInt.Add(pTypeInfoTemp);
			}
		}
	}
	
	return bRet;
}

// This method is used to "extract" the appropriate property from the variant
// (usually for regular data types -- not the enumerated int/string ones)
BOOL COleAllPage::GetPropValueForOtherTypes(
	VARIANT*	pVarValue,
	CPropItem*	pItem)
{
	BSTR	bstrVal;
	BOOL	bRet = TRUE;

	USES_CONVERSION;
	pItem->m_strProp.Empty();	// initialize the out value
	switch(pItem->m_dataType)
	{
		default:
			bRet = FALSE;
			break;
		case data_Int2:
		case data_UInt2:
			wsprintf(pItem->m_strProp.GetBuffer(32), "%d", pVarValue->iVal);
			pItem->m_strProp.ReleaseBuffer();
			pItem->unionValue.m_nValue = pVarValue->iVal;
			break;
		case data_Int4:
		case data_UInt4:
			wsprintf(pItem->m_strProp.GetBuffer(32), "%d", pVarValue->lVal);
			pItem->m_strProp.ReleaseBuffer();
			pItem->unionValue.m_nValue = pVarValue->lVal;
			break;
		case data_Color:
		{
			CString	strZero(_T("0x00000000"));

			_ltot(pVarValue->lVal, pItem->m_strProp.GetBuffer(32), 16);
			pItem->m_strProp.ReleaseBuffer();
			pItem->m_strProp.MakeUpper();

			pItem->m_strProp = strZero.Left(strZero.GetLength() -
				pItem->m_strProp.GetLength()) + pItem->m_strProp;
			pItem->unionValue.m_nValue = pVarValue->lVal;
		}
			break;
		case data_Bool:
			pItem->m_strProp = (pVarValue->boolVal) ? s_strTrue : s_strFalse;
			pItem->unionValue.m_nValue = (pVarValue->boolVal) ? TRUE : FALSE;
			break;
		case data_Font:
		{
			LPUNKNOWN	pUnk = V_UNKNOWN(pVarValue);
			if (pUnk)
			{
				COleRef<IFont>	srpFont;

				if (SUCCEEDED(pUnk->QueryInterface(IID_IFont, (void**)&srpFont)))
				{
					srpFont->get_Name(&bstrVal);
					pItem->m_strProp = OLE2CT(bstrVal);
					::SysFreeString(bstrVal);
				}
			}
		}
			break;
		case data_Picture:
		{
			LPUNKNOWN	pUnk = V_UNKNOWN(pVarValue);
			if (pUnk)
			{
				COleRef<IDispatch>	srpDisp;

				if (SUCCEEDED(pUnk->QueryInterface(IID_IDispatch,
					(void**)&srpDisp)) && srpDisp)
					GetPictureType(srpDisp, pItem->m_strProp);
			}

			if (pItem->m_strProp.IsEmpty())
				pItem->m_strProp.LoadString(IDS_PICTURETYPENONE);
		}
			break;
		case data_Double:
			if (S_OK == VarBstrFromR8(pVarValue->dblVal, s_lcid,
				LOCALE_NOUSEROVERRIDE, &bstrVal))
			{
				pItem->m_strProp = OLE2T(bstrVal);
				::SysFreeString(bstrVal);
				pItem->unionValue.m_dValue = pVarValue->dblVal;
			}
			break;
		case data_Path:
		case data_String:
			pItem->m_strProp = OLE2T(pVarValue->bstrVal);
			break;
		case data_EnumString:
			// taken care of elsewhere (call GetPropValueForEnumString)
			ASSERT(FALSE);
			break;
		case data_EnumInt2:
		case data_EnumInt4:
			// taken care of elsewhere (call GetPropValueForEnumInt)
			ASSERT(FALSE);
			break;
		case data_Float:
			if (S_OK == VarBstrFromR4(pVarValue->fltVal, s_lcid,
				LOCALE_NOUSEROVERRIDE, &bstrVal))
			{
				pItem->m_strProp = OLE2T(bstrVal);
				::SysFreeString(bstrVal);
				pItem->unionValue.m_fValue = pVarValue->fltVal;
			}
			break;
		case data_Currency:
			if (S_OK == VarBstrFromCy(pVarValue->cyVal, s_lcid,
				LOCALE_NOUSEROVERRIDE, &bstrVal))
			{
				pItem->m_strProp = OLE2T(bstrVal);
				::SysFreeString(bstrVal);
				pItem->unionValue.m_currency = pVarValue->cyVal;
			}
			break;
		case data_Date:
			if (S_OK == VarBstrFromDate(pVarValue->date, s_lcid,
				LOCALE_NOUSEROVERRIDE, &bstrVal))
			{
				pItem->m_strProp = OLE2T(bstrVal);
				::SysFreeString(bstrVal);
				pItem->unionValue.m_date = pVarValue->date;
			}
			break;
	}

	return bRet;
}

// A silly little method to get the property value
BOOL COleAllPage::GetPropValue(
	IDispatch*	pDisp,
	DISPID		dispid,
	VARIANT*	pVarValue)
{
	DISPPARAMS	dp = {NULL, NULL, 0, 0};
	BOOL bRet = FALSE;
	::VariantInit(pVarValue);	// initialize the return value
	
	try 
	{
		bRet = (S_OK == pDisp->Invoke(dispid, IID_NULL, 0, INVOKE_PROPERTYGET,
			&dp, pVarValue, NULL, NULL));
	}
	catch (...)
	{
	}
	return bRet;
}

// This method gets property value and other details necessary to edit a
// property
BOOL COleAllPage::GetPropDetails(
	IDispatch*	pDisp,
	LPTYPEINFO	pTypeInfo,
	HREFTYPE	hrefType,
	DISPID		dispid,
	CPropItem*	pItem,
	BOOL		bSetAllDetails,
	VARIANT*	pVarValue)
{
	BOOL	bRet = FALSE;

	switch(pItem->m_dataType)
	{
		default:
			bRet = GetPropValueForOtherTypes(pVarValue, pItem);
			break;
		case data_EnumInt2:
		case data_EnumInt4:
		{
			long	lValue = GetLongFromVariant(pVarValue);

			bRet = GetPropValueForEnumInt(pTypeInfo, hrefType, lValue,
				pItem, bSetAllDetails);
			// if we have a multiple selection, we need to get the enumdata now
			if (bRet && m_ulCountControls > 1)
				bRet = UpdateEnumIntData(0, dispid, pItem, lValue);
		}
			break;
		case data_EnumString:
			bRet = GetPropValueForEnumString(pDisp, dispid, pItem, pVarValue);
			// if we have a multiple selection, we need to get the enumdata now
			if (bRet && m_ulCountControls > 1)
				bRet = UpdateEnumStringData(pDisp, dispid, pItem, FALSE);
			break;
	}

	if (bRet && bSetAllDetails)
	{
		// set the builder type for later use
		switch(pItem->m_dataType)
		{
			default:
				bRet = FALSE;
				break;
			case data_Int2:
			case data_Int4:
			case data_UInt2:
			case data_UInt4:
			case data_Path:
			case data_Float:
			case data_Double:
			case data_String:
			case data_Date:
			case data_Currency:
				DetermineBuilderType(dispid, pItem);
				if (pItem->m_ctrlType == ctrl_Unknown)
					pItem->m_ctrlType = ctrl_Edit;
				break;
			case data_Color:
			case data_Font:
			case data_Picture:
				DetermineBuilderType(dispid, pItem);
				bRet = (pItem->m_ctrlType != ctrl_Unknown);
				break;
			case data_EnumString:
				// builder type already set in GetPropValueForEnumString
				bRet = (pItem->m_ctrlType != ctrl_Unknown);
				break;
			case data_Bool:
			case data_EnumInt2:
			case data_EnumInt4:
				pItem->m_ctrlType = ctrl_ComboLimitToList;
				break;
		}
	}

	return bRet;
}

// This method updates the enum int data (frees the old data before
// gathering the new enum int data
BOOL COleAllPage::UpdateEnumIntData(
	int			iControlIndex,
	DISPID		dispid,
	CPropItem*	pItem,
	long		lEnumVal)
{
	// if the enum int data is not "dirty" no need to re-get the data
	if (!pItem->IsEnumDataDirty())
		return TRUE;

	// do not release the following interface here, this is cached and will
	// be released upon the destruction of pItem
	ASSERT(iControlIndex <= pItem->m_TypeInfoArrayEnumInt.GetSize());
	ITypeInfo*	pTypeInfo;	// ref typeinfo
	LPTYPEATTR	ptaSub;

	pTypeInfo = (ITypeInfo*)pItem->m_TypeInfoArrayEnumInt.GetAt(iControlIndex);
	ASSERT(pTypeInfo);
	if (SUCCEEDED(pTypeInfo->GetTypeAttr(&ptaSub)))
	{
		LPVARDESC	pvdSub;
		long		lEnumValInLoop;
		BSTR		bstr;
		CString		strLoop;
		int			iCount;

		// free the old data and get memory for new data
		pItem->GetNewEnumData();

		for (int i = 0; i < ptaSub->cVars; ++i)
		{
			if (FAILED(pTypeInfo->GetVarDesc(i, &pvdSub)))
				continue;

			ASSERT(pvdSub->varkind == VAR_CONST);
			if (V_VT(pvdSub->lpvarValue) == VT_I4)
			{
				pItem->m_dataType = data_EnumInt4;
				lEnumValInLoop = V_I4(pvdSub->lpvarValue);
			}
			else
			{
				ASSERT(V_VT(pvdSub->lpvarValue) == VT_I2);
				pItem->m_dataType = data_EnumInt2;
				lEnumValInLoop = (long)(short)V_I2(pvdSub->lpvarValue);
			}

			if (SUCCEEDED(pTypeInfo->GetDocumentation(
					pvdSub->memid, NULL, &bstr, NULL, NULL)))
			{
				strLoop = FormatEnumIntPropertyValue(lEnumValInLoop, bstr);
				::SysFreeString(bstr);

				// store the enumerated string and the cookie
				// in the enum data structure
				pItem->m_pEnumData->m_straEnumStrings.Add(strLoop);
				pItem->m_pEnumData->m_dwaCookies.Add(lEnumValInLoop);
			}

			pTypeInfo->ReleaseVarDesc(pvdSub);
		}

		pItem->SetEnumDataDirty(FALSE);
		pTypeInfo->ReleaseTypeAttr(ptaSub);

		// select the current value and store it in pItem->mstrProp
		iCount = pItem->m_pEnumData->m_dwaCookies.GetSize();
		for (i = 0; i < iCount; i++)
		{
			if (pItem->m_pEnumData->m_dwaCookies[i] == lEnumVal)
			{
				pItem->m_strProp = pItem->m_pEnumData->m_straEnumStrings[i];
				pItem->unionValue.m_nValue = lEnumVal;
				i = iCount;	// break out of the loop
			}
		}

		return TRUE;
	}

	return FALSE;
}

// This method updates the enum string data (frees the old data before
// gathering the new enum string data
BOOL COleAllPage::UpdateEnumStringData(
	IDispatch*	pDisp,
	DISPID		dispid,
	CPropItem*	pItem,
	BOOL		bUpdatePropValue)
{
	// if the enum string data is not "dirty" no need to re-get the data
	if (!pItem->IsEnumDataDirty())
		return TRUE;

	COleRef<IPerPropertyBrowsing>	srpPPB;

	if (!GetPerPropertyBrowsing(pDisp, &srpPPB))
		return FALSE;

	HRESULT		hr;
	CALPOLESTR	calpOleStr;
	CADWORD		cadw;

	hr = srpPPB->GetPredefinedStrings(dispid, &calpOleStr, &cadw);
	if (hr == S_OK && calpOleStr.cElems > 0) //NOTE: Don't use SUCCEEDED()!
	{
		// free the old data and get memory for new data
		pItem->GetNewEnumData();

		USES_CONVERSION;
		ASSERT(calpOleStr.cElems == cadw.cElems);
		for (ULONG i = 0; i < calpOleStr.cElems; ++i)
		{
			// store the enumerated string and the cookie
			// in the enum data structure
			pItem->m_pEnumData->m_straEnumStrings.Add(OLE2CT(calpOleStr.pElems[i]));
			pItem->m_pEnumData->m_dwaCookies.Add((DWORD)cadw.pElems[i]);

			AfxFreeTaskMem(calpOleStr.pElems[i]);
		}

		AfxFreeTaskMem(calpOleStr.pElems);
		AfxFreeTaskMem(cadw.pElems);

		pItem->SetEnumDataDirty(FALSE);

		if (bUpdatePropValue)
		{
			VARIANT	var;

			if (GetPropValue(pDisp, dispid, &var))
				GetPropValueForEnumString(pDisp, dispid, pItem, &var);

			::VariantClear(&var);
		}
		return TRUE;
	}

	return FALSE;
}

// When more than 1 control is selected, this method verifies that the enum
// string is similar in all the controls.  That is, it makes sure that the
// number of elements enumerated is the same and the enumerated items are also
// the same.  This method assumes that the "order" in the elements are enumerated
// must be the same for 2 controls to edit a property -- if the order is different
// then the user has to select each control separately to edit the enumerated
// string property
BOOL COleAllPage::IntersectPropEnumString(
	IDispatch*	pDisp,
	DISPID		dispid,
	CPropItem*	pItem)
{
	ASSERT(pItem->m_pEnumData);	// this should already be set

	BOOL		bRet = FALSE;
	CALPOLESTR	calpOleStr;
	CADWORD		cadw;
	HRESULT		hr;

	COleRef<IPerPropertyBrowsing>	srpPPB;

	if (!GetPerPropertyBrowsing(pDisp, &srpPPB))
		return bRet;

	// get all the strings
	// make sure that each of the pre-defined string exists in
	// the pItem->m_pEnumData structure.  If not, return FALSE
	// so that that property is removed from the list (map)
	hr = srpPPB->GetPredefinedStrings(dispid, &calpOleStr, &cadw);
	if (hr == S_OK && calpOleStr.cElems > 0) //NOTE: Don't use SUCCEEDED()!
	{
		// if the number of strings we have in pItem->m_pEnumData is not the
		// same as the number we got here, then we have to drop this property
		ASSERT(calpOleStr.cElems == cadw.cElems);
		bRet = (pItem->m_pEnumData->m_straEnumStrings.GetSize() == (int) calpOleStr.cElems);

		USES_CONVERSION;
		// the loop has to go through to free elements
		for (ULONG i = 0; i < calpOleStr.cElems; ++i)
		{
			// assumption:
			// the "order" in which the elements exist must be
			// the same for all the controls selected
			if (bRet)
				bRet = 
					(0 == pItem->m_pEnumData->m_straEnumStrings[i].Compare(OLE2CT(calpOleStr.pElems[i]))) &&
					((DWORD)cadw.pElems[i] == pItem->m_pEnumData->m_dwaCookies[i]);

			AfxFreeTaskMem(calpOleStr.pElems[i]);
		}

		AfxFreeTaskMem(calpOleStr.pElems);
		AfxFreeTaskMem(cadw.pElems);
	}

	return bRet;
}

// When more than 1 control is selected, this method verifies that the enum
// int (2 or 4) is similar in all the controls.  That is, it makes sure that the
// number of elements enumerated is the same and the enumerated items are also
// the same.  This method assumes that the "order" in the elements are enumerated
// must be the same for 2 controls to edit a property -- if the order is different
// then the user has to select each control separately to edit the enumerated
// int property
BOOL COleAllPage::IntersectPropEnumInt(
	IDispatch*	pDisp,
	DISPID		dispid,
	CPropItem*	pItem,
	LPTYPEINFO	pTypeInfo,
	TYPEDESC*	pTypeDesc)
{
	ASSERT(pItem->m_pEnumData);	// this should already be set

	BOOL				bRet = FALSE;
	COleRef<ITypeInfo>	srpTypeInfoSub;
	LPTYPEATTR			ptaSub;

	if (SUCCEEDED(pTypeInfo->GetRefTypeInfo(pTypeDesc->hreftype, &srpTypeInfoSub))
		&& SUCCEEDED(srpTypeInfoSub->GetTypeAttr(&ptaSub)))
	{
		LPVARDESC	pvdSub;
		long		lEnumValInLoop;
		long		lEnumVal;
		BSTR		bstr;
		CString		strLoop;

		// if the number of strings we have in pItem->m_pEnumData
		// is not the same as the number we got here, then
		// we have to drop this property
		bRet = (pItem->m_pEnumData->m_straEnumStrings.GetSize() == ptaSub->cVars);

		// get all the strings
		// make sure that each of the pre-defined string exists in
		// the pItem->m_pEnumData structure.  If not, return FALSE
		// so that that property is removed from the list (map)
		for (int i = 0; bRet && (i < ptaSub->cVars); ++i)
		{
			if (FAILED(srpTypeInfoSub->GetVarDesc(i, &pvdSub)))
				continue;

			ASSERT(pvdSub->varkind == VAR_CONST);
			lEnumValInLoop = GetLongFromVariant(pvdSub->lpvarValue);
			if (SUCCEEDED(srpTypeInfoSub->GetDocumentation(
						pvdSub->memid, NULL, &bstr, NULL, NULL)))
			{
				strLoop = FormatEnumIntPropertyValue(lEnumValInLoop, bstr);
				::SysFreeString(bstr);

				// assumption:
				// the "order" in which the elements exist must be
				// the same for all the controls selected
				if (bRet)
					bRet =
						(0 == pItem->m_pEnumData->m_straEnumStrings[i].Compare(strLoop)) &&
						(lEnumValInLoop == pItem->m_pEnumData->m_dwaCookies[i]);
			}

			srpTypeInfoSub->ReleaseVarDesc(pvdSub);
		}

		srpTypeInfoSub->ReleaseTypeAttr(ptaSub);
		
		if (bRet)
		{
			// cache the ref type info for later use
			ITypeInfo*	pTypeInfoTemp = srpTypeInfoSub.Disown();
			pItem->m_TypeInfoArrayEnumInt.Add(pTypeInfoTemp);
		}
	}

	return bRet;
}

// When more than one control is selected, the AddToMap method is called
// for the first control and for the rest, this method is called.
// This method basically "reduces" the properties' set to the "least common
// denominator" set of properties
BOOL COleAllPage::IntersectPropItemData(
	IDispatch*	pDisp,
	DISPID		dispid,
	CPropItem*	pItem, 
	LPTYPEINFO	pTypeInfo,
	TYPEDESC*	pTypeDesc)
{
	ASSERT(pItem && pTypeInfo && m_dispDriver.m_lpDispatch);

	BOOL	bRet = FALSE;
	VARIANT	var;

	// do not allow editing of font and picture for multiple selection
	if ((pItem->m_dataType == data_Unknown) ||
		(pItem->m_dataType == data_Font) ||
		(pItem->m_dataType == data_Picture))
		return bRet;

	bRet = GetPropValue(pDisp, dispid, &var);
	if (!bRet)
		return bRet;

	// compare the existing property value with the new one
	// if they don't match, set the property value to nothing
	switch(pItem->m_dataType)
	{
		default:
			ASSERT(FALSE);
			bRet = FALSE;
			break;
		case data_EnumString:	// taken care of above
			bRet = IntersectPropEnumString(pDisp, dispid, pItem);
			if (bRet && !(pItem->m_strProp.IsEmpty()))
			{
				ControlType	ct;
				CString		strPropNew;

				strPropNew = pItem->m_strProp;
				ct = pItem->m_ctrlType;
				bRet = GetPropValueForEnumString(pDisp, dispid, pItem, &var);
				if (bRet && (0 != strPropNew.Compare(pItem->m_strProp)))
					pItem->m_strProp.Empty();
			}
			break;
		case data_EnumInt2:
		case data_EnumInt4:
			bRet = IntersectPropEnumInt(pDisp, dispid, pItem, pTypeInfo, pTypeDesc);
			if (bRet && (pItem->unionValue.m_nValue != GetLongFromVariant(&var)))
			{
				pItem->unionValue.m_nValue = -1;
				pItem->m_strProp.Empty();
			}
			break;

		case data_Int2:
		case data_UInt2:
		case data_Int4:
		case data_UInt4:
		case data_Color:
		case data_Float:
		case data_Double:
		case data_Bool:
		case data_Path:
		case data_String:
		case data_Date:
		case data_Currency:
			if (!(pItem->m_strProp.IsEmpty()))
			{
				switch(pItem->m_dataType)
				{
					case data_Int2:		// maps to VT_I2
					case data_UInt2:
						if (pItem->unionValue.m_nValue != var.iVal)
							pItem->m_strProp.Empty();
						break;
					case data_Int4:		// maps to VT_I4
					case data_UInt4:
					case data_Color:
						if (pItem->unionValue.m_nValue != var.lVal)
							pItem->m_strProp.Empty();
						break;
					case data_Float:		// maps to VT_R4
						if (pItem->unionValue.m_fValue != var.fltVal)
							pItem->m_strProp.Empty();
						break;
					case data_Double:		// maps to VT_R8
						if (pItem->unionValue.m_dValue != var.dblVal)
							pItem->m_strProp.Empty();
						break;
					case data_Bool:		// maps to VT_BOOL
						if (pItem->unionValue.m_nValue !=
							((var.boolVal) ? TRUE : FALSE))
							pItem->m_strProp.Empty();
						break;
					case data_Path:
					case data_String:	// maps to VT_BSTR
					{
						USES_CONVERSION;

						// bstrVal can be NULL, CString.Compare does not accept NULL
						if ((NULL == var.bstrVal) ||
							(0 != pItem->m_strProp.Compare(OLE2T(var.bstrVal))))
							pItem->m_strProp.Empty();
					}
						break;
					case data_Date:
						if (pItem->unionValue.m_date!= var.date)
							pItem->m_strProp.Empty();
						break;
					case data_Currency:
					{
						BSTR	bstrVal;

						VarBstrFromCy(var.cyVal, s_lcid, LOCALE_NOUSEROVERRIDE,
							&bstrVal);
						USES_CONVERSION;

						// bstrVal can be NULL, CString.Compare does not accept NULL
						if ((NULL == bstrVal) ||
							(0 != pItem->m_strProp.Compare(OLE2T(bstrVal))))
							pItem->m_strProp.Empty();

						::SysFreeString(bstrVal);
					}
						break;
				}
			}
			break;
	}

	::VariantClear(&var);

	return bRet;
}

// Everytime a property is changed, or whenever an OnChanged notification
// (through IPropertyNotifySink interface) is received, this method is called
// to re-read the data from the control(s)
void COleAllPage::UpdateChangedProperties()
{
	BOOL		bRepaint = FALSE;
	try
	{
		int			iCount = m_lbProps.GetCount();
		HRESULT		hr;
		CPropItem*	pItem;
		BOOL		bNewPropSet;
		BOOL		bEnumIntUpdated;
		BOOL		bEnumStringUpdated;
		DISPID		dispid;
		IDispatch*	pDisp;
		VARIANT		var;
		CString		strPropNew;
		DataType	dt;

		USES_CONVERSION;
		// go through all the properties
		for (int i = 0; i < iCount; ++i)
		{
			pItem = (CPropItem*) m_lbProps.GetItemData(i);
			ASSERT(pItem);
			bNewPropSet = FALSE;
			bEnumIntUpdated = FALSE;
			bEnumStringUpdated = FALSE;

			// re-get the property type again.
			// this is done since some properties can change it property type
			// (for example, from data_String to data_EnumString when a property
			// like the DataSource property is changed).
			dt = GetPropType(m_pprgDisp[0], pItem->m_TypeInfo,
				pItem->m_pvd, pItem->m_pfd, (pItem->m_dataType == data_Path));

			if (dt != pItem->m_dataType)
			{
				// if the new data type is not the same as the old one, just delete
				// the enumeration data if it exists
				if (pItem->m_pEnumData)
				{
					delete pItem->m_pEnumData;
					pItem->m_pEnumData = NULL;
				}
				
				pItem->m_dataType = dt;
			}

			// go through all the controls (for each property)
			for (int j = 0; j < m_ulCountControls; ++j)
			{
				pDisp = m_pprgDisp[j];
				ASSERT(pDisp);
				dispid = pItem->m_pDispid[j];

				switch(pItem->m_dataType)
				{
					default:
						ASSERT(FALSE);
						break;
					case data_EnumString:	// taken care of above
						// refill the property list and property
						// it is easier to do this once instead of comparing
						// and finding out if this property changed
						if (!bEnumStringUpdated)
						{
							pItem->SetEnumDataDirty(TRUE);
							if (UpdateEnumStringData(pDisp, dispid, pItem, TRUE))
							{
								bEnumStringUpdated = TRUE;
								bNewPropSet = TRUE;
							}
						}
						break;
					case data_EnumInt2:
					case data_EnumInt4:
					case data_Int2:
					case data_UInt2:
					case data_Int4:
					case data_UInt4:
					case data_Color:
					case data_Float:
					case data_Font:
					case data_Picture:
					case data_Double:
					case data_Bool:
					case data_Path:
					case data_String:
					case data_Currency:
					case data_Date:
						if (GetPropValue(pDisp, dispid, &var))
						{
							if (pItem->m_dataType == data_EnumInt2 ||
								pItem->m_dataType == data_EnumInt4)
							{
								// refill the property list and property
								// it is easier to do this once instead of comparing
								// and finding out if this property changed
								if (!bEnumIntUpdated)
								{
									pItem->SetEnumDataDirty(TRUE);
									if (UpdateEnumIntData(j, dispid, pItem,
										GetLongFromVariant(&var)))
									{
										bEnumIntUpdated = TRUE;
										bNewPropSet = TRUE;
									}
								}
							}
							else
							{
								// store the current value and get the new value
								strPropNew = pItem->m_strProp;
								GetPropValueForOtherTypes(&var, pItem);
								// if they are not the same, we have the updated value
								if (0 != strPropNew.Compare(pItem->m_strProp))
								{
									bNewPropSet = TRUE;
									// if this happened and we have multiple
									// controls selected, set property to nothing
									if (m_ulCountControls > 1)
									{
										pItem->m_strProp.Empty();
										pItem->unionValue.m_nValue = 0;
										pItem->unionValue.m_dValue = 0.0;
									}
								}
							}
							::VariantClear(&var);
						}
						break;
				}
			}

			if (bNewPropSet && !bRepaint)
				bRepaint = TRUE;
		}
	}
	catch(...)
	{
	}

	if (bRepaint)
		m_lbProps.InvalidateRect(NULL);	// update ui (properties)
}

// This method updates the enum data (string or int2/int4)
// for the drop-down list to display the most recent data
void COleAllPage::UpdateEnumData(
	CPropItem*	pItem)
{
	if (!IsEnumDataType(pItem))
		return;

	IDispatch*	pDisp = m_pprgDisp[0];
	DISPID		dispid = pItem->m_pDispid[0];

	ASSERT(pDisp);

	try 
	{
		if (pItem->m_dataType == data_EnumString)
		{
			UpdateEnumStringData(pDisp, dispid, pItem, FALSE);
		}
		else
		{
			VARIANT	var;

			if (GetPropValue(pDisp, dispid, &var))
			{
				UpdateEnumIntData(0, dispid, pItem, GetLongFromVariant(&var));
				::VariantClear(&var);
			}
		}
	}
	catch (...)
	{}
}

#if 0
void CPropEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (VK_TAB == nChar)
	{
		CWnd *pWnd = GetParent();
		ASSERT(pWnd);
		CWnd *pDlg = pWnd->GetParent();
		ASSERT(pDlg);
		pDlg->SetFocus();
// #else
		GetParent()->SetFocus();
		return;
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
#endif
