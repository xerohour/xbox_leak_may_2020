// Pattern.cpp : implementation file
//

#include "stdafx.h"

#include "StyleDesignerDLL.h"
#include "Style.h"
#include "Pattern.h"
#include "VarChoices.h"
#include "PatternCtl.h"
#include <mmreg.h>
#include <PChannelname.h>
#include "DialogLinkExisting.h"
#include <SegmentGUIDS.h>
#include <ioDMStyle.h>
#include "RiffStructs.h"
#include "StyleCtl.h"
#include "TabPatternPattern.h"
#include "TabMotifMotif.h"
#include "TabMotifLoop.h"
#include <TabBoundaryFlags.h>
#include "SharedPattern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// FROM SuperJAM! Drum Map
static char achMappingsToDMUS[128] = {
         0,			//0 },
         35,		//1  },
         36,		//2  },
         38,		//3  },
         40,		//4  },
         41,		//5  },
         45,		//6  },
         48,		//7  },
         42,		//8  },
         44,		//9  },
         46,		//10 },
         39,		//11 },
         37,		//12 },
         51,		//13 },
         49,		//14 },
         54,		//15 },
         56,		//16 },
         61,		//17 },
         60,		//18 },
         64,		//19 },
         63,		//20 },
         66,		//21 },
         65,		//22 },
         69,		//23 },
         70,		//24 },
         71,		//25 },
         72,		//26 },
         73,		//27 },
         75,		//28 },
         47,		//29 },
         50,		//30 },
         53,		//31 },
         57,		//32 },
         52,		//33 },
         74,		//34 },
		 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
		 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
		 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94,
		 95, 96, 97, 98,
         87,		//99 },
         86,		//100 },
         85,		//101 },
         84,		//102 },
         83,		//103 },
         82,		//104 },
         81,		//105 },
         80,		//106 },
         79,		//107 },
         78,		//108 },
         77,		//109 },
         76,		//110 },
         68,		//111 },
         67,		//112 },
         62,		//113 },
         59,		//114 },
         58,		//115 },
         55,		//116 },
         43,		//117 },
         34,		//118 },
         33,		//119 },
         32,		//120 },
         31,		//121 },
         30,		//122 },
         29,		//123 },
         28,		//124 },
         27,		//125 },
         26,		//126 },
		 25,		//127 }
};

// TO SuperJAM! Drum Map
static char achMappingsToIMA[128] = {
         0,
		 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
		 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
		 21, 22, 23, 24,
		 127,		//25
		 126,		//26
		 125,		//27
		 124,		//28
		 123,		//29
		 122,		//30
		 121,		//31
		 120,		//32
		 119,		//33
		 118,		//34
		 1,			//35
		 2,			//36
		 12,		//37
		 3,			//38
		 11,		//39
		 4,			//40
		 5,			//41
		 8,			//42
		 117,		//43
		 9,			//44
		 6,			//45
		 10,		//46
		 29,		//47
		 7,			//48
		 14,		//49
		 30,		//50	
		 13,		//51
		 33,		//52
		 31,		//53
		 15,		//54
		 116,		//55
		 16,		//56
		 32,		//57
		 115,		//58
		 114,		//59
		 18,		//60
		 17,		//61
		 113,		//62
		 20,		//63
		 19,		//64
		 22,		//65
		 21,		//66
		 112,		//67
		 111,		//68
		 23,		//69
		 24,		//70
		 25,		//71
		 26,		//72
		 27,		//73
		 34,		//74
		 28,		//75
		 110,		//76
		 109,		//77
		 108,		//78
		 107,		//79
		 106,		//80
		 105,		//81
		 104,		//82
		 103,		//83
		 102,		//84
		 101,		//85
		 100,		//86
		 99,		//87
		 88,		//88
		 89,		//89
		 90,		//90
		 91,		//91
		 92,		//92
		 93,		//93
		 94,		//64
		 95,		//95
		 96,		//96
		 97,		//97
		 98,		//98
		 99, 100, 
		 101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
		 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
		 121, 122, 123, 124, 125, 126, 127
};


// {27D9B240-733E-11d1-89AE-00A0C9054129}
static const GUID GUID_MotifPropPageManager = 
{ 0x27d9b240, 0x733e, 0x11d1, { 0x89, 0xae, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };

// {37A0B2A0-733E-11d1-89AE-00A0C9054129}
static const GUID GUID_PatternPropPageManager = 
{ 0x37a0b2a0, 0x733e, 0x11d1, { 0x89, 0xae, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };

short CMotifPropPageManager::sm_nActiveTab = 0;

static BOOL sg_fPatternFromMidiMgr = FALSE;

// This code is shared with Segment.cpp in the Segment Designer project
HRESULT DoAutoTransition( const ConductorTransitionOptions &TransitionOptions, IDirectMusicPerformance *pPerformance,
						IDirectMusicComposer8 *pIDMComposer, IDirectMusicSegment *pSegmentTo,
						IDirectMusicChordMap *pIDMChordMap, IDirectMusicSegmentState **ppTransitionSegmentState,
						IDirectMusicSegmentState **ppSegmentStateTo )
{
	IDirectMusicSegment *pITransitionSegment = NULL;

	// Set the transition pattern (embellishment)
	WORD wCommand = 0;
	if( !(TransitionOptions.dwFlags & TRANS_REGULAR_PATTERN) )
	{
		// if we have an embellishment, set it.
		wCommand = TransitionOptions.wPatternType;
	}

	// Set the transition flags
	DWORD dwFlags = TransitionOptions.dwBoundaryFlags;

	// Set the Boundary settings (bar/beat/grid/immediate/end of segment)
	{
		// Can't use COMPOSEF_QUEUE, so use COMPOSEF_SEGMENTEND
		dwFlags |= DMUS_COMPOSEF_SEGMENTEND;
	}

	HRESULT hr = pIDMComposer->AutoTransition(pPerformance, 
											   pSegmentTo, 
											   wCommand, 
											   dwFlags, 
											   pIDMChordMap, 
											   &pITransitionSegment, 
											   ppSegmentStateTo, 
											   ppTransitionSegmentState);

	if( pITransitionSegment )
	{
		pITransitionSegment->Release();
		pITransitionSegment = NULL;
	}

	if( ppSegmentStateTo
	&&	(NULL == *ppSegmentStateTo) )
	{
		hr = E_FAIL;
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
// CMotifPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMotifPropPageManager::CMotifPropPageManager()
{
	m_pTabMotif = NULL;
	m_pTabLoop = NULL;
	m_pTabBoundary = NULL;
	m_GUIDManager = GUID_MotifPropPageManager;
}

CMotifPropPageManager::~CMotifPropPageManager()
{
	if( m_pTabMotif )
	{
		delete m_pTabMotif;
	}

	if( m_pTabLoop )
	{
		delete m_pTabLoop;
	}

	if( m_pTabBoundary )
	{
		delete m_pTabBoundary;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMotifPropPageManager::RemoveCurrentObject

void CMotifPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CMotifPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CMotifPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strMotif;

	strMotif.LoadString( IDS_MOTIF_TEXT );

	CString strTitle = strMotif;

	PPGMotif ppgMotif;
	PPGMotif* pPPGMotif = &ppgMotif;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pPPGMotif))) )
	{
		if( ppgMotif.pMotif )
		{
			// Get Style Name
			CString strStyleName;
			BSTR bstrStyleName;
			IDMUSProdNode* pIDocRootNode;

			if( SUCCEEDED ( ppgMotif.pMotif->GetDocRootNode( &pIDocRootNode ) ) )
			{
				if( SUCCEEDED ( pIDocRootNode->GetNodeName( &bstrStyleName ) ) )
				{
					strStyleName = bstrStyleName;
					::SysFreeString( bstrStyleName );
					strStyleName += _T(" - ");
				}

				RELEASE( pIDocRootNode );
			}

			// Format title
			strTitle = strMotif + _T(": ") + strStyleName + ppgMotif.pMotif->m_strName;
		}
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMotifPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CMotifPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	// Add Motif tab
	m_pTabMotif = new CTabMotifMotif( this );
	if( m_pTabMotif )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabMotif->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Loop tab
	m_pTabLoop = new CTabMotifLoop( this );
	if( m_pTabLoop )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabLoop->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Boundary tab
	m_pTabBoundary = new CTabBoundaryFlags( this );
	if( m_pTabBoundary )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabBoundary->m_psp );
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
// CMotifPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CMotifPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CMotifPropPageManager::sm_nActiveTab );

	CDllBasePropPageManager::OnRemoveFromPropertySheet();

	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMotifPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CMotifPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pTabBoundary )
	{
		m_pTabBoundary->SetObject( pINewPropPageObject );
	}
	
	if( m_pIPropPageObject != pINewPropPageObject )
	{
		RemoveCurrentObject();

		m_pIPropPageObject = pINewPropPageObject;
//		m_pIPropPageObject->AddRef();		intentionally missing
	}

	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMotifPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CMotifPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure changes to current Motif are processed in OnKillFocus
	// messages before setting the new Motif
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabMotif->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Refresh Property tabs to display the new Motif
	m_pTabMotif->RefreshTab( m_pIPropPageObject );
	m_pTabLoop->RefreshTab( m_pIPropPageObject );
	m_pTabBoundary->RefreshTab();

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// CPatternPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPatternPropPageManager::CPatternPropPageManager()
{
	m_pTabPattern = NULL;
	m_GUIDManager = GUID_PatternPropPageManager;
}

CPatternPropPageManager::~CPatternPropPageManager()
{
	if( m_pTabPattern )
	{
		delete m_pTabPattern;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPatternPropPageManager::RemoveCurrentObject

void CPatternPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CPatternPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strPattern;

	strPattern.LoadString( IDS_PATTERN_TEXT );

	CString strTitle = strPattern;

	PPGPattern ppgPattern;
	PPGPattern* pPPGPattern = &ppgPattern;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pPPGPattern))) )
	{
		// Get Style Name
		CString strStyleName;
		BSTR bstrStyleName;
		IDMUSProdNode* pIDocRootNode;

		if( SUCCEEDED ( ppgPattern.pPattern->GetDocRootNode( &pIDocRootNode ) ) )
		{
			if( SUCCEEDED ( pIDocRootNode->GetNodeName( &bstrStyleName ) ) )
			{
				strStyleName = bstrStyleName;
				::SysFreeString( bstrStyleName );
				strStyleName += _T(" - ");
			}

			RELEASE( pIDocRootNode );
		}

		// Format title
		strTitle = strPattern + _T(": ") + strStyleName + ppgPattern.pPattern->m_strName;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CPatternPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
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

	// Add Pattern tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	m_pTabPattern = new CTabPatternPattern( this );
	if( m_pTabPattern )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabPattern->m_psp );
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
// CPatternPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CPatternPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDllBasePropPageManager::OnRemoveFromPropertySheet();

	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPatternPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CPatternPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure changes to current Pattern are processed in OnKillFocus
	// messages before setting the new Pattern
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabPattern->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Refresh Property tabs to display the new Pattern
	m_pTabPattern->RefreshTab( m_pIPropPageObject );

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CIMA25EventItem destructor

