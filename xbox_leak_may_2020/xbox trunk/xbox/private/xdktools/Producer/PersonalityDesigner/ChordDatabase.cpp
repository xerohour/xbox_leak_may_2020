#include "stdafx.h"
#include <afxtempl.h>
#pragma warning(disable:4201)
#include "PersonalityDesigner.h"
#pragma warning(default:4201)
#include "StyleDesigner.h"
#include "Personality.h"
#include "RiffStructs.h"					// Definition of ioPersonality.
#include "ChordDatabase.h"

HRESULT DMLoadChordData(ChordEntryList& chordEntryList, IStream* pIStream)
{
	MMCKINFO		ck;
	HRESULT			hr=S_OK;

	DMSubChordSet subchords;
	bool bDMSubChordSetRead = false;
	bool bDMChordMapRead = false;

	IDMUSProdRIFFStream* pIRiffStream;;
	ASSERT( pIStream != NULL );

	hr = AllocRIFFStream(pIStream, &pIRiffStream);



	while(SUCCEEDED(hr) && pIRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
		case FOURCC_SUBCHORD:			
			hr = subchords.Read(pIRiffStream, &ck);
			bDMSubChordSetRead = true;
			if(FAILED(hr))
			{
				hr = E_FAIL;
				goto ON_ERR;
			}
			break;

		case FOURCC_CHORDENTRYEDIT:
			ASSERT(bDMChordMapRead);
			if(bDMChordMapRead)
			{
				hr = ReadChordEntryEditChunk(pIRiffStream, &chordEntryList, &ck);
			}
			break;

		case FOURCC_LIST:
			switch( ck.fccType )
			{

/*
			case FOURCC_LISTCHORDPALETTE:
				{
					ChordPaletteList chordpalette;
					if(!bDMSubChordSetRead)
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					hr = chordpalette.Read(pIRiffStream, &ck);
					if(FAILED(hr))
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					chordpalette.Insert(m_pPersonality->m_chordpalette, subchords);
				}
				break;
*/
			case FOURCC_LISTCHORDMAP:
				{
					ChordMapList chordmap;
 					if(!bDMSubChordSetRead)
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					hr = chordmap.Read(pIRiffStream, &ck);
					if(FAILED(hr))
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					chordmap.Insert(chordEntryList, subchords);
				}
				bDMChordMapRead = true;
				break;
/*
			case FOURCC_SIGNPOSTLIST:
				{
					SignPostListChunk signposts;
					if(!bDMSubChordSetRead)
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					hr = signposts.Read(pIRiffStream, &ck);
					if(FAILED(hr))
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					signposts.Insert(m_pPersonality->m_signpostlist, subchords);
				}
				break;
*/
			}
		}
        pIRiffStream->Ascend( &ck, 0 );
    }


ON_ERR:
    if ( pIRiffStream != NULL )
	{
		pIRiffStream->Release();
	}

    return hr;
}


/////////////////////////////////////////////// SignPostChunk members
//
SignPostChunk::SignPostChunk(const SignPost* pSignPost)
{
	dwFlags = pSignPost->m_dwflags;
	dwChords = pSignPost->m_chords;
}

HRESULT SignPostChunk::Write(IDMUSProdRIFFStream* pRiffStream)
{
	IStream* pIStream;
	HRESULT hr;
	DWORD dwBytesWritten;
	// Write SignPost chunk header
	WriteChunk chunk(pRiffStream, FOURCC_IOSIGNPOST);
	hr = chunk.State();
	if(  hr  == 0 )
	{
		pIStream = pRiffStream->GetStream();
		ASSERT( pIStream != NULL );

		// Write Signpost chunk data
		hr = pIStream->Write( this, sizeof(SignPostChunk), &dwBytesWritten) == 0
			&& dwBytesWritten == sizeof(ioSignPost) ? S_OK : E_FAIL;
	    pIStream->Release();
	}
    return hr;
}

HRESULT SignPostChunk::Read(IDMUSProdRIFFStream* pIRiffStream)
{
	ASSERT(pIRiffStream);
	HRESULT hr;
	DWORD dwBytesRead;
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}
	hr = pIStream->Read(this, sizeof(SignPostChunk), &dwBytesRead) == 0
		&& dwBytesRead == sizeof(ioSignPost) ? S_OK : E_FAIL;
	pIStream->Release();
	return hr;
}

HRESULT SignPostChunk::Extract(const SignPost* pSignPost)
{
	ASSERT(pSignPost);
	if(!pSignPost)
		return E_INVALIDARG;

	dwFlags = pSignPost->m_dwflags;
	dwChords = pSignPost->m_chords;

	return S_OK;
}

HRESULT  SignPostChunk::Insert(SignPost* pSignPost)
{
	ASSERT(pSignPost);
	if(!pSignPost)
		return E_INVALIDARG;

	pSignPost->m_dwflags = dwFlags;
	pSignPost->m_chords = dwChords;

	return S_OK;
}

//////////////////////////////////////////// NextChordChunk members
//
NextChordChunk::NextChordChunk(const NextChord* pNext)
{
	dwFlags = pNext->m_dwflags;
	nWeight = pNext->m_nweight;
	wMinBeats = pNext->m_nminbeats;
	wMaxBeats = pNext->m_nmaxbeats;
	wConnectionID = pNext->m_nid;
}

HRESULT NextChordChunk::Write(IDMUSProdRIFFStream* pRiffStream)
{
	IStream* pIStream;
	HRESULT hr = S_OK;
	DWORD dwBytesWritten;
	
	ASSERT(pRiffStream);

// Deliberitely commented out
//	WriteChunk chunk(pRiffStream, FOURCC_IONEXTCHORD);
//	hr = chunk.State();
	if( hr == S_OK )
	{

	    pIStream = pRiffStream->GetStream();
		ASSERT( pIStream != NULL );

		// Write NextChord chunk data
		hr = pIStream->Write( this, sizeof(NextChordChunk), &dwBytesWritten) == 0
			&& dwBytesWritten == sizeof(ioNextChord) ? S_OK : E_FAIL;

	    pIStream->Release();
	}

    return hr;
}

