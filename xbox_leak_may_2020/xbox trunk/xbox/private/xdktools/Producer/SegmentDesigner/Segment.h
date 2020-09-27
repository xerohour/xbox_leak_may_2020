#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// Segment.h : header file
//

/*-----------
@doc DMUSPROD
-----------*/

#include "SegmentDesignerDLL.h"
#include "SegmentDocType.h"
#include "SegmentPPGMgr.h"
#include "DLLJazzDataObject.h"
#include "ProxyStripMgr.h"
#include <DMUSProd.h>
#include <DMPPrivate.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )
#include <dmusici.h>

#define DMUS_SEGF_VALID_ALL (DMUS_SEGF_VALID_START_BEAT | DMUS_SEGF_VALID_START_GRID | DMUS_SEGF_VALID_START_TICK)

#define DMUS_FOURCC_SEGMENT_DESIGN_LIST		mmioFOURCC('s','g','d','l')
#define DMUS_FOURCC_SEGMENT_DESIGN_CHUNK	mmioFOURCC('s','e','g','d')
#define DMUS_FOURCC_SEGMENT_DESIGN_STYLE_NAME	mmioFOURCC('s','n','a','m')
#define DMUS_FOURCC_SEGMENT_DESIGN_STYLE_REF	mmioFOURCC('s','r','e','f')
#define DMUS_FOURCC_SEGMENT_DESIGN_PERS_NAME	mmioFOURCC('p','n','a','m')
#define DMUS_FOURCC_SEGMENT_DESIGN_PERS_REF	mmioFOURCC('p','r','e','f')
#define DMUS_FOURCC_SEGMENT_DESIGN_TRACK_PRODUCER_ONLY	mmioFOURCC('s','g','p','o')

typedef struct _DMUS_IO_SEGMENT_DESIGN
{
	double		dblTempo;			// Tempo. By default, 120
	float		fTempoModifier;		// Tempo modifier. By default, 0.5
	double		dblZoom;			// Timeline zoom factor. By default, 0.03125
	DWORD		dwMeasureLength;	// Length in measures
	WORD		wSegmentRepeats;	// Design-time repeats
	BYTE		bKey;				// Key 0-11 (NO LONGER USED!!!)
	BOOL		fShowFlats;			// Flats/sharps (NO LONGER USED!!!)
	BOOL		fReserved;			// No longer used (was Template/Section)
	DWORD		dwPlayStartMeasure;	// Start measure
	BYTE		bPlayStartBeat;		// Start beat
	WORD		wPlayStartGrid;		// Start grid
	WORD		wPlayStartTick;		// Start tick
	DWORD		dwTrackGroup;		// Group to get TimeSig from

	DWORD		dwLoopStartMeasure; // Loop start measure
	BYTE		bLoopStartBeat;		// Loop start beat
	WORD		wLoopStartGrid;		// Loop start grid
	WORD		wLoopStartTick;		// Loop start tick
	DWORD		dwLoopEndMeasure;	// Loop end measure
	BYTE		bLoopEndBeat;		// Loop end beat
	WORD		wLoopEndGrid;		// Loop end grid
	WORD		wLoopEndTick;		// Loop end tick
	BOOL		fPad;				

	WORD		wReserved;			// No longer used (was Activity level)

	DWORD		dwVerticalScroll;	// Vertical Scroll
	DWORD		dwReserved;			// No longer used (was Horizontal Scroll)

	WORD		wQuantizeTarget;	// Params used by sequence strip quantize dlg
	BYTE		bQuantizeResolution;
	BYTE		bQuantizeStrength;
	DWORD		dwQuantizeFlags;
	
	WORD		wVelocityTarget;	// Params used by sequence strip edit velocity dlg
	BYTE		bCompressMin;
	BYTE		bCompressMax;
	LONG		lAbsoluteChangeStart;
	DWORD		dwVelocityMethod;
	LONG		lAbsoluteChangeEnd;

	DWORD		dwSnapTo;
	LONG		lFunctionbarWidth;
	double		dblHorizontalScroll;// Horizontal Scroll
} DMUS_IO_SEGMENT_DESIGN;

