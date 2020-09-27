// UnknownStripMgr.h : Declaration of the CUnknownStripMgr

#ifndef __UNKNOWNSTRIPMGR_H_
#define __UNKNOWNSTRIPMGR_H_

#include "resource.h"		// main symbols
#include "timeline.h"
#include <DMUSProd.h>
#include <afxtempl.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )

class CUnknownStrip;
class CDllJazzDataObject;
interface IDirectMusicTrack;

/////////////////////////////////////////////////////////////////////////////
// CUnknownStripMgr
class CUnknownStripMgr : 
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend CUnknownStrip;

public:
	CUnknownStripMgr();
	~CUnknownStripMgr();

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

// IPersist methods
	HRESULT STDMETHODCALLTYPE GetClassID( CLSID* pClsId );

// IPersistStream methods
	HRESULT STDMETHODCALLTYPE IsDirty();
	HRESULT STDMETHODCALLTYPE Load( IStream* pIStream );
	HRESULT STDMETHODCALLTYPE Save( IStream* pIStream, BOOL fClearDirty );
	HRESULT STDMETHODCALLTYPE GetSizeMax( ULARGE_INTEGER FAR* pcbSize );

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

protected:
	HRESULT STDMETHODCALLTYPE OnDataChanged( void);

private:
	DWORD						m_dwRef;
protected:
	IDMUSProdTimeline*			m_pTimeline;

	BOOL						m_fPropPageActive;
	BOOL						m_bSelected;
	IDMUSProdFramework* 		m_pDMProdFramework;
	IDMUSProdNode*				m_pISegmentNode; // DocRoot node of Segment
	CPropertyPage*				m_pPropertyPage;
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	CDllJazzDataObject*			m_pCopyDataObject;
	BOOL						m_fDirty;
	DWORD						m_dwGroupBits;
	DWORD						m_dwTrackExtrasFlags;
	DWORD						m_dwProducerOnlyFlags;
	IDirectMusicTrack*			m_pIDMTrack;
	FOURCC						m_ckid;
	FOURCC						m_fccType;
	IStream*					m_pIStreamCopy;
	ULARGE_INTEGER				m_uliStreamSize;

	CUnknownStrip*				m_pUnknownStrip;
};


class CUnknownStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar
{
friend CUnknownStripMgr;

public:
	CUnknownStrip( CUnknownStripMgr* pUnknownStripMgr );
	~CUnknownStrip();

public:
// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

// IDMUSProdStrip
	HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
	HRESULT STDMETHODCALLTYPE SetStripProperty( STRIPPROPERTY sp, VARIANT var);
	HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar
	HRESULT STDMETHODCALLTYPE FBDraw( HDC hDC, STRIPVIEW sv );
	HRESULT STDMETHODCALLTYPE FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

protected:
	HRESULT ShowPropertySheet(IDMUSProdTimeline* pTimeline);
	void UpdateName();

	long		m_cRef;
	CUnknownStripMgr*	m_pUnknownStripMgr;
	IDMUSProdStripMgr*	m_pStripMgr;
	CString		m_strName;
	BOOL		m_bGutterSelected;	// whether the gutter select is selected, use
									// CUnknownStripMgr::m_bSelected for whether lyric is selected

private:
	HRESULT		OnShowProperties(IDMUSProdTimeline*);
};
#endif //__UNKNOWNSTRIPMGR_H_
