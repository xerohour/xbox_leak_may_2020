// DlgAddRemoveBuses.cpp : implementation file
//

#include "stdafx.h"
#include "DlgAddRemoveBuses.h"
#include "dmusicc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddRemoveBuses dialog

#define BUSID_STEREO 0xFFFFFFFF

CDlgAddRemoveBuses::CDlgAddRemoveBuses(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddRemoveBuses::IDD, pParent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	//{{AFX_DATA_INIT(CDlgAddRemoveBuses)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgAddRemoveBuses::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddRemoveBuses)
	DDX_Control(pDX, IDC_BUTTON_UP, m_btnUp);
	DDX_Control(pDX, IDC_BUTTON_DOWN, m_btnDown);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_BUTTON_REMOVE, m_btnRemove);
	DDX_Control(pDX, IDC_LIST_BUS_INUSE, m_listInUse);
	DDX_Control(pDX, IDC_LIST_BUS_AVAILBLE, m_listAvailable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddRemoveBuses, CDialog)
	//{{AFX_MSG_MAP(CDlgAddRemoveBuses)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	ON_LBN_DBLCLK(IDC_LIST_BUS_AVAILBLE, OnDblclkListBusAvailble)
	ON_LBN_DBLCLK(IDC_LIST_BUS_INUSE, OnDblclkListBusInuse)
	ON_LBN_SELCHANGE(IDC_LIST_BUS_INUSE, OnSelchangeListBusInuse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddRemoveBuses message handlers

void CDlgAddRemoveBuses::AddStringToAvailableList( int nStringID, DWORD dwBusID )
{
	CString strBus;
	if( strBus.LoadString( nStringID ) )
	{
		int nIndex = m_listAvailable.AddString( strBus );
		m_listAvailable.SetItemData( nIndex, dwBusID );
	}
}

BOOL CDlgAddRemoveBuses::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	m_listAvailable.ResetContent();

	AddStringToAvailableList( IDS_BUSID512, DSBUSID_DYNAMIC_0 );
	AddStringToAvailableList( IDS_BUSID_STEREO, BUSID_STEREO );

	AddStringToAvailableList( IDS_BUSID64, DSBUSID_REVERB_SEND );
	AddStringToAvailableList( IDS_BUSID65, DSBUSID_CHORUS_SEND );

	AddStringToAvailableList( IDS_BUSID0, DSBUSID_LEFT );
	AddStringToAvailableList( IDS_BUSID1, DSBUSID_RIGHT );

	CString strBus;
	for( int i=0; i<m_adwBusIDs.GetSize(); i++ )
	{
		DWORD dwBusID = m_adwBusIDs.GetAt( i );
		if( strBus.LoadString( dwBusID - DSBUSID_LEFT + IDS_BUSID0 ) )
		{
			int nIndex = m_listInUse.AddString( strBus );
			m_listInUse.SetItemData( nIndex, dwBusID );
		}
	}

	m_btnRemove.EnableWindow( (m_adwBusIDs.GetSize() > 0) ? TRUE : FALSE );
	// Fix 46903: DMP: Order of busses not correct if users does not list them in ascending order
	// Disallow moving the buses, since we're going to resort them anyways
	//m_btnUp.EnableWindow( (m_adwBusIDs.GetSize() > 1) ? TRUE : FALSE );
	//m_btnDown.EnableWindow( (m_adwBusIDs.GetSize() > 1) ? TRUE : FALSE );
	m_btnUp.EnableWindow( FALSE );
	m_btnDown.EnableWindow( FALSE );
	m_btnAdd.EnableWindow( (m_adwBusIDs.GetSize() < MAX_AUDIO_CHANNELS) ? TRUE : FALSE );

	m_listInUse.SetCurSel( 0 );
	m_listAvailable.SetCurSel( 0 );

	OnSelchangeListBusInuse();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAddRemoveBuses::OnButtonAdd() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nIndex = m_listAvailable.GetCurSel();
	if( nIndex != LB_ERR )
	{
		DWORD dwBusID = m_listAvailable.GetItemData( nIndex );

		if( dwBusID == BUSID_STEREO )
		{
			AddBusIDToUsedList( DSBUSID_LEFT );
			AddBusIDToUsedList( DSBUSID_RIGHT );
		}
		else
		{
			AddBusIDToUsedList( dwBusID );
		}

		SortUsedList();

		m_listAvailable.SetCurSel( nIndex + 1 );
	}
}

