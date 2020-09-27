// AudioPathCtl.cpp : Implementation of the CAudioPathCtrl ActiveX Control class.

#include "stdafx.h"
#include "AudioPathDesignerDLL.h"
#include "AudioPath.h"
#include "AudioPathCtl.h"
#include "AudioPathRef.h"
#include "AudioPathDlg.h"
#include "EffectListDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CAudioPathCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CAudioPathCtrl, COleControl)
	//{{AFX_MSG_MAP(CAudioPathCtrl)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_ABOUT, AboutBox)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, OnUpdateEditInsert)
	ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(IDM_HELP_FINDER, OnHelpFinder)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CAudioPathCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CAudioPathCtrl)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CAudioPathCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CAudioPathCtrl, COleControl)
	//{{AFX_EVENT_MAP(CAudioPathCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl Interface map

BEGIN_INTERFACE_MAP(CAudioPathCtrl, COleControl)
    INTERFACE_PART(CAudioPathCtrl, IID_IOleInPlaceActiveObject, MyOleInPlaceActiveObject)
    INTERFACE_PART(CAudioPathCtrl, IID_IDMUSProdEditor, Editor)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CAudioPathCtrl, 1)
//	PROPPAGEID(CAudioPathPropPage::guid)
END_PROPPAGEIDS(CAudioPathCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CAudioPathCtrl, "AUDIOPATHDESIGNER.AudioPathCtrl.1", 
	0xC466D59C, 0xCD47, 0x4b38, 0x99, 0x64, 0x4d, 0xe3, 0x73, 0xaf, 0x48, 0x30);


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CAudioPathCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DAudioPath = 
		{ 0xC08121D8, 0x06A9, 0x4e07, { 0x80, 0x32, 0x5f, 0x11, 0x51, 0x86, 0x53, 0xbd } };
