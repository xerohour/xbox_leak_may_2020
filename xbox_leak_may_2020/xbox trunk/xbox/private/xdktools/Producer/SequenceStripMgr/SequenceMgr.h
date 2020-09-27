// SequenceMgr.h : Declaration of the CSequenceMgr

#ifndef __SEQUENCEMGR_H_
#define __SEQUENCEMGR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"		// main symbols
#include "SequenceStripMgr.h"
#include "SequenceIO.h"
#include "timeline.h"
#include <DMUSProd.h>
#include <dmusici.h>
#include "SequenceScrollbar.h"
#include "PropSequence.h"
#include <SegmentPrivate.h>
#include <Conductor.h>
#include <RectList.h>

class CSequenceStrip;
class CSequenceItem;
class CDllJazzDataObject;
class CCurveTracker;
class CCurveStrip;
class CPropItem;
class CPropCurve;
interface IDirectMusicTrack;
interface IDMUSProdRIFFStream;
interface IDMUSProdConductor;
interface IDMUSProdPChannelName;

// {4DF1708C-AFF6-11d2-B0DA-00105A26620B}
DEFINE_GUID(GUID_Sequence_Color_Change, 0x4df1708c, 0xaff6, 0x11d2, 0xb0, 0xda, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);

// {0B12D932-ED21-11d2-A6E6-00105A26620B}
DEFINE_GUID(GUID_Sequence_Notation_Change, 0xb12d932, 0xed21, 0x11d2, 0xa6, 0xe6, 0x0, 0x10, 0x5a, 0x26, 0x62, 0xb);

#define PRIVATE_SP_CLSID		(SP_USER+45)
#define PRIVATE_SP_PCHANNEL		(SP_USER+4)

#define INVALID_PATCH			(0xFFFFFFFF)

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_OVERRIDE_ALL | DMUS_TRACKCONFIG_OVERRIDE_PRIMARY | DMUS_TRACKCONFIG_FALLBACK |	DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_PLAY_CLOCKTIME | DMUS_TRACKCONFIG_CONTROL_PLAY)

// structure for sequence strip design time information
struct ioSeqStripDesign
{
	STRIPVIEW	m_svView;			// current strip view (minimized or maximized)
	long		m_lVScroll;			// current vertical scroll (in c_nChannelHeight units)
	long		m_lHeight;			// height of maximized strip
	BOOL		m_fHybridNotation;	// If set, display in hybrid notation, otherwise use PianoRoll display
	double		m_dblVerticalZoom;
	COLORREF	m_crUnselectedNoteColor;// Note color
	COLORREF	m_crSelectedNoteColor;	// Selected note color
	COLORREF	m_crOverlappingNoteColor;// Color of overlapping notes
	COLORREF	m_crAccidentalColor;	// Color of accidentals in hybrid notation
	DWORD		m_dwPChannel;		// PChannel #
	DWORD		m_dwExtraBars;		// Number of bars to display after the segment
	BOOL		m_fPickupBar;		// Whether or not to display a pickup bar
};

struct ioGlobalCurveStripState
{
	int			m_nCurveStripView;		// Minimized or maximized
};

struct ioCurveStripState
{
	int			m_nStripHeight;			// Height of strip
	BYTE		m_bCCType;				// Identifies type of control strip
	BYTE		m_bPad;
	WORD		m_wRPNType;
};

// stuff to identify the current mouse mode
typedef enum tagSEQ_MOUSEMODE
{
	SEQ_MM_NORMAL,
	SEQ_MM_MOVE,
	SEQ_MM_ACTIVEMOVE,
	SEQ_MM_RESIZE_START,
	SEQ_MM_ACTIVERESIZE_START,
	SEQ_MM_RESIZE_END,
	SEQ_MM_ACTIVERESIZE_END,
	SEQ_MM_RESIZE_VELOCITY,
	SEQ_MM_ACTIVERESIZE_VELOCITY,
	SEQ_MM_ACTIVEDRAWBOX,
} SEQ_MOUSEMODE;

