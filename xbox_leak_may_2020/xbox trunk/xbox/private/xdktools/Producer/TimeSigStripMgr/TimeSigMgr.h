// TimeSigMgr.h : Declaration of the CTimeSigMgr

#ifndef __TIMESIGMGR_H_
#define __TIMESIGMGR_H_

#include "resource.h"		// main symbols
#include "TimeSigStripMgr.h"
#include "timeline.h"
#include <DMUSProd.h>
#include <afxtempl.h>
#include "proppagemgr.h"
#include <mmsystem.h>
#include <dmusici.h>
#include <dmusicf.h>

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_CONTROL_ENABLED | DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_NOTIFICATION_ENABLED | DMUS_TRACKCONFIG_CONTROL_PLAY | DMUS_TRACKCONFIG_CONTROL_NOTIFICATION )

class CTimeSigStrip;
class CTimeSigItem;
interface IDirectMusicTrack;
interface IDMUSProdRIFFStream;

/////////////////////////////////////////////////////////////////////////////
// CTimeSigMgr
class ATL_NO_VTABLE CTimeSigMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimeSigMgr, &CLSID_TimeSigMgr>,
	public ITimeSigMgr,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend CTimeSigStrip;
friend CTimeSigItem;

public:
	CTimeSigMgr();
	~CTimeSigMgr();

public:
DECLARE_REGISTRY_RESOURCEID(IDR_TIMESIGMGR)

BEGIN_COM_MAP(CTimeSigMgr)
	COM_INTERFACE_ENTRY_IID(IID_ITimeSigMgr,ITimeSigMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

// IDMUSProdStripMgr methods
	HRESULT STDMETHODCALLTYPE IsParamSupported( REFGUID guidType );
	HRESULT STDMETHODCALLTYPE GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData );
	HRESULT STDMETHODCALLTYPE SetParam( REFGUID guidType, MUSIC_TIME mtTime, void* pData );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	HRESULT STDMETHODCALLTYPE GetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT* pVariant );
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

// ITimeSigMgr methods
	HRESULT STDMETHODCALLTYPE IsMeasureOpen( DWORD dwMeasure );

// IPersist methods
	STDMETHOD(GetClassID)( CLSID* pClsId );

// IPersistStream methods
	STDMETHOD(IsDirty)();
	STDMETHOD(Load)( IStream* pIStream );
	STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
	STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

// IDMUSProdPropPageObject methods
	HRESULT STDMETHODCALLTYPE GetData( void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

protected:
	HRESULT SyncWithDirectMusic();
	void ClocksToMeasure(MUSIC_TIME mtTime, DWORD& rdwMeasure);
	MUSIC_TIME MeasureToClocks(DWORD dwMeasure);
	HRESULT LoadTimeSigChunk( IStream* pIStream, 
								long lSize,
										BOOL fPaste,
								MUSIC_TIME mtPasteTime );
	HRESULT STDMETHODCALLTYPE OnDataChanged();
	void	EmptyTimeSigList();
	void	InsertByAscendingTime( CTimeSigItem *pTimeSig );
	void	GetBoundariesOfSelectedTimeSigs( long *plStart, long *plEnd );
	BOOL	DeleteBetweenTimes( long lStart, long lEnd );

	BOOL	SelectSegment(long begintime, long endtime);

protected:
	IDMUSProdTimeline*			m_pTimeline;

	CTypedPtrList<CPtrList, CTimeSigItem*> m_lstTimeSigs;
	IDMUSProdFramework* 		m_pDMProdFramework;
	CPropertyPage*				m_pPropertyPage;
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	IDataObject*				m_pCopyDataObject;
	DWORD						m_dwGroupBits;
	DWORD						m_dwTrackExtrasFlags;
	DWORD						m_dwProducerOnlyFlags;
	IDirectMusicTrack*			m_pIDMTrack;
	IDMUSProdNode*				m_pISegmentNode; // DocRoot node of Segment
	BOOL						m_fDirty;
	BOOL						m_fHaveStyleRefStrip;

	CTimeSigStrip*				m_pTimeSigStrip;

protected:
	HRESULT DeleteSelectedTimeSigs();
	HRESULT SaveSelectedTimeSigs(LPSTREAM pIStream, MUSIC_TIME mtOffset);
	HRESULT CreateTimeSig( MUSIC_TIME mtTime, CTimeSigItem*& rpTimeSig );
	void UpdateHaveStyleFlag();

// general helpers
	void MarkSelectedTimeSigs(DWORD flags);
	void UnMarkTimeSigs(DWORD flags);
	void DeleteMarked(DWORD flags);
	void UnselectAllKeepBits();
	BOOL UnselectAll();
	void SelectAll();
	BOOL RemoveItem( CTimeSigItem* pItem );

// Misc
	BOOL		IsSelected();	// returns if one or more timesigs are selected.
	CTimeSigItem* FirstSelectedTimeSig();
	CTimeSigItem* FirstSelectedRealTimeSig();
};


class CTimeSigStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDropSource,
	public IDropTarget,
	public IDMUSProdPropPageObject
{
friend CTimeSigMgr;

public:
	CTimeSigStrip( CTimeSigMgr* pTimeSigMgr );
	~CTimeSigStrip();

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

// IDMUSProdTimelineEdit
	HRESULT STDMETHODCALLTYPE Cut( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Copy( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanCut( void );
	HRESULT STDMETHODCALLTYPE CanCopy( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE CanInsert( void );
	HRESULT STDMETHODCALLTYPE CanDelete( void );
	HRESULT STDMETHODCALLTYPE CanSelectAll( void );

// IDropSource
	HRESULT STDMETHODCALLTYPE QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState );
	HRESULT STDMETHODCALLTYPE GiveFeedback( DWORD dwEffect );

// IDropSource helpers
	HRESULT 	CreateDataObject(IDataObject**, long position);

// IDropTarget methods
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

// IDropTarget helpers
	CWnd*	GetTimelineCWnd();
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteReplace( long firstMeasure, long firstBeat, long lastMeasure, long lastBeat);
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, MUSIC_TIME mtTime, BOOL bDropNotEditPaste, BOOL &fChanged);

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

private:
	HRESULT	DisplayPropertySheet( IDMUSProdTimeline* );
	void UnselectGutterRange( void );

protected:
// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LONG lXPos );
	HRESULT OnRButtonDown( WPARAM wParam, LONG lXPos );
	BOOL DoDragDrop();

	long				m_cRef;
	CTimeSigMgr*		m_pTimeSigMgr;
	IDMUSProdStripMgr*	m_pStripMgr;

	LONG			m_lGutterBeginSelect;
	LONG			m_lGutterEndSelect;
	BOOL			m_bGutterSelected;	// whether the gutter select is selected, use

	BOOL			m_bSelecting;
	BOOL			m_bContextMenuPaste;

private:
	CTimeSigItem* GetTimeSigFromPoint( long lPos );
	CTimeSigItem* GetTimeSigFromMeasureBeat( DWORD dwMeasure, BYTE bBeat );
	HRESULT SelectRange( CTimeSigItem* pTimeSig );

	LONG		m_lXPos;				// used for temp storage of xpos when doing mouse edits

	IDataObject*m_pISourceDataObject;	// Object being dragged 
	IDataObject*m_pITargetDataObject;
	short		m_nStripIsDragDropSource;	
	DWORD		m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD		m_dwOverDragButton; 	// Mouse button stored in IDropTarget::DragOver
	DWORD		m_dwOverDragEffect; 	// Drag effects stored in IDropTarget::DragOver
	DWORD		m_dwDragRMenuEffect;	// Result from drag context menu
	UINT		m_cfTimeSigList;			// CF_TIMESIGLIST Clipboard format
	UINT		m_cfStyle;					// CF_STYLE Clipboard format
	LONG		m_lStartDragPosition;	// xpos where drag was started
	UINT		m_nLastEdit;			// resource id of last edit
	BOOL		m_fShowTimeSigProps;		// if TRUE, show TimeSig property page, else show group property page
	BOOL		m_fPropPageActive;		// TRUE if our property page is active
	BOOL		m_fInRightClickMenu;
	BOOL		m_fSingleSelect;
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	DWORD		m_dwShiftSelectFromMeasure;
};
#endif //__TIMESIGMGR_H_
