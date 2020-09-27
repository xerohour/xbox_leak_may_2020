// RegionPropPg.cpp : implementation file
//

#include "stdafx.h"
#include "Region.h"
#include "RegionPropPg.h"
#include "RegionPropPgMgr.h"
#include "DlsDefsPlus.h"
#include "Collection.h"
#include "Instrument.h"
#include "Wave.h"
#include "MonoWave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern short stringtonote(char * psz);
extern BOOL isValidNoteString(char * psz);

 
UINT CRegionPropPg::m_arrChannelPositions[MAX_WAVELINK_CHANNELS] = 
{IDS_REGION_CHANNEL0, IDS_REGION_CHANNEL1,  IDS_REGION_CHANNEL2, IDS_REGION_CHANNEL3, IDS_REGION_CHANNEL4, 
IDS_REGION_CHANNEL5, IDS_REGION_CHANNEL6, IDS_REGION_CHANNEL7, IDS_REGION_CHANNEL8, IDS_REGION_CHANNEL9, 
IDS_REGION_CHANNEL10, IDS_REGION_CHANNEL11, IDS_REGION_CHANNEL12, IDS_REGION_CHANNEL13, IDS_REGION_CHANNEL14,
IDS_REGION_CHANNEL15, IDS_REGION_CHANNEL16, IDS_REGION_CHANNEL17};

/////////////////////////////////////////////////////////////////////////////
// CRegionPropPg property page

IMPLEMENT_DYNCREATE(CRegionPropPg, CPropertyPage)

CRegionPropPg::CRegionPropPg() : CPropertyPage(CRegionPropPg::IDD), CSliderCollection(2),
m_pRegion(NULL),
m_fInOnInitialUpdate(true),
m_fActivateFineTune(false),
m_fActivateAttenuation(false),
m_fActivateRootNote(false),
m_fNeedToDetach(FALSE),
m_pContextMenu(NULL),
m_pmsAttenuation(NULL),
m_pmsTune(NULL)
{
	//{{AFX_DATA_INIT(CRegionPropPg)
	//}}AFX_DATA_INIT

    m_pContextMenu = new CMenu();
	m_pContextMenu->LoadMenu(IDM_REGION_PROP_PAGE_RMENU);
	CSliderCollection::Init(this);
}

CRegionPropPg::~CRegionPropPg()
{
	delete m_pContextMenu;
}

/////////////////////////////////////////////////////////////////////////////
// CRegionPropPg::EnableControls

void CRegionPropPg::EnableControls(BOOL fEnable) 
{
	CWnd * pCtrl = GetDlgItem(IDC_PHASE_GROUP_CHECK);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_OWS_ACTIVE_ATTENUATION);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_OWS_ACTIVE_ROOT_NOTE);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_OWS_ACTIVE_FINE_TUNE);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}

	EnablePhaseGroupControls(fEnable != 0);

	EnableFineTune(m_fActivateFineTune && fEnable);
	
	EnableAttenuation(m_fActivateAttenuation && fEnable);

	EnableRootNote(m_fActivateRootNote && fEnable);
}

