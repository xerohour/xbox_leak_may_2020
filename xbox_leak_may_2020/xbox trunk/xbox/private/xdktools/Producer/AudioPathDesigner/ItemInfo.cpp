// ItemInfo.cpp : implementation file
//

#include "stdafx.h"
#include "dsoundp.h"
#include "ItemInfo.h"
#include "EffectInfo.h"
#include "RiffStrm.h"
#include "AudioPath.h"
#include "AudioPathDesignerDLL.h"
#include "MultiTree.h"
#include "AudioPathDlg.h"
#include "MixGroupPPGMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif //_DEBUG


static CString PortNameFromGUID( const GUID &rguidPort )
{
	// Ensure we have a pointer to DirectMusic
	ASSERT( theApp.m_pAudioPathComponent->m_pIDMusic );

	// Check if asking for the default port
	if( rguidPort == GUID_Synth_Default )
	{
		// Yes - return 'Default Synth'
		CString strDefault;
		strDefault.LoadString( IDS_DEFAULT_PORT );
		return strDefault;
	}

	// Iterate through all ports
	for(DWORD dwIndex = 0; ; dwIndex++)
	{
		// Initialize dmpc
		DMUS_PORTCAPS dmpc;
		ZeroMemory(&dmpc, sizeof(dmpc));
		dmpc.dwSize = sizeof(DMUS_PORTCAPS);

		// Get the port's capabilities
		HRESULT hr = theApp.m_pAudioPathComponent->m_pIDMusic->EnumPort(dwIndex, &dmpc);

		// If we succeeded and didn't hit the end of the list
		if(SUCCEEDED(hr) && (hr != S_FALSE) )
		{
			if( rguidPort == dmpc.guidPort )
			{
				return CString(dmpc.wszDescription);
			}
		}
		else
		{
			break;
		}
	}

	CString strUnknown;
	strUnknown.LoadString( IDS_UNKNOWN_SYNTH_NAME );
	return strUnknown;
}

/////////////////////////////////////////////////////////////////////////////
// BufferOptions

BufferOptions::BufferOptions()
{
	Empty();
}

BufferOptions::~BufferOptions()
{
	// Delete just the effect wrappers
	while( !lstEffects.IsEmpty() )
	{
		delete lstEffects.RemoveHead();
	}
}

void BufferOptions::Empty( void )
{
	// Delete just the effect wrappers
	while( !lstEffects.IsEmpty() )
	{
		delete lstEffects.RemoveHead();
	}

	lstBusIDs.RemoveAll();
#ifndef DMP_XBOX
	lstBusIDs.Add( DSBUSID_LEFT );
	lstBusIDs.Add( DSBUSID_RIGHT );
	wChannels = 2;
	CoCreateGuid( &guidBuffer );
	dwBufferFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFX | DSBCAPS_CTRLPAN | DSBCAPS_LOCDEFER;
	dwHeaderFlags = 0;
#else
	wChannels = 1;
	guidBuffer = GUID_Buffer_3D_Dry;
	dwBufferFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFX | DSBCAPS_CTRLPAN | DSBCAPS_LOCDEFER;
	dwHeaderFlags = DMUS_BUFFERF_DEFINED;
#endif
	guid3DAlgorithm = DS3DALG_DEFAULT;
	InitializeDS3DBUFFER( &ds3DBuffer );
	lVolume = 0;
	lPan = 0;
	fSelected = false;
	CoCreateGuid( &guidDesignEffectGUID );
}

HRESULT BufferOptions::Write( IDMUSProdRIFFStream *pIRiffStream ) const
{
	if( !pIRiffStream )
	{
		return E_POINTER;
	}

	IStream *pStream = pIRiffStream->GetStream();
	if( !pStream )
	{
		return E_POINTER;
	}

	// Save the buffer in a LIST chunk
	MMCKINFO ckBuffer;
	ckBuffer.fccType = DMUSPROD_FOURCC_ITEM_BUFFER_LIST;
	if( 0 == pIRiffStream->CreateChunk( &ckBuffer, MMIO_CREATELIST ) )
	{
		MMCKINFO ckSubChunk;
		// If there are any bus IDs
		if( lstBusIDs.GetSize() > 0 )
		{
			// Save the bus IDs in a BUS_ID chunk
			ckSubChunk.ckid = DMUSPROD_FOURCC_BUS_ID;
			if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, 0 ) )
			{
				pStream->Write( lstBusIDs.GetData(), sizeof(DWORD) * lstBusIDs.GetSize(), NULL );
				pIRiffStream->Ascend( &ckSubChunk, 0 );
			}
		}

		// Save the buffer settings in a BUFFER_CHUNK chunk
		ckSubChunk.ckid = DMUSPROD_FOURCC_BUFFER_CHUNK;
		if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, 0 ) )
		{
			ioBufferOptions oBufferOptions;
			memcpy( &oBufferOptions.ds3DBuffer, &ds3DBuffer, sizeof( DS3DBUFFER ) );
			oBufferOptions.dwBufferFlags = dwBufferFlags;
			oBufferOptions.dwHeaderFlags = dwHeaderFlags;
			oBufferOptions.guid3DAlgorithm = guid3DAlgorithm;
			oBufferOptions.guidBuffer = guidBuffer;
			oBufferOptions.wChannels = wChannels;
			oBufferOptions.lPan = lPan;
			oBufferOptions.lVolume = lVolume;

			pStream->Write( &oBufferOptions, sizeof( ioBufferOptions ), NULL );
			pIRiffStream->Ascend( &ckSubChunk, 0 );
		}

		// Only save the effect list when using a non-defined buffer
		if( !(dwHeaderFlags & DMUS_BUFFERF_DEFINED) )
		{
			// If there are any effects, save them
			POSITION pos = lstEffects.GetHeadPosition();
			while( pos )
			{
				lstEffects.GetNext( pos )->m_EffectInfo.Write( pIRiffStream );
			}
		}

		pIRiffStream->Ascend( &ckBuffer, 0 );
	}

	pStream->Release();

	return S_OK;
}

