#include "stdafx.h"
#include "templates.h"
#include <DMUSProd.h>
#include "DLLJazzDataObject.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include <dmusicf.h>
#include <mmreg.h>

#include "segmentriff.h"
#include "resource.h"


HRESULT SequenceChunk :: Extract(const DMUS_IO_SEQ_ITEM& item)
{
	memcpy(dynamic_cast<DMUS_IO_SEQ_ITEM*>(this), &item, sizeof(DMUS_IO_SEQ_ITEM));
	return S_OK;
}

HRESULT SequenceChunk :: Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	if(pRiffStream == 0)
		return E_INVALIDARG;
	WriteChunk chunk(pRiffStream, DMUS_FOURCC_SEQ_TRACK);
	HRESULT hr = chunk.State();
	if(hr == S_OK)
	{
		IStream* pStream = pRiffStream->GetStream();
		ASSERT(pStream);
		if(pStream == 0)
			return E_FAIL;
		DWORD dw;
		hr = pStream->Write(dynamic_cast<DMUS_IO_SEQ_ITEM*>(this), sizeof(DMUS_IO_SEQ_ITEM), &dw);
		hr = (hr == S_OK) && (dw == sizeof(DMUS_IO_SEQ_ITEM)) ? S_OK : E_FAIL;
		pStream->Release();
	}
	return hr;
}

HRESULT TempoChunk :: Extract(const DMUS_IO_TEMPO_ITEM& item)
{
	memcpy(dynamic_cast<DMUS_IO_TEMPO_ITEM*>(this), &item, sizeof(DMUS_IO_TEMPO_ITEM));
	return S_OK;
}

HRESULT TempoChunk :: Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	if(pRiffStream == 0)
		return E_INVALIDARG;
	WriteChunk chunk(pRiffStream, DMUS_FOURCC_TEMPO_TRACK);
	HRESULT hr = chunk.State();
	if(hr == S_OK)
	{
		IStream* pStream = pRiffStream->GetStream();
		ASSERT(pStream);
		if(pStream == 0)
			return E_FAIL;
		DWORD dw;
		hr = pStream->Write(dynamic_cast<DMUS_IO_TEMPO_ITEM*>(this), sizeof(DMUS_IO_TEMPO_ITEM), &dw);
		hr = (hr == S_OK) && (dw == sizeof(DMUS_IO_TEMPO_ITEM)) ? S_OK : E_FAIL;
		pStream->Release();
	}
	return hr;
}

HRESULT SysexChunk :: Extract(const DMUS_IO_SYSEX_ITEM& item)
{
	memcpy(dynamic_cast<DMUS_IO_SYSEX_ITEM*>(this), &item, sizeof(DMUS_IO_SYSEX_ITEM));
	return S_OK;
}

HRESULT SysexChunk :: Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	if(pRiffStream == 0)
		return E_INVALIDARG;
	WriteChunk chunk(pRiffStream, DMUS_FOURCC_SYSEX_TRACK);
	HRESULT hr = chunk.State();
	if(hr == S_OK)
	{
		IStream* pStream = pRiffStream->GetStream();
		ASSERT(pStream);
		if(pStream == 0)
			return E_FAIL;
		DWORD dw;
		hr = pStream->Write(dynamic_cast<DMUS_IO_SYSEX_ITEM*>(this), sizeof(DMUS_IO_SYSEX_ITEM), &dw);
		hr = (hr == S_OK) && (dw == sizeof(DMUS_IO_SYSEX_ITEM)) ? S_OK : E_FAIL;
		pStream->Release();
	}
	return hr;
}

HRESULT TimeSignatureChunk :: Extract(const DMUS_IO_TIMESIGNATURE_ITEM& item)
{
	memcpy(dynamic_cast<DMUS_IO_TIMESIGNATURE_ITEM*>(this), &item, sizeof(DMUS_IO_TIMESIGNATURE_ITEM));
	return S_OK;
}

HRESULT TimeSignatureChunk :: Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	if(pRiffStream == 0)
		return E_INVALIDARG;
	WriteChunk chunk(pRiffStream, DMUS_FOURCC_TIMESIGNATURE_TRACK);
	HRESULT hr = chunk.State();
	if(hr == S_OK)
	{
		IStream* pStream = pRiffStream->GetStream();
		ASSERT(pStream);
		if(pStream == 0)
			return E_FAIL;
		DWORD dw;
		hr = pStream->Write(dynamic_cast<DMUS_IO_TIMESIGNATURE_ITEM*>(this), sizeof(DMUS_IO_TIMESIGNATURE_ITEM), &dw);
		hr = (hr == S_OK) && (dw == sizeof(DMUS_IO_TIMESIGNATURE_ITEM)) ? S_OK : E_FAIL;
		pStream->Release();
	}
	return hr;
}