typedef struct _DMUS_IO_SEGMENT_HEADER_EX
{
    DWORD       dwRepeats;      /* Number of repeats. By default, 0. */
    MUSIC_TIME  mtLength;       /* Length, in music time. */
    MUSIC_TIME  mtPlayStart;    /* Start of playback. By default, 0. */
    MUSIC_TIME  mtLoopStart;    /* Start of looping portion. By default, 0. */
    MUSIC_TIME  mtLoopEnd;      /* End of loop. Must be greater than dwPlayStart. Or, 0, indicating loop full segment. */
    DWORD       dwResolution;   /* Default resolution. */
    /* Following added for DX8: */
    REFERENCE_TIME rtLength;    /* Length, in reference time (overrides music time length.) */
    DWORD       dwFlags;
    DWORD       dwReserved;     /* Reserved. */
    /* Added for XBOX. */
    REFERENCE_TIME rtLoopStart; /* Clock time loop start. */
    REFERENCE_TIME rtLoopEnd;   /* Clock time loop end. */
} DMUS_IO_SEGMENT_HEADER_EX;

#define DMUS_SEGIOF_REFLOOP     2  /* Use the values in rtLoopStart and rtLoopEnd to do clock time looping. */

int TrackCLSIDToInt( REFCLSID clsidType );
int CompareTracks( int nType1, DWORD dwGroups1, int nType2, DWORD dwGroups2 );

interface IDMUSProdRIFFStream;
interface IDirectMusicChordMap;
class CSegmentCtrl;
class CSegmentDlg;
class CSegmentComponent;
class CTemplateStruct;
class CTrack;
class CSegmentUndoMan;

interface IPrivateSegment : public IUnknown
{
    public:
        virtual HRESULT STDMETHODCALLTYPE Compose( DWORD dwFlags, DWORD dwActivity ) = 0;
};

struct SegStateParams
{
	IDirectMusicSegmentState *pSegState;
	MUSIC_TIME			mtLoopStart;
	MUSIC_TIME			mtLoopEnd;
	DWORD				dwMaxLoopRepeats;

	SegStateParams(	IDirectMusicSegmentState *pNewSegState,	MUSIC_TIME mtNewLoopStart, MUSIC_TIME mtNewLoopEnd, DWORD dwNewMaxLoopRepeats )
	{
		pSegState = pNewSegState;
		mtLoopStart = mtNewLoopStart;
		mtLoopEnd = mtNewLoopEnd;
		dwMaxLoopRepeats = dwNewMaxLoopRepeats;
		ASSERT( pSegState );
		pSegState->AddRef();
	}
	~SegStateParams() {	RELEASE( pSegState ); };
};

class CNotificationHandler : public CWnd
{
// Construction
public:
	CNotificationHandler() { m_pSegment = NULL; };

// Attributes
public:
	CSegment	*m_pSegment;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotificationHandler)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNotificationHandler() { m_pSegment = NULL; };

	// Generated message map functions
protected:
	//{{AFX_MSG(CNotificationHandler)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	LRESULT OnApp( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

