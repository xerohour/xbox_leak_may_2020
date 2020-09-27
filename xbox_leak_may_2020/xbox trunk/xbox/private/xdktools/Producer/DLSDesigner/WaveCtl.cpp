// =========================================================================
//  Implementation of the wave editor.
//
//  TODO: Change snaptpzero based on spec.
// =========================================================================
// WaveCtl.cpp : Implementation of the CWaveCtrl ActiveX Control class.

#include "stdafx.h"
#include "DLSDesignerDLL.h"
#include "WaveNode.h"
#include "Wave.h"
#include "WaveDataManager.h"
#include "WaveRefNode.h"
#include "MonoWave.h"
#include "WaveCtl.h"
#include "Collection.h"
#include "Instrument.h"
#include "Region.h"
#include "Articulation.h"
#include "Conductor.h"
#include "aboutdlg.h"
#include "WavePropPgMgr.h"
#include "waveproppg.h"
#include "RiffStrm.h"
#include "DLSDefsPlus.h"
#include <math.h>
#include "InsertSilenceDlg.h"
#include "ResampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define	ID_WAVE_PLAYBACK_TIMER		100
#define ID_WAVE_AUTOSCROLL_TIMER	ID_WAVE_PLAYBACK_TIMER + 1


IMPLEMENT_DYNCREATE(CWaveCtrl, COleControl)
/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CWaveCtrl, COleControl)
	//{{AFX_MSG_MAP(CWaveCtrl)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_WM_HSCROLL()
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_SNAP_TO_ZERO, OnSnapToZero)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(IDM_SET_LOOP_FROM_SELECTION, OnSetLoopFromSelection)
	ON_COMMAND(IDM_PROPERTIES, OnProperties)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(IDM_SET_LOOP_FROM_SELECTION, OnUpdateSetLoopFromSelection)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(IDM_COPY, OnCopy)
	ON_UPDATE_COMMAND_UI(IDM_COPY, OnUpdateCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(IDM_PASTE, OnPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(IDM_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(IDM_CUT, OnCut)
	ON_UPDATE_COMMAND_UI(IDM_CUT, OnUpdateCut)
	ON_WM_KILLFOCUS()
	ON_WM_TIMER()
	ON_COMMAND(IDM_HELP_FINDER, OnHelpFinder)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDM_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(IDM_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(IDM_COPY_LOOP, OnCopyLoop)
	ON_COMMAND(IDM_WAVEEDITORREGION_FADE_IN, OnRegionFadeIn)
	ON_COMMAND(IDM_WAVEEDITORREGION_FADE_OUT, OnRegionFadeOut)
	ON_UPDATE_COMMAND_UI(IDM_WAVEEDITORREGION_FADE_IN, OnUpdateRegionFadeIn)
	ON_UPDATE_COMMAND_UI(IDM_WAVEEDITORREGION_FADE_OUT, OnUpdateRegionFadeOut)
	ON_UPDATE_COMMAND_UI(IDM_COPY_LOOP, OnUpdateCopyLoop)
	ON_COMMAND(ID_EDIT_SELECTALL, OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTALL, OnUpdateSelectAll)
	ON_COMMAND(IDM_FIND_BEST_LOOP, OnFindBestLoop)
	ON_UPDATE_COMMAND_UI(IDM_FIND_BEST_LOOP, OnUpdateFindBestLoop)
	ON_UPDATE_COMMAND_UI(IDM_DECOMPRESS_START, OnUpdateDecompressStart)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_DECOMPRESS_START, OnDecompressStart)
	ON_COMMAND(IDM_INSERT_SILENCE, OnInsertSilence)
	ON_UPDATE_COMMAND_UI(IDM_INSERT_SILENCE, OnUpdateInsertSilence)
	ON_COMMAND(IDM_RESAMPLE, OnResample)
	ON_UPDATE_COMMAND_UI(IDM_RESAMPLE, OnUpdateResample)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CWaveCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CWaveCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CWaveCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CWaveCtrl, COleControl)
	//{{AFX_EVENT_MAP(CWaveCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl Interface map

BEGIN_INTERFACE_MAP(CWaveCtrl, COleControl)
    INTERFACE_PART(CWaveCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CWaveCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*
BEGIN_PROPPAGEIDS(CWaveCtrl, 1)
END_PROPPAGEIDS(CWaveCtrl)
*/

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CWaveCtrl, "DLS Designer.WaveCtrl.1",
	0xbc964e8e, 0x96f7, 0x11d0, 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CWaveCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DWave =
		{ 0xbc964e8c, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };
const IID BASED_CODE IID_DWaveEvents =
		{ 0xbc964e8d, 0x96f7, 0x11d0, { 0x89, 0xaa, 0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwWaveOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_SIMPLEFRAME;

IMPLEMENT_OLECTLTYPE(CWaveCtrl, IDS_WAVE, _dwWaveOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::CWaveCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CWaveCtrl

BOOL CWaveCtrl::CWaveCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_WAVE,
			IDB_WAVE,
			afxRegApartmentThreading,
			_dwWaveOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::CWaveCtrl - Constructor

CWaveCtrl::CWaveCtrl() : 
m_pWave(NULL),
m_pWaveNode(NULL),
m_pDummyInstrument(NULL),
m_hMenuInPlace(NULL),
m_hAcceleratorTable(NULL),
m_hKeyStatusBar(NULL),
m_hSIZEWECursor(NULL),
m_hSizeLoopStartCursor(NULL),
m_hSizeLoopEndCursor(NULL),
m_hPrevCursor(NULL),
m_bSizeCursorSet(false),
m_bInSelMode(false),
m_bDraggingLoop(false),
m_nDragDir(_NONE),
m_pRMenu(NULL),
m_dwCookie(0),
m_pClipboardDataObject(NULL),
m_dwMaxScrollPos(0),
m_nSamplesPerPixelBeforeZoom(0),
m_nSnapStart(0),
m_nSnapEnd(0),
m_nStartSel(0),
m_nEndSel(0)
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DWave, &IID_DWaveEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DWave;
	m_piidEvents = &IID_DWaveEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	EnableSimpleFrame();
    m_nxSampleOffset = 0;
    m_nZoomFactor = 0xFF;	// Default zoom is 1:8.
    m_nSamplesPerPixel = 8; // Default is 8 samples per pixel.

	for(int i = 0; i < 128; i++)
	{
		m_nMIDINoteOns[i] = 0;
	}

	m_nPlaybackPos = 0;
	m_uiTimer = 0;
}   


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::~CWaveCtrl - Destructor

CWaveCtrl::~CWaveCtrl()
{
    // We had done an extra addref on the wave node 
	if(m_pWave)
	{
		m_pWaveNode->Release();
	}

	if( m_pClipboardDataObject != NULL)
	{
		if( S_OK == OleIsCurrentClipboard( m_pClipboardDataObject ))
		{
			OleFlushClipboard();
		}
		m_pClipboardDataObject->Release();
	}

	// free cursors
	if (m_bSizeCursorSet)
		SetCursor(m_hPrevCursor);
	if (m_hSizeLoopStartCursor != NULL)
		DestroyCursor(m_hSizeLoopStartCursor);
	if (m_hSizeLoopEndCursor != NULL)
		DestroyCursor(m_hSizeLoopEndCursor);
}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CWaveCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CWaveCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CWaveCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CWaveCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::XEditor::QueryInterface

STDMETHODIMP CWaveCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CWaveCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::XEditor::AttachObjects implementation

HRESULT CWaveCtrl::XEditor::AttachObjects(IDMUSProdNode* pNode)
{
	METHOD_MANAGE_STATE( CWaveCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pNode != NULL );

	GUID guidNode;

	pNode->GetNodeId( &guidNode );
	if( ::IsEqualGUID( guidNode, GUID_WaveRefNode ) )
	{
		CWaveRefNode* pWaveRefNode = (CWaveRefNode *)pNode;
		ASSERT( pWaveRefNode->m_pWaveNode != NULL );
		pThis->m_pWaveNode = pWaveRefNode->m_pWaveNode;
	}
	else
	{
		pThis->m_pWaveNode = (CWaveNode*) pNode;
	}
	
	pThis->m_pWave = pThis->m_pWaveNode->GetWave();
	pThis->m_pWaveNode->AddRef();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CInstrumentCtrl::XEditor::OnViewProperties implementation

HRESULT CWaveCtrl::XEditor::OnViewProperties()
{
	METHOD_MANAGE_STATE( CWaveCtrl, Editor )
	ASSERT_VALID( pThis );
	if (pThis->m_pWaveNode)
    {
        return pThis->m_pWaveNode->OnViewProperties();
    }
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::XEditor::OnInitMenuFilePrint

HRESULT CWaveCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CWaveCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::XEditor::OnFilePrint

HRESULT CWaveCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CWaveCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CWaveCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CWaveCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::XEditor::OnFilePrintPreview

HRESULT CWaveCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CWaveCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::XEditor::OnF1Help implementation

HRESULT CWaveCtrl::XEditor::OnF1Help()
{
	METHOD_MANAGE_STATE( CWaveCtrl, Editor )
	ASSERT_VALID( pThis );
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/WaveEditor.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CWaveCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CWaveCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CWaveCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CWaveCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CWaveCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CWaveCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}

STDMETHODIMP CWaveCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CWaveCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CWaveCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CWaveCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CWaveCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CWaveCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );
	
	// Translate only if the wavectrl has the focus
	CWnd* pWnd = pThis->GetFocus();
	if(pWnd == pThis || pThis->IsChild(pWnd))
	{
		if( ::TranslateAccelerator(pThis->m_hWnd, pThis->m_hAcceleratorTable, lpmsg) )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}

STDMETHODIMP CWaveCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CWaveCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	CDLSComponent* pComponent = pThis->m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	
	if ( !fActivate )
	{
		if (pThis->m_bSizeCursorSet)
		{
			pThis->ReleaseCapture();
			pThis->m_bInSelMode = false;
        
			pThis->SetSizeCursor(false);    
			pThis->m_nDragDir = _NONE;
			pThis->InvalidateRect(NULL);
		}
	}
	
	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CWaveCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CWaveCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	CDLSComponent* pComponent = pThis->m_pWaveNode->GetComponent();
	ASSERT(pComponent);

	if ( fActivate )
	{
		pThis->m_pWaveNode->OnViewProperties();
		pThis->SetFocus();
	}
	else
	{
		if (pThis->m_bSizeCursorSet)
		{
			pThis->ReleaseCapture();
			pThis->m_bInSelMode = false;
        
			pThis->SetSizeCursor(false);    
			pThis->m_nDragDir = _NONE;
			pThis->InvalidateRect(NULL);
		}

	}
	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CWaveCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CWaveCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE)
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CWaveCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CWaveCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}

// ==========================================================================
// CWaveCtrl::OnDraw - Drawing function
// ==========================================================================
void CWaveCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{			
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(pdc);
	
	CBitmap	bmBlank;
	CRect rcClientArea, rcScrollBar;
	
	GetClientRect(&rcClientArea);
	if(m_HScrollBar)
	{
		m_HScrollBar.GetWindowRect(&rcScrollBar);
		rcClientArea.bottom -= (rcScrollBar.bottom - rcScrollBar.top);
	}

	if (rcClientArea.Width() <= 0 || rcClientArea.Height() <= 0) 
	{
		// Avoid division by zero; don't bother drawing anything
		return;
	}
	
	BOOL bStatus = bmBlank.CreateCompatibleBitmap(pdc, rcClientArea.right, rcClientArea.bottom);
    ASSERT(bStatus);

	// Create a memory dc so that we can draw by doing one blt to the screen.
	CDC memdc;
    bStatus = memdc.CreateCompatibleDC(pdc);
    ASSERT(bStatus);

    // Select an empty bitmap into the dc. 
	CBitmap* pOldbm = memdc.SelectObject(&bmBlank);

    // Draw the WHITE background. 
	memdc.FillSolidRect(rcBounds, RGB(255, 255, 255));

	CRect rcWave = rcClientArea;

	// Fill the area BEFORE the decompressed start of the wave (if compressed) with GRAY
	DWORD dwDecompressedStart = m_pWave->GetDwDecompressedStart(true);
	if (dwDecompressedStart > 0)
		{
		// gray background
		int xRectRight = SampleToPixel(dwDecompressedStart, false);
		if (xRectRight >= rcBounds.left)
			memdc.FillSolidRect(rcBounds.left, rcBounds.top, xRectRight, rcBounds.bottom, GetSysColor(COLOR_BTNFACE));
		}
	
	// Fill the area AFTER the end of the wave with GRAY
	DWORD dwLength= m_pWave->m_dwWaveLength - 1;
	DWORD dwRectLeft = 0;
	if((int)dwLength >= 1)
	{
		dwRectLeft = SampleToPixel(dwLength, false);
	}

	if((int)dwRectLeft < rcBounds.right)
	{
		CRect rectGray(dwRectLeft, rcBounds.top, rcBounds.right, rcBounds.bottom);
		memdc.FillSolidRect(rectGray, GetSysColor(COLOR_BTNFACE));
		rcWave.SubtractRect(&rcWave, &rectGray);
	}

    int nStart = SampleToPixel(m_nStartSel, true);
    int nEnd = SampleToPixel(m_nEndSel, true);

    // If we have a region selected, draw it.    
    if(nStart != nEnd) 
    {
		int nSelectionStart = nStart;
		int nSelectionEnd = nEnd;

		if(nStart > rcClientArea.right)
		{
			nSelectionStart = rcWave.right;
		}
		if(nEnd > rcClientArea.right)
		{
			nSelectionEnd = rcWave.right;
		}

        CRect rect(nSelectionStart, 0, nSelectionEnd, rcWave.bottom);
        memdc.FillSolidRect(&rect, PALETTERGB(255, 0, 0));
    }
    else
    {
		// Draw selection cursor
        if (nStart)
			memdc.FillSolidRect(nStart, 0, 1, rcWave.bottom, RGB(255, 0, 0));
    }

	// Ask the wave to draw
	m_pWaveNode->DrawWave(&memdc, rcWave);

    // Draw the loop start and end points.
	if (m_bDraggingLoop)
		{
		// draw loop point being dragged
		DrawLoopCursor(memdc, PS_DASH, m_ptDraggedLoop.x, m_nDragDir);

		// if loop length is locked, draw both markers (fix 48682)
		if (m_pWave->m_pWavePropPgMgr && m_pWave->m_pWavePropPgMgr->m_pWavePage && m_pWave->m_pWavePropPgMgr->m_pWavePage->IsLoopLengthLocked())
			{
			// compute delta for other loop point
			WLOOP waveLoop = m_pWave->GetWLOOP();
			int nLoopLength = waveLoop.ulLength;
			if (m_nDragDir == _END)
				nLoopLength = -nLoopLength;

			// draw it
			DrawLoopCursor(memdc, PS_DASH,
				SampleToPixel(PixelToSample(m_ptDraggedLoop.x, true)+nLoopLength, true),
				(m_nDragDir == _START) ? _END : _START);
			}
		}

	DrawLoopPoints(&memdc);

    // Blt the memdc to the screen
	bStatus = pdc->BitBlt(0, 0, rcClientArea.right, rcClientArea.bottom, &memdc, 0, 0, SRCCOPY);

    ASSERT(bStatus);
 	
	// Select the GDI Objects out of the memory DC and delete it
	memdc.SelectObject(pOldbm); 
    bStatus = memdc.DeleteDC(); 
    ASSERT(bStatus);

    UpdateStatusBar();
	
	CRect rectTemp = rcInvalid;
	UpdatePlaybackPos(0, &rectTemp);
}



/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::DoPropExchange - Persistence support

void CWaveCtrl::DoPropExchange(CPropExchange* pPX)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl::AboutBox - Display an "About" box to the user

void CWaveCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog dlgAbout(IDD_ABOUTBOX_WAVE);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl message handlers

void CWaveCtrl::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	COleControl::OnSize(nType, cx, cy);
    
	// Move the scrollbar
    RECT rc;
    GetClientRect(&rc);
    int nHeight = GetSystemMetrics(SM_CYHSCROLL);
    rc.bottom = cy;
    rc.right = cx - 2*nHeight;
    rc.top = cy-nHeight;
    rc.left = 0;
    m_HScrollBar.MoveWindow(&rc, true);
    
	// Calc the range of scrollbar.
    ResetScrollBar();

    // Move the buttons also. CButton derives from CWnd.
    rc.left = rc.right; //from previous
    rc.right = rc.left + nHeight;
    m_BtnZoomIn.MoveWindow(&rc, true);

    // Now the second button.
    rc.left = rc.right;
    rc.right = cx;
    m_BtnZoomOut.MoveWindow(&rc, true);

	// Keep zoom within limits
	SetZoomFactor(m_nZoomFactor);

	// CONSIDER: Do all these movewindows need to set repaint = true or only the
    // last one?
}

// =============================================================================
//  OnCreate
// =============================================================================
int CWaveCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);

    ASSERT(m_pWave);

    // Plug in the pointer to this wavectrl in the wave.
    m_pWaveNode->SetWaveEditor(this);

    // Set the Looped mode in the wave. default is true if loops set else it's 
    // whatever the person asked for in property page.
    m_pWave->m_bPlayLooped = (m_pWave->m_rWSMP.cSampleLoops != 0);

	// Load control's in-place menu
	m_hMenuInPlace = ::LoadMenu(theApp.m_hInstance, MAKEINTRESOURCE(IDM_DLS_DESIGNER));

    // load the right click menu

	m_pRMenu = new CMenu();
	if (m_pRMenu)
    {
        if ( m_pRMenu->LoadMenu(IDM_WAVE_EDITOR_REGION_RMENU))
        {   //loaded successfully
			if(pComponent->IsSnapToZero() == TRUE)
				m_pRMenu->CheckMenuItem(IDM_SNAP_TO_ZERO, MF_CHECKED);
			else
				m_pRMenu->CheckMenuItem(IDM_SNAP_TO_ZERO, MF_UNCHECKED);

        }
        else
        {
            delete m_pRMenu;
            m_pRMenu = NULL;
        }
    }
	
	// Create the horizontal scrollbar.
    RECT rc;
    int nHeight = GetSystemMetrics(SM_CYHSCROLL);
    rc.top = rc.left = 0;
    rc.right = lpCreateStruct->cx - 2*nHeight;
    rc.bottom = lpCreateStruct->cy;
	
	CStatic hiddenStatic;
	hiddenStatic.Create("", WS_CHILD, rc, this);

    m_HScrollBar.Create(WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN, 
                            rc, this, IDC_HSCROLLBAR);

    // Create the zoomin-out buttons then Load the bitmaps
    rc.right = rc.bottom = nHeight;
	m_BtnZoomIn.Create( "+", BS_OWNERDRAW | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, rc, this, BTN_ZOOMIN );
    m_BtnZoomIn.LoadBitmaps( IDB_ZOOMIN, IDB_ZOOMINDOWN );

	m_BtnZoomOut.Create( "-", BS_OWNERDRAW | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, rc, this, BTN_ZOOMOUT );
    m_BtnZoomOut.LoadBitmaps( IDB_ZOOMOUT, IDB_ZOOMOUTDOWN );
             
    // Create the default status string.
    SetupDefaultStatusText();
	
    m_hAcceleratorTable = LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_DLS_ACCELERATOR));

	// Do the necessary things with the property page
	IDMUSProdPropSheet* pIPropSheet = NULL;
	if(SUCCEEDED(pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
	{
		if(pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(m_pWave) == S_OK)
		{
			// Change the wave property page if it's up.
			if (m_pWave->m_pWavePropPgMgr && m_pWave->m_pWavePropPgMgr->m_pWavePage)
			{
				m_pWave->m_pWavePropPgMgr->m_pWavePage->EnableSelectionControls(true);
			}
		}

		RELEASE(pIPropSheet);
	}

	// Set the timer for autoscroll
	SetTimer(ID_WAVE_AUTOSCROLL_TIMER, 10, NULL);

    return 0;
}

// =============================================================================
//  OnDestroy
// =============================================================================
void CWaveCtrl::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	COleControl::OnDestroy();

	ASSERT(m_pWaveNode->GetComponent());

	if(m_hMenuInPlace)
	{
		::DestroyMenu(m_hMenuInPlace);
		m_hMenuInPlace = NULL;
	}

	// delete the context menu
	if (m_pRMenu)
	{
		delete m_pRMenu;
	}

	// Unplug in the pointer to this wavectrl in the wave.
	m_pWaveNode->SetWaveEditor(NULL);
}

