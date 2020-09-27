// RegionKeyboardCtl.cpp : Implementation of the CRegionKeyboardCtrl ActiveX Control class.

#include "stdafx.h"
#include "KeyBoardMap.h"
#include "RegionKeyboard.h"
#include "RegionKeyboardCtl.h"
#include "RegionKeyboardPpg.h"
#include "Region.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CRegionKeyboardApp theApp;

IMPLEMENT_DYNCREATE(CRegionKeyboardCtrl, COleControl)

#define MIDI_NOTEON     0x90
#define MIDI_NOTEOFF    0x80

// These contant variables are determined by the bitmap used for keyboard. If it is changed
// they must be updated as well as enum PAGE_SIZE_PIXEL and REGION_LAYER_HEIGHT
// It is assume that the keyboard bitmap has 128 keys
const int CRegionKeyboardCtrl::m_nRegionKeyboardSize = 600;
const int CRegionKeyboardCtrl::m_nKeyPos[12] = {0, 5, 10, 13, 18, 24, 29, 34, 37, 42, 45, 50};
const int CRegionKeyboardCtrl::m_nKeySize[12] = {5, 5, 4, 5, 5, 5, 5, 4, 4, 4, 5, 5};
//old values for m_nKeyPos[12] = {0, 5, 10, 14, 19, 24, 29, 34, 38, 42, 46, 51};
// These 2 arrays are used to draw the circle for the rootnote.
const int CRegionKeyboardCtrl::m_nKeyCenter[12] = {4, 7, 12, 15, 20, 28, 31, 36, 39, 44, 47, 52};
// white keys have a radius of 2 - black keys have a radius 1. 
const int CRegionKeyboardCtrl::m_nRadius[12] = {2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2};

const int NUMKEYS = 127;

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CRegionKeyboardCtrl, COleControl)
	//{{AFX_MSG_MAP(CRegionKeyboardCtrl)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CRegionKeyboardCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CRegionKeyboardCtrl)
	DISP_FUNCTION(CRegionKeyboardCtrl, "InitializeKeyBoard", InitializeKeyBoard, VT_EMPTY, VTS_UNKNOWN)
	DISP_FUNCTION(CRegionKeyboardCtrl, "SetCurrentRegion", SetCurrentRegion, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CRegionKeyboardCtrl, "MidiEvent", MidiEvent, VT_EMPTY, VTS_I2 VTS_I2 VTS_I2)
	DISP_FUNCTION(CRegionKeyboardCtrl, "SetRootNote", SetRootNote, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CRegionKeyboardCtrl, "InsertNewRegion", InsertNewRegion, VT_I2, VTS_I2 VTS_I2 VTS_I2 VTS_I2)
	DISP_FUNCTION(CRegionKeyboardCtrl, "InsertRegion", InsertRegion, VT_I2, VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_BSTR)
	DISP_FUNCTION(CRegionKeyboardCtrl, "SetRange", SetRange, VT_BOOL, VTS_I2 VTS_I2 VTS_I2 VTS_I2)
	DISP_FUNCTION(CRegionKeyboardCtrl, "DeleteRegion", DeleteRegion, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CRegionKeyboardCtrl, "DeleteAllRegions", DeleteAllRegions, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CRegionKeyboardCtrl, "ReleaseMouseCapture", ReleaseMouseCapture, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CRegionKeyboardCtrl, "TurnOffMidiNotes", TurnOffMidiNotes, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CRegionKeyboardCtrl, "SetCurrentLayer", SetCurrentLayer, VT_I2, VTS_I2)
	DISP_FUNCTION(CRegionKeyboardCtrl, "SetFirstVisibleLayer", SetFirstVisibleLayer, VT_I2, VTS_I2)
	DISP_FUNCTION(CRegionKeyboardCtrl, "ScrollLayers", ScrollLayers, VT_I2, VTS_I2 VTS_I2)
	DISP_FUNCTION(CRegionKeyboardCtrl, "GetNumberOfLayers", GetNumberOfLayers, VT_I2, VTS_NONE)
	DISP_FUNCTION(CRegionKeyboardCtrl, "GetFirstVisibleLayer", GetFirstVisibleLayer, VT_I2, VTS_NONE)
	DISP_FUNCTION(CRegionKeyboardCtrl, "AddNewLayer", AddNewLayer, VT_I2, VTS_NONE)
	DISP_FUNCTION(CRegionKeyboardCtrl, "DeleteActiveLayer", DeleteActiveLayer, VT_I2, VTS_NONE)
	DISP_FUNCTION(CRegionKeyboardCtrl, "SetAuditionMode", SetAuditionMode, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION(CRegionKeyboardCtrl, "EnableRegion", EnableRegion, VT_EMPTY, VTS_I2 VTS_I2 VTS_BOOL)
	DISP_FUNCTION(CRegionKeyboardCtrl, "SetNumberOfLayers", SetNumberOfLayers, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CRegionKeyboardCtrl, "SetWaveName", SetWaveName, VT_EMPTY, VTS_I2 VTS_I2 VTS_BSTR)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CRegionKeyboardCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CRegionKeyboardCtrl, COleControl)
	//{{AFX_EVENT_MAP(CRegionKeyboardCtrl)
	EVENT_CUSTOM("RegionSelectedChanged", FireRegionSelectedChanged, VTS_I2 VTS_I2)
	EVENT_CUSTOM("NewRegion", FireNewRegion, VTS_I2 VTS_I4  VTS_I4)
	EVENT_CUSTOM("RangeChanged", FireRangeChanged, VTS_I2 VTS_I4  VTS_I4)
	EVENT_CUSTOM("NotePlayed", FireNotePlayed, VTS_I4  VTS_BOOL)
	EVENT_CUSTOM("RegionMoved", FireRegionMoved, VTS_I2  VTS_I2  VTS_I2  VTS_I2  VTS_I2)
	EVENT_CUSTOM("RegionDeleted", FireRegionDeleted, VTS_I2  VTS_I2)
	EVENT_CUSTOM("ActiveLayerChanged", FireActiveLayerChanged, VTS_I2)
	EVENT_CUSTOM("CopyRegion", FireCopyRegion, VTS_I2  VTS_I2  VTS_I2  VTS_I2)
	EVENT_CUSTOM("ScrollLayers", FireScrollLayers, VTS_BOOL)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CRegionKeyboardCtrl, 1)
	PROPPAGEID(CRegionKeyboardPropPage::guid)
END_PROPPAGEIDS(CRegionKeyboardCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CRegionKeyboardCtrl, "REGIONKEYBOARD.RegionKeyboardCtrl.1",
	0x36cd3187, 0xee61, 0x11d0, 0x87, 0x6a, 0, 0xaa, 0, 0xc0, 0x81, 0x46)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CRegionKeyboardCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DRegionKeyboard =
		{ 0x36cd3185, 0xee61, 0x11d0, { 0x87, 0x6a, 0, 0xaa, 0, 0xc0, 0x81, 0x46 } };
