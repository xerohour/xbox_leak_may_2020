//	chord.cpp

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>
#include <time.h>
#include "chord.h"
#include "chordio.h"
#include "propchord.h"
#include "..\shared\RiffStrm.h"
#include "ChordMapStripMgr.h"


ChordChangeCallback::~ChordChangeCallback()
{
	if(m_pChordEntry)delete m_pChordEntry;
}

HRESULT ChordChangeCallback::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
    if(::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

    if(::IsEqualIID(riid, IID_ChordChangeCallback))
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG ChordChangeCallback::AddRef()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));	
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG ChordChangeCallback::Release()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}



HRESULT PreEditNotification::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
    if(::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

	if(::IsEqualIID(riid, IID_PreEditNotification))
	{
		AddRef();
		*ppvObj = this;
		return S_OK;
	}


    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG PreEditNotification::AddRef()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));	
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG PreEditNotification::Release()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}

HRESULT CheckForOrphansNotification::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
    if(::IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

	if(::IsEqualIID(riid, IID_CheckForOrphansNotification))
	{
		AddRef();
		*ppvObj = this;
		return S_OK;
	}


    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CheckForOrphansNotification::AddRef()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));	
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CheckForOrphansNotification::Release()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}

ULONG ZoomChangeNotification::AddRef()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));	
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG ZoomChangeNotification::Release()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/*
#ifndef CHORDMAP
#pragma pack(6)
#endif
*/

DWORD OldChordEntry::SizeExtendedChordEntry()
{
	return sizeof(OldChordEntry);
}
DWORD OldChordEntry::SizeSmallChordEntry()
{
	return SizeExtendedChordEntry() - 5*sizeof(short);
}
DWORD OldChordEntry::SizeChordSelection()
{
	return SizeExtendedChordEntry() - (5*sizeof(short) + sizeof(short) + sizeof(DWORD));
}

DMChord::DMChord()
{
	m_dwChordPattern = 0x91;
	m_bChordRoot = 12;
	m_bScaleRoot = 0;
	m_bFlat = FALSE;
	m_bBits = 3 | CHORD_INVERT | CHORD_SIMPLE;
	m_wFlags =0;
	m_dwScalePattern = 0xAB5AB5;
	m_dwLevels = ~(unsigned)0;	// all levels
	m_bInUse = false;
	m_dwInvertPattern = ~(unsigned)0;
}

int DMChord::BassNote()
{
	int retval = -1;
	for(int i = 0; i < 24; i++)
	{
		if(m_dwChordPattern & (1 << i))
		{
			retval = i;
			break;
		}
	}
	return retval;
}

int DMChord::Third(int* bass)
{
	int third = -1;
	int bassnote = BassNote();

	if(bassnote != third)
	{
		for(int i = bassnote + 1; i < 24; i++)
		{
			if(m_dwChordPattern & (1 << i))
			{
					third = i;
					break;
			}
		}
	}

	if(bass)
	{
		*bass = bassnote;
	}
	return third;
}

int DMChord::Fifth(int* bass, int* third)
{
	int fifth = -1;
	int thirdnote = Third(bass);

	if(thirdnote != fifth)
	{
		for(int i = thirdnote+1; i < 24; i++)
		{
			if(m_dwChordPattern & (1 << i))
			{
					fifth = i;
					break;
			}
		}
	}

	if(third)
	{
		*third = thirdnote;
	}
	return fifth;
}

int DMChord::Seventh(int* bass, int* third, int* fifth)
{
	int seventh = -1;
	int fifthnote = Fifth(bass, third);

	if(fifthnote != seventh)
	{
		for(int i = fifthnote+1; i < 24; i++)
		{
			if(m_dwChordPattern & (1 << i))
			{
				seventh = i;
				break;
			}
		}
	}

	if(fifth)
	{
		*fifth = fifthnote;
	}
	return seventh;
}

void DMChord::ChordNotes(int& bass, int& third, int& fifth, int& seventh)
{
	seventh = Seventh(&bass, &third, &fifth);
}

int DMChord::NoteCount()
{
	int count  = 0;
	for(int i = 0; i < 24; i++)
	{
		if(m_dwChordPattern & (1 << i))
		{
			++count;
		}
	}
	return count;
}

DMPolyChord::DMPolyChord()
{
	strcpy(m_szName, _T("M"));
	m_dwTime = 0;
	m_nMeasure = 0;
	m_bBeat = 0;
	m_bRootIndex = 0;
	m_keydown = false;
	InitLevels();
}

DMPolyChord::DMPolyChord(const DMPolyChord& poly)
{
	memcpy(this, &poly, sizeof(DMPolyChord));
}

DMPolyChord& DMPolyChord::operator = (const DMPolyChord& poly)
{
	memcpy(this, &poly, sizeof(DMPolyChord));
	return *this;
}

void DMPolyChord::Load(OldChordEntry& oce)
{
	m_dwTime = oce.time;
	m_nMeasure = oce.measure;
	m_bBeat = oce.beat;
	strcpy(m_szName, _T(oce.name));
	m_keydown = oce.keydown ? true : false;
	m_bRootIndex = 0;
	DMChord* pChord = 0;

	// old chords are single level, bottom 4 notes = level 1 (bass), top 4 notes = level two (upper).
	DWORD bass = 0, upper = 0;
	DWORD cbass = 4, cupper = 4;
	int bits = sizeof(DWORD) * 8;	// should be 32
	for(int j = 0; j < sizeof(DWORD)*8; j++)
	{
		if( (oce.pattern & (1 << j)) && cbass > 0)
		{
			--cbass;
			bass |= (1 << j);
		}
		if( (oce.pattern & (1 << (bits - 1 - j))) && cupper > 0)
		{
			--cupper;
			upper |= 1 << (bits - 1 - j);
		}
	}

	// now fill in rest of chord information
	for(int i = 0; i < MAX_POLY; i++)
	{
		pChord = (*this)[i];
		pChord->ChordRoot() = oce.root;
//		pChord->ScaleRoot() = oce.root;
		pChord->ScaleRoot() = 0;
		switch(i)
		{
		case 0:	// bass chord
			pChord->ChordPattern() = bass;
			pChord->InUse() = true;
			break;
		case 1: // upper chord
			pChord->ChordPattern() = upper;
			break;
		default:	// rest of the levels
			pChord->ChordPattern() = oce.pattern;
			break;
		}
		pChord->ScalePattern() = oce.scalepattern;
		pChord->Flags() = oce.varflags;
		pChord->UseFlat() = oce.flat;
		pChord->Bits() = oce.bits;
		pChord->InUse() = false;
	}
	InitLevels();
}

void DMPolyChord::Save(OldChordEntry& oce)
{
	oce.time = m_dwTime;
	oce.measure = m_nMeasure;
	oce.beat = m_bBeat;

	ASSERT(sizeof(m_szName) == sizeof(oce.name));	// catch wide char conversion

	strcpy(oce.name, m_szName);
	oce.keydown = m_keydown;
	DMChord* pChord = SubChord(m_bRootIndex);
	oce.root = pChord->ChordRoot();
	oce.pattern = pChord->ChordPattern();
	oce.varflags = pChord->Flags();
	oce.flat = pChord->UseFlat();
	oce.bits = pChord->Bits();
	oce.scalepattern = pChord->ScalePattern();
	oce.melodypattern = 0;
	oce.inscale = InScale(oce.root, oce.pattern, oce.scalepattern);
}

	// mass copy functions, from base to other subchords
void DMPolyChord::PropagateChordPattern()
{
	int root = static_cast<int>(m_bRootIndex);
	for(int i = 0; i < MAX_POLY; i++)
	{
		if(i == root)
			continue;
		m_Chords[i].ChordPattern() = m_Chords[root].ChordPattern();
	}
}

void DMPolyChord::PropagateScalePattern()
{
	int root = static_cast<int>(m_bRootIndex);
	for(int i = 0; i < MAX_POLY; i++)
	{
		if(i == root)
			continue;
		m_Chords[i].ScalePattern() = m_Chords[root].ScalePattern();
	}
}

