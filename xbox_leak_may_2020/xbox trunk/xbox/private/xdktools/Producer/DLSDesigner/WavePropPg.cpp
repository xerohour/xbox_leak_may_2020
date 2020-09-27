// WavePropPg.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "WavePropPg.h"
#include "WaveNode.h"
#include "Wave.h"
#include "monoWave.h"
#include "Collection.h"
#include "instrument.h"
#include "region.h"
#include "WavePropPgMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BUFFER_64 64

class CRegion;
extern BOOL isValidNoteString(char * psz);
 
/////////////////////////////////////////////////////////////////////////////
// CWavePropPg property page

IMPLEMENT_DYNCREATE(CWavePropPg, CPropertyPage)

CWavePropPg::CWavePropPg() :
CPropertyPage(CWavePropPg::IDD),
CSliderCollection(2),
m_pWave(NULL),
m_pPropMgr(NULL),
m_fOneShot(true),  
m_fActivateLoop(false),   //TODO: this must be !m_fOneShot. Keep one variable.
m_fInitialUpdate(false),
m_bfTouched(0),
m_fNeedToDetach(FALSE),
m_bTouchedByProgram(false),
m_dwLoopStart(MINIMUM_VALID_LOOPSTART),
m_dwLoopLength(0),
m_bLockLoopLength(false),
m_bLockSelectionLength(false),
m_nSelectionStart(0),
m_nSelectionEnd(0),
m_bInLoopUpdate(false),
m_pmsFineTune(NULL)
{
	//{{AFX_DATA_INIT(CWavePropPg)
	m_fAllowCompression = FALSE;
	m_fAllowTruncation = FALSE;
	m_dwLength = 0;
	//}}AFX_DATA_INIT
	CSliderCollection::Init(this);
}

CWavePropPg::~CWavePropPg()
{
}

void CWavePropPg::SetObject(CWave* pWave)
{
	m_pWave = pWave; 
	
	if(pWave == NULL) // huh?
		return;

	m_fOneShot = (m_pWave->IsLooped() == false);
    m_fActivateLoop = !m_fOneShot;

	UpdateData(FALSE);
}


BOOL CWavePropPg::OnInitDialog()
{
	BOOL retval;

	m_bTouchedByProgram = true;
	retval = CDialog::OnInitDialog();
	m_bTouchedByProgram = false;

	m_LoopLengthSpin.SetRange(UD_MINVAL, UD_MAXVAL);
	m_LoopStartSpin.SetRange(UD_MINVAL, UD_MAXVAL);
	m_LoopEndSpin.SetRange(UD_MINVAL, UD_MAXVAL);
	m_SelectionStartSpin.SetRange(UD_MINVAL, UD_MAXVAL);
	m_SelectionLengthSpin.SetRange(UD_MINVAL, UD_MAXVAL);
	m_SelectionEndSpin.SetRange(UD_MINVAL, UD_MAXVAL);


	if(m_pWave && m_pWave->GetWaveEditor())
	{
		EnableSelectionControls(true);
		EnableControl(IDC_SELECTION_LENGTH_LOCK_CHECK, true);
	}
	else
	{
		EnableSelectionControls(false);
		EnableControl(IDC_SELECTION_LENGTH_LOCK_CHECK, false);
	}
	SetSelectionLengthLock(m_bLockSelectionLength);

	// create and hook up the sliders
	CSliderCollection::Free(); // refresh sliders after a page switch

	m_pmsFineTune = Insert(
		IDC_FINE_TUNE, //slider
		IDC_SEMI_TONES,    //edit box
		IDC_SEMI_TONES_SPIN,
		MYSLIDER_PITCHCENTS,
		0,
		IDS_UNDO_FINETUNE,
		&m_lFineTune);
    
	m_pmsAttenuation = Insert(
		IDC_ATTENUATION,   //slider
		IDC_DB_ATTENUATION,  //editbox
		IDC_DB_ATTENUATION_SPIN,
		MYSLIDER_VOLUME,
		0,
		IDS_UNDO_ATTENUATION,
		&m_lAttenuation);
	
	// Initialize the values.
	InitializeDialogValues();
	
	return retval;
}
/////////////////////////////////////////////////////////////////////////////
// CCollectionPropPg::EnableControls

void CWavePropPg::EnableControls() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    EnableLoopControls(m_fActivateLoop);
}

void CWavePropPg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWavePropPg)
	DDX_Control(pDX, IDC_SELECTION_START_SPIN, m_SelectionStartSpin);
	DDX_Control(pDX, IDC_SELECTION_LENGTH_SPIN, m_SelectionLengthSpin);
	DDX_Control(pDX, IDC_SELECTION_LENGTH_LOCK_CHECK, m_SelectionLengthLockCheck);
	DDX_Control(pDX, IDC_SELECTION_END_SPIN, m_SelectionEndSpin);
	DDX_Control(pDX, IDC_LOOP_START_SPIN, m_LoopStartSpin);
	DDX_Control(pDX, IDC_LOOP_LENGTH_SPIN, m_LoopLengthSpin);
	DDX_Control(pDX, IDC_LOOP_LENGTH_LOCK_CHECK, m_LoopLengthLockCheck);
	DDX_Control(pDX, IDC_LOOP_CHECK, m_LoopCheck);
	DDX_Control(pDX, IDC_LOOP_END_SPIN, m_LoopEndSpin);
	DDX_Control(pDX, IDC_LOOP_END, m_LoopEndEdit);
	DDX_Control(pDX, IDC_LOOP_START, m_LoopStartEdit);
	DDX_Control(pDX, IDC_LOOP_LENGTH, m_LoopLengthEdit);
	DDX_Control(pDX, IDC_ROOT_NOTE, m_RootNoteEdit);
	DDX_Control(pDX, IDC_ROOT_NOTE_SPIN, m_RootNoteSpin);
	DDX_Check(pDX, IDC_SAMPLE_COMPRESSION, m_fAllowCompression);
	DDX_Check(pDX, IDC_SAMPLE_TRUNCATION, m_fAllowTruncation);
	DDX_Text(pDX, IDC_LENGTH, m_dwLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWavePropPg, CPropertyPage)
	//{{AFX_MSG_MAP(CWavePropPg)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_ROOT_NOTE, OnKillfocusRootNote)
	ON_EN_KILLFOCUS(IDC_LOOP_LENGTH, OnKillfocusLoopLength)
	ON_EN_KILLFOCUS(IDC_LOOP_START, OnKillfocusLoopStart)
	ON_BN_CLICKED(IDC_SAMPLE_TRUNCATION, OnSampleTruncation)
	ON_BN_CLICKED(IDC_SAMPLE_COMPRESSION, OnSampleCompression)
	ON_MESSAGE(DM_VALIDATE,OnValidate)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ROOT_NOTE_SPIN, OnDeltaposRootNoteSpin)
	ON_BN_CLICKED(IDC_LOOP_CHECK, OnLoopCheck)
	ON_EN_KILLFOCUS(IDC_LOOP_END, OnKillfocusLoopEnd)
	ON_NOTIFY(UDN_DELTAPOS, IDC_LOOP_END_SPIN, OnDeltaposLoopEndSpin)
	ON_BN_CLICKED(IDC_LOOP_LENGTH_LOCK_CHECK, OnLoopLengthLockCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_LOOP_LENGTH_SPIN, OnDeltaposLoopLengthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_LOOP_START_SPIN, OnDeltaposLoopStartSpin)
	ON_EN_KILLFOCUS(IDC_SELECTION_END, OnKillfocusSelectionEnd)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SELECTION_END_SPIN, OnDeltaposSelectionEndSpin)
	ON_EN_KILLFOCUS(IDC_SELECTION_LENGTH, OnKillfocusSelectionLength)
	ON_BN_CLICKED(IDC_SELECTION_LENGTH_LOCK_CHECK, OnSelectionLengthLockCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SELECTION_LENGTH_SPIN, OnDeltaposSelectionLengthSpin)
	ON_EN_KILLFOCUS(IDC_SELECTION_START, OnKillfocusSelectionStart)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SELECTION_START_SPIN, OnDeltaposSelectionStartSpin)
	ON_BN_CLICKED(IDC_FORWARD_LOOP_RADIO, OnForwardLoopRadio)
	ON_BN_CLICKED(IDC_LOOP_AND_RELEASE_RADIO, OnLoopAndReleaseRadio)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(EN_CHANGE, IDC_LOOP_START, IDC_ROOT_NOTE, OnEditChange)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWavePropPg message handlers

