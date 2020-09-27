// Personality.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include <mmsystem.h>
#pragma warning(default:4201)
#include <mmreg.h>

#include "PersonalityDesigner.h"
#include "StyleDesigner.h"
#include "ChordMapStripMgr.h"
#include "PersonalityCtl.h"
#include "Personality.h"
#include "RiffStructs.h"					// Definition of ioPersonality.
#include "StyleRIFFID.h"					// Definition of FOURCC Macros.
#include "templates.h"
#include "PersonalityRiff.h"
#include "ChordDatabase.h"
#include "SignPostDialog.h"
#include <SegmentGuids.h>

#include "ChordBuilder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma warning(disable:4189)

// {BF272F00-F851-11d0-89AE-00A0C9054129}
static const GUID GUID_PersonalityPageManager = 
{ 0xBF272F00, 0xF851, 0x11d0, { 0x89, 0xAE, 0x00, 0xA0, 0xC9, 0x05, 0x41, 0x29 } };





short CPersonalityPageManager::sm_nActiveTab = 0;

//////////////////////////////////////////////////////////////////////
// CPersonalityPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPersonalityPageManager::CPersonalityPageManager( IDMUSProdFramework* pIFramework, CPersonality* pPersonality ) 
: CDllBasePropPageManager(), m_pPersonality(pPersonality)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pIFramework != NULL );

	m_GUIDManager = GUID_PersonalityPageManager;

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	m_pTabPersonality = NULL;
	m_pTabAudition = NULL;
	m_pTabChordPalette = NULL;
	m_pTabPersonalityInfo = NULL;
}

CPersonalityPageManager::~CPersonalityPageManager()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( m_pIFramework )
	{
		m_pIFramework->Release();
	}

	if( m_pTabPersonality )
	{
		delete m_pTabPersonality;
	}

	if( m_pTabAudition )
	{
		delete m_pTabAudition;
	}

	if( m_pTabChordPalette )
	{
		delete m_pTabChordPalette;
	}

	if(m_pTabPersonalityInfo)
	{
		delete m_pTabPersonalityInfo;
	}

}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonalityPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CPersonalityPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_PERSONALITY_TEXT );

	tabPersonality tabData;
	void* pTabData = &tabData;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pTabData))) )
	{
		strTitle = tabData.strName + _T(" ") + strTitle;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CPersonalityPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( theApp.m_hInstance );

	// Add Personality tab
	m_pTabPersonality = new CTabPersonality( this );
	if( m_pTabPersonality )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabPersonality->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}


	// Add ChordPalette tab
	m_pTabChordPalette = new CTabChordPalette (this, m_pIFramework);
	if( m_pTabChordPalette )
	{
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pTabChordPalette->m_psp, sizeof(m_pTabChordPalette->m_psp) );
		m_pTabChordPalette->PreProcessPSP(psp, FALSE );
		hPage = ::CreatePropertySheetPage( &psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Audition tab
	m_pTabAudition = new CTabAudition( this, m_pIFramework );
	if( m_pTabAudition )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabAudition->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add PersonalityInfo tab
	m_pTabPersonalityInfo = new CTabPersonalityInfo( this);
	if( m_pTabPersonalityInfo )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabPersonalityInfo->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}


	AfxSetResourceHandle( hInstance );

	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CPersonalityPageManager::OnRemoveFromPropertySheet( void )
{
	AFX_MANAGE_STATE( _afxModuleAddrThis );

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CPersonalityPageManager::sm_nActiveTab );

	CDllBasePropPageManager::OnRemoveFromPropertySheet();

	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CPersonalityPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIPropPageObject == NULL )
	{
		m_pTabPersonality->CopyDataToTab( NULL );
		m_pTabAudition->CopyDataToTab( NULL );
		m_pTabChordPalette->CopyDataToTab( NULL );
		return S_OK;
	}

	tabPersonality tabData;
	void* pTabData = &tabData;
	if( SUCCEEDED ( m_pIPropPageObject->GetData( (void **)&pTabData ) ) )
	{
		/*
		// *********** Update CTabPersonalityInfo
		// Make sure changes to current Personality are processed in OnKillFocus
		// messages before setting the new Personality
		CWnd* pWndHadFocus = CWnd::GetFocus();
		CWnd* pWnd = pWndHadFocus;
		CWnd* pWndParent = m_pTabPersonality->GetParent();

		while( pWnd )
		{
			if( pWnd == pWndParent )
			{
				::SetFocus( NULL );
				break;
			}
			pWnd = pWnd->GetParent();
		}
		*/
		m_pTabPersonalityInfo->SetPersonality( tabData.pPersonality);

		/*
		// Restore focus
		if( pWndHadFocus
		&&  pWndHadFocus != CWnd::GetFocus() )
		{
			pWndHadFocus->SetFocus();
		}
		*/

		
		// ***************** Update other tabs
		m_pTabPersonality->CopyDataToTab( &tabData );
		m_pTabAudition->CopyDataToTab( &tabData );
		m_pTabChordPalette->CopyDataToTab( &tabData );
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonalityPageManager IDMUSProdPropPageManager::UpdateObjectWithTabData

void CPersonalityPageManager::UpdateObjectWithTabData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropPageObject != NULL );

	tabPersonality tabData;
	tabPersonality* pTabData = &tabData;

	// since scale is shared by both tabs, need to arbitrate (active tab wins)
	// NB: assumes 0 -> personality tab active, 1-> ChordPalette tab active
//	long lScalePersonality, lScaleChordPalette;  // no longer have separate scales
	// make sure active tab variable is refreshed
	m_pIPropSheet->GetActivePage( &CPersonalityPageManager::sm_nActiveTab );

	// Populate the tabPersonality structure
	m_pTabPersonality->GetDataFromTab( &tabData );
//	lScalePersonality = tabData.lScalePattern;

	m_pTabAudition->GetDataFromTab( &tabData ); 

	m_pTabChordPalette->GetDataFromTab (&tabData);
//	lScaleChordPalette = tabData.lScalePattern;

	// Send the new data to the PropPageObject
	m_pIPropPageObject->SetData( (void *)&tabData );

	// get latest data including stuff that may have changed in previous instruction
	m_pIPropPageObject->GetData( (void **)&pTabData);

	if(sm_nActiveTab == 0)
	{
//		tabData.lScalePattern = lScalePersonality;
		// make sure that chord palette scale is updated
		m_pTabChordPalette->CopyDataToTab( pTabData );
	}
	else if(sm_nActiveTab == 1)
	{
//		tabData.lScalePattern = lScaleChordPalette;
		// make sure that personality scale is updated
		m_pTabPersonality->CopyDataToTab( pTabData );
	}
	
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPersonality constructor/destructor

CPersonality::CPersonality( CPersonalityComponent* pComponent )
{
	ASSERT( pComponent != NULL );

    m_dwRef = 0;
	AddRef();

	m_fModified = FALSE;
	m_fVariableNotFixed = FALSE;

	m_dblZoom = 0.03125;
	m_lFunctionbarWidth = 0;

	m_pComponent = pComponent;
	m_pComponent->AddRef();

	m_pIDirectMusicChordMap = NULL;
	m_pIDocRootNode = this;	// was NULL
	m_pIParentNode = NULL;
	m_hWndEditor = NULL;

	m_pUndoMgr = 0;

	CoCreateGuid( &m_guidPersonality ); 

	m_paletteChordType = ChordBuilder::AllMaj;
	m_selectiontype = CHORD_NO_SELECTION;
	m_nidToChord = -1;
	m_nidFromChord = -1;

	m_pPersonality = new Personality;
		ZeroMemory( &(m_pPersonality->m_username), MAX_LENGTH_PERSONALITYUSER_NAME );
		ZeroMemory( &(m_pPersonality->m_name), MAX_LENGTH_PERSONALITYUSER_NAME );
		m_pPersonality->m_dwflags = DMUS_CHORDMAPF_VERSION8;
		m_pPersonality->m_pfirstchord = 0;
		m_pPersonality->m_personref.m_stylename[0] = 0;
		m_pPersonality->m_personref.m_name[0] = 0;
		m_pPersonality->m_personref.m_filename[0] = 0;
		m_pPersonality->m_personref.m_isdefault = (char)0;
		m_pPersonality->m_cx = (short)0;
		m_pPersonality->m_cy = (short)0;

		m_wSplitterPos = 0;

		m_pIDMSegmentState = 0;
		
		
// We will persist to and from this IDirectMusicChordMap when we want to change the
// Engine's Personality data.
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIEngine != NULL );
// Its created when we either initialize a new personality or load one



////////////////////////////
// Initialize Strip Managers
	// Initialize the ChordMap Manager...
	m_punkChordMapMgr = NULL;
	if( SUCCEEDED( CoCreateInstance( CLSID_ChordMapMgr, NULL, CLSCTX_INPROC_SERVER,
		IID_IUnknown, (void**)&m_punkChordMapMgr )))
	{
		// Hand ChordMapMgr pointer to ChordMap's IDMUSProdNode interface
		IDMUSProdStripMgr* pIStripMgr;
		if( SUCCEEDED ( m_punkChordMapMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pIStripMgr ) ) )
		{
			if( pIStripMgr->IsParamSupported( GUID_DocRootNode ) == S_OK )
			{
				pIStripMgr->SetParam( GUID_DocRootNode, 0, (IDMUSProdNode *)this );
			}

			pIStripMgr->Release();
		}
	}
	else
	{
		MessageBox( 0, "ChordMapMgr initialization failed!", "ChordMapDesigner", MB_ICONEXCLAMATION );
		//ASSERT(FALSE); // couldn't create ChordMapMgr.
		m_punkChordMapMgr = NULL;
	}

	ASSERT( m_punkChordMapMgr != NULL );


// Personality data for Music Engine
	TCHAR achName[SMALL_BUFFER];

	::LoadString( theApp.m_hInstance, IDS_PERSONALITY_TEXT, achName, SMALL_BUFFER );
    wsprintf( m_pPersonality->m_username, "%s%d", achName, ++m_pComponent->m_nNextPersonality );
	strcpy( m_pPersonality->m_name, m_pPersonality->m_username );

	m_fDefault = FALSE;

	
// Additional fields added to store Personality properties
	m_fUseTimeSignature = TRUE;
	m_nBPM = 4;
	m_nBeat = 4;
	m_dwKey = 12;	// 2C
	m_pIStyleRefNode = NULL;
	m_dwGroove = 50;
	m_nChordMapLength = 20;

    m_paChordChanges = 0;		// List of Chord changes.
	m_lstCommandList = 0;		// Command list.

	// used by Advise/UnAdviseConnection
	m_dwCookie = 0;
	m_bAdvised = FALSE;

	
	
	m_pISection = NULL;
	m_paChordChanges = NULL;
	m_bSectionStopping = FALSE;


	m_dwVersionMS = ioPersonalityVersion::PersonalityVersionMS;
	m_dwVersionLS = ioPersonalityVersion::PersonalityVersionLS;

	CreateUndoMgr();

	m_pSignPostDlg = 0;
	m_pChordDlg = 0;

	m_fLockAllScales = true;
	m_fSyncLevelOneToAll = true;
}

CPersonality::~CPersonality()
{
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// Remove personality from Property Sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		pIPropSheet->Release();
	}

	RELEASE( m_pIDirectMusicChordMap );
	RELEASE( m_pComponent );
	RELEASE( m_punkChordMapMgr );

	// m_pPersonality will == NULL if an error occurs during loading and
	// this object is deleted.
	if( m_pPersonality )
	{
		ChordEntry *pchord = m_pPersonality->m_chordlist.GetHead();
		SignPost *psignpost = m_pPersonality->m_signpostlist.GetHead();
		for (;pchord;)
		{
			ChordEntry *next = pchord->GetNext();
			delete pchord;
			pchord = next;	
		}
		m_pPersonality->m_chordlist.RemoveAll();
		for (;psignpost;)
		{
			SignPost *next = psignpost->GetNext();
			delete psignpost;
			psignpost = next;	
		}
		m_pPersonality->m_signpostlist.RemoveAll();
		if( m_pPersonality )
		{
			delete m_pPersonality;
		}
	}

// Additional field added to store Personality properties
	if( m_pIStyleRefNode )
	{
		m_pIStyleRefNode->Release();
	}

	if( m_pISection )
	{
		m_pISection->Release();
		m_pISection = NULL;
	}

	if( m_pIDMSegmentState )
	{
		m_pIDMSegmentState->Release();
		m_pIDMSegmentState = NULL;
	}

	ClearChordList();
	List_Free( m_lstCommandList );

	// Clean up the undo mgr.
	if(m_pUndoMgr)
	{
		delete m_pUndoMgr;
		m_pUndoMgr = NULL;
	}

	m_pSignPostDlg = 0;
	m_pChordDlg = 0;
}


/////////////////////////////////////////////////////////////////////////////
//
HRESULT	CPersonality::Initialize()
{
		HRESULT hr = S_OK;

		hr = ::CoCreateInstance( CLSID_DirectMusicChordMap, NULL, CLSCTX_INPROC_SERVER,
										 IID_IDirectMusicChordMap, (void**)&m_pIDirectMusicChordMap );

		return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality::SyncPersonalityWithEngine
//


HRESULT CPersonality::SyncPersonalityWithEngine( int why )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(why & syncStopMusic)
		return S_FALSE;
// TODO: Fix for DirectMusic
//			if( why & syncStopMusic)
//			{
//				theApp.m_pStyleComponent->m_pIEngine->Stop( AAF_IMMEDIATE );
//			}


	HRESULT hr = E_FAIL;

	ASSERT( m_pIDirectMusicChordMap != NULL );
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	IDirectMusicSegment* pISection = NULL;
	IDMUSProdNode* pIStyle = NULL;
	IDMUSProdReferenceNode* pIDMUSProdRef = NULL;

	
	IStream* pIMemStream;
	IPersistStream* pIPersistStream;

	if(why & syncPersonality)
	{
		// make sure chord map is updated
//		SendChordMapAndPaletteToChordMapMgr();

		hr = m_pComponent->m_pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIMemStream );

		if( SUCCEEDED ( hr ) )
		{
			hr = Save( pIMemStream, FALSE );
			if( SUCCEEDED ( hr ) )
			{
				m_pIDirectMusicChordMap->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
				if( pIPersistStream )
				{
					StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
					hr = pIPersistStream->Load( pIMemStream );
					ASSERT( SUCCEEDED(hr) );

					pIPersistStream->Release();
				}
			}
		}

		pIMemStream->Release();
	}
	if(why & syncAudition)
	{
		if(m_pIStyleRefNode == NULL)
		{
			// too early
			return S_OK;
		}
		hr = m_pIStyleRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void**)&pIDMUSProdRef);
		if(FAILED(hr))
		{
			goto Leave;
		}
		hr = pIDMUSProdRef->GetReferencedFile(&pIStyle);
		pIDMUSProdRef->Release();
		if(pIStyle)
			pIStyle->Release();
		if(!m_paChordChanges  || FAILED(hr))
		{
			// no chord selected, nothing to do
			return S_OK;
		}
	
		if(FAILED(hr = ComposeSection(&pISection)))
		{
			goto Leave;
		}

		if(m_pISection)		// already have a composed section
		{
		// find out whether its playing
			m_pISection->Release();
			m_pISection = pISection;
			pISection = NULL;	
			if(S_OK == m_pComponent->m_pIConductor->IsTransportPlaying((IDMUSProdTransport*)this))
			{
				IDirectMusicPerformance* pIDMPerformance = 0;
				m_pComponent->m_pIConductor->GetPerformanceEngine( (IUnknown**)&pIDMPerformance);
				if(!pIDMPerformance)
				{
					hr = E_FAIL;
				}
				else
				{
					hr = pIDMPerformance->PlaySegment( m_pISection, DMUS_SEGF_BEAT, 0, &m_pIDMSegmentState );
					pIDMPerformance->Release();
				}
			}
		} // if(m_pISection)
		else
		{
			// section not defined
			m_pISection = pISection;
			pISection = NULL;
		}

	}
Leave:
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IUnknown implementation

HRESULT CPersonality::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageObject *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IPersist) )
    {
        AddRef();
        *ppvObj = (IPersist *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IPersistStream) )
    {
        AddRef();
        *ppvObj = (IPersistStream *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdTimelineCallback) )
    {
        AddRef();
        *ppvObj = (IDMUSProdTimelineCallback *)this;
        return S_OK;
    }

	if( ::IsEqualIID(riid, IID_IDMUSProdTransport) )
	{
		AddRef();
		*ppvObj = (IDMUSProdTransport*)this;
		return S_OK;
	}
    if( ::IsEqualIID(riid, IID_IDMUSProdNotifyCPt) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNotifyCPt *)this;
        return S_OK;
    }

	if( ::IsEqualIID(riid, IID_IDMUSProdNotifySink))
	{
		AddRef();
		*ppvObj = (IDMUSProdNotifySink*)this;
		return S_OK;
	}

	if( ::IsEqualIID(riid, IID_IDMUSProdChordMapInfo))
	{
		AddRef();
		*ppvObj = (IDMUSProdChordMapInfo*)this;
		return S_OK;
	}

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CPersonality::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    ++m_dwRef;
//	TRACE( "CPersonality::AddRef: %ld\n", m_dwRef );
	return m_dwRef;
}

