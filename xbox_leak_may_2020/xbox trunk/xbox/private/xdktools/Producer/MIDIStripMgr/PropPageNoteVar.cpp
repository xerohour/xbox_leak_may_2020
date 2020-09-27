// PropPageNoteVar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MIDIStripMgr.h"
#include "PropPageNoteVar.h"
#include "PropPageMgr.h"
#include "ioDMStyle.h"
#include "pattern.h"
#include "MIDI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char MidiValueToName[61] = "C    C#/DbD    D#/EbE    F    F#/GbG    G#/AbA    A#/BbB    ";
static const TCHAR g_pstrUnknown[11] = _T("----------");

/////////////////////////////////////////////////////////////////////////////
// PropPageNoteVar property page

PropPageNoteVar::PropPageNoteVar(CNotePropPageMgr* pPageManager) : CPropertyPage(PropPageNoteVar::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pPageManager != NULL );
	m_pPageManager = pPageManager;

	//{{AFX_DATA_INIT(PropPageNoteVar)
	//}}AFX_DATA_INIT

	m_pIPPO = NULL;
	m_dwVariation = 0;
	m_fAllDisabled = TRUE;
	m_fValidPropNote = FALSE;
	m_fNeedToDetach = FALSE;
}

PropPageNoteVar::~PropPageNoteVar()
{
}

void PropPageNoteVar::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropPageNoteVar)
	//}}AFX_DATA_MAP
}

void PropPageNoteVar::SetNote( CPropNote* pPropNote )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pPropNote != NULL );
	if( pPropNote == NULL )
	{
		return;
	}

	// Store new values so that OnSetActive can use them
	m_PropNote = *pPropNote;

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	// if m_PropNote is not valid, disable everything
	if( !m_fValidPropNote )
	{
		if( m_fAllDisabled == FALSE )
		{
			SetDlgItemText( IDC_EDIT_MIDINOTE, NULL );
			EnableItem( IDC_EDIT_MIDINOTE, FALSE );

			for( int i = IDC_CHECK_VAR1 ;  i <= IDC_CHECK_VAR32 ;  i++ )
			{
				CheckDlgButton( i, 0 );
				EnableItem( i, FALSE );
			}

			CheckDlgButton( IDC_CHECK_REGENERATE, 0 );
			EnableItem( IDC_CHECK_REGENERATE, FALSE );
			CheckDlgButton( IDC_CHECK_CUT_OFF_SCALE, 0 );
			EnableItem( IDC_CHECK_CUT_OFF_SCALE, FALSE );
			CheckDlgButton( IDC_CHECK_CUT_OFF_CHORD, 0 );
			EnableItem( IDC_CHECK_CUT_OFF_CHORD, FALSE );
			CheckDlgButton( IDC_CHECK_CUT_OFF, 0 );
			EnableItem( IDC_CHECK_CUT_OFF, FALSE );

			m_fAllDisabled = TRUE;
		}
		
		return;
	}

	m_fAllDisabled = FALSE;

	// Prevent control notifications from being dispatched during UpdateData
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;

	// MIDI note/octave text
	if( m_PropNote.m_dwUndetermined & UD_MUSICVALUE )
	{
		SetDlgItemText( IDC_EDIT_MIDINOTE, g_pstrUnknown );
		EnableItem( IDC_EDIT_MIDINOTE, TRUE );
	}
	else
	{
		long lNote;
		long lOctave;
		CString cstrMidiNote;
		CString cstrText;

		lOctave = pPropNote->m_bOctave;
		lNote = pPropNote->m_bMIDIValue % 12;
		cstrMidiNote = CString(MidiValueToName).Mid(lNote*5, 5);
		cstrMidiNote.TrimRight();
		cstrText.Format( "%s %d", cstrMidiNote, lOctave );

		SetDlgItemText( IDC_EDIT_MIDINOTE, cstrText );
		EnableItem( IDC_EDIT_MIDINOTE, TRUE );
	}

	// Variation check boxes
	int nState;
	int j;

	for( int i = IDC_CHECK_VAR1 ;  i <= IDC_CHECK_VAR32 ;  i++ )
	{
		j = i - IDC_CHECK_VAR1;

		if( m_PropNote.m_dwVarUndetermined & (1 << j) )
		{
			nState = 2;
		}
		else
		{
			if( m_PropNote.m_dwVariation & (1 << j) )
			{
				nState = 1;
			}
			else
			{
				nState = 0;
			}
		}

		CheckDlgButton( i, nState );
		EnableItem( i, TRUE );
	}

	// DMUS_NOTEF_REGENERATE
	CheckDlgButton( IDC_CHECK_REGENERATE, (m_PropNote.m_bNoteFlagsUndetermined & DMUS_NOTEF_REGENERATE) ? 2 : ((m_PropNote.m_bNoteFlags & DMUS_NOTEF_REGENERATE) ? 1 : 0) );
	EnableItem( IDC_CHECK_REGENERATE, TRUE );

	// DMUS_NOTEF_NOINVALIDATE_INSCALE
	CheckDlgButton( IDC_CHECK_CUT_OFF_SCALE, (m_PropNote.m_bNoteFlagsUndetermined & DMUS_NOTEF_NOINVALIDATE_INSCALE) ? 2 : ((m_PropNote.m_bNoteFlags & DMUS_NOTEF_NOINVALIDATE_INSCALE) ? 1 : 0) );
	EnableItem( IDC_CHECK_CUT_OFF_SCALE, TRUE );

	// DMUS_NOTEF_NOINVALIDATE_INCHORD
	CheckDlgButton( IDC_CHECK_CUT_OFF_CHORD, (m_PropNote.m_bNoteFlagsUndetermined & DMUS_NOTEF_NOINVALIDATE_INCHORD) ? 2 : ((m_PropNote.m_bNoteFlags & DMUS_NOTEF_NOINVALIDATE_INCHORD) ? 1 : 0) );
	EnableItem( IDC_CHECK_CUT_OFF_CHORD, TRUE );

	// DMUS_NOTEF_NOINVALIDATE
	CheckDlgButton( IDC_CHECK_CUT_OFF, (m_PropNote.m_bNoteFlagsUndetermined & DMUS_NOTEF_NOINVALIDATE) ? 2 : ((m_PropNote.m_bNoteFlags & DMUS_NOTEF_NOINVALIDATE) ? 1 : 0) );
	EnableItem( IDC_CHECK_CUT_OFF, TRUE );

	//UpdateData(FALSE);
	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;
}

