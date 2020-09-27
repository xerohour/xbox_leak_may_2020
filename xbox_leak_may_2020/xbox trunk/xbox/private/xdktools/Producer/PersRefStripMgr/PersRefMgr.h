// PersRefMgr.h : Declaration of the CPersRefMgr

#ifndef __PERS_REFMGR_H_
#define __PERS_REFMGR_H_

#include "resource.h"		// main symbols
#include "ChordMapRefStripMgr.h"
#include "timeline.h"
#include <DMUSProd.h>
#include <afxtempl.h>
#include "proppagemgr.h"
#include <mmsystem.h>

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_CONTROL_ENABLED | DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT | DMUS_TRACKCONFIG_TRANS1_TOSEGSTART)

class CPersRefStrip;
class CPersRefItem;
interface IDirectMusicTrack;
interface IDirectMusicChordMap;
interface IDMUSProdRIFFStream;

/////////////////////////////////////////////////////////////////////////////
// CPersRefMgr
class ATL_NO_VTABLE CPersRefMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPersRefMgr, &CLSID_PersRefMgr>,
	public IPersRefMgr,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend CPersRefStrip;
friend CPersRefItem;

public:
	CPersRefMgr();
	~CPersRefMgr();

public:
DECLARE_REGISTRY_RESOURCEID(IDR_PERS_REFMGR)

BEGIN_COM_MAP(CPersRefMgr)
	COM_INTERFACE_ENTRY_IID(IID_IPersRefMgr,IPersRefMgr)
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

// IPersRefMgr methods
	HRESULT STDMETHODCALLTYPE IsMeasureBeatOpen( DWORD dwMeasure, BYTE bBeat);

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
	HRESULT STDMETHODCALLTYPE OnDataChanged();
	void	FixPersRefListMeasureBeat();	
	void	EmptyPersRefList();
	void	InsertByAscendingTime( CPersRefItem *pPersRef, BOOL fPaste );
	void	GetBoundariesOfSelectedPersRefs( long *plStart, long *plEnd );
	BOOL	DeleteBetweenTimes( long lStart, long lEnd );

	BOOL	SelectSegment(long begintime, long endtime);

protected:
	IDMUSProdTimeline*			m_pTimeline;

	CTypedPtrList<CPtrList, CPersRefItem*> m_lstPersRefs;
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

	CPersRefStrip*				m_pPersRefStrip;
	CString 					m_strLastPersName;
protected:
	HRESULT SyncWithDirectMusic();
	HRESULT GetDirectMusicPersonality(IDMUSProdNode* pIPersDocRoot, IDirectMusicChordMap** ppIPers);
	HRESULT LoadPersRefList( IDMUSProdRIFFStream* pIRiffStream, 
								MMCKINFO* pckParent,
								bool fPaste,
								MUSIC_TIME mtTime,
								BOOL &fChanged );
	HRESULT LoadPersRef( IDMUSProdRIFFStream* pIRiffStream, 
								MMCKINFO* pckParent,
								bool fPaste,
								MUSIC_TIME mtTime );
	IDMUSProdNode* FindPersonality( CString strStyleName, IStream* pIStream );
	HRESULT SavePersReference(
		IStream* pIStream, IDMUSProdRIFFStream* pIRiffStream, CPersRefItem* pPersRefItem);
	HRESULT SaveDMRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode, WhichLoader whichLoader );
	HRESULT SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode );
	HRESULT SetPersReference( IDMUSProdNode* pINewPersDocRootNode, CPersRefItem* pItem );
	HRESULT DeleteSelectedPersRefs();
	HRESULT SaveSelectedPersRefs(LPSTREAM, CPersRefItem* pPersRefAtDragPoint, BOOL bNormalize = FALSE);

// general helpers
	void MarkSelectedPersRefs(DWORD flags);
	void UnMarkPersRefs(DWORD flags);
	void DeleteMarked(DWORD flags);
	bool UnselectAll();
	void SelectAll();
	BOOL RemoveItem( CPersRefItem* pItem );

// Misc
	BOOL		IsSelected();	// returns if one or more personality references are selected.
	CPersRefItem* FirstSelectedPersRef();
};


class CPersRefStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDropSource,
	public IDropTarget,
	public IDMUSProdPropPageObject
{
friend CPersRefMgr;

public:
	CPersRefStrip( CPersRefMgr* pPersRefMgr );
	~CPersRefStrip();

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
	CImageList* CreateDragImage();
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
	HRESULT	ShowPropertySheet( IDMUSProdTimeline* );
	void UnselectGutterRange( void );

protected:
// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);

	long				m_cRef;
	CPersRefMgr*		m_pPersRefMgr;
	IDMUSProdStripMgr*	m_pStripMgr;

	LONG			m_lGutterBeginSelect;
	LONG			m_lGutterEndSelect;
	BOOL			m_bGutterSelected;	// whether the gutter select is selected, use
									// CPersRefMgr::m_bSelected for whether personality reference is selected
	BOOL			m_bSelecting;
	HCURSOR 		m_hCursor;

private:
	CPersRefItem* GetPersRefFromPoint( long lPos );
	CPersRefItem* GetPersRefFromMeasure( DWORD dwMeasure );
	LONG			m_lXPos;				// used for temp storage of xpos when doing mouse edits

	IDataObject*	m_pISourceDataObject;	// Object being dragged 
	IDataObject*	m_pITargetDataObject;
	short			m_nStripIsDragDropSource;	
	DWORD			m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD			m_dwOverDragButton; 	// Mouse button stored in IDropTarget::DragOver
	DWORD			m_dwOverDragEffect; 	// Drag effects stored in IDropTarget::DragOver
	CImageList*		m_pDragImage;			// Image used for drag operation feedback
	DWORD			m_dwDragRMenuEffect;	// Result from drag context menu
	UINT			m_cfPersRefList;			// CF_PERSREFLIST Clipboard format
	UINT			m_cfPersonality;			// CF_PERSONALITY Clipboard format
	LONG			m_lStartDragPosition;	// xpos where drag was started
	UINT			m_nLastEdit;			// resource id of last edit
	BOOL			m_fShowPersRefProps;		// if TRUE, show PersRef property page, else show group property page
	BOOL			m_fPropPageActive;		// TRUE if our property page is active
	BOOL			m_fInRightClickMenu;
	IDMUSProdPropPageManager*	m_pPropPageMgr;

};
#endif //__PERS_REFMGR_H_
