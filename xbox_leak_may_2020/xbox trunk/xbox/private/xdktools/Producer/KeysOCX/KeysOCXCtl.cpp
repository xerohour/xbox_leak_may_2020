// KeysOCXCtl.cpp : Implementation of the CKeysOCXCtrl ActiveX Control class.

#include "stdafx.h"
#include "KeysOCX.h"
#include "KeysOCXCtl.h"
#include "KeysOCXPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CKeysOCXCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CKeysOCXCtrl, COleControl)
	//{{AFX_MSG_MAP(CKeysOCXCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CKeysOCXCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CKeysOCXCtrl)
	DISP_FUNCTION(CKeysOCXCtrl, "SetNoteState", SetNoteState, VT_EMPTY, VTS_I2 VTS_BSTR VTS_I2)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CKeysOCXCtrl, COleControl)
	//{{AFX_EVENT_MAP(CKeysOCXCtrl)
	EVENT_CUSTOM("NoteDown", FireNoteDown, VTS_I2)
	EVENT_CUSTOM("NoteUp", FireNoteUp, VTS_I2)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CKeysOCXCtrl, 1)
	PROPPAGEID(CKeysOCXPropPage::guid)
END_PROPPAGEIDS(CKeysOCXCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CKeysOCXCtrl, "KEYSOCX.KeysOCXCtrl.1",
	0xcdd09f86, 0xe73c, 0x11d0, 0x89, 0xab, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CKeysOCXCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DKeysOCX =
		{ 0xcdd09f84, 0xe73c, 0x11d0, { 0x89, 0xab, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const IID BASED_CODE IID_DKeysOCXEvents =
		{ 0xcdd09f85, 0xe73c, 0x11d0, { 0x89, 0xab, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwKeysOCXOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CKeysOCXCtrl, IDS_KEYSOCX, _dwKeysOCXOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CKeysOCXCtrl::CKeysOCXCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CKeysOCXCtrl

BOOL CKeysOCXCtrl::CKeysOCXCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_KEYSOCX,
			IDB_KEYSOCX,
			afxRegApartmentThreading,
			_dwKeysOCXOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

KeyInfo::KeyInfo()

{
	m_nState = 0;
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
// CKeysOCXCtrl::CKeysOCXCtrl - Constructor

CKeysOCXCtrl::CKeysOCXCtrl()
{
	InitializeIIDs(&IID_DKeysOCX, &IID_DKeysOCXEvents);
	static BOOL fBlackKey[24] = { 
		FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, FALSE,
		FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, TRUE, FALSE
	};
	int nX;
	for (nX = 0; nX < 24; nX++)
	{
		m_Keys[nX].m_fBlackKey = fBlackKey[nX];
		m_Keys[nX].SetState(0);
	}
	m_crBoundRect.SetRectEmpty();
}

int CKeysOCXCtrl::FindKey(CPoint point)

{
	int nX;
	for (nX = 0; nX < 24; nX++)
	{
		if (m_Keys[nX].m_fBlackKey)
		{
			if (m_Keys[nX].m_crDrawRect.PtInRect(point))
				return nX;
		}
	}
	for (nX = 0; nX < 24; nX++)
	{
		if (!m_Keys[nX].m_fBlackKey)
		{
			if (m_Keys[nX].m_crDrawRect.PtInRect(point))
				return nX;
		}
	}
	return -1;
}

void CKeysOCXCtrl::DrawKeyboard(CDC* pDC)
{
	int nX;
	CBrush Frame;
	Frame.CreateSolidBrush( 0 );
	for (nX = 0; nX < 24; nX++)
	{
		if (!m_Keys[nX].m_fBlackKey)
		{
			CBrush Color;
			Color.CreateSolidBrush( m_Keys[nX].m_Color );
			pDC->FillRect(m_Keys[nX].m_crDrawRect,&Color);
			pDC->FrameRect(m_Keys[nX].m_crDrawRect,&Frame);
		}
	}
	for (nX = 0; nX < 24; nX++)
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
		for (nX=0;nX<24;nX++)
		{
			char string[30];
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

void CKeysOCXCtrl::SetNewSize(const CRect& crNewRect)

{
	int nX;
	double dKeyWidth = crNewRect.Width();
	dKeyWidth /= 14;
	double dLeft = crNewRect.TopLeft().x;
	int dBlackHeight = crNewRect.Height() * 6;
	dBlackHeight /= 10;
	int nBlackWidth = (int) dKeyWidth * 2 + 2;
	nBlackWidth /= 3;
	for (nX = 0; nX < 24; nX++)
	{
		if (m_Keys[nX].m_fBlackKey)
		{
			m_Keys[nX].m_crDrawRect.SetRect(
				(int) (dLeft - dKeyWidth) + nBlackWidth,
				crNewRect.TopLeft().y,
				(int) (dLeft - dKeyWidth) + nBlackWidth + nBlackWidth,
				crNewRect.TopLeft().y + dBlackHeight);
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

/////////////////////////////////////////////////////////////////////////////
// CKeysOCXCtrl::~CKeysOCXCtrl - Destructor

CKeysOCXCtrl::~CKeysOCXCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CKeysOCXCtrl::OnDraw - Drawing function

void CKeysOCXCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (rcBounds != m_crBoundRect)
	{
		SetNewSize(rcBounds);
		m_crBoundRect = rcBounds;
	}
	DrawKeyboard(pdc);
}


/////////////////////////////////////////////////////////////////////////////
// CKeysOCXCtrl::DoPropExchange - Persistence support

void CKeysOCXCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CKeysOCXCtrl::OnResetState - Reset control to default state

void CKeysOCXCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CKeysOCXCtrl message handlers

void CKeysOCXCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	short nX = FindKey(point);
	FireNoteDown(nX);
	COleControl::OnLButtonDown(nFlags, point);
}

void CKeysOCXCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	short nX = FindKey(point);
	FireNoteUp(nX);
	COleControl::OnLButtonUp(nFlags, point);
}

void CKeysOCXCtrl::SetNoteState(short nKey, LPCTSTR pszName, short nState) 
{
	if ((nKey < 24) && (nKey >= 0))
	{
		m_Keys[nKey].m_csName = pszName;
		m_Keys[nKey].SetState(nState);
		CClientDC dc(this);
		DrawKeyboard(&dc);
	}
}
