// ChordMgr.cpp : Implementation of CChordMgr
#include "stdafx.h"
#include "ChordIO.h"
#include "ChordStripMgr.h"
#include "ChordMgr.h"
#include "RiffStrm.h"
#include <dmusici.h>
#include <dmusicf.h>
#include "chordstripriff.h"
#include "SegmentGuids.h"
#include "SegmentIO.h"
#include <ChordMapDesigner.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CChordMgr

CChordMgr::CChordMgr()
{
	m_pTimeline = NULL;
	m_pIDMUSProdFramework = NULL;
	m_pPropertyPage = NULL;
	m_pPropPageMgr = NULL;
	m_pCopyDataObject = NULL;
	m_pChordStrip = NULL;
	m_dwGroupBits = 1;
	m_dwOldGroupBits = 1;
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
	m_dwProducerOnlyFlags = 0;
	m_pIDMTrack = NULL;
	m_fUseChordStripPPG = TRUE;
	m_fNeedChordMeasure1Beat1 = FALSE;
	m_bPatternEditorMode = 0xFF;
	m_bKey = 0;
	m_dwScale = 0xAB5AB5;	
	m_fDisplayingFlats = FALSE;
	m_nKeyRoot = 0;
	m_nNumAccidentals = 0;
	m_fLockAllScales = true;
	m_fSyncLevelOneToAll = true;
	m_pChordStrip = new CChordStrip(this);
	ASSERT( m_pChordStrip );
}