HRESULT NextChordChunk::Read(IDMUSProdRIFFStream* pIRiffStream)
{
	IStream* pIStream;
	HRESULT hr;
	DWORD dwBytesRead;
	
	ASSERT(pIRiffStream);

	pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	hr = pIStream->Read(this, sizeof(NextChordChunk), &dwBytesRead) == 0
		&& dwBytesRead == sizeof(ioNextChord) ? S_OK : E_FAIL;
	pIStream->Release();
	return hr;
}

HRESULT NextChordChunk::Extract(const NextChord* pNext)
{
	dwFlags = pNext->m_dwflags;
	nWeight = pNext->m_nweight;
	wMinBeats = pNext->m_nminbeats;
	wMaxBeats = pNext->m_nmaxbeats;
	wConnectionID = pNext->m_nid;
	return S_OK;
}

HRESULT NextChordChunk::Insert( NextChord* pNext)
{
	pNext->m_dwflags = dwFlags;
	pNext->m_nweight = nWeight;
	pNext->m_nminbeats = wMinBeats;
	pNext->m_nmaxbeats = wMaxBeats;
	pNext->m_nid = wConnectionID;
	return S_OK;
}

//////////////////////////////////////////////// ChordEntryChunk members
//
ChordEntryChunk::ChordEntryChunk(const ChordEntry* pChord)
{
	dwFlags = pChord->m_dwflags;
	wConnectionID = pChord->m_nid;
}

HRESULT ChordEntryChunk::Write(IDMUSProdRIFFStream* pRiffStream)
{
	IStream* pIStream;
	HRESULT hr;
	DWORD dwBytesWritten;

	ASSERT(pRiffStream);

	WriteChunk chunk(pRiffStream, FOURCC_CHORDENTRY);
	hr = chunk.State();

	if(hr == 0)
	{
		pIStream = pRiffStream->GetStream();
		ASSERT( pIStream != NULL );
		// Write ChordEntry chunk data
		hr = pIStream->Write( this, sizeof(ChordEntryChunk), &dwBytesWritten)
				&&  dwBytesWritten == sizeof(ioChordEntry);
		pIStream->Release();
	}

    return hr;
}

HRESULT ChordEntryChunk::Read(IDMUSProdRIFFStream* pIRiffStream)
{
	IStream* pIStream;
	HRESULT hr;
	DWORD dwBytesRead;

	ASSERT(pIRiffStream);
	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );
	hr = (pIStream->Read( this, sizeof(ChordEntryChunk), &dwBytesRead) == 0
		&&  dwBytesRead == sizeof(ioChordEntry)) ? S_OK : E_FAIL;
	pIStream->Release();

    return hr;
}

HRESULT ChordEntryChunk::Extract(const ChordEntry* pChord)
{
	dwFlags = pChord->m_dwflags;
	wConnectionID = pChord->m_nid;
	return S_OK;
}

HRESULT ChordEntryChunk::Insert( ChordEntry* pChord)
{
	pChord->m_dwflags = dwFlags;
	pChord->m_nid = wConnectionID;
	return S_OK;
}

///////////////////////////////////////////////// ChordEntryEdit helpers
//
HRESULT WriteChordEntryEditChunk(IDMUSProdRIFFStream* pRiffStream, ChordEntryList* plist)
{
	ASSERT(pRiffStream);
	ASSERT(plist);
	if(pRiffStream == NULL || plist == NULL)
	{
		return E_INVALIDARG;
	}
	if(plist->GetHead() == NULL)
	{
		// nothing to do
		return S_OK;
	}
	IStream* pStream = pRiffStream->GetStream();
	ASSERT(pStream);
	if(pStream == NULL)
	{
		return E_FAIL;
	}

	WriteChunk chunk(pRiffStream, FOURCC_CHORDENTRYEDIT);
	HRESULT hr = chunk.State();
	if(hr == S_OK)
	{
		DWORD size = sizeof(ChordEntryEditChunk);
		hr = pStream->Write(&size, sizeof(DWORD), 0);
		ChordEntry* pChord = plist->GetHead();
		while(pChord != NULL && hr == S_OK)
		{
			ChordEntryEditChunk chunk(pChord);
			chunk.Write(pRiffStream);
			pChord = pChord->GetNext();
		}
	}
	pStream->Release();
	return hr;
}

HRESULT ReadChordEntryEditChunk(IDMUSProdRIFFStream* pRiffStream, ChordEntryList* plist, MMCKINFO* pckInfo)
{
	ASSERT(pRiffStream);
	ASSERT(pckInfo);
	ASSERT(plist);
	if(pRiffStream == NULL || pckInfo == NULL || plist == NULL)
	{
		return E_INVALIDARG;
	}
	IStream* pStream = pRiffStream->GetStream();
	ASSERT(pStream);
	if(pStream == NULL)
	{
		return E_FAIL;
	}
	
	CMap<short,short, ChordEntryEditChunk, ChordEntryEditChunk&> chunkmap;
	DWORD size, cbSize;
	HRESULT hr = pStream->Read(&size, sizeof(DWORD), &cbSize);
	ASSERT(size == sizeof(ChordEntryEditChunk));
	DWORD count = (pckInfo->cksize - sizeof(DWORD)) / size;
	if(hr == S_OK && size == sizeof(ChordEntryEditChunk))
	{
		// read in edit chunks
		for(DWORD i = 0; hr == S_OK && i < count; i++)
		{
			ChordEntryEditChunk chunk;
			hr = chunk.Read(pRiffStream);
			chunkmap.SetAt(chunk.wConnectionId, chunk);
		}
	}
	ASSERT(hr == S_OK);
	if(hr == S_OK)
	{
		// assign positional coords to chordentries
		for(ChordEntry* pChord = plist->GetHead(); pChord; pChord = pChord->GetNext())
		{
			ChordEntryEditChunk chunk;
			if(chunkmap.Lookup(pChord->m_nid, chunk))
			{
				chunk.Insert(pChord);
			}
			else
			{
				hr = E_FAIL;
				break;
			}
		}
	}
	pStream->Release();
	return hr;
}

