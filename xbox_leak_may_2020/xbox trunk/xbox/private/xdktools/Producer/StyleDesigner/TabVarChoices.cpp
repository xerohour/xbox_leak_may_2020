// TabVarChoices.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Style.h"
#include "VarChoices.h"
#include "TabVarChoices.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabVarChoices property page

CTabVarChoices::CTabVarChoices( CVarChoicesPropPageManager* pVarChoicesPropPageManager ) : CPropertyPage(CTabVarChoices::IDD)
{
	//{{AFX_DATA_INIT(CTabVarChoices)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	ASSERT( pVarChoicesPropPageManager != NULL );

	m_pVarChoices = NULL;
	m_pPageManager = pVarChoicesPropPageManager;
	m_fNeedToDetach = FALSE;
}

CTabVarChoices::~CTabVarChoices()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabVarChoices::SetVarChoices

void CTabVarChoices::SetVarChoices( CVarChoices* pVarChoices )
{
	m_pVarChoices = pVarChoices;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CTabVarChoices::EnableControls

void CTabVarChoices::EnableControls( BOOL fEnable ) 
{
	m_editVariation.EnableWindow( fEnable );
	m_spinVariation.EnableWindow( fEnable );
	m_comboFunction.EnableWindow( fEnable );
	m_btnMajor.EnableWindow( fEnable );
	m_btnMinor.EnableWindow( fEnable );
	m_btnOther.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabVarChoices::UpdateControls

void CTabVarChoices::UpdateControls()
{
	// Make sure controls have been created
	if( ::IsWindow(m_editVariation.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editVariation.LimitText( 2 );

	if( m_pVarChoices )
	{
		EnableControls( TRUE );

		// Set Variation
		m_spinVariation.SetRange( 1, NBR_VARIATIONS );
		m_spinVariation.SetPos( 1 );

		// Set Function
		m_comboFunction.SetCurSel( 0 );

		// Set Major
		m_btnMajor.SetCheck( 0 );

		// Set Minor
		m_btnMinor.SetCheck( 0 );

		// Set Other
		m_btnOther.SetCheck( 0 );
	}
	else
	{
		m_spinVariation.SetRange( 1, NBR_VARIATIONS );
		m_spinVariation.SetPos( 1 );
		m_btnMajor.SetCheck( 0 );
		m_btnMinor.SetCheck( 0 );
		m_btnOther.SetCheck( 0 );

		EnableControls( FALSE );
	}
}


void CTabVarChoices::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabVarChoices)
	DDX_Control(pDX, IDC_VARIATION_SPIN, m_spinVariation);
	DDX_Control(pDX, IDC_VARIATION, m_editVariation);
	DDX_Control(pDX, IDC_OTHER, m_btnOther);
	DDX_Control(pDX, IDC_MINOR, m_btnMinor);
	DDX_Control(pDX, IDC_MAJOR, m_btnMajor);
	DDX_Control(pDX, IDC_FUNCTION, m_comboFunction);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabVarChoices, CPropertyPage)
	//{{AFX_MSG_MAP(CTabVarChoices)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabVarChoices message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabVarChoices::OnSetActive

BOOL CTabVarChoices::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabVarChoices::OnCreate

int CTabVarChoices::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
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
// CTabVarChoices::OnDestroy

void CTabVarChoices::OnDestroy() 
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
