#ifndef __PROPCHORD_H_
#define __PROPCHORD_H_

#include "chord.h"

#define CHORD_INVERT  0x10      /* This chord may be inverted           */
#define CHORD_FOUR    0x20      /* This should be a 4 note chord        */
#define CHORD_UPPER   0x40      /* Shift upper octave down              */
#define CHORD_SIMPLE  0x80      /* This is a simple chord               */
#define CHORD_COUNT   0x0F      /* Number of notes in chord (up to 15)  */

class CPropChord : public DMPolyChord
{
//	Used to track which fields are shared by multiple 
//	chords and have multiple values, so are undetermined.
	DWORD		m_dwUndetermined[MAX_POLY];
public:
	enum {ALL = -1};
				CPropChord();
				CPropChord(const CPropChord&);
	void		SetBits(int nSubChord);
	void		CopyToPropChord( CPropChord *pPropChord, int nSubChord );	// nSubChord = -1 -> all subchords
	void		CopyFromPropChord( CPropChord *pPropChord, int nSubChord ); // nSubChord = -1 -> all subchords
	void		RootToString(char *pszName, int nSubChord);
	CPropChord&	operator = (const CPropChord&);
	BOOL		operator < (const CPropChord&);
	void		SetUndetermined(int nSubChord, DWORD value);	// nSubChord = -1 -> all subchords
	DWORD	GetUndetermined(int nSubChord)
	{
//		ASSERT(nSubChord < MAX_POLY);
		if(nSubChord >= MAX_POLY)
			nSubChord = 0;			// this chord is invalid but we still need a valid subchord index
									// as zero is the rootindex use that
		return m_dwUndetermined[nSubChord];
	}
	DWORD& Undetermined(int nSubChord)
	{
//		ASSERT(nSubChord < MAX_POLY);
		if(nSubChord >= MAX_POLY)
			nSubChord = 0;			// this chord is invalid but we still need a valid subchord index
									// as zero is the rootindex use that
		return m_dwUndetermined[nSubChord];
	}
	/*
    DWORD		m_dwTime;
    DWORD		m_dwChordPattern;	// Pattern that defines chord
    DWORD		m_dwScalePattern;	// Scale Pattern for the chord
    short		m_nMeasure;			// What measure
    char		m_szName[12];		// Text for display
    BYTE		m_bRoot;			// Root note of chord
    BYTE		m_bFlat;			// Display with flat
    BYTE		m_bBeat;			// What beat this falls on
    BYTE		m_bBits;			// Invert and item count
	*/
};

// Flags for m_dwUndetermined:

#define UD_CHORDPATTERN		(1 << 0)
#define UD_SCALEPATTERN		(1 << 1)
#define UD_NAME				(1 << 2)
#define UD_CHORDROOT				(1 << 3)
#define UD_FLAT				(1 << 4)
#define UD_INVERT			(1 << 5)
#define UD_FOUR				(1 << 6)
#define UD_UPPER			(1 << 7)
#define UD_SIMPLE			(1 << 8)
#define UD_DRAGSELECT		(1 << 9)
#define UD_DROPSELECT		(1 << 10)
#define UD_FROMCHORDPALETTE	(1 << 11)
#define UD_SCALEROOT		(1<<12)
#define UD_INVERTPATTERN	(1 << 13)
#define UD_LEVELS (1 << 14)

#endif // __PROPCHORD_H_
