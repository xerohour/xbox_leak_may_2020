// PropPianoRoll.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MIDIStripMgr.h"
#include "MIDIMgr.h"
#include "PropPianoRoll.h"
#include "PropPageMgr.h"
#include "PartLengthDlg.h"
#include "TimeSignatureDlg.h"
#include "Timeline.h"
#include <ioDMStyle.h>
#include <PChannelName.h>
#include "MIDIStripMgrApp.h"
#include "TabPatternPattern.h"
#include "TrackFlagsPPG.h"
#include "GroupBitsPPG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static const char MidiValueToName[61] = "C    C#/DbD    D#/EbE    F    F#/GbG    G#/AbA    A#/BbB    ";


/////////////////////////////////////////////////////////////////////////////
// CPianoRollPropPageMgr property page

short CPianoRollPropPageMgr::sm_nActiveTab = 3;

CPianoRollPropPageMgr::CPianoRollPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pPropPianoRoll = NULL;
	m_fShowPatternPropPage = FALSE;
	m_pTabPatternPattern = NULL;
	m_pTrackFlagsPPG = NULL;
	m_pGroupBitsPPG = NULL;
	CStaticPropPageManager::CStaticPropPageManager();
}

CPianoRollPropPageMgr::~CPianoRollPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPianoRoll )
	{
		delete m_pPropPianoRoll;
		m_pPropPianoRoll = NULL;
	}
	if( m_pTabPatternPattern )
	{
		delete m_pTabPatternPattern;
		m_pTabPatternPattern = NULL;
	}
	if( m_pTrackFlagsPPG )
	{
		delete m_pTrackFlagsPPG;
		m_pTrackFlagsPPG = NULL;
	}
	if( m_pGroupBitsPPG )
	{
		delete m_pGroupBitsPPG;
		m_pGroupBitsPPG = NULL;
	}
	CStaticPropPageManager::~CStaticPropPageManager();
}

HRESULT STDMETHODCALLTYPE CPianoRollPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return CStaticPropPageManager::QueryInterface( riid, ppv );
}

HRESULT STDMETHODCALLTYPE CPianoRollPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
	BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	// Get "Part" text
	CString strPart;
	strPart.LoadString( m_fShowPatternPropPage ? IDS_PROPPAGE_PATTERN_TRACK : IDS_PROPPAGE_PART );

	// Format title
	CString strTitle = strPart;
	
	if( !m_fShowPatternPropPage
	&&	m_pIPropPageObject )
	{
		CPianoRollStrip* pPianoRollStrip = (CPianoRollStrip *)m_pIPropPageObject;

		if( pPianoRollStrip 
		&&  pPianoRollStrip->m_pMIDIMgr
		&&  pPianoRollStrip->m_pPartRef
		&&  pPianoRollStrip->m_pPartRef->m_pPattern )
		{
			CString strStyleName;
			BSTR bstrStyleName;

			// Get Style Name
			if( pPianoRollStrip->m_pMIDIMgr->m_pIStyleNode )
			{
				if( SUCCEEDED ( pPianoRollStrip->m_pMIDIMgr->m_pIStyleNode->GetNodeName( &bstrStyleName ) ) )
				{
					strStyleName = bstrStyleName;
					::SysFreeString( bstrStyleName );
				}
			}

			// Put together the title
			if( strStyleName.IsEmpty() == FALSE )
			{
				strTitle = strStyleName +
						   _T(" - " );
			}
			strTitle += pPianoRollStrip->m_pPartRef->m_pPattern->m_strName +
					    _T(" - ") +
					    pPianoRollStrip->m_pPartRef->m_strName +
					    _T(" ") +
					    strPart;
		}
	}

	*pbstrTitle = strTitle.AllocSysString();
	*pfAddPropertiesText = TRUE;
		
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CPianoRollPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
	LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	// Add group bits and pattern tab, if necessary
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;
	if( m_fShowPatternPropPage )
	{
		if( NULL == m_pGroupBitsPPG )
		{
			m_pGroupBitsPPG = new CGroupBitsPPG();
		}
		if( m_pGroupBitsPPG )
		{
			hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pGroupBitsPPG->m_psp );
			if( hPage )
			{
				hPropSheetPage[nNbrPages] = (LONG *)hPage;
				nNbrPages++;
			}			

			// Weak reference
			m_pGroupBitsPPG->m_pIPropSheet = m_pIPropSheet;

			// Tell the property page to update the active tab setting
			CGroupBitsPPG::sm_pnActiveTab = &CPianoRollPropPageMgr::sm_nActiveTab;
		}

		if( NULL == m_pTrackFlagsPPG )
		{
			m_pTrackFlagsPPG = new CTrackFlagsPPG();
		}
		if( m_pTrackFlagsPPG )
		{
			hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTrackFlagsPPG->m_psp );
			if( hPage )
			{
				hPropSheetPage[nNbrPages] = (LONG *)hPage;
				nNbrPages++;
			}			

			// Weak reference
			m_pTrackFlagsPPG->m_pIPropSheet = m_pIPropSheet;

			// Tell the property page to update the active tab setting
			CTrackFlagsPPG::sm_pnActiveTab = &CPianoRollPropPageMgr::sm_nActiveTab;
		}

		if( NULL == m_pTabPatternPattern )
		{
			m_pTabPatternPattern = new CTabPatternPattern();
		}
		if( m_pTabPatternPattern )
		{
			hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabPatternPattern->m_psp );
			if( hPage )
			{
				hPropSheetPage[nNbrPages] = (LONG *)hPage;
				nNbrPages++;
			}			

			// Weak reference
			m_pTabPatternPattern->m_pIPropSheet = m_pIPropSheet;

			// Tell the property page to update the active tab setting
			CTabPatternPattern::sm_pnActiveTab = &CPianoRollPropPageMgr::sm_nActiveTab;
		}
	}

	// Add Part tab
	if( NULL == m_pPropPianoRoll )
	{
		m_pPropPianoRoll = new PropPianoRoll();
	}
	if( m_pPropPianoRoll )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pPropPianoRoll->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			

		// Weak reference
		m_pPropPianoRoll->m_pIPropSheet = m_pIPropSheet;

		// Tell the property page to update the active tab setting
		PropPianoRoll::sm_pnActiveTab = &CPianoRollPropPageMgr::sm_nActiveTab;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CPianoRollPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pGroupBitsPPG )
	{
		m_pGroupBitsPPG->RefreshData();
	}
	if( m_pTrackFlagsPPG )
	{
		m_pTrackFlagsPPG->RefreshData();
	}
	if( m_pPropPianoRoll )
	{
		m_pPropPianoRoll->RefreshData();
	}
	// m_pTabPatternPattern is taken care of by the caller
	return S_OK;
}

HRESULT CPianoRollPropPageMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pGroupBitsPPG )
	{
		m_pGroupBitsPPG->SetObject( pINewPropPageObject );
	}
	if( m_pTrackFlagsPPG )
	{
		m_pTrackFlagsPPG->SetObject( pINewPropPageObject );
	}
	if( m_pPropPianoRoll )
	{
		m_pPropPianoRoll->SetObject( pINewPropPageObject );
	}
	return CBasePropPageManager::SetObject( pINewPropPageObject );
}

/////////////////////////////////////////////////////////////////////////////
// PropPianoRoll property page

IMPLEMENT_DYNCREATE(PropPianoRoll, CPropertyPage)

short *PropPianoRoll::sm_pnActiveTab = NULL;

