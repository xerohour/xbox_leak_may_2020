
#ifndef _MIDISAVE_H_
#define _MIDISAVE_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <dmusicc.h>
#include <dmusici.h>
#include "alist.h"

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

#define MIDI_CC_BS_MSB	0x00
#define MIDI_CC_BS_LSB	0x20

#define MIDI_CC_DATA_MSB	0x06
#define MIDI_CC_DATA_LSB	0x26
#define MIDI_CC_NRPN_LSB	0x62
#define MIDI_CC_NRPN_MSB	0x63
#define MIDI_CC_RPN_LSB	0x64
#define MIDI_CC_RPN_MSB	0x65

class MIDIEvent : public AListItem
{
public:
	MIDIEvent* GetNext()
	{
		return (MIDIEvent*)AListItem::GetNext();
	};
    MIDIEvent* GetItem(LONG lIndex) 
	{
		return (MIDIEvent*) AListItem::GetItem(lIndex);
	};
    MUSIC_TIME  m_mtTime;   // Music time this event occurs.
    BYTE        m_bStatus;  // MIDI status. FF for Meta Event.
    BYTE        m_bLength;  // How many bytes follow.
    BYTE        m_bData[12];  // Event data 
};

class MIDIList : public AList
{
public:
    MIDIList();
    ~MIDIList();
    MIDIEvent* GetHead() 
	{
		return (MIDIEvent*)AList::GetHead();
	};
    MIDIEvent* RemoveHead() 
	{
		return (MIDIEvent*)AList::RemoveHead();
	};
    MIDIEvent* GetItem(LONG lIndex) 
	{
		return (MIDIEvent*) AList::GetItem(lIndex);
	};
    void    Sort();
    void    Clear();
    DWORD   PrepToSave(MUSIC_TIME mtStartOffset, MUSIC_TIME *pmtLength);
    void    SetName(char *pName);
    HRESULT SaveTrack( FILE *file, MUSIC_TIME mtEnd );
    DWORD   CalcTrack( MUSIC_TIME mtEnd );
    short   m_nTranspose;
private:
    char * m_pName;
};

class MIDISaveTool : public IDirectMusicTool
{
public:
	MIDISaveTool();
    ~MIDISaveTool();
    HRESULT StartRecording();
    HRESULT StopRecording();
    HRESULT SetChannels(DWORD dwChannels);
    HRESULT SetChannelName(DWORD dwTrack, char *pName);
    HRESULT SaveMIDIFile(LPCTSTR pFileName, IDirectMusicSegmentState *pState, 
        BOOL fExtraMeasure, BOOL fRMID);
public:
// IUnknown
    virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    virtual STDMETHODIMP_(ULONG) AddRef();
    virtual STDMETHODIMP_(ULONG) Release();

// IDirectMusicTool
	HRESULT STDMETHODCALLTYPE Init(IDirectMusicGraph* pGraph) ;
	HRESULT STDMETHODCALLTYPE GetMsgDeliveryType(DWORD* pdwDeliveryType ) ;
	HRESULT STDMETHODCALLTYPE GetMediaTypeArraySize(DWORD* pdwNumElements ) ;
	HRESULT STDMETHODCALLTYPE GetMediaTypes(DWORD** padwMediaTypes, DWORD dwNumElements) ;
	HRESULT STDMETHODCALLTYPE ProcessPMsg(IDirectMusicPerformance* pPerf, DMUS_PMSG* pDMUS_PMSG) ;
	HRESULT STDMETHODCALLTYPE Flush(IDirectMusicPerformance* pPerf, DMUS_PMSG* pDMUS_PMSG, REFERENCE_TIME rt) ;
private:
    HRESULT SaveMIDISong( FILE *file, MUSIC_TIME mtLength, DWORD dwTracks);
    CRITICAL_SECTION    m_CrSec;	        
	long	            m_cRef;			    // reference counter
    MIDIList **         m_ppChannelTracks;  // Array of lists, one for each PChannel
    MIDIList            m_ControlTrack;     // Control commands
    BOOL                m_fRecordEnabled;   // Must be active to allow recording.
    DWORD               m_dwChannels;       // Number of PChannels being recorded.
};

#endif