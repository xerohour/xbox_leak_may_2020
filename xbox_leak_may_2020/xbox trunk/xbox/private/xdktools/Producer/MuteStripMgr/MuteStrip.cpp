// MuteStrip.cpp : Implementation of CMuteStrip
#include "stdafx.h"
#include "MuteItem.h"
#include "MuteStripMgr.h"
#include "MuteMgr.h"
#include "DLLJazzDataObject.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#include <dmusici.h>
#include <dmusicf.h>
#include "RemapDlg.h"
#include "RiffStrm.h"
#include "SegmentIO.h"
#include "DlgAddPChannel.h"
#include "GrayOutRect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// height and width constants
const c_nChannelHeight = 12;
const c_nMinHeight = c_nChannelHeight * 2;
const c_nDefaultHeight = c_nChannelHeight * 4;
const c_nChannelWidth = 20;

// clipboard format
#define CF_MUTELIST "DMUSProd v.1 Mutelist"

// control id for vertical scrollbar
#define IDC_VSCROLL 100

// used for horizontal scroll
#define FIRST_SCROLL_ID		1
#define SECOND_SCROLL_ID	2
#define SCROLL_HORIZ_AMOUNT 20 


MUSIC_TIME CMuteStrip::m_mtBeginCopied = 0;	
MUSIC_TIME CMuteStrip::m_mtEndCopied = 0;	

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip constructor/destructor

CMuteStrip::CMuteStrip(CMuteMgr* pMuteMgr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(pMuteMgr);
	
	m_pMuteMgr = pMuteMgr;
	m_pStripMgr = (IDMUSProdStripMgr*)pMuteMgr;
	//m_pStripMgr->AddRef();

	// initialize our reference count
	m_cRef = 0;
	AddRef();

	m_EditMode = emNone;

	m_bGutterSelected = FALSE;
	m_mtBeginSelect = 0;
	m_mtEndSelect = 0;
	m_nLastEdit = 0;
	m_fPropPageActive = FALSE;
	m_pPropPageMgr = NULL;

	UpdateName();

	// default to show the first 16 PChannels
	for (int i = 0; i < 16; i++) {
		m_arrChannels.Add(i);
	}

	m_lVScroll = 0;
	memset(&m_StateData, 0, sizeof(m_StateData));
	m_fLoadedStateData = false;

	m_nScrollTimerID = 0;
	m_fLButtonDown = false;
	m_dwTimerCount = 0;
}

CMuteStrip::~CMuteStrip()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_nScrollTimerID )
	{
		StopScrollTimer();
	}

	m_pStripMgr = NULL;
	m_pMuteMgr = NULL;
	
	if (m_pPropPageMgr)
	{
		m_pPropPageMgr->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::QueryInterface

STDMETHODIMP CMuteStrip::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( ppv );
	if ( ppv == NULL )
	{
		return E_INVALIDARG;
	}

    *ppv = NULL;

    if (IsEqualIID(riid, IID_IUnknown))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip*) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStrip))
	{
        *ppv = (IUnknown *) (IDMUSProdStrip *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdStripFunctionBar))
	{
        *ppv = (IUnknown *) (IDMUSProdStripFunctionBar *) this;
	}
	else if (IsEqualIID(riid, IID_IDMUSProdPropPageObject))
	{
        *ppv = (IUnknown *) (IDMUSProdPropPageObject *) this;
	}
	else if( IsEqualIID( riid, IID_IDMUSProdTimelineEdit ))
	{
		*ppv = (IDMUSProdTimelineEdit*) this;
	}
	else
	{
		return E_NOTIMPL;
	}

    ((IUnknown *) *ppv)->AddRef();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::AddRef

