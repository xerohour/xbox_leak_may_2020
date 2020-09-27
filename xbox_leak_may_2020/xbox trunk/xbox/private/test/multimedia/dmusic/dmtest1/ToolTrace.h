//-----------------------------------------------------------------------------
// File: ToolEcho.h
//
// Desc: Implements an object based on IDirectMusicTool
//       that provides echoing effects.
//
// Copyright (c) 1998-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
//lint -e1510
#pragma once

#include <globals.h>


// Maximum echoes is 4 (the number of extra groups opened
// on the port in helper.cpp)
//#define MAX_ECHOES	4

class CToolTrace : public IDirectMusicTool
{
public:
	CToolTrace();
    ~CToolTrace();

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
	CRITICAL_SECTION m_CrSec;	// To make SetEchoNum() and SetDelay() thread-safe

};


HRESULT InsertToolTrace(CtIDirectMusicPerformance *ptPerf8);
HRESULT InsertToolTrace(CtIDirectMusicAudioPath *ptAudioPath);
void RemoveToolTrace(CtIDirectMusicPerformance *ptPerf8);
void RemoveToolTrace(CtIDirectMusicAudioPath *ptAudioPath);
//lint +e1510