void CWavePropPg::OnEditChange( UINT nEditControl )
{
	if ( m_bTouchedByProgram )
		return;
	switch ( nEditControl )
	{
		case IDC_LOOP_START:
			m_bfTouched = fLOOP_START;
			break;
		case IDC_LOOP_LENGTH:
			m_bfTouched = fLOOP_LENGTH;
			break;
		case IDC_ROOT_NOTE:
			m_bfTouched |= fROOT_NOTE;
			break;
		default:
			break;
	}
}

BOOL CWavePropPg::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	bool bSetEditorAndSnap = true;
		   
    if (m_pWave)
    {
		CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
        InitializeDialogValues();
		if(pWaveEditor)
		{
			pWaveEditor->GetSelection(m_nSelectionStart, m_nSelectionEnd);
			bSetEditorAndSnap = false;
		}

        EnableControls();
    }
	
	if(m_pWave == NULL || (m_pWave && m_pWave->GetWaveEditor() == NULL))
	{
		EnableSelectionControls(false);
		EnableControl(IDC_SELECTION_LENGTH_LOCK_CHECK, false);
	}
	else
	{
		EnableSelectionControls(true);
		EnableControl(IDC_SELECTION_LENGTH_LOCK_CHECK, true);
	}
	
	SetSelectionLengthLock(m_bLockSelectionLength);
	SetSelection(m_nSelectionStart, m_nSelectionEnd, bSetEditorAndSnap, bSetEditorAndSnap);

	CWavePropPgMgr::dwLastSelPage = WAVE_PROP_PAGE;

	return CPropertyPage::OnSetActive();
}

int CWavePropPg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CWavePropPg::OnDestroy() 
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


void CWavePropPg::OnLoopCheck() 
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	// Can't set a loop for a wave too short
	if(m_pWave->m_dwWaveLength < MINIMUM_VALID_LOOPLENGTH)
	{
		m_LoopCheck.SetCheck(0);
		EnableLoopTypeButtons(FALSE);
		return;
	}

	WSMPL waveWSMPL = m_pWave->GetWSMPL();

	CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();

	// Check if the values in the edits are legal...
#if MINIMUM_VALID_LOOPSTART > 0
	if(m_dwLoopStart < MINIMUM_VALID_LOOPSTART || m_dwLoopLength < MINIMUM_VALID_LOOPLENGTH)
#else
	if(m_dwLoopLength < MINIMUM_VALID_LOOPLENGTH)
#endif
	{
		OnKillfocusLoopStart();
	}
	// Save the state for undo/redo
	else if(pWaveEditor)
	{
		if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_LOOPSETTINGS)))
		{
			// Out Of Memory??
			CButton* pRadioBtn = (CButton*) GetDlgItem(IDC_LOOP_CHECK);
			if(pRadioBtn)
			{
				int nState = pRadioBtn->GetCheck();	
				if(nState == 0)
					m_LoopCheck.SetCheck(1);
				else
					m_LoopCheck.SetCheck(0);
			}
			return;
		}
	}

    CButton* pRadioBtn = (CButton*) GetDlgItem(IDC_LOOP_CHECK);
	if(pRadioBtn)
	{
		int nState = pRadioBtn->GetCheck();	
	
		switch(nState)
		{
			case 0:
			{
				m_fActivateLoop = false;
				EnableLoopControls(false);
                waveWSMPL.cSampleLoops = 0;
                m_pWave->SetLooped(false);
				break;
			}
		
			case 1:
			{
				m_fActivateLoop = true;
				EnableLoopControls(true);
                waveWSMPL.cSampleLoops = 1;		
                m_pWave->SetLooped(true);
				break;
			}
		}

        m_fOneShot = !m_fActivateLoop;

		m_pWave->SetWSMPL(waveWSMPL);

        //m_pWave->CopyWSMPToSMPL();
		m_pWave->UpdateWave();
		m_pWave->UpdateDummyInstrument();

		UpdatePlaybackSettingsInRegions();

		if(pWaveEditor)
		{
			pWaveEditor->InvalidateRect(NULL);
		}

	    // notify of change and set dirty flag
		m_pWave->NotifyWaveChange(true);
	}	
}

// ========================================================================
// ========================================================================

void CWavePropPg::EnableLoopControls(BOOL bEnable)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    EnableControl(IDC_LOOP_START, bEnable);
	EnableControl(IDC_LOOP_START_SPIN, bEnable);
	EnableControl(IDC_LOOP_END, bEnable);
	EnableControl(IDC_LOOP_END_SPIN, bEnable);

	if(bEnable)
	{
		EnableControl(IDC_LOOP_LENGTH, !m_bLockLoopLength);
		EnableControl(IDC_LOOP_LENGTH_SPIN, !m_bLockLoopLength);
	}
	else
	{
		EnableControl(IDC_LOOP_LENGTH, bEnable);
		EnableControl(IDC_LOOP_LENGTH_SPIN, bEnable);
	}

	EnableControl(IDC_LOOP_LENGTH_LOCK_CHECK, bEnable);
	EnableLoopTypeButtons(bEnable);
}


void CWavePropPg::EnableSelectionControls(BOOL bEnable)
{
	EnableControl(IDC_SELECTION_START, bEnable);
	EnableControl(IDC_SELECTION_START_SPIN, bEnable);
	EnableControl(IDC_SELECTION_END, bEnable);
	EnableControl(IDC_SELECTION_END_SPIN, bEnable);

	if(bEnable)
	{
		EnableControl(IDC_SELECTION_LENGTH, !m_bLockSelectionLength);
		EnableControl(IDC_SELECTION_LENGTH_SPIN, !m_bLockSelectionLength);
	}
	else
	{
		EnableControl(IDC_SELECTION_LENGTH, bEnable);
		EnableControl(IDC_SELECTION_LENGTH_SPIN, bEnable);
	}

	EnableControl(IDC_SELECTION_LENGTH_LOCK_CHECK, bEnable);
}


void CWavePropPg::EnableControl(UINT ID, BOOL bEnable)
{
	CWnd* pCtrl = GetDlgItem(ID);
	if(pCtrl)
		pCtrl->EnableWindow(bEnable);
}