void DMPolyChord::PropagateInvertPattern()
{
	int root = static_cast<int>(m_bRootIndex);
	for(int i = 0; i < MAX_POLY; i++)
	{
		if(i == root)
			continue;
		m_Chords[i].InvertPattern() = m_Chords[root].InvertPattern();
	}
}

void DMPolyChord::PropagateLevels()
{
	int root = static_cast<int>(m_bRootIndex);
	for(int i = 0; i < MAX_POLY; i++)
	{
		if(i == root)
			continue;
		m_Chords[i].Levels() = m_Chords[root].Levels();
	}
}

void DMPolyChord::PropagateFlags()
{
	int root = static_cast<int>(m_bRootIndex);
	for(int i = 0; i < MAX_POLY; i++)
	{
		if(i == root)
			continue;
		m_Chords[i].Flags() = m_Chords[root].Flags();
	}
}

void DMPolyChord::PropagateChordRoot()
{
	int root = static_cast<int>(m_bRootIndex);
	for(int i = 0; i < MAX_POLY; i++)
	{
		if(i == root)
			continue;
		m_Chords[i].ChordRoot() = m_Chords[root].ChordRoot();
	}
}

void DMPolyChord::PropagateScaleRoot()
{
	int root = static_cast<int>(m_bRootIndex);
	for(int i = 0; i < MAX_POLY; i++)
	{
		if(i == root)
			continue;
		m_Chords[i].ScaleRoot() = m_Chords[root].ScaleRoot();
	}
}

void DMPolyChord::PropagateUseFlat()
{
	int root = static_cast<int>(m_bRootIndex);
	for(int i = 0; i < MAX_POLY; i++)
	{
		if(i == root)
			continue;
		m_Chords[i].UseFlat() = m_Chords[root].UseFlat();
	}
}

void DMPolyChord::PropagateBits()
{
	int root = static_cast<int>(m_bRootIndex);
	for(int i = 0; i < MAX_POLY; i++)
	{
		if(i == root)
			continue;
		m_Chords[i].Bits() = m_Chords[root].Bits();
	}
}
void DMPolyChord::PropagateInUse()
{
	int root = static_cast<int>(m_bRootIndex);
	for(int i = 0; i < MAX_POLY; i++)
	{
		if(i == root)
			continue;
		m_Chords[i].InUse() = m_Chords[root].InUse();
	}
}

