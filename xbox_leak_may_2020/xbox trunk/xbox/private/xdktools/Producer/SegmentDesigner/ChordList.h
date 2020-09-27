#ifndef __CHORDLIST_H__
#define __CHORDLIST_H__ 1
#include <afxtempl.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include <dmusicf.h>
#include "RiffStructs.h"

#define SUBCHORD_BASS				0
#define SUBCHORD_STANDARD_CHORD		1

inline long Rotate24(long val, long shift)
{
	__int64 mask64 = 0xffffff;
	__int64 val64 = val;

	ASSERT(shift > -24 && shift < 24);
	if(shift < 0)
	{
		shift = 24 + shift;
	}
	val64 = (val64 & mask64) << shift;
	__int64 excess = (val64 >> 24) & mask64;	// rotate
	val64 |= (excess & ~(1<<shift));
	val =  long(val64 & mask64);
	return val;
}


struct DMSubChord
{
public:
	DMSubChord() : m_dwChordPattern(0), m_dwScalePattern(0), m_dwInversionPoints(0),
		m_bChordRoot(0), m_bScaleRoot(0), m_dwLevels(1 << SUBCHORD_STANDARD_CHORD)
	{}
	DMSubChord(const ioIMAChord& iOldChord,  BYTE key)
	{
		m_dwChordPattern = iOldChord.lChordPattern;
		m_dwScalePattern = iOldChord.lScalePattern;
		m_dwInversionPoints = iOldChord.lInvertPattern;
		m_dwLevels = 0xFFFFFFFF;
		m_bChordRoot = iOldChord.bRoot;
		m_bScaleRoot = unsigned char(key & 0x7f);
		// Unused members:
		// iChordSelection.aChord[0,1,2,3].bReserved
		// iChordSelection.aChord[0,1,2,3].wCFlags
		// iChordSelection.aChord[0,1,2,3].lReserved
	}
	operator DMUS_IO_SUBCHORD()
	{
		DMUS_IO_SUBCHORD result;
		result.dwChordPattern = m_dwChordPattern;
		result.dwScalePattern = m_dwScalePattern;
		result.dwInversionPoints = m_dwInversionPoints;
		result.dwLevels = m_dwLevels;
		result.bChordRoot = m_bChordRoot;
		result.bScaleRoot = m_bScaleRoot;
		return result;
	}
	DWORD	m_dwChordPattern;		// Notes in the subchord
	DWORD	m_dwScalePattern;		// Notes in the scale
	DWORD	m_dwInversionPoints;	// Where inversions can occur
	DWORD	m_dwLevels;				// Which levels are supported by this subchord
	BYTE	m_bChordRoot;			// Root of the subchord
	BYTE	m_bScaleRoot;			// Root of the scale
};

struct DMChord
{
public:
	DMChord() : m_strName(""), m_mtTime(0), m_wMeasure(0), m_bBeat(0) {}
	~DMChord();
	DMChord(const ioIMAChordSelection &iChordSelection,  BYTE key);
	HRESULT Save( interface IDMUSProdRIFFStream* pIRiffStream );

	CString	m_strName;		// Name of the chord
	MUSIC_TIME	m_mtTime;		// Time, in clocks
	WORD	m_wMeasure;		// Measure this falls on
	BYTE	m_bBeat;		// Beat this falls on
	CTypedPtrList<CPtrList, DMSubChord*> m_lstSubChord;
};

class CChordList {
public:
    CChordList();
	~CChordList();
	HRESULT IMA_AddChord( IStream* pIStream, long lRecSize,  BYTE key);
	HRESULT CreateTrack( class CTrack** ppTrack );

	interface IDMUSProdFramework*	m_pIFramework;

private:
	HRESULT DM_SaveChordList( interface IDMUSProdRIFFStream* pIRIFFStream );
	CTypedPtrList<CPtrList, DMChord*> m_lstChords;
    short           m_nLastImportantMeasure;
};

#endif //__COMMANDLIST_H__
