//      Copyright (c) 1996-2001 Microsoft Corporation
//      dls.cpp
//

#include "simple.h"
#include <mmsystem.h>
#include <dmerror.h>
#include "synth.h"
#include "math.h"
#include "debug.h"
#include "..\shared\validate.h"
#include <dmusicf.h>
void MemDump(char * prompt);

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
#include "synth.h"
#include <regstr.h>
#include <share.h>
//#include "waveutil.h"
#include "dmstrm.h"


STDAPI DirectSoundWaveCreate(REFIID iid, void ** ppDMC )
{
    CSourceWave *pWave = new CSourceWave;
    if (pWave)
    {
        return pWave->QueryInterface(iid,ppDMC);
    }
    return E_OUTOFMEMORY;
}

CSourceWave::CSourceWave() : CMemTrack(DMTRACK_WAVE)
{
    InitializeCriticalSection(&m_CriticalSection);

	m_fdwFlags		  = 0;
	m_pStream         = NULL;
    m_dwLoopStart     = 0;
    m_dwLoopEnd       = 0;
    m_dwRepeatCount   = 0;
	m_rtReadAheadTime = 100 * 10000;
    m_Articulation.Init(TRUE);
    m_dwSampleDataSize = 0;
    m_pnWave = NULL;

    m_cRef = 0;
}

CSourceWave::~CSourceWave()
{
	if (m_pStream) m_pStream->Release();
    delete [] m_pnWave;

    DeleteCriticalSection(&m_CriticalSection);
}

