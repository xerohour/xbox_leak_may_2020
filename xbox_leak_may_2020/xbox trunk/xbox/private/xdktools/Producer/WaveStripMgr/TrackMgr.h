// TrackMgr.h : Declaration of the CTrackMgr

#ifndef __WAVE_TRACKMGR_H_
#define __WAVE_TRACKMGR_H_

#include "resource.h"		// main symbols
#include "WaveStripMgr.h"
#include "WaveStripLayer.h"
#include "TrackItem.h"
#include "PropTrackItem.h"
#include "LayerScrollBar.h"
#include "selectedregion.h"
#include "BaseMgr.h"
#include <AList.h>
#include <dsoundp.h>

interface IDirectMusicVoiceP;

// Private interface for wave track
DEFINE_GUID(IID_IPrivateWaveTrack, 0x492abe2a, 0x38c8, 0x48a3, 0x8f, 0x3c, 0x1e, 0x13, 0xba, 0x1, 0x78, 0x4e);
interface IPrivateWaveTrack : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SetVariation(
		IDirectMusicSegmentState* pSegState,
		DWORD dwVariationFlags,
		DWORD dwPChannel,
		DWORD dwIndex)=0;
    virtual HRESULT STDMETHODCALLTYPE ClearVariations(IDirectMusicSegmentState* pSegState)=0;
    virtual HRESULT STDMETHODCALLTYPE AddWave(
		IDirectSoundWave* pWave,
		REFERENCE_TIME rtTime,
		DWORD dwPChannel,
		DWORD dwIndex,
		REFERENCE_TIME* prtLength)=0;
    virtual HRESULT STDMETHODCALLTYPE DownloadWave(
		IDirectSoundWave* pWave,   // wave to download
		IUnknown* pUnk,            // performance or audio path
		REFGUID rguidVersion)=0;   // version of downloaded wave
    virtual HRESULT STDMETHODCALLTYPE UnloadWave(
		IDirectSoundWave* pWave,   // wave to unload
		IUnknown* pUnk)=0;         // performance or audio path
    virtual HRESULT STDMETHODCALLTYPE RefreshWave(
		IDirectSoundWave* pWave,   // wave to refresh
		IUnknown* pUnk,            // performance or audio path
		DWORD dwPChannel,          // new PChannel for the wave
		REFGUID rguidVersion)=0;;  // version of refreshed wave
    virtual HRESULT STDMETHODCALLTYPE FlushAllWaves()=0;
    virtual HRESULT STDMETHODCALLTYPE OnVoiceEnd(IDirectMusicVoiceP *pVoice, void *pStateData)=0;
};

// Private interface for wave track
DEFINE_GUID(IID_IPrivateWaveTrack9, 0x49095b17, 0xf46a, 0x4acb, 0xa2, 0xee, 0x1e, 0x5f, 0x58, 0xc8, 0x4e, 0x7c);
interface IPrivateWaveTrack9 : IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SetVariationMask(
		IDirectMusicSegmentState* pSegState,
		DWORD dwVariationFlags,
		DWORD dwPChannel,
		DWORD dwIndex)=0;
};

typedef struct CFProducerFile			// Used when working with CF_DMUSPROD_FILE clipboard data
{
    GUID	guidFile;
	CString strFileName;
} CFProducerFile;

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

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_CONTROL_PLAY | DMUS_TRACKCONFIG_PLAY_CLOCKTIME | DMUS_TRACKCONFIG_OVERRIDE_ALL | DMUS_TRACKCONFIG_OVERRIDE_PRIMARY | DMUS_TRACKCONFIG_FALLBACK )

class CTrackMgr;
class CWaveStrip;
class CTrackItem;
class CDllJazzDataObject;
interface IDirectMusicTrack;

void EmptyList( CTypedPtrList<CPtrList, CTrackItem*>& list );
void NormalizeList( CTrackMgr* pTrackMgr, CTypedPtrList<CPtrList, CTrackItem*>& list, REFERENCE_TIME rtOffset );

