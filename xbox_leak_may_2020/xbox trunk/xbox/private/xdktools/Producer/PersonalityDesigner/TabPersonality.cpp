// TabPersonality.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "PersonalityDesigner.h"
#include "Personality.h"
#include "TabPersonality.h"
#include "DlgChangeLength.h"
#include "LockoutNotification.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabPersonality property page

CTabPersonality::CTabPersonality( CPersonalityPageManager* pPageManager ) : CPropertyPage(CTabPersonality::IDD)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	//{{AFX_DATA_INIT(CTabPersonality)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );

	m_pPageManager = pPageManager;
//	m_pPageManager->AddRef();		intentionally missing

	m_fHaveData = FALSE;
	m_fInOnSetActive = FALSE;
	m_fNeedToDetach = FALSE;

	m_fUseTimeSignature = FALSE;
	m_nBPM = 0;
	m_nBeat = 0;
	m_nChordMapLength = 0;
	m_dwKey = 12;
	m_lScalePattern = 0xab5ab5;
	m_fVariableNotFixed = FALSE;
}

CTabPersonality::~CTabPersonality()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::CopyDataToTab

void CTabPersonality::CopyDataToTab( tabPersonality* pTabData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pTabData )
	{
		m_fHaveData = TRUE;

		m_strName = pTabData->strName;
//		m_strRefName = pTabData->strRefName;
		m_strDescription = pTabData->strDescription;

		m_fUseTimeSignature = pTabData->fUseTimeSignature;
		m_fVariableNotFixed = pTabData->fVariableNotFixed;
		m_nBPM = pTabData->nBPM;
		m_nBeat = pTabData->nBeat;
		m_dwKey = pTabData->dwKey;
		m_lScalePattern = pTabData->lScalePattern;
		m_nChordMapLength = pTabData->nChordMapLength;
	}
	else
	{
		m_fHaveData = FALSE;

		m_strName.Empty();
//		m_strRefName.Empty();
		m_strDescription.Empty();

		m_fUseTimeSignature = FALSE;
		m_fVariableNotFixed = FALSE;
		m_nBPM = 0;
		m_nBeat = 0;
		m_nChordMapLength = 0;

		m_dwKey = 12;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::GetDataFromTab

void CTabPersonality::GetDataFromTab( tabPersonality* pTabData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	pTabData->strName = m_strName;
//	pTabData->strRefName = m_strRefName;
	pTabData->strDescription = m_strDescription;

	pTabData->fUseTimeSignature = m_fUseTimeSignature;
	pTabData->fVariableNotFixed = m_fVariableNotFixed;
	pTabData->nBPM = m_nBPM;
	pTabData->nBeat = m_nBeat;
	pTabData->dwKey = m_dwKey;
	pTabData->lScalePattern = m_lScalePattern;
	pTabData->nChordMapLength = m_nChordMapLength;
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::EnableControls

void CTabPersonality::EnableControls( BOOL fEnable ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_editName.EnableWindow( fEnable );
//	m_editRefName.EnableWindow( fEnable );
	m_editDescription.EnableWindow( fEnable );

//	m_checkUseTimeSig.EnableWindow( FALSE );	// TEMP - FALSE until free form editing supported
//	m_checkUseTimeSig.EnableWindow( fEnable );
/*
	if( m_fUseTimeSignature == FALSE )
	{
		fEnable = FALSE;
	}
*/
	m_staticTimeSig.EnableWindow( fEnable );
	m_editBPM.EnableWindow( fEnable );
	m_spinBPM.EnableWindow( fEnable );
	m_comboBeat.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::SetTimeSignature

void CTabPersonality::SetTimeSignature( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fUseTimeSignature == FALSE )
	{
		m_staticTimeSig.SetWindowText( _T("") );
		m_editBPM.SetWindowText( _T("") );
		m_comboBeat.SetCurSel( -1 );
		m_spinBPM.SetPos( 1 );
		return;
	}

	CString strText;

	strText.Format( "%d/%d", m_nBPM, m_nBeat );
	m_staticTimeSig.SetWindowText( strText );

	strText.Format( "%d", m_nBPM );
	m_editBPM.SetWindowText( strText );
	m_spinBPM.SetPos( m_nBPM  );

    int i;
	for( i = 0 ;  i < 5 ;  i++ )
	{
        if( (1 << i) == m_nBeat )
            break ;
    }
	m_comboBeat.SetCurSel( i );
}


void CTabPersonality::DoDataExchange( CDataExchange* pDX )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabPersonality)
	DDX_Control(pDX, IDC_STATICTIMESIG, m_staticTimeSig);
	DDX_Control(pDX, IDC_SPINKEY, m_spinkey);
	DDX_Control(pDX, IDC_SPIN_BPM, m_spinBPM);
	DDX_Control(pDX, IDC_NAME, m_editName);
	DDX_Control(pDX, IDC_DESCRIPTION, m_editDescription);
	DDX_Control(pDX, IDC_COMBO_BEAT, m_comboBeat);
	DDX_Control(pDX, IDC_BPM, m_editBPM);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabPersonality, CPropertyPage)
	//{{AFX_MSG_MAP(CTabPersonality)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_REF_NAME, OnChangeRefName)
	ON_EN_CHANGE(IDC_DESCRIPTION, OnChangeDescription)
	ON_CBN_SELCHANGE(IDC_COMBO_BEAT, OnSelchangeComboBeat)
	ON_EN_CHANGE(IDC_BPM, OnChangeBPM)
	ON_BN_CLICKED(IDC_CHANGELENGTH, OnChangeLength)
	ON_EN_KILLFOCUS(IDC_NAME, OnKillfocusName)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINKEY, OnDeltaposSpinkey)
	ON_BN_CLICKED(IDC_USEFLATS, OnUseflats)
	ON_BN_CLICKED(IDC_USESHARPS, OnUsesharps)
	ON_BN_CLICKED(IDC_RADIOFIX, OnRadiofix)
	ON_BN_CLICKED(IDC_RADIOVAR, OnRadiovar)
	ON_EN_KILLFOCUS(IDC_REF_NAME, OnKillfocusRefName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabPersonality message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::OnSetActive

BOOL CTabPersonality::OnSetActive( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pPageManager && m_pPageManager->m_pIPropPageObject )
	{
		CLockoutNotification lock(m_hWnd);	// keep notifications out while we are here!
		// Set enable state of controls
		BOOL fEnable;

		if( m_fHaveData )
		{
			fEnable = TRUE;
		}
		else
		{
			fEnable = FALSE;
		}
		EnableControls( fEnable );

		m_pPageManager->RefreshData();

		// Place values in controls
		m_fInOnSetActive = TRUE;
		m_editName.SetWindowText( m_strName );
	//	m_editRefName.SetWindowText( m_strRefName );
		m_editDescription.SetWindowText( m_strDescription );
	//	m_checkUseTimeSig.SetCheck( m_fUseTimeSignature );
		SetTimeSignature();

		char str[30];
		CPersonality::RootToString(str, m_dwKey);
		SetDlgItemText(IDC_EDITKEY, str);

		if(CPersonality::GetFlatsFlag(m_dwKey))
		{
			CheckRadioButton(IDC_USESHARPS, IDC_USEFLATS , IDC_USEFLATS);
		}
		else
		{
			CheckRadioButton(IDC_USESHARPS, IDC_USEFLATS, IDC_USESHARPS);
		}
		
		if(m_fVariableNotFixed)
		{
			CheckRadioButton( IDC_RADIOFIX, IDC_RADIOVAR, IDC_RADIOVAR );
		}
		else
		{
			CheckRadioButton( IDC_RADIOFIX, IDC_RADIOVAR, IDC_RADIOFIX );
		}
		
		m_fInOnSetActive = FALSE;
	}
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::OnInitDialog

BOOL CTabPersonality::OnInitDialog( void ) 
{
	// Need this AFX_MANAGE_STATE so that resource defined
	// combo box strings can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();
	
	m_editName.LimitText( 20 );
//	m_editRefName.LimitText( 20 );
	m_editDescription.LimitText( 80 );
	m_editBPM.LimitText( 2 );
	m_spinBPM.SetRange( 1,99 );
//	m_spinkey.SetRange(0,23);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::OnCreate

int CTabPersonality::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
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
// CTabPersonality::OnDestroy

void CTabPersonality::OnDestroy( void ) 
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
// CTabPersonality::OnChangeName

void CTabPersonality::OnChangeName( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
/*
	if( m_fInOnSetActive )
	{
		return;
	}

	// Save the current caret position because UpdateObjectWithTabData changes
	// it.
	DWORD dwSel = m_editName.GetSel();

	m_editName.GetWindowText( m_strName );
	m_pPageManager->UpdateObjectWithTabData();

	
	// Restore current caret position.
	m_editName.SetFocus();
	m_editName.SetSel( dwSel, TRUE );
*/
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::OnChangeRefName

void CTabPersonality::OnChangeRefName( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
/*
	if( m_fInOnSetActive )
	{
		return;
	}

	m_editRefName.GetWindowText( m_strRefName );
	m_pPageManager->UpdateObjectWithTabData();
*/
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::OnChangeDescription

void CTabPersonality::OnChangeDescription( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fInOnSetActive )
	{
		return;
	}

	m_editDescription.GetWindowText( m_strDescription );
	m_pPageManager->UpdateObjectWithTabData();
}




/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::OnChangeBPM

void CTabPersonality::OnChangeBPM( void ) 
{
	// Need this AFX_MANAGE_STATE so that error
	// message string resource can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (m_fInOnSetActive == TRUE)
	||  (m_fUseTimeSignature == FALSE) )
	{
		return;
	}

	CString strBPM;

	m_editBPM.GetWindowText( strBPM );

	short nBPM = (short)atoi( strBPM );
	if( (nBPM < 0)
	||  (nBPM > 100) )
	{
		AfxMessageBox( IDS_ERR_BPM_VALUE );
		strBPM.Format( "%d", m_nBPM );
		m_editBPM.SetWindowText( strBPM );
		m_editBPM.SetFocus();
		m_editBPM.SetSel( 0, -1 );
		return;
	}

	CString strText;

	m_nBPM = nBPM;
	strText.Format( "%d/%d", m_nBPM, m_nBeat );
	m_staticTimeSig.SetWindowText( strText );
	m_pPageManager->UpdateObjectWithTabData();
}


/////////////////////////////////////////////////////////////////////////////
// CTabPersonality::OnSelchangeComboBeat

void CTabPersonality::OnSelchangeComboBeat( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fInOnSetActive )
	{
		return;
	}

	int nCursel = m_comboBeat.GetCurSel();
	if( nCursel != CB_ERR )
	{
		CString strText;

		m_nBeat = (short)(1 << nCursel) ;
		strText.Format( "%d/%d", m_nBPM, m_nBeat );
		m_staticTimeSig.SetWindowText( strText );
		m_pPageManager->UpdateObjectWithTabData();
	}
}

void CTabPersonality::OnChangeLength() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fInOnSetActive )
	{
		return;
	}
	CDlgChangeLength dlg(this);
	dlg.SetChordMapLength(m_nChordMapLength);
	int retval = dlg.DoModal();
	if(retval == IDOK)
	{
		CString s;
		bool bMakeChange = true;
		short n = (short)atoi((LPCSTR)dlg.m_strLength);
		if(n < 1)
		{
			s.LoadString(IDS_LENGTHTOOSMALL);
			AfxMessageBox(s);
			n = 1;
		}
		else if(n > 100)
		{
			s.LoadString(IDS_LENGTHTOOLARGE);
			AfxMessageBox(s);
			n = 100;
		}
		if(n < m_nChordMapLength)
		{
			if(m_pPageManager->GetPersonality()->CheckForData(n))
			{
				s.LoadString(IDS_SHORTEN_LENGTH);
				retval = AfxMessageBox(s, MB_YESNO);
				if(retval != IDYES)
				{
					bMakeChange = false;
				}
			}
		}
		if(bMakeChange)
		{
			m_nChordMapLength = n;
			m_pPageManager->UpdateObjectWithTabData();
		}
	}
}