typedef enum tagSELECTING
{
	SEQ_NO_SELECT,
	SEQ_SINGLE_SELECT,
	SEQ_MULTIPLE_SELECT,
} SEQ_SELECTING;

typedef enum tagSNAPTO
{
	SNAP_GRID,
	SNAP_BEAT,
	SNAP_BAR,
	SNAP_NONE,
	SNAP_UNKNOWN,
} SNAPTO;

extern class CSequenceStripMgrApp theApp;

// TODO: move to includes/ioDmStyle.h
#define DMUS_FOURCC_SEQUENCE_UI_LIST	mmioFOURCC('p','s','q','l')
#define DMUS_FOURCC_SEQUENCE_UI_CHUNK	mmioFOURCC('p','s','q','c')
#define DMUS_FOURCC_ALLCURVES_UI_CHUNK	mmioFOURCC('c','v','a','u')
#define DMUS_FOURCC_CURVE_UI_CHUNK		mmioFOURCC('c','v','s','u')

void GetTimeSig( MUSIC_TIME mtTime, DMUS_TIMESIGNATURE *pTimeSig, IDMUSProdTimeline* pTimeline, DWORD dwGroupBits );

/////////////////////////////////////////////////////////////////////////////
// CSequenceMgr
class ATL_NO_VTABLE CSequenceMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSequenceMgr, &CLSID_SequenceMgr>,
	public ISequenceMgr,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend CSequenceStrip;
friend CPropItem;
friend CCurveStrip;
friend CPropCurve;
friend class CNoteTracker;
friend class CNewPartDlg;
friend class PropPageNote;
friend class CCurveTracker;

public:
	CSequenceMgr();
	~CSequenceMgr();

public:
	HRESULT GetBoundariesOfSelectedEvents(long& lStartTime, long& lEndTime);
DECLARE_REGISTRY_RESOURCEID(IDR_SEQUENCEMGR)

BEGIN_COM_MAP(CSequenceMgr)
	COM_INTERFACE_ENTRY_IID(IID_ISequenceMgr,ISequenceMgr)
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

// ISequenceMgr methods
	HRESULT STDMETHODCALLTYPE SetPChannel( DWORD dwPChannel );
	HRESULT STDMETHODCALLTYPE SaveEventsToMIDITrack( IStream *pStream );
	HRESULT STDMETHODCALLTYPE PasteEventsFromMIDITrack( IStream *pStream, short nPPQN, DWORD dwLength );

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
	void	EmptySequenceList(void);
	void	InsertByAscendingTime(CSequenceItem *pSequence);
	void	InsertByAscendingTimeBackwards(CSequenceItem *pSequence);
	void	InsertByAscendingTime(CCurveItem *pSequence);
	void	InsertByAscendingTimeBackwards(CCurveItem *pSequence);
