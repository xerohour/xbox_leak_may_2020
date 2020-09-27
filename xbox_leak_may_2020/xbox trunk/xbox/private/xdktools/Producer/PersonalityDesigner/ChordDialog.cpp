// ChordDialog.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "PersonalityDesigner.h"
#pragma warning(default:4201)
#include "ChordDialog.h"
#include "PersonalityCtl.h"
#include "ChordMapStripMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void SetFocusCallback(long hint)
{
	((CPersonalityCtrl*)hint)->SetLastFocus(CPersonalityCtrl::ChordMap);
}

/////////////////////////////////////////////////////////////////////////////
// CChordDialog dialog


CChordDialog::CChordDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CChordDialog::IDD, pParent)
{
	m_pITimeline = NULL;
	m_pPersonality = NULL;
	m_pChordMapStrip = NULL;
	EnableAutomation();

	//{{AFX_DATA_INIT(CChordDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CChordDialog::~CChordDialog()
{
	ASSERT( m_pITimeline == NULL );
	ASSERT( m_pPersonality == NULL );
	ASSERT( m_pChordMapStrip == NULL );
}

void CChordDialog::OnFinalRelease()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void CChordDialog::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChordDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChordDialog, CDialog)
	//{{AFX_MSG_MAP(CChordDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CChordDialog, CDialog)
	//{{AFX_DISPATCH_MAP(CChordDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IChordDialog to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {2B651782-E908-11D0-9EDC-00AA00A21BA9}
static const IID IID_IChordDialog =
{ 0x2b651782, 0xe908, 0x11d0, { 0x9e, 0xdc, 0x0, 0xaa, 0x0, 0xa2, 0x1b, 0xa9 } };

BEGIN_INTERFACE_MAP(CChordDialog, CDialog)
	INTERFACE_PART(CChordDialog, IID_IChordDialog, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChordDialog message handlers

BOOL CChordDialog::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	// Load control's accelerator table
	m_hAcceleratorTable = ::LoadAccelerators( theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1) );

	// Initialize the Timeline
	CWnd *pWnd = GetDlgItem(IDC_TIMELINECTL);
	if( pWnd == NULL ) return FALSE;

	IUnknown *punk = pWnd->GetControlUnknown();
	if( punk == NULL ) return FALSE;

	if( (FAILED(punk->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pITimeline ))) ||
		(m_pITimeline == NULL) )
	{
		return FALSE;
	}

	// Set Timeline properties and then Add the Strip Manager
	ASSERT( m_pPersonality->m_punkChordMapMgr != NULL );
	VARIANT vtInit;
	V_UNKNOWN(&vtInit) = m_pPersonality->m_pComponent->m_pIFramework;
	vtInit.vt = VT_UNKNOWN;
	m_pITimeline->SetTimelineProperty( TP_DMUSPRODFRAMEWORK, vtInit );

	// set the zoom factor
	vtInit.vt = VT_R8;
	V_R8(&vtInit) = m_pPersonality->ZoomFactor();
	m_pITimeline->SetTimelineProperty(TP_ZOOM, vtInit);

	// set the functionbar width
	vtInit.vt = VT_I4;
	V_I4(&vtInit) = m_pPersonality->FunctionbarWidth();
	m_pITimeline->SetTimelineProperty(TP_FUNCTIONBAR_WIDTH, vtInit);

	// give personality a pointer to me
	m_pPersonality->m_pChordDlg = this;

	// Initialize the ChordStrip Manager.
	IDMUSProdStripMgr* pStripMgr;
	punk = m_pPersonality->m_punkChordMapMgr;
	if( SUCCEEDED( punk->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgr )))
	{
		m_pITimeline->AddStripMgr( pStripMgr, 1 );
		IChordMapMgr *pIChordMapMgr;
		if( SUCCEEDED( pStripMgr->QueryInterface( IID_IChordMapMgr, (void**)&pIChordMapMgr ) ) )
		{
			if( SUCCEEDED( pIChordMapMgr->CreateChordMapStrip( &m_pChordMapStrip )))
			{
				m_pITimeline->AddStrip( m_pChordMapStrip );

				vtInit.vt = VT_BOOL;
				V_BOOL(&vtInit) = (short)m_pPersonality->VariableNotFixed();
				m_pChordMapStrip->SetStripProperty((STRIPPROPERTY)666, vtInit);

				// set focus call back
				vtInit.vt = VT_INT;
				V_INT(&vtInit) = (long)SetFocusCallback;
				m_pChordMapStrip->SetStripProperty((STRIPPROPERTY)(WM_USER+1), vtInit);
				vtInit.vt = VT_INT;
				V_INT(&vtInit) = (long)m_pPersonalityCtrl;
				m_pChordMapStrip->SetStripProperty((STRIPPROPERTY)(WM_USER+2), vtInit);

	//			m_pChordMapStrip->Release(); // Intentionally missing (needed for 'this' Strip reference).

				// Set the Timeline's callback member so we get callbacks when data changes
				// in the ChordMap Strip.
				IUnknown *punkPersonality;
				if( SUCCEEDED( m_pPersonality->QueryInterface( IID_IDMUSProdTimelineCallback, (void**)&punkPersonality )) )
				{
					vtInit.vt = VT_UNKNOWN;
					V_UNKNOWN(&vtInit) = punkPersonality;
					m_pITimeline->SetTimelineProperty( TP_TIMELINECALLBACK, vtInit );

					DMUS_TIMESIGNATURE timesig;
					timesig.mtTime = 0;
					timesig.bBeatsPerMeasure = static_cast<BYTE>(m_pPersonality->GetBPM());
					timesig.bBeat = static_cast<BYTE>(m_pPersonality->GetBeat());
					if(timesig.bBeat > 4)
					{
						// compound meter
						timesig.wGridsPerBeat = 3;
					}
					else
					{
						// simple meter
						timesig.wGridsPerBeat = 4;
					}

					m_pITimeline->SetParam(GUID_TimeSignature, 0xFFFFFFFF, 0, 0, static_cast<void*>(&timesig));


					punkPersonality->Release();
				}			
			}
			pIChordMapMgr->Release();
		}
		pStripMgr->Release();
	}



	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChordDialog::OnSize(UINT nType, int cx, int cy) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnSize(nType, cx, cy);
	

	CWnd *pWnd = GetDlgItem( IDC_TIMELINECTL );
	if( pWnd != NULL )
	{
		pWnd->MoveWindow( 0, 0, cx, cy );

		// now resize my window
		RECT rect;

		pWnd->GetClientRect(&rect);
		pWnd->CalcWindowRect(&rect);
		CalcWindowRect(&rect);
		MoveWindow(&rect);		
	}	
}