ULONG CPersonality::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

//	TRACE( "CPersonality::Release: %ld\n", m_dwRef );

    if( m_dwRef == 0 )
    {
//		TRACE( "PERSONALITY::Release: CPersonality destroyed!\n" );
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetNodeImageIndex

HRESULT CPersonality::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );

	return (m_pComponent->GetPersonalityImageIndex(pnFirstImage));
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetFirstChild

HRESULT CPersonality::GetFirstChild( IDMUSProdNode** /*ppIFirstChildNode*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetNextChild

HRESULT CPersonality::GetNextChild( IDMUSProdNode* /*pIChildNode*/, IDMUSProdNode** /*ppINextChildNode*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;		// Personality nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetComponent

HRESULT CPersonality::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetDocRootNode

HRESULT CPersonality::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CPersonality IDMUSProdNode::SetDocRootNode

HRESULT CPersonality::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetParentNode

HRESULT CPersonality::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::SetParentNode

HRESULT CPersonality::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetNodeId

HRESULT CPersonality::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_PersonalityNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetNodeName

HRESULT CPersonality::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	CString csName;
	csName = m_pPersonality->m_username;
	*pbstrName = csName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetNodeNameMaxLength

HRESULT CPersonality::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pnMaxLength = MAX_LENGTH_PERSONALITYUSER_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::ValidateNodeName

HRESULT CPersonality::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString strName;

	strName = bstrName;
	::SysFreeString( bstrName );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::SetNodeName

HRESULT CPersonality::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	CString strName = bstrName;
	::SysFreeString( bstrName );
	
	if(!_tcscmp(m_pPersonality->m_name, m_pPersonality->m_username))
	{
		// if names were the same, keep in sync
		_tcscpy( m_pPersonality->m_name, (LPCTSTR)strName );
	}
	_tcscpy( m_pPersonality->m_username, (LPCTSTR)strName );
	m_fModified = TRUE;

	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		pIPropSheet->Release();
	}

	// sync personality (with new name) to engine
	SyncPersonalityWithEngine(syncPersonality);

	// Notify connected nodes that Style name has changed
	m_pComponent->m_pIFramework->NotifyNodes( this, PERSONALITY_NameChange, NULL );

	OnNameChange();	// notify internal components

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetNodeListInfo

HRESULT CPersonality::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pListInfo != NULL );

	CString strDescriptor;

	if( m_pPersonality )
	{
		ASSERT( m_pComponent != NULL );
		m_pComponent->ScalePatternToScaleName( m_pPersonality->m_scalepattern, strDescriptor );
	}

	// Put in place to provide Template 'username' requirement.
    CString csUserName = m_pPersonality->m_username;

	pListInfo->bstrName = csUserName.AllocSysString();
    pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
	memcpy( &pListInfo->guidObject, &m_guidPersonality, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetEditorClsId

HRESULT CPersonality::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    *pClsId = CLSID_PersonalityEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetEditorTitle

HRESULT CPersonality::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	::LoadString( theApp.m_hInstance, IDS_PERSONALITY_TEXT, achTitle, MID_BUFFER );
	strTitle  = achTitle;
	strTitle += _T(": " );
	strTitle += m_pPersonality->m_username;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetEditorWindow

HRESULT CPersonality::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::SetEditorWindow

HRESULT CPersonality::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::UseOpenCloseImages

HRESULT CPersonality::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetRightClickMenuId

HRESULT CPersonality::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_PERSONALITY_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::OnRightClickMenuInit

HRESULT CPersonality::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::OnRightClickMenuSelect

HRESULT CPersonality::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

//	IDMUSProdNode* pIDocRootNode;


	switch( lCommandId )
	{
		case IDM_RENAME:
			if( SUCCEEDED ( m_pComponent->m_pIFramework->EditNodeLabel((IDMUSProdNode *)this) ) )
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
// CPersonality IDMUSProdNode::DeleteChildNode

HRESULT CPersonality::DeleteChildNode( IDMUSProdNode* /*pIChildNode*/, BOOL /*fPromptUser*/ )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;	// Personality nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::InsertChildNode

HRESULT CPersonality::InsertChildNode( IDMUSProdNode* /*pIChildNode*/ )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;	// Personality nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::DeleteNode

HRESULT CPersonality::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// If this pIChildNode is currently playing, then Stop it.
	StopIfPlaying( 0 );

	// Remove from Project Tree
	if( m_pComponent->m_pIFramework->RemoveNode( (IDMUSProdNode*)this, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Unadvise ConnectionPoint and unregister the CSection (JAZZNode) from the Transport.
	UnAdviseConnectionPoint();

	// Remove from Component Personality list
	m_pComponent->RemoveFromPersonalityFileList( this );

	m_fModified = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::OnNodeSelChanged

HRESULT CPersonality::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if ( fSelected )
	{
		ASSERT( m_pComponent );
		ASSERT( m_pComponent->m_pIConductor );
		if( !m_pComponent || !m_pComponent->m_pIConductor )
		{
			return E_UNEXPECTED;
		}

		m_pComponent->m_pIConductor->SetActiveTransport((IDMUSProdTransport *) this, NULL);
	}
	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CPersonality Drag/Drop
/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::CreateDataObject

HRESULT CPersonality::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

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

	// Save Personality into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( m_pComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_PERSONALITY into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfPersonality, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		pIStream->Release();
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if( SUCCEEDED ( m_pComponent->m_pIFramework->SaveClipFormat( m_pComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// Style nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfProducerFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			pIStream->Release();
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		pDataObject->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::CanCut

HRESULT CPersonality::CanCut( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return CanDelete();
 
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::CanCopy

HRESULT CPersonality::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::CanDelete

HRESULT CPersonality::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::CanDeleteChildNode

HRESULT CPersonality::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UNREFERENCED_PARAMETER(pIChildNode);
	return S_FALSE;	
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality::PasteCF_STYLE

HRESULT CPersonality::PasteCF_STYLE( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject,
							  IDMUSProdNode* pIPositionNode )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	UNREFERENCED_PARAMETER(pIPositionNode);

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, m_pComponent->m_cfStyle, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// replace doc root node of section
		IDMUSProdNode* pIDocRootNode;
		IDMUSProdReferenceNode* pIDMUSProdReferenceNode;
		hr = m_pComponent->m_pIFramework->GetDocRootNodeFromData(pIDataObject, &pIDocRootNode);
		if(SUCCEEDED(hr))
		{
			if(m_pIStyleRefNode == NULL)
			{
				// Allocate a new reference node
				IDMUSProdComponent* pIStyleComponent;
				hr = m_pComponent->m_pIFramework->FindComponent( CLSID_StyleComponent,  &pIStyleComponent );
				if(FAILED(hr))
				{
					return E_FAIL;
				}
				hr = pIStyleComponent->AllocReferenceNode(GUID_StyleRefNode, &m_pIStyleRefNode);
				RELEASE(pIStyleComponent);
				if(FAILED(hr))
				{
					return  E_OUTOFMEMORY;
				}
			}

			hr = m_pIStyleRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void **)&pIDMUSProdReferenceNode);
			if(SUCCEEDED(hr))
			{
				hr = SetStyleReference(pIDocRootNode);
//				hr = pIDMUSProdReferenceNode->SetReferencedFile(pIDocRootNode);
				m_fModified = TRUE;
				pIDMUSProdReferenceNode->Release();
			}
			pIDocRootNode->Release();
		}
	}

	if(m_fModified)
	{
//		CPersonalityPageManager* pPageManager;
		if( theApp.m_pIPageManager
		&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_PersonalityPageManager ) == S_OK )
		{
//			pPageManager = (CPersonalityPageManager *)theApp.m_pIPageManager;
//			pPageManager->Refresh();
			theApp.m_pIPageManager->RefreshData();
		}
		SyncPersonalityWithEngine(syncAudition);
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::CanPasteFromData

HRESULT CPersonality::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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
	
	if((pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfStyle ) == S_OK) )
	{
		*pfWillSetReference = TRUE;
		hr = S_OK;
	}

	pDataObject->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::PasteFromData

HRESULT CPersonality::PasteFromData( IDataObject* pIDataObject )
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


	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfStyle ) ) )
	{
		// Handle CF_Style format
		hr = PasteCF_STYLE( pDataObject, pIDataObject, NULL );
	}


	pDataObject->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::CanChildPasteFromData

HRESULT CPersonality::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
											 BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if(pIDataObject == NULL || pIChildNode == NULL)
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
	
	IDMUSProdNode* pINode;
	if(SUCCEEDED(m_pIStyleRefNode->QueryInterface(IID_IDMUSProdNode, (void**)&pINode)))
	{
		BOOL b2 = (pINode == m_pIStyleRefNode) 
						&&( S_OK ==  pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfStyle));
		pINode->Release();
		if(b2)
		{
			*pfWillSetReference = TRUE;
			hr = S_OK;
		}
		else
		{
			hr = S_FALSE;
		}
	}

	pDataObject->Release();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::ChildPasteFromData

HRESULT CPersonality::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(pIDataObject == NULL || pIChildNode == NULL)
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
	
	IDMUSProdNode* pINode;
	if(SUCCEEDED(m_pIStyleRefNode->QueryInterface(IID_IDMUSProdNode, (void**)&pINode)))
	{
		BOOL b2 = (pINode == m_pIStyleRefNode) 
						&&( S_OK ==  pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfStyle));
		pINode->Release();
		if(b2)
		{
			// Handle CF_STYLE format
			hr = PasteCF_STYLE( pDataObject, pIDataObject, NULL );
		}
		else
		{
			hr = S_FALSE;
		}
	}
	return hr;
}




/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdPropPageObject::GetData

HRESULT CPersonality::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );
	ASSERT( m_pPersonality != NULL );

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	tabPersonality* pData = (tabPersonality *)*ppData;

	// put personality in
	pData->pPersonality = this;

	// Get Project GUID for this personality 
	IDMUSProdProject* pIProject;

	memset( &pData->guidProject, 0, sizeof( pData->guidProject ) );
	if( SUCCEEDED ( m_pComponent->m_pIFramework->FindProject( this, &pIProject ) ) )
	{
		pIProject->GetGUID( &pData->guidProject );
		pIProject->Release();
	}


	// Get Style DocRoot and GUID
	IDMUSProdNode* pDocRoot = NULL;
	IDMUSProdReferenceNode* pIDMUSProdReferenceNode;
	HRESULT hr;
	if(m_pIStyleRefNode)
	{
		hr = m_pIStyleRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void**)&pIDMUSProdReferenceNode);
		ASSERT(SUCCEEDED(hr));
		if(SUCCEEDED(hr))
		{
			hr = pIDMUSProdReferenceNode->GetReferencedFile(&pDocRoot);
		}
		pIDMUSProdReferenceNode->Release();
		pIDMUSProdReferenceNode = 0;
	}
	pData->pIStyleDocRootNode = pDocRoot;	// make assignment if succeed or not (pDocRoot == NULL)
/*
	memset(&pData->guidStyleProject, 0, sizeof(GUID));
	if( SUCCEEDED (hr))
	{
		hr = m_pComponent->m_pIFramework->FindProject( pData->pIStyleDocRootNode, &pIProject );
		if(SUCCEEDED(hr))
		{
			pIProject->GetGUID( &pData->guidStyleProject );
			pIProject->Release();
		}
	}
*/
	RELEASE(pDocRoot);	// this gets ref'ed by caller when data is copied




	// Fill remaining fields in tabPersonality structure
	pData->strName = m_pPersonality->m_username;
//	pData->strRefName = m_pPersonality->m_name;
	pData->strDescription = m_pPersonality->m_description;
	pData->fUseTimeSignature = m_fUseTimeSignature;
	pData->nBPM = m_nBPM;
	pData->nBeat = m_nBeat;
	pData->dwGroove = m_dwGroove;
	pData->nChordMapLength = m_nChordMapLength;
	pData->lScalePattern = Rotate24( m_pPersonality->m_scalepattern, GetAbsKey() );
	pData->dwKey = m_dwKey;
	pData->fVariableNotFixed = m_fVariableNotFixed;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdPropPageObject::SetData

HRESULT CPersonality::SetData( void* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HINSTANCE hInst = AfxGetInstanceHandle( );

	tabPersonality* pTabData = (tabPersonality *)pData;

	CString csName = m_pPersonality->m_username;

	// Name has changed
	if( pTabData->strName != csName )
	{
		if(m_pUndoMgr)
			m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_NAMECHANGE);
		BSTR bstrNodeName = pTabData->strName.AllocSysString();
		SetNodeName( bstrNodeName );
	
		// change guid if name has changed
		//CoCreateGuid(&m_guidPersonality);

		// Update the Editor's Title.
		if( m_hWndEditor != NULL )
		{
			CString csName;
			csName.Format( "ChordMap: %s", m_pPersonality->m_username );
			SetWindowText( m_hWndEditor, csName );
		}

		// Update the Project Node.
		m_pComponent->m_pIFramework->RefreshNode( (IDMUSProdNode*)this );

		m_fModified = TRUE;
	}
/* refname is no longer editable by the user
	// Ref Name has changed
	if( _tcscmp( pTabData->strRefName, m_pPersonality->m_name ) != 0 )
	{
		if(m_pUndoMgr)
			m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_REFNAME);
		// LimitText() in OnInitDialog() limits text to 20 bytes
		_tcscpy( m_pPersonality->m_name, pTabData->strRefName );
		m_fModified = TRUE;
	}
*/
	// Description has changed
	if( _tcscmp( pTabData->strDescription, m_pPersonality->m_description ) != 0 )
	{
		if(m_pUndoMgr)
			m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_DESCR);
		// LimitText() in OnInitDialog() limits text to 20 bytes
		_tcscpy( m_pPersonality->m_description, pTabData->strDescription );
		m_fModified = TRUE;
	}

	// Use TimeSig has changed
	if( pTabData->fUseTimeSignature != m_fUseTimeSignature )
	{
		if(m_pUndoMgr)
			m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_EDITMODE);
		m_fUseTimeSignature = pTabData->fUseTimeSignature;
		m_fModified = TRUE;
	}
	else if( m_fUseTimeSignature == TRUE )
	{
		// Time Signature has changed
		if( (pTabData->nBPM != m_nBPM)
		||  (pTabData->nBeat != m_nBeat) )
		{
			if(m_pUndoMgr)
				m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_TIMESIG);
			m_nBPM = pTabData->nBPM;
			m_nBeat = pTabData->nBeat;
			
			ChangeChordSignPostTimeSig();
			ComputeChordMap();
			ChangeChordMapLength();	// need to make timeline know that there's a different number of
			// clocks (measures stay the same).
			SyncPersonalityWithEngine(syncAudition | syncPersonality);
			m_fModified = TRUE;
		}
	}

	// VariableNotFixed has changed
	if( pTabData->fVariableNotFixed != m_fVariableNotFixed)
	{
		if(m_pUndoMgr)
		{
			if(m_fVariableNotFixed == TRUE)
			{
				m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_SWITCHTOFIXED);
			}
			else
			{
				m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_SWITCHTOVAR);
			}
		}

		m_fVariableNotFixed = pTabData->fVariableNotFixed;
		ChangeChordMapEditMode();
		m_fModified = TRUE;
	}

	// Chordmap length has changed
	if( pTabData->nChordMapLength != m_nChordMapLength)
	{
		if(m_pUndoMgr)
			m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_CHORDMAPLENGTH);
		m_nChordMapLength = pTabData->nChordMapLength;
		ChangeChordMapLength();
		m_fModified = TRUE;
	}

	// scalepattern has changed
	long lScalePattern = Rotate24( pTabData->lScalePattern, -(long)GetAbsKey() );
	if( lScalePattern != m_pPersonality->m_scalepattern)
	{
		if(m_pUndoMgr)
			m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_SCALEPAT);
		m_pPersonality->m_scalepattern = lScalePattern;
		TransposeAllChords(pTabData->dwKey);
		SendChordMapAndPaletteToChordMapMgr();
		UpdateChordPalette( false );
//		RefreshSignPosts(false);		// no need to call this and it does bad things in some situations
		RefreshChordMap();
		m_fModified = TRUE;
	}

	// Audition Style has changed
	// no style?
	IDMUSProdNode* pDocRoot = NULL;
//	IDMUSProdReferenceNode* pIDMUSProdReferenceNode;
	HRESULT hr;
	if(m_pIStyleRefNode == NULL)
	{
		// Allocate a new reference node
		IDMUSProdComponent* pIStyleComponent;
		hr = m_pComponent->m_pIFramework->FindComponent( CLSID_StyleComponent,  &pIStyleComponent );
		if(FAILED(hr))
		{
			return E_FAIL;
		}
		hr = pIStyleComponent->AllocReferenceNode(GUID_StyleRefNode, &m_pIStyleRefNode);
		RELEASE(pIStyleComponent);
		if(FAILED(hr))
		{
			return  E_OUTOFMEMORY;
		}
	}

	// Get DocRoot of Style
