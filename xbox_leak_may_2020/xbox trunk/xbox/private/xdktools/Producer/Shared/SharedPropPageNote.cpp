// SharedPropPageNote.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropPageNote.h"
#include "LockoutNotification.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//static const TCHAR g_MidiValueToName[25] = _T("C DbD EbE F GbG AbA BbB ");
static const TCHAR g_pstrUnknown[11] = _T("----------");

PropPageNote::~PropPageNote()
{
}

void PropPageNote::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_pIPPO = pINewPropPageObject;
}

int PropPageNote::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void PropPageNote::OnDestroy() 
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

void PropPageNote::SetItemWidth( int nItem, int nWidth)
{
	CEdit* pEdit;
	pEdit = (CEdit *) GetDlgItem(nItem);
	if ((pEdit != NULL) && (nWidth >=0))
	{
		pEdit->LimitText( nWidth );
	}
}

BOOL PropPageNote::IsEditEmpty(CEdit* pEdit)
{
	CString csTmp;

	pEdit->GetWindowText(csTmp);
	if (csTmp.GetLength() == 0)
	{
		return FALSE;
	}
	return TRUE;
}

void PropPageNote::EnableItem(int nItem, BOOL fEnable)
{
	// prevent control notifications from being dispatched changing edit text
	CLockoutNotification LockoutNotification( m_hWnd );

	CWnd* pWnd;
	pWnd = GetDlgItem(nItem);
	if (pWnd)
	{
		BOOL fRes, fRes2;
		fRes = FALSE;
		fRes2 = pWnd->EnableWindow(TRUE);
		if ( !fEnable )
		{
			TCHAR tcstrTmp[DIALOG_LEN];
			GetDlgItemText( nItem, tcstrTmp, DIALOG_LEN );
			if ( _tcscmp( tcstrTmp, g_pstrUnknown ) != 0 )
			{
				SetDlgItemText( nItem, g_pstrUnknown );
				fRes = TRUE;
			}
		}
		/*
		if (fRes || fRes2)
		{
			RECT rect;
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			InvalidateRect(&rect);
		}
		*/
	}
}

void PropPageNote::EnableItem(int nItem, BOOL fEnable, long lValue)
{
	// Not necessary - this method is only called from SetNote, which already
	// does the notification lockout
	/*
	CLockoutNotification LockoutNotification( m_hWnd );
	*/

	CWnd* pWnd;
	pWnd = GetDlgItem(nItem);
	if (pWnd)
	{
		BOOL fRes, fRes2;
		fRes = FALSE;
		fRes2 = pWnd->EnableWindow(TRUE);
		if (fEnable)
		{
			BOOL fTrans;
			if( (lValue != (signed int) GetDlgItemInt( nItem, &fTrans )) || !fTrans  )
			{
				SetDlgItemInt( nItem, lValue );
				fRes = TRUE;
			}
			else
			{
				TCHAR tcstrTmp[DIALOG_LEN];
				GetDlgItemText( nItem, tcstrTmp, DIALOG_LEN );
				if ( (tcstrTmp[0] == NULL) || (_tcscmp( tcstrTmp, g_pstrUnknown ) == 0) )
				{
					SetDlgItemInt( nItem, lValue );
					fRes = TRUE;
				}
			}
		}
		else
		{
			TCHAR tcstrTmp[DIALOG_LEN];
			GetDlgItemText( nItem, tcstrTmp, DIALOG_LEN );
			if ( _tcscmp( tcstrTmp, g_pstrUnknown ) != 0 )
			{
				SetDlgItemText( nItem, g_pstrUnknown );
				fRes = TRUE;
			}
		}
		/*
		if (fRes || fRes2)
		{
			RECT rect;
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);
			InvalidateRect(&rect);
		}
		*/
	}
}

