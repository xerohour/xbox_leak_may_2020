// PropPageNote.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MIDIStripMgr.h"
#include "MIDIMgr.h"
#include "PropPageNote.h"
#include "PropPageNoteVar.h"
#include "PropPageMgr.h"
#include "ioDMStyle.h"
#include "pattern.h"
#include "LockoutNotification.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DURATION_MAXBAR			32767
#define DURATION_MAXBARWIDTH	5
#define HUMANIZE_VELOCITY_MAX	127
#define HUMANIZE_MAX			255
#define IS_DMUS_PLAYMODE_FIXED (!(m_PropNote.m_dwUndetermined & UD_PLAYMODE) && (m_PropNote.m_bPlayMode == DMUS_PLAYMODE_FIXED))
#define SCALEFUNC_MAX			7

static const TCHAR g_MidiValueToName[12*2+1] = _T("C DbD EbE F GbG AbA BbB ");
static const TCHAR g_pstrUnknown[11] = _T("----------");
static const TCHAR g_AccidentalToName[16*2+1] = _T("8b7b6b5b4b3bbb b   # x#x4#5#6#7#");
static const TCHAR g_DiatonicOffsetToName[16*2+1] = _T("-9-8-7-6-5-4-3-2  +2+3+4+5+6+7+8");

/*
When editing a note, we pass back to the MIDIMgr a PropNote that only contains
the changes just made.  We let the MIDIMgr update us with a new PropNote that
propagates the changes to the other componenets (m_bOctave, m_bScalevalue, start/end/dur times, etc.)
This way we remove our dependence on the chord/key/root that we're editing in, and the
playmode(s) of the note(s) we're editing.
*/


short CNotePropPageMgr::sm_nActiveTab = 0;

/////////////////////////////////////////////////////////////////////////////
// CNotePropPageMgr property page

CNotePropPageMgr::CNotePropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pPropPageNote = NULL;
	m_pPropPageNoteVar = NULL;
	m_pDMUSProdFramework = NULL;
	CStaticPropPageManager::CStaticPropPageManager();
}

CNotePropPageMgr::~CNotePropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageNote )
	{
		delete m_pPropPageNote;
	}
	if( m_pPropPageNoteVar )
	{
		delete m_pPropPageNoteVar;
	}
	/*if( m_pDMUSProdFramework )
	{
		m_pDMUSProdFramework->Release();
	}*/
	CStaticPropPageManager::~CStaticPropPageManager();
}

HRESULT STDMETHODCALLTYPE CNotePropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};

HRESULT STDMETHODCALLTYPE CNotePropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
	BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	// Get "Note" text
	CString strNote;
	strNote.LoadString( IDS_PROPPAGE_NOTE );

	// Format title
	CString strTitle = strNote;
	
	if( m_pIPropPageObject )
	{
		CMIDIMgr* pMIDIMgr = (CMIDIMgr *)m_pIPropPageObject;

		if( pMIDIMgr
		&&	pMIDIMgr->m_pIDMTrack )
		{
			strTitle.LoadString( IDS_PROPPAGE_PATTERN_NOTE );
		}
		else if( pMIDIMgr 
		&&  pMIDIMgr->m_pActivePianoRollStrip
		&&  pMIDIMgr->m_pActivePianoRollStrip->m_pPartRef
		&&  pMIDIMgr->m_pActivePianoRollStrip->m_pPartRef->m_pPattern )
		{
			CString strStyleName;
			BSTR bstrStyleName;

			// Get Style Name
			if( pMIDIMgr->m_pIStyleNode )
			{
				if( SUCCEEDED ( pMIDIMgr->m_pIStyleNode->GetNodeName( &bstrStyleName ) ) )
				{
					strStyleName = bstrStyleName;
					::SysFreeString( bstrStyleName );
				}
			}

			// Put together the title
			if( strStyleName.IsEmpty() == FALSE )
			{
				strTitle = strStyleName +
						   _T(" - " );
			}
			strTitle += pMIDIMgr->m_pActivePianoRollStrip->m_pPartRef->m_pPattern->m_strName +
					    _T(" - ") +
					    pMIDIMgr->m_pActivePianoRollStrip->m_pPartRef->m_strName +
					    _T(" ") +
					    strNote;
		}
	}

	*pbstrTitle = strTitle.AllocSysString();
	*pfAddPropertiesText = TRUE;
		
	return S_OK;
};

HRESULT STDMETHODCALLTYPE CNotePropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
	LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	// Add Note tab
	if( NULL == m_pPropPageNote )
	{
		m_pPropPageNote = new PropPageNote( this );
	}
	if( m_pPropPageNote )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pPropPageNote->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
		if (!m_pDMUSProdFramework)
		{
			pIPropSheet->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pDMUSProdFramework );
			// This is another work-around to get rid of a cyclical dependency.
			// I really need to take some time and think about a better way
			// to fix them.  -jdooley
			m_pDMUSProdFramework->Release();
		}
	}

	// Add Note Variation tab
	if( NULL == m_pPropPageNoteVar )
	{
		m_pPropPageNoteVar = new PropPageNoteVar( this );
	}
	if( m_pPropPageNoteVar )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pPropPageNoteVar->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};

HRESULT STDMETHODCALLTYPE CNotePropPageMgr::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pIPropSheet )
	{
		m_pIPropSheet->GetActivePage( &CNotePropPageMgr::sm_nActiveTab );
	}

	RemoveCurrentObject();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->Release();
		m_pIPropSheet = NULL;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CNotePropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	NotePropData* pNotePropData;
	if( m_pIPropPageObject == NULL )
	{
		pNotePropData = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pNotePropData ) ) )
	{
		return E_FAIL;
	}
	if( pNotePropData )
	{
		m_pPropPageNote->SetParameters( &pNotePropData->m_Param );
		m_pPropPageNote->SetNote( &pNotePropData->m_PropNote );
		m_pPropPageNoteVar->SetParameters( &pNotePropData->m_Param );
		m_pPropPageNoteVar->SetNote( &pNotePropData->m_PropNote );
	}

	// Update the property page title
	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
	}
	return S_OK;
};

HRESULT CNotePropPageMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageNote )
	{
		m_pPropPageNote->SetObject( pINewPropPageObject );
	}
	if( m_pPropPageNoteVar )
	{
		m_pPropPageNoteVar->SetObject( pINewPropPageObject );
	}
	HRESULT hr = CBasePropPageManager::SetObject( pINewPropPageObject );

	// Update the property page
	RefreshData();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// PropPageNote property page

PropPageNote::PropPageNote(CNotePropPageMgr* pPageManager) : CPropertyPage(PropPageNote::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pPageManager != NULL );
	m_pPageManager = pPageManager;

	//{{AFX_DATA_INIT(PropPageNote)
	//}}AFX_DATA_INIT
	// default to 16th note (768 PPQ / 4 16th notes per quarter note)
	m_lMaxStartBeats = 4;
	m_lMaxStartGrids = 4;
	m_lMinStartClocks = -(192 / 2);
	m_lMaxStartClocks = 192 + m_lMinStartClocks - 1;

	m_lMaxChordBeats = m_lMaxStartBeats;

	m_lMaxEndBeats = m_lMaxStartBeats;
	m_lMaxEndGrids = m_lMaxStartGrids;
	m_lMinEndClocks = m_lMinStartClocks;
	m_lMaxEndClocks = m_lMaxStartClocks;

	m_lMaxMeasures = 1000;  // default to maximum of 1000 measures
	m_pIPPO = NULL;
	m_dwChanging = 0;
	m_fValidPropNote = FALSE;
	m_fAllDisabled = TRUE;
	m_fNeedToDetach = FALSE;
	m_fInUpdateControls = FALSE;
}