//////////////////////////////////////////////// ChordEntryEditChunk members
//
ChordEntryEditChunk::ChordEntryEditChunk(const ChordEntry* pChord)
{
	ChordSelection* psel = &(const_cast<ChordEntry*>(pChord)->m_chordsel);

	for(int i = 0; i < MaxSubChords; i++)
	{
		bBits[i] = psel->SubChord(i)->Bits();
		bUseFlat[i] = psel->SubChord(i)->UseFlat();
	}
	wMeasure = psel->Measure();
	bBeat = psel->Beat();
	wConnectionId = pChord->m_nid;
	left = pChord->m_rect.left;
	top = pChord->m_rect.top;
	right = pChord->m_rect.right;
	bottom = pChord->m_rect.bottom;
	page = pChord->m_page;
	if(psel->KeyDown())
	{
		page |= CHORD_KEYDOWN;
	}
	else
	{
		page &= ~CHORD_KEYDOWN;
	}
}

HRESULT ChordEntryEditChunk::Write(IDMUSProdRIFFStream* pRiffStream)
{
	IStream* pIStream;
	HRESULT hr = S_OK;
	DWORD dwBytesWritten;

	ASSERT(pRiffStream);

// deliberately commented out
//	WriteChunk chunk(pRiffStream, FOURCC_CHORDENTRYEDIT);
//	hr = chunk.State();

	if(hr == 0)
	{
		pIStream = pRiffStream->GetStream();
		ASSERT( pIStream != NULL );
		// Write ChordEntryEdit chunk data
		hr = pIStream->Write( this, sizeof(ChordEntryEditChunk), &dwBytesWritten)
				&&  dwBytesWritten == sizeof(ioChordEntryEdit);
		pIStream->Release();
	}

    return hr;
}

HRESULT ChordEntryEditChunk::Read(IDMUSProdRIFFStream* pIRiffStream)
{
	IStream* pIStream;
	HRESULT hr;
	DWORD dwBytesRead;

	ASSERT(pIRiffStream);
	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );
	hr = (pIStream->Read( this, sizeof(ChordEntryEditChunk), &dwBytesRead) == 0
		&&  dwBytesRead == sizeof(ioChordEntryEdit)) ? S_OK : E_FAIL;
	pIStream->Release();

    return hr;
}

HRESULT ChordEntryEditChunk::Extract(const ChordEntry* pChord)
{
	ChordSelection* psel = &(const_cast<ChordEntry*>(pChord)->m_chordsel);

	for(int i = 0; i < MaxSubChords; i++)
	{
		bBits[i] = psel->SubChord(i)->Bits();
		bUseFlat[i] = psel->SubChord(i)->UseFlat();
	}
	wMeasure = psel->Measure();
	bBeat = psel->Beat();
	wConnectionId = pChord->m_nid;
	left = pChord->m_rect.left;
	top = pChord->m_rect.top;
	right = pChord->m_rect.right;
	bottom = pChord->m_rect.bottom;
	page = pChord->m_page;
	if(psel->KeyDown())
	{
		page |= CHORD_KEYDOWN;
	}
	else
	{
		page &= ~CHORD_KEYDOWN;
	}
	return S_OK;
}

HRESULT ChordEntryEditChunk::Insert( ChordEntry* pChord)
{
	ChordSelection* psel = &((pChord)->m_chordsel);
	if(page & CHORD_KEYDOWN)
	{
		psel->KeyDown() = true;
		page &= ~ CHORD_KEYDOWN;
	}
	for(int i = 0; i < MaxSubChords; i++)
	{
		psel->SubChord(i)->Bits() = bBits[i];
		psel->SubChord(i)->UseFlat() = bUseFlat[i];
	}
	pChord->m_chordsel.Measure() = wMeasure;
	pChord->m_chordsel.Beat() = bBeat;
	pChord->m_nid = wConnectionId;
	pChord->m_rect.left = left;
	pChord->m_rect.top = top;
	pChord->m_rect.right = right;
	pChord->m_rect.bottom = bottom;
	pChord->m_page = page;
	return S_OK;
}

////////////////////////////////////////////// ChordListChunk members
//
ChordListChunk::ChordListChunk(DMPolyChord& Chord, DMSubChordSet& chordset, bool bIncludeEditInfo)
{
	memset(name, 0, sizeof(name));
	m_bIncludeEditInfo = bIncludeEditInfo;
	if(m_bIncludeEditInfo)
	{
		m_chordedit.Extract(Chord);
	}
	chordset.Add(Chord, *this);
#ifdef _UNICODE
	// use straight wstrcpy
	_wstrncpy(name, Chord.Name(), 12);
#else
	// use conversion
    MultiByteToWideChar( CP_ACP, 0, Chord.Name(), -1, name, sizeof( name) / sizeof( wchar_t ) );
#endif
}

HRESULT ChordListChunk::Write(IDMUSProdRIFFStream* pIRiffStream)
{
	IStream* pIStream;
	HRESULT hr;
	DWORD dwBytesWritten;

	// Write ChordList chunk header
	WriteListChunk	list(pIRiffStream, FOURCC_LISTCHORD);
	hr = list.State();

	if(hr == 0)
	{
	    pIStream = pIRiffStream->GetStream();
		ASSERT( pIStream != NULL );
		hr = (pIStream != 0) ? S_OK : E_FAIL;

		if(hr == 0)
		{
			WriteChunk* pchunk = new WriteChunk(pIRiffStream, FOURCC_CHORDNAME);
			hr = pchunk->State();
			if(hr == 0)
			{
				hr = pIStream->Write(name, sizeof(name), &dwBytesWritten) == 0 
					&& dwBytesWritten == sizeof(name) ? S_OK : E_FAIL;
			}
			delete pchunk;
		}

		if(hr == 0)
		{
			WriteChunk* pchunk = new WriteChunk(pIRiffStream, FOURCC_SUBCHORDID);
			hr = pchunk->State();
			hr = pIStream->Write( subchordids, sizeof(subchordids), &dwBytesWritten) == 0
				&&  dwBytesWritten == sizeof(subchordids) ? S_OK : E_FAIL;
			delete pchunk;
		}

		if(hr == 0)
		{
			WriteChunk* pchunk = new WriteChunk(pIRiffStream, FOURCC_CHORDEDIT);
			hr = pchunk->State();
			if(hr == 0)
			{
				hr = pIStream->Write(&m_chordedit, sizeof (m_chordedit), &dwBytesWritten);
				hr = (hr == S_OK) && (dwBytesWritten == sizeof(m_chordedit)) ? S_OK : E_FAIL;
			}
			delete pchunk;
		}

	    pIStream->Release();
	}

    return hr;
}

