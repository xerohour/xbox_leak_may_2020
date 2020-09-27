// SharedPianoRoll.h : Declarations for shared PianoRoll code between sequences and midistrips

#ifndef __SHAREDPIANOROLL_H_
#define __SHAREDPIANOROLL_H_

#define MAX_NOTE_HEIGHT		100
#define HORIZ_LINE_HEIGHT	1

#define DEFAULT_VERTICAL_SCROLL 70

#define NOTATION_FONT_ZOOMFACTOR 4
#define NOTATION_FONT_CLEF_ZOOMFACTOR 7
//int nTopAdjustNumerator = 10;
//int nTopAdjustDenominator = 3;
//int nBottomAdjustNumerator = 3;
//int nBottomAdjustDenominator = 2;
#define TOPADJUST ((m_lMaxNoteHeight * 10) / 3)
#define BOTTOMADJUST ((m_lMaxNoteHeight * 3) / 2)
#define TREBLE_TOP 13
#define TREBLE_BOTTOM 21
#define BASS_TOP 19
#define BASS_BOTTOM 25

#define MINIMUM_ZOOM_LEVEL (0.02)

#define SCROLL_HORIZ_AMOUNT 10 
#define SCROLL_HORIZ_RANGE 40
#define SCROLL_VERT_AMOUNT 10
#define SCROLL_VERT_RANGE 30

#define MINIMIZE_HEIGHT		   20
#define DEFAULT_HEIGHT		  200
#define MAXIMUM_HEIGHT		 1000

#define COLOR_EARLY_NOTES RGB(230,230,230)
#define COLOR_HATCH_OVERLAPPING RGB(230,230,230)

#define COLOR_DEFAULT_UNSELECTED RGB(10,10,255)
#define COLOR_DEFAULT_SELECTED RGB(255,0,0)
#define COLOR_DEFAULT_OVERLAPPING RGB(128,0,128)
#define COLOR_DEFAULT_ACCIDENTAL RGB(30, 160, 35)

#define PIANOROLL_BLACKKEY_COLOR	RGB( 0, 0, 0 )
#define PIANOROLL_WHITEKEY_COLOR	RGB( 255, 255, 232 )
#define PIANOROLL_RED_COLOR			RGB( 255, 0, 0 )

typedef enum tagPIANO_KEY_COLOR
{
	WHITE,
	BLACK
} PIANO_KEY_COLOR;

extern const PIANO_KEY_COLOR PianoKeyColor[];

extern const int aChromToScaleSharps[12];
extern const int aChromToScaleFlats[12];
extern const int aChromToSharpAccidentals[12];
extern const int aChromToFlatAccidentals[12];
extern const int aScaleToChromNat[7];
extern const int aDoubleScaleToChrom[14];

extern const int aTrebleSharpKeyToPosx2[7];
extern const int aBassSharpKeyToPosx2[7];
extern const int aTrebleFlatKeyToPosx2[7];
extern const int aBassFlatKeyToPosx2[7];
extern const char MidiValueToName[61];
extern const char ScaleToName[11];

extern const char *acDrums[61];

int MIDIToHybridPos( BYTE bMIDIValue, const int *paChromToScale );
void ComputeNoteMarkerVerticalRect( RECT &rectMark, BOOL fHybridNotation, BOOL fDisplayingFlats, long lMaxNoteHeight, long lInsertVal );
void DrawHybridLinesHelper( HDC hDC, const RECT &rectClip, int nOldBottomNote, int nOldTopNote, COLORREF crWhite, long lMaxNoteHeight, bool fDrawWhite );
void DrawHorizontalLinesHelper( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote, COLORREF crWhite, COLORREF crBlack, long lMaxNoteHeight, bool fDrawWhite );
BYTE CurveTypeToStripCCType( BYTE bType, BYTE bCCData );
BYTE StripCCTypeToCurveType( BYTE bStripCCType );
WORD GetCCTypeSortValue( BYTE bCCType, WORD wRPNType );
DWORD PChannelToStripIndex( DWORD dwPChannel );

#endif //__SHAREDPIANOROLL_H_