class CSegment : public IDMUSProdNode, public IPersistStream, public IDMUSProdTransport, public IDMUSProdNotifyCPt,
				  public IDMUSProdPropPageObject, public IDMUSProdTimelineCallback, public IDMUSProdNotifySink,
				  public IDMUSProdConductorTempo, IPrivateSegment, public IDMUSProdSecondaryTransport,
				  public IDMUSProdSegmentEdit8, public IDMUSProdSortNode, public IDMUSProdGetReferencedNodes
{
friend CProxyStripMgr;
friend CSegmentCtrl;
friend CSegmentDlg;
friend CSegmentDocType;
friend CSegmentComponent;
friend CNotificationHandler;
friend HRESULT CreateSegmentFromMIDIStream(CSegment* pSegment, LPSTREAM pStream);
friend static DWORD ReadEvent( LPSTREAM pStream, DWORD dwTime, struct FullSeqEvent** plstEvent, struct _DMUS_IO_PATCH_ITEM** pplstPatchEvent,
										struct FullSeqEvent*& lstVol, struct FullSeqEvent*& lstPan,
										struct FullSeqEvent*& lstOther);
public:
    CSegment( CSegmentComponent* pComponent );
	~CSegment();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnFirstImage );

    HRESULT STDMETHODCALLTYPE GetComponent( IDMUSProdComponent** ppIComponent );
    HRESULT STDMETHODCALLTYPE GetDocRootNode( IDMUSProdNode** ppIDocRootNode );
    HRESULT STDMETHODCALLTYPE SetDocRootNode( IDMUSProdNode* pIDocRootNode );
    HRESULT STDMETHODCALLTYPE GetParentNode( IDMUSProdNode** ppIParentNode );
    HRESULT STDMETHODCALLTYPE SetParentNode( IDMUSProdNode* pIParentNode );
    HRESULT STDMETHODCALLTYPE GetNodeId( GUID* pguid );
    HRESULT STDMETHODCALLTYPE GetNodeName( BSTR* pbstrName );
    HRESULT STDMETHODCALLTYPE GetNodeNameMaxLength( short* pnMaxLength );
    HRESULT STDMETHODCALLTYPE ValidateNodeName( BSTR bstrName );
    HRESULT STDMETHODCALLTYPE SetNodeName( BSTR bstrName );
	HRESULT STDMETHODCALLTYPE GetFirstChild(IDMUSProdNode **ppIFirstChildNode);
	HRESULT STDMETHODCALLTYPE GetNextChild(IDMUSProdNode *pIChildNode, IDMUSProdNode **ppINextChildNode);
	HRESULT STDMETHODCALLTYPE GetNodeListInfo( DMUSProdListInfo* pListInfo );

    HRESULT STDMETHODCALLTYPE GetEditorClsId( CLSID* pclsid );
    HRESULT STDMETHODCALLTYPE GetEditorTitle( BSTR* pbstrTitle );
    HRESULT STDMETHODCALLTYPE GetEditorWindow( HWND* hWndEditor );
    HRESULT STDMETHODCALLTYPE SetEditorWindow( HWND hWndEditor );

	HRESULT STDMETHODCALLTYPE UseOpenCloseImages( BOOL* pfUseOpenCloseImages );

    HRESULT STDMETHODCALLTYPE GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnResourceId );
    HRESULT STDMETHODCALLTYPE OnRightClickMenuInit( HMENU hMenu );
    HRESULT STDMETHODCALLTYPE OnRightClickMenuSelect( long lCommandId );

    HRESULT STDMETHODCALLTYPE DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser );
    HRESULT STDMETHODCALLTYPE InsertChildNode( IDMUSProdNode* pIChildNode );
    HRESULT STDMETHODCALLTYPE DeleteNode( BOOL fPromptUser );

    HRESULT STDMETHODCALLTYPE OnNodeSelChanged( BOOL fSelected );

	HRESULT STDMETHODCALLTYPE GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject );

	// Drag / Drop Methods
	HRESULT STDMETHODCALLTYPE CreateDataObject( IDataObject** ppIDataObject );
	HRESULT STDMETHODCALLTYPE CanCut();
	HRESULT STDMETHODCALLTYPE CanCopy();
	HRESULT STDMETHODCALLTYPE CanDelete();
	HRESULT STDMETHODCALLTYPE CanDeleteChildNode( IDMUSProdNode* pIChildNode );
	HRESULT STDMETHODCALLTYPE CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE PasteFromData( IDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL* pfWillSetReference  );
	HRESULT STDMETHODCALLTYPE ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode);

    // IPersist functions
    STDMETHOD(GetClassID)( CLSID* pClsId );

    // IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

	// IIDMUSProdConductorTempo methods
	STDMETHOD(SetTempo)( double dblTempo);
	STDMETHOD(GetTempo)( double *pdblTempo);
	STDMETHOD(SetTempoModifier)( double dblModifier);
	STDMETHOD(GetTempoModifier)( double *pdblModifier);
	STDMETHOD(TempoModified)( BOOL fModified);

	// IDMUSProdTransport methods
	STDMETHOD(Play)( BOOL fPlayFromStart );
	STDMETHOD(Stop)( BOOL fStopImmediate );
	STDMETHOD(Transition)();
	STDMETHOD(Record)( BOOL fPressed );
	STDMETHOD(GetName)( BSTR* pbstrName );
	STDMETHOD(TrackCursor)(BOOL fTrackCursor);

	// IDMUSProdSecondaryTransport methods
	STDMETHOD(GetSecondaryTransportSegment)( IUnknown **ppunkSegment );
 	STDMETHOD(GetSecondaryTransportName)( BSTR* pbstrName );

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

	// IDMUSProdTimelineCallback functions
	HRESULT STDMETHODCALLTYPE OnDataChanged(/* [in] */ LPUNKNOWN punkStripMgr);
        
	// IDMUSProdNotifySink
	HRESULT STDMETHODCALLTYPE OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData);

	// IDMUSProdNotifyCPt
	HRESULT STDMETHODCALLTYPE OnNotify(ConductorNotifyEvent* pEvent);

	// IPrivateSegment
	HRESULT STDMETHODCALLTYPE Compose( DWORD dwFlags, DWORD dwActivity );

