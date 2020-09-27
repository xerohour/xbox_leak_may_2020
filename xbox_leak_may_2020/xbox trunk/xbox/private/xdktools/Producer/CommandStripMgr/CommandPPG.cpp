// CommandPPG.cpp : Implementation of command property page.
//

#include "stdafx.h"
#include "commandstripmgr.h"
#include "CommandPPG.h"
#include <dmusici.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

inline int RepeatModeToIndex( BYTE bRepeatMode )
{
	switch( bRepeatMode )
	{
	case 4:
		return 5;
	case 5:
		return 4;
	default:
		return bRepeatMode;
	}
}

inline BYTE IndexToRepeatMode( int nIndex )
{
	switch( nIndex )
	{
	case 4:
		return 5;
	case 5:
		return 4;
	default:
		return BYTE(nIndex);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCommandPPG property page

IMPLEMENT_DYNCREATE( CCommandPPG, CPropertyPage )

CCommandPPG::CCommandPPG() : CPropertyPage( CCommandPPG::IDD )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pPageManager = NULL;
	ZeroMemory( &m_PPGCommandData, sizeof(PPGCommand) );
	m_cstrOldLevel = _T("75");  // upper bound of old C groove
	m_bEmbCustom = 100;
	//{{AFX_DATA_INIT(CCommandPPG)
	//}}AFX_DATA_INIT
}

CCommandPPG::~CCommandPPG()
{
}

void CCommandPPG::DoDataExchange( CDataExchange* pDX )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPropertyPage::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(CCommandPPG)
	DDX_Control(pDX, IDC_COMBO_REPEAT_PATTERN, m_comboRepeatPattern);
	DDX_Control(pDX, IDC_STATIC_EMB_CUSTOM, m_staticEmbCustom);
	DDX_Control(pDX, IDC_SPIN_GROOVE_RANGE, m_spinGrooveRange);
	DDX_Control(pDX, IDC_SPIN_EMB_CUSTOM, m_spinEmbCustom);
	DDX_Control(pDX, IDC_EDIT_GROOVE_RANGE, m_editGrooveRange);
	DDX_Control(pDX, IDC_EDIT_EMB_CUSTOM, m_editEmbCustom);
	DDX_Control(pDX, IDC_CHECK_GROOVE_LEVEL, m_CheckGrooveLevel);
	DDX_Control(pDX, IDC_SPIN_GROOVE_LEVEL, m_spinGrooveLevel);
	DDX_Control(pDX, IDC_EDIT_GROOVE_LEVEL, m_editGrooveLevel);
	DDX_Control(pDX, IDC_COMMAND_EMBELLISHMENTLIST, m_listboxEmbellishment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( CCommandPPG, CPropertyPage )
	//{{AFX_MSG_MAP(CCommandPPG)
	ON_LBN_SELCHANGE(IDC_COMMAND_EMBELLISHMENTLIST, OnSelchangeCommandEmbellishmentlist)
	ON_EN_CHANGE(IDC_EDIT_GROOVE_LEVEL, OnChangeEditGrooveLevel)
	ON_BN_CLICKED(IDC_CHECK_GROOVE_LEVEL, OnCheckGrooveLevel)
	ON_EN_CHANGE(IDC_EDIT_GROOVE_RANGE, OnChangeEditGrooveRange)
	ON_CBN_SELCHANGE(IDC_COMBO_REPEAT_PATTERN, OnSelchangeComboRepeatPattern)
	ON_EN_KILLFOCUS(IDC_EDIT_EMB_CUSTOM, OnKillfocusEditEmbCustom)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EMB_CUSTOM, OnDeltaposSpinEmbCustom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommandPPG message handlers

void CCommandPPG::OnSelchangeCommandEmbellishmentlist() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( (m_pPageManager == NULL)
	||	(m_pPageManager->m_pIPropPageObject == NULL) )
	{
		return;
	}

	int iIndex = m_listboxEmbellishment.GetCurSel();
	ASSERT( iIndex >= 0 );

	// Set the embellishment.
	BYTE bNewCommand;
	if( iIndex < 6 )
	{
		if( iIndex == 4 )
		{
			bNewCommand = m_bEmbCustom; // Custom
		}
		else
		{
			static const BYTE abIndexToCommand[6] = {
				DMUS_COMMANDT_INTRO,
				DMUS_COMMANDT_FILL,
				DMUS_COMMANDT_BREAK,
				DMUS_COMMANDT_END,
				0,
				DMUS_COMMANDT_GROOVE };
			bNewCommand = abIndexToCommand[iIndex];
		}
	}
	else
	{
		ASSERT(FALSE);
		return;
	}

	if( !(m_PPGCommandData.dwValid & VALID_COMMAND)
	||	(bNewCommand != m_PPGCommandData.bCommand) )
	{
		m_PPGCommandData.dwValid |= VALID_COMMAND;
		m_PPGCommandData.bCommand = bNewCommand;

		// If custom, ensure the edit control displays the correct data
		if( bNewCommand >= 100 )
		{
			SetDlgItemInt( IDC_EDIT_EMB_CUSTOM, bNewCommand, FALSE );
		}

		// Update the state of the 'Custom Embellishment' controls
		EnableItems( TRUE );

		HRESULT hr = m_pPageManager->m_pIPropPageObject->SetData( &m_PPGCommandData );
		ASSERT( SUCCEEDED( hr ));
		if( S_FALSE == hr )
		{
			// The new data was rejected!
			PPGCommand*	pCommand = NULL;

			hr = m_pPageManager->m_pIPropPageObject->GetData( (void **) &pCommand );
			ASSERT( SUCCEEDED( hr ));
			hr = SetData( pCommand );
			ASSERT( SUCCEEDED( hr ));
		}
	}
}

BOOL CCommandPPG::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPropertyPage::OnInitDialog();
	
	// Fill the embellishment listbox
	m_listboxEmbellishment.ResetContent();
	CString strEmbellishment;
	for( int iItem = IDS_INTRO_TEXT; iItem <= IDS_NONE_TEXT; iItem++ )
	{
		strEmbellishment.LoadString( iItem );
		m_listboxEmbellishment.AddString( strEmbellishment );
	}

	m_editGrooveLevel.SetWindowText( _T("") );
	m_editGrooveLevel.LimitText( 3 );
	m_spinGrooveLevel.SetRange(1, 100);

	m_editGrooveRange.SetWindowText( _T("") );
	m_editGrooveRange.LimitText( 3 );
	m_spinGrooveRange.SetRange(0, 100);

	m_editEmbCustom.SetWindowText( _T("") );
	m_editEmbCustom.LimitText( 3 );
	m_spinEmbCustom.SetRange(100, 199);

	m_CheckGrooveLevel.SetCheck(BST_UNCHECKED);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCommandPPG::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Set the controls in case they have changed since this was last activated
	// and RefreshData has not yet been called.
	SetData( &m_PPGCommandData );

	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// CCommandPPG additional functions

void CCommandPPG::EnableItems( BOOL fEnable )
{
	// Embellishment list box
	if( IsWindow( m_listboxEmbellishment.m_hWnd ))
	{
		m_listboxEmbellishment.EnableWindow( fEnable );
	}

	// Groove level edit box
	if( IsWindow( m_editGrooveLevel.m_hWnd ))
	{
		m_editGrooveLevel.EnableWindow( fEnable );
	}
	if( IsWindow( m_spinGrooveLevel.m_hWnd ))
	{
		m_spinGrooveLevel.EnableWindow( fEnable );
	}

	// Groove level check box
	if( IsWindow( m_CheckGrooveLevel.m_hWnd ))
	{
		m_CheckGrooveLevel.EnableWindow( fEnable );
	}

	// Groove range edit box
	if( IsWindow( m_spinGrooveRange.m_hWnd ))
	{
		if( !fEnable
		||	((m_PPGCommandData.dwValid & VALID_GROOVE)
			 &&	(m_PPGCommandData.bGrooveLevel == 0)) )
		{
			m_editGrooveRange.EnableWindow( FALSE );
			m_spinGrooveRange.EnableWindow( FALSE );
		}
		else
		{
			m_editGrooveRange.EnableWindow( TRUE );
			m_spinGrooveRange.EnableWindow( TRUE );
		}
	}

	// Repeat Pattern combo box
	if( IsWindow( m_comboRepeatPattern.m_hWnd ) )
	{
		m_comboRepeatPattern.EnableWindow( fEnable );
	}

	// Custom embellishment edit box (and static text)
	if( IsWindow( m_editEmbCustom.m_hWnd ) )
	{
		if( !fEnable || !(m_PPGCommandData.dwValid & VALID_COMMAND)
		||	(m_PPGCommandData.bCommand < 100) )
		{
			m_editEmbCustom.EnableWindow( FALSE );
			m_spinEmbCustom.EnableWindow( FALSE );
			m_staticEmbCustom.EnableWindow( FALSE );
		}
		else
		{
			m_editEmbCustom.EnableWindow( TRUE );
			m_spinEmbCustom.EnableWindow( TRUE );
			m_staticEmbCustom.EnableWindow( TRUE );
		}
	}
}

HRESULT CCommandPPG::SetData( PPGCommand* pCommand )
{
	if( pCommand == NULL )
	{
		// Disable the property page.
		EnableItems( FALSE );
		return S_OK;
	}

	memcpy( &m_PPGCommandData, pCommand, sizeof( PPGCommand ));

	// Enable the property page
	EnableItems( TRUE );

	// prevent control notifications from being dispatched during UpdateData
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;

	if( !( m_PPGCommandData.dwValid & VALID_COMMAND ))
	{
		// If we don't have a valid embellishment, just don't select anything
		m_listboxEmbellishment.SetCurSel( -1 );
	}
	else
	{
		// Otherwise go select the appropriate value in the listbox.
		int iIndex = -1;

		if( m_PPGCommandData.bCommand < 5 )
		{
			static const int aiCommandToIndex[5] = {
				5, // DMUS_COMMANDT_GROOVE
				1, // DMUS_COMMANDT_FILL
				0, // DMUS_COMMANDT_INTRO
				2, // DMUS_COMMANDT_BREAK
				3};// DMUS_COMMANDT_END

			iIndex = aiCommandToIndex[m_PPGCommandData.bCommand];
		}
		else
		{
			ASSERT( m_PPGCommandData.bCommand >= 100 );
			m_bEmbCustom = m_PPGCommandData.bCommand;
			SetDlgItemInt( IDC_EDIT_EMB_CUSTOM, m_bEmbCustom, FALSE );
			iIndex = 4;
		}

		m_listboxEmbellishment.SetCurSel( iIndex );
	}

	if( ( m_PPGCommandData.dwValid & VALID_GROOVE ))
	{
		if (m_PPGCommandData.bGrooveLevel >= 1 && m_PPGCommandData.bGrooveLevel <= 100)
		{
			m_CheckGrooveLevel.SetCheck(0);

			SetDlgItemInt( IDC_EDIT_GROOVE_LEVEL, m_PPGCommandData.bGrooveLevel, FALSE );
		}
		else
		{
			m_CheckGrooveLevel.SetCheck(1);
			m_editGrooveLevel.SetWindowText( _T("") );
		}
	}
	else
	{
		m_CheckGrooveLevel.SetCheck(1);
		m_editGrooveLevel.SetWindowText( _T("") );
	}

	if( ( m_PPGCommandData.dwValid & VALID_GROOVERANGE ))
	{
		SetDlgItemInt( IDC_EDIT_GROOVE_RANGE, m_PPGCommandData.bGrooveRange / 2, FALSE );
	}
	else
	{
		m_editGrooveRange.SetWindowText( _T("") );
	}

	if( ( m_PPGCommandData.dwValid & VALID_REPEATMODE ))
	{
		int iSel = RepeatModeToIndex( m_PPGCommandData.bRepeatMode );
		ASSERT( iSel < 6 );

		m_comboRepeatPattern.SetCurSel( iSel );
	}
	else
	{
		m_comboRepeatPattern.SetCurSel( -1 );
	}

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;

	return S_OK;
}


void CCommandPPG::OnChangeEditGrooveLevel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !IsWindow( m_spinGrooveLevel.m_hWnd ) )
	{
		return;
	}

	BOOL fTransSuccess;
	long lValue = GetDlgItemInt( IDC_EDIT_GROOVE_LEVEL, &fTransSuccess, FALSE );
	if ( !fTransSuccess || (lValue < 0) )
	{
		lValue = 0;
	}
	else if (lValue > 100)
	{
		lValue = 100;
		m_spinGrooveLevel.SetPos( lValue );
	}

	if( !(m_PPGCommandData.dwValid & VALID_GROOVE)
	||	(m_PPGCommandData.bGrooveLevel != (BYTE)lValue) )
	{
		// Set the groove.
		m_PPGCommandData.dwValid |= VALID_GROOVE;
		m_PPGCommandData.bGrooveLevel = (BYTE)lValue;

		// Update the state of the 'None' checkbox
		if (lValue != 0)
		{
			m_CheckGrooveLevel.SetCheck(BST_UNCHECKED);
			EnableItems(TRUE);
		}
		else
		{
			m_CheckGrooveLevel.SetCheck(BST_CHECKED);
			m_editGrooveLevel.SetWindowText( _T("") );
			EnableItems(TRUE);
		}

		if( m_pPageManager && m_pPageManager->m_pIPropPageObject)
		{
			HRESULT hr = m_pPageManager->m_pIPropPageObject->SetData( &m_PPGCommandData );
			ASSERT( SUCCEEDED( hr ));
			if( S_FALSE == hr )
			{
				// The new data was rejected!
				PPGCommand*	pCommand = NULL;

				hr = m_pPageManager->m_pIPropPageObject->GetData( (void **) &pCommand );
				ASSERT(SUCCEEDED(hr));
				hr = SetData(pCommand);
				ASSERT(SUCCEEDED(hr));
			}
		}
	}
}

