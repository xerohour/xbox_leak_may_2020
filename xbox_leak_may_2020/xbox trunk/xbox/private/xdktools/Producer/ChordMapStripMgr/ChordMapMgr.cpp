// ChordMapMgr.cpp : Implementation of CChordMapMgr
#include "stdafx.h"
#include "ChordMapStripMgr.h"
#include "ChordMapMgr.h"
#include "..\shared\RiffStrm.h"
#include "ChordDatabase.h"
#include <ChordMapDesigner.h>
#include <SegmentGuids.h>
#include "..\shared\musictimeconverter.h"

/////////////////////////////////////////////////////////////////////////////
// CChordMapMgr

CChordMapMgr::CChordMapMgr()
{
	m_pTimeline = NULL;
	m_pChordMapList = NULL;
	m_nIDCounter = 1;
	m_pPropertyPage = NULL;
	m_pChordPropPageMgr = NULL;
	m_pConnectionPropPageMgr = NULL;
	m_pJazzFramework = NULL;
	m_pCopyDataObject = NULL;
	m_pChordMapStrip = NULL;
	m_pSelectedConnection = 0;
	m_selection = NoSelection;
	m_dwGroupBits = 1;
	m_dblZoom = 0.03125;

	m_pIChordMapNode = NULL;
	m_pIDMTrack = NULL;
	m_dwKey = 12;	// default = 2 C
}