void PropPageNote::DisableItem(int nItem)
{
	// prevent control notifications from being dispatched changing edit text
	CLockoutNotification LockoutNotification( m_hWnd );

	CWnd* pWnd;
	//TRACE("DisableItem in %d\n", ::timeGetTime());
	pWnd = GetDlgItem(nItem);
	if (pWnd)
	{
		SetDlgItemText( nItem, g_pstrUnknown );
		// BUGBUG: This call can be really slow when we get here when adding notes
		// from a MIDI keyboard
		pWnd->EnableWindow(FALSE);
	}
	//TRACE("DisableItem out %d\n", ::timeGetTime());
}

void PropPageNote::OnChangeNoteEditDurtick() 
{
	HandleEditChange(m_spinDurTick, CHGD_DURTICK,
		m_PropNote.m_lDurTick);
}

void PropPageNote::OnChangeNoteEditDurgrid() 
{
	HandleEditChange(m_spinDurGrid, CHGD_DURGRID,
		m_PropNote.m_lDurGrid);
}

void PropPageNote::OnChangeNoteEditDurbeat() 
{
	HandleEditChange(m_spinDurBeat, CHGD_DURBEAT,
		m_PropNote.m_lDurBeat);
}

void PropPageNote::OnChangeNoteEditDurbar() 
{
	HandleEditChange(m_spinDurBar, CHGD_DURBAR,
		m_PropNote.m_lDurBar);
}

void PropPageNote::OnChangeNoteEditVelocity() 
{
	HandleEditChangeByte(m_spinVelocity, CHGD_VELOCITY,
		m_PropNote.m_bVelocity);
}

void PropPageNote::OnDeltaposNoteSpinMidinote(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (m_spinMidiNote.GetSafeHwnd() == NULL)
	{
		return;
	}

	if( m_dwChanging & CHGD_MIDIVALUE )
	{
		return;
	}

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	long lTemp;

	lTemp = m_PropNote.m_bMIDIValue + pNMUpDown->iDelta;

	// Move up or down an octave if we've wrapped around
	long lNewOctave = m_PropNote.m_bOctave;
	if (lTemp > 11)
	{
		if( !(m_PropNote.m_dwUndetermined & UD_OCTAVE) )
		{
			if( lNewOctave < 10)
			{
				// Go up an octave
				lTemp -= 12;
				lNewOctave++;
			}
			else
			{
				// Snap to 7 (G10)
				lTemp = 7;
			}
		}
		else
		{
			lTemp -= 12;
		}
	}
	else if (lTemp < 0)
	{
		if( !(m_PropNote.m_dwUndetermined & UD_OCTAVE) )
		{
			if( lNewOctave > 0)
			{
				// Go down an octave
				lTemp += 12;
				lNewOctave--;
			}
			else
			{
				// Snap to C0
				lTemp = 0;
			}
		}
		else
		{
			lTemp += 12;
		}
	}

	// Snap to G10
	if( lNewOctave * 12 + lTemp > 127 )
	{
		ASSERT( lNewOctave == 10 );
		lTemp = 7;
	}

	// update mPropNote
	BYTE bValue = BYTE(lNewOctave * 12 + lTemp);
	if ( bValue <= 127 && (lTemp != m_PropNote.m_bMIDIValue) )
	{
		m_PropNote.m_bMIDIValue = (BYTE)lTemp;
		m_PropNote.m_dwUndetermined = ~UD_MIDIVALUE;
		m_PropNote.m_dwChanged = CHGD_MIDIVALUE;

		// The text will be updated when SetNote() is called in response to UpdatePPO()
		/*
		// update the displayed MidiNote character representation
		CString strTmp = CString(g_MidiValueToName).Mid(lTemp*2, 2);
		m_editMidiNote.SetWindowText( strTmp );

		// The the ScaleValue slider and editbox and the accidntal spin controls will be updated
		// by the the MIDIMgr calling the NotePropPageMgr's Refresh() method.
		EnableItem(IDC_NOTE_EDIT_MIDINOTE, TRUE);
		*/

		if( lNewOctave != m_PropNote.m_bOctave )
		{
			m_PropNote.m_bOctave = (BYTE) lNewOctave;
			m_PropNote.m_dwUndetermined &= ~UD_OCTAVE;
			m_PropNote.m_dwChanged |= CHGD_OCTAVE;
		}

		UpdatePPO();
	}
	// Yes, we've handled this message, don't change the position of the spin control
	*pResult = 1;
}