//	void	InsertByAscendingTimeReplaceSame( CSequenceItem *pSequence );
//	void	InsertByAscendingTimeFrom( CSequenceItem *pSequence, POSITION pos );
//	void	InsertByAscendingTimeBackwardsFrom( CSequenceItem *pSequence, POSITION pos );
	void	DeleteBetweenTimes( long lStart, long lEnd );
	void	UnselectAllNotes();
	void	UnselectAllCurves();
	void	PlayNote( CSequenceItem *pSequence );
	void	PlayPatch( MUSIC_TIME mtTime );
	void	StopNote( void );
	HRESULT	SendPMsg( DMUS_PMSG *pPMsg );
	BOOL	OffsetSelectedNoteValuePositionAndStart( int nValue, long lMoveOffset, long lStartOffset, BOOL fMakeCopy );
	BOOL	OffsetSelectedNoteDurationAndVelocity( long lDuration, long lVelocity, BOOL fMakeCopy );
	void	MergeSequence( CTypedPtrList<CPtrList, CSequenceItem*>& lstSequences );
	void	MergeCurve( CTypedPtrList<CPtrList, CCurveItem*>& lstCurves );
	int		GetNumSelected();
	void	RefreshPropertyPage( );
	DWORD	ApplyToSelectedNotes( const CPropSequence* pPropNote );
	void	InsertNote( CSequenceItem *pSequence, BOOL fUpdateSegment );
	void	InsertCurve( CCurveItem *pSequence, BOOL fUpdateSegment );
	BOOL	Quantize( SequenceQuantize* pSequenceQuantize );
	BOOL	QuantizeNote( CSequenceItem* pSequenceItem, SequenceQuantize* pSequenceQuantize );
	BOOL	Velocitize( SequenceVelocitize* pSequenceVelocitize );
	BOOL	VelocitizeNote( CSequenceItem* pSequenceItem, BYTE bCompressMin, BYTE bCompressMax, LONG lAbsoluteChange, DWORD dwVelocityMethod );
	MUSIC_TIME CalculatePasteTime( MUSIC_TIME mtTime );
	void UpdateStatusBarDisplay( void );

	BOOL	SelectSegmentNotes(long begintime, long endtime);
	HRESULT SyncWithDirectMusic();
	BOOL	UpdateFlatsAndKey();
	HRESULT	AddNewPart( void );

public:
	BOOL						m_fHasNotationStation; // TRUE if we have the NotationStation font installed

protected:
	IDMUSProdTimeline*			m_pTimeline;

	CTypedPtrList<CPtrList, CSequenceItem*> m_lstSequences;
	CTypedPtrList<CPtrList, CCurveItem*> m_lstCurves;
	IDMUSProdFramework* 		m_pDMProdFramework;
	IDMUSProdNode*				m_pDMProdSegmentNode;
	IDMUSProdConductor*			m_pIConductor;
	IDMUSProdPChannelName*		m_pIPChannelName;
	IDirectMusicPerformance*	m_pIDMPerformance;
	DWORD						m_dwGroupBits;
	DWORD						m_dwOldGroupBits;
	DWORD						m_dwIndex;;
	DWORD						m_dwTrackExtrasFlags;
	DWORD						m_dwProducerOnlyFlags;
	IDirectMusicTrack*			m_pIDMTrack;
	BOOL						m_fDirty;
	BOOL						m_fDisplayingFlats;// If set, display hybrid notation with flats, otherwise use sharps
	int							m_nKeyRoot;
	int							m_nNumAccidentals;
	ioSeqStripDesign			m_SeqStripDesign;
	BOOL						m_fUpdateDirectMusic;
	HANDLE						m_hStatusBar;

	BOOL						m_fSetPChannel;
	DWORD						m_dwPChannel;

	CPropSequence				m_PropSequence;
	IDMUSProdPropPageManager*	m_pPropPageMgr;

	CSequenceStrip*				m_pSequenceStrip;

	BOOL						m_fShuttingDown;
	BOOL						m_fRecordEnabled;
	IDirectMusicSegmentState	*m_pSegmentState;
	BOOL						m_fNoteInserted;
	BOOL						m_fCurveInserted;
	BOOL						m_fWindowActive;
	MUSIC_TIME					m_mtCurrentStartPoint;
	MUSIC_TIME					m_mtCurrentStartTime;
	MUSIC_TIME					m_mtCurrentLoopStart;
	MUSIC_TIME					m_mtCurrentLoopEnd;
	DWORD						m_dwCurrentMaxLoopRepeats;
	DWORD						m_dwCurrentLoopRepeats;

	// State variables for clipping off played notes when dragging them around
	REFERENCE_TIME		m_rtLastPlayNoteOffTime;
	MUSIC_TIME			m_mtLastPlayNoteEnd;
	BYTE				m_bLastPlayNoteMIDIValue;

	CString m_strName;
	CString m_strAuthor;
	CString m_strCopyright;
	CString m_strSubject;
	CString m_strInfo;