CChordMapMgr::~CChordMapMgr()
{
//				ClearChordMapList(); Intentially left commented.  CPersonality will delete this list.
	if( m_pJazzFramework ) m_pJazzFramework->Release();
	ASSERT( m_pPropertyPage == NULL );
	if( m_pIDMTrack )
	{
		m_pIDMTrack->Release();
	}
	if( m_pChordPropPageMgr ) 
		m_pChordPropPageMgr->Release();
	if( m_pConnectionPropPageMgr ) 
		m_pConnectionPropPageMgr->Release();
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
// CChordMapMgr IChordMapMgr

HRESULT STDMETHODCALLTYPE CChordMapMgr::CreateChordMapStrip( 
        /* [out] */ IDMUSProdStrip **ppStrip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr;
	
	if( NULL == ppStrip )
	{
		return E_POINTER;
	}
	if( m_pChordMapStrip )
	{
		// Sloppy coding, but it shouldn't cause a memory leak if m_pChordMapStrip is non-NULL here
		//ASSERT(FALSE);
	}
	m_pChordMapStrip = new CChordMapStrip(this);
	if( !m_pChordMapStrip )
	{
		return E_OUTOFMEMORY;
	}
	hr = m_pChordMapStrip->QueryInterface( IID_IDMUSProdStrip, (void**)ppStrip );
	m_pChordMapStrip->Release();	// intentional
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CChordMapMgr IDMUSProdStripMgr

HRESULT STDMETHODCALLTYPE CChordMapMgr::GetParam(
		/* [in] */  REFGUID		guidType,
		/* [in] */  MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME*	pmtNext,
		/* [out] */ void*		pData)
{
	ASSERT(pData);
	if(pData == 0)
	{
		return E_POINTER;
	}

	if(::IsEqualGUID(guidType, GUID_ChordMapStripUndoText))
	{
		// pData is CString ptr
		CString* pstr = static_cast<CString*>(pData);
		GetLastEdit(*pstr);
		return S_OK;
	}
	else if(::IsEqualGUID(guidType, GUID_SelectedObjectType))
	{
		// pData is int ptr
		int* pint = static_cast<int*>(pData);
		*pint = m_selection;
		return S_OK;
	}
	else if(::IsEqualGUID(guidType, GUID_SelectedConnection))
	{
		// pData is NextChord ptr
		NextChord** pnext = static_cast<NextChord**>(pData);
		*pnext = m_pSelectedConnection;
		return S_OK;
	}
	else if(::IsEqualGUID(guidType, GUID_SelectedPaletteIndex))
	{
		// pData is int ptr
		*static_cast<int*>(pData) = m_nSelectedIndex;
		return S_OK;
	}
	else if(::IsEqualGUID(guidType, GUID_DocRootNode))
	{
		IDMUSProdNode** ppIDocRootNode = (IDMUSProdNode **)pData;

		*ppIDocRootNode = m_pIChordMapNode;
		if( m_pIChordMapNode )
		{
			m_pIChordMapNode->AddRef();
		}
		return S_OK;
	}
	else
	{
		return E_INVALIDARG;
	}
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	if( ::IsEqualGUID( guidType, GUID_VariableNotFixed ) )
	{
		VARIANT v;
		v.vt = VT_BOOL;
		V_BOOL(&v) = *(BOOL*)pData;
		m_pChordMapStrip->SetStripProperty((STRIPPROPERTY)666, v);
		m_pChordMapStrip->RecomputeConnections( true );
		m_pTimeline->StripInvalidateRect(m_pChordMapStrip, NULL, TRUE);
		return S_OK;
	}
	else if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		IDMUSProdNode* pIDocRootNode = (IDMUSProdNode *)pData;

		m_pIChordMapNode = pIDocRootNode;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::IsParamSupported(
		/* [in] */ REFGUID		guidType)
{
	if( ::IsEqualGUID( guidType, GUID_VariableNotFixed ) 
	||  ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		return S_OK;
	}
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	
	// TimeSig change
	if( ::IsEqualGUID( rguidType, GUID_TimeSignature ) )
	{
		if(m_pTimeline)	// only update if strip exists (timeline exists -> strip exists)
			m_pChordMapStrip->ComputeChordMap();
		return S_OK;
	}
	else
		return E_INVALIDARG;
}
HRESULT STDMETHODCALLTYPE CChordMapMgr::GetStripMgrProperty(
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

		/* Unsupported
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
		if( m_pDMProdFramework )
		{
			return m_pDMProdFramework->QueryInterface( IID_IUnknown, (void**)&V_UNKNOWN( pVariant ) );
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
		DMUS_IO_TRACK_HEADER *pioTrackHeader = V_BYREF( &variant );
		if( pioTrackHeader == NULL )
		{
			return E_POINTER;
		}

		pioTrackHeader->guidClassID = CLSID_DirectMusicBandTrack;
		pioTrackHeader->dwPosition = 0;
		pioTrackHeader->dwGroup = m_dwGroupBits;
		pioTrackHeader->ckid = NULL;
		pioTrackHeader->fccType = DMUS_FOURCC_BANDTRACK_FORM;
		break;
		*/

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::SetStripMgrProperty(
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
		if( m_pJazzFramework )
		{
			m_pJazzFramework->Release();
			m_pJazzFramework = NULL;
		}
		if( m_pTimeline )
		{
			if( V_UNKNOWN( &variant ) == NULL )
			{
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);
			}
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
				// set m_pJazzFramework to the jazz framework pointer
				VARIANT var;
				LPUNKNOWN punk;
				if( SUCCEEDED(m_pTimeline->GetTimelineProperty( TP_DMUSPRODFRAMEWORK, &var )) )
				{
					punk = V_UNKNOWN(&var);
					if( punk )
					{
						punk->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pJazzFramework );
						punk->Release();
					}
				}
			}
		}
		break;

		/* Unsupported
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
		if( m_pDMProdFramework )
		{
			m_pDMProdFramework->Release();
			m_pDMProdFramework = NULL;
		}
		if( V_UNKNOWN( &variant ) )
		{
			return V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdFramework, (void**)&m_pDMProdFramework);
		}
		break;

	case SMP_DMUSIOTRACKHEADER:
		if( variant.vt != VT_BYREF )
		{
			return E_INVALIDARG;
		}
		DMUS_IO_TRACK_HEADER *pioTrackHeader = V_BYREF( &variant );
		if( pioTrackHeader == NULL )
		{
			return E_POINTER;
		}
		m_dwGroupBits = pioTrackHeader->dwGroup;
		m_dwOldGroupBits = pioTrackHeader->dwGroup;
		break;
		*/

	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::CreateBlankChord( ChordEntry** ppChord )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT		hr = S_OK;
	ChordEntry*	pChord = NULL;

	// Allocate new Chord
	if( (pChord = new ChordEntry) == NULL )
	{
		return E_OUTOFMEMORY;
	}

	// Set chord roots to key
	DWORD key = m_dwKey & ~UseFlats;
	pChord->m_chordsel = m_pChordPalette->m_chords[key];
	if(m_dwKey & UseFlats)
	{
		pChord->m_chordsel.SubChord(0)->UseFlat() = true;
		pChord->m_chordsel.PropagateUseFlat();
	}

	NextChord* pNextChord = new NextChord;
	if( pNextChord == NULL )
	{
		delete pChord;
		return E_OUTOFMEMORY;
	}

	pNextChord->m_parent = pChord;
	pChord->m_nextchordlist.AddTail(pNextChord);

	ZeroMemory( &(pChord->m_rect), sizeof(pChord->m_rect) );
	ZeroMemory( &(pChord->m_lastrect), sizeof(pChord->m_lastrect) );

	((SmallChordEntry*)pChord)->m_nid = m_nIDCounter++;

	*ppChord = pChord;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CChordMapMgr IPersist

HRESULT CChordMapMgr::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CChordMapMgr IPersistStream functions

HRESULT CChordMapMgr::IsDirty()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return S_FALSE;
}

HRESULT CChordMapMgr::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	HRESULT hr = E_FAIL;

	if( FAILED(LoadChordMapAndPalette( pIStream )) )
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CChordMapMgr::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return E_NOTIMPL;
}

HRESULT CChordMapMgr::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return E_NOTIMPL;
}

HRESULT CChordMapMgr::GetTimeSig(long* top, long* bottom)
{
	if(!m_pTimeline)
		return E_FAIL;

	DMUS_TIMESIGNATURE timesig;

	HRESULT hr = m_pTimeline->GetParam(GUID_TimeSignature, 0xFFFFFFFF, 0, 0, NULL, static_cast<void*>(&timesig));

	if(top && SUCCEEDED(hr))
	{
		*top = static_cast<long>(timesig.bBeatsPerMeasure);
	}

	if(bottom && SUCCEEDED(hr))
	{
		*bottom = static_cast<long>(timesig.bBeat);
	}

	return hr;
}



static void SetChordMapBits()
{

/*
	LONG    i;
	short   count = 0;

    for( i=0L ;  i<32L ;  i++ )
    {
        if( pChordMap->pattern & (1L << i) )
            count++;
    }

    if( !pChordMap->bits )
    {
        pChordMap->bits |= ChordMap_INVERT;
        if( count > 3 )
            pChordMap->bits |= ChordMap_FOUR;
        if( pChordMap->pattern & (15L << 18L) )
            pChordMap->bits |= ChordMap_UPPER;
    }
    pChordMap->bits &= ~ChordMap_COUNT;
    pChordMap->bits |= count;
*/
}

