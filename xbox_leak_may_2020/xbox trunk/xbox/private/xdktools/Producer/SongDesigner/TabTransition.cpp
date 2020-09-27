// TabTransition.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"
#include "resource.h"

#include "Song.h"
#include "TransitionPropPageObject.h"
#include "TabTransition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabTransition property page

CTabTransition::CTabTransition( IDMUSProdPropPageManager* pIPageManager ) : CPropertyPage(CTabTransition::IDD)
{
	//{{AFX_DATA_INIT(CTabTransition)
	//}}AFX_DATA_INIT

	ASSERT( pIPageManager != NULL );

	m_pIPropPageObject = NULL;
	m_pIPageManager = pIPageManager;
	m_fNeedToDetach = FALSE;
}

CTabTransition::~CTabTransition()
{
	RELEASE( m_pIPropPageObject );
}


void CTabTransition::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabTransition)
	DDX_Control(pDX, IDC_STATIC_INTRO_PROMPT, m_staticIntroPrompt);
	DDX_Control(pDX, IDC_CHECK_INTRO, m_checkIntro);
	DDX_Control(pDX, IDC_COMBO_TO_SEGMENT, m_comboToSegment);
	DDX_Control(pDX, IDC_COMBO_TRANSITION_SEGMENT, m_comboTransitionSegment);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::SetObject

void CTabTransition::SetObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->Release();
	}
	m_pIPropPageObject = pIPropPageObject;
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->AddRef();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::UpdateObject

