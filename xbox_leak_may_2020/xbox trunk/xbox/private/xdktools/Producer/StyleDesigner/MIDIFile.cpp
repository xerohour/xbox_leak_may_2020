//
// midifile.cpp
//
// original author: Dave Miller
// orignal project: AudioActive
// modified by: Mark Burton
// Adopted for use in DMUSProducer by: Jeff Fried
// project: DirectMusic
//
#define assert ASSERT
#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "styleriff.h"
#include "RiffStrm.h"
#include <mmreg.h>
#include <math.h>

#include "midifile.h"
void CreateChordFromKey(char cSharpsFlats, BYTE bMode, DWORD dwTime, DMUS_CHORD_PARAM& rChord,
							DWORD* scalepattern=0, DWORD* chordpattern=0, BYTE* scaleroot=0, BYTE* chordroot=0);
static DMUS_CHORD_PARAM g_Chord; // Holds the latest chord
static DMUS_CHORD_PARAM g_DefaultChord; // in case no chords are extracted from the track

// specific to pattern
static DWORD	gdwDefaultKeyPattern = 0xAB5AB5;	// Major	
static DWORD	gdwDefaultChordPattern = 0x891;	// M7
static DWORD	gbDefaultKeyRoot = 0;		
static DWORD	gbDefaultChordRoot = 12;		

static DWORD	gdwKeyPattern;
static DWORD	gdwChordPattern;
static BYTE	gbKeyRoot;
static BYTE	gbChordRoot;
static BOOL gfKeyFlatsNotSharps;
static BOOL gfChordFlatsNotSharps;

static bool	gKeySigExists = false;


// Flags for CBandInstrument::m_dwFlags. When set, a flag indicates that the 
// corresponding field in CBandInstrument should be used to generate
// a MIDI event as well as information about the instrument

#define DMBI_PATCH			(1 << 0)		// m_dwPatch is valid.
#define DMBI_VOLUME			(1 << 1)		// m_bVolume is valid
#define DMBI_PAN			(1 << 2)		// m_bPan is valid
#define DMBI_BANKSELECT_MSB	(1 << 3)		// m_dwPatch contains a valid Bank Select MSB part
#define DMBI_BANKSELECT_LSB	(1 << 4)		// m_dwPatch contains a valid Bank Select LSB part
#define DMBI_GM				(1 << 5)		// Instrument is from GM collection
#define DMBI_GS				(1 << 6)		// Instrument is from GS collection
#define DMBI_ASSIGN_PATCH	(1 << 7)		// m_AssignPatch is valid


bool IsGS(DWORD dwBank, DWORD dwInstrument)
{
	BYTE	bMSB, bLSB, bPatch;

	bMSB = (BYTE) ((dwBank & 0x00007F00) >> 8);
	bLSB = (BYTE) (dwBank & 0x0000007F);
	bPatch = (BYTE) (dwInstrument & 0x0000007F);
	
	if (bLSB != 0) return false;

	if ( (bMSB == 0) ) // && (bLSB == 0) 
	{
		if ((dwBank & 0x80000000)) //Drum Kit
		{
			if ((bPatch == 0x0)  ||
				(bPatch == 0x08) ||
				(bPatch == 0x10) ||
				(bPatch == 0x18) ||
				(bPatch == 0x19) ||
				(bPatch == 0x20) ||
				(bPatch == 0x28) ||
				(bPatch == 0x30) || 
				(bPatch == 0x38) )
			{
				return  true;
			}
			else
				return false;
		}
		else return true;//is GM
	}
	// check for GS
	switch (bMSB)
	{
		case 6:
		case 7:
			if (bPatch == 0x7D) return true;
			break;
		case 24:
			if ((bPatch == 0x04) || (bPatch == 0x06)) return true;
			break;
		case 9:
			if ((bPatch == 0x0E) || (bPatch == 0x76) || (bPatch == 0x7D)) return true;
			break;
		case 2:
			if ( (bPatch == 0x66) || (bPatch == 0x78) || ((bPatch > 0x79)&&(bPatch < 0x80) )) return true;
			break;
		case 3:
			if ((bPatch > 0x79) && (bPatch < 0x80)) return true;
			break;
		case 4:
		case 5:
			if ( (bPatch == 0x7A) || ((bPatch > 0x7B)&&(bPatch < 0x7F) )) return true;
			break;
		case 32:
			if ((bPatch == 0x10) ||
				(bPatch == 0x11) ||
				(bPatch == 0x18) ||
				(bPatch == 0x34) ) return true;
			break;
		case 1:
			if ((bPatch == 0x26) ||
				(bPatch == 0x39) ||
				(bPatch == 0x3C) ||
				(bPatch == 0x50) ||
				(bPatch == 0x51) ||
				(bPatch == 0x62) ||
				(bPatch == 0x66) ||
				(bPatch == 0x68) ||
				((bPatch > 0x77) && (bPatch < 0x80))) return true;
				break;
		case 16:
			switch (bPatch)
			{
				case 0x00:
					return true;
					break;
				case 0x04:
					return true;
					break;
				case 0x05:
					return true;
					break;
				case 0x06:
					return true;
					break;
				case 0x10:
					return true;
					break;
				case 0x13:
					return true;
					break;
				case 0x18:
					return true;
					break;
				case 0x19:
					return true;
					break;
				case 0x1C:
					return true;
					break;
				case 0x27:
					return true;
					break;
				case 0x3E:
					return true;
					break;
				case 0x3F:
					return true;
					break;
				default:
					return false;
			}
			break;
		case 8:
			if ((bPatch < 0x07) || ((bPatch == 0x7D)))
			{
				return true;
			}
			else if ((bPatch > 0x3F) && (bPatch < 0x50))
			{
				return false;
			}
			else if ((bPatch > 0x4F) && (bPatch < 0x72)  )
			{
				if ((bPatch == 0x50) || 
					(bPatch == 0x51) ||
					(bPatch == 0x6B))
				{
					return true;
				}
				return false;
			}
			else if ((bPatch > 0x1F) && (bPatch < 0x40))
			{
				if ((bPatch > 0x25) && (bPatch < 0x29) ||
					(bPatch > 0x3C)  ||
					(bPatch == 0x30) || 
					(bPatch == 0x32) )
				{
					return true;
				}
				return false;
			}
			else if ((bPatch > 0x0A) && (bPatch < 0x12) && 
				     (bPatch != 0x0D) && (bPatch != 0x0F))
			{
				return true;
			}
			else if ((bPatch > 0x0F) && (bPatch < 0x20))
			{
				if (bPatch > 0x17)
				{
					return true;
				}
				else if ( (bPatch == 0x13) || (bPatch == 0x15) )
					return true;
				else
					return false;
			}
			break;
		default:
			return false;
	}
	return false;
}



