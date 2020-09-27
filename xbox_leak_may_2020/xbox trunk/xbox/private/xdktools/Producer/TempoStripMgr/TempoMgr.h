// TempoMgr.h : Declaration of the CTempoMgr

#ifndef __TEMPOMGR_H_
#define __TEMPOMGR_H_

#include "resource.h"		// main symbols
#include "TempoStripMgr.h"
#include "timeline.h"
#include "PropTempo.h"
#include <DMUSProd.h>
#include <afxtempl.h>

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_CONTROL_ENABLED | DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_PLAY_CLOCKTIME | DMUS_TRACKCONFIG_CONTROL_PLAY | DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT | DMUS_TRACKCONFIG_TRANS1_TOSEGSTART)

class CTempoStrip;
class CTempoItem;
class CDllJazzDataObject;
class PropPageTempo;
interface IDirectMusicTrack;

/////////////////////////////////////////////////////////////////////////////
// CTempoMgr
class ATL_NO_VTABLE CTempoMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTempoMgr, &CLSID_TempoMgr>,
	//public ITempoMgr,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend CTempoStrip;
friend PropPageTempo;

public:
	CTempoMgr();
	~CTempoMgr();

public:
DECLARE_REGISTRY_RESOURCEID(IDR_TEMPOMGR)

BEGIN_COM_MAP(CTempoMgr)
	//COM_INTERFACE_ENTRY(ITempoMgr)
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

// ITempoMgr methods
//	HRESULT STDMETHODCALLTYPE IsMeasureBeatOpen( long lMeasure, long lBeat );

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
	HRESULT STDMETHODCALLTYPE OnShowProperties( void);
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void);

protected:
	HRESULT STDMETHODCALLTYPE OnDataChanged( void);
	void	EmptyTempoList(void);
	void	InsertByAscendingTime(CTempoItem *pTempo, BOOL fPaste);
	void	SetPropTempoBarBeat( CPropTempo *pTempo );
	BOOL	SetPropTempoTime( CPropTempo *pTempo );
	void	RecomputeTempoMeasureBeats();
	BOOL	RecomputeTempoTimes();
	void	GetBoundariesOfSelectedTempos( long *plStart, long *plEnd );
	HRESULT	CreateTempo( MUSIC_TIME mtTime, CTempoItem *&pTempo );
	CTempoItem* FindTempo(long lMeasure, long lBeat);
	void	OnTransportChangeTempo( double dblTempo );
	CTempoItem* GetTempoForTime( MUSIC_TIME mtTime, CTempoItem** ppTempoItemNext );

	BOOL	SelectSegment(long begintime, long endtime);
	HRESULT SyncWithDirectMusic();

protected:
	IDMUSProdTimeline*			m_pTimeline;

	CTypedPtrList<CPtrList, CTempoItem*> m_lstTempos;
	POSITION					m_posLastGetParam;
	IDMUSProdFramework* 		m_pDMProdFramework;
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	IDataObject*				m_pCopyDataObject;
	DWORD						m_dwGroupBits;
	DWORD						m_dwIndex;;
	DWORD						m_dwTrackExtrasFlags;
	DWORD						m_dwProducerOnlyFlags;
	IDirectMusicTrack*			m_pIDMTrack;
	BOOL						m_fDirty;
	CPropTempo					m_SelectedPropTempo;
	IDirectMusicPerformance		*m_pDMPerformance;

	// Playback state variables
	IDirectMusicSegmentState	*m_pSegmentState;
	MUSIC_TIME					m_mtLoopEnd;
	MUSIC_TIME					m_mtLoopStart;
	MUSIC_TIME					m_mtStartTime;
	MUSIC_TIME					m_mtStartPoint;
	DWORD						m_dwNumLoops;

	CTempoStrip*				m_pTempoStrip;

protected:
//	HRESULT LoadTempoList( LPSTREAM pStream );
//	HRESULT SaveTempoList( LPSTREAM );
	HRESULT DeleteSelectedTempos();
	HRESULT SaveSelectedTempos(LPSTREAM pIStream, long lOffset);