#define MAX_TICK (DMUS_PPQ << 1)
#define MINIMUM_ZOOM_LEVEL	(0.07)
#define MAX_WAVE_HEIGHT		100
#define MAX_NBR_LAYERS		99
#define HORIZ_LINE_HEIGHT	1

#define MIN_LOOP_LENGTH		5
#define MAX_LOOP_LENGTH		524286

#define MIN_NBR_GRIDS_PER_SECOND		1
#define MAX_NBR_GRIDS_PER_SECOND		100
#define DEFAULT_NBR_GRIDS_PER_SECOND	1

#define VARIATION_BUTTON_HEIGHT		15
#define VARIATION_BUTTONS_HEIGHT	(VARIATION_BUTTON_HEIGHT << 1)
#define VARIATION_BUTTON_WIDTH		17
#define VARIATION_BUTTONS_WIDTH		(VARIATION_BUTTON_WIDTH << 4)
#define VARIATION_GUTTER_HEIGHT		30
#define VARIATION_GUTTER_WIDTH		 8

#define STRIP_MINIMIZE_HEIGHT		20
#define STRIP_MIN_HEIGHT			VARIATION_BUTTONS_HEIGHT

// Timer defines
#define TIMER_ZOOM_1				150
#define TIMER_ZOOM_2				151
#define TIMER_VSCROLL_LAYER_1		160
#define TIMER_VSCROLL_LAYER_2		161

// Mouse modes
#define MM_VARBTNS					1
#define MM_ZOOMBTNS					2
#define MM_LAYERBTNS				3

// Variation actions
#define VA_NONE						0					
#define VA_ENABLING					1					
#define VA_DISABLING				2					

// Chunk ID used for wave track design data
#define DMUS_FOURCC_WAVEPART_UI_LIST        mmioFOURCC('w','p','l','u')
#define DMUS_FOURCC_WAVEPART_UI_CHUNK       mmioFOURCC('w','p','c','u')
#define DMUS_FOURCC_WAVEITEM_UI_CHUNK		mmioFOURCC('w','v','c','u')
#define DMUS_FOURCC_COPYPASTE_UI_CHUNK		mmioFOURCC('c','p','c','u')

// Part State flags
#define PARTUI_ACTIVESTRIP		0x00000001

struct ioWavePartUI
{
	ioWavePartUI()
	{
		m_dwVariationBtns = 0xFFFFFFFF;
		m_dwOldVariationBtns = m_dwVariationBtns;
		m_dblVerticalZoom = 0.4;
		m_lVerticalScrollYPos = 0;
		m_lStripHeight = VARIATION_BUTTONS_HEIGHT + (2 * long(MAX_WAVE_HEIGHT * m_dblVerticalZoom + HORIZ_LINE_HEIGHT));
		m_nStripView = SV_NORMAL;
		m_nSnapToRefTime = IDM_SNAP_GRID;
		m_nSnapToMusicTime = IDM_SNAP_GRID;
		m_dwFlagsUI = 0;
		m_fVarGutterDown = true;
		m_nNbrLayers = 1;
		m_nSnapToRefTime_GridsPerSecond = DEFAULT_NBR_GRIDS_PER_SECOND;
	}

	DWORD		m_dwVariationBtns;		// Which variations are viewed
	double		m_dblVerticalZoom;		// Vertical zoom factor
	long		m_lVerticalScrollYPos;	// Vertical scroll Y position
	long		m_lStripHeight;			// Height of strip
	int			m_nStripView;			// Minimized or maximized
	short		m_nSnapToRefTime;		// 'Snap To' increment when moving waves with mouse (DMUS_TRACKCONFIG_PLAY_CLOCKTIME)
	short		m_nSnapToMusicTime;		// 'Snap To' increment when moving waves with mouse (!DMUS_TRACKCONFIG_PLAY_CLOCKTIME)
	DWORD		m_dwFlagsUI;			// various state flags (see PARTUI_ prefix)
	DWORD		m_dwOldVariationBtns;	// Previous button state (used when soloing)
	bool		m_fVarGutterDown;		// State of variation gutter
	int			m_nNbrLayers;			// Numbers of layers
	short	    m_nSnapToRefTime_GridsPerSecond;
};

