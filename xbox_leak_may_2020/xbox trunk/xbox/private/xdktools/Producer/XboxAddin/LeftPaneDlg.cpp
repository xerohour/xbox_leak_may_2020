// LeftPaneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XboxAddin.h"
#include "XboxAddinComponent.h"
#include "XboxAddinDlg.h"
#include "LeftPaneDlg.h"
#include "Audiopath.h"
#include "PrimaryDlg.h"
#include "SecondaryDlg.h"
#include "dmpprivate.h"
#include "DllJazzDataObject.h"
#pragma warning ( push )
#pragma warning ( disable : 4201 )
#include <Xbox-dmusici.h>
#pragma warning ( pop )
#include "Segment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftPaneDlg dialog


CLeftPaneDlg::CLeftPaneDlg(CWnd* pParent /*=NULL*/)
: CEndTrack(CLeftPaneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLeftPaneDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    m_wSplitterPos = 0;
    m_pPrimaryDlg = NULL;
    m_pSecondaryDlg = NULL;
	m_plstAudiopaths = &theApp.m_pXboxAddinComponent->m_lstAudiopaths;
	m_cfFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );
	m_cfAudiopath = ::RegisterClipboardFormat( CF_AUDIOPATH );
}

CLeftPaneDlg::~CLeftPaneDlg()
{
    while( !m_lstAudiopathsToSynchronize.IsEmpty() )
    {
        delete m_lstAudiopathsToSynchronize.RemoveHead();
    }
	if( m_pPrimaryDlg )
	{
		delete m_pPrimaryDlg;
		m_pPrimaryDlg = NULL;
	}
	if( m_pSecondaryDlg )
	{
		delete m_pSecondaryDlg;
		m_pSecondaryDlg = NULL;
	}
}

void CLeftPaneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLeftPaneDlg)
	DDX_Control(pDX, IDC_STATIC_AUDIOPATH, m_staticAudiopath);
	DDX_Control(pDX, IDC_LIST_AUDIOPATH, m_listAudiopath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLeftPaneDlg, CDialog)
	//{{AFX_MSG_MAP(CLeftPaneDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_APP,OnApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftPaneDlg message handlers

BOOL CLeftPaneDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // Resize the panes
    /*
    RECT rectClient;
    GetClientRect( &rectClient );
    OnSize( SIZE_RESTORED, rectClient.right, rectClient.bottom );
    */

	ASSERT( m_plstAudiopaths );

    POSITION pos = m_plstAudiopaths->GetHeadPosition();
    while( pos )
    {
        m_listAudiopath.AddString( (LPCTSTR) m_plstAudiopaths->GetNext( pos ) );
    }

	m_DropTarget.m_pDropControl = this;
	::RegisterDragDrop( m_listAudiopath.GetSafeHwnd(), &m_DropTarget );
	
    UpdateListBoxPositions();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CLeftPaneDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create the Splitter and position it in the Editor
	// (temporary values added until coords are saved to design-time file)
	RECT rect;
	rect.left = lpCreateStruct->x;
    rect.right = lpCreateStruct->cx;
    rect.top = lpCreateStruct->y;
	rect.bottom = rect.top + DEFAULTSPLITTERHEIGHT;

	m_wndHSplitter.Create( this, &rect );

	// Create the Secondary segment dialog and position it in the First Splitter Pane
	m_pSecondaryDlg = new CSecondaryDlg;
	if( m_pSecondaryDlg == NULL )
	{
		return -1;
	}

	// Point the Secondary segment dialog back to us and at the AudioPath object
	m_pSecondaryDlg->Create( IDD_SEGMENT, this );
	m_pSecondaryDlg->ShowWindow( SW_SHOW );
	m_wndHSplitter.SetFirstPane( m_pSecondaryDlg );

	// Create the Primary segment and position it in the Second Splitter Pane
	m_pPrimaryDlg = new CPrimaryDlg;
	if( m_pPrimaryDlg == NULL )
	{
		return -1;
	}

	// Point the EffectList dialog back to us and at the AudioPath object
	//m_pEffectListDlg->m_pAudioPath = m_pAudioPath;
	//m_pEffectListDlg->m_pAudioPathCtrl = this;
	m_pPrimaryDlg->Create( IDD_SEGMENT, this );
	m_pPrimaryDlg->ShowWindow( SW_SHOW );
	m_wndHSplitter.SetSecondPane( m_pPrimaryDlg );
	
	return 0;
}

void CLeftPaneDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

    UpdateListBoxPositions();
}

void CLeftPaneDlg::UpdateListBoxPositions( void )
{
    if( m_staticAudiopath.GetSafeHwnd() == NULL )
    {
        return;
    }

    RECT rectClient;
    GetClientRect( &rectClient );

    // Update width of audiopath boxes
    RECT rect;
    m_staticAudiopath.GetClientRect( &rect );
    rect.right = rectClient.right;
    rect.bottom += 2 * ::GetSystemMetrics(SM_CYBORDER);
    m_staticAudiopath.MoveWindow( &rect );

    RECT rectList;
    m_listAudiopath.GetClientRect( &rectList );
    rect.top = rect.bottom;
    rect.bottom += rectList.bottom + 2 * ::GetSystemMetrics(SM_CYEDGE);
    m_listAudiopath.MoveWindow( &rect );

    // Save height offset
    long lHeightOffset = rect.bottom;
    m_wndHSplitter.SetHeightOffset( lHeightOffset );

    // get splitter position from audio path
	WORD pos = m_wSplitterPos;

	// calculate where splitter should be
	if(pos == 0)
	{
		// put it right in the middle
		pos = WORD(min( USHRT_MAX, (rectClient.bottom - DEFAULTSPLITTERHEIGHT)/2 ));

		// save it
        m_wSplitterPos = pos;
	}

    // Update splitter position, if it's non-zero
	if( pos != 0 )
	{
		// splitter is right side of chord dialog and left side
		// of signpost dialog
		rect.bottom = lHeightOffset + max( DEFAULTSPLITTERHEIGHT, pos );
		rect.top = rect.bottom - DEFAULTSPLITTERHEIGHT;
		m_wndHSplitter.SetTracker(rect);
		m_wndHSplitter.MoveWindow( &rect, TRUE );
		m_pSecondaryDlg->MoveWindow(0, lHeightOffset, rect.right, rect.top);
		m_pPrimaryDlg->MoveWindow(0, rect.bottom, rect.right, rectClient.bottom - rect.bottom);
	}
}

void CLeftPaneDlg::EndTrack( long lNewPos )
{
    m_wSplitterPos = (WORD)lNewPos;
}

void CLeftPaneDlg::OnConnectionStateChanged( void )
{
	ASSERT( m_plstAudiopaths );
	if( theApp.m_fConnected
	&&	0 == m_plstAudiopaths->GetCount() )
	{
		AddStandard( DMUS_APATH_SHARED_STEREOPLUSREVERB );
	}

	m_staticAudiopath.EnableWindow( theApp.m_fConnected );
	m_listAudiopath.EnableWindow( theApp.m_fConnected );

	m_pSecondaryDlg->OnConnectionStateChanged();
	m_pPrimaryDlg->OnConnectionStateChanged();
}

bool CLeftPaneDlg::IsNodeDisplayed( const IDMUSProdNode *pIDMUSProdNode )
{
	return m_pPrimaryDlg->IsNodeDisplayed( pIDMUSProdNode ) || m_pSecondaryDlg->IsNodeDisplayed( pIDMUSProdNode );
}

HRESULT CLeftPaneDlg::AddNodeToDisplay( IDMUSProdNode *pIDMUSProdNode )
{
	return m_pPrimaryDlg->AddNodeToDisplay( pIDMUSProdNode );
}

void CLeftPaneDlg::HandleNotification( NOTIFICATION_TYPE notificationType, DWORD dwSegmentID, DWORD dwData1 )
{
	m_pSecondaryDlg->HandleNotification( notificationType, dwSegmentID, dwData1 );
	m_pPrimaryDlg->HandleNotification( notificationType, dwSegmentID, dwData1 );
}

void CLeftPaneDlg::OnPanic( void )
{
	m_pSecondaryDlg->OnPanic();
	m_pPrimaryDlg->OnPanic();
}

bool CLeftPaneDlg::IsFileInUse( CFileItem *pFileItem )
{
	return m_pPrimaryDlg->IsFileInUse( pFileItem ) || m_pSecondaryDlg->IsFileInUse( pFileItem );
}

void CLeftPaneDlg::UnloadAll( void )
{
	m_pSecondaryDlg->UnloadAll();
	m_pPrimaryDlg->UnloadAll();
}

