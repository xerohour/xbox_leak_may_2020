#ifndef __PERSONALITY_H__
#define __PERSONALITY_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Personality.h : header file
//

#include "PersonalityComponent.h"
#include "PersonalityDocType.h"
#include "PersonalityRef.h"

#include "RiffStrm.h"
#include "Chord.h"

#include "dmusici.h"
#include "dmusicf.h"

#include "DllBasePropPageManager.h"
#include "TabPersonality.h"
#include "TabAudition.h"
#include "TabChordPalette.h"

#include "Timeline.h"
#include "DLLJazzDataObject.h"

#include "JazzUndoMan.h"

#include "TabPersonalityInfo.h"

#include "SignPostDialog.h"

//#include "chorddialog.h"

#define MAKE_TIMESIG( bpm, beat ) ( (long)( ((DWORD)(WORD)beat) | ( ((DWORD)(WORD)bpm) << 16 ) ) )
#define TIMESIG_BEAT( tsig ) ( (short)tsig )
#define TIMESIG_BPM( tsig ) ( (WORD)( ((DWORD)tsig) >> 16 ) )


class CChordDialog;
class CTabAudition;
class CTabPersonality;
class CTabChordPalette;
class CTabPersonalityInfo;

class CSignPostDialog;

#define PERH_INVALID_BITS	0x11110000


//////////////////////////////////////////////////////////////////////
//  CPersonalityPageManager

class CPersonalityPageManager : public CDllBasePropPageManager 
{
	friend class CTabPersonalityInfo;
	friend class CTabPersonality;
public:
	CPersonalityPageManager( IDMUSProdFramework* pIFramework, CPersonality* pPersonality );
	virtual ~CPersonalityPageManager();


    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
	//HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Additional methods
	void UpdateObjectWithTabData();

	// Member variables
private:
	IDMUSProdFramework*	m_pIFramework;
	CTabPersonality*	m_pTabPersonality;
	CTabAudition*		m_pTabAudition;
	CTabChordPalette*	m_pTabChordPalette;
	CTabPersonalityInfo* m_pTabPersonalityInfo;
	CPersonality*		m_pPersonality;
public:
	CPersonality*		GetPersonality()
	{
		return m_pPersonality;
	}
	static short		sm_nActiveTab;
};