// =============================================================================
//  OnCreate
// =============================================================================
HMENU CWaveCtrl::OnGetInPlaceMenu() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return m_hMenuInPlace;
}

void CWaveCtrl::OnEditRedo() 
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	// No Wave?
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(FAILED(m_pWave->Redo()))
	{
		return;
	}

	SetupDefaultStatusText();
	
    m_pWave->OnWaveBufferUpdated();
	
	m_pWave->RefreshPropertyPage();
	SetSelectionBounds();
	UpdateSelectionInPropertyPage();
	UpdateLoopInPropertyPage();

	m_nSamplesPerPixelBeforeZoom = m_nSamplesPerPixel;
	ResetScrollBar();
	InvalidateRect(NULL);
}

void CWaveCtrl::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	CString		csMenuText,csShortCut;

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	csMenuText.LoadString(IDS_REDO_MENU_TEXT);
	csShortCut.LoadString(IDS_REDO_SHORTCUT_TEXT);

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}
	
    CString sStateName = m_pWave->GetUndoMenuText(true);
	if(!sStateName.IsEmpty())
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText(csMenuText + _T(" ") + sStateName + csShortCut);
	}
	else 
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetText(csMenuText + csShortCut);
	}
}

void CWaveCtrl::OnEditUndo() 
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// No Wave?
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(FAILED(m_pWave->Undo()))
	{
		return;
	}
	
	SetupDefaultStatusText();
		
    m_pWave->OnWaveBufferUpdated();

	m_pWave->RefreshPropertyPage();
	SetSelectionBounds();
	UpdateSelectionInPropertyPage();
	UpdateLoopInPropertyPage();

	m_nSamplesPerPixelBeforeZoom = m_nSamplesPerPixel;
	ResetScrollBar();
	InvalidateRect(NULL);
}

void CWaveCtrl::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	CString		csMenuText,csShortCut;

	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	csMenuText.LoadString(IDS_UNDO_MENU_TEXT);
	csShortCut.LoadString(IDS_UNDO_SHORTCUT_TEXT);

    ASSERT(m_pWave);
    if(m_pWave == NULL)
    {
        return;
    }

	CString sStateName = m_pWave->GetUndoMenuText();

	if(!sStateName.IsEmpty())
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText(csMenuText + _T(" ") + sStateName + csShortCut);
	}
	else 
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetText(csMenuText + csShortCut);
	}
}

void CWaveCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    // we only want to process the control scroll bar!!
    // In case anyone makes the window scrollable!
    ASSERT(pScrollBar);

	SCROLLINFO	stgInfo;

    switch(nSBCode)
    {
    case SB_LINELEFT:
    {
		// 20 pixels left
        m_nCurPos -= 20; 
    }
    break;

    case SB_LINERIGHT:
    {
        // 20 pixels right
        m_nCurPos += 20; 
    }
    break;

    case SB_PAGELEFT:
    {
		stgInfo.cbSize = sizeof(SCROLLINFO);
        stgInfo.fMask = SIF_ALL;
		m_HScrollBar.GetScrollInfo(&stgInfo,SIF_ALL);

        m_nCurPos -= stgInfo.nPage;
    }
    break;

    case SB_PAGERIGHT:
    {
		stgInfo.cbSize = sizeof(SCROLLINFO);
        stgInfo.fMask = SIF_ALL;
		m_HScrollBar.GetScrollInfo(&stgInfo,SIF_ALL);

        m_nCurPos += stgInfo.nPage;
    }
    break;

	case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
    {
		stgInfo.cbSize = sizeof(SCROLLINFO);
        stgInfo.fMask = SIF_TRACKPOS;
		m_HScrollBar.GetScrollInfo(&stgInfo,SIF_TRACKPOS);
		m_nCurPos = stgInfo.nTrackPos;
    }
    break;
        
    default:
        break;

    }   //switch

    // set the scrollpos.
    if (m_nCurPos > 0)
    {
        m_nCurPos = min(m_dwMaxScrollPos, (DWORD)m_nCurPos);
    }
    else
    {
        m_nCurPos = max (0, m_nCurPos);
    }

    if (m_nZoomFactor >= 0)
    {
        m_nxSampleOffset = m_nCurPos * m_nSamplesPerPixel;
    }
    else
    {
        m_nxSampleOffset = m_nCurPos/m_nSamplesPerPixel;
    }

    pScrollBar->SetScrollPos(m_nCurPos, true);
    // cause a ondraw to be called.
    InvalidateRect(NULL);   
}

// ===================================================================================
// ===================================================================================
void CWaveCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	COleControl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// ===================================================================================
//  OnCommand
//      Handles the button-click notifications
// ===================================================================================
BOOL CWaveCtrl::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    switch(LOWORD(wParam))
    {
        case BTN_ZOOMIN:
			SetZoomFactor(--m_nZoomFactor);
			return TRUE;

        case BTN_ZOOMOUT:
			SetZoomFactor(++m_nZoomFactor);
			return TRUE;
                
        case ID_APP_ABOUT:
            ASSERT(TRUE);
            break;
    }

    return COleControl::OnCommand(wParam, lParam);

}

// ===================================================================================
// OnLButtonDown
//      Draws a line to show which sample is selected unless it's received while
//      dragging out a region.
// ===================================================================================
void CWaveCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return;
	}

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return;
	}

    // Convert to pixels.
    int nStart = SampleToPixel(m_nStartSel, true);
    int nEnd = SampleToPixel(m_nEndSel, true);
	
	int nLoopStart = SampleToPixel(m_pWave->m_rWLOOP.ulStart, true);
	int nLoopEnd = SampleToPixel(m_pWave->m_rWLOOP.ulLength + m_pWave->m_rWLOOP.ulStart - 1, true);
	bool bWaveIsLooped = (m_pWave->m_bPlayLooped != 0);

    // Check for end point first. In the case of a new line,
    // we always assume that the end is being dragged out.
	DRAGCURSOR dc;
#ifdef _DEBUG
	dc = dcArrow;
#endif
    if(point.x <= nEnd + 1 && point.x >= nEnd - 1)
    {
        m_nDragDir = _END;
		dc = dcSizeSelection;
    }
	else if(bWaveIsLooped && point.x <= nLoopEnd + 1 && point.x >= nLoopEnd - 1)
    {
        m_nDragDir = _END;
		m_bDraggingLoop = true;
		dc = dcSizeLoopEnd;
    }
    else if (point.x <= nStart+1 && point.x >= nStart-1)  
    {
        m_nDragDir = _START;
		dc = dcSizeSelection;
    }
	else if(bWaveIsLooped && point.x <= nLoopStart + 1 && point.x >= nLoopStart - 1)  
    {
        m_nDragDir = _START;
		m_bDraggingLoop = true;
		dc = dcSizeLoopStart;
    }
    else
    {
        int nSel = PixelToSample(point.x, true);
		SnapToSelectionBoundaries(nSel);
		m_nStartSel = m_nEndSel = nSel; // since there's no region.
        m_nDragDir = _END;
		dc = dcSizeSelection;
    }
	ASSERT(dc != dcArrow);

    UpdateStatusBar();

    m_bInSelMode = true;
    SetSizeCursor(true, dc);    // change to move cursor.
    SetCapture();

	// Show the wave's properties if the property page is up
	IDMUSProdPropSheet* pIPropSheet = NULL;
	if(SUCCEEDED(pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
	{
		if(pIPropSheet->IsShowing() == S_OK)
		{	
			m_pWave->OnShowProperties();
		}
		
		RELEASE(pIPropSheet);
	}

	SetFocus();
}

// ===================================================================================
// ===================================================================================
void CWaveCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWaveNode == NULL)
	{
		return;
	}

    if (m_bSizeCursorSet)
    {
        ReleaseCapture();
	    m_bInSelMode = false;

		if(m_bDraggingLoop)
		{
			SetDraggedLoopPoint(point);
		}

		m_bDraggingLoop = false;
        
        SetSizeCursor(false);    
        m_nDragDir = _NONE;
        InvalidateRect(NULL);
		
		CDLSComponent* pComponent = m_pWaveNode->GetComponent();
		ASSERT(pComponent);
		if(pComponent == NULL)
		{
			return;
		}

		// Snap to zero is turned on?
		if(pComponent->IsSnapToZero())
		{
			SnapToZero();
		}

		UpdateSelectionInPropertyPage(false);
    }

    if (m_nStartSel > m_nEndSel)
    {
        SwapPoints();
    }

	SetFocus();
}

// ===================================================================================
// OnMouseMove
//  Sets the m_nStartSel and m_nEndSel points and draws it by calling InvalidateRect.
// ===================================================================================
void CWaveCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// No Wave?
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	if((int)m_pWave->m_dwWaveLength <= 0)
		return;

    RECT rc;
    GetClientRect(&rc); 

    int nStart = SampleToPixel(m_nStartSel, true);
    int nEnd = SampleToPixel(m_nEndSel, true);
	
	int nLoopStart = SampleToPixel(m_pWave->m_rWLOOP.ulStart, true);
	int nLoopEnd = SampleToPixel(m_pWave->m_rWLOOP.ulLength + m_pWave->m_rWLOOP.ulStart - 1, true);
	bool bWaveIsLooped = (m_pWave->m_bPlayLooped != 0);

    ASSERT(nStart >= 0);
    ASSERT(nEnd >= 0);

	// we're not currently dragging a region or loop points
    if (!m_bInSelMode)  
    {
        if (point.x <= nEnd + 1 && point.x >= nEnd - 1)
        {
            m_nDragDir = _END;
	        SetSizeCursor(true, dcSizeSelection);
        }
        else if(point.x <= nStart + 1 && point.x >= nStart - 1)
        {
			m_nDragDir = _START;
	        SetSizeCursor(true, dcSizeSelection);
        }
		else if(bWaveIsLooped && point.x <= nLoopEnd + 1 && point.x >= nLoopEnd - 1)
        {
            m_nDragDir = _END;
	        SetSizeCursor(true, dcSizeLoopEnd);
        }
		else if(bWaveIsLooped && point.x <= nLoopStart + 1 && point.x >= nLoopStart - 1)
        {
			m_nDragDir = _START;
	        SetSizeCursor(true, dcSizeLoopStart);
        }
    }
    else    //we're dragging a region. lbtndown is received but no lbtnup.
    {
		if(!m_bDraggingLoop)
			SetSelection(point);
		else
			m_ptDraggedLoop = point;	
        
		// Invalidate only the changed rect
		InvalidateRect(NULL);
    }
}


void CWaveCtrl::SetSelection(CPoint ptSelection)
{
	DWORD dwOldStartSel = m_nStartSel;
	DWORD dwOldEndSel = m_nEndSel;

	if (m_nDragDir == _START)
    {
		m_nStartSel = PixelToSample(ptSelection.x, true);
		SnapToSelectionBoundaries(m_nStartSel);
    }
    else if (m_nDragDir == _END)
    {
		m_nEndSel = PixelToSample(ptSelection.x, true);
		SnapToSelectionBoundaries(m_nEndSel);
    }

	int nSelMin = -(int)(m_pWave->GetDwDecompressedStart(true));

	// All this to make sure we preserve the selection length.....
	if(m_pWave && m_pWave->m_pWavePropPgMgr && m_pWave->m_pWavePropPgMgr->m_pWavePage)
	{
		if(m_pWave->m_pWavePropPgMgr->m_pWavePage->IsSelectionLengthLocked())
		{
			if(m_nDragDir == _START)
			{
				m_nEndSel += m_nStartSel - dwOldStartSel;
				int nSelMax = (int)m_pWave->m_dwWaveLength + nSelMin;
				if(m_nEndSel > nSelMax)
				{
					m_nEndSel = nSelMax;
					m_nStartSel = m_nEndSel - (dwOldEndSel - dwOldStartSel);
				}
			}
			else if(m_nDragDir == _END)
			{
				m_nStartSel += m_nEndSel - dwOldEndSel;
				if(m_nStartSel < nSelMin)
				{
					m_nStartSel = nSelMin;
					m_nEndSel = m_nStartSel + (dwOldEndSel - dwOldStartSel);
				}
			}
		}
	}
}

void CWaveCtrl::SetDraggedLoopPoint(CPoint ptDraggedPoint)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// No Wave? Huh?
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	// Not dragging loop point? How did we get here?
	if(!m_bDraggingLoop || m_nDragDir == _NONE)
	{
		return;
	}

	// Is the loop length locked?
	bool bLoopLengthLocked = false;
	if(m_pWave->m_pWavePropPgMgr && m_pWave->m_pWavePropPgMgr->m_pWavePage)
	{
		bLoopLengthLocked = m_pWave->m_pWavePropPgMgr->m_pWavePage->IsLoopLengthLocked();
	}

	WLOOP waveLoop = m_pWave->GetWLOOP();

	DWORD dwLoopLength = waveLoop.ulLength;
	int nLoopStart = waveLoop.ulStart;
	int nLoopEnd = waveLoop.ulStart + waveLoop.ulLength;
	
	bool bWaveIsLooped = m_pWave->IsLooped();

	int nNewPosition = PixelToSample(ptDraggedPoint.x, true);
	if(m_nDragDir == _START)
	{
		nLoopStart = nNewPosition;
		if(bLoopLengthLocked)
			nLoopEnd = nLoopStart + dwLoopLength;
	}
	if(m_nDragDir == _END)
	{
		nLoopEnd = nNewPosition;
		if(bLoopLengthLocked)
			nLoopStart = nLoopEnd - dwLoopLength;
	}

	int nLoopMax = (int)m_pWave->GetDwSelMax();
	nLoopStart = max(0, nLoopStart);
	if (nLoopStart > nLoopMax)
		nLoopStart = nLoopMax;
	nLoopEnd = max(0, nLoopEnd);
	if (nLoopEnd > nLoopMax)
		nLoopEnd = nLoopMax;

	// Swap the points if necessary
	if (nLoopStart > nLoopEnd)
	{
		int nTemp = nLoopStart;
		nLoopStart = nLoopEnd;
		nLoopEnd = nTemp;
	}

	DWORD dwNewLoopLength = nLoopEnd - nLoopStart;
	if(bLoopLengthLocked)
	{
		dwNewLoopLength = dwLoopLength;
		if(nLoopStart + (int)dwNewLoopLength > nLoopMax)
		{
			nLoopStart = nLoopMax - dwLoopLength;
		}
	}
	
	if(dwNewLoopLength < MINIMUM_VALID_LOOPLENGTH)
	{
		return;
	}

	if(bLoopLengthLocked && dwLoopLength != dwNewLoopLength)
	{
		return;
	}

	// Save the undo state
	if(FAILED(SaveStateForUndo(IDS_UNDO_MODIFY_LOOP)))
	{
		return;
	}

	if(dwNewLoopLength >= MSSYNTH_LOOP_LIMIT)
	{
		CWnd* pWnd = GetFocus();
		int nChoice = AfxMessageBox(IDS_WARN_MSSYNTH_LOOP_LIMIT, MB_OKCANCEL);
		if(pWnd)
		{
			pWnd->SetFocus();
		}
		
		if(nChoice == IDCANCEL)
		{
			dwNewLoopLength = MSSYNTH_LOOP_LIMIT - 1;
		}
	}

	// Set the loop value now...
	waveLoop.ulStart = nLoopStart;
	waveLoop.ulLength = dwNewLoopLength;

	m_pWave->SetWLOOP(waveLoop);

	if(FAILED(m_pWave->UpdateWave()))
	{
		return;
	}

	UpdateLoopInRegions();
    m_pWave->UpdateDummyInstrument();

	UpdateLoopInPropertyPage();
	
    // Dirty the collection so we know the file gets saved
	CCollection* pCollection = m_pWave->GetCollection();
	if(pCollection)
	{
		pCollection->SetDirtyFlag();
	}

    InvalidateRect(NULL);
}

