// MIDIMgr.h : Declaration of the CMIDIMgr

#ifndef __MIDIMGR_H_
#define __MIDIMGR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "timeline.h"
#include "MIDI.h"
#include <dmusprod.h>
#include "MIDIStripMgrApp.h"
#include "PianoRollScrollBar.h"
#include "QuantizeDlg.h"
#include "DialogVelocity.h"
#include "RectList.h"
#include "MIDIStripMgr.h"
#include <Conductor.h>
#include <ioDMStyle.h>

#define PRIVATE_SP_CLSID		(SP_USER+45)

#define INVALID_PATCH			(0xFFFFFFFF)

#define FOURCC_START_END		mmioFOURCC('m','m','t','s')

#define TRACKCONFIG_VALID_MASK (DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_CONTROL_PLAY | DMUS_TRACKCONFIG_PLAY_CLOCKTIME | DMUS_TRACKCONFIG_NOTIFICATION_ENABLED | DMUS_TRACKCONFIG_CONTROL_NOTIFICATION | DMUS_TRACKCONFIG_OVERRIDE_ALL | DMUS_TRACKCONFIG_OVERRIDE_PRIMARY | DMUS_TRACKCONFIG_FALLBACK | DMUS_TRACKCONFIG_CONTROL_ENABLED )

typedef enum tagPRSDRAWTYPE
{
	PRS_DT_MAXIMIZED,
	PRS_DT_HYBRID,
	PRS_DT_ACCIDENTAL,
	PRS_DT_MINIMIZED
} PRS_DRAWTYPE;

typedef void (CPianoRollStrip::*DRAW_NOTE_FUNC)(BOOL fSelected, HDC hDC, CDirectMusicStyleNote *pDMNote, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, MUSIC_TIME mtPartLength, MUSIC_TIME mtOffset);

typedef void EACHPART_CALLBACK( CDirectMusicPartRef *pPartRef, MUSIC_TIME mtPartOffset, long lGridLength, void *pThis, void *pData );
void CallFnForEachPart( CDirectMusicPartRef *pPartRef, EACHPART_CALLBACK eachPartFn, void *pThis, void *pData );

// Utility functions
DWORD MapVariations( DWORD dwOrigVaritions, DWORD dwVariationMap[32] );

// stuff to identify the current mouse mode
typedef enum tagPRSMOUSEMODE
{
	PRS_MM_NORMAL,
	PRS_MM_MOVE,
	PRS_MM_ACTIVEMOVE,
	PRS_MM_RESIZE_START,
	PRS_MM_ACTIVERESIZE_START,
	PRS_MM_RESIZE_END,
	PRS_MM_ACTIVERESIZE_END,
	PRS_MM_RESIZE_VELOCITY,
	PRS_MM_ACTIVERESIZE_VELOCITY,
	PRS_MM_ACTIVEDRAWBOX,
	PRS_MM_ACTIVESELECT_VARIATIONS,
	PRS_MM_ACTIVEPRESS_VARIATIONCHOICES,
} PRS_MOUSEMODE;

typedef enum tagSELECTING
{
	PRS_NO_SELECT,
	PRS_SINGLE_SELECT,
	PRS_MULTIPLE_SELECT,
} PRS_SELECTING;

typedef enum tagSNAPTO
{
	SNAP_GRID,
	SNAP_BEAT,
	SNAP_BAR,
	SNAP_NONE,
} SNAPTO;

struct MelodyLoadStructure
{
	DWORD dwPChannel;
	ioPianoRollDesign prdDesign;
};

#define VELOCITY_SCALE_FACTOR 0.5

class CCurveStrip;
class CDllDMUSProdDataObject;
class CPianoRollStrip;
class CDirectMusicPart;
class CDirectMusicPartRef;
class CDirectMusicPattern;
class CRectList;
class CNotePropPageMgr;
class CChordTrack;
interface IDMUSProdPChannelName;
interface IDMUSProdConductor;