const IID BASED_CODE IID_DAudioPathEvents =
		{ 0xA1C0DB37, 0x2e71, 0x48d8, { 0xbd, 0x15, 0x4a, 0xe9, 0x41, 0xd8, 0x21, 0xe2 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwAudioPathOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CAudioPathCtrl, IDS_AUDIOPATH, _dwAudioPathOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::CAudioPathCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CAudioPathCtrl

BOOL CAudioPathCtrl::CAudioPathCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_AUDIOPATH,
			IDB_AUDIOPATH,
			afxRegApartmentThreading,
			_dwAudioPathOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::CAudioPathCtrl - Constructor

CAudioPathCtrl::CAudioPathCtrl()
{
/////////////////////////////////////////////////////////////////////////
// Following commented out to prevent 3 ASSERTS that occur
// because we are not building a typelib
//	InitializeIIDs(&IID_DAudioPath, &IID_DAudioPathEvents);
//
// Following code copied from InitializeIIDs (since we cannot override)
	m_piidPrimary = &IID_DAudioPath;
	m_piidEvents = &IID_DAudioPathEvents;
	EnableTypeLib();	// Needed to prevent ASSERT when closing editor
	InitStockEventMask();
	InitStockPropMask();
//////////////////////////////////////////////////////////////////////////

	m_pAudioPathDlg = NULL;
	m_pEffectListDlg = NULL;
	m_hWndContainer = NULL;
	m_hMenuInPlace = NULL;
	m_pAudioPath = NULL;
	m_hAcceleratorTable = NULL;
	m_dlgFocus = FOC_NONE;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::~CAudioPathCtrl - Destructor

CAudioPathCtrl::~CAudioPathCtrl()
{
	RELEASE( m_pAudioPath );
	if( m_pAudioPathDlg )
	{
		delete m_pAudioPathDlg;
		m_pAudioPathDlg = NULL;
	}
	if( m_pEffectListDlg )
	{
		delete m_pEffectListDlg;
		m_pEffectListDlg = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl IDMUSProdEditor implementation

/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XEditor::AddRef

STDMETHODIMP_(ULONG) CAudioPathCtrl::XEditor::AddRef()
{
	METHOD_PROLOGUE_EX_( CAudioPathCtrl, Editor )

	return (ULONG)pThis->ExternalAddRef();
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XEditor::Release

STDMETHODIMP_(ULONG) CAudioPathCtrl::XEditor::Release()
{
	METHOD_PROLOGUE_EX_( CAudioPathCtrl, Editor )

	return (ULONG)pThis->ExternalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XEditor::QueryInterface

STDMETHODIMP CAudioPathCtrl::XEditor::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_PROLOGUE_EX_( CAudioPathCtrl, Editor )

	return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XEditor::AttachObjects implementation

HRESULT CAudioPathCtrl::XEditor::AttachObjects( IDMUSProdNode* pINode )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, Editor )

	ASSERT_VALID( pThis );
	ASSERT( pINode != NULL );

	GUID guidNode;

	pINode->GetNodeId( &guidNode );
	if( ::IsEqualGUID( guidNode, GUID_AudioPathRefNode ) )
	{
		CAudioPathRef* pAudioPathRef = (CAudioPathRef *)pINode;
		ASSERT( pAudioPathRef->m_pAudioPath != NULL );
		pThis->m_pAudioPath = pAudioPathRef->m_pAudioPath;
	}
	else
	{
		pThis->m_pAudioPath = (CDirectMusicAudioPath *)pINode;
	}

	ASSERT( pThis->m_pAudioPath != NULL );
	pThis->m_pAudioPath->AddRef();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XEditor::OnInitMenuFilePrint

HRESULT CAudioPathCtrl::XEditor::OnInitMenuFilePrint( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XEditor::OnFilePrint

HRESULT CAudioPathCtrl::XEditor::OnFilePrint( void )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XEditor::OnInitMenuFilePrintPreview

HRESULT CAudioPathCtrl::XEditor::OnInitMenuFilePrintPreview( HMENU hMenu, UINT nMenuId )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, Editor )

	::EnableMenuItem( hMenu, nMenuId, (MF_GRAYED | MF_BYCOMMAND) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XEditor::OnFilePrintPreview

HRESULT CAudioPathCtrl::XEditor::OnFilePrintPreview( void )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, Editor )

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XEditor::OnViewProperties

HRESULT CAudioPathCtrl::XEditor::OnViewProperties( void )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, Editor )

	HRESULT hr = E_FAIL;

	if( pThis->m_pAudioPathDlg )
	{
		if( pThis->m_pAudioPathDlg->OnViewProperties() )
		{
			hr = S_OK;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XEditor::OnF1Help

HRESULT CAudioPathCtrl::XEditor::OnF1Help( void )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, Editor )
	
    // Determine name of DMUSProd.exe help file
	CString strHelpFileName;

	if( theApp.GetHelpFileName( strHelpFileName ) )
	{
		strHelpFileName += "::/htm/AudioPathDesignerWindow.htm";
		::HtmlHelp( NULL, strHelpFileName, HH_DISPLAY_TOPIC, 0 );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::XMyOleInPlaceActiveObject implementation

STDMETHODIMP_(ULONG) CAudioPathCtrl::XMyOleInPlaceActiveObject::AddRef()
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.AddRef();
}

STDMETHODIMP_(ULONG) CAudioPathCtrl::XMyOleInPlaceActiveObject::Release()
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.Release();
}

STDMETHODIMP CAudioPathCtrl::XMyOleInPlaceActiveObject::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.QueryInterface( iid, ppvObj );
}

STDMETHODIMP CAudioPathCtrl::XMyOleInPlaceActiveObject::GetWindow( HWND* lphwnd )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.GetWindow( lphwnd );
}

STDMETHODIMP CAudioPathCtrl::XMyOleInPlaceActiveObject::ContextSensitiveHelp( BOOL fEnterMode )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.ContextSensitiveHelp( fEnterMode );
}

STDMETHODIMP CAudioPathCtrl::XMyOleInPlaceActiveObject::TranslateAccelerator( LPMSG lpmsg )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );
	
	// Translate only if we have focus
	if( pThis->m_pAudioPathDlg && pThis->m_pEffectListDlg )
	{
		const HWND hWndMixGroup = pThis->m_pAudioPathDlg->GetSafeHwnd();
		const HWND hWndEffectList = pThis->m_pEffectListDlg->GetSafeHwnd();
		const HWND hWndFocus = ::GetFocus();
		const HWND hWndFocusParent = ::GetParent( hWndFocus );

		// Only check one level, so that we don't try and translate accelerators
		// in the edit control in the PChannel tree (used for editing the Mix
		// Group name).
		if( hWndFocus == hWndMixGroup
		||	hWndFocus == hWndEffectList
		||	hWndFocusParent == hWndMixGroup
		||	hWndFocusParent == hWndEffectList )
		{
			if( ::TranslateAccelerator(pThis->m_hWnd, pThis->m_hAcceleratorTable, lpmsg) )
			{
				return S_OK;
			}
		}
	}

	return pThis->m_xOleInPlaceActiveObject.TranslateAccelerator( lpmsg );
}