void CWaveCtrl::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(IsClipboardFormatAvailable(CF_WAVE))
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
	
}

void CWaveCtrl::OnUpdatePaste(CCmdUI* pCmdUI) 
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	if(IsClipboardFormatAvailable(CF_WAVE))
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}


void CWaveCtrl::OnUpdateSetLoopFromSelection(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// disable if selection has negative values
	if ((m_nStartSel < 0) || (m_nEndSel < 0))
		{
		pCmdUI->Enable(FALSE);
		return;
		}

	// command update UI handler
	// disable the snaptozero menu if there is no region selected.
	UpdateSelectionCmdUI(pCmdUI);
}

void CWaveCtrl::OnEditDelete() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	DeleteSelection();
}

void CWaveCtrl::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	UpdateSelectionCmdUI(pCmdUI);
}


void CWaveCtrl::OnDelete() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	DeleteSelection();
}

void CWaveCtrl::OnUpdateDelete(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	UpdateSelectionCmdUI(pCmdUI);
}


void CWaveCtrl::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	UpdateSelectionCmdUI(pCmdUI);
}

void CWaveCtrl::OnUpdateCopy(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	UpdateSelectionCmdUI(pCmdUI);
}

void CWaveCtrl::OnUpdateCut(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	UpdateSelectionCmdUI(pCmdUI);
}

void CWaveCtrl::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	UpdateSelectionCmdUI(pCmdUI);
}

void CWaveCtrl::OnSelectAll() 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	DWORD dwDecompressedStart = m_pWave->GetDwDecompressedStart(true);
	SetSelection(-(int)(dwDecompressedStart), m_pWave->m_dwWaveLength - dwDecompressedStart - 1);
	InvalidateRect(NULL);
}

void CWaveCtrl::OnUpdateSelectAll(CCmdUI* pCmdUI) 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWave->m_dwWaveLength == 0)
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
	}
}



void CWaveCtrl::UpdateSelectionCmdUI(CCmdUI * pCmdUI)
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Disabled if no selection
	if ( m_nEndSel - m_nStartSel < 1)
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
	}
}


// ===================================================================================
// ===================================================================================
void CWaveCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(m_pRMenu);
	if(m_pRMenu == NULL)
	{
		return;
	}

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return;
	}

	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	// Load the popup menu.
	CMenu* pPopupMenu = NULL;

    pPopupMenu = m_pRMenu->GetSubMenu(0);
    if (pPopupMenu)
    {
        ASSERT(IsMenu(pPopupMenu->m_hMenu));
        pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON,
								   point.x,
								   point.y,
								   this,
								   NULL);
	}
}

// ===================================================================================
// ===================================================================================
void CWaveCtrl::OnSnapToZero() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(m_pRMenu);

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);


    // the checked state toggles. 
    if (m_pRMenu->CheckMenuItem(IDM_SNAP_TO_ZERO, MF_BYCOMMAND) == MF_CHECKED)
    {
        m_pRMenu->CheckMenuItem(IDM_SNAP_TO_ZERO, MF_UNCHECKED);
		pComponent->SetSnapToZero(FALSE);
    }
    else
    {
	    pComponent->SetSnapToZero(TRUE);
        m_pRMenu->CheckMenuItem(IDM_SNAP_TO_ZERO, MF_CHECKED);
    }

    if (pComponent->IsSnapToZero() == TRUE)
    {
		// This sets the m_nSnapStart and End values also.
        SnapToZero();   
        // Cause a redraw; OnDraw will update statusbar.
        InvalidateRect(NULL);   

		IDMUSProdPropSheet* pIPropSheet = NULL;
		if(SUCCEEDED(pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
		{
			if(pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(m_pWave) == S_OK)
			{	
				// Update the property page
				if(m_pWave->m_pWavePropPgMgr && m_pWave->m_pWavePropPgMgr->m_pWavePage)
				{
					m_pWave->m_pWavePropPgMgr->m_pWavePage->SetSelection(m_nStartSel, m_nEndSel);
					m_pWave->m_pWavePropPgMgr->m_pWavePage->SetSelectionLengthLock(false);
				}
			}

			RELEASE(pIPropSheet);
		}
    }
}


// =========================================================================
// TODO: Handle errors!
//
// =========================================================================
void CWaveCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

    switch(nChar)
    {
		case VK_SPACE :
		{
      		if(m_nMIDINoteOns[m_pWave->m_rWSMP.usUnityNote] > 0)
            {
                break;
            }

            m_pWave->Play(TRUE);
			m_nMIDINoteOns[m_pWave->m_rWSMP.usUnityNote]++;
			break;
		}

		case VK_DELETE :
		{
			DeleteSelection();
			break;
		}

		case VK_INSERT:
			OnInsertSilence();
			break;

		case 'a':
		case 'A':
		{
			// Don't react to Ctrl + Alt + A...that's a shortcut for transport play button....
			BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
			BOOL bAlt = (GetKeyState(VK_MENU) & 0x8000);
			BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000);
			if(bCtrl && !bAlt && !bShift)
			{
				OnSelectAll();
			}
			break;
		}

		case 'I':
		case 'i':
		{
			SetZoomFactor(--m_nZoomFactor);
			break;
		}

		case 'O':
		case 'o':
		{
			SetZoomFactor(++m_nZoomFactor);
			break;
		}
    }
}

void CWaveCtrl::DeleteSelection()
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	CWaveDataManager* pDataManager = m_pWave->GetDataManager();
	ASSERT(pDataManager);
	if(pDataManager == NULL)
		return;

	// Can not allow modifications to a compressed wave.
	if(m_pWave->m_bCompressed == true)
	{
		CWnd* pWnd = GetFocus();
		AfxMessageBox(IDS_ERR_DELETE_ON_COMPRESSED_WAVE, MB_ICONEXCLAMATION); 
		if(pWnd)
			pWnd->SetFocus();
		return;
	}

	if((int)m_pWave->m_dwWaveLength <= 1)
		return;

	if(FAILED(SaveStateForUndo(IDS_UNDO_DELETE_SELECTION)))
		return;
	
	if(FAILED(RemoveSelection()))
	{
		// We don't delete the state because the action
		// may fail due to insuffcient memory in update 
		// for the synth. In that case the data has already 
		// been removed and deleting the state will mean 
		// that the user can't go back to the original state
		if(FAILED(pDataManager->Undo()))
			return;
	}

	if(FAILED(m_pWave->OnWaveBufferUpdated()))
	{
		pDataManager->Undo();
		return;
	}

	UpdateStatusBar();
	UpdateRefRegions();

	// Update the sample length in the property page
	m_pWave->RefreshPropertyPage();

	ResetScrollBar();
	
	// HACK! HACK! This recalculates the scroll position and 
	// prevents the jump/disappearance of the wave
	OnHScroll(0, m_nCurPos, &m_HScrollBar);
	InvalidateRect(NULL);

}

// =========================================================================
// =========================================================================
void CWaveCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch (nChar)
    {
    case VK_SPACE:     // 32
		while (m_nMIDINoteOns[m_pWave->m_rWSMP.usUnityNote] > 0)
        {

			m_pWaveNode->Stop( TRUE );
            m_nMIDINoteOns[m_pWave->m_rWSMP.usUnityNote]--;
        }
	}	
}


// =========================================================================
// =========================================================================
void CWaveCtrl::OnProperties() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
    // Call the onshowproperties in the CWave.
    if (m_pWave)
    {
        m_pWave->OnShowProperties();
    }
}


// =========================================================================
// =========================================================================
void CWaveCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	COleControl::OnSetFocus(pOldWnd);

    UpdateStatusBar();
}       


// ===========================================================================
// Private Functions
// ===========================================================================
void CWaveCtrl::ResetScrollBar(void)
{
    // compute the new range.
    // Get the width of the window. Assuming zoom == 1 for now.
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    RECT rc;
	int nRemPixels;

    GetClientRect(&rc);
	if (m_nZoomFactor >= 0)
	{
		nRemPixels = ((m_pWave->m_dwWaveLength)/m_nSamplesPerPixel) - rc.right;
	}
	else
	{
		nRemPixels = ((m_pWave->m_dwWaveLength)*m_nSamplesPerPixel) - rc.right;
	}

/**************************************************************************************/
/*	ScrollBar parameters are a little funky. PageSize is supposed to be the amount    */
/*  of data that can fit into one screen. MaxScroll is the highest value that would   */
/*  be used in a call to SetScrollPos() on the scrollbar (if given a number greater   */
/*  than the max specified here the scrollbar would set its position to max). But     */
/*  dragging the thumb returns a different range of values. The Maximum value ret-    */
/*  urned by the thumb is given by: MaxScrollPos = MaxRangeValue - (PageSize - 1)     */
/*	where MaxRangeValue and PageSize are specified in SetScrollInfo. Consequently I   */
/*  am going to set the MaxRangeVal such that the m_dwMaxScrollPosition is the range  */
/*  and enforce that on the range of values determined for clicking the arrow buttons.*/
/*  (as opposed to querying the scrollbar for it's range to enforce the limit.)		  */
/**************************************************************************************/

	m_dwMaxScrollPos = max (0, nRemPixels + 100);

	int nPageSize;
	int nMaxHScroll;
	nPageSize = m_dwMaxScrollPos ? min((int)m_dwMaxScrollPos, rc.right) : rc.right;
	nMaxHScroll = m_dwMaxScrollPos + (nPageSize - 1);

	SCROLLINFO	stgInfo;
	stgInfo.cbSize = sizeof(SCROLLINFO);
	stgInfo.fMask = SIF_PAGE | SIF_RANGE;
	stgInfo.nPage = nPageSize;
	stgInfo.nMin = 0;
	stgInfo.nMax = nMaxHScroll;
	m_HScrollBar.SetScrollInfo(&stgInfo,true);

    // Get the current position - In case of zoom we need to reset the scroll pos.
	int nCenterSample;
	int nWidth;

	// Determine the center sample of the selection
	nCenterSample = m_nStartSel + ((m_nEndSel - m_nStartSel) / 2);

	// Check and do the needful if the center of 
	// the selection is not in the client rect 
	int nTempSamplesPerPixel = m_nSamplesPerPixel;
	m_nSamplesPerPixel = m_nSamplesPerPixelBeforeZoom;
	DWORD dwFirstSample = PixelToSample(rc.left, false);
	DWORD dwLastSample = PixelToSample(rc.right, false);
	m_nSamplesPerPixel = nTempSamplesPerPixel;

	
	if(nCenterSample < (int)dwFirstSample || nCenterSample > (int)dwLastSample)
	{
		nCenterSample = dwFirstSample + ((dwLastSample - dwFirstSample) / 2);
	}
	
	// Get the width, in samples of one screen
	if (m_nZoomFactor >= 0)
		nWidth = (rc.right) * m_nSamplesPerPixel;
	else
		nWidth = (rc.right) / m_nSamplesPerPixel;

	// Set offset so as to center the selection
	m_nxSampleOffset = (nCenterSample - (nWidth / 2));
	m_nxSampleOffset = max(0,m_nxSampleOffset);
	
    // Setup the scroll pos to reflect the current sampleoffset.
	if (m_nZoomFactor >= 0)
	{
		m_nCurPos = (m_nxSampleOffset / m_nSamplesPerPixel);
	}
	else
	{
		m_nCurPos = (m_nxSampleOffset * m_nSamplesPerPixel);
	}
    m_HScrollBar.SetScrollPos(m_nCurPos);
}