void DMPolyChord::PropagateAll()
{
	PropagateChordPattern();
	PropagateScalePattern();
	PropagateInvertPattern();
	PropagateLevels();
	PropagateFlags();
	PropagateChordRoot();
	PropagateScaleRoot();
	PropagateUseFlat();
	PropagateBits();
	PropagateInUse();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void swaplong(char data[])
{
    char temp;
    temp = data[0];
    data[0] = data[3];
    data[3] = temp;
    temp = data[1];
    data[1] = data[2];
    data[2] = temp;
}

void WriteID( IStream *pIStream, DWORD id )
{
	pIStream->Write( &id, 4, NULL );
}

void WriteSize( IStream *pIStream, long size )
{
	swaplong((char *)&size);
	pIStream->Write( &size, 4, NULL );
}

long ReadID( IStream *pIStream )
{
	DWORD	dwTemp;
	long	lID;
	
	pIStream->Read( &lID, 4, &dwTemp );
	return lID;
}

long ReadSize( IStream *pIStream )
{
    long	lTemp,
			lSize;
	
	pIStream->Read( &lSize, 4, (DWORD*)&lTemp );
	swaplong( (char*)&lSize );

	return lSize;
}


void ChordSelection::SetBits(int nSubChord)

{
	if(nSubChord >= MAX_POLY)
	{
		return;	// out of range
	}

	int nBegin = nSubChord < 0 ? 0 : nSubChord;
	int nEnd = nSubChord < 0 ? MAX_POLY : nSubChord + 1;
	for( int k = nBegin; k < nEnd; k++)
	{
		LONG    i ;
		short   count = 0 ;

		for( i=0L ;  i<32L ;  i++ ) {
			if( m_Chords[k].ChordPattern() & (1L << i) )
				count++ ;
		}
		
		if( !m_Chords[k].Bits() ) {
			m_Chords[k].Bits() |= CHORD_INVERT ;
			if( count > 3 )
				m_Chords[k].Bits() |= CHORD_FOUR ;
			if( m_Chords[k].ChordPattern() & (15L << 18L) )
				m_Chords[k].Bits() |= CHORD_UPPER ;
		}
		m_Chords[k].Bits() &= ~CHORD_COUNT ;
		m_Chords[k].Bits() |= count ;
	}
}

void ChordSelection::SetChordPattern(DWORD dwPattern, int nSubChord)
{
	int nBegin = nSubChord < 0 ? 0 : nSubChord;
	int nEnd = nSubChord < 0 ? MAX_POLY : nSubChord + 1;
	for( int k = nBegin; k < nEnd; k++)
	{
		m_Chords[k].ChordPattern() = dwPattern;
	}
}

void ChordSelection::SetScalePattern(DWORD dwPattern, int nSubChord)
{
	int nBegin = nSubChord < 0 ? 0 : nSubChord;
	int nEnd = nSubChord < 0 ? MAX_POLY : nSubChord + 1;
	for( int k = nBegin; k < nEnd; k++)
	{
		m_Chords[k].ScalePattern() = dwPattern;
	}
}

void ChordSelection::SetChordRoot(BYTE root, int nSubChord)
{
	int nBegin = nSubChord < 0 ? 0 : nSubChord;
	int nEnd = nSubChord < 0 ? MAX_POLY : nSubChord + 1;
	for( int k = nBegin; k < nEnd; k++)
	{
		m_Chords[k].ChordRoot() = root;
	}
}

void ChordSelection::SetScaleRoot(BYTE root, int nSubChord)
{
	int nBegin = nSubChord < 0 ? 0 : nSubChord;
	int nEnd = nSubChord < 0 ? MAX_POLY : nSubChord + 1;
	for( int k = nBegin; k < nEnd; k++)
	{
		m_Chords[k].ScaleRoot() = root;
	}
}

void ChordSelection::SetUseFlats(BYTE bFlat, int nSubChord)
{
	int nBegin = nSubChord < 0 ? 0 : nSubChord;
	int nEnd = nSubChord < 0 ? MAX_POLY : nSubChord + 1;
	for( int k = nBegin; k < nEnd; k++)
	{
		m_Chords[k].ScaleRoot() = bFlat;
	}
}


ChordSelection::ChordSelection()
{
}

BOOL ChordSelection::Equals(ChordSelection *second, int nSubChord)

{
	if(nSubChord >= MAX_POLY)
	{
		return FALSE;	// out of range
	}

	BOOL bMatch = TRUE;
	int nBegin = nSubChord < 0 ? 0 : nSubChord;
	int nEnd = nSubChord < 0 ? MAX_POLY : nSubChord + 1;
	for( int i = nBegin; bMatch && i < nEnd; i++)
	{
		if(!m_Chords[i].InUse())
		{
			// dont match empty chords
			continue;
		}
		char a = m_Chords[i].ChordRoot();
		char b = second->m_Chords[i].ChordRoot();
		while (a > 11) a -= 12;
		while (b > 11) b -= 12;
		bMatch = ((m_Chords[i].ChordPattern() == second->m_Chords[i].ChordPattern()) && (a == b));
	}
	return bMatch;
}


static char *convert[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
static char *flatconvert[] = {"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"};

void ChordSelection::RootToString(char *string)
{
	int nSubChord = (int)m_bRootIndex;
	char note = (char)( m_Chords[nSubChord].ChordRoot() % 12 );
	if( m_Chords[nSubChord].UseFlat() )
		wsprintf( string,"%d %s",1 + (m_Chords[nSubChord].ChordRoot() / 12), (LPSTR)flatconvert[note] ) ;
	else
		wsprintf( string,"%d %s",1 + (m_Chords[nSubChord].ChordRoot() / 12), (LPSTR)convert[note] ) ;
}

void ChordSelection::GetName(char *string)
{
	char root[20];
	RootToString(root);
	wsprintf(string,"%s %s",root,m_szName);
}



ChordPalette::ChordPalette()
{
	ASSERT(FALSE);	// make sure we don't call this
	InitChords(TRUE,FALSE,FALSE,FALSE);
}

static void autochord(ChordSelection *pchord, short type, short index, int nSubChord)

{


	static long chordpatternarray[8] = {
		0x91,0x89,0x49,0x891,0x491,0x489,0x249,0x449
	};
	static char *namearray[8] = {
		"M","m","o","M7","7","m7","o7","ø"
	};
	char maj3table[12] = { 0,0,1,0,1,0,0,0,0,1,0,2 } ;
	char min3table[12] = { 1,0,2,0,2,1,2,0,0,2,0,2 } ;
	char maj4table[12] = { 3,6,5,6,5,3,6,4,6,5,3,6 } ;
	char min4table[12] = { 1,3,6,0,6,5,6,4,3,7,4,6 } ;
	static long  scalearray[4] = {0xAB5AB5, //Maj3 
												0x9AD9AD, // min3
												0xAB5AB4, // Maj4
												0x9AD9AD }; // min4

	
	int nBegin = nSubChord < 0 ? 0 : nSubChord;
	int nEnd = nSubChord < 0 ? DMPolyChord::MAX_POLY : nSubChord + 1;
	ChordSelection& chord = *pchord;
	for( int i = nBegin; i < nEnd; i++)
	{
		chord[i]->ScalePattern() = scalearray[type];
		if( type == 0 ) {        // major triads.
			index = maj3table[index];
		}
		else if( type == 1 ) {        // minor triads.
			index = min3table[index];
		}
		else if( type == 2 ) {        // major sevenths.
			index = maj4table[index];
		}
		else 
		{ 
			index = min4table[index];
		}
		strcpy(chord.Name(),namearray[index]);
		chord[i]->ChordPattern() = chordpatternarray[index];
	}
}  

void ChordPalette::InitChords(BOOL major,BOOL allnotes,BOOL fat,BOOL flat)

{
	short key;
	short index;
	short type;
	long pattern;
	if (fat) type = 2;
	else type = 0;
	if (major) 
	{
		pattern = 0xAB5AB5; 
	}
	else
	{
		pattern = 0x9AD9AD; 
		type++;
	}
	for( key = 0;  key < 24;  key++ ) 
	{
		index = (short)(key % 12);
		if( pattern & (1 << index)) 
		{
			autochord(&m_chords[key], type, index, DMPolyChord::ALL ) ;
		}
		else if (allnotes) 
		{
			autochord(&m_chords[key], type, index, DMPolyChord::ALL ) ;
		}
		else  {
			*m_chords[key].Name() = 0;
			m_chords[key].SetChordPattern(1, DMPolyChord::ALL);
		}
		m_chords[key].SetChordRoot((char) key, DMPolyChord::ALL);
		m_chords[key].SetScaleRoot(0, DMPolyChord::ALL);
		m_chords[key].SetUseFlats((char) flat, DMPolyChord::ALL);
		m_chords[key].SetBits(DMPolyChord::ALL);
	}
} 

ChordEntry::ChordEntry()
{
	DWListItem();
	m_nid = 0;
	m_dwflags = 0;
	m_parent = 0;
	m_page = 0;
	m_nextchordlist.RemoveAll();
}   

void ChordEntry::LoadSmall(OldChordEntry& oce)
{
	m_nid = oce.nid;
	m_dwflags = oce.dwflags;
	m_chordsel.Load(oce);
}

void ChordEntry::LoadExtended(OldChordEntry& oce)
{
	m_page = oce.page;
	memcpy(&m_rect, &oce.rect, sizeof(m_rect));
	LoadSmall(oce);
}

void ChordEntry::LoadFromPropChord(CPropChord& chord)
{
	dynamic_cast<DMPolyChord&>(const_cast<ChordEntry*>(this)->m_chordsel) = chord;
	m_dwflags = 0;
}

void ChordEntry::SaveToPropChord(CPropChord& chord)
{
	dynamic_cast<DMPolyChord&>(chord) = dynamic_cast<DMPolyChord&>(const_cast<ChordEntry*>(this)->m_chordsel);
}

void ChordEntry::LoadFromChordItem(CChordItem& chord)
{
	m_chordsel.KeyDown() = chord.IsSelected() ? true : false;
	LoadFromPropChord(dynamic_cast<CPropChord&>(chord));
	if(chord.IsSignPost())
	{
/*
		if(chord.Measure() == 0)
		{
			m_dwflags |= CE_START;
		}
		else
		{
			m_dwflags |= CE_END;
		}
*/
		if(chord.IsBegSignPost())
		{
			m_dwflags |= CE_START;
		}
		if(chord.IsEndSignPost())
		{
			m_dwflags |= CE_END;
		}
	}
}


void ChordEntry::SaveSmall(OldChordEntry& oce)
{
	oce.nid = m_nid;
	oce.dwflags = m_dwflags;
	m_chordsel.Save(oce);
}

void ChordEntry::SaveExtended(OldChordEntry& oce)
{
	oce.page = m_page;
	memcpy(&oce.rect, &m_rect, sizeof(m_rect));
	SaveSmall(oce);
}

void ChordEntry::Debug()

{
	NextChord *pnext = m_nextchordlist.GetHead();
	TRACE("ChordEntry %ls: \n",m_chordsel.Name());
	for (;pnext;)
	{
		pnext->Debug();
		pnext = pnext->GetNext();
	}
}

void ChordEntry::AssignParents()

{
	NextChord *pnext = m_nextchordlist.GetHead();
	for (;pnext;)
	{
		pnext->m_parent = this;
		pnext = pnext->GetNext();
	}
}

ChordEntry::~ChordEntry()

{
	m_nextchordlist.ClearList();
}

void ChordSelection::SaveFile(char * /*filename*/)

{
	ASSERT(FALSE);
}

void ChordSelection::LoadFile(char * /*filename*/)

{
	ASSERT(FALSE);
}


void ChordEntry::CleanupNexts()

{
	NextChord *pnext = m_nextchordlist.GetHead();
	BOOL gotone = 0;
	CRect rect( m_rect.left, m_rect.top, m_rect.right, m_rect.bottom );
	rect.left += NEXT_INDENT;
	rect.bottom = rect.top;
	rect.top -= NEXT_HEIGHT;  
	for (;pnext;pnext = pnext->GetNext())
	{
		if (pnext->m_nextchord == NULL) gotone = TRUE;
		pnext->m_rect.left = (short)rect.left;
		pnext->m_rect.top = (short)rect.top;
		pnext->m_rect.right = (short)rect.right;
		pnext->m_rect.bottom = (short)rect.bottom;
		rect.bottom -= NEXT_HEIGHT;
		rect.top -= NEXT_HEIGHT;
	}
	if (gotone) return;
	pnext = new NextChord;
	if (pnext)
	{
		m_nextchordlist.AddTail(pnext);
		pnext->m_rect.left = (short)rect.left;
		pnext->m_rect.top = (short)rect.top;
		pnext->m_rect.right = (short)rect.right;
		pnext->m_rect.bottom = (short)rect.bottom;
		pnext->m_parent = this;
	}
}

void ChordEntry::Draw(CDC *pDC,short page)

{ 
	CPen *pOldPen;
	CPen pen;
	CBrush brush;
	CFont font;
	CFont *oldfont;
	NextChord *pnext;
	char string[50];
	if (page & m_page)
	{
		pnext = m_nextchordlist.GetHead();
		for (;pnext;pnext = pnext->GetNext())
		{
			pnext->Draw(pDC,page);
		}
		if (m_dwflags & CE_SELECTED) 
		{
			if (!pen.CreatePen(PS_SOLID,1,RGB(0xFF,0,0))) return; 
		} 
		else if (m_dwflags & CE_PATH)
		{ 
			if (!pen.CreatePen(PS_SOLID,1,RGB(0,0,0xFF))) return;   
		}
		else if (m_dwflags & CE_TREE)
		{ 
			if (!pen.CreatePen(PS_SOLID,1,RGB(0x77,0,0x77))) return;   
		}
		else 
		{ 
			if (!pen.CreatePen(PS_SOLID,1,RGB(0,0,0))) return;   
		}
		pOldPen = pDC->SelectObject(&pen);
		pDC->MoveTo(m_rect.left,m_rect.top);
		pDC->LineTo(m_rect.right,m_rect.top);
		pDC->LineTo(m_rect.right,m_rect.bottom);
		pDC->LineTo(m_rect.left,m_rect.bottom);
		pDC->LineTo(m_rect.left,m_rect.top);
		if (font.CreateFont(14,0,0,0,0,0,0,0,0,0,0,0,0,0))
		{
			oldfont = pDC->SelectObject(&font);
			strcpy(string, m_chordsel.Name());
			pDC->TextOut(m_rect.left + 4,m_rect.bottom - 2,string,strlen(string)); 
			pDC->SelectObject(oldfont);
		}      
	//	if (brush.CreateSolidBrush(RGB(0,0,0xFF)))
	//	{
	//		CBrush *pOldBrush;
	//		pOldBrush = pDC->SelectObject(&brush);
			if (m_dwflags & CE_MEASURE)
				pDC->Ellipse(m_rect.right - 14,m_rect.bottom - 4,
							m_rect.right - 4,m_rect.bottom - 14);  
			if (m_dwflags & CE_START)  
			{
				pDC->MoveTo(m_rect.left,(m_rect.top + m_rect.bottom) >> 1);
				pDC->LineTo(m_rect.left + CENTER_Y,m_rect.top); 
			}
			if (m_dwflags & CE_END)  
			{
				pDC->MoveTo(m_rect.right,(m_rect.top + m_rect.bottom) >> 1);
				pDC->LineTo(m_rect.right - CENTER_Y,m_rect.top); 
			}
	//		pDC->SelectObject(pOldBrush);
	//	}
		pDC->SelectObject(pOldPen);
	}
}

void NextChord::Draw(CDC *pDC,short page)

{ 
	CPen *pOldPen;
	CPen pen;
	CBrush brush;
	CFont font;
	CFont *oldfont;
	short cx,cy;
	if (m_dwflags & NC_SELECTED) 
	{
		if (!pen.CreatePen(PS_SOLID,1,RGB(0xFF,0,0))) return; 
	} 
	else if (m_dwflags & NC_TREE) 
	{
		if (!pen.CreatePen(PS_SOLID,1,RGB(0x77,0,0x77))) return; 
	} 
	else 
	{ 
		if (!pen.CreatePen(PS_SOLID,1,RGB(0,0,0))) return;   
	}
	pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(m_rect.right,m_rect.bottom);
	pDC->LineTo(m_rect.right,m_rect.top);
	pDC->LineTo(m_rect.left,m_rect.top);
	pDC->LineTo(m_rect.left,m_rect.bottom);
	if (m_nextchord)
	{
		cx = (short)(m_rect.right - 6);
		cy = (short)((m_rect.top + m_rect.bottom) >> 1);
		pDC->Ellipse(cx - 3,cy - 3,cx + 3, cy + 3);
		if (m_nextchord->m_page & page)
		{
			pDC->MoveTo(cx,cy);
			pDC->LineTo(m_nextchord->m_rect.left,
				(m_nextchord->m_rect.top + m_nextchord->m_rect.bottom) >> 1); 
		}
		if (brush.CreateSolidBrush(RGB(0,0,0)))
		{
			CBrush *pOldBrush; 
			short w = (short)(m_nweight * (m_rect.right - m_rect.left - (CHORD_WIDTH >> 1)));
			w /= 101;
			pOldBrush = pDC->SelectObject(&brush);
			pDC->Rectangle(m_rect.left + 3,m_rect.top + 3,
				m_rect.left + 3 + w,m_rect.bottom - 3);
			pDC->SelectObject(pOldBrush);
		}		
		if (font.CreateFont(10,0,0,0,0,0,0,0,0,0,0,0,0,0))
		{
			char string[20];
			oldfont = pDC->SelectObject(&font);
			wsprintf(string,"%d:%d",m_nminbeats,m_nmaxbeats);
			cx = (short)(m_rect.right - (CHORD_WIDTH >> 1) + 3);
			pDC->TextOut(cx,m_rect.bottom - 2,string,strlen(string)); 
			pDC->SelectObject(oldfont);
		} 
	}        
	pDC->SelectObject(pOldPen);
}

void NextChord::Debug()

{
	TRACE("NextChord: Flags: %lx, ",m_dwflags);
	if (m_nextchord) TRACE("Connecting to %ls\n",m_nextchord->m_chordsel.Name());
	else TRACE("No connection\n");
}

NextChord::NextChord()

{
	DWListItem();
	m_nextchord = NULL;
	m_nid = 0;
	m_dwflags = 0; 
	m_nweight = 50;
	m_nminbeats = 1;
	m_nmaxbeats = 12; 
	m_parent = 0;
}



long totalnext = 0;

void NextList::Save( IStream *pIStream, BOOL bSmall )
{
	NextChord *pnext = GetHead();
	long size;
	if( bSmall )
	{
		size = SmallNextChordFileSize; 
		WriteID( pIStream, ID_NEXTLIST );
	}
	else
	{
	    size = NextChordFileSize;
		WriteID( pIStream, ID_ENEXTLIST );
	}
	WriteSize( pIStream, Size(bSmall) );
	WriteSize( pIStream, size );
	for (;pnext;pnext = pnext->GetNext())
	{
		if( bSmall )
		{
			if (!pnext->m_nextchord) continue;
		}
		pIStream->Write( (void *)&pnext->m_dwflags, (short)size, NULL );
		totalnext++;
	}
}

long NextList::Size(BOOL bSmall)

{
	NextChord *pnext = GetHead();
	long size = 4;
	if( bSmall )
	{
		for (;pnext;pnext = pnext->GetNext())
		{
			if (pnext->m_nextchord) size += SmallNextChordFileSize;
		}
	}
	else size += (GetCount() * NextChordFileSize);
	return(size);
}

void NextList::Load( IStream *pIStream,long lsize)

{
	long lrecordsize = ReadSize( pIStream );
	lsize -= 4;
	ClearList();
	for (;lsize > 0;)
	{
		NextChord *pnext = new NextChord;
		if (pnext)
		{
			pIStream->Read( &pnext->m_dwflags, NextChordFileSize, NULL );
			pnext->SetNext(NULL);
			AddTail(pnext);
		}
		else
		{
			StreamSeek( pIStream, lrecordsize, STREAM_SEEK_CUR );
		}
		lsize -= lrecordsize;
	}
}

void NextList::ClearList()

{
	NextChord *pnext = GetHead();
	for (;pnext;)
	{
		NextChord *next = pnext->GetNext();
		delete pnext;
		pnext = next;	
	}
	RemoveAll();
}

SmallChordEntry::SmallChordEntry(const SmallChordEntry& sce)
	: m_chordsel(sce.m_chordsel), m_dwflags(sce.m_dwflags), m_nid(sce.m_nid)
{

}

void ChordEntry::CopyNextList(ChordEntry& sce, bool bCopyOnlySelected)
{
	for(NextChord* pnc = const_cast<NextList&>(sce.m_nextchordlist).GetHead();	pnc; pnc = pnc->GetNext())
	{
		
		if(!bCopyOnlySelected	// unconditional copy
				||  pnc->m_nextchord == NULL // always copy "extra" null next chord
				|| pnc->m_nextchord->m_chordsel.KeyDown()	// connected to chord is also selected
			
		   )
		   
		{
			NextChord* pnc1 = new NextChord(*pnc);
			pnc1->m_parent = this;	// copy the next list, but make parent the target chord
			m_nextchordlist.AddTail(pnc1);
		}
	}
}

ChordEntry::ChordEntry(const ChordEntry& ce)
	:SmallChordEntry(ce), m_page(ce.m_page), m_parent(ce.m_parent)
{
	memcpy(&m_rect, &(ce.m_rect), sizeof(m_rect));
	memcpy(&m_lastrect, &(ce.m_lastrect), sizeof(m_lastrect));
}

void ChordEntry::Save( IStream *pIStream, BOOL bSmall )
{
	OldChordEntry oce;
	if (bSmall)
	{
		WriteID( pIStream, ID_CHORDENTRY );
		WriteSize( pIStream, Size(bSmall) );
		WriteSize( pIStream, SmallChordEntryFileSize );
		SaveSmall(oce);
		pIStream->Write( &oce, SmallChordEntryFileSize, NULL ); 
	}
	else 
	{
		WriteID( pIStream, ID_ECHORDENTRY );
		WriteSize( pIStream, Size(bSmall) );
		WriteSize( pIStream, ChordEntryFileSize );
		SaveExtended(oce);
		pIStream->Write( &oce, ChordEntryFileSize, NULL );
	}
	m_nextchordlist.Save( pIStream,bSmall );
}


long ChordEntry::Size(BOOL bSmall, long version)
{
	if(version == ID_DMCHORDENTRY)
	{
		ASSERT(FALSE);	// not implemented yet
		return 0;
	}
	else
	{
		long lsize = 0; 
		lsize += 4;						// Size of record field. 
		if (bSmall) lsize += SmallChordEntryFileSize;
		else lsize += (ChordEntryFileSize);  
		lsize += (m_nextchordlist.Size(bSmall) + 8);
		return(lsize);
	}
}

ChordEntry *ChordEntry::Load( IStream *pIStream,long lfilesize, long id)

{
	ChordEntry *pchord = new ChordEntry;
	long lrecordsize = ReadSize( pIStream );
	lfilesize -= 4;
	if (pchord)
	{
		if(id == ID_DMCHORDENTRY)
		{
			ASSERT(FALSE);	// not implemented
		}
		else
		{
			OldChordEntry oce;
			pIStream->Read( &oce, lrecordsize, NULL );
			if(id == ID_ECHORDENTRY)
			{
				pchord->LoadExtended(oce);
			}
			else
			{
				pchord->LoadSmall(oce);
			}
			lfilesize -= lrecordsize;
			pchord->m_nextchordlist.RemoveAll();
			for (;lfilesize > 0;)
			{
				long dwid = ReadID( pIStream );
				lrecordsize = ReadSize( pIStream );
				lfilesize -= 8;
				if (dwid == ID_ENEXTLIST)
				{
					pchord->m_nextchordlist.Load(pIStream,lrecordsize);
				}
				else if (dwid == ID_NEXTLIST)
				{
					pchord->m_nextchordlist.Load(pIStream,lrecordsize);
				}
				else
				{
					StreamSeek( pIStream, lrecordsize, STREAM_SEEK_CUR );
				}
				lfilesize -= lrecordsize;
			}
		}
		if (lfilesize) TRACE("ERROR: Chord read leaves filesize at %ld\n",
			lfilesize);
	}
	else
	{
		StreamSeek( pIStream, lfilesize, STREAM_SEEK_CUR );
	}
	return(pchord);		
}

void ChordEntryList::ClearList()
{
	ChordEntry* pChord;
	while( pChord = RemoveHead() )
	{
		delete pChord;
	}
}

void Personality::SaveRef( IStream *pIStream,BOOL bSmall)

{ 
	if (bSmall) WriteID( pIStream, ID_PERSONREF ); 
	else WriteID( pIStream, ID_EPERSONREF );
	strcpy(m_personref.m_name,m_name);  
	if (bSmall) m_personref.m_isdefault = (char) (CM_DEFAULT & m_dwflags);
	else m_personref.m_isdefault = (char)((CM_DEFAULT | CM_EMBED) & m_dwflags);
	pIStream->Write( &m_personref, sizeof(PersonalityRef), NULL );
}

static char drivename[_MAX_DRIVE];
static char dirname[_MAX_DIR];

void SignPost::Save( IStream *pIStream )

{
	WriteID( pIStream, ID_SIGNPOST );
	WriteSize( pIStream, Size() );
	OldChordEntry oce;

	WriteSize( pIStream, 0L);	// must write space for signpost's DWListItem pointer

	WriteSize( pIStream, 0L);	// must write space for chord's DWListItem pointer
	m_chord.Save(oce);
	DWORD size = OldChordEntry::SizeChordSelection();
	pIStream->Write(&oce, size, NULL);
	WriteSize( pIStream, 0L); // must write space for pointer
	m_cadence[0].Save(oce);
	pIStream->Write(&oce, size, NULL);
	WriteSize( pIStream, 0L); // must write space for pointer
	m_cadence[1].Save(oce);
	pIStream->Write(&oce, size, NULL);

    pIStream->Write( &m_chords, sizeof(DWORD)*3, NULL );
}

long SignPost::Size(long version)
{
	if(version == ID_DMSIGNPOST)
	{
		ASSERT(FALSE); // not implemented
		return 0;
	}
	else
	{
		long size = OldChordEntry::SizeChordSelection()*(MAX_CADENCE+1);
		size += 3*sizeof(DWORD);
		size += 4*sizeof(void *);
		return size;
	}
}

SignPost *SignPost::Load( IStream *pIStream,long /*lfilesize*/)

{
    SignPost *psignpost = new SignPost;
	ReadSize(pIStream);	// move past sizeof(DWListItem) == sizeof(pointer)
	/*
	long id = ReadID(pIStream);
	ReadSize(pIStream);	// move past size record
	if(id == ID_DMSIGNPOST)
	{
		ASSERT(FALSE);	// not implemented
	}
	*/
	OldChordEntry oce;
	DWORD size = OldChordEntry::SizeChordSelection();
	ReadSize(pIStream);	// move past sizeof(DWListItem) == sizeof(pointer)
	pIStream->Read( &oce, size, NULL);
	psignpost->m_chord.Load(oce);
	ReadSize(pIStream);	// move past sizeof(DWListItem) == sizeof(pointer)
	pIStream->Read( &oce, size, NULL);
	psignpost->m_cadence[0].Load(oce);
	ReadSize(pIStream);	// move past sizeof(DWListItem) == sizeof(pointer)
	pIStream->Read( &oce, size, NULL);
	psignpost->m_cadence[1].Load(oce);
    pIStream->Read( &(psignpost->m_chords), sizeof(DWORD)*3, NULL );
    return(psignpost);      
}

PlayChord::PlayChord()
{
    m_pchord = NULL;
    m_pnext = NULL;
    m_dwflags = 0;
//  m_time = 0;
    m_beat = 0;
    m_measure = 0;
}

/*
static void setdirname(char *fullpath)
{ 
	_splitpath(fullpath,drivename,dirname,NULL,NULL);
}
*/
/*
static void getdirname(char *path)

{
	_makepath(path,drivename,dirname,NULL,NULL);
}
*/

Personality * Personality::LoadRef( IStream* /*pIStream*/,long /*lsize*/,BOOL /*bSmall*/)

{
/*
//	char filename[9];
	char fullfile[200];
	char dirname[120];
	PersonalityRef personref;

	pfile->ReadRecord(&personref,sizeof(PersonalityRef),lsize);
	getdirname(dirname);
    if (bSmall) wsprintf(fullfile,"%s%s.per",dirname,personref.m_filename);
    else wsprintf(fullfile,"%s%s.cmp",dirname,personref.m_filename);

	MyFile file ;
	unsigned long size, id ;
	Personality *pcmap = NULL;

TRACE("Loading Personality from file %s\n",fullfile);
	if(file.Open(fullfile, CFile::modeRead ))
	{
	    id = file.ReadID();
    	size = file.ReadSize();
	    if (!bSmall && (id == ID_EPERSONALITY))
    	{
			pcmap = Personality::Load(&file,size);
		}
		else if (bSmall && (id == ID_PERSONALITY))
		{
			pcmap = Personality::Load(&file,size);
		}
		file.Close();
	}
	if (pcmap) 
	{
		pcmap->m_personref = personref;
		pcmap->m_dwflags &= ~(CM_EMBED | CM_DEFAULT);
		pcmap->m_dwflags |= personref.m_isdefault;
	}
	return(pcmap);
*/
	return NULL;
}

void Personality::Save( IStream *pIStream, long version )
// only save personality, not contained objects
{
	if(version == ID_DMPERSONALITY)
	{
		ASSERT(FALSE);	// not implemented
	}
	else
	{
		WriteSize(pIStream, 0);	// Personality is a DWListItem, write out "pointer"
		pIStream->Write(&m_chordlist, sizeof(m_chordlist), NULL);	// m_chordlist
		pIStream->Write(&m_signpostlist, sizeof(m_signpostlist), NULL);
		pIStream->Write(&m_scalepattern, sizeof(m_scalepattern), NULL);
		pIStream->Write(&m_name, sizeof(m_name), NULL);
		pIStream->Write(&m_description, sizeof(m_description), NULL);
		pIStream->Write(&m_username, sizeof(m_username), NULL);
	
		m_chordpalette.Save(pIStream);

		pIStream->Write( &m_dwflags, sizeof(m_dwflags), NULL );
		pIStream->Write( &m_playlist, sizeof(m_playlist), NULL);
		pIStream->Write( &m_pfirstchord, sizeof(m_pfirstchord), NULL);
		pIStream->Write( &m_personref, sizeof(m_personref), NULL);
		pIStream->Write( &m_cx, sizeof(m_cx), NULL);
		pIStream->Write( &m_cy, sizeof(m_cy), NULL);
	}
}


void Personality::Load( IStream *pIStream,long /*lfilesize*/, long version)
// load only personality, not contained objects
{
	if(version == ID_DMPERSONALITY)
	{
		ASSERT(FALSE);	// not implemented
	}
	// else
	DWORD size;
	ReadSize(pIStream);	// Personality is a DWListItem, read past the pointer
	pIStream->Read(&m_chordlist, sizeof(m_chordlist), &size);	// m_chordlist
	pIStream->Read(&m_signpostlist, sizeof(m_signpostlist), &size);
	pIStream->Read(&m_scalepattern, sizeof(m_scalepattern), &size);
	pIStream->Read(&m_name, sizeof(m_name), &size);
	pIStream->Read(&m_description, sizeof(m_description), &size);
	pIStream->Read(&m_username, sizeof(m_username), &size);

	m_chordpalette.Load(pIStream);

	pIStream->Read( &m_dwflags, sizeof(m_dwflags), NULL );
	pIStream->Read( &m_playlist, sizeof(m_playlist), NULL);
	pIStream->Read( &m_pfirstchord, sizeof(m_pfirstchord), NULL);
	pIStream->Read( &m_personref, sizeof(m_personref), NULL);
	pIStream->Read( &m_cx, sizeof(m_cx), NULL);
	pIStream->Read( &m_cy, sizeof(m_cy), NULL);

//		pIStream->Read(pcmap,lrecordsize,NULL);
	m_pfirstchord = NULL;
	m_playlist.RemoveAll();
	m_chordlist.RemoveAll();
	m_signpostlist.RemoveAll();
}
	
	
long Personality::TotalSize(BOOL bSmall, long version)

{
	ChordEntry *pchord;
	long lsize = 0; 
	lsize += 4;						// Size of record field.

	if(version == ID_DMPERSONALITY)
	{
		ASSERT(FALSE);	// not implemented
		return 0;
	}
	else
	{
		lsize += PersonalitySize();	    	// Size of record.

		pchord = m_chordlist.GetHead();
		for(;pchord;pchord = pchord->GetNext())
		{       
			lsize += (pchord->Size(bSmall) + 8);
		}
		SignPost *psign;
		psign = m_signpostlist.GetHead();
		for(;psign;psign = psign->GetNext())
		{       
			lsize += (psign->Size() + 8);
		}
		return(lsize);
	}
}

long Personality::PersonalitySize(long version)
{
	if(version == ID_DMPERSONALITY)
	{
		return sizeof(Personality);
	}
	else
	{
		long size = sizeof(Personality) - sizeof(ChordPalette);
		size += (OldChordEntry::SizeChordSelection() + sizeof(void *)) * 24;
		return size;
	}
}


BOOL Personality::LoadFile(char* /*filename*/)

{
/*	MyFile file ;
	unsigned long size, id ;
	Personality *pcmap;

	if(file.Open(filename, CFile::modeRead ))
	{
	    id = file.ReadID();
    	size = file.ReadSize();
	    if (id == ID_EPERSONALITY)
    	{
			pcmap = Personality::Load(&file,size);
			if (pcmap)
			{
				Personality temp;
				temp = *this;
				*this = *pcmap;
				AssignParents();
				*pcmap = temp;
				delete pcmap;
				file.Close();
				_splitpath(filename,NULL,NULL,m_personref.m_filename,NULL);
				memset(&temp,0,sizeof(Personality));
				return(TRUE);
			}
		}
		file.Close();
	}
	return(FALSE);
*/
	return TRUE;
}

BOOL Personality::SaveFile(char* /*filename*/, BOOL /*bSmall*/)

{
/*
	MyFile file ;
	if(file.Open(filename, CFile::modeCreate |
	  	CFile::modeReadWrite | CFile::shareExclusive))
	{
		Save(&file,bSmall);
		file.Close();
		return(TRUE);
	}
	return(FALSE);
*/
	return TRUE;
}

BOOL ChordPalette::LoadFile(char* /*filename*/)

{
/*
	MyFile file;
	
	if (file.Open(filename, CFile::modeRead ))
	{
	    long id = file.ReadID();
    	long size = file.ReadSize();
	    if (id == ID_CHORDPALETTE)
    	{
			file.ReadRecord(this,sizeof(ChordPalette),size);
			file.Close();
			return(TRUE);
		}
		file.Close();
	}
	return(FALSE);                                    
*/
	return TRUE;
}

BOOL ChordPalette::SaveFile(char* /*filename*/)

{
/*
	MyFile file ;

	if(file.Open(filename, CFile::modeCreate |
	  	CFile::modeReadWrite | CFile::shareExclusive))
	{
		file.WriteID(ID_CHORDPALETTE);
		file.WriteRecord(this,sizeof(ChordPalette));
		file.Close();
		return(TRUE);
	}
	return(FALSE);
*/
	return TRUE;
}

void Personality::InsertChords(short leftedge,DWORD flags)

{
	CRect rect;
	ChordEntryList list;
	ChordEntry *pchord;
   	rect.left = leftedge;
   	rect.right = rect.left + CHORD_WIDTH;
   	rect.bottom = -20;
   	rect.top = rect.bottom - CHORD_HEIGHT;
	BuildNeededChords(&list,flags);
	m_chordlist.Cat(list.GetHead());
	pchord = list.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
		pchord->m_rect.top = (short)rect.top;
		pchord->m_rect.left = (short)rect.left;
		pchord->m_rect.right = (short)rect.right;
		pchord->m_rect.bottom = (short)rect.bottom;
		pchord->m_dwflags |= flags;  
		rect.bottom -= (CHORD_HEIGHT * 4);
		rect.top -= (CHORD_HEIGHT * 4);
		pchord->m_parent = this;
		pchord->CleanupNexts(); 
	}
	AssignIDs();
}


static BOOL inchordlist(ChordEntry *plist,ChordSelection *pchord,DWORD flags)

{
	for (;plist;plist = plist->GetNext())
	{
		if (pchord->Equals(&plist->m_chordsel, DMPolyChord::ALL) && (plist->m_dwflags & flags))
		{
			return(TRUE);
		}
	}
	return(FALSE);			
}

static void addchordtolist(ChordEntryList *list,ChordSelection *chord,DWORD flags)

{
	ChordEntry *pchord = new ChordEntry;
	if (pchord)
	{
		pchord->m_chordsel = *chord;
		pchord->m_dwflags = flags;
		list->AddTail(pchord);
	}
}

static void maybeputinlist(
	ChordEntryList *newlist,ChordEntryList *oldlist,
	ChordSelection *pcsel,DWORD flags)

{
	if (!inchordlist(newlist->GetHead(),pcsel,flags))
	{
		if (!inchordlist(oldlist->GetHead(),pcsel,flags))
		{
			addchordtolist(newlist,pcsel,flags);
		}
	}
}

void Personality::BuildNeededChords(ChordEntryList *list,DWORD flags)

{
	SignPost *psign = m_signpostlist.GetHead();
	for (;psign;psign = psign->GetNext())
	{
		maybeputinlist(list,&m_chordlist,&psign->m_chord,flags);
		if (flags & CE_END)
		{
			if (psign->m_dwflags & SPOST_CADENCE1)
			{
				maybeputinlist(list,&m_chordlist,&psign->m_cadence[0],flags);
			}
			else if (psign->m_dwflags & SPOST_CADENCE2)
			{
				maybeputinlist(list,&m_chordlist,&psign->m_cadence[1],flags);
			}
		}
	}
} 

Personality::Personality()

{
	static short count = 1;
	m_chordlist.RemoveAll();
	m_playlist.RemoveAll(); 
	m_signpostlist.RemoveAll();
	m_scalepattern = 0xAB5AB5; 
	m_description[0] = 0; 
	m_personref.m_stylename[0] = 0;
	wsprintf(m_name,"Personality %d",count++);
				
}

void PlayList::ClearList()

{
	PlayChord *pnext = GetHead();
	for (;pnext;)
	{
		PlayChord *next = pnext->GetNext();
		delete pnext;
		pnext = next;	
	}
	RemoveAll();
}

SignPost::SignPost()
{
    m_chords = SP_A | SP_B | SP_C;
    m_dwflags = SPOST_CADENCE1 | SPOST_CADENCE2;
    m_tempflags = 0;
}

BOOL Personality::PlayStop()

{
	m_playlist.ClearList();
	return(TRUE);
}

void Personality::AssignIDs()

{
	ChordEntry *pchord = m_chordlist.GetHead();   
	for (;pchord;pchord = pchord->GetNext())
	{ 
		if (!pchord->m_nid) 
		{
			short scan = 1;
			for (scan = 1;;scan++)
			{
				ChordEntry *pscan = m_chordlist.GetHead();   
				for (;pscan;pscan = pscan->GetNext())
				{
					if (pscan->m_nid == scan) break;
				}
				if (!pscan) break;
			}
			pchord->m_nid = scan;
		}
	}
}

void Personality::ResolveConnections()

{
	ChordEntry *pchord = m_chordlist.GetHead();   
	for (;pchord;pchord = pchord->GetNext())
	{
		NextChord *pnext = pchord->m_nextchordlist.GetHead();
		for (;pnext;pnext = pnext->GetNext())
		{
			pnext->m_nextchord = NULL;
		} 
	}
	pchord = m_chordlist.GetHead();   
	for (;pchord;pchord = pchord->GetNext())
	{
		NextChord *pnext = pchord->m_nextchordlist.GetHead();
		for (;pnext;pnext = pnext->GetNext())
		{
			if (pnext->m_nid)
			{
				ChordEntry *pconnect = m_chordlist.GetHead();   
				for (;pconnect;pconnect = pconnect->GetNext())
				{
                	if (pconnect->m_nid == pnext->m_nid) break;
                }
				pnext->m_nextchord = pconnect;
			}
		} 
	}
}

Personality::~Personality()

{
	ChordEntry *pchord = m_chordlist.GetHead();
	SignPost *psignpost = m_signpostlist.GetHead();
	PlayStop(); 
	for (;pchord;)
	{
		ChordEntry *next = pchord->GetNext();
		delete pchord;
		pchord = next;	
	}
	m_chordlist.RemoveAll();
	for (;psignpost;)
	{
		SignPost *next = psignpost->GetNext();
		delete psignpost;
		psignpost = next;	
	}
	m_signpostlist.RemoveAll();
}

void Personality::Debug()

{
	ChordEntry *pchord = m_chordlist.GetHead(); 
	TRACE("Chord List:\n");
	for (;pchord;pchord = pchord->GetNext())
	{
		pchord->Debug();
	}
/*	TRACE("Play List:\n");
	NextChord *pnext = (NextChord *) m_playlist.GetHead();
	for (;pnext;pnext = (NextChord *) pnext->GetNext())
	{
		pnext->Debug();
	}  */
}

void Personality::AssignParents()

{
	ChordEntry *pchord = m_chordlist.GetHead(); 
	for (;pchord;pchord = pchord->GetNext())
	{
		pchord->m_parent = this;
		pchord->AssignParents();
	}
/*	NextChord *pnext = (NextChord *) m_playlist.GetHead();
	for (;pnext;pnext = (NextChord *) pnext->GetNext())
	{
		pnext->m_parent = NULL;
	}  */
}

void Personality::Draw(CDC *pDC,short page)

{
	ChordEntry *pchord = m_chordlist.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
		pchord->Draw(pDC,page);
	}
}   

