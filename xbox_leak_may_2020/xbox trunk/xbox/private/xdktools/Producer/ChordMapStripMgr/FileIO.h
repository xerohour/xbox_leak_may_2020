
#define CHORD_INVERT  0x10      /* This chord may be inverted           */
#define CHORD_FOUR    0x20      /* This should be a 4 note chord        */
#define CHORD_UPPER   0x40      /* Shift upper octave down              */
#define CHORD_SIMPLE  0x80      /* This is a simple chord               */
#define CHORD_COUNT   0x0F      /* Number of notes in chord (up to 15)  */

//////////////////////////////////////////
// ChordExt Structure
// Used to store a Section's single linked chord list.
// Use: CSection::m_paChordChanges.
typedef struct ChordExt				// Based on ChordSelection
{
    ChordExt		*pNext;
    long			time;
    long			pattern;		// Pattern that defines chord
    char			name[12];		// Text for display
    char			keydown;		// Currently held down
    char			root;			// Root note of chord
    char			inscale;		// Member of scale
    char			flat;			// Display with flat
    short			varflags;		// Used to select appropriate variation
    short			measure;		// What measure
    char			beat;			// What beat this falls on
    unsigned char	bits;			// Invert and item count
    long			scalepattern;	// Scale Pattern for the chord
    long			melodypattern;	// Melody Pattern for the chord
} ChordExt;

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

typedef struct ioChordSelection
{
    wchar_t wstrName[16];   // text for display
    BYTE    fCSFlags;      // ChordSelection flags
    BYTE    bBeat;         // beat this falls on
    WORD    wMeasure;       // measure this falls on
    ioChord aChord[4];      // array of chords: levels
    BYTE    bClick;        // click this falls on
} ioChordSelection;

enum
{
	CSF_KEYDOWN = 	1,	// key currently held down in sjam kybd
	CSF_INSCALE = 	2,	// member of scale
	CSF_FLAT =		4,	// display with flat
	CSF_SIMPLE =	8,	// simple chord, display at top of sjam list
};

// seeks to a 32-bit position in a stream.
HRESULT __inline StreamSeek( LPSTREAM pStream, long lSeekTo, DWORD dwOrigin )
{
	LARGE_INTEGER li;

	if( lSeekTo < 0 )
	{
		li.HighPart = -1;
	}
	else
	{
        li.HighPart = 0;
	}
	li.LowPart = lSeekTo;
	return pStream->Seek( li, dwOrigin, NULL );
}
