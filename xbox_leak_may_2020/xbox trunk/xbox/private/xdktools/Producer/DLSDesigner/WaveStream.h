#ifndef __WAVESTREAM_H__
#define __WAVESTREAM_H__

class CWaveDataManager;


class CWaveStream : public IStream
{

public:

	CWaveStream(CWave* pWave);
	~CWaveStream();

	
public:

	// IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG	STDMETHODCALLTYPE AddRef();
    ULONG	STDMETHODCALLTYPE Release();

	// ISequentialStream Methods
	HRESULT STDMETHODCALLTYPE Read(void* pv, ULONG cb, ULONG* pcbRead);
	HRESULT STDMETHODCALLTYPE Write(void const* pv, ULONG cb, ULONG* pcbWritten);

	// IStream methods
	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition);
	HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize);
	HRESULT STDMETHODCALLTYPE CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten);
	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
	HRESULT STDMETHODCALLTYPE Revert(void);
	HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	HRESULT STDMETHODCALLTYPE Stat(STATSTG* pstatstg, DWORD grfStatFlag);
	HRESULT STDMETHODCALLTYPE Clone(IStream** ppstm);

private:
	CWaveStream(CWaveStream* pWaveStream);	// Copy ctor
	HRESULT GetWaveFileSize(DWORD* pdwWaveFileSize);

private:

	DWORD			m_dwCurrentPos;
	CWave*			m_pWave;

	long			m_lRef;


};


#endif // __WAVESTREAM_H__