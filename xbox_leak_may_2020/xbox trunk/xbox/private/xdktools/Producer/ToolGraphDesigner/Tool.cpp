// Tool.cpp : implementation file
//

#include "stdafx.h"
#include "ToolGraphDesignerDLL.h"
#include "Tool.h"
#include "TabTool.h"
#include <RiffStrm.h>
#include <dmusicf.h>
#include <mmreg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// {FF085241-5314-4619-B4CA-11B101BE266A}
static const GUID GUID_ToolPropPageManager = 
{ 0xff085241, 0x5314, 0x4619, { 0xb4, 0xca, 0x11, 0xb1, 0x1, 0xbe, 0x26, 0x6a } };


//////////////////////////////////////////////////////////////////////
// CToolPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolPropPageManager::CToolPropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabTool = NULL;
}

CToolPropPageManager::~CToolPropPageManager()
{
	RELEASE( m_pIPropSheet );

	if( m_pTabTool )
	{
		delete m_pTabTool;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager::RemoveCurrentObject

void CToolPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager IUnknown implementation

HRESULT CToolPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageManager)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageManager *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CToolPropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CToolPropPageManager::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager IDMUSProdPropPageManager implementation


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CToolPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_TOOL_TEXT );

	CTool *pTool;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pTool))) )
	{
		if(  m_pTabTool == NULL
		|| !(m_pTabTool->GetBitsUI() & UD_MULTIPLESELECT) )
		{
			CString strTemp;
			if( pTool->GetGraph() )
			{
				pTool->GetGraph()->GetName( strTemp );
				strTemp += _T(" - ");
			}

			CLSID clsidTool;
			pTool->GetCLSID( &clsidTool );

			CString strToolName;
			theApp.m_pGraphComponent->GetRegisteredToolName( clsidTool, strToolName );

			strTitle = strTemp + pTool->m_strName  + _T(" ") + strToolName + _T(" ") + strTitle;
		}
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CToolPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	// Add Tool tab
	m_pTabTool = new CTabTool( this );
	if( m_pTabTool )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabTool->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CToolPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CToolPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}
	if( m_pIPropPageObject == pINewPropPageObject )
	{
		return S_OK;
	}

	RemoveCurrentObject();

	m_pIPropPageObject = pINewPropPageObject;
//	m_pIPropPageObject->AddRef();		intentionally missing

	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CToolPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pIPropPageObject == NULL)
	||  (pIPropPageObject != m_pIPropPageObject) )
	{
		return E_INVALIDARG;
	}

	RemoveCurrentObject();
	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CToolPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( pIPropPageObject == m_pIPropPageObject )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CToolPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CTool* pTool;
	
	if( m_pIPropPageObject == NULL )
	{
		pTool = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pTool ) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current Tool are processed in OnKillFocus
	// messages before setting the new Tool
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabTool->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new Tool
	m_pTabTool->SetTool( pTool );

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CToolPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CToolPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_ToolPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CTool constructor/destructor

CTool::CTool( void )
{
    m_dwRef = 0;
	AddRef();

	m_fSelected = FALSE;
	m_dwBitsUI = 0;

	m_pGraph = NULL;
	m_pIDMTool = NULL;
	m_pIStream = NULL;

	memset( &m_clsidTool, 0, sizeof(CLSID) );
}

CTool::CTool( CLSID clsidTool, IDirectMusicTool* pIDMTool )
{
    m_dwRef = 0;
	AddRef();

	m_pGraph = NULL;

	ASSERT( m_pIDMTool != NULL );
	m_pIDMTool = pIDMTool;
	m_pIDMTool->AddRef();

	m_pIStream = NULL;
	m_clsidTool = clsidTool;
}

CTool::~CTool()
{
	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	// Remove Tool from clipboard
	theApp.FlushClipboard( this );

	// Remove Tool from property sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}

	// Cleanup references
	RELEASE( m_pIDMTool );
	RELEASE( m_pIStream );
}


/////////////////////////////////////////////////////////////////////////////
// CTool::SetModified

