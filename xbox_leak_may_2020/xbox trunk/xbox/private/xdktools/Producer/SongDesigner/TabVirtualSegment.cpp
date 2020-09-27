// TabVirtualSegment.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"
#include "resource.h"

#include "Song.h"
#include "TabVirtualSegment.h"
#include "DlgSetLength.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Length (in characters) of the string to allocate to store text entered in
// the measure/beat/grid/tick edit boxes
#define DIALOG_EDIT_LEN 15

#define MAX_TICK (DMUS_PPQ << 1)


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment property page

CTabVirtualSegment::CTabVirtualSegment( IDMUSProdPropPageManager* pIPageManager ) : CPropertyPage(CTabVirtualSegment::IDD)
{
	//{{AFX_DATA_INIT(CTabVirtualSegment)
	//}}AFX_DATA_INIT

	ASSERT( pIPageManager != NULL );

	m_pIPropPageObject = NULL;
	m_pIPageManager = pIPageManager;
	m_lTrackGroupBits = 0;
	m_fNeedToDetach = FALSE;
}

CTabVirtualSegment::~CTabVirtualSegment()
{
	RELEASE( m_pIPropPageObject );
}


void CTabVirtualSegment::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabVirtualSegment)
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_BUTTON_LENGTH, m_btnLength);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_BAR, m_editStartMeasure);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_BEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_GRID, m_editStartGrid);
	DDX_Control(pDX, IDC_EDIT_PLAY_START_TICK, m_editStartTick);
	DDX_Control(pDX, IDC_EDIT_TRACK_GROUP, m_editTrackGroup);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_BAR, m_spinStartMeasure);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_BEAT, m_spinStartBeat);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_GRID, m_spinStartGrid);
	DDX_Control(pDX, IDC_SPIN_PLAY_START_TICK, m_spinStartTick);
	DDX_Control(pDX, IDC_SPIN_TRACK_GROUP, m_spinTrackGroup);
	DDX_Control(pDX, IDC_CHECK_EXTEND_TIMESIG, m_checkExtendTimeSig);
	DDX_Control(pDX, IDC_CHECK_AUDIOPATH, m_checkAudioPath);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::SetObject

void CTabVirtualSegment::SetObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->Release();
	}
	m_pIPropPageObject = pIPropPageObject;
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->AddRef();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::UpdateObject

void CTabVirtualSegment::UpdateObject()
{
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->SetData( (void *)&m_PPGTabVirtualSegment );
	}

	RefreshTab();
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::RefreshTab