protected:
	HRESULT LoadSeqTrack( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO ck );
	HRESULT DeleteSelectedSequences();
	HRESULT SaveSelectedSequences( LPSTREAM, MUSIC_TIME mtOffset );
	HRESULT SaveSelectedSequencesAsMidi( LPSTREAM, MUSIC_TIME mtOffset );
	HRESULT	SaveSelectedSequenceChunk( IDMUSProdRIFFStream* pIRiffStream, MUSIC_TIME mtBeatsOffset);
	HRESULT SaveSelectedCurveChunk( IDMUSProdRIFFStream* pIRiffStream, MUSIC_TIME mtBeatsOffset);
	HRESULT SaveSelectedEvents( LPSTREAM, MUSIC_TIME mtOffset );
	HRESULT SaveSelectedEventsAsMidi( LPSTREAM );
	HRESULT ImportSequenceChunkData( LPSTREAM, long lSize, long lTimeOffset );
	HRESULT ImportCurveChunkData( LPSTREAM, long lSize, long lTimeOffset );
	HRESULT LoadSequenceUIList( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO *pckMain );
	HRESULT SaveSequenceUIList( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT ImportMIDIFileToMultipleTracks( LPSTREAM );

// general helpers
	/*
	void MarkSelectedSequences(DWORD flags);
	void UnMarkSequences(DWORD flags);
	void DeleteMarked(DWORD flags);
	*/
	void SelectAllNotes();
	BOOL RemoveItem( CSequenceItem* pItem );
	BOOL FirstGutterSelectedSequenceMgr( void );

// Misc
	BOOL		IsSelected();	// returns if one or more sequences are selected.
	CSequenceItem* FirstSelectedSequence(POSITION* pos=NULL);	// make non-null for iteration
	CSequenceItem* GetFirstSequence(POSITION& pos)
	{
		pos = m_lstSequences.GetHeadPosition();
		if(pos)
		{
			return m_lstSequences.GetNext(pos);
		}
		else
		{
			return 0;
		}
	}
	CSequenceItem* GetNextSequence(POSITION& pos)
	{
		if(pos == 0)
		{
			return 0;
		}
		else
		{
			return m_lstSequences.GetNext(pos);
		}
	}
};


class CSequenceStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDMUSProdPropPageObject,
	public IDropTarget,
	public IDMUSProdMidiInCPt
{
friend CSequenceMgr;
friend CSequenceScrollBar;
friend CCurveTracker;
friend CCurveStrip;
friend CPropCurve;
friend class CNoteTracker;
friend class CNewPartDlg;
friend class PropPageNote;
friend class CPropItem;

public:
	CSequenceStrip( CSequenceMgr* pSequenceMgr );
	~CSequenceStrip();

public:
	HRESULT PasteMidiFile(IDMUSProdTimelineDataObject* pITimelineDataObject, long lClocks, BOOL fPasteToMultipleStrips );
#ifdef _DEBUG
	//void TraceFormatsInClipboard();
#endif
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

// IDropTarget methods
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

// IDropTarget helpers
	HWND	GetTimelineHWnd();
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteReplace( long firstMeasure, long firstBeat, long lastMeasure, long lastBeat);
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, long lClocks, BOOL bDropNotEditPaste, BOOL fPasteMIDIToMultipleStrips);

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

// IDMUSProdMidiInCPt functions
	HRESULT STDMETHODCALLTYPE OnMidiMsg(REFERENCE_TIME rtTime, BYTE bStatus, BYTE bData1, BYTE bData2);

public:
	BOOL	GetStripRect(CRect& rectStrip);
	BOOL	SelectInRect( CRect* pRect );

protected:
// Internal message handlers
	HRESULT OnLButtonDown( WPARAM wParam, LONG lXPos, LONG lYPos);
	HRESULT OnLButtonUp( long lXPos, long lYPos);
	HRESULT OnSetCursor( long lXPos, long lYPos);
	HRESULT OnRButtonUp( void );

// This is called when our scrollbar receives a WM_APP message (used for MIDI input)
	LRESULT OnApp( WPARAM wParam, LPARAM lParam );

