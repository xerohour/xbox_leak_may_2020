// Style.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"

#include "Style.h"
#include "Pattern.h"
#include "Personality.h"
#include "PatternDlg.h"
#include "PatternCtl.h"
#include <mmreg.h>
#include <math.h>
#include <ioDMStyle.h>
#include "TabStyleStyle.h"
#include "TabStyleInfo.h"
#include "StyleCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// {06776460-C27E-11d0-8AC1-444553540000}
static const GUID GUID_StylePropPageManager = 
{ 0x6776460, 0xc27e, 0x11d0, { 0x8a, 0xc1, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };

// {16B7FC40-84F7-11d1-8AC1-444553540000}
static const GUID GUID_StyleUndo = 
{ 0x16b7fc40, 0x84f7, 0x11d1, { 0x8a, 0xc1, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };


short CStylePropPageManager::sm_nActiveTab = 0;


//////////////////////////////////////////////////////////////////////
// CStylePropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStylePropPageManager::CStylePropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabStyle = NULL;
	m_pTabInfo = NULL;
}

CStylePropPageManager::~CStylePropPageManager()
{
	RELEASE( m_pIPropSheet );

	if( m_pTabStyle )
	{
		delete m_pTabStyle;
	}

	if( m_pTabInfo )
	{
		delete m_pTabInfo;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStylePropPageManager::RemoveCurrentObject

void CStylePropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePropPageManager IUnknown implementation

HRESULT CStylePropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CStylePropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CStylePropPageManager::Release()
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
// CStylePropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CStylePropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CStylePropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_STYLE_TEXT );

	CDirectMusicStyle *pStyle;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pStyle))) )
	{
		CString strNodeName;
		BSTR bstrNodeName;

		if( SUCCEEDED ( pStyle->GetNodeName( &bstrNodeName ) ) )
		{
			strNodeName = bstrNodeName;
			::SysFreeString( bstrNodeName );
		}
		strTitle = strNodeName + _T(" ") + strTitle;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CStylePropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	// Add Style tab
	m_pTabStyle = new CTabStyleStyle( this );
	if( m_pTabStyle )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabStyle->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Info tab
	m_pTabInfo = new CTabStyleInfo( this );
	if( m_pTabInfo )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabInfo->m_psp );
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
// CStylePropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CStylePropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CStylePropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CStylePropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
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
// CStylePropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CStylePropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CStylePropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CStylePropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
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
// CStylePropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CStylePropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicStyle* pStyle;
	
	if( m_pIPropPageObject == NULL )
	{
		pStyle = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pStyle ) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current Style are processed in OnKillFocus
	// messages before setting the new Style
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabStyle->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new Style
	m_pTabStyle->SetStyle( pStyle);
	m_pTabInfo->SetStyle( pStyle);

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStylePropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CStylePropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_StylePropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle constructor/destructor

CDirectMusicStyle::CDirectMusicStyle()
{
	ASSERT( theApp.m_pStyleComponent != NULL );

	// Style needs Style Component
	theApp.m_pStyleComponent->AddRef();

    m_dwRef = 0;
	AddRef();

	m_pUndoMgr = NULL;
	m_fModified = FALSE;

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pIDMStyle = NULL;

	m_hWndEditor = NULL;
	m_pStyleCtrl = NULL;
	m_pINodeBeingDeleted = NULL;
	m_pIProject = NULL;

	m_pIActiveBandNode = NULL;

	m_dwNextPatternID = 0;
	m_fPChannelChange = FALSE;

	m_StyleBands.m_pStyle = this;
	m_StylePatterns.m_pStyle = this;
	m_StyleMotifs.m_pStyle = this;
	m_StylePersonalities.m_pStyle = this;

// Style UI fields
	m_nSplitterYPos = 0;
	m_dwNotationType = 0;

// Style data for Music Engine
	TCHAR achName[SMALL_BUFFER];

	::LoadString( theApp.m_hInstance, IDS_STYLE_TEXT, achName, SMALL_BUFFER );
    m_strName.Format( "%s%d", achName, ++theApp.m_pStyleComponent->m_nNextStyle );

	CoCreateGuid( &m_guidStyle ); 

	m_dwVersionMS = 0x00010000;
	m_dwVersionLS = 0x00000000;

	m_TimeSignature.m_bBeatsPerMeasure = 4;
	m_TimeSignature.m_bBeat = 4;
	m_TimeSignature.m_wGridsPerBeat = 4;

	m_dblTempo = 120;

	m_wUseId = 0x003F;
}

CDirectMusicStyle::~CDirectMusicStyle()
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// Remove Style from clipboard
	theApp.FlushClipboard( this );

	// Remove Style from property sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}

	// Cleanup the Undo Manager
	if( m_pUndoMgr )
	{
		delete m_pUndoMgr;
	}

	// Cleanup DirectMusic references
	RELEASE( m_pIDMStyle );

	// Cleanup active Band
	RELEASE( m_pIActiveBandNode );

	// m_lstStyleParts gets freed as Patterns
	// delete their Part References

	// Style no longer needs Style Component
	theApp.m_pStyleComponent->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::CreateUndoMgr