PropPianoRoll::PropPianoRoll() : CPropertyPage(PropPianoRoll::IDD)
{
	//{{AFX_DATA_INIT(PropPianoRoll)
	//}}AFX_DATA_INIT
	m_pPPO = NULL;
	m_pIPChannelName = NULL;
	m_fNeedToDetach = FALSE;
	m_pIPropSheet = NULL;

	ASSERT(m_pcrCustomColors != NULL);
	int nColor;
	for (nColor = 0; nColor < 16; nColor++)
	{
		m_pcrCustomColors[nColor] = RGB(0, 0, 0);
	}
}

PropPianoRoll::~PropPianoRoll()
{
	RELEASE( m_pIPChannelName );
	RELEASE( m_pPPO );
}

void PropPianoRoll::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPianoRoll)
	DDX_Control(pDX, IDC_STATIC_EXTENSION, m_staticExtension);
	DDX_Control(pDX, IDC_COMBO_VARMODE, m_cmbVarMode);
	DDX_Control(pDX, IDC_PART_EDIT_PCHANNELNAME, m_editPChannelName);
	DDX_Control(pDX, IDC_PART_CHECK_INV_AUTOMATIC, m_checkAutoInvert);
	DDX_Control(pDX, IDC_PART_SPIN_PCHANNEL, m_spinPChannel);
	DDX_Control(pDX, IDC_PART_SPIN_INV_UPPER, m_spinInvUpper);
	DDX_Control(pDX, IDC_PART_SPIN_INV_LOWER, m_spinInvLower);
	DDX_Control(pDX, IDC_PART_EDIT_PCHANNEL, m_editPChannel);
	DDX_Control(pDX, IDC_PART_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_PART_EDIT_INV_UPPER, m_editInvUpper);
	DDX_Control(pDX, IDC_PART_EDIT_INV_LOWER, m_editInvLower);
	DDX_Control(pDX, IDC_COMBO_VAR_LOCK, m_cmbVarLock);
	DDX_Control(pDX, IDC_COMBO_PLAYMODE_2, m_cmbChordLevel);
	DDX_Control(pDX, IDC_BUTTON_TIME_SIGNATURE, m_btnTimeSignature);
	DDX_Control(pDX, IDC_BUTTON_SELECTIONCOLOR, m_btnSelNoteColor);
	DDX_Control(pDX, IDC_BUTTON_NOTECOLOR, m_btnNoteColor);
	DDX_Control(pDX, IDC_BUTTON_LENGTH, m_btnLength);
	DDX_Control(pDX, IDC_COMBO_PLAYMODE, m_cmbPlayMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPianoRoll, CPropertyPage)
	//{{AFX_MSG_MAP(PropPianoRoll)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_PLAYMODE, OnSelchangeComboPlaymode)
	ON_BN_CLICKED(IDC_BUTTON_NOTECOLOR, OnButtonNotecolor)
	ON_BN_CLICKED(IDC_BUTTON_SELECTIONCOLOR, OnButtonSelectioncolor)
	ON_EN_CHANGE(IDC_PART_EDIT_NAME, OnChangePartEditName)
	ON_BN_CLICKED(IDC_BUTTON_LENGTH, OnButtonLength)
	ON_BN_CLICKED(IDC_BUTTON_TIME_SIGNATURE, OnButtonTimeSignature)
	ON_EN_CHANGE(IDC_PART_EDIT_PCHANNEL, OnChangePartEditPChannel)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PART_SPIN_PCHANNEL, OnDeltaPosPartSpinPChannel)
	ON_CBN_SELCHANGE(IDC_COMBO_VAR_LOCK, OnSelChangeComboVarLock)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PART_SPIN_INV_LOWER, OnDeltaPosPartSpinInvLower)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PART_SPIN_INV_UPPER, OnDeltaPosPartSpinInvUpper)
	ON_BN_CLICKED(IDC_PART_CHECK_INV_AUTOMATIC, OnPartCheckInvAutomatic)
	ON_CBN_SELCHANGE(IDC_COMBO_PLAYMODE_2, OnSelchangeComboPlaymode2)
	ON_EN_CHANGE(IDC_PART_EDIT_PCHANNELNAME, OnChangePartEditPchannelname)
	ON_CBN_SELCHANGE(IDC_COMBO_VARMODE, OnSelchangeComboVarmode)
	ON_EN_KILLFOCUS(IDC_PART_EDIT_PCHANNEL, OnKillfocusPartEditPchannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void PropPianoRoll::SetObject( IDMUSProdPropPageObject* pPPO )
{
	RELEASE( m_pPPO );
	m_pPPO = pPPO;
	if( m_pPPO )
	{
		m_pPPO->AddRef();
	}
}

void PropPianoRoll::EnableControls( PianoRollData* pPRD, BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
	m_editPChannel.EnableWindow( fEnable );
	m_spinPChannel.EnableWindow( fEnable );
	m_editPChannelName.EnableWindow( fEnable );
	m_checkAutoInvert.EnableWindow( fEnable );
	m_cmbVarMode.EnableWindow( fEnable );
	m_cmbVarLock.EnableWindow( fEnable );
	m_btnNoteColor.EnableWindow( fEnable );
	m_btnSelNoteColor.EnableWindow( fEnable );
	m_btnTimeSignature.EnableWindow( fEnable );
	m_btnLength.EnableWindow( fEnable );
	m_cmbPlayMode.EnableWindow( fEnable );
	m_cmbChordLevel.EnableWindow( fEnable );

	if( fEnable == TRUE )
	{
		if( pPRD
		&&  pPRD->bAutoInvert )
		{
			fEnable = FALSE;
		}
	}
	
	m_editInvUpper.EnableWindow( fEnable );
	m_spinInvUpper.EnableWindow( fEnable );
	m_editInvLower.EnableWindow( fEnable );
	m_spinInvLower.EnableWindow( fEnable );
}
	
void PropPianoRoll::SetTimeSignatureBitmap( DirectMusicTimeSig* pTimeSig )
{
	HBITMAP hNewBits = NULL;

	RECT rect;
	m_btnTimeSignature.GetClientRect( &rect );

	// Create a DC for the new bitmap
	// a DC for the 'Grids Per Beat' bitmap
	// a Bitmap for the new bits
	CDC cdcDest;
	CDC cdcGridsPerBeat;
	CBitmap bmpNewBits;
	CBitmap bmpGridsPerBeat;

	CDC* pDC = m_btnTimeSignature.GetDC();
	if( pDC )
	{

		if( cdcDest.CreateCompatibleDC( pDC ) == FALSE
		||  cdcGridsPerBeat.CreateCompatibleDC( pDC ) == FALSE
		||  bmpNewBits.CreateCompatibleBitmap( pDC, rect.right, rect.bottom ) == FALSE )
		{
			m_btnTimeSignature.ReleaseDC( pDC );
			return;
		}

		m_btnTimeSignature.ReleaseDC( pDC );
	}

	// Create the new bitmap
	CBitmap* pbmpOldMem = cdcDest.SelectObject( &bmpNewBits );

	// Fill Rect with button color
	cdcDest.SetBkColor( ::GetSysColor(COLOR_BTNFACE) );
	cdcDest.ExtTextOut( 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

	// Write text
	CString strTimeSignature;

	CFont font;
	CFont* pfontOld = NULL;
	
	if( font.CreateFont( 10, 0, 0, 0, FW_NORMAL, 0, 0, 0,
 						DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" ) )
	{
		pfontOld = cdcDest.SelectObject( &font );
	}

	strTimeSignature.Format( "%d/%d",
							 pTimeSig->m_bBeatsPerMeasure,
							 pTimeSig->m_bBeat );
	rect.left += 6;
	cdcDest.SetTextColor( COLOR_BTNTEXT );
	cdcDest.DrawText( strTimeSignature, -1, &rect, (DT_SINGLELINE | DT_LEFT | DT_VCENTER) );
	rect.left -= 6;

	if( pfontOld )
	{
		cdcDest.SelectObject( pfontOld );
		font.DeleteObject();
	}

	// Set x coord for 'Grids Per Beat' image
	CSize sizeText = cdcDest.GetTextExtent( strTimeSignature );
	int nX = max( 48, (sizeText.cx + 8) );

	// Draw "splitter"
	{
		CPen pen1;
		CPen pen2;
		CPen* ppenOld;

		int nPlace = nX - 6;
		int nModeOld = cdcDest.SetROP2( R2_COPYPEN );

		// Highlight
		if( pen1.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW) ) )
		{
			ppenOld = cdcDest.SelectObject( &pen1 );
			cdcDest.MoveTo( nPlace, (rect.top + 3) );
			cdcDest.LineTo( nPlace, (rect.bottom - 3) );
			cdcDest.SelectObject( ppenOld );
		}

		// Shadow
		if( pen2.CreatePen( PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT) ) )
		{
			ppenOld = cdcDest.SelectObject( &pen2 );
			cdcDest.MoveTo( ++nPlace, (rect.top + 3) );
			cdcDest.LineTo( nPlace, (rect.bottom - 3) );
			cdcDest.SelectObject( ppenOld );
		}

		if( nModeOld )
		{
			cdcDest.SetROP2( nModeOld );
		}
	}

	// Add 'Grids Per Beat' bitmap
	{
		int nResourceID = pTimeSig->m_wGridsPerBeat - 1;
		if( pTimeSig->m_bBeat != 4 )		// 4 = quarter note gets the beat
		{
			nResourceID += MAX_GRIDS_PER_BEAT;
		}
		ASSERT( (nResourceID >= 0) && (nResourceID <= MAX_GRIDS_PER_BEAT_ENTRIES) );
		
		if( bmpGridsPerBeat.LoadBitmap( g_nGridsPerBeatBitmaps[nResourceID] ) )
		{
			BITMAP bm;

			bmpGridsPerBeat.GetBitmap( &bm );

			int nY = ((rect.bottom - rect.top) - bm.bmHeight) >> 1;

			CBitmap* pbmpOld = cdcGridsPerBeat.SelectObject( &bmpGridsPerBeat );

			{
				CDC cdcMono;
				CBitmap bmpMono;

				if( cdcMono.CreateCompatibleDC( &cdcDest )
				&&  bmpMono.CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL ) )
				{
					CBitmap* pbmpOldMono = cdcMono.SelectObject( &bmpMono );
					
					cdcGridsPerBeat.SetBkColor( RGB(255,255,255) );
					cdcDest.SetBkColor( RGB(255,255,255) );

					cdcMono.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight,
									&cdcGridsPerBeat, 0, 0, SRCCOPY);
					cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
									&cdcGridsPerBeat, 0, 0, SRCINVERT ) ;
					cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
									&cdcMono, 0, 0, SRCAND ) ;
					cdcDest.BitBlt( nX, nY, bm.bmWidth, bm.bmHeight,
									&cdcGridsPerBeat, 0, 0, SRCINVERT ) ;

					cdcMono.SelectObject( pbmpOldMono ) ;
				}
			}

			cdcGridsPerBeat.SelectObject( pbmpOld );
		}
	}

	cdcDest.SelectObject( pbmpOldMem );

	// Set the new bitmap
	hNewBits = (HBITMAP)bmpNewBits.Detach();
	if( hNewBits )
	{
		HBITMAP hBitmapOld = m_btnTimeSignature.SetBitmap( hNewBits );
		if( hBitmapOld )
		{
			::DeleteObject( hBitmapOld );
		}
	}
}

