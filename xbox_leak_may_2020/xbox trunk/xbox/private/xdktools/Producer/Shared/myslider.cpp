/*
    MySlider.cpp

*/

#include "stdafx.h"
#include "myslider.h"
#include <math.h>
#include "DLSLoadSaveUtils.h"

MySlider::MySlider():
    m_lPosition(0), m_plData(0), m_lBase(0), m_lRange(100), m_lPageSize(10), m_lLineSize(1), m_nID(0), m_nDisplayID(0),
	m_bFocus(false), m_bDisplayEdited(false), m_pSpinButtonCtrl(NULL), m_nSpinnerID(0), m_lDefaultValue(0), m_uStrRes(0),
	m_bUndetermined(false)
{}

MySlider::~MySlider()
{
	if (m_pSpinButtonCtrl)
		delete m_pSpinButtonCtrl;
}

void MySlider::Update(CWnd *pWnd, bool bUpdateDisplay)
{
	// update slider
    CSliderCtrl* pControl = (CSliderCtrl*) pWnd->GetDlgItem(m_nID);
    if (pControl)
		pControl->SetPos(m_lPosition);

	// update spinner
	if (m_pSpinButtonCtrl)
		m_pSpinButtonCtrl->SetPos(m_lPosition);

	// update data and compute display string
    char text[100];
    long lTemp = 0;
    double flTemp = 0;

	if (m_bUndetermined)
		strcpy(text, "-----------");
	else switch(m_dwType)
		{
		case MYSLIDER_MIDIRANGE :
		{
			break;
		}

		case MYSLIDER_LFORANGE :
		{
			*m_plData = (m_lPosition + m_lBase) << 16;
			lTemp = PitchCents2Hertz(*m_plData);
			sprintf(text," %02d.%03d",lTemp / 1000, lTemp % 1000);
			break;
		}

		case MYSLIDER_FILTERABSPITCH :
		{
			lTemp = m_lPosition + m_lBase;
			*m_plData = lTemp << 16;
			sprintf(text," %02d", lTemp);
			break;
		}

		case MYSLIDER_TIMECENTS :
		case MYSLIDER_LFODELAY :
		{
			*m_plData = (m_lPosition + m_lBase) << 16;
			lTemp = TimeCents2Mils(*m_plData);
			sprintf(text," %02d.%03d",lTemp / 1000, lTemp % 1000);
			break;
		}

		case MYSLIDER_FILTERGAIN :
		case MYSLIDER_VOLUMECENTS :
		case MYSLIDER_VOLUME :
		{
			*m_plData = (m_lPosition + m_lBase) << 16;
			lTemp = m_lPosition + m_lBase;
			if (lTemp < 0)
			{
				lTemp = -lTemp;
				sprintf(text,"-%02d.%01d",lTemp / 10, lTemp % 10);
			}
			else 
			{
				sprintf(text," %02d.%01d",lTemp / 10, lTemp % 10);
			}
			break;
		}

		case MYSLIDER_FILTERPITCHCENTS :
		{
			*m_plData = (m_lPosition + m_lBase) << 16;
			lTemp = m_lPosition + m_lBase;
			if (lTemp < 0)
			{
				sprintf(text,"-%d",-lTemp);
			}
			else
			{
				sprintf(text," %d",lTemp);
			}
			break;
		}

		case MYSLIDER_PITCHCENTS :
		{
			*m_plData = (m_lPosition + m_lBase) << 16;
			lTemp = m_lPosition + m_lBase;
			if (lTemp < 0)
			{
				lTemp = -lTemp;
				sprintf(text,"-%02d.%02d",lTemp / 100, lTemp % 100);
			}
			else
			{
				sprintf(text," %02d.%02d",lTemp / 100, lTemp % 100);
			}
			break;
		}

		case MYSLIDER_PERCENT :
		{
			*m_plData = m_lPosition << 16;
			sprintf(text," %02d",m_lPosition / 10);
			break;
		}

		case MYSLIDER_PAN :
		{
			*m_plData = (m_lPosition + m_lBase) << 16;
			CString sPanString = GetPanString((float)(m_lPosition + m_lBase) / 10);
			sprintf(text, sPanString);
				
			break;
		}

		case MYSLIDER_NOTE :
		{
			*m_plData = m_lPosition;
			sprintf(text," %ld = ",m_lPosition);
			notetostring( (DWORD) m_lPosition, &text[strlen(text)]);
			break;
		}

		case MYSLIDER_GROUP :
		{
			*m_plData = m_lPosition;
			if (m_lPosition == 0)
			{
				strcpy(text," No Group");
			}
			else
			{
				sprintf(text," Group %ld",m_lPosition);
			}
			break;
		}

		case MYSLIDER_TIMESCALE :
		{
			*m_plData = (m_lPosition + m_lBase) << 16;
			lTemp = m_lPosition + m_lBase;
			flTemp = lTemp / 1200.0;
			flTemp = pow(2.0,flTemp);
			if (lTemp < 0)
			{
				sprintf(text," (%04.3f->1) * Time",flTemp);
			}
			else
			{
				sprintf(text," (1->%05.2f) * Time",flTemp);
			}
			break;
		}

		default :
		{
			strcpy(text," Error");
			break;
		}
    }

	// update display
	if (bUpdateDisplay)
		{
		CWnd* pDisplay = pWnd->GetDlgItem(m_nDisplayID);
		if (pDisplay)
			pDisplay->SetWindowText(text);
		}
}