HRESULT BufferOptions::Read( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO *pckParent, CDirectMusicAudioPath *pAudioPath )
{
	if( !pIRiffStream || !pckParent )
	{
		return E_POINTER;
	}

	// Get a pointer to the IStream
	IStream *pStream = pIRiffStream->GetStream();
	if( !pStream )
	{
		return E_POINTER;
	}

	// Initialize ourself to a clean state
	Empty();

	// Initialize the result to S_OK
	HRESULT hr = S_OK;

	// Descend into all chunks within pckParent
	MMCKINFO ckChunk;
	DWORD cbRead;
	while( SUCCEEDED(hr)
		&& (0 == pIRiffStream->Descend( &ckChunk, pckParent, 0 )) )
	{
		switch( ckChunk.ckid )
		{
		case DMUSPROD_FOURCC_BUS_ID:
			if( ckChunk.cksize >= sizeof(DWORD) )
			{
				// Remove any existing bus IDs
				lstBusIDs.RemoveAll();

				// Read in the bus IDs
				long lSize = ckChunk.cksize;
				DWORD dwBusIDTmp;
				while( SUCCEEDED(hr)
				&&	(lSize >= sizeof(DWORD)) )
				{
					hr = pStream->Read( &dwBusIDTmp, sizeof(DWORD), &cbRead );
					lSize -= sizeof(DWORD);

					// If we didn't read in enough data, set hr to a failure code
					if( SUCCEEDED(hr)
					&&	(cbRead != sizeof(DWORD)) )
					{
						hr = E_FAIL;
					}
					else
					{
						// Otherwise, add the bus ID on to the end of the array
						lstBusIDs.Add( dwBusIDTmp );
					}
				}
			}
			else
			{
				// Chunk is too small - fail
				hr = E_FAIL;
			}
			break;

		case DMUSPROD_FOURCC_BUFFER_CHUNK:
			{
				// Read in the buffer settings
				ioBufferOptions iBufferOptions;
				DWORD cbToRead = min( ckChunk.cksize, sizeof( ioBufferOptions ) );
				hr = pStream->Read( &iBufferOptions, cbToRead, &cbRead );

				// If we didn't read in enough data, set hr to a failure code
				if( SUCCEEDED(hr)
				&&	(cbRead != cbToRead) )
				{
					hr = E_FAIL;
				}
				else
				{
					// Otherwise, apply the settings
					memcpy( &ds3DBuffer, &iBufferOptions.ds3DBuffer, sizeof( DS3DBUFFER ) );
					ds3DBuffer.dwSize = sizeof( DS3DBUFFER );
					dwBufferFlags = iBufferOptions.dwBufferFlags;
					dwHeaderFlags = iBufferOptions.dwHeaderFlags;
					guid3DAlgorithm = iBufferOptions.guid3DAlgorithm;
					guidBuffer = iBufferOptions.guidBuffer;
					wChannels = iBufferOptions.wChannels;
					lPan = iBufferOptions.lPan;
					lVolume = iBufferOptions.lVolume;
				}
			}
			break;

		case FOURCC_LIST:
			switch( ckChunk.fccType )
			{
			// Read in each effect chunk
			case DMUSPROD_FOURCC_EFFECTINFO_LIST:
				{
					// Try and read in the effect and
					// Get a pointer to the EffectInfo structure
					EffectInfo *pEffectInfo = pAudioPath->LoadEffect( pIRiffStream, &ckChunk );

					if( pEffectInfo )
					{
						// If the read succeeded, create a BusEffectInfo to wrap the EffectInfo
						BusEffectInfo *pBusEffectInfo = new BusEffectInfo(pEffectInfo);

						// Add the effect to our list of effects
						lstEffects.AddTail( pBusEffectInfo );

						delete pEffectInfo;
					}
				}
				break;
			}
			break;
		}

		pIRiffStream->Ascend( &ckChunk, 0 );
	}

	// Release the IStream
	pStream->Release();

	// Return the result
	return hr;
}

void BufferOptions::Copy( const BufferOptions &bufferOptions )
{
	// Delete just the effect wrappers
	while( !lstEffects.IsEmpty() )
	{
		delete lstEffects.RemoveHead();
	}

	lstBusIDs.Copy( bufferOptions.lstBusIDs );
	guidBuffer = bufferOptions.guidBuffer;
	dwBufferFlags = bufferOptions.dwBufferFlags;
	dwHeaderFlags = bufferOptions.dwHeaderFlags;
	guid3DAlgorithm = bufferOptions.guid3DAlgorithm;
	wChannels = bufferOptions.wChannels;
	ds3DBuffer = bufferOptions.ds3DBuffer;
	fSelected = bufferOptions.fSelected;
	CoCreateGuid( &guidDesignEffectGUID );

	lPan = bufferOptions.lPan;
	lVolume = bufferOptions.lVolume;

	POSITION pos = bufferOptions.lstEffects.GetHeadPosition();
	while( pos )
	{
		BusEffectInfo *pBusEffectInfo = new BusEffectInfo( bufferOptions.lstEffects.GetNext( pos ) );
		lstEffects.AddTail( pBusEffectInfo );
	}
}

/////////////////////////////////////////////////////////////////////////////
// ItemInfo

ItemInfo::ItemInfo()
{
	nNumPChannels = 0;
	adwPChannels = NULL;
	pPortOptions = NULL;
	fSubFieldSelected = false;
	dwDisplayIndex = 0;
}

ItemInfo::ItemInfo( DWORD dwPChannel )
{
	nNumPChannels = 1;
	adwPChannels = new DWORD[1];
	adwPChannels[0] = dwPChannel;
	pPortOptions = NULL;
	fSubFieldSelected = false;
	dwDisplayIndex = 0;
}