/* --------------------------------------------------------------------------
@interface IDMUSProdSegmentEdit8 | 
	This interface contains various helper methods for use when editing Segments.

@base public | IUnknown
-------------------------------------------------------------------------- */

	// IDMUSProdSegmentEdit8
// @meth HRESULT | AddStrip | Adds a track of type <p clsidTrackType> to this Segment.
	HRESULT STDMETHODCALLTYPE AddStrip( CLSID clsidTrackType, DWORD dwGroupBits, IUnknown** ppIStripMgr );
// @meth HRESULT | ContainsTempoStrip | Determines whether this Segment contains a tempo track.
	HRESULT STDMETHODCALLTYPE ContainsTempoStrip( BOOL* pfTempoStripExists );
// @meth HRESULT | GetObjectDescriptor | Fills a DMUS_OBJECTDESC strucure with information about this Segment.
	HRESULT	STDMETHODCALLTYPE GetObjectDescriptor( void* pObjectDesc );
// @meth HRESULT | RemoveStripMgr | Removes the <pIStripMgr> from the segment and adds an item to the undo queue.
	HRESULT STDMETHODCALLTYPE RemoveStripMgr( IUnknown* pIStripMgr );
// @meth HRESULT | EnumTrack | Enumerates through the tracks in the segment.
	HRESULT STDMETHODCALLTYPE EnumTrack( DWORD dwIndex, DWORD dwTrackHeaderSize, void *pioTrackHeader, DWORD dwTrackExtrasHeaderSize, void *pioTrackExtrasHeader );
// @meth HRESULT | ReplaceTrackInStrip | Replaces the existing track in a strip manager with a new track.
	HRESULT STDMETHODCALLTYPE ReplaceTrackInStrip( IUnknown* pIStripMgr, IUnknown* pITrack );

	// IDMUSProdSortNode : Sorting interface
	HRESULT STDMETHODCALLTYPE CompareNodes(IDMUSProdNode* pINode1, IDMUSProdNode* pINode2, int* pnResult);

    // IDMUSProdGetReferencedNodes
    HRESULT STDMETHODCALLTYPE GetReferencedNodes(DWORD *pdwArraySize, IDMUSProdNode **ppIDMUSProdNode, DWORD dwErrorLength, WCHAR *wcstrErrorText );

	// Additional functions
private:
//////////////////////////////////////////////////////////////////////////
// CSegment IDMUSProdNotifySink helpers
	// other funcs
	HRESULT ReadTemplate(IStream *pIStream, CTemplateStruct *pTemplateStruct, long lRecSize);

	BOOL	FindTrackByCLSID( REFCLSID clsidTrackId, CTrack **ppTrack );
	HRESULT GetParam(REFGUID rguidType, DWORD dwGroupBits, DWORD dwIndex, MUSIC_TIME mtTime, MUSIC_TIME *pmtNext, void *pData);
	HRESULT NotifyAllStripMgrs( GUID guidUpdateType, DWORD dwGroupBits, void* pData );
	HRESULT GUIDToStripMgr( REFGUID guidCLSID, GUID* guidStripMgr );

	void CleanUp();
	HRESULT InitSegmentForPlayback();
	void RegisterSegementStateWithAudiopath( BOOL fRegister, IDirectMusicSegmentState *pSegState );
	HRESULT InsertTrackAtDefaultPos( CTrack *pTrackToInsert );
	DWORD GetButtonState() const;
	void OnLengthChanged( MUSIC_TIME mtOldLength );
	void SetSegmentHeaderChunk( void );
//	HRESULT InternalGetStripMgr( GUID guidType, DWORD dwGroupBits, DWORD dwIndex, IDMUSProdStripMgr **ppStripMgr );


