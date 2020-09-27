#include "stdafx.h"
#include "DLSStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDLSStatic::CDLSStatic(UINT nID, CString csTitle, CRect rcPosition, LOGFONT* plfTextFont):
m_nID(nID), m_csTitle(csTitle), m_rcPosition(rcPosition), m_bSelected(FALSE)
{
	ASSERT(plfTextFont);
	m_plfTextFont = plfTextFont;
	
	m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrBackground = ::GetSysColor(COLOR_BTNFACE);
	m_clrSelectedText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_clrSelectedBackground = ::GetSysColor(COLOR_HIGHLIGHT);

	m_nTextAlignment = DT_LEFT;

	m_pParentWnd = NULL;
}

CDLSStatic::~CDLSStatic()
{
	if(m_plfTextFont)
	{
		delete m_plfTextFont;
		m_plfTextFont = NULL;
	}

	m_pParentWnd = NULL;
}



// The passed CWnd* pStaticWnd must point to a "Static" control...else this method will fail...
HRESULT CDLSStatic::CreateControl(CWnd* pParentWnd, CWnd* pStaticWnd, CDLSStatic** ppDLSStatic)
{
	if(pParentWnd == NULL)
	{
		return E_FAIL;
	}
	
	if(pStaticWnd == NULL)
	{
		return E_FAIL;
	}

	UINT nID = pStaticWnd->GetDlgCtrlID();

	CStatic* pStatic = (CStatic*)pParentWnd->GetDlgItem(nID);
	ASSERT(pStatic);
	if(pStatic == NULL)
	{
		return E_FAIL;
	}
	
	// We want to add statics without bitmaps
	if(pStatic->GetBitmap() == NULL)
	{
		// What are the parent's screen co-ordinates?
		CRect rcParent;
		pParentWnd->GetWindowRect(&rcParent);

		CRect clientRect;
		pStaticWnd->GetClientRect(&clientRect);

		// Calculate the relative position of the control in the parent window...
		CRect rcWindow;
		pStaticWnd->GetWindowRect(&rcWindow);
		rcWindow.left = rcWindow.left - rcParent.left;
		rcWindow.top = rcWindow.top - rcParent.top;
		rcWindow.right= rcWindow.left + clientRect.Width();
		rcWindow.bottom = rcWindow.top + clientRect.Height();

		// What's the display text?
		CString sText;
		pStaticWnd->GetWindowText(sText);

		// Setup the FONT you want to display the text in...
		LOGFONT* pLogFont = NULL;
		CFont* pStaticFont = pStaticWnd->GetFont();
		if(pStaticFont)
		{
			pLogFont = new LOGFONT;
			pStaticFont->GetLogFont(pLogFont);
		}

		// wha tis the alignment specified in the resource file?
		UINT nTextAlign = DT_LEFT;
		LONG lStyle = GetWindowLong(pStaticWnd->m_hWnd, GWL_STYLE);
		if(lStyle & SS_RIGHT)
			nTextAlign = DT_RIGHT;
		else if(lStyle & SS_CENTER) 
			nTextAlign = DT_CENTER;
			
		CDLSStatic* pDLSStatic = new CDLSStatic(nID, sText, rcWindow, pLogFont);
		if(pDLSStatic == NULL)
		{
			return E_OUTOFMEMORY;
		}

		pDLSStatic->m_pParentWnd = pParentWnd;

		pDLSStatic->SetTextAlign(nTextAlign);
		*ppDLSStatic = pDLSStatic;
		return S_OK;
	}

	return E_FAIL;
}


UINT CDLSStatic::GetID()
{
	return m_nID;
}

CRect CDLSStatic::GetPosition()
{
	return m_rcPosition;
}

CString CDLSStatic::GetText()
{
	return m_csTitle;
}

void CDLSStatic::SetText(CString sText)
{
	m_csTitle = sText;
}

void CDLSStatic::OnDraw(CDC* pDC)
{

	ASSERT(pDC);
	if(pDC == NULL)
		return;

	CFont textFont;
	CFont* pOldFont = NULL;
	if(m_plfTextFont)
	{
		textFont.CreateFontIndirect(m_plfTextFont);
		pOldFont = pDC->SelectObject(&textFont);
	}

	COLORREF clrText = m_clrText;
	COLORREF clrBackground = m_clrBackground;

	if(m_bSelected == TRUE)
	{
		clrText = m_clrSelectedText;
		clrBackground = m_clrSelectedBackground;
	}

	COLORREF clrOldBk = pDC->SetBkColor(clrBackground);
	COLORREF clrOldText = pDC->SetTextColor(clrText);
	
	pDC->DrawText(m_csTitle, &m_rcPosition, DT_SINGLELINE | m_nTextAlignment);

	if(clrOldBk != 0x80000000)
		pDC->SetBkColor(clrOldBk);

	if(clrOldText != 0x80000000)
		pDC->SetTextColor(clrOldText);

	if(pOldFont)
	{
		pDC->SelectObject(pOldFont);
		textFont.DeleteObject();
	}

}


void CDLSStatic::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
}


void CDLSStatic::SetBackgroundColor(COLORREF clrBckgnd)
{
	m_clrBackground = clrBckgnd;
}

void CDLSStatic::SetTextAlign(UINT nTextAlign)
{
	m_nTextAlignment = nTextAlign;
}


void CDLSStatic::UnderlineText(BOOL bUnderline)
{
	ASSERT(m_plfTextFont);
	if(m_plfTextFont == NULL)
		return;

	m_plfTextFont->lfUnderline = BYTE(bUnderline);
}

void CDLSStatic::SetSelected(BOOL bSelection)
{
	m_bSelected = bSelection;
}

BOOL CDLSStatic::IsSelected()
{
	return m_bSelected;
}

BOOL CDLSStatic::IsPointInControl(CPoint ptCheck)
{
	return m_rcPosition.PtInRect(ptCheck);
}

void CDLSStatic::Invalidate(CDC* pDC)
{
	ASSERT(pDC);
	if(pDC == NULL)
	{
		return;
	}
	
	m_pParentWnd->InvalidateRect(m_rcPosition);
}