void PropPageNote::OnKillfocusNoteEditOctave() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_spinOctave.GetSafeHwnd() != NULL)
	{
		TCHAR tcstrTmp[DIALOG_LEN];
		GetDlgItemText(IDC_NOTE_EDIT_OCTAVE, tcstrTmp, DIALOG_LEN);
		ValidateOctave( tcstrTmp );
	}
}

void PropPageNote::OnChangeNoteEditOctave() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_spinOctave.GetSafeHwnd() != NULL)
	{
		TCHAR tcstrTmp[DIALOG_LEN];
		if (GetDlgItemText(IDC_NOTE_EDIT_OCTAVE, tcstrTmp, DIALOG_LEN))
		{
			ValidateOctave( tcstrTmp );
		}
	}
}

void PropPageNote::OnChangeNoteEditChordbar() 
{
	HandleEditChange(m_spinChordBar, CHGD_CHORDBAR,
		m_PropNote.m_lChordBar);
}

void PropPageNote::OnChangeNoteEditChordbeat() 
{
	HandleEditChange(m_spinChordBeat, CHGD_CHORDBEAT,
		m_PropNote.m_lChordBeat);
}

void PropPageNote::OnChangeNoteEditStartbar() 
{
	HandleEditChange(m_spinStartBar, CHGD_STARTBAR,
		m_PropNote.m_lStartBar);
}

void PropPageNote::OnChangeNoteEditStartbeat() 
{
	HandleEditChange(m_spinStartBeat, CHGD_STARTBEAT,
		m_PropNote.m_lStartBeat);
}

void PropPageNote::OnChangeNoteEditStartgrid() 
{
	HandleEditChange(m_spinStartGrid, CHGD_STARTGRID,
		m_PropNote.m_lStartGrid);
}

void PropPageNote::OnChangeNoteEditStarttick() 
{
	HandleEditChange(m_spinStartTick, CHGD_STARTTICK,
		m_PropNote.m_lStartTick);
}

void PropPageNote::OnChangeNoteEditEndbar() 
{
	HandleEditChange(m_spinEndBar, CHGD_ENDBAR,
		m_PropNote.m_lEndBar);
}

void PropPageNote::OnChangeNoteEditEndbeat() 
{
	HandleEditChange(m_spinEndBeat, CHGD_ENDBEAT,
		m_PropNote.m_lEndBeat);
}

void PropPageNote::OnChangeNoteEditEndgrid() 
{
	HandleEditChange(m_spinEndGrid, CHGD_ENDGRID,
		m_PropNote.m_lEndGrid);
}

void PropPageNote::OnChangeNoteEditEndtick() 
{
	HandleEditChange(m_spinEndTick, CHGD_ENDTICK,
		m_PropNote.m_lEndTick);
}

void PropPageNote::OnKillfocusNoteEditVelocity() 
{
	HandleKillFocusByte(m_spinVelocity, CHGD_VELOCITY,
		m_PropNote.m_bVelocity);
}

void PropPageNote::OnKillfocusNoteEditChordbar() 
{
	HandleKillFocus(m_spinChordBar, CHGD_CHORDBAR,
		m_PropNote.m_lChordBar);
}

void PropPageNote::OnKillfocusNoteEditChordbeat() 
{
	HandleKillFocus(m_spinChordBeat, CHGD_CHORDBEAT,
		m_PropNote.m_lChordBeat);
}

void PropPageNote::OnKillfocusNoteEditDurtick() 
{
	HandleKillFocus(m_spinDurTick, CHGD_DURTICK,
		m_PropNote.m_lDurTick);
}

void PropPageNote::OnKillfocusNoteEditDurgrid() 
{
	HandleKillFocus(m_spinDurGrid, CHGD_DURGRID,
		m_PropNote.m_lDurGrid);
}

void PropPageNote::OnKillfocusNoteEditDurbeat() 
{
	HandleKillFocus(m_spinDurBeat, CHGD_DURBEAT,
		m_PropNote.m_lDurBeat);
}