const IID BASED_CODE IID_DRegionKeyboardEvents =
		{ 0x36cd3186, 0xee61, 0x11d0, { 0x87, 0x6a, 0, 0xaa, 0, 0xc0, 0x81, 0x46 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwRegionKeyboardOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CRegionKeyboardCtrl, IDS_REGIONKEYBOARD, _dwRegionKeyboardOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardCtrl::CRegionKeyboardCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CRegionKeyboardCtrl

BOOL CRegionKeyboardCtrl::CRegionKeyboardCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_REGIONKEYBOARD,
			IDB_REGIONKEYBOARD,
			afxRegInsertable | afxRegApartmentThreading,
			_dwRegionKeyboardOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardCtrl::CRegionKeyboardCtrl - Constructor

int CRegionKeyboardCtrl::m_nKeyboards = 0;

HANDLE CRegionKeyboardCtrl::m_hKeyBoard128 = NULL;
HANDLE CRegionKeyboardCtrl::m_hBlank128 = NULL;
CBitmap CRegionKeyboardCtrl::m_bmKeyBoard128;
CBitmap CRegionKeyboardCtrl::m_bmBlank128;

HCURSOR	CRegionKeyboardCtrl::m_hArrowCursor = NULL;
HCURSOR	CRegionKeyboardCtrl::m_hStartEditCursor = NULL;
HCURSOR	CRegionKeyboardCtrl::m_hEndEditCursor = NULL;
HCURSOR	CRegionKeyboardCtrl::m_hSIZEWECursor = NULL;
HCURSOR	CRegionKeyboardCtrl::m_hMOVECursor = NULL;
HCURSOR	CRegionKeyboardCtrl::m_hCOPYCursor = NULL;
HCURSOR	CRegionKeyboardCtrl::m_hNODOCursor = NULL;


CRegionKeyboardCtrl::CRegionKeyboardCtrl() : m_nCurKeyPos(0), m_nKeyboardOffset(0), m_nCurRegion(0),	
											 m_nRegionMapIndex(0), m_bHasCapture(false), m_ptLButtonDown(-1,-1),
											 m_bStartCursorSet(false), m_bEndCursorSet(false), 
											 m_nStartEnd(RK_NULL), m_nMaxEndPixel(CRegionKeyboardCtrl::m_nRegionKeyboardSize),
											 m_nMinEndPixel(0), m_bNoteIsOn(false), m_ptLastNote(NULL), m_pDraggedRegion(NULL), 
											 m_nActiveLayer(1), m_nNumberOfLayers(NUMBER_OF_VISIBLE_LAYERS),
											 m_nFirstVisibleLayer(0), m_pCurrentRegion(NULL), m_nDraggedKey(0), m_bAuditionMode(TRUE),
											 m_bRegionFontCreated(false)
{
	InitializeIIDs(&IID_DRegionKeyboard, &IID_DRegionKeyboardEvents);

	m_nKeyboards++;
	
	if(m_hKeyBoard128 == NULL)
	{
		m_hKeyBoard128 = LoadImage(theApp.m_hInstance,
							   MAKEINTRESOURCE(IDB_KEYBOARD_128),
							   IMAGE_BITMAP,
							   0,
							   0,
							   LR_CREATEDIBSECTION);

		ASSERT(m_hKeyBoard128);
		
		if(m_hKeyBoard128)
		{
			m_bmKeyBoard128.Attach(m_hKeyBoard128);
		}
	}

	
	if(m_hBlank128 == NULL)
	{
		m_hBlank128 = LoadImage(theApp.m_hInstance,
							MAKEINTRESOURCE(IDB_BLANK_128),
							IMAGE_BITMAP,
							0,
							0,
							LR_CREATEDIBSECTION);

		ASSERT(m_hBlank128);
		
		if(m_hBlank128)
		{
			m_bmBlank128.Attach(m_hBlank128);
		}

		memset(m_nLastMidiNoteOn, 0, sizeof(m_nLastMidiNoteOn));
	}

	// Load cursors
	if(m_hNODOCursor == NULL)
		m_hNODOCursor = theApp.LoadStandardCursor(MAKEINTRESOURCE(IDC_NO));

	if(m_hStartEditCursor == NULL)
		m_hStartEditCursor = theApp.LoadCursor(MAKEINTRESOURCE(IDC_CURSOR_STARTEDIT));

	if(m_hEndEditCursor == NULL)
		m_hEndEditCursor = theApp.LoadCursor(MAKEINTRESOURCE(IDC_CURSOR_ENDEDIT));
	
	if(m_hMOVECursor == NULL)
		m_hMOVECursor = theApp.LoadCursor(MAKEINTRESOURCE(IDC_CURSOR_MOVE));

	if(m_hCOPYCursor == NULL)
		m_hCOPYCursor = theApp.LoadCursor(MAKEINTRESOURCE(IDC_CURSOR_COPY));

	if(m_hArrowCursor == NULL)
		m_hArrowCursor = theApp.LoadStandardCursor(MAKEINTRESOURCE(IDC_ARROW));

	m_lastGhostRect.SetRectEmpty();
}

/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardCtrl::~CRegionKeyboardCtrl - Destructor

CRegionKeyboardCtrl::~CRegionKeyboardCtrl()
{
	DeleteAllRegions();

	m_nKeyboards--;

	if(m_nKeyboards <= 0)
	{
		m_bmKeyBoard128.Detach();
		m_bmBlank128.Detach();

		DeleteObject(m_hKeyBoard128);
		m_hKeyBoard128 = NULL;
		DeleteObject(m_hBlank128);
		m_hBlank128 = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardCtrl::OnDraw - Drawing function

void CRegionKeyboardCtrl::OnDraw(CDC* pdc, 
								const CRect& rcBounds, 
								const CRect& rcInvalid)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	// MFC based containers call OnDraw multiple times with different DC's.
	// We only want to paint once and when DC is client area of control.
	// This should happen when both rcBounds.top = 0 and rcBounds.left = 0
	if(rcBounds.top != 0 && rcBounds.left != 0)
	{
		TRACE0("Returning from RegionKeyboard control's OnDraw call without drawing\n");
		return;
	}
	
	DrawKeyboard(pdc, rcBounds);
}

/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardCtrl::DoPropExchange - Persistence support

void CRegionKeyboardCtrl::DoPropExchange(CPropExchange* pPX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

}

/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardCtrl::OnResetState - Reset control to default state

void CRegionKeyboardCtrl::OnResetState()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	// Resets defaults found in DoPropExchange
	COleControl::OnResetState();  
}

/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardCtrl::AboutBox - Display an "About" box to the user

void CRegionKeyboardCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	CDialog dlgAbout(IDD_ABOUTBOX_REGIONKEYBOARD);
	dlgAbout.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardCtrl message handlers

int CRegionKeyboardCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

    // Find out how many keys are initially visible?
    CRect rcClient;
    GetClientRect(rcClient);
    int nWidth = rcClient.Width();
    // on creation we display starting with key 0. 56 = size of octave in pixels
    int nOctaves = nWidth/56; // how many octaves are visible.
    int nVisible = nOctaves * 56;    //at least
    for (int key = 0; nWidth >= nVisible; key++)
    {
        nVisible += m_nKeySize[key];
    }
    
    //NOTE: The array above (m_nKeySize) is 0-based but we are interested in the 
    // number of keys which is 1-based. So we don't decrement the value of key.
    // If the value of key is 12 it means we are showing the full octave.
    
    //EnableScrollBarCtrl(SB_VERT, TRUE);


    int nVisibleKeys = (nOctaves*12) + key;  // number of keys is not 0-based. It's 128.

    m_nMaxScrollPos = NUMKEYS - nVisibleKeys; // This goes into 0-based again. numkeys = 127.

	SetScrollRange(SB_HORZ, 0, m_nMaxScrollPos, FALSE);	
	//ShowScrollBar(SB_VERT, TRUE);
 
	return 0;
}

void CRegionKeyboardCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	static int nCurrPos = 0;

	BITMAP bmInfo;
	int nStatus =  m_bmKeyBoard128.GetBitmap(&bmInfo);

	CClientDC clientDC(this);	
	CRect rcClientRect;
	GetClientRect(rcClientRect);

	int nMaxKeyboardOffset = bmInfo.bmWidth - rcClientRect.Width();
	nMaxKeyboardOffset = nMaxKeyboardOffset < 0 ? 0 : nMaxKeyboardOffset;
	
	switch(nSBCode)
	{
		case SB_LEFT:
			break;

		case SB_RIGHT:
			break;

		case SB_LINELEFT:
			nCurrPos -= 1;
			m_nKeyboardOffset -= m_nKeySize[m_nCurKeyPos];
			m_nCurKeyPos--;
			m_nCurKeyPos = m_nCurKeyPos < 0 ? 0 : m_nCurKeyPos;
			break;

		case SB_LINERIGHT:
			nCurrPos += 1;
			m_nCurKeyPos++;
			m_nCurKeyPos = m_nCurKeyPos > 11 ? 11 : m_nCurKeyPos;
			m_nKeyboardOffset += m_nKeySize[m_nCurKeyPos];
			break;

		case SB_PAGELEFT:
			nCurrPos -= PAGE_SIZE;
			m_nKeyboardOffset -= PAGE_SIZE_PIXELS;
			break;

		case SB_PAGERIGHT:
			nCurrPos += PAGE_SIZE;
			m_nKeyboardOffset += PAGE_SIZE_PIXELS;
			break;

		case SB_THUMBPOSITION:
			nCurrPos = nPos;
			int nNumOctaves = nPos / 12;
			m_nCurKeyPos = nPos % 12;
			if(nNumOctaves == 0 && m_nCurKeyPos == 0)
			{
				m_nKeyboardOffset = 0;
			}
			else
			{
				m_nKeyboardOffset = nNumOctaves * PAGE_SIZE_PIXELS;
                for (int i = 0; i <= m_nCurKeyPos; i++)
                    m_nKeyboardOffset += m_nKeySize[i];
			}
			break;
	}


	nCurrPos = max(0, min(nCurrPos, m_nMaxScrollPos));

	SetScrollPos(SB_HORZ, nCurrPos, TRUE);

	m_nKeyboardOffset = max(0, min(m_nKeyboardOffset, bmInfo.bmWidth));
	
	m_nKeyboardOffset = m_nKeyboardOffset < nMaxKeyboardOffset ? m_nKeyboardOffset : nMaxKeyboardOffset;

	DrawKeyboard(&clientDC, rcClientRect);
}

void CRegionKeyboardCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	SetFocus();
	SetActiveLayer(point);
 
	// Index of the key hit on the keyboard
	int nIndex = GetKeyIndexFromPoint(point);
	nIndex = nIndex < 0 ? 0 : nIndex;
	nIndex = nIndex > 127 ? 127 : nIndex;

	// This takes care of the condition when the user clicks on the keyboard but NOT on the region strip
	int nPlayedNote = GetNotePlayed(point);
	CRegion* pRegion = IsMemberOfRegion(nPlayedNote);
	if(pRegion && pRegion->IsSelected() == FALSE)
	{
		pRegion->SetSelected(true);
		if(m_pCurrentRegion)
		{
			m_pCurrentRegion->SetSelected(false);
		}

		m_pCurrentRegion = pRegion;
		FireRegionSelectedChanged(m_pCurrentRegion->GetLayer(), m_pCurrentRegion->GetStartNote());
		SetFocus();
		CClientDC clientDC(this);
		DrawRegionRects(&clientDC);
	}

	// Play the note...
	// turn off the last note
	int nLastNotePlayed = GetNotePlayed(m_ptLastNote);
	if(m_bNoteIsOn == true)
	{
		PlayNote(nLastNotePlayed, false); 
	}

	m_bNoteIsOn = true;
	SetCapture();
	PlayNote(nPlayedNote, TRUE); // Turn this note On
	/*if(m_bAuditionMode == AUDITION_MULTIPLE)
	{
		// Change the color for all the regions that will play for this note
		SetPlayColorForNote(nPlayedNote);
	}*/
	m_ptLastNote = point;

	// Now we start dealing with the actual region which was clicked
	pRegion = GetRegionForPoint(point, false);

	if(pRegion && !(nFlags & MK_CONTROL))
	{
		m_pDraggedRegion = pRegion;
		if(m_pDraggedRegion == NULL)
			return;

		SetCapture();
		m_bHasCapture = true;
		m_ptLButtonDown = point;
	
		if(m_pDraggedRegion->IsPointOnStartBoundary(point)) 
		{
			m_nStartEnd = RK_START;
			SetSizeCursor(true, true);
		}
		else if(m_pDraggedRegion->IsPointOnEndBoundary(point))
		{
			m_nStartEnd = RK_END;
			SetSizeCursor(true, false);
		}
		else
		{
			m_nStartEnd = RK_MOVE;
			m_nDraggedKey = GetKeyIndexFromPoint(point) - m_pDraggedRegion->GetStartNote();
			SetMoveCursor(true);
		}

		if(m_pCurrentRegion)
		{
			FireRegionSelectedChanged(m_pCurrentRegion->GetLayer(), m_pCurrentRegion->GetStartNote());
			SetFocus();
		}

		if ( m_bStartCursorSet || m_bEndCursorSet ||m_bMoveCursorSet )
			return;
	} 
	else if(pRegion && (nFlags & MK_CONTROL))
	{
		SetCapture();
		m_bHasCapture = true;
		m_ptLButtonDown = point;
		m_nDraggedKey = GetKeyIndexFromPoint(point) - pRegion->GetStartNote();

		m_nStartEnd |= RK_COPY;
		SetCopyCursor(true);

		SetCurrentRegion(m_nActiveLayer, pRegion->GetStartNote());
		m_pDraggedRegion = pRegion;
		
		return;
	}
		
	int nNumOctaves = (point.x + m_nKeyboardOffset) / 56; //TODO: replace 56 with PAGE_SIZE_PIXELS
	int nNearestOctaveOffset = (point.x + m_nKeyboardOffset) % 56;
	int key = 0;

	// Find key within Octave which contains nNearestOctaveOffset
	for(; key < 12 && nNearestOctaveOffset >= m_nKeyPos[key]; key++);

	BITMAP bmInfoR;
	int nStatus =  m_bmKeyBoard128.GetBitmap(&bmInfoR);

	// If clicked on the black key else play the white key
	// The black keys extend for 4 pixels more than half the height of the keyboard rect
	if((point.y - REGION_LAYER_AREA_HEIGHT) > (bmInfoR.bmHeight/2) + 4 && m_nRadius[key - 1] == 1)
	{
		if((point.x - nNearestOctaveOffset*56) > 2)
			key++;
		else
			key--;
	}
	
	// Index of the key hit on the keyboard
	int nLayer = GetGoodLayer(point);

	if(IsMemberOfRegion(nIndex) == NULL && nIndex <= 127)
	{
		// Don't want to insert regions in wrong layers
		if(nFlags & MK_CONTROL && m_nActiveLayer <= m_nNumberOfLayers)
		{	
			CRegion* pNewRegion = new CRegion(m_nActiveLayer, nIndex, nIndex); 
			pNewRegion->SetParentControl(this);
			m_lstRegions.AddTail(pNewRegion);

			m_pDraggedRegion = pNewRegion;

			FireNewRegion(m_nActiveLayer, pNewRegion->GetStartNote(), pNewRegion->GetEndNote());			
		
			CClientDC clientDC(this);
			DrawRegionRects(&clientDC);
			
			SetCapture();
			m_bHasCapture = true;
			m_ptLButtonDown = point;
			m_nStartEnd = RK_END;
			SetSizeCursor(true, false);
			
			InvalidateControl();
			return;
		}
	}

	if(m_pCurrentRegion)
		FireRegionSelectedChanged(m_pCurrentRegion->GetLayer(), m_pCurrentRegion->GetStartNote());
	
	SetFocus();
	InvalidateControl();
}

int CRegionKeyboardCtrl::GetNotePlayed(CPoint point)
{
	int nNumOctaves = (point.x + m_nKeyboardOffset) / 56; 
	int nNearestOctaveOffset = (point.x + m_nKeyboardOffset) % 56;
	int key = 0;

	// Find key within Octave which contains nNearestOctaveOffset
	for(; key < 12 && nNearestOctaveOffset >= m_nKeyPos[key]; key++);

	BITMAP bmInfoR;
	int nStatus =  m_bmKeyBoard128.GetBitmap(&bmInfoR);

	// If clicked on the black key else play the white key
	// The black keys extend for 4 pixels more than half the height of the keyboard rect
	if((point.y - REGION_LAYER_AREA_HEIGHT) > (bmInfoR.bmHeight/2) + 4 && m_nRadius[key - 1] == 1)
	{
		if((point.x - nNearestOctaveOffset*56) > 2)
			key++;
		else
			key--;
	}

	int nNote = nNumOctaves * 12 + (key - 1); // This is the note Played

	return nNote;
}


void CRegionKeyboardCtrl::PlayNote(int nNote, bool bType)
{
	FireNotePlayed(nNote, bType);
	SetFocus();
}


void CRegionKeyboardCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	m_lastGhostRect.SetRectEmpty();

	if(m_bHasCapture)
	{
		ReleaseCapture();
		m_bHasCapture = false;

		int x = m_ptLButtonDown.x;
		int y = m_ptLButtonDown.y;

		m_ptLButtonDown.x = -1;
		m_ptLButtonDown.y = -1;

		// User simply clicked over selected region
		// No need to change anything so return
		if(point.x == x && point.y == y)
		{
			m_nStartEnd = RK_NULL;
			m_nMaxEndPixel = CRegionKeyboardCtrl::m_nRegionKeyboardSize;
			m_nMinEndPixel = 0; 
			if(m_bNoteIsOn == true)
			{
				int nLastNotePlayed = GetNotePlayed(m_ptLastNote);
				PlayNote(nLastNotePlayed, FALSE); // Note Off
				m_bNoteIsOn = false;
			}
			return;
		}

		// Clamp point
		point.x = point.x > m_nMaxEndPixel ? m_nMaxEndPixel : point.x;
		point.x = point.x < m_nMinEndPixel ? m_nMinEndPixel : point.x;

		if(m_nStartEnd == RK_END)
		{
			int nIndex = GetKeyIndexFromPoint(point); 
			nIndex = nIndex < 0 ? 0 : nIndex;
			nIndex = nIndex > 127 ? 127 : nIndex;

			// We don't want to let the user overlap regions in the same layer
			if(m_pDraggedRegion && CheckForCollision(m_pDraggedRegion, m_pDraggedRegion->GetLayer(), m_pDraggedRegion->GetStartNote(), nIndex))
			{
				ReleaseCapture();
				m_bHasCapture = false;
				SetNormalCursor();
				if(m_bNoteIsOn == true)
				{
					int nLastNotePlayed = GetNotePlayed(m_ptLastNote);
					PlayNote(nLastNotePlayed, FALSE); // Note Off
					m_bNoteIsOn = false;
				}
				return;
			}

			if(m_pDraggedRegion)
				m_pDraggedRegion->SetEndNote(nIndex);

			InvalidateControl();			
		}
		
		if(m_nStartEnd & RK_START)
		{
			int nIndex = GetKeyIndexFromPoint(point);
			nIndex = nIndex < 0 ? 0 : nIndex;

			// We don't want to let the user overlap regions in the same layer
			if(m_pDraggedRegion &&  CheckForCollision(m_pDraggedRegion, m_pDraggedRegion->GetLayer(), nIndex, m_pDraggedRegion->GetEndNote()))
			{
				ReleaseCapture();
				m_bHasCapture = false;
				SetNormalCursor();
				if(m_bNoteIsOn == true)
				{
					int nLastNotePlayed = GetNotePlayed(m_ptLastNote);
					PlayNote(nLastNotePlayed, FALSE); // Note Off
					m_bNoteIsOn = false;
				}

				return;
			}

			if(m_pDraggedRegion)
				m_pDraggedRegion->SetStartNote(nIndex);

			InvalidateControl();			
		}

		if(m_nStartEnd & RK_MOVE)
		{
			m_bHasCapture = false;
			SetMoveCursor(false);
		}

		if(m_nStartEnd & RK_COPY)
		{
			 CopySelectedRegionAtPoint(point);
			 InvalidateControl();
		}

		m_nStartEnd = RK_NULL;
		m_nMaxEndPixel = CRegionKeyboardCtrl::m_nRegionKeyboardSize;
		m_nMinEndPixel = 0; 
	} 

	if(m_bNoteIsOn == true)
	{
		int nLastNotePlayed = GetNotePlayed(m_ptLastNote);
		PlayNote(nLastNotePlayed, FALSE); // Note Off
		ReleaseCapture();
	}
	m_bNoteIsOn = false;
}

void CRegionKeyboardCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	
	// If we don't have the capture then just show the required cursors when the mouse moves
	if(m_bHasCapture == false)
	{
		CRegion* pRegion = GetRegionForPoint(point, false);
		if(pRegion == NULL)
		{
			SetNormalCursor();
		}
		else if(pRegion->IsPointOnStartBoundary(point)) 
		{
			SetSizeCursor(true, true);
		}
		else if(pRegion->IsPointOnEndBoundary(point)) 
		{
			SetSizeCursor(true, false);
		}
		else
		{
			SetMoveCursor(true);
		}
	} 

	if(m_bHasCapture)
	{
		// If we don't have a region to drag then how the hell did we get the capture??
		// Anyway let's make it right then....
		if(m_pDraggedRegion == NULL)
		{
			ReleaseCapture();
			m_bHasCapture = false;
			return;
		}

		// Index of the key hit on the keyboard
		int nIndex = GetKeyIndexFromPoint(point);
		nIndex = nIndex < 0 ? 0 : nIndex;
		nIndex = nIndex > 127 ? 127 : nIndex;

		 // It's the closest region in the direction you're trying to resize the region
		CRegion* pClosestRegion = NULL;
		
		// If we're moving the start note AND the index for moving is on the left side AND 
		// there IS a colliding region (maybe NULL) BUT it's not the dragged region * WHEW *
		// MUST be a better way of doing this but at this hour I like this a lot!!
		if((m_nStartEnd & RK_START) && nIndex <= m_pDraggedRegion->GetStartNote() && IsMemberOfRegion(nIndex) != m_pDraggedRegion)
		{
			// Get the region before the dragged region
			pClosestRegion = GetRegionBeforeStartNote(m_pDraggedRegion);

			// If there's a closest region then get a possible valid index  
			if(pClosestRegion)
			{
				int nPossibleValidIndex = pClosestRegion->GetEndNote() + 1;

				// If there wasn't a colliding region the see which index is better for us
				if(IsMemberOfRegion(nIndex) || (IsMemberOfRegion(nIndex) == NULL && nIndex < nPossibleValidIndex))
				{
					nIndex =  nPossibleValidIndex;
				}
			}
			else if(nIndex < 0)
			{
				nIndex = 0;
			}
		}
		// See if we're past the end note and if yes the do the needful
		if((m_nStartEnd & RK_START) && nIndex > m_pDraggedRegion->GetEndNote())
		{
			m_nStartEnd = RK_END;
			SetSizeCursor(true, false);
		}

		if((m_nStartEnd & RK_END) && nIndex >= m_pDraggedRegion->GetEndNote() && IsMemberOfRegion(nIndex) != m_pDraggedRegion)
		{
			pClosestRegion = GetRegionAfterEndNote(m_pDraggedRegion);
			if(pClosestRegion)
			{
				int nPossibleValidIndex = pClosestRegion->GetStartNote() - 1;
				if(IsMemberOfRegion(nIndex) || (IsMemberOfRegion(nIndex) == NULL && nIndex > nPossibleValidIndex))
				{
					nIndex =  nPossibleValidIndex;
				}
			}
			else if(nIndex > 127)
			{
				nIndex = 127;
			}
		}
		
		if((m_nStartEnd & RK_END) && nIndex < m_pDraggedRegion->GetStartNote())
		{
			m_nStartEnd = RK_START;
			SetSizeCursor(true, true);
		}


		// Now do the actual resizing if 
		if((m_nStartEnd & RK_END) && m_bEndCursorSet
			&& !CheckForCollision(m_pDraggedRegion, m_pDraggedRegion->GetLayer(), m_pDraggedRegion->GetStartNote(), nIndex) 
			&& nIndex != m_pDraggedRegion->GetEndNote())
		{
			// Turn off any notes 
			if(m_bNoteIsOn == true)
			{
				int nLastNotePlayed = GetNotePlayed(m_ptLastNote);
				PlayNote(nLastNotePlayed, FALSE); // Note Off
				m_bNoteIsOn = false;
			}

			m_pDraggedRegion->SetEndNote(nIndex);
			FireRangeChanged(m_pDraggedRegion->GetLayer(), m_pDraggedRegion->GetStartNote(), m_pDraggedRegion->GetEndNote());
			SetFocus();
			m_nStartEnd = RK_END;
		}

		if((m_nStartEnd & RK_START) && m_bStartCursorSet
			&& !CheckForCollision(m_pDraggedRegion, m_pDraggedRegion->GetLayer(), nIndex, m_pDraggedRegion->GetEndNote())
			&& nIndex != m_pDraggedRegion->GetStartNote())
		{
			// Turn off any notes 
			if(m_bNoteIsOn == true)
			{
				int nLastNotePlayed = GetNotePlayed(m_ptLastNote);
				PlayNote(nLastNotePlayed, FALSE); // Note Off
				m_bNoteIsOn = false;
			}

			m_pDraggedRegion->SetStartNote(nIndex);
			FireRangeChanged(m_pDraggedRegion->GetLayer(), m_pDraggedRegion->GetStartNote(), m_pDraggedRegion->GetEndNote());
			SetFocus();
			m_nStartEnd = RK_START;
		}

		if((m_nStartEnd & RK_MOVE))
		{
			int nMovedStartNote = nIndex - m_nDraggedKey;
			nMovedStartNote = (nMovedStartNote < 0) ? 0 : nMovedStartNote;
			
			int nMovedEndNote = nMovedStartNote + (m_pDraggedRegion->GetEndNote() - m_pDraggedRegion->GetStartNote());
			nMovedEndNote = (nMovedEndNote > 127) ? 127 : nMovedEndNote;
			
			nMovedStartNote = nMovedEndNote - (m_pDraggedRegion->GetEndNote() - m_pDraggedRegion->GetStartNote());
			
			int nMovedLayer = GetAbsoluteLayerFromPoint(point);
			int nOldLayer = m_pDraggedRegion->GetLayer();
			int nOldStartNote = m_pDraggedRegion->GetStartNote();
			
			if(nMovedLayer > m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS - 1)
			{
				FireScrollLayers(TRUE);
				return;
			}
			else if(nMovedLayer < m_nFirstVisibleLayer)
			{
				FireScrollLayers(FALSE);
				return;
			}
	
			if(nOldLayer != nMovedLayer || nOldStartNote != nMovedStartNote)
			{
				if(nMovedLayer < m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS 
					&& nMovedLayer >= m_nFirstVisibleLayer 
					&& nMovedStartNote >= 0 && nMovedEndNote <= 127
					&& !CheckForCollision(m_pDraggedRegion, nMovedLayer, nMovedStartNote, nMovedEndNote))
				{
					// Turn off any notes 
					if(m_bNoteIsOn == true)
					{
						int nLastNotePlayed = GetNotePlayed(m_ptLastNote);
						PlayNote(nLastNotePlayed, FALSE); // Note Off
						m_bNoteIsOn = false;
					}

					FireRegionMoved(nOldLayer, nOldStartNote, nMovedLayer, nMovedStartNote, nMovedEndNote);
					SetFocus();

					m_pDraggedRegion->SetNoteRange(nMovedStartNote, nMovedEndNote);
					m_pDraggedRegion->SetLayer(nMovedLayer);
					m_nActiveLayer = nMovedLayer;
				}
			}

			SelectRegion(m_pDraggedRegion->GetLayer(), m_pDraggedRegion->GetStartNote());
			UpdateRegionRect();
		}
		if(m_nStartEnd & RK_COPY)
		{
			int nStartIndex = nIndex - m_nDraggedKey;
			//nStartIndex = (nStartIndex < 0) ? 0 : nStartIndex;

			int nEndIndex = nStartIndex + (m_pDraggedRegion->GetEndNote() - m_pDraggedRegion->GetStartNote());
			//nEndIndex = nEndIndex > 127 ? 127 : nEndIndex;

			nStartIndex = nEndIndex - (m_pDraggedRegion->GetEndNote() - m_pDraggedRegion->GetStartNote());

			int nCopyLayer = GetLayerFromPoint(point);
			if(nStartIndex >= 0 && nEndIndex <= 127 && !CheckForCollision(m_pDraggedRegion, nCopyLayer, nStartIndex, nEndIndex, true) && nEndIndex <= 127)
			{
				SetCopyCursor(true);
			}
			else
			{
				SetNoDoCursor(true);
			}

			DrawGhostRect(m_pDraggedRegion, m_ptLButtonDown, point);
		}
	}
}


