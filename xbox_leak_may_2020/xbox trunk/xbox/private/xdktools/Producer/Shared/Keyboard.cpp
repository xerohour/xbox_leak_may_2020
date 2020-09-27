
// Keyboard.cpp : implementation file
//
#include "stdafx.h"

#include "Keyboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


KeyInfo::KeyInfo()

{
	m_nState = 0;
	m_fRootKey = FALSE;
}

void KeyInfo::SetState(short nState)

{
	m_nState = nState;
	switch (nState)
	{
	case 0:
		if (m_fBlackKey)
		{
			m_Color = RGB( 0,0,0 );
		}
		else if(m_fRootKey)
		{
			m_Color = RGB( 0xFF,0xFF,0xC0 );
		}
		else
		{
			m_Color = RGB( 0xFF,0xFF,0xFF );
		}
		break;
	case 1 :
		m_Color = RGB( 0,0,0xFF );
		break;
	case 2 :
		m_Color = RGB( 0xFF,0,0 );
		break;
	case 3 :
		m_Color = RGB( 0xFF,0,0xFF );
		break;
	case 4 :
		if (m_fBlackKey)
		{
			m_Color = RGB( 0x20,0x20,0x20 );
		}
		else
		{
			m_Color = RGB( 0xE0,0xE0,0xE0 );
		}
		break;
	case 5 :
		m_Color = RGB( 0x80,0x80,0x80 );
		break;
	case 6 :
		m_Color = RGB( 0x40,0x40,0x40 );
		break;
	case 7 :
		m_Color = RGB( 0xC0,0xC0,0xC0 );
		break;
	default :
		m_Color = RGB( nState, nState, nState );
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CKeyboard

CKeyboard::CKeyboard()
{
	static BOOL fBlackKey[Size] = { 
		FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, FALSE,
		FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, FALSE,
		FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, FALSE,
		FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, FALSE
	};
	int nX;
	for (nX = 0; nX < Size; nX++)
	{
		m_Keys[nX].m_fBlackKey = fBlackKey[nX];
		m_Keys[nX].SetState(0);
		if(nX % 12 == 0)
		{
			m_Keys[nX].m_csName.Format("%d", (nX/12) + 1);
		}
	}
	m_lowerbound = 0;
	m_transpose = 0;
	m_pfnLButtonDown = 0;
	m_pHintLButtonDown = 0;
	m_pfnRButtonDown = 0;
	m_pHintRButtonDown = 0;
	m_pfnLButtonUp = 0;
	m_pHintLButtonUp = 0;
	m_visible = 24;
	m_nRootKeys = 0;
}

CKeyboard::~CKeyboard()
{
}


BEGIN_MESSAGE_MAP(CKeyboard, CButton)
	//{{AFX_MSG_MAP(CKeyboard)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CKeyboard::DrawKeyboard(CDC* pDC)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int nX;
	CBrush Frame;
	Frame.CreateSolidBrush( 0 );
	for (nX = m_lowerbound; nX < m_lowerbound+m_visible; nX++)
	{
		if (!m_Keys[nX].m_fBlackKey)
		{
			CBrush Color;
			Color.CreateSolidBrush( m_Keys[nX].m_Color );
			pDC->FillRect(m_Keys[nX].m_crDrawRect,&Color);
			pDC->FrameRect(m_Keys[nX].m_crDrawRect,&Frame);
		}
	}
	for (nX = m_lowerbound; nX < m_lowerbound + m_visible; nX++)
	{
		if (m_Keys[nX].m_fBlackKey)
		{
			CBrush Color;
			Color.CreateSolidBrush( m_Keys[nX].m_Color );
			pDC->FillRect(m_Keys[nX].m_crDrawRect,&Color);
//			pDC->FillSolidRect(m_Keys[nX].m_crDrawRect,m_Keys[nX].m_Color);
			pDC->FrameRect(m_Keys[nX].m_crDrawRect,&Frame);
		}
	}
	CFont font;
	CFont *oldfont;
	if (font.CreateFont(10,0,0,0,0,0,0,0,0,0,0,0,0,0))
	{
		int oldmode = pDC->SetBkMode(TRANSPARENT);
		COLORREF oldcolor = pDC->GetTextColor();
		oldfont = pDC->SelectObject(&font);
		for (nX=m_lowerbound; nX < m_lowerbound+m_visible;nX++)
		{
			char string[30];
			if(nX % 12 == 0 && m_Keys[nX].m_csName.IsEmpty())
			{
				m_Keys[nX].m_csName.Format("%d", (nX/12) + 1);
			}
			if (!m_Keys[nX].m_csName.IsEmpty())
			{
				strcpy(string,m_Keys[nX].m_csName);
				string[3] = 0;
				if (m_Keys[nX].m_fBlackKey)
				{
					pDC->SetTextColor(RGB(0xFF,0xFF,0xFF));
				}
				else
				{
					pDC->SetTextColor(RGB(0,0,0));
				}
				pDC->ExtTextOut(m_Keys[nX].m_crDrawRect.TopLeft().x,
					m_Keys[nX].m_crDrawRect.BottomRight().y - 14,
					ETO_CLIPPED,
					m_Keys[nX].m_crDrawRect,
					m_Keys[nX].m_csName, NULL);
			}
		} 
		pDC->SelectObject(oldfont); 
		pDC->SetBkMode(oldmode);
		pDC->SetTextColor(oldcolor);
	} 
}

void CKeyboard::SetNewSize(const CRect& crNewRect)

{
	int nX;
	double dKeyWidth = crNewRect.Width();
	double widthDivisor = 7.*(m_visible/12.);	// heuristic: 7 for each octave
	dKeyWidth /= widthDivisor;
	double dLeft = crNewRect.TopLeft().x;
	int dBlackHeight = crNewRect.Height() * 6;
	dBlackHeight /= 10;
	int nBlackWidth = (int) dKeyWidth * 2 + 2;
	nBlackWidth /= 3;
	for (nX = m_lowerbound; nX < m_lowerbound+m_visible; nX++)
	{
		if (m_Keys[nX].m_fBlackKey)
		{
			if(m_lowerbound < nX && nX < m_lowerbound + m_visible - 1)
			{
				m_Keys[nX].m_crDrawRect.SetRect(
						(int) (dLeft - dKeyWidth) + nBlackWidth,
						crNewRect.TopLeft().y,
						(int) (dLeft - dKeyWidth) + nBlackWidth + nBlackWidth,
						crNewRect.TopLeft().y + dBlackHeight);
			}
			else if(nX == m_lowerbound)
			{
				// make sure we don't paint over the edges of controls
				m_Keys[nX].m_crDrawRect.SetRect(
						(int) (dLeft - dKeyWidth + nBlackWidth/2) + nBlackWidth,
						crNewRect.TopLeft().y,
						(int) (dLeft - dKeyWidth) + nBlackWidth + nBlackWidth,
						crNewRect.TopLeft().y + dBlackHeight);
			}
			else
			{
				// make sure we don't paint over the edges of controls
				m_Keys[nX].m_crDrawRect.SetRect(
						(int) (dLeft - dKeyWidth) + nBlackWidth,
						crNewRect.TopLeft().y,
						(int) (dLeft - dKeyWidth) + nBlackWidth + nBlackWidth/2,
						crNewRect.TopLeft().y + dBlackHeight);
			}
		}
		else
		{
			m_Keys[nX].m_crDrawRect.SetRect(
				(int) dLeft,
				crNewRect.TopLeft().y,
				(int) (dLeft+dKeyWidth),
				crNewRect.BottomRight().y);
			dLeft += dKeyWidth; 
		}
	}
}

void CKeyboard::SetNoteState(short nKey, LPCTSTR pszName, short nState) 
{
	nKey = static_cast<short>(nKey + m_transpose);
	if ((nKey < Size) && (nKey >= 0))
	{
		m_Keys[nKey].m_csName = pszName;
		m_Keys[nKey].SetState(nState);
		Invalidate(FALSE);
//		CClientDC dc(this);
//		DrawKeyboard(&dc);
	}
}

void CKeyboard::OnLButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	short nX = static_cast<short>(FindKey(point));
//	ASSERT(0<= (nX-m_transpose) && (nX-m_transpose) <= m_visible);
	TRACE("Mouse down on %d\n", nX);
	if(m_pfnLButtonDown)
	{
		m_pfnLButtonDown(this, m_pHintLButtonDown, nX);
	}
	CButton::OnLButtonDown(nFlags, point);
}