// drawing helpers
	void	DrawMinimizedNotes( HDC hDC, POSITION pos, long lStartTime, long lEndTime, long lXOffset, BOOL fSelected );
	void	DrawMaximizedNotes( HDC hDC, POSITION pos, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, BOOL fSelected );
	void	DrawHybridMaximizedNotes( HDC hDC, POSITION pos, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, BOOL fSelected );
	void	DrawHybridMaximizedAccidentals( HDC hDC, POSITION pos, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote );
	void	DrawFunctionBar( HDC hDC, STRIPVIEW sv );
	void	DrawHorizontalLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote );
	void	DrawHybridLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote );
	void	DrawDarkHorizontalLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote );
	void	DrawDarkHybridLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote );
	POSITION GetFirstVisibleNote( long lStartTime, long lEndTime, int nTopNote, int nBottomNote );
	void	InitializeScaleAccidentals( void );
	void	UpdateKeyPattern( void );
	void	InvalidateFunctionBar( void );
	void	InvalidatePianoRoll( void );
	void	InvalidateStrip( void );
	void	DrawSymbol( HDC hDC, const TCHAR *pstrText, int iHybridPos, long lLeftPos, long lTopAjust, long lBottomAdjust );
	void	DrawNoteInsertionMark( HDC hDC, MUSIC_TIME mtStartTime, MUSIC_TIME mtEndTime, long lXOffset );
	HFONT	GetAccidentalFont( void );

// scroll stuff
	void	UpdateVScroll();	
	void	SetVScroll(long lNewValue);
	void	LimitVScroll();
	void	OnVScroll(UINT nSBCode, UINT nPos);
	HRESULT OnVScroll( void );
	HRESULT OnSize( void );
	HRESULT OnChar( WPARAM wParam);
	HRESULT OnMouseMove( long lXPos, long lYPos);
	HRESULT OnKeyDown( WPARAM wParam );
	HRESULT OnDestroy( void );
	HRESULT OnCreate( void );
	HRESULT SetFocus( int iFocus ); // 0 = None, 1 = Sequence, 2 = Curve
	void	OnTimer( void );
	void	KillTimer( void );
	void	EnableTimer( void );
	void    OnChangeStripView( STRIPVIEW svNewStripView );
	void	AdjustScroll(long lXPos, long lYPos);

// general helpers
	void	UpdateName();
	void	GetNoteRect( const CSequenceItem* pSeqItem, RECT* pRect );
	void	GetHybridRect( RECT* pRect, int iHybridPos, int iVelocity, MUSIC_TIME mtStart, MUSIC_TIME mtDuration );
	CSequenceItem* GetSeqItemAndRectFromPoint( long lXPos, long lYPos, RECT *pRect );
	void	SetMouseMode( long lXPos, long lYPos);
	void	UnselectGutterRange( void );
	BOOL	SelectEventsBetweenTimes( long lStart, long lEnd );
	int		PositionToMIDIValue( long lYPos );
	void	ApplyUIChunk( const ioSeqStripDesign *pSeqStripDesign );
	void	UpdateSequenceUIChunk();
	void	BumpTimeCursor( SNAPTO snapTo = SNAP_UNKNOWN );
	void	UpdateNoteCursorTime( BOOL fRedraw = TRUE );
	SNAPTO	GetSnapToBoundary( long lTime ) const;
	long	GetSnapAmount( long lTime, SNAPTO st = SNAP_UNKNOWN ) const;
	long	GetGridClocks( long lTime ) const;
	void	ChangeNotationType( BOOL fHybridNotation );
	void	ChangeZoom( double dblVerticalZoom );
	HRESULT DoQuantize( void );
	HRESULT DoVelocity( void );
	void	EnsureNoteCursorVisible( void );
	void	UpdateSelectionState( void );
	DWORD	GetNumExtraBars( void ) const;
	BOOL	ShouldDisplayPickupBar( void ) const;
	CSequenceItem *CreateNoteToInsert( void ) const;
	void	UpdateInstrumentName( void );

