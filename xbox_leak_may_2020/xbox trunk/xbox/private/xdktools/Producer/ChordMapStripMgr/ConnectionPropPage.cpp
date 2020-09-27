// ConnectionPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "chordmapstripmgr.h"
#include "ConnectionPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectionPropPage property page

IMPLEMENT_DYNCREATE(CConnectionPropPage, CPropertyPage)

CConnectionPropPage::CConnectionPropPage() : CPropertyPage(CConnectionPropPage::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	//{{AFX_DATA_INIT(CConnectionPropPage)
	//}}AFX_DATA_INIT

	m_nWeight = 0;
	m_nMinBeats = 1;
	m_nMaxBeats = 1;
	m_fNeedToDetach = FALSE;
}

CConnectionPropPage::~CConnectionPropPage()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
}

void CConnectionPropPage::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectionPropPage)
	DDX_Control(pDX, IDC_SPIN_MINBEATS, m_spinMinBeats);
	DDX_Control(pDX, IDC_SPIN_MAXBEATS, m_spinMaxBeats);
	DDX_Control(pDX, IDC_EDIT_MINBEATS, m_editMinBeats);
	DDX_Control(pDX, IDC_EDIT_MAXBEATS, m_editMaxBeats);
	DDX_Control(pDX, IDC_SPINWEIGHT, m_spinWeight);
	DDX_Control(pDX, IDC_EDITWEIGHT, m_editWeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectionPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectionPropPage)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_UPDATE(IDC_EDITWEIGHT, OnUpdateEditweight)
	ON_EN_KILLFOCUS(IDC_EDIT_MAXBEATS, OnKillFocusEditMaxBeats)
	ON_EN_KILLFOCUS(IDC_EDIT_MINBEATS, OnKillFocusEditMinBeats)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAXBEATS, OnDeltaPosSpinMaxBeats)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINBEATS, OnDeltaPosSpinMinBeats)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionPropPage message handlers

BOOL CConnectionPropPage::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	CPropertyPage::OnInitDialog();
	
	m_editWeight.SetLimitText(3);
	m_editMinBeats.SetLimitText(3);
	m_editMaxBeats.SetLimitText(3);
	m_spinWeight.SetRange(0,100);
	m_spinMinBeats.SetRange(1, 100);
	m_spinMaxBeats.SetRange(1, 100);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectionPropPage::SetConnection(short val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsWindow( m_spinWeight.m_hWnd ) == FALSE )
	{
		return;
	}

	ASSERT(val <= 100);
	if(val <= 100)
	{
		m_spinWeight.SetPos(val);
		CString str;
		str.Format("%d", val);
		m_editWeight.SetWindowText(str);
		m_nWeight = val;
	}
}

void CConnectionPropPage::SetMinBeats(short val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsWindow( m_spinMinBeats.m_hWnd ) == FALSE )
	{
		return;
	}
	
	m_spinMinBeats.SetPos(val);
	CString str;
	str.Format("%d", val);
	m_editMinBeats.SetWindowText(str);
	m_nMinBeats = val;
}

void CConnectionPropPage::SetMaxBeats(short val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsWindow( m_spinMaxBeats.m_hWnd ) == FALSE )
	{
		return;
	}

	m_spinMaxBeats.SetPos(val);
	CString str;
	str.Format("%d", val);
	m_editMaxBeats.SetWindowText(str);
	m_nMaxBeats = val;
}

void CConnectionPropPage::SetMinMaxEdit(BOOL fEnable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_editMaxBeats.EnableWindow(fEnable);
	m_editMinBeats.EnableWindow(fEnable);
	m_spinMaxBeats.EnableWindow(fEnable);
	m_spinMinBeats.EnableWindow(fEnable);

	if( fEnable == FALSE )
	{
		m_editMaxBeats.SetWindowText( NULL );
		m_editMinBeats.SetWindowText( NULL );
	}
}

int CConnectionPropPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CConnectionPropPage::OnDestroy() 
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