STDMETHODIMP_(ULONG) CMuteStrip::AddRef(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return ++m_cRef;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::Release

STDMETHODIMP_(ULONG) CMuteStrip::Release(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if ( 0L == --m_cRef )
	{
		delete this;
		return 0;
	}
	else
	{
		return m_cRef;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::GetPosRect
//
// Get a bounding rectangle for the given measure, beat.  chIndex is the
// index of the pchannel in m_lstChannels.
//
void CMuteStrip::GetPosRect(long chIdx, long lMeasure, long lBeat, LONG lXOffset, LPRECT pRectBox)
{
	m_pMuteMgr->m_pTimeline->MeasureBeatToPosition(m_pMuteMgr->m_dwGroupBits, 0, lMeasure, lBeat, &pRectBox->left);
	m_pMuteMgr->m_pTimeline->MeasureBeatToPosition(m_pMuteMgr->m_dwGroupBits, 0, lMeasure, lBeat+1, &pRectBox->right);
	pRectBox->top = chIdx * c_nChannelHeight;
	pRectBox->bottom = pRectBox->top + c_nChannelHeight;

	int nWidth = pRectBox->right - pRectBox->left;
	int nHeight = pRectBox->bottom - pRectBox->top;

	if (min(nWidth, nHeight) > 10) 
	{
		::InflateRect(pRectBox, -2, -2);
		nWidth = pRectBox->right - pRectBox->left;
		nHeight = pRectBox->bottom - pRectBox->top;
	}
	else if	(min(nWidth, nHeight) > 8)
	{
		::InflateRect(pRectBox, -1, -1);
		nWidth = pRectBox->right - pRectBox->left;
		nHeight = pRectBox->bottom - pRectBox->top;
	}

	// make it square
	POINT ptCenter;
	ptCenter.x = pRectBox->left + (nWidth >> 1);
	ptCenter.y = pRectBox->top + (nHeight >> 1);
	int nMinExtent = min(nHeight, nWidth) >> 1;
	::SetRect(pRectBox, ptCenter.x - nMinExtent, ptCenter.y - nMinExtent, 
		ptCenter.x + nMinExtent, ptCenter.y + nMinExtent);
	
	pRectBox->left -= lXOffset;
	pRectBox->right -= lXOffset;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::YPosToChannelIdx
//
// Given a y position, return the index of the channel in m_arrChannels.
// Takes vertical scrolling into account.  Note that a number that is
// out of the index bounds of the array may be returned.
//
int CMuteStrip::YPosToChannelIdx(long lYPos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return lYPos / c_nChannelHeight;// + m_lVScroll;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::DrawMuteItem
//
// Draw the given mute item for the bars it spans.
//
void CMuteStrip::DrawMuteItem(HDC hDC, CMuteItem* pMuteItem, int nChanIdx, 
							  MUSIC_TIME mtMax, LONG lXOffset) 
{
	// get index of pchannel
	ASSERT(nChanIdx < m_arrChannels.GetSize());

	long lMeasure, lBeat;
	pMuteItem->GetStartMeasureBeat(lMeasure, lBeat);
	
	// Get the clipping rectangle
	RECT rectClip;
	::GetClipBox( hDC, &rectClip );
	long lStartMeasure, lStartBeat, lEndTime;
	m_pMuteMgr->m_pTimeline->PositionToMeasureBeat( m_pMuteMgr->m_dwGroupBits, 0, rectClip.left + lXOffset, &lStartMeasure, &lStartBeat );
	m_pMuteMgr->m_pTimeline->PositionToClocks( rectClip.right + lXOffset, &lEndTime );

	mtMax = min(min(pMuteItem->GetEndTime(), mtMax), lEndTime);

	if( lMeasure < lStartMeasure )
	{
		lMeasure = lStartMeasure;
		lBeat = lStartBeat;
	}

	MUSIC_TIME mtCurTime;

	// draw the beats that apply to this mute item
	do {
		RECT rectBox;
		GetPosRect(nChanIdx, lMeasure, lBeat, lXOffset, &rectBox);
			
		if (pMuteItem->IsMute()) {
			// Draw 'X'
			::MoveToEx(hDC, rectBox.left, rectBox.top, NULL);
			::LineTo(hDC, rectBox.right, rectBox.bottom);
			::MoveToEx(hDC, rectBox.right, rectBox.top, NULL);
			::LineTo(hDC, rectBox.left, rectBox.bottom);
		}
		else {
			// Draw PChannel string
			TCHAR achText[20];
			_itot( pMuteItem->GetPChannelMap()+1, achText, 10 );

			int nCenterX = rectBox.left + ((rectBox.right - rectBox.left) >> 1);

			::SetTextAlign(hDC, TA_CENTER | TA_TOP);
			::TextOut(hDC, nCenterX, rectBox.top, achText, _tcslen(achText));
		}
		m_pMuteMgr->m_pTimeline->MeasureBeatToClocks( m_pMuteMgr->m_dwGroupBits, 0, lMeasure, ++lBeat, &mtCurTime);
		m_pMuteMgr->m_pTimeline->ClocksToMeasureBeat( m_pMuteMgr->m_dwGroupBits, 0, mtCurTime, &lMeasure, &lBeat);
	}
	while (mtCurTime < mtMax);
			
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::InvalidatePosition
//
// Invalidate the given measure,beat position.
//
void CMuteStrip::InvalidatePosition(long chIdx, long lMeasure, long lBeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long lMaxPos;
	m_pMuteMgr->m_pTimeline->MeasureBeatToPosition(m_pMuteMgr->m_dwGroupBits, 0, lMeasure, lBeat+1, &lMaxPos);

	RECT rect;
	GetPosRect(chIdx, lMeasure, lBeat, 0, &rect);

	InflateRect(&rect, 4, 4);
	rect.right = min( rect.right, (lMaxPos - 1) );
	m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, &rect, true);
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip IDMUSProdStrip implementation


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::Draw

HRESULT	STDMETHODCALLTYPE CMuteStrip::Draw(HDC hDC, STRIPVIEW sv, LONG lXOffset)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDC	dc;
	MUSIC_TIME mtMax;	// end time
	VARIANT var;
	int nMaxX;

	ASSERT( m_pMuteMgr != NULL );
	ASSERT( m_pMuteMgr->m_pTimeline != NULL );

	m_svView = sv;
	if( sv == SV_NORMAL )
	{
		UpdateVScroll();
	}
	
	if (!dc.Attach(hDC)) {
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	if (sv == SV_MINIMIZED) {
		
		// we are in minimized mode; so draw text to indicate the 
		// pchannels that this strip applies to.
		CString str;
		str.LoadString(IDS_PCHANNELS);
		CString strPrev;
		
		DWORD startChan;
		if (m_arrChannels.GetSize() > 0) 
			startChan = m_arrChannels[0];
		for (int i = 0; i < m_arrChannels.GetSize(); i++) {
			DWORD chan = m_arrChannels[i];
			if (i < m_arrChannels.GetSize()-1 && chan == m_arrChannels[i+1]-1) {
				continue;
			}
			else {
				strPrev = str;
				CString strFmt;
				if (i == m_arrChannels.GetSize()-1) {
					if (startChan == chan)
						strFmt = "%s %d.";
					else
						strFmt = "%s %d-%d.";
					str.Format(strFmt, strPrev, startChan+1, chan+1);
				}
				else {
					if (startChan == chan)
						strFmt = "%s %d,";
					else
						strFmt = "%s %d-%d,";
					str.Format(strFmt, strPrev, startChan+1, chan+1);
					startChan = m_arrChannels[i+1];
				}
			}
		}

		// setup the font
		CFont font;
		CFont* pOldFont;
		if (font.CreatePointFont(80, "Arial")) {
			pOldFont = dc.SelectObject(&font);
		}
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextAlign(TA_LEFT | TA_TOP);
		
		// draw the text
		dc.TextOut(0, 0, str);

		// delete the font
		if (pOldFont) {
			dc.SelectObject(pOldFont);
			font.DeleteObject();
		}
	}
	else if (sv == SV_NORMAL) {

		// Get end time and position
		if (FAILED(m_pMuteMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var))) {
			hr = E_FAIL;
			goto ON_EXIT;
		}
		mtMax = V_I4(&var);
		if (FAILED(m_pMuteMgr->m_pTimeline->ClocksToPosition(mtMax, (long*)&nMaxX))) {
			hr = E_FAIL;
			goto ON_EXIT;
		}

		// Draw measure and beat lines in our strip
		m_pMuteMgr->m_pTimeline->DrawMusicLines(hDC, ML_DRAW_MEASURE_BEAT, m_pMuteMgr->m_dwGroupBits, 0, lXOffset);

		long lYOffset = 0;

		// draw pchannel lines
		int nCurY = 0;
		for (int i = 0; i < m_arrChannels.GetSize(); i++) {
			nCurY += c_nChannelHeight;

			dc.MoveTo(0, nCurY);
			dc.LineTo(nMaxX, nCurY);
		}

		// prepare the font
		CFont font;
		CFont* pOldFont = NULL;
		if (font.CreatePointFont(70, "Small Fonts")) {
			pOldFont = dc.SelectObject(&font);
		}
		dc.SetBkMode(TRANSPARENT);
		
		// prepare the pen
		CPen pen;
		CPen *pPenOld = NULL;
		if (pen.CreatePen(PS_SOLID, 2, ::GetSysColor(COLOR_WINDOWTEXT))) {
			 pPenOld = dc.SelectObject(&pen);
		}

		// Get the clipping rectangle
		RECT rectClip;
		dc.GetClipBox( &rectClip );
		long lStartTime, lEndTime;
		m_pMuteMgr->m_pTimeline->PositionToClocks( rectClip.left + lXOffset, &lStartTime );
		m_pMuteMgr->m_pTimeline->PositionToClocks( rectClip.right + lXOffset, &lEndTime );
		
		// draw each mute item
		int nChanIdx = 0;
		POSITION pos = m_pMuteMgr->m_lstMutes.GetHeadPosition();
		while (pos) {
			CMuteItem* pItem = m_pMuteMgr->m_lstMutes.GetNext(pos);
			// If any part of this item is visible, draw it
			if( (pItem->GetEndTime() > lStartTime) && (pItem->GetStartTime() < lEndTime) )
			{
				while (m_arrChannels[nChanIdx] != pItem->GetPChannel()) {
					ASSERT(m_arrChannels[nChanIdx] < pItem->GetPChannel());
					nChanIdx++;
				}
				DrawMuteItem(dc, pItem, nChanIdx, mtMax, lXOffset);
			}
		}

		// delete the pen
		if (pPenOld) {
			dc.SelectObject(pPenOld);
			pen.DeleteObject();
		}

		// delete the font
		if (pOldFont) {
			dc.SelectObject(pOldFont);
			font.DeleteObject();
		}
	}

	// inidcate selected area
	MUSIC_TIME mtBeginTime, mtEndTime;
	GetSelectTimes(mtBeginTime, mtEndTime);

	if (mtBeginTime != mtEndTime) {
		m_pMuteMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip*)this, STP_HEIGHT, &var);
		long lHeight = V_I4(&var);

		// calc begin and end x coord for selection area
		long lBeginX, lEndX;
		m_pMuteMgr->m_pTimeline->ClocksToPosition(mtBeginTime, (long*)&lBeginX);
		m_pMuteMgr->m_pTimeline->ClocksToPosition(mtEndTime, (long*)&lEndX);
		lBeginX = min(lBeginX, 32767);
		lEndX = min(lEndX, 32767);
		CRect rect(lBeginX, 0, lEndX, lHeight);
		rect.OffsetRect(-lXOffset, m_lVScroll * c_nChannelHeight);
		
		// do the inverting
		GrayOutRect( dc.m_hDC, rect );
	}


ON_EXIT:
	dc.Detach();		
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::GetStripProperty

HRESULT STDMETHODCALLTYPE CMuteStrip::GetStripProperty( STRIPPROPERTY sp, VARIANT *pvar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( NULL == pvar )
	{
		return E_POINTER;
	}

	switch( sp )
	{
	case SP_RESIZEABLE:
		// We are resizable
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = TRUE;
		break;
	case SP_GUTTERSELECTABLE:
		// We support gutter selection
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = TRUE;
		break;
	case SP_MINMAXABLE:
		// We support Minimize/maximize
		pvar->vt = VT_BOOL;
		V_BOOL(pvar) = TRUE;
		break;
	case SP_DEFAULTHEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = c_nDefaultHeight;
		break;
	case SP_MAXHEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = max(c_nMinHeight, m_arrChannels.GetSize() * c_nChannelHeight);
		break;
	case SP_MINHEIGHT:
		pvar->vt = VT_INT;
		V_INT(pvar) = c_nMinHeight;
		break;
	case SP_NAME:
		{
			BSTR bstr;

			pvar->vt = VT_BSTR; 
			try
			{
				bstr = m_strName.AllocSysString();
			}
			catch(CMemoryException*)
			{
				return E_OUTOFMEMORY;
			}
			V_BSTR(pvar) = bstr;
		}
		break;
	case SP_STRIPMGR:
		pvar->vt = VT_UNKNOWN;
		if( m_pMuteMgr )
		{
			m_pMuteMgr->QueryInterface( IID_IUnknown, (void **) &V_UNKNOWN(pvar) );
		}
		else
		{
			V_UNKNOWN(pvar) = NULL;
		}
		break;
	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::SetStripProperty

HRESULT STDMETHODCALLTYPE CMuteStrip::SetStripProperty( STRIPPROPERTY sp, VARIANT var)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	switch (sp)
	{
	case SP_BEGINSELECT:
	case SP_ENDSELECT:
		if (var.vt != VT_I4)
		{
			return E_FAIL;
		}
		if (sp == SP_BEGINSELECT)
		{
			m_mtBeginSelect = V_I4(&var);
		}
		else
		{
			m_mtEndSelect = V_I4(&var);
		}
		
		if (m_mtBeginSelect == m_mtEndSelect)
		{	
			m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, TRUE);
			break;
		}
	
		/*
		if( m_bGutterSelected )
		{
			m_pMuteMgr->SelectSegment( m_mtBeginSelect, m_mtEndSelect );
		}
		else
		{
			m_pMuteMgr->UnselectAll();
		}
		*/
		 m_pMuteMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );

		// Update the property page
		if( m_pMuteMgr->m_pPropPageMgr != NULL )
		{
			m_pMuteMgr->m_pPropPageMgr->RefreshData();
		}
		break;
	case SP_GUTTERSELECT:
		m_bGutterSelected = V_BOOL(&var);
		m_pMuteMgr->m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)this, NULL, TRUE );
		break;
	default:
		return E_FAIL;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnWMMessage
//
// Handles windows messages in the strip.
//
HRESULT STDMETHODCALLTYPE CMuteStrip::OnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Process the window message
	HRESULT hr = S_OK;
	if (m_pMuteMgr->m_pTimeline == NULL)
	{
		hr = E_FAIL;
		goto ON_EXIT;
	}
	switch( nMsg )
	{
	case WM_TIMER:
		OnTimer();
		break;

	case WM_LBUTTONDOWN:
		m_fLButtonDown = true;
		hr = OnLButtonDown(wParam, lParam, lXPos, lYPos);
		break;

	case WM_MOUSEMOVE:
		hr = OnMouseMove(wParam, lParam, lXPos, lYPos);
		break;
	
	case WM_LBUTTONUP:
		StopScrollTimer();
		m_fLButtonDown = false;
		hr = OnLButtonUp(wParam, lParam, lXPos, lYPos);
		break;

	case WM_RBUTTONDOWN:
		break;

	case WM_RBUTTONUP:
		hr = OnRButtonUp(wParam, lParam, lXPos, lYPos);
		break;
		
	case WM_VSCROLL:
		hr = OnVScroll(wParam, lParam);
		break;
	case WM_MOVE:
	case WM_SIZE:
		hr = OnSize( wParam, lParam );
		break;

	case WM_COMMAND:
		// We should only get this message in response to a selection in the right-click context menu.
		WORD wNotifyCode;
		WORD wID;

		wNotifyCode	= HIWORD( wParam );	// notification code 
		wID			= LOWORD( wParam );	// item, control, or accelerator identifier 
		switch( wID )
		{
		case ID_VIEW_PROPERTIES:
			hr = OnShowProperties(m_pMuteMgr->m_pTimeline);
			// Change to our property page
			OnShowProperties();
			break;
		
		case CM_ADDREMOVE:
			DoAddRemoveDlg();
			break;

		case CM_REMAP:
			DoRemapDlg();
			break;
		
		case CM_UNREMAP:
			if (m_nClickedIdx < 0 || m_nClickedIdx >= m_arrChannels.GetSize()) {
				// we are outside a pchannel
				break;
			}
			m_pMuteMgr->ClearRemap(m_lClickedMeasure, m_lClickedBeat,
				m_arrChannels[m_nClickedIdx]);
			m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, true);
			m_nLastEdit = IDS_UNDO_CLEAR_REMAP;
			m_pMuteMgr->OnDataChanged();
			break;

		case CM_MUTEALL:
		{
			if (m_nClickedIdx >= m_arrChannels.GetSize()) {
				// we are outside a pchannel
				break;
			}
			// mute the entire pchannel
			m_pMuteMgr->MuteEntirePChannel(m_arrChannels[m_nClickedIdx]);
			m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, true);
			m_nLastEdit = IDS_UNDO_MUTE_PCHANNEL;
			m_pMuteMgr->OnDataChanged();
			break;
		}
		
		case CM_UNMUTEALL:
		{
			if (m_nClickedIdx < 0 || m_nClickedIdx >= m_arrChannels.GetSize()) {
				// we are outside a pchannel
				break;
			}
			// unmute the entire pchannel
			m_pMuteMgr->UnmuteEntirePChannel(m_arrChannels[m_nClickedIdx]);
			m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, true);
			m_nLastEdit = IDS_UNDO_UNMUTE_PCHANNEL;
			m_pMuteMgr->OnDataChanged();
			break;
		}

		case CM_CLEARALL:
			if (m_nClickedIdx < 0 || m_nClickedIdx >= m_arrChannels.GetSize()) {
				// we are outside a pchannel
				break;
			}
			// unmute the entire pchannel
			m_pMuteMgr->RemoveChannelData(m_arrChannels[m_nClickedIdx]);
			m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, true);
			m_nLastEdit = IDS_UNDO_CLEAR_PCHANNEL;
			m_pMuteMgr->OnDataChanged();
			break;

		case ID_EDIT_DELETE:
			hr = Delete();
			break;
		case ID_EDIT_CUT:
			hr = Cut();
			break;
		case ID_EDIT_COPY:
			hr = Copy();
			break;
		case ID_EDIT_PASTE:
			hr = Paste();
			break;
		case ID_EDIT_SELECT_ALL:
			hr = SelectAll();
			m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, true);
			break;
		default:
			break;
		}
		break;

	case WM_CREATE:
		m_cfMuteList = RegisterClipboardFormat(CF_MUTELIST);
		UpdateName();

		// Get Left and right selection boundaries
		m_bGutterSelected = FALSE;
		m_pMuteMgr->m_pTimeline->GetMarkerTime( MARKER_BEGINSELECT, TIMETYPE_CLOCKS, &m_mtBeginSelect );
		m_pMuteMgr->m_pTimeline->GetMarkerTime( MARKER_ENDSELECT, TIMETYPE_CLOCKS, &m_mtEndSelect );
		
		// setup the vertical scrollbar
		if (m_VScrollBar.GetSafeHwnd() == NULL) {
			IOleInPlaceObjectWindowless* pIOleInPlaceObjectWindowless;
			m_pMuteMgr->m_pTimeline->QueryInterface(IID_IOleWindow, (void**)&pIOleInPlaceObjectWindowless);
			
			if (pIOleInPlaceObjectWindowless) {
				HWND hWnd;
				if (pIOleInPlaceObjectWindowless->GetWindow(&hWnd) == S_OK)	{
					CWnd wnd;
					wnd.Attach(hWnd);
					m_rectVScroll = CRect(0, 0, 40, 100);
					m_VScrollBar.Create(SBS_RIGHTALIGN | SBS_VERT | WS_CHILD | WS_CLIPSIBLINGS,
						m_rectVScroll, &wnd, IDC_VSCROLL);
					wnd.Detach();
					m_VScrollBar.SetMuteStrip(this);
					pIOleInPlaceObjectWindowless->Release();
				}
				UpdateVScroll();
			}
		}
		break;
	
	default:
		break;
	}

