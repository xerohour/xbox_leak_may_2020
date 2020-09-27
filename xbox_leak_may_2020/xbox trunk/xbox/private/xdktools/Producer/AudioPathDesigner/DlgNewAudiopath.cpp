// DlgNewAudiopath.cpp : implementation file
//

#include "stdafx.h"
#include "DlgNewAudiopath.h"
#include "AudioPathComponent.h"
#include "AudioPath.h"
#include "dmusici.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewAudiopath dialog


CDlgNewAudiopath::CDlgNewAudiopath(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewAudiopath::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewAudiopath)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pAudioPathComponent = NULL;
	m_fBaseOnExistingAudiopath = false;
	m_pExistingAudiopath = NULL;
	m_dwDefaultAudiopathID = 0;
}


void CDlgNewAudiopath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewAudiopath)
	DDX_Control(pDX, IDC_AUDIOPATH_LIST, m_listAudiopath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewAudiopath, CDialog)
	//{{AFX_MSG_MAP(CDlgNewAudiopath)
	ON_BN_CLICKED(IDC_NEW_AUDIOPATH, OnNewAudiopath)
	ON_BN_CLICKED(IDC_EXISTING_AUDIOPATH, OnExistingAudiopath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewAudiopath message handlers

void CDlgNewAudiopath::OnOK() 
{
	if( GetCheckedRadioButton( IDC_NEW_AUDIOPATH, IDC_EXISTING_AUDIOPATH ) == IDC_EXISTING_AUDIOPATH )
	{
		int nSel = m_listAudiopath.GetCurSel();
		if( nSel != LB_ERR )
		{
			DWORD dwData = m_listAudiopath.GetItemData( nSel );
			if( (dwData >= DMUS_APATH_SHARED_STEREOPLUSREVERB)
			&&	(dwData <= DMUS_APATH_DYNAMIC_STEREO) )
			{
				m_fBaseOnExistingAudiopath = true;
				m_dwDefaultAudiopathID = dwData;
			}
			else
			{
				m_pExistingAudiopath = (CDirectMusicAudioPath *)m_listAudiopath.GetItemDataPtr( nSel );
				if( m_pExistingAudiopath )
				{
					m_fBaseOnExistingAudiopath = true;
				}
			}
		}
	}

	CDialog::OnOK();
}

void AddStandardStringToList( int nStringID, DWORD dwStandardID, CListBox &listBox )
{
	CString strName;
	if( strName.LoadString( nStringID ) )
	{
		int nPos = listBox.InsertString( 0, strName );
		listBox.SetItemData( nPos, dwStandardID );
	}
}

BOOL CDlgNewAudiopath::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( m_pAudioPathComponent )
	{
		POSITION pos = m_pAudioPathComponent->m_lstAudioPaths.GetHeadPosition();
		while( pos )
		{
			CDirectMusicAudioPath *pAudioPath = m_pAudioPathComponent->m_lstAudioPaths.GetNext( pos );

			BSTR bstrName = pAudioPath->GetNameForConductor();
			if( bstrName )
			{
				CString strName = bstrName;
				::SysFreeString( bstrName );
				int nPos = m_listAudiopath.AddString( strName );
				m_listAudiopath.SetItemDataPtr( nPos, pAudioPath );
			}
		}
	}

	AddStandardStringToList( IDS_APATH_STEREO, DMUS_APATH_DYNAMIC_STEREO, m_listAudiopath );
	AddStandardStringToList( IDS_APATH_MONO, DMUS_APATH_DYNAMIC_MONO, m_listAudiopath );
	AddStandardStringToList( IDS_APATH_3D_DRY, DMUS_APATH_DYNAMIC_3D, m_listAudiopath );
	//AddStandardStringToList( IDS_APATH_3D, DMUS_APATH_DYNAMIC_ENV3D, m_listAudiopath );
	AddStandardStringToList( IDS_APATH_MUSIC, DMUS_APATH_SHARED_STEREOPLUSREVERB, m_listAudiopath );

	CheckRadioButton( IDC_NEW_AUDIOPATH, IDC_EXISTING_AUDIOPATH, IDC_NEW_AUDIOPATH );

	CWnd *pWnd = GetDlgItem( IDC_EXISTING_AUDIOPATH );
	if( pWnd )
	{
		pWnd->EnableWindow( (m_listAudiopath.GetCount() > 0) ? TRUE : FALSE );
	}

	// Disable the list until the 'use existing' button is checked
	m_listAudiopath.EnableWindow( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNewAudiopath::OnNewAudiopath() 
{
	// Disable the list since the 'use existing' button is unchecked
	m_listAudiopath.EnableWindow( FALSE );
}

void CDlgNewAudiopath::OnExistingAudiopath() 
{
	// Enable the list since the 'use existing' button is checked
	m_listAudiopath.EnableWindow( TRUE );

	if( m_listAudiopath.GetCurSel() == LB_ERR )
	{
		m_listAudiopath.SetCurSel( 0 );
	}
}
