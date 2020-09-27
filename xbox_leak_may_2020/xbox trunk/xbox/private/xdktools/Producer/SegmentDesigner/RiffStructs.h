#ifndef __RIFFSTRUCTS_SEGMENT_H__
#define __RIFFSTRUCTS_SEGMENT_H__ 1

#pragma pack(2)

// *********************************************************************
// *********************************************************************
// ********************* File io for IMA 25 objects ********************
// *********************************************************************
// *********************************************************************

typedef struct ioIMACommand
{
	long	lTime;		// Time, in clocks.
	DWORD	dwCommand;	// Command type.
} ioIMACommand;


typedef struct ioIMAChord
{
	long	lChordPattern;	// pattern that defines chord
	long	lScalePattern;	// scale pattern for the chord
	long	lInvertPattern; // inversion pattern
	BYTE	bRoot;		// root note of chord
	BYTE	bReserved;	// expansion room
	WORD	wCFlags;	// bit flags
	long	lReserved;	// expansion room
} ioIMAChord;

enum
{
	CSF_KEYDOWN =	1,	// key currently held down in sjam kybd
	CSF_INSCALE =	2,	// member of scale
	CSF_FLAT =		4,	// display with flat
	CSF_SIMPLE =	8,	// simple chord, display at top of sjam list
	CSF_SELECTED = 0x80,// this chord is selected
	CSF_SIGNPOST = 0x40,// this chord is from a signpost
};

typedef struct ioIMAChordSelection
{
	wchar_t wstrName[16];	// text for display
	BYTE	fCSFlags;		// ChordSelection flags
	BYTE	bBeat;			// beat this falls on
	WORD	wMeasure;		// measure this falls on
	ioIMAChord aChord[4];	// array of chords: levels
	BYTE	bClick; 		// click this falls on
} ioIMAChordSelection;


#define KEY_FLAT 0x80
typedef struct ioIMASection
{
	long	lTime;				// Time this section starts.
	wchar_t wstrName[16];		// Each section has a name.
	WORD	wTempo; 			// Tempo.
	WORD	wRepeats;			// Number of repeats.
	WORD	wMeasureLength; 	// Length, in measures.
	WORD	wClocksPerMeasure;	// Length of each measure.
	WORD	wClocksPerBeat; 	// Length of each beat.
	WORD	wTempoFract;		// Tempo fraction.	(0-65536) (Score only)
	DWORD	dwFlags;			// Currently not used in SuperJAM!
	char	chKey;				// key sig. High bit is flat bit, the rest is root.
	char	chPad[3];
	GUID	guidStyle;
	GUID	guidPersonality;
	wchar_t wstrCategory[16];
} ioIMASection;

typedef struct ioIMANoteEvent
{
	long	lTime;			// When this event occurs.
	BYTE	bStatus;		// MIDI status.
	BYTE	bNote;			// Note value.
	BYTE	bVelocity;		// Note velocity.
	BYTE	bVoiceID;		// Band member who will play note
	WORD	wDuration;		// Lead line note duration. (Song)
	BYTE	bEventType; 	// Type of event
} ioIMANoteEvent;

typedef struct ioIMAMute
{
	long	lTime;			// Time in clocks.
	WORD	wMuteBits;		// Which instruments to mute.
	WORD	wLock;			// Lock flag
} ioIMAMute;

typedef struct ioIMACurveEvent
{
	long	lTime;
	WORD	wVariation;
	BYTE	bVoiceID;
	BYTE	bVelocity;
	BYTE	bEventType;
} ioIMACurveEvent;

typedef struct ioIMASubCurve
{
	BYTE	bCurveType; // defines the shape of the curve
	char	fFlipped;	// flaggs defining the flipped state: not, vertical, or horizontal
	short	nMinTime;	// left lower corner of bounding box.
	short	nMinValue;	// also used by the ECT_INSTANT curve type.
	short	nMaxTime;	// right upper corner of bounding box.
	short	nMaxValue;
} ioIMASubCurve;

typedef struct ioIMABand
{
	wchar_t wstrName[20]; // Band name
	BYTE	abPatch[16];  // GM
	BYTE	abVolume[16];
	BYTE	abPan[16];
	signed char achOctave[16];
	char	fDefault;		// This band is the style's default band
	char	chPad;
	WORD	awDLSBank[16];	// if GM bit set use abPatch
							// if GS bit set, use this plus abDLSPatch
							// else use both as a DLS
	BYTE	abDLSPatch[16];
	GUID	guidCollection;
//	  wchar_t wstrCollection[16];
	char	szCollection[32];			// this only needs to be single-wide chars
} ioIMABand;

 #pragma pack()

#endif //__RIFFSTRUCTS_SEGMENT_H__
