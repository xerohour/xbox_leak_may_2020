// SegmentPPGMgr.cpp: implementation of the CSegmentPPGMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SegmentDesignerDLL.h"
#include "SegmentPPGMgr.h"
#include "SegmentPPG.h"
#include "LoopPPG.h"
#include "InfoPPG.h"
#include "TabBoundaryFlags.h"
#include "Segment.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


short CSegmentPPGMgr::sm_nActiveTab = 0;

PPGSegment::PPGSegment()
{
	dwPageIndex = 0;
	strSegmentName.Empty();
	dwMeasureLength = 1;
	mtSegmentLength = 0;
	rtSegmentLength = 0;
    rtLoopStart = 0;
    rtLoopEnd = 0;
	wSegmentRepeats = 0;
	dwResolution = DMUS_SEGF_MEASURE;
	dwPlayStartMeasure = 0;
	bPlayStartBeat = 0;
	wPlayStartGrid = 0;
	wPlayStartTick = 0;
	dwTrackGroup = 1;
	dwExtraBars = 0;
	fPickupBar = FALSE;
	dwSegmentFlags = 0;

	pITimelineCtl = NULL;

	dwLoopStartMeasure = 0;
	bLoopStartBeat = 0;
	wLoopStartGrid = 0;
	wLoopStartTick = 0;
	dwLoopEndMeasure = 0;
	bLoopEndBeat = 0;
	wLoopEndGrid = 0;
	wLoopEndTick = 0;
	dwLoopRepeats = 0;

	strSubject.Empty();
	strAuthor.Empty();
	strCopyright.Empty();
	wVersion1 = 1;
	wVersion2 = 0;
	wVersion3 = 0;
	wVersion4 = 0;
	strInfo.Empty();
	ZeroMemory( &guidSegment, sizeof(GUID) );

	dwFlags = 0;
}

PPGSegment::~PPGSegment()
{
	if(pITimelineCtl != NULL)
	{
		pITimelineCtl->Release();
		pITimelineCtl = NULL;
	}
}

void PPGSegment::Copy( const PPGSegment* pSegment )
{
	if(pSegment == this)
	{
		return;
	}

	strSegmentName = pSegment->strSegmentName;
	dwMeasureLength = pSegment->dwMeasureLength;
	mtSegmentLength = pSegment->mtSegmentLength;
	rtSegmentLength = pSegment->rtSegmentLength;
    rtLoopStart = pSegment->rtLoopStart;
    rtLoopEnd = pSegment->rtLoopEnd;
	wSegmentRepeats = pSegment->wSegmentRepeats;
	dwResolution = pSegment->dwResolution;
	dwPlayStartMeasure = pSegment->dwPlayStartMeasure;
	bPlayStartBeat = pSegment->bPlayStartBeat;
	wPlayStartGrid = pSegment->wPlayStartGrid;
	wPlayStartTick = pSegment->wPlayStartTick;
	dwTrackGroup = pSegment->dwTrackGroup;
	dwExtraBars = pSegment->dwExtraBars;
	fPickupBar = pSegment->fPickupBar;
	dwSegmentFlags = pSegment->dwSegmentFlags;

	if ( pITimelineCtl )
	{
		pITimelineCtl->Release();
	}
	pITimelineCtl = pSegment->pITimelineCtl;
	if ( pITimelineCtl )
	{
		pITimelineCtl->AddRef();
	}

	dwLoopStartMeasure = pSegment->dwLoopStartMeasure;
	bLoopStartBeat = pSegment->bLoopStartBeat;
	wLoopStartGrid = pSegment->wLoopStartGrid;
	wLoopStartTick = pSegment->wLoopStartTick;
	dwLoopEndMeasure = pSegment->dwLoopEndMeasure;
	bLoopEndBeat = pSegment->bLoopEndBeat;
	wLoopEndGrid = pSegment->wLoopEndGrid;
	wLoopEndTick = pSegment->wLoopEndTick;
	dwLoopRepeats = pSegment->dwLoopRepeats;

	strSubject = pSegment->strSubject;
	strAuthor = pSegment->strAuthor;
	strCopyright = pSegment->strCopyright;
	wVersion1 = pSegment->wVersion1;
	wVersion2 = pSegment->wVersion2;
	wVersion3 = pSegment->wVersion3;
	wVersion4 = pSegment->wVersion4;
	strInfo = pSegment->strInfo;
	memcpy( &guidSegment, &pSegment->guidSegment, sizeof(GUID) );

	dwFlags = pSegment->dwFlags;
	return;
}

