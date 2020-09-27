// MuteMgr.cpp : Implementation of CMuteMgr
#include "stdafx.h"
#include "MuteStripMgr.h"
#include "MuteMgr.h"
#include "MuteItem.h"
#include "DLLJazzDataObject.h"
#include <dmusici.h>
#include <dmusicf.h>
#include <mmreg.h>
#include "SegmentGuids.h"
#include "SegmentIO.h"
#include "RiffStrm.h"

#define contains(a, b, x) ((x >= a) && (x <= b))

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr constructor/destructor 

CMuteMgr::CMuteMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Initialize all our pointers to NULL
	m_pTimeline = NULL;
	m_pDMProdFramework = NULL;
	m_pPropPageMgr = NULL;
	m_pCopyDataObject = NULL;
	m_pMuteStrip = NULL;
	m_pIDMTrack = NULL;
	m_pDMProdSegmentNode = NULL;

	// Initially we don't need to be saved
	m_fDirty = false;

	// Initialize flags
	m_fTimeSigChange = false;

	// By default, belong to Group 1
	m_dwGroupBits = 1;
	m_dwOldGroupBits = 1;

	// Set up default track flags
	m_dwTrackExtrasFlags = DMUS_TRACKCONFIG_DEFAULT & TRACKCONFIG_VALID_MASK;
	m_dwProducerOnlyFlags = 0;

	// Create a MuteStrip
	m_pMuteStrip = new CMuteStrip(this);
	ASSERT(m_pMuteStrip);
}

CMuteMgr::~CMuteMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Clean up our references
	if (m_pDMProdFramework)
	{
		m_pDMProdFramework->Release();
		m_pDMProdFramework = NULL;
	}

	ASSERT(m_pTimeline == NULL);

	// No Release!
	/*
	if( m_pDMProdSegmentNode )
	{
		m_pDMProdSegmentNode->Release();
		m_pDMProdSegmentNode = NULL;
	}
	*/

	if (m_pPropPageMgr)
	{
		m_pPropPageMgr->Release();
		m_pPropPageMgr = NULL;
	}
	if (m_pIDMTrack)
	{
		m_pIDMTrack->Release();
		m_pIDMTrack = NULL;
	}
	if (m_pMuteStrip)
	{
		m_pMuteStrip->Release();
		m_pMuteStrip = NULL;
	}

	// Delete all mute items
	EmptyMuteList();

	// If we have an object on the clipboard, make sure it doesn't have any references
	// back to us.
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
// CMuteMgr IDMUSProdStripMgr implementation

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::GetParam