void PropPageNote::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropPageNote)
	DDX_Control(pDX, IDC_NOTE_EDIT_DIATONIC_OFFSET, m_editDiatonicOffset);
	DDX_Control(pDX, IDC_NOTE_SPIN_DIATONIC_OFFSET, m_spinDiatonicOffset);
	DDX_Control(pDX, IDC_NOTE_EDIT_VELOCITY, m_editVelocity);
	DDX_Control(pDX, IDC_NOTE_EDIT_STARTTICK, m_editStartTick);
	DDX_Control(pDX, IDC_NOTE_EDIT_STARTGRID, m_editStartGrid);
	DDX_Control(pDX, IDC_NOTE_EDIT_STARTBEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_NOTE_EDIT_STARTBAR, m_editStartBar);
	DDX_Control(pDX, IDC_NOTE_EDIT_SCALEFUNC, m_editScaleFunc);
	DDX_Control(pDX, IDC_NOTE_EDIT_OCTAVE, m_editOctave);
	DDX_Control(pDX, IDC_NOTE_EDIT_MIDINOTE, m_editMidiNote);
	DDX_Control(pDX, IDC_NOTE_EDIT_HUMVELOCITY, m_editHumVel);
	DDX_Control(pDX, IDC_NOTE_EDIT_HUMSTART, m_editHumStart);
	DDX_Control(pDX, IDC_NOTE_EDIT_HUMDURATION, m_editHumDuration);
	DDX_Control(pDX, IDC_NOTE_EDIT_ENDTICK, m_editEndTick);
	DDX_Control(pDX, IDC_NOTE_EDIT_ENDGRID, m_editEndGrid);
	DDX_Control(pDX, IDC_NOTE_EDIT_ENDBEAT, m_editEndBeat);
	DDX_Control(pDX, IDC_NOTE_EDIT_ENDBAR, m_editEndBar);
	DDX_Control(pDX, IDC_NOTE_EDIT_DURTICK, m_editDurTick);
	DDX_Control(pDX, IDC_NOTE_EDIT_DURGRID, m_editDurGrid);
	DDX_Control(pDX, IDC_NOTE_EDIT_DURBEAT, m_editDurBeat);
	DDX_Control(pDX, IDC_NOTE_EDIT_DURBAR, m_editDurBar);
	DDX_Control(pDX, IDC_NOTE_EDIT_CHORDBEAT, m_editChordBeat);
	DDX_Control(pDX, IDC_NOTE_EDIT_CHORDBAR, m_editChordBar);
	DDX_Control(pDX, IDC_NOTE_EDIT_ACCIDENTAL, m_editAccidental);
	DDX_Control(pDX, IDC_NOTE_SPIN_ACCIDENTAL, m_spinAccidental);
	DDX_Control(pDX, IDC_NOTE_SPIN_SCALEFUNC, m_spinScaleFunc);
	DDX_Control(pDX, IDC_COMBO_INVERSION_ID, m_comboInversionId);
	DDX_Control(pDX, IDC_COMBO_PLAYMODE, m_comboPlayMode);
	DDX_Control(pDX, IDC_NOTE_SPIN_STARTGRID, m_spinStartGrid);
	DDX_Control(pDX, IDC_NOTE_SPIN_ENDGRID, m_spinEndGrid);
	DDX_Control(pDX, IDC_NOTE_SPIN_DURGRID, m_spinDurGrid);
	DDX_Control(pDX, IDC_NOTE_SPIN_DURTICK, m_spinDurTick);
	DDX_Control(pDX, IDC_NOTE_SPIN_DURBEAT, m_spinDurBeat);
	DDX_Control(pDX, IDC_NOTE_SPIN_DURBAR, m_spinDurBar);
	DDX_Control(pDX, IDC_NOTE_SPIN_VELOCITY, m_spinVelocity);
	DDX_Control(pDX, IDC_NOTE_SPIN_STARTTICK, m_spinStartTick);
	DDX_Control(pDX, IDC_NOTE_SPIN_STARTBEAT, m_spinStartBeat);
	DDX_Control(pDX, IDC_NOTE_SPIN_STARTBAR, m_spinStartBar);
	DDX_Control(pDX, IDC_NOTE_SPIN_OCTAVE, m_spinOctave);
	DDX_Control(pDX, IDC_NOTE_SPIN_MIDINOTE, m_spinMidiNote);
	DDX_Control(pDX, IDC_NOTE_SPIN_HUMVELOCITY, m_spinHumVelocity);
	DDX_Control(pDX, IDC_NOTE_SPIN_HUMSTART, m_spinHumStart);
	DDX_Control(pDX, IDC_NOTE_SPIN_HUMDURATION, m_spinHumDuration);
	DDX_Control(pDX, IDC_NOTE_SPIN_ENDTICK, m_spinEndTick);
	DDX_Control(pDX, IDC_NOTE_SPIN_ENDBEAT, m_spinEndBeat);
	DDX_Control(pDX, IDC_NOTE_SPIN_ENDBAR, m_spinEndBar);
	DDX_Control(pDX, IDC_NOTE_SPIN_CHORDBEAT, m_spinChordBeat);
	DDX_Control(pDX, IDC_NOTE_SPIN_CHORDBAR, m_spinChordBar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageNote, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageNote)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_MIDINOTE, OnDeltaposNoteSpinMidinote)
	ON_EN_CHANGE(IDC_NOTE_EDIT_DURTICK, OnChangeNoteEditDurtick)
	ON_EN_CHANGE(IDC_NOTE_EDIT_DURBEAT, OnChangeNoteEditDurbeat)
	ON_EN_CHANGE(IDC_NOTE_EDIT_DURBAR, OnChangeNoteEditDurbar)
	ON_EN_CHANGE(IDC_NOTE_EDIT_HUMDURATION, OnChangeNoteEditHumduration)
	ON_EN_CHANGE(IDC_NOTE_EDIT_HUMSTART, OnChangeNoteEditHumstart)
	ON_EN_CHANGE(IDC_NOTE_EDIT_HUMVELOCITY, OnChangeNoteEditHumvelocity)
	ON_EN_CHANGE(IDC_NOTE_EDIT_VELOCITY, OnChangeNoteEditVelocity)
	ON_EN_CHANGE(IDC_NOTE_EDIT_OCTAVE, OnChangeNoteEditOctave)
	ON_EN_CHANGE(IDC_NOTE_EDIT_DURGRID, OnChangeNoteEditDurgrid)
	ON_EN_CHANGE(IDC_NOTE_EDIT_CHORDBAR, OnChangeNoteEditChordbar)
	ON_EN_CHANGE(IDC_NOTE_EDIT_CHORDBEAT, OnChangeNoteEditChordbeat)
	ON_EN_CHANGE(IDC_NOTE_EDIT_STARTBAR, OnChangeNoteEditStartbar)
	ON_EN_CHANGE(IDC_NOTE_EDIT_STARTBEAT, OnChangeNoteEditStartbeat)
	ON_EN_CHANGE(IDC_NOTE_EDIT_STARTGRID, OnChangeNoteEditStartgrid)
	ON_EN_CHANGE(IDC_NOTE_EDIT_STARTTICK, OnChangeNoteEditStarttick)
	ON_EN_CHANGE(IDC_NOTE_EDIT_ENDBAR, OnChangeNoteEditEndbar)
	ON_EN_CHANGE(IDC_NOTE_EDIT_ENDBEAT, OnChangeNoteEditEndbeat)
	ON_EN_CHANGE(IDC_NOTE_EDIT_ENDGRID, OnChangeNoteEditEndgrid)
	ON_EN_CHANGE(IDC_NOTE_EDIT_ENDTICK, OnChangeNoteEditEndtick)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_DURTICK, OnKillfocusNoteEditDurtick)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_DURGRID, OnKillfocusNoteEditDurgrid)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_DURBEAT, OnKillfocusNoteEditDurbeat)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_DURBAR, OnKillfocusNoteEditDurbar)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_HUMDURATION, OnKillfocusNoteEditHumduration)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_HUMSTART, OnKillfocusNoteEditHumstart)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_HUMVELOCITY, OnKillfocusNoteEditHumvelocity)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_VELOCITY, OnKillfocusNoteEditVelocity)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_OCTAVE, OnKillfocusNoteEditOctave)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_CHORDBAR, OnKillfocusNoteEditChordbar)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_CHORDBEAT, OnKillfocusNoteEditChordbeat)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_STARTBAR, OnKillfocusNoteEditStartbar)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_STARTBEAT, OnKillfocusNoteEditStartbeat)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_STARTGRID, OnKillfocusNoteEditStartgrid)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_STARTTICK, OnKillfocusNoteEditStarttick)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_ENDBAR, OnKillfocusNoteEditEndbar)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_ENDBEAT, OnKillfocusNoteEditEndbeat)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_ENDGRID, OnKillfocusNoteEditEndgrid)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_ENDTICK, OnKillfocusNoteEditEndtick)
	ON_CBN_SELCHANGE(IDC_COMBO_PLAYMODE, OnSelchangeComboPlaymode)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_HUMDURATION, OnDeltaposNoteSpinHumduration)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_HUMSTART, OnDeltaposNoteSpinHumstart)
	ON_CBN_SELCHANGE(IDC_COMBO_INVERSION_ID, OnSelchangeComboInversionId)
	ON_EN_CHANGE(IDC_NOTE_EDIT_SCALEFUNC, OnChangeNoteEditScalefunc)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_ACCIDENTAL, OnDeltaposNoteSpinAccidental)
	ON_EN_KILLFOCUS(IDC_NOTE_EDIT_SCALEFUNC, OnKillfocusNoteEditScalefunc)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_CHORDBEAT, OnDeltaposNoteSpinChordbeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_DURBEAT, OnDeltaposNoteSpinDurbeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_DURGRID, OnDeltaposNoteSpinDurgrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_DURTICK, OnDeltaposNoteSpinDurtick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_ENDBEAT, OnDeltaposNoteSpinEndbeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_ENDGRID, OnDeltaposNoteSpinEndgrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_ENDTICK, OnDeltaposNoteSpinEndtick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_STARTBEAT, OnDeltaposNoteSpinStartbeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_STARTGRID, OnDeltaposNoteSpinStartgrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_STARTTICK, OnDeltaposNoteSpinStarttick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NOTE_SPIN_DIATONIC_OFFSET, OnDeltaposNoteSpinDiatonicOffset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PropPageNote::SetParameters( PropPageNoteParams* pParam )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pParam != NULL );
	if (pParam == NULL)
	{
		return;
	}

	CDirectMusicPart* pDMPart = pParam->pPart;

	BOOL fChanged = FALSE;

	if( !m_fValidPropNote && pDMPart )
	{
		fChanged = TRUE;
	}

	m_fValidPropNote = (pDMPart != NULL) ? TRUE : FALSE;

	if( !m_fValidPropNote )
	{
		return;
	}

	m_PropNote.SetPart( pDMPart );

	if ( m_lMaxStartBeats != pDMPart->m_TimeSignature.m_bBeatsPerMeasure )
		fChanged = TRUE;
	m_lMaxStartBeats = pDMPart->m_TimeSignature.m_bBeatsPerMeasure;

	if ( m_lMaxStartGrids != pDMPart->m_TimeSignature.m_wGridsPerBeat )
		fChanged = TRUE;
	m_lMaxStartGrids = pDMPart->m_TimeSignature.m_wGridsPerBeat;

	if ( m_lMinStartClocks != -(pDMPart->m_mtClocksPerGrid / 2) )
		fChanged = TRUE;
	m_lMinStartClocks = -(pDMPart->m_mtClocksPerGrid / 2);
	if ( m_lMaxStartClocks != (pDMPart->m_mtClocksPerGrid + m_lMinStartClocks - 1) )
		fChanged = TRUE;
	m_lMaxStartClocks = pDMPart->m_mtClocksPerGrid + m_lMinStartClocks - 1;

	long lTmpPartMeasureLength = pDMPart->GetGridLength() / (pDMPart->m_TimeSignature.m_bBeatsPerMeasure * pDMPart->m_TimeSignature.m_wGridsPerBeat);
	if ( m_lMaxMeasures != lTmpPartMeasureLength )
		fChanged = TRUE;
	m_lMaxMeasures = lTmpPartMeasureLength;

	m_lMaxEndBeats = m_lMaxStartBeats;
	m_lMaxEndGrids = m_lMaxStartGrids;
	m_lMinEndClocks = m_lMinStartClocks;
	m_lMaxEndClocks = m_lMaxStartClocks;

	m_lMaxChordBeats = m_lMaxStartBeats;

	// Update the spin controls if any of the clock lengths have changed
	if (fChanged)
		OnInitDialog();
}

