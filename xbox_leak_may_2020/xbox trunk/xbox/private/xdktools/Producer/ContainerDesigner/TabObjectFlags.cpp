// TabObjectFlags.cpp : implementation file
//

#include "stdafx.h"
#include "ContainerDesignerDLL.h"

#include "Container.h"
#include "Object.h"
#include "TabObjectFlags.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabObjectFlags property page

CTabObjectFlags::CTabObjectFlags( CObjectPropPageManager* pPageManager ) : CPropertyPage(CTabObjectFlags::IDD)
{
	//{{AFX_DATA_INIT(CTabObjectFlags)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );

	m_pContainerObject = NULL;
	m_pPageManager = pPageManager;
	m_fNeedToDetach = FALSE;
}

CTabObjectFlags::~CTabObjectFlags()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabObjectFlags::SetContainerObject	

void CTabObjectFlags::SetContainerObject( CContainerObject* pContainerObject )
{
	m_pContainerObject = pContainerObject;
}


void CTabObjectFlags::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabObjectFlags)
	DDX_Control(pDX, IDC_CACHE, m_checkCache);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTabObjectFlags::SetModifiedFlag

void CTabObjectFlags::SetModifiedFlag( void ) 
{
	ASSERT( m_pContainerObject != NULL );
	ASSERT( m_pContainerObject->m_pContainer != NULL );

	if( m_pContainerObject
	&&  m_pContainerObject->m_pContainer )
	{
		m_pContainerObject->m_pContainer->SetModified( TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabObjectFlags::EnableControls

void CTabObjectFlags::EnableControls( BOOL fEnable ) 
{
	m_checkCache.EnableWindow( fEnable );
}


BEGIN_MESSAGE_MAP(CTabObjectFlags, CPropertyPage)
	//{{AFX_MSG_MAP(CTabObjectFlags)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CACHE, OnCache)
	ON_BN_DOUBLECLICKED(IDC_CACHE, OnDoubleClickedCache)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabObjectFlags message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabObjectFlags::OnSetActive

BOOL CTabObjectFlags::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pContainerObject == NULL )
	{
		EnableControls( FALSE );
		return CPropertyPage::OnSetActive();
	}

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CObjectPropPageManager::sm_nActiveTab );

	EnableControls( TRUE );

	// Cache check
	if( m_pContainerObject->m_dwFlagsDM & DMUS_CONTAINED_OBJF_KEEP )
	{
		m_checkCache.SetCheck( 1 );
	}
	else
	{
		m_checkCache.SetCheck( 0 );
	}
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabObjectFlags::OnDestroy

void CTabObjectFlags::OnDestroy() 
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
// CTabObjectFlags::OnCreate

int CTabObjectFlags::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabObjectFlags::OnCache

void CTabObjectFlags::OnCache() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pContainerObject
	&&  m_pContainerObject->m_pContainer )
	{
		m_pContainerObject->m_pContainer->m_pUndoMgr->SaveState( m_pContainerObject->m_pContainer, theApp.m_hInstance, IDS_UNDO_OBJECT_KEEP );
					
		if( m_checkCache.GetCheck() )
		{
			m_pContainerObject->m_dwFlagsDM |= DMUS_CONTAINED_OBJF_KEEP;
		}
		else
		{
			m_pContainerObject->m_dwFlagsDM &= ~DMUS_CONTAINED_OBJF_KEEP;
		}

		SetModifiedFlag();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabObjectFlags::OnDoubleClickedCache

void CTabObjectFlags::OnDoubleClickedCache() 
{
	OnCache();
}