void PropPianoRoll::FillVarLockComoboBox( PianoRollData* pPRD )
{
	// Don't redraw until we are finished building the list
	m_cmbVarLock.SetRedraw( FALSE );
	
	// Remove all items
	m_cmbVarLock.ResetContent();

	// Load strings
	CString cstrNone;
	cstrNone.LoadString( IDS_NONE_TEXT );

	// Insert 'None'
	m_cmbVarLock.AddString( cstrNone );

	TCHAR tcstrText[100];
	CString cstrCreate; 
	cstrCreate.LoadString( IDS_CREATE_TEXT );

	// Rebuild InversionId list
	for( int i = 1 ;  i < 256 ;  i++ )
	{
		_itot( i, tcstrText, 10 );
		if( !(pPRD->adwVarLocks[i >> 5] & (1 << (i % 32))) )
		{
			_tcscat( tcstrText, cstrCreate );
		}
		m_cmbVarLock.AddString( tcstrText );
	}

	// Redraw the new list
	m_cmbVarLock.SetRedraw( TRUE );
}

void PropPianoRoll::FillChordLevelComoboBox( PianoRollData* pPRD )
{
	// Don't redraw until we are finished building the list
	m_cmbChordLevel.SetRedraw( FALSE );
	
	// Remove all items
	m_cmbChordLevel.ResetContent();

	// Load strings
	CString cstrText;

	// Rebuild Chord Level list
	if (pPRD->bPlayModeFlags == DMUS_PLAYMODE_FIXED
	||  pPRD->bPlayModeFlags == DMUS_PLAYMODE_FIXEDTOSCALE
	||  pPRD->bPlayModeFlags == DMUS_PLAYMODE_FIXEDTOCHORD)
	{
		for( int i = IDS_PLAYMODE_ABSOLUTE ;  i <= IDS_PLAYMODE_FIXEDTOCHORD ;  i++ )
		{
			cstrText.LoadString( i );
			m_cmbChordLevel.AddString( cstrText );
		}
	}
	else
	{
		for( int i = IDS_PLAYMODE_CHORD4 ;  i <= IDS_PLAYMODE_CHORD1 ;  i++ )
		{
			cstrText.LoadString( i );
			m_cmbChordLevel.AddString( cstrText );
		}
	}

	// Redraw the new list
	m_cmbChordLevel.SetRedraw( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// PropPianoRoll message handlers

int PropPianoRoll::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
	if( !FromHandlePermanent( m_hWnd ) )
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		Attach( hWnd );
		m_fNeedToDetach = TRUE;
	}

	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void PropPianoRoll::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Delete the time signature button's bitmap
	HBITMAP hBitmap = m_btnTimeSignature.GetBitmap();
	if( hBitmap )
	{
		::DeleteObject( hBitmap );
	}

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.
	if( m_fNeedToDetach && m_hWnd )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
	}

	RELEASE( m_pIPChannelName );
	RELEASE( m_pPPO );

	CPropertyPage::OnDestroy();
}