CChordMgr::~CChordMgr()
{
	if( m_pIDMUSProdFramework )
	{
		m_pIDMUSProdFramework->Release();
	}
	m_ChordList.ClearList();
	ASSERT( m_pTimeline == NULL );
	ASSERT( m_pPropertyPage == NULL );
	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->Release();
	}
	if( m_pIDMTrack )
	{
		m_pIDMTrack->Release();
	}
	if( m_pChordStrip )
	{
		m_pChordStrip->Release();
	}
	if( m_pCopyDataObject )
	{
		if( S_OK == OleIsCurrentClipboard( m_pCopyDataObject ))
		{
			OleFlushClipboard();
		}
		m_pCopyDataObject->Release();
		m_pCopyDataObject = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChordMgr IDMUSProdStripMgr

HRESULT STDMETHODCALLTYPE CChordMgr::GetParam(
		/* [in] */  REFGUID		guidType,
		/* [in] */  MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME*	pmtNext,
		/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pData != NULL );
	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( ::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) )
	{
		BSTR bstr;
		CString str;
		str.LoadString(m_pChordStrip->m_nLastEdit);
		try
		{
			bstr = str.AllocSysString();
		}
		catch(CMemoryException*)
		{
			return E_OUTOFMEMORY;
		}
		*(BSTR*)pData = bstr;
		return S_OK;
	}

	else if( ::IsEqualGUID( guidType, GUID_PatternEditorMode ) )
	{
		BYTE* pPatternEditorMode = (BYTE *)pData;

		*pPatternEditorMode= m_bPatternEditorMode;
		return S_OK;
	}

	else if( ::IsEqualGUID( guidType, GUID_ChordParam ) )
	{

		HRESULT hr = E_FAIL;
		long lChordMeasure = 0;
		long lChordBeat = 0;
		// convert mtTime into Measure/Beat
		CMusicTimeConverter cmt(mtTime);

		if(m_pTimeline)
		{
			hr = cmt.GetMeasureBeat(lChordMeasure, lChordBeat, m_pTimeline, m_dwGroupBits, 0);
		}
		else
		{
			hr = cmt.GetMeasureBeat(lChordMeasure, lChordBeat, 4, 4);
		}


		CChordItem* pTheChord = m_ChordList.GetHead();
		CChordItem* pNext = pTheChord ? pTheChord->GetNext() : NULL;

		while( pNext )
		{
			if( pNext->Measure() > lChordMeasure )
			{
				break;
			}

			if( pNext->Measure() == lChordMeasure
			&&  pNext->Beat() > lChordBeat )
			{
				break;
			}

			pTheChord = pNext;
			pNext = pNext->GetNext();
		}

		if( pTheChord )
		{
			DMUS_CHORD_PARAM* pchordData = (DMUS_CHORD_PARAM *)pData;

			// Set key/scale of chord strip
			pchordData->bKey = m_bKey;
			pchordData->dwScale = m_dwScale;

			// Set chord information
	        MultiByteToWideChar( CP_ACP, 0, pTheChord->Name(), -1, 
				pchordData->wszName, sizeof( pchordData->wszName ) / sizeof( wchar_t ) );
			pchordData->wMeasure = pTheChord->Measure();
			pchordData->bBeat = pTheChord->Beat();
			pchordData->bSubChordCount = DMPolyChord::MAX_POLY;


			for( int i=0 ;  i<DMPolyChord::MAX_POLY ;  i++ )
			{
				pchordData->SubChordList[i].dwChordPattern 
					= dynamic_cast<DMPolyChord*>(pTheChord)->SubChord(i)->ChordPattern();
				pchordData->SubChordList[i].dwScalePattern 
					= dynamic_cast<DMPolyChord*>(pTheChord)->SubChord(i)->ScalePattern();
				pchordData->SubChordList[i].dwInversionPoints 
					= dynamic_cast<DMPolyChord*>(pTheChord)->SubChord(i)->InvertPattern();
				pchordData->SubChordList[i].dwLevels 
					= dynamic_cast<DMPolyChord*>(pTheChord)->SubChord(i)->Levels();
				pchordData->SubChordList[i].bChordRoot 
					= dynamic_cast<DMPolyChord*>(pTheChord)->SubChord(i)->ChordRoot();
				pchordData->SubChordList[i].bScaleRoot 
					= dynamic_cast<DMPolyChord*>(pTheChord)->SubChord(i)->ScaleRoot();
			}

			if( pmtNext )
			{
				if( pNext )
				{
					CMusicTimeConverter cmtNext;
					if(m_pTimeline)
					{
						cmtNext.SetTime(pNext->Measure(), pNext->Beat(), m_pTimeline, m_dwGroupBits, 0);
					}
					else
					{
						cmtNext.Time() = pNext->Time();
					}
					*pmtNext = cmtNext.Time() - mtTime;
				}
				else
				{
					*pmtNext = 0;
				}
			}

			return S_OK;
		}
		else
		{
			return E_FAIL;
		}

	}

	// Get overall key
	else if( ::IsEqualGUID( guidType, GUID_ChordKey ) )
	{
		DWORD* pdwKey = (DWORD *)pData;

		ASSERT( m_nKeyRoot >= 0 );
		ASSERT( m_nNumAccidentals >= 0);
		*pdwKey = (m_nKeyRoot & 0x0000FFFF) | ((m_nNumAccidentals & 0x0000FFFF) << 16);
		return S_OK;
	}

	// Get sharps/flats flag
	else if( ::IsEqualGUID( guidType, GUID_ChordSharpsFlats ) )
	{
		BOOL* pfDisplayingFlats = (BOOL *)pData;

		*pfDisplayingFlats= m_fDisplayingFlats;
		return S_OK;
	}
	// get sharps/flats flag for an individual chord
	else if(::IsEqualGUID( guidType, GUID_ChordIndividualChordSharpsFlats))
	{
		HRESULT hr = E_FAIL;
		long lMeasure = 0;
		long lBeat = 0;
		// convert mtTime into Measure/Beat
		CMusicTimeConverter cmt(mtTime);

		if(m_pTimeline)
		{
			hr = cmt.GetMeasureBeat(lMeasure, lBeat, m_pTimeline, m_dwGroupBits, 0);
		}
		else
		{
			hr = cmt.GetMeasureBeat(lMeasure, lBeat, 4, 4);
		}

		BOOL* pfDisplayingFlats = (BOOL *)pData;
		for(CChordItem* pChord = m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
		{
			if(pChord->Measure() == lMeasure && pChord->Beat() == lBeat)
			{
				*pfDisplayingFlats = pChord->SubChord(pChord->RootIndex())->UseFlat();
				return S_OK;
			}
		}
		return E_INVALIDARG;
	}
/* not necessary in a get
	if( m_pTimeline )
	{
		m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)m_pChordStrip, NULL, TRUE );
	}
*/

	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CChordMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = E_INVALIDARG;

	if( pData == NULL )
	{
		return E_POINTER;
	}

	if( ::IsEqualGUID( guidType, GUID_UseGroupBitsPPG ) )
	{
		BOOL* pUseGroupBitsPPG = (BOOL *)pData;

		m_fUseChordStripPPG = *pUseGroupBitsPPG;
		hr = S_OK;
	}

	else if( ::IsEqualGUID( guidType, GUID_PatternEditorMode ) )
	{
		BYTE* pPatternEditorMode = (BYTE *)pData;

		m_bPatternEditorMode = *pPatternEditorMode;
		hr = S_OK;
	}

	else if( ::IsEqualGUID( guidType, GUID_NeedChordMeasure1Beat1 ) )
	{
		BOOL* pNeedChordMeasure1Beat1 = (BOOL *)pData;

		m_fNeedChordMeasure1Beat1 = *pNeedChordMeasure1Beat1;
		hr = S_OK;
	}

	else if( ::IsEqualGUID( guidType, GUID_ChordParam))
	{
		long lMeasure = 0;
		long lBeat = 0;
		// convert mtTime into Measure/Beat
		CMusicTimeConverter cmt(mtTime);

		if(m_pTimeline)
		{
			hr = cmt.GetMeasureBeat(lMeasure, lBeat, m_pTimeline, m_dwGroupBits, 0);
		}
		else
		{
			hr = cmt.GetMeasureBeat(lMeasure, lBeat, 4, 4);
		}

		CChordItem* pChord = new CChordItem;
		DMUS_CHORD_PARAM* pChordParam = (DMUS_CHORD_PARAM*)pData;

		memset(pChord, 0, sizeof(CChordItem));

		WideCharToMultiByte(CP_ACP, 0, pChordParam->wszName, -1, pChord->Name(), DMPolyChord::MAX_NAME, NULL, NULL);

		pChord->Measure() = static_cast<WORD>(lMeasure);
		pChord->Beat() = static_cast<BYTE>(lBeat);
		ASSERT(pChordParam->bSubChordCount <= DMPolyChord::MAX_POLY);
		ASSERT(pChordParam->bSubChordCount > 0);

		if(pChordParam->bSubChordCount > DMPolyChord::MAX_POLY || pChordParam->bSubChordCount == 0)
		{
			delete pChord;
			return E_INVALIDARG;
		}

		for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
		{
			if(i >= pChordParam->bSubChordCount)
			{
				pChord->SubChord(i)->UseFlat() = 0;
				pChord->SubChord(i)->Levels() = 1 << i;
				pChord->SubChord(i)->ChordPattern() = pChord->SubChord(0)->ChordPattern();
				pChord->SubChord(i)->ChordRoot() = pChord->SubChord(0)->ChordRoot();
				pChord->SubChord(i)->ScalePattern() = pChord->SubChord(0)->ScalePattern();
				pChord->SubChord(i)->ScaleRoot() = pChord->SubChord(0)->ScaleRoot();
				pChord->SubChord(i)->InvertPattern() = pChord->SubChord(0)->InvertPattern();
			}
			else
			{
				pChord->SubChord(i)->UseFlat() = 0;
				pChord->SubChord(i)->Levels() = pChordParam->SubChordList[i].dwLevels;
				pChord->SubChord(i)->ChordPattern() = pChordParam->SubChordList[i].dwChordPattern;
				pChord->SubChord(i)->ChordRoot() = pChordParam->SubChordList[i].bChordRoot;
				pChord->SubChord(i)->ScalePattern() = pChordParam->SubChordList[i].dwScalePattern;
				pChord->SubChord(i)->ScaleRoot() = pChordParam->SubChordList[i].bScaleRoot;
				pChord->SubChord(i)->InvertPattern() = pChordParam->SubChordList[i].dwInversionPoints;
			}
		}
		m_ChordList.InsertByAscendingTime(pChord);
		hr = S_OK;
	}
	// Change overall key
	else if( ::IsEqualGUID( guidType, GUID_ChordKey ) )
	{
		DWORD dwKey = *(DWORD *)pData;
		m_nKeyRoot = dwKey & 0xFFFF;
		m_nNumAccidentals = (dwKey >> 16) & 0xFFFF;
		UpdateMasterScaleAndKey();
		hr =  S_OK;
	}
	// change sharps/flats flag
	else if( ::IsEqualGUID( guidType, GUID_ChordSharpsFlats ) )
	{
		m_fDisplayingFlats = *(BOOL *)pData;
		UpdateMasterScaleAndKey();
		hr = S_OK;
	}
	// change sharps/flats flag in an individual chord
	else if(::IsEqualGUID( guidType, GUID_ChordIndividualChordSharpsFlats))
	{
		long lMeasure = 0;
		long lBeat = 0;
		// convert mtTime into Measure/Beat
		CMusicTimeConverter cmt(mtTime);

		if(m_pTimeline)
		{
			hr = cmt.GetMeasureBeat(lMeasure, lBeat, m_pTimeline, m_dwGroupBits, 0);
		}
		else
		{
			hr = cmt.GetMeasureBeat(lMeasure, lBeat, 4, 4);
		}

		BOOL fDisplayingFlats = *(BOOL *)pData;
		for(CChordItem* pChord = m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
		{
			if(pChord->Measure() == lMeasure && pChord->Beat() == lBeat)
			{
				for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
				{
					pChord->SubChord(i)->UseFlat() = fDisplayingFlats;
				}
				hr = S_OK;
				break;
			}
		}
		hr = E_INVALIDARG;
	}

	if( m_pTimeline )
	{
		m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)m_pChordStrip, NULL, TRUE );
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE CChordMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	if( ::IsEqualGUID( guidType, GUID_ChordParam )
	||  ::IsEqualGUID( guidType, GUID_PatternEditorMode )
	||  ::IsEqualGUID( guidType, GUID_ChordKey )
	||  ::IsEqualGUID( guidType, GUID_ChordSharpsFlats )
	||	::IsEqualGUID( guidType, GUID_Segment_Undo_BSTR ) 
	||  ::IsEqualGUID( guidType, GUID_ChordIndividualChordSharpsFlats))
	{
		return S_OK;
	}
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CChordMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(dwGroupBits);
	UNREFERENCED_PARAMETER(pData);

	// TimeSig change
	if( ::IsEqualGUID( rguidType, GUID_TimeSignature ) )
	{
		RecalculateMusicTimeValues();
		if( m_pTimeline )
		{
			m_pTimeline->OnDataChanged( (IDMUSProdStripMgr *)this );
		}
		UpdateDirectMusic( FALSE );
		return S_OK;
	}

	// All tracks added
	else if( ::IsEqualGUID( rguidType, GUID_Segment_AllTracksAdded ) )
	{
		RecalculateMusicTimeValues();
		UpdateDirectMusic( FALSE );
		return S_OK;
	}

	// Pattern track group bits change
	else if( ::IsEqualGUID( rguidType, CLSID_DirectMusicPatternTrack ) )
	{
		// Check if there are any MIDIStripMgrs in m_dwGroupBits
		IDMUSProdStripMgr *pStripMgr;
		if( SUCCEEDED( m_pTimeline->GetStripMgr( CLSID_DirectMusicPatternTrack, m_dwGroupBits, 0, &pStripMgr ) ) )
		{
			pStripMgr->Release();
		}
		else
		{
			// No MIDIStripMgrs, reset the CHORDSTRIP_MODE to nothing
			m_bPatternEditorMode = 0xFF;

			// Redraw the strip
			m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)m_pChordStrip, NULL, TRUE );
		}
		return S_OK;
	}

	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CChordMgr::GetStripMgrProperty(
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

	case SMP_IDIRECTMUSICTRACK:
		pVariant->vt = VT_UNKNOWN;
		if( m_pIDMTrack )
		{
			return m_pIDMTrack->QueryInterface( IID_IUnknown, (void**)&V_UNKNOWN( pVariant ) );
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;

	case SMP_IDMUSPRODFRAMEWORK:
		pVariant->vt = VT_UNKNOWN;
		if( m_pIDMUSProdFramework )
		{
			return m_pIDMUSProdFramework->QueryInterface( IID_IUnknown, (void**)&V_UNKNOWN( pVariant ) );
		}
		else
		{
			V_UNKNOWN( pVariant ) = NULL;
			return E_FAIL;
		}
		break;

	case SMP_DMUSIOTRACKHEADER:
		if( pVariant->vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_HEADER *pioTrackHeader = static_cast<DMUS_IO_TRACK_HEADER *>(V_BYREF( pVariant ));
			if( pioTrackHeader == NULL )
			{
				return E_POINTER;
			}

			pioTrackHeader->guidClassID = CLSID_DirectMusicChordTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = NULL;
			pioTrackHeader->fccType = DMUS_FOURCC_CHORDTRACK_LIST;
		}
		break;

	case SMP_DMUSIOTRACKEXTRASHEADER:
		if( pVariant->vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_EXTRAS_HEADER *pioTrackExtrasHeader = static_cast<DMUS_IO_TRACK_EXTRAS_HEADER *>(V_BYREF( pVariant ));
			if( pioTrackExtrasHeader == NULL )
			{
				return E_POINTER;
			}

			pioTrackExtrasHeader->dwFlags = m_dwTrackExtrasFlags;
		}
		break;

	case SMP_DMUSIOTRACKEXTRASHEADER_MASK:
		pVariant->vt = VT_I4;
		V_I4(pVariant) = TRACKCONFIG_VALID_MASK;
		break;

	case SMP_PRODUCERONLY_FLAGS:
		if( pVariant->vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			IOProducerOnlyChunk *pioProducerOnlyChunk = static_cast<IOProducerOnlyChunk *>(V_BYREF( pVariant ));
			if( pioProducerOnlyChunk == NULL )
			{
				return E_POINTER;
			}

			pioProducerOnlyChunk->dwProducerOnlyFlags = m_dwProducerOnlyFlags;
		}
		break;

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMgr::SetStripMgrProperty(
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
			m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);
			if( m_pChordStrip )
			{
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)m_pChordStrip);
				m_pTimeline->RemoveStrip( (IDMUSProdStrip *)m_pChordStrip );
			}
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, CLSID_DirectMusicPatternTrack, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwOldGroupBits );
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
//				RecalculateMusicTimeValues();
//				UpdateDirectMusic( FALSE );

				m_pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)m_pChordStrip, CLSID_DirectMusicChordTrack, m_dwGroupBits, 0 );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, CLSID_DirectMusicPatternTrack, m_dwGroupBits );
			}
		}
		break;

	case SMP_IDIRECTMUSICTRACK:
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}
		if( m_pIDMTrack )
		{
			m_pIDMTrack->Release();
		}
		if( V_UNKNOWN( &variant ) )
		{
			V_UNKNOWN( &variant )->QueryInterface( IID_IDirectMusicTrack, (void**)&m_pIDMTrack );
		}
		else
		{
			m_pIDMTrack = NULL;
		}
		break;

	case SMP_IDMUSPRODFRAMEWORK:
		if( variant.vt != VT_UNKNOWN )
		{
			return E_INVALIDARG;
		}
		if( m_pIDMUSProdFramework )
		{
			m_pIDMUSProdFramework->Release();
			m_pIDMUSProdFramework = NULL;
		}
		if( V_UNKNOWN( &variant ) )
		{
			return V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pIDMUSProdFramework);
		}
		break;

	case SMP_DMUSIOTRACKHEADER:
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_HEADER *pioTrackHeader = static_cast<DMUS_IO_TRACK_HEADER *>(V_BYREF( &variant ));
			if( pioTrackHeader == NULL )
			{
				return E_POINTER;
			}
			m_dwGroupBits = pioTrackHeader->dwGroup;
			m_dwOldGroupBits = pioTrackHeader->dwGroup;
		}
		break;

	case SMP_DMUSIOTRACKEXTRASHEADER:
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			DMUS_IO_TRACK_EXTRAS_HEADER *pioTrackExtrasHeader = static_cast<DMUS_IO_TRACK_EXTRAS_HEADER *>(V_BYREF( &variant ));
			if( pioTrackExtrasHeader == NULL )
			{
				return E_POINTER;
			}

			m_dwTrackExtrasFlags = TRACKCONFIG_VALID_MASK & (pioTrackExtrasHeader->dwFlags);
		}
		break;

	case SMP_PRODUCERONLY_FLAGS:
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		else
		{
			IOProducerOnlyChunk *pioProducerOnlyChunk = static_cast<IOProducerOnlyChunk *>(V_BYREF( &variant ));
			if( pioProducerOnlyChunk == NULL )
			{
				return E_POINTER;
			}

			m_dwProducerOnlyFlags = pioProducerOnlyChunk->dwProducerOnlyFlags;
		}
		break;

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CChordMgr IPersist

