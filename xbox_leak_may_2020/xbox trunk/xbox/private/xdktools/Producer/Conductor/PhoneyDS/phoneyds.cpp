#include "stdafx.h"
#include "phoneyds.h"


CPhoneyDSound::CPhoneyDSound()
{
    m_cRef = 0;
	m_fFormatSet = false;
	m_hFile = INVALID_HANDLE_VALUE;
	::InitializeCriticalSection( &m_CriticalSection );
	AddRef();
}

CPhoneyDSound::~CPhoneyDSound()
{
	::DeleteCriticalSection( &m_CriticalSection );
}

STDMETHODIMP CPhoneyDSound::QueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUnknown || iid == IID_IDirectSound || iid == IID_IDirectSound8)
    {
        *ppv = static_cast<IDirectSound8*>(this);
    } 
    else if(iid == IID_IDirectSoundPrivate) 
    {
        *ppv = static_cast<IDirectSoundPrivate*>(this);
    } 
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    
    return S_OK;
}

STDMETHODIMP_(ULONG) CPhoneyDSound::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CPhoneyDSound::Release()
{
    if(!InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

STDMETHODIMP CPhoneyDSound::CreateSoundBuffer(LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter) 
{ 
    CBuffer *pBuffer = new CBuffer(NULL,NULL,NULL,0,GUID_NULL);
    if (pBuffer)
    {
        pBuffer->QueryInterface(IID_IDirectSoundBuffer,(void **)ppDSBuffer);
        return S_OK;
    }
    return E_OUTOFMEMORY; 
}

STDMETHODIMP CPhoneyDSound::GetCaps(LPDSCAPS pDSCaps) 
{ 
    return E_NOTIMPL; 
}

STDMETHODIMP CPhoneyDSound::DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER *ppDSBufferDuplicate) 
{ 
    return E_NOTIMPL; 
}

STDMETHODIMP CPhoneyDSound::SetCooperativeLevel(HWND hwnd, DWORD dwLevel) 
{ 
    return E_NOTIMPL; 
}

STDMETHODIMP CPhoneyDSound::Compact() 
{ 
    return E_NOTIMPL; 
}

STDMETHODIMP CPhoneyDSound::GetSpeakerConfig(LPDWORD pdwSpeakerConfig) 
{ 
    return E_NOTIMPL; 
}

STDMETHODIMP CPhoneyDSound::SetSpeakerConfig(DWORD dwSpeakerConfig) 
{ 
    return E_NOTIMPL; 
}

STDMETHODIMP CPhoneyDSound::Initialize(LPCGUID pcGuidDevice) 
{ 
    return E_NOTIMPL; 
}

STDMETHODIMP CPhoneyDSound::VerifyCertification(LPDWORD pdwCertified) 
{ 
    return E_NOTIMPL; 
}

// IDirectSoundPrivate methods
STDMETHODIMP CPhoneyDSound::AllocSink(LPWAVEFORMATEX pwfxFormat, LPDIRECTSOUNDCONNECT *ppSinkConnect) 
{ 
    CAudioSink *pSink = new CAudioSink(pwfxFormat);
    if (pSink)
    {
		if( pwfxFormat )
		{
			m_wfFormat = *pwfxFormat;
			if( m_wfFormat.nChannels == 1 )
			{
				m_wfFormat.nChannels = 2;
				m_wfFormat.nBlockAlign *= 2;
				m_wfFormat.nAvgBytesPerSec *= 2;
			}
			m_fFormatSet = true;
		}
        return pSink->QueryInterface(IID_IDirectSoundConnect,(void **)ppSinkConnect);
    }
    return E_OUTOFMEMORY;
}

HRESULT CPhoneyDSound::GetFormat( LPWAVEFORMATEX pwfxFormat )
{
	if( pwfxFormat == NULL )
	{
		return E_POINTER;
	}

	if( !m_fFormatSet )
	{
		return E_UNEXPECTED;
	}

	*pwfxFormat = m_wfFormat;

	return S_OK;
}

HANDLE CPhoneyDSound::GetFileHandle( void )
{
	HANDLE hFile;
    ::EnterCriticalSection(&m_CriticalSection);
	hFile = m_hFile;
    ::LeaveCriticalSection(&m_CriticalSection);
	return hFile;
}

void CPhoneyDSound::SetFileHandle( HANDLE hFile )
{
    ::EnterCriticalSection(&m_CriticalSection);
	m_hFile = hFile;
    ::LeaveCriticalSection(&m_CriticalSection);
}
