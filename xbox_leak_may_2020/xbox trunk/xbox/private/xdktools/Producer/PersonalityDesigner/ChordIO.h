#ifndef __CHORDIO_H_
#define __CHORDIO_H_

#include "dmusprod.h"
#include "DWList.h"
#include "PropChord.h"

class ChordEntry;
class ChordSelection;
class CChordItem : public DWListItem, public CPropChord
{
//	Used to track the selection of the chord in display.
	DWORD		m_fSelected;		// Selection and more
public:
	enum {Selected = 1, SignPost = 2, BegSignPost = 4, EndSignPost = 8}; 
	CChordItem();
	CChordItem(const CChordItem&);
	CChordItem(const ChordEntry&);
	CChordItem(const ChordSelection&);
	CChordItem& operator = (const CChordItem& item)
	{
		*dynamic_cast<CPropChord*>(this) = dynamic_cast<const CPropChord&>(item);
		m_fSelected = item.m_fSelected;
		return *this;
	}
	CChordItem *    GetNext() {return(CChordItem *)DWListItem::GetNext();};

	BOOL IsSelected() { return m_fSelected & Selected; }
	void SetSelected(BOOL b)
	{
		if(b)
		{
			m_fSelected |= Selected;
		}
		else
		{
			m_fSelected &= ~Selected;
		}
	}

	BOOL IsSignPost() { return m_fSelected & SignPost; }
	DWORD FSelected() { return m_fSelected; }
	void SetSignPost(BOOL b)
	{
		if(b)
		{
			m_fSelected |= SignPost;
		}
		else
		{
			m_fSelected &= ~SignPost;
		}
	}

//	Used to manage drawing and indexing.
	long		m_lDrawPosition;	// Drawn position in strip.
	DWORD		m_dwIndex;			// Index in list. (probably not needed anymore)
}; 

class CChordList : public DWList
{
public:
    CChordItem *    GetHead() {return(CChordItem *)DWList::GetHead();};
	CChordItem *	RemoveHead() { return(CChordItem *)DWList::RemoveHead();};
	void		ClearList();
	void		ClearSelections();
	DWORD		PreparePropChord( CPropChord *pPropChord );
	DWORD		RetrievePropChord( CPropChord *pPropChord );
	void		InsertByAscendingTime(CChordItem *pChord);	// *pChord <= *(pChord->next)
	HRESULT		Load( LPSTREAM pStream );
	HRESULT		Save( LPSTREAM pStream, BOOL bClearDirty );
	HRESULT		DMLoad(IStream* pStream);
	HRESULT		DMSave(IStream* pStream, BOOL bClearDirty);
};


typedef struct ioOldChord
{
	long	lChordPattern;	// pattern that defines chord
	long	lScalePattern;	// scale pattern for the chord
	long	lInvertPattern;	// inversion pattern
    BYTE    bRoot;			// root note of chord
	BYTE    bReserved;		// expansion room
	WORD    wCFlags;		// bit flags
	long	lReserved;		// expansion room
} ioOldChord;

typedef struct ioOldChordSelection
{
    wchar_t wstrName[16];   // text for display
    BYTE    fCSFlags;      // ChordSelection flags
    BYTE    bBeat;         // beat this falls on
    WORD    wMeasure;       // measure this falls on
    ioOldChord aChord[4];      // array of chords: levels
    BYTE    bClick;        // click this falls on
} ioOldChordSelection;


typedef struct ioChord
{
	DWORD	dwChordPattern;	// pattern that defines chord
	DWORD	dwScalePattern;	// scale pattern for the chord
	DWORD	dwInvertPattern;	// inversion pattern
	DWORD	dwLevels;				// what levels this chord supports
    BYTE    bChordRoot;			// root note of chord
	BYTE    bScaleRoot;		// root note of scale
	BYTE	bFlat;
	BYTE	bBits;
	WORD    wFlags;		// bit flags
	bool		bInUse;
} ioChord;

typedef struct ioChordSelection
{
    ioChord aChord[DMPolyChord::MAX_POLY];      // array of chords: levels
	DWORD	dwTime;
	short		nMeasure;
	BYTE	bBeat;
	BYTE	bRootIndex;
    wchar_t wstrName[DMPolyChord::MAX_NAME];   // text for display
	bool		bKeydown;
	BYTE	fCSFlags;
} ioChordSelection;

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
/*
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
*/
#endif // __CHORDIO_H_