void PropPageNoteVar::GetNote( CPropNote* pPropNote )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	*pPropNote = m_PropNote;
}

void PropPageNoteVar::SetParameters( PropPageNoteParams* pParam )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pParam != NULL );
	if (pParam == NULL)
	{
		return;
	}

	m_fValidPropNote = (pParam->pPart != NULL) ? TRUE : FALSE;
}

void PropPageNoteVar::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_pIPPO = pINewPropPageObject;
}

void PropPageNoteVar::UpdatePPO()
{
	NotePropData* pNotePropData;

	if( m_pIPPO )
	{
		if( SUCCEEDED ( m_pIPPO->GetData( (void **)&pNotePropData ) ) )
		{
			if( pNotePropData )
			{
				GetNote( &pNotePropData->m_PropNote );
				m_pIPPO->SetData( (void *)pNotePropData );
			}
		}
	}
}


BEGIN_MESSAGE_MAP(PropPageNoteVar, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageNoteVar)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageNoteVar message handlers

BOOL PropPageNoteVar::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CNotePropPageMgr::sm_nActiveTab );

	// Refresh Controls
	m_pPageManager->RefreshData();
	
	return CPropertyPage::OnSetActive();
}

int PropPageNoteVar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
	if( !FromHandlePermanent( m_hWnd ) )
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

void PropPageNoteVar::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.
	if( m_fNeedToDetach && m_hWnd )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
	}

	CPropertyPage::OnDestroy();
}

BOOL PropPageNoteVar::OnKillActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT_VALID(this);

	return TRUE;
}