void CWavePropPg::InitializeDialogValues()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_fInitialUpdate = true;

	m_pmsFineTune->SetValue(this, m_lFineTune);
	m_pmsAttenuation->SetValue(this, m_lAttenuation);
	
    if (m_pWave)
    {
		WSMPL waveWSMPL =  m_pWave->GetWSMPL();

	    m_lFineTune = (long) waveWSMPL.sFineTune << 16;
	    m_lAttenuation = (long) waveWSMPL.lAttenuation;
        
        // set the sliders
	    m_pmsFineTune->SetValue(this, m_lFineTune);
	    m_pmsAttenuation->SetValue(this, m_lAttenuation);

		// Sample length
		m_dwLength = m_pWave->m_dwWaveLength;
		CWnd *pCtrl = GetDlgItem(IDC_LENGTH);    
		CString sText;
		sText.Format("%d", m_dwLength);
	    if(pCtrl)
	    {
		    pCtrl->SetWindowText(sText);
			pCtrl = NULL;
	    }

        //Now playback.
		WLOOP waveLoop = m_pWave->GetWLOOP();

#if MINIMUM_VALID_LOOPSTART > 0
		// Can't have a loop start at sample 0 can we?
		if(waveLoop.ulStart >= MINIMUM_VALID_LOOPSTART)
		{
			m_dwLoopStart = waveLoop.ulStart;
		}
		else
		{
			waveLoop.ulStart = m_dwLoopStart;
		}
#else
		m_dwLoopStart = waveLoop.ulStart;
#endif
		
		// Can't have a loop length less than MINIMUM_VALID_LOOPLENGTH can we?
		if(waveLoop.ulLength >= MINIMUM_VALID_LOOPLENGTH)
		{
			m_dwLoopLength = waveLoop.ulLength;
		}
		else if(m_pWave->m_dwWaveLength >= MINIMUM_VALID_LOOPLENGTH)
		{
			waveLoop.ulLength = m_pWave->m_dwWaveLength;
		}
		else
		{
			waveLoop.ulLength = MINIMUM_VALID_LOOPLENGTH;
		}

		if(waveLoop.ulLength >= MSSYNTH_LOOP_LIMIT && m_pWave->IsLooped() == false)
		{
			waveLoop.ulLength = MSSYNTH_LOOP_LIMIT - 1;
			m_dwLoopLength = waveLoop.ulLength;
		}


        m_fAllowTruncation = !(waveWSMPL.fulOptions & F_WSMP_NO_TRUNCATION);
	    m_fAllowCompression = !(waveWSMPL.fulOptions & F_WSMP_NO_COMPRESSION);
        m_fOneShot = (m_pWave->IsLooped() == false);
        m_fActivateLoop = !m_fOneShot;

		// Set the loop type button states
		SetWaveLoopType(waveLoop.ulType);

		// Update the modified wave WSMPL and WLOOP values
		m_pWave->SetWSMPL(waveWSMPL);
		m_pWave->SetWLOOP(waveLoop);

        // set root note data
		ASSERT(m_RootNoteSpin.GetSafeHwnd());
        m_RootNoteSpin.SetRange(0, 127);
		m_RootNoteSpin.SetPos((long) waveWSMPL.usUnityNote);
		ASSERT(m_RootNoteEdit.GetSafeHwnd());
		UpdateRootNoteText(waveWSMPL.usUnityNote);

        //set text in the edit boxes.
	    char text[BUFFER_64];
	    sprintf(text, "%d", m_dwLoopLength);
		m_bTouchedByProgram = true;
		m_LoopLengthEdit.SetWindowText(text);
		m_bTouchedByProgram = false;

        sprintf(text, "%d", m_dwLoopStart);
		m_bTouchedByProgram = true;
		m_LoopStartEdit.SetWindowText(text);
		m_bTouchedByProgram = false;

		sprintf(text, "%d", (m_dwLoopStart + m_dwLoopLength - 1));
		m_bTouchedByProgram = true;
		m_LoopEndEdit.SetWindowText(text);
		m_bTouchedByProgram = false;


        // set the checkboxes
        CButton* pBtn = (CButton *) GetDlgItem(IDC_ONE_SHOT);
	    if(pBtn)
	    {
		    pBtn->SetCheck(m_fOneShot);
	    }

	    pBtn = (CButton *) GetDlgItem(IDC_LOOP_CHECK);
	    if(pBtn)
	    {
		    pBtn->SetCheck(!m_fOneShot);
	    }
		EnableLoopControls(!m_fOneShot);

        //Now the compression and truncation checkboxes.
	    pBtn = (CButton *) GetDlgItem(IDC_SAMPLE_TRUNCATION);
	    if(pBtn)
	    {
		    pBtn->SetCheck(m_fAllowTruncation);
	    }

	    pBtn = (CButton *) GetDlgItem(IDC_SAMPLE_COMPRESSION);
	    if(pBtn)
	    {
		    pBtn->SetCheck(m_fAllowCompression);
	    }

    } //if m_pWave
	m_fInitialUpdate = false;
}

void CWavePropPg::UpdateDbAttenuation() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_fInitialUpdate) 
	{
		return;
	}

	BOOL bUpdate = FALSE;
	bUpdate = UpdateData(TRUE);	
	
    if(bUpdate && m_pWave)
    {
		WSMPL waveWSMPL =  m_pWave->GetWSMPL();
		if (waveWSMPL.lAttenuation != m_lAttenuation)
        {
			CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
			if(pWaveEditor)
			{
				if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_ATTENUATION)))
				{
					// Out Of Memory??
					m_lAttenuation = (long) waveWSMPL.lAttenuation;
					m_pmsAttenuation->SetValue(this, m_lAttenuation);
					UpdateData(FALSE);
					return;
				}
			}
		}
		else
			return;

        waveWSMPL.lAttenuation = m_lAttenuation;

		// Update the wave SMPL values
		m_pWave->SetWSMPL(waveWSMPL);
        m_pWave->CopyWSMPToSMPL();
            
		m_pWave->UpdateWave();
		// Change the values in all the regions that refer to this wave 
        // and have not selected to override wave properties.
        UpdateAttenuationInRegions();

		m_pWave->UpdateDummyInstrument();
		m_pWave->NotifyWaveChange(true);
    }
}

void CWavePropPg::UpdateFineTune() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);


	if ( m_fInitialUpdate ) 
		return;

    BOOL bUpdate;
	bUpdate = UpdateData(TRUE);	

	if(bUpdate && m_pWave)
	{		
		WSMPL waveWSMPL =  m_pWave->GetWSMPL();
		if(waveWSMPL.sFineTune != (WORD) (m_lFineTune >> 16))
		{
			CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
			if(pWaveEditor)
			{
				if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_FINETUNE)))
				{
					// Out Of Memory??
					m_lFineTune = (long) waveWSMPL.sFineTune << 16;
					m_pmsFineTune->SetValue(this, m_lFineTune);
					UpdateData(FALSE);
					return;
				}
			}
		}
		else
			return;

		waveWSMPL.sFineTune = (WORD) (m_lFineTune >> 16);

		// Update the wave WSMPL values
		m_pWave->SetWSMPL(waveWSMPL);
        m_pWave->CopyWSMPToSMPL();

		m_pWave->UpdateWave();

        // Change the values in all the regions that refer to this wave 
        // and have not selected to override wave properties.
        UpdateFineTuneInRegions();

		m_pWave->UpdateDummyInstrument();
		m_pWave->NotifyWaveChange(true);
	}    	
}

void CWavePropPg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CSliderCollection::OnHScroll(nSBCode, nPos, pScrollBar);
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CWavePropPg::OnValidate(UINT wParam, long lParam)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	PostValidate(wParam);
	return 0;
}