//	m_pIStyleRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void**)&pIDMUSProdReferenceNode);
//	hr = pIDMUSProdReferenceNode->GetReferencedFile(&pDocRoot);
//	if(FAILED(hr))
//	{
//		// no doc root, that's ok
//		pDocRoot = NULL;
//	}
	pDocRoot = GetStyleDocRoot();

	// style has changed, NB, this catches newly created ref node as doc root will be null
	if( pDocRoot != pTabData->pIStyleDocRootNode )
	{
//		pIDMUSProdReferenceNode->SetReferencedFile(pTabData->pIStyleDocRootNode);
		if(m_pUndoMgr)
			m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_AUDSTYLE);
		hr = SetStyleReference(pTabData->pIStyleDocRootNode);
		m_fModified = TRUE;
	}


	RELEASE(pDocRoot);
//	RELEASE(pIDMUSProdReferenceNode);


	// Audition Groove has changed
	if( pTabData->dwGroove != m_dwGroove  )
	{
		if(m_pUndoMgr)
			m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_GROOVE);
		m_dwGroove = pTabData->dwGroove;						// xxxx
		m_fModified = TRUE;
	}

	if(pTabData->dwKey != m_dwKey)
	{
		if(m_pUndoMgr && ((pTabData->dwKey & 0x0fffffff) == (m_dwKey & 0x0fffffff)))
		{
			m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_UseFlat);
		}
		else if(m_pUndoMgr)
			m_pUndoMgr->SaveState(this, hInst, IDS_UNDO_KEY);
		TransposeAllChords(pTabData->dwKey);
		m_dwKey = pTabData->dwKey;
		SendChordMapAndPaletteToChordMapMgr();
		UpdateChordPalette( true );
		RefreshSignPosts(false);
		RefreshChordMap();
		m_fModified = TRUE;
	}

	if(m_fModified)
	{
		SyncPersonalityWithEngine(syncAudition | syncPersonality);
	}

	return S_OK;
}

void CPersonality::ChangeChordSignPostTimeSig()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Change the Time Signature of the Timeline
	if( m_punkChordMapMgr == NULL ) return;

	IDMUSProdStripMgr *pStripMgrChordMap = NULL;
	if( FAILED(m_punkChordMapMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgrChordMap )) )
	{
		return;
	}

	IDMUSProdTimeline *pTimeline = NULL;
	VARIANT varTimeline;
	if( FAILED(pStripMgrChordMap->GetStripMgrProperty( SMP_ITIMELINECTL, &varTimeline )) || (V_UNKNOWN( &varTimeline ) == NULL) )
	{
		pStripMgrChordMap->Release();
		return;
	}
	if( FAILED( V_UNKNOWN( &varTimeline )->QueryInterface( IID_IDMUSProdTimeline, (void**)&pTimeline ) ) || (pTimeline == NULL) )
	{
		V_UNKNOWN( &varTimeline )->Release();
		pStripMgrChordMap->Release();
		return;
	}
	V_UNKNOWN( &varTimeline )->Release();


	DMUS_TIMESIGNATURE timesig;
	timesig.mtTime = 0;
	timesig.bBeatsPerMeasure = static_cast<BYTE>(m_nBPM);
	timesig.bBeat = static_cast<BYTE>(m_nBeat);
	if(timesig.bBeat > 4)
	{
		// compound meter
		timesig.wGridsPerBeat = 3;
	}
	else
	{
		// simple meter
		timesig.wGridsPerBeat = 4;
	}

	pTimeline->SetParam(GUID_TimeSignature, 0xFFFFFFFF, 0, 0, static_cast<void*>(&timesig));

	pTimeline->Refresh();
	pTimeline->Release();
	pStripMgrChordMap->Release();
}

void CPersonality::ChangeChordMapEditMode()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if(m_punkChordMapMgr == NULL) return;

	IDMUSProdStripMgr *pStripMgrChordMap = NULL;
	if( FAILED(m_punkChordMapMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgrChordMap )) )
	{
		return;
	}

	IDMUSProdTimeline *pTimeline = NULL;
	VARIANT varTimeline;
	if( FAILED(pStripMgrChordMap->GetStripMgrProperty( SMP_ITIMELINECTL, &varTimeline )) || (V_UNKNOWN( &varTimeline ) == NULL) )
	{
		pStripMgrChordMap->Release();
		return;
	}
	if( FAILED( V_UNKNOWN( &varTimeline )->QueryInterface( IID_IDMUSProdTimeline, (void**)&pTimeline ) ) || (pTimeline == NULL) )
	{
		V_UNKNOWN( &varTimeline )->Release();
		pStripMgrChordMap->Release();
		return;
	}
	V_UNKNOWN( &varTimeline )->Release();

	pStripMgrChordMap->SetParam(GUID_VariableNotFixed, 0, &m_fVariableNotFixed);


	pTimeline->Release();
	pStripMgrChordMap->Release();
}

void CPersonality::ChangeChordMapLength()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Update chord map
	TrimChordMap();


	// Change the Length of the Timeline
	if( m_punkChordMapMgr == NULL ) return;

	IDMUSProdStripMgr *pStripMgrChordMap = NULL;
	if( FAILED(m_punkChordMapMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgrChordMap )) )
	{
		return;
	}

	IDMUSProdTimeline *pTimeline = NULL;
	VARIANT varTimeline;
	if( FAILED(pStripMgrChordMap->GetStripMgrProperty( SMP_ITIMELINECTL, &varTimeline )) || (V_UNKNOWN( &varTimeline ) == NULL) )
	{
		pStripMgrChordMap->Release();
		return;
	}
	if( FAILED( V_UNKNOWN( &varTimeline )->QueryInterface( IID_IDMUSProdTimeline, (void**)&pTimeline ) ) || (pTimeline == NULL) )
	{
		V_UNKNOWN( &varTimeline )->Release();
		pStripMgrChordMap->Release();
		return;
	}
	V_UNKNOWN( &varTimeline )->Release();

	// compute how many clocks in desired measure length
	long clocks;
	// BUGBUG: Assuming our ChordMapStrip is in group 1 and that we're using the default
	// TimeSig strip (the one displayed at the top of the timeline)
	if( FAILED(pTimeline->MeasureBeatToClocks( 1, 0, m_nChordMapLength, 0, &clocks)))
	{
		ASSERT(0);
	}
	
	// now set it.
	VARIANT vtInit;
	vtInit.vt = VT_I4;

	V_I4(&vtInit) = clocks;
	pTimeline->SetTimelineProperty( TP_CLOCKLENGTH, vtInit );


	pTimeline->Refresh();
	pTimeline->Release();
	pStripMgrChordMap->Release();
}


void CPersonality::DeleteAllConnectionsToChord( int nID )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	// Delete Connection
	for(ChordEntry *pchord = m_pPersonality->m_chordlist.GetHead();
		pchord != NULL;
		pchord = pchord->GetNext())
	{
		for(NextChord *pnext = pchord->m_nextchordlist.GetHead();
			pnext != NULL;
			)
		{
			// Delete all connections to the newly removed Chord.
			if( pnext->m_nid == nID )
			{
				NextChord* ptemp = pnext->GetNext();
				pchord->m_nextchordlist.Remove(pnext);
				delete pnext;
				pnext = ptemp;
			}
			else
			{
				pnext->m_nextchord = NULL;
				pnext = pnext->GetNext();
			}
		} 
	}
}

//////////////////////////////////////////////////////////////////////////////
//
void CPersonality::TrimChordMap()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT(m_pPersonality);
	ChordEntry* pChord = m_pPersonality->m_chordlist.GetHead();
	for(pChord; pChord; )
	{
		if(pChord->m_chordsel.Measure() < m_nChordMapLength)
		{
			// leave this chord
			pChord = pChord->GetNext();
			continue;
		}
		else
		{
			// delete this sucker and all his connections
			int id = pChord->m_nid;
			m_pPersonality->m_chordlist.Remove(pChord);
			delete pChord;
			pChord = m_pPersonality->m_chordlist.GetHead();
			DeleteAllConnectionsToChord(id);
			m_pPersonality->ResolveConnections();			
		}
	}
}



/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdPropPageObject::OnShowProperties

HRESULT CPersonality::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// Get the Personality page manager
	CPersonalityPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_PersonalityPageManager ) == S_OK )
	{
		pPageManager = (CPersonalityPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CPersonalityPageManager( m_pComponent->m_pIFramework, this );
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Personality properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CPersonalityPageManager::sm_nActiveTab;

		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CPersonality::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonality IPersist::GetClassID

HRESULT CPersonality::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonality IPersistStream::IsDirty

HRESULT CPersonality::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );

	// check whether zoom has changed
	// need to do this here because chord dialog can't notify us if it has changed.
	if(m_pChordDlg)
	{
		double dblZoom = m_pChordDlg->GetZoom();
		if(dblZoom != m_dblZoom)
		{
			m_dblZoom = dblZoom;
			m_fModified = TRUE;
		}

		const long lFunctionbarWidth = m_pChordDlg->GetFunctionbarWidth();
		if( lFunctionbarWidth != m_lFunctionbarWidth )
		{
			m_lFunctionbarWidth = lFunctionbarWidth;
			m_fModified = TRUE;
		}
	}

	return (m_fModified == TRUE)?S_OK:S_FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// CPersonality IPersistStream::Load

HRESULT CPersonality::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ULONG	ulSize,
			ulID;
	Personality *pcmap = NULL;
	HRESULT	hr = E_FAIL;

	ASSERT(pIStream != NULL);
	if(pIStream == NULL)
	{
		return E_INVALIDARG;
	}


	CleanupObjects();
	
	DWORD dwPos = StreamTell(pIStream);	// save stream position
	IDMUSProdRIFFStream* pIRiffStream;
	MMCKINFO ckMain;
	bool bFoundFormat = false;
	// check for Direct Music format
	if(SUCCEEDED(AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		ckMain.fccType = FOURCC_PERSONALITY;
		if(pIRiffStream->Descend(&ckMain, NULL, MMIO_FINDRIFF)==0)
		{
			hr = LoadPersonality(pIRiffStream, &ckMain);
			bFoundFormat = true;
		}
		RELEASE(pIRiffStream);
	}

	if(!bFoundFormat)
	{
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
		// Check for Old Format Personality .PEP file...
		ulID = ReadID( pIStream );
		ulSize = ReadSize( pIStream );
		if (ulID == ID_EPERSONALITY || ulID == ID_JPERSONALITY || ulID == ID_J4PERSONALITY || ulID == ID_J5PERSONALITY)
		{
			CPersonality::LoadPersonality( pIStream, ulSize, ulID );
			if (m_pPersonality)
			{
				m_pPersonality->AssignParents();
				hr = S_OK;
			}
			hr = LoadJazzStyleReference(pIStream);
			if(FAILED(hr))
			{
				TRACE("LoadJazzStyleReference failed, ignoring for now");
				hr = S_OK;
			}
		}
		else
		{
			AfxMessageBox(IDS_ERR_IMARUNTIME);
			hr = E_FAIL;	// .PER and non-personality RIFF files not supported.
		}
	}
	//
	if(SUCCEEDED(hr))
	{
		m_pPersonality->RenumberIDs();
		
		if(m_pIDirectMusicChordMap == NULL)
		{
			hr = ::CoCreateInstance( CLSID_DirectMusicChordMap, NULL, CLSCTX_INPROC_SERVER,
												 IID_IDirectMusicChordMap, (void**)&m_pIDirectMusicChordMap );
		}
		//
		//	Allocate a new Music Engine Personality and Persist it to the engine.
		//
		if( SUCCEEDED( hr ) )
		{
			// check for selected chords
			UpdateSelectedChordList();
			if( SUCCEEDED(hr = SyncPersonalityWithEngine(syncPersonality+syncAudition)) )
			{
//				/* this call made in SyncPersonalityWithEngine(syncPersonality)
				if( FAILED(hr = SendChordMapAndPaletteToChordMapMgr()) )
				{
					TRACE( "SendChordMapAndPaletteToChordMapMgr() Failed!\n" );
				}
				else
				{
//					LinkAll(true);
				}
//				*/
			}
		}
	}
	if(!SUCCEEDED(hr))
	{
		SyncPersonalityWithEngine(syncStopMusic);
	}

	// many functions called during load set this to true, we want to make sure that
	// a newly loaded Personality is "clean"
	m_fModified = FALSE;
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality IPersistStream::Save

HRESULT CPersonality::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	HRESULT hr = S_OK;
	DMUSProdStreamInfo	StreamInfo;//Added ECW 4/24/98
	FileType	ftFileType;
	GUID		guidDataFormat;

	// We want to know what type of save we are doing because we
	// don't want to save UI information in Personality 'Release' files...	
	IDMUSProdPersistInfo *pIDMUSProdPersist = NULL;
	if( FAILED( hr = pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void**)&pIDMUSProdPersist ) ) )
	{
		ftFileType = FT_RUNTIME;
		guidDataFormat = GUID_CurrentVersion;
		hr = S_OK;
	}
	else
	{
		hr = pIDMUSProdPersist->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pIDMUSProdPersist->Release();
		if (FAILED( hr ))
		{
			return hr;
		}
	}		
	

	if( ::IsEqualGUID(guidDataFormat, GUID_CurrentVersion) )
	{
		return DMSave(pIStream, fClearDirty);
	}
	else if( ::IsEqualGUID(guidDataFormat, GUID_DirectMusicObject) )
	{
		return DMSave(pIStream, fClearDirty);
	}

	ASSERT( 0 );
	return E_UNEXPECTED;
}

HRESULT CPersonality::SaveChordList( IStream* pIStream, BOOL bReleaseSave )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ChordEntry *pchord = m_pPersonality->m_chordlist.GetHead();
	for(;pchord;pchord = pchord->GetNext())
	{       
		pchord->Save( pIStream, bReleaseSave );
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSection IPersistStream::GetSizeMax

HRESULT CPersonality::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality additional functions


/////////////////////////////////////////////////////////////////////////////
// CPersonality::LoadPersonality

HRESULT CPersonality::LoadPersonality( IStream* pIStream, DWORD dwSize, DWORD id)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pPersonality != NULL );

//	if( m_pPersonality ) delete m_pPersonality;
//	m_pPersonality = new Personality;
	long lrecordsize = ReadSize( pIStream );
	long lfilesize = (long)dwSize;

	lfilesize -= 4;   
	if (m_pPersonality)
	{
		m_fVariableNotFixed = TRUE;
		lfilesize -= lrecordsize; 
		m_pPersonality->Load(pIStream, lrecordsize);
//		pIStream->Read(m_pPersonality,lrecordsize,NULL);
		m_pPersonality->m_pfirstchord = NULL;
		m_pPersonality->m_playlist.RemoveAll();		
		m_pPersonality->m_chordlist.RemoveAll();
		m_pPersonality->m_signpostlist.RemoveAll();

		// load meter
		if( id == ID_JPERSONALITY)
		{
			pIStream->Read(&m_nBeat, sizeof(short), NULL);
			pIStream->Read(&m_nBPM, sizeof(short), NULL);
			lfilesize -= sizeof(short) * 2;
		}
		// load length and meter
		if( id == ID_J4PERSONALITY)
		{
			m_fVariableNotFixed = FALSE;
			long size;
			pIStream->Read(&size, sizeof(long), NULL);
			m_nChordMapLength = LOWORD(size);
			pIStream->Read(&m_nBeat, sizeof(short), NULL);
			pIStream->Read(&m_nBPM, sizeof(short), NULL);
			lfilesize -= (sizeof(short) * 2 + sizeof(long));
		}

		// load length and meter
		if( id == ID_J5PERSONALITY)
		{
			m_fVariableNotFixed = FALSE;
			long size;
			pIStream->Read(&size, sizeof(long), NULL);
			m_nChordMapLength = LOWORD(size);
			pIStream->Read(&m_nBeat, sizeof(short), NULL);
			pIStream->Read(&m_nBPM, sizeof(short), NULL);
			pIStream->Read(&m_fVariableNotFixed, sizeof(BOOL), NULL);
			lfilesize -= (sizeof(short) * 2 + sizeof(long) + sizeof(BOOL));
		}

		for (;lfilesize > 0;)
		{
			DWORD dwid = ReadID( pIStream );
			lrecordsize = ReadSize( pIStream );
			lfilesize -= 8;
			if (dwid == ID_ECHORDENTRY)
			{
				ChordEntry *pchord = ChordEntry::Load(pIStream,lrecordsize, dwid);
				if (pchord)
				{
					pchord->SetNext(NULL);
				    m_pPersonality->m_chordlist.AddTail(pchord);
				}
			}
			else if (dwid == ID_CHORDENTRY)
			{
				ChordEntry *pchord = ChordEntry::Load(pIStream,lrecordsize, dwid);
				if (pchord)
				{
					pchord->SetNext(NULL);
				    m_pPersonality->m_chordlist.AddTail(pchord);
					pchord->m_rect.left = 100 - CENTER_X;
					pchord->m_rect.right = 100 + CENTER_X;
					pchord->m_rect.top = -100 - CENTER_Y;
					pchord->m_rect.bottom = -100 + CENTER_Y;
					//pchord->m_lastrect = pchord->m_rect;
					pchord->m_lastrect.left = pchord->m_rect.left;
					pchord->m_lastrect.right = pchord->m_rect.right;
					pchord->m_lastrect.top = pchord->m_rect.top;
					pchord->m_lastrect.bottom = pchord->m_rect.bottom;

					pchord->CleanupNexts();
				}
			}
			else if (dwid == ID_DMSIGNPOST)
			{
				ASSERT(FALSE);	// not implemented yet
			}
			else if (dwid == ID_SIGNPOST)
			{
				SignPost *psign = SignPost::Load(pIStream,lrecordsize);
				if (psign)
				{
					psign->SetNext(NULL);
				    m_pPersonality->m_signpostlist.AddTail(psign);
				}
			}
			else
			{
				StreamSeek( pIStream, lrecordsize, STREAM_SEEK_CUR );
			}
			lfilesize -= lrecordsize;
		}
		m_pPersonality->ResolveConnections();
		if (lfilesize)
		{
			TRACE("ERROR: Map read leaves filesize at %ld\n", lfilesize);
			return E_FAIL;
		}
	}
	else
	{
		StreamSeek( pIStream, lfilesize, STREAM_SEEK_CUR );
	}
	return(S_OK);		
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality::LoadPersonality

