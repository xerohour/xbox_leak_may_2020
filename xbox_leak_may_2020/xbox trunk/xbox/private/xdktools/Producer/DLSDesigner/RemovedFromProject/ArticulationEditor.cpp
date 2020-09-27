// ArticulationEditor.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DLSDesignerDLL.h"
#include "DLSDesigner.h"
#include "jazz.h"
#include "ArticulationCtl.h"
#include "ArticulationEditor.h"
#include "Articulation.h"
#include "Instrument.h"

#include "region.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArticulationEditor

IMPLEMENT_DYNCREATE(CArticulationEditor, CFormView)

CArticulationEditor::CArticulationEditor(CArticulationCtrl* parent)
	: CFormView(CArticulationEditor::IDD)
{
	m_parent = parent;
	
	CArticulation* pArticulation = m_parent->GetArticulation();
	
	ASSERT(pArticulation != NULL);

#ifdef _DEBUG	
	pArticulation->ValidateParent();
#endif

	m_ArticParams = pArticulation->GetArticParams();
	
	if(pArticulation->m_pInstrument)
	{
		// Articulation is owned by an instrument			
		m_pCollection = (pArticulation->m_pInstrument)->m_pCollection;
	}
	else
	{
		// Articulation is owned by a region
		m_pCollection = ((pArticulation->m_pRegion)->m_pInstrument)->m_pCollection;
	}

	//{{AFX_DATA_INIT(CArticulationEditor)
	//}}AFX_DATA_INIT
}

CArticulationEditor::~CArticulationEditor()
{
}

void CArticulationEditor::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArticulationEditor)
	DDX_Control(pDX, IDC_VEG_ADSRENVELOPECTRL, m_VEGASDREnvelope);
	DDX_Control(pDX, IDC_PEG_ADSRENVELOPECTRL, m_PEGADSREnvelope);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArticulationEditor, CFormView)
	//{{AFX_MSG_MAP(CArticulationEditor)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArticulationEditor diagnostics

#ifdef _DEBUG
void CArticulationEditor::AssertValid() const
{
	CFormView::AssertValid();
}

void CArticulationEditor::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArticulationEditor message handlers