HRESULT ChordListChunk::Read(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent)
{
	HRESULT hr1=S_OK, hr2=S_OK;
	HRESULT hr3 = S_OK;
	ASSERT(pIRiffStream && pckParent);
	MMCKINFO ck;
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(!pIStream)
		return E_FAIL;

	while(pIRiffStream->Descend(&ck, pckParent, 0) == 0)
	{
		switch(ck.ckid)
		{
		case FOURCC_CHORDNAME:
			hr1 = pIStream->Read(name, sizeof(name), 0);
			break;
		case FOURCC_SUBCHORDID:
			hr2 = pIStream->Read(subchordids, sizeof(subchordids), 0);
			break;
		case FOURCC_CHORDEDIT:
			hr3 = pIStream->Read(&m_chordedit, sizeof(m_chordedit), 0);
			m_bIncludeEditInfo = true;
			break;
		}
		pIRiffStream->Ascend(&ck, 0);
	}
	pIStream->Release();
	return (hr1 == S_OK && hr2 == S_OK) ? S_OK : E_FAIL;
}

HRESULT ChordListChunk::Extract( DMPolyChord& Chord, DMSubChordSet& subchords, bool bIncludeEditInfo)
{
	m_bIncludeEditInfo = bIncludeEditInfo;
	if(m_bIncludeEditInfo)
	{
		m_chordedit.Extract(Chord);
	}
	m_bIncludeEditInfo = bIncludeEditInfo;
	subchords.Add(Chord, *this);
#ifdef _UNICODE
	// use straight wstrcpy
	_wstrncpy(name, Chord.Name(), NameSize);
#else
	// use conversion
    MultiByteToWideChar( CP_ACP, 0, Chord.Name(), -1, name, sizeof( name) / sizeof( wchar_t ) );
#endif
	return S_OK;
}

HRESULT ChordListChunk::Insert( DMPolyChord& Chord,  const DMSubChordSet& subchords)
{
	memset(Chord.Name(), 0, NameSize);
#ifdef _UNICODE
	// use straight wstrcpy
	_wstrcpy(Chord.Name(), name);
#else
	// use conversion
	WideCharToMultiByte( CP_ACP, 0, name, -1, Chord.Name(), NameSize, NULL, NULL );
#endif
	if(m_bIncludeEditInfo)
	{
		m_chordedit.Insert(Chord);
	}
	return const_cast<DMSubChordSet&>(subchords).Retrieve(Chord, *this);
}

//////////////////////////////////////////////////////////////////// ChordEntryListItem members
//

ChordEntryListItem::ChordEntryListItem(ChordEntry* pChord, DMSubChordSet& subchords, bool bIncludeEditInfo)
: m_bIncludeEditInfo(bIncludeEditInfo)
{
	ASSERT(pChord);
	m_chordentry.Extract(pChord);
	m_chordlist.Extract(pChord->m_chordsel, subchords, m_bIncludeEditInfo);
	for(NextChord* pNext = pChord->m_nextchordlist.GetHead(); pNext; pNext = pNext->GetNext())
	{
		NextChordChunk nextchunk(pNext);
		m_nextchordlist.AddTail(nextchunk);
	}
}

HRESULT ChordEntryListItem::Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	WriteListChunk list(pRiffStream, FOURCC_LISTCHORDENTRY);
	HRESULT hr = list.State();
	if(hr == 0)
	{
		hr = m_chordentry.Write(pRiffStream) == 0
			&& m_chordlist.Write(pRiffStream) == 0 ? S_OK : E_FAIL;
		if(hr == 0)
		{
			WriteChunk chunk(pRiffStream, FOURCC_NEXTCHORDSEQ);
			hr = chunk.State();
			WORD cbioNextChord = sizeof(ioNextChord);
			WriteWord(pRiffStream, cbioNextChord);
			POSITION pos = m_nextchordlist.GetHeadPosition();
			while(pos && hr == 0)
			{
				NextChordChunk nextchunk;
				nextchunk = m_nextchordlist.GetNext(pos);
				hr = nextchunk.Write(pRiffStream);
			}
		}

	}
	return hr;
}

HRESULT ChordEntryListItem::Read(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent)
{
	ASSERT(pIRiffStream);
	HRESULT hr1=S_OK,hr2=S_OK,hr3=S_OK;
	ASSERT(pIRiffStream && pckParent);
	MMCKINFO ck;

	m_nextchordlist.RemoveAll();

	while(pIRiffStream->Descend(&ck, pckParent, 0)==0)
	{
		switch(ck.ckid)
		{
		case FOURCC_CHORDENTRY:
			hr1 = m_chordentry.Read(pIRiffStream);
			break;
		case FOURCC_LIST:
			switch(ck.fccType)
			{
			case FOURCC_LISTCHORD:
				hr2 = m_chordlist.Read(pIRiffStream, &ck);
				break;
			}
			break;
			case FOURCC_NEXTCHORDSEQ:
				{
					WORD count;
					if(ReadWord(pIRiffStream, count) == 0)
					{
						count = static_cast<short>((ck.cksize-sizeof(WORD)) / count);
						hr3 = hr2;
						for(WORD i = 0; i < count && hr3 == S_OK; i++)
						{
							NextChordChunk nextchunk;
							hr3 = nextchunk.Read(pIRiffStream);
							m_nextchordlist.AddTail(nextchunk);
						}
					}
				}
				break;
		}
		pIRiffStream->Ascend(&ck, 0);
	}
	return (hr1 || hr2 || hr3) == 0 ? S_OK : E_FAIL;
}