HRESULT CPersonality::LoadPersonality( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	IStream			*pIStream = NULL;
	IDMUSProdRIFFExt	*pIRIFFExt = NULL;
	MMCKINFO		ck;
	DWORD			dwByteCount;
	DWORD			dwSize;
	DWORD			dwPos;
	HRESULT			hr=S_OK;
	DMUS_IO_CHORDMAP		iPersonality;

	DMSubChordSet subchords;
	bool bDMSubChordSetRead = false;
	bool bDMChordMapRead = false;

	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

	while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
        case FOURCC_IOPERSONALITY:
			dwSize = min( ck.cksize, sizeof( iPersonality ) );
            hr = pIStream->Read( &iPersonality, dwSize, &dwByteCount );
			if( FAILED( hr )
			||  dwByteCount != dwSize )
			{
				hr = E_FAIL;
				goto ON_ERR;
			}
			
			if(m_pPersonality)
			{
		        WideCharToMultiByte( CP_ACP, 0, iPersonality.wszLoadName, -1, 
									 m_pPersonality->m_name, sizeof(m_pPersonality->m_name), 
									 NULL, NULL );
				strcpy( m_pPersonality->m_username, m_pPersonality->m_name );
				m_pPersonality->m_dwflags = iPersonality.dwFlags;
				if( m_pPersonality->m_dwflags & PERH_INVALID_BITS )
				{
					// There was a time (pre-DX8) when this field was not initialized
					 m_pPersonality->m_dwflags = 0;
				}
				if( m_pPersonality->m_dwflags & DMUS_CHORDMAPF_VERSION8 )
				{
					// Root is stored in high MSByte
					m_dwKey = (iPersonality.dwScalePattern & 0xFF000000) >> 24;
					m_pPersonality->m_scalepattern = iPersonality.dwScalePattern & 0x00FFFFFF;
				}
				else
				{
					m_pPersonality->m_scalepattern = iPersonality.dwScalePattern & 0x00FFFFFF;
					m_dwKey = m_pComponent->DetermineRoot( m_pPersonality->m_scalepattern );
					m_pPersonality->m_scalepattern = Rotate24( m_pPersonality->m_scalepattern, -(long)m_dwKey );
					m_pPersonality->m_dwflags |= DMUS_CHORDMAPF_VERSION8;
				}
			}
            break;

		case DM_FOURCC_GROOVE_LEVEL:
			hr = ReadDword(pIRiffStream, m_dwGroove);
			break;
		case FOURCC_PERSONALITYKEY:
			hr = ReadDword(pIRiffStream, m_dwKey);
			break;
		case FOURCC_PERSONALITYEDIT:
			hr = DMLoadUI(pIRiffStream, &ck);
			break;

		case FOURCC_PERSONALITYSTRUCT:
			hr = DMLoadStructure(pIRiffStream);
			break;

        case FOURCC_DMUSPROD_FILEREF:
			StreamSeek( pIStream, dwPos, 0 );
			hr = LoadJazzStyleReference(pIStream);
			break;

		case FOURCC_GUID:
			hr = pIStream->Read(&m_guidPersonality, sizeof(GUID), &dwByteCount);
			if( FAILED( hr )
			||  dwByteCount != ck.cksize )
			{
				hr = E_FAIL;
				goto ON_ERR;
			}
			break;

		case DM_FOURCC_VERSION_CHUNK:
			hr = DMLoadVersionChunk(pIRiffStream, &ck);
			if(FAILED(hr))
			{
				hr = E_FAIL;
				goto ON_ERR;
			}
			break;


		case FOURCC_SUBCHORD:			
			hr = subchords.Read(pIRiffStream, &ck);
			bDMSubChordSetRead = true;
			if(FAILED(hr))
			{
				hr = E_FAIL;
				goto ON_ERR;
			}
			break;

		case FOURCC_CHORDENTRYEDIT:
			ASSERT(bDMChordMapRead);
			if(bDMChordMapRead)
			{
				hr = ReadChordEntryEditChunk(pIRiffStream, &(m_pPersonality->m_chordlist), &ck);
			}
			break;

		case FOURCC_LIST:
			switch( ck.fccType )
			{

			case DM_FOURCC_INFO_LIST:
			case DM_FOURCC_UNFO_LIST:
				hr = DMLoadInfoChunk(pIRiffStream, &ck);
				if(FAILED(hr))
				{
					hr = E_FAIL;
					goto ON_ERR;
				}
				break;

			case FOURCC_LISTCHORDPALETTE:
				{
					ChordPaletteList chordpalette;
					if(!bDMSubChordSetRead)
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					hr = chordpalette.Read(pIRiffStream, &ck);
					if(FAILED(hr))
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					chordpalette.Insert(m_pPersonality->m_chordpalette, subchords);
				}
				break;

			case FOURCC_LISTCHORDMAP:
				{
					ChordMapList chordmap;
 					if(!bDMSubChordSetRead)
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					hr = chordmap.Read(pIRiffStream, &ck);
					if(FAILED(hr))
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					chordmap.Insert(m_pPersonality->m_chordlist, subchords);
				}
				bDMChordMapRead = true;
				break;

			case FOURCC_SIGNPOSTLIST:
				{
					SignPostListChunk signposts;
					if(!bDMSubChordSetRead)
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					hr = signposts.Read(pIRiffStream, &ck);
					if(FAILED(hr))
					{
						hr = E_FAIL;
						goto ON_ERR;
					}
					signposts.Insert(m_pPersonality->m_signpostlist, subchords);
				}
				break;
			}
		}
        pIRiffStream->Ascend( &ck, 0 );
		dwPos = StreamTell( pIStream );
    }


ON_ERR:
    if( FAILED( hr ) )
    {
		if( pIRIFFExt != NULL )
		{
			pIRIFFExt->Release();
		}
    }
	else
	{
		m_pPersonality->ResolveConnections();
	}

    if ( pIStream != NULL )
	{
		pIStream->Release();
	}

    return hr;

}


HRESULT CPersonality::SendChordMapAndPaletteToChordMapMgr()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;
	IPersistStream*	pIPS;
	LPSTREAM		pStream;
	LARGE_INTEGER	dlib;

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	dlib.LowPart = 0;
	dlib.HighPart = 0;
	if( SUCCEEDED( m_pComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStream )))
	{
		if( m_punkChordMapMgr )
		{
			if( SUCCEEDED( m_punkChordMapMgr->QueryInterface( IID_IPersistStream, (void**)&pIPS )))
			{
				// Just Persist the address of the chord list so ChordMapStripMgr can edit the
				// structure in realtime.
				DWORD	dwWritten = 0;
				DWORD	dwBuffer = (DWORD)&(m_pPersonality->m_chordlist);
				DWORD	dwBuffer2 = (DWORD)&(m_pPersonality->m_chordpalette);
				if( SUCCEEDED(pStream->Write( &dwBuffer, sizeof(dwBuffer), &dwWritten )) )
				{
					if( SUCCEEDED(pStream->Write(&dwBuffer2, sizeof(dwBuffer2), &dwWritten)))
					{
						if( SUCCEEDED(pStream->Write(&m_dwKey, sizeof(DWORD), &dwWritten)))
						{
							if(SUCCEEDED(pStream->Write(&m_selectiontype, sizeof(m_selectiontype), &dwWritten)))
							{
								if(SUCCEEDED(pStream->Write(&m_nidFromChord, sizeof(m_nidFromChord), &dwWritten)))
								{
									if(SUCCEEDED(pStream->Write(&m_nidToChord, sizeof(m_nidToChord), &dwWritten)))
									{
										pStream->Seek( dlib, STREAM_SEEK_SET, NULL ); //seek to beginning
										if( SUCCEEDED(pIPS->Load( pStream )) )
										{
											hr = S_OK;
										}
									}
								}
							}
						}
					}
				}
				pIPS->Release();
			}
		}
		pStream->Release();
	}

	if(hr == S_OK)
	{
		RefreshChordMap();
	}

	return hr;
}

DWORD CPersonality::GetSignPostListPtr()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return (DWORD)&(m_pPersonality->m_signpostlist);
}

HRESULT STDMETHODCALLTYPE CPersonality::OnDataChanged(IUnknown *punk)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr = S_OK;
	ChordEntry* pChordEntry;
	if(punk)
	{
		ChordChangeCallback* pCallback;
		PreEditNotification* pEditNotify;
		CheckForOrphansNotification* pCheckForOrphans;
		ZoomChangeNotification* pZoomChangeNotify;
		if(SUCCEEDED(punk->QueryInterface(IID_ChordChangeCallback, (void**)&pCallback)))
		{
			if(pCallback->GetCallbackType() == ChordChangeCallback::ParentShowProps)
			{
				OnShowProperties();
			}
			else if (pCallback->GetChordEntry())
			{
				pChordEntry = pCallback->GetChordEntry();
				ClearChordList();
				m_paChordChanges = new ChordExt;
				memset(m_paChordChanges, 0, sizeof(ChordExt));
				ChordEntryToChordExt(pChordEntry, m_paChordChanges);
				hr = SyncPersonalityWithEngine(syncPersonality+syncAudition);
				if( (pChordEntry->m_dwflags & CE_START) || (pChordEntry->m_dwflags & CE_END) )
				{
					LinkAll(false);
				}
				m_fModified = TRUE;
			}
			else
			{
				ASSERT(FALSE);
				hr = E_FAIL;
			}
			pCallback->Release();
		}
		else if(SUCCEEDED(punk->QueryInterface(IID_CheckForOrphansNotification, (void**)&pCheckForOrphans)))
		{
			LinkAll(false);
			pCheckForOrphans->Release();
		}
		else if(SUCCEEDED(punk->QueryInterface(IID_PreEditNotification, (void**)&pEditNotify)))
		{
			// save old state
			if(m_pUndoMgr)
			{
				m_pUndoMgr->SaveState(this, const_cast<char*>((LPCSTR)(pEditNotify->m_strReason)));
			}
			m_fModified = TRUE;
			pEditNotify->Release();
		}
		else if(SUCCEEDED(punk->QueryInterface(IID_ZoomChangeNotification, (void**)&pZoomChangeNotify)))
		{
			m_dblZoom = pZoomChangeNotify->m_dblZoom;
			Modified() = TRUE;
			pZoomChangeNotify->Release();
		}
		else
		{
			ASSERT(FALSE);
			hr = E_FAIL;
		}
		punk->Release();
	}
	else
	{

		hr = SyncPersonalityWithEngine(syncPersonality);
		m_fModified = TRUE;
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality::Modified
BOOL &CPersonality::Modified()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return m_fModified;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdTransport implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdTransport::Play

HRESULT CPersonality::Play(BOOL /*fPlayFromStart*/)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	HRESULT hr;

	ASSERT(m_pComponent != NULL);
	if(m_pComponent == NULL)
	{
		return E_UNEXPECTED;
	}

	CString str;
	if(m_paChordChanges == NULL)
	{
		str.LoadString(IDS_NOCHORD);
		AfxMessageBox(str);
		return E_FAIL;
	}

	IDMUSProdNode* pNode = GetStyleDocRoot();

	if(pNode == NULL)
	{
		SetStyleDocRootNode();
		pNode = GetStyleDocRoot();
		if(pNode)
			SyncPersonalityWithEngine(syncAudition);
		else
		{
			str.LoadString(IDS_NOSTYLE);
			AfxMessageBox(str);
			return E_FAIL;
		}
	}

	pNode->Release();

	if(m_pISection == NULL)
	{
		hr = SyncPersonalityWithEngine(syncAudition);
		if(FAILED(hr) || m_pISection == NULL)
		{
			str.LoadString(IDS_NOSTYLE);
			AfxMessageBox(str);
			return E_FAIL;
		}
	}


	hr = SyncPersonalityWithEngine(syncAudition);
/* this already done in sync personality
	if(SUCCEEDED(hr))
	{
		IDirectMusicPerformance* pIDMPerformance = 0;
		m_pComponent->m_pIConductor->GetPerformanceEngine( (IUnknown**)&pIDMPerformance);
		if(!pIDMPerformance)
		{
			hr = E_FAIL;
		}
		else
		{
			hr = pIDMPerformance->PlaySegment( m_pISection, DMUS_SEGF_BEAT, 0, &m_pIDMSegmentState );
			pIDMPerformance->Release();
		}
	}
*/
	if(FAILED(hr) && m_pISection)
	{
		m_pISection->Release();
		m_pISection = NULL;
	}
	return(hr);
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdTransport::Stop

HRESULT CPersonality::Stop(BOOL fStopImmediate)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_bSectionStopping = TRUE;
	StopIfPlaying( fStopImmediate ? 0 : DMUS_SEGF_DEFAULT );
	m_bSectionStopping = FALSE;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdTransport::Transition

HRESULT CPersonality::Transition()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdTransport::Record

HRESULT CPersonality::Record(BOOL fPressed)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPressed);
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdTransport::GetName

HRESULT CPersonality::GetName(BSTR* pbstrName)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

	// Segment file name
	CString strName;
	BSTR bstrFileName;

	IDMUSProdNode*	iNode;

	HRESULT hr = QueryInterface(IID_IDMUSProdNode, (void**)&iNode);

	if(!SUCCEEDED(hr))
		return E_UNEXPECTED;

	if( SUCCEEDED ( m_pComponent->m_pIFramework->GetNodeFileName( iNode, &bstrFileName ) ) )
	{
		// Node is in the Project tree
		strName = bstrFileName;
		::SysFreeString( bstrFileName );
		strName += _T(": ");
	}
	else
	{
		// Node has not yet been placed in the Project Tree
		// because the Segment is still in the process of being loaded
		strName = m_strOrigFileName;
		strName += _T(": ");
	}
	
	// Plus "name"
	strName += m_pPersonality->m_username;

    *pbstrName = strName.AllocSysString();
	
	iNode->Release();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdTransport::TrackCursor

HRESULT CPersonality::TrackCursor(BOOL fTrackCursor)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fTrackCursor);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality::StopIfPlaying()

void CPersonality::StopIfPlaying( DWORD dwFlags )
{
	ASSERT( m_pComponent );
	ASSERT( m_pComponent->m_pIConductor );
	// If this dialog is already playing, then Enable the Dialog's Timer.
	HRESULT hr = 
		m_pComponent->m_pIConductor->IsTransportPlaying( (IDMUSProdTransport*)this ) ;
	if (hr == S_OK)
	{
		IDirectMusicPerformance* pIDMPerformance = 0;
		m_pComponent->m_pIConductor->GetPerformanceEngine( (IUnknown**)&pIDMPerformance);
		if(!pIDMPerformance)
		{
			hr = E_FAIL;
		}
		else
		{
			pIDMPerformance->Stop( m_pISection, m_pIDMSegmentState, 0, dwFlags );
			pIDMPerformance->Release();
			if(m_pIDMSegmentState != NULL)
			{
				m_pIDMSegmentState->Release();
				m_pIDMSegmentState = NULL;
			}
		}
	}
}


namespace MyTime
{
	DWORD starttime;
void Time(char* s)
{
	UNREFERENCED_PARAMETER(s);
//	timeGetTime();
//	TRACE("%s done at %d\n", s, timeGetTime() - starttime);
}
void ResetTime()
{
//	starttime = timeGetTime();
}
}

using namespace MyTime;

///////////////////// Section Construction Methods
//
/////////////////////////////////////////////////////////////////////////////
// Personality IPersistStream::Save