struct ioWaveItemUI
{
	ioWaveItemUI()
	{
		m_nLayerIndex = 0;
		m_dwLoopStartUI = 0;
		m_dwLoopEndUI = 0;
		m_fLockLoopLengthUI = FALSE;
		m_fLockEndUI = FALSE;
		m_fLockLengthUI = FALSE;
		m_fSyncDuration = TRUE;
	}

	int		m_nLayerIndex;				// Wave displays in this layer
    DWORD   m_dwLoopStartUI;			// Start point for a looping wave
    DWORD   m_dwLoopEndUI;				// End point for a looping wave
	BOOL	m_fLockLoopLengthUI;		// State of 'Lock Loop Length' button (Loop Tab)
	BOOL	m_fLockEndUI;				// State of 'Lock End' button (FileRef Tab)
	BOOL	m_fLockLengthUI;			// State of 'Lock Length' button (FileRef Tab)
	BOOL	m_fSyncDuration;			// Always sync duration to actual length of wave
};

struct ioCopyPasteUI
{
	ioCopyPasteUI()
	{
		m_rtPasteOverwriteRange = 0;	
		m_rtOffset = 0;
		m_rtStartTime = 0;
		m_lRefTimeSec = 0;
		m_lRefTimeGrid = 0;
		m_lRefTimeMs = 0;
		m_mtPasteOverwriteRange = 0;	
		m_mtOffset = 0;
		m_mtStartTime = 0;
		m_lMusicTimeBeat = 0;
		m_lMusicTimeGrid = 0;	
		m_lMusicTimeTick = 0;	
		m_nStartLayerIndex = 0;	
		m_nMinLayerIndex = 0;	
		m_nMaxLayerIndex = 0;
		m_fRefTimeTrack = false;
	}

	REFERENCE_TIME	m_rtPasteOverwriteRange;// Total clock time of list (REFERENCE_TIME)
	REFERENCE_TIME	m_rtOffset;				// Offset from start of wave (REFERENCE_TIME)
	REFERENCE_TIME  m_rtStartTime;			// Start time of first item in list (REFERENCE_TIME)
	long			m_lRefTimeSec;			// Beat offset of first item in list
	long			m_lRefTimeGrid;			// Grid offset of first item in list
	long			m_lRefTimeMs;			// Tick offset of first item in list
	MUSIC_TIME		m_mtPasteOverwriteRange;// Total clock time of list (MUSIC_TIME)
	MUSIC_TIME		m_mtOffset;				// Offset from start of wave (REFERENCE_TIME)
	MUSIC_TIME		m_mtStartTime;			// Start time of first item in list (MUSIC_TIME)
	long			m_lMusicTimeBeat;		// Beat offset of first item in list
	long			m_lMusicTimeGrid;		// Grid offset of first item in list
	long			m_lMusicTimeTick;		// Tick offset of first item in list
	int				m_nStartLayerIndex;		// Layer of first item in list
	int				m_nMinLayerIndex;		// First layer 
	int				m_nMaxLayerIndex;		// Last layer 
	bool			m_fRefTimeTrack;		// Time is REFERENCE_TIME
};


struct QueuedWave
{
	QueuedWave()
	{
		pIDSWave = NULL;
		lDownloadCount = 0;
		memset( &guidVersion, 0, sizeof(GUID) );
	}

	~QueuedWave()
	{
		RELEASE( pIDSWave );
	}

	IDirectSoundWave* pIDSWave;			// Wave object
	GUID			  guidVersion;		// Wave version
    long			  lDownloadCount;	// Unload if less than zero, download if greater than zero
};


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr class

