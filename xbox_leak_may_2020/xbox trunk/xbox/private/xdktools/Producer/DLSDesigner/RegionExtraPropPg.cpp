// RegionExtraPropPg.cpp : implementation file
//

#include "stdafx.h"
#include "RegionPropPgMgr.h"
#include "RegionExtraPropPg.h"
#include "region.h"
#include "DlsDefsPlus.h"
#include "Collection.h"
#include "Instrument.h"
#include "Wave.h"
#include "MonoWave.h"
#include "RegionPropPgMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegionExtraPropPg property page

IMPLEMENT_DYNCREATE(CRegionExtraPropPg, CPropertyPage)

CRegionExtraPropPg::CRegionExtraPropPg() : CPropertyPage(CRegionExtraPropPg::IDD), 
m_pRegion(NULL),
m_fActivateOverride(false),
m_fActivateAST(false),
m_fActivateASC(false),
m_fInOnInitialUpdate(true),
m_fOneShot(false),
m_pContextMenu(NULL),
m_bfTouched(0),
m_bTouchedByProgram(false),
m_dwLoopStart(MINIMUM_VALID_LOOPSTART),
m_dwLoopLength(0)
{
	//{{AFX_DATA_INIT(CRegionExtraPropPg)
	m_fAllowCompress = FALSE;
	m_fAllowTruncate = FALSE;
	//}}AFX_DATA_INIT

	m_pContextMenu = new CMenu();
	m_pContextMenu->LoadMenu(IDM_REGION_PROP_PAGE_RMENU);

	m_fNeedToDetach = FALSE;
}

CRegionExtraPropPg::~CRegionExtraPropPg()
{
	delete m_pContextMenu;
}

/////////////////////////////////////////////////////////////////////////////
// CRegionExtraPropPg::EnableControls

void CRegionExtraPropPg::EnableControls(bool fEnable) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	CWnd* pCtrl;

	pCtrl = GetDlgItem(IDC_OWS_OVERRIDE);
	if(pCtrl)
	{
		pCtrl->EnableWindow(fEnable);
		pCtrl = NULL;
	}
		
    if (m_pRegion)
    {
	    EnableAST(m_pRegion->m_bOverrideWaveMoreSettings && fEnable);
	    
	    EnableASC(m_pRegion->m_bOverrideWaveMoreSettings && fEnable);

    	EnablePlayBack(m_pRegion->m_bOverrideWaveMoreSettings && fEnable);
    }
    else    // no region. fEnable will be false.
    {
        ASSERT(fEnable == FALSE);
	    EnableAST(fEnable);	   
	    EnableASC(fEnable);
    }

	// Loop controls should be grayed if playback is not checked.
    if (m_pRegion)
    {
        EnableLoopCtrls(m_pRegion->m_bOverrideWaveMoreSettings && !m_fOneShot && fEnable);
    }
}