int CRegionKeyboardCtrl::GetKeyIndexFromPoint(CPoint point)
{
	//TODO: replace 56 with PAGE_SIZE_PIXELS
	int nNumOctaves = (point.x + m_nKeyboardOffset) / 56; 
	int nNearestOctaveOffset = (point.x + m_nKeyboardOffset) % 56;
	int key = 0;
	// Find key within Octave which contains nNearestOctaveOffset
	for(; key < 12 && nNearestOctaveOffset >= m_nKeyPos[key]; key++);
	
	// Index of the key hit on the keyboard
	return nNumOctaves * 12 + (key - 1); 
}

void CRegionKeyboardCtrl::DrawKeyboard(CDC* pDC, const CRect& rcBounds)
{
	ASSERT(pDC);

	CRect windowRect;
	CRect clientRect;

	BOOL bDCStatus;
	CDC bitmapDC;
	CDC regionsDC;

	bDCStatus = regionsDC.CreateCompatibleDC(pDC);		
	CBitmap* pOldRDCBM = regionsDC.SelectObject(&m_bmBlank128);

	BITMAP bmInfoR;
	int nStatus =  m_bmBlank128.GetBitmap(&bmInfoR);
	CRect rect(0, 0, bmInfoR.bmWidth, bmInfoR.bmHeight);
	m_RegionsRect = rect;
	
	regionsDC.FillSolidRect(&rect, ::GetSysColor(COLOR_WINDOW));
	DrawLayersOnRegionBitmap(&regionsDC, rect);
	
	DrawRegionRects(&regionsDC);
		
	bDCStatus = bitmapDC.CreateCompatibleDC(pDC);
	CBitmap* pOldBDCBM = bitmapDC.SelectObject(&m_bmKeyBoard128);
	BITMAP bmInfoKB;
	nStatus =  m_bmKeyBoard128.GetBitmap(&bmInfoKB);

	// color the background not covered by regions and keyboard
	CRect rcDiff;
	rect.SetRect(0, 0, bmInfoR.bmWidth, REGION_LAYER_AREA_HEIGHT+bmInfoKB.bmHeight);
	rcDiff.SubtractRect(rcBounds, rect);
	pDC->FillSolidRect(&rcDiff, ::GetSysColor(COLOR_BTNFACE));
	
	BOOL bStatus = pDC->BitBlt(0, REGION_LAYER_AREA_HEIGHT, bmInfoKB.bmWidth, bmInfoKB.bmHeight, &bitmapDC, m_nKeyboardOffset, 0, SRCCOPY);

    // Draw the circle denoting rootnote on the memory dc.
    DrawRootNote(pDC);
	
	bStatus = pDC->BitBlt(0,0, bmInfoR.bmWidth, REGION_LAYER_AREA_HEIGHT, &regionsDC, m_nKeyboardOffset, 0, SRCCOPY);

	if((m_nStartEnd & RK_COPY) && m_bHasCapture == true)
	{
		CPoint pt1, pt2;
		DrawGhostRect(NULL, pt1, pt2);
	}
	
	regionsDC.SelectObject(pOldRDCBM); 
	bitmapDC.SelectObject(pOldBDCBM); 
	
	bDCStatus = regionsDC.DeleteDC();
	bDCStatus = bitmapDC.DeleteDC();
}

// This draws the default number of layers in gray
void CRegionKeyboardCtrl::DrawLayersOnRegionBitmap(CDC* pDC, CRect& regionRect)
{
	ASSERT(pDC);

	regionRect.NormalizeRect();

	// Create and select the gray pen into the device context
	DWORD dwGrayColor = ::GetSysColor(COLOR_HIGHLIGHT);
	CPen grayPen(PS_DOT, 1, dwGrayColor);  
	CPen* pOldPen = pDC->SelectObject(&grayPen);
	
	int nLayerIndex = 0;
	
	while(nLayerIndex < NUMBER_OF_VISIBLE_LAYERS)
	{
		pDC->MoveTo(0, (nLayerIndex + 1) * REGION_LAYER_HEIGHT);
		pDC->LineTo(regionRect.Width(), (nLayerIndex + 1) * REGION_LAYER_HEIGHT);

		nLayerIndex++;
	}

	// Highlight the selected layer
	if(m_nActiveLayer != -1 && (m_nActiveLayer < m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS && m_nActiveLayer >= m_nFirstVisibleLayer))
	{
		pDC->FillSolidRect(0, (REGION_LAYER_AREA_HEIGHT - ((m_nActiveLayer - m_nFirstVisibleLayer + 1) * REGION_LAYER_HEIGHT - 1)), regionRect.Width(), REGION_LAYER_HEIGHT - 1, COLOR_ACTIVE_LAYER);
	}

	// Select the old pen back into the device context
	if(pOldPen)
		pDC->SelectObject(pOldPen);
}

void CRegionKeyboardCtrl::DrawRegionRects(CDC* pDC)
{
	int nCount = 0;
	int nStartPos = 0;
	int nEndPos = 0;
	
	ASSERT(pDC);

	// create label font if not already cached
	if (!m_bRegionFontCreated)
		{
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = -(REGION_LAYER_HEIGHT-2);
		strcpy(lf.lfFaceName, "Arial");
		VERIFY(m_fnRegion.CreateFontIndirect(&lf));
 		m_bRegionFontCreated = true;
		}

	// set up for all Regions to draw
	CFont* pfnSav = pDC->SelectObject(&m_fnRegion);
	COLORREF crSav = pDC->GetBkColor();

	// loop over regions
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		pRegion->Draw(pDC, REGION_LAYER_AREA_HEIGHT, m_nFirstVisibleLayer);
	}

	// cleanup
	pDC->SetBkColor(crSav);
	if (pfnSav)
		pDC->SelectObject(pfnSav);
}

void CRegionKeyboardCtrl::SetNormalCursor()
{
	if(m_bStartCursorSet || m_bEndCursorSet)
		SetSizeCursor(false, false);
	if(m_bMoveCursorSet)
		SetMoveCursor(false);
	if(m_bCopyCursorSet)
		SetCopyCursor(false);
	if(m_bNoDoCursorSet)
		SetNoDoCursor(false);
}

void CRegionKeyboardCtrl::SetNoDoCursor(bool bOn)
{
	if(bOn)
	{
		if(m_hNODOCursor)
		{
			m_bNoDoCursorSet = true;
			SetCursor(m_hNODOCursor);
		}
	}
	else
	{
		m_bNoDoCursorSet = false;
		SetCursor(m_hArrowCursor);
	}
}


		

void CRegionKeyboardCtrl::SetSizeCursor(bool bOn, bool bStart)
{
	if(bOn)
	{
		if(bStart)
		{
			m_bStartCursorSet = true;
			m_hSIZEWECursor = m_hStartEditCursor;
		}
		else
		{
			m_bEndCursorSet = true;
			m_hSIZEWECursor = m_hEndEditCursor;
		}


		SetCursor(m_hSIZEWECursor);
	}
	else
	{
		m_bStartCursorSet = false;
		m_bEndCursorSet = false;
		
		SetCursor(m_hArrowCursor);
	}
}

void CRegionKeyboardCtrl::SetMoveCursor(bool bOn)
{
	if(bOn)
	{
		if(m_hMOVECursor)
		{
			m_bMoveCursorSet = true;
			SetCursor(m_hMOVECursor);
		}
	}
	else
	{
		m_bMoveCursorSet = false;
		SetCursor(m_hArrowCursor);
	}
}

void CRegionKeyboardCtrl::SetCopyCursor(bool bOn)
{
	if(bOn)
	{
		if(m_hCOPYCursor)
		{
			m_bCopyCursorSet = true;
			SetCursor(m_hCOPYCursor);
		}
	}
	else
	{
		m_bCopyCursorSet = false;
		SetCursor(m_hArrowCursor);
	}
}



void CRegionKeyboardCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	InvalidateControl();

	COleControl::OnKillFocus(pNewWnd);
}

void CRegionKeyboardCtrl::InitializeKeyBoard(LPUNKNOWN RegionMap)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	ASSERT(RegionMap);

	InvalidateControl();
}


void CRegionKeyboardCtrl::DeleteAllRegions()
{
	CRegion* pRegion;
	while(m_lstRegions.IsEmpty() == FALSE)
	{
		pRegion = m_lstRegions.RemoveHead();
		delete pRegion;
	}
}

void CRegionKeyboardCtrl::SetCurrentRegion(short nLayer, short nStartNote) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	
	SelectRegion(nLayer, nStartNote);

	InvalidateControl();
}