BOOL Personality::IsValidChord(ChordEntry *ptest)

{
	ChordEntry *pchord = m_chordlist.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
		if (pchord == ptest) return(TRUE);
	}
	return (FALSE);
	
}

BOOL Personality::IsValidNext(NextChord *ptest)

{
	ChordEntry *pchord = m_chordlist.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
		NextChord *pnext = pchord->m_nextchordlist.GetHead();
		for (;pnext;pnext = pnext->GetNext())
		{
			if (pnext == ptest) return(TRUE);
		}
	}
	return (FALSE);
}

void Personality::ClearChordFlags(unsigned long flag)

{
	ChordEntry *pchord = m_chordlist.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
		pchord->m_dwflags &= ~flag;
	}
}

void Personality::ClearNextFlags(unsigned long flag)

{
	ChordEntry *pchord = m_chordlist.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
		NextChord *pscan = pchord->m_nextchordlist.GetHead();
		for (;pscan;pscan = pscan->GetNext())
		{
			pscan->m_dwflags &= ~flag;
		}
	}
}


void ChordEntry::MarkTree()

{
	m_dwflags |= CE_TREE;
	NextChord *pscan = m_nextchordlist.GetHead();
	for (;pscan;pscan = pscan->GetNext())
	{
		pscan->MarkTree();
	}
}