CIMA25EventItem::~CIMA25EventItem()
{
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicEventItem destructor

CDirectMusicEventItem::~CDirectMusicEventItem()
{
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleCurve contructor

CDirectMusicStyleCurve::CDirectMusicStyleCurve()
{
	m_mtGridStart = 0;
	m_nTimeOffset = 0;
	m_dwVariation = 0;
	m_bType = ET_CURVE;

	m_mtDuration = 0;
	m_mtResetDuration = 0;
	m_nStartValue = 0;	
	m_nEndValue = 0;	
	m_nResetValue = 0;	
    m_bEventType = DMUS_CURVET_CCCURVE;	
	m_bCurveShape = DMUS_CURVES_SINE;	
	m_bCCData = 0;		
	m_bFlags = 0;		
	m_wParamType = 0;
	m_wMergeIndex =0;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleMarker contructor

CDirectMusicStyleMarker::CDirectMusicStyleMarker()
{
	m_pNext = 0;
	m_mtGridStart = 0;
	m_nTimeOffset = 0;
	m_dwVariation = 0;
	m_bType = ET_MARKER;
	m_wMarkerFlags = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart constructor/destructor

CDirectMusicPart::CDirectMusicPart( CDirectMusicStyle* pStyle )
{
	ASSERT( pStyle != NULL );
    
	m_pStyle = pStyle;
//	m_pStyle->AddRef();		intentionally missing

	m_dwUseCount = 0;
	m_dwHardLinkCount = 0;
	
	// Set defaults
	m_TimeSignature = m_pStyle->m_TimeSignature;
	m_wNbrMeasures = 1;
	m_bPlayModeFlags = DMUS_PLAYMODE_PURPLEIZED;			
	m_bInvertLower = 0;			
	m_bInvertUpper = 127;
	m_dwFlags = 0;

	for( int i = 0 ;  i < NBR_VARIATIONS ;  i++ )
	{
		m_dwVariationChoices[i] = (DM_VF_MODE_DMUSIC | DM_VF_FLAG_BITS);	
	}

	m_pVarChoicesNode = NULL;
	m_pVarChoicesPartRef = NULL;
	m_pPartDesignData = NULL;
	m_dwPartDesignDataSize = 0;
	
	CoCreateGuid( &m_guidPartID ); 
}

CDirectMusicPart::~CDirectMusicPart()
{
	if( m_pPartDesignData )
	{
		GlobalFree( m_pPartDesignData );
	}

	if( m_pVarChoicesNode )
	{
		HWND hWndEditor;
		m_pVarChoicesNode->GetEditorWindow( &hWndEditor );
		if( hWndEditor )
		{
			theApp.m_pStyleComponent->m_pIFramework->CloseEditor( m_pVarChoicesNode );
		}
		RELEASE( m_pVarChoicesNode );
	}

	m_pVarChoicesPartRef = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::CopyTo

void CDirectMusicPart::CopyTo( CDirectMusicPart* pDMPart )
{
	ASSERT( pDMPart );

	pDMPart->m_pStyle = m_pStyle;
//	m_pStyle->AddRef();		intentionally missing

	ASSERT( pDMPart->m_dwPartDesignDataSize == m_dwPartDesignDataSize );
	if( pDMPart->m_dwPartDesignDataSize == m_dwPartDesignDataSize )
	{
		*pDMPart->m_pPartDesignData = *m_pPartDesignData;
	}

	pDMPart->m_pVarChoicesNode = NULL;
	pDMPart->m_pVarChoicesPartRef = NULL;
	pDMPart->m_strName = m_strName;
	memcpy( &pDMPart->m_guidPartID, &m_guidPartID, sizeof(GUID) );
	pDMPart->m_TimeSignature = m_TimeSignature;
	pDMPart->m_wNbrMeasures = m_wNbrMeasures;
	memcpy( pDMPart->m_dwVariationChoices, m_dwVariationChoices, sizeof(DWORD) * 32 );
	pDMPart->m_bPlayModeFlags = m_bPlayModeFlags;
	pDMPart->m_bInvertLower = m_bInvertLower;
	pDMPart->m_bInvertUpper = m_bInvertUpper;
	pDMPart->m_dwFlags = m_dwFlags;

	// Copy the list of events
	CDirectMusicEventItem* pItem = m_lstEvents.GetHead();
	while( pItem )
	{
		if( pItem->m_bType == ET_NOTE )
		{
			CDirectMusicStyleNote* pNote = new CDirectMusicStyleNote;
			*pNote = *(CDirectMusicStyleNote *)pItem;
			pDMPart->m_lstEvents.AddHead( pNote );
		}
		else if( pItem->m_bType == ET_CURVE )
		{
			CDirectMusicStyleCurve* pCurve = new CDirectMusicStyleCurve;
			*pCurve = *(CDirectMusicStyleCurve *)pItem;
			pDMPart->m_lstEvents.AddHead( pCurve );
		}
		else if( pItem->m_bType == ET_MARKER )
		{
			CDirectMusicStyleMarker* pMarker = new CDirectMusicStyleMarker;
			*pMarker = *(CDirectMusicStyleMarker *)pItem;
			pDMPart->m_lstEvents.AddHead( pMarker );
		}

		pItem = pItem->GetNext();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_LoadPart

HRESULT CDirectMusicPart::DM_LoadPart( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	MMCKINFO ckList;
	DWORD dwByteCount;
	DWORD dwSize;
    DWORD dwStructSize;
    DWORD dwExtra;

	CDirectMusicEventList lstNotes;	
	CDirectMusicEventList lstCurves;
	CDirectMusicEventList lstMarkers;

	ASSERT( m_pStyle != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Clear Part UI fields
	memset( m_pPartDesignData, 0, m_dwPartDesignDataSize );

	// Clear Part name
	m_strName.Empty();

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_PART_CHUNK:
			{
				BOOL fIsDX7 = ck.cksize == 154;	// Size of DX7 struct

				DMUS_IO_STYLEPART dmusStylePartIO;
				ZeroMemory( &dmusStylePartIO, sizeof( DMUS_IO_STYLEPART ) );

				dwSize = min( ck.cksize, sizeof( DMUS_IO_STYLEPART ) );
				hr = pIStream->Read( &dmusStylePartIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( m_pStyle->FindPartByGUID( dmusStylePartIO.guidPartID ) )
				{
					hr = S_FALSE;	// Style already contains a Part with this GUID
					goto ON_ERROR;	// so we must bypass this Part
				}

				m_TimeSignature.m_bBeatsPerMeasure = dmusStylePartIO.timeSig.bBeatsPerMeasure;
				m_TimeSignature.m_bBeat = dmusStylePartIO.timeSig.bBeat;
				m_TimeSignature.m_wGridsPerBeat = dmusStylePartIO.timeSig.wGridsPerBeat;

				m_wNbrMeasures = dmusStylePartIO.wNbrMeasures;
				m_bInvertUpper = dmusStylePartIO.bInvertUpper;
				m_bInvertLower = dmusStylePartIO.bInvertLower;
				m_bPlayModeFlags = dmusStylePartIO.bPlayModeFlags;
				m_dwFlags = dmusStylePartIO.dwFlags;

				memcpy( &m_guidPartID, &dmusStylePartIO.guidPartID, sizeof(GUID) );
				memcpy( &m_dwVariationChoices, &dmusStylePartIO.dwVariationChoices, sizeof(m_dwVariationChoices) );

				for( int i = 0 ;  i < NBR_VARIATIONS ;  i++ )
				{
					if( m_dwVariationChoices[i] == 0xFFFFFFFF )
					{
						m_dwVariationChoices[i] = (DM_VF_MODE_DMUSIC | DM_VF_FLAG_BITS);	
					}
					if( fIsDX7
					&& (m_dwVariationChoices[i] & DM_VF_MODE_DMUSIC) )
					{
						m_dwVariationChoices[i] |= DM_VF_DEST_TOOTHER;	
					}
				}
				break;
			}

			case DMUS_FOURCC_NOTE_CHUNK:
			{
				CDirectMusicStyleNote* pNote;
				DMUS_IO_STYLENOTE dmusStyleNoteIO;

				dwSize = ck.cksize;

				// Read size of the note structure
				hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != sizeof( dwStructSize ) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				dwSize -= dwByteCount;

				if( dwStructSize > sizeof( DMUS_IO_STYLENOTE ) )
				{
					dwExtra = dwStructSize - sizeof( DMUS_IO_STYLENOTE );
					dwStructSize = sizeof( DMUS_IO_STYLENOTE );
				}
				else
				{
					dwExtra = 0;
				}

				// now read in the notes
				while( dwSize > 0 )
				{
					// Prepare DMUS_IO_STYLENOTE structure
					ZeroMemory( &dmusStyleNoteIO, sizeof( DMUS_IO_STYLENOTE ) );

					hr = pIStream->Read( &dmusStyleNoteIO, dwStructSize, &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != dwStructSize )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}
					dwSize -= dwStructSize;

					if( dwExtra > 0 )
					{
						StreamSeek( pIStream, dwExtra, STREAM_SEEK_CUR );
						dwSize -= dwExtra;
					}

					// Create Direct Music Note
					pNote = new CDirectMusicStyleNote;
					if( pNote )
					{
						pNote->m_mtGridStart = dmusStyleNoteIO.mtGridStart;
						pNote->m_dwVariation = dmusStyleNoteIO.dwVariation;
						pNote->m_mtDuration = dmusStyleNoteIO.mtDuration;
						pNote->m_nTimeOffset = dmusStyleNoteIO.nTimeOffset;

						pNote->m_wMusicValue = dmusStyleNoteIO.wMusicValue;
						pNote->m_bVelocity = dmusStyleNoteIO.bVelocity;
						pNote->m_bTimeRange = dmusStyleNoteIO.bTimeRange;
						pNote->m_bDurRange = dmusStyleNoteIO.bDurRange;
						pNote->m_bVelRange = dmusStyleNoteIO.bVelRange;
						pNote->m_bInversionId = dmusStyleNoteIO.bInversionID; 
						pNote->m_bPlayModeFlags = dmusStyleNoteIO.bPlayModeFlags;
						pNote->m_bNoteFlags = dmusStyleNoteIO.bNoteFlags;

						// Place note in Part's event list
						lstNotes.AddHead( pNote );
					}
				}
				break;
			}

			case DMUS_FOURCC_CURVE_CHUNK:
			{
				CDirectMusicStyleCurve* pCurve;
				DMUS_IO_STYLECURVE dmusStyleCurveIO;

				dwSize = ck.cksize;

				// Read size of the note structure
				hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != sizeof( dwStructSize ) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				dwSize -= dwByteCount;

				if( dwStructSize > sizeof( DMUS_IO_STYLECURVE ) )
				{
					dwExtra = dwStructSize - sizeof( DMUS_IO_STYLECURVE );
					dwStructSize = sizeof( DMUS_IO_STYLECURVE );
				}
				else
				{
					dwExtra = 0;
				}

				// now read in the curves
				while( dwSize > 0 )
				{
					ZeroMemory( &dmusStyleCurveIO, sizeof( DMUS_IO_STYLECURVE ) );
					hr = pIStream->Read( &dmusStyleCurveIO, dwStructSize, &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != dwStructSize )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}
					dwSize -= dwStructSize;

					if( dwExtra > 0 )
					{
						StreamSeek( pIStream, dwExtra, STREAM_SEEK_CUR );
						dwSize -= dwExtra;
					}

					// Create Direct Music Curve
					pCurve = new CDirectMusicStyleCurve;
					if( pCurve )
					{
						pCurve->m_mtGridStart = dmusStyleCurveIO.mtGridStart;
						pCurve->m_dwVariation = dmusStyleCurveIO.dwVariation;
						pCurve->m_mtDuration = dmusStyleCurveIO.mtDuration;
						pCurve->m_mtResetDuration = dmusStyleCurveIO.mtResetDuration;
						pCurve->m_nTimeOffset = dmusStyleCurveIO.nTimeOffset;

						pCurve->m_nStartValue = dmusStyleCurveIO.nStartValue;
						pCurve->m_nEndValue = dmusStyleCurveIO.nEndValue;
						pCurve->m_nResetValue = dmusStyleCurveIO.nResetValue;
						pCurve->m_bEventType = dmusStyleCurveIO.bEventType;
						pCurve->m_bCurveShape = dmusStyleCurveIO.bCurveShape;
						pCurve->m_bCCData = dmusStyleCurveIO.bCCData;
						pCurve->m_bFlags = dmusStyleCurveIO.bFlags;
						pCurve->m_wParamType = dmusStyleCurveIO.wParamType;
						pCurve->m_wMergeIndex = dmusStyleCurveIO.wMergeIndex;

						// Place curve in Part's event list
						lstCurves.AddHead( pCurve );
					}
				}
				break;
			}

			case DMUS_FOURCC_MARKER_CHUNK:
			{
				dwSize = ck.cksize;

				// Read size of the marker structure
				hr = pIStream->Read( &dwStructSize, sizeof( dwStructSize ), &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != sizeof( dwStructSize ) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				dwSize -= dwByteCount;

				// Check if there is any extra data to skip over
				if( dwStructSize > sizeof( DMUS_IO_STYLEMARKER ) )
				{
					dwExtra = dwStructSize - sizeof( DMUS_IO_STYLEMARKER );
					dwStructSize = sizeof( DMUS_IO_STYLEMARKER );
				}
				else
				{
					dwExtra = 0;
				}

				// now read in the markers
				DMUS_IO_STYLEMARKER iDMStyleMarker;
				while( dwSize > 0 )
				{
					ZeroMemory( &iDMStyleMarker, sizeof( DMUS_IO_STYLEMARKER ) );
					hr = pIStream->Read( &iDMStyleMarker, dwStructSize, &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != dwStructSize )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}
					dwSize -= dwStructSize;

					// Skip over any extra data, if necessary
					if( dwExtra > 0 )
					{
						StreamSeek( pIStream, dwExtra, STREAM_SEEK_CUR );
						dwSize -= dwExtra;
					}

					// Create Direct Music marker
					CDirectMusicStyleMarker* pMarker;
					pMarker = new CDirectMusicStyleMarker;
					if( pMarker )
					{
						pMarker->m_mtGridStart = iDMStyleMarker.mtGridStart;
						pMarker->m_dwVariation = iDMStyleMarker.dwVariation;

						pMarker->m_wMarkerFlags = iDMStyleMarker.wMarkerFlags;

						// Place marker in Part's event list
						lstMarkers.AddHead( pMarker );
					}
				}
				break;
			}

			case DMUS_FOURCC_PART_DESIGN:
				dwSize = min( ck.cksize, m_dwPartDesignDataSize );
				hr = pIStream->Read( m_pPartDesignData, dwSize, &dwByteCount );
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
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case RIFFINFO_INAM:
								case DMUS_FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
		}

        pIRiffStream->Ascend( &ck, 0 );
	}

	CDirectMusicEventItem* pEvent;

	// Transfer lstNotes to m_lstEvents
	ASSERT( m_lstEvents.GetHead() == NULL );		// Part event list should be empty
	pEvent = lstNotes.RemoveHead();
	while( pEvent )
	{
		m_lstEvents.AddHead( pEvent );
		pEvent = lstNotes.RemoveHead();
	}

	// Transfer lstCurves to m_lstEvents
	pEvent = lstCurves.RemoveHead();
	while( pEvent )
	{
		m_lstEvents.AddHead( pEvent );
		pEvent = lstCurves.RemoveHead();
	}

	// Transfer lstMarkers to m_lstEvents
	pEvent = lstMarkers.RemoveHead();
	while( pEvent )
	{
		m_lstEvents.AddHead( pEvent );
		pEvent = lstMarkers.RemoveHead();
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SavePartInfoList

HRESULT CDirectMusicPart::DM_SavePartInfoList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;

	if( m_strName.IsEmpty() )
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

	// Write Part name
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
// CDirectMusicPart::DM_SavePart

HRESULT CDirectMusicPart::DM_SavePart( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_STYLEPART dmusStylePartIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Part list header
	ckMain.fccType = DMUS_FOURCC_PART_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Part chunk header
	ck.ckid = DMUS_FOURCC_PART_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_STYLEPART structure
	memset( &dmusStylePartIO, 0, sizeof(DMUS_IO_STYLEPART) );

	dmusStylePartIO.timeSig.bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
	dmusStylePartIO.timeSig.bBeat = m_TimeSignature.m_bBeat;
	dmusStylePartIO.timeSig.wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;

	dmusStylePartIO.wNbrMeasures = m_wNbrMeasures;
	dmusStylePartIO.bInvertUpper = m_bInvertUpper;
	dmusStylePartIO.bInvertLower = m_bInvertLower;
	dmusStylePartIO.bPlayModeFlags = m_bPlayModeFlags;
	dmusStylePartIO.dwFlags = m_dwFlags;

	memcpy( &dmusStylePartIO.guidPartID, &m_guidPartID, sizeof(GUID) );
	memcpy( &dmusStylePartIO.dwVariationChoices, &m_dwVariationChoices, sizeof(m_dwVariationChoices) );

	// Write Part chunk data
	hr = pIStream->Write( &dmusStylePartIO, sizeof(DMUS_IO_STYLEPART), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_STYLEPART) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

// Save Part info
	hr = DM_SavePartInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	CDirectMusicEventItem* pEvent;

	// Save Part note list (when applicable)
	pEvent = m_lstEvents.GetHead();
	while( pEvent )
	{
		if( pEvent->m_bType == ET_NOTE )
		{
			hr = DM_SaveNoteList( pIRiffStream );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}
			break;
		}

		pEvent = pEvent->GetNext();
	}

	// Save Part curve list (when applicable)
	pEvent = m_lstEvents.GetHead();
	while( pEvent )
	{
		if( pEvent->m_bType == ET_CURVE )
		{
			hr = DM_SaveCurveList( pIRiffStream );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}
			break;
		}

		pEvent = pEvent->GetNext();
	}

	// Save Marker list (when applicable)
	pEvent = m_lstEvents.GetHead();
	while( pEvent )
	{
		if( pEvent->m_bType == ET_MARKER )
		{
			hr = DM_SaveMarkerList( pIRiffStream );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}
			break;
		}

		pEvent = pEvent->GetNext();
	}

	// Save Part Design data
	hr = DM_SavePartDesignData( pIRiffStream );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ckMain, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SaveNoteList

HRESULT CDirectMusicPart::DM_SaveNoteList( IDMUSProdRIFFStream* pIRiffStream )
{
	CDirectMusicEventItem* pEvent;
	CDirectMusicStyleNote* pNote;
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwStructSize;
	DMUS_IO_STYLENOTE dmusStyleNoteIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Note chunk header
	ck.ckid = DMUS_FOURCC_NOTE_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save size of DMUS_IO_STYLENOTE structure
	dwStructSize = sizeof(DMUS_IO_STYLENOTE);
	hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Now save all of the notes
	pEvent = m_lstEvents.GetHead();
	while( pEvent )
	{
		if( pEvent->m_bType == ET_NOTE )
		{
			pNote = (CDirectMusicStyleNote *)pEvent;
			
			// Prepare DMUS_IO_STYLENOTE structure
			ZeroMemory( &dmusStyleNoteIO, sizeof( DMUS_IO_STYLENOTE ) );

			dmusStyleNoteIO.mtGridStart = pNote->m_mtGridStart;
			dmusStyleNoteIO.dwVariation = pNote->m_dwVariation;
			dmusStyleNoteIO.nTimeOffset = pNote->m_nTimeOffset;	

			dmusStyleNoteIO.mtDuration = pNote->m_mtDuration;
			dmusStyleNoteIO.wMusicValue = pNote->m_wMusicValue;	
			dmusStyleNoteIO.bVelocity = pNote->m_bVelocity;	
			dmusStyleNoteIO.bTimeRange = pNote->m_bTimeRange;	
			dmusStyleNoteIO.bDurRange = pNote->m_bDurRange;	
			dmusStyleNoteIO.bVelRange = pNote->m_bVelRange;	
			dmusStyleNoteIO.bInversionID = pNote->m_bInversionId;	
			dmusStyleNoteIO.bPlayModeFlags = pNote->m_bPlayModeFlags;
			dmusStyleNoteIO.bNoteFlags = pNote->m_bNoteFlags;

			// Write DMUS_IO_STYLENOTE structure
			hr = pIStream->Write( &dmusStyleNoteIO, sizeof(DMUS_IO_STYLENOTE), &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DMUS_IO_STYLENOTE) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		pEvent = pEvent->GetNext();
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SaveCurveList

HRESULT CDirectMusicPart::DM_SaveCurveList( IDMUSProdRIFFStream* pIRiffStream )
{
	CDirectMusicEventItem* pEvent;
	CDirectMusicStyleCurve* pCurve;
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwStructSize;
	DMUS_IO_STYLECURVE dmusStyleCurveIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	ck.ckid = DMUS_FOURCC_CURVE_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save size of DMUS_IO_STYLECURVE structure
	dwStructSize = sizeof(DMUS_IO_STYLECURVE);
	hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Now save all of the curves
	pEvent = m_lstEvents.GetHead();
	while( pEvent )
	{
		if( pEvent->m_bType == ET_CURVE )
		{
			pCurve = (CDirectMusicStyleCurve *)pEvent;
			
			// Prepare DMUS_IO_STYLECURVE structure
			dmusStyleCurveIO.mtGridStart = pCurve->m_mtGridStart;
			dmusStyleCurveIO.dwVariation = pCurve->m_dwVariation;
			dmusStyleCurveIO.nTimeOffset = pCurve->m_nTimeOffset;	

			dmusStyleCurveIO.mtDuration = pCurve->m_mtDuration;
			dmusStyleCurveIO.mtResetDuration = pCurve->m_mtResetDuration;
			dmusStyleCurveIO.nStartValue = pCurve->m_nStartValue;	
			dmusStyleCurveIO.nEndValue = pCurve->m_nEndValue;	
			dmusStyleCurveIO.nResetValue = pCurve->m_nResetValue;	
			dmusStyleCurveIO.bEventType = pCurve->m_bEventType;	
			dmusStyleCurveIO.bCurveShape = pCurve->m_bCurveShape;	
			dmusStyleCurveIO.bCCData = pCurve->m_bCCData;	
			dmusStyleCurveIO.bFlags = pCurve->m_bFlags;	
			dmusStyleCurveIO.wParamType = pCurve->m_wParamType;	
			dmusStyleCurveIO.wMergeIndex = pCurve->m_wMergeIndex;

			// Write DMUS_IO_STYLECURVE structure
			hr = pIStream->Write( &dmusStyleCurveIO, sizeof(DMUS_IO_STYLECURVE), &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DMUS_IO_STYLECURVE) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		pEvent = pEvent->GetNext();
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SaveMarkerList

HRESULT CDirectMusicPart::DM_SaveMarkerList( IDMUSProdRIFFStream* pIRiffStream ) const
{
    MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_MARKER_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Save size of DMUS_IO_STYLEMARKER structure
	DWORD dwBytesWritten, dwStructSize = sizeof(DMUS_IO_STYLEMARKER);
	HRESULT hr = pIStream->Write( &dwStructSize, sizeof(DWORD), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DWORD) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Now save all the markers
	CDirectMusicEventItem* pEvent;
	pEvent = m_lstEvents.GetHead();
	while( pEvent )
	{
		if( pEvent->m_bType == ET_MARKER )
		{
			// Prepare DMUS_IO_STYLEMARKER structure
			DMUS_IO_STYLEMARKER oDMStyleMarker;
			ZeroMemory( &oDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER) );

			oDMStyleMarker.mtGridStart = pEvent->m_mtGridStart;
			oDMStyleMarker.dwVariation = pEvent->m_dwVariation;

			oDMStyleMarker.wMarkerFlags = ((CDirectMusicStyleMarker *)pEvent)->m_wMarkerFlags;

			// Write DMUS_IO_STYLEMARKER structure
			hr = pIStream->Write( &oDMStyleMarker, sizeof(DMUS_IO_STYLEMARKER), &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DMUS_IO_STYLEMARKER) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		pEvent = pEvent->GetNext();
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::DM_SavePartDesignData

HRESULT CDirectMusicPart::DM_SavePartDesignData( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;

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

	// Only save this chunk in "Design" files
	if( StreamInfo.ftFileType != FT_DESIGN )
	{
		hr = S_OK;
		goto ON_ERROR;	// Not an error, just need to free pIStream
	}

	// Write Part Design header
	ck.ckid = DMUS_FOURCC_PART_DESIGN;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

    hr = pIStream->Write( m_pPartDesignData, m_dwPartDesignDataSize, &dwBytesWritten );
    if( FAILED( hr )
    ||  dwBytesWritten != m_dwPartDesignDataSize )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::SetTimeSignature

void CDirectMusicPart::SetTimeSignature( DirectMusicTimeSig timeSig, BOOL fInUndo, BOOL fStyleTimeSigChanging )
{
	ASSERT( m_pStyle != NULL );

	if( fStyleTimeSigChanging )
	{
		if( m_pStyle->m_TimeSignature.m_bBeatsPerMeasure != m_TimeSignature.m_bBeatsPerMeasure
		||  m_pStyle->m_TimeSignature.m_bBeat != m_TimeSignature.m_bBeat )
		{
			// Don't change Part's timesig unless equal to Style's timesig
			m_pPartDesignData->m_bStyleTimeSigChange = 0;
			return;
		}

		if( fInUndo )
		{
			if( m_pPartDesignData->m_bStyleTimeSigChange == 0 )
			{
				// Don't change Part's timesig if it wasn't changed when Style's timesig changed
				return;
			}
		}

		m_pPartDesignData->m_bStyleTimeSigChange = 0;
	}

	if( timeSig.m_bBeatsPerMeasure != m_TimeSignature.m_bBeatsPerMeasure
	||  timeSig.m_bBeat != m_TimeSignature.m_bBeat
	||  timeSig.m_wGridsPerBeat != m_TimeSignature.m_wGridsPerBeat )
	{
		if( fStyleTimeSigChanging )
		{
			m_pPartDesignData->m_bStyleTimeSigChange = 1;
		}

		// Store values based on original time signature
		DWORD dwOrigClocksPerBeat = DM_PPQNx4 / m_TimeSignature.m_bBeat;
		DWORD dwOrigClocksPerGrid = dwOrigClocksPerBeat / (DWORD)m_TimeSignature.m_wGridsPerBeat;
		DWORD dwOrigClocksPerMeasure = dwOrigClocksPerBeat * (DWORD)m_TimeSignature.m_bBeatsPerMeasure;
		DWORD dwOrigLength = dwOrigClocksPerMeasure * m_wNbrMeasures;

		// Change Part time signature
		DirectMusicTimeSig timeSigOrig = m_TimeSignature;
		m_TimeSignature = timeSig;

		// Store values based on new time signature
		DWORD dwNewClocksPerBeat = DM_PPQNx4 / m_TimeSignature.m_bBeat;
		DWORD dwNewClocksPerGrid = dwNewClocksPerBeat / (DWORD)m_TimeSignature.m_wGridsPerBeat;
		DWORD dwNewClocksPerMeasure = dwNewClocksPerBeat * (DWORD)m_TimeSignature.m_bBeatsPerMeasure;
		
		// Adjust Part length
		m_wNbrMeasures = (WORD)(dwOrigLength / dwNewClocksPerMeasure);
		if( dwOrigLength % dwNewClocksPerMeasure )
		{
			m_wNbrMeasures++;
		}
		DWORD dwNewLength = dwNewClocksPerMeasure * m_wNbrMeasures;
		
		// Adjust event list
		if( dwNewClocksPerBeat != dwOrigClocksPerBeat
		||  m_TimeSignature.m_wGridsPerBeat != timeSigOrig.m_wGridsPerBeat )  
		{
			DWORD dwMaxGrids = (DWORD)m_wNbrMeasures *
							   (DWORD)m_TimeSignature.m_bBeatsPerMeasure *
							   (DWORD)m_TimeSignature.m_wGridsPerBeat;

			DWORD dwBeat;
			DWORD dwExtraGrids;	    // Number of grids into the beat
			DWORD dwExtraClocks;	
			DWORD dwOffsetClocks;	

			CDirectMusicEventItem* pEvent = m_lstEvents.GetHead();
			while( pEvent )
			{
				// Store number of grids into the beat
				dwExtraGrids = (DWORD)pEvent->m_mtGridStart % timeSigOrig.m_wGridsPerBeat;

				// Sync to the beat
				dwBeat = (DWORD)pEvent->m_mtGridStart / (DWORD)timeSigOrig.m_wGridsPerBeat;
				dwBeat *= dwOrigClocksPerBeat;
				dwExtraClocks = dwBeat % dwNewClocksPerBeat;
				dwBeat /= dwNewClocksPerBeat;
				pEvent->m_mtGridStart = dwBeat * (DWORD)m_TimeSignature.m_wGridsPerBeat;

				// Take dwExtraGrids into consideration
				dwExtraGrids *= dwOrigClocksPerGrid;
				dwExtraGrids += dwExtraClocks;
				dwOffsetClocks = dwExtraGrids % dwNewClocksPerGrid;
				dwExtraGrids /= dwNewClocksPerGrid;
				pEvent->m_mtGridStart += dwExtraGrids;

				// Adjust pEvent->m_nTimeOffset
				pEvent->m_nTimeOffset += (short)dwOffsetClocks;

				// Make pEvent->m_mtGridStart is valid
				ASSERT( pEvent->m_mtGridStart >= 0 ); 
				ASSERT( pEvent->m_mtGridStart <= (signed)dwMaxGrids ); 

				// Adjust Curve reset duration
				if( pEvent->m_bType == ET_CURVE )
				{
					CDirectMusicStyleCurve* pDMCurve = (CDirectMusicStyleCurve *)pEvent;

					if( (pDMCurve->m_bFlags & DMUS_CURVE_RESET)
					&&  (pDMCurve->m_mtResetDuration == (long)dwOrigLength) )
					{
						pDMCurve->m_mtResetDuration = (long)dwNewLength;
					}
				}

				pEvent = pEvent->GetNext();
			}
		}

		m_pStyle->SetModified( TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::SetNbrMeasures

void CDirectMusicPart::SetNbrMeasures( WORD wNbrMeasures )
{
	// Helper function - no need to save undo, refresh, sync, set modified

	if( wNbrMeasures != m_wNbrMeasures )
	{
		long lClocksPerBeat = DM_PPQNx4 / (long)m_TimeSignature.m_bBeat;
		long lClocksPerMeasure = lClocksPerBeat * (long)m_TimeSignature.m_bBeatsPerMeasure;
		long lOldPartClockLength = lClocksPerMeasure * (long)m_wNbrMeasures;

		m_wNbrMeasures = wNbrMeasures;
		
		long lNewPartClockLength = lClocksPerMeasure * (long)m_wNbrMeasures; 
		long lNewNbrBeats = (long)m_wNbrMeasures * (long)m_TimeSignature.m_bBeatsPerMeasure; 
		long lNewLastGrid = lNewNbrBeats * (long)m_TimeSignature.m_wGridsPerBeat; 

		CDirectMusicEventItem* pEvent;
		CDirectMusicEventItem* pEventNext = m_lstEvents.GetHead();
		while( pEventNext )
		{
			pEvent = pEventNext;
			pEventNext = pEvent->GetNext();

			// Delete events with start times past end of Part
			if( pEvent->m_mtGridStart >= lNewLastGrid )
			{
				 m_lstEvents.Remove( pEvent );
				 pEvent->m_pNext = NULL;	// To prevent entire list from being deleted
				 delete pEvent;
			}
			else
			{
				// Adjust Curve reset durations
				if( pEvent->m_bType == ET_CURVE )
				{
					CDirectMusicStyleCurve* pDMCurve = (CDirectMusicStyleCurve *)pEvent;

					if( pDMCurve->m_bFlags & DMUS_CURVE_RESET )
					{
						if( (pDMCurve->m_mtResetDuration == 0) 
						||  (pDMCurve->m_mtResetDuration == lOldPartClockLength) )
						{
							pDMCurve->m_mtResetDuration = lNewPartClockLength;
						}
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::UpdateFromVarChoices

void CDirectMusicPart::UpdateFromVarChoices( IDMUSProdFramework *pFramework, IPersistStream *pIPersistStream )
{
	// Validate the parameters
	ASSERT( pIPersistStream );
	ASSERT( pFramework );
	if( pFramework == NULL
	||	pIPersistStream == NULL )
	{
		return;
	}

	// Try and allocate the stream
	IStream *pStream = NULL;
	if( SUCCEEDED( pFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStream ) ) )
	{
		IDMUSProdRIFFStream* pIRiffStream;
		if( SUCCEEDED( AllocRIFFStream( pStream, &pIRiffStream ) ) )
		{
			// Save the editor into the stream
			if( SUCCEEDED( pIPersistStream->Save( pStream, TRUE ) ) )
			{
				// Seek back to the start of the stream
				StreamSeek( pStream, 0, STREAM_SEEK_SET );

				// Prepare the chunk headers
				MMCKINFO ckMain, ckSubChunk;
				ckMain.fccType = DM_FOURCC_VARCHOICES_FORM;
				ckSubChunk.ckid = DM_FOURCC_VARCHOICES_CHUNK;

				// Prepare ioVarChoices structure
				ioVarChoices iVarChoices;
				memset( &iVarChoices, 0, sizeof(ioVarChoices) );
				memcpy( iVarChoices.m_dwVariationChoices, m_dwVariationChoices, sizeof(DWORD) * NBR_VARIATIONS );

				// Read in the data
				DWORD dwBytesRead;
				if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0
				&&	pIRiffStream->Descend( &ckSubChunk, NULL, MMIO_FINDCHUNK ) == 0
				&&	SUCCEEDED( pStream->Read( &iVarChoices, sizeof(ioVarChoices), &dwBytesRead) )
				&&	sizeof(ioVarChoices) == dwBytesRead )
				{
					// Copy the data
					memcpy( m_dwVariationChoices, iVarChoices.m_dwVariationChoices, sizeof(DWORD) * NBR_VARIATIONS );

					// Update the design data
					if( m_pPartDesignData )
					{
						BOOL fRowDisabled;
						BOOL fDisabledFlag;

						for( short nRow = 0 ;  nRow < NBR_VARIATIONS ; nRow++ )
						{
							fRowDisabled = IsVarChoicesRowDisabled( nRow );
							fDisabledFlag = m_pPartDesignData->m_dwVariationsDisabled & (1 << nRow) ? TRUE : FALSE;

							if( fRowDisabled != fDisabledFlag )
							{
								if( fRowDisabled )
								{
									m_pPartDesignData->m_dwVariationsDisabled |= (1 << nRow);
									m_pPartDesignData->m_dwDisabledChoices[nRow] = m_dwVariationChoices[nRow];
								}
								else
								{
									m_pPartDesignData->m_dwVariationsDisabled &= ~(1 << nRow);
									m_pPartDesignData->m_dwDisabledChoices[nRow] = 0x00000000;
								}
							}
						}
					}
				}
			}
			RELEASE( pIRiffStream );
		}
		RELEASE( pStream );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef constructor/destructor

CDirectMusicPartRef::CDirectMusicPartRef( CDirectMusicPattern* pPattern )
{
	ASSERT( pPattern != NULL );
    
	m_pPattern = pPattern;
//	m_pPattern->AddRef();		intentionally missing

	m_pDMPart = NULL;
	m_dwPChannel = LOGICAL_PART_MELODY;
	m_bVariationLockID = 0;		// no locking
	m_bSubChordLevel = SUBCHORD_BASS;
	m_bPriority = 100;			
	m_bRandomVariation = 1;
	memset( &m_guidOldPartID, 0, sizeof(GUID) );
	m_fSetGUIDOldPartID = TRUE;

	m_fHardLink = FALSE;
}

CDirectMusicPartRef::~CDirectMusicPartRef()
{
	FreePianoRollStateList();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::FreePianoRollStateList

void CDirectMusicPartRef::FreePianoRollStateList( void )
{
	PianoRollUIState* pPianoRollUIState;

	while( !m_lstPianoRollUIStates.IsEmpty() )
	{
		pPianoRollUIState = static_cast<PianoRollUIState*>( m_lstPianoRollUIStates.RemoveHead() );

		if( pPianoRollUIState->pPianoRollData )
		{
			GlobalFree( pPianoRollUIState->pPianoRollData );
		}
		delete pPianoRollUIState;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::SetName

void CDirectMusicPartRef::SetName( LPCTSTR pszName )
{
	m_strName.Empty();

	if( pszName )
	{
		m_strName = pszName;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::SetPart

void CDirectMusicPartRef::SetPart( CDirectMusicPart* pPart )
{
	if( m_pDMPart == pPart )
	{
		return;
	}

	if( m_pDMPart )
	{
		ASSERT( m_pDMPart->m_pStyle != NULL );
		ASSERT( m_pDMPart->m_dwUseCount > 0 );

		m_pDMPart->m_dwUseCount--;

		if( m_fHardLink )
		{
			ASSERT( m_pDMPart->m_dwHardLinkCount > 0 );
			m_pDMPart->m_dwHardLinkCount--;
		}

		if( m_pDMPart->m_dwUseCount == 0 )
		{
			m_pDMPart->m_pStyle->DeletePart( m_pDMPart );
		}

		m_pDMPart = NULL;
	}

	if( pPart )
	{
		m_pDMPart = pPart;
		m_pDMPart->m_dwUseCount++;

		if( m_fHardLink )
		{
			m_pDMPart->m_dwHardLinkCount++;
		}

		if( ::IsEqualGUID( GUID_AllZeros, m_guidOldPartID ) )
		{
			memcpy ( &m_guidOldPartID, &m_pDMPart->m_guidPartID, sizeof( GUID ));
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::LinkParts

HRESULT CDirectMusicPartRef::LinkParts( CDirectMusicPartRef* pPartRef )
{
	ASSERT( pPartRef != NULL );

	pPartRef->SetPart( m_pDMPart );

	pPartRef->m_dwPChannel		 = m_dwPChannel;
	pPartRef->m_bVariationLockID = m_bVariationLockID;	
	pPartRef->m_bSubChordLevel	 = m_bSubChordLevel;
	pPartRef->m_bPriority		 = m_bPriority;			
	pPartRef->m_bRandomVariation = m_bRandomVariation;
	pPartRef->m_strName			 = m_strName;

	// Make sure hard link flag is set in both PartRefs
	if( !pPartRef->m_fHardLink )
	{
		m_pDMPart->m_dwHardLinkCount++;
		pPartRef->m_fHardLink = TRUE;
	}
	if( !m_fHardLink )
	{
		m_pDMPart->m_dwHardLinkCount++;
		m_fHardLink = TRUE;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::DM_SavePartRefPianoRollStates

HRESULT CDirectMusicPartRef::DM_SavePartRefPianoRollStates( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	POSITION pos;

	if( m_lstPianoRollUIStates.IsEmpty() )
	{
		return S_OK;
	}

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

	// Only save this chunk in "Design" files
	if( StreamInfo.ftFileType != FT_DESIGN )
	{
		hr = S_OK;
		goto ON_ERROR;	// Not an error, just need to free pIStream
	}

	PianoRollUIState* pPianoRollUIState;

	pos = m_lstPianoRollUIStates.GetHeadPosition();
	while( pos )
	{
        pPianoRollUIState = m_lstPianoRollUIStates.GetNext( pos );

		// Write Piano Roll LIST header
		ck.fccType = DMUS_FOURCC_PIANOROLL_LIST;
		if( pIRiffStream->CreateChunk(&ck, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = pIStream->Write( pPianoRollUIState->pPianoRollData,
							  pPianoRollUIState->dwPianoRollDataSize,
							  &dwBytesWritten );
		if( FAILED( hr )
		||  dwBytesWritten != pPianoRollUIState->dwPianoRollDataSize )
		{
			hr = E_FAIL;
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
// CDirectMusicPartRef::DM_SavePartRefDesignData

HRESULT CDirectMusicPartRef::DM_SavePartRefDesignData( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;

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

	// Only save this chunk in "Design" files
	if( StreamInfo.ftFileType != FT_DESIGN )
	{
		hr = S_OK;
		goto ON_ERROR;	// Not an error, just need to free pIStream
	}

	// Write PartOldGUID chunk header
	ck.ckid = DMUS_FOURCC_OLDGUID_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioDMPartOldGUID structure
	ioDMPartOldGUID oDMPartOldGUID;
	memcpy( &oDMPartOldGUID.m_guidOldPartID, &m_guidOldPartID, sizeof(GUID) );

	// Write PartOldGUID data
	hr = pIStream->Write( &oDMPartOldGUID, sizeof(ioDMPartOldGUID), &dwBytesWritten);
	if( FAILED( hr )
		||  dwBytesWritten != sizeof(ioDMPartOldGUID) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Reset flag so that m_guidOldPartID will be updated
	m_fSetGUIDOldPartID = TRUE;

	// Write PartRef Design header
	ck.ckid = DMUS_FOURCC_PARTREF_DESIGN;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0)
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Initialize ioDMStylePartRefDesign
	ioDMStylePartRefDesign oDMStylePartRefDesign;
	oDMStylePartRefDesign.m_fHardLink = m_fHardLink;

    hr = pIStream->Write( &oDMStylePartRefDesign, sizeof(ioDMStylePartRefDesign), &dwBytesWritten );
    if( FAILED( hr )
    ||  dwBytesWritten != sizeof(ioDMStylePartRefDesign) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::DM_SavePartRef

HRESULT CDirectMusicPartRef::DM_SavePartRef( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_PARTREF dmusPartRefIO;

	ASSERT( m_pDMPart != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write PartRef list header
	ckMain.fccType = DMUS_FOURCC_PARTREF_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write PartRef chunk header
	ck.ckid = DMUS_FOURCC_PARTREF_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_PARTREF structure
	memset( &dmusPartRefIO, 0, sizeof(DMUS_IO_PARTREF) );

	dmusPartRefIO.wLogicalPartID = WORD(m_dwPChannel);
	dmusPartRefIO.dwPChannel = m_dwPChannel;
	dmusPartRefIO.bVariationLockID = m_bVariationLockID; 
	dmusPartRefIO.bSubChordLevel = m_bSubChordLevel;
	dmusPartRefIO.bPriority = m_bPriority;
	dmusPartRefIO.bRandomVariation = m_bRandomVariation;
	dmusPartRefIO.dwPChannel = m_dwPChannel;

	memcpy( &dmusPartRefIO.guidPartID, &m_pDMPart->m_guidPartID, sizeof(GUID) );

	// Write PartRef chunk data
	hr = pIStream->Write( &dmusPartRefIO, sizeof(DMUS_IO_PARTREF), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_PARTREF) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save PartRef info
	hr = DM_SavePartRefInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save PartRef PianoRoll UI States
	hr = DM_SavePartRefPianoRollStates( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save PartRef Design Data
	hr = DM_SavePartRefDesignData( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ckMain, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::DM_LoadPartRef

HRESULT CDirectMusicPartRef::DM_LoadPartRef( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	CDirectMusicPart* pPart;
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	MMCKINFO ckList;
	DWORD dwByteCount;
	DWORD dwSize;
	BOOL fSetOldGuid = TRUE;

	ASSERT( m_pPattern != NULL );
	ASSERT( m_pPattern->m_pStyle != NULL );

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Free all items in the PianoRoll UI State list
	FreePianoRollStateList();

	// Clear PartRef name
	m_strName.Empty();

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_PARTREF_CHUNK:
			{
				DMUS_IO_PARTREF dmusPartRefIO;
				ZeroMemory( &dmusPartRefIO, sizeof( DMUS_IO_PARTREF ) );

				dwSize = min( ck.cksize, sizeof( DMUS_IO_PARTREF ) );
				hr = pIStream->Read( &dmusPartRefIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}


				if (dwSize < DX8_PARTREF_SIZE)
				{
					m_dwPChannel = dmusPartRefIO.wLogicalPartID;	
				}
				else
				{
					// BUGBUG: TODO: Remove this HACK (added to cleanup part# corruption)
					if( dmusPartRefIO.dwPChannel < DMUS_PCHANNEL_BROADCAST_GROUPS )
					{
						m_dwPChannel = dmusPartRefIO.wLogicalPartID;
					}
					else
					{
						m_dwPChannel = dmusPartRefIO.dwPChannel;
					}
				}
				m_bVariationLockID = dmusPartRefIO.bVariationLockID; 
				m_bSubChordLevel = dmusPartRefIO.bSubChordLevel;
				m_bPriority = dmusPartRefIO.bPriority;
				m_bRandomVariation = dmusPartRefIO.bRandomVariation;

				pPart = m_pPattern->m_pStyle->FindPartByGUID( dmusPartRefIO.guidPartID );
				if( pPart == NULL )
				{
					ASSERT( 0 );
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if (fSetOldGuid)
				{
					memcpy ( &m_guidOldPartID, &dmusPartRefIO.guidPartID, sizeof( GUID ));
				}
				SetPart( pPart );
				break;
			}

			case DMUS_FOURCC_OLDGUID_CHUNK:
			{
				ioDMPartOldGUID iDMPartOldGUID;

				dwSize = min( ck.cksize, sizeof( ioDMPartOldGUID ) );
				hr = pIStream->Read( &iDMPartOldGUID, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				fSetOldGuid = FALSE;
				memcpy( &m_guidOldPartID, &iDMPartOldGUID.m_guidOldPartID, sizeof(GUID) );
				break;
			}

			case DMUS_FOURCC_PARTREF_DESIGN:
			{
				ioDMStylePartRefDesign iDMStylePartRefDesign;

				dwSize = min( ck.cksize, sizeof( ioDMStylePartRefDesign ) );
				hr = pIStream->Read( &iDMStylePartRefDesign, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_fHardLink = iDMStylePartRefDesign.m_fHardLink;

				if( m_fHardLink )
				{
					if( m_pDMPart )
					{
						m_pDMPart->m_dwHardLinkCount++;
					}
					if( !fSetOldGuid )
					{
						CString strText;
						if( m_pPattern->m_fInUndo )
						{
							strText.LoadString( IDS_UNDO );
						}
						else if( m_pPattern->m_fInRedo )
						{
							strText.LoadString( IDS_REDO );
						}

						if( !strText.IsEmpty() )
						{
							// Remove all '&' from the string
							int nIndex = 0;
							while( nIndex >= 0 )
							{
								nIndex = strText.Find(_T("&"),nIndex);
								if( nIndex >= 0 )
								{
									strText.Delete(nIndex);
									nIndex = max( nIndex - 1, 0 );
								}
							}
						}

						TCHAR achText[256];
						BOOL fEnable = FALSE;

						ASSERT( m_pPattern->m_pUndoMgr != NULL );

						if( m_pPattern->m_pUndoMgr )
						{
							if( m_pPattern->m_fInRedo )
							{
								if( m_pPattern->m_pUndoMgr->GetRedo(achText, 256) )
								{
									if( *achText )
									{
										fEnable = TRUE;
										strText += " ";
										strText += achText;
										// Should now say 'Redo Insert Note'.
									}
								}
							}
							else //if( m_fInUndo || !m_fInUndo )
							{
								if( m_pPattern->m_pUndoMgr->GetUndo(achText, 256) )
								{
									if( *achText )
									{
										fEnable = TRUE;
										if( m_pPattern->m_fInUndo )
										{
											strText += " ";
											strText += achText;
											// Should now say 'Undo Insert Note'.
										}
										else
										{
											strText = achText;
											// Should now say 'Insert Note'
										}
									}
								}
							}
						}
						m_pPattern->m_pStyle->UpdateLinkedParts( this, fEnable ? ((LPCTSTR)strText) : NULL );
					}
				}
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case RIFFINFO_INAM:
								case DMUS_FOURCC_UNAM_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case DMUS_FOURCC_PIANOROLL_LIST:
					{
						PianoRollUIState* pPianoRollUIState;

						pPianoRollUIState = new PianoRollUIState;
						if( pPianoRollUIState == NULL )
						{
							hr = E_OUTOFMEMORY;
							goto ON_ERROR;
						}

						dwSize = ck.cksize;
						dwSize -= 4;
						pPianoRollUIState->pPianoRollData = (BYTE *)GlobalAlloc( GPTR, dwSize );
						if( pPianoRollUIState->pPianoRollData == NULL )
						{
							delete pPianoRollUIState;
							hr = E_OUTOFMEMORY;
							goto ON_ERROR;
						}
						pPianoRollUIState->dwPianoRollDataSize = dwSize;
						hr = pIStream->Read( pPianoRollUIState->pPianoRollData,
											 pPianoRollUIState->dwPianoRollDataSize,
											 &dwByteCount );
						if( FAILED( hr )
						||  dwByteCount != pPianoRollUIState->dwPianoRollDataSize )
						{
							hr = E_FAIL;
							GlobalFree( pPianoRollUIState->pPianoRollData );
							delete pPianoRollUIState;
							goto ON_ERROR;
						}
						m_lstPianoRollUIStates.AddTail( pPianoRollUIState );
						break;
					}
				}
				break;
		}

        pIRiffStream->Ascend( &ck, 0 );
	}

	if( !m_strName.IsEmpty()
	&&   m_pPattern->m_pStyle->m_pIProject )
	{
		// Query for an IDMUSProdPChannelName interface on the project
		IDMUSProdPChannelName* pIPChannelName;
		if( SUCCEEDED( m_pPattern->m_pStyle->m_pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&pIPChannelName ) ) )
		{
			// Found the interface, update the PChannel name if one doesn't already exist
			WCHAR*		wstrText = NULL;
			wstrText = new wchar_t[MAX_PATH];
			if( pIPChannelName->GetPChannelName( m_dwPChannel, wstrText ) != S_OK )
			{
				// Convert the name to wide characters
				MultiByteToWideChar( CP_ACP, 0, m_strName, -1, wstrText, MAX_PATH );

				// Update the name
				pIPChannelName->SetPChannelName( m_dwPChannel, wstrText );
			}
			delete wstrText;
			RELEASE(pIPChannelName);
		}
	}
ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern constructor/destructor

CDirectMusicPattern::CDirectMusicPattern( CDirectMusicStyle* pStyle, BOOL fMotif )
{
	ASSERT( pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );

	// Pattern needs Style Component
	theApp.m_pStyleComponent->AddRef();
    
	m_dwRef = 0;
	AddRef();

	m_fModified = FALSE;
	m_fSendChangeNotification = FALSE;
	m_fRecordPressed = FALSE;
	m_fDialogActive = FALSE;
	m_fInUndo = FALSE;
	m_fInRedo = FALSE;
	m_fDontUpdateDirectMusicDuringLoad = FALSE;
	m_fPatternIsPlaying = FALSE;
	m_fInGroupEdit = FALSE;
	m_fInOpenVarChoicesEditor = FALSE;

	m_pStyle = pStyle;
//	m_pStyle->AddRef();		intentionally missing

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pPatternCtrl = NULL;
	m_hWndEditor = NULL;
	m_pIDMSegment = NULL;
	m_pIDMSegmentState = NULL;
	m_rpIDMStoppedSegmentState = NULL;
	m_pIDMTransitionSegmentState = NULL;
	m_pIDMTransitionSegment = NULL;
	m_fTrackTimeCursor = TRUE;
	m_rtCurrentStartTime = 0;

	m_pPatternDesignData = NULL;
	m_dwPatternDesignDataSize = 0;

	m_dwPlaybackID = ++m_pStyle->m_dwNextPatternID;

    m_dwDefaultKeyPattern = 0xAB5AB5;	// Major	
    m_dwDefaultChordPattern = 0x891;	// M7
    m_bDefaultKeyRoot = 0;		
    m_bDefaultChordRoot = 12;		
	m_fKeyFlatsNotSharps = FALSE;
	m_fChordFlatsNotSharps = FALSE;

	m_dblZoom = 0.0;
	m_dblHorizontalScroll = 0.0;
	m_lVerticalScroll = 0;
	m_tlSnapTo = DMUSPROD_TIMELINE_SNAP_GRID;
	m_lFunctionbarWidth = 0;
	m_fIgnoreTimelineSync = false;

	// Set defaults
	m_TimeSignature = m_pStyle->m_TimeSignature;
	m_wNbrMeasures = 1;
	RecalcLength();

    m_pIBandNode = NULL;
	m_dwRepeats = 0;
    m_mtPlayStart = 0;
    m_mtLoopStart = 0;
    m_mtLoopEnd = m_dwLength;
    m_dwResolution = DMUS_SEGF_BEAT;

	m_bGrooveBottom = 1; 
	m_bGrooveTop = 100;
	m_bDestGrooveBottom = 1; 
	m_bDestGrooveTop = 100;

	m_nLastCustomId = MIN_EMB_CUSTOM_ID;

	m_dwFlags = fMotif ? DMUS_PATTERNF_PERSIST_CONTROL : 0;

	m_pRhythmMap = NULL;
	if( fMotif )
	{
		m_wEmbellishment = EMB_MOTIF;
	}
	else
	{
		m_wEmbellishment = EMB_NORMAL;
	}

	// Set default name
	TCHAR achName[MID_BUFFER];

	if( fMotif )
	{
		::LoadString( theApp.m_hInstance, IDS_MOTIF_TEXT, achName, MID_BUFFER );
	}
	else
	{
		::LoadString( theApp.m_hInstance, IDS_PATTERN_TEXT, achName, MID_BUFFER );
	}

	m_strName.Format( "%s%d", achName, 1 );
	m_pStyle->GetUniquePatternName( this );

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	m_pUndoMgr = new CJazzUndoMan(theApp.m_pStyleComponent->m_pIFramework);
	m_pUndoMgr->SetStreamAttributes(FT_DESIGN, GUID_SinglePattern);
}

CDirectMusicPattern::~CDirectMusicPattern()
{
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// Remove Pattern from clipboard
	theApp.FlushClipboard( this );

	// Remove Pattern/Motif from property sheet
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}

	// Cleanup references
	RELEASE( m_pIBandNode );
	RELEASE( m_pIDMSegment );
	RELEASE( m_pIDMSegmentState );
	RELEASE( m_pIDMTransitionSegmentState );
	RELEASE( m_pIDMTransitionSegment );

	// Don't release m_rpIDMStoppedSegmentState, it's just a reference

	// Cleanup CDirectMusicPattern
	if( m_pRhythmMap )
	{
		delete [] m_pRhythmMap;
	}

	// Clean up the undo mgr.
	if( m_pUndoMgr )
	{
		delete m_pUndoMgr;
		m_pUndoMgr = NULL;
	}

	// Free Pattern design data
	if( m_pPatternDesignData )
	{
		GlobalFree( m_pPatternDesignData );
		m_pPatternDesignData = NULL;
	}

	CDirectMusicPartRef* pPartRef;

	while( !m_lstPartRefs.IsEmpty() )
	{
		pPartRef = m_lstPartRefs.GetHead();
		DeletePartRef( pPartRef );
	}

	// Pattern no longer needs Style Component
	theApp.m_pStyleComponent->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::Initialize

HRESULT CDirectMusicPattern::Initialize( void )
{
	ASSERT( m_pStyle != NULL );

	// This method should only be called immedialtely
	// after construction to create an empty Pattern

	HRESULT hr = Initialize1();		// initialize state
	if( SUCCEEDED(hr) )
	{
		hr = Initialize2();		// create single empty part
	}
	
	return hr;	
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SyncPatternWithDirectMusic

HRESULT CDirectMusicPattern::SyncPatternWithDirectMusic()
{
	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );

	HRESULT hr;

	if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( this ) == S_OK )
	{
		hr = SyncPatternWithAuditionSegment( FALSE, m_pIDMSegment );
	}
	else
	{
		hr = m_pStyle->SyncStyleWithDirectMusic();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SyncPatternWithMidiStripMgr

HRESULT CDirectMusicPattern::SyncPatternWithMidiStripMgr()
{
	IPersistStream* pIPersistStream;
	IStream* pIMemStream;
	HRESULT hr = S_OK;

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( m_pPatternCtrl
	&&  m_pPatternCtrl->m_pPatternDlg
	&&  m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr )
	{
		IDMUSProdStripMgr* pIMIDIStripMgr = m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr;

		// Persist this Pattern's data to the MIDI strip manager
		hr = pIMIDIStripMgr->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( SUCCEEDED( hr ) )
		{
			hr = theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_SinglePattern, &pIMemStream );
			if( SUCCEEDED( hr ) )
			{
				hr = Save( pIMemStream, FALSE );
				if( SUCCEEDED( hr ) )
				{
					StreamSeek( pIMemStream, 0, STREAM_SEEK_SET ); //seek to beginning

					// If timeline exists, disable drawing operations until Load() is completed
					if( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
					{
						IOleWindow *pOleWindow;
						if( SUCCEEDED( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->QueryInterface( IID_IOleWindow, (void **)&pOleWindow ) ) )
						{
							HWND hwnd;
							pOleWindow->GetWindow( &hwnd );
							::LockWindowUpdate( hwnd );
							pOleWindow->Release();
						}
					}

					hr = pIPersistStream->Load( pIMemStream );
		
					// Reset timeline time signature and length
					if( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
					{
						DMUS_TIMESIGNATURE dmusTimeSig;

						dmusTimeSig.mtTime = 0;
						dmusTimeSig.bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
						dmusTimeSig.bBeat = m_TimeSignature.m_bBeat;			
						dmusTimeSig.wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;	
						m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->SetParam( GUID_TimeSignature, 1, 0, 0, &dmusTimeSig );

						VARIANT vtInit;
						vtInit.vt = VT_I4;

						V_I4(&vtInit) = m_dwLength;
						m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->SetTimelineProperty( TP_CLOCKLENGTH, vtInit );

						// Update the horizontal zoom, horizontal scroll, and vertical scroll
						m_pPatternCtrl->m_pPatternDlg->UpdateZoomAndScroll();

						// Now, redraw the dialog
						::LockWindowUpdate( NULL );
					}
				}

				RELEASE( pIMemStream );
			}

			RELEASE( pIPersistStream );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetModified

void CDirectMusicPattern::SetModified( BOOL fModified )
{
	m_fModified = fModified;

	if( m_wEmbellishment & EMB_MOTIF )
	{
		if( fModified )
		{
			// We must notify others when motif's have changed
			// In particualr, segment trigger track need to know
			m_fSendChangeNotification = TRUE;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IUnknown implementation

HRESULT CDirectMusicPattern::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
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

	if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
	{
		AddRef();
		*ppvObj = (IDMUSProdPropPageObject *)this;
		return S_OK;
	}

	if( ::IsEqualIID(riid, IID_IDMUSProdNotifyCPt) )
	{
		AddRef();
		*ppvObj = (IDMUSProdNotifyCPt *)this;
		return S_OK;
	}

	if( ::IsEqualIID(riid, IID_IDMUSProdTransport) )
	{
		AddRef();
		*ppvObj = (IDMUSProdTransport *)this;
		return S_OK;
	}

	if( ::IsEqualIID(riid, IID_IDMUSProdConductorTempo) )
	{
		AddRef();
		*ppvObj = (IDMUSProdConductorTempo *)this;
		return S_OK;
	}

	if( ::IsEqualIID(riid, IID_IPatternNodePrivate) )
	{
		AddRef();
		*ppvObj = (IPatternNodePrivate *)this;
		return S_OK;
	}

	if( ::IsEqualIID(riid, IID_IDMUSProdSecondaryTransport) )
	{
		AddRef();
		*ppvObj = (IDMUSProdSecondaryTransport *)this;
		return S_OK;
	}

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CDirectMusicPattern::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CDirectMusicPattern::Release()
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
// CDirectMusicPattern IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetNodeImageIndex

HRESULT CDirectMusicPattern::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	if( pnFirstImage == NULL )
	{
		return E_POINTER;
	}

	if( m_wEmbellishment & EMB_MOTIF )
	{
		return( theApp.m_pStyleComponent->GetMotifImageIndex(pnFirstImage) );
	}
	else
	{
		return( theApp.m_pStyleComponent->GetPatternImageIndex(pnFirstImage) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetFirstChild

HRESULT CDirectMusicPattern::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstChildNode = NULL;

	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have children
		return E_FAIL;
	}

	if( m_pIBandNode )
	{
		m_pIBandNode->AddRef();
		*ppIFirstChildNode = m_pIBandNode;
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetNextChild

HRESULT CDirectMusicPattern::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
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

	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have children
		return E_FAIL;
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetComponent

HRESULT CDirectMusicPattern::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	if( ppIComponent == NULL )
	{
		return E_POINTER;
	}

	return theApp.m_pStyleComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetDocRootNode

HRESULT CDirectMusicPattern::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CDirectMusicPattern IDMUSProdNode::SetDocRootNode

HRESULT CDirectMusicPattern::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );
	if( pIDocRootNode == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetParentNode

HRESULT CDirectMusicPattern::GetParentNode( IDMUSProdNode** ppIParentNode )
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
// CDirectMusicPattern IDMUSProdNode::SetParentNode

HRESULT CDirectMusicPattern::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetNodeId

HRESULT CDirectMusicPattern::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	if( m_wEmbellishment & EMB_MOTIF )
	{
		*pguid = GUID_MotifNode;
	}
	else
	{
		*pguid = GUID_PatternNode;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetNodeName

HRESULT CDirectMusicPattern::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetNodeNameMaxLength

HRESULT CDirectMusicPattern::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pnMaxLength == NULL )
	{
		return E_POINTER;
	}

	*pnMaxLength = DMUS_MAX_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::ValidateNodeName

HRESULT CDirectMusicPattern::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName = bstrName;
	::SysFreeString( bstrName );

	HRESULT hr = S_OK;

	// Store original Pattern name
	CString strOrigName = m_strName;

	// Make sure new Pattern name is unique
	m_strName = strName;

	if( m_pStyle->IsPatternNameUnique( this ) == FALSE )
	{
		CString strMsg;

		if( m_wEmbellishment & EMB_MOTIF )
		{
			AfxFormatString1( strMsg, IDS_DUPE_MOTIF_NAME, m_strName );
		}
		else
		{
			AfxFormatString1( strMsg, IDS_DUPE_PATTERN_NAME, m_strName );
		}

		AfxMessageBox( strMsg, MB_OK );
		hr = S_FALSE;
	}

	// Put back original Pattern name
	m_strName = strOrigName;

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::SetNodeName

HRESULT CDirectMusicPattern::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( strName.CompareNoCase( m_strName ) != 0 )
	{
		if( m_wEmbellishment & EMB_MOTIF )
		{
			m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_MOTIF_NAME );
		}
		else
		{
			m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_PATTERN_NAME );
		}
	}

	CString strOrigName = m_strName;
	m_strName = strName;

	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		RELEASE( pIPropSheet );
	}

	if( strOrigName.Compare( m_strName ) != 0 )
	{
		BSTR bstrTransportName;

		SetModified( TRUE );
		if( SUCCEEDED ( GetName( &bstrTransportName ) ) )
		{
			theApp.m_pStyleComponent->m_pIConductor->SetTransportName( this, bstrTransportName );
			if( m_wEmbellishment & EMB_MOTIF )
			{
				if( SUCCEEDED ( GetName( &bstrTransportName ) ) )
				{
					theApp.m_pStyleComponent->m_pIConductor->SetSecondaryTransportName( this, bstrTransportName );
				}
			}
		}
		m_pStyle->SyncStyleEditor( m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();

		// Notify connected nodes that Motif name has changed
		{
			DMUSProdMotifData	mtfData;
			WCHAR				wstrMotifName[MAX_PATH];
			WCHAR				wstrOldMotifName[MAX_PATH];
		
			MultiByteToWideChar( CP_ACP, 0, m_strName, -1, wstrMotifName, MAX_PATH );
			mtfData.pwszMotifName = wstrMotifName;

			MultiByteToWideChar( CP_ACP, 0, strOrigName, -1, wstrOldMotifName, MAX_PATH );
			mtfData.pwszOldMotifName = wstrOldMotifName;
			
			theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( m_pStyle, STYLE_MotifNameChange, &mtfData );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetNodeListInfo

HRESULT CDirectMusicPattern::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
//	AFX_MANAGE_STATE( _afxModuleAddrThis );
	UNREFERENCED_PARAMETER(pListInfo);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetEditorClsId

HRESULT CDirectMusicPattern::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pClsId = CLSID_PatternEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetEditorTitle

HRESULT CDirectMusicPattern::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	if( pbstrTitle == NULL )
	{
		return E_POINTER;
	}

	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	if( m_wEmbellishment & EMB_MOTIF )
	{
		// "Motif: "
		::LoadString( theApp.m_hInstance, IDS_MOTIF_TEXT, achTitle, MID_BUFFER );
	}
	else
	{
		// "Pattern: "
		::LoadString( theApp.m_hInstance, IDS_PATTERN_TEXT, achTitle, MID_BUFFER );
	}
	strTitle  = achTitle;
	strTitle += _T(": " );

	// plus "Style name - "
	BSTR bstrStyleName;

	if( SUCCEEDED ( m_pStyle->GetNodeName( &bstrStyleName ) ) )
	{
		CString strStyleName = bstrStyleName;
		::SysFreeString( bstrStyleName );
		strTitle += strStyleName;
		strTitle += " - ";
	}

	// plus "name"
	strTitle += m_strName;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetEditorWindow

HRESULT CDirectMusicPattern::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( hWndEditor == NULL )
	{
		return E_POINTER;
	}

	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::SetEditorWindow

HRESULT CDirectMusicPattern::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::UseOpenCloseImages

HRESULT CDirectMusicPattern::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfUseOpenCloseImages == NULL )
	{
		return E_POINTER;
	}

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetRightClickMenuId

HRESULT CDirectMusicPattern::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( phInstance == NULL
	||  pnMenuId == NULL )
	{
		return E_POINTER;
	}

	*phInstance = theApp.m_hInstance;

	if( m_wEmbellishment & EMB_MOTIF )
	{
		*pnMenuId = IDM_MOTIF_NODE_RMENU;
	}
	else
	{
		*pnMenuId = IDM_PATTERN_NODE_RMENU;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::OnRightClickMenuInit

HRESULT CDirectMusicPattern::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CMenu menu;

	if( menu.Attach(hMenu) )
	{
		// IDM_NEW_BAND
		if( m_wEmbellishment & EMB_MOTIF
		&&  m_pIBandNode == NULL )
		{
			// Only applies to Motifs that do not already have Bands
			menu.EnableMenuItem( IDM_NEW_BAND, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			menu.EnableMenuItem( IDM_NEW_BAND, (MF_GRAYED | MF_BYCOMMAND) );
		}

		// IDM_CUT
		if( CanCut() == S_OK )
		{
			menu.EnableMenuItem( IDM_CUT, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			menu.EnableMenuItem( IDM_CUT, (MF_GRAYED | MF_BYCOMMAND) );
		}

		// IDM_COPY
		if( CanCopy() == S_OK )
		{
			menu.EnableMenuItem( IDM_COPY, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			menu.EnableMenuItem( IDM_COPY, (MF_GRAYED | MF_BYCOMMAND) );
		}

		// IDM_PASTE
		{
			IDataObject* pIDataObject;
			BOOL fWillSetReference;

			menu.EnableMenuItem( IDM_PASTE, (MF_GRAYED | MF_BYCOMMAND) );

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					menu.EnableMenuItem( IDM_PASTE, (MF_ENABLED | MF_BYCOMMAND) );
				}
				
				pIDataObject->Release();
			}
		}

		// IDM_DELETE
		if( CanDelete() == S_OK )
		{
			menu.EnableMenuItem( IDM_DELETE, (MF_ENABLED | MF_BYCOMMAND) );
		}
		else
		{
			menu.EnableMenuItem( IDM_DELETE, (MF_GRAYED | MF_BYCOMMAND) );
		}

		menu.Detach();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::OnRightClickMenuSelect

HRESULT CDirectMusicPattern::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	IDataObject* pIDataObject;

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_NEW_BAND:
			// Only applies to Motifs
			if( m_wEmbellishment & EMB_MOTIF )
			{
				hr = InsertChildNode( NULL );
			}
			break;

		case IDM_CUT:
			if( CanCut() == S_OK )
			{
				if( SUCCEEDED ( CreateDataObject( &pIDataObject ) ) )
				{
					if( theApp.PutDataInClipboard( pIDataObject, this ) )
					{
						DeleteNode( FALSE );
					}
					
					RELEASE( pIDataObject );
				}
			}
			break;
	
		case IDM_COPY:
			if( CanCopy() == S_OK )
			{
				if( SUCCEEDED ( CreateDataObject( &pIDataObject ) ) )
				{
					theApp.PutDataInClipboard( pIDataObject, this );
					
					RELEASE( pIDataObject );
				}
			}
			break;
	
		case IDM_PASTE:
		{
			BOOL fWillSetReference;

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					PasteFromData( pIDataObject );
				}
				
				RELEASE( pIDataObject );
			}
			break;
		}
	
		case IDM_RENAME:
			hr = theApp.m_pStyleComponent->m_pIFramework->EditNodeLabel( this );
			break;

		case IDM_DELETE:
			hr = DeleteNode( TRUE );
			break;

		case IDM_PROPERTIES:
			OnShowProperties();
			hr = S_OK;
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::DeleteChildNode

HRESULT CDirectMusicPattern::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIChildNode != NULL );
	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have children
		return E_FAIL;
	}

	CWaitCursor wait;

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Remove node from Project Tree
	if( theApp.m_pStyleComponent->m_pIFramework->RemoveNode( pIChildNode, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Reset Style's active Band to NULL (when applicable)
	m_pStyle->RemoveActiveBand( pIChildNode );

	// Save undo state
//	m_pStyle->m_pINodeBeingDeleted = pIChildNode;
//	m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_DELETE_BAND );
//	m_pStyle->m_pINodeBeingDeleted = NULL;

	// Remove from Motif
	RELEASE( m_pIBandNode );

	SetModified( TRUE );

	// Update the DirectMusic Style object
	HRESULT hr = m_pStyle->SyncStyleWithDirectMusic();
	ASSERT( SUCCEEDED ( hr ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::InsertChildNode

HRESULT CDirectMusicPattern::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have children
		return E_FAIL;
	}

	IDMUSProdDocType* pIDocType;

	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( pIChildNode == NULL )
	{
		HRESULT hr;

		hr = theApp.m_pStyleComponent->m_pIFramework->FindDocTypeByNodeId( GUID_BandNode, &pIDocType );
		if( SUCCEEDED ( hr ) ) 
		{
			// Create a new Band 
			hr = pIDocType->AllocNode( GUID_BandNode, &pIChildNode );
			if( !SUCCEEDED ( hr ) )
			{
				pIChildNode = NULL;
			}

			RELEASE( pIDocType );
		}

		if( pIChildNode == NULL )
		{
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		pIChildNode->AddRef();
	}

	// Motif can only have one Band
	if( m_pIBandNode )
	{
		DeleteChildNode( m_pIBandNode, FALSE );
	}

	// Set the Motif's Band
	ASSERT( m_pIBandNode == NULL );
	m_pIBandNode = pIChildNode;

	// Set root and parent node of ALL children
	theApp.SetNodePointers( m_pIBandNode, (IDMUSProdNode *)m_pStyle, (IDMUSProdNode *)this );

	// Add node to Project Tree
	if( !SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AddNode(m_pIBandNode, (IDMUSProdNode *)this) ) )
	{
		DeleteChildNode( m_pIBandNode, FALSE );
		return E_FAIL;
	}

	// Make sure "Default" flag is off
	IDMUSProdBandEdit* pIBandEdit;

	if( SUCCEEDED ( pIChildNode->QueryInterface( IID_IDMUSProdBandEdit, (void**)&pIBandEdit ) ) )
	{
		pIBandEdit->SetDefaultFlag( FALSE );
		RELEASE( pIBandEdit );
	}

	SetModified( TRUE );

	// Update the DirectMusic Style object
	HRESULT hr = m_pStyle->SyncStyleWithDirectMusic();
	ASSERT( SUCCEEDED ( hr ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::DeleteNode

HRESULT CDirectMusicPattern::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIParentNode;

	// Let our parent delete us
	if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
	{
		if( pIParentNode )
		{
			HRESULT hr = pIParentNode->DeleteChildNode( this, fPromptUser );
			RELEASE( pIParentNode );
			return hr;
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::OnNodeSelChanged

HRESULT CDirectMusicPattern::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );

	if( fSelected )
	{
		if( m_pIDocRootNode == this )
		{
			// if the Pattern isn't part of a Style, SetActiveTransport to NULL so
			// the Conductor knows that this isn't a playable Pattern (until
			// the music engine is updated to support Pattern w/o Style playback
			theApp.m_pStyleComponent->m_pIConductor->SetActiveTransport( NULL, 0 );	
		}
		else
		{
			DWORD dwButtonState = BS_AUTO_UPDATE;
			if( m_pPatternCtrl != NULL && m_pPatternCtrl->m_pPatternDlg != NULL &&
				m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr != NULL )
			{
				dwButtonState |= BS_RECORD_ENABLED;
				if( m_fRecordPressed )
				{
					dwButtonState |= BS_RECORD_CHECKED;
				}
			}
			theApp.m_pStyleComponent->m_pIConductor->SetActiveTransport( this, dwButtonState );	
			theApp.m_pStyleComponent->m_pIConductor->SetTempo( this, m_pStyle->m_dblTempo, TRUE );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::CreateDataObject

HRESULT CDirectMusicPattern::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

	// Save Pattern into stream
	HRESULT hr = E_FAIL;
	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_SinglePattern, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			UINT uClipFormat;

			if( m_wEmbellishment & EMB_MOTIF )
			{
				// Place CF_MOTIF into CDllJazzDataObject
				uClipFormat = theApp.m_pStyleComponent->m_cfMotif;
			}
			else
			{
				// Place CF_PATTERN into CDllJazzDataObject
				uClipFormat = theApp.m_pStyleComponent->m_cfPattern;
			}

			// Place stream into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( uClipFormat, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		RELEASE( pIStream );
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
// CDirectMusicPattern IDMUSProdNode::CanCut

HRESULT CDirectMusicPattern::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::CanCopy

HRESULT CDirectMusicPattern::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::CanDelete

HRESULT CDirectMusicPattern::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode != this )
	{
		ASSERT( m_pIParentNode != NULL );

		return m_pIParentNode->CanDeleteChildNode( this );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::CanDeleteChildNode

HRESULT CDirectMusicPattern::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have children
		return E_FAIL;
	}

	if( m_pIBandNode )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::CanPasteFromData

HRESULT CDirectMusicPattern::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Can paste Bands on Motifs
	if( m_wEmbellishment & EMB_MOTIF )
	{
		// Create a new CDllJazzDataObject and see if it can read the data object's format.
		CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
		if( pDataObject == NULL )
		{
			return E_OUTOFMEMORY;
		}

		if( pDataObject->IsClipFormatAvailable(pIDataObject, theApp.m_pStyleComponent->m_cfBand) == S_OK
		||  pDataObject->IsClipFormatAvailable(pIDataObject, theApp.m_pStyleComponent->m_cfBandList) == S_OK 
		||  pDataObject->IsClipFormatAvailable(pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack) == S_OK 
		||  m_pStyle->ContainsClipFormat(pDataObject, pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack) == S_OK )
		{
			RELEASE( pDataObject );
			return S_OK;
		}

		RELEASE( pDataObject );
	}

	if( m_pIDocRootNode != this )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let Style decide what can be dropped
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	// Can't drop anything on a Pattern file in the Project Tree
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::PasteFromData

HRESULT CDirectMusicPattern::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Can paste Bands on Motifs
	if( m_wEmbellishment & EMB_MOTIF )
	{
		// Create a new CDllJazzDataObject and see if it can read the data object's format.
		CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
		if( pDataObject == NULL )
		{
			return E_OUTOFMEMORY;
		}

		HRESULT hr = E_FAIL;

		if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) ) )
		{
			// Handle CF_BAND 
			hr = PasteCF_BAND( pDataObject, pIDataObject );
		}

		else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) ) )
		{
			// Handle CF_BANDLIST format
			hr = PasteCF_BANDLIST( pDataObject, pIDataObject );
		}

		else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) )
		{
			// Handle CF_BANDTRACK format
			hr = PasteCF_BANDTRACK( pDataObject, pIDataObject );
		}

		else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfTimeline ) ) )
		{
			// Handle CF_TIMELINE format
			hr = PasteCF_TIMELINE ( pDataObject, pIDataObject );
		}

		RELEASE( pDataObject );

		if( hr == S_OK )
		{
			return hr;
		}
	}

	if( m_pIDocRootNode != this )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let Style handle paste
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	// Can't drop anything on a Pattern file in the Project Tree
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::CanChildPasteFromData

HRESULT CDirectMusicPattern::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												    BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have children
		return E_FAIL;
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
	||  SUCCEEDED ( m_pStyle->ContainsClipFormat( pDataObject, pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::ChildPasteFromData

HRESULT CDirectMusicPattern::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL
	||  pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have children
		return E_FAIL;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) ) )
	{
		// Handle CF_BAND format
		hr = PasteCF_BAND( pDataObject, pIDataObject );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) ) )
	{
		// Handle CF_BANDLIST format
		hr = PasteCF_BANDLIST( pDataObject, pIDataObject );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) )
	{
		// Handle CF_BANDTRACK format
		hr = PasteCF_BANDTRACK( pDataObject, pIDataObject );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfTimeline ) ) )
	{
		// Handle CF_TIMELINE format
		hr = PasteCF_TIMELINE ( pDataObject, pIDataObject );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNode::GetObject

HRESULT CDirectMusicPattern::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if( ::IsEqualCLSID( rclsid, GUID_TimeSignature ) && ::IsEqualIID( riid, GUID_TimeSignature ) )
	{
		DMUS_TIMESIGNATURE *pTimeSig = (DMUS_TIMESIGNATURE *)ppvObject;
		ASSERT( pTimeSig );
		if( !pTimeSig )
		{
			return E_POINTER;
		}

		pTimeSig->bBeat = m_TimeSignature.m_bBeat;
		pTimeSig->bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
		pTimeSig->wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;
		pTimeSig->mtTime = 0;

		return S_OK;
	}

	else if( ::IsEqualCLSID( rclsid, GUID_TempoParam ) && ::IsEqualIID( riid, GUID_TempoParam ) )
	{
		DMUS_TEMPO_PARAM *pTempo = (DMUS_TEMPO_PARAM *)ppvObject;
		ASSERT( pTempo );
		if( !pTempo )
		{
			return E_POINTER;
		}

		if( !m_pStyle )
		{
			return E_FAIL;
		}
		
		pTempo->mtTime = 0;
		pTempo->dblTempo = m_pStyle->m_dblTempo;

		return S_OK;
	}

	else if( ::IsEqualCLSID( rclsid, GUID_ConductorCountInBeatOffset ) && ::IsEqualIID( riid, GUID_ConductorCountInBeatOffset ) )
	{
		if( m_pPatternCtrl && m_pPatternCtrl->m_pPatternDlg &&
			 m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
		{
			MUSIC_TIME mtTime;
			if( SUCCEEDED(	m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
			{
				MUSIC_TIME *pmtOffset = (MUSIC_TIME *)ppvObject;
				*pmtOffset = mtTime % (DM_PPQNx4 / m_TimeSignature.m_bBeat);
			}
		}
	}

    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersist::GetClassID

HRESULT CDirectMusicPattern::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

	if( pClsId == NULL )
	{
		return E_POINTER;
	}

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersistStream implementation


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersistStream::Load

HRESULT CDirectMusicPattern::Load( IStream* pIStream )
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
		ckMain.fccType = DMUS_FOURCC_PATTERN_LIST;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) == 0 )
		{
			hr = DM_LoadPattern( pIRiffStream, &ckMain, false );
			fFoundFormat = TRUE;
		}

		RELEASE( pIRiffStream );
	}

	// Check for Direct Music single Pattern format
	if( fFoundFormat == FALSE )
	{
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_PATTERN_LIST;

			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				hr = DM_LoadPattern( pIRiffStream, &ckMain, true );
				fFoundFormat = TRUE;
			}

			RELEASE( pIRiffStream );
		}
	}

	// Check for IMA 2.5 format
	if( fFoundFormat == FALSE )
	{
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );

		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = FOURCC_PATTERN_FORM;

			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				hr = IMA25_LoadPattern( pIRiffStream, &ckMain );
				fFoundFormat = TRUE;
			}

			RELEASE( pIRiffStream );
		}
	}

	if( m_pPatternCtrl && m_pPatternCtrl->m_pPatternDlg )
	{
		// Update the horizontal zoom, horizontal scroll, and vertical scroll
		m_pPatternCtrl->m_pPatternDlg->UpdateZoomAndScroll();
	}


    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersistStream::Save

HRESULT CDirectMusicPattern::Save( IStream* pIStream, BOOL fClearDirty )
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

	// Save the Pattern
	if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion )
	||  ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_DirectMusicObject ) )
	{
		// Direct Music format
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_PATTERN_LIST;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATELIST ) == 0
			&&  SUCCEEDED( DM_SavePattern( pIRiffStream, fClearDirty ) )
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

	else if( ::IsEqualGUID( StreamInfo.guidDataFormat, GUID_SinglePattern ) )
	{
		// Single Pattern (Direct Music format)
		// Used by clipboard and MIDI Manager
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_PATTERN_LIST;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( DM_SaveSinglePattern( pIRiffStream, fClearDirty ) )
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

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern additional functions


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::FindPartRefByPChannelAndGUID

CDirectMusicPartRef* CDirectMusicPattern::FindPartRefByPChannelAndGUID( DWORD dwPChannel, REFGUID guidPart ) const
{
	CDirectMusicPartRef* pPartRef;

	POSITION pos = m_lstPartRefs.GetHeadPosition();

	while( pos )
	{
		pPartRef = m_lstPartRefs.GetNext( pos );

		if( pPartRef->m_dwPChannel == dwPChannel )
		{
			if( ::IsEqualGUID( guidPart, pPartRef->m_pDMPart->m_guidPartID ) 
			||  ::IsEqualGUID( guidPart, pPartRef->m_guidOldPartID ) )
			{
				return pPartRef;
			}
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::RecalcLength

void CDirectMusicPattern::RecalcLength( void )
{
	// Store the original length
	DWORD dwOrigLength = m_dwLength;

	// Set m_dwLength
	DWORD dwClocksPerBeat = DM_PPQNx4 / m_TimeSignature.m_bBeat;
	DWORD dwClocksPerMeasure = dwClocksPerBeat * (DWORD)m_TimeSignature.m_bBeatsPerMeasure;
	m_dwLength = dwClocksPerMeasure * m_wNbrMeasures;

	// Adjust m_mtLoopEnd
	if( m_mtLoopEnd == (long)dwOrigLength 
	||  m_mtLoopEnd > (long)m_dwLength )
	{
		m_mtLoopEnd = m_dwLength;
	}

	// Adjust m_mtLoopStart
	if( m_mtLoopStart >= m_mtLoopEnd ) 
	{
		m_mtLoopStart = m_mtLoopEnd - 1;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::MusicValueToNote

/*
unsigned char CDirectMusicPattern::MusicValueToNote(

unsigned short value,   // Music value to convert.
char scalevalue,        // Scale value if chord failes.
long keypattern,        // Description of key as interval pattern.
char keyroot,           // Root note of key.
long chordpattern,      // Description of chord as interval pattern.
char chordroot,         // Root note of chord.
char count)             // Total notes in chord.

{
unsigned char   result ;
char            octpart   = (char)(value >> 12) ;
char            chordpart = (char)((value >> 8) & 0xF) ;
char            keypart   = (char)((value >> 4) & 0xF) ;
char            accpart   = (char)(value & 0xF) ;
unsigned char   bits      = (unsigned char) (count & 0xF0) ;

    count  &= CHORD_COUNT ;

    result  = unsigned char(12 * octpart) ;
    result += chordroot ;

    if( accpart > 8 )
        accpart -= 16 ;

    if( count ) {
        if( bits & CHORD_FOUR ) {
            if( count > 4 )
                chordpart += (count - 4) ;
        } else {
            if( count > 3 )
                chordpart += (count - 3) ;
        }
    }

//    if( chordpart ) {
        for( ;  chordpattern ;  result++ ) {
            if( chordpattern & 1L ) {
                if( !chordpart )
                    break ;
                chordpart-- ;
            }
            chordpattern = chordpattern >> 1L ;
            if( !chordpattern ) {
                if( !scalevalue )
                    return( 0 ) ;
                result  = unsigned char(12 * octpart) ;
                result += chordroot ;
                keypart = char(scalevalue >> 4) ;
                accpart = char(scalevalue & 0x0F) ;
                break ;
            }
        }
//    }

    if( keypart ) {
        keypattern |= (keypattern << 12L) ;
        keypattern  = keypattern >> (LONG)((result - keyroot) % 12) ;
        for( ;  keypattern ;  result++ ) {
            if( keypattern & 1L ) {
                if( !keypart )
                    break ;
                keypart-- ;
            }
            keypattern = keypattern >> 1L ;
        }
    }

    result += unsigned char(accpart) ;
    return( result ) ;

}
*/


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::NoteToMusicValue
/*
WORD CDirectMusicPattern::NoteToMusicValue( BYTE bMIDINote, BYTE bPlayModes,
											const DMUS_SUBCHORD &dmSubChord )
{
	ASSERT( bPlayModes != DMUS_PLAYMODE_NONE );

	if( bPlayModes == DMUS_PLAYMODE_FIXED )
	{
		return (WORD)bMIDINote;
	}
	else if( bPlayModes == DMUS_PLAYMODE_FIXEDTOSCALE )
	{
		return (bMIDINote >= dmSubChord.bScaleRoot) ?
			WORD(bMIDINote - dmSubChord.bScaleRoot) : 0;
	}
	else if( bPlayModes == DMUS_PLAYMODE_FIXEDTOCHORD )
	{
		return (bMIDINote >= (dmSubChord.bChordRoot % 12) + dmSubChord.bScaleRoot) ?
			WORD(bMIDINote - (dmSubChord.bChordRoot % 12) - dmSubChord.bScaleRoot) : 0;
	}

	WORD wNewMusicValue = OldNoteToMusicValue( bMIDINote, dmSubChord.dwScalePattern,
			dmSubChord.bScaleRoot, dmSubChord.dwChordPattern, dmSubChord.bChordRoot );

	// If DMUS_PLAYMODE_CHORD_ROOT is set, take the result up an octave.
	if( bPlayModes & DMUS_PLAYMODE_CHORD_ROOT )
	{
		WORD wOctave = (wNewMusicValue % 0xF000) >> 12;
		wOctave++;
		wNewMusicValue = (wNewMusicValue & 0x0FFF) | (wOctave << 12);
	}

	return wNewMusicValue;
}
*/


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_SavePatternEditInfo

HRESULT CDirectMusicPattern::DM_SavePatternEditInfo( IDMUSProdRIFFStream* pIRiffStream )
{
    MMCKINFO ckMain;
	DWORD dwBytesWritten;
    HRESULT hr = S_OK;

    IStream* pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

	if( m_pPatternDesignData == NULL )
	{
		hr = S_FALSE;
		goto ON_ERROR;
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

	if( StreamInfo.ftFileType != FT_DESIGN )
	{
		hr = S_FALSE;
		goto ON_ERROR;
	}

	// Write DMUS_FOURCC_PATTERN_DESIGN LIST header
	ckMain.fccType = DMUS_FOURCC_PATTERN_DESIGN;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Pattern design data
	hr = pIStream->Write( m_pPatternDesignData,
						  m_dwPatternDesignDataSize,
						  &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != m_dwPatternDesignDataSize )
	{
		hr = E_FAIL;
		goto ON_ERROR;
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
// CDirectMusicPattern::DM_SavePatternChunk

HRESULT CDirectMusicPattern::DM_SavePatternChunk( IDMUSProdRIFFStream* pIRiffStream )
{
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_PATTERN dmusPatternIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Pattern chunk header
    ck.ckid = DMUS_FOURCC_PATTERN_CHUNK;
    if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_PATTERN
	memset( &dmusPatternIO, 0, sizeof(DMUS_IO_PATTERN) );

	dmusPatternIO.timeSig.bBeatsPerMeasure = m_TimeSignature.m_bBeatsPerMeasure;
	dmusPatternIO.timeSig.bBeat = m_TimeSignature.m_bBeat;
	dmusPatternIO.timeSig.wGridsPerBeat = m_TimeSignature.m_wGridsPerBeat;

	dmusPatternIO.bGrooveBottom = m_bGrooveBottom;
	dmusPatternIO.bGrooveTop = m_bGrooveTop;
	dmusPatternIO.bDestGrooveBottom = m_bDestGrooveBottom;
	dmusPatternIO.bDestGrooveTop = m_bDestGrooveTop;

	dmusPatternIO.dwFlags = m_dwFlags;

	dmusPatternIO.wEmbellishment = m_wEmbellishment;
	dmusPatternIO.wNbrMeasures = m_wNbrMeasures;

	// Write Pattern chunk data
	hr = pIStream->Write( &dmusPatternIO, sizeof(DMUS_IO_PATTERN), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_PATTERN) )
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
// CDirectMusicPattern::DM_SavePatternUIChunk

HRESULT CDirectMusicPattern::DM_SavePatternUIChunk( IDMUSProdRIFFStream* pIRiffStream )
{
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	ioDMPatternUI8 oDMPatternUI;

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

	// Only save this chunk in "Design" files
	if( StreamInfo.ftFileType != FT_DESIGN )
	{
		hr = S_OK;
		goto ON_ERROR;	// Not an error, just need to free pIStream
	}

	// Write Pattern UI chunk header
    ck.ckid = DMUS_FOURCC_PATTERN_UI_CHUNK;
    if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioDMPatternUI8
	memset( &oDMPatternUI, 0, sizeof(ioDMPatternUI8) );

	oDMPatternUI.m_dwDefaultKeyPattern = m_dwDefaultKeyPattern;
	oDMPatternUI.m_dwDefaultChordPattern = m_dwDefaultChordPattern;
	oDMPatternUI.m_bDefaultKeyRoot = m_bDefaultKeyRoot;
	oDMPatternUI.m_bDefaultChordRoot = m_bDefaultChordRoot;
	oDMPatternUI.m_fDefaultKeyFlatsNotSharps = m_fKeyFlatsNotSharps;	
	oDMPatternUI.m_fDefaultChordFlatsNotSharps = m_fChordFlatsNotSharps;

	// Sync up Timeline data, if our editor is open
	SyncTimelineSettings();

	// Copy Timeline settings
	oDMPatternUI.m_dblZoom = m_dblZoom;
	oDMPatternUI.m_dblHorizontalScroll = m_dblHorizontalScroll;
	oDMPatternUI.m_lVerticalScroll = m_lVerticalScroll;
	oDMPatternUI.m_dwSnapTo = m_tlSnapTo;
	oDMPatternUI.m_lFunctionbarWidth = m_lFunctionbarWidth;

	// Write Pattern UI chunk data
	hr = pIStream->Write( &oDMPatternUI, sizeof(ioDMPatternUI8), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioDMPatternUI8) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
/* not necessary, this info is in ioDMPatternUI8	
	// so we can display the right enharmonics in pattern editor
	hr = pIStream->Write(&m_fChordFlatsNotSharps, sizeof(BOOL), &dwBytesWritten);
	if(SUCCEEDED(hr) && dwBytesWritten == sizeof(BOOL))
	{
		hr = pIStream->Write(&m_fKeyFlatsNotSharps, sizeof(BOOL), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(BOOL) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}			
	}
*/
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
// CDirectMusicPattern::DM_SavePattern

HRESULT CDirectMusicPattern::DM_SavePattern( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty )
{
	HRESULT hr = S_OK;
	IStream* pIStream;
	POSITION pos;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

// Save Pattern chunk
	hr = DM_SavePatternChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Pattern UI chunk
	hr = DM_SavePatternUIChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Pattern rhythm
	hr = DM_SavePatternRhythm( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Pattern info
	hr = DM_SavePatternInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Motif Settings
	hr = DM_SaveMotifSettingsChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Motif Band
	if( m_wEmbellishment & EMB_MOTIF )
	{
		if( m_pIBandNode )
		{
			IPersistStream* pIPersistStream;

			hr = m_pIBandNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
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

// Save Pattern Edit info
	hr = DM_SavePatternEditInfo( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Part References
	CDirectMusicPartRef* pPartRef;

    pos = m_lstPartRefs.GetHeadPosition();
    while( pos )
    {
        pPartRef = m_lstPartRefs.GetNext( pos );
		hr = pPartRef->DM_SavePartRef( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
    }

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_SaveSinglePattern

HRESULT CDirectMusicPattern::DM_SaveSinglePattern( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty )
{
	CTypedPtrList<CPtrList, CDirectMusicPart*> lstParts;
	CDirectMusicPartRef* pPartRef;
	CDirectMusicPart* pPart;
	HRESULT hr = S_OK;
	IStream* pIStream;
	POSITION pos;
	POSITION pos2;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

// Save Pattern chunk
	hr = DM_SavePatternChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Pattern UI chunk
	hr = DM_SavePatternUIChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Pattern rhythm
	hr = DM_SavePatternRhythm( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Pattern info
	hr = DM_SavePatternInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Motif Settings
	hr = DM_SaveMotifSettingsChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Motif Band
	if( m_wEmbellishment & EMB_MOTIF )
	{
		if( m_pIBandNode )
		{
			IPersistStream* pIPersistStream;

			hr = m_pIBandNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
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

// Save Pattern Edit info
	hr = DM_SavePatternEditInfo( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Build list of Parts
    pos = m_lstPartRefs.GetHeadPosition();
    while( pos )
    {
        pPartRef = m_lstPartRefs.GetNext( pos );
		ASSERT( pPartRef->m_pDMPart != NULL );

		pos2 = lstParts.Find( pPartRef->m_pDMPart );
		if( pos2 == NULL )
		{
			lstParts.AddTail( pPartRef->m_pDMPart ); 
		}
    }

// Save Parts
    pos = lstParts.GetHeadPosition();
    while( pos )
    {
        pPart = lstParts.GetNext( pos );

		hr = pPart->DM_SavePart( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
    }

// Save Part References
    pos = m_lstPartRefs.GetHeadPosition();
    while( pos )
    {
        pPartRef = m_lstPartRefs.GetNext( pos );

		hr = pPartRef->DM_SavePartRef( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
    }

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::IMA25_LoadNoteList

HRESULT CDirectMusicPattern::IMA25_LoadNoteList( IStream* pIStream, MMCKINFO* pckMain, short nClickTime )
{
	HRESULT		hr = S_OK;;
    ioNote      iNote;
    DWORD       dwByteCount;
    WORD        wNoteSize;
    WORD        wExtra;
    long        dwSize;
    CDirectMusicEventItem*  pNote = NULL;

    dwSize = pckMain->cksize;

	// read size of the note structure
    hr = pIStream->Read( &wNoteSize, sizeof( wNoteSize ), &dwByteCount );
    if( FAILED( hr )
	||  dwByteCount != sizeof( wNoteSize ) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	dwSize -= dwByteCount;

    if( wNoteSize > sizeof( ioNote ) )
	{
        wExtra = static_cast<WORD>( wNoteSize - sizeof( ioNote ) );
        wNoteSize = sizeof( ioNote );
	}
    else
    {
        wExtra = 0;
    }

    // now read in the notes
	while( dwSize > 0 )
	{
        hr = pIStream->Read( &iNote, wNoteSize, &dwByteCount );
        if( FAILED( hr )
		||  dwByteCount != wNoteSize )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
        dwSize -= wNoteSize;

        if( wExtra > 0 )
        {
            StreamSeek( pIStream, wExtra, STREAM_SEEK_CUR );
            dwSize -= wExtra;
        }

		// Create Direct Music Note
        pNote = new CDirectMusicStyleNote;
        if( pNote != NULL )
		{
			pNote->m_mtGridStart = nClickTime;

			pNote->m_nTimeOffset = (DM_PPQN / PPQN) * iNote.nTime;
            pNote->m_dwVariation = iNote.wVariation;
            ((CDirectMusicStyleNote*)pNote)->m_bVelocity = iNote.bVelocity;
            ((CDirectMusicStyleNote*)pNote)->m_mtDuration = (DM_PPQN / PPQN) * iNote.nDuration;
            ((CDirectMusicStyleNote*)pNote)->m_bTimeRange = iNote.bTimeRange;
            ((CDirectMusicStyleNote*)pNote)->m_bDurRange = iNote.bDurRange;
            ((CDirectMusicStyleNote*)pNote)->m_bVelRange = iNote.bVelRange;
            ((CDirectMusicStyleNote*)pNote)->m_bInversionId = 0;  // not in IMA2.5
            ((CDirectMusicStyleNote*)pNote)->m_bNoteFlags = 0;  // not in IMA2.5

			// Make sure SuperJAM! play mode is valid
			if ( !(iNote.bPlayMode & 0x80) )
        	{
        		iNote.bPlayMode = CHTYPE_NONE;
        	}
            
			// Strip 0x80 before checking SuperJAM! play mode
			switch( iNote.bPlayMode & 0x0F )
			{
				case CHTYPE_NOTINITIALIZED:
				case CHTYPE_NONE:
					((CDirectMusicStyleNote*)pNote)->m_bPlayModeFlags = DMUS_PLAYMODE_NONE;
					break;
				
				case CHTYPE_DRUM:
				case CHTYPE_FIXED:
				   ((CDirectMusicStyleNote*)pNote)->m_bPlayModeFlags = DMUS_PLAYMODE_FIXED;
				   break;

				case CHTYPE_UPPER:
				case CHTYPE_BASS:
					((CDirectMusicStyleNote*)pNote)->m_bPlayModeFlags = DMUS_PLAYMODE_NORMALCHORD;
					break;

				case CHTYPE_SCALEONLY:
					((CDirectMusicStyleNote*)pNote)->m_bPlayModeFlags = DMUS_PLAYMODE_PEDALPOINT;
					break;

				case CHTYPE_BASSMELODIC:
				case CHTYPE_UPPERMELODIC:
					((CDirectMusicStyleNote*)pNote)->m_bPlayModeFlags = DMUS_PLAYMODE_SCALE_INTERVALS | DMUS_PLAYMODE_CHORD_ROOT;
					break;

				default:
					ASSERT( 0 );
					if( iNote.bVoiceID == LOGICAL_PART_DRUM )
					{
						((CDirectMusicStyleNote*)pNote)->m_bPlayModeFlags = DMUS_PLAYMODE_FIXED;
					}
					else
					{
						((CDirectMusicStyleNote*)pNote)->m_bPlayModeFlags = DMUS_PLAYMODE_NORMALCHORD;
					}
			}

			// Handle SuperJAM! drum tracks
			if( iNote.bVoiceID == LOGICAL_PART_DRUM )
			{
				// Use SuperJAM! drum map to convert note values				
				if( iNote.bValue < 128
				&&  achMappingsToDMUS[iNote.bValue] < 128 )
				{
					iNote.bValue = achMappingsToDMUS[iNote.bValue];
				}
				else
				{
					iNote.bValue = 0;
				}
			}

			// Find Part
			CDirectMusicPart* pPart = FindPart( IMA25_VOICEID_TO_PCHANNEL( iNote.bVoiceID ) );
			ASSERT( pPart != NULL );

			// Determine playmode of note
			BYTE bNotePlayModeFlags;

			if( ((CDirectMusicStyleNote*)pNote)->m_bPlayModeFlags == DMUS_PLAYMODE_NONE )
			{
				bNotePlayModeFlags = pPart->m_bPlayModeFlags;
			}
			else
			{
				bNotePlayModeFlags = ((CDirectMusicStyleNote*)pNote)->m_bPlayModeFlags;
			}

			// if ScaleValue is non-zero and the note isn't a pedalpoint, it's purpleized.
			if( iNote.bScaleValue != 0
			&&  bNotePlayModeFlags != DMUS_PLAYMODE_PEDALPOINT )
			{
				((CDirectMusicStyleNote*)pNote)->m_bPlayModeFlags = DMUS_PLAYMODE_PURPLEIZED;
			}

			// if the note is a fixed event, use the MIDI value; otherwise use MusicValue
			if( bNotePlayModeFlags == DMUS_PLAYMODE_FIXED )
			{
				((CDirectMusicStyleNote*)pNote)->m_wMusicValue = iNote.bValue;
			}
			else
			{
				((CDirectMusicStyleNote*)pNote)->m_wMusicValue = iNote.nMusicValue;
			}

			// Place note in Part
			pPart->m_lstEvents.AddTail( pNote ); 
        }
    }

ON_ERROR:

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::IMA25_LoadCurveList

HRESULT CDirectMusicPattern::IMA25_LoadCurveList( IStream* pIStream, MMCKINFO* pckMain, short nClickTime )
{
	HRESULT		hr = S_OK;
	DWORD		dwByteCount;
    WORD        wCurveSize;
    WORD        wCurveExtra;
    WORD        wSubSize;
    WORD        wSubExtra;
	long		dwSize;
    ioSubCurve  iSubCurve;
    ioCurve     iCurve;
	WORD		wCount;
    CDirectMusicEventItem*  pCurve = NULL;

    dwSize = pckMain->cksize;

	// read size of the curve structure
    hr = pIStream->Read( &wCurveSize, sizeof( wCurveSize ), &dwByteCount );
    if( FAILED( hr )
	||  dwByteCount != sizeof( wCurveSize ) )
	{
        hr = E_FAIL;
		goto ON_ERROR;
	}
	dwSize -= dwByteCount;

    if( wCurveSize > sizeof( ioCurve ) )
	{
        wCurveExtra = static_cast<WORD>( wCurveSize - sizeof( ioCurve ) );
        wCurveSize = sizeof( ioCurve );
	}
    else
    {
        wCurveExtra = 0;
    }

    // read size of the subcurve structure
    hr = pIStream->Read( &wSubSize, sizeof( wSubSize ), &dwByteCount );
    if( FAILED( hr )
	||  dwByteCount != sizeof( wSubSize ) )
	{
        hr = E_FAIL;
		goto ON_ERROR;
	}
	dwSize -= dwByteCount;

    if( wSubSize > sizeof( ioSubCurve ) )
	{
        wSubExtra = static_cast<WORD>( wSubSize - sizeof( ioSubCurve ) );
        wSubSize = sizeof( ioSubCurve );
	}
    else
    {
        wSubExtra = 0;
    }

    // now read in the curve
	while( dwSize > 0 )
	{
        hr = pIStream->Read( &iCurve, wCurveSize, &dwByteCount );
        if( FAILED( hr )
		||  dwByteCount != wCurveSize )
		{
            hr = E_FAIL;
			goto ON_ERROR;
		}
		dwSize -= dwByteCount;

        if( wCurveExtra > 0 )
        {
            StreamSeek( pIStream, wCurveExtra, STREAM_SEEK_CUR );
            dwSize -= wCurveExtra;
        }

        pCurve = new CDirectMusicStyleCurve;
        if( pCurve == NULL )
		{
            hr = E_FAIL;
            goto ON_ERROR;
        }

        pCurve->m_dwVariation = iCurve.wVariation;
		pCurve->m_mtGridStart = nClickTime;
		((CDirectMusicStyleCurve*)pCurve)->m_bEventType = iCurve.bEventType;
		((CDirectMusicStyleCurve*)pCurve)->m_bCCData = iCurve.bCCData;

        // read subcurve count
        hr = pIStream->Read( &wCount, sizeof( wCount ), &dwByteCount );
        if( FAILED( hr )
		||  dwByteCount != sizeof( wCount ) )
		{
            hr = E_FAIL;
			goto ON_ERROR;
		}
		dwSize -= dwByteCount;

		BOOL fEnteredLoop = wCount > 0;
        for( ; wCount > 0 ; --wCount )
		{
            // read subcurves
            hr = pIStream->Read( &iSubCurve, wSubSize, &dwByteCount );
            if( FAILED( hr )
			||  dwByteCount != wSubSize )
            {
                hr = E_FAIL;
                goto ON_ERROR;
            }
            dwSize -= wSubSize;

            if( wSubExtra > 0 )
            {
                StreamSeek( pIStream, wSubExtra, STREAM_SEEK_CUR );
                dwSize -= wSubExtra;
            }
			
            ((CDirectMusicStyleCurve*)pCurve)->m_bCurveShape = iSubCurve.bCurveType;

			// Make sure nMaxTime > nMinTime
			if( iSubCurve.nMaxTime < iSubCurve.nMinTime )
			{
				short nMaxTime = iSubCurve.nMaxTime;
				iSubCurve.nMaxTime = iSubCurve.nMinTime;
				iSubCurve.nMinTime = nMaxTime;
			}

			// Make sure nMaxValue > nMinValue
			if( iSubCurve.nMaxValue < iSubCurve.nMinValue )
			{
				short nMaxValue = iSubCurve.nMaxValue;
				iSubCurve.nMaxValue = iSubCurve.nMinValue;
				iSubCurve.nMinValue = nMaxValue;
				iSubCurve.fFlipped ^= CURVE_FLIPVALUE; // toggle fFlipped --- different
			}

			if( iSubCurve.fFlipped & CURVE_FLIPTIME )
			{
				switch( ((CDirectMusicStyleCurve*)pCurve)->m_bCurveShape )
				{
					case DMUS_CURVES_LINEAR:
						if( iSubCurve.fFlipped & CURVE_FLIPVALUE )
						{
							iSubCurve.fFlipped = 0;
						}
						else
						{
							iSubCurve.fFlipped = CURVE_FLIPVALUE;
						}
						break;

					case DMUS_CURVES_INSTANT:
						iSubCurve.nMinTime = iSubCurve.nMaxTime;
						iSubCurve.nMaxTime = iSubCurve.nMinTime + 1;
						break;

					case DMUS_CURVES_EXP:
						((CDirectMusicStyleCurve*)pCurve)->m_bCurveShape = DMUS_CURVES_LOG;
						iSubCurve.fFlipped ^= CURVE_FLIPVALUE; // toggle fFlipped
						// log is the horiz flipped version of vertical flipped exp
						break;

					case DMUS_CURVES_LOG:
						((CDirectMusicStyleCurve*)pCurve)->m_bCurveShape = DMUS_CURVES_EXP;
						iSubCurve.fFlipped ^= CURVE_FLIPVALUE; // toggle fFlipped
						// exp is the horiz flipped version of vertical flipped log
						break;

					case DMUS_CURVES_SINE:
						iSubCurve.fFlipped ^= CURVE_FLIPVALUE; // toggle fFlipped
						// because horiz. and vert. flip are the same for sine wave
						break;

					default:
						ASSERT( 0 );
						break;
				}
			}
			
			if( iSubCurve.fFlipped & CURVE_FLIPVALUE )
			{
				((CDirectMusicStyleCurve*)pCurve)->m_nStartValue = iSubCurve.nMaxValue;
				((CDirectMusicStyleCurve*)pCurve)->m_nEndValue = iSubCurve.nMinValue;
			}
			else
			{
				((CDirectMusicStyleCurve*)pCurve)->m_nStartValue = iSubCurve.nMinValue;
				((CDirectMusicStyleCurve*)pCurve)->m_nEndValue = iSubCurve.nMaxValue;
			}

            pCurve->m_nTimeOffset = (DM_PPQN / PPQN) * (iSubCurve.nMinTime);
			((CDirectMusicStyleCurve*)pCurve)->m_mtDuration = (DM_PPQN / PPQN) * (iSubCurve.nMaxTime - iSubCurve.nMinTime);

			// Place curve in Part
			CDirectMusicPart* pPart = FindPart( IMA25_VOICEID_TO_PCHANNEL( iCurve.bVoiceID ) );
			ASSERT( pPart != NULL );
			if( pPart )
			{
				long lClocksPerBeat = DM_PPQNx4 / (long)pPart->m_TimeSignature.m_bBeat;
				long lClocksPerMeasure = lClocksPerBeat * (long)pPart->m_TimeSignature.m_bBeatsPerMeasure;
				long lPartClockLength = lClocksPerMeasure * (long)pPart->m_wNbrMeasures;

				((CDirectMusicStyleCurve*)pCurve)->SetDefaultResetValues( lPartClockLength );
				pPart->m_lstEvents.AddTail( pCurve ); 
			}
			
			pCurve = new CDirectMusicStyleCurve;
			if( pCurve == NULL )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			pCurve->m_dwVariation = iCurve.wVariation;
			pCurve->m_mtGridStart = nClickTime;
			((CDirectMusicStyleCurve*)pCurve)->m_bEventType = iCurve.bEventType;
			((CDirectMusicStyleCurve*)pCurve)->m_bCCData = iCurve.bCCData;
        }

		if( fEnteredLoop )
		{	
			delete pCurve; // the loop generates an extra one 
		}
    }

ON_ERROR:

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::IMA25_LoadEvents

HRESULT CDirectMusicPattern::IMA25_LoadEvents( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    ioClick iClick;
    HRESULT hr = S_OK;
	IStream* pIStream;
	MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;

    pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

	short nClickTime = 0;

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case FOURCC_CLICK:
				dwSize = min( ck.cksize, sizeof( iClick ) );
				hr = pIStream->Read( &iClick, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				nClickTime = iClick.lTime;
				break;
			
			case FOURCC_NOTE:
				hr = IMA25_LoadNoteList( pIStream, &ck, nClickTime);
				if( FAILED( hr ) )
				{
					goto ON_ERROR;
				}
				break;

			case FOURCC_CURVE:
				hr = IMA25_LoadCurveList( pIStream, &ck, nClickTime);
				if( FAILED( hr ) )
				{
					goto ON_ERROR;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
    }

ON_ERROR:
	RELEASE( pIStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::IMA25_LoadPattern

HRESULT CDirectMusicPattern::IMA25_LoadPattern( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	CDirectMusicPart *pPart;
	CDirectMusicPartRef* pPartRef;
    IStream*    pIStream;
    HRESULT     hr = S_OK;
	DWORD		dwByteCount;
	DWORD		dwSize;
	WORD		wKludge;
	MMCKINFO	ck;
	POSITION	pos;
	ioPattern   iPattern;
	int			i;
	int			j;

    ASSERT( m_pStyle != NULL );
	ASSERT( pIRiffStream != NULL );
    ASSERT( pckMain != NULL );

    pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case FOURCC_PATTERN:
				dwSize = min( ck.cksize, sizeof( iPattern ) );
				hr = pIStream->Read( &iPattern, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwDefaultKeyPattern = iPattern.dwKeyPattern;
				m_dwDefaultChordPattern = iPattern.dwChordPattern;
				m_bDefaultKeyRoot = iPattern.bRoot;		
				m_bDefaultChordRoot = (iPattern.bRoot + 12) % 24;

				// Make sure Pattern name is unique
				m_strName = iPattern.wstrName;
				m_pStyle->GetUniquePatternName( this );

				m_TimeSignature.m_bBeatsPerMeasure = (BYTE)( (iPattern.dwLength / (DWORD)iPattern.wClocksPerBeat) / (DWORD)iPattern.wMeasures );
				m_TimeSignature.m_bBeat = (BYTE)iPattern.wBeat;
				m_TimeSignature.m_wGridsPerBeat = iPattern.wClocksPerBeat / iPattern.wClocksPerClick;
				m_wNbrMeasures = iPattern.wMeasures;
				RecalcLength();

				// Set bottom of groove range
				if(  iPattern.fFlags & PF_A
				|| !(iPattern.fFlags & (PF_A | PF_B | PF_C | PF_D)) )
				{
					// Bottom of groove range is 1 if either level A or no levels were specified
					m_bGrooveBottom = 1;
				}
				else if( iPattern.fFlags & PF_B )
				{
					// 26 if level B is the lowest level specified
					m_bGrooveBottom = 26;
				}
				else if( iPattern.fFlags & PF_C )
				{
					// 51 if level C is the lowest level specified
					m_bGrooveBottom = 51;
				}
				else
				{
					// 76 if level D is the lowest level specified
					m_bGrooveBottom = 76;
				}

				// Set top of groove range
				if(  iPattern.fFlags & PF_D
				|| !(iPattern.fFlags & (PF_A | PF_B | PF_C | PF_D)) )
				{
					// Top of groove range is 100 if either level D or no levels were specified
					m_bGrooveTop = 100;
				}
				else if( iPattern.fFlags & PF_C )
				{
					// 75 if level C is the highest level specified
					m_bGrooveTop = 75;
				}
				else if( iPattern.fFlags & PF_B )
				{
					// 50 if level B is the highest level specified
					m_bGrooveTop = 50;
				}
				else
				{
					// 25 if level A is the highest level specified
					m_bGrooveTop = 25;
				}

				// Set embellishment
				m_wEmbellishment = EMB_NORMAL;
				if( iPattern.fFlags & PF_FILL )
				{
					m_wEmbellishment |= EMB_FILL;
				}
				if( iPattern.fFlags & PF_INTRO )
				{
					m_wEmbellishment |= EMB_INTRO;
				}
				if( iPattern.fFlags & PF_END )
				{
					m_wEmbellishment |= EMB_END;
				}
				if( iPattern.fFlags & PF_BREAK )
				{
					m_wEmbellishment |= EMB_BREAK;
				}
				if( iPattern.fFlags & PF_MOTIF )
				{
					m_wEmbellishment |= EMB_MOTIF;
				}

				// Create rhythm map
				DWORD dwRhythm;

				if( m_pRhythmMap )
				{
					delete [] m_pRhythmMap;
					m_pRhythmMap = NULL;
				}
				m_pRhythmMap = new DWORD[iPattern.wMeasures];
				if( m_pRhythmMap == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}
				if( iPattern.fFlags & PF_QUARTER )
				{
					dwRhythm = 15;	// bits 1-4
				}
				else if( iPattern.fFlags & PF_HALF )
				{
					dwRhythm = 5;	// bits 1 and 3
				}
				else
				{
					dwRhythm = 1;	// bit 1 set
				}
				for( i = 0; i < iPattern.wMeasures; ++i )
				{
					m_pRhythmMap[i] = dwRhythm;
				}

				// Create 16 Parts and 16 Part References
				for( i = 0 ; i < 16 ; ++i )			// Loop over Parts
				{
					// Create Part
					pPart = m_pStyle->AllocPart();
					if( pPart == NULL )
					{
						hr = E_OUTOFMEMORY;	
						goto ON_ERROR;
					}

					pPart->m_TimeSignature = m_TimeSignature;
					pPart->m_wNbrMeasures = m_wNbrMeasures;

					if( iPattern.wInvert & (1 << i) 
					&&  iPattern.wAutoInvert & (1 << i) )
					{
						pPart->m_pPartDesignData->m_bAutoInvert = 1;
					}

					if( iPattern.wInvert & (1 << i) )
					{
						pPart->m_bInvertLower = iPattern.abInvertLower[i];
						pPart->m_bInvertUpper = iPattern.abInvertUpper[i];
					}
					else
					{
						pPart->m_bInvertLower = 0;
						pPart->m_bInvertUpper = 127;
					}

					switch( iPattern.achChordChoice[i] )
					{
						case CHTYPE_DRUM:
						case CHTYPE_FIXED:
						   pPart->m_bPlayModeFlags = DMUS_PLAYMODE_FIXED;
						   break;

						 case CHTYPE_UPPER:
						 case CHTYPE_BASS:
							pPart->m_bPlayModeFlags = DMUS_PLAYMODE_NORMALCHORD;
							break;

						case CHTYPE_SCALEONLY:
							pPart->m_bPlayModeFlags = DMUS_PLAYMODE_PEDALPOINT;
							break;

						case CHTYPE_BASSMELODIC:
						case CHTYPE_UPPERMELODIC:
							pPart->m_bPlayModeFlags = DMUS_PLAYMODE_SCALE_INTERVALS | DMUS_PLAYMODE_CHORD_ROOT;
							break;

						default:
							ASSERT( 0 );
							pPart->m_bPlayModeFlags = DMUS_PLAYMODE_FIXED;
					}

					// If none of the variations have the ->I or ->V flag set,
					// set it in all variations
					wKludge = VF_TO1 | VF_TO5;
					for( j = 0 ; j < 16 ; ++j )
					{
						pPart->m_dwVariationChoices[j] = iPattern.awVarFlags[i][j];
						if( ( pPart->m_dwVariationChoices[j] & VF_TO1 ) != 0 )
						{
							wKludge &= ~VF_TO1;
						}
						if( ( pPart->m_dwVariationChoices[j] & VF_TO5 ) != 0 )
						{
							wKludge &= ~VF_TO5;
						}
					}
					if( wKludge != 0 )
					{
						for( j = 0 ; j < 16 ; ++j )
						{
							pPart->m_dwVariationChoices[j] |= wKludge;
						}
					}

					// Disable variations 17-32
					for( j = 16 ; j < NBR_VARIATIONS ; ++j )
					{
						if( pPart->m_pPartDesignData )
						{
							pPart->m_pPartDesignData->m_dwVariationsDisabled |= (1 << j);
							pPart->m_pPartDesignData->m_dwDisabledChoices[j] = pPart->m_dwVariationChoices[j];
							pPart->m_dwVariationChoices[j] &= 0xE0000000;
						}
					}

					// Create Part Reference
					pPartRef = AllocPartRef();
					if( pPartRef == NULL )
					{
						m_pStyle->DeletePart( pPart );
						hr = E_OUTOFMEMORY;	
						goto ON_ERROR;
					}

					pPartRef->SetPart( pPart );
					pPartRef->m_dwPChannel = IMA25_VOICEID_TO_PCHANNEL( i );
 					pPartRef->m_bVariationLockID = 0;	// no locking between parts...

					if( iPattern.achChordChoice[i] == CHTYPE_UPPER
					||	iPattern.achChordChoice[i] == CHTYPE_UPPERMELODIC )
					{
						pPartRef->m_bSubChordLevel = SUBCHORD_STANDARD_CHORD;
					}
					else
					{
						pPartRef->m_bSubChordLevel = SUBCHORD_BASS;
					}
				}
				break;

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case FOURCC_CLICK_LIST:
						IMA25_LoadEvents( pIRiffStream, &ck );
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
    }

	// Delete empty Parts
    pos = m_lstPartRefs.GetHeadPosition();
    while( pos )
    {
        pPartRef = m_lstPartRefs.GetNext( pos );

		pPart = pPartRef->m_pDMPart;
		ASSERT( pPart != NULL );

		if( pPart->m_lstEvents.IsEmpty() )
		{
			DeletePartRef( pPartRef );	// Will also delete Part
		}
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_LoadPattern

HRESULT CDirectMusicPattern::DM_LoadPattern( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, bool fSinglePattern )
{
	CDirectMusicPartRef* pPartRef;
	CDirectMusicPart* pPart;
    IStream*    pIStream;
    HRESULT     hr = S_OK;
	DWORD		dwByteCount;
	DWORD		dwSize;
	DWORD		dwPos;
	MMCKINFO	ck;
	MMCKINFO	ckList;
	int			i;
	BOOL		fUndoChangedLength = FALSE;
	WORD		wOrigEmbellishment = -1;
	WORD		wNewEmbellishment = -1;
	BOOL		fNeedToRequeuePatternPlayback = FALSE;

    ASSERT( m_pStyle != NULL );
	ASSERT( pIRiffStream != NULL );
    ASSERT( pckMain != NULL );

    pIStream = pIRiffStream->GetStream();
    ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

	// Keep track of original PartRef list
	CTypedPtrList<CPtrList, CDirectMusicPartRef*> lstOrigPartRefs;

	while( !m_lstPartRefs.IsEmpty() )
	{
		pPartRef = m_lstPartRefs.RemoveHead();
		lstOrigPartRefs.AddTail( pPartRef );
	}

	// Store values in original Pattern
	CString strOrigName = m_strName;

	// Load the Pattern
    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_PATTERN_CHUNK:
			{
				DMUS_IO_PATTERN dmusPatternIO;

				ZeroMemory( &dmusPatternIO, sizeof( DMUS_IO_PATTERN ) );

				// Initialize DX8 fields not in DX7's DMUS_IO_PATTERN
				dmusPatternIO.bDestGrooveBottom = 1;
				dmusPatternIO.bDestGrooveTop = 100;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_PATTERN ) );
				hr = pIStream->Read( &dmusPatternIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_bGrooveBottom = dmusPatternIO.bGrooveBottom;
				m_bGrooveTop = dmusPatternIO.bGrooveTop;

				m_bDestGrooveBottom = dmusPatternIO.bDestGrooveBottom;
				m_bDestGrooveTop = dmusPatternIO.bDestGrooveTop;

				m_dwFlags = dmusPatternIO.dwFlags;

				wOrigEmbellishment = m_wEmbellishment;
				wNewEmbellishment = dmusPatternIO.wEmbellishment;
				m_wEmbellishment = dmusPatternIO.wEmbellishment;

				m_TimeSignature.m_bBeatsPerMeasure = dmusPatternIO.timeSig.bBeatsPerMeasure;
				m_TimeSignature.m_bBeat = dmusPatternIO.timeSig.bBeat;
				m_TimeSignature.m_wGridsPerBeat = dmusPatternIO.timeSig.wGridsPerBeat;
				
				m_wNbrMeasures = dmusPatternIO.wNbrMeasures;
				DWORD dwOrigLength = m_dwLength;
				RecalcLength();
				if( (m_fInUndo || m_fInRedo)
				&&  m_dwLength != dwOrigLength )
				{
					fUndoChangedLength = TRUE;
				}
				break;
			}

			case DMUS_FOURCC_PATTERN_UI_CHUNK:
			{
				ioDMPatternUI8 iDMPatternUI;
				ZeroMemory( &iDMPatternUI, sizeof( ioDMPatternUI8 ) );
				iDMPatternUI.m_dblZoom = 0.125;

				dwSize = min( ck.cksize, sizeof( ioDMPatternUI8 ) );
				hr = pIStream->Read( &iDMPatternUI, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				if( dwSize > sizeof( WORD ) )
				{
					m_dwDefaultKeyPattern = iDMPatternUI.m_dwDefaultKeyPattern;
					m_dwDefaultChordPattern = iDMPatternUI.m_dwDefaultChordPattern;
					m_bDefaultKeyRoot = iDMPatternUI.m_bDefaultKeyRoot;
					m_bDefaultChordRoot = iDMPatternUI.m_bDefaultChordRoot;
					if(dwSize >= sizeof(ioDMPatternUI))
					{
						/* not needed: info is in ioDMPatternUI8
						hr = pIStream->Read(&m_fChordFlatsNotSharps, sizeof(BOOL), &dwByteCount);
						if(FAILED(hr) || dwByteCount != sizeof(BOOL))
						{
							hr = E_FAIL;
							goto ON_ERROR;
						}
						hr = pIStream->Read(&m_fKeyFlatsNotSharps, sizeof(BOOL), &dwByteCount);
						if(FAILED(hr) || dwByteCount != sizeof(BOOL))
						{
							hr = E_FAIL;
							goto ON_ERROR;
						}
						*/
						m_fKeyFlatsNotSharps = iDMPatternUI.m_fDefaultKeyFlatsNotSharps;
						m_fChordFlatsNotSharps = iDMPatternUI.m_fDefaultChordFlatsNotSharps;

						if( dwSize >= sizeof( ioDMPatternUI8 ) )
						{
							// Copy Timeline settings
							if( m_dblZoom == 0.0 )
							{
								m_dblZoom = iDMPatternUI.m_dblZoom;
							}
							m_dblHorizontalScroll = iDMPatternUI.m_dblHorizontalScroll;
							m_lVerticalScroll = iDMPatternUI.m_lVerticalScroll;
							m_tlSnapTo = (DMUSPROD_TIMELINE_SNAP_TO)iDMPatternUI.m_dwSnapTo;
							if( m_lFunctionbarWidth == 0 )
							{
								m_lFunctionbarWidth = iDMPatternUI.m_lFunctionbarWidth;
							}
						}
					}
					else
					{
						m_fKeyFlatsNotSharps = FALSE;
						m_fChordFlatsNotSharps = FALSE;
					}
				}
				break;
			}

			case DMUS_FOURCC_RHYTHM_CHUNK:
				if( m_pRhythmMap )
				{
					delete [] m_pRhythmMap;
					m_pRhythmMap = NULL;
				}
				m_pRhythmMap = new DWORD[m_wNbrMeasures];
				if( m_pRhythmMap == NULL )
				{
					hr = E_OUTOFMEMORY;
					goto ON_ERROR;
				}
				for( i = 0 ;  i < m_wNbrMeasures ;  i++ )
				{
					hr = pIStream->Read( &m_pRhythmMap[i], sizeof(DWORD), &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != sizeof(DWORD) )
					{
						hr = E_FAIL;
						goto ON_ERROR;
					}
				}
				break;

			case DMUS_FOURCC_MOTIFSETTINGS_CHUNK:
			{
				DMUS_IO_MOTIFSETTINGS dmusMotifSettingsIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_MOTIFSETTINGS ) );
				hr = pIStream->Read( &dmusMotifSettingsIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwRepeats = dmusMotifSettingsIO.dwRepeats;
				m_mtPlayStart = dmusMotifSettingsIO.mtPlayStart;
				m_mtLoopStart = dmusMotifSettingsIO.mtLoopStart;
				m_mtLoopEnd = dmusMotifSettingsIO.mtLoopEnd;
				m_dwResolution = dmusMotifSettingsIO.dwResolution;
				break;
			}

			case FOURCC_RIFF:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_BAND_FORM: 
					{
						IDMUSProdRIFFExt* pIRIFFExt;
						IDMUSProdNode* pINode;

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
						if( m_pIBandNode )
						{
							InsertChildNode( pINode);
							pINode->Release();
						}
						else
						{
							m_pIBandNode = pINode;
						}
						break;
					}
				}
				break;

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case RIFFINFO_INAM:
								case DMUS_FOURCC_UNAM_CHUNK:
								{
									CString strOrigName = m_strName;

									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strName );

									if( fSinglePattern
									&&  wOrigEmbellishment != -1 )
									{
										// Done so that name won't be needlessly changed
										// when pasting Pattern as Motif or pasting Motif as Pattern
										m_wEmbellishment = wOrigEmbellishment;
									}
									// Make sure Pattern name is unique
									m_pStyle->GetUniquePatternName( this );
									if( fSinglePattern
									&&  wNewEmbellishment != -1 )
									{
										m_wEmbellishment = wNewEmbellishment;
									}

									if( strOrigName.CompareNoCase( m_strName ) != 0 )
									{
										if( m_pIDocRootNode )
										{
											// Node is in Project Tree so we need to sync changes
											theApp.m_pStyleComponent->m_pIFramework->RefreshNode( this );

											// Notify transport that name has changed
											BSTR bstrTransportName;
											if( SUCCEEDED ( GetName( &bstrTransportName ) ) )
											{
												theApp.m_pStyleComponent->m_pIConductor->SetTransportName( this, bstrTransportName );
												if( m_wEmbellishment & EMB_MOTIF )
												{
													if( SUCCEEDED ( GetName( &bstrTransportName ) ) )
													{
														theApp.m_pStyleComponent->m_pIConductor->SetSecondaryTransportName( this, bstrTransportName );
													}
												}
											}

											// Notify connected nodes that Motif name has changed
											if( m_wEmbellishment & EMB_MOTIF )
											{
												DMUSProdMotifData	mtfData;
												WCHAR				wstrMotifName[MAX_PATH];
												WCHAR				wstrOldMotifName[MAX_PATH];
											
												MultiByteToWideChar( CP_ACP, 0, m_strName, -1, wstrMotifName, MAX_PATH );
												mtfData.pwszMotifName = wstrMotifName;

												MultiByteToWideChar( CP_ACP, 0, strOrigName, -1, wstrOldMotifName, MAX_PATH );
												mtfData.pwszOldMotifName = wstrOldMotifName;
												
												theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( m_pStyle, STYLE_MotifNameChange, &mtfData );
											}
										}
									}
									break;
								}
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case DMUS_FOURCC_PATTERN_DESIGN:
						dwSize = ck.cksize;
						dwSize -= 4;
						if( m_pPatternDesignData )
						{
							GlobalFree( m_pPatternDesignData );
							m_pPatternDesignData = NULL;
						}
						m_pPatternDesignData = (BYTE *)GlobalAlloc( GPTR, dwSize );
						if( m_pPatternDesignData == NULL )
						{
							hr = E_OUTOFMEMORY;
							goto ON_ERROR;
						}
						m_dwPatternDesignDataSize = dwSize;
						hr = pIStream->Read( m_pPatternDesignData,
											 m_dwPatternDesignDataSize,
											 &dwByteCount );
						if( FAILED( hr )
						||  dwByteCount != m_dwPatternDesignDataSize )
						{
							hr = E_FAIL;
							GlobalFree( m_pPatternDesignData );
							m_pPatternDesignData = NULL;
							goto ON_ERROR;
						}
						break;

					case DMUS_FOURCC_PART_LIST:		// only in GUID_SinglePattern format
						pPart = m_pStyle->AllocPart();
						if( pPart == NULL )
						{
							hr = E_OUTOFMEMORY ;
			                goto ON_ERROR;
						}
						hr = pPart->DM_LoadPart( pIRiffStream, &ck );
						if( FAILED( hr ) )
						{
							m_pStyle->DeletePart( pPart );
			                goto ON_ERROR;
						}
						if( hr == S_FALSE )
						{
							// Bypass this Part because Style already contains a Part
							// whose GUID matches pPart->m_guidPartID
							m_pStyle->DeletePart( pPart );
						}
						break;

					case DMUS_FOURCC_PARTREF_LIST:
						pPartRef = AllocPartRef();
						if( pPartRef == NULL )
						{
							hr = E_OUTOFMEMORY ;
			                goto ON_ERROR;
						}
						hr = pPartRef->DM_LoadPartRef( pIRiffStream, &ck );
						if( FAILED( hr ) )
						{
							DeletePartRef( pPartRef );
			                goto ON_ERROR;
						}
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
	    dwPos = StreamTell( pIStream );
    }

	// If undo changed length of Pattern we need to change length of all Parts
//	if( fUndoChangedLength )
//	{
//		CDirectMusicPartRef* pPartRef;
//
//		POSITION pos = m_lstPartRefs.GetHeadPosition();
//		while( pos )
//		{
//			pPartRef = m_lstPartRefs.GetNext( pos );
//
//			ASSERT( pPartRef->m_pDMPart != NULL);
//			ASSERT( pPartRef->m_pDMPart->m_dwUseCount >= 1 );
//
//			pPartRef->m_pDMPart->SetNbrMeasures( m_wNbrMeasures );
//		}
//	}

	// If we got here, we should have succeeded
	ASSERT( SUCCEEDED(hr) );

	if( lstOrigPartRefs.GetCount() != m_lstPartRefs.GetCount() )
	{
		fNeedToRequeuePatternPlayback = TRUE;
	}

	// Delete original PartRef list
	while( !lstOrigPartRefs.IsEmpty() )
	{
		pPartRef = lstOrigPartRefs.RemoveHead();

		// Transfer m_pVarChoicesNode pointer to new PartRef and update any hard-linked parts
		if( pPartRef
		&&  pPartRef->m_pDMPart
		&& (pPartRef->m_pDMPart->m_pVarChoicesNode || pPartRef->m_fHardLink) )
		{
			CDirectMusicPartRef* pNewPartRef;

			pNewPartRef = FindPartRefByPChannelAndGUID( pPartRef->m_dwPChannel, pPartRef->m_guidOldPartID );
			if( pNewPartRef == NULL )
			{
				pNewPartRef = FindPartRefByPChannelAndGUID( pPartRef->m_dwPChannel, pPartRef->m_pDMPart->m_guidPartID );
			}

			// Ensure we have a valid pNewPartRef pointer, and that the part actually needs to be changed
			if( pNewPartRef && pNewPartRef->m_pDMPart && (pNewPartRef->m_pDMPart != pPartRef->m_pDMPart) )
			{
				ASSERT( pNewPartRef->m_pDMPart != NULL );

				// Update all hard linked part
				if( pPartRef->m_fHardLink && pNewPartRef->m_fHardLink )
				{
					CString strText;
					if( m_fInUndo )
					{
						strText.LoadString( IDS_UNDO );
					}
					else if( m_fInRedo )
					{
						strText.LoadString( IDS_REDO );
					}

					if( !strText.IsEmpty() )
					{
						// Remove all '&' from the string
						int nIndex = 0;
						while( nIndex >= 0 )
						{
							nIndex = strText.Find(_T("&"),nIndex);
							if( nIndex >= 0 )
							{
								strText.Delete(nIndex);
								nIndex = max( nIndex - 1, 0 );
							}
						}
					}

					TCHAR achText[256];
					BOOL fEnable = FALSE;

					ASSERT( m_pUndoMgr != NULL );

					if( m_pUndoMgr )
					{
						if( m_fInRedo )
						{
							if( m_pUndoMgr->GetRedo(achText, 256) )
							{
								if( *achText )
								{
									fEnable = TRUE;
									strText += " ";
									strText += achText;
									// Should now say 'Redo Insert Note'.
								}
							}
						}
						else //if( m_fInUndo || !m_fInUndo )
						{
							if( m_pUndoMgr->GetUndo(achText, 256) )
							{
								if( *achText )
								{
									fEnable = TRUE;
									if( m_fInUndo )
									{
										strText += " ";
										strText += achText;
										// Should now say 'Undo Insert Note'.
									}
									else
									{
										strText = achText;
										// Should now say 'Insert Note'
									}
								}
							}
						}
					}
					m_pStyle->UpdateMIDIMgrsAndChangePartPtrs( pPartRef->m_pDMPart, pNewPartRef, fEnable ? ((LPCTSTR)strText) : NULL );
				}

				// Update the variation choices editor, if open
				if( pPartRef->m_pDMPart->m_pVarChoicesNode )
				{
					pNewPartRef->m_pDMPart->m_pVarChoicesNode = pPartRef->m_pDMPart->m_pVarChoicesNode;
					// We re-initialize the variation choices editor below
					// We set m_pVarChoicesPartRef below

					// Clear the old part's pointers
					if( pNewPartRef->m_pDMPart != pPartRef->m_pDMPart )
					{
						pPartRef->m_pDMPart->m_pVarChoicesNode = NULL;
						pPartRef->m_pDMPart->m_pVarChoicesPartRef = NULL;
					}
				}
			}
			// If we couldn't find the new part ref, close the editor
			else if( !pNewPartRef )
			{
				if( pPartRef->m_pDMPart->m_pVarChoicesNode )
				{
					// PChannel must have just changed so close Variation Choices Editor
					HWND hWndEditor;
					pPartRef->m_pDMPart->m_pVarChoicesNode->GetEditorWindow( &hWndEditor );
					if( hWndEditor )
					{
						theApp.m_pStyleComponent->m_pIFramework->CloseEditor( pPartRef->m_pDMPart->m_pVarChoicesNode );
					}
					RELEASE( pPartRef->m_pDMPart->m_pVarChoicesNode );
					pPartRef->m_pDMPart->m_pVarChoicesPartRef = NULL;
				}
			}

			if( pNewPartRef
			&&	pNewPartRef->m_pDMPart
			&&	pNewPartRef->m_pDMPart->m_pVarChoicesNode )
			{
				// Re-initialize the variation choices editor
				pNewPartRef->m_pDMPart->m_pVarChoicesPartRef = pNewPartRef;
				InitializeVarChoicesEditor( pNewPartRef );
			}
		}

		DeletePartRef( pPartRef );
	}

	if( m_fInUndo || m_fInRedo )	
	{
		// Mark all 'linked' PartRefs that point to a part with m_dwHardLinkCount of one as 'unlinked'
		m_pStyle->UpdateLinkFlags();
	}

	// If node is in Project Tree we need to sync changes
	if( m_pIDocRootNode )
	{
		// Sync changes with Style editor
		if( m_pStyle->m_pStyleCtrl )
		{
			m_pStyle->m_pStyleCtrl->PostMessage( WM_SYNC_STYLEEDITOR,
					m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS, 0 );
		}

		// Sync changes with MIDI strip manager
		if( sg_fPatternFromMidiMgr == FALSE )
		{
			m_fIgnoreTimelineSync = true;
			SyncPatternWithMidiStripMgr();
			m_fIgnoreTimelineSync = false;

			// Sync changes with property sheet
			IDMUSProdPropSheet* pIPropSheet;
			if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
			{
				pIPropSheet->RefreshTitleByObject( this );
				pIPropSheet->RefreshActivePageByObject( this );
				RELEASE( pIPropSheet );
			}
		}

		// Sync changes with DirectMusic objects
		if( !m_fDontUpdateDirectMusicDuringLoad )
		{
			SyncPatternWithDirectMusic();

			if( fNeedToRequeuePatternPlayback
			&&	m_pIDMSegmentState
			&& (theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( this ) == S_OK) )
			{
				REFERENCE_TIME rtQueue;
				DWORD dwPrepareTime;
				MUSIC_TIME mtNow, mtStartPoint, mtStartTime;
				if( SUCCEEDED( m_pIDMSegmentState->GetStartPoint( &mtStartPoint ) )
				&&	SUCCEEDED( m_pIDMSegmentState->GetStartTime( &mtStartTime ) )
				&&	SUCCEEDED( theApp.m_pStyleComponent->m_pIDMPerformance->GetQueueTime( &rtQueue ) )
				&&	SUCCEEDED( theApp.m_pStyleComponent->m_pIDMPerformance->GetPrepareTime( &dwPrepareTime ) )
				&&	SUCCEEDED( theApp.m_pStyleComponent->m_pIDMPerformance->ReferenceToMusicTime( rtQueue + dwPrepareTime * 10000, &mtNow ) ) )
				{
					// Change mtNow into an offset
					MUSIC_TIME mtOffset = mtNow - mtStartTime + mtStartPoint;
					if( mtNow >=0 )
					{
						mtOffset = mtOffset % m_dwLength;

						// No need to stop the music - PlaySegment will just interrupt the currently playing segment
						//theApp.m_pStyleComponent->m_pIDMPerformance->Stop( m_pIDMSegment, m_pIDMSegmentState, -5 + mtNow, 0 );
						//theApp.m_pStyleComponent->m_pIDMPerformance->Stop( NULL, m_pIDMTransitionSegmentState, -5 + mtNow, 0 );

						// Release the segment states
						RELEASE( m_pIDMSegmentState );
						RELEASE( m_pIDMTransitionSegmentState );
						RELEASE( m_pIDMTransitionSegment );

						m_pIDMSegment->SetStartPoint( mtOffset );
						theApp.m_pStyleComponent->m_pIDMPerformance->PlaySegment( m_pIDMSegment, 0, mtNow, &m_pIDMSegmentState );
					}
				}
			}
		}
	}

ON_ERROR:
	if( FAILED(hr) )
	{
		// Put back original PartRef list
		while( !lstOrigPartRefs.IsEmpty() )
		{
			pPartRef = lstOrigPartRefs.RemoveHead();
			m_lstPartRefs.AddTail( pPartRef );
		}
	}

    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdTimelineCallback implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdTimelineCallback::OnDataChanged

HRESULT CDirectMusicPattern::OnDataChanged( LPUNKNOWN punkStripMgr )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );
	ASSERT( m_pPatternCtrl != NULL );
	ASSERT( m_pPatternCtrl->m_pPatternDlg != NULL );
	ASSERT( m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr != NULL );
	ASSERT( m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr != NULL );

	ASSERT(m_pUndoMgr != NULL);

	if(m_pUndoMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	if( punkStripMgr == NULL )
	{
		return E_POINTER;
	}

	if( m_fInOpenVarChoicesEditor )
	{
		return S_FALSE;
	}

	HRESULT hr = E_FAIL;

	IDMUSProdNode *pIVarChoicesNode = NULL;

	if( punkStripMgr == m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr )
	{
		BOOL fUpdateDirectMusic = TRUE;
		IMIDIMgr* pIMIDIMgr;
		if( SUCCEEDED ( punkStripMgr->QueryInterface( IID_IMIDIMgr, (void**) &pIMIDIMgr) ) )
		{
			BSTR bstr;

			if( SUCCEEDED( pIMIDIMgr->GetUndoText( &fUpdateDirectMusic, &bstr) ) )
			{
				if( bstr != NULL )
				{
					BOOL fFreezeUndo = FALSE;

					if( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
					{
						VARIANT var;
						if( SUCCEEDED( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
						{
							fFreezeUndo = V_BOOL(&var);
						}
					}
					// Only save an undo state here if TP_FREEZE_UNDO is not set and we are not
					// in a group edit operation (See CPatternCtl::SendEditToTimeline)
					if( !fFreezeUndo && !m_fInGroupEdit )
					{
						CString strEdit;
						strEdit = bstr;
						m_pUndoMgr->SaveState( this, (char*)(LPCTSTR)strEdit );
					}
					SysFreeString( bstr );
				}
			}
			RELEASE( pIMIDIMgr );
		}

		// Check to see if any PChannels changed
		IDMUSProdStripMgr* pIStripMgr;
		if( SUCCEEDED ( punkStripMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**) &pIStripMgr) ) )
		{
			BOOL fPChannelChange;

			if( SUCCEEDED ( pIStripMgr->GetParam(  STYLE_PChannelChange, 0, NULL, &fPChannelChange ) ) )
			{
				if( fPChannelChange )
				{
					m_pStyle->m_fPChannelChange = TRUE;

					fPChannelChange = FALSE;
					pIStripMgr->SetParam( STYLE_PChannelChange, 0, &fPChannelChange );
				}
			}

			RELEASE( pIStripMgr );
		}

		m_fDontUpdateDirectMusicDuringLoad = !fUpdateDirectMusic;
		if( SUCCEEDED( GetPatternFromMIDIStripMgr() ) )
		{
			hr = S_OK;
		}
		m_fDontUpdateDirectMusicDuringLoad = FALSE; 
	}
	// Check if this is a change from the variation choices editor
	else if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IDMUSProdNode, (void **)&pIVarChoicesNode ) ) )
	{
		// Find the part that has this var choices pointer
		POSITION pos = m_lstPartRefs.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each part ref
			CDirectMusicPartRef* pPartRef = m_lstPartRefs.GetNext( pos );

			// Check if we found the editor
			if( pPartRef->m_pDMPart->m_pVarChoicesNode == pIVarChoicesNode )
			{
				// Found the editor - make sure we're using the correct partref pointer
				pPartRef = pPartRef->m_pDMPart->m_pVarChoicesPartRef;

				// QI the variation choices node for its IPersistStream interface
				IPersistStream *pIPersistStream;
				if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
				{
					// QI the variation choices node for its IVarChoices interface
					IVarChoices *pIVarChoices;
					if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IVarChoices, (void **)&pIVarChoices ) ) )
					{
						// Try and get undo text to display
						BSTR bstrUndoText;
						if( SUCCEEDED( pIVarChoices->GetUndoText( &bstrUndoText ) ) )
						{
							CString strUndoText = bstrUndoText;
							if( !strUndoText.IsEmpty() )
							{
								m_pUndoMgr->SaveState( this, strUndoText.GetBuffer( 1 ) );
								strUndoText.ReleaseBuffer();
							}
							::SysFreeString( bstrUndoText );
						}

						// Create a new part, if needed
						PreChangePartRef( pPartRef );

						pPartRef->m_pDMPart->UpdateFromVarChoices( theApp.m_pStyleComponent->m_pIFramework, pIPersistStream );

						m_pStyle->SyncMidiStripMgrsThatUsePart( pPartRef->m_pDMPart );
						SyncPatternWithDirectMusic();

						pIVarChoices->Release();
					}
					pIPersistStream->Release();
				}
				break;
			}
		}

		pIVarChoicesNode->Release();
	}
	else
	{
		// Save an undo state if we're about to do a multiple-strip cut/delete/paste
		BOOL fFreezeUndo = FALSE;

		if( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
		{
			VARIANT var;
			if( SUCCEEDED( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_FREEZE_UNDO, &var ) ) )
			{
				fFreezeUndo = V_BOOL(&var);
			}
		}

		if( fFreezeUndo == FALSE )
		{
			// BUGBUG: We should be smart here and only save the strip that changed to the Undo Manager.
			m_fInUndo = TRUE;

			// If the strip supports the IPersist interface, ignore it - it must be the chord strip
			IPersist *pIPersist;
			if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IPersist, (void**)&pIPersist ) ) )
			{
				pIPersist->Release();
			}
			else
			{
				// This must be for a multi-strip cut/delete/paste, since the stripmgr can't save itself.
				// Look in CTimelineCtl::Delete, etc. methods

				// Ask the StripMgr for its undo text
				IDMUSProdStripMgr *pStripMgr;
				if( SUCCEEDED( punkStripMgr->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgr ) ) )
				{
					BSTR bstrUndoText = NULL;
					if( SUCCEEDED( pStripMgr->GetParam( GUID_Segment_Undo_BSTR, 0, NULL, &bstrUndoText ) ) )
					{
						if( bstrUndoText )
						{
							CString strUndoText = bstrUndoText;
							::SysFreeString( bstrUndoText );
							m_pUndoMgr->SaveState( this, (char*)(LPCTSTR)strUndoText );
						}
					}
					pStripMgr->Release();
				}
			}
		}
	}

	// Mark all 'linked' PartRefs that point to a part with m_dwUseCount of one as 'unlinked'
	m_pStyle->UpdateLinkFlags();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::GetPatternFromMIDIStripMgr()

HRESULT CDirectMusicPattern::GetPatternFromMIDIStripMgr()
{
	ASSERT( m_pPatternCtrl != NULL );
	ASSERT( m_pPatternCtrl->m_pPatternDlg != NULL );
	ASSERT( m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr != NULL );

	// Synchronize our Timeline settings (for when they're set in Load())
	SyncTimelineSettings();

	IDMUSProdStripMgr* pIMIDIStripMgr = m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr; 

	IPersistStream	*pIPS;
	IStream* pIStream;

	// Get Pattern from the MIDI Manager
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_SinglePattern, &pIStream) ) )
	{
		if( SUCCEEDED( pIMIDIStripMgr->QueryInterface(IID_IPersistStream, (void**)&pIPS) ) )
		{
			if( SUCCEEDED( pIPS->Save( pIStream, FALSE) ) )
			{
				// Seek to beginning of stream
				LARGE_INTEGER liTemp;
				liTemp.QuadPart = 0;
				pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL ); 

				sg_fPatternFromMidiMgr = TRUE;
				if( SUCCEEDED( Load( pIStream ) ) )
				{
					SetModified( TRUE );
					hr = S_OK;
				}
				sg_fPatternFromMidiMgr = FALSE;
			}

			RELEASE( pIPS );
		}

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::OpenVarChoicesEditor()

HRESULT CDirectMusicPattern::OpenVarChoicesEditor( CDirectMusicPartRef* pPartRef )
{
	m_fInOpenVarChoicesEditor = TRUE;
	ASSERT( pPartRef != NULL );
	ASSERT( pPartRef->m_pDMPart != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	// TODO: Check if this editor is still valid
	if( pPartRef->m_pDMPart->m_pVarChoicesNode )
	{
		HWND hWndEditor;
		pPartRef->m_pDMPart->m_pVarChoicesNode->GetEditorWindow( &hWndEditor );
		if( hWndEditor )
		{
			theApp.m_pStyleComponent->m_pIFramework->CloseEditor( pPartRef->m_pDMPart->m_pVarChoicesNode );
		}
		RELEASE( pPartRef->m_pDMPart->m_pVarChoicesNode );
		pPartRef->m_pDMPart->m_pVarChoicesPartRef = NULL;
	}

	if( pPartRef->m_pDMPart->m_pVarChoicesNode == NULL )
	{
		pPartRef->m_pDMPart->m_pVarChoicesNode = new CVarChoices();
		
		if( pPartRef->m_pDMPart->m_pVarChoicesNode == NULL )
		{
			m_fInOpenVarChoicesEditor = FALSE;
			return E_OUTOFMEMORY;
		}
		pPartRef->m_pDMPart->m_pVarChoicesPartRef = pPartRef;
	}

	// Set the dialog's title and callback and data
	if( pPartRef->m_pDMPart->m_pVarChoicesNode )
	{
		InitializeVarChoicesEditor( pPartRef );
	}

	HRESULT hr = theApp.m_pStyleComponent->m_pIFramework->OpenEditor( pPartRef->m_pDMPart->m_pVarChoicesNode );

	if( SUCCEEDED ( hr ) )
	{
		// Set WINDOWPLACEMENT
		// TODO: ?
		/*
		if( pPartRef->m_pDMPart->m_pVarChoicesNode->m_wp.length )
		{
			if( ::IsWindow( pPartRef->m_pDMPart->m_pVarChoicesNode->m_hWndEditor ) )
			{
				::SetWindowPlacement( pPartRef->m_pDMPart->m_pVarChoicesNode->m_hWndEditor,
									 &pPartRef->m_pDMPart->m_pVarChoicesNode->m_wp );
			}
		}
		*/
	}

	m_fInOpenVarChoicesEditor = FALSE;
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::InitializeVarChoicesEditor()

HRESULT CDirectMusicPattern::InitializeVarChoicesEditor( CDirectMusicPartRef* pPartRef )
{
	ASSERT( pPartRef != NULL );
	ASSERT( pPartRef->m_pDMPart != NULL );
	ASSERT( pPartRef->m_pDMPart->m_pVarChoicesNode != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );


	// Set the dialog's title and callback and data
	CString strTitle;

	// "Style name - "
	if( m_pStyle )
	{
		strTitle = m_pStyle->m_strName;
		strTitle += _T(" - " );
	}

	// "Pattern name "
	strTitle += m_strName;
	strTitle += _T(" " );

	// "(Track name)"
	CString strTrack;
	TCHAR achTemp[MID_BUFFER];

	strTrack.LoadString( IDS_TRACK );
	sprintf( achTemp, "%s %d", strTrack, pPartRef->m_dwPChannel + 1 );
	strTrack = achTemp;

	strTitle += _T("(" );
	strTitle += strTrack;
	strTitle += _T(")" );

	IVarChoices *pIVarChoices;
	if( SUCCEEDED( pPartRef->m_pDMPart->m_pVarChoicesNode->QueryInterface( IID_IVarChoices, (void **)&pIVarChoices ) ) )
	{
		pIVarChoices->SetVarChoicesTitle( strTitle.AllocSysString() );

		pIVarChoices->SetDataChangedCallback( (IDMUSProdTimelineCallback *)this );

		pIVarChoices->Release();
	}

	pPartRef->m_pDMPart->m_pVarChoicesNode->SetDocRootNode( m_pStyle );


	IPersistStream *pIPersistStream;
	if( SUCCEEDED( pPartRef->m_pDMPart->m_pVarChoicesNode->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream ) ) )
	{
		IStream *pStream = CreateStreamForVarChoices( theApp.m_pStyleComponent->m_pIFramework, pPartRef->m_pDMPart->m_dwVariationChoices );
		if( pStream )
		{
			StreamSeek( pStream, 0, STREAM_SEEK_SET );
			pIPersistStream->Load( pStream );
			pStream->Release();
		}
		pIPersistStream->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdPropPageObject::GetData

HRESULT CDirectMusicPattern::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (ppData == NULL)
	|| (*ppData == NULL) )
	{
		return E_INVALIDARG;
	}

	DWORD *pdwIndex = static_cast<DWORD *>(*ppData);
	switch( *pdwIndex )
	{
		// Asking for Motif pointer
		case 0:
		{
			PPGMotif *pPPGMotif = reinterpret_cast<PPGMotif *>(*ppData);
			pPPGMotif->pMotif = this;
			break;
		}

		// Asking for Pattern pointer
		case 1:
		{
			PPGPattern *pPPGPattern = reinterpret_cast<PPGPattern *>(*ppData);
			pPPGPattern->pPattern = this;
			break;
		}

		// Boundary tab
		case 2:
		{
			PPGTabBoundaryFlags *pPPGTabBoundaryFlags = reinterpret_cast<PPGTabBoundaryFlags *>(*ppData);
			pPPGTabBoundaryFlags->dwPlayFlags = m_dwResolution;
			pPPGTabBoundaryFlags->dwFlagsUI = PROPF_HAVE_VALID_DATA;
			pPPGTabBoundaryFlags->strPrompt = m_strName;
			break;
		}

		default:
			ASSERT( 0 );
			break;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdPropPageObject::SetData

HRESULT CDirectMusicPattern::SetData( void* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate parameter
	if( pData == NULL )
	{
		return E_POINTER;
	}
	
	DWORD *pdwIndex = reinterpret_cast<DWORD *>(pData);
	switch( *pdwIndex )
	{
		// Boundary tab
		case 2:
		{
			PPGTabBoundaryFlags *pPPGTabBoundaryFlags = reinterpret_cast<PPGTabBoundaryFlags *>(pData);

			// m_dwResolution
			if( pPPGTabBoundaryFlags->dwPlayFlags != m_dwResolution )
			{
				SetMotifResolution( pPPGTabBoundaryFlags->dwPlayFlags );
			}
			break;
		}

		default:
			ASSERT( 0 );
			break;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdPropPageObject::OnShowProperties

HRESULT CDirectMusicPattern::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	CDllBasePropPageManager* pPageManager;
	short nActiveTab = 0;

	if( m_wEmbellishment & EMB_MOTIF )
	{
		// Get the Motif page manager
		if( theApp.m_pIPageManager
		&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_MotifPropPageManager ) == S_OK )
		{
			pPageManager = (CDllBasePropPageManager *)theApp.m_pIPageManager;
		}
		else
		{
			pPageManager = new CMotifPropPageManager();
		}
	}
	else
	{
		// Get the Pattern page manager
		if( theApp.m_pIPageManager
		&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_PatternPropPageManager ) == S_OK )
		{
			pPageManager = (CDllBasePropPageManager *)theApp.m_pIPageManager;
		}
		else
		{
			pPageManager = new CPatternPropPageManager();
		}
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Pattern/Motif properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		if( m_wEmbellishment & EMB_MOTIF )
		{
			if( pPageManager == theApp.m_pIPageManager )
			{
				pIPropSheet->GetActivePage( &CMotifPropPageManager::sm_nActiveTab );
			}
			nActiveTab = CMotifPropPageManager::sm_nActiveTab;
		}

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
// CDirectMusicPattern IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CDirectMusicPattern::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdTransport implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::Play

HRESULT CDirectMusicPattern::Play(BOOL fPlayFromStart )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );
	ASSERT( m_pStyle->m_pIDMStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );

	ASSERT( m_pIDMSegment == NULL );
	ASSERT( m_pIDMSegmentState == NULL );
	ASSERT( m_pIDMTransitionSegmentState == NULL );

	RELEASE( m_pIDMTransitionSegment );

	HRESULT hr;
	hr = CreateSegment( FALSE );

	if( SUCCEEDED( hr ) )
	{
		ASSERT( m_pIDMSegment );

		// If play button was pressed, set the start position to the current cursor position
		if( m_pPatternCtrl && m_pPatternCtrl->m_pPatternDlg &&
			 m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
		{
			if( !fPlayFromStart )
			{
				MUSIC_TIME mtTime;
				if( SUCCEEDED(	m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->GetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, &mtTime ) ) )
				{
					m_pIDMSegment->SetStartPoint( mtTime );
				}
			}
			else
			{
				m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->SetMarkerTime( MARKER_CURRENTTIME, TIMETYPE_CLOCKS, 0 );
			}
		}

		// Play the band
		PlayBand();

		// Send Tempo event NOW - the only thing that may be playing are pick-up bars, which shold be
		// at this tempo already anyways
		ASSERT( m_pIDMSegmentState == NULL );
		QueueTempoEvent();

		// Play the segment
		hr = theApp.m_pStyleComponent->m_pIDMPerformance->PlaySegment( m_pIDMSegment, DMUS_SEGF_QUEUE, 0, &m_pIDMSegmentState );

		// Notify the MIDIMgr that the music started
		if( SUCCEEDED( hr ) )
		{
			// If editor is open make sure cursor starts moving
			// Wait for DMUS_NOTIFICATION_SEGSTART
			//StartStopMusic( TRUE );

			// Send a tempo event to establish the tempo (this queues it to play 1ms
			// before the SegmentState's start time)
			// Already set - don't set it again.
			/*
			if( fSendTempoEvent )
			{
				QueueTempoEvent();
			}
			*/

			// Notify the MIDIStripMgr that we've started, so that pick-up notes are recorded correctly.
			if( m_pPatternCtrl
			&&	m_pPatternCtrl->m_pPatternDlg
			&&	m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr )
			{
				IMIDIMgr* pIMIDIMgr;
				if( SUCCEEDED ( m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void**) &pIMIDIMgr) ) )
				{
					pIMIDIMgr->SetSegmentState( m_pIDMSegmentState );
					pIMIDIMgr->Release();
				}
			}
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::Stop

HRESULT CDirectMusicPattern::Stop( BOOL fStopImmediate )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );

	MUSIC_TIME mtNow, mtSegmentStart = 0;
	theApp.m_pStyleComponent->m_pIDMPerformance->GetTime( NULL, &mtNow );

	if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( this ) == S_OK )
	{
		// Only stop the transition if there is one.  Otherwise this causes all music to cease immediately.
		if( m_pIDMTransitionSegmentState )
		{
			m_pIDMTransitionSegmentState->GetStartTime( &mtSegmentStart );
			theApp.m_pStyleComponent->m_pIDMPerformance->Stop( NULL, m_pIDMTransitionSegmentState, 0,
															   fStopImmediate ? 0 : DMUS_SEGF_DEFAULT );
		}
		else if( m_pIDMTransitionSegment )
		{
			theApp.m_pStyleComponent->m_pIDMPerformance->StopEx( m_pIDMTransitionSegment, 0,
															   fStopImmediate ? 0 : DMUS_SEGF_DEFAULT );
		}
		// Stop the pattern
		theApp.m_pStyleComponent->m_pIDMPerformance->Stop( m_pIDMSegment, m_pIDMSegmentState, 0,
														   fStopImmediate ? 0 : DMUS_SEGF_DEFAULT );
		if( m_pIDMSegmentState && !mtSegmentStart )
		{
			m_pIDMSegmentState->GetStartTime( &mtSegmentStart );
		}

		// Update the DirectMusic Style object
		m_pStyle->ForceSyncStyleWithDirectMusic();
	}

	// If editor is open make sure cursor stops moving
	// Wait for DMUS_NOTIFICATION_SEGABORT or DMUS_NOTIFICATION_SEGEND
	//StartStopMusic( FALSE );

	// Release the segment
	m_rpIDMStoppedSegmentState = m_pIDMSegmentState;
	RELEASE( m_pIDMSegment );
	RELEASE( m_pIDMSegmentState );
	RELEASE( m_pIDMTransitionSegmentState );
	RELEASE( m_pIDMTransitionSegment );

	// If we are asked to stop our segment state before it begins playing, return an error code because
	// it probably won't stop whatever is currently playing.
	if( mtNow < mtSegmentStart )
	{
		return E_UNEXPECTED;
	}
	else
	{
		return S_OK;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::Transition

HRESULT CDirectMusicPattern::Transition( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );
	ASSERT( m_pStyle->m_pIDMStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );

	/*
	if( theApp.m_pStyleComponent->m_pIConductor->IsTransportPlaying( this ) == S_OK )
	{
		// We don't support transitioning from ourself to ourself.
		return E_NOTIMPL;
	}
	*/

	ConductorTransitionOptions TransitionOptions;
	if( FAILED( theApp.m_pStyleComponent->m_pIConductor->GetTransitionOptions( &TransitionOptions ) ) )
	{
		return E_NOTIMPL;
	}

	RELEASE( m_pIDMSegment );
	RELEASE( m_pIDMSegmentState );
	RELEASE( m_pIDMTransitionSegmentState );
	RELEASE( m_pIDMTransitionSegment );

	HRESULT hr;
	if( TransitionOptions.dwFlags & TRANS_NO_TRANSITION )
	{
		hr = CreateSegment( TRUE );
		if( SUCCEEDED( hr ) )
		{
			DWORD dwFlags;
			// Set the Boundary settings (bar/beat/grid/immediate/end of segment)
			if( TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT )
			{
				dwFlags = DMUS_SEGF_QUEUE;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_IMMEDIATE )
			{
				dwFlags = 0;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_GRID )
			{
				dwFlags = DMUS_SEGF_GRID;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_BEAT )
			{
				dwFlags = DMUS_SEGF_BEAT;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MEASURE )
			{
				dwFlags = DMUS_SEGF_MEASURE;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_SEGMENTEND )
			{
				dwFlags = DMUS_SEGF_SEGMENTEND;
			}
			else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT )
			{
				dwFlags = DMUS_SEGF_DEFAULT;
			}
			else
			{
				ASSERT( FALSE );
				dwFlags = 0;
			}

			if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN )
			{
				dwFlags |= DMUS_SEGF_ALIGN;

				if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_TICK )
				{
					dwFlags |= DMUS_SEGF_VALID_START_TICK;
				}
				else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_GRID )
				{
					dwFlags |= DMUS_SEGF_VALID_START_GRID;
				}
				else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_BEAT )
				{
					dwFlags |= DMUS_SEGF_VALID_START_BEAT;
				}
				else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_MEASURE )
				{
					dwFlags |= DMUS_SEGF_VALID_START_MEASURE;
				}
			}

			if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MARKER )
			{
				dwFlags |= DMUS_SEGF_MARKER;
			}

			if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_NOINVALIDATE )
			{
				dwFlags |= DMUS_SEGF_NOINVALIDATE;
			}

			if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_AFTERPREPARETIME )
			{
				dwFlags |= DMUS_SEGF_AFTERPREPARETIME;
			}

			if( TransitionOptions.pDMUSProdNodeSegmentTransition )
			{
				IDMUSProdNode *pNode;
				if( SUCCEEDED( TransitionOptions.pDMUSProdNodeSegmentTransition->QueryInterface( IID_IDMUSProdNode, (void **)&pNode ) ) )
				{
					if( SUCCEEDED( pNode->GetObject( CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&m_pIDMTransitionSegment ) ) )
					{
						dwFlags |= DMUS_SEGF_AUTOTRANSITION;
					}
					pNode->Release();
				}
			}

			m_rpIDMStoppedSegmentState = NULL;

			// Play the segment
			hr = theApp.m_pStyleComponent->m_pIDMPerformance->PlaySegmentEx( m_pIDMSegment, 0, m_pIDMTransitionSegment,
				dwFlags, 0, &m_pIDMSegmentState, NULL, NULL );

			if( SUCCEEDED( hr ) )
			{
				// If editor is open make sure cursor starts moving
				// Wait for DMUS_NOTIFICATION_SEGSTART
				//StartStopMusic( TRUE, TRUE );

				// Queue a tempo event to establish the tempo
				QueueTempoEvent();
			}
		}
		return hr;
	}

	IDirectMusicComposer8 *pIDMComposer;
	::CoCreateInstance( CLSID_DirectMusicComposer, NULL, CLSCTX_INPROC, 
					   IID_IDirectMusicComposer8, (void**)&pIDMComposer );
	if( !pIDMComposer )
	{
		return E_NOTIMPL;
	}

	m_rpIDMStoppedSegmentState = NULL;

	hr = CreateSegment( TRUE );
	if( SUCCEEDED( hr ) )
	{
		hr = DoAutoTransition( TransitionOptions, theApp.m_pStyleComponent->m_pIDMPerformance,
						  pIDMComposer, m_pIDMSegment,
						  NULL, &m_pIDMTransitionSegmentState,
						  &m_pIDMSegmentState );

		// Notify the MIDIMgr that the music started
		if( SUCCEEDED( hr ) )
		{
			// If editor is open make sure cursor starts moving
			// Wait for DMUS_NOTIFICATION_SEGSTART
			//StartStopMusic( TRUE, TRUE );

			// Queue a tempo event to establish the tempo
			QueueTempoEvent();
		}
		else
		{
			// Fallback to non-composed transition
			RELEASE( m_pIDMSegment );
			RELEASE( m_pIDMSegmentState );
			RELEASE( m_pIDMTransitionSegmentState );
			ASSERT( m_pIDMSegment == NULL );

			hr = CreateSegment( TRUE );
			if( SUCCEEDED( hr ) )
			{
				DWORD dwFlags;
				// Set the Boundary settings (bar/beat/grid/immediate/end of segment)
				if( TransitionOptions.dwFlags & TRANS_END_OF_SEGMENT )
				{
					dwFlags = DMUS_SEGF_QUEUE;
				}
				else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_IMMEDIATE )
				{
					dwFlags = 0;
				}
				else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_GRID )
				{
					dwFlags = DMUS_SEGF_GRID;
				}
				else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_BEAT )
				{
					dwFlags = DMUS_SEGF_BEAT;
				}
				else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MEASURE )
				{
					dwFlags = DMUS_SEGF_MEASURE;
				}
				else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_SEGMENTEND )
				{
					dwFlags = DMUS_SEGF_SEGMENTEND;
				}
				else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_DEFAULT )
				{
					dwFlags = DMUS_SEGF_DEFAULT;
				}
				else
				{
					ASSERT( FALSE );
					dwFlags = 0;
				}

				if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_ALIGN )
				{
					dwFlags |= DMUS_SEGF_ALIGN;

					if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_TICK )
					{
						dwFlags |= DMUS_SEGF_VALID_START_TICK;
					}
					else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_GRID )
					{
						dwFlags |= DMUS_SEGF_VALID_START_GRID;
					}
					else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_BEAT )
					{
						dwFlags |= DMUS_SEGF_VALID_START_BEAT;
					}
					else if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_VALID_START_MEASURE )
					{
						dwFlags |= DMUS_SEGF_VALID_START_MEASURE;
					}
				}

				if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_MARKER )
				{
					dwFlags |= DMUS_SEGF_MARKER;
				}

				if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_NOINVALIDATE )
				{
					dwFlags |= DMUS_SEGF_NOINVALIDATE;
				}

				if( TransitionOptions.dwBoundaryFlags & DMUS_COMPOSEF_AFTERPREPARETIME )
				{
					dwFlags |= DMUS_SEGF_AFTERPREPARETIME;
				}

				m_rpIDMStoppedSegmentState = NULL;

				// Play the segment
				hr = theApp.m_pStyleComponent->m_pIDMPerformance->PlaySegment( m_pIDMSegment, dwFlags, 0, &m_pIDMSegmentState );

				// Notify the MIDIMgr that the music started
				if( SUCCEEDED( hr ) )
				{
					// If editor is open make sure cursor starts moving
					// Wait for DMUS_NOTIFICATION_SEGSTART
					//StartStopMusic( TRUE, TRUE );

					// Queue a tempo event to establish the tempo
					QueueTempoEvent();
				}
			}
		}
	}

	RELEASE( pIDMComposer );

	if( FAILED( hr ) )
	{
		return E_NOTIMPL;
	}
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::Record

HRESULT CDirectMusicPattern::Record( BOOL fPressed )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Dialog active - tell MIDIMgr that the Record button state changed
	if( m_fDialogActive )
	{
		if( m_pPatternCtrl == NULL || m_pPatternCtrl->m_pPatternDlg == NULL ||
			m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr == NULL )
		{
			m_fRecordPressed = fPressed;
			// We shouldn't have an active dialog if we don't even have a dialog...
			ASSERT(FALSE);
			return S_FALSE;
		}

		HRESULT hr = E_FAIL;

		IMIDIMgr* pIMIDIMgr;
		if( SUCCEEDED ( m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void**) &pIMIDIMgr) ) )
		{
			hr = pIMIDIMgr->OnRecord( fPressed );
			if( SUCCEEDED( hr ) )
			{
				m_fRecordPressed = fPressed;
			}
			pIMIDIMgr->Release();
		}
		return hr;
	}
	// Dialog inactive - just set m_fRecordPressed
	else
	{
		m_fRecordPressed = fPressed;
		return S_OK;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::GetName

HRESULT CDirectMusicPattern::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

	// Style file name
	CString strName;
	BSTR bstrStyleFileName;

	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->GetNodeFileName( m_pStyle, &bstrStyleFileName ) ) )
	{
		// Node is in the Project tree
		strName = bstrStyleFileName;
		::SysFreeString( bstrStyleFileName );
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
		// because the Style is still in the process of being loaded
		strName = m_pStyle->m_strOrigFileName;
		strName += _T(": ");
	}
	
	// Plus "name"
	strName += m_strName;

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::TrackCursor

HRESULT CDirectMusicPattern::TrackCursor( BOOL fTrackCursor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_fTrackTimeCursor = fTrackCursor;

	if( m_pPatternCtrl && m_pPatternCtrl->m_pPatternDlg && m_fPatternIsPlaying )
	{
		m_pPatternCtrl->m_pPatternDlg->EnableTimer( fTrackCursor );
	}

	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdSecondaryTransport implementation

// CDirectMusicPattern IDMUSProdSecondaryTransport::GetSecondaryTransportSegment

HRESULT CDirectMusicPattern::GetSecondaryTransportSegment( IUnknown **ppunkSegment )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate ppunkSegment
	if( ppunkSegment == NULL )
	{
		return E_POINTER;
	}

	/**/
	// Try and create a segment
	ASSERT( m_pStyle != NULL );
	ASSERT( m_pStyle->m_pIDMStyle != NULL );

	IDirectMusicSegment *pSegment = NULL;

	// Get the Wide character name of this motif
	BSTR bstrName = m_strName.AllocSysString();

	HRESULT hr = m_pStyle->m_pIDMStyle->GetMotif( bstrName, &pSegment );

	if( hr == S_OK )
	{
		hr = pSegment->QueryInterface( IID_IUnknown, (void**)ppunkSegment );
	}
	else if ( hr == S_FALSE )
	{
		hr = E_FAIL;
	}

	RELEASE(pSegment);
	SysFreeString( bstrName );
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdSecondaryTransport::GetSecondaryTransportName

HRESULT CDirectMusicPattern::GetSecondaryTransportName( BSTR* pbstrName )
{
	return GetName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IConductorTempo implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetTempo

HRESULT CDirectMusicPattern::SetTempo( double dblTempo )
{
	if( !m_pStyle )
	{
		return E_FAIL;
	}

	m_pStyle->SetTempo( dblTempo, FALSE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::GetTempo

HRESULT CDirectMusicPattern::GetTempo( double *pdblTempo )
{
	if( !pdblTempo )
	{
		return E_POINTER;
	}

	if( !m_pStyle )
	{
		return E_FAIL;
	}

	*pdblTempo = m_pStyle->m_dblTempo;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetTempoModifier

HRESULT CDirectMusicPattern::SetTempoModifier( double dblModifier )
{
	UNREFERENCED_PARAMETER(dblModifier);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::GetTempoModifier

HRESULT CDirectMusicPattern::GetTempoModifier( double *pdblModifier )
{
	UNREFERENCED_PARAMETER(pdblModifier);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::TempoModified

HRESULT CDirectMusicPattern::TempoModified( BOOL fModified )
{
	UNREFERENCED_PARAMETER(fModified);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IDMUSProdNotifyCPt implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::OnNotify

HRESULT CDirectMusicPattern::OnNotify( ConductorNotifyEvent *pConductorNotifyEvent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pConductorNotifyEvent != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );

	switch( pConductorNotifyEvent->m_dwType )
	{
		case DMUS_PMSGT_NOTIFICATION:
		{
			DMUS_NOTIFICATION_PMSG* pNotifyEvent = (DMUS_NOTIFICATION_PMSG *)pConductorNotifyEvent->m_pbData;
			//HRESULT hr;
			//hr = theApp.m_pStyleComponent->m_pIDMPerformance->IsPlaying( NULL, m_pIDMSegmentState);

			// Handle GUID_NOTIFICATION_SEGMENT notifications
			if( ::IsEqualGUID ( pNotifyEvent->guidNotificationType, GUID_NOTIFICATION_SEGMENT ) )
			{
				//TRACE("SegmentState: %d %x\n", pNotifyEvent->dwNotificationOption, pNotifyEvent->punkUser );
				if( (pNotifyEvent->punkUser == m_pIDMSegmentState)
				||	(pNotifyEvent->punkUser == m_rpIDMStoppedSegmentState) )
				{
					switch( pNotifyEvent->dwNotificationOption )
					{
						case DMUS_NOTIFICATION_SEGSTART:
							m_rtCurrentStartTime = pNotifyEvent->rtTime;
						case DMUS_NOTIFICATION_SEGLOOP:
							// Update the playing flag
							StartStopMusic( TRUE );

							// Make sure cursor starts moving
							if( m_pPatternCtrl
							&&  m_pPatternCtrl->m_pPatternDlg )
							{
								::PostMessage( m_pPatternCtrl->m_pPatternDlg->m_hWnd, WM_APP, pNotifyEvent->dwNotificationOption, pNotifyEvent->mtTime );
							}
							break;

						case DMUS_NOTIFICATION_SEGABORT:
						case DMUS_NOTIFICATION_SEGEND:
							//TRACE("SegmentState: %d %x\n", pNotifyEvent->dwNotificationOption, pNotifyEvent->punkUser );
							// Update the playing flag
							StartStopMusic( FALSE );

							// Notify the Conductor we stopped playing
							if( theApp.m_pStyleComponent && theApp.m_pStyleComponent->m_pIConductor )
							{
								theApp.m_pStyleComponent->m_pIConductor->TransportStopped( (IDMUSProdTransport*) this );
							}

							// If editor is open, make sure cursor stops moving
							if( m_pPatternCtrl
							&&  m_pPatternCtrl->m_pPatternDlg )
							{
								IDirectMusicSegmentState* pIDMSegmentState = m_pIDMSegmentState ? m_pIDMSegmentState : m_rpIDMStoppedSegmentState;
								MUSIC_TIME mtStartTime;
								MUSIC_TIME mtOffset = -1;
								MUSIC_TIME mtStartPoint;

								if( SUCCEEDED ( pIDMSegmentState->GetStartTime( &mtStartTime ) ) && 
									SUCCEEDED ( pIDMSegmentState->GetStartPoint( &mtStartPoint ) ) )
								{
									mtOffset = pNotifyEvent->mtTime - mtStartTime + mtStartPoint;
									if( mtOffset >=0 )
									{
										mtOffset = mtOffset % m_dwLength;
									}
								}
								::PostMessage( m_pPatternCtrl->m_pPatternDlg->m_hWnd, WM_APP, pNotifyEvent->dwNotificationOption, mtOffset );
							}

							// Don't need this reference or time anymore
							m_rpIDMStoppedSegmentState = NULL;
							m_rtCurrentStartTime = 0;
							break;
					}
				}
			}
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPatternNodePrivate implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DisplayPartLinkDialog

HRESULT CDirectMusicPattern::DisplayPartLinkDialog( GUID *pGuid, IStream **ppPartStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Validate parameters
	if( !pGuid || !ppPartStream )
	{
		return E_POINTER;
	}

	// Initialize parameters to 0
	*ppPartStream = NULL;
	memset( pGuid, 0, sizeof(GUID) );

	// Display the dialog
	CDialogLinkExisting dlg;
	dlg.m_pDMStyle = m_pStyle;
	if( (dlg.DoModal() == IDOK) && (dlg.m_pDMPartRef != NULL) )
	{
		if( dlg.m_pDMPartRef->m_pPattern != this )
		{
			// Linking to another pattern - get a copy of the part in a stream

			// Get a memory stream
			HRESULT hr;
			hr = theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, ppPartStream );
			if( FAILED ( hr ) )
			{
				return hr;
			}

			// Get a RIFF stream
			IDMUSProdRIFFStream* pIRiffStream = NULL;
			hr = AllocRIFFStream( *ppPartStream, &pIRiffStream );
			if( FAILED ( hr ) )
			{
				RELEASE(*ppPartStream);
				return hr;
			}

			// Save the part to the RIFF stream
			hr = dlg.m_pDMPartRef->m_pDMPart->DM_SavePart( pIRiffStream );

			// Release the RIFF stream
			RELEASE( pIRiffStream );

			// Exit if we failed
			if( FAILED ( hr ) )
			{
				RELEASE(*ppPartStream);
				return hr;
			}

			// Seek back to the beginning
			LARGE_INTEGER li;
			li.QuadPart = 0;
			hr = (*ppPartStream)->Seek( li, STREAM_SEEK_SET, NULL );
			if( FAILED ( hr ) )
			{
				RELEASE(*ppPartStream);
				return hr;
			}
		}

		// Copy the GUID
		memcpy( pGuid, &dlg.m_pDMPartRef->m_pDMPart->m_guidPartID, sizeof(GUID) );

		// Fix 22261: Always mark the linked to PartRef as hardLinked here, rather than
		// letting the MIDIStripMgr try and figure out which PartRef it was linked to.
		//if( dlg.m_pDMPartRef->m_pPattern != this )
		{
			// Save "Undo" state
			//   Don't save an undo state - this is a 'side effect', so we don't care about it
			//dlg.m_pDMPartRef->m_pPattern->m_pUndoMgr->SaveState( dlg.m_pDMPartRef->m_pPattern, theApp.m_hInstance, IDS_UNDO_PATTERN_PART_LINK );

			dlg.m_pDMPartRef->m_fHardLink = TRUE;
			dlg.m_pDMPartRef->m_pDMPart->m_dwHardLinkCount++;

			dlg.m_pDMPartRef->m_pPattern->SetModified( TRUE );
			dlg.m_pDMPartRef->m_pPattern->m_pStyle->SyncStyleEditor( dlg.m_pDMPartRef->m_pPattern->m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
			dlg.m_pDMPartRef->m_pPattern->SyncPatternWithMidiStripMgr();
			dlg.m_pDMPartRef->m_pPattern->SyncPatternWithDirectMusic();
		}

		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DisplayVariationChoicesDlg

HRESULT CDirectMusicPattern::DisplayVariationChoicesDlg( REFGUID guidPart, DWORD dwPChannel )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicPartRef* pPartRef = FindPartRefByPChannelAndGUID( dwPChannel, guidPart );
	m_pPatternCtrl->PostMessage( WM_OPEN_VARCHOICES, 0, (LPARAM)pPartRef );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetAuditionVariations

HRESULT CDirectMusicPattern::SetAuditionVariations( DWORD dwVariations, REFGUID rguidPart, DWORD dwPChannel )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDirectMusicTrack* pIDMTrack;

	HRESULT hr = E_FAIL;

	// Update Pattern Track
	if( !m_pIDMSegment )
	{
		hr = S_FALSE;
	}
	else if( SUCCEEDED ( m_pIDMSegment->GetTrack( CLSID_DirectMusicPatternTrack, 1, 0, &pIDMTrack ) ) )
	{
		IPrivatePatternTrack* pIDMPrivatePatternTrack;
		IPrivatePatternTrack9* pIDMPrivatePatternTrack9;
		if( SUCCEEDED ( pIDMTrack->QueryInterface( IID_IPrivatePatternTrack9, (void **)&pIDMPrivatePatternTrack9 ) ) )
		{
			// If no variations are selected, then pass GUID_AllZeros
			hr = pIDMPrivatePatternTrack9->SetVariationMaskByGUID( m_pIDMSegmentState, dwVariations, dwVariations ? rguidPart : GUID_AllZeros, dwPChannel );

			RELEASE( pIDMPrivatePatternTrack9 );
		}
		else if( SUCCEEDED ( pIDMTrack->QueryInterface( IID_IPrivatePatternTrack, (void **)&pIDMPrivatePatternTrack ) ) )
		{
			hr = pIDMPrivatePatternTrack->SetVariationByGUID( m_pIDMSegmentState, dwVariations, rguidPart, dwPChannel );

			RELEASE( pIDMPrivatePatternTrack );
		}
		
		RELEASE( pIDMTrack );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::GetNumHardLinkRefs

HRESULT CDirectMusicPattern::GetNumHardLinkRefs( REFGUID guidPart, DWORD* pdwReferences )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( !pdwReferences )
	{
		return E_POINTER;
	}

	CDirectMusicPart* pDMPart = m_pStyle->FindPartByGUID( guidPart );

	if( pDMPart )
	{
		*pdwReferences = pDMPart->m_dwHardLinkCount;
		return S_OK;
	}
	else
	{
		*pdwReferences = 0;
		return E_INVALIDARG;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::CanShowPartLinkDialog

HRESULT CDirectMusicPattern::CanShowPartLinkDialog()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle )
	{
		if( !m_pStyle->m_lstStyleParts.IsEmpty() )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::RegisterWithTransport

HRESULT CDirectMusicPattern::RegisterWithTransport( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );

	DWORD dwButtonState = BS_AUTO_UPDATE;
	if( m_pPatternCtrl != NULL && m_pPatternCtrl->m_pPatternDlg != NULL &&
		m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr != NULL )
	{
		dwButtonState |= BS_RECORD_ENABLED;
		if( m_fRecordPressed )
		{
			dwButtonState |= BS_RECORD_CHECKED;
		}
	}

	// If we're a Motif, register with Conductor's Secondary Transport system
	if( m_wEmbellishment & EMB_MOTIF )
	{
		if( FAILED ( theApp.m_pStyleComponent->m_pIConductor->RegisterSecondaryTransport( this ) ) )
		{
			return E_FAIL;
		}
	}

	// Register Pattern with Conductor's Transport and notification system
	if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIConductor->RegisterTransport( this, dwButtonState ) )
	&&  SUCCEEDED ( theApp.m_pStyleComponent->m_pIConductor->RegisterNotify( this, GUID_NOTIFICATION_SEGMENT ) )
	&&  SUCCEEDED ( theApp.m_pStyleComponent->m_pIConductor->RegisterNotify( this, GUID_NOTIFICATION_MEASUREANDBEAT ) ) )
	{
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::UnRegisterWithTransport

HRESULT CDirectMusicPattern::UnRegisterWithTransport( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );

	// Make sure Pattern is not playing
	Stop( TRUE );

	// If we're a motif, unregister the Transport's secondary Play, Stop functionality.
	if( m_wEmbellishment & EMB_MOTIF )
	{
		theApp.m_pStyleComponent->m_pIConductor->UnRegisterSecondaryTransport( this );
	}

	// Unregister the Transport's Play, Stop, Transition functionality.
	theApp.m_pStyleComponent->m_pIConductor->UnRegisterTransport( this );

	// Unregister notification messages
	theApp.m_pStyleComponent->m_pIConductor->UnregisterNotify( this, GUID_NOTIFICATION_SEGMENT );
	theApp.m_pStyleComponent->m_pIConductor->UnregisterNotify( this, GUID_NOTIFICATION_MEASUREANDBEAT );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetNbrMeasures

void CDirectMusicPattern::SetNbrMeasures( WORD wNbrMeasures )
{
	ASSERT( m_pStyle != NULL );

	if( wNbrMeasures != m_wNbrMeasures )
	{
		// Save "Undo" state
		if( m_wEmbellishment & EMB_MOTIF )
		{
			m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_MOTIF_LENGTH );
		}
		else
		{
			m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_PATTERN_LENGTH );
		}

		// Change Pattern rhythm map
		DWORD* pRhythmMap = m_pRhythmMap;
		m_pRhythmMap = new DWORD[wNbrMeasures];
		if( m_pRhythmMap )
		{
			for( int i = 0 ;  i < wNbrMeasures ;  ++i )
			{
				if( i < m_wNbrMeasures )
				{
					m_pRhythmMap[i] = pRhythmMap[i];
				}
				else
				{
					m_pRhythmMap[i] = 0;
				}
			}
		}
		if( pRhythmMap )
		{
			delete [] pRhythmMap;
		}

		// Change Pattern length
		WORD wOrigNbrMeasures = m_wNbrMeasures;
		m_wNbrMeasures = wNbrMeasures; 
		RecalcLength();

		// Change length of all Parts
		CDirectMusicPartRef* pPartRef;

		POSITION pos = m_lstPartRefs.GetHeadPosition();
		while( pos )
		{
			pPartRef = m_lstPartRefs.GetNext( pos );

			ASSERT( pPartRef->m_pDMPart != NULL);

			// Only change when length of part equals original pattern length
			// and time signature of part equals time signature of pattern
			if( pPartRef->m_pDMPart->m_wNbrMeasures == wOrigNbrMeasures 
			&&  pPartRef->m_pDMPart->m_TimeSignature.m_bBeat == m_TimeSignature.m_bBeat 
			&&  pPartRef->m_pDMPart->m_TimeSignature.m_bBeatsPerMeasure == m_TimeSignature.m_bBeatsPerMeasure 
			&&  pPartRef->m_pDMPart->m_TimeSignature.m_wGridsPerBeat == m_TimeSignature.m_wGridsPerBeat )
			{
				if( pPartRef->m_fHardLink == FALSE )
				{
					PreChangePartRef( pPartRef );
					pPartRef->m_pDMPart->SetNbrMeasures( wNbrMeasures );
				}
				else
				{
					DWORD dwLinkCount = pPartRef->m_pDMPart->m_dwHardLinkCount;
					POSITION pos2 = m_lstPartRefs.GetHeadPosition();
					while( pos2 && dwLinkCount )
					{
						CDirectMusicPartRef* pTmpPartRef = m_lstPartRefs.GetNext( pos2 );
						if( pTmpPartRef->m_fHardLink
						&&	pTmpPartRef->m_pDMPart == pPartRef->m_pDMPart )
						{
							dwLinkCount--;
						}
					}

					// If all hard links to this part are in this pattern, change the part's length
					if( dwLinkCount == 0 )
					{
						PreChangePartRef( pPartRef );
						pPartRef->m_pDMPart->SetNbrMeasures( wNbrMeasures );
					}
				}
			}
		}
		
		SetModified( TRUE );
		m_pStyle->SyncStyleEditor( m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();

		if( m_pPatternCtrl
		&&  m_pPatternCtrl->m_pPatternDlg
		&&  m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
		{
			m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->Refresh();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetEmbellishment

void CDirectMusicPattern::SetEmbellishment( WORD wOnEmbellishment, WORD wOffEmbellishment, WORD wCustomId )
{
	ASSERT( m_pStyle != NULL );
	ASSERT( (wCustomId == 0) || ((wCustomId >= MIN_EMB_CUSTOM_ID) && (wCustomId <= MAX_EMB_CUSTOM_ID)) );

	// LOBYTE(m_wEmbellishment) contains bit flags for intro, fill, break, end. When set, HIBYTE must be zero.
	// HIBYTE(m_wEmbellishment) contains custom id number between 100-199.  When set, LOBYTE must be zero.
	
	WORD wOrigEmbellishment = m_wEmbellishment;
	WORD wEmbellishment = m_wEmbellishment;
	wEmbellishment &= ~wOffEmbellishment;	// Turn off bits in LOBYTE
	wEmbellishment |= wOnEmbellishment;		// Turn on bits in LOBYTE
	wEmbellishment &= 0x00FF;				// Set HIBYTE to zero
	wEmbellishment += wCustomId << 8;		// Set HIBYTE to new value

	// Store last custom embellishment ID
	if( HIBYTE(wOrigEmbellishment) )
	{
		// Store original
		m_nLastCustomId = HIBYTE(wOrigEmbellishment);
	}
	if( HIBYTE(wEmbellishment) )
	{
		// Overlay original with current
		m_nLastCustomId = HIBYTE(wEmbellishment);
	}

	if( wEmbellishment != m_wEmbellishment )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_PATTERN_EMBELLISHMENT );

		// Change Embellishment
		m_wEmbellishment = wEmbellishment; 
		
		SetModified( TRUE );
		m_pStyle->SyncStyleEditor( m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
		if( (HIBYTE(wOrigEmbellishment) == 0)  &&  (HIBYTE(m_wEmbellishment) != 0) 
		||  (HIBYTE(wOrigEmbellishment) != 0)  &&  (HIBYTE(m_wEmbellishment) == 0) )
		{
			// Refresh properties when switching to/from a custom embellishment
			IDMUSProdPropSheet* pIPropSheet;
			if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
			{
				pIPropSheet->RefreshActivePageByObject( this );
				RELEASE( pIPropSheet );
			}
		}
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetGrooveRange

void CDirectMusicPattern::SetGrooveRange( BYTE bGrooveBottom, BYTE bGrooveTop )
{
	ASSERT( m_pStyle != NULL );

	if( bGrooveBottom != m_bGrooveBottom
	||  bGrooveTop != m_bGrooveTop )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_PATTERN_GROOVE_RANGE );

		// Change Groove Range
		m_bGrooveBottom = bGrooveBottom; 
		m_bGrooveTop = bGrooveTop; 
		
		SetModified( TRUE );
		m_pStyle->SyncStyleEditor( m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetDestGrooveRange

void CDirectMusicPattern::SetDestGrooveRange( BYTE bDestGrooveBottom, BYTE bDestGrooveTop )
{
	ASSERT( m_pStyle != NULL );

	if( bDestGrooveBottom != m_bDestGrooveBottom
	||  bDestGrooveTop != m_bDestGrooveTop )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_PATTERN_DEST_GROOVE_RANGE );

		// Change Dest Groove Range
		m_bDestGrooveBottom = bDestGrooveBottom; 
		m_bDestGrooveTop = bDestGrooveTop; 
		
		SetModified( TRUE );
		m_pStyle->SyncStyleEditor( m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetRhythmMap

void CDirectMusicPattern::SetRhythmMap( DWORD* pRythmMap  )
{
	ASSERT( m_pStyle != NULL );
	ASSERT( pRythmMap != NULL );

	BOOL fModified = FALSE;

	// Determine if rhythm map has changed
	for( int i = 0 ;  i < m_wNbrMeasures ;  ++i )
	{
		if( pRythmMap[i] != m_pRhythmMap[i] )
		{
			fModified = TRUE;
			break;
		}
	}

	// Change rhythm map
	if( fModified )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_PATTERN_RHYTHM_MAP );

		for( int i = 0 ;  i < m_wNbrMeasures ;  ++i )
		{
			m_pRhythmMap[i] = pRythmMap[i];
		}
		
		SetModified( TRUE );
		m_pStyle->SyncStyleEditor( m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetTimeSignature

void CDirectMusicPattern::SetTimeSignature( DirectMusicTimeSig timeSig, BOOL fSyncWithEngine )
{
	if( timeSig.m_bBeatsPerMeasure != m_TimeSignature.m_bBeatsPerMeasure
	||  timeSig.m_bBeat != m_TimeSignature.m_bBeat
	||  timeSig.m_wGridsPerBeat != m_TimeSignature.m_wGridsPerBeat )
	{
		// Change Pattern time signature
		m_TimeSignature = timeSig;
		
		// Adjust Pattern length
		DWORD dwClocksPerBeat = DM_PPQNx4 / m_TimeSignature.m_bBeat;
		DWORD dwClocksPerMeasure = dwClocksPerBeat * (DWORD)m_TimeSignature.m_bBeatsPerMeasure;
		WORD wNbrMeasures = (WORD)(m_dwLength / dwClocksPerMeasure);
		if( m_dwLength % dwClocksPerMeasure )
		{
			wNbrMeasures++;
		}

		if( wNbrMeasures != m_wNbrMeasures )
		{
			// Change Pattern rhythm map
			DWORD* pRhythmMap = m_pRhythmMap;
			m_pRhythmMap = new DWORD[wNbrMeasures];
			if( m_pRhythmMap )
			{
				for( int i = 0 ;  i < wNbrMeasures ;  ++i )
				{
					if( i < m_wNbrMeasures )
					{
						m_pRhythmMap[i] = pRhythmMap[i];
					}
					else
					{
						m_pRhythmMap[i] = 0;
					}
				}
			}
			if( pRhythmMap )
			{
				delete [] pRhythmMap;
			}

			// Change Pattern length
			m_wNbrMeasures = wNbrMeasures; 
		}
		
		RecalcLength();

		// Refresh Property Page
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			pIPropSheet->RefreshActivePageByObject( this );
			RELEASE( pIPropSheet );
		}

		SetModified( TRUE );
		SyncPatternWithMidiStripMgr();
		if( fSyncWithEngine )
		{
			SyncPatternWithDirectMusic();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetMotifResolution

void CDirectMusicPattern::SetMotifResolution( DWORD dwResolution )
{
	ASSERT( m_pStyle != NULL );

	// Only applies to Motifs
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		return;
	}

	if( dwResolution != m_dwResolution )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_MOTIF_RESOLUTION );

		// Change Motif Resolution
		m_dwResolution = dwResolution; 
		
		SetModified( TRUE );
//		m_pStyle->SyncStyleEditor( SSE_MOTIFS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetMotifRepeats

void CDirectMusicPattern::SetMotifRepeats( DWORD dwRepeats )
{
	ASSERT( m_pStyle != NULL );

	// Only applies to Motifs
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		return;
	}

	if( dwRepeats != m_dwRepeats )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_MOTIF_REPEATS );

		// Change Motif Repeats
		m_dwRepeats = dwRepeats; 
		
		SetModified( TRUE );
//		m_pStyle->SyncStyleEditor( SSE_MOTIFS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetMotifPlayStart

BOOL CDirectMusicPattern::SetMotifPlayStart( MUSIC_TIME mtPlayStart )
{
	ASSERT( m_pStyle != NULL );

	// Only applies to Motifs
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		return FALSE;
	}

	if( mtPlayStart > ((long)m_dwLength - 1) )
	{
		return FALSE;
	}
	mtPlayStart = max( mtPlayStart, 0 );

	if( mtPlayStart != m_mtPlayStart )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_MOTIF_PLAYSTART );

		// Change Motif Play Start
		m_mtPlayStart = mtPlayStart; 
		
		SetModified( TRUE );
//		m_pStyle->SyncStyleEditor( SSE_MOTIFS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetMotifLoopStart

BOOL CDirectMusicPattern::SetMotifLoopStart( MUSIC_TIME mtLoopStart )
{
	ASSERT( m_pStyle != NULL );

	// Only applies to Motifs
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		return FALSE;
	}

	mtLoopStart = min( mtLoopStart, (long)m_dwLength - 1 );
	mtLoopStart = max( mtLoopStart, 0 );

	if( mtLoopStart != m_mtLoopStart )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_MOTIF_LOOPPOINTS );

		// Change Motif Loop Start
		m_mtLoopStart = mtLoopStart; 

		// Adjust related fields
		if( m_mtLoopEnd <= m_mtLoopStart )
		{
			m_mtLoopEnd = m_mtLoopStart + 1;
		}
		
		SetModified( TRUE );
//		m_pStyle->SyncStyleEditor( SSE_MOTIFS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetMotifLoopEnd

BOOL CDirectMusicPattern::SetMotifLoopEnd( MUSIC_TIME mtLoopEnd )
{
	ASSERT( m_pStyle != NULL );

	// Only applies to Motifs
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		return FALSE;
	}

	mtLoopEnd = min( mtLoopEnd, (long)m_dwLength );
	mtLoopEnd = max( mtLoopEnd, 1 );

	if( mtLoopEnd != m_mtLoopEnd )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_MOTIF_LOOPPOINTS );

		// Change Motif Loop End
		m_mtLoopEnd = mtLoopEnd; 

		// Adjust related fields
		if( m_mtLoopStart >= m_mtLoopEnd )
		{
			m_mtLoopStart = m_mtLoopEnd - 1;
		}
		
		SetModified( TRUE );
//		m_pStyle->SyncStyleEditor( SSE_MOTIFS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::ResetMotifLoopPoints

void CDirectMusicPattern::ResetMotifLoopPoints( void )
{
	ASSERT( m_pStyle != NULL );

	// Only applies to Motifs
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		return;
	}

	if( m_mtLoopStart != 0
	||  m_mtLoopEnd != (long)m_dwLength )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_MOTIF_LOOPPOINTS );

		// Change Motif Loop Start/End
		m_mtLoopStart = 0; 
		m_mtLoopEnd = (long)m_dwLength; 
		
		SetModified( TRUE );
//		m_pStyle->SyncStyleEditor( SSE_MOTIFS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::MusicTimeToBarBeatGridTick

void CDirectMusicPattern::MusicTimeToBarBeatGridTick( MUSIC_TIME mtTime,
													  long* plBar, long* plBeat, long* plGrid, long* plTick )
{
	long lClocksPerBeat = DM_PPQNx4 / (long)m_TimeSignature.m_bBeat;
	long lClocksPerMeasure = lClocksPerBeat * (long)m_TimeSignature.m_bBeatsPerMeasure;
	long lClocksPerGrid = lClocksPerBeat / (long)m_TimeSignature.m_wGridsPerBeat;
	long lRemainder;

	// Bar
	*plBar = (mtTime / lClocksPerMeasure) + 1;
	lRemainder = mtTime % lClocksPerMeasure;

	// Beat
	*plBeat = (lRemainder / lClocksPerBeat) + 1;
	lRemainder = lRemainder % lClocksPerBeat;

	// Grid
	*plGrid = (lRemainder / lClocksPerGrid) + 1;

	// Tick
	*plTick = lRemainder % lClocksPerGrid;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::BarBeatGridTickToMusicTime

void CDirectMusicPattern::BarBeatGridTickToMusicTime( long lBar, long lBeat, long lGrid, long lTick,
													  MUSIC_TIME* pmtTime )
{
	long lClocksPerBeat = DM_PPQNx4 / (long)m_TimeSignature.m_bBeat;
	long lClocksPerMeasure = lClocksPerBeat * (long)m_TimeSignature.m_bBeatsPerMeasure;
	long lClocksPerGrid = lClocksPerBeat / (long)m_TimeSignature.m_wGridsPerBeat;

	*pmtTime  = (lBar - 1) * lClocksPerMeasure;
	*pmtTime += (lBeat - 1) * lClocksPerBeat;
	*pmtTime += (lGrid - 1) * lClocksPerGrid;
	*pmtTime += lTick;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SyncTempo

void CDirectMusicPattern::SyncTempo( void )
{
	ASSERT( m_pStyle != NULL );

	// Sync timeline tempo
	if( m_pPatternCtrl
	&&  m_pPatternCtrl->m_pPatternDlg
	&&  m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
	{
		DMUS_TEMPO_PARAM dmusTempoParam;

		dmusTempoParam.mtTime = 0;
		dmusTempoParam.dblTempo = m_pStyle->m_dblTempo;

		m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->SetParam( GUID_TempoParam, 1, 0, 0, &dmusTempoParam );

		// Notify the Timeline that the tempo changed
		m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->NotifyStripMgrs( GUID_TempoParam, 0xffffffff, NULL );
	}

	// Sync conductor tempo
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIConductor != NULL );
	theApp.m_pStyleComponent->m_pIConductor->SetTempo( this, m_pStyle->m_dblTempo, TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::PlayBand

HRESULT CDirectMusicPattern::PlayBand( void )
{
	ASSERT( m_pStyle != NULL );
	ASSERT( m_pStyle->m_pIDMStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );

	// Determine which Band to use
	IDMUSProdNode* pIBandNode = GetActiveBand();
	if( pIBandNode == NULL )
	{
		return S_OK;
	}

	HRESULT hr = E_FAIL;

	// Persist the Band into a DirectMusicBand object
	IDirectMusicBand* pIDMBand = m_pStyle->GetDMBand( pIBandNode );
	if( pIDMBand )
	{
		// Send the Band
		IDirectMusicSegment* pIDMSegmentBand;

		if( SUCCEEDED ( pIDMBand->CreateSegment( &pIDMSegmentBand ) ) )
		{
			theApp.m_pStyleComponent->m_pIDMPerformance->PlaySegment( pIDMSegmentBand, DMUS_SEGF_SECONDARY , 0, NULL );
			hr = S_OK;

			RELEASE( pIDMSegmentBand );
		}

		RELEASE( pIDMBand );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::InsertBandTrack

HRESULT CDirectMusicPattern::InsertBandTrack( IDirectMusicSegment *pSegment, BOOL fNeedBandTrack )
{
	ASSERT( pSegment != NULL );
	if( pSegment == NULL )
	{
		return E_POINTER;
	}
	ASSERT( m_pStyle != NULL );
	ASSERT( m_pStyle->m_pIDMStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );

	// Determine which Band to use
	IDMUSProdNode* pIBandNode = GetActiveBand();
	if( pIBandNode == NULL )
	{
		return S_OK;
	}

	HRESULT hr = E_FAIL;

	// Persist the Band into a DirectMusicBand object
	IDirectMusicBand* pIDMBand = m_pStyle->GetDMBand( pIBandNode );
	if( pIDMBand )
	{
		// Create a new Band track
		IDirectMusicTrack* pIDMTrack;

		if( SUCCEEDED ( ::CoCreateInstance( CLSID_DirectMusicBandTrack, NULL, CLSCTX_INPROC, 
											IID_IDirectMusicTrack, (void**)&pIDMTrack ) ) )
		{
			// Place the Band into the track
			if( SUCCEEDED ( pIDMTrack->SetParam( GUID_IDirectMusicBand, 0, pIDMBand ) )
			&&  SUCCEEDED (	pIDMTrack->SetParam( GUID_Disable_Auto_Download, 0, NULL ) )
			&&  SUCCEEDED ( pSegment->InsertTrack( pIDMTrack, 1 ) ) ) 
			{
				pIDMTrack->Init( pSegment );

				if( !fNeedBandTrack )
				{
					pSegment->RemoveTrack( pIDMTrack );
					// Disable playback of the band track
					//pSegment->SetTrackConfig( CLSID_DirectMusicBandTrack, 1, 0, 0, 0xFFFFFFFF );
				}
				hr = S_OK;
			}

			RELEASE( pIDMTrack );
		}

		RELEASE( pIDMBand );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::InsertChordTrack

HRESULT CDirectMusicPattern::InsertChordTrack( IDirectMusicSegment *pSegment )
{
	ASSERT( pSegment != NULL );
	if( pSegment == NULL )
	{
		return E_POINTER;
	}

	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );

	IDirectMusicTrack* pIDMTrack;

	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( ::CoCreateInstance( CLSID_DirectMusicChordTrack, NULL, CLSCTX_INPROC, 
										IID_IDirectMusicTrack, (void**)&pIDMTrack ) ) )
	{
		hr = pSegment->InsertTrack( pIDMTrack, 1 );
		RELEASE( pIDMTrack );
	}

	return hr;
}



/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::GetChordList

IStream* CDirectMusicPattern::GetChordList( void )
{
	IDMUSProdRIFFStream* pIRiffStream = NULL;
	IStream* pIMemStream = NULL;
	BOOL fFoundChordList = FALSE;
    MMCKINFO ckMain;
    MMCKINFO ck;
	HRESULT hr;

	// Get a memory stream
	hr = theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIMemStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Get a RIFF stream
	hr = AllocRIFFStream( pIMemStream, &pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save the Pattern design data
	hr = DM_SavePatternEditInfo( pIRiffStream );
	if( hr != S_OK )
	{
		goto ON_ERROR;
	}

	// Reposition to beginning of stream
	LARGE_INTEGER liTemp;
	liTemp.QuadPart = 0;
	pIMemStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

	// Find the Chord list
	ckMain.fccType = DMUS_FOURCC_PATTERN_DESIGN;
	if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) == 0 )
	{
		ck.fccType = DMUS_FOURCC_CHORDSTRIP_LIST;
		if( pIRiffStream->Descend( &ck, NULL, MMIO_FINDLIST ) == 0 )
		{
			// Fix 19549: Check to see if the chord list's key and scale are 0.
			// If so, change them to 0x0C and 0xAB5AB5.
			DWORD dwPosition = StreamTell( pIMemStream );
			ck.fccType = DMUS_FOURCC_CHORDTRACK_LIST;
			if( pIRiffStream->Descend( &ck, NULL, MMIO_FINDLIST ) == 0 )
			{
				ck.ckid = DMUS_FOURCC_CHORDTRACKHEADER_CHUNK;
				if( pIRiffStream->Descend( &ck, NULL, MMIO_FINDCHUNK ) == 0 )
				{
					DWORD dwScalePos = StreamTell( pIMemStream );
					DWORD dwScale, dwBytes;
					if( (pIMemStream->Read(&dwScale, sizeof(DWORD), &dwBytes) == S_OK)
					&&	(dwBytes == sizeof(DWORD))
					&&	(dwScale == 0) )
					{
						VERIFY( SUCCEEDED( StreamSeek( pIMemStream, dwScalePos, STREAM_SEEK_SET) ) );
						// Default key and scale from Chord strip
						// (different from m_bDefaultKeyRoot and m_dwDefaultKeyPattern)
						dwScale = 0x0CAB5AB5;
						VERIFY( SUCCEEDED( pIMemStream->Write(&dwScale, sizeof(DWORD), &dwBytes) ) );
					}
				}
			}

			VERIFY( SUCCEEDED( StreamSeek( pIMemStream, dwPosition, STREAM_SEEK_SET) ) );
			fFoundChordList = TRUE;
		}
	}

ON_ERROR:
	RELEASE( pIRiffStream );

	if( fFoundChordList == FALSE )
	{
		RELEASE( pIMemStream );
	}

	return pIMemStream;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::UpdateChordTrack

HRESULT CDirectMusicPattern::UpdateChordTrack( IDirectMusicSegment *pSegment )
{
	if( pSegment == NULL )
	{
		return E_POINTER;
	}

	// Get the Chord list
	IStream* pIStream = GetChordList();
	if( pIStream == NULL )
	{
		DMUS_CHORD_PARAM chordData;

		wcscpy( chordData.wszName, L"M7" );
		chordData.wMeasure = 0;
		chordData.bBeat = 0;
		chordData.bKey = m_bDefaultKeyRoot;
		chordData.dwScale = m_dwDefaultKeyPattern;
		chordData.bSubChordCount = 4;
		chordData.SubChordList[0].dwChordPattern = m_dwDefaultChordPattern;
		chordData.SubChordList[0].dwScalePattern = m_dwDefaultKeyPattern;
		chordData.SubChordList[0].dwInversionPoints = 0xffffffff; // default: inversions everywhere
		chordData.SubChordList[0].dwLevels = 0xffffff01;			// default: match everything above level 16 and level 0
		chordData.SubChordList[0].bChordRoot = m_bDefaultChordRoot;
		chordData.SubChordList[0].bScaleRoot = m_bDefaultKeyRoot;
		chordData.SubChordList[1] = chordData.SubChordList[0];
		chordData.SubChordList[1].dwLevels = 0x00000002;			// level 1
		chordData.SubChordList[2] = chordData.SubChordList[0];
		chordData.SubChordList[2].dwLevels = 0x00000004;			// level 2
		chordData.SubChordList[3] = chordData.SubChordList[0];
		chordData.SubChordList[3].dwLevels = 0x00000008;			// level 3

		return ChangeChord( &chordData, pSegment );
	}

	IDirectMusicTrack* pIDMTrack = NULL;
	IPersistStream* pIPersistStreamTrack = NULL;

	HRESULT hr = E_FAIL;

	// Get the DirectMusic Chord track
	hr = pSegment->GetTrack( CLSID_DirectMusicChordTrack, 1, 0, &pIDMTrack );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Load Chords into DirectMusic Chord track
	hr = pIDMTrack->QueryInterface( IID_IPersistStream, (void**)&pIPersistStreamTrack );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}
	hr = pIPersistStreamTrack->Load( pIStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIDMTrack );
	RELEASE( pIStream );
	RELEASE( pIPersistStreamTrack );

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::ChangeChord

HRESULT CDirectMusicPattern::ChangeChord( DMUS_CHORD_PARAM* pChordData, IDirectMusicSegment *pSegment )
{
	if( pSegment == NULL )
	{
		return E_POINTER;
	}

	IDirectMusicTrack* pIDMTrack;

	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( pSegment->GetTrack( CLSID_DirectMusicChordTrack, 1, 0, &pIDMTrack ) ) )
	{
		hr = pIDMTrack->SetParam( GUID_ChordParam, 0, pChordData );

		RELEASE( pIDMTrack );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SyncPatternWithAuditionSegment

HRESULT CDirectMusicPattern::SyncPatternWithAuditionSegment( BOOL fFirstTime, IDirectMusicSegment *pSegment )
{
	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );

	if( pSegment == NULL )
	{
		return S_OK;
	}

	// Set segment length and repeats
	pSegment->SetLength( m_dwLength );
	pSegment->SetLoopPoints( 0, 0 ); // Repeat entire segment
	pSegment->SetRepeats( DMUS_SEG_REPEAT_INFINITE );

	// Get active track and active variations
	long lTrack = -1;
	DWORD dwVariations = 0xFFFFFFFF;
	GUID guidPart = GUID_NULL;
	BOOL fHaveTrack = FALSE;
	BOOL fHaveVariations = FALSE;
	BOOL fHaveGUID = FALSE;

	if( m_pPatternCtrl
	&&  m_pPatternCtrl->m_pPatternDlg
	&&  m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
	{
		VARIANT var;
		
		if( SUCCEEDED( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_ACTIVESTRIP, &var )))
		{
			IUnknown* pIUnknown = V_UNKNOWN(&var);
			if( pIUnknown )
			{
				IDMUSProdStrip* pIStrip;
				if( SUCCEEDED( pIUnknown->QueryInterface( IID_IDMUSProdStrip, (void**)&pIStrip )))
				{
					if( SUCCEEDED( pIStrip->GetStripProperty( MSP_PIANOROLL_VARIATIONS, &var )))
					{
						fHaveVariations = TRUE;
						dwVariations = V_UI4(&var);
					}

					if( SUCCEEDED( pIStrip->GetStripProperty( MSP_PIANOROLL_TRACK, &var )))
					{
						fHaveTrack = TRUE;
						lTrack = V_I4(&var);
					}

					var.vt = VT_BYREF;
					V_BYREF(&var) = &guidPart;
					if( SUCCEEDED( pIStrip->GetStripProperty( MSP_PIANOROLL_GUID, &var )))
					{
						fHaveGUID = TRUE;
					}

					RELEASE( pIStrip );
				}

				RELEASE( pIUnknown );
			}
		}
	}

	IDirectMusicTrack* pIDMTrack;
	IPrivatePatternTrack* pIDMPrivatePatternTrack;
	IStream* pIMemStream;

	// Update Chord track
	UpdateChordTrack( pSegment );

	// Update Pattern Track
	if( SUCCEEDED ( pSegment->GetTrack( CLSID_DirectMusicPatternTrack, 1, 0, &pIDMTrack ) ) )
	{
		if( SUCCEEDED ( pIDMTrack->QueryInterface( IID_IPrivatePatternTrack, (void **)&pIDMPrivatePatternTrack ) ) )
		{
			// Set variation/track
			if( fFirstTime
			||  fHaveTrack
			||  fHaveVariations )
			{
				IPrivatePatternTrack9* pIDMPrivatePatternTrack9;
				if( SUCCEEDED ( pIDMTrack->QueryInterface( IID_IPrivatePatternTrack9, (void **)&pIDMPrivatePatternTrack9 ) ) )
				{
					// If no variations are selected, then pass GUID_AllZeros
					pIDMPrivatePatternTrack9->SetVariationMaskByGUID( m_pIDMSegmentState, dwVariations, dwVariations ? guidPart : GUID_AllZeros, lTrack );

					RELEASE( pIDMPrivatePatternTrack9 );
				}
				else
				{
					pIDMPrivatePatternTrack->SetVariationByGUID( m_pIDMSegmentState, dwVariations, guidPart, lTrack );
				}
			}

			// Set Pattern/Motif
			if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_RUNTIME, GUID_SinglePattern, &pIMemStream ) ) )
			{
				if( SUCCEEDED ( Save( pIMemStream, FALSE ) ) )
				{
					DWORD dwLength;

					pIDMPrivatePatternTrack->SetPattern( m_pIDMSegmentState, pIMemStream, &dwLength ); 
				}

				RELEASE( pIMemStream );
			}

			RELEASE( pIDMPrivatePatternTrack );
		}
		
		RELEASE( pIDMTrack );
	}

	// Invalidate queued events when chord strip has changed
	if( m_pPatternCtrl
	&&  m_pPatternCtrl->m_pPatternDlg
	&&  m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr )
	{
		BOOL fChordStripChanged = FALSE;

		m_pPatternCtrl->m_pPatternDlg->m_pIMIDIStripMgr->GetParam( GUID_ChordStripChanged, 0, NULL, &fChordStripChanged );
		if( fChordStripChanged )
		{
			MUSIC_TIME mtTimeNow;
			theApp.m_pStyleComponent->m_pIDMPerformance->GetTime( NULL, &mtTimeNow );
			theApp.m_pStyleComponent->m_pIDMPerformance->Invalidate( mtTimeNow, 0 );
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::CreateSegment

HRESULT CDirectMusicPattern::CreateSegment( BOOL fNeedBandTrack )
{
	ASSERT( m_pIDMSegment == NULL );

	ASSERT( m_pStyle != NULL );
	ASSERT( m_pStyle->m_pIDMStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIDMPerformance != NULL );

	IDirectMusicPatternTrack* pIDMPatternTrack;

	// Create the pattern track object
	HRESULT hr = ::CoCreateInstance( CLSID_DirectMusicPatternTrack, NULL, CLSCTX_INPROC, 
								 	 IID_IDirectMusicPatternTrack, (void**)&pIDMPatternTrack );
	if( SUCCEEDED ( hr ) )
	{
		// Create a segment for the pattern track
		hr = pIDMPatternTrack->CreateSegment( m_pStyle->m_pIDMStyle, &m_pIDMSegment );
		if( SUCCEEDED ( hr ) )
		{
			// Mark this segment as a Pattern audition segment
			IDirectMusicObject *pIDirectMusicObject = NULL;
			if( SUCCEEDED( m_pIDMSegment->QueryInterface( IID_IDirectMusicObject, (void **)&pIDirectMusicObject ) ) )
			{
				DMUS_OBJECTDESC objDesc;
				objDesc.dwSize = sizeof( DMUS_OBJECTDESC );
				objDesc.dwValidData = DMUS_OBJ_OBJECT;
				objDesc.guidObject = GUID_PatternAuditionSegment;
				pIDirectMusicObject->SetDescriptor( &objDesc );
				pIDirectMusicObject->Release();
			}

			// Insert a Band track when needed
			hr = InsertBandTrack( m_pIDMSegment, fNeedBandTrack );

			if( SUCCEEDED ( hr )  )
			{
				// Prepare the rest of the Segment
				if( SUCCEEDED ( InsertChordTrack( m_pIDMSegment ) )
				&&  SUCCEEDED ( SyncPatternWithAuditionSegment( TRUE, m_pIDMSegment ) ) )
				{
					// Disable tempo track so that repeats do not reset tempo
					IDirectMusicTrack* pIDMTrack;
					if( SUCCEEDED ( m_pIDMSegment->GetTrack( CLSID_DirectMusicTempoTrack, 1, 0, &pIDMTrack ) ) )
					{
						pIDMTrack->SetParam( GUID_DisableTempo, 0, NULL );

						RELEASE( pIDMTrack );
					}

					m_pIDMSegment->SetDefaultResolution( m_wEmbellishment & EMB_MOTIF ? m_dwResolution : DMUS_SEGF_BEAT );
				}
				else
				{
					hr = E_FAIL;
				}
			}
		}

		RELEASE( pIDMPatternTrack );
	}
	return hr;
}

HRESULT CDirectMusicPattern::Initialize1()
// initialize state
{
	ASSERT( m_pRhythmMap == NULL );

	// Initialize rhythm map
	m_pRhythmMap = new DWORD[m_wNbrMeasures];
	if( m_pRhythmMap == NULL )
	{
		return E_OUTOFMEMORY;
	}

	for( int i = 0 ;  i < m_wNbrMeasures ;  i++ )
	{
		if( i )
		{
			m_pRhythmMap[i] = 0;
		}
		else
		{
			m_pRhythmMap[i] = 1;
		}
	}
	return S_OK;
}

HRESULT CDirectMusicPattern::Initialize2()
{
	// Create an empty Part
	CDirectMusicPart* pPart = m_pStyle->AllocPart();
	if( pPart == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Create a Part Reference
	CDirectMusicPartRef* pPartRef = AllocPartRef();
	if( pPartRef == NULL )
	{
		m_pStyle->DeletePart( pPart );
		return E_OUTOFMEMORY;
	}

	pPartRef->SetPart( pPart );

	return S_OK;
}


CDirectMusicEventItem* CDirectMusicPattern::MakeDirectMusicEventItem(FullSeqEvent* pEvent)
{
	ASSERT(pEvent);
	if(!pEvent)
	{
		return 0;
	}

	CDirectMusicEventItem* pItem = 0;
	CDirectMusicStyleNote* pNote = 0;
	CDirectMusicStyleCurve* pCurve = 0;

	long lBeatClocks, lMeasureClocks, lGridClocks;
	lBeatClocks = DM_PPQNx4 / m_TimeSignature.m_bBeat;
	lMeasureClocks = lBeatClocks * m_TimeSignature.m_bBeatsPerMeasure;
	lGridClocks = lBeatClocks / m_TimeSignature.m_wGridsPerBeat;

	switch(pEvent->bStatus&0xF0)
	{
	case MIDI_NOTEON:
		pNote = new CDirectMusicStyleNote;
		pNote->m_mtDuration = pEvent->mtDuration;
		pNote->m_bVelocity = pEvent->bByte2;
		pNote->m_bTimeRange = pNote->m_bDurRange = pNote->m_bVelRange = 0;
		pNote->m_bInversionId = 0;
		pNote->m_bNoteFlags = 0;
		pNote->m_bPlayModeFlags = DMUS_PLAYMODE_NONE;
		if( pEvent->dwPChannel == 9 /* drum channel */)
		{
			pNote->m_wMusicValue = pEvent->bByte1;
		}
		else
		{
			DMUS_CHORD_KEY chord;
			ZeroMemory( &chord, sizeof( DMUS_CHORD_KEY ) );
			chord.bSubChordCount = 1;
			chord.dwScale = 0x00AB5AB5;
			//chord.bKey = 0;
			chord.SubChordList[0].dwChordPattern = m_dwDefaultChordPattern;
			chord.SubChordList[0].dwScalePattern = m_dwDefaultKeyPattern;
			chord.SubChordList[0].dwInversionPoints = 0xFFFFFFFF;
			chord.SubChordList[0].dwLevels = 0xFFFFFFFF;
			chord.SubChordList[0].bChordRoot = m_bDefaultChordRoot;
			chord.SubChordList[0].bScaleRoot = m_bDefaultKeyRoot;
			theApp.m_pStyleComponent->m_pIDMPerformance->MIDIToMusic( pEvent->bByte1, &chord,
																	  DMUS_PLAYMODE_PURPLEIZED, 0,
																	  &pNote->m_wMusicValue );
		}
		pItem = pNote;
		break;
	case MIDI_PTOUCH:
	case MIDI_CCHANGE:
	case MIDI_MTOUCH:
	case MIDI_PBEND:
		pCurve = new CDirectMusicStyleCurve;
		pCurve->m_mtDuration = 1;
		pCurve->m_bCurveShape = 	DMUS_CURVES_INSTANT;
		switch(pEvent->bStatus&0xF0)
		{
			case MIDI_PTOUCH:
				pCurve->m_bEventType = DMUS_CURVET_PATCURVE;
				pCurve->m_bCCData = pEvent->bByte1;
				pCurve->m_nStartValue = pCurve->m_nEndValue = pEvent->bByte2;
				break;
			case MIDI_CCHANGE:
				pCurve->m_bEventType = DMUS_CURVET_CCCURVE;
				pCurve->m_bCCData = pEvent->bByte1;
				pCurve->m_nStartValue = pCurve->m_nEndValue = pEvent->bByte2;
				break;
			case MIDI_MTOUCH:
				pCurve->m_bEventType = DMUS_CURVET_MATCURVE;
				pCurve->m_nStartValue = pCurve->m_nEndValue = pEvent->bByte1;
				break;
			case MIDI_PBEND:
				pCurve->m_bEventType = DMUS_CURVET_PBCURVE;
				pCurve->m_nStartValue = pCurve->m_nEndValue = ((pEvent->bByte2 & 0x7F) << 7) + (pEvent->bByte1 & 0x7F);
				break;
		}
		pItem = pCurve;
		break;
	default:
		ASSERT(FALSE);
		return 0;
	}


	pItem->m_dwVariation = 1;
	// compute offsets (imported events are not quantized)
	pItem->m_mtGridStart = (pEvent->mtTime + pEvent->nOffset) / lGridClocks ;
	pItem->m_nTimeOffset = (short)((pEvent->mtTime + pEvent->nOffset) - (long)pItem->m_mtGridStart*lGridClocks);
	return pItem;
}

DWORD CDirectMusicPartRef::GetPChannel()
{
	return m_dwPChannel;
}

void CDirectMusicPartRef::SetPChannel(DWORD pchan)
{
	m_dwPChannel = pchan;
}

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::OldNoteToMusicValue

/*
unsigned short CDirectMusicPattern::OldNoteToMusicValue(

unsigned char note,     // MIDI note to convert.
long keypattern,        // Description of key as interval pattern.
char keyroot,           // Root note of key.
long chordpattern,      // Description of chord as interval pattern.
char chordroot)         // Root note of chord.

{
unsigned char   octpart = 0 ;
unsigned char   chordpart ;
unsigned char   keypart = (BYTE)-1 ;       
unsigned char   accpart = 0 ;
unsigned char   scan, test, base, last ;    // was char
long            pattern ;
short           testa, testb ;


    scan = chordroot ;

	// If we're trying to play a note below the bottom of our chord, forget it
	if( note < scan)
	{
		return 0;
	}

    while( scan < (note - 24) )
    {
        scan += 12 ;
        octpart++ ;
    }

    base = scan ;

    for( ;  base<=note ;  base+=12 )
    {
        chordpart = (unsigned char)-1 ;
        pattern   = chordpattern ;
        scan      = last = base ;
        if( scan == note )
            return( unsigned short (octpart << 12) ) ;           // if octave, return.
        for( ;  pattern ;  pattern=pattern >> 1 )
        {
            if( pattern & 1 )                   // chord interval?
            {                 
                if( scan == note )              // note in chord?
                {            
                    chordpart++ ;
                    return(unsigned short ((octpart << 12) | (chordpart << 8))) ; // yes, return.
                }
                else if (scan > note)           // above note?
                {         
                    test = scan ;
                    break ;                     // go on to key.
                }
                chordpart++ ;
                last = scan ;
            }
            scan++ ;
        }
        if( !pattern )                          // end of chord.
        {                        
            test = unsigned char(base + 12) ;                  // set to next note.
        }
        octpart++ ;
        if( test > note )
        {
            break ;                             // above our note?
        }
    }

    octpart-- ;

//  To get here, the note is not in the chord.  Scan should show the last
//  note in the chord.  octpart and chordpart have their final values.
//  Now, increment up the key to find the match.

    scan        = last ;
    keypattern |= keypattern << 12 ;
    keypattern  = keypattern >> ((scan - keyroot) % 12) ;

    for( ;  keypattern ;  keypattern=keypattern >> 1 )
    {
        if( 1 & keypattern )
        {
            keypart++ ;
            accpart = 0 ;
        }
        else
        {
            accpart++ ;
        }
        if( scan == note )
            break ;
        scan++;
    }

    if( accpart && keypart )
    {
        testa = short((octpart << 12) + (chordpart << 8) + (keypart << 4) + accpart + 1);
        testb = short((octpart << 12) + ((chordpart + 1) << 8) + 0);
        testa = OldMusicValueToNote( testa, 0, keypattern, keyroot,
                                     chordpattern, chordroot, (char)0 );
        testb = OldMusicValueToNote( testb, 0, keypattern, keyroot,
                                     chordpattern, chordroot, (char)0 );
        if( testa == testb )
        {
            chordpart++ ;
            keypart = 0 ;
            accpart = 15 ;
        }
    }

    return unsigned short((octpart << 12) + (chordpart << 8) + (keypart << 4) + accpart);

}
*/

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::OldMusicValueToNote

/*
unsigned char CDirectMusicPattern::OldMusicValueToNote(

unsigned short value,   // Music value to convert.
char scalevalue,        // Scale value if chord failes.
long keypattern,        // Description of key as interval pattern.
char keyroot,           // Root note of key.
long chordpattern,      // Description of chord as interval pattern.
char chordroot,         // Root note of chord.
char count)             // Total notes in chord.

{
unsigned char   result ;
char            octpart   = (char)(value >> 12) ;
char            chordpart = (char)((value >> 8) & 0xF) ;
char            keypart   = (char)((value >> 4) & 0xF) ;
char            accpart   = (char)(value & 0xF) ;
unsigned char   bits      = (unsigned char) (count & 0xF0) ;

    count  &= CHORD_COUNT ;

    result  = unsigned char(12 * octpart) ;
    result += chordroot ;

    if( accpart > 8 )
        accpart -= 16 ;

    if( count ) {
        if( bits & CHORD_FOUR ) {
            if( count > 4 )
                chordpart += (count - 4) ;
        } else {
            if( count > 3 )
                chordpart += (count - 3) ;
        }
    }

//    if( chordpart ) {
        for( ;  chordpattern ;  result++ ) {
            if( chordpattern & 1L ) {
                if( !chordpart )
                    break ;
                chordpart-- ;
            }
            chordpattern = chordpattern >> 1L ;
            if( !chordpattern ) {
                if( !scalevalue )
                    return( 0 ) ;
                result  = unsigned char(12 * octpart) ;
                result += chordroot ;
                keypart = char(scalevalue >> 4) ;
                accpart = char(scalevalue & 0x0F) ;
                break ;
            }
        }
//    }

    if( keypart ) {
        keypattern |= (keypattern << 12L) ;
        keypattern  = keypattern >> (LONG)((result - keyroot) % 12) ;
        for( ;  keypattern ;  result++ ) {
            if( keypattern & 1L ) {
                if( !keypart )
                    break ;
                keypart-- ;
            }
            keypattern = keypattern >> 1L ;
        }
    }

    result += unsigned char(accpart) ;
    return( result ) ;

}
*/


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::ActivateDialog

void CDirectMusicPattern::ActivateDialog( BOOL fActivate )
{
	m_fDialogActive = fActivate;

	if( m_fDialogActive )
	{
		MUSIC_TIME mtNow;
		if( SUCCEEDED( theApp.m_pStyleComponent->m_pIDMPerformance->GetTime( NULL, &mtNow ) ) )
		{
			IDirectMusicSegmentState *pSegmentState = NULL;
			if( FAILED( theApp.m_pStyleComponent->m_pIDMPerformance->GetSegmentState( &pSegmentState, mtNow ) ) )
			{
				// Play the active band
				PlayBand();
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

	if( m_fRecordPressed )
	{
		if( m_pPatternCtrl && m_pPatternCtrl->m_pPatternDlg
		&&	m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr )
		{
			IMIDIMgr* pIMIDIMgr;
			if( SUCCEEDED ( m_pPatternCtrl->m_pPatternDlg->m_punkMIDIStripMgr->QueryInterface( IID_IMIDIMgr, (void**) &pIMIDIMgr) ) )
			{
				// If dialog active - tell MIDIMgr Record is pressed
				// If dialog inactive - tell MIDIMgr Record is no longer pressed
				pIMIDIMgr->OnRecord( m_fDialogActive );
				pIMIDIMgr->Release();
			}
		}
	}

	if( m_pPatternCtrl && m_pPatternCtrl->m_pPatternDlg )
	{
		m_pPatternCtrl->m_pPatternDlg->Activate( fActivate );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::PreChangePartRef

void CDirectMusicPattern::PreChangePartRef( CDirectMusicPartRef* pDMPartRef )
{
	ASSERT( m_pStyle );
	m_pStyle->PreChangePartRef( pDMPartRef );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::UpdateLinkedParts

void CDirectMusicPattern::UpdateLinkedParts( CDirectMusicPartRef* pPartRef, LPCTSTR pcstrText )
{
	CDirectMusicPartRef* pPartRefList;
	POSITION pos = m_lstPartRefs.GetHeadPosition();
	BOOL fChange = FALSE;

	while( pos )
	{
		pPartRefList = m_lstPartRefs.GetNext( pos );

		if( pPartRefList->m_pDMPart && pPartRefList->m_fHardLink &&
		    (pPartRefList->m_pDMPart != pPartRef->m_pDMPart) )
		{
			if( ::IsEqualGUID( pPartRefList->m_pDMPart->m_guidPartID, pPartRef->m_guidOldPartID ) )
			{
				fChange = TRUE;
			}
		}
	}
	if( fChange )
	{
		// Save "Undo" state
		if( pcstrText )
		{
			m_pUndoMgr->SaveState( this, (char *)pcstrText );
		}
		else
		{
			m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_PATTERN_PART );
		}

		pos = m_lstPartRefs.GetHeadPosition();
		while( pos )
		{
			pPartRefList = m_lstPartRefs.GetNext( pos );

			if( pPartRefList->m_pDMPart && pPartRefList->m_fHardLink &&
				(pPartRefList->m_pDMPart != pPartRef->m_pDMPart) )
			{
				if( ::IsEqualGUID( pPartRefList->m_pDMPart->m_guidPartID, pPartRef->m_guidOldPartID ) )
				{
					pPartRefList->SetPart( pPartRef->m_pDMPart );
				}
			}
		}
		
		SetModified( TRUE );
		m_pStyle->SyncStyleEditor( m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::UpdateLinkFlags

void CDirectMusicPattern::UpdateLinkFlags()
{
	CDirectMusicPartRef* pPartRefList;
	POSITION pos = m_lstPartRefs.GetHeadPosition();
	BOOL fChange = FALSE;

	while( pos )
	{
		pPartRefList = m_lstPartRefs.GetNext( pos );

		if( pPartRefList->m_pDMPart && pPartRefList->m_fHardLink &&
		    (pPartRefList->m_pDMPart->m_dwHardLinkCount == 1) )
		{
			fChange = TRUE;
		}
	}

	if( fChange )
	{
		// Save "Undo" state
		//  Don't save an undo state - this is a side effect
		//m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_PATTERN_PART_UNLINK );

		pos = m_lstPartRefs.GetHeadPosition();
		while( pos )
		{
			pPartRefList = m_lstPartRefs.GetNext( pos );

			if( pPartRefList->m_pDMPart && pPartRefList->m_fHardLink &&
				(pPartRefList->m_pDMPart->m_dwHardLinkCount == 1) )
			{
				pPartRefList->m_fHardLink = FALSE;
				pPartRefList->m_pDMPart->m_dwHardLinkCount = 0;
			}
		}
		
		SetModified( TRUE );
		m_pStyle->SyncStyleEditor( m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::StartStopMusic

void CDirectMusicPattern::StartStopMusic( BOOL fStart, BOOL fInTransition )
{
	// If our start/stop state changed, or if we're in transition
	if( (fStart != m_fPatternIsPlaying) || fInTransition )
	{
		m_fPatternIsPlaying = fStart;

		if( !fStart && m_pIDMSegmentState )
		{
			m_rpIDMStoppedSegmentState = m_pIDMSegmentState;
			RELEASE( m_pIDMSegment );
			RELEASE( m_pIDMSegmentState );
			RELEASE( m_pIDMTransitionSegmentState );
			RELEASE( m_pIDMTransitionSegment );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::UpdateMIDIMgrsAndChangePartPtrs

void CDirectMusicPattern::UpdateMIDIMgrsAndChangePartPtrs( CDirectMusicPart* pOldPart, CDirectMusicPart* pNewPart, LPCTSTR pcstrText )
{
	ASSERT( pOldPart );
	ASSERT( pNewPart );

	CDirectMusicPartRef* pPartRefList;
	POSITION pos = m_lstPartRefs.GetHeadPosition();
	BOOL fChange = FALSE;

	while( pos )
	{
		pPartRefList = m_lstPartRefs.GetNext( pos );

		if( pPartRefList->m_fHardLink && (pPartRefList->m_pDMPart == pOldPart) )
		{

			fChange = TRUE;
		}
	}

	if( fChange )
	{
		// Save "Undo" state
		if( pcstrText )
		{
			m_pUndoMgr->SaveState( this, (char *)pcstrText );
		}
		else
		{
			m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_PATTERN_PART );
		}

		pos = m_lstPartRefs.GetHeadPosition();
		while( pos )
		{
			pPartRefList = m_lstPartRefs.GetNext( pos );

			if( pPartRefList->m_fHardLink && (pPartRefList->m_pDMPart == pOldPart) )
			{
				pPartRefList->SetPart( pNewPart );
			}
		}
		
		SetModified( TRUE );
		m_pStyle->SyncStyleEditor( m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::CopyToForLinkAllParts

HRESULT CDirectMusicPattern::CopyToForLinkAllParts( CDirectMusicPattern* pPattern )
{
	pPattern->m_dwDefaultKeyPattern		= m_dwDefaultKeyPattern;	
	pPattern->m_dwDefaultChordPattern	= m_dwDefaultChordPattern;
	pPattern->m_bDefaultKeyRoot			= m_bDefaultKeyRoot;		
	pPattern->m_bDefaultChordRoot		= m_bDefaultChordRoot;	

	pPattern->m_TimeSignature		= m_TimeSignature;
	pPattern->m_wNbrMeasures		= m_wNbrMeasures;
	pPattern->m_dwLength			= m_dwLength;
	pPattern->m_bGrooveBottom		= m_bGrooveBottom;
	pPattern->m_bGrooveTop			= m_bGrooveTop;
	pPattern->m_bDestGrooveBottom	= m_bDestGrooveBottom;
	pPattern->m_bDestGrooveTop		= m_bDestGrooveTop;
	pPattern->m_dwFlags				= m_dwFlags;

	// Initialize rhythm map
	pPattern->m_pRhythmMap = new DWORD[pPattern->m_wNbrMeasures];
	if( pPattern->m_pRhythmMap == NULL )
	{
		return E_OUTOFMEMORY;
	}
	for( int i = 0 ;  i < pPattern->m_wNbrMeasures ;  i++ )
	{
		if( pPattern->m_wEmbellishment & EMB_MOTIF )
		{
			if( i )
			{
				pPattern->m_pRhythmMap[i] = 0;
			}
			else
			{
				pPattern->m_pRhythmMap[i] = 1;
			}
		}
		else
		{
			pPattern->m_pRhythmMap[i] = m_pRhythmMap[i];
		}
	}

	// Copy UI state and Chords for Composition
	if( pPattern->m_pPatternDesignData )
	{
		GlobalFree( pPattern->m_pPatternDesignData );
		pPattern->m_pPatternDesignData = NULL;
	}
	pPattern->m_pPatternDesignData = (BYTE *)GlobalAlloc( GPTR, m_dwPatternDesignDataSize );
	if( pPattern->m_pPatternDesignData )
	{
		pPattern->m_dwPatternDesignDataSize = m_dwPatternDesignDataSize;
		memcpy( pPattern->m_pPatternDesignData, m_pPatternDesignData, m_dwPatternDesignDataSize );  
	}

	// Motif specific data
	if( pPattern->m_wEmbellishment & EMB_MOTIF
	&&  m_wEmbellishment & EMB_MOTIF )
	{
		pPattern->m_dwRepeats		= m_dwRepeats;
		pPattern->m_mtPlayStart		= m_mtPlayStart;
		pPattern->m_mtLoopStart		= m_mtLoopStart;
		pPattern->m_mtLoopEnd		= m_mtLoopEnd;
		pPattern->m_dwResolution	= m_dwResolution;

		if( m_pIBandNode )
		{
			IStream* pIMemStream;

			if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIMemStream ) ) )
			{
				IPersistStream* pIPersistStream;

				if( SUCCEEDED ( m_pIBandNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
				{
					if( SUCCEEDED ( pIPersistStream->Save( pIMemStream, FALSE ) ) )
					{
						IDMUSProdRIFFExt* pIRIFFExt;

						if( SUCCEEDED ( theApp.m_pStyleComponent->m_pIBandComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
						{
							IDMUSProdNode* pINode;

							StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
							if( SUCCEEDED ( pIRIFFExt->LoadRIFFChunk( pIMemStream, &pINode ) ) )
							{
								ASSERT( pPattern->m_pIBandNode == NULL );
								pPattern->m_pIBandNode = pINode;
							}

							RELEASE( pIRIFFExt );
						}
					}

					RELEASE( pIPersistStream );
				}

				RELEASE( pIMemStream );
			}
		}
	}

	// Pattern specific data
	if( !(pPattern->m_wEmbellishment & EMB_MOTIF)
	&&  !(m_wEmbellishment & EMB_MOTIF) )
	{
		pPattern->m_wEmbellishment	= m_wEmbellishment;
	}

	// Link all Parts
	CDirectMusicPartRef *pPartRefList;
	CDirectMusicPartRef *pPartRef;

	POSITION pos = m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		pPartRefList = m_lstPartRefs.GetNext( pos );

		pPartRef = new CDirectMusicPartRef( pPattern );
		if( pPartRef )
		{
			// Copy pertinent information from the original PartRef
			// and link to the original PartRef's Part
			if( SUCCEEDED ( pPartRefList->LinkParts( pPartRef ) ) )
			{
				// Add to Motif's PartRef list
				pPattern->m_lstPartRefs.AddTail( pPartRef );
			}
			else
			{
				delete pPartRef;
			}
		}
	}

	pPattern->SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::QueueTempoEvent

HRESULT CDirectMusicPattern::QueueTempoEvent()
{
	DMUS_TEMPO_PMSG* pTempo;
	if( SUCCEEDED( theApp.m_pStyleComponent->m_pIDMPerformance->AllocPMsg( sizeof(DMUS_TEMPO_PMSG),
		(DMUS_PMSG**)&pTempo ) ) )
	{
		// Queue tempo event
		ZeroMemory( pTempo, sizeof(DMUS_TEMPO_PMSG) );
		pTempo->dblTempo = m_pStyle->m_dblTempo;
		pTempo->dwFlags = DMUS_PMSGF_REFTIME | DMUS_PMSGF_TOOL_IMMEDIATE;
		pTempo->dwType = DMUS_PMSGT_TEMPO;

		// Make it play when our SegmentState starts playing
		REFERENCE_TIME rtStartTime = 0;
		if( m_pIDMSegmentState )
		{
			MUSIC_TIME mtStartTime = 0;
			m_pIDMSegmentState->GetStartTime( &mtStartTime );
			theApp.m_pStyleComponent->m_pIDMPerformance->MusicToReferenceTime( mtStartTime, &rtStartTime );
			rtStartTime -= 10000;
		}
		pTempo->rtTime = rtStartTime;

		return theApp.m_pStyleComponent->m_pIDMPerformance->SendPMsg( (DMUS_PMSG*)pTempo );
	}
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::GetActiveBand

IDMUSProdNode* CDirectMusicPattern::GetActiveBand( void )
{
	IDMUSProdNode* pIBandNode = NULL;

	// Determine which band to use
	if( m_wEmbellishment & EMB_MOTIF )
	{
		// Use motif's band
		pIBandNode = m_pIBandNode;
	}
	
	if( pIBandNode == NULL )
	{
		pIBandNode = m_pStyle->GetActiveBand();
		if( pIBandNode == NULL )
		{
			pIBandNode = m_pStyle->GetTheDefaultBand();
		}
	}

	return pIBandNode;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::PasteCF_BAND

HRESULT CDirectMusicPattern::PasteCF_BAND( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have Bands
		return E_FAIL;
	}

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
				InsertChildNode( pINode );
				RELEASE( pINode );
			}

			RELEASE( pIRIFFExt );
		}

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::PasteCF_BANDLIST

HRESULT CDirectMusicPattern::PasteCF_BANDLIST( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have Bands
		return E_FAIL;
	}

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
// CDirectMusicPattern::LoadCF_BANDTRACK

HRESULT CDirectMusicPattern::LoadCF_BANDTRACK( IStream* pIStream )
{
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have Bands
		return E_FAIL;
	}

	if( pIStream == NULL )
	{
		return E_POINTER;
	}

	HRESULT hr = E_FAIL;
	BOOL fFinished = FALSE;

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
				if( fFinished )
				{
					break;
				}

				switch( ckList.ckid )
				{
					case FOURCC_LIST :
						switch( ckList.fccType )
						{
							case DMUS_FOURCC_BAND_LIST:
							{
								MMCKINFO ck;

								// Extract the first Band and attach it to the Motif
								while( pIRiffStream->Descend( &ck, &ckList, 0 ) == 0 )
								{
									if( fFinished )
									{
										break;
									}

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

															InsertChildNode( pINode );
															fFinished = TRUE;

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
// CDirectMusicPattern::PasteCF_BANDTRACK

HRESULT CDirectMusicPattern::PasteCF_BANDTRACK( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have Bands
		return E_FAIL;
	}

	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		hr = LoadCF_BANDTRACK( pIStream );

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::PasteCF_TIMELINE

HRESULT CDirectMusicPattern::PasteCF_TIMELINE( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Pattern nodes do not have Bands
		return E_FAIL;
	}

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
										hr = LoadCF_BANDTRACK( pIStream );
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
// CDirectMusicPattern::SendChangeNotification

void CDirectMusicPattern::SendChangeNotification( void )
{
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		// Only send notifications for Motifs
		return;
	}

	// Notify connected nodes that Motif has changed
	DMUSProdMotifData	mtfData;
	WCHAR				wstrMotifName[MAX_PATH];

	MultiByteToWideChar( CP_ACP, 0, m_strName, -1, wstrMotifName, MAX_PATH );
	mtfData.pwszMotifName = wstrMotifName;
	mtfData.pwszOldMotifName = NULL;
	
	theApp.m_pStyleComponent->m_pIFramework->NotifyNodes( m_pStyle, STYLE_MotifChanged, &mtfData );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SetFlags

void CDirectMusicPattern::SetFlags( DWORD dwFlags )
{
	ASSERT( m_pStyle != NULL );

	if( dwFlags != m_dwFlags )
	{
		// Save "Undo" state
		m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_PATTERN_FLAGS );

		// Change flags
		m_dwFlags = dwFlags; 

		SetModified( TRUE );
		m_pStyle->SyncStyleEditor( m_wEmbellishment & EMB_MOTIF ? SSE_MOTIFS : SSE_PATTERNS );
		SyncPatternWithMidiStripMgr();
		SyncPatternWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::PreDeleteCleanup

void CDirectMusicPattern::PreDeleteCleanup( void )
{
	// See if we need to deal with link flags or the var. choices window
	POSITION pos = m_lstPartRefs.GetHeadPosition();
	while( pos )
	{
		CDirectMusicPartRef* pDMPartRef = m_lstPartRefs.GetNext( pos );

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
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::SyncTimelineSettings

void CDirectMusicPattern::SyncTimelineSettings( void )
{
	if( !m_fIgnoreTimelineSync
	&&	m_pPatternCtrl
	&&	m_pPatternCtrl->m_pPatternDlg
	&&	m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl )
	{
		VARIANT var;
		if( SUCCEEDED( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_ZOOM, &var ) ) )
		{
			m_dblZoom = V_R8(&var);
		}

		if( (m_dblZoom > 0.0)
		&&	SUCCEEDED( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_HORIZONTAL_SCROLL, &var ) ) )
		{
			m_dblHorizontalScroll = V_I4(&var) / m_dblZoom;
		}

		if( SUCCEEDED( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_VERTICAL_SCROLL, &var ) ) )
		{
			m_lVerticalScroll = V_I4(&var);
		}

		if( SUCCEEDED( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_SNAP_TO, &var ) ) )
		{
			m_tlSnapTo = (DMUSPROD_TIMELINE_SNAP_TO)V_I4(&var);
		}

		if( SUCCEEDED( m_pPatternCtrl->m_pPatternDlg->m_pITimelineCtl->GetTimelineProperty( TP_FUNCTIONBAR_WIDTH, &var ) ) )
		{
			m_lFunctionbarWidth = V_I4(&var);
		}
	}
}
