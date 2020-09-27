// VolDialog.cpp : implementation file
//

#include "stdafx.h"
#include "VolDialog.h"
#include "InstrumentFVEditor.h"
#include "DLSStatic.h"
#include "Articulation.h"
#include "DlsDefsPlus.h"
#include "UndoMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVolDialog dialog


CVolDialog::CVolDialog(CArticulation* pArticulation) :
CDialog(), CSliderCollection(5),
m_pArticParams(pArticulation->GetArticParams()),
m_pCollection(NULL),
m_bInitingDialog(false)
{
	m_pArticulation = pArticulation;
#ifdef _DEBUG	
	pArticulation->ValidateParent();
#endif

	CInstrument* pInstrument = pArticulation->GetInstrument();
	if(pInstrument)
	{
		// Articulation is owned by an instrument			
		m_pCollection = pInstrument->m_pCollection;
		m_pInstrument = pInstrument;
	}
	else
	{
		// Articulation is owned by a region
		CRegion* pRegion = pArticulation->GetRegion();
		ASSERT(pRegion);
		pInstrument = pRegion->GetInstrument();
		ASSERT(pInstrument);
		m_pCollection = pInstrument->m_pCollection;
		m_pInstrument = pInstrument;
	}
	
	//{{AFX_DATA_INIT(CVolDialog)
	m_flAttack = 0.0f;
	m_flDecay = 0.0f;
	m_flRelease = 0.0f;
	m_flSustain = 0.0f;
	m_flDelay = 0.0f;
	m_flHold = 0.0f;
	//}}AFX_DATA_INIT
	m_bfTouched = 0;
	m_bTouchedByProgram = false;

	CSliderCollection::Init(this);
}


void CVolDialog::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVolDialog)
	DDX_Control(pDX, IDC_VEG_ADSRENVELOPECTRL, m_VEGASDREnvelope);
	DDX_Text(pDX, IDC_VEG_DATTACK, m_flAttack);
	DDV_MinMaxFloat(pDX, m_flAttack, 1.e-003f, 40.f);
	DDX_Text(pDX, IDC_VEG_DDECAY, m_flDecay);
	DDV_MinMaxFloat(pDX, m_flDecay, 1.e-003f, 40.f);
	DDX_Text(pDX, IDC_VEG_DRELEASE, m_flRelease);
	DDV_MinMaxFloat(pDX, m_flRelease, 1.e-003f, 40.f);
	DDX_Text(pDX, IDC_VEG_DSUSTAIN, m_flSustain);
	DDV_MinMaxFloat(pDX, m_flSustain, 0.f, 100.f);
	DDX_Text(pDX, IDC_VEG_DDELAY, m_flDelay);
	DDV_MinMaxFloat(pDX, m_flDelay, 1.e-003f, 40.f);
	DDX_Text(pDX, IDC_VEG_DHOLD, m_flHold);
	DDV_MinMaxFloat(pDX, m_flHold, 1.e-003f, 40.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVolDialog, CDialog)
	//{{AFX_MSG_MAP(CVolDialog)
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_VEG_DATTACK, OnKillfocusVegDattack)
	ON_EN_KILLFOCUS(IDC_VEG_DDECAY, OnKillfocusVegDdecay)
	ON_EN_KILLFOCUS(IDC_VEG_DRELEASE, OnKillfocusVegDrelease)
	ON_EN_KILLFOCUS(IDC_VEG_DSUSTAIN, OnKillfocusVegDsustain)
	ON_MESSAGE(DM_VALIDATE,OnValidate)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_EN_KILLFOCUS(IDC_VEG_DDELAY, OnKillfocusVegDelay)
	ON_EN_KILLFOCUS(IDC_VEG_DHOLD, OnKillfocusVegHold)
	//}}AFX_MSG_MAP
	// This is to get notified of changes to the edit controls
	ON_CONTROL_RANGE(EN_CHANGE, IDC_VEG_DATTACK, IDC_VEG_DHOLD, OnEditChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolDialog message handlers

LRESULT CVolDialog::OnValidate(UINT wParam,LONG lParam)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch ( wParam )
	{
		case IDC_VEG_DDELAY :
			ValidateDelayTime();
			break;
		case IDC_VEG_DATTACK :
			ValidateAttackTime();
			break;
		case IDC_VEG_DHOLD :
			ValidateHoldTime();
			break;
		case IDC_VEG_DDECAY :
			ValidateDecayTime();
			break;
		case IDC_VEG_DSUSTAIN :
			ValidateSustainLevel();
			break;
		case IDC_VEG_DRELEASE :
			ValidateReleaseTime();
			break;
		default:
			break;
	}
	return 0;
}

//OnEditChange : See VolDialog.h for details about this. (In private member
//				 section of CVolDialog)
void CVolDialog::OnEditChange( UINT nEditControl )
{
	if ( !m_bTouchedByProgram )
		m_bfTouched |= 0x01 << (nEditControl - FIRST_CONTROL);
}

void CVolDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pArticParams);
	if(m_pArticParams == NULL)
	{
		return;
	}

	ASSERT(m_pInstrument);
	if(m_pInstrument == NULL)
	{
		return;
	}

	ASSERT(m_pCollection);
	if(m_pCollection == NULL)
	{
		return;
	}

	CSliderCollection::OnHScroll(nSBCode, nPos, pScrollBar);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BEGIN_EVENTSINK_MAP(CVolDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CVolDialog)
	ON_EVENT(CVolDialog, IDC_VEG_ADSRENVELOPECTRL, 4 /* ADSRESustainLevelChanged */, OnADSRESustainLevelChangedVegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CVolDialog, IDC_VEG_ADSRENVELOPECTRL, 3 /* ADSREReleaseTimeChanged */, OnADSREReleaseTimeChangedVegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CVolDialog, IDC_VEG_ADSRENVELOPECTRL, 2 /* ADSREAttackTimeChanged */, OnADSREAttackTimeChangedVegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CVolDialog, IDC_VEG_ADSRENVELOPECTRL, 1 /* ADSREDecayTimeChanged */, OnADSREDecayTimeChangedVegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CVolDialog, IDC_VEG_ADSRENVELOPECTRL, 5 /* RealDecayChanged */, OnRealDecayChangedVegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CVolDialog, IDC_VEG_ADSRENVELOPECTRL, 6 /* RealReleaseChanged */, OnRealReleaseChangedVegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CVolDialog, IDC_VEG_ADSRENVELOPECTRL, 7 /* ADSRMouseMoveStart */, OnADSRMouseMoveStart, VTS_NONE)
	ON_EVENT(CVolDialog, IDC_VEG_ADSRENVELOPECTRL, 9 /* ADSREDelayTimeChanged */, OnADSREDelayTimeChangedVegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CVolDialog, IDC_VEG_ADSRENVELOPECTRL, 10 /* ADSREHoldTimeChanged */, OnADSREHoldTimeChangedVegAdsrenvelopectrl, VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

////////////////////////////////////////////////////////////////////////////////
// ADSR control ATTACK Time changed
void CVolDialog::OnADSREAttackTimeChangedVegAdsrenvelopectrl(long NewAttackTime) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	char text[BUFFER_64];
    sprintf(text," %02d.%03d", NewAttackTime / 1000, NewAttackTime % 1000);
	CWnd* pDisplay = GetDlgItem(IDC_VEG_DATTACK);
	
	m_bTouchedByProgram = true;
	pDisplay->SetWindowText(text);
	m_bTouchedByProgram = false;

	if( !m_bInitingDialog )
	{
		if ( !( m_bfTouched & fVEG_DATTACK ) )
		{
			m_pArticParams->m_VolEG.m_tcAttack = Mils2TimeCents(NewAttackTime);
		}

		m_pCollection->SetDirtyFlag();
		m_pInstrument->UpdateInstrument();
	}
}

////////////////////////////////////////////////////////////////////////////////
// ADSR control DECAY Time changed
void CVolDialog::OnADSREDecayTimeChangedVegAdsrenvelopectrl(long NewDecayTime) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	char text[BUFFER_64];
    sprintf(text," %02d.%03d", NewDecayTime / 1000, NewDecayTime % 1000);
	CWnd * pDisplay = GetDlgItem(IDC_VEG_DDECAY);
	
	m_bTouchedByProgram = true;
	pDisplay->SetWindowText(text);
	m_bTouchedByProgram = false;

	if(!m_bInitingDialog)
	{
		if ( !( m_bfTouched & fVEG_DDECAY ) )
			m_pArticParams->m_VolEG.m_tcDecay = Mils2TimeCents(NewDecayTime);
		m_pCollection->SetDirtyFlag();
		m_pInstrument->UpdateInstrument();
	}
}

////////////////////////////////////////////////////////////////////////////////
// ADSR control SUSTAIN Level changed
void CVolDialog::OnADSRESustainLevelChangedVegAdsrenvelopectrl(long NewSustainLevel) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	char text[BUFFER_64];
    sprintf(text," %02d.%d",NewSustainLevel / 10, NewSustainLevel % 10);
	CWnd * pDisplay = GetDlgItem(IDC_VEG_DSUSTAIN);
	
	m_bTouchedByProgram = true;
	pDisplay->SetWindowText(text);
	m_bTouchedByProgram = false;

	if(!m_bInitingDialog)
	{
		m_pArticParams->m_VolEG.m_ptSustain = (PERCENT) NewSustainLevel << 16;
		m_pCollection->SetDirtyFlag();
		m_pInstrument->UpdateInstrument();
	}
}

////////////////////////////////////////////////////////////////////////////////
// ADSR control RELEASE Time changed
void CVolDialog::OnADSREReleaseTimeChangedVegAdsrenvelopectrl(long NewReleaseTime) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	char text[BUFFER_64];
    sprintf(text," %02d.%03d", NewReleaseTime / 1000, NewReleaseTime % 1000);
	CWnd * pDisplay = GetDlgItem(IDC_VEG_DRELEASE);
	
	m_bTouchedByProgram = true;
	pDisplay->SetWindowText(text);
	m_bTouchedByProgram = false;

	if(!m_bInitingDialog)
	{
		if ( !( m_bfTouched & fVEG_DRELEASE ) )
			m_pArticParams->m_VolEG.m_tcRelease = Mils2TimeCents(NewReleaseTime);
		m_pCollection->SetDirtyFlag();
		m_pInstrument->UpdateInstrument();
	}
}