void CRegionPropPg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegionPropPg)
	DDX_Control(pDX, IDC_MULTICHANNEL_CHECK, m_MultichannelCheck);
	DDX_Control(pDX, IDC_CHANNEL_COMBO, m_ChannelCombo);
	DDX_Control(pDX, IDC_PHASE_GROUP_SPIN, m_PhaseGroupSpin);
	DDX_Control(pDX, IDC_PHASE_GROUP_EDIT, m_PhaseGroupEdit);
	DDX_Control(pDX, IDC_PHASE_GROUP_CHECK, m_PhaseGroupCheck);
	DDX_Control(pDX, IDC_MASTER_CHECK, m_MasterCheck);
	DDX_Control(pDX, IDC_OWS_DUNITYNOTE, m_RootNoteEdit);
	DDX_Control(pDX, IDC_OWS_ROOT_NODE_SPIN, m_RootNoteSpin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegionPropPg, CPropertyPage)
	//{{AFX_MSG_MAP(CRegionPropPg)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_OWS_ACTIVE_ATTENUATION, OnOwsActiveAttenuation)
	ON_BN_CLICKED(IDC_OWS_ACTIVE_FINE_TUNE, OnOwsActiveFineTune)
	ON_BN_CLICKED(IDC_OWS_ACTIVE_ROOT_NOTE, OnOwsActiveRootNote)
	ON_WM_HSCROLL()
	ON_WM_CONTEXTMENU()
	ON_EN_KILLFOCUS(IDC_OWS_DUNITYNOTE, OnKillfocusOwsDunitynote)
	ON_NOTIFY(UDN_DELTAPOS, IDC_OWS_ROOT_NODE_SPIN, OnDeltaposOwsRootNodeSpin)
	ON_BN_CLICKED(IDC_PHASE_GROUP_CHECK, OnPhaseGroupCheck)
	ON_EN_KILLFOCUS(IDC_PHASE_GROUP_EDIT, OnKillfocusPhaseGroupEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PHASE_GROUP_SPIN, OnDeltaposPhaseGroupSpin)
	ON_BN_CLICKED(IDC_MASTER_CHECK, OnMasterCheck)
	ON_CBN_SELCHANGE(IDC_CHANNEL_COMBO, OnSelchangeChannelCombo)
	ON_BN_CLICKED(IDC_MULTICHANNEL_CHECK, OnMultichannelCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionPropPg message handlers

BOOL CRegionPropPg::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	BOOL	bRetval;

	m_fInOnInitialUpdate = true;

	CRegionPropPgMgr::dwLastSelPage = REGION_PROP_PAGE;
	bRetval = CPropertyPage::OnSetActive();
	
	if(m_pRegion == NULL)
	{
		EnableControls(FALSE);
		return bRetval;
	}

	InitPhaseGroupControls();

    if (m_pRegion->m_pWave)
    {
		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();

        // If the wave root note changed in the instrument dialog and we were not
        // initiliazed we need to check to see if rootnote changed.
        if (!m_pRegion->m_bOverrideWaveRootNote) // see if we need to turn it on
        {
            m_pRegion->m_bOverrideWaveRootNote =  
                (m_pRegion->m_rWSMP.usUnityNote == waveWSMPL.usUnityNote) ? false : true;
        }
        
        if (!m_pRegion->m_bOverrideWaveAttenuation) // see if we need to turn it on
        {
            m_pRegion->m_bOverrideWaveAttenuation =  
                (m_pRegion->m_rWSMP.lAttenuation == waveWSMPL.lAttenuation) ? false : true;
        }

        if (m_pRegion->m_bOverrideWaveFineTune) // see if we need to turn it on
        {
            m_pRegion->m_bOverrideWaveFineTune =  
                (m_pRegion->m_rWSMP.sFineTune == waveWSMPL.sFineTune) ? false : true;
        }

        m_fActivateRootNote = m_pRegion->m_bOverrideWaveRootNote;
        m_fActivateAttenuation = m_pRegion->m_bOverrideWaveAttenuation;
        m_fActivateFineTune = m_pRegion->m_bOverrideWaveFineTune;
    }
    else    // If there is no wave, override must be true.
    {
        m_pRegion->m_bOverrideWaveRootNote = true;
        m_fActivateRootNote = true;

        m_pRegion->m_bOverrideWaveAttenuation = true;
        m_fActivateAttenuation = true;

        m_pRegion->m_bOverrideWaveFineTune = true;
        m_fActivateFineTune = true;

    }

	// If the user chose to override wave settings we use the region root note.
    // else the region root note and the wave root note are the same.
    m_RootNoteSpin.SetRange(0, 127);
	if (m_pRegion->m_bOverrideWaveRootNote)
    {
	    UpdateRootNoteText(m_pRegion->m_rWSMP.usUnityNote);
		m_RootNoteSpin.SetPos(m_pRegion->m_rWSMP.usUnityNote);
	}
    else    
    {   // If we are not overriding the wave, there must be a wave.
        ASSERT(m_pRegion->m_pWave);
		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();

		UpdateRootNoteText(waveWSMPL.usUnityNote);
        // use default root note for a region
        m_pRegion->m_rWSMP.usUnityNote = waveWSMPL.usUnityNote;
		m_RootNoteSpin.SetPos(waveWSMPL.usUnityNote);
    }
   	
	

    // Set finetune and attenuation
    if (m_pRegion->m_bOverrideWaveAttenuation)
    {
    	m_lAttenuation = m_pRegion->m_rWSMP.lAttenuation;
    }
    else    
    {   //If we are not overriding the wave, there must be a wave.
        ASSERT(m_pRegion->m_pWave);
		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();

    	m_lAttenuation = waveWSMPL.lAttenuation;
        //else use default root note for a region
        m_pRegion->m_rWSMP.lAttenuation = m_lAttenuation;
    }

    if (m_pRegion->m_bOverrideWaveFineTune)
    {
        m_lFineTune = (long) m_pRegion->m_rWSMP.sFineTune << 16;
    }
    else    
    {   //If we are not overriding the wave, there must be a wave.
        ASSERT(m_pRegion->m_pWave);
		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();

        m_lFineTune = (long) waveWSMPL.sFineTune << 16;
        //else use default root note for a region
        m_pRegion->m_rWSMP.sFineTune = waveWSMPL.sFineTune;
    }
       
    // Initialize the sliders
	if (m_pmsTune)
		{
		m_pmsTune->SetValue(this, m_lFineTune);
		m_pmsAttenuation->SetValue(this, m_lAttenuation);
		}

	char text[BUFFER_64];
	if (m_lFineTune < 0)
    {
		m_lFineTune = -m_lFineTune >> 16;
        sprintf(text,"-%02d.%02d ", m_lFineTune / 100, m_lFineTune % 100);
	}
	else
	{
		m_lFineTune = m_lFineTune >> 16;		
		sprintf(text," %02d.%02d", m_lFineTune / 100, m_lFineTune % 100);
	}

	CWnd * pDisplay = GetDlgItem(IDC_OWS_DTUNE);
	if(pDisplay)
	{
		pDisplay->SetWindowText(text);
	}

	sprintf(text, "%d", m_lAttenuation);
    if (m_lAttenuation < 0)
    {
        m_lAttenuation = -m_lAttenuation >> 16;
        sprintf(text,"-%02d.%01d", m_lAttenuation / 10, m_lAttenuation % 10);
    }
    else 
    {
        m_lAttenuation = m_lAttenuation >> 16;
        sprintf(text," %02d.%01d", m_lAttenuation / 10, m_lAttenuation % 10);
    }

	pDisplay = GetDlgItem(IDC_OWS_ATTENUATION);

	if(pDisplay)
	{
		pDisplay->SetWindowText(text);
	}

    // Now enable controls
	EnableControls(TRUE);
	m_fInOnInitialUpdate = false;
	return bRetval;
}