void CWavePropPg::OnKillfocusRootNote() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pWave == NULL)
		return;

	USHORT usNewNote = GetNoteInEditBox();
	WSMPL waveWSMPL = m_pWave->GetWSMPL();
	if(FAILED(UpdateRootNote(usNewNote)))
	{
		UpdateRootNoteText(waveWSMPL.usUnityNote);
	}

	m_bfTouched &= ~fROOT_NOTE;
}

HRESULT CWavePropPg::UpdateRootNote(USHORT usNewNote)
{
	if (!m_pWave) 
		return E_FAIL;

	WSMPL waveWSMPL =  m_pWave->GetWSMPL();

	// update the wave if the note's different
	if (usNewNote != waveWSMPL.usUnityNote) 
	{
		CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
		// Save the state for undo/redo
		if(pWaveEditor)
		{
			if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_ROOTNOTE)))
			{
				// Out of Memory??
				return E_FAIL;	
			}
		}

		waveWSMPL.usUnityNote = usNewNote;
		m_pWave->SetWSMPL(waveWSMPL);
		m_pWave->CopyWSMPToSMPL();
		m_pWave->UpdateWave();
		// update the regions that use this wave.
		UpdateRootNoteInRegions();
		m_pWave->UpdateDummyInstrument();
		m_pWave->NotifyWaveChange(true);
	}

	return S_OK;
}

void CWavePropPg::UpdateRootNoteText(int note)
{
	ASSERT(note >= 0 && note <= 127);
	char pszNote[20];
	notetostring((DWORD)note, pszNote);
	m_RootNoteEdit.SetWindowText(pszNote);
}

// get the note typed by the user in the edit box, if valid. if not valid,
// returns the current unity note for the wave.
USHORT CWavePropPg::GetNoteInEditBox()
{
	ASSERT(m_pWave);
	
	WSMPL waveWSMPL = m_pWave->GetWSMPL();

	USHORT usNote;
	CString strNote;
	m_RootNoteEdit.GetWindowText(strNote);
	LPSTR lpsBuf = strNote.GetBuffer(3);
	if (isValidNoteString(lpsBuf)) 
	{
		usNote = stringtonote(lpsBuf);
	}
	else 
	{
		usNote = waveWSMPL.usUnityNote;
	}
	
	// reset the note in the edit box
	char szNote[20];
	notetostring(usNote, szNote);
	m_bTouchedByProgram = true;
	m_RootNoteEdit.SetWindowText(szNote);
	m_bTouchedByProgram = false;
	
	
	return usNote;
}

HRESULT CWavePropPg::ValidateAndSetLoopValues(DWORD dwLoopStart, DWORD dwLoopLength, bool bSaveUndoState)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = S_OK;

	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return E_FAIL;

	WLOOP waveLoop = m_pWave->GetWLOOP();
	CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
	CWnd* pWndWithFocus = GetFocus();

	if(m_bInLoopUpdate)
	{
		TRACE("\nBusy doing wave loop update!\n");
		return E_FAIL;
	}

	BOOL bWaveWasPlaying = m_pWave->WaveIsPlaying();

	m_bInLoopUpdate = true;
	TRACE("\nSetting busy signal for wave loop update!\n");

	// validate loop values
	hr = m_pWave->HrValidateLoopValues(dwLoopStart, dwLoopLength, m_bLockLoopLength);

	// Set the actual values only if they're different
	if(SUCCEEDED(hr) && (m_dwLoopStart != dwLoopStart || m_dwLoopLength != dwLoopLength))
	{
		// Save the undo state first
		if(pWaveEditor && bSaveUndoState)
		{
			if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_LOOPPOINTS)))
			{
				// Out Of Memory??
				hr = E_FAIL;
			}
		}
	}

	// Set the values now
	if(SUCCEEDED(hr))
	{
		m_dwLoopStart = dwLoopStart;
		waveLoop.ulStart = m_dwLoopStart;

		m_dwLoopLength = dwLoopLength;
		waveLoop.ulLength = dwLoopLength;
	}

	CString sWindowText;
	sWindowText.Format("%d", m_dwLoopStart);
	m_LoopStartEdit.SetWindowText(sWindowText);

	sWindowText.Format("%d", (m_dwLoopStart + m_dwLoopLength - 1));
	m_LoopEndEdit.SetWindowText(sWindowText);
	
	sWindowText.Format("%d", m_dwLoopLength);
	m_LoopLengthEdit.SetWindowText(sWindowText);

	if(FAILED(hr))
	{
		m_bInLoopUpdate = false;
		UpdateData(FALSE);
		return hr;
	}

	UpdateData(TRUE);

	m_pWave->SetWLOOP(waveLoop);

	// Copy the loop information to the legacy structure
	m_pWave->CopyWSMPToSMPL();

	// the compressed loop also gets modified
	m_pWave->RememberLoopAfterCompression();

	// Update the wave in all the regions that reference it
	m_pWave->UpdateWave();
	m_pWave->UpdateLoopStartAndLengthInRegions();
	m_pWave->UpdateDummyInstrument();
	
	// notify of change and set dirty flag
	m_pWave->NotifyWaveChange(true);

	if (pWaveEditor)
		pWaveEditor->InvalidateRect(NULL);

	m_bInLoopUpdate = false;
	
	if(pWndWithFocus)
	{
		pWndWithFocus->SetFocus();
	}

	if(bWaveWasPlaying)
	{
        CWaveNode* pWaveNode = m_pWave->GetNode();
        ASSERT(pWaveNode);
        if(pWaveNode)
        {
            pWaveNode->Stop(TRUE);
		    pWaveNode->Play(FALSE);
        }
	}

	return hr;
}



// ========================================================================
// ========================================================================
void CWavePropPg::OnSampleTruncation() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);

    if (m_pWave)
    {
		WSMPL waveWSMPL = m_pWave->GetWSMPL();

	    BOOL bUpdate = false;
	    bUpdate = UpdateData(TRUE);	

	    if(bUpdate)
	    {
			CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
			// Save the state for undo/redo
			if(pWaveEditor)
			{
				if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_OVERRIDE_ALLOWTRUNCATION)))
				{
					// Couldn't save the undo state!! Out Of Memory??
					m_fAllowTruncation = !m_fAllowTruncation;
					UpdateData(FALSE);
					return;
				}
			}


		    if(m_fAllowTruncation)
            {
			    waveWSMPL.fulOptions &= ~F_WSMP_NO_TRUNCATION;
		    }
		    else
		    {
			    waveWSMPL.fulOptions |= F_WSMP_NO_TRUNCATION;
		    }
			
			m_pWave->SetWSMPL(waveWSMPL);

            // update regions that use this wave
			UpdateSampleTruncationInRegions();

			// notify of change and set dirty flag
			m_pWave->NotifyWaveChange(true);
	    }
    }
}

// ========================================================================
// ========================================================================
void CWavePropPg::OnSampleCompression() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (m_pWave)
    {
		WSMPL waveWSMPL = m_pWave->GetWSMPL();

	    BOOL bUpdate = false;
	    bUpdate = UpdateData(TRUE);	
		if(bUpdate)
	    {
			CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
			// Save the state for undo/redo
			if(pWaveEditor)
			{
				if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_OVERRIDE_ALLOWCOMPRESSION)))
				{
					// Couldn't save the undo state!! Out Of Memory??
					m_fAllowCompression = !m_fAllowCompression;
					UpdateData(FALSE);
					return;
				}
			}

		    if(m_fAllowCompression)
		    {
			    waveWSMPL.fulOptions &= ~F_WSMP_NO_COMPRESSION;
            }
            else
		    {		
			    waveWSMPL.fulOptions |= F_WSMP_NO_COMPRESSION;
		    }

			m_pWave->SetWSMPL(waveWSMPL);

            // update regions that use this wave
            UpdateSampleCompressionInRegions();

		    // notify of change and set dirty flag
			m_pWave->NotifyWaveChange(true);
	    }
    }	
}


