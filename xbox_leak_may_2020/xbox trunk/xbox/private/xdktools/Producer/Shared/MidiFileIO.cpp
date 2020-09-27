#include "stdafx.h"
#include <stdlib.h>
#include "SequenceIO.h"
#include "CurveIO.h"
#include "midifileio.h"
#include <RiffStrm.h>

#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static HRESULT StreamRead(IStream* pStream, void* data, DWORD cbData)
{
	DWORD cbRead;
	HRESULT hr;
	hr = pStream->Read((char*)data, cbData, &cbRead);
	if(FAILED(hr) || cbData != cbRead)
		return E_FAIL;
	else
		return hr;
}

static short snPPQN;

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
    __int64 d;

    if( snPPQN == DMUS_PPQ )
    {
		return dwTime;
    }
    d = dwTime;
    d *= DMUS_PPQ;
    d /= snPPQN;
    return (DWORD)d;
}
#endif


const DWORD	gdwSMFHeader =	mmioFOURCC('M', 'T', 'h', 'd'); 
const DWORD gdwSMFTrack	 =	mmioFOURCC('M', 'T', 'r', 'k');
const DWORD gdwEndOfTrack = mmioFOURCC('\x00', '\xFF', '\x2F', '\x00');

BYTE gbChannel;





int _cdecl CompareCSequenceItem(const void* m1,const void* m2)
{
	CSequenceItem* item1 = (CSequenceItem*)m1;
	CSequenceItem* item2 = (CSequenceItem*)m2;
	if(item1->Before(*item2))
		return -1;
	else if(item1->After(*item2))
		return 1;
	else
	{
		if( item1->m_bStatus == item2->m_bStatus )
		{
			return 0;
		}
		else
		{
			switch( item1->m_bStatus )
			{
			case MIDI_CCHANGE:
				return -1;
			case MIDI_NOTEON:
				return 1;
			default:
				switch( item2->m_bStatus )
				{
				case MIDI_CCHANGE:
					return 1;
				case MIDI_NOTEON:
					return -1;
				default:
					return 0;
				}
			}
		}
	}
}

static void swap(char& c1, char& c2)
{
	char temp = c2;
	c2 = c1;
	c1 = temp;
}

void ReverseIfNotMac(DWORD &dw)
{

#ifndef _MAC
	char* p = reinterpret_cast<char*>(&dw);
	swap(p[0], p[3]);
	swap(p[1], p[2]);
#endif

}

void ReverseIfNotMac(WORD &w)
{

#ifndef _MAC
	char* p = reinterpret_cast<char*>(&w);
	swap(p[0], p[1]);
#endif

}


DWORD WriteVarLen(IStream* pStream, long value)
// returns number of bytes written
{
	long buffer;
	DWORD cb = 0;
	buffer = value & 0x7f;
	while((value >>=7) > 0)
	{
		buffer <<= 8;
		buffer |= 0x80;
		buffer += (value & 0x7f);
	}

	while(true)
	{
		char c = static_cast<char>(buffer & 0xFF);
		pStream->Write(&c, 1, 0);
		cb++;
		if(buffer & 0x80)
		{
			buffer >>= 8;
		}
		else
		{
			break;
		}
	}
	return cb;
}

HRESULT WriteTrackHeader(IStream* pStream, DWORD cbSize)
{

	HRESULT hr = pStream->Write(&gdwSMFTrack, sizeof(DWORD), 0);
	if(SUCCEEDED(hr))
	{
		ReverseIfNotMac(cbSize);
		hr = pStream->Write(&cbSize, sizeof(DWORD), 0);
	}
	return hr;
}