void CChordMapMgr::ClearChordMapList(void)
{
	/* m_pChordMapList is shared with CPersonality.  DON'T DELETE IT! */
	ASSERT( 0 );
}


HRESULT CChordMapMgr::LoadChordMapAndPalette( LPSTREAM pIStream /*, LPMMCKINFO pck, ChordMapExt** plstChordMap */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pIStream != NULL );

	DWORD			cbRead = 0;
	int			cbSize;

	STATSTG	stg;
	STATFLAG stgflag = STATFLAG_NONAME;

	if(FAILED(pIStream->Stat(&stg, stgflag)))
	{
		return E_FAIL;
	}
	
	cbSize = stg.cbSize.LowPart;

	// we're going to redo the selection, so clear out the current one
	m_pSelectedConnection = NULL;	// ok, since the chordlist is managed by the personality
	m_selection = NoSelection;

	// Merely save the ChordMapList ptr. 
	if( FAILED(pIStream->Read( &m_pChordMapList, sizeof(m_pChordMapList), &cbRead )) )
	{
		m_pChordMapList = NULL;
		return E_FAIL;
	}
	cbSize -= cbRead;

	// And the palette
	if( FAILED(pIStream->Read( &m_pChordPalette, sizeof(m_pChordPalette), &cbRead )) )
	{
		m_pChordMapList = NULL;
		return E_FAIL;
	}
	cbSize -= cbRead;

	// anything left in stream --> we've got a personality key spec.
	if(cbSize > 0)
	{
		if( FAILED(pIStream->Read( &m_dwKey, sizeof(DWORD), &cbRead )) )
		{
			m_pChordMapList = NULL;
			return E_FAIL;
		}
		cbSize -= cbRead;
		// check for selection info
		if(cbSize > 0)
		{
			int selection;
			if( FAILED(pIStream->Read( &selection, sizeof(selection), &cbRead)))
			{
				m_selection = NoSelection;
				return E_FAIL;
			}
			else if(selection > 0 && selection < EndEnum)
			{
				m_selection = static_cast<CChordMapMgr::SelectionType>(selection);
			}
			else
			{
				// keep current selection
			}
			cbSize -= cbRead;
			if(cbSize > 0)
			{
				short nidToChord, nidFromChord;
				// check for connecting chords
				if( FAILED(pIStream->Read(&nidFromChord, sizeof(nidFromChord), &cbRead)))
				{
					return E_FAIL;
				}
				cbSize -= cbRead;
				if(cbSize > 0)
				{
					if(FAILED(pIStream->Read(&nidToChord, sizeof(nidToChord), &cbRead)))
					{
						return E_FAIL;
					}
					if(m_selection == ConnectionSelected)
					{
						SetUpNextChord(nidFromChord, nidToChord);
					}
					cbSize -= cbRead;
					if(cbSize > 0)
					{
						if(FAILED(pIStream->Read(&m_nSelectedIndex, sizeof(m_nSelectedIndex), &cbRead)))
						{
							return E_FAIL;
						}
						cbSize -= cbRead;
						/*
						if(cbSize > 0)
						{
							if(FAILED(pIStream->Read(&m_dblZoom, sizeof(double), &cbRead)))
							{
								return E_FAIL;
							}
							cbSize -= cbRead;
						}
						*/
					}
				}
			}
		}
	}

	// Find next Chord ID number.
	if( FAILED(FindUniqueID( &m_nIDCounter )) )
	{
		m_pChordMapList = NULL;
		return E_FAIL;
	}

	if(m_selection != ChordSelected && m_selection != ConnectionSelected && m_selection != PaletteSelected)
	{
		// clear selections
		ChordEntry* pChord = NULL;

		// Go through the Chords in the ChordList and check each connection for a hit.
		for(	pChord = m_pChordMapList->GetHead();
				pChord != NULL;
				pChord = pChord->GetNext() )
		{
			if( pChord->m_chordsel.KeyDown() == TRUE )
			{
				pChord->m_chordsel.KeyDown() = FALSE;
			}
			// clear out flags too
	//		pChord->m_dwflags = 0;
		}

		// now clear out chord palette
		for(int i = 0; i < 24; i++)
		{
			m_pChordPalette->m_chords[i].KeyDown() = false;
		}
	}
	else if (IsPropPageShowing())
	{
		bool bShow = false;
		if(m_selection == ChordSelected || m_selection == PaletteSelected)
		{
			PreparePropChord();

			if( m_pChordPropPageMgr == NULL )
			{
				HRESULT hr;
				CChordPropPageMgr* pPPM = new CChordPropPageMgr(m_pJazzFramework);
				if( NULL == pPPM ) return E_OUTOFMEMORY;
				hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pChordPropPageMgr );
				m_pChordPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
				if( FAILED(hr) )
					return hr;
			}
			m_pChordPropPageMgr->RefreshData();
			bShow = true;
		}
		else if(m_selection == ConnectionSelected)
		{
			if( m_pConnectionPropPageMgr == NULL )
			{
				HRESULT hr;
				CConnectionPropPageMgr* pPPM = new CConnectionPropPageMgr;
				if( NULL == pPPM ) return E_OUTOFMEMORY;
				hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pConnectionPropPageMgr );
				m_pConnectionPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
				if( FAILED(hr) )
					return hr;
			}
			m_pConnectionPropPageMgr->RefreshData();
			bShow = true;
		}

		if(bShow)
		{
			ShowPropPage(TRUE);
			OnShowProperties();
		}
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::FindUniqueID( /* [out] */ int* pnID )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pnID != NULL );

	(*pnID) = 0;
	for(	ChordEntry* pChord = m_pChordMapList->GetHead();
			pChord;
			pChord = pChord->GetNext() )
	{
		if( pChord->m_nid > (*pnID) )
		{
			(*pnID) = pChord->m_nid;
		}
	}
	
	(*pnID)++;			

	return S_OK;
}