HRESULT CChordMgr::GetClassID( CLSID* pClsId )
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CChordMgr IPersistStream functions

HRESULT CChordMgr::IsDirty()
{
	return S_FALSE;
}

HRESULT CChordMgr::Load( IStream* pStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pStream == NULL )
	{
		ImportKeyFromChordTrack();
		return ImportChordsFromChordTrack();
	}

	IDMUSProdRIFFStream*	pRiffStream;
    MMCKINFO				ck;
	HRESULT					hr;
	if( FAILED( hr = AllocRIFFStream( pStream, &pRiffStream ) ) )
	{
		return hr;
	}

	// Load the Track
	while( pRiffStream->Descend( &ck, NULL, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
		case DMUS_FOURCC_CHORD_DESIGN_CHUNK:
			{
				ioDMChordDesignChunk iDesignChunk;

				DWORD cbRead, dwSize = min ( ck.cksize, sizeof(ioDMChordDesignChunk) );
				if( SUCCEEDED( pStream->Read( &iDesignChunk, dwSize, &cbRead) ) 
				 && (cbRead == dwSize) )
				{
					m_fDisplayingFlats = (iDesignChunk.m_bDisplayingFlats != 0);
					m_nKeyRoot = iDesignChunk.m_bKeyRoot;
					m_nNumAccidentals = iDesignChunk.m_bNumAccidentals;
					m_fLockAllScales = iDesignChunk.m_fLockAllScales;
					m_fSyncLevelOneToAll = iDesignChunk.m_fSyncLevelOneToAll;
					// make all chord display or not display flats according to m_fDisplayingFlats
					// no -- this is a load, chords are displayed according to individual settings
//					m_ChordList.UseFlats(static_cast<bool>(m_fDisplayingFlats));
				}
			}
			break;

		case FOURCC_LIST:
			switch( ck.fccType )
			{
			// found the chord list chunk
			case DMUS_FOURCC_CHORDTRACK_LIST:
				{
					// read the chordlist (this also clears m_ChordList)
					ChordListChunk	chordlist(&m_ChordList);
					hr = chordlist.Read(pRiffStream, &ck);
					if(hr == S_OK)
					{
						if(chordlist.Scale() == 0)
						{
							m_dwScale = DefaultScale;
							m_bKey = DefaultKey;
							chordlist.Scale() = (m_bKey << 24) + m_dwScale;
						}
						else
						{
							m_dwScale = chordlist.Scale() & 0x00FFFFFF;
							m_bKey = (BYTE)(chordlist.Scale() >> 24);
						}
						// Initialize m_nKeyRoot and m_nNumAccidentals
						UpdateKeyRootAndAccidentals();
						UpdateDirectMusic( FALSE );
						hr = pRiffStream->Ascend(&ck, 0) ;
					}
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
	pRiffStream->Release();
	if( m_pTimeline )
	{
		m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)m_pChordStrip, NULL, TRUE );
	}
	return hr;
}

HRESULT CChordMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = S_OK;
	DMUSProdStreamInfo	StreamInfo;//Added ECW 4/24/98
	FileType	ftFileType;
	GUID		guidDataFormat;

	RecalculateMusicTimeValues();

	// We want to know what type of save we are doing because we
	// don't want to save UI information in Personality 'Release' files...	
	IDMUSProdPersistInfo *pIDMUSProdPersist = NULL;
	if( FAILED( hr = pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void**)&pIDMUSProdPersist ) ) )
	{
		ftFileType = FT_RUNTIME;
		guidDataFormat = GUID_CurrentVersion;
//		hr = S_OK;
		return E_NOTIMPL;
	}
	else
	{//Changed ECW 4/24/98
		hr = pIDMUSProdPersist->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pIDMUSProdPersist->Release();
		if (FAILED( hr ))
		{
			return hr;
		}
	}

	IDMUSProdRIFFStream* pRiffStream;
	hr = AllocRIFFStream(pIStream, &pRiffStream);
	if(FAILED(hr))
	{
		return hr;
	}

	// save track
	ChordListChunk chordlistchunk(&m_ChordList, m_dwScale + (m_bKey << 24),(ftFileType==FT_DESIGN));
	hr = chordlistchunk.Write(pRiffStream);

	if( (ftFileType == FT_DESIGN) && SUCCEEDED(hr) )
	{
		// Write the Design-time chunk
	    MMCKINFO ckDesign;
		ckDesign.ckid = DMUS_FOURCC_CHORD_DESIGN_CHUNK;
		if( pRiffStream->CreateChunk( &ckDesign, 0 ) == 0 )
		{
			ioDMChordDesignChunk oDesignChunk;
			ZeroMemory( &oDesignChunk, sizeof(ioDMChordDesignChunk) );
			oDesignChunk.m_bDisplayingFlats = m_fDisplayingFlats;
			oDesignChunk.m_bKeyRoot = (BYTE)m_nKeyRoot;
			oDesignChunk.m_bNumAccidentals = (BYTE)m_nNumAccidentals;
			oDesignChunk.m_fLockAllScales = m_fLockAllScales;
			oDesignChunk.m_fSyncLevelOneToAll = m_fSyncLevelOneToAll;

			DWORD cbWritten;
			hr = pIStream->Write( &oDesignChunk, sizeof(ioDMChordDesignChunk), &cbWritten );
			if( SUCCEEDED(hr) && (cbWritten != sizeof(ioDMChordDesignChunk)) )
			{
				hr = E_FAIL;
			}

			pRiffStream->Ascend( &ckDesign, 0 );
		}
	}

	pRiffStream->Release();
	return hr;
}

