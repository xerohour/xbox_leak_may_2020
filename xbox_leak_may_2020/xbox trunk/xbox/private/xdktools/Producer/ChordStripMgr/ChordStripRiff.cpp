#include "stdafx.h"
#include "..\includes\DMUSProd.h"
#include "ChordIO.h"
#include "chord.h"
#include "DLLJazzDataObject.h"
#include <dmusici.h>
#include <dmusicf.h>
#include "resource.h"

#include "chordstripriff.h"

SubChordChunk::SubChordChunk(const DMChord* pChord)
{
	ZeroMemory(this, sizeof(SubChordChunk)); // Zeros out trailing padding bytes
	ASSERT(pChord);
	if(pChord)
	{
		dwChordPattern = pChord->ChordPattern();
		dwScalePattern = pChord->ScalePattern();
		dwInversionPoints = pChord->InvertPattern();
		dwLevels = pChord->Levels();
		bChordRoot = pChord->ChordRoot();
		bScaleRoot = pChord->ScaleRoot();
	}
}

HRESULT SubChordChunk::Write(IStream* pIStream)
{
	HRESULT hr=S_OK;
	DWORD dwBytesWritten;
//	WriteChunk chunk(pRiffStream, DMUS_FOURCC_CHORDTRACK_CHORD);
//	hr = chunk.State();
	if(  hr  == S_OK )
	{
		hr = pIStream->Write( this, sizeof(DMUS_IO_SUBCHORD), &dwBytesWritten) == 0
			&& dwBytesWritten == sizeof(DMUS_IO_SUBCHORD) ? S_OK : E_FAIL;
	}
    return hr;
}

HRESULT SubChordChunk::Read(IStream* pIStream, DWORD dwSubChordSize)
{
	ASSERT(pIStream);
	HRESULT hr;
	DWORD dwBytesRead, dwBytesToRead;
	dwBytesToRead = min( sizeof(DMUS_IO_SUBCHORD), dwSubChordSize );
	hr = pIStream->Read(this, dwBytesToRead, &dwBytesRead) == 0
		&& dwBytesRead == dwBytesToRead ? S_OK : E_FAIL;

	if( dwSubChordSize > sizeof(DMUS_IO_SUBCHORD)
	&&	SUCCEEDED(hr) )
	{
		StreamSeek( pIStream, dwSubChordSize - sizeof(DMUS_IO_SUBCHORD), SEEK_CUR );
	}
	/* uncomment to fix bug 17868 in Manbugs DB
	if(hr == S_OK)
	{
		bool bWarnUser = false;
		// make sure that root is correct and pattern is lowest available
		while(dwChordPattern && !(dwChordPattern & 1))
		{
			dwChordPattern = dwChordPattern >> 1;
			++bChordRoot;
			bWarnUser = true;
		}
		while(bChordRoot > 23)
		{
			bChordRoot -= 12;
			bWarnUser = true;
		}
		if(bWarnUser)
		{
			AfxMessageBox(IDS_ILLEGALCHORD);
		}
	}
	*/
	return hr;
}