void NextChord::MarkTree()

{
	m_dwflags |= NC_TREE;
	if (m_nextchord) m_nextchord->MarkTree();
}

void Personality::MarkTree(ChordEntry *pchord)

{
//	ClearNextFlags(NC_TREE);
//	ClearChordFlags(CE_TREE);
	if (pchord)
	{
		pchord->MarkTree();
	}
}

ChordEntry *Personality::FindXYChord(CPoint point,short page)

{
	CRect crTemp;
	ChordEntry *pchord = m_chordlist.GetHead();

	for (;pchord;pchord = pchord->GetNext())
	{
		if (pchord->m_page & page)
//		if (findall || (pchord->m_dwflags & (CE_TREE | CE_END)))
		{
			crTemp.left = pchord->m_rect.left;
			crTemp.right = pchord->m_rect.right;
			crTemp.top = pchord->m_rect.top;
			crTemp.bottom = pchord->m_rect.bottom;
//			if (pchord->m_rect.PtInRect(point)) break;
			if (crTemp.PtInRect(point)) break;
		}
	}
	return (pchord);
}

NextChord *Personality::FindXYNext(CPoint point,short page)

{
	CRect crTemp;

	ChordEntry *pchord = m_chordlist.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
		if (pchord->m_page & page)
//		if (findall || (pchord->m_dwflags & (CE_TREE | CE_END)))
		{
			NextChord *pnext = (NextChord *) pchord->m_nextchordlist.GetHead();
			for (;pnext;pnext = (NextChord *) pnext->GetNext())
			{
				crTemp.SetRect( pnext->m_rect.left, pnext->m_rect.top,
								pnext->m_rect.right, pnext->m_rect.bottom );
//				if (findall || (pnext->m_dwflags & NC_TREE))
//				{
//					if (pnext->m_rect.PtInRect(point)) return(pnext);  
					if (crTemp.PtInRect(point)) return(pnext);  
//				}
			}   
		}
	}
	return (NULL);
}   