// ========================================================================
// UpdateAttenuationInRegions
// ========================================================================
void CWavePropPg::UpdateAttenuationInRegions()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    
	// Go thru all regions and chenge values in the regions that
    // refer to the wave being edited if the override flag is not set.
	CCollection* pCollection = m_pWave->GetCollection();
	if(pCollection == NULL)
	{
		return;
	}

	WSMPL waveWSMPL = m_pWave->GetWSMPL();

    CInstrument * pInstrument = pCollection->m_Instruments.GetHead();	
    while (pInstrument) // for all instruments go thru all regions
    {

        CRegion * pRegion = pInstrument->m_Regions.GetHead();
        while (pRegion)
        {
			// Do we refer to the same WaveNode?
            if (pRegion->GetWaveNode() == m_pWave->GetNode())
            {
                // is override attenuation set?
                if (!pRegion->m_bOverrideWaveAttenuation)
                {
                    pRegion->m_rWSMP.lAttenuation = waveWSMPL.lAttenuation;
                    // updatesynth. 
                    ASSERT(pRegion->m_pInstrument);
                    ASSERT(pRegion->m_pInstrument->m_pCollection);
        			pRegion->m_pInstrument->UpdateInstrument();
					pRegion->m_pInstrument->RefreshRegion();
                }
            }

            pRegion = pRegion->GetNext();

        } // while pregion

        pInstrument = pInstrument->GetNext();
    }
}

// ========================================================================
// UpdateFineTuneInRegions
// ========================================================================
void CWavePropPg::UpdateFineTuneInRegions()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    // Go thru all regions and chenge values in the regions that
    // refer to the wave being edited if the override flag is not set.
	CCollection* pCollection = m_pWave->GetCollection();
	if(!pCollection)
	{
		return;
	}
    
	CInstrument * pInstrument = pCollection->m_Instruments.GetHead();

	WSMPL waveWSMPL = m_pWave->GetWSMPL();

    while (pInstrument) // for all instruments go thru all regions
    {
        CRegion * pRegion = pInstrument->m_Regions.GetHead();
        while (pRegion)
        {
            if (pRegion->GetWaveNode() == m_pWave->GetNode())
            {
                // is override attenuation set?
                if (!pRegion->m_bOverrideWaveFineTune)
                {
                    pRegion->m_rWSMP.sFineTune = waveWSMPL.sFineTune;
                    // updatesynth. 
                    ASSERT(pRegion->m_pInstrument);
                    ASSERT(pRegion->m_pInstrument->m_pCollection);
        			pRegion->m_pInstrument->UpdateInstrument();
                }
            }

            pRegion = pRegion->GetNext();

        } // while pregion

        pInstrument = pInstrument->GetNext();
    }
}

// ========================================================================
// UpdateSampleTruncationInRegions
// ========================================================================
void CWavePropPg::UpdateSampleTruncationInRegions()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}
    
	// Go thru all regions and chenge values in the regions that
    // refer to the wave being edited if the override flag is not set.
	CCollection* pCollection = m_pWave->GetCollection();
	if(!pCollection)
	{
		return;
	}

	WSMPL waveWSMPL = m_pWave->GetWSMPL();

    CInstrument * pInstrument = pCollection->m_Instruments.GetHead();

    while (pInstrument) // for all instruments go thru all regions
    {

        CRegion * pRegion = pInstrument->m_Regions.GetHead();
        while (pRegion)
        {
            if (pRegion->GetWaveNode() == m_pWave->GetNode())
            {
                // is override attenuation set?
                if (!pRegion->m_bOverrideWaveMoreSettings)
                {
                    pRegion->m_rWSMP.fulOptions = waveWSMPL.fulOptions;
                    // updatesynth. 
                    ASSERT(pRegion->m_pInstrument);
                    ASSERT(pRegion->m_pInstrument->m_pCollection);
        			pRegion->m_pInstrument->UpdateInstrument();
                }
            }

            pRegion = pRegion->GetNext();

        } // while pregion

        pInstrument = pInstrument->GetNext();
    }
}

// ========================================================================
// UpdateSampleCompressionInRegions
// ========================================================================
void CWavePropPg::UpdateSampleCompressionInRegions()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(!m_pWave)
	{
		return;
	}

    // Go thru all regions and chenge values in the regions that
    // refer to the wave being edited if the override flag is not set.
	CCollection* pCollection = m_pWave->GetCollection();
	if(!pCollection)
	{
		return;
	}

	WSMPL waveWSMPL = m_pWave->GetWSMPL();

    CInstrument* pInstrument = pCollection->m_Instruments.GetHead();

    while (pInstrument) // for all instruments go thru all regions
    {

        CRegion * pRegion = pInstrument->m_Regions.GetHead();
        while (pRegion)
        {
            if (pRegion->GetWaveNode() == m_pWave->GetNode())
            {
                // is override attenuation set?
                if (!pRegion->m_bOverrideWaveMoreSettings)
                {
                    pRegion->m_rWSMP.fulOptions = waveWSMPL.fulOptions;
                    // updatesynth. 
                    ASSERT(pRegion->m_pInstrument);
                    ASSERT(pRegion->m_pInstrument->m_pCollection);
        			pRegion->m_pInstrument->UpdateInstrument();
                }
            }

            pRegion = pRegion->GetNext();

        } // while pregion

        pInstrument = pInstrument->GetNext();
    }
}

// ========================================================================
// UpdateRootNoteInRegions
// ========================================================================
void CWavePropPg::UpdateRootNoteInRegions()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);

	if(!m_pWave)
	{
		return;
	}

    // Go thru all regions and chenge values in the regions that
    // refer to the wave being edited if the override flag is not set.
	CCollection* pCollection = m_pWave->GetCollection();
	if(!pCollection)
	{
		return;
	}
    
	WSMPL waveWSMPL = m_pWave->GetWSMPL();

    CInstrument * pInstrument = pCollection->m_Instruments.GetHead();
    while (pInstrument) // for all instruments go thru all regions
    {
        CRegion * pRegion = pInstrument->m_Regions.GetHead();
        while (pRegion)
        {
            if (pRegion->GetWaveNode() == m_pWave->GetNode())
            {
                // is override attenuation set?
                if (!pRegion->m_bOverrideWaveRootNote)
                {
                    pRegion->m_rWSMP.usUnityNote = waveWSMPL.usUnityNote;
					pInstrument->RefreshUI(true);
                    // updatesynth. 
                    ASSERT(pRegion->m_pInstrument);
                    ASSERT(pRegion->m_pInstrument->m_pCollection);
        			pRegion->m_pInstrument->UpdateInstrument();
                }
            }

            pRegion = pRegion->GetNext();

        } // while pregion

        pInstrument = pInstrument->GetNext();
    }
}