void CTabPersonality::OnKillfocusName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString strName;

	if( m_fInOnSetActive )
	{
		return;
	}
	m_editName.GetWindowText(strName);

	// strip leading and trailing ws
	strName.TrimRight();
	strName.TrimLeft();

	if(strName.IsEmpty())
	{
		m_editName.SetWindowText(m_strName);
	}
	else
	{
		if(strName.Compare(m_strName) != 0)
		{
		/* ref name no longer editable by user
			if(m_strName.Compare(m_strRefName) == 0)
			{
				// keep in sync
				m_strRefName = strName;
			}
		*/
			m_strName = strName;
			m_pPageManager->UpdateObjectWithTabData();
		}
	}
}

void CTabPersonality::OnDeltaposSpinkey(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if( m_fInOnSetActive )
	{
		return;
	}
	bool useflats = CPersonality::GetFlatsFlag(m_dwKey);
	CPersonality::SetFlatsFlag(false, m_dwKey);

	DWORD oldkey = m_dwKey & ~CPersonality::UseFlats;

	m_dwKey -= pNMUpDown->iDelta;

	if(m_dwKey < 0 || m_dwKey >= 0x80000000)
		m_dwKey = 0;
	if(m_dwKey > 23)
		m_dwKey = 23;

	CPersonality::SetFlatsFlag(useflats, m_dwKey);

	DWORD keydiff = (m_dwKey & ~CPersonality::UseFlats) - oldkey;
	if(keydiff != 0)
	{
		// adjust scale;
//		m_lScalePattern = Rotate24(m_lScalePattern, keydiff);
		// update
		OnChangeRoot();
	}
	
	*pResult = 0;
}