// ===================================================================================
// SetSizeCursor.
//      taken from regionkeyboard.
// ===================================================================================
void CWaveCtrl::SetSizeCursor(bool bOn, DRAGCURSOR dc)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	/*if (bOn == m_bSizeCursorSet)
		return;*/

	if(bOn)
		{
		HCURSOR hcursor;
		switch (dc)
			{
			case dcSizeLoopStart:
				if (m_hSizeLoopStartCursor == NULL)
					m_hSizeLoopStartCursor = theApp.LoadCursor(MAKEINTRESOURCE(IDC_SIZEWE_GREEN));
				hcursor = m_hSizeLoopStartCursor;
				break;
				
			case dcSizeLoopEnd:
				if (m_hSizeLoopEndCursor == NULL)
					m_hSizeLoopEndCursor = theApp.LoadCursor(MAKEINTRESOURCE(IDC_SIZEWE_BLUE));
				hcursor = m_hSizeLoopEndCursor;
				break;
				
			default:
				ASSERT(FALSE); // unknown drag cursor
				// fall thru

			case dcSizeSelection:
				if (m_hSIZEWECursor == NULL)
					m_hSIZEWECursor = theApp.LoadStandardCursor(MAKEINTRESOURCE(IDC_SIZEWE));
				hcursor = m_hSIZEWECursor;
				break;
			}

		if (hcursor != NULL)
		{
			m_bSizeCursorSet = true;
			m_hPrevCursor = SetCursor(hcursor);
		}
	}
	else
	{
		ASSERT(dc == dcArrow);
		m_bSizeCursorSet = false;
		SetCursor(m_hPrevCursor);
	}
}

// ===================================================================================
// 
// ===================================================================================
// Returns the sample that draws at this pixel. Counts from start if bCountFromDecompressedStart is false. Sample can be negative.
int CWaveCtrl::PixelToSample(int xVal, bool bCountFromDecompressedStart)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    int nSample = 0;

    if (m_nZoomFactor < 0)
    {   // we're zoomed in.
        nSample = xVal/m_nSamplesPerPixel;
    }
    else
    {
        nSample = xVal * m_nSamplesPerPixel;
    }

    nSample += m_nxSampleOffset;

	if (bCountFromDecompressedStart)
		nSample -= m_pWave->GetDwDecompressedStart(true);

    return nSample;
}

// ===================================================================================
// ===================================================================================
// Returns the pixel the sample should draw at. Counts from start if bCountFromDecompressedStart is false. Sample can be negative.
int CWaveCtrl::SampleToPixel(int nSamp, bool bCountFromDecompressedStart)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (bCountFromDecompressedStart)
		nSamp += m_pWave->GetDwDecompressedStart(true);

    if ( (nSamp == 0) || ( (int)nSamp < m_nxSampleOffset ) )
        return 0;

    int nx = 0;
    if (m_nZoomFactor < 0)
    {
        nx = (nSamp - m_nxSampleOffset) * m_nSamplesPerPixel ;
    }
    else
    {
        nx = (nSamp - m_nxSampleOffset)/m_nSamplesPerPixel;
    }

    return nx;
}

// ===================================================================================
//  Set the zoom factor to a new value.  If the zoom factor is out of range, it is
//  adjusted.  The allowable range is based on the size of the wave sample and the
//  size of the window.  Calling SetZoomFactor(m_nZoomFactor) has the effect of
//  assuring the zoom factor is within limits.
// ===================================================================================
void CWaveCtrl::SetZoomFactor(int nNewZoomFactor)
{
	CRect rect;
	GetClientRect(&rect);

	double dMaxSampsPerPixel = ((double)m_pWave->m_dwWaveLength / rect.right);
	int nMaxZoomFactor = (int)ceil(log(dMaxSampsPerPixel) / log(2));

    m_nZoomFactor = min(m_nZoomFactor, nMaxZoomFactor);
	m_nZoomFactor = max(m_nZoomFactor, -5);
    
	m_nSamplesPerPixelBeforeZoom = m_nSamplesPerPixel;

	m_nSamplesPerPixel = 1 << abs(m_nZoomFactor);
    
	ResetScrollBar();
    InvalidateRect(NULL);
}
 
// ===================================================================================
// ===================================================================================
void CWaveCtrl::UpdateStatusBar()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    char sz[255] = "Zoom ";
    char szT[255];

    if (m_nZoomFactor == 0)
    {
        strcat(sz, " 1:1");
    }
    else if (m_nZoomFactor > 0) // zoomed out
    {
        strcat(sz, "1:");
        _itoa(1 << m_nZoomFactor, szT, 10);
        strcat(sz, szT);
    }
    else if (m_nZoomFactor < 0) //zoomed in.
    {
        _itoa(1 << abs(m_nZoomFactor), szT, 10);
        strcat(sz, szT);
        strcat(sz, ":1");
    }

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);

    // Setup the status bar text for the 1st pane
    // Setup the status bar text.
    CString cstr1(sz);      //NOTE: only 10 chars displayed in status bar.
  	BSTR bstr1 = cstr1.AllocSysString();
	pComponent->m_pIFramework->SetStatusBarPaneText( m_hKeyStatusBar, 0, bstr1, TRUE );

	// Swap the selection points if start > end
	DWORD dwSelectionStart = m_nStartSel;
	DWORD dwSelectionEnd = m_nEndSel;
	DWORD dwTemp = m_nStartSel;
	if(dwSelectionStart > dwSelectionEnd)
	{
		dwSelectionStart = dwSelectionEnd;
		dwSelectionEnd = dwTemp;
	}

    sz[0] = '\0';
    if (m_nStartSel == 0 && m_nEndSel == 0)
    {
        strcat(sz, m_szDefault);
    }
    else
    {
		
		CString sStartTime = m_pWave->SampleToTime(dwSelectionStart);
		CString sEndTime = m_pWave->SampleToTime(dwSelectionEnd);
		
        strcat(sz, " Samples ");
        _itoa(dwSelectionStart, szT, 10);
        strcat(sz, szT);
		strcat(sz, " (Time ");
		strcat(sz,  sStartTime);
		strcat(sz, ")");
        strcat(sz, " thru ");
        _itoa(dwSelectionEnd, szT, 10);
        strcat(sz, szT);
		strcat(sz, " (Time ");
		strcat(sz, sEndTime);
		strcat(sz, ")");
        strcat(sz, " selected.");
    }

    //Setup the status bar text.
    CString cstr2(sz);  //NOTE: only 30 chars displayed.
	int nLength = cstr2.GetLength();
    BSTR bstr2 = cstr2.AllocSysString();
	
	pComponent->m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 1, SBS_SUNKEN, nLength );
	pComponent->m_pIFramework->SetStatusBarPaneText( m_hKeyStatusBar, 1, bstr2, TRUE );
}


// ===================================================================================
// ===================================================================================
void CWaveCtrl::DrawLoopPoints(CDC* pDC)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;
	
	WLOOP waveLoop = m_pWave->GetWLOOP();
    if(waveLoop.ulLength >= 1)
    {
		int nPenStyle = m_pWave->IsLooped() ? PS_SOLID : PS_DOT;

		// start loop sample
		DrawLoopCursor(*pDC, nPenStyle, SampleToPixel(waveLoop.ulStart, true), _START);
        
		// end loop sample
		int nEnd = waveLoop.ulLength + waveLoop.ulStart - 1;
		DrawLoopCursor(*pDC, nPenStyle, SampleToPixel(nEnd, true), _END);
    }
}


// ===================================================================================
// SnapToZero
//  If m_pWave->m_bSnapToZero is true, the selection moves to a point where the value is >= the zero
//  line and the previous sample is below the zero line. 
//  The search is in both directions and the closest sample that satisfies this
//  conditionis chosen.
// ===================================================================================
void CWaveCtrl::SnapToZero(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	// No wave??
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;
	
	// No component?
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
		return;

	// Snap to zero is turned on?
    ASSERT(pComponent->IsSnapToZero() == TRUE);

    // Check what the values are. If they're the same that we did a snap on before, don't do it again.
    bool bSnapStart = (m_nSnapStart != m_nStartSel); 
    bool bSnapEnd = (m_nSnapEnd != m_nEndSel);
	if (bSnapStart || bSnapEnd)
		{
		DWORD dwDecompressedStart = m_pWave->GetDwDecompressedStart(true);

		if(bSnapStart)
		{
			DWORD dwStartSel = m_nStartSel+dwDecompressedStart;
			m_pWave->SnapToZero(dwStartSel);
			m_nSnapStart = m_nStartSel = dwStartSel-dwDecompressedStart;
		}
		
		if(bSnapEnd)
		{
			DWORD dwEndSel = m_nEndSel+dwDecompressedStart;
			m_pWave->SnapToZero(dwEndSel);
			m_nSnapEnd = m_nEndSel = dwEndSel-dwDecompressedStart;
		}
	}
}


// =========================================================================
// =========================================================================
void CWaveCtrl::OnSetLoopFromSelection() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// loop points must be positive
	if ((m_nStartSel < 0) || (m_nEndSel < 0))
		{
		ASSERT(FALSE);
		return;
		}

	if(m_pWave->m_dwWaveLength < MINIMUM_VALID_LOOPLENGTH)
		return;

	if(FAILED(SaveStateForUndo(m_pWave->m_bPlayLooped ? IDS_UNDO_LOOPPOINTS : IDS_UNDO_LOOPSETTINGS)))
		return;

	// Take the current region selection and set it to be the loop points.
    if (m_nStartSel > m_nEndSel)
        SwapPoints();

	if(abs(m_nEndSel - m_nStartSel) < MINIMUM_VALID_LOOPLENGTH)
	{
		CWnd* pWnd = GetFocus();
		AfxMessageBox(IDS_LOOP_SELECTION_TOO_SMALL, MB_ICONEXCLAMATION);
		if(pWnd)
			pWnd->SetFocus();
		return;
	}

	if(abs(m_nEndSel - m_nStartSel) >= MSSYNTH_LOOP_LIMIT)
	{
		CWnd* pWnd = GetFocus();
		int nChoice = AfxMessageBox(IDS_WARN_MSSYNTH_LOOP_LIMIT, MB_OKCANCEL);
		if(pWnd)
			pWnd->SetFocus();
		if(nChoice == IDCANCEL)
			return;
	}

    if(FAILED(m_pWave->SetLoop(m_nStartSel, m_nEndSel)))
		return;

	UpdateLoopInPropertyPage();
	m_pWave->NotifyWaveChange(true);
    InvalidateRect(NULL);
}


// =========================================================================
//  SetupDefaultStatusText
// =========================================================================
void CWaveCtrl::SetupDefaultStatusText()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT(m_pWave);

	char szT[20];
    _itoa(m_pWave->m_rWaveformat.nSamplesPerSec, m_szDefault, 10);
    strcat(m_szDefault, " Hz ");
    _itoa(m_pWave->m_rWaveformat.wBitsPerSample, szT, 10);
    strcat(m_szDefault, szT);
    strcat(m_szDefault, " bit "); 
	if(m_pWave->IsStereo())
	{
		strcat(m_szDefault, "Stereo, ");
	}
	else
	{
		strcat(m_szDefault, "Mono, ");
	}

    _itoa(m_pWave->m_dwWaveLength, szT, 10);
    strcat(m_szDefault, szT);
    strcat(m_szDefault, " samples");

}


// =========================================================================
//  SwapPoints
//      Swaps the Start and End Selection points.
// =========================================================================
void CWaveCtrl::SwapPoints()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    DWORD nT = m_nStartSel;
	int dwSnapT = m_nSnapStart; 
    
	m_nStartSel = m_nEndSel;
	m_nSnapStart = m_nSnapEnd;

    m_nEndSel = nT;
	m_nSnapEnd = dwSnapT;
}