/**
	Gets the String to be displayed for a Pan value. 
	(0 is extreme Left, 63 Mid and 127 extreme Right)
*/
CString MySlider::GetPanString(float fPan)
{
	CString sPanString;
	if(fPan < 0)
		sPanString.Format("L%2.1f%%%%", 0 - fPan); 
	else if(fPan > 0)
		sPanString.Format("R%2.1f%%%%", fPan);
	else if(fPan == 0)
		sPanString.Format("Mid");
	else
		sPanString.Format("---");

	return sPanString;
}


BOOL MySlider::Init(CWnd *pWnd, UINT nID, UINT nDisplayID, 
                    DWORD dwType, long * plData, UINT nSpinnerID)
{
    CSliderCtrl *pControl = (CSliderCtrl *)
        pWnd->GetDlgItem(nID);
    m_lPageSize = 100;
    m_lLineSize = 10;

    if (pControl != NULL)
    {
        m_plData = plData;
        m_nID = nID;
        m_nDisplayID = nDisplayID;
        m_dwType = dwType;
        switch (dwType)
        {
			case MYSLIDER_MIDIRANGE :
			{
				m_lBase = 0;
				m_lRange = 127;
				m_lPageSize = 12;
				m_lLineSize = 1;
				break;
			}

			case MYSLIDER_LFORANGE :
			{
				m_lBase = -7625;
				//m_lRange = 7973;
				m_lRange = 9174;
				break;
			}

			case MYSLIDER_TIMECENTS :
			{
				m_lBase = -11960;
				m_lRange = 18347;
				break;
			}

			case MYSLIDER_VOLUMECENTS :
			{
				m_lBase = 0;
				m_lRange = 120;
				m_lPageSize = 10;
				m_lLineSize = 1;
				break;
			}

			case MYSLIDER_FILTERGAIN :
			{
				m_lBase = 0;
				m_lRange = 225;
				m_lPageSize = 10;
				m_lLineSize = 1;
				break;
			}

			case MYSLIDER_PITCHCENTS :
			{
				m_lBase = -1200;
				m_lRange = 2400;
				m_lLineSize = 1;
				break;
			}

			case MYSLIDER_FILTERPITCHCENTS :
			{
				m_lBase = -12800;
				m_lRange = 25600;
				m_lLineSize = 1;
				break;
			}

			case MYSLIDER_FILTERABSPITCH:
			{
				m_lBase = 5535;
				m_lRange = 6386;
				m_lLineSize = 1;
				break;
			}

			case MYSLIDER_PERCENT :
			{
				m_lBase = 0;
				m_lRange = 1000;
				break;
			}

			case MYSLIDER_PAN :
			{
				m_lBase = -500;	// 
				m_lRange = 1000;	// Hard right
				m_lLineSize = 1;
				m_lPageSize = 10;
				break;
			}

			case MYSLIDER_VOLUME :
			{
				m_lBase = -960;
				m_lRange = 960;
				m_lPageSize = 10;
				m_lLineSize = 1;
				break;
			}

			case MYSLIDER_LFODELAY :
			{
				m_lBase = -11960;
				m_lRange = 15947;
				break;
			}

			case MYSLIDER_NOTE :
			{
				m_lBase = 0;
				m_lRange = 127;
				m_lLineSize = 1;
				m_lPageSize = 12;
				break;
			}

			case MYSLIDER_GROUP :
			{
				m_lBase = 0;
				m_lRange = 15;
				m_lLineSize = 1;
				m_lPageSize = 3;
				break;
			}

			case MYSLIDER_TIMESCALE :
			{
				m_lBase = -4800;
				m_lRange = 9600;
				m_lPageSize = 1200;
				m_lLineSize = 12;
				break;
			}
        }
        pControl->SetRange(0,m_lRange,FALSE);
        pControl->SetLineSize(m_lLineSize);
        pControl->SetPageSize(m_lPageSize);
        m_nID = nID;
        m_nDisplayID = nDisplayID;

		// create spinner control on the fly
		if (nSpinnerID != 0)
			{
		    CWnd* pDisplay = pWnd->GetDlgItem(nDisplayID);
		    if (pDisplay)
		    	{
				// create a spinner control attached to the display
				ASSERT(m_pSpinButtonCtrl == NULL); // I don't expect multiple Init calls, call SetValue instead
				m_pSpinButtonCtrl = new CSpinButtonCtrl();
				if (!m_pSpinButtonCtrl)
					return FALSE;
					
				RECT rc;
				m_pSpinButtonCtrl->Create(WS_VISIBLE | UDS_ARROWKEYS | UDS_ALIGNRIGHT, rc, pWnd, nSpinnerID);
				m_pSpinButtonCtrl->SetBuddy(pDisplay);
		    	m_nSpinnerID = nSpinnerID;
				}
			}

		// initialize spinner parameters too
		if (m_pSpinButtonCtrl)
			{
			m_pSpinButtonCtrl->SetRange(m_lBase, m_lRange);
			UDACCEL rguda[2];
			rguda[0].nSec = 0;
			rguda[0].nInc = m_lLineSize; // increment by m_lLineSize at first
			rguda[1].nSec = 2;
			rguda[1].nInc = m_lPageSize; // increment by m_lPageSize after two seconds
			m_pSpinButtonCtrl->SetAccel(2, rguda);
			}

        SetValue(pWnd, *plData);
        return (TRUE);
    }
    return (FALSE);
}

