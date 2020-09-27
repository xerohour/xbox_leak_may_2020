#ifndef CHORDSTRIPRIFF_H
#define CHORDSTRIPRIFF_H


#include "dmusicf.h"
#include "chordio.h"
#include "riffstrm.h"

// constants
const int MaxSubChords = 4;
/*
#ifndef DMUS_FOURCC_CHORDTRACK_LIST
#define DMUS_FOURCC_CHORDTRACK_LIST	mmioFOURCC('c','h','r','d')
#endif

#define DMUS_FOURCC_CHORDTRACK_CHORD mmioFOURCC('c', 'r', 'd', 'b')
*/

#define DMUSPROD_FOURCC_CHORDDESIGNINFO mmioFOURCC('c','r','d','t')

// simple riff read/writers
inline HRESULT ReadDWord(IDMUSProdRIFFStream* pIRiffStream, DWORD& val)
{
	ASSERT(pIRiffStream);
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream)
	{
		HRESULT hr = pIStream->Read(&val, sizeof(DWORD), 0);
		pIStream->Release();
		return hr;
	}
	else
	{
		return E_FAIL;
	}
}

inline HRESULT WriteDWord(IDMUSProdRIFFStream* pIRiffStream, DWORD val)
{
	ASSERT(pIRiffStream);
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream)
	{
		HRESULT hr = pIStream->Write(&val, sizeof(DWORD), 0);
		pIStream->Release();
		return hr;
	}
	else
	{
		return E_FAIL;
	}
}
// chunk navigators
class WriteChunk
{
	MMCKINFO m_ck;
	IDMUSProdRIFFStream* m_pRiffStream;
	HRESULT m_hr;
public:
	WriteChunk(IDMUSProdRIFFStream* pRiffStream, FOURCC id)
	{
		m_ck.ckid = id;
		m_pRiffStream = pRiffStream;
		m_hr = pRiffStream->CreateChunk( &m_ck, 0 );
	}
	HRESULT	State(MMCKINFO* pck = 0)
	{
		if(pck)
		{
			memcpy(pck,  &m_ck, sizeof(MMCKINFO));
		}
		return m_hr;
	}
	FOURCC Id()
	{
		return m_ck.ckid;
	}
	~WriteChunk()
	{
		if(m_hr == 0)
		{
			m_hr = m_pRiffStream->Ascend(&m_ck, 0);
		}
	}
};

class ReadChunk
{
	MMCKINFO m_ck;
	MMCKINFO* m_pckParent;
	IDMUSProdRIFFStream* m_pRiffStream;
	HRESULT m_hr;
public:
	ReadChunk(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent) : m_pRiffStream(pRiffStream)
	{
		m_pckParent = pckParent;
		m_hr = pRiffStream->Descend( &m_ck,  m_pckParent, 0 );
	}
	~ReadChunk()
	{
		if(m_hr == 0)
		{
			m_hr = m_pRiffStream->Ascend(&m_ck, 0);
		}
	}
	HRESULT	State(MMCKINFO* pck=0)
	{
		if(pck)
		{
			memcpy(pck, &m_ck, sizeof(MMCKINFO));
		}
		return m_hr;
	}
	FOURCC Id()
	{
		if(m_ck.ckid == FOURCC_LIST)
		{
			return m_ck.fccType;
		}
		else
		{
			return m_ck.ckid;
		}
	}
};

// list navigators
class WriteListChunk
{
	MMCKINFO m_ck;
	IDMUSProdRIFFStream* m_pRiffStream;
	HRESULT m_hr;
public:
	WriteListChunk(IDMUSProdRIFFStream* pRiffStream, FOURCC id)
	{
		m_ck.fccType = id;
		m_pRiffStream = pRiffStream;
 		m_hr = pRiffStream->CreateChunk( &m_ck, MMIO_CREATELIST );
	}
	HRESULT	State(MMCKINFO* pck=0)
	{
		if(pck)
		{
			memcpy(pck, &m_ck, sizeof(MMCKINFO));
		}
		return m_hr;
	}
	FOURCC Id()
	{
		return m_ck.ckid;
	}
	~WriteListChunk()
	{
		if(m_hr == 0)
		{
			m_hr = m_pRiffStream->Ascend(&m_ck, 0);
		}
	}
};