// general helpers
	void MarkSelectedTempos(DWORD flags);
	void UnMarkTempos(DWORD flags);
	void DeleteMarked(DWORD flags);
	bool UnselectAll();
	void SelectAll();
	BOOL RemoveItem( CTempoItem* pItem );
	BOOL DeleteBetweenTimes( long lStart, long lEnd );
	BOOL CanCycle( long lXPos );
	HRESULT CycleTempos( long lXPos );
	HRESULT ClocksToMeasureBeat( long lTime, long *plMeasure, long *plBeat );
	MUSIC_TIME GetCurrentOffset( void );
	MUSIC_TIME GetNextGreatestUniqueTime( MUSIC_TIME mtOrigTime );

// Misc
	BOOL		IsSelected();	// returns if one or more tempos are selected.
	CTempoItem* FirstSelectedTempo();
};


class CTempoStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDropSource,
	public IDropTarget,
	public IDMUSProdPropPageObject
{
friend CTempoMgr;

public:
	CTempoStrip( CTempoMgr* pTempoMgr );
	~CTempoStrip();

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
	HRESULT STDMETHODCALLTYPE Cut( IDMUSProdTimelineDataObject *pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE Copy( IDMUSProdTimelineDataObject *pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE Paste( IDMUSProdTimelineDataObject *pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanCut( void );
	HRESULT STDMETHODCALLTYPE CanCopy( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE CanInsert( void );
	HRESULT STDMETHODCALLTYPE CanDelete( void );
	HRESULT STDMETHODCALLTYPE CanSelectAll( void );

// IDropSource
	HRESULT STDMETHODCALLTYPE QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState );
	HRESULT STDMETHODCALLTYPE GiveFeedback( DWORD dwEffect );

// IDropTarget methods
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

// IDropTarget helpers
protected:
	CWnd*	GetTimelineCWnd();
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, MUSIC_TIME mtTime, BOOL &fChanged );
	HRESULT DoDragDrop(WPARAM mousekeybutton, LONG lXPos);

// IDMUSProdPropPageObject functions
public:
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

protected:
// IDropSource helpers
	CImageList* CreateDragImage();
	HRESULT 	CreateDataObject(IDataObject**, long position);

// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	void UpdateName();
	long GetBeatWidth(long measure);
	HRESULT SelectRange(CTempoItem* pTempo);

	long		m_cRef;
	LONG		m_lGutterBeginSelect;
	LONG		m_lGutterEndSelect;
	BOOL		m_bGutterSelected;	// whether the gutter select is selected
	CTempoMgr*	m_pTempoMgr;
	IDMUSProdStripMgr*	m_pStripMgr;
	BOOL		m_bSelecting;
	CString		m_strName;
	CTempoItem*	m_pCurrentlySelectedTempoItem;

private:
	HRESULT		AddTempo( double dblTempo, MUSIC_TIME mtTime );
	HRESULT		ShowPropertySheet(IDMUSProdTimeline*);
	void		UnselectGutterRange( void );
	CTempoItem* GetTopTempoFromPoint( long lPos );
	CTempoItem* GetTopTempoFromMeasureBeatAndPosition( long lMeasure, long lBeat, POSITION posStart );
	CTempoItem* GetTopTempoFromMeasureBeatAndPositionReverse( long lMeasure, long lBeat, POSITION posStart );
	LONG		m_lXPos;				// used for temp storage of xpos when doing mouse edits

	IDataObject*m_pISourceDataObject;	// Object being dragged 
	IDataObject*m_pITargetDataObject;
	DWORD		m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD		m_dwOverDragButton; 	// Mouse button stored in IDropTarget::DragOver
	DWORD		m_dwOverDragEffect; 	// Drag effects stored in IDropTarget::DragOver
	CImageList* m_pDragImage;			// Image used for drag operation feedback
	DWORD		m_dwDragRMenuEffect;	// Result from drag context menu
	UINT		m_cfTempoList;			// Clipboard format
	LONG		m_lStartDragPosition;	// xpos where drag was started
	UINT		m_nLastEdit;			// resource id of last edit
	BOOL		m_fShowTempoProps;		// if TRUE, show Tempo property page, else show group property page
	BOOL		m_fPropPageActive;		// TRUE if our property page is active
	BOOL		m_fLButtonDown;			// TRUE if the left mouse button is down
	BOOL		m_fInDragDrop;			// TRUE if we are doing a drag'n'drop operation
	BOOL		m_fInRightClickMenu;
	BOOL		m_fSingleSelect;
	IDMUSProdPropPageManager*	m_pPropPageMgr;

};
#endif //__TEMPOMGR_H_
