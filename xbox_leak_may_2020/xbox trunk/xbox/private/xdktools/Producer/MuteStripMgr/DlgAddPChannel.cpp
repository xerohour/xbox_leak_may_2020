// DlgAddPChannel.cpp : implementation file
//

#include "stdafx.h"
#include "DlgAddPChannel.h"
#include "PChannelName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PCHANNEL_NAME_BUFFER_SIZE	(MAX_PATH + 10)

/////////////////////////////////////////////////////////////////////////////
// CDlgAddPChannel dialog


CDlgAddPChannel::CDlgAddPChannel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddPChannel::IDD, pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	//{{AFX_DATA_INIT(CDlgAddPChannel)
	//}}AFX_DATA_INIT
	m_pIPChannelName = NULL;
	m_fChanged = false;
}

CDlgAddPChannel::~CDlgAddPChannel()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pIPChannelName )
	{
		m_pIPChannelName->Release();
		m_pIPChannelName = NULL;
	}
}

void CDlgAddPChannel::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddPChannel)
	DDX_Control(pDX, IDC_LIST_CHANNEL, m_listPChannel);
	DDX_Control(pDX, IDC_BTN_REMOVE, m_btnRemove);
	DDX_Control(pDX, IDC_BUTTON_ADD16, m_btnAdd16);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_EDIT_PCHANNEL_NAME, m_editPChannelName);
	DDX_Control(pDX, IDC_SPIN_PCHANNEL, m_spinPChannel);
	DDX_Control(pDX, IDC_EDIT_PCHANNEL, m_editPChannel);
	DDX_Control(pDX, IDOK, m_btnOK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddPChannel, CDialog)
	//{{AFX_MSG_MAP(CDlgAddPChannel)
	ON_EN_UPDATE(IDC_EDIT_PCHANNEL, OnUpdateEditPchannel)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnBtnRemove)
	ON_BN_CLICKED(IDC_BUTTON_ADD16, OnButtonAdd16)
	ON_LBN_SELCHANGE(IDC_LIST_CHANNEL, OnSelchangeListChannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddPChannel message handlers

void CDlgAddPChannel::OnUpdateEditPchannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_btnAdd.GetSafeHwnd() == NULL )
	{
		return;
	}

	BOOL fTrans;
	DWORD dwPChannel = GetDlgItemInt( IDC_EDIT_PCHANNEL, &fTrans, FALSE );
	if( fTrans && dwPChannel > 0 && dwPChannel < 1000 )
	{
		dwPChannel--;
		for( int i=m_adwExistingPChannels.GetUpperBound(); i >= 0; i-- )
		{
			if( m_adwExistingPChannels.GetAt( i ) == dwPChannel )
			{
				break;
			}
		}
		if( i == -1 )
		{
			// Didn't find the PChannel - look through the mix group's list
			for(  i=m_adwPChannels.GetUpperBound(); i >= 0; i-- )
			{
				if( m_adwPChannels.GetAt( i ) == dwPChannel )
				{
					break;
				}
			}
		}

		if( i == -1 )
		{
			// Didn't find the PChannel - enable the Add button
			m_btnAdd.EnableWindow( TRUE );
			m_btnAdd16.EnableWindow( TRUE );
		}
		else
		{
			// Found the PChannel - disable the Add button
			m_btnAdd.EnableWindow( FALSE );
			m_btnAdd16.EnableWindow( FALSE );
		}

		WCHAR wcstrPChannelName[MAX_PATH];
		if( m_pIPChannelName && SUCCEEDED( m_pIPChannelName->GetPChannelName( dwPChannel, wcstrPChannelName ) ) )
		{
			CString strText = wcstrPChannelName;
			m_editPChannelName.SetWindowText( strText );
		}
		else
		{
			m_editPChannelName.SetWindowText(NULL);
		}
	}
	else
	{
		// PChannel number invalid - disable the Add button
		m_btnAdd.EnableWindow( FALSE );
		m_btnAdd16.EnableWindow( FALSE );
		m_editPChannelName.SetWindowText(NULL);
	}
}

