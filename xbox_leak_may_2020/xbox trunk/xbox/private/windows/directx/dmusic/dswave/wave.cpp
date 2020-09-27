#ifdef XBOX
#include <xtl.h>
#define COM_NO_WINDOWS_H
#define RPC_NO_WINDOWS_H
#define _WINGDI_
#endif // XBOX

#include <objbase.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <dsoundp.h>
//#include "debug.h"

#include "debug.h" 
#include "dmusicc.h" 
#include "dmusicip.h" 
#include "dmusicf.h" 
#include "validate.h"
#include "riff.h"
#include "dswave.h"
#include "riff.h"
#include <regstr.h>
#include <share.h>
#include "waveutil.h"
#include "dmstrm.h"

#ifdef XBOX
#define GLOBAL_HANDLE   GPTR
#else
#define GLOBAL_HANDLE   GHND
#endif

#ifdef XBOX
#define NO_COMPRESSION_SUPPORT
#define IGNORE_ACM(format) ((format->wFormatTag == WAVE_FORMAT_PCM) || (format->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)) 
#else
#define IGNORE_ACM(format) (format->wFormatTag == WAVE_FORMAT_PCM) 
#endif // XBOX

// seeks to a 32-bit position in a stream.
HRESULT __inline StreamSeek( LPSTREAM pStream, long lSeekTo, DWORD dwOrigin )
{
	LARGE_INTEGER li;

	if( lSeekTo < 0 )
	{
		li.HighPart = -1;
	}
	else
	{
        li.HighPart = 0;
	}
	li.LowPart = lSeekTo;
	return pStream->Seek( li, dwOrigin, NULL );
}

// returns the current 32-bit position in a stream.
DWORD __inline StreamTell( LPSTREAM pStream )
{
	LARGE_INTEGER li;
    ULARGE_INTEGER ul;
#ifdef DBG
    HRESULT hr;
#endif

    li.HighPart = 0;
    li.LowPart = 0;
#ifdef DBG
    hr = pStream->Seek( li, STREAM_SEEK_CUR, &ul );
    if( FAILED( hr ) )
#else
    if( FAILED( pStream->Seek( li, STREAM_SEEK_CUR, &ul ) ) )
#endif
    {
        return 0;
    }
    return ul.LowPart;
}

CWave2::CWave2() : CMemTrack(DMTRACK_WAVE)
{
	V_INAME(CWave2::CWave2);

	InterlockedIncrement(&g_cComponent);

    InitializeCriticalSection(&m_CriticalSection);

    m_pwfx            = NULL;
    m_pwfxDst         = NULL;
	m_fdwFlags		  = 0;
    m_fdwOptions      = 0;
    m_cSamples        = 0L;
	m_pStream         = NULL;
    m_dwLoopStart     = 0;
    m_dwLoopEnd       = 0;
    m_dwRepeatCount   = 0;
	m_rtReadAheadTime = 0;

    m_cRef = 1;
}

CWave2::~CWave2()
{
	V_INAME(CWave2::~CWave2);

    if (NULL != m_pwfx)
    {
        GlobalFreePtr(m_pwfx);
    }
    if (NULL != m_pwfxDst)
    {
        GlobalFreePtr(m_pwfxDst);
    }

	if (m_pStream) m_pStream->Release();

    DeleteCriticalSection(&m_CriticalSection);

	InterlockedDecrement(&g_cComponent);
}