void PPGSegment::Import( const _DMUS_IO_SEGMENT_DESIGN* pSegmentDesign )
{
	// Ensure the segment is at least one measure long
	dwMeasureLength = max( 1, pSegmentDesign->dwMeasureLength);
	wSegmentRepeats = pSegmentDesign->wSegmentRepeats;
	dwPlayStartMeasure = pSegmentDesign->dwPlayStartMeasure;
	bPlayStartBeat = pSegmentDesign->bPlayStartBeat;
	wPlayStartGrid = pSegmentDesign->wPlayStartGrid;
	wPlayStartTick = pSegmentDesign->wPlayStartTick;
	dwTrackGroup = pSegmentDesign->dwTrackGroup;

	dwLoopStartMeasure = pSegmentDesign->dwLoopStartMeasure;
	bLoopStartBeat = pSegmentDesign->bLoopStartBeat;
	wLoopStartGrid = pSegmentDesign->wLoopStartGrid;
	wLoopStartTick = pSegmentDesign->wLoopStartTick;
	dwLoopEndMeasure = pSegmentDesign->dwLoopEndMeasure;
	bLoopEndBeat = pSegmentDesign->bLoopEndBeat;
	wLoopEndGrid = pSegmentDesign->wLoopEndGrid;
	wLoopEndTick = pSegmentDesign->wLoopEndTick;

	return;
}

void PPGSegment::Export( _DMUS_IO_SEGMENT_DESIGN* pSegmentDesign ) const
{
	pSegmentDesign->dwMeasureLength = dwMeasureLength;
	pSegmentDesign->wSegmentRepeats = wSegmentRepeats;
	pSegmentDesign->dwPlayStartMeasure = dwPlayStartMeasure;
	pSegmentDesign->bPlayStartBeat = bPlayStartBeat;
	pSegmentDesign->wPlayStartGrid = wPlayStartGrid;
	pSegmentDesign->wPlayStartTick = wPlayStartTick;
	pSegmentDesign->dwTrackGroup = dwTrackGroup;

	pSegmentDesign->dwLoopStartMeasure = dwLoopStartMeasure;
	pSegmentDesign->bLoopStartBeat = bLoopStartBeat;
	pSegmentDesign->wLoopStartGrid = wLoopStartGrid;
	pSegmentDesign->wLoopStartTick = wLoopStartTick;
	pSegmentDesign->dwLoopEndMeasure = dwLoopEndMeasure;
	pSegmentDesign->bLoopEndBeat = bLoopEndBeat;
	pSegmentDesign->wLoopEndGrid = wLoopEndGrid;
	pSegmentDesign->wLoopEndTick = wLoopEndTick;

	return;
}

//////////////////////////////////////////////////////////////////////
// CSegmentPPGMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSegmentPPGMgr::CSegmentPPGMgr( ) : CDllBasePropPageManager()
{
	m_pSegmentPPG	= NULL;
	m_pLoopPPG		= NULL;
	m_pInfoPPG		= NULL;
	m_pBoundaryFlags = NULL;
	m_GUIDManager	= GUID_SegmentPPGMgr;
}

