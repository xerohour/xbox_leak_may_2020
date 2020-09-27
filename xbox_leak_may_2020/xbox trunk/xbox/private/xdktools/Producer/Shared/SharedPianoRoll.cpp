// SharedPianoRoll.cpp : Shared PianoRoll code between sequences and midistrips
#include "stdafx.h"
#include "SharedPianoRoll.h"
#include <dmusici.h>
#include "CurveStrip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const PIANO_KEY_COLOR PianoKeyColor[] = {
	WHITE,BLACK,WHITE,BLACK,WHITE,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,WHITE };

const int aChromToScaleSharps[12] = { 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6 };
const int aChromToScaleFlats[12] = { 0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6 };
const int aChromToSharpAccidentals[12] = { 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 };
const int aChromToFlatAccidentals[12] =  { 0,-1, 0,-1, 0, 0,-1, 0,-1, 0,-1, 0 };
const int aScaleToChromNat[7] = { 0, 2, 4, 5, 7, 9, 11 };
const int aDoubleScaleToChrom[14] = { 0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 11 };

const int aTrebleSharpKeyToPosx2[7] = { 44, 41, 45, 42, 39, 43, 40 };
const int aBassSharpKeyToPosx2[7] = { 30, 27, 31, 28, 25, 29, 26 };
const int aTrebleFlatKeyToPosx2[7] = { 40, 43, 39, 42, 38, 41, 37 };
const int aBassFlatKeyToPosx2[7] = { 26, 29, 25, 28, 24, 27, 23 };
const char MidiValueToName[61] = "C    C#/DbD    D#/EbE    F    F#/GbG    G#/AbA    A#/BbB    ";
const char ScaleToName[11] = "ACEGBDFACE";

// Drums
const char *acDrums[61] = {
"High Q",
"Slap ",
"Scratch Push",
"Scratch Pull",
"Sticks",
"Square Click",
"Click Metronome",
"Bell Metronome",
"Kick Drum 2",
"C3: Kick Drum 1",
"Side Stick",
"Snare Drum 1",
"Hand Clap",
"Snare Drum 2",
"Low Tom 2",
"Closed Hi-Hat",
"Low Tom 1",
"Pedal Hi-Hat",
"Mid Tom 2",
"Open Hi-Hat",
"Mid Tom 1",
"C4: Hi Tom 2 ",
"Crash Cymbal 1",
"Hi Tom 1",
"Ride Cymbal 1",
"Chinese Cymbal ",
"Ride Bell",
"Tambourine",
"Splash Cymbal",
"Cowbell",
"Crash Cymbal 2",
"Vibra-slap",
"Ride Cymbal 2",
"C5: Hi Bongo",
"Low Bongo",
"Mute Hi Conga",
"Open Hi Conga",
"Low Conga",
"Hi Timbale",
"Low Timbale",
"Hi Agogo",
"Low Agogo",
"Cabasa",
"Maracas",
"Short Whistle",
"C6: Long Whistle",
"Short Guiro",
"Long Guiro",
"Claves",
"Hi Woodblock",
"Low Woodblock",
"Mute Cuica",
"Open Cuica",
"Mute Triangle",
"Open Triangle",
"Shaker",
"Jingle Bell",
"C7: Belltree",
"Castanets",
"Mute Surdo",
"Open Surdo" };

int MIDIToHybridPos( BYTE bMIDIValue, const int *paChromToScale )
{
	return (bMIDIValue / 12) * 7 + paChromToScale[bMIDIValue % 12];
}

void ComputeNoteMarkerVerticalRect( RECT &rectMark, BOOL fHybridNotation, BOOL fDisplayingFlats, long lMaxNoteHeight, long lInsertVal )
{
	if( fHybridNotation )
	{
		int iHybridPos;
		// FLATS
		if( fDisplayingFlats )
		{
			iHybridPos = MIDIToHybridPos( (BYTE)lInsertVal, aChromToScaleFlats);
		}
		// SHARPS
		else
		{
			iHybridPos = MIDIToHybridPos( (BYTE)lInsertVal, aChromToScaleSharps);
		}

		rectMark.bottom = ((76 - iHybridPos) * lMaxNoteHeight) >> 1;
		
		// Find the top of the note's rectangle
		// Note: (m_lMaxNoteHeight - m_lMaxNoteHeight / 2) != m_lMaxNoteHeight / 2 because of rounding errors
		rectMark.bottom -= (lMaxNoteHeight - lMaxNoteHeight / 2) / 2;
		rectMark.top = rectMark.bottom - (lMaxNoteHeight / 2);
		
		// Ensure the note is at least one pixel high
		if( rectMark.top == rectMark.bottom )
		{
			rectMark.top--;
		}
	}
	else
	{
		rectMark.top = (127 - lInsertVal) * lMaxNoteHeight + lMaxNoteHeight / 4;

		// Find the bottom of the mark's rectangle
		if( (lMaxNoteHeight / 2) != 0 )
		{
			rectMark.bottom = rectMark.top + (lMaxNoteHeight / 2);
		}
		else
		{
			rectMark.bottom = rectMark.top + 1;
		}
	}
}