bool MySlider::Init(CWnd *pWnd, UINT nID, UINT nDisplayID, UINT nSpinnerID, DWORD dwType, long lDefaultValue, UINT uStrRes, long * plData)
{
	if (!Init(pWnd, nID, nDisplayID, dwType, plData, nSpinnerID))
		return false;
	m_lDefaultValue = lDefaultValue;
	m_uStrRes = uStrRes;
	return true;
}

void MySlider::SetValue(CWnd *pWnd, long lData)
{
    CSliderCtrl *pControl = (CSliderCtrl *)
        pWnd->GetDlgItem(m_nID);

    if (pControl != NULL)
    {
		m_bUndetermined = false;
    	m_lPosition = Value2Position(lData);
		if (m_lPosition > m_lRange)
			m_lPosition = m_lRange;
		else if (m_lPosition < 0)
			m_lPosition = 0;

		// turn off focus flag to prevent edit control from processing EN_CHANGE
		bool bFocusSav = m_bFocus;
		m_bFocus = false;
	
        SetPosition(pWnd, TB_THUMBPOSITION, (UINT)m_lPosition);

		// reset focus flag
		m_bFocus = bFocusSav;
    }
}

/* computes new position given slider change */
long MySlider::ComputeNewPositionFromSlider(UINT nSBCode, UINT nPos)
{
	long lPosition;
    switch (nSBCode)
    {
		case TB_THUMBPOSITION:
		case TB_THUMBTRACK:
		{
			lPosition = nPos;
			break;
		}

		case TB_LINEDOWN:
		{
			lPosition = m_lPosition+m_lLineSize;
			break;
		}

		case TB_PAGEDOWN:
		{
			lPosition = m_lPosition+m_lPageSize;
			break;
		}

		case TB_LINEUP:
		{
			lPosition = m_lPosition-m_lLineSize;
			break;
		}

		case TB_PAGEUP:
		{
			lPosition = m_lPosition-m_lPageSize;
			break;
		}

		case TB_TOP:
		{
			lPosition = 0;
			break;
		}

		case TB_BOTTOM:
		{
			lPosition = m_lRange;
			break;
		}

		default:
		{
			lPosition = m_lPosition;
		}
    }

	if (lPosition > m_lRange)
		lPosition = m_lRange;
	else if (lPosition < 0)
		lPosition = 0;
	return lPosition;
}

