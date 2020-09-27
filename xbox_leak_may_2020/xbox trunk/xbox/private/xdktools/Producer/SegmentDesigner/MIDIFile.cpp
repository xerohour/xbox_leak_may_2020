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
#include "Templates.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include <dmusicf.h>
#include <dmusicc.h>
#include "alist.h"

#include "SegmentDesignerDLL.h"
#include "Segment.h"
#include "SegmentComponent.h"
#include "segmentguids.h"
#include "SegmentRiff.h"
#include <initguid.h>

void CreateChordFromKey(char cSharpsFlats, BYTE bMode, DWORD dwTime, DMUS_CHORD_PARAM& rChord);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// XG indicator
bool gbIsXG = false;
bool IsXG()
{
	return gbIsXG;
}


/*  MIDI status bytes ==================================================*/

#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0
#define MIDI_SYSX       0xF0
#define MIDI_MTC        0xF1
#define MIDI_SONGPP     0xF2
#define MIDI_SONGS      0xF3
#define MIDI_EOX        0xF7
#define MIDI_CLOCK      0xF8
#define MIDI_START      0xFA
#define MIDI_CONTINUE   0xFB
#define MIDI_STOP       0xFC
#define MIDI_SENSE      0xFE
#define ET_NOTEOFF  ( MIDI_NOTEOFF >> 4 )  // 0x08
#define ET_NOTEON       ( MIDI_NOTEON >> 4 )   // 0x09
#define ET_PTOUCH       ( MIDI_PTOUCH >> 4 )   // 0x0A
#define ET_CCHANGE      ( MIDI_CCHANGE >> 4 )  // 0x0B
#define ET_PCHANGE      ( MIDI_PCHANGE >> 4 )  // 0x0C
#define ET_MTOUCH       ( MIDI_MTOUCH >> 4 )   // 0x0D
#define ET_PBEND        ( MIDI_PBEND >> 4 )    // 0x0E
#define ET_SYSX         ( MIDI_SYSX >> 4 )     // 0x0F
#define ET_PBCURVE          0x03
#define ET_CCCURVE      0x04
#define ET_MATCURVE     0x05
#define ET_PATCURVE     0x06
#define ET_TEMPOEVENT   0x01
#define ET_NOTDEFINED   0

#define NUM_MIDI_CHANNELS	16

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


CMIDITrack::CMIDITrack(	FullSeqEvent* lstTrackEvent, FullSeqEvent* lstOther, FullSeqEvent* lstVol, FullSeqEvent* lstPan, const CString &strTitle )
{
	m_lstTrackEvent = lstTrackEvent;
	m_lstOther = lstOther;
	m_lstVol = lstVol;
	m_lstPan = lstPan;
	m_strTitle = strTitle;
	m_pNext = NULL;
}

CMIDITrack::~CMIDITrack()
{
	List_Free( m_lstTrackEvent );
	List_Free( m_lstOther );
	List_Free( m_lstVol );
	List_Free( m_lstPan );
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


DWORD gPatchTable[NUM_MIDI_CHANNELS];
long gPos;	// keeps track of order of events in the file
DWORD gdwLastControllerTime[NUM_MIDI_CHANNELS];	// Holds the time of the last CC event
DWORD gdwControlCollisionOffset[NUM_MIDI_CHANNELS];

static IDMUSProdStripMgr* g_pChordStripMgr = NULL;
static DMUS_CHORD_PARAM g_Chord; // Holds the latest chord
static DMUS_CHORD_PARAM g_DefaultChord; // in case no chords are extracted from the track
static DMUS_IO_TIMESIGNATURE_ITEM gTimeSig; // holds the latest time sig

static long glLastSysexTime = -5;

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

// this function gets a short that is formatted the correct way
// i.e. the motorola way as opposed to the intel way
BOOL __inline GetMShort( LPSTREAM pStream, short& n )
{
    union uShort
	{
	unsigned char buf[2];
	short n;
	} u;
    unsigned char ch;

    if( S_OK != pStream->Read( u.buf, 2, NULL ) )
    {
	return FALSE;
    }

#ifndef _MAC
    // swap bytes
    ch = u.buf[0];
    u.buf[0] = u.buf[1];
    u.buf[1] = ch;
#endif

    n = u.n;
    return TRUE;
}

static short snPPQN;
static CString gstrTitle;
static IStream* gpTempoStream = NULL;
static IStream* gpSysExStream = NULL;
static DWORD gdwSizeTimeSigStream = 0;
static IStream* gpTimeSigStream = NULL;
static CSegment* gpSegment = NULL;
static BOOL	gfSetTempo = FALSE;
long	glTimeSig = 1; // flag to see if we should be paying attention to time sigs.
	// this is needed because we only care about the time sigs on the first track to
	// contain them that we read

static WORD GetVarLength( LPSTREAM pStream, DWORD& rfdwValue )
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
static DWORD ConvertTime( DWORD dwTime )
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
static DWORD ConvertTime( DWORD dwTime )
{
    if( snPPQN == DMUS_PPQ )
    {
		return dwTime;
    }
	DWORD dw = MulDiv( dwTime, DMUS_PPQ, snPPQN );
	if( dw == -1 )
	{
		return dwTime;
	}
    return dw;
}
#endif

static FullSeqEvent* CompressEventList( FullSeqEvent* lstEvent )
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
		pEvent->mtTime =  pEvent->mtTime ;
		if( ( pEvent->bStatus & 0xf0 ) == MIDI_NOTEON )
		{
			pEvent->mtDuration =  pEvent->mtDuration ;
			if( pEvent->mtDuration == 0 ) pEvent->mtDuration = 1;
		}
    }

    return lstEvent;
}

static int CompareEvents( FullSeqEvent* pEvent1, FullSeqEvent* pEvent2 )
{
	BYTE bEventType1 = static_cast<BYTE>( pEvent1->bStatus >> 4 );
	BYTE bEventType2 = static_cast<BYTE>( pEvent2->bStatus >> 4 );
    if( pEvent1->mtTime < pEvent2->mtTime )
    {
	return -1;
    }
    else if( pEvent1->mtTime > pEvent2->mtTime )
    {
	return 1;
    }
    else if( bEventType1 != ET_SYSX && bEventType2 != ET_SYSX )
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

static FullSeqEvent* MergeEvents( FullSeqEvent* lstLeftEvent, FullSeqEvent* lstRightEvent )
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

static FullSeqEvent* SortEventList( FullSeqEvent* lstEvent )
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
		return MergeEvents( SortEventList( lstEvent ),
					SortEventList( pRightEvent ) );
    }
    return lstEvent;
}

static FullSeqEvent* ReverseEventList( FullSeqEvent* lstEvent )
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

static FullSeqEvent* RemoveDuplicateCCs( FullSeqEvent* lstOther )
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
	FullSeqEvent *pCurEvent = lstOther;
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
	return lstOther;
}