HRESULT	ChordEntryListItem::Extract(ChordEntry* pChord, DMSubChordSet& subchords, bool bIncludeEditInfo)
{
	ASSERT(pChord);
	m_bIncludeEditInfo = bIncludeEditInfo;
	HRESULT hr = m_chordentry.Extract(pChord)
					|| m_chordlist.Extract(pChord->m_chordsel, subchords, m_bIncludeEditInfo);
	if(hr == 0)
	{
		for(NextChord* pNext = pChord->m_nextchordlist.GetHead(); pNext; pNext = pNext->GetNext())
		{
			NextChordChunk nextchunk;
			hr |= nextchunk.Extract(pNext);
			m_nextchordlist.AddTail(nextchunk);
		}
	}
	return hr;
}

HRESULT ChordEntryListItem::Insert( ChordEntry* pChord, const DMSubChordSet& subchords)
{
	ASSERT(pChord);
	HRESULT hr = m_chordentry.Insert(pChord)
					|| m_chordlist.Insert(pChord->m_chordsel, subchords);

	if(hr == 0)
	{
		POSITION pos = m_nextchordlist.GetHeadPosition();
		while(pos)
		{
			NextChordChunk nextchunk;
			nextchunk = m_nextchordlist.GetNext(pos);
			NextChord* pNext = new NextChord;
			nextchunk.Insert(pNext);
			pNext->m_parent = pChord;
			pChord->m_nextchordlist.AddTail(pNext);
		}			
	}
	
	return hr;
}


//////////////////////////////////////////////////////////////////// CadenceListChunk members
//
CadenceListChunk::CadenceListChunk(SignPost* pSignPost, DMSubChordSet& subchords, bool bIncludeEditInfo)
: m_bIncludeEditInfo(bIncludeEditInfo)
{
	m_bHasCadenceChord1 = (pSignPost->m_dwflags & SPOST_CADENCE1);
	m_bHasCadenceChord2 = (pSignPost->m_dwflags & SPOST_CADENCE2);

	if(m_bHasCadenceChord1)
	{
		m_CadenceChord1.Extract(pSignPost->m_cadence[0],  subchords, m_bIncludeEditInfo);
	}

	if(m_bHasCadenceChord2)
	{
		m_CadenceChord1.Extract(pSignPost->m_cadence[1],  subchords, m_bIncludeEditInfo);
	}
}



HRESULT CadenceListChunk::Write(IDMUSProdRIFFStream* pRiffStream)
{
	HRESULT hr = S_OK;

	// Write ChordList chunk header
	if(m_bHasCadenceChord1 || m_bHasCadenceChord2)
	{
		WriteListChunk	list(pRiffStream, FOURCC_LISTCADENCE);
		hr = list.State();

		if(hr == 0)
		{
			// Write Cadence1 chordlist chunk data
			if(m_bHasCadenceChord1)
			{
				hr = m_CadenceChord1.Write(pRiffStream);
			}


			// Write Cadence2 chordlist chunk data
			if(hr == 0 && m_bHasCadenceChord2)
			{
				hr = m_CadenceChord2.Write(pRiffStream);
			}
		}
	}
    return hr;
}

HRESULT CadenceListChunk::Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
// for this to work properly, user must first read in signpost chunk to get which cadence chords,
// otherwise first chord read goes to cadence chord 1 etc.
{
	ASSERT(pRiffStream && pckParent);
	MMCKINFO ck;
	HRESULT hr = S_OK;
	int nextToRead  = 1;
	if(m_bHasCadenceChord2 && !m_bHasCadenceChord1)
	{
		nextToRead = 2;
	}

	while(pRiffStream->Descend(&ck, pckParent, 0) == 0 && hr == S_OK)
	{
		switch(nextToRead)
		{
		case 1:
			hr = m_CadenceChord1.Read(pRiffStream, &ck);
			nextToRead = 2;
			break;
		case 2:
			hr = m_CadenceChord2.Read(pRiffStream, &ck);
			nextToRead = 1;
			break;
		}
		pRiffStream->Ascend(&ck, 0);
	}
	return hr;
}

HRESULT CadenceListChunk::Extract( SignPost* pSignPost, DMSubChordSet& subchords, bool bIncludeEditInfo)
{
	m_bIncludeEditInfo = bIncludeEditInfo;
	m_bHasCadenceChord1 = (pSignPost->m_dwflags & SPOST_CADENCE1);
	m_bHasCadenceChord2 = (pSignPost->m_dwflags & SPOST_CADENCE2);

	if(m_bHasCadenceChord1)
	{
		m_CadenceChord1.Extract(pSignPost->m_cadence[0],  subchords, m_bIncludeEditInfo);
	}

	if(m_bHasCadenceChord2)
	{
		m_CadenceChord2.Extract(pSignPost->m_cadence[1],  subchords, m_bIncludeEditInfo);
	}
	return S_OK;
}

HRESULT CadenceListChunk::Insert( SignPost* pSignPost, const DMSubChordSet& subchords)
// pSignpost should have been filled in from SignPostChunk before calling this function
{
	ASSERT(pSignPost);
	m_bHasCadenceChord1 = (pSignPost->m_dwflags & SPOST_CADENCE1);
	m_bHasCadenceChord2 = (pSignPost->m_dwflags & SPOST_CADENCE2);

	if(m_bHasCadenceChord1)
	{
		m_CadenceChord1.Insert(pSignPost->m_cadence[0],  subchords);
	}

	if(m_bHasCadenceChord2)
	{
		m_CadenceChord2.Insert(pSignPost->m_cadence[1],  subchords);
	}
	return S_OK;
}

////////////////////////////////////////////////////////// SignPostListItemChunk members
//
SignPostListItemChunk::SignPostListItemChunk(SignPost* pSignPost, DMSubChordSet& subchords, bool bIncludeEditInfo)
: m_bIncludeEditInfo(bIncludeEditInfo)
{
	m_signpost.Extract(pSignPost);
	m_signpostchord.Extract(pSignPost->m_chord, subchords, m_bIncludeEditInfo);
	m_cadencelist.Extract(pSignPost, subchords, m_bIncludeEditInfo);
};