private:	
	// Load methods
	HRESULT DM_LoadSegment( IStream* pIStream );
	HRESULT IMA25_LoadTemplate( IStream* pIStream );
	HRESULT IMA25_LoadSection( IStream* pIStream );
	HRESULT IMA25_LoadSectionBand( IStream* pIStream, MMCKINFO* pckMain );
	HRESULT IMA25_LoadSectionChordList( IStream* pIStream, MMCKINFO* pckMain, char cKey );
	HRESULT IMA25_LoadSectionCommandList( IStream* pIStream, MMCKINFO* pckMain );
	HRESULT IMA25_LoadSectionNoteList( IStream* pIStream, MMCKINFO* pckMain );
	HRESULT IMA25_LoadSectionMuteList( IStream* pIStream, MMCKINFO* pckMain );
	HRESULT IMA25_LoadSectionCurveList( IStream* pIStream, MMCKINFO* pckMain );
	HRESULT IMA25_LoadSectionStyleRef( IStream* pIStream, MMCKINFO* pckMain );
	HRESULT IMA25_LoadSectionPersonalityRef( IStream* pIStream, MMCKINFO* pckMain );
	HRESULT IMA25_LoadRIFFSection( IStream* pIStream );
	HRESULT ReadMIDIFile( IStream* pIStream );
	HRESULT ReadRIFFMIDIFile( IStream* pIStream );
	HRESULT DM_AddTrack( IDMUSProdRIFFStream* pIRiffStream, DWORD dwSize );
	IDMUSProdNode* FindStyle( CString strStyleName, IStream* pIStream );
	IDMUSProdNode* FindPersonality( CString strPersName, IStream* pIStream );
	HRESULT AddAuditionStyleRefTrack( IDMUSProdNode *pINode );
	HRESULT AddAuditionPersRefTrack( IDMUSProdNode *pINode );

	// Save methods
	HRESULT DM_SaveSegment( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty );
	HRESULT DM_SaveSegmentHeader( IDMUSProdRIFFStream* pIRiffStream ) const;
	HRESULT DM_SaveSegmentGUID( IDMUSProdRIFFStream* pIRiffStream ) const;
	HRESULT DM_SaveSegmentDesign( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT DM_SaveSegmentInfoList( IDMUSProdRIFFStream* pIRiffStream ) const;
	HRESULT DM_SaveSegmentVersion( IDMUSProdRIFFStream* pIRiffStream ) const;
	HRESULT DM_SaveTrackList( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty );
	HRESULT WriteMIDIFile( IStream* pIStream );
	HRESULT PasteCF_CONTAINER( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );
	HRESULT PasteCF_AUDIOPATH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );
	HRESULT PasteCF_GRAPH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );

	// SetData helper methods
	void UpdateSavedState( BOOL &fStateSaved, int nUndoID );
	void UpdateTrackGroupBits( CTrack *pTrack, IDMUSProdStripMgr* pIStripMgr, DWORD dwGroupBits );
	HRESULT UpdateTrackConfig( CTrack *pTrack );
	CTrack *FindCTrackFromDMTrack( IDirectMusicTrack* pIDMTrack );
	void GetNumExtraBars( DWORD *pdwExtraBars, BOOL *pfPickupBars );
	HRESULT GetData_Boundary( void** ppData );
	HRESULT SetData_Boundary( void* pData );

	// ToolGraph helper methods
	HRESULT AddSegmentToGraphUserList();
	HRESULT RemoveSegmentFromGraphUserList();

public:
	HRESULT Initialize();
	void	SetModifiedFlag(BOOL fModified);
	void	EnableDialogTimer( BOOL bEnable ) const;
	void	SetDialog( CSegmentDlg *pDialog );
	void	RemoveDialog( void );
	HRESULT UnRegisterWithTransport();
	HRESULT RegisterWithTransport();
	HRESULT	AddTrack( CTrack *pTrack );
	HRESULT	AddSequenceTrack( CTrack *pTrack );
	void	PrivRemoveStripMgr( IDMUSProdStripMgr *pIStripMgr );
	void	OnTimeSigChange( void );
	void	OnNameChange( void );
	void	UpdateConductorTempo( void );
	HRESULT SetQuantizeParams( SequenceQuantize* pSequenceQuantize );
	HRESULT GetQuantizeParams( SequenceQuantize* pSequenceQuantize );
	HRESULT SetVelocitizeParams( SequenceVelocitize* pSequenceVelocitize );
	HRESULT GetVelocitizeParams( SequenceVelocitize* pSequenceVelocitize );
	void	OnNotificationCallback( void );
	HRESULT	UpdateChordStripFromSegment( void );
	HRESULT CreateEmptyAudioPath();
	HRESULT CreateEmptyContainer();
	HRESULT CreateEmptyToolGraph();
	void	GetGUID( GUID* pguidSegment );
	void	SetGUID( GUID guidSegment );
	void	SyncTimelineSettings( void );

	BOOL IsPlaying();

// Attributes
public:
	CSegmentComponent		*m_pComponent;
	CString					m_strOrigFileName;
	CRITICAL_SECTION		m_csSegmentState;