class SubChordChunk : public DMUS_IO_SUBCHORD
{
public:
	SubChordChunk() {ZeroMemory(this, sizeof(SubChordChunk));}
	SubChordChunk(const DMChord * pChord);
	HRESULT Write(IStream* pIStream);
	HRESULT Read(IStream* pIStream, DWORD dwSubChordSize);
	HRESULT Extract(const DMChord* pChord);
	HRESULT Insert(DMChord* pChord);
};


class ChordChunk : public DMUS_IO_CHORD
{
	SubChordChunk	m_subchords[DMPolyChord::MAX_POLY];
public:
	ChordChunk() {}
	ChordChunk(IDMUSProdRIFFStream* pRiffStream)
	{
		Read(pRiffStream);
	}
	ChordChunk(const CChordItem * pChord);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Extract(const CChordItem* pChord);
	HRESULT Insert(CChordItem* pChord);
};

class ChordDesignChunk
{
	BOOL	m_fFlatsNotSharps;
public:
	enum { structSize = sizeof(BOOL), firstItemSize = sizeof(BOOL), secondItemSize = 0 };	// use to determine how much design time info to read
	ChordDesignChunk() {m_fFlatsNotSharps = FALSE;}
	ChordDesignChunk(IDMUSProdRIFFStream* pRiffStream)
	{
		Read(pRiffStream);
	}
	ChordDesignChunk(const CChordItem * pChord);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Extract(const CChordItem* pChord);
	HRESULT Insert(CChordItem* pChord);
	BOOL& UseFlats() 
	{
		return m_fFlatsNotSharps;
	}
};

class ChordTrackHeaderChunk : public DMUS_IO_TRACK_HEADER
{
public:
	ChordTrackHeaderChunk() 
	{
		guidClassID = CLSID_DirectMusicChordTrack;
		ckid = DMUS_FOURCC_TRACK_CHUNK;
		fccType = 0;
		dwPosition = 0;	// this is undetermined
		dwGroup = 0;	// so is this
	}
	ChordTrackHeaderChunk(IDMUSProdRIFFStream* pRiffStream)
	{
		Read(pRiffStream);
	}
	ChordTrackHeaderChunk(DWORD position, DWORD group)
	{
		guidClassID = CLSID_DirectMusicChordTrack;
		ckid = 0;
		fccType = DMUS_FOURCC_TRACK_CHUNK;
		dwPosition = position;
		dwGroup = group;
	}
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream)
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
		WriteChunk chunk(pRiffStream, DMUS_FOURCC_TRACK_CHUNK);
		hr = chunk.State();
		if(hr == S_OK)
		{
			hr = pIStream->Write(dynamic_cast<DMUS_IO_TRACK_HEADER*>(this),
											  sizeof(DMUS_IO_TRACK_HEADER), &dwBytesRead);
			hr = (hr == S_OK) && (dwBytesRead == sizeof(DMUS_IO_TRACK_HEADER)) ? S_OK : E_FAIL;
		}
		pIStream->Release();
		return hr;
	}
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream)
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
		hr = pIStream->Read(dynamic_cast<DMUS_IO_TRACK_HEADER*>(this),
										  sizeof(DMUS_IO_TRACK_HEADER), &dwBytesRead);
		hr = (hr == S_OK) && (dwBytesRead == sizeof(DMUS_IO_TRACK_HEADER)) ? S_OK : E_FAIL;
		pIStream->Release();
		return hr;
	}
	DWORD& Position()
	{
		return dwPosition;
	}
	DWORD& Group()
	{
		return dwGroup;
	}
};

//
class ChordListChunk
{
	CChordList* m_plist;
	DWORD	m_scale;
	bool	m_designTime;
public:
	ChordListChunk() {m_designTime = false;}
	ChordListChunk(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
	{
		Read(pRiffStream, pckParent);
	}
	ChordListChunk( CChordList * plist, DWORD scale = 0, bool designTime = false)
	{
		m_plist = plist;
		m_scale = scale;
		m_designTime = designTime;
	}
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent);
	CChordList*& List()
	{
		return m_plist;
	}
	DWORD& Scale()
	{
		return m_scale;
	}
	bool& DesignTime()
	{
		return m_designTime;
	}
};

#endif