HRESULT ReadTrackHeader(IStream* pStream, DWORD& cbHeader)
{
	DWORD streampos = StreamTell(pStream);

	// read header type
	DWORD dwType;
	HRESULT hr = StreamRead(pStream, &dwType, sizeof(DWORD));
	if(FAILED(hr))
		return hr;
	if(dwType != gdwSMFTrack)
		return E_UNEXPECTED;
	

	// read chunk size
	hr = StreamRead(pStream, &cbHeader,sizeof(DWORD));
	if(FAILED(hr))
		return hr;
	else
		ReverseIfNotMac(cbHeader);

	// move stream to end of chunk
	hr = StreamSeek(pStream, streampos + sizeof(DWORD) + cbHeader, STREAM_SEEK_SET);
	return hr;
}

HRESULT WriteEndOfTrack(IStream* pStream, DWORD& cbWritten)
{
	char buf[] = {'\x00','\xFF', '\x2F', '\x00'};
	HRESULT hr = pStream->Write(buf, 4, &cbWritten);
	hr = hr == S_OK && cbWritten == 4 ? S_OK : E_FAIL;
	return hr;
}

HRESULT WriteSMFHeader(IStream* pStream, WORD nTracks)
{
	DWORD cbWritten;
	DWORD cbSize = 6;
	WORD format = 1;
	WORD PPQ = DMUS_PPQ;

	HRESULT hr = pStream->Write(&gdwSMFHeader, sizeof(DWORD), 0);
	if(FAILED(hr))
		goto Leave;

	ReverseIfNotMac(cbSize);
	hr = pStream->Write(&cbSize, sizeof(DWORD), &cbWritten);
	if(FAILED(hr) || sizeof(DWORD) != cbWritten)
	{
		hr = E_FAIL;
		goto Leave;
	}

	// write format
	ReverseIfNotMac(format);
	hr = pStream->Write(&format, sizeof(WORD), &cbWritten);
	if(FAILED(hr) || sizeof(WORD) != cbWritten)
	{
		hr = E_FAIL;
		goto Leave;
	}
	
	// write number of tracks
	ReverseIfNotMac(nTracks);
	hr = pStream->Write(&nTracks, sizeof(WORD), &cbWritten);
	if(FAILED(hr) || sizeof(WORD) != cbWritten)
	{
		hr = E_FAIL;
		goto Leave;
	}

	// write timing info
	ReverseIfNotMac(PPQ);
	hr = pStream->Write(&PPQ, sizeof(WORD), &cbWritten);
	if(sizeof(WORD) != cbWritten)
		hr = E_FAIL;

Leave:
	return hr;
}

HRESULT ReadSMFHeader(IStream* pStream, WORD& nTracks, WORD& nFormat, WORD& nDiv)
{
	DWORD chunk;
	DWORD cbHeader;

	DWORD streampos = StreamTell(pStream);

	// read header type
	HRESULT hr = StreamRead(pStream, &chunk, sizeof(DWORD));
	if(FAILED(hr))
		return hr;

	if(chunk != gdwSMFHeader)
		return E_UNEXPECTED;

	// read chunk size
	hr = StreamRead(pStream, &cbHeader,sizeof(DWORD));
	if(FAILED(hr))
		return hr;
	else
		ReverseIfNotMac(cbHeader);

	// read format
	hr = StreamRead(pStream, &nFormat, sizeof(WORD));
	if(FAILED(hr))
		return hr;
	else
		ReverseIfNotMac(nFormat);

	// read ntracks
	hr = StreamRead(pStream, &nTracks, sizeof(WORD));
	if(FAILED(hr))
		return hr;
	else
		ReverseIfNotMac(nTracks);

	// read ndiv
	hr = StreamRead(pStream, &nDiv, sizeof(WORD));
	if(FAILED(hr))
		return hr;
	else
		ReverseIfNotMac(nDiv);

	// move stream to end of chunk
	hr = StreamSeek(pStream, streampos + sizeof(DWORD) + cbHeader, STREAM_SEEK_SET);
	return hr;
}