void CRegionExtraPropPg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegionExtraPropPg)
	DDX_Control(pDX, IDC_SAMPLE_LENGTH_EDIT, m_SampleLengthEdit);
	DDX_Control(pDX, IDC_OWS_LOOPSTART, m_LoopStartEdit);
	DDX_Control(pDX, IDC_OWS_LOOPLENGTH, m_LoopLengthEdit);
	DDX_Check(pDX, IDC_OWS_ALLOW_COMPRESS, m_fAllowCompress);
	DDX_Check(pDX, IDC_OWS_ALLOW_TRUNCATE, m_fAllowTruncate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegionExtraPropPg, CPropertyPage)
	//{{AFX_MSG_MAP(CRegionExtraPropPg)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_OWS_LOOP, OnOwsLoop)
	ON_BN_CLICKED(IDC_OWS_ALLOW_TRUNCATE, OnOwsAllowTruncate)
	ON_BN_CLICKED(IDC_OWS_OVERRIDE, OnOwsOverride)
	ON_BN_CLICKED(IDC_OWS_ALLOW_COMPRESS, OnOwsAllowCompress)
	ON_BN_CLICKED(IDC_OWS_ONE_SHOT, OnOwsOneShot)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_REVERT_TO_WAVE, OnRevertToWave)
	ON_EN_KILLFOCUS(IDC_OWS_LOOPSTART, OnKillfocusOwsLoopstart)
	ON_EN_KILLFOCUS(IDC_OWS_LOOPLENGTH, OnKillfocusOwsLooplength)
	ON_EN_CHANGE(IDC_OWS_LOOPLENGTH, OnChangeLooplength)
	ON_EN_CHANGE(IDC_OWS_LOOPSTART, OnChangeLoopstart)
	ON_MESSAGE(DM_VALIDATE,OnValidate)
	ON_BN_CLICKED(IDC_LOOP_AND_RELEASE_RADIO, OnOwsLoop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionExtraPropPg message handlers

BOOL CRegionExtraPropPg::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_fInOnInitialUpdate = true;

	if(m_pRegion == NULL || m_pRegion->m_pWave == NULL) 
	{
		EnableControls(FALSE);
		CRegionPropPgMgr::dwLastSelPage = REGION_EXTRA_PROP_PAGE;
		return CPropertyPage::OnSetActive();
	}

	// Get the loop values from the wave
	WLOOP waveLoop = m_pRegion->m_pWave->GetWLOOP();

	// Get the smpl values from the wave
	WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();

	// Get values from regions. If override is off we will set it to wave's properties.
#if MINIMUM_VALID_LOOPSTART > 0
	if(m_pRegion->m_rWLOOP.ulStart >= MINIMUM_VALID_LOOPSTART)
		m_dwLoopStart = m_pRegion->m_rWLOOP.ulStart;
	else
		m_pRegion->m_rWLOOP.ulStart = m_dwLoopStart;
#else
	m_dwLoopStart = m_pRegion->m_rWLOOP.ulStart;
#endif

	// Can't have a looop length less than 5 can we?
	if(m_pRegion->m_rWLOOP.ulLength >= MINIMUM_VALID_LOOPLENGTH)
		m_dwLoopLength = m_pRegion->m_rWLOOP.ulLength;
	else
	{
		m_pRegion->m_rWLOOP.ulLength = waveLoop.ulLength;
		m_dwLoopLength = m_pRegion->m_rWLOOP.ulLength;
	}

	m_fAllowTruncate = !(m_pRegion->m_rWSMP.fulOptions & F_WSMP_NO_TRUNCATION);
	m_fAllowCompress = !(m_pRegion->m_rWSMP.fulOptions & F_WSMP_NO_COMPRESSION);
    m_fOneShot = (m_pRegion->m_rWSMP.cSampleLoops == 0);
    
    //override wave playback settings?
    if (m_pRegion->m_pWave)
    {   
		// Set the total sample length
		CString sSampleLength;
		sSampleLength.Format("%d", m_pRegion->m_pWave->m_dwWaveLength);
		m_SampleLengthEdit.SetWindowText(sSampleLength);

        if (!m_pRegion->m_bOverrideWaveMoreSettings)    // see if we need to turn it on
        {   // override is off so set it to wave settings   
	        m_fOneShot = !(m_pRegion->m_pWave->IsLooped()); 

#if MINIMUM_VALID_LOOPSTART > 0
			if(waveLoop.ulStart >= MINIMUM_VALID_LOOPSTART)
				m_dwLoopStart = waveLoop.ulStart;
			else
				waveLoop.ulStart = m_dwLoopStart;
#else
			m_dwLoopStart = waveLoop.ulStart;
#endif

			
			if(waveLoop.ulLength >= MINIMUM_VALID_LOOPLENGTH)
			{
				m_dwLoopLength = waveLoop.ulLength;
			}
			else if(m_pRegion->m_pWave->m_dwWaveLength >= MINIMUM_VALID_LOOPLENGTH)
			{
				waveLoop.ulLength = m_pRegion->m_pWave->m_dwWaveLength;
				m_dwLoopLength = waveLoop.ulLength;
			}
			else
			{
				waveLoop.ulLength = MINIMUM_VALID_LOOPLENGTH;
				m_dwLoopLength = MINIMUM_VALID_LOOPLENGTH;
			}

			m_fAllowTruncate = !(waveWSMPL.fulOptions & F_WSMP_NO_TRUNCATION);
			m_fAllowCompress = !(waveWSMPL.fulOptions & F_WSMP_NO_COMPRESSION);
        }
		
		m_pRegion->m_pWave->SetWLOOP(waveLoop);
    }
    else   // no wave 
    {
        m_pRegion->m_bOverrideWaveMoreSettings = true;
    }

   // override Check
	m_fActivateOverride = (m_pRegion->m_bOverrideWaveMoreSettings != 0);
    CButton * pCheckBox = (CButton *) GetDlgItem(IDC_OWS_OVERRIDE);
    if (pCheckBox)
    {
        pCheckBox->SetCheck(m_pRegion->m_bOverrideWaveMoreSettings);
    }

	char text[BUFFER_64];
	sprintf(text, "%d", m_dwLoopStart);
	CWnd * pDisplay = GetDlgItem(IDC_OWS_LOOPSTART);
	if(pDisplay)
	{
		pDisplay->SetWindowText(text);
	}

	sprintf(text, "%d", m_dwLoopLength);
	pDisplay = GetDlgItem(IDC_OWS_LOOPLENGTH);
	if(pDisplay)
	{
		pDisplay->SetWindowText(text);
	}

    pCheckBox = (CButton *) GetDlgItem(IDC_OWS_ALLOW_TRUNCATE);
	if(pCheckBox)
	{
		pCheckBox->SetCheck(m_fAllowTruncate);
	}

	pCheckBox = (CButton *) GetDlgItem(IDC_OWS_ALLOW_COMPRESS);
	if(pCheckBox)
	{
		pCheckBox->SetCheck(m_fAllowCompress);
	}
    

	pCheckBox = (CButton *) GetDlgItem(IDC_OWS_ONE_SHOT);
	if(pCheckBox)
	{
		pCheckBox->SetCheck(!m_fOneShot);
	}
	
	if(waveLoop.ulType == WLOOP_TYPE_FORWARD)
	{
		// Set the forward loop type
		pCheckBox = (CButton *) GetDlgItem(IDC_OWS_LOOP);
		if(pCheckBox)
		{
			pCheckBox->SetCheck(1);
		}
	}
	else
	{
		// Set the forward loop type
		pCheckBox = (CButton *) GetDlgItem(IDC_LOOP_AND_RELEASE_RADIO);
		if(pCheckBox)
		{
			pCheckBox->SetCheck(1);
		}
	}

	EnableControls(TRUE);

	m_fInOnInitialUpdate = false;

	CRegionPropPgMgr::dwLastSelPage = REGION_EXTRA_PROP_PAGE;
	return CPropertyPage::OnSetActive();
}

