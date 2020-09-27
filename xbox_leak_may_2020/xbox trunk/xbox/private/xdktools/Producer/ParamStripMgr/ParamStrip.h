#ifndef __PARAMSTRIP__H_
#define __PARAMSTRIP__H_

#include "TrackMgr.h"

void EmptyList(CTypedPtrList<CPtrList, CTrackItem*>& list);
void NormalizeList(IDMUSProdTimeline *pITimeline, DWORD dwGroupBits, CTypedPtrList<CPtrList, CTrackItem*>& list, REFERENCE_TIME rtOffset);

struct ioObjectUI
{
    DWORD cbSize;           // size of the structure
    GUID m_guidInstance;    // GUID that uniquely identifies the DMO instance
};

struct ioStripUI
{
	ioStripUI()
	{
		m_dblVerticalZoom = 0.1;
		m_lStripHeight = CRV_DEFAULT_HEIGHT;
		m_lVerticalScrollYPos = 0;
		m_nSnapToRefTime = (short) IDM_SNAP_GRID;
		m_nSnapToMusicTime = (short)IDM_SNAP_GRID;
		m_nStripView = SV_MINIMIZED;
		m_dwFlagsUI = 0;
		m_nSnapToRefTime_GridsPerSecond = DEFAULT_NBR_GRIDS_PER_SECOND;
	}

	double		m_dblVerticalZoom;		// Vertical zoom factor
	long		m_lStripHeight;			// Height of strip
	long		m_lVerticalScrollYPos;	// Vertical scroll Y position
	short		m_nSnapToRefTime;		// 'Snap To' increment when moving items with mouse (DMUS_TRACKCONFIG_PLAY_CLOCKTIME)
	short		m_nSnapToMusicTime;		// 'Snap To' increment when moving items with mouse (!DMUS_TRACKCONFIG_PLAY_CLOCKTIME)
	int			m_nStripView;			// Minimized or maximized
	DWORD		m_dwFlagsUI;			// various state flags (see STRIPUI_ prefix)
	short	    m_nSnapToRefTime_GridsPerSecond;
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
		m_rtOffset = 0;
		m_mtStartTime = 0;
		m_lMusicTimeBeat = 0;
		m_lMusicTimeGrid = 0;	
		m_lMusicTimeTick = 0;	
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
	bool			m_fRefTimeTrack;		// Time is REFERENCE_TIME
};


class CParamStrip :	public CBaseStrip
{
public:
	CParamStrip( CTrackMgr* pTrackMgr, CTrackObject* pTrackObject );
	~CParamStrip();

public:
// IDMUSProdStrip overrides
	HRESULT STDMETHODCALLTYPE Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
	HRESULT STDMETHODCALLTYPE OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar overrides
	HRESULT STDMETHODCALLTYPE FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

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
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

// General helper overrides
	HRESULT	PostRightClickMenu( POINT pt );

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

	// Helpers
	HRESULT LoadStrip( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent );
	HRESULT SaveStrip( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void *pData );
	bool	RecomputeTimes();														// Returns true if anything changed
	void	RecomputeMeasureBeats();
	void	TimelineToStripUI();
	void	StripUIToTimeline();
	void	InvalidateFBar( void );
	void	CleanUp();
	void	RefreshUI(WORD& wRefreshUI);
	void	RefreshCurvePropertyPage( void );
	void	RefreshCurveStrips( void );


	// Methods to set properties of the strip
	void	OnCurvePropPageMgrDeleted();
	HRESULT SetProperties(StripInfo* pStripInfo);

	void	OnUpdateInsertCurveValue(POINT ptLeft, POINT ptRight);
	WORD	SelectCurvesInRect( CRect* pRect );
	CRect	GetUpdatedCurveRect(CCurveTracker& curveTracker, CTrackItem* pCurve, int nAction);
	void	OnUpdateDragCurveValue(CCurveTracker& tracker, int nAction);	

	// Accessors / Mutators
	IDMUSProdPropPageManager* GetPropPageMgr();

	CString			GetName();
	void			SetName(CString sName);

	CTrackMgr*		GetTrackManager();
	CTrackObject*	GetTrackObject();

	DWORD			GetDMParamFlags();
	void			SetDMParamFlags(DWORD dwParamFlagsDM);

