// ADSREnvelopeCtl.cpp : Implementation of the CADSREnvelopeCtrl ActiveX Control class.

#include "stdafx.h"
#include "ADSREnvelope.h"
#include "ADSREnvelopeCtl.h"
#include "ADSREnvelopePpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CADSREnvelopeCtrl, COleControl)
const int HALF_POS_BOX_HW = 4;
const int UI_MARGIN = HALF_POS_BOX_HW;
static const COLORREF arrPositionBoxColors[SEGMENT_THUMBS] = {DELAY_COLOR, ATTACK_COLOR, HOLD_COLOR, DECAY_COLOR, RELEASE_COLOR};

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CADSREnvelopeCtrl, COleControl)
	//{{AFX_MSG_MAP(CADSREnvelopeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CADSREnvelopeCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CADSREnvelopeCtrl)
	DISP_PROPERTY_EX(CADSREnvelopeCtrl, "ReleaseTime", GetReleaseTime, SetReleaseTime, VT_I4)
	DISP_PROPERTY_EX(CADSREnvelopeCtrl, "AttackTime", GetAttackTime, SetAttackTime, VT_I4)
	DISP_PROPERTY_EX(CADSREnvelopeCtrl, "DecayTime", GetDecayTime, SetDecayTime, VT_I4)
	DISP_PROPERTY_EX(CADSREnvelopeCtrl, "SustainLevel", GetSustainLevel, SetSustainLevel, VT_I4)
	DISP_PROPERTY_EX(CADSREnvelopeCtrl, "DelayTime", GetDelayTime, SetDelayTime, VT_I4)
	DISP_PROPERTY_EX(CADSREnvelopeCtrl, "HoldTime", GetHoldTime, SetHoldTime, VT_I4)
	DISP_FUNCTION(CADSREnvelopeCtrl, "SetDLS1", SetDLS1, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CADSREnvelopeCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CADSREnvelopeCtrl, COleControl)
	//{{AFX_EVENT_MAP(CADSREnvelopeCtrl)
	EVENT_CUSTOM("ADSREDecayTimeChanged", FireADSREDecayTimeChanged, VTS_I4)
	EVENT_CUSTOM("ADSREAttackTimeChanged", FireADSREAttackTimeChanged, VTS_I4)
	EVENT_CUSTOM("ADSREReleaseTimeChanged", FireADSREReleaseTimeChanged, VTS_I4)
	EVENT_CUSTOM("ADSRESustainLevelChanged", FireADSRESustainLevelChanged, VTS_I4)
	EVENT_CUSTOM("RealDecayChanged", FireRealDecayChanged, VTS_I4)
	EVENT_CUSTOM("RealReleaseChanged", FireRealReleaseChanged, VTS_I4)
	EVENT_CUSTOM("ADSRMouseMoveStart", FireADSRMouseMoveStart, VTS_NONE)
	EVENT_CUSTOM("ADSRHoldTimeChanged", FireADSRHoldTimeChanged, VTS_I4)
	EVENT_CUSTOM("ADSREDelayTimeChanged", FireADSREDelayTimeChanged, VTS_I4)
	EVENT_CUSTOM("ADSREHoldTimeChanged", FireADSREHoldTimeChanged, VTS_I4)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CADSREnvelopeCtrl, 1)
	PROPPAGEID(CADSREnvelopePropPage::guid)
END_PROPPAGEIDS(CADSREnvelopeCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CADSREnvelopeCtrl, "ADSRENVELOPE.ADSREnvelopeCtrl.1",
	0x71ae3626, 0xa9bd, 0x11d0, 0xbc, 0xba, 0, 0xaa, 0, 0xc0, 0x81, 0x46)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CADSREnvelopeCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DADSREnvelope =
		{ 0x71ae3624, 0xa9bd, 0x11d0, { 0xbc, 0xba, 0, 0xaa, 0, 0xc0, 0x81, 0x46 } };
