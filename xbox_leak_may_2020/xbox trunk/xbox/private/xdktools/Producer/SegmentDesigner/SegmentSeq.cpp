// SegmentSeq.cpp : implementation file
//

#include "stdafx.h"
#include "SegmentDesignerDLL.h"
#include "Segment.h"
#include "Track.h"
#include "Pre388_dmusicf.h"
#include <dmusicf.h>
#include <RiffStrm.h>
#include "Track.h"
#include "SegmentRiff.h"
#include "SegmentComponent.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define OLD_DMUS_FOURCC_SEQ_TRACK	mmioFOURCC('s','q','t','r')
#define OLD_DMUS_FOURCC_SEQ_LIST	mmioFOURCC('s','e','q','l')
#define OLD_DMUS_FOURCC_CURVE_LIST	mmioFOURCC('c','r','v','l')

// This class encapusulates a list of MIDI events, all of which play on one PChannel.
// It is used to split a MIDI file into separate Sequence tracks.
class CSeqTrack
{
public:
	CSeqTrack()
	{
		// Clean up the sequence and curve lists
		m_lstSequences.RemoveAll();
		m_lstCurves.RemoveAll();
	}
	~CSeqTrack()
	{
		// Clean up the sequence and curve lists
		while( !m_lstSequences.IsEmpty() )
		{
			delete m_lstSequences.RemoveHead();
		}
		while( !m_lstCurves.IsEmpty() )
		{
			delete m_lstCurves.RemoveHead();
		}
	}
	void FillCTrack( CTrack *pTrack, DWORD dwGroupBits, DWORD dwPosition );
	static HRESULT FillStream( IStream *pIStream, 
				 	CTypedPtrList<CPtrList, DMUS_IO_SEQ_ITEM*> &lstSequences,
					CTypedPtrList<CPtrList, DMUS_IO_CURVE_ITEM*> &lstCurves );
	CTypedPtrList<CPtrList, DMUS_IO_SEQ_ITEM*> m_lstSequences;
	CTypedPtrList<CPtrList, DMUS_IO_CURVE_ITEM*> m_lstCurves;
};

HRESULT CSeqTrack::FillStream( IStream *pIStream, 
							   CTypedPtrList<CPtrList, DMUS_IO_SEQ_ITEM*> &lstSequences,
							   CTypedPtrList<CPtrList, DMUS_IO_CURVE_ITEM*> &lstCurves )
{
	// This method fills in pIStream with the list of sequences and curves passed in
	// All items are removed from lstSequences and lstCurves if this method completes successfully
	IDMUSProdRIFFStream *pIRiffStream = NULL;
	if( FAILED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
    {
		TRACE("Segment: Sequence import unable to alloc RIFF stream\n");
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	{
		WriteChunk chunk(pIRiffStream, DMUS_FOURCC_SEQ_TRACK);
		DWORD dwBytesWritten;
		hr = chunk.State();
		if(hr == S_OK)
		{
			// write notes
			WriteChunk chunk(pIRiffStream, DMUS_FOURCC_SEQ_LIST);
			hr = chunk.State();
			if(hr == S_OK)
			{
				WriteDWORD(pIRiffStream, sizeof(DMUS_IO_SEQ_ITEM));

				DMUS_IO_SEQ_ITEM *pSequence;
				while( !lstSequences.IsEmpty() )
				{
					pSequence = lstSequences.RemoveHead();

					hr = pIStream->Write( pSequence, sizeof(DMUS_IO_SEQ_ITEM), &dwBytesWritten );
					delete pSequence;
					if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_SEQ_ITEM) )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}

					if( FAILED( hr ) )
					{
						goto ON_ERROR;
					}
				}
			}
		}
		// write curves
		if(hr == S_OK)
		{
			WriteChunk curve(pIRiffStream, DMUS_FOURCC_CURVE_LIST);
			hr = curve.State();
			if(hr == S_OK)
			{			
				WriteDWORD(pIRiffStream, sizeof(DMUS_IO_CURVE_ITEM));

				DMUS_IO_CURVE_ITEM *pCurve;
				while( !lstCurves.IsEmpty() )
				{
					pCurve = lstCurves.RemoveHead();

					hr = pIStream->Write( pCurve, sizeof(DMUS_IO_CURVE_ITEM), &dwBytesWritten );
					delete pCurve;
					if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_CURVE_ITEM) )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}

					if( FAILED( hr ) )
					{
						goto ON_ERROR;
					}
				}
			}
		}
	}

ON_ERROR:
	pIRiffStream->Release();
	return hr;
}

void CSeqTrack::FillCTrack( CTrack *pTrack, DWORD dwGroupBits, DWORD dwPosition )
{
	ASSERT( pTrack );
	memcpy( &(pTrack->m_guidClassID), &CLSID_DirectMusicSeqTrack, sizeof( GUID ) );
	pTrack->m_ckid = DMUS_FOURCC_SEQ_TRACK;
	pTrack->m_dwGroupBits = dwGroupBits;
	pTrack->m_dwPosition = dwPosition;

	IStream *pIStream;
	pTrack->GetStream( &pIStream );

	FillStream( pIStream, m_lstSequences, m_lstCurves );

	pTrack->SetStream( pIStream );
	pIStream->Release();
}

