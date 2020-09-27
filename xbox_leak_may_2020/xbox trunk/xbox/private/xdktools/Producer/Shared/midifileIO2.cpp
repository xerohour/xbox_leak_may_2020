
// midifileIO2.cpp
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
//#include "initguid.h"
//#include "dmperf.h"
#include <dmusici.h>
#include <dmusicf.h>
#include <dmusicc.h>
#include <dmuspriv.h>
//#include "..\dmusic\dmcollec.h"
#include "alist.h"
//#include "dmime_i.c"
//#include "dmime.h"
//#include "..\dmband\dmbndtrk.h"
//#include "..\dmband\bandinst.h"

//#include "Segment.h"
//#include "SegmentComponent.h"

#include "SeqSegmentRiff.h"
#include "curveio.h"
#include "sequenceio.h"
#include "midifileio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
#endif

short gnPPQN;
IStream* gpTempoStream = NULL;
IStream* gpSysExStream = NULL;
IStream* gpTimeSigStream = NULL;
long	glTimeSig = 1; // flag to see if we should be paying attention to time sigs.
	// this is needed because we only care about the time sigs on the first track to
	// contain them that we read

WORD GetVarLength( LPSTREAM pStream, DWORD& rfdwValue )
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

	if( gnPPQN == DMUS_PPQ )  {
		return dwTime;
	}
	WideMultiply( dwTime, DMUS_PPQ, &d );
	return WideDivide( &d, gnPPQN, &l );
}
#else
static DWORD ConvertTime( DWORD dwTime )
{
    //__int64 d;

    if( gnPPQN == DMUS_PPQ )
    {
		return dwTime;
    }
	/*
    d = dwTime;
    d *= DMUS_PPQ;
    d /= gnPPQN;
	*/
    return MulDiv( dwTime, DMUS_PPQ, gnPPQN );
}
#endif