void CRegionPropPg::OnOwsActiveAttenuation() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Save the undo state 
	if(m_pRegion && m_pRegion->m_pInstrument)
	{
		if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_OVERRIDE_ATTENUATION)))
		{
			CButton * pBtn = (CButton *)GetDlgItem(IDC_OWS_ACTIVE_ATTENUATION);
			if (pBtn)
			{
				pBtn->SetCheck(m_fActivateAttenuation);
			}
			return;
		}
	}


    CButton* pCheckBox = (CButton *) GetDlgItem(IDC_OWS_ACTIVE_ATTENUATION);

	if(pCheckBox)
	{
        if (m_pRegion->m_pWave == NULL)
        {
            AfxMessageBox(IDS_CANNOT_CHECK_OVERRIDE, MB_ICONEXCLAMATION);
            pCheckBox->SetCheck(true);
            return;
        }

		UINT uState = pCheckBox->GetState();	
	
		switch(uState & 0x0003)
		{
			case 0:
				m_pRegion->m_bOverrideWaveAttenuation = m_fActivateAttenuation = false;
                RevertToWaveAttenuation();                
				EnableAttenuation(false);
				break;
		
			case 1:
				m_pRegion->m_bOverrideWaveAttenuation = m_fActivateAttenuation = true;
				EnableAttenuation(true);
				break;
		}

		if(m_pRegion->m_pInstrument && m_pRegion->m_pInstrument->m_pCollection)
			m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
	}
}

void CRegionPropPg::OnOwsActiveFineTune() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
		
	// Save the undo state 
	if(m_pRegion && m_pRegion->m_pInstrument)
	{
		if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_OVERRIDE_FINETUNE)))
		{
			CButton * pBtn = (CButton *)GetDlgItem(IDC_OWS_ACTIVE_FINE_TUNE);
			if (pBtn)
			{
				pBtn->SetCheck(m_fActivateFineTune);
			}
			return;
		}	
	}

    CButton* pCheckBox = (CButton *) GetDlgItem(IDC_OWS_ACTIVE_FINE_TUNE);

	if(pCheckBox)
	{
        if (m_pRegion->m_pWave == NULL)
        {
            AfxMessageBox(IDS_CANNOT_CHECK_OVERRIDE, MB_ICONEXCLAMATION);
            pCheckBox->SetCheck(true);
            return;
        }

        UINT uState = pCheckBox->GetState();	
	
		switch(uState & 0x0003)
		{
			case 0:
				m_pRegion->m_bOverrideWaveFineTune = m_fActivateFineTune = false;
                RevertToWaveFineTune();                
				EnableFineTune(false);
				break;
		
			case 1:
				m_pRegion->m_bOverrideWaveFineTune = m_fActivateFineTune = true;
				EnableFineTune(true);
				break;
		}
		
		if(m_pRegion->m_pInstrument && m_pRegion->m_pInstrument->m_pCollection)
			m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
	}
}