ItemInfo::ItemInfo( LPCTSTR strText )
{
	strBandName = strText;
	pPortOptions = NULL;
	nNumPChannels = 0;
	adwPChannels = NULL;
	fSubFieldSelected = false;
	dwDisplayIndex = 0;
}

void ItemInfo::Empty()
{
	if( adwPChannels )
	{
		delete []adwPChannels;
		adwPChannels = NULL;
	}
	nNumPChannels = 0;
	fSubFieldSelected = false;
	while( !lstBuffers.IsEmpty() )
	{
		delete lstBuffers.RemoveHead();
	}
	strBandName.Empty();

	// Don't delete pPortOptions - it is managed by CDirectMusicAudioPath
	pPortOptions = NULL;
	dwDisplayIndex = 0;
}

ItemInfo::~ItemInfo()
{
	while( !lstBuffers.IsEmpty() )
	{
		delete lstBuffers.RemoveHead();
	}
	if( adwPChannels )
	{
		delete[] adwPChannels;
		adwPChannels = NULL;
	}
	pPortOptions = NULL;
}

HRESULT ItemInfo::Write( IDMUSProdRIFFStream *pIRiffStream, const ItemInfo *pParentInfo ) const
{
	if( !pIRiffStream )
	{
		return E_POINTER;
	}

	IStream *pStream = pIRiffStream->GetStream();
	if( !pStream )
	{
		return E_POINTER;
	}

	// Save the item in a LIST chunk
	MMCKINFO ckItem;
	ckItem.fccType = DMUSPROD_FOURCC_ITEMINFO_LIST;
	if( 0 == pIRiffStream->CreateChunk( &ckItem, MMIO_CREATELIST ) )
	{
		MMCKINFO ckSubChunk;

		// Save the name in a UNAM chunk
		if( !strBandName.IsEmpty()
		||	(pParentInfo && !pParentInfo->strBandName.IsEmpty()) )
		{
			ckSubChunk.ckid = DMUS_FOURCC_UNAM_CHUNK;
			if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, 0 ) )
			{
				SaveMBStoWCS( pStream, pParentInfo ? &pParentInfo->strBandName : &strBandName );
				pIRiffStream->Ascend( &ckSubChunk, 0 );
			}
		}

		// Save the PChannel list in an DMUSPROD_FOURCC_PCHANNEL_CHUNK chunk
		if( nNumPChannels )
		{
			ckSubChunk.ckid = DMUSPROD_FOURCC_PCHANNEL_CHUNK;
			if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, 0 ) )
			{
				pStream->Write( adwPChannels, nNumPChannels * sizeof(DWORD), NULL );
				pIRiffStream->Ascend( &ckSubChunk, 0 );
			}
		}

		// Write out the port settings
		if( pParentInfo
		&&	pParentInfo->pPortOptions )
		{
			pParentInfo->pPortOptions->Write( pIRiffStream );
		}
		else if( pPortOptions )
		{
			pPortOptions->Write( pIRiffStream );
		}

		// If there are any buffers, save them
		if( pParentInfo )
		{
			POSITION posBuffer = pParentInfo->lstBuffers.GetHeadPosition();
			while( posBuffer )
			{
				pParentInfo->lstBuffers.GetNext( posBuffer )->Write( pIRiffStream );
			}
		}
		else
		{
			POSITION posBuffer = lstBuffers.GetHeadPosition();
			while( posBuffer )
			{
				lstBuffers.GetNext( posBuffer )->Write( pIRiffStream );
			}
		}
			/*
			ckSubChunk.fccType = DMUSPROD_FOURCC_ITEM_BUFFER_LIST;
			if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, MMIO_CREATELIST ) )
			{
				if( pParentInfo )
				{
					POSITION posBus = pParentInfo->lstBuffers.GetHeadPosition();
					while( posBus )
					{
						pParentInfo->lstBuffers.GetNext( posBus )->Write( pIRiffStream );
					}
				}
				else
				{
					POSITION posBus = lstBuffers.GetHeadPosition();
					while( posBus )
					{
						lstBuffers.GetNext( posBus )->Write( pIRiffStream );
					}
				}
				pIRiffStream->Ascend( &ckSubChunk, 0 );
			}
			*/

		pIRiffStream->Ascend( &ckItem, 0 );
	}

	pStream->Release();

	return S_OK;
}