void CLeftPaneDlg::DeleteAll( void )
{
	ASSERT( m_plstAudiopaths );

	// Copy the audiopaths to a temporary list and delete them from the Xbox
	while( !m_plstAudiopaths->IsEmpty() )
	{
		CAudiopath *pAudiopath = m_plstAudiopaths->RemoveHead();
		m_lstAudiopathsToSynchronize.AddHead( pAudiopath );
        pAudiopath->Release();
		pAudiopath->RemoveFromXbox();
	}

	m_pSecondaryDlg->DeleteAll();
	m_pPrimaryDlg->DeleteAll();

	// Now, clean up the display
	theApp.m_pXboxAddinComponent->m_pXboxAddinDlg->CleanUpDisplay();
}

void CLeftPaneDlg::ReCopyAll( void )
{
	// Clear the display
    m_listAudiopath.ResetContent();

	// Re-add the audiopaths
	IDMUSProdProject *pIDMUSProdProject;
	while( !m_lstAudiopathsToSynchronize.IsEmpty() )
	{
		// Remove the audiopath from our temporary list
		CAudiopath *pAudiopath = m_lstAudiopathsToSynchronize.RemoveHead();

		// Check if this is a standard audiopath, or if the node still exists
		pIDMUSProdProject = NULL;
		if( pAudiopath->m_dwStandardType
        ||  (SUCCEEDED( theApp.m_pXboxAddinComponent->m_pIFramework->FindProject( pAudiopath->m_pFileNode, &pIDMUSProdProject ) )
			 && pIDMUSProdProject) )
		{
			// Yes - add the audiopath to the display
            AddAudiopathToList( new CAudiopath( pAudiopath->m_pFileNode, pAudiopath->m_dwStandardType ) );
			if( pIDMUSProdProject )
			{
				pIDMUSProdProject->Release();
			}
		}

		// Delete the temporary audiopath
		delete pAudiopath;
	}

    m_pSecondaryDlg->ReCopyAll();
	m_pPrimaryDlg->ReCopyAll();
}

void CLeftPaneDlg::InternalDragOver( DWORD grfKeyState, POINTL ptScreen, IDataObject* pIDataObject, DWORD* pdwEffect )
{
	UNREFERENCED_PARAMETER( grfKeyState );
	UNREFERENCED_PARAMETER( ptScreen );
	if( !theApp.m_fConnected )
	{
		*pdwEffect = DROPEFFECT_NONE;
		return;
	}

	/*
	// Highlight the item under the mouse 
	CMultiTree *pDestTreeCtrl = &(m_pAudioPathDlg->m_tcTree);
	POINT point = {pointl.x, pointl.y};
	pDestTreeCtrl->ScreenToClient( &point );

	ItemInfo *pItemInfo;
	BufferOptions *pBufferOptions;
	pDestTreeCtrl->GetItemBufferEffectUnderPoint( point, &pItemInfo, &pBufferOptions, NULL );
	*/

	// Determine effect of drop
	DWORD dwEffect = DROPEFFECT_NONE;

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject )
	{
		/*
		if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_cfAudiopath ) ) )
		{
			dwEffect = DROPEFFECT_COPY;
		}
		else*/ if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_cfFile ) ) )
		{
			IDMUSProdNode *pIDMUSProdNode = NULL;
			if( theApp.m_pXboxAddinComponent
			&&	theApp.m_pXboxAddinComponent->m_pIFramework
			&&	SUCCEEDED( theApp.m_pXboxAddinComponent->m_pIFramework->GetDocRootNodeFromData( pIDataObject, &pIDMUSProdNode ) ) )
			{
				GUID guidNode = GUID_NULL;
				if( SUCCEEDED( pIDMUSProdNode->GetNodeId( &guidNode ) )
				&&	guidNode == GUID_AudioPathNode)
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

void CLeftPaneDlg::InternalDrop( IDataObject* pIDataObject, POINTL ptScreen, DWORD dwEffect)
{
	UNREFERENCED_PARAMETER( ptScreen );

	if( dwEffect != DROPEFFECT_COPY )
	{
		return;
	}

	// Display a wait cursor - this may take a while
	CWaitCursor waitCursor;

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject )
	{
		IDMUSProdNode *pIDMUSProdNode = NULL;
		/*
		if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_cfAudiopath ) ) )
		{
		}
		else*/ if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_cfFile ) ) )
		{
			if( theApp.m_pXboxAddinComponent
			&&	theApp.m_pXboxAddinComponent->m_pIFramework
			&&	SUCCEEDED( theApp.m_pXboxAddinComponent->m_pIFramework->GetDocRootNodeFromData( pIDataObject, &pIDMUSProdNode ) ) )
			{
				GUID guidNode = GUID_NULL;
				if( SUCCEEDED( pIDMUSProdNode->GetNodeId( &guidNode ) )
				&&	guidNode == GUID_AudioPathNode)
				{
					// Add it to our internal list and the display
					AddAudiopathToDisplay( ptScreen, pIDMUSProdNode );
				}
				pIDMUSProdNode->Release();
			}
		}
		delete pDataObject;
	}

	/*
	//Remove highlighting
	//m_pAudioPathDlg->m_tcTree.SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE,0);

	POINT ptShort = {point.x, point.y};
	m_pAudioPathDlg->m_tcTree.ScreenToClient( &ptShort );

	HRESULT hr = m_pAudioPathDlg->DropOnTree( pIDataObject, m_dwOverDragEffect, ptShort );
	*/
}

