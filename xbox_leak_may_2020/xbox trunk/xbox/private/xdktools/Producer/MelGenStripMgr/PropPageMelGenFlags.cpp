// PropPageMelGenFlags.cpp : implementation file
//

#include "stdafx.h"
#include "PropPageMelGenFlags.h"
#include <dmusici.h>
#include "dmusprod.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPageMelGenFlags property page

short* CPropPageMelGenFlags::sm_pnActiveTab = NULL;

IMPLEMENT_DYNCREATE(CPropPageMelGenFlags, CPropertyPage)

CPropPageMelGenFlags::CPropPageMelGenFlags() : CPropertyPage(CPropPageMelGenFlags::IDD)
{
	//{{AFX_DATA_INIT(CPropPageMelGenFlags)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPPO = NULL;
	m_PPGTrackParams.dwPageIndex = 2;
	m_PPGTrackParams.dwPlayMode = DMUS_PLAYMODE_NONE;
	m_fNeedToDetach = FALSE;
}

CPropPageMelGenFlags::~CPropPageMelGenFlags()
{
	if( m_pPPO )
	{
		m_pPPO->Release();
		m_pPPO = NULL;
	}
}

void CPropPageMelGenFlags::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPageMelGenFlags)
	DDX_Control(pDX, IDC_COMBO_PLAYMODE_LIST, m_PlaymodeDropDownList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPageMelGenFlags, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPageMelGenFlags)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_PLAYMODE_LIST, OnSelchangeComboPlaymodeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void CPropPageMelGenFlags::SetObject( IDMUSProdPropPageObject* pPPO )
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
	m_pPPO = pPPO;
	if( m_pPPO )
	{
		m_pPPO->AddRef();
	}
}

void CPropPageMelGenFlags::EnableControls( BOOL fEnable ) 
{
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	if(IsWindow(m_PlaymodeDropDownList.m_hWnd))
	{
		m_PlaymodeDropDownList.EnableWindow(fEnable);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageMelGenFlags message handlers

int CPropPageMelGenFlags::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CPropPageMelGenFlags::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

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

	CPropertyPage::OnDestroy();
}

void CPropPageMelGenFlags::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	ioMelGenFlagsPPG *pioMelGenFlagsPPG = &m_PPGTrackParams;
	if( FAILED( m_pPPO->GetData( (void**)&pioMelGenFlagsPPG ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	// Prevent control notifications from being dispatched during UpdateData
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;

	EnableControls( TRUE );

	switch (m_PPGTrackParams.dwPlayMode)
	{
	case DMUS_PLAYMODE_FIXED:
		m_PlaymodeDropDownList.SetCurSel(0);
		break;
	case DMUS_PLAYMODE_FIXEDTOCHORD:
		m_PlaymodeDropDownList.SetCurSel(1);
		break;
	case DMUS_PLAYMODE_FIXEDTOKEY:
		m_PlaymodeDropDownList.SetCurSel(2);
		break;
	case DMUS_PLAYMODE_MELODIC:
		m_PlaymodeDropDownList.SetCurSel(3);
		break;
	case DMUS_PLAYMODE_NORMALCHORD:
		m_PlaymodeDropDownList.SetCurSel(4);
		break;
	case DMUS_PLAYMODE_ALWAYSPLAY:
		m_PlaymodeDropDownList.SetCurSel(5);
		break;
	case DMUS_PLAYMODE_PEDALPOINT:
		m_PlaymodeDropDownList.SetCurSel(6);
		break;
	case DMUS_PLAYMODE_PEDALPOINTCHORD:
		m_PlaymodeDropDownList.SetCurSel(7);
		break;
	case DMUS_PLAYMODE_PEDALPOINTALWAYS:
		m_PlaymodeDropDownList.SetCurSel(8);
		break;
	}

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;

}

BOOL CPropPageMelGenFlags::OnSetActive() 
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

void CPropPageMelGenFlags::UpdatePPO()
{
	if( m_pPPO )
	{
		VERIFY( SUCCEEDED( m_pPPO->SetData(&m_PPGTrackParams) ) );
	}
}

void CPropPageMelGenFlags::OnSelchangeComboPlaymodeList() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	switch (m_PlaymodeDropDownList.GetCurSel())
	{
	case 0:
		m_PPGTrackParams.dwPlayMode = 0;
		break;
	case 1:
		m_PPGTrackParams.dwPlayMode = DMUS_PLAYMODE_FIXEDTOCHORD;
		break;
	case 2:
		m_PPGTrackParams.dwPlayMode = DMUS_PLAYMODE_FIXEDTOKEY;
		break;
	case 3:
		m_PPGTrackParams.dwPlayMode = DMUS_PLAYMODE_MELODIC;
		break;
	case 4:
		m_PPGTrackParams.dwPlayMode = DMUS_PLAYMODE_NORMALCHORD;
		break;
	case 5:
		m_PPGTrackParams.dwPlayMode = DMUS_PLAYMODE_ALWAYSPLAY;
		break;
	case 6:
		m_PPGTrackParams.dwPlayMode = DMUS_PLAYMODE_PEDALPOINT;
		break;
	case 7:
		m_PPGTrackParams.dwPlayMode = DMUS_PLAYMODE_PEDALPOINTCHORD;
		break;
	case 8:
		m_PPGTrackParams.dwPlayMode = DMUS_PLAYMODE_PEDALPOINTALWAYS;
		break;
	}

	UpdatePPO();
}