void CVolDialog::OnRealDecayChangedVegAdsrenvelopectrl(long lNewRealDecay) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWnd*	pWnd;
	char text[BUFFER_64];

    sprintf(text," %02d.%03d", lNewRealDecay / 1000, lNewRealDecay % 1000);
	pWnd = GetDlgItem(IDC_VEG_RDECAY);
	pWnd->SetWindowText(text);
}

void CVolDialog::OnRealReleaseChangedVegAdsrenvelopectrl(long lNewRealRelease) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWnd*	pWnd;
	char text[BUFFER_64];

    sprintf(text," %02d.%03d", lNewRealRelease / 1000, lNewRealRelease % 1000);
	pWnd = GetDlgItem(IDC_VEG_RRELEASE);
	pWnd->SetWindowText(text);	
}


HBRUSH CVolDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	COLORREF crTextColor = 0;
	int nStaticText;

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	nStaticText = pWnd->GetDlgCtrlID();
	switch (nStaticText)
	{
		case IDC_VEG_DELAY_STATIC:
			crTextColor = DELAY_COLOR;
			break;

		case IDC_VEG_ATTACK_STATIC:
			crTextColor = ATTACK_COLOR;
			break;

		case IDC_VEG_HOLD_STATIC:
			crTextColor = HOLD_COLOR;
			break;

		case IDC_VEG_DECAY_STATIC:
			crTextColor = DECAY_COLOR;
			break;

		case IDC_VEG_SUSTAIN_STATIC:
			crTextColor = SUSTAIN_COLOR;
			break;

		case IDC_VEG_RELEASE_STATIC:
			crTextColor = RELEASE_COLOR;
			break;
		default:
			return hbr;
	}
	pDC->SetTextColor(crTextColor);
	return hbr;
}


BOOL CVolDialog::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_bInitingDialog = true;

	CDialog::OnInitDialog();

	// Set all the bitmaps
	CStatic* pStatic = (CStatic*)GetDlgItem(IDC_ATTACK_STATIC);
	if(pStatic)
		pStatic->SetBitmap(CInstrumentFVEditor::m_bmpAttack);
	
	pStatic = (CStatic*)GetDlgItem(IDC_DECAY_STATIC);
	if(pStatic)
		pStatic->SetBitmap(CInstrumentFVEditor::m_bmpDecay);
	
	pStatic = (CStatic*)GetDlgItem(IDC_DECAY2_STATIC);
	if(pStatic)
		pStatic->SetBitmap(CInstrumentFVEditor::m_bmpDecay2);

	pStatic = (CStatic*)GetDlgItem(IDC_SUSTAIN_STATIC);
	if(pStatic)
		pStatic->SetBitmap(CInstrumentFVEditor::m_bmpSustain);

	pStatic = (CStatic*)GetDlgItem(IDC_RELEASE_STATIC);
	if(pStatic)
		pStatic->SetBitmap(CInstrumentFVEditor::m_bmpRelease);

	pStatic = (CStatic*)GetDlgItem(IDC_RELEASE2_STATIC);
	if(pStatic)
		pStatic->SetBitmap(CInstrumentFVEditor::m_bmpRelease2);
		
	m_pmsVEGShutdownTime = Insert(
		IDC_VEG_SHUTDOWNTIME,
		IDC_VEG_DSHUTDOWNTIME,
		0,
		MYSLIDER_TIMESCALE,
		0,
		IDS_UNDO_SHUTDOWNTIME_EDIT,
		&(m_pArticParams->m_VolEG.m_tcShutdownTime));

	m_pmsMSCDefaultPan = Insert(
		IDC_MSC_PAN,
		IDC_MSC_DPAN,
		IDC_MSC_PAN_SPIN,
		MYSLIDER_PAN,
		0,
		IDS_PAN_UNDO_TEXT,
		&(m_pArticParams->m_Misc.m_ptDefaultPan));

	m_pmsVEGVel2Attack = Insert(
		IDC_VEG_VEL2ATTACK,
		IDC_VEG_DVEL2ATTACK,
		0,
		MYSLIDER_TIMESCALE,
		0,
		IDS_VEL2ATTACK_UNDO_TEXT,
		&(m_pArticParams->m_VolEG.m_tcVel2Attack));
    
	m_pmsVEGKey2Hold = Insert(
		IDC_VEG_KEY2HOLD,
		IDC_VEG_DKEY2HOLD,
		0,
		MYSLIDER_TIMESCALE,
		0,
		IDS_UNDO_KEYTOHOLD_EDIT,
		&(m_pArticParams->m_VolEG.m_tcKey2Hold));

	m_pmsVEGKey2Decay = Insert(
		IDC_VEG_KEY2DECAY,
		IDC_VEG_DKEY2DECAY,
		0,
		MYSLIDER_TIMESCALE,
		0,
		IDS_KEY2DECAY_UNDO_TEXT,
		&(m_pArticParams->m_VolEG.m_tcKey2Decay));

	// Set Sustain first; this is a kludge we need to fix ADSR control J4
	// UpdateADSREnvelope fires an Sustain changed event before it the sustain is set 
	// this messes up the proper sustain value
	m_VEGASDREnvelope.SetSustainLevel(m_pArticParams->m_VolEG.m_ptSustain >> 16);
	
	DWORD dw;
	
	dw = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcRelease);
	m_VEGASDREnvelope.SetReleaseTime(dw ? (dw <= MAXADSRTIME ? dw : MAXADSRTIME) : 1);

	dw = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcDelay);
	m_VEGASDREnvelope.SetDelayTime(dw ? (dw <= MAXADSRTIME ? dw : MAXADSRTIME) : 1);
	
	dw = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcAttack);
	m_VEGASDREnvelope.SetAttackTime(dw ? (dw <= MAXADSRTIME ? dw : MAXADSRTIME) : 1);

	dw = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcHold);
	m_VEGASDREnvelope.SetHoldTime(dw ? (dw <= MAXADSRTIME ? dw : MAXADSRTIME) : 1);
	
	dw = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcDecay);
	m_VEGASDREnvelope.SetDecayTime(dw ? (dw <= MAXADSRTIME ? dw : MAXADSRTIME) : 1);

	BOOL bDLS1 = m_pArticulation->IsDLS1();
	EnableDLS2Controls(!bDLS1);

	m_bInitingDialog = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CVolDialog::OnKillfocusVegDattack() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	PostMessage(DM_VALIDATE,IDC_VEG_DATTACK);	
	//Default();
}