HRESULT ItemInfo::Read( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO *pckParent, CDirectMusicAudioPath *pAudioPath )
{
	if( !pIRiffStream || !pckParent )
	{
		return E_POINTER;
	}

	// Get a pointer to the IStream
	IStream *pStream = pIRiffStream->GetStream();
	if( !pStream )
	{
		return E_POINTER;
	}

	// Initialize ourself to a clean state
	Empty();

	// Initialize the result to S_OK
	HRESULT hr = S_OK;

	// Descend into all chunks within pckParent
	MMCKINFO ckChunk;
	DWORD cbRead;
	while( SUCCEEDED(hr)
		&& (0 == pIRiffStream->Descend( &ckChunk, pckParent, 0 )) )
	{
		switch( ckChunk.ckid )
		{
		// Read in the mix group name
		case DMUS_FOURCC_UNAM_CHUNK:
			ReadMBSfromWCS( pStream, ckChunk.cksize, &strBandName );
			break;

		// Read in the list of PChannels
		case DMUSPROD_FOURCC_PCHANNEL_CHUNK:
			// Compute how many PChannels there are
			nNumPChannels = ckChunk.cksize / sizeof(DWORD);

			// Allocate memory for the list of PChannels
			adwPChannels = new DWORD[nNumPChannels];

			// Try and read in the list of PChannels
			hr = pStream->Read( adwPChannels, nNumPChannels * sizeof(DWORD), &cbRead );

			// If we were unable to read all of the PChannels, set hr to a failure code
			if( SUCCEEDED(hr)
			&&	(cbRead != nNumPChannels * sizeof(DWORD)) )
			{
				hr = E_FAIL;
			}

			// If we failed, set the number of PChannels to 0 and delete the memory
			// allocated for them
			if( FAILED(hr) )
			{
				nNumPChannels = 0;
				delete []adwPChannels;
				adwPChannels = NULL;
			}
			break;

		case FOURCC_LIST:
			switch( ckChunk.fccType )
			{
			case DMUSPROD_FOURCC_PORTSETTINGS_LIST:
				// Read in the port settings chunk
				{
					// Create a temporary PortOptions structure
					PortOptions *pTmpPortOptions = new PortOptions;

					// Read in the structure
					hr = pTmpPortOptions->Read( pIRiffStream, &ckChunk );

					if( SUCCEEDED( hr ) )
					{
						// Read succeeded - add the port to the AudioPath's list
						// of ports, if necessary
						pPortOptions = pAudioPath->PastePortOptionsHelper( pTmpPortOptions );
					}
					else
					{
						// Read failed - delete the port
						delete pTmpPortOptions;
					}
				}
				break;

			case DMUSPROD_FOURCC_ITEM_BUFFER_LIST:
				// Read in a buffer
				{
					// Create a new BufferOptions structure
					BufferOptions *pBufferOptions = new BufferOptions();

					// Read in the BufferOptions structure
					hr = pBufferOptions->Read( pIRiffStream, &ckChunk, pAudioPath );

					if( SUCCEEDED( hr ) )
					{
						// If the read succeeded, add it to the list of buses
						lstBuffers.AddTail( pBufferOptions );
					}
					else
					{
						// If the read failed, delete the BufferOptions structure
						delete pBufferOptions;
					}
					/*
					// Iterate through all BUS_LIST chunks
					MMCKINFO ckBus;
					while( SUCCEEDED(hr)
						&& (0 == pIRiffStream->Descend( &ckBus, &ckChunk, 0 )) )
					{
						switch( ckBus.ckid )
						{
						case FOURCC_LIST:
							switch( ckBus.fccType )
							{
							case DMUSPROD_FOURCC_BUS_LIST:
								{
									// Create a new BufferOptions structure
									BufferOptions *pBufferOptions = new BufferOptions();

									// Read in the BufferOptions structure
									hr = pBufferOptions->Read( pIRiffStream, &ckBus, pAudioPath );

									if( SUCCEEDED( hr ) )
									{
										// If the read succeeded, add it to the list of buses
										lstBuffers.AddTail( pBufferOptions );
									}
									else
									{
										// If the read failed, delete the BufferOptions structure
										delete pBufferOptions;
									}
								}
								break;
							}
						}

						// Ascend out of the BUS_LIST chunk
						pIRiffStream->Ascend( &ckBus, 0 );
					}
					*/
				}
				break;
			}
			break;
		}

		// Ascend out of the child chunk
		pIRiffStream->Ascend( &ckChunk, 0 );
	}

	// Release the IStream
	pStream->Release();

	// Return the result
	return hr;
}

bool ItemInfo::Select( bool fNewSelectState )
{
	bool fRefresh = false;

	if( !fNewSelectState && fSubFieldSelected )
	{
		fSubFieldSelected = false;
		fRefresh = true;
	}

	BEI_SELECTION beiNewState = fNewSelectState ? BEI_NORMAL : BEI_NONE;

	POSITION posBuffer = lstBuffers.GetHeadPosition();
	while( posBuffer )
	{
		BufferOptions *pBufferOptions = lstBuffers.GetNext( posBuffer );
		if( pBufferOptions->fSelected != fNewSelectState )
		{
			pBufferOptions->fSelected = fNewSelectState;
			fRefresh = true;
		}

		POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
		while( posEffect )
		{
			BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );
			if( pBusEffectInfo->m_beiSelection != beiNewState )
			{
				pBusEffectInfo->m_beiSelection = beiNewState;
				fRefresh = true;
			}
		}
	}

	return fRefresh;
}

void ItemInfo::Copy( const ItemInfo &itemInfo )
{
	strBandName = itemInfo.strBandName;
	fSubFieldSelected = itemInfo.fSubFieldSelected;
	nNumPChannels = itemInfo.nNumPChannels;
	if( adwPChannels )
	{
		delete [] adwPChannels;
		adwPChannels = 0;
	}
	if( nNumPChannels )
	{
		adwPChannels = new DWORD[nNumPChannels];
		memcpy( adwPChannels, itemInfo.adwPChannels, sizeof(DWORD) * nNumPChannels );
	}

	pPortOptions = itemInfo.pPortOptions;

	while( !lstBuffers.IsEmpty() )
	{
		delete lstBuffers.RemoveHead();
	}

	POSITION pos = itemInfo.lstBuffers.GetHeadPosition();
	while( pos )
	{
		BufferOptions *pBufferOptions = new BufferOptions;
		pBufferOptions->Copy( *itemInfo.lstBuffers.GetNext( pos ) );
		lstBuffers.AddTail( pBufferOptions );
	}
}