// BUGBUG: This call can be really slow when we get here when adding notes from
// a MIDI keyboard
void PropPageNote::SetNote( CPropNote* pPropNote )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	//TRACE("SetNote in %d\n", ::timeGetTime());

	// Store new values so that OnSetActive can use them
	m_PropNote = *pPropNote;
	m_PropNote.m_dwUndetermined |= UD_GRIDSTART | UD_OFFSET | UD_DURATION;
	m_PropNote.m_mtDuration = 0;
	m_PropNote.m_mtGridStart = 0;
	m_PropNote.m_nOffset = 0;

	// Make sure controls have been created and are visible
	if( (::IsWindow(m_spinStartBar.m_hWnd) == FALSE)
	||	(::IsWindowVisible(m_spinStartBar.m_hWnd) == FALSE) )
	{
		return;
	}

	// if m_PropNote is not valid, disable everything
	if ( !m_fValidPropNote )
	{
		if (m_fAllDisabled)
		{
			//TRACE("SetNote out really early%d\n", ::timeGetTime());
			return;
		}
		DisableItem(IDC_NOTE_EDIT_STARTBAR);
		m_spinStartBar.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_STARTBEAT);
		m_spinStartBeat.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_STARTGRID);
		m_spinStartGrid.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_STARTTICK);
		m_spinStartTick.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_HUMSTART);
		m_spinHumStart.EnableWindow(FALSE);

		DisableItem(IDC_NOTE_EDIT_ENDBAR);
		m_spinEndBar.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_ENDBEAT);
		m_spinEndBeat.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_ENDGRID);
		m_spinEndGrid.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_ENDTICK);
		m_spinEndTick.EnableWindow(FALSE);

		DisableItem(IDC_NOTE_EDIT_DURBAR);
		m_spinDurBar.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_DURBEAT);
		m_spinDurBeat.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_DURGRID);
		m_spinDurGrid.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_DURTICK);
		m_spinDurTick.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_HUMDURATION);
		m_spinHumDuration.EnableWindow(FALSE);

		DisableItem(IDC_NOTE_EDIT_VELOCITY);
		m_spinVelocity.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_HUMVELOCITY);
		m_spinHumVelocity.EnableWindow(FALSE);

		m_comboPlayMode.EnableWindow(FALSE);

		m_comboInversionId.SetCurSel( -1 );
		m_comboInversionId.EnableWindow(FALSE);

		DisableItem(IDC_NOTE_EDIT_SCALEFUNC);
		m_spinScaleFunc.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_ACCIDENTAL);
		m_spinAccidental.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_DIATONIC_OFFSET);
		m_spinDiatonicOffset.EnableWindow(FALSE);

		DisableItem(IDC_NOTE_EDIT_OCTAVE);
		m_spinOctave.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_MIDINOTE);
		m_spinMidiNote.EnableWindow(FALSE);

		DisableItem(IDC_NOTE_EDIT_CHORDBAR);
		m_spinDurBar.EnableWindow(FALSE);
		DisableItem(IDC_NOTE_EDIT_CHORDBEAT);
		m_spinChordBeat.EnableWindow(FALSE);
		m_fAllDisabled = TRUE;
		//TRACE("SetNote out early%d\n", ::timeGetTime());
		return;
	}

	long lTemp;
	CString strTmp;

	m_fAllDisabled = FALSE;

	// prevent control notifications from being dispatched when changing edit text
	CLockoutNotification LockoutNotification( m_hWnd );
	m_fInUpdateControls = TRUE;

	// Start Position
	if ( m_PropNote.m_dwUndetermined & UD_STARTBAR )
	{
		strTmp.Format( "%d", m_PropNote.m_lStartBar );
		m_editStartBar.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_STARTBAR, FALSE);
		m_spinStartBar.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_STARTBAR, TRUE, m_PropNote.m_lStartBar);
		m_spinStartBar.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_STARTBEAT )
	{
		//strTmp.Format( "%d", m_PropNote.m_lStartBeat );
		//m_editStartBeat.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_STARTBEAT, FALSE);
		m_spinStartBeat.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_STARTBEAT, TRUE, m_PropNote.m_lStartBeat);
		m_spinStartBeat.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_STARTGRID )
	{
		//strTmp.Format( "%d", m_PropNote.m_lStartGrid );
		//m_editStartGrid.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_STARTGRID, FALSE);
		m_spinStartGrid.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_STARTGRID, TRUE, m_PropNote.m_lStartGrid);
		m_spinStartGrid.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_STARTTICK )
	{
		//strTmp.Format( "%d", m_PropNote.m_lStartTick );
		//m_editStartTick.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_STARTTICK, FALSE);
		m_spinStartTick.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_STARTTICK, TRUE, m_PropNote.m_lStartTick);
		m_spinStartTick.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_TIMERANGE )
	{
		//PosToRangeText( m_PropNote.m_bTimeRange, strTmp );
		//m_editHumStart.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_HUMSTART, FALSE);
		m_spinHumStart.EnableWindow(TRUE);
	}
	else
	{
		PosToRangeText( m_PropNote.m_bTimeRange, strTmp );
		EnableItem(IDC_NOTE_EDIT_HUMSTART, TRUE, _ttol(strTmp));
		m_spinHumStart.EnableWindow(TRUE);
		m_spinHumStart.SetPos(m_PropNote.m_bTimeRange);
	}

	// End position
	if ( m_PropNote.m_dwUndetermined & UD_ENDBAR )
	{
		//strTmp.Format( "%d", m_PropNote.m_lEndBar );
		//m_editEndBar.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_ENDBAR, FALSE);
		m_spinEndBar.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_ENDBAR, TRUE, m_PropNote.m_lEndBar);
		m_spinEndBar.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_ENDBEAT )
	{
		//strTmp.Format( "%d", m_PropNote.m_lEndBeat );
		//m_editEndBeat.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_ENDBEAT, FALSE);
		m_spinEndBeat.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_ENDBEAT, TRUE, m_PropNote.m_lEndBeat);
		m_spinEndBeat.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_ENDGRID )
	{
		//strTmp.Format( "%d", m_PropNote.m_lEndGrid );
		//m_editEndGrid.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_ENDGRID, FALSE);
		m_spinEndGrid.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_ENDGRID, TRUE, m_PropNote.m_lEndGrid);
		m_spinEndGrid.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_ENDTICK )
	{
		//strTmp.Format( "%d", m_PropNote.m_lEndTick );
		//m_editEndTick.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_ENDTICK, FALSE);
		m_spinEndTick.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_ENDTICK, TRUE, m_PropNote.m_lEndTick);
		m_spinEndTick.EnableWindow(TRUE);
	}

	if ( m_PropNote.m_dwUndetermined & UD_DURBAR )
	{
		//strTmp.Format( "%d", m_PropNote.m_lDurBar );
		//m_editDurBar.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_DURBAR, FALSE);
		m_spinDurBar.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_DURBAR, TRUE, m_PropNote.m_lDurBar);
		m_spinDurBar.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_DURBEAT )
	{
		//strTmp.Format( "%d", m_PropNote.m_lDurBeat );
		//m_editDurBeat.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_DURBEAT, FALSE);
		m_spinDurBeat.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_DURBEAT, TRUE, m_PropNote.m_lDurBeat);
		m_spinDurBeat.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_DURGRID )
	{
		//strTmp.Format( "%d", m_PropNote.m_lDurGrid );
		//m_editDurGrid.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_DURGRID, FALSE);
		m_spinDurGrid.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_DURGRID, TRUE, m_PropNote.m_lDurGrid);
		m_spinDurGrid.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_DURTICK )
	{
		//strTmp.Format( "%d", m_PropNote.m_lDurTick );
		//m_editDurTick.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_DURTICK, FALSE);
		m_spinDurTick.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_DURTICK, TRUE, m_PropNote.m_lDurTick);
		m_spinDurTick.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_DURRANGE )
	{
		//PosToRangeText( m_PropNote.m_bDurRange, strTmp );
		//m_editHumDuration.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_HUMDURATION, FALSE);
		m_spinHumDuration.EnableWindow(TRUE);
	}
	else
	{
		PosToRangeText( m_PropNote.m_bDurRange, strTmp );
		EnableItem(IDC_NOTE_EDIT_HUMDURATION, TRUE, _ttol(strTmp));
		m_spinHumDuration.EnableWindow(TRUE);
		m_spinHumDuration.SetPos(m_PropNote.m_bDurRange);
	}

	if ( m_PropNote.m_dwUndetermined & UD_VELOCITY )
	{
		//strTmp.Format( "%d", m_PropNote.m_bVelocity );
		//m_editVelocity.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_VELOCITY, FALSE);
		m_spinVelocity.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_VELOCITY, TRUE, m_PropNote.m_bVelocity);
		m_spinVelocity.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_VELRANGE )
	{
		//strTmp.Format( "%d", m_PropNote.m_bVelRange );
		//m_editHumVel.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_HUMVELOCITY, FALSE);
		m_spinHumVelocity.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_HUMVELOCITY, TRUE, m_PropNote.m_bVelRange);
		m_spinHumVelocity.EnableWindow(TRUE);
	}

	if ( m_PropNote.m_dwUndetermined & UD_PLAYMODE )
	{
		m_comboPlayMode.SetCurSel( -1 );
		m_comboPlayMode.EnableWindow(TRUE);
	}
	else
	{
		static int aiNewSel[17] = {
			 7, // 0 DMUS_PLAYMODE_FIXED
			 8, // 1 DMUS_PLAYMODE_KEY_ROOT (DMUS_PLAYMODE_FIXEDTOKEY, DMUS_PLAYMODE_FIXEDTOSCALE)
			 9, // 2 DMUS_PLAYMODE_CHORD_ROOT (DMUS_PLAYMODE_FIXEDTOCHORD)
			-1, // 3
			-1, // 4 DMUS_PLAYMODE_SCALE_INTERVALS
			 4, // 5 DMUS_PLAYMODE_PEDALPOINT
			 3, // 6 DMUS_PLAYMODE_MELODIC
			-1, // 7
			-1, // 8 DMUS_PLAYMODE_CHORD_INTERVALS
			 5, // 9 DMUS_PLAYMODE_PEDALPOINTCHORD  
			 2, // 10 DMUS_PLAYMODE_NORMALCHORD
			-1, // 11
			-1, // 12
			 6, // 13 DMUS_PLAYMODE_PEDALPOINTALWAYS
			 1, // 14 DMUS_PLAYMODE_ALWAYSPLAY (DMUS_PLAYMODE_PURPLEIZED)
			-1, // 15
			 0};// 16 DMUS_PLAYMODE_NONE

		int iCurSel = m_comboPlayMode.GetCurSel();

		ASSERT( m_PropNote.m_bPlayMode < 17 );
		if( m_PropNote.m_bPlayMode > 16 )
		{
			m_PropNote.m_bPlayMode = 16;
		}

		if( iCurSel != aiNewSel[m_PropNote.m_bPlayMode] )
		{
			m_comboPlayMode.SetCurSel( aiNewSel[m_PropNote.m_bPlayMode] );
		}

		m_comboPlayMode.EnableWindow(TRUE);
	}

	// TODO: This is slow.  Optimize so this only happens when the ComboBox is dropped down
	FillInversionIdComoboBox();
	if ( m_PropNote.m_dwUndetermined & UD_INVERSIONID )
	{
		m_comboInversionId.SetCurSel( -1 );
		m_comboInversionId.EnableWindow(TRUE);
	}
	else
	{
		m_comboInversionId.SetCurSel( m_PropNote.m_bInversionId );
		m_comboInversionId.EnableWindow(TRUE);
	}

	// Update MIDI value edit box
	if ( (m_PropNote.m_dwUndetermined & UD_SCALEVALUE) ||
		 (m_PropNote.m_dwUndetermined & UD_ACCIDENTAL) )
	{
		if( !(m_dwChanging & CHGD_MIDIVALUE) )
		{
			//m_editMidiNote.SetWindowText( strTmp );
			m_spinMidiNote.SetPos( m_PropNote.m_bMIDIValue );
		}

		EnableItem(IDC_NOTE_EDIT_MIDINOTE, FALSE);
		m_spinMidiNote.EnableWindow(TRUE);
	}
	else
	{
		lTemp = m_PropNote.m_bMIDIValue;
		if( !(m_dwChanging & CHGD_MIDIVALUE) )
		{
			strTmp = CString(g_MidiValueToName).Mid(lTemp*2, 2);
			m_editMidiNote.SetWindowText( strTmp );
			m_spinMidiNote.SetPos(lTemp);
		}

		EnableItem(IDC_NOTE_EDIT_MIDINOTE, TRUE);
		m_spinMidiNote.EnableWindow(TRUE);
	}

	// Update ScaleValue edit box
	if ( m_PropNote.m_dwUndetermined & UD_SCALEVALUE )
	{
		//strTmp.Format( "%d", m_PropNote.m_bScaleValue );
		//m_editScaleFunc.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_SCALEFUNC, FALSE);
		m_spinScaleFunc.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_SCALEFUNC, TRUE, m_PropNote.m_bScaleValue);
		m_spinScaleFunc.EnableWindow(TRUE);
	}

	// Update Accidental edit box
	if ( m_PropNote.m_dwUndetermined & UD_ACCIDENTAL )
	{
		//strTmp = CString( g_pstrUnknown );
		//m_editAccidental.SetWindowText( strTmp );
		m_spinAccidental.SetPos( m_PropNote.m_cAccidental );

		EnableItem(IDC_NOTE_EDIT_ACCIDENTAL, FALSE);
		m_spinAccidental.EnableWindow(FALSE);
		//m_spinAccidental.EnableWindow(TRUE);
	}
	else
	{
		lTemp = m_PropNote.m_cAccidental + 8;
		strTmp = CString( g_AccidentalToName ).Mid(lTemp*2, 2);
		m_spinAccidental.SetPos( m_PropNote.m_cAccidental );

		EnableItem(IDC_NOTE_EDIT_ACCIDENTAL, TRUE);
		m_editAccidental.SetWindowText( strTmp );
		m_spinAccidental.EnableWindow(TRUE);
	}

	// Update diatonic offset edit box
	if ( m_PropNote.m_dwUndetermined & UD_DIATONICOFFSET )
	{
		//strTmp = CString( g_pstrUnknown );
		//m_editDiatonicOffset.SetWindowText( strTmp );
		m_spinDiatonicOffset.SetPos( m_PropNote.m_cDiatonicOffset );

		EnableItem(IDC_NOTE_EDIT_DIATONIC_OFFSET, FALSE);
		m_spinDiatonicOffset.EnableWindow(FALSE);
		//m_spinDiatonicOffset.EnableWindow(TRUE);
	}
	else
	{
		lTemp = m_PropNote.m_cDiatonicOffset + 8;
		strTmp = CString( g_DiatonicOffsetToName ).Mid(lTemp*2, 2);
		m_spinDiatonicOffset.SetPos( m_PropNote.m_cDiatonicOffset );

		EnableItem(IDC_NOTE_EDIT_DIATONIC_OFFSET, TRUE);
		m_editDiatonicOffset.SetWindowText( strTmp );
		m_spinDiatonicOffset.EnableWindow(TRUE);
	}

	if ( m_PropNote.m_dwUndetermined & UD_OCTAVE )
	{
		//strTmp.Format( "%d", m_PropNote.m_bOctave );
		//m_editOctave.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_OCTAVE, FALSE);
		m_spinOctave.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_OCTAVE, TRUE, m_PropNote.m_bOctave);
		m_spinOctave.EnableWindow(TRUE);
	}

	if ( m_PropNote.m_dwUndetermined & UD_CHORDBAR )
	{
		//strTmp.Format( "%d", m_PropNote.m_lChordBar );
		//m_editChordBar.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_CHORDBAR, FALSE);
		m_spinChordBar.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_CHORDBAR, TRUE, m_PropNote.m_lChordBar);
		m_spinChordBar.EnableWindow(TRUE);
	}
	if ( m_PropNote.m_dwUndetermined & UD_CHORDBEAT )
	{
		//strTmp.Format( "%d", m_PropNote.m_lChordBeat );
		//m_editChordBeat.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_CHORDBEAT, FALSE);
		m_spinChordBeat.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_CHORDBEAT, TRUE, m_PropNote.m_lChordBeat);
		m_spinChordBeat.EnableWindow(TRUE);
	}

	//TRACE("SetNote out %d\n", ::timeGetTime());
	m_fInUpdateControls = FALSE;
}