void CVolDialog::OnKillfocusVegDdecay() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	PostMessage(DM_VALIDATE,IDC_VEG_DDECAY);	
}

void CVolDialog::OnKillfocusVegDsustain() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	PostMessage(DM_VALIDATE,IDC_VEG_DSUSTAIN);	
}

void CVolDialog::OnKillfocusVegDrelease() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	PostMessage(DM_VALIDATE,IDC_VEG_DRELEASE);	
}


void CVolDialog::ValidateAttackTime()
{
	BOOL bUpdate;
	if ( m_bfTouched & fVEG_DATTACK )
	{
		float flOldAttack = m_flAttack;

		bUpdate = UpdateData(TRUE);	
		if(bUpdate)
		{		
			if(!m_bInitingDialog)
			{
				if(FAILED(SaveUndoState(IDS_ATTACK_UNDO_TEXT)))
				{
					m_flAttack = flOldAttack;
					m_bfTouched &= ~fVEG_DATTACK;
					UpdateData(FALSE);
					return;
				}
				// Add 0.5 to the float value so that on conversion to int,
				// we get the correct value. float of 10.0 is stored as 9.99999..
				// m_flAttack is stored in seconds.
				DWORD flAttack = (DWORD)((m_flAttack*(float)1000.0) + (float)0.5);

				// Find out the error creeping up in conversion and adjust the actual value for that
				TCENT tcAttack = Mils2TimeCents(flAttack);
				DWORD dwAttack =  TimeCents2Mils(tcAttack);
				DWORD dwErrorMargin = dwAttack - flAttack;
				flAttack = flAttack - dwErrorMargin;

				m_pArticParams->m_VolEG.m_tcAttack = Mils2TimeCents(flAttack);
				// TimeCents is not millisecond accurate at higher values so we
				// need to convert back to milliseconds to display exactly how
				// many milliseconds the TimeCents value represents.
				flAttack = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcAttack);

				m_pCollection->SetDirtyFlag();
				if (m_VEGASDREnvelope)
				{
					m_VEGASDREnvelope.SetAttackTime(flAttack); //it's stored internally as milliseconds
				}
				m_pInstrument->UpdateInstrument();
			}
		} 
		else
		{
			long flAttack = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcAttack);
			char text[BUFFER_64];
			sprintf(text," %02d.%03d", flAttack/1000, flAttack%1000);

			CEdit* pDisplay = (CEdit*)GetDlgItem(IDC_VEG_DATTACK);
			if(pDisplay)
			{
				pDisplay->SetWindowText(text);
				SelectEditAndBringWindowToTop(pDisplay);
			}
		}
		m_bfTouched &= ~fVEG_DATTACK;
	}
}

