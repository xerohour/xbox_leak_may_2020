// ChordMapMgr.h : Declaration of the CChordMapMgr

#ifndef __ChordMapMGR_H_
#define __ChordMapMGR_H_
#include "stdafx.h"
#include "resource.h"       // main symbols
#include "timeline.h"
#include "dmusici.h"
#include "Chord.h"
#include "chordio.h"
#include "PropChord.h"
#include "PropPageMgr.h"
#include "ConnectionPropPageMgr.h"
#include "DLLJazzDataObject.h"
//#include <AFXCMN.H>

#define CHORDNAME_MAX 12
#define WM_INSERT_CHORD (WM_USER + 51)

#ifndef __CCHORDMAPSTRIPMGRAPP__
#define __CCHORDMAPSTRIPMGRAPP__
class CChordMapStripMgrApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordMapMgr
class ATL_NO_VTABLE CChordMapMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CChordMapMgr, &CLSID_ChordMapMgr>,
	public IChordMapMgr,
	public IDMUSProdStripMgr,
	public IPersistStream,
	public IDMUSProdPropPageObject
{
friend class CChordMapStrip;
public:
	CChordMapMgr();
	~CChordMapMgr();

protected:
	void ClearChordMapList(void);

protected:
	DWORD				m_dwKey;
	IDMUSProdTimeline*	m_pTimeline;
	ChordEntryList*		m_pChordMapList;
	ChordPalette*		m_pChordPalette;
	int					m_nIDCounter;
	int					m_nSelectedIndex;
	double				m_dblZoom;
	CChordMapStrip*		m_pChordMapStrip;

	IDataObject*		m_pCopyDataObject;

	// Supports Chord and Connection Property Page.
			// TO DO: SelectedObjectType parameter in GetTypeThis should be replaced by explicit calls to determine 
			// selection type
		// Note: until then DO NOT change order of enums in CChordMapMgr
	enum SelectionType{NoSelection, PaletteSelected, ChordSelected, ConnectionSelected, 
		MultipleSelections, EndEnum}	m_selection;

	// TBD: the numeric flag below needs to be set from some const global to project ( == CPersonality::UseFlats)
	enum {UseFlats = 0x10000000 };

	CPropChord					m_SelectedChord;
	NextChord*					m_pSelectedConnection;
	IDMUSProdFramework*			m_pJazzFramework;
	CPropertyPage*				m_pPropertyPage;
	IDMUSProdPropPageManager*	m_pChordPropPageMgr;
	IDMUSProdPropPageManager*	m_pConnectionPropPageMgr;
	DWORD						m_dwGroupBits;
	IDirectMusicTrack*			m_pIDMTrack;
	IDMUSProdNode*				m_pIChordMapNode; // DocRoot node of ChordMap

public:
DECLARE_REGISTRY_RESOURCEID(IDR_CHORDMAPMGR)

BEGIN_COM_MAP(CChordMapMgr)
	COM_INTERFACE_ENTRY_IID(IID_IChordMapMgr,IChordMapMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY(IDMUSProdPropPageObject)
END_COM_MAP()


BEGIN_MSG_MAP(CChordMapMgr)
END_MSG_MAP()


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

// IChordMapMgr
public:
	void GetBoundariesOfSelectedChords(long& lStartTime, long& lEndTime);
	void HitTest(RECT& rectSelect);
	void MakeCompatibleNids(ChordEntryList& list);
	HRESULT ShowPropPage(BOOL fShow);
	bool IsPropPageShowing();
	void SendEditNotification(UINT type);
	void GetLastEdit(CString& str);

	HRESULT STDMETHODCALLTYPE CreateChordMapStrip( 
		/* [out] */ IDMUSProdStrip **ppStrip);
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
	HRESULT STDMETHODCALLTYPE CreateBlankChord(
		/* [out] */ ChordEntry** ppChord );
	HRESULT STDMETHODCALLTYPE FindUniqueID(
		/* [out] */ int* pnID );
	HRESULT STDMETHODCALLTYPE PreparePropChord();
//	HRESULT STDMETHODCALLTYPE PrepareConnection();
	HRESULT STDMETHODCALLTYPE PropChordToChord(
		/* [in] */  ChordEntry *pChord,
		/* [out] */ CPropChord *pPropChord );
	HRESULT STDMETHODCALLTYPE ChordToPropChord(
		/* [in] */  CPropChord *pPropChord,
		/* [out] */ ChordEntry *pChord );

	HRESULT GetTimeSig(long* top, long* bottom);

protected:
	HRESULT LoadChordMapAndPalette( LPSTREAM pIStream /*, LPMMCKINFO pck, ChordMapExt** plstChordMap */);
	HRESULT SaveChordMapList( LPSTREAM );
	

private:
	void DetectAndReverseConnections(ChordEntryList& list);
	void SetUpNextChord(short nidFrom, short nidTo);
	bool LessThan(const ChordEntry&a, const ChordEntry&b);
	bool GreaterThan(const ChordEntry&a, const ChordEntry&b);
	long AbsDiff(const ChordEntry&a, const ChordEntry&b);
};

#include "ContextMenuHandler.h"
// String lengths
#define ROOT_TO_NAME_MAX		30

/*
#define STOP_SIGN_HEIGHT		27
#define	STOP_SIGN_WIDTH			14
#define GO_SIGN_HEIGHT			27

#define MOUSEDRAG_GRACE_X		10
#define MOUSEDRAG_GRACE_Y		10
*/

///*
#define STOP_SIGN_HEIGHT		16
#define	STOP_SIGN_WIDTH			12
#define GO_SIGN_HEIGHT			16

#define MOUSEDRAG_GRACE_X		5
#define MOUSEDRAG_GRACE_Y		5
//*/

#define	FUNCTION_NAME_HEIGHT		12

#define HORIZ_LINE_HEIGHT	1
#define PIANOROLL_BLACKKEY_COLOR	RGB( 0, 0, 0 )
#define PIANOROLL_WHITEKEY_COLOR	RGB( 255, 255, 232 )
#define PIANOROLL_SELKEY_COLOR		RGB( 255, 0, 0)


/*
#define CHORD_DEFAULT_HEIGHT 40
#define CHORD_DEFAULT_WIDTH 70  // For testing.  Real chords will use zoom factor into account for width.

#define CONNECTION_HEIGHT		15
#define CONNECTION_WIDTH		35
#define CONNECTION_SEPERATOR	15
*/

///*
#define CHORD_DEFAULT_HEIGHT 30
#define CHORD_DEFAULT_WIDTH 60  // For testing.  Real chords will use zoom factor into account for width.

#define CONNECTION_HEIGHT		6
#define CONNECTION_WIDTH		10
#define CONNECTION_SEPERATOR	6
//*/

#define CONNECTION_FILL_BACKGROUND		RGB( 204, 204, 255 )
#define CONNECTION_FILL					RGB( 192, 192, 192 )
#define CONNECTION_TEXT_COLOR			RGB( 0, 0, 0 )
#define CHORD_CONNECTION_3D_HIGHLIGHT	RGB( 255, 255, 255 )
#define CHORD_CONNECTION_3D_LOWLIGHT	RGB( 0, 0, 0 )
#define CHORD_CONNECTION_3D_FILL		RGB( 0, 64, 128 )
#define CHORD_CONNECTION_TEXT_COLOR		RGB( 255, 255, 0 )

#define CONNECTION_SEL_FILL				RGB(255,255,0)
#define CONNECTION_SEL_FILL_BACKGROUND	RGB(255,0,255)

#define CHORD_CONNECTION_3D_SEL_FILL		RGB( 255, 0, 0 )
#define CHORD_CONNECTION_3D_SEL_HIGHLIGHT	RGB( 255, 255, 0 )
#define CHORD_CONNECTION_3D_SEL_LOWLIGHT	RGB( 0, 0, 128 )


typedef void (*pfnCallback)(long hint);

struct DragState
{
	enum {off=0, left=1, right=2, up=4, down=8};	// use when scrolling outside window boundaries
	short xOrg;
	short yOrg;
	short xTo;
	short yTo;
	bool  bActive;
	bool  bAutoscroll;
	short dir;
	DragState()
	{
		xOrg = yOrg = xTo = yTo = 0;
		bActive = false;
		bAutoscroll = false;
		dir = off;
	}
};

class CChordMapStrip : public IDMUSProdStrip , 
					   public IDropTarget, 
					   public IDropSource, 
					   public IDMUSProdTimelineEdit,
					   public IDMUSProdStripFunctionBar
{
friend class CChordMapMgr;
protected:
	void ComputeSelectionBoxOutline(POINTS ptBeg, POINTS ptEnd, bool bRecomputeDragState = true);
	HRESULT RecomputeConnections( bool fChangingToVariableMode );
	HRESULT OnCommand(UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos, BOOL bRefresh, BOOL bSyncEngine, ChordEntry*&	pChordChange);
	HRESULT OnRButtonUp(UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos);
	HRESULT OnInsert(UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos,
								  BOOL bRefresh, BOOL bSyncEngine, ChordEntry*&	pChordChange);
	HRESULT MergeChords(ChordEntryList& list, POINTL pt, ChordEntry* pRefChord, bool bDropNotPaste);
	void GetBoundingRectRelativeToChord(ChordEntryList& list, ChordEntry* pRefChord, RECT& rect);
	void GetBoundingRect(ChordEntryList& list, RECT& rect);
	void GetBoundingRectOfSelectedChords(RECT& rect);
	long GetVerticalScroll();
	HRESULT CheckBoundsForDrag(POINTL pt, RECT* pExcess = 0);
	void	SetFocusToTimeline();
	HRESULT DoDragDrop(WPARAM wParam, LPARAM lParam, ChordEntry* pChord, ChordEntry*& pChordChange);
	bool ContainsSignpost(ChordEntryList& list);
	void PositionToMeasureBeat(long pos, long& measure, long& beat);
	void MeasureBeatToPosition(long measure, long beat, long& pos);
	void PositionToClocks(long pos, long& clocks);
	void ClocksToPosition(long clocks, long& pos);
	long ClocksPerMeasure();
	bool SelectedChordsContainsSignpost();
	long				m_cRef;
	CChordMapMgr*		m_pChordMapMgr;
	IDMUSProdStripMgr*	m_pStripMgr;
	BOOL				m_fSelected;
	BOOL				m_fDragging;
	bool				m_fCapture;
	bool				m_bDrop;
	BOOL				m_fConnectionDrag;
	RECT				m_rcScreenClip;
	BOOL				m_fVariableNotFixed;
	long				m_lFocusHint;
	pfnCallback			m_pfnFocusCallback;

	enum { ConnectionDrag = 1, SelectionDrag = 2 };
	DragState		m_SelectionDragState;
	DragState		m_ConnectionDragState;

	// bounding box select state vars
	RECT			m_rectSelect;
	RECT			m_rectBounding;
	POINTS			m_ptAnchor;
	bool	m_bInSelectionNet;

	// Kludge for messaging.
	CContextMenuHandler m_MenuHandler;

	// For rubber-band UI
	CBitmap *m_pbmpBackground;
	void GetWindowOrg(long& xpos, long& ypos);
	long	m_xOrg;
	long	m_yOrg;

	// strip size
	enum { enumMaxHeight = 2000, enumDefaultHeight = 800, enumFBWidth = 100 };

	long CChordMapStrip::GetStripHeight()
	{	
		long h = enumDefaultHeight;
		return h;
	}
	// drag drop state
	IDataObject*	m_pISourceDataObject;			// Object being dragged	
	IDataObject*	m_pITargetDataObject;
	RECT			m_rectAggregate;		// used to store boundaries of chord aggregate being dragged
											// relative to chord mouse is on top of
	DWORD			m_dwStartDragButton;	// Mouse button that initiated drag operation
	DWORD			m_dwOverDragButton;		// Mouse button stored in IDropTarget::DragOver
	DWORD			m_dwOverDragEffect;		// Drag effects stored in IDropTarget::DragOver
	CImageList*		m_pDragImage;			// Image used for drag operation feedback
	DWORD			m_dwDragRMenuEffect;	// Result from drag context menu
	UINT			m_cfChordList;
	UINT			m_cfChordMapList;
	POINTL			m_startDragPosition;	// pos where drag was started

	ChordEntry*		m_pDragChord;			// intra-chordmap chord being dragged
	POINTS			m_ptDrop;				// where a drag move or drag copy is completed
	bool			m_bInternalDrop;		// false at beg of drag and set to true if source == target

	UINT			m_nLastEdit;			// resource id of last edit

	POINTS			m_ptLastMouse;			// point at last mouse click
	POINTS			m_ptLastRightMouse;
	BOOL			m_bSelecting;
	enum { BegSignPost, EndSignPost };
	BOOL			ToggleSignPost(int BegOrEnd, POINTS where);
	double			 ZoomFactor();
	int				m_irowSaved;			// used to save last position of chords inserted from
											// from signpost grid

public:
	CChordMapStrip( CChordMapMgr* pChordMapMgr)
	{
		ASSERT( pChordMapMgr );
		RECT rect = {0,0,0,0};
		memcpy(&m_rectSelect, &rect, sizeof(RECT));
		m_pChordMapMgr = pChordMapMgr;
		m_pStripMgr = (IDMUSProdStripMgr*)pChordMapMgr;
		m_pStripMgr->AddRef();
		m_cRef = 0;
		AddRef();
		m_fSelected = FALSE;
		m_bSelecting = FALSE;
		m_fDragging = FALSE;
		m_bInSelectionNet = false;
		m_bDrop = false;
		m_fCapture = false;
		m_fConnectionDrag = FALSE;
//		m_fVariableNotFixed = FALSE;
		m_fVariableNotFixed = TRUE;
		ZeroMemory( &m_rcScreenClip, sizeof(m_rcScreenClip) );
		m_pbmpBackground = NULL;
		m_MenuHandler.CreateEx(0L, NULL, "Hidden command strip menu handler",
							   WS_POPUP | WS_CHILD, rect, NULL, 0, NULL);


		m_lFocusHint = 0;
		m_pfnFocusCallback = 0;

		m_nLastEdit = 0;

		m_xOrg = m_yOrg = 0;

		m_irowSaved = 0;

		m_pITargetDataObject = 0;
		m_pISourceDataObject = 0;
		m_bInternalDrop = false;
		m_dwStartDragButton = 0;
		m_dwOverDragButton = 0;
		m_dwOverDragEffect = 0;
		m_pDragImage = 0;
	};

	~CChordMapStrip()
	{
		if(IsWindow(m_MenuHandler.m_hWnd))
		{
			//m_MenuHandler.DestroyWindow();
			::DestroyWindow( m_MenuHandler.m_hWnd );
		}

		ASSERT( m_pStripMgr );
		m_pStripMgr->Release();	
		
		ASSERT( m_pbmpBackground == NULL ); // MUST be NULL or Connection drag is leaking.
	}

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
	HRESULT	STDMETHODCALLTYPE	FBDraw( HDC hDC, STRIPVIEW sv );
	HRESULT STDMETHODCALLTYPE	FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

	// helpers for IDMUSProdStripFunctionBar
	void	DrawFunctionBar( HDC hDC, STRIPVIEW sv );


// IChordMapStrip
public:
	HRESULT STDMETHODCALLTYPE	SetChordXY( ChordEntry* pChord, POINTS& Point );
	HRESULT STDMETHODCALLTYPE	GetClickedChord( POINTS& Point, ChordEntry** ppChord );
	HRESULT STDMETHODCALLTYPE	GetClickedConnection( POINTS& Point, ChordEntry** ppChord, ChordEntry** ppNextChord );
	HRESULT STDMETHODCALLTYPE	CountConnections( ChordEntry* pChord, int* pnCount );
	HRESULT STDMETHODCALLTYPE	ConnectionToChord( NextChord* pNextChord, ChordEntry** ppChord );
	HRESULT STDMETHODCALLTYPE	ChordFromID( int nID, ChordEntry** ppChord );
	HRESULT STDMETHODCALLTYPE	DrawConnectionBoxes( CDC& cDC, ChordEntry* pChord, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE	DrawChord( CDC& cDC, ChordEntry* pChord, LONG lXOffset );
	HRESULT STDMETHODCALLTYPE	SetNewChordXY( POINTS& DropPoint );
	HRESULT STDMETHODCALLTYPE	ClearSelectedChords();
	HRESULT STDMETHODCALLTYPE	GetNumSelections( int* pnSelected = NULL );
	HRESULT STDMETHODCALLTYPE	AddConnection( ChordEntry* pFromChord, ChordEntry* pToChord );
	HRESULT STDMETHODCALLTYPE	ClearDragRes();
	HRESULT STDMETHODCALLTYPE	DoPropertyPage( enum CChordMapMgr::SelectionType selection, BOOL fShow = FALSE );
	HRESULT STDMETHODCALLTYPE	DelSelChords();
	HRESULT STDMETHODCALLTYPE	DelSelConnections();
	HRESULT STDMETHODCALLTYPE	DeleteAllConnections( int nID );
	HRESULT STDMETHODCALLTYPE	DeleteConnection(NextChord* pNextChord);
	HRESULT	STDMETHODCALLTYPE	SetSelectedConnection(NextChord* pNextChord);
	HRESULT STDMETHODCALLTYPE	ResolveConnections();
	HRESULT	STDMETHODCALLTYPE	SyncRectToMeasureBeat( ChordEntry* pChord );

// IDMUSProdTimelineEdit
	HRESULT STDMETHODCALLTYPE Cut( /* in */ /* out */ IDMUSProdTimelineDataObject *pIDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Copy( /* in */ /* out */ IDMUSProdTimelineDataObject *pIDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Paste( /* in */ IDMUSProdTimelineDataObject* pIDataObject = NULL );
	HRESULT STDMETHODCALLTYPE Insert( void );
	HRESULT STDMETHODCALLTYPE Delete( void );
	HRESULT STDMETHODCALLTYPE SelectAll( void );
	HRESULT STDMETHODCALLTYPE CanCut( void );
	HRESULT STDMETHODCALLTYPE CanCopy( void );
	HRESULT STDMETHODCALLTYPE CanPaste( IDMUSProdTimelineDataObject* pIDataObject = NULL );
	HRESULT STDMETHODCALLTYPE CanInsert( void );
	HRESULT STDMETHODCALLTYPE CanDelete( void );
	HRESULT STDMETHODCALLTYPE CanSelectAll( void );

// Overloaded copy
	HRESULT Copy( IDMUSProdTimelineDataObject *pITimelineDataObject, int index );

	// IDropTarget methods
	HRESULT STDMETHODCALLTYPE DragEnter( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	HRESULT STDMETHODCALLTYPE DragLeave( void );
	HRESULT STDMETHODCALLTYPE Drop( IDataObject* pIDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

	// IDropSource
	HRESULT STDMETHODCALLTYPE QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState );
	HRESULT STDMETHODCALLTYPE GiveFeedback( DWORD dwEffect );
	
	// IDropSource helpers
	CImageList* CreateDragImage();
	HRESULT		CreateDataObject(IDataObject**, POINTS where);
	HRESULT		CreateDataObject(IDataObject** ppIDataObject, int index);
	HRESULT SaveSelectedChords(LPSTREAM, ChordEntry* pChordAtDragPoint, BOOL bNormalize = FALSE);
	HRESULT SaveSelectedChordItems(LPSTREAM, ChordEntry* pChordAtDragPoint, BOOL bNormalize = FALSE);
	void MarkSelectedChords(DWORD flags);
	void UnMarkChords(DWORD flags);
	void UnMarkChords(ChordEntryList& list, DWORD flags);
	// deletes chords marked by given flag
	void DeleteMarkedChords(DWORD flags);
	ChordEntry* FirstSelectedChord();

	// scrolling helpers
	long	LeftEdge();
	long	RightEdge();
	HRESULT	ScrollHoriz(long pos);
	HRESULT ScrollVert(long pos);

	// added jmf 8/8/97
	HRESULT STDMETHODCALLTYPE	ComputeChordMap();
	HRESULT STDMETHODCALLTYPE	ComputeChord( ChordEntry* pChord );
	HRESULT STDMETHODCALLTYPE	ComputeConnectionBoxes( ChordEntry* pChord );

	BOOL ProcessKey(int nVirtKey, unsigned long lKeyData);
	BOOL ProcessOnSize();
	void SetFocus();
	HRESULT InsertNewChord(POINTS ptWhere, ChordEntry*& pChord);
    void RenumberIDs();    // Renumber all connection IDs.


	// DropTarget helpers
	CWnd* GetTimelineCWnd();
	HRESULT CanPasteFromData(IDataObject* pIDataObject);
	HRESULT ChordEntryPasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, POINTL pt, bool bDropNotEditPaste);
	HRESULT ChordItemPasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, POINTL pt, bool bDropNotEditPaste);
	HRESULT FBPasteAt( IDMUSProdTimelineDataObject* pITimelineDataObject, POINTL pt, bool bDropNotEditPaste);
	HRESULT InsertChords(CChordList& list, POINTL pt);
	HRESULT NormalizeChordList(ChordEntryList& list, long beatOffset, 
								long* firstMeasure=0, long* firstBeat=0,
								long* lastMeasure=0, long* lastBeat=0);
	HRESULT NormalizeChordList(CChordList& list, long beatOffset, 
								long* firstMeasure=0, long* firstBeat=0,
								long* lastMeasure=0, long* lastBeat=0);
	void InternalMove(POINTS ptDrop);
	ChordEntry* InternalCopy(POINTS ptDrop);
	// other helpers
	int PaletteChordIndexFromCoords( POINTS pt);
	void ClearPaletteSelections();
	void ClearSelectedConnections();
	void FindYPositionForChord(short xpos, short& ypos, bool bUpdatePosition);
	BOOL IntersectsChordInMap(CRect& rect);
	HRESULT DragConnection(LPARAM lParam, POINTS ptMouseDown);
	void UpdateAutoScroll(DragState* pDragState);
};

typedef enum tagPIANO_KEY_COLOR
{
	WHITE,
	BLACK
} PIANO_KEY_COLOR;
const PIANO_KEY_COLOR PianoKeyColor[] = {
	WHITE,BLACK,WHITE,BLACK,WHITE,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,WHITE };



#endif //__ChordMapMGR_H_
