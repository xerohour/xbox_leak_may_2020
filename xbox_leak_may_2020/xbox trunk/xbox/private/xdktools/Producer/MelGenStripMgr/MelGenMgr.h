// MelGenMgr.h : Declaration of the CMelGenMgr

#ifndef __MELGENMGR_H_
#define __MELGENMGR_H_

#include "resource.h"		// main symbols
#include "MelGenStripMgr.h"
#include "timeline.h"
#include <DMUSProd.h>
#include <afxtempl.h>
#include "proppagemgr.h"
#include <mmsystem.h>
#include <dmusici.h>
#include <dmusicf.h>
#include <dmusicp.h>
#include <afxcmn.h>

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_CONTROL_PLAY | DMUS_TRACKCONFIG_NOTIFICATION_ENABLED | DMUS_TRACKCONFIG_CONTROL_NOTIFICATION | DMUS_TRACKCONFIG_CONTROL_ENABLED | DMUS_TRACKCONFIG_PLAY_COMPOSE | DMUS_TRACKCONFIG_LOOP_COMPOSE | DMUS_TRACKCONFIG_COMPOSING | DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT | DMUS_TRACKCONFIG_TRANS1_TOSEGSTART)

class CMelGenStrip;
class CMelGenItem;
interface IDirectMusicTrack;
interface IDMUSProdRIFFStream;

/////////////////////////////////////////////////////////////////////////////
// CMelGenMgr
class ATL_NO_VTABLE CMelGenMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMelGenMgr, &CLSID_MelGenMgr>,
	public IMelGenMgr,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend CMelGenStrip;
friend CMelGenItem;

public:
	CMelGenMgr();
	~CMelGenMgr();

public:
DECLARE_REGISTRY_RESOURCEID(IDR_MELGENMGR)

BEGIN_COM_MAP(CMelGenMgr)
	COM_INTERFACE_ENTRY_IID(IID_IMelGenMgr,IMelGenMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

/*// IStripMgr methods
	HRESULT STDMETHODCALLTYPE CreateStrip( IStrip **ppStrip, REFGUID type, VARIANT **paVariants );
	HRESULT STDMETHODCALLTYPE SetClientTimeline( ITimelineCtl* pTimeline );
	HRESULT STDMETHODCALLTYPE GetClientTimeline( ITimelineCtl** ppTimeline );
	HRESULT STDMETHODCALLTYPE GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData );
	HRESULT STDMETHODCALLTYPE SetParam( REFGUID guidType, MUSIC_TIME mtTime, void* pData );
	HRESULT STDMETHODCALLTYPE IsParamSupported( REFGUID guidType );
	HRESULT STDMETHODCALLTYPE SetGroupBits( DWORD dwGroupBits );
	HRESULT STDMETHODCALLTYPE GetGroupBits( DWORD* pdwGroupBits );
	HRESULT STDMETHODCALLTYPE SetIDirectMusicTrack( IUnknown* punkDirectMusicTrack );
// JHD: Added next four methods to sync with new IStripMgr interface
	HRESULT STDMETHODCALLTYPE SetIDMUSProdFramework( IUnknown* punkFramework );
	HRESULT STDMETHODCALLTYPE SetFourCCCKIDs( FOURCC ckid, FOURCC fccType );
	HRESULT STDMETHODCALLTYPE GetFourCCCKIDs( FOURCC* pckid, FOURCC* pfccType );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );*/
// IDMUSProdStripMgr methods
	HRESULT STDMETHODCALLTYPE IsParamSupported( REFGUID guidType );
	HRESULT STDMETHODCALLTYPE GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData );
	HRESULT STDMETHODCALLTYPE SetParam( REFGUID guidType, MUSIC_TIME mtTime, void* pData );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	HRESULT STDMETHODCALLTYPE GetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT* pVariant );
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

// IMelGenMgr methods
	HRESULT STDMETHODCALLTYPE IsMeasureBeatOpen( DWORD dwMeasure, BYTE bBeat);
	HRESULT STDMETHODCALLTYPE EnumMelGens(DWORD dwIndex, void** ppMelGen);

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
//	void ClocksToMeasureAndBeat(MUSIC_TIME mtTime, DWORD& rdwMeasure, DWORD& rdwBeat);
//	MUSIC_TIME MeasureAndBeatToClocks(DWORD dwMeasure, DWORD dwBeat);
	HRESULT LoadMelGenChunk( IStream* pIStream, 
								long lSize,
								BOOL fPaste,
								MUSIC_TIME mtPasteTime );
	HRESULT STDMETHODCALLTYPE OnDataChanged();
	void	EmptyMelGenList();
	void	InsertByAscendingTime( CMelGenItem *pMelGen );
	void	GetBoundariesOfSelectedMelGens( long *plStart, long *plEnd );
	void	DeleteBetweenTimes( long lStart, long lEnd );

	BOOL	SelectSegment(long begintime, long endtime);
	void	RemoveMelGen(POSITION pos);
	void	HookUpRepeats();
	void	HookUpRepeat(CMelGenItem* pItem);
	int		NewFragmentID();
	bool	IsUniqueFragmentID(CMelGenItem* pMelGen);

