// BandMgr.h : Declaration of the CBandMgr

#ifndef __BANDMGR_H_
#define __BANDMGR_H_

#include "resource.h"		// main symbols
#include "BandStripMgr.h"
#include "timeline.h"
#include "PropBand.h"
#include <DMUSProd.h>
#include <afxtempl.h>
#include <BandEditor.h>
#include <mmsystem.h>

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0
#define NOTE_TO_CLOCKS(note, ppq) ( (ppq)*4 /(note) )
#define CF_BANDTRACK "DMUSProd v.1 Band Track"

#define TRACKCONFIG_VALID_MASK (  DMUS_TRACKCONFIG_CONTROL_ENABLED | DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_PLAY_CLOCKTIME | DMUS_TRACKCONFIG_CONTROL_PLAY | DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT | DMUS_TRACKCONFIG_TRANS1_TOSEGSTART)

// Used by SaveSelectedBands()
#define SSB_COPY	1
#define SSB_DRAG	2

//class CBandComponent;
class CBandStrip;
class CBandItem;
interface IDirectMusicTrack;
interface IDMUSProdRIFFStream;
interface IDirectMusicPerformance;

HRESULT MeasureBeatToBeats( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, DWORD dwIndex, long lMeasure, long lBeat, long &lNumBeats );
HRESULT BeatsToMeasureBeat( IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, DWORD dwIndex, long lNumBeats, long &lMeasure, long &lBeat );
POSITION GetFirstValidBand( POSITION pos, const CTypedPtrList<CPtrList, CBandItem*> &lstBands );
bool GetBandForPChannelAndTime( const CTypedPtrList<CPtrList, CBandItem*> &lstBands, DWORD dwPChannel, MUSIC_TIME mtTime, CBandItem **ppBandItem, CBandItem **ppBandItemNext );
/////////////////////////////////////////////////////////////////////////////
// CBandMgr
class ATL_NO_VTABLE CBandMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBandMgr, &CLSID_BandMgr>,
	public IBandMgr,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdBandMgrEdit,
	public IDMUSProdPropPageObject
{
friend CBandStrip;

public:
	CBandMgr();
	~CBandMgr();

public:
DECLARE_REGISTRY_RESOURCEID(IDR_BANDMGR)

BEGIN_COM_MAP(CBandMgr)
	COM_INTERFACE_ENTRY_IID(IID_IBandMgr,IBandMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY_IID(IID_IDMUSProdBandMgrEdit,IDMUSProdBandMgrEdit)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

// IDMUSProdStripMgr methods
	HRESULT STDMETHODCALLTYPE IsParamSupported( REFGUID guidType );
	HRESULT STDMETHODCALLTYPE GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData );
	HRESULT STDMETHODCALLTYPE SetParam( REFGUID guidType, MUSIC_TIME mtTime, void* pData );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	HRESULT STDMETHODCALLTYPE GetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT* pVariant );
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

// IBandMgr methods
	HRESULT STDMETHODCALLTYPE IsMeasureBeatOpen( DWORD dwMeasure, BYTE bBeat );

// IPersist methods
	STDMETHOD(GetClassID)( CLSID* pClsId );

// IPersistStream methods
	STDMETHOD(IsDirty)();
	STDMETHOD(Load)( IStream* pIStream );
	STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
	STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

// IDMUSProdBandMgrEdit methods
    HRESULT STDMETHODCALLTYPE DisplayEditBandButton( DWORD dwPChannel, LONG lXPos, LONG lYPos );

