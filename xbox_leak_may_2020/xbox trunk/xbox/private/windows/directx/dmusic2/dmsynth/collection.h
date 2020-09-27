#ifndef COLLEC_H
#define COLLEC_H

#include "dmusicc.h"
#include "dmusics.h"
#include "CSynth.h"
#include "synth.h"
#include "..\shared\xsoundp.h" // For IDirectSoundSource
#include "..\shared\dmstrm.h"


class CCollection : public IDirectMusicCollection, public IPersistStream, public IDirectMusicObject
{
friend class CInstrument;

public:
    
	// IUnknown
    STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IPersist
	STDMETHODIMP GetClassID(CLSID* pClassID) {return E_NOTIMPL;}

    // IPersistStream
	STDMETHODIMP IsDirty() {return S_FALSE;}
    STDMETHODIMP Load(IStream* pIStream);
    STDMETHODIMP Save(IStream* pIStream, BOOL fClearDirty) {return E_NOTIMPL;}
    STDMETHODIMP GetSizeMax(ULARGE_INTEGER* pcbSize) {return E_NOTIMPL;}

	// IDirectMusicObject 
	STDMETHODIMP GetDescriptor(LPDMUS_OBJECTDESC pDesc);
	STDMETHODIMP SetDescriptor(LPDMUS_OBJECTDESC pDesc);
	STDMETHODIMP ParseDescriptor(LPSTREAM pStream, LPDMUS_OBJECTDESC pDesc);

	// IDirectMusicCollection
	STDMETHODIMP GetInstrument(DWORD dwPatch, IDirectMusicInstrument** pInstrument);
	STDMETHODIMP EnumInstrument(DWORD dwIndex, DWORD* pdwPatch, LPWSTR pName, DWORD cwchName);

	// Class
	CCollection();
    ~CCollection();

private:
    CMemTrack           m_MemTrack;
    CInfo               m_Info;
	long				m_cRef;
//    CInstrumentList                 m_InstList;
    CRITICAL_SECTION    m_CriticalSection;
};