/////////////////////////////////////////////////////////////////////////////
// CMIDIMgr
class ATL_NO_VTABLE CMIDIMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMIDIMgr, &CLSID_MIDIMgr>,
	public IMIDIMgr,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject,
	public IDMUSProdNotifyCPt,
	public IDMUSProdPortNotify
{
friend class CPianoRollStrip;
friend class CCurveStrip;
friend class CDirectMusicPart;
friend class CDirectMusicPartRef;
friend class CDirectMusicPattern;
friend class CDialogNewPart;
friend class CNotePropPageMgr;
friend class CPianoRollPropPageMgr;
friend class CCurvePropPageMgr;
friend class CVarSwitchStrip;
friend class CMarkerPropPageMgr;

public:
	CMIDIMgr();
	~CMIDIMgr();

public:
	BOOL				m_fHasNotationStation; // TRUE if we have the NotationStation font installed
	IDMUSProdTimeline*	m_pTimeline;

protected:
//	int					m_nParts;
//	CMNotePtrArray		m_MNotePtrArray;
	BOOL				m_fDirty; // TRUE if any of our parts have changed
	BOOL				m_fPropPageActive; // TRUE iff the Note property page is displayed
	int					m_nUndoString;
	CString				m_strUndoString;
	BOOL				m_fShuttingDown;
	BOOL				m_fPChannelChange;

	// Since there is only one property page, we don't need an array of these.
	// However, an array may become more useful in the future.
	NotePropData		m_NotePropData;
	CPropNote			m_SelectedPropNote;
	CNotePropPageMgr*	m_pPropPageMgr;

//	long				m_lLastTrackOpened;

	// Piano Roll Strips
	PianoRollData		m_PianoRollData;
	CTypedPtrList<CPtrList, CPianoRollStrip*> m_pPRSList;

	// Chord strip
	CChordTrack*		m_pChordTrack;
	DMUS_CHORD_PARAM	m_chordDefault;
	BOOL				m_fDefaultChordFlatsNotSharps;
	BOOL				m_fChordStripChanged;

	CDirectMusicPattern*	m_pDMPattern;
	CTypedPtrList<CPtrList, CDirectMusicPart*> m_lstStyleParts;
	CDllDMUSProdDataObject*	m_pDragDataObject;
	IDataObject*			m_pCopyIDataObject;

	IDMUSProdFramework*		m_pIFramework;
	IDMUSProdConductor*		m_pIConductor;
	IDMUSProdPChannelName*	m_pIPChannelName;
	IDirectMusicPerformance8* m_pIDMPerformance;
	IDMUSProdNode*			m_pIStyleNode;	
	IDMUSProdNode*			m_pDMProdSegmentNode;	

	DirectMusicTimeSig	m_TimeSignature;	 // The default time signature
	DWORD				m_dwNextPatternID;

	TIMELINE_PASTE_TYPE	m_ptPasteType;
	HANDLE				m_hStatusBar;

	MUSIC_TIME			m_mtLastLoop;	// Time of last loop (or the start of the segment, if no loops)
	IDirectMusicSegmentState* m_pSegmentState;
	BOOL				m_fNoteInserted;
	BOOL				m_fUpdateDirectMusic;

	// Hybrid notation options
	BOOL			m_fDisplayingFlats;
	int				m_nNumAccidentals;
	int				m_nKeyRoot;
	int				m_aiScalePattern[7]; // Current scale pattern

	// Quantization parameters
	QUANTIZE_TARGET		m_qtQuantTarget;
	DWORD				m_dwQuantFlags;
	BYTE				m_bQuantResolution;
	BYTE				m_bQuantStrength;

	// Velocity parameters
	VELOCITY_TARGET		m_vtVelocityTarget;
	bool				m_fVelocityAbsolue;
	bool				m_fVelocityPercent;
	LONG				m_lVelocityAbsChangeStart;
	LONG				m_lVelocityAbsChangeEnd;
	BYTE				m_bVelocityCompressMin;
	BYTE				m_bVelocityCompressMax;

	// State variables for clipping off played notes when dragging them around
	REFERENCE_TIME		m_rtLastPlayNoteOffTime;
	DWORD				m_dwLastPlayNotePChannel;
	BYTE				m_bLastPlayNoteMIDIValue;

	// Segment Designer-specific stuff
	IDirectMusicTrack*			m_pIDMTrack;
	DWORD						m_dwGroupBits;
	DWORD						m_dwOldGroupBits;
	DWORD						m_dwIndex;;
	DWORD						m_dwTrackExtrasFlags;
	DWORD						m_dwProducerOnlyFlags;
	MUSIC_TIME					m_mtCurrentLength;
	MUSIC_TIME					m_mtCurrentLoopStart;
	MUSIC_TIME					m_mtCurrentLoopEnd;
	DWORD						m_dwCurrentMaxLoopRepeats;
	BYTE						m_bTempPatternEditorMode;

	// Melody-generation recompose helper list
	CTypedPtrList< CPtrList, MelodyLoadStructure *> m_lstMelodyLoadStructures;

public:

DECLARE_REGISTRY_RESOURCEID(IDR_MIDIMGR)

BEGIN_COM_MAP(CMIDIMgr)
	COM_INTERFACE_ENTRY_IID(IID_IMIDIMgr,IMIDIMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
	COM_INTERFACE_ENTRY(IDMUSProdNotifyCPt)
	COM_INTERFACE_ENTRY(IDMUSProdPortNotify)
END_COM_MAP()

// IViewObjectEx
	STDMETHOD(GetViewStatus)(DWORD* pdwStatus)
	{
		*pdwStatus = VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE;
		return S_OK;
	}


// IPersist functions
    STDMETHOD(GetClassID)( CLSID* pClsId );

// IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( /* [retval][out] */ void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( /* [in] */ void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties( void);
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void);

// IMIDIMgr
	HRESULT STDMETHODCALLTYPE GetUndoText( /*[out]*/ BOOL* pfUpdateDirectMusic,
				/*[out,retval]*/ BSTR* pbstrUndoText );
	HRESULT STDMETHODCALLTYPE OnRecord( /*[in]*/ BOOL fEnableRecord );
	HRESULT STDMETHODCALLTYPE MergeVariations( 	/*[in]*/ BOOL fChangeData );
	HRESULT STDMETHODCALLTYPE SetSegmentState( /*[in]*/ IUnknown* punkSegmentState );
	HRESULT STDMETHODCALLTYPE CanDeleteTrack();
	HRESULT STDMETHODCALLTYPE DeleteTrack();
	HRESULT STDMETHODCALLTYPE Activate( /*[in]*/ BOOL fActive );
	HRESULT STDMETHODCALLTYPE AddNewStrip();

// IDMUSProdNotifyCPt
	HRESULT STDMETHODCALLTYPE OnNotify( /* [in] */ ConductorNotifyEvent *pConductorNotifyEvent);

// IDMUSProdPortNotify
	HRESULT STDMETHODCALLTYPE OnOutputPortsChanged( void );
	HRESULT STDMETHODCALLTYPE OnOutputPortsRemoved( void );

// IDMUSProdStripMgr
public:
	HRESULT STDMETHODCALLTYPE IsParamSupported(
		/* [in] */ REFGUID		guidType);
	HRESULT STDMETHODCALLTYPE GetParam(
		/* [in] */  REFGUID		guidType,
		/* [in] */  MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME*	pmtNext,
		/* [out] */ void*		pData);
	HRESULT STDMETHODCALLTYPE SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData);
	HRESULT STDMETHODCALLTYPE OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData);
	HRESULT STDMETHODCALLTYPE GetStripMgrProperty( 
		/* [in] */ STRIPMGRPROPERTY stripMgrProperty,
		/* [out] */ VARIANT*	pVariant);
	HRESULT STDMETHODCALLTYPE SetStripMgrProperty( 
		/* [in] */ STRIPMGRPROPERTY smp,
		/* [in] */ VARIANT		variant);