CSegmentPPGMgr::~CSegmentPPGMgr()
{
	if(m_pSegmentPPG)
	{
		delete m_pSegmentPPG;
	}
	if(m_pLoopPPG)
	{
		delete m_pLoopPPG;
	}
	if(m_pInfoPPG)
	{
		delete m_pInfoPPG;
	}
	if(m_pBoundaryFlags)
	{
		delete m_pBoundaryFlags;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPGMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPGMgr IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CSegmentPPGMgr::GetPropertySheetTitle(BSTR* pbstrTitle, BOOL* pfAddPropertiesText)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if((pbstrTitle == NULL)
	|| (pfAddPropertiesText == NULL))
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_SEGMENT_TEXT );

	PPGSegment ppgSegment;
	PPGSegment *pSegment = &ppgSegment;

	// Nothing is constructed in the call to GetData, so we don't need to worry
	// about cleaning up any memory
	if(m_pIPropPageObject
	// This call actually changes the value of pSegment - the data in ppgSegment is NOT modified
	&& (SUCCEEDED(m_pIPropPageObject->GetData((void **)&pSegment)))
	&& pSegment->strSegmentName.GetLength() > 0)
	{
		strTitle = pSegment->strSegmentName + _T(" ") + strTitle;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentPPGMgr IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CSegmentPPGMgr::GetPropertySheetPages(IDMUSProdPropSheet *pIPropSheet, LONG *hPropSheetPage[], short *pnNbrPages)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if((hPropSheetPage == NULL)
	|| (pnNbrPages == NULL))
	{
		return E_POINTER;
	}

	if(pIPropSheet == NULL)
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	// Add Segment tab
	if( m_pSegmentPPG == NULL )
	{
		m_pSegmentPPG = new CSegmentPPG();
		m_pSegmentPPG->m_pPageManager = this;
	}
	if(m_pSegmentPPG)
	{
		hPage = ::CreatePropertySheetPage((LPCPROPSHEETPAGE)&m_pSegmentPPG->m_psp);
		if(hPage)
		{
			hPropSheetPage[nNbrPages] = (LONG *) hPage;
			nNbrPages++;
		}
	}

	// Add Loop tab
	if( m_pLoopPPG == NULL )
	{
		m_pLoopPPG = new CLoopPPG();
		m_pLoopPPG->m_pPageManager = this;
	}
	if(m_pLoopPPG)
	{
		hPage = ::CreatePropertySheetPage((LPCPROPSHEETPAGE)&m_pLoopPPG->m_psp);
		if(hPage)
		{
			hPropSheetPage[nNbrPages] = (LONG *) hPage;
			nNbrPages++;
		}
	}

	// Add Info tab
	if( m_pInfoPPG == NULL )
	{
		m_pInfoPPG = new CInfoPPG( this );
	}
	if(m_pInfoPPG)
	{
		hPage = ::CreatePropertySheetPage((LPCPROPSHEETPAGE)&m_pInfoPPG->m_psp);
		if(hPage)
		{
			hPropSheetPage[nNbrPages] = (LONG *) hPage;
			nNbrPages++;
		}
	}

	// Add Boundary tab
	if( m_pBoundaryFlags == NULL )
	{
		m_pBoundaryFlags = new CTabBoundaryFlags( this );
		m_pBoundaryFlags->m_rpIPropSheet = m_pIPropSheet;
		m_pBoundaryFlags->sm_pnActiveTab = &CSegmentPPGMgr::sm_nActiveTab;
	}
	if(m_pBoundaryFlags)
	{
		hPage = ::CreatePropertySheetPage((LPCPROPSHEETPAGE)&m_pBoundaryFlags->m_psp);
		if(hPage)
		{
			hPropSheetPage[nNbrPages] = (LONG *) hPage;
			nNbrPages++;
		}
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentPPGMgr IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CSegmentPPGMgr::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	if(m_pIPropSheet)
	{
		m_pIPropSheet->GetActivePage( &CSegmentPPGMgr::sm_nActiveTab );
	}

	HRESULT hr = CBasePropPageManager::OnRemoveFromPropertySheet();

	if(m_pIPropSheet)
	{
		m_pIPropSheet->Release();
		m_pIPropSheet = NULL;
	}

	if(m_pSegmentPPG)
	{
		m_pSegmentPPG->SetData(NULL);
	}
	if( m_pLoopPPG )
	{
		m_pLoopPPG->SetData(NULL);
	}
	if( m_pInfoPPG )
	{
		m_pInfoPPG->SetData(NULL);
	}
	if( m_pBoundaryFlags )
	{
		m_pBoundaryFlags->SetObject(NULL);
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPGMgr IDMUSProdPropPageManager::RefreshData

HRESULT CSegmentPPGMgr::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegmentPPG == NULL
	&&	m_pLoopPPG == NULL
	&&	m_pInfoPPG == NULL
	&&	m_pBoundaryFlags == NULL )
	{
		return S_OK;
	}

	PPGSegment ppgSegment;
	PPGSegment* pSegment = &ppgSegment;
	
	if(m_pIPropPageObject == NULL)
	{
		pSegment = NULL;
	}
	// This call actually changes the value of pSegment - the data in ppgSegment is NOT modified
	else if(FAILED(m_pIPropPageObject->GetData((void **)&pSegment)))
	{
		return E_FAIL;
	}

	if ( m_pSegmentPPG )
	{
		m_pSegmentPPG->SetData( pSegment );
	}
	if ( m_pLoopPPG )
	{
		m_pLoopPPG->SetData( pSegment );
	}
	if ( m_pInfoPPG )
	{
		m_pInfoPPG->SetData( pSegment );
	}
	if( m_pBoundaryFlags )
	{
		m_pBoundaryFlags->SetObject( m_pIPropPageObject );
		m_pBoundaryFlags->RefreshTab();
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPGMgr IDMUSProdPropPageManager::RemoveObject

HRESULT CSegmentPPGMgr::RemoveObject( IDMUSProdPropPageObject *pIPropPageObject )
{
	HRESULT hr;

	hr = CDllBasePropPageManager::RemoveObject(pIPropPageObject);
	if(SUCCEEDED(hr))
	{
		if(m_pSegmentPPG)
		{
			m_pSegmentPPG->SetData(NULL);
		}
		if(m_pLoopPPG)
		{
			m_pLoopPPG->SetData(NULL);
		}
		if(m_pInfoPPG)
		{
			m_pInfoPPG->SetData(NULL);
		}
		if( m_pBoundaryFlags )
		{
			m_pBoundaryFlags->SetObject(NULL);
		}
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CSegmentPPGMgr IDMUSProdPropPageManager::SetObject

HRESULT CSegmentPPGMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	HRESULT hr;

	hr = CDllBasePropPageManager::SetObject(pINewPropPageObject);
	if(FAILED(hr))
	{
		return hr;
	}

	if(pINewPropPageObject != NULL && pINewPropPageObject != m_pIPropPageObject)
	{
		if(m_pSegmentPPG != NULL)
		{
			// This is a new segment, so reset the warnuser flag to notify the user when data will be lost.
			//m_pSegmentPPG->SetWarnUser(TRUE);
		}
	}
	return hr;
}