void CRegionPropPg::OnOwsActiveRootNote() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	// Save the undo state 
	if(m_pRegion && m_pRegion->m_pInstrument)
	{
		if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_OVERRIDE_ROOTNOTE)))
		{
			CButton * pBtn = (CButton *)GetDlgItem(IDC_OWS_ACTIVE_ROOT_NOTE);
			if (pBtn)
			{
				pBtn->SetCheck(m_fActivateRootNote);
			}
			return;
		}
	}

    CButton* pCheckBox = (CButton *) GetDlgItem(IDC_OWS_ACTIVE_ROOT_NOTE);

	if(pCheckBox)
	{
        if (m_pRegion->m_pWave == NULL)
        {
            AfxMessageBox(IDS_CANNOT_CHECK_OVERRIDE, MB_ICONEXCLAMATION);
            pCheckBox->SetCheck(true);
            return;
        }
		
        UINT uState = pCheckBox->GetState();	
	
		switch(uState & 0x0003)
		{
			case 0:
				m_pRegion->m_bOverrideWaveRootNote = m_fActivateRootNote = false;
                RevertToWaveRootNote();                
				EnableRootNote(false);
				break;
		
			case 1:
				m_pRegion->m_bOverrideWaveRootNote = m_fActivateRootNote = true;
				EnableRootNote(true);
				break;
		}

		if(m_pRegion->m_pInstrument && m_pRegion->m_pInstrument->m_pCollection)
			m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
	}
}

void CRegionPropPg::UpdateOwsDattenuation() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_fInOnInitialUpdate || !m_pRegion)
	{
		return;
	}
	if(UpdateData(TRUE))
	{
		if(m_pRegion->m_rWSMP.lAttenuation != m_lAttenuation)
		{
			if(m_pRegion && m_pRegion->m_pInstrument)
			{
				if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_ATTENUATION)))
				{
					m_lAttenuation = m_pRegion->m_rWSMP.lAttenuation ;
					m_pmsAttenuation->SetValue(this, m_lAttenuation);
					UpdateData(FALSE);
					return;
				}
			}
		}
		else
			return;

		m_pRegion->m_rWSMP.lAttenuation = m_lAttenuation;

		// Set flag so we know to save file 
		m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
		m_pRegion->m_pInstrument->UpdateInstrument();
		m_pRegion->m_pInstrument->RefreshRegion();
	}
}

void CRegionPropPg::UpdateOwsDtune() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_fInOnInitialUpdate || !m_pRegion)
	{
		return;
	}
	if(UpdateData(TRUE))
	{
		if(m_pRegion->m_rWSMP.sFineTune != (WORD) (m_lFineTune >> 16))
		{
			if(m_pRegion && m_pRegion->m_pInstrument)
			{
				if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_FINETUNE)))
				{
					m_lFineTune = (long) m_pRegion->m_rWSMP.sFineTune << 16;
					m_pmsTune->SetValue(this, m_lFineTune);
					UpdateData(FALSE);
					return;
				}
			}
		}
		else
			return;

		m_pRegion->m_rWSMP.sFineTune = (WORD) (m_lFineTune >> 16);

		// Set flag so we know to save file 
		m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
		m_pRegion->m_pInstrument->UpdateInstrument();
	}
}

void CRegionPropPg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CSliderCollection::OnHScroll(nSBCode, nPos, pScrollBar);
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRegionPropPg::EnablePhaseGroupControls(bool bNewState)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	EnablePhaseGroupEdit(bNewState);
	m_MultichannelCheck.EnableWindow(bNewState);

	EnableChannelCombo(bNewState);

}

void CRegionPropPg::EnablePhaseGroupEdit(bool bNewState)
{
	bool bEnablePhaseGroupEdit = bNewState;
	if(bNewState == TRUE)
	{
		ASSERT(m_pRegion);
		WAVELINK waveLink = m_pRegion->GetWaveLink();
		bEnablePhaseGroupEdit = waveLink.usPhaseGroup != 0;
	}

	m_PhaseGroupEdit.EnableWindow(bEnablePhaseGroupEdit);
	m_PhaseGroupSpin.EnableWindow(bEnablePhaseGroupEdit);
	m_MasterCheck.EnableWindow(bEnablePhaseGroupEdit);
}

void CRegionPropPg::EnableChannelCombo(bool bNewState)
{
	bool bEnableChannelCombo = bNewState;
	if(bNewState == TRUE)
	{
		ASSERT(m_pRegion);
		WAVELINK waveLink = m_pRegion->GetWaveLink();
		bEnableChannelCombo = (waveLink.fusOptions & F_WAVELINK_MULTICHANNEL) != 0;
	}

	m_ChannelCombo.EnableWindow(bEnableChannelCombo);
}

