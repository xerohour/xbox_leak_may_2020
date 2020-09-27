// SegmentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XboxAddin.h"
#include "SegmentDlg.h"
#include "Segment.h"
#include "DllJazzDataObject.h"
#include "dmpprivate.h"
#include "dmusprod.h"
#pragma warning ( push )
#pragma warning ( disable : 4201 )
#include <Xbox-dmusici.h>
#pragma warning ( pop )
#include "XboxAddinComponent.h"
#include "SegmentListBox.h"
#include "XboxAddinDlg.h"
#include "LeftPaneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg dialog


CSegmentDlg::CSegmentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSegmentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSegmentDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_cfFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );
	m_cfSegment = ::RegisterClipboardFormat( CF_SEGMENT );
	m_plstSegments = NULL;
}

CSegmentDlg::~CSegmentDlg()
{
	while( !m_lstSegmentsToSynchronize.IsEmpty() )
	{
		delete m_lstSegmentsToSynchronize.RemoveHead();
	}
}

void CSegmentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSegmentDlg)
	DDX_Control(pDX, IDC_STATIC_SEGMENT, m_staticSegment);
	DDX_Control(pDX, IDC_LIST_SEGMENT, m_listSegment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSegmentDlg, CDialog)
	//{{AFX_MSG_MAP(CSegmentDlg)
	ON_WM_SIZE()
	ON_MESSAGE(WM_APP,OnApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSegmentDlg message handlers

void CSegmentDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if( nType == SIZE_MINIMIZED )
	{
		return;
	}

	// Exit if we are not fully created yet
	if( !::IsWindow( m_listSegment.GetSafeHwnd() ) )
	{
		return;
	}

	RECT rectWindow;
	GetClientRect( &rectWindow );

	RECT rect;
	m_staticSegment.GetClientRect( &rect );
	rect.right = rectWindow.right;
	rect.bottom += 2 * ::GetSystemMetrics(SM_CYBORDER);
	m_staticSegment.MoveWindow( &rect );

	rect.top = rect.bottom;
	rect.bottom = rectWindow.bottom;
	m_listSegment.MoveWindow( &rect );
}

BOOL CSegmentDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT( m_plstSegments );

	POSITION pos = m_plstSegments->GetHeadPosition();
	while( pos )
	{
		CSegment *pSegment = m_plstSegments->GetNext( pos );
		m_listSegment.AddString( (LPCTSTR) pSegment );
	}

	m_DropTarget.m_pDropControl = this;
	::RegisterDragDrop( m_listSegment.GetSafeHwnd(), &m_DropTarget );

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CSegmentDlg::OnOK() 
{
	// Do nothing
}

LRESULT CSegmentDlg::OnApp(WPARAM wParam, LPARAM lParam)
{
	bool fSetAudiopath = true;
	CAudiopath *pAudiopath = NULL;
	DWORD dwStandardAudiopath = 0;
	switch( wParam )
	{
	case ID_DELETE_SEGMENT:
		DeleteSegment( (CSegment *) lParam, false );
	default:
		fSetAudiopath = false;
		break;
	case ID__USEAUDIOPATH_AUDIOPATH1:
		pAudiopath = reinterpret_cast<CAudiopath *>(theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->m_pLeftPaneDlg->m_listAudiopath.GetItemDataPtr( 0 ));
		break;
	case ID__USEAUDIOPATH_AUDIOPATH2:
		pAudiopath = reinterpret_cast<CAudiopath *>(theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->m_pLeftPaneDlg->m_listAudiopath.GetItemDataPtr( 1 ));
		break;
	case ID__USEAUDIOPATH_AUDIOPATH3:
		pAudiopath = reinterpret_cast<CAudiopath *>(theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->m_pLeftPaneDlg->m_listAudiopath.GetItemDataPtr( 2 ));
		break;
	case ID__USEAUDIOPATH_STANDARDSTEREOREVERB:
		dwStandardAudiopath = DMUS_APATH_SHARED_STEREOPLUSREVERB;
		break;
	case ID__USEAUDIOPATH_STANDARD3DDRY:
		dwStandardAudiopath = DMUS_APATH_DYNAMIC_3D;
		break;
	case ID__USEAUDIOPATH_STANDARDMONO:
		dwStandardAudiopath = DMUS_APATH_DYNAMIC_MONO;
		break;
	case ID__USEAUDIOPATH_STANDARDSHAREDSTEREO:
		dwStandardAudiopath = DMUS_APATH_SHARED_STEREO;
		break;
	case ID__USEAUDIOPATH_STANDARDQUAD:
		dwStandardAudiopath = DMUS_APATH_MIXBIN_QUAD;
		break;
	case ID__USEAUDIOPATH_STANDARDQUADMUSIC:
		dwStandardAudiopath = DMUS_APATH_MIXBIN_QUAD_MUSIC;
		break;
	case ID__USEAUDIOPATH_STANDARDQUADENV:
		dwStandardAudiopath = DMUS_APATH_MIXBIN_QUAD_ENV;
		break;
	case ID__USEAUDIOPATH_STANDARD51:
		dwStandardAudiopath = DMUS_APATH_MIXBIN_5DOT1;
		break;
	case ID__USEAUDIOPATH_STANDARD51MUSIC:
		dwStandardAudiopath = DMUS_APATH_MIXBIN_5DOT1_MUSIC;
		break;
	case ID__USEAUDIOPATH_STANDARD51ENV:
		dwStandardAudiopath = DMUS_APATH_MIXBIN_5DOT1_ENV;
		break;
	case ID__USEAUDIOPATH_STANDARDSTEREOEFFECT:
		dwStandardAudiopath = DMUS_APATH_MIXBIN_STEREO_EFFECTS;
		break;
	}

	if( fSetAudiopath )
	{
		CSegment *pSegment = (CSegment *) lParam;
		pSegment->m_pAudiopath = pAudiopath;
		pSegment->m_dwStandardAudiopath = dwStandardAudiopath;
	}

	return 0;
}