void CTabPersonality::OnChangeRoot()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	char str[30];

	if( m_fInOnSetActive )
	{
		return;
	}
	CPersonality::RootToString(str, m_dwKey);
	SetDlgItemText(IDC_EDITKEY, str);

	m_pPageManager->UpdateObjectWithTabData();
}

void CTabPersonality::OnUseflats() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_fInOnSetActive )
	{
		return;
	}
	if(!CPersonality::GetFlatsFlag(m_dwKey))
	{
		CPersonality::SetFlatsFlag(true, m_dwKey);
		OnChangeRoot();
	}
}

void CTabPersonality::OnUsesharps() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_fInOnSetActive )
	{
		return;
	}
	if(CPersonality::GetFlatsFlag(m_dwKey))
	{
		CPersonality::SetFlatsFlag(false, m_dwKey);
		OnChangeRoot();
	}	
}

void CTabPersonality::OnRadiofix() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_fInOnSetActive )
	{
		return;
	}
	m_fVariableNotFixed = FALSE;
	CheckRadioButton( IDC_RADIOFIX, IDC_RADIOVAR, IDC_RADIOFIX );
	m_pPageManager->UpdateObjectWithTabData();	
}

void CTabPersonality::OnRadiovar() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_fInOnSetActive )
	{
		return;
	}
	m_fVariableNotFixed = TRUE;
	CheckRadioButton( IDC_RADIOFIX, IDC_RADIOVAR, IDC_RADIOVAR );
	m_pPageManager->UpdateObjectWithTabData();
}

void CTabPersonality::OnKillfocusRefName() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
/* ref name no longer editable by user
	CString strName;

	m_editRefName.GetWindowText(strName);

	// strip leading and trailing ws
	strName.TrimRight();
	strName.TrimLeft();

	if(strName.IsEmpty())
	{
		m_editRefName.SetWindowText(m_strRefName);
	}
	else
	{
		if(strName.Compare(m_strRefName) != 0)
		{
			m_strRefName = strName;
			m_pPageManager->UpdateObjectWithTabData();
		}
	}
*/
}