void CTabTransition::UpdateObject()
{
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->SetData( (void *)&m_PPGTabTransition );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::FillToSegmentComboBox

void CTabTransition::FillToSegmentComboBox( void )
{
	// Empty combo box
	m_comboToSegment.ResetContent();

	// Add "<Any>"
	CString strName;
	strName.LoadString( IDS_ANY_TEXT );
	int nPos = m_comboToSegment.AddString( strName  );
	m_comboToSegment.SetItemDataPtr( nPos, (void *)(NULL + 1) );

	// Add "<None>"
	strName.LoadString( IDS_NONE_TEXT );
	nPos = m_comboToSegment.AddString( strName  );
	m_comboToSegment.SetItemDataPtr( nPos, NULL );

	// Add VirtualSegments
	CVirtualSegment* pVirtualSegment;
	for( int i = 0 ;  pVirtualSegment = m_PPGTabTransition.pSong->IndexToVirtualSegment(i) ;  i++ )
	{
		if( pVirtualSegment == NULL )
		{
			break;
		}

		pVirtualSegment->GetName( strName );
		nPos = m_comboToSegment.AddString( strName  );
		m_comboToSegment.SetItemDataPtr( nPos, pVirtualSegment );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::FillTransitionSegmentComboBox

void CTabTransition::FillTransitionSegmentComboBox( void )
{
	// Empty combo box
	m_comboTransitionSegment.ResetContent();

	// Add "<None>"
	CString strName;
	strName.LoadString( IDS_NONE_TEXT );
	int nPos = m_comboTransitionSegment.AddString( strName  );
	m_comboTransitionSegment.SetItemDataPtr( nPos, NULL );

	// Add VirtualSegments
	CVirtualSegment* pVirtualSegment;
	for( int i = 0 ;  pVirtualSegment = m_PPGTabTransition.pSong->IndexToVirtualSegment(i) ;  i++ )
	{
		if( pVirtualSegment == NULL )
		{
			break;
		}

		pVirtualSegment->GetName( strName );
		nPos = m_comboTransitionSegment.AddString( strName  );
		m_comboTransitionSegment.SetItemDataPtr( nPos, pVirtualSegment );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::SelectToSegment

void CTabTransition::SelectToSegment( void )
{
	CVirtualSegment* pTheVirtualSegment;

	if( m_PPGTabTransition.dwToSegmentFlag == DMUS_SONG_NOSEG )
	{
		pTheVirtualSegment = (CVirtualSegment *)NULL;
	}
	else if( m_PPGTabTransition.dwToSegmentFlag == DMUS_SONG_ANYSEG )
	{
		pTheVirtualSegment = (CVirtualSegment *)(NULL + 1);
	}
	else if( m_PPGTabTransition.dwToSegmentFlag == DMUS_SONG_NOFROMSEG )
	{
		PPGTransition ppgTransition;
		PPGTransition* pPPGTransition = &ppgTransition;

		if( m_pIPropPageObject 
		&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pPPGTransition))) )
		{
			pTheVirtualSegment = ppgTransition.pVirtualSegmentToEdit;
		}
		else
		{
			ASSERT( 0 );	// Should not happen!
			pTheVirtualSegment = (CVirtualSegment *)NULL;
		}
	}
	else
	{
		pTheVirtualSegment = m_PPGTabTransition.pToSegment;
	}

	int nNbrItems = m_comboToSegment.GetCount();

	for( int nPos = 0 ;  nPos < nNbrItems ;  nPos++ )
	{
		CVirtualSegment* pVirtualSegmentList = (CVirtualSegment *)m_comboToSegment.GetItemDataPtr( nPos );

		if( pVirtualSegmentList == pTheVirtualSegment )
		{
			m_comboToSegment.SetCurSel( nPos );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::SelectTransitionSegment

void CTabTransition::SelectTransitionSegment( void )
{
	int nNbrItems = m_comboTransitionSegment.GetCount();

	for( int nPos = 0 ;  nPos < nNbrItems ;  nPos++ )
	{
		CVirtualSegment* pVirtualSegmentList = (CVirtualSegment *)m_comboTransitionSegment.GetItemDataPtr( nPos );

		if( pVirtualSegmentList == m_PPGTabTransition.pTransitionSegment )
		{
			m_comboTransitionSegment.SetCurSel( nPos );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::RefreshTab

void CTabTransition::RefreshTab( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	if( m_pIPropPageObject == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	PPGTabTransition *pPPGTabTransition = &m_PPGTabTransition;
	if( FAILED( m_pIPropPageObject->GetData( (void**)&pPPGTabTransition ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	if( !(m_PPGTabTransition.dwFlagsUI & PROPF_ONE_SELECTED) )
	{
		EnableControls( FALSE );
		return;
	}

	// Prevent control notifications from being dispatched during RefreshTab
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;

	EnableControls( TRUE );

	// "To" segment
	FillToSegmentComboBox();
	SelectToSegment();

	// "Transition" segment
	FillTransitionSegmentComboBox();
	SelectTransitionSegment();

	// "Intro" check box
	if( m_PPGTabTransition.dwToSegmentFlag == DMUS_SONG_NOFROMSEG )
	{
		m_checkIntro.SetCheck( 1 );
	}
	else
	{
		m_checkIntro.SetCheck( 0 );
	}

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::EnableControls

void CTabTransition::EnableControls( BOOL fEnable ) 
{
	if( m_PPGTabTransition.dwToSegmentFlag == DMUS_SONG_NOFROMSEG )
	{
		m_comboToSegment.EnableWindow( FALSE );
		m_staticIntroPrompt.ShowWindow( SW_NORMAL );
	}
	else
	{
		m_comboToSegment.EnableWindow( fEnable );
		m_staticIntroPrompt.ShowWindow( SW_HIDE );
	}
	
	m_comboTransitionSegment.EnableWindow( fEnable );
	m_checkIntro.EnableWindow( fEnable );
	m_staticIntroPrompt.EnableWindow( fEnable );

	if( fEnable == FALSE )
	{
		m_comboToSegment.ResetContent();
		m_comboTransitionSegment.ResetContent();
		m_checkIntro.SetCheck( 0 );
		m_staticIntroPrompt.ShowWindow( SW_HIDE );
	}
}


BEGIN_MESSAGE_MAP(CTabTransition, CPropertyPage)
	//{{AFX_MSG_MAP(CTabTransition)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_TO_SEGMENT, OnSelChangeComboToSegment)
	ON_CBN_SELCHANGE(IDC_COMBO_TRANSITION_SEGMENT, OnSelChangeComboTransitionSegment)
	ON_BN_CLICKED(IDC_CHECK_INTRO, OnCheckIntro)
	ON_BN_DOUBLECLICKED(IDC_CHECK_INTRO, OnDoubleClickedCheckIntro)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabTransition message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabTransition::OnSetActive

BOOL CTabTransition::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	RefreshTab();
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::OnDestroy

void CTabTransition::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::OnCreate

int CTabTransition::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::OnSelChangeComboToSegment

void CTabTransition::OnSelChangeComboToSegment( void ) 
{
	int nCurSel = m_comboToSegment.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		CVirtualSegment* pVirtualSegment = (CVirtualSegment *)m_comboToSegment.GetItemDataPtr( nCurSel );

		if( pVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
		{
			DWORD dwToSegmentFlag = 0;
			if( pVirtualSegment == (CVirtualSegment *)NULL )
			{
				pVirtualSegment = NULL;
				dwToSegmentFlag = DMUS_SONG_NOSEG;
			}
			else if( pVirtualSegment == (CVirtualSegment *)(NULL + 1) )
			{
				pVirtualSegment = NULL;
				dwToSegmentFlag = DMUS_SONG_ANYSEG;
			}
			if( m_PPGTabTransition.pToSegment != pVirtualSegment 
			||  m_PPGTabTransition.dwToSegmentFlag != dwToSegmentFlag )
			{
				m_PPGTabTransition.pToSegment = pVirtualSegment;
				m_PPGTabTransition.dwToSegmentFlag = dwToSegmentFlag;
				UpdateObject();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::OnSelChangeComboTransitionSegment

void CTabTransition::OnSelChangeComboTransitionSegment( void ) 
{
	int nCurSel = m_comboTransitionSegment.GetCurSel();
	if( nCurSel != CB_ERR )
	{
		CVirtualSegment* pVirtualSegment = (CVirtualSegment *)m_comboTransitionSegment.GetItemDataPtr( nCurSel );

		if( pVirtualSegment != (CVirtualSegment *)0xFFFFFFFF )
		{
			if( m_PPGTabTransition.pTransitionSegment != pVirtualSegment )
			{
				m_PPGTabTransition.pTransitionSegment = pVirtualSegment;
				UpdateObject();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::OnCheckIntro

void CTabTransition::OnCheckIntro() 
{
	CVirtualSegment* pVirtualSegment;
	DWORD dwToSegmentFlag;

	if( m_checkIntro.GetCheck() )
	{
		pVirtualSegment = NULL;
		dwToSegmentFlag = DMUS_SONG_NOFROMSEG;
		m_comboToSegment.EnableWindow( FALSE );
		m_staticIntroPrompt.ShowWindow( SW_NORMAL );
	}
	else
	{
		pVirtualSegment = NULL;
		dwToSegmentFlag = DMUS_SONG_NOSEG;
		m_comboToSegment.EnableWindow( TRUE );
		m_staticIntroPrompt.ShowWindow( SW_HIDE );
	}

	if( m_PPGTabTransition.pToSegment != pVirtualSegment 
	||  m_PPGTabTransition.dwToSegmentFlag != dwToSegmentFlag )
	{
		m_PPGTabTransition.pToSegment = pVirtualSegment;
		m_PPGTabTransition.dwToSegmentFlag = dwToSegmentFlag;
		UpdateObject();
		RefreshTab();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabTransition::OnDoubleClickedCheckIntro

void CTabTransition::OnDoubleClickedCheckIntro() 
{
	OnCheckIntro();
}