STDMETHODIMP CAudioPathCtrl::XMyOleInPlaceActiveObject::OnFrameWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnFrameWindowActivate( fActivate );
}

STDMETHODIMP CAudioPathCtrl::XMyOleInPlaceActiveObject::OnDocWindowActivate( BOOL fActivate )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.OnDocWindowActivate( fActivate );
}

STDMETHODIMP CAudioPathCtrl::XMyOleInPlaceActiveObject::ResizeBorder(
	LPCRECT, LPOLEINPLACEUIWINDOW, BOOL fFrameWindow )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

    if( fFrameWindow == TRUE )
	{
		pThis->OnShowToolBars();
	}

	return S_OK;
}

STDMETHODIMP CAudioPathCtrl::XMyOleInPlaceActiveObject::EnableModeless( BOOL fEnable )
{
	METHOD_MANAGE_STATE( CAudioPathCtrl, MyOleInPlaceActiveObject )
	ASSERT_VALID( pThis );

	return pThis->m_xOleInPlaceActiveObject.EnableModeless( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnDraw - Drawing function

void CAudioPathCtrl::OnDraw( CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid )
{
	UNREFERENCED_PARAMETER(pdc);
	UNREFERENCED_PARAMETER(rcBounds);
	UNREFERENCED_PARAMETER(rcInvalid);
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::DoPropExchange - Persistence support

void CAudioPathCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnResetState - Reset control to default state

void CAudioPathCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CAudioPathAbout dialog used for AudioPath Editor About Box

class CAudioPathAbout : public CDialog
{
public:
	CAudioPathAbout();

// Dialog Data
	//{{AFX_DATA(CAudioPathAbout)
	enum { IDD = IDD_ABOUTBOX_AUDIOPATH };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioPathAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAudioPathAbout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAudioPathAbout::CAudioPathAbout() : CDialog(CAudioPathAbout::IDD)
{
	//{{AFX_DATA_INIT(CAudioPathAbout)
	//}}AFX_DATA_INIT
}

void CAudioPathAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioPathAbout)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAudioPathAbout, CDialog)
	//{{AFX_MSG_MAP(CAudioPathAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CAudioPathAbout::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Get version information
	TCHAR achExeName[FILENAME_MAX + 1];
	TCHAR achFileVersion[MID_BUFFER];

	if( GetModuleFileName ( theApp.m_hInstance, achExeName, FILENAME_MAX ) )
	{
		if( theApp.GetFileVersion( achExeName, achFileVersion, MID_BUFFER ) )
		{
			CString strFileVersion;

			AfxFormatString1( strFileVersion, IDS_AUDIOPATH_VERSION_TEXT, achFileVersion );
			SetDlgItemText( IDC_FILE_VERSION, strFileVersion );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::AboutBox - Display an "About" box to the user

void CAudioPathCtrl::AboutBox()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CAudioPathAbout dlgAbout;
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl message handlers

/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnHideToolBars

void CAudioPathCtrl::OnHideToolBars() 
{
	COleControl::OnHideToolBars();
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnShowToolBars

void CAudioPathCtrl::OnShowToolBars() 
{
	if( m_pInPlaceFrame == NULL )
	{
		return;
	}

	m_pInPlaceFrame->SetActiveObject( &m_xMyOleInPlaceActiveObject, NULL );

	if( m_pInPlaceDoc != NULL )
	{
		m_pInPlaceDoc->SetActiveObject( &m_xMyOleInPlaceActiveObject, NULL );
	}

	COleControl::OnShowToolBars();
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnGetInPlaceMenu

HMENU CAudioPathCtrl::OnGetInPlaceMenu() 
{
	return m_hMenuInPlace;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnCreate

int CAudioPathCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( COleControl::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}

	if( m_pInPlaceFrame )
	{
		m_pInPlaceFrame->GetWindow( &m_hWndContainer );
	}
	else
	{
		// out of place active?
		if( m_bOpen )
		{
			m_hWndContainer = GetParent()->GetSafeHwnd();
		}
	}

	// Load control's in-place menu
	m_hMenuInPlace = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDR_AUDIOPATH_EDITOR) );

	// Load control's accelerator table
	m_hAcceleratorTable = ::LoadAccelerators( theApp.m_hInstance, MAKEINTRESOURCE(IDR_AUDIOPATH_ACCELS) );

	// Create the Splitter and position it in the Editor
	// (temporary values added until coords are saved to design-time file)
	RECT rect;
	GetClientRect(&rect);
	rect.left = lpCreateStruct->cx;
	rect.right = rect.left + DefaultSplitterWidth;
	rect.bottom = lpCreateStruct->cy;
	rect.top = lpCreateStruct->y;

	m_wndSplitter.Create( (CWnd*)this, &rect );

	// Create the PChannel dialog and position it in the First Splitter Pane
	m_pAudioPathDlg = new CAudioPathDlg;
	if( m_pAudioPathDlg == NULL )
	{
		return -1;
	}

	// Point the AudioPath dialog back to us and at the AudioPath object
	m_pAudioPathDlg->m_pAudioPath = m_pAudioPath;
	m_pAudioPathDlg->m_pAudioPathCtrl = this;
	m_pAudioPathDlg->Create( IDD_DLG_AUDIOPATH, this );
	m_pAudioPathDlg->ShowWindow( SW_SHOW );
	m_wndSplitter.SetFirstPane( m_pAudioPathDlg );

	// Create the effect list and position it in the Second Splitter Pane
	m_pEffectListDlg = new CEffectListDlg;
	if( m_pEffectListDlg == NULL )
	{
		return -1;
	}

	// Point the EffectList dialog back to us and at the AudioPath object
	m_pEffectListDlg->m_pAudioPath = m_pAudioPath;
	m_pEffectListDlg->m_pAudioPathCtrl = this;
	m_pEffectListDlg->Create( IDD_DLG_EFFECTLIST, this );
	m_pEffectListDlg->ShowWindow( SW_SHOW );
	m_wndSplitter.SetSecondPane( m_pEffectListDlg );

	// Point the AudioPath at us
	m_pAudioPath->m_pAudioPathCtrl = this;

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnDestroy

void CAudioPathCtrl::OnDestroy() 
{
	COleControl::OnDestroy();

	if( m_hMenuInPlace )
	{
		::DestroyMenu( m_hMenuInPlace );
		m_hMenuInPlace = NULL;
	}

	if( m_hAcceleratorTable )
	{
		::DestroyAcceleratorTable( m_hAcceleratorTable );
		m_hAcceleratorTable = NULL;
	}

	ASSERT( m_pAudioPath != NULL );

	m_pAudioPath->m_pAudioPathCtrl = NULL;

	// Delete the dialog windows in the destructor, not here!
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnSize

void CAudioPathCtrl::OnSize( UINT nType, int cx, int cy ) 
{
	COleControl::OnSize( nType, cx, cy );

	// get splitter position from audio path
	WORD pos = m_pAudioPath->GetSplitterPos();

	// calculate where splitter should be
	if(pos == 0)
	{
		// Don't update the AudioPath if the window width is 200, since that is
		// the size set before the window is initialized
		if( cx != 200 )
		{
			// Calculate size of effect palette columns
			if( m_pEffectListDlg )
			{
				int nWidth = m_pEffectListDlg->m_listEffects.GetColumnWidth( 0 );

				// If the effect palette is less than half the width of the display
				if( nWidth < (cx - DefaultSplitterWidth)/2 )
				{
					// Give the effect palette just enough space to display itself
					pos = WORD(min( USHRT_MAX, nWidth ));
				}
			}

			// If the effect pallete is more than half the width of the display,
			// or if there is no effect palette
			if( pos == 0 )
			{
				// put it right in the middle
				pos = WORD(min( USHRT_MAX, (cx - DefaultSplitterWidth)/2 ));
			}

			// tell audio path where it is so it can save it
			m_pAudioPath->SetSplitterPos(pos, false);
		}
	}

	// Update splitter position, if it's non-zero and the width is not 200
	if( (pos != 0) || (cx != 200) )
	{
		// splitter is right side of chord dialog and left side
		// of signpost dialog
		RECT rect;
		GetClientRect(&rect);
		MoveWindow(0,0,cx,cy);
		rect.right = max( DefaultSplitterWidth, cx - pos );
		rect.left = rect.right - DefaultSplitterWidth;
		m_wndSplitter.SetTracker(rect);
		m_wndSplitter.MoveWindow( &rect, TRUE );
		m_pAudioPathDlg->MoveWindow(0,0,rect.left, rect.bottom);
		m_pEffectListDlg->MoveWindow(rect.right, 0, cx - rect.right, rect.bottom);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnUpdateEditUndo

void CAudioPathCtrl::OnUpdateEditUndo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	CString strUndo;
	CString strUndoAccel;

	strUndo.LoadString( IDS_UNDO );
	strUndoAccel.LoadString( IDS_UNDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pAudioPath  )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pAudioPath->m_pUndoMgr != NULL );

		if( m_pAudioPath->m_pUndoMgr )
		{
			if( m_pAudioPath->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
			{
				if( *achText )
				{
					strUndo += " ";
					strUndo += achText;
				}

				fEnable = TRUE;
			}
		}
	}

	strUndo += strUndoAccel;
	pCmdUI->SetText( strUndo );
	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnEditUndo

void CAudioPathCtrl::OnEditUndo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	if( m_pAudioPath )
	{
		ASSERT( m_pAudioPath->m_pUndoMgr != NULL );

		if( m_pAudioPath->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pAudioPath->m_pUndoMgr->GetUndo(achText, MAX_BUFFER) )
			{
				m_pAudioPath->m_pUndoMgr->Undo( m_pAudioPath );
				m_pAudioPath->SetModified( TRUE );

				// Get a pointer to the property sheet
				IDMUSProdPropSheet* pIPropSheet;
				if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
				{
					// Check if property sheet is visible
					if( pIPropSheet->IsShowing() == S_OK )
					{
						SwitchToCorrectPropertyPage();
					}

					pIPropSheet->Release();
				}

				m_pAudioPath->SyncAudiopathsInUse( AUDIOPATH_UNLOAD_DOWNLOAD_WAVES );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnUpdateEditRedo

void CAudioPathCtrl::OnUpdateEditRedo( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	CString strRedo;
	CString strRedoAccel;

	strRedo.LoadString( IDS_REDO );
	strRedoAccel.LoadString( IDS_REDO_ACCEL );

	BOOL fEnable = FALSE;

	if( m_pAudioPath )
	{
		TCHAR achText[MAX_BUFFER];

		ASSERT( m_pAudioPath->m_pUndoMgr != NULL );

		if( m_pAudioPath->m_pUndoMgr )
		{
			if( m_pAudioPath->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
			{
				if( *achText )
				{
					strRedo += " ";
					strRedo += achText;
				}

				fEnable = TRUE;
			}
		}
	}

	strRedo += strRedoAccel;
	pCmdUI->SetText( strRedo );
	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnEditRedo

void CAudioPathCtrl::OnEditRedo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pAudioPath != NULL );

	if( m_pAudioPath )
	{
		ASSERT( m_pAudioPath->m_pUndoMgr != NULL );

		if( m_pAudioPath->m_pUndoMgr )
		{
			TCHAR achText[MAX_BUFFER];

			if( m_pAudioPath->m_pUndoMgr->GetRedo(achText, MAX_BUFFER) )
			{
				m_pAudioPath->m_pUndoMgr->Redo( m_pAudioPath );
				m_pAudioPath->SetModified( TRUE );

				// Get a pointer to the property sheet
				IDMUSProdPropSheet* pIPropSheet;
				if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
				{
					// Check if property sheet is visible
					if( pIPropSheet->IsShowing() == S_OK )
					{
						SwitchToCorrectPropertyPage();
					}

					pIPropSheet->Release();
				}

				m_pAudioPath->SyncAudiopathsInUse( AUDIOPATH_UNLOAD_DOWNLOAD_WAVES );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnUpdateEditCut

void CAudioPathCtrl::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnUpdateEditCut( pCmdUI );
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnUpdateEditCut( pCmdUI );
			return;
		}
		break;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnEditCut

void CAudioPathCtrl::OnEditCut() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnEditCut();
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnEditCut();
			return;
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnUpdateEditCopy

void CAudioPathCtrl::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnUpdateEditCopy( pCmdUI );
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnUpdateEditCopy( pCmdUI );
			return;
		}
		break;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnEditCopy

void CAudioPathCtrl::OnEditCopy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnEditCopy();
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnEditCopy();
			return;
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnUpdateEditPaste

void CAudioPathCtrl::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnUpdateEditPaste( pCmdUI );
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnUpdateEditPaste( pCmdUI );
			return;
		}
		break;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnEditPaste

void CAudioPathCtrl::OnEditPaste() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnEditPaste();
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnEditPaste();
			return;
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnUpdateEditInsert

void CAudioPathCtrl::OnUpdateEditInsert( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnUpdateEditInsert( pCmdUI );
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnUpdateEditInsert( pCmdUI );
			return;
		}
		break;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnEditInsert

void CAudioPathCtrl::OnEditInsert() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnEditInsert();
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnEditInsert();
			return;
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnUpdateEditDelete

void CAudioPathCtrl::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnUpdateEditDelete( pCmdUI );
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnUpdateEditDelete( pCmdUI );
			return;
		}
		break;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnEditDelete

void CAudioPathCtrl::OnEditDelete() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnEditDelete();
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnEditDelete();
			return;
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnUpdateEditSelectAll

void CAudioPathCtrl::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnUpdateEditSelectAll( pCmdUI );
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnUpdateEditSelectAll( pCmdUI );
			return;
		}
		break;
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnEditSelectAll

void CAudioPathCtrl::OnEditSelectAll() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		if( m_pAudioPathDlg )
		{
			m_pAudioPathDlg->OnEditSelectAll();
			return;
		}
		break;
	case FOC_EFFECT:
		if( m_pEffectListDlg )
		{
			m_pEffectListDlg->OnEditSelectAll();
			return;
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::OnHelpFinder

void CAudioPathCtrl::OnHelpFinder() 
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


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::EndTrack

void CAudioPathCtrl::EndTrack( int nWidth )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	RECT rect;
	GetWindowRect( &rect );

	// tell AudioPath the new splitter width
	m_pAudioPath->SetSplitterPos( WORD(min( USHRT_MAX, max( 0, rect.right - rect.left - nWidth - DefaultSplitterWidth) )) );

	CalcWindowRect( &rect );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::SetLastFocus

void CAudioPathCtrl::SetLastFocus(DialogFocus dlgFocus)
{
	m_dlgFocus = dlgFocus;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathCtrl::SwitchToCorrectPropertyPage

void CAudioPathCtrl::SwitchToCorrectPropertyPage( void )
{
	IDMUSProdPropSheet* pIPropSheet;

	switch( m_dlgFocus )
	{
	case FOC_MIXGROUP:
		m_pAudioPathDlg->SwitchToCorrectPropertyPage();
		break;

	default:
		// Show the AudioPath properties
		if( SUCCEEDED ( theApp.m_pAudioPathComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			short nActiveTab = CAudioPathPropPageManager::sm_nActiveTab;

			if( SUCCEEDED ( pIPropSheet->SetPageManager(theApp.m_pAudioPathComponent->m_pIAudioPathPageManager) ) )
			{
				theApp.m_pAudioPathComponent->m_pIAudioPathPageManager->SetObject( m_pAudioPath );
				pIPropSheet->SetActivePage( nActiveTab ); 
			}

			pIPropSheet->Release();
		}
		break;
	}
}