const IID BASED_CODE IID_DADSREnvelopeEvents =
		{ 0x71ae3625, 0xa9bd, 0x11d0, { 0xbc, 0xba, 0, 0xaa, 0, 0xc0, 0x81, 0x46 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwADSREnvelopeOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CADSREnvelopeCtrl, IDS_ADSRENVELOPE, _dwADSREnvelopeOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopeCtrl::CADSREnvelopeCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CADSREnvelopeCtrl

BOOL CADSREnvelopeCtrl::CADSREnvelopeCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_ADSRENVELOPE,
			IDB_ADSRENVELOPE,
			afxRegApartmentThreading,
			_dwADSREnvelopeOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopeCtrl::CADSREnvelopeCtrl - Constructor

CADSREnvelopeCtrl::CADSREnvelopeCtrl() : 
				   m_bHasCapture(false), 
				   m_nSelPB(0), 
				   m_nOnTop(ATTACK),
				   m_bFireUndoNotify(false),
				   m_lDelayTime(0),
				   m_lATime(0), 
				   m_lHTime(0),
				   m_lDTime(0),
				   m_lSLevel(0),
				   m_lRTime(0), 
				   m_nDelayPixels(0),
				   m_nAPixels(0),
				   m_nHPixels(0),
				   m_nDPixels(0), 
				   m_nRPixels(0), 
				   m_nMaxPixels(0),
				   m_nSegmentLength(0),
				   m_TimeCentsPerPixel(0),
				   m_PixelsPerTenthPercent(0),
				   m_bDLS1(FALSE)
{
	InitializeIIDs(&IID_DADSREnvelope, &IID_DADSREnvelopeEvents);

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopeCtrl::~CADSREnvelopeCtrl - Destructor

CADSREnvelopeCtrl::~CADSREnvelopeCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopeCtrl::OnDraw - Drawing function

void CADSREnvelopeCtrl::OnDraw(CDC* pdc, 
							   const CRect& rcBounds, 
							   const CRect& rcInvalid)
{
	// MFC based containers call OnDraw multiple times with different DC's.
	// We only want to paint once and when DC is client area of control.
	// This should happen when both rcBounds.top = 0 and rcBounds.left = 0
	if(rcBounds.top != 0 && rcBounds.left != 0)
	{
		//TRACE0("Returning from ADSREnvelope control's OnDraw call without drawing\n");
		return;
	}

	//TRACE0("ADSREnvelope control's OnDraw call and going to draw\n");
	UpdateADSREnvelope(pdc, &rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopeCtrl::DoPropExchange - Persistence support

void CADSREnvelopeCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopeCtrl::OnResetState - Reset control to default state

void CADSREnvelopeCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopeCtrl::AboutBox - Display an "About" box to the user

void CADSREnvelopeCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog dlgAbout(IDD_ABOUTBOX_ADSRENVELOPE);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CADSREnvelopeCtrl message handlers

void CADSREnvelopeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nOverlappedBoxes = 0;
	int nLastBox = 0;
	for(int nBoxNo = 0; nBoxNo < SEGMENT_THUMBS; nBoxNo++)
	{
		if (ADRVisible(START+1+nBoxNo) && m_rcADRPosBox[nBoxNo].PtInRect(point))
		{
			nLastBox = nBoxNo;
			nOverlappedBoxes++;
		}
	}
	if (nOverlappedBoxes == 0)
		return;
	ASSERT((m_nOnTop > START) && (m_nOnTop < END));
	if ((nOverlappedBoxes <= 1) || !m_rcADRPosBox[m_nOnTop-1].PtInRect(point))
		m_nOnTop = nLastBox + 1;
	ASSERT((m_nOnTop > START) && (m_nOnTop < END));
	ASSERT(m_rcADRPosBox[m_nOnTop-1].PtInRect(point));
	SetCapture();
	m_bHasCapture = true;
	m_nSelPB = m_nOnTop;

	ASSERT(ADRVisible(m_nSelPB)); // handle shouldn't be visible or selectable
	m_ptPrevMousePos = point;
	m_bFireUndoNotify = true;
}

void CADSREnvelopeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bHasCapture)
	{
		ReleaseCapture();
		m_bHasCapture = false;
		m_nSelPB = 0;
		m_ptPrevMousePos = (0,0);
	}

	int nBoxCount = m_nOnTop;

	for(int nCount = 0; nCount < SEGMENT_THUMBS; nCount++)
	{
		if(nBoxCount >= SEGMENT_THUMBS || nBoxCount < 0)
		{
			nBoxCount = 0;
		}
		if(ADRVisible(nBoxCount+1) && m_rcADRPosBox[nBoxCount].PtInRect(point))
		{
			m_nOnTop = nBoxCount+1;
			break;
		}
		nBoxCount++;
	}

	CClientDC ctrlDC(this);
	UpdateADSREnvelope(&ctrlDC);
}

void CADSREnvelopeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	long newX = 0;
	long newY = 0;
	long newRealPixels = 0;
	long delta_x = 0;
	long delta_y = 0;

	if(!m_bHasCapture)
	{
		return;
	}

	ASSERT((m_nSelPB > 0 && m_nSelPB < SEGMENTS));

	if (m_bFireUndoNotify)
	{
		FireADSRMouseMoveStart();
		m_bFireUndoNotify = false;
	}
	
	delta_x = point.x - m_ptPrevMousePos.x;
	delta_y = point.y - m_ptPrevMousePos.y;

	switch(m_nSelPB)
	{
		case DELAY:
		{
			// Can't change delay for DLS1 envelop
			if(m_bDLS1)
			{
				ASSERT(FALSE); // shouldn't be visible or selectable
				break;
			}

			// No response to y changes
			newX = m_nDelayPixels + delta_x;
			if(newX > m_nSegmentLength) 
			{
				newX = m_nSegmentLength;
				delta_x = newX - m_nDelayPixels;
			}
			else if (newX < 0) 
			{
				newX = 0;
				delta_x = -m_nDelayPixels;
			}

			m_nDelayPixels = newX;
			m_ptADRPts[DELAY].x = newX;

			// Attack drag point shifts with Delay
			if(m_ptADRPts[ATTACK].x != m_ptADRPts[DELAY].x)
			{
				m_ptADRPts[ATTACK].x += delta_x;
				m_nAPixels = m_ptADRPts[ATTACK].x;
				
				// The hold drag point shifts with attack
				m_ptADRPts[HOLD].x += delta_x;
				m_nHPixels = m_ptADRPts[HOLD].x;

				// The decay drag point shifts with hold
				m_ptADRPts[DECAY].x += delta_x;
			}

			m_lDelayTime = (long) (m_nDelayPixels * m_TimeCentsPerPixel + 0.5) + TIMECENTS_MIN;
			FireADSREDelayTimeChanged(TimeCents2Mils(m_lDelayTime));

			break;
		}

		// Attack changes a straightfoward. The x component of the drag point IS the Attack time
		case ATTACK:
		{
			int nMaxAttackLength = m_nDelayPixels + m_nSegmentLength;

			// There is no response to y changes
			newX = m_nAPixels + delta_x;
			newX = newX > nMaxAttackLength ? nMaxAttackLength : newX;
			newX = newX < m_nDelayPixels ? m_nDelayPixels : newX;
			newX = newX < 0 ? 0 : newX;
			
			delta_x = newX - m_nAPixels;
			m_nAPixels = newX;
			m_ptADRPts[ATTACK].x = newX;
			
			// The hold drag point shifts with attack
			if(m_ptADRPts[HOLD].x != m_ptADRPts[ATTACK].x)
			{
				m_ptADRPts[HOLD].x += delta_x;
				m_nHPixels = m_ptADRPts[HOLD].x;

				// The decay drag point shifts with hold
				m_ptADRPts[DECAY].x += delta_x;
			}

			m_lATime = (long) ((m_nAPixels - m_nDelayPixels) * m_TimeCentsPerPixel + 0.5) + TIMECENTS_MIN;
			FireADSREAttackTimeChanged(TimeCents2Mils(m_lATime));

			break;
		}

		case HOLD:
		{
			// Can't change delay for DLS1 envelop
			if(m_bDLS1)
			{
				ASSERT(FALSE); // shouldn't be visible or selectable
				break;
			}

			int nMaxHoldLength = m_nAPixels + m_nSegmentLength;

			// There is no response to y changes
			newX = m_nHPixels + delta_x;
			newX = newX > nMaxHoldLength ? nMaxHoldLength : newX;
			newX = newX < m_nAPixels ? m_nAPixels : newX;
			newX = newX < 0 ? 0 : newX;
			
			delta_x = newX - m_nHPixels;
			m_nHPixels = newX;
			m_ptADRPts[HOLD].x = newX;
			
			// The decay drag point shifts with hold
			if(m_ptADRPts[DECAY].x != m_ptADRPts[HOLD].x)
			{
				m_ptADRPts[DECAY].x += delta_x;
			}

			m_lHTime = (long) ((m_nHPixels - m_nAPixels) * m_TimeCentsPerPixel + 0.5) + TIMECENTS_MIN;
			FireADSREHoldTimeChanged(TimeCents2Mils(m_lHTime));

			break;
		}

		case DECAY:
		{
			int nMaxDecayLength = (m_nHPixels + m_nSegmentLength) / 2;

			newX = m_ptADRPts[DECAY].x + delta_x;
			newY = m_ptADRPts[DECAY].y + delta_y;
			if (newY > m_nHeight)
			{
				newY = m_nHeight;
			}
			if (newY < 0)
			{
				newY = 0;
			}

			m_ptADRPts[RELEASE].y = m_ptADRPts[DECAY].y = newY;
			
			// We have to extrapolate the actual (DLS spec) Decay Time from the drag point.
			if(newY)
			{
				newRealPixels = MUL_DIV(m_nHeight, newX - m_nHPixels, newY);
			}
			else // avoid divide-by-zero error
			{
				newRealPixels = MUL_DIV(m_nHeight, newX - m_nHPixels, 1);
			}

			if(TimeCents2Mils(m_lDTime) < 40000 && newRealPixels >= m_nSegmentLength)
			{
				newRealPixels = m_nSegmentLength;
			}
			if(TimeCents2Mils(m_lDTime) > 00000 && newX  <= m_nHPixels)
			{
				newRealPixels = 0;
				newX = m_nHPixels;
			}
			
			if ((newRealPixels <= m_nSegmentLength) && (newX >= m_nHPixels))
			{
				m_nDPixels = newRealPixels;
				m_ptADRPts[DECAY].x = newX;
				m_lDTime = (long) (m_nDPixels * m_TimeCentsPerPixel + 0.5) + TIMECENTS_MIN;
				FireADSREDecayTimeChanged(TimeCents2Mils(m_lDTime));
				//calculate the TimeCent value of the x component of the drag point.
				//this is the "real" time in terms of the sound of the instruments articulation
				UpdateRealDecayTime();
			}
			else if (delta_y)//even if we are maxed/mined out on the x we may have had a change in y. So
			{//the drag point must "slide" down its line
				m_ptADRPts[DECAY].x = m_nHPixels + MUL_DIV(m_ptADRPts[DECAY].y, m_nDPixels, m_nHeight);
				UpdateRealDecayTime();
			}
			if (delta_y)//in either case changes in y (i.e. sustain level) require
			{//the release drag point to "slide" down its line
				m_ptADRPts[RELEASE].x = (m_nWidth - 1) - MUL_DIV(m_nHeight - newY, m_nRPixels, m_nHeight);
				UpdateRealReleaseTime();
				m_lSLevel = (long) (((m_nHeight - m_ptADRPts[DECAY].y) / m_PixelsPerTenthPercent) + 0.5);
				FireADSRESustainLevelChanged(m_lSLevel);
			}
			break;
		}

		case RELEASE:
		{
			int nMaxReleaseLength = (m_nHPixels + m_nSegmentLength) / 2;

			newX = m_ptADRPts[RELEASE].x + delta_x;
			newY = m_ptADRPts[RELEASE].y + delta_y;

			if (newY > m_nHeight)
			{
				newY = m_nHeight;
			}
			if (newY < 0)
			{
				newY = 0;
			}

			m_ptADRPts[DECAY].y = m_ptADRPts[RELEASE].y = newY;
			
			// We have to extrapolate the actual (DLS spec) Release Time from the drag point.
			if(newY < m_nHeight)
			{
				newRealPixels = MUL_DIV((m_nWidth - 1) - newX, m_nHeight, m_nHeight - newY);
			}
			else
			{
				newRealPixels = MUL_DIV((m_nWidth - 1) - newX, m_nHeight, 1);
			}

			if(TimeCents2Mils(m_lRTime) < 40000 && newRealPixels > m_nSegmentLength)
			{
				newRealPixels = m_nSegmentLength;
			}
			
			if(TimeCents2Mils(m_lRTime) > 00000 && newX  < m_nDPixels)
			{
				newRealPixels = m_nDPixels;
				newX = m_nDPixels;
			}

			if((newRealPixels <= m_nSegmentLength) && (newX < m_nWidth))
			{
				m_nRPixels = newRealPixels;
				m_ptADRPts[RELEASE].x = newX;
				m_lRTime = (long) (m_nRPixels * m_TimeCentsPerPixel + 0.5) + TIMECENTS_MIN;
				FireADSREReleaseTimeChanged(TimeCents2Mils(m_lRTime));
				
				// Calculate the TimeCent value of the x component of the drag point.
				// This is the "real" time in terms of the sound of the instruments articulation
				UpdateRealReleaseTime();
			}
			// Even if we are maxed/mined out on the x we may have had a change in y. So
			// the drag point must "slide" down its line
			else if(delta_y)
			{
				m_ptADRPts[RELEASE].x = (m_nWidth-1)- MUL_DIV(m_nHeight - newY, m_nRPixels, m_nHeight);
				UpdateRealReleaseTime();
			}
			
			// In either case changes in y (i.e. sustain level) require the release drag point to "slide" down its line
			if(delta_y)	
			{				
				m_ptADRPts[DECAY].x = m_nHPixels + MUL_DIV(m_ptADRPts[DECAY].y, m_nDPixels, m_nHeight);
				UpdateRealDecayTime();
				m_lSLevel = (long) (((m_nHeight - m_ptADRPts[RELEASE].y) / m_PixelsPerTenthPercent) + 0.5);
				FireADSRESustainLevelChanged(m_lSLevel);
			}
			break;
		}
	}
	m_ptPrevMousePos = point;

	CClientDC* pCtrlDC = new CClientDC(this);
	UpdateADSREnvelope(pCtrlDC);
	delete pCtrlDC;
}


void CADSREnvelopeCtrl::UpdateADSREnvelope(CDC *pDC, const CRect* rcBounds)
{
	// background: white box with a margin colored like the dialog
	CRect rcClient, rcDraw;
	const COLORREF rgbMargin = GetSysColor(COLOR_BTNFACE);
	GetClientRect(&rcClient);
	rcDraw.SetRect(rcClient.left, rcClient.top, rcClient.right, rcClient.top+UI_MARGIN);
	pDC->FillSolidRect(&rcDraw, rgbMargin);
	rcDraw.SetRect(rcClient.left, rcClient.top+UI_MARGIN, rcClient.left+UI_MARGIN, rcClient.bottom-UI_MARGIN);
	pDC->FillSolidRect(&rcDraw, rgbMargin);
	rcDraw.SetRect(rcClient.right-UI_MARGIN, rcClient.top+UI_MARGIN, rcClient.right, rcClient.bottom-UI_MARGIN);
	pDC->FillSolidRect(&rcDraw, rgbMargin);
	rcDraw.SetRect(rcClient.left, rcClient.bottom-UI_MARGIN, rcClient.right, rcClient.bottom);
	pDC->FillSolidRect(&rcDraw, rgbMargin);
	rcClient.InflateRect(-UI_MARGIN, -UI_MARGIN);
	pDC->FillSolidRect(&rcClient, PALETTERGB(255, 255, 255));

	// articulation segments
	int nPrevMode = pDC->SetROP2(R2_COPYPEN);
	pDC->MoveTo(UI_MARGIN+m_ptADRPts[0].x, UI_MARGIN+m_ptADRPts[0].y);
	for (int i = 0; i < SEGMENTS; i++)
		{
		BYTE iY = (i == DELAY-1) ? 0 : i+1;
		COLORREF cr;
		switch (i)
			{
		case 5:
			cr = RELEASE_COLOR;
			break;

		case 4:
			cr = SUSTAIN_COLOR;
			break;
		
		default:
			ASSERT(i <= 3);
			cr = arrPositionBoxColors[i];
			break;
			}
		
		CPen pen(PS_SOLID, 1, cr);
		CPen *pOldPen = pDC->SelectObject(&pen);
		pDC->LineTo(UI_MARGIN+m_ptADRPts[i+1].x , UI_MARGIN+m_ptADRPts[iY].y);
		pDC->SelectObject(pOldPen);
		}
	pDC->SetROP2(nPrevMode);

	// Create handle used to adjust ADR times
	for (i = 0; i < SEGMENT_THUMBS; i++)
		{
		int iY = (i == DELAY-1) ? 0 : i+1;
		m_rcADRPosBox[i].SetRect(
			UI_MARGIN + m_ptADRPts[i+1].x - HALF_POS_BOX_HW, 
			UI_MARGIN + m_ptADRPts[iY].y - HALF_POS_BOX_HW, 
			UI_MARGIN + m_ptADRPts[i+1].x + HALF_POS_BOX_HW, 
			UI_MARGIN + m_ptADRPts[iY].y + HALF_POS_BOX_HW);
		}

	for (i = 0; i < SEGMENT_THUMBS; i++)
	{
		// don't draw handles that can't be drawn
		if (!ADRVisible(i+1))
			continue;

		pDC->FillSolidRect(m_rcADRPosBox[i], arrPositionBoxColors[i]);
	}

	// draw the one on top
	if(m_nOnTop > 0)
		pDC->FillSolidRect(m_rcADRPosBox[m_nOnTop - 1], arrPositionBoxColors[m_nOnTop - 1]);
}

int CADSREnvelopeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	m_lDelayTime = -11960;
	m_lATime = 173;
	m_lHTime = 6387;
	m_lDTime = -10000;
	m_lRTime = 347;
	m_lSLevel = 750;

	RECT rect;
	GetClientRect(&rect);
	InflateRect(&rect, -UI_MARGIN, -UI_MARGIN);

	m_nWidth = rect.right;
	m_nHeight = rect.bottom;

	m_nMaxPixels = m_nWidth / 2;
	m_nSegmentLength = m_nWidth / 4;

	m_TimeCentsPerPixel = (double)TIMECENTS_RANGE / m_nSegmentLength;
	m_PixelsPerTenthPercent = m_nHeight / 1000.0;

	m_nDelayPixels = (long) (((m_lDelayTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	m_nAPixels = (long) (((m_lATime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	m_nHPixels = (long) (((m_lHTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	m_nDPixels = (long) (((m_lDTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	m_nRPixels = (long) (((m_lRTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);

	m_ptADRPts[START].x = 0;
	m_ptADRPts[START].y = m_nHeight - 1;

	m_ptADRPts[DELAY].x = m_nDelayPixels;
	m_ptADRPts[DELAY].y = 0;

	m_ptADRPts[ATTACK].x = m_nAPixels;
	m_ptADRPts[ATTACK].y = 0;

	m_ptADRPts[HOLD].x = m_nHPixels;
	m_ptADRPts[HOLD].y = 0;
	
	m_ptADRPts[DECAY].y = m_nHeight - (long)((m_lSLevel * m_PixelsPerTenthPercent) + 0.5);
	m_ptADRPts[DECAY].x = m_nAPixels + MUL_DIV(m_ptADRPts[DECAY].y, m_nDPixels, m_nHeight);
	
	m_ptADRPts[RELEASE].y = m_ptADRPts[DECAY].y;
	m_ptADRPts[RELEASE].x = (m_nWidth-1) - MUL_DIV(m_nHeight - m_ptADRPts[RELEASE].y, m_nRPixels, m_nHeight );
	
	m_ptADRPts[END].x = m_nWidth - 1;
	m_ptADRPts[END].y = m_nHeight - 1;

	return 0;
}

long CADSREnvelopeCtrl::GetAttackTime() 
{
	return (long) TimeCents2Mils(m_lATime);
}

void CADSREnvelopeCtrl::SetAttackTime(long nNewValue) 
{
	ASSERT(nNewValue >= 1 && nNewValue <= 40000);		
	
	m_lATime = Mils2TimeCents(nNewValue);

	m_nAPixels = m_nDelayPixels + (long) (((m_lATime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	int nDeltaX = m_nAPixels - m_ptADRPts[ATTACK].x; 

	m_ptADRPts[ATTACK].x = m_nAPixels;

	// Hold drag point changes with attack
	m_ptADRPts[HOLD].x += nDeltaX;
	m_nHPixels = m_ptADRPts[HOLD].x;

	// The decay drag point shifts with hold
	m_ptADRPts[DECAY].x += nDeltaX;
	m_nDPixels = m_ptADRPts[DECAY].x;
	
	CClientDC ctrlDC(this);
	UpdateADSREnvelope(&ctrlDC);

	SetModifiedFlag();
	
	FireADSREAttackTimeChanged(nNewValue);
}

long CADSREnvelopeCtrl::GetDecayTime() 
{
	return (long) TimeCents2Mils(m_lDTime);
}

void CADSREnvelopeCtrl::SetDecayTime(long nNewValue) 
{

	ASSERT(nNewValue >= 1 && nNewValue <= 40000);		
	
	m_lDTime = Mils2TimeCents(nNewValue);

	m_nDPixels = (long) (((m_lDTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	m_ptADRPts[DECAY].x = m_nHPixels + MUL_DIV(m_ptADRPts[DECAY].y, m_nDPixels, m_nHeight);

	CClientDC ctrlDC(this);
	UpdateADSREnvelope(&ctrlDC);

	SetModifiedFlag();
	
	UpdateRealDecayTime();

	FireADSREDecayTimeChanged(nNewValue);
}

long CADSREnvelopeCtrl::GetSustainLevel() 
{
	return m_lSLevel;
}

void CADSREnvelopeCtrl::SetSustainLevel(long nNewValue) 
{
	ASSERT(nNewValue >= 0 && nNewValue <= 1000);

	m_lSLevel = nNewValue;

	m_ptADRPts[DECAY].y = m_nHeight - (long)((m_lSLevel * m_PixelsPerTenthPercent) + 0.5);
	m_ptADRPts[DECAY].x = m_nHPixels + MUL_DIV(m_ptADRPts[DECAY].y, m_nDPixels, m_nHeight);

	m_ptADRPts[RELEASE].y = m_ptADRPts[DECAY].y;
	m_ptADRPts[RELEASE].x = (m_nWidth-1) - MUL_DIV(m_nHeight - m_ptADRPts[RELEASE].y, m_nRPixels, m_nHeight);

	CClientDC ctrlDC(this);
	UpdateADSREnvelope(&ctrlDC);

	FireADSRESustainLevelChanged(nNewValue);
	UpdateRealDecayTime();
	UpdateRealReleaseTime();
}

long CADSREnvelopeCtrl::GetReleaseTime() 
{
	return (long) TimeCents2Mils(m_lRTime);
}

void CADSREnvelopeCtrl::SetReleaseTime(long nNewValue) 
{

	ASSERT(nNewValue >= 1 && nNewValue <= 40000);		
	
	m_lRTime = Mils2TimeCents(nNewValue);

	m_nRPixels = (long) (((m_lRTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	m_ptADRPts[RELEASE].x = (m_nWidth-1) - MUL_DIV(m_nHeight - m_ptADRPts[RELEASE].y, m_nRPixels, m_nHeight );
	
	CClientDC ctrlDC(this);
	UpdateADSREnvelope(&ctrlDC);

	SetModifiedFlag();
	UpdateRealReleaseTime();
	FireADSREReleaseTimeChanged(nNewValue);
}

void CADSREnvelopeCtrl::OnSize(UINT nType, int cx, int cy) 
{
	COleControl::OnSize(nType, cx, cy);

	cx -= UI_MARGIN*2;
	cy -= UI_MARGIN*2;

	m_nWidth = cx;
	m_nHeight = cy;

	m_nMaxPixels = m_nWidth / 2;
	m_nSegmentLength = m_nWidth / 4;

	m_TimeCentsPerPixel = (double)TIMECENTS_RANGE / m_nSegmentLength;
	m_PixelsPerTenthPercent = m_nHeight / 1000.0;

	m_nDelayPixels = (long) (((m_lDelayTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	m_nAPixels = (long) (((m_lATime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	m_nHPixels = (long) (((m_lHTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	m_nDPixels = (long) (((m_lDTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);
	m_nRPixels = (long) (((m_lRTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);

	m_ptADRPts[START].y = m_nHeight - 1;

	m_ptADRPts[DELAY].x = m_nDelayPixels;
	m_ptADRPts[ATTACK].x = m_nAPixels;
	m_ptADRPts[HOLD].x = m_nHPixels;

	m_ptADRPts[DECAY].y = m_nHeight - (long)((m_lSLevel * m_PixelsPerTenthPercent) + 0.5);
	m_ptADRPts[DECAY].x = m_nHPixels + MUL_DIV(m_ptADRPts[DECAY].y, m_nDPixels, m_nHeight);

	m_ptADRPts[RELEASE].y = m_ptADRPts[DECAY].y;
	m_ptADRPts[RELEASE].x = (m_nWidth-1) - MUL_DIV(m_nHeight - m_ptADRPts[RELEASE].y, m_nRPixels, m_nHeight);

	m_ptADRPts[END].x = m_nWidth - 1;
	m_ptADRPts[END].y = m_nHeight - 1;

}



void CADSREnvelopeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
		/*int nOverlappedBoxes = 0;
	int nLastBox = 0;
	// If there's no overlap return
	for(int nBoxNo = 0; nBoxNo < 3; nBoxNo++)
	{
		if(m_rcADRPosBox[nBoxNo].PtInRect(point))
		{
			nLastBox = nBoxNo;
			nOverlappedBoxes++;
		}
	}
	if(nOverlappedBoxes <= 1)
	{
		m_nOnTop = nLastBox + 1;
		return;
	}

	// -1 is no overlap
	int nOnTop = -1;
	for(nBoxNo = 0; nBoxNo < 3; nBoxNo++)
	{
		if(m_rcADRPosBox[nBoxNo].PtInRect(point) && m_nOnTop != nBoxNo + 1)
		{
			nOnTop = nBoxNo + 1;
			break;
		}
	}
	m_nOnTop = nOnTop;*/
	

	COleControl::OnLButtonDblClk(nFlags, point);
}


long CADSREnvelopeCtrl::GetDelayTime() 
{
	return (long) TimeCents2Mils(m_lDelayTime);
	return 0;
}

void CADSREnvelopeCtrl::SetDelayTime(long nNewValue) 
{
	ASSERT(nNewValue >= 1 && nNewValue <= 40000);		
	
	m_lDelayTime = Mils2TimeCents(nNewValue);


	m_nDelayPixels = (long) (((m_lDelayTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);

	int nDeltaX = m_nDelayPixels - m_ptADRPts[DELAY].x; 
	m_ptADRPts[DELAY].x = m_nDelayPixels;


	// Attack drag point shifts with Delay
	m_ptADRPts[ATTACK].x += nDeltaX;
	m_nAPixels = m_ptADRPts[ATTACK].x;
	
	// The hold drag point shifts with attack
	m_ptADRPts[HOLD].x += nDeltaX;
	m_nHPixels = m_ptADRPts[HOLD].x;

	// The decay drag point shifts with hold
	m_ptADRPts[DECAY].x += nDeltaX;
	m_nDPixels = m_ptADRPts[DECAY].x;
	
	CClientDC ctrlDC(this);
	UpdateADSREnvelope(&ctrlDC);

	SetModifiedFlag();
	
	FireADSREDelayTimeChanged(nNewValue);
}

long CADSREnvelopeCtrl::GetHoldTime() 
{
	return (long) TimeCents2Mils(m_lHTime);
	return 0;
}

void CADSREnvelopeCtrl::SetHoldTime(long nNewValue) 
{
	ASSERT(nNewValue >= 1 && nNewValue <= 40000);		
	
	m_lHTime = Mils2TimeCents(nNewValue);

	m_nHPixels = m_nAPixels + (long)(((m_lHTime - TIMECENTS_MIN) / m_TimeCentsPerPixel) + 0.5);

	int nDeltaX = m_nHPixels - m_ptADRPts[HOLD].x; 
	m_ptADRPts[HOLD].x = m_nHPixels;

	// The decay drag point shifts with hold
	m_ptADRPts[DECAY].x += nDeltaX;
	m_nDPixels = m_ptADRPts[DECAY].x;

	CClientDC ctrlDC(this);
	UpdateADSREnvelope(&ctrlDC);

	SetModifiedFlag();
	
	FireADSREHoldTimeChanged(nNewValue);
}

void CADSREnvelopeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnLButtonUp(nFlags, point) ;
	
	COleControl::OnRButtonDown(nFlags, point);
}

void CADSREnvelopeCtrl::SetDLS1(BOOL bDLS1) 
{
	m_bDLS1 = bDLS1;

	// change selected handle if it was DLS2 and we're switching to DLS1
	if (bDLS1)
		if ((m_nOnTop == DELAY) || (m_nOnTop == HOLD))
			m_nOnTop = ATTACK;

	// update envelope control to display/hide DLS1 controls
	CClientDC ctrlDC(this);
	UpdateADSREnvelope(&ctrlDC);
}

/* returns whether the given handle (DELAY, ATTACK, etc) is visible given the DLS1 setting. */
bool CADSREnvelopeCtrl::ADRVisible(BYTE btLevel)
{
	ASSERT((btLevel > START) && (btLevel < END));
	return (!m_bDLS1 || ((btLevel != DELAY) && (btLevel != HOLD)));
}