// =========================================================================
// Update the playback position during wave play. nNewPos should be a pixel
// position of where the new playback position is.  If pRectInvalid != NULL,
// the previous playback pos is simply redrawn, and nNewPos is ignored.
// =========================================================================
void CWaveCtrl::UpdatePlaybackPos(int nNewPos, CRect* pRectInvalid /* = NULL */)
{
	static bool bPrevWasRedraw = false;		// was previous call a redraw request?
	static CRect rectInvalidPrev;			// the previous invalid rectangle

	if (nNewPos == m_nPlaybackPos) {
		return;
	}

	CDC * pdc = GetDC();
	ASSERT(pdc);

	// Return without doing anything if we fail to get the DC
	if(pdc == NULL)
		return;
	
	// draw in inverted mode
	int nOldMode = pdc->SetROP2(R2_NOT);
	
	RECT rc;
	GetClientRect(&rc);
	
	if (m_nPlaybackPos != 0) 
	{
		
		// figure out drawing/erasing rectangle
		CRect rectDraw;
		if (pRectInvalid != NULL)
			rectDraw = *pRectInvalid;
		else if (bPrevWasRedraw)
			rectDraw = rectInvalidPrev;
		else 
			rectDraw = rc;
		
		// erase old playback pos (or redraw if pRectInvalid != NULL)
		if (m_nPlaybackPos >= rectDraw.left && m_nPlaybackPos <= rectDraw.right) 
		{
			pdc->MoveTo(m_nPlaybackPos, rectDraw.top);
			pdc->LineTo(m_nPlaybackPos, rectDraw.bottom);
			// keep track of prev invalid rect
			if (pRectInvalid)
				rectInvalidPrev = *pRectInvalid;
			bPrevWasRedraw = (pRectInvalid != NULL);
		}
	}

	if (pRectInvalid == NULL) 
	{
		m_nPlaybackPos = nNewPos;

		if (m_nPlaybackPos != 0) 
		{
			// draw new playback pos
			pdc->MoveTo(m_nPlaybackPos, 0);
			pdc->LineTo(m_nPlaybackPos, rc.bottom);
		}
	}
	
	// restore drawing mode
	pdc->SetROP2(nOldMode);
	ReleaseDC(pdc);
}

// =========================================================================
// Prepare timer for keeping track of sample playback.
// =========================================================================
void CWaveCtrl::PreparePlaybackTimer()
{
	m_uiTimer = SetTimer(ID_WAVE_PLAYBACK_TIMER, 10, NULL);
	m_bPlayedLooped = (m_pWave->m_bPlayLooped == TRUE);
	m_sPlayFineTune = m_pWave->m_rWSMP.sFineTune; 
	if (m_bPlayedLooped) {
		m_nPlayLoopStart = m_pWave->m_rWLOOP.ulStart;
		m_nPlayLoopLength = m_pWave->m_rWLOOP.ulLength;
	}

}

// =========================================================================
// Kill timer for keeping track of sample playback.
// =========================================================================
void CWaveCtrl::KillPlaybackTimer()
{
	if (m_uiTimer) {
		// kill playback timer
		KillTimer(ID_WAVE_PLAYBACK_TIMER);
		m_uiTimer = 0;
		// stop showing playback pos
		UpdatePlaybackPos(0);
	}
}

void CWaveCtrl::OnAppAbout() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CAboutDlg AboutDlg;
	AboutDlg.DoModal();
	
}

// =========================================================================
//  Restores status bars
// =========================================================================
void CWaveCtrl::OnHideToolBars() 
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( m_pWaveNode != NULL );	// Need m_pWave to remove status bar panes
	if(m_pWaveNode)
    {
		CDLSComponent* pComponent = m_pWaveNode->GetComponent();
		ASSERT(pComponent);

        pComponent->m_pIFramework->RestoreStatusBar( m_hKeyStatusBar );
    }

	m_hKeyStatusBar = NULL;
}

// =========================================================================
//  Sets up status bar
// =========================================================================
void CWaveCtrl::OnShowToolBars() 
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pInPlaceFrame == NULL )
	{
		return;
	}

	m_pInPlaceFrame->SetActiveObject( &m_xMyOleInPlaceActiveObject, NULL );
    
	// Create status bar panes if they don't already exist
	if( m_hKeyStatusBar == NULL )
	{
		if(m_pWaveNode)
		{
			CDLSComponent* pComponent = m_pWaveNode->GetComponent();
			ASSERT(pComponent);

			pComponent->m_pIFramework->SetNbrStatusBarPanes( 2, SBLS_EDITOR, &m_hKeyStatusBar );
    		pComponent->m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 0, SBS_SUNKEN, 10);
    		pComponent->m_pIFramework->SetStatusBarPaneInfo( m_hKeyStatusBar, 1, SBS_SUNKEN, 34 );
		}
	}

	if( m_pInPlaceDoc != NULL )
	{
		m_pInPlaceDoc->SetActiveObject( &m_xMyOleInPlaceActiveObject, NULL );
	}
		
}

void CWaveCtrl::OnEditCopy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pWave->m_bCompressed)
	{
		CWnd* pWnd = GetFocus();
		AfxMessageBox(IDS_ERR_CLIPBOARD_ON_COMPRESSED_WAVE, MB_ICONEXCLAMATION); 
		if(pWnd)
			pWnd->SetFocus();

		return;
	}
	CopySelectionToClipboard();	
}


void CWaveCtrl::OnCopy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pWave->m_bCompressed)
	{
		CWnd* pWnd = GetFocus();
		AfxMessageBox(IDS_ERR_CLIPBOARD_ON_COMPRESSED_WAVE, MB_ICONEXCLAMATION); 
		if(pWnd)
			pWnd->SetFocus();
		return;
	}

	CopySelectionToClipboard();	
}

void CWaveCtrl::OnEditPaste() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pWave->m_bCompressed)
	{
		CWnd* pWnd = GetFocus();
		AfxMessageBox(IDS_ERR_CLIPBOARD_ON_COMPRESSED_WAVE, MB_ICONEXCLAMATION); 
		if(pWnd)
			pWnd->SetFocus();
		return;
	}

	PasteFromClipboard();
	UpdateStatusBar();
	m_pWave->RefreshPropertyPage();
}

void CWaveCtrl::OnPaste() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pWave->m_bCompressed)
	{
		CWnd* pWnd = GetFocus();
		AfxMessageBox(IDS_ERR_CLIPBOARD_ON_COMPRESSED_WAVE, MB_ICONEXCLAMATION); 
		if(pWnd)
			pWnd->SetFocus();
		return;
	}

	PasteFromClipboard();
	UpdateStatusBar();
	m_pWave->RefreshPropertyPage();
}

void CWaveCtrl::OnEditCut() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pWave->m_bCompressed)
	{
		CWnd* pWnd = GetFocus();
		AfxMessageBox(IDS_ERR_CLIPBOARD_ON_COMPRESSED_WAVE, MB_ICONEXCLAMATION); 
		if(pWnd)
			pWnd->SetFocus();
		return;
	}

	if((int)m_pWave->m_dwWaveLength <= 0)
		return;

	CutSelectionToClipboard();
	UpdateStatusBar();
	m_pWave->RefreshPropertyPage();
}


void CWaveCtrl::OnCut() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_pWave->m_bCompressed)
	{
		CWnd* pWnd = GetFocus();
		AfxMessageBox(IDS_ERR_CLIPBOARD_ON_COMPRESSED_WAVE, MB_ICONEXCLAMATION); 
		if(pWnd)
			pWnd->SetFocus();
		return;
	}

	if((int)m_pWave->m_dwWaveLength <= 1)
		return;

	CutSelectionToClipboard();
	UpdateStatusBar();
	m_pWave->RefreshPropertyPage();
}

void CWaveCtrl::CutSelectionToClipboard()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	CWaveDataManager* pDataManager = m_pWave->GetDataManager();
	ASSERT(pDataManager);
	if(pDataManager == NULL)
		return;

	if(FAILED(CopySelectionToClipboard()))
		return;

	// Out Of Memory??
	if(FAILED(SaveStateForUndo(IDS_WAVE_CUT_UNDO_TEXT)))
		return;

	if(FAILED(RemoveSelection()))
	{
		// We don't delete the state because the action
		// may fail due to insuffcient memory in update 
		// for the synth. In that case the data has already 
		// been removed and deleting the state will mean 
		// that the user can't go back to the original state
		pDataManager->Undo();
		return;
	}

	// Pop the undo state if the update fails
	if(FAILED(m_pWave->OnWaveBufferUpdated()))
	{
		pDataManager->Undo();
		return;
	}
	
	UpdateStatusBar();
	UpdateRefRegions();

	m_pWave->RefreshPropertyPage();

	ResetScrollBar();
	
	// HACK! HACK! This recalculates the scroll position and 
	// prevents the jump/disappearance of the wave
	OnHScroll(0, m_nCurPos, &m_HScrollBar);
	InvalidateRect(NULL);
}

HRESULT CWaveCtrl::CopySelectionToClipboard()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
		
	DWORD dwStartSel = m_nStartSel;
	DWORD dwEndSel = m_nEndSel;

	CDllJazzDataObject* pDataObject = new CDllJazzDataObject;
	ASSERT(pDataObject);
	if(pDataObject == NULL)
	{
		// Error
		return E_OUTOFMEMORY;
	}

	IStream* pIStream = NULL;
	if(FAILED(CreateStreamOnHGlobal(NULL, TRUE, &pIStream)))
	{
		// Errror
		return E_OUTOFMEMORY;
	}

	if(FAILED(m_pWave->CopySelectionToClipboard(pIStream, dwStartSel, dwEndSel)))
	{
		pIStream->Release();
		return E_FAIL;
	}

	HRESULT hr = pDataObject->AddClipFormat(CF_WAVE, pIStream);
	
	if(m_pClipboardDataObject != NULL)
	{
		m_pClipboardDataObject->Release();
	}
	
	m_pClipboardDataObject = pDataObject;

	if(S_OK != OleSetClipboard(pDataObject))
	{
		hr = E_FAIL;
	}

	pIStream->Release();
	return hr;
}

void CWaveCtrl::PasteFromClipboard()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	IStream*				pIStream = NULL;
	CDllJazzDataObject*		pJazzDataObject = NULL;
	IDataObject*			pIDataObject = NULL;
	HRESULT					hr = E_FAIL;
	short*					pnBuffer = NULL;

	
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}


	CWaveDataManager* pDataManager = m_pWave->GetDataManager();
	ASSERT(pDataManager);
	if(pDataManager == NULL)
	{
		return;
	}

	
	if(FAILED(SaveStateForUndo(IDS_WAVE_PASTE_UNDO_TEXT)))
	{
		return;
	}


	hr = OleGetClipboard(&pIDataObject);
	if ( FAILED(hr) )
	{
		//error
		return;
	}
	
	pJazzDataObject = new CDllJazzDataObject;
	if (pJazzDataObject == NULL)
	{
		//error
		return;
	}
	
	if( SUCCEEDED (	pJazzDataObject->IsClipFormatAvailable(pIDataObject, CF_WAVE) ) )
	{
		if( SUCCEEDED(pJazzDataObject->AttemptRead(pIDataObject, CF_WAVE, &pIStream) ) )
		{
			DWORD dwDecompressedStart = m_pWave->GetDwDecompressedStart(true);

			// Check if the start selection is valid...
			int nSelMax = m_pWave->GetDwSelMax();
			if(m_nStartSel > nSelMax)
				m_nStartSel = nSelMax;

			if(FAILED(m_pWave->PasteSelectionFromClipboard(pIStream, m_nStartSel+dwDecompressedStart)))
			{
				goto ERROR_EXIT;
			}

			if(FAILED(m_pWave->OnWaveBufferUpdated()))
			{
				// Call Undo here because the insertion of data has succeeded
				// and the failure is most likely due to out of memory while
				// downloading to the synth....if we just delete the state the
				// synth will not be in sync with what we hold...
				pDataManager->Undo();
				goto ERROR_EXIT;
			}
			
			UpdateRefRegions();
			
			SetupDefaultStatusText();

			ResetScrollBar();
			InvalidateRect(NULL);
		}
	}
	else
	{
		pIDataObject->Release();
		return;
	}
