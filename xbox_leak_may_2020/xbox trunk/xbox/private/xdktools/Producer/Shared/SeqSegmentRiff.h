#ifndef SEGMENTRIFF_H
#define SEGMENTRIFF_H


#include <dmusici.h>
#include <dmusicf.h>

#include <riffstrm.h>

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

#define EVENT_VOICE     1       // Performance event
#define EVENT_REALTIME  2       // qevent() must invoke interrupt
#define EVENT_ONTIME    3       // event should be handled on time



/* FullSeqEvent is SeqEvent plus next pointers*/
struct FullSeqEvent : DMUS_IO_SEQ_ITEM
{
	struct FullSeqEvent*   pNext;
	struct FullSeqEvent*	pTempNext; /* used in the compresseventlist routine */
	FullSeqEvent()
	{
		memset(this, 0, sizeof(FullSeqEvent));
	}
	FullSeqEvent(const FullSeqEvent& ev)
	{
		memset(this, 0, sizeof(FullSeqEvent));
		mtTime = ev.mtTime;
		mtDuration = ev.mtDuration;
		nOffset = ev.nOffset;
		bStatus = ev.bStatus;
		bByte1 = ev.bByte1;
		bByte2 = ev.bByte2;
//		pNext = pTempNext = 0;
	}
	FullSeqEvent& operator=(const FullSeqEvent& ev)
	{
		if(&ev != this)
		{
			memset(this, 0, sizeof(FullSeqEvent));
			mtTime = ev.mtTime;
			mtDuration = ev.mtDuration;
			nOffset = ev.nOffset;
			bStatus = ev.bStatus;
			bByte1 = ev.bByte1;
			bByte2 = ev.bByte2;
//			pNext = pTempNext = 0;
		}
		return *this;
	}	
};



inline HRESULT ReadWord(IDMUSProdRIFFStream* pIRiffStream, WORD& val)
{
	ASSERT(pIRiffStream);
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream)
	{
		HRESULT hr = pIStream->Read(&val, sizeof(WORD), 0);
		pIStream->Release();
		return hr;
	}
	else
	{
		return E_FAIL;
	}
}

inline HRESULT WriteWord(IDMUSProdRIFFStream* pIRiffStream, WORD val)
{
	ASSERT(pIRiffStream);
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream)
	{
		HRESULT hr = pIStream->Write(&val, sizeof(WORD), 0);
		pIStream->Release();
		return hr;
	}
	else
	{
		return E_FAIL;
	}
}

inline HRESULT ReadDWORD(IDMUSProdRIFFStream* pIRiffStream, DWORD& val)
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

inline HRESULT WriteDWORD(IDMUSProdRIFFStream* pIRiffStream, DWORD val)
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

// Riff navigators
class WriteRiffChunk
{
	MMCKINFO m_ck;
	IDMUSProdRIFFStream* m_pRiffStream;
	HRESULT m_hr;
public:
	WriteRiffChunk(IDMUSProdRIFFStream* pRiffStream, FOURCC id)
	{
		m_ck.fccType = id;
		m_pRiffStream = pRiffStream;
 		m_hr = pRiffStream->CreateChunk( &m_ck, MMIO_CREATERIFF );
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
	~WriteRiffChunk()
	{
		if(m_hr == 0)
		{
			m_hr = m_pRiffStream->Ascend(&m_ck, 0);
		}
	}
};



////////////////////////////////////////////////////// Segment RIFF Objects
//
class SequenceChunk : public DMUS_IO_SEQ_ITEM
{
public:
	SequenceChunk() {}
	SequenceChunk(const DMUS_IO_SEQ_ITEM& item)
	{
		Extract(item);
	}
	HRESULT Extract(const DMUS_IO_SEQ_ITEM& item);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
};

class SysexChunk : public DMUS_IO_SYSEX_ITEM
{
public:
	SysexChunk() {}
	SysexChunk(const DMUS_IO_SYSEX_ITEM& item)
	{
		Extract(item);
	}
	HRESULT Extract(const DMUS_IO_SYSEX_ITEM& item);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
};


class TrackHeaderChunk : public DMUS_IO_TRACK_HEADER
{
public:
	TrackHeaderChunk(GUID guid, FOURCC id, FOURCC type) 
	{
		guidClassID = guid;
		ckid = id;
		fccType = type;
		dwPosition = 0;	// this is undetermined
		dwGroup = 1;	// so is this
	}
	TrackHeaderChunk(GUID guid, FOURCC id, FOURCC type, DWORD position, DWORD group)
	{
		guidClassID = guid;
		ckid = id;
		fccType = type;
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
};

#endif