//////////////////////////////////////////////////////////////////////
//  CPersonality
class CPersonality : public IDMUSProdNode, public IDMUSProdPropPageObject, public IPersistStream, 
					 public IDMUSProdTimelineCallback, public IDMUSProdTransport, public IDMUSProdNotifyCPt,
					 public IDMUSProdNotifySink, public IDMUSProdChordMapInfo
{
//friend class CPersonalityCtrl;
//friend class CPersonalityDlg;
friend class CPersonalityRef;
friend class CTabPersonalityInfo;

//	friend class CSignPostDialog;
public:
    CPersonality( CPersonalityComponent* pComponent );
	~CPersonality();
	HRESULT Initialize();

	static bool GetFlatsFlag(DWORD key);
	static void SetFlatsFlag(bool useflats, DWORD& key);
	static void RootToString(char* string, DWORD key);

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdNode functions
	HRESULT STDMETHODCALLTYPE GetNodeImageIndex( short* pnFirstImage );
    HRESULT STDMETHODCALLTYPE GetFirstChild( IDMUSProdNode **ppIFirstChildNode );
    HRESULT STDMETHODCALLTYPE GetNextChild( IDMUSProdNode *pIChildNode, IDMUSProdNode **ppINextChildNode );

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

	
	// IDMUSProdPropPageObject functions
    HRESULT STDMETHODCALLTYPE GetData( void** ppData );
    HRESULT STDMETHODCALLTYPE SetData( void* pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties();
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager();

    // IPersist functions
    STDMETHOD(GetClassID)( CLSID* pClsId );

    // IPersistStream functions
    STDMETHOD(IsDirty)();
    STDMETHOD(Load)( IStream* pIStream );
    STDMETHOD(Save)( IStream* pIStream, BOOL fClearDirty );
    STDMETHOD(GetSizeMax)( ULARGE_INTEGER FAR* pcbSize );

	// IDMUSProdTimelineCallback method
	HRESULT STDMETHODCALLTYPE OnDataChanged(IUnknown *);

	// IDMUSProdTransport methods
	STDMETHOD(Play)(BOOL /*fPlayFromStart*/);
	STDMETHOD(Stop)(BOOL fStopImmediate);
	STDMETHOD(Transition)();
	STDMETHOD(Record)(BOOL fPressed);
	STDMETHOD(GetName)(BSTR* pbstrName);
	STDMETHOD(TrackCursor)(BOOL fTrackCursor);

	// IAANotifyCPt methods
	STDMETHOD(OnNotify)( ConductorNotifyEvent* pEvent);

	// IDMUSProdNotifySink
	HRESULT STDMETHODCALLTYPE OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData);

	// IDMUSProdChordMapInfo
    HRESULT STDMETHODCALLTYPE GetChordMapInfo(DMUSProdChordMapInfo* pChordMapInfo);
    HRESULT STDMETHODCALLTYPE GetChordMapUIInfo(DMUSProdChordMapUIInfo* pChordMapUIInfo);
    HRESULT STDMETHODCALLTYPE SetChordMapUIInfo(DMUSProdChordMapUIInfo* pChordMapUIInfo);

	// CPersonality native methods
	void	ChangeChordSignPostTimeSig(); // Changes the TimeSigs of all Chords and SignPosts
	void	ChangeChordMapLength();		  // Changes the length of the chord map
	void	ChangeChordMapEditMode();	  // Changes chordmap to fixed or variable mode
	void	StopIfPlaying( DWORD dwFlags );
	HRESULT UnAdviseConnectionPoint();
	HRESULT AdviseConnectionPoint();
	BOOL UpdateSelectedChordList();
	Personality*	GetPersonality() { return m_pPersonality; }
	// Additional functions
protected:

    // JazzNotify helpers
	HRESULT SetStyleReference(IDMUSProdNode* pDocRoot);
	IDMUSProdNode* GetStyleDocRoot();

	// section construction methods
	HRESULT ComposeSection( IDirectMusicSegment** ppISection);
	HRESULT SaveJazzStyleReference( LPSTREAM);
	HRESULT LoadJazzStyleReference( LPSTREAM);
	HRESULT SaveChordList( LPSTREAM pStream );
	HRESULT SaveChordList( IDMUSProdRIFFStream* pRIFF );
	HRESULT SaveCommandList( IDMUSProdRIFFStream* pRIFF );
	HRESULT SaveCommandList( IStream* pIStream );
	HRESULT GetDirectMusicStyle(IDirectMusicStyle** ppIStyle);

	// some chord map deletion helpers
	void DeleteAllConnectionsToChord( int nID );
	void TrimChordMap();

	// drag / drop helpers
	HRESULT PasteCF_STYLE( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject,
							  IDMUSProdNode* pIPositionNode );


public:
	DWORD GetSignPostListPtr();
	BOOL &Modified();

	void  SetChordChanges(DMPolyChord& pc);

public:
    HRESULT LoadPersonality( IStream* pStream, DWORD dwSize, DWORD id = ID_PERSONALITY);
    HRESULT LoadPersonality( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );

	enum { syncPersonality=0x1, syncAudition = 0x2, syncStopMusic=0x4};	//why SyncPersonalityWithEngine is being called, or'd
	enum { UseFlats = 0x10000000};
	HRESULT SyncPersonalityWithEngine(int why);

	HRESULT SendChordMapAndPaletteToChordMapMgr();
	HRESULT SaveChordList( IStream* pIStream, BOOL bReleaseSave );

	BOOL	VariableNotFixed() { return m_fVariableNotFixed; }
	void ClearChordList();

	short GetBeat() { return m_nBeat; }
	short GetBPM() { return m_nBPM; }
	short GetChordMapLength() { return m_nChordMapLength; }
public:
	HRESULT DMSaveChordData(IDMUSProdRIFFStream* pIRiffStream, bool bIncludeDesignInfo);
	HRESULT DMSaveUI(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT DMSaveVersion(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT DMSaveInfoList(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT DMSavePersonality(IDMUSProdRIFFStream* pIRiffStream, FileType ftFileType);
	HRESULT DMSave(IStream* pIStream, BOOL fClearDirty);

	HRESULT DMLoadVersionChunk(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent);
	HRESULT DMLoadInfoChunk(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent);
	HRESULT DMLoadUI(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent);

	DWORD GetKey()
	{
		return m_dwKey;
	}

	DWORD GetAbsKey()	// no flats flag
	{
		return m_dwKey & ~UseFlats;
	}

	void SetKey(DWORD key)
	{
			m_dwKey = key;
	}

	// returns palette chord corresponding to m_dwKey
	const ChordSelection&	GetKeyChord() const
	{
		return m_pPersonality->m_chordpalette.Chord(m_dwKey & ~UseFlats);
	}

	CPersonalityComponent	*m_pComponent;

	// Personality's Edit Strip Managers
	LPUNKNOWN		m_punkChordMapMgr;

	// Dialog Sizing Functions
	WORD	GetSplitterPos()
	{
		return m_wSplitterPos;
	}
	void SetSplitterPos(WORD pos, bool bMarkAsDirty=true)
	{
		m_wSplitterPos = pos;
		if(bMarkAsDirty)
			m_fModified = TRUE;
	}

	double& ZoomFactor();
	LONG& FunctionbarWidth();

	// Personality specific data.
protected:
	Personality*	m_pPersonality;		// Personality
	BOOL			m_fModified;
	
	int				m_paletteChordType;

private:
    DWORD			m_dwRef;
	IDMUSProdNode*		m_pIDocRootNode;
	IDMUSProdNode*		m_pIParentNode;
	IDirectMusicChordMap*	m_pIDirectMusicChordMap;
	HWND			m_hWndEditor;
	// used by Advise/UnAdviseConnection
	DWORD			m_dwCookie;
	BOOL			m_bAdvised;
	BOOL			m_fVariableNotFixed;
	WORD			m_wSplitterPos;
	double			m_dblZoom;
	LONG			m_lFunctionbarWidth;

	// used to determine what objects are selected when loading or saving chordmap chords.
	int					m_selectiontype;		// types #define'd in chord.h
	short					m_nidToChord;
	short					m_nidFromChord;
	int					m_idxPaletteChord;
	void				SetSelectionType();		// from chord map to internal state
//	void				GetSelectionType();		// from loaded chords to internal state

// Personality Reference data for Music Engine
//    CString			m_strName;
//	CString			m_strFileName;
    GUID			m_guidPersonality;
    BYTE			m_fDefault;           // 1=Default personality
	IDirectMusicSegment*		m_pISection;		  // section being auditioned
	IDirectMusicSegmentState	*m_pIDMSegmentState;
    ChordExt	*m_paChordChanges;		// List of Chord changes.
	CommandExt	*m_lstCommandList;		// Command list.
	BOOL			m_bSectionStopping;
// Additional fields added to store Personality properties
	BOOL			m_fUseTimeSignature;	// for edit mode
	short			m_nBPM;					// for edit mode
	short			m_nBeat;				// for edit mode
	short			m_nChordMapLength;		// for edit mode
	DWORD			m_dwKey;				// Loword is key, high bit is sharp, flat flag
	IDMUSProdNode*	m_pIStyleRefNode;		// for audition
	DWORD			m_dwGroove;				// for audition

	// personality info list
	CString		m_strName;
	CString		m_strCategoryName;
	CString		m_strAuthor;
	CString		m_strCopyright;
	CString		m_strSubject;
	CString		m_strInfo;


	// personality version
	DWORD	m_dwVersionLS;
	DWORD	m_dwVersionMS;

	// undo
	void	CreateUndoMgr();
public:
	void OnNameChange( void );
	CString		m_strOrigFileName;
	void SetStyleDocRootNode();
	bool CheckForData(long lBeginAtMeasure);
	ChordEntryList* GetChordList();
	void ComputeChordMap();
	// functions for matching signpost chords on grid to those in chordmap
	void ClearSPLinks();
	void ClearCMLinks();
	bool Match(ChordEntry* ce, SignPost* sp);
	bool Match(SignPost* sp1, SignPost* sp2);
	void Link(ChordEntry* ce, SignPost* sp)
	{
		ce->m_dwflags |= CE_MATCHED;
		sp->m_dwflags |= SPOST_MATCHED;
	}
	void Link(SignPost* sp1, SignPost* sp2)
	{
		sp1->m_dwflags |= SPOST_MATCHED;
		sp2->m_dwflags |= SPOST_MATCHED;
	}
	void LinkCMToSP();
	bool Link2SP(ChordEntry* ce, bool bRefreshChordMap);
	bool Link2SP(SignPost* sp);
	bool Link2CM(SignPost* sp);
	void LinkAll(bool bIsStructuralChange);


	int& PaletteChordType();
	void UpdateChordPalette(bool fUpdateChords);
	void RefreshSignPosts(bool bRefresh);
	void RefreshChordMap();
	void TransposeAllChords(DWORD dwNewKey);
	HRESULT InitializeStyleReference();
	HRESULT DMLoadStructure(IDMUSProdRIFFStream* pIRiffStream);
	HRESULT DMSaveStructure(IDMUSProdRIFFStream* pIRiffStream);
    HRESULT GetObjectDescriptor( void* pObjectDesc );
	void CleanupObjects();
	bool GetLastEdit(CString& str);
	CJazzUndoMan*		m_pUndoMgr;
	void RefreshProperties();
	void GetGUID( GUID* pguidPersonality );
	void SetGUID( GUID guidPersonality );
	
	CSignPostDialog*	m_pSignPostDlg;
	CChordDialog*		m_pChordDlg;

	bool				m_fLockAllScales;
	bool				m_fSyncLevelOneToAll;
};

#endif // __PERSONALITY_H__