HRESULT CPersonality::ComposeSection( IDirectMusicSegment** ppISection)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = S_OK;
	IDirectMusicTrack*			pIChordTrack			=	NULL;

	IDirectMusicTrack*			pICommandTrack			=	NULL;

	IDirectMusicTrack*			pIBandTrack				=	NULL;
	IDirectMusicBand*			pBand					=	NULL;

	IDirectMusicTrack*			pIStyleTrack			=	NULL;

	IDirectMusicTrack*			pITempoTrack		= NULL;

	long						nClocks					=	0;
	GUID						Guid;

	IDirectMusicStyle*			pIStyle					=	NULL;

	DMPolyChord*				pChord					=	NULL;

	DWORD nPatternLength = 120;	// highly divisible number to get integral number of pattern repeats
	int i;	// index for for loop

	// check to see if any chords to audition
	if(m_paChordChanges == NULL)
	{
		return E_FAIL;
	}
	else
	{
		pChord = dynamic_cast<DMPolyChord*>(m_paChordChanges);
	}

	// GetStyle from selected node
	hr = GetDirectMusicStyle(&pIStyle);
	if(!SUCCEEDED(hr))
	{
		return E_FAIL;
	}


	// create a section segment
	hr = ::CoCreateInstance(	CLSID_DirectMusicSegment,
								NULL,
								CLSCTX_INPROC,
								IID_IDirectMusicSegment,
								(void**)ppISection
							);

	if(!SUCCEEDED(hr))
		goto ON_END;

	// extract the style's timesig
	DMUS_TIMESIGNATURE	TimeSig;
	pIStyle->GetTimeSignature(&TimeSig);

	// convert m_paChordChanges to ChordTrack
	hr = ::CoCreateInstance(	CLSID_DirectMusicChordTrack,
								NULL,
								CLSCTX_INPROC,
								IID_IDirectMusicTrack,
								(void**)&pIChordTrack);
	if(!SUCCEEDED(hr))
		goto ON_END;

	DMUS_CHORD_PARAM	dmchord;

	memset(dmchord.wszName, 0, sizeof(dmchord.wszName));

#ifdef _UNICODE
	// use straight wstrcpy
	_wstrncpy(dmchord.wszName, pChord->Name(), 12);
#else
	// use conversion
    MultiByteToWideChar( CP_ACP, 0, pChord->Name(), -1, 
						dmchord.wszName, 12 );
#endif

	dmchord.wMeasure = 0;
	dmchord.bBeat = 0;
	dmchord.bSubChordCount = DMPolyChord::MAX_POLY;
	for(i = 0; i < DMPolyChord::MAX_POLY; i++)
	{
		DMUS_SUBCHORD* pdmsub = &(dmchord.SubChordList[i]);
		pdmsub->dwChordPattern = pChord->SubChord(i)->ChordPattern();
		pdmsub->dwScalePattern = pChord->SubChord(i)->ScalePattern();
		pdmsub->dwInversionPoints = pChord->SubChord(i)->InvertPattern();
		pdmsub->dwLevels = pChord->SubChord(i)->Levels();
		pdmsub->bChordRoot = pChord->SubChord(i)->ChordRoot();
		pdmsub->bScaleRoot = pChord->SubChord(i)->ScaleRoot();
	}

	Guid = GUID_ChordParam;
	hr = pIChordTrack->SetParam(Guid, 0, &dmchord);
	if(!SUCCEEDED(hr))
	{
		goto ON_END;
	}

	// now load command track
	hr = ::CoCreateInstance(	CLSID_DirectMusicCommandTrack,
								NULL,
								CLSCTX_INPROC,
								IID_IDirectMusicTrack,
								(void**)&pICommandTrack);
	if(!SUCCEEDED(hr))
	{
		goto ON_END;
	}

	DMUS_COMMAND_PARAM	dmcommand;
	memset(&dmcommand, 0, sizeof(dmcommand));
	dmcommand.bGrooveLevel = static_cast<BYTE>(m_dwGroove);
	Guid = GUID_CommandParam;
	hr = pICommandTrack->SetParam(Guid, 0, &dmcommand);
	if(!SUCCEEDED(hr))
	{
		goto ON_END;
	}

	// get length from patterns in style
	DWORD nMinLen, nMaxLen;
	hr = pIStyle->GetEmbellishmentLength(0, m_dwGroove, &nMinLen, &nMaxLen);
	if(!SUCCEEDED(hr))
	{
		goto ON_END;
	}

	// load style track
	hr = ::CoCreateInstance(	CLSID_DirectMusicStyleTrack,
								NULL,
								CLSCTX_INPROC,
								IID_IDirectMusicTrack,
								(void**)&pIStyleTrack);
	if(!SUCCEEDED(hr))
	{
		goto ON_END;
	}
	Guid = GUID_IDirectMusicStyle;
	hr = pIStyleTrack->SetParam(Guid, 0, pIStyle);
	if(!SUCCEEDED(hr))
	{
		goto ON_END;
	}

	// load band track
	hr = ::CoCreateInstance(	CLSID_DirectMusicBandTrack,
								NULL,
								CLSCTX_INPROC,
								IID_IDirectMusicTrack,
								(void**)&pIBandTrack);
	if(!SUCCEEDED(hr))
	{
		goto ON_END;
	}
	Guid = GUID_IDirectMusicBand;
	hr = pIStyle->GetDefaultBand(&pBand);
	if(!SUCCEEDED(hr))
	{
		goto NO_BAND;
	}
	else if(hr == S_FALSE)
	{
		// no default band, get first band in style
		WCHAR	name[MAX_PATH];
		hr = pIStyle->EnumBand(0, name);
		if(!SUCCEEDED(hr))
		{
			goto NO_BAND;
		}
		hr = pIStyle->GetBand(name, &pBand);
		if(!SUCCEEDED(hr))
		{
			goto NO_BAND;
		}
	}

	hr = pIBandTrack->SetParam(Guid, 0, pBand);
	if(!SUCCEEDED(hr))
	{
		goto NO_BAND;
	}

NO_BAND:
	hr = S_OK;

	// extract the style's tempo
	double	dblTempo;
	pIStyle->GetTempo(&dblTempo);

	// create tempo track
	hr = ::CoCreateInstance(	CLSID_DirectMusicTempoTrack,
								NULL,
								CLSCTX_INPROC,
								IID_IDirectMusicTrack,
								(void**)&pITempoTrack);
	if(!SUCCEEDED(hr))
		goto ON_END;

	DMUS_TEMPO_PARAM tempo;
	tempo.mtTime = 0;
	tempo.dblTempo = dblTempo;
	if( FAILED( pITempoTrack->SetParam(GUID_TempoParam, 0, &tempo) ) )
	{
			goto ON_END;
	}

	// initialize the segment and insert above tracks
	(*ppISection)->SetRepeats(999);
//	(*ppISection)->SetRepeats(8);
	nClocks = nPatternLength /* measure */ * TimeSig.bBeatsPerMeasure * (DMUS_PPQ * 4 / TimeSig.bBeat);
	(*ppISection)->SetLength(nClocks);
	(*ppISection)->InsertTrack(pIBandTrack, 1);
	(*ppISection)->InsertTrack(pIStyleTrack, 1);
	(*ppISection)->InsertTrack(pICommandTrack, 1);
	(*ppISection)->InsertTrack(pIChordTrack, 1);
	(*ppISection)->InsertTrack(pITempoTrack, 1);
	
 ON_END:

	if(pBand)
		pBand->Release();
	if(pIBandTrack)
		pIBandTrack->Release();
	if(pICommandTrack)
		pICommandTrack->Release();
	if(pIChordTrack)
		pIChordTrack->Release();
	if(pIStyle)
		pIStyle->Release();
	if(pIStyleTrack)
		pIStyleTrack->Release();
	if(pITempoTrack)
		pITempoTrack->Release();


    return hr;
}


// Save the chordlist to a normal stream
HRESULT CPersonality::SaveChordList( LPSTREAM pStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    HRESULT     hr=S_OK;
    DWORD       cb;
    WORD        wSize;
    ioChordSelection iChordSelection;
    ChordExt*   pChord;
    int         i;

//HRESULT GetStreamInfo(FileType* pftFileType, GUID* pguidDataFormat)

	DMUSProdStreamInfo	StreamInfo;//Added ECW 4/24/98
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}
	else
	{
		ASSERT(FALSE);
	}

	if(::IsEqualGUID(guidDataFormat, GUID_CurrentVersion))
	{
		wSize = sizeof( ioChordSelection );
	//	FixBytes( FBT_SHORT, &wSize );
		hr = pStream->Write( &wSize, sizeof( wSize ), &cb );
		if( FAILED( hr ) || cb != sizeof( wSize ) )
		{
			return E_FAIL;
		}

		for( pChord = m_paChordChanges ; pChord; pChord = pChord->pNext )
		{
			memset( &iChordSelection, 0, sizeof( iChordSelection ) );
			MultiByteToWideChar( CP_ACP, 0, pChord->Name(), -1, iChordSelection.wstrName, sizeof( iChordSelection.wstrName ) / sizeof( wchar_t ) );
			if( pChord->SubChord(0)->UseFlat() != 0 )
			{
				iChordSelection.fCSFlags |= CSF_FLAT;
			}
			iChordSelection.fCSFlags |= CSF_INSCALE;
			iChordSelection.fCSFlags |= CSF_KEYDOWN;
			if( ( pChord->SubChord(0)->Bits() & CHORD_SIMPLE ) != 0 )
			{
				iChordSelection.fCSFlags |= CSF_SIMPLE;
			}
			iChordSelection.bBeat = pChord->Beat();
			iChordSelection.wMeasure = pChord->Measure();
			ASSERT(DMPolyChord::MAX_POLY >= 4);
			for( i = 0 ; i < 4 ; ++i )
			{
				iChordSelection.aChord[i].lChordPattern = pChord->SubChord(i)->ChordPattern();
				iChordSelection.aChord[i].lScalePattern = pChord->SubChord(i)->ScalePattern();
				iChordSelection.aChord[i].lInvertPattern = pChord->SubChord(i)->InvertPattern();
				iChordSelection.aChord[i].bRoot = pChord->SubChord(i)->ChordRoot();
			}
	//		FixBytes( FBT_IOCHORDSELECTION, &iChordSelection );
			if( FAILED( pStream->Write( &iChordSelection, sizeof( iChordSelection), &cb ) ) ||
				cb != sizeof( iChordSelection ) )
			{
				hr = E_FAIL;
				break;
			}
		}
	}
	return hr;
}


// Save the chordlist to a JazzRIFFStream
HRESULT CPersonality::SaveChordList( IDMUSProdRIFFStream* pRIFF )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    IStream*    pStream;
    MMCKINFO    ck;
    HRESULT     hr;

	// don't save null chord list
	if(m_paChordChanges == NULL)
	{
		return S_OK;
	}

    pStream = pRIFF->GetStream();

    hr = E_FAIL;
    ck.ckid = FOURCC_CHORD;
    if( pRIFF->CreateChunk( &ck, 0 ) == 0 )
    {
		hr = SaveChordList( pStream );
        if( SUCCEEDED(hr) &&
            pRIFF->Ascend( &ck, 0 ) == 0 )
        {
            hr = S_OK;
        }
    }

    pStream->Release();
    return hr;
}

// Save the commandlist to a JazzRIFFStream
HRESULT CPersonality::SaveCommandList( IDMUSProdRIFFStream* pRIFF )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    IStream*    pStream;
    MMCKINFO    ck;
    HRESULT     hr;

	// don't save empty command list
	if(m_lstCommandList == NULL)
	{
		return S_OK;
	}

    pStream = pRIFF->GetStream();

    hr = E_FAIL;
    ck.ckid = FOURCC_COMMAND;
    if( pRIFF->CreateChunk( &ck, 0 ) == 0 )
    {
		hr = SaveCommandList( pStream );
		if( SUCCEEDED(hr) &&
			pRIFF->Ascend( &ck, 0 ) == 0 )
		{
			hr = S_OK;
		}
    }

    pStream->Release();
    return hr;
}

HRESULT CPersonality::SaveCommandList( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    DWORD       cb;
    WORD        wSize;
    ioCommand   iCommand;
    CommandExt* pCommand;
	HRESULT		hr = S_OK;

    wSize = sizeof( ioCommand );
//    FixBytes( FBT_SHORT, &wSize );
    hr = pIStream->Write( &wSize, sizeof( wSize ), &cb );
    if( FAILED( hr ) || cb != sizeof( wSize ) )
    {
        return E_FAIL; 
    }

    for( pCommand = m_lstCommandList ; pCommand != NULL ; pCommand = pCommand->pNext )
    {
        memset( &iCommand, 0, sizeof( iCommand ) );
        iCommand.lTime = pCommand->time;
        iCommand.dwCommand = pCommand->command;
//        FixBytes( FBT_IOCOMMAND, &iCommand );
        if( FAILED( pIStream->Write( &iCommand, sizeof( iCommand ), &cb ) ) ||
            cb != sizeof( iCommand ) )
        {
			hr = E_FAIL;
            break;
        }
    }

    return hr;
}

HRESULT CPersonality::GetDirectMusicStyle(IDirectMusicStyle** ppIStyle)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pStyleDocRoot = NULL;
	IDMUSProdReferenceNode* pIStyleRef = NULL;
	
	HRESULT hr = m_pIStyleRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void**)&pIStyleRef);
	if(FAILED(hr))
	{
		return E_FAIL;
	}
	hr = pIStyleRef->GetReferencedFile(&pStyleDocRoot);
	pIStyleRef->Release();
	pIStyleRef = NULL;
	if(FAILED(hr))
	{
		return E_FAIL;
	}

	if(pStyleDocRoot)
	{
		hr = pStyleDocRoot->GetObject(CLSID_DirectMusicStyle, IID_IDirectMusicStyle, (void**)ppIStyle);
		pStyleDocRoot->Release();
	}
	return hr;
}

void CPersonality::ClearChordList()
{
	ChordExt *pTemp;

	while(m_paChordChanges)
	{
		pTemp = m_paChordChanges->pNext;
		delete m_paChordChanges;
		m_paChordChanges = pTemp;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality::AdviseConnectionPoint()

HRESULT CPersonality::AdviseConnectionPoint()
{
	HRESULT hr = E_FAIL;

	if( m_bAdvised )
	{
		return S_OK;
	}

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIConductor != NULL );
	if ( !m_pComponent || !m_pComponent->m_pIConductor )
	{
		return E_UNEXPECTED;
	}

	// Register Segment with Conductor's Transport and notification system
	if( FAILED( m_pComponent->m_pIConductor->RegisterTransport( this, 0 ) ) )
	{
		return E_FAIL;
	}
	if( FAILED( m_pComponent->m_pIConductor->RegisterNotify( this, GUID_NOTIFICATION_SEGMENT ) ) )
	{
		return E_FAIL;
	}

	// Tell Conductor we are the active node in case it doesn't know already
	// this causes personality to get transport when it is opened instead of when it gets the forcus
//	if ( FAILED( m_pComponent->m_pIConductor->SetActiveTransport((IDMUSProdTransport *)this, NULL ) ) )
//	{
//		return E_FAIL;
//	}

	m_bAdvised = TRUE;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality::UnAdviseConnectionPoint()

HRESULT CPersonality::UnAdviseConnectionPoint()
{
	if(m_bAdvised == FALSE)
	{
		return S_OK;
	}

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIConductor != NULL );
	if ( !m_pComponent || !m_pComponent->m_pIConductor )
	{
		return E_UNEXPECTED;
	}

	// Make sure Segment is not playing
	if ( FAILED( Stop( TRUE ) ) )
	{
		return E_FAIL;
	}

	// Unregister the Transport's Play, Stop, Transition functionality.
	if( FAILED( m_pComponent->m_pIConductor->UnRegisterTransport( this ) ) )
	{
		return E_FAIL;
	}

	// Unregister notification messages
	if( FAILED( m_pComponent->m_pIConductor->UnregisterNotify( this, GUID_NOTIFICATION_SEGMENT ) ) )
	{
		return E_FAIL;
	}

	m_bAdvised = FALSE;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNotifyCPt implementation

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNotifyCPt::OnNotify

HRESULT CPersonality::OnNotify(ConductorNotifyEvent* pConductorNotifyEvent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pConductorNotifyEvent != NULL );
	if( !pConductorNotifyEvent )
	{
		return E_POINTER;
	}

	switch( pConductorNotifyEvent->m_dwType )
	{
		case DMUS_PMSGT_NOTIFICATION:
		{
			DMUS_NOTIFICATION_PMSG* pNotifyEvent = (DMUS_NOTIFICATION_PMSG *)pConductorNotifyEvent->m_pbData;

			// Handle GUID_NOTIFICATION_SEGMENT notifications
			if( ::IsEqualGUID ( pNotifyEvent->guidNotificationType, GUID_NOTIFICATION_SEGMENT ) )
			{
				if( pNotifyEvent->punkUser == m_pIDMSegmentState )
				{
					switch( pNotifyEvent->dwNotificationOption )
					{
						case DMUS_NOTIFICATION_SEGSTART:
							break;
						case DMUS_NOTIFICATION_SEGLOOP:
							break;

						case DMUS_NOTIFICATION_SEGABORT:
						case DMUS_NOTIFICATION_SEGEND:
							// Notify the Conductor we stopped playing
							if( m_pComponent && m_pComponent->m_pIConductor )
							{
								m_pComponent->m_pIConductor->TransportStopped( (IDMUSProdTransport*) this );
							}
							else
							{
								ASSERT(FALSE);
							}
							break;
					}
				}
			}
		}
	}

	return S_OK;
}



////////////////////////////////////// more helper funcs
BOOL CPersonality::UpdateSelectedChordList()
{
	ClearChordList();
	ChordEntry* pChord = m_pPersonality->m_chordlist.GetHead();
	for(; pChord; pChord = pChord->GetNext())
	{
		if(pChord->m_chordsel.KeyDown())
		{
			if(m_paChordChanges)
			{
				pChord->m_chordsel.KeyDown() = FALSE;
			}
			else
			{
				m_paChordChanges = new ChordExt;
				memset(m_paChordChanges, 0, sizeof(ChordExt));
				ChordEntryToChordExt(pChord, m_paChordChanges);
			}
		}
	}
	return m_paChordChanges ? TRUE : FALSE;
}


HRESULT CPersonality::SaveJazzStyleReference(IStream* pIStream)
{
	if(m_pIStyleRefNode == NULL)
	{
		return S_OK;
	}


	HRESULT hr;
	IDMUSProdReferenceNode* pIDMUSProdReferenceNode;

	hr = m_pIStyleRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void **)&pIDMUSProdReferenceNode);
	ASSERT(SUCCEEDED(hr));
	if(FAILED(hr))
	{
		return E_UNEXPECTED;
	}

	IDMUSProdNode* pIDocRootNode;
	hr = pIDMUSProdReferenceNode->GetReferencedFile(&pIDocRootNode);
