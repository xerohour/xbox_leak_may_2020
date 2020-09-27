// ChordMgr.h : Declaration of the CChordMgr

#ifndef __CHORDMGR_H_
#define __CHORDMGR_H_

#include "resource.h"       // main symbols
#include "timeline.h"
#include "..\includes\DMUSProd.h"
#include "PropPageMgr.h"
#include "ChordIO.h"
#include "DLLJazzDataObject.h"
#include "MusicTimeConverter.h"
#include "..\shared\SelectedRegion.h"

////////////////////////////////////////////////////////////////////////////////
// Chord Track design-time RIFF chunk header
#define DMUS_FOURCC_CHORD_DESIGN_CHUNK     mmioFOURCC('c','t','d','c')

////////////////////////////////////////////////////////////////////////////////
// Chord Track design-time structure

#pragma pack(2)

struct ioDMChordDesignChunk
{
	ioDMChordDesignChunk()
	{
		m_bDisplayingFlats = 0;
		m_bKeyRoot = 0;
		m_bNumAccidentals = 0;
		m_bPad = 0;
		m_fLockAllScales = true;
		m_fSyncLevelOneToAll = true;
	}

	BYTE	m_bDisplayingFlats;
	BYTE	m_bKeyRoot;
	BYTE	m_bNumAccidentals;
	BYTE	m_bPad;
	bool	m_fLockAllScales;
	bool	m_fSyncLevelOneToAll;
};

#pragma pack()

#define PPQN            192				// IMA pulses per quarter note
#define PPQNx4          ( PPQN << 2 )
#define PPQN_2          ( PPQN >> 1 )

#define DM_PPQN         768				// Direct Music pulses per quarter note
#define DM_PPQNx4       ( DM_PPQN << 2 )
#define DM_PPQN_2       ( DM_PPQN >> 1 )

#define SEL_NONE		0
#define SEL_ONE			1
#define SEL_MULTIPLE	2

#define HIDE_NONE_SELECTED	0
#define HIDE_ALL_ON			1
#define HIDE_ALL_OFF		2
#define HIDE_MIXED			3

#define TRACKCONFIG_VALID_MASK ( DMUS_TRACKCONFIG_NOTIFICATION_ENABLED | DMUS_TRACKCONFIG_CONTROL_NOTIFICATION | DMUS_TRACKCONFIG_CONTROL_ENABLED | DMUS_TRACKCONFIG_TRANS1_FROMSEGSTART | DMUS_TRACKCONFIG_TRANS1_FROMSEGCURRENT | DMUS_TRACKCONFIG_TRANS1_TOSEGSTART)


interface IDirectMusicTrack;
/////////////////////////////////////////////////////////////////////////////
// CChordMgr
class ATL_NO_VTABLE CChordMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CChordMgr, &CLSID_ChordMgr>,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend class CChordStrip;
public:
	CChordMgr();
	~CChordMgr();

	enum {DefaultScale = 0xAB5AB5, DefaultKey = 12};
protected:
	IDMUSProdTimeline*		m_pTimeline;
	CChordList				m_ChordList;
	CChordStrip*			m_pChordStrip;
	CPropChord				m_SelectedChord;
	IDMUSProdFramework*		m_pIDMUSProdFramework;
	CPropertyPage*			m_pPropertyPage;
	IDMUSProdPropPageManager*	m_pPropPageMgr;
	IDataObject*		m_pCopyDataObject;
	DWORD					m_dwGroupBits;
	DWORD					m_dwOldGroupBits;
	DWORD					m_dwTrackExtrasFlags;
	DWORD					m_dwProducerOnlyFlags;
	IDirectMusicTrack*		m_pIDMTrack;
	BOOL					m_fUseChordStripPPG;
	BOOL					m_fNeedChordMeasure1Beat1;
	DWORD					m_dwPosition;	// of track in segment
	DWORD					m_dwScale;
	BYTE					m_bKey;
	BYTE					m_bPatternEditorMode;

	BOOL					m_fDisplayingFlats;
	int						m_nKeyRoot;
	int						m_nNumAccidentals;

	bool					m_fLockAllScales;
	bool					m_fSyncLevelOneToAll;
public:
DECLARE_REGISTRY_RESOURCEID(IDR_CHORDMGR)

