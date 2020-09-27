#ifndef __ENUM_H__
#define __ENUM_H__

// Enum.h : header file
//

#include "alist.h"

class CRegion;
class CInstrument;
class CWave;
class CCollection;
class CArticulation;


/////////////////////////////////////////////////////////////////////////////
// CEnumCollectionFolder

class CEnumCollectionFolder : public IEnumJazzNodes
{
public:
    CEnumCollectionFolder( AList *pList );

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IEnum functions
    HRESULT STDMETHODCALLTYPE Next( ULONG celt, IJazzNode __RPC_FAR* __RPC_FAR* rgelt, ULONG __RPC_FAR* pceltFetched );
    HRESULT STDMETHODCALLTYPE Skip( ULONG celt );
    HRESULT STDMETHODCALLTYPE Reset();
    HRESULT STDMETHODCALLTYPE Clone( IEnumJazzNodes __RPC_FAR* __RPC_FAR* ppenum );

private:
    CEnumCollectionFolder();

private:
    DWORD			m_dwRef;
	DWORD			m_dwPos;
    AList *			m_pList;
};

/////////////////////////////////////////////////////////////////////////////
// CEnumWave

class CEnumWave : public IEnumJazzNodes
{
public:
    CEnumWave( CWave* pWave );

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IEnum functions
    HRESULT STDMETHODCALLTYPE Next( ULONG celt, IJazzNode __RPC_FAR* __RPC_FAR* rgelt, ULONG __RPC_FAR* pceltFetched );
    HRESULT STDMETHODCALLTYPE Skip( ULONG celt );
    HRESULT STDMETHODCALLTYPE Reset();
    HRESULT STDMETHODCALLTYPE Clone( IEnumJazzNodes __RPC_FAR* __RPC_FAR* ppenum );

private:
    CEnumWave();
	IJazzNode* FindAtPosition();

private:
    DWORD			m_dwRef;
    DWORD			m_dwPos;
    CWave*			m_pWave;
};


#endif // __ENUM_H__