// Save the ChordMaplist to a normal stream
HRESULT CChordMapMgr::SaveChordMapList( LPSTREAM pStream )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject functions

HRESULT STDMETHODCALLTYPE CChordMapMgr::GetData( /* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_selection == ConnectionSelected )
	{
		ConnectionData* pConnectionData = (ConnectionData *)*ppData;

		if( m_pSelectedConnection )
		{
			VARIANT var;
			m_pChordMapStrip->GetStripProperty( (STRIPPROPERTY)666, &var );
			pConnectionData->fVariableNotFixed = V_BOOL(&var);
			pConnectionData->nWeight = m_pSelectedConnection->m_nweight;
			pConnectionData->nMinBeats = m_pSelectedConnection->m_nminbeats;
			pConnectionData->nMaxBeats = m_pSelectedConnection->m_nmaxbeats;
		}
		else
		{
			ASSERT( m_pSelectedConnection != NULL );
			return E_UNEXPECTED;
		}
	}
	else if( m_selection == ChordSelected
		 ||	 m_selection == PaletteSelected 
		 ||  m_selection == MultipleSelections
		 ||  m_selection == NoSelection )
	{
		CChordScalePropPageData* pChordScalePropPageData = (CChordScalePropPageData *)*ppData;

		pChordScalePropPageData->m_pPropChord = &m_SelectedChord;
		pChordScalePropPageData->m_fLockAllScales = true;
		pChordScalePropPageData->m_fSyncLevelOneToAll = true;

		if( m_pIChordMapNode )
		{
			IDMUSProdChordMapInfo* pIChordMapInfo;
			if( SUCCEEDED ( m_pIChordMapNode->QueryInterface( IID_IDMUSProdChordMapInfo, (void**)&pIChordMapInfo ) ) )
			{
				DMUSProdChordMapUIInfo info;
				memset( &info, 0, sizeof(DMUSProdChordMapUIInfo) );
				info.wSize = sizeof(DMUSProdChordMapUIInfo);

				if( SUCCEEDED ( pIChordMapInfo->GetChordMapUIInfo( &info ) ) )
				{
					pChordScalePropPageData->m_fLockAllScales = info.fLockAllScales ? TRUE : FALSE;
					pChordScalePropPageData->m_fSyncLevelOneToAll = info.fSyncLevelOneToAll ? TRUE : FALSE;
				}

				pIChordMapInfo->Release();
			}
		}
	}
	else
	{
		ASSERT(FALSE);
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::PropChordToChord( ChordEntry *pChord, CPropChord *pPropChord )
{
	ASSERT( pPropChord != NULL );
	ASSERT( pChord != NULL );

	if(pPropChord == NULL || pChord == NULL)
	{
		return E_INVALIDARG;
	}

	dynamic_cast<DMPolyChord&>(pChord->m_chordsel) = *dynamic_cast<DMPolyChord*>(pPropChord);
	
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::ChordToPropChord( CPropChord *pPropChord, ChordEntry *pChord )
{
	ASSERT( pPropChord != NULL );
	ASSERT( pChord != NULL );

	if(pPropChord == NULL || pChord == NULL)
	{
		return E_INVALIDARG;
	}

	ZeroMemory( pPropChord, sizeof(*pPropChord) );

	*dynamic_cast<DMPolyChord*>(pPropChord) = dynamic_cast<DMPolyChord&>(pChord->m_chordsel);

	return S_OK;
}


HRESULT STDMETHODCALLTYPE CChordMapMgr::SetData( /* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	BOOL fRefresh = FALSE;
	CPropChord* pNewChord = 0;

	if( m_selection == ConnectionSelected )
	{
		ASSERT(m_pSelectedConnection);
		if(!m_pSelectedConnection)
		{
			return E_UNEXPECTED;
		}
		ConnectionData* pCon = reinterpret_cast<ConnectionData*>(pData);
		if(m_pSelectedConnection->m_nweight != pCon->nWeight)
		{
			m_pSelectedConnection->m_nweight = pCon->nWeight;
			fRefresh = TRUE;
		}
		if(m_pSelectedConnection->m_nminbeats != pCon->nMinBeats)
		{
			m_pSelectedConnection->m_nminbeats = pCon->nMinBeats;
			fRefresh = TRUE;
		}
		if(m_pSelectedConnection->m_nmaxbeats != pCon->nMaxBeats)
		{
			m_pSelectedConnection->m_nmaxbeats = pCon->nMaxBeats;
			fRefresh = TRUE;
		}
		if(fRefresh)
		{
			SendEditNotification(IDS_UNDO_EditConnection);
		}
	}
	else if( m_selection == ChordSelected
		 ||  m_selection == PaletteSelected )
	{
		CChordScalePropPageData* pChordScalePropPageData = (CChordScalePropPageData *)pData;
		pNewChord = pChordScalePropPageData->m_pPropChord;

		DWORD dwChangedFlags[DMPolyChord::MAX_POLY];
		bool bStructureChange = false;

		if( m_pIChordMapNode )
		{
			IDMUSProdChordMapInfo* pIChordMapInfo;
			if( SUCCEEDED ( m_pIChordMapNode->QueryInterface( IID_IDMUSProdChordMapInfo, (void**)&pIChordMapInfo ) ) )
			{
				DMUSProdChordMapUIInfo info;
				memset( &info, 0, sizeof(DMUSProdChordMapUIInfo) );
				info.wSize = sizeof(DMUSProdChordMapUIInfo);

				if( SUCCEEDED( pIChordMapInfo->GetChordMapUIInfo( &info ) ) )
				{
					if( pChordScalePropPageData->m_fLockAllScales != info.fLockAllScales )
					{
						fRefresh = TRUE;
						SendEditNotification( IDS_UNDO_LOCK_SCALES );
						info.fLockAllScales = pChordScalePropPageData->m_fLockAllScales;
						pIChordMapInfo->SetChordMapUIInfo( &info );
					}
					else if( pChordScalePropPageData->m_fSyncLevelOneToAll != info.fSyncLevelOneToAll )
					{
						fRefresh = TRUE;
						SendEditNotification( IDS_UNDO_SYNC_LEVELS );	
						info.fSyncLevelOneToAll = pChordScalePropPageData->m_fSyncLevelOneToAll;
						pIChordMapInfo->SetChordMapUIInfo( &info );
					}
				}
			
				pIChordMapInfo->Release();
			}
		}

		m_SelectedChord = *pNewChord;

		if( fRefresh == FALSE )
		{
			if(m_SelectedChord.GetUndetermined(m_SelectedChord.RootIndex()) & UD_FROMCHORDPALETTE)
			{
				SendEditNotification(IDS_UNDO_EditPaletteChord);	
				ChordSelection* pSel = &(m_pChordPalette->m_chords[m_nSelectedIndex]);
				// Only refresh if UI elements have changed.
				fRefresh =	((pSel->Base()->ChordRoot() != m_SelectedChord.Base()->ChordRoot())
							|| (strcmp(pSel->Name(), m_SelectedChord.Name()))
							|| pSel->Base()->UseFlat() != m_SelectedChord.Base()->UseFlat());

				for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
				{
					DMChord* pDMFrom = pNewChord->SubChord(i);
					DMChord* pDMTo = pSel->SubChord(i);

					dwChangedFlags[i] = ~pNewChord->GetUndetermined(i);

					if (dwChangedFlags[i] & UD_NAME)
					{
						strcpy(pSel->Name(), pNewChord->Name());
					}
					if (dwChangedFlags[i] & UD_CHORDPATTERN)
					{
						pDMTo->ChordPattern() = pDMFrom->ChordPattern();
					}
					if (dwChangedFlags[i] & UD_SCALEPATTERN)
					{
						pDMTo->ScalePattern() = pDMFrom->ScalePattern();
					}
					if(dwChangedFlags[i] & UD_INVERTPATTERN)
					{
						pDMTo->InvertPattern() = pDMFrom->InvertPattern();
					}
					if (dwChangedFlags[i] & UD_CHORDROOT)
					{
						pDMTo->ChordRoot() = pDMFrom->ChordRoot();
					}
					if(dwChangedFlags[i] & UD_SCALEROOT)
					{
						pDMTo->ScaleRoot() = pDMFrom->ScaleRoot();
					}
					if (dwChangedFlags[i] & UD_FLAT)
					{
						pDMTo->UseFlat() = pDMFrom->UseFlat();
					}
					if (dwChangedFlags[i] & UD_LEVELS)
					{
						pDMTo->Levels() = pDMFrom->Levels();
					}
					if (dwChangedFlags[i] & UD_INVERT)
					{
						pDMTo->Bits() &= ~CHORD_INVERT;
						pDMTo->Bits() |= (pDMFrom->Bits() & CHORD_INVERT);
					}
					if (dwChangedFlags[i] & UD_FOUR)
					{
						pDMTo->Bits() &= ~CHORD_FOUR;
						pDMTo->Bits() |= (pDMFrom->Bits() & CHORD_FOUR);
					}
					if (dwChangedFlags[i] & UD_UPPER)
					{
						pDMTo->Bits() &= ~CHORD_UPPER;
						pDMTo->Bits() |= (pDMFrom->Bits() & CHORD_UPPER);
					}
				}
			}
			else
			{
				SendEditNotification(IDS_UNDO_EditChordMapChord);	
			
				for( ChordEntry *pChord = m_pChordMapList->GetHead(); pChord != NULL; pChord = pChord->GetNext() )
				{
	//				DMChord* pdm = pChord->m_chordsel.SubChord(pChord->m_chordsel.RootIndex());
					if( pChord->m_chordsel.KeyDown() != 1 ) continue;

					
					// Only refresh if UI elements have changed.
					fRefresh =	((pChord->m_chordsel.Base()->ChordRoot() != m_SelectedChord.Base()->ChordRoot()) ||
							(strcmp(pChord->m_chordsel.Name(), m_SelectedChord.Name())) ||
							pChord->m_chordsel.Base()->UseFlat() != m_SelectedChord.Base()->UseFlat());

					for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
					{
						DMChord* pDMFrom = pNewChord->SubChord(i);
						DMChord* pDMTo = pChord->m_chordsel.SubChord(i);

						if(pChord->IsSignPost())
						{
							if(i == pNewChord->RootIndex())
							{
								bStructureChange = (pDMFrom->ChordRoot() != pDMTo->ChordRoot());
							}

							bStructureChange = bStructureChange || (pDMFrom->ChordPattern() != pDMTo->ChordPattern());
						}

						dwChangedFlags[i] = ~pNewChord->GetUndetermined(i);
						if (dwChangedFlags[i] & UD_NAME)
						{
							strcpy(pChord->m_chordsel.Name(), pNewChord->Name());
						}
						if (dwChangedFlags[i] & UD_CHORDPATTERN)
						{
							pDMTo->ChordPattern() = pDMFrom->ChordPattern();
						}
						if (dwChangedFlags[i] & UD_SCALEPATTERN)
						{
							pDMTo->ScalePattern() = pDMFrom->ScalePattern();
						}
						if(dwChangedFlags[i] & UD_INVERTPATTERN)
						{
							pDMTo->InvertPattern() = pDMFrom->InvertPattern();
						}
						if (dwChangedFlags[i] & UD_CHORDROOT)
						{
							pDMTo->ChordRoot() = pDMFrom->ChordRoot();
						}
						if(dwChangedFlags[i] & UD_SCALEROOT)
						{
							pDMTo->ScaleRoot() = pDMFrom->ScaleRoot();
						}
						if (dwChangedFlags[i] & UD_FLAT)
						{
							pDMTo->UseFlat() = pDMFrom->UseFlat();
						}
						if (dwChangedFlags[i] & UD_LEVELS)
						{
							pDMTo->Levels() = pDMFrom->Levels();
						}
						if (dwChangedFlags[i] & UD_INVERT)
						{
							pDMTo->Bits() &= ~CHORD_INVERT;
							pDMTo->Bits() |= (pDMFrom->Bits() & CHORD_INVERT);
						}
						if (dwChangedFlags[i] & UD_FOUR)
						{
							pDMTo->Bits() &= ~CHORD_FOUR;
							pDMTo->Bits() |= (pDMFrom->Bits() & CHORD_FOUR);
						}
						if (dwChangedFlags[i] & UD_UPPER)
						{
							pDMTo->Bits() &= ~CHORD_UPPER;
							pDMTo->Bits() |= (pDMFrom->Bits() & CHORD_UPPER);
						}
					}
				}
			}
		}

		if( bStructureChange )
		{
			m_pTimeline->OnDataChanged(new CheckForOrphansNotification);
		}
	}

	if( fRefresh )
	{
		m_pTimeline->StripInvalidateRect(m_pChordMapStrip, NULL, TRUE);

		if(pNewChord)
		{
			ChordChangeCallback* pCallback = new ChordChangeCallback(ChordChangeCallback::ChordMapNewSelection);
			ChordEntry* pChordEntry = new ChordEntry;
			PropChordToChord(pChordEntry, pNewChord);
			pCallback->SetChordEntry(pChordEntry);
			m_pTimeline->OnDataChanged( pCallback );
		}
		else
		{
			// chord connection has changed, just update personality
			m_pTimeline->OnDataChanged( NULL );
		}
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::OnShowProperties( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	HRESULT hr = S_OK;
	if(m_selection == ChordSelected || m_selection == PaletteSelected || 
		m_selection == MultipleSelections || m_selection == NoSelection)
	{
		if( m_pChordPropPageMgr == NULL )
		{
			CChordPropPageMgr* pPPM = new CChordPropPageMgr(m_pJazzFramework);
			if( NULL == pPPM ) return E_OUTOFMEMORY;
			hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pChordPropPageMgr );
			m_pChordPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
			if( FAILED(hr) )
				return hr;
		}
		hr = m_pTimeline->SetPropertyPage(m_pChordPropPageMgr, (IDMUSProdPropPageObject*)this);
		m_pChordPropPageMgr->RefreshData();
	}
	else if(m_selection == ConnectionSelected)
	{
		if( m_pConnectionPropPageMgr == NULL )
		{
			CConnectionPropPageMgr* pPPM = new CConnectionPropPageMgr;
			if( NULL == pPPM ) return E_OUTOFMEMORY;
			hr = pPPM->QueryInterface( IID_IDMUSProdPropPageManager, (void**)&m_pConnectionPropPageMgr );
			m_pConnectionPropPageMgr->Release(); // this releases the 2nd ref, leaving only one
			if( FAILED(hr) )
				return hr;
		}
		hr = m_pTimeline->SetPropertyPage(m_pConnectionPropPageMgr, (IDMUSProdPropPageObject*)this);
		m_pConnectionPropPageMgr->RefreshData();
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::OnRemoveFromPageManager( void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CChordMapMgr::PreparePropChord()
{
	BOOL	fFirst = TRUE;
	CPropChord tempChord;

	for( ChordEntry *pChord = m_pChordMapList->GetHead(); pChord != NULL; pChord = pChord->GetNext() )
	{
		// Only add selected chords to the Chord template.
		if( pChord->m_chordsel.KeyDown() != 1 ) continue;

		ZeroMemory( &tempChord, sizeof(tempChord) );

//		ChordToPropChord( &tempChord, pChord );
		dynamic_cast<DMPolyChord&>(tempChord) = dynamic_cast<DMPolyChord&>(pChord->m_chordsel);

		if( fFirst == TRUE )
		{
			ZeroMemory( &m_SelectedChord, sizeof(m_SelectedChord) );
			m_SelectedChord = tempChord;
			fFirst = FALSE;
			// m_SelectedChord.m_dwUndetermined = 0; 	// ZeroMemory() handles this.
		}
		else
		{
			tempChord.CopyToPropChord( &m_SelectedChord, CPropChord::ALL );
		}
	}

	return S_OK;
}


void CChordMapMgr::GetLastEdit(CString & str)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(m_pChordMapStrip);
	str = "";

	if(m_pChordMapStrip)
	{
		str.LoadString( m_pChordMapStrip->m_nLastEdit );
	}
}


void CChordMapMgr::SendEditNotification(UINT type)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(m_pChordMapStrip);

	if(m_pChordMapStrip)
	{
		HINSTANCE hinst = _Module.GetModuleInstance();
		TCHAR buf[256];
		m_pChordMapStrip->m_nLastEdit = type;
		::LoadString(hinst, m_pChordMapStrip->m_nLastEdit, buf, 256);
		if(strlen(buf) > 0)
		{
			PreEditNotification* pEditNotify = new PreEditNotification(buf);
			m_pTimeline->OnDataChanged( pEditNotify );
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

void CChordMapMgr::SetUpNextChord(short nidFrom, short nidTo)
{
	bool bDone = false;
	for(ChordEntry *pchord = m_pChordMapList->GetHead();
		pchord != NULL && !bDone;
		pchord = pchord->GetNext())
	{
		for(NextChord *pnext = pchord->m_nextchordlist.GetHead();
			pnext != NULL;
			pnext = pnext->GetNext()
			)
		{
			if(pnext->m_nid == nidTo && pnext->m_parent->m_nid == nidFrom)
			{
				m_pSelectedConnection = pnext;
				bDone = true;	// force break from outer loop
				break;
			}
		} 
	}
}


bool CChordMapMgr::IsPropPageShowing()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if(!m_pJazzFramework)
	{
		return false;	// no framework then proppage isn't showing
	}
	/*
	if( m_pPropPageMgr == NULL )
	{
		return false;
	}
	*/

	// check PropertyPage
	IDMUSProdPropSheet* pJPS;
	bool rc = false;
	if( SUCCEEDED(m_pJazzFramework->QueryInterface( IID_IDMUSProdPropSheet,
		(void**)&pJPS )))
	{
		if(pJPS->IsShowing() == S_OK&& pJPS->IsEqualPageManagerObject((IDMUSProdPropPageObject*)this) == S_OK)
			rc = true;
		else
			rc = false;
		pJPS->Release();
	}

	return rc;

}