void PropPageNote::GetNote( CPropNote* pPropNote )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	*pPropNote = m_PropNote;
}

/////////////////////////////////////////////////////////////////////////////
// PropPageNote message handlers

BOOL PropPageNote::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CNotePropPageMgr::sm_nActiveTab );

	// Refresh Controls
	m_pPageManager->RefreshData();
	
	return CPropertyPage::OnSetActive();
}

BOOL PropPageNote::OnInitDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	long	lTemp;
	CString	cstrTemp;

	CPropertyPage::OnInitDialog();
	
	// set ranges for the sliders and spin controls
	/// TICKS
	cstrTemp.Format("%d",m_lMinStartClocks);
	lTemp = cstrTemp.GetLength();
	m_spinStartTick.SetRange( m_lMinStartClocks, m_lMaxStartClocks );
	SetItemWidth(IDC_NOTE_EDIT_STARTTICK, lTemp);
	m_spinDurTick.SetRange( m_lMinStartClocks, m_lMaxStartClocks );
	SetItemWidth(IDC_NOTE_EDIT_DURTICK, lTemp);

	cstrTemp.Format("%d",m_lMinEndClocks);
	lTemp = cstrTemp.GetLength();
	m_spinEndTick.SetRange( m_lMinEndClocks, m_lMaxEndClocks );
	SetItemWidth(IDC_NOTE_EDIT_ENDTICK, lTemp);

	/// GRIDS
	cstrTemp.Format("%d",m_lMaxStartGrids);
	lTemp = cstrTemp.GetLength();
	m_spinStartGrid.SetRange(1, m_lMaxStartGrids);
	SetItemWidth(IDC_NOTE_EDIT_STARTGRID, lTemp);

	cstrTemp.Format("%d",m_lMaxStartGrids);
	lTemp = cstrTemp.GetLength();
	m_spinEndGrid.SetRange(1, m_lMaxEndGrids);
	SetItemWidth(IDC_NOTE_EDIT_ENDGRID, lTemp);

	cstrTemp.Format("%d",m_lMaxStartGrids - 1);
	lTemp = cstrTemp.GetLength();
	m_spinDurGrid.SetRange(0, m_lMaxStartGrids-1);
	SetItemWidth(IDC_NOTE_EDIT_DURGRID, lTemp);

	/// BEATS
	cstrTemp.Format("%d",m_lMaxStartBeats);
	lTemp = cstrTemp.GetLength();
	m_spinStartBeat.SetRange(1, m_lMaxStartBeats);
	SetItemWidth(IDC_NOTE_EDIT_STARTBEAT, lTemp);

	cstrTemp.Format("%d",m_lMaxEndBeats);
	lTemp = cstrTemp.GetLength();
	m_spinEndBeat.SetRange(1, m_lMaxEndBeats);
	SetItemWidth(IDC_NOTE_EDIT_ENDBEAT, lTemp);

	cstrTemp.Format("%d",m_lMaxStartBeats - 1);
	lTemp = cstrTemp.GetLength();
	m_spinDurBeat.SetRange(0, m_lMaxStartBeats - 1);
	SetItemWidth(IDC_NOTE_EDIT_DURBEAT, lTemp);

	cstrTemp.Format("%d",m_lMaxChordBeats);
	lTemp = cstrTemp.GetLength();
	m_spinChordBeat.SetRange(0, m_lMaxChordBeats);
	SetItemWidth(IDC_NOTE_EDIT_CHORDBEAT, lTemp);

	/// BARS
	cstrTemp.Format("%d",m_lMaxMeasures + 1);
	lTemp = cstrTemp.GetLength();
	m_spinStartBar.SetRange(0, m_lMaxMeasures + 1);	// We can have pick-up notes
	SetItemWidth(IDC_NOTE_EDIT_STARTBAR, lTemp);
	m_spinEndBar.SetRange(0, DURATION_MAXBAR);	// We can have pick-up notes
	SetItemWidth(IDC_NOTE_EDIT_ENDBAR, DURATION_MAXBARWIDTH);
	m_spinDurBar.SetRange(0, DURATION_MAXBAR);
	SetItemWidth(IDC_NOTE_EDIT_DURBAR, DURATION_MAXBARWIDTH);
	cstrTemp.Format("%d",m_lMaxMeasures);
	lTemp = cstrTemp.GetLength();
	m_spinChordBar.SetRange(1, m_lMaxMeasures);
	SetItemWidth(IDC_NOTE_EDIT_CHORDBAR, lTemp);

	PosToRangeText( HUMANIZE_MAX, cstrTemp );
	lTemp = cstrTemp.GetLength();
	m_spinHumStart.SetRange(0, HUMANIZE_MAX);
	SetItemWidth(IDC_NOTE_EDIT_HUMSTART, lTemp);
	m_spinHumDuration.SetRange(0, HUMANIZE_MAX);
	SetItemWidth(IDC_NOTE_EDIT_HUMDURATION, lTemp);

	cstrTemp.Format("%d", HUMANIZE_VELOCITY_MAX);
	lTemp = cstrTemp.GetLength();
	m_spinHumVelocity.SetRange(0, HUMANIZE_VELOCITY_MAX);
	SetItemWidth(IDC_NOTE_EDIT_HUMVELOCITY, lTemp);

	m_spinMidiNote.SetRange(0, 11);
	m_spinAccidental.SetRange(-7, 7);
	m_spinDiatonicOffset.SetRange(-7, 7);
	m_spinOctave.SetRange(0, 10);
	SetItemWidth(IDC_NOTE_EDIT_OCTAVE, 2);
	m_spinScaleFunc.SetRange(1, SCALEFUNC_MAX);
	SetItemWidth(IDC_NOTE_EDIT_SCALEFUNC, 1);

	m_spinVelocity.SetRange(1, 127);
	SetItemWidth(IDC_NOTE_EDIT_VELOCITY, 3);

	// Add the items to the PlayMode Combobox
	m_comboPlayMode.ResetContent();
	for( lTemp = IDS_PLAYMODE_NONE;  lTemp <= IDS_PLAYMODE_FIXED_TOCHORD;  lTemp++ )
	{
		if( cstrTemp.LoadString( lTemp ) )
		{
			m_comboPlayMode.AddString( cstrTemp );
		}
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PropPageNote::UpdatePPO()
{
	NotePropData* pNotePropData;
	//ASSERT(m_pIPPO != NULL);
	if( m_pIPPO && SUCCEEDED ( m_pIPPO->GetData( (void **)&pNotePropData ) ) )
	{
		if( pNotePropData )
		{
			pNotePropData->m_PropNote.Copy( &m_PropNote );
			m_pIPPO->SetData( (void *)pNotePropData);
		}
	}
}

void PropPageNote::OnChangeNoteEditHumduration() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_spinHumDuration.GetSafeHwnd() != NULL)
	{
		TCHAR tcstrTmp[DIALOG_LEN];
		if (GetDlgItemText(IDC_NOTE_EDIT_HUMDURATION, tcstrTmp, DIALOG_LEN))
		{
			ValidateHumDuration( tcstrTmp );
		}
	}
}