HRESULT CChordMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject functions

HRESULT STDMETHODCALLTYPE CChordMgr::GetData( /* [retval][out] */ void **ppData )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CChordScalePropPageData* pChordScalePropPageData = (CChordScalePropPageData *)*ppData;

	pChordScalePropPageData->m_pPropChord = &m_SelectedChord;
	pChordScalePropPageData->m_fLockAllScales = m_fLockAllScales;
	pChordScalePropPageData->m_fSyncLevelOneToAll = m_fSyncLevelOneToAll;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CChordScalePropPageData* pChordScalePropPageData = (CChordScalePropPageData *)pData;
	CPropChord* pChord = pChordScalePropPageData->m_pPropChord;

	TRACE("\r\n invert masks: %x %x %x %x\r\n", 
			pChord->SubChord(0)->InvertPattern(),
			pChord->SubChord(1)->InvertPattern(),
			pChord->SubChord(2)->InvertPattern(),
			pChord->SubChord(3)->InvertPattern());

	BOOL fRefresh = FALSE;

	if( pChordScalePropPageData->m_fLockAllScales != m_fLockAllScales )
	{
		fRefresh = TRUE;
		m_pChordStrip->m_nLastEdit = IDS_UNDO_LOCK_SCALES;
	}
	else if( pChordScalePropPageData->m_fSyncLevelOneToAll != m_fSyncLevelOneToAll )
	{
		fRefresh = TRUE;
		m_pChordStrip->m_nLastEdit = IDS_UNDO_SYNC_LEVELS;
	}
	else
	{
		fRefresh = memcmp(pChord, &m_SelectedChord, sizeof(CPropChord));
		if( fRefresh )
		{
			m_pChordStrip->m_nLastEdit = IDS_UNDO_CHANGE;
		}
	}

	m_SelectedChord = *pChord;
	m_fLockAllScales = pChordScalePropPageData->m_fLockAllScales;
	m_fSyncLevelOneToAll = pChordScalePropPageData->m_fSyncLevelOneToAll;

	m_ChordList.RetrievePropChord( pChord );
	m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)m_pChordStrip, NULL, TRUE );
	if(fRefresh)
	{
		// Let the object know about the changes
		UpdateDirectMusic( TRUE );
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMgr::OnShowProperties( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;
	ASSERT(m_pIDMUSProdFramework);
	if( m_pPropPageMgr == NULL )
	{
		CChordPropPageMgr* pPPM = new CChordPropPageMgr(m_pIDMUSProdFramework);
		if( NULL == pPPM ) return E_OUTOFMEMORY;
		hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pPropPageMgr );
		m_pPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
		if( FAILED(hr) )
			return hr;
	}
	m_pTimeline->SetPropertyPage(m_pPropPageMgr, (IDMUSProdPropPageObject*)this);
	return hr;
}