//	ASSERT(SUCCEEDED(hr));
	if(FAILED(hr))
	{
		// nothing to save
		pIDMUSProdReferenceNode->Release();
		return S_OK;
	}

	pIDMUSProdReferenceNode->Release();
	
	if(pIDocRootNode == NULL)
	{
		return S_OK;	// nothing to save
	}

	IDMUSProdFileRefChunk* pIFileRefChunk;
	hr = m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdFileRefChunk, (void **)&pIFileRefChunk);
	ASSERT(SUCCEEDED(hr));
	if(FAILED(hr))
	{
		pIDocRootNode->Release();
		return E_UNEXPECTED;
	}

	hr = pIFileRefChunk->SaveRefChunk(pIStream, pIDocRootNode);
	pIFileRefChunk->Release();
	ASSERT(SUCCEEDED(hr));
	if(FAILED(hr))
	{
		pIDocRootNode->Release();
		return E_FAIL;
	}

	pIDocRootNode->Release();
	return S_OK;
}

HRESULT CPersonality::LoadJazzStyleReference(IStream* pIStream)
{
	// release current style reference
	if(m_pIStyleRefNode)
	{
		m_pIStyleRefNode->Release();
		m_pIStyleRefNode = 0;
	}
	// Get the IDMUSProdFileRefChunk
	IDMUSProdFileRefChunk* pIFileRefChunk;
	HRESULT hr;
	hr = m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdFileRefChunk, (void **)&pIFileRefChunk);
	ASSERT(SUCCEEDED(hr));
	if(FAILED(hr))
	{
		return E_UNEXPECTED;
	}

	// Load the chunk
	IDMUSProdNode* pIDocRootNode;
	hr = pIFileRefChunk->LoadRefChunk(pIStream, &pIDocRootNode);
	pIFileRefChunk->Release();

	if(FAILED(hr))
	{
		return E_FAIL;
	}

	GUID guidNodeId;
	hr = pIDocRootNode->GetNodeId(&guidNodeId);

	if(FAILED(hr))
	{
		return E_UNEXPECTED;
	}

	if(IsEqualGUID(guidNodeId, GUID_StyleNode))
	{

		if(m_pIStyleRefNode == NULL)
		{
			// Allocate a new reference node
			IDMUSProdComponent* pIStyleComponent;
			hr = m_pComponent->m_pIFramework->FindComponent( CLSID_StyleComponent,  &pIStyleComponent );
			if(FAILED(hr))
			{
				return E_FAIL;
			}
			hr = pIStyleComponent->AllocReferenceNode(GUID_StyleRefNode, &m_pIStyleRefNode);
			RELEASE(pIStyleComponent);
			if(FAILED(hr))
			{
				return  E_OUTOFMEMORY;
			}
		}
		
		ASSERT(m_pIStyleRefNode);
//		IDMUSProdReferenceNode* pIDMUSProdReferenceNode;

//		hr = m_pIStyleRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void **)&pIDMUSProdReferenceNode);
//		ASSERT(SUCCEEDED(hr));
//		if(FAILED(hr))
//		{
//			return E_UNEXPECTED;
//		}

//		hr = pIDMUSProdReferenceNode->SetReferencedFile(pIDocRootNode);
		hr = SetStyleReference(pIDocRootNode);

		pIDocRootNode->Release();
//		pIDMUSProdReferenceNode->Release();
		ASSERT(SUCCEEDED(hr));
		if(FAILED(hr))
		{
			return E_UNEXPECTED;
		}
	}
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////
// CPersonality :: OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData)

HRESULT CPersonality::OnUpdate(IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdReferenceNode* pIReferenceNode;
	IDMUSProdNode* pIDocRoot;

	HRESULT hr = E_FAIL;

	// Is it our node
	if( m_pIStyleRefNode )
	{
		if( SUCCEEDED ( m_pIStyleRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void**)&pIReferenceNode ) ) )
		{
			if( SUCCEEDED ( pIReferenceNode->GetReferencedFile( &pIDocRoot ) ) )
			{
				if( pIDocRoot == pIDocRootNode )
				{
					// Notification is from our Style node
					if( ::IsEqualGUID(guidUpdateType, FRAMEWORK_FileDeleted )
					||  ::IsEqualGUID(guidUpdateType, FRAMEWORK_FileClosed ) )
					{
						// node deleted or closed
						SetStyleReference( NULL );
						if( theApp.m_pIPageManager
						&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_PersonalityPageManager ) == S_OK )
						{
							theApp.m_pIPageManager->RefreshData();
						}
						SyncPersonalityWithEngine(syncAudition);
						hr = S_OK;
					}
					else if( ::IsEqualGUID(guidUpdateType, FRAMEWORK_FileReplaced ) )
					{
						// node replaced with another node pointer
						SetStyleReference( NULL );
						if( pData )
						{
							SetStyleReference( (IDMUSProdNode *)pData );
							if( theApp.m_pIPageManager
							&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_PersonalityPageManager ) == S_OK )
							{
								theApp.m_pIPageManager->RefreshData();
							}
							SyncPersonalityWithEngine(syncAudition);
							hr = S_OK;
						}
						else
						{
							hr = E_UNEXPECTED;
						}
					}
					else if( ::IsEqualGUID(guidUpdateType, DOCROOT_GuidChange ) )
					{
						// Style GUID changed so we have to resave the Personality
						m_fModified = TRUE;
						hr = S_OK;
					}
					else if( ::IsEqualGUID(guidUpdateType, STYLE_NameChange ) )
					{
						if( theApp.m_pIPageManager
						&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_PersonalityPageManager ) == S_OK )
						{
							theApp.m_pIPageManager->RefreshData();
						}
						m_fModified = TRUE;
						hr = S_OK;
					}
					else if( ::IsEqualGUID(guidUpdateType, STYLE_TimeSigChange ) )
					{
//						Update UI?
						SyncPersonalityWithEngine(syncAudition);
						hr = S_OK;
					}
				}

				if( pIDocRoot )
				{
					pIDocRoot->Release();
					pIDocRoot = NULL;
				}
			}

			pIReferenceNode->Release();
		}
	}

	// is from our node?
	{
		IDMUSProdNode* pNode;
		hr = QueryInterface(IID_IDMUSProdNode, (void**)&pNode);
		if(SUCCEEDED(hr))
		{
			// Notification is from this Personality node
			if( pIDocRootNode == pNode && ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileNameChange ) 
			||  pIDocRootNode == pNode && ::IsEqualGUID( guidUpdateType, FRAMEWORK_AfterFileSave ) )
			{
				OnNameChange();
				hr = S_OK;
			}
			pNode->Release();
		}
	}



	return hr;
}

///////////////////////////////////////////////////////////////////////////
// CPersonality :: GetChordMapInfo(DMUSProdChordMapInfo* pChordMapInfo)

HRESULT CPersonality::GetChordMapInfo(DMUSProdChordMapInfo* pChordMapInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pChordMapInfo == NULL
	||  pChordMapInfo->wSize == 0 )
	{
		return E_INVALIDARG;
	}

	if( m_pPersonality == NULL )
	{
		return E_UNEXPECTED;
	}

	// Initialize structure
	WORD wSize = pChordMapInfo->wSize;
	memset( pChordMapInfo, 0, wSize );
	pChordMapInfo->wSize = wSize;

	// Fill in structure
	pChordMapInfo->lScalePattern = m_pPersonality->m_scalepattern;
	pChordMapInfo->bKey = (BYTE)GetAbsKey();
	pChordMapInfo->fUseFlats = (m_dwKey & CPersonality::UseFlats) ? TRUE : FALSE;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////
// CPersonality :: GetChordMapUIInfo(DMUSProdChordMapUIInfo* pChordMapUIInfo)

HRESULT CPersonality::GetChordMapUIInfo(DMUSProdChordMapUIInfo* pChordMapUIInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pChordMapUIInfo == NULL
	||  pChordMapUIInfo->wSize == 0 )
	{
		return E_INVALIDARG;
	}

	// Initialize structure
	WORD wSize = pChordMapUIInfo->wSize;
	memset( pChordMapUIInfo, 0, wSize );
	pChordMapUIInfo->wSize = wSize;

	// Fill in structure
	pChordMapUIInfo->fLockAllScales = m_fLockAllScales;
	pChordMapUIInfo->fSyncLevelOneToAll = m_fSyncLevelOneToAll;

	return S_OK;
}

#define MIN_SIZE_CHORDMAP_UI_STRUCT	8

///////////////////////////////////////////////////////////////////////////
// CPersonality :: SetChordMapUIInfo(DMUSProdChordMapUIInfo* pChordMapUIInfo)

HRESULT CPersonality::SetChordMapUIInfo(DMUSProdChordMapUIInfo* pChordMapUIInfo)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pChordMapUIInfo == NULL
	||  pChordMapUIInfo->wSize < MIN_SIZE_CHORDMAP_UI_STRUCT )
	{
		return E_INVALIDARG;
	}

	// Update UI fields
	m_fLockAllScales = pChordMapUIInfo->fLockAllScales ? true : false;
	m_fSyncLevelOneToAll = pChordMapUIInfo->fSyncLevelOneToAll ? true : false;

	// Check pChordMapUIInfo->wSize before updating additional fields!

	m_fModified = TRUE;
	return S_OK;
}

//#pragma optimize("", off)
HRESULT CPersonality::SetStyleReference(IDMUSProdNode* pIStyleDocRootNode)
{
	HRESULT hr=S_OK;
	IDMUSProdNode* pOldDocRoot = GetStyleDocRoot();
	IDMUSProdNode* thisnode = static_cast<IDMUSProdNode*>(this);

	IDMUSProdReferenceNode* pRef = 0;
	if(m_pIStyleRefNode == NULL)
		return E_UNEXPECTED;
	hr = m_pIStyleRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void**)&pRef);
	if(SUCCEEDED(hr))
	{
		if(pOldDocRoot)
		{
			// turn off notifications for this node
			hr = m_pComponent->m_pIFramework->RemoveFromNotifyList(pOldDocRoot, thisnode);
			pRef->SetReferencedFile(NULL);
			pOldDocRoot->Release();
		}

		if(pIStyleDocRootNode)
		{
			hr = pRef->SetReferencedFile(pIStyleDocRootNode);
			// turn on notifications
			hr = m_pComponent->m_pIFramework->AddToNotifyList(pIStyleDocRootNode, thisnode);
			m_fModified = TRUE;
		}

		pRef->Release();
	}
	return hr;
}
//#pragma optimize("", on)

IDMUSProdNode* CPersonality::GetStyleDocRoot()
// returns NULL if TRUE == IsNullReference 
// returned pointer needs to be released if non-null
{
	IDMUSProdReferenceNode* pRef = NULL;
	IDMUSProdNode* pDoc = NULL;
	if(m_pIStyleRefNode == NULL)
	{
		return NULL;
	}
	if(SUCCEEDED(m_pIStyleRefNode->QueryInterface(IID_IDMUSProdReferenceNode, (void**)&pRef)))
	{
		if(SUCCEEDED(pRef->GetReferencedFile(&pDoc)))
		{
		}
		pRef->Release();
	}
	return pDoc;
}


void CPersonality::SetChordChanges(DMPolyChord&pc)
{
	ClearChordList();
	m_paChordChanges = new ChordExt;
	*dynamic_cast<DMPolyChord*>(m_paChordChanges) = pc;
}

HRESULT CPersonality::DMSave(IStream * pIStream, BOOL fClearDirty)
{
	AFX_MANAGE_STATE( _afxModuleAddrThis);

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdPersistInfo* pPersistInfo = NULL;
	DMUSProdStreamInfo	StreamInfo;//Added ECW 4/24/98
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	if(SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo, (void**)&pPersistInfo)))
	{
		pPersistInfo->GetStreamInfo(&StreamInfo);
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

	IDMUSProdRIFFStream* pIRiffStream;
	MMCKINFO ckMain;

	HRESULT hr = E_FAIL;

	hr = AllocRIFFStream(pIStream, &pIRiffStream);
	if(SUCCEEDED(hr))
	{
		ckMain.fccType = FOURCC_PERSONALITY;
		if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATERIFF ) == 0
			&& SUCCEEDED( DMSavePersonality(pIRiffStream, ftFileType))
			&& pIRiffStream->Ascend(&ckMain, 0) == 0)
		{
			if(fClearDirty)
			{
				Modified() = FALSE;
			}
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}
			pIRiffStream->Release();
	}
	return hr;
}

