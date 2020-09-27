#include "stdafx.h"
#include "templates.h"
#include "..\includes\DMUSProd.h"
#include "DLLJazzDataObject.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#include <dmusicf.h>
#pragma warning( pop )

#include "styleriff.h"
#include "MIDIFile.h"


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

///////////////////////////////////////////////////// Band Riff Objects
///



HRESULT ListOfBands::Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	HRESULT hr;
	WriteListChunk	list(pRiffStream, DMUS_FOURCC_BANDS_LIST);
	hr = list.State();
	if(hr == S_OK)
	{
		// put all patch changes occurring simultaneously in same band
		DMUS_IO_PATCH_ITEM*	plist = pPatchList;
		while(SUCCEEDED(hr) && plist)
		{
			DMUS_IO_PATCH_ITEM*	phead = plist;
			DMUS_IO_PATCH_ITEM* ptemp;
			while(plist->pNext && plist->lTime == plist->pNext->lTime)
			{
				plist = plist->pNext;
			}
			if(plist->pNext)
			{
				ptemp = plist->pNext;
				plist->pNext = 0;
				plist = ptemp;	// start here next time around
			}
			else
			{
				plist = 0;
			}
			BandList bandlist(phead);
			hr = bandlist.Write(pRiffStream);
			List_Free(phead);
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
		for(DMUS_IO_PATCH_ITEM* p = pPatchList; p && (hr == S_OK); p = p->pNext)
		{
			InstList instlist(*p);
			hr = instlist.Write(pRiffStream, lstVol ? lstVol[(p->byStatus & 0xF)] : NULL, lstPan ? lstPan[(p->byStatus & 0xF)] : NULL);
		}
	}
	return hr;
}

HRESULT InstList::Write(IDMUSProdRIFFStream* pRiffStream, FullSeqEvent *pVol, FullSeqEvent *pPan)
{
	ASSERT(pRiffStream);
	if(!pRiffStream)
		return E_INVALIDARG;
	WriteListChunk list(pRiffStream, DMUS_FOURCC_INSTRUMENT_LIST);
	HRESULT hr = list.State();
	if(hr == S_OK)
	{
		InstChunk inst(*this);
		if( pVol )
		{
			inst.bVolume = pVol->bByte2;
			inst.dwFlags |= DMUS_IO_INST_VOLUME;
		}
		if( pPan )
		{
			inst.bPan = pPan->bByte2;
			inst.dwFlags |= DMUS_IO_INST_PAN;
		}
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
				ListOfInst listinst(pPatchList, NULL, NULL);
				hr = listinst.Write(pRiffStream);
			}
		}
	}
	return hr;
}