void PropPageNote::OnChangeNoteEditHumstart() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_spinHumStart.GetSafeHwnd() != NULL)
	{
		TCHAR tcstrTmp[DIALOG_LEN];
		if (GetDlgItemText(IDC_NOTE_EDIT_HUMSTART, tcstrTmp, DIALOG_LEN))
		{
			ValidateHumStart( tcstrTmp );
		}
	}
}

void PropPageNote::OnChangeNoteEditHumvelocity() 
{
	HandleEditChangeByte(m_spinHumVelocity, CHGD_VELRANGE,
		m_PropNote.m_bVelRange);
}

void PropPageNote::OnSelchangeComboPlaymode() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_comboPlayMode.GetSafeHwnd() != NULL)
	{
		BYTE bPlayMode = DMUS_PLAYMODE_NONE;

		switch( m_comboPlayMode.GetCurSel() )
		{
		case 0: // Default
			//bPlayMode = DMUS_PLAYMODE_NONE;
			break;
		case 1: // Chord/Scale
			bPlayMode = DMUS_PLAYMODE_PURPLEIZED;
			break;
		case 2: // Chord
			bPlayMode = DMUS_PLAYMODE_NORMALCHORD;
			break;
		case 3: // Scale
			bPlayMode = (DMUS_PLAYMODE_SCALE_INTERVALS | DMUS_PLAYMODE_CHORD_ROOT);
			break;
		case 4: // PedalPoint
			bPlayMode = DMUS_PLAYMODE_PEDALPOINT;
			break;
		case 5: // PedalPoint Chord
			bPlayMode = DMUS_PLAYMODE_PEDALPOINTCHORD;
			break;
		case 6: // PedalPoint Always
			bPlayMode = DMUS_PLAYMODE_PEDALPOINTALWAYS;
			break;
		case 7: // Fixed Absolute
			bPlayMode = DMUS_PLAYMODE_FIXED;
			break;
		case 8: // Fixed to Scale
			bPlayMode = DMUS_PLAYMODE_FIXEDTOSCALE;
			break;
		case 9: // Fixed to Chord
			bPlayMode = DMUS_PLAYMODE_FIXEDTOCHORD;
			break;
		case CB_ERR:
			// None selected
		default:
			ASSERT( FALSE );
			break;
		}

		if ((m_PropNote.m_bPlayMode != bPlayMode) || (m_PropNote.m_dwUndetermined & UD_PLAYMODE))
		{
			// The PropNote code handles changing the note's MusicValue is we're changing
			// fixed <-> music.  We'll be updated because the MIDIMgr's SetData() function
			// call the NotePropPageMgr's Refresh() method, which updates us.
			m_PropNote.m_bPlayMode = bPlayMode;
			m_PropNote.m_dwUndetermined = ~UD_PLAYMODE; // We're only updating the PlayMode

			// Notify the PPO that the Playmode changed
			m_PropNote.m_dwChanged = CHGD_PLAYMODE;

			UpdatePPO();
		}
	}
}