inline BYTE Status(BYTE x)
{
	return static_cast<BYTE>(x & 0xF0);
}

inline BYTE Channel(BYTE x)
{
	return static_cast<BYTE>(x & 0xf);
}

#define MIDI_NOTEON     0x90
#define MIDI_PBEND      0xE0
#define MIDI_PTOUCH     0xA0
#define MIDI_MTOUCH     0xD0

void SetCurve( DMUS_IO_CURVE_ITEM *pItem, BYTE bStatus, BYTE bByte1, BYTE bByte2 )
{
	switch( Status(bStatus) )
	{
		case MIDI_PBEND:
			pItem->bType = DMUS_CURVET_PBCURVE;
			pItem->bCCData = 0;
			pItem->nStartValue = (short)(((bByte2 & 0x7F) << 7) + (bByte1 & 0x7F));
			pItem->nEndValue = (short)(((bByte2 & 0x7F) << 7) + (bByte1 & 0x7F));
			break;
		case MIDI_PTOUCH:
			pItem->bType = DMUS_CURVET_PATCURVE;
			pItem->bCCData = bByte1;
			pItem->nStartValue = bByte2;
			pItem->nEndValue = bByte2;
			break;
		case MIDI_MTOUCH:
			pItem->bType = DMUS_CURVET_MATCURVE;
			pItem->bCCData = 0;
			pItem->nStartValue = bByte1;
			pItem->nEndValue = bByte1;
			break;
		default:
			pItem->bType = DMUS_CURVET_CCCURVE;
			pItem->bCCData = bByte1;
			pItem->nStartValue = bByte2;
			pItem->nEndValue = bByte2;
			break;
	}
}

HRESULT LoadSeqTrack( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO ck,
				 	CTypedPtrList<CPtrList, DMUS_IO_SEQ_ITEM*> &lstSequences,
					CTypedPtrList<CPtrList, DMUS_IO_CURVE_ITEM*> &lstCurves )
{
	ASSERT( pIRiffStream );
	if( pIRiffStream == NULL )
	{
		return E_POINTER;
	}

	IStream *pIStream;
	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream );
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	HRESULT hr = E_FAIL;

	MMCKINFO ck1;
	ck1.ckid = DMUS_FOURCC_SEQ_LIST;
	if(pIRiffStream->Descend(&ck1, &ck, MMIO_FINDCHUNK) == 0)
	{
		DWORD dwSequenceSize, dwByteCount;
		hr = pIStream->Read(&dwSequenceSize, sizeof(DWORD), &dwByteCount);
		if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		dwSequenceSize = min( dwSequenceSize, sizeof(DMUS_IO_SEQ_ITEM) );

		DMUS_IO_SEQ_ITEM iSequence;
		long lChunkSize = ck1.cksize - sizeof(DWORD);
		while(lChunkSize > 0)
		{
			ZeroMemory(	&iSequence, sizeof(DMUS_IO_SEQ_ITEM) );
			hr = pIStream->Read( &iSequence, dwSequenceSize, &dwByteCount );
			if( FAILED( hr ) || dwByteCount != dwSequenceSize )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			lChunkSize -= dwSequenceSize;

			if( Status(iSequence.bStatus) != MIDI_NOTEON )
			{
				DMUS_IO_CURVE_ITEM* pItem = new DMUS_IO_CURVE_ITEM;
				if ( pItem == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}

				ZeroMemory( pItem, sizeof( DMUS_IO_CURVE_ITEM ) );

				pItem->mtStart = iSequence.mtTime;
				pItem->mtDuration = iSequence.mtDuration;
				//pItem->mtResetDuration = iSequence.;
				pItem->dwPChannel = iSequence.dwPChannel;
				pItem->nOffset = iSequence.nOffset;
				SetCurve( pItem, iSequence.bStatus, iSequence.bByte1, iSequence.bByte2 );
				//pItem->nResetValue = iSequence.;
				pItem->bCurveShape = DMUS_CURVES_INSTANT;
				//pItem->bFlags = iSequence.;

				lstCurves.AddTail( pItem );
			}
			else
			{
				DMUS_IO_SEQ_ITEM* pItem = new DMUS_IO_SEQ_ITEM;
				if ( pItem == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}

				memcpy( pItem, &iSequence, sizeof( DMUS_IO_SEQ_ITEM ) );

				lstSequences.AddTail( pItem );
			}
		}
		pIRiffStream->Ascend(&ck1, 0);
	}

	ck1.ckid = DMUS_FOURCC_CURVE_LIST;
	if(pIRiffStream->Descend(&ck1, &ck, MMIO_FINDCHUNK) == 0)
	{
		DWORD dwSequenceSize, dwByteCount;
		hr = pIStream->Read(&dwSequenceSize, sizeof(DWORD), &dwByteCount);
		if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		dwSequenceSize = min( dwSequenceSize, sizeof(DMUS_IO_CURVE_ITEM) );

		DMUS_IO_CURVE_ITEM iCurve;
		long lChunkSize = ck1.cksize - sizeof(DWORD);
		while(lChunkSize > 0)
		{
			ZeroMemory(	&iCurve, sizeof(DMUS_IO_CURVE_ITEM) );
			hr = pIStream->Read( &iCurve, dwSequenceSize, &dwByteCount );
			if( FAILED( hr ) || dwByteCount != dwSequenceSize )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			lChunkSize -= dwSequenceSize;

			DMUS_IO_CURVE_ITEM* pItem = new DMUS_IO_CURVE_ITEM;
			if ( pItem == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto ON_ERROR;
			}

			memcpy( pItem, &iCurve, sizeof( DMUS_IO_CURVE_ITEM ) );

			lstCurves.AddTail( pItem );
		}
		pIRiffStream->Ascend(&ck1, 0);
	}