public:
	void	PlayNote( const CPropNote *pPropNote, DWORD dwPChannel );
	void	PlayPatch( MUSIC_TIME mtTime, DWORD dwPChannel );
	void	StopNote( void );
	HRESULT SendPMsg( DMUS_PMSG *pPMsg );
	void	RefreshPartDisplay( CDirectMusicPart* pPart, DWORD dwVariations, BOOL fRefreshCurveStrips, BOOL fRefreshMarkerStrip );
	void	RefreshPartRefDisplay( const CDirectMusicPartRef* pPartRef );
	void	RefreshCurveStripStateLists( CDirectMusicPart* pPart );
	void	RefreshPropertyPage( CDirectMusicPart* pPart );
	CDirectMusicStyleNote*	DMPartToDMNote( CDirectMusicPart* pPart ) const;
	CPianoRollStrip*		DMPartToStrip( CDirectMusicPart* pPart ) const;
	CPianoRollStrip*		DMPartRefToStrip( CDirectMusicPartRef* pPartRef ) const;

protected:
	HRESULT ImportNoteList( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, PianoRollClipboardData *pPRCD, IStream* pIStream, MUSIC_TIME mtGrid, long lDataSize, BOOL &fChanged, BOOL fUpdatePatternEditorIfNeeded );
	HRESULT ImportCurveList( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, PianoRollClipboardData *pPRCD, IStream* pIStream, MUSIC_TIME mtGrid, long lDataSize, BOOL &fChanged );
	HRESULT ImportMarkerList( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, PianoRollClipboardData *pPRCD, IStream* pIStream, MUSIC_TIME mtGrid, long lDataSize, BOOL &fChanged );
	HRESULT ImportEventList( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, PianoRollClipboardData *pPRCD, IStream* pIStream, MUSIC_TIME mtGrid, BOOL &fChanged );
	HRESULT ImportSeqEventList( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, IStream* pIStream, MUSIC_TIME mtGrid, BOOL fChanged);
	HRESULT ImportSeqNoteChunk( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, IStream* pIStream, MUSIC_TIME mtGrid, long lDataSize, BOOL &fChanged );
	HRESULT ImportSeqCurveChunk( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, IStream* pIStream, MUSIC_TIME mtGrid, long lDataSize, BOOL &fChanged );
	HRESULT ImportEventsFromMIDIFile( CDirectMusicPartRef* pDMPartRef, DWORD dwVariations, IStream* pIStream, TIMELINE_PASTE_TYPE ptPasteType, MUSIC_TIME mtGrid );
	HRESULT ImportMIDIFileToMultipleStrips( IStream* pStream );
	HRESULT SelectNote( CPianoRollStrip* pPRS, CDirectMusicStyleNote *pDMNote );
	HRESULT UnselectNote( CPianoRollStrip* pPRS, CDirectMusicStyleNote *pDMNote );
	HRESULT UnselectAllNotes( CDirectMusicPart* pPart, DWORD dwVariations );
	HRESULT UnselectAllEvents( CDirectMusicPart* pPart, DWORD dwVariations );
	void	DeleteSelectedNotes( CDirectMusicPartRef* pPartRef, DWORD dwVariations );
	void	DeleteSelectedEvents( CDirectMusicPartRef* pPartRef, DWORD dwVariations );
	void	SelectAllNotes( CPianoRollStrip* pPRS, DWORD dwVariations );
	void	MergeVariations( CDirectMusicPartRef* pPartRef, DWORD dwVariations );
	HRESULT SaveSelectedTime( long lGridStart, long lGridEnd, IStream *pIStream );
	HRESULT	SaveSelectedNoteList( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IStream* pIStream );
	HRESULT	SaveSelectedCurveList( CDirectMusicPart* pPart, DWORD dwVariations, long lGridStart, IStream* pIStream );
	HRESULT	SaveSelectedMarkerList( CDirectMusicPart* pPart, DWORD dwVariations, long lGridStart, IStream* pIStream );
	HRESULT	SaveSelectedEvents( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lStart, long lEnd, IStream* pIStream );
	HRESULT	SaveSelectedNoteListForSeqTrack( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IStream* pIStream );
	HRESULT	SaveSelectedCurveListForSeqTrack( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IStream* pIStream );
	HRESULT	SaveSelectedEventsForSeqTrack( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lStart, IStream* pIStream );
	HRESULT SaveSelectedSeqNoteChunk( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveSelectedSeqCurveChunk( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveSelectedNotesForMidi( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, IStream* pIStream );
	HRESULT SaveSelectedEventsInAllSelectedStripsForMidi( long lStartGrid, IStream* pIStream );
	void	StopDisplayUpdate();
	void	StartDisplayUpdate();
	void	InsertNote( CDirectMusicPartRef* pPartRef, CDirectMusicStyleNote *pDMNote, BOOL fUpdate );
	void	KillFocus( CPianoRollStrip* pPRStrip );
	void	SetFocus( CPianoRollStrip* pPRStrip, int iFocus ); // 1 = PianoRoll, 2 = Curve, 3 = Marker
	int		ComputeSelectedPropNote( CDirectMusicPart* pPart, DWORD dwVariations ); // return the # of selected notes
	void	PreChangePartRef( CDirectMusicPartRef* pPartRef );
	long	EarliestSelectedNote( CDirectMusicPart *pPart, DWORD dwVariations );
	void	DeleteNotesBetweenBoundaries( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, long lGridEnd, BOOL &fChanged );
	void	DeleteCurvesBetweenBoundaries( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, long lGridEnd, BOOL &fChanged );
	void	DeleteMarkersBetweenBoundaries( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lGridStart, long lGridEnd, BOOL &fChanged );
	void	DeleteEventsBetweenBoundaries( AList &lstEvents, DWORD dwVariations, long lGridStart, long lGridEnd );
	CDirectMusicEventItem *DeleteEvent( CDirectMusicEventItem* pEvent, AList &lstEvents, DWORD dwVariations );
	void	ValidateActiveDMNote( CDirectMusicPart* pPart, DWORD dwVariations );

	HRESULT OnDeletePart( CPianoRollStrip* pPRSClickedOn );
	HRESULT RemoveStrip( CPianoRollStrip* pPRS );
	DWORD	DeterminePositionForPianoRollStrip( const CDirectMusicPartRef* pPartRef ) const;
	void	UpdatePositionOfStrips( const CDirectMusicPartRef* pPartRef );
	
	HRESULT InsertDefaultChord();
	HRESULT AddChordTrack( CChordTrack* pChordTrack, IStream* pIStream );
	HRESULT SaveChordStripDesignData( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT LoadChordStripDesignData( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT CreateDefaultChordStrip();
	HRESULT GetChordFromChordStrip( MUSIC_TIME mt, DMUS_CHORD_PARAM* pchordData, BOOL* pfFlatsNotSharps );
	HRESULT GetChord( CDirectMusicPart *pDMPart, const CDirectMusicStyleNote *pDMNote, DMUS_CHORD_PARAM* pchordData );

	HRESULT SavePianoRollDesignData( IDMUSProdRIFFStream* pIRiffStream, CDirectMusicPartRef* pPartRef );
	HRESULT LoadPianoRollDesignData( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, CDirectMusicPartRef* pPartRef );
	HRESULT CreateDefaultPianoRollStrip( CDirectMusicPartRef* pPartRef );
	HRESULT	UpdatePartParamsAfterChange( CDirectMusicPart* pPart );
	void	UpdateOnDataChanged( int nUndoString );
	void	SetVarLocksArray( PianoRollData* pPRD ) const;
	CPianoRollStrip* CreatePianoRollStrip( CDirectMusicPartRef* pDMPartRef );
	HRESULT SaveQuantizeDesignData( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT LoadQuantizeDesignData( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT SaveVelocitizeDesignData( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT LoadVelocitizeDesignData( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT SaveTimelineDesignData( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT LoadTimelineDesignData( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	void	QuantizeSelectedNotes( CDirectMusicPart* pPart, DWORD dwVariations );
	void	QuantizeEntirePart( CDirectMusicPart* pPart );
	void	QuantizeEntirePattern();
	void	VelocitizeSelectedNotes( CDirectMusicPart* pPart, DWORD dwVariations );
	void	VelocitizeEntirePart( CDirectMusicPart* pPart );
	void	VelocitizeEntirePattern();
	BOOL	OffsetSelectedNoteStart( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lMoveTick, long lMoveGrid );
	BOOL	OffsetSelectedNoteValuePosition( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lMoveTick, long lMoveGrid, int nMIDIValue, BOOL fMakeCopy );
	BOOL	OffsetSelectedNoteDurationAndVelocity( CDirectMusicPartRef* pPartRef, DWORD dwVariations, long lDuration, long lVelocity, BOOL fMakeCopy );
	void	ChangeNotationType( BOOL fHybridNotation );
	void	InvalidateAllStrips( void );
	bool	ParentNotePPGShowing( void );
    HRESULT CreateStreamFromPatternTrack(IStream** ppIStream);
	void	UpdateStatusBarDisplay( void );
	void	RefreshVarChoicesEditor( const CDirectMusicPart* pPart );
	void	SetVarsForPartPChannel( const DWORD dwVariations, const CDirectMusicPartRef *pPartRef );
	void	UpdateAuditionVariations( void );

	// Segment Specific
	void	AddPianoRollStripsToTimeline( void );
	HRESULT	CreateDefaultPattern( void );
	void	UpdateDirectMusicTrack( void );
	void	UpdateChordTrackEditorMode( void );

	// Melody generation specific
	void	PopulateMelodyLoadStructureList( void );

	// Internal functions
private:
	DWORD	ApplyToSelectedNotes( CDirectMusicPartRef* pPartRef, DWORD dwVariations, const CPropNote* pPropNote );
	//void	PropagateGUID( GUID oldGuid, GUID newGuid );
	DWORD	SelectedNoteVariations( CDirectMusicPart* pPart );
	BOOL	UpdateSelectedParamsFromActiveStrip( void );
	BOOL	UpdateFlatsAndKey();
	void	UpdateKeyPattern( void );

	inline BOOL ValidPartGUID( GUID guid )
	{
		ASSERT(!IsEqualGUID( guid, GUID_AllZeros ));
		ASSERT(FindPartByGUID( guid ) != NULL);
		if ( (IsEqualGUID(guid, GUID_AllZeros)) || (FindPartByGUID(guid) == NULL) )
		{
			return FALSE;
		}
		return TRUE;
	}

	BOOL				m_fWindowActive; // TRUE if we allow MIDI input, FALSE if we don't
	BOOL				m_fRecordEnabled; // TRUE if record button pressed, FALSE if not
	BOOL				m_fNoDisplayUpdate; // If this is set, then don't update the property page or any of the strips.
	CPianoRollStrip*	m_pActivePianoRollStrip;
	int					m_iFocus;
	CRITICAL_SECTION	m_critsecUpdate;
	BOOL				m_fCriticalSectionValid;

	// Part support routines
protected:
	CDirectMusicPart* FindPartByGUID( GUID guidPartID );
	CDirectMusicPart* FindPartByTrack( long nPart );
	void DeletePart( CDirectMusicPart* pPart );
	CDirectMusicPart* AllocPart( void );
};

class CPianoRollStrip : public IDMUSProdStrip, 
	public IDMUSProdStripFunctionBar,
	public IDMUSProdPropPageObject,
	public IDMUSProdTimelineEdit,
	public IDMUSProdMidiInCPt
{
friend class CMIDIMgr;
friend class CCurveStrip;
friend class CCurvePropPageMgr;
friend class CCurveTracker;
friend class CPropNote;
friend class CNoteTracker;
friend class CNotePropPageMgr;
friend class CPianoRollPropPageMgr;
friend class CDirectMusicPattern;
friend class CDirectMusicPart;
friend class CDialogNewPart;
friend class CVarSwitchStrip;
friend class CMarkerPropPageMgr;

protected:
	long				m_cRef; // reference count
	CMIDIMgr*			m_pMIDIMgr; // pointer to our MIDI manager
	IDMUSProdStripMgr*	m_pStripMgr; // pointer to our strip manager's interface (same as m_pMINIMgr)
	
	BOOL			m_fGutterSelected; // flag for our having gutter selected
	CPianoRollPropPageMgr*	m_pPropPageMgr;
	CDirectMusicStyleNote*	m_pActiveDMNote;
	CDirectMusicStyleNote*	m_pShiftDMNote;
	POINT			m_pointClicked;
	CDirectMusicPartRef*	m_pPartRef;
//	CDirectMusicStyleNote			m_OriginalMNote;
	CPropNote		m_OriginalPropNote;
	PRS_SELECTING	m_prsSelecting;
	long			m_lLastDeltaStart;
	char			m_cLastDeltaValue;
	long			m_lLastDeltaVel;
	long			m_lLastDeltaDur;
//	PropPageNoteParams m_PPNParams;
	long			m_lBeginTimelineSelection;
	long			m_lEndTimelineSelection;
	POINT			m_pointSelection;
	long			m_lInsertVal;
	long			m_lInsertTime;
	BOOL			m_fPropPageActive;
	BOOL			m_fRefreshCurveProperties;
	BOOL			m_fRefreshVarSwitchProperties;
	BOOL			m_fCtrlKeyDown; // If set, then a ctrl-click was on (1) selected note, or (2) unselected note
	BOOL			m_fPianoRollMenu;
	BOOL			m_fScrollTimerActive;
	bool			m_fVariationsTimerActive;
	CPianoRollScrollBar	m_prScrollBar;
	//CToolTipCtrl	m_ToolTipCtrl;
	BOOL			m_fInSetData;
	BOOL			m_fInShowProperties;
	MUSIC_TIME		m_mtTimelineLengthForGetNoteRect;
	long			m_lLastHeight;
	MUSIC_TIME		m_mtLastEarlyTime;
	MUSIC_TIME		m_mtLastLateTime;
	bool			m_fInsertingNoteFromMouseClick;
	CCurveStrip		*m_pActiveCurveStrip;

	// Reference counter for bitmaps
	static long		m_lBitmapRefCount;

	// Variation Button bar
	static CBitmap	m_BitmapBlankButton;
	static CBitmap	m_BitmapPressedButton;
	static CBitmap	m_BitmapBlankInactiveButton;
	static CBitmap	m_BitmapPressedInactiveButton;
	static CBitmap	m_BitmapGutter;
	static CBitmap	m_BitmapPressedGutter;
	static CBitmap	m_BitmapMoaw;
	static CBitmap	m_BitmapPressedMoaw;
	BOOL			m_fGutterPressed;
	BOOL			m_fVariationChoicesPressed;

	// Drawing
	BOOL			m_fDontRedraw;
	CRectList		m_aNoteRectList[128];
	int				m_aiAccidentals[75];	// Current Accidental
	HCURSOR 		m_hCursor;
	PRS_MOUSEMODE	m_MouseMode; // current Mouse Mode
	DWORD			m_dwLastPatch;
	DWORD			m_dwPlayingVariation; // Which variation is currently playing

	// Bitmap status variables
	BOOL			m_fZoomInPressed;
	BOOL			m_fZoomOutPressed;
	BOOL			m_fNewBandPressed;
	BOOL			m_fInstrumentPressed;
	BOOL			m_fInstrumentEnabled;
	static CBitmap	m_BitmapZoomInUp;
	static CBitmap	m_BitmapZoomInDown;
	static CBitmap	m_BitmapZoomOutUp;
	static CBitmap	m_BitmapZoomOutDown;
	static CBitmap	m_BitmapNewBand;

	// MIDI input
	DWORD			m_dwCookie;			// Cookie for MIDI input
	BOOL			m_fMIDIInEnabled;	// True if MIDI input enabled
	BOOL			m_fMIDIThruEnabled;	// True if MIDI thruing enabled
	IDMUSProdConductor*	m_pIConductor;
	MUSIC_TIME		m_mtStartTimeOffset[128]; // Start time, in segment time, for all 128 MIDI notes. 0 == none
	MUSIC_TIME		m_mtStartTime[128];	// Start time, in perf. time, for all 128 MIDI notes.  0 == none
	BYTE			m_bVelocity[128];	// Velocity of all 128 MIDI notes

// State variables for the variation selection bar
	int				m_nLastVariation;
	int				m_nSoloVariation;
	BOOL			m_fSolo;
	BOOL			m_fEnablingVariations;
	DWORD			m_dwOldVariations;

	long			m_lMeasureClocks;
	long			m_lBeatClocks;
	long			m_lGridClocks;

	CRect			m_rectScrollBar;
	DWORD			m_dwPlatformId;

	BYTE			m_bMinimizeTopNote;
	BYTE			m_bMinimizeNoteRange;

	// Data that needs persisting
	DWORD		m_dwVariations;		// Which variations to display and play
	COLORREF	m_crSelectedNoteColor;
	COLORREF	m_crUnselectedNoteColor;
	COLORREF	m_crOverlappingNoteColor;
	COLORREF	m_crAccidentalColor;
	double		m_dblVerticalZoom;
	long		m_lMaxNoteHeight;
	long		m_lVerticalScroll;
	STRIPVIEW	m_StripView;
	long		m_lSnapValue;		// # of clocks to move by when mouse editing
	BOOL		m_fHybridNotation;
	DWORD		m_dwExtraBars;		// Number of extra bars to display after the pattern
	BOOL		m_fPickupBar;

	// Curve Strips
	STRIPVIEW	m_CurveStripView;
	CTypedPtrList<CPtrList, CCurveStrip*> m_lstCurveStrips;
	CTypedPtrList<CPtrList, ioCurveStripState*> m_lstCurveStripStates;	// Used when loading in a Pattern.

	// Marker strip
	CVarSwitchStrip	*m_pVarSwitchStrip;

public:
	CPianoRollStrip( CMIDIMgr* pMIDIMgr, CDirectMusicPartRef *pPartRef );
	~CPianoRollStrip();

// IUnknown
public:
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
public:
// IDMUSProdStrip
	HRESULT	STDMETHODCALLTYPE	Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
    HRESULT STDMETHODCALLTYPE	GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
    HRESULT STDMETHODCALLTYPE	SetStripProperty( STRIPPROPERTY sp, VARIANT var);
	HRESULT STDMETHODCALLTYPE	OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar
	HRESULT	STDMETHODCALLTYPE	FBDraw( HDC hDC, STRIPVIEW sv );
	HRESULT STDMETHODCALLTYPE	FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( /* [retval][out] */ void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( /* [in] */ void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties( void);
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void);

	// IDMUSProdTimelineEdit
public:
	HRESULT STDMETHODCALLTYPE Cut( /* in */ IDMUSProdTimelineDataObject *pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Copy( /* in */ IDMUSProdTimelineDataObject *pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( /* in */ IDMUSProdTimelineDataObject *pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanCut( void );
	HRESULT STDMETHODCALLTYPE CanCopy( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject *pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE CanInsert( void );
	HRESULT STDMETHODCALLTYPE CanDelete( void );
	HRESULT STDMETHODCALLTYPE CanSelectAll( void );

	// IDMUSProdMidiInCPt functions
public:
	HRESULT STDMETHODCALLTYPE OnMidiMsg(REFERENCE_TIME rtTime, 
										BYTE bStatus, 
										BYTE bData1, 
										BYTE bData2);
	// Useful utility stuff
public:
	HRESULT DoQuantize( void );
	HRESULT DoVelocity( void );
	HRESULT STDMETHODCALLTYPE GetDataConst( /* [retval][out] */ void **ppData) const;
	void	OnVScrollFromScrollbar(UINT nSBCode, UINT nPos);
	static BOOL	CurveStripExists( void *pVoid, BYTE bCCType, WORD wRPNType );
	HRESULT SaveCurveStripStateData( IDMUSProdRIFFStream* pIRiffStream );
	void	SyncCurveStripStateList( void );
	BOOL	GetStripRect(CRect& rectStrip);
	CString	GetName( void );
	void	SendAllNotesOffIfNotPlaying( void );
	LRESULT OnApp( WPARAM wParam, LPARAM lParam );
	void	OnVariationTimer( void );

	//private stuff
protected:
	void	InitializePianoRollData( PianoRollData* pPRD ) const;
	ioCurveStripState* GetCurveStripState( BYTE bCCType, WORD wRPNType );
	CCurveStrip* GetCurveStrip( BYTE bCCType, WORD wRPNType );
	HRESULT AddCurveStrip( BYTE bCCType, WORD wRPNType );
	HRESULT AddCurveStrips( BOOL fUseActivateFlag );
	HRESULT RemoveCurveStrip( CCurveStrip* pCurveStrip );
	HRESULT OnDeleteCurveStrip( CCurveStrip* pCurveStrip );
	HRESULT OnNewCurveStrip( void );
	DWORD	DeterminePositionForCurveStrip(BYTE bCCType, WORD wRPNType);
	static BYTE CurveTypeToStripCCType( CDirectMusicStyleCurve* pDMCurve );
	void    OnChangeStripView( STRIPVIEW svNewStripView );
	void    OnChangeCurveStripView( STRIPVIEW svNewStripView );
	void	InvalidateCurveStrips( void );
	void	InvalidateMarkerStrip( void );
	void	UpdateCurveStripGutterSelection( BOOL fChanged );
	void	UpdateMarkerStripGutterSelection( BOOL fChanged );
	void	BumpTimeCursor( BOOL fBumpRight, SNAPTO stSnapSetting );
	void	EnsureNoteCursorVisible( void );
	CDirectMusicStyleNote *CreateNoteToInsert( void );
	void	SegmentDisplayVarChoicesDlg( void );
	void	UpdateInstrumentName( void );
	void	UpdatePlayingVariation( void );

// drawing helpers
	void	DrawNotesHelper( BOOL fSelected, PRS_DRAWTYPE prsDrawType, HDC hDC, CDirectMusicStyleNote *pDMNote, MUSIC_TIME mtStartTime, MUSIC_TIME mtEndTime, long lXOffset, int nTopNote, int nBottomNote, BOOL fLoopingPart, DRAW_NOTE_FUNC DrawNoteFunc );
	void	DrawMinimizedNotes( BOOL fSelected, HDC hDC, CDirectMusicStyleNote *pDMNote, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, MUSIC_TIME mtPartLength, MUSIC_TIME mtOffset );
	void	DrawMaximizedNotes( BOOL fSelected, HDC hDC, CDirectMusicStyleNote *pDMNote, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, MUSIC_TIME mtPartLength, MUSIC_TIME mtOffset );
	void	DrawHybridMaximizedNotes( BOOL fSelected, HDC hDC, CDirectMusicStyleNote *pDMNote, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, MUSIC_TIME mtPartLength, MUSIC_TIME mtOffset );
	void	DrawHybridMaximizedAccidentals( BOOL fSelected, HDC hDC, CDirectMusicStyleNote *pDMNote, long lStartTime, long lEndTime, long lXOffset, int nTopNote, int nBottomNote, MUSIC_TIME mtPartLength, MUSIC_TIME mtOffset );
	void	DrawFunctionBar( HDC hDC, STRIPVIEW sv );
	void	DrawHorizontalLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote );
	void	DrawHybridLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote );
	void	DrawDarkHorizontalLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote );
	void	DrawDarkHybridLines( HDC hDC, const RECT &rectClip, int nBottomNote, int nTopNote );
	void	DrawVariationButtonBar( HDC hDC );
	HRESULT	DrawVerticalines( HDC hDC, long lXOffset );
	CDirectMusicStyleNote* GetFirstVisibleNote( long lStartTime, long lEndTime, int nTopNote, int nBottomNote );
	void	InitializeScaleAccidentals( void );
	void	DrawNoteInsertionMark( HDC hDC, MUSIC_TIME mtStartTime, MUSIC_TIME mtEndTime, long lXOffset );
	HFONT	GetAccidentalFont( void );

	void	GetNoteTopBottomRect( CDirectMusicStyleNote* pDMNote, RECT* pRect );
	void	GetNoteRect( CDirectMusicStyleNote* pDMNote, RECT* pRect );
	void	GetHybridTopBottomRect( RECT* pRect, int iHybridPos, int iVelocity );
	void	GetHybridRect( RECT* pRect, int iHybridPos, int iVelocity, MUSIC_TIME mtStart, MUSIC_TIME mtDuration );
	HRESULT OnLButtonDown( WPARAM wParam, long lXPos, long lYPos);
	HRESULT OnSetCursor( long lXPos, long lYPos);
	HRESULT OnMouseMove( long lXPos, long lYPos);
	HRESULT OnLButtonUp( long lXPos, long lYPos);
	HRESULT OnRButtonUp( void );
	HRESULT OnKeyDown( WPARAM wParam, LPARAM lParam);
	HRESULT OnChar( WPARAM wParam );
	HRESULT OnVScroll( void );
	HRESULT OnSize( void );
	HRESULT OnDestroy( void );
	HRESULT OnCreate( void );
	void	OnTimer( void );
	void	KillTimer( void );
	void	EnableTimer( void );
	void	EnableVariationTimer( void );
	void	KillVariationTimer( void );
	CDirectMusicStyleNote*	GetDMNoteAndRectFromPoint( long lXPos, long lYPos, RECT *pRect );
	void	SetMouseMode( long lXPos, long lYPos);
	HCURSOR GetResizeStartCursor( void );
	HCURSOR GetResizeEndCursor( void );
	HCURSOR GetArrowCursor( void );
	HCURSOR GetNSCursor( void );
	HCURSOR GetAllCursor( void );
	HRESULT SendEditToTimeline(WPARAM wId);
	BOOL	SelectNotesInRect( CRect* pRect, int *pnSelected );
	BOOL	SelectEventsBetweenTimes( long lStart, long lEnd, int *pnSelected );
	BOOL	SelectOnlyEventsBetweenTimesHelper( long lStart, long lEnd, int *pnSelected );
	BOOL	SelectEventsBetweenTimesHelper( long lStart, long lEnd, int *pnSelected );
	BOOL	UnSelectEventsBetweenTimesHelper( long lStart, long lEnd );
	void	AdjustScroll(long lXPos, long lYPos);
	void	SetNewVerticalScroll( long lNewVertScroll );
	HRESULT	OnVariationBarMsg( UINT nMsg, WPARAM wParam, LPARAM lParam, long lXPos, long lYPos);
	void	InvalidatePianoRoll( void );
	void	InvalidateFunctionBar( void );
	void	InvalidateVariationBar( void );
	void	UpdateSelectionState( void );
	void	GetDesignInfo( ioPianoRollDesign* pPianoRollDesign );
	void	SetDesignInfo( ioPianoRollDesign* pPianoRollDesign );
	void	ClipRectToWindow( const RECT* pSourceRect, RECT* pDestRect );
	BOOL	IsEnginePlaying( void );
	HRESULT	ComputeVScrollBar( void );
	BOOL	InitializeNotePropData( NotePropData *pNPD );
	CDirectMusicStyleNote* GetFirstNote( void ) const;
	CDirectMusicStyleCurve* GetFirstCurve( void ) const;
	CDirectMusicStyleMarker* GetFirstMarker( void ) const;
	CDirectMusicStyleNote* GetLastNote( void ) const;
	BOOL	FixPartRefPtr( void ) const;
	BOOL	ValidPartRefPtr( void ) const;
	BOOL	ValidPasteVariations( DWORD dwVariations );
	//void	DrawNote( HDC hDC, RECT *pRectNote, char cValue, BOOL fEarly,
	//				  HBRUSH brushHatchVert,BOOL fInverstionId, HBRUSH brushHatch,
	//				  HPEN penOverlapping );
	void	DrawOverlapping( HDC hDC, char cValue, const RECT *pRect,
							 HBRUSH hbrushHatchOverlapping, COLORREF crNoteColor );
	void	DrawSymbol( HDC hDC, const TCHAR *pstrText, int iHybridPos, long lLeftPos, long lTopAdjust, long lBottomAdjust );
	void	OnLoop( void );
	void	UnselectGutterRange( void );
	BYTE		PositionToMIDIValue( long lYPos );
	HWND	GetTimelineHWnd();
	HRESULT ReadPRCDFromStream( IStream *pIStream, PianoRollClipboardData *pPRCD );
	CDirectMusicStyleNote* GetFirstSelectedNote( void );
	void	UpdateNoteCursorTime( void );
	SNAPTO	GetSnapToBoundary( void );
	long	GetSnapAmount( void );
	void	ChangeNotationType( BOOL fHybridNotation, BOOL fUpdatePatternEditor );
	void	ChangeZoom( double dblVerticalZoom );
	BOOL	FirstGutterSelectedPianoRollStrip( void );
	void	RefreshPropertyPage( void );
	DWORD	GetNumExtraBars( void ) const;
	BOOL	ShouldDisplayPickupBar( void ) const;
	MUSIC_TIME ConvertAbsTimeToPartOffset( MUSIC_TIME mtTime ) const;
	void	SetSelectedVariations( const DWORD dwVariations );

	// MIDI Input helper methods
	HRESULT RecordStepNoteOn( BYTE bData1, BYTE bData2 );
	HRESULT RecordStepNoteOff( BYTE bData1 );
	HRESULT RecordRealTimeNoteOn( REFERENCE_TIME rtTime, BYTE bData1, BYTE bData2 );
	HRESULT RecordRealTimeNoteOff( REFERENCE_TIME rtTime, BYTE bData1 );
	HRESULT RecordRealTimeCurve( REFERENCE_TIME rtTime, BYTE bStatus, BYTE bData1, BYTE bData2 );

	// MIDI registration methods
	void	RegisterMidi();
	void	UnRegisterMidi();
	void	SetPChannelThru();
	void	CancelPChannelThru();
};

#define VARIATION_MOAW_WIDTH 28
#define VARIATION_MOAW_HEIGHT 30
#define VARIATION_GUTTER_WIDTH 8
#define VARIATION_GUTTER_HEIGHT 30
#define VARIATION_BUTTON_HEIGHT	15
#define VARIATION_BUTTON_WIDTH	17

#endif //__MIDIMGR_H_
