// TimelineCtl.cpp : Implementation of CTimelineCtl
// @doc Timeline
#include "stdafx.h"
#include <math.h>
#include <SegmentGuids.h>
#include "Timeline.h"
#include "TimelineCtl.h"
#include "TimeStripMgr.h"
#pragma warning( push )
#pragma warning( disable : 4005 )
#include <winresrc.h>
#pragma warning( pop )
#include <Conductor.h>
#include <initguid.h>
#include <dmusici.h>
#include <dmusicf.h>
#include "TimelineDataObject.h"
#include "SegmentIO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SHADING_BLOCK_WIDTH 16

#define TRACKCONFIG_PLAY_FLAGS (DMUS_TRACKCONFIG_PLAY_ENABLED | DMUS_TRACKCONFIG_CONTROL_ENABLED | DMUS_TRACKCONFIG_NOTIFICATION_ENABLED)

long	CTimelineCtl::m_lBitmapRefCount = 0;
CBitmap	CTimelineCtl::m_BitmapZoomIn;
CBitmap	CTimelineCtl::m_BitmapZoomOut;
CBitmap	CTimelineCtl::m_BitmapMinimize;
CBitmap	CTimelineCtl::m_BitmapMinimized;
CBitmap	CTimelineCtl::m_BitmapMaximize;
CBitmap	CTimelineCtl::m_BitmapMaximized;
CString	CTimelineCtl::m_strZoomInChars;
CString	CTimelineCtl::m_strZoomOutChars;
bool	CTimelineCtl::m_fFilledColorArrays = false;
DWORD	CTimelineCtl::m_adwNormalColor[256];
DWORD	CTimelineCtl::m_adwActiveColor[256];
/*
DWORD	CTimelineCtl::m_adwSelectedColor[256];
DWORD	CTimelineCtl::m_adwActiveSelectedColor[256];
*/

// Helper function for RealTime display

void RefTimeToString( REFERENCE_TIME rtTime, int nResourceID, int nNegativeResourceID, CString &cstrTime )
{
	bool fNegative = false;
	if( rtTime < 0 )
	{
		fNegative = true;
		rtTime = -rtTime;
	}

	int iMillisecond, iSecond, iMinute, iHour;
	// Convert to milliseconds
	iMillisecond = int(rtTime / 10000);
	iSecond = iMillisecond / 1000;
	iMillisecond %= 1000;
	iMinute = iSecond / 60;
	iSecond %= 60;
	iHour = iMinute / 60;
	iMinute %= 60;

	CString strFormat;
	if( strFormat.LoadString( fNegative ? nNegativeResourceID : nResourceID ) )
	{
		cstrTime.Format(strFormat, iHour, iMinute, iSecond, iMillisecond);
	}
	else
	{
		cstrTime.Format(fNegative ? "-%02d:%02d:%02d.%03d" : "%02d:%02d:%02d.%03d", iHour, iMinute, iSecond, iMillisecond);
	}
}

BOOL StripSupportTimeSigs( IDMUSProdStrip *pStrip )
{
	if( !pStrip )
	{
		return FALSE;
	}

	VARIANT var;
	BOOL fSupportsTimeSigs = FALSE;
	if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &var ) ) 
	&&	V_UNKNOWN(&var) )
	{
		IDMUSProdStripMgr *pStripMgr;
		if( SUCCEEDED( V_UNKNOWN(&var)->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgr ) ) )
		{
			if( pStripMgr->IsParamSupported( GUID_TimeSignature ) == S_OK )
			{
				fSupportsTimeSigs = TRUE;
			}
			pStripMgr->Release();
		}
		V_UNKNOWN(&var)->Release();
	}

	return fSupportsTimeSigs;
}


/*  --------------------------------------------------------------------------
	@interface IDMUSProdTimelineCallback | The Timeline uses this interface to complete its
		<om IDMUSProdTimeline::OnDataChanged> method.

	@meth HRESULT | OnDataChanged | Called when data in one of the strips changes.

	@base public | IUnknown

	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <om IDMUSProdTimeline::OnDataChanged>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineCallback | OnDataChanged | This method is
		called in response to a call to <om IDMUSProdTimeline::OnDataChanged>.

	@comm	Typically an <i IDMUSProdStripMgr> interface is passed in <p punk> and this method
		is used to notify the component that created it that data in the strip has changed.
		The component should then get the new data from the <p punk>, either by calling
		<om IStream::Save> or another method.

	@parm   IUnknown* | punk | The interface to pass.

	@xref	<om IDMUSProdTimeline::OnDataChanged> <i IDMUSProdStripMgr>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@interface IDMUSProdTimeline | Coordinates the display of strips in a Timeline format.

	@meth HRESULT | AddStripMgr | Add a strip manager to the Timeline.

	@meth HRESULT | AddStrip | Add a strip to the Timeline.

	@meth HRESULT | SetMarkerTime | Set the location of one of the selection markers.

	@meth HRESULT | GetMarkerTime | Get the location of one of the selection markers.

 	@meth HRESULT | ClocksToPosition | Convert from a time in clocks to a horizontal pixel position.

	@meth HRESULT | PositionToClocks | Convert from a horizontal pixel position to a time in clocks.

	@meth HRESULT | DrawMusicLines | Draw vertical bar, beat, and grid lines in the specified device context.

	@meth HRESULT | SetTimelineProperty | Set a property of the Timeline.

	@meth HRESULT | GetTimelineProperty | Get a property of the Timeline.

	@meth HRESULT | Refresh | Redraw the entire Timeline window.

	@meth HRESULT | ClocksToMeasureBeat | Convert from a time in clocks to a measure and beat value.

	@meth HRESULT | PositionToMeasureBeat | Convert from a pixel position to a measure and beat value.

 	@meth HRESULT | MeasureBeatToClocks | Convert from a measure and beat to a time in clocks.

	@meth HRESULT | MeasureBeatToPosition | Convert from a measure and beat to a pixel position.

	@meth HRESULT | StripInvalidateRect | Invalidate the specified rectangle in a strip, causing the area to be redrawn.

	@meth HRESULT | SetPropertyPage | Change the currently display property page.

	@meth HRESULT | RemovePropertyPageObject | Remove a property page object from the currently displayed property sheet.

	@meth HRESULT | StripSetTimelineProperty | Set a strip property that is controlled by the Timeline.

	@meth HRESULT | OnDataChanged | Notify the main editor that data has changed.

	@meth HRESULT | TrackPopupMenu | Display a context menu at the specified position.

	@meth HRESULT | ClocksToRefTime | Convert from a time in clocks to a time in REFERENCE_TIME units.

	@meth HRESULT | PositionToRefTime | Convert from a pixel position to a time in REFERENCE_TIME units.

	@meth HRESULT | MeasureBeatToRefTime | Convert from a measure and beat to a time in REFERENCE_TIME units.

	@meth HRESULT | RefTimeToClocks | Convert from a time in REFERENCE_TIME units to a time in clocks.

	@meth HRESULT | RefTimeToPosition | Convert from a time in REFERENCE_TIME units to a pixel position.

	@meth HRESULT | RefTimeToMeasureBeat | Convert from a time in REFERENCE_TIME units to a measure and beat value.

	@meth HRESULT | ScreenToStripPosition | Convert a point from screen coordinates to strip coordinates.

	@meth HRESULT | StripGetTimelineProperty | Get a strip property that is controlled by the Timeline.

	@meth HRESULT | RemoveStripMgr | Remove a strip manager from the Timeline.

	@meth HRESULT | RemoveStrip | Remove a strip from the Timeline.

	@meth HRESULT | GetParam | Retrieve data from a strip manager in the Timeline.

	@meth HRESULT | SetParam | Set data on a strip manager in the Timeline

	@meth HRESULT | GetStripMgr | Retrieve a specified strip manager.

	@meth HRESULT | InsertStripAtDefaultPos | Insert a strip into the Timeline, using the default sorting order.

	@meth HRESULT | EnumStrip | Enumerate through all strips displayed within the Timeline.

	@meth HRESULT | InsertStripAtPos | Insert a strip into the Timeline at a specified position.

	@meth HRESULT | StripToWindowPos | Convert a point from strip coordinates to Timeline Window coordinates.

	@meth HRESULT | AddToNotifyList | Register a StripMgr to receive notifications of a specified type.

	@meth HRESULT | RemoveFromNotifyList | Unregister a StripMgr from receiving notifications of a specified type.

	@meth HRESULT | NotifyStripMgrs | Broadcast the specified notification to all registered strip managers.

	@meth HRESULT | AllocTimelineDataObject | Allocate an object that implements the <i IDMUSProdTimelineDataObject> interface.

	@meth HRESULT | GetPasteType | Returns the type of Paste operation to perform.

	@meth HRESULT | SetPasteType | Sets the type of Paste operation to perform.

	@base public | IUnknown

	@xref	<i IDMUSProdStrip>, <i IDMUSProdStripMgr>, <i IDMUSProdStripFunctionBar>,
		<i IDMUSProdTimelineEdit>, <i IDMUSProdTimelineCallback>, <i IDMUSProdTimelineDataObject>
	--------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
//
//	@enum TIMETYPE | Time unit types
//		@emem TIMETYPE_CLOCKS | Use clocks units.
//		@emem TIMETYPE_MS | Use millisecond units.  This is converted to and from clocks internally.
//
//	@enum TIMELINE_PROPERTY | Timeline properties
//		@emem TP_CLOCKLENGTH | The length of the Timeline, in ticks.
//		@emem TP_STRIPMOUSECAPTURE | Allows a strip to capture the mouse cursor.  This should be
//					set to true when a strip receives a WM_LBUTTONDOWN, WM_RBUTTONDOWN, or
//					WM_LBUTTONDBLCLK message.  Getting this parameter is unsupported.
//		@emem TP_DMUSPRODFRAMEWORK | Sets or gets a pointer to the DirectMusic Producer Framework.
//		@emem TP_TIMELINECALLBACK | Sets or gets a pointer to the object which receives notification
//					when data changes in any of the strips.  The object must implement the
//					<i IDMUSProdTimelineCallback> interface.
//		@emem TP_ACTIVESTRIP | Sets or gets a pointer to the active strip.  NULL is a valid parameter
//					for setting this property, but it will never be successfully returned when
//					retrieving this property.
//		@emem TP_FUNCTIONBAR_WIDTH | The width of the function bar as a number of pixels as a VT_I4.
//		@emem TP_MAXIMUM_HEIGHT | The maximum height of a strip, if the entire strip is to be shown
//					at once.  Setting this parameter is unsupported.
//		@emem TP_ZOOM | The horizontal zoom faction, as a VT_R8 parameter.
//		@emem TP_HORIZONTAL_SCROLL | The horizontal scroll amount.
//		@emem TP_VERTICAL_SCROLL | The vertical scroll amount.
//		@emem TP_SNAPAMOUNT | Returns the snap granularity for the given VT_I4 parameter.  Setting
//					this parameter is unsupported.
//		@emem TP_FREEZE_UNDO | Segment specific: When set to TRUE, any calls to OnDataChanged will
//					not create undo states, but the changes will still be taken.
//		@emem TP_SNAP_TO | (<t DMUSPROD_TIMELINE_SNAP_TO>) as a VT_I4.  Gets or sets the snap-to boundary.
//		@emem TP_GUTTER_WIDTH | The width of the gutter as a number of pixels as a VT_I4.  Setting
//					this parameter is unsupported.
//
//	@enum DMUSPROD_TIMELINE_SNAP_TO | Snap-to settings
//		@emem DMUSPROD_TIMELINE_SNAP_NONE | No snap setting.
//		@emem DMUSPROD_TIMELINE_SNAP_GRID | Snap to nearest grid.
//		@emem DMUSPROD_TIMELINE_SNAP_BEAT | Snap to nearest beat.
//		@emem DMUSPROD_TIMELINE_SNAP_BAR | Snap to nearest bar.
//
//	@enum STRIP_TIMELINE_PROPERTY | Timeline strip properties
//		@emem STP_VERTICAL_SCROLL | VT_I4.  The current vertical scroll position of the strip (in pixels).
//		@emem STP_GET_HDC | VT_I4.  Gets a handle to the device context (a HDC stored as a VT_I4) that
//					is used to draw the strip.  Setting this parameter is unsupported.
//		@emem STP_HEIGHT | VT_I4.  The current height of the strip.
//		@emem STP_STRIPVIEW | VT_I4.  The current <t STRIPVIEW> of the strip.
//		@emem STP_STRIP_RECT | VT_BYREF.  Allows a strip to get its clipped boundaries in Timeline
//					window coordinates.  The caller must pass a VARIANT of type VT_BYREF that contains
//					a non-NULL pointer to a RECT structure.  Setting this parameter is unsupported.
//		@emem STP_FBAR_RECT | VT_BYREF.  Allows a strip to get the boundaries of its function bar
//					(including the gutter and line diving the function bar from the rest of the strip)
//					in Timeline window coordinates.  The caller must pass a VARIANT of type
//					VT_BYREF that contains a non-NULL pointer to a RECT structure.  Setting this
//					parameter is unsupported.
//		@emem STP_FBAR_CLIENT_RECT | VT_BYREF.  Allows a strip to get the boundaries of the drawable
//					area of its function bar in the window coordinates used in the <om IStrip::FBDraw>
//					method.  The caller must pass a VARIANT of type	VT_BYREF that contains a non-NULL
//					pointer to a RECT structure.  Setting this parameter is unsupported.
//		@emem STP_POSITION | VT_I4.  Returns the position of the strip, with the top strip having the
//					value of 0.  Setting this parameter is unsupported.
//		@emem STP_GUTTER_SELECTED | VT_BOOL.  Allows a strip to get or set its gutter selection state.
//					When the strip is selectd, its gutter will display either as red or orange.  When
//					the strip is unselected, its gutter will display as either grey or yellow.
//		@emem STP_ENTIRE_STRIP_RECT | VT_BYREF.  Allows a strip to get its unclipped boundaries in Timeline
//					window coordinates.  The caller must pass a VARIANT of type VT_BYREF that contains
//					a non-NULL pointer to a RECT structure.  Setting this parameter is unsupported.
//		@emem STP_STRIP_INFO | VT_BYREF.  Allows a strip to set its position properties, if the strip was
//					not added by <om IDMUSProdTimeline::InsertStripAtDefaultPos>.  The caller must pass a
//					VARIANT of type VT_BYREF that contains a non-NULL pointer to a <t DMUSPROD_TIMELINE_STRIP_INFO>
//					structure.  Getting this parameter is unsupported.
//
//	@enum MARKERID | Marker types in the time strip
//		@emem MARKER_CURRENTTIME | The current time position, displayed as a infinitely tall
//			line.
//		@emem MARKER_BEGINSELECT | The start of the edit selection.
//		@emem MARKER_ENDSELECT | The end of the edit selection.
//		@emem MARKER_LEFTDISPLAY | The left edge of the strip display window.  Setting this
//			parameter is unsupported.
//		@emem MARKER_RIGHTDISPLAY | The right edge of the strip display window.  Setting this
//			parameter is unsupported.
//
//	@enum MUSICLINE_PROPERTY | Type of bar lines to draw
//		@emem ML_DRAW_MEASURE_BEAT_GRID | Draw bar, beat, and grid lines.
//		@emem ML_DRAW_MEASURE_BEAT | Draw bar and beat lines.
//
//	@enum TIMELINE_PASTE_TYPE | Type of paste operation to do
//		@emem TL_PASTE_MERGE | Merge pasted data with existing data.
//		@emem TL_PASTE_OVERWRITE | Overwrite existing data with pasted data.
//

CTimelineCtl::CTimelineCtl()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_pDMUSProdFramework = NULL;
	m_bWindowOnly = TRUE; 
	m_pStripMgrList = NULL;
	m_pStripList = NULL;
	m_pActiveStripList = NULL;
	m_pTempoMapMgr = NULL;
	m_dblZoom = .125; // set initial zoom factor
	m_MouseMode = MM_NORMAL;
	m_pMouseStripList = NULL;
	m_lLength = DMUS_PPQ * 4 * 20; // start at 20 measures, four quarter notes per measure
	m_lBeginSelect = 0;
	m_lEndSelect = 0;
	m_lCursor = 0;
	m_lXScrollPos = 0;
	m_lYScrollPos = 0;
	m_fStripMouseCapture = FALSE;
	m_fFreezeUndo = FALSE;
	m_bPasting = FALSE;
	m_lFunctionBarWidth = LEFT_DRAW_OFFSET + 80;
	m_colorMeasureLine = MEASURE_LINE_COLOR;
	m_colorBeatLine = BEAT_LINE_COLOR;
	m_colorSubBeatLine = SUBBEAT_LINE_COLOR;
	m_pLastDragStripList = NULL;
	m_pCurrentDataObject = NULL;
	m_pCopyDataObject = NULL;
	m_dwDragScrollTick = 0;
	m_pTimelineCallback = NULL;
	m_fMusicTime = TRUE;
	m_fHScrollTracking = FALSE;
	m_fActiveGutterState = FALSE;
	m_ptPasteType = TL_PASTE_OVERWRITE;
	// Default values - real values are set in OnCreate()
	m_sizeMinMaxButton.cx = 16;
	m_sizeMinMaxButton.cy = 16;
	m_lstNotifyEntry.RemoveAll();
	InitializeCriticalSection( &m_csOnDataChanged );
	m_pIDMPerformance = NULL;
	m_rtSegmentStart = 0;
	m_rtLastElapsedTime = 0;
	m_hKeyRealTimeStatusBar = NULL;
	m_pTimeStrip = NULL;
	m_lLastEarlyPosition = 0;
	m_lLastLatePosition = 0;
	m_lLastLateTime = 0;
	if( m_strZoomInChars.IsEmpty() )
	{
		if( !m_strZoomInChars.LoadString( IDS_ZOOM_IN_KEYS ) )
		{
			m_strZoomInChars = _T("iI");
		}
	}
	if( m_strZoomOutChars.IsEmpty() )
	{
		if( !m_strZoomOutChars.LoadString( IDS_ZOOM_OUT_KEYS ) )
		{
			m_strZoomOutChars = _T("oO");
		}
	}

	if( !m_fFilledColorArrays )
	{
		// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
		DWORD dwColorBase = RGB( SHADING_DARK_COLOR, SHADING_DARK_COLOR, SHADING_DARK_COLOR );
		int iBScale = 255 - SHADING_DARK_COLOR;
		int iGScale = 255 - SHADING_DARK_COLOR;
		int iRScale = 255 - SHADING_DARK_COLOR;
		for( long lIndex = 0; lIndex < 256; lIndex++ )
		{
			// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
			const DWORD dwColor = dwColorBase + RGB( (iBScale * lIndex) / 255, (iGScale * lIndex) / 255, (iRScale * lIndex) / 255 );
			m_adwNormalColor[lIndex] = dwColor;
		}

		// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
		dwColorBase = RGB( 192, 255, 255 ); // Yellow (COLOR_GUTTER_ACTIVE)
		iBScale = 255 - 192;
		iGScale = 255 - 255;
		iRScale = 255 - 255;
		for( lIndex = 0; lIndex < 256; lIndex++ )
		{
			// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
			const DWORD dwColor = dwColorBase + RGB( (iBScale * lIndex) / 255, (iGScale * lIndex) / 255, (iRScale * lIndex) / 255 );
			m_adwActiveColor[lIndex] = dwColor;
		}

		/*
		// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
		dwColorBase = RGB( 128, 128, 255 ); // Red (COLOR_GUTTER_SELECTED)
		iBScale = 0; iGScale = 0; iRScale = 7;
		for( lIndex = 0; lIndex < 256; lIndex++ )
		{
			const BYTE bHeightVal = BYTE(lIndex >> 1);

			// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
			const DWORD dwColor = dwColorBase + RGB( bHeightVal >> iBScale, bHeightVal >> iGScale, bHeightVal >> iRScale );
			m_adwSelectedColor[lIndex] = dwColor;
		}

		// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
		dwColorBase = RGB( 128, 192, 255 ); // Orange (COLOR_GUTTER_ACTIVESELECTED)
		iBScale = 0; iGScale = 1; iRScale = 7;
		for( lIndex = 0; lIndex < 256; lIndex++ )
		{
			const BYTE bHeightVal = BYTE(lIndex >> 1);

			// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
			const DWORD dwColor = dwColorBase + RGB( bHeightVal >> iBScale, bHeightVal >> iGScale, bHeightVal >> iRScale );
			m_adwActiveSelectedColor[lIndex] = dwColor;
		}
		*/

		m_fFilledColorArrays = true;
	}
}

CTimelineCtl::~CTimelineCtl()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	StripMgrList*	pSML;
	StripList*		pSL;
	IDMUSProdPropPageObject* pPPO;

	if( m_pCopyDataObject )
	{
		if(S_OK == OleIsCurrentClipboard(m_pCopyDataObject))
		{
			OleFlushClipboard();
		}
		m_pCopyDataObject->Release();
		m_pCopyDataObject = NULL;
	}

	DeleteCriticalSection( &m_csOnDataChanged );

	if( ::InterlockedDecrement( &m_lBitmapRefCount ) == 0 )
	{
		if ( m_BitmapZoomIn.GetSafeHandle() != NULL )
		{
			m_BitmapZoomIn.DeleteObject();
		}
		if ( m_BitmapZoomOut.GetSafeHandle() != NULL )
		{
			m_BitmapZoomOut.DeleteObject();
		}

		if ( m_BitmapMinimize.GetSafeHandle() != NULL )
		{
			m_BitmapMinimize.DeleteObject();
		}
		if ( m_BitmapMinimized.GetSafeHandle() != NULL )
		{
			m_BitmapMinimized.DeleteObject();
		}

		if ( m_BitmapMaximize.GetSafeHandle() != NULL )
		{
			m_BitmapMaximize.DeleteObject();
		}
		if ( m_BitmapMaximized.GetSafeHandle() != NULL )
		{
			m_BitmapMaximized.DeleteObject();
		}
	}

	while( m_pStripMgrList )
	{
		if( m_pStripMgrList->m_pStripMgr )
		{
			// make sure that there's no chance an object is left over
			if( SUCCEEDED( m_pStripMgrList->m_pStripMgr->QueryInterface( 
				IID_IDMUSProdPropPageObject, (void**)&pPPO )))
			{
				RemovePropertyPageObject(pPPO);
				pPPO->Release();
			}
			VARIANT varTimeline;
			varTimeline.vt = VT_UNKNOWN;
			V_UNKNOWN(&varTimeline) = NULL;
			m_pStripMgrList->m_pStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
		}
		pSML = m_pStripMgrList->m_pNext;
		delete m_pStripMgrList;
		m_pStripMgrList = pSML;
	}
	while( m_pStripList )
	{
		if( m_pStripList->m_pStrip )
		{
			// make sure that there's no chance an object is left over
			if( SUCCEEDED( m_pStripList->m_pStrip->QueryInterface( 
				IID_IDMUSProdPropPageObject, (void**)&pPPO )))
			{
				RemovePropertyPageObject(pPPO);
				pPPO->Release();
			}
		}
		pSL = m_pStripList->m_pNext;
		delete m_pStripList;
		m_pStripList = pSL;
	}
	NotifyEntry* pNotifyEntry = NULL;
	while( !m_lstNotifyEntry.IsEmpty() )
	{
		pNotifyEntry = m_lstNotifyEntry.RemoveHead();
		if ( pNotifyEntry != NULL )
		{
			delete pNotifyEntry;
		}
	};
	if( m_pDMUSProdFramework )
	{
		m_pDMUSProdFramework->Release();
	}
	if( m_pIDMPerformance )
	{
		m_pIDMPerformance->Release();
	}
	if( m_pTimelineCallback )
	{
		m_pTimelineCallback->Release();
	}

	m_wnd.Detach();
}

void CTimelineCtl::CallStripWMMessage( StripList* pSL, UINT nMsg, WPARAM wParam,
	LPARAM lParam )
{
	BOOL fFunctionBar = FALSE; // true if mouse is in function bar area

	ASSERT(pSL != NULL);
	if (pSL == NULL)
	{
		return;
	}
	ASSERT(m_pStripList != NULL);
	if (m_pStripList == NULL)
	{
		return;
	}

	long xPos, yPos;
	xPos = LOWORD(lParam);
	yPos = HIWORD(lParam);

	// if X position is negative, make xPos negative
	if( lParam & 0x8000 )
	{
		xPos = xPos | 0xFFFF0000;
	}

	// if Y position is negative, make yPos negative
	if( lParam & 0x80000000 )
	{
		yPos = yPos | 0xFFFF0000;
	}

	switch(nMsg)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_SETCURSOR:
	case WM_CONTEXTMENU:
		{
		// adjust lParam so the cursor position is relative to 0,0 of the strip

			if( xPos < m_lFunctionBarWidth )
			{
				fFunctionBar = TRUE;
			}

			// add horizontal scroll and subtract the function bar and any extras time at the start
			xPos = xPos + m_lXScrollPos - m_lLastEarlyPosition - m_lFunctionBarWidth;

			// subtract away the border
			yPos -= BORDER_HORIZ_WIDTH;
			// if not minized, add any strip vertical scrolling
			if (pSL->m_sv != SV_MINIMIZED)
			{
				yPos += pSL->m_lVScroll;
			}

			// add timeline scroll for all except top strip, which doesn't scroll
			if( pSL != m_pStripList )
			{
				yPos += m_lYScrollPos;
			}

			// subtract the heights of any strips above this one
			for( StripList* pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
			{
				if( pTempSL == pSL )
				{
					break;
				}
				yPos -= ( pTempSL->m_lHeight + BORDER_HORIZ_DRAWWIDTH );
			}
			lParam = MAKELONG( xPos, yPos );
		}
		break;
	default:
		break;
	}
	if( fFunctionBar )
	{
		if( nMsg == WM_LBUTTONDOWN || nMsg == WM_RBUTTONDOWN )
		{
			// Unselect everything
			SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
			SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
		}

		// we're in the function bar area
		IDMUSProdStripFunctionBar* pSFB;
		if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdStripFunctionBar, (void**)&pSFB)))
		{
			pSFB->FBOnWMMessage( nMsg, wParam, lParam, xPos, yPos );
			pSFB->Release();
		}
	}
	else
	{
		if( nMsg == WM_LBUTTONDOWN || nMsg == WM_RBUTTONDOWN )
		{
			// Set the time cursor to where the user clicked
			long lTime;
			PositionToClocks( xPos, &lTime );
			SetTimeCursor( lTime, false, true );
		}
		pSL->m_pStrip->OnWMMessage( nMsg, wParam, lParam, xPos, yPos );
	}
}


long CTimelineCtl::TotalStripHeight(void) const
{
	StripList* pSL;
	long lReturn = 0;

	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		lReturn += pSL->m_lHeight + ( 2 * BORDER_HORIZ_DRAWWIDTH );
	}
	return lReturn;
}

void CTimelineCtl::ComputeScrollBars(void)
{
	RECT rect, rectS;
	long lTemp;
	SCROLLINFO	siH, siV, si;

	// Get the current information for each scrollbar
	m_ScrollVertical.GetScrollInfo( &siV, SIF_ALL );
	m_ScrollHorizontal.GetScrollInfo( &siH, SIF_ALL );

	// compute the number of pixels that show in one window
	m_ScrollVertical.GetClientRect(&rectS);
	GetClientRect(&rect);
	rect.right -= ( rectS.right + m_lFunctionBarWidth + BORDER_VERT_WIDTH );

	// compute the length, in # of pixels
	ClocksToPosition( m_lLength, &lTemp );

	// Check if the pixel length is greater than 0
	if( lTemp > 0 )
	{
		// Determine how much of the window is visible, horizontally
		double dbl = (double)max( rect.right, 0 ) / double(lTemp + m_lLastEarlyPosition + m_lLastLatePosition);
		si.nPage = unsigned int((double)MAX_SCROLL * dbl);
	}
	else
	{
		si.nPage = 0;
	}

	// Fill out the structure to update the horizontal scroll bar with
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = MAX_SCROLL;
	si.nPos = siH.nPos;
	m_ScrollHorizontal.SetScrollInfo( &si, TRUE );

	// Get the rectangle containing the horizontal scroll bar
	m_ScrollHorizontal.GetClientRect(&rectS);

	// Compute the total height of all strips
	lTemp = TotalStripHeight();

	// Check that the total height is greather than 0
	if( lTemp > 0 )
	{
		// Compute the visible portion of the window (subtact off the horizontal scrollbar)
		rect.bottom -= rectS.bottom;

		// Determine how much of the window is visible, vertically
		double dbl = (double)max( rect.bottom, 0 ) / (double)lTemp;
		si.nPage = unsigned int ((double)MAX_SCROLL * dbl);
	}
	else
	{
		si.nPage = 0;
	}

	// Fill out the structure to update the vertical scroll bar with
	si.nPos = siV.nPos;
	m_ScrollVertical.SetScrollInfo( &si, TRUE );

	// set the scroll position of the horizontal scroll bar
	ScrollToPosition( m_lXScrollPos );
}

LRESULT CTimelineCtl::OnCreate(UINT /* nMsg */, WPARAM /* wParam */,
	LPARAM /* lParam */, BOOL& /* lResult */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( !m_wnd.Attach( m_hWndCD ) )
	{
		return -1; // failed to attach window
	}

	// Clip out the scrollbars (and windows the strips create) when we redraw.
	// This gets rid of most of their flicker.
	m_wnd.ModifyStyle( 0, WS_CLIPCHILDREN, 0 );

	CRect rect, rectSB;

	// initialize popup menu's window
	rect.top = 0;
	rect.bottom = 0;
	rect.left = 0;
	rect.right = 0;

	m_MenuHandler.Create( NULL, "Hidden command strip menu handler",
		WS_CHILD, rect, &m_wnd, 0 );
	m_MenuHandler.m_pTimeline = this;

	// create horizontal scroll bar
	m_wnd.GetWindowRect(&rect);
	rect.bottom = rect.bottom - rect.top;
	rect.top = 0;
	rect.right = rect.right - rect.left;
	rect.left = 0;
	m_ScrollHorizontal.Create( SBS_HORZ | SBS_BOTTOMALIGN | WS_CHILD | WS_VISIBLE, rect, &m_wnd, SB_HORIZONTAL );

	// create vertical scroll bar
	m_ScrollHorizontal.GetClientRect( &rectSB );
	rect.bottom -= rectSB.bottom;
	m_ScrollVertical.Create( SBS_VERT | SBS_RIGHTALIGN | WS_CHILD | WS_VISIBLE, rect, &m_wnd, SB_VERTICAL );

	// Load the bitmaps, if necessary
	if( ::InterlockedIncrement( &m_lBitmapRefCount ) == 1 )
	{
		m_BitmapZoomIn.LoadBitmap( IDB_ZOOMIN );
		m_BitmapZoomOut.LoadBitmap( IDB_ZOOMOUT );
		m_BitmapMinimize.LoadOEMBitmap( OBM_REDUCE );
		m_BitmapMinimized.LoadOEMBitmap( OBM_REDUCED );
		m_BitmapMaximize.LoadOEMBitmap( OBM_ZOOM );
		m_BitmapMaximized.LoadOEMBitmap( OBM_ZOOMD );
	}

	// create zoom in and zoom out buttons
	m_ScrollHorizontal.GetClientRect( &rect );
	rect.right = rect.bottom; // make the buttons square
	m_BtnZoomIn.Create( "Z", BS_PUSHBUTTON | BS_BITMAP | WS_CHILD | WS_VISIBLE, rect, &m_wnd, BTN_ZOOMIN );
	m_BtnZoomIn.SetBitmap( m_BitmapZoomIn );
	m_BtnZoomOut.Create( "z", BS_PUSHBUTTON | BS_BITMAP | WS_CHILD | WS_VISIBLE, rect, &m_wnd, BTN_ZOOMOUT );
	m_BtnZoomOut.SetBitmap( m_BitmapZoomOut );

	BITMAP bitmap;
	if( m_BitmapMaximized.GetBitmap( &bitmap ) )
	{
		m_sizeMinMaxButton.cx = bitmap.bmWidth;
		m_sizeMinMaxButton.cy = bitmap.bmHeight;
	}
	// create the time display strip
	IDMUSProdStripMgr*	pSM;
	if( SUCCEEDED( CoCreateInstance( CLSID_TimeStripMgr, NULL, CLSCTX_INPROC_SERVER,
		IID_IDMUSProdStripMgr, (void**)&pSM )))
	{
		AddStripMgr( pSM, 0xffffffff );
		pSM->Release();
	}
	else
	{
		ASSERT(FALSE); // couldn't create time display mgr.
	}
	
	HRESULT hr;
	hr = RegisterDragDrop(m_hWndCD, (IDropTarget *) this);
	ASSERT(SUCCEEDED(hr));

	m_MenuHandler.m_hwndDeleteTracks = GetParent();

	return 0;
}

LRESULT CTimelineCtl::OnDestroy( UINT nMsg, WPARAM /*wParam*/,
		LPARAM /*lParam*/, BOOL& lResult )
{
	UNREFERENCED_PARAMETER( nMsg );
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr;
	hr = RevokeDragDrop(m_hWndCD);
	ASSERT(SUCCEEDED(hr));

	lResult = FALSE;
	return 0;
}

LRESULT CTimelineCtl::OnDrawItem(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& lResult )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UINT	idCtl;
	idCtl = (UINT) wParam;             // control identifier 
	if( idCtl == BTN_ZOOMIN )
	{
		m_BtnZoomIn.SendMessage( nMsg, wParam, lParam );
	}
	else if( idCtl == BTN_ZOOMOUT )
	{
		m_BtnZoomOut.SendMessage( nMsg, wParam, lParam );
	}
	else
	{
		if (m_pStripList)
		{
			CallStripWMMessage( m_pStripList, nMsg, wParam, lParam );
		}
		else
		{
			lResult = FALSE;
		}
	}
 	return 0;
}

LRESULT CTimelineCtl::OnRealTime(WORD /*wNotifyCode*/, WORD wID, 
		HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UNREFERENCED_PARAMETER( wID );
	if (m_pStripList)
	{
		CallStripWMMessage( m_pStripList, BN_CLICKED, BTN_REALTIME, 0 );
	}
 	return 0;
}

LRESULT CTimelineCtl::OnMusicTime(WORD /*wNotifyCode*/, WORD /*wID*/, 
		HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_pStripList)
	{
		CallStripWMMessage( m_pStripList, BN_CLICKED, BTN_MUSICTIME, 0 );
	}
 	return 0;
}

LRESULT CTimelineCtl::OnSize(UINT nMsg, WPARAM /* wParam */,
	LPARAM lParam, BOOL& /* lResult */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	RECT rectSH, rectSV;
	RECT rectZoomIn, rectZoomOut;
	int nWidth = LOWORD(lParam);
	int nHeight = HIWORD(lParam);

	m_ScrollHorizontal.GetClientRect(&rectSH);
	m_ScrollVertical.GetClientRect(&rectSV);
	// move the vertical scroll bar to where it needs to go
	rectSV.bottom = nHeight - rectSH.bottom;
	rectSV.left = nWidth - rectSV.right;
	rectSV.right = nWidth;
	if( m_pStripList )
	{
		rectSV.top += m_pStripList->m_lHeight + BORDER_HORIZ_WIDTH*2; // top of scroll bar begins at 2nd strip down
	}
	m_ScrollVertical.MoveWindow(&rectSV,TRUE);
	// move the horizontal scroll bar and zoom buttons to where they need to go
	m_BtnZoomIn.GetClientRect(&rectZoomIn);
	m_BtnZoomOut.GetClientRect(&rectZoomOut);
	rectSH.top = nHeight - rectSH.bottom;
	rectSH.bottom = nHeight;
	rectSH.right = nWidth - rectZoomIn.right - rectZoomOut.right;
	m_ScrollHorizontal.MoveWindow(&rectSH,TRUE);
	rectZoomIn.left = nWidth - rectZoomIn.right - rectZoomOut.right;
	rectZoomIn.right = nWidth - rectZoomOut.right;
	rectZoomIn.top = nHeight - rectZoomIn.bottom;
	rectZoomIn.bottom = nHeight;
	m_BtnZoomIn.MoveWindow(&rectZoomIn,TRUE);
	rectZoomOut.left = nWidth - rectZoomOut.right;
	rectZoomOut.right = nWidth;
	rectZoomOut.top = nHeight - rectZoomOut.bottom;
	rectZoomOut.bottom = nHeight;
	m_BtnZoomOut.MoveWindow(&rectZoomOut,TRUE);

	// resize the thumb
	ComputeScrollBars();
	int iTemp = 0;
	OnHScroll( 0, MAKELONG( SB_THUMBPOSITION, m_ScrollHorizontal.GetScrollPos() ), (LPARAM) ((HWND) m_ScrollHorizontal), iTemp );
	iTemp = 0;
	OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, m_ScrollVertical.GetScrollPos() ), (LPARAM) ((HWND) m_ScrollVertical), iTemp );

	StripList* pTempSL;
	for( pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
	{
		CallStripWMMessage( pTempSL, nMsg, 0, 0 );
	}
	return 0;
}

void CTimelineCtl::GetStripClientRect( StripList* pSL, LPRECT pRect )
{
	// Display format:
	// BORDER_HORIZ_DRAWWIDTH
	// Strip
	// BORDER_HORIZ_DRAWWIDTH
	// Strip
	// BORDER_HORIZ_DRAWWIDTH
	// Strip
	// BORDER_HORIZ_DRAWWIDTH

	// BORDER_HORIZ_WIDTH == BORDER_HORIZ_DRAWWIDTH

	StripList* pList;
	RECT rect, rectSV, rectSH;
	long lMaxBottom;

	// Get a rect defining our window
	GetClientRect( &rect );
	// Get areas to exclude from the strip's rect
	m_ScrollVertical.GetClientRect( &rectSV );
	m_ScrollHorizontal.GetClientRect( &rectSH );
	// We want to include the function bar in the strip's rect
//	rect.left += m_lFunctionBarWidth;
	// Exclude the vertical scroll bar
	rect.right -= ( BORDER_VERT_WIDTH + rectSV.right );
	// Set a maximum limit on how far down a strip can display so it doesn't overwrite the
	// horizontal scrollbar
	lMaxBottom = rect.bottom - rectSH.bottom;
	// If we're getting the rect for the top strip
	if( pSL == m_pStripList )
	{
		rect.top = BORDER_HORIZ_WIDTH;
		rect.bottom = rect.top + pSL->m_lHeight;
	}
	else
	{
		// Set a minimum limit on how far up a strip can display so it doesn't overwrite the 
		// top strip.
		long lMinTop;
		lMinTop  = BORDER_HORIZ_WIDTH + m_pStripList->m_lHeight + BORDER_HORIZ_WIDTH;
		// Offset everything by the timeline's vertical scroll position
		rect.top = -m_lYScrollPos;
		// Add up the heights of all strips above ours to set rect.top and rect.bottom
		for( pList = m_pStripList; pList; pList = pList->m_pNext )
		{
			rect.bottom = rect.top + pList->m_lHeight + BORDER_HORIZ_WIDTH;
			if( pList == pSL )
			{
				break;
			}
			// The previous's strip bottom is our top, except if the previous strip is the first
			// strip and there is a vertical scroll
			rect.top = rect.bottom;
		}
		rect.top += BORDER_HORIZ_WIDTH;
		// Ensure we don't overwrite the top strip
		if (rect.top < lMinTop)
		{
			rect.top = lMinTop;
		}
	}
	// Ensure we don't overwrite the horizontal scroll bar
	if (rect.bottom > lMaxBottom)
	{
		rect.bottom = lMaxBottom;
	}
	if (rect.top > rect.bottom)
	{
		rect.top = rect.bottom;
	}
	*pRect = rect;
}

void CTimelineCtl::SetMouseMode( long xPos, long yPos)
{
	StripList*	pSL;
	long yOrg = -m_lYScrollPos; // eventually this will need to take into account vertical scrolling
	VARIANT	var;

	// If we're beyond the end of time, set the cusor to MM_NORMAL, NULL m_pMouseStripList and return.
	if( xPos > m_lFunctionBarWidth )
	{
		long lXTime;
		PositionToClocks( xPos - m_lFunctionBarWidth + m_lXScrollPos - m_lLastEarlyPosition - m_lLastLatePosition, &lXTime );

		if( lXTime >= m_lLength )
		{
			m_MouseMode = MM_NORMAL;
			m_pMouseStripList = NULL;
			return;
		}
	}

	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( pSL == m_pStripList )
		{
			// the top strip doesn't scroll
			yOrg += m_lYScrollPos;
		}
		else if( pSL == m_pStripList->m_pNext )
		{
			yOrg -= m_lYScrollPos;
		}
		yOrg += pSL->m_lHeight + BORDER_HORIZ_WIDTH;

	// If the mouse is in the left gutter change to a right facing arrow. Return.
		if( ( xPos <= GUTTER_WIDTH ) && 
			( yPos > yOrg - pSL->m_lHeight - BORDER_HORIZ_WIDTH ) && 
			( yPos < yOrg + BORDER_HORIZ_WIDTH ) )
		{
			m_MouseMode = MM_GUTTER;
			m_pMouseStripList = pSL;
			return;
		}

	// If the mouse is between Strips, ask the top Strip if it can be resized.
	// If it can, change to a resize cursor. Return.
		else if( (yPos <= yOrg + BORDER_HORIZ_WIDTH) && (yPos >= yOrg) )
		{
			m_pMouseStripList = NULL;
			if (pSL->m_sv != SV_MINIMIZED)
			{
				if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_RESIZEABLE, &var )))
				{
					if( ( var.vt == VT_BOOL ) && ( V_BOOL(&var) == TRUE ))
					{
						m_MouseMode = MM_RESIZE;
						m_pMouseStripList = pSL;
						m_lResizeYPos = yOrg;
						m_lResizeOriginalYPos = m_lResizeYPos;
					}
				}
			}
			return;
		}

	// If the mouse is over where the minimize icon would be, ask the strip if it
	// is min/maxable.  If it is, change our state to MM_MINIMIZE. Return.
		else if( (yPos >= yOrg - pSL->m_lHeight ) && (yPos < yOrg - pSL->m_lHeight + m_sizeMinMaxButton.cy) )
		{
			// check for minimize if the strip is MinMaxable and is not already minimized
			VARIANT var;
			if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_MINMAXABLE, &var )))
			{
				if( ( var.vt == VT_BOOL ) && ( V_BOOL(&var) == TRUE ))
				{
					if ((pSL->m_sv == SV_NORMAL) || (pSL->m_sv == SV_MINIMIZED))
					{
						RECT rect, rectSV;
						GetClientRect( &rect );
						m_ScrollVertical.GetClientRect( &rectSV );
						rect.right -= ( BORDER_VERT_WIDTH + rectSV.right );
						long posLength;
						ClocksToPosition( m_lLength, &posLength );
						if( rect.right + m_lXScrollPos > posLength + m_lFunctionBarWidth + m_lLastEarlyPosition)
						{
							rect.right = posLength - m_lXScrollPos + m_lFunctionBarWidth + m_lLastEarlyPosition + MEASURE_LINE_WIDTH;
						}
						if ((xPos < rect.right) && (xPos > rect.right - m_sizeMinMaxButton.cx))
						{
							m_MouseMode = MM_MINMAX;
							m_pMouseStripList = pSL;
							return;
						}
					}
				}
			}
		}

	// if the mouse is inside of a strip, set a pointer to that strip so we
	// can capture it inside OnLButtonDown
		if ( ( yPos > yOrg - pSL->m_lHeight - BORDER_HORIZ_WIDTH ) && 
			( yPos < yOrg + BORDER_HORIZ_WIDTH ) )
		{
			// If the cursor is over the line between the fuction bar and strip, change to a resize cursor
			if ( abs(xPos - (m_lFunctionBarWidth-2)) < 2 )
			{
				m_pMouseStripList = NULL;
				m_MouseMode = MM_RESIZE_FN;
				// resize FunctionBar
				return;
			}

			// Otherwise, set m_pMouseStripList to the strip we're over
			m_pMouseStripList = pSL;
			break; // this break must be here or else the top strip might not get selected
		}
	}

	// Change back to a normal cursor.
	// This shouldn't be blithely overwriting somebody else's cursor because
	// if a strip wants its own cursor, m_MouseMode will be changed back
	// to MM_UNKNOWN in OnSetCursor (and the correct cursor will be
	// displayed there)
	if( m_MouseMode != MM_NORMAL )
	{
		m_MouseMode = MM_NORMAL;
	}

	if( pSL != NULL ) // Found a strip
	{
		if( xPos > m_lFunctionBarWidth )
		{
			VARIANT varTemp;

			// Check if we're in the pick-up bar
			if( xPos - m_lFunctionBarWidth < max( 0, m_lLastEarlyPosition - m_lXScrollPos ) )
			{
				// Check if the strip supports pick-up measures
				if( FAILED( pSL->m_pStrip->GetStripProperty( SP_EARLY_TIME, &varTemp ) ) )
				{
					// No - treat it as if the mouse is below all strips
					m_pMouseStripList = NULL;
					m_MouseMode = MM_NORMAL;
					return;
				}

				// Check if we're before the strip's pick-up measure
				long lEarlyPosition;
				ClocksToPosition( V_I4(&varTemp), &lEarlyPosition );
				if( xPos - m_lFunctionBarWidth < max( 0, m_lLastEarlyPosition - lEarlyPosition - m_lXScrollPos ) )
				{
					// Before the strip's pick-up bar - treat it as if the mouse is below all strips
					m_pMouseStripList = NULL;
					m_MouseMode = MM_NORMAL;
					return;
				}
			}
			// Check if we're in the extension bar
			else
			{
				long lTimelineLength;
				ClocksToPosition( m_lLength, &lTimelineLength );

				if( xPos - m_lFunctionBarWidth + m_lXScrollPos - m_lLastEarlyPosition >= lTimelineLength )
				{
					// Check if the strip supports extension measures
					if( FAILED( pSL->m_pStrip->GetStripProperty( SP_LATE_TIME, &varTemp ) ) )
					{
						// No - treat it as if the mouse is below all strips
						m_pMouseStripList = NULL;
						m_MouseMode = MM_NORMAL;
						return;
					}

					// Check if we're after the strip's extension measure
					long lLatePosition;
					ClocksToPosition( V_I4(&varTemp), &lLatePosition );
					if( xPos - m_lFunctionBarWidth + m_lXScrollPos - m_lLastEarlyPosition >= lLatePosition + lTimelineLength )
					{
						// After the strip's extension bar - treat it as if the mouse is below all strips
						m_pMouseStripList = NULL;
						m_MouseMode = MM_NORMAL;
						return;
					}
				}
			}
		}
	}
	else //if ( pSL == NULL ) // mouse is below all strips
	{
		m_pMouseStripList = NULL;
		m_MouseMode = MM_NORMAL;
	}
}

LRESULT CTimelineCtl::OnMouseMove(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& /* lResult */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	long yPos = HIWORD(lParam);
	long xPos = LOWORD(lParam);
	long yOrg = -m_lYScrollPos; // eventually this will need to take into account vertical scrolling
	long lResizeYOffset;
	RECT rect;

	// This message may belong to the Timeline or one of the Strips.

	// If a Strip has the mouse capture, send this message to the Strip. Return.
	if( m_fStripMouseCapture && m_pMouseStripList )
	{
		CallStripWMMessage( m_pMouseStripList, nMsg, wParam, lParam );
		return 0;
	}

	// if X position is negative, make xPos negative
	if( lParam & 0x8000 )
	{
		xPos = xPos | 0xFFFF0000;
	}

	// Compute which Strip is below the cursor.
	// If we're resizing a Strip we need to redraw the splitter line. Return.
	if( m_MouseMode == MM_ACTIVERESIZE )
	{
		if( yPos > 0xFF00 || !m_pMouseStripList )
		{
			return 0; // this is assuredly the mouse pointer going above the top of the window
		}
 
		VARIANT var;
		long lMinStripHeight = MIN_STRIP_HEIGHT;
		if( SUCCEEDED( m_pMouseStripList->m_pStrip->GetStripProperty( SP_MINHEIGHT, &var )))
		{
			lMinStripHeight = V_INT(&var);
		}

		// don't allow resize to go smaller than the minimum strip height
		if( yPos < m_lResizeOriginalYPos + lMinStripHeight - m_pMouseStripList->m_lHeight )
		{
			lResizeYOffset = m_lResizeOriginalYPos + lMinStripHeight
				- m_pMouseStripList->m_lHeight - m_lResizeYPos;
		}
		else
		{
			// don't allow resize to go greater than the maximum strip height
			if( SUCCEEDED( m_pMouseStripList->m_pStrip->GetStripProperty( SP_MAXHEIGHT, &var )))
			{
				if( yPos > m_lResizeOriginalYPos + V_INT(&var) - m_pMouseStripList->m_lHeight )
				{
					lResizeYOffset = m_lResizeOriginalYPos + V_INT(&var)
						- m_pMouseStripList->m_lHeight - m_lResizeYPos;
				}
				else
				{
					lResizeYOffset = yPos - m_lResizeYPos;
				}
			}
			else
			{
				lResizeYOffset = yPos - m_lResizeYPos;
			}
		}
		// Invalidate rect to erase old xor line and draw new one
		if( lResizeYOffset )
		{
			RECT rectSV;
			GetClientRect( &rect );
			m_ScrollVertical.GetClientRect( &rectSV );
			// invalidate area to draw xor line
			rect.right -= rectSV.right;
			rect.left = LEFT_DRAW_OFFSET;
			rect.top = m_lResizeYPos;
			rect.bottom = m_lResizeYPos + BORDER_HORIZ_WIDTH;
//			InvalidateRect( &rect, TRUE );
			InvalidateRect( &rect, FALSE );
			m_lResizeYPos += lResizeYOffset;
			rect.top = m_lResizeYPos;
			rect.bottom = m_lResizeYPos + BORDER_HORIZ_WIDTH;
//			InvalidateRect( &rect, TRUE );
			InvalidateRect( &rect, FALSE );
		}
		return 0;
	}
	// If we're resizing the function bar we need to redraw the splitter line. Return.
	else if( m_MouseMode == MM_ACTIVERESIZE_FN )
	{
		// don't allow resize to go smaller than the minimum function bar
		// width or larger than the maximum function bar widht
		if(( xPos >= MIN_FNBAR_WIDTH ) && ( xPos <= MAX_FNBAR_WIDTH))
		{
			m_lFunctionBarWidth = xPos;

			// Probably should calculate the area that will be affected
			// and only invalidate it.
			// Perhaps we could just 'scroll' the affected area to the
			// right and invalidate the small new area to be displayed

			// send WM_SIZE to all strips, letting them know something has changed size
			StripList* pSL = m_pStripList;
			while (pSL)
			{
				// BUGBUG: Should have meaningful values for lParam.
				CallStripWMMessage( pSL, WM_SIZE, SIZE_RESTORED, MAKELONG(0, pSL->m_lHeight) );
				pSL = pSL->m_pNext;
			}

			ComputeScrollBars();
			GetClientRect( &rect );
//			InvalidateRect( &rect, TRUE );
			InvalidateRect( &rect, FALSE );
		}
		return 0;
	}
	// if we've clicked in a gutter, set whichever strip we're on to m_fActiveGutterState.
	else if( m_MouseMode == MM_ACTIVEGUTTER )
	{
		SetMouseMode(xPos,yPos);
		if(m_pMouseStripList)
		{
			GetStripClientRect( m_pMouseStripList, &rect );
			yOrg = rect.bottom;
			if( ( xPos <= GUTTER_WIDTH ) && 
				( yPos > yOrg - m_pMouseStripList->m_lHeight - BORDER_HORIZ_WIDTH ) && 
				( yPos < yOrg + BORDER_HORIZ_WIDTH ) )
			{
				SetStripGutter( m_pMouseStripList, m_fActiveGutterState );
			}
		}

		// Reset m_MouseMode
		m_MouseMode = MM_ACTIVEGUTTER;
		return 0;
	}
	// If we're moving a Strip we need to see if we're over a different
	// Strip than before, and if so we need to redraw the splitter line
	// which appears across the lower border of the Strip where the Strip
	// we are dragging will go. (I.e. when we click a Strip's drag bar,
	// the bottom border of the Strip above the current Strip highlights.) Return.
	// If we've clicked on the minimize/maximize button, ignore all events until the
	// mouse button is released.
	else if( m_MouseMode == MM_ACTIVEMINMAX )
	{
		return 0;
	}

	SetMouseMode(xPos,yPos);
	// Then, send this message to the Strip beneath the cursor, if there is one.
	// Pass the message to the Strip below the cursor.
	if( m_pMouseStripList )
	{
		CallStripWMMessage( m_pMouseStripList, nMsg, wParam, lParam );
	}
	return 0;
}

LRESULT CTimelineCtl::OnMouseWheel(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& /* lResult */)
{
	/*short fwKeys = LOWORD(wParam);
	short zDelta = HIWORD(wParam);
	short xPos = LOWORD(lParam);
	short yPos = HIWORD(lParam);*/
		
	if( m_pMouseStripList )
	{
		CallStripWMMessage( m_pMouseStripList, nMsg, wParam, lParam );
	}
	return 0;
}

bool IsStripInPatternTrack( IDMUSProdStrip *pStrip )
{
	bool fResult = false;
	if( pStrip )
	{
		VARIANT varStripMgr;
		if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) )
		&&	V_UNKNOWN(&varStripMgr) )
		{
			// Get an IDMUSProdStripMgr interface
			IDMUSProdStripMgr *pIStripMgr;
			if( SUCCEEDED( V_UNKNOWN(&varStripMgr)->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pIStripMgr ) ) )
			{
				DMUS_IO_TRACK_HEADER ioTrackHeader;
				ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
				VARIANT varTrackHeader;
				varTrackHeader.vt = VT_BYREF;
				V_BYREF(&varTrackHeader) = &ioTrackHeader;
				if( SUCCEEDED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
				{
					if( ioTrackHeader.guidClassID == CLSID_DirectMusicPatternTrack )
					{
						fResult = true;
					}
				}
				pIStripMgr->Release();

			}
			V_UNKNOWN(&varStripMgr)->Release();
		}
	}

	return fResult;
}

LRESULT CTimelineCtl::OnKeyDown(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& /* lResult */)
{
	if( m_pTimeStrip )
	{
		long lNewCursorPos = LONG_MIN;
		switch( wParam )
		{
		case VK_INSERT:
		case VK_DELETE:
			// If the Ctrl key is down, but the shift and alt keys are up
			if( (0x8000 & GetKeyState( VK_CONTROL ))
			&&	!(0x8000 & GetKeyState( VK_SHIFT ))
			&&	!(0x8000 & GetKeyState( VK_MENU )) )
			{
				WPARAM wNewParam = MAKELONG( wParam == VK_DELETE ? ID_EDIT_DELETE_TRACK : ID_EDIT_ADD_TRACK, 1 );
				::SendMessage( GetParent(), WM_COMMAND, wNewParam, 0 );
				return 0;
			}
			break;
		case VK_HOME:
			lNewCursorPos = 0;
			break;
		case VK_END:
			lNewCursorPos = m_lLength - 1;
			break;
		case VK_LEFT:
		case VK_RIGHT:
			if( (m_pActiveStripList == NULL)
			||	!IsStripInPatternTrack(m_pActiveStripList->m_pStrip) )
			{
				// If the Ctrl key is down
				if( GetKeyState( VK_CONTROL ) & 0x8000 )
				{
					// Move by a measures
					if( BumpTimeCursor( wParam == VK_RIGHT, DMUSPROD_TIMELINE_SNAP_BAR ) )
					{
						return 0;
					}
				}
				else
				{
					// Otherwise, move by the snap-to setting
					if( BumpTimeCursor( wParam == VK_RIGHT, m_pTimeStrip->m_stSetting ) )
					{
						return 0;
					}
				}
			}
			break;
		}

		if( lNewCursorPos != LONG_MIN )
		{
			SetTimeCursor( min( lNewCursorPos, m_lLength - 1), m_pTimeStrip->m_fScrollWhenSettingTimeCursor, true );
			return 0;
		}
	}

	if( m_pActiveStripList )
	{
		CallStripWMMessage( m_pActiveStripList, nMsg, wParam, lParam );
	}
	return 0;
}

LRESULT CTimelineCtl::OnChar(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& /* lResult */)
{
	static COLORREF acrCustomColors[16];
	static COLORREF crGreyColor = RGB( SHADING_DARK_COLOR, SHADING_DARK_COLOR, SHADING_DARK_COLOR );
	static COLORREF crYellowColor = RGB( 255, 255, 192 );

	if( m_strZoomInChars.Find( (TCHAR)wParam, 0 ) >= 0 )
	{
		BOOL bReturn;
		OnZoomIn( 0, 0, 0, bReturn );
		return 0;
	}
	else if( m_strZoomOutChars.Find( (TCHAR)wParam, 0 ) >= 0 )
	{
		BOOL bReturn;
		OnZoomOut( 0, 0, 0, bReturn );
		return 0;
	}
	else if( (wParam == 'g') || (wParam == 'G') )
	{
		CHOOSECOLOR cc;

		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = m_hWnd;
		cc.hInstance = NULL;
		cc.rgbResult = crGreyColor; // initial color
		cc.lpCustColors = acrCustomColors;
		cc.Flags = CC_ANYCOLOR | CC_RGBINIT;
		cc.lCustData = NULL;
		cc.lpfnHook = NULL;
		cc.lpTemplateName = NULL;

		HWND hwndFocus = ::GetFocus();
		if (ChooseColor( &cc ))
		{
			// User chose a color
			crGreyColor = cc.rgbResult;

			// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
			DWORD dwColorBase = RGB( GetBValue(crGreyColor), GetGValue(crGreyColor), GetRValue(crGreyColor) );
			int iBScale = 255 - GetBValue(crGreyColor);
			int iGScale = 255 - GetGValue(crGreyColor);
			int iRScale = 255 - GetRValue(crGreyColor);
			for( long lIndex = 0; lIndex < 256; lIndex++ )
			{
				// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
				const DWORD dwColor = dwColorBase + RGB( (iBScale * lIndex) / 255, (iGScale * lIndex) / 255, (iRScale * lIndex) / 255 );
				m_adwNormalColor[lIndex] = dwColor;
			}

			Invalidate( FALSE );
		}
		if( ::GetFocus() != hwndFocus )
		{
			::SetFocus( hwndFocus );
		}
	}
	else if( (wParam == 'y') || (wParam == 'Y') )
	{
		CHOOSECOLOR cc;

		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = m_hWnd;
		cc.hInstance = NULL;
		cc.rgbResult = crYellowColor; // initial color
		cc.lpCustColors = acrCustomColors;
		cc.Flags = CC_ANYCOLOR | CC_RGBINIT;
		cc.lCustData = NULL;
		cc.lpfnHook = NULL;
		cc.lpTemplateName = NULL;

		HWND hwndFocus = ::GetFocus();
		if (ChooseColor( &cc ))
		{
			// User chose a color
			crYellowColor = cc.rgbResult;

			// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
			DWORD dwColorBase = RGB( GetBValue(crYellowColor), GetGValue(crYellowColor), GetRValue(crYellowColor) );
			int iBScale = 255 - GetBValue(crYellowColor);
			int iGScale = 255 - GetGValue(crYellowColor);
			int iRScale = 255 - GetRValue(crYellowColor);
			for( long lIndex = 0; lIndex < 256; lIndex++ )
			{
				// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
				const DWORD dwColor = dwColorBase + RGB( (iBScale * lIndex) / 255, (iGScale * lIndex) / 255, (iRScale * lIndex) / 255 );
				m_adwActiveColor[lIndex] = dwColor;
			}

			Invalidate( FALSE );
		}
		if( ::GetFocus() != hwndFocus )
		{
			::SetFocus( hwndFocus );
		}
	}
	else if( m_pTimeStrip )
	{
		switch (wParam)
		{
		case 2: // Ctrl-B
			// Snap to bar
			m_pTimeStrip->SetSnapTo( DMUSPROD_TIMELINE_SNAP_BAR );
			return 0;
			break;
		case 5: // Ctrl-E
		// Snap to none
			m_pTimeStrip->SetSnapTo( DMUSPROD_TIMELINE_SNAP_NONE );
			return 0;
			break;
		case 7: // Ctrl-G
		// Snap to grid
			m_pTimeStrip->SetSnapTo( DMUSPROD_TIMELINE_SNAP_GRID );
			return 0;
			break;
		case 20: // Ctrl-T
		// Snap to beat
			m_pTimeStrip->SetSnapTo( DMUSPROD_TIMELINE_SNAP_BEAT );
			return 0;
			break;
		}
	}

	if( m_pActiveStripList )
	{
		CallStripWMMessage( m_pActiveStripList, nMsg, wParam, lParam );
	}
	return 0;
}

POINT CTimelineCtl::GetMousePoint() const
{
	POINT point;
	::GetCursorPos( &point );
	ScreenToClient( &point );
	return point;
}

LRESULT CTimelineCtl::OnSetCursor(UINT nMsg, WPARAM wParam,
	LPARAM /*lParam*/, BOOL& /* lResult */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	POINT point;
	point = GetMousePoint(); // to get the point in our window that the cursor is at
	SetMouseMode( point.x, point.y ); // to set m_pMouseStripList

	if( m_pMouseStripList && ((m_MouseMode==MM_UNKNOWN) || (m_MouseMode==MM_NORMAL)) )
	{
		LPARAM lparam = MAKELONG( point.x, point.y );

		CallStripWMMessage( m_pMouseStripList, nMsg, wParam, lparam );

		// ask the Strip what it wants its cursor to be
		VARIANT var;
		if( SUCCEEDED( m_pMouseStripList->m_pStrip->GetStripProperty( SP_CURSOR_HANDLE, &var )))
		{
			HCURSOR hStripCursor;
			hStripCursor = (HCURSOR) V_I4(&var);
			if ( hStripCursor ) {
				SetCursor( hStripCursor);
				m_MouseMode = MM_UNKNOWN;
			}
			else 
				m_MouseMode = MM_NORMAL; // Null pointer -> use normal cursor
		}
	}
	// if a strip is not using its own cursor, and a strip does not have
	// capture, then set our own cursor
	if ((m_MouseMode != MM_UNKNOWN) && !(m_fStripMouseCapture && m_pMouseStripList))
	{
		switch(m_MouseMode) {
		case MM_NORMAL:
		case MM_MINMAX:
			{
				static HCURSOR hCursorArrow;
				if (!hCursorArrow)
					hCursorArrow = LoadCursor( NULL, IDC_ARROW );
				if( hCursorArrow )
					SetCursor( hCursorArrow );
			}
			break;
		case MM_ACTIVEGUTTER:
		case MM_GUTTER:
			{
				static HCURSOR hCursorRightArrow;
				if (!hCursorRightArrow)
					hCursorRightArrow = LoadCursor( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDC_RIGHTARROW) );
				if( hCursorRightArrow )
					SetCursor( hCursorRightArrow );
			}
			break;
		case MM_ACTIVERESIZE:
		case MM_RESIZE:
			{
				static HCURSOR hCursorSizeNS;
				if (!hCursorSizeNS)
					hCursorSizeNS = LoadCursor( NULL, IDC_SIZENS ); // North/south resize
				if( hCursorSizeNS )
					SetCursor( hCursorSizeNS );
			}
			break;
		case MM_ACTIVERESIZE_FN:
		case MM_RESIZE_FN:
			{
				static HCURSOR hCursorSizeWE;
				if (!hCursorSizeWE)
					hCursorSizeWE = LoadCursor( NULL, IDC_SIZEWE ); // West/east resize
				if( hCursorSizeWE )
					SetCursor( hCursorSizeWE );
			}
			break;
		default:
			break;
	}
	}
	
	return 0;
}

void CTimelineCtl::SetStripGutter( StripList* pSL, BOOL fSelect )
{
	ASSERT( pSL );

	// Check if the selection state would change
	if( pSL->m_fSelected != fSelect )
	{
		// Yes - Check if this strip support gutter selection
		VARIANT var;
		if( SUCCEEDED( m_pMouseStripList->m_pStrip->GetStripProperty( SP_GUTTERSELECTABLE, &var )))
		{
			if( ( var.vt == VT_BOOL ) && ( V_BOOL(&var) == TRUE ))
			{
				// This code is duplicated in StripSetTimelineProperty, with STP_GUTTER_SELECTED
				// Yep - change it's selection
				pSL->m_fSelected = fSelect;

				// Update the gutter display
				RECT rect;
				GetStripClientRect( pSL, &rect );
				rect.left = 0;
				rect.right = rect.left + GUTTER_WIDTH;
				InvalidateRect( &rect, FALSE );

				// Notify the strip
				var.vt = VT_BOOL;
				V_BOOL(&var) = (short)m_pMouseStripList->m_fSelected;
				m_pMouseStripList->m_pStrip->SetStripProperty( SP_GUTTERSELECT, var );
			}
		}
	}
}

void CTimelineCtl::DeactivateStripList(void)
{
	RECT rect;

	if( m_pActiveStripList )
	{
		GetStripClientRect( m_pActiveStripList, &rect );
		//rect.left = 0;
		//rect.right = rect.left + GUTTER_WIDTH;
		InvalidateRect( &rect, FALSE );
		StripList* pTempSL = m_pActiveStripList;
		m_pActiveStripList = NULL;
		CallStripWMMessage( pTempSL, WM_KILLFOCUS, 0, 0 );
	}
}

void CTimelineCtl::ActivateStripList( StripList* pSL )
{
	if( pSL != m_pActiveStripList )
	{
		RECT rect;
//		long lMaxHeight;

		if( m_pActiveStripList )
		{
			DeactivateStripList();
		}
		m_pActiveStripList = pSL;
		GetStripClientRect( pSL, &rect );
		//rect.left = 0;
		//rect.right = rect.left + GUTTER_WIDTH;
		InvalidateRect( &rect, FALSE );
		CallStripWMMessage( m_pActiveStripList, WM_SETFOCUS, 0, 0 );
	}
}

// the following is mostly copied from OnLButtonDown
LRESULT CTimelineCtl::OnLButtonDblclk(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& /* lResult */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the Strip has mouse capture, send it on.

	// If there are any other buttons down, we don't care about
	// this message, so pass it along to the Strip below the cursor. Return.

	// If we are a gutter cursor:
	if( m_MouseMode == MM_GUTTER )
	{
		m_MouseMode = MM_ACTIVEGUTTER;
		SetCapture();
		if( m_pMouseStripList )
		{
			// invalidate the rect to the left of m_pMouseStripList and toggle
			// m_pMouseStripList's selection.
			if(m_pMouseStripList->m_fSelected)
			{
				m_fActiveGutterState = FALSE;
			}
			else
			{
				m_fActiveGutterState = TRUE;
			}
			SetStripGutter( m_pMouseStripList, m_fActiveGutterState );
		}
		return 0;
	}

	if( (m_MouseMode != MM_MINMAX) && (m_pMouseStripList != NULL) )
	{
		VARIANT var;
		if (SUCCEEDED (m_pMouseStripList->m_pStrip->GetStripProperty( SP_MINMAXABLE, &var)) &&
			(var.vt == VT_BOOL) && (V_BOOL(&var) == TRUE) )
		{
			if (m_pMouseStripList->m_sv == SV_MINIMIZED)
			{
				m_pMouseStripList->m_lHeight = m_pMouseStripList->m_lRestoreHeight;
				m_pMouseStripList->m_sv = SV_NORMAL;

				RECT rectWin, rect;
				GetClientRect( &rectWin );
				GetStripClientRect( m_pMouseStripList, &rect );
				rectWin.top = rect.top - BORDER_HORIZ_WIDTH;
//				InvalidateRect( &rectWin );
				InvalidateRect( &rectWin, FALSE );

				ComputeScrollBars();
				// If, as a result of the strip resize, the total height of the strips is less than the
				// height of the window, scroll back to the top.
				if ( (TotalStripHeight() < rectWin.bottom - rectWin.top) && (m_lYScrollPos != 0) )
				{
					int iTemp = 0;
					OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, 0 ), (LPARAM) ((HWND) m_ScrollVertical), iTemp );
				}
				// Tell this strip that it's been resized
				CallStripWMMessage( m_pMouseStripList, WM_SIZE, SIZE_MAXIMIZED, MAKELONG(0, m_pMouseStripList->m_lHeight) );

				// Tell all the strips below this one that they've moved.
				StripList* pTempSL;
				BOOL fDeleted = TRUE;
				for( pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
				{
					// Make sure that this strip hasn't been removed..
					if( pTempSL == m_pMouseStripList )
					{
						for( pTempSL = m_pMouseStripList->m_pNext; pTempSL; pTempSL = pTempSL->m_pNext )
						{
							CallStripWMMessage( pTempSL, WM_MOVE, 0, 0 );
						}
						fDeleted = FALSE;
						break;
					}
				}
				if( fDeleted )
				{
					// If it was deleted, send WM_MOVE to all strips because we don't know which strips to update anymore.
					for( pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
					{
						CallStripWMMessage( pTempSL, WM_MOVE, 0, 0 );
					}
				}
				return 0;
			}
			else if (m_pMouseStripList->m_sv == SV_NORMAL)
			{
				// Get a rectangle defining the size of the strip
				RECT rectStrip;
				GetStripClientRect( m_pMouseStripList, &rectStrip );

				 // Get the point in our window that the cursor is at
				POINT point;
				point = GetMousePoint();

				// Check if the point is vertically where the function name would be
				if( point.y < rectStrip.top + FUNCTION_NAME_HEIGHT )
				{
					int nExcludePixels = 0;
					VARIANT var;
					if( SUCCEEDED( m_pMouseStripList->m_pStrip->GetStripProperty( SP_FUNCTIONBAR_EXCLUDE_WIDTH, &var ) ) 
					&&	(var.vt == VT_I4) )
					{
						nExcludePixels = V_I4( &var );
					}

					// Check if the point is horizontally where the function name would be
					if( (point.x >= GUTTER_WIDTH + BORDER_VERT_DRAWWIDTH) &&
						(point.x < m_lFunctionBarWidth - nExcludePixels - BORDER_VERT_WIDTH) )
					{
						// Save the current height and change the strip's state to minimized
						m_pMouseStripList->m_lRestoreHeight = m_pMouseStripList->m_lHeight;
						m_pMouseStripList->m_sv = SV_MINIMIZED;

						// Get the desired minimized height - if the strip doesn't have a preference,
						// use the default MIN_STRIP_HEIGHT
						if (SUCCEEDED (m_pMouseStripList->m_pStrip->GetStripProperty( SP_MINIMIZE_HEIGHT, &var)))
						{
							m_pMouseStripList->m_lHeight = V_INT(&var);
						}
						else
						{
							m_pMouseStripList->m_lHeight = MIN_STRIP_HEIGHT;
						}

						// Get the rectangle defining the Timeline window
						RECT rectWin;
						GetClientRect( &rectWin );

						// This gets the new rectangle, based on the strip's new height
						GetStripClientRect( m_pMouseStripList, &rectStrip );

						// Refresh the window from the top of the changed strip on down.
						rectWin.top = rectStrip.top - BORDER_HORIZ_WIDTH;
						InvalidateRect( &rectWin, FALSE );

						ComputeScrollBars();

						// If, as a result of the strip resize, the total height of the strips is less than the
						// height of the window, scroll back to the top.
						// The can happen if the strip's maximized size is smaller than its minimized size
						if ( (TotalStripHeight() < rectWin.bottom - rectWin.top) && (m_lYScrollPos != 0) )
						{
							int iTemp = 0;
							OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, 0 ), (LPARAM) ((HWND) m_ScrollVertical), iTemp );
						}

						// Tell this strip that it's been resized
						CallStripWMMessage( m_pMouseStripList, WM_SIZE, SIZE_MINIMIZED, MAKELONG(0, m_pMouseStripList->m_lHeight) );

						// Tell all the strips below this one that they've moved.
						StripList* pTempSL;
						BOOL fDeleted = TRUE;
						for( pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
						{
							// Make sure that this strip hasn't been removed..
							// Curve strips remove themselves when the minimized.  There may be some
							// strip that removes itself (is replaced by another strip) when it is
							// maximized.
							if( pTempSL == m_pMouseStripList )
							{
								for( pTempSL = m_pMouseStripList->m_pNext; pTempSL; pTempSL = pTempSL->m_pNext )
								{
									CallStripWMMessage( pTempSL, WM_MOVE, 0, 0 );
								}
								fDeleted = FALSE;
								break;
							}
						}
						if( fDeleted )
						{
							// If it was deleted, send WM_MOVE to all strips because we don't know which strips to update anymore.
							for( pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
							{
								CallStripWMMessage( pTempSL, WM_MOVE, 0, 0 );
							}
						}
						return 0;
					}
				}
			}
		}
	}

	// Pass the message to the Strip below the cursor.
	if( m_pMouseStripList )
	{
		// first, make this is the active strip if it's not already
		ActivateStripList( m_pMouseStripList );
		CallStripWMMessage( m_pMouseStripList, nMsg, wParam, lParam );
	}
	return 0;		
}

/* Using this breaks selection for sequence, pattern, var switch, and curve strips
static bool fSetMarkerTime_Ignore_m_pMouseStripList = false;
*/

LRESULT CTimelineCtl::OnLButtonDown(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& /* lResult */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	RECT rect;
	SetFocus();

	// If the Strip has mouse capture, send it on.

	// If there are any other buttons down, we don't care about
	// this message, so pass it along to the Strip below the cursor. Return.

	// If we are a resize cursor, start the resize operation. Return.
	if( m_MouseMode == MM_RESIZE )
	{
		m_MouseMode = MM_ACTIVERESIZE;
		// capture the mouse so we don't lose the mouse button up
		SetCapture();
		// invalidate area to draw xor line
		GetClientRect( &rect );
		rect.top = m_lResizeYPos;
		rect.bottom = m_lResizeYPos + BORDER_HORIZ_WIDTH;
//		InvalidateRect( &rect, TRUE );
		InvalidateRect( &rect, FALSE );
		return 0;
	}
	// If we are a function bar resize cursor, start the resize operation. Return.
	if( m_MouseMode == MM_RESIZE_FN )
	{
		m_MouseMode = MM_ACTIVERESIZE_FN;
		// capture the mouse so we don't lose the mouse button up
		SetCapture();
		return 0;
	}
	// If we are a gutter cursor:
	if( m_MouseMode == MM_GUTTER )
	{
		m_MouseMode = MM_ACTIVEGUTTER;
		SetCapture();
		if( m_pMouseStripList )
		{
			// invalidate the rect to the left of m_pMouseStripList and toggle
			// m_pMouseStripList's selection.
			if(m_pMouseStripList->m_fSelected)
			{
				m_fActiveGutterState = FALSE;
			}
			else
			{
				m_fActiveGutterState = TRUE;
			}
			SetStripGutter( m_pMouseStripList, m_fActiveGutterState );
		}
		return 0;
	}
		// if the shift key is pressed, add the Strip and all Strips between the
		// Strip and the last chosen Strip (inclusive) to the local active Strip list
		// Otherwise, put only the Strip beneath the cursor in the local active Strip list
		// if the control key is pressed add the local Strip list to the global one.
		// Otherwise, replace the global active Strip list with the local one.
		// Return.
	
	// If we are over the minimize/maximize button:
	if( m_MouseMode == MM_MINMAX )
	{
		m_MouseMode = MM_ACTIVEMINMAX;
		HDC hdc = GetDC();
		if (hdc && m_pMouseStripList)
		{
				RECT rect;
				GetStripClientRect( m_pMouseStripList, &rect );
				long posLength;
				ClocksToPosition( m_lLength, &posLength );
				posLength -= m_lXScrollPos;
				if( rect.right > posLength + m_lFunctionBarWidth + m_lLastEarlyPosition )
				{
					rect.right = posLength + m_lFunctionBarWidth + m_lLastEarlyPosition + MEASURE_LINE_WIDTH;
				}
				if (m_pMouseStripList->m_sv == SV_NORMAL)
				{
					//dc.DrawState(CPoint( rect.right - m_sizeMinMaxButton.cx, rect.top ), m_sizeMinMaxButton, &m_BitmapMinimized, DSS_NORMAL);
					::DrawState( hdc, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapMinimized.GetSafeHandle() ), NULL,
						rect.right - m_sizeMinMaxButton.cx, rect.top, m_sizeMinMaxButton.cx, m_sizeMinMaxButton.cy,
						DST_BITMAP| DSS_NORMAL );
				}
				else if (m_pMouseStripList->m_sv == SV_MINIMIZED)
				{
					//dc.DrawState(CPoint( rect.right - m_sizeMinMaxButton.cx, rect.top ), m_sizeMinMaxButton, &m_BitmapMaximized, DSS_NORMAL);
					::DrawState( hdc, NULL, NULL, reinterpret_cast<LPARAM>( m_BitmapMaximized.GetSafeHandle() ), NULL,
						rect.right - m_sizeMinMaxButton.cx, rect.top, m_sizeMinMaxButton.cx, m_sizeMinMaxButton.cy,
						DST_BITMAP| DSS_NORMAL );
				}
		}

		// Release the DC
		::ReleaseDC(m_hWnd, hdc);

		// capture the mouse so we don't lose the mouse button up
		SetCapture();
		return 0;
	}

	// Pass the message to the Strip below the cursor.
	if( m_pMouseStripList )
	{
		// first, make this the active strip if it's not already
		ActivateStripList( m_pMouseStripList );

		/* Using this breaks selection for sequence, pattern, var switch, and curve strips
		fSetMarkerTime_Ignore_m_pMouseStripList = true;
		SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
		SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, 0 );
		fSetMarkerTime_Ignore_m_pMouseStripList = false;
		*/

		CallStripWMMessage( m_pMouseStripList, nMsg, wParam, lParam );
	}
	return 0;		
}

LRESULT CTimelineCtl::OnRButtonDown(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& /* lResult */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	SetFocus();

	// Pass the message to the Strip below the cursor.
	if( m_pMouseStripList )
	{
		// first, make this the active strip if it's not already
		ActivateStripList( m_pMouseStripList );
		CallStripWMMessage( m_pMouseStripList, nMsg, wParam, lParam );
	}
	return 0;		
}

// this function makes it so things are compressed more.
LRESULT CTimelineCtl::OnZoomOut(WORD /*wNotifyCode*/, WORD /*wID*/, 
		HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// See how many pixels a measure displays in
	// BUGBUG: hardcoded at 4 quarter notes
	long pos;
	ClocksToPosition( DMUS_PPQ * 4, &pos );

	if( pos > MIN_MEASURE_PIXELS ) // if we're trying to view an entire measure in under 10 pixels, forget it
	{
		// Compute the pixel position of the cursor
		long lOffset;
		ClocksToPosition( m_lCursor, &lOffset );

		// Offset it so we have the number of pixels from the left edge of the strip display
		lOffset -= m_lXScrollPos - m_lLastEarlyPosition;

		// Change the zoom level
		m_dblZoom *= .8;

		// Compute pixel position of the cursor
		long lNewCursorPos;
		ClocksToPosition( m_lCursor, &lNewCursorPos );

		// Scroll so the cursor position is constant
		ScrollToPosition( lNewCursorPos - lOffset );
		m_lXScrollPos = lNewCursorPos - lOffset;

		//ComputeXScrollPos();
		ComputeScrollBars();
		InvalidateRect(NULL, FALSE);
	}

	// Set focus back to the timeline (away from the zoom out button)
	SetFocus();
	return 0;
}

// this function makes it so things are spread out more.
LRESULT CTimelineCtl::OnZoomIn(WORD /*wNotifyCode*/, WORD /*wID*/, 
		HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check to see how many pixels a measure is displayed in
	// BUGBUG: hardcoded at four quarter notes
	long pos;
	ClocksToPosition( DMUS_PPQ * 4, &pos );
	if( pos < MAX_MEASURE_PIXELS ) // a measure maxes out at 2000 pixels
	{
		// Compute the pixel position of the cursor
		long lOffset;
		ClocksToPosition( m_lCursor, &lOffset );

		// Offset it so we have the number of pixels from the left edge of the strip display
		lOffset -= m_lXScrollPos - m_lLastEarlyPosition;

		// Change the zoom level
		m_dblZoom /= .8;

		// Compute pixel position of the cursor
		long lNewCursorPos;
		ClocksToPosition( m_lCursor, &lNewCursorPos );

		// Scroll so the cursor position is constant
		ScrollToPosition( lNewCursorPos - lOffset );
		m_lXScrollPos = lNewCursorPos - lOffset;

		//ComputeXScrollPos( lCenterPos );
		ComputeScrollBars();
		InvalidateRect(NULL, FALSE);
	}

	// Set focus back to the timeline (away from the zoom in button)
	SetFocus();
	return 0;
}

// This just tells Windows that we don't need it to erase the background for us.
// This is OK because we erase the background in OnDrawAdvanced().
LRESULT CTimelineCtl::OnEraseBkgnd(UINT /* nMsg */, WPARAM /* wParam */,
		LPARAM /* lParam */, BOOL& /* lResult */)
{
	return FALSE;
}

LRESULT CTimelineCtl::OnContextMenu(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& /* lResult */)
{
	if( m_pMouseStripList )
	{
		CallStripWMMessage( m_pMouseStripList, nMsg, wParam, lParam );
	}
	return 0;
}

LRESULT CTimelineCtl::OnLButtonUp(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& lResult )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	RECT	rectStrip, rectWin;

	ReleaseCapture();
	m_fStripMouseCapture = FALSE;
	// if we have been resizing a strip, complete the resize operation.
	if( m_MouseMode == MM_ACTIVERESIZE )
	{
		m_MouseMode = MM_RESIZE;
		if( !m_pMouseStripList )
		{
			return 0;
		}
		StripList*	pSL;
		pSL = m_pMouseStripList;
		pSL->m_lHeight += (m_lResizeYPos - m_lResizeOriginalYPos);
		// send WM_SIZE to the strip, letting it know it has changed size
		// BUGBUG: Should have meaningful values for lParam.
		CallStripWMMessage( pSL, WM_SIZE, SIZE_RESTORED, MAKELONG(0, pSL->m_lHeight) );
		ComputeDrawingArea( &rectWin );
		GetStripClientRect( pSL, &rectStrip );
		rectStrip.top -= BORDER_HORIZ_WIDTH;
		rectStrip.bottom = rectWin.bottom;
		rectStrip.right = rectWin.right + BORDER_VERT_WIDTH;
		rectStrip.left = 0;
//		InvalidateRect( &rectWin );
		InvalidateRect( &rectStrip, FALSE );
		ComputeScrollBars();
		// If, as a result of the strip resize, the total height of the strips is less than the
		// height of the window, scroll back to the top.
		if ( (TotalStripHeight() < rectWin.bottom - rectWin.top) && (m_lYScrollPos != 0) )
		{
			int iTemp = 0;
			OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, 0 ), (LPARAM) ((HWND) m_ScrollVertical), iTemp );
		}
		// Tell all the strips below this one that they've moved.
		StripList* pTempSL;
		for( pTempSL = m_pMouseStripList->m_pNext; pTempSL; pTempSL = pTempSL->m_pNext )
		{
			CallStripWMMessage( pTempSL, WM_MOVE, 0, 0 );
		}
	}
	else if( m_MouseMode == MM_ACTIVERESIZE_FN )
	{
		//StripList*	pSL;
		m_MouseMode = MM_RESIZE_FN;
		/*pSL = m_pMouseStripList;
		pSL->m_lHeight += (m_lResizeYPos - m_lResizeOriginalYPos);
		GetClientRect( &rectWin );
		GetStripClientRect( pSL, &rectStrip );
		rectWin.top = rectStrip.top - BORDER_HORIZ_WIDTH;
//		InvalidateRect( &rectWin );
		InvalidateRect( &rectWin, FALSE );
		ComputeScrollBars();*/
	}
	else if( m_MouseMode == MM_ACTIVEGUTTER )
	{
		m_MouseMode = MM_GUTTER;
	}
	else if( m_MouseMode == MM_ACTIVEMINMAX )
	{
		m_MouseMode = MM_MINMAX;
		if( !m_pMouseStripList )
		{
			return 0;
		}

		// Get the rectangle defining the Timeline window
		GetClientRect( &rectWin );

		// toggle a strips's minimize state
		if (m_pMouseStripList->m_sv == SV_MINIMIZED)
		{
			m_pMouseStripList->m_lHeight = m_pMouseStripList->m_lRestoreHeight;
			m_pMouseStripList->m_sv = SV_NORMAL;
			GetStripClientRect( m_pMouseStripList, &rectStrip );
			rectWin.top = rectStrip.top - BORDER_HORIZ_WIDTH;
			InvalidateRect( &rectWin, FALSE );
			ComputeScrollBars();
			// If, as a result of the strip resize, the total height of the strips is less than the
			// height of the window, scroll back to the top.
			if ( (TotalStripHeight() < rectWin.bottom - rectWin.top) && (m_lYScrollPos != 0) )
			{
				int iTemp = 0;
				OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, 0 ), (LPARAM) ((HWND) m_ScrollVertical), iTemp );
			}
			// Tell this strip that it's been resized
			CallStripWMMessage( m_pMouseStripList, WM_SIZE, SIZE_MAXIMIZED, MAKELONG(0, m_pMouseStripList->m_lHeight) );
		}
		else if (m_pMouseStripList->m_sv == SV_NORMAL)
		{
			m_pMouseStripList->m_lRestoreHeight = m_pMouseStripList->m_lHeight;
			m_pMouseStripList->m_sv = SV_MINIMIZED;
			VARIANT var;
			if (SUCCEEDED (m_pMouseStripList->m_pStrip->GetStripProperty( SP_MINIMIZE_HEIGHT, &var)))
			{
				m_pMouseStripList->m_lHeight = V_INT(&var);
			}
			else
			{
				m_pMouseStripList->m_lHeight = MIN_STRIP_HEIGHT;
			}

			// This gets the new rectangle, based on the strip's new height
			GetStripClientRect( m_pMouseStripList, &rectStrip );

			// Refresh the window from the top of the changed strip on down.
			rectWin.top = rectStrip.top - BORDER_HORIZ_WIDTH;
			InvalidateRect( &rectWin, FALSE );

			ComputeScrollBars();

			// If, as a result of the strip resize, the total height of the strips is less than the
			// height of the window, scroll back to the top.
			// The can happen if the strip's maximized size is smaller than its minimized size
			if ( (TotalStripHeight() < rectWin.bottom - rectWin.top) && (m_lYScrollPos != 0) )
			{
				int iTemp = 0;
				OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, 0 ), (LPARAM) ((HWND) m_ScrollVertical), iTemp );
			}

			// Tell this strip that it's been resized
			CallStripWMMessage( m_pMouseStripList, WM_SIZE, SIZE_MINIMIZED, MAKELONG(0, m_pMouseStripList->m_lHeight) );
		}
		// Tell all the strips below this one that they've moved.
		StripList* pTempSL;
		BOOL fDeleted = TRUE;
		for( pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
		{
			// Make sure that this strip hasn't been removed..
			if( pTempSL == m_pMouseStripList )
			{
				for( pTempSL = m_pMouseStripList->m_pNext; pTempSL; pTempSL = pTempSL->m_pNext )
				{
					CallStripWMMessage( pTempSL, WM_MOVE, 0, 0 );
				}
				fDeleted = FALSE;
				break;
			}
		}
		if( fDeleted )
		{
			// If it was deleted, send WM_MOVE to all strips because we don't know which strips to update anymore.
			for( pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
			{
				CallStripWMMessage( pTempSL, WM_MOVE, 0, 0 );
			}
		}
	}
	else
	{
		// Pass the message to the Strip below the cursor.
		if( m_pMouseStripList )
		{
			CallStripWMMessage( m_pMouseStripList, nMsg, wParam, lParam );
			OnMouseMove( WM_MOUSEMOVE, wParam, lParam, lResult);
		}
	}
	return 0;
}

LRESULT CTimelineCtl::OnRButtonUp(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& /* lResult */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	// This was copied from OnLButtonUp.  Currently all it does is pass the message
	// to the strip which is currently receiving mouse messages.
	// WM_RBUTTONDOWN does not currently capture the mouse, but it doesn't hurt to 
	// try and release the mouse capture

	ReleaseCapture();
	m_fStripMouseCapture = FALSE;

	// Pass the message to the Strip below the cursor.
	if( m_pMouseStripList )
	{
		CallStripWMMessage( m_pMouseStripList, nMsg, wParam, lParam );
	}
	else if( SUCCEEDED( GetParam( GUID_Segment_DisplayContextMenu, 0xFFFFFFFF, 0, 0, NULL, &lParam ) ) )
	{
		// Display a right-click context menu.

		// Get the cursor position (To put the menu there)
		POINT pt;
		if( GetCursorPos( &pt ) )
		{
			// Display the menu
			TrackPopupMenu(NULL, pt.x, pt.y, NULL, TRUE);
		}
	}
	return 0;
}

void CTimelineCtl::ComputeDrawingArea( LPRECT pRect ) const
{
	// Get the Timeline's client rectangle
	GetClientRect( pRect );

	// Get the client rectangles for the scrollbars
	RECT rectSH, rectSV;
	m_ScrollHorizontal.GetClientRect( &rectSH );
	m_ScrollVertical.GetClientRect( &rectSV );

	// offset the left edge to mask out the functionbar
	pRect->left += m_lFunctionBarWidth;

	// check that there is some horizontal viewing space
	ASSERT( pRect->right > BORDER_VERT_WIDTH + rectSV.right );

	// Mask off the vertical scrollbar and vertical border
	pRect->right -= ( BORDER_VERT_WIDTH + rectSV.right );

	// Maxk off the horizontal scrollbar, and ensure the visible area isn't negative
	pRect->bottom = max( 0, pRect->bottom - rectSH.bottom );
}

long CTimelineCtl::PositionToXScroll( long lPos )
{
	// Compute the drawing rectangle of the Timeline (strips off scrollbars and function bar)
	RECT rect;
	ComputeDrawingArea( &rect );

	// Compute the number of pixels in length the Timeline is
	long lPosLength;
	ClocksToPosition( m_lLength, &lPosLength );

	// Add on the early and late amounts, then subtract off the number of visible pixels
	lPosLength += m_lLastEarlyPosition + m_lLastLatePosition - (rect.right - rect.left);

	// Check if there's any need to scroll
	if(lPosLength > 0)
	{
		// Convert from a pixel position to a % of MAX_SCROLL
		return ( lPos * MAX_SCROLL ) / lPosLength;
	}
	else
	{
		return 0;
	}
}

LRESULT CTimelineCtl::OnHScroll(UINT /* nMsg */, WPARAM wParam,
	LPARAM /* lParam */, BOOL& /* lResult */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int nScrollCode = (int) LOWORD(wParam);  // scroll bar value 
	long nPos = m_ScrollHorizontal.GetScrollPos();   // scroll box position
	long nScrollAmount = 0;
	RECT rect;
	long lMaxScroll;

	// Compute the strip drawing area
	ComputeDrawingArea( &rect );

	// Compute how much to scroll (offset stored in nScrollAmount or position in nPos)
	switch( nScrollCode )
	{
	case SB_LEFT:
	case SB_RIGHT:
		break;
	case SB_LINELEFT:
		// BUGBUG: hardcoded at one quarter note
		ClocksToPosition( DMUS_PPQ, &nScrollAmount);
		nScrollAmount = min( -1, -PositionToXScroll(nScrollAmount) );
		break;
	case SB_LINERIGHT:
		// BUGBUG: hardcoded at one quarter note
		ClocksToPosition( DMUS_PPQ, &nScrollAmount);
		nScrollAmount = max( 1, PositionToXScroll(nScrollAmount) );
		break;
	case SB_PAGELEFT:
		nScrollAmount = -( PositionToXScroll(rect.right - rect.left) * 3 / 4 );
		break;
	case SB_PAGERIGHT:
		nScrollAmount = PositionToXScroll(rect.right - rect.left) * 3 / 4;
		break;
	case SB_THUMBTRACK:
		m_fHScrollTracking = TRUE;
		nPos = (short int) HIWORD(wParam);
		break;
	case SB_THUMBPOSITION:
		m_fHScrollTracking = FALSE;
		nPos = (short int) HIWORD(wParam);
		break;
	case SB_ENDSCROLL:
	default:
		return 1;
	}

	// Get the maximum scroll position
	lMaxScroll = m_ScrollHorizontal.GetScrollLimit();

	// Update nPos, ensuring we don't go beyond the maximum scroll value or below zero
	nPos = max( 0, min( lMaxScroll, nPos + nScrollAmount ) );

	// Update the scrollbar position
	m_ScrollHorizontal.SetScrollPos(nPos);

	// Compute the new X scroll position
	long lNewXScrollPos = ComputeXScrollPos();

	// Check if the X scroll position actually changed
	if( m_lXScrollPos != lNewXScrollPos )
	{
		// Yes - update it
		m_lXScrollPos = lNewXScrollPos;

		// send WM_HSCROLL to all strips, letting them know they have scrolled
		StripList* pSL = m_pStripList;
		while (pSL)
		{
			// BUGBUG: Should have meaningful values for wParam and lParam.
			CallStripWMMessage( pSL, WM_HSCROLL, 0 /*wParam*/, 0 /*lParam*/ );
			pSL = pSL->m_pNext;
		}

		// Redraw the entire Timeline
		InvalidateRect(&rect, FALSE);
	}

	// This is commented out because it looks very bad when we're usin overlayed icons,
	// sunch as the min/max button or the variation choices buttons
	/*
	ScrollWindow( lOriginalPos - m_lXScrollPos, 0, &rect, &rect );

	StripList* pSL;
	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		VARIANT var;
		if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_MINMAXABLE, &var )))
		{
			if( ( var.vt == VT_BOOL ) && ( V_BOOL(&var) == TRUE ))
			{
				if ((pSL->m_sv == SV_NORMAL) || (pSL->m_sv == SV_MINIMIZED))
				{
					RECT rectStrip;
					GetStripClientRect(pSL, &rectStrip);
					long posLength;
					ClocksToPosition( m_lLength, &posLength );
					posLength += m_lFunctionBarWidth; 
					if( (posLength - m_lXScrollPos > rectStrip.right) ||
						(posLength - lOriginalPos > rectStrip.right))
					{
						// invalidate where button will go
						rectStrip.left = rectStrip.right - 16;
						rectStrip.bottom = rectStrip.top + 14;
						InvalidateRect( &rectStrip, FALSE);
					}
					if (lOriginalPos < m_lXScrollPos)
					{
						// invalidate where button used to be
						rectStrip.left += lOriginalPos - m_lXScrollPos;
						rectStrip.right = rectStrip.left + 16;
//						InvalidateRect( &rectStrip, TRUE);
						InvalidateRect( &rectStrip, FALSE);
					}
				}
			}
		}
	}
	*/
	return 0;
}

LRESULT CTimelineCtl::OnVScroll(UINT nMsg, WPARAM wParam,
	LPARAM lParam, BOOL& /* lResult */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HWND hwndScrollBar = (HWND)lParam;

	if( hwndScrollBar == m_ScrollVertical )
	{
		int nScrollCode = (int) LOWORD(wParam);  // scroll bar value 
		long nPos;
		RECT rect;
		RECT rectSH, rectSV;
		long lMaxBarScroll, lMaxStripScroll;//,lScrollAmount
		nPos = m_ScrollVertical.GetScrollPos();
		GetClientRect( &rect );
		m_ScrollHorizontal.GetClientRect( &rectSH );
		m_ScrollVertical.GetClientRect( &rectSV );
		rect.right -= rectSV.right;
		rect.bottom -= rectSH.bottom;
		if( m_pStripList )
		{
			// don't scroll top strip (usually the time strip)
			rect.top += m_pStripList->m_lHeight + BORDER_HORIZ_WIDTH*2;
		}
		
		// Get the maximum scroll bar thumb position
		lMaxBarScroll = m_ScrollVertical.GetScrollLimit();
		
		// Get the height of all strips
		lMaxStripScroll = TotalStripHeight();
		// subtract off the top strip
		if ( m_pStripList )
		{
			lMaxStripScroll -= m_pStripList->m_lHeight + BORDER_HORIZ_WIDTH;
		}
		// subtract the height of the viewing region
		lMaxStripScroll -= rect.bottom - rect.top;

		if (lMaxStripScroll < 0)
		{
			lMaxStripScroll = 0;
		}
		//lScrollAmount = m_lYScrollPos;

		if (lMaxStripScroll > 0)
		{
			switch( nScrollCode )
			{
			case SB_TOP:
				break;
			case SB_LINEDOWN:
				nPos = lMaxBarScroll * ( m_lYScrollPos + (long)(20)) / lMaxStripScroll;
				break;
			case SB_LINEUP:
				nPos = lMaxBarScroll * ( m_lYScrollPos - (long)(20)) / lMaxStripScroll;
				break;
			case SB_PAGEDOWN:
				nPos = lMaxBarScroll * ( m_lYScrollPos + (long)(rect.bottom - rect.top)) / lMaxStripScroll;
				break;
			case SB_PAGEUP:
				nPos = lMaxBarScroll * ( m_lYScrollPos - (long)(rect.bottom - rect.top)) / lMaxStripScroll;
				break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				nPos = (short int) HIWORD(wParam);
				break;
			case SB_BOTTOM:
				break;
			case SB_ENDSCROLL:
			default:
				return 1;
			}
		}
		if( nPos < 0 ) nPos = 0;
		if( nPos > lMaxBarScroll ) nPos = (short)lMaxBarScroll;
		m_ScrollVertical.SetScrollPos(nPos);

		long lNewYScrollPos;
		if (lMaxBarScroll > 0)
		{
			lNewYScrollPos = nPos * lMaxStripScroll / lMaxBarScroll;
		}
		else
		{
			lNewYScrollPos = 0;
		}

		if( lNewYScrollPos != m_lYScrollPos )
		{
			m_lYScrollPos = lNewYScrollPos;

			// send WM_VSCROLL to all strips, letting them know they have scrolled
			StripList* pSL = m_pStripList;
			while (pSL)
			{
				// BUGBUG: Should have meaningful values for wParam and lParam.
				CallStripWMMessage( pSL, WM_VSCROLL, 0 /*wParam*/, 0 /*lParam*/ );
				pSL = pSL->m_pNext;
			}

			// By invalidating ourself, we get rid of the flicker of the minimize buttons.
			InvalidateRect(&rect, FALSE);

			// This causes the minimize buttons to flicker
			//lScrollAmount -= m_lYScrollPos;
			//ScrollWindow( 0, lScrollAmount, &rect, &rect );
		}
	}
	else
	{
		::SendMessage( hwndScrollBar, nMsg, wParam, lParam );
	}
	return FALSE; // Handled
}

long CTimelineCtl::ComputeXScrollPos()
{
	long lPos = m_ScrollHorizontal.GetScrollPos();
	long lMaxScroll;

	lMaxScroll = m_ScrollHorizontal.GetScrollLimit();

	// Compute the drawing rectangle of the Timeline (strips off scrollbars and function bar)
	RECT rect;
	ComputeDrawingArea( &rect );

	// Compute the number of pixels in length the Timeline is
	long lPosLength;
	ClocksToPosition( m_lLength, &lPosLength );

	// Add on the early and late amounts, then subtract off the number of visible pixels
	lPosLength += m_lLastEarlyPosition + m_lLastLatePosition - (rect.right - rect.left);

	// lPos / lMaxScroll = % of way scrolled
	// lPosLength - width of drawing rectangle = range we can scroll

	// Check if there's any need to scroll
	double dbl = 0;
	if( lMaxScroll > 0 )
	{
		// Convert from a % of lMaxScroll to a pixel position
		dbl = ((double) lPos / (double) lMaxScroll) * double(lPosLength);
	}
	
	if( dbl <= 0 )
	{
		return 0; // don't allow it to scroll left past the beginning
	}

	return (long)( dbl + 0.5);
}

void CTimelineCtl::ScrollToPosition( long lPos )
{
	long lScrollPos;
	double	dbl;
	long lMaxScroll;

	lMaxScroll = m_ScrollHorizontal.GetScrollLimit();

	// Compute the drawing rectangle of the Timeline (strips off scrollbars and function bar)
	RECT rect;
	ComputeDrawingArea( &rect );

	// Compute the number of pixels in length the Timeline is
	long lPosLength;
	ClocksToPosition( m_lLength, &lPosLength );

	// Add on the early and late amounts, then subtract off the number of visible pixels
	lPosLength += m_lLastEarlyPosition + m_lLastLatePosition - (rect.right - rect.left);

	// lPosLength - width of drawing rectangle = range we can scroll
	// lPos * range we can scroll = % of way scrolled
	// Multiply this % by lMaxScroll to get the thumb position

	// Check if there's any need to scroll
	if(lPosLength > 0)
	{
		// Convert from a pixel position to a % of lMaxScroll
		dbl = (double(lPos) * double(lMaxScroll)) / double (lPosLength);

		lScrollPos = long( dbl + 0.5 );
		if (lScrollPos < 0)
		{
			lScrollPos = 0;
		}
	}
	else
	{
		lScrollPos = 0;
	}

	int iTemp = 0;
	OnHScroll( 0, MAKELONG( SB_THUMBPOSITION, lScrollPos ), 0, iTemp );
}

HRESULT CTimelineCtl::OnDrawAdvanced(ATL_DRAWINFO& di)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	long lEarlyPosition, lLatePosition;
	ComputeEarlyAndLateTime( lEarlyPosition, m_lLastLateTime );
	ClocksToPosition( lEarlyPosition, &lEarlyPosition );
	ClocksToPosition( m_lLastLateTime, &lLatePosition );

	if( lEarlyPosition != m_lLastEarlyPosition )
	{
		m_lXScrollPos = max( 0, m_lXScrollPos + lEarlyPosition - m_lLastEarlyPosition);
		m_lLastEarlyPosition = lEarlyPosition;
		if( lLatePosition != m_lLastLatePosition )
		{
			m_lXScrollPos = min( m_lLength + lLatePosition + lEarlyPosition, m_lXScrollPos );
			m_lLastLatePosition = lLatePosition;
		}
		InvalidateRect(NULL, FALSE);
		ComputeScrollBars();
		return S_OK;
	}

	if( lLatePosition != m_lLastLatePosition )
	{
		m_lXScrollPos = min( m_lLength + lLatePosition + lEarlyPosition, m_lXScrollPos );
		m_lLastLatePosition = lLatePosition;
		InvalidateRect(NULL, FALSE);
		ComputeScrollBars();
		return S_OK;
	}

	CRect rc(*(RECT*)di.prcBounds);
	CDC realDC;
	if( realDC.Attach( di.hdcDraw ) )
	{
//////////////////////////////////////////////////////////////////////////////////
		CBitmap		bmBlank;
		CRect		rcClientArea;
		
		realDC.GetClipBox(&rcClientArea);
		//GetClientRect(&rcClientArea);
		
		BOOL bStatus = bmBlank.CreateCompatibleBitmap(&realDC, rcClientArea.right, rcClientArea.bottom);
		//BOOL bStatus = bmBlank.CreateCompatibleBitmap(&realDC, rcClientArea.right - rcClientArea.left, rcClientArea.bottom - rcClientArea.top);
		if (!bStatus)
		{
			realDC.Detach();
			return E_FAIL;
		}
		
		// create a memory dc so that we can draw by doing one blt to the screen.
		CDC dc;
		bStatus = dc.CreateCompatibleDC(&realDC);
		if (!bStatus)
		{
			realDC.Detach();
			return E_FAIL;
		}
		
		//select an empty bitmap into the dc. 
		CBitmap* pOldbm = dc.SelectObject(&bmBlank);
		if (!pOldbm)
		{
			dc.DeleteDC();
			realDC.Detach();
			return E_FAIL;
		}

		CRgn* pRgn = new CRgn;
		if (!pRgn)
		{
			dc.DeleteDC();
			realDC.Detach();
			return E_FAIL;
		}
		pRgn->CreateRectRgnIndirect( rcClientArea );
		dc.SelectClipRgn( pRgn );
		
		// draw the background. 
		dc.FillSolidRect(rcClientArea, GetSysColor(COLOR_WINDOW));

		// Get the name of the font to use for the strip names
		CString strFontName;
		if( !strFontName.LoadString(IDS_DRAGBAR_FONTNAME) )
		{
			strFontName = "Times New Roman";
		}

		LOGFONT lf;
		ZeroMemory( &lf, sizeof(LOGFONT));
		lf.lfHeight = FUNCTION_NAME_HEIGHT;
		//lf.lfWidth = 0;
		//lf.lfEscapement = 0;
		//lf.lfOrientation = 0;
		lf.lfWeight = 700;
		//lf.lfItalic = FALSE;
		//lf.lfUnderline = FALSE;
		//lf.lfStrikeOut = FALSE;
		//lf.lfCharSet = ANSI_CHARSET;
		//lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		//lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		//lf.lfQuality = DEFAULT_QUALITY;
		//lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		int nLength = min( strFontName.GetLength(), 31 );
		memcpy( lf.lfFaceName, (LPCTSTR)strFontName, nLength );
		
		CFont fontStripName;
		if ( !fontStripName.CreateFontIndirect( &lf ) )
		{
			dc.DeleteDC();
			realDC.Detach();
			return E_FAIL;
		}

		lf.lfHeight = STRIP_NAME_HEIGHT;

		CFont fontStrip;
		if( !fontStrip.CreateFontIndirect( &lf ) )
		{
			fontStripName.DeleteObject();
			dc.DeleteDC();
			realDC.Detach();
			return E_FAIL;
		}

		CPen penHorizFnBar;
		if( !penHorizFnBar.CreatePen( PS_SOLID, BORDER_HORIZ_DRAWWIDTH, dc.GetNearestColor( BORDER_COLOR ) ) )
		{
			fontStrip.DeleteObject();
			fontStripName.DeleteObject();
			dc.DeleteDC();
			realDC.Detach();
			return E_FAIL;
		}

		CPen penVertFnBar;
		if( !penVertFnBar.CreatePen( PS_SOLID, BORDER_VERT_DRAWWIDTH, dc.GetNearestColor( BORDER_COLOR ) ) )
		{
			penHorizFnBar.DeleteObject();
			fontStrip.DeleteObject();
			fontStripName.DeleteObject();
			dc.DeleteDC();
			realDC.Detach();
			return E_FAIL;
		}

//////////////////////////////////////////////////////////////////////////////////
		// call each Strip's Draw routine
		StripList*	pSL;
		int			iSavedDC;
		CRect		rectClip, rectSV, rectSH;
		CRgn		rgn;
		CBrush		brushBorder;
		long		lRightBounds = di.prcBounds->right;
		COLORREF prevColor;

		long xOrgForNonPickupStrips;
		xOrgForNonPickupStrips = m_lFunctionBarWidth + max( 0, lEarlyPosition - m_lXScrollPos );

		long lXScrollOffset;
		int xOrg, yOrg, yFirst = 0, yTemp;
		int yEnd, iClipRgn;
		CRect tempRect;

		xOrg = m_lFunctionBarWidth;
		yOrg = BORDER_HORIZ_WIDTH;
		lXScrollOffset = m_lXScrollPos;
		brushBorder.CreateSolidBrush( dc.GetNearestColor( BORDER_COLOR ) );
		m_ScrollVertical.GetClientRect( &rectSV );
		m_ScrollHorizontal.GetClientRect( &rectSH );
		lRightBounds -= rectSV.right; // clip the vert scroll bar off the drawing area
		if( m_pStripList )
		{
			// color the rectangle to the right of the first strip and above the
			// vertical scrollbar grey
			tempRect.top = 0;
			tempRect.bottom = m_pStripList->m_lHeight + BORDER_HORIZ_WIDTH*2;
			tempRect.left = lRightBounds;
			tempRect.right = di.prcBounds->right;
			if( tempRect.IntersectRect( tempRect, rcClientArea ) )
			{
				prevColor = dc.GetBkColor();
				dc.FillSolidRect( &tempRect, dc.GetNearestColor(COLOR_GUTTER_NORMAL));
				dc.SetBkColor(prevColor);
			}
		}

		CFont *pOldStripFont;
		pOldStripFont = dc.SelectObject( &fontStrip );

		// Calculate the length (in pixels) of the timeline
		long posLength;
		ClocksToPosition( m_lLength, &posLength );
		posLength += lEarlyPosition + lLatePosition;

		for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
		{
			VARIANT	varTemp;

			if( pSL == m_pStripList->m_pNext )
			{
				// the first strip (usually time strip) always plots at the top.
				// the rest are scrolled.
				yFirst = yOrg;
				yOrg -= m_lYScrollPos;
			}
			if( pSL == m_pStripList )
			{
				yTemp = yOrg;
			}
			else
			{
				yTemp = ( yOrg < yFirst ) ? yFirst : yOrg;
			}

			// Save the Device Context
			iSavedDC = dc.SaveDC();

			// prevent strips from drawing on top of vert scroll bar
			long xEnd = lRightBounds - BORDER_VERT_WIDTH;

			// clip the xEnd so it doesn't go past the length
			long lStripLatePosition = 0;
			if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_LATE_TIME, &varTemp ) ) )
			{
				// Convert from clocks to pixels
				ClocksToPosition( V_I4(&varTemp), &lStripLatePosition );

				xEnd = min( xEnd, posLength - lLatePosition - lXScrollOffset + lStripLatePosition + m_lFunctionBarWidth + BORDER_VERT_DRAWWIDTH - 1 );
			}
			else
			{
				xEnd = min( xEnd, posLength - lLatePosition - lXScrollOffset + m_lFunctionBarWidth + BORDER_VERT_DRAWWIDTH - 1 );
			}

			yEnd = yOrg + pSL->m_lHeight;
			// Clip yEnd to the bottom of the viewable area, minus the height of
			// the horizontal scroll bar
			if( yEnd > di.prcBounds->bottom - rectSH.bottom )
			{
				yEnd = di.prcBounds->bottom - rectSH.bottom;
			}

			// Draw the normal gutter
			tempRect.left = 0;
			tempRect.right = GUTTER_WIDTH;
			tempRect.top = yOrg - BORDER_HORIZ_WIDTH;
			tempRect.bottom = yEnd + BORDER_HORIZ_WIDTH;

			// Ensure we don't overwrite the top strip.
			// If we're not the top strip
			if( pSL != m_pStripList )
			{
				rgn.CreateRectRgn( rcClientArea.left, yTemp-1, rcClientArea.right, rcClientArea.bottom );
				dc.SelectClipRgn( &rgn );
				rgn.DeleteObject();
			}

			if( (dc.GetDeviceCaps( RASTERCAPS ) & RC_DIBTODEV)
			&&	(min( rcClientArea.right, xEnd ) > max( rcClientArea.left, GUTTER_WIDTH - 1 ))
			&&	(pSL->m_lHeight > 0) )
			{
				// Color the entire strip - not just the gutter
				// For DIBs, RGB() is actually BGR(), so we need to switch the order of the values
				DWORD *pdwColorArray = m_adwNormalColor;
				/*
				if( pSL->m_fSelected )
				{
					if( m_pActiveStripList == pSL )
					{
						// Orange (COLOR_GUTTER_ACTIVESELECTED)
						pdwColorArray = m_adwActiveSelectedColor;
					}
					else
					{
						// Red (COLOR_GUTTER_SELECTED)
						pdwColorArray = m_adwSelectedColor;
					}
				}
				else */	if( m_pActiveStripList == pSL )
				{
					// Yellow (COLOR_GUTTER_ACTIVE)
					pdwColorArray = m_adwActiveColor;
				}

				{
					const long lLeftEdge = max( rcClientArea.left, GUTTER_WIDTH - 1 );
					const long lRightEdge = min( rcClientArea.right, xEnd );

					BITMAPINFO bmInfo;
					ZeroMemory( &bmInfo, sizeof( BITMAPINFO ) );
					//bmInfo.bmiColors = ;
					bmInfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
					bmInfo.bmiHeader.biWidth = min( SHADING_BLOCK_WIDTH, lRightEdge - lLeftEdge );
					bmInfo.bmiHeader.biHeight = -pSL->m_lHeight; // Negative, to make a top-down DIB
					bmInfo.bmiHeader.biPlanes = 1;
					bmInfo.bmiHeader.biBitCount = 32;
					bmInfo.bmiHeader.biClrImportant = 0;
					bmInfo.bmiHeader.biClrUsed = 0;
					bmInfo.bmiHeader.biCompression = BI_RGB;
					bmInfo.bmiHeader.biSizeImage = 0;
					bmInfo.bmiHeader.biXPelsPerMeter = 0;
					bmInfo.bmiHeader.biYPelsPerMeter = 0;
					const long lDIBWidth = bmInfo.bmiHeader.biWidth;
					const long lDIBHeight = pSL->m_lHeight;
					DWORD *adwDIBArray = new DWORD[lDIBWidth * lDIBHeight];
					DWORD *pdwIndex = adwDIBArray;

					for( long lDIBY = 0; lDIBY < lDIBHeight; lDIBY++ )
					{
						/*
						const BYTE bVal = SHADING_DARK_COLOR + (lDIBY << 6) / lDIBHeight;
						memset( pdwIndex, bVal, sizeof(DWORD) * lDIBWidth );
						pdwIndex += lDIBWidth;
						*/

						// Color the entire strip - not just the gutter
						const DWORD dwColor = pdwColorArray[ (lDIBY << 8)/ lDIBHeight ];
						for( long lDIBX = 0; lDIBX < lDIBWidth; lDIBX++ )
						{
							*pdwIndex = dwColor;
							pdwIndex++;
						}
					}

					::SetDIBitsToDevice( dc.m_hDC,
										 lLeftEdge, yOrg,
										 lDIBWidth, lDIBHeight,
										 0, 0,
										 0, lDIBHeight,
										 adwDIBArray, &bmInfo, DIB_RGB_COLORS );

					if( lRightEdge - lLeftEdge > SHADING_BLOCK_WIDTH )
					{
						for( long lOffset = SHADING_BLOCK_WIDTH; lOffset < lRightEdge - lLeftEdge; lOffset += SHADING_BLOCK_WIDTH )
						{
							const long lTmpWidth = min( lDIBWidth, lRightEdge - (lLeftEdge + lOffset));
							::BitBlt( dc.m_hDC,
									  lLeftEdge + lOffset, yOrg,
									  lTmpWidth, lDIBHeight,
									  dc.m_hDC,
									  lLeftEdge, yOrg,
									  SRCCOPY );
						}
					}

					//delete [] abDIBArray;
					delete [] adwDIBArray;
				}
			}

			// Create yet another temporary rect so we don't destroy the gutter rectangle
			CRect reallyTempRect;
			if( reallyTempRect.IntersectRect( tempRect, rcClientArea ) )
			{
				// Save the background color
				prevColor = dc.GetBkColor();

				// Choose the gutter's color
				COLORREF gutterColor;
				/*
				if( pSL == m_pActiveStripList )
				{
					if( pSL->m_fSelected )
					{
						gutterColor = dc.GetNearestColor(COLOR_GUTTER_ACTIVESELECTED);
					}
					else
					{
						gutterColor = dc.GetNearestColor(COLOR_GUTTER_ACTIVE);
					}
				}
				else */ if( pSL->m_fSelected )
				{
					gutterColor = dc.GetNearestColor(COLOR_GUTTER_SELECTED);
				}
				else
				{
					gutterColor = dc.GetNearestColor(COLOR_GUTTER_NORMAL);
				}

				// Draw a 3d button-like highlight around it
				dc.Draw3dRect( &tempRect, ::GetSysColor(COLOR_3DHILIGHT),
					::GetSysColor(COLOR_3DSHADOW));

				// Fill the gutter with correct color
				tempRect.top+=2;
				tempRect.bottom-=2;
				tempRect.left++;
				tempRect.right--;
				dc.FillSolidRect( &tempRect, gutterColor );

				// Restore the background color
				dc.SetBkColor( prevColor );
			}

			// draw a rectangle around the strip.
			rgn.CreateRectRgn( xOrg - BORDER_VERT_WIDTH, yOrg - BORDER_HORIZ_WIDTH,
				lRightBounds, yEnd + BORDER_HORIZ_WIDTH );
			dc.FrameRgn( &rgn, &brushBorder, BORDER_VERT_DRAWWIDTH, BORDER_HORIZ_DRAWWIDTH );
			rgn.DeleteObject();

			// also draw a rectangle around the strip's function area
			CPen *pOldStripPen = dc.SelectObject( &penVertFnBar );

			// Draw vertical lines
			dc.MoveTo( GUTTER_WIDTH, yOrg - BORDER_HORIZ_WIDTH );
			dc.LineTo( GUTTER_WIDTH, yEnd + BORDER_HORIZ_WIDTH - 1 );
			dc.MoveTo( xOrg - 1, yOrg - BORDER_HORIZ_WIDTH );
			dc.LineTo( xOrg - 1, yEnd + BORDER_HORIZ_WIDTH - 1 );

			// Draw horizontal lines
			BOOL fDrawHorizLines = TRUE;
			if( pSL->m_pNext )
			{
				if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_STRIPMGR, &varTemp )))
				{
					VARIANT varNext;
					if( SUCCEEDED( pSL->m_pNext->m_pStrip->GetStripProperty( SP_STRIPMGR, &varNext )))
					{
						if( V_UNKNOWN(&varTemp) == V_UNKNOWN(&varNext) )
						{
							fDrawHorizLines = FALSE;
						}
						V_UNKNOWN(&varNext)->Release();
					}
					V_UNKNOWN(&varTemp)->Release();
				}

				if( (pSL->m_clsidType == pSL->m_pNext->m_clsidType)
				&&	(pSL->m_clsidType == CLSID_DirectMusicSeqTrack) )
				{
					fDrawHorizLines = FALSE;
				}
			}

			if( fDrawHorizLines )
			{
				dc.SelectObject( &penHorizFnBar );
				if( pSL->m_pNext )
				{
					dc.MoveTo( GUTTER_WIDTH, yEnd + BORDER_HORIZ_WIDTH );
					dc.LineTo( xOrg - 1, yEnd + BORDER_HORIZ_WIDTH );
				}
				dc.MoveTo( GUTTER_WIDTH, yEnd + BORDER_HORIZ_WIDTH - 1 );
				dc.LineTo( xOrg - 1, yEnd + BORDER_HORIZ_WIDTH - 1 );
			}

			dc.SelectObject( pOldStripPen );

			if( yEnd > yTemp )
			{
				long lEarlyStripPosition = 0;
				long lStripXOrg = xOrgForNonPickupStrips;
				if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_EARLY_TIME, &varTemp ) ) )
				{
					// Convert from clocks to pixels
					ClocksToPosition( V_I4(&varTemp), &lEarlyStripPosition );

					lStripXOrg = m_lFunctionBarWidth + max( 0, lEarlyPosition - lEarlyStripPosition - m_lXScrollPos );
				}

				tempRect.left = lStripXOrg;
				tempRect.right = xEnd;
				tempRect.top = yTemp;
				tempRect.bottom = yEnd;
				if( tempRect.IntersectRect( tempRect, rcClientArea ) )
				{
					rgn.CreateRectRgn( tempRect.left, tempRect.top, tempRect.right, tempRect.bottom );
					iClipRgn = dc.SelectClipRgn( &rgn );

					// By using fStripWantsEarlyTime we can trick strips into not displaying anything
					// in pick-up measures

					// Only use pSL->m_lVScroll when the strip is maximized.
					long lTmpXScrollOffset = max( lXScrollOffset - lEarlyPosition, -lEarlyStripPosition );
					if (pSL->m_sv == SV_MINIMIZED)
					{
						dc.SetWindowOrg( -lStripXOrg, -yOrg );
						dc.SetBrushOrg( -lStripXOrg - lTmpXScrollOffset, -yOrg );
					}
					else
					{
						dc.SetWindowOrg( -lStripXOrg, -yOrg + pSL->m_lVScroll );
						dc.SetBrushOrg( -lStripXOrg - lTmpXScrollOffset, -yOrg + pSL->m_lVScroll );
					}

					// Color the entire strip - not just the gutter
					DWORD dwNewBkColor = RGB((SHADING_DARK_COLOR + 255) / 2, (SHADING_DARK_COLOR + 255) / 2, (SHADING_DARK_COLOR + 255) / 2);
					/*
					if( pSL->m_fSelected )
					{
						if( m_pActiveStripList == pSL )
						{
							// Orange (COLOR_GUTTER_ACTIVESELECTED)
							dwNewBkColor = RGB((255 + 255) / 2, (192 + 255) / 2, (128 + 255) / 2);
						}
						else
						{
							// Red (COLOR_GUTTER_SELECTED)
							dwNewBkColor = RGB((255 + 255) / 2, (128 + 255) / 2, (128 + 255) / 2);
						}
					}
					else */ if( m_pActiveStripList == pSL )
					{
						// Yellow (COLOR_GUTTER_ACTIVE)
						dwNewBkColor = RGB((255 + 255) / 2, (255 + 255) / 2, (192 + 255) / 2);
					}

					prevColor = dc.SetBkColor( dwNewBkColor );
					pSL->m_pStrip->Draw( dc.m_hDC, pSL->m_sv, lTmpXScrollOffset );
					dc.SetBkColor(prevColor);

					// draw minimize icon if the strip is MinMaxable and is not already minimized
					VARIANT var;
					if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_MINMAXABLE, &var )))
					{
						if( ( var.vt == VT_BOOL ) && ( V_BOOL(&var) == TRUE ))
						{
							// Compute the location to put the min/max button at
							long lXPosForMinMaxButton;
							if( lStripLatePosition
							&&	(xEnd > posLength - lLatePosition - lXScrollOffset) )
							{
								// Need to compute the position, can't use xEnd since it's beyond the end of the segment
								lXPosForMinMaxButton = min( lRightBounds - BORDER_VERT_WIDTH, posLength - lLatePosition - lXScrollOffset + m_lFunctionBarWidth + BORDER_VERT_DRAWWIDTH - 1 );
							}
							else
							{
								// Just use xEnd
								lXPosForMinMaxButton = xEnd;
							}

							// Offset it from lStripXOrg, and subtract the width of the button
							lXPosForMinMaxButton -= lStripXOrg + m_sizeMinMaxButton.cx;

							if (pSL->m_sv == SV_NORMAL)
							{
								if ( m_MouseMode == MM_ACTIVEMINMAX )
								{
									dc.DrawState(CPoint(lXPosForMinMaxButton, pSL->m_lVScroll ), m_sizeMinMaxButton, &m_BitmapMinimized, DSS_NORMAL);
								}
								else
								{
									dc.DrawState(CPoint(lXPosForMinMaxButton, pSL->m_lVScroll - 2), CSize( m_sizeMinMaxButton.cx, m_sizeMinMaxButton.cy + 2 ) , &m_BitmapMinimize, DSS_NORMAL);
								}
							}
							else if (pSL->m_sv == SV_MINIMIZED)
							{
								if ( m_MouseMode == MM_ACTIVEMINMAX )
								{
									dc.DrawState(CPoint(lXPosForMinMaxButton, 0 ), m_sizeMinMaxButton, &m_BitmapMaximized, DSS_NORMAL);
								}
								else
								{
									dc.DrawState(CPoint(lXPosForMinMaxButton, 0), m_sizeMinMaxButton, &m_BitmapMaximize, DSS_NORMAL);
								}
							}
						}
					}
					rgn.DeleteObject();
				}

				// if the name bar is showing, draw it
				if( yOrg + FUNCTION_NAME_HEIGHT >= yTemp )
				{
					CFont *pOldFont;
					pOldFont = dc.SelectObject( &fontStripName );

					int nOldBkMode;
					nOldBkMode = dc.SetBkMode( TRANSPARENT );

					long yEvenMoreTemp = max( yTemp, yOrg );

					// Make sure we don't overwrite the horizontal scroll bar
					if ( yEnd < yOrg + FUNCTION_NAME_HEIGHT )
					{
						rgn.CreateRectRgn( GUTTER_WIDTH + BORDER_VERT_DRAWWIDTH, 
							yEvenMoreTemp,
							xOrg - BORDER_VERT_WIDTH, yEnd );
					}
					else
					{
						rgn.CreateRectRgn( GUTTER_WIDTH + BORDER_VERT_DRAWWIDTH, 
							yEvenMoreTemp,
							xOrg - BORDER_VERT_WIDTH, yOrg + FUNCTION_NAME_HEIGHT );
					}
					dc.SelectClipRgn( &rgn );
					rgn.DeleteObject();
					dc.SetWindowOrg( -(GUTTER_WIDTH + BORDER_VERT_DRAWWIDTH), -yOrg );
					tempRect.top = 0;
					tempRect.left = 0;
					tempRect.right = xOrg - BORDER_VERT_WIDTH - GUTTER_WIDTH;
					tempRect.bottom = FUNCTION_NAME_HEIGHT;

					// Determine what color to use as the background for thr strip's name
					COLORREF crBackGround = dc.GetNearestColor(COLOR_FUNCTION_NAME);
					BOOL fAuditionOnly = FALSE;

					// Get the strip's strip manager
					if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_STRIPMGR, &varTemp ) ) 
					&&	V_UNKNOWN(&varTemp) )
					{
						// Get an IDMUSProdStripMgr interface
						IDMUSProdStripMgr *pStripMgr;
						if( SUCCEEDED( V_UNKNOWN(&varTemp)->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgr ) ) )
						{
							// Get the track extras flags
							VARIANT varTrackExtras;
							varTrackExtras.vt = VT_BYREF;
							DMUS_IO_TRACK_EXTRAS_HEADER ioTrackExtrasHeader;
							V_BYREF( &varTrackExtras ) = &ioTrackExtrasHeader;
							if( pStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, &varTrackExtras ) == S_OK )
							{
								if( pStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER_MASK, &varTrackExtras ) == S_OK )
								{
									if( 0 != (V_I4(&varTrackExtras) & TRACKCONFIG_PLAY_FLAGS & (~ioTrackExtrasHeader.dwFlags)) )
									{
										// If anything is disabled, change the name background to grey
										crBackGround = dc.GetNearestColor( COLOR_GREY_FUNCTION_NAME );
									}
								}
							}

							// Get the producer-only flags
							IOProducerOnlyChunk ioProducerOnlyChunk;
							varTrackExtras.vt = VT_BYREF;
							V_BYREF( &varTrackExtras ) = &ioProducerOnlyChunk;
							if( pStripMgr->GetStripMgrProperty( SMP_PRODUCERONLY_FLAGS, &varTrackExtras ) == S_OK )
							{
								// If an audition-only strip, hatch the background of the name
								fAuditionOnly = ioProducerOnlyChunk.dwProducerOnlyFlags & SEG_PRODUCERONLY_AUDITIONONLY;
							}
							pStripMgr->Release();
						}
						V_UNKNOWN(&varTemp)->Release();
					}

					// If an audition-only strip, cross hatch the background of the name
					if( fAuditionOnly )
					{
						HBRUSH hbrushHatch = ::CreateHatchBrush( HS_DIAGCROSS, COLOR_HATCH_FUNCTION_NAME ); 
						COLORREF crOldBkColor = dc.SetBkColor( crBackGround );
						::FillRect( dc.GetSafeHdc(), &tempRect, hbrushHatch );
						dc.SetBkColor( crOldBkColor );
					}
					// Otherwise, just fill the background with a solid color
					else
					{
						dc.FillSolidRect( &tempRect, crBackGround );
					}

					CString strName;
					if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_NAME, &varTemp )))
					{
						if( varTemp.vt == VT_BSTR )
						{
							strName = V_BSTR(&varTemp);
							SysFreeString( V_BSTR(&varTemp) );
						}
					}
					COLORREF oldColor;
					oldColor = dc.SetTextColor( RGB(0, 0, 0) );
					UINT oldAlign;
					oldAlign = dc.SetTextAlign( TA_LEFT );
					dc.DrawText( strName, &tempRect,
						DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX );
					dc.SetTextAlign( oldAlign );
					dc.SetTextColor( oldColor );
					dc.SetBkMode( nOldBkMode );
					dc.SelectObject(pOldFont);
				}

				// and now draw the function area if the strip supports it
				IDMUSProdStripFunctionBar* pSFB;
				if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdStripFunctionBar, (void**)&pSFB )))
				{
					// Don't exclude the area where the name was drawn when testing whether or not
					// to draw this function bar
					yTemp = max( yTemp, yOrg );
					tempRect.left = LEFT_DRAW_OFFSET;
					tempRect.right = xOrg - BORDER_VERT_WIDTH;
					tempRect.top = yTemp;
					tempRect.bottom = yEnd;
					if( tempRect.IntersectRect( tempRect, rcClientArea ) )
					{
						// Exclude the area where the name was drawn
						yTemp = max( yTemp, yOrg + FUNCTION_NAME_HEIGHT );
						tempRect.top = yTemp;

						rgn.CreateRectRgn( tempRect.left, tempRect.top, tempRect.right, tempRect.bottom );
						dc.SelectClipRgn( &rgn );
						if (pSL->m_sv == SV_MINIMIZED)
						{
							dc.SetWindowOrg( -LEFT_DRAW_OFFSET, -yOrg );
						}
						else
						{
							dc.SetWindowOrg( -LEFT_DRAW_OFFSET, -yOrg + pSL->m_lVScroll);
						}
						if (pSL->m_sv == SV_MINIMIZED)
						{
							pSFB->FBDraw( dc.m_hDC, SV_FUNCTIONBAR_MINIMIZED );
						}
						else
						{
							pSFB->FBDraw( dc.m_hDC, SV_FUNCTIONBAR_NORMAL );
						}
						rgn.DeleteObject();
					}
					pSFB->Release();
				}
			}
			dc.RestoreDC( iSavedDC );
			yOrg = yEnd + BORDER_HORIZ_DRAWWIDTH;
		}

		// if we are currently in MM_ACTIVERESIZE mode, we need to draw xor'd lines
		// across the width of the window
		if( m_MouseMode == MM_ACTIVERESIZE )
		{
			xOrg = di.prcBounds->left;
			if( xOrg < GUTTER_WIDTH )
			{
				xOrg = GUTTER_WIDTH;
			}
			rgn.CreateRectRgn( xOrg, m_lResizeYPos, lRightBounds,
				m_lResizeYPos + BORDER_HORIZ_DRAWWIDTH );
			dc.InvertRgn( &rgn );
			rgn.DeleteObject();
		}

		dc.SelectObject(pOldStripFont);

		// Always draw the time cursor
		//if( m_lCursor >= 0 )
		{
			long position;
			ClocksToPosition( m_lCursor, &position );
			position -= lXScrollOffset - m_lFunctionBarWidth - lEarlyPosition;
			if( (rcClientArea.left <= position) && (position <= rcClientArea.right) && (position >= m_lFunctionBarWidth) )
			{
				CPen penCursor;
				if( penCursor.CreatePen( PS_SOLID, 1, COLOR_TIME_CURSOR ) )
				{
					CPen *pPenOld;
					pPenOld = dc.SelectObject( &penCursor );
					int nOldROP;
					nOldROP = dc.SetROP2( R2_XORPEN );

					RECT tempScrollRect;
					m_ScrollHorizontal.GetClientRect( &tempScrollRect );
					GetClientRect( &tempRect );
					tempRect.bottom = min(tempRect.bottom - tempScrollRect.bottom, rcClientArea.bottom);
					if( rcClientArea.top < tempRect.bottom )
					{
						dc.MoveTo( position, rcClientArea.top );
						dc.LineTo( position, tempRect.bottom );
					}

					dc.SetROP2( nOldROP );
					dc.SelectObject( pPenOld );
					penCursor.DeleteObject();
				}
			}
		}
///////////////////////////////////////////////
		//Blt the memdc to the screen
		bStatus = realDC.BitBlt(rcClientArea.left, rcClientArea.top, rcClientArea.right - rcClientArea.left, rcClientArea.bottom - rcClientArea.top,
			&dc, rcClientArea.left, rcClientArea.top, SRCCOPY);
		ASSERT(bStatus);
		
		dc.SelectObject(pOldbm); //select out of dc
		bStatus = dc.DeleteDC(); //delete it.
		ASSERT(bStatus);
		delete pRgn;
///////////////////////////////////////////////
		realDC.Detach();
	}
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | DrawMusicLines | This method draws
//		vertical bar, beat, and grid lines in the specified device context.
//
//  @parm   HDC | hdc | A handle to the device context to draw in
//  @parm   <t MUSICLINE_PROPERTY> | mlp | Which type of lines to draw.  Must be one of <t MUSICLINE_PROPERTY>
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_FAIL | Unable to attach to <p hdc>.
//	@rvalue E_INVALIDARG | <p mlp> does not contain a valid value.
//
//	@xref	<i IDMUSProdTimeline>, <t MUSICLINE_PROPERTY>
HRESULT CTimelineCtl::DrawMusicLines( HDC hdc, MUSICLINE_PROPERTY mlp, DWORD dwGroupBits, DWORD dwIndex, LONG lXOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	ASSERT( hdc );
	ASSERT( (mlp == ML_DRAW_MEASURE_BEAT_GRID) || (mlp == ML_DRAW_MEASURE_BEAT) );
	if ( !hdc || ((mlp != ML_DRAW_MEASURE_BEAT_GRID) && (mlp != ML_DRAW_MEASURE_BEAT)) )
	{
		return E_INVALIDARG;
	}

	// Time signature behavior:  If the time signature changes during a measure, the current time
	// signature will continute to apply for that measure.  At the end of the measure, the time
	// signature will change to the most recent one available.  Repeat this process for each
	// time signature change.

	// Create the pens
	HPEN hPenMeasureLine, hPenBeatLine, hPenSubBeatLine;
	hPenMeasureLine = ::CreatePen( MEASURE_LINE_PENSTYLE, MEASURE_LINE_WIDTH, m_colorMeasureLine );
	if( hPenMeasureLine == NULL )
	{
		return E_OUTOFMEMORY;
	}

	hPenBeatLine = ::CreatePen( BEAT_LINE_PENSTYLE, BEAT_LINE_WIDTH, m_colorBeatLine );
	if( hPenBeatLine == NULL )
	{
		::DeleteObject( hPenMeasureLine );
		return E_OUTOFMEMORY;
	}

	if ( mlp == ML_DRAW_MEASURE_BEAT_GRID )
	{
		hPenSubBeatLine = ::CreatePen( SUBBEAT_LINE_PENSTYLE, SUBBEAT_LINE_WIDTH, m_colorSubBeatLine );
		if( hPenSubBeatLine == NULL )
		{
			::DeleteObject( hPenMeasureLine );
			::DeleteObject( hPenBeatLine );
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		hPenSubBeatLine = NULL;
	}

	// Set up our start time, end time
	RECT rectClip;
	MUSIC_TIME mtLeft, mtRight, mtTemp;
	::GetClipBox( hdc, &rectClip );
	PositionToClocks( rectClip.left + lXOffset, &mtLeft );
	// Since the bar lines are two pixels wide, we need to add 1 here
	PositionToClocks( rectClip.right + lXOffset + 1, &mtRight );

	long lPosition = 0; // integer position

	// Save the current pen and switch to the Measure Line pen
	HPEN hPenOld;
	hPenOld = static_cast<HPEN>( ::SelectObject( hdc, hPenMeasureLine ) );

	DMUS_TIMESIGNATURE TimeSig;
	MUSIC_TIME mtTSCur = 0, mtTSNext = 1;
	BYTE bBeat;

	// Handle drawing in negative times
	if( mtLeft < 0 )
	{
		if ( FAILED( GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, 0, NULL, &TimeSig ) ) )
		{
			::SelectObject( hdc, hPenOld );
			::DeleteObject( hPenMeasureLine );
			::DeleteObject( hPenBeatLine );
			if( hPenSubBeatLine )
			{
				::DeleteObject( hPenSubBeatLine );
			}
			return E_UNEXPECTED;
		}

		// Compute the number of clocks per measure
		const MUSIC_TIME mtClocksPerMeasure = TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
		mtTSCur = (mtLeft - mtClocksPerMeasure + 1) / mtClocksPerMeasure;
		mtTSCur *= mtClocksPerMeasure;
	}

	while( mtTSCur < mtRight )
	{
		if ( FAILED( GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, max( 0, mtTSCur), &mtTSNext, &TimeSig ) ) )
		{
			::SelectObject( hdc, hPenOld );
			::DeleteObject( hPenMeasureLine );
			::DeleteObject( hPenBeatLine );
			if( hPenSubBeatLine )
			{
				::DeleteObject( hPenSubBeatLine );
			}
			return E_UNEXPECTED;
		}

		MUSIC_TIME mtNext;
		if( mtTSNext == 0 )
		{
			mtTSNext = m_lLength;
			mtNext = LONG_MAX;
		}
		else
		{
			mtTSNext += max( 0, mtTSCur );
			mtNext = mtTSNext;
		}

		while( (mtTSCur < mtNext) && (mtTSCur < mtRight) )
		{
			// For this measure, compute clocks per beat and clocks per grid
			const MUSIC_TIME mtClocksPerBeat = NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );

			// Draw this measure
			if ( mtTSCur + TimeSig.bBeatsPerMeasure * mtClocksPerBeat >= mtLeft )
			{

				// Draw measure line
				if ( mtTSCur >= mtLeft )
				{
					::SelectObject( hdc, hPenMeasureLine );
					ClocksToPosition( mtTSCur, &lPosition );
					::MoveToEx( hdc, lPosition - lXOffset, rectClip.top, NULL );
					::LineTo( hdc, lPosition - lXOffset, rectClip.bottom );
				}

				// Draw beats
				bBeat = 1;
				::SelectObject( hdc, hPenBeatLine );
				while ( bBeat < TimeSig.bBeatsPerMeasure )
				{
					mtTSCur += mtClocksPerBeat;

					// Draw beat line
					ClocksToPosition( mtTSCur, &lPosition );
					::MoveToEx( hdc, lPosition - lXOffset, rectClip.top, NULL );
					::LineTo( hdc, lPosition - lXOffset, rectClip.bottom );

					bBeat++;
				}

				// Draw Grids
				if ( mlp == ML_DRAW_MEASURE_BEAT_GRID )
				{
					const MUSIC_TIME mtClocksPerGrid = mtClocksPerBeat / TimeSig.wGridsPerBeat;
					bBeat = 0;
					mtTSCur = mtTSCur - mtClocksPerBeat * (TimeSig.bBeatsPerMeasure - 1);
					::SelectObject( hdc, hPenSubBeatLine );
					while ( bBeat < TimeSig.bBeatsPerMeasure )
					{
						// Draw Grids

						// Save next beat position
						mtTemp = mtTSCur + mtClocksPerBeat;

						BYTE bGrid = 1;
						mtTSCur += mtClocksPerGrid;

						// Draw grid lines
						while ( bGrid < TimeSig.wGridsPerBeat )
						{
							ClocksToPosition( mtTSCur, &lPosition );
							::MoveToEx( hdc, lPosition - lXOffset, rectClip.top, NULL );
							::LineTo( hdc, lPosition - lXOffset, rectClip.bottom );

							bGrid++;
							mtTSCur += mtClocksPerGrid;
						}
						mtTSCur = mtTemp;
						bBeat++;
					}
				}
				else
				{
					mtTSCur += mtClocksPerBeat;
				}
			}
			else
			{	// Advance time by one measure
				mtTSCur += TimeSig.bBeatsPerMeasure * mtClocksPerBeat;
			}
		}
	}

	// Restore the previous pen
	::SelectObject( hdc, hPenOld );

	::DeleteObject( hPenMeasureLine );
	::DeleteObject( hPenBeatLine );
	if( hPenSubBeatLine )
	{
		::DeleteObject( hPenSubBeatLine );
	}

	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | AddStripMgr | This method adds an <i IDMUSProdStripMgr> to the
//		list of strip managers within the Timeline.
//
//	@comm	Using the <p dwGroupBits> parameter and the Strip manager's FourCCCKIds
//		(returned by the method <om IDMUSProdStripMgr::GetFourCCCKIDs>), the Timeline determines a
//		position for the StripMgr in its internal list.<nl>
//		They are ordered first by the least track group number they belong to, then in the following order,
//		then by the order they were inserted:<nl>
//		Chord<nl>
//		Signpost<nl>
//		ChordMap Reference<nl>
//		Groove<nl>
//		Tempo<nl>
//		Style Reference<nl>
//		Sequence<nl>
//		Time Signature<nl>
//		Band<nl>
//		Mute<nl>
//		All other strips
//
//  @parm   <i IDMUSProdStripMgr>* | pIStripMgr | The strip manager to add
//  @parm	DWORD | dwGroupBits | Which track group(s) this strip manager belongs to.  A value
//		of 0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. 
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p punkStripMgr>
//	@rvalue E_INVALIDARG | The strip manager was previously added to the Timeline
//	@rvalue E_OUTOFMEMORY | Unable to allocate memory while adding the strip manager
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdTimeline::RemoveStripMgr>
HRESULT CTimelineCtl::AddStripMgr( IDMUSProdStripMgr* pIStripMgr, DWORD dwGroupBits)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( NULL == pIStripMgr )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	StripMgrList*	pScan;
	for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_pStripMgr == pIStripMgr )
		{
			return E_INVALIDARG;
		}
	}

	StripMgrList*	pSML = new StripMgrList( pIStripMgr, dwGroupBits );
	if( pSML )
	{
		// Empty list
		if( !m_pStripMgrList )
		{
			m_pStripMgrList = pSML;
			pSML->m_pNext = NULL;

			VARIANT varTimeline;
			varTimeline.vt = VT_UNKNOWN;
			QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&varTimeline)) );
			pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
			Release();
			return S_OK;
		}

		DMUS_IO_TRACK_HEADER ioTrackHeader;
		ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
		VARIANT varTrackHeader;
		varTrackHeader.vt = VT_BYREF;
		V_BYREF(&varTrackHeader) = &ioTrackHeader;
		if( FAILED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
		{
			TRACE("Timeline: Unable to get StripMgr's FourCCCKIDs\n");
		}

		int nType1, nType2;
		nType1 = StripCLSIDToInt( ioTrackHeader.guidClassID );

		StripMgrList*	pScanOld = NULL;
		for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
		{
			ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
			varTrackHeader.vt = VT_BYREF;
			V_BYREF(&varTrackHeader) = &ioTrackHeader;
			if( FAILED( pScan->m_pStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
			{
				TRACE("Timeline: Unable to get StripMgr's FourCCCKIDs\n");
			}

			// Ensure the TimeStripMgr is always the last strip
			if( pScan->m_pStripMgr->IsParamSupported( CLSID_TimeStripMgr ) == S_OK )
			{
				// if pSML should go before pScan, insert it there
				if( pScanOld )
				{
					pScanOld->m_pNext = pSML;
				}
				else
				{
					// Head of the list
					ASSERT( pScan == m_pStripMgrList );
					m_pStripMgrList = pSML;
				}
				pSML->m_pNext = pScan;

				VARIANT varTimeline;
				varTimeline.vt = VT_UNKNOWN;
				QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&varTimeline)) );
				pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
				Release();
				return S_OK;
			}
			nType2 = StripCLSIDToInt( ioTrackHeader.guidClassID );

			if( CompareStrips( nType2, pScan->m_dwGroupBits, 0, NULL,
							   nType1, dwGroupBits, 0, NULL ) == 2 )
			{
				// if pSML should go before pScan, insert it there
				if( pScanOld )
				{
					pScanOld->m_pNext = pSML;
				}
				else
				{
					// Head of the list
					ASSERT( pScan == m_pStripMgrList );
					m_pStripMgrList = pSML;
				}
				pSML->m_pNext = pScan;

				VARIANT varTimeline;
				varTimeline.vt = VT_UNKNOWN;
				QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&varTimeline)) );
				pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
				Release();
				return S_OK;
			}
			pScanOld = pScan;
		}
		
		// End of the list
		pScanOld->m_pNext = pSML;
		pSML->m_pNext = NULL;

		VARIANT varTimeline;
		varTimeline.vt = VT_UNKNOWN;
		QueryInterface( IID_IUnknown, (void **) &(V_UNKNOWN(&varTimeline)) );
		pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
		Release();
		return S_OK;
	}
	else
	{
		return E_OUTOFMEMORY;
	}
}

//  @method HRESULT | IDMUSProdTimeline | RemoveStripMgr | This method removes a strip manager
//		from the Timeline.
//
//  @parm   <i IDMUSProdStripMgr>* | pIStripMgr | The strip manager to remove
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p punkStripMgr>
//	@rvalue E_INVALIDARG | <p pIStripMgr> was not previously added to the Timeline via
//		<om IDMUSProdTimeline::AddStripMgr>.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdTimeline::AddStripMgr>
HRESULT CTimelineCtl::RemoveStripMgr(
	/* [in] */	IDMUSProdStripMgr* pIStripMgr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT			hr;

	if( NULL == pIStripMgr )
	{
		return E_POINTER;
	}
	StripMgrList	*pSML, *pSMLold;
	pSML = m_pStripMgrList;
	pSMLold = NULL;
	hr = E_INVALIDARG;
	// Remove pIStripMgr from m_pStripMgrList
	while ( pSML )
	{
		if ( pSML->m_pStripMgr == pIStripMgr )
		{
			VARIANT varTimeline;
			varTimeline.vt = VT_UNKNOWN;
			V_UNKNOWN(&varTimeline) = NULL;
			pIStripMgr->SetStripMgrProperty(SMP_ITIMELINECTL, varTimeline);
			if ( pSMLold )
			{
				pSMLold->m_pNext = pSML->m_pNext;
			}
			else
			{
				m_pStripMgrList = pSML->m_pNext;
			}
			delete pSML;
			pSML = NULL;
			hr = S_OK;
		}
		else
		{
			pSMLold = pSML;
			pSML = pSML->m_pNext;
		}
	}

	// Remove the Stripmgr from the list of Notify Entries
	POSITION pos, pos2;
	NotifyEntry *pNotifyEntry;
	pos = m_lstNotifyEntry.GetHeadPosition();
	while ( pos != NULL )
	{
		pNotifyEntry = m_lstNotifyEntry.GetNext( pos );
		// Found guidNotify, now find pIStripMgr
		NotifyListEntry* pNotifyListEntry = NULL;
		pos2 = pNotifyEntry->m_lstNotifyListEntry.GetHeadPosition( );
		while ( pos2 != NULL )
		{
			POSITION pos3 = pos2;
			pNotifyListEntry = pNotifyEntry->m_lstNotifyListEntry.GetNext( pos2 );
			if( pNotifyListEntry->pIStripMgr == pIStripMgr )
			{
				// We've found it, now remove it from the list and delete our entry
				pNotifyEntry->m_lstNotifyListEntry.RemoveAt( pos3 );
				delete pNotifyListEntry;

				// Check if the list is empty
				if ( pNotifyEntry->m_lstNotifyListEntry.IsEmpty() )
				{
					// If the list is empty, remove it from m_lstNotifyEntry
					pos2 = m_lstNotifyEntry.Find( pNotifyEntry );
					ASSERT( pos2 );
					if ( pos2 )
					{
						m_lstNotifyEntry.RemoveAt( pos2 );
						delete pNotifyEntry;
						pos2 = NULL;
					}
				}
			}
		}
	}

	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | AddStrip | This method adds a strip at the
//		bottom of the strips displayed by the Timeline.
//
//  @parm   <i IDMUSProdStrip>* | pIStrip | The strip to add
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p punkStrip>
//	@rvalue E_FAIL | The strip is already displayed by the Timeline
//	@rvalue E_OUTOFMEMORY | Unable to allocate memory while adding the strip
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <om IDMUSProdTimeline::RemoveStrip>,
//		<om IDMUSProdTimeline::InsertStripAtDefaultPos>, <om IDMUSProdTimeline::InsertStripAtPos>.
HRESULT CTimelineCtl::AddStrip( 
    /* [in] */ IDMUSProdStrip* pIStrip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( NULL == pIStrip )
	{
		return E_POINTER;
	}

	if( m_pTimeStrip == NULL )
	{
		m_pTimeStrip = static_cast<CTimeStrip *>(pIStrip);
		ASSERT( m_pTimeStrip );
	}

	return InternalInsertStripAtPos( pIStrip, 0xffffffff, GUID_AllZeros, 1, 0 );
}

//  @method HRESULT | IDMUSProdTimeline | RemoveStrip | This method removes a strip from
//		the Timeline.
//
//  @parm   <i IDMUSProdStrip>* | pIStrip | The strip to remove
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStrip>
//	@rvalue E_INVALIDARG | <p pIStrip> was not previously added to the Timeline via
//		<om IDMUSProdTimeline::AddStrip>.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <om IDMUSProdTimeline::AddStrip>
HRESULT CTimelineCtl::RemoveStrip(
	/* [in] */	IDMUSProdStrip* pIStrip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT			hr = S_OK;
	RECT			rect, rectWin;

	if( NULL == pIStrip )
	{
		return E_POINTER;
	}
	// if the strip is already in the list, return E_FAIL
	StripList* pSL = FindStripList(pIStrip);
	if( !pSL )
	{
		hr = E_INVALIDARG;
	}
	else
	{
		// If this strip is active, sent it a killfocus
		if ( m_pActiveStripList == pSL )
		{
			CallStripWMMessage( pSL, WM_KILLFOCUS, 0, 0 );
			m_pActiveStripList = NULL;
		}

		// If a strip is removed after the Timeline window is destroyed,
		// don't recompute the positions of the strips and scrollbars.
		if( ::IsWindow( m_hWnd ) )
		{
			GetClientRect( &rectWin );
			GetStripClientRect( pSL, &rect );
			rectWin.top = rect.top - BORDER_HORIZ_WIDTH;
			InvalidateRect( &rectWin, FALSE );
			ComputeScrollBars();
		}

		// remove the strip from m_pStripList
		ASSERT(m_pStripList);
		if( m_pStripList )
		{
			StripList*	pTemp = m_pStripList;
			StripList*	pTempOld = NULL;
			while( pTemp )
			{
				if ( pTemp == pSL )
				{
					if (pTempOld)
					{
						pTempOld->m_pNext = pTemp->m_pNext;
					}
					else
					{
						m_pStripList = pTemp->m_pNext;
					}

					// Send WM_MOVE messages to all strips that were moved
					if( ::IsWindow(m_hWnd) )
					{
						pTemp = pTemp->m_pNext;
						while( pTemp )
						{
							pTemp->m_pStrip->OnWMMessage( WM_MOVE, 0, 0, 0, 0 );
							pTemp = pTemp->m_pNext;
						}
					}
					else
					{
						pTemp = NULL;
					}
				}
				else
				{
					pTempOld = pTemp;
					pTemp = pTemp->m_pNext;
				}
			}
		}

		if ( m_pMouseStripList == pSL )
		{
			m_pMouseStripList = NULL;
		}
		if ( m_pLastDragStripList == pSL )
		{
			m_pLastDragStripList = NULL;
		}
		IDMUSProdTimelineEdit* pITimelineEdit;
		if ( SUCCEEDED(pIStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**)&pITimelineEdit )))
		{
			pITimelineEdit->Release();
		}

		pIStrip->OnWMMessage( WM_DESTROY, 0, 0, 0, 0 );

		delete pSL;

		// If we need to scroll up, do so
		if( m_lYScrollPos && ::IsWindow( m_hWnd ) )
		{
			RECT rect, rectS;
			GetClientRect(&rect);
			m_ScrollHorizontal.GetClientRect(&rectS);
			rect.bottom -= rectS.bottom;

			long lMaxScroll = TotalStripHeight() - rect.bottom;
			if( lMaxScroll < m_lYScrollPos )
			{
				int nPos;
				if( lMaxScroll < 1 )
				{
					nPos = 0;
				}
				else
				{
					nPos = ( lMaxScroll * m_ScrollVertical.GetScrollLimit()) / lMaxScroll;
				}
				int iTemp = 0;
				OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, nPos ), (LPARAM) ((HWND) m_ScrollVertical), iTemp );
			}
		}
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | GetParam | This method retrieves data of the specified type
//		from a strip manager in the Timeline.
//
//  @parm   REFGUID | rguidType | Reference to the identifier of the type of data to obtain.
//		See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible data types.  Strips can also define
//		their own types for custom data.
///  @parm   DWORD | dwGroupBits | Which track group(s) to scan for the strip manager in.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. To scan all strip managers
//		regardless of groups, set this parameter to 0xFFFFFFFF. 
//  @parm   DWORD | dwIndex | Index of the strip manager in the group(s) from which to obtain the data. 
//  @parm   MUSIC_TIME | mtTime | Time from which to obtain the data.
//	@parm   MUSIC_TIME* | pmtNext | Address of a variable to receive the time (relative to
//		the current time) until which the data is valid. If this returns a value of 0, it means
//		either that the data will always be valid, or that it is unknown when it will become
//		invalid. If this information is not needed, <p pmtNext> may be set to NULL. 
//  @parm   void* | pData | Address of an allocated structure in which the data is to be returned. This
//		structure must be of the appropriate kind and size for the data type identified by <p rguidType>.
//
//	@comm	This method is analagous to the DirectMusic method <om IDirectMusicPerformance::GetParam>.
//
//	@comm	Strip managers are searched in the order that they are listed in the Timeline.  See
//		<om IDMUSProdTimeline::AddStripMgr> for the order they are listed.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pData>
//	@rvalue E_INVALIDARG | No strip managers support the requested <p rguidType>.
//	@rvalue E_UNEXPECTED | An internal error occurred.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdTimeline::SetParam>, <om IDMUSProdStripMgr::GetParam>
HRESULT CTimelineCtl::GetParam(/* [in] */  REFGUID		guidType,
				/* [in] */  DWORD		dwGroupBits,
				/* [in] */  DWORD		dwIndex,
				/* [in] */  MUSIC_TIME	mtTime,
				/* [out] */ MUSIC_TIME*	pmtNext,
				/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	StripMgrList*	pScan;
	HRESULT hr = E_INVALIDARG;
	for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_dwGroupBits & dwGroupBits )
		{
			ASSERT( pScan->m_pStripMgr != NULL );
			if ( pScan->m_pStripMgr == NULL )
			{
				return E_UNEXPECTED;
			}
			if ( pScan->m_pStripMgr->IsParamSupported( guidType ) == S_OK )
			{
				/*
				// Get the track extras flags
				VARIANT varTrackExtras;
				varTrackExtras.vt = VT_BYREF;
				DMUS_IO_TRACK_EXTRAS_HEADER ioTrackExtrasHeader;
				if( pScan->m_pStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKEXTRASHEADER, &varTrackExtras ) == S_OK )
				{
					if( !(ioTrackExtrasHeader.dwFlags & DMUS_TRACKCONFIG_CONTROL_ENABLED) )
					{
						continue;
					}
				}
				*/

				if ( dwIndex == 0 )
				{
					if ( pData == NULL )
					{
						return E_POINTER;
					}
					else
					{
						if( SUCCEEDED( pScan->m_pStripMgr->GetParam( guidType, mtTime, pmtNext, pData ) ) )
						{
							return S_OK;
						}
						else
						{
							hr = E_UNEXPECTED;
						}
					}
				}
				else
				{
					dwIndex--;
				}
			}
		}
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | SetParam | This method sets data on a
//		strip manager in the timeline
//
//  @parm   REFGUID | rguidType | Reference to the identifier of the type of data to set.
//		See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible data types.  Strips can also define
//		their own types for custom data.
//  @parm   DWORD | dwGroupBits | Which track group(s) to scan for the strip manager in.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. To scan all strip managers
//		regardless of groups, set this parameter to 0xFFFFFFFF. 
//  @parm   DWORD | dwIndex | Index of the strip manager in the group(s) identified by <p dwGroupBits> where data is to be set. 
//  @parm   MUSIC_TIME | mtTime | Time at which to set the data.
//  @parm   void* | pData | Address of structure containing the data. This structure must be of
//		the appropriate kind and size for the data type identified by rguidType.
//
//	@comm	This method is analagous to the DirectMusic method <om IDirectMusicPerformance::SetParam>.
//
//	@comm	Strip managers are searched in the order that they are listed in the Timeline.  See
//		<om IDMUSProdTimeline::AddStripMgr> for the order they are listed.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pData>
//	@rvalue E_INVALIDARG | No strip managers support the requested <p rguidType>.
//	@rvalue E_UNEXPECTED | An internal error occurred.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdTimeline::GetParam>, <om IDMUSProdStripMgr::SetParam>
HRESULT CTimelineCtl::SetParam(/* [in] */ REFGUID		guidType,
				/* [in] */ DWORD		dwGroupBits,
				/* [in] */ DWORD		dwIndex,
				/* [in] */ MUSIC_TIME	mtTime,
				/* [in] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	if( ::IsEqualGUID( GUID_TimelineSetSegStartTime, guidType ) )
	{
		ASSERT( pData );
		if( pData )
		{
			// Save the current elapsed time, so we can display it
			if( (*(REFERENCE_TIME*)pData) == 0 )
			{
				REFERENCE_TIME rtRefTime;
				if( m_pIDMPerformance && (m_rtSegmentStart != 0)
				&&	SUCCEEDED( m_pIDMPerformance->GetTime( &rtRefTime, NULL ) ) )
				{
					m_rtLastElapsedTime = rtRefTime - m_rtSegmentStart;
				}
				else
				{
					m_rtLastElapsedTime = 0;
				}
			}

			m_rtSegmentStart = *(REFERENCE_TIME*)pData;
			// Don't do this - it causes bad things to happen in MFC if
			// we're not in a message handler thread.
			/*
			if( m_fShowRealTime )
			{
				// Initialize status bar panes to '0'
				CString strText;
				RefTimeToString( 0, strText );
				BSTR bstrName = strText.AllocSysString();
				m_pDMUSProdFramework->SetStatusBarPaneText( 0, bstrName, TRUE );
			}
			*/
		}
		return S_OK;
	}
	if( ::IsEqualGUID( GUID_TimelineShowRealTime, guidType ) )
	{
		ASSERT( pData );
		if( pData )
		{
			m_hKeyRealTimeStatusBar = *(HANDLE *)pData;
			if( m_hKeyRealTimeStatusBar )
			{
				// Initialize status bar panes
				REFERENCE_TIME rtRefTime = 0;
				CString strText;
				ClocksToRefTime( m_lCursor, &rtRefTime );
				RefTimeToString( rtRefTime, IDS_OFFSET_TIME, IDS_OFFSET_NEG_TIME, strText );
				m_pDMUSProdFramework->SetStatusBarPaneText( m_hKeyRealTimeStatusBar, 1, strText.AllocSysString(), TRUE );

				rtRefTime = 0;
				if( m_pIDMPerformance && (m_rtSegmentStart != 0)
				&&	SUCCEEDED( m_pIDMPerformance->GetTime( &rtRefTime, NULL ) ) )
				{
					rtRefTime -= m_rtSegmentStart;
					RefTimeToString( rtRefTime, IDS_ELAPSED_TIME, IDS_ELAPSED_NEG_TIME, strText );
				}
				else if( m_rtSegmentStart == 0 )
				{
					// Display the last value of the ElapsedTime counter
					RefTimeToString( m_rtLastElapsedTime, IDS_ELAPSED_TIME, IDS_ELAPSED_NEG_TIME, strText );
				}
				m_pDMUSProdFramework->SetStatusBarPaneText( m_hKeyRealTimeStatusBar, 0, strText.AllocSysString(), TRUE );
			}
		}
		return S_OK;
	}

	StripMgrList*	pScan;
	for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_dwGroupBits & dwGroupBits )
		{
			ASSERT( pScan->m_pStripMgr != NULL );
			if ( pScan->m_pStripMgr == NULL )
			{
				return E_UNEXPECTED;
			}
			if ( pScan->m_pStripMgr->IsParamSupported( guidType ) == S_OK )
			{
				if ( dwIndex == 0 )
				{
					if ( pData == NULL )
					{
						return E_POINTER;
					}
					else
					{
						return pScan->m_pStripMgr->SetParam( guidType, mtTime, pData );
					}
				}
				else
				{
					dwIndex--;
				}
			}
		}
	}
	return E_INVALIDARG;
}

//  @method HRESULT | IDMUSProdTimeline | GetStripMgr | This method retrieves a pointer to the specified
//		strip manager.
//
//  @parm   REFGUID | rguidType | Reference to the identifier of the type of data to search for.
//		See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible data types.  Strips can also define
//		their own types for custom data.
//  @parm   DWORD | dwGroupBits | Which track group(s) to scan for the strip manager in.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. To scan all tracks
//		regardless of groups, set this parameter to 0xFFFFFFFF. 
//  @parm   DWORD | dwIndex | Zero-based index into the list of tracks of type <p rguidType>
//		and in group <p dwGroupBits> to return. If multiple groups are selected in <p dwGroupBits>,
//		this index will indicate the nth track of type <p rguidType> encountered in the union of
//		the groups selected. 
//  @parm   <i IDMUSProdStripMgr>** | ppIStripMgr | Address of a variable to receive a pointer to
//		the strip manager.
//
//	@comm	This method is analagous to the DirectMusic method <om IDirectMusicPerformance::GetTrack>.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p ppIStripMgr>
//	@rvalue E_INVALIDARG | No matching strip manager was found
//	@rvalue E_UNEXPECTED | An internal error occurred.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdStripMgr::IsParamSupported>
HRESULT CTimelineCtl::GetStripMgr(/* [in] */ REFGUID				guidType,
					/* [in] */ DWORD				dwGroupBits,
					/* [in] */ DWORD				dwIndex,
					/* [out,retval] */ IDMUSProdStripMgr**	ppStripMgr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ppStripMgr == NULL )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	StripMgrList*	pScan;
	for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_dwGroupBits & dwGroupBits )
		{
			ASSERT( pScan->m_pStripMgr != NULL );
			if ( pScan->m_pStripMgr == NULL )
			{
				return E_UNEXPECTED;
			}
			if ( pScan->m_pStripMgr->IsParamSupported( guidType ) == S_OK )
			{
				if ( dwIndex == 0 )
				{
					*ppStripMgr = pScan->m_pStripMgr;
					pScan->m_pStripMgr->AddRef();
					return S_OK;
				}
				else
				{
					dwIndex--;
				}
			}
		}
	}

	DMUS_IO_TRACK_HEADER ioTrackHeader;
	VARIANT varTrackHeader;
	for( pScan = m_pStripMgrList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_dwGroupBits & dwGroupBits )
		{
			ASSERT( pScan->m_pStripMgr != NULL );
			if ( pScan->m_pStripMgr == NULL )
			{
				return E_UNEXPECTED;
			}

			ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );
			varTrackHeader.vt = VT_BYREF;
			V_BYREF(&varTrackHeader) = &ioTrackHeader;
			if ( SUCCEEDED( pScan->m_pStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
			{
				if( ::IsEqualGUID( guidType, ioTrackHeader.guidClassID ) )
				{
					if ( dwIndex == 0 )
					{
						*ppStripMgr = pScan->m_pStripMgr;
						pScan->m_pStripMgr->AddRef();
						return S_OK;
					}
					else
					{
						dwIndex--;
					}
				}
			}
		}
	}

	return E_INVALIDARG;
}

//  @method HRESULT | IDMUSProdTimeline | InsertStripAtDefaultPos | This method inserts a strip into
//		timeline, ordering them by using <p rclsidType>, <p dwGroupBits> and <p dwIndex>.
//
//	@comm	Using the <p dwGroupBits> parameter and the <p rclsidType> parameter, the Timeline determines a
//		position for <p pIStrip> in its display list.<nl>
//		They are ordered first by the least track group number they belong to, then in the following order,
//		then by the value of <p dwIndex>, then by the order they were inserted:<nl>
//		Chord<nl>
//		Signpost<nl>
//		ChordMap Reference<nl>
//		Groove<nl>
//		Tempo<nl>
//		Style Reference<nl>
//		Sequence<nl>
//		Time Signature<nl>
//		Band<nl>
//		Mute<nl>
//		All other strips
//
//  @parm   <i IDMUSProdStrip>* | pIStrip | The strip to add.
//  @parm	REFCLSID | rclsidType | The CLSID of the strip to add.
//  @parm   DWORD | dwGroupBits | Which track group(s) to add the strip in.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group.
//  @parm   DWORD | dwIndex | Where in the list of matching strips to add the strip..
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStrip>
//	@rvalue E_FAIL | <p pIStrip> was previously added.
//	@rvalue E_INVALIDARG | 0 was passed as <p dwGroupBits>.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <om IDMUSProdTimeline::RemoveStrip>,
//		<om IDMUSProdTimeline::InsertStripAtPos>, <om IDMUSProdTimeline::AddStrip>
HRESULT CTimelineCtl::InsertStripAtDefaultPos(
		/* [in] */ IDMUSProdStrip* pStrip,
		/* [in] */ REFCLSID		clsidType,
		/* [in] */ DWORD		dwGroupBits,
		/* [in] */ DWORD		dwIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	DWORD		dwPosition = 0;

	if( NULL == pStrip )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	// if the strip is already in the list, return E_FAIL
	StripList*	pScan;
	for( pScan = m_pStripList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_pStrip == pStrip )
		{
			pStrip->Release();
			return E_FAIL;
		}
	}

	// determine the default position
	dwPosition = 1;
	if( m_pStripList )
	{
		if( m_pStripList->m_pNext != NULL )
		{
			// Remember to skip over the first strip.
			StripList*	pTemp = m_pStripList->m_pNext;
			int nMyId = StripCLSIDToInt( clsidType );
			BSTR bstrMyName = NULL;
			VARIANT var;
			if( SUCCEEDED( pStrip->GetStripProperty( SP_NAME, &var ) ) && (var.vt == VT_BSTR) )
			{
				bstrMyName = V_BSTR(&var);
			}
			IUnknown *punkMyStripMgr = NULL;
			if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &var ) ) && (var.vt == VT_UNKNOWN) )
			{
				punkMyStripMgr = V_UNKNOWN(&var);
			}

			IUnknown *punkLastStripMgr = NULL;
			IUnknown *punkTempStripMgr = NULL;
			while( pTemp )
			{
				BSTR bstrTempName = NULL;
				if( SUCCEEDED( pTemp->m_pStrip->GetStripProperty( SP_NAME, &var ) ) && (var.vt == VT_BSTR) )
				{
					bstrTempName = V_BSTR(&var);
				}
				if( SUCCEEDED( pTemp->m_pStrip->GetStripProperty( SP_STRIPMGR, &var ) ) && (var.vt == VT_UNKNOWN) )
				{
					punkTempStripMgr = V_UNKNOWN(&var);
				}
				// (If the last StripMgr and the temp StripMgr are different and the last StripMgr and my
				// StripMgr are the same)
				// OR
				// (If the last StripMgr and the temp StripMgr are different, or if the last, temp, and my
				// StripMgr are all the same, check if the strip being inserted should go BEFORE pTemp.)
				// If so, break.
				if( ((punkLastStripMgr != punkTempStripMgr) && (punkLastStripMgr == punkMyStripMgr))
				||	(((punkLastStripMgr != punkTempStripMgr) || (punkMyStripMgr == punkTempStripMgr))
					 && CompareStrips( StripCLSIDToInt(pTemp->m_clsidType), pTemp->m_dwGroupBits, pTemp->m_dwIndex, bstrTempName,
								   nMyId, dwGroupBits, dwIndex, bstrMyName ) == 2) )
				{
					if( bstrTempName )
					{
						::SysFreeString( bstrTempName );
					}
					if( punkLastStripMgr )
					{
						punkLastStripMgr->Release();
					}
					break;
				}
				if( bstrTempName )
				{
					::SysFreeString( bstrTempName );
				}
				if( punkLastStripMgr )
				{
					punkLastStripMgr->Release();
				}
				punkLastStripMgr = punkTempStripMgr;
				dwPosition++;
				pTemp = pTemp->m_pNext;
			}

			if( punkTempStripMgr )
			{
				punkTempStripMgr->Release();
			}
			if( punkMyStripMgr )
			{
				punkMyStripMgr->Release();
			}

			if( bstrMyName )
			{
				::SysFreeString( bstrMyName );
			}
		}
	}

	return InternalInsertStripAtPos( pStrip, dwPosition, clsidType, dwGroupBits, dwIndex );
}

int StripCLSIDToInt( REFCLSID clsidType )
{
	if( memcmp( &clsidType, &CLSID_DirectMusicTempoTrack, sizeof(GUID) ) == 0 )
	{
		return 0;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicTimeSigTrack, sizeof(GUID) ) == 0 )
	{
		return 10;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicChordMapTrack, sizeof(GUID) ) == 0 )
	{
		return 20;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicSignPostTrack, sizeof(GUID) ) == 0 )
	{
		return 30;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicChordTrack, sizeof(GUID) ) == 0 )
	{
		return 40;
	}
	else if( (memcmp( &clsidType, &CLSID_DirectMusicMotifTrack, sizeof(GUID) ) == 0)
		 ||	 (memcmp( &clsidType, &CLSID_DirectMusicSegmentTriggerTrack, sizeof(GUID) ) == 0) )
	{
		return 50;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicCommandTrack, sizeof(GUID) ) == 0 )
	{
		return 60;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicStyleTrack, sizeof(GUID) ) == 0 )
	{
		return 70;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicMelodyFormulationTrack, sizeof(GUID) ) == 0 )
	{
		return 80;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicPatternTrack, sizeof(GUID) ) == 0 )
	{
		return 90;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicSeqTrack, sizeof(GUID) ) == 0 )
	{
		return 100;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicWaveTrack, sizeof(GUID) ) == 0 )
	{
		return 110;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicBandTrack, sizeof(GUID) ) == 0 )
	{
		return 120;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicParamControlTrack, sizeof(GUID) ) == 0 )
	{
		return 130;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicMuteTrack, sizeof(GUID) ) == 0 )
	{
		return 140;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicScriptTrack, sizeof(GUID) ) == 0 )
	{
		return 150;
	}
	else if( memcmp( &clsidType, &CLSID_DirectMusicLyricsTrack, sizeof(GUID) ) == 0 )
	{
		return 160;
	}
	else
	{
		return 170;
	}
}

int CompareStrips( int nType1, DWORD dwGroups1, DWORD dwIndex1, BSTR bstrName1, int nType2, DWORD dwGroups2, DWORD dwIndex2, BSTR bstrName2 )
{
	// If the group bits are equal, check the types
	if( dwGroups1 == dwGroups2 )
	{
		if( nType1 == nType2 )
		{
			if( dwIndex1 == dwIndex2 )
			{
				if( bstrName1 && bstrName2 )
				{
					CString strName1 = bstrName1;
					CString strName2 = bstrName2;
					return strName1.CompareNoCase( strName2 ) < 0 ? 1 : 2;
				}
			}
			return long(dwIndex1) <= long(dwIndex2) ? 1 : 2;
		}
		return nType1 < nType2 ? 1 : 2;
	}

	// Continue while both group bits have at least one bit still set
	while( dwGroups1 && dwGroups2 )
	{
		if( (dwGroups1 & dwGroups2 & 1) == 0 )
		{
			// One of the first bits of dwGroups1 or dwGroups2 is zero
			if( dwGroups1 & 1 )
			{
				// The first bit of dwGroups1 is one and at least one bit in dwGroups2 is set -
				// #1 should go first
				return 1;
			}
			else if( dwGroups2 & 1 )
			{
				// The first bit of dwGroups2 is one and at least one bit in dwGroups1 is set -
				// #2 should go first
				return 2;
			}
			// else both of the first bits are zero - try the next bit
		}
		else
		{
			// Both of the first bits of dwGroups1 and dwGroups2 are one
			if( nType1 < nType2 )
			{
				return 1; // #1 should go first
			}
			else if ( nType1 > nType2 )
			{
				return 2; // #2 should go first
			}
			// else both are the same type - check the next bit
		}
		dwGroups1 = dwGroups1 >> 1;
		dwGroups2 = dwGroups2 >> 1;
	}

	if( dwGroups1 )
	{
		// Some of the bits in dwGroups1 are set - #2 should go first
		return 2;
	}
	else if( dwGroups2 )
	{
		// Some of the bits in dwGroups2 are set - #1 should go first
		return 1;
	}
	// dwGroups1 == dwGroups2 == 0 (Shouldn't happen!)
	ASSERT( FALSE );
	return 1;
}

HRESULT CTimelineCtl::InternalInsertStripAtPos( IDMUSProdStrip* pStrip, DWORD dwPosition, REFCLSID clsidType, DWORD dwGroupBits, DWORD dwIndex )
{
	HRESULT			hr = S_OK;
	StripList*		pSL;

	if( pStrip == NULL )
	{
		return E_POINTER;
	}

	if( dwPosition == 0 )
	{
		return E_INVALIDARG;
	}
	dwPosition--;

	// if the strip is already in the list, return E_FAIL
	StripList*	pScan;
	for( pScan = m_pStripList; pScan; pScan = pScan->m_pNext )
	{
		if( pScan->m_pStrip == pStrip )
		{
			pStrip->Release();
			return E_FAIL;
		}
	}
	// add the strip to the list of strips
	pSL = new StripList( pStrip, clsidType );
	if( pSL == NULL )
	{
		hr = E_OUTOFMEMORY;
	}
	else
	{
		pSL->m_dwGroupBits = dwGroupBits;
		pSL->m_dwIndex = dwIndex;

		// ask the Strip how tall it wants to be
		VARIANT var;
		if( SUCCEEDED( pStrip->GetStripProperty( SP_DEFAULTHEIGHT, &var )))
		{
			pSL->m_lHeight = V_INT(&var);
		}
		else if( SUCCEEDED( pStrip->GetStripProperty( SP_MAXHEIGHT, &var )))
		{
			pSL->m_lHeight = V_INT(&var);
		}
		else if( SUCCEEDED( pStrip->GetStripProperty( SP_MINHEIGHT, &var )))
		{
			pSL->m_lHeight = V_INT(&var);
		}

		// add the strip to the specified position
		if( m_pStripList )
		{
			StripList* pTemp = m_pStripList;
			while( pTemp->m_pNext && dwPosition > 0 )
			{
				pTemp = pTemp->m_pNext;
				dwPosition--;
			}
			pSL->m_pNext = pTemp->m_pNext;
			pTemp->m_pNext = pSL;

			// call the strip's callback that it has been added
			pStrip->OnWMMessage( WM_CREATE, 0, 0, 0, 0 );

			// Send WM_MOVE messages to all strips that were moved
			if( ::IsWindow(m_hWnd) )
			{
				pTemp = pSL->m_pNext;
				while( pTemp )
				{
					pTemp->m_pStrip->OnWMMessage( WM_MOVE, 0, 0, 0, 0 );
					pTemp = pTemp->m_pNext;
				}
			}
		}
		else
		{
			m_pStripList = pSL;

			// call the strip's callback that it has been added
			pStrip->OnWMMessage( WM_CREATE, 0, 0, 0, 0 );
		}
		// Compute what the scroll bars look like
		ComputeScrollBars();

		RECT rectWin, rect;
		GetClientRect( &rectWin );
		GetStripClientRect( pSL, &rect );
		rectWin.top = rect.top - BORDER_HORIZ_WIDTH;
		InvalidateRect( &rectWin, FALSE );
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | EnumStrip | This method enumerates through all strips
//		displayed within the Timeline.
//
//  @parm   DWORD | dwEnum | Zero-based index into the Timeline's strip list. 
//  @parm   <i IDMUSProdStrip>** | ppIStrip | Address of a variable to receive a pointer to the strip.
//		The caller is responsible for Release()ing the pointer when it is done with it.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p ppIStrip>
//	@rvalue E_FAIL | There is no strip at the given index.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>
HRESULT CTimelineCtl::EnumStrip(
		/* [in] */  DWORD			 dwEnum,
		/* [in] */  IDMUSProdStrip** ppStrip)
{
	if( ppStrip == NULL )
	{
		return E_POINTER;
	}
	StripList	*pSL;

	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( dwEnum == 0 )
		{
			*ppStrip = pSL->m_pStrip;
			if( pSL->m_pStrip )
			{
				pSL->m_pStrip->AddRef();
			}
			return S_OK;
		}
		dwEnum--;
	}
	*ppStrip = NULL;
	return E_FAIL;
}


//  @method HRESULT | IDMUSProdTimeline | InsertStripAtPos | This method inserts a strip at the
//		specified position.
//
//  @parm   <i IDMUSProdStrip>* | pIStrip | The strip to add to the Timeline.  The strip must not
//		have been previously added to the Timeline.
//  @parm   DWORD | dwPosition | The 0-based position to add the strip at.
//
//	@comm	No strip may be inserted before the Time Strip, the first strip in the Timeline.
//		Consequently, 0 is an invalid value for <p dwPosition>.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStrip>
//	@rvalue E_FAIL | The operation failed.
//	@rvalue E_OUTOFMEMORY | There was not enough available memory to complete the operation.
//	@rvalue E_INVALIDARG | dwPosition is 0.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <om IDMUSProdTimeline::RemoveStrip>,
//		<om IDMUSProdTimeline::InsertStripAtDefaultPos>, <om IDMUSProdTimeline::AddStrip>
HRESULT CTimelineCtl::InsertStripAtPos(
		/* [in] */  IDMUSProdStrip*	pStrip,
		/* [in] */  DWORD		    dwPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pStrip != NULL );
	if( pStrip == NULL )
	{
		return E_POINTER;
	}

	DMUS_IO_TRACK_HEADER ioTrackHeader;
	ZeroMemory( &ioTrackHeader, sizeof(DMUS_IO_TRACK_HEADER) );

	VARIANT varStripMgr;
	if( SUCCEEDED( pStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) )
	&&	(varStripMgr.vt == VT_UNKNOWN) && (V_UNKNOWN(&varStripMgr) != NULL) )
	{
		IDMUSProdStripMgr* pIStripMgr;
		if( SUCCEEDED( V_UNKNOWN(&varStripMgr)->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pIStripMgr ) ) )
		{
			VARIANT varTrackHeader;
			varTrackHeader.vt = VT_BYREF;
			V_BYREF(&varTrackHeader) = &ioTrackHeader;
			if( FAILED( pIStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &varTrackHeader ) ) )
			{
				TRACE("Timeline: Unable to get StripMgr's TrackHeader\n");
			}

			pIStripMgr->Release();
		}
		V_UNKNOWN(&varStripMgr)->Release();
	}

	return InternalInsertStripAtPos( pStrip, dwPosition, ioTrackHeader.guidClassID, ioTrackHeader.dwGroup, ioTrackHeader.dwPosition );
}


//  @method HRESULT | IDMUSProdTimeline | StripToWindowPos | This method converts a point from strip
//		coordinates to Timeline Window coordinates
//
//  @parm   <i IDMUSProdStrip>* | pIStrip | The strip from whose coordinates to convert from.
//  @parm   POINT* | pPoint | Address of the point to convert.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pPoint> or <p pIStrip>
//	@rvalue E_INVALIDARG | <p pIStrip> was not previously added to the Timeline
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>
HRESULT CTimelineCtl::StripToWindowPos(
		/* [in] */		IDMUSProdStrip*	pIStrip,
		/* [in,out] */	POINT*			pPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( NULL == pIStrip || NULL == pPoint )
	{
		return E_POINTER;
	}

	if (m_pStripList == NULL)
	{
		return E_INVALIDARG;
	}

	StripList* pTempSL;
	// subtract the heights of any strips above this one
	for( pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
	{
		if( pIStrip == pTempSL->m_pStrip )
		{
			return StripPointToClient( pTempSL, pPoint );
		}
	}
	return E_INVALIDARG;
}


//  @method HRESULT | IDMUSProdTimeline | AddToNotifyList | This method enables a StripMgr to receive
//		notifications of type <p rguidType> send to at least one group of <p dwGroupBits>
//
//  @parm   <i IDMUSProdStripMgr>* | pIStripMgr | The strip manager to add.
//  @parm   REFGUID | rguidType | Reference to the identifier of the notification type to start
//		receiving notifications for.  See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible notifications.  Strips can also define
//		their own types for custom notifications.
//  @parm   DWORD | dwGroupBits | Which track group(s) to receive notifications for.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. To receive all notifications
//		of the type specified by <p rguidType> regardless of groups, set this parameter to 0xFFFFFFFF. 
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStripMgr>
//	@rvalue E_OUTOFMEMORY | Not enough memory available
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdStripMgr::OnUpdate>, <om IDMUSProdTimeline::RemoveFromNotifyList>,
//		<om IDMUSProdTimeline::NotifyStripMgrs>
HRESULT CTimelineCtl::AddToNotifyList(
		/* [in] */	IDMUSProdStripMgr*	pIStripMgr,
		/* [in] */	REFGUID				rguidType,
		/* [in] */	DWORD				dwGroupBits)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( pIStripMgr == NULL )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	NotifyEntry* pNotifyEntry = NULL;
	BOOL fFound = FALSE;
	NotifyListEntry* pNotifyListEntry = NULL;

	try
	{
		// Look through m_lstNotifyEntry for guidNotify
		POSITION pos;
		pos = m_lstNotifyEntry.GetHeadPosition();
		while ( pos != NULL && !fFound )
		{
			pNotifyEntry = m_lstNotifyEntry.GetNext( pos );
			if ( InlineIsEqualGUID( pNotifyEntry->m_guid, rguidType ) )
			{
				// Found guidNotify, add pNotifyListEntry to the end of the list
				fFound = TRUE;

				pNotifyListEntry = new NotifyListEntry;
				pNotifyListEntry->pIStripMgr = pIStripMgr;
				//pIStripMgr->AddRef();
				pNotifyListEntry->dwGroupBits = dwGroupBits;

				pNotifyEntry->m_lstNotifyListEntry.AddTail( pNotifyListEntry );
			}
		}

		// Didn't find guidNotify, add a new NotifyEntry to the end of m_lstNotifyEntry
		if ( !fFound )
		{
			pNotifyEntry = new NotifyEntry( pIStripMgr, rguidType, dwGroupBits );
			m_lstNotifyEntry.AddTail( pNotifyEntry );
		}
	}
	catch( CMemoryException *pMemoryException )
	{
		if( pNotifyListEntry )
		{
			delete pNotifyListEntry;
		}
		if( pNotifyEntry )
		{
			delete pNotifyEntry;
		}
		pMemoryException->Delete();
		return E_OUTOFMEMORY;
	}
		
	return S_OK;
}


//  @method HRESULT | IDMUSProdTimeline | RemoveFromNotifyList | This method stops a StripMgr from
//		receiving notifications of type <p rguidType> for groups in <p dwGroupBits>
//
//  @parm   <i IDMUSProdStripMgr>* | pIStripMgr | The strip manager to remove.
//  @parm   REFGUID | rguidType |  Reference to the identifier of the notification type to stop
//		receiving notifications for.  See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible notifications.  Strips can also define
//		their own types for custom notifications.
//  @parm   DWORD | dwGroupBits | Which track group(s) to stop receiving notifications for.  A value of
//		0 is invalid. Each bit in <p dwGroupBits> corresponds to a track group. To stop receiving all notifications
//		of the type specified by <p rguidType> regardless of groups, set this parameter to 0xFFFFFFFF. 
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p pIStripMgr>
//	@rvalue E_INVALIDARG | The specified <p pIStripMgr> can not be found
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdStripMgr::OnUpdate>, <om IDMUSProdTimeline::AddToNotifyList>,
//		<om IDMUSProdTimeline::NotifyStripMgrs>
HRESULT CTimelineCtl::RemoveFromNotifyList(
		/* [in] */	IDMUSProdStripMgr*	pIStripMgr,
		/* [in] */	REFGUID				rguidType,
		/* [in] */	DWORD				dwGroupBits)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if ( pIStripMgr == NULL )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	NotifyEntry* pNotifyEntry = NULL;
	BOOL fFound = FALSE;

	// Look through m_lstNotifyEntry for guidNotify
	POSITION pos;
	pos = m_lstNotifyEntry.GetHeadPosition();
	while ( pos != NULL && !fFound )
	{
		pNotifyEntry = m_lstNotifyEntry.GetNext( pos );
		if ( InlineIsEqualGUID( pNotifyEntry->m_guid, rguidType ) )
		{
			// Found guidNotify, now find pIStripMgr
			NotifyListEntry* pNotifyListEntry = NULL;
			POSITION pos2;
			pos2 = pNotifyEntry->m_lstNotifyListEntry.GetHeadPosition( );
			while ( pos2 != NULL && !fFound )
			{
				POSITION pos3 = pos2;
				pNotifyListEntry = pNotifyEntry->m_lstNotifyListEntry.GetNext( pos2 );
				if( pNotifyListEntry->pIStripMgr == pIStripMgr &&
					pNotifyListEntry->dwGroupBits == dwGroupBits )
				{
					// We've found it, now remove it from the list and delete our entry
					fFound = TRUE;
					pNotifyEntry->m_lstNotifyListEntry.RemoveAt( pos3 );
					delete pNotifyListEntry;

					// Check if the list is empty
					if ( pNotifyEntry->m_lstNotifyListEntry.IsEmpty() )
					{
						// If the list is empty, remove it from m_lstNotifyEntry
						pos2 = m_lstNotifyEntry.Find( pNotifyEntry );
						ASSERT( pos2 );
						if ( pos2 )
						{
							m_lstNotifyEntry.RemoveAt( pos2 );
							delete pNotifyEntry;
						}
					}
				}
			}
		}
	}

	// Didn't find guidNotify, add a new NotifyEntry to the end of m_lstNotifyEntry
	if ( !fFound )
	{
		return E_INVALIDARG;
	}
	return S_OK;
}


//  @method HRESULT | IDMUSProdTimeline | NotifyStripMgrs | This method broadcasts the specified
//		notification to all registered strip managers that belong to at least one of <p dwGroupBits>
//
//  @parm   REFGUID | rguidType |  Reference to the identifier of the notification type to send.
//		See <t SegmentGUIDs> and the list of track parameter types
//		in the DirectX documentation for a list of possible notifications.  Strips can also define
//		their own types for custom notifications.
///  @parm   DWORD | dwGroupBits | Which track group(s) to notify.  A value of 0 is invalid. Each bit in
//		<p dwGroupBits> corresponds to a track group.  To notify all strip managers that asked to hear
//		notifications of the type specified by <p rguidType> regardless of groups, set this parameter to 0xFFFFFFFF. 
//
//  @rvalue S_OK | The operation was successful
//	@rvalue E_FAIL | No matching strip managers were found, so no notifications were sent
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdStripMgr::OnUpdate>, <om IDMUSProdTimeline::AddToNotifyList>,
//		<om IDMUSProdTimeline::RemoveFromNotifyList>
HRESULT CTimelineCtl::NotifyStripMgrs(
		/* [in] */	REFGUID		rguidType,
		/* [in] */	DWORD		dwGroupBits,
		/* [in] */  void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	// If displaying realtime in status bar, update it when tempo changes
	if( m_pDMUSProdFramework && m_hKeyRealTimeStatusBar
	&&	((rguidType == GUID_TempoParam) || (rguidType == GUID_Segment_AllTracksAdded)) )
	{
		// Initialize status bar panes
		REFERENCE_TIME rtRefTime = 0;
		CString strText;
		ClocksToRefTime( m_lCursor, &rtRefTime );
		RefTimeToString( rtRefTime, IDS_OFFSET_TIME, IDS_OFFSET_NEG_TIME, strText );
		m_pDMUSProdFramework->SetStatusBarPaneText( m_hKeyRealTimeStatusBar, 1, strText.AllocSysString(), TRUE );

		rtRefTime = 0;
		if( m_pIDMPerformance && (m_rtSegmentStart != 0)
		&&	SUCCEEDED( m_pIDMPerformance->GetTime( &rtRefTime, NULL ) ) )
		{
			rtRefTime -= m_rtSegmentStart;
			RefTimeToString( rtRefTime, IDS_ELAPSED_TIME, IDS_ELAPSED_NEG_TIME, strText );
		}
		else if( m_rtSegmentStart == 0 )
		{
			// Display the last value of the ElapsedTime counter
			RefTimeToString( m_rtLastElapsedTime, IDS_ELAPSED_TIME, IDS_ELAPSED_NEG_TIME, strText );
		}
		m_pDMUSProdFramework->SetStatusBarPaneText( m_hKeyRealTimeStatusBar, 0, strText.AllocSysString(), TRUE );
	}

	// Find the NotifyEntry corresponding to rguidType
	NotifyEntry* pNotifyEntry = NULL;
	BOOL fFound = FALSE;
	POSITION pos;
	pos = m_lstNotifyEntry.GetHeadPosition();
	while ( pos != NULL && !fFound )
	{
		pNotifyEntry = m_lstNotifyEntry.GetNext( pos );
		if ( InlineIsEqualGUID( pNotifyEntry->m_guid, rguidType ) )
		{
			VARIANT var;

			BOOL fOrigFreezeUndo = FALSE;
			if( SUCCEEDED( GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
			{
				fOrigFreezeUndo = V_BOOL(&var);
			}

			// Freeze undo queue
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			SetTimelineProperty( TP_FREEZE_UNDO, var );

			// Found the NotifyEntry corresponding to rguidType
			// Now, call OnUpdate for all matching StripMgrs
			HRESULT hr = E_FAIL;
			POSITION pos2;
			NotifyListEntry *pNotifyListEntry;
			pos2 = pNotifyEntry->m_lstNotifyListEntry.GetHeadPosition();
			while ( pos2 != NULL )
			{
				pNotifyListEntry = pNotifyEntry->m_lstNotifyListEntry.GetNext( pos2 );
				ASSERT( pNotifyListEntry );
				if ( pNotifyListEntry && (pNotifyListEntry->dwGroupBits & dwGroupBits) )
				{
					ASSERT( pNotifyListEntry->pIStripMgr );
					if( pNotifyListEntry->pIStripMgr )
					{
						fFound = TRUE;
						HRESULT hr2;
						hr2 = pNotifyListEntry->pIStripMgr->OnUpdate( rguidType, pNotifyListEntry->dwGroupBits & dwGroupBits, pData );
						if( (hr != S_OK) && (FAILED( hr ) || SUCCEEDED( hr2 )) )
						{
							hr = hr2;
						}
					}
				}
			}

			// Restore undo queue
			var.vt = VT_BOOL;
			V_BOOL(&var) = (short)fOrigFreezeUndo;
			SetTimelineProperty( TP_FREEZE_UNDO, var );

			return hr;
		}
	}
	return E_FAIL;
}


//  @method HRESULT | IDMUSProdTimeline | AllocTimelineDataObject | This method allocates an object that
//		implements the <i IDMUSProdTimelineDataObject> interface.
//
//  @parm	<i IDMUSProdTimelineDataObject> | ppITimelineDataObject | Address of a variable to receive a pointer
//		to the TimelineDataObject.
//
//	@comm	A Timeline DataObject simplifies dealing with the clipboard, and allows the Timeline to manage
//		multiple-strip copy and paste operations.
//
//  @rvalue S_OK | The operation was successful
//	@rvalue E_OUTOFMEMORY | Not enough memory available to allocate a TimelineDataObject
//	@rvalue E_POINTER | <p ppITimelineDataObject> is NULL
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdTimelineDataObject>
HRESULT CTimelineCtl::AllocTimelineDataObject(
		/* [out,retval] */ IDMUSProdTimelineDataObject**	ppITimelineDataObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Validate ppITimelineDataObject
	if( ppITimelineDataObject == NULL )
	{
		return E_POINTER;
	}

	// Initialize variables
	HRESULT hr = S_OK;
	CTimelineDataObject *pTimelineDataObject = NULL;

	// Try and allocate a CTimelineDataObject object
	try
	{
		pTimelineDataObject = new CTimelineDataObject;
	}
	// Catch out of memory exception
	catch( CMemoryException *pMemException )
	{
		hr = E_OUTOFMEMORY;
		pMemException->Delete();
	}

	// If successful, QI for an IDMUSProdTimelineDataObject to store in ppITimelineDataObject
	if( SUCCEEDED(hr) && pTimelineDataObject )
	{
		hr = pTimelineDataObject->QueryInterface( IID_IDMUSProdTimelineDataObject, (void**)ppITimelineDataObject );
		pTimelineDataObject->Release();
	}
	
	return hr;
}


//  @method HRESULT | IDMUSProdTimeline | GetPasteType | This method returns the type of Paste operation
//		to perform.
//
//  @parm   <t TIMELINE_PASTE_TYPE>* | ptlptPasteType | Address of a variable to store the
//		<t TIMELINE_PASTE_TYPE> in.
//
//  @rvalue S_OK | The operation was successful
//	@rvalue E_POINTER | <p ptlptPasteType> is NULL
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::SetPasteType>
HRESULT CTimelineCtl::GetPasteType(
		/* [out,retval] */ TIMELINE_PASTE_TYPE*	ptlptPasteType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Validate ptlptPasteType
	if( ptlptPasteType == NULL )
	{
		return E_POINTER;
	}

	*ptlptPasteType = m_ptPasteType;

	return S_OK;
}


//  @method HRESULT | IDMUSProdTimeline | SetPasteType | This method sets the type of Paste operation
//		to perform.
//
//  @parm   <t TIMELINE_PASTE_TYPE> | tlptPasteType | The type of <t TIMELINE_PASTE_TYPE> to set
//
//  @rvalue S_OK | The operation was successful
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::GetPasteType>
HRESULT CTimelineCtl::SetPasteType(
		/* [in] */ TIMELINE_PASTE_TYPE tlptPasteType )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_ptPasteType = tlptPasteType;

	return S_OK;
}


void CTimelineCtl::ScrollToTimeCursor( long lTime, long lLeftSide, long lRightSide )
{
	// Convert from a time to a pixel position
	long lPosition;
	ClocksToPosition( lTime, &lPosition );
	lPosition += m_lLastEarlyPosition;

	// Scroll backward in time (if the cursor is off the left edge of the display)
	if( lPosition < m_lXScrollPos )
	{
		ScrollToPosition( lPosition );
	}

	// Check to see if there is new music that we can scroll to.

	// Compute the length of the segment, in pixels
	long lMaxPos;
	ClocksToPosition( m_lLength, &lMaxPos );

	// Add the early and late amounts
	lMaxPos += m_lLastEarlyPosition + m_lLastLatePosition;

	// Check if the right edge of the display is earlier than the last visible pixel
	if( lRightSide + m_lXScrollPos < lMaxPos + m_lFunctionBarWidth )
	{
		// yes - at least one pixel exists beyond the right edge of the display

		// scroll after we've covered 9/10 of the window
		if( lPosition + CURSOR_WIDTH - m_lXScrollPos > (long)(( lRightSide - lLeftSide ) * .9) )
		{
			// Scroll so that lPosition displays 2/10 of the way from the left border
			long lScrollTo = lPosition - (long)(( lRightSide - lLeftSide ) * .2 );

			// Ensure lScrollTo is less than the maximum scroll position
			lScrollTo = min( lScrollTo, lMaxPos - (lRightSide - lLeftSide) );

			// Finally, scroll to it
			ScrollToPosition( lScrollTo );
		}
	}
}

void CTimelineCtl::SetTimeCursor( long lTime, BOOL fScroll, bool fUserSetCursor )
{
	// Check if the length is greater than 0
	if( m_lLength > 0 )
	{
		// if time is past the length, wrap around
		if( lTime > m_lLength + m_lLastLateTime )
		{
			lTime %= m_lLength + m_lLastLateTime;
		}
	}
	else
	{
		// Length is 0, so lTime must also be 0
		lTime = 0;
	}

	// If the cursor time actually changed
	if( lTime != m_lCursor )
	{
		// Compute the drawing area for the strips
		RECT rect;
		ComputeDrawingArea( &rect );

		// If the user isn't scrolling with the horizontal scroll bar, if fScroll is set,
		// and if the mouse isn't captured
		if( !m_fHScrollTracking
		&&	fScroll
		&&	!m_fStripMouseCapture )
		{
			// Scroll the display so the time cursor is visible
			ScrollToTimeCursor( lTime, rect.left, rect.right );
		}

		// Compute the old position of the cursor
		long lOldPosition;
		ClocksToPosition( m_lCursor, &lOldPosition );
		lOldPosition = lOldPosition + m_lFunctionBarWidth + m_lLastEarlyPosition - m_lXScrollPos;

		// Compute the new position of the cursor
		long lNewPosition;
		ClocksToPosition( lTime, &lNewPosition );
		lNewPosition = lNewPosition + m_lFunctionBarWidth + m_lLastEarlyPosition - m_lXScrollPos;

		// If either the new cursor position or the old cursor position is visible, redraw the cursor
		if( (lNewPosition >= m_lFunctionBarWidth)
		||	(lOldPosition >= m_lFunctionBarWidth) )
		{
			// Get our device context
			HDC hdc = GetDC();
			if( hdc )
			{
				// Create the pen to draw the time cursor with
				HPEN hpen;
				hpen = ::CreatePen( PS_SOLID, 1, COLOR_TIME_CURSOR );
				if( hpen )
				{
					// Save the currently used pen and select the time cursor pen
					HPEN hpenOld;
					hpenOld = static_cast<HPEN>(::SelectObject( hdc, hpen ));

					// Save the currently used ROP and select the time cursor ROP
					int nOldROP;
					nOldROP = ::SetROP2( hdc, R2_XORPEN );

					// If the old cursor is visible
					if ( (lOldPosition >= m_lFunctionBarWidth) && (lOldPosition <= rect.right) )
					{
						// Draw over it (this should erase it)
						::MoveToEx( hdc, lOldPosition, rect.top, NULL );
						::LineTo( hdc, lOldPosition, rect.bottom );
					}

					// If the new cursor is visible
					if ( (lNewPosition >= m_lFunctionBarWidth) && (lNewPosition <= rect.right) )
					{
						// Draw it
						::MoveToEx( hdc, lNewPosition, rect.top, NULL );
						::LineTo( hdc, lNewPosition, rect.bottom );
					}

					// Restore the old ROP and pen
					::SetROP2( hdc, nOldROP );
					::SelectObject( hdc, hpenOld );

					// Delete the time cursor pen
					::DeleteObject( hpen );
				}

				// Release the DC
				ReleaseDC( hdc );
			}
		}

		// Update the time of the cursor
		m_lCursor = lTime;

		// Check if we're displaying the real-time in the status bar
		if( m_pDMUSProdFramework && m_hKeyRealTimeStatusBar )
		{
			// Yes - update the text in the status bar panes

			// Get the REFERENCE_TIME of the cursor
			REFERENCE_TIME rtRefTime = 0;
			ClocksToRefTime( m_lCursor, &rtRefTime );

			// Convert from a number to a string
			CString strText;
			RefTimeToString( rtRefTime, IDS_OFFSET_TIME, IDS_OFFSET_NEG_TIME, strText );

			// Update the status bar text
			m_pDMUSProdFramework->SetStatusBarPaneText( m_hKeyRealTimeStatusBar, 1, strText.AllocSysString(), TRUE );

			// Now, try and get the offset from the start of the segment
			rtRefTime = 0;

			// Check if the segment is playing
			if( m_pIDMPerformance && (m_rtSegmentStart != 0)
			&&	SUCCEEDED( m_pIDMPerformance->GetTime( &rtRefTime, NULL ) ) )
			{
				// Yes - set rtRefTime to the current offset in the segment
				rtRefTime -= m_rtSegmentStart;
			}
			else if( m_rtSegmentStart == 0 )
			{
				// Display the last value of the ElapsedTime counter
				rtRefTime = m_rtLastElapsedTime;
			}

			// Convert from a number to a string
			RefTimeToString( rtRefTime, IDS_ELAPSED_TIME, IDS_ELAPSED_NEG_TIME, strText );

			// Update the status bar text
			m_pDMUSProdFramework->SetStatusBarPaneText( m_hKeyRealTimeStatusBar, 0, strText.AllocSysString(), TRUE );
		}

		// Notify the strips that the time cursor changed
		NotifyStripMgrs( fUserSetCursor ? GUID_TimelineUserSetCursor : GUID_TimelineSetCursor, 0xFFFFFFFF, &lTime );
	}
}

void CTimelineCtl::InvalidateSelectedStrips(BOOL fErase)
{
	UNREFERENCED_PARAMETER( fErase );
	StripList* pSL;
	RECT rect;
	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( pSL->m_fSelected )
		{
			GetStripClientRect( pSL, &rect );
//			InvalidateRect( &rect, fErase );
			InvalidateRect( &rect, FALSE );
		}
	}
}

//  @method HRESULT | IDMUSProdTimeline | SetMarkerTime | This method sets the location of
//		one of the markers in the time strip.
//
//  @parm   <t MARKERID> | idMarkerType | Which marker to get the location of.  Must be
//		one of <t MARKERID>.  MARKER_LEFTDISPLAY and MARKER_RIGHTDISPLAY are not supported.
//  @parm   <t TIMETYPE> | ttType | Which units to use when computing <p plTime>.  Must be
//		one of <t TIMETYPE>.
//  @parm   long | lTime | The time the marker should be set to.
//
//  @rvalue S_OK | The operation was successful.
//	@rvalue E_INVALIDARG | <p ttType> or <p idMarkerType> do not contain valid values, or
//		lTime is less than zero.
//
//	@xref	<i IDMUSProdTimeline>, <t TIMETYPE>, <t MARKERID>, <om IDMUSProdTimeline::GetMarkerTime>
HRESULT CTimelineCtl::SetMarkerTime( 
    /* [in] */ MARKERID idMarkerType,
	/* [in] */ TIMETYPE	ttType,
    /* [in] */ long lTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	StripList*	pSL;
	VARIANT		var;

	// If in milliseconds, convert to clocks
	if( ttType == TIMETYPE_MS )
	{
		RefTimeToClocks( lTime * 10000, &lTime );
	}

	switch( idMarkerType )
	{
	case MARKER_CURRENTTIME:		// signifies the time cursor
		if( m_pTimeStrip )
		{
			SetTimeCursor( lTime, m_pTimeStrip->m_fScrollWhenSettingTimeCursor, false );
		}
		break;

	case MARKER_BEGINSELECT:		// begin of edit selection
		if( lTime < 0 )
			return E_INVALIDARG;

		if( m_bPasting )
		{
			break;
		}
		m_lBeginSelect = lTime;
		var.vt = VT_I4;
		V_I4( &var ) = lTime;
		for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
		{
			if( pSL->m_pStrip != NULL )
			{
				/*
				if( !fSetMarkerTime_Ignore_m_pMouseStripList
				||	pSL != m_pMouseStripList )
				{
				*/
				pSL->m_pStrip->SetStripProperty( SP_BEGINSELECT, var);
				/*
				}
				*/
			}
		}
		break;

	case MARKER_ENDSELECT:		// end of edit selection
		if( lTime < 0 )
			return E_INVALIDARG;

		if( m_bPasting )
		{
			break;
		}
		m_lEndSelect = lTime; // note that end select isn't guaranteed to be > begin select
		var.vt = VT_I4;
		V_I4( &var ) = lTime;
		for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
		{
			if( pSL->m_pStrip != NULL )
			{
				/*
				if( !fSetMarkerTime_Ignore_m_pMouseStripList
				||	pSL != m_pMouseStripList )
				{
				*/
				pSL->m_pStrip->SetStripProperty( SP_ENDSELECT, var);
				/*
				}
				*/
			}
		}
		break;

	case MARKER_LEFTDISPLAY:		// scroll the display area to this time
	case MARKER_RIGHTDISPLAY:
		return E_NOTIMPL;
		break;

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | GetMarkerTime | This method gets the location of
//		one of the markers in the time strip.
//
//  @parm   <t MARKERID> | idMarkerType | Which marker to get the location of.  Must be
//		one of <t MARKERID>
//  @parm   <t TIMETYPE> | ttType | Which units to use when computing <p plTime>.  Must be
//		one of <t TIMETYPE>.
//  @parm   long* | plTime | Address of a variable to receive the location of <p idMarkerType>.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | <p plTime> is NULL.
//	@rvalue E_INVALIDARG | <p ttType> or <p idMarkerType> do not contain valid values.
//
//	@xref	<i IDMUSProdTimeline>, <t TIMETYPE>, <t MARKERID>, <om IDMUSProdTimeline::SetMarkerTime>
HRESULT CTimelineCtl::GetMarkerTime( 
    /* [in] */ MARKERID idMarkerType,
	/* [in] */ TIMETYPE	ttType,
    /* [out] */ long *plTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( plTime == NULL )
	{
		return E_POINTER;
	}

	if( ttType == TIMETYPE_MS )
	{
		// Use reference time
		REFERENCE_TIME rtTime;
		switch( idMarkerType )
		{
		case MARKER_CURRENTTIME:		// signifies the time cursor
			ClocksToRefTime( m_lCursor, &rtTime );
			*plTime = long(rtTime / 10000);
			break;

		case MARKER_BEGINSELECT:		// begin of edit selection
			ClocksToRefTime( m_lBeginSelect, &rtTime );
			*plTime = long(rtTime / 10000);
			break;

		case MARKER_ENDSELECT:			// end of edit selection
			ClocksToRefTime( m_lEndSelect, &rtTime );
			*plTime = long(rtTime / 10000);
			break;

		case MARKER_LEFTDISPLAY:		// left edge of display window
			PositionToRefTime( m_lXScrollPos - m_lLastEarlyPosition, &rtTime );
			*plTime = long(rtTime / 10000);
			break;

		case MARKER_RIGHTDISPLAY:		// right edge of display window
			{
				RECT rectDrawingArea;
				ComputeDrawingArea( &rectDrawingArea );
				PositionToRefTime( m_lXScrollPos - m_lLastEarlyPosition + rectDrawingArea.right - rectDrawingArea.left, &rtTime );
				*plTime = long(rtTime / 10000);
			}
			break;

		default:
			return E_INVALIDARG;
		}
	}
	else
	{
		// Use clocks
		switch( idMarkerType )
		{
		case MARKER_CURRENTTIME:
			*plTime = m_lCursor;		// signifies the time cursor
			break;

		case MARKER_BEGINSELECT:		// begin of edit selection
			*plTime = m_lBeginSelect;
			break;

		case MARKER_ENDSELECT:			// end of edit selection
			*plTime = m_lEndSelect;
			break;

		case MARKER_LEFTDISPLAY:		// left edge of display window
			PositionToClocks( m_lXScrollPos - m_lLastEarlyPosition, plTime );
			break;

		case MARKER_RIGHTDISPLAY:		// right edge of display window
			{
				RECT rectDrawingArea;
				ComputeDrawingArea( &rectDrawingArea );
				PositionToClocks( m_lXScrollPos - m_lLastEarlyPosition + rectDrawingArea.right - rectDrawingArea.left, plTime );
			}
			break;

		default:
			return E_INVALIDARG;
		}
	}
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | ClocksToPosition | This method converts
//		from a time in clocks to a horizontal pixel position
//
//	@comm	Negative values for <p lTime> are valid.
//
//	@comm	Due to rounding errors, converting from a time to a pixel position and back
//		will usually return a value different from the original one.
//
//  @parm   long | lTime | The time in clocks
//  @parm   long* | plPosition | Address of a variable to receive the pixel position this
//		time resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p plPosition>
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::PositionToClocks>
HRESULT CTimelineCtl::ClocksToPosition( 
    /* [in] */ long   lTime,
    /* [out] */ long *plPosition)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( NULL == plPosition )
	{
		return E_POINTER;
	}
	if (m_fMusicTime == TRUE) 
	{
		*plPosition = (long)floor( double(lTime) * m_dblZoom + 0.5 );
		return S_OK;
	}
	else
	{
		REFERENCE_TIME rTime;
		HRESULT hr = ClocksToRefTime( lTime, &rTime);
		if (SUCCEEDED(hr))
		{
			return RefTimeToPosition( rTime, plPosition);
		}
		return hr;
	}
}

//  @method HRESULT | IDMUSProdTimeline | PositionToClocks | This method converts
//		from a horizontal pixel position to a time in clocks.
//
//	@comm	Negative values for <p lPosition> are valid.
//
//	@comm	Due to rounding errors, converting from a pixel position to a time and back
//		may return a value different from the original one.
//
//  @parm   long | lPosition | The horizontal position, in pixels.
//  @parm   long* | plTime | Address of a variable to receive the time in clocks this
//		position resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p plTime>
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::ClocksToPosition>
HRESULT CTimelineCtl::PositionToClocks( 
    /* [in] */ long position,
    /* [out] */ long *plTime)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( NULL == plTime )
	{
		return E_POINTER;
	}
	if (m_fMusicTime == TRUE)
	{
		double dblPosition;
		double dblTime;
		dblPosition = position;
		dblTime = dblPosition / m_dblZoom;
		*plTime = (long)floor(dblTime + 0.5);
		return S_OK;
	}
	else
	{
		REFERENCE_TIME rTime;
		HRESULT hr = PositionToRefTime( position, &rTime);
		if (SUCCEEDED(hr))
		{
			return RefTimeToClocks( rTime, plTime);
		}
		return hr;
	}
}

void CTimelineCtl::OptimizeZoomFactor(void)
{
	// examine the clocklength of the piece. If the final position is a fraction
	// of the overall window real estate, zoom in until it fills the window

	// Compute the pixel length of the timeline
	long position;
	ClocksToPosition( m_lLength, &position );
	position += m_lLastEarlyPosition + m_lLastLatePosition;

	// Get the client timeline window (without the scrollbars)
	RECT rectClient;
	GetWindowRect(&rectClient);

	long desired = (rectClient.right - rectClient.left ) * 3 / 8;
	if( position < desired )
	{
		// Convert from a pixel length to a clock length
		PositionToClocks( position, &position );

		// Set the zoom factor
		m_dblZoom = (double)desired / double(position);
	}
}

static HRESULT GetVT_I4( VARIANT var, long* plVal )
{
	if( var.vt != VT_I4 )
		return E_FAIL;
	*plVal = V_I4(&var);
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | SetTimelineProperty | This method sets a property of
//		the Timeline.
//
//	@comm	The TP_SNAPAMOUNT property is not supported.
//
//  @parm   <t TIMELINE_PROPERTY> | tp | Which property to get.  Must be one of <t TIMELINE_PROPERTY>.
//  @parm   VARIANT | var | The data to set the property with.
//
//  @rvalue S_OK | The operation was successful.
//	@rvalue E_INVALIDARG | <p tp> does not contain a valid property type.
//	@rvalue E_FAIL | <p var> contained an invalid value for the specified property type.
//
//	@xref	<i IDMUSProdTimeline>, <t TIMELINE_PROPERTY>, <om IDMUSProdTimeline::GetTimelineProperty>
HRESULT CTimelineCtl::SetTimelineProperty(
		/* [in] */ TIMELINE_PROPERTY	tp,
		/* [in] */ VARIANT				var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = E_FAIL;
	long	lVal;

	switch(tp)
	{
	case TP_CLOCKLENGTH:
		if( SUCCEEDED( hr = GetVT_I4( var, &lVal )))
		{
			if( (lVal >= 0) && (m_lLength != lVal) )
			{
				m_lLength = lVal;
				OptimizeZoomFactor();
				ComputeScrollBars();

				// send WM_SIZE to all strips, letting them know they have changed size
				StripList* pSL = m_pStripList;
				while (pSL)
				{
					// BUGBUG: Should have meaningful values for lParam.
					CallStripWMMessage( pSL, WM_SIZE, SIZE_RESTORED, MAKELONG(0, pSL->m_lHeight) );
					pSL = pSL->m_pNext;
				}

				hr = S_OK;
			}
		}
		break;

	case TP_STRIPMOUSECAPTURE:
		if( V_BOOL(&var) == TRUE )
		{
			m_fStripMouseCapture = TRUE;
			SetCapture();
			// Make sure the active strip is getting the mouse messages
			if( m_pActiveStripList )
			{
				m_pMouseStripList = m_pActiveStripList;
				m_MouseMode = MM_UNKNOWN;
			}
			hr = S_OK;
		}
		else
		{
			m_fStripMouseCapture = FALSE;
			ReleaseCapture();
			// Change the active strip to whatever strip the mouse cursor is currently over
			BOOL fBool = FALSE;
			OnSetCursor( WM_SETCURSOR, 0, NULL, fBool );
			if ( m_pMouseStripList && (m_pMouseStripList != m_pActiveStripList) )
			{
				ActivateStripList( m_pMouseStripList );
			}
			hr = S_OK;
		}
		break;

	case TP_ACTIVESTRIP:
		{
			hr = E_INVALIDARG;
			IUnknown* punk;
			if( var.vt == VT_UNKNOWN )
			{
				punk = V_UNKNOWN(&var);
				if( punk )
				{
					IDMUSProdStrip* pIStrip;
					if( SUCCEEDED( punk->QueryInterface( IID_IDMUSProdStrip, (void**)&pIStrip ) ) )
					{
						StripList* pSL = FindStripList( pIStrip );
						if( pSL )
						{
							ActivateStripList( pSL );
							hr = S_OK;
						}
						pIStrip->Release();
					}
					punk->Release();
				}
				else
				{
					DeactivateStripList();
					hr = S_OK;
				}
			}
		}
		break;

	case TP_DMUSPRODFRAMEWORK:
		{
			IUnknown* punk;
			if( var.vt == VT_UNKNOWN )
			{
				if( m_pDMUSProdFramework )
				{
					m_pDMUSProdFramework->Release();
					m_pDMUSProdFramework = NULL;
				}
				punk = V_UNKNOWN(&var);
				if( punk )
				{
					if( SUCCEEDED( punk->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pDMUSProdFramework ) ) )
					{
						IDMUSProdComponent* pIComponent = NULL;
						if( SUCCEEDED ( m_pDMUSProdFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ))
						{
							IDMUSProdConductor *pIConductor;
							if( SUCCEEDED( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&pIConductor ) ) )
							{
								if( m_pIDMPerformance )
								{
									m_pIDMPerformance->Release();
									m_pIDMPerformance = NULL;
								}
								IUnknown* punkPerformance;
								if( SUCCEEDED( pIConductor->GetPerformanceEngine( &punkPerformance ) ) )
								{
									punkPerformance->QueryInterface( IID_IDirectMusicPerformance, (void **)&m_pIDMPerformance ) ;
									punkPerformance->Release();
								}
								pIConductor->Release();
							}
							pIComponent->Release();
						}
					}
				}
				hr = S_OK;
			}
		}
		break;

	case TP_TIMELINECALLBACK:
		{
			IUnknown* punk;
			if( var.vt == VT_UNKNOWN )
			{
				if( m_pTimelineCallback )
				{
					m_pTimelineCallback->Release();
					m_pTimelineCallback = NULL;
				}
				punk = V_UNKNOWN(&var);
				if( punk )
				{
					punk->QueryInterface( IID_IDMUSProdTimelineCallback, (void**)&m_pTimelineCallback );
				}
				hr = S_OK;
			}
		}
		break;

	case TP_ZOOM:
		if( var.vt == VT_R8 )
		{
			if ( V_R8(&var) > 0 )
			{
				m_dblZoom = V_R8(&var);
				hr = S_OK;
			}
		}
		break;

	case TP_HORIZONTAL_SCROLL:
		if( var.vt == VT_I4 )
		{
			ScrollToPosition( V_I4(&var) + m_lLastEarlyPosition );
			hr = S_OK;
		}
		break;

	case TP_VERTICAL_SCROLL:
		if( var.vt == VT_I4 )
		{
			hr = S_OK;
			if( m_lYScrollPos != V_I4(&var) )
			{
				// Get the height of all strips
				long lMaxStripScroll = TotalStripHeight();

				RECT rect;
				GetClientRect( &rect );

				RECT rectTmp;
				m_ScrollHorizontal.GetClientRect( &rectTmp );
				rect.bottom -= rectTmp.bottom;

				if( m_pStripList )
				{
					// don't scroll top strip (usually the time strip)
					rect.top += m_pStripList->m_lHeight + BORDER_HORIZ_WIDTH*2;

					// subtract off the top strip
					lMaxStripScroll -= m_pStripList->m_lHeight + BORDER_HORIZ_WIDTH;
				}

				// subtract the height of the viewing region
				lMaxStripScroll -= rect.bottom - rect.top;

				int nPos;
				if (lMaxStripScroll < 1)
				{
					nPos = 0;
				}
				else
				{
					nPos = (V_I4(&var) * m_ScrollVertical.GetScrollLimit()) / lMaxStripScroll;
				}

				int iTemp = 0;
				OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, nPos ), (LPARAM) ((HWND) m_ScrollVertical), iTemp );
			}
		}
		break;

	case TP_FREEZE_UNDO:
		if( var.vt == VT_BOOL )
		{
			m_fFreezeUndo = V_BOOL(&var);
			hr = S_OK;
		}
		break;

	case TP_SNAP_TO:
		if( var.vt == VT_I4 )
		{
			m_pTimeStrip->SetSnapTo( (DMUSPROD_TIMELINE_SNAP_TO) V_I4(&var) );
			hr = S_OK;
		}
		break;
	
	case TP_FUNCTIONBAR_WIDTH:
		if( var.vt == VT_I4 )
		{
			if(( V_I4(&var) >= MIN_FNBAR_WIDTH ) && ( V_I4(&var) <= MAX_FNBAR_WIDTH))
			{
				m_lFunctionBarWidth = V_I4(&var);
				hr = S_OK;

				// Probably should calculate the area that will be affected
				// and only invalidate it.
				// Perhaps we could just 'scroll' the affected area to the
				// right and invalidate the small new area to be displayed

				// send WM_SIZE to all strips, letting them know something has changed size
				StripList* pSL = m_pStripList;
				while (pSL)
				{
					// BUGBUG: Should have meaningful values for lParam.
					CallStripWMMessage( pSL, WM_SIZE, SIZE_RESTORED, MAKELONG(0, pSL->m_lHeight) );
					pSL = pSL->m_pNext;
				}

				ComputeScrollBars();
				RECT rect;
				GetClientRect( &rect );
//				InvalidateRect( &rect, TRUE );
				InvalidateRect( &rect, FALSE );
			}
		}
		break;

	case TP_TOP_STRIP:
		{
			hr = E_INVALIDARG;
			IUnknown* punk;
			if( var.vt == VT_UNKNOWN )
			{
				punk = V_UNKNOWN(&var);
				if( punk )
				{
					IDMUSProdStrip* pIStrip;
					if( SUCCEEDED( punk->QueryInterface( IID_IDMUSProdStrip, (void**)&pIStrip ) ) )
					{
						StripList* pSL = FindStripList( pIStrip );
						if( pSL )
						{
							SetTopStripList( pSL );
							hr = S_OK;
						}
						pIStrip->Release();
					}
					punk->Release();
				}
				else
				{
					DeactivateStripList();
					hr = S_OK;
				}
			}
		}
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | GetTimelineProperty | This method gets a property of
//		the Timeline.
//
//  @parm   <t TIMELINE_PROPERTY> | tp | Which property to get.  Must be one of <t TIMELINE_PROPERTY>.
//  @parm   VARIANT* | pVar | Address of the variant to return the property's data in.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | <p pVar> is NULL.
//	@rvalue E_INVALIDARG | <p tp> does not contain a valid property type.
//
//	@xref	<i DMUSProdTimeline>, <t TIMELINE_PROPERTY>, <om IDMUSProdTimeline::SetTimelineProperty>
HRESULT CTimelineCtl::GetTimelineProperty(
		/* [in] */ TIMELINE_PROPERTY	tp,
		/* [out] */ VARIANT*			pVar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;

	if( pVar == NULL )
	{
		return E_POINTER;
	}

	switch(tp)
	{
	case TP_CLOCKLENGTH:
		pVar->vt = VT_I4;
		V_I4(pVar) = m_lLength;
		break;

	case TP_TIMELINECALLBACK:
		pVar->vt = VT_UNKNOWN;
		V_UNKNOWN(pVar) = m_pTimelineCallback;
		if( m_pTimelineCallback )
		{
			m_pTimelineCallback->AddRef();
		}
		else
		{
			return E_FAIL;
		}
		break;

	case TP_DMUSPRODFRAMEWORK:
		pVar->vt = VT_UNKNOWN;
		V_UNKNOWN(pVar) = m_pDMUSProdFramework;
		if( m_pDMUSProdFramework )
		{
			m_pDMUSProdFramework->AddRef();
		}
		else
		{
			return E_FAIL;
		}
		break;

	case TP_ACTIVESTRIP:
		pVar->vt = VT_UNKNOWN;
		if( m_pActiveStripList && m_pActiveStripList->m_pStrip )
		{
			V_UNKNOWN(pVar) = m_pActiveStripList->m_pStrip;
			m_pActiveStripList->m_pStrip->AddRef();
		}
		else
		{
			V_UNKNOWN(pVar) = NULL;
			return E_FAIL;
		}
		break;
	
	case TP_FUNCTIONBAR_WIDTH:
		pVar->vt = VT_I4;
		V_I4(pVar) = m_lFunctionBarWidth;
		break;

	case TP_MAXIMUM_HEIGHT:
		{
			RECT rectWin, rectSH;
			GetClientRect( &rectWin );
			m_ScrollHorizontal.GetClientRect( &rectSH );
			pVar->vt = VT_I4;
			V_I4(pVar) = rectWin.bottom - m_pStripList->m_lHeight - rectSH.bottom - BORDER_HORIZ_WIDTH*3;
		}
		break;

	case TP_HORIZONTAL_SCROLL:
		pVar->vt = VT_I4;
		V_I4(pVar) = m_lXScrollPos - m_lLastEarlyPosition;
		break;

	case TP_VERTICAL_SCROLL:
		pVar->vt = VT_I4;
		V_I4(pVar) = m_lYScrollPos;
		break;

	case TP_ZOOM:
		pVar->vt = VT_R8;
		V_R8(pVar) = m_dblZoom;
		break;

	case TP_SNAPAMOUNT:
		if( m_pTimeStrip )
		{
			V_I4(pVar) = m_pTimeStrip->SnapAmount( V_I4(pVar) );
			pVar->vt = VT_I4;
		}
		else
		{
			return E_FAIL;
		}
		break;

	case TP_FREEZE_UNDO:
		pVar->vt = VT_BOOL;
		V_BOOL(pVar) = (short)m_fFreezeUndo;
		break;

	case TP_SNAP_TO:
		pVar->vt = VT_I4;
		V_I4(pVar) = m_pTimeStrip->m_stSetting;
		break;

	case TP_FUNCTIONNAME_HEIGHT:
		pVar->vt = VT_I4;
		V_I4(pVar) = FUNCTION_NAME_HEIGHT;
		break;

		/* TODO: Implement
	case TP_TOP_STRIP:
		pVar->vt = VT_UNKNOWN;
		{
			StripList* pSL = GetTopStripList();
			if( pSL && pSL->m_pStrip )
			{
				V_UNKNOWN(pVar) = pSL->m_pStrip;
				pSL->m_pStrip->AddRef();
			}
			else
			{
				V_UNKNOWN(pVar) = NULL;
				return E_FAIL;
			}
		}
		break;
		*/

	default:
		hr = E_INVALIDARG;
		break;
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | Refresh | This method causes a redraw of the
//		entire Timeline.
//
//	@comm	This method should be used sparingly.  If at all possible,
//		<om IDMUSProdTimeline::StripInvalidateRect> and <om IDMUSProdTimeline::NotifyStripMgrs>
//		should be used instead.
//
//  @rvalue S_OK | The operation was successful
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::StripInvalidateRect>, <om IDMUSProdTimeline::NotifyStripMgrs>
HRESULT CTimelineCtl::Refresh(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ComputeScrollBars();
//	InvalidateRect(NULL);
	InvalidateRect(NULL, FALSE);
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | ClocksToMeasureBeat | This method converts
//		from a time in clocks to a time in measures and beats
//
//	@comm	If <p lTime> is less than 0, the Time Signature at time 0 will
//			be used to compute <p plMeasure> and <p plBeat>.  <p plMeasure> will
//			contain the negative measure number, and <p plBeat> will contain
//			the beat in the measure that <p lTime> falls in.
//
//	@comm	Either <p plMeasure> or <p lBeat> may be NULL, but not both.
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm   long | lTime | The time in clocks
//  @parm   long* | plMeasure | Address of a variable to recieve the measure number, with
//		measure 0 as the first measure.
//  @parm   long* | plBeat | Address of a variable to receive the beat number, with beat
//		0 as the first beat in each measure.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p plMeasure> and <p plBeat>
//	@rvalue E_UNEXPECTED | The Time Signature was unable to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::MeasureBeatToClocks>
HRESULT CTimelineCtl::ClocksToMeasureBeat( 
	/* [in] */ DWORD  dwGroupBits,
	/* [in] */ DWORD  dwIndex,
    /* [in] */ long	  lTime,
    /* [out] */ long *plMeasure,
    /* [out] */ long *plBeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (NULL == plMeasure) && (NULL == plBeat) )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	DMUS_TIMESIGNATURE TimeSig;
	MUSIC_TIME mtTSCur = 0, mtTSNext;
	long lBeat = 0, lMeasure = 0;

	do
	{
		// Try and get the current time signature
		if ( FAILED( GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, mtTSCur, &mtTSNext, &TimeSig ) ) )
		{
			return E_UNEXPECTED;
		}

		// If lTime is less than 0, only use the first TimeSig
		if( lTime < 0 )
		{
			lMeasure += lTime / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));
			lTime = -(abs(lTime) % (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ )));
			break;
		}
		// If there is no next time signature, do the math to find how many more measures to add
		else if( mtTSNext == 0 )
		{
			lMeasure += lTime / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));
			lTime %= TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
			break;
		}
		// Otherwise it's more complicated
		else
		{
			// If the next time signature is after the time we're looking for
			if( lTime < mtTSNext )
			{
				// Add the number of complete measures between here and there
				lMeasure += lTime / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));

				// lTime now stores an offset from the beginning of the measure
				lTime %= TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
				break;
			}
			// The next time signature is before the time we're looking for
			else
			{
				// Compute how many complete measures there are between now and the next Time signature
				long lMeasureDiff= mtTSNext / (TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ));

				// Add them to lMeasure
				lMeasure += lMeasureDiff;

				// Change lMeasureDiff from measures to clocks
				lMeasureDiff *= TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );

				// Subtract from the time left (lTime) and add to the current time (mtTSCur)
				lTime -= lMeasureDiff;
				mtTSCur += lMeasureDiff;
			}
		}
	}
	// While the time left is greater than 0
	while ( lTime > 0 );

	if ( lTime < 0 )
	{
		lTime += TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
		lMeasure--;
	}

	if ( lTime != 0 && plBeat != NULL )
	{
		lBeat = lTime / NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
	}
	else
	{
		lBeat = 0;
	}

	if( plMeasure != NULL )
	{
		*plMeasure = lMeasure;
	}
	if( plBeat != NULL )
	{
		*plBeat = lBeat;
	}

	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | PositionToMeasureBeat | This method converts
//		from a horizontal pixel position to a measure and beat value.
//
//	@comm	If <p lPosition> is less than 0, the Time Signature at time 0 will
//			be used to compute <p plMeasure> and <p plBeat>.  <p plMeasure> will
//			contain the negative measure number, and <p plBeat> will contain
//			the beat in the measure that <p lTime> falls in.
//
//	@comm	Either <p plMeasure> or <p lBeat> may be NULL, but not both.
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm   long | lPosition | The horizontal pixel position.
//  @parm   long* | plMeasure | Address of a variable to receive the measure number, with
//		measure 0 as the first measure.
//  @parm   long* | plBeat | Address of a variable to receive the beat number, with beat
//		0 as the first beat in each measure.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p plMeasure> and <p plBeat>
//	@rvalue E_UNEXPECTED | The Time Signature was unable to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::MeasureBeatToPosition>
HRESULT CTimelineCtl::PositionToMeasureBeat( 
	/* [in] */ DWORD  dwGroupBits,
	/* [in] */ DWORD  dwIndex,
    /* [in] */ long   position,
    /* [out] */ long *plMeasure,
    /* [out] */ long *plBeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	long lTime;
	HRESULT hr;

	// Since this uses PositionToClocks, we don't care if we're in music time
	// or real time.  PositionToClocks will deal with it appropriately.
	if( SUCCEEDED( hr = PositionToClocks( position, &lTime ) ))
	{
		hr = ClocksToMeasureBeat( dwGroupBits, dwIndex, lTime, plMeasure, plBeat );
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | MeasureBeatToClocks | This method converts
//		from a measure and beat to a time in clocks.
//
//	@comm	If <p lMeasure> is less than 0, the Time Signature at time 0 will
//			be used to compute <p plTime>.  If <p lBeat> is less than 0, the
//			Time Signature in measure <p lMeasure> will be used to compute the offset
//			from the start of measure <p lMeasure>.
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm	long | lMeasure | The measure number, with measure 0 as the first measure.
//  @parm	long | lBeat | The beat number, with beat 0 as the first beat in each measure.
//  @parm	long* | plTime | Address of a variable to receive the converted time in clocks.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | NULL was passed as <p plTime>.
//	@rvalue E_UNEXPECTED | The Time Signature was unable to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::ClocksToMeasureBeat>
HRESULT CTimelineCtl::MeasureBeatToClocks( 
	/* [in] */ DWORD  dwGroupBits,
	/* [in] */ DWORD  dwIndex,
    /* [in] */ long lMeasure,
    /* [in] */ long lBeat,
    /* [out] */ long *plTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( NULL == plTime )
	{
		return E_POINTER;
	}

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	HRESULT hr;
	DMUS_TIMESIGNATURE TimeSig;
	MUSIC_TIME mtTSCur = 0, mtTSNext = 1;
	do
	{
		hr = GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, mtTSCur, &mtTSNext, &TimeSig );
		if ( FAILED( hr ) )
		{
			return E_UNEXPECTED;
		}

		long lMeasureClocks = TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ );
		if( mtTSNext == 0 )
		{
			mtTSCur += lMeasureClocks * lMeasure;
			break;
		}
		else
		{
			long lTmpMeasures = mtTSNext / lMeasureClocks;
			if( lMeasure <= lTmpMeasures )
			{
				mtTSCur += lMeasureClocks * lMeasure;
				break;
			}
			else
			{
				mtTSCur += lMeasureClocks * lTmpMeasures;
				lMeasure -= lTmpMeasures;
			}
		}
	}
	while( lMeasure > 0 );

	if( lBeat >= 0 )
	{
		hr = GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, max(mtTSCur, 0), NULL, &TimeSig );
	}
	else
	{
		hr = GetParam( GUID_TimeSignature, dwGroupBits, dwIndex, max(mtTSCur - TimeSig.bBeatsPerMeasure * NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ), 0), NULL, &TimeSig );
	}
	if ( FAILED( hr ) )
	{
		return E_UNEXPECTED;
	}

	mtTSCur += NOTE_TO_CLOCKS( TimeSig.bBeat, DMUS_PPQ ) * lBeat;
	*plTime = mtTSCur;
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | MeasureBeatToPosition | This method converts
//		from a measure and beat to a pixel position.
//
//	@comm	If <p lMeasure> is less than 0, the Time Signature at time 0 will
//			be used to compute <p plPosition>.  If <p lBeat> is less than 0, the
//			Time Signature in measure <p lMeasure> will be used to compute the offset
//			from the start of measure <p lMeasure>.
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm	long | lMeasure | The measure number, with measure 0 as the first measure.
//  @parm	long | lBeat | The beat number, with beat 0 as the first beat in each measure.
//  @parm	long* | plPosition | Address of a variable to receive the pixel position.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | NULL was passed as <p plPosition>.
//	@rvalue E_UNEXPECTED | The Time Signature was unable to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::PositionToMeasureBeat>
HRESULT CTimelineCtl::MeasureBeatToPosition( 
	/* [in] */ DWORD  dwGroupBits,
	/* [in] */ DWORD  dwIndex,
    /* [in] */ long   lMeasure,
    /* [in] */ long   lBeat,
    /* [out] */ long *pPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	long lTime;
	HRESULT hr;
	// Since this uses ClocksToPosition, we don't care if we're in music time
	// or real time.  ClocksToPosition will deal with it appropriately.
	if( SUCCEEDED( hr = MeasureBeatToClocks( dwGroupBits, dwIndex, lMeasure, lBeat, &lTime ) ))
	{
		hr = ClocksToPosition( lTime, pPosition );
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | StripInvalidateRect | This method invalidates the
//		specified rectangle in a strip, causing the area to be redrawn.
//
//	@parm	<i IDMUSProdStrip>* | pStrip | Which strip to invalidate the rectangle in.
//  @parm   RECT* | pRect | The rectangle to invalidate.  If NULL, invalidate the entire strip.
//  @parm   BOOL | fErase | If TRUE, erase the background of the rectangle first.
//
//	@comm	The parameter <p fErase> is ignore, as the background of the strip is always filled
//		before <om IDMUSProdStrip::Draw> is called.
//
//  @rvalue S_OK | The operation was successful.
//	@rvalue E_INVALIDARG | <p pStrip> was not added to the timeline by calling one of
//		<om IDMUSProdTimeline::AddStrip>, <om IDMUSProdTimeline::InsertStripAtDefaultPos>, or
//		<om IDMUSProdTimeline::InsertStripAtPos>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::Refresh>, <i IDMUSProdStrip>
HRESULT CTimelineCtl::StripInvalidateRect(
	/* [in] */ IDMUSProdStrip*	pStrip,
	/* [in] */ RECT*			pRect,
	/* [in] */ BOOL				fErase)
{
	UNREFERENCED_PARAMETER( fErase );
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !::IsWindow(m_hWnd) )
	{
		// Our window has been destroyed - fail.
		return E_FAIL;
	}

	StripList* pSL;
	RECT	rect;

	pSL = FindStripList( pStrip );
	if( pSL )
	{
		RECT rectStripClientRect;
		GetStripClientRect( pSL, &rectStripClientRect );

		int nResult = 1;

		// if pRect is NULL, we should invalidate the entire strip.
		if( pRect == NULL )
		{
			rect = rectStripClientRect;
		}
		// otherwise, we need to convert from strip coordinates to
		// window coordinates
		else
		{
			HRESULT hr;
			rect = *pRect;
			if( FAILED(hr = StripRectToClient( pSL, &rect )) )
			{
				return hr;
			}
			nResult = ::IntersectRect( &rect, &rect, &rectStripClientRect );
		}

		// Only invalidate if the rect isn't empty
		if( (rect.left != rect.right)
		&&	(rect.top != rect.bottom) )
		{
			InvalidateRect( &rect, FALSE );
		}
	}
	else
	{
		return E_INVALIDARG;
	}
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | RemovePropertyPageObject | This method removes
//		a property page object previously set by <om IDMUSProdTimeline::SetPropertyPage>
//
//	@comm <t TP_DMUSPRODFRAMEWORK> must be set to the DirectMusic Producer framework
//		object (via a call to <om IDMUSProdTimeline::SetTimelineProperty> prior to calling
//		this method.
//
//  @parm   IUnknown* | punkPropPageObj | Reference to the <i IDMUSProdPropPageObject>
//		to remove from the current property sheet.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p punkPropPageObj>
//	@rvalue E_FAIL | <t TP_DMUSPRODFRAMEWORK> was not set previous to calling this method.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdPropPageObject>,	<om IDMUSProdTimeline::SetPropertyPage>
HRESULT CTimelineCtl::RemovePropertyPageObject(	/* [in] */ IUnknown* punkPropPageObj )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( punkPropPageObj == NULL )
	{
		return E_POINTER;
	}

	if ( m_pDMUSProdFramework == NULL )
	{
		return E_FAIL;
	}

	IDMUSProdPropPageObject* pPPO;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED(hr = punkPropPageObj->QueryInterface( IID_IDMUSProdPropPageObject,
		(void**)&pPPO )))
	{
		IDMUSProdPropSheet* pJPS;
		if( SUCCEEDED(hr = m_pDMUSProdFramework->QueryInterface( IID_IDMUSProdPropSheet,
			(void**)&pJPS )))
		{
			pJPS->RemovePageManagerByObject( pPPO );
			pJPS->Release();
		}
		pPPO->Release();
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | SetPropertyPage | This method changes the
//		currently displayed <i IDMUSProdPropSheet> to refer to <p punkPropPageMgr>
//		and <p punkPropPageObj>.
//
//	@comm <t TP_DMUSPRODFRAMEWORK> must be set to the DirectMusic Producer framework
//		object (via a call to <om IDMUSProdTimeline::SetTimelineProperty> prior to calling
//		this method.
//
//  @parm   IUnknown* | punkPropPageMgr | Reference to the <i IDMUSProdPropPageManager>
//		to set for the currently displayed property sheet.
//  @parm   IUnknown* | punkPropPageObj | Reference to the <i IDMUSProdPropPageObject>
//		to set for the currently displayed property sheet.
//
//	@rdesc	If the property sheet is hidden, this method returns S_FALSE and does not
//		set either <p punkPropPageMgr> or <p punkPropPageObj>.
//
//  @rvalue S_OK | The operation was successful
//	@rvalue S_FALSE | The current property sheet is hidden.
//  @rvalue E_POINTER | NULL was passed as  <p punkPropPageObj> or <p punkPropPageMgr>
//	@rvalue E_FAIL | <t TP_DMUSPRODFRAMEWORK> was not set previous to calling this method.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>,
//		<i IDMUSProdPropSheet>, <om IDMUSProdTimeline::RemovePropertyPageObject>
HRESULT CTimelineCtl::SetPropertyPage(
		/* [in] */ IUnknown* punkPropPageMgr,
		/* [in] */ IUnknown* punkPropPageObj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !punkPropPageMgr || !punkPropPageObj )
	{
		return E_POINTER;
	}

	IDMUSProdPropPageManager* pPPM;
	IDMUSProdPropPageObject* pPPO;
	HRESULT hr = E_FAIL;

	if( m_pDMUSProdFramework )
	{
		IDMUSProdPropSheet* pJPS;
		if( SUCCEEDED(hr = m_pDMUSProdFramework->QueryInterface( IID_IDMUSProdPropSheet,
			(void**)&pJPS )))
		{
			//  If the property sheet is hidden, exit
			if( pJPS->IsShowing() != S_OK )
			{
				pJPS->Release();
				return S_FALSE;
			}
			if( SUCCEEDED(hr = punkPropPageMgr->QueryInterface( IID_IDMUSProdPropPageManager, 
				(void**)&pPPM )))
			{
				if( SUCCEEDED(hr = punkPropPageObj->QueryInterface( IID_IDMUSProdPropPageObject,
					(void**)&pPPO )))
				{
					HWND hwnd = ::GetFocus();	// save window to setfocus to afterwards if necessary
					if( SUCCEEDED(pJPS->SetPageManager( pPPM )))
					{
						pPPM->SetObject(pPPO);
// don't open property page automatically	pJPS->Show(TRUE);
					}
					HWND hwnd2 = ::GetFocus();
					if(hwnd != hwnd2)
					{
						::SetFocus(hwnd);
					}
					pPPO->Release();
				}
				pPPM->Release();
			}
			pJPS->Release();
		}
	}
	return hr;
}

StripList* CTimelineCtl::FindStripList(IDMUSProdStrip* pStrip) const
{
	StripList* pSL;

	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( pSL->m_pStrip == pStrip )
		{
			return pSL;
		}
	}
	return NULL;
}

//  @method HRESULT | IDMUSProdTimeline | StripSetTimelineProperty | This method sets a strip property
//		that is controlled by the Timeline.
//
//	@parm	<i IDMUSProdStrip>* | pIStrip | Which strip to set the property for.
//  @parm   <t STRIP_TIMELINE_PROPERTY> | stp | Which property to set.  Must be one of <t STRIP_TIMELINE_PROPERTY>.
//  @parm   VARIANT | variant | The data to set the property with.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | <p pIStrip> is NULL.
//	@rvalue E_INVALIDARG | <p stp> does not contain a valid property type, <p variant>
//		contains invalid data for the specified property type, or <p pIStrip> was not previously
//		added to the Timeline.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>, <t STRIP_TIMELINE_PROPERTY>,
//		<om IDMUSProdTimeline::StripGetTimelineProperty>
HRESULT CTimelineCtl::StripSetTimelineProperty( 
    /* [in] */ IDMUSProdStrip* pIStrip,
    /* [in] */ STRIP_TIMELINE_PROPERTY stp,
    /* [in] */ VARIANT variant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = E_INVALIDARG;

	if( NULL == pIStrip )
	{
		return E_POINTER;
	}

	StripList* pSL = FindStripList(pIStrip);
	if( NULL == pSL )
	{
		return E_INVALIDARG;
	}

	switch( stp )
	{
	case STP_VERTICAL_SCROLL:
		{
				long lScrollAmount = pSL->m_lVScroll - V_I4(&variant);
				RECT rect;

				GetStripClientRect(pSL, &rect);
				if (( lScrollAmount >= rect.bottom - rect.top ) || ( -lScrollAmount >= rect.bottom - rect.top ))
				{
					pSL->m_lVScroll = V_I4(&variant);
					StripInvalidateRect( pIStrip, NULL, TRUE );
				}
				else
				{
					// Scroll strip
					pSL->m_lVScroll = V_I4(&variant);
					ScrollWindow( 0, lScrollAmount, &rect, &rect );
					if (lScrollAmount < 0) // scroll up
					{
						// bottom is invalidated automatically by ScrollWindow()
						RECT oldRect = rect;

						// invalidate region displaying the track's title
						rect.bottom = rect.top + FUNCTION_NAME_HEIGHT;
						rect.right = rect.left + m_lFunctionBarWidth;
						// Don't need to erase, since the title will overwrite it anyways
						InvalidateRect( &rect, FALSE);

						// invalidate region displaying the track's minize icon
						VARIANT var;
						if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_MINMAXABLE, &var )))
						{
							if( ( var.vt == VT_BOOL ) && ( V_BOOL(&var) == TRUE ))
							{
								if ((pSL->m_sv == SV_NORMAL) || (pSL->m_sv == SV_MINIMIZED))
								{
									rect.bottom += m_sizeMinMaxButton.cy - FUNCTION_NAME_HEIGHT;
									rect.right = oldRect.right;
									long posLength;
									ClocksToPosition( m_lLength, &posLength );
									posLength++;
									if( posLength < rect.right - m_lFunctionBarWidth)
									{
										rect.right = posLength + m_lFunctionBarWidth;
									}
									rect.left = rect.right - m_sizeMinMaxButton.cx;
									// Don't need to erase, since the button will overwrite it anyways
									InvalidateRect( &rect, FALSE);
								}
							}
						}
					}
					else // scroll down
					{
						RECT oldRect = rect;
						// top is invalidated automatically by ScrollWindow()

						// invalidate region displaying the track's title
						rect.top += lScrollAmount;
						rect.bottom = rect.top + FUNCTION_NAME_HEIGHT;
						rect.right = rect.left + m_lFunctionBarWidth;
						InvalidateRect( &rect, FALSE);

						// invalidate region displaying the track's minize icon
						VARIANT var;
						if( SUCCEEDED( pSL->m_pStrip->GetStripProperty( SP_MINMAXABLE, &var )))
						{
							if( ( var.vt == VT_BOOL ) && ( V_BOOL(&var) == TRUE ))
							{
								if ((pSL->m_sv == SV_NORMAL) || (pSL->m_sv == SV_MINIMIZED))
								{
									rect.bottom += m_sizeMinMaxButton.cy - FUNCTION_NAME_HEIGHT;
									rect.right = oldRect.right;
									long posLength;
									ClocksToPosition( m_lLength, &posLength );
									posLength++;
									if( posLength < rect.right - m_lFunctionBarWidth)
									{
										rect.right = posLength + m_lFunctionBarWidth;
									}
									rect.left = rect.right - m_sizeMinMaxButton.cx;
									InvalidateRect( &rect, FALSE);
								}
							}
						}
					}
				}
		}
		break;

	case STP_HEIGHT:
		if (variant.vt != VT_I4)
		{
			hr = E_INVALIDARG;
		}
		else
		{
				hr = S_OK;
				if ( pSL->m_sv == SV_NORMAL )
				{
					if( pSL->m_lHeight != V_I4(&variant) )
					{
						pSL->m_lHeight = V_I4(&variant);

						// Send WM_MOVE messages to all strips (below this one) that were moved
						if( ::IsWindow(m_hWnd) )
						{
							StripList* pTemp = pSL->m_pNext;
							while( pTemp )
							{
								pTemp->m_pStrip->OnWMMessage( WM_MOVE, 0, 0, 0, 0 );
								pTemp = pTemp->m_pNext;
							}
						}

						RECT rectWin, rect;
						GetClientRect( &rectWin );
						GetStripClientRect( pSL, &rect );
						rectWin.top = rect.top - BORDER_HORIZ_WIDTH;
	//						InvalidateRect( &rectWin );
						InvalidateRect( &rectWin, FALSE );
						ComputeScrollBars();
						// If, as a result of the strip resize, the total height of the strips is less than the
						// height of the window, scroll back to the top.
						if ( (TotalStripHeight() < rectWin.bottom - rectWin.top) && (m_lYScrollPos != 0) )
						{
							int iTemp = 0;
							OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, 0 ), (LPARAM) ((HWND) m_ScrollVertical), iTemp );
						}
					}
				}
				else if ( pSL->m_sv == SV_MINIMIZED )
				{
					pSL->m_lRestoreHeight = V_I4(&variant);
				}
				else
				{
					hr = E_UNEXPECTED;
				}
		}
		break;

	case STP_STRIPVIEW:
		if (variant.vt != VT_I4)
		{
			hr = E_INVALIDARG;
		}
		else
		{
			if( (V_I4(&variant) <= (int) SV_FUNCTIONBAR_MINIMIZED))
			{
				if ( pSL->m_sv != (STRIPVIEW) V_I4(&variant) )
				{
					if ( (STRIPVIEW) V_I4(&variant) == SV_MINIMIZED)
					{
						pSL->m_lRestoreHeight = pSL->m_lHeight;
						pSL->m_sv = SV_MINIMIZED;
						VARIANT var;
						if (SUCCEEDED (pIStrip->GetStripProperty( SP_MINIMIZE_HEIGHT, &var)))
						{
							pSL->m_lHeight = V_INT(&var);
						}
						else
						{
							pSL->m_lHeight = MIN_STRIP_HEIGHT;
						}
					}
					else if ( (STRIPVIEW) V_I4(&variant) == SV_NORMAL)
					{
						pSL->m_lHeight = pSL->m_lRestoreHeight;
						pSL->m_sv = SV_NORMAL;
					}

					// Send WM_MOVE messages to all strips (below this one) that were moved
					if( ::IsWindow(m_hWnd) )
					{
						StripList* pTemp = pSL->m_pNext;
						while( pTemp )
						{
							pTemp->m_pStrip->OnWMMessage( WM_MOVE, 0, 0, 0, 0 );
							pTemp = pTemp->m_pNext;
						}
					}
	
					RECT rectWin, rect;
					GetClientRect( &rectWin );
					GetStripClientRect( pSL, &rect );
					rectWin.top = rect.top - BORDER_HORIZ_WIDTH;
					InvalidateRect( &rectWin, FALSE );
					ComputeScrollBars();
					// If, as a result of the strip resize, the total height of the strips is less than the
					// height of the window, scroll back to the top.
					if ( (TotalStripHeight() < rectWin.bottom - rectWin.top) && (m_lYScrollPos != 0) )
					{
						int iTemp = 0;
						OnVScroll( 0, MAKELONG( SB_THUMBPOSITION, 0 ), (LPARAM) ((HWND) m_ScrollVertical), iTemp );
					}
				}
			}
			else
			{
				hr = E_INVALIDARG;
			}
		}
		break;

	case STP_GUTTER_SELECTED:
		if (variant.vt != VT_BOOL)
		{
			hr = E_INVALIDARG;
		}
		else
		{
			if( (V_BOOL(&variant) != pSL->m_fSelected) )
			{
				// This code is duplicated in SetStripGutter
				// Change its selection
				pSL->m_fSelected = V_BOOL(&variant);

				// Update the gutter display
				RECT rect;
				GetStripClientRect( pSL, &rect );
				rect.left = 0;
				rect.right = rect.left + GUTTER_WIDTH;
				InvalidateRect( &rect, FALSE );

				// Notify the strip
				/*
				var.vt = VT_BOOL;
				V_BOOL(&var) = (short)m_pMouseStripList->m_fSelected;
				m_pMouseStripList->m_pStrip->SetStripProperty( SP_GUTTERSELECT, var );
				*/
				hr = S_OK;
			}
		}
		break;

	case STP_STRIP_INFO:
		if (variant.vt != VT_BYREF)
		{
			hr = E_INVALIDARG;
		}
		else
		{
			DMUSPROD_TIMELINE_STRIP_INFO *pDMUSPROD_TIMELINE_STRIP_INFO = (DMUSPROD_TIMELINE_STRIP_INFO *)V_BYREF( &variant );
			if( NULL == pDMUSPROD_TIMELINE_STRIP_INFO )
			{
				hr = E_POINTER;
			}
			else
			{
				pSL->m_clsidType = pDMUSPROD_TIMELINE_STRIP_INFO->clsidType;
				pSL->m_dwGroupBits = pDMUSPROD_TIMELINE_STRIP_INFO->dwGroupBits;
				pSL->m_dwIndex = pDMUSPROD_TIMELINE_STRIP_INFO->dwIndex;
			}
		}
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | OnDataChanged | This method calls the registered
//		<om IDMUSProdTimelineCallback::OnDataChanged> method with <p punk> as a
//		parameter.
//
//	@comm	When used in the Segment Designer, an <i IDMUSProdStripMgr> interface must be passed
//		in <p punk>.  The Segment Designer will then call the strip manager's <om IStream::Save>
//		method to retrieve the new data.
//
//  @parm   IUnknown* | punk | The interface to pass.
//
//	@rdesc	If there is an <om IDMUSProdTimelineCallback::OnDataChanged> method registered, 
//		the return value is the value returned by <om IDMUSProdTimelineCallback::OnDataChanged>.
//		Otherwise, the return value is E_FAIL.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStripMgr>, <om IDMUSProdTimelineCallback::OnDataChanged>
HRESULT CTimelineCtl::OnDataChanged( IUnknown* punk )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = E_FAIL;

	if( m_pTimelineCallback )
	{
		EnterCriticalSection( &m_csOnDataChanged );
		hr = m_pTimelineCallback->OnDataChanged( punk );
		LeaveCriticalSection( &m_csOnDataChanged );
	}
	else
	{
		return E_FAIL;
	}
	return hr;
}

// This method is used by the drag'n'drop code
void CTimelineCtl::GetStripAndCoordinates( POINTL ptTimeline, POINTL *ptStrip, StripList **ppSL )
{
	long xPos = ptTimeline.x;
	long yPos = ptTimeline.y;

	ASSERT(ppSL != NULL);

	// No strip should have the mouse captured
	ASSERT(m_fStripMouseCapture == FALSE);

	// We should not be in any "active" mouse mode
	ASSERT(m_MouseMode != MM_ACTIVERESIZE && m_MouseMode != MM_ACTIVEGUTTER);

	// Convert X coordinate to be an offset from the strip origin
	// add horizontal scroll and subtract the function bar and early amount
	xPos += m_lXScrollPos - m_lLastEarlyPosition - m_lFunctionBarWidth;

	// Convert the X coordinate to a clock position
	long lClockPos;
	PositionToClocks( xPos, &lClockPos );

	// If beyond the end of time, set ppSL to NULL
	if( lClockPos >= m_lLength + m_lLastLateTime )
	{
		*ppSL = NULL;
		return;
	}

	// Compute which Strip is below the cursor.
	StripList	*pSLSelected;
	StripList	*pTempSL;
	StripList	*pSL;
	long		yOrg = -m_lYScrollPos;

	// Initialize the found strip to NULL
	pSLSelected = NULL;

	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( pSL == m_pStripList )
		{
			// the top strip doesn't scroll
			yOrg += m_lYScrollPos;
		}
		else if( pSL == m_pStripList->m_pNext )
		{
			yOrg -= m_lYScrollPos;
		}
		yOrg += pSL->m_lHeight + BORDER_HORIZ_WIDTH;
		if( ( xPos < BORDER_VERT_WIDTH ) && 
			( yPos > yOrg - pSL->m_lHeight - BORDER_HORIZ_WIDTH ) && 
			( yPos < yOrg + BORDER_HORIZ_WIDTH ) )
		{
			// The mouse is in the left gutter
			// Set selected strip to NULL to indicate failure
			pSLSelected = NULL;
		}
		else if( (yPos < yOrg + BORDER_HORIZ_WIDTH) && (yPos > yOrg) )
		{
			// We are on the border between strips, choose the one we are closer to and send
			// the message there.
			if(yPos - yOrg > BORDER_HORIZ_WIDTH / 2)
			{
				// Choose the lower strip
				if(pSL->m_pNext != NULL)
				{
					pSLSelected = pSL->m_pNext;
				}
				else
				{
					pSLSelected = pSL;
				}
			}
			else
			{
				// Choose the upper strip
				pSLSelected = pSL;
			}
		}
		else if ( ( yPos > yOrg - pSL->m_lHeight - BORDER_HORIZ_WIDTH ) && 
			( yPos < yOrg + BORDER_HORIZ_WIDTH ) )
		{
			pSLSelected = pSL;
			break; // this break must be here or else the top strip might not get selected
		}
	}

	// If we didn't find a strip, set ppSL to NULL and return
	if(pSLSelected == NULL)
	{
		*ppSL = NULL;
		return;
	}

	// Convert Y coordinate to be relative to strip's origin
	// add any strip vertical scrolling, along with any borders
	yPos += pSLSelected->m_lVScroll - BORDER_HORIZ_WIDTH;

	// add vertical timeline scroll for all except top strip, which doesn't scroll
	if( pSLSelected != m_pStripList )
	{
		yPos += m_lYScrollPos;
	}

	// subtract the heights of any strips above this one
	for( pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
	{
		if( pTempSL == pSLSelected )
		{
			break;
		}
		yPos -= ( pTempSL->m_lHeight + BORDER_HORIZ_DRAWWIDTH );
	}

	// Set the strip position (ptStrip), in strip coordinates
	ASSERT( ptStrip != NULL );
	(*ptStrip).x = xPos;
	(*ptStrip).y = yPos;
	*ppSL = pSLSelected;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl IDropTarget implementation

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::DragEnter

HRESULT CTimelineCtl::DragEnter(
	/* [in] */ IDataObject __RPC_FAR *pDataObj,
	/* [in] */ DWORD grfKeyState,
	/* [in] */ POINTL pt,
	/* [out][in] */ DWORD __RPC_FAR *pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Save a pointer to the data object to pass to strips as the object is dragged over them
	ASSERT(m_pCurrentDataObject == NULL);
	if(pDataObj == NULL || pdwEffect == NULL)
	{
		return E_INVALIDARG;
	}
	m_pCurrentDataObject = pDataObj;
	m_pCurrentDataObject->AddRef();

	// Reset timer used to scroll timeline
	m_dwDragScrollTick = 0;

	HRESULT		hr;
	StripList	*pSLSelected;
	IDropTarget	*pIDropTarget;
	POINTL		ptStrip;
	
	{
		// BUGBUG: May cut off point coordinates here.
		ASSERT( (pt.x < SHRT_MAX) && (pt.y < SHRT_MAX) && (pt.x > SHRT_MIN) && (pt.y > SHRT_MIN));
		POINT	ptTmp;
		ptTmp.x = pt.x;
		ptTmp.y = pt.y;
		ScreenToClient( &ptTmp );
		pt.x = ptTmp.x;
		pt.y = ptTmp.y;
	}

	GetStripAndCoordinates(pt, &ptStrip, &pSLSelected);
	if(pSLSelected == NULL)
	{
		// We succeeded, but we cannot accept the object
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}

	ASSERT(pSLSelected->m_pStrip != NULL);

	hr = pSLSelected->m_pStrip->QueryInterface(IID_IDropTarget, (void **) &pIDropTarget);
	if(FAILED(hr))
	{
		// We succeeded, but we cannot accept the object
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}

	m_pLastDragStripList = pSLSelected;
	m_pLastDragStripList->m_pStrip->AddRef();

	// Call the strip's IDropTarget::DragEnter method
	hr = pIDropTarget->DragEnter(pDataObj, grfKeyState, ptStrip, pdwEffect);
	if(FAILED(hr))
	{
		m_pCurrentDataObject->Release();
		m_pCurrentDataObject = NULL;
		m_pLastDragStripList->m_pStrip->Release();
		m_pLastDragStripList = NULL;
	}
	pIDropTarget->Release();
	return hr;
}

 
#define DO_HSCROLL		1
#define DO_VSCROLL		2
   
////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::ScrollTimeline

void CTimelineCtl::ScrollTimeline( POINTL point )
{
	// Check to see if we need to scroll the timeline
	DWORD dwScrollTick = m_dwDragScrollTick;
	m_dwDragScrollTick = 0;

	// Determine width of scrollbars
	int nHScrollBarHeight = GetSystemMetrics( SM_CYHSCROLL );
	int nVScrollBarWidth = GetSystemMetrics( SM_CXVSCROLL );
	
	LPARAM lParam;
	int nGutterWidth = GUTTER_WIDTH;
	if( SUCCEEDED( GetParam( GUID_Segment_DisplayContextMenu, 0xFFFFFFFF, 0, 0, NULL, &lParam ) ) )
	{
		// Segments should include FBar width
		nGutterWidth += m_lFunctionBarWidth;
	}

	CRect rect;
	UINT nAction = (UINT)-1;
	short nDirection = -1;

	GetClientRect( &rect );

	int nScrollMin;
	int nScrollMax;
	int nScrollPos;

	if( point.x <= (rect.left + nGutterWidth) )
	{
		m_ScrollHorizontal.GetScrollRange( &nScrollMin, &nScrollMax );
		nScrollPos = m_ScrollHorizontal.GetScrollPos();

		if( nScrollPos > nScrollMin )
		{
			nDirection = DO_HSCROLL;
			nAction = SB_LINELEFT;
		}
	}
	else if( point.x >= (rect.right - nVScrollBarWidth) )	
	{
		m_ScrollHorizontal.GetScrollRange( &nScrollMin, &nScrollMax );
		nScrollPos = m_ScrollHorizontal.GetScrollPos();

		if( nScrollPos < nScrollMax )
		{
			nDirection = DO_HSCROLL;
			nAction = SB_LINERIGHT;
		}
	}
	else if( point.y <= (rect.top + STRIP_HEIGHT) )
	{
		m_ScrollVertical.GetScrollRange( &nScrollMin, &nScrollMax );
		nScrollPos = m_ScrollVertical.GetScrollPos();

		if( nScrollPos > nScrollMin )
		{
			nDirection = DO_VSCROLL;
			nAction = SB_LINEUP;
		}
	}
	else if( point.y >= (rect.bottom - nHScrollBarHeight) )	
	{
		m_ScrollVertical.GetScrollRange( &nScrollMin, &nScrollMax );
		nScrollPos = m_ScrollVertical.GetScrollPos();

		if( nScrollPos < nScrollMax )
		{
			nDirection = DO_VSCROLL;
			nAction = SB_LINEDOWN;
		}
	}

	if( nAction != -1 )
	{
		DWORD dwTick = GetTickCount();

		if( dwScrollTick )
		{
			if( dwTick >= dwScrollTick )
			{
				// Time to scroll
				if( nDirection == DO_VSCROLL )
				{
					SendMessage( WM_VSCROLL, nAction, (LPARAM)m_ScrollVertical.GetSafeHwnd() );
				}
				else
				{
					SendMessage( WM_HSCROLL, nAction, 0L );
				}

				if( nDirection == DO_HSCROLL )
				{
					m_dwDragScrollTick = dwTick + 90;	// wait 90 ms
				}
				else
				{
					m_dwDragScrollTick = dwTick + 90;	// wait 90 ms
				}
			}
			else
			{
				m_dwDragScrollTick = dwScrollTick;	// still waiting...
			}
		}
		else
		{
			m_dwDragScrollTick = dwTick + 350;		// wait 300 ms
		}
	}
}

   
////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::DragOver

HRESULT CTimelineCtl::DragOver(
	/* [in] */ DWORD grfKeyState,
	/* [in] */ POINTL pt,
	/* [out][in] */ DWORD __RPC_FAR *pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(pdwEffect == NULL)
	{
		return E_INVALIDARG;
	}

	ASSERT(m_pCurrentDataObject != NULL);

	HRESULT		hr;
	StripList	*pSLSelected;
	IDropTarget	*pIDropTarget;
	POINTL		ptStrip;

	ScreenToClient( (PPOINT) &pt );

	// Scroll timeline when necessary
	ScrollTimeline( pt );

	GetStripAndCoordinates(pt, &ptStrip, &pSLSelected);

	if(m_pLastDragStripList != NULL && m_pLastDragStripList != pSLSelected)
	{
		hr = m_pLastDragStripList->m_pStrip->QueryInterface(IID_IDropTarget, (void **) &pIDropTarget);
		// m_pLastDragStripList should only be non-NULL if it is a drop target
		ASSERT(SUCCEEDED(hr));
		pIDropTarget->DragLeave();
		pIDropTarget->Release();
		m_pLastDragStripList->m_pStrip->Release();
		m_pLastDragStripList = NULL;
		if(pSLSelected != NULL)
		{
			ASSERT(pSLSelected->m_pStrip != NULL);

			hr = pSLSelected->m_pStrip->QueryInterface(IID_IDropTarget, (void **) &pIDropTarget);
			if(FAILED(hr))
			{
				goto Leave;
			}

			m_pLastDragStripList = pSLSelected;
			m_pLastDragStripList->m_pStrip->AddRef();

			// Call the strip's IDropTarget::DragEnter method
			hr = pIDropTarget->DragEnter(m_pCurrentDataObject, grfKeyState, ptStrip, pdwEffect);
			pIDropTarget->Release();
		}
		else
		{
			goto Leave;
		}
	}
	else if(pSLSelected == NULL)
	{
		goto Leave;
	}
	else if(m_pLastDragStripList == pSLSelected)
	{
		ASSERT(pSLSelected->m_pStrip != NULL);

		hr = pSLSelected->m_pStrip->QueryInterface(IID_IDropTarget, (void **) &pIDropTarget);
		if(FAILED(hr))
		{
			goto Leave;
		}

		// Call the strip's IDropTarget::DragOver method
		hr = pIDropTarget->DragOver(grfKeyState, ptStrip, pdwEffect);
		pIDropTarget->Release();
	}
	else // m_pLastDragStrip == NULL, pSLSelected != NULL
	{
		ASSERT(pSLSelected->m_pStrip != NULL);

		hr = pSLSelected->m_pStrip->QueryInterface(IID_IDropTarget, (void **) &pIDropTarget);
		if(FAILED(hr))
		{
			goto Leave;
		}

		ASSERT( m_pLastDragStripList == NULL );
		m_pLastDragStripList = pSLSelected;
		m_pLastDragStripList->m_pStrip->AddRef();

		// Call the strip's IDropTarget::DragEnter method
		hr = pIDropTarget->DragEnter(m_pCurrentDataObject, grfKeyState, ptStrip, pdwEffect);
		pIDropTarget->Release();
	}

	return hr;

Leave:
	// We succeeded, but we cannot accept the object
	*pdwEffect = DROPEFFECT_NONE;
	return S_OK;
}
    
////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::DragLeave

HRESULT CTimelineCtl::DragLeave(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT(m_pCurrentDataObject != NULL);
	m_pCurrentDataObject->Release();
	m_pCurrentDataObject = NULL;

	if(m_pLastDragStripList != NULL)
	{
		ASSERT(m_pLastDragStripList->m_pStrip != NULL);

		HRESULT		hr;
		IDropTarget	*pIDropTarget;

		hr = m_pLastDragStripList->m_pStrip->QueryInterface(IID_IDropTarget, (void **) &pIDropTarget);
		// This strip should always have this implementation
		ASSERT(SUCCEEDED(hr));
		if(SUCCEEDED(hr))
		{
			pIDropTarget->DragLeave();
			pIDropTarget->Release();
		}
		m_pLastDragStripList->m_pStrip->Release();
		m_pLastDragStripList = NULL;
	}

	return S_OK;
}
    
////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::Drop

HRESULT CTimelineCtl::Drop(
	/* [in] */ IDataObject __RPC_FAR *pDataObj,
	/* [in] */ DWORD grfKeyState,
	/* [in] */ POINTL pt,
	/* [out][in] */ DWORD __RPC_FAR *pdwEffect)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(pDataObj == NULL || pdwEffect == NULL)
	{
		return E_POINTER;
	}

	// Assert that we are dropping the object we got the DragEnter on
	ASSERT(pDataObj == m_pCurrentDataObject);

	HRESULT		hr;
	StripList	*pSLSelected;
	IDropTarget	*pIDropTarget;
	POINTL		ptStrip;

	{
		// BUGBUG: May cut off point coordinates here.
		ASSERT( (pt.x < SHRT_MAX) && (pt.y < SHRT_MAX) && (pt.x > SHRT_MIN) && (pt.y > SHRT_MIN));
		POINT	ptTmp;
		ptTmp.x = pt.x;
		ptTmp.y = pt.y;
		ScreenToClient( &ptTmp );
		pt.x = ptTmp.x;
		pt.y = ptTmp.y;
	}

	GetStripAndCoordinates(pt, &ptStrip, &pSLSelected);
	ASSERT(pSLSelected == m_pLastDragStripList);

	if(pSLSelected == NULL)
	{
		goto Leave;
	}

	ASSERT(pSLSelected->m_pStrip != NULL);

	hr = pSLSelected->m_pStrip->QueryInterface(IID_IDropTarget, (void **) &pIDropTarget);
	if(FAILED(hr))
	{
		goto Leave;
	}
	// Call the strip's IDropTarget::Drop method
	hr = pIDropTarget->Drop(pDataObj, grfKeyState, ptStrip, pdwEffect);

	m_pCurrentDataObject->Release();
	m_pCurrentDataObject = NULL;
	m_pLastDragStripList->m_pStrip->Release();
	m_pLastDragStripList = NULL;

	pIDropTarget->Release();

	if( SUCCEEDED( hr )
	&&	(*pdwEffect != DROPEFFECT_NONE) )
	{
		// If we did a drop, set the dropped on node as active
		if( m_pDMUSProdFramework
		&&	m_pTimelineCallback )
		{
			IDMUSProdNode *pIDMUSProdNode;
			if( SUCCEEDED( m_pTimelineCallback->QueryInterface( IID_IDMUSProdNode, (void**)&pIDMUSProdNode ) ) )
			{
				m_pDMUSProdFramework->SetSelectedNode( pIDMUSProdNode );
				pIDMUSProdNode->Release();
			}
		}
	}

	return hr;

Leave:
	// We succeeded, but we cannot accept the object
	*pdwEffect = DROPEFFECT_NONE;

	m_pCurrentDataObject->Release();
	m_pCurrentDataObject = NULL;

	if(m_pLastDragStripList != NULL)
	{
		m_pLastDragStripList->m_pStrip->Release();
		m_pLastDragStripList = NULL;
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl IDMUSProdTimelineEdit implementation

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::Cut
HRESULT CTimelineCtl::Cut( /* out */ IDMUSProdTimelineDataObject* pIDataObject )
{
	return CutCopy( TRUE, pIDataObject );
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::Copy
HRESULT CTimelineCtl::Copy( /* out */ IDMUSProdTimelineDataObject* pIDataObject )
{
	return CutCopy( FALSE, pIDataObject );
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::Paste
HRESULT CTimelineCtl::Paste( /* in */ IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT					hr;
	StripList*				pSL;
	IDMUSProdTimelineEdit*	pITimelineEdit = NULL;
	long					lBeginTime = -1;
	long					lEndTime = -1;

	if(pITimelineDataObject == NULL)
	{
		// Get the IDataObject from the clipboard
		IDataObject *pIDataObject;
		hr = OleGetClipboard(&pIDataObject);
		if(FAILED(hr) || (pIDataObject == NULL))
		{
			return E_FAIL;
		}

		// Create a new TimelineDataObject
		hr = AllocTimelineDataObject( &pITimelineDataObject );
		if( FAILED(hr) || (pITimelineDataObject == NULL) )
		{
			pIDataObject->Release();
			return E_FAIL;
		}

		// Insert the IDataObject into the TimelineDataObject
		hr = pITimelineDataObject->Import( pIDataObject );
		pIDataObject->Release();
		if( FAILED(hr) )
		{
			pITimelineDataObject->Release();
			return E_FAIL;
		}
	}
	else
	{
		pITimelineDataObject->AddRef();
	}

	// Make sure we can paste
	hr = CanPaste( pITimelineDataObject );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	m_bPasting = TRUE;	// So the strips won't change the select times thinking
						// they are the only ones pasting.

	// If no strips are gutter selected, only call the Active strip
	if( !AnyGutterSelectedStrips() )
	{
		if( m_pActiveStripList && m_pActiveStripList->m_pStrip &&
			SUCCEEDED( m_pActiveStripList->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**) &pITimelineEdit ) ) )
		{
			hr = pITimelineEdit->Paste(pITimelineDataObject);

			pITimelineEdit->Release();
		}
		else
		{
			hr = E_UNEXPECTED;
		}
	}
	else
	{
		int iNumCanPaste = 0;
		// Find the strip which will accept this.
		for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
		{
			if( pSL->m_fSelected && pSL->m_pStrip != NULL )
			{
				if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**) &pITimelineEdit )))
				{
					if ( pITimelineEdit->CanPaste( pITimelineDataObject ) == S_OK )
					{
						iNumCanPaste++;
					}
					pITimelineEdit->Release();
				}
			}
		}

		BOOL fOrigFreezeUndo = FALSE;
		if( iNumCanPaste > 1 )
		{
			// Freeze updates so only one Undo item is created
			VARIANT var;
			if( SUCCEEDED( GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
			{
				fOrigFreezeUndo = V_BOOL(&var);
			}

			// Freeze undo queue
			if( !fOrigFreezeUndo )
			{
				// Tell Segment to get undo text from TimeStripMgr
				m_pTimeStrip->m_nLastEdit = IDS_UNDO_PASTE_DATA;
				OnDataChanged( m_pTimeStrip->m_pTimeStripMgr );
				var.vt = VT_BOOL;
				V_BOOL(&var) = TRUE;
				SetTimelineProperty( TP_FREEZE_UNDO, var );
			}
		}

		BOOL fOneSucceeded = FALSE;

		// Find the strip which will accept this.
		// Paste TimeSigs first
		for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
		{
			if( pSL->m_fSelected && pSL->m_pStrip != NULL )
			{
				// Get the selected time so it can be restored at the end of paste.
				if( lBeginTime == -1 && lEndTime == -1 )
				{
					lBeginTime = m_lBeginSelect;
					lEndTime= m_lEndSelect;
				}
				if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**) &pITimelineEdit )))
				{
					if ( pITimelineEdit->CanPaste( pITimelineDataObject ) == S_OK )
					{
						if( StripSupportTimeSigs( pSL->m_pStrip ) )
						{
							if( pITimelineEdit->Paste( pITimelineDataObject ) == S_OK )
							{
								fOneSucceeded = TRUE;
							}
						}
					}
					pITimelineEdit->Release();
				}
			}
		}

		// Paste non-TimeSigs second
		for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
		{
			if( pSL->m_fSelected && pSL->m_pStrip != NULL )
			{
				// Get the selected time so it can be restored at the end of paste.
				if( lBeginTime == -1 && lEndTime == -1 )
				{
					lBeginTime = m_lBeginSelect;
					lEndTime= m_lEndSelect;
				}
				if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**) &pITimelineEdit )))
				{
					if ( pITimelineEdit->CanPaste( pITimelineDataObject ) == S_OK )
					{
						if( !StripSupportTimeSigs( pSL->m_pStrip ) )
						{
							if( pITimelineEdit->Paste( pITimelineDataObject ) == S_OK )
							{
								fOneSucceeded = TRUE;
							}
						}
					}
					pITimelineEdit->Release();
				}
			}
		}

		// UnLock updates
		if( (iNumCanPaste > 1) && !fOrigFreezeUndo )
		{
			// Restore undo queue
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			SetTimelineProperty( TP_FREEZE_UNDO, var );
		}

		if( fOneSucceeded ) 
		{
			hr = S_OK;
		}
		else
		{
			hr = E_UNEXPECTED;
		}
	}

	m_bPasting = FALSE;

	// Now set the correct selection time.
	if( ((lBeginTime != m_lBeginSelect) || (lEndTime != m_lEndSelect))
	&&	lBeginTime > -1 && lEndTime > -1 )
	{
		SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, lBeginTime );
		SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, lEndTime );
	}

	pITimelineDataObject->Release();

	return hr;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::Insert
// Can insert iff one strip is selected. (CanInsert() checks this)
HRESULT CTimelineCtl::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;

	hr = CanInsert();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	// Now, call Insert() on the active strip
	IDMUSProdTimelineEdit *pITimelineEdit;
	if( m_pActiveStripList && m_pActiveStripList->m_pStrip &&
		SUCCEEDED( m_pActiveStripList->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**) &pITimelineEdit ) ) )
	{
		hr = pITimelineEdit->Insert();

		pITimelineEdit->Release();
		return hr;
	}

	// No active strip, or strip doesn't support IDMUSProdTimelineEdit
	return E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::Delete
HRESULT CTimelineCtl::Delete()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT					hr;
	IDMUSProdTimelineEdit*	pITimelineEdit = NULL;

	hr = CanDelete();
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	// If no strips are gutter selected, or the selection range is empty - call Delete
	// for the active strip only
	if( (m_lBeginSelect == m_lEndSelect) || !AnyGutterSelectedStrips() )
	{
		hr = E_UNEXPECTED;
		if( m_pActiveStripList && m_pActiveStripList->m_pStrip &&
			SUCCEEDED( m_pActiveStripList->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**) &pITimelineEdit ) ) )
		{
			hr = pITimelineEdit->Delete();

			pITimelineEdit->Release();
		}
		return hr;
	}

	// At least one strip is gutter selected and the selection range is non-empty
	int iNumCanDelete = 0;
	StripList* pSL;
	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( pSL->m_fSelected && (pSL->m_pStrip != NULL) )
		{
			if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit,
														  (void**) &pITimelineEdit )))
			{
				if( pITimelineEdit->CanDelete() == S_OK )
				{
					iNumCanDelete++;
				}
				pITimelineEdit->Release();
			}
		}
	}

	BOOL fOrigFreezeUndo = FALSE;
	if( iNumCanDelete > 1 )
	{
		// Freeze updates so only one Undo item is created
		VARIANT var;
		if( SUCCEEDED( GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
		{
			fOrigFreezeUndo = V_BOOL(&var);
		}

		// Freeze undo queue
		if( !fOrigFreezeUndo )
		{
			// Tell Segment to get undo text from TimeStripMgr
			m_pTimeStrip->m_nLastEdit = IDS_UNDO_DELETE_DATA;
			OnDataChanged( m_pTimeStrip->m_pTimeStripMgr );
			var.vt = VT_BOOL;
			V_BOOL(&var) = TRUE;
			SetTimelineProperty( TP_FREEZE_UNDO, var );
		}
	}

	// Delete all non-TimeSig supporting strips
	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( pSL->m_fSelected && (pSL->m_pStrip != NULL) )
		{
			if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit,
														  (void**) &pITimelineEdit )))
			{
				if( pITimelineEdit->CanDelete() == S_OK )
				{
					if( !StripSupportTimeSigs( pSL->m_pStrip ) )
					{
						hr = pITimelineEdit->Delete();
						// Assert that a strip successfully deleted after it told us it could
						ASSERT( SUCCEEDED(hr) );
					}
				}
				pITimelineEdit->Release();
			}
		}
	}

	// Delete all Time-Sig supporting strips
	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( pSL->m_fSelected && (pSL->m_pStrip != NULL) )
		{
			if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit,
														  (void**) &pITimelineEdit )))
			{
				if( pITimelineEdit->CanDelete() == S_OK )
				{
					if( StripSupportTimeSigs( pSL->m_pStrip ) )
					{
						hr = pITimelineEdit->Delete();
						// Assert that a strip successfully deleted after it told us it could
						ASSERT( SUCCEEDED(hr) );
					}
				}
				pITimelineEdit->Release();
			}
		}
	}

	// UnLock updates
	if( (iNumCanDelete > 1) && !fOrigFreezeUndo )
	{
		// Restore undo queue
		VARIANT var;
		var.vt = VT_BOOL;
		V_BOOL(&var) = FALSE;
		SetTimelineProperty( TP_FREEZE_UNDO, var );
	}

	// TODO: Add code to return E_UNEXPECTED if no strip succeeds on the Delete() call.
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::SelectAll
HRESULT CTimelineCtl::SelectAll()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( AnyGutterSelectedStrips() )
	{
		SetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, 0 );
		SetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, m_lLength );

		return S_OK;
	}
	else if( m_pActiveStripList && m_pActiveStripList->m_pStrip )
	{
		// None selected, call SelectAll for the active strip
		IDMUSProdTimelineEdit* pITimelineEdit;
		if( SUCCEEDED( m_pActiveStripList->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void **)&pITimelineEdit ) ) )
		{
			HRESULT hr = pITimelineEdit->CanSelectAll();
			ASSERT( hr == S_OK );
			if( hr == S_OK )
			{
				hr = pITimelineEdit->SelectAll();
			}
			pITimelineEdit->Release();

			return hr;
		}
	}

	return E_UNEXPECTED;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::CanCut
HRESULT CTimelineCtl::CanCut( void )
{
	return CanEdit( IDC_TIMELINE_CUT );
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::CanCopy
HRESULT CTimelineCtl::CanCopy( void )
{
	return CanEdit( IDC_TIMELINE_COPY );
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::CanPaste
HRESULT CTimelineCtl::CanPaste( /* in */ IDMUSProdTimelineDataObject* pIDataObject )
{
	UNREFERENCED_PARAMETER( pIDataObject );
	return CanEdit( IDC_TIMELINE_PASTE );;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::CanSelectAll
HRESULT CTimelineCtl::CanSelectAll()
{
	return CanEdit( IDC_TIMELINE_SELECTALL );;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::CanInsert
HRESULT CTimelineCtl::CanInsert()
{
	HRESULT			hr;

	hr = CanEdit( IDC_TIMELINE_INSERT );
	if( hr == S_OK )
	{
		return S_OK;
	}
	return S_FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// CTimelineCtl::CanDelete
HRESULT CTimelineCtl::CanDelete()
{
	return CanEdit( IDC_TIMELINE_DELETE );
}

HRESULT CTimelineCtl::CutCopy( BOOL bCut , IDMUSProdTimelineDataObject* pITimelineDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT					hr;
	IDMUSProdTimelineEdit*	pITimelineEdit;
	BOOL					fSetClipboard = FALSE;

	// Ensure we can Cut or Copy
	hr = CanEdit( bCut ? IDC_TIMELINE_CUT : IDC_TIMELINE_COPY );
	ASSERT( hr == S_OK );
	if( hr != S_OK )
	{
		return E_UNEXPECTED;
	}

	// If no strips are gutter selected, or if the selection is NULL, only Cut/Copy in the active strip
	if( !AnyGutterSelectedStrips() || (m_lBeginSelect == m_lEndSelect) )
	{
		if( m_pActiveStripList && m_pActiveStripList->m_pStrip &&
			SUCCEEDED( m_pActiveStripList->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**) &pITimelineEdit ) ) )
		{
			if( bCut )
			{
				hr = pITimelineEdit->Cut( pITimelineDataObject );
			}
			else
			{
				hr = pITimelineEdit->Copy( pITimelineDataObject );
			}
			pITimelineEdit->Release();
		}
	}
	// Otherwise, pass the ITimelineDataObject to all selected strips.
	else
	{
		// If pITimelineDataObject is NULL, set the clipboard before returning
		fSetClipboard = (pITimelineDataObject == NULL) ? TRUE : FALSE;

		if( pITimelineDataObject )
		{
			pITimelineDataObject->AddRef();
		}
		else
		{
			hr = AllocTimelineDataObject( &pITimelineDataObject );
			ASSERT( SUCCEEDED( hr ) );
			if( FAILED( hr ) )
			{
				return E_UNEXPECTED;
			}
		}

		BOOL fOneSucceeded = FALSE;

		// Set the selection boundaries
		pITimelineDataObject->SetBoundaries( m_lBeginSelect, m_lEndSelect );

		int iNumCanCut = 0;
		StripList* pSL;
		if( bCut )
		{
			// Iterate through all selected strips
			for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
			{
				if( (pSL->m_pStrip != NULL) && pSL->m_fSelected )
				{
					if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit,
																  (void**) &pITimelineEdit)))
					{
						if( pITimelineEdit->CanCut() == S_OK )
						{
							iNumCanCut++;
						}
						pITimelineEdit->Release();
					}
				}
			}
		}

		BOOL fOrigFreezeUndo = FALSE;
		if( iNumCanCut > 1 )
		{
			// Freeze updates so only one Undo item is created
			VARIANT var;
			if( SUCCEEDED( GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
			{
				fOrigFreezeUndo = V_BOOL(&var);
			}

			// Freeze undo queue
			if( !fOrigFreezeUndo )
			{
				// Tell Segment to get undo text from TimeStripMgr
				m_pTimeStrip->m_nLastEdit = IDS_UNDO_CUT_DATA;
				OnDataChanged( m_pTimeStrip->m_pTimeStripMgr );
				var.vt = VT_BOOL;
				V_BOOL(&var) = TRUE;
				SetTimelineProperty( TP_FREEZE_UNDO, var );
			}
		}

		if( bCut )
		{
			// Cut
			// Iterate through all selected strips
			// Cut Non-TimeSigs first
			for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
			{
				if( (pSL->m_pStrip != NULL) && pSL->m_fSelected )
				{
					if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit,
																  (void**) &pITimelineEdit)))
					{
						if( pITimelineEdit->CanCut() == S_OK )
						{
							if( !StripSupportTimeSigs( pSL->m_pStrip ) )
							{
								if( pITimelineEdit->Cut( pITimelineDataObject ) == S_OK )
								{
									fOneSucceeded = TRUE;
								}
							}
						}
						pITimelineEdit->Release();
					}
				}
			}

			// Cut TimeSigs last
			for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
			{
				if( (pSL->m_pStrip != NULL) && pSL->m_fSelected )
				{
					if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit,
																  (void**) &pITimelineEdit)))
					{
						if( pITimelineEdit->CanCut() == S_OK )
						{
							if( StripSupportTimeSigs( pSL->m_pStrip ) )
							{
								if( pITimelineEdit->Cut( pITimelineDataObject ) == S_OK )
								{
									fOneSucceeded = TRUE;
								}
							}
						}
						pITimelineEdit->Release();
					}
				}
			}
		}
		else
		{
			// Copy
			// Iterate through all selected strips
			for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
			{
				if( (pSL->m_pStrip != NULL) && pSL->m_fSelected )
				{
					if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit,
																  (void**) &pITimelineEdit)))
					{
						if( pITimelineEdit->CanCopy() == S_OK )
						{
							if( pITimelineEdit->Copy( pITimelineDataObject ) == S_OK )
							{
								fOneSucceeded = TRUE;
							}
						}
						pITimelineEdit->Release();
					}
				}
			}
		}

		// UnLock updates
		if( (iNumCanCut > 1) && !fOrigFreezeUndo )
		{
			// Restore undo queue
			VARIANT var;
			var.vt = VT_BOOL;
			V_BOOL(&var) = FALSE;
			SetTimelineProperty( TP_FREEZE_UNDO, var );
		}

		// If at least one returned S_OK, then we suceeded.. Otherwise we failed.
		if( fOneSucceeded )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
	}

	if( SUCCEEDED( hr ) && fSetClipboard )
	{
		// Get the IDataObject
		IDataObject* pIDataObject = NULL;
		pITimelineDataObject->Export( &pIDataObject );

		// Send the IDataObject to the Clipboard
		ASSERT( pIDataObject != NULL );
		if( pIDataObject != NULL )
		{
			// Set m_pCopyDataObject so we can flush the clipboard if we're left on it
			// when the timeline goes away.
			if( m_pCopyDataObject )
			{
				m_pCopyDataObject->Release();
			}
			m_pCopyDataObject = pIDataObject;
			m_pCopyDataObject->AddRef();

			// Put the IDataObject on the clipboard.
			hr = OleSetClipboard( pIDataObject );
			pIDataObject->Release();
		}
	}

	if( pITimelineDataObject )
	{
		pITimelineDataObject->Release();
	}

	return hr;
}

HRESULT CTimelineCtl::CanEdit( long lCommand )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT					hr;
	IDMUSProdTimelineEdit*	pITimelineEdit;

	// Take care of Insert - can only insert into the active strip
	if( lCommand == IDC_TIMELINE_INSERT )
	{
		if( m_pActiveStripList && m_pActiveStripList->m_pStrip &&
			SUCCEEDED( m_pActiveStripList->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**) &pITimelineEdit ) ) )
		{
			hr = pITimelineEdit->CanInsert();

			pITimelineEdit->Release();
			return hr;
		}
		return S_FALSE;
	}

	// Take care of case of no gutter selection - only check active strip
	if( !AnyGutterSelectedStrips() )
	{
		if( m_pActiveStripList && m_pActiveStripList->m_pStrip &&
			SUCCEEDED( m_pActiveStripList->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**) &pITimelineEdit ) ) )
		{
			hr = CanStripEdit( pITimelineEdit, lCommand );

			pITimelineEdit->Release();
			return hr;
		}
		return S_FALSE;
	}

	// For Cut/Copy/Delete, if the selection size is 0 - only check active strip
	if( (lCommand == IDC_TIMELINE_CUT) || (lCommand == IDC_TIMELINE_COPY) ||
		(lCommand == IDC_TIMELINE_DELETE) )
	{
		if( m_lBeginSelect == m_lEndSelect )
		{
			if( m_pActiveStripList && m_pActiveStripList->m_pStrip &&
				SUCCEEDED( m_pActiveStripList->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit, (void**) &pITimelineEdit ) ) )
			{
				hr = CanStripEdit( pITimelineEdit, lCommand );

				pITimelineEdit->Release();
				return hr;
			}
			return S_FALSE;
		}
	}

	// Go through the strip list and find out if at least one can do the specified edit operation.
	// End when pSL is null or when a CanStripEdit returns S_OK
	StripList* pSL;
	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		// Only look at the selected strips.
		if( (pSL->m_pStrip != NULL) && pSL->m_fSelected )
		{
			if( SUCCEEDED( pSL->m_pStrip->QueryInterface( IID_IDMUSProdTimelineEdit,
														  (void**) &pITimelineEdit)))
			{
				hr = CanStripEdit( pITimelineEdit, lCommand );
				pITimelineEdit->Release();

				if( hr == S_OK )
				{
					return S_OK;
				}
			}
		}
	}

	// None return S_OK - return S_FALSE
	return S_FALSE;
}

// insert pMenu2 before nPos in pMenu1
void InsertMenuInMenu( CMenu* pMenu1, UINT nPos, CMenu* pMenu2)
{
	if (!pMenu2) return;
	if (!pMenu1) {
		pMenu1 = new CMenu();
		pMenu1->Attach(pMenu2->GetSafeHmenu());
	}
	else {
		if (nPos > pMenu1->GetMenuItemCount())
			nPos = pMenu1->GetMenuItemCount();
		for (int i=pMenu2->GetMenuItemCount()-1; i>=0; i--) {
			int state, ID;
			state=pMenu2->GetMenuState(i, MF_BYPOSITION);
			ID=pMenu2->GetMenuItemID(i);
			if (state && MF_SEPARATOR) {
				// separator
				pMenu1->InsertMenu(nPos, MF_BYPOSITION|MF_SEPARATOR);
			}
			else if (ID==-1) {
				// pop-up menu
				HMENU hSubMenu = pMenu2->GetSubMenu(i)->GetSafeHmenu();
				pMenu1->InsertMenu(nPos, MF_BYPOSITION|MF_POPUP, (UINT) hSubMenu);
			}
			else {
				// just a regular menu item;
				CString Name;
				pMenu2->GetMenuString( i, Name, MF_BYPOSITION );
				pMenu1->InsertMenu(nPos, MF_BYPOSITION|state|MF_STRING, ID, Name);
			}
		}
	}
}

//  @method HRESULT | IDMUSProdTimeline | TrackPopupMenu | This method displays a
//		context menu at the specified position.
//
//  @parm   HMENU | hMenu | The handle of the menu to add to the default menu.  If NULL, don't add
//		any custom items.
//  @parm   long | lXPos | The horizontal coordinate to display the menu at, in screen coordinates.
//  @parm   long | lYPos | The vertical coordinate to display the menu at, in screen coordinates.
//	@parm	<i DMUSProdStrip>* | pIStrip | Which strip to display the popup menu for.  If NULL, display the default
//		edit menu and send the results to the Timeline.
//  @parm   BOOL | fEditMenu | If TRUE, display the default edit menu.
//
//	@comm	If a custom menu is specified in <p hMenu> and <p fEditMenu> is TRUE, the items in <p hMenu>
//		will be added immediately before the "Properties" item in the edit menu.
//
//	@comm	If a valid <p pIStrip> is given, a WM_COMMAND message will be sent to <om IDMUSProdStrip::OnWMMessage>
//		if the user chooses a menu item.  No message is sent if the user cancels the context menu.
//
//	@comm	If <p hMenu> is non-NULL, <p pIStrip> should also be non-NULL to ensure the custom menu items
//		are properly handled.
//
//  @rvalue S_OK | The operation was successful.
//	@rvalue E_INVALIDARG | <p hMenu> is NULL and fEditMenu is FALSE.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>
HRESULT CTimelineCtl::TrackPopupMenu(HMENU hMenu, long x, long y, IDMUSProdStrip *pIStrip, BOOL fEditMenu)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HMENU hMenuPopup;
	CMenu MenuStrip, MenuEdit;
	IDMUSProdTimelineEdit * pITimelineEdit;
	HRESULT hr = S_OK;
	BOOL fNeedToDestroy = FALSE;

	if (fEditMenu == TRUE)
	{
		// Get default Edit menu items
		hr = MenuEdit.LoadMenu(IDR_EDIT_RMENU);
		ASSERT(SUCCEEDED( hr ));
		fNeedToDestroy = TRUE;
		hMenuPopup = ::GetSubMenu( MenuEdit.Detach(), 0 );
		hr = MenuEdit.Attach(hMenuPopup);
		ASSERT(SUCCEEDED( hr ));

		// default state of Edit items is greyed out.
		// Query to see if the strip supports any of them
		if( pIStrip && SUCCEEDED( pIStrip->QueryInterface( 
			IID_IDMUSProdTimelineEdit, (void**)&pITimelineEdit )))
		{
			EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( pITimelineEdit->CanCut() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( pITimelineEdit->CanCopy() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			EnableMenuItem( hMenuPopup, 2, ( pITimelineEdit->CanPaste( NULL ) == S_OK ) ? MF_ENABLED | MF_BYPOSITION :
					MF_GRAYED | MF_BYPOSITION );
			EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( pITimelineEdit->CanInsert() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( pITimelineEdit->CanDelete() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( pITimelineEdit->CanSelectAll() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			pITimelineEdit->Release();
		}
		else
		{
			EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED : MF_GRAYED );
			EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED : MF_GRAYED );
			EnableMenuItem( hMenuPopup, 2, ( CanPaste( NULL ) == S_OK ) ? MF_ENABLED | MF_BYPOSITION : MF_GRAYED | MF_BYPOSITION );
			EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( CanInsert() == S_OK ) ? MF_ENABLED : MF_GRAYED );
			EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED : MF_GRAYED );
			EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( CanSelectAll() == S_OK ) ? MF_ENABLED : MF_GRAYED );
		}
		
		EnableMenuItem( hMenuPopup, ID_EDIT_DELETE_TRACK, ShouldEnableDeleteTrack() ? MF_ENABLED : MF_GRAYED );

		long lFourQuarterNoteLength;
		ClocksToPosition( DMUS_PPQ * 4, &lFourQuarterNoteLength );

		EnableMenuItem( hMenuPopup, ID_EDIT_HORIZZOOMIN, ( lFourQuarterNoteLength < MAX_MEASURE_PIXELS ) ? MF_ENABLED : MF_GRAYED );
		EnableMenuItem( hMenuPopup, ID_EDIT_HORIZZOOMOUT, ( lFourQuarterNoteLength > MIN_MEASURE_PIXELS ) ? MF_ENABLED : MF_GRAYED );

		// If there is one, insert the strip's own menu immediately
		// before Properties in the Edit menu.
		if (hMenu) {
			hr = MenuStrip.Attach(hMenu);
			ASSERT( SUCCEEDED( hr ));
			InsertMenuInMenu( &MenuEdit, 8, &MenuStrip);
			MenuStrip.Detach();
		}
	}
	else // bEditMenu == FALSE
	{
		if ( hMenu )
		{
			// Only use the given menu items
			hr=MenuEdit.Attach(hMenu);
			ASSERT(SUCCEEDED( hr ));
			m_MenuHandler.m_fCustomMenu = TRUE;
		}
		else
		{
			return E_INVALIDARG;
		}
	}

	m_MenuHandler.m_pStrip = pIStrip;

	hr = MenuEdit.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, x, y, &m_MenuHandler);
	ASSERT( SUCCEEDED( hr ));

	if( fNeedToDestroy )
	{
		MenuEdit.DestroyMenu();
	}
	MenuEdit.Detach();

	m_MenuHandler.m_fCustomMenu = FALSE;

	return S_OK;
}

CWndClassInfo& CTimelineCtl::GetWndClassInfo()
{
	static CWndClassInfo wc =
	{
		{ sizeof(WNDCLASSEX), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, StartWindowProc,
		  0, 0, 0, 0, 0, (HBRUSH)(COLOR_WINDOW+1), 0, NULL, 0 },
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("")
	};
	return wc;
}

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject functions

HRESULT STDMETHODCALLTYPE CTimelineCtl::GetData( /* [retval][out] */ void **ppData)
{
	UNREFERENCED_PARAMETER( ppData );
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CTimelineCtl::SetData( /* [in] */ void *pData)
{
	UNREFERENCED_PARAMETER( pData );
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CTimelineCtl::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;
	if (m_pActiveStripList)
	{
		if (m_pActiveStripList->m_pStrip)
		{
			IDMUSProdPropPageObject* pPropPageObject;
			if ( SUCCEEDED( m_pActiveStripList->m_pStrip->QueryInterface( IID_IDMUSProdPropPageObject, (void**)&pPropPageObject ) ) )
			{
				hr = pPropPageObject->OnShowProperties();
				pPropPageObject->Release();
			}
		}
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CTimelineCtl::OnRemoveFromPageManager( void)
{
	return E_NOTIMPL;
}

// Get a list of tempos before the given time
void CTimelineCtl::GetTempoList( CList<DMUS_TEMPO_PARAM,DMUS_TEMPO_PARAM&> &lstTempos, long lTime )
{
	DWORD dwIndex = 0;
	while( TRUE )
	{
		IDMUSProdStripMgr *pTempoStripMgr = NULL;
		if( SUCCEEDED( GetStripMgr( GUID_TempoParam, 0xFFFFFFFF, dwIndex, &pTempoStripMgr ) ) )
		{
			if( S_OK != pTempoStripMgr->IsParamSupported( GUID_TimeSignature ) )
			{
				POSITION posToInsert = lstTempos.GetHeadPosition();
				MUSIC_TIME mtCur = 0, mtNextTempo = LONG_MIN;
				DMUS_TEMPO_PARAM tempo;
				while( SUCCEEDED( pTempoStripMgr->GetParam( GUID_TempoParam, mtCur, &mtNextTempo, &tempo ) ) )
				{
					// Look for an element to insert before
					if( posToInsert )
					{
						DMUS_TEMPO_PARAM tmpTempo;
						while( posToInsert )
						{
							POSITION posLast = posToInsert;
							tmpTempo = lstTempos.GetNext(posToInsert);

							if( tmpTempo.mtTime > tempo.mtTime )
							{
								posToInsert = posLast;
								lstTempos.InsertBefore( posToInsert, tempo );
								break;
							}
						}

					}

					// If we didn't find an element to insert before, insert at the end of the list
					if( posToInsert == NULL )
					{
						lstTempos.AddTail( tempo );
					}

					if( mtNextTempo == 0 )
					{
						break;
					}

					mtCur += mtNextTempo;

					if( mtCur > lTime )
					{
						break;
					}
				}
			}

			pTempoStripMgr->Release();
		}
		else
		{
			// No more strips to check - exit
			break;
		}
		dwIndex++;
	}

}

inline REFERENCE_TIME ClockToRef( REFERENCE_TIME mtTime, long double dblTempo )
{
	long double ldDivisor = dblTempo * long double(DMUS_PPQ);
	long double ldDivisorDiv2;
	if( mtTime < 0 )
	{
		ldDivisorDiv2 = -ldDivisor / 2.0;
	}
	else
	{
		ldDivisorDiv2 = ldDivisor / 2.0;
	}
	return REFERENCE_TIME((long double(mtTime * REFCLOCKS_PER_MINUTE) + ldDivisorDiv2) / ldDivisor);
}

//  @method HRESULT | IDMUSProdTimeline | ClocksToRefTime | This method converts
//		from a time in clocks to a time in REFERENCE_TIME units
//
//	@comm	Negative values for <p lTime> are valid.  The tempo at time 0 will be used
//		to compute the value to return in <p pRefTime>.
//
//  @parm   long | lTime | The time in clocks
//  @parm   REFERENCE_TIME* | pRefTime | Address of a variable to receive the time in
//		REFERENCE_TIME units this time resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p pRefTime>
//	@rvalue E_UNEXPECTED | The tempo is zero at some point.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::RefTimeToClocks>
HRESULT CTimelineCtl::ClocksToRefTime(
		/* [in] */  long		lTime,
		/* [out] */ REFERENCE_TIME		*pRefTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	if (NULL == pRefTime)
	{
		return E_POINTER;
	}

	if (NULL != m_pTempoMapMgr)
	{
		return m_pTempoMapMgr->ClocksToRefTime( lTime, pRefTime);
	}

	CList<DMUS_TEMPO_PARAM,DMUS_TEMPO_PARAM&> lstTempos;
	GetTempoList( lstTempos, lTime );

	REFERENCE_TIME rtTime = 0;
	DMUS_TEMPO_PARAM tempo;
	POSITION pos = lstTempos.GetHeadPosition();
	while( pos )
	{
		tempo = lstTempos.GetNext( pos );

		ASSERT(tempo.dblTempo > 0.0);
		if( tempo.dblTempo <= 0.0 )
		{
			return E_UNEXPECTED;
		}

		MUSIC_TIME mtNext = 0;
		if( pos )
		{
			mtNext = lstTempos.GetAt( pos ).mtTime - tempo.mtTime;
		}

		REFERENCE_TIME rtRes;
		if( !mtNext || mtNext + tempo.mtTime > lTime )
		{
			rtRes = ClockToRef( lTime - tempo.mtTime, tempo.dblTempo );
		}
		else
		{
			rtRes = ClockToRef( mtNext, tempo.dblTempo );
		}
		rtTime += rtRes;
	}

	// If there are no tempos
	if( lstTempos.GetCount() == 0 )
	{
		// Get the tempo from the time strip
		if( SUCCEEDED( m_pTimeStrip->m_pTimeStripMgr->GetParam( GUID_TempoParam, 0, NULL, &tempo ) ) )
		{
			rtTime = ClockToRef( lTime, tempo.dblTempo );
		}
	}

	*pRefTime = rtTime;
	return S_OK;
}

inline MUSIC_TIME RefToClock( REFERENCE_TIME rtTime, long double dblTempo )
{
	REFERENCE_TIME rtRoundingFactor;
	if( rtTime < 0 )
	{
		rtRoundingFactor = -REFCLOCKS_PER_MINUTE / 2;
	}
	else
	{
		rtRoundingFactor = REFCLOCKS_PER_MINUTE / 2;
	}
	REFERENCE_TIME rtResult = (REFERENCE_TIME(long double(rtTime) * dblTempo * long double(DMUS_PPQ)) + rtRoundingFactor) / REFCLOCKS_PER_MINUTE;
	return MUSIC_TIME( max( min( rtResult, LONG_MAX ), LONG_MIN ) );
}

//  @method HRESULT | IDMUSProdTimeline | RefTimeToClocks | This method converts
//		from a time in REFERENCE_TIME units to a time in clocks.
//
//	@comm	Negative values for <p RefTime> are valid, in which case the tempo at time 0 will
//		be used to compute the value of <p plTime>.
//
//  @parm   REFERENCE_TIME | RefTime | The time in REFERENCE_TIME units.
//  @parm   long* | plTime | Address of a variable to receive the time in clocks.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p plTime>
//	@rvalue E_UNEXPECTED | The tempo is zero at some point.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::ClocksToRefTime>
HRESULT CTimelineCtl::RefTimeToClocks(
		/* [in] */  REFERENCE_TIME		RefTime,
		/* [out] */ long		*plTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (NULL == plTime)
	{
		return E_POINTER;
	}

	if (NULL != m_pTempoMapMgr)
	{
		return m_pTempoMapMgr->RefTimeToClocks( RefTime, plTime );
	}

	MUSIC_TIME mtNext, mtCur = 0;
	do
	{
		mtNext = 0;

		BOOL fHaveTempoTrack = FALSE;
		double dblTempo = 120.0;
		MUSIC_TIME mtLatestTempoTime = LONG_MIN;
		DWORD dwIndex = 0;
		while( TRUE )
		{
			IDMUSProdStripMgr *pTempoStripMgr = NULL;
			if( SUCCEEDED( GetStripMgr( GUID_TempoParam, 0xFFFFFFFF, dwIndex, &pTempoStripMgr ) ) )
			{
				if( S_OK != pTempoStripMgr->IsParamSupported( GUID_TimeSignature ) )
				{
					DMUS_TEMPO_PARAM tempo;
					MUSIC_TIME mtNextTemp;
					HRESULT hr = pTempoStripMgr->GetParam( GUID_TempoParam, mtCur, &mtNextTemp, &tempo );

					if( SUCCEEDED( hr ) )
					{
						fHaveTempoTrack = TRUE;
						// If we found a real tempo and it's later than all other tempos,
						// save it to use for this time span
						if( (hr == S_OK) && (tempo.mtTime > mtLatestTempoTime) )
						{
							dblTempo = tempo.dblTempo;
							mtLatestTempoTime = tempo.mtTime;
						}
						else if( mtLatestTempoTime == LONG_MIN )
						{
							dblTempo = tempo.dblTempo;
						}

						// Look for the earliest next tempo (mtNextTempo==0 means no more tempo changes)
						if( mtNextTemp && (!mtNext || (mtNextTemp < mtNext)) )
						{
							mtNext = mtNextTemp;
						}
					}
				}

				pTempoStripMgr->Release();
			}
			else
			{
				// No more strips to check - exit
				break;
			}
			dwIndex++;
		}

		if( !fHaveTempoTrack )
		{
			DMUS_TEMPO_PARAM tempo;
			if( SUCCEEDED( m_pTimeStrip->m_pTimeStripMgr->GetParam( GUID_TempoParam, mtCur, &mtNext, &tempo ) ) )
			{
				dblTempo = tempo.dblTempo;
			}
		}

		ASSERT(dblTempo > 0.0);
		if( dblTempo <= 0.0 )
		{
			return E_UNEXPECTED;
		}

		// Convert mtNext to rtRes
		REFERENCE_TIME rtRes = ClockToRef( mtNext, dblTempo );
		if( !mtNext || RefTime <= rtRes )
		{
			// Convert RefTime to a musictime value and stuff it in plTime
			*plTime = mtCur + RefToClock( RefTime, dblTempo );
		}
		else
		{
			mtCur += mtNext;
		}
		RefTime -= rtRes;
	}
	while( mtNext && RefTime > 0 );
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | PositionToRefTime | This method converts
//		from a horizontal pixel position to a time in REFERENCE_TIME units.
//
//	@comm	Negative values for <p lPosition> are valid, in which case the tempo at time 0 will be used
//		to compute the value to return in <p pRefTime>.
//
//  @parm   long | lPosition | The horizontal pixel position.
//  @parm   REFERENCE_TIME* | pRefTime | Address of a variable to receive the time
//		in REFERENCE_TIME units this position resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as  <p pRefTime>
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::RefTimeToClocks>
HRESULT CTimelineCtl::PositionToRefTime(
		/* [in] */  long		position,
		/* [out] */ REFERENCE_TIME		*pRefTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (NULL == pRefTime)
	{
		return E_POINTER;
	}

	if (m_fMusicTime == TRUE)
	{
		long lClocks;
		HRESULT hr;
		if (SUCCEEDED(hr = PositionToClocks( position, &lClocks)))
		{
			if (NULL != m_pTempoMapMgr)
			{
				hr = m_pTempoMapMgr->ClocksToRefTime( lClocks, pRefTime);
			}
			else
			{
				hr = ClocksToRefTime( lClocks, pRefTime);
			}
		}
		return hr;
	}
	else
	{
		DMUS_TEMPO_PARAM dmTempo;
		if( FAILED( GetParam( GUID_TempoParam, 0xffffffff, 0, 0, NULL, &dmTempo ) ) )
		{
			return E_FAIL;
		}
		double dblTmp = position;
		dblTmp /= m_dblZoom * (double)DMUS_PPQ * dmTempo.dblTempo / (double)REFCLOCKS_PER_MINUTE;
		*pRefTime = (REFERENCE_TIME) dblTmp;
		return S_OK;
	}
}

//  @method HRESULT | IDMUSProdTimeline | RefTimeToPosition | This method converts
//		from a time in REFERENCE_TIME units to a horizontal pixel position
//
//	@comm	Negative values for <p RefTime> are valid, in which case the tempo at time 0 will
//		be used to compute the value of <p plPosition>.
//
//  @parm   REFERENCE_TIME | RefTime | The time in REFERENCE_TIME units.
//  @parm   long* | plPosition | Address of a variable to receive the pixel position this
//		time resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p plPosition>
//	@rvalue E_UNEXPECTED | The tempo is zero at some point.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::PositionToRefTime>
HRESULT CTimelineCtl::RefTimeToPosition(
		/* [in] */  REFERENCE_TIME		RefTime,
		/* [out] */ long		*pPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (NULL == pPosition)
	{
		return E_POINTER;
	}

	if (m_fMusicTime == TRUE)
	{
		long lClocks;
		// RefTimeToClocks will map this fn to m_pTempoMapMgr if necessary
		HRESULT hr;
		if (SUCCEEDED(hr = RefTimeToClocks( RefTime, &lClocks)))
		{
			hr = ClocksToPosition( lClocks, pPosition);
		}
		return hr;
	}
	else
	{
		DMUS_TEMPO_PARAM dmTempo;
		if( FAILED( GetParam( GUID_TempoParam, 0xffffffff, 0, 0, NULL, &dmTempo ) ) )
		{
			return E_FAIL;
		}
		double dblTmp = (double)RefTime * m_dblZoom * (double)DMUS_PPQ * dmTempo.dblTempo /
						(double)REFCLOCKS_PER_MINUTE;;
		*pPosition = (long) dblTmp;
		return S_OK;
	}
}

//  @method HRESULT | IDMUSProdTimeline | MeasureBeatToRefTime | This method converts
//		from a measure and beat to a time in REFERENCE_TIME units.
//
//	@comm	If <p lMeasure> is less than 0, the Time Signature at time 0 will
//			be used to compute <p pRefTime>.  If <p lBeat> is less than 0, the
//			Time Signature in measure <p lMeasure> will be used to compute the offset
//			from the start of measure <p lMeasure>.
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm   long | lMeasure | The measure.
//  @parm   long | lBeat | The beat.
//  @parm   REFERENCE_TIME* | pRefTime | Address of a variable to receive the
//		reference time.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | NULL was passed as <p pRefTime>.
//	@rvalue E_UNEXPECTED | The Time Signature was unable to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::RefTimeToMeasureBeat>
HRESULT CTimelineCtl::MeasureBeatToRefTime(
		/* [in] */ DWORD		dwGroupBits,
		/* [in] */ DWORD		dwIndex,
		/* [in] */  long		lMeasure,
		/* [in] */  long		lBeat,
		/* [out] */ REFERENCE_TIME		*pRefTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (NULL == pRefTime)
	{
		return E_POINTER;
	}

	long lClocks;
	HRESULT hr;
	if (SUCCEEDED( hr = MeasureBeatToClocks( dwGroupBits, dwIndex, lMeasure, lBeat, &lClocks)))
	{
		if (NULL != m_pTempoMapMgr)
		{
			return m_pTempoMapMgr->ClocksToRefTime( lClocks, pRefTime);
		}
		hr = ClocksToRefTime( lClocks, pRefTime);
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | RefTimeToMeasureBeat | This method converts
//		from a time in REFERENCE_CLOCK units to a time in measures and beats
//
//	@parm   DWORD | dwGroupBits | Which track group(s) to look for a time signature in.  A value of
//		0 is invalid.  Each bit in <p dwGroupBits> corresponds to a track group.  To look for a time
//		signature in any strip manager regardless of groups, set this parameter to 0xFFFFFFFF. 
//	@parm   DWORD | dwIndex | Zero-based index of the specified time signature to use.  This index
//		will indicate to use the nth strip manager that provides time signature information.
//  @parm   REFERENCE_TIME | RefTime | The time in REFERENCE_CLOCK units
//  @parm   long* | plMeasure | Address of a variable to receive the measure this
//							   time resolves to.
//  @parm   long* | plBeat | Address of a variable to receive the beat this
//							time resolves to.
//
//  @rvalue S_OK | The operation was successful
//  @rvalue E_POINTER | NULL was passed as <p plMeasure> or <p plBeat>
//	@rvalue E_INVALIDARG | <p RefTime> is less than 0.  <p plMeasure> and <p plBeat> are
//						   set to -1.
//	@rvalue E_UNEXPECTED | The tempo is zero at some point or the Time Signature was unable
//		to be read by a call to <om IDMUSProdTimeline::GetParam>.
//
//	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline::MeasureBeatToRefTime>
HRESULT CTimelineCtl::RefTimeToMeasureBeat(
		/* [in] */  DWORD		 dwGroupBits,
		/* [in] */  DWORD		 dwIndex,
		/* [in] */  REFERENCE_TIME		RefTime,
		/* [out] */ long		*plMeasure,
		/* [out] */ long		*plBeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( dwGroupBits == 0 )
	{
		return E_INVALIDARG;
	}

	long lClocks;
	HRESULT hr;
	if (NULL != m_pTempoMapMgr)
	{
		hr = m_pTempoMapMgr->RefTimeToClocks( RefTime, &lClocks );
	}
	else
	{
		hr = RefTimeToClocks( RefTime, &lClocks);
	}
	if ( SUCCEEDED(hr) )
	{
		hr = ClocksToMeasureBeat( dwGroupBits, dwIndex, lClocks, plMeasure, plBeat);
	}
	return hr;
}

//  @method HRESULT | IDMUSProdTimeline | ScreenToStripPosition | This method converts from a
//		<t POINT> in screen coordinates to a <t POINT> in strip coordinates.
//
//	@parm	<i IDMUSProdStrip>* | pIStrip | Which strip to get the new coordinates from.
//  @parm   <t POINT>* | pPoint | The screen point to convert.
//
//  @rvalue S_OK | The operation was successful.
//	@rvalue E_POINTER | <p pIStrip> or <p pPoint> are NULL.
//	@rvalue E_INVALIDARG | <p pIStrip> was not previously added to the Timeline.
//
//	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>
HRESULT CTimelineCtl::ScreenToStripPosition(
		/* [in] */		 IDMUSProdStrip		*pIStrip,
		/* [in] [out] */ POINT				*pPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameters
	if( NULL == pIStrip || NULL == pPoint )
	{
		return E_POINTER;
	}

	// Ensure the strip is in the timeline
	StripList* pSLToUse = FindStripList( pIStrip );
	if (pSLToUse == NULL)
	{
		return E_INVALIDARG;
	}

	// Convert the point from screen coordinates to timeline window coordinates
	ScreenToClient( pPoint );

	// Store the point's y coordinate in a working variable
	long yPos;
	yPos = pPoint->y;

	// add horizontal scroll and subtract the function bar and early time
	pPoint->x += m_lXScrollPos - m_lFunctionBarWidth - m_lLastEarlyPosition;

	// subtract any borders
	yPos -= BORDER_HORIZ_WIDTH;

	// Check if we're looking in the first strip
	if (pSLToUse == m_pStripList)
	{
		if (pSLToUse->m_sv != SV_MINIMIZED)
		{
			// add any strip vertical scrolling, if not minized
			yPos += pSLToUse->m_lVScroll;
		}
		pPoint->y = yPos;
		return S_OK;
	}

	// add timeline scroll for all except top strip, which doesn't scroll
	yPos += m_lYScrollPos;

	// subtract the heights of any strips above this one
	for( StripList* pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
	{
		if( pTempSL == pSLToUse )
		{
			break;
		}
		yPos -= ( pTempSL->m_lHeight + BORDER_HORIZ_DRAWWIDTH );
	}

	// add any strip vertical scrolling
	if (pSLToUse->m_sv != SV_MINIMIZED)
	{
		// add any strip vertical scrolling, if not minized
		yPos += pSLToUse->m_lVScroll;
	}

	pPoint->y = yPos;
	return S_OK;
}

//  @method HRESULT | IDMUSProdTimeline | StripGetTimelineProperty | This method gets a strip property
//		that is controlled by the Timeline.
//
//	@parm	<i IDMUSProdStrip>* | pIStrip | Which strip to get the property of.
//  @parm   <t STRIP_TIMELINE_PROPERTY> | stp | Which property to get.  Must be one of <t STRIP_TIMELINE_PROPERTY>.
//  @parm   VARIANT* | pVariant | The address to return to property's data in.
//
//  @rvalue S_OK | The operation was successful.
//  @rvalue E_POINTER | <p pVariant> or <p pIStrip> are NULL.
//	@rvalue E_INVALIDARG | <p stp> does not contain a valid property type, <p pIStrip> was not previously
//		added to the Timeline,	or <p pVariant> contained an invalid value for the specified property type.
//
//	@xref	<i IDMUSProdTimeline>, <t STRIP_TIMELINE_PROPERTY>,
//		<om IDMUSProdTimeline::StripSetTimelineProperty>, <i IDMUSProdStrip>
HRESULT CTimelineCtl::StripGetTimelineProperty(
		/* [in] */	IDMUSProdStrip*	pIStrip,
		/* [in] */	STRIP_TIMELINE_PROPERTY stp,
		/* [out] */	VARIANT*	pVariant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = E_INVALIDARG;

	if( NULL == pIStrip || NULL == pVariant )
	{
		return E_POINTER;
	}

	if (m_pStripList == NULL)
	{
		return E_INVALIDARG;
	}

	StripList* pSL = FindStripList( pIStrip );
	if( pSL == NULL )
	{
		return E_INVALIDARG;
	}

	switch( stp )
	{
	case STP_VERTICAL_SCROLL:
		pVariant->vt = VT_I4;
		V_I4(pVariant) = pSL->m_lVScroll;
		hr = S_OK;
		break;

	case STP_ENTIRE_STRIP_RECT:
		if( (pVariant->vt == VT_BYREF) && (V_BYREF(pVariant) != NULL) )
		{
			RECT *pRect = (RECT *)V_BYREF(pVariant);
			GetEntireStripClientRect( pSL, pRect );
			hr = S_OK;
		}
		break;
	case STP_STRIP_RECT:
	case STP_FBAR_RECT:
	case STP_FBAR_CLIENT_RECT:
		if( (pVariant->vt == VT_BYREF) && (V_BYREF(pVariant) != NULL) )
		{
			RECT *pRect = (RECT *)V_BYREF(pVariant);
			GetStripClientRect( pSL, pRect );
			if( stp == STP_STRIP_RECT )
			{
				pRect->left += m_lFunctionBarWidth;
			}
			else if( stp == STP_FBAR_RECT )
			{
				pRect->right = m_lFunctionBarWidth;
			}
			else // STP_FBAR_CLIENT_RECT
			{
				pRect->bottom = pRect->bottom - pRect->top;
				pRect->top = 0;
				pRect->right = m_lFunctionBarWidth - GUTTER_WIDTH - BORDER_VERT_WIDTH;
			}
			if( pRect->left > pRect->right )
			{
				// there is no client area to draw in, so fail
				pRect->left = 0;
				pRect->right = 0;
				pRect->bottom = 0;
				pRect->top = 0;
				hr = E_FAIL;
			}
			else
			{
				hr = S_OK;
			}
		}
		break;

	case STP_GET_HDC:
		// Only succeed if m_hWnd is actually a real window
		if( ::IsWindow( m_hWnd ) )
		{
			HDC hdc;
			hdc = GetDC();

			if( hdc )
			{
				CDC dc;
				if( dc.Attach( hdc ) )
				{
					RECT rect;

					GetStripClientRect( pSL, &rect );
					rect.top += BORDER_HORIZ_WIDTH;
					rect.left += m_lFunctionBarWidth;
					hr = S_OK;
					if( rect.left > rect.right )
					{
						// there is no client area to draw in, so return
						// a null hdc
						ReleaseDC(hdc);
						hdc = NULL;
						hr = E_FAIL;
					}
					else
					{
						CRgn rgn;
						// set window org so m_lXScrollPos,0 is top left of strip
						dc.SetWindowOrg( -rect.left, -rect.top );
						// set clip region so we can't plot outside of strip
						rgn.CreateRectRgn( rect.left, rect.top, rect.right, rect.bottom );
						dc.SelectClipRgn( &rgn );
						rgn.DeleteObject();
					}
					dc.Detach();
				}
				else
				{
					ReleaseDC(hdc);
					hdc = NULL;
					hr = E_FAIL;
				}
			}
			else
			{
				hr = E_FAIL;
			}
			pVariant->vt = VT_I4;
			V_I4(pVariant) = (long)hdc;
		}
		else
		{
			hr = E_FAIL;
		}
		break;

	case STP_HEIGHT:
		pVariant->vt = VT_I4;
		hr = S_OK;
		if ( pSL->m_sv == SV_NORMAL )
		{
			V_I4(pVariant) = pSL->m_lHeight;
		}
		else if ( pSL->m_sv == SV_MINIMIZED )
		{
			V_I4(pVariant) = pSL->m_lRestoreHeight;
		}
		else
		{
			V_I4(pVariant) = -1;
			hr = E_UNEXPECTED;
		}
		break;

	case STP_STRIPVIEW:
		pVariant->vt = VT_I4;
		V_I4(pVariant) = (int) pSL->m_sv;
		hr = S_OK;
		break;

	case STP_POSITION:
		{
			pVariant->vt = VT_I4;
			StripList*	pScan;
			long lResult = -1, lPosition = 0;
			for( pScan = m_pStripList; pScan; pScan = pScan->m_pNext )
			{
				if( pScan->m_pStrip == pIStrip )
				{
					lResult = lPosition;
					break;
				}
				lPosition++;
			}
			if( lResult == -1 )
			{
				hr = E_INVALIDARG;
				V_I4(pVariant) = lResult;
			}
			else
			{
				hr = S_OK;
				V_I4(pVariant) = lResult;
			}
		}
		break;

	case STP_GUTTER_SELECTED:
		pVariant->vt = VT_BOOL;
		V_BOOL(pVariant) = (short)pSL->m_fSelected;
		hr = S_OK;
		break;

	default:
		hr = E_INVALIDARG;
		break;
	}
	return hr;
}

HRESULT CTimelineCtl::StripPointToClient( StripList* pSL, POINT *pPoint) const
{
	// Validate parameters
	ASSERT( pSL && pPoint );
	if ((NULL == pSL)||(NULL == pPoint))
	{
		return E_INVALIDARG;
	}

	// Store the point's y coordinate in a working variable
	long yPos;
	yPos = pPoint->y;

	// subtract horizontal scroll and add the function bar and early time
	pPoint->x -= m_lXScrollPos - m_lFunctionBarWidth - m_lLastEarlyPosition;

	// add any borders
	yPos += BORDER_HORIZ_WIDTH;

	// Check if we're looking in the first strip
	if (pSL == m_pStripList)
	{
		if (pSL->m_sv != SV_MINIMIZED)
		{
			// subtract any strip vertical scrolling, if not minized
			yPos -= pSL->m_lVScroll;
		}
		pPoint->y = yPos;
		return S_OK;
	}

	// subtract timeline scroll for all except top strip, which doesn't scroll
	yPos -= m_lYScrollPos;

	// add the heights of any strips above this one
	for( StripList* pTempSL = m_pStripList; pTempSL; pTempSL = pTempSL->m_pNext )
	{
		if( pSL == pTempSL )
		{
			break;
		}
		yPos += ( pTempSL->m_lHeight + BORDER_HORIZ_DRAWWIDTH );
	}

	if ( pTempSL == NULL )
	{
		// Strip not in our list.
		return E_INVALIDARG;
	}

	if (pSL->m_sv != SV_MINIMIZED)
	{
		// subtract any strip vertical scrolling, if not minized
		yPos -= pSL->m_lVScroll;
	}

	pPoint->y = yPos;
	return S_OK;
}

HRESULT CTimelineCtl::StripRectToClient( StripList* pSL, LPRECT pRect) const
{
	POINT point;
	point.x = 0;
	point.y = 0;

	HRESULT hr;
	hr = StripPointToClient( pSL, &point );
	if ( SUCCEEDED(hr) )
	{
		pRect->top += point.y;
		pRect->bottom += point.y;
		pRect->left += point.x;
		pRect->right += point.x;
	}
	return hr;
}

bool CTimelineCtl::AnyGutterSelectedStrips( void ) const
{
	StripList* pSL;
	for( pSL = m_pStripList; pSL; pSL = pSL->m_pNext )
	{
		if( (pSL->m_pStrip != NULL) && pSL->m_fSelected )
		{
			return true;
		}
	}

	return false;
}

HRESULT CTimelineCtl::CanStripEdit( IDMUSProdTimelineEdit* pITimelineEdit, long lCommand )
{
	HRESULT hr = S_FALSE;

	switch( lCommand )
	{
	case IDC_TIMELINE_CUT:
		hr = pITimelineEdit->CanCut();
		break;
	case IDC_TIMELINE_COPY:
		hr = pITimelineEdit->CanCopy();
		break;
	case IDC_TIMELINE_PASTE:
		{
			// Get the IDataObject from the clipboard
			IDataObject *pIDataObject;
			if( SUCCEEDED( OleGetClipboard(&pIDataObject) ) )
			{
				// Create a new TimelineDataObject
				IDMUSProdTimelineDataObject *pITimelineDataObject;
				if( SUCCEEDED( AllocTimelineDataObject( &pITimelineDataObject ) ) )
				{
					// Insert the IDataObject into the TimelineDataObject
					if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
					{
						hr = pITimelineEdit->CanPaste( pITimelineDataObject );
					}
					pITimelineDataObject->Release();
				}
				pIDataObject->Release();
			}
		}
		break;
	case IDC_TIMELINE_INSERT:
		ASSERT(FALSE);
		// Should never get here
		//hr = pITimelineEdit->CanInsert();
		break;
	case IDC_TIMELINE_DELETE:
		hr = pITimelineEdit->CanDelete();
		break;
	case IDC_TIMELINE_SELECTALL:
		hr = pITimelineEdit->CanSelectAll();
		break;
	}

	return hr == S_OK ? S_OK : S_FALSE;
}

void CTimelineCtl::DisplayPropertySheet( void )
{
	// Get a pointer to the property sheet and show it
	IDMUSProdPropSheet*	pIPropSheet;
	if( m_pDMUSProdFramework )
	{
		if( SUCCEEDED( m_pDMUSProdFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			pIPropSheet->Show( TRUE );
			pIPropSheet->Release();
		}
	}
}

LRESULT CTimelineCtl::OnTimer(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& /*lResult*/)
{
	if( m_pActiveStripList )
	{
		CallStripWMMessage( m_pActiveStripList, nMsg, wParam, lParam );
	}
	return 0;
}

void CTimelineCtl::GetEntireStripClientRect( StripList* pSL, LPRECT pRect )
{
	// Display format:
	// BORDER_HORIZ_DRAWWIDTH
	// Strip
	// BORDER_HORIZ_DRAWWIDTH
	// Strip
	// BORDER_HORIZ_DRAWWIDTH
	// Strip
	// BORDER_HORIZ_DRAWWIDTH

	// BORDER_HORIZ_WIDTH == BORDER_HORIZ_DRAWWIDTH

	// Ensure pRect is valid
	ASSERT( pRect );

	// Set the left and right boundaries
	ClocksToPosition( m_lLength, &pRect->right );
	pRect->right += m_lLastEarlyPosition + m_lLastLatePosition;
	pRect->right -= m_lXScrollPos;
	pRect->left = -m_lXScrollPos;

	// If we're getting the rect for the top strip
	if( pSL == m_pStripList )
	{
		pRect->top = BORDER_HORIZ_WIDTH;
		pRect->bottom = pRect->top + pSL->m_lHeight;
	}
	else
	{
		// Offset everything by the timeline's vertical scroll position
		pRect->top = -m_lYScrollPos;
		// Add up the heights of all strips above ours to set pRect->top and pRect->bottom
		for( StripList* pList = m_pStripList; pList; pList = pList->m_pNext )
		{
			pRect->bottom = pRect->top + pList->m_lHeight + BORDER_HORIZ_WIDTH;
			if( pList == pSL )
			{
				break;
			}
			// The previous's strip bottom is our top, except if the previous strip is the first
			// strip and there is a vertical scroll
			pRect->top = pRect->bottom;
		}
		pRect->top += BORDER_HORIZ_WIDTH;
	}
}

bool CTimelineCtl::ComputeEarlyAndLateTime( long &lEarlyTime, long &lLateTime )
{
	// Initialize the times to 0.
	lEarlyTime = 0;
	lLateTime = 0;

	VARIANT varTime;
	for( StripList* pList = m_pStripList; pList; pList = pList->m_pNext )
	{
		if( SUCCEEDED( pList->m_pStrip->GetStripProperty( SP_EARLY_TIME, &varTime ) ) )
		{
			lEarlyTime = max( lEarlyTime, V_I4(&varTime) );
		}

		if( SUCCEEDED( pList->m_pStrip->GetStripProperty( SP_LATE_TIME, &varTime ) ) )
		{
			lLateTime = max( lLateTime, V_I4(&varTime) );
		}
	}

	return (lEarlyTime != 0) || (lLateTime != 0);
}

bool CTimelineCtl::BumpTimeCursor( bool fMoveRight, DMUSPROD_TIMELINE_SNAP_TO stAmount )
{
	// TODO: Use the dwIndex value also?
	DWORD dwGroupBits = m_pActiveStripList ? m_pActiveStripList->m_dwGroupBits : 0xFFFFFFFF;

	long lNewCursorPos = m_lCursor;

	switch( stAmount )
	{
	case DMUSPROD_TIMELINE_SNAP_BAR:
		{
			long lMeasure;
			if( SUCCEEDED( ClocksToMeasureBeat( dwGroupBits, 0, m_lCursor, &lMeasure, NULL ) ) )
			{
				long lMeasureStart, lNextMeasureStart;
				if( SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure, 0, &lMeasureStart ) )
				&&	SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure + 1, 0, &lNextMeasureStart ) ) )
				{
					if( fMoveRight )
					{
						// If we're not in the last measure
						if( lNextMeasureStart < m_lLength )
						{
							long lNewMeasureEnd;
							if( SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure + 2, 0, &lNewMeasureEnd ) ) )
							{
								// Keep the cursor the same % of the way through the bar
								lNewCursorPos = lNextMeasureStart + ((m_lCursor - lMeasureStart) * (lNewMeasureEnd - lNextMeasureStart)) / (lNextMeasureStart - lMeasureStart);
							}
						}
					}
					else
					{
						// If we're not in the first measure
						if( lMeasure != 0 )
						{
							long lNewMeasureStart;
							if( SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure - 1, 0, &lNewMeasureStart ) ) )
							{
								// Keep the cursor the same % of the way through the bar
								lNewCursorPos = lNewMeasureStart + ((m_lCursor - lMeasureStart) * (lMeasureStart - lNewMeasureStart)) / (lNextMeasureStart - lMeasureStart);
							}
						}
					}
				}
			}
		}
		break;
	case DMUSPROD_TIMELINE_SNAP_NONE:
		lNewCursorPos += fMoveRight ? 1 : -1;
		// Boundary conditions are handled at the bottom of this function
		break;
	case DMUSPROD_TIMELINE_SNAP_GRID:
		{
			long lMeasure, lBeat;
			if( SUCCEEDED( ClocksToMeasureBeat( dwGroupBits, 0, m_lCursor, &lMeasure, &lBeat ) ) )
			{
				DMUS_TIMESIGNATURE currentTimeSig;
				long lBeatStart;
				if( SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure, lBeat, &lBeatStart ) )
				&&	SUCCEEDED( GetParam( GUID_TimeSignature, dwGroupBits, 0, lBeatStart, NULL, &currentTimeSig ) ) )
				{
					const long lCurrentBeatLength = (DMUS_PPQ * 4) / currentTimeSig.bBeat;
					const long lCurrentGridLength = lCurrentBeatLength / currentTimeSig.wGridsPerBeat;
					long lGridStart = (m_lCursor - lBeatStart) / lCurrentGridLength;
					lGridStart *= lCurrentGridLength;
					lGridStart += lBeatStart;

					if( fMoveRight )
					{
						long lNewGridEnd = lGridStart + lCurrentGridLength * 2;
						if( lNewGridEnd > lBeatStart + lCurrentBeatLength )
						{
							// Check if we did not move past the end of the segment
							if( lBeatStart + lCurrentBeatLength < m_lLength )
							{
								// Moved into new beat, so get new time sig
								DMUS_TIMESIGNATURE newTimeSig;
								if( SUCCEEDED( GetParam( GUID_TimeSignature, dwGroupBits, 0, lBeatStart + lCurrentBeatLength + 1, NULL, &newTimeSig ) ) )
								{
									// Check if the TimeSigs are the same (don't care about # of beats per measure)
									if( (newTimeSig.bBeat == currentTimeSig.bBeat)
									&&	(newTimeSig.wGridsPerBeat == currentTimeSig.wGridsPerBeat) )
									{
										// Keep the cursor the same % of the way through the grid
										// Can't just add grid length because there may be a non-even number of grids per beat
										lNewCursorPos = m_lCursor - lGridStart + lBeatStart + lCurrentBeatLength;
									}
									else
									{
										const long lNewGridLength = ((DMUS_PPQ * 4) / newTimeSig.bBeat) / newTimeSig.wGridsPerBeat;

										// Keep the cursor the same % of the way through the grid
										lNewCursorPos = lBeatStart + lCurrentBeatLength + ((m_lCursor - lGridStart) * lNewGridLength) / lCurrentGridLength;
									}
								}
							}
						}
						else
						{
							// Check if we did not move past the end of the segment
							if( lGridStart + lCurrentGridLength < m_lLength )
							{
								// Keep the cursor the same % of the way through the grid
								lNewCursorPos = m_lCursor + lCurrentGridLength;
							}
						}
					}
					else
					{
						// Check if not moving before start of the segment
						if( lGridStart != 0 )
						{
							long lNewGridStart = lGridStart - lCurrentGridLength;
							if( lNewGridStart < lBeatStart )
							{
								// Moved into new beat, so get new time sig
								DMUS_TIMESIGNATURE newTimeSig;
								if( SUCCEEDED( GetParam( GUID_TimeSignature, dwGroupBits, 0, lBeatStart - 1, NULL, &newTimeSig ) ) )
								{
									// Check if the TimeSigs are the same (don't care about # of beats per measure)
									if( (newTimeSig.bBeat == currentTimeSig.bBeat)
									&&	(newTimeSig.wGridsPerBeat == currentTimeSig.wGridsPerBeat) )
									{
										// Keep the cursor the same % of the way through the grid
										// Can't just subtract grid length because there may be a non-even number of grids per beat
										lNewCursorPos = m_lCursor - lCurrentBeatLength + (currentTimeSig.wGridsPerBeat - 1) * lCurrentGridLength;
									}
									else
									{
										const long lNewBeatLength = (DMUS_PPQ * 4) / newTimeSig.bBeat;
										const long lNewGridLength = lNewBeatLength / newTimeSig.wGridsPerBeat;

										// Keep the cursor the same % of the way through the grid
										// Can't just subtract grid length because there may be a non-even number of grids per beat
										lNewCursorPos = lBeatStart - lNewBeatLength + (newTimeSig.wGridsPerBeat - 1) * lNewGridLength + ((m_lCursor - lGridStart) * lNewGridLength) / lCurrentGridLength;
									}
								}
							}
							else
							{
								// Keep the cursor the same % of the way through the grid
								lNewCursorPos = m_lCursor - lCurrentGridLength;
							}
						}
					}
				}
			}
		}
		break;
	case DMUSPROD_TIMELINE_SNAP_BEAT:
		{
			long lMeasure, lBeat;
			if( SUCCEEDED( ClocksToMeasureBeat( dwGroupBits, 0, m_lCursor, &lMeasure, &lBeat ) ) )
			{
				long lBeatStart, lNextBeatStart;
				if( SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure, lBeat, &lBeatStart ) )
				&&	SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure, lBeat + 1, &lNextBeatStart ) ) )
				{
					if( fMoveRight )
					{
						// If the next beat starts before the segment
						if( lNextBeatStart < m_lLength )
						{
							long lNewBeatEnd, lNextMeasureStart;
							if( SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure, lBeat + 2, &lNewBeatEnd ) )
							&&	SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure + 1, 0, &lNextMeasureStart ) ) )
							{
								// Check if we're moving into a new measure
								if( lNewBeatEnd > lNextMeasureStart )
								{
									// Yes - find the end of the first beat in the new measure
									if( SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure + 1, 1, &lNewBeatEnd ) ) )
									{
										// Keep the cursor the same % of the way through the beat
										lNewCursorPos = lNextBeatStart + ((m_lCursor - lBeatStart) * (lNewBeatEnd - lNextBeatStart)) / (lNextBeatStart - lBeatStart);
									}
								}
								else
								{
									// Keep the cursor the same % of the way through the beat
									lNewCursorPos = lNextBeatStart + ((m_lCursor - lBeatStart) * (lNewBeatEnd - lNextBeatStart)) / (lNextBeatStart - lBeatStart);
								}
							}
						}
					}
					else // Move left
					{
						// If we're not in the first beat
						if( lBeatStart != 0 )
						{
							long lNewBeatStart, lThisMeasureStart;
							if( SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure, lBeat - 1, &lNewBeatStart ) )
							&&	SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure, 0, &lThisMeasureStart ) ) )
							{
								// Check if we're moving into a new measure
								if( lNewBeatStart < lThisMeasureStart )
								{
									// Yes - find the start of the last beat in the new measure
									DMUS_TIMESIGNATURE TimeSig;
									if( SUCCEEDED( GetParam( GUID_TimeSignature, dwGroupBits, 0, lThisMeasureStart - 1, NULL, &TimeSig ) )
									&&	SUCCEEDED( MeasureBeatToClocks( dwGroupBits, 0, lMeasure - 1, TimeSig.bBeatsPerMeasure - 1, &lNewBeatStart ) ) )
									{
										// Keep the cursor the same % of the way through the beat
										lNewCursorPos = lNewBeatStart + ((m_lCursor - lBeatStart) * (lBeatStart - lNewBeatStart)) / (lNextBeatStart - lBeatStart);
									}
								}
								else
								{
									// Keep the cursor the same % of the way through the beat
									lNewCursorPos = lNewBeatStart + ((m_lCursor - lBeatStart) * (lBeatStart - lNewBeatStart)) / (lNextBeatStart - lBeatStart);
								}
							}
						}
					}
				}
			}
		}
		break;
	}

	lNewCursorPos = max( 0, min( lNewCursorPos, m_lLength - 1 ) );

	if( lNewCursorPos != m_lCursor )
	{
		SetTimeCursor( lNewCursorPos, m_pTimeStrip->m_fScrollWhenSettingTimeCursor, true );
		return true;
	}

	return false;
}

bool CTimelineCtl::ShouldEnableDeleteTrack( void )
{
	// Only enable Delete Track if any strips are gutter selected, or if the 
	// there is an active strip and it is not the first strip.
	return AnyGutterSelectedStrips()
		   || (m_pActiveStripList && (m_pActiveStripList != m_pStripList));
}

void CTimelineCtl::SetTopStripList( StripList* pSL )
{
	RECT rectStrip;
	GetEntireStripClientRect( pSL, &rectStrip );

	VARIANT var;
	var.vt = VT_I4;
	V_I4( &var ) = rectStrip.top + m_lYScrollPos - m_pStripList->m_lHeight - BORDER_HORIZ_WIDTH*2;
	SetTimelineProperty( TP_VERTICAL_SCROLL, var );
}

/* TODO: Implement
StripList *CTimelineCtl::GetTopStripList( void )
{
	return m_pStripList;
}
*/
