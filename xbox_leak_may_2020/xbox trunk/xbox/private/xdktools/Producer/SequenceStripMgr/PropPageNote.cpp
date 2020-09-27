// PropPageNote.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropPageNote.h"
#include "LockoutNotification.h"
#include "NotePropPageMgr.h"
#include "SequenceMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DURATION_MAXBAR			32767
#define DURATION_MAXBARWIDTH	5
const cbKeySize = 2;
const TCHAR SharpKey[12*cbKeySize+1] = _T("C C#D D#E F F#G G#A A#B ");
const TCHAR FlatKey[12*cbKeySize +1] = _T("C DbD EbE F GbG AbA BbB ");

/*
When editing a note, we pass back to the MIDIMgr a PropNote that only contains
the changes just made.  We let the MIDIMgr update us with a new PropNote that
propagates the changes to the other componenets (m_bOctave, m_bScalevalue, start/end/dur times, etc.)
This way we remove our dependence on the chord/key/root that we're editing in, and the
playmode(s) of the note(s) we're editing.
*/


/////////////////////////////////////////////////////////////////////////////
// PropPageNote property page

PropPageNote::PropPageNote(CNotePropPageMgr* pPageManager) : CPropertyPage(PropPageNote::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pPageManager != NULL );
	m_pPageManager = pPageManager;

	//{{AFX_DATA_INIT(PropPageNote)
	//}}AFX_DATA_INIT
	m_lMaxStartBeats = 4;
	m_lMaxStartGrids = 4;
	m_lMinStartClocks = -(192 / 2);
	m_lMaxStartClocks = 192 + m_lMinStartClocks - 1;

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
	DDX_Control(pDX, IDC_NOTE_SPIN_CHORDBEAT, m_spinChordBeat);
	DDX_Control(pDX, IDC_NOTE_SPIN_CHORDBAR, m_spinChordBar);
	DDX_Control(pDX, IDC_NOTE_EDIT_CHORDBEAT, m_editChordBeat);
	DDX_Control(pDX, IDC_NOTE_EDIT_CHORDBAR, m_editChordBar);
	DDX_Control(pDX, IDC_NOTE_EDIT_VELOCITY, m_editVelocity);
	DDX_Control(pDX, IDC_NOTE_EDIT_STARTTICK, m_editStartTick);
	DDX_Control(pDX, IDC_NOTE_EDIT_STARTGRID, m_editStartGrid);
	DDX_Control(pDX, IDC_NOTE_EDIT_STARTBEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_NOTE_EDIT_STARTBAR, m_editStartBar);
	DDX_Control(pDX, IDC_NOTE_EDIT_OCTAVE, m_editOctave);
	DDX_Control(pDX, IDC_NOTE_EDIT_MIDINOTE, m_editMidiNote);
	DDX_Control(pDX, IDC_NOTE_EDIT_ENDTICK, m_editEndTick);
	DDX_Control(pDX, IDC_NOTE_EDIT_ENDGRID, m_editEndGrid);
	DDX_Control(pDX, IDC_NOTE_EDIT_ENDBEAT, m_editEndBeat);
	DDX_Control(pDX, IDC_NOTE_EDIT_ENDBAR, m_editEndBar);
	DDX_Control(pDX, IDC_NOTE_EDIT_DURTICK, m_editDurTick);
	DDX_Control(pDX, IDC_NOTE_EDIT_DURGRID, m_editDurGrid);
	DDX_Control(pDX, IDC_NOTE_EDIT_DURBEAT, m_editDurBeat);
	DDX_Control(pDX, IDC_NOTE_EDIT_DURBAR, m_editDurBar);
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
	DDX_Control(pDX, IDC_NOTE_SPIN_ENDTICK, m_spinEndTick);
	DDX_Control(pDX, IDC_NOTE_SPIN_ENDBEAT, m_spinEndBeat);
	DDX_Control(pDX, IDC_NOTE_SPIN_ENDBAR, m_spinEndBar);
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// BUGBUG: This call can be really slow when we get here when adding notes from
// a MIDI keyboard
void PropPageNote::SetNote( CPropSequence* pPropSequence )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	//TRACE("SetNote in %d\n", ::timeGetTime());

	// Store new values so that OnSetActive can use them
	m_PropNote.Copy( pPropSequence );
	m_PropNote.m_dwUndetermined |= UD_OFFSET | UD_DURATION;
	m_PropNote.m_mtDuration = 0;
	m_PropNote.m_nOffset = 0;

	// Make sure controls have been created
	if( ::IsWindow(m_spinStartBar.m_hWnd) == FALSE )
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

		DisableItem(IDC_NOTE_EDIT_VELOCITY);
		m_spinVelocity.EnableWindow(FALSE);

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

	if( m_PropNote.m_pSequenceMgr && m_PropNote.m_pSequenceMgr->m_pTimeline && m_PropNote.m_pSequenceMgr->m_pSequenceStrip )
	{
		LONG lMeasure, lBeat;
		if( SUCCEEDED( m_PropNote.m_pSequenceMgr->m_pTimeline->ClocksToMeasureBeat( m_PropNote.m_pSequenceMgr->m_dwGroupBits, 0, m_PropNote.m_pSequenceMgr->m_pSequenceStrip->m_mtLength, &lMeasure, NULL ) ) )
		{
			if( SUCCEEDED( m_PropNote.m_pSequenceMgr->m_pTimeline->MeasureBeatToClocks( m_PropNote.m_pSequenceMgr->m_dwGroupBits, 0, lMeasure, 0, &lBeat ) ) )
			{
				if( lBeat < m_PropNote.m_pSequenceMgr->m_pSequenceStrip->m_mtLength )
				{
					lMeasure++;
				}
			}
			// Ensure the segment is at least one measure long
			m_lMaxMeasures = max( 1, lMeasure );
		}
	}

	// Update ranges for the spin controls
	SetStartDurRanges();
	SetEndRanges();
	SetChordRanges();

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
		strTmp.Format( "%d", m_PropNote.m_lStartBeat );
		m_editStartBeat.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lStartGrid );
		m_editStartGrid.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lStartTick );
		m_editStartTick.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_STARTTICK, FALSE);
		m_spinStartTick.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_STARTTICK, TRUE, m_PropNote.m_lStartTick);
		m_spinStartTick.EnableWindow(TRUE);
	}

	// End position
	if ( m_PropNote.m_dwUndetermined & UD_ENDBAR )
	{
		strTmp.Format( "%d", m_PropNote.m_lEndBar );
		m_editEndBar.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lEndBeat );
		m_editEndBeat.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lEndGrid );
		m_editEndGrid.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lEndTick );
		m_editEndTick.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lDurBar );
		m_editDurBar.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lDurBeat );
		m_editDurBeat.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lDurGrid );
		m_editDurGrid.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lDurTick );
		m_editDurTick.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_DURTICK, FALSE);
		m_spinDurTick.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_DURTICK, TRUE, m_PropNote.m_lDurTick);
		m_spinDurTick.EnableWindow(TRUE);
	}

	if ( m_PropNote.m_dwUndetermined & UD_VELOCITY )
	{
		strTmp.Format( "%d", m_PropNote.m_bVelocity );
		m_editVelocity.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_VELOCITY, FALSE);
		m_spinVelocity.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_VELOCITY, TRUE, m_PropNote.m_bVelocity);
		m_spinVelocity.EnableWindow(TRUE);
	}

	// Update MIDI value edit box
	if ( (m_PropNote.m_dwUndetermined & UD_MIDIVALUE) )
	{
		if( !(m_dwChanging & CH_VALUE) )
		{
			m_editMidiNote.SetWindowText( strTmp );
			m_spinMidiNote.SetPos( m_PropNote.m_bMIDIValue );
		}

		EnableItem(IDC_NOTE_EDIT_MIDINOTE, FALSE);
		m_spinMidiNote.EnableWindow(TRUE);
	}
	else
	{
		lTemp = m_PropNote.m_bMIDIValue;
		if( !(m_dwChanging & CH_VALUE) )
		{
			if( m_PropNote.m_dwBits & BF_DISPLAYFLATS )
			{
				strTmp = CString(FlatKey).Mid(lTemp * cbKeySize, cbKeySize);
			}
			else
			{
				strTmp = CString(SharpKey).Mid(lTemp * cbKeySize, cbKeySize);
			}
			m_editMidiNote.SetWindowText( strTmp );
			m_spinMidiNote.SetPos(lTemp);
		}

		EnableItem(IDC_NOTE_EDIT_MIDINOTE, TRUE);
		m_spinMidiNote.EnableWindow(TRUE);
	}

	if ( m_PropNote.m_dwUndetermined & UD_OCTAVE )
	{
		strTmp.Format( "%d", m_PropNote.m_bOctave );
		m_editOctave.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lChordBar );
		m_editChordBar.SetWindowText( strTmp );
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
		strTmp.Format( "%d", m_PropNote.m_lChordBeat );
		m_editChordBeat.SetWindowText( strTmp );
		EnableItem(IDC_NOTE_EDIT_CHORDBEAT, FALSE);
		m_spinChordBeat.EnableWindow(TRUE);
	}
	else
	{
		EnableItem(IDC_NOTE_EDIT_CHORDBEAT, TRUE, m_PropNote.m_lChordBeat);
		m_spinChordBeat.EnableWindow(TRUE);
	}

	m_fInUpdateControls = FALSE;
	//TRACE("SetNote out %d\n", ::timeGetTime());
}