BOOL CDlgAddPChannel::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::OnInitDialog();

	// Set dialog title
	if( m_strTitle.IsEmpty() == FALSE )
	{
		SetWindowText( m_strTitle );
	}

	// Set the range of the spin control
	m_spinPChannel.SetRange( 1, 999 );

	// Limit the text size of the PChannel edit box
	m_editPChannel.SetLimitText( 3 );

	// Update the PChannel number with the first available PChannel
	m_spinPChannel.SetPos( FindNextValidPChannel( 0 ) + 1 );

	// Update the PChannel name
	OnUpdateEditPchannel();

	// Resync the listbox
	RefreshListbox();

	// Update the state of the 'Remove' button
	OnSelchangeListChannel();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAddPChannel::RefreshListbox( void )
{
	m_listPChannel.SetRedraw( FALSE );
	m_listPChannel.ResetContent();

	WCHAR wcstrPChannelName[MAX_PATH];
	TCHAR tcstrPChannel[PCHANNEL_NAME_BUFFER_SIZE];

	for( int i=m_adwPChannels.GetUpperBound(); i >= 0; i-- )
	{
		DWORD dwPChannel = m_adwPChannels.GetAt(i);
		ASSERT( dwPChannel < 1000 ); 
		_itot( dwPChannel + 1, tcstrPChannel, 10 );

		if( m_pIPChannelName && SUCCEEDED( m_pIPChannelName->GetPChannelName( dwPChannel, wcstrPChannelName ) ) )
		{
			_tcsncat( tcstrPChannel, _T(" - "), 3 );
			CString strText = wcstrPChannelName;
			_tcsncat( tcstrPChannel, strText, strText.GetLength() );
		}

		int nIndex = m_listPChannel.InsertString( 0, tcstrPChannel );
		m_listPChannel.SetItemData( nIndex, dwPChannel );
	}
	m_listPChannel.SetRedraw( TRUE );
	m_listPChannel.Invalidate( TRUE );
}

DWORD CDlgAddPChannel::FindNextValidPChannel( DWORD dwPChannel ) const
{
	while( dwPChannel < 999 )
	{
		// Check through m_adwExistingPChannels for dwPChannel
		for( int i=m_adwExistingPChannels.GetUpperBound(); i >= 0; i-- )
		{
			if( m_adwExistingPChannels.GetAt( i ) == dwPChannel )
			{
				break;
			}
		}
		if( i == -1 )
		{
			// Didn't find the PChannel - look through the mix group's list
			for(  i=m_adwPChannels.GetUpperBound(); i >= 0; i-- )
			{
				if( m_adwPChannels.GetAt( i ) == dwPChannel )
				{
					break;
				}
			}
		}

		// Didn't find the PChannel
		if( i == -1 )
		{
			// Break out of the while() loop and return dwPChannel
			break;
		}

		// Found the PChannel, go through the loop again with the next PChannel
		dwPChannel++;
	}

	return min( dwPChannel, 998 );
}

void CDlgAddPChannel::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	/*
	if( m_pfnInsertPChannel )
	{
		if( (*m_pfnInsertPChannel)( m_pCallbackData, m_fChanged, m_dwPChannel ) )
		{
			DWORD *adwNewArray = new DWORD[m_lCountExistingPChannels + 1];
			memcpy( adwNewArray, m_adwExistingPChannels, sizeof(DWORD) * m_lCountExistingPChannels );
			adwNewArray[m_lCountExistingPChannels] = m_dwPChannel;
			delete[] m_adwExistingPChannels;
			m_adwExistingPChannels = adwNewArray;
			m_lCountExistingPChannels++;

			// Increment the PChannel # by one
			m_dwPChannel++;

			// Find the next available PChannel and return it (up to a maximum of 999)
			m_dwPChannel = FindNextValidPChannel( m_dwPChannel );

			// Update the PChannel number
			m_spinPChannel.SetPos( m_dwPChannel + 1 );

			OnUpdateEditPchannel();
			return;
		}
	}
	*/

	CDialog::OnOK();
}

bool CDlgAddPChannel::AddToList( DWORD dwPChannel )
{
	// Search through all items in the array
	for( int i=m_adwPChannels.GetUpperBound(); i >= 0; i-- )
	{
		// Get the PChannel value for each index
		DWORD dwTmpPChannel = m_adwPChannels.GetAt(i);

		// Ensure the PChannel # is valid
		ASSERT( dwTmpPChannel < 1000 );

		// Check that the PChannel doesn't already exist
		if( dwTmpPChannel == dwPChannel )
		{
			ASSERT(FALSE);
			return false;
		}

		// Check if the new PChannel should be inserted above the current one
		if( dwTmpPChannel < dwPChannel )
		{
			// Yes - break out and insert above the current index
			break;
		}
	}

	// Insert the new PChannel at the index above the one we have.
	// If the for() loop completes, i will be -1.
	m_adwPChannels.InsertAt( i+1, dwPChannel, 1 );
	return true;
}