STDMETHODIMP CWave2::QueryInterface
(
    const IID &iid,
    void **ppv
)
{
	V_INAME(CWave2::QueryInterface);
	V_REFGUID(iid);
	V_PTRPTR_WRITE(ppv);

    if (iid == IID_IUnknown || iid == IID_IDirectSoundWave)
    {
        *ppv = static_cast<IDirectSoundWave*>(this);
    }
    else if (iid == IID_IPersistStream)
    {
        *ppv = static_cast<IPersistStream*>(this);
    }
    else if (iid == IID_IDirectMusicObject)
    {
        *ppv = static_cast<IDirectMusicObject*>(this);
    }
    else if (iid == IID_IPrivateWave)
    {
        *ppv = static_cast<IPrivateWave*>(this);
    }
    else
    {
        *ppv = NULL;
        Trace(4,"Warning: Request to query unknown interface on Wave Object\n");
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(this)->AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) CWave2::AddRef()
{
	V_INAME(CWave2::AddRef);

    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CWave2::Release()
{
	V_INAME(CWave2::Release);

    if (!InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

static inline HRESULT MMRESULTToHRESULT(
    MMRESULT mmr)
{
    switch (mmr)
    {
    case MMSYSERR_NOERROR:
        return S_OK;

    case MMSYSERR_ALLOCATED:
#ifdef XBOX
		return E_FAIL;
#else
        return DSERR_ALLOCATED;
#endif // XBOX

    case MMSYSERR_NOMEM:
        return E_OUTOFMEMORY;

    case MMSYSERR_INVALFLAG:
        return E_INVALIDARG;

    case MMSYSERR_INVALHANDLE:
        return E_INVALIDARG;

    case MMSYSERR_INVALPARAM:
        return E_INVALIDARG;
    }

    return E_FAIL;
}   

STDMETHODIMP CWave2::GetFormat
(
    LPWAVEFORMATEX pwfx,
    DWORD dwSizeAllocated,
    LPDWORD pdwSizeWritten
)
{
    DWORD           cbSize;
    LPWAVEFORMATEX  pwfxTemp = NULL;

	V_INAME(CWave2::GetFormat);

    if (!pwfx && !pdwSizeWritten)
    {
        Trace(1, "ERROR: GetFormat (Wave): Must request either the format or the required size\n.");
        return E_INVALIDARG;
    }

#ifndef XBOX
    if (!m_pwfx)
    {
        return DSERR_BADFORMAT;
    }
#endif

    if (IGNORE_ACM(m_pwfx))
    {
        pwfxTemp = m_pwfx;
    }
    else
    {
        pwfxTemp = m_pwfxDst;
        if (!pwfxTemp)
        {
            return E_FAIL;
        }
    }

    //  Note: Assuming that the wave object fills the cbSize field even
    //  on PCM formats...

    if (IGNORE_ACM(pwfxTemp))
    {
        cbSize = sizeof(WAVEFORMATEX);
    }
    else
    {
        cbSize = sizeof(WAVEFORMATEX) + pwfxTemp->cbSize;
    }

    if (cbSize > dwSizeAllocated || !pwfx)
    {
        if (pdwSizeWritten)
        {
            *pdwSizeWritten = cbSize;
            return S_OK;  //  What to return?
        }
        else
        {
            return E_FAIL;
        }
    }

	//	We don't validate this parameter any earlier on the off chance
	//	that they're doing a query...

	V_BUFPTR_WRITE(pwfx, dwSizeAllocated); 
    pwfx->cbSize = 0;
    CopyMemory(pwfx, pwfxTemp, cbSize);

    //  Set the cbSize field in destination for PCM, IF WE HAVE ROOM...

/*    if (IGNORE_ACM(pwfxTemp))
    {
        if (sizeof(WAVEFORMATEX) <= dwSizeAllocated)
        {
            pwfx->cbSize = 0;
        }
    }*/

    // Return the numbers of bytes actually writted
    if (pdwSizeWritten)
    {
        *pdwSizeWritten = cbSize;
    }

    return S_OK;
}

STDMETHODIMP CWave2::CreateSource
(
    IDirectSoundSource  **ppSource,
    LPWAVEFORMATEX      pwfx,
    DWORD               fdwFlags
)
{
	HRESULT			hr = S_OK;
	CWaveViewPort* 	pVP;
	CREATEVIEWPORT	cvp;

	V_INAME(CWave2::CreateSource);
	V_PTRPTR_WRITE(ppSource);
    V_PWFX_READ(pwfx);

    DWORD dwCreateFlags = 0;
    if (fdwFlags == DMUS_DOWNLOADINFO_ONESHOTWAVE)
    {
        dwCreateFlags |= DSOUND_WAVEF_ONESHOT;
    }
	if (dwCreateFlags & (~DSOUND_WAVEF_CREATEMASK))
	{
        Trace(1, "ERROR: CreateSource (Wave): Unknown flag.\n");
		return (E_INVALIDARG);
	}

    TraceI(5, "CWave2::CreateSource [%d samples]\n", m_cSamples);
	
	pVP = new CWaveViewPort;
    if (!pVP)
    {
        return E_OUTOFMEMORY;
    }

	cvp.pStream 	= m_pStream;
	cvp.cSamples	= m_cSamples;
	cvp.cbStream	= m_cbStream;
	cvp.pwfxSource	= m_pwfx;
	cvp.pwfxTarget  = pwfx;
    cvp.fdwOptions  = dwCreateFlags;

	hr = pVP->Create(&cvp);

	if (SUCCEEDED(hr)) 
	{
		hr = pVP->QueryInterface(IID_IDirectSoundSource, (void **)ppSource);
	}
	else
	{
	    TraceI(5, "CWave2::CreateSource 00\n");
	}

	if (SUCCEEDED(hr))
	{
		// The QI gave us one ref too many
		pVP->Release();
	}
	else
	{
	    TraceI(5, "CWave2::CreateSource 01\n");
	}

    return hr;
}

STDMETHODIMP CWave2::GetStreamingParms
(
    LPDWORD              pdwFlags, 
    LPREFERENCE_TIME    prtReadAhead
)
{
    V_INAME(IDirectSoundWave::GetStreamingParms);
    V_PTR_WRITE(pdwFlags, DWORD);
    V_PTR_WRITE(prtReadAhead, REFERENCE_TIME);
    
    *pdwFlags = 0;

    if(!(m_fdwFlags & DSOUND_WAVEF_ONESHOT))
    {
        *pdwFlags = DMUS_WAVEF_STREAMING;
    }

    *pdwFlags |= m_fdwFlags & (DMUS_WAVEF_NOPREROLL | DMUS_WAVEF_IGNORELOOPS);

    *prtReadAhead  = m_rtReadAheadTime;
    return S_OK;
}

STDMETHODIMP CWave2::GetClassID
(
    CLSID*  pClsId
)
{
	V_INAME(CWave2::GetClassID);
	V_PTR_WRITE(pClsId, CLSID); 

    *pClsId = CLSID_DirectSoundWave;
    return S_OK;
}

STDMETHODIMP CWave2::IsDirty()
{
	V_INAME(CWave2::IsDirty);

    return S_FALSE;
}

BOOL CWave2::ParseHeader
(
    IStream*        pIStream,
    IRIFFStream*    pIRiffStream,
    LPMMCKINFO      pckMain
)
{
    MMCKINFO    ck;
    DWORD       cb = 0;
    DWORD       dwPos = 0;
    MMCKINFO    ckINFO;
    HRESULT     hr;

    ck.ckid    = 0;
    ck.fccType = 0;

	V_INAME(CWave2::ParseHeader);

	BOOL fFormat = FALSE;
 	BOOL fData = FALSE;
 	BOOL fHeader = FALSE;
    DWORD dwSamplesFromFact = 0;
   
    while (0 == pIRiffStream->Descend(&ck, pckMain, MMIO_FINDCHUNK))
    {
        switch (ck.ckid)
        {
            case DMUS_FOURCC_WAVEHEADER_CHUNK:
			{
                m_fdwFlags = 0;
				fHeader = TRUE;
				DMUS_IO_WAVE_HEADER iWaveHeader;
				memset(&iWaveHeader, 0, sizeof(iWaveHeader));
                hr = pIStream->Read(&iWaveHeader, sizeof(iWaveHeader), &cb);
				if (iWaveHeader.dwFlags & DMUS_WAVEF_STREAMING)
				{
					m_rtReadAheadTime = iWaveHeader.rtReadAhead;
				}
				else
				{
					m_fdwFlags = DSOUND_WAVEF_ONESHOT;
					m_rtReadAheadTime = 0;
				}
                m_fdwFlags |= iWaveHeader.dwFlags & 
                    (DMUS_WAVEF_NOPREROLL | DMUS_WAVEF_IGNORELOOPS);

                break;
			}

			case DMUS_FOURCC_GUID_CHUNK:
				hr = pIStream->Read(&m_guid, sizeof(GUID), &cb);
				m_fdwOptions |= DMUS_OBJ_OBJECT;
                break;

            case DMUS_FOURCC_VERSION_CHUNK:
				hr = pIStream->Read( &m_vVersion, sizeof(DMUS_VERSION), &cb );
				m_fdwOptions |= DMUS_OBJ_VERSION;
                break;

            case FOURCC_LIST:
				switch(ck.fccType)
				{
                    case DMUS_FOURCC_INFO_LIST:
		                while( pIRiffStream->Descend( &ckINFO, &ck, 0 ) == 0 )
		                {
		                    switch(ckINFO.ckid)
		                    {
			                    case mmioFOURCC('I','N','A','M'):
			                    {
				                    DWORD cbSize;
				                    cbSize = min(ckINFO.cksize, DMUS_MAX_NAME);
				                    char szName[DMUS_MAX_NAME];
				                    hr = pIStream->Read((BYTE*)szName, cbSize, &cb);
				                    if(SUCCEEDED(hr))
				                    {
					                    MultiByteToWideChar(CP_ACP, 0, szName, -1, m_wszFilename, DMUS_MAX_NAME);
							            m_fdwOptions |= DMUS_OBJ_NAME;
				                    }
                                    break;
                                }
                            }
    		                pIRiffStream->Ascend( &ckINFO, 0 );
                        }
                        break;
                }
                break;

            case mmioFOURCC('f','m','t',' '):
                {
                    // If we have already allocated m_pwfx, delete it first
                    if (m_pwfx)
                    {
                        GlobalFreePtr(m_pwfx);
                    }    			    
                    m_pwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GLOBAL_HANDLE, max (sizeof(WAVEFORMATEX), ck.cksize));

                    if (!m_pwfx)
                    {
                        return FALSE;
                    }

                    hr = pIStream->Read(m_pwfx, ck.cksize, &cb);

                    if (S_OK != hr)
                    {
                        return FALSE;
                    }

#ifdef XBOX
                    // Stupid hack to fool the download system into thinking this
                    // is PCM data when it is really XBOXADPCM. Make it look like 
                    // 8 bit mono data and let the synth decode it.
                    if (m_pwfx->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
                    {
                        m_pwfx->cbSize = 0;
                        m_pwfx->nBlockAlign = 1;
                        m_pwfx->wBitsPerSample = 8;
                    }
#endif
			        if (!IGNORE_ACM(m_pwfx))
			        {
#ifdef NO_COMPRESSION_SUPPORT
					    TraceI(5, "Compression not supported\n");
                        GlobalFreePtr(m_pwfxDst);
                        m_pwfxDst = NULL;
                        return FALSE;
#else
                        if (m_pwfxDst)
                        {
                            GlobalFreePtr(m_pwfxDst);
                        }
                        m_pwfxDst = (LPWAVEFORMATEX)GlobalAllocPtr(GLOBAL_HANDLE, sizeof(WAVEFORMATEX));
                        if (!m_pwfxDst)
                        {
				            return FALSE;
                        }
                        m_pwfxDst->wFormatTag = WAVE_FORMAT_PCM;

                        MMRESULT mmr = acmFormatSuggest(NULL, m_pwfx, m_pwfxDst, sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG);
	                    if(MMSYSERR_NOERROR != mmr)
                        {
                            GlobalFreePtr(m_pwfxDst);
                            m_pwfxDst = NULL;
                            return FALSE;
                        }
#endif // NO_COMPRESSION_SUPPORT
                    }
				    fFormat = TRUE;
                    TraceI(5, "Format [%d:%d%c%02d]\n", m_pwfx->wFormatTag, m_pwfx->nSamplesPerSec/1000, ((2==m_pwfx->nChannels)?'S':'M'), m_pwfx->wBitsPerSample);
                }
                break;

            case mmioFOURCC('f','a','c','t'):
                hr = pIStream->Read(&dwSamplesFromFact, sizeof(DWORD), &cb);
                TraceI(5, "Stream is %d samples\n", dwSamplesFromFact);
                break;

            case mmioFOURCC('s','m','p','l'):
                {
                    typedef struct _rsmpl 
                    {
                      DWORD dwManufacturer;
                      DWORD dwProduct;
                      DWORD dwSamplePeriod;
                      DWORD dwMIDIUnityNote;
                      DWORD dwMIDIPitchFraction;
                      DWORD dwSMPTEFormat;
                      DWORD dwSMPTEOffset;
                      DWORD cSampleLoops;
                      DWORD cbSamplerData;
                      // That ends the RSMPL structure, which is followed
                      // by the RLOOP structure:
                      DWORD dwIdentifier;
                      DWORD dwType;
                      DWORD dwStart;
                      DWORD dwEnd;
                      DWORD dwFraction;
                      DWORD dwPlayCount;
                    } RSMPL_PLUS_RLOOP;
                    RSMPL_PLUS_RLOOP rloop;
                    rloop.dwEnd = rloop.dwStart = 0;
                    rloop.cSampleLoops = 0;
                	DWORD cSize = (sizeof(RSMPL_PLUS_RLOOP));
	                if(pckMain->cksize < (sizeof(RSMPL_PLUS_RLOOP)))
	                {
		                cSize = pckMain->cksize;
	                }
                    pIStream->Read(&rloop, cSize, &cb);
                    if (rloop.cSampleLoops)
                    {
                        m_dwLoopStart = rloop.dwStart;
                        m_dwLoopEnd = rloop.dwEnd + 1; // File format has endpoint inclusive.
                        if (rloop.dwPlayCount)
                        {
                            m_dwRepeatCount = rloop.dwPlayCount;
                        }
                        else
                        {
                            m_dwRepeatCount = -1; 
                        }
                    }
                }
                break;

            case mmioFOURCC('d','a','t','a'):
                TraceI(5, "Data chunk %d bytes\n", ck.cksize);
                m_cbStream = ck.cksize;

				fData = TRUE;
				if (!fHeader) FallbackStreamingBehavior();

                // save stream position so we can seek back later
                dwPos = StreamTell( pIStream ); 
                break;

            default:
                break;
        }
        pIRiffStream->Ascend(&ck, 0);
        ck.ckid    = 0;
        ck.fccType = 0;
    }

	if (m_pwfx && !IGNORE_ACM(m_pwfx) && dwSamplesFromFact)
	{
        m_cSamples = dwSamplesFromFact;
    }

	if (!fHeader) FallbackStreamingBehavior();

    // Seek to beginning of data
    if (fData)
    {
		StreamSeek(pIStream, dwPos, STREAM_SEEK_SET);
    }
    return fFormat && fData;
}

STDMETHODIMP CWave2::Load
(
    IStream*    pIStream
)
{
    IRIFFStream*    pIRiffStream = NULL;
    HRESULT         hr = S_OK;

	V_INAME(CWave2::Load);

    if (NULL == pIStream)
    {
        Trace(1, "ERROR: Load (Wave): Attempt to load from null stream.\n");
        return E_INVALIDARG;
    }

    EnterCriticalSection( &m_CriticalSection );

    if (SUCCEEDED(AllocRIFFStream(pIStream, &pIRiffStream)))
    {
        MMCKINFO    ckMain;

        ckMain.fccType = mmioFOURCC('W','A','V','E');

        if (0 != pIRiffStream->Descend(&ckMain, NULL, MMIO_FINDRIFF))
        {
            Trace(1, "ERROR: Load (Wave): Stream does not contain a wave chunk.\n");
            hr = E_INVALIDARG;
			goto ON_END;
        }

        //  Parses the header information and seeks to the beginning
        //  of the data in the data chunk.

        if (0 == ParseHeader(pIStream, pIRiffStream, &ckMain))
        {
		    Trace(1, "ERROR: Load (Wave): Attempt to read wave's header information failed.\n");
            hr = E_INVALIDARG;
			goto ON_END;
        }

		if (0 == m_cSamples)
		{
			if (m_pwfx && IGNORE_ACM(m_pwfx))
			{
#ifdef XBOX
                if (m_pwfx->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
                {
                    m_cSamples = (m_cbStream * 16) / (9 * m_pwfx->nChannels);
                }
                else
#endif
                {
                    m_cSamples = m_cbStream / m_pwfx->nBlockAlign;
                }
			}
			else // wave format not supported
			{
#ifdef XBOX
				hr = E_FAIL;
#else
				hr = DSERR_BADFORMAT;
#endif // XBOX
				goto ON_END;
			}
		}
    }

	pIStream->AddRef();
    if (m_pStream)
    {
        m_pStream->Release();
    }
	m_pStream = pIStream;

ON_END:
	if (pIRiffStream) pIRiffStream->Release();
    LeaveCriticalSection( &m_CriticalSection );
    TraceI(5, "CWave2::Load01\n");

    return hr;
}

STDMETHODIMP CWave2::Save
(
    IStream*    pIStream,
    BOOL        fClearDirty
)
{
	V_INAME(CWave2::Save);

	return E_NOTIMPL; 
}

STDMETHODIMP CWave2::GetSizeMax
(
    ULARGE_INTEGER FAR* pcbSize
)
{
	V_INAME(CWave2::GetSizeMax);

	return E_NOTIMPL; 
}

STDMETHODIMP CWave2::GetDescriptor
(
    LPDMUS_OBJECTDESC   pDesc
)
{
    //  Parameter validation...

	V_INAME(CWave2::GetDescriptor);
    V_STRUCTPTR_WRITE(pDesc, DMUS_OBJECTDESC)

    ZeroMemory(pDesc, sizeof(DMUS_OBJECTDESC));

    pDesc->dwSize      = sizeof(DMUS_OBJECTDESC);
    pDesc->dwValidData = DMUS_OBJ_CLASS;
    pDesc->guidClass   = CLSID_DirectSoundWave;

    if (NULL != m_pwfx)
    {
        pDesc->dwValidData |= DMUS_OBJ_LOADED;
    }

    if (m_fdwOptions & DMUS_OBJ_OBJECT)
    {
        pDesc->guidObject   = m_guid;
        pDesc->dwValidData |= DMUS_OBJ_OBJECT;
    }

    if (m_fdwOptions & DMUS_OBJ_NAME)
    {
        memcpy( pDesc->wszName, m_wszFilename, sizeof(m_wszFilename) );
        pDesc->dwValidData |= DMUS_OBJ_NAME;
    }

    if (m_fdwOptions & DMUS_OBJ_VERSION)
    {
		pDesc->vVersion.dwVersionMS = m_vVersion.dwVersionMS;
		pDesc->vVersion.dwVersionLS = m_vVersion.dwVersionLS;
        pDesc->dwValidData |= DMUS_OBJ_VERSION;
    }

    return S_OK;
}

STDMETHODIMP CWave2::SetDescriptor
(
    LPDMUS_OBJECTDESC   pDesc
)
{
    HRESULT hr = E_INVALIDARG;
    DWORD   dw = 0;

    //  Parameter validation...

	V_INAME(CWave2::SetDescriptor);
    V_PTR_READ(pDesc, DMUS_OBJECTDESC)

    if (pDesc->dwSize >= sizeof(DMUS_OBJECTDESC))
    {
        if(pDesc->dwValidData & DMUS_OBJ_CLASS)
        {
            dw           |= DMUS_OBJ_CLASS;
        }

        if(pDesc->dwValidData & DMUS_OBJ_LOADED)
        {
            dw           |= DMUS_OBJ_LOADED;
        }
        if (pDesc->dwValidData & DMUS_OBJ_OBJECT)
        {
            m_guid        = pDesc->guidObject;
            dw           |= DMUS_OBJ_OBJECT;
            m_fdwOptions |= DMUS_OBJ_OBJECT;
        }

        if (pDesc->dwValidData & DMUS_OBJ_NAME)
        {
            memcpy( m_wszFilename, pDesc->wszName, sizeof(WCHAR)*DMUS_MAX_NAME );
            dw           |= DMUS_OBJ_NAME;
            m_fdwOptions |= DMUS_OBJ_NAME;
        }

        if (pDesc->dwValidData & DMUS_OBJ_VERSION)
        {
			m_vVersion.dwVersionMS = pDesc->vVersion.dwVersionMS;
			m_vVersion.dwVersionLS = pDesc->vVersion.dwVersionLS;
			dw           |= DMUS_OBJ_VERSION;
			m_fdwOptions |= DMUS_OBJ_VERSION;
        }

        if (pDesc->dwValidData & (~dw))
        {
            Trace(2, "WARNING: SetDescriptor (Wave): Descriptor contains fields that were not set.\n");
            hr = S_FALSE;
            pDesc->dwValidData = dw;
        }
        else
        {
            hr = S_OK;
        }
    }

    return hr;
}

STDMETHODIMP CWave2::ParseDescriptor
(
    LPSTREAM            pStream,
    LPDMUS_OBJECTDESC   pDesc
)
{
	V_INAME(CWave2::ParseDescriptor);
    V_PTR_READ(pDesc, DMUS_OBJECTDESC)

    CRiffParser Parser(pStream);
    RIFFIO ckMain;
	RIFFIO ckNext;
    RIFFIO ckINFO;
    HRESULT hr = S_OK;

    Parser.EnterList(&ckMain);  
    if (Parser.NextChunk(&hr) && (ckMain.fccType == mmioFOURCC('W','A','V','E')))
    {
        Parser.EnterList(&ckNext);
	    while(Parser.NextChunk(&hr))
	    {
		    switch(ckNext.ckid)
		    {
            case DMUS_FOURCC_GUID_CHUNK:
				hr = Parser.Read( &pDesc->guidObject, sizeof(GUID) );
				if( SUCCEEDED(hr) )
				{
					pDesc->dwValidData |= DMUS_OBJ_OBJECT;
				}
				break;

            case DMUS_FOURCC_VERSION_CHUNK:
				hr = Parser.Read( &pDesc->vVersion, sizeof(DMUS_VERSION) );
				if( SUCCEEDED(hr) )
				{
					pDesc->dwValidData |= DMUS_OBJ_VERSION;
				}
				break;

			case FOURCC_LIST:
				switch(ckNext.fccType)
				{
                case DMUS_FOURCC_INFO_LIST:
                    Parser.EnterList(&ckINFO);
                    while (Parser.NextChunk(&hr))
					{
						switch( ckINFO.ckid )
						{
                        case mmioFOURCC('I','N','A','M'):
						{
				            DWORD cbSize;
				            cbSize = min(ckINFO.cksize, DMUS_MAX_NAME);
						    char szName[DMUS_MAX_NAME];
                            hr = Parser.Read(&szName, sizeof(szName));
						    if(SUCCEEDED(hr))
						    {
								MultiByteToWideChar(CP_ACP, 0, szName, -1, pDesc->wszName, DMUS_MAX_NAME);
								pDesc->dwValidData |= DMUS_OBJ_NAME;
                            }
							break;
						}
						default:
							break;
						}
					}
                    Parser.LeaveList();
					break;            
				}
				break;

			default:
				break;

		    }
        }
        Parser.LeaveList();
    }
    else
    {
        Trace(2, "WARNING: ParseDescriptor (Wave): The stream does not contain a Wave chunk.\n");
        hr = DMUS_E_CHUNKNOTFOUND;
    }
	
	return hr;
}

STDMETHODIMP CWave2::SetWaveParams(REFERENCE_TIME rtReadAhead,DWORD dwFlags)
{
	if (dwFlags & DMUS_WAVEF_STREAMING)
	{
		m_fdwFlags &= ~DSOUND_WAVEF_ONESHOT;
		m_rtReadAheadTime = rtReadAhead;
	}
	else
	{
		m_fdwFlags |= DSOUND_WAVEF_ONESHOT;
		m_rtReadAheadTime = 0;
	}

    m_fdwFlags &= ~(DMUS_WAVEF_NOPREROLL | DMUS_WAVEF_IGNORELOOPS);
    m_fdwFlags |= dwFlags & (DMUS_WAVEF_NOPREROLL | DMUS_WAVEF_IGNORELOOPS);

    return S_OK;
}

STDMETHODIMP CWave2::GetLength(REFERENCE_TIME *prtLength,
                               DWORD *dwLoopStart, DWORD *dwLoopEnd)
{
	HRESULT hr = S_OK;
	if (0 == m_cSamples)
	{
		if (m_pwfx)
        {
            LONGLONG llTemp;
            switch (m_pwfx->wFormatTag)
            {
            case WAVE_FORMAT_XBOX_ADPCM:
                llTemp = (m_cbStream * 64) / (36 * m_pwfx->nChannels);
                break;
            case WAVE_FORMAT_PCM:
                llTemp = m_cbStream * 8; // Total bits.
                llTemp /= (m_pwfx->nChannels * m_pwfx->wBitsPerSample); // divided by bits per interleaved sample.
                break;
            default:
                Trace(0,"ERROR: Unknown wave format tag.");
                llTemp = 0;
                break;
            }
            m_cSamples = (DWORD) llTemp;
        }
	}
    *dwLoopStart = m_dwLoopStart;
    *dwLoopEnd = m_dwLoopEnd;
	if (m_cSamples && m_pwfx && m_pwfx->nSamplesPerSec)
	{
        *prtLength = m_cSamples;
        if (m_dwRepeatCount)
        {
            // If it repeats infinitely, just return the max.
            if (m_dwRepeatCount == -1)
            {
                // Make it play for 5 years, which is the life expectancy of the XBOX. 
                // That should be long enough, and it still
                // avoids overflowing which would happen if we set this to max.
                // Note that this is in mils, not reference time.
                *prtLength = 157680000000; // 1000 * 60 * 60 * 24 * 365 * 5
                return S_OK;
            }
            else
            {
                // Add all the repeats to the length.
                *prtLength += m_dwRepeatCount * (m_dwLoopEnd - m_dwLoopStart);
                // Subtract the section between the end of loop and the end.
                // We have to lose this because the synth loops infinitely, so we are really
                // sending back a length that will cause playback to stop after the
                // nth repeat, but there currently is no way to get it to follow into
                // the final section, so we lose that.
                // But, first test for a bad file.
                if (m_dwLoopEnd > m_cSamples)
                {
                    Trace(1, "Warning: Wave has loop end %ld longer than wave length %ld\n",m_dwLoopEnd,m_cSamples);
                    m_dwLoopEnd = m_cSamples;
                }
                *prtLength -= (long) (m_cSamples - m_dwLoopEnd);
            }
        }
        *prtLength *= 1000;
        *prtLength /= m_pwfx->nSamplesPerSec;
	}
	else
	{
        Trace(2, "WARNING: Couldn't get a length for a Wave.\n");
		hr = DMUS_E_BADWAVE;
	}
	return hr;
}

#ifdef XBOX

STDAPI DirectSoundWaveCreate( REFIID iid, void ** ppDSW )
{
	CWave2 *pWave = new CWave2();
	if (pWave)
	{
		HRESULT hr = pWave->QueryInterface(iid,ppDSW);
		if (SUCCEEDED(hr))
		{
			pWave->Release();
		}
		return hr;
	}
	return E_OUTOFMEMORY;
}

#endif