HRESULT STDMETHODCALLTYPE CChordMgr::OnRemoveFromPageManager( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return S_OK;
}

HRESULT CChordMgr::UpdateDirectMusic( BOOL fUpdateSegmentDesigner )
{
	HRESULT hr = E_FAIL;
	if( m_pTimeline && fUpdateSegmentDesigner )
	{
		m_pTimeline->OnDataChanged( (IDMUSProdStripMgr *)this );
		m_pTimeline->NotifyStripMgrs( GUID_ChordParam, m_dwGroupBits, NULL );
	}

	if( m_pIDMTrack && m_pIDMUSProdFramework )
	{
		IPersistStream* pIPersistStream;
		if( SUCCEEDED( m_pIDMTrack->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
		{
			IStream *pIStream;
			if( SUCCEEDED( m_pIDMUSProdFramework->AllocMemoryStream( FT_RUNTIME, GUID_DirectMusicObject, &pIStream ) ) )
			{
				if( SUCCEEDED( Save( pIStream, FALSE ) ) )
				{
					LARGE_INTEGER liTemp;
					liTemp.QuadPart = 0;
					if( SUCCEEDED( pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL ) ) )
					{
						hr = pIPersistStream->Load( pIStream );
					}
				}
				pIStream->Release();
			}
			pIPersistStream->Release();
		}
	}
	return hr;
}

////////////////////// methods to support drag and drop
HRESULT CChordMgr::DeleteSelectedChords()
{
	CChordList list;
	CChordItem* pChord;

	// get rid of all selected chords
	while( pChord = m_ChordList.RemoveHead() )
	{
		if( pChord->IsSelected() )
		{
			delete pChord;
		}
		else
		{
			pChord->SetNext(0);
			list.AddTail(pChord);
		}
	}

	// write back remaining runs
	while( pChord = list.RemoveHead())
	{
		pChord->SetNext(0);
		m_ChordList.AddTail(pChord);
	}
	return S_OK;
}



HRESULT CChordMgr::SaveSelectedChords(LPSTREAM pStream, CChordItem* pChordAtDragPoint, 
									  BOOL bNormalize, long* pOffset)
{
	ASSERT(pStream);
	ASSERT(pChordAtDragPoint);
	CChordList list;
	CChordItem* pChord;
	// copy selected chords to list
	for(pChord = m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->IsSelected())
		{
			// make sure that only the chord at the mouse drag point is selected
			pChord->SetSelected( (pChord == pChordAtDragPoint) ? TRUE : FALSE);
			list.AddTail(new CChordItem(*pChord));
			// re-select chord
			pChord->SetSelected(TRUE);
		}
	}
	// check that anything is selected
	if(!list.GetHead())
		return S_FALSE; // Nothing selected
	// normalize chords so that first chord is at offset 0
	if(bNormalize)
	{
		pChord = list.GetHead();
		CMusicTimeConverter cmt(pChord->Measure(), pChord->Beat(), m_pTimeline, m_dwGroupBits);
		NormalizeChordList(list, -cmt);
		if(pOffset)
		{
			*pOffset = cmt.Time();	// save for later use
		}

	}
	HRESULT hr = list.Save(pStream, FALSE);
	list.ClearList();
	return hr;
}

HRESULT CChordMgr::GetTimeSig(long* top, long* bottom, long mtTime)
{
	if(!m_pTimeline)
		return E_FAIL;

	DMUS_TIMESIGNATURE dmTimeSig;

	if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, mtTime, NULL, &dmTimeSig ) ) )
	{
		if( top )
		{
			*top = dmTimeSig.bBeatsPerMeasure;
		}
		if( bottom )
		{
			*bottom = dmTimeSig.bBeat;
		}
		return S_OK;
	}
	return E_FAIL;
}



HRESULT CChordMgr::NormalizeChordList(CChordList& list, long clockOffset, long* pStartClocks, long* pEndClocks)
//
// normalize using strip's timesig, also returns min/max measure/beat of list if user asks for it
//
{
	HRESULT hr = S_OK;
	CMusicTimeConverter cmtMin(0x7fffffff);
	CMusicTimeConverter cmtMax(0);
	
	ASSERT(m_pTimeline);
	if(!m_pTimeline)
		return E_UNEXPECTED;

	for(CChordItem* pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
	{
		long m,b;
		CMusicTimeConverter cmt(pChord->Measure(), pChord->Beat(), m_pTimeline, m_dwGroupBits);
		// convert min/max
		if(cmt < cmtMin)
			cmtMin = cmt;
		if(cmtMax < cmt)
			cmtMax = cmt;
		// offset chord
		cmt = cmt + clockOffset;
		hr = cmt.GetMeasureBeat(m,b, m_pTimeline, m_dwGroupBits);
		if(FAILED(hr))
		{
			break;
		}
		pChord->Measure() = static_cast<short>(m);
		pChord->Beat() = static_cast<BYTE>(b);
	}
	if(SUCCEEDED(hr))
	{
		// indicate min/max
		if(pStartClocks)
			*pStartClocks = cmtMin;
		if(pEndClocks)
			*pEndClocks = cmtMax;
	}
	return hr;
}

void CChordMgr::MarkSelectedChords(DWORD flags)
{
	for(CChordItem* pChord = m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->IsSelected())
		{
			for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
			{
				pChord->Undetermined(i) |= flags;
			}
		}
	}
}

