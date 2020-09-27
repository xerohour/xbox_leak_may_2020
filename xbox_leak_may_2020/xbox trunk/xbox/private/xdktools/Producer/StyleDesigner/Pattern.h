#ifndef __PATTERN_H__
#define __PATTERN_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Pattern.h : header file
//

#include <afxtempl.h>

#include "RiffStrm.h"
#include "RiffStructs.h"
#include "Timeline.h"
#include "alist.h"
#include "midifile.h"
#include "MidiStripMgr.h"
#include "Conductor.h"
#include "DllBasePropPageManager.h"
#include "StyleDesignerDLL.h"
#include "SharedPattern.h"

struct ioDMStylePartDesign;
class CDllJazzDataObject;
class CTabPatternPattern;
class CTabMotifMotif;
class CTabMotifLoop;
class CTabBoundaryFlags;
class CJazzUndoMan;
class CVarChoices;

// This expects a voiceid from 1-16
#define IMA25_VOICEID_TO_PCHANNEL( id )  ( ( id + 4 ) & 0xf )
#define PCHANNEL_TO_IMA25_VOICEID( id )  ( ( id + 12 ) & 0xf )

#define SUBCHORD_STANDARD_CHORD 1


// IMA25 Variation flags
#define VF_SCALE			0x0000007F   // Seven positions in the scale.    
#define VF_ACCIDENTAL		0x00000080   // Handles chords outside of scale. 
#define VF_MAJOR			0x00000100   // Handles major chords.            
#define VF_MINOR			0x00000200   // Handles minor chords.            
#define VF_ALL				0x00000400   // Handles all chord types.         
#define VF_TO1				0x00000800   // Handles transitions to 1 chord.  
#define VF_TO5				0x00001000   // Handles transitions to 5 chord.  
#define VF_SIMPLE			0x00002000   // Handles simple chords.  
#define VF_COMPLEX			0x00004000   // Handles complex chords.  

// Chord types for use by each instrument in a pattern
enum
{ 
	CHTYPE_NOTINITIALIZED = 0,
	CHTYPE_DRUM,		// superceded by CHTYPE_FIXED
						// no longer in Note Dialog's combo box selections
						// no longer in Pattern Dialog's combo box selections
	CHTYPE_BASS,		// scale + lower chord
	CHTYPE_UPPER,		// scale + upper chord
	CHTYPE_SCALEONLY, 	// scale, no chord
	CHTYPE_BASSMELODIC,
	CHTYPE_UPPERMELODIC,
	CHTYPE_NONE,		// Ignored on Pattern Dialog's menu selections.
	CHTYPE_FIXED
};

class CDirectMusicPattern;


typedef struct _PPGMotif
{
	_PPGMotif( void )
	{
		dwPageID = 0;
		pMotif = NULL;
	}

	DWORD					dwPageID;
	CDirectMusicPattern*	pMotif;
} PPGMotif;


typedef struct _PPGPattern
{
	_PPGPattern( void )
	{
		dwPageID = 1;
		pPattern = NULL;
	}

	DWORD					dwPageID;
	CDirectMusicPattern*	pPattern;
} PPGPattern;


//////////////////////////////////////////////////////////////////////
//  CMotifPropPageManager

class CMotifPropPageManager : public CDllBasePropPageManager 
{
friend class CTabMotifMotif;
friend class CTabMotifLoop;

public:
	CMotifPropPageManager();
	virtual ~CMotifPropPageManager();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
    HRESULT STDMETHODCALLTYPE RefreshData();

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:
	CTabMotifMotif*			m_pTabMotif;
	CTabMotifLoop*			m_pTabLoop;
	CTabBoundaryFlags*		m_pTabBoundary;

public:
	static short			sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CPatternPropPageManager

class CPatternPropPageManager : public CDllBasePropPageManager
{
friend class CTabPatternPattern;

public:
	CPatternPropPageManager();
	virtual ~CPatternPropPageManager();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();

	//Additional functions
private:
	void RemoveCurrentObject();