ON_ERROR:
	pIStream->Release();
	return hr;
}

HRESULT LoadOldSeqTrack( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO ck,
				 	CTypedPtrList<CPtrList, DMUS_IO_SEQ_ITEM*> &lstSequences,
					CTypedPtrList<CPtrList, DMUS_IO_CURVE_ITEM*> &lstCurves )
{
	ASSERT( pIRiffStream );
	if( pIRiffStream == NULL )
	{
		return E_POINTER;
	}

	IStream *pIStream;
	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream );
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	long lChunkSize;
	DWORD dwSequenceSize, dwByteCount;
	HRESULT hr = pIStream->Read( &dwSequenceSize, sizeof(DWORD), &dwByteCount );
	if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	dwSequenceSize = min( dwSequenceSize, sizeof(PRE388_DMUS_IO_SEQ_ITEM) );

	PRE388_DMUS_IO_SEQ_ITEM i388Sequence;

	lChunkSize = ck.cksize - sizeof(DWORD);
	while(lChunkSize > 0)
	{
		ZeroMemory(	&i388Sequence, sizeof(PRE388_DMUS_IO_SEQ_ITEM) );
		hr = pIStream->Read( &i388Sequence, dwSequenceSize, &dwByteCount );
		if( FAILED( hr ) || dwByteCount != dwSequenceSize )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		lChunkSize -= dwSequenceSize;

		if( Status(i388Sequence.bStatus) != MIDI_NOTEON )
		{
			DMUS_IO_CURVE_ITEM* pItem = new DMUS_IO_CURVE_ITEM;
			if ( pItem == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto ON_ERROR;
			}

			ZeroMemory( pItem, sizeof( DMUS_IO_CURVE_ITEM ) );

			pItem->mtStart = i388Sequence.lTime;
			pItem->mtDuration = i388Sequence.lDuration;
			//pItem->mtResetDuration = i388Sequence.;
			pItem->dwPChannel = Channel(i388Sequence.bStatus);
			//pItem->nOffset = 0;
			//pItem->nResetValue = i388Sequence.;
			pItem->bCurveShape = DMUS_CURVES_INSTANT;
			//pItem->bFlags = i388Sequence.;
			SetCurve( pItem, i388Sequence.bStatus, i388Sequence.bByte1, i388Sequence.bByte2 );

			lstCurves.AddTail( pItem );
		}
		else
		{
			DMUS_IO_SEQ_ITEM* pItem = new DMUS_IO_SEQ_ITEM;
			if ( pItem == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto ON_ERROR;
			}

			pItem->mtTime = i388Sequence.lTime;
			pItem->mtDuration = i388Sequence.lDuration;
			pItem->dwPChannel = Channel(i388Sequence.bStatus);
			pItem->nOffset = 0;
			pItem->bStatus = Status(i388Sequence.bStatus);
			pItem->bByte1 = i388Sequence.bByte1;
			pItem->bByte2 = i388Sequence.bByte2;

			lstSequences.AddTail( pItem );
		}
	}

ON_ERROR:
	pIStream->Release();
	return hr;
}