void CChordDialog::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// persist zoom factor to personality
	VARIANT var;
	m_pITimeline->GetTimelineProperty(TP_ZOOM, &var);
	double zoom = V_R8(&var);
	if(zoom != m_pPersonality->ZoomFactor())
	{
		m_pPersonality->ZoomFactor() = zoom;
		m_pPersonality->Modified() = TRUE;
	}

	m_pITimeline->GetTimelineProperty(TP_FUNCTIONBAR_WIDTH, &var);
	const long lFunctionbarWidth = V_I4(&var);
	if( lFunctionbarWidth != m_pPersonality->FunctionbarWidth() )
	{
		m_pPersonality->FunctionbarWidth() = lFunctionbarWidth;
		m_pPersonality->Modified() = TRUE;
	}

	// tell personality we're no longer around
	m_pPersonality->m_pChordDlg = 0;

	// Release the Timeline/CPersonality callback mechanism.
	VARIANT vtInit;
	V_UNKNOWN(&vtInit) = NULL;
	vtInit.vt = VT_UNKNOWN;
	m_pITimeline->SetTimelineProperty( TP_TIMELINECALLBACK, vtInit );

	RELEASE( m_pITimeline );
	m_pITimeline = NULL;

	RELEASE( m_pPersonality );
	m_pPersonality = NULL;

	m_pPersonalityCtrl = NULL;

	RELEASE( m_pChordMapStrip );
	m_pChordMapStrip = NULL;

	if( m_hAcceleratorTable )
	{
		::DestroyAcceleratorTable( m_hAcceleratorTable );
		m_hAcceleratorTable = NULL;
	}
	

	CDialog::OnDestroy();
}

void CChordDialog::InitializeDialog( CPersonality* pPersonality, CPersonalityCtrl* const pCtl )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(pPersonality);
	ASSERT(pCtl);

	ASSERT( m_pPersonality == NULL );
	m_pPersonality = pPersonality;
	m_pPersonality->AddRef();

	m_pPersonalityCtrl = pCtl;

	// this could go here or in CChordDialog as long as the two are opened at same time
	// basically, we need a place to initialize the style reference
	pPersonality->InitializeStyleReference();
	pPersonality->SyncPersonalityWithEngine(CPersonality::syncAudition);
}



BOOL CChordDialog::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	::TranslateAccelerator(m_hWnd, m_hAcceleratorTable, pMsg);
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CChordDialog::Refresh(bool isStructual)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT(m_pChordMapStrip && m_pITimeline);
	if(isStructual)
	{
		KillProps();
	}
	m_pITimeline->StripInvalidateRect(m_pChordMapStrip, NULL, TRUE);
	m_pITimeline->Refresh();
}

void CChordDialog::KillProps()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_pChordMapStrip->OnWMMessage(WM_COMMAND, IDM_KILLPROPS,  0,0,0);
}

void CChordDialog::ClearSelections()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_pChordMapStrip->OnWMMessage(WM_COMMAND, IDM_CLEARSELECT,  0,0,0);
}