void CChordMgr::DeleteMarkedChords(DWORD flags)
{
	CChordList list;
	CChordItem* pChord;
	// get rid of all marked chords
	while( pChord = m_ChordList.RemoveHead() )
	{
		if(pChord->Undetermined(pChord->RootIndex()) & flags)
		{
			delete pChord;
		}
		else
		{
			pChord->SetNext(0);
			list.AddTail(pChord);
		}
	}

	// write back remaining runs
	while( pChord = list.RemoveHead())
	{
		pChord->SetNext(0);
		m_ChordList.AddTail(pChord);
	}
}

void CChordMgr::UnMarkChords(DWORD flags)
{

	for(CChordItem* pChord = m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
	{
			for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
			{
				pChord->Undetermined(i) &= ~flags;
			}
	}
}

void CChordMgr::UnselectChords()
{
	m_ChordList.ClearSelections();
}


bool CChordMgr::IsSelected()
{
	for(CChordItem* pChord = m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->IsSelected())
			return true;
	}
	return false;
}


short CChordMgr::GetSelectedHideState()
{
	short nHideState = HIDE_NONE_SELECTED;

	for( CChordItem* pChord = m_ChordList.GetHead();  pChord;  pChord = pChord->GetNext() )
	{
		if( pChord->IsSelected() )
		{
			if( pChord->Flags() & DMUS_CHORDKEYF_SILENT )
			{
				if( nHideState == HIDE_ALL_OFF )
				{
					nHideState = HIDE_MIXED;
					break;
				}
				nHideState = HIDE_ALL_ON;
			}
			else
			{
				if( nHideState == HIDE_ALL_ON )
				{
					nHideState = HIDE_MIXED;
					break;
				}
				nHideState = HIDE_ALL_OFF;
			}
		}
	}

	return nHideState;
}


short CChordMgr::HowManySelected()
{
	short nHowMany = SEL_NONE;

	for( CChordItem* pChord = m_ChordList.GetHead();  pChord;  pChord = pChord->GetNext() )
	{
		if( pChord->IsSelected() )
		{
			if( nHowMany )
			{
				return SEL_MULTIPLE;
			}

			nHowMany = SEL_ONE;
		}
	}

	return nHowMany;
}

CChordItem* CChordMgr::FirstSelectedChord()
{
	CChordItem* pChord = m_ChordList.GetHead();
	bool found = false;
	for(;pChord && !found; pChord = pChord->GetNext())
	{
		if(pChord->IsSelected())
		{
			found = true;
			break;
		}
	}
	if(found)
	{
		return pChord;
	}
	else
	{
		return NULL;
	}
}

BOOL CChordMgr::SelectSegment(long begintime, long endtime)
//
// returns true if anything was selected, otherwise returns false
//
{
	ASSERT(m_pTimeline);
	ASSERT(m_pChordStrip);
	if(!m_pTimeline || !m_pChordStrip)
	{
		return FALSE;
	}
	if( begintime == -1)
	{
		// till we implement m_lShiftFromMeasure (see CommandMgr::SelectSegment),
		// just set it to beginning
		begintime = 0;
	}

	CMusicTimeConverter cmtBeg(begintime);
	CMusicTimeConverter cmtEnd(endtime);
	CSelectedRegion region(cmtBeg, cmtEnd);

	m_pChordStrip->m_pSelectedRegions->AddRegion(region);
	m_pChordStrip->SelectChordsInSelectedRegions();
/*
	for(CChordItem* pChord = m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
	{
		long measure = static_cast<long>(pChord->Measure()) & 0xffff;
		long beat = static_cast<long>(pChord->Beat()) & 0xff;
		long clocks;
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, measure, beat, &clocks);
		if(begintime <= clocks && clocks < endtime)
		{
			pChord->SetSelected(TRUE);
			m_SelectedChord = *pChord;
			result = TRUE;
		}
		else
		{
			pChord->SetSelected(FALSE);
		}
	}
*/
	return TRUE;
}

HRESULT CChordMgr::ImportKeyFromChordTrack( void )
{
	if( m_pTimeline == NULL )
	{
		return E_FAIL;
	}

	// Get the ChordMap's DocRoot
	MUSIC_TIME mtTime;
	IDMUSProdNode* pIPersonalityNode;
	if( SUCCEEDED( m_pTimeline->GetParam( GUID_PersonalityNode, m_dwGroupBits, 0, 0, &mtTime, &pIPersonalityNode ) ) )
	{
		IDMUSProdChordMapInfo* pIChordMapInfo;
		if( SUCCEEDED ( pIPersonalityNode->QueryInterface( IID_IDMUSProdChordMapInfo, (void**)&pIChordMapInfo ) ) )
		{
			DMUSProdChordMapInfo info;
			memset( &info, 0, sizeof(DMUSProdChordMapInfo) );
			info.wSize = sizeof(DMUSProdChordMapInfo);

			if( SUCCEEDED ( pIChordMapInfo->GetChordMapInfo( &info ) ) )
			{
				m_bKey = info.bKey % 24;
				m_dwScale = info.lScalePattern;
				m_fDisplayingFlats = info.fUseFlats;
				UpdateKeyRootAndAccidentals();
			}

			pIChordMapInfo->Release();
		}

		pIPersonalityNode->Release();
	}

	return S_OK;
}


HRESULT CChordMgr::ImportChordsFromChordTrack( void )
{
	if( m_pIDMTrack == NULL )
	{
		return E_FAIL;
	}

	// Clear chords before load
	m_ChordList.ClearList();

	// Load chords
	DMUS_CHORD_PARAM dmChordParam;
	MUSIC_TIME mtNow, mtNext;
	mtNow = 0;
	while( SUCCEEDED( m_pIDMTrack->GetParam( GUID_ChordParam, mtNow, &mtNext, &dmChordParam ) ) )
	{
		ChordSelection chordSelection;
		chordSelection.Beat() = dmChordParam.bBeat;
		chordSelection.Measure() = dmChordParam.wMeasure;
		chordSelection.Time() = mtNow;
		// Convert dmChordParam.wszName[16] to chordSelection.m_szName[12]
		memset(chordSelection.Name(), 0, DMPolyChord::MAX_NAME);
		WideCharToMultiByte( CP_ACP, 0, dmChordParam.wszName, -1, chordSelection.Name(), 
											DMPolyChord::MAX_NAME, NULL, NULL );
		
		// Convert dmChordParam.bSubChordCount, DMUS_SUBCHORD SubChordList[DMUS_MAXSUBCHORD] 
		// to DMChord chordSelection.m_Chords[MAX_POLY]
		int nchords = dmChordParam.bSubChordCount < DMPolyChord::MAX_POLY ? dmChordParam.bSubChordCount : DMPolyChord::MAX_POLY;
		for(int i = 0; i < nchords; i++)
		{
			chordSelection.SubChord(i)->ChordPattern() = dmChordParam.SubChordList[i].dwChordPattern;
			chordSelection.SubChord(i)->ScalePattern() = dmChordParam.SubChordList[i].dwScalePattern;
			chordSelection.SubChord(i)->InvertPattern() = dmChordParam.SubChordList[i].dwInversionPoints;
			chordSelection.SubChord(i)->Levels() = dmChordParam.SubChordList[i].dwLevels;
			chordSelection.SubChord(i)->ChordRoot() = dmChordParam.SubChordList[i].bChordRoot;
			chordSelection.SubChord(i)->ScaleRoot() = dmChordParam.SubChordList[i].bScaleRoot;
		}

		CChordItem* pItem = new CChordItem( chordSelection );
		if( pItem )
		{
			m_ChordList.AddTail( pItem );
		}
		if( mtNext == 0 )
		{
			break;
		}
		else
		{
			mtNow += mtNext;
		}
	}

	// Redraw the chord strip
	if( m_pTimeline )
	{
		m_pTimeline->StripInvalidateRect( (IDMUSProdStrip*)m_pChordStrip, NULL, TRUE );
	}
	m_ChordList.UseFlats( (m_fDisplayingFlats != 0) ? true : false);

	// Sync to refresh chord strip's root/key
	UpdateDirectMusic( FALSE );
	return S_FALSE;
}


