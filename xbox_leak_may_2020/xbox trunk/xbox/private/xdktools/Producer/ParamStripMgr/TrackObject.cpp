// TrackObject.cpp : implementation file
//

#include "stdafx.h"
#include "RiffStrm.h"
#include "TrackMgr.h"
#include "ParamStrip.h"
//#include "TrackObject.h"


// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTrackObject constructor/destructor 

CTrackObject::CTrackObject( CTrackMgr* pTrackMgr )
{
	ASSERT( pTrackMgr != NULL );
	m_pTrackMgr = pTrackMgr;

	m_dwPChannel = 0;	
	m_dwBuffer = 0;
	m_dwStage = 0;		
	memset( &m_guidObject, 0, sizeof(GUID) );
	m_dwObjectIndex = 0;
	m_dwObjectFlagsDM = 0;

	m_bDisabled = FALSE;
	m_ParamStripView = SV_MINIMIZED;

	m_pMinimizedStrip = new CParamStrip(pTrackMgr, this);
	ASSERT(m_pMinimizedStrip);
}

CTrackObject::~CTrackObject()
{
	// Release all the items in m_lstParamStrips
	while( !m_lstParamStrips.IsEmpty() )
	{
		CParamStrip* pParamStrip = m_lstParamStrips.RemoveHead();

		long cRef = pParamStrip->Release();
		ASSERT( cRef == 0 );
	}

	if(m_pMinimizedStrip)
	{
		m_pMinimizedStrip->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackObject::LoadObject

HRESULT CTrackObject::LoadObject(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckParent)
{
	MMCKINFO		ck;
	MMCKINFO		ckList;
	DWORD			dwByteCount = 0;
	DWORD			dwSize = 0;
	HRESULT			hr = E_FAIL;

	ASSERT(pIRiffStream);
	if(pIRiffStream == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pckParent);
	if(pckParent == NULL )
	{
		return E_POINTER;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_FAIL;
	}

	// Load the items
	while(pIRiffStream->Descend(&ck, pckParent, 0) == 0)
	{
		switch(ck.ckid)
		{
            case DMUS_FOURCC_OBJECT_UI_CHUNK:
            {
                ioObjectUI iObjectUI;
                hr = pIStream->Read(&iObjectUI, sizeof(ioObjectUI), &dwByteCount);
                if(FAILED(hr) || dwByteCount != sizeof(ioObjectUI))
                {
                    goto ON_ERROR;
                }

                m_guidInstance = iObjectUI.m_guidInstance;

                break;
            }

			case DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_CHUNK:
			{
				DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER iObjectHeader;

				// Read in the item's header structure
				dwSize = min( sizeof( DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER ), ck.cksize );
				hr = pIStream->Read( &iObjectHeader, dwSize, &dwByteCount );

				// Handle any I/O error by returning a failure code
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwObjectFlagsDM = iObjectHeader.dwFlags;
				m_guidTimeFormat = iObjectHeader.guidTimeFormat;
				m_dwPChannel = iObjectHeader.dwPChannel;
				m_dwBuffer = iObjectHeader.dwBuffer;
				m_dwStage = iObjectHeader.dwStage;
				m_guidObject = iObjectHeader.guidObject;
				m_dwObjectIndex = iObjectHeader.dwIndex;
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_PARAMCONTROLTRACK_PARAM_LIST:
					{
						CParamStrip* pNewStrip = new CParamStrip( m_pTrackMgr, this);
						if( pNewStrip == NULL )
						{
							hr = E_OUTOFMEMORY;
							goto ON_ERROR;
						}
						hr = pNewStrip->LoadStrip( pIRiffStream, &ckList );
						if( FAILED ( hr ) )
						{
							delete pNewStrip;
							goto ON_ERROR;
						}
						InsertByAscendingParameter( pNewStrip );
						m_ParamStripView = pNewStrip->m_StripUI.m_nStripView == SV_MINIMIZED ? SV_MINIMIZED : SV_NORMAL;
						break;
					}
				}
				break;
		}

		// Ascend out of the chunk
		pIRiffStream->Ascend( &ck, 0 );
	}

	RefreshObjectProperties();

ON_ERROR:
	RELEASE( pIStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackObject::SaveObject

HRESULT CTrackObject::SaveObject( IDMUSProdRIFFStream* pIRiffStream )
{
	MMCKINFO ckObject;
	MMCKINFO ck;
	HRESULT hr = E_FAIL;

	// No need to save if this is an empty object....
	if(IsEmptyTrackObject())
	{
		return S_FALSE;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream == NULL)
	{
		return E_OUTOFMEMORY;
	}
    
	// Structures for determining the stream type
	DMUSProdStreamInfo	StreamInfo;
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		ftFileType = StreamInfo.ftFileType;
		guidDataFormat = StreamInfo.guidDataFormat;
		pPersistInfo->Release();
	}


	// Create the DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_LIST list chunk
	ckObject.fccType = DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_LIST;
	if( pIRiffStream->CreateChunk( &ckObject, MMIO_CREATELIST ) != 0 )
	{
		// If unable to create the chunk, return E_FAIL
		hr = E_FAIL;
		goto ON_ERROR;
	}

    // Create the UI chunk
    if(ftFileType == FT_DESIGN)
    {
        ck.ckid = DMUS_FOURCC_OBJECT_UI_CHUNK;
        if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

        ioObjectUI oObjectUI;
        oObjectUI.cbSize = sizeof(ioObjectUI);
        oObjectUI.m_guidInstance = m_guidInstance;
        
        DWORD dwBytesWritten = 0;
        hr = pIStream->Write(&oObjectUI, sizeof(ioObjectUI), &dwBytesWritten);
        if(FAILED(hr) || dwBytesWritten != sizeof(ioObjectUI))
        {
            goto ON_ERROR;
        }

        if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
     }

	// Create the DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_CHUNK chunk
	{
		ck.ckid = DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			// If unable to create the chunk, return E_FAIL
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Clear out the structure (clears out the padding bytes as well).
		DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER oObjectHeader;
		ZeroMemory( &oObjectHeader, sizeof(DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER) );

		// Fill in the members of the DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER structure
		oObjectHeader.dwFlags = m_dwObjectFlagsDM;
		// TODO - set time guid
		oObjectHeader.guidTimeFormat = m_guidTimeFormat;
		oObjectHeader.dwPChannel = m_dwPChannel;
		oObjectHeader.dwStage = m_dwStage;
		oObjectHeader.dwBuffer = m_dwBuffer;
		oObjectHeader.guidObject = m_guidObject;
		oObjectHeader.dwIndex = m_dwObjectIndex;

		// Write the structure out to the stream
		DWORD dwBytesWritten;
		hr = pIStream->Write( &oObjectHeader, sizeof(DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER), &dwBytesWritten );
		if( FAILED( hr ) || dwBytesWritten != sizeof(DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER) )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Ascend out of the DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_CHUNK chunk
		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			// Handle I/O errors by return an error code
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Save the parameter control strips
	if( !m_lstParamStrips.IsEmpty() )
	{

		// Iterate through the item list
		POSITION pos = m_lstParamStrips.GetHeadPosition();
		while( pos )
		{
			// Get a pointer to each item
			CParamStrip* pParamStrip = m_lstParamStrips.GetNext( pos );

			// Save each strip
			hr = pParamStrip->SaveStrip( pIRiffStream );
			if( FAILED ( hr ) )
			{
				goto ON_ERROR;
			}
		}
	}

	// Ascend out of the DMUS_FOURCC_PARAMCONTROLTRACK_OBJECT_LIST list chunk
	if( pIRiffStream->Ascend(&ckObject, 0) != 0 )
	{
		// Handle I/O errors by return an error code
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIStream );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackObject::InsertByAscendingParameter

void CTrackObject::InsertByAscendingParameter( CParamStrip* pParamStripToInsert )
{
	// Ensure the pParamStripToInsert pointer is valid
	if ( pParamStripToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	CParamStrip* pParamStrip;
	POSITION posCurrent, posNext = m_lstParamStrips.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pParamStrip = m_lstParamStrips.GetNext( posNext );

		CString sName1 = pParamStripToInsert->GetName();
		CString sName2 = pParamStrip->GetName();

		if( sName1.CompareNoCase(sName2) <= 0 )
		{
			// insert before posCurrent (which is the position of pParamStrip)
			m_lstParamStrips.InsertBefore( posCurrent, pParamStripToInsert );
			return;
		}
	}

	// pParamStripToInsert has higher parameter than all items in the list
	// add it at the end of the list
	m_lstParamStrips.AddTail( pParamStripToInsert );
	return;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackObject::AllStrips_OnUpdate

void CTrackObject::AllStrips_OnUpdate( REFGUID rguidType, DWORD dwGroupBits, void* pData )
{
	POSITION pos = m_lstParamStrips.GetHeadPosition();
	while( pos )
	{
		CParamStrip* pParamStrip = m_lstParamStrips.GetNext( pos );

		pParamStrip->OnUpdate( rguidType, dwGroupBits, pData );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackObject::AllStrips_Invalidate

void CTrackObject::AllStrips_Invalidate( void )
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	if(pTimeline == NULL )
	{
		return;
	}

	POSITION pos = m_lstParamStrips.GetHeadPosition();
	while( pos )
	{
		CParamStrip* pParamStrip = m_lstParamStrips.GetNext( pos );

		ASSERT(pParamStrip);
		if(pParamStrip == NULL)
		{
			return;
		}

		pParamStrip->InvalidateFBar();

		IDMUSProdPropPageManager* pPropPageMgr = pParamStrip->GetPropPageMgr();
		// Refresh strip properties
		if( pPropPageMgr )
		{
			pPropPageMgr->RefreshData();
		}

		// Redraw strip
		pTimeline->StripInvalidateRect( pParamStrip, NULL, TRUE );
	}


	// Invalidate the minimized strip too...
	pTimeline->StripInvalidateRect( m_pMinimizedStrip, NULL, TRUE );
}


/////////////////////////////////////////////////////////////////////////////
// CTrackObject::AllStrips_RecomputeMeasureBeats

void CTrackObject::AllStrips_RecomputeMeasureBeats()
{
	POSITION pos = m_lstParamStrips.GetHeadPosition();
	while( pos )
	{
		CParamStrip* pParamStrip = m_lstParamStrips.GetNext( pos );

		pParamStrip->RecomputeMeasureBeats();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTrackObject::AllStrips_RecomputeTimes

bool CTrackObject::AllStrips_RecomputeTimes()
{
	bool fChanged = FALSE;

	POSITION pos = m_lstParamStrips.GetHeadPosition();
	while( pos )
	{
		CParamStrip* pParamStrip = m_lstParamStrips.GetNext( pos );

		fChanged |= pParamStrip->RecomputeTimes();
	}

	return fChanged;
}


void CTrackObject::AllStrips_AddToTimeline()
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}

	int nPosition = -1;
	
	POSITION positionStrip = m_lstParamStrips.GetHeadPosition();
	if(positionStrip == NULL)
	{
		AddStripToTimeline(m_pMinimizedStrip, nPosition);
		return;
	}

	while(positionStrip)
	{
		CParamStrip* pParamStrip = (CParamStrip*)m_lstParamStrips.GetNext(positionStrip);
		ASSERT(pParamStrip);
		if(pParamStrip)
		{
			if(pParamStrip->m_StripUI.m_nStripView == SV_MINIMIZED)
			{
				AddStripToTimeline(m_pMinimizedStrip, nPosition);
				return;
			}

			AddStripToTimeline( pParamStrip, nPosition );

			if( nPosition == -1 )
			{
				VARIANT var;
				if( SUCCEEDED( pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)pParamStrip, STP_POSITION, &var) ) )
				{
					nPosition = V_I4(&var);
					ASSERT( nPosition > 0 );
				}
			}

			nPosition++;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTrackObject::AllStrips_SetNbrGridsPerSecond

BOOL CTrackObject::AllStrips_SetNbrGridsPerSecond( short nNbrGridsPerSecond )
{
	BOOL fChanged = FALSE;

	POSITION pos = m_lstParamStrips.GetHeadPosition();
	while( pos )
	{
		CParamStrip* pParamStrip = m_lstParamStrips.GetNext( pos );

		if( pParamStrip->m_StripUI.m_nSnapToRefTime_GridsPerSecond != nNbrGridsPerSecond )
		{
			pParamStrip->m_StripUI.m_nSnapToRefTime_GridsPerSecond = nNbrGridsPerSecond;
			fChanged = TRUE;
		}
	}

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackObject::AllStrips_SwitchTimeBase

void CTrackObject::AllStrips_SwitchTimeBase( void )
{
	POSITION pos = m_lstParamStrips.GetHeadPosition();
	while( pos )
	{
		CParamStrip* pParamStrip = m_lstParamStrips.GetNext( pos );
		pParamStrip->SwitchTimeBase();
	}
}


BOOL CTrackObject::IsEmptyTrackObject()
{
	// Is the strip list empty? 
	if(m_lstParamStrips.IsEmpty())
	{
		return TRUE;
	}

	// Is this the first dummy object we insert
	GUID guid;
	memset(&guid, 0, sizeof(GUID));

	if(m_dwPChannel == 0 &&	m_dwStage == 0 && m_dwBuffer == 0 &&
		::IsEqualGUID(m_guidObject, guid) && 
		m_dwObjectIndex == 0 && m_dwObjectFlagsDM == 0)
	{
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CTrackObject::OnChangeCurveStripView

void CTrackObject::OnChangeParamStripView( STRIPVIEW svNewStripView )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT( m_pTrackMgr != NULL );
	if( m_ParamStripView == svNewStripView )
	{
		return;
	}

	CParamStrip* pParamStrip = NULL;

	// Don't allow SV_NORMAL when there are no ParamStrips
	if( svNewStripView == SV_NORMAL && IsEmptyTrackObject())
	{
		m_ParamStripView = SV_MINIMIZED;

		VARIANT var;
		var.vt = VT_I4;
		V_I4(&var) = m_ParamStripView;

		if(m_lstParamStrips.IsEmpty())
		{
			m_pTrackMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)m_pMinimizedStrip, STP_STRIPVIEW, var );
		}

		POSITION pos = m_lstParamStrips.GetHeadPosition();
		while( pos )
		{
			pParamStrip = m_lstParamStrips.GetNext( pos );
			m_pTrackMgr->m_pTimeline->StripSetTimelineProperty( (IDMUSProdStrip *)pParamStrip, STP_STRIPVIEW, var );
		}

		return;
	}

	if(m_ParamStripView == SV_MINIMIZED)
	{
		RemoveStripFromTimeline(m_pMinimizedStrip);
		m_pTrackMgr->NotifyDataChange(IDS_UNDO_STRIP_MAXIMIZED);
	}
	else
	{
		m_pTrackMgr->NotifyDataChange(IDS_UNDO_STRIP_MINIMIZED);
	}

	// Change Param StripView field
	m_ParamStripView = svNewStripView;

	// Remove all existing Param Strips
	AllStrips_RemoveFromTimeline();

	// Go through all the strips and set their view state
	POSITION pos = m_lstParamStrips.GetHeadPosition();
	while(pos)
	{
		CParamStrip* pParamStrip = m_lstParamStrips.GetNext(pos);
		ASSERT(pParamStrip);
		if(pParamStrip)
		{
			pParamStrip->m_StripUI.m_nStripView = svNewStripView;

		}
	}

	// Add new Param Strips
	if(m_ParamStripView == SV_MINIMIZED)
	{
		AddStripToTimeline(m_pMinimizedStrip, -1);
	}
	else
	{
		AllStrips_AddToTimeline();
	}

	// Let the object know about the changes
	m_pTrackMgr->SetDirtyFlag(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CTrackObject::AddStripToTimeline

HRESULT CTrackObject::AddStripToTimeline(CParamStrip* pParamStrip, int nPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(pParamStrip);
	if( pParamStrip == NULL)
	{
		return E_POINTER;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_FAIL;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	HRESULT hr = E_FAIL;
	// Place strip in timeline
	if( nPosition <= 0 )
	{
		CLSID clsid = m_pTrackMgr->GetCLSID();
		DWORD dwGroupBits = m_pTrackMgr->GetGroupBits();
		hr = pTimeline->InsertStripAtDefaultPos( (IDMUSProdStrip *)pParamStrip, clsid, dwGroupBits, 0 );
	}
	else
	{
		hr = pTimeline->InsertStripAtPos( (IDMUSProdStrip *)pParamStrip, nPosition );
	}

	// Set strip state information
	pParamStrip->StripUIToTimeline();

	return hr;
}




/////////////////////////////////////////////////////////////////////////////
// CTrackObject::RemoveStripFromTimeline
HRESULT CTrackObject::RemoveStripFromTimeline( CParamStrip* pParamStrip )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	ASSERT(pParamStrip);
	if( pParamStrip == NULL )
	{
		return E_POINTER;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_FAIL;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	// Don't assert since this gets called in OnUpdate after the 
	// track's deleted in which case m_pTimeline is always NULL
	// ASSERT(pTimeline); 
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	// Make sure we are storing the latest UI state information
	pParamStrip->TimelineToStripUI();

	// Make sure our strip's property page isn't displayed
	m_pTrackMgr->RemoveFromPropSheet(pParamStrip);
	
	pTimeline->RemovePropertyPageObject((IDMUSProdPropPageObject*)pParamStrip);

	IDMUSProdPropPageManager* pPropPageMgr = pParamStrip->GetPropPageMgr();
	RELEASE(pPropPageMgr);

	pTimeline->RemoveStrip((IDMUSProdStrip *)pParamStrip);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CTrackMgr::AllStrips_RemoveFromTimeline

void CTrackObject::AllStrips_RemoveFromTimeline( void )
{
	POSITION pos = m_lstParamStrips.GetHeadPosition();
	while( pos )
	{
		CParamStrip* pParamStrip = m_lstParamStrips.GetNext(pos);
		ASSERT(pParamStrip);
		RemoveStripFromTimeline( pParamStrip );
	}

	if(m_pMinimizedStrip)
	{
		RemoveStripFromTimeline(m_pMinimizedStrip);
	}
}


HRESULT CTrackObject::AddStrip(StripInfo* pStripInfo)
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}

	ASSERT(pStripInfo);
	if(pStripInfo == NULL)
	{
		return E_POINTER;
	}
	
	m_sStageName = pStripInfo->m_sStageName;
	m_sObjectName = pStripInfo->m_sObjectName;
	m_dwPChannel = pStripInfo->m_dwPChannel;
	m_dwBuffer = pStripInfo->m_dwBuffer;
	m_sPChannelText = pStripInfo->m_sPChannelText; 
	m_dwStage = pStripInfo->m_dwStage;
	m_guidObject = pStripInfo->m_guidObject;
    m_guidInstance = pStripInfo->m_guidInstance;
	m_guidTimeFormat = pStripInfo->m_guidTimeFormat;
	m_dwObjectIndex = pStripInfo->m_dwObjectIndex;

	CParamStrip* pStrip = new CParamStrip(m_pTrackMgr, this);
	ASSERT(pStrip);
	if(pStrip == NULL)
	{
		return E_OUTOFMEMORY;
	}
	
	if(FAILED(pStrip->SetProperties(pStripInfo)))
	{
		delete pStrip;
		return E_FAIL;
	}

	pStrip->m_StripUI.m_nStripView = m_ParamStripView;

	// Get position of first wave strip in the timeline
	/*int nPosition = -1;
	if( m_lstParamStrips.IsEmpty() == FALSE  && m_ParamStripView != SV_MINIMIZED)
	{
		CParamStrip* pFirstStrip = m_lstParamStrips.GetHead();

		VARIANT var;
		if( SUCCEEDED( m_pTrackMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)pFirstStrip, STP_POSITION, &var) ) )
		{
			nPosition = V_I4(&var);
		}
	}
	else
	{
		VARIANT var;
		if( SUCCEEDED( m_pTrackMgr->m_pTimeline->StripGetTimelineProperty( (IDMUSProdStrip *)m_pMinimizedStrip, STP_POSITION, &var) ) )
		{
			nPosition = V_I4(&var);
		}

		 if(m_lstParamStrips.IsEmpty())
		 {
			 RemoveStripFromTimeline(m_pMinimizedStrip);
		 }
	}
	
	if(m_ParamStripView != SV_MINIMIZED || m_lstParamStrips.IsEmpty())
	{
		nPosition += m_lstParamStrips.GetCount();

		if(FAILED(AddStripToTimeline(pStrip, nPosition)))
		{	
			delete pStrip;
			return E_FAIL;
		}
	}*/

	AllStrips_RemoveFromTimeline();
	InsertByAscendingParameter(pStrip);
	AllStrips_AddToTimeline();

	AllStrips_Invalidate();
	pStrip->InvalidateFBar();

	m_pTrackMgr->SetLastEdit(IDS_UNDO_ADD_PARAM);
	m_pTrackMgr->TrackDataChanged();

	return S_OK;
	

}

CString CTrackObject::GetMinimizedStripText()
{
	CString sText;
	POSITION position= m_lstParamStrips.GetHeadPosition();
	while(position)
	{
		CParamStrip* pStrip = (CParamStrip*)m_lstParamStrips.GetNext(position);
		ASSERT(pStrip);
		if(pStrip)
		{
			CString sStripName = pStrip->GetName();
			if(!sStripName.IsEmpty())
			{
				if(!sText.IsEmpty())
				{
					sText += ",";
				}

				sText += sStripName;
			}
		}
	}

	if(sText.IsEmpty())
	{
		sText.LoadString(IDS_NO_PARAM_TEXT);
	}

	CString sFullText = sText;

	return sFullText;
}

HRESULT CTrackObject::DeleteAllStrips()
{
	POSITION position = m_lstParamStrips.GetHeadPosition();
	while(position)
	{
		CParamStrip* pParamStrip = m_lstParamStrips.GetNext(position);
		DeleteStrip(pParamStrip);
	}

	return S_OK;	
}

HRESULT CTrackObject::DeleteAllSelectedStrips()
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return E_UNEXPECTED;
	}


	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return E_UNEXPECTED;
	}

	BOOL fStripWasDeleted = FALSE;
	VARIANT var;

	// Delete all gutter selected strips
	POSITION pos = m_lstParamStrips.GetHeadPosition();
	while(pos)
	{
		CParamStrip* pParamStrip = m_lstParamStrips.GetNext(pos);
		if(SUCCEEDED(pTimeline->StripGetTimelineProperty( pParamStrip, STP_GUTTER_SELECTED, &var)))
		{
			if(V_BOOL(&var) == TRUE)
			{
				DeleteStrip(pParamStrip);
				fStripWasDeleted = TRUE;
			}
		}
	}
	
	// If none of the parameter control strips were gutter selected, delete the active strip
	IDMUSProdStrip* pIActiveStrip = NULL;
	if(fStripWasDeleted == FALSE)
	{
		if(SUCCEEDED(pTimeline->GetTimelineProperty(TP_ACTIVESTRIP, &var)))
		{
			if(V_UNKNOWN(&var) != NULL)
			{
				if(SUCCEEDED(V_UNKNOWN(&var)->QueryInterface(IID_IDMUSProdStrip, (void **) &pIActiveStrip)))
				{
					pos = m_lstParamStrips.GetHeadPosition();
					while(pos)
					{
						CParamStrip* pParamStrip = m_lstParamStrips.GetNext(pos);
						if(pParamStrip == pIActiveStrip)
						{
							DeleteStrip(pParamStrip);
						}
					}

					RELEASE(pIActiveStrip);
				}

				V_UNKNOWN(&var)->Release();
			}
		}
	}

	return S_OK;
}


void CTrackObject::DeleteStrip(CParamStrip* pParamStrip)
{
	ASSERT(pParamStrip);
	if(pParamStrip == NULL)
	{
		return;
	}

	RemoveStripFromTimeline(pParamStrip);

	POSITION position = m_lstParamStrips.Find(pParamStrip);
	if(position)
	{
		m_lstParamStrips.RemoveAt(position);
		pParamStrip->CleanUp();
		RELEASE(pParamStrip);
	}

	return;
}

DWORD CTrackObject::GetNumberOfStrips()
{
	return m_lstParamStrips.GetCount();
}

void CTrackObject::RefreshAllStrips()
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = m_pTrackMgr->GetTimeline();
	if(pTimeline == NULL)
	{
		return;
	}

	POSITION position = m_lstParamStrips.GetHeadPosition();
	while(position)
	{
		CParamStrip* pStrip = (CParamStrip*)m_lstParamStrips.GetNext(position);
		ASSERT(pStrip);
		if(pStrip)
		{
			pTimeline->StripInvalidateRect((IDMUSProdStrip*)pStrip, NULL, TRUE);
		}
	}
}

CString CTrackObject::GetName()
{
	return m_sObjectName;
}

HRESULT CTrackObject::GetParamEnumTypeText(GUID guidObject, DWORD dwParamIndex, WCHAR** ppwchParamEnums)
{
	IUnknown* pIUnkObject = NULL;
	if(FAILED(::CoCreateInstance(guidObject, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**) &pIUnkObject)))
	{
		return E_FAIL;
	}

	ASSERT(pIUnkObject);

	IMediaParamInfo* pIParamInfo = NULL;
	if(FAILED(pIUnkObject->QueryInterface(IID_IMediaParamInfo, (void**)&pIParamInfo)))
	{
		return E_FAIL;
	}

	if(FAILED(pIParamInfo->GetParamText(dwParamIndex, ppwchParamEnums)))
	{
		pIParamInfo->Release();
		pIUnkObject->Release();
		return E_FAIL;
	}

	CString sParamText = *ppwchParamEnums;
	
	pIParamInfo->Release();
	pIUnkObject->Release();
	return S_OK;
}

HRESULT CTrackObject::ParseEnumText(WCHAR* pwszParamText, CString& sParamName, CString& sParamUnitLabel, CList<CString, CString&>* plstEnumTypes, DWORD* pdwCount)
{
	ASSERT(pwszParamText);
	if(pwszParamText == NULL)
	{
		return E_POINTER;
	}

	ASSERT(plstEnumTypes);
	if(plstEnumTypes == NULL)
	{
		return E_POINTER;
	}

	ASSERT(pdwCount);
	if(pdwCount == NULL)
	{
		return E_POINTER;
	}

	// The paramtext format is [ParamName]\0[Unit]\0[Val1]\0[Val2]\0....\0[Val(n)]\0\0

	// Skip the Param Name and Unit label
	sParamName = pwszParamText;
	int nParamNameLength = sParamName.GetLength() + 1;

	WCHAR* pwszUnitLabel = pwszParamText + nParamNameLength;
	sParamUnitLabel = pwszUnitLabel;
	int nUnitLabelLength = sParamUnitLabel.GetLength() + 1;

	WCHAR* pwszText = pwszUnitLabel + nUnitLabelLength;

	while(1)
	{
		CString sText = pwszText;
		if(sText != "\0")
		{
			plstEnumTypes->AddTail(sText);
			int nTextLength = sText.GetLength() + 1;
			pwszText += nTextLength;
		}
		else
		{
			break;
		}
	}
	

	return S_OK;
}

GUID CTrackObject::GetCLSID()
{
	return m_guidObject;
}

void CTrackObject::GetObjectHeader(DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER& oObjectHeader)
{
	ZeroMemory( &oObjectHeader, sizeof(DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER) );

	// Fill in the members of the DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER structure
	oObjectHeader.dwFlags = m_dwObjectFlagsDM;
	oObjectHeader.guidTimeFormat = m_guidTimeFormat;
	oObjectHeader.dwPChannel = m_dwPChannel;
	oObjectHeader.dwBuffer = m_dwBuffer;
	oObjectHeader.dwStage = m_dwStage;
	oObjectHeader.guidObject = m_guidObject;
	oObjectHeader.dwIndex = m_dwObjectIndex;
}

void CTrackObject::RefreshObjectProperties()
{
	
	if(m_dwStage == DMUS_PATH_SEGMENT_TOOL || m_dwStage == DMUS_PATH_AUDIOPATH_TOOL ||
		m_dwStage == DMUS_PATH_PERFORMANCE_TOOL)
	{
		CDMToolInfo* pToolInfo = GetToolInfo(m_dwStage, m_guidObject);
		if(pToolInfo)
		{
			m_sObjectName = pToolInfo->m_ToolInfo.awchToolName;
			m_sPChannelText = pToolInfo->m_ToolInfo.awchPChannels;
			m_dwPChannel = pToolInfo->m_ToolInfo.dwFirstPChannel;
			m_dwBuffer = 0;

			delete pToolInfo;
		}

		m_sStageName = GetToolStageName(m_dwStage);
	}
	else if(m_dwStage == DMUS_PATH_BUFFER_DMO || m_dwStage == DMUS_PATH_MIXIN_BUFFER_DMO || m_dwStage == DMUS_PATH_PERFORMANCE)
	{
		CDMOInfo* pDMOInfo = GetDMOInfo(this);
		if(pDMOInfo)
		{
			m_sObjectName = pDMOInfo->m_DMOInfo.awchDMOName;
			m_sPChannelText = pDMOInfo->m_DMOInfo.awchPChannelText;
			m_dwPChannel = pDMOInfo->m_DMOInfo.dwPChannel;
			m_dwBuffer = pDMOInfo->m_DMOInfo.dwBufferIndex;
            m_guidInstance = pDMOInfo->m_DMOInfo.guidDesignGUID;

			m_bDisabled = FALSE;

			delete pDMOInfo;
		}
		else
		{
			m_bDisabled = TRUE;
		}

		if(m_sStageName.IsEmpty())
		{
			m_sStageName = GetDMOStageName(m_dwStage);
		}

	}
}

CDMToolInfo* CTrackObject::GetToolInfo(DWORD dwStage, GUID clsidObject)
{
	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return NULL;
	}

	IDMUSProdSegmentEdit8* pISegment = m_pTrackMgr->GetSegment();
	ASSERT(pISegment);
	if(pISegment == NULL)
	{
		return NULL;
	}

	IDMUSProdConductor* pIConductor = m_pTrackMgr->GetConductor();
	ASSERT(pIConductor);
	if(pIConductor == NULL)
	{
		return NULL;
	}

	CPtrList lstObjects;
	if(FAILED(CNewParameterDialog::GetToolList(pISegment, pIConductor, dwStage, &lstObjects)))
	{
		pISegment->Release();
		pIConductor->Release();
		return NULL;
	}

	CDMToolInfo* pMatchingTool = NULL;
	while(!lstObjects.IsEmpty())
	{
		CDMToolInfo* pToolInfo = (CDMToolInfo*) lstObjects.RemoveHead();
		ASSERT(pToolInfo);
		if(pToolInfo && ::IsEqualCLSID(pToolInfo->m_ToolInfo.clsidTool, clsidObject) == FALSE)
		{
			delete pToolInfo;
		}
		else
		{
			pMatchingTool = pToolInfo;
		}
	}

	pISegment->Release();
	pIConductor->Release();
	return pMatchingTool;
}

CDMOInfo* CTrackObject::GetDMOInfo(CTrackObject* pTrackObject)
{
	ASSERT(pTrackObject);
	if(pTrackObject == NULL)
	{
		return NULL;
	}

	ASSERT(m_pTrackMgr);
	if(m_pTrackMgr == NULL)
	{
		return NULL;
	}

	IDMUSProdSegmentEdit8* pISegment = m_pTrackMgr->GetSegment();
	ASSERT(pISegment);
	if(pISegment == NULL)
	{
		return NULL;
	}

	IDMUSProdConductor* pIConductor = m_pTrackMgr->GetConductor();
	ASSERT(pIConductor);
	if(pIConductor == NULL)
	{
		return NULL;
	}
	
	CPtrList lstObjects;
	CDMOInfo* pMatchingDMO = NULL;
	if(SUCCEEDED(CNewParameterDialog::GetDMOList(pISegment, pIConductor, pTrackObject->m_dwStage, &lstObjects)))
	{
		pMatchingDMO = CheckListForEqualObject(lstObjects, pTrackObject);
	}

	// No matching DMO? Then delete this list and get a new one
	if(pMatchingDMO == NULL)
	{
		// Delete the list
		while(!lstObjects.IsEmpty())
		{
			CDMOInfo* pDMOInfo = (CDMOInfo*)lstObjects.RemoveHead();
			if(pMatchingDMO != pDMOInfo)
			{
				delete pDMOInfo;
			}
		}
	}

	if(pMatchingDMO == NULL && SUCCEEDED(CNewParameterDialog::GetDMOList(pISegment, pIConductor, DMUS_PATH_PERFORMANCE, &lstObjects)))
	{
		pMatchingDMO = CheckListForEqualObject(lstObjects, pTrackObject);
	}

	// Delete the list
	while(!lstObjects.IsEmpty())
	{
		CDMOInfo* pDMOInfo = (CDMOInfo*)lstObjects.RemoveHead();
		if(pMatchingDMO != pDMOInfo)
		{
			delete pDMOInfo;
		}
	}

	pISegment->Release();
	pIConductor->Release();
	return pMatchingDMO;
}

CString CTrackObject::GetToolStageName(DWORD dwStage)
{
	for(int nIndex = 0; nIndex < MAX_STAGES_TOOLS; nIndex++)
	{
		CString sStageName;
		StageInfo stageInfo = CNewParameterDialog::m_arrToolsStages[nIndex];
		if(stageInfo.m_dwStage == dwStage)
		{
			sStageName.LoadString(stageInfo.m_uStageName);
			return sStageName;
		}
	}

	return "";
}

CString CTrackObject::GetDMOStageName(DWORD dwStage)
{
	for(int nIndex = 0; nIndex < MAX_STAGES_DMOS; nIndex++)
	{
		CString sStageName;
		StageInfo stageInfo = CNewParameterDialog::m_arrDMOStages[nIndex];
		if(stageInfo.m_dwStage == dwStage)
		{
			sStageName.LoadString(stageInfo.m_uStageName);
			return sStageName;
		}
	}

	return "";
}

CString CTrackObject::GetPChannelText()
{
	return m_sPChannelText;
}

CString	CTrackObject::GetObjectName()
{
	return m_sObjectName;
}

BOOL CTrackObject::IsEqualObject(GUID guidInstance)
{
    return ::IsEqualGUID(m_guidInstance, guidInstance);
}

BOOL CTrackObject::IsEqualObject(StripInfo* pStripInfo)
{
	ASSERT(pStripInfo);
	if(pStripInfo == NULL)
	{
		return FALSE;
	}

    if(pStripInfo->m_guidInstance == m_guidInstance)
    {
        return TRUE;
    }

	if(::IsEqualGUID(m_guidObject, pStripInfo->m_guidObject) == FALSE)
	{
		return FALSE;
	}

	if(m_dwStage != pStripInfo->m_dwStage)
	{
		return FALSE;
	}

	if(m_dwBuffer != pStripInfo->m_dwBuffer)
	{
		return FALSE;
	}

	if(m_dwPChannel != pStripInfo->m_dwPChannel)
	{
		return FALSE;
	}

	if(m_dwObjectIndex != pStripInfo->m_dwObjectIndex)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CTrackObject::IsDisabled()
{
	return m_bDisabled;
}

CDMOInfo* CTrackObject::CheckListForEqualObject(CPtrList& lstObjects, CTrackObject* pTrackObject)
{
	ASSERT(pTrackObject);
	if(pTrackObject == NULL)
	{
		return NULL;
	}

    CDMOInfo* pMatchingDMO = NULL;
	POSITION position = lstObjects.GetHeadPosition();
	while(position)
	{
		CDMOInfo* pDMOInfo = (CDMOInfo*) lstObjects.GetNext(position);
		ASSERT(pDMOInfo);
		if(pDMOInfo)
		{
            // Checking for instance GUID failed so check for everything else
			StripInfo stripInfo;
			stripInfo.m_dwStage = pDMOInfo->m_DMOInfo.dwStage;
			stripInfo.m_guidObject = pDMOInfo->m_DMOInfo.clsidDMO;
            stripInfo.m_guidInstance = pDMOInfo->m_DMOInfo.guidDesignGUID;
			stripInfo.m_dwBuffer = pDMOInfo->m_DMOInfo.dwBufferIndex;
			stripInfo.m_dwPChannel = pDMOInfo->m_DMOInfo.dwPChannel;
			stripInfo.m_dwObjectIndex = pDMOInfo->m_DMOInfo.dwEffectIndex;

			if(pTrackObject->IsEqualObject(&stripInfo) == TRUE)
			{
				return pDMOInfo;

			}
		}
	}

	return NULL;
}