// ========================================================================
// UpdatePlaybackSettingsInRegions
// ========================================================================
void CWavePropPg::UpdatePlaybackSettingsInRegions()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(!m_pWave)
	{
		return;
	}

    // Go thru all regions and chenge values in the regions that
    // refer to the wave being edited if the override flag is not set.
	CCollection* pCollection = m_pWave->GetCollection();
	if(!pCollection)
	{
		return;
	}

    CInstrument * pInstrument = pCollection->m_Instruments.GetHead();
    while (pInstrument) // for all instruments go thru all regions
    {
        CRegion * pRegion = pInstrument->m_Regions.GetHead();
        while (pRegion)
        {
			if (pRegion->GetWaveNode() == m_pWave->GetNode())
			{
				// is override playback settings set?
				if (!pRegion->m_bOverrideWaveMoreSettings)
				{
					WSMPL regionWaveWSMPL = pRegion->m_pWave->GetWSMPL();
					WLOOP regionWaveLoop = pRegion->m_pWave->GetWLOOP();

					pRegion->m_rWSMP.cSampleLoops = regionWaveWSMPL.cSampleLoops;
					memcpy(&pRegion->m_rWLOOP, &regionWaveLoop , sizeof(pRegion->m_rWLOOP));
					// update the synth.
					ASSERT(pRegion->m_pInstrument);
        			pRegion->m_pInstrument->UpdateInstrument();
				}
			}
            pRegion = pRegion->GetNext();
        } // while pregion
        pInstrument = pInstrument->GetNext();
    }
}


void CWavePropPg::PostValidate(UINT	nControl)
{
	m_pPropMgr->ShowPropSheet();
	CWnd * pWnd = GetDlgItem(nControl);
	if (IsWindow(pWnd->GetSafeHwnd()))
    {
        pWnd->SetFocus();
        ((CEdit *)pWnd)->SetSel(0, -1);
    }
}

void CWavePropPg::OnDeltaposRootNoteSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	if (m_bTouchedByProgram) return;

	USHORT usNote = GetNoteInEditBox();

	int nNewPos = usNote + pNMUpDown->iDelta;
	
	if (nNewPos < 0) nNewPos = 0; else if (nNewPos > 127) nNewPos = 127;
	*pResult = 0;
	
	if(SUCCEEDED(UpdateRootNote((USHORT)nNewPos)))
		UpdateRootNoteText(nNewPos);
}

void CWavePropPg::OnKillfocusLoopStart() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	DWORD dwLoopStart = GetDlgItemInt(IDC_LOOP_START);
	DWORD dwLoopEnd = GetDlgItemInt(IDC_LOOP_END);
	DWORD dwLoopLength = m_dwLoopLength;
	
	if(!m_bLockLoopLength)
		dwLoopLength = dwLoopEnd - dwLoopStart + 1;

	if((int)dwLoopLength < MINIMUM_VALID_LOOPLENGTH)
		dwLoopLength = MINIMUM_VALID_LOOPLENGTH;

	if(dwLoopStart + dwLoopLength > m_pWave->m_dwWaveLength)
		dwLoopStart = m_pWave->m_dwWaveLength - dwLoopLength;

	if(m_bLockLoopLength)
		dwLoopEnd = dwLoopStart + m_dwLoopLength - 1;

	if(m_bLockLoopLength && (dwLoopStart + dwLoopLength) > m_pWave->m_dwWaveLength - 1)
	{
		CString sWindowText;
		sWindowText.Format("%d", m_dwLoopStart);
		m_LoopStartEdit.SetWindowText(sWindowText);
		return;
	}

	ValidateAndSetLoopValues(dwLoopStart, dwLoopLength);
}

void CWavePropPg::OnKillfocusLoopLength() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CString sLoopValue;
	
	m_LoopLengthEdit.GetWindowText(sLoopValue);
	DWORD dwLoopLength = (DWORD)_ttol(sLoopValue);

	ValidateAndSetLoopValues(m_dwLoopStart, dwLoopLength);
}

void CWavePropPg::OnKillfocusLoopEnd() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CString sLoopValue;
	m_LoopEndEdit.GetWindowText(sLoopValue);
	DWORD dwLoopEnd = (DWORD)_ttol(sLoopValue);

	if(dwLoopEnd > m_pWave->m_dwWaveLength - 1)
		dwLoopEnd = m_pWave->m_dwWaveLength - 1;
	// Always false
	//else if(dwLoopEnd < 0)
	//	dwLoopEnd = 0;

	
	DWORD dwLoopStart = m_dwLoopStart;
	DWORD dwLoopLength = m_dwLoopLength;

	// If the length is locked the start point gets modified;
	// else the length is adjusted for the end point
	if(m_bLockLoopLength)
		dwLoopStart = dwLoopEnd - (m_dwLoopLength - 1);
	else
		dwLoopLength = abs(dwLoopEnd - dwLoopStart + 1);

	if(m_bLockLoopLength && (int)dwLoopStart < 0)
	{
		CString sWindowText;
		sWindowText.Format("%d", (m_dwLoopStart + m_dwLoopLength));
		m_LoopEndEdit.SetWindowText(sWindowText);
		return;
	}
		
	ValidateAndSetLoopValues(dwLoopStart, dwLoopLength);
}

void CWavePropPg::OnDeltaposLoopEndSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// If the length is locked the start point gets modified;
	// else the length is adjusted for the end point
	if(m_bLockLoopLength)
		OnDeltaposLoopStartSpin(pNMHDR, pResult);
	else
		OnDeltaposLoopLengthSpin(pNMHDR, pResult);

	*pResult = 1;
}

void CWavePropPg::OnLoopLengthLockCheck() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_bLockLoopLength = m_LoopLengthLockCheck.GetCheck() != 0;

	// If length is locked don't allow the edit
	if(m_bLockLoopLength == 1)
	{
		m_LoopLengthEdit.EnableWindow(false);
		m_LoopLengthSpin.EnableWindow(false);
	}
	else
	{
		m_LoopLengthEdit.EnableWindow(true);
		m_LoopLengthSpin.EnableWindow(true);
	}
}

void CWavePropPg::OnDeltaposLoopLengthSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iDelta = pNMUpDown->iDelta;
	
	DWORD dwLoopLength = m_dwLoopLength + iDelta;
	if((int)dwLoopLength < MINIMUM_VALID_LOOPLENGTH)
		dwLoopLength = MINIMUM_VALID_LOOPLENGTH;
	else if((m_dwLoopStart + dwLoopLength) > m_pWave->m_dwWaveLength)
		dwLoopLength = m_pWave->m_dwWaveLength - m_dwLoopStart;

	CString sLoopValue;
	m_LoopStartEdit.GetWindowText(sLoopValue);
	DWORD dwLoopStart = (DWORD)_ttol(sLoopValue);

	ValidateAndSetLoopValues(dwLoopStart, dwLoopLength);
	
	*pResult = 1;
}

void CWavePropPg::OnDeltaposLoopStartSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iDelta = pNMUpDown->iDelta;

	DWORD dwLoopStart = m_dwLoopStart + iDelta;
	DWORD dwLoopLength = m_dwLoopLength;
	if(!m_bLockLoopLength)
	{
		dwLoopLength -= iDelta;
		if((int)dwLoopLength < MINIMUM_VALID_LOOPLENGTH)
			dwLoopLength = MINIMUM_VALID_LOOPLENGTH;
	}

	if((int)dwLoopStart < MINIMUM_VALID_LOOPSTART)
		dwLoopStart = MINIMUM_VALID_LOOPSTART;
	
	if(dwLoopStart + dwLoopLength > m_pWave->m_dwWaveLength)
		dwLoopStart = m_pWave->m_dwWaveLength - dwLoopLength;
		
	ValidateAndSetLoopValues(dwLoopStart, dwLoopLength);
	
	*pResult = 1;
}