HRESULT CChordMapMgr::ShowPropPage(BOOL fShow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(m_pJazzFramework);
	HRESULT hr = E_FAIL;
	if(m_pJazzFramework == NULL)
	{
		return false;
	}

	// SetPropertyPage
	if( m_pJazzFramework )
	{
		IDMUSProdPropSheet* pJPS;
		if( SUCCEEDED(hr = m_pJazzFramework->QueryInterface( IID_IDMUSProdPropSheet,
			(void**)&pJPS )))
		{
			pJPS->Show(fShow);
			pJPS->Release();
		}
	}

	return hr;

}



void CChordMapMgr::MakeCompatibleNids(ChordEntryList &list)
// makes nids of ChordEntries in list compatabile with existing chord entries
{
	int maxnid = -1;

	for(ChordEntry* pchord = list.GetHead(); pchord; pchord = pchord->GetNext())
	{
		pchord->m_nid += m_nIDCounter;
		if(maxnid < pchord->m_nid)
		{
			maxnid = pchord->m_nid;
		}
		for(NextChord* pnext = pchord->m_nextchordlist.GetHead(); pnext; pnext = pnext->GetNext())
		{
			if(pnext->m_nid > 0)
			{
				pnext->m_nid += m_nIDCounter;
				if(maxnid < pnext->m_nid)
				{
					maxnid = pnext->m_nid;
				}
			}
		}
	}
	m_nIDCounter = maxnid + 1;
}