private:
    DWORD					m_dwRef;
	IDMUSProdNode*			m_pIDocRootNode;
	IDMUSProdNode*			m_pIParentNode;
	IDMUSProdNode*			m_pIContainerNode;
	IDMUSProdNode*			m_pIAudioPathNode;
	IDMUSProdNode*			m_pIToolGraphNode;
	bool					m_fDeletingContainer;

	HWND				m_hWndEditor;

	// Proxy Strip manager - used to communicate with other StripMgrs
	CProxyStripMgr		m_ProxyStripMgr;

	// CSegmentDlg ptr (used to update the timer on the dialog if it is open).
	CSegmentDlg			*m_pSegmentDlg;

	// Undo manager
	CSegmentUndoMan		*m_pUndoMgr;

	// List of Strip data in this segment
	CTypedPtrList<CPtrList, CTrack*> m_lstTracks;

	// Segment data for Music Engine
	IDirectMusicSegment	*m_pIDMSegment;
	IDirectMusicSegment8 *m_pIDMSegment8;

	// List of segment states
	CTypedPtrList<CPtrList, SegStateParams*> m_lstSegStates;
	CTypedPtrList<CPtrList, IDirectMusicSegmentState*> m_lstTransSegStates;
	IDirectMusicSegmentState	*m_pIDMCurrentSegmentState;
	/*
	IDirectMusicSegmentState	*m_pIDMSegmentState;
	IDirectMusicSegmentState	*m_pIDMTransitionSegmentState;
	IDirectMusicSegmentState	*m_rpIDMStoppedSegmentState;
	*/

	// Old template parameter, only for giving to the SignPost track when loading template files
	WORD				m_wLegacyActivityLevel;
	bool				m_fLoadedLegacyActivityLevel;

	// Segment properties
	BOOL				m_fModified;
	BOOL				m_fTransportRegistered;
	BOOL				m_fSettingLength;
	BOOL				m_fAddedToTree;
	BOOL				m_fInUndo;
	BOOL				m_fHaveTempoStrip;
	BOOL				m_fRecordPressed;
	BOOL				m_fTrackTimeCursor;
	BOOL				m_fInTransition;
	BOOL				m_fCtrlKeyDownWhenStopCalled;
	DWORD				m_dwTransitionPlayFlags;
	IDirectMusicSegment	*m_pIDMTransitionSegment;

	// Used to display the 'Segment Length' dialog when a segment is first created
	bool				m_fBrandNew;

	// Set when the segment's property page is active
	bool				m_fPropPageActive;

	REFERENCE_TIME		m_rtCurrentStartTime;
    REFERENCE_TIME      m_rtCurrentLoopStart;
    REFERENCE_TIME      m_rtCurrentLoopEnd;
	MUSIC_TIME			m_mtCurrentStartTime;
	MUSIC_TIME			m_mtCurrentStartPoint;
	MUSIC_TIME			m_mtCurrentLoopStart;
	MUSIC_TIME			m_mtCurrentLoopEnd;
	DWORD				m_dwCurrentMaxLoopRepeats;

	// Design time parameters that are persisted
	PPGSegment			m_PPGSegment;
	double				m_dblZoom;
	long				m_lVerticalScroll;
	double				m_dblHorizontalScroll;
	SequenceQuantize	m_SequenceQuantizeParms;
	SequenceVelocitize	m_SequenceVelocitizeParams;
	DMUSPROD_TIMELINE_SNAP_TO m_tlSnapTo;
	long				m_lFunctionbarWidth;

	// IDirectMusicSegment parameters
	DWORD		m_dwLoopRepeats;// Number of repeats. By default, 0.
	MUSIC_TIME	m_mtLength;		// Length, in music time.
	MUSIC_TIME	m_mtPlayStart;	// Start of playback. By default, 0.
	MUSIC_TIME	m_mtLoopStart;	// Start of looping portion. By default, 0.
	MUSIC_TIME	m_mtLoopEnd;	// End of loop. Must be greater than m_dwPlayStart. 
								// By default equal to length.  If 0, use m_mtLength.
	DWORD		m_dwResolution;	// Default resolution.
	DWORD		m_dwSegmentFlags;// Segment extra flags (DMUS_SEGIOF_REFLENGTH)
	REFERENCE_TIME m_rtLength;	// Length, in reference time
    REFERENCE_TIME m_rtLoopStart;
    REFERENCE_TIME m_rtLoopEnd;
	// A window so we can handle DMusic notification in a messaging thread
	CNotificationHandler	m_wndNotificationHandler;
};

#endif // __SEGMENT_H__