void CCommandPPG::OnCheckGrooveLevel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_CheckGrooveLevel.SetCheck(m_CheckGrooveLevel.GetCheck() ? BST_UNCHECKED : BST_CHECKED);
	if (m_CheckGrooveLevel.GetCheck())
	{
		m_editGrooveLevel.GetWindowText(m_cstrOldLevel);
		m_editGrooveLevel.SetWindowText(_T(""));
	}
	else
	{
		m_editGrooveLevel.SetWindowText(m_cstrOldLevel);
	}
	OnSetActive();
}

void CCommandPPG::OnKillfocusEditEmbCustom() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !IsWindow( m_spinEmbCustom.m_hWnd ) )
	{
		return;
	}

	BOOL fTransSuccess;
	long lValue = GetDlgItemInt( IDC_EDIT_EMB_CUSTOM, &fTransSuccess, FALSE );
	if ( !fTransSuccess || (lValue < 100) )
	{
		lValue = 100;
		m_spinEmbCustom.SetPos( lValue );
	}
	else if (lValue > 199)
	{
		lValue = 199;
		m_spinEmbCustom.SetPos( lValue );
	}

	if( !(m_PPGCommandData.dwValid & VALID_COMMAND)
	||	(m_PPGCommandData.bCommand != (BYTE)lValue) )
	{
		// Set the custom embellishment
		m_PPGCommandData.dwValid |= VALID_COMMAND;
		m_PPGCommandData.bCommand = (BYTE)lValue;
		m_bEmbCustom = (BYTE)lValue;

		if( m_pPageManager && m_pPageManager->m_pIPropPageObject)
		{
			HRESULT hr = m_pPageManager->m_pIPropPageObject->SetData( &m_PPGCommandData );
			ASSERT( SUCCEEDED( hr ));
			if( S_FALSE == hr )
			{
				// The new data was rejected!
				PPGCommand*	pCommand = NULL;

				hr = m_pPageManager->m_pIPropPageObject->GetData( (void **) &pCommand );
				ASSERT(SUCCEEDED(hr));
				hr = SetData(pCommand);
				ASSERT(SUCCEEDED(hr));
			}
		}
	}
}

