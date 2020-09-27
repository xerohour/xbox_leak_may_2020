//  The Note structure.  Use this to store the information
//  about a note in a pattern (one measure.)

#ifndef __MIDI_H_
#define __MIDI_H_

#include "PropNote.h"
#include "StripMgr.h"

#define INUM	16

#define NB_OFFSET   0x3         // Offset note up to 7 clicks early. (masks note->bits)

// this struct is used to pass values into the PropPageNote dialog
// and PropNote class
typedef struct PropPageNoteParams
{
	CDirectMusicPart* pPart;// pointer to part we belong in
	DWORD	dwVariations;	// Which variations to edit
} PropPageNoteParams;

// struct used for passing data to the piano roll property page
typedef struct PianoRollData
{
	DWORD		dwPageIndex;
	CString		cstrPartRefName;		// PartRef name
	CString		cstrPChannelName;		// PChannel name
	DWORD		dwPChannel;
	DirectMusicTimeSig	ts;
	DWORD		dwVariations;	// Which variations to display and play
	COLORREF	crUnselectedNoteColor;
	COLORREF	crSelectedNoteColor;
	COLORREF	crOverlappingNoteColor;
	COLORREF	crAccidentalColor;
	double		dblVerticalZoom;
	long		lVerticalScroll;
	STRIPVIEW	StripView;
	long		lSnapValue;		// # of clocks to move by when mouse editing
	WORD		wNbrMeasures;	// Length of the Part
	DWORD		adwVarLocks[8];	// Bit on means variation lock is in use.
	BYTE		bInvertUpper;	// inversion upper limit
	BYTE		bInvertLower;	// inversion lower limit
	BYTE		bAutoInvert;	// 0 = Manual inversion boundaries
								// 1 = Inversion boundaries set to note range
	BYTE		bPlayModeFlags;
	BYTE		bSubChordLevel;	// Tells which sub chord level this part wants
	BYTE		bVariationLockID; // Parts with the same ID lock variations.
								  // high bit is used to identify master Part
	BYTE		bRandomVariation; // when set, matching variations play in random order
								// when clear, matching variations play sequentially
	BYTE		bPad;			// Not used
	DWORD		dwExtraBars;	// Number of extra bars to display after the pattern
	BOOL		fPickupBar;		// TRUE if we should display a pick-up bar
} PianoRollData;

// this struct is used to pass data between the CNotePropPageMgr and the object
typedef struct NotePropData
{
	CPropNote			m_PropNote;
	PropPageNoteParams	m_Param;
	BOOL				m_fUpdatePatternEditor;
	NotePropData()
	{
		m_fUpdatePatternEditor = TRUE;
	}
} NotePropData;

// struct used for passing data from the piano roll to the clipboard
typedef struct 
{
	DWORD		dwPChannel;
	DirectMusicTimeSig	ts;
	DWORD		dwVariations;	// Which variations to display and play
	BYTE		bPlayModeFlags;
} PianoRollClipboardData;

#endif // __MIDI_H_