BOOL PropPianoRoll::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();

	CString	cstrTemp;
	long lTemp;

	// Fill play mode combo box 
	m_cmbPlayMode.ResetContent();
	for( lTemp = IDS_PLAYMODE_CHORDSCALE; lTemp <= IDS_PLAYMODE_FIXED; lTemp++ )
	{
		if( cstrTemp.LoadString( lTemp ) )
		{
			m_cmbPlayMode.AddString( cstrTemp );
		}
	}
	if (m_cmbPlayMode.m_hWnd != NULL)
	{
		m_cmbPlayMode.EnableWindow(TRUE);
	}

	// Fill variation play mode combo box
	m_cmbVarMode.ResetContent();
	for( lTemp = IDS_VARMODE_RANDOM; lTemp <= IDS_VARMODE_RANDOM_ROW; lTemp++ )
	{
		if( cstrTemp.LoadString( lTemp ) )
		{
			m_cmbVarMode.AddString( cstrTemp );
		}
	}
	if (m_cmbVarMode.m_hWnd != NULL)
	{
		m_cmbVarMode.EnableWindow(TRUE);
	}

	// Limit PChannel names to DMUS_MAX_NAME - 1 characters
	m_editPChannelName.LimitText( DMUS_MAX_NAME - 1 );

	// Set ranges
	m_editPChannel.LimitText( 5 ); // 'APath'
	m_spinPChannel.SetRange( MIN_PCHANNEL, MAX_PCHANNEL );

	m_editInvLower.LimitText( 3 );
	m_spinInvLower.SetRange( MIN_INV_LOWER, MAX_INV_LOWER );

	m_editInvUpper.LimitText( 3 );
	m_spinInvUpper.SetRange( MIN_INV_UPPER, MAX_INV_UPPER );

	m_strBroadcastSeg.LoadString(IDS_BROADCAST_SEG);
	m_strBroadcastPerf.LoadString(IDS_BROADCAST_PERF);
	m_strBroadcastAPath.LoadString(IDS_BROADCAST_APATH);
	m_strBroadcastGrp.LoadString(IDS_BROADCAST_GRP);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void PropPianoRoll::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure controls have been created
	if( m_cmbPlayMode.GetSafeHwnd() == NULL )
	{
		return;
	}

	if( m_pPPO == NULL )
	{
		m_editName.SetWindowText( _T("") );
		m_editInvLower.SetWindowText( _T("") );
		m_editInvUpper.SetWindowText( _T("") );
		m_btnTimeSignature.SetWindowText( _T("") );
		m_btnLength.SetWindowText( _T("") );
		m_spinPChannel.SetPos( MIN_PCHANNEL );
		m_spinInvLower.SetPos( MIN_INV_LOWER );
		m_spinInvUpper.SetPos( MAX_INV_UPPER );
		m_cmbVarLock.SetCurSel( -1 );
		m_cmbVarMode.SetCurSel( -1 );
		m_checkAutoInvert.SetCheck( 0 );

		EnableControls( NULL, FALSE );
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	EnableControls( &PRD, TRUE );

	CString cstrText;

	// Set PartRef name
	m_editName.GetWindowText( cstrText );
	if( cstrText.Compare( PRD.cstrPartRefName ) != 0 )
	{
		m_editName.SetWindowText( PRD.cstrPartRefName );
	}

	// Set PChannel
	if( PRD.dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS )
	{
		m_editPChannelName.EnableWindow( FALSE );
		if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
		{
			m_editPChannel.SetWindowText( m_strBroadcastSeg );
		}
		else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
		{
			m_editPChannel.SetWindowText( m_strBroadcastPerf );
		}
		else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
		{
			m_editPChannel.SetWindowText( m_strBroadcastGrp );
		}
		else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
		{
			m_editPChannel.SetWindowText( m_strBroadcastAPath );
		}
	}
	else
	{
		m_spinPChannel.SetPos( PRD.dwPChannel + 1 );
	}

	// Set PChannel name
	m_editPChannelName.GetWindowText( cstrText );
	if( cstrText.Compare( PRD.cstrPChannelName ) != 0 )
	{
		m_editPChannelName.SetWindowText( PRD.cstrPChannelName );
	}

	FillInversionBoundaries( &PRD );

	// Set auto inversion boundaries
	m_checkAutoInvert.SetCheck( PRD.bAutoInvert );

	// Set time signature
	SetTimeSignatureBitmap( &PRD.ts );
		
	// Set length
	CString strLength;
	strLength.Format( "%d", PRD.wNbrMeasures );
	m_btnLength.SetWindowText( strLength );

	// Set extra length
	TCHAR tcstrNum1[20], tcstrNum2[20];
	_itot( PRD.fPickupBar ? 1 : 0, tcstrNum1, 10 );
	_itot( PRD.dwExtraBars, tcstrNum2, 10 );
	AfxFormatString2( strLength, IDS_EXTENSION_PPG_TEXT, tcstrNum1, tcstrNum2 );
	m_staticExtension.SetWindowText( strLength );

	// Set variation lock
	FillVarLockComoboBox( &PRD );
	m_cmbVarLock.SetCurSel( PRD.bVariationLockID );

	// Set variation order
	switch( PRD.bRandomVariation )
	{
	case DMUS_VARIATIONT_RANDOM:
		m_cmbVarMode.SetCurSel( 0 );
		break;
	case DMUS_VARIATIONT_SEQUENTIAL:
		m_cmbVarMode.SetCurSel( 1 );
		break;
	case DMUS_VARIATIONT_RANDOM_START:
	case DMUS_VARIATIONT_NO_REPEAT:
	case DMUS_VARIATIONT_RANDOM_ROW:
		m_cmbVarMode.SetCurSel( PRD.bRandomVariation );
		break;
	default:
		ASSERT(FALSE);
		m_cmbVarMode.SetCurSel( 0 );
		break;
	}

	// Set play mode
	if (PRD.bPlayModeFlags == DMUS_PLAYMODE_NONE)
	{
		ASSERT(FALSE);
	}
	else if (PRD.bPlayModeFlags == DMUS_PLAYMODE_PURPLEIZED)
	{
		m_cmbPlayMode.SetCurSel( 0 );
	}
	else if (PRD.bPlayModeFlags == DMUS_PLAYMODE_NORMALCHORD)
	{
		m_cmbPlayMode.SetCurSel( 1 );
	}
	else if (PRD.bPlayModeFlags == (DMUS_PLAYMODE_SCALE_INTERVALS | DMUS_PLAYMODE_CHORD_ROOT))
	{
		m_cmbPlayMode.SetCurSel( 2 );
	}
	else if (PRD.bPlayModeFlags == DMUS_PLAYMODE_PEDALPOINT)
	{
		m_cmbPlayMode.SetCurSel( 3 );
	}
	else if( PRD.bPlayModeFlags == DMUS_PLAYMODE_PEDALPOINTCHORD )
	{
		m_cmbPlayMode.SetCurSel( 4 );
	}
	else if( PRD.bPlayModeFlags == DMUS_PLAYMODE_PEDALPOINTALWAYS )
	{
		m_cmbPlayMode.SetCurSel( 5 );
	}
	else if (PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXED
		 ||  PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXEDTOSCALE
		 ||  PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXEDTOCHORD)
	{
		m_cmbPlayMode.SetCurSel( 6 );
	}
	m_cmbPlayMode.EnableWindow(TRUE);

	// Set chord level
	FillChordLevelComoboBox( &PRD );
	if (PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXED)
	{
		m_cmbChordLevel.SetCurSel( 0 );
	}
	else if (PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXEDTOSCALE)
	{
		m_cmbChordLevel.SetCurSel( 1 );
	}
	else if (PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXEDTOCHORD)
	{
		m_cmbChordLevel.SetCurSel( 2 );
	}
	else
	{
		m_cmbChordLevel.SetCurSel( 3 - PRD.bSubChordLevel );
	}
	m_cmbChordLevel.EnableWindow(TRUE);
}

BOOL PropPianoRoll::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RefreshData();

	// Store active tab
	if( sm_pnActiveTab && m_pIPropSheet )
	{
		m_pIPropSheet->GetActivePage( sm_pnActiveTab );
	}

	return CPropertyPage::OnSetActive();
}

