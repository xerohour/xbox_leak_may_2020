/************************************************************************
*                                                                       *
*   Copyright (c) 1998-2001 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// BaseMgr.h : Declaration of the CBaseMgr

#ifndef __BASEMGR_H_
#define __BASEMGR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <DMUSProd.h>

class CDllJazzDataObject;
interface IDirectMusicTrack;
class CBaseMgr;
class CBaseStrip;
class CListSelectedRegion;

extern const DWORD g_dwTrackExtrasMask;
extern const DWORD g_dwProducerOnlyMask;

#define NOTE_TO_CLOCKS(note, ppq) ( (ppq)*4 /(note) )

CString GetName(DWORD dwGroupBits, CString strName);
HRESULT CopyDataToClipboard( IDMUSProdTimelineDataObject* pITimelineDataObject, IStream* pStreamCopy, UINT uiClipFormat, CBaseMgr *pBaseMgr, CBaseStrip *pBaseStrip );
HRESULT GetTimelineDataObject( IDMUSProdTimelineDataObject* &pITimelineDataObject, IDMUSProdTimeline* pTimeline, IDataObject *pIDataObject = NULL );
HRESULT MeasureBeatToBeats( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, DWORD dwIndex, long lMeasure, long lBeat, long &lNumBeats );
HRESULT BeatsToMeasureBeat( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, DWORD dwIndex, long lNumBeats, long &lMeasure, long &lBeat );

/////////////////////////////////////////////////////////////////////////////
// CBaseMgr
class ATL_NO_VTABLE CBaseMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend CBaseStrip;

public:
	CBaseMgr();
	virtual ~CBaseMgr();

BEGIN_COM_MAP(CBaseMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

// IDMUSProdStripMgr methods
	virtual STDMETHODIMP IsParamSupported( REFGUID guidType );
	virtual STDMETHODIMP GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData );
	virtual STDMETHODIMP SetParam( REFGUID guidType, MUSIC_TIME mtTime, void* pData );
	virtual STDMETHODIMP OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	virtual STDMETHODIMP GetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT* pVariant );
	virtual STDMETHODIMP SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

// IPersist methods
	virtual STDMETHODIMP GetClassID( CLSID* pClsId );

// IPersistStream methods
	virtual HRESULT STDMETHODCALLTYPE IsDirty();
	virtual HRESULT STDMETHODCALLTYPE Load( IStream* pIStream ) = 0;
	virtual HRESULT STDMETHODCALLTYPE Save( IStream* pIStream, BOOL fClearDirty ) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetSizeMax( ULARGE_INTEGER FAR* pcbSize );

// IDMUSProdPropPageObject methods
	virtual HRESULT STDMETHODCALLTYPE GetData( void **ppData) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetData( void *pData) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnShowProperties( void) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void);

protected:
	virtual void	OnDataChanged( void );
	virtual bool	RecomputeTimes( void ) = 0; // Returns true if anything changed
	virtual HRESULT	SyncWithDirectMusic( void ); // Returns S_OK if succeeded, S_FALSE if no track, E_* if failed.

public:
	IDMUSProdNode* GetDocRootNode()
	{
		return m_pIDocRootNode;
	}

public:
	IDMUSProdTimeline*			m_pTimeline;
	IDataObject*				m_pCopyDataObject;

protected:
	IDMUSProdFramework* 		m_pDMProdFramework;
	IDMUSProdNode*		 		m_pIDocRootNode;		// strip mgr belongs to this file (i.e. segment)
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	IDirectMusicTrack*			m_pIDMTrack;
	CBaseStrip*					m_pBaseStrip;
	DWORD						m_dwGroupBits;
	DWORD						m_dwOldGroupBits;
	bool						m_fDirty;
	UINT						m_nLastEdit;			// resource id of last edit
	DWORD						m_dwTrackExtrasFlags;
	DWORD						m_dwProducerOnlyFlags;

protected:
	static const CLSID			m_clsid;
	static const DWORD			m_fccType;
	static const DWORD			m_ckid;
};


class CBaseStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDropSource,
	public IDropTarget,
	public IDMUSProdPropPageObject
{
friend CBaseMgr;

public:
	CBaseStrip( CBaseMgr* pLyricMgr );
	virtual ~CBaseStrip();

// IUnknown
	virtual STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
	virtual STDMETHODIMP_(ULONG) AddRef();
	virtual STDMETHODIMP_(ULONG) Release();

// IDMUSProdStrip
	virtual HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	virtual HRESULT STDMETHODCALLTYPE GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
	virtual HRESULT STDMETHODCALLTYPE SetStripProperty( STRIPPROPERTY sp, VARIANT var);
	virtual HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar
	virtual HRESULT STDMETHODCALLTYPE FBDraw( HDC hDC, STRIPVIEW sv );
	virtual HRESULT STDMETHODCALLTYPE FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdTimelineEdit
	virtual HRESULT STDMETHODCALLTYPE Cut( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	virtual HRESULT STDMETHODCALLTYPE Copy( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL ) = 0;
	virtual HRESULT STDMETHODCALLTYPE Paste( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL ) = 0;
	virtual HRESULT STDMETHODCALLTYPE Insert( void ) = 0;
	virtual HRESULT STDMETHODCALLTYPE Delete( void ) = 0;
	virtual HRESULT STDMETHODCALLTYPE SelectAll( void );
	virtual HRESULT STDMETHODCALLTYPE CanCut( void );
	virtual HRESULT STDMETHODCALLTYPE CanCopy( void );
	virtual HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject ) = 0;
	virtual HRESULT STDMETHODCALLTYPE CanInsert( void ) = 0;
	virtual HRESULT STDMETHODCALLTYPE CanDelete( void );
	virtual HRESULT STDMETHODCALLTYPE CanSelectAll( void );

// IDropSource
	virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState );
	virtual HRESULT STDMETHODCALLTYPE GiveFeedback( DWORD dwEffect );

// IDropTarget methods
	virtual HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	virtual HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	virtual HRESULT STDMETHODCALLTYPE DragLeave( void );
	virtual HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) = 0;

// IDMUSProdPropPageObject functions
	virtual HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	virtual HRESULT STDMETHODCALLTYPE SetData( void *pData );
	virtual HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	virtual HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

protected:
// General helpers
	virtual bool	SelectItemsInSelectedRegions( void ) = 0;
	virtual bool	IsSelected( void ) = 0; // Returns true if any items are selected
	virtual bool	IsEmpty( void ) = 0;	// Returns false if there are any items
	virtual bool	SelectSegment(long begintime, long endtime) = 0; // Returns true if anything changed
	virtual HRESULT	ShowPropertySheet( void );
	virtual HRESULT	PostRightClickMenu( POINT pt );
	virtual HWND	GetTimelineHWND();

// IDropTarget helper
	virtual HRESULT CanPasteFromData(IDataObject* pIDataObject) = 0;

protected:
	long		m_cRef;
	CBaseMgr*	m_pBaseMgr;

public:
	CListSelectedRegion*	m_pSelectedRegions;

protected:
	LONG		m_lGutterBeginSelect;
	LONG		m_lGutterEndSelect;
	BOOL		m_bGutterSelected;		// whether the gutter select is selected, use
	
	BOOL		m_fSelecting;
	short		m_nStripIsDragDropSource;	
	static IDMUSProdNode*	m_pIDocRootOfDragDropSource;
	static BOOL				m_fDragDropIntoSameDocRoot;

	LONG		m_lXPos;				// used for temp storage of xpos when doing mouse edits

	IDataObject*m_pISourceDataObject;	// Object being dragged 
	IDataObject*m_pITargetDataObject;
	DWORD		m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD		m_dwOverDragButton; 	// Mouse button stored in IDropTarget::DragOver
	DWORD		m_dwOverDragEffect; 	// Drag effects stored in IDropTarget::DragOver
	DWORD		m_dwDragRMenuEffect;	// Result from drag context menu
	LONG		m_lStartDragPosition;	// xpos where drag was started
	BOOL		m_fPropPageActive;		// TRUE if our property page is active
	DWORD		m_dwDropEffect;			// temp storage of whether drag/drop completed or aborted
	bool		m_fInRightClickMenu;
	bool		m_fShowItemProps;
	bool		m_fInFunctionBarMenu;	// true if the user right-clicked in the function bar
	IDMUSProdPropPageManager*	m_pPropPageMgr;
};


#endif //__LYRICMGR_H_