inline bool operator == (const ULARGE_INTEGER& i1, const ULARGE_INTEGER& i2)
{
	if(i1.LowPart == i2.LowPart && i1.HighPart == i2.HighPart)
		return true;
	else
		return false;
}


// One for each MIDI channel 0-15	
DMUS_IO_BANKSELECT_ITEM gBankSelect[NUM_MIDI_CHANNELS];

DMUS_IO_BANKSELECT_ITEM& MIDIBankSelect(int ch)
{
	return gBankSelect[ch];
}


DWORD gdwLastControllerTime[NUM_MIDI_CHANNELS];	// Holds the time of the last CC event
DWORD gdwControlCollisionOffset[NUM_MIDI_CHANNELS];

void MIDIInit()
{
	gKeySigExists = false;
	MIDIInitBankSelect();
}

void MIDIInitBankSelect()
{
	FillMemory(&gBankSelect, (sizeof(DMUS_IO_BANKSELECT_ITEM) * NUM_MIDI_CHANNELS), 0xFF);
}


void MIDIInitChordOfComposition()
{
	CreateChordFromKey(0, 0, 0, g_Chord, &gdwKeyPattern, &gdwChordPattern, &gbKeyRoot, &gbChordRoot);
	CreateChordFromKey(0, 0, 0, g_DefaultChord, &gdwKeyPattern, &gdwChordPattern, &gbKeyRoot, &gbChordRoot);
}

void MIDIGetChordOfComposition(DMUS_CHORD_PARAM& chord)
{
	memcpy(&chord, &g_Chord, sizeof(DMUS_CHORD_PARAM));
}


void MIDIGetChordOfComposition(DWORD& keypattern, DWORD& chordpattern, BYTE& keyroot, 
							   BYTE& chordroot, BOOL& fKeyFlatsNotSharps, BOOL& fChordFlatsNotSharps)
{
	if( gKeySigExists )
	{
		keypattern = gdwKeyPattern;
		chordpattern = gdwChordPattern;
		keyroot = gbKeyRoot;
		chordroot = gbChordRoot;
		fKeyFlatsNotSharps = gfKeyFlatsNotSharps;
		fChordFlatsNotSharps = gfChordFlatsNotSharps;
	}
}

/*
HRESULT LoadCollection(IDirectMusicCollection** ppIDMCollection,
					   IDirectMusicLoader* pIDMLoader)
{
	// Any changes made to this function should also be made to CDirectMusicBand::LoadCollection
	// in dmband.dll

	ASSERT(ppIDMCollection);
	ASSERT(pIDMLoader);

	DMUS_OBJECTDESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);

	desc.guidClass = CLSID_DirectMusicCollection;
	wcscpy(desc.wszFileName, L"gm.dls");	
	desc.dwValidData |= (DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME);
	
	LPDMUS_OBJECT m_pDMObject = NULL;

	HRESULT hr = pIDMLoader->GetObject(&m_pDMObject, &desc);

	if(SUCCEEDED(hr))
	{
		hr = m_pDMObject->QueryInterface(IID_IDirectMusicCollection, (void**)ppIDMCollection);
	}

	if(m_pDMObject)
	{
		m_pDMObject->Release();
	}
	
	return hr;
}
*/

#ifndef  _CRIFFStream_
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

// this function gets a long that is formatted the correct way
// i.e. the motorola way as opposed to the intel way
BOOL __inline GetMLong( LPSTREAM pStream, DWORD& dw )
{
    union uLong
	{
		unsigned char buf[4];
	DWORD dw;
	} u;
    unsigned char ch;

    if( S_OK != pStream->Read( u.buf, 4, NULL ) )
    {
	return FALSE;
    }


#ifndef _MAC
    // swap bytes
    ch = u.buf[0];
    u.buf[0] = u.buf[3];
    u.buf[3] = ch;

    ch = u.buf[1];
    u.buf[1] = u.buf[2];
    u.buf[2] = ch;
#endif

    dw = u.dw;
    return TRUE;
}

#endif


static short snPPQN;
//static IStream* gpTempoStream = NULL;
//static IStream* gpSysExStream = NULL;
//static IStream* gpTimeSigStream = NULL;
//static BOOL	gfSetTempo = FALSE;
//static CDirectMusicPattern* gpPattern = NULL;


short& MIDIsnPPQN()
{
	return snPPQN;
}


/*
DMUS_IO_TIMESIGNATURE_ITEM	gTimeSigItem;


long	glTimeSig = 1; // flag to see if we should be paying attention to time sigs.
	// this is needed because we only care about the time sigs on the first track to
	// contain them that we read -- need to initialize this in createPatternfromMidi

DMUS_IO_TIMESIGNATURE_ITEM& MIDITimeSig()
{
	return gTimeSigItem;
}

long& MIDITimeSigFlag()
{
	return glTimeSig;
}
*/

WORD MIDIGetVarLength( LPSTREAM pStream, DWORD& rfdwValue )
{
    BYTE b;
    WORD wBytes;

    if( S_OK != pStream->Read( &b, 1, NULL ) )
    {
		rfdwValue = 0;
		return 0;
    }
    wBytes = 1;
    rfdwValue = b & 0x7f;
    while( ( b & 0x80 ) != 0 )
    {
		if( S_OK != pStream->Read( &b, 1, NULL ) )
		{
			break;
		}
		++wBytes;
		rfdwValue = ( rfdwValue << 7 ) + ( b & 0x7f );
    }
    return wBytes;
}

#ifdef _MAC
DWORD MIDIConvertTime( DWORD dwTime )
{
	wide d;
	long l;  // storage for the remainder

	if( snPPQN == DMUS_PPQ )  {
		return dwTime;
	}
	WideMultiply( dwTime, DMUS_PPQ, &d );
	return WideDivide( &d, snPPQN, &l );
}
#else
DWORD MIDIConvertTime( DWORD dwTime )
{
    __int64 d;

    if( snPPQN == DMUS_PPQ )
    {
		return dwTime;
    }
    d = dwTime;
    d *= DMUS_PPQ;
    d /= snPPQN;
    return (DWORD)d;
}
#endif