void CDlgAddRemoveBuses::AddBusIDToUsedList( DWORD dwBusID )
{
	if( m_listInUse.GetCount() >= MAX_AUDIO_CHANNELS )
	{
		return;
	}

	CString strBus;
	if( strBus.LoadString( dwBusID - DSBUSID_LEFT + IDS_BUSID0 ) )
	{
		int nNewIndex = m_listInUse.GetCurSel();
		if( nNewIndex == LB_ERR )
		{
			nNewIndex = 0;
		}
		else
		{
			nNewIndex++;
		}

		m_listInUse.InsertString( nNewIndex, strBus );
		m_listInUse.SetItemData( nNewIndex, dwBusID );

		m_btnRemove.EnableWindow( TRUE );

		m_listInUse.SetCurSel( nNewIndex );

		if( m_listInUse.GetCount() >= MAX_AUDIO_CHANNELS )
		{
			m_btnAdd.SetButtonStyle( BS_PUSHBUTTON, TRUE );
			m_btnRemove.SetButtonStyle( BS_DEFPUSHBUTTON, TRUE );
			m_btnAdd.EnableWindow( FALSE );
			m_btnRemove.SetFocus();
		}

		OnSelchangeListBusInuse();
	}
}

void CDlgAddRemoveBuses::OnButtonRemove() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nIndex = m_listInUse.GetCurSel();
	if( nIndex != LB_ERR )
	{
		// TODO: Select dwBusID in m_listAvailable ?
		//DWORD dwBusID = m_listInUse.GetItemData( nIndex );

		m_listInUse.DeleteString( nIndex );

		m_btnAdd.EnableWindow( TRUE );

		if( m_listInUse.GetCount() == 0 )
		{
			m_btnAdd.SetFocus();
			m_btnAdd.SetButtonStyle( BS_DEFPUSHBUTTON, TRUE );
			m_btnRemove.SetButtonStyle( BS_PUSHBUTTON, TRUE );
			m_btnRemove.EnableWindow( FALSE );
		}
		else
		{
			m_listInUse.SetCurSel( min(m_listInUse.GetCount() - 1, nIndex) );
		}

		OnSelchangeListBusInuse();
	}
}

void CDlgAddRemoveBuses::OnButtonReset() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Remove all existing buses
	m_listInUse.ResetContent();

	// Add just the dynamic bus
	CString strBus;
	if( strBus.LoadString( IDS_BUSID512 ) )
	{
		int nNewIndex = m_listInUse.AddString( strBus );
		m_listInUse.SetItemData( nNewIndex, DSBUSID_DYNAMIC_0 );

		m_listInUse.SetCurSel( nNewIndex );

		m_btnRemove.EnableWindow( TRUE );
		m_btnAdd.EnableWindow( TRUE );
	}

	OnSelchangeListBusInuse();
}

void CDlgAddRemoveBuses::OnButtonUp() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the index of the selected bus
	int nIndex = m_listInUse.GetCurSel();
	if( nIndex != LB_ERR )
	{
		// This shouldn't be zero
		ASSERT( nIndex != 0 );

		// Get the bus ID of the selected bus
		DWORD dwBusID = m_listInUse.GetItemData( nIndex );

		// Remove the selected bus
		m_listInUse.DeleteString( nIndex );

		// Move the selected bus up one
		nIndex = max( 0, nIndex - 1 );

		// Re-add the selected bus
		CString strBus;
		if( strBus.LoadString( dwBusID - DSBUSID_LEFT + IDS_BUSID0 ) )
		{
			m_listInUse.InsertString( nIndex, strBus );
			m_listInUse.SetItemData( nIndex, dwBusID );

			m_listInUse.SetCurSel( nIndex );
		}

		OnSelchangeListBusInuse();

		SortUsedList();
	}
}

