#ifndef __RIFFSTRUCTS_SEGMENT_H__
#define __RIFFSTRUCTS_SEGMENT_H__ 1

#pragma pack(2)

// *********************************************************************
// *********************************************************************
// ********************* File io for IMA 25 objects ********************
// *********************************************************************
// *********************************************************************

typedef struct ioCommand
{
    long    lTime;       // Time, in clocks.
    DWORD   dwCommand;    // Command type.
} ioCommand;


typedef struct ioChord
{
	long	lChordPattern;	// pattern that defines chord
	long	lScalePattern;	// scale pattern for the chord
	long	lInvertPattern;	// inversion pattern
    	BYTE    bRoot;		// root note of chord
	BYTE    bReserved;	// expansion room
	WORD    wCFlags;	// bit flags
	long	lReserved;	// expansion room
} ioChord;

enum
{
	CSF_KEYDOWN = 	1,	// key currently held down in sjam kybd
	CSF_INSCALE = 	2,	// member of scale
	CSF_FLAT =		4,	// display with flat
	CSF_SIMPLE =	8,	// simple chord, display at top of sjam list
};

typedef struct ioChordSelection
{
    wchar_t wstrName[16];   // text for display
    BYTE    fCSFlags;      // ChordSelection flags
    BYTE    bBeat;         // beat this falls on
    WORD    wMeasure;       // measure this falls on
    ioChord aChord[4];      // array of chords: levels
    BYTE    bClick;        // click this falls on
} ioChordSelection;


#define KEY_FLAT 0x80
typedef struct ioSect
{
    long    lTime;           // Time this section starts.
    wchar_t wstrName[16];       // Each section has a name.
    WORD    wTempo;             // Tempo.
    WORD    wRepeats;           // Number of repeats.
    WORD    wMeasureLength;     // Length, in measures.
    WORD    wClocksPerMeasure;  // Length of each measure.
    WORD    wClocksPerBeat;     // Length of each beat.
    WORD    wTempoFract;        // Tempo fraction.  (0-65536) (Score only)
    DWORD   dwFlags;           // Currently not used in SuperJAM!
    char    chKey;          // key sig. High bit is flat bit, the rest is root.
    char    chPad[3];
    GUID    guidStyle;
    GUID    guidPersonality;
    wchar_t wstrCategory[16];
} ioSection;

#pragma pack()

#endif //__RIFFSTRUCTS_SEGMENT_H__