public:
	void	OnRecompose( void *pVoid );

protected:
	IDMUSProdTimeline*			m_pTimeline;

	CTypedPtrList<CPtrList, CMelGenItem*> m_lstMelGens;
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
	DWORD						m_dwPlaymode;

	CMelGenStrip*				m_pMelGenStrip;
	DMUS_MELODY_FRAGMENT			m_DefaultMelGen;
	DWORD							m_dwID;

protected:
	HRESULT DeleteSelectedMelGens();
	HRESULT SaveSelectedMelGens(LPSTREAM, CMelGenItem* pMelGenAtDragPoint, BOOL bNormalize = FALSE);
	HRESULT CreateMelGen( long lXPos, CMelGenItem*& rpMelGen );

// general helpers
	void MarkSelectedMelGens(DWORD flags);
	void UnMarkMelGens(DWORD flags);
	void DeleteMarked(DWORD flags);
	void UnselectAllKeepBits();
	void UnselectAll();
	void SelectAll();
	BOOL RemoveItem( CMelGenItem* pItem );

// Misc
	BOOL		IsSelected();	// returns if one or more melody fragments are selected.
	CMelGenItem* FirstSelectedMelGen();
	CMelGenItem* CurrentlySelectedMelGen();
};


class CMelGenStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDropSource,
	public IDropTarget,
	public IDMUSProdPropPageObject
{
friend CMelGenMgr;

public:
	CMelGenStrip( CMelGenMgr* pMelGenMgr );
	~CMelGenStrip();

public:
// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

/*// IStrip
	HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
	HRESULT STDMETHODCALLTYPE SetStripProperty( STRIPPROPERTY sp, VARIANT var);
	HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT STDMETHODCALLTYPE OnAddedToTimeline(void);
	HRESULT STDMETHODCALLTYPE GetStripMgr( IStripMgr** ppIStripMgr );*/
// IDMUSProdStrip
	HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
	HRESULT STDMETHODCALLTYPE SetStripProperty( STRIPPROPERTY sp, VARIANT var);
	HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );


/*// IStripFunctionBar
	HRESULT STDMETHODCALLTYPE FBDraw( HDC hDC, STRIPVIEW sv );
	HRESULT STDMETHODCALLTYPE FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );*/
// IDMUSProdStripFunctionBar
	HRESULT STDMETHODCALLTYPE FBDraw( HDC hDC, STRIPVIEW sv );
	HRESULT STDMETHODCALLTYPE FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

/*// ITimelineEdit
	HRESULT STDMETHODCALLTYPE Cut( ITimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Copy( ITimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( ITimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanCut( void );
	HRESULT STDMETHODCALLTYPE CanCopy( void );
	HRESULT STDMETHODCALLTYPE CanPaste( ITimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE CanInsert( void );
	HRESULT STDMETHODCALLTYPE CanDelete( void );
	HRESULT STDMETHODCALLTYPE CanSelectAll( void );*/
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
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, MUSIC_TIME mtTime, BOOL bDropNotEditPaste);

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

private:
	HRESULT	DisplayPropertySheet( IDMUSProdTimeline* );
	void UnselectGutterRange( void );
	HRESULT ComposeMelody( IDMUSProdNode* pSegmentNode );

protected:
// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	BOOL DoDragDrop();

	long			m_cRef;
	CMelGenMgr*		m_pMelGenMgr;
	IDMUSProdStripMgr*		m_pStripMgr;

	LONG			m_lGutterBeginSelect;
	LONG			m_lGutterEndSelect;
	BOOL			m_bGutterSelected;	// whether the gutter select is selected, use

	BOOL			m_bSelecting;
	BOOL			m_bContextMenuPaste;

	CBitmap			m_bmCompose;
	BOOL			m_fComposeButtonDown;

private:
	CMelGenItem* GetMelGenFromPoint( long lPos );
	CMelGenItem* GetMelGenFromMeasureBeat( DWORD dwMeasure, BYTE bBeat );
	HRESULT SelectRange( CMelGenItem* pMelGen );

	LONG		m_lXPos;				// used for temp storage of xpos when doing mouse edits

	IDataObject*m_pISourceDataObject;	// Object being dragged 
	IDataObject*m_pITargetDataObject;
	short		m_nStripIsDragDropSource;	
	DWORD		m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD		m_dwOverDragButton; 	// Mouse button stored in IDropTarget::DragOver
	DWORD		m_dwOverDragEffect; 	// Drag effects stored in IDropTarget::DragOver
	DWORD		m_dwDragRMenuEffect;	// Result from drag context menu
	UINT		m_cfMelGenList;			// CF_MELGENLIST Clipboard format
	UINT		m_cfStyle;					// CF_STYLE Clipboard format
	LONG		m_lStartDragPosition;	// xpos where drag was started
	UINT		m_nLastEdit;			// resource id of last edit
	BOOL		m_fShowMelGenProps;		// if TRUE, show MelGen property page, else show group property page
	BOOL		m_fPropPageActive;		// TRUE if our property page is active
	IDMUSProdPropPageManager*	m_pPropPageMgr;
};
#endif //__MELGENMGR_H_
