// TabVirtualSegmentLoop.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"
#include "resource.h"

#include "Song.h"
#include "TabVirtualSegmentLoop.h"

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
// CTabVirtualSegmentLoop property page

CTabVirtualSegmentLoop::CTabVirtualSegmentLoop( IDMUSProdPropPageManager* pIPageManager ) : CPropertyPage(CTabVirtualSegmentLoop::IDD)
{
	//{{AFX_DATA_INIT(CTabVirtualSegmentLoop)
	//}}AFX_DATA_INIT

	ASSERT( pIPageManager != NULL );

	m_pIPropPageObject = NULL;
	m_pIPageManager = pIPageManager;
	m_fNeedToDetach = FALSE;
}

CTabVirtualSegmentLoop::~CTabVirtualSegmentLoop()
{
	RELEASE( m_pIPropPageObject );
}


void CTabVirtualSegmentLoop::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabVirtualSegmentLoop)
	DDX_Control(pDX, IDC_EDIT_LOOP_REPEAT, m_editLoopRepeat);
	DDX_Control(pDX, IDC_SPIN_LOOP_REPEAT, m_spinLoopRepeat);
	DDX_Control(pDX, IDC_CHECK_REPEAT_FOREVER, m_btnLoopInfinite);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_BAR, m_editStartMeasure);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_BEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_GRID, m_editStartGrid);
	DDX_Control(pDX, IDC_EDIT_LOOP_START_TICK, m_editStartTick);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_BAR, m_editEndMeasure);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_BEAT, m_editEndBeat);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_GRID, m_editEndGrid);
	DDX_Control(pDX, IDC_EDIT_LOOP_END_TICK, m_editEndTick);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_BAR, m_spinStartMeasure);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_BEAT, m_spinStartBeat);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_GRID, m_spinStartGrid);
	DDX_Control(pDX, IDC_SPIN_LOOP_START_TICK, m_spinStartTick);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_BAR, m_spinEndMeasure);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_BEAT, m_spinEndBeat);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_GRID, m_spinEndGrid);
	DDX_Control(pDX, IDC_SPIN_LOOP_END_TICK, m_spinEndTick);
	DDX_Control(pDX, IDC_RESET_LOOP, m_btnResetLoop);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::SetObject

void CTabVirtualSegmentLoop::SetObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CTabVirtualSegmentLoop::UpdateObject

void CTabVirtualSegmentLoop::UpdateObject()
{
	if( m_pIPropPageObject )
	{
		m_pIPropPageObject->SetData( (void *)&m_PPGTabVirtualSegmentLoop );
	}

	RefreshTab();
}
	

/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::SetEditControl

void CTabVirtualSegmentLoop::SetEditControl( CEdit& edit, long lValue, int nOffset )
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
// CTabVirtualSegmentLoop::HandleKillFocus
//
// Generic handler for KillFocus changes
bool CTabVirtualSegmentLoop::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset )
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
// CTabVirtualSegmentLoop::HandleKillFocus
//
// Generic handler for KillFocus changes
bool CTabVirtualSegmentLoop::HandleKillFocus( CSpinButtonCtrl& spin, DWORD& dwUpdateVal, int nOffset )
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
// CTabVirtualSegmentLoop::HandleDeltaChange
//
// Generic handler for deltapos changes
bool CTabVirtualSegmentLoop::HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal )
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


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::HandleDeltaChange
//
// Generic handler for deltapos changes
bool CTabVirtualSegmentLoop::HandleDeltaChange( CSpinButtonCtrl& spin, NMHDR* pNMHDR, LRESULT* pResult, DWORD& dwUpdateVal )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	bool fChanged = false;

	// If the value changed - update the selected item
	if( ((NM_UPDOWN* )pNMHDR)->iDelta != 0 ) 
	{
		// Get current position
		int iPos = spin.GetPos();

		// Get the valid range
		int iLower, iUpper;
		spin.GetRange( iLower, iUpper );
	
		if( (iPos == iLower)  &&  (((NM_UPDOWN* )pNMHDR)->iDelta < 0) 
		||  (iPos == iUpper)  &&  (((NM_UPDOWN* )pNMHDR)->iDelta > 0) )
		{
			// Nothing to do
			fChanged = false;
		}
		else
		{
			// Update the value
			dwUpdateVal += ((NM_UPDOWN* )pNMHDR)->iDelta;
			fChanged = true;
		}
	}

	// Set the result to 1 to show that we handled this message
	*pResult = 1;

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::RefreshTab