void PropPianoRoll::OnSelchangeComboPlaymode() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO )
	{
		PianoRollData PRD;
		InitializePRData( &PRD );

		BYTE bPlayMode = DMUS_PLAYMODE_NONE;

		switch( m_cmbPlayMode.GetCurSel() )
		{
		case 0: // Chord/Scale
			bPlayMode = DMUS_PLAYMODE_PURPLEIZED;
			break;
		case 1: // Chord
			bPlayMode = DMUS_PLAYMODE_NORMALCHORD;
			break;
		case 2: // Scale
			bPlayMode = (DMUS_PLAYMODE_SCALE_INTERVALS | DMUS_PLAYMODE_CHORD_ROOT);
			break;
		case 3: // PedalPoint
			bPlayMode = DMUS_PLAYMODE_PEDALPOINT;
			break;
		case 4: // PedalPoint Chord
			bPlayMode = DMUS_PLAYMODE_PEDALPOINTCHORD;
			break;
		case 5: // PedalPoint Always
			bPlayMode = DMUS_PLAYMODE_PEDALPOINTALWAYS;
			break;
		case 6: // Fixed
			bPlayMode = DMUS_PLAYMODE_FIXED;
			break;
		case CB_ERR:
			// None selected
		default:
			ASSERT( FALSE );
			break;
		}

		if (bPlayMode == DMUS_PLAYMODE_NONE)
		{
			ASSERT( FALSE );
		}
		else
		{
			if( PRD.bPlayModeFlags != bPlayMode )
			{
				PRD.bPlayModeFlags = bPlayMode;

				// Set chord level
				FillChordLevelComoboBox( &PRD );
				if (PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXED)
				{
					m_cmbChordLevel.SetCurSel( 0 );
				}
				else if (PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXEDTOSCALE)
				{
					m_cmbChordLevel.SetCurSel( 1 );
				}
				else if (PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXEDTOCHORD)
				{
					m_cmbChordLevel.SetCurSel( 2 );
				}
				else
				{
					m_cmbChordLevel.SetCurSel( 3 - PRD.bSubChordLevel );
				}
				m_cmbChordLevel.EnableWindow(TRUE);

				m_pPPO->SetData( (void*)&PRD );
			}
		}
	}
}

void PropPianoRoll::OnSelchangeComboPlaymode2() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO )
	{
		PianoRollData PRD;
		InitializePRData( &PRD );

		if( PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXED
		||  PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXEDTOSCALE
		||  PRD.bPlayModeFlags == DMUS_PLAYMODE_FIXEDTOCHORD )
		{
			BYTE bPlayMode = DMUS_PLAYMODE_NONE;

			switch( m_cmbChordLevel.GetCurSel() )
			{
			case 0: // Absolute
				bPlayMode = DMUS_PLAYMODE_FIXED;
				break;
			case 1: // to Scale
				bPlayMode = DMUS_PLAYMODE_FIXEDTOSCALE;
				break;
			case 2: // to Chord
				bPlayMode = DMUS_PLAYMODE_FIXEDTOCHORD;
				break;
			case CB_ERR:
				// None selected
			default:
				ASSERT( FALSE );
				break;
			}

			if( bPlayMode == DMUS_PLAYMODE_NONE )
			{
				ASSERT( FALSE );
			}
			else
			{
				if( PRD.bPlayModeFlags != bPlayMode )
				{
					PRD.bPlayModeFlags = bPlayMode;
					m_pPPO->SetData( (void*)&PRD );
				}
			}
		}
		else
		{
			BYTE bSubChordLevel = 0xFF;

			switch( m_cmbChordLevel.GetCurSel() )
			{
			case 0: // Chord Level 4
				bSubChordLevel = SUBCHORD_UPPER_3;
				break;
			case 1: // Chord Level 3
				bSubChordLevel = SUBCHORD_UPPER_2;
				break;
			case 2: // Chord Level 2
				bSubChordLevel = SUBCHORD_UPPER_1;
				break;
			case 3: // Chord Level 1
				bSubChordLevel = SUBCHORD_BASS;
				break;
			case CB_ERR:
				// None selected
			default:
				ASSERT( FALSE );
				break;
			}

			if( bSubChordLevel == -1 )
			{
				ASSERT( FALSE );
			}
			else
			{
				if( PRD.bSubChordLevel != bSubChordLevel )
				{
					PRD.bSubChordLevel = bSubChordLevel;
					m_pPPO->SetData( (void*)&PRD );
				}
			}
		}
	}
}

void PropPianoRoll::OnButtonNotecolor() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CHOOSECOLOR cc;

	PianoRollData PRD;
	InitializePRData( &PRD );

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = this->GetSafeHwnd();
	cc.hInstance = NULL;
	cc.rgbResult = PRD.crUnselectedNoteColor; // initial color
	cc.lpCustColors = m_pcrCustomColors;
	cc.Flags = CC_ANYCOLOR | CC_RGBINIT;
	cc.lCustData = NULL;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;

	HWND hwndFocus = ::GetFocus();
	if (ChooseColor( &cc ))
	{
		// User chose a color
		PRD.crUnselectedNoteColor = cc.rgbResult;
		// Display is refreshed automatically
		m_pPPO->SetData( (void*) &PRD);
	}

	if( ::GetFocus() != hwndFocus )
	{
		::SetFocus( hwndFocus );
	}
}

void PropPianoRoll::OnButtonSelectioncolor() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CHOOSECOLOR cc;

	PianoRollData PRD;
	InitializePRData( &PRD );

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = this->GetSafeHwnd();
	cc.hInstance = NULL;
	cc.rgbResult = PRD.crSelectedNoteColor; // initial color
	cc.lpCustColors = m_pcrCustomColors;
	cc.Flags = CC_ANYCOLOR | CC_RGBINIT;
	cc.lCustData = NULL;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;

	HWND hwndFocus = ::GetFocus();
	if (ChooseColor( &cc ))
	{
		// User chose a color
		PRD.crSelectedNoteColor = cc.rgbResult;
		// Display is refreshed automatically
		m_pPPO->SetData( (void*) &PRD);
	}

	if( ::GetFocus() != hwndFocus )
	{
		::SetFocus( hwndFocus );
	}
}

void PropPianoRoll::OnChangePartEditName() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	CString cstrNewName;

	m_editName.GetWindowText( cstrNewName );

	// Strip leading and trailing spaces
	cstrNewName.TrimRight();
	cstrNewName.TrimLeft();

	if( cstrNewName.Compare( PRD.cstrPartRefName ) != 0 )
	{
		PRD.cstrPartRefName = cstrNewName;
		m_pPPO->SetData( (void*)&PRD );
	}
}