HRESULT WriteMidiEvent(IStream* pStream, CSequenceItem* pItem, DWORD delta_t, DWORD& cbWritten)
{
	BYTE status = Status(pItem->m_bStatus);
	BYTE bNewStatus = BYTE(status | gbChannel );
	
	HRESULT hr = S_OK;

	if( status == MIDI_PCHANGE
	||  status == MIDI_MTOUCH )
	{
		cbWritten = 2;	// size of supported types excluding delta-time
		cbWritten += WriteVarLen(pStream, delta_t);

		if(cbWritten == 2)
		{
			hr = E_FAIL;
			goto Leave;
		}

		hr = pStream->Write(&bNewStatus, 1, 0)
				|| pStream->Write(&pItem->m_bByte1, 1, 0);
	}
	else
	{
		// If this is a RPN or NRPN event
		if( (pItem->m_bStatus == MIDI_CCHANGE)
		&&	(pItem->m_bByte1 == 0xFF) )
		{
			cbWritten = 12;
			cbWritten += WriteVarLen(pStream, delta_t);

			if(cbWritten == 12)
			{
				hr = E_FAIL;
				goto Leave;
			}

			BYTE bRPNMSB = (BYTE)((pItem->m_dwMIDISaveData >> 23) & 0x7F);
			BYTE bRPNLSB = (BYTE)((pItem->m_dwMIDISaveData >> 16) & 0x7F);
			BYTE bDataMSB = (BYTE)((pItem->m_dwMIDISaveData >> 7) & 0x7F);
			BYTE bDataLSB = (BYTE)(pItem->m_dwMIDISaveData & 0x7F);
			BYTE bDataMSBCC = 0x6;
			BYTE bDataLSBCC = 0x26;
			BYTE bZero = 0;
			BYTE bRPNMSBCC;
			BYTE bRPNLSBCC;
			if( pItem->m_bByte2 == DMUS_CURVET_RPNCURVE )
			{
				bRPNMSBCC = 0x65;
				bRPNLSBCC = 0x64;
			}
			else
			{
				ASSERT( pItem->m_bByte2 == DMUS_CURVET_NRPNCURVE );
				bRPNMSBCC = 0x63;
				bRPNLSBCC = 0x62;
			}
			hr = pStream->Write(&bNewStatus, 1, 0)
					|| pStream->Write(&bRPNMSBCC, 1, 0)
					|| pStream->Write(&bRPNMSB, 1, 0)
					|| pStream->Write(&bZero, 1, 0)
					|| pStream->Write(&bRPNLSBCC, 1, 0)
					|| pStream->Write(&bRPNLSB, 1, 0)
					|| pStream->Write(&bZero, 1, 0)
					|| pStream->Write(&bDataMSBCC, 1, 0)
					|| pStream->Write(&bDataMSB, 1, 0)
					|| pStream->Write(&bZero, 1, 0)
					|| pStream->Write(&bDataLSBCC, 1, 0)
					|| pStream->Write(&bDataLSB, 1, 0);
			//BYTE bNullFunction = 0x7F;
			/* Don't write out null function - Cakewalk gets confused by it
					|| pStream->Write(&bZero, 1, 0)
					|| pStream->Write(&bRPNMSBCC, 1, 0)
					|| pStream->Write(&bNullFunction, 1, 0)
					|| pStream->Write(&bZero, 1, 0)
					|| pStream->Write(&bRPNLSBCC, 1, 0)
					|| pStream->Write(&bNullFunction, 1, 0);
			*/
		}
		else
		{
			cbWritten = 3;	// size of supported types excluding delta-time
			cbWritten += WriteVarLen(pStream, delta_t);

			if(cbWritten == 3)
			{
				hr = E_FAIL;
				goto Leave;
			}

			hr = pStream->Write(&bNewStatus, 1, 0)
					|| pStream->Write(&pItem->m_bByte1, 1, 0)
					|| pStream->Write(&pItem->m_bByte2, 1, 0);
		}
	}


Leave:
	return hr;
}


//////////////////////////////////////// Track class