void PropPageNote::OnKillfocusNoteEditDurbar() 
{
	HandleKillFocus(m_spinDurBar, CHGD_DURBAR,
		m_PropNote.m_lDurBar);
}

void PropPageNote::OnKillfocusNoteEditEndbar() 
{
	HandleKillFocus(m_spinEndBar, CHGD_ENDBAR,
		m_PropNote.m_lEndBar);
}

void PropPageNote::OnKillfocusNoteEditEndbeat() 
{
	HandleKillFocus(m_spinEndBeat, CHGD_ENDBEAT,
		m_PropNote.m_lEndBeat);
}

void PropPageNote::OnKillfocusNoteEditEndgrid() 
{
	HandleKillFocus(m_spinEndGrid, CHGD_ENDGRID,
		m_PropNote.m_lEndGrid);
}

void PropPageNote::OnKillfocusNoteEditEndtick() 
{
	HandleKillFocus(m_spinEndTick, CHGD_ENDTICK,
		m_PropNote.m_lEndTick);
}

void PropPageNote::OnKillfocusNoteEditStartbar() 
{
	HandleKillFocus(m_spinStartBar, CHGD_STARTBAR,
		m_PropNote.m_lStartBar);
}

void PropPageNote::OnKillfocusNoteEditStartbeat() 
{
	HandleKillFocus(m_spinStartBeat, CHGD_STARTBEAT,
		m_PropNote.m_lStartBeat);
}

void PropPageNote::OnKillfocusNoteEditStartgrid() 
{
	HandleKillFocus(m_spinStartGrid, CHGD_STARTGRID,
		m_PropNote.m_lStartGrid);
}

void PropPageNote::OnKillfocusNoteEditStarttick() 
{
	HandleKillFocus(m_spinStartTick, CHGD_STARTTICK,
		m_PropNote.m_lStartTick);
}

void PropPageNote::OnDeltaposNoteSpinChordbeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, CHGD_CHORDBEAT, m_PropNote.m_lChordBeat );
}

void PropPageNote::OnDeltaposNoteSpinDurbeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, CHGD_DURBEAT, m_PropNote.m_lDurBeat );
}

void PropPageNote::OnDeltaposNoteSpinDurgrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, CHGD_DURGRID, m_PropNote.m_lDurGrid );
}

void PropPageNote::OnDeltaposNoteSpinDurtick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, CHGD_DURTICK, m_PropNote.m_lDurTick );
}

void PropPageNote::OnDeltaposNoteSpinEndbeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, CHGD_ENDBEAT, m_PropNote.m_lEndBeat );
}

void PropPageNote::OnDeltaposNoteSpinEndgrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, CHGD_ENDGRID, m_PropNote.m_lEndGrid );
}

void PropPageNote::OnDeltaposNoteSpinEndtick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, CHGD_ENDTICK, m_PropNote.m_lEndTick );
}

void PropPageNote::OnDeltaposNoteSpinStartbeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, CHGD_STARTBEAT, m_PropNote.m_lStartBeat );
}

void PropPageNote::OnDeltaposNoteSpinStartgrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, CHGD_STARTGRID, m_PropNote.m_lStartGrid );
}

void PropPageNote::OnDeltaposNoteSpinStarttick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, CHGD_STARTTICK, m_PropNote.m_lStartTick );
}

BOOL PropPageNote::OnKillActive()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT_VALID(this);

	return TRUE;
}