void CWavePropPg::OnKillfocusSelectionEnd() 
{
	int nSelectionEnd = GetDlgItemInt(IDC_SELECTION_END);
	if(m_pWave->m_dwWaveLength == 0)
		nSelectionEnd = m_nSelectionEnd;

	m_pWave->ConstrainToSelectionBoundaries(nSelectionEnd);

	int nSelectionStart = m_nSelectionStart;
	if (nSelectionEnd < nSelectionStart)
		nSelectionStart = nSelectionEnd;

	if(m_bLockSelectionLength)
		nSelectionStart = nSelectionEnd - (m_nSelectionEnd - m_nSelectionStart);
	
	SetSelection(nSelectionStart, nSelectionEnd);
}

void CWavePropPg::OnDeltaposSelectionEndSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iDelta = pNMUpDown->iDelta;
	if(m_pWave->m_dwWaveLength == 0)
		iDelta = 0;

	int nSelectionEnd = m_nSelectionEnd;
	nSelectionEnd += iDelta;

	m_pWave->ConstrainToSelectionBoundaries(nSelectionEnd);

	int nSelectionStart = m_nSelectionStart;
	if (nSelectionEnd < nSelectionStart)
		nSelectionStart = nSelectionEnd;

	if(m_bLockSelectionLength)
		nSelectionStart = nSelectionEnd - (m_nSelectionEnd - m_nSelectionStart);

	// Check for Snap
	EnsureSnap(nSelectionEnd, (iDelta < 0) ? SNAP_BACKWARD : SNAP_FORWARD);

	SetSelection(nSelectionStart, nSelectionEnd, true, false);
	
	*pResult = 1;
}

void CWavePropPg::OnKillfocusSelectionLength() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nSelectionLength = GetDlgItemInt(IDC_SELECTION_LENGTH);
	if(m_pWave->m_dwWaveLength == 0)
		nSelectionLength = 0;

	ConstrainSelectionLength(nSelectionLength);
	SetSelection(m_nSelectionStart, m_nSelectionStart + nSelectionLength - 1);
}

void CWavePropPg::OnSelectionLengthLockCheck() 
{
	m_bLockSelectionLength = m_SelectionLengthLockCheck.GetCheck() != 0;
	
	// Turn SnapToZero On/Off
	if(m_pWave)
	{
		const CWaveNode* pWaveNode = m_pWave->GetNode();
		if(pWaveNode)
		{
			CDLSComponent* pComponent = pWaveNode->GetComponent();
			ASSERT(pComponent);
			
			pComponent->SetSnapToZero(!m_bLockSelectionLength);
		}
	}

	SetSelectionLengthLock(m_bLockSelectionLength);
}

void CWavePropPg::OnDeltaposSelectionLengthSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iDelta = pNMUpDown->iDelta;

	if(m_pWave->m_dwWaveLength == 0)
		iDelta = 0;

	int nSelectionLength  = m_nSelectionEnd - m_nSelectionStart;
	nSelectionLength += iDelta;

	ConstrainSelectionLength(nSelectionLength);
	int nSelectionEnd = m_nSelectionStart + nSelectionLength;

	// Check for Snap
	EnsureSnap(nSelectionEnd, (iDelta < 0) ? SNAP_BACKWARD : SNAP_FORWARD);

	SetSelection(m_nSelectionStart, nSelectionEnd, true, false);
	*pResult = 1;
}

void CWavePropPg::OnKillfocusSelectionStart() 
{
	int nSelectionStart = GetDlgItemInt(IDC_SELECTION_START);
	if(m_pWave->m_dwWaveLength == 0)
		nSelectionStart = m_nSelectionStart;

	m_pWave->ConstrainToSelectionBoundaries(nSelectionStart);

	int nSelectionEnd = m_nSelectionEnd;
	if (nSelectionStart > nSelectionEnd)
		nSelectionEnd = nSelectionStart;

	if (m_bLockSelectionLength && ((m_nSelectionEnd - m_nSelectionStart) + nSelectionStart < (int)m_pWave->m_dwWaveLength))
		nSelectionEnd = (m_nSelectionEnd - m_nSelectionStart) + nSelectionStart;

	SetSelection(nSelectionStart, nSelectionEnd);
}

void CWavePropPg::OnDeltaposSelectionStartSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int iDelta = pNMUpDown->iDelta;

	if(m_pWave->m_dwWaveLength == 0)
	{
		iDelta = 0;
	}

	int nSelectionStart = m_nSelectionStart+iDelta;
	m_pWave->ConstrainToSelectionBoundaries(nSelectionStart);

	int nSelectionEnd = m_nSelectionEnd;
	if (nSelectionStart > nSelectionEnd)
		nSelectionEnd = nSelectionStart;

	if(m_bLockSelectionLength)
		nSelectionEnd = nSelectionStart + (m_nSelectionEnd - m_nSelectionStart);

	int nSelMax = m_pWave->m_dwWaveLength-m_pWave->GetDwDecompressedStart(true);
	if(m_bLockSelectionLength && nSelectionEnd > nSelMax-1)
	{
		nSelectionEnd = nSelMax-1;
		nSelectionStart = nSelectionEnd - (m_nSelectionEnd - m_nSelectionStart);
	}

	// Check for Snap
	EnsureSnap(nSelectionStart, (iDelta < 0) ? SNAP_BACKWARD : SNAP_FORWARD);

	SetSelection(nSelectionStart, nSelectionEnd, true, false);
	
	*pResult = 1;
}

void CWavePropPg::SetSelection(int nSelectionStart, int nSelectionEnd, bool bSetEditor, bool bDoSnap)
{
	if(nSelectionStart > nSelectionEnd)
		Swap(nSelectionStart, nSelectionEnd);

	if(m_bLockSelectionLength && (nSelectionEnd - nSelectionStart) != (m_nSelectionEnd - m_nSelectionStart))
	{
		nSelectionStart = m_nSelectionStart;
		nSelectionEnd = m_nSelectionEnd;
	}

	m_nSelectionStart = nSelectionStart;
	m_nSelectionEnd = nSelectionEnd;

	SetDlgItemInt(IDC_SELECTION_START, nSelectionStart);
	SetDlgItemInt(IDC_SELECTION_END, nSelectionEnd);
	SetDlgItemInt(IDC_SELECTION_LENGTH, nSelectionEnd - nSelectionStart + 1);

	if(bSetEditor && m_pWave) 
	{
		CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
		if(pWaveEditor)
			pWaveEditor->SetSelection(m_nSelectionStart, m_nSelectionEnd, bDoSnap);
	}
}

bool CWavePropPg::IsSelectionLengthLocked()
{
	return m_bLockSelectionLength;
}

bool CWavePropPg::IsLoopLengthLocked()
{
	return m_bLockLoopLength;
}

void CWavePropPg::SetSelectionLengthLock(bool bLock)
{
	m_bLockSelectionLength = bLock;

	if(m_pWave && m_pWave->GetWaveEditor())
	{
		CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
		EnableControl(IDC_SELECTION_LENGTH, !m_bLockSelectionLength);
		EnableControl(IDC_SELECTION_LENGTH_SPIN, !m_bLockSelectionLength);

		pWaveEditor->InvalidateRect(NULL);
	}
	else
	{
		EnableControl(IDC_SELECTION_LENGTH, false);
		EnableControl(IDC_SELECTION_LENGTH_SPIN, false);
		EnableControl(IDC_SELECTION_LENGTH_LOCK_CHECK, false);
	}

	m_SelectionLengthLockCheck.SetCheck(bLock);
}