void CKeyboard::OnLButtonUp(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	short nX = static_cast<short>(FindKey(point));
	TRACE("Mouse up on %d\n", nX);
	if(m_pfnLButtonUp)
	{
		m_pfnLButtonUp(this, m_pHintLButtonUp, nX);
	}
	CButton::OnLButtonUp(nFlags, point);
}

int CKeyboard::FindKey(CPoint point)

{
	int nX;
	for (nX = m_lowerbound; nX < m_visible + m_lowerbound; nX++)
	{
		if (m_Keys[nX].m_fBlackKey)
		{
			if (m_Keys[nX].m_crDrawRect.PtInRect(point))
				return nX;
		}
	}
	for (nX = m_lowerbound; nX < m_visible + m_lowerbound; nX++)
	{
		if (!m_Keys[nX].m_fBlackKey)
		{
			if (m_Keys[nX].m_crDrawRect.PtInRect(point))
				return nX;
		}
	}
	return -1;
}

void CKeyboard::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
//	CPaintDC dc(this); // device context for painting
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect(lpDrawItemStruct->rcItem);
	m_crBoundRect = rect;
	SetNewSize(m_crBoundRect);
	DrawKeyboard(pDC);
	
}

void CKeyboard::ClearKeys()
{
	for(int i = 0; i < Size; i++)
	{
		SetNoteState(static_cast<short>(i), "", 0);
	}
}

BOOL CKeyboard::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	return CButton::OnCommand(wParam, lParam);
}

void CKeyboard::OnRButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	short nX = static_cast<short>(FindKey(point));
	TRACE("RightMouse(%d,%d) down on %d\n", point.x, point.y, nX);
	if(m_pfnRButtonDown)
	{
		m_pfnRButtonDown(this, m_pHintRButtonDown, nX, point);
	}
	CButton::OnRButtonDown(nFlags, point);
}

void CKeyboard::SetRootKeys(short n)
{
	m_nRootKeys = n;
	for(short i = 0; i < Size; i++)
	{
		if(i < m_nRootKeys)
		{
			m_Keys[i].m_fRootKey = TRUE;
		}
		else
		{
			m_Keys[i].m_fRootKey = FALSE;
		}
	}
}