void CChordMgr::RecalculateMusicTimeValue(CChordItem* pChord)
{
	if( m_pTimeline )
	{
		MUSIC_TIME mtTime;
		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, pChord->Measure(), pChord->Beat(), &mtTime );
		pChord->Time() = mtTime;
	}
}

/*
static void MarkChord(CChordItem* pChord, long value)
{
	pChord->Undetermined(pChord->RootIndex()) = value;
}


void CChordMgr::MarkChordsInMeasure(long measure, long bpm)
{
	for(CChordItem* pChord = m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->Measure() == measure && pChord->Beat() < bpm)
		{
			MarkChord(pChord, InMeasure);
		}
	}
}
*/

bool CChordMgr::IsChordAt(long measure, long beat)
{
	for(CChordItem* pItem = m_ChordList.GetHead(); pItem; pItem = pItem->GetNext())
	{
		if(measure == pItem->Measure() && beat == pItem->Beat())
		{
			return true;
		}
	}
	return false;
}

long CChordMgr::ShiftChordsBackwards(long measure, long bpm)
{
	long firstEmptyBeat = 0;
	CChordItem* pItem = m_ChordList.GetHead();
	CChordItem* pFirst = 0;
	// find the first chord of the target measure
	for(; pItem && pItem->Measure() < measure; pItem = pItem->GetNext())
	{
		continue;
	}
	if(pItem && pItem->Measure() == measure)
	{
		pFirst = pItem;
	}
	else
	{
		ASSERT(FALSE);	// we should never get here: no chords in measure -> plenty of empty beats
		return bpm;		// assues that function returns failure.
	}
	// find first empty beat
	for(pItem = pFirst; pItem && firstEmptyBeat < bpm && pItem->Measure() == measure; pItem = pItem->GetNext())
	{
		if(pItem->Beat() > firstEmptyBeat)
		{
			break;
		}
		firstEmptyBeat++;
	}
	if(firstEmptyBeat < bpm)
	{
		// do the shift
		for(pItem = pFirst; 
			pItem && pItem->Measure() <= measure;  
			pItem = pItem->GetNext())
		{
			if(pItem->Measure() == measure && pItem->Beat() > firstEmptyBeat)
			{
				pItem->Beat() -=1;
			}
		}
	}
	return firstEmptyBeat;
}