BOOL PropPageNoteVar::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int nControlID = LOWORD( wParam );
	int nCommandID = HIWORD( wParam );

	if( nCommandID == BN_CLICKED
	||  nCommandID == BN_DOUBLECLICKED )
	{
		if( nControlID >= IDC_CHECK_VAR1
		&&  nControlID <= IDC_CHECK_VAR32 )
		{
			int nVariation = nControlID - IDC_CHECK_VAR1;

			switch( IsDlgButtonChecked( nControlID ) )
			{
			case 0:		// Currently unchecked
			case 2:		// Currently undetermined
				CheckDlgButton( nControlID, 1 );
				m_PropNote.m_dwVariation |= (1 << nVariation);
				break;

			case 1:		// Currently checked
				CheckDlgButton( nControlID, 0 );
				m_PropNote.m_dwVariation &= ~(1 << nVariation);
				break;
			}

			m_PropNote.m_dwVarUndetermined &= ~(1 << nVariation);
			m_PropNote.m_dwUndetermined = ~UD_VARIATIONS;
			m_PropNote.m_dwChanged = CHGD_VARIATIONS;
			UpdateData(FALSE);
			UpdatePPO();

			return TRUE;
		}

		switch( nControlID )
		{
		case IDC_CHECK_CUT_OFF:
			switch( IsDlgButtonChecked( IDC_CHECK_CUT_OFF ) )
			{
			case 0:		// Currently unchecked
			case 2:		// Currently undetermined
				CheckDlgButton( IDC_CHECK_CUT_OFF, 1 );
				m_PropNote.m_bNoteFlags |= DMUS_NOTEF_NOINVALIDATE;
				break;

			case 1:		// Currently checked
				CheckDlgButton( IDC_CHECK_CUT_OFF, 0 );
				m_PropNote.m_bNoteFlags &= ~DMUS_NOTEF_NOINVALIDATE;
				break;
			}

			m_PropNote.m_bNoteFlagsUndetermined &= ~DMUS_NOTEF_NOINVALIDATE;
			m_PropNote.m_dwUndetermined = ~UD_NOTEFLAGS;
			m_PropNote.m_dwChanged = CHGD_NOTEFLAGS;
			UpdateData(FALSE);
			UpdatePPO();

			return TRUE;
			break;

		case IDC_CHECK_CUT_OFF_SCALE:
			switch( IsDlgButtonChecked( IDC_CHECK_CUT_OFF_SCALE ) )
			{
			case 0:		// Currently unchecked
			case 2:		// Currently undetermined
				CheckDlgButton( IDC_CHECK_CUT_OFF_SCALE, 1 );
				m_PropNote.m_bNoteFlags |= DMUS_NOTEF_NOINVALIDATE_INSCALE;
				break;

			case 1:		// Currently checked
				CheckDlgButton( IDC_CHECK_CUT_OFF_SCALE, 0 );
				m_PropNote.m_bNoteFlags &= ~DMUS_NOTEF_NOINVALIDATE_INSCALE;
				break;
			}

			m_PropNote.m_bNoteFlagsUndetermined &= ~DMUS_NOTEF_NOINVALIDATE_INSCALE;
			m_PropNote.m_dwUndetermined = ~UD_NOTEFLAGS;
			m_PropNote.m_dwChanged = CHGD_NOTEFLAGS;
			UpdateData(FALSE);
			UpdatePPO();

			return TRUE;
			break;

		case IDC_CHECK_REGENERATE:
			switch( IsDlgButtonChecked( IDC_CHECK_REGENERATE ) )
			{
			case 0:		// Currently unchecked
			case 2:		// Currently undetermined
				CheckDlgButton( IDC_CHECK_REGENERATE, 1 );
				m_PropNote.m_bNoteFlags |= DMUS_NOTEF_REGENERATE;
				break;

			case 1:		// Currently checked
				CheckDlgButton( IDC_CHECK_REGENERATE, 0 );
				m_PropNote.m_bNoteFlags &= ~DMUS_NOTEF_REGENERATE;
				break;
			}

			m_PropNote.m_bNoteFlagsUndetermined &= ~DMUS_NOTEF_REGENERATE;
			m_PropNote.m_dwUndetermined = ~UD_NOTEFLAGS;
			m_PropNote.m_dwChanged = CHGD_NOTEFLAGS;
			UpdateData(FALSE);
			UpdatePPO();

			return TRUE;
			break;

		case IDC_CHECK_CUT_OFF_CHORD:
			switch( IsDlgButtonChecked( IDC_CHECK_CUT_OFF_CHORD ) )
			{
			case 0:		// Currently unchecked
			case 2:		// Currently undetermined
				CheckDlgButton( IDC_CHECK_CUT_OFF_CHORD, 1 );
				m_PropNote.m_bNoteFlags |= DMUS_NOTEF_NOINVALIDATE_INCHORD;
				break;

			case 1:		// Currently checked
				CheckDlgButton( IDC_CHECK_CUT_OFF_CHORD, 0 );
				m_PropNote.m_bNoteFlags &= ~DMUS_NOTEF_NOINVALIDATE_INCHORD;
				break;
			}

			m_PropNote.m_bNoteFlagsUndetermined &= ~DMUS_NOTEF_NOINVALIDATE_INCHORD;
			m_PropNote.m_dwUndetermined = ~UD_NOTEFLAGS;
			m_PropNote.m_dwChanged = CHGD_NOTEFLAGS;
			UpdateData(FALSE);
			UpdatePPO();

			return TRUE;
			break;
		}
	}
	
	return CPropertyPage::OnCommand( wParam, lParam );
}

void PropPageNoteVar::EnableItem(int nItem, BOOL fEnable)
{
	CWnd* pWnd = GetDlgItem(nItem);
	if (pWnd)
	{
		pWnd->EnableWindow( fEnable );
	}
}