void CSegmentDlg::InternalDragOver( DWORD grfKeyState, POINTL ptScreen, IDataObject* pIDataObject, DWORD* pdwEffect )
{
	UNREFERENCED_PARAMETER( grfKeyState );
	UNREFERENCED_PARAMETER( ptScreen );
	if( !theApp.m_fConnected )
	{
		*pdwEffect = DROPEFFECT_NONE;
		return;
	}

	/*
	// Highlight the item under the mouse and 
	CMultiTree *pDestTreeCtrl = &(m_pAudioPathDlg->m_tcTree);
	POINT point = {pointl.x, pointl.y};
	pDestTreeCtrl->ScreenToClient( &point );

	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	pDestTreeCtrl->GetItemBufferEffectUnderPoint( point, &pItemInfo, &pBufferOptions, NULL );
	
	// Scroll Tree control depending on mouse position
	CRect rectClient;
	pDestTreeCtrl->GetClientRect(&rectClient);
	pDestTreeCtrl->ClientToScreen(rectClient);
	pDestTreeCtrl->ClientToScreen(&point);
	int nScrollDir = -1;
	if ( point.y >= rectClient.bottom - RECT_BORDER)
		nScrollDir = SB_LINEDOWN;
	else
	if ( (point.y <= rectClient.top + RECT_BORDER) )
		nScrollDir = SB_LINEUP;

	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = pDestTreeCtrl->GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		pDestTreeCtrl->SendMessage(WM_VSCROLL, wParam);
	}
	
	nScrollDir = -1;
	if ( point.x <= rectClient.left + RECT_BORDER )
		nScrollDir = SB_LINELEFT;
	else
	if ( point.x >= rectClient.right - RECT_BORDER)
		nScrollDir = SB_LINERIGHT;
	
	if ( nScrollDir != -1 ) 
	{
		int nScrollPos = pDestTreeCtrl->GetScrollPos(SB_VERT);
		WPARAM wParam = MAKELONG(nScrollDir, nScrollPos);
		pDestTreeCtrl->SendMessage(WM_HSCROLL, wParam);
	}
	*/

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject )
	{
		if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_cfSegment ) ) )
		{
			dwEffect = DROPEFFECT_COPY;
		}
		else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_cfFile ) ) )
		{
			IDMUSProdNode *pIDMUSProdNode = NULL;
			if( theApp.m_pXboxAddinComponent
			&&	theApp.m_pXboxAddinComponent->m_pIFramework
			&&	SUCCEEDED( theApp.m_pXboxAddinComponent->m_pIFramework->GetDocRootNodeFromData( pIDataObject, &pIDMUSProdNode ) ) )
			{
				GUID guidNode = GUID_NULL;
				if( SUCCEEDED( pIDMUSProdNode->GetNodeId( &guidNode ) )
				&&	guidNode == GUID_SegmentNode)
				{
					dwEffect = DROPEFFECT_COPY;
				}
				pIDMUSProdNode->Release();
			}
		}

		delete pDataObject;
	}

	*pdwEffect = dwEffect;
}