BEGIN_COM_MAP(CChordMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()


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

// IDMUSProdStripMgr
public:
	bool UseFlatsNotSharps(BYTE bKey, DWORD dwScale);
	HRESULT SaveSelectedChords(IStream* pStream, long offset);
	void SelectAll();
	HRESULT GetSpanOfChordList(long& lStartTime, long& lEndTime, CChordList* pList = NULL, bool bSelectedChordsOnly = false);
	HRESULT GetBoundariesOfSelectedChords(long& lStartTime, long& lEndTime);
	BOOL SelectSegment(long begintime, long endtime);

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

protected:
//	HRESULT LoadChordList( LPSTREAM pStream );
//	HRESULT SaveChordList( LPSTREAM );
	HRESULT	DeleteSelectedChords();
	// pChordAtDragPoint == NULL means ignore
	HRESULT SaveSelectedChords(LPSTREAM, CChordItem* pChordAtDragPoint, BOOL bNormalize = FALSE,
								long* pNormalOffset=0);
	HRESULT GetTimeSig(long* top, long* bottom, long mtTime);
// general helpers
	HRESULT NormalizeChordList(CChordList& list, long clockOffset, long* pStartClocks=0, long* pEndClocks=0);

	// marks m_dwUndermined field CChordItems in list
	void MarkSelectedChords(DWORD flags);
	// unmarks flag m_dwUndermined field CChordItems in list
	void UnMarkChords(DWORD flags);
	// deletes chords marked by given flag
	void DeleteMarkedChords(DWORD flags);
	void UnselectChords();
	HRESULT UpdateDirectMusic( BOOL fUpdateSegmentDesigner );
	HRESULT ImportChordsFromChordTrack( void );
	HRESULT ImportKeyFromChordTrack( void );
	
// Misc
	bool	IsSelected();				// returns if one or more chords are selected.
	short	HowManySelected();			// returns SEL_NONE, SEL_ONE, SEL_MULTIPLE
	short	GetSelectedHideState();		// returns HIDE_ALL_ON, HIDE_ALL_OFF, HIDE_MIXED
	CChordItem* FirstSelectedChord();
	void RecalculateMusicTimeValue(CChordItem* pChord);
	bool IsChordAt(long measure, long beat);
	long ShiftChordsBackwards(long measure, long bpm);
	void	RecalculateMusicTimeValues();
	void	UpdateMasterScaleAndKey(); // Updates m_dwScale and m_bKey based on the key root and # of sharps/flats
	void	UpdateKeyRootAndAccidentals(); // Updates m_nNumAccidentals and m_nKeyRoot from m_dwScale and m_bKey
};