void PropPageNote::OnKillfocusNoteEditHumduration() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_spinHumDuration.GetSafeHwnd() != NULL)
	{
		TCHAR tcstrTmp[DIALOG_LEN];
		GetDlgItemText(IDC_NOTE_EDIT_HUMDURATION, tcstrTmp, DIALOG_LEN);
		ValidateHumDuration( tcstrTmp );
	}
}

void PropPageNote::OnKillfocusNoteEditHumstart() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_spinHumStart.GetSafeHwnd() != NULL)
	{
		TCHAR tcstrTmp[DIALOG_LEN];
		GetDlgItemText(IDC_NOTE_EDIT_HUMSTART, tcstrTmp, DIALOG_LEN);
		ValidateHumStart( tcstrTmp );
	}
}

void PropPageNote::OnKillfocusNoteEditHumvelocity() 
{
	HandleKillFocusByte(m_spinHumVelocity, CHGD_VELRANGE,
		m_PropNote.m_bVelRange);
}

long PropPageNote::RangeTextToPos( LPCTSTR szText )
{
	long lSpinPos = 0;
	long lTextValue = _ttol( szText );

	// 501-1650 by 50's
	if( lTextValue > 500 )
	{
		lSpinPos += (lTextValue - 500) / 50;
		lTextValue = 500;
	}

	// 301-500 by 10's
	if( lTextValue > 300 )
	{
		lSpinPos += (lTextValue - 300) / 10;
		lTextValue = 300;
	}

	// 191-300 by 5's
	if( lTextValue > 190 )
	{
		lSpinPos += (lTextValue - 190) / 5;
		lTextValue = 190;
	}

	lSpinPos += lTextValue;

	return lSpinPos;
}

