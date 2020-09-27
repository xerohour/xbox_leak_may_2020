#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackItem.h"
#include "TrackMgr.h"
#include <dmusicf.h>

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//	CTrackItem Constructors/Destructor

CTrackItem::CTrackItem( void )
{
	m_pTrackMgr = NULL;

	Clear();
}

CTrackItem::CTrackItem( CTrackMgr* pTrackMgr )
{
	ASSERT( pTrackMgr != NULL );
	m_pTrackMgr = pTrackMgr;

	Clear();
}

CTrackItem::CTrackItem( CTrackMgr* pTrackMgr, const CTrackItem& item )
{
	ASSERT( pTrackMgr != NULL );
	m_pTrackMgr = pTrackMgr;

	// Copy the passed-in item
	Copy( &item );
}

CTrackItem::~CTrackItem( void )
{
	if( m_pTrackMgr )
	{
		SetFileReference( NULL );
	}
	else
	{
		ClearListInfo();
		RELEASE( m_FileRef.pIDocRootNode );
	}
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::Clear

void CTrackItem::Clear()
{
	m_dwFlagsDM = DMUS_IO_SCRIPTTRACKF_QUEUE;
	m_rtTimeLogical = 0;
	m_rtTimePhysical = 0;

	m_lMeasure = 0;
	m_lBeat = 0;
	m_lTick = 0;
	m_lLogicalMeasure = 0;
	m_lLogicalBeat = 0;

	m_dwBitsUI = 0;
	m_wFlagsUI = 0;
	m_fSelected = FALSE;

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( _Module.GetModuleInstance() );

	m_strRoutine.LoadString( IDS_NONE_TEXT );

	if( m_pTrackMgr )
	{
		SetFileReference( NULL );
	}
	else
	{
		ClearListInfo();
		RELEASE( m_FileRef.pIDocRootNode );
	}

	AfxSetResourceHandle( hInstance );
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::ClearListInfo

void CTrackItem::ClearListInfo()
{

	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( _Module.GetModuleInstance() );

	m_FileRef.li.pIProject = NULL;

	m_FileRef.li.strProjectName.LoadString( IDS_EMPTY_TEXT );
	m_FileRef.li.strName.LoadString( IDS_EMPTY_TEXT );
	m_FileRef.li.strDescriptor.LoadString( IDS_EMPTY_TEXT );

	memset( &m_FileRef.li.guidFile, 0, sizeof(GUID) );

	AfxSetResourceHandle( hInstance );
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::Copy

void CTrackItem::Copy( const CTrackItem* pItem )
{
	ASSERT( pItem != NULL );
	if ( pItem == NULL )
	{
		return;
	}

	if( pItem == this )
	{
		return;
	}

	m_dwFlagsDM = pItem->m_dwFlagsDM;
	m_rtTimeLogical = pItem->m_rtTimeLogical;
	m_rtTimePhysical = pItem->m_rtTimePhysical;

	m_lMeasure = pItem->m_lMeasure;
	m_lBeat = pItem->m_lBeat;
	m_lTick = pItem->m_lTick;
	m_lLogicalMeasure = pItem->m_lLogicalMeasure;
	m_lLogicalBeat = pItem->m_lLogicalBeat;

	m_dwBitsUI = pItem->m_dwBitsUI;
	m_wFlagsUI = pItem->m_wFlagsUI;
	m_fSelected = pItem->m_fSelected;
	m_strRoutine = pItem->m_strRoutine;

	if( m_pTrackMgr )
	{
		SetFileReference( pItem->m_FileRef.pIDocRootNode );
	}
	else
	{
		m_FileRef.li.pIProject = pItem->m_FileRef.li.pIProject;
		m_FileRef.li.strProjectName = pItem->m_FileRef.li.strProjectName;
		m_FileRef.li.strName = pItem->m_FileRef.li.strName;
		m_FileRef.li.strDescriptor = pItem->m_FileRef.li.strDescriptor;
		memcpy( &m_FileRef.li.guidFile, &pItem->m_FileRef.li.guidFile, sizeof(GUID) );

		RELEASE( m_FileRef.pIDocRootNode );
		m_FileRef.pIDocRootNode = pItem->m_FileRef.pIDocRootNode;
		if( m_FileRef.pIDocRootNode )
		{
			m_FileRef.pIDocRootNode->AddRef();
		}
	}
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::SetFileReference

HRESULT CTrackItem::SetFileReference( IDMUSProdNode* pINewDocRootNode )
{
	HRESULT hr = S_OK;

	ASSERT( m_pTrackMgr != NULL );
	if( m_pTrackMgr == NULL )
	{
		// Will be NULL when CTrackItem used for properties!
		// Must not set Script references for properties!
		return E_FAIL;
	}

	// Get Framework pointer
	IDMUSProdFramework* pIFramework;
	VARIANT variant;
	hr = m_pTrackMgr->GetStripMgrProperty( SMP_IDMUSPRODFRAMEWORK, &variant );
	if( FAILED ( hr ) )
	{
		return hr;
	}
	hr = V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&pIFramework );
	V_UNKNOWN( &variant )->Release();
	if( FAILED ( hr ) )
	{
		return hr;
	}

	// Get DocRoot pointer (i.e. Segment's DocRoot node)
	IDMUSProdNode* pIDocRootNode;
	hr = m_pTrackMgr->GetParam( GUID_DocRootNode, 0, 0, &pIDocRootNode );
	if( FAILED ( hr ) )
	{
		RELEASE( pIFramework );
		return hr;
	}

	// Clean up old Script reference
	if( m_FileRef.pIDocRootNode )
	{
		// Turn off notifications for this node
		if( m_FileRef.fRemoveNotify )
		{
			if( pIDocRootNode )
			{
				hr = pIFramework->RemoveFromNotifyList( m_FileRef.pIDocRootNode, pIDocRootNode );
			}
			m_FileRef.fRemoveNotify = FALSE;
		}

		// Release reference 
		RELEASE( m_FileRef.pIDocRootNode );

		// Initialize pertinent fields
		ClearListInfo();
		m_dwBitsUI = 0;
	}

	// Set new Script DocRoot 
	if( pINewDocRootNode )
	{
		// Turn on notifications
		ASSERT( m_FileRef.fRemoveNotify == FALSE );
		if( pIDocRootNode )
		{
			hr = pIFramework->AddToNotifyList( pINewDocRootNode, pIDocRootNode );
			if( SUCCEEDED ( hr ) )
			{
				m_FileRef.fRemoveNotify = TRUE;
			}
		}
		
		// Update Script DocRoot member variable
		m_FileRef.pIDocRootNode = pINewDocRootNode;
		m_FileRef.pIDocRootNode->AddRef();

		// Update Script list info
		SetListInfo( pIFramework );
	}

	RELEASE( pIFramework );
	RELEASE( pIDocRootNode );

	return hr;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::SetFileReference

HRESULT CTrackItem::SetFileReference( GUID guidFile )
{
	HRESULT hr;

	// Get Framework pointer
	IDMUSProdFramework* pIFramework;
	VARIANT variant;
	hr = m_pTrackMgr->GetStripMgrProperty( SMP_IDMUSPRODFRAMEWORK, &variant );
	if( FAILED ( hr ) )
	{
		return hr;
	}
	hr = V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&pIFramework );
	V_UNKNOWN( &variant )->Release();
	if( FAILED ( hr ) )
	{
		return hr;
	}

	hr = E_FAIL;

	// Find the DocRoot node
	IDMUSProdNode* pIDocRootNode;
	if( SUCCEEDED ( pIFramework->FindDocRootNodeByFileGUID( guidFile, &pIDocRootNode ) ) )
	{
		hr = SetFileReference( pIDocRootNode );

		RELEASE( pIDocRootNode );
	}

	RELEASE( pIFramework );
	return hr;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::SetListInfo

HRESULT CTrackItem::SetListInfo( IDMUSProdFramework* pIFramework )
{
	HRESULT hr = S_OK;

	ClearListInfo();

	if( m_FileRef.pIDocRootNode )
	{
		DMUSProdListInfo ListInfo;
		ZeroMemory( &ListInfo, sizeof(ListInfo) );
		ListInfo.wSize = sizeof(ListInfo);

		hr = m_FileRef.pIDocRootNode->GetNodeListInfo ( &ListInfo );
		if( SUCCEEDED ( hr ) )
		{
			IDMUSProdProject* pIProject;

			if( ListInfo.bstrName )
			{
				m_FileRef.li.strName = ListInfo.bstrName;
				::SysFreeString( ListInfo.bstrName );
			}
			if( ListInfo.bstrDescriptor )
			{
				m_FileRef.li.strDescriptor = ListInfo.bstrDescriptor;
				::SysFreeString( ListInfo.bstrDescriptor );
			}
			if( SUCCEEDED ( pIFramework->FindProject( m_FileRef.pIDocRootNode, &pIProject ) ) )
			{
				BSTR bstrProjectName;

				m_FileRef.li.pIProject = pIProject;
//				m_FileRef.li.pIProject->AddRef();		intentionally missing

				if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
				{
					m_FileRef.li.strProjectName = bstrProjectName;
					::SysFreeString( bstrProjectName );
				}

				pIProject->Release();
			}

		    pIFramework->GetNodeFileGUID( m_FileRef.pIDocRootNode, &m_FileRef.li.guidFile );
		}
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::FormatUIText

void CTrackItem::FormatUIText( CString& strText )
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( _Module.GetModuleInstance() );

	// Routine
	strText = m_strRoutine;
	if( strText.IsEmpty() )
	{
		strText.LoadString( IDS_NONE_TEXT );
	}

	AfxSetResourceHandle( hInstance );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SwitchTimeBase

void CTrackItem::SwitchTimeBase( void )
{
	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		// Switching from MUSIC_TIME to REFERENCE_TIME
		m_pTrackMgr->m_pTimeline->ClocksToRefTime( (MUSIC_TIME)m_rtTimePhysical, &m_rtTimePhysical );
	}
	else
	{
		// Switching from REFERENCE_TIME to MUSIC_TIME
		MUSIC_TIME mtTime;
		m_pTrackMgr->m_pTimeline->RefTimeToClocks( m_rtTimePhysical, &mtTime );
		m_rtTimePhysical = mtTime;
	}

	SetTimePhysical( m_rtTimePhysical, STP_LOGICAL_SET_DEFAULT );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackItem::SetTimePhysical

HRESULT CTrackItem::SetTimePhysical( REFERENCE_TIME rtTimePhysical, short nAction )
{
	// Set item's physical time
	REFERENCE_TIME rtOrigTimePhysical = m_rtTimePhysical;
	m_rtTimePhysical = rtTimePhysical;

	if( m_pTrackMgr 
	&&  m_pTrackMgr->IsRefTimeTrack() )
	{
		if( m_pTrackMgr->m_pTimeline )
		{
			m_pTrackMgr->RefTimeToMinSecMs( m_rtTimePhysical, &m_lMeasure, &m_lBeat, &m_lTick );
		}
		m_rtTimeLogical = max( 0, m_rtTimePhysical );
		return S_OK;
	}

	if( m_pTrackMgr == NULL
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		if( nAction == STP_LOGICAL_NO_ACTION 
		||  nAction == STP_LOGICAL_RECALC_MEASURE_BEAT )
		{
			return S_OK;
		}
		else
		{
			ASSERT( 0 );
			return E_UNEXPECTED;
		}
	}

	// Set measure, beat, tick 
	// RefTimeTrack handled above so we can assume we are dealing with clocks at this point
	m_pTrackMgr->ClocksToMeasureBeatTick( (MUSIC_TIME)m_rtTimePhysical, &m_lMeasure, &m_lBeat, &m_lTick );

	// Set logical time
	long lMeasure, lBeat, lTick;

	// Set item's logical time
	switch( nAction )
	{
		case STP_LOGICAL_NO_ACTION:
			// Nothing to do
			break;

		case STP_LOGICAL_RECALC_MEASURE_BEAT:
			// Simply recalc item's logical measure/beat
			m_pTrackMgr->ClocksToMeasureBeatTick( (MUSIC_TIME)m_rtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			break;

		case STP_LOGICAL_SET_DEFAULT:
		{
			MUSIC_TIME mtTimePhysical;
			MUSIC_TIME mtTimeLogical;
			m_pTrackMgr->UnknownTimeToClocks( m_rtTimePhysical, &mtTimePhysical );
			m_pTrackMgr->ClocksToMeasureBeatTick( mtTimePhysical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			m_pTrackMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &mtTimeLogical );
			m_rtTimeLogical = mtTimeLogical;
			break;
		}

		case STP_LOGICAL_FROM_BEAT_OFFSET:
		{
			MUSIC_TIME mtTimePhysical;
			MUSIC_TIME mtTimeLogical;

			// Get track GroupBits
			DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();

			// Snap new physical time to number of beats
			long lPhysicalBeats;
			m_pTrackMgr->UnknownTimeToClocks( m_rtTimePhysical, &mtTimePhysical );
			m_pTrackMgr->ClocksToMeasureBeatTick( mtTimePhysical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Set item's new logical time
			long lNewLogicalBeats = lPhysicalBeats + (MUSIC_TIME)m_rtTimeLogical;	// m_rtTimeLogical stores beat offset
			if( lNewLogicalBeats < 0 )
			{
				lNewLogicalBeats = 0;
			}
			BeatsToMeasureBeat( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lNewLogicalBeats, lMeasure, lBeat );
			m_pTrackMgr->ForceBoundaries( lMeasure, lBeat, 0, &mtTimeLogical );
			m_pTrackMgr->ClocksToMeasureBeatTick( mtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			m_pTrackMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &mtTimeLogical );
			m_rtTimeLogical = mtTimeLogical;
			break;
		}

		case STP_LOGICAL_ADJUST:
		{
			MUSIC_TIME mtTimePhysical;
			MUSIC_TIME mtTimeLogical;

			// Get track GroupBits
			DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();

			// Snap original physical time to number of beats
			long lPhysicalBeats;
			m_pTrackMgr->UnknownTimeToClocks( rtOrigTimePhysical, &mtTimePhysical );
			m_pTrackMgr->ClocksToMeasureBeatTick( mtTimePhysical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Convert logical time to number of beats
			long lLogicalBeats;
			m_pTrackMgr->ClocksToMeasureBeatTick( (MUSIC_TIME)m_rtTimeLogical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lLogicalBeats );

			// Compute difference between original physical time and original logical time
			long lBeatDiff = lLogicalBeats - lPhysicalBeats;

			// Snap new physical time to number of beats
			m_pTrackMgr->UnknownTimeToClocks( m_rtTimePhysical, &mtTimePhysical );
			m_pTrackMgr->ClocksToMeasureBeatTick( mtTimePhysical, &lMeasure, &lBeat, &lTick );
			MeasureBeatToBeats( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lMeasure, lBeat, lPhysicalBeats );

			// Set item's new logical time
			long lNewLogicalBeats = lPhysicalBeats + lBeatDiff;
			if( lNewLogicalBeats < 0 )
			{
				lNewLogicalBeats = 0;
			}
			BeatsToMeasureBeat( m_pTrackMgr->m_pTimeline, dwGroupBits, 0, lNewLogicalBeats, lMeasure, lBeat );
			m_pTrackMgr->ForceBoundaries( lMeasure, lBeat, 0, &mtTimeLogical );
			m_pTrackMgr->ClocksToMeasureBeatTick( mtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
			m_pTrackMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &mtTimeLogical );
			m_rtTimeLogical = mtTimeLogical;
			break;
		}

		default:
			ASSERT( 0 );	// Should not happen!
			break;
	}

	// Make sure item's logical time is not less than zero
	if( m_rtTimeLogical < 0 )
	{
		ASSERT( 0 );
		m_rtTimeLogical = 0;
	}
	ASSERT( m_lLogicalMeasure >= 0 );
	ASSERT( m_lLogicalBeat >= 0 );

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
//	CTrackItem::SetTimeLogical
	
HRESULT CTrackItem::SetTimeLogical( REFERENCE_TIME rtTimeLogical )
{
	ASSERT( m_pTrackMgr != NULL );
	if( m_pTrackMgr == NULL )
	{
		// Will be NULL when CTrackItem used for properties!
		return E_FAIL;
	}

	if( m_pTrackMgr->IsRefTimeTrack() )
	{
		// Cannot edit logical time in REFERENCE_TIME tracks
		ASSERT( 0 );
		return E_UNEXPECTED;
	}


	// Make sure item's logical time is not less than zero
	if( rtTimeLogical < 0 )
	{
		ASSERT( 0 );
		rtTimeLogical = 0;
	}

	// Set item's logical time
	m_rtTimeLogical = rtTimeLogical;

	if( m_pTrackMgr == NULL 
	||  m_pTrackMgr->m_pTimeline == NULL )
	{
		ASSERT( 0 );
		return E_UNEXPECTED;
	}

	// Set item's logical measure, beat
	long lTick;
	MUSIC_TIME mtTimeLogical;
	m_pTrackMgr->ClocksToMeasureBeatTick( (MUSIC_TIME)m_rtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lTick );
	m_pTrackMgr->MeasureBeatTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, &mtTimeLogical );
	m_rtTimeLogical = mtTimeLogical;

	return S_OK;
}