HRESULT LoadBetaIIISeq( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO ck,
				 	CTypedPtrList<CPtrList, DMUS_IO_SEQ_ITEM*> &lstSequences,
					CTypedPtrList<CPtrList, DMUS_IO_CURVE_ITEM*> &lstCurves )
{
	ASSERT( pIRiffStream );
	if( pIRiffStream == NULL )
	{
		return E_POINTER;
	}

	IStream *pIStream;
	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream );
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	HRESULT hr = E_FAIL;
	// pre388 beta III sequence
	PRE388_DMUS_IO_SEQ_ITEM i388Sequence;
	PRE388_DMUS_IO_CURVE_ITEM i388Curve;
	MMCKINFO ck1;
	ck1.ckid = OLD_DMUS_FOURCC_SEQ_LIST;
	if(pIRiffStream->Descend(&ck1, &ck, MMIO_FINDCHUNK) == 0)
	{
		DWORD dwSequenceSize, dwByteCount;
		hr = pIStream->Read(&dwSequenceSize, sizeof(DWORD), &dwByteCount);
		if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		dwSequenceSize = min( dwSequenceSize, sizeof(DMUS_IO_SEQ_ITEM) );

		long lChunkSize = ck1.cksize - sizeof(DWORD);
		while(lChunkSize > 0)
		{
			ZeroMemory(	&i388Sequence, sizeof(PRE388_DMUS_IO_SEQ_ITEM) );
			hr = pIStream->Read( &i388Sequence, dwSequenceSize, &dwByteCount );
			if( FAILED( hr ) || dwByteCount != dwSequenceSize )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			lChunkSize -= dwSequenceSize;

			DMUS_IO_SEQ_ITEM* pItem = new DMUS_IO_SEQ_ITEM;
			if ( pItem == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto ON_ERROR;
			}

			pItem->mtTime = i388Sequence.lTime;
			pItem->mtDuration = i388Sequence.lDuration;
			pItem->dwPChannel = Channel(i388Sequence.bStatus);
			pItem->nOffset = 0;
			pItem->bStatus = Status(i388Sequence.bStatus);
			pItem->bByte1 = i388Sequence.bByte1;
			pItem->bByte2 = i388Sequence.bByte2;

			lstSequences.AddTail( pItem );
		}
		pIRiffStream->Ascend(&ck1, 0);
	}
	ck1.ckid = OLD_DMUS_FOURCC_CURVE_LIST;
	if(pIRiffStream->Descend(&ck1, &ck, MMIO_FINDCHUNK) == 0)
	{
		DWORD dwSequenceSize, dwByteCount;
		hr = pIStream->Read(&dwSequenceSize, sizeof(DWORD), &dwByteCount);
		if( FAILED( hr ) || dwByteCount != sizeof(DWORD) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		dwSequenceSize = min( dwSequenceSize, sizeof(PRE388_DMUS_IO_CURVE_ITEM) );

		long lChunkSize = ck1.cksize - sizeof(DWORD);

		while(lChunkSize > 0)
		{
			ZeroMemory(	&i388Curve, sizeof(PRE388_DMUS_IO_CURVE_ITEM) );
			hr = pIStream->Read( &i388Curve, dwSequenceSize, &dwByteCount );
			if( FAILED( hr ) || dwByteCount != dwSequenceSize )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			lChunkSize -= dwSequenceSize;

			DMUS_IO_CURVE_ITEM* pItem = new DMUS_IO_CURVE_ITEM;
			if ( pItem == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto ON_ERROR;
			}

			pItem->mtStart = i388Curve.mtStart;
			pItem->mtDuration = i388Curve.mtDuration;
			pItem->mtResetDuration = i388Curve.mtResetDuration;
			pItem->dwPChannel = i388Curve.dwPChannel;
			pItem->nOffset = 0;
			pItem->nStartValue = i388Curve.nStartValue;
			pItem->nEndValue = i388Curve.nEndValue;
			pItem->nResetValue = i388Curve.nResetValue;
			pItem->bType = i388Curve.bType;
			pItem->bCurveShape = i388Curve.bCurveShape;
			pItem->bCCData = i388Curve.bCCData;
			pItem->bFlags = i388Curve.bFlags;

			lstCurves.AddTail( pItem );
		}
		pIRiffStream->Ascend(&ck1, 0);
	}

ON_ERROR:
	pIStream->Release();
	return hr;
}