void PropPageNote::GetNote( CPropSequence* pPropSequence )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	*pPropSequence = m_PropNote;
}

/////////////////////////////////////////////////////////////////////////////
// PropPageNote message handlers

BOOL PropPageNote::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Refresh Controls
	SetNote( &m_PropNote );
	
	return CPropertyPage::OnSetActive();
}

BOOL PropPageNote::OnInitDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();
	
	// set ranges for the sliders and spin controls
	SetStartDurRanges();
	SetEndRanges();
	SetChordRanges();

	m_spinMidiNote.SetRange(0, 11);
	m_spinOctave.SetRange(0, 10);
	SetItemWidth(IDC_NOTE_EDIT_OCTAVE, 2);

	m_spinVelocity.SetRange(1, 127);
	SetItemWidth(IDC_NOTE_EDIT_VELOCITY, 3);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PropPageNote::UpdatePPO()
{
	if( m_pIPPO  )
	{
		m_pIPPO->SetData( (void *)&m_PropNote);
	}
}
void PropPageNote::SetStartDurRanges()
{
	// Compute Start time max/min values
	DMUS_TIMESIGNATURE ts;
	m_PropNote.GetTimeSig( m_PropNote.AbsTime(), &ts );

	long lGridClocks = ((DMUS_PPQ * 4) / ts.bBeat) / ts.wGridsPerBeat;
	m_lMaxStartBeats = ts.bBeatsPerMeasure;
	m_lMaxStartGrids = ts.wGridsPerBeat;
	m_lMinStartClocks = -(lGridClocks / 2);
	m_lMaxStartClocks = lGridClocks + m_lMinStartClocks - 1;

	/// TICKS
	long	lTemp;
	CString	cstrTemp;
	cstrTemp.Format("%d",m_lMinStartClocks);
	lTemp = cstrTemp.GetLength();
	m_spinStartTick.SetRange( m_lMinStartClocks, m_lMaxStartClocks );
	SetItemWidth(IDC_NOTE_EDIT_STARTTICK, lTemp);
	m_spinDurTick.SetRange( m_lMinStartClocks, m_lMaxStartClocks );
	SetItemWidth(IDC_NOTE_EDIT_DURTICK, lTemp);

	/// GRIDS
	cstrTemp.Format("%d",m_lMaxStartGrids);
	lTemp = cstrTemp.GetLength();
	m_spinStartGrid.SetRange(1, m_lMaxStartGrids);
	SetItemWidth(IDC_NOTE_EDIT_STARTGRID, lTemp);

	cstrTemp.Format("%d",m_lMaxStartGrids - 1);
	lTemp = cstrTemp.GetLength();
	m_spinDurGrid.SetRange(0, m_lMaxStartGrids-1);
	SetItemWidth(IDC_NOTE_EDIT_DURGRID, lTemp);

	/// BEATS
	cstrTemp.Format("%d",m_lMaxStartBeats);
	lTemp = cstrTemp.GetLength();
	m_spinStartBeat.SetRange(1, m_lMaxStartBeats);
	SetItemWidth(IDC_NOTE_EDIT_STARTBEAT, lTemp);

	cstrTemp.Format("%d",m_lMaxStartBeats - 1);
	lTemp = cstrTemp.GetLength();
	m_spinDurBeat.SetRange(0, m_lMaxStartBeats - 1);
	SetItemWidth(IDC_NOTE_EDIT_DURBEAT, lTemp);

	/// BARS
	cstrTemp.Format("%d",m_lMaxMeasures + 1);
	lTemp = cstrTemp.GetLength();
	m_spinStartBar.SetRange(0, m_lMaxMeasures + 1);	// We can have pick-up notes
	SetItemWidth(IDC_NOTE_EDIT_STARTBAR, lTemp);
	m_spinDurBar.SetRange(0, DURATION_MAXBAR);
	SetItemWidth(IDC_NOTE_EDIT_DURBAR, DURATION_MAXBARWIDTH);
}

