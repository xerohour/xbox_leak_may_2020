#ifndef SEGMENTRIFF_H
#define SEGMENTRIFF_H


#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusicf.h>
#include "..\shared\dmuspriv.h"
#pragma warning( pop )
#include <riffstrm.h>

bool IsXG();


/* FullSeqEvent is SeqEvent plus next pointers*/
typedef struct FullSeqEvent : DMUS_IO_SEQ_ITEM
{
	struct FullSeqEvent*   pNext;
	struct FullSeqEvent*	pTempNext; /* used in the compresseventlist routine */
} FullSeqEvent;


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

class TempoChunk : public DMUS_IO_TEMPO_ITEM
{
public:
	TempoChunk() {}
	TempoChunk(const DMUS_IO_TEMPO_ITEM& item)
	{
		Extract(item);
	}
	HRESULT Extract(const DMUS_IO_TEMPO_ITEM& item);
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

class TimeSignatureChunk : public DMUS_IO_TIMESIGNATURE_ITEM
{
	TimeSignatureChunk() {}
	TimeSignatureChunk(const DMUS_IO_TIMESIGNATURE_ITEM& item)
	{
		Extract(item);
	}
	HRESULT Extract(const DMUS_IO_TIMESIGNATURE_ITEM& item);
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

//////////////////////////////////////////////////////////////////////////// BandTrack Riff Objects
///
/*
#ifndef DMBI_PATCH
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
#define DMBI_TRANSPOSE		(1 << 8)		// m_nTranspose
#endif
*/

#define EVENT_VOICE     1       // Performance event
#define EVENT_REALTIME  2       // qevent() must invoke interrupt
#define EVENT_ONTIME    3       // event should be handled on time

// to use, construct with type controller
// then add instances of FullSeqEvent corresponding to that controller.  If successfully added, will return true
// otherwise false.  Success will occur only if there is no controller of that type and channel whose entry delay
// (start time) is earlier.
class BandControllerArray
{
public:
	enum { MaxMidiChan = 16, Volume = 7, Pan = 10 };
	void Init();
	void DefaultUnusedChannels(long time);
	BandControllerArray(int type);
	// true if added successfully
	bool	Add(FullSeqEvent* pEvent);
	// returns true if controller on specified channel and fills in arg with that value
	bool	GetValue(int channel, char& value)
	{
		value = -1;
		ASSERT(0 <= channel && channel < MaxMidiChan);
		if(channel < 0 || channel >= MaxMidiChan)
		{
			return FALSE;
		}
		value = m_val[channel];
		return TRUE;
	}
private:
	BYTE	m_val[MaxMidiChan];
	long	  m_time[MaxMidiChan];
	bool	m_occupied[MaxMidiChan];
	int	m_type;
};

class BandPatchArray
{
public:
	enum {MaxMidiChan = 16};
	void Init();
	BandPatchArray();
	bool	Add(DMUS_IO_PATCH_ITEM*);
	bool	GetValue(int channel, DMUS_IO_PATCH_ITEM* value, bool clearPatchFlags, bool returnOnlyActive)
	{
		bool rc = false;
		ASSERT(0 <= channel || channel < MaxMidiChan);
		if(channel < 0 || channel >= MaxMidiChan)
		{
			return rc;
		}
		memcpy(value, &(m_val[channel]), sizeof(DMUS_IO_PATCH_ITEM));
		if(returnOnlyActive && m_val[channel].dwFlags == 0)
		{
			rc = false;
		}
		else
		{
			rc = true;
		}
		if(clearPatchFlags)
		{
			// mark patch as inactive
			m_val[channel].dwFlags = 0;
		}
		return rc;
	}
private:
	DMUS_IO_PATCH_ITEM	m_val[MaxMidiChan];
	long	m_time[MaxMidiChan];
	bool	m_occupied[MaxMidiChan];
};

class BandTrackHeader : public DMUS_IO_BAND_TRACK_HEADER
{
public:
	BandTrackHeader(BOOL fAutoDownload = TRUE) 
	{
		bAutoDownload = fAutoDownload;
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
		WriteChunk chunk(pRiffStream, DMUS_FOURCC_BANDTRACK_CHUNK);
		hr = chunk.State();
		if(hr == S_OK)
		{
			hr = pIStream->Write(dynamic_cast<DMUS_IO_BAND_TRACK_HEADER*>(this),
											  sizeof(DMUS_IO_BAND_TRACK_HEADER), &dwBytesRead);
			hr = (hr == S_OK) && (dwBytesRead == sizeof(DMUS_IO_BAND_TRACK_HEADER)) ? S_OK : E_FAIL;
		}
		pIStream->Release();
		return hr;
	}
};

class ListOfInst
{
	BandPatchArray& lastPatchArray;
	BandControllerArray& vol;
	BandControllerArray& pan;
public:
	ListOfInst(BandPatchArray& lastpatches, BandControllerArray& v, BandControllerArray& p) 
	: lastPatchArray(lastpatches), vol(v), pan(p) {}
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
};

class InstList
{
	DMUS_IO_PATCH_ITEM	patchitem;
	int vol, pan;
public:
	InstList(const DMUS_IO_PATCH_ITEM& item, int v = -1, int p = -1)	// -1 = controller not specified
		: vol(v), pan(p)
	{
		memcpy(&patchitem, &item, sizeof(DMUS_IO_PATCH_ITEM));
		patchitem.pIDMCollection = 0;
		patchitem.pNext = 0;	
	}
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
};

class InstChunk : public DMUS_IO_INSTRUMENT
{
public:
	InstChunk(const DMUS_IO_PATCH_ITEM& item, int vol = -1, int pan = -1)	// -1 = controller not specified
	{
		memset(dynamic_cast<DMUS_IO_INSTRUMENT*>(this), 0, sizeof(DMUS_IO_INSTRUMENT));
		dwFlags = item.dwFlags;

		if(vol > -1)
		{
			bVolume = (BYTE)vol;
			dwFlags |= DMUS_IO_INST_VOLUME;
		}
		else
		{
			bVolume = 64;
		}


		if(pan > -1)
		{
			bPan = (BYTE)pan;
			dwFlags |= DMUS_IO_INST_PAN;
		}
		else
		{
			bPan = 64;
		}
		
		// set priority to standard
		dwChannelPriority = DAUD_STANDARD_VOICE_PRIORITY;
		dwFlags |= DMUS_IO_INST_CHANNEL_PRIORITY;

		// set transpose and octave
		nTranspose = 0;
		dwFlags |= DMUS_IO_INST_TRANSPOSE;

		nPitchBendRange = 2;

		dwPatch |= item.byPChange & 0x7F;
		if(item.byLSB != 0xFF)
			dwPatch |= (item.byLSB & 0x7F) << 8;
		if(item.byMSB != 0xFF)
			dwPatch |= (item.byMSB & 0x7F) << 16;
		dwPChannel = item.byStatus & 0xF;
		if(dwPChannel == 0x9 && !IsXG())
		{
			dwPatch |= F_INSTRUMENT_DRUMS;
		}
		for(int i= 0; i < 4; i++)
			dwNoteRanges[i] = 0xFFFFFFFF;
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
		WriteChunk chunk(pRiffStream, DMUS_FOURCC_INSTRUMENT_CHUNK);
		hr = chunk.State();
		if(hr == S_OK)
		{
			hr = pIStream->Write(dynamic_cast<DMUS_IO_INSTRUMENT*>(this),
											  sizeof(DMUS_IO_INSTRUMENT), &dwBytesRead);
			hr = (hr == S_OK) && (dwBytesRead == sizeof(DMUS_IO_INSTRUMENT)) ? S_OK : E_FAIL;
		}
		pIStream->Release();
		return hr;
	}
};

class BandHeader : public DMUS_IO_BAND_ITEM_HEADER
{
public:
	BandHeader(const DMUS_IO_PATCH_ITEM& item)
	{
		lBandTime = item.lTime;
		if(lBandTime == 0)
		{
			lBandTime = -1;
		}
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
		WriteChunk chunk(pRiffStream, DMUS_FOURCC_BANDITEM_CHUNK);
		hr = chunk.State();
		if(hr == S_OK)
		{
			hr = pIStream->Write(dynamic_cast<DMUS_IO_BAND_ITEM_HEADER*>(this),
											  sizeof(DMUS_IO_BAND_ITEM_HEADER), &dwBytesRead);
			hr = (hr == S_OK) && (dwBytesRead == sizeof(DMUS_IO_BAND_ITEM_HEADER)) ? S_OK : E_FAIL;
		}
		pIStream->Release();
		return hr;
	}
};

class BandList
{
	BandControllerArray& vol;
	BandControllerArray& pan;
	BandPatchArray& lastPatchArray;
	DMUS_IO_PATCH_ITEM* pPatchList;
	int m_bandno;
public:
	BandList(DMUS_IO_PATCH_ITEM* ptch, BandPatchArray& b,	BandControllerArray& v, BandControllerArray& p, int bandno)
		:pPatchList(ptch),lastPatchArray(b), vol(v), pan(p), m_bandno(bandno) {}
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
};

class ListOfBands
{
	DMUS_IO_PATCH_ITEM*	pPatchList;
	class CMIDITrack* pMIDITracks;
public:
	enum {continuity = 192};	// minimum separation before patch change becomes separate band
	ListOfBands(DMUS_IO_PATCH_ITEM* p,   CMIDITrack *MIDITracks) 
	: pPatchList(p), pMIDITracks(MIDITracks) {}
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);	// deletes pPatchList
};

// This class encapusulates a list of MIDI events, all of which play on one PChannel.
// It is used to split a MIDI file into separate Sequence tracks.
class CMIDITrack
{
public:
	CMIDITrack(	FullSeqEvent* lstTrackEvent, FullSeqEvent* lstOther, FullSeqEvent* lstVol, FullSeqEvent* lstPan, const CString &strTitle );
	~CMIDITrack();

	FullSeqEvent*		m_lstTrackEvent;
	FullSeqEvent*		m_lstOther;
	FullSeqEvent*		m_lstPan;
	FullSeqEvent*		m_lstVol;
	CString				m_strTitle;
	class CMIDITrack*	m_pNext;
};

#endif