void CRegionExtraPropPg::OnOwsOneShot() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_fInOnInitialUpdate)
	{
		return;
	}

	ASSERT(m_pRegion);
	
	// Save the undo state 
	if(m_pRegion && m_pRegion->m_pInstrument)
	{
		if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_LOOPSETTINGS)))
			return;
	}

	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{		
		// This is actually a loop check box
		CButton* pRadioBtn = (CButton *) GetDlgItem(IDC_OWS_ONE_SHOT);

		if(pRadioBtn)
		{
			int nState = pRadioBtn->GetCheck();
			
			switch(nState)
			{
				case 0:
					m_fOneShot = true;
					EnableLoopCtrls(false);
					m_pRegion->m_rWSMP.cSampleLoops = 0;
					m_pRegion->m_rWLOOP.ulType = 0;
					m_pRegion->m_rWLOOP.ulStart = 0;
					m_pRegion->m_rWLOOP.ulLength = 0;
					m_pRegion->m_rWLOOP.cbSize = sizeof(m_pRegion->m_rWLOOP);
					break;

				case 1:
					m_fOneShot = false;
					EnableLoopCtrls(true);
					m_pRegion->m_rWSMP.cSampleLoops = 1;
					m_pRegion->m_rWLOOP.ulType = WLOOP_TYPE_FORWARD;
					CButton* pLoopAndReleaseRadio = (CButton*)GetDlgItem(IDC_LOOP_AND_RELEASE_RADIO);
					if(pLoopAndReleaseRadio)
					{
						int nCheck = pLoopAndReleaseRadio->GetState();
						if(nCheck == 1)
						{
							m_pRegion->m_rWLOOP.ulType = WLOOP_TYPE_RELEASE;
						}
					}
					m_pRegion->m_rWLOOP.ulStart = m_dwLoopStart;
					m_pRegion->m_rWLOOP.ulLength = m_dwLoopLength;
					m_pRegion->m_rWLOOP.cbSize = sizeof(m_pRegion->m_rWLOOP);
					break;
			}

			// Set flag so we know to save file 
			m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
		
			m_pRegion->m_pInstrument->UpdateInstrument();
		}
	}
}