void CLeftPaneDlg::InternalDragLeave( void )
{
	if( theApp.m_fConnected )
	{
		//m_pAudioPathDlg->m_tcTree.SendMessage(TVM_SELECTITEM, TVGN_DROPHILITE,0);
	}
}

void CLeftPaneDlg::AddAudiopathToDisplay( POINTL ptScreen, IDMUSProdNode *pIDMUSProdNode )
{
	ASSERT( m_plstAudiopaths );
	if( m_plstAudiopaths->GetCount() < MAX_NUM_AUDIOPATHS )
	{
		// Create the audiopath item and add it to our list
		AddAudiopathToList( new CAudiopath( pIDMUSProdNode, 0 ) );
	}
}

void CLeftPaneDlg::SetDefault( CAudiopath *pAudiopath )
{
	ASSERT( m_plstAudiopaths );
	if( SUCCEEDED( pAudiopath->SetDefault( true ) ) )
	{
		POSITION pos = m_plstAudiopaths->GetHeadPosition();
		while( pos )
		{
			CAudiopath *pTemp = m_plstAudiopaths->GetNext( pos );
			if( pTemp != pAudiopath )
			{
				pTemp->SetDefault( false );
			}
		}

		m_listAudiopath.Invalidate( TRUE );
	}
}

void CLeftPaneDlg::AddStandard( DWORD dwType )
{
	ASSERT( m_plstAudiopaths );
	if( m_plstAudiopaths->GetCount() < MAX_NUM_AUDIOPATHS )
	{
		// Create the audiopath item and add it to our list
		AddAudiopathToList( new CAudiopath( NULL, dwType ) );
	}
}

void CLeftPaneDlg::DeleteAudiopath( CAudiopath *pAudiopath )
{
	// Release it on the Xbox
	if( FAILED( pAudiopath->Release() ) )
	{
		CString strErrorText;
		strErrorText.FormatMessage( IDS_ERR_RELEASE_AUDIOPATH, pAudiopath->GetName() );
		XboxAddinMessageBox( this, strErrorText, MB_ICONERROR | MB_OK );
		return;
	}

	// Remove it from the Xbox
	if( FAILED( pAudiopath->RemoveFromXbox() ) )
	{
		CString strErrorText;
		strErrorText.FormatMessage( IDS_ERR_REMOVE_AUDIOPATH, pAudiopath->GetName() );
		XboxAddinMessageBox( this, strErrorText, MB_ICONERROR | MB_OK );
		return;
	}

	// Remove it from our list
	ASSERT( m_plstAudiopaths );
	POSITION pos = m_plstAudiopaths->Find( pAudiopath, NULL );
	if( pos )
	{
		m_plstAudiopaths->RemoveAt( pos );
	}

	// Remove it from the display
	if( m_listAudiopath.GetSafeHwnd() )
	{
		int nIndex = m_listAudiopath.IndexFromFile( pAudiopath );
		if( nIndex != LB_ERR )
		{
			m_listAudiopath.DeleteString( nIndex );
		}
	}

	// Remove it from any segments that were referencing it
	pos = theApp.m_pXboxAddinComponent->m_lstPrimarySegments.GetHeadPosition();
	while( pos )
	{
		CSegment *pSegment = theApp.m_pXboxAddinComponent->m_lstPrimarySegments.GetNext( pos );
		if( pSegment->m_pAudiopath == pAudiopath )
		{
			pSegment->m_pAudiopath = NULL;
		}
	}

	// If it was the default audiopath, set another audiopath to be the default one
	if( pAudiopath->IsDefault()
	&&	m_plstAudiopaths->GetCount() )
	{
		if( m_listAudiopath.GetSafeHwnd() )
		{
			CAudiopath *pTmpAudiopath = (CAudiopath *)m_listAudiopath.GetItemDataPtr( 0 );
			if( pTmpAudiopath)
			{
				pTmpAudiopath->SetDefault( true );
				m_listAudiopath.Invalidate( TRUE );
			}
		}
		else
		{
			m_plstAudiopaths->GetHead()->SetDefault( true );
		}
	}

	// Delete it
	delete pAudiopath;
}

