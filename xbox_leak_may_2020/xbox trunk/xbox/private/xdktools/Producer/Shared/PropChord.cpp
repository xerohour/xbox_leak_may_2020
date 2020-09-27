#include "stdafx.h"
#include "PropChord.h"

CPropChord::CPropChord()
{
	m_bBeat = 0;
	m_bBits = 0;
	m_bFlat = 0;
	m_bRoot = 12;
	m_dwChordPattern = 0;
	m_dwScalePattern = 0;
	m_dwTime = 0;
	m_nMeasure = 0;
	m_szName[0] = 0;
	m_dwUndetermined = 0;
}

static char *convert[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
static char *flatconvert[] = {"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"};

void CPropChord::RootToString(char *pszName)

{
	short nX = (char)( m_bRoot % 12 );
	if( m_bFlat )
		wsprintf( pszName,"%d%s",1 + (m_bRoot / 12), (LPSTR)flatconvert[nX] ) ;
	else
		wsprintf( pszName,"%d%s",1 + (m_bRoot / 12), (LPSTR)convert[nX] ) ;
}


void CPropChord::SetBits()
{
	LONG    nX;
	short   nCount = 0;

    for( nX=0L ;  nX<32L ;  nX++ )
    {
        if( m_dwChordPattern & (1L << nX) )
            nCount++;
    }

    if( !m_bBits )
    {
        m_bBits |= CHORD_INVERT;
        if( nCount > 3 )
            m_bBits |= CHORD_FOUR;
        if( m_dwChordPattern & (15L << 18L) )
            m_bBits |= CHORD_UPPER;
    }
    m_bBits &= ~CHORD_COUNT;
    m_bBits |= nCount;
}

void CPropChord::CopyToPropChord( CPropChord *pPropChord )

{
	DWORD dwChangeFlags = 0;
	if (m_dwChordPattern != pPropChord->m_dwChordPattern)
	{
		dwChangeFlags |= UD_CHORDPATTERN;
	}
	if (m_dwScalePattern != pPropChord->m_dwScalePattern)
	{
		dwChangeFlags |= UD_SCALEPATTERN;
	}
	if (strcmp(m_szName, pPropChord->m_szName))
	{
		dwChangeFlags |= UD_NAME;
	}
	if (m_bRoot != pPropChord->m_bRoot)
	{
		dwChangeFlags |= UD_ROOT;
	}
	if (m_bFlat != pPropChord->m_bFlat)
	{
		dwChangeFlags |= UD_FLAT;
	}
	if ((m_bBits & CHORD_INVERT) != (pPropChord->m_bBits & CHORD_INVERT))
	{
		dwChangeFlags |= UD_INVERT;
	}
	if ((m_bBits & CHORD_FOUR) != (pPropChord->m_bBits & CHORD_FOUR))
	{
		dwChangeFlags |= UD_FOUR;
	}	
	if ((m_bBits & CHORD_UPPER) != (pPropChord->m_bBits & CHORD_UPPER))
	{
		dwChangeFlags |= UD_UPPER;
	}
	pPropChord->m_dwUndetermined = dwChangeFlags;
}

  
void CPropChord::CopyFromPropChord( CPropChord *pPropChord )

{
	DWORD dwChangeFlags = ~pPropChord->m_dwUndetermined;
	if (dwChangeFlags & UD_CHORDPATTERN)
	{
		m_dwChordPattern = pPropChord->m_dwChordPattern;
	}
	if (dwChangeFlags & UD_SCALEPATTERN)
	{
		m_dwScalePattern = pPropChord->m_dwScalePattern;
	}
	if (dwChangeFlags & UD_NAME)
	{
		strcpy(m_szName, pPropChord->m_szName);
	}
	if (dwChangeFlags & UD_ROOT)
	{
		m_bRoot = pPropChord->m_bRoot;
	}
	if (dwChangeFlags & UD_FLAT)
	{
		m_bFlat = pPropChord->m_bFlat;
	}
	if (dwChangeFlags & UD_INVERT)
	{
		m_bBits &= ~CHORD_INVERT;
		m_bBits |= (pPropChord->m_bBits & CHORD_INVERT);
	}
	if (dwChangeFlags & UD_FOUR)
	{
		m_bBits &= ~CHORD_FOUR;
		m_bBits |= (pPropChord->m_bBits & CHORD_FOUR);
	}
	if (dwChangeFlags & UD_UPPER)
	{
		m_bBits &= ~CHORD_UPPER;
		m_bBits |= (pPropChord->m_bBits & CHORD_UPPER);
	}
}


BOOL CPropChord :: operator < (const CPropChord& chord)
{
	if( m_nMeasure == chord.m_nMeasure)
	{
		if(m_bBeat < chord.m_bBeat)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if (m_nMeasure < chord.m_nMeasure)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

CPropChord& CPropChord::operator = (const CPropChord& chord)
{
	if(&chord == this)
	{
		return *this;
	}

	m_dwTime = chord.m_dwTime;
	m_dwChordPattern = chord.m_dwChordPattern;
	m_dwScalePattern = chord.m_dwScalePattern;
	m_nMeasure = chord.m_nMeasure;
	strcpy(m_szName, chord.m_szName);
	m_bRoot = chord.m_bRoot;
	m_bFlat = chord.m_bFlat;
	m_bBeat = chord.m_bBeat;
	m_bBits = chord.m_bBits;
	m_dwUndetermined = chord.m_dwUndetermined;

	return *this;
}

CPropChord::CPropChord(const CPropChord& chord)
{

	m_dwTime = chord.m_dwTime;
	m_dwChordPattern = chord.m_dwChordPattern;
	m_dwScalePattern = chord.m_dwScalePattern;
	m_nMeasure = chord.m_nMeasure;
	strcpy(m_szName, chord.m_szName);
	m_bRoot = chord.m_bRoot;
	m_bFlat = chord.m_bFlat;
	m_bBeat = chord.m_bBeat;
	m_bBits = chord.m_bBits;
	m_dwUndetermined = chord.m_dwUndetermined;
}