void CRegionExtraPropPg::OnOwsAllowCompress() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_fInOnInitialUpdate)
	{
		return;
	}

	ASSERT(m_pRegion);
	
	// Save the undo state 
	if(m_pRegion && m_pRegion->m_pInstrument)
	{
		if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_OVERRIDE_ALLOWCOMPRESSION)))
			return;
	}
	
	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	
	{
		if(m_fAllowCompress)
		{
			m_pRegion->m_rWSMP.fulOptions &= ~F_WSMP_NO_COMPRESSION;
        }
        else
		{		
			m_pRegion->m_rWSMP.fulOptions |= F_WSMP_NO_COMPRESSION;
		}

		// Set flag so we know to save file 
		m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
		
		m_pRegion->m_pInstrument->UpdateInstrument();

	}
}

void CRegionExtraPropPg::OnOwsAllowTruncate() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_fInOnInitialUpdate)
	{
		return;
	}

	ASSERT(m_pRegion);
	
	// Save the undo state 
	if(m_pRegion && m_pRegion->m_pInstrument)
	{
		if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_OVERRIDE_ALLOWTRUNCATION)))
			return;
	}
	
	BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate)
	{
		if(m_fAllowTruncate)
        {
			m_pRegion->m_rWSMP.fulOptions &= ~F_WSMP_NO_TRUNCATION;
		}
		else
		{
			m_pRegion->m_rWSMP.fulOptions |= F_WSMP_NO_TRUNCATION;
		}

		// Set flag so we know to save file 
		m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
		
		m_pRegion->m_pInstrument->UpdateInstrument();

	}
}

void CRegionExtraPropPg::OnOwsLoop() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// Save the undo state 
	if(m_pRegion && m_pRegion->m_pInstrument)
	{
		if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_LOOPSETTINGS)))
			return;
	}

	// This is actually a loop check box
    CButton* pRadioBtn = (CButton *) GetDlgItem(IDC_OWS_ONE_SHOT);

	if(pRadioBtn)
	{
		int nState = pRadioBtn->GetCheck();	
	
		switch(nState)
		{
			case 0:
                m_fOneShot = true;
				EnableLoopCtrls(false);
                m_pRegion->m_rWSMP.cSampleLoops = 0;
				m_pRegion->m_rWLOOP.ulType = 0;
				m_pRegion->m_rWLOOP.ulStart = 0;
				m_pRegion->m_rWLOOP.ulLength = 0;
				m_pRegion->m_rWLOOP.cbSize = sizeof(m_pRegion->m_rWLOOP);
				break;
		
			case 1:
                m_fOneShot = false;
				EnableLoopCtrls(true);
                m_pRegion->m_rWSMP.cSampleLoops = 1;		
				m_pRegion->m_rWLOOP.ulType = WLOOP_TYPE_FORWARD;
				CButton* pLoopAndReleaseRadio = (CButton*)GetDlgItem(IDC_LOOP_AND_RELEASE_RADIO);
				if(pLoopAndReleaseRadio)
				{
					int nCheck = pLoopAndReleaseRadio->GetCheck();
					if(nCheck == 1)
					{
						m_pRegion->m_rWLOOP.ulType = WLOOP_TYPE_RELEASE;
					}
				}
				m_pRegion->m_rWLOOP.ulStart = m_dwLoopStart;
				m_pRegion->m_rWLOOP.ulLength = m_dwLoopLength;
				m_pRegion->m_rWLOOP.cbSize = sizeof(m_pRegion->m_rWLOOP);
				ValidateAndSetLoopValues();
				break;
		}

		// Set flag so we know to save file 
		m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
	
		m_pRegion->m_pInstrument->UpdateInstrument();

	}
}