BOOL Personality::CalcViewSize()

{
    CRect rect;
    CSize size;
    rect.SetRectEmpty(); 
	ChordEntry *pchord = m_chordlist.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
//		rect |= pchord->m_rect;
		rect.left |= pchord->m_rect.left;
  		rect.right |= pchord->m_rect.right;
  		rect.bottom |= pchord->m_rect.bottom;  
  		rect.top |= pchord->m_rect.top;  
    }
    size.cx = rect.Width(); + CHORD_WIDTH;
    size.cy = rect.Height() + CHORD_HEIGHT + (4 * NEXT_HEIGHT);
    if (size.cx == m_cx || size.cy == m_cy) return(FALSE);
    //m_size = size;
    return(TRUE);
}

BOOL Personality::GetChord(ChordSelection *pcsel,DWORD flag,short index)

{
	ChordEntry *pchord = m_chordlist.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
		if (pchord->m_dwflags & flag)
		{
			if (!index) 
			{
				*pcsel = pchord->m_chordsel; 
				return(TRUE);
			}
			index--;
		}
	}
	return(FALSE);
}

short Personality::GetChordIndex(ChordSelection *pcsel,DWORD flag)

{
	short index = 0;
	ChordEntry *pchord = m_chordlist.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
		if (pchord->m_dwflags & flag)
		{
			if (pcsel->Equals(&pchord->m_chordsel, DMPolyChord::ALL)) return(index); 
			index++;
		}
	}
	return(0);
}