void PropPianoRoll::OnButtonLength() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	CPartLengthDlg plDlg;

	plDlg.m_wNbrMeasures = PRD.wNbrMeasures;
	plDlg.m_dwNbrExtraBars = PRD.dwExtraBars;
	plDlg.m_fPickupBar = PRD.fPickupBar;

	if( plDlg.DoModal() == IDOK )
	{
		CString strLength;
		bool fChange = false;
		if(	plDlg.m_wNbrMeasures != PRD.wNbrMeasures )
		{
			strLength.Format( "%d", plDlg.m_wNbrMeasures );
			m_btnLength.SetWindowText( strLength );

			PRD.wNbrMeasures = plDlg.m_wNbrMeasures;
			fChange = true;
		}

		if(	(plDlg.m_fPickupBar != PRD.fPickupBar)
		||	(plDlg.m_dwNbrExtraBars != PRD.dwExtraBars) )
		{
			PRD.fPickupBar = plDlg.m_fPickupBar;
			PRD.dwExtraBars = plDlg.m_dwNbrExtraBars;

			// Set extra length
			TCHAR tcstrNum1[20], tcstrNum2[20];
			_itot( PRD.fPickupBar ? 1 : 0, tcstrNum1, 10 );
			_itot( PRD.dwExtraBars, tcstrNum2, 10 );
			AfxFormatString2( strLength, IDS_EXTENSION_PPG_TEXT, tcstrNum1, tcstrNum2 );
			m_staticExtension.SetWindowText( strLength );

			fChange = true;
		}

		if( fChange )
		{
			m_pPPO->SetData( (void*)&PRD );

			// Check to see if the pickup/extension data was invalid
			PianoRollData* pPRD = &PRD;
			m_pPPO->GetData( (void**)&pPRD );
			if( (plDlg.m_fPickupBar != PRD.fPickupBar)
			||	(plDlg.m_dwNbrExtraBars != PRD.dwExtraBars) )
			{
				// Set extra length
				TCHAR tcstrNum1[20], tcstrNum2[20];
				_itot( PRD.fPickupBar ? 1 : 0, tcstrNum1, 10 );
				_itot( PRD.dwExtraBars, tcstrNum2, 10 );
				AfxFormatString2( strLength, IDS_EXTENSION_PPG_TEXT, tcstrNum1, tcstrNum2 );
				m_staticExtension.SetWindowText( strLength );
			}
		}
	}

	m_btnLength.SetFocus();
}

void PropPianoRoll::OnButtonTimeSignature() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );
	
	CTimeSignatureDlg tsDlg;

	tsDlg.m_TimeSignature = PRD.ts;

	if( tsDlg.DoModal() == IDOK )
	{
		if( tsDlg.m_TimeSignature.m_bBeatsPerMeasure != PRD.ts.m_bBeatsPerMeasure
		||  tsDlg.m_TimeSignature.m_bBeat != PRD.ts.m_bBeat
		||  tsDlg.m_TimeSignature.m_wGridsPerBeat != PRD.ts.m_wGridsPerBeat )
		{
			SetTimeSignatureBitmap( &tsDlg.m_TimeSignature );

			PRD.ts = tsDlg.m_TimeSignature;
			m_pPPO->SetData( (void*)&PRD );

			// Sync nbr measures
			CString strLength;
			strLength.Format( "%d", PRD.wNbrMeasures );
			m_btnLength.SetWindowText( strLength );
		}
	}

	m_btnTimeSignature.SetFocus();
}

void PropPianoRoll::OnChangePartEditPChannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (m_pPPO == NULL) || (m_editPChannel.GetSafeHwnd() == NULL) )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	int nOldPChannel = PRD.dwPChannel + 1; 

	CString cstrNewPChannel;

	m_editPChannel.GetWindowText( cstrNewPChannel );

	// Strip leading and trailing spaces
	cstrNewPChannel.TrimRight();
	cstrNewPChannel.TrimLeft();

	if( !cstrNewPChannel.IsEmpty() )
	{
		// Check if this is broadcast PChannel
		if( _tcsnicmp( cstrNewPChannel, m_strBroadcastPerf, cstrNewPChannel.GetLength() ) == 0 )
		{
			if( PRD.dwPChannel != DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
			{
				PRD.dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
				PRD.cstrPChannelName = UpdatePChannelName( PRD.dwPChannel );
				m_pPPO->SetData( (void*)&PRD );

				// Changing a PChannel may cause this property sheet to be removed
				// so don't assume controls are still hanging around
				if( ::IsWindow(m_editPChannelName.m_hWnd) )
				{
					m_editPChannelName.SetWindowText( PRD.cstrPChannelName );
					m_editPChannelName.EnableWindow( FALSE );
				}
			}
		}
		else if( _tcsnicmp( cstrNewPChannel, m_strBroadcastSeg, cstrNewPChannel.GetLength() ) == 0 )
		{
			if( PRD.dwPChannel != DMUS_PCHANNEL_BROADCAST_SEGMENT )
			{
				PRD.dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
				PRD.cstrPChannelName = UpdatePChannelName( PRD.dwPChannel );
				m_pPPO->SetData( (void*)&PRD );

				// Changing a PChannel may cause this property sheet to be removed
				// so don't assume controls are still hanging around
				if( ::IsWindow(m_editPChannelName.m_hWnd) )
				{
					m_editPChannelName.SetWindowText( PRD.cstrPChannelName );
					m_editPChannelName.EnableWindow( FALSE );
				}
			}
		}
		else if( _tcsnicmp( cstrNewPChannel, m_strBroadcastAPath, cstrNewPChannel.GetLength() ) == 0 )
		{
			if( PRD.dwPChannel != DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
			{
				PRD.dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
				PRD.cstrPChannelName = UpdatePChannelName( PRD.dwPChannel );
				m_pPPO->SetData( (void*)&PRD );

				// Changing a PChannel may cause this property sheet to be removed
				// so don't assume controls are still hanging around
				if( ::IsWindow(m_editPChannelName.m_hWnd) )
				{
					m_editPChannelName.SetWindowText( PRD.cstrPChannelName );
					m_editPChannelName.EnableWindow( FALSE );
				}
			}
		}
		else if( _tcsnicmp( cstrNewPChannel, m_strBroadcastGrp, cstrNewPChannel.GetLength() ) == 0 )
		{
			if( PRD.dwPChannel != DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				PRD.dwPChannel = DMUS_PCHANNEL_BROADCAST_GROUPS;
				PRD.cstrPChannelName = UpdatePChannelName( PRD.dwPChannel );
				m_pPPO->SetData( (void*)&PRD );

				// Changing a PChannel may cause this property sheet to be removed
				// so don't assume controls are still hanging around
				if( ::IsWindow(m_editPChannelName.m_hWnd) )
				{
					m_editPChannelName.SetWindowText( PRD.cstrPChannelName );
					m_editPChannelName.EnableWindow( FALSE );
				}
			}
		}
		else
		{
			BOOL fTransSucceeded;
			int nNewPChannel = GetDlgItemInt( IDC_PART_EDIT_PCHANNEL, &fTransSucceeded, FALSE );
			if ( !fTransSucceeded || (nNewPChannel < MIN_PCHANNEL) )
			{
				nNewPChannel = MIN_PCHANNEL;
				m_spinPChannel.SetPos( nNewPChannel );
			}
			else if( nNewPChannel > MAX_PCHANNEL )
			{
				nNewPChannel = MAX_PCHANNEL;
				m_spinPChannel.SetPos( nNewPChannel );
			}

			m_editPChannelName.EnableWindow( TRUE );
			
			if( nNewPChannel != nOldPChannel )
			{
				PRD.dwPChannel = nNewPChannel - 1;
				PRD.cstrPChannelName = UpdatePChannelName( PRD.dwPChannel );
				m_pPPO->SetData( (void*)&PRD );
				// Changing a PChannel may cause this property sheet to be removed
				// so don't assume controls are still hanging around
				if( ::IsWindow(m_editPChannelName.m_hWnd) )
				{
					m_editPChannelName.SetWindowText( PRD.cstrPChannelName );
				}
			}
		}
	}
}

void PropPianoRoll::OnDeltaPosPartSpinPChannel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	*pResult = 0;

	if( (m_pPPO == NULL) || (m_editPChannelName.GetSafeHwnd() == NULL) )
	{
		return;
	}

	if( pNMUpDown )
	{
		PianoRollData PRD;
		InitializePRData( &PRD );

		if( pNMUpDown->iDelta > 0 )
		{
			if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
			{
				m_editPChannel.SetWindowText( m_strBroadcastGrp);
				// Taken care of by OnChangeEditPchannel
				//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_GROUPS;
				*pResult = 1;
			}
			if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
			{
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
				// Taken care of by OnChangeEditPchannel
				//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
				*pResult = 1;
			}
			if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				m_editPChannel.SetWindowText( m_strBroadcastAPath );
				// Taken care of by OnChangeEditPchannel
				//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
				*pResult = 1;
			}
			else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
			{
				m_spinPChannel.SetPos( 1 );
				// Taken care of by OnChangeEditPchannel
				//m_dwPChannel = 0;
				*pResult = 1;
			}
		}
		else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
		{
			*pResult = 1;
		}
		else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
		{
			m_editPChannel.SetWindowText( m_strBroadcastGrp );
			// Taken care of by OnChangeEditPchannel
			//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_GROUPS;
			*pResult = 1;
		}
		else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
		{
			m_editPChannel.SetWindowText( m_strBroadcastAPath );
			// Taken care of by OnChangeEditPchannel
			//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_AUDIOPATH;
			*pResult = 1;
		}
		else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
		{
			m_editPChannel.SetWindowText( m_strBroadcastPerf );
			// Taken care of by OnChangeEditPchannel
			//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
			*pResult = 1;
		}
		else if( PRD.dwPChannel == 0 )
		{
			m_editPChannel.SetWindowText( m_strBroadcastSeg );
			// Taken care of by OnChangeEditPchannel
			//m_dwPChannel = DMUS_PCHANNEL_BROADCAST_SEGMENT;
			*pResult = 1;
		}
	}
}