void PropPageNote::SetEndRanges()
{
	// Compute End time max/min values
	DMUS_TIMESIGNATURE ts;
	m_PropNote.GetTimeSig( m_PropNote.AbsTime() + m_PropNote.m_mtDuration, &ts );

	long lGridClocks = ((DMUS_PPQ * 4) / ts.bBeat) / ts.wGridsPerBeat;
	m_lMaxEndBeats = ts.bBeatsPerMeasure;
	m_lMaxEndGrids = ts.wGridsPerBeat;
	m_lMinEndClocks = -(lGridClocks / 2);
	m_lMaxEndClocks = lGridClocks + m_lMinEndClocks - 1;

	/// TICKS
	long	lTemp;
	CString	cstrTemp;
	cstrTemp.Format("%d",m_lMinEndClocks);
	lTemp = cstrTemp.GetLength();
	m_spinEndTick.SetRange( m_lMinEndClocks, m_lMaxEndClocks );
	SetItemWidth(IDC_NOTE_EDIT_ENDTICK, lTemp);

	/// GRIDS
	cstrTemp.Format("%d",m_lMaxEndGrids);
	lTemp = cstrTemp.GetLength();
	m_spinEndGrid.SetRange(1, m_lMaxEndGrids);
	SetItemWidth(IDC_NOTE_EDIT_ENDGRID, lTemp);

	/// BEATS
	cstrTemp.Format("%d",m_lMaxEndBeats);
	lTemp = cstrTemp.GetLength();
	m_spinEndBeat.SetRange(1, m_lMaxEndBeats);
	SetItemWidth(IDC_NOTE_EDIT_ENDBEAT, lTemp);

	/// BARS
	m_spinEndBar.SetRange(0, DURATION_MAXBAR);	// We can have pick-up notes
	SetItemWidth(IDC_NOTE_EDIT_ENDBAR, DURATION_MAXBARWIDTH);
}

void PropPageNote::SetChordRanges()
{
	// Compute Chord time max values
	DMUS_TIMESIGNATURE ts;
	m_PropNote.GetTimeSig( m_PropNote.MusicTime(), &ts );

	m_lMaxChordBeats = ts.bBeatsPerMeasure;

	/// BEATS
	long	lTemp;
	CString	cstrTemp;
	cstrTemp.Format("%d",m_lMaxChordBeats);
	lTemp = cstrTemp.GetLength();
	m_spinChordBeat.SetRange(1, m_lMaxChordBeats);
	SetItemWidth(IDC_NOTE_EDIT_CHORDBEAT, lTemp);

	/// BARS
	cstrTemp.Format("%d",m_lMaxMeasures);
	lTemp = cstrTemp.GetLength();
	m_spinChordBar.SetRange(1, m_lMaxMeasures);
	SetItemWidth(IDC_NOTE_EDIT_CHORDBAR, lTemp);
}