void CWavePropPg::Swap(int& nSelectionStart, int& nSelectionEnd)
{
	int nTemp = nSelectionStart;
	nSelectionStart = nSelectionEnd;
	nSelectionEnd = nTemp;
}

void CWavePropPg::OnForwardLoopRadio() 
{
	ASSERT(m_pWave);
	if(m_pWave== NULL)
	{
		return;
	}

	CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
	if(pWaveEditor)
	{
		if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_WAVE_LOOP_TYPE)))
		{
			return;
		}
	}

	SetWaveLoopType(WLOOP_TYPE_FORWARD);
	m_pWave->NotifyWaveChange(true);
}

void CWavePropPg::OnLoopAndReleaseRadio() 
{
	ASSERT(m_pWave);
	if(m_pWave== NULL)
	{
		return;
	}

	CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
	if(pWaveEditor)
	{
		if(FAILED(pWaveEditor->SaveStateForUndo(IDS_UNDO_WAVE_LOOP_TYPE)))
		{
			return;
		}
	}

	SetWaveLoopType(WLOOP_TYPE_RELEASE);
	m_pWave->NotifyWaveChange(true);
}

void CWavePropPg::SetWaveLoopType(ULONG ulLoopType)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	CButton* pForwardLoopBtn = (CButton*)GetDlgItem(IDC_FORWARD_LOOP_RADIO);
	CButton* pLoopReleaseBtn = (CButton*)GetDlgItem(IDC_LOOP_AND_RELEASE_RADIO);
	ASSERT(pForwardLoopBtn);
	ASSERT(pLoopReleaseBtn);
	if(pForwardLoopBtn == NULL || pLoopReleaseBtn == NULL)
	{
		return;
	}

	if(ulLoopType == WLOOP_TYPE_FORWARD)
	{
		pForwardLoopBtn->SetCheck(1);
		pLoopReleaseBtn->SetCheck(0);
	}

	if(ulLoopType == WLOOP_TYPE_RELEASE)
	{
		pForwardLoopBtn->SetCheck(0);
		pLoopReleaseBtn->SetCheck(1);
	}

	WLOOP waveLoop = m_pWave->GetWLOOP();
	if(waveLoop.ulType != ulLoopType)
	{
		waveLoop.ulType = ulLoopType;

		m_pWave->SetWLOOP(waveLoop);
		m_pWave->CopyWSMPToSMPL();
		m_pWave->UpdateWave();
		m_pWave->UpdateDummyInstrument();

		UpdatePlaybackSettingsInRegions();

		// Set flag so we know to save file 
		m_pWave->NotifyWaveChange(true);
	}
}

void CWavePropPg::EnableLoopTypeButtons(BOOL bEnable)
{
	CButton* pForwardLoopBtn = (CButton*)GetDlgItem(IDC_FORWARD_LOOP_RADIO);
	CButton* pLoopReleaseBtn = (CButton*)GetDlgItem(IDC_LOOP_AND_RELEASE_RADIO);
	ASSERT(pForwardLoopBtn);
	ASSERT(pLoopReleaseBtn);
	if(pForwardLoopBtn == NULL || pLoopReleaseBtn == NULL)
	{
		return;
	}

	pForwardLoopBtn->EnableWindow(bEnable);
	pLoopReleaseBtn->EnableWindow(bEnable);
}

void CWavePropPg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	bool bWaveChanged = false;
	WSMPL waveWSMPL = m_pWave->GetWSMPL();
	if(CheckSliderForDefaultClick(*m_pmsAttenuation, point) == TRUE)
	{
		waveWSMPL.lAttenuation = 0;
		bWaveChanged = true;
	}

	if(CheckSliderForDefaultClick(*m_pmsFineTune, point) == TRUE)
	{
		waveWSMPL.sFineTune = 0;
		bWaveChanged = true;
	}

	if(bWaveChanged)
	{
		// Update the wave SMPL values
		m_pWave->SetWSMPL(waveWSMPL);
		m_pWave->CopyWSMPToSMPL();
    
		m_pWave->UpdateWave();
		// Change the values in all the regions that refer to this wave 
		// and have not selected to override wave properties.
		UpdateAttenuationInRegions();

		m_pWave->UpdateDummyInstrument();
		m_pWave->NotifyWaveChange(true);
	}
	
	CPropertyPage::OnLButtonDblClk(nFlags, point);
}

BOOL CWavePropPg::CheckSliderForDefaultClick(MySlider& msSlider, CPoint point)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return FALSE;
	}

	CSliderCtrl* pSlider = (CSliderCtrl*)GetDlgItem(msSlider.SliderID());
	if(pSlider)
	{
		CRect rectThumb;
		pSlider->GetThumbRect(&rectThumb);
		if(rectThumb.PtInRect(point))
		{
			CWaveCtrl* pWaveEditor = m_pWave->GetWaveEditor();
			if(FAILED(pWaveEditor->SaveStateForUndo(msSlider.GetUndoStringID())))
			{
				return FALSE;
			}
			
			msSlider.SetValue(this, msSlider.GetDefaultValue());
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CWavePropPg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult;
	if (CSliderCollection::OnCommand(wParam, lParam, &lResult))
		return lResult;

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CWavePropPg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LRESULT lResult;
	if (CSliderCollection::OnNotify(wParam, lParam, &lResult))
		return lResult;
	
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

bool CWavePropPg::OnSliderUpdate(MySlider *pms, DWORD dwmscupdf)
{
	switch (dwmscupdf)
		{
		case dwmscupdfStart:
			return true; // save state is done on End of updates

		case dwmscupdfEnd:
			if (pms == m_pmsAttenuation)
				UpdateDbAttenuation();
			else
				{
				ASSERT(pms == m_pmsFineTune); // slider not handled
				UpdateFineTune();
				}
			return true;

		default:
			ASSERT(FALSE);
			return false;
		}
}

/* ensures the given selection length fits within boundaries */
void CWavePropPg::ConstrainSelectionLength(int& nSelLen)
{
	if (nSelLen < 1)
		nSelLen = (m_nSelectionEnd - m_nSelectionStart) + 1;
	else
		{
		int nSelMax = m_pWave->m_dwWaveLength-m_pWave->GetDwDecompressedStart(true);
		if ((m_nSelectionStart + nSelLen) >= nSelMax)
			nSelLen = nSelMax - m_nSelectionStart;
		}
}

/* if necessary, snaps the given selection value in the given direction */
void CWavePropPg::EnsureSnap(int& nSel, UINT nSnapDirection)
{
	if(m_pWave)
		{
		CWaveNode* pWaveNode = m_pWave->GetNode();
		ASSERT(pWaveNode);
		if(pWaveNode)
			{
			CDLSComponent* pComponent = pWaveNode->GetComponent();
			ASSERT(pComponent);
			if(pComponent && pComponent->IsSnapToZero())
				{
				DWORD dwDecompressedStart = m_pWave->GetDwDecompressedStart(true);
				DWORD dwSel = nSel+dwDecompressedStart;
				m_pWave->SnapToZero(dwSel, nSnapDirection);
				nSel= dwSel-dwDecompressedStart;
				}
			}
		}
}
