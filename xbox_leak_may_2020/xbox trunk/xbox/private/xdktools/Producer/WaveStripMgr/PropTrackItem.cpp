#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackItem.h"
#include "PropTrackItem.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
//	CPropTrackItem Constructors/Destructor

CPropTrackItem::CPropTrackItem( void )
{
	m_nPropertyTab = -1;

	Clear();
}

CPropTrackItem::~CPropTrackItem( void )
{
}


//////////////////////////////////////////////////////////////////////
//	CPropTrackItem::Clear

void CPropTrackItem::Clear()
{
	// Don't touch m_nPropertyTab field

	m_dwChanged = 0;
	m_dwBits = 0;

	m_dwUndetermined_TabFileRef = 0;
	m_dwUndetermined_TabLoop = 0;
	m_dwUndetermined_TabPerformance = 0;
	m_dwUndetermined_TabVariations = 0;

	m_dwChangedVar_TabVariations = 0;

	m_Item.Clear();
	m_Item.SetWaveStripForPropSheet( NULL );
	m_Item.SetTrackMgr( NULL );
}


//////////////////////////////////////////////////////////////////////
//	CPropTrackItem::Copy

void CPropTrackItem::Copy( const CPropTrackItem* pPropItem )
{
	if( pPropItem == NULL )
	{
		ASSERT( 0 );
		return;
	}

	if( pPropItem == this )
	{
		return;
	}

	m_nPropertyTab = pPropItem->m_nPropertyTab;
	m_dwChanged = pPropItem->m_dwChanged;
	m_dwBits = pPropItem->m_dwBits;

	m_dwUndetermined_TabFileRef = pPropItem->m_dwUndetermined_TabFileRef;
	m_dwUndetermined_TabLoop = pPropItem->m_dwUndetermined_TabLoop;
	m_dwUndetermined_TabPerformance = pPropItem->m_dwUndetermined_TabPerformance;
	m_dwUndetermined_TabVariations = pPropItem->m_dwUndetermined_TabVariations;

	m_dwChangedVar_TabVariations = pPropItem->m_dwChangedVar_TabVariations;

	m_Item.Copy( &pPropItem->m_Item );
	m_Item.SetWaveStripForPropSheet( pPropItem->m_Item.GetWaveStripForPropSheet() );
}


//////////////////////////////////////////////////////////////////////
//	CPropTrackItem::CopyTrackItem

void CPropTrackItem::CopyTrackItem( const CTrackItem* pItem )
{
	if( pItem == NULL )
	{
		ASSERT( 0 );
		return;
	}

	m_Item.Copy( pItem );
	m_Item.SetWaveStripForPropSheet( pItem->GetWaveStripForPropSheet() );
}


//////////////////////////////////////////////////////////////////////
//	CPropTrackItem::MergeTrackItem

void CPropTrackItem::MergeTrackItem( const CTrackItem* pItem )
{
	if( pItem == NULL )
	{
		ASSERT( 0 );
		return;
	}

	// TabFileRef
	if( m_Item.m_FileRef.pIDocRootNode != pItem->m_FileRef.pIDocRootNode )
	{
		m_dwUndetermined_TabFileRef |= UNDT_DOCROOT;
	}
	if( m_Item.m_rtTimePhysical != pItem->m_rtTimePhysical )
	{
		m_dwUndetermined_TabFileRef |= UNDT_TIME_PHYSICAL;
	}
	if( m_Item.m_rtStartOffset != pItem->m_rtStartOffset )
	{
		m_dwUndetermined_TabFileRef |= UNDT_START_OFFSET;
	}
	if( m_Item.m_rtDuration != pItem->m_rtDuration )
	{
		m_dwUndetermined_TabFileRef |= UNDT_DURATION;
	}
	if( m_Item.m_fLockEndUI != pItem->m_fLockEndUI )
	{
		m_dwUndetermined_TabFileRef |= UNDT_LOCK_END;
	}
	if( m_Item.m_fLockLengthUI != pItem->m_fLockLengthUI )
	{
		m_dwUndetermined_TabFileRef |= UNDT_LOCK_LENGTH;
	}

	// TabLoop
	if( m_Item.m_fLoopedUI != pItem->m_fLoopedUI )
	{
		m_dwUndetermined_TabLoop |= UNDT_IS_LOOPED;
	}
	if( m_Item.m_dwLoopStartUI != pItem->m_dwLoopStartUI )
	{
		m_dwUndetermined_TabLoop |= UNDT_LOOP_START;
	}
	if( m_Item.m_dwLoopEndUI != pItem->m_dwLoopEndUI )
	{
		m_dwUndetermined_TabLoop |= UNDT_LOOP_END;
	}
	if( m_Item.m_fLockLoopLengthUI != pItem->m_fLockLoopLengthUI )
	{
		m_dwUndetermined_TabLoop |= UNDT_LOOP_LOCK_LENGTH;
	}

	// TabPerformance
	if( m_Item.m_rtTimeLogical != pItem->m_rtTimeLogical )
	{
		m_dwUndetermined_TabPerformance |= UNDT_TIME_LOGICAL;
	}
	if( (m_Item.m_dwFlagsDM & DMUS_WAVEF_NOINVALIDATE) != (pItem->m_dwFlagsDM & DMUS_WAVEF_NOINVALIDATE) )
	{
		m_dwUndetermined_TabPerformance |= UNDT_WAVEF_NOINVALIDATE ;
	}
	if( (m_Item.m_dwFlagsDM & DMUS_WAVEF_IGNORELOOPS) != (pItem->m_dwFlagsDM & DMUS_WAVEF_IGNORELOOPS) )
	{
		m_dwUndetermined_TabPerformance |= UNDT_WAVEF_IGNORELOOPS;
	}
	if( m_Item.m_lVolume != pItem->m_lVolume )
	{
		m_dwUndetermined_TabPerformance |= UNDT_VOLUME;
	}
	if( m_Item.m_lPitch != pItem->m_lPitch )
	{
		m_dwUndetermined_TabPerformance |= UNDT_PITCH;
	}

	// TabVariations
	for( int i = 0  ;  i < 32 ;  i++ )
	{
		if( (m_Item.m_dwVariations & (1 << i)) != (pItem->m_dwVariations & (1 << i)) )
		{
			m_dwUndetermined_TabVariations |= (1 << i);
		}
	}
}
