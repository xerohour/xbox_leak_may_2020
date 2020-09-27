//-----------------------------------------------------------------------------
// File: Tool.h
//
// Desc: Header file for CDispayTool class
//
// Hist: 3.13.01 - New for April XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <dmusici.h>

#define NUM_NOTES 128
#define NUM_CHANNELS 96

class CDisplayTool : public IDirectMusicTool
{
public:
	CDisplayTool( DMUS_NOTE_PMSG * pNotes );
    ~CDisplayTool();

public:
// IUnknown
    virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    virtual STDMETHODIMP_(ULONG) AddRef();
    virtual STDMETHODIMP_(ULONG) Release();

// IDirectMusicTool
	HRESULT STDMETHODCALLTYPE Init( IDirectMusicGraph* pGraph );
	HRESULT STDMETHODCALLTYPE GetMsgDeliveryType( DWORD* pdwDeliveryType );
	HRESULT STDMETHODCALLTYPE GetMediaTypeArraySize( DWORD* pdwNumElements );
	HRESULT STDMETHODCALLTYPE GetMediaTypes( DWORD** padwMediaTypes, DWORD dwNumElements) ;
	HRESULT STDMETHODCALLTYPE ProcessPMsg( IDirectMusicPerformance* pPerf, DMUS_PMSG* pDMUS_PMSG );
	HRESULT STDMETHODCALLTYPE Flush( IDirectMusicPerformance* pPerf, DMUS_PMSG* pDMUS_PMSG, REFERENCE_TIME rt );
private:
	long	m_cRef;			    // Reference counter
    DMUS_NOTE_PMSG * m_pNotes;

public:
// Public class methods
};