void CCommandPPG::OnChangeEditGrooveRange() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !IsWindow( m_spinEmbCustom.m_hWnd ) )
	{
		return;
	}

	BOOL fTransSuccess;
	long lValue = GetDlgItemInt( IDC_EDIT_GROOVE_RANGE, &fTransSuccess, FALSE );
	if ( !fTransSuccess || (lValue < 0) )
	{
		lValue = 0;
		m_spinGrooveRange.SetPos( lValue );
	}
	else if (lValue > 100)
	{
		lValue = 100;
		m_spinGrooveRange.SetPos( lValue );
	}

	if( !(m_PPGCommandData.dwValid & VALID_GROOVERANGE)
	||	(m_PPGCommandData.bGrooveRange != (BYTE)(lValue * 2)) )
	{
		// Set the groove range
		m_PPGCommandData.dwValid |= VALID_GROOVERANGE;
		m_PPGCommandData.bGrooveRange = (BYTE)(lValue * 2);

		if( m_pPageManager && m_pPageManager->m_pIPropPageObject)
		{
			HRESULT hr = m_pPageManager->m_pIPropPageObject->SetData( &m_PPGCommandData );
			ASSERT( SUCCEEDED( hr ));
			if( S_FALSE == hr )
			{
				// The new data was rejected!
				PPGCommand*	pCommand = NULL;

				hr = m_pPageManager->m_pIPropPageObject->GetData( (void **) &pCommand );
				ASSERT(SUCCEEDED(hr));
				hr = SetData(pCommand);
				ASSERT(SUCCEEDED(hr));
			}
		}
	}
}