void CreateBuffers( ioPortConfig *pioPortConfig,
			 const DWORD dwPChStart, const DWORD dwPChannelSpan,
			 CString strMixGroupName,
			 const CTypedPtrList< CPtrList, BufferOptions *> &lstBuffers,
			 CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers )
{
	// Create a new ioPChannelToBuffer for this buffer
	ioPChannelToBuffer *pioPChannelToBuffer = new ioPChannelToBuffer;
	pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelBase = dwPChStart;
	pioPChannelToBuffer->ioPChannelToBufferHeader.dwPChannelCount = dwPChannelSpan;
	pioPChannelToBuffer->ioPChannelToBufferHeader.dwBufferCount = lstBuffers.GetCount();
	//pioPChannelToBuffer->ioPChannelToBufferHeader.dwFlags = 0; // TODO: Fix this
	pioPChannelToBuffer->strMixGroupName = strMixGroupName;

	// Iterate through the list of buffers
	POSITION posBuffer = lstBuffers.GetHeadPosition();
	while( posBuffer )
	{
		// Get a pointer to each BufferOptions structure
		BufferOptions *pBufferOptions = lstBuffers.GetNext( posBuffer );

		// Store the buffer's GUID
		GUID *pGuid = new GUID;
		*pGuid = pBufferOptions->guidBuffer;
		pioPChannelToBuffer->lstGuids.AddTail( pGuid );

		// If a defined buffer, store the design-time GUID
		if( pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED )
		{
			GUID *pGuid = new GUID;
			*pGuid = pBufferOptions->guidBuffer;
			pioPChannelToBuffer->lstDesignGuids.AddTail( pGuid );
			pGuid = new GUID;
			*pGuid = pBufferOptions->guidDesignEffectGUID;
			pioPChannelToBuffer->lstDesignGuids.AddTail( pGuid );
		}

		// Look for an existing ioDSoundBuffer structure for this GUID
		// If we're using a standard GUID, there can only be one
		bool fFoundExistingBuffer = false;
		POSITION posDSBuffer = lstDSoundBuffers.GetHeadPosition();
		while( posDSBuffer )
		{
			ioDSoundBuffer *pioDSoundBuffer = lstDSoundBuffers.GetNext( posDSBuffer );
			if( pioDSoundBuffer->ioBufferHeader.guidBufferID == *pGuid )
			{
				fFoundExistingBuffer = true;
				break;
			}
		}

		// If we didn't find an existing buffer, create a new one
		if( !fFoundExistingBuffer )
		{
			// Create a new ioDSoundBuffer structure
			ioDSoundBuffer *pioDSoundBuffer = new ioDSoundBuffer;

			// Copy the header information
			pioDSoundBuffer->ioBufferHeader.guidBufferID = pBufferOptions->guidBuffer;
			pioDSoundBuffer->ioBufferHeader.dwFlags = pBufferOptions->dwHeaderFlags;

			// If not using a standard buffer
			if( !(pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED) )
			{
				// Copy the buffer-specific information
				pioDSoundBuffer->ioDSBufferDesc.dwFlags = pBufferOptions->dwBufferFlags;
				pioDSoundBuffer->ioDSBufferDesc.lVolume = pBufferOptions->lVolume;
				pioDSoundBuffer->ioDSBufferDesc.lPan = pBufferOptions->lPan;
				//pioDSoundBuffer->ioDSBufferDesc.dwReserved = 0;

				// Ensure there are at least as many channels as Bus IDs
				pioDSoundBuffer->ioDSBufferDesc.nChannels = max( WORD(pBufferOptions->lstBusIDs.GetSize()), pBufferOptions->wChannels );

				// Copy the 3D information
				pioDSoundBuffer->ioDS3D.guid3DAlgorithm = pBufferOptions->guid3DAlgorithm;
				pioDSoundBuffer->ioDS3D.ds3d = pBufferOptions->ds3DBuffer;

				// Set the bus ID(s) for this buffer
				pioDSoundBuffer->lstBusIDs.Copy( pBufferOptions->lstBusIDs );

				// Only save the effect list when using a non-defined buffer
				// and when the buffer has the CTRLFX flag set
				if( !(pBufferOptions->dwHeaderFlags & DMUS_BUFFERF_DEFINED) )
				{
					// Now, save the information for the effect chain
					POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
					while( posEffect )
					{
						// Get a pointer to each effect
						BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );

						// Create a new ioDSoundEffect structure
						ioDSoundEffect *pioDSoundEffect = new ioDSoundEffect;

						// Initialize the DMUS_IO_FX_HEADER structure
						pioDSoundEffect->ioFXHeader.dwEffectFlags = pBusEffectInfo->m_EffectInfo.m_dwFlags;
						pioDSoundEffect->ioFXHeader.guidDSFXClass = pBusEffectInfo->m_EffectInfo.m_clsidObject;
						//pioDSoundEffect->ioFXHeader.guidReserved = GUID_AllZeros;
						pioDSoundEffect->ioFXHeader.guidSendBuffer = pBusEffectInfo->m_EffectInfo.m_clsidSendBuffer;
						//pioDSoundEffect->ioFXHeader.dwReserved = 0;

						// Copy the effect's instance name and real name
						pioDSoundEffect->strInstanceName = pBusEffectInfo->m_EffectInfo.m_strInstanceName;
						pioDSoundEffect->strEffectName = pBusEffectInfo->m_EffectInfo.m_strName;

						// Copy the effect's guid
						pioDSoundEffect->guidDesignGUID = pBusEffectInfo->m_guidEffectInfo;

						// Clone the effect's data stream, if necessary
						if( pBusEffectInfo->m_EffectInfo.m_pIStream )
						{
							pBusEffectInfo->m_EffectInfo.m_pIStream->Clone( &pioDSoundEffect->pStreamData );
						}

						// Add the effect to the end of the effect chain
						pioDSoundBuffer->lstDSoundEffects.AddTail( pioDSoundEffect );
					}
				}
			}

			// if not using a defined buffer
			if( !(pioDSoundBuffer->ioBufferHeader.dwFlags & DMUS_BUFFERF_DEFINED) )
			{
				// Ensure either the Ctrl3D or CtrlPan flag is set
				if( pioDSoundBuffer->ioDSBufferDesc.dwFlags & DSBCAPS_CTRL3D )
				{
					pioDSoundBuffer->ioDSBufferDesc.dwFlags &= ~DSBCAPS_CTRLPAN;
				}
				else
				{
					pioDSoundBuffer->ioDSBufferDesc.dwFlags |= DSBCAPS_CTRLPAN;
				}

				// Ensure only the globalfocus flag is set
				pioDSoundBuffer->ioDSBufferDesc.dwFlags |= DSBCAPS_GLOBALFOCUS;
				pioDSoundBuffer->ioDSBufferDesc.dwFlags &= ~DSBCAPS_STICKYFOCUS;

				// Ensure the CtrlVolume flag is set
				pioDSoundBuffer->ioDSBufferDesc.dwFlags |= DSBCAPS_CTRLVOLUME;

				// Ensure the CtrlFx flag is set iff there are effects
				if( pioDSoundBuffer->lstDSoundEffects.GetCount() > 0 )
				{
					pioDSoundBuffer->ioDSBufferDesc.dwFlags |= DSBCAPS_CTRLFX;
				}
				else
				{
					pioDSoundBuffer->ioDSBufferDesc.dwFlags &= ~DSBCAPS_CTRLFX;
				}
			}

			if( dwPChannelSpan == 0 )
			{
				// There are no PChannel pointing to this buffer, so use the MixGroup name
				pioDSoundBuffer->strName = strMixGroupName;

				// Flag the buffer as 'MixIn'
				pioDSoundBuffer->ioBufferHeader.dwFlags |= DMUS_BUFFERF_MIXIN;
				pioDSoundBuffer->ioDSBufferDesc.dwFlags |= DSBCAPS_MIXIN;

				// Ensure none of the location flags are set
				pioDSoundBuffer->ioDSBufferDesc.dwFlags &= ~(DSBCAPS_LOCDEFER | DSBCAPS_LOCSOFTWARE | DSBCAPS_LOCHARDWARE);

				// Ensure there are no BusIDs
				pioDSoundBuffer->lstBusIDs.RemoveAll();
			}
			else
			{
				// Clear the name of the DSound buffer
				pioDSoundBuffer->strName.Empty();

				// Ensure the buffer is not MixIn
				pioDSoundBuffer->ioBufferHeader.dwFlags &= ~DMUS_BUFFERF_MIXIN;
				pioDSoundBuffer->ioDSBufferDesc.dwFlags &= ~DSBCAPS_MIXIN;

				// Ensure only the LocDefer location flag is set
				pioDSoundBuffer->ioDSBufferDesc.dwFlags |= DSBCAPS_LOCDEFER;
				pioDSoundBuffer->ioDSBufferDesc.dwFlags &= ~(DSBCAPS_LOCSOFTWARE | DSBCAPS_LOCHARDWARE);

				// if not using a defined buffer
				if( !(pioDSoundBuffer->ioBufferHeader.dwFlags & DMUS_BUFFERF_DEFINED) )
				{
					// Verify there is at least one Bus ID
					ASSERT( pioDSoundBuffer->lstBusIDs.GetSize() > 0 );
				}
			}

			// Now, add the ioDSoundBuffer to the end of the list of buffers
			lstDSoundBuffers.AddTail( pioDSoundBuffer );
		}
	}

	if( pioPortConfig
	&&	(dwPChannelSpan > 0) )
	{
		// Add the ioPChannelToBuffer to the end of the list of PChannel->Buffer mappings
		pioPortConfig->lstPChannelToBuffer.AddTail( pioPChannelToBuffer );
	}
	else
	{
		// No PChannels, so delete pioPChannelToBuffer
		delete pioPChannelToBuffer;
	}
}

