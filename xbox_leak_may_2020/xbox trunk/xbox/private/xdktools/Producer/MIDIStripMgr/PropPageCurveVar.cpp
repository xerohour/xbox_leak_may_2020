// PropPageCurveVar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MidiStripMgr.h"
#include "PropPageMgr.h"
#include "PropCurve.h"
#include "PropPageCurveVar.h"
#include "PropNote.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveVar property page

CPropPageCurveVar::CPropPageCurveVar( CCurvePropPageMgr* pCurvePropPageMgr ) : CPropertyPage(CPropPageCurveVar::IDD)
{
	//{{AFX_DATA_INIT(CPropPageCurveVar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	ASSERT( pCurvePropPageMgr != NULL );

	m_pPageManager = pCurvePropPageMgr;

	m_dwVariation = 0x00000000;
	m_dwVarUndetermined = 0x00000000;
	m_fNeedToDetach = FALSE;
}

CPropPageCurveVar::~CPropPageCurveVar()
{
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveVar::EnableControls

void CPropPageCurveVar::EnableControls( BOOL fEnable ) 
{
	CWnd* pWnd;

	for( int i = IDC_CHECK_VAR1 ;  i <= IDC_CHECK_VAR32 ;  i++ )
	{
		pWnd = GetDlgItem( i );
		if( pWnd )
		{
			if( fEnable == FALSE )
			{
				CheckDlgButton( i, 0 );
			}
			pWnd->EnableWindow( fEnable );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveVar::UpdateControls

void CPropPageCurveVar::UpdateControls( CPropCurve* pPropCurve )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	if( pPropCurve == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	EnableControls( TRUE );

	m_dwVariation = pPropCurve->m_dwVariation;
	m_dwVarUndetermined = pPropCurve->m_dwVarUndetermined;

	// Variation check boxes
	CWnd* pWnd;
	int nState;
	int nVariation;

	for( int i = IDC_CHECK_VAR1 ;  i <= IDC_CHECK_VAR32 ;  i++ )
	{
		pWnd = GetDlgItem( i );
		if( pWnd )
		{
			nVariation = i - IDC_CHECK_VAR1;

			if( m_dwVarUndetermined & (1 << nVariation) )
			{
				nState = 2;
			}
			else
			{
				if( m_dwVariation & (1 << nVariation) )
				{
					nState = 1;
				}
				else
				{
					nState = 0;
				}
			}

			CheckDlgButton( i, nState );
		}
	}
}


void CPropPageCurveVar::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPageCurveVar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPageCurveVar, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPageCurveVar)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveVar message handlers

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveVar::OnCreate

int CPropPageCurveVar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
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

	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveVar::OnDestroy

void CPropPageCurveVar::OnDestroy() 
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


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveVar::OnSetActive

BOOL CPropPageCurveVar::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Update controls
	m_pPageManager->RefreshData();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CCurvePropPageMgr::sm_nActiveTab );
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveVar::OnCommand

BOOL CPropPageCurveVar::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject )
	{
		int nControlID = LOWORD( wParam );
		int nCommandID = HIWORD( wParam );

		if( nCommandID == BN_CLICKED
		||  nCommandID == BN_DOUBLECLICKED )
		{
			if( nControlID >= IDC_CHECK_VAR1
			&&  nControlID <= IDC_CHECK_VAR32 )
			{
				CWnd* pWnd = GetDlgItem( nControlID );
				if( pWnd )
				{
					int nVariation = nControlID - IDC_CHECK_VAR1;

					switch( IsDlgButtonChecked( nControlID ) )
					{
						case 0:		// Currently unchecked
						case 2:		// Currently undetermined
							CheckDlgButton( nControlID, 1 );
							m_dwVariation |= (1 << nVariation);
							m_dwVarUndetermined &= ~(1 << nVariation);
							m_pPageManager->m_PropCurve.m_dwChanged = CHGD_VARIATIONS;
							m_pPageManager->m_PropCurve.m_dwVariation = m_dwVariation;
							m_pPageManager->m_PropCurve.m_dwVarUndetermined = m_dwVarUndetermined;
							m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
							break;

						case 1:		// Currently checked
							CheckDlgButton( nControlID, 0 );
							m_dwVariation &= ~(1 << nVariation);
							m_dwVarUndetermined &= ~(1 << nVariation);
							m_pPageManager->m_PropCurve.m_dwChanged = CHGD_VARIATIONS;
							m_pPageManager->m_PropCurve.m_dwVariation = m_dwVariation;
							m_pPageManager->m_PropCurve.m_dwVarUndetermined = m_dwVarUndetermined;
							m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
							break;
					}
				}

				return TRUE;
			}
		}
	}
	
	return CPropertyPage::OnCommand( wParam, lParam );
}