void DrawHybridLinesHelper( HDC hDC, const RECT &rectClip, int nOldBottomNote, int nOldTopNote, COLORREF crWhite, long lMaxNoteHeight, bool fDrawWhite )
{
	HPEN hpenLine = ::CreatePen( PS_SOLID, 1, RGB(0, 0, 0) );
	if( !hpenLine )
	{
		return;
	}

	HPEN hpenGreyLine = ::CreatePen( PS_SOLID, 1, RGB(200, 200, 200) );
	if( !hpenGreyLine )
	{
		::DeleteObject( hpenLine );
		return;
	}

	// Set background to white
	if( fDrawWhite )
	{
		COLORREF crOldBkColor = ::GetBkColor( hDC );
		::SetBkColor( hDC, ::GetNearestColor( hDC, crWhite ) );
		::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectClip, NULL, 0, NULL);
		::SetBkColor( hDC, crOldBkColor );
	}

	// Save the current pen
	HPEN hpenOld = static_cast<HPEN> (::SelectObject( hDC, hpenLine ));

	// Draw the black lines of the staves
	int nBottomNote = max( nOldBottomNote, 13 );
	int nTopNote = min( nOldTopNote, 23 );

	int nVertPos = (38 - nBottomNote) * ( lMaxNoteHeight );
	for( int nValue = nBottomNote; nValue <= nTopNote; nValue++ )
	{
		// Skip middle C
		if( nValue == 18 )
		{
			nVertPos -= lMaxNoteHeight;
			continue;
		}

		::MoveToEx( hDC, rectClip.left, nVertPos, NULL );
		::LineTo( hDC, rectClip.right, nVertPos );
		nVertPos -= lMaxNoteHeight;
	}

	// Draw the grey lines outside of the staves
	::SelectObject( hDC, hpenGreyLine );
	nVertPos = (38 - nOldBottomNote) * ( lMaxNoteHeight );
	for( nValue = nOldBottomNote; nValue <= nOldTopNote; nValue++ )
	{
		// Skip the staves
		if( (nValue != 18) && (nValue >= 13) && (nValue <= 23) )
		{
			nVertPos -= lMaxNoteHeight;
			continue;
		}

		::MoveToEx( hDC, rectClip.left, nVertPos, NULL );
		::LineTo( hDC, rectClip.right, nVertPos );
		nVertPos -= lMaxNoteHeight;
	}

	::SelectObject( hDC, hpenOld );
	::DeleteObject( hpenLine );
	::DeleteObject( hpenGreyLine );
}

void DrawHorizontalLinesHelper( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote, COLORREF crWhite, COLORREF crBlack, long lMaxNoteHeight, bool fDrawWhite )
{
	HPEN hpenNoteLine = ::CreatePen( PS_SOLID, 1, RGB(0, 0, 0) );
	if( !hpenNoteLine )
	{
		return;
	}

	HPEN hpenOld = static_cast<HPEN> (::SelectObject( hDC, hpenNoteLine ));

	RECT rectNote;
	rectNote.left = rectClip.left;
	rectNote.right = rectClip.right;

	COLORREF crOldBkColor = ::GetBkColor( hDC );

	// Set background to white (this takes care of all the white keys)
	if( fDrawWhite )
	{
		::SetBkColor( hDC, ::GetNearestColor( hDC, crWhite ) );
		::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectClip, NULL, 0, NULL);
	}

	// Now, draw all the black keys
	::SetBkColor( hDC, ::GetNearestColor( hDC, crBlack ) );
	for( int nValue = nBottomNote; nValue <= nTopNote; nValue++ )
	{
		// Compute the top of the note
		rectNote.top = ( 127 - nValue ) * ( lMaxNoteHeight );

		// If the top is below the display, continue
		// TODO: Remove this check
		if( rectNote.top > rectClip.bottom )
		{
			ASSERT(FALSE);
			continue;
		}

		// Compute the bottom of the note
		rectNote.bottom = rectNote.top + ( lMaxNoteHeight );

		// If the bottom is above the display, break out of the loop (since we're done)
		// TODO: Remove this check
		if( rectNote.bottom < rectClip.top )
		{
			ASSERT(FALSE);
			break;
		}

		rectNote.top--;
		rectNote.bottom--;

		if( PianoKeyColor[ nValue % 12 ] == BLACK )
		{
			// Copied from the implementation of CDC::FillSolidRect (the SetBkColor is above)
			::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rectNote, NULL, 0, NULL);
		}
		
		::MoveToEx( hDC, rectNote.left, rectNote.top, NULL );
		::LineTo( hDC, rectNote.right, rectNote.top );
	}
	::SetBkColor( hDC, crOldBkColor );
	::SelectObject( hDC, hpenOld );
	::DeleteObject( hpenNoteLine );
}

