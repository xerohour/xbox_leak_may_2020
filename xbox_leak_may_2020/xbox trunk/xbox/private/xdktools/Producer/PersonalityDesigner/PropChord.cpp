#include "stdafx.h"
#include "PropChord.h"

CPropChord::CPropChord()
{
	for(int i = 0; i < MAX_POLY; i++)
		m_dwUndetermined[i] = 0;
}

static char *convert[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
static char *flatconvert[] = {"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"};

void CPropChord::RootToString(char *pszName, int nSubChord)
{
	ASSERT(nSubChord < MAX_POLY);
	nSubChord = nSubChord < 0 ? 0 : nSubChord;	// < 0 --> default root (0)
	int nX = (char)( SubChord(nSubChord)->ChordRoot() % 12 );
	if( SubChord(nSubChord)->UseFlat() )
		wsprintf( pszName,"%d%s",1 + (SubChord(nSubChord)->ChordRoot() / 12), (LPSTR)flatconvert[nX] ) ;
	else
		wsprintf( pszName,"%d%s",1 + (SubChord(nSubChord)->ChordRoot()/ 12), (LPSTR)convert[nX] ) ;
}


void CPropChord::SetBits(int nSubChord)
{
	LONG    nX;
	int   nCount = 0;

	DMChord* pSub = SubChord(nSubChord);

    for( nX=0L ;  nX<32L ;  nX++ )
    {
        if( pSub->ChordPattern() & (1L << nX) )
            nCount++;
    }

    if( !pSub->Bits() )
    {
        pSub->Bits() |= CHORD_INVERT;
        if( nCount > 3 )
            pSub->Bits() |= CHORD_FOUR;
        if( pSub->ChordPattern() & (15L << 18L) )
            pSub->Bits() |= CHORD_UPPER;
    }
    pSub->Bits() &= ~CHORD_COUNT;
    pSub->Bits() |= nCount;
}

void CPropChord::CopyToPropChord( CPropChord *pPropChord,  int nSubChord)
{
	ASSERT(nSubChord < MAX_POLY);

	int lower = nSubChord <0 ? 0 : nSubChord;
	int upper = nSubChord < -1 ? MAX_POLY : nSubChord + 1;
	int i;
	DWORD dwChangeFlags[MAX_POLY] ;
	
	for(i = 0; i < MAX_POLY; i++)
	{
		dwChangeFlags[i] = 0;
		pPropChord->m_dwUndetermined[i] = 0;
	}

	for(i = lower; i < upper; i++)
	{
		DMChord* pSubL = SubChord(i);
		DMChord* pSubR = pPropChord->SubChord(i);

		if (pSubL->ChordPattern() != pSubR->ChordPattern())
		{
			dwChangeFlags[i] |= UD_CHORDPATTERN;
		}

		if (pSubL->ScalePattern() != pSubR->ScalePattern())
		{
			dwChangeFlags[i] |= UD_SCALEPATTERN;
		}

		if (strcmp(Name(), pPropChord->Name()))
		{
			dwChangeFlags[i] |= UD_NAME;
		}

		if (pSubL->ChordRoot() != pSubR->ChordRoot())
		{
			dwChangeFlags[i] |= UD_CHORDROOT;
		}

		if (pSubL->ScaleRoot() != pSubR->ScaleRoot())
		{
			dwChangeFlags[i] |= UD_CHORDROOT;
		}

		if (pSubL->UseFlat() != pSubR->UseFlat())
		{
			dwChangeFlags[i] |= UD_FLAT;
		}

		if ((pSubL->Bits() & CHORD_INVERT) != (pSubR->Bits() & CHORD_INVERT))
		{
			dwChangeFlags[i] |= UD_INVERT;
		}

		if ((pSubL->Bits() & CHORD_FOUR) != (pSubR->Bits() & CHORD_FOUR))
		{
			dwChangeFlags[i] |= UD_FOUR;
		}

		if ((pSubL->Bits() & CHORD_UPPER) != (pSubR->Bits() & CHORD_UPPER))
		{
			dwChangeFlags[i] |= UD_UPPER;
		}

		if(pSubL->InvertPattern() != pSubR->InvertPattern())
		{
			dwChangeFlags[i] |= UD_INVERTPATTERN;
		}
		
		if(pSubL->Levels() != pSubR->Levels())
		{
			dwChangeFlags[i] |= UD_LEVELS;
		}

		pPropChord->m_dwUndetermined[i] = dwChangeFlags[i];
	}
}

  
void CPropChord::CopyFromPropChord( CPropChord *pPropChord, int nSubChord )
{
	ASSERT(nSubChord < MAX_POLY);

	int lower = nSubChord == -1 ? 0 : nSubChord;
	int upper = nSubChord == -1 ? MAX_POLY : nSubChord + 1;
	int i;
	DWORD dwChangeFlags[MAX_POLY] ;
	for(i = 0; i < MAX_POLY; i++)
	{
		dwChangeFlags[i] = ~(pPropChord->m_dwUndetermined[i]);
	}

	for(i = lower; i < upper; i++)
	{
		DMChord* pSubL = SubChord(i);
		DMChord* pSubR = pPropChord->SubChord(i);

		if (dwChangeFlags[i] & UD_CHORDPATTERN)
		{
			pSubL->ChordPattern() = pSubR->ChordPattern();
		}

		if (dwChangeFlags[i] & UD_SCALEPATTERN)
		{
			pSubL->ScalePattern() = pSubR->ScalePattern();
		}

		if (dwChangeFlags[i] & UD_NAME)
		{
			strcpy(Name(), pPropChord->Name());
		}

		if (dwChangeFlags[i] & UD_CHORDROOT)
		{
			pSubL->ChordRoot() = pSubR->ChordRoot();
		}

		if( dwChangeFlags[i] & UD_SCALEROOT)
		{
			pSubL->ScaleRoot() = pSubR->ScaleRoot();
		}

		if (dwChangeFlags[i] & UD_FLAT)
		{
			pSubL->UseFlat() = pSubR->UseFlat();
		}

		if (dwChangeFlags[i] & UD_INVERT)
		{
			pSubL->Bits() &= ~CHORD_INVERT;
			pSubL->Bits() |= (pSubR->Bits() & CHORD_INVERT);
		}

		if (dwChangeFlags[i] & UD_FOUR)
		{
			pSubL->Bits() &= ~CHORD_FOUR;
			pSubL->Bits() |= (pSubR->Bits() & CHORD_FOUR);
		}

		if (dwChangeFlags[i] & UD_UPPER)
		{
			pSubL->Bits() &= ~CHORD_UPPER;
			pSubL->Bits() |= (pSubR->Bits() & CHORD_UPPER);
		}

		if(dwChangeFlags[i] & UD_INVERTPATTERN)
		{
			pSubL->InvertPattern() = pSubR->InvertPattern();
		}

		if(dwChangeFlags[i] & UD_LEVELS)
		{
			pSubL->Levels() = pSubR->Levels();
		}
	}
}


BOOL CPropChord :: operator < (const CPropChord& chord)
{
	if( Measure() == chord.Measure())
	{
		if(Beat() < chord.Beat())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if (Measure() < chord.Measure())
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

	*dynamic_cast<DMPolyChord*>(const_cast<CPropChord*>(this)) = chord;

	for(int i = 0; i < MAX_POLY; i++)
	{
		m_dwUndetermined[i] = chord.m_dwUndetermined[i];
	}

	return *this;
}

CPropChord::CPropChord(const CPropChord& chord)
{
	*dynamic_cast<DMPolyChord*>(const_cast<CPropChord*>(this)) = chord;

	for(int i = 0; i < MAX_POLY; i++)
	{
		m_dwUndetermined[i] = chord.m_dwUndetermined[i];
	}
}

void	CPropChord::SetUndetermined(int nSubChord, DWORD value)
{
	ASSERT(nSubChord < MAX_POLY);
	int lower = nSubChord == -1 ? 0 : static_cast<int>(nSubChord);
	int upper = nSubChord == -1 ? MAX_POLY : static_cast<int>(nSubChord) + 1;
	for(int i = lower; i < upper; i++)
	{
		m_dwUndetermined[i] = value;
	}
}