HRESULT SignPostListItemChunk::Write(IDMUSProdRIFFStream* pRiffStream)
{
	HRESULT hr;

	ASSERT(pRiffStream);
	if(pRiffStream == 0)
	{
		return E_INVALIDARG;
	}

	WriteListChunk list(pRiffStream, FOURCC_LISTSIGNPOSTITEM);
	hr = list.State();

	if(hr == S_OK)
	{
		hr =  m_signpost.Write(pRiffStream)
				||m_signpostchord.Write(pRiffStream)
				|| m_cadencelist.Write(pRiffStream);
	}

	return hr;
}

HRESULT SignPostListItemChunk::Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
{
	ASSERT(pRiffStream);
	if(pRiffStream == 0)
	{
		return E_INVALIDARG;
	}

	MMCKINFO ck;
	HRESULT hr = S_OK;
	while(pRiffStream->Descend(&ck, pckParent, 0) == 0 && hr == S_OK)
	{
		switch(ck.ckid)
		{
		case FOURCC_IOSIGNPOST:
			hr = m_signpost.Read(pRiffStream);
			m_cadencelist.HasCadenceChord1() = m_signpost.dwFlags & SPOST_CADENCE1;
			m_cadencelist.HasCadenceChord2() = m_signpost.dwFlags & SPOST_CADENCE2;
			break;
		case FOURCC_LIST:
			switch(ck.fccType)
			{
			case FOURCC_LISTCHORD:
				hr = m_signpostchord.Read(pRiffStream, &ck);
				break;
			case FOURCC_LISTCADENCE:
				hr = m_cadencelist.Read(pRiffStream, &ck);
			break;
			}
		}
		pRiffStream->Ascend(&ck, 0);
	}

	return hr;
}

HRESULT SignPostListItemChunk::Extract(SignPost* pSignPost, DMSubChordSet& subchords, bool bIncludeEditInfo)
{
	m_bIncludeEditInfo = bIncludeEditInfo;
	HRESULT hr;
	ASSERT(pSignPost);
	if(pSignPost == 0)
	{
		return E_INVALIDARG;
	}

	hr =  m_signpost.Extract(pSignPost)
			|| m_signpostchord.Extract(pSignPost->m_chord, subchords, m_bIncludeEditInfo)
			|| m_cadencelist.Extract(pSignPost, subchords, m_bIncludeEditInfo);

	return hr;
}

HRESULT SignPostListItemChunk::Insert( SignPost* pSignPost, const DMSubChordSet& subchords)
{
	HRESULT hr;
	ASSERT(pSignPost);
	if(pSignPost == 0)
	{
		return E_INVALIDARG;
	}

	hr =	m_signpost.Insert(pSignPost)
			|| m_signpostchord.Insert(pSignPost->m_chord, subchords)
			|| m_cadencelist.Insert(pSignPost, subchords);

	return hr;
}


///////////////////////////////////////////////////////// SignPostListChunk members
//
SignPostListChunk::SignPostListChunk(SignPostList& signpostlist, DMSubChordSet& subchords, bool bIncludeEditInfo)
{
	m_bIncludeEditInfo = bIncludeEditInfo;
	for(SignPost* pSignPost = signpostlist.GetHead(); pSignPost; pSignPost = pSignPost->GetNext())
	{
		SignPostListItemChunk* pItem = new SignPostListItemChunk(pSignPost, subchords, m_bIncludeEditInfo);
		AddTail(pItem);
	}
}

SignPostListChunk::~SignPostListChunk()
{
	while(!IsEmpty())
	{
		SignPostListItemChunk* pItem = RemoveHead();
		delete pItem;
	}
}

HRESULT	SignPostListChunk::Write(IDMUSProdRIFFStream* pRiffStream)
{
	HRESULT hr = S_OK;
	POSITION pos = GetHeadPosition();
	if(pos != NULL)
	{
		WriteListChunk	list(pRiffStream, FOURCC_SIGNPOSTLIST);
		hr = list.State();
		while(pos != NULL && hr==S_OK)
		{
			SignPostListItemChunk* pItem = GetNext(pos);
			hr = pItem->Write(pRiffStream);
		}
	}
	return hr;
}

HRESULT SignPostListChunk::Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
{
	ASSERT(pRiffStream);
	if(pRiffStream == 0)
	{
		return E_INVALIDARG;
	}

	MMCKINFO ck;
	HRESULT hr = S_OK;
	while(pRiffStream->Descend(&ck, pckParent, 0) == 0 && hr == S_OK)
	{
		switch(ck.ckid)
		{
		case FOURCC_LIST:
			switch(ck.fccType)
			{
			case FOURCC_LISTSIGNPOSTITEM:
				{
					SignPostListItemChunk* pchunk = new SignPostListItemChunk;
					hr = pchunk->Read(pRiffStream, &ck);
					if(hr == S_OK)
					{
						AddTail(pchunk);
					}
					break;
				}
			break;
			}
		}
		pRiffStream->Ascend(&ck, 0);
	}

	return hr;
}

HRESULT SignPostListChunk::Extract(SignPostList& signpostlist, DMSubChordSet& subchords, bool bIncludeEditInfo)
{
	m_bIncludeEditInfo = bIncludeEditInfo;
	for(SignPost* pSignPost = signpostlist.GetHead(); pSignPost; pSignPost = pSignPost->GetNext())
	{
		SignPostListItemChunk* pItem = new SignPostListItemChunk(pSignPost, subchords, m_bIncludeEditInfo);
		AddTail(pItem);
	}
	return S_OK;
}

HRESULT SignPostListChunk::Insert(SignPostList& signpostlist, const DMSubChordSet& subchords)
{
	HRESULT hr = S_OK;
	POSITION pos = GetHeadPosition();
	while(pos != NULL && hr==S_OK)
	{
		SignPostListItemChunk* pItem = GetNext(pos);
		SignPost* pSignPost = new SignPost;
		if(pSignPost)
		{
			hr = pItem->Insert(pSignPost, subchords);
			signpostlist.AddTail(pSignPost);
		}
		else
		{
			hr = E_FAIL;
		}
	}
	return hr;
}