////////////////////////////// Write Band Name
HRESULT WriteBandName( IDMUSProdRIFFStream* pIRiffStream, CString& strName)
{
	IStream* pIStream;
	HRESULT hr = S_OK;
    MMCKINFO ckMain;
    MMCKINFO ck;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write INFO LIST header
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Segment name
	if( !strName.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &strName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}


	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	if( pIStream )
	{
		pIStream->Release();
	}
    return hr;
}


///////////////////////////////////////////////////// Band Riff Objects
///


void BandControllerArray::Init()
{
	for(int i = 0; i < MaxMidiChan; i++)
	{
		m_val[i] = BYTE(-1);
		m_time[i] = 0;
		m_occupied[i] = false;
	}
}

BandControllerArray::BandControllerArray(int type)
: m_type(type) 
{
	Init();
}

bool BandControllerArray::Add(FullSeqEvent* pEvent)
{
	BYTE type = pEvent->bByte1;
	if(type != m_type)
	{
		// wrong type
		return false;
	}

	ASSERT( 0 == (pEvent->bStatus & 0x0F) );
	BYTE chan = BYTE(pEvent->dwPChannel);
	if(!m_occupied[chan] || ( (pEvent->mtTime + pEvent->nOffset)  > m_time[chan]))
	{
		// add event
		m_val[chan] = pEvent->bByte2;
		m_time[chan] = pEvent->mtTime + pEvent->nOffset;
		m_occupied[chan] = true;
		return true;
	}
	else
	{
		return false;
	}
}

void BandPatchArray::Init()
{
	for(int i = 0; i < MaxMidiChan; i++)
	{
		memset(&(m_val[i]), 0, sizeof(DMUS_IO_PATCH_ITEM));
		m_time[i] = 0;
		m_val[i].byStatus = (BYTE) (0xC0 + i);
		m_occupied[i] = false;
	}
}

BandPatchArray::BandPatchArray()
{
	Init();
}

bool BandPatchArray::Add(DMUS_IO_PATCH_ITEM* patch)
{
	BYTE chan = BYTE(patch->byStatus & 0x0f);
	memcpy(&(m_val[chan]), patch, sizeof(DMUS_IO_PATCH_ITEM));
	m_time[chan] = patch->lTime;
	m_occupied[chan] = true;
	return true;
}


void FindControllersInTimespan(FullSeqEvent*& lstEvent, BandControllerArray& controllers, long beg, long end)
// this function assumes that lstEvent is sorted by time
// this is also ineffecient as each search starts from the top each time
{
	FullSeqEvent* pEvent = lstEvent;
	while(pEvent)
	{
		if( (pEvent->mtTime + pEvent->nOffset) < beg)
		{
			pEvent = pEvent->pNext;
			continue;
		}
		if( (pEvent->mtTime + pEvent->nOffset) > end )
			break;
		// candidate event
		controllers.Add(pEvent);
		pEvent = pEvent->pNext;
	}
}

int ChannelFromTrack( CMIDITrack *pMIDITrack )
{
	if( pMIDITrack->m_lstTrackEvent )
	{
		return pMIDITrack->m_lstTrackEvent->dwPChannel;
	}
	if( pMIDITrack->m_lstOther )
	{
		return pMIDITrack->m_lstOther->dwPChannel;
	}
	if( pMIDITrack->m_lstPan )
	{
		return pMIDITrack->m_lstPan->dwPChannel;
	}
	if( pMIDITrack->m_lstVol )
	{
		return pMIDITrack->m_lstVol->dwPChannel;
	}
	return -1;
}