	HRESULT			GetTimeSignature(DirectMusicTimeSig* pTimeSig, MUSIC_TIME mtTime);
	HRESULT			GetParamInfoFromCLSID(MP_PARAMINFO* pParamInfo);

	BOOL			GetStripRect(LPRECT pRectStrip);

	DWORD			GetParamIndex();
	BOOL			UseCenterLine( void );
	HRESULT			CanDeleteParameter();
	
	void			SwitchTimeBase( void );
	BOOL			IsOneGrid();


protected:
	HRESULT	DrawMinimizedStrip( HDC hDC, RECT* pRectStrip );
	HRESULT	DrawMaximizedStrip( HDC hDC, STRIPVIEW sv, RECT* pRectStrip, RECT* pRectClip, LONG lXOffset );
	void	InvertGutterRange( HDC hDC, RECT* pRectClip, LONG lXOffset );
	void	UnselectGutterRange();
	void	DrawClockVerticalLines( HDC hDC, long lXOffset );
	void	HighlightTimelineRange(HDC hDC, long lXOffset);
	void	DrawHorizontalLines(HDC hDC, const RECT &rectClip);
	void	DrawCurve(CTrackItem* pTrackItem, HDC hDC, RECT* pRectStrip, LONG lXOffset);
	void	DragModify(CTrackItem* pTrackItem, int nStripXPos, int nStripYPos, int nAction, bool bMakeCopy = false);
	WORD	SelectCurve( CTrackItem* pTrackItem, BOOL fState );
	WORD	SelectAllCurves( BOOL fState );
	BOOL	CanCycle();
	HRESULT OnCreate();
	HRESULT OnDestroy();
	void	ShowCurveValueOnStausBar(CTrackItem* pItem);
	HRESULT SnapUnknownToNextIncrement(REFERENCE_TIME rtTime, REFERENCE_TIME* prtSnapTime);
	bool	CListSelectedRegion_Contains( REFERENCE_TIME rtTime );

// IDropTarget helpers 
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteReplace( long firstMeasure, long firstBeat, long lastMeasure, long lastBeat);
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lXPos, bool bDropNotPaste, BOOL &fChanged );

// IDropSource helpers
	HRESULT CreateDataObject(IDataObject**, long position);
	HRESULT	DoDragDrop(IDMUSProdTimeline* pTimeline, WPARAM wParam, long lXPos);

// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LONG lXPos, LONG lYPos);
	HRESULT OnLButtonUp( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnRButtonDown( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnMouseMove( WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnSetCursor( long lXPos, long lYPos);

	CTrackItem* GetItemFromPoint( long lPos );
	bool SelectItemsInSelectedRegions();
	void SelectRegionsFromSelectedItems();	
	void AdjustTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );
	BOOL RegisterClipboardFormats();

	CTrackItem* GetTopItemFromMeasureBeat( long lMeasure, long lBeat );
	CTrackItem* GetNextTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );
	CTrackItem* GetNextSelectedTopItem( POSITION pos, long lCurrentMeasure, long lCurrentBeat );
	CTrackItem* GetNextItem( POSITION pos );
	CTrackItem* GetNextSelectedItem( POSITION pos );
	CTrackItem* GetFirstSelectedItem();
	CTrackItem*	GetCurveFromPoint( int nStripXPos, int nStripYPos, BOOL bFrameOnly );
	UINT		HitTest( CCurveTracker& tracker, long lStripXPos, long lStripYPos );

	// Methods for Parameter curves
	void	InsertCurve(long lXPos, long lYPos);
	void	StartInsertingCurve();
	void	StopInsertingCurve();
	float	YPosToValue(long lYPos);
	short	ValueToYPos(float fltValue);
	int		AdjustXPos( int nStripXPos );	
	void	CreateCurveCopy(CTrackItem* pCurve);
	
	// Overrides of CBaseStrip
	bool	IsSelected();	// Returns true if any items are selected
	bool	IsEmpty();		// Returns false if there are any items
	bool	SelectSegment( MUSIC_TIME mtBeginTime, MUSIC_TIME mtEndTime );

		// From CTrackMgr
	void	UnselectAll();
	void	InsertByAscendingTime( CTrackItem *pItem, BOOL fPaste );
	bool	DeleteBetweenMeasureBeats( long lmStart, long lbStart, long lmEnd, long lbEnd );
	HRESULT SaveSelectedItems( LPSTREAM pIStream, REFERENCE_TIME rtOffset, CTrackItem* pItemAtDragPoint );
	HRESULT CycleItems( long lXPos );
	MUSIC_TIME GetNextGreatestUniqueTime( long lMeasure, long lBeat, long lTick );
	void	MarkSelectedItems( DWORD flags );
	void	UnMarkItems( DWORD flags );
	void	DeleteMarked( DWORD flags );
	void	DeleteSelectedItems();

	// New methods
	void	GetFBName( CString& strFBName );
	void	OnShowStripProperties();
	void	OnShowItemProperties();
	HWND	GetMyWindow();

	HRESULT LoadList( CTypedPtrList<CPtrList, CTrackItem*>& list, IStream* pIStream );
	HRESULT SaveList( CTypedPtrList<CPtrList, CTrackItem*>& list, IStream* pIStream );
	HRESULT SaveStripDesignData( IDMUSProdRIFFStream* pIRiffStream );
	
	void	DoGridsPerSecond( void );
	void	SetSnapTo( WORD wID );
	HRESULT SnapXPosToUnknownTime(long lXPos, REFERENCE_TIME* prtSnapTime);
	HRESULT SnapUnknownTime(REFERENCE_TIME rtTime, REFERENCE_TIME* prtSnapTime);
	REFERENCE_TIME CalculateDropTime(MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI);
	REFERENCE_TIME CalculatePasteTime( MUSIC_TIME mtTime, ioCopyPasteUI* pCopyPasteUI );

	// Fields used for UI 
	CTrackMgr*		m_pTrackMgr;
	CTrackObject*	m_pTrackObject;
	CTrackItem		m_SelectedTrackItem;
	CTrackItem*		m_pItemToToggle;
	UINT			m_cfParamTrack;				// CF_PARAMTRACK Clipboard format
	bool			m_fLeftMouseDown;
	HANDLE			m_hKeyStatusBar;

	// For inserting parameter curves
	BOOL			m_fInsertingCurve;
	float			m_fltInsertingStartValue;		// new curve's start value
	float			m_fltInsertingEndValue;		// new curve's end value
	long			m_lInsertingStartClock;		// new curve's start music time
	long			m_lInsertingEndClock;		// new curve's music time
					

	// Reference counter for bitmaps
	static long		sm_lBitmapRefCount;

	// Zoom status variables
	static CBitmap	sm_bmpZoomInBtnUp;
	static CBitmap	sm_bmpZoomInBtnDown;
	static CBitmap	sm_bmpZoomOutBtnUp;
	static CBitmap	sm_bmpZoomOutBtnDown;
	bool			m_fZoomInDown;
	bool			m_fZoomOutDown;

	// Vertical scroll variables
	long			m_lVerticalScroll; 

	// Fields that are persisted (DirectMusic parameter control track data)
	DWORD			m_dwParamIndex;			// Index number of the parameter on the object
	DWORD			m_dwParamFlagsDM;		// Various flags pertaining to parameter (strip)

	CTypedPtrList<CPtrList, CTrackItem*> m_lstTrackItems;

	// Fields that are only persisted in design files
	CString			m_strStripName;			// Used by FBDraw()
	HCURSOR 		m_hCursor;

public:
	MP_PARAMINFO	m_ParamInfo;			// Keeps the parameter info 
	ioStripUI		m_StripUI;				// UI info for the strip
};

inline void CListSelectedRegion_AddRegion(CListSelectedRegion& csrList, CTrackItem& item)
{
	CMusicTimeConverter cmtBeg(item.m_lMeasure, item.m_lBeat, csrList.Timeline(), csrList.GroupBits());
	CMusicTimeConverter cmtEnd = cmtBeg;
	cmtEnd.AddOffset(0,1, csrList.Timeline(), csrList.GroupBits());
	CSelectedRegion* psr = new CSelectedRegion(cmtBeg, cmtEnd);
	csrList.AddHead(psr);
}

#endif // __PARAMSTRIP__H_