FullSeqEvent* MIDICompressEventList( FullSeqEvent* lstEvent )
{
    static FullSeqEvent* paNoteOnEvent[16][128];
    FullSeqEvent* pEvent;
    FullSeqEvent* pPrevEvent;
    FullSeqEvent* pNextEvent;
    int nChannel;

	ZeroMemory( paNoteOnEvent, sizeof( paNoteOnEvent ) );
    pPrevEvent = NULL;

    for( pEvent = lstEvent ; pEvent != NULL ; pEvent = pNextEvent )
    {
		pEvent->pTempNext = NULL;
		pNextEvent = pEvent->pNext;
		nChannel = pEvent->dwPChannel;
		if( ( pEvent->bStatus & 0xf0 ) == MIDI_NOTEON )
		{
			// add this event to the end of the list of events based
			// on the event's pitch. Keeping track of multiple events
			// of the same pitch allows us to have overlapping notes
			// of the same pitch, choosing that note on's and note off's
			// follow in the same order.
			if( NULL == paNoteOnEvent[nChannel][pEvent->bByte1] )
			{
				paNoteOnEvent[nChannel][pEvent->bByte1] = pEvent;
			}
			else
			{
				FullSeqEvent* pScan;
				for( pScan = paNoteOnEvent[nChannel][pEvent->bByte1];
					 pScan->pTempNext != NULL; pScan = pScan->pTempNext );
				pScan->pTempNext = pEvent;
			}
		}
		else if( ( pEvent->bStatus & 0xf0 ) == MIDI_NOTEOFF )
		{
			if( paNoteOnEvent[nChannel][pEvent->bByte1] != NULL )
			{
				paNoteOnEvent[nChannel][pEvent->bByte1]->mtDuration =
					pEvent->mtTime - paNoteOnEvent[nChannel][pEvent->bByte1]->mtTime;
				paNoteOnEvent[nChannel][pEvent->bByte1] =
					paNoteOnEvent[nChannel][pEvent->bByte1]->pTempNext;
			}
			if( pPrevEvent == NULL )
			{
				lstEvent = pNextEvent;
			}
			else
			{
				pPrevEvent->pNext = pNextEvent;
			}
			delete pEvent;
			continue;
		}
		pPrevEvent = pEvent;
    }

    for( pEvent = lstEvent ; pEvent != NULL ; pEvent = pEvent->pNext )
    {
//		pEvent->mtTime =  MIDIConvertTime(pEvent->mtTime);
		if( ( pEvent->bStatus & 0xf0 ) == MIDI_NOTEON )
		{
//			pEvent->mtDuration = MIDIConvertTime( pEvent->mtDuration );
			if( pEvent->mtDuration == 0 ) pEvent->mtDuration = 1;
		}
    }

    return lstEvent;
}

static int CompareEvents( FullSeqEvent* pEvent1, FullSeqEvent* pEvent2 )
{
    if( pEvent1->mtTime < pEvent2->mtTime )
    {
	return -1;
    }
    else if( pEvent1->mtTime > pEvent2->mtTime )
    {
	return 1;
    }
    else if( (pEvent1->bStatus & 0xF0) != MIDI_SYSX
		&&	 (pEvent2->bStatus & 0xF0) != ET_SYSX )
    {
	BYTE bStatus1;
	BYTE bStatus2;

	bStatus1 = (BYTE)( pEvent1->bStatus & 0xf0 );
	bStatus2 = (BYTE)( pEvent2->bStatus & 0xf0 );
	if( bStatus1 == bStatus2 )
	{
	    return 0;
	}
	else if( bStatus1 == MIDI_NOTEON )
	{
	    return -1;
	}
	else if( bStatus2 == MIDI_NOTEON )
	{
	    return 1;
	}
	else if( bStatus1 > bStatus2 )
	{
	    return 1;
	}
	else if( bStatus1 < bStatus2 )
	{
	    return -1;
	}
    }
    return 0;
}

FullSeqEvent* MIDIMergeEvents( FullSeqEvent* lstLeftEvent, FullSeqEvent* lstRightEvent )
{
    FullSeqEvent  anchorEvent;
    FullSeqEvent* pEvent;

    anchorEvent.pNext = NULL;
    pEvent = &anchorEvent;

    do
    {
	if( CompareEvents( lstLeftEvent, lstRightEvent ) < 0 )
	{
	    pEvent->pNext = lstLeftEvent;
	    pEvent = lstLeftEvent;
	    lstLeftEvent = lstLeftEvent->pNext;
	    if( lstLeftEvent == NULL )
	    {
		pEvent->pNext = lstRightEvent;
	    }
	}
	else
	{
	    pEvent->pNext = lstRightEvent;
	    pEvent = lstRightEvent;
	    lstRightEvent = lstRightEvent->pNext;
	    if( lstRightEvent == NULL )
	    {
		pEvent->pNext = lstLeftEvent;
		lstLeftEvent = NULL;
	    }
	}
    } while( lstLeftEvent != NULL );

    return anchorEvent.pNext;
}