// ===========================================================================
// DrawRootNote
//  Draws a circle on the dc passed in to denote that this key is a rootnote.
// ===========================================================================
void CRegionKeyboardCtrl::DrawRootNote(CDC* pdc)
{
    ASSERT(pdc);
    int nKey;

	nKey = GetRootNoteFromActiveRegion();

    if (nKey == 128)
    {
        //ASSERT(false);  // The rootnote has not been set! This is an error.
        return;			// Actually for a new collection without a wave it should be OK
    }					// We will have to be alert for other problems associated
						// with no wave/root note.

    // Get which octave this is in.
    int nOctave = nKey/12;
    nKey = nKey % 12;   // which key in the octave.

    int nXOffset = nOctave * 56;

    // draw the circle.
    int x1, y1;
    if (m_nRadius[nKey] == 1)   // it's a black key. Draw a white circle.
    {
        CBrush * pOldBrush = pdc->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));      
        if (pOldBrush)
        {
            x1 = nXOffset + m_nKeyPos[nKey] - m_nKeyboardOffset; 
            y1 = REGION_LAYER_AREA_HEIGHT + 12; // 12 is about 4 lower than center of black keys
            pdc->Ellipse(x1+1, y1,
                        x1 + 5, // Width of 5 works better to draw a white circle.
                        y1 + 5  // Use 5 so that ht = width.
                        );
            pdc->SelectObject(pOldBrush);
        }
    }
    else    //it's a white key. Draw a black circle.
    {
        CBrush * pOldBrush = pdc->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));        
        if (pOldBrush)
        {
            x1 = nXOffset + m_nKeyCenter[nKey] - 2 - m_nKeyboardOffset; // 2 is arbitrary - for aesthetic reasons.
            y1 = REGION_LAYER_AREA_HEIGHT + 24, //y1 24 is the height at which we draw for white keys

            pdc->Ellipse(x1, y1, 
                         x1 + 4, // we want a dia of 4 for the circle.
                         y1 + 4  //since radius is 2. Dia = 4
                         );

            pdc->SelectObject(pOldBrush);
        }
    }

	// Redraw the MIDI note that was on before the invalidate came in...if it's still on that is...
	for(int nIndex = 0; nIndex < 128; nIndex++)
	{
		if(m_nLastMidiNoteOn[nIndex] == 1)
		{
			MidiEvent(nIndex, MIDI_NOTEON, 0);
		}
	}
}

void CRegionKeyboardCtrl::MidiEvent(short nMidiNote, short nNoteFlag, short nVelocity) 
{
	CClientDC		dcClient(this);
    int				nKey;
	CBrush*			pOldBrush = NULL;
	CBrush			cRedBrush;
	CPen			cRedWhitePen;
	CPen*			pOldPen = NULL;

	if(nNoteFlag == MIDI_NOTEON)
	{
		m_nLastMidiNoteOn[nMidiNote] = 1;
	}
	else
	{
		m_nLastMidiNoteOn[nMidiNote] = 0;
	}


    // Get which octave this is in.
    int nOctave = nMidiNote/12;
    nKey = nMidiNote % 12;   // which key in the octave.

    int nXOffset = nOctave * 56;

    // draw the circle.
    int x1, y1;
    if (m_nRadius[nKey] == 1)   // it's a black key. Draw a red circle.
    {
		if (nNoteFlag == MIDI_NOTEON)
		{
			cRedBrush.CreateSolidBrush(COLOR_LAYER_SELECT);
			pOldBrush = dcClient.SelectObject(&cRedBrush);
		}
		else if (nNoteFlag == MIDI_NOTEOFF)
		{
			int nRootKey = GetRootNoteFromActiveRegion();
			if((nOctave*12 + nKey) != nRootKey)
				pOldBrush = dcClient.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
			else
				pOldBrush = dcClient.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));

		}
        if (pOldBrush)
        {
            x1 = nXOffset + m_nKeyPos[nKey] - m_nKeyboardOffset; 
            y1 = REGION_LAYER_AREA_HEIGHT + 12; // 12 is about 4 lower than center of black keys
            dcClient.Ellipse(x1+1, y1,
                        x1 + 5, // Width of 5 works better to draw a white circle.
                        y1 + 5  // Use 5 so that ht = width.
                        );
            dcClient.SelectObject(pOldBrush);
        }
    }
    else    //it's a white key. Draw a red circle.
    {
		if (nNoteFlag == MIDI_NOTEON)
		{
			cRedBrush.CreateSolidBrush(COLOR_LAYER_SELECT);
			cRedWhitePen.CreatePen(PS_SOLID, 0, COLOR_LAYER_SELECT);
			pOldBrush = dcClient.SelectObject(&cRedBrush);
			pOldPen = dcClient.SelectObject(&cRedWhitePen);
			if (pOldBrush)
			{
				x1 = nXOffset + m_nKeyCenter[nKey] - 2 - m_nKeyboardOffset; // 2 is arbitrary - for aesthetic reasons.
				y1 = REGION_LAYER_AREA_HEIGHT + 24, //y1 24 is the height at which we draw for white keys

				dcClient.Ellipse(x1, y1, 
							 x1 + 4, // we want a dia of 4 for the circle.
							 y1 + 4  //since radius is 2. Dia = 4
							 );

				dcClient.SelectObject(pOldBrush);
				dcClient.SelectObject(pOldPen);
			}
		}
		else if (nNoteFlag == MIDI_NOTEOFF)
		{
			CRgn			crClipRgn;
			CRect			rcBounds;
			GetClientRect(&rcBounds);

			x1 = nXOffset + m_nKeyPos[nKey] - m_nKeyboardOffset;

			crClipRgn.CreateRectRgn(x1, 22, x1 + 6, REGION_LAYER_AREA_HEIGHT + 35);
			dcClient.SelectClipRgn(&crClipRgn);
			DrawKeyboard(&dcClient, rcBounds);
		}
    }

	CPtrList lstPlayingRegions;
	MarkPlayingRegions(nMidiNote, nNoteFlag, nVelocity, &lstPlayingRegions);
}


CRegion* CRegionKeyboardCtrl::IsMemberOfRegion(int nKey)
{
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		if(pRegion->IsNoteInRegion(m_nActiveLayer, nKey) == TRUE)
			return pRegion;
	}
	return NULL;
}

CRegion* CRegionKeyboardCtrl::GetRegionForPoint(CPoint point, bool bIgnoreY)
{
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		if(pRegion->IsPointInRegion(point, bIgnoreY) == TRUE)
		{
			// Check for the layer now
			int nLayer = pRegion->GetLayer();
			if(nLayer >= m_nFirstVisibleLayer && nLayer <= (m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS))
			{
				return pRegion;
			}
		}
	}
	return NULL;
}


// Gets a good layer to draw this region
int CRegionKeyboardCtrl::GetGoodLayer(CPoint point)
{
	return 0;
}

// Selects all regions containing this note
void CRegionKeyboardCtrl::SelectAllRegions(int nNote)
{
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		if(pRegion->IsNoteInRegion(nNote) == TRUE)
			pRegion->SetSelected(TRUE);
		else
			pRegion->SetSelected(FALSE);
	}
}

// Selects the region containing this point; Deselcts all other regions
void CRegionKeyboardCtrl::SelectRegion(CPoint point)
{
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		if(pRegion->IsPointInRegion(point) == TRUE)
		{
			pRegion->SetSelected(TRUE);
			m_pCurrentRegion = pRegion;
		}
		else
		{
			pRegion->SetSelected(FALSE);
		}
	}
}

// Layer and StartNote uniquely identify a region since there can be no overlap of regions in the same layer
void CRegionKeyboardCtrl::SelectRegion(int nLayer, int nStartNote)
{
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		if(pRegion->IsNoteInRegion(nLayer, nStartNote) == TRUE)
		{
			pRegion->SetSelected(TRUE);
			m_pCurrentRegion = pRegion;

			// Scroll to the layer if it's not visible
			m_nFirstVisibleLayer = nLayer < m_nFirstVisibleLayer ? nLayer : m_nFirstVisibleLayer;
			m_nFirstVisibleLayer = nLayer > (m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS) ? 1 + (nLayer - NUMBER_OF_VISIBLE_LAYERS) : m_nFirstVisibleLayer;
			
			m_nActiveLayer = nLayer;
		}
		else
		{
			pRegion->SetSelected(FALSE);
		}
	}
}

int CRegionKeyboardCtrl::GetRootNoteFromActiveRegion()
{
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		if(pRegion->IsSelected())
			return pRegion->GetRootNote();
	}

	return 128;
}

void CRegionKeyboardCtrl::SetActiveLayer(CPoint point)
{
	int nActiveLayer = m_nFirstVisibleLayer + (REGION_LAYER_AREA_HEIGHT - point.y)/ REGION_LAYER_HEIGHT;

	if(nActiveLayer < 0)
	{
		//m_nActiveLayer = 0;
		return;
	}
	
	m_nActiveLayer = nActiveLayer;

	if(m_nActiveLayer >= m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS)
	{
		m_nActiveLayer = m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS - 1;
		return;
	}

	InvalidateRect(NULL);
	//UpdateRegionRect();

	FireActiveLayerChanged(m_nActiveLayer);
}

void CRegionKeyboardCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	ScrollLayers(nSBCode, nPos);
}


void CRegionKeyboardCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
		/*case VK_INSERT:
		{
			m_nNumberOfLayers++;
			UpdateRegionRect();
			InvalidateControl();

			break;
		}*/
		case VK_DELETE:
		{
			if(m_pCurrentRegion)
			{
				FireRegionDeleted(m_pCurrentRegion->GetLayer(), m_pCurrentRegion->GetStartNote());
			}

			SetFocus();
			UpdateRegionRect();
			InvalidateControl();
			break;
		}
	}

}

// Adds a region, creates a new layer if necessary (if there's already a region containing some notes)
// Returns the layer in which this region was added...
int CRegionKeyboardCtrl::_InsertNewRegion(int nStartNote, int nEndNote)
{
	return _InsertNewRegion(m_nActiveLayer, nStartNote, nEndNote, "");
}