void PropPageNote::ValidateOctave( LPCTSTR lpszTmp )
{
	// If the string is equal to the string denoting 'unknown value', return
	if ( _tcscmp( lpszTmp, g_pstrUnknown ) == 0 )
	{
		return;
	}

	// Convert from text to an integer
	long lOctave = _ttoi( lpszTmp );

	// Ensure the value stays within bounds
	if ( lOctave < 0 ) 
	{
		lOctave = 0;
		SetDlgItemInt(IDC_NOTE_EDIT_OCTAVE, lOctave);
	}
	else if ( lOctave >= 10 )
	{
		if( m_PropNote.m_dwUndetermined & UD_MIDIVALUE )
		{
			// Undetermined MIDI value, compare against upper limit of octave 10
			if( lOctave > 10 )
			{
				lOctave = 10;
				SetDlgItemInt(IDC_NOTE_EDIT_OCTAVE, lOctave);
			}
		}
		else
		{
			// Determined MIDI value - make sure the MIDI value is below 127
			long lValue;
			lValue = lOctave * 12 + m_PropNote.m_bMIDIValue;
			if( lValue > 127 )
			{
				lOctave -= (lValue - 116) / 12; // (11 + lValue - 127) / 12
				ASSERT( lOctave <= 10 );
				SetDlgItemInt(IDC_NOTE_EDIT_OCTAVE, lOctave);
			}
		}
	}
	else if( lpszTmp[0] == NULL )
	{
		SetDlgItemInt( IDC_NOTE_EDIT_OCTAVE, 0);
	}

	if( (m_PropNote.m_dwUndetermined & UD_OCTAVE) ||
		(m_PropNote.m_bOctave != lOctave ) )
	{
		// Copy the value to m_PropNote
		m_PropNote.m_bOctave = (BYTE) lOctave;

		// Enable the edit control
		EnableItem(IDC_NOTE_EDIT_OCTAVE, TRUE);

		// Clear the octave undetermined flag
		m_PropNote.m_dwUndetermined = ~UD_OCTAVE;

		// Notify the PPO that the DurRange changed
		m_PropNote.m_dwChanged = CHGD_OCTAVE;

		// Update the notes
		UpdatePPO();
	}
}

/////////////////////////////////////////////////////////////////////////////
// PropPageNote::HandleEditChange
//
// Generic handler for edit box value changes
void PropPageNote::HandleEditChange(CSpinButtonCtrl& spin,
										   DWORD dwChg,
										   long& lUpdateVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	if (!::IsWindow(spin.m_hWnd)) {
		return;
	}

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_LEN );

	// If empty, exit early
	if( tcstrTmp[0] == NULL )
	{
		return;
	}

	// Get StartBar range
	int nMin;
	int nMax;
	spin.GetRange( nMin, nMax );

	// Convert from text to an integer
	BOOL fTransSuccess;
	long lNewValue = GetDlgItemInt( pEditCtrl->GetDlgCtrlID(), &fTransSuccess, TRUE );

	// If unable to translate (if dialog contains invalid data, such as '-'), exit early
	if( !fTransSuccess )
	{
		return;
	}

	// Ensure the value stays within bounds
	if ( lNewValue < nMin )
	{
		lNewValue = nMin;
		spin.SetPos( nMin );
	}
	else if (lNewValue > nMax)
	{
		lNewValue = nMax;
		spin.SetPos( nMax );
	}
	
	// If the value changed, or it was previously undetermined - update the selected PropCurves
	if( (m_PropNote.m_dwUndetermined & dwChg) || (lNewValue != lUpdateVal) )
	{
		m_PropNote.m_dwChanged = dwChg;
		lUpdateVal = lNewValue;
		UpdatePPO();
	}
}