void CVolDialog::ValidateDecayTime()
{
	BOOL bUpdate;
	if ( m_bfTouched & fVEG_DDECAY )
	{
		float flOldDecay = m_flDecay;
		bUpdate = UpdateData(TRUE);	
		if(bUpdate)
		{		
			if(!m_bInitingDialog)
			{
				if(FAILED(SaveUndoState(IDS_DECAY_UNDO_TEXT)))
				{
					m_bfTouched &= ~fVEG_DDECAY;
					m_flDecay = flOldDecay;
					UpdateData(FALSE);
					return;
				}
				// Add 0.5 to the float value so that on conversion to int,
				// we get the correct value. float of 10.0 is stored as 9.99999..
				// m_fldecay is stored in seconds.
				DWORD flDecay = (DWORD)((m_flDecay*(float)1000) + (float)0.5);

				// Find out the error creeping up in conversion and adjust the actual value for that
				TCENT tcDecay = Mils2TimeCents(flDecay);
				DWORD dwDecay =  TimeCents2Mils(tcDecay);
				DWORD dwErrorMargin = dwDecay - flDecay;
				flDecay = flDecay - dwErrorMargin;

				m_pArticParams->m_VolEG.m_tcDecay = Mils2TimeCents((long)flDecay);
				// TimeCents is not millisecond accurate at higher values so we
				// need to convert back to milliseconds to display exactly how
				// many milliseconds the TimeCents value represents.
				flDecay = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcDecay);

				m_pCollection->SetDirtyFlag();
				if (m_VEGASDREnvelope)
				{
					m_VEGASDREnvelope.SetDecayTime((long)flDecay);
				}
				m_pInstrument->UpdateInstrument();
			}
		} 
		else
		{
			char text[BUFFER_64];
			long flDecay = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcDecay);
			sprintf(text," %02d.%03d", flDecay / 1000, flDecay % 1000);
			CEdit* pDisplay = (CEdit*)GetDlgItem(IDC_VEG_DDECAY);
			if(pDisplay)
			{
				pDisplay->SetWindowText(text);
				SelectEditAndBringWindowToTop(pDisplay);
			}
		}
		m_bfTouched &= ~fVEG_DDECAY;
	}
}

void CVolDialog::ValidateSustainLevel()
{
	BOOL bUpdate;
	if ( m_bfTouched & fVEG_DSUSTAIN )
	{
		float flOldSustain = m_flSustain;

		bUpdate = UpdateData(TRUE);	
		if(bUpdate)
		{		
			if(!m_bInitingDialog)
			{
				if(FAILED(SaveUndoState(IDS_SUSTAIN_UNDO_TEXT)))
				{
					m_bfTouched &= ~fVEG_DSUSTAIN;
					m_flSustain = flOldSustain;
					UpdateData(FALSE);
					return;
				}
				// Add 0.5 to the float value so that on conversion to int,
				// we get the correct value. float of 10.0 is stored as 9.99999..
				// m_flSustain goes from 0 - 1000 (0 - 100%)
				DWORD flSustain = (DWORD)((m_flSustain * (float)10)+ (float)0.5);

				m_pArticParams->m_VolEG.m_ptSustain = (PERCENT)flSustain << 16;

				m_pCollection->SetDirtyFlag();
				if (m_VEGASDREnvelope)
				{
					m_VEGASDREnvelope.SetSustainLevel(flSustain);
				}
				m_pInstrument->UpdateInstrument();
			}
		}
		else
		{
			char text[BUFFER_64];
			long flSustain = m_pArticParams->m_VolEG.m_ptSustain >> 16;
			sprintf(text," %02d.%d",flSustain / 10, flSustain % 10);
			CEdit* pDisplay = (CEdit*)GetDlgItem(IDC_VEG_DSUSTAIN);
			
			if(pDisplay)
			{
				pDisplay->SetWindowText(text);
				SelectEditAndBringWindowToTop(pDisplay);
			}
		}
		m_bfTouched &= ~fVEG_DSUSTAIN;
	}
}

void CVolDialog::ValidateReleaseTime()
{
	BOOL bUpdate;
	if ( m_bfTouched & fVEG_DRELEASE )
	{
		float flOldRelease = m_flRelease;

		bUpdate = UpdateData(TRUE);	
		if(bUpdate)
		{
			if(!m_bInitingDialog)
			{
				if(FAILED(SaveUndoState(IDS_RELEASE_UNDO_TEXT)))
				{
					m_bfTouched &= ~fVEG_DRELEASE;
					m_flRelease = flOldRelease;
					UpdateData(FALSE);
					return;
				}
				// Add 0.5 to the float value so that on conversion to int,
				// we get the correct value. float of 10.0 is stored as 9.99999..
				// m_flRelease is stored in seconds.
				DWORD flRelease = (DWORD)((m_flRelease*(float)1000) + (float)0.5);

				// Find out the error creeping up in conversion and adjust the actual value for that
				TCENT tcRelease = Mils2TimeCents(flRelease);
				DWORD dwRelease =  TimeCents2Mils(tcRelease);
				DWORD dwErrorMargin = dwRelease - flRelease;
				flRelease = flRelease - dwErrorMargin;

				m_pArticParams->m_VolEG.m_tcRelease = Mils2TimeCents((long)flRelease);
				// TimeCents is not millisecond accurate at higher values so we
				// need to convert back to milliseconds to display exactly how
				// many milliseconds the TimeCents value represents.
				flRelease = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcRelease);

				m_pCollection->SetDirtyFlag();
				if (m_VEGASDREnvelope)
				{
					m_VEGASDREnvelope.SetReleaseTime((long)flRelease);
				}
				m_pInstrument->UpdateInstrument();
			}
		}
		else
		{
			char text[BUFFER_64];
			long flRelease = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcRelease);
			sprintf(text," %02d.%03d", flRelease / 1000, flRelease % 1000);
			CEdit* pDisplay = (CEdit*)GetDlgItem(IDC_VEG_DRELEASE);

			if(pDisplay)
			{
				pDisplay->SetWindowText(text);
				SelectEditAndBringWindowToTop(pDisplay);
			}
		}

		m_bfTouched &= ~fVEG_DRELEASE;

	}
}