// IDMUSProdPropPageObject methods
	HRESULT STDMETHODCALLTYPE GetData( void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

protected:
	HRESULT STDMETHODCALLTYPE OnDataChanged();
	BSTR	CreateUniqueName();
	void	EmptyBandList();
	void	InsertByAscendingTime(CBandItem *pBand, BOOL fPaste);
	void	RecomputeBandMeasureBeats();
	BOOL	RecomputeBandTimes();
	void	SetDocRootForAllBands();

	BOOL	SelectSegment(long begintime, long endtime);

protected:
	CTypedPtrList<CPtrList, CBandItem*> m_lstBands;
	IDMUSProdFramework* 		m_pDMProdFramework;
	IDMUSProdNode*				m_pDMProdSegmentNode;
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	IDataObject*				m_pCopyDataObject;
	DWORD						m_dwGroupBits;
	DWORD						m_dwOldGroupBits;
	DWORD						m_dwTrackExtrasFlags;
	DWORD						m_dwProducerOnlyFlags;
	IDirectMusicTrack*			m_pIDMTrack;
	BOOL						m_fDirty;
	bool						m_bAutoDownload;
	IDirectMusicPerformance*	m_pIPerformance;
	CBandStrip*					m_pBandStrip;
	CBandItem*					m_pBandItemForEditBandButton;
	DWORD						m_dwPChannelForEditBandButton;
	bool						m_fNoUpdateSegment;
	IDirectMusicSegmentState8*	m_pSegmentState;

public:
	IDMUSProdTimeline*			m_pTimeline;

protected:
	void CleanUpPropBand( CPropBand* pPropBand );
	HRESULT CreateBand( DWORD dwMeasure, BYTE bBeat, CBandStrip* pBandStrip, CBandItem*& rpBand );
	HRESULT CreateBand( long lXPos, CBandStrip* pBandStrip, CBandItem*& rpBand );
	HRESULT SaveBandWrapper( IStream* pIStream, IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveBand( IStream* pIStream, IDMUSProdRIFFStream* pIRiffStream, CBandItem* pBandItem, MUSIC_TIME mtOffset, short nReason );
	HRESULT BuildDirectMusicBandList(IStream* pStream,
										      IDMUSProdRIFFStream *pIDirectMusicStream,
										      MMCKINFO& ckParent,
											  BOOL& fChanged,
											  bool fPaste = false,
											  MUSIC_TIME mtPasteTime = 0);
	HRESULT ExtractBand(IStream* pStream, 
								 IDMUSProdRIFFStream *pIDirectMusicStream, 
								 MMCKINFO& ckParent,
								 bool fPaste = false,
								 MUSIC_TIME mtPasteTime = 0);
	HRESULT DeleteSelectedBands();
	HRESULT SaveSelectedBands(LPSTREAM pIStream, MUSIC_TIME mtOffset, short nReason);

// general helpers
	void MarkSelectedBands(DWORD flags);
	void UnMarkBands(DWORD flags);
	void DeleteMarked(DWORD flags);
	void UnselectAllKeepBits();
	void UnselectAll();
	void SelectAll();
	BOOL RemoveItem( CBandItem* pItem );
	BOOL CanCycle( long lXPos );
	HRESULT CycleBands( long lXPos );
	void UpdateBandNodesWithAudiopath( void );

public:
// general helpers
	HRESULT ClocksToMeasureBeatTick( MUSIC_TIME mtTime, long* plMeasure, long* plBeat, long* plTick );
	HRESULT MeasureBeatTickToClocks( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmtTime );
	HRESULT ForceBoundaries( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmtTime );
	DWORD	GetGroupBits();
	bool    IsRefTimeTrack();

protected:
// Misc
	BOOL	IsSelected();	// returns if one or more bands are selected.
	CBandItem* FirstSelectedBand();
	CBandItem* FirstSelectedRealBand();
	CBandItem* CurrentlySelectedBand();
	HRESULT STDMETHODCALLTYPE AddBand(  CBandItem* pBand  );
	CBandItem* FindBand(long lMeasure, long lBeat) const;
	CBandItem* FindBandAtTime(MUSIC_TIME mtTime) const;
	HRESULT SyncWithDirectMusic();
	void GetBoundariesOfSelectedBands( long *plStart, long *plEnd );
	BOOL DeleteBetweenTimes( long lStart, long lEnd );
	MUSIC_TIME GetNextGreatestUniqueTime( long lMeasure, long lBeat, long lTick );
};


class CBandStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDropSource,
	public IDropTarget,
	public IDMUSProdPropPageObject
{
friend CBandMgr;

public:
	CBandStrip( CBandMgr* pBandMgr );
	~CBandStrip();

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
	HRESULT STDMETHODCALLTYPE Cut( IDMUSProdTimelineDataObject *pIDataObject );
	HRESULT STDMETHODCALLTYPE Copy( IDMUSProdTimelineDataObject *pIDataObject );
	HRESULT STDMETHODCALLTYPE Paste( IDMUSProdTimelineDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanCut( void );
	HRESULT STDMETHODCALLTYPE CanCopy( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject* pIDataObject );
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
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pIDataObject, MUSIC_TIME mtPasteTime, BOOL bDropNotEditPaste, BOOL &fChanged);

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

// Misc. functions
	HRESULT AddBandNode( IDMUSProdNode* pIBandNode, MUSIC_TIME mtTime );
	HRESULT RightClickMenuCanSelectAll();
	void OnDragRMenuMove(  );
	void OnDragRMenuCopy(  );
	void OnDragRMenuCancel(  );
	IDMUSProdNode* GetStripNode() { return m_pStripNode; }
	IDMUSProdFramework* GetFramework()
	{ 
		return (m_pBandMgr) ? m_pBandMgr->m_pDMProdFramework : NULL;
	}

protected:
// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	BOOL DoDragDrop();

	long				m_cRef;
	LONG				m_lBeginSelect;
	LONG				m_lEndSelect;
	IDMUSProdStripMgr*	m_pStripMgr;
	BOOL				m_bGutterSelected;	// whether the gutter select is selected
	BOOL				m_bSelecting;
	BOOL				m_bContextMenuPaste;

private:
	HRESULT		SelectRange(CBandItem* pBand);
	HRESULT		ShowPropertySheet(IDMUSProdTimeline*);
	void		UnselectGutterRange( void );
	CBandItem*  GetBandFromPoint( long lPos );
	CBandItem*  GetTopBandFromMeasureBeat( DWORD dwMeasure, BYTE bBeat );
	long		GetBeatWidth( DWORD dwMeasure );
	LONG		m_lXPos;				// used for temp storage of xpos when doing mouse edits

	IDataObject*m_pISourceDataObject;	// Object being dragged 
	IDataObject*m_pITargetDataObject;
	short		m_nStripIsDragDropSource;	
	DWORD		m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD		m_dwOverDragButton; 	// Mouse button stored in IDropTarget::DragOver
	DWORD		m_dwOverDragEffect; 	// Drag effects stored in IDropTarget::DragOver
	CImageList* m_pDragImage;			// Image used for drag operation feedback
	DWORD		m_dwDragRMenuEffect;	// Result from drag context menu
	UINT		m_cfBand;				// Clipboard format
	UINT		m_cfBandTrack;			// Clipboard format
	LONG		m_lStartDragPosition;	// xpos where drag was started
	UINT		m_nLastEdit;			// resource id of last edit
	BOOL		m_fShowBandProps;		// if TRUE, show Band property page, else show group property page
	BOOL		m_fPropPageActive;		// TRUE if our property page is active
	BOOL		m_fSingleSelect;		
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	IDMUSProdNode* m_pStripNode;

public:
	CBandMgr*	m_pBandMgr;
};
#endif //__BANDMGR_H_
