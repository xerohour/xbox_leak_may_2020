//////////////////////////////////////////////////////////////////////
//
// WaveStream.cpp : Implementation file
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaveNode.h"
#include "Wave.h"
#include "WaveDataManager.h"
#include "WaveStream.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWaveStream::CWaveStream(CWave* pWave) : m_pWave(pWave), m_lRef(0)
{
	ASSERT(pWave);
	m_dwCurrentPos = 0;
	AddRef();
}

CWaveStream::CWaveStream(CWaveStream* pWaveStream) : m_lRef(0)
{
	ASSERT(pWaveStream);
	m_pWave = pWaveStream->m_pWave;
	m_dwCurrentPos = pWaveStream->m_dwCurrentPos;
	AddRef();
}


CWaveStream::~CWaveStream()
{
}


//////////////////////////////////////////////////////////////////////
//
//	CWaveStream::QueryInterface()
//
//////////////////////////////////////////////////////////////////////
HRESULT CWaveStream::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if(IsEqualIID(riid, IID_IUnknown))
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

    if(::IsEqualIID(riid, IID_IStream))
    {
        AddRef();
        *ppvObj = (IStream*)this;
        return S_OK;
    } 

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveStream::AddRef()
//
//////////////////////////////////////////////////////////////////////
ULONG CWaveStream::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return InterlockedIncrement(&m_lRef);
}

//////////////////////////////////////////////////////////////////////
//
//	CWaveStream::Release()
//
//////////////////////////////////////////////////////////////////////
ULONG CWaveStream::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(m_lRef != 0);

	InterlockedDecrement(&m_lRef); 

    if(m_lRef == 0)
    {
        delete this;
        return 0;
    }
	
	AfxOleUnlockApp();
    return m_lRef;
}


HRESULT CWaveStream::Read(void* pv, ULONG cb, ULONG* pcbRead)
{
	ASSERT(pv);
	if(pv == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pcbRead);
	if(pcbRead == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	DWORD dwHeaderSize = m_pWave->GetRIFFHeaderSize();
	DWORD dwDataSize = m_pWave->GetDataSize();
	DWORD dwFileSize = dwHeaderSize + dwDataSize;

	if(m_dwCurrentPos < dwHeaderSize && m_dwCurrentPos + cb <= dwHeaderSize)
	{
		IStream* pIStream = m_pWave->GetHeaderMemStream();
		ASSERT(pIStream);
		if(pIStream == NULL)
		{
			return E_FAIL;
		}

		if(FAILED(pIStream->Read(pv, cb, pcbRead)))
		{
			return E_FAIL;
		}

		// Update the position
		m_dwCurrentPos += *pcbRead;
		return S_OK;

	}

	if(m_dwCurrentPos >= dwHeaderSize && m_dwCurrentPos + cb <= dwFileSize)
	{
		// We need to read cb number of bytes of wave data
		// Convert that to samples
		int nBytesPerSample = m_pWave->m_rWaveformat.wBitsPerSample / 8;
		int nChannels = m_pWave->m_rWaveformat.nChannels;

		DWORD dwStart = ((m_dwCurrentPos - dwHeaderSize) / nBytesPerSample) / nChannels;
		DWORD dwLength = (cb / nBytesPerSample) / nChannels;

		DWORD cbRead = 0;
		CWaveDataManager* pDataManager = m_pWave->GetDataManager();
		ASSERT(pDataManager);
		if(pDataManager == NULL)
		{
			return E_UNEXPECTED;
		}

		CWaveNode* pWaveNode = m_pWave->GetNode();
		ASSERT(pWaveNode);
		if(pWaveNode == NULL)
		{
			return E_UNEXPECTED;
		}

		EnterCriticalSection(&pWaveNode->m_CriticalSection);
		HRESULT hr = pDataManager->GetData(dwStart, dwLength, (BYTE*)pv, cbRead);
		LeaveCriticalSection(&pWaveNode->m_CriticalSection);
		if(FAILED(hr))
		{
			return hr;
		}

		m_dwCurrentPos += cbRead;
		*pcbRead = cbRead;

		return S_OK;
	}

	return E_FAIL;
}


HRESULT CWaveStream::Write(void const* pv, ULONG cb, ULONG * pcbWritten)
{
	ASSERT(0);
	return E_NOTIMPL;
}



HRESULT CWaveStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	DWORD dwHeaderSize = m_pWave->GetRIFFHeaderSize();
	DWORD dwDataSize = m_pWave->GetDataSize();
	DWORD dwWaveFileSize = dwHeaderSize + dwDataSize;
	
	long  lSeekPosition = 0;
	if(dwOrigin == STREAM_SEEK_SET)
	{
		lSeekPosition = dlibMove.LowPart;
	}
	if(dwOrigin == STREAM_SEEK_CUR)
	{
		lSeekPosition = m_dwCurrentPos + dlibMove.LowPart;
	}
	if(dwOrigin == STREAM_SEEK_END)
	{
		lSeekPosition = dwWaveFileSize - dlibMove.LowPart;
	}

	// reject seeking before start, but not after end (fix 50785)
	if(lSeekPosition < 0)
	{
		return E_FAIL;
	}

	if(plibNewPosition)
	{
		plibNewPosition->LowPart = lSeekPosition;
		plibNewPosition->HighPart = 0L;
	}

	m_dwCurrentPos = (DWORD) lSeekPosition;

	return S_OK;
}


HRESULT CWaveStream::SetSize(ULARGE_INTEGER libNewSize)
{
	return E_NOTIMPL;
}


HRESULT CWaveStream::CopyTo(IStream * pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
{
	ASSERT(0);
	return E_NOTIMPL;
}


HRESULT CWaveStream::Commit(DWORD grfCommitFlags)
{
	ASSERT(0);
	return E_NOTIMPL;
}


HRESULT CWaveStream::Revert(void)
{
	ASSERT(0);
	return E_NOTIMPL;
}


HRESULT CWaveStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	ASSERT(0);
	return E_NOTIMPL;
}


HRESULT CWaveStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	ASSERT(0);
	return E_NOTIMPL;
}


HRESULT CWaveStream::Stat(STATSTG* pstatstg, DWORD grfStatFlag)
{
	ASSERT(0);
	return E_NOTIMPL;
}


HRESULT CWaveStream::Clone(IStream** ppstm)
{
	CWaveStream* pClonedStream = new CWaveStream(this);
	if(pClonedStream == NULL)
	{
		return E_FAIL;
	}

	*ppstm = (IStream*)pClonedStream;
	
	return S_OK;
}

HRESULT CWaveStream::GetWaveFileSize(DWORD* pdwWaveFileSize)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	ASSERT(pdwWaveFileSize);
	if(pdwWaveFileSize == NULL)
	{
		return E_POINTER;
	}

	DWORD dwHeaderSize = m_pWave->GetRIFFHeaderSize();
	DWORD dwDataSize = m_pWave->GetDataSize();

	return dwHeaderSize + dwDataSize;
}