void CChordMgr::RecalculateMusicTimeValues()
{
	if(m_pTimeline)
	{
		long measure = -1;
		long beat = -1;
		CMusicTimeConverter cmt;
		CChordList lstByMeasure;
		for(CChordItem* pChord = m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
		{
			DMUS_TIMESIGNATURE dmTimeSig;
			measure = pChord->Measure();
			beat = pChord->Beat();
			cmt.SetTime(measure,beat,m_pTimeline, m_dwGroupBits);
			cmt.GetTimeSig(dmTimeSig, m_pTimeline, m_dwGroupBits);
			if(beat >= dmTimeSig.bBeatsPerMeasure)
			{
				// adjust beat to last beat of measure and check if coincident chord on last beat
				pChord->Beat() = dmTimeSig.bBeatsPerMeasure;	// this is too many beats, but hopefully
																// it'll be shifted below
				if(IsChordAt(measure, dmTimeSig.bBeatsPerMeasure - 1))
				{
					long firstEmptyBeat = ShiftChordsBackwards(measure, dmTimeSig.bBeatsPerMeasure);
					if(firstEmptyBeat >= dmTimeSig.bBeatsPerMeasure)
					{
						// no room for chord, mark it for deletion
						pChord->Undetermined(pChord->RootIndex()) = 1;
					}
					else
					{
						pChord->Undetermined(pChord->RootIndex()) = 0;
						RecalculateMusicTimeValue(pChord);
					}
				}
				else
				{
					pChord->Beat() = dmTimeSig.bBeatsPerMeasure - 1;
					pChord->Undetermined(pChord->RootIndex()) = 0;
					RecalculateMusicTimeValue(pChord);
				}
			}
			else
			{
				pChord->Undetermined(pChord->RootIndex()) = 0;
				RecalculateMusicTimeValue(pChord);
			}
		}
		DeleteMarkedChords(1);
	}
}

HRESULT CChordMgr::GetBoundariesOfSelectedChords(long & lStartTime, long & lEndTime)
{
	ASSERT(m_pTimeline);
	HRESULT hr = S_OK;
	lStartTime = lEndTime = -1;
	CMusicTimeConverter cmt;
	CChordItem* pChord = m_ChordList.GetHead();
	for(; pChord; pChord = pChord->GetNext())
	{
		if(pChord->IsSelected())
		{
			hr = cmt.SetTime(pChord->Measure(), pChord->Beat(),
							m_pTimeline, m_dwGroupBits);
			if(FAILED(hr))
			{
				goto CheckBounds;
			}
			if(lStartTime == -1 || cmt < lStartTime)
			{
				lStartTime = cmt;
			}
			if(lEndTime == -1 || cmt > lEndTime)
			{
				lEndTime = cmt;
			}
		}
	}
CheckBounds:
	if(hr == S_OK && (lStartTime == -1 || lEndTime == -1))
	{
		// nothing selected
		hr = S_FALSE;
	}
	else
	{
		// advance endtime by one beat
		DMUS_TIMESIGNATURE dmTimeSig;
		long m,b;
		cmt = lEndTime;
		hr = cmt.GetTimeSig(dmTimeSig, m_pTimeline, m_dwGroupBits);
		ASSERT(SUCCEEDED(hr));
		if(FAILED(hr))
			return hr;
		hr = cmt.GetMeasureBeat(m,b, m_pTimeline, m_dwGroupBits);
		ASSERT(SUCCEEDED(hr));
		if(FAILED(hr))
			return hr;
		b++;
		if(b > dmTimeSig.bBeatsPerMeasure)
		{
			m++;
			b = 0;
		}
		hr = cmt.SetTime(m,b, m_pTimeline, m_dwGroupBits);
		ASSERT(SUCCEEDED(hr));
		if(FAILED(hr))
			return hr;
		lEndTime = cmt - 1;
	}
	return hr;
}


HRESULT CChordMgr::GetSpanOfChordList(long& lStartTime, long& lEndTime, CChordList * pList, bool bSelectedChordsOnly)
{
	ASSERT(m_pTimeline);
	if(m_pTimeline == NULL)
		return E_UNEXPECTED;
	HRESULT hr = S_OK;
	if(pList == NULL)
	{
		// use internal chord list
		pList = &m_ChordList;
	}
	lStartTime = -1;
	lEndTime = -1;
	CChordItem* pChord;
	for(pChord = pList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		CMusicTimeConverter cmt;
		if(!bSelectedChordsOnly || pChord->IsSelected())
		{
			hr = cmt.SetTime(pChord->Measure(), pChord->Beat(),
							m_pTimeline, m_dwGroupBits);
			if(hr == S_OK)
			{
				if(lStartTime == -1 || cmt < lStartTime)
					lStartTime = cmt;
				if(lEndTime == -1 || cmt > lEndTime)
					lEndTime = cmt;
			}
			else
			{
				break;
			}
		}
	}
	if(lStartTime == -1 || lEndTime == -1)
	{
		lStartTime = lEndTime = -1;
		hr = S_FALSE;
	}

	// extend endtime out 1/4
//	lEndTime += DMUS_PPQ;
	return hr;
}

void CChordMgr::SelectAll()
{
	CChordItem* pChord = m_ChordList.GetHead();
	while(pChord)
	{
		pChord->SetSelected(TRUE);
		pChord = pChord->GetNext();
	}
}

const DWORD adwFlatScales[8] = {
	0x00AB5AB5, // C
	0x006B56B5,	// F
	0x006AD6AD,	// Bb
	0x005AD5AD, // Eb
	0x005AB5AB,	// Ab
	0x0056B56B,	// Db
	0x00D6AD6A,	// Gb
	0x00D5AD5A};// Cb
const DWORD adwSharpScales[8] = {
	0x00AB5AB5,	// C
	0x00AD5AD5,	// G
	0x00AD6AD6,	// D
	0x00B56B56,	// A
	0x00B5AB5A,	// E
	0x00D5AD5A,	// B
	0x00D6AD6A,	// F#
	0x0056B56B};// C#

// Updates m_dwScale and m_bKey based on the key root and # of sharps/flats
void CChordMgr::UpdateMasterScaleAndKey()
{
	ASSERT( m_nNumAccidentals < 8 );
	ASSERT( m_nKeyRoot < 12 );

	m_nNumAccidentals = min( m_nNumAccidentals, 7 );
	m_nKeyRoot = m_nKeyRoot % 12;

	DWORD dwOrigScale;
	if( m_fDisplayingFlats )
	{
		dwOrigScale = adwFlatScales[m_nNumAccidentals];
	}
	else
	{
		dwOrigScale = adwSharpScales[m_nNumAccidentals];
	}

	// Shift the scale so bit 0 of m_dwScale is the key root
	m_dwScale = (dwOrigScale >> m_nKeyRoot)
			  | (0x00FFFFFF & (dwOrigScale << (12 - m_nKeyRoot)));
	m_bKey = m_nKeyRoot;
}

// Updates m_nNumAccidentals and m_nKeyRoot from m_dwScale and m_bKey
void CChordMgr::UpdateKeyRootAndAccidentals()
{
	ASSERT( m_bKey < 24 );
	m_nKeyRoot = m_bKey % 12;

	for( int iNumAccidentals = 0; iNumAccidentals < 8; iNumAccidentals++ )
	{
		// Check sharp scales
		if( !m_fDisplayingFlats && m_dwScale == ((adwSharpScales[iNumAccidentals] >> m_nKeyRoot) |
						  (0x00FFFFFF & (adwSharpScales[iNumAccidentals] << (12 - m_nKeyRoot)))) )
		{
			m_nNumAccidentals = iNumAccidentals;
			goto Leave;
		}
		// Check flat scales
		else if( m_fDisplayingFlats && m_dwScale == ((adwFlatScales[iNumAccidentals] >> m_nKeyRoot) |
						  (0x00FFFFFF & (adwFlatScales[iNumAccidentals] << (12 - m_nKeyRoot)))) )
		{
			m_nNumAccidentals = iNumAccidentals;
			goto Leave;
		}
	}

	// Not a normal scale
//	TRACE("CChordMgr::UpdateKeyRootAndAccidentals: Loaded abnormal scale %x - using C major\n", m_dwScale );
//	m_nNumAccidentals = 0;
//	m_fDisplayingFlats = FALSE;
Leave:
	;
}

HRESULT CChordMgr::SaveSelectedChords(IStream *pStream, long offset)
// save selected chords and adjust by "offset"
{
	ASSERT(pStream);
	CChordList list;
	CChordItem* pChord;
	// copy selected chords to list
	for(pChord = m_ChordList.GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->IsSelected())
		{
			// make sure that only the chord at the mouse drag point is selected
			pChord->SetSelected( FALSE );
			list.AddTail(new CChordItem(*pChord));
			// re-select chord
			pChord->SetSelected(TRUE);
		}
	}
	// check that anything is selected
	if(!list.GetHead())
		return S_FALSE; // Nothing selected
	// substract offset from each chord

	pChord = list.GetHead();
	NormalizeChordList(list, -offset);

	HRESULT hr = list.Save(pStream, FALSE);
	list.ClearList();
	return hr;
}

bool CChordMgr::UseFlatsNotSharps(BYTE bKey, DWORD dwScale)
// uses key and scale portion of DMUS_CHORD_PARAM to determine if chord should be displayed with sharps or flats
{
	bool bFlatsNotSharps = false;
	for( int iNumAccidentals = 0; iNumAccidentals < 8; iNumAccidentals++ )
	{
		// Check sharp scales
		if( dwScale == ((adwSharpScales[iNumAccidentals] >> bKey) |
						  (0x00FFFFFF & (adwSharpScales[iNumAccidentals] << (12 - bKey)))) )
		{
			bFlatsNotSharps = false;
			goto Leave;
		}
		// Check flat scales
		if( dwScale == ((adwFlatScales[iNumAccidentals] >> bKey) |
						  (0x00FFFFFF & (adwFlatScales[iNumAccidentals] << (12 - bKey)))) )
		{
			bFlatsNotSharps = true;
			goto Leave;
		}
	}
Leave:
	return bFlatsNotSharps;
}