HRESULT STDMETHODCALLTYPE CMuteMgr::GetParam(
		/* [in] */	REFGUID 	rguidType,
		/* [in] */	MUSIC_TIME	mtTime,
		/* [out] */ MUSIC_TIME* pmtNext,
		/* [out] */ void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (::IsEqualGUID(rguidType, GUID_MuteParam))
	{
		if (pData == NULL)
		{
			return E_POINTER;
		}
		CMuteItem *pItem = NULL;
		CMuteItem *pFoundItem = NULL;

		POSITION pos = m_lstMutes.GetHeadPosition();
		while (pos) {
			pItem = m_lstMutes.GetNext(pos);
			if (pItem->GetStartTime() > mtTime && 
				(pFoundItem == NULL || pFoundItem->GetStartTime() > pItem->GetStartTime())) 
			{
				pFoundItem = pItem;
				if (pmtNext != NULL) 
					*pmtNext = pFoundItem->GetStartTime();
			}
		}
		if (pFoundItem != NULL) {
			DMUS_MUTE_PARAM *pMuteParam;
			pMuteParam = (DMUS_MUTE_PARAM*)pData;
			pMuteParam->dwPChannel = pFoundItem->GetPChannel();
			pMuteParam->dwPChannelMap = pFoundItem->GetPChannelMap();
			return S_OK;
		}
		else {
			if (pmtNext != NULL) {
				*pmtNext = 0;
			}    
			pData = NULL;
			return E_FAIL;
		}
	}

	if( ::IsEqualGUID( rguidType, GUID_Segment_Undo_BSTR ) )
	{
		BSTR bstr;
		CString str;
		str.LoadString(m_pMuteStrip->m_nLastEdit);
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

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::SetParam

HRESULT STDMETHODCALLTYPE CMuteMgr::SetParam(
		/* [in] */ REFGUID		guidType,
		/* [in] */ MUSIC_TIME	mtTime,
		/* [in] */ void*		pData)
{
	UNREFERENCED_PARAMETER(mtTime);
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( ::IsEqualGUID( guidType, GUID_DocRootNode ) )
	{
		m_pDMProdSegmentNode = reinterpret_cast<IDMUSProdNode*>(pData);
		// No addref!
		//m_pDMProdSegmentNode->AddRef();
		return S_OK;
	}

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::IsParamSupported

HRESULT STDMETHODCALLTYPE CMuteMgr::IsParamSupported(
		/* [in] */ REFGUID		rguidType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( ::IsEqualGUID(rguidType, GUID_MuteParam)
	||  ::IsEqualGUID(rguidType, GUID_Segment_Undo_BSTR)
	||	::IsEqualGUID( rguidType, GUID_DocRootNode ) )
	{
		return S_OK;
	}
	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::OnUpdate

HRESULT STDMETHODCALLTYPE CMuteMgr::OnUpdate(
		/* [in] */  REFGUID		rguidType,
		/* [in] */  DWORD		dwGroupBits,
		/* [in] */	void*		pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( !(dwGroupBits & m_dwGroupBits) )
	{
		return E_INVALIDARG;
	}

	// TimeSig change
	if(::IsEqualGUID(rguidType, GUID_TimeSignature))
	{
		// Update start/end times of all Mute events
		m_fTimeSigChange = true;
		RecomputeMuteTimes();
		m_fTimeSigChange = false;

		// Refresh property page
		if( m_pPropPageMgr )
		{
			m_pPropPageMgr->RefreshData();
		}
		OnDataChanged();

		// Redraw our strip
		m_pTimeline->StripInvalidateRect(m_pMuteStrip, NULL, TRUE);
		return S_OK;
	}
	if (::IsEqualGUID(rguidType, GUID_Segment_AllTracksAdded))
	{
		// Update the Measure/beat value of all Mute events
		RecomputeMuteMeasureBeats();

		// Redraw our strip
		m_pTimeline->StripInvalidateRect(m_pMuteStrip, NULL, TRUE);
		
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::GetStripMgrProperty

HRESULT STDMETHODCALLTYPE CMuteMgr::GetStripMgrProperty(
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
		else
		{
			DMUS_IO_TRACK_HEADER *pioTrackHeader = static_cast<DMUS_IO_TRACK_HEADER *>(V_BYREF( pVariant ));
			if( pioTrackHeader == NULL )
			{
				return E_POINTER;
			}

			pioTrackHeader->guidClassID = CLSID_DirectMusicMuteTrack;
			pioTrackHeader->dwPosition = 0;
			pioTrackHeader->dwGroup = m_dwGroupBits;
			pioTrackHeader->ckid = DMUS_FOURCC_MUTE_CHUNK;
			pioTrackHeader->fccType = NULL;
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


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::SetStripMgrProperty

HRESULT STDMETHODCALLTYPE CMuteMgr::SetStripMgrProperty(
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
		if (m_pTimeline)
		{
			m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)this);
			if (m_pPropPageMgr)
			{
				m_pPropPageMgr->Release();
				m_pPropPageMgr = NULL;
			}
			if (m_pMuteStrip)
			{
				m_pMuteStrip->UpdateStateData();
				m_pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)m_pMuteStrip);
				m_pTimeline->RemoveStrip((IDMUSProdStrip *)m_pMuteStrip);
			}
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwOldGroupBits );
			m_pTimeline->RemoveFromNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwOldGroupBits );
			m_pTimeline->Release();
			m_pTimeline = NULL;
		}

		if (V_UNKNOWN( &variant ))
		{
			if (FAILED(V_UNKNOWN( &variant )->QueryInterface(IID_IDMUSProdTimeline, 
				(void**)&m_pTimeline)))
			{
				return E_FAIL;
			}
			else
			{
				// Add the strip to the timeline
				m_pTimeline->InsertStripAtDefaultPos((IDMUSProdStrip*)m_pMuteStrip, 
					CLSID_DirectMusicMuteTrack, m_dwGroupBits, 0);
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_Segment_AllTracksAdded, m_dwGroupBits );
				m_pTimeline->AddToNotifyList( (IDMUSProdStripMgr *)this, GUID_TimeSignature, m_dwGroupBits );
				m_pMuteStrip->ApplyStateData();
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
		else
		{
			DMUS_IO_TRACK_HEADER *pioTrackHeader = static_cast<DMUS_IO_TRACK_HEADER *>(V_BYREF( &variant ));
			if( pioTrackHeader == NULL )
			{
				return E_POINTER;
			}
			m_dwGroupBits = pioTrackHeader->dwGroup;
			m_dwOldGroupBits = pioTrackHeader->dwGroup;
			if (m_pMuteStrip)
			{
				m_pMuteStrip->UpdateName();
			}
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
// CMuteMgr IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::GetClassID

HRESULT CMuteMgr::GetClassID(CLSID* pClsId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (pClsId == NULL)
	{
		return E_POINTER;
	}

	// return our CLSID
	memcpy(pClsId, &CLSID_MuteMgr, sizeof(CLSID));

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::IsDirty

HRESULT CMuteMgr::IsDirty()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_fDirty ? S_OK : S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::Load

HRESULT CMuteMgr::Load(IStream* pIStream)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT( m_pMuteStrip != NULL );

	if (pIStream == NULL)
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Check for Direct Music format
	if (FAILED(hr = AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		return hr;
	}

	// Remove all existing Mutes
	EmptyMuteList();

	MMCKINFO	ck;
	DWORD		dwByteCount;
	DWORD		dwMuteSize;
	DMUS_IO_MUTE iMute;
	long		lChunkSize;

	if (m_pMuteStrip) m_pMuteStrip->m_fLoadedStateData = false;

	// Load the Track
	while (pIRiffStream->Descend(&ck, NULL, 0) == 0)
	{
		switch(ck.ckid) {
		case DMUS_FOURCC_MUTE_CHUNK:
		{
			hr = pIStream->Read(&dwMuteSize, sizeof(DWORD), &dwByteCount);
			if (FAILED(hr) || dwByteCount != sizeof(DWORD))
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			dwMuteSize = min(dwMuteSize, sizeof(DMUS_IO_MUTE));

			lChunkSize = ck.cksize - sizeof(DWORD);

			CMuteItem* pPrevItem = NULL;

			while (lChunkSize > 0)
			{
				ZeroMemory(&iMute, sizeof(DMUS_IO_MUTE));
				hr = pIStream->Read(&iMute, dwMuteSize, &dwByteCount);
				if (FAILED(hr) || dwByteCount != dwMuteSize)
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				lChunkSize -= dwMuteSize;

				if (iMute.dwPChannel == iMute.dwPChannelMap) {
					if (pPrevItem != NULL) {
						// this is an "end mute" item
						pPrevItem->SetEnd(iMute.mtTime);
					}
				}
				else {
					CMuteItem* pItem = new CMuteItem;
					if (pItem == NULL)
					{
						hr = E_OUTOFMEMORY;
						goto ON_ERROR;
					}

					pItem->SetStart(iMute.mtTime);
					pItem->SetPChannel(iMute.dwPChannel);
					pItem->SetPChannelMap(iMute.dwPChannelMap);
					//UpdateMuteMeasureBeat(pItem);
					InsertMuteItem(pItem);
					pPrevItem = pItem;
				}
			}
			pIRiffStream->Ascend(&ck, 0);
			break;
		}

		case DMUS_FOURCC_MUTE_UI_CHUNK:
		{
			// load design time info
			if (m_pMuteStrip) {
				hr = m_pMuteStrip->LoadStateData(pIRiffStream, &ck);
				if (FAILED(hr))	{
					goto ON_ERROR; 
				}
				m_pMuteStrip->m_fLoadedStateData = true;
			}
			pIRiffStream->Ascend(&ck, 0);
			break;
		}
	}
	}

	if( m_pMuteStrip 
	&&  m_pMuteStrip->m_fLoadedStateData == FALSE )
	{
		// Sync PChannel array
		m_pMuteStrip->SyncStateData();

		// ok...  StateData is all set up
		m_pMuteStrip->m_fLoadedStateData = TRUE;
	}
	
	SyncWithDM();

ON_ERROR:
	pIRiffStream->Release();
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::Save

HRESULT CMuteMgr::Save(IStream* pIStream, BOOL fClearDirty)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (pIStream == NULL)
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo	StreamInfo;
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if (SUCCEEDED(pIStream->QueryInterface(IID_IDMUSProdPersistInfo, (void **)&pPersistInfo)))
	{
		pPersistInfo->GetStreamInfo(&StreamInfo);
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}

	// We only support saving to a design-time stream (GUID_CurrentVersion) or a DirectMusic
	// stream that will be loaded into a DirectMusicMuteTrack (GUID_DirectMusicObject)
	if (!::IsEqualGUID(guidDataFormat, GUID_CurrentVersion) &&
		!::IsEqualGUID(guidDataFormat, GUID_DirectMusicObject))
	{
		return E_INVALIDARG;
	}

	// Now, finally save ourself
	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Alloc an IDMUSProdRIFFStream from the IStream
	if (FAILED(hr = AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		return hr;
	}

    MMCKINFO ckMain;

	// If the Mute list isn't empty, save it
	if (!m_lstMutes.IsEmpty())
	{
		// Create the chunk to store the Mute data
		ckMain.ckid = DMUS_FOURCC_MUTE_CHUNK;
		if (pIRiffStream->CreateChunk(&ckMain, 0) != 0)
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write the structure size
		DWORD dwBytesWritten;
		DWORD dwMuteSize;
		dwMuteSize = sizeof(DMUS_IO_MUTE);
		hr = pIStream->Write(&dwMuteSize, sizeof(DWORD), &dwBytesWritten);
		if (FAILED(hr) || dwBytesWritten != sizeof(DWORD))
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		POSITION pos = m_lstMutes.GetHeadPosition();
		while (pos)
		{
			CMuteItem* pMuteItem = m_lstMutes.GetNext(pos);
			if (pMuteItem)
			{
				DMUS_IO_MUTE oMute;
				// Clear out the structure (clears out the padding bytes as well).
				ZeroMemory(&oMute, sizeof(DMUS_IO_MUTE));
				
				oMute.mtTime = pMuteItem->GetStartTime();
				oMute.dwPChannel = pMuteItem->GetPChannel();
				oMute.dwPChannelMap = pMuteItem->GetPChannelMap();
				hr = pIStream->Write(&oMute, sizeof(DMUS_IO_MUTE), &dwBytesWritten);
				if (FAILED(hr) || dwBytesWritten != sizeof(DMUS_IO_MUTE))
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// write an item to "end" the mute.
				oMute.mtTime = pMuteItem->GetEndTime();
				oMute.dwPChannel = pMuteItem->GetPChannel();
				oMute.dwPChannelMap = pMuteItem->GetPChannel();
				hr = pIStream->Write(&oMute, sizeof(DMUS_IO_MUTE), &dwBytesWritten);
				if (FAILED(hr) || dwBytesWritten != sizeof(DMUS_IO_MUTE))
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}
		// Ascend out of the Mute LIST chunk.
		pIRiffStream->Ascend(&ckMain, 0);
	}
	// save design time info
	if (StreamInfo.ftFileType == FT_DESIGN)
	{
		if (m_pMuteStrip) {
			hr = m_pMuteStrip->SaveStateData(pIRiffStream);
			if (FAILED(hr))
			{
				goto ON_ERROR;
			}
		}
	}

ON_ERROR:
	pIRiffStream->Release();
	if (fClearDirty) m_fDirty = false;
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::GetSizeMax

HRESULT CMuteMgr::GetSizeMax(ULARGE_INTEGER FAR* pcbSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Compute size of stream needed to persist ourself into.
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::GetData
//
// See CMuteStrip::GetData()
HRESULT STDMETHODCALLTYPE CMuteMgr::GetData(/* [retval][out] */ void **ppData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::SetData
//
HRESULT STDMETHODCALLTYPE CMuteMgr::SetData(/* [in] */ void *pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::OnShowProperties

HRESULT STDMETHODCALLTYPE CMuteMgr::OnShowProperties(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::OnRemoveFromPageManager

HRESULT STDMETHODCALLTYPE CMuteMgr::OnRemoveFromPageManager(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// If you want to do something special when your property page is no longer
	// displayed, do it here.
	return E_NOTIMPL;
}



/////////////////////////////////////////////////////////////////////////////
// CMuteMgr implementation
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::SaveRange
//
// Saves all mute items (or parts of them) that overlap the given begin and 
// end times.  
//
HRESULT CMuteMgr::SaveRange(IStream* pIStream, MUSIC_TIME mtStart, MUSIC_TIME mtEnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (pIStream == NULL)
	{
		return E_INVALIDARG;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;

	// Alloc an IDMUSProdRIFFStream from the IStream
	if (FAILED(hr = AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		return hr;
	}

    MMCKINFO ckMain;

	// If the Mute list isn't empty, save it
	if (!m_lstMutes.IsEmpty())
	{
		// Create the chunk to store the Mute data
		ckMain.ckid = DMUS_FOURCC_MUTE_CHUNK;
		if (pIRiffStream->CreateChunk(&ckMain, 0) != 0)
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write the structure size
		DWORD dwBytesWritten;
		DWORD dwMuteSize;
		dwMuteSize = sizeof(DMUS_IO_MUTE);
		hr = pIStream->Write(&dwMuteSize, sizeof(DWORD), &dwBytesWritten);
		if (FAILED(hr) || dwBytesWritten != sizeof(DWORD))
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		POSITION pos = m_lstMutes.GetHeadPosition();
		while (pos)
		{
			CMuteItem* pMuteItem = m_lstMutes.GetNext(pos);
			if (pMuteItem->Intersects(mtStart, mtEnd)) {
				// the mute item overlaps the selection area, so save it
				DMUS_IO_MUTE oMute;
				// Clear out the structure (clears out the padding bytes as well).
				ZeroMemory(&oMute, sizeof(DMUS_IO_MUTE));
				
				oMute.mtTime = max(pMuteItem->GetStartTime(), mtStart);
				oMute.dwPChannel = pMuteItem->GetPChannel();
				oMute.dwPChannelMap = pMuteItem->GetPChannelMap();
				hr = pIStream->Write(&oMute, sizeof(DMUS_IO_MUTE), &dwBytesWritten);
				if (FAILED(hr) || dwBytesWritten != sizeof(DMUS_IO_MUTE))
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// write an item to "end" the mute.
				oMute.mtTime = min(pMuteItem->GetEndTime(), mtEnd);
				oMute.dwPChannel = pMuteItem->GetPChannel();
				oMute.dwPChannelMap = pMuteItem->GetPChannel();
				hr = pIStream->Write(&oMute, sizeof(DMUS_IO_MUTE), &dwBytesWritten);
				if (FAILED(hr) || dwBytesWritten != sizeof(DMUS_IO_MUTE))
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}
		// Ascend out of the Mute LIST chunk.
		pIRiffStream->Ascend(&ckMain, 0);
	}
	// save design time info
	if (m_pMuteStrip) {
		hr = m_pMuteStrip->SaveStateData(pIRiffStream);
		if (FAILED(hr))
		{
			goto ON_ERROR;
		}
	}

ON_ERROR:
	pIRiffStream->Release();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::ShiftMuteItems
//
// Shifts all mute items (or parts of them) that have data after the given
// start time.  Mute items that move out of bounds of the timeline are 
// resized or deleted as appropriate.
// 
void CMuteMgr::ShiftMuteItems(MUSIC_TIME mtStart, MUSIC_TIME mtOffset)
{
	// got max time
	VARIANT var;
	VERIFY(SUCCEEDED(m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var)));
	MUSIC_TIME mtMax = V_I4(&var);
	CTypedPtrList<CPtrList, CMuteItem*> lstAdded;
	
	CMuteItem* pItem = NULL;
	POSITION pos = m_lstMutes.GetHeadPosition();
	while (pos) {
		POSITION posItem = pos;
		pItem = m_lstMutes.GetNext(pos);
		if (pItem->GetStartTime() >= mtStart) {
			// offset the times
			pItem->SetStart(max(0, pItem->GetStartTime() + mtOffset));
			pItem->SetEnd(min(mtMax, pItem->GetEndTime() + mtOffset));
		}
		else if (pItem->GetEndTime() > mtStart) {
			// only the end time is within the time range to be shifted,
			// so this item must be split apart.
			CMuteItem* newItem = new CMuteItem;
			if (newItem == NULL) return;
			newItem->SetPChannel(pItem->GetPChannel());
			newItem->SetPChannelMap(pItem->GetPChannelMap());
			newItem->SetStart(max(0, mtStart + mtOffset));
			newItem->SetEnd(min(mtMax, pItem->GetEndTime() + mtOffset));
			UpdateMuteMeasureBeat(newItem);
			pItem->SetEnd(mtStart);
			lstAdded.AddTail(newItem);
		}
		UpdateMuteMeasureBeat(pItem);
		if (pItem->IsUseless()) {
			// delete items that no longer take up at least a whole beat
			m_lstMutes.RemoveAt(posItem);
			delete pItem;
		}
	}
	// merge any newly created items with the existing ones
	pos = lstAdded.GetHeadPosition();
	while (pos) {
		CMuteItem* pAddedItem = lstAdded.GetNext(pos);
		MergeMuteItem(pAddedItem);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::DeleteRange
//
// Deletes mute items (or "parts" of mute items) that overlap any part of the
// given time span.
// 
void CMuteMgr::DeleteRange(MUSIC_TIME mtStart, MUSIC_TIME mtEnd)
{
	// insert the mute item; sorted by pchannel.
	CMuteItem* pItem = NULL;
	POSITION pos = m_lstMutes.GetHeadPosition();
	while (pos) {
		POSITION posItem = pos;
		pItem = m_lstMutes.GetNext(pos);

		// adjust start time if necessary
		if (contains(mtStart, mtEnd, pItem->GetStartTime())) {
			pItem->SetStart(mtEnd);
		}
		// adjust end time if necessary
		if (contains(mtStart, mtEnd, pItem->GetEndTime())) {
			pItem->SetEnd(mtStart);
		}
		// split item if necessary
		if (pItem->GetStartTime() < mtStart && pItem->GetEndTime() > mtEnd)  {
			CMuteItem* newItem = new CMuteItem();
			if (newItem == NULL) return;
			newItem->SetPChannel(pItem->GetPChannel());
			newItem->SetPChannelMap(pItem->GetPChannelMap());
			newItem->SetStart(mtEnd);
			newItem->SetEnd(pItem->GetEndTime());
			UpdateMuteMeasureBeat(newItem);
			pItem->SetEnd(mtStart);
			InsertMuteItem(newItem);
		}
		UpdateMuteMeasureBeat(pItem);
		if (pItem->IsUseless()) {
			// delete items that no longer take up at least a whole beat
			m_lstMutes.RemoveAt(posItem);
			delete pItem;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::LoadPaste
//
// Pastes the items in the given stream to the given mtPos.  mtStart and
// mtEnd are the boundaries of items on the stream (can be greater than the
// timespans of the mute items on the stream).  mtPos is the time where
// insertion of loaded mute items should be begin.
//
HRESULT	CMuteMgr::LoadPaste(IStream* pIStream, MUSIC_TIME mtPos, MUSIC_TIME mtStart, MUSIC_TIME mtEnd)
{

	IDMUSProdRIFFStream* pIRiffStream;
	HRESULT hr = E_FAIL;
	CTypedPtrList<CPtrList, CMuteItem*> lstMutes;
	MMCKINFO	ck;
	DWORD		dwByteCount;
	DWORD		dwMuteSize;
	DMUS_IO_MUTE iMute;
	long		lChunkSize;
	MUSIC_TIME	mtOffset = mtPos - mtStart;
	TIMELINE_PASTE_TYPE	pasteType;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (pIStream == NULL) {
		return E_INVALIDARG;
	}
	ASSERT(m_pMuteStrip);
	ASSERT(mtStart < mtEnd);

	// Check for Direct Music format
	if (FAILED(hr = AllocRIFFStream(pIStream, &pIRiffStream)))
	{
		return hr;
	}

	// get max time
	VARIANT var;
	VERIFY(SUCCEEDED(m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var)));
	MUSIC_TIME mtMax = V_I4(&var);

	// get paste type
	VERIFY(SUCCEEDED(m_pTimeline->GetPasteType(&pasteType)));
	
	if (m_pMuteStrip) m_pMuteStrip->m_fLoadedStateData = false;

	/*
	if (pasteType == TL_PASTE_INSERT) {
		// move exisiting mute items in or after the time range over
		ShiftMuteItems(mtPos, (mtEnd - mtStart));
	}
	else*/
	if (pasteType == TL_PASTE_OVERWRITE) {
		// delete existing items that are in the way
		DeleteRange(mtPos, mtPos + (mtEnd - mtStart));
	}
	
	// Load the Track
	while (pIRiffStream->Descend(&ck, NULL, 0) == 0)
	{
		switch(ck.ckid) {
		case DMUS_FOURCC_MUTE_CHUNK:
		{
			hr = pIStream->Read(&dwMuteSize, sizeof(DWORD), &dwByteCount);
			if (FAILED(hr) || dwByteCount != sizeof(DWORD))
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			dwMuteSize = min(dwMuteSize, sizeof(DMUS_IO_MUTE));

			lChunkSize = ck.cksize - sizeof(DWORD);

			CMuteItem* pPrevItem = NULL;

			while (lChunkSize > 0)
			{
				ZeroMemory(&iMute, sizeof(DMUS_IO_MUTE));
				hr = pIStream->Read(&iMute, dwMuteSize, &dwByteCount);
				if (FAILED(hr) || dwByteCount != dwMuteSize)
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				lChunkSize -= dwMuteSize;

				if (iMute.dwPChannel == iMute.dwPChannelMap) {
					if (pPrevItem != NULL) {
						// this is an "end mute" item
						pPrevItem->SetEnd(min(mtMax, iMute.mtTime + mtOffset));
						UpdateMuteMeasureBeat(pPrevItem);
					}
				}
				else {
					CMuteItem* pItem = new CMuteItem;
					if (pItem == NULL)
					{
						hr = E_OUTOFMEMORY;
						goto ON_ERROR;
					}

					pItem->SetStart(min(mtMax, iMute.mtTime + mtOffset));
					pItem->SetPChannel(iMute.dwPChannel);
					pItem->SetPChannelMap(iMute.dwPChannelMap);
					lstMutes.AddTail(pItem);
					pPrevItem = pItem;
				}
			}
			// insert the mute items appropriately
			pIRiffStream->Ascend(&ck, 0);
			break;
		}

		case DMUS_FOURCC_MUTE_UI_CHUNK:
		{
			// load design time info
			if (m_pMuteStrip) {
				hr = m_pMuteStrip->MergeStateData(pIRiffStream, &ck);
				if (FAILED(hr))	{
					goto ON_ERROR; 
				}
				m_pMuteStrip->m_fLoadedStateData = true;
			}
			pIRiffStream->Ascend(&ck, 0);
			break;
		}
	}
	}
	
	// now actually merge the mute items into the existing ones
	{
		POSITION pos = lstMutes.GetHeadPosition();
		while (pos) {
			CMuteItem* pItem = lstMutes.GetNext(pos);
			if (pasteType == TL_PASTE_MERGE) {
				PasteMerge(pItem);
			}
			else {
				MergeMuteItem(pItem);
			}
		}
	}

	SyncWithDM();

ON_ERROR:
	pIRiffStream->Release();
	return hr;
	
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::PasteMerge
//
// Merge the given mute item with the existing mute items.  pMuteItem may
// overlap with already existing mute items.  This method will insert
// pMuteItem, possibly cutting it up into several items to fill in only beats
// that aren't already occupied by another item.
//
void CMuteMgr::PasteMerge(CMuteItem* pMuteItem)
{
	CTypedPtrList<CPtrList, CMuteItem*> lstMergeItems;
	lstMergeItems.AddTail(pMuteItem);

	POSITION pos = m_lstMutes.GetHeadPosition();
	while (pos) {
		if (lstMergeItems.IsEmpty()) break;
		POSITION posHere = pos;
		CMuteItem* pExistingItem = m_lstMutes.GetNext(pos);
		
		POSITION posMerge = lstMergeItems.GetHeadPosition();
		while (posMerge) {
			POSITION posMergeHere = posMerge;
			CMuteItem* pMergeItem = lstMergeItems.GetNext(posMerge);
			
			if( pMergeItem->GetPChannel() == pExistingItem->GetPChannel() )
			{
				if (pMergeItem->GetStartTime() >= pExistingItem->GetStartTime() &&
					pMergeItem->GetEndTime() <= pExistingItem->GetEndTime()) {
					// complete collision, so get rid of this item.
					lstMergeItems.RemoveAt(posMergeHere);
					delete pMergeItem;
					continue;
				}
			}

			bool containsStart = FALSE;
			bool containsEnd = FALSE;
		
			if( pMergeItem->GetPChannel() == pExistingItem->GetPChannel() )
			{
				containsStart = contains(pMergeItem->GetStartTime(), pMergeItem->GetEndTime(),
					pExistingItem->GetStartTime());
				containsEnd = contains(pMergeItem->GetStartTime(), pMergeItem->GetEndTime(),
					pExistingItem->GetEndTime());
			}
			
			if (containsStart && containsEnd) {
				// gotta break up this item into two
				CMuteItem* newItem = new CMuteItem;
				newItem->SetPChannel(pMergeItem->GetPChannel());
				newItem->SetPChannelMap(pMergeItem->GetPChannelMap());
				newItem->SetStart(pExistingItem->GetEndTime());
				newItem->SetEnd(pMergeItem->GetEndTime());
				pMergeItem->SetEnd(pExistingItem->GetStartTime());
				UpdateMuteMeasureBeat(newItem);
				if (newItem->IsUseless()) {
					delete newItem;
				}
				else {
					lstMergeItems.AddTail(newItem);
				}
			}
			else if (containsStart) {
				// move end time backwards
				pMergeItem->SetEnd(pExistingItem->GetStartTime());
			}
			else if (containsEnd) {
				// move start time forward
				pMergeItem->SetStart(pExistingItem->GetEndTime());

			}
			UpdateMuteMeasureBeat(pMergeItem);
			if (pMergeItem->IsUseless()) {
				lstMergeItems.RemoveAt(posMergeHere);
				delete pMergeItem;
			}
		}
	}

	// finally, merge them
	pos = lstMergeItems.GetHeadPosition();
	while (pos) {
		CMuteItem* pItem = lstMergeItems.GetNext(pos);
		MergeMuteItem(pItem);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::OnDataChanged
//
// Called when the mute data has been changed; sends the data to the
// dmusic track and notifies the timeline.
//
HRESULT STDMETHODCALLTYPE CMuteMgr::OnDataChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// send the data to the dmusic track
	SyncWithDM();

	ASSERT(m_pTimeline);
	if (m_pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// Let our hosting editor know about the change
	m_pTimeline->OnDataChanged((IDMUSProdStripMgr*)this);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::SyncWithDM
//
// Stream the mute data to the DirectMusic track.
//
HRESULT CMuteMgr::SyncWithDM()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr = S_OK;

	// 1. persist the strip as a Mute track to a stream
	IStream* pIMemStream = NULL;
	IPersistStream* pIPersistStream = NULL;
	hr = m_pDMProdFramework->AllocMemoryStream(FT_RUNTIME, GUID_CurrentVersion, &pIMemStream);
	if (SUCCEEDED (hr))
	{
		StreamSeek(pIMemStream, 0, STREAM_SEEK_SET);
		hr = Save(pIMemStream, FALSE);
		if (SUCCEEDED(hr))
		{
			// 2. load the stream into m_pIDMTrack
			hr = m_pIDMTrack->QueryInterface(IID_IPersistStream, (void**)&pIPersistStream);
			if (SUCCEEDED(hr))
			{
				StreamSeek(pIMemStream, 0, STREAM_SEEK_SET);
				hr = pIPersistStream->Load(pIMemStream);

				if( m_pDMProdSegmentNode )
				{
					IDirectMusicSegment *pSegment;
					if( SUCCEEDED( m_pDMProdSegmentNode->GetObject(CLSID_DirectMusicSegment, IID_IDirectMusicSegment, (void **)&pSegment ) ) )
					{
						m_pIDMTrack->Init( pSegment );
						pSegment->Release();
					}
				}

				pIPersistStream->Release();
				//TRACE("successfuly synced with dmusic\n");
			}
		}
		pIMemStream->Release();
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::EmptyMuteList
//
// Frees all mute items.
//
void CMuteMgr::EmptyMuteList(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Remove all items from the list
	if (!m_lstMutes.IsEmpty()) {
		CMuteItem *pMuteItem;
		while (!m_lstMutes.IsEmpty())
		{
			pMuteItem = m_lstMutes.RemoveHead();
			delete pMuteItem;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::InsertMuteItem
//
// Insert pMuteItem into the list in ascending order, determined by 
// the value of the item's m_mtTime member.
//
void CMuteMgr::InsertMuteItem(CMuteItem* pMuteItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT(pMuteItem != NULL);
	
	m_fDirty = true;
	
	// insert the mute item; sorted by pchannel.
	CMuteItem* pItem = NULL;
	POSITION pos = m_lstMutes.GetHeadPosition();
	POSITION prevPos = NULL;
	while (pos)
	{
		prevPos = pos;
		pItem = m_lstMutes.GetNext(pos);
		if (pItem->GetPChannel() > pMuteItem->GetPChannel()) {
			// insert between pItem and the previous item
			m_lstMutes.InsertBefore(prevPos, pMuteItem);
			return;
		}
	}
	
	// insert at end of list
	m_lstMutes.AddTail(pMuteItem);
	
	return;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::MergeMuteItem
//
// Inserts the given mute item into the list of items, merging it with
// other items that are adjacent and the same if possible.  Note that
// the pMuteItem pointer may be made invalid after this call since
// pMuteItem may be deleted. Also, pMuteItem should not already be
// in the list when calling this method.
//
void CMuteMgr::MergeMuteItem(CMuteItem* pMuteItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT(pMuteItem != NULL);

	CMuteItem* pLeftItem = NULL;	// mute item found to the left
	CMuteItem* pRightItem = NULL;	// mute item found to the right
	POSITION posAfter = NULL;		// position after location to insert
	POSITION posRight = NULL;		// position of right item, if any.

	// don't do anything with items for which starttime >= endtime
	if (pMuteItem->IsUseless()) {
		delete pMuteItem;
		return;
	}
	// traverse the list of mute items
	POSITION pos = m_lstMutes.GetHeadPosition();
	while (pos) {
		POSITION posPrev = pos;
		CMuteItem* pItem = m_lstMutes.GetNext(pos);
		if (pItem->GetPChannel() > pMuteItem->GetPChannel() && posAfter == NULL) {
			// this is pos after where we should insert the item if we do
			// (we sort by pchannel; time order doesn't matter)
			posAfter = posPrev;
		}
		if (pItem->GetPChannel() != pMuteItem->GetPChannel() 
			|| pItem->GetPChannelMap() != pMuteItem->GetPChannelMap())
			// the items don't match, so we arent interested.
			continue;
		
		if (pItem->GetEndTime() == pMuteItem->GetStartTime()) {
			ASSERT(pLeftItem == NULL);
			// this item is to the left
			pLeftItem = pItem;
		}
		else if (pItem->GetStartTime() == pMuteItem->GetEndTime()) {
			ASSERT(pRightItem == NULL);
			// this item is to the right
			pRightItem = pItem;
			posRight = posPrev; 
		}
		if (pLeftItem != NULL && pRightItem != NULL) {
			break;
		}
	}

	if (pLeftItem != NULL) {
		// extend left item forwards
		pLeftItem->SetEnd(pMuteItem->GetEndTime());
		delete pMuteItem;

		if (pRightItem != NULL) {
			pLeftItem->SetEnd(pRightItem->GetEndTime());
			ASSERT(posRight != NULL);
			m_lstMutes.RemoveAt(posRight);
			delete pRightItem;
		}
		UpdateMuteMeasureBeat(pLeftItem);
	}
	else if (pRightItem != NULL) {
		// extend right item backwards
		pRightItem->SetStart(pMuteItem->GetStartTime());
		UpdateMuteMeasureBeat(pRightItem);
		delete pMuteItem;
	}
	else {
		// no merges, simply add the new item
		if (posAfter != NULL) {
			m_lstMutes.InsertBefore(posAfter, pMuteItem);
		}
		else {
			m_lstMutes.AddTail(pMuteItem);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::UpdateMuteTime
//
// Updates the mute item's mtTime to be in sync with its measure and beat.
//
void CMuteMgr::UpdateMuteTime(CMuteItem* pMuteItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ASSERT(pMuteItem != NULL);

	pMuteItem->m_mtStart = GetMusicTime(pMuteItem->m_lStartMeasure, 
		pMuteItem->m_lStartBeat);
	pMuteItem->m_mtEnd = GetMusicTime(pMuteItem->m_lEndMeasure, 
		pMuteItem->m_lEndBeat);

	// update measure and beat so that they follow time sig correctly
	// if they don't already
	GetMeasureBeat(pMuteItem->m_mtStart, pMuteItem->m_lStartMeasure, 
		pMuteItem->m_lStartBeat);
	GetMeasureBeat(pMuteItem->m_mtEnd, pMuteItem->m_lEndMeasure, 
		pMuteItem->m_lEndBeat);
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::UpdateMuteMeasureBeat
//
// Update the mute item's measure and beat, based on the mute item's time
// and the time signature of the track group(s) we belong to.
//
void CMuteMgr::UpdateMuteMeasureBeat(CMuteItem* pMuteItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ASSERT( pMuteItem != NULL );
	ASSERT( m_pTimeline != NULL );
	ASSERT( pMuteItem->m_mtStart >= 0 );
	
	MUSIC_TIME mtOrigEndTime = pMuteItem->m_mtEnd;

	GetMeasureBeat( pMuteItem->m_mtStart, pMuteItem->m_lStartMeasure, pMuteItem->m_lStartBeat );
	GetMeasureBeat( pMuteItem->m_mtEnd, pMuteItem->m_lEndMeasure, pMuteItem->m_lEndBeat );
	
	// update times (they could be in the middle of beats; we dont want this)
	UpdateMuteTime( pMuteItem );

	// Round up
	if( mtOrigEndTime > pMuteItem->m_mtEnd )
	{
		MUSIC_TIME mtTime;

		m_pTimeline->MeasureBeatToClocks( m_dwGroupBits, 0, pMuteItem->m_lEndMeasure, ++pMuteItem->m_lEndBeat, &mtTime );
		m_pTimeline->ClocksToMeasureBeat( m_dwGroupBits, 0, mtTime, &pMuteItem->m_lEndMeasure, &pMuteItem->m_lEndBeat );
		UpdateMuteTime( pMuteItem );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::GetMusicTime
//
// Get's the music time for the given measure and beat.
//
MUSIC_TIME	CMuteMgr::GetMusicTime(long lMeasure, long lBeat)
{
	ASSERT(m_pTimeline != NULL);

	MUSIC_TIME mtTime;
	VERIFY(SUCCEEDED(m_pTimeline->MeasureBeatToClocks(
		m_dwGroupBits, 0, lMeasure, lBeat, &mtTime)));

	return mtTime;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::GetMeasureBeat
//
// Return measure, beat corresponding to the given time.
//
void CMuteMgr::GetMeasureBeat(MUSIC_TIME mtTime, long &lMeasure, long &lBeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VERIFY(SUCCEEDED(m_pTimeline->ClocksToMeasureBeat(m_dwGroupBits, 
		0, mtTime, &lMeasure, &lBeat)));
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::NormalizeMeasureBeat
//
// Returns a measure, beat that follows the time signature correctly.
//
void CMuteMgr::NormalizeMeasureBeat(long &lMeasure, long &lBeat) 
{
	/*
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	MUSIC_TIME mtTime = GetMusicTime(lMeasure, lBeat);
	GetMeasureBeat(mtTime, lMeasure, lBeat);
	*/
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::GetNextMuteMeasureBeat
//
// Returns the start measurebeat of the next mute item following the 
// given measurebeat.  If none, returns the last measurebeat.
// 
void CMuteMgr::GetNextMuteMeasureBeat(long lMeasure, long lBeat, DWORD dwPChannel, 
									  long &lNextMeasure, long &lNextBeat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// traverse the list of mute items
	CMuteItem* pFoundItem = NULL;
	MUSIC_TIME mtStart = GetMusicTime(lMeasure, lBeat);
	POSITION pos = m_lstMutes.GetHeadPosition();
	while (pos) {
		CMuteItem* pItem = m_lstMutes.GetNext(pos);
		if (pItem->GetPChannel() == dwPChannel && pItem->GetStartTime() > mtStart) {
			if (pFoundItem == NULL || pFoundItem->GetStartTime() > pItem->GetStartTime()) {
				pFoundItem = pItem;
			}
		}
	}
	
	if (pFoundItem != NULL) {
		pFoundItem->GetStartMeasureBeat(lNextMeasure, lNextBeat);
	}
	else {
		// figure out the max ending time
		VARIANT var;
		VERIFY(SUCCEEDED(m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var)));
		MUSIC_TIME mtMax = V_I4(&var);
		GetMeasureBeat(mtMax, lNextMeasure, lNextBeat);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::RecomputeMuteMeasureBeats
//
// Recomputes the measure and beat for all mute items, based on their times.
// Items that no longer take up at least a whole beat are deleted. 
void CMuteMgr::RecomputeMuteMeasureBeats()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CMuteItem* pMuteItem;
	POSITION pos = m_lstMutes.GetHeadPosition();
	POSITION prev;
	while(pos)
	{
		prev = pos;
		pMuteItem = m_lstMutes.GetNext(pos);
		ASSERT(pMuteItem);
		if (pMuteItem)
		{
			UpdateMuteMeasureBeat(pMuteItem);
			if (pMuteItem->IsUseless()) {
				// delete items that no longer take up at least a whole beat
				m_lstMutes.RemoveAt(prev);
				delete pMuteItem;
			}			
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::RecomputeMuteTimes
//
// Recomputes time for all mute items, based on their measure and beat.
//
void CMuteMgr::RecomputeMuteTimes()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CMuteItem* pMuteItem;
	long lOrigStartMeasure;
	POSITION posItem;
	POSITION pos = m_lstMutes.GetHeadPosition();
	while( pos )
	{
		posItem = pos;
		pMuteItem = m_lstMutes.GetNext(pos);

		lOrigStartMeasure = pMuteItem->m_lStartMeasure;

		if( m_fTimeSigChange )
		{
			DMUS_TIMESIGNATURE TimeSig;
			MUSIC_TIME mt;

			mt = GetMusicTime( pMuteItem->m_lStartMeasure, 0 );
			if( SUCCEEDED ( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, mt, NULL, &TimeSig ) ) )
			{
				if( pMuteItem->m_lStartBeat >= TimeSig.bBeatsPerMeasure )
				{
					pMuteItem->m_lStartMeasure++;
					pMuteItem->m_lStartBeat = 0; 
				}
			}
			mt = GetMusicTime( pMuteItem->m_lEndMeasure, 0 );
			if( SUCCEEDED ( m_pTimeline->GetParam( GUID_TimeSignature, m_dwGroupBits, 0, mt, NULL, &TimeSig ) ) )
			{
				pMuteItem->m_lEndBeat = min( pMuteItem->m_lEndBeat, TimeSig.bBeatsPerMeasure ); 
			}
		}

		UpdateMuteTime( pMuteItem );

		if( pMuteItem->m_mtStart >= pMuteItem->m_mtEnd )
		{
			m_lstMutes.RemoveAt( posItem );
			delete pMuteItem;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::FindMuteItem
//
// Finds a mute item on the given pchannel such that the given measure and
// beat is included within or next to the start and/or end time of the mute item.
// If there is no such mute item, pMuteItem is set to NULL.
// If an item is found, the position of the given measure and beat 
// with respect to the mute item(s) is returned.
// If mpMiddle is returned, pItem will be the mute item to the left of the
// measurebeat and pOther will be the mute item to the right.
//
CMuteMgr::MutePos CMuteMgr::FindMuteItem(long lMeasure, long lBeat, DWORD dwPChannel, 
										 CMuteItem*& pItem, CMuteItem*& pOther)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	MUSIC_TIME mtStart = GetMusicTime(lMeasure, lBeat);
	MUSIC_TIME mtEnd = GetMusicTime(lMeasure, lBeat+1);

	CMuteItem* pFoundItem = NULL;
	pOther = NULL;
	MutePos foundPos;

	// traverse the list of mute items
	POSITION pos = m_lstMutes.GetHeadPosition();
	while (pos) {
		pItem = m_lstMutes.GetNext(pos);
		if (pItem->GetPChannel() == dwPChannel) {
			
			if (pItem->GetStartTime() <= mtStart && pItem->GetEndTime() >= mtEnd) 
			{
				return mpMiddle;
			}
			else if (pItem->GetStartTime() == mtEnd)
			{
				if (pFoundItem == NULL) {
					pFoundItem = pItem;
					foundPos = mpLeft;
				}
				else {
					ASSERT(pOther == NULL);
					pOther = pItem;
					foundPos = mpBetween;
				}
				
				// don't return yet, because we may find another
				// item we are in the middle of.
			}
			else if (pItem->GetEndTime() == mtStart)
			{
				if (pFoundItem == NULL)	{
					foundPos = mpRight;
				}
				else {
					ASSERT(pOther == NULL);
					pOther = pFoundItem;
					foundPos = mpBetween;
				}
				pFoundItem = pItem;
			}
		}
	}
	
	pItem = pFoundItem;
	return foundPos;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::TurnOnMute
//
// Turn mute on for the given measure, beat, and pchannel, if it isn't
// on already.  If there already is a remap item at the measurebeat, nothing
// happens.
//
void CMuteMgr::TurnOnMute(long lMeasure, long lBeat, DWORD dwPChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	NormalizeMeasureBeat(lMeasure, lBeat);
	CMuteItem *foundItem, *otherItem;
	MutePos pos = FindMuteItem(lMeasure, lBeat, dwPChannel, foundItem, otherItem);
	
	if (foundItem == NULL || pos != mpMiddle) {
		CMuteItem* newItem = new CMuteItem();
		if (newItem == NULL) return;
		newItem->SetStart(lMeasure, lBeat);
		newItem->SetEnd(lMeasure, lBeat+1);
		newItem->SetPChannel(dwPChannel);
		newItem->SetAsMute();
		UpdateMuteTime(newItem);
		MergeMuteItem(newItem);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::TurnOffMute
//
// Turn mute off for the given measure, beat, and pchannel, if it isn't
// off already.  If there already is a remap item at the measurebeat, nothing
// happens.
//
void CMuteMgr::TurnOffMute(long lMeasure, long lBeat, DWORD dwPChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	NormalizeMeasureBeat(lMeasure, lBeat);
	
	CMuteItem *foundItem, *otherItem;
	MutePos pos = FindMuteItem(lMeasure, lBeat, dwPChannel, foundItem, otherItem);

	MUSIC_TIME mtStart = GetMusicTime(lMeasure, lBeat);
	MUSIC_TIME mtEnd = GetMusicTime(lMeasure, lBeat+1);

	if (foundItem != NULL && foundItem->IsMute() && pos == mpMiddle) {
		
		if (foundItem->GetStartTime() == mtStart) {
			// move start time forward
			foundItem->SetStart(lMeasure, lBeat+1);
			m_fDirty = true;
		}
		else if (foundItem->GetEndTime() == mtEnd) {
			// move end time backwards
			foundItem->SetEnd(lMeasure, lBeat);
			m_fDirty = true;
		}
		else {
			// the position is somewhere in between, so we need to
			// "break" an existing mute item in half.
			CMuteItem* newItem = new CMuteItem();
			if (newItem == NULL) return;
			newItem->SetStart(lMeasure, lBeat+1);
			newItem->SetEnd(foundItem->m_lEndMeasure, foundItem->m_lEndBeat);
			newItem->SetPChannel(dwPChannel);
			newItem->SetAsMute();
			// adjust the existing item's end
			foundItem->SetEnd(lMeasure, lBeat);
			UpdateMuteTime(newItem);
			InsertMuteItem(newItem);
		}
		
		UpdateMuteTime(foundItem);
		
		// delete existing item if start == end time
		if (foundItem->IsUseless()) {
			m_lstMutes.RemoveAt(m_lstMutes.Find(foundItem));
			delete foundItem;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::RemapPChannel
//
// Remap the given pchannel starting a the given measure and beat.
// If there is an existing mute item at the location, nothing happens.
// If there is an existing remap, the remap changes to the new
// channel from the given measurebeat onwards.
//
void CMuteMgr::RemapPChannel(long lMeasure, long lBeat, DWORD dwPChannel, 
							 DWORD dwPChannelMap)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	NormalizeMeasureBeat(lMeasure, lBeat);
	CMuteItem *foundItem, *otherItem;
	MutePos pos = FindMuteItem(lMeasure, lBeat, dwPChannel, foundItem, otherItem);
	
	CMuteItem* newItem = new CMuteItem();
	if (newItem == NULL) return;
	newItem->SetStart(lMeasure, lBeat);
	newItem->SetPChannel(dwPChannel);
	newItem->SetPChannelMap(dwPChannelMap);

	long lEndMeasure, lEndBeat;
	if (foundItem != NULL && pos == mpMiddle) {
		if (foundItem->GetPChannelMap() != dwPChannelMap && foundItem->IsRemap()) {
			// the existing remap is to a different channel,
			// so change it from this measure,beat onwards.
			foundItem->GetEndMeasureBeat(lEndMeasure, lEndBeat);
			newItem->SetEnd(lEndMeasure, lEndBeat);
			foundItem->SetEnd(lMeasure, lBeat);
			
			UpdateMuteTime(foundItem);
			// delete the existing item if its no longer necessary
			if (foundItem->IsUseless()) {
				m_lstMutes.RemoveAt(m_lstMutes.Find(foundItem));
				delete foundItem;
			}
			
			UpdateMuteTime(newItem);
			MergeMuteItem(newItem);
		}
		else {
			// they're equal or its a mute
			delete newItem;
		}
	}
	else {
		GetNextMuteMeasureBeat(lMeasure, lBeat, dwPChannel, lEndMeasure, lEndBeat);
		newItem->SetEnd(lEndMeasure, lEndBeat);
		UpdateMuteTime(newItem);
		MergeMuteItem(newItem);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::ClearRemap
//
// Clear the remap on the given pchannel starting at the given measurebeat. 
// If there is no remap at the measurebeat, nothing happens.
//
void CMuteMgr::ClearRemap(long lMeasure, long lBeat, DWORD dwPChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	NormalizeMeasureBeat(lMeasure, lBeat);
	
	CMuteItem *foundItem, *otherItem;
	MutePos pos = FindMuteItem(lMeasure, lBeat, dwPChannel, foundItem, otherItem);

	if (foundItem != NULL && foundItem->IsRemap() && pos == mpMiddle) {
		foundItem->SetEnd(lMeasure, lBeat);
		UpdateMuteTime(foundItem);
		if (foundItem->IsUseless()) {
			// remove it.
			m_lstMutes.RemoveAt(m_lstMutes.Find(foundItem));
			delete foundItem;
		}
		m_fDirty = true;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::MuteEntirePChannel
//
// Mute the entire pchannel.
//
void CMuteMgr::MuteEntirePChannel(DWORD dwPChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// remove all items on the PChannel
	POSITION pos = m_lstMutes.GetHeadPosition();
	while (pos) {
		CMuteItem* pItem = m_lstMutes.GetNext(pos);
		if (pItem->GetPChannel() == dwPChannel) {
			m_lstMutes.RemoveAt(m_lstMutes.Find(pItem));
			delete pItem;
		}
	}
	
	VARIANT var;
	m_pTimeline->GetTimelineProperty(TP_CLOCKLENGTH, &var);
	MUSIC_TIME mtEnd = V_I4(&var);

	long lMeasure, lBeat;

	VERIFY(SUCCEEDED(m_pTimeline->ClocksToMeasureBeat(m_dwGroupBits, 
		0, mtEnd, &lMeasure, &lBeat)));
	
	// insert a new mute item that spans the entire track
	CMuteItem* newItem = new CMuteItem();
	if (newItem == NULL) return;
	newItem->SetStart(0, 0);
	newItem->SetEnd(lMeasure, lBeat);
	newItem->SetPChannel(dwPChannel);
	newItem->SetAsMute();
	UpdateMuteTime(newItem);
	InsertMuteItem(newItem);
}


/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::UnmuteEntirePChannel
//
// Unmute the entire pchannel (remove all mute items that are mutes 
// on the pchannel). Remaps are left alone.
//
void CMuteMgr::UnmuteEntirePChannel(DWORD dwPChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// delete all mutes on the pchannel
	POSITION pos = m_lstMutes.GetHeadPosition();
	while (pos) {
		POSITION posPrev = pos;
		CMuteItem* pItem = m_lstMutes.GetNext(pos);
		if (pItem->GetPChannel() == dwPChannel && pItem->IsMute()) {
			m_lstMutes.RemoveAt(posPrev);
			delete pItem;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::RemoveChannelData
//
// Remove all mute items that are on the given channel.
//
void CMuteMgr::RemoveChannelData(DWORD dwPChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	POSITION pos = m_lstMutes.GetHeadPosition();
	while (pos) {
		POSITION posPrev = pos;
		CMuteItem* pItem = m_lstMutes.GetNext(pos);
		if (pItem->GetPChannel() == dwPChannel) {
			m_lstMutes.RemoveAt(posPrev);
			delete pItem;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::IsPositionMuted
//
// Returns true iff the position at the given measure, beat is muted.
//
bool CMuteMgr::IsPositionMuted(long lMeasure, long lBeat, DWORD dwPChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CMuteItem *foundItem, *otherItem;
	MutePos pos = FindMuteItem(lMeasure, lBeat, dwPChannel, foundItem, otherItem);
	
	// return true if there is a mute item that contains this position
	return (foundItem != NULL && pos == mpMiddle);
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::GetItemAtPosition
//
// Returns the mute item that contains the given measurebeat.  If none,
// returns NULL.
//
CMuteItem* CMuteMgr::GetItemAtPosition(long lMeasure, long lBeat, DWORD dwPChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CMuteItem *foundItem, *otherItem;
	MutePos pos = FindMuteItem(lMeasure, lBeat, dwPChannel, foundItem, otherItem);
	
	// return true if there is a mute item that contains this position
	return (foundItem != NULL && pos == mpMiddle) ? foundItem : NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CMuteMgr::ChannelContainsData
//
// Returns true iff the given channel contains any mute data.
//
bool CMuteMgr::ChannelContainsData(DWORD dwPChannel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	POSITION pos = m_lstMutes.GetHeadPosition();
	while (pos) {
		CMuteItem* pItem = m_lstMutes.GetNext(pos);
		if (pItem->GetPChannel() == dwPChannel) {
			return true;
		}
	}
	return false;
}