void PropPageNote::PosToRangeText( long lSpinPos, CString& strText )
{
	long lTextValue = 0;

	// 233-255 by 50's
	if( lSpinPos > 232 )
	{
		lTextValue += (lSpinPos - 232) * 50;
		lSpinPos = 232;
	}

	// 213-232 by 10's
	if( lSpinPos > 212 )
	{
		lTextValue += (lSpinPos - 212) * 10;
		lSpinPos = 212;
	}

	// 191-212 by 5's
	if( lSpinPos > 190 )
	{
		lTextValue += (lSpinPos - 190) * 5;
		lSpinPos = 190;
	}

	lTextValue += lSpinPos;

	strText.Format( "%d", lTextValue );
}

void PropPageNote::FillInversionIdComoboBox( void )
{
	// Don't redraw until we are finished builkding the list
	m_comboInversionId.SetRedraw( FALSE );
	
	// Remove all items
	m_comboInversionId.ResetContent();

	// Load strings
	CString cstrNone;
	cstrNone.LoadString( IDS_NONE_TEXT );

	// Insert 'None'
	m_comboInversionId.AddString( cstrNone );

	TCHAR tcstrText[100];
	CString cstrCreate; 
	cstrCreate.LoadString( IDS_CREATE_TEXT );

	// Rebuild InversionId list
	for( int i = 1 ;  i < 256 ;  i++ )
	{
		_itot( i, tcstrText, 10 );
		if( !(m_PropNote.m_adwInversionIds[i >> 5] & (1 << (i % 32))) )
		{
			_tcscat( tcstrText, cstrCreate );
		}
		m_comboInversionId.AddString( tcstrText );
	}

	// Redraw the new list
	m_comboInversionId.SetRedraw( TRUE );
}

void PropPageNote::OnDeltaposNoteSpinHumduration(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_spinHumDuration.GetSafeHwnd() == NULL )
	{
		return;
	}

	if( m_dwChanging & CHGD_MIDIVALUE )
	{
		return;
	}

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	long lPos = pNMUpDown->iPos + pNMUpDown->iDelta;

	// Update mPropNote
	if( lPos >= 0
	&&  lPos <= HUMANIZE_MAX )
	{
		m_PropNote.m_bDurRange = (BYTE)lPos;
		m_PropNote.m_dwUndetermined = ~UD_DURRANGE;

		// Notify the PPO that the DurRange changed
		m_PropNote.m_dwChanged = CHGD_DURRANGE;

		EnableItem(IDC_NOTE_EDIT_HUMDURATION, TRUE);

		UpdatePPO();
	}

	// Yes, we've handled this message, don't change the position of the spin control
	*pResult = 1;
}

void PropPageNote::OnDeltaposNoteSpinHumstart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_spinHumStart.GetSafeHwnd() == NULL )
	{
		return;
	}

	if( m_dwChanging & CHGD_MIDIVALUE )
	{
		return;
	}

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	long lPos = pNMUpDown->iPos + pNMUpDown->iDelta;

	// Update mPropNote
	if( lPos >= 0
	&&  lPos <= HUMANIZE_MAX )
	{
		m_PropNote.m_bTimeRange = (BYTE)lPos;
		m_PropNote.m_dwUndetermined = ~UD_TIMERANGE;

		// Notify the PPO that the TimeRange changed
		m_PropNote.m_dwChanged = CHGD_TIMERANGE;

		EnableItem(IDC_NOTE_EDIT_HUMSTART, TRUE);

		UpdatePPO();
	}

	// Yes, we've handled this message, don't change the position of the spin control
	*pResult = 1;
}