class ATL_NO_VTABLE CTrackMgr : 
	public CBaseMgr,
	public CComCoClass<CTrackMgr, &CLSID_WaveMgr>,
	public IWaveMgr
{
friend CWaveStrip;

public:
	CTrackMgr();
	~CTrackMgr();

DECLARE_REGISTRY_RESOURCEID(IDR_TRACKMGR)

BEGIN_COM_MAP(CTrackMgr)
	COM_INTERFACE_ENTRY_IID(IID_IWaveMgr,IWaveMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()

// IDMUSProdStripMgr overrides

// IWaveMgr methods
	HRESULT STDMETHODCALLTYPE IsMeasureBeatOpen( long lMeasure, long lBeat );

// IDMUSProdStripMgr overrides
	HRESULT STDMETHODCALLTYPE IsParamSupported( REFGUID guidType );
	HRESULT STDMETHODCALLTYPE GetParam( REFGUID guidType, MUSIC_TIME mtTime, MUSIC_TIME* pmtNext, void* pData );
	HRESULT STDMETHODCALLTYPE SetParam( REFGUID guidType, MUSIC_TIME mtTime, void* pData );
	HRESULT STDMETHODCALLTYPE OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( STRIPMGRPROPERTY stripMgrProperty, VARIANT variant );

// IPersist overrides

// IPersistStream overrides
	STDMETHOD(Load)( IStream* pIStream );
	STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );

// IDMUSProdPropPageObject overrides
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

protected:
	// Overrides of CBaseMgr
	bool	RecomputeTimes();			 // Returns true if anything changed
	HRESULT	SyncWithDirectMusic( void ); // Returns S_OK if succeeded, S_FALSE if no track, E_* if failed.
	
// Performs action on CTrackMgr
	void	InsertByAscendingPChannel( CWaveStrip *pWaveStrip );

// Performs action on all CWaveStrips
	void	AllStrips_Invalidate();
	void	AllStrips_ResetIndexes();
	void	AllStrips_AddToTimeline();
	void	AllStrips_RemoveFromTimeline();
	void	AllStrips_DeleteAllParts();
	void	AllStrips_DeleteSelectedParts();
	void	AllStrips_GetWaveInfo();
	void	AllStrips_SwitchTimeBase();
	BOOL	AllStrips_SetNbrGridsPerSecond( short nNbrGridsPerSecond );

// Performs action on one CWaveStrip
	HRESULT AddStripToTimeline( CWaveStrip* pWaveStrip, int nPosition );
	HRESULT RemoveStripFromTimeline( CWaveStrip* pWaveStrip );
	HRESULT OnNewPart();
	HRESULT DeletePart( CWaveStrip* pWaveStrip );
	HRESULT RepositionPart( CWaveStrip* pWaveStrip );

// Download/Unload waves
	QueuedWave* GetQueuedWave( IDirectSoundWave* pIDSWave, GUID guidVersion );
	void UnloadQueuedWaves();
	void DownloadQueuedWaves();
	void FlushAllWaves();

public:
// General helpers
	REFERENCE_TIME GetSegmentLength();	// MUSIC_TIME or REFERENCE_TIME
	HRESULT GetPChannelName( DWORD dwPChannel, CString& strPChannelName );
	HRESULT GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject ); 
	bool    IsRefTimeTrack();
	bool	IsValidLockID( DWORD dwLockID );
	bool	StripExists( DWORD dwPChannel );
	DWORD	GetGreatestUsedPChannel();
	HRESULT SetAuditionVariations( bool fForceClear );
	CWaveStrip* GetActiveStripInThisTrack();
	CWaveStrip* GetWaveStrip( DWORD dwPChannel, DWORD dwIndex );
	CBitmap* GetLockBitmap( BOOL fLocked );
	void	UpdateStatusBarDisplay( void );