void CTabVirtualSegmentLoop::RefreshTab( void )
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

	PPGTabVirtualSegmentLoop *pPPGTabVirtualSegmentLoop = &m_PPGTabVirtualSegmentLoop;
	if( FAILED( m_pIPropPageObject->GetData( (void**)&pPPGTabVirtualSegmentLoop ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	if( !(m_PPGTabVirtualSegmentLoop.dwFlagsUI & PROPF_ONE_SELECTED) 
	||  !(m_PPGTabVirtualSegmentLoop.dwFlagsUI & PROPF_TIME_CONVERSIONS_OK) )
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

	// "Infinite"
	m_btnLoopInfinite.SetCheck( (m_PPGTabVirtualSegmentLoop.dwRepeats == DMUS_SEG_REPEAT_INFINITE) ? 1 : 0 );

	// "Repeats"
	if( m_PPGTabVirtualSegmentLoop.dwRepeats != DMUS_SEG_REPEAT_INFINITE )
	{
		SetEditControl( m_editLoopRepeat, m_PPGTabVirtualSegmentLoop.dwRepeats, 0 );
	}

	// "Start Offset"
	SetEditControl( m_editStartMeasure, m_PPGTabVirtualSegmentLoop.lStartMeasure, 1 );
	SetEditControl( m_editStartBeat, m_PPGTabVirtualSegmentLoop.lStartBeat, 1 );
	SetEditControl( m_editStartGrid, m_PPGTabVirtualSegmentLoop.lStartGrid, 1 );
	SetEditControl( m_editStartTick, m_PPGTabVirtualSegmentLoop.lStartTick, 0 );

	// "End Offset"
	SetEditControl( m_editEndMeasure, m_PPGTabVirtualSegmentLoop.lEndMeasure, 1 );
	SetEditControl( m_editEndBeat, m_PPGTabVirtualSegmentLoop.lEndBeat, 1 );
	SetEditControl( m_editEndGrid, m_PPGTabVirtualSegmentLoop.lEndGrid, 1 );
	SetEditControl( m_editEndTick, m_PPGTabVirtualSegmentLoop.lEndTick, 0 );

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::EnableControls

void CTabVirtualSegmentLoop::EnableControls( BOOL fEnable ) 
{
	m_btnLoopInfinite.EnableWindow( fEnable );

	BOOL fEnableRepeat = fEnable;
	if( m_PPGTabVirtualSegmentLoop.dwRepeats == DMUS_SEG_REPEAT_INFINITE )
	{
		fEnableRepeat = FALSE;
	}

	m_editLoopRepeat.EnableWindow( fEnableRepeat );
	m_spinLoopRepeat.EnableWindow( fEnableRepeat );

	BOOL fEnableLoop = fEnable;
	if( m_PPGTabVirtualSegmentLoop.dwRepeats == 0 )
	{
		fEnableLoop = FALSE;
	}

	m_editStartMeasure.EnableWindow( fEnableLoop );
	m_editStartBeat.EnableWindow( fEnableLoop );
	m_editStartGrid.EnableWindow( fEnableLoop );
	m_editStartTick.EnableWindow( fEnableLoop );
	m_editEndMeasure.EnableWindow( fEnableLoop );
	m_editEndBeat.EnableWindow( fEnableLoop );
	m_editEndGrid.EnableWindow( fEnableLoop );
	m_editEndTick.EnableWindow( fEnableLoop );

	m_spinStartMeasure.EnableWindow( fEnableLoop );
	m_spinStartBeat.EnableWindow( fEnableLoop );
	m_spinStartGrid.EnableWindow( fEnableLoop );
	m_spinStartTick.EnableWindow( fEnableLoop );
	m_spinEndMeasure.EnableWindow( fEnableLoop );
	m_spinEndBeat.EnableWindow( fEnableLoop );
	m_spinEndGrid.EnableWindow( fEnableLoop );
	m_spinEndTick.EnableWindow( fEnableLoop );

	m_btnResetLoop.EnableWindow( fEnableLoop );

	if( fEnableRepeat == FALSE )
	{
		m_editLoopRepeat.SetWindowText( NULL );
	}

	if( fEnableLoop == FALSE )
	{
		m_editStartMeasure.SetWindowText( NULL );
		m_editStartBeat.SetWindowText( NULL );
		m_editStartGrid.SetWindowText( NULL );
		m_editStartTick.SetWindowText( NULL );
		m_editEndMeasure.SetWindowText( NULL );
		m_editEndBeat.SetWindowText( NULL );
		m_editEndGrid.SetWindowText( NULL );
		m_editEndTick.SetWindowText( NULL );
	}
}


BEGIN_MESSAGE_MAP(CTabVirtualSegmentLoop, CPropertyPage)
	//{{AFX_MSG_MAP(CTabVirtualSegmentLoop)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_BAR, OnSpinStartMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_BEAT, OnSpinStartBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_GRID, OnSpinStartGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_START_TICK, OnSpinStartTick)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_BAR, OnKillFocusStartMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_BEAT, OnKillFocusStartBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_GRID, OnKillFocusStartGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_START_TICK, OnKillFocusStartTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_BAR, OnSpinEndMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_BEAT, OnSpinEndBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_GRID, OnSpinEndGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_END_TICK, OnSpinEndTick)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_BAR, OnKillFocusEndMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_BEAT, OnKillFocusEndBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_GRID, OnKillFocusEndGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_END_TICK, OnKillFocusEndTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LOOP_REPEAT, OnSpinRepeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LOOP_REPEAT, OnKillFocusRepeat)
	ON_BN_CLICKED(IDC_CHECK_REPEAT_FOREVER, OnCheckRepeat)
	ON_BN_DOUBLECLICKED(IDC_CHECK_REPEAT_FOREVER, OnDoubleClickedCheckRepeat)
	ON_BN_CLICKED(IDC_RESET_LOOP, OnResetLoop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnSetActive

BOOL CTabVirtualSegmentLoop::OnSetActive() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Set limits for edit controls
	m_editLoopRepeat.LimitText( 5 );
	m_editStartMeasure.LimitText( 5 );
	m_editStartBeat.LimitText( 3 );
	m_editStartGrid.LimitText( 3 );
	m_editStartTick.LimitText( 5 );
	m_editEndMeasure.LimitText( 5 );
	m_editEndBeat.LimitText( 3 );
	m_editEndGrid.LimitText( 3 );
	m_editEndTick.LimitText( 5 );

	// Set ranges of spin controls
	m_spinLoopRepeat.SetRange( 0, 32767 );
	m_spinStartMeasure.SetRange( 1, 32767 );
	m_spinStartBeat.SetRange( 1, 256 );
	m_spinStartGrid.SetRange( 1, 256 );
	m_spinStartTick.SetRange( -MAX_TICK, MAX_TICK );
	m_spinEndMeasure.SetRange( 1, 32767 );
	m_spinEndBeat.SetRange( 1, 256 );
	m_spinEndGrid.SetRange( 1, 256 );
	m_spinEndTick.SetRange( -MAX_TICK, MAX_TICK );

	// Refresh the tab
	RefreshTab();
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnDestroy

void CTabVirtualSegmentLoop::OnDestroy() 
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
// CTabVirtualSegmentLoop::OnCreate

int CTabVirtualSegmentLoop::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabVirtualSegmentLoop::OnSpinStartMeasure

void CTabVirtualSegmentLoop::OnSpinStartMeasure( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegmentLoop.lStartMeasure ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnSpinStartBeat

void CTabVirtualSegmentLoop::OnSpinStartBeat( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegmentLoop.lStartBeat ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnSpinStartGrid

void CTabVirtualSegmentLoop::OnSpinStartGrid( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartGrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegmentLoop.lStartGrid ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnSpinStartTick

void CTabVirtualSegmentLoop::OnSpinStartTick( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegmentLoop.lStartTick ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnSpinEndMeasure

void CTabVirtualSegmentLoop::OnSpinEndMeasure( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegmentLoop.lEndMeasure ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnSpinEndBeat

void CTabVirtualSegmentLoop::OnSpinEndBeat( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegmentLoop.lEndBeat ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnSpinEndGrid

void CTabVirtualSegmentLoop::OnSpinEndGrid( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndGrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegmentLoop.lEndGrid ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnSpinEndTick

void CTabVirtualSegmentLoop::OnSpinEndTick( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_PPGTabVirtualSegmentLoop.lEndTick ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnKillFocusStartMeasure

void CTabVirtualSegmentLoop::OnKillFocusStartMeasure() 
{
	if( HandleKillFocus( m_spinStartMeasure, m_PPGTabVirtualSegmentLoop.lStartMeasure, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnKillFocusStartBeat

void CTabVirtualSegmentLoop::OnKillFocusStartBeat() 
{
	if( HandleKillFocus( m_spinStartBeat, m_PPGTabVirtualSegmentLoop.lStartBeat, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnKillFocusStartGrid

void CTabVirtualSegmentLoop::OnKillFocusStartGrid() 
{
	if( HandleKillFocus( m_spinStartGrid, m_PPGTabVirtualSegmentLoop.lStartGrid, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnKillFocusStartTick

void CTabVirtualSegmentLoop::OnKillFocusStartTick() 
{
	if( HandleKillFocus( m_spinStartTick, m_PPGTabVirtualSegmentLoop.lStartTick, 0 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnKillFocusEndMeasure

void CTabVirtualSegmentLoop::OnKillFocusEndMeasure() 
{
	if( HandleKillFocus( m_spinEndMeasure, m_PPGTabVirtualSegmentLoop.lEndMeasure, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnKillFocusEndBeat

void CTabVirtualSegmentLoop::OnKillFocusEndBeat() 
{
	if( HandleKillFocus( m_spinEndBeat, m_PPGTabVirtualSegmentLoop.lEndBeat, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnKillFocusEndGrid

void CTabVirtualSegmentLoop::OnKillFocusEndGrid() 
{
	if( HandleKillFocus( m_spinEndGrid, m_PPGTabVirtualSegmentLoop.lEndGrid, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnKillFocusEndTick

void CTabVirtualSegmentLoop::OnKillFocusEndTick() 
{
	if( HandleKillFocus( m_spinEndTick, m_PPGTabVirtualSegmentLoop.lEndTick, 0 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnSpinRepeat

void CTabVirtualSegmentLoop::OnSpinRepeat( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusRepeat();

	if( HandleDeltaChange( m_spinLoopRepeat, pNMHDR, pResult, m_PPGTabVirtualSegmentLoop.dwRepeats) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnKillFocusRepeat

void CTabVirtualSegmentLoop::OnKillFocusRepeat() 
{
	if( HandleKillFocus( m_spinLoopRepeat, m_PPGTabVirtualSegmentLoop.dwRepeats, 0 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnCheckRepeat

void CTabVirtualSegmentLoop::OnCheckRepeat() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	DWORD dwRepeats = m_btnLoopInfinite.GetCheck() ? DMUS_SEG_REPEAT_INFINITE : 0;

	if( dwRepeats != m_PPGTabVirtualSegmentLoop.dwRepeats )
	{
		m_PPGTabVirtualSegmentLoop.dwRepeats = dwRepeats;
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnDoubleClickedCheckRepeat

void CTabVirtualSegmentLoop::OnDoubleClickedCheckRepeat() 
{
	OnCheckRepeat();
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::OnResetLoop

void CTabVirtualSegmentLoop::OnResetLoop() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_PPGTabVirtualSegmentLoop.lStartMeasure != 0
	||  m_PPGTabVirtualSegmentLoop.lStartBeat != 0
	||  m_PPGTabVirtualSegmentLoop.lStartGrid != 0
	||  m_PPGTabVirtualSegmentLoop.lStartTick != 0
	||  m_PPGTabVirtualSegmentLoop.lEndMeasure != 0
	||  m_PPGTabVirtualSegmentLoop.lEndBeat != 0
	||  m_PPGTabVirtualSegmentLoop.lEndGrid != 0
	||  m_PPGTabVirtualSegmentLoop.lEndTick != 0 )
	{
		m_PPGTabVirtualSegmentLoop.lStartMeasure = 0;
		m_PPGTabVirtualSegmentLoop.lStartBeat = 0;
		m_PPGTabVirtualSegmentLoop.lStartGrid = 0;
		m_PPGTabVirtualSegmentLoop.lStartTick = 0;
		m_PPGTabVirtualSegmentLoop.lEndMeasure = 0;
		m_PPGTabVirtualSegmentLoop.lEndBeat = 0;
		m_PPGTabVirtualSegmentLoop.lEndGrid = 0;
		m_PPGTabVirtualSegmentLoop.lEndTick = 0;
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVirtualSegmentLoop::PreTranslateMessage

BOOL CTabVirtualSegmentLoop::PreTranslateMessage( MSG* pMsg ) 
{
/*
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !(m_PPGTabVirtualSegmentLoop.dwFlagsUI & PROPF_ONE_SELECTED) )
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
							case IDC_EDIT_LOOP_REPEAT: 
								m_spinRepeat.SetPos( m_pPPGSegment->dwLoopRepeats );
								break;

							case IDC_EDIT_LOOP_END_BAR:
								m_spinEndMeasure.SetPos( m_pPPGSegment->dwLoopEndMeasure + 1 );
								break;

							case IDC_EDIT_LOOP_END_BEAT:
								m_spinEndBeat.SetPos( m_pPPGSegment->bLoopEndBeat + 1 );
								break;

							case IDC_EDIT_LOOP_END_GRID:
								m_spinEndGrid.SetPos( m_pPPGSegment->wLoopEndGrid + 1 );
								break;

							case IDC_EDIT_LOOP_END_TICK:
								m_spinEndTick.SetPos( m_pPPGSegment->wLoopEndTick );
								break;

							case IDC_EDIT_LOOP_START_BAR:
								m_spinStartMeasure.SetPos( m_pPPGSegment->dwLoopStartMeasure + 1 );
								break;

							case IDC_EDIT_LOOP_START_BEAT:
								m_spinStartBeat.SetPos( m_pPPGSegment->bLoopStartBeat + 1 );
								break;

							case IDC_EDIT_LOOP_START_GRID:
								m_spinStartGrid.SetPos( m_pPPGSegment->wLoopStartGrid + 1 );
								break;

							case IDC_EDIT_LOOP_START_TICK:
								m_spinStartTick.SetPos( m_pPPGSegment->wLoopStartTick );
								break;
						}
					}
					return TRUE;
				}

				case VK_RETURN:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						if( pWnd->GetDlgCtrlID() == IDC_EDIT_LOOP_REPEAT )
						{
							OnKillfocusEditLoopRepeat();
						}

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
*/	
	return CPropertyPage::PreTranslateMessage( pMsg );
}