void CVolDialog::OnADSRMouseMoveStart() 
{
	if(FAILED(SaveUndoState(IDS_ADSR_UNDO_TEXT)))
		return;
}

HRESULT CVolDialog::SaveUndoState(UINT uStringRes)
{
	CInstrumentFVEditor* pParent = (CInstrumentFVEditor*) GetParent();
	ASSERT(pParent);

	if(pParent && pParent->m_pInstrument)
	{
		if(FAILED(pParent->m_pInstrument->SaveStateForUndo(uStringRes)))
			return E_FAIL;
	}
	else
		return E_FAIL;

	return S_OK;
}

void CVolDialog::UpdateArticulation(CArticulation * pNewArt)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pArticParams = pNewArt->GetArticParams();

	m_pArticulation = pNewArt;
#ifdef _DEBUG	
	pNewArt->ValidateParent();
#endif

	CInstrument* pInstrument = pNewArt->GetInstrument();
	if(pInstrument)
	{
		// Articulation is owned by an instrument			
		m_pCollection = pInstrument->m_pCollection;
		m_pInstrument = pInstrument;
	}
	else
	{
		// Articulation is owned by a region
		CRegion* pRegion = pNewArt->GetRegion();
		ASSERT(pRegion);
		pInstrument = pRegion->GetInstrument();
		ASSERT(pInstrument);
		m_pCollection = pInstrument->m_pCollection;
		m_pInstrument = pInstrument;
	}
	
	m_bfTouched = 0;
	m_bTouchedByProgram = false;
	m_bInitingDialog = true;

	// Point sliders at new articulation values
	m_pmsVEGShutdownTime->SetDataValueToEdit(
		&(m_pArticParams->m_VolEG.m_tcShutdownTime));
	m_pmsMSCDefaultPan->SetDataValueToEdit(
		&(m_pArticParams->m_Misc.m_ptDefaultPan));
	m_pmsVEGVel2Attack->SetDataValueToEdit(
		&(m_pArticParams->m_VolEG.m_tcVel2Attack));
	m_pmsVEGKey2Hold->SetDataValueToEdit(
		&(m_pArticParams->m_VolEG.m_tcKey2Hold));
	m_pmsVEGKey2Decay->SetDataValueToEdit(
		&(m_pArticParams->m_VolEG.m_tcKey2Decay));

	//UpdateData(FALSE);
	m_pmsMSCDefaultPan->SetValue(this, m_pArticParams->m_Misc.m_ptDefaultPan);
	m_pmsVEGVel2Attack->SetValue(this, m_pArticParams->m_VolEG.m_tcVel2Attack);
	m_pmsVEGKey2Decay->SetValue(this, m_pArticParams->m_VolEG.m_tcKey2Decay);
	m_pmsVEGKey2Hold->SetValue(this, m_pArticParams->m_VolEG.m_tcKey2Hold);
	m_pmsVEGShutdownTime->SetValue(this, m_pArticParams->m_VolEG.m_tcShutdownTime);
	
	// Set Sustain first; this is a kludge we need to fix ADSR control J4
	// UpdateADSREnvelope fires an Sustain changed event before it the sustain is set 
	// this messes up the proper sustain value
	m_VEGASDREnvelope.SetSustainLevel(m_pArticParams->m_VolEG.m_ptSustain >> 16);
	
	DWORD dw;

	dw = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcRelease);
	m_VEGASDREnvelope.SetReleaseTime(dw ? (dw <= MAXADSRTIME ? dw : MAXADSRTIME) : 1);

	dw = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcDelay);
	m_VEGASDREnvelope.SetDelayTime(dw ? (dw <= MAXADSRTIME ? dw : MAXADSRTIME) : 1);
	
	dw = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcAttack);
	m_VEGASDREnvelope.SetAttackTime(dw ? (dw <= MAXADSRTIME ? dw : MAXADSRTIME) : 1);

	dw = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcHold);
	m_VEGASDREnvelope.SetHoldTime(dw ? (dw <= MAXADSRTIME ? dw : MAXADSRTIME) : 1);
	
	dw = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcDecay);
	m_VEGASDREnvelope.SetDecayTime(dw ? (dw <= MAXADSRTIME ? dw : MAXADSRTIME) : 1);

	BOOL bDLS1 = m_pArticulation->IsDLS1();
	EnableDLS2Controls(!bDLS1);

	m_bInitingDialog = false;
}

void CVolDialog::SelectEditAndBringWindowToTop(CEdit* pEdit)
{
	ASSERT(pEdit);
	if(pEdit == NULL)
		return;

	HWND hwndEditor = NULL;
	m_pInstrument->GetEditorWindow(&hwndEditor);
	if(hwndEditor)
		::BringWindowToTop(hwndEditor);

	pEdit->SetFocus();
	pEdit->SetSel(0, -1);
}