int CRegionKeyboardCtrl::_InsertNewRegion(int nLayer, int nStartNote, int nEndNote, LPCTSTR pszWaveName)
{
	if(nLayer < 0)
		nLayer = 0;

	int nInsertionLayer = nLayer;
	
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		if(pRegion->IsOverlappingRegion(nInsertionLayer, nStartNote, nEndNote))
		{
			// We found a region in this layer that overlaps the region between the start and end notes
			// So try a different layer...
			// Start from the first region again...
			nInsertionLayer++;
			position = m_lstRegions.GetHeadPosition();
		}
	}

	// Add a new layer if need be...
	if(nInsertionLayer >= m_nNumberOfLayers)
		m_nNumberOfLayers = nInsertionLayer + 1;

	// Add the region to this layer
	CRegion* pNewRegion = new CRegion(nInsertionLayer, nStartNote, nEndNote); 
	pNewRegion->SetParentControl(this);
	pNewRegion->SetWaveName(pszWaveName);
	m_lstRegions.AddTail(pNewRegion);

	// Set the inserted layer as the currently active layer and visible layer 
	// redraw the regions too...
	if(nInsertionLayer > m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS || nInsertionLayer < m_nFirstVisibleLayer)
		m_nFirstVisibleLayer = nInsertionLayer;

	m_nActiveLayer = nInsertionLayer;
	m_pCurrentRegion = pNewRegion;
	
	UpdateRegionRect();

	return nInsertionLayer;
}


void CRegionKeyboardCtrl::UpdateRegionRect()
{
	CClientDC clientDC(this);
	DrawLayersOnRegionBitmap(&clientDC, m_RegionsRect);
	DrawRegionRects(&clientDC);
}

// Sets the root note for the current region
void CRegionKeyboardCtrl::SetRootNote(short nNote)
{
	ASSERT(m_pCurrentRegion);

	m_pCurrentRegion->SetRootNote(nNote);
	InvalidateControl();
}

BOOL CRegionKeyboardCtrl::SetRange(short nStartNote, short nEndNote, short nStartVelocity, short nEndVelocity)
{
	ASSERT(m_pCurrentRegion);
	if(m_pCurrentRegion == NULL)
		return FALSE;

	if(nStartNote > nEndNote)
		return FALSE;
	if(nStartVelocity > nEndVelocity)
		return FALSE;

	if(CheckForCollision(m_pCurrentRegion, m_pCurrentRegion->GetLayer(), nStartNote, nEndNote))
		return FALSE;

	m_pCurrentRegion->SetNoteRange(nStartNote, nEndNote);
	m_pCurrentRegion->SetVelocityRange(nStartVelocity, nEndVelocity);

	UpdateRegionRect();

	return TRUE;
}


// Returns the colliding region for this layer and range 
// bCheckSelfCollision = false allows you to look for a colliding region that is NOT the current layer
// If bCheckSelfCollision == true; then it'll return a valid pointer for self collision 
// This is used in the region CTRL+Drag->Copy functionality where you shouldn't be able to copy regions over itself!
CRegion* CRegionKeyboardCtrl::CheckForCollision(CRegion* pCheckedRegion, int nLayer, int nStartNote,int nEndNote, bool bCheckSelfCollision)
{
	ASSERT(pCheckedRegion);
	if(m_pCurrentRegion == NULL)
	{
		return NULL;
	}

	CRegion* pRegion  = NULL;
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		pRegion = m_lstRegions.GetNext(position);
		if(pRegion->IsOverlappingRegion(nLayer, nStartNote, nEndNote))
		{
			if(bCheckSelfCollision) 
				return pRegion;
			else if(pRegion != pCheckedRegion)
				return pRegion;
		}
			
	}

	return NULL;
}


short CRegionKeyboardCtrl::InsertNewRegion(short nStartNote, short nEndNote, short nStartVelocity, short nEndVelocity) 
{
	return _InsertNewRegion(nStartNote, nEndNote);
}


short CRegionKeyboardCtrl::InsertRegion(short nLayer, short nStartNote, short nEndNote, short nStartVelocity, short nEndVelocity, LPCTSTR pszWaveName)
{
	return _InsertNewRegion(nLayer, nStartNote, nEndNote, pszWaveName);
}

void CRegionKeyboardCtrl::DeleteRegion(short nLayer, short nStartNote) 
{
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		POSITION deletePosition = position; 
		CRegion* pRegion = m_lstRegions.GetNext(position);
		if(pRegion->GetLayer() == nLayer && nStartNote == pRegion->GetStartNote())
		{
			pRegion = m_lstRegions.GetAt(deletePosition);
			m_lstRegions.RemoveAt(deletePosition);
			delete pRegion;
			pRegion = NULL;
			break;
		}
	}
	
	UpdateRegionRect();
}


int CRegionKeyboardCtrl::GetLayerFromPoint(CPoint point)
{
	int nLayer = m_nFirstVisibleLayer + (REGION_LAYER_AREA_HEIGHT - point.y)/ REGION_LAYER_HEIGHT;
	if(nLayer > 0 && nLayer < m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS)
	{
		return nLayer;
	}
	else if(nLayer >= m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS)
	{
		return m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS - 1;
	}
	else
	{
		return 0; // Can't have negative layers
	}
}

int CRegionKeyboardCtrl::GetAbsoluteLayerFromPoint(CPoint point)
{
	// Doen't massage the data
	int nLayer = m_nFirstVisibleLayer + (REGION_LAYER_AREA_HEIGHT - point.y)/ REGION_LAYER_HEIGHT;
	return nLayer;
}


int CRegionKeyboardCtrl::GetActiveLayer()
{
	return m_nActiveLayer;
}

// Assumption is that we're copying the m_pCurrentRegion...
// Used by the Ctrl + Drag -> Copy functionality
void CRegionKeyboardCtrl::CopySelectedRegionAtPoint(CPoint point)
{
	ASSERT(m_pCurrentRegion);
	if(m_pCurrentRegion == NULL)
	{
		return;
	}

	int nCopyLayer = GetLayerFromPoint(point);
	int nStartIndex = GetKeyIndexFromPoint(point) - m_nDraggedKey;
	nStartIndex = nStartIndex < 0 ? 0 : nStartIndex;
	int nEndIndex = nStartIndex;

	m_bHasCapture = false;
	SetCopyCursor(false);

	if(m_pCurrentRegion)
	{
		nEndIndex = nStartIndex + (m_pCurrentRegion->GetEndNote() - m_pCurrentRegion->GetStartNote());
		nEndIndex = nEndIndex > 127 ? 127 : nEndIndex;
	}

	if(!CheckForCollision(m_pCurrentRegion, nCopyLayer, nStartIndex, nEndIndex, true) && nEndIndex <= 127)
	{
		CRegion* pNewRegion = new CRegion(nCopyLayer, nStartIndex, nEndIndex); 
		if(pNewRegion == NULL)
		{
			return;
		}

		pNewRegion->SetParentControl(this);
		m_lstRegions.AddTail(pNewRegion);

		int nSourceLayer = m_pCurrentRegion->GetLayer();
		int nSourceStartNote = m_pCurrentRegion->GetStartNote();

		FireCopyRegion(nSourceLayer, nSourceStartNote, nCopyLayer, pNewRegion->GetStartNote());

		// Set the new Region as currently selected
		//SetCurrentRegion(nCopyLayer, pNewRegion->GetStartNote());
	
		CClientDC clientDC(this);
		DrawRegionRects(&clientDC);
	}
}


CRegion* CRegionKeyboardCtrl::GetRegionBeforeStartNote(CRegion* pRegion)
{
	int nStartNote = pRegion->GetStartNote();
	int nIndex = nStartNote;
	CRegion* pCollidingRegion = NULL;
	
	while(nIndex >= 0)
	{
		pCollidingRegion = IsMemberOfRegion(nIndex);
		if(pCollidingRegion && pCollidingRegion != pRegion)
			break;
		nIndex--;
	}

	if(nIndex <= 0)
		return NULL;
	else
		return pCollidingRegion;
}

CRegion* CRegionKeyboardCtrl::GetRegionAfterEndNote(CRegion* pRegion)
{
	int nEndNote = pRegion->GetEndNote();
	int nIndex = nEndNote;
	CRegion* pCollidingRegion = NULL;
	
	while(nIndex <= 127)
	{
		pCollidingRegion = IsMemberOfRegion(nIndex);
		if(pCollidingRegion && pCollidingRegion != pRegion)
			break;
		nIndex++;
	}

	if(nIndex >= 127)
		return NULL;
	else
		return pCollidingRegion;
}

void CRegionKeyboardCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	SetActiveLayer(point);

	// Index of the key hit on the keyboard
	int nIndex = GetKeyIndexFromPoint(point);
	nIndex = nIndex < 0 ? 0 : nIndex;
	nIndex = nIndex > 127 ? 127 : nIndex;

	CRegion* pRegion = GetRegionForPoint(point, false);
	if(pRegion && pRegion->IsSelected() == FALSE)
	{
		pRegion->SetSelected(true);
		m_pCurrentRegion = pRegion;
		FireRegionSelectedChanged(m_pCurrentRegion->GetLayer(), m_pCurrentRegion->GetStartNote());
		SetFocus();
		CClientDC clientDC(this);
		DrawRegionRects(&clientDC);
	}
	
	COleControl::OnRButtonDown(nFlags, point);
}