bool CChordMapMgr::LessThan(const ChordEntry& a, const ChordEntry&b )
{
	CMusicTimeConverter cmta(a.m_chordsel.Measure(), a.m_chordsel.Beat(), m_pTimeline, m_dwGroupBits);
	CMusicTimeConverter cmtb(b.m_chordsel.Measure(), b.m_chordsel.Beat(), m_pTimeline, m_dwGroupBits);
	return cmta < cmtb;
}

bool CChordMapMgr::GreaterThan(const ChordEntry& a, const ChordEntry&b )
{
	CMusicTimeConverter cmta(a.m_chordsel.Measure(), a.m_chordsel.Beat(), m_pTimeline, m_dwGroupBits);
	CMusicTimeConverter cmtb(b.m_chordsel.Measure(), b.m_chordsel.Beat(), m_pTimeline, m_dwGroupBits);
	return cmta > cmtb;
}

long CChordMapMgr::AbsDiff(const ChordEntry& a, const ChordEntry&b)
{
	DMUS_TIMESIGNATURE dmTimeSig;
	m_pTimeline->GetParam(GUID_TimeSignature, m_dwGroupBits, 0, 0,0, &dmTimeSig);
	long beatsa = a.m_chordsel.Measure() * dmTimeSig.bBeatsPerMeasure + a.m_chordsel.Beat();
	long beatsb = b.m_chordsel.Measure() * dmTimeSig.bBeatsPerMeasure + b.m_chordsel.Beat();
	return beatsa >= beatsb ? beatsa - beatsb : beatsb - beatsa;
}