HRESULT	CSegment::AddSequenceTrack( CTrack *pTrack )
{
	// Set up all the variables we'll need
	HRESULT hr = S_OK;
	IStream *pIStream = NULL;
	pTrack->GetStream( &pIStream );
	CTypedPtrList<CPtrList, DMUS_IO_SEQ_ITEM*> lstSequences;
	CTypedPtrList<CPtrList, DMUS_IO_CURVE_ITEM*> lstCurves;

	// Store unknown chunks in this list and array
	CTypedPtrList<CPtrList, BYTE*> lstUnknownData;
	CTypedPtrList<CPtrList, MMCKINFO*> lstUnknownChunks;

	// TRUE if we need to rebuild the main track
	BOOL fNeedToRebuild = FALSE;

	// Alloc a RIFF stream to parse the sequence track
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	hr = AllocRIFFStream( pIStream, &pIRiffStream );
	if( SUCCEEDED( hr ) )
	{
		// Load the Track
		MMCKINFO	ck;
		while( SUCCEEDED( hr ) && (pIRiffStream->Descend( &ck, NULL, 0 ) == 0) )
		{
			switch( ck.ckid )
			{
			case DMUS_FOURCC_SEQ_TRACK:
				{
					// Check for a DX 6.1 sequence chunk
					MMCKINFO ck1;
					ck1.ckid = DMUS_FOURCC_SEQ_LIST;
					DWORD dwPos = StreamTell(pIStream);
					if(pIRiffStream->Descend(&ck1, &ck, MMIO_FINDCHUNK) == 0)
					{
						if(StreamSeek(pIStream, dwPos, STREAM_SEEK_SET) == S_OK)
						{
							hr = LoadSeqTrack( pIRiffStream, ck, lstSequences, lstCurves );
							if( FAILED( hr ) )
							{
								hr = E_FAIL;
							}
						}
					}
					// Didn't find a DX 6.1 sequence chunk, seek back the the start and load a
					// Beta 3 Sequence chunk
					else if(StreamSeek(pIStream, dwPos, STREAM_SEEK_SET) == S_OK)
					{
						hr = LoadBetaIIISeq( pIRiffStream, ck, lstSequences, lstCurves );
						if( FAILED( hr ) )
						{
							hr = E_FAIL;
						}
						else
						{
							// This track is in an old format, need to re-save in the new format
							fNeedToRebuild = TRUE;
						}
					}
				}
				break;

			case OLD_DMUS_FOURCC_SEQ_TRACK:
				// Found a pre-beta 3 Sequence chunk.
				hr = LoadOldSeqTrack( pIRiffStream, ck, lstSequences, lstCurves );
				if( FAILED( hr ) )
				{
					hr = E_FAIL;
				}
				else
				{
					// This track is in an old format, need to re-save in the new format
					fNeedToRebuild = TRUE;
				}
				break;

			default:
				{
					// Found an unknown chunk (most likely an UNFO list)
					// Create a chunk of memory to save it in
					BYTE *pMem = new BYTE[ck.cksize];
					if( pMem )
					{
						// Read the chunk into memory
						if( SUCCEEDED( pIStream->Read( pMem, ck.cksize, NULL ) ) )
						{
							// Save the MMCKINFO structure (ckid, fccType)
							MMCKINFO *pCkInfo = new MMCKINFO;
							memcpy( pCkInfo, &ck, sizeof(MMCKINFO) );

							// Add the memory and MMCKINFO structure to the 'unknown' lists
							lstUnknownChunks.AddTail( pCkInfo );
							lstUnknownData.AddTail( pMem );
						}
						else
						{
							// Unable to read from stream, delete the memory we allocated
							delete pMem;
						}
					}
				}
				break;
			}
			pIRiffStream->Ascend( &ck, 0 );
		}

		if( pIRiffStream )
		{
			pIRiffStream->Release();
			pIRiffStream = NULL;
		}
	}

	if( SUCCEEDED( hr ) )
	{
		// Create and initialize the mapping of sequence tracks
		CMap< DWORD, DWORD, CSeqTrack*, CSeqTrack*& > m_mpSeqTracks;

		// The PChannel for the main track (set by the first event)
		DWORD dwPChannel = -1;

		// Iterate through all note events in this track
		DMUS_IO_SEQ_ITEM *pSeqItem;
		POSITION pos2, pos = lstSequences.GetHeadPosition();
		while( pos )
		{
			// Save the current position, in case we need to remove this event
			pos2 = pos;
			pSeqItem = lstSequences.GetNext( pos );

			// If the PChannel has not been set, set it
			if( dwPChannel == -1 )
			{
				dwPChannel = pSeqItem->dwPChannel;
			}
			// Otherwise, check to see if this event is different from any others
			else if( dwPChannel != pSeqItem->dwPChannel )
			{
				// If a track on this PChannel doesn't yet exist, create one
				CSeqTrack* pSeqTrack;
				if( !m_mpSeqTracks.Lookup( pSeqItem->dwPChannel, pSeqTrack ) )
				{
					pSeqTrack = new CSeqTrack();
					m_mpSeqTracks.SetAt( pSeqItem->dwPChannel, pSeqTrack );
				}

				// Add this sequence to the list of sequences for this track
				pSeqTrack->m_lstSequences.AddTail( pSeqItem );

				// Remove the event from the main track
				lstSequences.RemoveAt( pos2 );

				// Set the flag so we rebuild the main track
				fNeedToRebuild = TRUE;
			}
		}

		// Iterate through all controller events in this track
		DMUS_IO_CURVE_ITEM *pCurveItem;
		pos = lstCurves.GetHeadPosition();
		while( pos )
		{
			// Save the current position, in case we need to remove this event
			pos2 = pos;
			pCurveItem = lstCurves.GetNext( pos );

			// If the PChannel has not been set, set it
			if( dwPChannel == -1 )
			{
				dwPChannel = pCurveItem->dwPChannel;
			}
			// Otherwise, check to see if this event is different from any others
			else if( dwPChannel != pCurveItem->dwPChannel )
			{
				// If a track on this PChannel doesn't yet exist, create one
				CSeqTrack* pSeqTrack;
				if( !m_mpSeqTracks.Lookup( pCurveItem->dwPChannel, pSeqTrack ) )
				{
					pSeqTrack = new CSeqTrack();
					m_mpSeqTracks.SetAt( pCurveItem->dwPChannel, pSeqTrack );
				}

				// Add this curve to the list of curves for this track
				pSeqTrack->m_lstCurves.AddTail( pCurveItem );

				// Remove the event from the main track
				lstCurves.RemoveAt( pos2 );

				// Set the flag so we rebuild the main track
				fNeedToRebuild = TRUE;
			}
		}

		// If we need to, rebuild the original track
		if( fNeedToRebuild )
		{
			// Allocate a stream for the track's events
			IStream *pITmpStream;
			if( SUCCEEDED( m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pITmpStream ) ) )
			{
 				IDMUSProdRIFFStream *pIRiffStream = NULL;
				if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
				{
					// Iterate through the unknown chunks and add them to the stream
					while( !lstUnknownChunks.IsEmpty() )
					{
						// Get the MMCKINFO pointer and a pointer to the block of data for this chunk
						MMCKINFO *pCkInfo = lstUnknownChunks.RemoveHead();
						BYTE *pData = lstUnknownData.RemoveHead();

						// Save the size of this chunk
						DWORD dwSize = pCkInfo->cksize;

						// Create the right kind of chunk
						if( pCkInfo->ckid == FOURCC_LIST )
						{
							pIRiffStream->CreateChunk( pCkInfo, MMIO_CREATELIST );
						}
						else if ( pCkInfo->ckid == FOURCC_RIFF )
						{
							pIRiffStream->CreateChunk( pCkInfo, MMIO_CREATERIFF );
						}
						else
						{
							pIRiffStream->CreateChunk( pCkInfo, 0 );
						}

						// Write out the data and ascend out of the chunk
						pITmpStream->Write( pData, dwSize, NULL );
						pIRiffStream->Ascend( pCkInfo, 0 );

						// Delete the copy of the data and chunk in memory, since we no longer need them
						delete []pData;
						delete pCkInfo;
					}

					// Release the RIFFStream we created
					pIRiffStream->Release();
					pIRiffStream = NULL;
				}

				// This writes out the sequence and curve chunks (and their parent chunk)
				// and removes all items from lstSequences and lstCurves
				CSeqTrack::FillStream( pITmpStream, lstSequences, lstCurves );

				pTrack->SetStream( pITmpStream );
				pITmpStream->Release();
				hr = AddTrack( pTrack );
			}
			else
			{
				TRACE("Segment: Sequence rebuild unable to alloc memory stream\n");
			}

			// If we added any new track, fNeedToRebuild must have been set to TRUE 
			// Add new tracks if we created any

			CSeqTrack* pSeqTrack;
			DWORD dwPChannel;
			POSITION pos = m_mpSeqTracks.GetStartPosition();
			while( pos )
			{
				m_mpSeqTracks.GetNextAssoc( pos, dwPChannel, pSeqTrack );

				if( pSeqTrack )
				{
					// Allocate a strip to store the new track's events in
					IStream *pITmpStream;
					if( SUCCEEDED( m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pITmpStream ) ) )
					{
						// Create a new CTrack for this track
						CTrack *pTmpTrack = new CTrack;

						// Set its stream pointer and release our reference to the stream
						pTmpTrack->SetStream( pITmpStream );
						pITmpStream->Release();

						// Fill in the CTrack class with information for this track
						pSeqTrack->FillCTrack( pTmpTrack, pTrack->m_dwGroupBits, pTrack->m_dwPosition );

						// Add this track to the segment's list and delete the local CSeqTrack object
						AddTrack( pTmpTrack );
						delete pSeqTrack;
					}
					else
					{
						TRACE("Segment: Sequence import unable to alloc memory stream\n");
					}
				}
			}
		}
		// Don't need to rebuild, just add the existing pTrack
		else
		{
			hr = AddTrack( pTrack );

			// Now, clean up after ourself
			while( !lstSequences.IsEmpty() )
			{
				delete lstSequences.RemoveHead();
			}
			while( !lstCurves.IsEmpty() )
			{
				delete lstCurves.RemoveHead();
			}
			while( !lstUnknownChunks.IsEmpty() )
			{
				delete lstUnknownChunks.RemoveHead();
			}
			while( !lstUnknownData.IsEmpty() )
			{
				delete []lstUnknownData.RemoveHead();
			}
		}
	}

	if( pIStream )
	{
		pIStream->Release();
		pIStream = NULL;
	}

	return hr;
}