void CVolDialog::CollectStatics()
{
	// Remove all previous statics....should there be any?
	while(!m_lstStatics.IsEmpty())
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.RemoveHead();
		ASSERT(pStatic);
		if(pStatic)
		{
			delete pStatic;
		}
	}

	CWnd* pChild = GetWindow(GW_CHILD);
	while(pChild)
	{
		char szClassName[MAX_PATH];
		::GetClassName(pChild->m_hWnd, szClassName, MAX_PATH);
		
		// Add it to the list if it's a "Static"
		if(strcmp(szClassName,"Static") == 0)
		{
			CDLSStatic* pDLSStatic = NULL;
			if(SUCCEEDED(CDLSStatic::CreateControl(this, pChild, &pDLSStatic)))
				m_lstStatics.AddTail(pDLSStatic);
		}

		pChild = pChild->GetNextWindow();
	}

	// Destroy the actual static controls now
	POSITION position = m_lstStatics.GetHeadPosition();
	while(position)
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.GetNext(position);
		ASSERT(pStatic);
		CWnd* pWnd = GetDlgItem(pStatic->GetID());
		if(pWnd)
		{
			pWnd->DestroyWindow();
		}
	}
}

CDLSStatic* CVolDialog::GetStaticControl(UINT nID)
{
	POSITION position = m_lstStatics.GetHeadPosition();
	while(position)
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.GetNext(position);
		ASSERT(pStatic);
		if(pStatic->GetID() == nID)
			return pStatic;
	}

	return NULL;
}

void CVolDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// Remove all previous statics....should there be any?
	while(!m_lstStatics.IsEmpty())
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.RemoveHead();
		ASSERT(pStatic);
		if(pStatic)
		{
			delete pStatic;
		}
	}
}


void CVolDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	POSITION position = m_lstStatics.GetHeadPosition();
	while(position)
	{
		CDLSStatic* pStatic = (CDLSStatic*) m_lstStatics.GetNext(position);
		ASSERT(pStatic);
		pStatic->OnDraw(&dc);
	}
}

void CVolDialog::OnKillfocusVegDelay() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	PostMessage(DM_VALIDATE,IDC_VEG_DDELAY);	
}

void CVolDialog::OnKillfocusVegHold() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	PostMessage(DM_VALIDATE,IDC_VEG_DHOLD);	
}


void CVolDialog::ValidateDelayTime()
{
	BOOL bUpdate;
	if ( m_bfTouched & fVEG_DDELAY )
	{
		float flOldDelay = m_flDelay;

		bUpdate = UpdateData(TRUE);	
		if(bUpdate)
		{		
			if(!m_bInitingDialog)
			{
				if(FAILED(SaveUndoState(IDS_UNDO_DELAY_EDIT)))
				{
					m_flDelay = flOldDelay;
					m_bfTouched &= ~fVEG_DDELAY;
					UpdateData(FALSE);
					return;
				}
				// Add 0.5 to the float value so that on conversion to int,
				// we get the correct value. float of 10.0 is stored as 9.99999..
				// m_flDelay is stored in seconds.
				DWORD flDelay = (DWORD)((m_flDelay*(float)1000.0) + (float)0.5);

				// Find out the error creeping up in conversion and adjust the actual value for that
				TCENT tcDelay = Mils2TimeCents(flDelay);
				DWORD dwDelay =  TimeCents2Mils(tcDelay);
				DWORD dwErrorMargin = dwDelay - flDelay;
				flDelay = flDelay - dwErrorMargin;

				m_pArticParams->m_VolEG.m_tcDelay = Mils2TimeCents(flDelay);

				// TimeCents is not millisecond accurate at higher values so we
				// need to convert back to milliseconds to display exactly how
				// many milliseconds the TimeCents value represents.

				flDelay = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcDelay);

				m_pCollection->SetDirtyFlag();
				if (m_VEGASDREnvelope)
				{
					m_VEGASDREnvelope.SetDelayTime(flDelay); //it's stored internally as milliseconds
				}
				m_pInstrument->UpdateInstrument();
			}
		} 
		else
		{
			long flDelay = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcDelay);
			char text[BUFFER_64];
			sprintf(text," %02d.%03d", flDelay/1000, flDelay%1000);

			CEdit* pDisplay = (CEdit*)GetDlgItem(IDC_VEG_DDELAY);
			if(pDisplay)
			{
				pDisplay->SetWindowText(text);
				SelectEditAndBringWindowToTop(pDisplay);
			}
		}
		m_bfTouched &= ~fVEG_DDELAY;
	}
}