void CreateSynthAndBuffers( CTypedPtrList< CPtrList, ioPortConfig*> &lstPortConfigs,
			 PortOptions *pPortOptions, 
			 const DWORD dwPChStart, const DWORD dwPChannelSpan,
			 CString strMixGroupName,
			 const CTypedPtrList< CPtrList, BufferOptions *> &lstBuffers,
			 CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers )
{
	ioPortConfig *pioPortConfig = NULL;

	if( dwPChannelSpan > 0 )
	{
		ASSERT( pPortOptions );

		DMUS_PORTPARAMS8 dmPortParams;
		ZeroMemory( &dmPortParams, sizeof(DMUS_PORTPARAMS8) );
		dmPortParams.dwSize = sizeof(DMUS_PORTPARAMS8);
		dmPortParams.dwValidParams = DMUS_PORTPARAMS_EFFECTS | DMUS_PORTPARAMS_SAMPLERATE | DMUS_PORTPARAMS_VOICES| DMUS_PORTPARAMS_FEATURES;
		dmPortParams.dwSampleRate = pPortOptions->m_dwSampleRate;
		dmPortParams.dwVoices = pPortOptions->m_dwVoices;
		dmPortParams.dwEffectFlags = pPortOptions->m_dwEffects;
		dmPortParams.dwFeatures = pPortOptions->m_fAudioPath ? DMUS_PORT_FEATURE_AUDIOPATH | DMUS_PORT_FEATURE_STREAMING : 0;

		const DWORD dwPChannelEndPlusOne = dwPChStart + dwPChannelSpan;
		POSITION posSynth = lstPortConfigs.GetHeadPosition();
		while( posSynth )
		{
			ioPortConfig *pPortConfig = lstPortConfigs.GetNext( posSynth );

			// Check if the PChannel ranges are adjacent
			if( (pPortConfig->ioPortConfigHeader.dwPChannelBase == dwPChannelEndPlusOne)
			||	(pPortConfig->ioPortConfigHeader.dwPChannelBase + pPortConfig->ioPortConfigHeader.dwPChannelCount == dwPChStart) )
			{
				// Check if the port guids and parameters are equal
				if( (pPortOptions->m_guidPort == pPortConfig->ioPortConfigHeader.guidPort)
				&&	(0 == memcmp( &pPortConfig->ioPortParams, &dmPortParams, sizeof( DMUS_PORTPARAMS8 ) )) )
				{
					// Synths are equal and adjacent - just extend the PChannel range
					pPortConfig->ioPortConfigHeader.dwPChannelBase = min( dwPChStart, pPortConfig->ioPortConfigHeader.dwPChannelBase );
					pPortConfig->ioPortConfigHeader.dwPChannelCount += dwPChannelSpan;

					// Create any buffers for this range of PChannels, if the port support audio paths
					if( pPortOptions->m_fAudioPath )
					{
						CreateBuffers( pPortConfig, dwPChStart, dwPChannelSpan, strMixGroupName, lstBuffers, lstDSoundBuffers );
					}
					else
					{
						// Otherwise, create a design-time chunk to store the PChannel range and Mix Group
						ioDesignMixGroup *pioDesignMixGroup = new ioDesignMixGroup;
						pioDesignMixGroup->dwPChannelStart = dwPChStart;
						pioDesignMixGroup->dwPChannelSpan = dwPChannelSpan;
						pioDesignMixGroup->strMixGroupName = strMixGroupName;
						pPortConfig->lstDesignMixGroup.AddTail( pioDesignMixGroup );
					}

					return;
				}
			}
		}

		// Didn't find an equal and adjacent synth - must create a new one
		pioPortConfig = new ioPortConfig;

		// Set up the DMUS_IO_PORTCONFIG_HEADER structure
		pioPortConfig->ioPortConfigHeader.guidPort = pPortOptions->m_guidPort;
		pioPortConfig->ioPortConfigHeader.dwPChannelBase = dwPChStart;
		pioPortConfig->ioPortConfigHeader.dwPChannelCount = dwPChannelSpan;
		pioPortConfig->ioPortConfigHeader.dwFlags = DMUS_PORTCONFIGF_DRUMSON10;

		// Just copy the DMUS_PORTPARAMS8 structure
		memcpy( &(pioPortConfig->ioPortParams ), &dmPortParams, sizeof(DMUS_PORTPARAMS8) );

		// Copy the names for this port
		pioPortConfig->strPortName = pPortOptions->m_strName;

		// Add the new synth to the list
		lstPortConfigs.AddTail( pioPortConfig );
	}

	// Create any buffers for this range of PChannels, if
	// the PChannel span is empty, or if the port supports AudioPaths
	if( dwPChannelSpan == 0
	||	(pPortOptions && pPortOptions->m_fAudioPath ) )
	{
		CreateBuffers( pioPortConfig, dwPChStart, dwPChannelSpan, strMixGroupName, lstBuffers, lstDSoundBuffers );
	}
	else
	{
		// Otherwise, create a design-time chunk to store the PChannel range and Mix Group
		ioDesignMixGroup *pioDesignMixGroup = new ioDesignMixGroup;
		pioDesignMixGroup->dwPChannelStart = dwPChStart;
		pioDesignMixGroup->dwPChannelSpan = dwPChannelSpan;
		pioDesignMixGroup->strMixGroupName = strMixGroupName;
		pioPortConfig->lstDesignMixGroup.AddTail( pioDesignMixGroup );
	}
}