void PropPageNote::OnSelchangeComboInversionId() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_comboInversionId.GetSafeHwnd() != NULL )
	{
		int nCurSel = m_comboInversionId.GetCurSel();

		switch( nCurSel )
		{
			case CB_ERR:
				break;

			default:
			{
				BYTE bInversionId = (BYTE)nCurSel;

				if( (m_PropNote.m_bInversionId != bInversionId)
				||  (m_PropNote.m_dwUndetermined & UD_INVERSIONID) )
				{
					m_PropNote.m_bInversionId = bInversionId;
					m_PropNote.m_dwUndetermined = ~UD_INVERSIONID; // We're only updating the InversionId

					// Notify the PPO that the inversion ID changed
					m_PropNote.m_dwChanged = CHGD_INVERSIONID;

					UpdatePPO();
				}
				break;
			}
		}
	}
}

void PropPageNote::OnChangeNoteEditScalefunc() 
{
	HandleEditChangeByte(m_spinScaleFunc, CHGD_SCALEVALUE,
		m_PropNote.m_bScaleValue);
}

void PropPageNote::OnKillfocusNoteEditScalefunc() 
{
	HandleKillFocusByte(m_spinScaleFunc, CHGD_SCALEVALUE,
		m_PropNote.m_bScaleValue);
}

void PropPageNote::OnDeltaposNoteSpinAccidental(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (m_spinAccidental.GetSafeHwnd() == NULL)
	{
		return;
	}

	if( m_dwChanging & CHGD_MIDIVALUE )
	{
		return;
	}

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	long lTemp;

	lTemp = pNMUpDown->iPos + pNMUpDown->iDelta;

	// Limit ourself to -7 to +7
	if (lTemp > 7)
	{
		lTemp = 7;
	}
	else if (lTemp < -7)
	{
		lTemp = -7;
	}

	// update mPropNote
	m_PropNote.m_cAccidental = (signed char)lTemp;
	m_PropNote.m_dwUndetermined = ~UD_ACCIDENTAL;

	// update the displayed MidiNote character representation
	CString strTmp = CString( g_AccidentalToName ).Mid( (lTemp + 8) * 2, 2);
	m_editAccidental.SetWindowText( strTmp );

	// The the ScaleValue slider and editbox will be updated by the the MIDIMgr calling
	// the NotePropPageMgr's Refresh() method.
	EnableItem(IDC_NOTE_EDIT_ACCIDENTAL, TRUE);

	// Notify the PPO that the Accidental changed
	m_PropNote.m_dwChanged = CHGD_ACCIDENTAL;

	UpdatePPO();

	// Yes, we've handled this message, don't change the position of the spin control
	*pResult = 1;
}

void PropPageNote::OnDeltaposNoteSpinDiatonicOffset(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (m_spinDiatonicOffset.GetSafeHwnd() == NULL)
	{
		return;
	}

	if( m_dwChanging & CHGD_MIDIVALUE )
	{
		return;
	}

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	long lTemp;

	lTemp = pNMUpDown->iPos + pNMUpDown->iDelta;

	// Limit ourself to -7 to +7
	if (lTemp > 7)
	{
		lTemp = 7;
	}
	else if (lTemp < -7)
	{
		lTemp = -7;
	}

	// update mPropNote
	m_PropNote.m_cDiatonicOffset = (signed char)lTemp;
	m_PropNote.m_dwUndetermined = ~UD_DIATONICOFFSET;

	// update the displayed MidiNote character representation
	CString strTmp = CString( g_DiatonicOffsetToName ).Mid( (lTemp + 8) * 2, 2);
	m_editDiatonicOffset.SetWindowText( strTmp );

	// The the ScaleValue slider and editbox will be updated by the the MIDIMgr calling
	// the NotePropPageMgr's Refresh() method.
	EnableItem(IDC_NOTE_EDIT_DIATONIC_OFFSET, TRUE);

	// Notify the PPO that the Diatonic Offset changed
	m_PropNote.m_dwChanged = CHGD_DIATONICOFFSET;

	UpdatePPO();

	// Yes, we've handled this message, don't change the position of the spin control
	*pResult = 1;
}

void PropPageNote::ValidateHumDuration( LPCTSTR lpszTmp )
{
	// If the string is equal to the string denoting 'unknown value', return
	if ( _tcscmp( lpszTmp, g_pstrUnknown ) == 0 )
	{
		return;
	}

	// Convert from text to an integer
	long lHumDuration = RangeTextToPos(lpszTmp);

	// Ensure the value stays within bounds
	if ( lHumDuration < 0 )
	{
		lHumDuration = 0;
		SetDlgItemInt( IDC_NOTE_EDIT_HUMDURATION, lHumDuration);
	}
	else if (lHumDuration > HUMANIZE_MAX)
	{
		lHumDuration = HUMANIZE_MAX;

		CString strTmp;
		PosToRangeText( HUMANIZE_MAX, strTmp );
		SetDlgItemText( IDC_NOTE_EDIT_HUMDURATION, strTmp);
	}
	else if( lpszTmp[0] == NULL )
	{
		SetDlgItemInt( IDC_NOTE_EDIT_HUMDURATION, 0);
	}

	if( (m_PropNote.m_dwUndetermined & UD_DURRANGE) ||
		(m_PropNote.m_bDurRange != lHumDuration ) )
	{
		// Copy the value to m_PropNote
		m_PropNote.m_bDurRange = (BYTE) lHumDuration;

		// Enable the edit control
		EnableItem(IDC_NOTE_EDIT_HUMDURATION, TRUE);

		// Clear the Duration range undetermined flag
		m_PropNote.m_dwUndetermined = ~UD_DURRANGE;

		// Notify the PPO that the DurRange changed
		m_PropNote.m_dwChanged = CHGD_DURRANGE;

		// Update the notes
		UpdatePPO();
	}
}

void PropPageNote::ValidateHumStart( LPCTSTR lpszTmp )
{
	// If the string is equal to the string denoting 'unknown value', return
	if ( _tcscmp( lpszTmp, g_pstrUnknown ) == 0 )
	{
		return;
	}

	// Convert from text to an integer
	long lHumStart = RangeTextToPos(lpszTmp);

	// Ensure the value stays within bounds
	if ( lHumStart < 0 )
	{
		lHumStart = 0;
		SetDlgItemInt( IDC_NOTE_EDIT_HUMSTART, lHumStart);
	}
	else if (lHumStart > HUMANIZE_MAX)
	{
		lHumStart = HUMANIZE_MAX;

		CString strTmp;
		PosToRangeText( HUMANIZE_MAX, strTmp );
		SetDlgItemText( IDC_NOTE_EDIT_HUMSTART, strTmp);
	}
	else if( lpszTmp[0] == NULL )
	{
		SetDlgItemInt( IDC_NOTE_EDIT_HUMSTART, 0);
	}

	if( (m_PropNote.m_dwUndetermined & UD_TIMERANGE) ||
		(m_PropNote.m_bTimeRange != lHumStart ) )
	{
		// Copy the value to m_PropNote
		m_PropNote.m_bTimeRange = (BYTE) lHumStart;

		// Enable the edit control
		EnableItem(IDC_NOTE_EDIT_HUMSTART, TRUE);

		// Clear the start range undetermined flag
		m_PropNote.m_dwUndetermined = ~UD_TIMERANGE;

		// Notify the PPO that the TimeRange changed
		m_PropNote.m_dwChanged = CHGD_TIMERANGE;

		// Update the notes
		UpdatePPO();
	}
}
