// ProxyStripMgr.h : Declaration of the CProxyStripMgr

#ifndef __PROXYSTRIPMGR_H__
#define __PROXYSTRIPMGR_H__

#include "StripMgr.h"

class CSegment;
interface IDMUSProdTimeline;

/////////////////////////////////////////////////////////////////////////////
// CProxyStripMgr
class CProxyStripMgr : public IDMUSProdStripMgr
{

public:
	CProxyStripMgr();
	~CProxyStripMgr();

public:
// IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

// IDMUSProdStripMgr methods
	HRESULT STDMETHODCALLTYPE IsParamSupported( REFGUID guidType );
	HRESULT STDMETHODCALLTYPE GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData );
	HRESULT STDMETHODCALLTYPE SetParam( REFGUID guidType, MUSIC_TIME mtTime, void* pData );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	HRESULT STDMETHODCALLTYPE GetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT* pVariant );
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

protected:
	DWORD						m_dwRef;
	IDMUSProdTimeline*			m_pTimeline;

public:
	BYTE						m_bKey;
	BOOL						m_fShowFlats;
	CSegment					*m_pSegment;
};



#endif // __PROXYSTRIPMGR_H__