void CDlgAddPChannel::OnButtonAdd()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get the PChannel # to add
	BOOL fTrans;
	DWORD dwPChannel = GetDlgItemInt( IDC_EDIT_PCHANNEL, &fTrans, FALSE );

	// Ensure the PChannel # is valid
	dwPChannel = min( dwPChannel-1, 998 );

	// Do a sorted add to the list of channels
	if( AddToList( dwPChannel ) )
	{
		RefreshListbox();
	}

	// Remove selection from all strings
	m_listPChannel.SetSel(-1, FALSE);

	// Search for the PChannel # we added
	for( int i=0; i < m_listPChannel.GetCount(); i++ )
	{
		if( m_listPChannel.GetItemData( i ) == dwPChannel )
		{
			// Select the PChannel
			m_listPChannel.SetSel( i, TRUE );
			break;
		}
	}

	// Find the next available PChannel and return it (up to a maximum of 999)
	dwPChannel = FindNextValidPChannel( dwPChannel + 1 );

	// Update the PChannel number
	m_spinPChannel.SetPos( dwPChannel + 1 );

	// Update the PChannel name
	OnUpdateEditPchannel();

	// Update the state of the 'Remove' button
	OnSelchangeListChannel();
}

void CDlgAddPChannel::OnBtnRemove() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check that something is selected
	if (m_listPChannel.GetSelCount() <= 0)
	{
		return;
	}

	m_listPChannel.SetRedraw( FALSE );

	// Remove channels selected in the list
	for (int nCount = m_listPChannel.GetCount() - 1; nCount >= 0; nCount--) 
	{
		// Check if this string is selected
		if (m_listPChannel.GetSel(nCount)) 
		{
			// Yes - get the PChannel #
			DWORD dwPChannel = m_listPChannel.GetItemData(nCount);			

			// Search for and remove the PChannel from our list
			for( int i=m_adwPChannels.GetUpperBound(); i >= 0; i-- )
			{
				if( m_adwPChannels.GetAt(i) == dwPChannel )
				{
					m_adwPChannels.RemoveAt( i, 1 );
					break;
				}
			}

			// Remove the string from the listbox
			m_listPChannel.DeleteString(nCount);
		}
	}

	m_listPChannel.SetRedraw( TRUE );
	m_listPChannel.Invalidate( TRUE );

	// Update the state of the add/add16 buttons.
	OnUpdateEditPchannel();

	// Update the state of the 'Remove' button
	OnSelchangeListChannel();
}

void CDlgAddPChannel::OnButtonAdd16() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	DWORD dwLastPChannelAdded = 0;
	bool fChanged = false;

	for( int i=0; i < 16; i++ )
	{
		if( m_btnAdd.IsWindowEnabled() )
		{
			// Get the PChannel # to add
			BOOL fTrans;
			DWORD dwPChannel = GetDlgItemInt( IDC_EDIT_PCHANNEL, &fTrans, FALSE );

			// Ensure the PChannel # is valid
			dwPChannel = min( dwPChannel-1, 998 );

			// Do a sorted add to the list of channels
			if( AddToList( dwPChannel ) )
			{
				dwLastPChannelAdded = dwPChannel;
				fChanged = true;
			}

			// Find the next available PChannel and return it (up to a maximum of 999)
			dwPChannel = FindNextValidPChannel( dwPChannel + 1 );

			// Update the PChannel number
			m_spinPChannel.SetPos( dwPChannel + 1 );

			// Update the PChannel name and add/add16 button state
			OnUpdateEditPchannel();
		}
	}

	if( fChanged )
	{
		RefreshListbox();

		// Remove selection from all strings
		m_listPChannel.SetSel(-1, FALSE);

		// Search for the PChannel # we added
		for( int i=0; i < m_listPChannel.GetCount(); i++ )
		{
			if( m_listPChannel.GetItemData( i ) == dwLastPChannelAdded )
			{
				// Select the PChannel
				m_listPChannel.SetSel( i, TRUE );
				break;
			}
		}

		// Update the state of the 'Remove' button
		OnSelchangeListChannel();
	}
}

void CDlgAddPChannel::OnSelchangeListChannel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (m_listPChannel.GetSelCount() > 0)
	{
		m_btnRemove.EnableWindow( TRUE );
	}
	else
	{
		m_btnRemove.EnableWindow( FALSE );
	}
}
