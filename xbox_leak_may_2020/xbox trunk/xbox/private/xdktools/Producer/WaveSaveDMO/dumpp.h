//
//
//
#ifndef _Dumpp_
#define _Dumpp_

#include <mmsystem.h>
#include <dsound.h>
#include "dsdmobse.h"
#include <dmobase.h>
#include "testdmo.h"


class CDirectSoundDumpDMO : public CDirectSoundDMO,
				public IDump
{

public:
    CDirectSoundDumpDMO();
    ~CDirectSoundDumpDMO();

    /* IUnknown */
    STDMETHODIMP QueryInterface             (THIS_ REFIID, LPVOID FAR *);
    STDMETHODIMP_(ULONG) AddRef             (THIS);
    STDMETHODIMP_(ULONG) Release            (THIS);

	// IPersist methods
	virtual HRESULT STDMETHODCALLTYPE GetClassID( CLSID *pClassID );

	// IPersistStream methods
	virtual HRESULT STDMETHODCALLTYPE IsDirty(void);
	virtual HRESULT STDMETHODCALLTYPE Load( IStream *pStm );
	virtual HRESULT STDMETHODCALLTYPE Save( IStream *pStm, BOOL fClearDirty );
	virtual HRESULT STDMETHODCALLTYPE GetSizeMax( ULARGE_INTEGER *pcbSize );

    // IDump
    STDMETHOD(SetDumpWave)	(THIS_ WCHAR *wszDumpWave);
    STDMETHOD(GetDumpWave)	(THIS_ WCHAR *wszDumpWave);
    STDMETHOD(GetOverwrite)	(THIS_ BOOL *pbOverwrite);
    STDMETHOD(SetOverwrite)	(THIS_ BOOL bOverwrite);
    STDMETHOD(Start)		(THIS);
    STDMETHOD(Stop)			(THIS);

    /* IMediaObjectInPlace */
    STDMETHODIMP Clone                      (THIS_ IMediaObjectInPlace **pDest);

	// All of these methods are called by the base class
	HRESULT FBRProcess(DWORD cQuanta, BYTE *pIn, BYTE *pOut);
	HRESULT Init();
	HRESULT Discontinuity();
    HRESULT ProcessInPlace(ULONG ulQuanta, LPBYTE pcbData, REFERENCE_TIME rtStart, DWORD dwFlags);
    
private:
    long m_cRef;
    HRESULT InitWave(WCHAR *wszDumpWave);

private:
    // dump params
    WCHAR m_wszDumpWave[_MAX_PATH];
   
    BOOL m_fDirty;
    BOOL m_bInitialized;
	BOOL m_fOverwriteExisting;
	BOOL m_fWritingWavFile;
	CRITICAL_SECTION	m_csFileHandle;

    MMCKINFO            m_ckDst;
    MMCKINFO            m_ckDstRIFF;
    HMMIO               m_hmmioDst;
};

#endif