void CTabVirtualSegment::RefreshTab( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	if( m_pIPropPageObject == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	PPGTabVirtualSegment *pPPGTabVirtualSegment= &m_PPGTabVirtualSegment;
	if( FAILED( m_pIPropPageObject->GetData( (void**)&pPPGTabVirtualSegment ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	if( !(m_PPGTabVirtualSegment.dwFlagsUI & PROPF_ONE_SELECTED) )
	{
		EnableControls( FALSE );
		return;
	}

	// Prevent control notifications from being dispatched during RefreshTab
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;

	EnableControls( TRUE );

	// "Name"
	m_editName.SetWindowText( m_PPGTabVirtualSegment.strName );

	if( m_PPGTabVirtualSegment.dwFlagsUI & PROPF_TIME_CONVERSIONS_OK )
	{
		// "Nbr Measures"
		CString strNbrMeasures;
		strNbrMeasures.Format( "%ld", m_PPGTabVirtualSegment.dwNbrMeasures );
		m_btnLength.SetWindowText( strNbrMeasures );

		// "Start Offset"
		SetEditControl( m_editStartMeasure, m_PPGTabVirtualSegment.lStartMeasure, 1 );
		SetEditControl( m_editStartBeat, m_PPGTabVirtualSegment.lStartBeat, 1 );
		SetEditControl( m_editStartGrid, m_PPGTabVirtualSegment.lStartGrid, 1 );
		SetEditControl( m_editStartTick, m_PPGTabVirtualSegment.lStartTick, 0 );

		// "TimeSig Track Group"
		DWORD dwTimeSigGroupBits = m_PPGTabVirtualSegment.dwTimeSigGroupBits;
		m_lTrackGroupBits = 0;
		while( dwTimeSigGroupBits )
		{
			m_lTrackGroupBits++;
			dwTimeSigGroupBits = dwTimeSigGroupBits >> 1;
		}
		m_spinTrackGroup.SetPos( m_lTrackGroupBits );
	}

	// "Extend TimeSig" flag
	m_checkExtendTimeSig.SetCheck( (m_PPGTabVirtualSegment.dwResolution & DMUS_SEGF_TIMESIG_ALWAYS) ? 1 : 0 );

	// "AudioPath" flag
//  m_checkAudioPath not supported in DX8
//	m_checkAudioPath.SetCheck( ((m_PPGTabVirtualSegment.dwResolution & DMUS_SEGF_USE_AUDIOPATH) && (m_PPGTabVirtualSegment.dwFlagsUI & PROPF_HAVE_AUDIOPATH)) ? 1 : 0 );

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::EnableControls

void CTabVirtualSegment::EnableControls( BOOL fEnable ) 
{
	m_editName.EnableWindow( fEnable );
	m_checkExtendTimeSig.EnableWindow( fEnable );
	if( fEnable == FALSE )
	{
		m_editName.SetWindowText( NULL );
		m_checkExtendTimeSig.SetCheck( 0 );
	}

	BOOL fEnableTimeConversions = fEnable;
	if( !(m_PPGTabVirtualSegment.dwFlagsUI & PROPF_TIME_CONVERSIONS_OK) )
	{
		fEnableTimeConversions = FALSE;
	}
	m_btnLength.EnableWindow( fEnableTimeConversions );
	m_editStartMeasure.EnableWindow( fEnableTimeConversions );
	m_editStartBeat.EnableWindow( fEnableTimeConversions );
	m_editStartGrid.EnableWindow( fEnableTimeConversions );
	m_editStartTick.EnableWindow( fEnableTimeConversions );
	m_editTrackGroup.EnableWindow( fEnableTimeConversions );
	m_spinStartMeasure.EnableWindow( fEnableTimeConversions );
	m_spinStartBeat.EnableWindow( fEnableTimeConversions );
	m_spinStartGrid.EnableWindow( fEnableTimeConversions );
	m_spinStartTick.EnableWindow( fEnableTimeConversions );
	m_spinTrackGroup.EnableWindow( fEnableTimeConversions );
	if( fEnableTimeConversions == FALSE )
	{
		m_btnLength.SetWindowText( NULL );
		m_editStartMeasure.SetWindowText( NULL );
		m_editStartBeat.SetWindowText( NULL );
		m_editStartGrid.SetWindowText( NULL );
		m_editStartTick.SetWindowText( NULL );
		m_editTrackGroup.SetWindowText( NULL );
	}

	BOOL fEnableAudioPath = fEnable;
	if( !(m_PPGTabVirtualSegment.dwFlagsUI & PROPF_HAVE_AUDIOPATH) )
	{
		fEnableAudioPath = FALSE;
	}
//  m_checkAudioPath not supported in DX8 /////////
	fEnableAudioPath = FALSE;
///////////////////////////////////////////////////
	m_checkAudioPath.EnableWindow( fEnableAudioPath );
	if( fEnableAudioPath == FALSE )
	{
		m_checkAudioPath.SetCheck( 0 );
	}
}
	

/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::SetEditControl

void CTabVirtualSegment::SetEditControl( CEdit& edit, long lValue, int nOffset )
{
	TCHAR tcstrTmp[DIALOG_EDIT_LEN];
	BOOL fTransSuccess;

	edit.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

	// Convert from text to an integer
	long lCurValue = GetDlgItemInt( edit.GetDlgCtrlID(), &fTransSuccess, TRUE );

	// Check if the conversion failed, the text is empty, or if the values are different
	if( !fTransSuccess
	|| (tcstrTmp[0] == NULL)
	|| (lCurValue != lValue + nOffset) )
	{
		// Update the displayed number
		SetDlgItemInt( edit.GetDlgCtrlID(), lValue + nOffset );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::HandleKillFocus
//
// Generic handler for KillFocus changes
bool CTabVirtualSegment::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Check if the spin control is a valid window
	if (!::IsWindow(spin.m_hWnd))
	{
		return false;
	}

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_EDIT_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	if( pEditCtrl == NULL )
	{
		return false;
	}
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

	bool fChanged = false;

	// Handle empty text
	if( tcstrTmp[0] == NULL )
	{
		// Set it back to the minimum value
		spin.SetPos( nOffset );

		// Check if the value changed
		if( lUpdateVal != 0 )
		{
			// Update the item with the new value
			lUpdateVal = 0;
			fChanged = true;
		}
	}
	else
	{
		// Convert from text to an integer
		BOOL fTransSuccess;
		long lNewValue = GetDlgItemInt( pEditCtrl->GetDlgCtrlID(), &fTransSuccess, TRUE );

		// If unable to translate (if dialog contains invalid data, such as '-'), exit early
		if( !fTransSuccess )
		{
			// If conversion failed, update dialog item text
			SetDlgItemInt( pEditCtrl->GetDlgCtrlID(), lUpdateVal + nOffset );
		}
		// If conversion succeeded, ensure the value stays in range
		else
		{
			// Get the valid range
			int iLower, iUpper;
			spin.GetRange( iLower, iUpper );

			// Check if the value is out of range
			if( lNewValue > iUpper )
			{
				// Value out of range, update dialog item text
				SetDlgItemInt( pEditCtrl->GetDlgCtrlID(), iUpper );

				// Update the value to the top of the range
				lNewValue = iUpper;
			}

			// Check if the value changed
			if( lUpdateVal != (lNewValue - nOffset) )
			{
				// Update the item with the new value
				lUpdateVal = (lNewValue - nOffset);
				fChanged = true;
			}
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::HandleKillFocus
//
// Generic handler for KillFocus changes
bool CTabVirtualSegment::HandleKillFocus( CSpinButtonCtrl& spin, DWORD& dwUpdateVal, int nOffset )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Check if the spin control is a valid window
	if (!::IsWindow(spin.m_hWnd))
	{
		return false;
	}

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_EDIT_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	if( pEditCtrl == NULL )
	{
		return false;
	}
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

	bool fChanged = false;

	// Handle empty text
	if( tcstrTmp[0] == NULL )
	{
		// Set it back to the minimum value
		spin.SetPos( nOffset );

		// Check if the value changed
		if( dwUpdateVal != 0 )
		{
			// Update the item with the new value
			dwUpdateVal = 0;
			fChanged = true;
		}
	}
	else
	{
		// Convert from text to an integer
		BOOL fTransSuccess;
		long lNewValue = GetDlgItemInt( pEditCtrl->GetDlgCtrlID(), &fTransSuccess, TRUE );

		// If unable to translate (if dialog contains invalid data, such as '-'), exit early
		if( !fTransSuccess )
		{
			// If conversion failed, update dialog item text
			SetDlgItemInt( pEditCtrl->GetDlgCtrlID(), dwUpdateVal + nOffset );
		}
		// If conversion succeeded, ensure the value stays in range
		else
		{
			// Get the valid range
			int iLower, iUpper;
			spin.GetRange( iLower, iUpper );

			// Check if the value is out of range
			if( lNewValue > iUpper )
			{
				// Value out of range, update dialog item text
				SetDlgItemInt( pEditCtrl->GetDlgCtrlID(), iUpper );

				// Update the value to the top of the range
				lNewValue = iUpper;
			}

			// Check if the value changed
			if( dwUpdateVal != (DWORD)(lNewValue - nOffset) )
			{
				// Update the item with the new value
				dwUpdateVal = (DWORD)(lNewValue - nOffset);
				fChanged = true;
			}
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::HandleDeltaChange
//
// Generic handler for deltapos changes
bool CTabVirtualSegment::HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	bool fChanged = false;

	// If the value changed - update the selected item
	if( ((NM_UPDOWN* )pNMHDR)->iDelta != 0 ) 
	{
		// Update the value
		lUpdateVal += ((NM_UPDOWN* )pNMHDR)->iDelta;
		fChanged = true;
	}

	// Set the result to 1 to show that we handled this message
	*pResult = 1;

	return fChanged;
}


BEGIN_MESSAGE_MAP(CTabVirtualSegment, CPropertyPage)
	//{{AFX_MSG_MAP(CTabVirtualSegment)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnKillFocusEditName)
	ON_BN_CLICKED(IDC_CHECK_AUDIOPATH, OnCheckAudioPath)
	ON_BN_DOUBLECLICKED(IDC_CHECK_AUDIOPATH, OnDoubleClickedCheckAudioPath)
	ON_BN_CLICKED(IDC_CHECK_EXTEND_TIMESIG, OnCheckExtendTimeSig)
	ON_BN_DOUBLECLICKED(IDC_CHECK_EXTEND_TIMESIG, OnDoubleClickedCheckExtendTimeSig)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_BAR, OnSpinStartMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_BEAT, OnSpinStartBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_GRID, OnSpinStartGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAY_START_TICK, OnSpinStartTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRACK_GROUP, OnSpinTrackGroup)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_BAR, OnKillFocusStartMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_BEAT, OnKillFocusStartBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_GRID, OnKillFocusStartGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAY_START_TICK, OnKillFocusStartTick)
	ON_EN_KILLFOCUS(IDC_EDIT_TRACK_GROUP, OnKillFocusTrackGroup)
	ON_BN_CLICKED(IDC_BUTTON_LENGTH, OnButtonLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnSetActive

BOOL CTabVirtualSegment::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Set limits for edit controls
	m_editName.LimitText( DMUS_MAX_NAME );
	m_editStartMeasure.LimitText( 5 );
	m_editStartBeat.LimitText( 3 );
	m_editStartGrid.LimitText( 3 );
	m_editStartTick.LimitText( 5 );
	m_editTrackGroup.LimitText( 2 );

	// Set ranges of spin controls
	m_spinStartMeasure.SetRange( 1, 32767 );
	m_spinStartBeat.SetRange( 1, 256 );
	m_spinStartGrid.SetRange( 1, 256 );
	m_spinStartTick.SetRange( -MAX_TICK, MAX_TICK );
	m_spinTrackGroup.SetRange( 1, 32 );

	// Refresh the tab
	RefreshTab();
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnDestroy

void CTabVirtualSegment::OnDestroy() 
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
// CTabVirtualSegment::OnCreate

int CTabVirtualSegment::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabVirtualSegment::OnKillFocusEditName

void CTabVirtualSegment::OnKillFocusEditName( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	m_editName.GetWindowText( strName );

	// Strip leading and trailing spaces
	strName.TrimRight();
	strName.TrimLeft();

	if( strName.IsEmpty() )
	{
		m_editName.SetWindowText( m_PPGTabVirtualSegment.strName );
	}
	else
	{
		if( strName.Compare( m_PPGTabVirtualSegment.strName ) != 0 )
		{
			m_PPGTabVirtualSegment.strName = strName;
			UpdateObject();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnCheckAudioPath

void CTabVirtualSegment::OnCheckAudioPath() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_PPGTabVirtualSegment.dwFlagsUI & PROPF_HAVE_AUDIOPATH );

	DWORD dwNewFlag = 0;
	if( m_checkAudioPath.GetCheck() )
	{
		dwNewFlag = DMUS_SEGF_USE_AUDIOPATH;
	}

	if( dwNewFlag != (m_PPGTabVirtualSegment.dwResolution & DMUS_SEGF_USE_AUDIOPATH) )
	{
		m_PPGTabVirtualSegment.dwResolution ^= DMUS_SEGF_USE_AUDIOPATH;
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnDoubleClickedCheckAudioPath

void CTabVirtualSegment::OnDoubleClickedCheckAudioPath() 
{
	OnCheckAudioPath();
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnCheckExtendTimeSig

void CTabVirtualSegment::OnCheckExtendTimeSig() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	DWORD dwNewFlag = 0;
	if( m_checkExtendTimeSig.GetCheck() )
	{
		dwNewFlag = DMUS_SEGF_TIMESIG_ALWAYS;
	}

	if( dwNewFlag != (m_PPGTabVirtualSegment.dwResolution & DMUS_SEGF_TIMESIG_ALWAYS) )
	{
		m_PPGTabVirtualSegment.dwResolution ^= DMUS_SEGF_TIMESIG_ALWAYS;
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnDoubleClickedCheckExtendTimeSig

void CTabVirtualSegment::OnDoubleClickedCheckExtendTimeSig() 
{
	OnCheckExtendTimeSig();
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnSpinStartMeasure

void CTabVirtualSegment::OnSpinStartMeasure( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegment.lStartMeasure ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnSpinStartBeat

void CTabVirtualSegment::OnSpinStartBeat( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegment.lStartBeat ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnSpinStartGrid

void CTabVirtualSegment::OnSpinStartGrid( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartGrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegment.lStartGrid ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnSpinStartTick

void CTabVirtualSegment::OnSpinStartTick( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegment.lStartTick ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnSpinTrackGroup

void CTabVirtualSegment::OnSpinTrackGroup( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusTrackGroup();

	if( HandleDeltaChange( pNMHDR, pResult, m_lTrackGroupBits ) )
	{
		m_lTrackGroupBits = min( m_lTrackGroupBits, 32 );
		m_lTrackGroupBits = max( m_lTrackGroupBits, 1 );
		m_lTrackGroupBits--;

		DWORD dwTimeSigGroupBits = (1 << m_lTrackGroupBits);

		if( dwTimeSigGroupBits != m_PPGTabVirtualSegment.dwTimeSigGroupBits )
		{
			m_PPGTabVirtualSegment.dwTimeSigGroupBits = dwTimeSigGroupBits;
			UpdateObject();
		}
		else
		{
			RefreshTab();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnKillFocusStartMeasure

void CTabVirtualSegment::OnKillFocusStartMeasure() 
{
	if( HandleKillFocus( m_spinStartMeasure, m_PPGTabVirtualSegment.lStartMeasure, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnKillFocusStartBeat

void CTabVirtualSegment::OnKillFocusStartBeat() 
{
	if( HandleKillFocus( m_spinStartBeat, m_PPGTabVirtualSegment.lStartBeat, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnKillFocusStartGrid

void CTabVirtualSegment::OnKillFocusStartGrid() 
{
	if( HandleKillFocus( m_spinStartGrid, m_PPGTabVirtualSegment.lStartGrid, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnKillFocusStartTick

void CTabVirtualSegment::OnKillFocusStartTick() 
{
	if( HandleKillFocus( m_spinStartTick, m_PPGTabVirtualSegment.lStartTick, 0 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnKillFocusTrackGroup

void CTabVirtualSegment::OnKillFocusTrackGroup() 
{
	if( HandleKillFocus( m_spinTrackGroup, m_lTrackGroupBits, 0 ) )
	{
		m_lTrackGroupBits = min( m_lTrackGroupBits, 32 );
		m_lTrackGroupBits = max( m_lTrackGroupBits, 1 );
		m_lTrackGroupBits--;

		DWORD dwTimeSigGroupBits = (1 << m_lTrackGroupBits);

		if( dwTimeSigGroupBits != m_PPGTabVirtualSegment.dwTimeSigGroupBits )
		{
			m_PPGTabVirtualSegment.dwTimeSigGroupBits = dwTimeSigGroupBits;
			UpdateObject();
		}
		else
		{
			RefreshTab();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegment::OnButtonLength

void CTabVirtualSegment::OnButtonLength() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDlgSetLength dlgSetLength;
	dlgSetLength.m_dwNbrMeasures = m_PPGTabVirtualSegment.dwNbrMeasures;

	if( dlgSetLength.DoModal() == IDOK )
	{
		if( dlgSetLength.m_dwNbrMeasures != m_PPGTabVirtualSegment.dwNbrMeasures )
		{
			m_PPGTabVirtualSegment.dwNbrMeasures = dlgSetLength.m_dwNbrMeasures;
			UpdateObject();
		}
	}

	m_btnLength.SetFocus();
}