void MergeSeqList( CTypedPtrList<CPtrList, DMUS_IO_SEQ_ITEM*>& m_lstSequences, CTypedPtrList<CPtrList, DMUS_IO_SEQ_ITEM*>& lstSequences )
{
	// We assume m_lstSequences and lstSequences are in sorted (ascending) order
	if( lstSequences.IsEmpty() )
	{
		return;
	}

	POSITION pos = m_lstSequences.GetHeadPosition();
	while( !lstSequences.IsEmpty() )
	{
		DMUS_IO_SEQ_ITEM *pMergeSeq = lstSequences.RemoveHead();

		while( pos && ( (pMergeSeq->mtTime + pMergeSeq->nOffset) > (m_lstSequences.GetAt(pos)->mtTime + m_lstSequences.GetAt(pos)->nOffset) ) )
		{
			m_lstSequences.GetNext( pos );
		}

		if( pos )
		{
			m_lstSequences.InsertBefore( pos, pMergeSeq );
		}
		else
		{
			m_lstSequences.AddTail( pMergeSeq );
		}
	}
}

void MergeCurveList( CTypedPtrList<CPtrList, DMUS_IO_CURVE_ITEM*>& m_lstCurves, CTypedPtrList<CPtrList, DMUS_IO_CURVE_ITEM*>& lstCurves )
{
	// We assume m_lstCurves and lstCurves are in sorted (ascending) order
	if( lstCurves.IsEmpty() )
	{
		return;
	}

	POSITION pos = m_lstCurves.GetHeadPosition();
	while( !lstCurves.IsEmpty() )
	{
		DMUS_IO_CURVE_ITEM *pMergeCurve = lstCurves.RemoveHead();

		while( pos && ( (pMergeCurve->mtStart + pMergeCurve->nOffset) > (m_lstCurves.GetAt(pos)->mtStart + m_lstCurves.GetAt(pos)->nOffset) ) )
		{
			m_lstCurves.GetNext( pos );
		}

		if( pos )
		{
			m_lstCurves.InsertBefore( pos, pMergeCurve );
		}
		else
		{
			m_lstCurves.AddTail( pMergeCurve );
		}
	}
}

