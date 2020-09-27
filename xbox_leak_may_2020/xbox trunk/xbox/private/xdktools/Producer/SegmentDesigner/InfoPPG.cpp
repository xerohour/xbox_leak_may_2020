// InfoPPG.cpp : implementation file
//

#include "stdafx.h"
#include "InfoPPG.h"
#include "SegmentPPGMgr.h"
#include <guiddlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoPPG property page

CInfoPPG::CInfoPPG( CSegmentPPGMgr* pSegmentPropPageManager ) : CPropertyPage(CInfoPPG::IDD)
{
	//{{AFX_DATA_INIT(CInfoPPG)
	//}}AFX_DATA_INIT
	
	ASSERT( pSegmentPropPageManager != NULL );

	m_pSegment = NULL;
	m_pPageManager = pSegmentPropPageManager;
	m_fNeedToDetach = FALSE;
}

CInfoPPG::~CInfoPPG()
{
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::SetSegment

void CInfoPPG::SetData(const PPGSegment *pSegment)
{
	if( pSegment == NULL )
	{
		if( m_pSegment )
		{
			delete m_pSegment;
			m_pSegment = NULL;
		}
		return;
	}
	if( m_pSegment == NULL )
	{
		m_pSegment = new PPGSegment;
	}

	m_pSegment->Copy( pSegment );
	m_pSegment->dwFlags = (m_pSegment->dwFlags & PPGT_NONVALIDFLAGS) | PPGT_VALIDINFO;

	UpdateControls();
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::UpdateSegment

void CInfoPPG::UpdateSegment()
{
	if( m_pPageManager )
	{
		if( m_pPageManager->m_pIPropPageObject )
		{
			if( m_pSegment )
			{
				m_pPageManager->m_pIPropPageObject->SetData( m_pSegment );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::EnableControls

void CInfoPPG::EnableControls( BOOL fEnable ) 
{
	m_editAuthor.EnableWindow( fEnable );
	m_editCopyright.EnableWindow( fEnable );
	m_editInfo.EnableWindow( fEnable );
	m_editSubject.EnableWindow( fEnable );
	m_editVersion_1.EnableWindow( fEnable );
	m_editVersion_2.EnableWindow( fEnable );
	m_editVersion_3.EnableWindow( fEnable );
	m_editVersion_4.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::UpdateControls

void CInfoPPG::UpdateControls() 
{
	// Make sure controls have been created
	if( ::IsWindow(m_editVersion_1.m_hWnd) == FALSE )
	{
		return;
	}
	
	// Update controls
	m_editVersion_1.LimitText( 4 );
	m_editVersion_2.LimitText( 4 );
	m_editVersion_3.LimitText( 4 );
	m_editVersion_4.LimitText( 4 );

	if( m_pSegment )
	{
		CString strText;

		EnableControls( TRUE );

		m_editAuthor.SetWindowText( m_pSegment->strAuthor );
		m_editCopyright.SetWindowText( m_pSegment->strCopyright );
		m_editInfo.SetWindowText( m_pSegment->strInfo );
		m_editSubject.SetWindowText( m_pSegment->strSubject );
		
		strText.Format( "%u", m_pSegment->wVersion1 );
		m_editVersion_1.SetWindowText( strText );
		
		strText.Format( "%u", m_pSegment->wVersion2 );
		m_editVersion_2.SetWindowText( strText );
		
		strText.Format( "%u", m_pSegment->wVersion3 );
		m_editVersion_3.SetWindowText( strText );
		
		strText.Format( "%u", m_pSegment->wVersion4 );
		m_editVersion_4.SetWindowText( strText );
	}
	else
	{
		m_editAuthor.SetWindowText( _T("") );
		m_editCopyright.SetWindowText( _T("") );
		m_editInfo.SetWindowText( _T("") );
		m_editSubject.SetWindowText( _T(""));
		m_editVersion_1.SetWindowText( _T("") );
		m_editVersion_2.SetWindowText( _T("") );
		m_editVersion_3.SetWindowText( _T("") );
		m_editVersion_4.SetWindowText( _T("") );

		EnableControls( FALSE );
	}
}


void CInfoPPG::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPPG)
	DDX_Control(pDX, IDC_VERSION_4, m_editVersion_4);
	DDX_Control(pDX, IDC_VERSION_3, m_editVersion_3);
	DDX_Control(pDX, IDC_VERSION_2, m_editVersion_2);
	DDX_Control(pDX, IDC_VERSION_1, m_editVersion_1);
	DDX_Control(pDX, IDC_SUBJECT, m_editSubject);
	DDX_Control(pDX, IDC_INFO, m_editInfo);
	DDX_Control(pDX, IDC_COPYRIGHT, m_editCopyright);
	DDX_Control(pDX, IDC_AUTHOR, m_editAuthor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoPPG, CPropertyPage)
	//{{AFX_MSG_MAP(CInfoPPG)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_AUTHOR, OnKillFocusAuthor)
	ON_EN_KILLFOCUS(IDC_COPYRIGHT, OnKillFocusCopyright)
	ON_EN_KILLFOCUS(IDC_INFO, OnKillFocusInfo)
	ON_EN_KILLFOCUS(IDC_SUBJECT, OnKillFocusSubject)
	ON_EN_KILLFOCUS(IDC_VERSION_1, OnKillFocusVersion_1)
	ON_EN_KILLFOCUS(IDC_VERSION_2, OnKillFocusVersion_2)
	ON_EN_KILLFOCUS(IDC_VERSION_3, OnKillFocusVersion_3)
	ON_EN_KILLFOCUS(IDC_VERSION_4, OnKillFocusVersion_4)
	ON_BN_CLICKED(IDC_EDIT_GUID, OnEditGuid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG message handlers

/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::OnSetActive

BOOL CInfoPPG::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UpdateControls();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CSegmentPPGMgr::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::OnCreate

int CInfoPPG::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CInfoPPG::OnDestroy

void CInfoPPG::OnDestroy() 
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
// CInfoPPG::OnKillFocusAuthor

void CInfoPPG::OnKillFocusAuthor() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment )
	{
		CString strAuthor;

		m_editAuthor.GetWindowText( strAuthor );

		// Strip leading and trailing spaces
		strAuthor.TrimRight();
		strAuthor.TrimLeft();

		if( strAuthor.Compare( m_pSegment->strAuthor ) != 0 )
		{
			m_pSegment->strAuthor = strAuthor;
			UpdateSegment();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::OnKillFocusCopyright

void CInfoPPG::OnKillFocusCopyright() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment )
	{
		CString strCopyright;

		m_editCopyright.GetWindowText( strCopyright );

		// Strip leading and trailing spaces
		strCopyright.TrimRight();
		strCopyright.TrimLeft();

		if( strCopyright.Compare( m_pSegment->strCopyright ) != 0 )
		{
			m_pSegment->strCopyright = strCopyright;
			UpdateSegment();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::OnKillFocusInfo

void CInfoPPG::OnKillFocusInfo() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment )
	{
		CString strInfo;

		m_editInfo.GetWindowText( strInfo );

		// Strip leading and trailing spaces
		strInfo.TrimRight();
		strInfo.TrimLeft();

		if( strInfo.Compare( m_pSegment->strInfo ) != 0 )
		{
			m_pSegment->strInfo = strInfo;
			UpdateSegment();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::OnKillFocusSubject

void CInfoPPG::OnKillFocusSubject() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment )
	{
		CString strSubject;

		m_editSubject.GetWindowText( strSubject );

		// Strip leading and trailing spaces
		strSubject.TrimRight();
		strSubject.TrimLeft();

		if( strSubject.Compare( m_pSegment->strSubject ) != 0 )
		{
			m_pSegment->strSubject = strSubject;
			UpdateSegment();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::OnKillFocusVersion_1

void CInfoPPG::OnKillFocusVersion_1() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment )
	{
		CString strVersion_1;
		CString strSegmentVersion_1;

		m_editVersion_1.GetWindowText( strVersion_1 );

		// Strip leading and trailing spaces
		strVersion_1.TrimRight();
		strVersion_1.TrimLeft();

		if( strVersion_1.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", m_pSegment->wVersion1 );
			m_editVersion_1.SetWindowText( strText );
		}
		else
		{
			strSegmentVersion_1.Format( "%u", m_pSegment->wVersion1 );
			
			if( strVersion_1.Compare( strSegmentVersion_1 ) != 0 )
			{
				WORD wVersion_1 = WORD(_ttoi( strVersion_1 ));

				m_pSegment->wVersion1 = wVersion_1;
				UpdateSegment();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::OnKillFocusVersion_2

void CInfoPPG::OnKillFocusVersion_2() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment )
	{
		CString strVersion_2;
		CString strSegmentVersion_2;

		m_editVersion_2.GetWindowText( strVersion_2 );

		// Strip leading and trailing spaces
		strVersion_2.TrimRight();
		strVersion_2.TrimLeft();

		if( strVersion_2.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", m_pSegment->wVersion2 );
			m_editVersion_2.SetWindowText( strText );
		}
		else
		{
			strSegmentVersion_2.Format( "%u", m_pSegment->wVersion2 );
			
			if( strVersion_2.Compare( strSegmentVersion_2 ) != 0 )
			{
				WORD wVersion_2 = WORD(_ttoi( strVersion_2 ));

				m_pSegment->wVersion2 = wVersion_2;
				UpdateSegment();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::OnKillFocusVersion_3

void CInfoPPG::OnKillFocusVersion_3() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment )
	{
		CString strVersion_3;
		CString strSegmentVersion_3;

		m_editVersion_3.GetWindowText( strVersion_3 );

		// Strip leading and trailing spaces
		strVersion_3.TrimRight();
		strVersion_3.TrimLeft();

		if( strVersion_3.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", m_pSegment->wVersion3 );
			m_editVersion_3.SetWindowText( strText );
		}
		else
		{
			strSegmentVersion_3.Format( "%u", m_pSegment->wVersion3 );
			
			if( strVersion_3.Compare( strSegmentVersion_3 ) != 0 )
			{
				WORD wVersion_3 = WORD(_ttoi( strVersion_3 ));
				m_pSegment->wVersion3 = wVersion_3;
				UpdateSegment();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::OnKillFocusVersion_4

void CInfoPPG::OnKillFocusVersion_4() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment )
	{
		CString strVersion_4;
		CString strSegmentVersion_4;

		m_editVersion_4.GetWindowText( strVersion_4 );

		// Strip leading and trailing spaces
		strVersion_4.TrimRight();
		strVersion_4.TrimLeft();

		if( strVersion_4.IsEmpty() )
		{
			CString strText;

			strText.Format( "%u", m_pSegment->wVersion4 );
			m_editVersion_4.SetWindowText( strText );
		}
		else
		{
			strSegmentVersion_4.Format( "%u", m_pSegment->wVersion4 );
			
			if( strVersion_4.Compare( strSegmentVersion_4 ) != 0 )
			{
				WORD wVersion_4 = WORD(_ttoi( strVersion_4 ));
				m_pSegment->wVersion4 = wVersion_4;
				UpdateSegment();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInfoPPG::PreTranslateMessage

BOOL CInfoPPG::PreTranslateMessage( MSG* pMsg ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment == NULL )
	{
		return CPropertyPage::PreTranslateMessage( pMsg );
	}

	switch( pMsg->message )
	{
		case WM_KEYDOWN:
			if( pMsg->lParam & 0x40000000 )
			{
				break;
			}

			switch( pMsg->wParam )
			{
				case VK_ESCAPE:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_AUTHOR: 
								m_editAuthor.SetWindowText( m_pSegment->strAuthor );
								break;

							case IDC_COPYRIGHT: 
								m_editCopyright.SetWindowText( m_pSegment->strCopyright );
								break;

							case IDC_INFO: 
								m_editInfo.SetWindowText( m_pSegment->strInfo );
								break;

							case IDC_SUBJECT: 
								m_editSubject.SetWindowText( m_pSegment->strSubject );
								break;

							case IDC_VERSION_1:
							{
								CString strText;

								strText.Format( "%u", m_pSegment->wVersion1 );
								m_editVersion_1.SetWindowText( strText );
								break;
							}

							case IDC_VERSION_2: 
							{
								CString strText;
	
								strText.Format( "%u", m_pSegment->wVersion2 );
								m_editVersion_2.SetWindowText( strText );
								break;
							}

							case IDC_VERSION_3: 
							{
								CString strText;
	
								strText.Format( "%u", m_pSegment->wVersion3 );
								m_editVersion_3.SetWindowText( strText );
								break;
							}

							case IDC_VERSION_4: 
							{
								CString strText;
	
								strText.Format( "%u", m_pSegment->wVersion4 );
								m_editVersion_4.SetWindowText( strText );
								break;
							}
						}
					}
					return TRUE;
				}

				case VK_RETURN:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						CWnd* pWndNext = GetNextDlgTabItem( pWnd );
						if( pWndNext )
						{
							pWndNext->SetFocus();
						}
					}
					return TRUE;
				}
			}
			break;
	}
	
	return CPropertyPage::PreTranslateMessage( pMsg );
}

void CInfoPPG::OnEditGuid() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment )
	{
		CGuidDlg dlg;
		memcpy( &dlg.m_guid, &m_pSegment->guidSegment, sizeof(GUID) );
		if( dlg.DoModal() == IDOK )
		{
			memcpy( &m_pSegment->guidSegment, &dlg.m_guid, sizeof(GUID) );
			UpdateSegment();
		}
	}
}