void CRegionExtraPropPg::OnOwsOverride() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Save the Undo State
	if(m_pRegion && m_pRegion->m_pInstrument)
	{
		if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_OVERRIDE_WAVESETTINGS)))
			return;
	}


    CButton* pCheckBox = (CButton *) GetDlgItem(IDC_OWS_OVERRIDE);

	if(pCheckBox)
	{
		int nState = pCheckBox->GetCheck();
	
		switch(nState)
		{
			case 0:
				if (m_pRegion->m_pWave == NULL)
				{
					pCheckBox->SetCheck(true);
					AfxMessageBox(IDS_CANNOT_CHECK_OVERRIDE, MB_ICONEXCLAMATION);

					return;
				}
				m_fActivateOverride = false;
                RevertToWavePlaybackSettings();
                RevertToWaveAST();
                RevertToWaveASC();
				EnablePlayBack(false);
				EnableLoopCtrls(false);
				EnableAST(false);
                EnableASC(false);
				break;
		
			case 1:
				m_fActivateOverride = true;
				EnableAST(true);
				EnableASC(true);
				EnablePlayBack(true);
                EnableLoopCtrls(!m_fOneShot && true);
				break;
		}

        m_pRegion->m_bOverrideWaveMoreSettings = m_fActivateOverride;

		if(m_pRegion->m_pInstrument && m_pRegion->m_pInstrument->m_pCollection)
			m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
	}
}

void CRegionExtraPropPg::EnableAST(bool bNewState)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    CWnd* pCtrl = GetDlgItem(IDC_OWS_ALLOW_TRUNCATE);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}
}

void CRegionExtraPropPg::EnablePlayBack(bool bNewState)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    CWnd* pCtrl = GetDlgItem(IDC_OWS_STATIC_PLAY_BACK);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}
    
	pCtrl = GetDlgItem(IDC_OWS_ONE_SHOT);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_OWS_LOOP);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}
}

void CRegionExtraPropPg::EnableASC(bool bNewState)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    CWnd* pCtrl = GetDlgItem(IDC_OWS_ALLOW_COMPRESS);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}
}

void CRegionExtraPropPg::EnableLoopCtrls(bool bNewState)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWnd* pCtrl = GetDlgItem(IDC_OWS_LOOP);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_LOOP_AND_RELEASE_RADIO);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}
    
	pCtrl = GetDlgItem(IDC_OWS_LOOPLENGTH);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

    pCtrl = GetDlgItem(IDC_OWS_LOOPSTART);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

    pCtrl = GetDlgItem(IDC_OWS_STATIC_START);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}

	pCtrl = GetDlgItem(IDC_OWS_STATIC_LENGTH);
	if(pCtrl)
	{
		pCtrl->EnableWindow(bNewState);
		pCtrl = NULL;
	}
}

void CRegionExtraPropPg::OnDestroy() 
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

int CRegionExtraPropPg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(m_pRegion == NULL)
		return 0;

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
	if (m_pRegion->m_pWave)
	{

		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();
		WLOOP waveLoop = m_pRegion->m_pWave->GetWLOOP();

		if ( (m_pRegion->m_rWSMP.cSampleLoops != waveWSMPL.cSampleLoops) ||
			 (m_pRegion->m_rWLOOP.ulStart != waveLoop.ulStart) ||
			 (m_pRegion->m_rWLOOP.ulLength != waveLoop.ulLength) ||
			 (m_pRegion->m_rWSMP.fulOptions != waveWSMPL.fulOptions) )
		{
			m_pRegion->m_bOverrideWaveMoreSettings = m_fActivateOverride = true;
			return 0;
		}
		
	}
	return 0;
}