BOOL CArticulationEditor::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BEGIN_EVENTSINK_MAP(CArticulationEditor, CFormView)
    //{{AFX_EVENTSINK_MAP(CArticulationEditor)
	ON_EVENT(CArticulationEditor, IDC_PEG_ADSRENVELOPECTRL, 2 /* ADSREAttackTimeChanged */, OnADSREAttackTimeChangedPegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CArticulationEditor, IDC_PEG_ADSRENVELOPECTRL, 3 /* ADSREReleaseTimeChanged */, OnADSREReleaseTimeChangedPegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CArticulationEditor, IDC_PEG_ADSRENVELOPECTRL, 1 /* ADSREDecayTimeChanged */, OnADSREDecayTimeChangedPegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CArticulationEditor, IDC_PEG_ADSRENVELOPECTRL, 4 /* ADSRESustainLevelChanged */, OnADSRESustainLevelChangedPegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CArticulationEditor, IDC_VEG_ADSRENVELOPECTRL, 4 /* ADSRESustainLevelChanged */, OnADSRESustainLevelChangedVegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CArticulationEditor, IDC_VEG_ADSRENVELOPECTRL, 3 /* ADSREReleaseTimeChanged */, OnADSREReleaseTimeChangedVegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CArticulationEditor, IDC_VEG_ADSRENVELOPECTRL, 2 /* ADSREAttackTimeChanged */, OnADSREAttackTimeChangedVegAdsrenvelopectrl, VTS_I4)
	ON_EVENT(CArticulationEditor, IDC_VEG_ADSRENVELOPECTRL, 1 /* ADSREDecayTimeChanged */, OnADSREDecayTimeChangedVegAdsrenvelopectrl, VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CArticulationEditor::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	m_LFOParams = m_ArticParams.m_LFO;
	m_VEGParams = m_ArticParams.m_VolEG;
    m_PEGParams = m_ArticParams.m_PitchEG;
    m_MSCParams = m_ArticParams.m_Misc;

// LFO
	m_msLFOFrequency.Init(this, 
					      IDC_LFO_FREQUENCY, 
					      IDC_LFO_DFREQUENCY, 
                          MYSLIDER_LFORANGE,
					      &m_LFOParams.m_pcFrequency);

	m_msLFODelay.Init(this, 
				      IDC_LFO_DELAY, 
				      IDC_LFO_DDELAY, 
                      MYSLIDER_LFODELAY,
				      &m_LFOParams.m_tcDelay);
    
	m_msLFOVolumeScale.Init(this, 
						    IDC_LFO_VOLUME, 
						    IDC_LFO_DVOLUME, 
						    MYSLIDER_VOLUMECENTS,
						    &m_LFOParams.m_gcVolumeScale);

	m_msLFOPitchScale.Init(this, 
						   IDC_LFO_PITCH, 
						   IDC_LFO_DPITCH, 
						   MYSLIDER_PITCHCENTS,
						   &m_LFOParams.m_pcPitchScale);

	m_msLFOMWToVolume.Init(this, 
						   IDC_LFO_MW2VOLUME, 
						   IDC_LFO_DMW2VOLUME, 
						   MYSLIDER_VOLUMECENTS,
						   &m_LFOParams.m_gcMWToVolume);

	m_msLFOMWToPitch.Init(this, 
					      IDC_LFO_MW2PITCH, 
					      IDC_LFO_DMW2PITCH, 
					      MYSLIDER_PITCHCENTS,
					      &m_LFOParams.m_pcMWToPitch);

// Misc
	m_msMSCDefaultPan.Init(this, 
						   IDC_MSC_PAN, 
						   IDC_MSC_DPAN, 
						   MYSLIDER_PAN,
						   &m_MSCParams.m_ptDefaultPan);

// VEG
	m_msVEGVel2Attack.Init(this, 
						   IDC_VEG_VEL2ATTACK, 
						   IDC_VEG_DVEL2ATTACK, 
						   MYSLIDER_TIMESCALE,
						   &m_VEGParams.m_tcVel2Attack);
    
	m_msVEGKey2Decay.Init(this, 
					      IDC_VEG_KEY2DECAY, 
					      IDC_VEG_DKEY2DECAY, 
					      MYSLIDER_TIMESCALE,
					      &m_VEGParams.m_tcKey2Decay);
// PEG
	m_msPEGVel2Attack.Init(this, 
						   IDC_PEG_VEL2ATTACK, 
						   IDC_PEG_DVEL2ATTACK, 
						   MYSLIDER_TIMESCALE,
						   &m_PEGParams.m_tcVel2Attack);
    
	m_msPEGKey2Decay.Init(this, 
						  IDC_PEG_KEY2DECAY, 
						  IDC_PEG_DKEY2DECAY, 
						  MYSLIDER_TIMESCALE,
					      &m_PEGParams.m_tcKey2Decay);
    
	m_msPEGRange.Init(this, 
					  IDC_PEG_RANGE, 
					  IDC_PEG_DRANGE, 
                      MYSLIDER_PITCHCENTS,
				      &m_PEGParams.m_pcRange);
	DWORD dw;

	dw = TimeCents2Mils(m_ArticParams.m_PitchEG.m_tcRelease);
	m_PEGADSREnvelope.SetReleaseTime(dw ? (dw <= MAXDSRTIME ? dw : MAXDSRTIME) : 1);
	dw = TimeCents2Mils(m_ArticParams.m_PitchEG.m_tcAttack);
	m_PEGADSREnvelope.SetAttachTime(dw ? (dw <= MAXDSRTIME ? dw : MAXDSRTIME) : 1);
	dw = TimeCents2Mils(m_ArticParams.m_PitchEG.m_tcDecay);
	m_PEGADSREnvelope.SetDecayTime(dw ? (dw <= MAXDSRTIME ? dw : MAXDSRTIME) : 1);

	int temp = (m_ArticParams.m_PitchEG.m_ptSustain) >> 16;
	m_PEGADSREnvelope.SetSustainLevel(temp);
	
	dw = TimeCents2Mils(m_ArticParams.m_VolEG.m_tcRelease);
	m_VEGASDREnvelope.SetReleaseTime(dw ? (dw <= MAXDSRTIME ? dw : MAXDSRTIME) : 1);
	dw = TimeCents2Mils(m_ArticParams.m_VolEG.m_tcAttack);
	m_VEGASDREnvelope.SetAttachTime(dw ? (dw <= MAXDSRTIME ? dw : MAXDSRTIME) : 1);
	dw = TimeCents2Mils(m_ArticParams.m_VolEG.m_tcDecay);
	m_VEGASDREnvelope.SetDecayTime(dw ? (dw <= MAXDSRTIME ? dw : MAXDSRTIME) : 1);

	temp = (m_ArticParams.m_VolEG.m_ptSustain) >> 16;	
	m_VEGASDREnvelope.SetSustainLevel(temp);
}