	// Member variables
private:
	CTabPatternPattern*		m_pTabPattern;
};


/////////////////////////////////////////////////////////////////////////////
// DirectMusicTimeSig structure

struct DirectMusicTimeSig
{
	// Time signatures define how many beats per measure, which note receives
	// the beat, and the grid resolution.
	DirectMusicTimeSig() : m_bBeatsPerMeasure(0), m_bBeat(0), m_wGridsPerBeat(0) { }
	BYTE	m_bBeatsPerMeasure;		// beats per measure (top of time sig)
	BYTE	m_bBeat;				// what note receives the beat (bottom of time sig.)
									// we can assume that 0 means 256th note
	WORD	m_wGridsPerBeat;		// grids per beat
};


// Event types
#define ET_NOTE		0x01
#define ET_CURVE	0x02
#define ET_MARKER	0x03


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicEventItem class

class CDirectMusicEventItem : public AListItem
{
friend class CDirectMusicStyle;
friend class CDirectMusicPattern;
friend class CDirectMusicPart;

public:
	virtual ~CDirectMusicEventItem(); 
	CDirectMusicEventItem* GetNext() { return (CDirectMusicEventItem*) AListItem::GetNext(); }

protected:
	MUSIC_TIME	m_mtGridStart;		// Grid position in track that this event belogs to.
	short		m_nTimeOffset;		// Offset, in music time, of event from designated grid position.
	DWORD		m_dwVariation;		// variation bits
	BYTE		m_bType;			// Note or curve
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicEventList class

class CDirectMusicEventList : public AList
{
public:
	virtual ~CDirectMusicEventList()	{	CDirectMusicEventItem* pEvent;	
											while( m_pHead != NULL )
											{
												pEvent = RemoveHead();
												delete pEvent;
											}
										}
    CDirectMusicEventItem *GetHead() const { return (CDirectMusicEventItem *)AList::GetHead();};
    CDirectMusicEventItem *RemoveHead() { return (CDirectMusicEventItem *)AList::RemoveHead();};
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleNote class

class CDirectMusicStyleNote : public CDirectMusicEventItem
{
friend class CDirectMusicPattern;
friend class CDirectMusicPart;

public:
	CDirectMusicStyleNote() : m_bPlayModeFlags(0) { m_bType = ET_NOTE; }

protected:
	MUSIC_TIME	m_mtDuration;		// how long this note lasts
    WORD		m_wMusicValue;		// Position in scale.
    BYTE		m_bVelocity;		// Note velocity.
    BYTE		m_bTimeRange;		// Range to randomize start time.
    BYTE		m_bDurRange;		// Range to randomize duration.
    BYTE		m_bVelRange;		// Range to randomize velocity.
	BYTE		m_bInversionId;		// Identifies inversion group to which this note belongs
	BYTE		m_bPlayModeFlags;	// can override part ref
	BYTE        m_bNoteFlags;		// values from DMUS_NOTEF_FLAGS
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleCurve class

class CDirectMusicStyleCurve : public CDirectMusicEventItem
{
friend class CDirectMusicStyle;
friend class CDirectMusicPattern;
friend class CDirectMusicPart;

public:
	CDirectMusicStyleCurve();

protected:
	void SetDefaultResetValues( MUSIC_TIME mtLength  );

protected:
	MUSIC_TIME	m_mtDuration;		// how long this curve lasts
	MUSIC_TIME	m_mtResetDuration;	// how long after the end of the curve to reset the curve
	short		m_nStartValue;		// curve's start value
	short		m_nEndValue;		// curve's end value
	short		m_nResetValue;		// the value to which to reset the curve 
    BYTE		m_bEventType;		// type of curve
	BYTE		m_bCurveShape;		// shape of curve
	BYTE		m_bCCData;			// CC#
	BYTE		m_bFlags;			// Bit 1=TRUE means to send nResetValue. Otherwise, don't.
									//	   Other bits are reserved. 
	WORD		m_wParamType;		// RPN or NRPN parameter number.
	WORD		m_wMergeIndex;		// Allows multiple parameters to be merged (pitchbend, volume, and expression.)
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleMarker class

class CDirectMusicStyleMarker : public CDirectMusicEventItem
{
friend class CDirectMusicPattern;
friend class CDirectMusicPart;

public:
	CDirectMusicStyleMarker();

protected:
    WORD        m_wMarkerFlags;   /* how the marker is used */
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart class

class CDirectMusicPart
{
friend class CDirectMusicStyle;
friend class CDirectMusicPattern;
friend class CDirectMusicPartRef;

public:
	CDirectMusicPart( CDirectMusicStyle* pStyle );
	~CDirectMusicPart();

public:
    HRESULT DM_LoadPart( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT DM_SavePart( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveNoteList( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveCurveList( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveMarkerList( IDMUSProdRIFFStream* pIRiffStream ) const;
    HRESULT DM_SavePartInfoList( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT DM_SavePartDesignData( IDMUSProdRIFFStream* pIRiffStream );

	void CopyTo( CDirectMusicPart* pDMPart );
	void SetTimeSignature( DirectMusicTimeSig timeSig, BOOL fInUndo, BOOL fStyleTimeSigChanging );
	void SetNbrMeasures( WORD wNbrMeasures );
	void UpdateFromVarChoices( IDMUSProdFramework *pFramework, IPersistStream *pIPersistStream );
	bool IsVarChoicesRowDisabled( short nRow );
	short GetVarChoicesRowType( short nRow );
	short GetVarChoicesNbrColumns( short nRow );

protected:
	CDirectMusicStyle*	  m_pStyle;
	DWORD				  m_dwUseCount;				// Nbr PartRefs using this Part
	DWORD				  m_dwHardLinkCount;		// Nbr PartRefs Hard Linked to this Part

protected:
	ioDMStylePartDesign*  m_pPartDesignData;		// UI info for Part
	DWORD				  m_dwPartDesignDataSize;	// Size of UI info

	IDMUSProdNode*		  m_pVarChoicesNode;		// Implements IDMUSProdNode for VarChoices editor
	CDirectMusicPartRef*  m_pVarChoicesPartRef;		// Part Reference for VarChoices

	CString				  m_strName;				// Part name 
	GUID				  m_guidPartID;
	DirectMusicTimeSig	  m_TimeSignature;			// can override pattern's
	WORD				  m_wNbrMeasures;			// length of the Part
	DWORD				  m_dwVariationChoices[NBR_VARIATIONS];	// MOAW choices bitfield
	BYTE	              m_bPlayModeFlags;			// see PLAYMODE flags (in ioDMStyle.h)
	BYTE				  m_bInvertUpper;			// inversion upper limit
	BYTE				  m_bInvertLower;			// inversion lower limit
	DWORD				  m_dwFlags;				// various flags
	CDirectMusicEventList m_lstEvents;				// list of events (notes, curves, etc.)
};


typedef struct PianoRollUIState
{
	void*				pPianoRollData;				// UI info for Piano Roll
	DWORD				dwPianoRollDataSize;		// Size of UI info
}	PianoRollUIState;


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef class

class CDirectMusicPartRef
{
friend class CDirectMusicPattern;
friend class CDirectMusicStyle;
friend class CDialogLinkExisting;

protected:
	void FreePianoRollStateList();

public:
	void SetPChannel(DWORD pchan);
	DWORD GetPChannel( void );
	CDirectMusicPartRef( CDirectMusicPattern* pPattern );
	~CDirectMusicPartRef();

	void SetPart( CDirectMusicPart* pPart );
	void SetName( LPCTSTR pszName );
	HRESULT LinkParts( CDirectMusicPartRef* pPartRef );

    HRESULT DM_LoadPartRef( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT DM_SavePartRef( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SavePartRefInfoList( IDMUSProdRIFFStream* pIRiffStream ) const;
    HRESULT DM_SavePartRefPianoRollStates( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT DM_SavePartRefDesignData( IDMUSProdRIFFStream* pIRiffStream );

public:
	CDirectMusicPattern*	m_pPattern;

protected:
	CDirectMusicPart* m_pDMPart;// the Part to which this refers
	CString m_strName;			// PartRef name 
	DWORD	m_dwPChannel;		// corresponds to port/device/midi channel
	BYTE	m_bVariationLockID; // parts with the same ID lock variations.
								// high bit is used to identify master Part
	BYTE	m_bSubChordLevel;	// tells which sub chord level this part wants
	BYTE	m_bPriority;		// Priority levels. Parts with lower priority
								// aren't played first when a device runs out of
								// notes
	BYTE	m_bRandomVariation;	// when set, matching variations play in random order
								// when clear, matching variations play sequentially
	BOOL	m_fHardLink;		// Set if this PartRef is 'Linked' to its part
	GUID	m_guidOldPartID;	// GUID for finding linked parts to change
	BOOL	m_fSetGUIDOldPartID;

	CTypedPtrList<CPtrList, PianoRollUIState*> m_lstPianoRollUIStates;
};


/////////////////////////////////////////////////////////////////////////////
// CIMA25EventItem class

class CIMA25EventItem : public AListItem
{
friend class CDirectMusicPattern;

public:
	virtual ~CIMA25EventItem(); 
	CIMA25EventItem* GetNext() { return (CIMA25EventItem*) AListItem::GetNext(); }

protected:
	CDirectMusicPartRef*		m_pPartRef;
	CDirectMusicEventItem*		m_pEvent;
};


/////////////////////////////////////////////////////////////////////////////
// CIMA25EventList class

class CIMA25EventList : public AList
{
public:
	virtual ~CIMA25EventList()	{	CIMA25EventItem* pEvent;	
									while( m_pHead != NULL )
									{
										pEvent = RemoveHead();
										delete pEvent;
									}
								}
    CIMA25EventItem *GetHead() const { return (CIMA25EventItem *)AList::GetHead();};
    CIMA25EventItem *RemoveHead() { return (CIMA25EventItem *)AList::RemoveHead();};
};


/////////////////////////////////////////////////////////////////////////////
// IMA25 Click structure

struct Click
{
	~Click() { }
	Click() { }
	int					m_nClick;			// click
	CIMA25EventList		m_lstIMA25Events;	// list of events (notes, curves, etc.)
};


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern class

class CDirectMusicPattern : public IDMUSProdNode, public IPersistStream, public IDMUSProdTimelineCallback,
							public IDMUSProdPropPageObject, public IDMUSProdTransport, public IDMUSProdNotifyCPt,
							public IDMUSProdConductorTempo, public IDMUSProdSecondaryTransport, public IPatternNodePrivate
{
friend class CDirectMusicStyle;
friend class CStylePatterns;
friend class CStyleMotifs;
friend class CStyleDlg;
friend class CDirectMusicPartRef;
friend class CPatternCtrl;
friend class CPatternDlg;
friend class CPatternLengthDlg;
friend class CTabPatternPattern;
friend class CTabMotifMotif;
friend class CTabMotifLoop;
friend class CDialogLinkExisting;
friend class CNewPatternDlg;

public:
    CDirectMusicPattern( CDirectMusicStyle* pStyle, BOOL fMotif );
	~CDirectMusicPattern();

	// Must call this method when the pattern is going to be deleted
	// It cleans up the references to the variation choices editor
	void	PreDeleteCleanup( void );

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnNbrFirstImage );
    HRESULT STDMETHODCALLTYPE GetFirstChild( IDMUSProdNode** ppIFirstChildNode );
    HRESULT STDMETHODCALLTYPE GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode );

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

	HRESULT STDMETHODCALLTYPE CreateDataObject( IDataObject** ppIDataObject );
	HRESULT STDMETHODCALLTYPE CanCut();
	HRESULT STDMETHODCALLTYPE CanCopy();
	HRESULT STDMETHODCALLTYPE CanDelete();
	HRESULT STDMETHODCALLTYPE CanDeleteChildNode( IDMUSProdNode* pIChildNode );
	HRESULT STDMETHODCALLTYPE CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE PasteFromData( IDataObject* pIDataObject );
	HRESULT STDMETHODCALLTYPE CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode, BOOL* pfWillSetReference );
	HRESULT STDMETHODCALLTYPE ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode );

	HRESULT STDMETHODCALLTYPE GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject ); 

    // IPersist functions
    STDMETHOD(GetClassID)( CLSID* pClsId );

    // IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

	// IDMUSProdTimelineCallback functions
	HRESULT STDMETHODCALLTYPE OnDataChanged( LPUNKNOWN punkStripMgr );

    // IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

	// IDMUSProdTransport methods
	STDMETHOD(Play)(BOOL fPlayFromStart);
	STDMETHOD(Stop)(BOOL fStopImmediate);
	STDMETHOD(Transition)();
	STDMETHOD(Record)( BOOL fPressed );
	STDMETHOD(GetName)( BSTR* pbstrName );
	STDMETHOD(TrackCursor)(BOOL fTrackCursor);

	// IDMUSProdSecondaryTransport methods
	STDMETHOD(GetSecondaryTransportSegment)( IUnknown **ppunkSegment );
	STDMETHOD(GetSecondaryTransportName)( BSTR* pbstrName );
	
	// IDMUSProdConductorTempo methods
	STDMETHOD(SetTempo)( double dblTempo );
	STDMETHOD(GetTempo)( double *pdblTempo );
	STDMETHOD(SetTempoModifier)( double dblModifier );
	STDMETHOD(GetTempoModifier)( double *pdblModifier );
	STDMETHOD(TempoModified)( BOOL fModified );

	//IDMUSProdNotifyCPt method
	HRESULT STDMETHODCALLTYPE OnNotify( ConductorNotifyEvent *pConductorNotifyEvent );

	// IPatternNodePrivate method
	HRESULT STDMETHODCALLTYPE DisplayPartLinkDialog( GUID *pGuid, IStream **ppPartStream );
	HRESULT STDMETHODCALLTYPE DisplayVariationChoicesDlg( REFGUID guidPart, DWORD dwPChannel );
	HRESULT STDMETHODCALLTYPE SetAuditionVariations( DWORD dwVariations, REFGUID rguidPart, DWORD dwPChannel );
	HRESULT STDMETHODCALLTYPE GetNumHardLinkRefs( REFGUID guidPart, DWORD* pdwReferences );
	HRESULT STDMETHODCALLTYPE CanShowPartLinkDialog();

	//Additional functions
	CDirectMusicEventItem* MakeDirectMusicEventItem(FullSeqEvent* pEvent);	// make music event from midi event
	void ActivateDialog( BOOL fActivate );
	void SyncTimelineSettings( void );

protected:
	HRESULT Initialize2 ( void );
	HRESULT Initialize1( void );
	void RecalcLength();
	void StartStopMusic( BOOL fStart, BOOL fInTransition = FALSE );
	MUSIC_TIME CalcLength() const { return m_dwLength; };

	CDirectMusicPart* FindPart( DWORD dwChannelID ) const;

	/*
	unsigned char MusicValueToNote( unsigned short value, char scalevalue, long keypattern,
									char keyroot, long chordpattern, char chordroot, char count );             // Total notes in chord.
	WORD NoteToMusicValue( BYTE bMIDINote, BYTE bPlayModes,  const DMUS_SUBCHORD &dmSubChord );
	unsigned short OldNoteToMusicValue(unsigned char note, long keypattern, char keyroot, long chordpattern, char chordroot);
	unsigned char OldMusicValueToNote(unsigned short value, char scalevalue, 
															long keypattern,  char keyroot,
															long chordpattern, char chordroot,
															char count);
	*/

	CDirectMusicPartRef* AllocPartRef();
	void DeletePartRef( CDirectMusicPartRef* pPartRef );
	CDirectMusicPartRef* FindPartRefByPChannel( DWORD dwPChannel ) const;
	CDirectMusicPartRef* FindPartRefByPChannelAndGUID( DWORD dwPChannel, REFGUID guidPart ) const;

	HRESULT GetPatternFromMIDIStripMgr();
	HRESULT OpenVarChoicesEditor( CDirectMusicPartRef* pPartRef );
	HRESULT InitializeVarChoicesEditor( CDirectMusicPartRef* pPartRef );
	HRESULT InsertBandTrack( IDirectMusicSegment *pSegment, BOOL fNeedBandTrack );
	HRESULT InsertChordTrack( IDirectMusicSegment *pSegment );
	HRESULT UpdateChordTrack( IDirectMusicSegment *pSegment );
	HRESULT ChangeChord( DMUS_CHORD_PARAM* pChordData, IDirectMusicSegment *pSegment );
	HRESULT CreateSegment( BOOL fNeedBandTrack );
	HRESULT PlayBand();
	IStream* GetChordList();
	HRESULT QueueTempoEvent();
	IDMUSProdNode* GetActiveBand();

    HRESULT DM_SaveSinglePattern( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty );
    HRESULT DM_SavePattern( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty );
    HRESULT DM_SavePatternChunk( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SavePatternUIChunk( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SavePatternInfoList( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SavePatternRhythm( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_SaveMotifSettingsChunk( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_LoadPattern( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, bool fSinglePattern );
    HRESULT DM_SavePatternEditInfo( IDMUSProdRIFFStream* pIRiffStream );

    HRESULT IMA25_LoadPattern( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT IMA25_LoadEvents( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT IMA25_LoadNoteList( IStream* pIStream, MMCKINFO* pckMain, short nClickTime );
	HRESULT IMA25_LoadCurveList( IStream* pIStream, MMCKINFO* pckMain, short nClickTime );

public:
	HRESULT Initialize();
	void SetModified( BOOL fModified );
	HRESULT SyncPatternWithDirectMusic();
	HRESULT SyncPatternWithMidiStripMgr();
	HRESULT SyncPatternWithAuditionSegment( BOOL fFirstTime, IDirectMusicSegment *pSegment );
	HRESULT RegisterWithTransport();
	HRESULT UnRegisterWithTransport();
	void PreChangePartRef( CDirectMusicPartRef* pDMPartRef );
	void UpdateLinkedParts( CDirectMusicPartRef* pPartRef, LPCTSTR pcstrText );
	void UpdateLinkFlags( void );
	void UpdateMIDIMgrsAndChangePartPtrs( CDirectMusicPart* pOldPart, CDirectMusicPart* pNewPart, LPCTSTR pcstrText );

	// Methods to set Pattern data
	void SetNbrMeasures( WORD wNbrMeasures );
	void SetEmbellishment( WORD wOnEmbellishment, WORD wOffEmbellishment, WORD wCustomId );
	void SetGrooveRange( BYTE bGrooveBottom, BYTE bGrooveTop );
	void SetDestGrooveRange( BYTE bDestGrooveBottom, BYTE bDestGrooveTop );
	void SetRhythmMap( DWORD* pRythmMap );
	void SetTimeSignature( DirectMusicTimeSig timeSig, BOOL fSyncWithEngine );
	void SetMotifResolution( DWORD dwResolution );
	void SetMotifRepeats( DWORD dwRepeats );
	BOOL SetMotifPlayStart( MUSIC_TIME mtPlayStart );
	BOOL SetMotifLoopStart( MUSIC_TIME mtLoopStart );
	BOOL SetMotifLoopEnd( MUSIC_TIME mtLoopEnd );
	void ResetMotifLoopPoints();
	void MusicTimeToBarBeatGridTick( MUSIC_TIME mtTime, long* plBar, long* plBeat, long* plGrid, long* plTick );
	void BarBeatGridTickToMusicTime( long lBar, long lBeat, long lGrid, long lTick, MUSIC_TIME* pmtTime );
	HRESULT CopyToForLinkAllParts( CDirectMusicPattern* pPattern );
	void SendChangeNotification();
	void SetFlags( DWORD dwFlags );

	// Methods to paste Bands
	HRESULT LoadCF_BANDTRACK( IStream* pIStream );
	HRESULT PasteCF_BAND( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );
	HRESULT PasteCF_BANDLIST( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );
	HRESULT PasteCF_BANDTRACK( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );
	HRESULT PasteCF_TIMELINE( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject );

	// Methods to sync Timeline data
	void SyncTempo();

private:
    CJazzUndoMan*      m_pUndoMgr;
    DWORD			   m_dwRef;
	BOOL			   m_fModified;
	BOOL			   m_fSendChangeNotification;
	BOOL			   m_fRecordPressed;
	BOOL			   m_fDialogActive;
	BOOL			   m_fInUndo;
	BOOL			   m_fInRedo;
	BOOL			   m_fDontUpdateDirectMusicDuringLoad;
	BOOL			   m_fPatternIsPlaying;
	BOOL			   m_fInTransition;
	BOOL			   m_fInGroupEdit;
	REFERENCE_TIME	   m_rtCurrentStartTime;
	BOOL			   m_fInOpenVarChoicesEditor;
	int				   m_nLastCustomId;


	CDirectMusicStyle* m_pStyle;

	IDMUSProdNode*	   m_pIDocRootNode;
	IDMUSProdNode*	   m_pIParentNode;
	HWND			   m_hWndEditor;

	DWORD						m_dwPlaybackID;
	IDirectMusicSegment*		m_pIDMSegment;
	IDirectMusicSegmentState*	m_pIDMSegmentState;
	IDirectMusicSegmentState*	m_rpIDMStoppedSegmentState;
	IDirectMusicSegment*		m_pIDMTransitionSegment;
	IDirectMusicSegmentState*	m_pIDMTransitionSegmentState;

public:
	CPatternCtrl*	   m_pPatternCtrl;		

// Pattern data
private:
    DWORD				m_dwDefaultKeyPattern;	// Default KeyPattern (Key/Chord for composing)
    DWORD				m_dwDefaultChordPattern;// Default ChordPattern (Key/Chord for composing)
    BYTE				m_bDefaultKeyRoot;		// Default KeyRoot (Key/Chord for composing)
    BYTE				m_bDefaultChordRoot;	// Default ChordRoot (Key/Chord for composing)
	BOOL				m_fChordFlatsNotSharps;	// Enharmonics for chord
	BOOL				m_fKeyFlatsNotSharps;	// Enharmonics for key

	DirectMusicTimeSig	m_TimeSignature;		// Patterns can override the Style's Time sig.
	WORD				m_wNbrMeasures;			// length in measures
	DWORD				m_dwLength;				// length in clocks
	WORD				m_wEmbellishment;		// Fill, Break, Intro, End, Normal, Motif
	BYTE				m_bGrooveBottom;		// bottom of groove range
	BYTE				m_bGrooveTop;			// top of groove range
	BYTE				m_bDestGrooveBottom;	// bottom of destination groove range
	BYTE				m_bDestGrooveTop;		// top of destination groove range
    DWORD               m_dwFlags;				// various flags (DMUS_PATTERNF_*)
	DWORD*				m_pRhythmMap;			// variable array of rhythms for chord matching
	CTypedPtrList<CPtrList, CDirectMusicPartRef*> m_lstPartRefs;	// list of part references

	void*				m_pPatternDesignData;		// UI info for Pattern
	DWORD				m_dwPatternDesignDataSize;	// Size of UI info

	BOOL				m_fTrackTimeCursor;

	// Timeline design data
	double				m_dblZoom;
	double				m_dblHorizontalScroll;
	long				m_lVerticalScroll;
	DMUSPROD_TIMELINE_SNAP_TO m_tlSnapTo;
	long				m_lFunctionbarWidth;
	bool				m_fIgnoreTimelineSync;

	// Motif specific data
	IDMUSProdNode*		m_pIBandNode;			// Motif's band.
    DWORD				m_dwRepeats;			// Number of repeats. By default, 0. 
    MUSIC_TIME			m_mtPlayStart;			// Start of playback. By default, 0. 
    MUSIC_TIME			m_mtLoopStart;			// Start of looping portion. By default, 0. 
    MUSIC_TIME			m_mtLoopEnd;			// End of loop. Must be greater than dwPlayStart.
												// By default, equal to length of motif. 
    DWORD				m_dwResolution;			// Default resolution. 
public:
    CString			    m_strName;				// Pattern name
};

#endif // __PATTERN_H__
