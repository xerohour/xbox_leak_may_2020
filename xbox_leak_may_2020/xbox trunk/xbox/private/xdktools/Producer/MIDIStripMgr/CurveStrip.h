// CurveStrip.h : Declaration of the CurveStrip

#ifndef __CURVESTRIP_H_
#define __CURVESTRIP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "timeline.h"
#include "MIDIStripMgr.h"
#include <afxtempl.h>
#include "Pattern.h"
#include "DMUSProd.h"

#define CRV_MINIMIZE_HEIGHT	   20
#define CRV_DEFAULT_HEIGHT	   80
#define CRV_MAXHEIGHT_HEIGHT   200

#define CURVE_MIN_SIZE			6

#define INSERT_CURVE_FALSE			0
#define INSERT_CURVE_START_VALUE	1
#define INSERT_CURVE_END_VALUE		2

#define CCTYPE_DATA_MSB_CURVE_STRIP		0x06
#define CCTYPE_PAN_CURVE_STRIP			0x0A
#define CCTYPE_DATA_LSB_CURVE_STRIP		0x26
#define CCTYPE_RPN_LSB_CURVE_STRIP		0x64
#define CCTYPE_RPN_MSB_CURVE_STRIP		0x65
#define CCTYPE_PB_CURVE_STRIP			0x80
#define CCTYPE_MAT_CURVE_STRIP			0x81
#define CCTYPE_PAT_CURVE_STRIP			0x82
#define CCTYPE_RPN_CURVE_STRIP			0x83
#define CCTYPE_NRPN_CURVE_STRIP			0x84
#define CCTYPE_MINIMIZED_CURVE_STRIP	0xFF

#define NBR_EXTRA_CCS			3	// PB, AT(M), AT(P) (CC's other than 0-127)

#define FOURCC_SEQ_CURVE_CLIPBOARD_FORM		mmioFOURCC('s','q','c','f')
#define DMUS_FOURCC_CURVE_CLIPBOARD_FORM	mmioFOURCC('c','v','c','f')
#define DMUS_FOURCC_CURVE_CLIPBOARD_CHUNK	mmioFOURCC('c','v','c','c')

#pragma pack(2)
struct ioDMCurveClipInfo
{
	DWORD	m_dwVariations;			// Variation buttons selected at time of clip
	DirectMusicTimeSig	m_ts;		// TimeSignature of clip
	BYTE	m_bCCType;				// Type of CC's contained in Curve clip
	BYTE	m_bPad;
	WORD	m_wRPNType;
};
#pragma pack()

class CPropPageCurve;
class CPropPageCurveReset;
class CCurveTracker;

/////////////////////////////////////////////////////////////////////////////
// CCurveRectItem class

class CCurveRectItem : public AListItem
{
friend class CCurveStrip;

public:
	CCurveRectItem( RECT* pRect );
	virtual ~CCurveRectItem(); 
	CCurveRectItem* GetNext() { return (CCurveRectItem*) AListItem::GetNext(); }

protected:
	RECT		m_rect;		// Curve rectangle
};


/////////////////////////////////////////////////////////////////////////////
// CCurveRectList class

class CCurveRectList : public AList
{
public:
	virtual ~CCurveRectList() { if(m_pHead != NULL) delete m_pHead; } 
    CCurveRectItem *GetHead() const { return (CCurveRectItem *)AList::GetHead(); };
    CCurveRectItem *RemoveHead() { return (CCurveRectItem *)AList::RemoveHead(); };
};


/////////////////////////////////////////////////////////////////////////////
// CCurveStrip class

class CCurveStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDMUSProdPropPageObject
{
friend CMIDIMgr;
friend CPianoRollStrip;
friend CCurveTracker;
friend CCurvePropPageMgr;
friend CPropPageCurve;
friend CPropPageCurveReset;

public:
	CCurveStrip( CMIDIMgr* pMIDIMgr, CPianoRollStrip* pPianoRollStrip, BYTE bCCType, WORD wRPNType );
	~CCurveStrip();

public:
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

// IDMUSProdPropPageObject functions
	HRESULT STDMETHODCALLTYPE GetData( void **ppData );
	HRESULT STDMETHODCALLTYPE SetData( void *pData );
	HRESULT STDMETHODCALLTYPE OnShowProperties( void );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void );