HRESULT CPersonality::DMSavePersonality(IDMUSProdRIFFStream * pIRiffStream, FileType ftFileType)
{
	HRESULT hr;
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream != NULL);
	if(!pIStream)
		return E_FAIL;

	if(ftFileType == FT_DESIGN)
	{
		SetSelectionType();			// for persisting what was selected on chord map
	}

	// write ioPersonality chunk
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_CHORDMAP personality;

	ck.ckid = FOURCC_IOPERSONALITY;
	if(pIRiffStream->CreateChunk(&ck,0) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	// prepare ioPersonality chunk
	MultiByteToWideChar( CP_ACP, 0, m_pPersonality->m_name, -1, personality.wszLoadName, (size_t)20 );
	personality.dwScalePattern = m_pPersonality->m_scalepattern;
	personality.dwScalePattern |= GetAbsKey() << 24;	// MSByte is Root
	personality.dwFlags = m_pPersonality->m_dwflags;
	personality.dwFlags |= DMUS_CHORDMAPF_VERSION8;		// Scale saved in DX8 format.

	// write ioPersonality chunk
	pIStream->Write(&personality, sizeof(personality), &dwBytesWritten);
	if(pIRiffStream->Ascend(&ck, 0) == 0 && dwBytesWritten == sizeof(personality))
	{
		hr = S_OK;
	}
	else
	{
		hr = E_FAIL;
	}

	if(!SUCCEEDED(hr))
	{
		goto ON_ERROR;
	}

	// write guid chunk
	ck.ckid = FOURCC_GUID;
	if(pIRiffStream->CreateChunk(&ck,0) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	pIStream->Write(&m_guidPersonality, sizeof(GUID),&dwBytesWritten);
	hr = pIRiffStream->Ascend(&ck, 0)  == 0 && dwBytesWritten == sizeof(GUID) ? S_OK : E_FAIL;

	if(!SUCCEEDED(hr))
	{
		goto ON_ERROR;
	}
		
	// write version chunk
	if(!SUCCEEDED(DMSaveVersion(pIRiffStream)))
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// write info chunk.
	if(!SUCCEEDED(DMSaveInfoList(pIRiffStream)))
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// write chord data
	if(!SUCCEEDED(DMSaveChordData(pIRiffStream, ftFileType == FT_DESIGN)))
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// write personality key
	{
		WriteChunk chunk(pIRiffStream, FOURCC_PERSONALITYKEY);
		if(!SUCCEEDED(WriteDWord(pIRiffStream, m_dwKey)))
		{
			goto ON_ERROR;
		}
	}

	// write personality groove
	{
		WriteChunk chunk(pIRiffStream, DM_FOURCC_GROOVE_LEVEL);
		if(!SUCCEEDED(WriteDWord(pIRiffStream, m_dwGroove)))
		{
			goto ON_ERROR;
		}
	}

	// if design time, write PersonalityEdit
	if(ftFileType == FT_DESIGN)
	{
		if(pIStream)
		{
			hr = SaveJazzStyleReference(pIStream);
		}
		if(!SUCCEEDED(DMSaveUI(pIRiffStream)))
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}
	else	// write Personality Structure info
	{
		if(!SUCCEEDED(DMSaveStructure(pIRiffStream)))
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

ON_ERROR:
	pIStream->Release();
	return hr;
}

HRESULT CPersonality::DMSaveInfoList(IDMUSProdRIFFStream * pIRiffStream)
{
	IStream* pIStream;
	HRESULT hr=S_OK;
    MMCKINFO ckMain;
    MMCKINFO ck;
/* need to save empty chunk else undo won't properly update tab
	if( m_strName.IsEmpty()
	&&  m_strCategoryName.IsEmpty()
	&&  m_strAuthor.IsEmpty()
	&&  m_strCopyright.IsEmpty()
	&&  m_strSubject.IsEmpty()
	&&  m_strInfo.IsEmpty() )
	{
		return S_OK;
	}
*/
    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write INFO LIST header
	ckMain.fccType = DM_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Personality name
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

	// Write Personality category
	if( !m_strCategoryName.IsEmpty() )
	{
		ck.ckid = DM_FOURCC_CATEGORY_CHUNK;
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

	// Write Personality author
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

	// Write Personality copyright
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

	// Write Personality subject
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

	// Write Personality comments
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
    pIStream->Release();
    return hr;
}

HRESULT CPersonality::DMSaveVersion(IDMUSProdRIFFStream * pIRiffStream)
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	ioPersonalityVersion oDMPersonalityVersion;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write PersonalityVersion chunk header
	ck.ckid = DM_FOURCC_VERSION_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioDMPersonalityVersion structure
	memset( &oDMPersonalityVersion, 0, sizeof(ioPersonalityVersion) );

	oDMPersonalityVersion.m_dwVersionMS = m_dwVersionMS ;
	oDMPersonalityVersion.m_dwVersionLS = m_dwVersionLS;

	// Write PersonalityVersion chunk data
	hr = pIStream->Write( &oDMPersonalityVersion, sizeof(ioPersonalityVersion), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioPersonalityVersion) )
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
    pIStream->Release();
    return hr;
}

HRESULT CPersonality::DMSaveUI(IDMUSProdRIFFStream * pIRiffStream)
{
	IStream* pIStream;
	DWORD dwBytesWritten;
	HRESULT hr;
	MMCKINFO ck;
	ck.ckid = FOURCC_PERSONALITYEDIT;
	if(pIRiffStream->CreateChunk(&ck,0) != 0)
	{
		return E_FAIL;
	}
	// prepare ioPersonalityEdit
	ioPersonalityEdit2 edit;
    MultiByteToWideChar( CP_ACP, 0, m_pPersonality->m_username, 
										-1, edit.wchUserName, sizeof( edit.wchUserName ) / sizeof( wchar_t ) );
    MultiByteToWideChar( CP_ACP, 0, m_pPersonality->m_description, 
										-1, edit.wchDescription, sizeof( edit.wchDescription ) / sizeof( wchar_t ) );
	edit.bVariableNotFixed = (m_fVariableNotFixed == TRUE);
	edit.wChordMapLength = m_nChordMapLength;
	edit.fUseTimeSignature = m_fUseTimeSignature;
	edit.lTimeSignature = MAKE_TIMESIG(m_nBPM, m_nBeat) ;
	edit.bKeySig = 0;	// this info stored in FOURCC_PERSONALITYKEY chunk

	// for next several fields: window and selection state not implemented
	edit.wSplitterYPos = GetSplitterPos();	
	edit.wScrollHorPos = 0;
	edit.wScrollVertPos = 0;

	edit.selection = static_cast<BYTE>(m_selectiontype);
	edit.wConnectionIdSelectedChord = 0;		// won't need this, info is in ioChordEntry keydown
	edit.wConnectionIdSelectedFromChord = m_nidFromChord;
	edit.wConnectionIdSelectedToChord = m_nidToChord;

	edit.wSelectedSignpostRow = 0;
	edit.wSelectedSignpostCol = 0;

	edit.fLockAllScales = m_fLockAllScales;
	edit.fSyncLevelOneToAll = m_fSyncLevelOneToAll;

	// check whether zoom has changed
	// need to do this here because chord dialog can't notify us if it has changed.
	if(m_pChordDlg)
	{
		double dblZoom = m_pChordDlg->GetZoom();
		if(dblZoom != m_dblZoom)
		{
			m_dblZoom = dblZoom;
		}

		const long lFunctionbarWidth = m_pChordDlg->GetFunctionbarWidth();
		if( lFunctionbarWidth != m_lFunctionbarWidth )
		{
			m_lFunctionbarWidth = lFunctionbarWidth;
		}
	}

	edit.dblZoom = m_dblZoom;
	edit.lFunctionbarWidth = m_lFunctionbarWidth;

	// write ioPersonalityEdit chunk
	pIStream = pIRiffStream->GetStream();
	if(pIStream)
	{
		pIStream->Write(&edit, sizeof(ioPersonalityEdit2), &dwBytesWritten);	
		hr = (pIRiffStream->Ascend(&ck, 0) == S_OK || dwBytesWritten == sizeof(ioPersonalityEdit2))
			? S_OK : E_FAIL;
	}
	else
	{
		hr = E_FAIL;
	}


	if(pIStream)
		pIStream->Release();
	return hr;
}



HRESULT CPersonality::DMSaveChordData(IDMUSProdRIFFStream * pIRiffStream, bool bIncludeDesignInfo)
{
	DMSubChordSet subchordset;
	ChordPaletteList chordpalette;
	ChordMapList	chordmap;
	SignPostListChunk	  signposts;

	HRESULT hr = S_OK;

	hr = chordpalette.Extract(m_pPersonality->m_chordpalette, subchordset)
			||  chordmap.Extract(m_pPersonality->m_chordlist, subchordset, false)
			|| signposts.Extract(m_pPersonality->m_signpostlist, subchordset, bIncludeDesignInfo);

	ASSERT(pIRiffStream);
	if(pIRiffStream == 0)
	{
		return E_INVALIDARG;
	}

	if(hr == S_OK)
	{
		hr = subchordset.Write(pIRiffStream)
				|| chordpalette.Write(pIRiffStream)
				|| chordmap.Write(pIRiffStream)
				|| signposts.Write(pIRiffStream);
		if(hr == S_OK /* && bIncludeDesignInfo */)
		{
			hr = WriteChordEntryEditChunk(pIRiffStream, &(m_pPersonality->m_chordlist));
		}
	}
	return hr;
}


HRESULT CPersonality::DMLoadVersionChunk(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent)
{
	ASSERT(pIRiffStream);
	ASSERT(pckParent);
	if(pIRiffStream == NULL || pckParent == NULL)
	{
		return E_INVALIDARG;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	ioPersonalityVersion oDMPersonalityVersion;
	DWORD dwBytesRead;
	if(SUCCEEDED(pIStream->Read(&oDMPersonalityVersion, sizeof(oDMPersonalityVersion), &dwBytesRead))
		&& dwBytesRead == sizeof(oDMPersonalityVersion))
	{
		m_dwVersionMS = oDMPersonalityVersion.m_dwVersionMS;
		m_dwVersionLS = oDMPersonalityVersion.m_dwVersionLS;
	}
	else
	{
		hr = E_FAIL;
	}

	pIStream->Release();
	return hr;
}

HRESULT CPersonality::DMLoadInfoChunk(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent)
{
	ASSERT(pIRiffStream);
	ASSERT(pckParent);
	if(pIRiffStream == NULL || pckParent == NULL)
	{
		return E_INVALIDARG;
	}

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}
	MMCKINFO ckList;

	m_strName = "";
	m_strAuthor = "";
	m_strCopyright = "";
	m_strSubject = "";
	m_strInfo = "";
	m_strCategoryName = "";
	while( pIRiffStream->Descend( &ckList, pckParent, 0 ) == 0 )
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

			case DM_FOURCC_CATEGORY_CHUNK:
				ReadMBSfromWCS( pIStream, ckList.cksize, &m_strCategoryName );
				break;
		}
		pIRiffStream->Ascend( &ckList, 0 );
	}

	if(pIStream)
		pIStream->Release();
	return S_OK;
}

HRESULT CPersonality::DMLoadUI(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent)
{
	ASSERT(pIRiffStream);
	ASSERT(pckParent);
	if(pIRiffStream == NULL || pckParent == NULL)
	{
		return E_INVALIDARG;
	}
	ioPersonalityEdit2 edit;
	DWORD dwBytesRead;
	HRESULT hr = S_OK;

	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	DWORD dwTotalBytes = pckParent->cksize;
	ASSERT(dwTotalBytes <= sizeof(ioPersonalityEdit2));
	
	dwTotalBytes = min( sizeof(ioPersonalityEdit2), dwTotalBytes );

	hr = pIStream->Read(&edit, dwTotalBytes, &dwBytesRead);
	if(FAILED(hr) || dwBytesRead < sizeof(ioPersonalityEdit))
	{
		hr = E_FAIL;
	}
	else
	{
		WideCharToMultiByte(CP_ACP, 0, edit.wchUserName, -1, m_pPersonality->m_username, sizeof(m_pPersonality->m_username), 0,0);
		WideCharToMultiByte(CP_ACP, 0, edit.wchDescription, -1, m_pPersonality->m_description, sizeof(m_pPersonality->m_description), 0,0);
		m_fVariableNotFixed = edit.bVariableNotFixed ? TRUE : FALSE;
		m_nChordMapLength = edit.wChordMapLength;
		m_fUseTimeSignature = edit.fUseTimeSignature;
		m_nBPM = TIMESIG_BPM(edit.lTimeSignature);
		m_nBeat = TIMESIG_BEAT(edit.lTimeSignature);

		SetSplitterPos(edit.wSplitterYPos);

		m_selectiontype = edit.selection;
		m_nidToChord = edit.wConnectionIdSelectedToChord;
		m_nidFromChord = edit.wConnectionIdSelectedFromChord;

		if(dwTotalBytes >= sizeof(ioPersonality) + sizeof(double))
		{
			m_dblZoom = edit.dblZoom;
			if(m_dblZoom <= 0.0)
				m_dblZoom = 0.03125;
		}

		m_lFunctionbarWidth = edit.lFunctionbarWidth;

		m_fLockAllScales = edit.fLockAllScales;
		m_fSyncLevelOneToAll = edit.fSyncLevelOneToAll;
	}	

	pIStream->Release();
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CPersonality IDMUSProdNode::GetObject

HRESULT CPersonality::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Must be asking for an interface to a CLSID_DMStyle object
	if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicChordMap ) )
	{
		if( m_pIDirectMusicChordMap )
		{
			return m_pIDirectMusicChordMap->QueryInterface( riid, ppvObject );
		}
	}

    *ppvObject = NULL;
    return E_NOINTERFACE;
}



////////////////////////////////////////////////////////// Personality static funcs

// conversions between personality key and root, flat/sharp
bool CPersonality::GetFlatsFlag(DWORD key)
{
	return (key & UseFlats) != 0;
}

void CPersonality::SetFlatsFlag(bool useflats, DWORD& key)
{
	if(useflats)
	{
		key |= CPersonality::UseFlats;
	}
	else
	{
		key &= ~CPersonality::UseFlats;
	}
}

static char *convert[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
static char *flatconvert[] = {"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"};

void CPersonality::RootToString(char *string, DWORD key)
{
	bool useflats = GetFlatsFlag(key);
	SetFlatsFlag(false, key);
	if(useflats)
	{
		wsprintf( string,"%d %s",1 + key/12, (LPSTR)flatconvert[key%12] ) ;
	}
	else
	{
		wsprintf( string,"%d %s",1 + key/12, (LPSTR)convert[key%12] ) ;
	}
}

void CPersonality::CreateUndoMgr()
{
	// create undo mgr;
	if(m_pUndoMgr == NULL)
	{
		m_pUndoMgr = new CJazzUndoMan(m_pComponent->m_pIFramework);
		m_pUndoMgr->SetStreamAttributes(FT_DESIGN, GUID_CurrentVersion);
	}
}

void CPersonality::RefreshProperties()
{
	if( theApp.m_pIPageManager == NULL )
	{
		return;
	}

	theApp.m_pIPageManager->RefreshData();

	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( pIPropSheet->IsShowing() == S_OK )
		{
			IDMUSProdPropPageObject* pIPageObject;

			if( SUCCEEDED ( QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
			{
				pIPageObject->OnShowProperties();
				pIPageObject->Release();
			}
		}

		pIPropSheet->Release();
	}
}

bool CPersonality::GetLastEdit(CString & str)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	IDMUSProdStripMgr* pChordMapMgr = NULL;
	HRESULT hr = m_punkChordMapMgr->QueryInterface(IID_IDMUSProdStripMgr, (void**)&pChordMapMgr);
	if(SUCCEEDED(hr))
	{
		pChordMapMgr->GetParam(GUID_ChordMapStripUndoText, 0, 0 , (void*)&str);
		pChordMapMgr->Release();
	}
	else
	{
		str = "";
	}
	return str.GetLength() > 0;
}

void CPersonality::CleanupObjects()
// delete objects held by personality
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	// m_pPersonality will == NULL if an error occurs during loading and
	// this object is deleted.
	if( m_pPersonality )
	{
		ChordEntry *pchord = m_pPersonality->m_chordlist.GetHead();
		SignPost *psignpost = m_pPersonality->m_signpostlist.GetHead();
		for (;pchord;)
		{
			ChordEntry *next = pchord->GetNext();
			delete pchord;
			pchord = next;	
		}
		m_pPersonality->m_chordlist.RemoveAll();
		for (;psignpost;)
		{
			SignPost *next = psignpost->GetNext();
			delete psignpost;
			psignpost = next;	
		}
		m_pPersonality->m_signpostlist.RemoveAll();
  }

	ClearChordList();
	List_Free( m_lstCommandList );
}

/*
// enums for selection type (matches Chordmapmgr enums
#define CHORD_NO_SELECTION						0
#define CHORD_PALETTE_SELECTED					1
#define CHORD_MAP_SELECTED						2
#define CHORD_CONNECTION_SELECTED				3
*/

void CPersonality::SetSelectionType()
{
	IDMUSProdStripMgr *pStripMgrChordMap = NULL;

	m_selectiontype = CHORD_NO_SELECTION;
	m_nidToChord = -1;
	m_nidFromChord = -1;

	if( SUCCEEDED ( m_punkChordMapMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgrChordMap ) ) )
	{
		// TO DO: This should be replaced by explicit calls to determine selection type
		// Note: until then DO NOT change order of enums in CChordMapMgr
		pStripMgrChordMap->GetParam(GUID_SelectedObjectType, 0,0, &(m_selectiontype));
		switch(m_selectiontype)
		{
		
			case CHORD_MAP_SELECTED:
				// nothing to do, keydown already set in chords
				break;
			
			case CHORD_CONNECTION_SELECTED:
				{
					NextChord* pNext;
					if(SUCCEEDED(pStripMgrChordMap->GetParam(GUID_SelectedConnection, 0,0, &pNext)))
					{
						if( pNext )
						{
							m_nidToChord = pNext->m_nid;
							m_nidFromChord = pNext->m_parent->m_nid;
						}
						else
						{
							m_selectiontype = CHORD_NO_SELECTION;
						}
					}
				}
				break;
			
			case CHORD_PALETTE_SELECTED:
					if(SUCCEEDED(pStripMgrChordMap->GetParam(GUID_SelectedPaletteIndex, 0,0, &m_idxPaletteChord)))
					{
					}
				break;
		}

		pStripMgrChordMap->Release();
	}

}


HRESULT CPersonality::DMSaveStructure(IDMUSProdRIFFStream * pIRiffStream)
{
	ASSERT(pIRiffStream);
	if(!pIRiffStream)
		return E_INVALIDARG;
	PersonalityStructure	ps;
	ps.bVariableNotFixed = (m_fVariableNotFixed == TRUE);
	ps.wChordMapLength = m_nChordMapLength;
	ps.bUseTimeSignature = (m_fUseTimeSignature == TRUE);
	ps.lTimeSig = MAKE_TIMESIG(m_nBPM, m_nBeat);
	return ps.Write(pIRiffStream);
}

HRESULT CPersonality::DMLoadStructure(IDMUSProdRIFFStream * pIRiffStream)
{
	ASSERT(pIRiffStream);
	if(!pIRiffStream)
		return E_INVALIDARG;
	PersonalityStructure	ps;
	HRESULT hr = ps.Read(pIRiffStream);
	if(hr == S_OK)
	{
		m_fVariableNotFixed = ps.bVariableNotFixed ? TRUE : FALSE;
		m_fUseTimeSignature = ps.bUseTimeSignature ? TRUE : FALSE;
		m_nBPM = TIMESIG_BPM(ps.lTimeSig);
		m_nBeat = TIMESIG_BEAT(ps.lTimeSig);
		m_nChordMapLength = ps.wChordMapLength;
	}
	return hr;
}