void CVolDialog::ValidateHoldTime()
{
	BOOL bUpdate;
	if ( m_bfTouched & fVEG_DHOLD )
	{
		float flOldHold = m_flHold;

		bUpdate = UpdateData(TRUE);	
		if(bUpdate)
		{		
			if(!m_bInitingDialog)
			{
				if(FAILED(SaveUndoState(IDS_UNDO_HOLD_EDIT)))
				{
					m_flHold = flOldHold;
					m_bfTouched &= ~fVEG_DHOLD;
					UpdateData(FALSE);
					return;
				}
				// Add 0.5 to the float value so that on conversion to int,
				// we get the correct value. float of 10.0 is stored as 9.99999..
				// m_flHold is stored in seconds.
				DWORD flHold = (DWORD)((m_flHold*(float)1000.0) + (float)0.5);

				// Find out the error creeping up in conversion and adjust the actual value for that
				TCENT tcHold = Mils2TimeCents(flHold);
				DWORD dwHold =  TimeCents2Mils(tcHold);
				DWORD dwErrorMargin = dwHold - flHold;
				flHold = flHold - dwErrorMargin;

				m_pArticParams->m_VolEG.m_tcHold = Mils2TimeCents(flHold);

				// TimeCents is not millisecond accurate at higher values so we
				// need to convert back to milliseconds to display exactly how
				// many milliseconds the TimeCents value represents.

				flHold = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcHold);

				m_pCollection->SetDirtyFlag();
				if (m_VEGASDREnvelope)
				{
					m_VEGASDREnvelope.SetHoldTime(flHold); //it's stored internally as milliseconds
				}
				m_pInstrument->UpdateInstrument();
			}
		} 
		else
		{
			long flHold = TimeCents2Mils(m_pArticParams->m_VolEG.m_tcHold);
			char text[BUFFER_64];
			sprintf(text," %02d.%03d", flHold/1000, flHold%1000);

			CEdit* pDisplay = (CEdit*)GetDlgItem(IDC_VEG_DHOLD);
			if(pDisplay)
			{
				pDisplay->SetWindowText(text);
				SelectEditAndBringWindowToTop(pDisplay);
			}
		}
		m_bfTouched &= ~fVEG_DHOLD;
	}
}

void CVolDialog::OnADSREDelayTimeChangedVegAdsrenvelopectrl(long lNewDelayTime) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	char text[BUFFER_64];
    sprintf(text," %02d.%03d", lNewDelayTime / 1000, lNewDelayTime % 1000);
	CWnd* pDisplay = GetDlgItem(IDC_VEG_DDELAY);
	
	m_bTouchedByProgram = true;
	pDisplay->SetWindowText(text);
	m_bTouchedByProgram = false;

	if( !m_bInitingDialog )
	{
		if ( !( m_bfTouched & fVEG_DDELAY ) )
			m_pArticParams->m_VolEG.m_tcDelay = Mils2TimeCents(lNewDelayTime);
		m_pCollection->SetDirtyFlag();
		m_pInstrument->UpdateInstrument();
	}
}

void CVolDialog::OnADSREHoldTimeChangedVegAdsrenvelopectrl(long lNewHoldTime) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	char text[BUFFER_64];
    sprintf(text," %02d.%03d", lNewHoldTime / 1000, lNewHoldTime % 1000);
	CWnd* pDisplay = GetDlgItem(IDC_VEG_DHOLD);
	
	m_bTouchedByProgram = true;
	pDisplay->SetWindowText(text);
	m_bTouchedByProgram = false;

	if( !m_bInitingDialog )
	{
		if ( !( m_bfTouched & fVEG_DHOLD ) )
			m_pArticParams->m_VolEG.m_tcHold = Mils2TimeCents(lNewHoldTime);
		m_pCollection->SetDirtyFlag();
		m_pInstrument->UpdateInstrument();
	}
}

void CVolDialog::EnableDLS2Controls(BOOL bEnable)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Delay and Hold edits
	CWnd* pWnd = GetDlgItem(IDC_VEG_DDELAY);
	if(pWnd)
		pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_VEG_DELAY_STATIC);
	if(pWnd)
		pWnd->EnableWindow(bEnable);

	pWnd = GetDlgItem(IDC_VEG_DHOLD);
	if(pWnd)
		pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_VEG_HOLD_STATIC);
	if(pWnd)
		pWnd->EnableWindow(bEnable);

	m_pmsVEGShutdownTime->EnableControl(this, bEnable != 0);
	m_pmsVEGKey2Hold->EnableControl(this, bEnable != 0);
	
	// Envelope
	m_VEGASDREnvelope.SetDLS1(!bEnable);
}

BOOL CVolDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult;
	if (CSliderCollection::OnCommand(wParam, lParam, &lResult))
		return lResult;

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CVolDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LRESULT Result;
	if (CSliderCollection::OnNotify(wParam, lParam, &Result))
		return Result;
	
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CVolDialog::UpdateInstrument()
{
	m_pInstrument->UpdateInstrument();
	m_pCollection->SetDirtyFlag();
}

bool CVolDialog::OnSliderUpdate(MySlider *pms, DWORD dwmscupdf)
{
	switch (dwmscupdf)
		{
		case dwmscupdfStart:
			if (SUCCEEDED(SaveUndoState(pms->GetUndoStringID())))
				return true;
			UpdateArticulation(m_pArticulation);
			return false;

		case dwmscupdfEnd:
			UpdateInstrument();
			return true;

		default:
			ASSERT(FALSE);
			return false;
		}
}