long MySlider::SetPosition(CWnd *pWnd, UINT nSBCode, UINT nPos, bool bUpdateDisplay)
{
	m_bUndetermined = false;
	m_lPosition = ComputeNewPositionFromSlider(nSBCode, nPos);
    Update(pWnd, bUpdateDisplay);
    return m_lPosition;
}

/* sets the control to the display value and returns new position. If bUpdateDisplay is true, recomputes display text using new position. */
long MySlider::SetPositionFromDisplay(CWnd *pWnd, bool bUpdateDisplay)
{
	// if no focus, refuse all updates
	if (!m_bFocus)
		return m_lPosition;

    CSliderCtrl* pControl = (CSliderCtrl*) pWnd->GetDlgItem(m_nID);
    CWnd* pDisplay = pWnd->GetDlgItem(m_nDisplayID);
    UINT nPos;

	ASSERT(pControl);
	ASSERT(pDisplay);

	// get string from display control
   	TCHAR szText[50];
   	ASSERT(pDisplay->GetWindowTextLength() < sizeof(szText)/sizeof(szText[0])); // control text won't fit in szText
	if (pDisplay->GetWindowText(szText, sizeof(szText)/sizeof(TCHAR)) == 0)
		{
		// an empty string is considered zero
		// FUTURE: use default slider value instead
		szText[0] = '0';
		szText[1] = 0;
		}

	// convert from text string to control position
	float f;
    switch(m_dwType)
		{
		case MYSLIDER_LFORANGE :
			sscanf(szText, "%f", &f);
			nPos = (Hertz2PitchCents((int)(f*1000)) >> 16) - m_lBase;
			break;

		case MYSLIDER_PITCHCENTS :
			sscanf(szText, "%f", &f);
			nPos = (int)(f*100)-m_lBase;
			break;

		case MYSLIDER_FILTERPITCHCENTS :
		case MYSLIDER_FILTERABSPITCH :
			{
			int i;
			sscanf(szText, "%d", &i);
			nPos = i-m_lBase;
			break;
			}

		case MYSLIDER_PAN :
			{
			// remove percent sign if present
			TCHAR *pch = szText;
			while (*pch)
				{
				if (*pch == '%')
					{
					*pch = 0;
					break;
					}
				pch++;
				}

			// skip leading spaces
			pch = szText;
			while (*pch && (*pch == ' '))
				pch++;
			
			// get signed float value (L is negative, R positive)
			switch (*pch)
				{
				case '-':
				case 'l':
				case 'L':
					// left
					sscanf(pch+1, "%f", &f);
					f = -f;
					break;

				case '+':
				case 'r':
				case 'R':
					// right
					pch++;
					// fall thru

				default:
					// right or mid (any string that can't be parsed is considered mid)
					sscanf(pch, "%f", &f);
				}

			// convert to slider position
			nPos = int(f*10)-m_lBase;
			break;
			}
		
		case MYSLIDER_FILTERGAIN :
		case MYSLIDER_VOLUMECENTS :
		case MYSLIDER_VOLUME :
			sscanf(szText, "%f", &f);
			nPos = (int)(f*10)-m_lBase;
			break;

		case MYSLIDER_TIMECENTS :
		case MYSLIDER_LFODELAY :
			sscanf(szText, "%f", &f);
			nPos = (Mils2TimeCents((DWORD)(f*1000)) >> 16) - m_lBase;
			break;

		default:
			ASSERT(FALSE); // slider type not implemented
			return m_lPosition;
		}
        
	SetPosition(pWnd, TB_THUMBPOSITION, nPos, bUpdateDisplay);
	return m_lPosition;
}

