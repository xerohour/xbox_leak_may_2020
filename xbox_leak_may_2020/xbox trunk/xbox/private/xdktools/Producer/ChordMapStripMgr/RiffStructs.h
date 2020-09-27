#ifndef __RIFFSTRUCTS_H__
#define __RIFFSTRUCTS_H__

#pragma pack(2)


/*
typedef struct ioNoteEvent
{
    long    lTime;           // When this event occurs.
    BYTE    bStatus;         // MIDI status.
    BYTE    bNote;           // Note value.
    BYTE    bVelocity;       // Note velocity.
    BYTE    bVoiceID;        // Band member who will play note
    WORD    wDuration;       // Lead line note duration. (Song)
    BYTE    bEventType;      // Type of event
} ioNoteEvent;
*/

/*
typedef struct ioNote
{
    BYTE    bEventType;           // Type of event
    BYTE    bVoiceID;             // Instrument identifier.
    short   nTime;                // Time from center of beat.
    WORD    wVariation;           // 16 variation bits.
    BYTE    bScaleValue;          // Position in scale.
    BYTE    bBits;                // Various bits.
    BYTE    bValue;               // Note value.
    BYTE    bVelocity;            // Note velocity.
    WORD    nMusicValue;  // Description of note in chord and key.
    short   nDuration;            // Duration
    BYTE    bTimeRange;           // Range to randomize time.
    BYTE    bDurRange;            // Range to randomize duration.
    BYTE    bVelRange;            // Range to randomize velocity.
    BYTE    bPlayMode;
} ioNote;
*/

/*
typedef struct ioCurveEvent
{
    long    lTime;
    WORD    wVariation;
    BYTE    bVoiceID;
    BYTE    bVelocity;
    BYTE    bEventType;
} ioCurveEvent;
*/

/*
typedef struct ioCurve
{
    BYTE    bEventType;
    BYTE    bVoiceID;
    short   nTime;
    WORD    wVariation;
    BYTE    bCCData;
} ioCurve;
*/

/*
typedef struct ioSubCurve
{
    BYTE    bCurveType; // defines the shape of the curve
    char    fFlipped;  // flaggs defining the flipped state: not, vertical, or horizontal
    short   nMinTime;   // left lower corner of bounding box.
    short   nMinValue;  // also used by the ECT_INSTANT curve type.
    short   nMaxTime;   // right upper corner of bounding box.
    short   nMaxValue;
} ioSubCurve;
*/

/*
typedef struct ioMute
{
    long    lTime;           // Time in clocks.
    WORD    wMuteBits;       // Which instruments to mute.
    WORD    wLock;          // Lock flag
} ioMute;
*/


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
    BYTE    bRoot;         // root note of chord
    BYTE    bReserved;     // expansion room
    WORD    wCFlags;        // bit flags
	long	lReserved;		// expansion room
} ioChord;


// this should match enums in iochord.h
enum
{
	CSF_KEYDOWN = 	1,	// key currently held down in sjam kybd
	CSF_INSCALE = 	2,	// member of scale
	CSF_FLAT =		4,	// display with flat
	CSF_SIMPLE =	8,	// simple chord, display at top of sjam list
	CSF_SELECTED = 0x80, // this chord is selected
	CSF_SIGNPOST = 0x40, // this chord is from a signpost
	CSF_SHIFTED = 0x20	// this chord has been altered from an illegal chord
						// used to make chord unique so that Load subchord database code works correctly
						// (see use in ChordDatabase.cpp)

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

/*
typedef struct ioBand
{
    wchar_t wstrName[20]; // Band name
    BYTE    abPatch[16];
    BYTE    abVolume[16];
    BYTE    abPan[16];
    signed char achOctave[16];
    char    fDefault;    // This band is the style's default band
    char    chPad;
    WORD    awDLSBank[16];
    BYTE    abDLSPatch[16];
    GUID    guidCollection;
//    wchar_t wstrCollection[16];
    char    szCollection[32];           // this only needs to be single-wide chars
} ioBand;
*/

/*
typedef struct ioLyric
{
    long    lTime;       // Time, in clocks
} ioLyric;
*/

/*
typedef struct ioPhrase
{
    long    lTime;
    BYTE    bID;    // which phrase it is. Index starting at 0.
} ioPhrase;
*/

/*
typedef struct ioClick
{
    short   lTime;               // Index into grid.
} ioClick;
*/

typedef struct ioPattern
{
    long    lTime;             // Time this starts.
    DWORD   dwLength;           // Pattern length in clocks.
    DWORD   fFlags;            // Various flags.
    WORD    wClocksPerClick;   // Size of each click.
    WORD    wBeat;             // What note gets the beat.
    WORD    wClocksPerBeat;    // Size of each beat.
    WORD    wMeasures;         // Number of measures.
    wchar_t wstrName[16];         // Name of pattern.
    DWORD   dwKeyPattern;       // Key for defining in.
    DWORD   dwChordPattern;     // Defining chord.
    BYTE    abInvertUpper[16];   // Inversion upper limit.
    BYTE    abInvertLower[16];   // Inversion upper limit.
    WORD    wInvert;     // Activate inversion flags.
    WORD    awVarFlags[16][16]; // Var flags for all musicians.
    WORD    wAutoInvert;    // Automatically set inversion limits
    BYTE    bRoot;             // Root for defining.
    char    achChordChoice[16];
} ioPattern;

/*
typedef struct ioStyle
{
    wchar_t wstrName[16];         // Each style has a name.
    WORD    wBPM;              // Beats per measure.
    WORD    wBeat;             // Beat note.
    WORD    wClocksPerClick;   // Clocks per click in patterns.
    WORD    wClocksPerBeat;    // Clocks per beat.
    WORD    wClocksPerMeasure; // Clocks per measure.
    WORD    wTempo;            // Tempo.
    WORD    wTempoFract;
    GUID    guid;
    wchar_t wstrCategory[16];
} ioStyle;
*/

typedef struct ioPersonalityRef
{
    wchar_t wstrName[20];       // Internal name stored in personality
    char    fDefault;           // 1=Default personality
    char    achPad[3];
    GUID    guid;
} ioPersonalityRef;

#pragma pack()

#endif // __RIFFSTRUCTS_H__