short Personality::GetChordCount(DWORD flag)

{
	short count = 0;
	ChordEntry *pchord = m_chordlist.GetHead();
	for (;pchord;pchord = pchord->GetNext())
	{
		if (pchord->m_dwflags & flag) count++;
	}
	return(count);
}

Instruments::Instruments()

{
	static unsigned char defaults[16] = { 73,48,25,0,32,0,0,0,0,0,0,0,0,0,0,0 };
	short i;
	for (i=0;i<16;i++) m_patch[i] = defaults[i]; 
	strcpy(m_name,"new");
	m_default = 0;
}

GrooveName * GrooveNameList::GetGrooveName(DWORD grooveid)

{
	GrooveName *pgroove = GetHead();
	for (;pgroove;pgroove = pgroove->GetNext())
	{
		if (pgroove->m_grooveid == grooveid) break;
	}
	return (pgroove);
}

void GrooveNameList::AddGrooveName(char *name,DWORD grooveid)

{
	GrooveName *pgroove;
	pgroove = new GrooveName;
	if (pgroove)
	{
		strncpy(pgroove->m_name,name,sizeof(pgroove->m_name));
		pgroove->m_grooveid = grooveid;
		AddTail(pgroove);
	}
}

void GrooveNameList::SetDefaults()

{
	AddGrooveName("Relaxed",PF_A);
	AddGrooveName("Medium",PF_B);
	AddGrooveName("Active",PF_C);
	AddGrooveName("Frantic",PF_D);
}
     