void CChordMapMgr::DetectAndReverseConnections(ChordEntryList &list)
{
	for(ChordEntry* pChord = list.GetHead(); pChord; pChord = pChord->GetNext())
	{
		NextChord* pNext = 0;
		for(pNext = pChord->m_nextchordlist.GetHead(); pNext; pNext = pNext->GetNext())
		{
			if(pNext->m_nextchord == NULL)
			{
				// this is the empty connection used for constructing new connects
				continue;
			}
			ChordEntry& parent = *(pNext->m_parent);
			ChordEntry& target = *(pNext->m_nextchord);
			if(LessThan(parent,target))
			{
				// order is ok
				continue;
			}
			if(GreaterThan(parent, target))
			{
				NextChord* pNewNext = new NextChord;
				pNewNext->m_nid = pNext->m_parent->m_nid;
				pNewNext->m_parent = pNext->m_nextchord;
				pNewNext->m_nextchord = pNext->m_parent;
				pNewNext->m_nminbeats = pNewNext->m_nmaxbeats = static_cast<short>(AbsDiff(parent, target));
				/*
				// sign post check and corrections
				if(parent.m_dwflags & CE_START)
				{
					parent.m_dwflags |= CE_END;
				}
				else if(target.m_dwflags & CE_END)
				{
					parent.m_dwflags |= CE_START;
				}
				*/
				// add new connection to target
				target.m_nextchordlist.AddHead(pNewNext);
			}
			// mark old nextchord for deletion
			pNext->m_dwflags |= NC_DELETE;
			// add new next chord to target

		}
		NextList nextlist;
		for(pNext = pChord->m_nextchordlist.RemoveHead(); pNext; pNext = pChord->m_nextchordlist.RemoveHead())
		{
			if(!(pNext->m_dwflags & NC_DELETE))
			{

				nextlist.AddTail(pNext);
			}
			else
			{
				delete pNext;
			}
		}
		pChord->m_nextchordlist.Cat(&nextlist);
	}
}