void CRegionExtraPropPg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	/*CMenu* pPopupMenu =  m_pContextMenu->GetSubMenu(0);
	
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON,
							   point.x,
							   point.y,
							   this,
							   NULL);*/
}

void CRegionExtraPropPg::OnRevertToWave() 
{
	// TODO: Add your command handler code here
	
}

// ===============================================================================
// ===============================================================================
void CRegionExtraPropPg::OnKillfocusOwsLoopstart() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if ((m_bfTouched & fLOOP_START) && FAILED(ValidateAndSetLoopValues()))
		PostMessage(DM_VALIDATE,IDC_LOOP_START);
}

// ===============================================================================
// ===============================================================================
void CRegionExtraPropPg::OnKillfocusOwsLooplength() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if ((m_bfTouched & fLOOP_LENGTH) && FAILED(ValidateAndSetLoopValues()))
		PostMessage(DM_VALIDATE,IDC_LOOP_LENGTH);
}

// ==============================================================
// RevertToWaveAST
//  Sets the wave allow sample truncation to the value specified
//  in the wave property.
// ==============================================================
void CRegionExtraPropPg::RevertToWaveAST()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    // Get the wave's finetune.
    ASSERT(m_pRegion);

    if (m_pRegion->m_pWave) 
    {
		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();

		m_pRegion->m_rWSMP.fulOptions = waveWSMPL.fulOptions;
        
    	m_fAllowTruncate = !(m_pRegion->m_rWSMP.fulOptions & F_WSMP_NO_TRUNCATION);

        // update the checkbox.
        CButton * pCheckBox = (CButton *)GetDlgItem(IDC_OWS_ALLOW_TRUNCATE);
        if (pCheckBox)
        {
            pCheckBox->SetCheck(m_fAllowTruncate);
        }
    }
}

// ==============================================================
// RevertToWaveASC
//  Sets the wave allow sample compression to the value specified
//  in the wave property.
// ==============================================================
void CRegionExtraPropPg::RevertToWaveASC()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    // Get the wave's finetune.
    ASSERT(m_pRegion);
    //ASSERT(m_pRegion->m_pWave); // there should always be one.

    if (m_pRegion->m_pWave) 
    {
		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();
		
		m_pRegion->m_rWSMP.fulOptions = waveWSMPL.fulOptions;
        
    	m_fAllowCompress = !(m_pRegion->m_rWSMP.fulOptions & F_WSMP_NO_COMPRESSION);

        // update the checkbox.
        CButton * pCheckBox = (CButton *)GetDlgItem(IDC_OWS_ALLOW_COMPRESS);
        if (pCheckBox)
        {
            pCheckBox->SetCheck(m_fAllowCompress);
        }
    }
}


// ==============================================================
// RevertToWavePlaybackSettings
//  Sets the plaback settings to the settings specified
//  in the wave property.
// ==============================================================
void CRegionExtraPropPg::RevertToWavePlaybackSettings()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    // Get the wave's finetune.
    ASSERT(m_pRegion);
    //ASSERT(m_pRegion->m_pWave); // there should always be one.

    if (m_pRegion->m_pWave) 
    {
		WSMPL waveWSMPL = m_pRegion->m_pWave->GetWSMPL();
		WLOOP waveLoop = m_pRegion->m_pWave->GetWLOOP();

		// Revert for only the allow sample truncation, sample compression and loop options
		// The other options on the first property page have separate checkboxes
		m_pRegion->m_rWSMP.fulOptions = waveWSMPL.fulOptions;
		m_pRegion->m_rWSMP.cSampleLoops = waveWSMPL.cSampleLoops;

		// Check if the loop values in WLOOP are good; else fix them
		if((int)waveLoop.ulStart < 0)
			waveLoop.ulStart = 0;
		if((int)waveLoop.ulLength < MINIMUM_VALID_LOOPLENGTH)
			waveLoop.ulLength = MINIMUM_VALID_LOOPLENGTH;
	
		memcpy(&m_pRegion->m_rWLOOP, &waveLoop, sizeof(m_pRegion->m_rWLOOP));

		// Set the modified loop for the wave
		m_pRegion->m_pWave->SetWLOOP(waveLoop);

        // Set the loop start & length values.
        m_dwLoopStart = m_pRegion->m_rWLOOP.ulStart;
        m_dwLoopLength = m_pRegion->m_rWLOOP.ulLength;

    	char text[BUFFER_64];
    	sprintf(text, "%d", m_dwLoopStart);
        CWnd * pWnd = GetDlgItem(IDC_OWS_LOOPSTART);
        if (pWnd)
        {
            pWnd->SetWindowText(text);
        }

    	sprintf(text, "%d", m_dwLoopLength);
	    pWnd = GetDlgItem(IDC_OWS_LOOPLENGTH);
        if (pWnd)
        {
            pWnd->SetWindowText(text);
        }

        // Set the radio buttons.
        CButton * pBtn = (CButton *)GetDlgItem(IDC_OWS_LOOP);
        if (pBtn)
        {
            pBtn->SetCheck(m_pRegion->m_pWave->IsLooped());
        }

        pBtn = (CButton *)GetDlgItem(IDC_OWS_ONE_SHOT);
        if (pBtn)
        {
            pBtn->SetCheck(!m_pRegion->m_pWave->IsLooped());
        }
    }
    // update the synth
    m_pRegion->m_pInstrument->UpdateInstrument();
}