void CRegionKeyboardCtrl::ReleaseMouseCapture() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	

	ReleaseCapture();
	m_bHasCapture = false;
	SetNormalCursor();
}

void CRegionKeyboardCtrl::TurnOffMidiNotes()
{
	bool bInvalidate = false;
	// Redraw the keyboard with all notes marked as off
	for(int nIndex = 0; nIndex < 128; nIndex++)
	{
		if(m_nLastMidiNoteOn[nIndex] == 1)
		{
			bInvalidate = true;
			m_nLastMidiNoteOn[nIndex] = 0;
		}
	}

	if(bInvalidate)
		InvalidateControl();
}


void CRegionKeyboardCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	COleControl::OnSetFocus(pOldWnd);
	
	InvalidateControl();
}

short CRegionKeyboardCtrl::SetCurrentLayer(short nLayer) 
{
	m_nActiveLayer = SetFirstVisibleLayer(nLayer);
	return m_nActiveLayer;
}

short CRegionKeyboardCtrl::SetFirstVisibleLayer(short nLayer) 
{
	if(nLayer == -1)
	{
		return m_nFirstVisibleLayer++;
	}

	if(nLayer >= m_nNumberOfLayers)
	{
		nLayer = m_nNumberOfLayers - 1;
	}

	if((nLayer + 4) < m_nNumberOfLayers)
	{
		m_nFirstVisibleLayer = nLayer;
	}
	else
	{
		m_nFirstVisibleLayer = m_nNumberOfLayers - abs(m_nNumberOfLayers - nLayer);
	}

	return m_nFirstVisibleLayer;
}

short CRegionKeyboardCtrl::ScrollLayers(short nSBCode, short nCurPos) 
{
	switch(nSBCode)
	{
		case SB_LINEDOWN:
		{
			if(m_nFirstVisibleLayer > 0)
			{
				m_nFirstVisibleLayer--;
			}
			break;
		}

		case SB_LINEUP:
		{
			if((m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS) < m_nNumberOfLayers)
			{
				m_nFirstVisibleLayer++;
			}
			break;
		}

		case SB_PAGEDOWN:
		{
			m_nFirstVisibleLayer = (m_nFirstVisibleLayer - NUMBER_OF_VISIBLE_LAYERS);
			m_nFirstVisibleLayer = m_nFirstVisibleLayer < 0 ? 0 : m_nFirstVisibleLayer;
			break;
		}

		case SB_PAGEUP:
		{
			m_nFirstVisibleLayer = (m_nFirstVisibleLayer + NUMBER_OF_VISIBLE_LAYERS);
			m_nFirstVisibleLayer = m_nFirstVisibleLayer > (m_nNumberOfLayers - NUMBER_OF_VISIBLE_LAYERS) ? (m_nNumberOfLayers - NUMBER_OF_VISIBLE_LAYERS) - 1 : m_nFirstVisibleLayer;
			break;
		}

		case SB_TOP:
		{
			m_nFirstVisibleLayer = m_nNumberOfLayers - NUMBER_OF_VISIBLE_LAYERS;
			break;
		}

		case SB_BOTTOM:
		{
			m_nFirstVisibleLayer = 0;
			break;
		}

		case SB_THUMBTRACK:
		{
			if(nCurPos < m_nNumberOfLayers && nCurPos >= 0)
			{
				m_nFirstVisibleLayer = (m_nNumberOfLayers - (nCurPos + 3));
			}
			break;
		}
	}
	
	m_nFirstVisibleLayer = m_nFirstVisibleLayer < 0 ? 0 : m_nFirstVisibleLayer;
	InvalidateRect(NULL);

	return m_nFirstVisibleLayer;
}

short CRegionKeyboardCtrl::GetNumberOfLayers() 
{
	return m_nNumberOfLayers;
}

short CRegionKeyboardCtrl::GetFirstVisibleLayer() 
{
	return m_nFirstVisibleLayer;
}

short CRegionKeyboardCtrl::AddNewLayer() 
{
	m_nNumberOfLayers++;
	
	// Bump up the regions
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		ASSERT(pRegion);
		if(pRegion)
		{
			USHORT usLayer = pRegion->GetLayer();
			if(usLayer >= m_nActiveLayer)
			{
				pRegion->SetLayer(usLayer + 1);
			}
		}
	}

	UpdateRegionRect();
	InvalidateControl();

	return m_nNumberOfLayers;
}

short CRegionKeyboardCtrl::DeleteActiveLayer() 
{
	if(m_nNumberOfLayers > NUMBER_OF_VISIBLE_LAYERS)
	{
		m_nNumberOfLayers--;
	}

	// Bump up the regions
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		ASSERT(pRegion);
		if(pRegion)
		{
			USHORT usLayer = pRegion->GetLayer();
			if(usLayer >= m_nActiveLayer)
			{
				pRegion->SetLayer(usLayer -  1);
			}
		}
	}


	SetFocus();
	UpdateRegionRect();
	InvalidateControl();

	return m_nNumberOfLayers;
}

void CRegionKeyboardCtrl::SetAuditionMode(BOOL bAuditionMode) 
{
	m_bAuditionMode = bAuditionMode;
}

void CRegionKeyboardCtrl::MarkPlayingRegions(short nNote, short nNoteFlag, short nVelocity, CPtrList* plstPlayingRegions)
{
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		ASSERT(pRegion);
		BOOL bRegionIsPlaying = FALSE;
		if(m_bAuditionMode == AUDITION_MULTIPLE)
		{
			bRegionIsPlaying = pRegion->IsNoteInRegion(m_nActiveLayer, nNote, nVelocity, true);
		}
		else if(m_bAuditionMode == AUDITION_SOLO)
		{
			bRegionIsPlaying = pRegion->IsNoteInRegion(m_nActiveLayer, nNote, nVelocity, false);
		}

		if(bRegionIsPlaying == TRUE)
		{
			if(nNoteFlag == MIDI_NOTEON)
			{
				pRegion->SetPlayState(TRUE);
				if(plstPlayingRegions)
				{
					plstPlayingRegions->AddTail(pRegion);
				}
			}
			else
			{
				pRegion->SetPlayState(FALSE);
			}
		}
	}

	CClientDC clientDC(this);
	DrawLayersOnRegionBitmap(&clientDC, m_RegionsRect);
	DrawRegionRects(&clientDC);
}

void CRegionKeyboardCtrl::EnableRegion(short nLayer, short nStartNote, BOOL bEnable) 
{
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		if(pRegion->IsNoteInRegion(nLayer, nStartNote) == TRUE)
		{
			pRegion->Enable(bEnable);
		}
	}
}

void CRegionKeyboardCtrl::SetNumberOfLayers(short nLayers) 
{
	ASSERT(nLayers >= NUMBER_OF_VISIBLE_LAYERS);
	if(nLayers < NUMBER_OF_VISIBLE_LAYERS)
	{
		nLayers = NUMBER_OF_VISIBLE_LAYERS;
	}

	m_nNumberOfLayers = nLayers;
}

void CRegionKeyboardCtrl::DrawGhostRect(CRegion* pRegion, CPoint ptPeg, CPoint ptDrag)
{
	CRect ghostRect;
	if(pRegion == NULL)
	{
		ghostRect = m_lastGhostRect;
	}
	else
	{
		InvalidateRect(&m_lastGhostRect);

		CRect regionRect = pRegion->GetRect();
		int regionWidth = regionRect.Width();
		int regionHeight = regionRect.Height();
		int nXOffset = ptPeg.x - regionRect.left;
		int nGhostLeft = ptDrag.x - nXOffset;
		int nGhostTop = ptDrag.y - (regionHeight / 2);
		ghostRect.SetRect(nGhostLeft, nGhostTop, nGhostLeft + regionWidth, nGhostTop + regionHeight);
	}

	
	CPen dotPen(PS_DOT, 1, RGB(0, 0, 0));
	CClientDC clientDC(this);
	CPen* pOldPen = clientDC.SelectObject(&dotPen);
	clientDC.Rectangle(&ghostRect);
	if(pOldPen)
	{
		clientDC.SelectObject(pOldPen);
	} 
	
	CBrush hatchBrush(HS_BDIAGONAL, RGB(64, 64, 64));
	CRect hatchRect = ghostRect;
	hatchRect.DeflateRect(1, 1, 1, 1);
	clientDC.FillRect(&hatchRect, &hatchBrush);
	m_lastGhostRect = ghostRect;
}

void CRegionKeyboardCtrl::SetWaveName(short nLayer, short nStartNote, LPCTSTR pszWaveName) 
{
	POSITION position = m_lstRegions.GetHeadPosition();
	while(position)
	{
		CRegion* pRegion = m_lstRegions.GetNext(position);
		if(pRegion->IsNoteInRegion(nLayer, nStartNote) == TRUE)
		{
			pRegion->SetWaveName(pszWaveName);
			break;
		}
	}
}