BYTE CurveTypeToStripCCType( BYTE bType, BYTE bCCData )
{
	switch( bType )
	{
		case DMUS_CURVET_PBCURVE:
			return CCTYPE_PB_CURVE_STRIP;
			break;

		case DMUS_CURVET_CCCURVE:
			ASSERT( (bCCData >= 0)  &&  (bCCData <= 127 ) );
			return bCCData;
			break;

		case DMUS_CURVET_MATCURVE:
			return CCTYPE_MAT_CURVE_STRIP;
			break;

		case DMUS_CURVET_PATCURVE:
			return CCTYPE_PAT_CURVE_STRIP;
			break;

		case DMUS_CURVET_RPNCURVE:
			return CCTYPE_RPN_CURVE_STRIP;
			break;

		case DMUS_CURVET_NRPNCURVE:
			return CCTYPE_NRPN_CURVE_STRIP;
			break;

		default:
			ASSERT( FALSE );	// Should not happen
			break;
	}

	return 0xFF;
}

BYTE StripCCTypeToCurveType( BYTE bStripCCType )
{
	if( bStripCCType >= 0
	&&  bStripCCType <= 127 )
	{
		return DMUS_CURVET_CCCURVE;
	}
	else
	{
		switch( bStripCCType )
		{
			case CCTYPE_PB_CURVE_STRIP:
				return DMUS_CURVET_PBCURVE;
				break;

			case CCTYPE_MAT_CURVE_STRIP:
				return DMUS_CURVET_MATCURVE;
				break;

			case CCTYPE_PAT_CURVE_STRIP:
				return DMUS_CURVET_PATCURVE;
				break;

			case CCTYPE_RPN_CURVE_STRIP:
				return DMUS_CURVET_RPNCURVE;
				break;

			case CCTYPE_NRPN_CURVE_STRIP:
				return DMUS_CURVET_NRPNCURVE;
				break;

			default:
				ASSERT( FALSE );	// Should not happen
				break;
		}
	}

	return 0xFF;
}

WORD GetCCTypeSortValue( BYTE bCCType, WORD wRPNType )
{
	WORD wCCTypeSortValue = bCCType;

	if( wCCTypeSortValue >= 128 )
	{
		wCCTypeSortValue -= 128; 
	}
	else
	{
		wCCTypeSortValue += NBR_EXTRA_CCS;
	}

	if( bCCType == CCTYPE_RPN_CURVE_STRIP )
	{
		return WORD(NBR_EXTRA_CCS + wRPNType + 128);
	}
	else if( bCCType == CCTYPE_NRPN_CURVE_STRIP )
	{
		return WORD(NBR_EXTRA_CCS + wRPNType + 128 + 0x4000);
	}

	return wCCTypeSortValue;
}

DWORD PChannelToStripIndex( DWORD dwPChannel )
{
	switch( dwPChannel )
	{
	case DMUS_PCHANNEL_BROADCAST_SEGMENT:
		return 0xFFFFFFFF; // -1
	case DMUS_PCHANNEL_BROADCAST_AUDIOPATH:
		return 0xFFFFFFFE; // -2
	case DMUS_PCHANNEL_BROADCAST_GROUPS:
		return 0xFFFFFFFD; // -3
	case DMUS_PCHANNEL_BROADCAST_PERFORMANCE:
		return 0xFFFFFFFC; // -4
	default:
		return dwPChannel;
	}
}