void PropPianoRoll::OnSelChangeComboVarLock() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	if( m_cmbVarLock.GetSafeHwnd() != NULL )
	{
		int nCurSel = m_cmbVarLock.GetCurSel();

		switch( nCurSel )
		{
			case CB_ERR:
				break;

			default:
			{
				BYTE bNewVarLock = BYTE(nCurSel);

				if( PRD.bVariationLockID != bNewVarLock )
				{
					PRD.bVariationLockID = bNewVarLock;
					m_pPPO->SetData( (void*)&PRD );

					// Refresh ourself, in case this create a new variation lock group
					RefreshData();
				}
				break;
			}
		}
	}
}

void PropPianoRoll::OnDeltaPosPartSpinInvLower(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	int nOldInvertLower = PRD.bInvertLower; 

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nPos = m_spinInvLower.GetPos();
	int nNewInvertLower = LOWORD(nPos) + pNMUpDown->iDelta;

	if( nNewInvertLower < MIN_INV_LOWER)
	{
		nNewInvertLower = MIN_INV_LOWER;
		m_spinInvLower.SetPos( nNewInvertLower );
	}

	if( nNewInvertLower > MAX_INV_LOWER )
	{
		nNewInvertLower = MAX_INV_LOWER;
		m_spinInvLower.SetPos( nNewInvertLower );	
	}
	
	if( nNewInvertLower != nOldInvertLower )
	{
		PRD.bInvertLower = (BYTE)nNewInvertLower;
		
		// Make sure range is at least an octave
		if( PRD.bInvertLower > 115 ) // 127-12
		{
			PRD.bInvertLower = 115;
			PRD.bInvertUpper = 127;
		}
		else if( (PRD.bInvertLower + 12) > PRD.bInvertUpper )
		{
			PRD.bInvertUpper = BYTE(PRD.bInvertLower + 12);
		}

		m_pPPO->SetData( (void*)&PRD );

		FillInversionBoundaries( &PRD );
	}
	
	*pResult = 1;
}

void PropPianoRoll::OnDeltaPosPartSpinInvUpper(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	int nOldInvertUpper = PRD.bInvertUpper; 

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nPos = m_spinInvUpper.GetPos();
	int nNewInvertUpper = LOWORD(nPos) + pNMUpDown->iDelta;

	if( nNewInvertUpper < MIN_INV_UPPER)
	{
		nNewInvertUpper = MIN_INV_UPPER;
		m_spinInvUpper.SetPos( nNewInvertUpper );
	}

	if( nNewInvertUpper > MAX_INV_UPPER )
	{
		nNewInvertUpper = MAX_INV_UPPER;
		m_spinInvUpper.SetPos( nNewInvertUpper );	
	}
	
	if( nNewInvertUpper != nOldInvertUpper )
	{
		PRD.bInvertUpper = (BYTE)nNewInvertUpper;
		
		// Make sure range is at least an octave
		if( PRD.bInvertUpper < 12 ) // 127-12
		{
			PRD.bInvertLower = 0;
			PRD.bInvertUpper = 12;
		}
		else if( (PRD.bInvertUpper - 12) < PRD.bInvertLower )
		{
			PRD.bInvertLower = BYTE(PRD.bInvertUpper - 12);
		}

		m_pPPO->SetData( (void*)&PRD );

		FillInversionBoundaries( &PRD );
	}
	
	*pResult = 1;
}

void PropPianoRoll::OnPartCheckInvAutomatic() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	int nOldAutoInvert = PRD.bAutoInvert; 
	int nNewAutoInvert = m_checkAutoInvert.GetCheck(); 

	if( nNewAutoInvert != nOldAutoInvert )
	{
		PRD.bAutoInvert = (BYTE)nNewAutoInvert;

		m_editInvUpper.EnableWindow( !PRD.bAutoInvert );
		m_spinInvUpper.EnableWindow( !PRD.bAutoInvert );
		m_editInvLower.EnableWindow( !PRD.bAutoInvert );
		m_spinInvLower.EnableWindow( !PRD.bAutoInvert );

		m_pPPO->SetData( (void*)&PRD );

		if( PRD.bAutoInvert )
		{
			InitializePRData( &PRD );

			FillInversionBoundaries( &PRD );
		}
	}
}

void PropPianoRoll::OnChangePartEditPchannelname() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	CString cstrNewName;

	m_editPChannelName.GetWindowText( cstrNewName );

	// Strip leading and trailing spaces
	cstrNewName.TrimRight();
	cstrNewName.TrimLeft();

	if( cstrNewName.Compare( PRD.cstrPChannelName ) != 0 )
	{
		PRD.cstrPChannelName = cstrNewName;
		m_pPPO->SetData( (void*)&PRD );
	}
}