CSMFTrack::~CSMFTrack()
{
	m_notes.RemoveAll();
	m_curves.RemoveAll();
}

void CSMFTrack::AddItem(CSequenceItem* pItem)
{
	m_notes.AddTail(pItem);
}

void CSMFTrack::AddCurveItem(CSequenceItem* pItem)
{
	m_curves.AddTail(pItem);
}


HRESULT CSMFTrack::Write(IStream* pStream)
{
	DWORD	cbSize=0;
	DWORD	cbInc;
	HRESULT hr;

	// need to add notes off and sort.
	int cItems = m_notes.GetCount()*2 + m_curves.GetCount();
	CSequenceItem* ary = new CSequenceItem[cItems];
	POSITION pos = m_notes.GetHeadPosition();
	int i = 0;
	while(pos)
	{
		CSequenceItem* pItem = m_notes.GetNext(pos);
		ary[i++] = *pItem;
		if( Status(pItem->m_bStatus) == MIDI_NOTEON )
		{
			pItem->m_bStatus -= 0x10;	// convert to note off - but keep MIDI Channel
			pItem->m_mtTime += pItem->m_mtDuration;
			ary[i++] = *pItem;
		}
	}

	// add curves
	pos = m_curves.GetHeadPosition();
	while(pos)
	{
		CSequenceItem* pItem = m_curves.GetNext(pos);
		ary[i++] = *pItem;
	}

	cItems = i;

	// sort
	qsort(ary, cItems, sizeof(CSequenceItem), CompareCSequenceItem);

	// save current pos
	DWORD streampos = StreamTell(pStream);

	// write header, leaving room for count
	hr = WriteTrackHeader(pStream, 0);
	//write track
	i = 0;
	DWORD t0=0, delta_t = 0;
	while(i < cItems && hr == S_OK)
	{
		CSequenceItem* pItem = &(ary[i++]);
		delta_t = (pItem->m_mtTime+pItem->m_nOffset) - t0;
		t0 = pItem->m_mtTime + pItem->m_nOffset;
		hr = WriteMidiEvent(pStream, pItem, delta_t, cbInc);
		cbSize += cbInc;
	}

	if(SUCCEEDED(hr))
	{
		DWORD cbEnd;
		// write end of track
		hr = WriteEndOfTrack(pStream, cbEnd);
		cbSize += cbEnd;
		// now back fill size
		hr = StreamSeek(pStream, streampos, STREAM_SEEK_SET);
		if(FAILED(hr))
			goto Leave;
		hr = WriteTrackHeader(pStream, cbSize);
	}

Leave:
	// set stream to end
	hr = StreamSeek(pStream,0,STREAM_SEEK_END);
	delete []ary;
	return hr;
}

HRESULT CSMFTrack::Read(IStream*)
{
	return E_NOTIMPL;
}

///////////////////////////////////////////////// MidiMerger
// Merges Midi Note on/offs into CSequenceEvents
// Assumes note on/offs are added in ascending time order
class MidiMerger
{
	CTypedPtrList<CPtrList, CSequenceItem*> m_notes;
public:
	~MidiMerger();
	bool AddNoteOn(CSequenceItem* pItem)
	{
		if(Status(pItem->m_bStatus) != MIDI_NOTEON)
			return false;

		m_notes.AddTail(pItem);
		return true;
	}

	CSequenceItem* Merge(CSequenceItem* pItem);
};

MidiMerger::~MidiMerger()
{
	POSITION pos = m_notes.GetHeadPosition();
	while(pos)
	{
		CSequenceItem* pItem = m_notes.GetNext(pos);
		delete pItem;
	}
	m_notes.RemoveAll();
}