void CArticulationEditor::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
//	LFO
    MySlider *pSlider = NULL;
	if (pScrollBar != NULL)
	{
        int nID = pScrollBar->GetDlgCtrlID();
        switch (nID)
        {
        case IDC_LFO_FREQUENCY :
            pSlider = &m_msLFOFrequency;
            break;
        case IDC_LFO_DELAY :
            pSlider = &m_msLFODelay;
            break;
        case IDC_LFO_VOLUME :
            pSlider = &m_msLFOVolumeScale;
            break;
        case IDC_LFO_PITCH :
            pSlider = &m_msLFOPitchScale;
            break;
        case IDC_LFO_MW2VOLUME :
            pSlider = &m_msLFOMWToVolume;
            break;
        case IDC_LFO_MW2PITCH :
            pSlider = &m_msLFOMWToPitch;
            break;
// MISC
        case IDC_MSC_PAN :
            pSlider = &m_msMSCDefaultPan;
            break;
// VEG
        case IDC_VEG_VEL2ATTACK :
            pSlider = &m_msVEGVel2Attack;
            break;
        case IDC_VEG_KEY2DECAY :
            pSlider = &m_msVEGKey2Decay;
            break;
// PEG
        case IDC_PEG_VEL2ATTACK :
            pSlider = &m_msPEGVel2Attack;
            break;
        case IDC_PEG_KEY2DECAY :
            pSlider = &m_msPEGKey2Decay;
            break;
        case IDC_PEG_RANGE :
            pSlider = &m_msPEGRange;
            break;
        default:
            pSlider = NULL;
            break;
        }

		m_ArticParams.m_LFO = m_LFOParams;
		m_ArticParams.m_VolEG = m_VEGParams;
		m_ArticParams.m_PitchEG = m_PEGParams;
		m_ArticParams.m_Misc = m_MSCParams;

        if (pSlider != NULL)
        {
            pSlider->SetPosition(this,nSBCode,nPos);
        }

#ifdef DLS_UPDATE_SYNTH
	m_pCollection->UpdateSynth(m_parent->m_pArticulation, UPDATE_ARTICULATION);
#endif

    }


	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CArticulationEditor::OnADSREAttackTimeChangedPegAdsrenvelopectrl(long NewAttackTime) 
{
	char text[64];
	sprintf(text, "%02d", NewAttackTime);
	CWnd* pDisplay = GetDlgItem(IDC_PEG_DATTACK);
	pDisplay->SetWindowText(text);

	m_PEGParams.m_tcAttack = Mils2TimeCents(NewAttackTime);
	m_ArticParams.m_PitchEG.m_tcAttack = m_PEGParams.m_tcAttack;

#ifdef DLS_UPDATE_SYNTH
			m_pCollection->UpdateSynth(m_parent->m_pArticulation, UPDATE_ARTICULATION);
#endif

}

void CArticulationEditor::OnADSREReleaseTimeChangedPegAdsrenvelopectrl(long NewReleaseTime) 
{
	char text[64];
	sprintf(text, "%02d", NewReleaseTime);
	CWnd * pDisplay = GetDlgItem(IDC_PEG_DRELEASE);
	pDisplay->SetWindowText(text);

	m_PEGParams.m_tcRelease = Mils2TimeCents(NewReleaseTime);
	m_ArticParams.m_PitchEG.m_tcRelease = m_PEGParams.m_tcRelease;

#ifdef DLS_UPDATE_SYNTH
			m_pCollection->UpdateSynth(m_parent->m_pArticulation, UPDATE_ARTICULATION);
#endif

}