HRESULT SubChordChunk::Extract(const DMChord* pChord)
{
	ASSERT(pChord);
	if(pChord)
	{
		dwChordPattern = pChord->ChordPattern();
		dwScalePattern = pChord->ScalePattern();
		dwInversionPoints = pChord->InvertPattern();
		dwLevels = pChord->Levels();
		bChordRoot = pChord->ChordRoot();
		bScaleRoot = pChord->ScaleRoot();
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT SubChordChunk::Insert(DMChord* pChord)
{
	ASSERT(pChord);
	if(pChord)
	{
		pChord->ChordPattern() = dwChordPattern;
		pChord->ScalePattern() = dwScalePattern;
		pChord->InvertPattern() = dwInversionPoints;
		pChord->Levels() = dwLevels;
		pChord->ChordRoot() = bChordRoot;
		pChord->ScaleRoot() = bScaleRoot;
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

/////////////////////////////////////////////// ChordChunk members

ChordChunk::ChordChunk(const CChordItem* pChord)
{
	ASSERT(pChord);
	if(pChord)
	{
		MultiByteToWideChar( CP_ACP, 0, const_cast<CChordItem*>(pChord)->Name(), -1, wszName, sizeof( wszName) / sizeof( wchar_t ) );
		mtTime = pChord->Time();
		wMeasure = pChord->Measure();
		bBeat = pChord->Beat();
		for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
		{
			m_subchords[i].Extract(const_cast<CChordItem*>(pChord)->SubChord(i));
		}
	}
}

HRESULT ChordChunk::Write(IDMUSProdRIFFStream* pRiffStream)
{
	HRESULT hr = S_OK;
//	WriteChunk chunk(pRiffStream, DMUS_FOURCC_CHORDTRACK_LIST);
//	hr = chunk.State();
	if(  hr  == 0 )
	{
		WriteDWord(pRiffStream, sizeof(DMUS_IO_CHORD));
		IStream* pIStream = pRiffStream->GetStream();
		ASSERT(pIStream != NULL);
		if(pIStream == NULL)
		{
			return E_FAIL;
		}
		hr = pIStream->Write(dynamic_cast<DMUS_IO_CHORD*>(this), sizeof(DMUS_IO_CHORD), 0);
		WriteDWord(pRiffStream, DMPolyChord::MAX_POLY);
		WriteDWord(pRiffStream, sizeof(DMUS_IO_SUBCHORD));
		for(int i = 0; i < DMPolyChord::MAX_POLY && hr == 0; i++)
		{
			hr = m_subchords[i].Write(pIStream);
		}
		pIStream->Release();
	}
    return hr;
}

HRESULT ChordChunk::Read(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	HRESULT hr;
	DWORD dwBytesRead;
	IStream* pIStream = pRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}
	DWORD chordsize;
	DWORD subchordsize;
	DWORD chordcount;
	hr = ReadDWord(pRiffStream, chordsize);
	ASSERT(chordsize == sizeof(DMUS_IO_CHORD));
	if(hr == 0)
	{
		hr = pIStream->Read(dynamic_cast<DMUS_IO_CHORD*>(this), chordsize, &dwBytesRead);
		hr = hr == S_OK && dwBytesRead == chordsize ? S_OK : E_FAIL;
	}
	if(hr == 0)
	{
		hr = ReadDWord(pRiffStream, chordcount);
	}
	if(hr == 0)
	{
		hr = ReadDWord(pRiffStream, subchordsize);
	}
	for(unsigned int i = 0; i < chordcount && hr == 0; i++)
	{
		if(i < DMPolyChord::MAX_POLY)
		{
			hr = m_subchords[i].Read(pIStream, subchordsize);
		}
	}

	pIStream->Release();
	return hr;
}

HRESULT ChordChunk::Extract(const CChordItem* pChord)
{
	ASSERT(pChord);
	if(pChord)
	{
		MultiByteToWideChar( CP_ACP, 0, const_cast<CChordItem*>(pChord)->Name(), -1, wszName, sizeof( wszName) / sizeof( wchar_t ) );
		mtTime = pChord->Time();
		wMeasure = pChord->Measure();
		bBeat = pChord->Beat();
		bFlags = pChord->Flags();
		for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
		{
			m_subchords[i].Extract(const_cast<CChordItem*>(pChord)->SubChord(i));
		}
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT ChordChunk::Insert(CChordItem* pChord)
{
	ASSERT(pChord);
	if(pChord)
	{
		memset(pChord->Name(), 0, DMPolyChord::MAX_NAME);
		WideCharToMultiByte( CP_ACP, 0, wszName, -1, pChord->Name(), DMPolyChord::MAX_NAME, NULL, NULL );
		pChord->Time() = mtTime;
		pChord->Measure() = wMeasure;
		pChord->Beat() = bBeat;
		pChord->Flags() = bFlags;
		for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
		{
			m_subchords[i].Insert(pChord->SubChord(i));
		}
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

/////////////////////////////////////////////// ChordDesignChunk members

ChordDesignChunk::ChordDesignChunk(const CChordItem* pChord)
{
	ASSERT(pChord);
	m_fFlatsNotSharps = FALSE;
	if(!pChord)
	{
		return;
	}
	if(pChord)
	{
		m_fFlatsNotSharps = const_cast<CChordItem*>(pChord)->SubChord(const_cast<CChordItem*>(pChord)->RootIndex())->UseFlat();
	}
}

HRESULT ChordDesignChunk::Write(IDMUSProdRIFFStream* pRiffStream)
{
	HRESULT hr = S_OK;
	if(  hr  == S_OK )
	{
		WriteDWord(pRiffStream, structSize);
		IStream* pIStream = pRiffStream->GetStream();
		ASSERT(pIStream != NULL);
		if(pIStream == NULL)
		{
			return E_FAIL;
		}
		hr = pIStream->Write(&m_fFlatsNotSharps, firstItemSize, 0);
		pIStream->Release();
	}
    return hr;
}

HRESULT ChordDesignChunk::Read(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	HRESULT hr;
	IStream* pIStream = pRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}
	DWORD infosize, dwBytesRead;
	hr = ReadDWord(pRiffStream, infosize);
	
	ASSERT(hr == S_OK);
	if(hr != S_OK)
	{
		pIStream->Release();
		return hr;
	}
	ASSERT(infosize >= firstItemSize);
	if(infosize < firstItemSize)
	{
		pIStream->Release();
		return E_UNEXPECTED;
	}

	hr = pIStream->Read(&m_fFlatsNotSharps, firstItemSize, &dwBytesRead);
	hr = (hr == S_OK && firstItemSize == dwBytesRead) ? S_OK : E_FAIL;
	pIStream->Release();
	return hr;
}

HRESULT ChordDesignChunk::Extract(const CChordItem* pChord)
{
	ASSERT(pChord);
	if(pChord)
	{
		m_fFlatsNotSharps = const_cast<CChordItem*>(pChord)->SubChord(const_cast<CChordItem*>(pChord)->RootIndex())->UseFlat();
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT ChordDesignChunk::Insert(CChordItem* pChord)
{
	ASSERT(pChord);
	if(pChord)
	{
		for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
		{
			pChord->SubChord(i)->UseFlat() = m_fFlatsNotSharps;
		}
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

////////////////////////////////// ChordListChunk members

HRESULT ChordListChunk::Write(IDMUSProdRIFFStream* pRiffStream)
{
	HRESULT hr;
	DWORD dwBytes;
	ASSERT(pRiffStream);
	if(pRiffStream == NULL)
	{
		return E_INVALIDARG;
	}


	WriteListChunk list(pRiffStream, DMUS_FOURCC_CHORDTRACK_LIST);
	hr = list.State();

	if(hr== S_OK)
	{
		IStream* pIStream = pRiffStream->GetStream();
		ASSERT(pIStream);
		if(!pIStream)
		{
			return E_FAIL;
		}
		WriteChunk chunk(pRiffStream, DMUS_FOURCC_CHORDTRACKHEADER_CHUNK);
		hr = chunk.State();
		if(hr == S_OK)
		{
			hr = pIStream->Write(&m_scale , sizeof(DWORD), &dwBytes);
			hr = dwBytes == sizeof(DWORD) ? hr : E_FAIL;
		}
		pIStream->Release();
	}


	for(CChordItem* pItem = m_plist->GetHead(); pItem && (hr == S_OK); pItem = pItem->GetNext())
	{
		{
			// need scope to destroy writechunk and complete ascension
			ChordChunk chordchunk;
			WriteChunk writechunk(pRiffStream, DMUS_FOURCC_CHORDTRACKBODY_CHUNK);
			hr = writechunk.State();

			if(hr == S_OK)
			{
				chordchunk.Extract(pItem);
				hr = chordchunk.Write(pRiffStream);
			}
		}
		if(m_designTime)
		{
			ChordDesignChunk designChunk;
			WriteChunk writechunk(pRiffStream, DMUSPROD_FOURCC_CHORDDESIGNINFO);
			hr = writechunk.State();
			if(hr == S_OK)
			{
				designChunk.Extract(pItem);
				hr = designChunk.Write(pRiffStream);
			}
		}
	}
	return hr;
}

HRESULT ChordListChunk::Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
{
	HRESULT hr = S_OK;
	ASSERT(pRiffStream && pckParent);
	MMCKINFO ck;

	ASSERT(m_plist);
	if(m_plist == NULL)
		return E_UNEXPECTED;

	DWORD dwBytes;

	CChordItem* pItem = 0;


	m_plist->ClearList();
	while(hr == S_OK && pRiffStream->Descend(&ck, pckParent, 0) == 0)
	{
		switch(ck.ckid)
		{
		case DMUS_FOURCC_CHORDTRACKHEADER_CHUNK:
			{
				IStream* pIStream = pRiffStream->GetStream();
				ASSERT(pIStream);
				if(!pIStream)
					hr = E_FAIL;
				else
				{
					hr = pIStream->Read(&m_scale, sizeof(DWORD), &dwBytes) == S_OK &&
							dwBytes == sizeof(DWORD) ? S_OK : E_FAIL;				
					pIStream->Release();
				}
			}
			break;
		case DMUS_FOURCC_CHORDTRACKBODY_CHUNK:
			{
				pItem = new CChordItem;
				ChordChunk chunk;
				hr = chunk.Read(pRiffStream);
				if(hr == S_OK)
				{
					chunk.Insert(pItem);
					m_plist->AddTail(pItem);
				}
				else
				{
					delete pItem;
				}
			}
			break;
		case DMUSPROD_FOURCC_CHORDDESIGNINFO:
			{
				// assumes that design info applies to the last read chorditem
				if(pItem)
				{
					ChordDesignChunk chunk;
					hr = chunk.Read(pRiffStream);
					if(hr == S_OK)
					{
						chunk.Insert(pItem);
					}
				}
			}
			break;
		default:
			// wrong chunk type
			hr = E_FAIL;
			break;
		}
		pRiffStream->Ascend(&ck, 0);
	}

	return hr;
}