/* enables or disables display window */
void MySlider::EnableControl(CWnd *pWnd, bool bEnable)
{
	// control
    CSliderCtrl* pControl = (CSliderCtrl*) pWnd->GetDlgItem(m_nID);
	if (pControl)
		pControl->EnableWindow(bEnable);

	// display
	CWnd* pDisplay = pWnd->GetDlgItem(m_nDisplayID);
	if (pDisplay)
		pDisplay->EnableWindow(bEnable);

	// spinner
	if (m_pSpinButtonCtrl)
		m_pSpinButtonCtrl->EnableWindow(bEnable);
}

/* handles edit notifications to update position */
void MySlider::HandleDisplayMessage(CWnd *pWnd, WORD wMsg)
{
	switch (wMsg)
		{
		case EN_SETFOCUS:
			ASSERT(!m_bFocus);
			m_bFocus = true;
			m_bDisplayEdited = false;
			break;
				
		case EN_CHANGE:
			if (m_bFocus)
				{
				SetPositionFromDisplay(pWnd, false);
				m_bDisplayEdited = true;
				}
			break;
				
		case EN_KILLFOCUS:
			ASSERT(m_bFocus);
			if (m_bDisplayEdited)
				SetPositionFromDisplay(pWnd, true);
			m_bFocus = false;
			break;
		}
}

/* handles spinner notifications to update position */
void MySlider::HandleSpinnerMessage(CWnd *pWnd, NMHDR* pNMHDR)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// turn off focus flag to prevent edit control from processing EN_CHANGE
	bool bFocusSav = m_bFocus;
	m_bFocus = false;
	
	SetPosition(pWnd, TB_THUMBPOSITION, pNMUpDown->iPos+pNMUpDown->iDelta);

	// reset focus flag
	m_bFocus = bFocusSav;
	
	// update message with new position
	pNMUpDown->iPos = m_lPosition;
	pNMUpDown->iDelta = 0;
}

long MySlider::Value2Position(long lValue)
{
	switch (m_dwType)
        {
		case MYSLIDER_MIDIRANGE :
		case MYSLIDER_NOTE :
		case MYSLIDER_GROUP :
			return lValue;

		case MYSLIDER_LFORANGE :
		case MYSLIDER_TIMECENTS :
		case MYSLIDER_VOLUMECENTS :
		case MYSLIDER_FILTERGAIN :
		case MYSLIDER_PITCHCENTS :
		case MYSLIDER_FILTERPITCHCENTS :
		case MYSLIDER_VOLUME :
		case MYSLIDER_LFODELAY :
		case MYSLIDER_TIMESCALE :
			return ((lValue + 32768) >> 16) - m_lBase;

		case MYSLIDER_FILTERABSPITCH:
		case MYSLIDER_PAN :
			return (lValue >> 16) - m_lBase;

		case MYSLIDER_PERCENT :
			return lValue >> 16;

		default:
			ASSERT(FALSE); // slider type not handled
			return 0;
    }
}

/* returns the value corresponding to the position */
long MySlider::Position2Value(long lPosition)
{
	switch (m_dwType)
        {
		case MYSLIDER_MIDIRANGE :
		case MYSLIDER_NOTE :
		case MYSLIDER_GROUP :
			return lPosition;

		case MYSLIDER_LFORANGE :
		case MYSLIDER_TIMECENTS :
		case MYSLIDER_VOLUMECENTS :
		case MYSLIDER_FILTERGAIN :
		case MYSLIDER_PITCHCENTS :
		case MYSLIDER_FILTERPITCHCENTS :
		case MYSLIDER_VOLUME :
		case MYSLIDER_LFODELAY :
		case MYSLIDER_TIMESCALE :
			return ((lPosition + m_lBase) << 16) - 32768;

		case MYSLIDER_FILTERABSPITCH:
		case MYSLIDER_PAN :
			return (lPosition + m_lBase) << 16;

		case MYSLIDER_PERCENT :
			return lPosition << 16;

		default:
			ASSERT(FALSE); // slider type not handled
			return 0;
    }
}

void MySlider::SetUndetermined(CWnd *pWnd)
{
	m_bUndetermined = true;
	m_lPosition = Value2Position(m_lDefaultValue);
    Update(pWnd, true);
}


/*==============================================================*/
/*  CSliderCollection
/*==============================================================*/
CSliderCollection::CSliderCollection(WORD cMaxSliders):
	m_pWnd(NULL), m_cSliders(0), m_cMaxSliders(cMaxSliders),
	m_prgSliders(NULL), m_pmsFocus(NULL), m_bSaveUndo(true)
{
	ASSERT(cMaxSliders > 0); // must be positive
}

