// AddTrackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "segmentdesignerdll.h"
#include "AddTrackDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddTrackDlg dialog


CAddTrackDlg::CAddTrackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddTrackDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddTrackDlg)
	//}}AFX_DATA_INIT
	m_dwCount = 0;
	m_pGuid = NULL;
}


CAddTrackDlg::~CAddTrackDlg()
{
	if( m_pGuid )
	{
		delete[] m_pGuid;
	}
}


void CAddTrackDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddTrackDlg)
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_LIST_TRACK, m_listTrack);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddTrackDlg, CDialog)
	//{{AFX_MSG_MAP(CAddTrackDlg)
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDC_LIST_TRACK, OnDblclkListTrack)
	ON_LBN_SELCHANGE(IDC_LIST_TRACK, OnSelChangeListTrack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddTrackDlg message handlers

BOOL CAddTrackDlg::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	LONG	lResult;

	TCHAR	szRegPath[MAX_BUFFER];
	HKEY	hKeyStripEditors;

	_tcscpy( szRegPath, _T("Software\\Microsoft\\DMUSProducer\\StripEditors") );
	lResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, szRegPath, 0, KEY_READ, &hKeyStripEditors );
	if ( lResult != ERROR_SUCCESS )
	{
		TRACE("CAddTrackDlg: Unable to open Software\\Microsoft\\DMUSProducer\\StripEditors key.\n");
		return TRUE;
	}

	TCHAR	szSubKey[MID_BUFFER], szStripName[MID_BUFFER];
	DWORD	dwType, dwIndex = 0, dwCbData = MID_BUFFER;
	int		nIndex;
	HKEY	hKeyStrip;

	while( ::RegEnumKeyEx( hKeyStripEditors, dwIndex, szSubKey, &dwCbData,
						   NULL, NULL, NULL, NULL ) == ERROR_SUCCESS )
	{
		if( ::RegOpenKeyEx( hKeyStripEditors, szSubKey, 0, KEY_READ, &hKeyStrip ) == ERROR_SUCCESS )
		{
			dwCbData = MID_BUFFER;
			if( ::RegQueryValueEx( hKeyStrip, NULL, NULL, &dwType, (BYTE *)&szStripName, &dwCbData ) == ERROR_SUCCESS &&
				dwType == REG_SZ )
			{
				wchar_t awchGuid[80];
				if( MultiByteToWideChar( CP_ACP, 0, szSubKey, -1, awchGuid, sizeof(awchGuid) / sizeof(wchar_t) ) != 0 )
				{
					GUID *pGuid = new GUID;
					if( SUCCEEDED( IIDFromString( awchGuid, pGuid ) ) )
					{
						nIndex = m_listTrack.AddString( szStripName );
						m_listTrack.SetItemDataPtr( nIndex, (void *)pGuid );
					}
					else
					{
						TRACE("CAddTrackDlg: Unable to convert %s to a GUID.\n",szSubKey);
						delete pGuid;
					}
				}
				else
				{
					TRACE("CAddTrackDlg: Unable to convert %s to a WideChar string.\n",szSubKey);
				}
			}
			else
			{
				TRACE("CAddTrackDlg: Unable to query default value of %s.\n",szSubKey);
			}
			::RegCloseKey( hKeyStrip );
		}
		else
		{
			TRACE("CAddTrackDlg: Unable to open key %s.\n",szSubKey);
		}
		dwIndex++;
		dwCbData = MID_BUFFER;
	}
	::RegCloseKey( hKeyStripEditors );

	if( m_listTrack.GetCount() > 0 )
	{
		m_listTrack.SetSel( 0, TRUE );
		OnSelChangeListTrack();
	}

	if( !m_strSegmentName.IsEmpty() )
	{
		CString strTitle;
		GetWindowText( strTitle );
		strTitle = m_strSegmentName + _T(" - ") + strTitle;
		SetWindowText( strTitle );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddTrackDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	GUID *pGuid;
	CDialog::OnDestroy();
	
	while( m_listTrack.GetCount() > 0 )
	{
		pGuid = (GUID *) m_listTrack.GetItemDataPtr( 0 );
		delete pGuid;
		m_listTrack.DeleteString( 0 );
	}
}

void CAddTrackDlg::OnOK() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the number of selected tracks
	int nSelected;
	nSelected = m_listTrack.GetSelCount();
	if( (nSelected != LB_ERR) && (nSelected != 0) )
	{
		// If no error, and at least one selection, create an array to store the indices in
		int *pnIndices, nTemp;
		pnIndices = new int[nSelected];
		ASSERT( pnIndices );

		// Get the array of indices
		nTemp = m_listTrack.GetSelItems( nSelected, pnIndices );
		ASSERT( nTemp == nSelected );
		if( nTemp == nSelected )
		{
			// We got however many we asked for
			// If the array shouldn't already exist
			ASSERT( m_pGuid == NULL );

			// Set the number of GUIDs, and create an array to store them in
			m_dwCount = nSelected;
			m_pGuid = new GUID[nSelected];

			// Copy the GUIDs to our local array
			GUID *pGuid;
			for( nTemp = 0; nTemp < nSelected; nTemp++ )
			{
				pGuid = (GUID *) m_listTrack.GetItemDataPtr( pnIndices[nTemp] );
				ASSERT( pGuid );
				memcpy( &(m_pGuid[nTemp]), pGuid, sizeof(GUID) );
			}
		}

		// Delete the array that stored the selected items' indices
		delete[] pnIndices;
	}
	
	CDialog::OnOK();
}

void CAddTrackDlg::OnDblclkListTrack() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Get the number of selected tracks
	int nSelected;
	nSelected = m_listTrack.GetSelCount();
	if( (nSelected != LB_ERR) && (nSelected != 0) )
	{
		// If no error, and at least one selection, create an array to store the indices in
		int *pnIndices, nTemp;
		pnIndices = new int[nSelected];
		ASSERT( pnIndices );

		// Get the array of indices
		nTemp = m_listTrack.GetSelItems( nSelected, pnIndices );
		ASSERT( nTemp == nSelected );
		if( nTemp == nSelected )
		{
			// We got however many we asked for
			// If the array shouldn't already exist
			ASSERT( m_pGuid == NULL );

			// Set the number of GUIDs, and create an array to store them in
			m_dwCount = nSelected;
			m_pGuid = new GUID[nSelected];

			// Copy the GUIDs to our local array
			GUID *pGuid;
			for( nTemp = 0; nTemp < nSelected; nTemp++ )
			{
				pGuid = (GUID *) m_listTrack.GetItemDataPtr( pnIndices[nTemp] );
				ASSERT( pGuid );
				memcpy( &(m_pGuid[nTemp]), pGuid, sizeof(GUID) );
			}
		}

		// Delete the array that stored the selected items' indices
		delete[] pnIndices;
	}
	
	CDialog::OnOK();
}

void CAddTrackDlg::OnSelChangeListTrack() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	int nSelected = m_listTrack.GetSelCount();

	if( (nSelected == LB_ERR)
	||  (nSelected == 0) )
	{
		m_btnOK.EnableWindow( FALSE );
	}
	else
	{
		m_btnOK.EnableWindow( TRUE );
	}
}