/////////////////////////////////////////////////////////////////////////////
// PropPageNote::HandleKillFocus
//
// Generic handler for edit box kill focus events.
void PropPageNote::HandleKillFocus(CSpinButtonCtrl& spin,
									 DWORD dwChg,
									 long& lUpdateVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	if (!::IsWindow(spin.m_hWnd)) {
		return;
	}

	// Get StartBar range
	int nMin;
	int nMax;
	spin.GetRange( nMin, nMax );

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_LEN );

	// Handle unknown text
	if( _tcsncmp( tcstrTmp, g_pstrUnknown, 10 ) == 0 )
	{
		return;
	}
	// Handle empty text
	else if( tcstrTmp[0] == NULL )
	{
		// Set it back to the minimum value (or 0, whichever is greater)
		spin.SetPos( max( nMin, 0) );

		if( (m_PropNote.m_dwUndetermined & dwChg) || (max( nMin, 0) != lUpdateVal) )
		{
			m_PropNote.m_dwChanged = dwChg;
			lUpdateVal = max( nMin, 0);
			UpdatePPO();
		}
	}
	else
	{
		// Convert from text to an integer
		BOOL fTransSuccess;
		GetDlgItemInt( pEditCtrl->GetDlgCtrlID(), &fTransSuccess, TRUE );

		// If unable to translate (if dialog contains invalid data, such as '-'), exit early
		if( !fTransSuccess )
		{
			// If conversion failed, update dialog item text
			SetDlgItemInt( pEditCtrl->GetDlgCtrlID(), lUpdateVal );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// PropPageNote::HandleEditChangeByte
//
// Generic handler for edit box value changes
void PropPageNote::HandleEditChangeByte(CSpinButtonCtrl& spin,
										   DWORD dwChg,
										   BYTE& bUpdateVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	if (!::IsWindow(spin.m_hWnd)) {
		return;
	}

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_LEN );

	// If empty, exit early
	if( tcstrTmp[0] == NULL )
	{
		return;
	}

	// Get StartBar range
	int nMin;
	int nMax;
	spin.GetRange( nMin, nMax );

	// Convert from text to an integer
	long lNewValue = _ttoi( tcstrTmp );

	// Ensure the value stays within bounds
	if ( lNewValue < nMin )
	{
		lNewValue = nMin;
		spin.SetPos( nMin );
	}
	else if (lNewValue > nMax)
	{
		lNewValue = nMax;
		spin.SetPos( nMax );
	}
	
	// If the value changed, or it was previously undetermined - update the selected PropCurves
	if( (m_PropNote.m_dwUndetermined & dwChg) || ((BYTE)lNewValue != bUpdateVal) )
	{
		m_PropNote.m_dwChanged = dwChg;
		bUpdateVal = (BYTE)lNewValue;
		UpdatePPO();
	}
}

/////////////////////////////////////////////////////////////////////////////
// PropPageNote::HandleKillFocusByte
//
// Generic handler for edit box kill focus events.
void PropPageNote::HandleKillFocusByte(CSpinButtonCtrl& spin,
									 DWORD dwChg,
									 BYTE& bUpdateVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	if (!::IsWindow(spin.m_hWnd)) {
		return;
	}

	// Get StartBar range
	int nMin;
	int nMax;
	spin.GetRange( nMin, nMax );

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_LEN );

	// Handle unknown text
	if( _tcsncmp( tcstrTmp, g_pstrUnknown, 10 ) == 0 )
	{
		return;
	}
	// Handle empty text
	else if( tcstrTmp[0] == NULL )
	{
		// Set it back to the minimum value (or 0, whichever is greater)
		spin.SetPos( max( nMin, 0) );

		if( (m_PropNote.m_dwUndetermined & dwChg) || ((BYTE)max( nMin, 0) != bUpdateVal) )
		{
			m_PropNote.m_dwChanged = dwChg;
			bUpdateVal = (BYTE)max( nMin, 0);
			UpdatePPO();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// PropPageNote::HandleDeltaChange
//
// Generic handler for deltapos changes
void PropPageNote::HandleDeltaChange(NMHDR* pNMHDR,
									 LRESULT* pResult,
									 DWORD dwChg,
									 long& lUpdateVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// If the value changed - update the selected PropNote
	if( ((NM_UPDOWN* )pNMHDR)->iDelta != 0 ) 
	{
		lUpdateVal += ((NM_UPDOWN* )pNMHDR)->iDelta;
		m_PropNote.m_dwChanged = dwChg;
		UpdatePPO();
	}
	
	*pResult = 1;
}


// Custom edit control that only accepts numbers, editing keys, and '-'
IMPLEMENT_DYNCREATE( CMyEdit, CEdit )

CMyEdit::CMyEdit() : CEdit()
{
}

LRESULT CMyEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_CHAR:
		switch( wParam )
		{
		case 8:  // Backspace
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 71: // Home
		case 75: // Left
		case 77: // Right
		case 79: // End
		case 82: // Ins
		case 83: // Del
			break;
		default:
			return TRUE;
		break;
		}
	}
	return CEdit::WindowProc( message, wParam, lParam );
}