FullSeqEvent* MIDIRemoveDuplicateCCs( FullSeqEvent* lstEvent )
{
#ifdef _DEBUG
	BYTE bCurrentCCValue[16][128];
	BYTE bCurPitchBendByte1[16];
	BYTE bCurPitchBendByte2[16];
	BYTE bCurPTouchValue[16][128];
	BYTE bCurMTouchValue[16];
#endif

	bool fHaveMTouch[16];
	bool fHavePBend[16];
	DWORD adwHavePTouch[16][8];
	DWORD adwHaveCC[16][8];

	FullSeqEvent *pLastEvent = NULL;
	FullSeqEvent *pCurEvent = lstEvent;
	while( pCurEvent )
	{
		// Initialize our flags
		ZeroMemory( fHaveMTouch, sizeof(bool) * 16 );
		ZeroMemory( fHavePBend, sizeof(bool) * 16 );
		ZeroMemory( adwHavePTouch, sizeof(DWORD) * 8 * 16 );
		ZeroMemory( adwHaveCC, sizeof(DWORD) * 8* 16 );

		// Set the current time
		const MUSIC_TIME mtCurrent = pCurEvent->mtTime;
		while( pCurEvent
			&& mtCurrent == pCurEvent->mtTime )
		{
			// The PChannel # must be less than 16
			ASSERT( pCurEvent->dwPChannel < 16 );

			FullSeqEvent *pNextEvent = pCurEvent->pNext;
			switch( pCurEvent->bStatus & 0xF0 )
			{
			case MIDI_PBEND:
				if( fHavePBend[pCurEvent->dwPChannel] )
				{
#ifdef _DEBUG
					TRACE("Throwing away PBend on %d at %d with value %d.  Keeping Value %d\n",
						pCurEvent->dwPChannel,
						pCurEvent->mtTime,
						(int)(((pCurEvent->bByte2 & 0x7F) << 7) + (pCurEvent->bByte1 & 0x7F)),
						(int)(((bCurPitchBendByte2[pCurEvent->dwPChannel] & 0x7F) << 7) + (bCurPitchBendByte1[pCurEvent->dwPChannel] & 0x7F)) );
#endif

					// pLastEvent should always be valid, because we never throw away the first event
					ASSERT( pLastEvent );
					pLastEvent->pNext = pNextEvent;
					pCurEvent->pNext = NULL;
					delete pCurEvent;
					pCurEvent = NULL;
				}
				else
				{
					fHavePBend[pCurEvent->dwPChannel] = true;
#ifdef _DEBUG
					bCurPitchBendByte1[pCurEvent->dwPChannel] = pCurEvent->bByte1;
					bCurPitchBendByte2[pCurEvent->dwPChannel] = pCurEvent->bByte2;
#endif
				}
				break;
			case MIDI_MTOUCH:
				if( fHaveMTouch[pCurEvent->dwPChannel] )
				{
#ifdef _DEBUG
					TRACE("Throwing away MTouch on %d at %d with value %d.  Keeping Value %d\n",
						pCurEvent->dwPChannel,
						pCurEvent->mtTime,
						pCurEvent->bByte1,
						bCurMTouchValue[pCurEvent->dwPChannel] );
#endif

					// pLastEvent should always be valid, because we never throw away the first event
					ASSERT( pLastEvent );
					pLastEvent->pNext = pNextEvent;
					pCurEvent->pNext = NULL;
					delete pCurEvent;
					pCurEvent = NULL;
				}
				else
				{
					fHaveMTouch[pCurEvent->dwPChannel] = true;
#ifdef _DEBUG
					bCurMTouchValue[pCurEvent->dwPChannel] = pCurEvent->bByte1;
#endif
				}
				break;
			case MIDI_PTOUCH:
				if( adwHavePTouch[pCurEvent->dwPChannel][pCurEvent->bByte1 / 32] & (0x1 << (pCurEvent->bByte1 & 0x1F)) )
				{
#ifdef _DEBUG
					TRACE("Throwing away PTouch %d on %d at %d with value %d.  Keeping Value %d\n",
						pCurEvent->bByte1,
						pCurEvent->dwPChannel,
						pCurEvent->mtTime,
						pCurEvent->bByte2,
						bCurPTouchValue[pCurEvent->dwPChannel][pCurEvent->bByte1] );
#endif

					// pLastEvent should always be valid, because we never throw away the first event
					ASSERT( pLastEvent );
					pLastEvent->pNext = pNextEvent;
					pCurEvent->pNext = NULL;
					delete pCurEvent;
					pCurEvent = NULL;
				}
				else
				{
					adwHavePTouch[pCurEvent->dwPChannel][pCurEvent->bByte1 / 32] |= (0x1 << (pCurEvent->bByte1 & 0x1F));
#ifdef _DEBUG
					bCurPTouchValue[pCurEvent->dwPChannel][pCurEvent->bByte1] = pCurEvent->bByte2;
#endif
				}
				break;
			case MIDI_CCHANGE:
				if( adwHaveCC[pCurEvent->dwPChannel][pCurEvent->bByte1 / 32] & (0x1 << (pCurEvent->bByte1 & 0x1F)) )
				{
#ifdef _DEBUG
					TRACE("Throwing away CC %d on %d at %d with value %d.  Keeping Value %d\n",
						pCurEvent->bByte1,
						pCurEvent->dwPChannel,
						pCurEvent->mtTime,
						pCurEvent->bByte2,
						bCurrentCCValue[pCurEvent->dwPChannel][pCurEvent->bByte1] );
#endif

					// pLastEvent should always be valid, because we never throw away the first event
					ASSERT( pLastEvent );
					pLastEvent->pNext = pNextEvent;
					pCurEvent->pNext = NULL;
					delete pCurEvent;
					pCurEvent = NULL;
				}
				else
				{
					adwHaveCC[pCurEvent->dwPChannel][pCurEvent->bByte1 / 32] |= (0x1 << (pCurEvent->bByte1 & 0x1F));
#ifdef _DEBUG
					bCurrentCCValue[pCurEvent->dwPChannel][pCurEvent->bByte1] = pCurEvent->bByte2;
#endif
				}
				break;
			}
			if( pCurEvent )
			{
				pLastEvent = pCurEvent;
			}
			pCurEvent = pNextEvent;
		}
	}
	return lstEvent;
}

FullSeqEvent* MIDIReverseEventList( FullSeqEvent* lstEvent )
{
    FullSeqEvent* lstNew = NULL;

	while( lstEvent )
	{
		// Save a poiter to the next event
		FullSeqEvent *pNextEvent = lstEvent->pNext;

		// Redirect the event to the start of lstNew
		lstEvent->pNext = lstNew;
		lstNew = lstEvent;

		// Move lstEvent to the next event
		lstEvent = pNextEvent;
	}

    return lstNew;
}

FullSeqEvent* MIDISortEventList( FullSeqEvent* lstEvent )
{
    FullSeqEvent* pMidEvent;
    FullSeqEvent* pRightEvent;

    if( lstEvent != NULL && lstEvent->pNext != NULL )
    {
	pMidEvent = lstEvent;
	pRightEvent = pMidEvent->pNext->pNext;
	if( pRightEvent != NULL )
	{
	    pRightEvent = pRightEvent->pNext;
	}
	while( pRightEvent != NULL )
	{
	    pMidEvent = pMidEvent->pNext;
	    pRightEvent = pRightEvent->pNext;
	    if( pRightEvent != NULL )
	    {
		pRightEvent = pRightEvent->pNext;
	    }
	}
	pRightEvent = pMidEvent->pNext;
	pMidEvent->pNext = NULL;
	return MIDIMergeEvents( MIDISortEventList( lstEvent ),
			    MIDISortEventList( pRightEvent ) );
    }
    return lstEvent;
}