// MIDI helpers
	void	UnRegisterMidi( void );
	void	RegisterMidi( void );
	void	SendAllNotesOffIfNotPlaying( void );
	HRESULT RecordStepNoteOn( BYTE bData1, BYTE bData2 );
	HRESULT RecordStepNoteOff( BYTE bData1 );
	HRESULT RecordRealTimeNoteOn(REFERENCE_TIME rtTime, BYTE bData1, BYTE bData2 );
	HRESULT RecordRealTimeNoteOff(REFERENCE_TIME rtTime, BYTE bData1 );
	HRESULT RecordRealTimeCurve( REFERENCE_TIME rtTime, BYTE bStatus, BYTE bData1, BYTE bData2 );
	void	SetPChannelThru();
	void	CancelPChannelThru();
	BOOL	IsEnginePlaying( void ) const;
	MUSIC_TIME FloorTimeToGrid( MUSIC_TIME mtTime, long *plGridClocks ) const;
	BOOL		m_fMIDIInRegistered;
	BOOL		m_fMIDIThruEnabled;
	DWORD		m_dwCookie;
	BYTE		m_bVelocity[128];
	MUSIC_TIME	m_mtStartTime[128];

// mouse cursors
	HCURSOR GetResizeStartCursor( void );
	HCURSOR GetResizeEndCursor( void );
	HCURSOR GetArrowCursor( void );
	HCURSOR GetNSCursor( void );
	HCURSOR GetAllCursor( void );

// Curve strip helpers
	static BOOL	CurveStripExists( void *pThis, BYTE bCCType, WORD wRPNType );
	BYTE	CurveTypeToStripCCType( CCurveItem* pDMCurve );
	void	SyncCurveStripStateList( void );
	ioCurveStripState* GetCurveStripState( BYTE bCCType, WORD wRPNType );
	CCurveStrip* GetCurveStrip( BYTE bCCType, WORD wRPNType );
	HRESULT AddCurveStrip( BYTE bCCType, WORD wRPNType );
	HRESULT AddCurveStrips( void );
	HRESULT RemoveCurveStrip( CCurveStrip* pCurveStrip );
	HRESULT OnDeleteCurveStrip( CCurveStrip* pCurveStrip );
	HRESULT OnNewCurveStrip( void );
	DWORD	DeterminePositionForCurveStrip( BYTE bCCType, WORD wRPNType );
	void	OnChangeCurveStripView( STRIPVIEW svNewStripView );
	void	InvalidateCurveStrips( void );
	HRESULT SaveCurveStripStateData( IDMUSProdRIFFStream* pIRiffStream );
	void	UpdateCurveStripGutterSelection( BOOL fChanged );

	// Useful stuff
	long				m_cRef;
	CSequenceMgr*		m_pSequenceMgr;
	IDMUSProdStripMgr*	m_pStripMgr;

// Selection parameters
	LONG		m_lBeginSelect;
	LONG		m_lEndSelect;
	BOOL		m_bGutterSelected;	// whether the gutter select is selected, use
									// CSequenceMgr::m_bSelected for whether sequence is selected
	SEQ_SELECTING	m_SelectionMode;
	BYTE		m_bSelectionCC; // If a CC strip is selected, which one
	WORD		m_wSelectionParamType; // Which RPN/NRPN CC strip is selected
	POINT		m_pointSelection;
	BOOL		m_fSelecting;

	// Random state information
	CString		m_strName;
	int			m_iHaveFocus; // 0 = No focus, 1 = Sequence, 2 = Curve
	MUSIC_TIME	m_mtLength;
	CCurveStrip	*m_pActiveCurveStrip;


	// Strip-specific data we should persist