void CArticulationEditor::OnADSREDecayTimeChangedPegAdsrenvelopectrl(long NewDecayTime) 
{
	char text[64];
	sprintf(text, "%02d", NewDecayTime);
	CWnd * pDisplay = GetDlgItem(IDC_PEG_DDECAY);
	pDisplay->SetWindowText(text);

	m_PEGParams.m_tcDecay = Mils2TimeCents(NewDecayTime);
	m_ArticParams.m_PitchEG.m_tcDecay = m_PEGParams.m_tcDecay;

#ifdef DLS_UPDATE_SYNTH
			m_pCollection->UpdateSynth(m_parent->m_pArticulation, UPDATE_ARTICULATION);
#endif

}

void CArticulationEditor::OnADSRESustainLevelChangedPegAdsrenvelopectrl(long NewSustainLevel) 
{
	char text[64];
	sprintf(text, "%02d", NewSustainLevel);
	CWnd * pDisplay = GetDlgItem(IDC_PEG_DSUSTAIN);
	pDisplay->SetWindowText(text);

//	m_PEGParams.m_ptSustain = Mils2TimeCents(NewSustainLevel);
//	m_ArticParams.m_PitchEG.m_ptSustain = m_PEGParams.m_ptSustain;

#ifdef DLS_UPDATE_SYNTH
			m_pCollection->UpdateSynth(m_parent->m_pArticulation, UPDATE_ARTICULATION);
#endif
}

void CArticulationEditor::OnADSRESustainLevelChangedVegAdsrenvelopectrl(long NewSustainLevel) 
{
	char text[64];
	sprintf(text, "%02d", NewSustainLevel);
	CWnd * pDisplay = GetDlgItem(IDC_VEG_DSUSTAIN);
	pDisplay->SetWindowText(text);

//	m_VEGParams.m_ptSustain = Mils2TimeCents(NewSustainLevel);
//	m_ArticParams.m_VolEG.m_ptSustain = m_VEGParams.m_ptSustain;

#ifdef DLS_UPDATE_SYNTH
	m_pCollection->UpdateSynth(m_parent->m_pArticulation, UPDATE_ARTICULATION);
#endif
}

void CArticulationEditor::OnADSREReleaseTimeChangedVegAdsrenvelopectrl(long NewReleaseTime) 
{
	char text[64];
	sprintf(text, "%02d", NewReleaseTime);
	CWnd * pDisplay = GetDlgItem(IDC_VEG_DRELEASE);
	pDisplay->SetWindowText(text);

	m_VEGParams.m_tcRelease = Mils2TimeCents(NewReleaseTime);
	m_ArticParams.m_VolEG.m_tcRelease = m_VEGParams.m_tcRelease;

#ifdef DLS_UPDATE_SYNTH
	m_pCollection->UpdateSynth(m_parent->m_pArticulation, UPDATE_ARTICULATION);
#endif
}

void CArticulationEditor::OnADSREAttackTimeChangedVegAdsrenvelopectrl(long NewAttackTime) 
{
	char text[64];
	sprintf(text, "%02d", NewAttackTime);
	CWnd* pDisplay = GetDlgItem(IDC_VEG_DATTACK);
	pDisplay->SetWindowText(text);

	m_VEGParams.m_tcAttack = Mils2TimeCents(NewAttackTime);
	m_ArticParams.m_VolEG.m_tcAttack = m_VEGParams.m_tcAttack;

#ifdef DLS_UPDATE_SYNTH
	m_pCollection->UpdateSynth(m_parent->m_pArticulation, UPDATE_ARTICULATION);
#endif
}

void CArticulationEditor::OnADSREDecayTimeChangedVegAdsrenvelopectrl(long NewDecayTime) 
{
	char text[64];
	sprintf(text, "%02d", NewDecayTime);
	CWnd * pDisplay = GetDlgItem(IDC_VEG_DDECAY);
	pDisplay->SetWindowText(text);

	m_VEGParams.m_tcDecay = Mils2TimeCents(NewDecayTime);
	m_ArticParams.m_VolEG.m_tcDecay = m_VEGParams.m_tcDecay;

#ifdef DLS_UPDATE_SYNTH
	m_pCollection->UpdateSynth(m_parent->m_pArticulation, UPDATE_ARTICULATION);
#endif
}