LRESULT CLeftPaneDlg::OnApp(WPARAM wParam, LPARAM lParam)
{
	// Command from list box right-click dialog
	switch( wParam )
	{
	case ID_SET_DEFAULT:
		SetDefault( (CAudiopath *)lParam );
		break;
	case ID_DELETE_AUDIOPATH:
		DeleteAudiopath( (CAudiopath *) lParam );
		break;
	case ID_CREATE_STANDARD3D:
		AddStandard( DMUS_APATH_DYNAMIC_3D );
		break;
	case ID_CREATE_STANDARDMIXBINQUAD:
		AddStandard( DMUS_APATH_MIXBIN_QUAD );
		break;
	case ID_CREATE_STANDARDMIXBINQUADMUSIC:
		AddStandard( DMUS_APATH_MIXBIN_QUAD_MUSIC );
		break;
	case ID_CREATE_STANDARDMIXBINQUADENV:
		AddStandard( DMUS_APATH_MIXBIN_QUAD_ENV );
		break;
	case ID_CREATE_STANDARDMIXBIN51:
		AddStandard( DMUS_APATH_MIXBIN_5DOT1 );
		break;
	case ID_CREATE_STANDARDMIXBIN51MUSIC:
		AddStandard( DMUS_APATH_MIXBIN_5DOT1_MUSIC );
		break;
	case ID_CREATE_STANDARDMIXBIN51ENV:
		AddStandard( DMUS_APATH_MIXBIN_5DOT1_ENV );
		break;
	case ID_CREATE_STANDARDSTEREOEFFECTS:
		AddStandard( DMUS_APATH_MIXBIN_STEREO_EFFECTS );
		break;
	case ID_CREATE_STANDARDSTEREO:
		AddStandard( DMUS_APATH_DYNAMIC_STEREO );
		break;
	case ID_CREATE_STANDARDSHAREDSTEREO:
		AddStandard( DMUS_APATH_SHARED_STEREO );
		break;
	case ID_CREATE_STANDARDMONO:
		AddStandard( DMUS_APATH_DYNAMIC_MONO );
		break;
	case ID_CREATE_STANDARDSTEREOREVERB:
		AddStandard( DMUS_APATH_SHARED_STEREOPLUSREVERB );
		break;
	}

	return 0;
}

void CLeftPaneDlg::AddAudiopathToList( CAudiopath *pAudiopath )
{
	// Check if audiopath name is already used
	int nAppendValue = 0;
	bool fFound = true;
	CString strAudiopathName = pAudiopath->GetFileName();
	while( fFound )
	{
		fFound = false;
		POSITION pos = m_plstAudiopaths->GetHeadPosition();
		while( pos )
		{
			CAudiopath *pTemp = m_plstAudiopaths->GetNext( pos );
			if( strAudiopathName == pTemp->GetFileName()
			&&	nAppendValue == pTemp->m_nAppendValue )
			{
				fFound = true;
				nAppendValue++;
				break;
			}
		}
	}

	// Set the audiopath's append value
	pAudiopath->SetAppendValue( nAppendValue );

	// Copy to Xbox
	HRESULT hr = pAudiopath->CopyToXbox( );

	if( SUCCEEDED(hr) )
	{
		// Add it to our list
		m_plstAudiopaths->AddHead( pAudiopath );

		// Add it to the display
		if( m_listAudiopath.GetSafeHwnd() )
		{
			m_listAudiopath.AddString( (LPCTSTR) pAudiopath );
		}

		// Check if this is the only audiopath
		if( m_plstAudiopaths->GetCount() == 1 )
		{
			// Set it as the default audiopath
			hr = pAudiopath->SetDefault( true );
		}
	}
	else
	{
		delete pAudiopath;
	}
}