static int ComparePatches( DMUS_IO_PATCH_ITEM* pPatch1, DMUS_IO_PATCH_ITEM* pPatch2 )
{
    if( pPatch1->lTime < pPatch2->lTime )
    {
		return -1;
    }
    else if( pPatch1->lTime > pPatch2->lTime )
    {
		return 1;
    }
    else if( pPatch1->byStatus < pPatch2->byStatus)
    {
		return -1;
	}
	else if( pPatch1->byStatus == pPatch2->byStatus)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static DMUS_IO_PATCH_ITEM* MergePatches( DMUS_IO_PATCH_ITEM* lstLeftPatch, DMUS_IO_PATCH_ITEM* lstRightPatch )
{
    DMUS_IO_PATCH_ITEM  anchorPatch;
    DMUS_IO_PATCH_ITEM* pPatch;

    anchorPatch.pNext = NULL;
    pPatch = &anchorPatch;

    do
    {
	if( ComparePatches( lstLeftPatch, lstRightPatch ) < 0 )
	{
	    pPatch->pNext = lstLeftPatch;
	    pPatch = lstLeftPatch;
	    lstLeftPatch = lstLeftPatch->pNext;
	    if( lstLeftPatch == NULL )
	    {
		pPatch->pNext = lstRightPatch;
	    }
	}
	else
	{
	    pPatch->pNext = lstRightPatch;
	    pPatch = lstRightPatch;
	    lstRightPatch = lstRightPatch->pNext;
	    if( lstRightPatch == NULL )
	    {
		pPatch->pNext = lstLeftPatch;
		lstLeftPatch = NULL;
	    }
	}
    } while( lstLeftPatch != NULL );

    return anchorPatch.pNext;
}

static DMUS_IO_PATCH_ITEM* SortPatchList( DMUS_IO_PATCH_ITEM* lstPatch )
{
    DMUS_IO_PATCH_ITEM* pMidPatch;
    DMUS_IO_PATCH_ITEM* pRightPatch;

    if( lstPatch != NULL && lstPatch->pNext != NULL )
    {
	pMidPatch = lstPatch;
	pRightPatch = pMidPatch->pNext->pNext;
	if( pRightPatch != NULL )
	{
	    pRightPatch = pRightPatch->pNext;
	}
	while( pRightPatch != NULL )
	{
	    pMidPatch = pMidPatch->pNext;
	    pRightPatch = pRightPatch->pNext;
	    if( pRightPatch != NULL )
	    {
		pRightPatch = pRightPatch->pNext;
	    }
	}
	pRightPatch = pMidPatch->pNext;
	pMidPatch->pNext = NULL;
	return MergePatches( SortPatchList( lstPatch ),
			    SortPatchList( pRightPatch ) );
    }
    return lstPatch;
}

static DWORD ReadEvent( LPSTREAM pStream, DWORD dwTime, FullSeqEvent** plstEvent, DMUS_IO_PATCH_ITEM** pplstPatchEvent,
										FullSeqEvent*& lstVol, FullSeqEvent*& lstPan,
										FullSeqEvent*& lstOther)
{
	UNREFERENCED_PARAMETER(lstPan);
	UNREFERENCED_PARAMETER(lstVol);

    static BYTE bRunningStatus;

	dwTime = ConvertTime(dwTime);

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
//			pEvent->bType = EVENT_VOICE;
//			pEvent->bStatus = bRunningStatus;
			pEvent->bStatus = static_cast<BYTE>(bRunningStatus & 0xf0);
			pEvent->bByte1 = b;
			pEvent->dwPChannel = bRunningStatus & 0xf;
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
				ASSERT( 0 == (pEvent->bStatus & 0xf) );
				pEvent->bStatus = MIDI_NOTEOFF;
			}

//			pEvent->bEventType = static_cast<BYTE>( pEvent->bStatus >> 4 );

			if((pEvent->bStatus & 0xf0) == MIDI_CCHANGE)
			{
				ASSERT( 0 == (pEvent->bStatus & 0xf) );
				DWORD dwChannel = pEvent->dwPChannel;
				if(dwTime == gdwLastControllerTime[dwChannel])
				{
					pEvent->mtTime += ++gdwControlCollisionOffset[dwChannel];
				}
				else
				{
					gdwControlCollisionOffset[dwChannel] = 0;
					gdwLastControllerTime[dwChannel] = dwTime;
				}
			}


			if(((pEvent->bStatus & 0xf0) == MIDI_CCHANGE) && (pEvent->bByte1 == 0 || pEvent->bByte1 == 0x20))
			{
				// We have a bank select or its LSB either of which are not added to event list
				if(pEvent->bByte1 == 0x20)
				{
					gBankSelect[pEvent->dwPChannel].byLSB = pEvent->bByte2;
				}
				else // pEvent->bByte1 == 0
				{
					gBankSelect[pEvent->dwPChannel].byMSB = pEvent->bByte2;
				}
				// We no longer need the event so we can free it
				delete pEvent;
			}
			else if( (pEvent->bStatus & 0xF0) == MIDI_CCHANGE && pEvent->bByte1 == 7 )
			{
				// volume
				pEvent->pNext = lstVol;
				pEvent->mtTime = dwTime;
				lstVol = pEvent;
			}
			else if( (pEvent->bStatus & 0xF0) == MIDI_CCHANGE && pEvent->bByte1 == 10)
			{
				// pan
				pEvent->pNext = lstPan;
				pEvent->mtTime = dwTime;
				lstPan = pEvent;
			}
			else if( (pEvent->bStatus & 0xF0) == MIDI_CCHANGE)
			{
				// other control events
				pEvent->pNext = lstOther;
				pEvent->mtTime = dwTime;
				lstOther = pEvent;
			}
			else if( (pEvent->bStatus & 0xF0) == MIDI_PBEND
				 ||  (pEvent->bStatus & 0xF0) == MIDI_PTOUCH )
			{
				// other control events
				pEvent->pNext = lstOther;
				pEvent->mtTime = dwTime;
				lstOther = pEvent;
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

			pPatchEvent->lTime = dwTime-1;
			pPatchEvent->byStatus = bRunningStatus;
			pPatchEvent->byPChange = b;
			pPatchEvent->byMSB = gBankSelect[bRunningStatus & 0xF].byMSB;
			pPatchEvent->byLSB = gBankSelect[bRunningStatus & 0xF].byLSB;
			pPatchEvent->dwFlags &= 0;
			pPatchEvent->dwFlags |= DMUS_IO_INST_PATCH;

			if(pPatchEvent->byMSB != 0xFF)
			{
				pPatchEvent->dwFlags |= DMUS_IO_INST_BANKSELECT;
			}
			
			if(pPatchEvent->byLSB != 0xFF)
			{
				pPatchEvent->dwFlags |= DMUS_IO_INST_BANKSELECT;
			}

			if(!(pPatchEvent->dwFlags & DMUS_IO_INST_BANKSELECT) )
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
				else
				{
					pPatchEvent->dwFlags |= DMUS_IO_INST_GM;
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
//			pEvent->bType = EVENT_VOICE;
			pEvent->mtDuration = 0;
			pEvent->nOffset = 0;
//			pEvent->bStatus = bRunningStatus;
			pEvent->bStatus = static_cast<BYTE>(bRunningStatus & 0xf0);
			pEvent->dwPChannel = bRunningStatus & 0xf;
			pEvent->bByte1 = b;
//			( (NoteEvent*)pEvent )->voiceid = (char)( ( ( bRunningStatus - 4 ) & 0xf ) + 1 );
//			pEvent->bEventType = static_cast<BYTE>( pEvent->bStatus >> 4 );

			pEvent->pNext = lstOther;
			lstOther = pEvent;
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
			// Get the length of the SysEx chunk
			dwBytes += GetVarLength( pStream, dwLen );

			// Create a new SysEx item
			pSysEx = new DMUS_IO_SYSEX_ITEM;
			if( pSysEx != NULL )
			{
				// Create an array to store the SysEx data in
				pbSysExData = new BYTE[dwLen + 1];
				if(pbSysExData != NULL)
				{
					//  The first byte is 0xF0 (which we've already read).
					pbSysExData[0] = 0xf0;
					if( FAILED( pStream->Read( pbSysExData + 1, dwLen, NULL ) ) )
					{
						delete []pbSysExData;
						delete pSysEx;
						return dwBytes;
					}
                    
					if( (dwLen > 0) && (pbSysExData[1] == 0x43) )
					{
						// check for XG files
						// Trailing F7 may be omitted in consequtive SysEx chunks
						if(dwLen >= 7)
						{
							BYTE abXG[] = { 0xF0, 0x43, 0x10, 0x4C, 0x00, 0x00, 0x7E, 0x00 };//, 0xF7 };
							if( memcmp( abXG, pbSysExData, 2 ) == 0 &&
								memcmp( &(abXG[3]), &(pbSysExData[3]), 5) == 0 &&
								(pbSysExData[2] & 0xF0) == abXG[2] )
							{
								// we have an XG!
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
					if(pSysEx->mtTime == 0)
					{
						pSysEx->mtTime = glLastSysexTime++;
						if(pSysEx->mtTime > 0)
						{
							pSysEx->mtTime = 0;
						}
					}
					pSysEx->dwSysExLength = dwLen + 1;

					if( NULL == gpSysExStream )
					{
						// create a stream to hold sysex events
						CreateStreamOnHGlobal( NULL, TRUE, &gpSysExStream );
					}

					if( gpSysExStream )
					{
						// bugbug error checking needs to be here
						gpSysExStream->Write( pSysEx, sizeof(DMUS_IO_SYSEX_ITEM), NULL );
						gpSysExStream->Write( pbSysExData, pSysEx->dwSysExLength, NULL );
					}

					// Clean up
					delete []pbSysExData;
					pbSysExData = NULL;
					delete pSysEx;
					pSysEx = NULL;
				}
				else
				{
					delete pSysEx;
					pSysEx = NULL;
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
			dwBytes += GetVarLength( pStream, dwLen );
			StreamSeek( pStream, dwLen, STREAM_SEEK_CUR );
			dwBytes += dwLen;
			break;
		case 0xff:
			if( FAILED( pStream->Read( &b, 1, NULL ) ) )
			{
				return dwBytes;
			}
			++dwBytes;
			dwBytes += GetVarLength( pStream, dwLen );
			if( b == 0x51 ) // tempo change
			{
				DWORD dw = 0;
				DMUS_IO_TEMPO_ITEM tempo;

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
				tempo.lTime = dwTime;
				tempo.dblTempo = 60000000.0 / (double)dw;
				if( gpSegment && !gfSetTempo )
				{
					gpSegment->SetTempo(tempo.dblTempo);
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
			}
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
				gTimeSig.lTime = dwTime;
				gTimeSig.bBeatsPerMeasure = b;
				++dwBytes;
				if( FAILED( pStream->Read( &b, 1, NULL ) ) )
				{
					return dwBytes;
				}
				gTimeSig.bBeat = (BYTE)( 1 << b ); // 0 means 256th note
				gTimeSig.wGridsPerBeat = 4; // this is irrelavent for MIDI files
				if( NULL == gpTimeSigStream )
				{
					CreateStreamOnHGlobal( NULL, TRUE, &gpTimeSigStream );
				}
				if( gpTimeSigStream )
				{
					gpTimeSigStream->Write( &gTimeSig, sizeof(DMUS_IO_TIMESIGNATURE_ITEM), NULL );
					gdwSizeTimeSigStream += sizeof(DMUS_IO_TIMESIGNATURE_ITEM);
				}
				++dwBytes;
				StreamSeek( pStream, dwLen - 2, STREAM_SEEK_CUR );
				dwBytes += ( dwLen - 2 );
			}
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

				// Create a chord (with one subchord) from the key info
				CreateChordFromKey(cSharpsFlats, bMode, dwTime, g_Chord);

				// If the chord track is empty, create it.
				if (!g_pChordStripMgr)
				{
					HRESULT hr = CoCreateInstance( 
							CLSID_ChordMgr, NULL, CLSCTX_INPROC,
							IID_IDMUSProdStripMgr,
							(void**)&g_pChordStripMgr );
					if (!SUCCEEDED(hr)) return dwBytes;

					// If dwTime > 0, use SetParam to insert the default chord at time 0
					if (dwTime > 0)
					{
						g_pChordStripMgr->SetParam(GUID_ChordParam, 0, &g_DefaultChord);
						BOOL fFlatsNotSharps = (cSharpsFlats < 0);
						g_pChordStripMgr->SetParam(GUID_ChordIndividualChordSharpsFlats, dwTime, &fFlatsNotSharps);
					}
				}

				// Use SetParam to insert the new chord into the chord track
				// use bKey member to determine whether to display as sharp or flat
				BOOL fFlatsNotSharps = (cSharpsFlats < 0);
				g_pChordStripMgr->SetParam(GUID_ChordParam, dwTime, &g_Chord);
				g_pChordStripMgr->SetParam(GUID_ChordIndividualChordSharpsFlats, dwTime, &fFlatsNotSharps);

				if(dwTime <= 0)
				{
					// Set key information in chord strip
					WORD nAccidentals = WORD(abs(cSharpsFlats));
					BOOL bFlatsNotSharps = (cSharpsFlats < 0);
					WORD nKey = g_Chord.SubChordList[0].bChordRoot;
					g_pChordStripMgr->SetParam(GUID_ChordSharpsFlats, 0, &bFlatsNotSharps);
					DWORD data = (nKey & 0x0000FFFF) | ((nAccidentals & 0x0000FFFF) << 16);
					g_pChordStripMgr->SetParam(GUID_ChordKey, 0, &data);
				}

			}
			else if( b == 0x03 )
			{
				// MIDI Sequence name/Track name/Subtitle
				char *pstrTitle = new char[dwLen + 1];
				if( (pstrTitle == NULL) || FAILED( pStream->Read( pstrTitle, dwLen, NULL ) ) )
				{
					if( pstrTitle != NULL )
					{
						delete []pstrTitle;
					}
					return dwBytes;
				}

				// Add the null terminator
				pstrTitle[dwLen] = 0;

				// If the existing title is not empty and does not end with a '\n', add one.
				if( !gpSegment->m_PPGSegment.strCopyright.IsEmpty() &&
					gpSegment->m_PPGSegment.strCopyright.Right( 1 ).Find( '\n' ) == -1)
				{
					gpSegment->m_PPGSegment.strCopyright += CString("\n");
				}

				// Add on to the existing title text
				gstrTitle += CString(pstrTitle);
				delete []pstrTitle;

				dwBytes += dwLen;
			}
			else if( b == 0x02 )
			{
				// Copyright
				char *pstrCopyright = new char[dwLen + 1];
				if( (pstrCopyright == NULL) || FAILED( pStream->Read( pstrCopyright, dwLen, NULL ) ) )
				{
					if( pstrCopyright != NULL )
					{
						delete []pstrCopyright;
					}
					return dwBytes;
				}

				// Add the null terminator
				pstrCopyright[dwLen] = 0;

				// If the existing copyright is not empty and does not end with a '\n', add one.
				if( !gpSegment->m_PPGSegment.strCopyright.IsEmpty() &&
					gpSegment->m_PPGSegment.strCopyright.Right( 1 ).Find( '\n' ) == -1)
				{
					gpSegment->m_PPGSegment.strCopyright += CString("\n");
				}

				// Add on to the existing copyright
				gpSegment->m_PPGSegment.strCopyright += CString(pstrCopyright);
				delete []pstrCopyright;

				dwBytes += dwLen;
			}
			else if( b == 0x01 )
			{
				// Author/Instructions/Text
				char *pstrInfo = new char[dwLen + 1];
				if( (pstrInfo == NULL) || FAILED( pStream->Read( pstrInfo, dwLen, NULL ) ) )
				{
					if( pstrInfo != NULL )
					{
						delete []pstrInfo;
					}
					return dwBytes;
				}

				// Add the null terminator
				pstrInfo[dwLen] = 0;

				// If the existing info text is not empty and does not end with a '\n', add one.
				if( gpSegment->m_PPGSegment.strInfo.Right( 1 ).Find( '\n' ) == -1)
				{
					gpSegment->m_PPGSegment.strInfo += CString("\n");
				}

				// Add on to the existing info text
				gpSegment->m_PPGSegment.strInfo += CString(pstrInfo);
				delete []pstrInfo;

				dwBytes += dwLen;
			}
			else if( b == 0x2f )
			{
				// Unknown - seems to be an 'end of track' marker
				StreamSeek( pStream, dwLen, STREAM_SEEK_CUR );
				dwBytes += dwLen;
			}
			else if( b == 0x21 )
			{
				// Unknown
				StreamSeek( pStream, dwLen, STREAM_SEEK_CUR );
				dwBytes += dwLen;
			}
			else
			{
				// Really unknown
				StreamSeek( pStream, dwLen, STREAM_SEEK_CUR );
				dwBytes += dwLen;
			}
			break;
		default:
	//            DisplayDebug( 1, "Unknown midi event type: 0x%x", b );
			break;
		}
    } // end else
    return dwBytes;
}


HRESULT WriteBandTrack(IDMUSProdRIFFStream* pRiffStream, DMUS_IO_PATCH_ITEM** lstPatchEvent,
											CMIDITrack *pMIDITracks )
// this function (in ListOfBands) deletes patch change and zeros out lstPatchEvent, assuming it's successful
{
	ASSERT(pRiffStream);
	*lstPatchEvent = SortPatchList(*lstPatchEvent);
	TrackHeaderChunk header(CLSID_DirectMusicBandTrack, NULL, DMUS_FOURCC_BANDTRACK_FORM);
	HRESULT hr = header.Write(pRiffStream);
	if(hr == S_OK)
	{
		WriteRiffChunk riff(pRiffStream, DMUS_FOURCC_BANDTRACK_FORM);
		hr = riff.State();
		if(hr == S_OK)
		{
			BandTrackHeader	trackheader;
			hr = trackheader.Write(pRiffStream);
			if(hr == S_OK)
			{
				ListOfBands	listObands(*lstPatchEvent, pMIDITracks);
				hr = listObands.Write(pRiffStream);
				*lstPatchEvent = 0;
			}
		}
	}
	return hr;
}

static void AddOffsets(FullSeqEvent* lstEvent, IDirectMusicTrack* pTimeSigTrack)
{
	HRESULT hr;
	MUSIC_TIME mtNext = 0;
	DMUS_IO_TIMESIGNATURE_ITEM timesig;
	timesig.bBeat = 4;
	timesig.bBeatsPerMeasure =  4;
	timesig.wGridsPerBeat = 4;
	timesig.lTime = 0;
	short nClocksPerGrid = static_cast<short>((DMUS_PPQ * (4 / timesig.bBeat)) / timesig.wGridsPerBeat);

	if (pTimeSigTrack)
	{
		hr = pTimeSigTrack->GetParam(GUID_TimeSignature, 0, &mtNext, (void*)&timesig);
		if (FAILED(hr))
		{
			mtNext = 0;
		}
		else
		{
			nClocksPerGrid = static_cast<short>((DMUS_PPQ * (4 / timesig.bBeat)) / timesig.wGridsPerBeat);
		}
	}

	for( FullSeqEvent* pEvent = lstEvent; pEvent; pEvent = pEvent->pNext )
	{
		if ( ( pEvent->bStatus & 0xf0 ) == MIDI_NOTEON )
		{
			if (mtNext && pTimeSigTrack && mtNext < pEvent->mtTime)
			{
				hr = pTimeSigTrack->GetParam(GUID_TimeSignature, mtNext, &mtNext, (void*)&timesig);
				if (FAILED(hr))
				{
					mtNext = 0;
				}
				else
				{
					nClocksPerGrid = static_cast<short>((DMUS_PPQ * (4 / timesig.bBeat)) / timesig.wGridsPerBeat);
				}
			}
			pEvent->nOffset = (short) ((pEvent->mtTime - timesig.lTime) % nClocksPerGrid);
			pEvent->mtTime -= pEvent->nOffset;
			if (pEvent->nOffset > (nClocksPerGrid / 2))
			{
				// make it a negative offset and bump the time a corresponding amount
				pEvent->nOffset = static_cast<short>(pEvent->nOffset - nClocksPerGrid);
				pEvent->mtTime += nClocksPerGrid;
			}
		}
	}

}

static void AddOffsetsToTracks(CMIDITrack *pMIDITracks, IDirectMusicTrack *pDMTrack)
{
	CMIDITrack *pMTrack = pMIDITracks;
	while( pMTrack )
	{
		// Only add offsets to note events (not 'other' events)
		AddOffsets( pMTrack->m_lstTrackEvent, pDMTrack );
		pMTrack = pMTrack->m_pNext;
	}
}


BOOL NeedDefaultResetValues( DMUS_IO_CURVE_ITEM* pCurveItem )
{
	BOOL fNeedDefaultResetValues = FALSE;

	switch( pCurveItem->bType )
	{
		case DMUS_CURVET_PBCURVE:
		case DMUS_CURVET_PATCURVE:
		case DMUS_CURVET_MATCURVE:
			fNeedDefaultResetValues = TRUE;
			break;
		
		case DMUS_CURVET_CCCURVE:
			switch( pCurveItem->bCCData )
			{
				case 1:		// Mod Wheel
				case 2:		// Breath Controller
				case 4:		// Foot Controller
				case 5:		// Portamento Time
				case 12:	// FX1
				case 13:	// FX2
				case 64:	// Sustain Pedal
				case 65:	// Portamento On/Off
				case 66:	// Sostenuto Pedal
				case 67:	// Soft Pedal
				case 68:	// Legato Footswitch
				case 69:	// Hold 2
					fNeedDefaultResetValues = TRUE;
					break;
			}
			break;
	}

	return fNeedDefaultResetValues;
}


void SetDefaultResetValues( DMUS_IO_CURVE_ITEM* pCurveItem, MUSIC_TIME mtClockLength  )
{
	pCurveItem->bFlags &= ~DMUS_CURVE_RESET;	
	pCurveItem->nResetValue = 0;	
	pCurveItem->mtResetDuration = 0;
	
	switch( pCurveItem->bType )
	{
		case DMUS_CURVET_PBCURVE:
			pCurveItem->bFlags |= DMUS_CURVE_RESET;	
			pCurveItem->nResetValue = 8192;	
			pCurveItem->mtResetDuration = mtClockLength;
			break;
		
		case DMUS_CURVET_CCCURVE:
			switch( pCurveItem->bCCData )
			{
				case 1:		// Mod Wheel
				case 2:		// Breath Controller
				case 4:		// Foot Controller
				case 5:		// Portamento Time
				case 12:	// FX1
				case 13:	// FX2
				case 64:	// Sustain Pedal
				case 65:	// Portamento On/Off
				case 66:	// Sostenuto Pedal
				case 67:	// Soft Pedal
				case 68:	// Legato Footswitch
				case 69:	// Hold 2
					pCurveItem->bFlags |= DMUS_CURVE_RESET;	
					pCurveItem->nResetValue = 0;	
					pCurveItem->mtResetDuration = mtClockLength;
					break;
			}
			break;

		case DMUS_CURVET_PATCURVE:
		case DMUS_CURVET_MATCURVE:
			pCurveItem->bFlags |= DMUS_CURVE_RESET;	
			pCurveItem->nResetValue = 0;	
			pCurveItem->mtResetDuration = mtClockLength;
			break;
	}
}


HRESULT CreateSegmentFromMIDIStream(CSegment* pSegment, LPSTREAM pStream)
{
	HRESULT hr = S_OK;

	glLastSysexTime = -5;
	gbIsXG = false;	// reset each time this is called

	if(pSegment == NULL || pStream == NULL)
	{
		return E_POINTER;
	}

    DWORD dwID;
    DWORD dwLength;
	DWORD dwSize;
    short nFormat;
    short nNumTracks;
    short nTracksRead;
	DMUS_IO_PATCH_ITEM* lstPatchEvent;
	IDMUSProdRIFFStream*	pRiffStream = 0;
	CMIDITrack*	pMIDITracks = 0;
	BOOL fFirstTrack = TRUE;


	gpSegment = pSegment;
	gfSetTempo = FALSE;
    lstPatchEvent = NULL;
	gstrTitle.Empty();

    nNumTracks = nTracksRead = 0;
    dwLength = 0;

	if (g_pChordStripMgr)
	{
		g_pChordStripMgr->Release();
		g_pChordStripMgr = NULL;
	}

	CreateChordFromKey(0, 0, 0, g_Chord);
	CreateChordFromKey(0, 0, 0, g_DefaultChord);

	FillMemory(&gBankSelect, (sizeof(DMUS_IO_BANKSELECT_ITEM) * NUM_MIDI_CHANNELS), 0xFF);
	FillMemory(&gdwLastControllerTime, (sizeof(DWORD) * NUM_MIDI_CHANNELS), 0xFF); 
	ZeroMemory(&gdwControlCollisionOffset, (sizeof(DWORD) * NUM_MIDI_CHANNELS)); 
	ZeroMemory(&gTimeSig, sizeof(DMUS_IO_TIMESIGNATURE_ITEM) );

    if( ( S_OK != pStream->Read( &dwID, sizeof( FOURCC ), NULL ) ) ||
		!GetMLong( pStream, dwSize ) )
    {
		return E_FAIL;
    }
// check for RIFF MIDI files
    if( dwID == mmioFOURCC( 'R', 'I', 'F', 'F' ) )
    {
		StreamSeek( pStream, 12, STREAM_SEEK_CUR );
		if( ( S_OK != pStream->Read( &dwID, sizeof( FOURCC ), NULL ) ) ||
			!GetMLong( pStream, dwSize ) )
		{
			return E_FAIL;
		}
    }
// check for normal MIDI files
	if( dwID != mmioFOURCC( 'M', 'T', 'h', 'd' ) )
	{
		return E_FAIL;
	}

    GetMShort( pStream, nFormat );
    GetMShort( pStream, nNumTracks );
    GetMShort( pStream, snPPQN );
    if( dwSize > 6 )
    {
		StreamSeek( pStream, dwSize - 6, STREAM_SEEK_CUR );
    }
    pStream->Read( &dwID, sizeof( FOURCC ), NULL );

	// Initialize glTimeSig to 1 so we read in TimeSig information
	glTimeSig = 1;
	gstrTitle.Empty();

    while( dwID == mmioFOURCC( 'M', 'T', 'r', 'k' ) )
    {
		FullSeqEvent* lstTrackEvent = NULL;
		FullSeqEvent* lstOther = NULL;
		FullSeqEvent* lstVol = NULL;
		FullSeqEvent* lstPan = NULL;
		DWORD dwCurTime = NULL;

		GetMLong( pStream, dwSize );
		while( dwSize > 0 )
		{
			DWORD localSize = GetVarLength( pStream, dwID );
			if( localSize > 0 )
			{
				dwSize -= localSize;
				dwCurTime += dwID;
				localSize = ReadEvent( pStream, dwCurTime, &lstTrackEvent, &lstPatchEvent, lstVol, lstPan, lstOther );
				if( localSize > 0 )
				{
					dwSize -= localSize;
				}
				else
				{
					hr = E_FAIL;
				}
			}
			else
			{
				hr = E_FAIL;
			}

			if( hr == E_FAIL )
			{
				// Need to delete local event lists
				List_Free(lstTrackEvent);
				List_Free(lstOther);
				List_Free(lstVol);
				List_Free(lstPan);
				goto EXIT;
			}
		}
		if( glTimeSig > 1 )
		{
			// if glTimeSig is greater than 1, it means we've read some time sigs
			// from this track (it was set to 2 inside ReadEvent.) This means that
			// we no longer want ReadEvent to pay any attention to time sigs, so
			// we set this to 0.
			glTimeSig = 0;
		}

		if( dwCurTime > dwLength )
		{
			dwLength = dwCurTime;
		}

		if( fFirstTrack && !gstrTitle.IsEmpty() )
		{
			pSegment->m_PPGSegment.strSegmentName = gstrTitle;
			gstrTitle.Empty();
			fFirstTrack = FALSE;
		}

		// If there is an existing title, or there were any note or curve events in this track, create a new CMIDITrack to store them in
		if( !gstrTitle.IsEmpty() || lstTrackEvent || lstOther || lstVol || lstPan )
		{
			// This should be in reverse order, so we only need to reverse it (not sort it)
			lstTrackEvent = ReverseEventList( lstTrackEvent );
			lstTrackEvent = CompressEventList( lstTrackEvent );

			// Remove duplicated controller messages - lstOther, lstVol, and lstPan must be in reverse order for this to work
			lstOther = RemoveDuplicateCCs( lstOther );
			lstVol = RemoveDuplicateCCs( lstVol );
			lstPan = RemoveDuplicateCCs( lstPan );

			// This should be in reverse order, so we only need to reverse it (not sort it)
			lstOther = ReverseEventList( lstOther );
			lstVol = ReverseEventList( lstVol );
			lstPan = ReverseEventList( lstPan );

			if( pMIDITracks == NULL )
			{
				pMIDITracks = new CMIDITrack( lstTrackEvent, lstOther, lstVol, lstPan, gstrTitle );
			}
			else
			{
				CMIDITrack *pMTrack = pMIDITracks;
				// Yes, this is slow - but it only happens once for each track
				while( pMTrack->m_pNext )
				{
					pMTrack = pMTrack->m_pNext;
				}
				pMTrack->m_pNext = new CMIDITrack( lstTrackEvent, lstOther, lstVol, lstPan, gstrTitle );
			}
		}

		// Reset title
		gstrTitle.Empty();

		if( FAILED( pStream->Read( &dwID, sizeof( FOURCC ), NULL ) ) )
		{
			break;
		}
    }

	dwLength = ConvertTime(dwLength);

	// Iterate through the pMIDITracks and create new ones for any events that are different
	CMIDITrack *pMTrack;
	pMTrack = pMIDITracks;
	while( pMTrack )
	{
		BOOL afExtraTracks[16];
		CMIDITrack *apMIDITracks[16];
		for( int i=0; i<16; i++ )
		{
			afExtraTracks[i] = FALSE;
			apMIDITracks[i] = NULL;
		}

		FullSeqEvent* pLastEvent = NULL;
		DWORD dwPChannel = 0xFFFFFFFF;

		// Iterate through all note events in this track
		FullSeqEvent* pEvent = pMTrack->m_lstTrackEvent;
		while( pEvent )
		{
			if( dwPChannel == -1 )
			{
				dwPChannel = pEvent->dwPChannel;
			}
			else if( dwPChannel != pEvent->dwPChannel )
			{
				if( !afExtraTracks[pEvent->dwPChannel] )
				{
					apMIDITracks[pEvent->dwPChannel] = new CMIDITrack( NULL, NULL, NULL, NULL, _T("") );
					afExtraTracks[pEvent->dwPChannel] = TRUE;
				}

				// Skip around the event we want to move
				pLastEvent->pNext = pEvent->pNext;
				// Set the pNext pointer of the event to move
				pEvent->pNext = apMIDITracks[pEvent->dwPChannel]->m_lstTrackEvent;
				// Update the list of note events in this CMIDITrack
				apMIDITracks[pEvent->dwPChannel]->m_lstTrackEvent = pEvent;
				// Set PEvent back to the last event
				pEvent = pLastEvent;
			}
			pLastEvent = pEvent;
			pEvent = pEvent->pNext;
		}

		// Iterate through all controller events in this track
		pLastEvent = NULL;
		pEvent = pMTrack->m_lstOther;
		while( pEvent )
		{
			if( dwPChannel == -1 )
			{
				dwPChannel = pEvent->dwPChannel;
				pLastEvent = pEvent;
				pEvent = pEvent->pNext;
			}
			else if( dwPChannel != pEvent->dwPChannel )
			{
				if( !afExtraTracks[pEvent->dwPChannel] )
				{
					apMIDITracks[pEvent->dwPChannel] = new CMIDITrack( NULL, NULL, NULL, NULL, _T("") );
					afExtraTracks[pEvent->dwPChannel] = TRUE;
				}

				// Skip around the event we want to move
				if( pLastEvent )
				{
					pLastEvent->pNext = pEvent->pNext;
				}
				else
				{
					pMTrack->m_lstOther = pMTrack->m_lstOther->pNext;
				}

				// Set the pNext pointer of the event to move
				pEvent->pNext = apMIDITracks[pEvent->dwPChannel]->m_lstOther;
				// Update the list of curve events in this CMIDITrack
				apMIDITracks[pEvent->dwPChannel]->m_lstOther = pEvent;

				if( pLastEvent )
				{
					// Set PEvent to the next event
					pEvent = pLastEvent->pNext;
				}
				else
				{
					// Set PEvent to the first event in the list
					pEvent = pMTrack->m_lstOther;
				}
			}
			else
			{
				pLastEvent = pEvent;
				pEvent = pEvent->pNext;
			}
		}

		// Iterate through all volume events in this track
		pLastEvent = NULL;
		pEvent = pMTrack->m_lstVol;
		while( pEvent )
		{
			if( dwPChannel == -1 )
			{
				dwPChannel = pEvent->dwPChannel;
				pLastEvent = pEvent;
				pEvent = pEvent->pNext;
			}
			else if( dwPChannel != pEvent->dwPChannel )
			{
				if( !afExtraTracks[pEvent->dwPChannel] )
				{
					apMIDITracks[pEvent->dwPChannel] = new CMIDITrack( NULL, NULL, NULL, NULL, _T("") );
					afExtraTracks[pEvent->dwPChannel] = TRUE;
				}

				// Skip around the event we want to move
				if( pLastEvent )
				{
					pLastEvent->pNext = pEvent->pNext;
				}
				else
				{
					pMTrack->m_lstVol = pMTrack->m_lstVol->pNext;
				}

				// Set the pNext pointer of the event to move
				pEvent->pNext = apMIDITracks[pEvent->dwPChannel]->m_lstVol;
				// Update the list of curve events in this CMIDITrack
				apMIDITracks[pEvent->dwPChannel]->m_lstVol = pEvent;

				if( pLastEvent )
				{
					// Set PEvent to the next event
					pEvent = pLastEvent->pNext;
				}
				else
				{
					// Set PEvent to the first event in the list
					pEvent = pMTrack->m_lstVol;
				}
			}
			else
			{
				pLastEvent = pEvent;
				pEvent = pEvent->pNext;
			}
		}

		// Iterate through all pan events in this track
		pLastEvent = NULL;
		pEvent = pMTrack->m_lstPan;
		while( pEvent )
		{
			if( dwPChannel == -1 )
			{
				dwPChannel = pEvent->dwPChannel;
				pLastEvent = pEvent;
				pEvent = pEvent->pNext;
			}
			else if( dwPChannel != pEvent->dwPChannel )
			{
				if( !afExtraTracks[pEvent->dwPChannel] )
				{
					apMIDITracks[pEvent->dwPChannel] = new CMIDITrack( NULL, NULL, NULL, NULL, _T("") );
					afExtraTracks[pEvent->dwPChannel] = TRUE;
				}

				// Skip around the event we want to move
				if( pLastEvent )
				{
					pLastEvent->pNext = pEvent->pNext;
				}
				else
				{
					pMTrack->m_lstPan = pMTrack->m_lstPan->pNext;
				}

				// Set the pNext pointer of the event to move
				pEvent->pNext = apMIDITracks[pEvent->dwPChannel]->m_lstPan;
				// Update the list of curve events in this CMIDITrack
				apMIDITracks[pEvent->dwPChannel]->m_lstPan = pEvent;

				if( pLastEvent )
				{
					// Set PEvent to the next event
					pEvent = pLastEvent->pNext;
				}
				else
				{
					// Set PEvent to the first event in the list
					pEvent = pMTrack->m_lstPan;
				}
			}
			else
			{
				pLastEvent = pEvent;
				pEvent = pEvent->pNext;
			}
		}

		// Add new tracks if we created any
		for( i=0; i<16; i++ )
		{
			if( afExtraTracks[i] )
			{
				CMIDITrack* pTmpTrack = pMTrack;
				while( pTmpTrack->m_pNext )
				{
					pTmpTrack = pTmpTrack->m_pNext;
				}
				pTmpTrack->m_pNext = apMIDITracks[i];
				apMIDITracks[i]->m_lstTrackEvent = SortEventList( apMIDITracks[i]->m_lstTrackEvent );
				apMIDITracks[i]->m_lstOther = SortEventList( apMIDITracks[i]->m_lstOther );
				apMIDITracks[i]->m_lstVol = SortEventList( apMIDITracks[i]->m_lstVol );
				apMIDITracks[i]->m_lstPan = SortEventList( apMIDITracks[i]->m_lstPan );
			}
		}
		pMTrack = pMTrack->m_pNext;
	}

		if( gpTimeSigStream )
		{
			IStream* pStreamCopy = 0;
			hr = pSegment->m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStreamCopy );
			if( SUCCEEDED( hr ) )
			{
				hr = AllocRIFFStream( pStreamCopy, &pRiffStream );
				TrackHeaderChunk trackchunk(CLSID_DirectMusicTimeSigTrack, DMUS_FOURCC_TIMESIGNATURE_TRACK, NULL);
				trackchunk.Write(pRiffStream);
				{
					WriteChunk chunk(pRiffStream, DMUS_FOURCC_TIMESIGNATURE_TRACK);
					hr = chunk.State();
					if( SUCCEEDED(hr) )
					{
						StreamSeek( gpTimeSigStream, 0, STREAM_SEEK_SET );
						STATSTG stg;
						hr = gpTimeSigStream->Stat(&stg, STATFLAG_NONAME);
						if( SUCCEEDED( hr ) )
						{
							WriteWord(pRiffStream, static_cast<WORD>(sizeof(DMUS_IO_TIMESIGNATURE_ITEM)));
							WriteWord(pRiffStream, 0);
							ULARGE_INTEGER	cbRead, cbWrite;
							hr = gpTimeSigStream->CopyTo( pStreamCopy, stg.cbSize, &cbRead, &cbWrite);
							hr = SUCCEEDED(hr) && (cbRead == cbWrite && cbWrite == stg.cbSize)
								? hr : E_FAIL;
						}
						else
						{
							TRACE("CreateSegmentFromMIDIStream: Unable to stat TimeSig stream chunk %x", hr);
						}
					}
					else
					{
						TRACE("CreateSegmentFromMIDIStream: Unable to create TimeSig chunk %x", hr);
					}
				}
				if( SUCCEEDED( hr ) )
				{
					// add timing offsets
					IPersistStream* pIPSTrack;
					IDirectMusicTrack* pDMTrack;

					// Create a TimeSig Track to store the TimeSig events
					if( SUCCEEDED( CoCreateInstance( CLSID_DirectMusicTimeSigTrack,
						NULL, CLSCTX_INPROC, IID_IPersistStream,
						(void**)&pIPSTrack )))
					{
						// set the overall size to the correct size
						StreamSeek( gpTimeSigStream, sizeof(DWORD), STREAM_SEEK_SET );
						gpTimeSigStream->Write( &gdwSizeTimeSigStream, sizeof(DWORD), NULL );

						// reset to beginning and persist to track.
						StreamSeek( gpTimeSigStream, 0, STREAM_SEEK_SET );
						pIPSTrack->Load( gpTimeSigStream );

						if( SUCCEEDED( pIPSTrack->QueryInterface( IID_IDirectMusicTrack, 
							(void**)&pDMTrack ) ) )
						{
							AddOffsetsToTracks(pMIDITracks, pDMTrack);
							pDMTrack->Release();
						}
						pIPSTrack->Release();
					}

					// send track to segment
					StreamSeek(pStreamCopy, 0, STREAM_SEEK_SET);
					STATSTG stg;
					hr = pStreamCopy->Stat(&stg, STATFLAG_NONAME);
					if( SUCCEEDED( hr ) )
					{
						hr = pSegment->DM_AddTrack(pRiffStream, stg.cbSize.LowPart);
					}
					else
					{
						TRACE("CreateSegmentFromMIDIStream: Unable to stat TimeSig stream chunk (2) %x", hr);
					}
				}
				else
				{
					TRACE("CreateSegmentFromMIDIStream: Unable to Copy TimeSig chunk %x", hr);
				}
			}
			else
			{
				TRACE("CreateSegmentFromMIDIStream: Unable to alloc memory stream for TimeSig track %x.\n", hr );
			}
			pRiffStream->Release();
			pStreamCopy->Release();
		}
		else
		{
			AddOffsetsToTracks(pMIDITracks, NULL);
		}


		// add events to segment

		// bugbug could be checking to see if there are actually tempo events,
		// sysex events, etc. to see if it's really necessary to create these
		// tracks...
		// Create a Tempo Track chunk in which to store the tempo events
		if( gpTempoStream )
		{
			IStream* pStreamCopy = 0;
			hr = pSegment->m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStreamCopy );
			if(SUCCEEDED(hr))
			{
				hr = AllocRIFFStream( pStreamCopy, &pRiffStream );
				TrackHeaderChunk trackchunk(CLSID_DirectMusicTempoTrack, DMUS_FOURCC_TEMPO_TRACK, NULL);
				trackchunk.Write(pRiffStream);
				{
					WriteChunk chunk(pRiffStream, DMUS_FOURCC_TEMPO_TRACK);
					hr = chunk.State();
					if( SUCCEEDED( hr ) )
					{
						StreamSeek( gpTempoStream, 0, STREAM_SEEK_SET );
						STATSTG stg;
						hr = gpTempoStream->Stat(&stg, STATFLAG_NONAME);
						if(hr == S_OK)
						{
							WriteWord(pRiffStream, static_cast<WORD>(sizeof(DMUS_IO_TEMPO_ITEM)));
							WriteWord(pRiffStream,0);
							ULARGE_INTEGER	cbRead, cbWrite;
							hr = gpTempoStream->CopyTo(  pStreamCopy, stg.cbSize, &cbRead, &cbWrite);
							hr = (hr == S_OK) && (cbRead == cbWrite && cbWrite == stg.cbSize)
								? hr : E_FAIL;
						}
					}
					else
					{
						TRACE("CreateSegmentFromMIDIStream: Unable to set Tempo track state %x.\n", hr );
					}
				}	// write chunk needs to be destroyed to properly ascend
				
				if( SUCCEEDED( hr ) )
				{
					// send track to segment
					StreamSeek(pStreamCopy, 0, STREAM_SEEK_SET);
					STATSTG stg;
					hr = pStreamCopy->Stat(&stg, STATFLAG_NONAME);
					if( SUCCEEDED( hr ) )
					{
						hr = pSegment->DM_AddTrack(pRiffStream, stg.cbSize.LowPart);
					}
					else
					{
						TRACE("CreateSegmentFromMIDIStream: Unable to stat Tempo track stream %x.\n", hr );
					}
				}
				else
				{
					TRACE("CreateSegmentFromMIDIStream: Unable to copy Tempo track stream %x.\n", hr );
				}
			}
			else
			{
				TRACE("CreateSegmentFromMIDIStream: Unable to alloc memory stream for Tempo track %x.\n", hr );
			}
			pRiffStream->Release();
			pStreamCopy->Release();
		}

		if(lstPatchEvent )
		{
			IStream* pStream = 0;
			hr = pSegment->m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStream );
			if(SUCCEEDED(hr))
			{
				hr = AllocRIFFStream( pStream, &pRiffStream );
				if(SUCCEEDED(hr))
				{
					hr = WriteBandTrack(pRiffStream, &lstPatchEvent, pMIDITracks);
					if(SUCCEEDED(hr))
					{
						// send track to segment
						StreamSeek(pStream, 0, STREAM_SEEK_SET);
						STATSTG stg;
						hr = pStream->Stat(&stg, STATFLAG_NONAME);
						if( SUCCEEDED( hr ) )
						{
							hr = pSegment->DM_AddTrack(pRiffStream, stg.cbSize.LowPart);
						}
						else
						{
							TRACE("CreateSegmentFromMIDIStream: Unable to stat Band track %x.\n", hr );
						}
					}
					else
					{
						TRACE("CreateSegmentFromMIDIStream: Unable to write Band track %x.\n", hr );
					}
				}
				else
				{
					TRACE("CreateSegmentFromMIDIStream: Unable to alloc RIFF stream for Band track %x.\n", hr );
				}
				pRiffStream->Release();
				pStream->Release();
			}
			else
			{
				TRACE("CreateSegmentFromMIDIStream: Unable to alloc memory stream for band track %x.\n", hr );
			}
		}

		// Create a Sequence Track in which to store the notes and curves
		//
		pMTrack = pMIDITracks;
		while( pMTrack )
		{
			IStream* pStreamCopy = 0;
			hr = pSegment->m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStreamCopy );
			if( SUCCEEDED( hr ) )
			{
				hr = AllocRIFFStream( pStreamCopy, &pRiffStream );
				TrackHeaderChunk trackchunk(CLSID_DirectMusicSeqTrack, DMUS_FOURCC_SEQ_TRACK, NULL);
				trackchunk.Write(pRiffStream);

				// Write UNFO chunk containing pMTrack->m_strName
				if( !pMTrack->m_strTitle.IsEmpty() )
				{
					WriteListChunk unfoList(pRiffStream, DMUS_FOURCC_UNFO_LIST);
					hr = unfoList.State();
					if( SUCCEEDED( hr ) )
					{
						WriteChunk unfoName(pRiffStream, DMUS_FOURCC_UNAM_CHUNK);
						hr = unfoName.State();
						if( SUCCEEDED( hr ) )
						{
							hr = SaveMBStoWCS( pStreamCopy, &pMTrack->m_strTitle );
						}
						if( FAILED(hr) )
						{
							TRACE("Failed to write Track's name: %s\n", pMTrack->m_strTitle);
						}
					}
					else
					{
						TRACE("Failed to write Track's Unfo chunk: %s\n", pMTrack->m_strTitle);
					}
				}

				{
					// write notes
					WriteChunk chunk(pRiffStream, DMUS_FOURCC_SEQ_TRACK);
					hr = chunk.State();
					if( SUCCEEDED( hr ) )
					{
						WriteChunk seq(pRiffStream, DMUS_FOURCC_SEQ_LIST);
						DWORD cb;
						FullSeqEvent* pEvent;
						DWORD cbWritten;
						cb = sizeof(DMUS_IO_SEQ_ITEM); // doesn't have the next pointers
						WriteDWORD(pRiffStream, cb);
						for( pEvent = pMTrack->m_lstTrackEvent; pEvent; pEvent = pEvent->pNext )
						{
							if( dwLength < (DWORD)(pEvent->mtTime + pEvent->mtDuration + pEvent->nOffset) )
							{
								dwLength = pEvent->mtTime + pEvent->mtDuration + pEvent->nOffset;
							}
							pStreamCopy->Write( pEvent, cb, &cbWritten );
							if( cb != cbWritten ) // error!
							{
								hr = E_FAIL;
								break;
							}
						}
					}
					else
					{
						TRACE("CreateSegmentFromMIDIStream: Invalid state for sequence track %x.\n", hr );
					}

					// write controllers
					// Don't care if sequence track saved correctly or not
					//if(hr == S_OK)
					{
						WriteChunk curve(pRiffStream, DMUS_FOURCC_CURVE_LIST);
						DWORD cb;
						FullSeqEvent* pEvent;
						DWORD cbWritten;
						cb = sizeof(DMUS_IO_CURVE_ITEM);
						WriteDWORD(pRiffStream, cb);

						// Merge the volume and pan information into lstOther
						pMTrack->m_lstOther = List_Cat( pMTrack->m_lstOther, pMTrack->m_lstVol );
						pMTrack->m_lstVol = NULL;
						pMTrack->m_lstOther = List_Cat( pMTrack->m_lstOther, pMTrack->m_lstPan );
						pMTrack->m_lstPan = NULL;

						// Sort the list
						pMTrack->m_lstOther = SortEventList(pMTrack->m_lstOther);

						for( pEvent = pMTrack->m_lstOther; pEvent; pEvent = pEvent->pNext )
						{
							DMUS_IO_CURVE_ITEM item;
							memset(&item, 0, sizeof(DMUS_IO_CURVE_ITEM));
							item.mtStart = pEvent->mtTime;
							item.nOffset = pEvent->nOffset;
							item.dwPChannel = pEvent->dwPChannel;
							item.bCurveShape = DMUS_CURVES_INSTANT;
							item.mtDuration = 1;
							switch( pEvent->bStatus & 0xF0 )
							{
								case MIDI_PBEND:
									item.bType = DMUS_CURVET_PBCURVE;
									item.bCCData = 0;
									item.nStartValue = (short)(((pEvent->bByte2 & 0x7F) << 7) + (pEvent->bByte1 & 0x7F));
									item.nEndValue = (short)(((pEvent->bByte2 & 0x7F) << 7) + (pEvent->bByte1 & 0x7F));
									break;
								case MIDI_PTOUCH:
									item.bType = DMUS_CURVET_PATCURVE;
									item.bCCData = pEvent->bByte1;
									item.nStartValue = pEvent->bByte2;
									item.nEndValue = pEvent->bByte2;
									break;
								case MIDI_MTOUCH:
									item.bType = DMUS_CURVET_MATCURVE;
									item.bCCData = 0;
									item.nStartValue = pEvent->bByte1;
									item.nEndValue = pEvent->bByte1;
									break;
								default:
									item.bType = DMUS_CURVET_CCCURVE;
									item.bCCData = pEvent->bByte1;
									item.nStartValue = pEvent->bByte2;
									item.nEndValue = pEvent->bByte2;
									break;
							}

							if( NeedDefaultResetValues(&item) )
							{
								DWORD dwResetLength = min( dwLength, DMUS_PPQ << 4 );
								FullSeqEvent* pListEvent = pEvent;
								while( pListEvent->pNext )
								{
									pListEvent = pListEvent->pNext;
									if( (pListEvent->bStatus & 0xF0) == (pEvent->bStatus & 0xF0) )
									{
										if( ((pListEvent->bStatus & 0xF0) == MIDI_CCHANGE)
										||  ((pListEvent->bStatus & 0xF0) == DMUS_CURVET_PATCURVE) )
										{
											if( pListEvent->bByte1 != pEvent->bByte1 )
											{
												continue;
											}
										}

										dwResetLength  = (pListEvent->mtTime + pListEvent->nOffset);
										dwResetLength -= (pEvent->mtTime + pEvent->nOffset);
										dwResetLength += DMUS_PPQ;
										ASSERT( dwResetLength >= DMUS_PPQ );
										break;
									}
								}
								SetDefaultResetValues( &item, dwResetLength + DMUS_PPQ );
							}

							pStreamCopy->Write( &item, cb, &cbWritten );
							if( cb != cbWritten ) // error!
							{
								hr = E_FAIL;
								break;
							}
						}
					}
				}	// write chunk needs to be destroyed to properly ascend
				if( SUCCEEDED( hr ) )
				{
					// send track to segment
					StreamSeek(pStreamCopy, 0, STREAM_SEEK_SET);
					STATSTG stg;
					hr = pStreamCopy->Stat(&stg, STATFLAG_NONAME);
					if( SUCCEEDED( hr ) )
					{
						hr = pSegment->DM_AddTrack(pRiffStream, stg.cbSize.LowPart);
					}
					else
					{
						TRACE("CreateSegmentFromMIDIStream: Unable to stat sequence track %x.\n", hr );
					}
				}
				else
				{
					TRACE("CreateSegmentFromMIDIStream: Error writing sequence track %x.\n", hr );
				}
			}
			else
			{
				TRACE("CreateSegmentFromMIDIStream: Unable to alloc memory stream for sequence track %x.\n", hr );
			}
			pRiffStream->Release();
			pStreamCopy->Release();

			pMTrack = pMTrack->m_pNext;
			delete pMIDITracks;
			pMIDITracks = pMTrack;
		}


		// set the length of the segment. Set it to the measure boundary
		// past the last note.
		DWORD dwResolvedLength;
		dwResolvedLength = gTimeSig.lTime;
		if( 0 == gTimeSig.bBeat ) gTimeSig.bBeat = 4;
		if( 0 == gTimeSig.bBeatsPerMeasure ) gTimeSig.bBeatsPerMeasure = 4;
		if( 0 == gTimeSig.wGridsPerBeat ) gTimeSig.wGridsPerBeat = 4;
		while( dwResolvedLength < dwLength )
		{
			dwResolvedLength += (((DMUS_PPQ * 4) / gTimeSig.bBeat) * gTimeSig.bBeatsPerMeasure);
		}
		if( pSegment->m_pIDMSegment )
		{
			pSegment->m_pIDMSegment->SetLength( dwResolvedLength );
		}
		pSegment->m_mtLength = dwResolvedLength;
		pSegment->m_rtLength = 0;
		pSegment->m_dwSegmentFlags = 0;

		if( gpSysExStream )
		{
			IStream* pStreamCopy = 0;
			hr = pSegment->m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStreamCopy );
			if(SUCCEEDED(hr))
			{
				hr = AllocRIFFStream( pStreamCopy, &pRiffStream );
				TrackHeaderChunk trackchunk(CLSID_DirectMusicSysExTrack, DMUS_FOURCC_SYSEX_TRACK, NULL);
				trackchunk.Write(pRiffStream);
				{
					WriteChunk chunk(pRiffStream, DMUS_FOURCC_SYSEX_TRACK);
					hr = chunk.State();
					if( SUCCEEDED( hr ) )
					{
						StreamSeek( gpSysExStream, 0, STREAM_SEEK_SET );
						STATSTG stg;
						hr = gpSysExStream->Stat(&stg, STATFLAG_NONAME);
						if( SUCCEEDED( hr ) )
						{
							ULARGE_INTEGER	cbRead, cbWrite;
							hr = gpSysExStream->CopyTo(  pStreamCopy, stg.cbSize, &cbRead, &cbWrite);
							hr = (hr == S_OK) && (cbRead == cbWrite && cbWrite == stg.cbSize)
								? hr : E_FAIL;
						}
						else
						{
							TRACE("CreateSegmentFromMIDIStream: Unable to stat sysex track %x.\n", hr );
						}
					}
					else
					{
						TRACE("CreateSegmentFromMIDIStream: Invalid state for sysex track %x.\n", hr );
					}
				}	// write chunk needs to be destroyed to properly ascend
				if( SUCCEEDED( hr ) )
				{
					// send track to segment
					StreamSeek(pStreamCopy, 0, STREAM_SEEK_SET);
					STATSTG stg;
					hr = pStreamCopy->Stat(&stg, STATFLAG_NONAME);
					if( SUCCEEDED( hr ) )
					{
						hr = pSegment->DM_AddTrack(pRiffStream, stg.cbSize.LowPart);
					}
					else
					{
						TRACE("CreateSegmentFromMIDIStream: Unable to stat sysex track (2) %x.\n", hr );
					}
				}
				else
				{
					TRACE("CreateSegmentFromMIDIStream: Unable to copy sysex track %x.\n", hr );
				}
			}
			else
			{
				TRACE("CreateSegmentFromMIDIStream: Unable to alloc memory stream for sysex track %x.\n", hr );
			}
			pRiffStream->Release();
			pStreamCopy->Release();
		}

		if (!g_pChordStripMgr)
		{
			hr = CoCreateInstance( 
					CLSID_ChordMgr, NULL, CLSCTX_INPROC,
					IID_IDMUSProdStripMgr,
					(void**)&g_pChordStripMgr );
			if (SUCCEEDED(hr))
			{
				g_pChordStripMgr->SetParam(GUID_ChordParam, 0, &g_DefaultChord);
			}
			else
			{
				TRACE("CreateSegmentFromMIDIStream: Unable to create CLSID_ChordMgr track %x.\n", hr );
			}
		}

		// Check g_pChordStripMgr, not hr
		if ( g_pChordStripMgr )
		{
			IStream* pStreamCopy = 0;
			hr = pSegment->m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStreamCopy );
			if( SUCCEEDED( hr ) )
			{
				hr = AllocRIFFStream( pStreamCopy, &pRiffStream );
				if( SUCCEEDED( hr ) )
				{
					TrackHeaderChunk trackchunk(CLSID_DirectMusicChordTrack, DMUS_FOURCC_CHORDTRACKHEADER_CHUNK, NULL);
					trackchunk.Write(pRiffStream);
					IPersistStream* pIPersistStream;
					if( SUCCEEDED( g_pChordStripMgr->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
					{
						hr = pIPersistStream->Save(pStreamCopy, TRUE);
						if( SUCCEEDED( hr ) )
						{
							// send track to segment
							StreamSeek(pStreamCopy, 0, STREAM_SEEK_SET);
							STATSTG stg;
							hr = pStreamCopy->Stat(&stg, STATFLAG_NONAME);
							if( SUCCEEDED( hr ) )
							{
								hr = pSegment->DM_AddTrack(pRiffStream, stg.cbSize.LowPart);
							}
							else
							{
								TRACE("CreateSegmentFromMIDIStream: Unable to stat chord track %x.\n", hr );
							}
						}
						else
						{
							TRACE("CreateSegmentFromMIDIStream: Unable to save chord track %x.\n", hr );
						}
						pIPersistStream->Release();
					}
					pRiffStream->Release();
				}
				else
				{
					TRACE("CreateSegmentFromMIDIStream: Unable to alloc RIFF stream for chord track %x.\n", hr );
				}
			}
			else
			{
				TRACE("CreateSegmentFromMIDIStream: Unable to alloc memory stream for chord track %x.\n", hr );
			}
			pStreamCopy->Release();
		}

EXIT:


	if(g_pChordStripMgr)
	{
		g_pChordStripMgr->Release();
		g_pChordStripMgr = NULL;
	}

	List_Free( lstPatchEvent );

	// release our hold on the streams
	RELEASE( gpTempoStream );
	RELEASE( gpSysExStream );
	RELEASE( gpTimeSigStream );

	gdwSizeTimeSigStream = 0;

	gpSegment = NULL;
	return hr;
	// Always succeed, since we don't keep a valid error code around
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

void CreateChordFromKey(char cSharpsFlats, BYTE bMode, DWORD dwTime, DMUS_CHORD_PARAM& rChord)
{
	TRACE("Creating a chord.  Key: %d Mode: %d Time: %d\n", cSharpsFlats, bMode, dwTime);
	static DWORD dwMajorScale = 0xab5ab5;	// 1010 1011 0101 1010 1011 0101
	static DWORD dwMinorScale = 0x5ad5ad;	// 0101 1010 1101 0101 1010 1101
	static DWORD dwMajor7Chord = 0x891;		// 1000 1001 0001
	static DWORD dwMinor7Chord = 0x489;		// 0100 1000 1001
	BYTE bScaleRoot = 0;
	BYTE bChordRoot = 0;
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
/*
	rChord.bKey = cSharpsFlats < 0;
	
	if(cSharpsFlats >= 0)
	{
		rChord.dwScale = ((adwSharpScales[cSharpsFlats] >> bChordRoot) |
						  (0x00FFFFFF & (adwSharpScales[cSharpsFlats] << (12 - bChordRoot))));
	}
	else
	{
		rChord.dwScale = ((adwFlatScales[-cSharpsFlats] >> bChordRoot) |
						  (0x00FFFFFF & (adwFlatScales[-cSharpsFlats] << (12 - bChordRoot))));
	}
//	rChord.dwScale = cSharpsFlats >= 0 ? adwSharpScales[cSharpsFlats] : adwFlatScales[-cSharpsFlats];
*/
}
#pragma warning(default:4244)