void CRegionPropPg::EnableFineTune(bool bNewState)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (m_pmsTune)
		m_pmsTune->EnableControl(this, bNewState);

	CWnd* pCtrl = GetDlgItem(IDC_OWS_STATIC_FINE_TUNE);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_OWS_STATIC_S_TONES);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

    CButton * pBtn = (CButton *)GetDlgItem(IDC_OWS_ACTIVE_FINE_TUNE);
    if (pBtn)
    {
        pBtn->SetCheck(m_fActivateFineTune);
    }

}

void CRegionPropPg::EnableAttenuation(bool bNewState)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (m_pmsAttenuation)
		m_pmsAttenuation->EnableControl(this, bNewState);

	CWnd* pCtrl = GetDlgItem(IDC_OWS_STATIC_ATTENUATION);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_OWS_STATIC_DB);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

    CButton * pBtn = (CButton *)GetDlgItem(IDC_OWS_ACTIVE_ATTENUATION);
    if (pBtn)
    {
        pBtn->SetCheck(m_fActivateAttenuation);
    }

}

void CRegionPropPg::EnableRootNote(bool bNewState)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CWnd* pCtrl = GetDlgItem(IDC_OWS_STATIC_ROOT_NOTE);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_OWS_ROOT_NODE_SPIN);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_OWS_DUNITYNOTE);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

    CButton * pBtn = (CButton *)GetDlgItem(IDC_OWS_ACTIVE_ROOT_NOTE);
    if (pBtn)
    {
        pBtn->SetCheck(m_fActivateRootNote);
    }

}

int CRegionPropPg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
	if(!FromHandlePermanent(m_hWnd))
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		Attach( hWnd );

		m_fNeedToDetach = TRUE;
	}

	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CRegionPropPg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.
	if( m_fNeedToDetach && m_hWnd != NULL )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
	}

	CPropertyPage::OnDestroy();
}

void CRegionPropPg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	/*CMenu* pPopupMenu =  m_pContextMenu->GetSubMenu(0);
	
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON,
							   point.x,
							   point.y,
							   this,
							   NULL);*/
}

void CRegionPropPg::OnKillfocusOwsDunitynote() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	USHORT usNewNote = GetNoteInEditBox();
	UpdateRootNote(usNewNote);
}

// ====================================================================
// Changes the instrument dialog wave root note and the region property
// root note to be the root note set for the wave.
// ====================================================================
void CRegionPropPg::RevertToWaveRootNote()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    // Get the wave's root note.
    ASSERT(m_pRegion);
    ASSERT(m_pRegion->m_pWave); // there should always be one.

    if (m_pRegion->m_pWave) 
    {
		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();

        m_pRegion->m_rWSMP.usUnityNote = waveWSMPL.usUnityNote;
        
        // change the unitynote stored in CRegionPropPg.
        UpdateRootNoteText(m_pRegion->m_rWSMP.usUnityNote);
		//m_RootNoteSpin.SetPos(m_pRegion->m_rWSMP.usUnityNote);
 
        // Calling SetPos results in an update message sent to the root_note edit box
        // and it will dirty collection and updateSynth also. This updates the 
        // property page also.
        if (m_pRegion->m_pInstrumentFVEditor)
        {
            if (m_pRegion->m_pInstrumentFVEditor->m_RootNoteSpin.GetSafeHwnd())
            {
                m_pRegion->m_pInstrumentFVEditor->m_RootNoteSpin.SetPos(m_pRegion->m_rWSMP.usUnityNote);
				char pszNote[20];
				notetostring(m_pRegion->m_rWSMP.usUnityNote, pszNote);
				m_pRegion->m_pInstrumentFVEditor->m_RootNoteEdit.SetWindowText(pszNote);
				m_pRegion->m_pInstrumentFVEditor->m_RegionKeyBoard.SetRootNote(m_pRegion->m_rWSMP.usUnityNote);
            }
        }
        else    // instr. editor isnot open. So update the root note yourself in prop. pg
        {
            if (m_RootNoteSpin.GetSafeHwnd())
            {
                m_RootNoteSpin.SetPos(m_pRegion->m_rWSMP.usUnityNote);
            }
        }

    }
}

// ====================================================================
// Changes the instrument dialog wave Attenuation and the region property
// attenuation to be the value set for the wave.
// ====================================================================
void CRegionPropPg::RevertToWaveAttenuation()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    // Get the wave's attenuation.
    ASSERT(m_pRegion);
    ASSERT(m_pRegion->m_pWave); // there should always be one.

    if (m_pRegion->m_pWave) 
    {
		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();

        m_pRegion->m_rWSMP.lAttenuation = waveWSMPL.lAttenuation;
        
        // change the value stored in CRegionPropPg.
        m_lAttenuation = m_pRegion->m_rWSMP.lAttenuation;

        // Set the slider position. This will cause the slider pos to be updated and
        // the corresponding text to be displayed.
        m_pmsAttenuation->SetValue(this, m_lAttenuation);        

		// Set flag so we know to save file 
		m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
		m_pRegion->m_pInstrument->UpdateInstrument();
		m_pRegion->m_pInstrument->RefreshRegion();
    }
}