BOOL CDirectMusicStyle::CreateUndoMgr()
{
	// Should only be called once - after Style first created
	ASSERT( m_pUndoMgr == NULL );

	ASSERT( theApp.m_pStyleComponent != NULL ); 
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL ); 

	m_pUndoMgr = new CJazzUndoMan( theApp.m_pStyleComponent->m_pIFramework );
	if( m_pUndoMgr )
	{
		m_pUndoMgr->SetStreamAttributes( FT_DESIGN, GUID_StyleUndo );
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::Initialize

HRESULT CDirectMusicStyle::Initialize( void )
{
	HRESULT hr;
	IDMUSProdNode* pINode;
	IDMUSProdDocType* pIDocType;

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// This method should only be called immedialtely
	// after construction to create a default Style
	ASSERT( m_pIDocRootNode == NULL );

	// Set root and parent node of ALL children
	// Must do this now to prevent ASSERT's
//	theApp.SetNodePointers( this, this, NULL );
		
	// Create the Undo Manager
	if( CreateUndoMgr() == FALSE )
	{
		return E_OUTOFMEMORY;
	}

	// Create a default Band
	hr = theApp.m_pStyleComponent->m_pIFramework->FindDocTypeByNodeId( GUID_BandNode, &pIDocType );
	if( SUCCEEDED ( hr ) ) 
	{
		hr = pIDocType->AllocNode( GUID_BandNode, &pINode );
		if( SUCCEEDED ( hr ) )
		{
			IDMUSProdBandEdit* pIBandEdit;

			// Set "Default" flag
			if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdBandEdit, (void**)&pIBandEdit ) ) )
			{
				pIBandEdit->SetDefaultFlag( TRUE );
				RELEASE( pIBandEdit );
			}

			// Make sure Band name is unique
			GetUniqueBandName( pINode );

			// Add to CDirectMusicStyle Band list
			m_StyleBands.m_lstBands.AddTail( pINode );
		}

		RELEASE( pIDocType );
	}
	if( FAILED ( hr ) )
	{
		return hr;
	}

	// Create a default Pattern
	CDirectMusicPattern* pPattern = new CDirectMusicPattern( this, FALSE );
	if( pPattern == NULL )
	{
		hr = E_OUTOFMEMORY ;
	}
	if( SUCCEEDED ( hr ) )
	{
		hr = pPattern->Initialize();
		if( SUCCEEDED ( hr ) )
		{
			// Add to CDirectMusicStyle Pattern list
			m_StylePatterns.m_lstPatterns.AddTail( pPattern );
		}
	}
	if( FAILED ( hr ) )
	{
		RELEASE( pPattern );
		return hr;
	}

	// Set root and parent node of ALL children
	// Must do this again because we added more nodes
	theApp.SetNodePointers( this, this, NULL );

	//	Allocate a new DirectMusic Style and Persist it to the DirectMusic DLLs.
	ASSERT( m_pIDMStyle == NULL );

	hr = ::CoCreateInstance( CLSID_DirectMusicStyle, NULL, CLSCTX_INPROC_SERVER,
							 IID_IDirectMusicStyle, (void**)&m_pIDMStyle );
	if( SUCCEEDED (  hr ) )
	{
		hr = SyncStyleWithDirectMusic();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SyncStyleEditor

void CDirectMusicStyle::SyncStyleEditor( DWORD dwFlags )
{
	if( m_pStyleCtrl
	&&  m_pStyleCtrl->m_pStyleDlg )
	{
		m_pStyleCtrl->m_pStyleDlg->RefreshControls( dwFlags );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::ForceSyncStyleWithDirectMusic

HRESULT CDirectMusicStyle::ForceSyncStyleWithDirectMusic( void )
{
	CWaitCursor wait;
	IStream* pIMemStream;
	IPersistStream* pIPersistStream;
	HRESULT hr;

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( m_pIDMStyle == NULL )
	{
		// Nothing to do
		return S_OK;
	}

	hr = theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );

	if( SUCCEEDED ( hr ) )
	{
		hr = Save( pIMemStream, FALSE );
		if( SUCCEEDED ( hr ) )
		{
			m_pIDMStyle->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
			if( pIPersistStream )
			{
				StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
				hr = pIPersistStream->Load( pIMemStream );

				RELEASE( pIPersistStream );
			}
		}

		RELEASE( pIMemStream );
	}

	// Check to see if any of the Style PChannels have changed
	if( m_fPChannelChange )
	{
		m_fPChannelChange = FALSE;

		// Notify connected nodes that a PChannel has changed
		theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( this, STYLE_PChannelChange, NULL );
	}

	// Check to see if any of the Motifs have changed
	POSITION pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
	{
		CDirectMusicPattern* pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );
	
		if( pMotif->m_fSendChangeNotification )
		{
			pMotif->SendChangeNotification();
			pMotif->m_fSendChangeNotification = FALSE;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SyncStyleWithDirectMusic

HRESULT CDirectMusicStyle::SyncStyleWithDirectMusic( void )
{
	HRESULT hr;

	IDirectMusicSegment* pIDMSegment = GetSegmentThatIsPlaying();
	if( pIDMSegment )
	{
		// No need to sync if one of the Style's Patterns is playing.
		// Patterns do not play Style objects.
		// CDirectMusicPattern::Stop will perform the sync when the
		// Pattern is finished playing.
		RELEASE( pIDMSegment );
		hr = S_OK;
	}
	else
	{
		hr =  ForceSyncStyleWithDirectMusic();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SyncMidiStripMgrsThatUsePart

void CDirectMusicStyle::SyncMidiStripMgrsThatUsePart( CDirectMusicPart* pDMPart )
{
	CDirectMusicPattern* pPatternList;
	CDirectMusicPartRef* pDMPartRef;
	POSITION pos;
	POSITION pos2;

	ASSERT( pDMPart != NULL );

	// Handle Motifs
	pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
	{
		pPatternList = m_StyleMotifs.m_lstMotifs.GetNext( pos );
	
		pos2 = pPatternList->m_lstPartRefs.GetHeadPosition();
		while( pos2 )
		{
			pDMPartRef = pPatternList->m_lstPartRefs.GetNext( pos2 );

			if( pDMPartRef->m_pDMPart == pDMPart )
			{
				pPatternList->SyncPatternWithMidiStripMgr();
				break;
			}
		}
	}
		
	// Handle Patterns
	pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( pos )
	{
		pPatternList = m_StylePatterns.m_lstPatterns.GetNext( pos );
	
		pos2 = pPatternList->m_lstPartRefs.GetHeadPosition();
		while( pos2 )
		{
			pDMPartRef = pPatternList->m_lstPartRefs.GetNext( pos2 );

			if( pDMPartRef->m_pDMPart == pDMPart )
			{
				pPatternList->SyncPatternWithMidiStripMgr();
				break;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::GetBandNodeByName

IDMUSProdNode* CDirectMusicStyle::GetBandNodeByName( LPCTSTR szBandName )
{
	IDMUSProdNode* pIBandNodeList;
	CString strBandName;
	BSTR bstrBandName;

	POSITION pos = m_StyleBands.m_lstBands.GetHeadPosition();
	while( pos )
	{
		pIBandNodeList = m_StyleBands.m_lstBands.GetNext( pos );

		if( SUCCEEDED ( pIBandNodeList->GetNodeName( &bstrBandName ) ) )
		{
			strBandName = bstrBandName;
			::SysFreeString( bstrBandName );

			if( strBandName.CompareNoCase( szBandName ) == 0 )
			{
				pIBandNodeList->AddRef();
				return pIBandNodeList;
			}
		}
	}

	return NULL; 
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::IsBandNameUnique

BOOL CDirectMusicStyle::IsBandNameUnique( IDMUSProdNode* pIBandNode, LPCTSTR szBandName )
{
	IDMUSProdNode* pIBandNodeList;
	CString strBandName;
	BSTR bstrBandName;

	POSITION pos = m_StyleBands.m_lstBands.GetHeadPosition();
	while( pos )
	{
		pIBandNodeList = m_StyleBands.m_lstBands.GetNext( pos );
		if( pIBandNodeList != pIBandNode )
		{
			if( SUCCEEDED ( pIBandNodeList->GetNodeName( &bstrBandName ) ) )
			{
				strBandName = bstrBandName;
				::SysFreeString( bstrBandName );

				if( strBandName.CompareNoCase( szBandName ) == 0 )
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::GetUniqueBandName

void CDirectMusicStyle::GetUniqueBandName( IDMUSProdNode* pIBandNode )
{
	CString	strOrigBandName;
	CString strBandName;
	BSTR bstrBandName;
	CString	strName;
	CString	strNbr;
	short nMaxLength;
	int	nNameLength;
	int	nNbrLength;
	int	i;

	if( FAILED ( pIBandNode->GetNodeName( &bstrBandName ) ) )
	{
		return;
	}

	strOrigBandName = bstrBandName;
	strName = strOrigBandName;
	::SysFreeString( bstrBandName );

	i = 0;
	while( !strName.IsEmpty()  &&  _istdigit(strName[strName.GetLength() - 1]) )
	{
		strNbr = strName.Right(1) + strNbr;
		strName = strName.Left( strName.GetLength() - 1 );
		if( ++i > 6 )
		{
			break;
		}
	}

	pIBandNode->GetNodeNameMaxLength( &nMaxLength );
	nNameLength = strName.GetLength();
	i = _ttoi( strNbr );

	strBandName = strOrigBandName;

	while( IsBandNameUnique( pIBandNode, strBandName ) == FALSE )
	{
		strNbr.Format( "%d", ++i ); 
		nNbrLength = strNbr.GetLength();
		
		if( (nNameLength + nNbrLength) <= nMaxLength )
		{
			strBandName = strName + strNbr;
		}
		else
		{
			strBandName = strName.Left(nMaxLength - nNbrLength) + strNbr;
		}
	}

	if( strBandName.CompareNoCase( strOrigBandName ) != 0 )
	{
		bstrBandName = strBandName.AllocSysString();
		pIBandNode->SetNodeName( bstrBandName );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::IsPatternNameUnique

BOOL CDirectMusicStyle::IsPatternNameUnique( CDirectMusicPattern* pPattern )
{
	CDirectMusicPattern* pPatternList;
	POSITION pos;

	if( pPattern->m_wEmbellishment & EMB_MOTIF )
	{
		pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
		while( pos )
		{
			pPatternList = m_StyleMotifs.m_lstMotifs.GetNext( pos );
			if( pPatternList != pPattern )
			{
				if( pPatternList->m_strName.CompareNoCase( pPattern->m_strName ) == 0 )
				{
					return FALSE;
				}
			}
		}
	}
	else
	{
		pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
		while( pos )
		{
			pPatternList = m_StylePatterns.m_lstPatterns.GetNext( pos );
			if( pPatternList != pPattern )
			{
				if( pPatternList->m_strName.CompareNoCase( pPattern->m_strName ) == 0 )
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::GetUniquePatternName

void CDirectMusicStyle::GetUniquePatternName( CDirectMusicPattern* pPattern )
{
	CString	strOrigName;
	CString	strNbr;
	int	nOrigNameLength;
	int	nNbrLength;
	int	i;

	if( pPattern->m_strName.GetLength() > DMUS_MAX_NAME )
	{
		pPattern->m_strName = pPattern->m_strName.Left( DMUS_MAX_NAME );
		pPattern->m_strName.TrimRight();
	}

	strOrigName = pPattern->m_strName;

	i = 0;
	while( !strOrigName.IsEmpty()  &&  _istdigit(strOrigName[strOrigName.GetLength() - 1]) )
	{
		strNbr = strOrigName.Right(1) + strNbr;
		strOrigName = strOrigName.Left( strOrigName.GetLength() - 1 );
		if( ++i > 6 )
		{
			break;
		}
	}

	nOrigNameLength = strOrigName.GetLength();
	i = _ttoi( strNbr );

	while( IsPatternNameUnique( pPattern ) == FALSE )
	{
		strNbr.Format( "%d", ++i ); 
		nNbrLength = strNbr.GetLength();
		
		if( (nOrigNameLength + nNbrLength) <= DMUS_MAX_NAME )
		{
			pPattern->m_strName = strOrigName + strNbr;
		}
		else
		{
			pPattern->m_strName = strOrigName.Left(DMUS_MAX_NAME - nNbrLength) + strNbr;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SetModified

void CDirectMusicStyle::SetModified( BOOL fModified )
{
	m_fModified = fModified;

	if( m_fModified == FALSE )
	{
		m_StyleBands.SetModified( FALSE );
		m_StyleMotifs.SetModified( FALSE );
		m_StylePatterns.SetModified( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IUnknown implementation

HRESULT CDirectMusicStyle::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IDMUSProdNode *)this;
    }
    else if( ::IsEqualIID(riid, IID_IPersist) )
    {
        *ppvObj = (IPersist *)this;
    }
    else if( ::IsEqualIID(riid, IID_IPersistStream) )
    {
        *ppvObj = (IPersistStream *)this;
    }
    else if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        *ppvObj = (IDMUSProdPropPageObject *)this;
    }
	else if( ::IsEqualIID(riid, IID_IDMUSProdNotifySink) )
	{
		*ppvObj = (IDMUSProdNotifySink*) this;
	}
	else if( ::IsEqualIID(riid, IID_IDMUSProdStyleInfo) )
	{
		*ppvObj = (IDMUSProdStyleInfo*) this;
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CDirectMusicStyle::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CDirectMusicStyle::Release()
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
// CDirectMusicStyle IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetNodeImageIndex

HRESULT CDirectMusicStyle::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return( theApp.m_pStyleComponent->GetStyleImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetFirstChild

HRESULT CDirectMusicStyle::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	m_StyleBands.AddRef();
	*ppIFirstChildNode = (IDMUSProdNode *)&m_StyleBands;

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetNextChild

HRESULT CDirectMusicStyle::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINextChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppINextChildNode = NULL;

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	if( pIChildNode == (IDMUSProdNode *)&m_StyleBands )
	{
		m_StylePatterns.AddRef();
		*ppINextChildNode = (IDMUSProdNode *)&m_StylePatterns;
	}
	else if( pIChildNode == (IDMUSProdNode *)&m_StylePatterns )
	{
		m_StyleMotifs.AddRef();
		*ppINextChildNode = (IDMUSProdNode *)&m_StyleMotifs;
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetComponent

HRESULT CDirectMusicStyle::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return theApp.m_pStyleComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetDocRootNode

HRESULT CDirectMusicStyle::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode )
	{
		m_pIDocRootNode->AddRef();
		*ppIDocRootNode = m_pIDocRootNode;
		return S_OK;
	}

	*ppIDocRootNode = NULL;
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::SetDocRootNode

HRESULT CDirectMusicStyle::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetParentNode

HRESULT CDirectMusicStyle::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIParentNode == NULL )
	{
		return E_POINTER;
	}

	*ppIParentNode = m_pIParentNode;

	if( m_pIParentNode )
	{
		m_pIParentNode->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::SetParentNode

HRESULT CDirectMusicStyle::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetNodeId

HRESULT CDirectMusicStyle::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_StyleNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetNodeName

HRESULT CDirectMusicStyle::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetNodeNameMaxLength

HRESULT CDirectMusicStyle::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = DMUS_MAX_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::ValidateNodeName

HRESULT CDirectMusicStyle::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;

	strName = bstrName;
	::SysFreeString( bstrName );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::SetNodeName

HRESULT CDirectMusicStyle::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( m_strName == strName )
	{
		return S_OK;
	}

	m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_STYLE_NAME );
	m_strName = strName;

	// Sync change with property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		RELEASE( pIPropSheet );
	}
	
	SetModified( TRUE );
	SyncStyleWithDirectMusic();

	// Notify connected nodes that Style name has changed
	theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( this, STYLE_NameChange, NULL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetNodeListInfo

HRESULT CDirectMusicStyle::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strDescriptor;

	strDescriptor.Format( "%d/%d", m_TimeSignature.m_bBeatsPerMeasure, m_TimeSignature.m_bBeat );

	if( !(m_TimeSignature.m_wGridsPerBeat % 3) )
	{
		strDescriptor += _T("  3");
	}

    pListInfo->bstrName = m_strName.AllocSysString();
    pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
	memcpy( &pListInfo->guidObject, &m_guidStyle, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetEditorClsId

HRESULT CDirectMusicStyle::GetEditorClsId( CLSID* pClsId )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

   *pClsId = CLSID_StyleEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetEditorTitle

HRESULT CDirectMusicStyle::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	::LoadString( theApp.m_hInstance, IDS_STYLE_TEXT, achTitle, MID_BUFFER );
	strTitle  = achTitle;
	strTitle += _T(": " );
	strTitle += m_strName;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetEditorWindow

HRESULT CDirectMusicStyle::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::SetEditorWindow

HRESULT CDirectMusicStyle::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::UseOpenCloseImages

HRESULT CDirectMusicStyle::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetRightClickMenuId

HRESULT CDirectMusicStyle::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_STYLE_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::OnRightClickMenuInit

HRESULT CDirectMusicStyle::OnRightClickMenuInit( HMENU hMenu )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::OnRightClickMenuSelect

HRESULT CDirectMusicStyle::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	IDMUSProdNode* pINode;

	switch( lCommandId )
	{
		case IDM_NEW_BAND:
			pINode = (IDMUSProdNode *)&m_StyleBands;
			if( SUCCEEDED ( pINode->InsertChildNode(NULL) ) )
			{
				hr = S_OK;
			}
			break;

		case IDM_NEW_MOTIF:
			hr = m_StyleMotifs.CreateNewMotif();
			break;

		case IDM_NEW_PATTERN:
			hr = m_StylePatterns.CreateNewPattern();
			break;

		case IDM_PATTERN_FROMMIDI:
			ImportMidiFileAsPattern( FALSE );
			break;

		case IDM_MOTIF_FROMMIDI:
			ImportMidiFileAsPattern( TRUE );
			break;
	
		case IDM_RENAME:
			if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->EditNodeLabel((IDMUSProdNode *)this) ) )
			{
				hr = S_OK;
			}
			break;

		case IDM_PROPERTIES:
			OnShowProperties();
			hr = S_OK;
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::DeleteChildNode

HRESULT CDirectMusicStyle::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// Cannot delete folders under style
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::InsertChildNode

HRESULT CDirectMusicStyle::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// Cannot insert additional folders under style
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::DeleteNode

HRESULT CDirectMusicStyle::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;

	// No parent so we will delete ourself
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// Remove from Project Tree
	if( theApp.m_pStyleComponent->m_pIFramework->RemoveNode( this, fPromptUser ) == S_FALSE )
	{
		// User cancelled delete
		return E_FAIL;
	}

	// Style no longer needs Transport Controls
	UnRegisterWithTransport();

	// Make sure that Bands cleanup properly.  Otherwisea 
	// Band that happens to be in the Framework's notification
	// list will never have its destructor called.
	m_StyleBands.CleanUp();

	// Remove from Component Style list
	theApp.m_pStyleComponent->RemoveFromStyleFileList( this );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::OnNodeSelChanged

HRESULT CDirectMusicStyle::OnNodeSelChanged( BOOL fSelected )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::CreateDataObject

HRESULT CDirectMusicStyle::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

	// Save Style into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_STYLE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pStyleComponent->m_cfStyle, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->SaveClipFormat( theApp.m_pStyleComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// Style nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pStyleComponent->m_cfProducerFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			RELEASE( pIStream );
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		RELEASE( pDataObject );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::CanCut

HRESULT CDirectMusicStyle::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::CanCopy

HRESULT CDirectMusicStyle::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::CanDelete

HRESULT CDirectMusicStyle::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::CanDeleteChildNode

HRESULT CDirectMusicStyle::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Cannot delete folders under Style
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle ContainsClipFormat

HRESULT CDirectMusicStyle::ContainsClipFormat( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject, UINT cfClipFormat )
{
	if( pDataObject == NULL 
	||  pIDataObject == NULL 
	||  cfClipFormat == 0 )
	{
		return E_POINTER;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfTimeline ) ) )
	{
		IStream* pIStream;

		if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfTimeline, &pIStream ) ) )
		{
			// Create a RIFF stream
			IDMUSProdRIFFStream* pIRiffStream;
			if( SUCCEEDED ( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
			{
				MMCKINFO ck1;
				MMCKINFO ck2;

				ck1.fccType = FOURCC_TIMELINE_LIST;
				if( pIRiffStream->Descend( &ck1, NULL, MMIO_FINDLIST ) == 0 )
				{
					ck2.fccType = FOURCC_TIMELINE_CLIPBOARD;
					while( pIRiffStream->Descend( &ck2, &ck1, MMIO_FINDLIST ) == 0 )
					{
						ck2.ckid = FOURCC_TIMELINE_CLIP_NAME;
						if( pIRiffStream->Descend( &ck2, &ck1, MMIO_FINDCHUNK ) == 0 )
						{
							DWORD dwSize;
							DWORD dwBytesRead;
							TCHAR achText[MAX_PATH];

							memset( achText, 0, sizeof( achText ) );

							dwSize = min( ck2.cksize, MAX_PATH );
							if( SUCCEEDED ( pIStream->Read( achText, dwSize, &dwBytesRead ) ) 
							&&  dwBytesRead == dwSize )
							{
								if( ::RegisterClipboardFormat(achText) == cfClipFormat )
								{
									// We found what we are looking for - CF_BANDTRACK format
									hr = S_OK;
									break;
								}
							}
						}

						pIRiffStream->Ascend( &ck2, 0 );
					}
				}

				RELEASE( pIRiffStream );
			}

			RELEASE( pIStream );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::PasteCF_BAND

HRESULT CDirectMusicStyle::PasteCF_BAND( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject,
										 IDMUSProdNode* pIPositionNode )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfBand, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new Band
		IDMUSProdRIFFExt* pIRIFFExt;
		IDMUSProdNode* pINode;

		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIBandComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
		{
			hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
			if( SUCCEEDED ( hr ) )
			{
				if( pIPositionNode )
				{
					m_StyleBands.InsertChildNodeAtPos( pINode, pIPositionNode );
				}
				else
				{
					m_StyleBands.InsertChildNode( pINode );
				}

				RELEASE( pINode );
			}

			RELEASE( pIRIFFExt );
		}

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::PasteCF_BANDLIST

HRESULT CDirectMusicStyle::PasteCF_BANDLIST( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject,
											 IDMUSProdNode* pIPositionNode )
{
	UNREFERENCED_PARAMETER(pIPositionNode);
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfBandList, &pIStream  ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::LoadCF_BANDTRACK

HRESULT CDirectMusicStyle::LoadCF_BANDTRACK( IStream* pIStream, IDMUSProdNode* pIPositionNode )
{
	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	HRESULT hr = E_FAIL;

	// Create a RIFF stream
	IDMUSProdRIFFStream* pIRiffStream;
	if( SUCCEEDED ( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		MMCKINFO ckMain;
		MMCKINFO ckList;

		// Look for the list of Bands in the RIFF stream
		ckMain.fccType = DMUS_FOURCC_BANDS_LIST;
		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) == 0 )
		{
			while( pIRiffStream->Descend( &ckList, &ckMain, 0 ) == 0 )
			{
				switch( ckList.ckid )
				{
					case FOURCC_LIST :
						switch( ckList.fccType )
						{
							case DMUS_FOURCC_BAND_LIST:
							{
								MMCKINFO ck;

								// Extract individual Bands and add them to the Style's list of Bands
								while( pIRiffStream->Descend( &ck, &ckList, 0 ) == 0 )
								{
									switch( ck.ckid )
									{
										case FOURCC_RIFF:
											switch( ck.fccType )
											{
												case DMUS_FOURCC_BAND_FORM:
												{
													StreamSeek( pIStream, -12, STREAM_SEEK_CUR );

													// Create and load a new Band
													IDMUSProdRIFFExt* pIRIFFExt;
													IDMUSProdNode* pINode;

													if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIBandComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
													{
														if( SUCCEEDED ( pIRIFFExt->LoadRIFFChunk( pIStream, &pINode ) ) )
														{
															// We were able to paste something so return S_OK
															hr = S_OK;

															if( pIPositionNode )
															{
																m_StyleBands.InsertChildNodeAtPos( pINode, pIPositionNode );
															}
															else
															{
																m_StyleBands.InsertChildNode( pINode );
															}

															RELEASE( pINode );
														}

														RELEASE( pIRIFFExt );
													}
													break;
												}
											}
									}

									pIRiffStream->Ascend( &ck, 0 );
								}
								break;
							}
						}
						break;
				}

				pIRiffStream->Ascend( &ckList, 0 );
			}
		}

		RELEASE( pIRiffStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::PasteCF_BANDTRACK

HRESULT CDirectMusicStyle::PasteCF_BANDTRACK( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject,
											  IDMUSProdNode* pIPositionNode )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		hr = LoadCF_BANDTRACK( pIStream, pIPositionNode );

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::PasteCF_TIMELINE

HRESULT CDirectMusicStyle::PasteCF_TIMELINE( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject,
											 IDMUSProdNode* pIPositionNode )
{
	if( pDataObject == NULL 
	||  pIDataObject == NULL ) 
	{
		return E_POINTER;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfTimeline ) ) )
	{
		IStream* pIStream;

		if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfTimeline, &pIStream ) ) )
		{
			// Create a RIFF stream
			IDMUSProdRIFFStream* pIRiffStream;
			if( SUCCEEDED ( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
			{
				MMCKINFO ck1;
				MMCKINFO ck2;

				ck1.fccType = FOURCC_TIMELINE_LIST;
				if( pIRiffStream->Descend( &ck1, NULL, MMIO_FINDLIST ) == 0 )
				{
					ck2.fccType = FOURCC_TIMELINE_CLIPBOARD;
					while( pIRiffStream->Descend( &ck2, &ck1, MMIO_FINDLIST ) == 0 )
					{
						ck2.ckid = FOURCC_TIMELINE_CLIP_NAME;
						if( pIRiffStream->Descend( &ck2, &ck1, MMIO_FINDCHUNK ) == 0 )
						{
							DWORD dwSize;
							DWORD dwBytesRead;
							TCHAR achText[MAX_PATH];

							memset( achText, 0, sizeof( achText ) );

							dwSize = min( ck2.cksize, MAX_PATH );
							if( SUCCEEDED ( pIStream->Read( achText, dwSize, &dwBytesRead ) ) 
							&&  dwBytesRead == dwSize )
							{
								if( ::RegisterClipboardFormat(achText) == theApp.m_pStyleComponent->m_cfBandTrack )
								{
									pIRiffStream->Ascend( &ck2, 0 );

									// Handle CF_BANDTRACK format
									ck2.ckid = FOURCC_TIMELINE_CLIP_DATA;
									if( pIRiffStream->Descend( &ck2, &ck1, MMIO_FINDCHUNK ) == 0 )
									{
										hr = LoadCF_BANDTRACK( pIStream, pIPositionNode );
									}
									break;
								}
							}
						}

						pIRiffStream->Ascend( &ck2, 0 );
					}
				}

				RELEASE( pIRiffStream );
			}

			RELEASE( pIStream );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::PasteCF_MOTIF

HRESULT CDirectMusicStyle::PasteCF_MOTIF( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject,
										  CDirectMusicPattern* pPositionMotif )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfMotif, &pIStream  ) ) 
	||  SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfPattern, &pIStream  ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new Motif
		CDirectMusicPattern* pMotif = new CDirectMusicPattern( this, TRUE );

		if( pMotif )
		{
			hr = pMotif->Load( pIStream );
			if( SUCCEEDED( hr ) )
			{
				if( !(pMotif->m_wEmbellishment & EMB_MOTIF) )
				{
					// Convert the Pattern into a Motif
					pMotif->m_wEmbellishment = EMB_MOTIF;
					pMotif->m_bGrooveBottom = 1; 
					pMotif->m_bGrooveTop = 100;
					pMotif->m_bDestGrooveBottom = 1; 
					pMotif->m_bDestGrooveTop = 100;
					ASSERT( pMotif->m_pIBandNode == NULL );
				}
				if( pPositionMotif )
				{
					m_StyleMotifs.InsertChildNodeAtPos( pMotif, pPositionMotif );
				}
				else
				{
					m_StyleMotifs.InsertChildNode( pMotif );
				}
			}

			RELEASE( pMotif );
		}
		else
		{
			hr = E_OUTOFMEMORY ;
		}

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::PasteCF_MOTIFLIST

HRESULT CDirectMusicStyle::PasteCF_MOTIFLIST( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject,
											  CDirectMusicPattern* pPositionMotif )
{
	UNREFERENCED_PARAMETER(pPositionMotif);
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList, &pIStream  ) ) 
	||  SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList, &pIStream  ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		RELEASE( pIStream );

		// Remember to convert Patterns into Motifs when this method is actually implemented
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::PasteCF_PATTERN

HRESULT CDirectMusicStyle::PasteCF_PATTERN( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject,
											CDirectMusicPattern* pPositionPattern )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfPattern, &pIStream  ) ) 
	||  SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfMotif, &pIStream  ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new Pattern
		CDirectMusicPattern* pPattern = new CDirectMusicPattern( this, FALSE );

		if( pPattern )
		{
			hr = pPattern->Load( pIStream );
			if( SUCCEEDED( hr ) )
			{
				if( pPattern->m_wEmbellishment & EMB_MOTIF )
				{
					// Convert the Motif into a Pattern
					pPattern->m_wEmbellishment = EMB_NORMAL;
					pPattern->m_dwRepeats = 0;
					pPattern->m_mtPlayStart = 0;
					pPattern->m_mtLoopStart = 0;
					pPattern->m_mtLoopEnd = pPattern->m_dwLength;
					pPattern->m_dwResolution = DMUS_SEGF_BEAT;
					RELEASE( pPattern->m_pIBandNode );	// Patterns do not have Bands
				}
				if( pPositionPattern )
				{
					m_StylePatterns.InsertChildNodeAtPos( pPattern, pPositionPattern );
				}
				else
				{
					m_StylePatterns.InsertChildNode( pPattern );
				}
			}

			RELEASE( pPattern );
		}
		else
		{
			hr = E_OUTOFMEMORY ;
		}
		
		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::PasteCF_PATTERNLIST

HRESULT CDirectMusicStyle::PasteCF_PATTERNLIST( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject,
												CDirectMusicPattern* pPositionPattern )
{
	UNREFERENCED_PARAMETER(pPositionPattern);
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList, &pIStream  ) ) 
	||  SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList, &pIStream  ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		RELEASE( pIStream );

		// Remember to convert Motifs into Patterns when this method is actually implemented
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::CanPasteFromData

HRESULT CDirectMusicStyle::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) )
	||  SUCCEEDED ( ContainsClipFormat( pDataObject, pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) 
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotif ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPattern ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::PasteFromData

HRESULT CDirectMusicStyle::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) ) )
	{
		// Handle CF_BAND format
		hr = PasteCF_BAND( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) ) )
	{
		// Handle CF_BANDLIST format
		hr = PasteCF_BANDLIST( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) )
	{
		// Handle CF_BANDTRACK format
		hr = PasteCF_BANDTRACK( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfTimeline ) ) )
	{
		// Handle CF_TIMELINE format
		hr = PasteCF_TIMELINE( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotif ) ) )
	{
		// Handle CF_MOTIF format
		hr = PasteCF_MOTIF( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfMotifList ) ) )
	{
		// Handle CF_MOTIFLIST format
		hr = PasteCF_MOTIFLIST( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPattern ) ) )
	{
		// Handle CF_PATTERN format
		hr = PasteCF_PATTERN( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfPatternList ) ) )
	{
		// Handle CF_PATTERNLIST format
		hr = PasteCF_PATTERNLIST( pDataObject, pIDataObject, NULL );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::CanChildPasteFromData

HRESULT CDirectMusicStyle::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	ASSERT( 0 );

	return E_NOTIMPL;	// Style children handle themselves
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::ChildPasteFromData

HRESULT CDirectMusicStyle::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Style children handle themselves
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdNode::GetObject

HRESULT CDirectMusicStyle::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Must be asking for an interface to a CLSID_DMStyle object
	if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicStyle ) )
	{
		if( m_pIDMStyle )
		{
			return m_pIDMStyle->QueryInterface( riid, ppvObject );
		}
	}

    *ppvObject = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdPropPageObject::GetData

HRESULT CDirectMusicStyle::GetData( void** ppData )
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
// CDirectMusicStyle IDMUSProdPropPageObject::SetData

HRESULT CDirectMusicStyle::SetData( void* pData )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdPropPageObject::OnShowProperties

HRESULT CDirectMusicStyle::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// Get the Style page manager
	CStylePropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_StylePropPageManager ) == S_OK )
	{
		pPageManager = (CStylePropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CStylePropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Style properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CStylePropPageManager::sm_nActiveTab;

		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		RELEASE( pIPropSheet );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CDirectMusicStyle::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IPersist::GetClassID

HRESULT CDirectMusicStyle::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdStyleInfo implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdStyleInfo::GetActiveBandForStyle

HRESULT CDirectMusicStyle::GetActiveBandForStyle( IUnknown** ppIActiveBandNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIActiveBandNode == NULL )
	{
		return E_POINTER;
	}

	if( m_pIActiveBandNode == NULL )
	{
		SetActiveBand( GetTheDefaultBand() );
	}

	if( m_pIActiveBandNode )
	{
		m_pIActiveBandNode->AddRef();
		*ppIActiveBandNode = m_pIActiveBandNode;
		return S_OK;
	}

	*ppIActiveBandNode = NULL;
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdStyleInfo::GetDefaultBand

HRESULT CDirectMusicStyle::GetDefaultBand( IUnknown** ppIDefaultBandNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDefaultBandNode == NULL )
	{
		return E_POINTER;
	}

	IDMUSProdNode* pIBandNode = GetTheDefaultBand();

	if( pIBandNode )
	{
		pIBandNode->AddRef();
		*ppIDefaultBandNode = pIBandNode;
		return S_OK;
	}

	*ppIDefaultBandNode = NULL;
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdStyleInfo::GetTempo

HRESULT CDirectMusicStyle::GetTempo( double* pTempo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pTempo == NULL )
	{
		return E_POINTER;
	}

	*pTempo = m_dblTempo;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdStyleInfo::GetTimeSignature

HRESULT CDirectMusicStyle::GetTimeSignature( DMUSProdTimeSignature* pTimeSignature )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !pTimeSignature )
	{
		return E_POINTER;
	}

	pTimeSignature->bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
	pTimeSignature->bBeat = m_TimeSignature.m_bBeat;
	pTimeSignature->wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdStyleInfo::GetNotationType

HRESULT CDirectMusicStyle::GetNotationType( DWORD *pdwType )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !pdwType )
	{
		return E_POINTER;
	}

	*pdwType = m_dwNotationType;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IDMUSProdStyleInfo::GetActiveBandForObject

HRESULT CDirectMusicStyle::GetActiveBandForObject( IUnknown* punkObject, IUnknown** ppIActiveBandNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIActiveBandNode == NULL )
	{
		return E_POINTER;
	}

	*ppIActiveBandNode = NULL;

	// Find motif with matching IDMUSProdStripMgr interface pointer
	IDMUSProdStripMgr* pIStripMgr;
	if( SUCCEEDED ( punkObject->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pIStripMgr ) ) )
	{
		CDirectMusicPattern* pMotif;

		POSITION pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
		while( pos )
		{
			pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );

			if( pMotif->m_pPatternCtrl
			&&  pMotif->m_pPatternCtrl->m_pPatternDlg
			&&  pMotif->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr == pIStripMgr )
			{
				// Found matching motif - use motif's band
				if( pMotif->m_pIBandNode )
				{
					pMotif->m_pIBandNode->AddRef();
				}
				*ppIActiveBandNode = pMotif->m_pIBandNode;
				break;
			}
		}

		RELEASE( pIStripMgr );
	}

	if( *ppIActiveBandNode == NULL )
	{
		// Find motif with matching IDMUSProdNode interface pointer
		IDMUSProdNode* pINode;
		if( SUCCEEDED ( punkObject->QueryInterface( IID_IDMUSProdNode, (void**)&pINode ) ) )
		{
			CDirectMusicPattern* pMotif;

			POSITION pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
			while( pos )
			{
				pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );

				if( pINode == (IDMUSProdNode *)pMotif )
				{
					// Found matching motif - use motif's band
					if( pMotif->m_pIBandNode )
					{
						pMotif->m_pIBandNode->AddRef();
					}
					*ppIActiveBandNode = pMotif->m_pIBandNode;
					break;
				}
			}

			RELEASE( pINode );
		}
	}

	if( *ppIActiveBandNode )
	{
		return S_OK;
	}

	return GetActiveBandForStyle( ppIActiveBandNode );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IPersistStream::IsDirty

HRESULT CDirectMusicStyle::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fModified
	||  m_StyleBands.IsDirty() 
	||  m_StyleMotifs.IsDirty() 
	||  m_StylePatterns.IsDirty() )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IPersistStream::Load

HRESULT CDirectMusicStyle::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DWORD dwPos = StreamTell( pIStream );

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    HRESULT hr = E_FAIL;

	BOOL fFoundFormat = FALSE;

	// Check for Direct Music format
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_STYLE_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			hr = DM_LoadStyle( pIRiffStream, &ckMain );
			fFoundFormat = TRUE;
		}

		if( fFoundFormat )
		{
			if( m_pIDocRootNode )
			{
				// Already in Project Tree so sync changes
				// Refresh Style editor (when open)
				SyncStyleEditor( SSE_ALL );

				// Sync change with property sheet
				IDMUSProdPropSheet* pIPropSheet;
				if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
				{
					pIPropSheet->RefreshTitle();
					pIPropSheet->RefreshActivePage();
					RELEASE( pIPropSheet );
				}
			}
		}

		RELEASE( pIRiffStream );
	}

	// Check for IMA 2.5 format
	if( fFoundFormat == FALSE )
	{
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = FOURCC_STYLE_FORM;

			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				hr = IMA25_LoadStyle( pIRiffStream, &ckMain );
				fFoundFormat = TRUE;
			}

			if( fFoundFormat )
			{
				if( m_pIDocRootNode )
				{
					// Already in Project Tree so sync changes
					// Refresh Style editor (when open)
					SyncStyleEditor( SSE_ALL );

					// Sync change with property sheet
					IDMUSProdPropSheet* pIPropSheet;
					if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
					{
						pIPropSheet->RefreshTitle();
						pIPropSheet->RefreshActivePage();
						RELEASE( pIPropSheet );
					}
				}
			}

			RELEASE( pIRiffStream );
		}
	}

	// Check for "Undo" format
	if( fFoundFormat == FALSE )
	{
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_STYLE_UNDO_FORM;

			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				hr = UNDO_LoadStyle( pIRiffStream, &ckMain );
				fFoundFormat = TRUE;
			}

			if( fFoundFormat )
			{
				if( m_pIDocRootNode )
				{
					// Already in Project Tree so sync changes
					// Refresh Style editor (when open)
					SyncStyleEditor( (SSE_STYLE | SSE_PATTERNS | SSE_MOTIFS | SSE_BANDS) );

					// Sync change with property sheet
					IDMUSProdPropSheet* pIPropSheet;
					if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
					{
						pIPropSheet->RefreshTitleByObject( this );
						pIPropSheet->RefreshActivePageByObject( this );
						RELEASE( pIPropSheet );
					}
				}
			}

			RELEASE( pIRiffStream );
		}
	}

	//	Persist Style to the DirectMusic DLLs.
	if( SUCCEEDED( hr ) )
	{
		// Make sure we have a default Band
		IDMUSProdNode* pIBandNode = GetTheDefaultBand();
		if( pIBandNode == NULL )
		{
			if( !m_StyleBands.m_lstBands.IsEmpty() )
			{
				pIBandNode = m_StyleBands.m_lstBands.GetHead();
				if( pIBandNode )
				{
					SetDefaultBand( pIBandNode, TRUE );
				}
			}
		}

		// Make sure we have a Music Engine Style
		if( m_pIDMStyle == NULL )
		{
			hr = ::CoCreateInstance( CLSID_DirectMusicStyle, NULL, CLSCTX_INPROC_SERVER,
									 IID_IDirectMusicStyle, (void**)&m_pIDMStyle );
		}

		if( SUCCEEDED( hr ) )
		{
			hr = SyncStyleWithDirectMusic();
		}

		// 21492: Need to notify Segment Designer that the Style's timesig changed during an Undo
		// AFTER we sync with DirectMusic, since a slaved TimeSig track gets its information
		// from the DirectMusic style.
		// Notify connected nodes that Style time signature has changed
		theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( this, STYLE_TimeSigChange, &m_TimeSignature );
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IPersistStream::Save

HRESULT CDirectMusicStyle::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pPersistInfo );
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

    HRESULT hr = E_FAIL;

	// Save the Style
	if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion )
	||  IsEqualGUID( StreamInfo.guidDataFormat, GUID_DirectMusicObject ) )
	{
		// DirectMusic format
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_STYLE_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( DM_SaveStyle( pIRiffStream, fClearDirty ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				if( fClearDirty )
				{
					SetModified( FALSE );
				}

				hr = S_OK;
			}
			RELEASE( pIRiffStream );
		}
	}

	else if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_StyleUndo ) )
	{
		// Style "Undo" format
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_STYLE_UNDO_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( UNDO_SaveStyle( pIRiffStream, StreamInfo.ftFileType ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				hr = S_OK;
			}
			RELEASE( pIRiffStream );
		}
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle IPersistStream::GetSizeMax

HRESULT CDirectMusicStyle::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle Additional functions

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::UNDO_SaveStyle

HRESULT CDirectMusicStyle::UNDO_SaveStyle( IDMUSProdRIFFStream* pIRiffStream, FileType ftFileType )
{
	HRESULT hr = S_OK;
	IStream* pIMemStream = NULL;
	IStream* pIStream;

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

// Save Style chunk
	hr = DM_SaveStyleChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Style UI chunk
	if( ftFileType == FT_DESIGN )
	{
		hr = DM_SaveStyleUIChunk( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

// Save Style GUID
	hr = DM_SaveStyleGUID( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Style info
	hr = DM_SaveStyleInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}


// Save name of default Band
	hr = DM_SaveStyleDefaultBand( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Style version
	hr = DM_SaveStyleVersion( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save node being deleted
	if( m_pINodeBeingDeleted )
	{
		STATSTG	statstg;
		ULARGE_INTEGER uliSizeOut, uliSizeRead, uliSizeWritten;
		GUID guidNodeId;

		hr = E_FAIL;

		m_pINodeBeingDeleted->GetNodeId( &guidNodeId );

		if( ::IsEqualGUID( guidNodeId, GUID_PatternNode )
		||  ::IsEqualGUID( guidNodeId, GUID_MotifNode ) )
		{
			CDirectMusicPattern* pPattern = (CDirectMusicPattern *)m_pINodeBeingDeleted;

			hr = theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_SinglePattern, &pIMemStream );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}

			hr = pPattern->Save( pIMemStream, FALSE );
		}

		else if( ::IsEqualGUID( guidNodeId, GUID_BandNode ) )
		{
			IPersistStream* pIPersistStream;

			hr = theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIMemStream );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}

			hr = m_pINodeBeingDeleted->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}

			hr = pIPersistStream->Save( pIMemStream, FALSE );
			RELEASE( pIPersistStream );
		}

		// Get node's data into "Undo" style RIFF file
		if( SUCCEEDED ( hr ) )
		{
			hr = pIMemStream->Stat( &statstg, STATFLAG_NONAME );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}

			// Store size of memory stream
			uliSizeOut = statstg.cbSize;

			// Seek to beginning of memory stream
			StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );

			// Copy memory stream to 
			hr = pIMemStream->CopyTo( pIStream, uliSizeOut, &uliSizeRead, &uliSizeWritten );
			if( FAILED( hr )
			||  uliSizeRead.QuadPart != uliSizeOut.QuadPart
			||  uliSizeWritten.QuadPart != uliSizeOut.QuadPart )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}
	}