protected:
	STRIPVIEW	m_svView;				// current strip view (minimized or maximized)
	long		m_lVScroll;				// current vertical scroll (in c_nChannelHeight units)
	BOOL		m_fHybridNotation;		// If set, display in hybrid notation, otherwise use PianoRoll display
	double		m_dblVerticalZoom;
	COLORREF	m_crSelectedNoteColor;
	COLORREF	m_crUnselectedNoteColor;
	COLORREF	m_crOverlappingNoteColor;
	COLORREF	m_crAccidentalColor;

	STRIPVIEW	m_CurveStripView;
	CTypedPtrList<CPtrList, ioCurveStripState*> m_lstCurveStripStates;
	CTypedPtrList<CPtrList, CCurveStrip*> m_lstCurveStrips;

protected:
	HRESULT	ShowPropertySheet(IDMUSProdTimeline*);

	// Edit information
	LONG		m_lXPos;				// used for temp storage of xpos when doing mouse edits
	CSequenceItem*	m_pSelectPivotSeq;	// last seq mouse clicked on when shift was not down.
	CSequenceItem*	m_pActiveNote;
	CSequenceItem	m_OriginalSeqItem;
	long		m_lInsertVal;
	long		m_lInsertTime;
	POINT		m_pointClicked;
	UINT		m_nLastEdit;			// resource id of last edit
	BOOL		m_fCtrlKeyDown;	// If set, then a ctrl-click was on (1) selected note, or (2) unselected note
	long		m_lLastDeltaStart;
	char		m_cLastDeltaValue;
	long		m_lLastDeltaDur;
	long		m_lLastDeltaVel;
	long		m_lLastDeltaMove;
	BOOL		m_fScrollTimerActive;
	bool		m_fInsertingNoteFromMouseClick;

	// Drag'n'Drop
	IDataObject*m_pITargetDataObject;
	DWORD		m_dwOverDragButton; 	// Mouse button stored in IDropTarget::DragOver
	DWORD		m_dwOverDragEffect; 	// Drag effects stored in IDropTarget::DragOver
	DWORD		m_dwDragRMenuEffect;	// Result from drag context menu

	// Cut 'n' paste
	UINT		m_cfSequenceList;		// Clipboard formats
	UINT		m_cfSequenceCurveList;	// Sequence and Curve data together
	UINT		m_cfMidiFile;

	// Drawing
	BYTE		m_bMinimizeTopNote;
	BYTE		m_bMinimizeNoteRange;
	long		m_lMaxNoteHeight;
	HCURSOR 	m_hCursor;
	SEQ_MOUSEMODE m_MouseMode;		// current Mouse Mode
	CRectList	m_aNoteRectList[128];
	int			m_aiAccidentals[75];	// Current Accidental
	int			m_aiScalePattern[7];	// Current Scale Pattern for initializing m_aiAccidentalst
	MUSIC_TIME	m_mtLastLateTime;
	DWORD		m_dwExtraBars;
	BOOL		m_fPickupBar;
	DWORD		m_dwLastPatch;

	// Bitmap status variables
	BOOL			m_fZoomInPressed;
	BOOL			m_fZoomOutPressed;
	BOOL			m_fNewBandPressed;
	BOOL			m_fInstrumentPressed;
	BOOL			m_fInstrumentEnabled;
	static long		m_lBitmapRefCount;
	static CBitmap	m_BitmapZoomInUp;
	static CBitmap	m_BitmapZoomInDown;
	static CBitmap	m_BitmapZoomOutUp;
	static CBitmap	m_BitmapZoomOutDown;
	static CBitmap	m_BitmapNewBand;

	// Property page
	BOOL		m_fShowSequenceProps;		// if TRUE, show Sequence property page, else show group property page
	BOOL		m_fPropPageActive;		// TRUE if our property page is active
	IDMUSProdPropPageManager*	m_pPropPageMgr;

	// vertical scrollbar stuff
	CRect			m_rectVScroll;		// rectangle for location of vertical scrollbar
	CSequenceScrollBar	m_VScrollBar;		// vertical scrollbar
};
#endif //__SEQUENCEMGR_H_