ERROR_EXIT:
	pIDataObject->Release();
	pIStream->Release();
	pJazzDataObject->Release();
}

HRESULT CWaveCtrl::RemoveSelection()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = S_OK;
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	if((int)m_pWave->m_dwWaveLength <= 0)
	{
		return E_FAIL;
	}

	DWORD dwStartSel = m_nStartSel;
	DWORD dwEndSel = m_nEndSel;

	WSMPL waveWSMPL = m_pWave->GetWSMPL();
	WLOOP waveLoop = m_pWave->GetWLOOP();
	RSMPL waveRSMPL = m_pWave->GetRSMPL();
	RLOOP waveRLoop = m_pWave->GetRLOOP();

	// Are we trimming the beginning of a looped wave?
	bool bCutBeforeLoopStart = false;
	if(m_nStartSel <= (int)waveLoop.ulStart)
		bCutBeforeLoopStart = true;

	if(FAILED(m_pWave->RemoveSelection(dwStartSel, dwEndSel)))
	{
		return E_FAIL;
	}
	
	if((int)dwStartSel <= 0)
		dwStartSel = 1;

	DWORD dwSamplesRemoved = abs(dwEndSel - dwStartSel);
	m_nSnapStart = m_nSnapEnd = m_nEndSel = m_nStartSel = dwStartSel - 1;

	int nLoops = 0;
	bool bPlayLooped = false;

	DWORD dwWaveLength = m_pWave->GetWaveLength();

	// Adjust the loop points
	if(dwWaveLength >= MINIMUM_VALID_LOOPLENGTH)
	{
		// Adjust the loop start if the samples before it have been trimmed...
		if(bCutBeforeLoopStart)
			waveLoop.ulStart -= dwSamplesRemoved;

		if(((int)waveLoop.ulStart) > (int)dwWaveLength)
			waveLoop.ulStart = dwWaveLength - waveLoop.ulLength;

		if(((int)waveLoop.ulStart) < 0)
			waveLoop.ulStart = 0;
		
		if(waveLoop.ulStart + waveLoop.ulLength > dwWaveLength)
			waveLoop.ulLength = dwWaveLength - waveLoop.ulStart;

		if(m_pWave->IsLooped())
		{
			nLoops = 1;
			bPlayLooped = m_pWave->IsLooped();
		}
	}
	else
	{
		waveLoop.ulStart = 0;
		waveLoop.ulLength = 1;
	}

	// The synth deals with the WSMPL and WLOOP values
    // But we want to save the smpl values also
    waveWSMPL.cSampleLoops = nLoops;
    waveRSMPL.cSampleLoops = nLoops;
    waveRLoop.dwStart = waveLoop.ulStart;
    waveRLoop.dwEnd = waveLoop.ulStart + waveLoop.ulLength;

	m_pWave->SetWSMPL(waveWSMPL);
	m_pWave->SetWLOOP(waveLoop);
	m_pWave->SetRSMPL(waveRSMPL);
	m_pWave->SetRLOOP(waveRLoop);

	m_pWave->SetLooped(bPlayLooped);

	UpdateLoopInPropertyPage();
	UpdateSelectionInPropertyPage();
	
	SetupDefaultStatusText();
	UpdateStatusBar();

	return hr;
}

void CWaveCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);		

	COleControl::OnKillFocus(pNewWnd);
}

void CWaveCtrl::TurnOffMidiNotes()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
		return;

	for(BYTE i = 0; i < 128; i++)
	{
        while (m_nMIDINoteOns[i] > 0)
        {
	        pComponent->PlayMIDIEvent(MIDI_NOTEOFF, i, DEFAULT_VELOCITY, 0, false);
            m_nMIDINoteOns[i]--;
        }
    }
}

void CWaveCtrl::UpdateLoopInRegions()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	m_pWave->UpdateLoopInRegions();
}

void CWaveCtrl::UpdateRefRegions()
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	m_pWave->UpdateRefRegions();
}


void CWaveCtrl::OnTimer(UINT nIDEvent) 
{
    AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	COleControl::OnTimer(nIDEvent);
	if(nIDEvent == ID_WAVE_AUTOSCROLL_TIMER && m_bInSelMode)
	{
		bool bUpdateSelection = false;
		
		// LEFT = false; RIGHT = true
		bool bScrollDirection = false;		

		RECT rcClient;
		GetClientRect(&rcClient);

		CPoint ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);

		DWORD dwFirstSample = PixelToSample(rcClient.left, false);
		DWORD dwLastSample = PixelToSample(rcClient.right, false);
		
		// Reset the scroll position if the cursor is outside the frame
		if(ptCursor.x <= 0 && (int)dwFirstSample >= 0)
		{
			int nDiff = rcClient.left - ptCursor.x;

			// Faster scroll if the window is maximixed
			ASSERT(m_pWaveNode);
			HWND hwndEditor;
			m_pWaveNode->GetEditorWindow(&hwndEditor);
			if(::IsZoomed(hwndEditor))
				nDiff = 100;

			m_nCurPos = m_nCurPos - nDiff;
		
			bUpdateSelection = true;
		}
		else if(ptCursor.x > rcClient.right)
		{
			int nDiff = ptCursor.x - rcClient.right;
			
			// Faster scroll if the window is maximixed
			ASSERT(m_pWaveNode);
			HWND hwndEditor;
			m_pWaveNode->GetEditorWindow(&hwndEditor);
			if(::IsZoomed(hwndEditor))
				nDiff = 100;

			DWORD dwSampleAtCursor = PixelToSample(ptCursor.x, false);
			if(dwSampleAtCursor > dwLastSample)
				m_nCurPos = m_nCurPos + nDiff;

			bUpdateSelection = true;
			bScrollDirection = true;
		}

		if (m_nCurPos > 0)
		{
			m_nCurPos = min(m_dwMaxScrollPos, (DWORD)m_nCurPos);
		}
		else
		{
			m_nCurPos = max (0, m_nCurPos);
		}

		if (m_nZoomFactor >= 0)
		{
			m_nxSampleOffset = m_nCurPos * m_nSamplesPerPixel;
		}
		else
		{
			m_nxSampleOffset = m_nCurPos/m_nSamplesPerPixel;
		}

		m_HScrollBar.SetScrollPos(m_nCurPos, true);
		InvalidateRect(NULL);   

		if(bUpdateSelection && !m_bDraggingLoop)
		{
			GetClientRect(&rcClient);
			if(bScrollDirection)
				SetSelection(CPoint(rcClient.right, rcClient.bottom));
			else
				SetSelection(CPoint(rcClient.left, rcClient.bottom));
		}
	}

	if (nIDEvent == ID_WAVE_PLAYBACK_TIMER) 
	{
		// Calc new playback position
		int nElapsedTime = GetTickCount() - m_pWave->GetTimePlayed();
		if (nElapsedTime < 0) return;
		
		// Playback freq is affect
		double freq = pow(2, (double)m_sPlayFineTune / 1200) * 
			m_pWave->m_rWaveformat.nSamplesPerSec;
		
		double pos = (double)nElapsedTime/1000 * freq;

		// Update pos if wave is looped
		if (m_bPlayedLooped && m_nPlayLoopLength != 0) 
		{
			int nEndLoop = m_nPlayLoopStart + m_nPlayLoopLength;
			if (pos > nEndLoop) 
			{
				pos -= m_nPlayLoopStart;
				pos = (int)pos % m_nPlayLoopLength;
				pos += m_nPlayLoopStart;
			}
		}			

		if (pos > m_pWave->GetDwSelMax()) 
		{
			// If we are pas the sample's length, stop showing playback pos
			m_pWaveNode->Stop( TRUE );
			m_nCurPos = 0;
			OnHScroll(SB_PAGELEFT, m_nCurPos, &m_HScrollBar);
		}
		else 
		{
			// Update display of playback position
			int nNewPos = SampleToPixel((int)pos, true);

			if(nNewPos == 0 && m_pWave->m_bPlayLooped)
			{
				if (m_nZoomFactor < 0)
				{
					nNewPos = (int)((pos - m_nxSampleOffset) * m_nSamplesPerPixel);
				}
				else
				{
					nNewPos = (int)((pos - m_nxSampleOffset) / m_nSamplesPerPixel);
				}

				m_nCurPos += (nNewPos - 40);
				OnHScroll(SB_PAGELEFT, m_nCurPos, &m_HScrollBar);
			}
			else
			{

				RECT rcClient;
				GetClientRect(&rcClient);

				// Scroll one page left
				if(nNewPos < rcClient.left)
				{
					OnHScroll(SB_PAGELEFT, m_nCurPos, &m_HScrollBar);
				}
				else if(nNewPos > rcClient.right)
				{
					OnHScroll(SB_PAGERIGHT, m_nCurPos, &m_HScrollBar);
				}
			}

			nNewPos = max(0, nNewPos);

			UpdatePlaybackPos(nNewPos);
		}
	}

}

void CWaveCtrl::OnHelpFinder() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/directmusicproducer.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}
}

// Save the state for Undo
HRESULT CWaveCtrl::SaveStateForUndo(UINT uUndoTextID)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	if(FAILED(m_pWave->SaveUndoState(uUndoTextID)))
	{
		AfxMessageBox(IDS_ERR_INSUFFICIENT_MEMORY, MB_OK | MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	
	return S_OK;
}

void CWaveCtrl::SetSelection(int nSelectionStart, int nSelectionEnd, BOOL bDoSnap)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	m_nStartSel = nSelectionStart;
	m_nEndSel = nSelectionEnd;

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
		return;

	if(bDoSnap)
	{
		CDLSComponent* pComponent = m_pWaveNode->GetComponent();
		ASSERT(pComponent);

		if(pComponent->IsSnapToZero() == TRUE)
		{
			SnapToZero();
			UpdateSelectionInPropertyPage(false);
		}
	}
	else if(pComponent->IsSnapToZero() == TRUE)
	{
		// Brute force!
		m_nSnapStart = nSelectionStart;
		m_nSnapEnd = nSelectionEnd;
	}

	InvalidateRect(NULL);
}

void CWaveCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	// Don't select anything in a 0 length wave
	if((int)m_pWave->m_dwWaveLength <= 0)
		return;

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	
	// Turn SnapToZero off 
	if (pComponent->IsSnapToZero() == TRUE)
    {
        OnSnapToZero();
    }

	// Tuen of the selection length lock
	IDMUSProdPropSheet* pIPropSheet = NULL;
	if(SUCCEEDED(pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
	{
		if(pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(m_pWave) == S_OK)
		{	
			m_pWave->m_pWavePropPgMgr->m_pWavePage->SetSelectionLengthLock(false);
		}

		RELEASE(pIPropSheet);
	}


	DWORD dwSampleClicked = PixelToSample(point.x, true);

	if(!m_pWave->m_bPlayLooped)
	{
		m_nStartSel = 0;
		m_nEndSel = m_pWave->m_dwWaveLength - 1;
	}
	else if(dwSampleClicked <= m_pWave->m_rWLOOP.ulStart)
	{
		m_nStartSel = 0;
		m_nEndSel = m_pWave->m_rWLOOP.ulStart - 1;
	}
	else if(dwSampleClicked > m_pWave->m_rWLOOP.ulStart && dwSampleClicked <= (m_pWave->m_rWLOOP.ulStart + m_pWave->m_rWLOOP.ulLength))
	{
		m_nStartSel = m_pWave->m_rWLOOP.ulStart;
		m_nEndSel = (m_pWave->m_rWLOOP.ulStart + m_pWave->m_rWLOOP.ulLength - 1);
	}
	else if(dwSampleClicked > (m_pWave->m_rWLOOP.ulStart + m_pWave->m_rWLOOP.ulLength))
	{
		m_nStartSel = (m_pWave->m_rWLOOP.ulStart + m_pWave->m_rWLOOP.ulLength + 1);
		m_nEndSel = m_pWave->m_dwWaveLength - 1;
	}

	InvalidateRect(NULL);
	UpdateSelectionInPropertyPage();

	
	COleControl::OnLButtonDblClk(nFlags, point);
}

	
void CWaveCtrl::UpdateSelectionInPropertyPage(bool bBroadcastUpdate)
{
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
		return;

	IDMUSProdPropSheet* pIPropSheet = NULL;
	if(SUCCEEDED(pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
	{
		if(pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(m_pWave) == S_OK)
		{	
			// Update the property page
			if(m_pWave->m_pWavePropPgMgr && m_pWave->m_pWavePropPgMgr->m_pWavePage)
			{
				m_pWave->m_pWavePropPgMgr->m_pWavePage->SetSelection(m_nStartSel, m_nEndSel, bBroadcastUpdate);
				if (pComponent->IsSnapToZero() == TRUE)
					m_pWave->m_pWavePropPgMgr->m_pWavePage->SetSelectionLengthLock(false);
			}
		}

		RELEASE(pIPropSheet);
	}
}

void CWaveCtrl::UpdateLoopInPropertyPage()
{
	ASSERT(m_pWave);
	ASSERT(m_pWaveNode);

	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);

	IDMUSProdPropSheet* pIPropSheet = NULL;
	if(SUCCEEDED(pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void**)&pIPropSheet)))
	{
		if(pIPropSheet->IsShowing() == S_OK && pIPropSheet->IsEqualPageManagerObject(m_pWave) == S_OK)
		{
			// Change the wave property page if it's up.
			if (m_pWave->m_pWavePropPgMgr)
			{
				if (m_pWave->m_pWavePropPgMgr->m_pWavePage)
				{
					if(m_pWave->m_dwWaveLength < MINIMUM_VALID_LOOPLENGTH)
					{
						m_pWave->m_bPlayLooped = false;
						m_pWave->m_pWavePropPgMgr->m_pWavePage->EnableLoopControls(false);
					}

					m_pWave->m_pWavePropPgMgr->m_pWavePage->m_dwLoopStart = m_pWave->m_rWLOOP.ulStart;
					m_pWave->m_pWavePropPgMgr->m_pWavePage->m_dwLoopLength = m_pWave->m_rWLOOP.ulLength;
					
					// update prop page 
					m_pWave->m_pWavePropPgMgr->m_pWavePage->InitializeDialogValues();
					m_pWave->m_pWavePropPgMgr->m_pWavePage->UpdateData(FALSE);

					// the compressed loop also gets modified
					m_pWave->RememberLoopAfterCompression();
				}
			}

		}

		RELEASE(pIPropSheet);
	}
}

void CWaveCtrl::GetSelection(int& nSelectionStart, int& nSelectionEnd)
{
	nSelectionStart = m_nStartSel;
	nSelectionEnd = m_nEndSel;
}

void CWaveCtrl::SetSelectionBounds()
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	DWORD dwDecompressedStart = m_pWave->GetDwDecompressedStart(true);

	int nSelMax = m_pWave->m_dwWaveLength-dwDecompressedStart;
	if(m_nStartSel >= nSelMax)
		m_nStartSel = nSelMax - 1;
	if(m_nEndSel >= nSelMax)
		m_nEndSel = nSelMax - 1;

	int nSelMin = -(int)dwDecompressedStart;
	if(m_nStartSel < nSelMin)
		m_nStartSel = nSelMin;
	if(m_nEndSel < nSelMin)
		m_nEndSel = nSelMin;

	if(m_nEndSel < m_nStartSel)
		m_nEndSel = m_nStartSel;
}

void CWaveCtrl::OnCopyLoop() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	// Wave's not looped?
	if(!m_pWave->IsLooped())
	{
		return;
	}

	if(FAILED(SaveStateForUndo(IDS_LOOP_COPY)))
	{
		return;
	}

	if(FAILED(m_pWave->CopyLoop()))
	{
		//m_pUndoMgr->DeleteState();
		return;
	}

    UpdateLoopInPropertyPage();
	m_pWave->NotifyWaveChange(true);
    InvalidateRect(NULL);
}

void CWaveCtrl::OnInsertSilence() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	CInsertSilenceDlg *pInsertSilenceDlg = new CInsertSilenceDlg(m_pWave->m_rWaveformat.nSamplesPerSec, this);
	if( !pInsertSilenceDlg )
	{
		return;
	}

	int nResult = pInsertSilenceDlg->DoModal();
	DWORD dwLength = pInsertSilenceDlg->GetSampleLength();
	delete pInsertSilenceDlg;

	if( IDOK != nResult )
	{
		return;
	}

	if(FAILED(SaveStateForUndo(IDS_UNDO_INS_SILENCE)))
	{
		return;
	}

	if(FAILED(m_pWave->InsertSilence(m_nStartSel, dwLength)))
	{
		//m_pUndoMgr->DeleteState();
		return;
	}

	if(FAILED(m_pWave->OnWaveBufferUpdated()))
    {
        return;
    }

    UpdateLoopInPropertyPage();
    InvalidateRect(NULL);
}

void CWaveCtrl::OnRegionFadeIn() 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	CWaveDataManager* pDataManager = m_pWave->GetDataManager();
	ASSERT(pDataManager);
	if(pDataManager == NULL)
	{
		return;
	}

	if(FAILED(SaveStateForUndo(IDS_UNDO_FADEIN)))
	{
		return;
	}

	if(FAILED(Fade()))
	{
		pDataManager->Undo();
	}
}

void CWaveCtrl::OnRegionFadeOut() 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	CWaveDataManager* pDataManager = m_pWave->GetDataManager();
	ASSERT(pDataManager);
	if(pDataManager == NULL)
	{
		return;
	}

	if(FAILED(SaveStateForUndo(IDS_UNDO_FADEOUT)))
	{
		return;
	}

	if(FAILED(Fade(false)))
	{
		pDataManager->Undo();
	}
}


HRESULT CWaveCtrl::Fade(bool bFadeDirection)
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return E_FAIL;
	}

	if(FAILED(m_pWave->Fade(bFadeDirection, m_nStartSel, m_nEndSel)))
	{
		return E_FAIL;
	}

	if(FAILED(m_pWave->OnWaveBufferUpdated()))
    {
        return E_FAIL;
    }

	InvalidateRect(NULL);
	return S_OK;
}

void CWaveCtrl::OnUpdateRegionFadeIn(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	UpdateSelectionCmdUI(pCmdUI);
}

void CWaveCtrl::OnUpdateRegionFadeOut(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	UpdateSelectionCmdUI(pCmdUI);
}

void CWaveCtrl::OnUpdateCopyLoop(CCmdUI* pCmdUI) 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;
	
	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

void CWaveCtrl::OnUpdateInsertSilence(CCmdUI* pCmdUI) 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;
	
	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

int CWaveCtrl::GetScrolledSamples()
{
	return m_nxSampleOffset;
}

int CWaveCtrl::GetSamplesPerPixel()
{
	return m_nSamplesPerPixel;
}

int CWaveCtrl::GetZoomFactor()
{
	return m_nZoomFactor;
}


void CWaveCtrl::OnFindBestLoop() 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	WLOOP wloop = m_pWave->GetWLOOP();
	DWORD dwLoopStart = wloop.ulStart;

	DWORD dwLoopEnd = 0;
	DWORD dwSearchEnd = m_nEndSel;
	if(m_nStartSel == m_nEndSel)
	{
		dwSearchEnd = m_pWave->GetWaveLength() - 1;
	}
	
	CWaitCursor wait;

	if(SUCCEEDED(m_pWave->FindBestLoop(dwLoopStart, m_nStartSel, dwSearchEnd, 128, &dwLoopEnd)))
	{
		SaveStateForUndo(IDS_UNDO_BEST_LOOP);
		m_pWave->SetLoop(dwLoopStart, dwLoopEnd);
		UpdateLoopInPropertyPage();
		m_pWave->NotifyWaveChange(true);
		InvalidateRect(NULL);
	}

}

void CWaveCtrl::OnUpdateFindBestLoop(CCmdUI* pCmdUI) 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	if(m_pWave->IsLooped())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CWaveCtrl::OnUpdateDecompressStart(CCmdUI* pCmdUI) 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;

	pCmdUI->Enable(m_pWave->FConsiderDecompressedStart());
}

void CWaveCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWaveNode);
	if(m_pWaveNode == NULL)
	{
		return;
	}

	// Activate transport if not activated
	CDLSComponent* pComponent = m_pWaveNode->GetComponent();
	ASSERT(pComponent);
	if(pComponent == NULL)
	{
		return;
	}

	pComponent->m_pIConductor->SetActiveTransport(m_pWaveNode, BS_PLAY_ENABLED | BS_NO_AUTO_UPDATE);
	
	COleControl::OnRButtonDown(nFlags, point);
}

void CWaveCtrl::OnDecompressStart() 
{
	ASSERT(m_pWave);
    if (m_pWave)
    	m_pWave->HrSetDecompressedStart(m_nStartSel+m_pWave->GetDwDecompressedStart(true));
}

/* ensures the given sample count is within selection boundaries */
void CWaveCtrl::SnapToSelectionBoundaries(int& nSel)
{
	ASSERT(m_pWave);
	if (m_pWave == NULL)
		return;

	DWORD dwDecompressedStart = m_pWave->GetDwDecompressedStart(true);
	int nSelMin = -(int)(dwDecompressedStart);
	int nSelMax = m_pWave->m_dwWaveLength-1-dwDecompressedStart;

	if (nSel < nSelMin)
		nSel = nSelMin;
	else if (nSel > nSelMax)
		nSel = nSelMax;
}

/* draws the given loop cursor using the given pen style */
void CWaveCtrl::DrawLoopCursor(CDC &dc, int nPenStyle, int x, DRAGDIR dd)
{
	ASSERT((dd == _START) || (dd == _END)); // unknown loop marker

	CRect rcClient;
	GetClientRect(&rcClient);
	if (x <= rcClient.right)
		{
		CPen pen(nPenStyle, 1, (dd == _START) ? RGB(0, 255, 0) : RGB(0, 0, 255));
		CPen* pOldPen = dc.SelectObject(&pen);
		ASSERT(pOldPen);
		dc.MoveTo(x, 0);
		dc.LineTo(x, rcClient.bottom);
		dc.SelectObject(pOldPen);
		}
}

void CWaveCtrl::OnResample() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pWave);
	if(m_pWave == NULL)
	{
		return;
	}

	CResampleDlg *pResampleDlg = new CResampleDlg(m_pWave->m_rWaveformat.nSamplesPerSec, this);
	if( !pResampleDlg )
	{
		return;
	}

	const int nResult = pResampleDlg->DoModal();
	const DWORD dwSampleRate = pResampleDlg->GetSampleRate();
	delete pResampleDlg;

	if( IDOK != nResult
	||	dwSampleRate == m_pWave->m_rWaveformat.nSamplesPerSec )
	{
		return;
	}

	if(FAILED(SaveStateForUndo(IDS_UNDO_RESAMPLE)))
	{
		return;
	}

	CWaitCursor wait;

	if(FAILED(m_pWave->Resample(dwSampleRate)))
	{
		//m_pUndoMgr->DeleteState();
		return;
	}

	if(FAILED(m_pWave->OnWaveBufferUpdated()))
    {
        return;
    }

    UpdateLoopInPropertyPage();
    InvalidateRect(NULL);
}

void CWaveCtrl::OnUpdateResample(CCmdUI* pCmdUI) 
{
	ASSERT(m_pWave);
	if(m_pWave == NULL)
		return;
	
	if(m_pWave->m_bCompressed)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}