// Additional methods
protected:
	HRESULT OnContextMenu( void );
	int		GetMouseMode( long lXPos, long lYPos );
	HRESULT OnLButtonDown( WPARAM wParam, long lXPos, long lYPos );
	HRESULT OnLButtonUp( void );
	HRESULT OnRButtonDown( WPARAM wParam, long lXPos, long lYPos );
	HRESULT OnRButtonUp( void );
	HRESULT OnMouseMove( long lXPos, long lYPos );
	HRESULT OnSetCursor( long lXPos, long lYPos);
	HRESULT OnSingleCurve( void );
    HRESULT Load( IStream* pIStream );
    HRESULT Save( IStream* pIStream );
    HRESULT SaveForSeqTrack( IStream* pIStream );
    HRESULT LoadFromSeqTrack( IStream* pIStream );
	HRESULT LoadDataObjectCurvesFromSeqTrack( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	
	// Inserting new Curves
	void StartInsertingCurve( void );
	void StopInsertingCurve( void );
	void InsertCurve( long lXPos, long lYPos );
	void OnUpdateInsertCurveValue( POINT ptLeft, POINT ptRight );
	void OnUpdateDragCurveValue(CCurveTracker& curveTracker, int nAction);
	
	// Modifying Curves
	CRect GetUpdatedCurveRect(CCurveTracker& curveTracker, 
		CDirectMusicStyleCurve* pCurve, int nAction);
	void DragModify(CDirectMusicStyleCurve* pDMCurve, int nStripXPos, int nStripYPos, RECT *pRectCurve, int nAction, bool fMakeCopy);

	// Deleting Curves
	void DeleteSelectedCurves();
	WORD DeleteAllCurves();

	// Drawing methods
	BOOL UseCenterLine( void );
	HRESULT DrawMaximizedStrip( HDC hDC, RECT* pRectStrip, RECT* pRectClip, LONG lXOffset );
	HRESULT DrawMinimizedStrip( HDC hDC, RECT* pRectStrip );
	void DrawCurve( CDirectMusicStyleCurve* pDMCurve, HDC hDC, RECT* pRectStrip, LONG lXOffset, MUSIC_TIME mtOffset );
	void InvertGutterRange( HDC hDC, RECT* pRectClip, LONG lXOffset );
	static void DrawMaximizedStrip_Callback( CDirectMusicPartRef *pPartRef, MUSIC_TIME mtPartOffset, long lGridLength, void *pThis, void *pData );

	// Selection methods
	CDirectMusicStyleCurve*	GetCurveFromPoint( int nXPos, int nYPos, BOOL bFrameOnly = FALSE, RECT *pRectCurve = NULL );
	WORD SelectCurve( CDirectMusicStyleCurve* pDMCurve, BOOL fState );
	WORD SelectAllCurves( BOOL fState );
	static void SelectCurvesInMusicTimeRange_Callback( CDirectMusicPartRef *pPartRef, MUSIC_TIME mtPartOffset, long lGridLength, void *pThis, void *pData );
	WORD SelectCurvesInMusicTimeRange( long lBegin, long lEnd );
	static void SelectCurvesInRect_Callback( CDirectMusicPartRef *pPartRef, MUSIC_TIME mtPartOffset, long lGridLength, void *pThis, void *pData );
	WORD SelectCurvesInRect( CRect* pRect );
	WORD UnselectAllCurvesInPart( void );
	void UnselectGutterRange();
	void OnGutterSelectionChange( BOOL fChanged );
	static void GetCurveFromPoint_Callback( CDirectMusicPartRef *pPartRef, MUSIC_TIME mtPartOffset, long lGridLength, void *pThis, void *pData );

	// Clipboard operations
	HRESULT CreateCopyStream( IStream** ppIStream, IStream** ppISeqStream );
	HRESULT MergeDataObjects( IDataObject* pIDataObject1, IDataObject* pIDataObject2, IDataObject** ppINewDataObject );
	HRESULT LoadDataObjectCurves( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
	HRESULT SaveSelectedCurves( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT SaveSelectedCurvesForSeqTrack( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT PasteCurve( CDirectMusicPart* pDMPart, CDirectMusicStyleCurve* pDMCurve, long lCursorGrid, DWORD dwVariationMap[] );

	// Helper methods
	void RefreshUI(WORD& wRefreshUI);
	HWND GetMyWindow();
	BOOL GetStripRect(LPRECT rectStrip);
	CDirectMusicStyleCurve* GetEarliestSelectedCurve( void );
	void DeleteCurvesBetweenTimes( MUSIC_TIME mtEarliestCurve, MUSIC_TIME mtLatestCurve, long lCursorGrid );
		
	BOOL IsCurveForThisStrip( CDirectMusicStyleCurve* pDMCurve );
	BOOL IsAnyCurveSelected();
	void RefreshCurvePropertyPage();
	void RefreshCurveStrips();
	short YPosToValue( long lYPos );
	short ValueToYPos( long lValue );
	long CalcPartClockLength();
	static void HitTest_Callback( CDirectMusicPartRef *pPartRef, MUSIC_TIME mtPartOffset, long lGridLength, void *pThis, void *pData );
	UINT HitTest( CDirectMusicStyleCurve* pDMCurve, long lStripXPos, long lStripYPos );

protected:
	DWORD				m_dwRef;
	IDMUSProdFramework* m_pIFramework;
	CMIDIMgr*			m_pMIDIMgr;			// pointer to our MIDI manager
	IDMUSProdStripMgr*	m_pStripMgr;
	CPianoRollStrip*	m_pPianoRollStrip;	// pointer to corresponding Piano Roll strip
	BYTE				m_bCCType;			// Type of CC displayed in Curve strip
	WORD				m_wRPNType;			// Type of RPN/NRPN displayed in Curve strip

	HCURSOR 			m_hCursor;
	HANDLE				m_hKeyStatusBar;

	BOOL				m_fInsertingCurve;
	short				m_nInsertingStartValue;		// new curve's start value
	short				m_nInsertingEndValue;		// new curve's end value
	long				m_lInsertingStartClock;		// new curve's start music time
	long				m_lInsertingEndClock;		// new curve's music time

	CCurveTracker*		m_pCCurveTracker;			// If set, massage the curves in GetData()
	int					m_nTrackerAction;			// If set, the action of the curve tracker
};


#endif //__CURVESTRIP_H_
