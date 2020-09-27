// TrackMgr.h : Declaration of the CTrackMgr

#ifndef __SCRIPT_TRACKMGR_H_
#define __SCRIPT_TRACKMGR_H_

#include "resource.h"		// main symbols
#include "ScriptStripMgr.h"
#include "TrackItem.h"
#include "selectedregion.h"
#include "BaseMgr.h"
#include "SegmentGUIDs.h"

#ifndef REFCLOCKS_PER_MINUTE
#define REFCLOCKS_PER_MINUTE 600000000
#endif

#ifndef REFCLOCKS_PER_SECOND
#define REFCLOCKS_PER_SECOND 10000000
#endif

#ifndef REFCLOCKS_PER_MILLISECOND
#define REFCLOCKS_PER_MILLISECOND 10000
#endif

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0
#define MAX_TICK (DMUS_PPQ << 1)

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_PLAY_CLOCKTIME | DMUS_TRACKCONFIG_PLAY_ENABLED)

// Chunk ID used for wave track design data
#define DMUS_FOURCC_COPYPASTE_UI_CHUNK		mmioFOURCC('c','p','c','u')

class CTrackMgr;
class CScriptStrip;
class CTrackItem;
class CDllJazzDataObject;
interface IDirectMusicTrack;

void	EmptyList( CTypedPtrList<CPtrList, CTrackItem*>& list );
HRESULT LoadList( CTypedPtrList<CPtrList, CTrackItem*>& list, IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr, IStream* pIStream );
HRESULT LoadListItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent, IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr, CTrackItem** ppItem );
HRESULT SaveList( CTypedPtrList<CPtrList, CTrackItem*>& list, CTrackMgr* pTrackMgr, IStream* pIStream);
HRESULT SaveListItem( IDMUSProdRIFFStream* pIRiffStream, CTrackMgr* pTrackMgr, CTrackItem* pItem );
void NormalizeList( CTrackMgr* pTrackMgr, CTypedPtrList<CPtrList, CTrackItem*>& list, REFERENCE_TIME rtOffset );
HRESULT GetBoundariesOfItems( IDMUSProdTimeline *pTimeline, DWORD dwGroupBits, long & lStartTime, long & lEndTime, CTypedPtrList<CPtrList, CTrackItem*>& list);


struct ioCopyPasteUI
{
	ioCopyPasteUI()
	{
		m_rtPasteOverwriteRange = 0;	
		m_rtStartTime = 0;
		m_lRefTimeSec = 0;
		m_lRefTimeMs = 0;
		m_mtPasteOverwriteRange = 0;	
		m_mtStartTime = 0;
		m_lMusicTimeBeat = 0;
		m_lMusicTimeTick = 0;	
		m_fRefTimeTrack = false;
	}

	REFERENCE_TIME	m_rtPasteOverwriteRange;// Total clock time of list (REFERENCE_TIME)
	REFERENCE_TIME  m_rtStartTime;			// Start time of first item in list (REFERENCE_TIME)
	long			m_lRefTimeSec;			// Beat offset of first item in list
	long			m_lRefTimeMs;			// Tick offset of first item in list
	MUSIC_TIME		m_mtPasteOverwriteRange;// Total clock time of list (MUSIC_TIME)
	MUSIC_TIME		m_mtStartTime;			// Start time of first item in list (MUSIC_TIME)
	long			m_lMusicTimeBeat;		// Beat offset of first item in list
	long			m_lMusicTimeTick;		// Tick offset of first item in list
	bool			m_fRefTimeTrack;		// Time is REFERENCE_TIME
};


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr
class ATL_NO_VTABLE CTrackMgr : 
	public CBaseMgr,
	public CComCoClass<CTrackMgr, &CLSID_ScriptMgr>,
	public IScriptMgr
{
friend CScriptStrip;

public:
	CTrackMgr();
	~CTrackMgr();

DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTMGR)

BEGIN_COM_MAP(CTrackMgr)
	COM_INTERFACE_ENTRY_IID(IID_IScriptMgr,IScriptMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

// IDMUSProdStripMgr overrides
	HRESULT STDMETHODCALLTYPE IsParamSupported( REFGUID guidType );
	HRESULT STDMETHODCALLTYPE GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData );

// IScriptMgr methods
	HRESULT STDMETHODCALLTYPE IsMeasureBeatOpen( long lMeasure, long lBeat );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );
	HRESULT STDMETHODCALLTYPE GetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT* pVariant );

// IPersist overrides

// IPersistStream overrides
	STDMETHOD(Load)( IStream* pIStream );
	STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );

// IDMUSProdPropPageObject overrides
	HRESULT STDMETHODCALLTYPE GetData( void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties( void);

protected:
	// Overrides of CBaseMgr
	bool	RecomputeTimes(); // Returns true if anything changed
	void	RecomputeMeasureBeats();
	HRESULT	SyncWithDirectMusic( void ); // Returns S_OK if succeeded, S_FALSE if no track, E_* if failed.
	
	void DeleteSelectedItems();
	HRESULT SaveSelectedItems( IStream* pIStream, REFERENCE_TIME rtOffset, CTrackItem* pScriptAtDragPoint );
	HRESULT LoadTrackItem( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent, CTrackItem** ppItem );
	HRESULT SaveTrackItem( IDMUSProdRIFFStream* pIRiffStream, CTrackItem* pItem );

// general helpers
	void	MarkSelectedItems(DWORD flags);
	void	UnMarkItems(DWORD flags);
	void	DeleteMarked(DWORD flags);
	void	UnselectAll();
	void	SelectAll();
	bool	RemoveItem( CTrackItem* pItem );
	void	InsertByAscendingTime( CTrackItem *pItem, BOOL fPaste );
	MUSIC_TIME GetNextGreatestUniqueTime( long lMeasure, long lBeat, long lTick );
	CTrackItem* FirstSelectedItem();
	bool	DeleteBetweenMeasureBeats(long lmStart, long lbStart, long lmEnd, long lbEnd );
	HRESULT CycleItems( long lXPos );
	HRESULT CallSelectedRoutines();

public:
	bool    IsRefTimeTrack();
	REFERENCE_TIME GetSegmentLength();	// MUSIC_TIME or REFERENCE_TIME
	HRESULT	RefTimeToMinSecMs( REFERENCE_TIME rtTime, long* plMinute, long* plSecond, long* plMillisecond );
	HRESULT	MinSecMsToRefTime( long lMinute, long lSecond, long lMillisecond, REFERENCE_TIME* prtTime );
	HRESULT	ClocksToMeasureBeatTick( MUSIC_TIME mTime, long* plMeasure, long* plBeat, long* plTick );
	HRESULT	MeasureBeatTickToClocks( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmTime );
	HRESULT ForceBoundaries( long lMeasure, long lBeat, long lTick, MUSIC_TIME* pmtTime );
	DWORD	GetGroupBits();

	HRESULT	UnknownTimeToMeasureBeatTick( REFERENCE_TIME rtTime, long* plMeasure, long* plBeat, long* plTick );
	HRESULT	MeasureBeatTickToUnknownTime( long lMeasure, long lBeat, long lTick, REFERENCE_TIME* prtTime );
	HRESULT	SnapUnknownTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtTime );
	HRESULT	SnapUnknownTimeToDisplayPosition( REFERENCE_TIME rtTime, long* plPosition );
	HRESULT	SnapUnknownTimeToDisplayClocks( REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime );
	HRESULT	SnapUnknownTimeToUnknownIncrements( REFERENCE_TIME rtTime, long* plMeasure, long* plBeat );
	HRESULT	SnapPositionToUnknownTime( long lPosition, REFERENCE_TIME* prtTime );
	HRESULT	SnapPositionToUnknownIncrements( long lPosition, long* plMeasure, long* plBeat );
	HRESULT	SnapPositionToStartEndClocks( long lPosition, MUSIC_TIME* pmtStart, MUSIC_TIME* pmtEnd );
	HRESULT ClocksToUnknownTime( MUSIC_TIME mtTime, REFERENCE_TIME* prtTime );
	HRESULT UnknownTimeToClocks( REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime );
	HRESULT RefTimeToUnknownTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtTime );
	HRESULT UnknownTimeToRefTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtTime );

	HRESULT FindScript( CTrackItem* pItem, CString strScriptName, IStream* pIStream, IDMUSProdNode** ppIDocRootNode );
	HRESULT SaveDMRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode, WhichLoader whichLoader );
	HRESULT SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode );
	HRESULT SaveRefPendingLoad( IDMUSProdRIFFStream* pIRiffStream, CTrackItem* pItem );

protected:
	CTypedPtrList<CPtrList, CTrackItem*> m_lstTrackItems;
	CTrackItem		m_SelectedTrackItem;
	CScriptStrip*	m_pScriptStrip;

public:
	CString			m_strLastScriptName;
};


class CScriptStrip : public CBaseStrip
{
friend CTrackMgr;

public:
	CScriptStrip( CTrackMgr* pTrackMgr );
	~CScriptStrip();

public:
// IDMUSProdStrip overrides
	HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
	HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar overrides

// IDMUSProdTimelineEdit overrides
	HRESULT STDMETHODCALLTYPE Copy( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE CanInsert( void );

// IDropSource overrides

// IDropTarget overrides
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect );

// General helper overrides
	HRESULT	PostRightClickMenu( POINT pt );

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );

protected:
	void	DrawClockVerticalLines( HDC hDC, long lXOffset );
	void	UnselectGutterRange( void );
	BOOL	CanCycle();
	void	SwitchTimeBase();

// IDropTarget helpers 
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteReplace( long firstMeasure, long firstBeat, long lastMeasure, long lastBeat);
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lXPos, bool bDropNotPaste, BOOL &fChanged );
	REFERENCE_TIME CalculatePasteTime( MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI );
	REFERENCE_TIME CalculateDropTime( MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI );

// IDropSource helpers
	HRESULT CreateDataObject(IDataObject**, long position);
	HRESULT	DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos);

// Overrides of CBaseMgr
	bool	SelectSegment(MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime);
	bool	IsSelected(); // Returns true if any items are selected
	bool	IsEmpty(); // Returns false if there are any items

// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnKeyDown( WPARAM wParam, LPARAM lParam );

// SelectedRegion list helpers
	void CListSelectedRegion_AddRegion( long lXPos );
	void CListSelectedRegion_AddRegion( CTrackItem* pItem );
	void CListSelectedRegion_ShiftAddRegion( long lXPos );
	void CListSelectedRegion_ToggleRegion( long lXPos );
	bool CListSelectedRegion_Contains( REFERENCE_TIME rtUnknownTime );

	CTrackMgr*		m_pTrackMgr;

	CTrackItem* GetItemFromPoint( long lPos );
	bool SelectItemsInSelectedRegions();
	void SelectRegionsFromSelectedItems();	
	CTrackItem* GetTopItem( long lMeasure, long lBeat );
	CTrackItem* GetNextTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );
	CTrackItem* GetNextSelectedTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );
	void AdjustTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );

	UINT			m_cfScriptTrack;			// Script track clipboard format
	UINT			m_cfScript;					// Script clipboard format
	bool			m_fLeftMouseDown;
	CTrackItem*		m_pTrackItemToToggle;
};

#endif //__SCRIPT_TRACKMGR_H_