void CSegmentDlg::InternalDrop( IDataObject* pIDataObject, POINTL ptScreen, DWORD dwEffect)
{
	UNREFERENCED_PARAMETER( ptScreen );

	if( dwEffect != DROPEFFECT_COPY )
	{
		return;
	}

	// Display a wait cursor - this may take a while
	CWaitCursor waitCursor;

	IDMUSProdNode *pIDMUSProdNode = NULL;
	if( theApp.m_pXboxAddinComponent
	&&	theApp.m_pXboxAddinComponent->m_pIFramework
	&&	SUCCEEDED( theApp.m_pXboxAddinComponent->m_pIFramework->GetDocRootNodeFromData( pIDataObject, &pIDMUSProdNode ) ) )
	{
		GUID guidNode = GUID_NULL;
		if( SUCCEEDED( pIDMUSProdNode->GetNodeId( &guidNode ) )
		&&	guidNode == GUID_SegmentNode)
		{
			// Add it to our internal list and the display
			AddNodeToDisplay( pIDMUSProdNode );
		}
		pIDMUSProdNode->Release();
	}
	/*
	//Remove highlighting
	//m_pAudioPathDlg->m_tcTree.SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE,0);

	POINT ptShort = {point.x, point.y};
	m_pAudioPathDlg->m_tcTree.ScreenToClient( &ptShort );

	HRESULT hr = m_pAudioPathDlg->DropOnTree( pIDataObject, m_dwOverDragEffect, ptShort );
	*/
}

void CSegmentDlg::InternalDragLeave( void )
{
	if( theApp.m_fConnected )
	{
		//m_pAudioPathDlg->m_tcTree.SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE,0);
	}
}

HRESULT CSegmentDlg::AddSegmentToList( CSegment *pSegment )
{
	if( pSegment == NULL )
	{
		return E_POINTER;
	}

	ASSERT( m_plstSegments );

	// Check if segment name is already used
	int nAppendValue = 0;
	bool fFound = true;
	CString strSegmentName = pSegment->GetFileName();
	while( fFound )
	{
		fFound = false;
		POSITION pos = m_plstSegments->GetHeadPosition();
		while( pos )
		{
			CSegment *pTemp = m_plstSegments->GetNext( pos );
			if( strSegmentName == pTemp->GetFileName()
			&&	nAppendValue == pTemp->m_nAppendValue )
			{
				fFound = true;
				nAppendValue++;
				break;
			}
		}
	}

	// Set the segment's append value
	pSegment->SetAppendValue( nAppendValue );

	// Add to our list
	m_plstSegments->AddHead( pSegment );

	// Copy to Xbox
	HRESULT hr = pSegment->CopyToXbox( );

	if( SUCCEEDED(hr) )
	{
		// Add it to the display
		if( m_listSegment.GetSafeHwnd() )
		{
			m_listSegment.AddString( (LPCTSTR) pSegment );
		}
	}
	else
	{
		// Remove it from our list
		POSITION pos = m_plstSegments->Find( pSegment );
		if( pos )
		{
			m_plstSegments->RemoveAt( pos );
		}

		// Delete it
		delete pSegment;

		// Now, clean up the display
		theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->CleanUpDisplay();
	}

	return hr;
}

void CSegmentDlg::OnConnectionStateChanged( void )
{
	m_listSegment.EnableWindow( theApp.m_fConnected );
	m_staticSegment.EnableWindow( theApp.m_fConnected );
}

bool CSegmentDlg::IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode )
{
	ASSERT( m_plstSegments );

	POSITION pos = m_plstSegments->GetHeadPosition();
	while( pos )
	{
		CSegment *pSegment = m_plstSegments->GetNext( pos );
		if( pSegment->ContainsNode( pIDMUSProdNode ) )
		{
			return true;
		}
	}
	return false;
}

HRESULT CSegmentDlg::AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode )
{
	// Add it to our internal list
	return AddSegmentToList( new CSegment(pIDMUSProdNode) );
}

void CSegmentDlg::HandleNotification( NOTIFICATION_TYPE notificationType, DWORD dwSegmentID, DWORD dwData1 )
{
	// Find the segment from the given ID
	CSegment *pSegment = SegmentFromID( dwSegmentID );

	switch( notificationType )
	{
	case NOTIFICATION_SEGMENT:
		HandleSegmentNotification( pSegment, dwData1 );
		break;
	default:
		break;
	}
}

void CSegmentDlg::HandleSegmentNotification( CSegment *pSegment, DWORD dwNotification )
{
	// Only handle SEGEND and SEGABORT notifications
	if( (dwNotification != DMUS_NOTIFICATION_SEGEND)
	&&	(dwNotification != DMUS_NOTIFICATION_SEGABORT) )
	{
		return;
	}

	// If we found the segment that stopped
	if( pSegment )
	{
		// Tell it that it stopped
		pSegment->OnSegEnd();

		// Search for it in the listbox
		for( int nIndex = m_listSegment.GetCount() - 1; nIndex >= 0; nIndex-- )
		{
			if( m_listSegment.GetItemDataPtr( nIndex ) == pSegment )
			{
				// Redraw it
				RECT rectItem;
				m_listSegment.GetItemRect( nIndex, &rectItem );
				m_listSegment.InvalidateRect( &rectItem, TRUE );
				break;
			}
		}
	}
}