DWORD MIDIReadEvent( LPSTREAM pStream, DWORD dwTime, FullSeqEvent** plstEvent, DMUS_IO_PATCH_ITEM** pplstPatchEvent, LPSTR* pszTrackName )
{
    static BYTE bRunningStatus;

	dwTime = MIDIConvertTime(dwTime);

    DWORD dwBytes;
    DWORD dwLen;
    FullSeqEvent* pEvent;
	DMUS_IO_PATCH_ITEM* pPatchEvent;
    DMUS_IO_SYSEX_ITEM* pSysEx;
    BYTE b;

	BYTE* pbSysExData = 0;

    if( FAILED( pStream->Read( &b, 1, NULL ) ) )
    {
		return 0;
    }

    if( b < 0x80 )
    {
		StreamSeek( pStream, -1, STREAM_SEEK_CUR );
		b = bRunningStatus;
		dwBytes = 0;
    }
    else
    {
		dwBytes = 1;
    }

    if( b < 0xf0 )
    {
		bRunningStatus = (BYTE)b;

		switch( b & 0xf0 )
		{
		case MIDI_CCHANGE:
		case MIDI_PTOUCH:
		case MIDI_PBEND:
		case MIDI_NOTEOFF:
		case MIDI_NOTEON:
			if( FAILED( pStream->Read( &b, 1, NULL ) ) )
			{
				return dwBytes;
			}
			++dwBytes;

			pEvent = new FullSeqEvent;
			if( pEvent == NULL )
			{
				return 0;
			}

			pEvent->mtTime = dwTime;
			pEvent->mtDuration = 0;
			pEvent->nOffset = 0;
			pEvent->bStatus = static_cast<BYTE>(bRunningStatus & 0xf0);
			pEvent->dwPChannel = bRunningStatus & 0xf;
			pEvent->bByte1 = b;
//			( (NoteEvent*)pEvent )->voiceid = (char)( ( ( bRunningStatus - 4 ) & 0xf ) + 1 );
			if( FAILED( pStream->Read( &b, 1, NULL ) ) )
			{
				delete pEvent;
				return dwBytes;
			}
			++dwBytes;
			pEvent->bByte2 = b;

			if( ( pEvent->bStatus & 0xf0 ) == MIDI_NOTEON &&
				pEvent->bByte2 == 0 )
			{
				pEvent->bStatus = (BYTE)( MIDI_NOTEOFF | ( pEvent->bStatus & 0xf ) );
			}

//			pEvent->bEventType = static_cast<BYTE>( pEvent->bStatus >> 4 );

			if(((pEvent->bStatus & 0xf0) == MIDI_CCHANGE) && (pEvent->bByte1 == 0 || pEvent->bByte1 == 0x20))
			{
				// We have a bank select or its LSB either of which are not added to event list
				if(pEvent->bByte1 == 0x20)
				{
					gBankSelect[pEvent->bStatus & 0xf].byLSB = pEvent->bByte2;
				}
				else // pEvent->bByte1 == 0
				{
					gBankSelect[pEvent->bStatus & 0xf].byMSB = pEvent->bByte2;
				}
				// We no longer need the event so we can free it
				delete pEvent;
			}
			else // Add to event list
			{
				pEvent->pNext = *plstEvent;
				*plstEvent = pEvent;
			}
			break;

		case MIDI_PCHANGE:
		{
			if(FAILED(pStream->Read(&b, 1, NULL)))
			{
				return dwBytes;
			}
			
			++dwBytes;

			pPatchEvent = new DMUS_IO_PATCH_ITEM;

			if(pPatchEvent == NULL)
			{
				return 0;
			}

			pPatchEvent->lTime = dwTime - 1;
			pPatchEvent->byStatus = bRunningStatus;
			pPatchEvent->byPChange = b;	// byte 0 in dwPatch
			pPatchEvent->byMSB = gBankSelect[bRunningStatus & 0xF].byMSB;
			pPatchEvent->byLSB = gBankSelect[bRunningStatus & 0xF].byLSB;
			pPatchEvent->dwFlags &= 0;
			pPatchEvent->dwFlags |= DMUS_IO_INST_PATCH;

			if(pPatchEvent->byMSB != 0xFF)
			{
				pPatchEvent->dwFlags |= DMBI_BANKSELECT_MSB;						
			}
			
			if(pPatchEvent->byLSB != 0xFF)
			{
				pPatchEvent->dwFlags |= DMBI_BANKSELECT_LSB;				
			}

			if(!(pPatchEvent->dwFlags & (DMBI_BANKSELECT_MSB | DMBI_BANKSELECT_LSB) ) )
			{
				// no bank select in effect, defaults to GM
					pPatchEvent->dwFlags |= DMUS_IO_INST_GM;
			}
			else
			{
				// test and set GM/GS flags
				DWORD dwPatch = pPatchEvent->byPChange & 0x7F
												| (pPatchEvent->byLSB & 0x7F) << 8
												| (pPatchEvent->byMSB & 0x7F) << 16;

				DWORD dwInstrument = dwPatch & 0xFF;
				DWORD dwBank = (dwPatch & ~(0x0 | 0xFF)) >> 8;

				if(IsGS(dwBank, dwInstrument))
				{
					pPatchEvent->dwFlags |= DMUS_IO_INST_GS;
					if(dwBank == 0 || (dwBank & ~(0x80000000)) == 0)
					{
						pPatchEvent->dwFlags |= DMUS_IO_INST_GM;
					}
				}
			}

			pPatchEvent->pNext = *pplstPatchEvent;
			pPatchEvent->pIDMCollection = NULL;

			*pplstPatchEvent = pPatchEvent;

			break;
		}
		case MIDI_MTOUCH:
			if( FAILED( pStream->Read( &b, 1, NULL ) ) )
			{
				return dwBytes;
			}
			++dwBytes;
			pEvent = new FullSeqEvent;
			if( pEvent == NULL )
			{
				return 0;
			}

			pEvent->mtTime = dwTime;
			pEvent->mtDuration = 0;
			pEvent->nOffset = 0;
			pEvent->bStatus = static_cast<BYTE>(bRunningStatus & 0xf0);
			pEvent->dwPChannel = bRunningStatus & 0xf;
			pEvent->bByte1 = b;
//			( (NoteEvent*)pEvent )->voiceid = (char)( ( ( bRunningStatus - 4 ) & 0xf ) + 1 );
//			pEvent->bEventType = static_cast<BYTE>( pEvent->bStatus >> 4 );

			pEvent->pNext = *plstEvent;
			*plstEvent = pEvent;
			break;
		default:
			// this should NOT be possible - unknown midi note event type
			ASSERT(FALSE);
			break;
		}
    }
    else
    {
		switch( b )
		{
		case 0xf0:
			dwBytes += MIDIGetVarLength( pStream, dwLen );
			pSysEx = new DMUS_IO_SYSEX_ITEM;
			if( pSysEx != NULL )
			{
				pbSysExData = new BYTE[dwLen + 1];
				if( pbSysExData != NULL )
				{
					pbSysExData[0] = 0xf0;
					if( FAILED( pStream->Read( pbSysExData + 1, dwLen, NULL ) ) )
					{
						delete []pbSysExData;
						delete pSysEx;
						return dwBytes;
					}

					if( pbSysExData[1] == 0x43 )
					{
						extern bool gbIsXG;
						// check for XG files
						BYTE abXG[] = { 0xF0, 0x43, 0x10, 0x4C, 0x00, 0x00, 0x7E, 0x00, 0xF7 };
						int i;
						if(dwLen >= 8)
						{
							for( i = 0; i < 8; i++ )
							{
								if( i == 2 )
								{
									//if( ( pSysEx->pbSysExData[i] & 0xF0 ) != abXG[i] )
									if( ( pbSysExData[i] & 0xF0 ) != abXG[i] )
										break;
								}
								else
								{
									//if( pSysEx->pbSysExData[i] != abXG[i] )
									if( pbSysExData[i] != abXG[i] )
										break;
								}
							}
							if( i == 8 ) // we have an XG!
							{
								// what direct music does
	//							TListItem<StampedGMGSXG>* pPair = new TListItem<StampedGMGSXG>;
	//							if (!pPair) return dwBytes;
	//                        	mt -= 2; // so it's before any PC
	//							pPair->GetItemValue().mtTime = mt;
	//							pPair->GetItemValue().dwMidiMode = DMUS_MIDIMODEF_XG;
	//							InsertMidiMode(pPair);

								// what producer does
								gbIsXG = true;
							}
						}
					}

					pSysEx->dwPChannel = 0;
					pSysEx->mtTime = dwTime;
					DWORD dwTempLen = dwLen + 1;
					pSysEx->dwSysExLength = dwTempLen;
					/*
					if( NULL == gpSysExStream )
					{
						// create a stream to hold sysex events
						CreateStreamOnHGlobal( NULL, TRUE, &gpSysExStream );
					}
					if( gpSysExStream )
					{
						// bugbug error checking needs to be here
						gpSysExStream->Write( &pSysEx->lTime, sizeof(long), NULL );
						gpSysExStream->Write( &pSysEx->dwSysExLength, sizeof(DWORD), NULL );
						gpSysExStream->Write( pSysEx->pbSysExData, dwLen, NULL );
					}
					*/
					delete []pbSysExData;
					delete pSysEx;
				}
				else
				{
					StreamSeek( pStream, dwLen, STREAM_SEEK_CUR );
				}
			}
			else
			{
				StreamSeek( pStream, dwLen, STREAM_SEEK_CUR );
			}
			dwBytes += dwLen;
			break;
		case 0xf7:
			// ignore sysex f7 chunks
			dwBytes += MIDIGetVarLength( pStream, dwLen );
			StreamSeek( pStream, dwLen, STREAM_SEEK_CUR );
			dwBytes += dwLen;
			break;
		case 0xff:
			if( FAILED( pStream->Read( &b, 1, NULL ) ) )
			{
				return dwBytes;
			}
			++dwBytes;
			dwBytes += MIDIGetVarLength( pStream, dwLen );
			if( b == 0x51 ) // tempo change
			{
				DWORD dw = 0;
//				DMUS_IO_TEMPO_ITEM tempo;

				while( dwLen > 0 )
				{
					if( FAILED( pStream->Read( &b, 1, NULL ) ) )
					{
						return dwBytes;
					}
					++dwBytes;
					--dwLen;
					dw <<= 8;
					dw += b;
				}
				/*
				tempo.lTime = ConvertTime( dwTime );
				tempo.dblTempo = 60000000 / (double)dw;
				if( gpSegment && !gfSetTempo )
				{
					gpSegment->m_dblTempo = tempo.dblTempo;
					gfSetTempo = TRUE;
				}
				if( NULL == gpTempoStream )
				{
					// create a stream to hold tempo events
					CreateStreamOnHGlobal( NULL, TRUE, &gpTempoStream );
				}
				if( gpTempoStream )
				{
					gpTempoStream->Write( &tempo, sizeof(DMUS_IO_TEMPO_ITEM), NULL );
					// bugbug error checking needs to be here
				}
				*/
			}
			// We don't care about TimeSigs - just use the Style's TimeSig.
			/*
			else if( b == 0x58 && glTimeSig )
			{
				// glTimeSig will be set to 0 inside the main calling function
				// once we no longer care about time sigs.
				if( FAILED( pStream->Read( &b, 1, NULL ) ) )
				{
					return dwBytes;
				}
				// set glTimeSig to 2 to signal to the main function that we've
				// read a time sig on this track
				glTimeSig = 2;
				gTimeSigItem.lTime = dwTime;
				gTimeSigItem.bBeatsPerMeasure = b;
				++dwBytes;
				if( FAILED( pStream->Read( &b, 1, NULL ) ) )
				{
					return dwBytes;
				}
				gTimeSigItem.bBeat = (BYTE)( 1 << b ); // 0 means 256th note
				gTimeSigItem.wGridsPerBeat = 4; // default
				++dwBytes;
				StreamSeek( pStream, dwLen - 2, STREAM_SEEK_CUR );
				dwBytes += ( dwLen - 2 );
			}
			*/
			else if( b == 0x59 )
			{
				// Read sharps/flats and major/minor bytes
				if( FAILED( pStream->Read( &b, 1, NULL ) ) )
				{
					return dwBytes;
				}
				char cSharpsFlats = b;
				++dwBytes;
				if( FAILED( pStream->Read( &b, 1, NULL ) ) )
				{
					return dwBytes;
				}
				BYTE bMode = b;
				++dwBytes;

				if(!gKeySigExists)
				{
					// Create a chord (with one subchord) from the key info
					CreateChordFromKey(cSharpsFlats, bMode, dwTime, g_Chord, 
										&gdwKeyPattern, &gdwChordPattern, &gbKeyRoot, &gbChordRoot);
					gfKeyFlatsNotSharps = cSharpsFlats < 0 ? TRUE : FALSE;
					gfChordFlatsNotSharps = gfKeyFlatsNotSharps;
					gKeySigExists = true;
				}
			}
			else if( b == 0x03 )
			{
				// MIDI Sequence name/Track name/Subtitle
				*pszTrackName = new char[dwLen + 2];
				if( (*pszTrackName == NULL) || FAILED( pStream->Read( *pszTrackName, dwLen, NULL ) ) )
				{
					if( *pszTrackName != NULL )
					{
						delete []*pszTrackName;
						*pszTrackName = NULL;
					}
					return dwBytes;
				}

				// Add the null terminator
				(*pszTrackName)[dwLen] = 0;
				dwBytes += dwLen;
			}
			else
			{
				StreamSeek( pStream, dwLen, STREAM_SEEK_CUR );
				dwBytes += dwLen;
			}
			break;
		default:
	//            DisplayDebug( 1, "Unknown midi event type: 0x%x", b );
			break;
		}
    }
    return dwBytes;
}