ON_EXIT:
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip IDMUSProdStripFunctionBar

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::FBDraw
//
// Draw the pchannel numbers.
//
HRESULT CMuteStrip::FBDraw(HDC hDC, STRIPVIEW sv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (sv == SV_FUNCTIONBAR_MINIMIZED)
	{
		// hide the scrolbar
		m_VScrollBar.ShowScrollBar(FALSE);
		return S_OK;
	}

	m_VScrollBar.ShowScrollBar(TRUE);
	
	CDC dc;
	if (!dc.Attach(hDC)) {
		return E_FAIL;
	}

	// get function bar rect
	CRect rectFBar;
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBar;
	if (FAILED(m_pMuteMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_FBAR_CLIENT_RECT, &var)))
	{
		return E_FAIL;
	}

	RECT rectFBarTimeline;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rectFBarTimeline;
	if (FAILED(m_pMuteMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_FBAR_RECT, &var)))
	{
		return E_FAIL;
	}

	// fix the clipping region
	RECT rectClip;
	dc.GetClipBox( &rectClip );

	POINT point = dc.GetWindowOrg();

	rectClip.left -= point.x;
	rectClip.right -= point.x;
	rectClip.top = rectFBarTimeline.top;
	rectClip.bottom = rectFBarTimeline.bottom;

	CRgn clipRgn;
	clipRgn.CreateRectRgnIndirect( &rectClip );
	dc.SelectClipRgn( &clipRgn );
	clipRgn.DeleteObject();

	long lXPos = rectFBar.right;

	CFont font;
	CFont* pOldFont = NULL;

	if (font.CreatePointFont(70, "Small Fonts")) {
		pOldFont = dc.SelectObject(&font);
	}
	
	dc.SetBkMode(TRANSPARENT);
	dc.SelectStockObject(LTGRAY_BRUSH);
	dc.SetTextAlign(TA_CENTER | TA_TOP);
	dc.SetTextColor(RGB(0,0,0));
	
	for (int i = 0; i < m_arrChannels.GetSize(); i++) {
		long lYPos = c_nChannelHeight * i;
		CString strChannel;
		strChannel.Format("%d", m_arrChannels[i]+1);
		dc.Rectangle(lXPos - c_nChannelWidth, lYPos, lXPos+1, lYPos + c_nChannelHeight + 1);
		dc.TextOut(lXPos - c_nChannelWidth / 2, lYPos, strChannel);
	}

	if (pOldFont) {
		dc.SelectObject(pOldFont);
		font.DeleteObject();
	}
	
	dc.Detach();
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::FBOnWMMessage