void MergeVolPanIntoBand( CMIDITrack *pMIDITrackList, BandPatchArray &lastpatch, BOOL *afUsedPChannel, long lBandTime, long endTime, BandControllerArray &vol, BandControllerArray &pan )
{
	DMUS_IO_PATCH_ITEM ioPatchItem;
	for( int nChannel = 0; nChannel < 16; nChannel++ )
	{
		// If this channel has not yet been used, and there is a valid patch for it
		if( !afUsedPChannel[nChannel]
		&&	lastpatch.GetValue( nChannel, &ioPatchItem, false, true ) )
		{
			// Store the earliest time of notes, volume, and pan changes
			MUSIC_TIME mtEarliestVol = LONG_MAX;
			MUSIC_TIME mtEarliestPan = LONG_MAX;

			// Store the MIDI track to pull the volume and pan from
			CMIDITrack *pMIDITrackVolToUse = NULL;
			CMIDITrack *pMIDITrackPanToUse = NULL;

			// Iterate through all tracks looking for this channel
			CMIDITrack *pCurMIDITrack = pMIDITrackList;
			while( pCurMIDITrack )
			{
				// If this track contains this channel
				if( ChannelFromTrack( pCurMIDITrack ) == nChannel )
				{
					// If this volume is earlier than all others
					if( pCurMIDITrack->m_lstVol
					&&	pCurMIDITrack->m_lstVol->mtTime + pCurMIDITrack->m_lstVol->nOffset < mtEarliestVol )
					{
						// Save this track as the one to use
						pMIDITrackVolToUse = pCurMIDITrack;

						// Save the volume's time
						mtEarliestVol = pCurMIDITrack->m_lstVol->mtTime + pCurMIDITrack->m_lstVol->nOffset;
					}

					// If the first note event is before the first volume event
					if( pCurMIDITrack->m_lstTrackEvent
					&&	pCurMIDITrack->m_lstTrackEvent->mtTime + pCurMIDITrack->m_lstTrackEvent->nOffset <= mtEarliestVol )
					{
						// If the any of the note, volume, or band are not at exactly time 0, don't use this volume event
						// Otherwise (if all are at 0), use the volume event, since the band will really be at time -1
						if( ((pCurMIDITrack->m_lstTrackEvent->mtTime + pCurMIDITrack->m_lstTrackEvent->nOffset) != 0)
						||	(mtEarliestVol != 0)
						||	(lBandTime > 0) )
						{
							// Set the 'earliest volume' value to the note start
							mtEarliestVol = pCurMIDITrack->m_lstTrackEvent->mtTime + pCurMIDITrack->m_lstTrackEvent->nOffset;

							// Clear the track pointer
							pMIDITrackVolToUse = NULL;
						}
					}

					// If this pan is earlier than all others
					if( pCurMIDITrack->m_lstPan
					&&	pCurMIDITrack->m_lstPan->mtTime + pCurMIDITrack->m_lstPan->nOffset < mtEarliestPan )
					{
						// Save this track as the one to use
						pMIDITrackPanToUse = pCurMIDITrack;

						// Save the pan's time
						mtEarliestPan = pCurMIDITrack->m_lstPan->mtTime + pCurMIDITrack->m_lstPan->nOffset;
					}

					// If the first note event is before the first pan event
					if( pCurMIDITrack->m_lstTrackEvent
					&&	pCurMIDITrack->m_lstTrackEvent->mtTime + pCurMIDITrack->m_lstTrackEvent->nOffset <= mtEarliestPan )
					{
						// If the any of the note, pan, or band are not at exactly time 0, don't use this pan event
						// Otherwise (if all are at 0), use the pan event, since the band will really be at time -1
						if( ((pCurMIDITrack->m_lstTrackEvent->mtTime + pCurMIDITrack->m_lstTrackEvent->nOffset) != 0)
						||	(mtEarliestPan != 0)
						||	(lBandTime > 0) )
						{
							// Set the 'earliest pan' value to the note start
							mtEarliestPan = pCurMIDITrack->m_lstTrackEvent->mtTime + pCurMIDITrack->m_lstTrackEvent->nOffset;

							// Clear the track pointer
							pMIDITrackPanToUse = NULL;
						}
					}
				}

				// Go to the next track
				pCurMIDITrack = pCurMIDITrack->m_pNext;
			}

			// If we found a volume event to use, and it is within this band's time range
			if( pMIDITrackVolToUse
			&&	mtEarliestVol >= lBandTime
			&&	mtEarliestVol < endTime )
			{
				// Remove the event from the track's list
				FullSeqEvent *pVol = pMIDITrackVolToUse->m_lstVol;
				pMIDITrackVolToUse->m_lstVol = pMIDITrackVolToUse->m_lstVol->pNext;
				pVol->pNext = NULL;

				// Add it to the controller array
				vol.Add( pVol );

				// Delete the event
				delete pVol;
			}

			// If we found a pan event to use, and it is within this band's time range
			if( pMIDITrackPanToUse
			&&	mtEarliestPan >= lBandTime
			&&	mtEarliestPan < endTime )
			{
				// Remove the event from the track's list
				FullSeqEvent *pPan = pMIDITrackPanToUse->m_lstPan;
				pMIDITrackPanToUse->m_lstPan = pMIDITrackPanToUse->m_lstPan->pNext;
				pPan->pNext = NULL;

				// Add it to the controller array
				pan.Add( pPan );

				// Delete the event
				delete pPan;
			}

			// Mark this channel as 'used'
			afUsedPChannel[nChannel] = TRUE;
		}
	}
}
	