void CCommandPPG::OnSelchangeComboRepeatPattern() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( (m_pPageManager == NULL)
	||	(m_pPageManager->m_pIPropPageObject == NULL) )
	{
		return;
	}

	// Get and validate the repeat pattern.
	int iIndex = m_comboRepeatPattern.GetCurSel();
	if( (iIndex >= 6)
	||	(iIndex < 0 ) )
	{
		ASSERT(FALSE);
		return;
	}

	BYTE bNewRepeatMode = IndexToRepeatMode( iIndex );

	if( !(m_PPGCommandData.dwValid & VALID_REPEATMODE)
	||	(bNewRepeatMode != m_PPGCommandData.bRepeatMode) )
	{
		m_PPGCommandData.dwValid |= VALID_REPEATMODE;
		m_PPGCommandData.bRepeatMode = bNewRepeatMode;

		HRESULT hr = m_pPageManager->m_pIPropPageObject->SetData( &m_PPGCommandData );
		ASSERT( SUCCEEDED( hr ));
		if( S_FALSE == hr )
		{
			// The new data was rejected!
			PPGCommand*	pCommand = NULL;

			hr = m_pPageManager->m_pIPropPageObject->GetData( (void **) &pCommand );
			ASSERT( SUCCEEDED( hr ));
			hr = SetData( pCommand );
			ASSERT( SUCCEEDED( hr ));
		}
	}
}