CString PropPianoRoll::UpdatePChannelName( DWORD dwPChannel ) 
{
	if( !m_pIPChannelName )
	{
		IDMUSProdStrip *pIStrip;
		if( SUCCEEDED( m_pPPO->QueryInterface( IID_IDMUSProdStrip, (void**) &pIStrip ) ) )
		{
			VARIANT varStripMgr;
			if( SUCCEEDED( pIStrip->GetStripProperty( SP_STRIPMGR, &varStripMgr ) )
			&& (V_UNKNOWN(&varStripMgr) != NULL) )
			{
				IDMUSProdStripMgr *pIStripMgr;
				if( SUCCEEDED( V_UNKNOWN(&varStripMgr)->QueryInterface( IID_IDMUSProdStripMgr, (void **)&pIStripMgr ) ) )
				{
					VARIANT varTimeline;
					if( SUCCEEDED( pIStripMgr->GetStripMgrProperty( SMP_ITIMELINECTL, &varTimeline ) ) )
					{
						IDMUSProdTimeline *pITimelineCtl;
						if( SUCCEEDED( V_UNKNOWN( &varTimeline )->QueryInterface( IID_IDMUSProdTimeline, (void **)&pITimelineCtl ) ) )
						{
							VARIANT varFramework;
							if (SUCCEEDED(pITimelineCtl->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &varFramework )))
							{
								IDMUSProdFramework *pIFramework;
								if( SUCCEEDED( V_UNKNOWN(&varFramework)->QueryInterface( IID_IDMUSProdFramework, (void**)&pIFramework ) ) )
								{
									VARIANT varCallback;
									if (SUCCEEDED(pITimelineCtl->GetTimelineProperty( TP_TIMELINECALLBACK, &varCallback )))
									{
										IDMUSProdNode* pINode;
										if( SUCCEEDED( V_UNKNOWN(&varCallback)->QueryInterface( IID_IDMUSProdNode, (void**)&pINode ) ) )
										{
											IDMUSProdProject* pIProject;
											if( SUCCEEDED ( pIFramework->FindProject( pINode, &pIProject ) ) )
											{
												pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&m_pIPChannelName );
												pIProject->Release();
											}
											pINode->Release();
										}
										V_UNKNOWN(&varCallback)->Release();
									}
									pIFramework->Release();
								}
								V_UNKNOWN(&varFramework)->Release();
							}
							pITimelineCtl->Release();
						}
						V_UNKNOWN( &varTimeline )->Release();
					}
					pIStripMgr->Release();
				}
				V_UNKNOWN(&varStripMgr)->Release();
			}
			pIStrip->Release();
		}
	}

	ASSERT( m_pIPChannelName );
	
	WCHAR wstrName[MAX_PATH];

	if( m_pIPChannelName && SUCCEEDED( m_pIPChannelName->GetPChannelName( dwPChannel, wstrName ) ) )
	{
		// Convert PChannel name
		CString cstrText;
		cstrText = wstrName;

		return cstrText;
	}

	return CString("");
}

void PropPianoRoll::FillInversionBoundaries( const PianoRollData* pPRD )
{
	// Set inversion lower boundary
	CString strMidiNote;
	CString strText;
	long lOctave = pPRD->bInvertLower / 12;
	long lNote = pPRD->bInvertLower % 12;
	strMidiNote = CString(MidiValueToName).Mid(lNote*5, 5);
	strMidiNote.TrimRight();
	strText.Format( "%s %d", strMidiNote, lOctave );
	m_editInvLower.SetWindowText( strText );
	int nPos = m_spinInvLower.GetPos();
	if( HIWORD(nPos) != 0
	||  LOWORD(nPos) != pPRD->bInvertLower )
	{
		m_spinInvLower.SetPos( pPRD->bInvertLower );
	}

	// Set inversion upper boundary
	lOctave = pPRD->bInvertUpper / 12;
	lNote = pPRD->bInvertUpper % 12;
	strMidiNote = CString(MidiValueToName).Mid(lNote*5, 5);
	strMidiNote.TrimRight();
	strText.Format( "%s %d", strMidiNote, lOctave );
	m_editInvUpper.SetWindowText( strText );
	nPos = m_spinInvUpper.GetPos();
	if( HIWORD(nPos) != 0
	||  LOWORD(nPos) != pPRD->bInvertUpper )
	{
		m_spinInvUpper.SetPos( pPRD->bInvertUpper );
	}
}

void PropPianoRoll::OnSelchangeComboVarmode() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	int nOldVarOrder = PRD.bRandomVariation; 
	int nNewVarOrder = m_cmbVarMode.GetCurSel();
	
	// Get variation order
	switch( nNewVarOrder )
	{
	case 0:
		nNewVarOrder = DMUS_VARIATIONT_RANDOM;
		break;
	case 1:
		nNewVarOrder = DMUS_VARIATIONT_SEQUENTIAL;
		break;
	default:
		// Do nothing - the index is the same as the value for
		// 2 - DMUS_VARIATIONT_RANDOM_START:
		// 3 - DMUS_VARIATIONT_NO_REPEAT:
		// 4 - DMUS_VARIATIONT_RANDOM_ROW:
		break;
	}
		
	if( nNewVarOrder != nOldVarOrder )
	{
		PRD.bRandomVariation = (BYTE)nNewVarOrder;
		m_pPPO->SetData( (void*)&PRD );
	}
}

void PropPianoRoll::OnKillfocusPartEditPchannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (m_pPPO == NULL) || (m_editPChannel.GetSafeHwnd() == NULL) )
	{
		return;
	}

	PianoRollData PRD;
	InitializePRData( &PRD );

	if (m_editPChannel.GetSafeHwnd() != NULL)
	{
		CString strNewPChannel;
		m_editPChannel.GetWindowText( strNewPChannel );

		// Strip leading and trailing spaces
		strNewPChannel.TrimRight();
		strNewPChannel.TrimLeft();

		if ( strNewPChannel.IsEmpty() )
		{
			if( PRD.dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				m_spinPChannel.SetPos( PRD.dwPChannel + 1 );
				m_editPChannelName.EnableWindow( TRUE );
			}
			else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT )
			{
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
				m_editPChannelName.EnableWindow( FALSE );
			}
			else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE )
			{
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
				m_editPChannelName.EnableWindow( FALSE );
			}
			else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
			{
				m_editPChannel.SetWindowText( m_strBroadcastAPath );
				m_editPChannelName.EnableWindow( FALSE );
			}
			else if( PRD.dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
			{
				m_editPChannel.SetWindowText( m_strBroadcastGrp );
				m_editPChannelName.EnableWindow( FALSE );
			}
		}
		else
		{
			if( _tcsnicmp( strNewPChannel, m_strBroadcastPerf, strNewPChannel.GetLength() ) == 0 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastPerf );
			}
			else if( _tcsnicmp( strNewPChannel, m_strBroadcastSeg, strNewPChannel.GetLength() ) == 0 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastSeg );
			}
			else if( _tcsnicmp( strNewPChannel, m_strBroadcastAPath, strNewPChannel.GetLength() ) == 0 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastAPath );
			}
			else if( _tcsnicmp( strNewPChannel, m_strBroadcastGrp, strNewPChannel.GetLength() ) == 0 )
			{
				m_editPChannel.SetWindowText( m_strBroadcastGrp );
			}
			else
			{
				// Convert from text to dword;
				BOOL fTransSucceeded;
				int nPChannel = GetDlgItemInt( IDC_PART_EDIT_PCHANNEL, &fTransSucceeded, FALSE );
				ASSERT( fTransSucceeded );
				/*
				if ( !fTransSucceeded || (nPChannel < 1) )
				{
					m_dwPChannel = 0;
					m_strPChannelName = GetPChannelName( m_dwPChannel );
					UpdatePPO();

					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( m_strPChannelName );
					}
				}
				else */ if( unsigned(nPChannel - 1) != PRD.dwPChannel )
				{
					PRD.dwPChannel = nPChannel - 1;
					PRD.cstrPChannelName = UpdatePChannelName( PRD.dwPChannel );
					m_pPPO->SetData( (void*)&PRD );
					// Changing a PChannel may cause this property sheet to be removed
					// so don't assume controls are still hanging around
					if( ::IsWindow(m_editPChannelName.m_hWnd) )
					{
						m_editPChannelName.SetWindowText( PRD.cstrPChannelName );
					}
				}
				m_editPChannelName.EnableWindow( TRUE );
			}
		}
	}
}

void PropPianoRoll::InitializePRData( PianoRollData* pPRD ) 
{
	pPRD->dwPageIndex = 2;
	m_pPPO->GetData( (void**)&pPRD );
}
