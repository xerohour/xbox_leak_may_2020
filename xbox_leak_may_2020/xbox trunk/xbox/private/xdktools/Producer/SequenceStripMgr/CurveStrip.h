#ifndef __CURVESTRIP_H_
#define __CURVESTRIP_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CurveStrip.h : Declaration of the CurveStrip

#include "resource.h"       // main symbols
#include "timeline.h"
#include <afxtempl.h>
#include <dmusprod.h>

#include "Tracker.h"
#include <RiffStrm.h>


#define CRV_MINIMIZE_HEIGHT	   20
#define CRV_DEFAULT_HEIGHT	   80
#define CRV_MAXHEIGHT_HEIGHT   200

#define SP_CURVESTRIP	SP_USER

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
#define DMUS_FOURCC_CURVE_CLIPBOARD_CHUNK	mmioFOURCC('c','v','c','c')


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

class CSequenceMgr;
class CSequenceStrip;
class CCurvePropPageMgr;
class CPropPageCurve;
class CPropPageCurveReset;

/////////////////////////////////////////////////////////////////////////////
// CCurveStrip class

class CCurveStrip :
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdTimelineEdit,
	public IDMUSProdPropPageObject
{
friend CSequenceMgr;
friend CSequenceStrip;
friend CCurveTracker;
friend CCurvePropPageMgr;
friend CPropPageCurve;
friend CPropPageCurveReset;

public:
	CCurveStrip( CSequenceMgr* pSequenceMgr, CSequenceStrip* pSequenceStrip, BYTE bCCType, WORD wRPNType );
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
	HRESULT OnSingleCurve();
    HRESULT Load( IStream* pIStream, TIMELINE_PASTE_TYPE tlPasteType );
    HRESULT Save( IStream* pIStream );
	
	// Inserting new Curves
	void StartInsertingCurve( void );
	void StopInsertingCurve();
	void InsertCurve( long lXPos, long lYPos );
	void OnUpdateInsertCurveValue( POINT ptLeft, POINT ptRight );
	void OnUpdateDragCurveValue(CCurveTracker& curveTracker, int nAction);
	
	// Modifying Curves
	CRect GetUpdatedCurveRect(CCurveTracker& curveTracker, 
		CCurveItem* pCurve, int nAction);
	void DragModify(CCurveItem* pDMCurve, int nStripXPos, int nStripYPos, int nAction, bool fDuplicateCurves);

	// Deleting Curves
	void DeleteSelectedCurves();
	WORD DeleteAllCurves();
	void DeleteCurvesBetweenTimes( MUSIC_TIME mtEarliestCurve, MUSIC_TIME mtLatestCurve, long lTime );

	// Drawing methods
	BOOL UseCenterLine( void );
	HRESULT DrawMaximizedStrip( HDC hDC, RECT* pRectStrip, RECT* pRectClip, LONG lXOffset );
	HRESULT DrawMinimizedStrip( HDC hDC, RECT* pRectStrip );
	void DrawCurve( CCurveItem* pDMCurve, HDC hDC, RECT* pRectStrip, LONG lXOffset );
	void InvertGutterRange( HDC hDC, RECT* pRectClip, LONG lXOffset );

	// Selection methods
	CCurveItem*	GetCurveFromPoint( int nXPos, int nYPos, BOOL bFrameOnly = FALSE );
	WORD SelectCurve( CCurveItem* pDMCurve, BOOL fState );
	WORD SelectAllCurves( BOOL fState );
	WORD SelectCurvesInMusicTimeRange( long lBegin, long lEnd );
	WORD SelectCurvesInRect( CRect* pRect );
	WORD UnselectAllCurvesInPart( void );
	void UnselectGutterRange();
	void OnGutterSelectionChange( BOOL fChanged );

	// Clipboard operations
	HRESULT CreateCopyStream( IStream** ppIStream );
	HRESULT MergeDataObjects( IDataObject* pIDataObject1, IDataObject* pIDataObject2, IDataObject** ppINewDataObject );
	HRESULT LoadDataObjectCurves( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, TIMELINE_PASTE_TYPE tlPasteType );
	HRESULT SaveSelectedCurves( IDMUSProdRIFFStream* pIRiffStream );
	HRESULT PasteCurve( CCurveItem* pDMCurve, long lCursorGrid );

	// Helper methods
	void RefreshUI(WORD& wRefreshUI);
	HWND GetMyWindow();
	BOOL GetStripRect(LPRECT rectStrip);
	CCurveItem* GetEarliestSelectedCurve( void );
		
	BOOL IsCurveForThisStrip( CCurveItem* pDMCurve );
	BOOL IsAnyCurveSelected();
	void RefreshCurvePropertyPage();
	void RefreshCurveStrips();
	short YPosToValue( long lYPos );
	short ValueToYPos( long lValue );
	int AdjustXPos( int nXPos );
	long CalcPartClockLength();
	UINT HitTest( CCurveTracker& tracker, long lStripXPos, long lStripYPos );

protected:
	DWORD				m_dwRef;
	CSequenceMgr*		m_pSequenceMgr;			// pointer to our Sequence manager
	IDMUSProdStripMgr*	m_pStripMgr;
	CSequenceStrip*		m_pSequenceStrip;	// pointer to corresponding Sequence strip
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