//////////////////////////////////////////////////////// ChordPaletteList members
//
ChordPaletteList::ChordPaletteList(ChordPalette& chordpalette, DMSubChordSet& subchords)
{
	for(int i = 0; i < 24; i++)
	{
		m_chordlist[i].Extract(chordpalette.m_chords[i], subchords, false);
	}
}

HRESULT ChordPaletteList::Write(IDMUSProdRIFFStream* pRiffStream)
{
	HRESULT hr;

	ASSERT(pRiffStream);
	if(pRiffStream == 0)
	{
		return E_INVALIDARG;
	}

	WriteListChunk list(pRiffStream, FOURCC_LISTCHORDPALETTE);
	hr = list.State();

	for(int i = 0; i < 24 && hr == S_OK; i++)
	{
		hr = m_chordlist[i].Write(pRiffStream);
	}
	return hr;
}

HRESULT ChordPaletteList::Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO*pckParent)
{
	ASSERT(pRiffStream);
	if(pRiffStream == 0)
	{
		return E_INVALIDARG;
	}

	MMCKINFO ck;
	HRESULT hr = S_OK;
	WORD count=0;
	while(pRiffStream->Descend(&ck, pckParent, 0) == 0 && hr == S_OK)
	{
		switch(ck.ckid)
		{
		case FOURCC_LIST:
			switch(ck.fccType)
			{
			case FOURCC_LISTCHORD:
				{
					hr = m_chordlist[count++].Read(pRiffStream, &ck);
					break;
				}
			break;
			}
		}
		pRiffStream->Ascend(&ck, 0);
	}
	ASSERT(count == 24);
	return hr;
}

HRESULT ChordPaletteList::Extract(ChordPalette& chordpalette, DMSubChordSet& subchords)
{
	HRESULT hr = S_OK;
	for(int i = 0; i < 24 && hr == S_OK; i++)
	{
		hr = m_chordlist[i].Extract(chordpalette.m_chords[i], subchords, false);
	}

	return hr;
}

HRESULT ChordPaletteList::Insert(ChordPalette& chordpalette,  const DMSubChordSet& subchords)
{
	HRESULT hr = S_OK;
	for(int i = 0; i < 24 && hr == S_OK; i++)
	{
		hr = m_chordlist[i].Insert(chordpalette.m_chords[i], subchords);
	}

	return hr;
}

//////////////////////////////////////////////////////// DMChordMapList 
//
ChordMapList::ChordMapList(ChordEntryList& chordlist, DMSubChordSet& subchords, bool bIncludeEditInfo)
{
	m_bIncludeEditInfo = bIncludeEditInfo;
	int idx = 0;
	for(ChordEntry* pchord = chordlist.GetHead(); pchord; pchord = pchord->GetNext())
	{
			ChordEntryListItem*	pItem = new ChordEntryListItem(pchord, subchords, m_bIncludeEditInfo);
			SetAt(idx++, pItem);	// does a copy
	}
}

ChordMapList::~ChordMapList()
{
	POSITION pos = GetStartPosition();
	while(pos != NULL)
	{
		int key;
		ChordEntryListItem* pItem;
		GetNextAssoc(pos, key, pItem);
		delete pItem;
		RemoveKey(key);
	}
}

HRESULT ChordMapList::Write(IDMUSProdRIFFStream* pRiffStream)
{
	POSITION pos = GetStartPosition();
	HRESULT hr = S_OK;

	WriteListChunk list(pRiffStream, FOURCC_LISTCHORDMAP);
	hr = list.State();
	
	while(hr == S_OK && pos != NULL)
	{
		int key;
		ChordEntryListItem* pItem;
		GetNextAssoc(pos, key, pItem);
		hr = pItem->Write(pRiffStream);
	}
	return hr;
}

HRESULT ChordMapList::Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
{
	ASSERT(pRiffStream);
	if(pRiffStream == 0)
	{
		return E_INVALIDARG;
	}

	MMCKINFO ck;
	HRESULT hr = S_OK;
	WORD count = 0;
	while(pRiffStream->Descend(&ck, pckParent, 0) == 0&& hr == S_OK)
	{
		switch(ck.ckid)
		{
		case FOURCC_LIST:
			switch(ck.fccType)
			{
			case FOURCC_LISTCHORDENTRY:
				{
					ChordEntryListItem* item = new ChordEntryListItem;
					hr = item->Read(pRiffStream, &ck);
					if(hr == S_OK)
					{
						SetAt(count++, item);
					}
					else
					{
						delete item;
					}
					break;
				}
			break;
			}
		}
		pRiffStream->Ascend(&ck, 0);
	}

	return hr;
}

HRESULT ChordMapList::Extract( ChordEntryList& chordlist, DMSubChordSet& subchords, bool bIncludeEditInfo)
{
	HRESULT hr = S_OK;
	m_bIncludeEditInfo = bIncludeEditInfo;
	int idx = 0;
	for(ChordEntry* pchord = chordlist.GetHead(); pchord; pchord = pchord->GetNext())
	{
			ChordEntryListItem*	pItem = new ChordEntryListItem(pchord, subchords, m_bIncludeEditInfo);
			SetAt(idx++, pItem);	// does a copy
	}
	return hr;
}

HRESULT ChordMapList::Insert(ChordEntryList& chordlist, const DMSubChordSet& subchords)
{
	POSITION pos = GetStartPosition();
	HRESULT hr = S_OK;
	while(hr == S_OK && pos != NULL)
	{
		int key;
		ChordEntryListItem* pItem;
		ChordEntry* pchord = new ChordEntry;
		if(!pchord)
		{
			hr = E_FAIL;
		}
		else
		{
			GetNextAssoc(pos, key, pItem);
			hr = pItem->Insert(pchord, subchords);
			chordlist.AddTail(pchord);
		}
	}
	return hr;
	
}

//////////////////////////////////////////////////////// DMSubChordSet members
//
DMSubChordSet::~DMSubChordSet()
{
	if(m_aSubChord)
	{
		delete [] m_aSubChord;
		m_aSubChord = 0;
		m_cSubChord = 0;
	}

	POSITION pos = GetStartPosition();
	while(pos != NULL)
	{
		WORD idx;
		ioSubChord Item;
		GetNextAssoc(pos, Item, idx);
		RemoveKey(Item);
	}
}