// Download/Unload waves
	HRESULT QueueWaveForDownload( CTrackItem* pItem );
	HRESULT QueueWaveForUnload( CTrackItem* pItem );

// Performs action on CTrackMgr
	void	SetDirtyFlag( bool fDirty );

// Performs action on one CWaveStrip
	HRESULT AddPart( DWORD dwPChannel );

	HRESULT	RefTimeToMinSecGridMs( CWaveStrip* pWaveStrip, REFERENCE_TIME rtTime, long* plMinute, long* plSecond, long* plGrid, long* plMillisecond );
	HRESULT	MinSecGridMsToRefTime( CWaveStrip* pWaveStrip, long lMinute, long lSecond, long lGrid, long lMillisecond, REFERENCE_TIME* prtTime );
	HRESULT	ClocksToMeasureBeatGridTick( MUSIC_TIME mtTime, long* plMeasure, long* plBeat, long* plGrid, long* plTick );
	HRESULT	MeasureBeatGridTickToClocks( long lMeasure, long lBeat, long lGrid, long lTick, MUSIC_TIME* pmtTime );
	HRESULT ForceBoundaries( long lMeasure, long lBeat, MUSIC_TIME* pmtTime );
	DWORD	GetGroupBits();
	HRESULT SaveTrackHeader( IDMUSProdRIFFStream* pIRiffStream );

	HRESULT	UnknownTimeToMeasureBeatGridTick( REFERENCE_TIME rtTime, long* plMeasure, long* plBeat, long* plGrid, long* plTick );
	HRESULT	MeasureBeatGridTickToUnknownTime( long lMeasure, long lBeat, long lGrid, long lTick, REFERENCE_TIME* prtTime );
	HRESULT	SnapUnknownTimeToBeat( REFERENCE_TIME rtTime, REFERENCE_TIME* prtBeatTime );
	HRESULT ClocksToUnknownTime( MUSIC_TIME mtTime, REFERENCE_TIME* prtTime );
	HRESULT UnknownTimeToClocks( REFERENCE_TIME rtTime, MUSIC_TIME* pmtTime );
	HRESULT RefTimeToUnknownTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtTime );
	HRESULT UnknownTimeToRefTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtTime );

public:
	CString		m_strLastWaveName;
	bool		m_fInAllTracksAdded;
	bool		m_fWasRuntimeTrack;

protected:
	IDirectMusicPerformance*	m_pIDMPerformance;
	IDirectMusicSegmentState*	m_pIDMSegmentState;
	CWaveStrip*					m_pWaveStripForPropSheet;
	HANDLE						m_hStatusBar;
	bool						m_fForceVariationBtnsClear;

	// Fields that are persisted (DirectMusic wave track data)
	long		m_lVolume;
	DWORD		m_dwTrackFlagsDM;		// Various flags pertaining to entire track	

	CTypedPtrList<CPtrList, CWaveStrip*> m_lstWaveStrips;
	CTypedPtrList<CPtrList, QueuedWave*> m_lstQueuedWaves;	// For download/unload
};


/////////////////////////////////////////////////////////////////////////////
// CWaveRectItem class

class CWaveRectItem : public AListItem
{
friend class CWaveStrip;

public:
	CWaveRectItem( RECT* pRect ){ m_rect = *pRect; }
	virtual ~CWaveRectItem() {}
	CWaveRectItem* GetNext() { return (CWaveRectItem*) AListItem::GetNext(); }

protected:
	CRect		m_rect;		// Wave rectangle
};


/////////////////////////////////////////////////////////////////////////////
// CWaveRectList class

class CWaveRectList : public AList
{
public:
	virtual ~CWaveRectList() { if(m_pHead != NULL) delete m_pHead; } 
    CWaveRectItem *GetHead() const { return (CWaveRectItem *)AList::GetHead(); };
    CWaveRectItem *RemoveHead() { return (CWaveRectItem *)AList::RemoveHead(); };
};


/////////////////////////////////////////////////////////////////////////////
// CWaveStrip class

