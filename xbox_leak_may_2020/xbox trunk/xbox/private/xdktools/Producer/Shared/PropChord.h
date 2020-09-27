#ifndef __PROPCHORD_H_
#define __PROPCHORD_H_

#define CHORD_INVERT  0x10      /* This chord may be inverted           */
#define CHORD_FOUR    0x20      /* This should be a 4 note chord        */
#define CHORD_UPPER   0x40      /* Shift upper octave down              */
#define CHORD_SIMPLE  0x80      /* This is a simple chord               */
#define CHORD_COUNT   0x0F      /* Number of notes in chord (up to 15)  */

class CPropChord 
{
public:
				CPropChord();
				CPropChord(const CPropChord&);
	void		SetBits();
	void		CopyToPropChord( CPropChord *pPropChord );
	void		CopyFromPropChord( CPropChord *pPropChord );
	void		RootToString(char *pszName);
	CPropChord&	operator = (const CPropChord&);
	BOOL		operator < (const CPropChord&);
    DWORD		m_dwTime;
    DWORD		m_dwChordPattern;	// Pattern that defines chord
    DWORD		m_dwScalePattern;	// Scale Pattern for the chord
    short		m_nMeasure;			// What measure
    char		m_szName[12];		// Text for display
    BYTE		m_bRoot;			// Root note of chord
    BYTE		m_bFlat;			// Display with flat
    BYTE		m_bBeat;			// What beat this falls on
    BYTE		m_bBits;			// Invert and item count
//	Used to track which fields are shared by multiple 
//	chords and have multiple values, so are undetermined.
	DWORD		m_dwUndetermined;
};

// Flags for m_dwUndetermined:

#define UD_CHORDPATTERN		(1 << 0)
#define UD_SCALEPATTERN		(1 << 1)
#define UD_NAME				(1 << 2)
#define UD_ROOT				(1 << 3)
#define UD_FLAT				(1 << 4)
#define UD_INVERT			(1 << 5)
#define UD_FOUR				(1 << 6)
#define UD_UPPER			(1 << 7)
#define UD_SIMPLE			(1 << 8)
#define UD_DRAGSELECT		(1 << 9)
#define UD_DROPSELECT		(1 << 10)
#define UD_FROMCHORDPALETTE	(1 << 11)

#endif // __PROPCHORD_H_