STDMETHODIMP CSourceWave::QueryInterface
(
    const IID &iid,
    void **ppv
)
{
	V_INAME(CSourceWave::QueryInterface);
	V_REFGUID(iid);
	V_PTRPTR_WRITE(ppv);

    if (iid == IID_IUnknown || iid == IID_IPersistStream)
    {
        *ppv = static_cast<IPersistStream*>(this);
    }
    else if (iid == IID_CSourceWave)
    {
        *ppv = static_cast<CSourceWave*>(this);
    }
    else if (iid == IID_IPrivateWave)
    {
        *ppv = static_cast<IPrivateWave*>(this);
    }
    else if (iid == IID_IDirectSoundWave)
    {
        *ppv = static_cast<IDirectSoundWave*>(this);
    }
    else if (iid == IID_IDirectMusicObject)
    {
        *ppv = static_cast<IDirectMusicObject*>(this);
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

STDMETHODIMP_(ULONG) CSourceWave::AddRef()
{
	V_INAME(CSourceWave::AddRef);

    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CSourceWave::Release()
{
	V_INAME(CSourceWave::Release);

    if (!InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

STDMETHODIMP CSourceWave::GetFormat
(
    LPWAVEFORMATEX pwfx,
    DWORD dwSizeAllocated,
    LPDWORD pdwSizeWritten
)
{
    if (pwfx && (dwSizeAllocated >= sizeof(WAVEFORMATEX)))
    {
        *pwfx = m_WaveFormat.wfx;
        return S_OK;
    }
    return E_FAIL;
/*	V_INAME(CSourceWave::GetFormat);

    if (!pwfx && !pdwSizeWritten)
    {
        Trace(1, "ERROR: GetFormat (Wave): Must request either the format or the required size\n.");
        return E_INVALIDARG;
    }

    dwSizeAllocated = min(sizeof(XBOXADPCMWAVEFORMAT),dwSizeAllocated);

	V_BUFPTR_WRITE(pwfx, dwSizeAllocated); 
    pwfx->cbSize = 0;
    memcpy(pwfx, &m_WaveFormat, dwSizeAllocated);
    // Return the numbers of bytes actually writted
    if (pdwSizeWritten)
    {
        *pdwSizeWritten = dwSizeAllocated;
    }

    return S_OK;*/
}

STDMETHODIMP CSourceWave::CreateSource
(
    IDirectSoundSource  **ppSource,
    LPWAVEFORMATEX      pwfx,
    DWORD               fdwFlags
)
{
//	CWaveViewPort* 	pVP;
//	CREATEVIEWPORT	cvp;

	V_INAME(CSourceWave::CreateSource);
	V_PTRPTR_WRITE(ppSource);
//    V_PWFX_READ(pwfx);

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

//    TraceI(5, "CSourceWave::CreateSource [%d samples]\n", m_cSamples);
	return E_FAIL;
/*	pVP = new CWaveViewPort;
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
	    TraceI(5, "CSourceWave::CreateSource 00\n");
	}

	if (SUCCEEDED(hr))
	{
		// The QI gave us one ref too many
		pVP->Release();
	}
	else
	{
	    TraceI(5, "CSourceWave::CreateSource 01\n");
	}

    return hr;*/
}

STDMETHODIMP CSourceWave::GetStreamingParms
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

STDMETHODIMP CSourceWave::GetClassID
(
    CLSID*  pClsId
)
{
	V_INAME(CSourceWave::GetClassID);
	V_PTR_WRITE(pClsId, CLSID); 

    *pClsId = CLSID_DirectSoundWave;
    return S_OK;
}

STDMETHODIMP CSourceWave::IsDirty()
{
	V_INAME(CSourceWave::IsDirty);

    return S_FALSE;
}

HRESULT CSourceWave::Read(DWORD dwStartPosition, BYTE *pbBuffer, DWORD dwLength)

{
    if ((dwLength + dwStartPosition) <= m_dwSampleDataSize)
    {
        LARGE_INTEGER li;
        li.QuadPart = m_dwSampleDataOffset + dwStartPosition;
        m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
        return m_pStream->Read(pbBuffer,dwLength,NULL);
    }
    return E_FAIL;
}


HRESULT CSourceWave::Load(CRiffParser *pParser)

{
	RIFFIO ckNext;
	HRESULT hr = S_OK; 

	BOOL fFormat = FALSE;
 	BOOL fHeader = FALSE;
    DWORD dwSamplesFromFact = 0;
    
    
    pParser->EnterList(&ckNext);
    while (pParser->NextChunk(&hr))
    {
		switch(ckNext.ckid)
		{
        case DMUS_FOURCC_WAVEHEADER_CHUNK:
			{
                m_fdwFlags = 0;
				fHeader = TRUE;
				DMUS_IO_WAVE_HEADER iWaveHeader;
				memset(&iWaveHeader, 0, sizeof(iWaveHeader));
                hr = pParser->Read(&iWaveHeader, sizeof(iWaveHeader));
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
        case mmioFOURCC('f','m','t',' ') :
            hr = pParser->Read(&m_WaveFormat, sizeof(XBOXADPCMWAVEFORMAT));
            if ((m_WaveFormat.wfx.wFormatTag != WAVE_FORMAT_PCM)
                && (m_WaveFormat.wfx.wFormatTag != WAVE_FORMAT_XBOX_ADPCM)) 
            {
                return E_FAIL;
            } 
            // Clean up ADPCM waveformat if it's broken.
/*            if (m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
            {
                m_WaveFormat.wSamplesPerBlock = 64;
                m_WaveFormat.wfx.nBlockAlign = 36;
                m_WaveFormat.wfx.wBitsPerSample = 4;
                m_WaveFormat.wfx.cbSize = sizeof(m_WaveFormat) - sizeof(m_WaveFormat.wfx);
            }
            else
            {
                m_WaveFormat.wfx.cbSize = 0;
            }*/
		    fFormat = TRUE;
            break;
        case DMUS_FOURCC_GUID_CHUNK:
        case DMUS_FOURCC_VERSION_CHUNK:
        case DMUS_FOURCC_CATEGORY_CHUNK:
        case DMUS_FOURCC_DATE_CHUNK:
            hr = m_Info.ReadChunk(pParser,ckNext.ckid);
			break;
		case FOURCC_LIST:
			switch(ckNext.fccType)
			{
                case DMUS_FOURCC_INFO_LIST:
                case DMUS_FOURCC_UNFO_LIST:
                    hr = m_Info.ReadChunk(pParser,ckNext.fccType);
                    break;
                case FOURCC_LART :
                case FOURCC_LAR2 :
                m_Articulation.Init(FALSE);
                m_Articulation.Load(pParser);
                break;
            }
            break;
        case mmioFOURCC('f','a','c','t'):
           hr = pParser->Read(&dwSamplesFromFact, sizeof(DWORD));
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
                hr = pParser->Read(&rloop,sizeof(RSMPL_PLUS_RLOOP));
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

        case mmioFOURCC('d','a','t','a') :
            if (!fFormat) 
            {
                hr = E_FAIL; // If no wave format, this is a bad file. 
                break;
            }
            m_dwSampleDataSize = ckNext.cksize;
            // Load the data if this is non streaming. If there was no header chunk,
            // decide by looking at the length of the wave.
            if (!fHeader)
            {
                LONGLONG llTemp = 0;
                if (m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
                {
                    llTemp = (m_dwSampleDataSize * 64) / (36 * m_WaveFormat.wfx.nChannels);
                }
                else if (m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_PCM)
                {
                    llTemp = m_dwSampleDataSize * 8; // Total bits.
                    llTemp /= (m_WaveFormat.wfx.nChannels * m_WaveFormat.wfx.wBitsPerSample); // divided by bits per interleaved sample.
                }
                llTemp *= 1000;
                llTemp /= m_WaveFormat.wfx.nSamplesPerSec;
                if (llTemp > 5000)
                {
                    m_rtReadAheadTime = 100 * 10000;
                    m_fdwFlags &= ~DSOUND_WAVEF_ONESHOT;
                }
                else
                {
                    m_rtReadAheadTime = 0;
                    m_fdwFlags |= DSOUND_WAVEF_ONESHOT;
                }
            }
            if (m_fdwFlags & DSOUND_WAVEF_ONESHOT)
            {
                m_pnWave = (short *) new char[m_dwSampleDataSize];
                if (m_pnWave)
                {
                    hr = pParser->Read(m_pnWave, m_dwSampleDataSize);
                }
                else
                {
                    hr = E_OUTOFMEMORY;
                }
            }
            else
            {
                // Store the start position if this is supposed to be streamed.
                ULARGE_INTEGER ul;
                LARGE_INTEGER li;
                li.QuadPart = 0;
                m_pStream = pParser->GetStream();
                m_pStream->AddRef();
                m_pStream->Seek(li, STREAM_SEEK_CUR, &ul);
                m_dwSampleDataOffset = (DWORD) ul.QuadPart;
            }
            break;
        default:
            break;
        }
    }
    pParser->LeaveList();
    if (fFormat)
    {
        // Now, calculate out the number of samples, taking into consideration compression.
        LONGLONG llTemp;
        if (m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
        {
            llTemp = (m_dwSampleDataSize * 64) / (36 * m_WaveFormat.wfx.nChannels);
        }
        else if (m_WaveFormat.wfx.wFormatTag == WAVE_FORMAT_PCM)
        {
            llTemp = m_dwSampleDataSize * 8; // Total bits.
            llTemp /= (m_WaveFormat.wfx.nChannels * m_WaveFormat.wfx.wBitsPerSample); // divided by bits per interleaved sample.
        }
        else
        {
            Trace(0,"ERROR: Unknown wave format tag.");
            llTemp = 0;
            hr = E_FAIL;
        }
        m_dwSampleCount = (DWORD) llTemp;
    }
    else 
    {
        // No format or no data, return error (and wave will be deleted.)
        hr = E_FAIL;
    }
    return hr;
}

HRESULT CSourceWave::Load( IStream* pIStream )
{
	V_INAME(IPersistStream::Load);
	V_INTERFACE(pIStream);

    CRiffParser Parser(pIStream);
    RIFFIO ckMain;
    HRESULT hr = S_OK;

    Parser.EnterList(&ckMain);  
    if (Parser.NextChunk(&hr) && (ckMain.fccType == mmioFOURCC('W','A','V','E')))
    {
        hr = Load(&Parser);
    }
    else
    {
        Trace(1,"Error: Unknown file format when parsing Wave\n");
        hr = E_FAIL;
    }
    return hr;
}


STDMETHODIMP CSourceWave::Save
(
    IStream*    pIStream,
    BOOL        fClearDirty
)
{
	V_INAME(CSourceWave::Save);

	return E_NOTIMPL; 
}

STDMETHODIMP CSourceWave::GetSizeMax
(
    ULARGE_INTEGER FAR* pcbSize
)
{
	V_INAME(CSourceWave::GetSizeMax);

	return E_NOTIMPL; 
}

/////////////////////////////////////////////////////////////////////////////
// IDirectMusicObject

STDMETHODIMP CSourceWave::GetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
	// Argument validation
	V_INAME(CSourceWave::GetDescriptor);
	V_STRUCTPTR_WRITE(pDesc, DMUS_OBJECTDESC);
    return m_Info.GetDescriptor(pDesc,CLSID_DirectSoundWave);
}

STDMETHODIMP CSourceWave::SetDescriptor(LPDMUS_OBJECTDESC pDesc)
{
	// Argument validation
	V_INAME(CSourceWave::SetDescriptor);
	V_STRUCTPTR_READ(pDesc, DMUS_OBJECTDESC);
	return m_Info.SetDescriptor(pDesc);
}

STDMETHODIMP CSourceWave::ParseDescriptor(LPSTREAM pIStream, LPDMUS_OBJECTDESC pDesc) 
{
    V_INAME(CSourceWave::ParseDescriptor);
    V_INTERFACE(pIStream);
    V_STRUCTPTR_WRITE(pDesc, DMUS_OBJECTDESC);
    return m_Info.ParseDescriptor(pIStream,pDesc,mmioFOURCC('W','A','V','E'),CLSID_DirectSoundWave);
}

STDMETHODIMP CSourceWave::SetWaveParams(REFERENCE_TIME rtReadAhead,DWORD dwFlags)
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

STDMETHODIMP CSourceWave::GetLength(REFERENCE_TIME *prtLength,
                               DWORD *dwLoopStart, DWORD *dwLoopEnd)
{
	HRESULT hr = S_OK;
    *dwLoopStart = m_dwLoopStart;
    *dwLoopEnd = m_dwLoopEnd;
	if (m_WaveFormat.wfx.nSamplesPerSec)
	{
        *prtLength = m_dwSampleCount;
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
                if (m_dwLoopEnd > m_dwSampleCount)
                {
                    Trace(1, "Warning: Wave has loop end %ld longer than wave length %ld\n",m_dwLoopEnd,m_dwSampleCount);
                    m_dwLoopEnd = m_dwSampleCount;
                }
                *prtLength -= (long) (m_dwSampleCount - m_dwLoopEnd);
            }
        }
        *prtLength *= 1000;
        *prtLength /= m_WaveFormat.wfx.nSamplesPerSec;
	}
	else
	{
        Trace(2, "WARNING: Couldn't get a length for a Wave.\n");
		hr = DMUS_E_BADWAVE;
	}
	return hr;
}

