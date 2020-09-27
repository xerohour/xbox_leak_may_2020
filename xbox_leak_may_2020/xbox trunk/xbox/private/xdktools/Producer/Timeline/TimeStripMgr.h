// TimeStripMgr.h : Declaration of the CTimeStripMgr

#ifndef __TIMESTRIPMGR_H_
#define __TIMESTRIPMGR_H_

#include "resource.h"       // main symbols
#include <afxext.h>
#include "TimelineDraw.h"
#include <dmusici.h>

#ifndef REFCLOCKS_PER_SECOND
#define REFCLOCKS_PER_SECOND 10000000
#endif

#define STRIP_HEIGHT 30

class CTimeStrip;
/////////////////////////////////////////////////////////////////////////////
// CTimeStripMgr
class ATL_NO_VTABLE CTimeStripMgr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimeStripMgr, &CLSID_TimeStripMgr>,
	public IDMUSProdStripMgr
{
friend class CTimeStrip;
protected:
	IDMUSProdTimeline*	m_pTimeline;
	CTimeStrip*			m_pTimeStrip;
public:
	CTimeStripMgr()
	{
		m_pTimeline = NULL;
		m_pTimeStrip = NULL;
		m_TimeSig.bBeatsPerMeasure = 4;
		m_TimeSig.bBeat = 4;
		m_TimeSig.wGridsPerBeat = 4;
		m_TimeSig.mtTime = 0;
		m_dblTempo = 120;
		m_fShowTimeSig = TRUE;
	}
	~CTimeStripMgr()
	{
		if( m_pTimeline )
		{
			m_pTimeline->Release();
		}
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TIMESTRIPMGR)

BEGIN_COM_MAP(CTimeStripMgr)
	COM_INTERFACE_ENTRY(IDMUSProdStripMgr)
END_COM_MAP()

// ITimeStripMgr
public:
	STDMETHODIMP GetParam(
		/* [in] */  REFGUID		guidType,
		/* [in] */  MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME*	pmtNext,
		/* [out] */ void*		pData);
	STDMETHODIMP SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData);
	STDMETHODIMP IsParamSupported(
		/* [in] */ REFGUID		guidType);
	STDMETHODIMP OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData);
	STDMETHODIMP GetStripMgrProperty( 
		/* [in] */ STRIPMGRPROPERTY stripMgrProperty,
		/* [out] */ VARIANT*	pVariant);
	STDMETHODIMP SetStripMgrProperty( 
		/* [in] */ STRIPMGRPROPERTY smp,
		/* [in] */ VARIANT		variant);

private:
	DMUS_TIMESIGNATURE	m_TimeSig;
	double				m_dblTempo;
	BOOL				m_fShowTimeSig;
};



typedef enum tagTSRESIZE
{
	TS_NOTRESIZING	= 0,
	TS_START		= TS_NOTRESIZING + 1,
	TS_END			= TS_START + 1
} TSRESIZE;

typedef enum tagTSRTDISPLAY
{
	TS_RT_SECOND	= 0,
	TS_RT_MEASURE	= TS_RT_SECOND + 1,
	TS_RT_BEAT		= TS_RT_MEASURE + 1
} TSRTDISPLAY;

class CTimeStrip : 
	public IDMUSProdStrip,
	public IDMUSProdStripFunctionBar,
	public IDMUSProdPropPageObject
{
	friend class CTimelineCtl;
public:
	BOOL			m_fScrollWhenSettingTimeCursor;
	BOOL			m_bDisplayRealTime; // flag to enable display of real time
	int				m_nLastEdit; // ID of string to return when asked for undo text
protected:
	long			m_cRef;
	long			m_lBeginSelect;
	long			m_lEndSelect;
	TSRESIZE		m_tsrResizing;
	HCURSOR			m_hCursor;
	CTimeStripMgr*	m_pTimeStripMgr;
	BOOL			m_fSelecting;
	COLORREF		m_colorTimeTick;
	CBitmapButton	m_BtnRealTime;	// Real Time button
	CBitmapButton	m_BtnMusicTime;	// Music Time button
	BOOL			m_bDisplayMusicTime; // flag to enable display of music time
	TSRTDISPLAY		m_tsdDisplayRealTime; // setting for display of real time
	DMUSPROD_TIMELINE_SNAP_TO m_stSetting;	// Snap to setting
	UINT			m_nScrollTimerID;
	BOOL			m_fLButtonDown;
public:
	CTimeStrip( CTimeStripMgr* pTimeStripMgr )
	{
		ASSERT( pTimeStripMgr );
		m_cRef = 0;
		AddRef();
		m_pTimeStripMgr = pTimeStripMgr;
		m_fSelecting = FALSE;
		m_hCursor = LoadCursor( NULL, IDC_ARROW );
		m_lBeginSelect = -1;
		m_lEndSelect = -1;
		m_tsrResizing = TS_NOTRESIZING;
		m_bDisplayMusicTime = TRUE;
		m_bDisplayRealTime = FALSE;
		m_tsdDisplayRealTime = TS_RT_SECOND;
		m_stSetting = DMUSPROD_TIMELINE_SNAP_GRID;
		m_colorTimeTick = RGB( 0, 0, 0 );
		m_nScrollTimerID = 0;
		m_fLButtonDown = FALSE;
		m_nLastEdit = 0;
		m_fScrollWhenSettingTimeCursor = TRUE;
	};

	~CTimeStrip()
	{
		if( m_nScrollTimerID )
		{
			StopScrollTimer();
		}
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
public:
    HRESULT STDMETHODCALLTYPE FBDraw( HDC hDC, STRIPVIEW sv);
    HRESULT STDMETHODCALLTYPE FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos );

// IDMUSProdPropPageObject functions
public:
	HRESULT STDMETHODCALLTYPE GetData( /* [retval][out] */ void **ppData);
	HRESULT STDMETHODCALLTYPE SetData( /* [in] */ void *pData);
	HRESULT STDMETHODCALLTYPE OnShowProperties( void);
	HRESULT STDMETHODCALLTYPE OnRemoveFromPageManager( void);

public:
	long SnapAmount( long lTime );
	void SetSnapTo( DMUSPROD_TIMELINE_SNAP_TO stNew );

protected:
	void OnGutterSelect( BOOL fSelect );
	void OnLButtonDown( WPARAM wParam, LONG lXPos );
	void OnMouseMove( LONG lXPos );
	void OnButtonClicked( WPARAM wParam );
	HRESULT OnSize( WPARAM wParam, LPARAM lParam );
	HCURSOR GetWECursor(void);
	HCURSOR GetArrowCursor(void);
	void OnDrawItem( WPARAM wParam, LPARAM lParam );
	void DrawMusicTime( HDC hDC, LONG lXOffset );
	void DrawRealOnSecond( HDC hDC, LONG lXOffset );
	void DrawRealOnBeat( HDC hDC, LONG lXOffset );
	void DrawRealOnMeasure( HDC hDC, LONG lXOffset );
	void SnapTime( long *plTime );
	void UpdateStartEnd( long lNewBegin, long lNewEnd );
	void StartScrollTimer( UINT nScrollTimerID );
	void StopScrollTimer();
	CWnd* GetTimelineCWnd();
	void OnTimer();
};

#endif //__TIMESTRIPMGR_H_