FullSeqEvent* CompressEventList( FullSeqEvent* lstEvent )
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
		nChannel = pEvent->bStatus & 0xf;
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
		pEvent->mtTime = ConvertTime( pEvent->mtTime );
		if( ( pEvent->bStatus & 0xf0 ) == MIDI_NOTEON )
		{
			pEvent->mtDuration = ConvertTime( pEvent->mtDuration );
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
    else if( Status(pEvent1->bStatus) != MIDI_SYSX && Status(pEvent2->bStatus) != MIDI_SYSX )
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

FullSeqEvent* SortEventList( FullSeqEvent* lstEvent )
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

DWORD ReadEvent( LPSTREAM pStream, DWORD dwTime, FullSeqEvent** plstEvent, DMUS_IO_PATCH_ITEM** pplstPatchEvent)
{
    static BYTE bRunningStatus;

    DWORD dwBytes;
    DWORD dwLen;
    FullSeqEvent* pEvent;
	DMUS_IO_PATCH_ITEM* pPatchEvent;
    DMUS_IO_SYSEX_ITEM* pSysEx;

	BYTE* pbSysExData = 0;
    BYTE b;

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
			pEvent->bStatus = bRunningStatus;
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

			if( pplstPatchEvent )
			{
				pPatchEvent = new DMUS_IO_PATCH_ITEM;

				if(pPatchEvent == NULL)
				{
					return 0;
				}

				pPatchEvent->lTime = ConvertTime(dwTime);
				pPatchEvent->byStatus = bRunningStatus;
				pPatchEvent->byPChange = b;	// byte 0 in dwPatch
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

				if(!(pPatchEvent->dwFlags & DMUS_IO_INST_BANKSELECT ) )
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
			}
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
			pEvent->bStatus = bRunningStatus;
			pEvent->bByte1 = b;
//			( (NoteEvent*)pEvent )->voiceid = (char)( ( ( bRunningStatus - 4 ) & 0xf ) + 1 );

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
			dwBytes += GetVarLength( pStream, dwLen );
			pSysEx = new DMUS_IO_SYSEX_ITEM;
			if( pSysEx != NULL )
			{
//				pSysEx->pbSysExData = new BYTE[dwLen + 1];
				pbSysExData = new BYTE[dwLen + 1];
//				if( pSysEx->pbSysExData != NULL )
				if(pbSysExData != NULL)
				{
//					pSysEx->pbSysExData[0] = 0xf0;
					pbSysExData[0] = 0xf0;
					if( FAILED( pStream->Read( pbSysExData + 1, dwLen, NULL ) ) )
					{
						delete []pbSysExData;
						delete pSysEx;
						return dwBytes;
					}
					pSysEx->dwPChannel = 0;
					pSysEx->mtTime = dwTime;
					DWORD dwTempLen = dwLen + 1;
					pSysEx->dwSysExLength = dwTempLen;
					if( NULL == gpSysExStream )
					{
						// create a stream to hold sysex events
						CreateStreamOnHGlobal( NULL, TRUE, &gpSysExStream );
					}
					if( gpSysExStream )
					{
						// bugbug error checking needs to be here
						gpSysExStream->Write( &pSysEx->mtTime, sizeof(long), NULL );
						gpSysExStream->Write( &pSysEx->dwPChannel, sizeof(DWORD), NULL);
						gpSysExStream->Write( &pSysEx->dwSysExLength, sizeof(DWORD), NULL );
//						gpSysExStream->Write( pSysEx->pbSysExData, pSysEx->dwSysExLength, NULL );
						gpSysExStream->Write( pbSysExData, dwTempLen, NULL );
					}
//					delete pSysEx->pbSysExData;
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
				tempo.lTime = ConvertTime( dwTime );
				tempo.dblTempo = 60000000 / (double)dw;
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
				DMUS_IO_TIMESIGNATURE_ITEM timesig;
				if( FAILED( pStream->Read( &b, 1, NULL ) ) )
				{
					return dwBytes;
				}
				// set glTimeSig to 2 to signal to the main function that we've
				// read a time sig on this track
				glTimeSig = 2;
				timesig.lTime = ConvertTime( dwTime );
				timesig.bBeatsPerMeasure = b;
				++dwBytes;
				if( FAILED( pStream->Read( &b, 1, NULL ) ) )
				{
					return dwBytes;
				}
				timesig.bBeat = (BYTE)( 1 << b ); // 0 means 256th note
				timesig.wGridsPerBeat = 0; // this is irrelavent for MIDI files
				if( NULL == gpTimeSigStream )
				{
					CreateStreamOnHGlobal( NULL, TRUE, &gpTimeSigStream );
				}
				if( gpTimeSigStream )
				{
					gpTimeSigStream->Write( &timesig, sizeof(DMUS_IO_TIMESIGNATURE_ITEM), NULL );
				}
				++dwBytes;
				StreamSeek( pStream, dwLen - 2, STREAM_SEEK_CUR );
				dwBytes += ( dwLen - 2 );
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


BOOL NeedDefaultResetValues( CCurveItem* pItem )
{
	BOOL fNeedDefaultResetValues = FALSE;

	switch( pItem->m_bType )
	{
		case DMUS_CURVET_PBCURVE:
		case DMUS_CURVET_PATCURVE:
		case DMUS_CURVET_MATCURVE:
			fNeedDefaultResetValues = TRUE;
			break;
		
		case DMUS_CURVET_CCCURVE:
			switch( pItem->m_bCCData )
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


void SetDefaultResetValues( CCurveItem* pItem, MUSIC_TIME mtClockLength  )
{
	pItem->m_bFlags &= ~DMUS_CURVE_RESET;	
	pItem->m_nResetValue = 0;	
	pItem->m_mtResetDuration = 0;
	
	switch( pItem->m_bType )
	{
		case DMUS_CURVET_PBCURVE:
			pItem->m_bFlags |= DMUS_CURVE_RESET;	
			pItem->m_nResetValue = 8192;	
			pItem->m_mtResetDuration = mtClockLength;
			break;
		
		case DMUS_CURVET_CCCURVE:
			switch( pItem->m_bCCData )
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
					pItem->m_bFlags |= DMUS_CURVE_RESET;	
					pItem->m_nResetValue = 0;	
					pItem->m_mtResetDuration = mtClockLength;
					break;
			}
			break;

		case DMUS_CURVET_PATCURVE:
		case DMUS_CURVET_MATCURVE:
			pItem->m_bFlags |= DMUS_CURVE_RESET;	
			pItem->m_nResetValue = 0;	
			pItem->m_mtResetDuration = mtClockLength;
			break;
	}
}



HRESULT ReadMidiSequenceFromStream(LPSTREAM pOrigStream,
								  CTypedPtrList<CPtrList, CSequenceItem*>& lstSequences,
								  CTypedPtrList<CPtrList, CCurveItem*>& lstCurves,
								  long	lOffsetTime,
								  DWORD& dwLength)
{
	IStream* pStream;

	pOrigStream->Clone(&pStream);

	if(pStream == NULL)
	{
		return E_POINTER;
	}

	HRESULT hr = S_OK;
    DWORD dwID;
    DWORD dwCurTime;
	DWORD dwSize;
    short nFormat;
    short nNumTracks;
    short nTracksRead;
     FullSeqEvent* lstEvent;
	FullSeqEvent* lstTrackEvent;


    lstEvent = NULL;

    nNumTracks = nTracksRead = 0;
    dwLength = 0;

	FillMemory(&gBankSelect, (sizeof(DMUS_IO_BANKSELECT_ITEM) * NUM_MIDI_CHANNELS), 0xFF);

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
    GetMShort( pStream, gnPPQN );
    if( dwSize > 6 )
    {
		StreamSeek( pStream, dwSize - 6, STREAM_SEEK_CUR );
    }
    pStream->Read( &dwID, sizeof( FOURCC ), NULL );
    while( dwID == mmioFOURCC( 'M', 'T', 'r', 'k' ) )
    {
		GetMLong( pStream, dwSize );
		dwCurTime = 0;
		lstTrackEvent = NULL;
		while( dwSize > 0 )
		{
			dwSize -= GetVarLength( pStream, dwID );
			dwCurTime += dwID;
			dwSize -= ReadEvent( pStream, dwCurTime, &lstTrackEvent, NULL);
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
		lstTrackEvent = SortEventList( lstTrackEvent );
		lstTrackEvent = CompressEventList( lstTrackEvent );
		lstEvent = List_Cat( lstEvent, lstTrackEvent );
		if( FAILED( pStream->Read( &dwID, sizeof( FOURCC ), NULL ) ) )
		{
			break;
		}
    }
	dwLength = ConvertTime(dwLength);

    lstEvent = SortEventList( lstEvent );


	if(lstEvent)
	{

		// bugbug could be checking to see if there are actually tempo events,
		// sysex events, etc. to see if it's really necessary to create these
		// tracks...
		// Create a Tempo Track chunk in which to store the tempo events
		if( gpTempoStream )
		{
			// nothing to do
		}


		// sort list
//		lstEvent = SortEventList(lstEvent); 

		// Create a Sequence Track in which to store the notes, curves
		// and SysEx events.
		//
		CSequenceItem* pItem;
		CCurveItem* pCurveItem;
		dwLength = 0;
		if(hr == S_OK)
		{
			for(FullSeqEvent* pEvent = lstEvent; pEvent; pEvent = pEvent->pNext )
			{
				pEvent->mtTime += lOffsetTime;

				if( (Status(pEvent->bStatus) == MIDI_PBEND)
				||  (Status(pEvent->bStatus) == MIDI_PTOUCH)
				||  (Status(pEvent->bStatus) == MIDI_MTOUCH)
				||  (Status(pEvent->bStatus) == MIDI_CCHANGE) )
				{
					pCurveItem = new CCurveItem( pEvent );
					if( pCurveItem )
					{
						lstCurves.AddTail( pCurveItem );
					}
				}
				else
				{
					pItem = new CSequenceItem( pEvent );
					if( pItem )
					{
						lstSequences.AddTail( pItem );
					}
				}
				
				if( dwLength < (DWORD)(pEvent->mtTime + pEvent->mtDuration) )
				{
					dwLength = pEvent->mtTime + pEvent->mtDuration;
				}
			}
		}

		if( gpSysExStream && hr == S_OK)
		{
			// nothing to do
		}

		if( gpTimeSigStream && hr == S_OK)
		{
			// nothing to do
		}
	}
	else
	{
		hr = E_POINTER;
	}
	

	List_Free( lstEvent );

	// release our hold on the streams
	if(gpTempoStream)
		gpTempoStream->Release();
	if(gpSysExStream)
		gpSysExStream->Release();
	if(gpTimeSigStream)
		gpTimeSigStream->Release();
	gpTempoStream = NULL;
	gpSysExStream = NULL;
	gpTimeSigStream = NULL;
	return hr;
}