HRESULT ItemInfo::ConvertToDMusic( CTypedPtrList< CPtrList, ioPortConfig*> &lstPortConfigs,
		CTypedPtrList< CPtrList, ioDSoundBuffer*> &lstDSoundBuffers ) const
{
	UNREFERENCED_PARAMETER(lstDSoundBuffers);

	if( nNumPChannels > 0 )
	{
		long lIndex = 0;
		while( lIndex < nNumPChannels )
		{
			const DWORD dwPChStart = adwPChannels[lIndex];
			DWORD dwPChCur = dwPChStart + 1;
			lIndex++;

			while( (lIndex < nNumPChannels) && (adwPChannels[lIndex] == dwPChCur) )
			{
				lIndex++;
				dwPChCur++;
			}

			CreateSynthAndBuffers( lstPortConfigs, pPortOptions, dwPChStart, dwPChCur - dwPChStart, strBandName, lstBuffers, lstDSoundBuffers );
		}
	}
	else
	{
		CreateSynthAndBuffers( lstPortConfigs, pPortOptions, 0, 0, strBandName, lstBuffers, lstDSoundBuffers );
	}

	return S_OK;
}

bool ItemInfo::IsAnythingSelected( void )
{
	POSITION posBuffer = lstBuffers.GetHeadPosition();
	while( posBuffer )
	{
		BufferOptions *pBufferOptions = lstBuffers.GetNext( posBuffer );
		if( pBufferOptions->fSelected )
		{
			return true;
		}

		POSITION posEffect = pBufferOptions->lstEffects.GetHeadPosition();
		while( posEffect )
		{
			BusEffectInfo *pBusEffectInfo = pBufferOptions->lstEffects.GetNext( posEffect );
			if( pBusEffectInfo->m_beiSelection != BEI_NONE )
			{
				return true;
			}
		}
	}

	return false;
}

HRESULT PortOptions::Write( IDMUSProdRIFFStream *pIRiffStream ) const
{
	if( !pIRiffStream )
	{
		return E_POINTER;
	}

	IStream *pStream = pIRiffStream->GetStream();
	if( !pStream )
	{
		return E_POINTER;
	}

	// Save the item in a LIST chunk
	MMCKINFO ckPort;
	ckPort.fccType = DMUSPROD_FOURCC_PORTSETTINGS_LIST;
	if( 0 == pIRiffStream->CreateChunk( &ckPort, MMIO_CREATELIST ) )
	{
		MMCKINFO ckSubChunk;
		// Save the name in a UNAM chunk
		if( m_strName.GetLength() )
		{
			ckSubChunk.ckid = DMUS_FOURCC_UNAM_CHUNK;
			if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, 0 ) )
			{
				SaveMBStoWCS( pStream, &m_strName );
				pIRiffStream->Ascend( &ckSubChunk, 0 );
			}
		}

		// Save the port settings in an PORTSETTINGS_CHUNK chunk
		ckSubChunk.ckid = DMUSPROD_FOURCC_PORTSETTINGS_CHUNK;
		if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, 0 ) )
		{
			ioPortOptions oPortOptions;
			oPortOptions.guidPort = m_guidPort;
			oPortOptions.dwSampleRate = m_dwSampleRate;
			oPortOptions.dwVoices = m_dwVoices;
			oPortOptions.dwMaxVoices = m_dwMaxVoices;
			oPortOptions.dwEffects = m_dwEffects;
			oPortOptions.dwSupportedEffects = m_dwSupportedEffects;
			oPortOptions.fAudioPath = m_fAudioPath;
			pStream->Write( &oPortOptions, sizeof(ioPortOptions), NULL );
			pIRiffStream->Ascend( &ckSubChunk, 0 );
		}

		pIRiffStream->Ascend( &ckPort, 0 );
	}

	pStream->Release();

	return S_OK;
}