class CWaveStrip :
	public CBaseStrip
{
friend CTrackMgr;

public:
	CWaveStrip( CTrackMgr* pTrackMgr );
	~CWaveStrip();

public:
// IDMUSProdStrip overrides
	HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
	HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar overrides
	HRESULT STDMETHODCALLTYPE FBDraw( HDC hDC, STRIPVIEW sv );
	HRESULT STDMETHODCALLTYPE FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdTimelineEdit overrides
	HRESULT STDMETHODCALLTYPE Copy( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject* pITimelineDataObject );
	HRESULT STDMETHODCALLTYPE CanInsert( void );

// IDropSource overrides

// IDropTarget overrides
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

// General helper overrides
	HRESULT	PostRightClickMenu( POINT pt );

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

protected:
	HRESULT	DrawMinimizedStrip( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT	DrawMaximizedStrip( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	void	DrawHorizontalLines( HDC hDC, const RECT &rectClip );
	void	DrawClockVerticalLines( HDC hDC, long lXOffset );
	void	DrawVariationButtonBar( HDC hDC );
	void	HighlightTimelineRange( HDC hDC, long lXOffset );
	void	InvalidateVariationBtns();
	void	InvalidateWaves();
	void	InvalidateFBar();
	void	UnselectGutterRange();
	HRESULT OnCreate();
	HRESULT OnDestroy();

// IDropTarget helpers 
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteReplace( long firstMeasure, long firstBeat, long lastMeasure, long lastBeat);
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lXPos, bool bDropNotPaste, BOOL &fChanged );

// IDropSource helpers
	HRESULT CreateDataObject(IDataObject**, long lXPos, long lYPos );
	HRESULT	DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos, long lYPos );

// IDMUSProdPropPageObject helpers
	BOOL SetData_TabFileRef( CPropTrackItem* pPropItem, CTrackItem* pItem );
	BOOL SetData_TabLoop( CPropTrackItem* pPropItem, CTrackItem* pItem );
	BOOL SetData_TabPerformance( CPropTrackItem* pPropItem, CTrackItem* pItem );
	BOOL SetData_TabVariations( CPropTrackItem* pPropItem, CTrackItem* pItem );

// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnLButtonUp( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnMouseMove( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnChar( WPARAM wParam );
	
	HRESULT OnLButtonDblClkVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnLButtonDownVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnLButtonUpVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnRButtonDownVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnRButtonUpVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnMouseMoveVarBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	
	HRESULT OnLButtonDownZoomBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnLButtonUpZoomBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);

	HRESULT OnLButtonDownLayerBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnLButtonUpLayerBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnRButtonDownLayerBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnRButtonUpLayerBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );
	HRESULT OnMouseMoveLayerBtns( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

	int	XYPosToVariation( long lXPos, long lYPos );

	CTrackItem* GetItemFromPoint( long lXPos, long lYPos );
	bool SelectItemsInSelectedRegions();
	void SelectRegionsFromSelectedItems();	
	BOOL RegisterClipboardFormats();

	CTrackItem* GetNextItemInLayer( POSITION pos, CWaveStripLayer* pLayer );
	CTrackItem* GetNextSelectedItemInLayer( POSITION pos, CWaveStripLayer* pLayer );
	CTrackItem* GetNextItem( POSITION pos );
	CTrackItem* GetNextSelectedItem( POSITION pos );
	CTrackItem* GetFirstSelectedItem();
	
	// Overrides of CBaseStrip
	bool	IsSelected();	// Returns true if any items are selected
	bool	IsEmpty();		// Returns false if there are any items
	bool	SelectSegment( MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime );

	// From CTrackMgr
	void	UnselectAll();
	void	InsertByAscendingTime( CTrackItem *pItem, BOOL fPaste );
	bool	DeleteBetweenMeasureBeats( long lmStart, long lbStart, long lmEnd, long lbEnd );
	HRESULT OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	HRESULT	SaveSelectedItems( IStream* pIStream, REFERENCE_TIME rtOffset, CTrackItem* pItemAtDragPoint );
	HRESULT OnEnableVariations( WORD wID );
	void	MarkSelectedItems( DWORD flags );
	void	UnMarkItems( DWORD flags );
	void	DeleteMarked( DWORD flags );
	void	DeleteSelectedItems();

	// New methods
	void	CleanUp();
	HRESULT	GetWaveInfo();
	void	SwitchTimeBase();
	void	GetFBName( CString& strFBName );
	void	OnShowStripProperties();
	void	OnShowItemProperties();
	void	SetZoom( double dblVerticalZoom );
	void	TimelineToWavePartUI();
	void	WavePartUIToTimeline();
	void	MergeVariations();
	DWORD	MapVariations( DWORD dwOrigVariations, DWORD dwVariationMap[32] );
	REFERENCE_TIME CalculatePasteTime( MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI );
	REFERENCE_TIME CalculateDropTime( MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI );
	void	RefreshPropertyPage( void );
	void	UpdatePlayingVariation( bool fRefreshDisplayIfNecessary = true );
	void	KillVariationTimer( void );
	void	EnableVariationTimer( void );
	IDMUSProdNode *EnsureFileIsOpen( IDMUSProdTimelineDataObject* pITimelineDataObject );
	HRESULT LoadCF_DMUSPROD_FILE( IStream* pIStream, CFProducerFile* pProducerFile );

	HRESULT LoadList( CTypedPtrList<CPtrList, CTrackItem*>& list, IStream* pIStream );
	HRESULT SaveList( CTypedPtrList<CPtrList, CTrackItem*>& list, IStream* pIStream );
	HRESULT LoadStrip( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent );
	HRESULT SaveStrip( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveStripDesignData( IDMUSProdRIFFStream* pIRiffStream );

protected:
	// Layers
	HRESULT	RefreshVerticalScrollBarUI();
	void SetVerticalScrollYPos( long lNewVertScrollYPos );
	CWaveStripLayer* YPosToLayer( long lYPos, bool fNewLayerIsValid );
	int YPosToLayerIndex( long lYPos );
	int GetNbrLayers();
	bool SelectLayer( CWaveStripLayer* pLayer );
	bool IsLayerEmpty( CWaveStripLayer* pLayer );
	BOOL SyncLayerSelectFlag( CWaveStripLayer* pLayer );
	HRESULT CanDeleteLayers();
	HRESULT DeleteSelectedLayers();
	HRESULT InsertTheLayer( int nLayerIndex );
	HRESULT InsertLayers( int nLayerIndex );

	// Snap To
	void DoGridsPerSecond();
	void SetSnapTo( WORD wID );
	HRESULT SnapXPosToUnknownTime( long lXPos, REFERENCE_TIME* prtSnapTime );
	HRESULT SnapUnknownTime( REFERENCE_TIME rtTime, REFERENCE_TIME* prtSnapTime );
	HRESULT	SnapUnknownTimeToDisplayPosition( REFERENCE_TIME rtTime, long* plPosition );
	HRESULT	SnapPositionToStartEndClocks( long lPosition, MUSIC_TIME* pmtStart, MUSIC_TIME* pmtEnd );
	HRESULT SnapUnknownToNextIncrement( REFERENCE_TIME rtTime, REFERENCE_TIME* prtSnapTime );

public:
	void OnVScrollFromScrollbar( UINT nSBCode, UINT nPos );
	CWaveStripLayer* CreateLayerForIndex( int nLayerIndex );
	CWaveStripLayer* IndexToLayer( int nLayerIndex );
	int LayerToIndex( CWaveStripLayer* pLayer );
	void IndexToLayerYCoords( int nLayerIndex, RECT* pRect );
	HRESULT InsertItem( CTrackItem* pItem );
	void OnVariationTimer( void );

	bool IsOneGrid();
	DWORD GetPChannel();
	void RefreshAllWaves();
    void GetReferencedNodes( DWORD *pdwIndex, DMUSProdReferencedNodes *pDMUSProdReferencedNodes );

// SelectedRegion list helpers
	void CListSelectedRegion_AddRegion( long lXPos );
	void CListSelectedRegion_AddRegion( CTrackItem* pItem );
	void CListSelectedRegion_ShiftAddRegion( long lXPos );
	void CListSelectedRegion_ToggleRegion( long lXPos );
	bool CListSelectedRegion_Contains( REFERENCE_TIME rtUnknownTime );

protected:
	// Fields used for UI 
	CTrackMgr*		 m_pTrackMgr;
	CTrackItem*		 m_pItemToToggle;
	CWaveStripLayer* m_pLayerForShiftClick;
	CLayerScrollBar	 m_VerticalScrollBar;
	UINT			 m_nVerticalScrollTimer;
	UINT			 m_cfWaveTrack;			// CF_WAVETRACK Clipboard format
	UINT			 m_cfWave;				// CF_DLS_WAVE clipboard format
	UINT			 m_cfDMUSProdFile;		// CF_DMUSPROD_FILE clipboard format
	long			 m_lYPos;
	bool			 m_fLeftMouseDown;
	WORD			 m_wMouseMode;			// MM_ flags
	DWORD			 m_dwSourceVariationBtns;
	DWORD			 m_dwScrollTick;

	// Reference counter for bitmaps
	static long		sm_lBitmapRefCount;

	// Lock buttons
	static CBitmap	sm_bmpLocked;
	static CBitmap	sm_bmpUnlocked;

	// Variation Button bar
	static CBitmap	sm_bmpVarBtnUp;
	static CBitmap	sm_bmpVarBtnDown;
	static CBitmap	sm_bmpVarInactiveBtnUp;
	static CBitmap	sm_bmpVarInactiveBtnDown;
	static CBitmap	sm_bmpVarGutterBtnUp;
	static CBitmap	sm_bmpVarGutterBtnDown;

	// State variables for the variation selection bar
	int				m_nVarAction;
	int				m_nLastVariation;
	int				m_nSoloVariation;
	bool			m_fVarSolo;
	DWORD			m_dwPlayingVariation;
	bool			m_fVariationsTimerActive;

	// Layer status variables
	static int		sm_nLayerBtnWidth;
	BOOL			m_fLayerSelected;

	// Zoom status variables
	static CBitmap	sm_bmpZoomInBtnUp;
	static CBitmap	sm_bmpZoomInBtnDown;
	static CBitmap	sm_bmpZoomOutBtnUp;
	static CBitmap	sm_bmpZoomOutBtnDown;
	static int		sm_nZoomBtnHeight;
	static int		sm_nZoomBtnWidth;
	int				m_nWaveHeight;			// Wave height based on vertical zoom
	bool			m_fZoomInDown;
	bool			m_fZoomOutDown;

	// Vertical scroll variables
	long			m_lVerticalScrollTopYPos;

	// Fields that are persisted (DirectMusic wave track data)
	DWORD			m_dwVariationsMask;		// Which variations are enabled
	DWORD			m_dwPChannel;			// PChannel number
	DWORD			m_dwIndex;				// Index for distinguishing multiple parts on same PChannel
	long			m_lVolume;				// Gain, in 1/100th of dB. Note: All gain values should be negative
	DWORD			m_dwLockToPart;			// Part ID to lock to
	DWORD			m_dwPartFlagsDM;		// Various flags pertaining to part (strip)

	CTypedPtrList<CPtrList, CTrackItem*> m_lstTrackItems;

	// Fields that are only persisted in design files
	CString			m_strStripName;			// Used by FBDraw()
	ioWavePartUI	m_StripUI;

	CTypedPtrList<CPtrList, CWaveStripLayer*> m_lstLayers;
};


#endif //__WAVE_TRACKMGR_H_
