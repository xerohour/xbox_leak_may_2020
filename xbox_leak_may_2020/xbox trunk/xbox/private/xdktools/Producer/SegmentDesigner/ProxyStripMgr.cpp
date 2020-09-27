// ProxyStripMgr.cpp : implementation file
//

#include "stdafx.h"
#include "ProxyStripMgr.h"
#include "Timeline.h"
#include <objbase.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include "SegmentDesignerDLL.h"
#include "Segment.h"
#include <initguid.h>
#include <SegmentGuids.h>
#include <dmusicf.h>
#include "SegmentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CProxyStripMgr constructor/destructor 

CProxyStripMgr::CProxyStripMgr()
{
	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pSegment = NULL;
	m_dwRef = 0;
	AddRef();
}

CProxyStripMgr::~CProxyStripMgr()
{
	ASSERT( m_pTimeline == NULL );
	if( m_pSegment )
	{
		// Wasn't addref'd - don't release
		//m_pSegment->Release();
		m_pSegment = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProxyStripMgr IUnknown implementation

HRESULT CProxyStripMgr::QueryInterface( REFIID riid, void** ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdStripMgr)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdStripMgr *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CProxyStripMgr::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	AfxOleLockApp();
    return ++m_dwRef;
}

ULONG CProxyStripMgr::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp();
    --m_dwRef;

    if( m_dwRef == 0 )
    {
		TRACE( "SEGMENT: CProxyStripMgr destroyed!\n" );
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CProxyStripMgr IDMUSProdStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CProxyStripMgr::GetParam

HRESULT STDMETHODCALLTYPE CProxyStripMgr::GetParam(
		/* [in] */	REFGUID 	guidType,
		/* [in] */	MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME* pmtNext,
		/* [out] */ void*		pData)
{
	UNREFERENCED_PARAMETER(mtTime);
	UNREFERENCED_PARAMETER(pmtNext);
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( ::IsEqualGUID( guidType, GUID_Segment_DisplayContextMenu ) )
	{
		return S_OK;
	}
	else if( ::IsEqualGUID( guidType, GUID_TimelineShowRealTime ) )
	{
		return S_OK;
	}
	else if( ::IsEqualGUID( guidType, GUID_Sequence_QuantizeParams ) )
	{
		if( m_pSegment )
		{
			return m_pSegment->GetQuantizeParams( (SequenceQuantize *)pData );
		}
		return E_UNEXPECTED;
	}
	else if( ::IsEqualGUID( guidType, GUID_Sequence_VelocitizeParams ) )
	{
		if( m_pSegment )
		{
			return m_pSegment->GetVelocitizeParams( (SequenceVelocitize *)pData );
		}
		return E_UNEXPECTED;
	}
	else if( ::IsEqualGUID( guidType, GUID_LegacyTemplateActivityLevel ) )
	{
		if( m_pSegment && m_pSegment->m_fLoadedLegacyActivityLevel )
		{
			*(DWORD *)pData = m_pSegment->m_wLegacyActivityLevel;
			return S_OK;
		}
		return E_UNEXPECTED;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CProxyStripMgr::SetParam

HRESULT STDMETHODCALLTYPE CProxyStripMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	UNREFERENCED_PARAMETER(mtTime);

	// Sequence strip quantize params
	if( ::IsEqualGUID( guidType, GUID_Sequence_QuantizeParams ) )
	{
		if( m_pSegment )
		{
			return m_pSegment->SetQuantizeParams( (SequenceQuantize *)pData );
		}
		return E_UNEXPECTED;
	}
	if( ::IsEqualGUID( guidType, GUID_Sequence_VelocitizeParams ) )
	{
		if( m_pSegment )
		{
			return m_pSegment->SetVelocitizeParams( (SequenceVelocitize *)pData );
		}
		return E_UNEXPECTED;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CProxyStripMgr::IsParamSupported

HRESULT STDMETHODCALLTYPE CProxyStripMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	if( ::IsEqualGUID( guidType, GUID_Segment_DisplayContextMenu ) )
	{
		return S_OK;
	}
	if( ::IsEqualGUID( guidType, GUID_TimelineShowRealTime ) )
	{
		return S_OK;
	}
	if( ::IsEqualGUID( guidType, GUID_Sequence_QuantizeParams ) )
	{
		return S_OK;
	}
	if( ::IsEqualGUID( guidType, GUID_Sequence_VelocitizeParams ) )
	{
		return S_OK;
	}
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CProxyStripMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CProxyStripMgr::OnUpdate(
		/* [out] */ REFGUID rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwGroupBits);
	UNREFERENCED_PARAMETER(pData);

	// TimeSig change
	if( ::IsEqualGUID( rguidType, GUID_TimeSignature ) )
	{
		// Reset the length of the Segment
		if( m_pSegment )
		{
			m_pSegment->OnTimeSigChange();
		}
		return S_OK;
	}

	// Time Cursor change or Tempo change
	if( ::IsEqualGUID( rguidType, GUID_TimelineSetCursor )
	||	::IsEqualGUID( rguidType, GUID_TempoParam ) )
	{
		// Update the conductor's tempo control
		if( m_pSegment && m_pSegment->m_fHaveTempoStrip )
		{
			m_pSegment->UpdateConductorTempo();
		}
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CProxyStripMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CProxyStripMgr::GetStripMgrProperty(
		/* [in] */ STRIPMGRPROPERTY stripMgrProperty,
		/* [out] */ VARIANT*	pVariant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pVariant );
	if( !pVariant )
	{
		return E_POINTER;
	}

	switch( stripMgrProperty )
	{
	case SMP_ITIMELINECTL:
		pVariant->vt = VT_UNKNOWN;
		if( m_pTimeline )
		{
			V_UNKNOWN( pVariant ) = m_pTimeline;
			V_UNKNOWN( pVariant )->AddRef();
			return S_OK;
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;

		/* Unimplemented
	case SMP_IDIRECTMUSICTRACK:
	case SMP_IDMUSPRODFRAMEWORK:
	case SMP_DMUSIOTRACKHEADER:
		*/

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProxyStripMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CProxyStripMgr::SetStripMgrProperty(
		/* [in] */ STRIPMGRPROPERTY stripMgrProperty,
		/* [in] */ VARIANT		variant)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	switch( stripMgrProperty )
	{
	case SMP_ITIMELINECTL:
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}
		// If we were previously attached to a timeline, remove our PropPageObject and strip from it
		if( m_pTimeline )
		{
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetCursor, 0xFFFFFFFF );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, 0xFFFFFFFF );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TempoParam, 0xFFFFFFFF );
			m_pTimeline->Release();
			m_pTimeline = NULL;
		}

		if( V_UNKNOWN( &variant ) )
		{
			if( FAILED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pTimeline )))
			{
				return E_FAIL;
			}
			else
			{
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TempoParam, 0xFFFFFFFF );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, 0xFFFFFFFF );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimelineSetCursor, 0xFFFFFFFF );
			}
		}
		break;

		/* Unimplemented
	case SMP_IDIRECTMUSICTRACK:
	case SMP_IDMUSPRODFRAMEWORK:
	case SMP_DMUSIOTRACKHEADER:
		*/

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}