void CRegionExtraPropPg::OnChangeLoopstart() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if ( m_fInOnInitialUpdate || m_bTouchedByProgram )
		return;
	m_bfTouched = fLOOP_START;
}


void CRegionExtraPropPg::OnChangeLooplength() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if ( m_fInOnInitialUpdate || m_bTouchedByProgram )
		return;
	m_bfTouched = fLOOP_LENGTH;
}

HRESULT CRegionExtraPropPg::ValidateAndSetLoopValues()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pRegion);
	if(m_pRegion == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pRegion->m_pInstrument);
	if(m_pRegion->m_pInstrument == NULL)
	{
		return E_UNEXPECTED;
	}

	HRESULT hr = S_OK;

	CString sLoopValue;

	m_LoopStartEdit.GetWindowText(sLoopValue);
	DWORD dwLoopStart = (DWORD)_ttol(sLoopValue);

	m_LoopLengthEdit.GetWindowText(sLoopValue);
	DWORD dwLoopLength = (DWORD)_ttol(sLoopValue);

	// validate loop values
	hr = m_pRegion->m_pWave->HrValidateLoopValues(dwLoopStart, dwLoopLength, false);

	// Now set the actual values
	// ONLY if they're different...
	if(SUCCEEDED(hr) && (m_dwLoopStart != dwLoopStart || m_dwLoopLength != dwLoopLength))
	{
		if(m_pRegion && m_pRegion->m_pInstrument)
		{
			if(FAILED(m_pRegion->m_pInstrument->SaveStateForUndo(IDS_UNDO_LOOPPOINTS)))
			{
				hr = E_FAIL;
			}
		}
	}

	if(SUCCEEDED(hr))
	{
		m_dwLoopStart = dwLoopStart;
		m_pRegion->m_rWLOOP.ulStart = m_dwLoopStart;

		m_dwLoopLength = dwLoopLength;
		m_pRegion->m_rWLOOP.ulLength = dwLoopLength;

		UpdateData(TRUE);

		if(m_pRegion->m_pInstrument)
		{
			m_pRegion->m_pInstrument->UpdateInstrument();
			m_pRegion->m_pInstrument->m_pCollection->SetDirtyFlag();
		}
	}

	return hr;
}


LRESULT CRegionExtraPropPg::OnValidate(UINT wParam, long lParam)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	PostValidate(wParam);
	return 0;
}

void CRegionExtraPropPg::PostValidate(UINT	nControl)
{
	m_pPropMgr->ShowPropSheet();
	CWnd * pWnd = GetDlgItem(nControl);
	if (IsWindow(pWnd->GetSafeHwnd()))
    {
        pWnd->SetFocus();
        ((CEdit *)pWnd)->SetSel(0, -1);
    }
}