HRESULT	DMSubChordSet::Add(DMPolyChord& Chord,  ChordListChunk& chordlist)
{

	int n = min(DMPolyChord::MAX_POLY, ChordListChunk::maxsubchords);

	for(int i = 0; i < n; i++)
	{
		ioSubChord	Sub(*Chord[i]);
		WORD id;
		if(!Lookup(Sub, id))
		{
			id = wNextKey++;
			SetAt(Sub, id);
		}
		chordlist.subchordids[i] = id;
	}
	return S_OK;
}

HRESULT DMSubChordSet::Retrieve( DMPolyChord& Chord, const ChordListChunk& chordlist) 
{
	int n = min(DMPolyChord::MAX_POLY, ChordListChunk::maxsubchords);

	HRESULT hr = S_OK;

	if(!m_aSubChord)
	{
		BuildSubChordArray();
	}
	ASSERT(m_cSubChord > 0);
	if(m_cSubChord == 0)
	{
		return E_FAIL;
	}

	for(int i = 0; i < n; i++)
	{
		WORD id = chordlist.subchordids[i];
		if(id < m_cSubChord)
		{
//			ioSubChord sub;
			m_aSubChord[id].CopyToDMChord(*Chord.SubChord(i));
			// make sure root in range
			if(i == Chord.RootIndex() && Chord.SubChord(i)->ChordRoot() > 23)
			{
				// make this chord unique
				while(Chord.SubChord(i)->ChordRoot() > 23)
				{
					Chord.SubChord(i)->ChordRoot() -= 12;
				}
				Chord.SubChord(i)->Flags() |= CSF_SHIFTED;	// make this chord unique
				AfxMessageBox(IDS_ILLEGALCHORD);
			}
		}
	}
	return hr;
}

HRESULT DMSubChordSet::Write(IDMUSProdRIFFStream* pRiffStream)
{
	ASSERT(pRiffStream);
	IStream* pStream;
	HRESULT hr;
	DWORD dwBytesWritten;
	if(!m_aSubChord)
	{
		BuildSubChordArray();
	}

	ASSERT(m_cSubChord > 0);
	if(m_cSubChord == 0)
	{
		return E_FAIL;
	}

	WriteChunk	chunk(pRiffStream, FOURCC_SUBCHORD);
	hr = chunk.State();

	if(hr == S_OK)
	{
		pStream = pRiffStream->GetStream();
		ASSERT(pStream);
		if(pStream == NULL)
		{
			return E_FAIL;
		}
		unsigned short cb = static_cast<unsigned short>(sizeof(ioSubChord));
		hr = pStream->Write(&cb, sizeof(unsigned short), &dwBytesWritten);
		if(hr == S_OK && dwBytesWritten == sizeof(unsigned short))
		{
			for(WORD i = 0; hr == S_OK && i < m_cSubChord; i++)
			{
				hr = pStream->Write(&m_aSubChord[i], sizeof(ioSubChord), &dwBytesWritten);
				if(dwBytesWritten != sizeof(ioSubChord))
				{
					hr = E_FAIL;
				}
			}
		}
		pStream->Release();
	}
	return hr;
}

HRESULT DMSubChordSet::Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pck)
{
	ASSERT(pRiffStream);
	HRESULT hr = S_OK;
	DWORD dwBytesRead;
	IStream* pIStream = pRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}
	WORD cbioSubChordSize;	
	hr = pIStream->Read(&cbioSubChordSize, sizeof(WORD), &dwBytesRead);
	ASSERT(cbioSubChordSize==sizeof(ioSubChord));
	WORD count = static_cast<WORD>((pck->cksize - sizeof(WORD)) / cbioSubChordSize);
	WORD key = 0;
	DWORD	cbRead = sizeof(WORD);	// already read size of ioSubChord

	if(cbioSubChordSize != sizeof(ioSubChord))
	{
		hr = E_FAIL;
		goto ON_ERR;
	}

	while(cbRead < pck->cksize && hr == S_OK)
	{
		ioSubChord sub;
		DWORD	cb;
		hr = pIStream->Read(&sub, cbioSubChordSize, &cb);
		if(cb != cbioSubChordSize)
		{
			hr = E_FAIL;
		}
		// make sure we got the lowest bit pattern in chords
		while(sub.dwChordPattern && !(sub.dwChordPattern & 0x1))
		{
			sub.dwChordPattern = sub.dwChordPattern >> 1;
			sub.bChordRoot++;
			sub.wCFlags |= CSF_SHIFTED;	// make this chord unique
		}
		// make sure root in range
		/*
		if(sub.bChordRoot > 23)
		{
			// make this chord unique
			while(sub.bChordRoot > 23)
			{
				sub.bChordRoot -= 12;
			}
			sub.wCFlags |= CSF_SHIFTED;	// make this chord unique
			AfxMessageBox(IDS_ILLEGALCHORD);
		}
*/
		WORD id;
		if(!Lookup(sub, id))
		{
			id = key++;
			SetAt(sub, id);
		}

		cbRead += cb;
	}

	ASSERT(count == key);
	if(count != key)
	{
		hr = E_FAIL;
	}
	else
	{
		m_cSubChord = count;
	}
ON_ERR:
	pIStream->Release();
	return hr;
}


void DMSubChordSet::BuildSubChordArray()
{
	if(m_aSubChord)
	{
		delete [] m_aSubChord;
		m_aSubChord = 0;
		m_cSubChord = 0;
	}

	m_cSubChord = static_cast<unsigned short>(GetCount());

	m_aSubChord = new ioSubChord[m_cSubChord];
	POSITION pos = GetStartPosition();
	while(pos)
	{
		ioSubChord Sub;
		WORD idx;
		GetNextAssoc(pos, Sub, idx);
		ASSERT(idx < m_cSubChord);
		memcpy(&m_aSubChord[idx], &Sub, sizeof(ioSubChord));
	}
}