CSequenceItem* MidiMerger::Merge(CSequenceItem* pItem)
// pItem must be a note off.  Returns 0 is corresponding note on not found
{
	if(Status(pItem->m_bStatus) != MIDI_NOTEOFF)
		return 0;

	CSequenceItem* pOn = NULL;
	POSITION pos = m_notes.GetHeadPosition();
	bool found = false;
	while(pos && !found)
	{
		POSITION tempos = pos;
		pOn = m_notes.GetNext(pos);
		if( ((pItem->m_bStatus & 0xF) == (pOn->m_bStatus & 0xF))
			&& (pItem->m_bByte1 == pOn->m_bByte1) )
		{
			found = true;
			m_notes.RemoveAt(tempos);
		}
	}

	if(!found)
		pOn = 0;

	return pOn;
}



///////////////////////////////////////////////// File class

class CSMFFile	
// sorts into different tracks based on channel
{
	CSMFTrack	tracks[16];
public:
	CSMFFile() {}
	CSMFTrack*	operator()(int i)
	{
		ASSERT( 0 <= i && i < 16);
		if( 0 <= i && i < 16)
		{
			return &tracks[i];
		}
		else
		{
			return 0;
		}
	}
	void AddItem(CSequenceItem*);
	void InsertSequence(CTypedPtrList<CPtrList, CSequenceItem*>& lst);
	HRESULT Write(IStream*);
	HRESULT Read(IStream*);
};


void CSMFFile::AddItem(CSequenceItem* pItem)
{
	tracks[gbChannel].AddItem(pItem);
}

HRESULT CSMFFile::Write(IStream* pStream)
{
	ASSERT(pStream);
	if(pStream == NULL)
		return E_INVALIDARG;

	// count # of active tracks
	WORD nTrack = 0;
	int i;
	for(i = 0; i < 16; i++)
	{
		if(!tracks[i].IsEmpty())
			nTrack++;
	}

	// write header
	HRESULT hr = WriteSMFHeader(pStream, nTrack);
	if(FAILED(hr))
	{
		goto Leave;
	}

	// write tracks
	for(i = 0; i < 16 && hr == S_OK; i++)
	{
		if(tracks[i].IsEmpty())
			continue;

		hr = tracks[i].Write(pStream);
	}

Leave:
	return hr;
}

HRESULT CSMFFile::Read(IStream* pStream)
{
	ASSERT(pStream);
	if(pStream == NULL)
		return E_INVALIDARG;

	return E_NOTIMPL;
/*
	// read file header
	WORD nTracks, nFormat
	HRESULT hr = ReadSMFHeader(pStream, nTracks, nFormat, ssnPPQ);

	if(ssnPPQ & 0x8000)
		return E_NOTIMPL;	// SMPTE not supported

	if(nTracks > 16)
		return E_NOTIMPL;	// Can't handle > 16 tracks

	// Read Tracks
	for(WORD i = 0; i < nTracks; i++)
	{
		// read track header
		DWORD cb;
		hr = ReadTrackHeader(pStream, cb);
		// read events
		bool bCont = (cb != 0);
		DWORD elapsedTime = 0;
		DWORD deltaTime;
		while(bCont)
		{
			CSequenceItem* pItem;
			hr = ReadMidiEvent(pStream, pItem, deltaTime);
		}

	}
Leave:

	return hr;
*/
}

void CSMFFile::InsertSequence(CTypedPtrList<CPtrList, CSequenceItem*>& lst)
{
	POSITION pos = lst.GetHeadPosition();
	while(pos)
	{
		CSequenceItem* pItem = lst.GetNext(pos);
		AddItem(pItem);
	}
}


//////////////////////////////////////// Public funcs
//
HRESULT	WriteMidiSequenceToStream(IStream* pIStream, DWORD dwPChannel,
								  CTypedPtrList<CPtrList, CSequenceItem*>& lstSequences)
{
	if( dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS )
	{
		gbChannel = 0;
	}
	else
	{
		gbChannel = static_cast<BYTE>(dwPChannel & 0xF);
	}
	CSMFFile midifile;
	midifile.InsertSequence(lstSequences);
	return midifile.Write(pIStream);
}