void CChordMapMgr::HitTest(RECT &rect)
{
	RECT rectSelect;
	long x;
	memcpy(&rectSelect, &rect, sizeof(RECT));


	if(rectSelect.top > rectSelect.bottom)
	{
		x = rectSelect.bottom;
		rectSelect.bottom = rectSelect.top;
		rectSelect.top = x;
	}
	if(rectSelect.left > rectSelect.right)
	{
		x = rectSelect.right;
		rectSelect.right = rectSelect.left;
		rectSelect.left = x;
	}

	for(ChordEntry* pChord = m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
//		TRACE("bound rect:(%d,%d,%d,%d), chord upper left(%d,%d)\r\n",
//			rectSelect.left, rectSelect.top, rectSelect.right, rectSelect.bottom,
//			pChord->m_rect.left, pChord->m_rect.top);

		bool bIntersectsAtTop = (rectSelect.top < pChord->m_rect.top 
										&& pChord->m_rect.top < rectSelect.bottom);

		bool bIntersectsAtLeft = (rectSelect.left < pChord->m_rect.left 
										&& pChord->m_rect.left < rectSelect.right);

		bool bIntersectsAtBottom = (rectSelect.bottom > pChord->m_rect.bottom 
										&& pChord->m_rect.bottom > rectSelect.top);

		if(bIntersectsAtLeft && (bIntersectsAtBottom || bIntersectsAtTop))
		{
			pChord->m_chordsel.KeyDown() = TRUE;
		}
		else
		{
			pChord->m_chordsel.KeyDown() = FALSE;
		}
	}
}

void CChordMapMgr::GetBoundariesOfSelectedChords(long& lStartTime, long& lEndTime)
{
	VARIANT vtInit;
	vtInit.vt = VT_I4;
	m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &vtInit );
	long lLength = V_I4(&vtInit);

	lEndTime = 0;
	lStartTime = lLength;
	if(m_pChordMapList == 0)
		return;

	for(ChordEntry* pChord = m_pChordMapList->GetHead(); pChord; pChord = pChord->GetNext())
	{
		if(pChord->m_chordsel.KeyDown() == TRUE)
		{
			long lTime, clocksPerBeat;
			m_pTimeline->MeasureBeatToClocks(m_dwGroupBits, 0, pChord->m_chordsel.Measure(), pChord->m_chordsel.Beat(), &lTime);
			m_pTimeline->MeasureBeatToClocks(m_dwGroupBits, 0, 0, 1, &clocksPerBeat);
			lStartTime = lTime < lStartTime ? lTime : lStartTime;
			lEndTime = (lTime+clocksPerBeat) > lEndTime ? (lTime+clocksPerBeat) : lEndTime;
		}
	}
}
