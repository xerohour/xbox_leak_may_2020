///////////////////////////////////////////////////////////////////////////////
//
// File: waveout.h
//
// Date: 2/16/2001
//
// Author: danrose
//
// Purpose: Interface class decleration for Wave Out file writer XMO
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>

///////////////////////////////////////////////////////////////////////////////
//
// The Wave writer xmo inherits off the generic XMO interface, and exposes
// no other functionality
//
///////////////////////////////////////////////////////////////////////////////

class CWaveOutMediaObject : public XMediaObject
{
public:

	// Constructor / Destructor

    CWaveOutMediaObject( void );
    virtual ~CWaveOutMediaObject( void );

public:
    // Initialization

    HRESULT STDMETHODCALLTYPE Initialize( LPCSTR pszFileName, LPCWAVEFORMATEX pwfxFormat );

    // IUnknown methods

    virtual ULONG STDMETHODCALLTYPE AddRef( void );
    virtual ULONG STDMETHODCALLTYPE Release( void );

    // XMediaObject methods

    virtual HRESULT STDMETHODCALLTYPE GetInfo( LPXMEDIAINFO pInfo );
    virtual HRESULT STDMETHODCALLTYPE GetStatus( LPDWORD pdwStatus );
    virtual HRESULT STDMETHODCALLTYPE Process( LPCXMEDIAPACKET pxmbInput, LPCXMEDIAPACKET pxmbOutput );
    virtual HRESULT STDMETHODCALLTYPE Discontinuity( void );
    virtual HRESULT STDMETHODCALLTYPE Flush( void );

protected:

	// Method to write out a wave file header

	HRESULT STDMETHODCALLTYPE WriteWaveHeader( void );

	// keep around a ref count, number of bytes written, the chunk size, the waveformatex, and a file handle

	ULONG m_ulRefCount;
	DWORD m_dwBufferBytesWritten;
	DWORD m_dwChunkSize;
	WAVEFORMATEX m_wfx;
	HANDLE m_hFile;
};

///////////////////////////////////////////////////////////////////////////////
//
// Standard prototype for the creation method for the XMO
//
///////////////////////////////////////////////////////////////////////////////

HRESULT WINAPI XWaveOutCreateMediaObject( LPCSTR pszFileName, LPCWAVEFORMATEX pwfxFormat, XMediaObject **ppMediaObject );