void CopyPatchItem(DMUS_IO_PATCH_ITEM* to, DMUS_IO_PATCH_ITEM* from)
{
	memcpy(to, from, sizeof(DMUS_IO_PATCH_ITEM));
	to->pNext = 0;
}

FullSeqEvent *DeleteEventFromList( FullSeqEvent *pList, FullSeqEvent *pEvent )
{
	if( pList == NULL )
	{
		return NULL;
	}
	if( pEvent == NULL )
	{
		return pList;
	}

	if( pList == pEvent )
	{
		pList = pEvent->pNext;
		pEvent->pNext = NULL;
		delete pEvent;
	}
	else
	{
		FullSeqEvent *pLast = pList;
		FullSeqEvent *pTmp = pList->pNext;
		while( (pTmp != pEvent) && pTmp)
		{
			pLast = pTmp;
			pTmp = pTmp->pNext;
		}

		ASSERT( pLast );
		ASSERT( pTmp );
		ASSERT( pTmp == pEvent );

		if( pLast && pTmp )
		{
			pLast->pNext = pTmp->pNext;
			pTmp->pNext = NULL;
			delete pTmp;
		}
	}

	return pList;
}

HRESULT WriteBandTrack(IDMUSProdRIFFStream* pRiffStream, DMUS_IO_PATCH_ITEM** lstPatchEvent, int nTracks, FullSeqEvent **lstEvent)
// this function (in ListOfBands) deletes patch change and zeros out lstPatchEvent, assuming its successful
// makes a band from first patch change on each channel
// This also eats the first volume and pan change, if they occur before (or concurrently with) the first note
{
	ASSERT(pRiffStream);

	// extract first patch change on each channel
	DMUS_IO_PATCH_ITEM*	lstFirstPatches = 0;
	DWORD	fFirstPatches = 0;
	
	for(DMUS_IO_PATCH_ITEM* pItem = *lstPatchEvent; pItem; pItem = pItem->pNext)
	{
		BYTE ch = (BYTE)(pItem->byStatus & 0xF);
		if(!(fFirstPatches & (1 << ch)))
		{
			// first patch change for this style
			DMUS_IO_PATCH_ITEM* pfirst = new DMUS_IO_PATCH_ITEM;
			CopyPatchItem(pfirst, pItem);
			pfirst->lTime = -1;
			lstFirstPatches = List_Cat(lstFirstPatches, pfirst);
			fFirstPatches |= (1 << ch);
		}
	}

	// Now, extract first vol/pan changes on each channel
	MUSIC_TIME mtEarliestVol[16] = {LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX};
	MUSIC_TIME mtEarliestPan[16] = {LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX,LONG_MAX};
	FullSeqEvent *lstEarliestVol[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	FullSeqEvent *lstEarliestPan[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int nTrackEarliestVol[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	int nTrackEarliestPan[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	for( int nIndex = nTracks - 1; nIndex >= 0; nIndex-- )
	{
		FullSeqEvent *curEvent = lstEvent[nIndex];
		while( curEvent )
		{
			if( curEvent->bStatus == MIDI_CCHANGE )
			{
				if( curEvent->bByte1 == 7 )
				{
					// Check if volume is earlier than any other volume on this channel
					if( (curEvent->mtTime + curEvent->nOffset) < mtEarliestVol[curEvent->dwPChannel] )
					{
						mtEarliestVol[curEvent->dwPChannel] = curEvent->mtTime + curEvent->nOffset;
						lstEarliestVol[curEvent->dwPChannel] = curEvent;
						nTrackEarliestVol[curEvent->dwPChannel] = nIndex;
					}
				}
				else if( curEvent->bByte1 == 10 )
				{
					// Check if pan is earlier than any other pan on this channel
					if( (curEvent->mtTime + curEvent->nOffset) < mtEarliestPan[curEvent->dwPChannel] )
					{
						mtEarliestPan[curEvent->dwPChannel] = curEvent->mtTime + curEvent->nOffset;
						lstEarliestPan[curEvent->dwPChannel] = curEvent;
						nTrackEarliestPan[curEvent->dwPChannel] = nIndex;
					}
				}
			}
			else if( curEvent->bStatus == MIDI_NOTEON )
			{
				// Check if note is earlier than earliest volume on this channel
				if( (curEvent->mtTime + curEvent->nOffset) < mtEarliestVol[curEvent->dwPChannel] )
				{
					mtEarliestVol[curEvent->dwPChannel] = curEvent->mtTime + curEvent->nOffset;
					lstEarliestVol[curEvent->dwPChannel] = NULL;
					nTrackEarliestVol[curEvent->dwPChannel] = -1;
				}

				// Check if note is earlier than earliest pan on this channel
				if( (curEvent->mtTime + curEvent->nOffset) < mtEarliestPan[curEvent->dwPChannel] )
				{
					mtEarliestPan[curEvent->dwPChannel] = curEvent->mtTime + curEvent->nOffset;
					lstEarliestPan[curEvent->dwPChannel] = NULL;
					nTrackEarliestPan[curEvent->dwPChannel] = -1;
				}

				break;
			}

			curEvent = curEvent->pNext;
		}
	}

	WriteRiffChunk riff(pRiffStream, DMUS_FOURCC_BAND_FORM);
	HRESULT hr = riff.State();
	if(hr == S_OK)
	{
		ListOfInst listinst(lstFirstPatches, lstEarliestVol, lstEarliestPan);
		hr = listinst.Write(pRiffStream);
	}

	// Delete the volume/pan events from the lists
	for( nIndex = 0; nIndex < 16; nIndex++ )
	{
		lstEvent[nTrackEarliestVol[nIndex]] = DeleteEventFromList( lstEvent[nTrackEarliestVol[nIndex]], lstEarliestVol[nIndex] );
		lstEvent[nTrackEarliestPan[nIndex]] = DeleteEventFromList( lstEvent[nTrackEarliestPan[nIndex]], lstEarliestPan[nIndex] );
	}

	List_Free(lstFirstPatches);
	List_Free(*lstPatchEvent);
	*lstPatchEvent = 0;

	return hr;
}




// Creates and returns (in rChord) a DMUS_CHORD_PARAM given the three input params.
// the new chord will have one subchord containing the root, third, fifth, and seventh
// of the key (as indicated by the sharps/flats and mode).  Scale will be either
// major or minor, depending on the mode (mode is 0 if major, 1 if minor).
#pragma warning(disable:4244)
const DWORD adwFlatScales[8] = {
	0x00AB5AB5,
	0x006B56B5,
	0x006AD6AD,
	0x005AD5AD,
	0x005AB5AB,
	0x0056B56B,
	0x00D6AD6A,
	0x00D5AD5A};
const DWORD adwSharpScales[8] = {
	0x00AB5AB5,
	0x00AD5AD5,
	0x00AD6AD6,
	0x00B56B56,
	0x00B5AB5A,
	0x00D5AD5A,
	0x00D6AD6A,
	0x0056B56B};

const bool bShiftScale = false;	// work around for bug: in producer scale root always 0, and scale pattern
								// shifted accordingly
void CreateChordFromKey(char cSharpsFlats, BYTE bMode, DWORD dwTime, DMUS_CHORD_PARAM& rChord,
						DWORD* scalepattern, DWORD* chordpattern, BYTE* scaleroot, BYTE* chordroot)
{
	TRACE("Creating a chord.  Key: %d Mode: %d Time: %d\n", cSharpsFlats, bMode, dwTime);
	static DWORD dwMajorScale = 0xab5ab5;	// 1010 1011 0101 1010 1011 0101
	static DWORD dwMinorScale = 0x5ad5ad;	// 0101 1010 1101 0101 1010 1101
	static DWORD dwMajor7Chord = 0x891;		// 1000 1001 0001
	static DWORD dwMinor7Chord = 0x489;		// 0100 1000 1001
	BYTE bScaleRoot = 0;
	BYTE bChordRoot = 0;
	bMode = 0;		// for now disable the ability to distinguish modes
					// will need to update MidiStripMgr::AddChordTrack to deal with this
	switch (cSharpsFlats)
	{
	case  0: bChordRoot = bMode ?  9 :  0; break;
	case  1: bChordRoot = bMode ?  4 :  7; break;
	case  2: bChordRoot = bMode ? 11 :  2; break;
	case  3: bChordRoot = bMode ?  6 :  9; break;
	case  4: bChordRoot = bMode ?  1 :  4; break;
	case  5: bChordRoot = bMode ?  8 : 11; break;
	case  6: bChordRoot = bMode ?  3 :  6; break;
	case  7: bChordRoot = bMode ? 10 :  1; break;
	case -1: bChordRoot = bMode ?  2 :  5; break;
	case -2: bChordRoot = bMode ?  7 : 10; break;
	case -3: bChordRoot = bMode ?  0 :  3; break;
	case -4: bChordRoot = bMode ?  5 :  8; break;
	case -5: bChordRoot = bMode ? 10 :  1; break;
	case -6: bChordRoot = bMode ?  3 :  6; break;
	case -7: bChordRoot = bMode ?  8 : 11; break;
	}

	// transpose up an octave
	bChordRoot += 12;
	bScaleRoot = bChordRoot;

	if (bMode)
	{
		wcscpy(rChord.wszName, L"m7");
	}
	else
	{
		wcscpy(rChord.wszName, L"M7");
	}
	DMUS_IO_TIMESIGNATURE_ITEM timesig;
	timesig.bBeat = 4;
	timesig.bBeatsPerMeasure = 4;
	timesig.wGridsPerBeat = 4;
	DWORD dwAbsBeat = dwTime / (DMUS_PPQ * (4 / timesig.bBeat));
	rChord.wMeasure = (WORD)(dwAbsBeat / timesig.bBeatsPerMeasure);
	rChord.bBeat = (BYTE)(dwAbsBeat % timesig.bBeatsPerMeasure);
	rChord.bSubChordCount = 4;
	for(int i = 0; i < 4; i++)
	{
		rChord.SubChordList[i].dwChordPattern = bMode ? dwMinor7Chord : dwMajor7Chord;
		rChord.SubChordList[i].dwScalePattern = bMode ? dwMinorScale : dwMajorScale;
		rChord.SubChordList[i].dwInversionPoints = 0xffffff;	// inversions allowed everywhere
		rChord.SubChordList[i].dwLevels = 1<<i;
		rChord.SubChordList[i].bChordRoot = bChordRoot;
		rChord.SubChordList[i].bScaleRoot = bScaleRoot;
	}

	BYTE key = bChordRoot % 12;
	DWORD scale;
	if(cSharpsFlats >= 0)
	{
		scale = ((adwSharpScales[cSharpsFlats] >> key) |
						  (0x00FFFFFF & (adwSharpScales[cSharpsFlats] << (12 - key))));
//		rChord.bKey = 0;
	}
	else
	{
		scale = ((adwFlatScales[-cSharpsFlats] >> key) |
						  (0x00FFFFFF & (adwFlatScales[-cSharpsFlats] << (12 - key))));
//		rChord.bKey = 1;
	}

	if(scalepattern)
	{
		*scalepattern = scale;
	}
	if(chordpattern)
		*chordpattern = bMode ? dwMinor7Chord : dwMajor7Chord;
	if(chordroot)
		*chordroot = bChordRoot;
	if(scaleroot)
	{
		if(bShiftScale)
			*scaleroot = 0;
		else
			*scaleroot = bScaleRoot;
	}
}