HRESULT ListOfBands::Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	HRESULT hr;
	WriteListChunk	list(pRiffStream, DMUS_FOURCC_BANDS_LIST);
	hr = list.State();
	if(hr == S_OK)
	{
		int bandno = 1;
		BOOL afUsedPChannel[16];
		ZeroMemory( afUsedPChannel, sizeof(BOOL) * 16 );
		BandPatchArray lastpatch;
		// put all patch changes occurring simultaneously in same band
		DMUS_IO_PATCH_ITEM*	plist = pPatchList;
		while(SUCCEEDED(hr) && plist)
		{
			DMUS_IO_PATCH_ITEM*	phead = plist;
			DMUS_IO_PATCH_ITEM* ptemp;
			long endTime;
			long begTime=0;
			if(plist)
			{
				endTime = plist->lTime;
				do
				{
					lastpatch.Add(plist);
					endTime = plist->lTime;
					ptemp = plist;
					plist = plist->pNext;
				}while(plist && ( (plist->lTime - endTime) <= continuity) );
				ptemp->pNext = 0;	// sub list of concurrent patches ends here

				BandControllerArray vol(7);
				BandControllerArray pan(10);
				MergeVolPanIntoBand( pMIDITracks, lastpatch, afUsedPChannel, phead->lTime, endTime + continuity, vol, pan );

				begTime = endTime+1;

				BandList bandlist(phead, lastpatch, vol, pan, bandno++);
				hr = bandlist.Write(pRiffStream);

				List_Free(phead);
			}
		}
	}
	return hr;
	
}


HRESULT ListOfInst::Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	if(!pRiffStream)
		return E_INVALIDARG;

	WriteListChunk list(pRiffStream, DMUS_FOURCC_INSTRUMENTS_LIST);
	HRESULT hr = list.State();
	if(hr == S_OK)
	{
		for(int i = 0; i < 16; i++)
		{
			char v,p;
			DMUS_IO_PATCH_ITEM Patch;
			if(lastPatchArray.GetValue(i, &Patch, true, true))
			{
				vol.GetValue(i, v);
				pan.GetValue(i, p);
				InstList instlist(Patch, v, p);
				hr = instlist.Write(pRiffStream);
			}
		}
	}
	return hr;
}

HRESULT InstList::Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	if(!pRiffStream)
		return E_INVALIDARG;
	WriteListChunk list(pRiffStream, DMUS_FOURCC_INSTRUMENT_LIST);
	HRESULT hr = list.State();
	if(hr == S_OK)
	{
		InstChunk inst(patchitem, vol, pan);
		hr = inst.Write(pRiffStream);
	}
	return hr;
}

HRESULT BandList::Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	if(!pRiffStream)
		return E_INVALIDARG;

	WriteListChunk list(pRiffStream, DMUS_FOURCC_BAND_LIST);
	HRESULT hr = list.State();
	if(hr == S_OK)
	{
		BandHeader header(*pPatchList);
		hr = header.Write(pRiffStream);
		if(hr == S_OK)
		{
			WriteRiffChunk riff(pRiffStream, DMUS_FOURCC_BAND_FORM);
			hr = riff.State();
			if(hr == S_OK)
			{
				CString name, number;
				name.LoadString(IDS_BANDNAMETEMPLATE);
				if(!name.IsEmpty())
				{
					number.Format("%d", m_bandno);
					name += number;
					hr = WriteBandName(pRiffStream, name);
				}
				ListOfInst listinst(lastPatchArray, vol, pan);
				hr = listinst.Write(pRiffStream);
			}
		}
	}
	return hr;
}