CSliderCollection::~CSliderCollection()
{
	Free();
}

void CSliderCollection::Init(CWnd *pWnd)
{
	ASSERT(m_pWnd == NULL); // Init must be called just once
	m_pWnd = pWnd;
}
	
/* creates a new slider/edit/spinner control. Returns pointer to spinner control if successful. */
MySlider *CSliderCollection::Insert(UINT nID, UINT nDisplayID, UINT nSpinnerID, DWORD dwType,
		long lDefaultValue, UINT uStrRes, long * plData)
{
	ASSERT(m_pWnd); // Init must have been called
	
	if (m_cSliders == m_cMaxSliders)
		{
		ASSERT(FALSE); // array is full
		return NULL;
		}

	// create the array if this is the first slider
	if (m_prgSliders == NULL)
		{
		ASSERT(m_cMaxSliders > 0); // must be positive
		m_prgSliders = new MySlider[m_cMaxSliders]; // this calls the MySlider constructor on each array element
		if (m_prgSliders == NULL)
			return NULL;
		}

#if _DEBUG
	// verify that a slider of this ID isn't in the array yet
	for (int iSlider = 0; iSlider < m_cSliders; iSlider++)
		if (m_prgSliders[iSlider].SliderID() == nID)
			{
			ASSERT(FALSE); // duplicate slider ID
			return NULL;
			}
	
#endif

	// initialize control
	MySlider *pSlider = &m_prgSliders[m_cSliders];
	if (!pSlider->Init(m_pWnd, nID, nDisplayID, nSpinnerID, dwType, lDefaultValue, uStrRes, plData))
		return NULL;
	
	m_cSliders++;
	return pSlider;
}

/* handles onCommand messages (for edit controls). If the message was processed, returns true,
	and fills *plResult with the message return value. */
bool CSliderCollection::OnCommand(WPARAM wParam, LPARAM lParam, LRESULT *plResult)
{
	lParam; // unreferenced
	ASSERT(m_pWnd); // Init must have been called
	ASSERT(plResult);

	int iSlider;
	WORD wMsg = HIWORD(wParam);
	UINT nDisplayID = LOWORD(wParam);

	switch (wMsg)
		{
		case EN_SETFOCUS:
			ASSERT(m_pmsFocus == NULL); // focus shouldn't have been set yet

			// a slider just got the focus, find out which one it is
			for (iSlider = 0; iSlider < m_cSliders; iSlider++)
				if (m_prgSliders[iSlider].DisplayID() == nDisplayID)
					break;
			if (iSlider == m_cSliders)
				return false; // message not handled

			m_pmsFocus = &m_prgSliders[iSlider];

			// FALL THRU

		case EN_CHANGE:
		case EN_KILLFOCUS:
			// ignore message if it's not for us
			if ((m_pmsFocus == NULL) || (m_pmsFocus->DisplayID() != nDisplayID))
				return false;

			if (m_bSaveUndo && (wMsg == EN_CHANGE) && m_pmsFocus->GetFocus())
				{
				if (!OnSliderUpdate(m_pmsFocus, dwmscupdfStart))
					return true; // handled

				m_bSaveUndo = false;
				}

			m_pmsFocus->HandleDisplayMessage(m_pWnd, wMsg);

			// kill focus
			if (wMsg == EN_KILLFOCUS)
				{
				OnSliderUpdate(m_pmsFocus, dwmscupdfEnd);
				m_bSaveUndo = true;
				m_pmsFocus = NULL;
				}

			*plResult = 0; // message processed
			return true;
		}

	return false; // message not handled
}

/* handles onNotify messages (for spinner controls). If the message was processed, returns true,
	and fills *plResult with the message return value. */