ON_ERROR:
	RELEASE( pIMemStream );
	RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::DM_SaveStyle

HRESULT CDirectMusicStyle::DM_SaveStyle( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty )
{
	HRESULT hr = S_OK;
	POSITION pos;
	IStream* pIStream;
	IPersistStream* pIPersistStream;
	IDMUSProdNode* pIDefaultBandNode;
	IDMUSProdNode* pINode;
	CDirectMusicPattern* pPattern;
	CDirectMusicPattern* pMotif;
	CDirectMusicPart* pPart;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pPersistInfo );
	}

// Save Style chunk
	hr = DM_SaveStyleChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Style UI chunk
	if( StreamInfo.ftFileType == FT_DESIGN )
	{
		hr = DM_SaveStyleUIChunk( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

// Save Style GUID
	hr = DM_SaveStyleGUID( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Style info
	hr = DM_SaveStyleInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Style version
	hr = DM_SaveStyleVersion( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save default Band
	pIDefaultBandNode = GetTheDefaultBand();
	if( pIDefaultBandNode )
	{
		hr = pIDefaultBandNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
		hr = pIPersistStream->Save( pIStream, fClearDirty );
		RELEASE( pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
	}

// Save other Bands
	pos = m_StyleBands.m_lstBands.GetHeadPosition();
	while( pos )
	{
		pINode = m_StyleBands.m_lstBands.GetNext( pos );

		if( pINode != pIDefaultBandNode )
		{
			hr = pINode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}
			hr = pIPersistStream->Save( pIStream, fClearDirty );
			RELEASE( pIPersistStream );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}
		}
	}

// Save Parts
	pos = m_lstStyleParts.GetHeadPosition();
	while( pos )
	{
		pPart = m_lstStyleParts.GetNext( pos );
		hr = pPart->DM_SavePart( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

// Sort Patterns
	m_StylePatterns.SortPatternList();

// Save Patterns
    pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( pos )
    {
        pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
		hr = pPattern->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
		hr = pIPersistStream->Save( pIStream, fClearDirty );
		RELEASE( pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
    }

// Sort Motifs
	m_StyleMotifs.SortMotifList();

// Save Motifs
    pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
    {
        pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );
		hr = pMotif->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
		hr = pIPersistStream->Save( pIStream, fClearDirty );
		RELEASE( pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
    }

// Save Personality References
	hr = DM_SaveStylePersonalityList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::DM_SaveStyleChunk

HRESULT CDirectMusicStyle::DM_SaveStyleChunk( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_STYLE dmusStyleIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Style chunk header
	ck.ckid = DMUS_FOURCC_STYLE_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_STYLE structure
	memset( &dmusStyleIO, 0, sizeof(DMUS_IO_STYLE) );

	dmusStyleIO.timeSig.bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
	dmusStyleIO.timeSig.bBeat = m_TimeSignature.m_bBeat;
	dmusStyleIO.timeSig.wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;

	dmusStyleIO.dblTempo = m_dblTempo;

	// Write Style chunk data
	hr = pIStream->Write( &dmusStyleIO, sizeof(DMUS_IO_STYLE), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_STYLE) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
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
// CDirectMusicStyle::DM_SaveStyleUIChunk

HRESULT CDirectMusicStyle::DM_SaveStyleUIChunk( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	ioDMStyleUI oDMStyleUI;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Style UI chunk header
	ck.ckid = DMUS_FOURCC_STYLE_UI_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioDMStyleUI structure
	memset( &oDMStyleUI, 0, sizeof(ioDMStyleUI) );

	oDMStyleUI.m_nSplitterYPos = m_nSplitterYPos;
	oDMStyleUI.m_dwNotationType = m_dwNotationType;

	// Write Style UI chunk data
	hr = pIStream->Write( &oDMStyleUI, sizeof(ioDMStyleUI), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioDMStyleUI) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
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
// CDirectMusicStyle::DM_SaveStyleGUID

HRESULT CDirectMusicStyle::DM_SaveStyleGUID( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write GUID chunk header
	ck.ckid = DMUS_FOURCC_GUID_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Style GUID
	hr = pIStream->Write( &m_guidStyle, sizeof(GUID), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(GUID) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
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
// CDirectMusicStyle::DM_SaveStyleInfoList

HRESULT CDirectMusicStyle::DM_SaveStyleInfoList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;

	if( m_strName.IsEmpty()
	&&  m_strCategoryName.IsEmpty()
	&&  m_strAuthor.IsEmpty()
	&&  m_strCopyright.IsEmpty()
	&&  m_strSubject.IsEmpty()
	&&  m_strInfo.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write UNFO LIST header
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Style name
	if( !m_strName.IsEmpty() )
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

	// Write Style category
	if( !m_strCategoryName.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_CATEGORY_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strCategoryName );
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

	// Write Style author
	if( !m_strAuthor.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UART_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strAuthor );
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

	// Write Style copyright
	if( !m_strCopyright.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UCOP_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strCopyright );
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

	// Write Style subject
	if( !m_strSubject.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_USBJ_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strSubject );
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

	// Write Style comments
	if( !m_strInfo.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UCMT_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strInfo );
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
// CDirectMusicStyle::DM_SaveStyleDefaultBand

HRESULT CDirectMusicStyle::DM_SaveStyleDefaultBand( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
    MMCKINFO ck;

	// Find default Band
	IDMUSProdNode* pIBandNode = GetTheDefaultBand();
	if( pIBandNode == NULL )
	{
		return S_OK;
	}

	// Get name of default Band
	CString strBandName;
	BSTR bstrBandName;
	if( SUCCEEDED ( pIBandNode->GetNodeName( &bstrBandName ) ) )
	{
		strBandName = bstrBandName;
		::SysFreeString( bstrBandName );
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write default Band name
	if( !strBandName.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_DEFAULT_BAND_UI_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &strBandName );
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

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::DM_SaveStyleVersion

HRESULT CDirectMusicStyle::DM_SaveStyleVersion( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_VERSION dmusStyleVersionIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write StyleVersion chunk header
	ck.ckid = DMUS_FOURCC_VERSION_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_VERSION structure
	memset( &dmusStyleVersionIO, 0, sizeof(DMUS_IO_VERSION) );

	dmusStyleVersionIO.dwVersionMS = m_dwVersionMS ;
	dmusStyleVersionIO.dwVersionLS = m_dwVersionLS;

	// Write StyleVersion chunk data
	hr = pIStream->Write( &dmusStyleVersionIO, sizeof(DMUS_IO_VERSION), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_VERSION) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
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
// CDirectMusicStyle::DM_SaveStylePersonalityList

HRESULT CDirectMusicStyle::DM_SaveStylePersonalityList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
	POSITION pos;
	CPersonality* pPersonality;

	if( m_StylePersonalities.m_lstPersonalities.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	hr = E_FAIL;

	// Write PERSONALITY LIST header
	ckMain.fccType = DMUS_FOURCC_STYLE_PERS_REF_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// First, save the default Personality
	pos = m_StylePersonalities.m_lstPersonalities.GetHeadPosition();
	while( pos )
	{
		pPersonality = m_StylePersonalities.m_lstPersonalities.GetNext( pos );

		if( pPersonality->m_fDefault )
		{
			hr = pPersonality->DM_SavePersonalityRefChunk( pIRiffStream );
			break;
		}
	}

	// Save remaining Personalities
	pos = m_StylePersonalities.m_lstPersonalities.GetHeadPosition();
	while( pos )
	{
		pPersonality = m_StylePersonalities.m_lstPersonalities.GetNext( pos );

		if( pPersonality->m_fDefault == FALSE )
		{
			hr = pPersonality->DM_SavePersonalityRefChunk( pIRiffStream );
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
// CDirectMusicStyle::IMA25_LoadStyle

HRESULT CDirectMusicStyle::IMA25_LoadStyle( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*      pIStream;
    IDMUSProdRIFFExt* pIRIFFExt;
    IDMUSProdNode*    pINode;
	MMCKINFO	  ck;
	DWORD		  dwByteCount;
	DWORD		  dwSize;
	DWORD		  dwPos;
    HRESULT       hr = S_OK;
    ioStyle       iStyle;

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIBandComponent != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case FOURCC_STYLE:
				dwSize = min( ck.cksize, sizeof( iStyle ) );
				hr = pIStream->Read( &iStyle, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				m_strName = iStyle.wstrName;
				m_strCategoryName = iStyle.wstrCategory;
				m_TimeSignature.m_bBeatsPerMeasure = (BYTE)iStyle.wBPM;
				m_TimeSignature.m_bBeat = (BYTE)iStyle.wBeat;
				m_TimeSignature.m_wGridsPerBeat = iStyle.wClocksPerBeat / iStyle.wClocksPerClick;
				m_dblTempo = iStyle.wTempo;
				memcpy( &m_guidStyle, &iStyle.guid, sizeof( m_guidStyle ) );
				break;

			case FOURCC_STYLEINFO:
				ReadMBSfromWCS( pIStream, ck.cksize, &m_strInfo );
				break;

			case FOURCC_PERSONALITYREF:
			{
				CPersonality* pPersonality = new CPersonality();
				if( pPersonality == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}
				hr = pPersonality->IMA25_LoadPersonalityRef( pIStream, &ck );
				if( FAILED( hr ) )
				{
					delete pPersonality;
	                goto ON_ERROR;
				}
				m_StylePersonalities.m_lstPersonalities.AddTail( pPersonality );
				break;
			}

			case FOURCC_RIFF:
				switch( ck.fccType )
				{
					case FOURCC_BAND_FORM:
						hr = theApp.m_pStyleComponent->m_pIBandComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
						RELEASE( pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
						// Make sure Band name is unique
						GetUniqueBandName( pINode );
						m_StyleBands.m_lstBands.AddTail( pINode );
						break;

					case FOURCC_PATTERN_FORM:
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						CDirectMusicPattern* pPattern = new CDirectMusicPattern( this, FALSE );
						if( pPattern == NULL )
						{
							hr = E_OUTOFMEMORY ;
			                goto ON_ERROR;
						}
						hr = pPattern->Load( pIStream );
						if( FAILED( hr ) )
						{
							RELEASE( pPattern );
			                goto ON_ERROR;
						}
						if( pPattern->m_wEmbellishment & EMB_MOTIF )
						{
							m_StyleMotifs.m_lstMotifs.AddTail( pPattern );
						}
						else
						{
							m_StylePatterns.m_lstPatterns.AddTail( pPattern );
						}
						break;
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
// CDirectMusicStyle::DM_LoadStyle

HRESULT CDirectMusicStyle::DM_LoadStyle( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	CDirectMusicPart* pPart;
    IStream*      pIStream;
    IDMUSProdRIFFExt* pIRIFFExt;
    IDMUSProdNode*    pINode;
	MMCKINFO	  ck;
	MMCKINFO	  ckList;
	POSITION	  pos;
	DWORD		  dwByteCount;
	DWORD		  dwSize;
	DWORD		  dwPos;
    HRESULT       hr = S_OK;

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIBandComponent != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_STYLE_CHUNK:
			{
			    DMUS_IO_STYLE dmusStyleIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_STYLE ) );
				hr = pIStream->Read( &dmusStyleIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_TimeSignature.m_bBeatsPerMeasure = dmusStyleIO.timeSig.bBeatsPerMeasure;
				m_TimeSignature.m_bBeat = dmusStyleIO.timeSig.bBeat;
				m_TimeSignature.m_wGridsPerBeat = dmusStyleIO.timeSig.wGridsPerBeat;
				m_dblTempo = dmusStyleIO.dblTempo;
				break;
			}

			case DMUS_FOURCC_STYLE_UI_CHUNK:
			{
			    ioDMStyleUI iDMStyleUI;
				ZeroMemory( &iDMStyleUI, sizeof(ioDMStyleUI ) );

				dwSize = min( ck.cksize, sizeof( ioDMStyleUI ) );
				hr = pIStream->Read( &iDMStyleUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_nSplitterYPos = iDMStyleUI.m_nSplitterYPos;
				m_dwNotationType = iDMStyleUI.m_dwNotationType;
				break;
			}

			case DMUS_FOURCC_GUID_CHUNK:
				dwSize = min( ck.cksize, sizeof( GUID ) );
				hr = pIStream->Read( &m_guidStyle, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;

			case DMUS_FOURCC_VERSION_CHUNK:
			{
			    DMUS_IO_VERSION dmusStyleVersionIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_VERSION ) );
				hr = pIStream->Read( &dmusStyleVersionIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwVersionMS = dmusStyleVersionIO.dwVersionMS;
				m_dwVersionLS = dmusStyleVersionIO.dwVersionLS;
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_INFO_LIST:
					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case RIFFINFO_INAM:
								case DMUS_FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );
									break;

								case RIFFINFO_IART:
								case DMUS_FOURCC_UART_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strAuthor );
									break;

								case RIFFINFO_ICOP:
								case DMUS_FOURCC_UCOP_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strCopyright );
									break;

								case RIFFINFO_ISBJ:
								case DMUS_FOURCC_USBJ_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strSubject );
									break;

								case RIFFINFO_ICMT:
								case DMUS_FOURCC_UCMT_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strInfo );
									break;

								case DMUS_FOURCC_CATEGORY_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strCategoryName );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case DMUS_FOURCC_PART_LIST:
						pPart = AllocPart();
						if( pPart == NULL )
						{
							hr = E_OUTOFMEMORY ;
			                goto ON_ERROR;
						}
						hr = pPart->DM_LoadPart( pIRiffStream, &ck );
						if( FAILED( hr ) )
						{
							DeletePart( pPart );
			                goto ON_ERROR;
						}
						if( hr == S_FALSE )
						{
							// This should not happen when loading a Style!
							ASSERT( 0 );
							// Bypass this Part because Style already contains a Part
							// whose GUID matches pPart->m_guidPartID
							DeletePart( pPart );
						}
						break;

					case DMUS_FOURCC_PATTERN_LIST:
					{
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						CDirectMusicPattern* pPattern = new CDirectMusicPattern( this, FALSE );
						if( pPattern == NULL )
						{
							hr = E_OUTOFMEMORY ;
			                goto ON_ERROR;
						}
						hr = pPattern->Load( pIStream );
						if( FAILED( hr ) )
						{
							RELEASE( pPattern );
			                goto ON_ERROR;
						}
						if( pPattern->m_wEmbellishment & EMB_MOTIF )
						{
							m_StyleMotifs.m_lstMotifs.AddTail( pPattern );
						}
						else
						{
							m_StylePatterns.m_lstPatterns.AddTail( pPattern );
						}
						break;
					}

					case DMUS_FOURCC_STYLE_PERS_REF_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case FOURCC_LIST:
									switch( ckList.fccType )
									{
										case DMUS_FOURCC_REF_LIST:
										{
											CPersonality* pPersonality = new CPersonality();
											if( pPersonality == NULL )
											{
												hr = E_OUTOFMEMORY;
												goto ON_ERROR;
											}
											hr = pPersonality->DM_LoadPersonalityRef( pIRiffStream, &ckList );
											if( FAILED( hr ) )
											{
												delete pPersonality;
												goto ON_ERROR;
											}
											if( m_StylePersonalities.m_lstPersonalities.IsEmpty() )
											{
												pPersonality->m_fDefault = TRUE;
											}
											m_StylePersonalities.m_lstPersonalities.AddTail( pPersonality );
											break;
										}
									}
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;

			case FOURCC_RIFF:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_BAND_FORM: //ECW DM band within IMA2.5 Style (mainly for testing/conversion)
					case FOURCC_BAND_FORM:
						hr = theApp.m_pStyleComponent->m_pIBandComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
						RELEASE( pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
						// Make sure Band name is unique
						GetUniqueBandName( pINode );
						m_StyleBands.m_lstBands.AddTail( pINode );
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
	    dwPos = StreamTell( pIStream );
    }

	// Delete Parts with m_dwUseCount of zero
    pos = m_lstStyleParts.GetHeadPosition();
    while( pos )
    {
        pPart = m_lstStyleParts.GetNext( pos );

		if( pPart->m_dwUseCount == 0 )
		{
			// This should not happen!
			ASSERT( 0 );
			DeletePart( pPart );
		}
	}

	// Mark all 'linked' PartRefs that point to a part with m_dwUseCount of one as 'unlinked'
	UpdateLinkFlags();

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::UNDO_LoadStyle

HRESULT CDirectMusicStyle::UNDO_LoadStyle( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*      pIStream;
    IDMUSProdRIFFExt* pIRIFFExt;
    IDMUSProdNode*    pINode;
	MMCKINFO	  ck;
	MMCKINFO	  ckList;
	DWORD		  dwByteCount;
	DWORD		  dwSize;
	DWORD		  dwPos;
    HRESULT       hr = S_OK;

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIBandComponent != NULL );

	// Following strings only saved when they have values
	// So make sure they are initialized!!
	m_strCategoryName.Empty();
	m_strAuthor.Empty();
	m_strCopyright.Empty();
	m_strSubject.Empty();
	m_strInfo.Empty();

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_STYLE_CHUNK:
			{
			    DMUS_IO_STYLE dmusStyleIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_STYLE ) );
				hr = pIStream->Read( &dmusStyleIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				DirectMusicTimeSig	timeSig;
			
				timeSig.m_bBeatsPerMeasure = dmusStyleIO.timeSig.bBeatsPerMeasure;
				timeSig.m_bBeat = dmusStyleIO.timeSig.bBeat;
				timeSig.m_wGridsPerBeat = dmusStyleIO.timeSig.wGridsPerBeat;

				SetTimeSignature( timeSig, TRUE );
				SetTempo( dmusStyleIO.dblTempo, TRUE );
				break;
			}

			case DMUS_FOURCC_STYLE_UI_CHUNK:
			{
			    ioDMStyleUI iDMStyleUI;

				dwSize = min( ck.cksize, sizeof( ioDMStyleUI ) );
				hr = pIStream->Read( &iDMStyleUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// m_nSplitterYPos = iDMStyleUI.m_nSplitterYPos;
				// m_dwNotationType = iDMStyleUI.m_dwNotationType;
				break;
			}

			case DMUS_FOURCC_GUID_CHUNK:
				dwSize = min( ck.cksize, sizeof( GUID ) );
				hr = pIStream->Read( &m_guidStyle, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;

			case DMUS_FOURCC_VERSION_CHUNK:
			{
			    DMUS_IO_VERSION dmusStyleVersionIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_VERSION ) );
				hr = pIStream->Read( &dmusStyleVersionIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwVersionMS = dmusStyleVersionIO.dwVersionMS;
				m_dwVersionLS = dmusStyleVersionIO.dwVersionLS;
				break;
			}

			case DMUS_FOURCC_DEFAULT_BAND_UI_CHUNK:
			{
				CString strBandName;

				ReadMBSfromWCS( pIStream, ck.cksize, &strBandName );
				IDMUSProdNode* pIBandNode = GetBandNodeByName( strBandName );
				if( pIBandNode )
				{
					SetDefaultBand( pIBandNode, TRUE );
					pIBandNode->Release();
				}
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_INFO_LIST:
					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case RIFFINFO_INAM:
								case DMUS_FOURCC_UNAM_CHUNK:
								{
									CString strName;

									ReadMBSfromWCS( pIStream, ckList.cksize, &strName );
									if( strName.CompareNoCase( m_strName ) != 0 )
									{
										m_strName = strName;
										theApp.m_pStyleComponent->m_pIFramework->RefreshNode( this );

										// Notify connected nodes that Style name has changed
										theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( this, STYLE_NameChange, NULL );
									}
									break;
								}

								case RIFFINFO_IART:
								case DMUS_FOURCC_UART_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strAuthor );
									break;

								case RIFFINFO_ICOP:
								case DMUS_FOURCC_UCOP_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strCopyright );
									break;

								case RIFFINFO_ISBJ:
								case DMUS_FOURCC_USBJ_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strSubject );
									break;

								case RIFFINFO_ICMT:
								case DMUS_FOURCC_UCMT_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strInfo );
									break;

								case DMUS_FOURCC_CATEGORY_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strCategoryName );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;

			case FOURCC_RIFF:
				switch( ck.fccType )
				{
					case FOURCC_BAND_FORM:
						hr = theApp.m_pStyleComponent->m_pIBandComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
						RELEASE( pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
						m_StyleBands.InsertChildNode( pINode );
						break;

					case DMUS_FOURCC_PATTERN_LIST:
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						CDirectMusicPattern* pPattern = new CDirectMusicPattern( this, FALSE );
						if( pPattern == NULL )
						{
							hr = E_OUTOFMEMORY ;
			                goto ON_ERROR;
						}
						hr = pPattern->Load( pIStream );
						if( FAILED( hr ) )
						{
							RELEASE( pPattern );
			                goto ON_ERROR;
						}
						if( pPattern->m_wEmbellishment & EMB_MOTIF )
						{
							m_StyleMotifs.InsertChildNode( pPattern );
						}
						else
						{
							m_StylePatterns.InsertChildNode( pPattern );
						}
						break;
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
// CDirectMusicStyle IDMUSProdNotifySink implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::OnUpdate

HRESULT CDirectMusicStyle::OnUpdate( IDMUSProdNode* pINode, GUID guidUpdateType, VOID* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	if( ::IsEqualGUID( guidUpdateType, BAND_ValidateNameChange ) )
	{
		BSTR bstrBandName;

		// Make sure name of Band is unique
		if( SUCCEEDED ( pINode->GetNodeName( &bstrBandName ) ) )
		{
			CString strBandName = bstrBandName;
			::SysFreeString( bstrBandName );

			if( IsBandNameUnique( pINode, strBandName ) == FALSE )
			{
				CString strMsg;

				AfxFormatString1( strMsg, IDS_DUPE_BAND_NAME, strBandName );
				AfxMessageBox( strMsg, MB_OK );
				return S_FALSE;
			}
		}
	}
	else if( ::IsEqualGUID( guidUpdateType, BAND_NameChange ) )
	{
		// Refresh Style editor (when open)
		SyncStyleEditor( SSE_BANDS );
	}
	else if( ::IsEqualGUID( guidUpdateType, GUID_BAND_ChangeNotifyMsg )
		 ||  ::IsEqualGUID( guidUpdateType, GUID_BAND_ActivateNotifyMsg ) )
	{
		if( IsInStyleBandList( pINode ) )
		{
			if( m_pIActiveBandNode != pINode )
			{
				SetActiveBand( pINode );
				if( m_pStyleCtrl
				&&  m_pStyleCtrl->m_pStyleDlg )
				{
					m_pStyleCtrl->m_pStyleDlg->SelectBand( pINode );
				}

				// Set SetActiveBand takes care of sending everything so return S_FALSE
				return S_FALSE;	
			}
		}
		else
		{
			if( ::IsEqualGUID( guidUpdateType, GUID_BAND_ChangeNotifyMsg ) )
			{
				// Check to see which Motif contains the Band
				POSITION pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
				while( pos )
				{
					CDirectMusicPattern* pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );
				
					if( pMotif->m_pIBandNode == pINode )
					{
						pMotif->SetModified( TRUE );
						pMotif->SyncPatternWithDirectMusic();
						break;
					}
				}
			}
		}

		if( SyncBand( pINode ) != S_OK )
		{
			return S_FALSE;
		}
	}
	else if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileNameChange) 
		 &&  (pINode == this) )
	{
		// Updates the names of the Style's Patterns in the Transport toolbar
		POSITION pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
		while( pos )
		{
			CDirectMusicPattern* pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
			BSTR bstrTransportName;

			if( SUCCEEDED ( pPattern->GetName( &bstrTransportName ) ) )
			{
				theApp.m_pStyleComponent->m_pIConductor->SetTransportName( pPattern, bstrTransportName );
			}
		}

		// Updates the names of the Style's Motifs in the Transport toolbars
		pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
		while( pos )
		{
			CDirectMusicPattern* pPattern = m_StyleMotifs.m_lstMotifs.GetNext( pos );
			BSTR bstrTransportName;

			if( SUCCEEDED ( pPattern->GetName( &bstrTransportName ) ) )
			{
				theApp.m_pStyleComponent->m_pIConductor->SetTransportName( pPattern, bstrTransportName );
				if( SUCCEEDED ( pPattern->GetName( &bstrTransportName ) ) )
				{
					theApp.m_pStyleComponent->m_pIConductor->SetSecondaryTransportName( pPattern, bstrTransportName );
				}
			}
		}
	}
	else if( ::IsEqualGUID( guidUpdateType, STYLE_NotationTypeChange ) )
	{
		ASSERT( pData );
		if( pData )
		{
			if( m_dwNotationType != *(static_cast<DWORD *>(pData)) )
			{
				m_dwNotationType = *(static_cast<DWORD *>(pData));

				// Update the Style's Patterns with the new notation type
				POSITION pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
				while( pos )
				{
					CDirectMusicPattern* pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
					if( pPattern->m_pPatternCtrl
					&&	pPattern->m_pPatternCtrl->m_pPatternDlg
					&&	pPattern->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr )
					{
						pPattern->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr->OnUpdate( STYLE_NotationTypeChange, 0xFFFFFFFF, pData );
					}
				}

				// Update the Style's Motifs with the new notation type
				pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
				while( pos )
				{
					CDirectMusicPattern* pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );
					if( pMotif->m_pPatternCtrl
					&&	pMotif->m_pPatternCtrl->m_pPatternDlg
					&&	pMotif->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr )
					{
						pMotif->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr->OnUpdate( STYLE_NotationTypeChange, 0xFFFFFFFF, pData );
					}
				}
			}
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::RegisterWithTransport

HRESULT CDirectMusicStyle::RegisterWithTransport( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicPattern* pPattern;
	CDirectMusicPattern* pMotif;
	HRESULT hr;

	// Register the Style's Patterns with the Transport Controls
    POSITION pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( pos )
    {
        pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
		hr = pPattern->RegisterWithTransport();
		if( FAILED( hr ) )
		{
			return hr;
		}
    }

	// Register the Style's Motifs with the Transport Controls
    pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
    {
        pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );
		hr = pMotif->RegisterWithTransport();
		if( FAILED( hr ) )
		{
			return hr;
		}
    }

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::UnRegisterWithTransport

HRESULT CDirectMusicStyle::UnRegisterWithTransport( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicPattern* pPattern;
	CDirectMusicPattern* pMotif;

	// Unregister the Style's Patterns with the Transport Controls
    POSITION pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( pos )
    {
        pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
		pPattern->UnRegisterWithTransport();
    }

	// Register the Style's Motifs with the Transport Controls
    pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
    {
        pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );
		pMotif->UnRegisterWithTransport();
    }

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::FindPartByGUID

CDirectMusicPart* CDirectMusicStyle::FindPartByGUID( GUID guidPartID )
{
	CDirectMusicPart* pThePart = NULL;
	CDirectMusicPart* pPart;

	POSITION pos = m_lstStyleParts.GetHeadPosition();
	while( pos )
	{
		pPart = m_lstStyleParts.GetNext( pos );
		
		if( ::IsEqualGUID( pPart->m_guidPartID, guidPartID ) )
		{
			pThePart = pPart;
			break;
		}
	}

	return pThePart;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::AllocPart

CDirectMusicPart* CDirectMusicStyle::AllocPart( void )
{
	CDirectMusicPart* pPart = new CDirectMusicPart( this );

	if( pPart )
	{
		pPart->m_pPartDesignData = (ioDMStylePartDesign *)GlobalAlloc( GPTR, sizeof(ioDMStylePartDesign) );

		if( pPart->m_pPartDesignData )
		{
			pPart->m_dwPartDesignDataSize = sizeof(ioDMStylePartDesign);

			// Add Part to Style's list of Parts
			m_lstStyleParts.AddTail( pPart );
		}
		else
		{
			delete pPart;
			pPart = NULL;
		}
	}

	return pPart;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::DeletePart

void CDirectMusicStyle::DeletePart( CDirectMusicPart* pPart )
{
	ASSERT( pPart != NULL );
	ASSERT( pPart->m_dwUseCount == 0 );

	// Remove Part from Style's Part list
	POSITION pos = m_lstStyleParts.Find( pPart );

	if( pos )
	{
		m_lstStyleParts.RemoveAt( pos );
	}

	delete pPart;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::AddPattern

HRESULT CDirectMusicStyle::AddPattern( CDirectMusicPattern* pPattern,
									   CDirectMusicPattern* pPositionPattern )
{
	ASSERT( pPattern != NULL );
	if( pPattern == NULL )
	{
		return E_INVALIDARG;
	}

	pPattern->AddRef();

	// Add to CDirectMusicStyle Pattern list
	POSITION pos = NULL;

	if( pPositionPattern )
	{
		pos = m_StylePatterns.m_lstPatterns.Find( pPositionPattern );
	}

	if( pos )
	{
		m_StylePatterns.m_lstPatterns.InsertBefore( pos, pPattern );
	}
	else
	{
		m_StylePatterns.m_lstPatterns.AddTail( pPattern );
	}

	// Sync Style editor (when open)
	SyncStyleEditor( SSE_PATTERNS );
	if( m_pStyleCtrl
	&&  m_pStyleCtrl->m_pStyleDlg )
	{
		m_pStyleCtrl->m_pStyleDlg->SelectPattern( pPattern );
	}

	// Update the DirectMusic Style object
	HRESULT hr = SyncStyleWithDirectMusic();
	ASSERT( SUCCEEDED ( hr ) );

	// Pattern needs transport controls
	hr = pPattern->RegisterWithTransport();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::RemovePattern

HRESULT CDirectMusicStyle::RemovePattern( CDirectMusicPattern* pPattern )
{
	BOOL fChange = FALSE;
	
	ASSERT( pPattern != NULL );

	// Pattern no longer needs transport controls
	pPattern->UnRegisterWithTransport();

	// Remove from CDirectMusicStyle Pattern list
	POSITION pos = m_StylePatterns.m_lstPatterns.Find( pPattern );
	if( pos )
	{
		m_StylePatterns.m_lstPatterns.RemoveAt( pos );

		pPattern->PreDeleteCleanup();

		// See if we need to deal with link flags or the var. choices window
		POSITION pos2 = pPattern->m_lstPartRefs.GetHeadPosition();
		while( pos2 )
		{
			CDirectMusicPartRef* pDMPartRef = pPattern->m_lstPartRefs.GetNext( pos2 );

			if( pDMPartRef->m_pDMPart
			&&	(pDMPartRef->m_pDMPart->m_pVarChoicesPartRef == pDMPartRef) )
			{
				// Close the variation choices dialog
				if( pDMPartRef->m_pDMPart->m_pVarChoicesNode )
				{
					HWND hWndEditor;
					pDMPartRef->m_pDMPart->m_pVarChoicesNode->GetEditorWindow( &hWndEditor );
					if( hWndEditor )
					{
						theApp.m_pStyleComponent->m_pIFramework->CloseEditor( pDMPartRef->m_pDMPart->m_pVarChoicesNode );
					}
					RELEASE( pDMPartRef->m_pDMPart->m_pVarChoicesNode );
				}
				pDMPartRef->m_pDMPart->m_pVarChoicesPartRef = NULL;
			}

			if( pDMPartRef->m_fHardLink )
			{
				pPattern->DeletePartRef( pDMPartRef );
				fChange = TRUE;
			}
		}

		RELEASE( pPattern );
	}

	// Sync Style editor (when open)
	SyncStyleEditor( SSE_PATTERNS );

	// Update link flags when necessary
	if( fChange )
	{
		UpdateLinkFlags();
	}

	// Update the DirectMusic Style object
	HRESULT hr = SyncStyleWithDirectMusic();
	ASSERT( SUCCEEDED ( hr ) );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::AddMotif

HRESULT CDirectMusicStyle::AddMotif( CDirectMusicPattern* pMotif,
									 CDirectMusicPattern* pPositionMotif )
{
	ASSERT( pMotif != NULL );
	if( pMotif == NULL )
	{
		return E_INVALIDARG;
	}

	pMotif->AddRef();

	// Add to CDirectMusicStyle Motif list
	POSITION pos = NULL;

	if( pPositionMotif )
	{
		pos = m_StyleMotifs.m_lstMotifs.Find( pPositionMotif );
	}

	if( pos )
	{
		m_StyleMotifs.m_lstMotifs.InsertBefore( pos, pMotif );
	}
	else
	{
		m_StyleMotifs.m_lstMotifs.AddTail( pMotif );
	}

	// Sync Style editor (when open)
	SyncStyleEditor( SSE_MOTIFS );
	if( m_pStyleCtrl
	&&  m_pStyleCtrl->m_pStyleDlg )
	{
		m_pStyleCtrl->m_pStyleDlg->SelectMotif( pMotif );
	}

	// Update the DirectMusic Style object
	HRESULT hr = SyncStyleWithDirectMusic();
	ASSERT( SUCCEEDED ( hr ) );

	// Motif needs transport controls
	hr = pMotif->RegisterWithTransport();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::RemoveMotif

HRESULT CDirectMusicStyle::RemoveMotif( CDirectMusicPattern* pMotif )
{
	CDirectMusicPartRef* pDMPartRef;
	BOOL fChange = FALSE;
	
	ASSERT( pMotif != NULL );

	// Motif no longer needs transport controls
	pMotif->UnRegisterWithTransport();

	// Remove from CDirectMusicStyle Motif list
	POSITION pos = m_StyleMotifs.m_lstMotifs.Find( pMotif );
	if( pos )
	{
		m_StyleMotifs.m_lstMotifs.RemoveAt( pos );

		pMotif->PreDeleteCleanup();

		// See if we need to deal with link flags
		POSITION pos2 = pMotif->m_lstPartRefs.GetHeadPosition();
		while( pos2 )
		{
			pDMPartRef = pMotif->m_lstPartRefs.GetNext( pos2 );

			if( pDMPartRef->m_fHardLink )
			{
				pMotif->DeletePartRef( pDMPartRef );
				fChange = TRUE;
			}
		}
		
		RELEASE( pMotif );
	}

	// Sync Style editor (when open)
	SyncStyleEditor( SSE_MOTIFS );

	// Update link flags when necessary
	if( fChange )
	{
		UpdateLinkFlags();
	}

	// Update the DirectMusic Style object
	HRESULT hr = SyncStyleWithDirectMusic();
	ASSERT( SUCCEEDED ( hr ) );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SyncBand

HRESULT CDirectMusicStyle::SyncBand( IDMUSProdNode* pIBandNode )
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );

	IDirectMusicSegment* pIDMSegment = NULL;

	if( IsInStyleBandList( pIBandNode ) )
	{
		CDirectMusicPattern* pPattern;
		CDirectMusicPattern* pMotif;

		// See if Conductor is playing a Pattern in this Style
		POSITION pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
		while( pos )
		{
			pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );

			if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( pPattern ) == S_OK )
			{
				if( pPattern->m_pIDMSegment )
				{
					// We are playing a Pattern in this Style...
					// so we must sync the Band we are hearing 
					pPattern->m_pIDMSegment->AddRef();
					pIDMSegment = pPattern->m_pIDMSegment;
				}
			}

			// Redraw the functionbar areas, since the band may have changed
			if( pPattern->m_pPatternCtrl
			&&	pPattern->m_pPatternCtrl->m_pPatternDlg
			&&	pPattern->m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
			{
				pPattern->m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->NotifyStripMgrs( GUID_Segment_BandTrackChange, 0xFFFFFFFF, NULL );
			}
		}

		// See if Conductor is playing a Motif in this Style
		pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
		while( pos )
		{
			pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );

			if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( pMotif ) == S_OK )
			{
				if( pMotif->m_pIDMSegment )
				{
					// We are playing a Motif that does not have an associated Band...
					// so we must sync the Band we are hearing 
					if( pMotif->m_pIBandNode == NULL )
					{
						pMotif->m_pIDMSegment->AddRef();
						pIDMSegment = pMotif->m_pIDMSegment;
					}
				}
			}

			// Redraw the functionbar areas, since the band may have changed
			if( pMotif->m_pPatternCtrl
			&&	pMotif->m_pPatternCtrl->m_pPatternDlg
			&&	pMotif->m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
			{
				pMotif->m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->NotifyStripMgrs( GUID_Segment_BandTrackChange, 0xFFFFFFFF, NULL );
			}
		}
	}
	else
	{
		CDirectMusicPattern* pMotif;

		// See if Conductor is playing a Motif in this Style
		POSITION pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
		while( pos )
		{
			pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );

			if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( pMotif ) == S_OK )
			{
				if( pMotif->m_pIDMSegment )
				{
					// We are playing this Motif 
					if( pMotif->m_pIBandNode == pIBandNode )
					{
						// Motif's Band has changed...
						// so we must sync the Band we are hearing 
						pMotif->m_pIDMSegment->AddRef();
						pIDMSegment = pMotif->m_pIDMSegment;
					}
				}
			}
		}
	}

	if( pIDMSegment )
	{
		// We need to sync the Band we are hearing
		CWaitCursor wait;

		// Persist the Band into a DirectMusicBand object
		IDirectMusicBand* pIDMBand = GetDMBand( pIBandNode );
		if( pIDMBand )
		{
			// Update the Band in the Band Track
			IDirectMusicTrack* pIDMTrack;

			if( SUCCEEDED ( pIDMSegment->GetTrack( CLSID_DirectMusicBandTrack, 1, 0, &pIDMTrack ) ) )
			{
				// Just clear the Band track so that it won't interfere
				// with realtime changes when the Pattern loops
				pIDMTrack->SetParam( GUID_Clear_All_Bands, 0, NULL );
				RELEASE( pIDMTrack );
			}

			RELEASE( pIDMBand );
		}

		RELEASE( pIDMSegment );
		
		// S_OK means this Band is playing
		return S_OK;
	}


	IDirectMusicSegment* pIDMSegmentTemp = GetSegmentThatIsPlaying();
	if( pIDMSegmentTemp )
	{
		// Nothing in Style is playing so we need to sync with DirectMusic now
		RELEASE( pIDMSegmentTemp );
		ForceSyncStyleWithDirectMusic();
	}

	// S_FALSE means this Band is not playing
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SendBand

void CDirectMusicStyle::SendBand( IDMUSProdNode* pIBandNode )
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );

	IDirectMusicSegment* pIDMSegment = NULL;

	if( IsInStyleBandList( pIBandNode ) )
	{
		CDirectMusicPattern* pPattern;
		CDirectMusicPattern* pMotif;

		// See if Conductor is playing a Pattern in this Style
		POSITION pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
		while( pos )
		{
			pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );

			if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( pPattern ) == S_OK )
			{
				if( pPattern->m_pIDMSegment )
				{
					// We are playing a Pattern in this Style...
					// so we must sync the Band we are hearing 
					pPattern->m_pIDMSegment->AddRef();
					pIDMSegment = pPattern->m_pIDMSegment;
				}
			}
		}

		if( pIDMSegment == NULL )
		{
			// See if Conductor is playing a Motif in this Style
			pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
			while( pos )
			{
				pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );

				if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( pMotif ) == S_OK )
				{
					if( pMotif->m_pIDMSegment )
					{
						// We are playing a Motif that does not have an associated Band...
						// so we must sync the Band we are hearing 
						if( pMotif->m_pIBandNode == NULL )
						{
							pMotif->m_pIDMSegment->AddRef();
							pIDMSegment = pMotif->m_pIDMSegment;
						}
					}
				}
			}
		}
	}
	else
	{
		CDirectMusicPattern* pMotif;

		// See if Conductor is playing a Motif in this Style
		POSITION pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
		while( pos )
		{
			pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );

			if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( pMotif ) == S_OK )
			{
				if( pMotif->m_pIDMSegment )
				{
					// We are playing this Motif 
					if( pMotif->m_pIBandNode == pIBandNode )
					{
						// Motif's Band has changed...
						// so we must sync the Band we are hearing 
						pMotif->m_pIDMSegment->AddRef();
						pIDMSegment = pMotif->m_pIDMSegment;
					}
				}
			}
		}
	}

	if( pIDMSegment )
	{
		// This Style is playing
		CWaitCursor wait;

		// Persist the Band into a DirectMusicBand object
		IDirectMusicBand* pIDMBand = GetDMBand( pIBandNode );
		if( pIDMBand )
		{
			// Update the Band in the Band Track
			IDirectMusicTrack* pIDMTrack;

			if( SUCCEEDED ( pIDMSegment->GetTrack( CLSID_DirectMusicBandTrack, 1, 0, &pIDMTrack ) ) )
			{
				pIDMTrack->SetParam( GUID_Clear_All_Bands, 0, NULL );
				pIDMTrack->SetParam( GUID_IDirectMusicBand, 0, pIDMBand );

				// Invalidate queued events
				MUSIC_TIME mtTimeNow;
				theApp.m_pStyleComponent->m_pIDMPerformance->GetTime( NULL, &mtTimeNow );
				theApp.m_pStyleComponent->m_pIDMPerformance->Invalidate( mtTimeNow, 0 );

				RELEASE( pIDMTrack );
			}

			// Send the Band
			IDirectMusicSegment* pIDMSegmentBand;

			if( SUCCEEDED ( pIDMBand->CreateSegment( &pIDMSegmentBand ) ) )
			{
				theApp.m_pStyleComponent->m_pIDMPerformance->PlaySegment( pIDMSegmentBand,
										 (DMUS_SEGF_SECONDARY | 0), 0, NULL );
				RELEASE( pIDMSegmentBand );
			}

			RELEASE( pIDMBand );
		}

		RELEASE( pIDMSegment );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::GetActiveBand

IDMUSProdNode* CDirectMusicStyle::GetActiveBand()
{
	return m_pIActiveBandNode;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::GetDMBand

IDirectMusicBand* CDirectMusicStyle::GetDMBand( IDMUSProdNode* pIBandNode )
{
	ASSERT( pIBandNode != NULL );

	IDirectMusicBand* pIDMBand = NULL;
	BOOL fSuccess = FALSE;

	// Persist the Band into a DirectMusicBand object
	if( SUCCEEDED ( ::CoCreateInstance( CLSID_DirectMusicBand, NULL, CLSCTX_INPROC, 
										IID_IDirectMusicBand, (void**)&pIDMBand ) ) )
	{
		IStream* pIMemStream;
		IPersistStream* pIPersistStream;
		IPersistStream* pIPersistStreamEngine;

		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream ) ) )
		{
			if( SUCCEEDED ( pIBandNode->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
			{
				if( SUCCEEDED ( pIPersistStream->Save( pIMemStream, FALSE ) ) )
				{
					if( SUCCEEDED ( pIDMBand->QueryInterface( IID_IPersistStream, (void **)&pIPersistStreamEngine ) ) )
					{
						StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
						if( SUCCEEDED ( pIPersistStreamEngine->Load( pIMemStream ) ) )
						{
							fSuccess = TRUE;
						}

						RELEASE( pIPersistStreamEngine );
					}
				}

				RELEASE( pIPersistStream );
			}

			RELEASE( pIMemStream );
		}
	}

	if( fSuccess == FALSE )
	{
		if( pIDMBand )
		{
			pIDMBand->Release();
			pIDMBand = NULL;
		}
	}

	return pIDMBand;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::IsInStyleBandList

BOOL CDirectMusicStyle::IsInStyleBandList( IDMUSProdNode* pIBandNode )
{
	BOOL fInList = FALSE;
 
	POSITION pos = m_StyleBands.m_lstBands.GetHeadPosition();
    while( pos )
    {
        IDMUSProdNode* pINode = m_StyleBands.m_lstBands.GetNext( pos );

		if( pINode == pIBandNode )
		{
			fInList = TRUE;
			break;
		}
	}

	return fInList;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SetActiveBand

void CDirectMusicStyle::SetActiveBand( IDMUSProdNode* pIBandNode )
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( m_pIActiveBandNode == pIBandNode )
	{
		return;
	}

	// pIBandNode must be in m_StyleBands list of bands
	if( IsInStyleBandList( pIBandNode ) == FALSE )
	{
		return;
	}

	RELEASE( m_pIActiveBandNode );

	if( pIBandNode )
	{
		CDirectMusicPattern* pPattern;
		CDirectMusicPattern* pMotif;

		m_pIActiveBandNode = pIBandNode;
		m_pIActiveBandNode->AddRef();

		SendBand( m_pIActiveBandNode );

		// Refresh all Pattern editors to display proper DLS names
		POSITION pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
		while( pos )
		{
			pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );

			if( pPattern->m_pPatternCtrl
			&&  pPattern->m_pPatternCtrl->m_pPatternDlg )
			{
				pPattern->m_pPatternCtrl->m_pPatternDlg->Invalidate();
			}
		}

		// Refresh all Motif editors to display proper DLS names
		pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
		while( pos )
		{
			pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );

			if( pMotif->m_pPatternCtrl
			&&  pMotif->m_pPatternCtrl->m_pPatternDlg )
			{
				pMotif->m_pPatternCtrl->m_pPatternDlg->Invalidate();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::RemoveActiveBand

void CDirectMusicStyle::RemoveActiveBand( IDMUSProdNode* pIBandNode )
{
	ASSERT( pIBandNode != NULL );

	if( m_pIActiveBandNode == pIBandNode )
	{
		RELEASE( m_pIActiveBandNode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::GetSegmentThatIsPlaying

IDirectMusicSegment* CDirectMusicStyle::GetSegmentThatIsPlaying()
{
	CDirectMusicPattern* pPattern;
	CDirectMusicPattern* pMotif;

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );

	// See if Conductor is playing a Pattern in this Style
	POSITION pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( pos )
	{
		pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );

		if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( pPattern ) == S_OK )
		{
			if( pPattern->m_pIDMSegment )
			{
				pPattern->m_pIDMSegment->AddRef();
				return pPattern->m_pIDMSegment;
			}
		}
	}

	// See if Conductor is playing a Motif in this Style
	pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
	{
		pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );

		if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( pMotif ) == S_OK )
		{
			if( pMotif->m_pIDMSegment )
			{
				pMotif->m_pIDMSegment->AddRef();
				return pMotif->m_pIDMSegment;
			}
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SetTempo

void CDirectMusicStyle::SetTempo( double dblTempo, BOOL fInUndo )
{
	ASSERT( m_pIDMStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );

	if( dblTempo != m_dblTempo )
	{
		// Change tempo if currently auditioning this Style
		IDirectMusicSegment* pIDMSegment = GetSegmentThatIsPlaying();
		if( pIDMSegment )
		{
			// Change tempo now!!!
			DMUS_TEMPO_PMSG* pTempo;

			if( SUCCEEDED( theApp.m_pStyleComponent->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_TEMPO_PMSG),
				(DMUS_PMSG**)&pTempo ) ) )
			{
				// Queue tempo event
				ZeroMemory( pTempo, sizeof(DMUS_TEMPO_PMSG) );
				pTempo->dblTempo = dblTempo;
				pTempo->dwFlags = DMUS_PMSGF_REFTIME;
				pTempo->dwType = DMUS_PMSGT_TEMPO;
				theApp.m_pStyleComponent->m_pIDMPerformance->SendPMsg( (DMUS_PMSG*)pTempo );
			}

			RELEASE( pIDMSegment );
		}

		// Change default tempo of Style
		if( fInUndo == FALSE )
		{
			m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_STYLE_TEMPO );
		}

		m_dblTempo = dblTempo;
		
		CDirectMusicPattern* pPattern;
		CDirectMusicPattern* pMotif;

		// Change timeline "tempo" in all Pattern editors
		POSITION pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
		while( pos )
		{
			pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
			pPattern->SyncTempo();
		}
		
		// Change timeline "tempo" in all Motif editors
		pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
		while( pos )
		{
			pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );
			pMotif->SyncTempo();
		}
		
		// Update DirectMusic Style object
		SyncStyleWithDirectMusic();

		if( fInUndo == FALSE )
		{
			// Refresh Style editor (when open)
			SyncStyleEditor( SSE_STYLE );

			// Sync change with property sheet
			IDMUSProdPropSheet* pIPropSheet;
			if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
			{
				pIPropSheet->RefreshActivePageByObject( this );
				RELEASE( pIPropSheet );
			}
		
			SetModified( TRUE );
		}

		// Notify connected nodes that Style tempo has changed
		theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( this, STYLE_TempoChange, &m_dblTempo );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SetTimeSignature

void CDirectMusicStyle::SetTimeSignature( DirectMusicTimeSig timeSig, BOOL fInUndo )
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( timeSig.m_bBeatsPerMeasure != m_TimeSignature.m_bBeatsPerMeasure
	||  timeSig.m_bBeat != m_TimeSignature.m_bBeat
	||  timeSig.m_wGridsPerBeat != m_TimeSignature.m_wGridsPerBeat )
	{
		CWaitCursor wait;

		CDirectMusicPart* pPart;
		CDirectMusicPattern* pPattern;
		CDirectMusicPattern* pMotif;

		// FIRST!! - Change time signature of all Parts
		POSITION pos = m_lstStyleParts.GetHeadPosition();
		while( pos )
		{
			pPart = m_lstStyleParts.GetNext( pos );
			pPart->SetTimeSignature( timeSig, fInUndo, TRUE );
		}
		
		// Change time signature of all Patterns
		pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
		while( pos )
		{
			pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
			pPattern->SetTimeSignature( timeSig, FALSE );
		}
		
		// Change time signature of all Motifs
		pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
		while( pos )
		{
			pMotif = m_StyleMotifs.m_lstMotifs.GetNext( pos );
			pMotif->SetTimeSignature( timeSig, FALSE );
		}
		
		// Change time signature of Style
		if( fInUndo == FALSE )
		{
			m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_STYLE_TIME_SIGNATURE );
		}
		m_TimeSignature = timeSig;

		if( fInUndo == FALSE )
		{
			// Refresh Style editor (when open)
			SyncStyleEditor( (SSE_STYLE | SSE_PATTERNS | SSE_MOTIFS) );

			// Sync change with property sheet
			IDMUSProdPropSheet* pIPropSheet;
			if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
			{
				pIPropSheet->RefreshActivePageByObject( this );
				RELEASE( pIPropSheet );
			}

			SetModified( TRUE );
			SyncStyleWithDirectMusic();
		}

		// Notify connected nodes that Style time signature has changed
		theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( this, STYLE_TimeSigChange, &m_TimeSignature );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::GetTheDefaultBand

IDMUSProdNode* CDirectMusicStyle::GetTheDefaultBand()
{
	IDMUSProdBandEdit* pIBandEdit;
	IDMUSProdNode* pINode;
	BOOL fDefaultFlag;

    POSITION pos = m_StyleBands.m_lstBands.GetHeadPosition();
	while( pos )
    {
        pINode = m_StyleBands.m_lstBands.GetNext( pos );

		if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdBandEdit, (void**)&pIBandEdit ) ) )
		{
			if( FAILED ( pIBandEdit->GetDefaultFlag( &fDefaultFlag ) ) )
			{
				fDefaultFlag = FALSE;
			}

			RELEASE( pIBandEdit );

			if( fDefaultFlag )
			{
				return pINode;
			}
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SetDefaultBand

void CDirectMusicStyle::SetDefaultBand( IDMUSProdNode* pIDefaultBandNode, BOOL fInUndo )
{
	IDMUSProdBandEdit* pIBandEdit;
	IDMUSProdNode* pINode;

    POSITION pos = m_StyleBands.m_lstBands.GetHeadPosition();
	while( pos )
    {
        pINode = m_StyleBands.m_lstBands.GetNext( pos );

		if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdBandEdit, (void**)&pIBandEdit ) ) )
		{
			pIBandEdit->SetDefaultFlag( pINode == pIDefaultBandNode );
			RELEASE( pIBandEdit );
		}
    }

	if( pIDefaultBandNode == NULL )
	{
		// Set first Band to be the "Default" Band
		pos = m_StyleBands.m_lstBands.GetHeadPosition();
		if( pos )
		{
			pINode = m_StyleBands.m_lstBands.GetNext( pos );

			if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdBandEdit, (void**)&pIBandEdit ) ) )
			{
				pIBandEdit->SetDefaultFlag( TRUE );
				RELEASE( pIBandEdit );
			}
		}
	}

	if( fInUndo == FALSE )
	{
		// Refresh Style editor (when open)
		SyncStyleEditor( SSE_STYLE );
	
		SetModified( TRUE );
		SyncStyleWithDirectMusic();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// AddOffset, correct time, offset for specified meter
static void AddOffsets(FullSeqEvent* lstEvent, DirectMusicTimeSig TimeSig)
//static void AddOffsets(FullSeqEvent* lstEvent, DMUS_IO_TIMESIGNATURE_ITEM timesig)
//
//	Add offsets relative to measure/beat/grid
//
{
//	HRESULT hr;
//	timesig.bBeat = 4;
//	timesig.bBeatsPerMeasure =  4;
//	timesig.wGridsPerBeat = 4;
//	timesig.lTime = 0;
	short nClocksPerGrid = static_cast<short>(((DMUS_PPQ * 4) / TimeSig.m_bBeat) / TimeSig.m_wGridsPerBeat);
	//short nClocksPerGrid = static_cast<short>(((DMUS_PPQ * 4) / timesig.bBeat) / timesig.wGridsPerBeat);


	for( FullSeqEvent* pEvent = lstEvent; pEvent; pEvent = pEvent->pNext )
	{
		if ( ( pEvent->bStatus & 0xf0 ) == MIDI_NOTEON )
		{
			long lGrid = TS_GRID_TO_CLOCKS( TS_CLOCKS_TO_GRID( pEvent->mtTime, TimeSig ), TimeSig );
			pEvent->nOffset = (short)(pEvent->mtTime - lGrid);
			//pEvent->nOffset = (short) ((pEvent->mtTime - timesig.lTime) % nClocksPerGrid);
			pEvent->mtTime -= pEvent->nOffset;
			if (pEvent->nOffset > (nClocksPerGrid / 2))
			{
				// make it a negative offset and bump the time a corresponding amount
				pEvent->nOffset = static_cast<short>(pEvent->nOffset - nClocksPerGrid);
				pEvent->mtTime += nClocksPerGrid;
			}
		}
	}

}




/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::CreatePatternFromMIDIStream

bool gbIsXG;

bool IsXG()
{
	return gbIsXG;
}

HRESULT CDirectMusicStyle::CreatePatternFromMIDIStream(CDirectMusicPattern* pPattern, IDMUSProdNode* pBandNode, LPSTREAM pStream, CString strFile)
{

	gbIsXG = false;

	if(pPattern == NULL || pBandNode == NULL, pStream == NULL)
	{
		return E_POINTER;
	}

	HRESULT hr = S_OK;
    DWORD dwID;
    DWORD dwCurTime;
    DWORD dwLength;
	DWORD dwSize;
    short nFormat;
    short nNumTracks;
    short nTracksRead;
    FullSeqEvent** lstEvent;
    LPSTR* lstTrackNames = NULL;
	DMUS_IO_PATCH_ITEM* lstPatchEvent;
	FullSeqEvent* lstTrackEvent;
	IDMUSProdRIFFStream*	pRiffStream = 0;

	/*
	MIDITimeSigFlag() = 1;
	MIDITimeSig().lTime = 0;
	MIDITimeSig().bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
	MIDITimeSig().bBeat = m_TimeSignature.m_bBeat;
	MIDITimeSig().wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;
	*/

    lstPatchEvent = NULL;
    nNumTracks = nTracksRead = 0;
    dwLength = 0;
	DWORD dwMaxLength = 0;
	long iTrack = 0;

	MIDIInit();
//	MIDIInitBankSelect();
//	MIDIInitChordOfComposition();

    if( ( S_OK != pStream->Read( &dwID, sizeof( FOURCC ), NULL ) ) ||
		!GetMLong( pStream, dwSize ) )
    {
		return E_FAIL;
    }
// check for RIFF MIDI files
    if( dwID == mmioFOURCC( 'R', 'I', 'F', 'F' ) )
    {
		StreamSeek( pStream, 12, STREAM_SEEK_CUR );
		if( ( S_OK != pStream->Read( &dwID, sizeof( FOURCC ), NULL ) ) ||
			!GetMLong( pStream, dwSize ) )
		{
			return E_FAIL;
		}
    }
// check for normal MIDI files
	if( dwID != mmioFOURCC( 'M', 'T', 'h', 'd' ) )
	{
		return E_FAIL;
	}

    GetMShort( pStream, nFormat );
    GetMShort( pStream, nNumTracks );
    
	lstEvent = new FullSeqEvent*[nNumTracks];
	if( lstEvent == NULL )
	{
		return E_OUTOFMEMORY;
	}
	for( int i=0 ;  i < nNumTracks ; i++ )
	{
		lstEvent[i] = NULL;
	}

    lstTrackNames = new LPSTR[nNumTracks];
	if( lstTrackNames == NULL )
	{
		return E_OUTOFMEMORY;
	}
	for( i=0 ;  i < nNumTracks ; i++ )
	{
		lstTrackNames[i] = NULL;
	}

    GetMShort( pStream, MIDIsnPPQN() );
    if( dwSize > 6 )
    {
		StreamSeek( pStream, dwSize - 6, STREAM_SEEK_CUR );
    }
    pStream->Read( &dwID, sizeof( FOURCC ), NULL );
    while( dwID == mmioFOURCC( 'M', 'T', 'r', 'k' ) )
    {
		GetMLong( pStream, dwSize );
		dwCurTime = 0;
		lstTrackEvent = NULL;
		while( dwSize > 0 )
		{
			dwSize -= MIDIGetVarLength( pStream, dwID );
			dwCurTime += dwID;
			DWORD localSize = MIDIReadEvent( pStream, dwCurTime, &lstTrackEvent, &lstPatchEvent, &lstTrackNames[nTracksRead] );
			if(localSize == 0)
			{
				HINSTANCE hInstance = AfxGetResourceHandle();
				AfxSetResourceHandle( theApp.m_hInstance );

				CString strMsg;
				AfxFormatString1( strMsg, IDS_ERR_CANNOT_IMPORT_FILE, strFile );
				AfxMessageBox( strMsg );

				AfxSetResourceHandle( hInstance );
				hr = E_FAIL;
				// lstPatchEvent and rest get deleted below.  Since this guy becomes part of lstEvent if
				// we're successful, we can't delete him below, so we delete him here.
				List_Free(lstTrackEvent);
				goto EXIT;
			}
			else
			{
				dwSize -= localSize;
			}
		}
		/*
		if( MIDITimeSigFlag() > 1 )
		{
			// if glTimeSig is greater than 1, it means we've read some time sigs
			// from this track (it was set to 2 inside ReadEvent.) This means that
			// we no longer want ReadEvent to pay any attention to time sigs, so
			// we set this to 0.
			MIDITimeSigFlag() = 0;
		}
		*/
		if( dwCurTime > dwLength )
		{
			dwLength = dwCurTime;
		}

		// Remove duplicated controller messages - lstOther must be in reverse order for this to work
		lstTrackEvent = MIDIRemoveDuplicateCCs( lstTrackEvent );
		lstTrackEvent = MIDIReverseEventList( lstTrackEvent );
		lstTrackEvent = MIDICompressEventList( lstTrackEvent );
//		lstEvent = List_Cat( lstEvent, lstTrackEvent );
	    lstEvent[nTracksRead] = MIDISortEventList( lstTrackEvent );
		AddOffsets(lstEvent[nTracksRead], m_TimeSignature);
		if( FAILED( pStream->Read( &dwID, sizeof( FOURCC ), NULL ) ) )
		{
			break;
		}
		nTracksRead++;
    }
	dwLength = MIDIConvertTime(dwLength);

//    lstEvent= MIDISortEventList( lstEvent );

	//AddOffsets(lstEvent, MIDITimeSig());

	DWORD dwClocksPerMeasure;
	MIDIGetChordOfComposition(	pPattern->m_dwDefaultKeyPattern,
								pPattern->m_dwDefaultChordPattern,
								pPattern->m_bDefaultKeyRoot,
								pPattern->m_bDefaultChordRoot,
								pPattern->m_fKeyFlatsNotSharps,
								pPattern->m_fChordFlatsNotSharps);

	for(iTrack = 0; pPattern && iTrack < nNumTracks; iTrack++)
	{
		if(iTrack == 0 && lstPatchEvent && (hr == S_OK))
		{
			IStream* pStream = 0;
			hr = theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStream );
			if(SUCCEEDED(hr))
			{
				hr = AllocRIFFStream( pStream, &pRiffStream );
				if(SUCCEEDED(hr))
				{
					hr = WriteBandTrack(pRiffStream, &lstPatchEvent, nNumTracks, lstEvent);
					if(SUCCEEDED(hr))
					{
						// stream node into band
						StreamSeek(pStream, 0, STREAM_SEEK_SET);
						STATSTG stg;
						hr = pStream->Stat(&stg, STATFLAG_NONAME);
						if(hr == S_OK)
						{
							IPersistStream* pIPS = NULL;
							hr = pBandNode->QueryInterface(IID_IPersistStream, (void **)&pIPS);
							if(SUCCEEDED(hr))
							{
								pIPS->Load(pStream);
								pIPS->Release();
							}
						}
					}
				}
				pRiffStream->Release();
				pStream->Release();
			}
		}

		// Populate Parts
		//
		if(hr == S_OK)
		{
			
			dwLength = 0;

			if(iTrack == 0)
			{
				// set up time grid
				pPattern->m_TimeSignature.m_bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
				pPattern->m_TimeSignature.m_bBeat = m_TimeSignature.m_bBeat;
				pPattern->m_TimeSignature.m_wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;
				DWORD dwClocksPerBeat = DM_PPQNx4 / pPattern->m_TimeSignature.m_bBeat;
				dwClocksPerMeasure = dwClocksPerBeat * (DWORD)pPattern->m_TimeSignature.m_bBeatsPerMeasure;
			}
			// potentially 16 parts
			CDirectMusicPart* apPart[NUM_MIDI_CHANNELS];
			int iPart = 0;
			for(iPart; iPart < NUM_MIDI_CHANNELS; iPart++)
			{
				apPart[iPart] = 0;
			}

			// scan event list
			FullSeqEvent* pEvent;
			for( pEvent = lstEvent[iTrack]; pEvent; pEvent = pEvent->pNext )
			{
				if( dwLength < (DWORD)(pEvent->mtTime + pEvent->nOffset + pEvent->mtDuration) )
				{
					dwLength = pEvent->mtTime + pEvent->nOffset + pEvent->mtDuration;
				}
				// convert event
				CDirectMusicEventItem* pDirectMusicEventItem = pPattern->MakeDirectMusicEventItem(pEvent);
				int nCh = pEvent->dwPChannel;
				if(apPart[nCh] == 0)
				{
					apPart[nCh] = AllocPart();
					// disable all variations except for 1
					for(int j = 0; j < NBR_VARIATIONS; j++)
					{
						if(j == 0)
						{
							apPart[nCh]->m_dwVariationChoices[j] = (DM_VF_MODE_DMUSIC | DM_VF_FLAG_BITS);	// Enable variations (DirectMusic mode)
						}
						else
						{
							apPart[nCh]->m_dwVariationChoices[j] = DM_VF_MODE_DMUSIC;	// Disable variations (DirectMusic mode)
						}
					}
					if(nCh == 9 /* drum channel*/)
					{
						apPart[nCh]->m_bPlayModeFlags = DMUS_PLAYMODE_FIXED;
					}
					else
					{
						apPart[nCh]->m_bPlayModeFlags = DMUS_PLAYMODE_PURPLEIZED;
					}
					CDirectMusicPartRef* pRef = pPattern->AllocPartRef();
					ASSERT(pRef);
					if(!pRef)
					{
						hr = E_FAIL;
						return hr;
					}
					pRef->SetPart(apPart[nCh]);
					pRef->SetPChannel(nCh);
					pRef->SetName(lstTrackNames[iTrack]);
				}

				if( pDirectMusicEventItem->m_bType == ET_CURVE )
				{
					CDirectMusicStyleCurve* pDMCurve = (CDirectMusicStyleCurve *)pDirectMusicEventItem;
					pDMCurve->SetDefaultResetValues( 0 );	// Correct reset duration will be set in SetNbrMeasures()

				}
				apPart[nCh]->m_lstEvents.AddTail(pDirectMusicEventItem);
			}
		}
		if(dwLength > dwMaxLength)
			dwMaxLength = dwLength;
		dwLength = 0;
	}
	if(!pPattern)
	{
		hr = E_POINTER;
	}
	else
	{
		// set the length of the Pattern. Round up to the next measure
		WORD wMeasures = static_cast<unsigned short>(dwMaxLength / dwClocksPerMeasure);
		if(dwMaxLength % dwClocksPerMeasure)
		{
			wMeasures++;
		}
		pPattern->SetNbrMeasures(wMeasures);
	}
EXIT:

	for(int jTrack = 0; jTrack < nNumTracks; jTrack++)
	{
		List_Free( lstEvent[jTrack] );
	}

	delete lstEvent;
	lstEvent = 0;
	List_Free( lstPatchEvent );

	if( lstTrackNames )
	{
		for( jTrack = 0; jTrack < nNumTracks; jTrack++)
		{
			if( lstTrackNames[jTrack] )
			{
				delete lstTrackNames[jTrack];
				lstTrackNames[jTrack] = NULL;
			}
		}
		delete lstTrackNames;
		lstTrackNames = NULL;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::ImportMidiFileAsPattern

HRESULT CDirectMusicStyle::ImportMidiFileAsPattern( BOOL fMotif )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = S_OK;
	CString strFileNames, strFileName, strPrompt;

	BOOL succeeded = FALSE;

	if( fMotif )
	{
		strPrompt.LoadString( IDS_IMPORT_MOTIF_FROMMIDI );
	}
	else
	{
		strPrompt.LoadString( IDS_IMPORT_PATTERN_FROMMIDI );
	}

	CString	strExt, strDefaultExt, strFilterDesc;
	CFileDialog dlg(TRUE);

	if(strExt.LoadString(IDS_MIDI_FILEEXT)
	&& strDefaultExt.LoadString(IDS_MIDI_DEFAULT_FILEEXT))
	{
		if(!strPrompt.IsEmpty())
		{
			dlg.m_ofn.lpstrTitle = strPrompt;
		}
		if(strFilterDesc.LoadString(IDS_MIDI_FILEDESC))
		{
			dlg.m_ofn.lpstrDefExt = strDefaultExt;
			dlg.m_ofn.nFilterIndex = dlg.m_ofn.nMaxCustFilter + 1;  // 1 based number

			// add *.mid to filter
			ASSERT(!strFilterDesc.IsEmpty());   // must have a file type name
			strFilterDesc += (TCHAR)'\0';		// next string please
			strFilterDesc += strExt;
			strFilterDesc += (TCHAR)'\0';		// next string please
			dlg.m_ofn.nMaxCustFilter++;

			// Append the "*.*" all files filter
			CString allFilter;
			VERIFY( allFilter.LoadString( AFX_IDS_ALLFILTER ) );
			strFilterDesc += allFilter;
			strFilterDesc += (TCHAR)'\0';   // next string please
			strFilterDesc += _T("*.*");
			strFilterDesc += (TCHAR)'\0';   // last string
			dlg.m_ofn.lpstrFilter = strFilterDesc;
			dlg.m_ofn.nMaxCustFilter++;

			// Set File Open dialog flags
			dlg.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT );

			dlg.m_ofn.lpstrFile = strFileNames.GetBuffer(_MAX_PATH);
			succeeded = dlg.DoModal() == IDOK;
			strFileNames.ReleaseBuffer();
		}
	}

	if(succeeded)
	{
		// Fix 23679: Display wait cursor during import
		CWaitCursor wait;

		POSITION pos = dlg.GetStartPosition();
		while( pos )
		{
			strFileName = dlg.GetNextPathName( pos );

			// Determine name of MIDI file
			CString strName;
			BSTR bstrName;
			TCHAR achName[_MAX_FNAME];
			_tsplitpath( strFileName, NULL, NULL, achName, NULL );
			strName = achName;
			strName.TrimLeft();
			strName.TrimRight();

			CDirectMusicPattern* pPattern = new CDirectMusicPattern(this,  fMotif);
			ASSERT(pPattern);
			if(pPattern)
			{
				IDMUSProdDocType* pIDocType;
				IDMUSProdNode* pBandNode = NULL;
				pPattern->Initialize1();	// creates rhythm map
				hr = theApp.m_pStyleComponent->m_pIFramework->FindDocTypeByNodeId( GUID_BandNode, &pIDocType );
				if( SUCCEEDED ( hr ) ) 
				{
					pIDocType->AllocNode( GUID_BandNode, &pBandNode );
					if( !SUCCEEDED ( hr ) )
					{
						pBandNode = NULL;
					}
					RELEASE( pIDocType );
				}

				if( pBandNode == NULL )
				{
					delete pPattern;
					return E_OUTOFMEMORY;
				}
				// make a stream for file
				BSTR bstrPathName = strFileName.AllocSysString();
				IStream* pIStream;
				if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AllocFileStream(bstrPathName, GENERIC_READ, FT_UNKNOWN,
														  GUID_AllZeros, NULL, &pIStream) ) )
				{
					hr = CreatePatternFromMIDIStream( pPattern, pBandNode, pIStream, strFileName );
					if( SUCCEEDED ( hr ) )
					{
						CString strBandName = strName;
						if( strBandName.GetLength() > DMUS_MAX_NAME )
						{
							strBandName = strBandName.Left( DMUS_MAX_NAME );
							strBandName.TrimRight();
						}
						bstrName = strBandName.AllocSysString();
						pBandNode->SetNodeName( bstrName );
						GetUniqueBandName( pBandNode );
						m_StyleBands.InsertChildNode( pBandNode );
						
						bstrName = strName.AllocSysString();
						pPattern->SetNodeName( bstrName );
						GetUniquePatternName( pPattern );
						if( fMotif )
						{
							// Convert the Pattern into a Motif
							pPattern->m_wEmbellishment = EMB_MOTIF;
							pPattern->m_bGrooveBottom = 1; 
							pPattern->m_bGrooveTop = 100;
							pPattern->m_bDestGrooveBottom = 1; 
							pPattern->m_bDestGrooveTop = 100;
							m_StyleMotifs.InsertChildNode( pPattern );
						}
						else
						{
							m_StylePatterns.InsertChildNode( pPattern );
						}

						pPattern->Release();	// needed as AddPattern (called by InsertChildNode) does a ref
						pBandNode->Release();	// ditto, this time CStyleBands::InsertChildNode is the culprit
						SetActiveBand( pBandNode );
						if( m_pStyleCtrl &&  m_pStyleCtrl->m_pStyleDlg )
						{
								m_pStyleCtrl->m_pStyleDlg->SelectBand( pBandNode );
						}				
					}
					else
					{
						pPattern->Release();
						pBandNode->Release();
					}

					pIStream->Release();
				}
			}

			if( FAILED ( hr ) )
			{
				break;
			}
		}
	}
	else
	{
		hr = E_FAIL;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::UpdateLinkedParts

void CDirectMusicStyle::UpdateLinkedParts( CDirectMusicPartRef* pPartRef, LPCTSTR pcstrText )
{
	ASSERT( pPartRef );
	ASSERT( pPartRef->m_pDMPart );
	if( ::IsEqualGUID( pPartRef->m_guidOldPartID, GUID_AllZeros  ) )
	{
		//ASSERT( FALSE );
		return;
	}

	CDirectMusicPattern* pPattern;
	POSITION pos;
	pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( pos )
	{
		pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
		if( pPattern != pPartRef->m_pPattern )
		{
			pPattern->UpdateLinkedParts( pPartRef, pcstrText );
		}
	}

	pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
	{
		pPattern = m_StyleMotifs.m_lstMotifs.GetNext( pos );
		if( pPattern != pPartRef->m_pPattern )
		{
			pPattern->UpdateLinkedParts( pPartRef, pcstrText );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::UpdateLinkFlags

void CDirectMusicStyle::UpdateLinkFlags()
{
	CDirectMusicPattern* pPattern;
	POSITION pos;
	pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( pos )
	{
		pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
		pPattern->UpdateLinkFlags( );
	}

	pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
	{
		pPattern = m_StyleMotifs.m_lstMotifs.GetNext( pos );
		pPattern->UpdateLinkFlags( );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::OnOutputPortsChanged

void CDirectMusicStyle::OnOutputPortsChanged()
{
	POSITION pos;
	CDirectMusicPattern *pPattern;
	IDMUSProdPortNotify *pIPortNotify;

    pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( pos )
    {
        pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
		if( pPattern &&
			pPattern->m_pPatternCtrl &&
			pPattern->m_pPatternCtrl->m_pPatternDlg )
		{
			if( pPattern->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr &&
				SUCCEEDED( pPattern->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr->QueryInterface( IID_IDMUSProdPortNotify, (void**)&pIPortNotify ) ) )
			{
				pIPortNotify->OnOutputPortsChanged();
				pIPortNotify->Release();
			}

			// If the window is active, play the pattern's band
			if( pPattern->m_pPatternCtrl->m_fDocWindowActive 
			&&	pPattern->m_fDialogActive )
			{
				MUSIC_TIME mtNow;
				if( SUCCEEDED( theApp.m_pStyleComponent->m_pIDMPerformance->GetTime( NULL, &mtNow ) ) )
				{
					IDirectMusicSegmentState *pSegmentState = NULL;
					if( FAILED( theApp.m_pStyleComponent->m_pIDMPerformance->GetSegmentState( &pSegmentState, mtNow ) ) )
					{
						pPattern->PlayBand();
					}
					else
					{
						if( pSegmentState )
						{
							pSegmentState->Release();
						}
					}
				}
			}
		}
	}

	pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
	{
		pPattern = m_StyleMotifs.m_lstMotifs.GetNext( pos );
		if( pPattern &&
			pPattern->m_pPatternCtrl &&
			pPattern->m_pPatternCtrl->m_pPatternDlg &&
			pPattern->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr &&
			SUCCEEDED( pPattern->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr->QueryInterface( IID_IDMUSProdPortNotify, (void**)&pIPortNotify ) ) )
		{
			pIPortNotify->OnOutputPortsChanged();
			pIPortNotify->Release();

			// If the window is active, play the pattern's band
			if( pPattern->m_pPatternCtrl->m_fDocWindowActive 
			&&	pPattern->m_fDialogActive )
			{
				MUSIC_TIME mtNow;
				if( SUCCEEDED( theApp.m_pStyleComponent->m_pIDMPerformance->GetTime( NULL, &mtNow ) ) )
				{
					IDirectMusicSegmentState *pSegmentState = NULL;
					if( FAILED( theApp.m_pStyleComponent->m_pIDMPerformance->GetSegmentState( &pSegmentState, mtNow ) ) )
					{
						pPattern->PlayBand();
					}
					else
					{
						if( pSegmentState )
						{
							pSegmentState->Release();
						}
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::OnOutputPortsRemoved

void CDirectMusicStyle::OnOutputPortsRemoved()
{
	POSITION pos;
	CDirectMusicPattern *pPattern;
	IDMUSProdPortNotify *pIPortNotify;

    pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( pos )
    {
        pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
		if( pPattern &&
			pPattern->m_pPatternCtrl &&
			pPattern->m_pPatternCtrl->m_pPatternDlg &&
			pPattern->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr &&
			SUCCEEDED( pPattern->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr->QueryInterface( IID_IDMUSProdPortNotify, (void**)&pIPortNotify ) ) )
		{
			pIPortNotify->OnOutputPortsRemoved();
			pIPortNotify->Release();
		}
	}

	pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
	{
		pPattern = m_StyleMotifs.m_lstMotifs.GetNext( pos );
		if( pPattern &&
			pPattern->m_pPatternCtrl &&
			pPattern->m_pPatternCtrl->m_pPatternDlg &&
			pPattern->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr &&
			SUCCEEDED( pPattern->m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr->QueryInterface( IID_IDMUSProdPortNotify, (void**)&pIPortNotify ) ) )
		{
			pIPortNotify->OnOutputPortsRemoved();
			pIPortNotify->Release();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::UpdateMIDIMgrsAndChangePartPtrs

void CDirectMusicStyle::UpdateMIDIMgrsAndChangePartPtrs( CDirectMusicPart* pOldPart, CDirectMusicPartRef* pNewPartRef, LPCTSTR pcstrText )
{
	ASSERT( pOldPart );
	ASSERT( pNewPartRef );

	CDirectMusicPattern* pPattern;
	POSITION pos;
	pos = m_StylePatterns.m_lstPatterns.GetHeadPosition();
	while( pos )
	{
		pPattern = m_StylePatterns.m_lstPatterns.GetNext( pos );
		if( pPattern != pNewPartRef->m_pPattern )
		{
			pPattern->UpdateMIDIMgrsAndChangePartPtrs( pOldPart, pNewPartRef->m_pDMPart, pcstrText );
		}
	}

	pos = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
	while( pos )
	{
		pPattern = m_StyleMotifs.m_lstMotifs.GetNext( pos );
		if( pPattern != pNewPartRef->m_pPattern )
		{
			pPattern->UpdateMIDIMgrsAndChangePartPtrs( pOldPart, pNewPartRef->m_pDMPart, pcstrText );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::PreChangePartRef

void CDirectMusicStyle::PreChangePartRef( CDirectMusicPartRef* pDMPartRef )
{
	ASSERT( pDMPartRef != NULL );
	ASSERT( pDMPartRef->m_pDMPart != NULL );
	ASSERT( pDMPartRef->m_pDMPart->m_pStyle != NULL );

	// If other PartRef's use the part that will change
	if( pDMPartRef->m_pDMPart->m_dwUseCount > 1 )
	{
		// If we're not linked to the part
		if( !pDMPartRef->m_fHardLink )
		{
			// Create a new part
			CDirectMusicPart *pDMPart = AllocPart();
			if( pDMPart )
			{
				// Copy the existing part to the new part
				pDMPartRef->m_pDMPart->CopyTo( pDMPart );

				// If the variation choices editor is pointing to this partref
				if( pDMPartRef->m_pDMPart->m_pVarChoicesPartRef == pDMPartRef )
				{
					// Move the variation choices editor to the new part
					pDMPart->m_pVarChoicesNode = pDMPartRef->m_pDMPart->m_pVarChoicesNode;
					pDMPart->m_pVarChoicesPartRef = pDMPartRef;

					// Remove the variation choices editor from the old part
					pDMPartRef->m_pDMPart->m_pVarChoicesNode = NULL;
					pDMPartRef->m_pDMPart->m_pVarChoicesPartRef = NULL;
				}

				// Point ourself to the new part
				pDMPartRef->SetPart( pDMPart );
			}
		}
		// If we ARE linked to the part
		else // pPartRef->m_fHardLink IS set
		{
			// Check if all other PartRefs are also linked to our part
			BOOL fOtherHardLinkNotSet = FALSE;

			// Iterate through all Patterns
			POSITION posPattern;
			posPattern = m_StylePatterns.m_lstPatterns.GetHeadPosition();
			while( posPattern && !fOtherHardLinkNotSet )
			{
				CDirectMusicPattern *pPatternList = m_StylePatterns.m_lstPatterns.GetNext( posPattern );
			
				// Iterate through all PartRefs
				POSITION posPartRef;
				posPartRef = pPatternList->m_lstPartRefs.GetHeadPosition();
				while( posPartRef != NULL )
				{
					CDirectMusicPartRef *pTmpPartRef = pPatternList->m_lstPartRefs.GetNext( posPartRef );

					// If this PartRef points to the same part, and does NOT
					// have the m_fHardLink set, set fOtherHardLinkNotSet and
					// break out
					if( (pTmpPartRef->m_pDMPart == pDMPartRef->m_pDMPart) &&
						!pTmpPartRef->m_fHardLink )
					{
						fOtherHardLinkNotSet = TRUE;
						break;
					}
				}
			}

			// Iterate through all Motifs
			posPattern = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
			while( posPattern && !fOtherHardLinkNotSet )
			{
				CDirectMusicPattern *pPatternList = m_StyleMotifs.m_lstMotifs.GetNext( posPattern );
			
				// Iterate through all PartRefs
				POSITION posPartRef;
				posPartRef = pPatternList->m_lstPartRefs.GetHeadPosition();
				while( posPartRef != NULL )
				{
					CDirectMusicPartRef *pTmpPartRef = pPatternList->m_lstPartRefs.GetNext( posPartRef );

					// If this PartRef points to the same part, and does NOT
					// have the m_fHardLink set, set fOtherHardLinkNotSet and
					// break out
					if( (pTmpPartRef->m_pDMPart == pDMPartRef->m_pDMPart) &&
						!pTmpPartRef->m_fHardLink )
					{
						fOtherHardLinkNotSet = TRUE;
						break;
					}
				}
			}

			// Another PartRef references this part, but does not have the
			// m_fHardLink flag set
			if( fOtherHardLinkNotSet )
			{
				// Create a new part
				CDirectMusicPart *pDMPart = AllocPart();
				if( pDMPart )
				{
					// Copy the existing part to the new part
					pDMPartRef->m_pDMPart->CopyTo( pDMPart );

					// Store the old variation choices partref
					const CDirectMusicPartRef *pOldVarChoicesPartRef = pDMPartRef->m_pDMPart->m_pVarChoicesPartRef;

					// Iterate through all Patterns
					posPattern = m_StylePatterns.m_lstPatterns.GetHeadPosition();
					while( posPattern )
					{
						CDirectMusicPattern *pPatternList = m_StylePatterns.m_lstPatterns.GetNext( posPattern );
					
						// Iterate through all PartRefs
						POSITION posPartRef;
						posPartRef = pPatternList->m_lstPartRefs.GetHeadPosition();
						while( posPartRef != NULL )
						{
							CDirectMusicPartRef *pTmpPartRef = pPatternList->m_lstPartRefs.GetNext( posPartRef );

							// Move all hard-linked partrefs to the new part
							if( pTmpPartRef->m_fHardLink )
							{
								// If the variation choices editor is pointing to this partref
								if( pOldVarChoicesPartRef == pTmpPartRef )
								{
									// Move the variation choices editor to the new part
									pDMPart->m_pVarChoicesNode = pTmpPartRef->m_pDMPart->m_pVarChoicesNode;
									pDMPart->m_pVarChoicesPartRef = pTmpPartRef;

									// Remove the variation choices editor from the old part
									pTmpPartRef->m_pDMPart->m_pVarChoicesNode = NULL;
									pTmpPartRef->m_pDMPart->m_pVarChoicesPartRef = NULL;
								}

								pTmpPartRef->SetPart( pDMPart );
							}
						}
					}

					// Iterate through all Motifs
					posPattern = m_StyleMotifs.m_lstMotifs.GetHeadPosition();
					while( posPattern )
					{
						CDirectMusicPattern *pPatternList = m_StyleMotifs.m_lstMotifs.GetNext( posPattern );
					
						// Iterate through all PartRefs
						POSITION posPartRef;
						posPartRef = pPatternList->m_lstPartRefs.GetHeadPosition();
						while( posPartRef != NULL )
						{
							CDirectMusicPartRef *pTmpPartRef = pPatternList->m_lstPartRefs.GetNext( posPartRef );

							// Move all hard-linked partrefs to the new part
							if( pTmpPartRef->m_fHardLink )
							{
								// If the variation choices editor is pointing to this partref
								if( pOldVarChoicesPartRef == pTmpPartRef )
								{
									// Move the variation choices editor to the new part
									pDMPart->m_pVarChoicesNode = pTmpPartRef->m_pDMPart->m_pVarChoicesNode;
									pDMPart->m_pVarChoicesPartRef = pTmpPartRef;

									// Remove the variation choices editor from the old part
									pTmpPartRef->m_pDMPart->m_pVarChoicesNode = NULL;
									pTmpPartRef->m_pDMPart->m_pVarChoicesPartRef = NULL;
								}

								pTmpPartRef->SetPart( pDMPart );
							}
						}
					}
				}
			}
		}
	}

	// Update Part's GUID
	GUID newGuid;
	if( SUCCEEDED(CoCreateGuid( &newGuid )) )
	{
		// Set m_guidOldPartID if it is unset
		if( pDMPartRef->m_fSetGUIDOldPartID )
		{
			memcpy( &pDMPartRef->m_guidOldPartID, &pDMPartRef->m_pDMPart->m_guidPartID, sizeof(GUID) );
			pDMPartRef->m_fSetGUIDOldPartID = FALSE;
		}

		// Just copy the new guid 
		memcpy( &pDMPartRef->m_pDMPart->m_guidPartID, &newGuid, sizeof(GUID) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::GetObjectDescriptor

HRESULT CDirectMusicStyle::GetObjectDescriptor( void* pObjectDesc )
{
	// Make sure method was passed a valid DMUS_OBJECTDESC pointer
	if( pObjectDesc == NULL ) 
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	DMUS_OBJECTDESC *pDMObjectDesc = (DMUS_OBJECTDESC *)pObjectDesc;

	if( pDMObjectDesc->dwSize == 0 ) 
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	}

	// Initialize DMUS_OBJECTDESC structure
	DWORD dwOrigSize = pDMObjectDesc->dwSize;
	memset( pDMObjectDesc, 0, dwOrigSize );
	pDMObjectDesc->dwSize = dwOrigSize;

	// Set values in DMUS_OBJECTDESC structure
	pDMObjectDesc->dwValidData = (DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS | DMUS_OBJ_VERSION | DMUS_OBJ_NAME );
	
	memcpy( &pDMObjectDesc->guidObject, &m_guidStyle, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicStyle, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = m_dwVersionMS;
	pDMObjectDesc->vVersion.dwVersionLS = m_dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, m_strName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::GetGUID

void CDirectMusicStyle::GetGUID( GUID* pguidStyle )
{
	if( pguidStyle )
	{
		*pguidStyle = m_guidStyle;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyle::SetGUID

void CDirectMusicStyle::SetGUID( GUID guidStyle )
{
	m_guidStyle = guidStyle;

	// Sync Style with DirectMusic
	SetModified( TRUE );
	SyncStyleWithDirectMusic();

	// Notify connected nodes that Style GUID has changed
	theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( this, DOCROOT_GuidChange, NULL );
}
