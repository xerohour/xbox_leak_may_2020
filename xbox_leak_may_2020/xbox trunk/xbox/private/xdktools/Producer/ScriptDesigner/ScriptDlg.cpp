// ScriptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptDesignerDLL.h"
#include "Script.h"
#include "ScriptCtl.h"
#include "ScriptDlg.h"
#include <ContainerDesigner.h>
#include <windowsx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptDlg

//IMPLEMENT_DYNCREATE(CScriptDlg, CFormView)

CScriptDlg::CScriptDlg()
	: CFormView(CScriptDlg::IDD)
{
	//{{AFX_DATA_INIT(CScriptDlg)
	//}}AFX_DATA_INIT

	m_pScriptCtrl = NULL;
	m_pScript = NULL;

	m_nLastStartChar = 0;
	m_nLastEndChar = 0;
	m_nLastPos = 0;

	VariantInit( &m_EditVariant );
	m_pEditValueCtrl = NULL;
}

CScriptDlg::~CScriptDlg()
{
	VariantClear( &m_EditVariant );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::SyncSource

void CScriptDlg::SyncSource( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsWindow(m_editSource) )
	{
		m_editSource.OnKillFocus();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::RefreshControls

void CScriptDlg::RefreshControls( DWORD dwFlags )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	if( dwFlags & SSE_SOURCE )
	{
		// Handle source
		m_editSource.SetWindowText( m_pScript->m_strSource );

		// Handle status bar
		m_nLastStartChar = 0;
		m_nLastEndChar = 0;
		m_nLastPos = 0;
		UpdateStatusBar();
	}

	if( dwFlags & SSE_ROUTINES )
	{
		// Handle routine list box
		FillRoutineListBox();
	}

	if( dwFlags & SSE_VARIABLES )
	{
		// Handle variables list box
		FillVariableListBox();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::FillRoutineListBox

void CScriptDlg::FillRoutineListBox( void )
{
	ASSERT( m_pScript != NULL );

	m_lstbxRoutines.SetRedraw( FALSE );

	// Store selected routines
	CTypedPtrList<CPtrList, CString*> listSelectedRoutines;
	int nNbrSelItems = m_lstbxRoutines.GetSelCount();
	if( nNbrSelItems > 0 )
	{
		int* pnSelItems = new int[nNbrSelItems];
		if( pnSelItems )
		{
			m_lstbxRoutines.GetSelItems( nNbrSelItems, pnSelItems );

			for( int i = 0;  i < nNbrSelItems ;  i++ )
			{
				CString* pstrRoutine = new CString;
				if( pstrRoutine )
				{
					m_lstbxRoutines.GetText( pnSelItems[i], *pstrRoutine );
					listSelectedRoutines.AddTail( pstrRoutine ); 
				}
			}

			delete [] pnSelItems;
		}
	}

	// Initialize content
	m_lstbxRoutines.ResetContent();
	m_lstbxRoutines.SetHorizontalExtent( 0 );
	
	// Fill the list box
	CDC* pDC = m_lstbxRoutines.GetDC();
	if( pDC )
	{
		CSize sizeText;
		int nCurExtent = m_lstbxRoutines.GetHorizontalExtent();

		CString strRoutineName;
		WCHAR awchRoutineName[MAX_PATH];
		int i = 0;
		if( m_pScript->m_pIDMScript )
		{
			while( m_pScript->m_pIDMScript->EnumRoutine( i++, awchRoutineName ) == S_OK )
			{
				strRoutineName = awchRoutineName;
				m_lstbxRoutines.AddString( strRoutineName );

				sizeText = pDC->GetTextExtent( strRoutineName );
				if( sizeText.cx > nCurExtent )
				{
					nCurExtent = sizeText.cx;
					m_lstbxRoutines.SetHorizontalExtent( nCurExtent );
				}
			}
		}

		m_lstbxRoutines.ReleaseDC( pDC );
	}

	// Process selected routine list
	while( !listSelectedRoutines.IsEmpty() )
	{
		CString* pstrText = static_cast<CString*>( listSelectedRoutines.RemoveHead() );

		// Re-establish selection state
		int nPos = m_lstbxRoutines.FindStringExact( -1, *pstrText );
		if( nPos != LB_ERR )
		{
			m_lstbxRoutines.SetSel( nPos, TRUE ); 
			m_lstbxRoutines.SetCaretIndex( nPos, 0 ); 
		}

		delete pstrText;
	}

	m_lstbxRoutines.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::FillVariableListBox

void CScriptDlg::FillVariableListBox( void )
{
	ASSERT( m_pScript != NULL );

	m_lstbxVariables.SetRedraw( FALSE );

	// Store selected variable
	CString strSelectedVariable;
	int nCurSelection = m_lstbxVariables.GetNextItem( -1, LVNI_SELECTED );
	if( nCurSelection >= 0 )
	{
		strSelectedVariable = m_lstbxVariables.GetItemText( nCurSelection, 0 );
	}

	// Initialize content
	m_lstbxVariables.DeleteAllItems();
	
	// Fill the list box
	CDC* pDC = m_lstbxVariables.GetDC();
	if( pDC )
	{
		CString strVariableName;
		WCHAR awchVariableName[MAX_PATH];

		int i = 0;
		BSTR bstrVariableName;
		bool fBypassVariable;
		
		if( m_pScript->m_pIDMScript )
		{
			while( m_pScript->m_pIDMScript->EnumVariable( i++, awchVariableName ) == S_OK )
			{
				// Bypass variables that are actually items in the Script's Container
				fBypassVariable = false;
				if( m_pScript->m_pIContainerNode )
				{
					IDMUSProdContainerInfo* pIContainerInfo;
					if( SUCCEEDED ( m_pScript->m_pIContainerNode->QueryInterface( IID_IDMUSProdContainerInfo, (void**)&pIContainerInfo ) ) )
					{
						strVariableName = awchVariableName;
						bstrVariableName = strVariableName.AllocSysString();

						IUnknown* pIDocRootNode;
						if( pIContainerInfo->FindDocRootFromScriptAlias(bstrVariableName, &pIDocRootNode) == S_OK )
						{
							fBypassVariable = true;
							RELEASE( pIDocRootNode );
						}

						RELEASE( pIContainerInfo );
					}
				}
				if( fBypassVariable )
				{
					continue;
				}

				// COLUMN 1 (Name)
				strVariableName = awchVariableName;
				int nPos = m_lstbxVariables.InsertItem( i, strVariableName );

				// COLUMN 2 (Value)
				CString strValue;
				FormatValueText( awchVariableName, strValue );
				m_lstbxVariables.SetItemText( nPos, 1, strValue );
			}
		}

		m_lstbxVariables.ReleaseDC( pDC );
	}

	// Reselect variable
	if( strSelectedVariable.IsEmpty() == FALSE )
	{
		LV_FINDINFO fi;
		fi.flags = LVFI_STRING;
		fi.psz = strSelectedVariable;
		fi.lParam = NULL;
		nCurSelection = m_lstbxVariables.FindItem( &fi );
		if( nCurSelection >= 0 )
		{
			m_lstbxVariables.SetItemState( nCurSelection, (LVIS_SELECTED|LVIS_FOCUSED), (LVIS_SELECTED|LVIS_FOCUSED) );
		}
	}

	m_lstbxVariables.SetRedraw( TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::FormatValueText

void CScriptDlg::FormatValueText( WCHAR* awchVariableName, CString& strValue )
{
	if( m_pScript == NULL
	||  m_pScript->m_pIDMScript == NULL )
	{
		// Can't do anything
		return;
	}

	// Initialize ErrorInfo struct
	DMUS_SCRIPT_ERRORINFO ErrorInfo;
	memset( &ErrorInfo, 0, sizeof(DMUS_SCRIPT_ERRORINFO) );
	ErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);

	VARIANT var;
	VariantInit( &var );

	// Get current value
	HRESULT hr = m_pScript->m_pIDMScript->GetVariableVariant( awchVariableName, &var, &ErrorInfo );
	if( FAILED ( hr ) )
	{
		// Value = (Error)
		strValue.LoadString( IDS_ERROR_TEXT );
		m_pScript->DisplayScriptError( &ErrorInfo, hr );
		return;
	}

	switch( var.vt )
	{
		case VT_I2:			// Number
			strValue.Format( "%d", V_I2(&var) );
			break;

		case VT_I4:			// Number
			strValue.Format( "%d", V_I4(&var) );
			break;

		case VT_BOOL:		// BOOL
			strValue.Format( "%d", V_BOOL(&var) );
			break;

		case VT_BSTR:		// String (Unicode)
			strValue = V_BSTR( &var );
			break;

		case VT_EMPTY:		// <Empty>
			strValue.LoadString( IDS_EMPTY_TEXT );
			break;

		case VT_UNKNOWN:	// (Object)
		case VT_DISPATCH:	
			strValue.LoadString( IDS_OBJECT_TEXT );
			break;

		default:			// <Unknown>
			strValue.LoadString( IDS_UNKNOWN_TEXT );
			break;
	}

	VariantClear( &var );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::UpdateStatusBar

void CScriptDlg::UpdateStatusBar( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	if( ::IsWindow(m_editSource) )
	{
		CString strLine;
		strLine.LoadString( IDS_LINE_TEXT );
		
		CString strColumn;
		strColumn.LoadString( IDS_COLUMN_TEXT );

		int nStartChar, nEndChar;
		m_editSource.GetSel( nStartChar, nEndChar );

		int nPos;
		if( nEndChar != m_nLastEndChar  )
		{
			nPos = nEndChar;
		}
		else if( nStartChar != m_nLastStartChar  )
		{
			nPos = nStartChar;
		}
		else
		{
			nPos = m_nLastPos;
		}
		m_nLastStartChar = nStartChar;
		m_nLastEndChar = nEndChar;
		m_nLastPos = nPos;

		int nLine, nColumn;
		nLine = m_editSource.LineFromChar( nPos );
		nColumn = nPos - m_editSource.LineIndex( nLine );

		CString strText;
		strText.Format( "%s %d, %s %d",
						strLine, (nLine + 1), 
						strColumn, (nColumn + 1) ); 
		BSTR bstrText = strText.AllocSysString();

		theApp.m_pScriptComponent->m_pIFramework->SetStatusBarPaneText( m_pScriptCtrl->m_hKeyStatusBar, 0, bstrText, TRUE );
	}
}


void CScriptDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptDlg)
	DDX_Control(pDX, IDC_INIT_VARIABLES, m_btnInitialize);
	DDX_Control(pDX, IDC_VARIABLE_LIST, m_lstbxVariables);
	DDX_Control(pDX, IDC_SOURCE_PROMPT, m_staticSourcePrompt);
	DDX_Control(pDX, IDC_REFRESH, m_btnRefresh);
	DDX_Control(pDX, IDC_VARIABLE_PROMPT, m_staticVariablePrompt);
	DDX_Control(pDX, IDC_ROUTINE_PROMPT, m_staticRoutinePrompt);
	DDX_Control(pDX, IDC_ROUTINE_LIST, m_lstbxRoutines);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScriptDlg, CFormView)
	//{{AFX_MSG_MAP(CScriptDlg)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SIZE()
	ON_LBN_DBLCLK(IDC_ROUTINE_LIST, OnDblClkRoutineList)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_NOTIFY(NM_DBLCLK, IDC_VARIABLE_LIST, OnDblClkVariableList)
	ON_NOTIFY(NM_CLICK, IDC_VARIABLE_LIST, OnClickVariableList)
	ON_BN_CLICKED(IDC_INIT_VARIABLES, OnInitVariables)
	ON_NOTIFY(LVN_KEYDOWN, IDC_VARIABLE_LIST, OnKeyDownVariableList)
	ON_NOTIFY(NM_RETURN, IDC_VARIABLE_LIST, OnReturnVariableList)
	ON_WM_PARENTNOTIFY()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_NOTIFY(NM_SETFOCUS, IDC_VARIABLE_LIST, OnSetFocusVariableList)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_END_EDIT_VALUE, OnEndEditValue)
	ON_MESSAGE(WM_CANCEL_EDIT_VALUE, OnCancelEditValue)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptDlg diagnostics

#ifdef _DEBUG
void CScriptDlg::AssertValid() const
{
	CFormView::AssertValid();
}

void CScriptDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScriptDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::Create

BOOL CScriptDlg::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !CFormView::Create( lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext ) )
	{
		return FALSE;
	}
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnInitialUpdate

void CScriptDlg::OnInitialUpdate() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	CFormView::OnInitialUpdate();

	// Subclass controls
	m_editSource.SubclassDlgItem( IDC_SOURCE, this );
	m_editSource.m_pScriptDlg = this;

	// Get client rect of variable list box
	CRect rect;
	m_lstbxVariables.GetClientRect( &rect );

	// Add columns to variable list box
	CString strText;
	strText.LoadString( IDS_NAME_TEXT );
	m_lstbxVariables.InsertColumn( 0, strText, LVCFMT_LEFT, (rect.right >> 1), 1 );
	strText.LoadString( IDS_VALUE_TEXT );
	m_lstbxVariables.InsertColumn( 1, strText, LVCFMT_LEFT, (rect.right >> 1), 0 );

	RefreshControls( SSE_ALL );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnDestroy

void CScriptDlg::OnDestroy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

// AMC - Will want to change way data syncs are handled!
	ASSERT( m_pScript != NULL );
	m_editSource.GetWindowText( m_pScript->m_strSource );

	CFormView::OnDestroy();
}

#define CONTROL_BORDER	5

/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnSize

void CScriptDlg::OnSize( UINT nType, int cx, int cy ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CFormView::OnSize( nType, cx, cy );

    if( nType == SIZE_MINIMIZED )
	{
        return;
	}

	// Exit if we are not fully created yet
	if( !::IsWindow(m_editSource) )
	{
		return;
	}

	ASSERT( m_pScript != NULL );

	// Set scroll positions to zero
	SetScrollPos( SB_HORZ, 0, FALSE );
	SetScrollPos( SB_VERT, 0, FALSE );

	CRect rect;
	CRect rectTemp;

	// Get size of dialog
	CRect rectDlg;
	GetClientRect( &rectDlg );

	// Enforce minimum size of dialog
	HRSRC hRes = ::FindResource( theApp.m_hInstance, MAKEINTRESOURCE(IDD_DLG_SCRIPT), RT_DIALOG );
	if( hRes )
	{
		HGLOBAL hTemplate = ::LoadResource( theApp.m_hInstance, hRes );
		if( hTemplate )
		{
			DLGTEMPLATE* pTemplate = (DLGTEMPLATE *)::LockResource( hTemplate );
			if( pTemplate )
			{
				CRect rect( 0, 0, pTemplate->cx, pTemplate->cy );

				if( MapDialogRect(GetSafeHwnd(), &rect) )
				{
					rectDlg.right = max( rectDlg.right, rect.right );
					rectDlg.bottom = max( rectDlg.bottom, rect.bottom );
				}

				UnlockResource( hTemplate );
			}

			::FreeResource( hTemplate );
		}
	}

	// Get size of IDC_ROUTINE_LIST
	CRect rectRoutineListBox;
	m_lstbxRoutines.GetWindowRect( &rectRoutineListBox );

	// Determine position of Refresh button
	m_btnRefresh.GetClientRect( &rectTemp );
	rect.right = rectDlg.right - CONTROL_BORDER;
	rect.left = rect.right - rectTemp.Width();
	rect.top = CONTROL_BORDER;
	rect.bottom = rect.top + rectTemp.Height();
	m_btnRefresh.MoveWindow( &rect, TRUE );

	// Determine position of Routine listbox prompt
	m_staticRoutinePrompt.GetClientRect( &rectTemp );
	rect.right = rect.left - CONTROL_BORDER;
	rect.left = rectDlg.right - CONTROL_BORDER - rectRoutineListBox.Width();
	rect.top = (CONTROL_BORDER << 1) + 3;
	rect.bottom = rect.top + rectTemp.Height();
	m_staticRoutinePrompt.MoveWindow( &rect, TRUE );

	// Determine position of Routine listbox
	rect.right = rectDlg.right - CONTROL_BORDER;
	rect.left = rect.right - rectRoutineListBox.Width();
	rect.top = rect.bottom + 4;
	rect.bottom = rect.top +  rectRoutineListBox.Height();
	rectRoutineListBox = rect;
	m_lstbxRoutines.MoveWindow( &rectRoutineListBox, TRUE );

	// Determine position of Initialize button
	m_btnInitialize.GetClientRect( &rectTemp );
	rect.left = rect.right - rectTemp.Width();
	rect.top = rect.bottom + 8;
	rect.bottom = rect.top + rectTemp.Height();
	m_btnInitialize.MoveWindow( &rect, TRUE );

	// Determine position of Variable listbox prompt
	m_staticVariablePrompt.GetClientRect( &rectTemp );
	rect.right = rect.left - CONTROL_BORDER;
	rect.left = rectDlg.right - CONTROL_BORDER - rectRoutineListBox.Width();
	rect.top += 8;
	rect.bottom = rect.top +  rectTemp.Height();
	m_staticVariablePrompt.MoveWindow( &rect, TRUE );

	// Determine position of Variable listbox
	rect.right = rectDlg.right - CONTROL_BORDER;
	rect.left = rect.right - rectRoutineListBox.Width();
	rect.top = rect.bottom + 4;
	rect.bottom = rectDlg.bottom - CONTROL_BORDER;
	m_lstbxVariables.MoveWindow( &rect, TRUE );

	// Determine position of IDC_SOURCE
	rect.right = rect.left - CONTROL_BORDER;
	rect.left = CONTROL_BORDER;
	rect.top = rectRoutineListBox.top;
	rect.bottom = rectDlg.bottom - CONTROL_BORDER;
	m_editSource.MoveWindow( &rect, TRUE );

	// Determine position of Source listbox prompt
	m_staticSourcePrompt.GetClientRect( &rectTemp );
	rect.left = CONTROL_BORDER;
	rect.right = rect.left + rectTemp.Width();
	rect.top = (CONTROL_BORDER << 1) + 3;
	rect.bottom = rect.top + rectTemp.Height();
	m_staticSourcePrompt.MoveWindow( &rect, TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnUpdateEditCut

void CScriptDlg::OnUpdateEditCut( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	if( ::IsWindow(m_editSource.m_hWnd) )
	{
		int nStartChar;
		int nEndChar;
		m_editSource.GetSel( nStartChar, nEndChar );
		if( nStartChar >= 0 
		&&  nEndChar >= 0 
		&& (nStartChar != nEndChar) )
		{
			pCmdUI->Enable( TRUE );
			return;
		}
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnEditCut

void CScriptDlg::OnEditCut() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );
	
	if( ::IsWindow(m_editSource.m_hWnd) )
	{
		m_editSource.Cut();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnUpdateEditCopy

void CScriptDlg::OnUpdateEditCopy( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	if( ::IsWindow(m_editSource.m_hWnd) )
	{
		int nStartChar;
		int nEndChar;
		m_editSource.GetSel( nStartChar, nEndChar );
		if( nStartChar >= 0 
		&&  nEndChar >= 0 
		&& (nStartChar != nEndChar) )
		{
			pCmdUI->Enable( TRUE );
			return;
		}
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnEditCopy

void CScriptDlg::OnEditCopy() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );
	
	if( ::IsWindow(m_editSource.m_hWnd) )
	{
		m_editSource.Copy();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnUpdateEditPaste

void CScriptDlg::OnUpdateEditPaste( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	BOOL fEnablePaste = FALSE;

	if( ::IsWindow(m_editSource.m_hWnd) )
	{
		// Get the IDataObject
		IDataObject* pIDataObject;
		if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
		{
			// Create a new CDllJazzDataObject and see if it can read the data object's format.
			CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
			if( pDataObject )
			{
				// Determine if it contains CF_TEXT
				if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, CF_TEXT ) ) )
				{
					fEnablePaste = TRUE;
				}

				delete pDataObject;
			}

			RELEASE( pIDataObject );
		}
	}
	
	pCmdUI->Enable( fEnablePaste );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnEditPaste

void CScriptDlg::OnEditPaste() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );
	
	if( ::IsWindow(m_editSource.m_hWnd) )
	{
		m_editSource.Paste();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnUpdateEditInsert

void CScriptDlg::OnUpdateEditInsert( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnEditInsert

void CScriptDlg::OnEditInsert() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );
	
	ASSERT( 0 );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnUpdateEditDelete

void CScriptDlg::OnUpdateEditDelete( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );

	if( ::IsWindow(m_editSource.m_hWnd) )
	{
		int nStartChar;
		int nEndChar;
		m_editSource.GetSel( nStartChar, nEndChar );
		if( nStartChar >= 0 
		&&  nEndChar >= 0 
		&& (nStartChar != nEndChar) )
		{
			pCmdUI->Enable( TRUE );
			return;
		}
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnEditDelete

void CScriptDlg::OnEditDelete() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );
	
	if( ::IsWindow(m_editSource.m_hWnd) )
	{
		m_editSource.Clear();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnEditSelectAll

void CScriptDlg::OnEditSelectAll() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScript != NULL );
	
	if( ::IsWindow(m_editSource.m_hWnd) )
	{
		m_editSource.SetSel( 0, -1, FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnViewProperties

BOOL CScriptDlg::OnViewProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdPropSheet* pIPropSheet;

	ASSERT( m_pScript != NULL );
	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	if( FAILED ( theApp.m_pScriptComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		return FALSE;
	}

	if( pIPropSheet->IsShowing() != S_OK )
	{
		RELEASE( pIPropSheet );
		return TRUE;
	}

	BOOL fSuccess = FALSE;
	if( SUCCEEDED ( m_pScript->OnShowProperties() ) )
	{
		fSuccess = TRUE;
	}

	RELEASE( pIPropSheet );

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnMouseActivate

int CScriptDlg::OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message )  
{
	switch( nHitTest )
	{
		case HTHSCROLL:
		case HTVSCROLL:
			if( m_pEditValueCtrl )
			{
				// Finish editing this variable
				OnEndEditValue( (WPARAM)m_pEditValueCtrl->GetSafeHwnd(), 0 );
			}
			break;
	}
	
	return CFormView::OnMouseActivate( pDesktopWnd, nHitTest, message );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnCommand

BOOL CScriptDlg::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( HIWORD(wParam) == 0 )	// menu command
	{
		switch( LOWORD(wParam) )
		{
			case IDM_CALL_ROUTINE:
				CallSelectedRoutines();
				return TRUE;

			case IDM_SET_VARIABLE:
				SetSelectedVariable();
				return TRUE;
		}
	}
	
	return CFormView::OnCommand( wParam, lParam );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnContextMenu

void CScriptDlg::OnContextMenu( CWnd* pWnd, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pWnd != NULL );
	ASSERT( m_pScript != NULL );

	switch( pWnd->GetDlgCtrlID() )
	{
		case IDC_ROUTINE_LIST:
		{
			// Load menu
			HMENU hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_ROUTINE_LIST) );
			HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

			// Initialize menu
			::EnableMenuItem( hMenuPopup, IDM_CALL_ROUTINE, ( m_lstbxRoutines.GetSelCount() > 0 )
								? MF_ENABLED : MF_GRAYED );

			::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, GetSafeHwnd(), NULL );
			::DestroyMenu( hMenu );
			break;
		}

		case IDC_VARIABLE_LIST:
		{
			// Load menu
			HMENU hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_VARIABLE_LIST) );
			HMENU hMenuPopup = ::GetSubMenu(hMenu, 0);

			// Initialize menu
			::EnableMenuItem( hMenuPopup, IDM_SET_VARIABLE, ( m_lstbxVariables.GetSelectedCount() > 0 )
								? MF_ENABLED : MF_GRAYED );

			::TrackPopupMenu( hMenuPopup, (TPM_LEFTALIGN | TPM_RIGHTBUTTON),
							  point.x, point.y, 0, GetSafeHwnd(), NULL );
			::DestroyMenu( hMenu );
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnDblClkRoutineList

void CScriptDlg::OnDblClkRoutineList() 
{
	CallSelectedRoutines();
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnKeyDownVariableList

void CScriptDlg::OnKeyDownVariableList( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	switch( pLVKeyDow->wVKey )
	{
		case VK_RIGHT:
			SetSelectedVariable();
			break;
	}
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnReturnVariableList

void CScriptDlg::OnReturnVariableList( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	SetSelectedVariable();
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnClickVariableList

void CScriptDlg::OnClickVariableList( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Select the variable
	int nItem = FindVariableAtYPos( pNMListView->ptAction.y );
	if( nItem >= 0 )
	{
		m_lstbxVariables.SetItemState( nItem, (LVIS_SELECTED|LVIS_FOCUSED), (LVIS_SELECTED|LVIS_FOCUSED) );

		RECT rect;
		m_lstbxVariables.GetItemRect( nItem, &rect, LVIR_BOUNDS );
		rect.right = rect.left + m_lstbxVariables.GetColumnWidth( 0 );
		if( pNMListView->ptAction.x >= rect.right )
		{
			SetSelectedVariable();
		}
	}
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnDblClkVariableList

void CScriptDlg::OnDblClkVariableList( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Select the variable
	int nItem = FindVariableAtYPos( pNMListView->ptAction.y );
	if( nItem >= 0 )
	{
		m_lstbxVariables.SetItemState( nItem, (LVIS_SELECTED|LVIS_FOCUSED), (LVIS_SELECTED|LVIS_FOCUSED) );

		RECT rect;
		m_lstbxVariables.GetItemRect( nItem, &rect, LVIR_BOUNDS );
		rect.right = rect.left + m_lstbxVariables.GetColumnWidth( 0 );
		if( pNMListView->ptAction.x < rect.right )
		{
			SetSelectedVariable();
		}
	}
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnRefresh

void CScriptDlg::OnRefresh( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		m_pScript->SyncScriptWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnInitVariables

void CScriptDlg::OnInitVariables( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript )
	{
		m_pScript->m_fInitializingVariables = true;
		m_pScript->SyncScriptWithDirectMusic();
		m_pScript->m_fInitializingVariables = false;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::CallSelectedRoutines

void CScriptDlg::CallSelectedRoutines() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	if( m_pScript == NULL
	||  m_pScript->m_pIDMScript == NULL )
	{
		// Can't do anything
		return;
	}

	int nNbrSelItems = m_lstbxRoutines.GetSelCount();
	if( nNbrSelItems == 0 )
	{
		// Nothing to do
		return;
	}

	CWaitCursor wait;

	CString strRoutine;
	WCHAR awchRoutineName[MAX_PATH];
	DMUS_SCRIPT_ERRORINFO ErrorInfo;
	HRESULT hr;

	int* pnSelItems = new int[nNbrSelItems];
	if( pnSelItems )
	{
		m_lstbxRoutines.GetSelItems( nNbrSelItems, pnSelItems );

		for( int i = 0;  i < nNbrSelItems ;  i++ )
		{
			m_lstbxRoutines.GetText( pnSelItems[i], strRoutine );
		
			// Convert the routine name to wide characters
			MultiByteToWideChar( CP_ACP, 0, strRoutine, -1, awchRoutineName, MAX_PATH );

			// Initialize ErrorInfo struct
			memset( &ErrorInfo, 0, sizeof(DMUS_SCRIPT_ERRORINFO) );
			ErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);

			// Call the routine
			hr = m_pScript->m_pIDMScript->CallRoutine( awchRoutineName, &ErrorInfo );
			if( FAILED ( hr ) )
			{
				m_pScript->DisplayScriptError( &ErrorInfo, hr );
			}
		}

		delete [] pnSelItems;
	}

	// Update variable list box
	FillVariableListBox();
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnSetFocusVariableList

void CScriptDlg::OnSetFocusVariableList( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	if( m_lstbxVariables.GetSelectedCount() == 0 )
	{
		int nTopPos = m_lstbxVariables.GetTopIndex();
		if( nTopPos >= 0 )
		{
			m_lstbxVariables.SetItemState( nTopPos, LVIS_FOCUSED, LVIS_FOCUSED );
		}
	}
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::SetSelectedVariable

void CScriptDlg::SetSelectedVariable() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	// Finish editing this variable before we start editing another one
	if( m_pEditValueCtrl )
	{
		OnEndEditValue( (WPARAM)m_pEditValueCtrl->GetSafeHwnd(), 0 );
	}

	if( m_pScript == NULL
	||  m_pScript->m_pIDMScript == NULL )
	{
		// Can't do anything
		return;
	}

	// Get selected variable
	int nEditItem = m_lstbxVariables.GetNextItem( -1, LVNI_SELECTED );
	if( nEditItem < 0 )
	{
		// Nothing to do
		return;
	}

	// Convert the variable name to wide characters
	WCHAR awchVariableName[MAX_PATH];
	m_strEditVariableName = m_lstbxVariables.GetItemText( nEditItem, 0 );
	MultiByteToWideChar( CP_ACP, 0, m_strEditVariableName, -1, awchVariableName, MAX_PATH );

	// Initialize ErrorInfo struct
	DMUS_SCRIPT_ERRORINFO ErrorInfo;
	memset( &ErrorInfo, 0, sizeof(DMUS_SCRIPT_ERRORINFO) );
	ErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);

	// Get current value
	VariantInit( &m_EditVariant );
	HRESULT hr = m_pScript->m_pIDMScript->GetVariableVariant( awchVariableName, &m_EditVariant, &ErrorInfo );
	if( FAILED ( hr ) )
	{
		m_pScript->DisplayScriptError( &ErrorInfo, hr );
		return;
	}

	// Translate value into string for edit control
	CString strValue;
	switch( m_EditVariant.vt )
	{
		case VT_I2:			// Number
			strValue.Format( "%d", V_I2(&m_EditVariant) );
			break;

		case VT_I4:			// Number
			strValue.Format( "%d", V_I4(&m_EditVariant) );
			break;

		case VT_BOOL:		// BOOL
			strValue.Format( "%d", V_BOOL(&m_EditVariant) );
			break;

		case VT_BSTR:		// String (Unicode)
			strValue = V_BSTR( &m_EditVariant );
			break;

		case VT_EMPTY:		// <Empty>
			strValue.Empty();
			break;

		case VT_UNKNOWN:	// (Object)
		case VT_DISPATCH:	
			return;			// Can't edit (Object)

		default:			// <Unknown>
			strValue.Empty();
			break;
	}

	// Determine coordinates for control
	RECT rect;
	m_lstbxVariables.GetItemRect( nEditItem, &rect, LVIR_BOUNDS );
	rect.left += m_lstbxVariables.GetColumnWidth( 0 );
	CDC* pDC = m_lstbxVariables.GetDC();
	if( pDC )
	{
		TEXTMETRIC tm;
		pDC->GetTextMetrics( &tm );
		rect.left += tm.tmAveCharWidth - 3;
		ReleaseDC( pDC );
	}

	// Create an edit control
	ASSERT( m_pEditValueCtrl == NULL );
	m_pEditValueCtrl = new CEditValue;
	if( m_pEditValueCtrl )
	{
		m_pEditValueCtrl->m_pScriptDlg = this;
		if( m_pEditValueCtrl->Create( (WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL),
									   rect, &m_lstbxVariables, IDC_EDIT_VALUE ) )
		{
			CFont* pFont = m_lstbxVariables.GetFont();
			if( pFont )
			{
				m_pEditValueCtrl->SetFont( pFont, FALSE );
			}
			m_pEditValueCtrl->SetWindowText( strValue );
			m_pEditValueCtrl->SetSel( 0, -1, FALSE );
			m_pEditValueCtrl->SetFocus();
		}
		else
		{
			delete m_pEditValueCtrl;
			m_pEditValueCtrl = NULL;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnEndEditValue

LRESULT CScriptDlg::OnEndEditValue( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript == NULL 
	||  m_pScript->m_pIDMScript == NULL
	||  m_pEditValueCtrl == NULL
	||  m_strEditVariableName.IsEmpty() )
	{
		// Can't do anything
		OnCancelEditValue( wParam, lParam );
		return 0;
	}

	if( (WPARAM)m_pEditValueCtrl->GetSafeHwnd() != wParam )
	{
		// Wrong control - so don't do anything
		return 0;
	}

	// Get the current value from the edit control
	CString strValue;
	m_pEditValueCtrl->GetWindowText( strValue );
	strValue.TrimRight();
	strValue.TrimLeft();

	// Is new value numeric?
	BOOL fIsNumeric = TRUE;
	for( int i = 0 ;  i < strValue.GetLength() ;  i++ )
	{
		if( strValue[i] == '-' )
		{
			if( i == 0 )
			{
				continue;
			}
		}
		if( !(isdigit(strValue[i])) )
		{
			fIsNumeric = FALSE;
			break;
		}
	}

	// Determine new value
	COleVariant* pVar;

	// VT_EMPTY
	if( strValue.IsEmpty() )
	{
		pVar = new COleVariant();
	}

	// VT_I4
	// VT_BOOL
	else if( fIsNumeric )
	{
		long lValue;
		if( strValue[0] == '-' )
		{
			if( strValue.GetLength() > 11 )
			{
				lValue = INT_MIN;
			}
			else
			{
				lValue = _ttol( strValue );
				if( lValue > 0 )
				{
					lValue = INT_MIN;
				}
			}
		}
		else
		{
			if( strValue.GetLength() > 10 )
			{
				lValue = INT_MAX;
			}
			else
			{
				lValue = _ttol( strValue );
				if( lValue < 0 )
				{
					lValue = INT_MAX;
				}
			}
		}
		if( m_EditVariant.vt == VT_BOOL )
		{
			pVar = new COleVariant( (short)lValue, VT_BOOL );
		}
		else
		{
			pVar = new COleVariant( lValue, VT_I4 );
		}
	}

	// VT_BSTR
	else
	{
		pVar = new COleVariant( strValue );
	}

	// Set new value
	{
		HRESULT hr;
		WCHAR awchVariableName[MAX_PATH];
		DMUS_SCRIPT_ERRORINFO ErrorInfo;

		// Convert the variable name to wide characters
		MultiByteToWideChar( CP_ACP, 0, m_strEditVariableName, -1, awchVariableName, MAX_PATH );

		// Initialize ErrorInfo struct
		memset( &ErrorInfo, 0, sizeof(DMUS_SCRIPT_ERRORINFO) );
		ErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);

		hr = m_pScript->m_pIDMScript->SetVariableVariant( awchVariableName, *pVar, FALSE, &ErrorInfo );
		if( FAILED ( hr ) )
		{
			m_pScript->DisplayScriptError( &ErrorInfo, hr );
		}
	}

	// Cleanup
	m_pEditValueCtrl->DestroyWindow();
	delete m_pEditValueCtrl;
	m_pEditValueCtrl = NULL;
	m_strEditVariableName.Empty();
	VariantClear( &m_EditVariant );
	delete pVar;
	pVar = NULL;

	// Refresh variable list box
	FillVariableListBox();
	m_lstbxVariables.SetFocus();

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnCancelEditValue

LRESULT CScriptDlg::OnCancelEditValue( WPARAM wParam, LPARAM lParam )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pEditValueCtrl
	&&  (WPARAM)m_pEditValueCtrl->GetSafeHwnd() != wParam )
	{
		// Wrong control - Shoul not happen!
		ASSERT( 0 );
		return 0;
	}

	if( m_pEditValueCtrl )
	{
		m_pEditValueCtrl->DestroyWindow();
		delete m_pEditValueCtrl;
		m_pEditValueCtrl = NULL;
	}

	m_strEditVariableName.Empty();
	VariantClear( &m_EditVariant );

	// Refresh variable list box
	FillVariableListBox();
	m_lstbxVariables.SetFocus();

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::IsInEditVariable

BOOL CScriptDlg::IsInEditVariable( void )
{
	if( m_pEditValueCtrl )
	{
		return TRUE;
	}
	
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::FindVariableAtYPos

int CScriptDlg::FindVariableAtYPos( int nYPos )
{
	int nIndex = -1;

	RECT rect;

	int nCount = m_lstbxVariables.GetItemCount();
	for ( int i = 0 ;  i < nCount ;  i++ )
	{
		m_lstbxVariables.GetItemRect( i, &rect, LVIR_BOUNDS );
		
		if( nYPos >= rect.top
		&&  nYPos <= rect.bottom )
		{
			nIndex = i;
			break;
		}
	}

	return nIndex;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptDlg::OnParentNotify

void CScriptDlg::OnParentNotify( UINT message, LPARAM lParam ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( message )
	{
		case WM_LBUTTONDOWN:
			if( m_pEditValueCtrl )
			{
				CPoint pt;
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);

				CRect rect;
				m_pEditValueCtrl->GetClientRect( &rect );
				m_pEditValueCtrl->ClientToScreen( &rect );
				ScreenToClient( &rect );

				if( rect.PtInRect(pt) == FALSE )
				{
					// Finish editing this variable
					OnEndEditValue( (WPARAM)m_pEditValueCtrl->GetSafeHwnd(), 0 );
				}
			}
			break;
	}

	CFormView::OnParentNotify( message, lParam );
}