void Instruments::Save( IStream *pIStream )

{ 
	WriteID( pIStream, ID_INSTRUMENTS );
	pIStream->Write( this, sizeof( Instruments ), NULL );
}

long Instruments::Size()

{
	return(sizeof(Instruments));
}

Instruments * Instruments::Load( IStream *pIStream,long /*lfilesize*/)

{
	Instruments *pinst = new Instruments;
	pIStream->Read( pinst, sizeof(Instruments), NULL );
	return(pinst);		
}
   
Instruments * Instruments::LoadFile(char* /*filename*/)

{
/*	MyFile file ;
	unsigned long size, id ;
	Instruments *pinst;

	if(file.Open(filename, CFile::modeRead ))
	{
	    id = file.ReadID();
    	size = file.ReadSize();
	    if (id == ID_INSTRUMENTS)
    	{
			pinst = Instruments::Load(&file,size);
		}
		file.Close();
	}
	return(pinst);
*/
	return NULL;
}

void Instruments::SaveFile(char* /*filename*/)

{
/*	MyFile file ;

	if(file.Open(filename, CFile::modeCreate |
	  	CFile::modeReadWrite | CFile::shareExclusive))
	{
		Save(&file);
		file.Close();
	}
*/
}

GrooveName::GrooveName()

{
	strcpy(m_name,"Duh...");
	m_grooveid = 0;
}

void GrooveName::Save( IStream *pIStream )

{ 
	WriteID( pIStream, ID_GROOVENAME );
	pIStream->Write(this, sizeof(GrooveName), NULL );
}

long GrooveName::Size()

{
	return(sizeof(GrooveName));
}

GrooveName * GrooveName::Load( IStream *pIStream,long /*lfilesize*/)

{
	GrooveName *pinst = new GrooveName;
	pIStream->Read( pinst, sizeof(GrooveName),NULL );
	return(pinst);		
}


void InstrumentsList::ClearList()

{
	Instruments *pinst;
	while ( (pinst = GetHead()) != 0 )
	{
		Remove(pinst);
		delete pinst;
	}
}

void GrooveNameList::ClearList()

{
	GrooveName *pgroove;
	while ( (pgroove = GetHead()) != 0 )
	{
		Remove(pgroove);
		delete pgroove;
	}
}

void PersonalityList::ClearList()

{
	Personality *map;
	while ( (map = GetHead()) != 0 )
	{
		Remove(map);
		delete map;
	}
}

#define ID_STYL        0x5354594C

/*
static void makenewname(char * filename)

{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char file[30];

	_splitpath(filename,drive,dir,file,NULL);
	_makepath(filename,drive,dir,file,".sct"); 
}
*/
/*
static BOOL copyfile(char * oldfile,char * newfile)

{
	CFile source;
	CFile dest;
	if (dest.Open(newfile,CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive))
	{
		if (source.Open(oldfile,CFile::modeRead | CFile::shareExclusive))
		{
			int length = 200;
			char buff[200];
			while (length > 0)
			{
				length = source.Read(buff,200);
				dest.Write(buff,length);
			}
			source.Close();
			dest.Close();
			return(TRUE);
		}
		dest.Close();
	}
	return(FALSE);	
}
*/




void ChordEntryToChordExt(ChordEntry* pChordEntry, ChordExt* pChordExt)
{
	ASSERT(pChordEntry);
	ASSERT(pChordExt);


	*dynamic_cast<DMPolyChord*>(pChordExt) = pChordEntry->m_chordsel;
//	pChordExt->KeyDown() = pChordEntry->KeyDown();
}


long ChordPalette::Size(long version)
{
	if(version == ID_DMCHORDPALETTE)
	{
		return sizeof(ChordPalette);
	}
	else
	{
		return 24 * (sizeof(void*) + OldChordEntry::SizeChordSelection());
	}
}

HRESULT ChordPalette::Save(IStream* pStream, long version)
{
	HRESULT hr = S_OK;
	OldChordEntry oce;
	for(int i = 0; i < 24; i++)
	{
		if(version == ID_DMCHORDPALETTE)
		{
			ASSERT(FALSE);	// not implemented yet
		}
		else
		{
			WriteSize(pStream, 0);	// dwlistitem ptr
			m_chords[i].Save(oce);
			hr = pStream->Write(&oce, OldChordEntry::SizeChordSelection(), NULL);
		}
	}
	return hr;
}

HRESULT ChordPalette::Load(IStream* pStream, long version)
{
	HRESULT hr = S_OK;
	OldChordEntry oce;
	for(int i = 0; i < 24; i++)
	{
		if(version == ID_DMCHORDPALETTE)
		{
			ASSERT(FALSE);	// not implemented yet
		}
		else
		{
			ReadSize(pStream);	// dwlistitem ptr
			hr = pStream->Read(&oce, OldChordEntry::SizeChordSelection(), NULL);
			m_chords[i].Load(oce);
		}
	}
	return hr;
}




void DMPolyChord::InitLevels()
{
	for(int i = 0; i < MAX_POLY; i++)
	{
		SubChord(i)->Levels() = (1 << i);
	}

}