void CCommandPPG::OnDeltaposSpinEmbCustom(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	OnKillfocusEditEmbCustom();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nCustomEmb = m_spinEmbCustom.GetPos();

	if( HIWORD(nCustomEmb) == 0 )
	{
		int nNewCustomEmb = LOWORD(nCustomEmb) + pNMUpDown->iDelta;

		if( nNewCustomEmb < 100)
		{
			nNewCustomEmb = 100;
		}
	
		if( nNewCustomEmb > 199)
		{
			nNewCustomEmb = 199;
		}

		m_spinEmbCustom.SetPos( nNewCustomEmb );

		if( !(m_PPGCommandData.dwValid & VALID_COMMAND)
		||	(m_PPGCommandData.bCommand != (BYTE)nNewCustomEmb) )
		{
			// Set the custom embellishment
			m_PPGCommandData.dwValid |= VALID_COMMAND;
			m_PPGCommandData.bCommand = (BYTE)nNewCustomEmb;
			m_bEmbCustom = (BYTE)nNewCustomEmb;

			if( m_pPageManager && m_pPageManager->m_pIPropPageObject)
			{
				HRESULT hr = m_pPageManager->m_pIPropPageObject->SetData( &m_PPGCommandData );
				ASSERT( SUCCEEDED( hr ));
				if( S_FALSE == hr )
				{
					// The new data was rejected!
					PPGCommand*	pCommand = NULL;

					hr = m_pPageManager->m_pIPropPageObject->GetData( (void **) &pCommand );
					ASSERT(SUCCEEDED(hr));
					hr = SetData(pCommand);
					ASSERT(SUCCEEDED(hr));
				}
			}
		}
	}

	*pResult = 1;
}