// ====================================================================
// Changes the instrument dialog wave finetune and the region property
// finetune to be the value set for the wave.
// ====================================================================
void CRegionPropPg::RevertToWaveFineTune()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    // Get the wave's finetune.
    ASSERT(m_pRegion);
    ASSERT(m_pRegion->m_pWave); // there should always be one.

    if (m_pRegion->m_pWave) 
    {
		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();

        m_pRegion->m_rWSMP.sFineTune = waveWSMPL.sFineTune;
        
        // change the value stored in CRegionPropPg.
        m_lFineTune = m_pRegion->m_rWSMP.sFineTune << 16;

        // Set the slider position. This will cause the slider pos to be updated and
        // the corresponding text to be displayed.
        m_pmsTune->SetValue(this, m_lFineTune);

		// Set flag so we know to save file 
		m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
		m_pRegion->m_pInstrument->UpdateInstrument();
		m_pRegion->m_pInstrument->RefreshRegion();
    }
}

void CRegionPropPg::UpdateRootNote(USHORT usNewNote)
{
	if(m_pRegion == NULL)
		return;
	if(!m_pRegion->m_pWave)
		return;

	// update the wave if the note's different
	if (usNewNote != m_pRegion->m_rWSMP.usUnityNote) 
	{
		// change the value in the instrument editor also.
		if (m_pRegion->m_pInstrumentFVEditor)
		{
			if (m_pRegion->m_pWave)
			{
				m_pRegion->m_pInstrumentFVEditor->UpdateRootNote(usNewNote);
			}
		}
	}
}

void CRegionPropPg::UpdateRootNoteText(int note)
{
	ASSERT(note >= 0 && note <= 127);
	char pszNote[20];
	notetostring((DWORD)note, pszNote);
	m_RootNoteEdit.SetWindowText(pszNote);
}

// get the note typed by the user in the edit box, if valid. if not valid,
// returns the current unity note for the wave.
USHORT CRegionPropPg::GetNoteInEditBox()
{
	ASSERT(m_pRegion->m_pWave);

	USHORT usNote;
	CString strNote;
	m_RootNoteEdit.GetWindowText(strNote);
	LPSTR lpsBuf = strNote.GetBuffer(3);
	if (isValidNoteString(lpsBuf)) {
		usNote = stringtonote(lpsBuf);
	}
	else {
		usNote = m_pRegion->m_rWSMP.usUnityNote;
	}
	
	// reset the note in the edit box
	char szNote[20];
	notetostring(usNote, szNote);
	m_RootNoteEdit.SetWindowText(szNote);
	
	return usNote;
}

void CRegionPropPg::SetRootNote(USHORT usNewNote)
{
	UpdateRootNoteText(usNewNote);
	m_RootNoteSpin.SetPos(usNewNote);
}


void CRegionPropPg::OnDeltaposOwsRootNodeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	USHORT usNote = GetNoteInEditBox();

	int nNewPos = usNote + pNMUpDown->iDelta;
	
	if (nNewPos < 0) 
		nNewPos = 0; 
	else if (nNewPos > 127) 
		nNewPos = 127;

	*pResult = 1;

	UpdateRootNoteText(nNewPos);
	OnKillfocusOwsDunitynote(); 
}

void CRegionPropPg::InitPhaseGroupControls()
{
	ASSERT(m_pRegion);
	if(m_pRegion == NULL)
	{
		return;
	}

	m_PhaseGroupSpin.SetRange(1, 255);
	InitChannelCombo();

	WAVELINK waveLink = m_pRegion->GetWaveLink();
	
	bool bPhaseGroupCheck = (waveLink.usPhaseGroup != 0);
	m_PhaseGroupCheck.SetCheck(bPhaseGroupCheck);
	
	bool bMasterCheck = (waveLink.fusOptions & F_WAVELINK_PHASE_MASTER) != 0;
	m_MasterCheck.SetCheck(bMasterCheck);

	bool bMultiChannelCheck = (waveLink.fusOptions & F_WAVELINK_MULTICHANNEL) != 0;
	m_MultichannelCheck.SetCheck(bMultiChannelCheck);

	m_PhaseGroupSpin.SetPos(waveLink.usPhaseGroup);
	USHORT usBitSet = 0;
	for(USHORT nIndex = 0; nIndex < (sizeof(waveLink.ulChannel) * 8); nIndex++)
	{
		ULONG ulTest = 1;
		if(waveLink.ulChannel & (1 << nIndex))
		{
			usBitSet = nIndex;
			break;
		}
	}

	m_ChannelCombo.SetCurSel(usBitSet);
}