void CTool::SetModified( BOOL fModified )
{
	ASSERT( m_pGraph != NULL );
	if( m_pGraph )
	{
		m_pGraph->SetModified( fModified );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTool::InsertPChannelGroup

void CTool::InsertPChannelGroup( CPChannelGroup* pPChannelGroupToInsert )
{
	if( pPChannelGroupToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	ASSERT( pPChannelGroupToInsert->m_dwNbrPChannels > 0 );

	CPChannelGroup* pPChannelGroup;
	POSITION posCurrent, posNext = m_lstPChannelGroups.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pPChannelGroup = m_lstPChannelGroups.GetNext( posNext );

		ASSERT( pPChannelGroup->m_dwNbrPChannels > 0 );

		if( *pPChannelGroup->m_pdwPChannel > *pPChannelGroupToInsert->m_pdwPChannel )
		{
			// PChannelGroup should point to Tool
			pPChannelGroupToInsert->InsertTool( m_pGraph, this );

			// Tool should point to PChannelGroup
			ASSERT( m_lstPChannelGroups.Find( pPChannelGroupToInsert ) == NULL );
			m_lstPChannelGroups.InsertBefore( posCurrent, pPChannelGroupToInsert );
			return;
		}
	}

	// PChannelGroup should point to Tool
	pPChannelGroupToInsert->InsertTool( m_pGraph, this );

	// Tool should point to PChannelGroup
	ASSERT( m_lstPChannelGroups.Find( pPChannelGroupToInsert ) == NULL );
	m_lstPChannelGroups.AddTail( pPChannelGroupToInsert );
}


/////////////////////////////////////////////////////////////////////////////
// CTool::RemovePChannelGroup

void CTool::RemovePChannelGroup( CPChannelGroup* pPChannelGroupToRemove )
{
	if( pPChannelGroupToRemove == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	AddRef();
	{
		// If tool is in pPChannelGroupToRemove->m_lstTools, remove it
		POSITION pos = pPChannelGroupToRemove->m_lstTools.Find( this );
		if( pos )
		{
			pPChannelGroupToRemove->m_lstTools.RemoveAt( pos );
			Release();
		}

		// If item is in m_lstPChannelGroups, remove it
		pos = m_lstPChannelGroups.Find( pPChannelGroupToRemove );
		if( pos )
		{
			m_lstPChannelGroups.RemoveAt( pos );
		}
	}
	Release();
}


/////////////////////////////////////////////////////////////////////////////
// CTool::RemoveAllPChannelGroups

void CTool::RemoveAllPChannelGroups( void )
{
	while( !m_lstPChannelGroups.IsEmpty() )
	{
		CPChannelGroup* pPChannelGroup = m_lstPChannelGroups.GetHead();

		RemovePChannelGroup( pPChannelGroup );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CompareDWORD()

static int __cdecl CompareDWORD( const void* arg1, const void* arg2 )
{
	if( *(DWORD**)arg1 < *(DWORD**)arg2 )
	{
		return -1;
	}
	if( *(DWORD**)arg1 > *(DWORD**)arg2 )
	{
		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::CombineAllPChannelGroups

void CTool::CombineAllPChannelGroups( void )
{
	// This method leaves a tool in invalid state
	// Must call InsertTool() immediately after
	//	to get everything back in working order

	// Build the PChannel array
	CDWordArray aPChannels;
	while( !m_lstPChannelGroups.IsEmpty() )
	{
		CPChannelGroup* pPChannelGroup = m_lstPChannelGroups.GetHead();

		RemovePChannelGroup( pPChannelGroup );
		for( DWORD i = 0 ;  i < pPChannelGroup->m_dwNbrPChannels ; i++ )
		{
			aPChannels.Add( pPChannelGroup->m_pdwPChannel[i] );
		}
	}

	// Sort the PChannel array
	qsort( aPChannels.GetData(), aPChannels.GetSize(), sizeof(DWORD), CompareDWORD );

	// Now create an orphan CPChannelGroup that InsertTool()
	//	will convert into valid pointers
	CPChannelGroup* pPChannelGroup = new CPChannelGroup();
	if( pPChannelGroup )
	{
		pPChannelGroup->m_dwNbrPChannels = aPChannels.GetSize();
		pPChannelGroup->m_pdwPChannel = new DWORD[aPChannels.GetSize()];
		if( pPChannelGroup->m_pdwPChannel )
		{
			memcpy( pPChannelGroup->m_pdwPChannel, aPChannels.GetData(), sizeof(DWORD) * pPChannelGroup->m_dwNbrPChannels);
		}

		// Don't call InsertPChannelGroup() because we do not want PChannelGroup to point to Tool
		ASSERT( m_lstPChannelGroups.IsEmpty() );
		m_lstPChannelGroups.AddTail( pPChannelGroup );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTool::RepositionPChannelGroup

void CTool::RepositionPChannelGroup( CPChannelGroup* pPChannelGroupToReposition )
{
	if( pPChannelGroupToReposition == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	ASSERT( pPChannelGroupToReposition->m_dwNbrPChannels > 0 );

	AddRef();
	{
		// If item is already in m_lstPChannelGroups, remove it
		POSITION pos = m_lstPChannelGroups.Find( pPChannelGroupToReposition );
		if( pos )
		{
			RemovePChannelGroup( pPChannelGroupToReposition );

			// Reinsert into the list in its new position
			InsertPChannelGroup( pPChannelGroupToReposition );
		}
	}
	Release();
}


/////////////////////////////////////////////////////////////////////////////
// CTool::ContainsPChannelGroup

bool CTool::ContainsPChannelGroup( CPChannelGroup* pPChannelGroup )
{
	POSITION pos = m_lstPChannelGroups.Find( pPChannelGroup );
	if( pos )
	{
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::GetColumnWidth

int CTool::GetColumnWidth( void )
{
	return m_ToolUI.nColumnWidth;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::SetColumnWidth

void CTool::SetColumnWidth( int nWidth )
{
	m_ToolUI.nColumnWidth = nWidth;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::GetName

void CTool::GetName( CString& strName )
{
	strName = m_strName;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::GetCLSID

void CTool::GetCLSID( CLSID* pclsidTool )
{
	*pclsidTool = m_clsidTool;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::GetBitsUI

DWORD CTool::GetBitsUI( void )
{
	return m_dwBitsUI;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::GetGraph

CDirectMusicGraph* CTool::GetGraph( void )
{
	return m_pGraph;
}


/////////////////////////////////////////////////////////////////////////////
// CTool IUnknown implementation

HRESULT CTool::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IUnknown) 
    ||  ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        *ppvObj = (IDMUSProdPropPageObject *)this;
    }

	else if( ::IsEqualIID(riid, IID_IDMUSProdUpdateObjectNow) )
	{
		*ppvObj = (IDMUSProdUpdateObjectNow *)this;
	}

	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CTool::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CTool::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp(); 
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CTool IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CTool IDMUSProdPropPageObject::GetData

HRESULT CTool::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	*ppData = this;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTool IDMUSProdPropPageObject::SetData

HRESULT CTool::SetData( void* pData )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CTool IDMUSProdPropPageObject::OnShowProperties

HRESULT CTool::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );
	ASSERT( theApp.m_pGraphComponent->m_pIFramework != NULL );

	// Get the Tool page manager
	CToolPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_ToolPropPageManager ) == S_OK )
	{
		pPageManager = (CToolPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CToolPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Tool properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pGraphComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;

			if( m_pGraph->IsMultipleSelectTools() )
			{
				m_dwBitsUI |= UD_MULTIPLESELECT;
			}
			if( pPageManager->IsEqualObject( this ) == S_OK )
			{
				pIPropSheet->RefreshTitleByObject( this );
				pIPropSheet->RefreshActivePageByObject( this );
			}
			else
			{
				pPageManager->SetObject( this );
			}
			m_dwBitsUI &= ~UD_MULTIPLESELECT;
		}

		pIPropSheet->Show( TRUE );
		RELEASE( pIPropSheet );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTool IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CTool::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTool IDMUSProdUpdateObjectNow implementation

/////////////////////////////////////////////////////////////////////////////
// CTool IDMUSProdUpdateObjectNow::UpdateObjectNow

HRESULT CTool::UpdateObjectNow( LPUNKNOWN punkObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate the object pointer
	if( punkObject == NULL )
	{
		return E_POINTER;
	}

	// Store the original stream of DirectMusic tool data
	IStream* pIOrigStream = m_pIStream;
	if( pIOrigStream )
	{
		pIOrigStream->AddRef();
	}

	// Get the "updated" stream
	RefreshStreamOfData();

	// Was anything changed?
	if( IsStreamOfDataEqual( pIOrigStream ) == false )
	{
		// Sync changes with DMusic DLLs
		m_pGraph->SyncGraphWithDirectMusic();
	}

	RELEASE( pIOrigStream );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::SaveHeader
    
HRESULT CTool::SaveHeader( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_TOOL_HEADER dmusToolIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Build the PChannel array
	CDWordArray aPChannels;
	POSITION pos = m_lstPChannelGroups.GetHeadPosition();
	while( pos )
	{
		CPChannelGroup* pPChannelGroup = m_lstPChannelGroups.GetNext( pos );

		for( DWORD i = 0 ;  i < pPChannelGroup->m_dwNbrPChannels ; i++ )
		{
			aPChannels.Add( pPChannelGroup->m_pdwPChannel[i] );
		}
	}

	// Sort the PChannel array
	qsort( aPChannels.GetData(), aPChannels.GetSize(), sizeof(DWORD), CompareDWORD );

	// Write Tool chunk header
	ck.ckid = DMUS_FOURCC_TOOL_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_TOOL_HEADER structure
	memset( &dmusToolIO, 0, sizeof(DMUS_IO_TOOL_HEADER) );

	dmusToolIO.guidClassID = m_clsidTool;
	dmusToolIO.lIndex = m_pGraph->ToolToIndex( this );
	ASSERT( dmusToolIO.lIndex !=  -1 );
	dmusToolIO.cPChannels = aPChannels.GetSize();
	dmusToolIO.ckid = 0;		// Will be filled in later
	dmusToolIO.fccType = 0;		// Will be filled in later
	if( dmusToolIO.cPChannels > 0 )
	{
		dmusToolIO.dwPChannels[0] = aPChannels[0];
	}
	else
	{
		// This Tool has no PChannels
		// Since count of zero means ALL PChannels
		//	we must force use of a bogus PChannel
		//	so that tool has no effect
		dmusToolIO.cPChannels = 1;
		dmusToolIO.dwPChannels[0] = TOOL_NO_PCHANNELS;
	}

	// Write Graph chunk data
	hr = pIStream->Write( &dmusToolIO, sizeof(DMUS_IO_TOOL_HEADER), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_TOOL_HEADER) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	// Write rest of PChannel array
	if( dmusToolIO.cPChannels > 1 )
	{
		// First element written as part of DMUS_IO_TOOL_HEADER struct
		aPChannels.RemoveAt( 0, 1 );

		hr = pIStream->Write( aPChannels.GetData(), (aPChannels.GetSize() * sizeof(DWORD)), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != (aPChannels.GetSize() * sizeof(DWORD)) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTool SaveUIState
    
HRESULT CTool::SaveUIState( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ckList;
    MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Structures for determining the stream type
	DMUSProdStreamInfo	StreamInfo;
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

	// Don't need to save unless this is a design stream
	if( ftFileType != FT_DESIGN )
	{
		hr = S_OK;
		goto ON_ERROR;
	}

	// Write DMUS_FOURCC_TOOL_UI_LIST header
	ckMain.fccType = DMUS_FOURCC_TOOL_UI_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save DMUS_FOURCC_TOOL_UI_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_TOOL_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write ToolUI data
		hr = pIStream->Write( &m_ToolUI, sizeof(ToolUI), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(ToolUI) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
		
		if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Save DMUS_FOURCC_UNFO_LIST list
	{
		ckList.fccType = DMUS_FOURCC_UNFO_LIST;
		if( pIRiffStream->CreateChunk(&ckList, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write Tool name
		{
			ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			hr = SaveMBStoWCS( pIStream, &m_strName );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}
		
		if( pIRiffStream->Ascend( &ckList, 0 ) != 0 )
		{
 			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::SaveTool

HRESULT CTool::SaveTool( IDMUSProdRIFFStream* pIRiffStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pGraph != NULL );

	DWORD dwHeaderFilePos;
	IPersistStream* pIPersistStream = NULL;
	FOURCC ckid_Tool = 0;
	FOURCC fccType_Tool = 0;

	if( pIRiffStream == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Structures for determining the stream type
	DMUSProdStreamInfo	StreamInfo;
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

	if( ::IsEqualGUID( guidDataFormat, GUID_SaveSelectedTools ) 
	&&  m_fSelected == FALSE )
	{
		// Nothing to do
		hr = S_OK;
		goto ON_ERROR;
	}

	// Write DMUS_FOURCC_TOOL_FORM header
	MMCKINFO ck;
	ck.fccType = DMUS_FOURCC_TOOL_FORM;
	if( pIRiffStream->CreateChunk( &ck, MMIO_CREATERIFF ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save Tool header chunk
    dwHeaderFilePos = StreamTell( pIStream );
	hr = SaveHeader( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save Tool UI
	hr = SaveUIState( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save Tool data
	ASSERT( m_pIDMTool != NULL );
	ASSERT( m_pIStream != NULL );
	if( m_pIDMTool 
	&&  m_pIStream )
	{
		DWORD dwByteCount;

		// Get m_pIStream's size
		STATSTG statstg;
		ULARGE_INTEGER uliSizeOut, uliSizeRead, uliSizeWritten;
		hr = m_pIStream->Stat( &statstg, STATFLAG_NONAME );
		if( FAILED( hr ) )
		{
			return E_FAIL;
		}
		uliSizeOut = statstg.cbSize;

		// Save data
		StreamSeek( m_pIStream, 0, STREAM_SEEK_SET );
		hr = m_pIStream->CopyTo( pIStream, uliSizeOut, &uliSizeRead, &uliSizeWritten );
		if( FAILED( hr )
		||  uliSizeRead.QuadPart != uliSizeOut.QuadPart
		||  uliSizeWritten.QuadPart != uliSizeOut.QuadPart )
		{
			goto ON_ERROR;
		}
//		hr = m_pIDMTool->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
//		if( FAILED ( hr ) )
//		{
//			goto ON_ERROR;
//		}
//		hr = pIPersistStream->Save( pIStream, FALSE );
//		if( FAILED ( hr ) )
//		{
//			goto ON_ERROR;
//		}

		// Get RIFF chunk ids
		StreamSeek( m_pIStream, 0, STREAM_SEEK_SET );
		hr = m_pIStream->Read( &ckid_Tool, sizeof(FOURCC), &dwByteCount );
		if( FAILED( hr )
		||  dwByteCount != sizeof(FOURCC) )
		{
			goto ON_ERROR;
		}
		if( ckid_Tool == FOURCC_RIFF
		||  ckid_Tool == FOURCC_LIST )
		{
			hr = m_pIStream->Read( &fccType_Tool, sizeof(FOURCC), &dwByteCount );
			if( FAILED( hr )
			||  dwByteCount != sizeof(FOURCC) )
			{
				goto ON_ERROR;
			}
		}
		else
		{
			fccType_Tool = 0;
		}
	}

	// Place RIFF chunk ids in Tool header
	if( ckid_Tool != 0
	||  fccType_Tool != 0 )
	{
		DWORD dwBytesWritten;

		DWORD dwCurFilePos = StreamTell( pIStream );
		StreamSeek( pIStream, dwHeaderFilePos + 32, STREAM_SEEK_SET );

		hr = pIStream->Write( &ckid_Tool, sizeof(FOURCC), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(FOURCC) )
		{
			goto ON_ERROR;
		}
		hr = pIStream->Write( &fccType_Tool, sizeof(FOURCC), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(FOURCC) )
		{
			goto ON_ERROR;
		}

		StreamSeek( pIStream, dwCurFilePos, STREAM_SEEK_SET );
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    RELEASE( pIPersistStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::LoadUIState

HRESULT CTool::LoadUIState( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*      pIStream;
	MMCKINFO	  ck;
	MMCKINFO	  ckList;
	DWORD		  dwByteCount;
	DWORD		  dwSize;
	DWORD		  dwPos;
    HRESULT       hr = S_OK;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_TOOL_UI_CHUNK:
				dwSize = min( ck.cksize, sizeof(ToolUI) );
				hr = pIStream->Read( &m_ToolUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_UNFO_LIST: 
					{
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );
									break;
							}
							pIRiffStream->Ascend( &ckList, 0 );
						}
					}
				}
				break;
		}

        pIRiffStream->Ascend( &ck, 0 );
	    dwPos = StreamTell( pIStream );
    }

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::LoadTool

HRESULT CTool::LoadTool( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IStream*	pIStream;
	MMCKINFO	ck;
	DWORD		dwByteCount;
	DWORD		dwSize;
	DWORD		dwCurrentFilePos;
    HRESULT     hr = S_OK;

	FOURCC		ckid_Tool = 0;
	FOURCC		fccType_Tool = 0;

	ASSERT( theApp.m_pGraphComponent != NULL );

	if( pIRiffStream == NULL 
	||  pckMain == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwCurrentFilePos = StreamTell( pIStream );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_TOOL_CHUNK:
			{
			    DMUS_IO_TOOL_HEADER dmusToolIO;

				dwSize = sizeof( DMUS_IO_TOOL_HEADER );
				hr = pIStream->Read( &dmusToolIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( dmusToolIO.cPChannels == 1
				&&  dmusToolIO.dwPChannels[0] == TOOL_NO_PCHANNELS )
				{
					// This Tool has no PChannels
					// Since count of zero means ALL PChannels
					//	we had to save a bogus PChannel
					//	so that tool has no effect
					dmusToolIO.cPChannels = 0;
					dmusToolIO.dwPChannels[0] = 0;
				}

				m_clsidTool = dmusToolIO.guidClassID;
				ckid_Tool = dmusToolIO.ckid;
				fccType_Tool = dmusToolIO.fccType;

				if( dmusToolIO.cPChannels )
				{
					CPChannelGroup* pPChannelGroup = new CPChannelGroup();
					if( pPChannelGroup == NULL )
					{
						hr = E_OUTOFMEMORY;
						goto ON_ERROR;
					}

					pPChannelGroup->m_dwNbrPChannels = dmusToolIO.cPChannels;
					pPChannelGroup->m_pdwPChannel = new DWORD[pPChannelGroup->m_dwNbrPChannels];
					if( pPChannelGroup->m_pdwPChannel )
					{
						pPChannelGroup->m_pdwPChannel[0] = dmusToolIO.dwPChannels[0];
						// subtract 1 from m_dwNbrPChannels, because 1 element is actually stored
						// in the dmusToolIO array.
						dwSize = (pPChannelGroup->m_dwNbrPChannels - 1) * sizeof(DWORD);
						if( dwSize )
						{
							hr = pIStream->Read( &pPChannelGroup->m_pdwPChannel[1], dwSize, &dwByteCount );
							if( FAILED( hr )
							||  dwByteCount != dwSize )
							{
								delete pPChannelGroup;
								hr = E_FAIL;
								goto ON_ERROR;
							}
						}
					}
					else
					{
						delete pPChannelGroup;
						hr = E_OUTOFMEMORY;
						goto ON_ERROR;
					}
					m_lstPChannelGroups.AddTail( pPChannelGroup );
				}
				break;
			}
        
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_TOOL_UI_LIST:
						hr = LoadUIState( pIRiffStream, &ck );
						if( FAILED ( hr ) )
						{
							goto ON_ERROR;
						}
						break;
				}
				break;

			default:
				if( ck.ckid == ckid_Tool
				&&  ck.fccType == fccType_Tool )
				{
					// CoCreate the DirectMusic tool
					if( SUCCEEDED ( ::CoCreateInstance( m_clsidTool, NULL, CLSCTX_INPROC_SERVER,
														IID_IDirectMusicTool, (void**)&m_pIDMTool ) ) )
					{
						// Load the DirectMusic tool
						IPersistStream* pIPersistStream;
						hr = m_pIDMTool->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
						if( FAILED ( hr ) )
						{
							goto ON_ERROR;
						}
						StreamSeek( pIStream, dwCurrentFilePos, STREAM_SEEK_SET );
						hr = pIPersistStream->Load( pIStream );
						if( FAILED ( hr ) )
						{
							goto ON_ERROR;
						}
						RELEASE( pIPersistStream );

						// Store a copy of the DirectMusic tool's data
						RefreshStreamOfData();
					}
					else
					{
						m_pIDMTool = NULL;
					}
				}
				break;

		}

        pIRiffStream->Ascend( &ck, 0 );
	    dwCurrentFilePos = StreamTell( pIStream );
    }

	// Bypass this tool if we can't CoCreate a DirectMusic object
	if( m_pIDMTool == NULL )
	{
		// PChannelGroups are not yet in Graph (must be deleted here)
		CPChannelGroup* pPChannelGroup;
		while( !m_lstPChannelGroups.IsEmpty() )
		{
			pPChannelGroup = m_lstPChannelGroups.RemoveHead();
			delete pPChannelGroup;
		}

		hr = S_FALSE;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTool::RefreshStreamOfData

HRESULT CTool::RefreshStreamOfData( void )
{
	HRESULT hr = S_OK;

	RELEASE( m_pIStream );

	ASSERT( m_pIDMTool != NULL );
	if( m_pIDMTool )
	{
		hr = theApp.m_pGraphComponent->m_pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_CurrentVersion, &m_pIStream );
		if( SUCCEEDED ( hr ) )
		{
			IPersistStream* pIPersistStream;
			hr = m_pIDMTool->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
			if( SUCCEEDED ( hr ) )
			{
				hr = pIPersistStream->Save( m_pIStream, FALSE );

				RELEASE( pIPersistStream );
			}
		}
	}

	return hr;
}


#define BUFFER_SIZE 256

/////////////////////////////////////////////////////////////////////////////
// CTool::IsStreamOfDataEqual

bool CTool::IsStreamOfDataEqual( IStream* pIStream )
{
	// Check if stream pointers are equal (including both NULL)
	if( m_pIStream == pIStream )
	{
		return true;
	}

	// Check if either stream pointer is NULL
	if( m_pIStream == NULL
	||	pIStream == NULL )
	{
		return false;
	}

	// Get the length of each stream
	STATSTG statStg1, statStg2;
	if( FAILED( m_pIStream->Stat( &statStg1, STATFLAG_NONAME ) )
	||	FAILED( pIStream->Stat( &statStg2, STATFLAG_NONAME ) )
	||	(statStg1.cbSize.QuadPart != statStg2.cbSize.QuadPart) )
	{
		// If either call to Stat failed, or the length are different, return false
		return false;
	}

	// Seek back to the start of each stream
	if( FAILED( StreamSeek( m_pIStream, 0, STREAM_SEEK_SET ) )
	||	FAILED( StreamSeek( pIStream, 0, STREAM_SEEK_SET ) ) )
	{
		// If either seek failed, return false
		return false;
	}

	// Try and allocate buffers to read data into
	BYTE *pbData1 = new BYTE[BUFFER_SIZE];
	if( pbData1 == NULL )
	{
		return false;
	}
	BYTE *pbData2 = new BYTE[BUFFER_SIZE];
	if( pbData2 == NULL )
	{
		delete []pbData1;
		return false;
	}

	// Initialize the amount of data to compare
	long lStreamSize = min( statStg1.cbSize.LowPart, LONG_MAX );

	while( lStreamSize > 0 )
	{
		// Try and read in a chunk of data
		ULONG cbRead1, cbRead2;
		const ULONG lReadSize = min( lStreamSize, BUFFER_SIZE );
		if( FAILED( m_pIStream->Read( pbData1, lReadSize, &cbRead1 ) )
		||	FAILED( pIStream->Read( pbData2, lReadSize, &cbRead2 ) )
		||	(lReadSize != cbRead1)
		||	(lReadSize != cbRead2) )
		{
			// If either read failed, or we didn't read in
			// the correct amount of data, return false
			break;
		}

		// Check if the data we read is identical
		if( memcmp( pbData1, pbData2, lReadSize ) != 0 )
		{
			// Nope
			break;
		}

		lStreamSize -= lReadSize;
	}

	delete []pbData1;
	delete []pbData2;

	// If we read all the data, return true.  Otherwise, return false
	return lStreamSize <= 0;
}