class CChordStrip : public IDMUSProdStrip, public IDMUSProdStripFunctionBar, public IDMUSProdTimelineEdit,
	public IDropSource, public IDropTarget,
	public IDMUSProdPropPageObject
{
friend class CChordMgr;
protected:
	long	m_cRef;
	long	m_lBeginSelect;
	long	m_lEndSelect;
	bool	m_bMouseSelect;
	CListSelectedRegion*	m_pSelectedRegions;
	CChordMgr*			m_pChordMgr;
	IDMUSProdStripMgr*  m_pStripMgr;
	BOOL	m_bGutterSelected;	// whether the gutter select is selected
	BOOL	m_bSelecting;
public:
	CChordStrip( CChordMgr* pChordMgr )
	{
		ASSERT( pChordMgr );
		m_bLeftMouseButtonDown =false;
		m_pSelectedRegions = 0;
		m_pChordMgr = pChordMgr;
		m_pStripMgr = (IDMUSProdStripMgr*)pChordMgr;
		//m_pStripMgr->AddRef();
		m_cRef = 0;
		AddRef();
		m_bGutterSelected = FALSE;
		m_bSelecting = FALSE;
		m_bMouseSelect = false;
		m_lBeginSelect = 0;
		m_lEndSelect = 0;
		m_pISourceDataObject = NULL;
		m_pITargetDataObject = NULL;
		m_dwStartDragButton = 0;
		m_dwOverDragButton = 0;
		m_dwOverDragEffect = 0;
		m_dwDragRMenuEffect = DROPEFFECT_NONE;
		m_nLastEdit = 0;
		m_fShowChordProps = FALSE;
		m_lXPos = -1;
		m_startDragPosition = 0;
		m_fPropPageActive = FALSE;
		m_pPropPageMgr = NULL;
		m_nStripIsDragDropSource = 0;
		m_cfChordList = 0;
	};

	~CChordStrip()
	{
		//m_pStripMgr->Release();
		if ( m_pPropPageMgr )
		{
			m_pPropPageMgr->Release();
		}

		if(m_pSelectedRegions)
		{
			delete m_pSelectedRegions;
		}
	}

private:
	HRESULT StartDragDrop(IDMUSProdTimeline* pTimeline, LONG lXPos);
	void SelectChordsInSelectedRegions();
	void SelectRegionsFromSelectedChords();
	void FormatNameForDrawRoutine( CString& strName, CChordItem* pChord );
	HRESULT NormalizeSelectedRegions(CListSelectedRegion& list, long clockoffset, 
									 long* pStartClocks=0, long* pEndClocks=0);
	HRESULT SaveSelectedRegions(IStream* pIStream, long offset, bool bNormalize);
	HRESULT PostRightClickMenu(IDMUSProdTimeline* pTimeline, POINT pt);
	HRESULT DoRightClickMenu(IDMUSProdTimeline* pTimeline, POINT pt);
	HRESULT ShowPropertySheet(IDMUSProdTimeline*);
	void UnselectGutterRange( void );
	BOOL ProcessKey(int nVirtKey, unsigned long lKeyData);
	CChordItem *		GetChordFromPoint( long lPos );
	LONG			m_lXPos;		// used for temp storage of xpos when doing mouse edits

	IDataObject*	m_pISourceDataObject;			// Object being dragged	
	IDataObject*	m_pITargetDataObject;
	bool			m_bLeftMouseButtonDown;
	DWORD			m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD			m_dwOverDragButton;		// Mouse button stored in IDropTarget::DragOver
	DWORD			m_dwOverDragEffect;		// Drag effects stored in IDropTarget::DragOver
	DWORD			m_dwDragRMenuEffect;	// Result from drag context menu
	UINT			m_cfChordList;
	LONG			m_startDragPosition;	// xpos where drag was started
	UINT			m_nLastEdit;			// resource id of last edit
	short			m_nStripIsDragDropSource;	

	BOOL			m_fShowChordProps;
	BOOL			m_fPropPageActive;
	IDMUSProdPropPageManager *m_pPropPageMgr;
// IUnknown
public:
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
// IDMUSProdStrip
public:
	HRESULT	STDMETHODCALLTYPE	Draw( HDC hDC, STRIPVIEW sv, LONG lXOffset );
    HRESULT STDMETHODCALLTYPE	GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar);
    HRESULT STDMETHODCALLTYPE	SetStripProperty( STRIPPROPERTY sp, VARIANT var);
	HRESULT STDMETHODCALLTYPE	OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdStripFunctionBar
	HRESULT STDMETHODCALLTYPE FBDraw( HDC hDC, STRIPVIEW sv );
	HRESULT STDMETHODCALLTYPE FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdTimelineEdit
	HRESULT STDMETHODCALLTYPE Cut( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Copy( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObject = NULL );
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
	HRESULT		CreateDataObject(IDataObject**, long position);

// IDropTarget methods
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

// IDropTarget helpers
	CWnd* GetTimelineCWnd();
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT PasteReplace(CChordList& list, CMusicTimeConverter& cmtStart, CMusicTimeConverter& cmtEnd, BOOL& fChanged);
	HRESULT PasteMerge(CChordList& list, CMusicTimeConverter& cmtStart, CMusicTimeConverter& cmtEnd, BOOL& fChanged);
	HRESULT PasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, POINTL pt, bool bDropNotEditPaste);

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

};


inline bool CListSelectedRegion_Contains(CListSelectedRegion& csrList, CChordItem& chord)
{
	return csrList.Contains(chord.Measure(), chord.Beat());
}

// add a region using a chord's position and duration (1 beat)
inline void CListSelectedRegion_AddRegion(CListSelectedRegion& csrList, CChordItem& chord)
{
	CMusicTimeConverter cmtBeg(chord.Measure(), chord.Beat(), csrList.Timeline(), csrList.GroupBits());
	CMusicTimeConverter cmtEnd = cmtBeg;
	cmtEnd.AddOffset(0,1, csrList.Timeline(), csrList.GroupBits());
	CSelectedRegion* psr = new CSelectedRegion(cmtBeg, cmtEnd);
	csrList.AddHead(psr);
}



#endif //__CHORDMGR_H_