HRESULT WriteCombinedSeqTracks( IStream *pIStream, CTypedPtrList<CPtrList, CTrack*> &lstSeqTracks )
{
	ASSERT( pIStream );
	if( !pIStream )
	{
		return E_POINTER;
	}

	// No tracks to save
	if( lstSeqTracks.IsEmpty() )
	{
		return S_FALSE;
	}

	CTypedPtrList<CPtrList, DMUS_IO_SEQ_ITEM*> lstSeqMain, lstSeqTmp;
	CTypedPtrList<CPtrList, DMUS_IO_CURVE_ITEM*> lstCurveMain, lstCurveTmp;
	HRESULT hr = S_OK;
	DWORD dwGroupBits = 0;

	CTrack *pTrack = NULL;
	while( !lstSeqTracks.IsEmpty() )
	{
		ASSERT( lstSeqTmp.IsEmpty() );
		ASSERT( lstCurveTmp.IsEmpty() );

		pTrack = lstSeqTracks.RemoveHead();

		ASSERT( !dwGroupBits || (dwGroupBits == pTrack->m_dwGroupBits) );
		dwGroupBits = pTrack->m_dwGroupBits;

		IStream *pStreamTrack = NULL;
		pTrack->GetStream( &pStreamTrack );
		ASSERT( pStreamTrack );
		if( !pStreamTrack )
		{
			TRACE("SegmentDesigner: WriteCombinedSeqTracks: Unable to get track's stream\n");
			continue;
		}

		IDMUSProdRIFFStream *pIRiffStream = NULL;
		if( FAILED( AllocRIFFStream( pStreamTrack, &pIRiffStream ) ) )
		{
			TRACE("SegmentDesigner: WriteCombinedSeqTracks: Unable to alloc RIFF stream for intpu\n");
			pStreamTrack->Release();
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Load the Track
		MMCKINFO	ck;
		while( pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
		{
			switch( ck.ckid )
			{
			case DMUS_FOURCC_SEQ_TRACK:
				{
					// Check for a DX 6.1 sequence chunk
					MMCKINFO ck1;
					ck1.ckid = DMUS_FOURCC_SEQ_LIST;
					DWORD dwPos = StreamTell(pStreamTrack);
					if(pIRiffStream->Descend(&ck1, &ck, MMIO_FINDCHUNK) == 0)
					{
						if(StreamSeek(pStreamTrack, dwPos, STREAM_SEEK_SET) == S_OK)
						{
							if( FAILED( LoadSeqTrack( pIRiffStream, ck, lstSeqTmp, lstCurveTmp ) ) )
							{
								TRACE("SegmentDesigner: WriteCombinedSeqTracks: Unable to read in sequence track\n");
								pIRiffStream->Release();
								pStreamTrack->Release();
								continue;
							}
						}
					}
				}
				break;
			}
			pIRiffStream->Ascend( &ck, 0 );
		}

		pStreamTrack->Release();
		//pStreamTrack = NULL;
		pIRiffStream->Release();
		//pIRiffStream = NULL;

		MergeSeqList( lstSeqMain, lstSeqTmp );
		MergeCurveList( lstCurveMain, lstCurveTmp );
	}

	// This should be pointing to the last track we read in
	ASSERT( pTrack );

	if( !lstSeqMain.IsEmpty() || !lstCurveMain.IsEmpty() || !pTrack )
	{
		// Allocate RIFF stream
		IDMUSProdRIFFStream *pIRiffStream = NULL;
		if( FAILED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			TRACE("SegmentDesigner: WriteCombinedSeqTracks: Unable to alloc RIFF stream for output\n");
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Create the RIFF chunk that surrounds the track
		MMCKINFO ckRiff;
		ckRiff.fccType = DMUS_FOURCC_TRACK_FORM;
		if( pIRiffStream->CreateChunk( &ckRiff, MMIO_CREATERIFF ) != 0 )
		{
			TRACE("SegmentDesigner: WriteCombinedSeqTracks: Unable to create RIFF chunk.\n");
			pIRiffStream->Release();
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write the Track Header chunk
		MMCKINFO ckTrkHeader;
		ckTrkHeader.ckid = DMUS_FOURCC_TRACK_CHUNK;
		if( pIRiffStream->CreateChunk( &ckTrkHeader, 0 ) != 0 )
		{
			TRACE("SegmentDesigner: WriteCombinedSeqTracks: Unable to create track header chunk.\n");
			pIRiffStream->Release();
			hr = E_FAIL;
			goto ON_ERROR;
		}

		DMUS_IO_TRACK_HEADER oDMTrack;
		DWORD dwByteCount;
		ZeroMemory( &oDMTrack, sizeof( DMUS_IO_TRACK_HEADER ) );
		pTrack->FillTrackHeader( &oDMTrack );
		hr = pIStream->Write( &oDMTrack, sizeof( DMUS_IO_TRACK_HEADER ), &dwByteCount );
		if( FAILED( hr )
			||  dwByteCount != sizeof( DMUS_IO_TRACK_HEADER ) )
		{
			TRACE("SegmentDesigner: WriteCombinedSeqTracks: Unable to write track header chunk.\n");
			pIRiffStream->Release();
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend( &ckTrkHeader, 0 ) != 0 )
		{
			TRACE("SegmentDesigner: WriteCombinedSeqTracks: Unable to ascend out of track header chunk.\n");
			pIRiffStream->Release();
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Check to see if the track extra's data is different from the defaults
		DMUS_IO_TRACK_EXTRAS_HEADER oDMTrackExtras;
		ZeroMemory( &oDMTrackExtras, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) );
		pTrack->FillTrackExtrasHeader( &oDMTrackExtras );
		if( oDMTrackExtras.dwFlags != DMUS_TRACKCONFIG_DEFAULT )
		{
			// Write the Track Extras Header chunk
			MMCKINFO ckTrkExtrasHeader;
			ckTrkExtrasHeader.ckid = DMUS_FOURCC_TRACK_EXTRAS_CHUNK;
			if( pIRiffStream->CreateChunk( &ckTrkExtrasHeader, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			hr = pIStream->Write( &oDMTrackExtras, sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ), &dwByteCount );
			if( FAILED( hr )
				||  dwByteCount != sizeof( DMUS_IO_TRACK_EXTRAS_HEADER ) )
			{
				hr = E_FAIL;
				TRACE("CSegment::DM_SaveTrackList: Unable to write track extras header chunk.\n");
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend( &ckTrkExtrasHeader, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		// This writes out the sequence and curve chunks (and their parent chunk)
		// and removes all items from lstSequences and lstCurves
		if( FAILED( CSeqTrack::FillStream( pIStream, lstSeqMain, lstCurveMain ) ) )
		{
			TRACE("SegmentDesigner: WriteCombinedSeqTracks: Unable to write seq and curve stream\n");
			pIRiffStream->Release();
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend( &ckRiff, 0 ) != 0 )
		{
			TRACE("SegmentDesigner: WriteCombinedSeqTracks: Unable to ascend out of RIFF chunk\n");
			pIRiffStream->Release();
			hr = E_FAIL;
			goto ON_ERROR;
		}

		pIRiffStream->Release();
	}

	ASSERT( lstSeqMain.IsEmpty() );
	ASSERT( lstSeqTmp.IsEmpty() );
	ASSERT( lstCurveMain.IsEmpty() );
	ASSERT( lstCurveTmp.IsEmpty() );

	ON_ERROR:
	while( !lstSeqMain.IsEmpty() )
	{
		delete lstSeqMain.RemoveHead();
	}
	while( !lstSeqTmp.IsEmpty() )
	{
		delete lstSeqTmp.RemoveHead();
	}
	while( !lstCurveMain.IsEmpty() )
	{
		delete lstCurveMain.RemoveHead();
	}
	while( !lstCurveTmp.IsEmpty() )
	{
		delete lstCurveTmp.RemoveHead();
	}

	return S_FALSE;
}