HRESULT PortOptions::Read( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO *pckParent )
{
	if( !pIRiffStream || !pckParent )
	{
		return E_POINTER;
	}

	// Get a pointer to the IStream
	IStream *pStream = pIRiffStream->GetStream();
	if( !pStream )
	{
		return E_POINTER;
	}

	// Initialize ourself to a clean state
	Empty();

	// Initialize the result to S_OK
	HRESULT hr = S_OK;

	// Descend into all chunks within pckParent
	MMCKINFO ckChunk;
	DWORD cbRead;
	while( SUCCEEDED(hr)
		&& (0 == pIRiffStream->Descend( &ckChunk, pckParent, 0 )) )
	{
		switch( ckChunk.ckid )
		{
		case DMUS_FOURCC_UNAM_CHUNK:
			// Read in the custom name
			ReadMBSfromWCS( pStream, ckChunk.cksize, &m_strName );
			break;

		case DMUSPROD_FOURCC_PORTSETTINGS_CHUNK:
			// Read in the port settings
			{
				// Initialize the ioPortOptions structre
				ioPortOptions iPortOptions;

				// Calculate how much of the structure to read
				DWORD cbToRead = min( ckChunk.cksize, sizeof(ioPortOptions) );

				// Read in the structure
				hr = pStream->Read( &iPortOptions, sizeof(ioPortOptions), &cbRead );

				// If we didn't read in enough data, set hr to a failure code
				if( SUCCEEDED(hr)
				&&	(cbRead != cbToRead) )
				{
					hr = E_FAIL;
				}

				// If the read succeeded
				if( SUCCEEDED(hr) )
				{
					// Copy the data to ourself
					m_guidPort = iPortOptions.guidPort;
					m_dwSampleRate = iPortOptions.dwSampleRate;
					m_dwVoices = iPortOptions.dwVoices;
					m_dwMaxVoices = iPortOptions.dwMaxVoices;
					m_dwEffects = iPortOptions.dwEffects;
					m_dwSupportedEffects = iPortOptions.dwSupportedEffects;
					m_fAudioPath = iPortOptions.fAudioPath ? true : false;
				}
			}
			break;
		}

		// If the name wasn't read in, get it from the port's GUID
		if( m_strName.IsEmpty() )
		{
			m_strName = PortNameFromGUID( m_guidPort );
		}

		pIRiffStream->Ascend( &ckChunk, 0 );
	}

	// Release the IStream
	pStream->Release();

	// Return the result
	return hr;
}

void PortOptions::Empty( void )
{
	m_guidPort = GUID_Synth_Default;
	m_dwEffects = 0;
	m_dwSampleRate = 22050;
	m_dwSupportedEffects = 0;
	m_dwVoices = DEFAULT_NUM_VOICES;
	m_dwMaxVoices = DEFAULT_NUM_VOICES;
	m_fAudioPath = true;
	m_strName.Empty();
}

void PortOptions::Copy( const PortOptions *pPortOptions )
{
	if( pPortOptions == NULL )
	{
		Empty();
	}
	else
	{
		m_strName = pPortOptions->m_strName;
		m_guidPort = pPortOptions->m_guidPort;
		m_dwEffects = pPortOptions->m_dwEffects;
		m_dwSupportedEffects = pPortOptions->m_dwSupportedEffects;
		m_dwSampleRate = pPortOptions->m_dwSampleRate;
		m_dwVoices = pPortOptions->m_dwVoices;
		m_dwMaxVoices = pPortOptions->m_dwMaxVoices;
		m_fAudioPath = pPortOptions->m_fAudioPath;
	}
}

bool PortOptions::IsEqual( const PortOptions *pPortOptions ) const
{
	// m_dwSupportedEffects, m_dwMaxVoices, and m_fAudioPath are non-editable parameters,
	// and solely depend on the port's GUID.
	if( (pPortOptions == NULL)
	||	(m_dwEffects != pPortOptions->m_dwEffects)
//	||	(m_dwSupportedEffects != pPortOptions->m_dwSupportedEffects)
	||	(m_dwSampleRate != pPortOptions->m_dwSampleRate)
	||	(m_dwVoices != pPortOptions->m_dwVoices)
//	||	(m_dwMaxVoices != pPortOptions->m_dwMaxVoices)
//	||	(m_fAudioPath != pPortOptions->m_fAudioPath)
	||	(m_guidPort != pPortOptions->m_guidPort)
	||	(m_strName != pPortOptions->m_strName) )
	{
		return false;
	}

	return true;
}

void InitializeDS3DBUFFER( DS3DBUFFER *pDS3DBUFFER )
{
	ASSERT( pDS3DBUFFER );
	ZeroMemory( pDS3DBUFFER, sizeof( DS3DBUFFER ) );
	pDS3DBUFFER->dwSize = sizeof( DS3DBUFFER );
	pDS3DBUFFER->dwInsideConeAngle = DS3D_DEFAULTCONEANGLE;
	pDS3DBUFFER->dwOutsideConeAngle = DS3D_DEFAULTCONEANGLE;
	pDS3DBUFFER->dwMode = DS3DMODE_NORMAL;
	pDS3DBUFFER->flMaxDistance = DS3D_DEFAULTMAXDISTANCE;
	pDS3DBUFFER->flMinDistance = DS3D_DEFAULTMINDISTANCE;
	pDS3DBUFFER->lConeOutsideVolume = DS3D_DEFAULTCONEOUTSIDEVOLUME;
	pDS3DBUFFER->vConeOrientation.z = 1.0;
	//pDS3DBUFFER->vPosition = 0;
	//pDS3DBUFFER->vVelocity = 0;
}