void CSegmentDlg::OnPanic( void )
{
	// Mark all segments as not playing
	ASSERT( m_plstSegments );
	POSITION pos = m_plstSegments->GetHeadPosition();
	while( pos )
	{
		CSegment *pSegment = m_plstSegments->GetNext( pos );
		pSegment->m_fPlaying = false;
	}

	// Redraw the listbox
	m_listSegment.Invalidate( TRUE );
}

CSegment *CSegmentDlg::SegmentFromID( DWORD dwID )
{
	ASSERT( m_plstSegments );

	POSITION pos = m_plstSegments->GetHeadPosition();
	while( pos )
	{
		CSegment *pSegment = m_plstSegments->GetNext( pos );
		if( pSegment->m_dwIndex == dwID )
		{
			return pSegment;
		}
	}

	return NULL;
}

void CSegmentDlg::DeleteSegment( CSegment *pSegment, bool fForceDeletion )
{
	// Block deletion if the segment is referenced by another file
	if( !fForceDeletion )
	{
		if( theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->IsFileInUse( pSegment ) )
		{
			return;
		}
	}

	// Unload it from the Xbox
	if( FAILED( pSegment->Unload() ) )
	{
		CString strErrorText;
		strErrorText.FormatMessage( IDS_ERR_UNLOAD, pSegment->GetName() );
		XboxAddinMessageBox( this, strErrorText, MB_ICONERROR | MB_OK );
		return;
	}

	// Remove it from the Xbox
	if( FAILED( pSegment->RemoveFromXbox() ) )
	{
		CString strErrorText;
		strErrorText.FormatMessage( IDS_ERR_REMOVE_SEGMENT, pSegment->GetName() );
		XboxAddinMessageBox( this, strErrorText, MB_ICONERROR | MB_OK );
		return;
	}

	// Remove it from the display
	int nIndex = m_listSegment.IndexFromFile( pSegment );
	if( nIndex != LB_ERR )
	{
		m_listSegment.DeleteString( nIndex );
	}

	// Remove it from the list
	ASSERT( m_plstSegments );
	POSITION posSegment = m_plstSegments->Find( pSegment );
	if( posSegment )
	{
		m_plstSegments->RemoveAt( posSegment );
	}

	// Finally, delete it
	delete pSegment;

	// Now, clean up the display
	theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->CleanUpDisplay();
}

bool CSegmentDlg::IsFileInUse( CFileItem *pFileItem )
{
	ASSERT( m_plstSegments );

	POSITION pos = m_plstSegments->GetHeadPosition();
	while( pos )
	{
		CSegment *pSegment = m_plstSegments->GetNext( pos );
		if( (CFileItem *)pSegment != pFileItem )
		{
			if( pSegment->UsesFile( pFileItem ) )
			{
				return true;
			}
		}
	}

	return false;
}

void CSegmentDlg::UnloadAll( void )
{
	ASSERT( m_plstSegments );

	POSITION pos = m_plstSegments->GetHeadPosition();
	while( pos )
	{
		CSegment *pSegment = m_plstSegments->GetNext( pos );
		pSegment->Unload();
	}
}

void CSegmentDlg::DeleteAll( void )
{
	ASSERT( m_plstSegments );

	// Copy the segments to a temporary list and delete them from the Xbox
	while( !m_plstSegments->IsEmpty() )
	{
		CSegment *pSegment = m_plstSegments->RemoveHead();
		m_lstSegmentsToSynchronize.AddHead( pSegment );
		pSegment->RemoveFromXbox();
	}
}

void CSegmentDlg::ReCopyAll( void )
{
	// Clear the display
	m_listSegment.ResetContent();

	// Re-add the segments
	IDMUSProdProject *pIDMUSProdProject;
	while( !m_lstSegmentsToSynchronize.IsEmpty() )
	{
		// Remove the segment from our temporary list
		CSegment *pSegment = m_lstSegmentsToSynchronize.RemoveHead();

		// Check if the node still exists
		pIDMUSProdProject = NULL;
		if( SUCCEEDED( theApp.m_pXboxAddinComponent->m_pIFramework->FindProject( pSegment->m_pFileNode, &pIDMUSProdProject ) )
		&&	pIDMUSProdProject )
		{
			// Yes - add the segment to the display
			AddNodeToDisplay( pSegment->m_pFileNode );
			pIDMUSProdProject->Release();
		}

		// Delete the temporary segment
		delete pSegment;
	}
}