void CDlgAddRemoveBuses::OnButtonDown() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the index of the selected bus
	int nIndex = m_listInUse.GetCurSel();
	if( nIndex != LB_ERR )
	{
		// This shouldn't be the last bus
		ASSERT( nIndex != m_listInUse.GetCount() - 1 );

		// Get the bus ID of the selected bus
		DWORD dwBusID = m_listInUse.GetItemData( nIndex );

		// Remove the selected bus
		m_listInUse.DeleteString( nIndex );

		// Move the selected bus down one
		nIndex = min( nIndex + 1, m_listInUse.GetCount() );

		// Re-add the selected bus
		CString strBus;
		if( strBus.LoadString( dwBusID - DSBUSID_LEFT + IDS_BUSID0 ) )
		{
			m_listInUse.InsertString( nIndex, strBus );
			m_listInUse.SetItemData( nIndex, dwBusID );

			m_listInUse.SetCurSel( nIndex );
		}

		OnSelchangeListBusInuse();

		SortUsedList();
	}
}

void CDlgAddRemoveBuses::OnOK() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_adwBusIDs.RemoveAll();

	for( int nIndex = 0; nIndex < m_listInUse.GetCount(); nIndex++ )
	{
		m_adwBusIDs.Add( m_listInUse.GetItemData( nIndex ) );
	}

	if( m_adwBusIDs.GetSize() == 0 )
	{
		AfxMessageBox(IDS_ZERO_BUSES);
		return;
	}

	CDialog::OnOK();
}

void CDlgAddRemoveBuses::OnDblclkListBusAvailble() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_listInUse.GetCount() < MAX_AUDIO_CHANNELS )
	{
		OnButtonAdd();
	}
}

void CDlgAddRemoveBuses::OnDblclkListBusInuse() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	OnButtonRemove();
}

void CDlgAddRemoveBuses::OnSelchangeListBusInuse() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Fix 46903: DMP: Order of busses not correct if users does not list them in ascending order
	// Disallow moving the buses, since we're going to resort them anyways
	/*
	BOOL fEnableUp = (m_listInUse.GetCount() > 1) ? TRUE : FALSE;
	BOOL fEnableDown = fEnableUp;

	// Get the selected index
	int nIndex = m_listInUse.GetCurSel();
	if( nIndex == LB_ERR )
	{
		fEnableUp = FALSE;
		fEnableDown = FALSE;
	}

	if( fEnableUp )
	{
		fEnableUp = (nIndex != 0) ? TRUE : FALSE;
	}

	if( fEnableDown )
	{
		fEnableDown = (nIndex != m_listInUse.GetCount() - 1) ? TRUE : FALSE;
	}

	m_btnUp.EnableWindow( fEnableUp );
	if( !fEnableUp )
	{
		m_btnUp.SetButtonStyle( BS_PUSHBUTTON, TRUE );
	}
	m_btnDown.EnableWindow( fEnableDown );
	if( !fEnableDown )
	{
		m_btnDown.SetButtonStyle( BS_PUSHBUTTON, TRUE );
	}
	*/
}

// Fix 46903: DMP: Order of busses not correct if users does not list them in ascending order
void CDlgAddRemoveBuses::SortUsedList( void )
{
	// Bubble sort m_listInUse

	bool fContinue = true;
	while( fContinue )
	{
		fContinue = false;

		for( int nIndex = m_listInUse.GetCount() - 1; nIndex > 0; nIndex-- )
		{
			// nIndex is at least 1
			ASSERT( nIndex > 0 );

			// If the items are out of order
			if( m_listInUse.GetItemData( nIndex ) < m_listInUse.GetItemData( nIndex - 1 ) )
			{
				// Set fContinue
				fContinue = true;

				// Reverse the items
				CString strItem1, strItem2;
				m_listInUse.GetText( nIndex, strItem1 );
				m_listInUse.GetText( nIndex - 1, strItem2 );

				DWORD dwData1, dwData2;
				dwData1 = m_listInUse.GetItemData( nIndex );
				dwData2 = m_listInUse.GetItemData( nIndex - 1 );

				int nSel = m_listInUse.GetCurSel();

				m_listInUse.DeleteString( nIndex );
				m_listInUse.DeleteString( nIndex - 1 );

				m_listInUse.InsertString( nIndex - 1, strItem1 );
				m_listInUse.SetItemData( nIndex - 1, dwData1 );

				m_listInUse.InsertString( nIndex, strItem2 );
				m_listInUse.SetItemData( nIndex, dwData2 );

				if( nSel == nIndex )
				{
					m_listInUse.SetCurSel( nIndex - 1 );
				}
				else if( nSel == nIndex - 1 )
				{
					m_listInUse.SetCurSel( nIndex );
				}
			}
		}
	}

	OnSelchangeListBusInuse();
}