BOOL CConnectionPropPage::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	return CPropertyPage::OnSetActive();
}

void CConnectionPropPage::OnUpdateEditweight() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsWindow( m_editWeight.m_hWnd ) == FALSE )
	{
		return;
	}

	char chNum[50];
	m_editWeight.GetWindowText(chNum, 50);
	short nWeight = atoi(chNum);

	if(nWeight > 100)
	{
		SetConnection(m_nWeight);
	}
	else
	{
		m_nWeight = nWeight;
		if (m_pPropPageMgr)
		{
			m_pPropPageMgr->UpdateObjectWithTabData();
		}
	}
	
}

void CConnectionPropPage::OnKillFocusEditMinBeats() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsWindow( m_editMinBeats.m_hWnd ) == FALSE )
	{
		return;
	}

	CString strMinBeats;
	m_editMinBeats.GetWindowText( strMinBeats );

	// Strip leading and trailing spaces
	strMinBeats.TrimRight();
	strMinBeats.TrimLeft();

	if( strMinBeats.IsEmpty() )
	{
		SetMinBeats( m_nMinBeats );
	}
	else
	{
		short nNewMinBeats = _ttoi( strMinBeats );

		nNewMinBeats = max( 1, nNewMinBeats );
		nNewMinBeats = min( m_nMaxBeats, nNewMinBeats );

		if( m_nMinBeats != nNewMinBeats )
		{
			m_nMinBeats = nNewMinBeats;
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->UpdateObjectWithTabData();
			}
		}

		SetMinBeats( m_nMinBeats );
	}
}

void CConnectionPropPage::OnDeltaPosSpinMinBeats(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsWindow( m_spinMinBeats.m_hWnd ) == FALSE )
	{
		return;
	}

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if( HIWORD(m_spinMinBeats.GetPos()) == 0 )
	{
		int nNewMinBeats = m_nMinBeats + pNMUpDown->iDelta;

		nNewMinBeats = max( 1, nNewMinBeats );
		nNewMinBeats = min( m_nMaxBeats, nNewMinBeats );

		if( m_nMinBeats != nNewMinBeats )
		{
			m_nMinBeats = nNewMinBeats;
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->UpdateObjectWithTabData();
			}
		}

		SetMinBeats( m_nMinBeats );
	}
	
	*pResult = 1;
}

void CConnectionPropPage::OnKillFocusEditMaxBeats() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsWindow( m_editMaxBeats.m_hWnd ) == FALSE )
	{
		return;
	}

	CString strMaxBeats;
	m_editMaxBeats.GetWindowText( strMaxBeats );

	// Strip leading and trailing spaces
	strMaxBeats.TrimRight();
	strMaxBeats.TrimLeft();

	if( strMaxBeats.IsEmpty() )
	{
		SetMaxBeats( m_nMaxBeats );
	}
	else
	{
		short nNewMaxBeats = _ttoi( strMaxBeats );

		nNewMaxBeats = max( m_nMinBeats, nNewMaxBeats );

		if( m_nMaxBeats != nNewMaxBeats )
		{
			m_nMaxBeats = nNewMaxBeats;
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->UpdateObjectWithTabData();
			}
		}

		SetMaxBeats( m_nMaxBeats );
	}
}

void CConnectionPropPage::OnDeltaPosSpinMaxBeats(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsWindow( m_spinMaxBeats.m_hWnd ) == FALSE )
	{
		return;
	}

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if( HIWORD(m_spinMaxBeats.GetPos()) == 0 )
	{
		int nNewMaxBeats = m_nMaxBeats + pNMUpDown->iDelta;

		nNewMaxBeats = max( m_nMinBeats, nNewMaxBeats );

		if( m_nMaxBeats != nNewMaxBeats )
		{
			m_nMaxBeats = nNewMaxBeats;
			if( m_pPropPageMgr )
			{
				m_pPropPageMgr->UpdateObjectWithTabData();
			}
		}

		SetMaxBeats( m_nMaxBeats );
	}
	
	*pResult = 1;
}