HRESULT CMuteStrip::FBOnWMMessage( UINT nMsg, WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;
	switch( nMsg )
	{
	case WM_LBUTTONUP:
		StopScrollTimer();
		m_fLButtonDown = FALSE;
		hr = OnLButtonUp(wParam, lParam, lXPos, lYPos);
		break;
	case WM_LBUTTONDOWN:
		m_fLButtonDown = TRUE;
		OnShowProperties();
		break;
	case WM_MOUSEMOVE:
		if( m_EditMode == emToggleOff 
		||  m_EditMode == emToggleOn )
		{
			// May need to scroll, cursor is NOT in the visible portion of the strip
			if( m_nScrollTimerID == 0 )
			{
				StartScrollTimer( FIRST_SCROLL_ID );
			}
		}
		break;
	case WM_RBUTTONUP:
		OnShowProperties();

		// Display a right-click context menu.
		POINT pt;
		// Get the cursor position (To put the menu there)
		if(!GetCursorPos(&pt))
		{
			hr = E_UNEXPECTED;
			break;
		}

		if (m_pMuteMgr->m_pTimeline)
		{
			HMENU hMenu = ::LoadMenu( _Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_EDIT_RMENU) );
			HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

			::EnableMenuItem( hMenuPopup, CM_ADDREMOVE, MF_ENABLED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			::EnableMenuItem( hMenuPopup, 4, ( CanPaste( NULL ) == S_OK ) ? MF_ENABLED | MF_BYPOSITION :
					MF_GRAYED | MF_BYPOSITION );
			::EnableMenuItem( hMenuPopup, ID_EDIT_INSERT, ( CanInsert() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_EDIT_SELECT_ALL, ( CanSelectAll() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
			::EnableMenuItem( hMenuPopup, ID_VIEW_PROPERTIES, MF_ENABLED );
			
			m_pMuteMgr->m_pTimeline->TrackPopupMenu(hMenuPopup, pt.x, pt.y, (IDMUSProdStrip *)this, FALSE);
			::DestroyMenu( hMenu );
		}
		break;
	}
	
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip IDMUSProdTimelineEdit

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::Cut

HRESULT CMuteStrip::Cut(IDMUSProdTimelineDataObject* pITimelineDataObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr = Copy(pITimelineDataObject);

	if( SUCCEEDED ( hr ) )
	{
		m_pMuteMgr->DeleteRange(m_mtBeginCopied, m_mtEndCopied);
		m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, true);
		m_nLastEdit = IDS_UNDO_DELETE_MUTE_REMAP;
		m_pMuteMgr->OnDataChanged();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::Copy

HRESULT CMuteStrip::Copy(IDMUSProdTimelineDataObject* pITimelineDataObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT				hr;
	IStream*			pStreamCopy;

	hr = CanCopy();
	ASSERT(hr == S_OK);
	if (hr != S_OK)
	{
		return E_UNEXPECTED;
	}

	ASSERT(m_pMuteMgr != NULL);
	if (m_pMuteMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	// If the format hasn't been registered yet, do it now.
	if (m_cfMuteList == 0)
	{
		m_cfMuteList = RegisterClipboardFormat(CF_MUTELIST);
		if (m_cfMuteList == 0)
		{
			return E_FAIL;
		}
	}

	// Create an IStream to save the selected commands in.
	hr = CreateStreamOnHGlobal(NULL, TRUE, &pStreamCopy);
	if (FAILED(hr))
	{
		return E_OUTOFMEMORY;
	}

	long mtBeginTime, mtEndTime;
	GetSelectTimes(mtBeginTime, mtEndTime);

	// Save the commands into the stream.
	hr = m_pMuteMgr->SaveRange(pStreamCopy, mtBeginTime, mtEndTime);
	if (FAILED(hr))
	{
		pStreamCopy->Release();
		return E_UNEXPECTED;
	}

	if(pITimelineDataObject != NULL)
	{
		// add the stream to the passed ITimelineDataObject
		hr = pITimelineDataObject->AddInternalClipFormat(m_cfMuteList, pStreamCopy);
		pStreamCopy->Release();
		ASSERT(hr == S_OK);
		if (hr != S_OK)
		{
			return E_FAIL;
		}
	}
	// Otherwise, add it to the clipboard
	else
	{
		// There is no existing data object, so just create a new one
		hr = m_pMuteMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject );
		ASSERT( hr == S_OK );
		if( hr != S_OK )
		{
			return E_FAIL;
		}

		// Set the start and edit time of this copy
		hr = pITimelineDataObject->SetBoundaries( mtBeginTime, mtEndTime );

		// add the stream to the DataObject
		hr = pITimelineDataObject->AddInternalClipFormat( m_cfMuteList, pStreamCopy );

		// Release the IStream we copied into
		RELEASE( pStreamCopy );

		// Exit with an error if we failed to add the stream
		ASSERT( hr == S_OK );
		if ( hr != S_OK )
		{
			RELEASE( pITimelineDataObject );
			return E_FAIL;
		}

		// get the IDataObject to place on the clipboard
		IDataObject* pIDataObject;
		hr = pITimelineDataObject->Export( &pIDataObject );

		// Release the IDMUSProdTimelineDataObject
		RELEASE( pITimelineDataObject );

		// Exit if the export failed
		if( FAILED(hr) )
		{
			return E_UNEXPECTED;
		}

		// Send the IDataObject to the clipboard
		hr = OleSetClipboard( pIDataObject );

		// Exit if we failed to set the clipboard with our data
		if( hr != S_OK )
		{
			// Release the IDataObject
			RELEASE( pIDataObject );
			return E_FAIL;
		}

		// If we already have a CopyDataObject, release it
		RELEASE( m_pMuteMgr->m_pCopyDataObject);

		// Set m_pCopyDataObject to the object we just copied to the clipboard
		m_pMuteMgr->m_pCopyDataObject = pIDataObject;

		// Not needed - Object was AddRef()'d when it was exported from the IDMUSProdTimelineDataObject
		// m_pMuteMgr->m_pCopyDataObject->AddRef();
	}
	
	if (SUCCEEDED(hr)) {
		// remember copied begin and end times
		// note: this is a workaround because timeline->GetBoundaries fails.
		m_mtBeginCopied = mtBeginTime;
		m_mtEndCopied = mtEndTime;
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::Paste

HRESULT CMuteStrip::Paste(IDMUSProdTimelineDataObject* pITimelineDataObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pMuteMgr);

	HRESULT			hr;
	IDataObject		*pIDataObject = NULL;
	
	// Make sure we can paste
	hr = CanPaste(pITimelineDataObject);
	ASSERT(hr == S_OK);
	if (hr != S_OK)
	{
		goto ON_EXIT;
	}

	// Get a Timeline pointer
	ASSERT(m_pMuteMgr != NULL);
	if (m_pMuteMgr == NULL)
	{
		hr = E_UNEXPECTED;
		goto ON_EXIT;
	}

	ASSERT(m_pMuteMgr->m_pTimeline != NULL);
	if (m_pMuteMgr->m_pTimeline == NULL)
	{
		hr = E_UNEXPECTED;
		goto ON_EXIT;
	}

	// If the format hasn't been registered yet, do it now.
	if (m_cfMuteList == 0)
	{
		m_cfMuteList = RegisterClipboardFormat(CF_MUTELIST);
		if (m_cfMuteList == 0)
		{
			hr = E_FAIL;
			goto ON_EXIT;
		}
	}
	
	if (pITimelineDataObject == NULL)
	{
		// Get the IDataObject from the clipboard
		hr = OleGetClipboard(&pIDataObject);
		if (FAILED(hr) || (pIDataObject == NULL))
		{
			hr = E_FAIL;
			goto ON_EXIT;
		}

		// Create a new TimelineDataObject
		hr = m_pMuteMgr->m_pTimeline->AllocTimelineDataObject(&pITimelineDataObject);
		if (FAILED(hr) || (pITimelineDataObject == NULL))
		{
			hr = E_FAIL;
			goto ON_EXIT;
		}

		// Insert the IDataObject into the TimelineDataObject
		hr = pITimelineDataObject->Import(pIDataObject);
		if (FAILED(hr))
		{
			goto ON_EXIT;
		}
		RELEASE(pIDataObject);
	}
	else
	{
		pITimelineDataObject->AddRef();
	}

	// check if clipboard format available
	if (SUCCEEDED(pITimelineDataObject->IsClipFormatAvailable(m_cfMuteList)))
	{
		// get insertion point (mtBegin)
		MUSIC_TIME mtBegin, mtEnd;
		GetSelectTimes(mtBegin, mtEnd);
		
		// get the stream
		IStream* pIStream;
		if (SUCCEEDED(pITimelineDataObject->AttemptRead(m_cfMuteList, &pIStream)))
		{
			// do the paste operation
			hr = m_pMuteMgr->LoadPaste(pIStream, mtBegin, m_mtBeginCopied, m_mtEndCopied);
		}
	}

ON_EXIT:
	RELEASE(pIDataObject);
	RELEASE(pITimelineDataObject);
	// If successful, redraw our strip
	if (SUCCEEDED(hr)) {
		m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip *)this, NULL, TRUE);
		m_nLastEdit = IDS_UNDO_PASTE_MUTE_REMAP;
		m_pMuteMgr->OnDataChanged();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::Insert

HRESULT CMuteStrip::Insert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return E_NOTIMPL;

}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::Delete

HRESULT CMuteStrip::Delete( void )
{
	ASSERT(m_pMuteMgr);
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	long mtBeginTime, mtEndTime;
	GetSelectTimes(mtBeginTime, mtEndTime);
	
	m_pMuteMgr->DeleteRange(mtBeginTime, mtEndTime);

	m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, true);
	m_nLastEdit = IDS_UNDO_DELETE_MUTE_REMAP;
	m_pMuteMgr->OnDataChanged();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::SelectAll

HRESULT CMuteStrip::SelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(m_pMuteMgr);
	ASSERT(m_pMuteMgr->m_pTimeline);

	VARIANT var;
	if (FAILED(m_pMuteMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var))) {
		return E_FAIL;
	}

	m_mtBeginSelect = 0;
	m_mtEndSelect = V_I4(&var);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::CanCut

HRESULT CMuteStrip::CanCut( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(m_pMuteMgr != NULL);
	if (m_pMuteMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	if (CanCopy() == S_OK && CanDelete() == S_OK)
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::CanCopy

HRESULT CMuteStrip::CanCopy( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_mtBeginSelect != m_mtEndSelect && m_bGutterSelected) {
		return S_OK;
	}
	else {
		return E_NOTIMPL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::CanPaste

HRESULT CMuteStrip::CanPaste(IDMUSProdTimelineDataObject* pITimelineDataObject)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT( m_pMuteMgr->m_pTimeline != NULL );

	HRESULT hr = E_FAIL;

	ASSERT(m_pMuteMgr != NULL);
	if (m_pMuteMgr == NULL)	{
		return E_UNEXPECTED;
	}

	if( m_bGutterSelected == FALSE )
	{
		// Can only paste when the mute strip's gutter is selected
		return S_FALSE;
	}

	// If the format hasn't been registered yet, do it now.
	if (m_cfMuteList == 0)	{
		m_cfMuteList = RegisterClipboardFormat(CF_MUTELIST);
		if (m_cfMuteList == 0) {
			return E_FAIL;
		}
	}

	// If pITimelineDataObject != NULL, check it.
	if (pITimelineDataObject != NULL) {
		hr = pITimelineDataObject->IsClipFormatAvailable(m_cfMuteList);
	}
	// Otherwise, check the clipboard
	else {
		// Get the IDataObject from the clipboard
		IDataObject *pIDataObject;
		if( SUCCEEDED( OleGetClipboard(&pIDataObject) ) )
		{
			// Create a new TimelineDataObject
			IDMUSProdTimelineDataObject *pITimelineDataObject;
			if( SUCCEEDED( m_pMuteMgr->m_pTimeline->AllocTimelineDataObject( &pITimelineDataObject ) ) )
			{
				// Insert the IDataObject into the TimelineDataObject
				if( SUCCEEDED( pITimelineDataObject->Import( pIDataObject ) ) )
				{
					hr = pITimelineDataObject->IsClipFormatAvailable( m_cfMuteList );
				}
				pITimelineDataObject->Release();
			}
			pIDataObject->Release();
		}
	}

	if( hr == S_OK )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::CanInsert

HRESULT CMuteStrip::CanInsert( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::CanDelete

HRESULT CMuteStrip::CanDelete( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_mtBeginSelect != m_mtEndSelect && m_bGutterSelected) {
		return S_OK;
	}
	else {
		return S_FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::CanSelectAll

HRESULT CMuteStrip::CanSelectAll( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_bGutterSelected) {
		return S_OK;
	}
	else {
		return S_FALSE;
	}
}


// IDMUSProdPropPageObject Methods

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::GetData

HRESULT CMuteStrip::GetData( void **ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( ppData == NULL )
	{
		return E_INVALIDARG;
	}

	// Check which property page is requesting the data
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(*ppData);
	switch( *pdwIndex )
	{
	case GROUPBITSPPG_INDEX:
	{
		// Copy our groupbits to the location pointed to by ppData
		PPGTrackParams *pPPGTrackParams = static_cast<PPGTrackParams *>(*ppData);
		pPPGTrackParams->dwGroupBits = m_pMuteMgr->m_dwGroupBits;
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		// Copy our track setting to the location pointed to by ppData
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(*ppData);
		pPPGTrackFlagsParams->dwTrackExtrasFlags = m_pMuteMgr->m_dwTrackExtrasFlags;
		pPPGTrackFlagsParams->dwTrackExtrasMask = TRACKCONFIG_VALID_MASK;
		pPPGTrackFlagsParams->dwProducerOnlyFlags = m_pMuteMgr->m_dwProducerOnlyFlags;
		pPPGTrackFlagsParams->dwProducerOnlyMask = SEG_PRODUCERONLY_AUDITIONONLY;
		break;
	}
	default:
		ASSERT(FALSE);
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::SetData

HRESULT CMuteStrip::SetData( void *pData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( pData == NULL )
	{
		return E_INVALIDARG;
	}

	// Check which property page is setting the data
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
	case GROUPBITSPPG_INDEX:
	{
		PPGTrackParams *pPPGTrackParams = reinterpret_cast<PPGTrackParams *>(pData);

		// Update our group bits setting, if necessary
		if( pPPGTrackParams->dwGroupBits != m_pMuteMgr->m_dwGroupBits )
		{
			m_pMuteMgr->m_dwGroupBits = pPPGTrackParams->dwGroupBits;

			UpdateName();

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_TRACK_GROUP;
			m_pMuteMgr->m_pTimeline->OnDataChanged( (IDMUSProdStripMgr*)m_pMuteMgr );

			// Update m_dwOldGroupBits after the call to OnDataChanged, because it is needed
			// to ensure the StripMgre removes itself correctly from the Timeline's notification
			// list.
			m_pMuteMgr->m_dwOldGroupBits = pPPGTrackParams->dwGroupBits;
		}
		break;
	}
	case TRACKFLAGSPPG_INDEX:
	{
		PPGTrackFlagsParams *pPPGTrackFlagsParams = reinterpret_cast<PPGTrackFlagsParams *>(pData);

		// Update our track extras flags, if necessary
		if( pPPGTrackFlagsParams->dwTrackExtrasFlags != m_pMuteMgr->m_dwTrackExtrasFlags )
		{
			m_pMuteMgr->m_dwTrackExtrasFlags = pPPGTrackFlagsParams->dwTrackExtrasFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_TRACKEXTRAS;
			m_pMuteMgr->OnDataChanged();
		}
		// Update our Producer-specific flags, if necessary
		else if( pPPGTrackFlagsParams->dwProducerOnlyFlags != m_pMuteMgr->m_dwProducerOnlyFlags )
		{
			m_pMuteMgr->m_dwProducerOnlyFlags = pPPGTrackFlagsParams->dwProducerOnlyFlags;

			// Notify our editor that we've changed
			m_nLastEdit = IDS_UNDO_PRODUCERONLY;
			m_pMuteMgr->OnDataChanged();
		}
		break;
	}
	default:
		ASSERT(FALSE);
		return E_INVALIDARG;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnShowProperties

HRESULT CMuteStrip::OnShowProperties()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IDMUSProdFramework* pIFramework = NULL;
	IDMUSProdPropSheet* pIPropSheet = NULL;
	HRESULT hr = S_OK;
	VARIANT var;

	// Get a pointer to the Timeline
	if (m_pMuteMgr->m_pTimeline == NULL)
	{
		hr = E_FAIL;
		goto EXIT;
	}

	// Get a pointer to the Framework from the timeline
	m_pMuteMgr->m_pTimeline->GetTimelineProperty(TP_DMUSPRODFRAMEWORK, &var);
	pIFramework = (IDMUSProdFramework*) V_UNKNOWN(&var);
	if (pIFramework == NULL)
	{
		hr = E_FAIL;
		goto EXIT;
	}

	// Get a pointer to the property sheet
	pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void **)&pIPropSheet);
	RELEASE(pIFramework);
	if (pIPropSheet == NULL)
	{
		hr = E_FAIL;
		goto EXIT;
	}

	//  If the property sheet is hidden, exit
	if (pIPropSheet->IsShowing() != S_OK)
	{
		hr = S_OK;
		goto EXIT;
	}

	// If our property page is already displayed, exit
	if (m_fPropPageActive)
	{
		ASSERT(m_pPropPageMgr != NULL);
		hr = S_OK;
		goto EXIT;
	}

	// Get a reference to our property page manager
	if(m_pPropPageMgr == NULL)
	{
		CGroupBitsPropPageMgr* pPPM = new CGroupBitsPropPageMgr;
		if (NULL == pPPM) {
			hr = E_OUTOFMEMORY;
			goto EXIT;
		}
			
		hr = pPPM->QueryInterface(IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr);
		m_pPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
		if (FAILED(hr)) {
			goto EXIT;
		}
	}

	// Set the property page to refer to the Piano Roll property page.
	short nActiveTab;
	nActiveTab = CGroupBitsPropPageMgr::sm_nActiveTab;
	m_pMuteMgr->m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	m_fPropPageActive = TRUE;
	pIPropSheet->SetActivePage( nActiveTab ); 

EXIT:
	RELEASE(pIPropSheet);
	RELEASE(pIFramework);
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnRemoveFromPageManager

HRESULT CMuteStrip::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if (m_pPropPageMgr)
	{
		m_pPropPageMgr->SetObject(NULL);
	}
	m_fPropPageActive = FALSE;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnShowProperties

HRESULT CMuteStrip::OnShowProperties(IDMUSProdTimeline* pTimeline)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;

	// Get a pointer to the property sheet and show it
	VARIANT			var;
	LPUNKNOWN		punk;
	IDMUSProdPropSheet*	pIPropSheet;
	pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var );
	if( var.vt == VT_UNKNOWN )
	{
		punk = V_UNKNOWN( &var );
		if( punk )
		{
			hr = punk->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet );
			ASSERT( SUCCEEDED( hr ));
			if( FAILED( hr ))
			{
				hr = E_UNEXPECTED;
			}
			else
			{
				pIPropSheet->Show( TRUE );
				pIPropSheet->Release();
			}
			punk->Release();
		}
	}
	m_pMuteMgr->OnShowProperties();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::UpdateVScroll
//
// Updates the vertical scrollbar's position, range and page size.  This
// is called when the mute strip is resized, or pchannels are added
// or removed.
//
void CMuteStrip::UpdateVScroll()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRect rect;
	VARIANT var;
	var.vt = VT_BYREF;
	V_BYREF(&var) = &rect;

	VERIFY(SUCCEEDED(m_pMuteMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip *)this, STP_FBAR_RECT, &var)));

	CRect oldRect = m_rectVScroll;
	
	// move the vertical scroll bar to where it needs to go
	m_VScrollBar.GetClientRect(&m_rectVScroll);
	m_rectVScroll.left = rect.right - m_rectVScroll.right - /*BORDER_HORIZ_WIDTH - */c_nChannelWidth;
	m_rectVScroll.right = m_rectVScroll.left + m_rectVScroll.right;
	m_rectVScroll.top = rect.top;
	m_rectVScroll.bottom = rect.bottom;
	
	// move the scroll bar
	m_VScrollBar.MoveWindow(&m_rectVScroll, TRUE);

	// compute the scrollbar range and pagesize
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.nMin = 0;
	si.nMax = m_arrChannels.GetSize() - (rect.Height() / c_nChannelHeight);
	if (si.nMax < 1) {
		// disable the scrollbar
		m_VScrollBar.EnableScrollBar(ESB_DISABLE_BOTH);
		m_VScrollBar.ShowWindow(TRUE);
	}
	else {
		// enable the scrollbar
		m_VScrollBar.EnableScrollBar(ESB_ENABLE_BOTH);
		m_VScrollBar.ShowWindow(TRUE);
		// set the range
		si.fMask = SIF_RANGE;
		m_VScrollBar.SetScrollInfo(&si, TRUE);
		
		if (m_rectVScroll != oldRect)
		{
			// set the new page size
			si.fMask = SIF_PAGE;
			si.nPage = 1;
			m_VScrollBar.SetScrollInfo(&si, TRUE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::SetVScroll
//
// Set a new vertical scroll value for the mute strip.  The scrollbar and
// strip are updated accordingly.
//
void CMuteStrip::SetVScroll(long lNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long lOldScroll = m_lVScroll;
	m_lVScroll = lNewValue;
	LimitVScroll();
	if (lOldScroll == m_lVScroll) {
		return;
	}
	VARIANT var;
	var.vt = VT_I4;
	V_I4(&var) = m_lVScroll * c_nChannelHeight;
	m_pMuteMgr->m_pTimeline->StripSetTimelineProperty((IDMUSProdStrip*)this, STP_VERTICAL_SCROLL, var);

	m_VScrollBar.SetScrollPos(m_lVScroll);
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::LimitVScroll
//
// Limits the Y scroll value to be within acceptable range.
//
void CMuteStrip::LimitVScroll() 
{
	CRect rect;
	m_VScrollBar.GetClientRect(&rect);
	long lMax = max( 0, m_arrChannels.GetSize() - rect.Height() / c_nChannelHeight );

	if (m_lVScroll < 0) {
		m_lVScroll = 0;
	}
	else if (m_lVScroll > lMax) {
		m_lVScroll = lMax;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnVScroll
//
// Called by the vertical scrollbar when the user scrolls.
//
void CMuteStrip::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRect rect;
	m_VScrollBar.GetClientRect(&rect);
	long lNewPos;
	switch(nSBCode)
	{
	case SB_TOP:
		lNewPos = 0;
		break;
	case SB_LINEDOWN:
		lNewPos = m_lVScroll + 1;
		break;
	case SB_LINEUP:
		lNewPos = m_lVScroll - 1;
		break;
	case SB_PAGEDOWN:
		lNewPos = m_lVScroll + (rect.Height() / c_nChannelHeight);
		break;
	case SB_PAGEUP:
		lNewPos = m_lVScroll - (rect.Height() / c_nChannelHeight);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		lNewPos = nPos;
		break;
	case SB_BOTTOM:
		lNewPos = m_arrChannels.GetSize() - (rect.Height() / c_nChannelHeight);
		break;
	case SB_ENDSCROLL:
	default:
		return;
	}

	SetVScroll(lNewPos);
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnVScroll
//
// Windows message handler for vertical scrolling.
//
HRESULT CMuteStrip::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// update position of scrollbar
	UpdateVScroll();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnSize
//
// Message handler for strip resizing.
//
HRESULT CMuteStrip::OnSize(WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// update position of scrollbar
	UpdateVScroll();
	// make sure current scroll position is within range (since size was changed)
	SetVScroll(m_lVScroll);
	//m_pMuteMgr->OnDataChanged();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::GetMyWindow
//
// Retrieves the handle of the window which this strip is in.
//
HWND CMuteStrip::GetMyWindow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	IOleWindow* pIOleWindow;
	m_pMuteMgr->m_pTimeline->QueryInterface(IID_IOleWindow, (void**)&pIOleWindow);
	ASSERT(pIOleWindow);
	
	HWND hWnd;

	VERIFY(pIOleWindow->GetWindow(&hWnd) == S_OK);
	pIOleWindow->Release();
	return hWnd;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::UpdateStateData
//
// Update design state data structure (m_StateData).
//
void CMuteStrip::UpdateStateData()
{
	// fill the structure with data
	if( m_pMuteMgr->m_pTimeline )
	{
		VARIANT var;
		m_pMuteMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip*)this, STP_HEIGHT, &var);
		m_StateData.m_lVScroll = m_lVScroll;
		m_StateData.m_lHeight = V_I4(&var);
		m_StateData.m_svView = m_svView;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::ApplyStateData
//
// Applies design state data previously loaded from a design time stream
// (by SaveStateData)
//
void CMuteStrip::ApplyStateData()
{
	// get data from the structure.
	if (!m_fLoadedStateData) return;
	VARIANT var;
	var.vt = VT_I4;
	V_I4(&var) = m_StateData.m_lHeight;
	m_pMuteMgr->m_pTimeline->StripSetTimelineProperty((IDMUSProdStrip*)this, STP_HEIGHT, var);
	m_svView = m_StateData.m_svView;
	V_I4(&var) = m_svView;
	m_pMuteMgr->m_pTimeline->StripSetTimelineProperty((IDMUSProdStrip*)this, STP_STRIPVIEW, var);
	SetVScroll(m_StateData.m_lVScroll);


}



/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::IsBeyondMaxTime
//
// Returns true iff the given measurebeat is beyond the maximum time in the
// timeline.
//
bool CMuteStrip::IsBeyondMaxTime(long lMeasure, long lBeat) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	MUSIC_TIME mtTime = m_pMuteMgr->GetMusicTime(lMeasure, lBeat);
	VARIANT var;
	VERIFY(SUCCEEDED(m_pMuteMgr->m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var)));
	MUSIC_TIME mtMax = V_I4(&var);
	return (mtTime >= mtMax);	
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::SnapTimes
//
// Returns the result of the given start and end times, fully
// including beats that they touch.
// The resulting mtBeginResult == mtEndResult if mtStart == mtEnd
//
void CMuteStrip::SnapTimes(MUSIC_TIME mtBegin, MUSIC_TIME mtEnd, 
						   MUSIC_TIME &mtBeginResult, MUSIC_TIME &mtEndResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pMuteMgr);

	mtBeginResult = mtBegin;
	mtEndResult = mtEnd;
	
	// swap if necessary
	if (mtBeginResult > mtEndResult) {
		MUSIC_TIME mtTemp = mtEndResult;
		mtEndResult = mtBeginResult;
		mtBeginResult = mtTemp;
	}

	// calc real begin and end times (fully include beats)
	long lMeasure, lBeat;
	m_pMuteMgr->GetMeasureBeat(mtBeginResult, lMeasure, lBeat);
	mtBeginResult = m_pMuteMgr->GetMusicTime(lMeasure, lBeat);
	if (mtBegin == mtEnd) {
		mtEndResult = mtBeginResult;
	}
	else {
		m_pMuteMgr->GetMeasureBeat(mtEndResult-1, lMeasure, lBeat);
		lBeat++;
		m_pMuteMgr->NormalizeMeasureBeat(lMeasure, lBeat);
		mtEndResult = m_pMuteMgr->GetMusicTime(lMeasure, lBeat);
		ASSERT(mtBeginResult != mtEndResult);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::GetSelectTimes
//
// Returns the begin and end selection times, fully
// including beats that are touched by the timeline selection area.
// The resulting mtBeginTime == mtEndTime if there is no selection.
//
void CMuteStrip::GetSelectTimes(MUSIC_TIME &mtBeginTime, MUSIC_TIME &mtEndTime) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(m_pMuteMgr);

	if (!m_bGutterSelected) {
		mtBeginTime = -1;
		mtEndTime = -1;
	}
	else {
		SnapTimes(m_mtBeginSelect, m_mtEndSelect, mtBeginTime, mtEndTime);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnLButtonDown
//
HRESULT CMuteStrip::OnLButtonDown(WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_svView != SV_NORMAL) {
		// we're minimized.
		return S_OK;
	}
	int nIdx = YPosToChannelIdx(lYPos);
	if (nIdx >= m_arrChannels.GetSize()) {
		// we are outside a pchannel
		return S_OK;
	}

	// set mouse capture
	VARIANT var;
	var.vt = VT_BOOL;
	V_BOOL(&var) = TRUE;
	m_pMuteMgr->m_pTimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);
	
	DWORD dwChannel = m_arrChannels[nIdx];

	long lMeasure, lBeat;
	m_pMuteMgr->m_pTimeline->PositionToMeasureBeat(m_pMuteMgr->m_dwGroupBits, 0, 
		lXPos, &lMeasure, &lBeat);
	
	if (IsBeyondMaxTime(lMeasure, lBeat)) {
		return S_OK;
	}

	if (m_pMuteMgr->IsPositionMuted(lMeasure, lBeat, dwChannel)) {
		m_EditMode = emToggleOff;
		m_pMuteMgr->TurnOffMute(lMeasure, lBeat, dwChannel);
	}
	else {
		m_EditMode = emToggleOn;
		m_pMuteMgr->TurnOnMute(lMeasure, lBeat, dwChannel);
	}
	
	// redraw the position
	InvalidatePosition(nIdx, lMeasure, lBeat);
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnMouseMove
//
HRESULT CMuteStrip::OnMouseMove(WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_EditMode == emNone) {
		return S_OK;
	}

	// see if we need to start the timer for horizontal scrolling
	if( m_fLButtonDown )
	{
		// Deal with timer used for scrolling
		VARIANT var;

		// Get rectangle defining strip position
		var.vt = VT_BYREF;
		RECT rectStrip;
		V_BYREF(&var) = &rectStrip;
		if( SUCCEEDED ( m_pMuteMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIP_RECT, &var) ) )
		{
			// Get cursor position
			POINT pt;
			::GetCursorPos( &pt );

			// Convert cursor position to strip coordinates
			if( SUCCEEDED ( m_pMuteMgr->m_pTimeline->ScreenToStripPosition( (IDMUSProdStrip *)this, &pt ) ) 
			&&  SUCCEEDED ( m_pMuteMgr->m_pTimeline->StripToWindowPos( (IDMUSProdStrip *)this, &pt ) ) )
			{
				if( pt.x < rectStrip.left 
				||  pt.x > rectStrip.right 
				||  pt.y < rectStrip.top 
				||  pt.y > rectStrip.bottom )
				{
					// May need to scroll, cursor is NOT in the visible portion of the strip
					if( m_nScrollTimerID == 0 )
					{
						StartScrollTimer( FIRST_SCROLL_ID );
					}
				}
				else
				{
					// No need to scroll, cursor IS in the visible portion of the strip
					if( m_nScrollTimerID )
					{
						StopScrollTimer();
					}
				}
			}
		}
	}

	// figure out maximum Y (used in mouse move).
	VARIANT var;
	m_pMuteMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip*)this, STP_HEIGHT, &var);
	long lMinY = m_lVScroll * c_nChannelHeight;
	long lMaxY = V_I4(&var) + lMinY - 2;

	// don't do anything if the mouse moves to a beat that isnt visible.
	if (lYPos < lMinY || lYPos > lMaxY) {
		return S_OK;
	}

	int nIdx = YPosToChannelIdx(lYPos);
	if (nIdx >= m_arrChannels.GetSize() || nIdx < 0) {
		// we are outside a pchannel
		return S_OK;
	}
	DWORD dwChannel = m_arrChannels[nIdx];

	long lMeasure, lBeat;
	m_pMuteMgr->m_pTimeline->PositionToMeasureBeat(m_pMuteMgr->m_dwGroupBits, 0, 
		lXPos, &lMeasure, &lBeat);
	
	if( lXPos < 0
	||  IsBeyondMaxTime(lMeasure, lBeat) )
	{
		return S_OK;
	}

	ASSERT( lMeasure >= 0 );
	ASSERT( lBeat >= 0 );

	if (m_EditMode == emToggleOff) {
		m_pMuteMgr->TurnOffMute(lMeasure, lBeat, dwChannel);
	}
	else if (m_EditMode == emToggleOn) {
		m_pMuteMgr->TurnOnMute(lMeasure, lBeat, dwChannel);
	}

	if (m_EditMode != emNone) {

		// redraw the position
		InvalidatePosition(nIdx, lMeasure, lBeat);
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnLButtonUp
//
HRESULT CMuteStrip::OnLButtonUp(WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_EditMode != emNone) {
		// release mouse capture
		VARIANT var;
		var.vt = VT_BOOL;
		V_BOOL(&var) = FALSE;
		m_pMuteMgr->m_pTimeline->SetTimelineProperty(TP_STRIPMOUSECAPTURE, var);

		m_EditMode = emNone;
		m_nLastEdit = IDS_UNDO_MUTE;
		m_pMuteMgr->OnDataChanged();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnRButtonUp
//
HRESULT CMuteStrip::OnRButtonUp(WPARAM wParam, LPARAM lParam, LONG lXPos, LONG lYPos)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// remember where the user clicked
	m_pMuteMgr->m_pTimeline->PositionToMeasureBeat(m_pMuteMgr->m_dwGroupBits, 0,
		lXPos, &m_lClickedMeasure, &m_lClickedBeat);
	m_nClickedIdx = YPosToChannelIdx(lYPos);

	CMenu menu;
	menu.LoadMenu(IDM_MUTESTRIP_POPUP);
	CMenu* pPopupMenu = menu.GetSubMenu(0);
	ASSERT(pPopupMenu != NULL);

	if (m_nClickedIdx >= 0 && m_nClickedIdx < m_arrChannels.GetSize() && m_svView == SV_NORMAL) {
		// prepare the menu items
		CString strFormat;
		CString str;
		
		DWORD dwChan = m_arrChannels[m_nClickedIdx]+1;

		// cut, copy, paste, select all, delete
		pPopupMenu->EnableMenuItem( ID_EDIT_CUT, ( CanCut() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
		pPopupMenu->EnableMenuItem( ID_EDIT_COPY, ( CanCopy() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
		pPopupMenu->EnableMenuItem( 4, ( CanPaste( NULL ) == S_OK ) ? MF_ENABLED | MF_BYPOSITION :
					MF_GRAYED | MF_BYPOSITION );
		pPopupMenu->EnableMenuItem( ID_EDIT_SELECT_ALL, ( CanSelectAll() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );
		pPopupMenu->EnableMenuItem( ID_EDIT_DELETE, ( CanDelete() == S_OK ) ? MF_ENABLED :
					MF_GRAYED );

		// mute pchan item
		pPopupMenu->GetMenuString(CM_MUTEALL, strFormat, MF_BYCOMMAND);
		str.Format(strFormat, dwChan);
		pPopupMenu->ModifyMenu(CM_MUTEALL, MF_BYCOMMAND | MF_STRING, CM_MUTEALL, str);

		// unmute pchan item
		pPopupMenu->GetMenuString(CM_UNMUTEALL, strFormat, MF_BYCOMMAND);
		str.Format(strFormat, dwChan);
		pPopupMenu->ModifyMenu(CM_UNMUTEALL, MF_BYCOMMAND | MF_STRING, CM_UNMUTEALL, str);

		// clear pchan item
		pPopupMenu->GetMenuString(CM_CLEARALL, strFormat, MF_BYCOMMAND);
		str.Format(strFormat, dwChan);
		pPopupMenu->ModifyMenu(CM_CLEARALL, MF_BYCOMMAND | MF_STRING, CM_CLEARALL, str);

		CMuteItem* pItem = m_pMuteMgr->GetItemAtPosition(m_lClickedMeasure, m_lClickedBeat, m_arrChannels[m_nClickedIdx]);
		
		// if no remap item at clicked measurebeat, disable clear remap menu item
		if (pItem == NULL || !pItem->IsRemap()) {
			pPopupMenu->EnableMenuItem(CM_UNREMAP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		
		// if a mute exists, disable remap menu item.
		if (pItem != NULL && pItem->IsMute()) {
			pPopupMenu->EnableMenuItem(CM_REMAP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}

	}
	else {
		// load menu strings that don't have channel number (%d's)
		CString str;
		
		// mute pchan item
		str.LoadString(IDS_MUTEPCHANNEL);
		pPopupMenu->ModifyMenu(CM_MUTEALL, MF_BYCOMMAND | MF_STRING, CM_MUTEALL, str);

		// unmute pchan item
		str.LoadString(IDS_UNMUTEPCHANNEL);
		pPopupMenu->ModifyMenu(CM_UNMUTEALL, MF_BYCOMMAND | MF_STRING, CM_UNMUTEALL, str);

		// clear pchan item
		str.LoadString(IDS_CLEARPCHANNEL);
		pPopupMenu->ModifyMenu(CM_CLEARALL, MF_BYCOMMAND | MF_STRING, CM_CLEARALL, str);

		// disable some items since no pchannel is clicked.
		pPopupMenu->EnableMenuItem(CM_REMAP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pPopupMenu->EnableMenuItem(CM_UNREMAP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pPopupMenu->EnableMenuItem(CM_MUTEALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pPopupMenu->EnableMenuItem(CM_UNMUTEALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pPopupMenu->EnableMenuItem(CM_CLEARALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}

	// bring up the menu
	POINT pt;
	if (GetCursorPos(&pt )) {
		m_pMuteMgr->m_pTimeline->TrackPopupMenu(pPopupMenu->GetSafeHmenu(),
			pt.x, pt.y, (IDMUSProdStrip*)this, NULL);
	}
	
	menu.DestroyMenu();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::DoAddRemoveDlg
//
//  Bring up the add/remove channels dialog and add/remove the channels if the
//  user clicks OK.
//
void CMuteStrip::DoAddRemoveDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Initialize the add PChannel dialog
	CDlgAddPChannel dlgAddPChannel;

	// Set the PChannelName pointer
	IDMUSProdProject* pIProject;
	if( SUCCEEDED ( m_pMuteMgr->m_pDMProdFramework->FindProject( m_pMuteMgr->m_pDMProdSegmentNode, &pIProject ) ) )
	{
		pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&dlgAddPChannel.m_pIPChannelName );
		RELEASE( pIProject );
	}

	// Set up the array of PChannels we're editing
	dlgAddPChannel.m_adwPChannels.RemoveAll();
	for( int idx = 0 ;  idx < m_arrChannels.GetSize() ;  idx++ )
	{
		dlgAddPChannel.m_adwPChannels.Add( m_arrChannels[idx] );
	}

	if( dlgAddPChannel.DoModal() == IDOK )
	{
		// figure out which channels were removed, if any
		CArray<DWORD, DWORD> arrRemoved;
		int idx1 = 0;
		int idx2 = 0;
		while( idx1 < m_arrChannels.GetSize() )
		{
			DWORD ch1, ch2;
			ch1 = m_arrChannels[idx1];
			if( idx2 == dlgAddPChannel.m_adwPChannels.GetSize()
			||  ch1 < (ch2 = dlgAddPChannel.m_adwPChannels[idx2]) )
			{
				// ch1 was removed
				arrRemoved.Add(ch1);
				idx1++;
			}
			else if (ch1 > ch2)
			{
				// ch2 was added
				idx2++;
			}
			else if (ch1 == ch2)
			{
				// ch1 is still there
				idx1++;
				idx2++;
			}
		}

		// remove data on removed channels
		for (int i = 0; i < arrRemoved.GetSize(); i++) 
			m_pMuteMgr->RemoveChannelData(arrRemoved[i]);
		
		// set the array of channels to what the user selected
		// and repaint the strip.
		m_arrChannels.RemoveAll();
		for( int idx = 0 ;  idx < dlgAddPChannel.m_adwPChannels.GetSize() ;  idx++ )
		{
			m_arrChannels.Add( dlgAddPChannel.m_adwPChannels[idx] );
		}
		m_arrChannels.FreeExtra();
		m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, true);
		
		if( arrRemoved.GetSize() > 0 )
		{
			// reduce the strip height if it is too large
			VARIANT var;
			m_pMuteMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip*)this, STP_HEIGHT, &var);
			long lHeight = V_I4(&var);
			if( m_arrChannels.GetSize() * c_nChannelHeight < lHeight )
			{
				var.vt = VT_I4;
				V_I4(&var) = max(c_nMinHeight, m_arrChannels.GetSize() * c_nChannelHeight);
				m_pMuteMgr->m_pTimeline->StripSetTimelineProperty((IDMUSProdStrip*)this, STP_HEIGHT, var);
				UpdateVScroll();
				SetVScroll(m_lVScroll);
			}
		}

		// update dmusic data if channels were removed
		m_nLastEdit = IDS_UNDO_ADD_REMOVE_PCHANNEL;
		m_pMuteMgr->OnDataChanged();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::DoRemapDlg
//
// Bring up the remap pchannel dialog and do the remapping if the user
// clicks OK.
//
void CMuteStrip::DoRemapDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_nClickedIdx < 0 || m_nClickedIdx >= m_arrChannels.GetSize()) {
		// we are outside a pchannel
		return;
	}

	// Get the IDMUSProdPChannelName interface pointer
	IDMUSProdPChannelName* pIPChannelName = NULL;
	if( m_pMuteMgr
	&&  m_pMuteMgr->m_pDMProdFramework 
	&&  m_pMuteMgr->m_pDMProdSegmentNode )
	{
		IDMUSProdProject* pIProject;
		if( SUCCEEDED ( m_pMuteMgr->m_pDMProdFramework->FindProject(m_pMuteMgr->m_pDMProdSegmentNode, &pIProject) ) )
		{
			if( FAILED ( pIProject->QueryInterface(IID_IDMUSProdPChannelName, (void**)&pIPChannelName) ) )
			{
				pIPChannelName = NULL;
			}
			pIProject->Release();
		}
	}

	// init the dialog
	CRemapDlg dlg;
	dlg.m_pIPChannelName = pIPChannelName;
	dlg.m_dwPChannel = m_arrChannels[m_nClickedIdx];
	dlg.m_lMeasure = m_lClickedMeasure;
	dlg.m_lBeat = m_lClickedBeat;
	if (dlg.DoModal() == IDOK) {
		// do the remap
		if( dlg.m_dwPChannel == dlg.m_dwPChannelMap )
		{
			m_pMuteMgr->ClearRemap(m_lClickedMeasure, m_lClickedBeat, dlg.m_dwPChannel);
		}
		else
		{
			m_pMuteMgr->RemapPChannel(m_lClickedMeasure, m_lClickedBeat, dlg.m_dwPChannel, dlg.m_dwPChannelMap);
		}

		// repaint the strip
		m_pMuteMgr->m_pTimeline->StripInvalidateRect((IDMUSProdStrip*)this, NULL, true);
	
		// sync with dmusic
		if( dlg.m_dwPChannel == dlg.m_dwPChannelMap )
		{
			m_nLastEdit = IDS_UNDO_CLEAR_REMAP;
		}
		else
		{
			m_nLastEdit = IDS_UNDO_REMAP_PCHANNEL;
		}
		m_pMuteMgr->OnDataChanged();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::SaveStateData
//
// Save design time state data for this mute strip.
//
HRESULT CMuteStrip::SaveStateData(IDMUSProdRIFFStream* pIRiffStream)
{
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	int i;
	DWORD dwNumChannels, dwChan;

	UpdateStateData();

    // prepare stream
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	if (pIStream == NULL)
	{
		return E_INVALIDARG;
	}
	
	// create the UI chunk.
	ck.ckid = DMUS_FOURCC_MUTE_UI_CHUNK;
	if (pIRiffStream->CreateChunk(&ck, 0) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// write the data
	hr = pIStream->Write(&m_StateData, sizeof(m_StateData), &dwBytesWritten);
	if (FAILED(hr)
	||  dwBytesWritten != sizeof(m_StateData)) 
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// write the channel data
	dwNumChannels = m_arrChannels.GetSize();
	hr = pIStream->Write(&dwNumChannels, sizeof(dwNumChannels), &dwBytesWritten);
	if (FAILED(hr)
	||  dwBytesWritten != sizeof(dwNumChannels)) 
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	for (i = 0; i < (int)dwNumChannels; i++) {
		dwChan = m_arrChannels[i];
		hr = pIStream->Write(&dwChan, sizeof(dwChan), &dwBytesWritten);
		if (FAILED(hr)
		||  dwBytesWritten != sizeof(dwChan)) 
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// finish the chunk
	if (pIRiffStream->Ascend(&ck, 0) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	pIStream->Release();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::MergeStateData
//
// Reads design time data from the given stream and adds the channels that
// aren't already being shown by the strip.
//
HRESULT CMuteStrip::MergeStateData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pck)
{
	HRESULT hr = S_OK;
	MMCKINFO ck = *pck;
	DWORD dwSize;
	DWORD dwByteCount;
	int i;
	DWORD dwNumChannels, dwChan;
	ioMuteStripDesign stateData; // for reading purposes; the data is actually ignored

    // prepare stream
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	if (pIStream == NULL)
	{
		return E_INVALIDARG;
	}
	
	dwSize = min(ck.cksize, sizeof(stateData));
	hr = pIStream->Read(&stateData, dwSize, &dwByteCount);
	if (FAILED(hr)
	||  dwByteCount != dwSize )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	// read the channel data
	hr = pIStream->Read(&dwNumChannels, sizeof(dwNumChannels), &dwByteCount);
	if (FAILED(hr)
	||  dwByteCount != sizeof(DWORD))
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
	// add the channels that aren't already being displayed.
	{
		int idx = 0;
		for (i = 0; i < (int)dwNumChannels; i++) {
			hr = pIStream->Read(&dwChan, sizeof(dwChan), &dwByteCount);
			if (FAILED(hr)
			||  dwByteCount != sizeof(dwChan)) 
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
			while (idx < m_arrChannels.GetSize() && m_arrChannels[idx] < dwChan) 
				idx++;
			if (idx == m_arrChannels.GetSize() || m_arrChannels[idx] != dwChan) {
				m_arrChannels.InsertAt(idx, dwChan);
			}
		}
	}
	
	if (pIRiffStream->Ascend(&ck, 0) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
ON_ERROR:
	pIStream->Release();
    return hr;
}



/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::SyncStateData
//
// Called from CMuteMgr::Load when design time state data for this mute strip
// was not loaded (runtime file was loaded).
// Must make sure m_arrChannels matches actual mute data.
//
void CMuteStrip::SyncStateData(void)
{
	// Set PChannel array
	if( m_pMuteMgr->m_lstMutes.IsEmpty() == FALSE )
	{
		DWORD dwLastPChannel = -1;
		DWORD dwPChannel;
		int idx;

		// Initialize PChannel array
		m_arrChannels.RemoveAll();

		// Fill PChannel array
		POSITION pos = m_pMuteMgr->m_lstMutes.GetHeadPosition();
		while( pos )
		{
			CMuteItem* pItem = m_pMuteMgr->m_lstMutes.GetNext( pos );

			dwPChannel = pItem->GetPChannel();
			if( dwPChannel != dwLastPChannel )
			{
				idx = 0;
				while( idx < m_arrChannels.GetSize()  &&  m_arrChannels[idx] < dwPChannel )
				{
					idx++;
				}
				if( idx == m_arrChannels.GetSize()  ||  m_arrChannels[idx] != dwPChannel )
				{
					m_arrChannels.InsertAt( idx, dwPChannel );
				}

				dwLastPChannel = dwPChannel;
			}
		}
	}

	// Set StateData strip height
	m_StateData.m_lHeight = c_nDefaultHeight;
	if( (m_arrChannels.GetSize() * c_nChannelHeight) < c_nDefaultHeight )
	{
		m_StateData.m_lHeight = max( c_nMinHeight, (m_arrChannels.GetSize() * c_nChannelHeight) );
	}

	// Set StateData scroll position
	m_lVScroll = 0;
	m_StateData.m_lVScroll = m_lVScroll;

	// Set StateData strip view
	m_StateData.m_svView = m_svView;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::LoadStateData
//
// Load design time state data for this mute strip.  It isn't actually
// applied (except for channels showing), until ApplyStateData is called.
//
HRESULT CMuteStrip::LoadStateData(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pck)
{
	HRESULT hr = S_OK;
	MMCKINFO ck = *pck;
	DWORD dwSize;
	DWORD dwByteCount;
	int i;
	DWORD dwNumChannels, dwChan;

    // prepare stream
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	if (pIStream == NULL)
	{
		return E_INVALIDARG;
	}
	
	dwSize = min(ck.cksize, sizeof(m_StateData));
	hr = pIStream->Read(&m_StateData, dwSize, &dwByteCount);
	if (FAILED(hr)
	||  dwByteCount != dwSize )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	// read the channel data
	hr = pIStream->Read(&dwNumChannels, sizeof(dwNumChannels), &dwByteCount);
	if (FAILED(hr)
	||  dwByteCount != sizeof(DWORD))
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	m_arrChannels.RemoveAll();
	for (i = 0; i < (int)dwNumChannels; i++) {
		hr = pIStream->Read(&dwChan, sizeof(dwChan), &dwByteCount);
		if (FAILED(hr)
		||  dwByteCount != sizeof(dwChan)) 
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
		m_arrChannels.Add(dwChan);
	}
	
	if (pIRiffStream->Ascend(&ck, 0) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
ON_ERROR:
	pIStream->Release();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::UpdateName

void CMuteStrip::UpdateName()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strText, strTmp;
	BOOL fFoundGroup = FALSE;
	BOOL fLastSet = FALSE;
	int nStartGroup = -1;

	for( int i = 0 ;  i < 32 ;  i++ )
	{
		if( m_pMuteMgr->m_dwGroupBits & (1 << i) )
		{
			if( !fLastSet )
			{
				fLastSet = TRUE;
				nStartGroup = i;
			}
		}
		else
		{
			if( fLastSet )
			{
				fLastSet = FALSE;
				if( nStartGroup == i - 1 )
				{
					if( fFoundGroup )
					{
						strTmp.Format(", %d", i);
					}
					else
					{
						strTmp.Format("%d", i);
						fFoundGroup = TRUE;
					}
				}
				else
				{
					if( fFoundGroup )
					{
						strTmp.Format(", %d-%d", nStartGroup + 1, i);
					}
					else
					{
						strTmp.Format("%d-%d", nStartGroup + 1, i);
						fFoundGroup = TRUE;
					}
				}
				strText += strTmp;
			}
		}
	}

	if( fLastSet )
	{
		fLastSet = FALSE;
		if( nStartGroup == i - 1 )
		{
			if( fFoundGroup )
			{
				strTmp.Format(", %d", i);
			}
			else
			{
				strTmp.Format("%d", i);
				fFoundGroup = TRUE;
			}
		}
		else
		{
			if( fFoundGroup )
			{
				strTmp.Format(", %d-%d", nStartGroup + 1, i);
			}
			else
			{
				strTmp.Format("%d-%d", nStartGroup + 1, i);
				fFoundGroup = TRUE;
			}
		}
		strText += strTmp;
	}

	strTmp.LoadString( IDS_STRIP_NAME );

	m_strName = strText + CString(": ") + strTmp;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::StartScrollTimer

void CMuteStrip::StartScrollTimer( UINT nScrollTimerID )
{
	if( m_nScrollTimerID )
	{
		StopScrollTimer();
	}

	CWnd *pWnd = GetTimelineCWnd();
	if( pWnd )
	{
		UINT nInterval;

		switch( nScrollTimerID )
		{
			case FIRST_SCROLL_ID:
				nInterval = 350;
				break;
			case SECOND_SCROLL_ID:
				nInterval = 80;
				break;
			default:
				nInterval = 80;
				break;
		}

		m_dwTimerCount = 0;

		if( ::SetTimer( pWnd->GetSafeHwnd(), nScrollTimerID, nInterval, NULL ) )
		{
			m_nScrollTimerID = nScrollTimerID;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::StopScrollTimer

void CMuteStrip::StopScrollTimer( void )
{
	if( m_nScrollTimerID )
	{
		CWnd *pWnd = GetTimelineCWnd();
		if( pWnd )
		{
			::KillTimer( pWnd->GetSafeHwnd(), m_nScrollTimerID );
		}

		m_nScrollTimerID = 0;
		m_dwTimerCount = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::GetTimelineCWnd

CWnd* CMuteStrip::GetTimelineCWnd( void )
{
	CWnd* pWnd = NULL;

	// Get the DC of our Strip
	if( m_pMuteMgr->m_pTimeline )
	{
		VARIANT vt;
		vt.vt = VT_I4;

		if( SUCCEEDED ( m_pMuteMgr->m_pTimeline->StripGetTimelineProperty( this, STP_GET_HDC, &vt ) ) )
		{
			CDC cDC;
			if( cDC.Attach( (HDC)(vt.lVal) ) != 0 )
			{
				pWnd = cDC.GetWindow();
				cDC.Detach();
			}
			if( pWnd )
			{
				::ReleaseDC( pWnd->GetSafeHwnd(), (HDC)(vt.lVal) );
			}
			else
			{
				::ReleaseDC( NULL, (HDC)(vt.lVal) );
			}
		}
	}

	return pWnd;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteStrip::OnTimer

void CMuteStrip::OnTimer( void )
{
	if( m_nScrollTimerID == FIRST_SCROLL_ID )
	{
		StartScrollTimer( SECOND_SCROLL_ID );
	}

	// Get cursor position
	POINT pt;
	::GetCursorPos( &pt );

	// Convert to strip coordinates
	if( FAILED ( m_pMuteMgr->m_pTimeline->ScreenToStripPosition( (IDMUSProdStrip *)this, &pt ) ) )
	{
		return;
	}

	// Get current horizontal scroll position
	VARIANT var;
	long lHScroll = 0;
	if( SUCCEEDED( m_pMuteMgr->m_pTimeline->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var ) ) )
	{
		lHScroll = V_I4(&var);
	}

	// Scroll left?
	if( pt.x < lHScroll 
	&&  lHScroll > 0 )
	{
		// Scroll left
		var.vt = VT_I4;
		V_I4(&var) = max( lHScroll - SCROLL_HORIZ_AMOUNT, 0 );
		m_pMuteMgr->m_pTimeline->SetTimelineProperty( TP_HORIZONTAL_SCROLL, var );

		OnMouseMove( NULL, NULL, max(lHScroll - SCROLL_HORIZ_AMOUNT, 0), pt.y );
		return;
	}

	// Scroll right?
	var.vt = VT_BYREF;
	RECT rectStrip;
	V_BYREF(&var) = &rectStrip;
	if( SUCCEEDED ( m_pMuteMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)this, STP_STRIP_RECT, &var) ) )
	{
		// Compute the right side of the display
		long lMaxScreenPos = lHScroll + rectStrip.right - rectStrip.left;

		// Compute the maximum scroll position
		m_pMuteMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
		long lTimelineClockLength = V_I4(&var);
		long lMaxXPos;
		if( SUCCEEDED ( m_pMuteMgr->m_pTimeline->ClocksToPosition( lTimelineClockLength, &lMaxXPos ) ) )
		{
			long lMaxHScroll = lMaxXPos - (rectStrip.right - rectStrip.left);

			if( pt.x > lMaxScreenPos 
			&&  lHScroll < lMaxHScroll )
			{
				// Scroll right
				var.vt = VT_I4;
				V_I4(&var) = min( lHScroll + SCROLL_HORIZ_AMOUNT, lMaxHScroll);
				m_pMuteMgr->m_pTimeline->SetTimelineProperty( TP_HORIZONTAL_SCROLL, var );

				OnMouseMove( NULL, NULL, min(lMaxScreenPos + SCROLL_HORIZ_AMOUNT, lMaxXPos), pt.y );
				return;
			}
		}
	}

	// Vertical scrolls must go slower
	if( ++m_dwTimerCount < 3 )
	{
		return;
	}
	m_dwTimerCount = 0;

	// Get current vertical scroll position
	long lVScroll = 0;
	if( SUCCEEDED( m_pMuteMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip*)this, STP_VERTICAL_SCROLL, &var ) ) )
	{
		lVScroll = V_I4(&var);
	}

	// Scroll up?
	if( pt.y < lVScroll 
	&&  lVScroll > 0 )
	{
		// Scroll up
		long lOrigVScroll = m_lVScroll;
		SetVScroll( m_lVScroll - 1 );

		if( m_lVScroll != lOrigVScroll )
		{
			OnMouseMove( NULL, NULL, pt.x, (m_lVScroll * c_nChannelHeight) );
		}
		return;
	}
	
	// Scroll down?
	m_pMuteMgr->m_pTimeline->StripGetTimelineProperty((IDMUSProdStrip*)this, STP_HEIGHT, &var);
	long lMinY = m_lVScroll * c_nChannelHeight;
	long lMaxY = V_I4(&var) + lMinY - 2;
	if( pt.y > lMaxY ) 
	{
		// Scroll up
		long lOrigVScroll = m_lVScroll;
		SetVScroll( m_lVScroll + 1 );

		if( m_lVScroll != lOrigVScroll )
		{
			lMinY = m_lVScroll * c_nChannelHeight;
			lMaxY = V_I4(&var) + lMinY - 2;
			OnMouseMove( NULL, NULL, pt.x, lMaxY );
		}
		return;
	}
}