bool CSliderCollection::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *plResult)
{
	ASSERT(m_pWnd); // Init must have been called
	ASSERT(plResult);
	
	NMHDR *pNMHDR = (NMHDR *)lParam;
	if (pNMHDR->code == UDN_DELTAPOS)
		{
		// find slider if it's one of ours
		for (int iSlider = 0; iSlider < m_cSliders; iSlider++)
			if (m_prgSliders[iSlider].SpinnerID() == LOWORD(wParam))
				break;
		if (iSlider == m_cSliders)
			return false; // not handled

		if (m_bSaveUndo)
			{
			if (!OnSliderUpdate(&m_prgSliders[iSlider], dwmscupdfStart))
				return true; // handled

			m_bSaveUndo = false;
			}

		m_prgSliders[iSlider].HandleSpinnerMessage(m_pWnd, pNMHDR);
		*plResult = 0; // allow the spin control to update to the values in pNMHDR
		return true;
		}
	
	return false; // caller should call its handler
}

// free all contained controls (call on property page switch, when spinners need to be recreated)
void CSliderCollection::Free()
{
	delete[] m_prgSliders; // this calls the MySlider destructor on each element
	m_prgSliders = NULL;
	m_cSliders = NULL;
	m_bSaveUndo = true;
}

bool CSliderCollection::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL)
		return false; // not handled

	// find slider if it's one of ours
	UINT nID = (UINT)pScrollBar->GetDlgCtrlID();
	for (int iSlider = 0; iSlider < m_cSliders; iSlider++)
		if (m_prgSliders[iSlider].SliderID() == nID)
			break;
	if (iSlider == m_cSliders)
		return false; // not handled

	MySlider *pms = &m_prgSliders[iSlider];

	if (GetKeyState(VK_CONTROL) & 0x1000)
		{
		nPos = pms->Value2Position(pms->GetDefaultValue());
		if (nSBCode != TB_ENDTRACK)
			nSBCode = TB_THUMBPOSITION;
		}

	long lPosition = pms->ComputeNewPositionFromSlider(nSBCode, nPos);
	if (m_bSaveUndo && (lPosition == pms->GetPosition()))
		{
		pms->SetPosition(m_pWnd, nSBCode, nPos);
		return true; // handled, no change in position
		}

	if ( m_bSaveUndo )
		{
		if (!OnSliderUpdate(pms, dwmscupdfStart))
			return true; // handled, even though we're not setting position
					
		m_bSaveUndo = false;
		}

	pms->SetPosition(m_pWnd, nSBCode, nPos);

	if ( TB_ENDTRACK == nSBCode )
		{
		OnSliderUpdate(pms, dwmscupdfEnd);
		m_bSaveUndo = true;
		}

	return true;
}

/* called when parameter values start or end changing. Return true on success. This method
	is intended for override.
		dwmscupdfStart: update starts. If you return false, SliderCollection will assume that
						you reset the state, and will call Start again on next update.
		dwmscupdfEnd: update ends. You can do expensive operations there, like commit &
						download. Return value is ignored. */
bool CSliderCollection::OnSliderUpdate(MySlider *pms, DWORD dwmscupdf)
{
	pms; // unused
	dwmscupdf; // unused
	return true;
}


/*==============================================================*/
/*  Utilities
/*==============================================================*/
const char *convert[] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" } ;

void notetostring( DWORD note, char *string )
{
    int octave ;
    int dontoctave = note & 0x100 ;

    note  &= 0xFF ;
    octave = note / 12 ;
    note   = note % 12 ;

    strcpy( string, convert[note] ) ;

    if( dontoctave )
        return ;

    wsprintf( &string[lstrlen(string)], "%1d", octave ) ;
    
}   /* end of notetostring() */

short stringtonote(char *string)
{
    char letter;
    unsigned short index;
    short note = 0;
	int octave;
    static char convert[] = { 9,11,0,2,4,5,7 };
    index = 0;
    for (;string[index] == ' ';index++);
    letter = string[index++];
    if ((letter >= 'a') && (letter <= 'g')) 
	{
		note = convert[letter - 'a'];
	}
    else if ((letter >= 'A') && (letter <= 'G'))
	{
		note = convert[letter - 'A'];
	}

    letter = string[index];
    if (letter == '#') 
	{
        index++;
        note++;
    }
    else if (letter == 'b' || letter == 'B') 
	{
        index++;
        note--;
    }
    for(;string[index] == ' ';index++);
    octave = atoi(&string[index]);
    octave = octave * 12;
    octave += note;

	// the note has to be within 0 - 127. 
	if(octave < 0)
		octave = 0;
	else if(octave > 127)
		octave = 127;

    return (short)octave;
}