void CRegionPropPg::InitChannelCombo()
{
	if (m_pRegion == NULL)
		{
		ASSERT(FALSE);
		return;
		}

	CString sChannel;
	m_ChannelCombo.ResetContent();
	if ((m_pRegion->m_pWave != NULL) && (m_pRegion->m_pWave->GetChannelCount() > 1))
		{
		sChannel.LoadString(IDS_REGION_CHANNEL_STEREO);
		m_ChannelCombo.AddString(sChannel);
		}
	else
		{
		for(int nIndex = 0; nIndex < MAX_WAVELINK_CHANNELS; nIndex++)
		{
			sChannel.LoadString(m_arrChannelPositions[nIndex]);
			m_ChannelCombo.AddString(sChannel);
		}
	}
}

void CRegionPropPg::OnPhaseGroupCheck() 
{
	ASSERT(m_pRegion);
	if(m_pRegion == NULL)
	{
		return;
	}

	int nCheck = m_PhaseGroupCheck.GetCheck();

	// Phase Group is 0 if the region doesn't belong to any Phase Group
	int nPhaseGroup = 0;
	if(nCheck)
	{
		nPhaseGroup = m_PhaseGroupSpin.GetPos();
	}

	WAVELINK waveLink = m_pRegion->GetWaveLink();

	// No change?
	if(nPhaseGroup == waveLink.usPhaseGroup)
	{
		return;
	}

	waveLink.usPhaseGroup = USHORT(nPhaseGroup);

	SaveUndoState(IDS_UNDO_PHASE_GROUP);
	SetRegionWaveLink(m_pRegion, waveLink);
	EnablePhaseGroupEdit(nCheck != 0);
}

void CRegionPropPg::OnKillfocusPhaseGroupEdit() 
{
	CString sPhaseGroup;
	m_PhaseGroupEdit.GetWindowText(sPhaseGroup);
	
	int nPhaseGroup = atoi((LPCSTR)sPhaseGroup);
	if(nPhaseGroup < 1 || nPhaseGroup > 255)
	{
		nPhaseGroup = m_PhaseGroupSpin.GetPos();
		sPhaseGroup.Format("%d", nPhaseGroup);
		m_PhaseGroupEdit.SetWindowText(sPhaseGroup);
		return;
	}

	m_PhaseGroupSpin.SetPos(nPhaseGroup);
	WAVELINK waveLink = m_pRegion->GetWaveLink();

	// No change?
	if(waveLink.usPhaseGroup == nPhaseGroup)
	{
		return;
	}

	waveLink.usPhaseGroup = USHORT(nPhaseGroup);
	
	SaveUndoState(IDS_UNDO_PHASE_GROUP_ID);
	SetRegionWaveLink(m_pRegion, waveLink);
}

void CRegionPropPg::OnDeltaposPhaseGroupSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	OnKillfocusPhaseGroupEdit();
	
	*pResult = 0;
}

void CRegionPropPg::OnMasterCheck() 
{
	ASSERT(m_pRegion);
	if(m_pRegion == NULL)
	{
		return;
	}

	int nMasterCheck = m_MasterCheck.GetCheck();
	
	WAVELINK thisWaveLink = m_pRegion->GetWaveLink();
	USHORT usThisPhaseGroup = thisWaveLink.usPhaseGroup;

	// There can be only one Master in the same phase group
	// So look in all the regions that belong to the same phase group in an instrument
	// and warn and change the master flag
	if(nMasterCheck)
	{
		CInstrument* pInstrument = m_pRegion->GetInstrument();
		ASSERT(pInstrument);
		if(pInstrument == NULL)
		{
			return;
		}

		CInstrumentRegions* pInstrumentRegions = pInstrument->GetRegions();
		ASSERT(pInstrumentRegions);
		if(pInstrumentRegions == NULL)
		{
			return;
		}

		CRegion* pRegion  = pInstrumentRegions->GetHead();
		while(pRegion)
		{
			WAVELINK waveLink = pRegion->GetWaveLink();
			if(waveLink.usPhaseGroup ==  usThisPhaseGroup && waveLink.fusOptions & F_WAVELINK_PHASE_MASTER)
			{
				if(pRegion == m_pRegion)
				{
					continue;
				}
				else
				{
					int nChoice = AfxMessageBox(IDS_MULTIPLE_MASTERS_IN_PHASE_GROUP, MB_OKCANCEL);
					if(nChoice == IDOK)
					{
						static bool bSavedUndoState = false;
						if(!bSavedUndoState)
						{
							SaveUndoState(IDS_UNDO_PHASE_GROUP_MASTER);
						}

						waveLink.fusOptions &= ~F_WAVELINK_PHASE_MASTER;
						SetRegionWaveLink(pRegion, waveLink);
					}
					else
					{
						thisWaveLink.fusOptions &= ~F_WAVELINK_PHASE_MASTER;
						m_MasterCheck.SetCheck(0);
						return;
					}
				}
			}

			pRegion = pRegion->GetNext();
		}

		thisWaveLink.fusOptions |= F_WAVELINK_PHASE_MASTER;
	}
	else
	{
		SaveUndoState(IDS_UNDO_PHASE_GROUP_MASTER);
		thisWaveLink.fusOptions &= ~F_WAVELINK_PHASE_MASTER;
	}

	
	SetRegionWaveLink(m_pRegion, thisWaveLink);
}