HRESULT CPersonality::InitializeStyleReference()
{
	ASSERT(m_pComponent->m_pIFramework);

	IDMUSProdDocType* pIDocType = NULL;
	IDMUSProdProject* pIProject = NULL;
	IDMUSProdNode* pIFileNode = NULL;
	IDMUSProdNode* pDocRoot = NULL;
	HRESULT hr = S_OK;

	IDMUSProdNode* pOldDocRoot = GetStyleDocRoot();
	if(pOldDocRoot)
	{
		// already set, nothing to do
		pOldDocRoot->Release();
		return hr;
	}

	if(m_pIStyleRefNode == NULL)
	{
		// Allocate a new reference node
		IDMUSProdComponent* pIStyleComponent;
		hr = m_pComponent->m_pIFramework->FindComponent( CLSID_StyleComponent,  &pIStyleComponent );
		if(FAILED(hr))
		{
			return E_FAIL;
		}
		hr = pIStyleComponent->AllocReferenceNode(GUID_StyleRefNode, &m_pIStyleRefNode);
		RELEASE(pIStyleComponent);
		if(FAILED(hr))
		{
			return  E_OUTOFMEMORY;
		}
	}

	if( FAILED ( m_pComponent->m_pIFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType ) ) )
	{
		return E_FAIL;
	}

	hr = m_pComponent->m_pIFramework->FindProject(this, &pIProject);

	if(FAILED(hr))
	{
		pIDocType->Release();
		return E_FAIL;
	}

	hr = pIProject->GetFirstFileByDocType( pIDocType, &pIFileNode );

	if(hr == S_OK)
	{
		GUID guidFile;
		if(SUCCEEDED(m_pComponent->m_pIFramework->GetNodeFileGUID(pIFileNode, &guidFile)))
		{
			IDMUSProdNode* pIStyleDocRootNode = NULL;
			if(SUCCEEDED(m_pComponent->m_pIFramework->FindDocRootNodeByFileGUID(guidFile, &pIStyleDocRootNode)))
			{
				SetStyleReference(pIStyleDocRootNode);
				pIStyleDocRootNode->Release();
				Modified() = FALSE;	// setstylereference dirties us, need to be clean
			}
		}
		pIFileNode->Release();
	}

	if(pIProject)
		pIProject->Release();
	if(pIDocType)
		pIDocType->Release();
	return hr;
}

#pragma warning(default:4189)

void TransposeDMPolyChord(DMPolyChord& chord, int howmuch, bool bFlatsNotSharps)
{
	for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
	{
		// first do chord
		DMChord* pChord = chord.SubChord(i);
		int newKey = pChord->ChordRoot() + howmuch;
		while(newKey < 0)
		{
			newKey += 12;
		}
		while(newKey > 23)
		{
			newKey -= 12;
		}
		pChord->UseFlat() = bFlatsNotSharps;
		pChord->ChordRoot() = static_cast<BYTE>(newKey);
		// now transpose scale by same amount
		pChord->ScalePattern() = Rotate24(pChord->ScalePattern(), howmuch);
	}
}

void CPersonality::TransposeAllChords(DWORD dwNewKey)
{
	// compute transposition
	int nTranspose = ((dwNewKey &0x7f) - (m_dwKey & 0x7f));

	if(nTranspose < 0)
	{
		nTranspose += 24;
	}

	bool bFlatsNotSharps = (dwNewKey & UseFlats) != 0;

	ChordPalette pal;

	// Transpose chords in ChordPalette
	int i1;
	int j;
	TRACE("\r\nTranspose = %d\r\n", nTranspose);
	for(i1 = 0; i1 < 24; i1++)
	{
		int i2 = (i1 + nTranspose) % 24;
		strcpy(pal.m_chords[i2].Name(), m_pPersonality->m_chordpalette.m_chords[i1].Name());
		for(j = 0; j < DMPolyChord::MAX_POLY; j++)
		{
			pal.m_chords[i2].SubChord(j)->ChordPattern()
				= m_pPersonality->m_chordpalette.m_chords[i1].SubChord(j)->ChordPattern();
			pal.m_chords[i1].SubChord(j)->ScalePattern() = m_pPersonality->m_scalepattern;
			pal.m_chords[i1].SubChord(j)->ScaleRoot() = (BYTE)GetAbsKey();
			pal.m_chords[i1].SubChord(j)->UseFlat() = bFlatsNotSharps;
		}
		TRACE("%d: Name: %s, Pattern: %x\r\n", i2, pal.m_chords[i2].Name(),
				pal.m_chords[i2].SubChord(0)->ChordPattern());
	}
	memcpy(&(m_pPersonality->m_chordpalette), &pal, sizeof(ChordPalette));

	

	// Go through the Chords in the ChordList and transpose each
	ChordEntry*	pChord		= NULL;
	for(pChord = m_pPersonality->m_chordlist.GetHead(); pChord != NULL;	pChord = pChord->GetNext())
	{
		TransposeDMPolyChord(pChord->m_chordsel, nTranspose, bFlatsNotSharps);
	}

	// go through chords in signpost list and transpose each
	SignPost* pSignPost = NULL;
	for(pSignPost = m_pPersonality->m_signpostlist.GetHead(); pSignPost != NULL; pSignPost = pSignPost->GetNext())
	{
		TransposeDMPolyChord(pSignPost->m_chord, nTranspose, bFlatsNotSharps);
		// check cadence chords
		if(pSignPost->m_dwflags & SPOST_CADENCE1)
		{
			TransposeDMPolyChord(pSignPost->m_cadence[0], nTranspose, bFlatsNotSharps);
		}
		if(pSignPost->m_dwflags & SPOST_CADENCE2)
		{
			TransposeDMPolyChord(pSignPost->m_cadence[1], nTranspose, bFlatsNotSharps);
		}
	}
}

void CPersonality::RefreshChordMap()
{
	if( m_punkChordMapMgr == NULL ) return;

	IDMUSProdStripMgr *pStripMgrChordMap = NULL;
	if( FAILED(m_punkChordMapMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgrChordMap )) )
	{
		return;
	}

	IDMUSProdTimeline *pTimeline = NULL;
	VARIANT varTimeline;
	if( FAILED(pStripMgrChordMap->GetStripMgrProperty( SMP_ITIMELINECTL, &varTimeline )) || (V_UNKNOWN( &varTimeline ) == NULL) )
	{
		pStripMgrChordMap->Release();
		return;
	}
	if( FAILED( V_UNKNOWN( &varTimeline )->QueryInterface( IID_IDMUSProdTimeline, (void**)&pTimeline ) ) || (pTimeline == NULL) )
	{
		V_UNKNOWN( &varTimeline )->Release();
		pStripMgrChordMap->Release();
		return;
	}
	V_UNKNOWN( &varTimeline )->Release();

	pStripMgrChordMap->Release();

	pTimeline->Refresh();
	pTimeline->Release();
}

void CPersonality::RefreshSignPosts(bool bRefresh)
{
	if(m_pSignPostDlg)
		m_pSignPostDlg->Refresh(bRefresh);
}




void CPersonality::UpdateChordPalette( bool fUpdateChords )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);	
	DWORD lScalePattern = Rotate24( m_pPersonality->m_scalepattern, GetAbsKey() );
	
	ChordBuilder* pcb = 0;
	try
	{
		if( m_paletteChordType == ChordBuilder::Seventh
		||  m_paletteChordType == ChordBuilder::Triad )
		{
			pcb = new ChordBuilder( lScalePattern, m_paletteChordType, GetAbsKey() );
		}
		else
		{
			switch(m_paletteChordType)
			{
			case ChordBuilder::AllMaj:
				pcb = new ChordBuilder(0x000091, "maj");
				break;
			case ChordBuilder::AllMin:
				pcb = new ChordBuilder(0x000089, "min");
				break;
			case ChordBuilder::AllMaj7:
				pcb = new ChordBuilder(0x000891, "maj7");
				break;
			case ChordBuilder::AllMin7:
				pcb = new ChordBuilder(0x000489, "min7");
				break;
			case ChordBuilder::AllDom7:
				pcb = new ChordBuilder(0x000491, "7");
				break;
			default:
				ASSERT(false);
				break;
			}
		}
	}
	catch(...)
	{
		ASSERT(false);
		return;
	}

	for( int i = 0;  i < 24;  i++ )
	{
		if( fUpdateChords )
		{
			strcpy( m_pPersonality->m_chordpalette.m_chords[i].Name(), pcb->ChordName(i) );
		}
		for( int j = 0;  j < DMPolyChord::MAX_POLY;  j++ )
		{
			if( fUpdateChords )
			{
				m_pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ChordPattern() = pcb->ChordPattern(i);
			}
			m_pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScalePattern() = m_pPersonality->m_scalepattern;
			m_pPersonality->m_chordpalette.m_chords[i].SubChord(j)->ScaleRoot() = GetAbsKey();
		}
	}

	if( pcb )
	{
		delete pcb;
	}
}

int& CPersonality::PaletteChordType()
{
	return m_paletteChordType;
}

void CPersonality::ClearSPLinks()
{
	for(SignPost* pSignPost = m_pPersonality->m_signpostlist.GetHead(); pSignPost; pSignPost = pSignPost->GetNext())
	{
		pSignPost->m_dwflags &= ~SPOST_MATCHED;
	}
}

void CPersonality::ClearCMLinks()
{
	for(ChordEntry* pChord = m_pPersonality->m_chordlist.GetHead();pChord;pChord = pChord->GetNext())
	{
		pChord->m_dwflags &= ~CE_MATCHED;
	}
}

bool CPersonality::Match(ChordEntry* ce, SignPost* sp)
{
	if(!ce->IsSignPost())
		return false;
	bool match = true;
	for(int i = 0; match && (i < DMPolyChord::MAX_POLY); i++)
	{
		if(ce->m_chordsel.SubChord(i)->ChordPattern() != sp->m_chord.SubChord(i)->ChordPattern())
		{
			match = false;
		}
		else if(ce->m_chordsel.SubChord(ce->m_chordsel.RootIndex())->ChordRoot()
					!= sp->m_chord.SubChord(sp->m_chord.RootIndex())->ChordRoot())
		{
			match = false;
		}
	}
	return match;
}

bool CPersonality::Match(SignPost* sp1, SignPost* sp2)
{
	bool match = true;
	for(int i = 0; match && (i < DMPolyChord::MAX_POLY); i++)
	{
		if(sp1->m_chord.SubChord(i)->ChordPattern() != sp2->m_chord.SubChord(i)->ChordPattern())
		{
			match = false;
		}
		else if(sp1->m_chord.SubChord(sp1->m_chord.RootIndex())->ChordRoot()
					!= sp2->m_chord.SubChord(sp2->m_chord.RootIndex())->ChordRoot())
		{
			match = false;
		}
	}
	return match;
}

void CPersonality::LinkCMToSP()
{
	for(ChordEntry* pChord = m_pPersonality->m_chordlist.GetHead();pChord;pChord = pChord->GetNext())
	{
		Link2SP(pChord, false);	
	}
}

bool CPersonality::Link2SP(ChordEntry* ce, bool bRefreshChordMap)
{
	UNREFERENCED_PARAMETER(bRefreshChordMap);
	if(!ce->IsSignPost())
		return false;
	bool match = false;
	for(SignPost* sp = m_pPersonality->m_signpostlist.GetHead(); sp; sp = sp->GetNext())
	{
		if(Match(ce,sp))
		{
			match = true;
			Link(ce,sp);
		}
	}
	return match;
}

bool CPersonality::Link2SP(SignPost* sp2)
{
	bool match = false;
	for(SignPost* sp = m_pPersonality->m_signpostlist.GetHead(); sp; sp = sp->GetNext())
	{
		if(sp == sp2)
			continue;	// don't try to match a signpost to itself
		if(Match(sp2,sp))
		{
			if(sp2->m_dwflags & SPOST_MATCHED)
			{
				match = true;
				Link(sp2,sp);
			}
		}
	}
	return match;
}

bool CPersonality::Link2CM(SignPost* sp)
{
	bool match = false;
	for(ChordEntry* pChord = m_pPersonality->m_chordlist.GetHead();pChord;pChord = pChord->GetNext())
	{
		if(Match(pChord, sp))
		{
			match = true;
			Link(pChord, sp);
		}
	}
	return match;
}


void CPersonality::LinkAll(bool bIsStructuralChange)
{
	UNREFERENCED_PARAMETER(bIsStructuralChange);

	ClearSPLinks();
	ClearCMLinks();
	LinkCMToSP();
	RefreshChordMap();
//	RefreshSignPosts(bIsStructuralChange);
	if(m_pSignPostDlg)
		m_pSignPostDlg->UpdateOrphanStatus(true);
}


void CPersonality::ComputeChordMap()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Change the Time Signature of the Timeline
	if( m_punkChordMapMgr == NULL ) return;

	IDMUSProdStripMgr *pStripMgrChordMap = NULL;
	if( FAILED(m_punkChordMapMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgrChordMap )) )
	{
		return;
	}
	
	pStripMgrChordMap->OnUpdate(GUID_TimeSignature, 0, 0);

	pStripMgrChordMap->Release();
}

ChordEntryList* CPersonality::GetChordList()
{
	return &(m_pPersonality->m_chordlist);
}

bool CPersonality::CheckForData(long lBeginMeasure)
{
	bool rc = false;
	for(ChordEntry* pChord = m_pPersonality->m_chordlist.GetHead();
		pChord; pChord = pChord->GetNext())
	{
		if(pChord->m_chordsel.Measure() >= lBeginMeasure)
		{
			rc = true;
			return rc;
		}
	}
	return rc;
}

void CPersonality::SetStyleDocRootNode()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pComponent->m_pIFramework);

	IDMUSProdDocType* pIDocType;
	IDMUSProdProject* pIProject;
	IDMUSProdNode* pIFileNode, *pIStyleDocRootNode;

	if( FAILED ( m_pComponent->m_pIFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType ) ) )
	{
		return;
	}

	HRESULT hr = m_pComponent->m_pIFramework->FindProject(this, &pIProject);

	if(hr==S_OK)
	{
		hr = pIProject->GetFirstFileByDocType( pIDocType, &pIFileNode );		
		if(hr == S_OK)
		{
			GUID guidFile;
			if(SUCCEEDED(m_pComponent->m_pIFramework->GetNodeFileGUID(pIFileNode, &guidFile)))
			{
				if(SUCCEEDED(m_pComponent->m_pIFramework->FindDocRootNodeByFileGUID(guidFile, &pIStyleDocRootNode)))
				{
					SetStyleReference(pIStyleDocRootNode);
					pIStyleDocRootNode->Release();
				}
			}
			pIFileNode->Release();
		}

		pIProject->Release();
	}
	pIDocType->Release();

}


double& CPersonality::ZoomFactor()
{
	return m_dblZoom;
}

LONG& CPersonality::FunctionbarWidth()
{
	return m_lFunctionbarWidth;
}

void CPersonality::OnNameChange()
{
	// Our File name changed - Get new Segment file name
	CString strName;
	BSTR bstrName;
	if( SUCCEEDED ( m_pComponent->m_pIFramework->GetNodeFileName( this, &bstrName ) ) )
	{
		// Node is in the Project tree
		strName = bstrName;
		::SysFreeString( bstrName );
		int nFindPos = strName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strName = strName.Right( strName.GetLength() - nFindPos - 1 );
		}
		strName += _T(": ");
	}
	else
	{
		// Node has not yet been placed in the Project Tree
		// because the Segment is still in the process of being loaded
		strName = m_strOrigFileName;
		strName += _T(": ");
	}

	// Plus "name"
	strName += m_pPersonality->m_username;

	// Update the Conductor's Toolbars
	bstrName = strName.AllocSysString();
	m_pComponent->m_pIConductor->SetTransportName( this, bstrName );

	// Update the Framework
	m_pComponent->m_pIFramework->RefreshNode( this );
/*
	// Update our property sheet
	ASSERT(m_pComponent);
	ASSERT(m_pComponent->m_pIFramework);
	IDMUSProdPropSheet *pIPropSheet;
	if(SUCCEEDED(m_pComponent->m_pIFramework->QueryInterface(IID_IDMUSProdPropSheet, (void **) &pIPropSheet)))
	{
		pIPropSheet->RefreshTitleByObject(this);
		pIPropSheet->Release();
	}
	*/
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality::GetObjectDescriptor

HRESULT CPersonality::GetObjectDescriptor( void* pObjectDesc )
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
	
	memcpy( &pDMObjectDesc->guidObject, &m_guidPersonality, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicChordMap, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = m_dwVersionMS;
	pDMObjectDesc->vVersion.dwVersionLS = m_dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, m_pPersonality->m_username, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality::GetGUID

void CPersonality::GetGUID( GUID* pguidPersonality )
{
	if( pguidPersonality )
	{
		*pguidPersonality = m_guidPersonality;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality::SetGUID

void CPersonality::SetGUID( GUID guidPersonality )
{
	m_guidPersonality = guidPersonality;

	// Sync Personality with DirectMusic
	m_fModified = TRUE;
	SyncPersonalityWithEngine(CPersonality::syncPersonality);

	// Notify connected nodes that Personality GUID has changed
	m_pComponent->m_pIFramework->NotifyNodes( this, DOCROOT_GuidChange, NULL );
}