void CRegionPropPg::OnSelchangeChannelCombo() 
{
	ASSERT(m_pRegion);
	if(m_pRegion == NULL)
	{
		return;
	}

	WAVELINK waveLink = m_pRegion->GetWaveLink();
	int nCurSel = m_ChannelCombo.GetCurSel();
	ULONG ulChannel = 1 << nCurSel;

	if(waveLink.ulChannel == ulChannel)
	{
		return;
	}

	waveLink.ulChannel = ulChannel;

	SaveUndoState(IDS_UNDO_CHANNEL);
	SetRegionWaveLink(m_pRegion, waveLink);
}

void CRegionPropPg::OnMultichannelCheck() 
{
	ASSERT(m_pRegion);
	if(m_pRegion == NULL)
	{
		return;
	}

	int nCheck = m_MultichannelCheck.GetCheck();

	WAVELINK waveLink = m_pRegion->GetWaveLink();
	if(nCheck == 0)
	{
		// Use it as a mono Wave
		waveLink.fusOptions &= ~F_WAVELINK_MULTICHANNEL;
		waveLink.ulChannel = 1; 
	}
	else
	{
		waveLink.fusOptions |= F_WAVELINK_MULTICHANNEL;
	}

	SaveUndoState(IDS_UNDO_MULTICHANNEL);
	SetRegionWaveLink(m_pRegion, waveLink);
	EnableChannelCombo(nCheck != 0);
}

void CRegionPropPg::SetRegionWaveLink(CRegion* pRegion, WAVELINK waveLink)
{
	ASSERT(pRegion);
	if(pRegion == NULL)
	{
		return;
	}

	CInstrument* pInstrument = pRegion->GetInstrument();
	ASSERT(pInstrument);
	if(pInstrument == NULL)
	{
		return;
	}

	pRegion->SetWaveLink(waveLink);
	pInstrument->UpdateInstrument();
}

void CRegionPropPg::SaveUndoState(UINT uUndoID)
{
	ASSERT(m_pRegion);
	if(m_pRegion == NULL)
	{
		return;
	}

	CInstrument* pInstrument = m_pRegion->GetInstrument();
	ASSERT(pInstrument);
	if(pInstrument)
	{
		pInstrument->SaveStateForUndo(uUndoID);
	}
}

BOOL CRegionPropPg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult;
	if (CSliderCollection::OnCommand(wParam, lParam, &lResult))
		return lResult;

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CRegionPropPg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LRESULT lResult;
	if (CSliderCollection::OnNotify(wParam, lParam, &lResult))
		return lResult;
	
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

BOOL CRegionPropPg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// create and hook up the sliders
	CSliderCollection::Free(); // refresh sliders after a page switch
	
	m_pmsTune = Insert(
		IDC_OWS_TUNE,
		IDC_OWS_DTUNE,
		IDC_OWS_DTUNE_SPIN,
		MYSLIDER_PITCHCENTS, 
		0,
		IDS_UNDO_ATTENUATION,
		&m_lFineTune);
	    
	m_pmsAttenuation = Insert(
		IDC_OWS_ATTENUATION,
		IDC_OWS_DATTENUATION,
		IDC_OWS_DATTENUATION_SPIN,
		MYSLIDER_VOLUME, 
		0,
		IDS_UNDO_FINETUNE,
		&m_lAttenuation);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

bool CRegionPropPg::OnSliderUpdate(MySlider *pms, DWORD dwmscupdf)
{
	switch (dwmscupdf)
		{
		case dwmscupdfStart:
			return true; // save state is done on End of updates

		case dwmscupdfEnd:
			if (pms == m_pmsAttenuation)
				UpdateOwsDattenuation();
			else
				{
				ASSERT(pms == m_pmsTune); // slider not handled
				UpdateOwsDtune();
				}
			return true;

		default:
			ASSERT(FALSE);
			return false;
		}
}

