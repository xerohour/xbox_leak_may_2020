// EffectInfo.cpp : implementation file
//

#include "stdafx.h"
#include "EffectInfo.h"
#include "RiffStrm.h"
#include "ItemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BUFFER_SIZE 256

bool StreamsAreEqual( IStream *pIStream1, IStream *pIStream2 )
{
	// Check if stream pointers are equal (including both NULL)
	if( pIStream1 == pIStream2 )
	{
		return true;
	}

	// Check if either stream pointer is NULL
	if( (pIStream1 == NULL)
	||	(pIStream2 == NULL) )
	{
		return false;
	}

	// Get the length of each stream
	STATSTG statStg1, statStg2;
	if( FAILED( pIStream1->Stat( &statStg1, STATFLAG_NONAME ) )
	||	FAILED( pIStream2->Stat( &statStg2, STATFLAG_NONAME ) )
	||	(statStg1.cbSize.QuadPart != statStg2.cbSize.QuadPart) )
	{
		// If either call to Stat failed, or the length are different, return false
		return false;
	}

	// Seek back to the start of each stream
	if( FAILED( StreamSeek( pIStream1, 0, STREAM_SEEK_SET ) )
	||	FAILED( StreamSeek( pIStream2, 0, STREAM_SEEK_SET ) ) )
	{
		// If either seek failed, return false
		return false;
	}

	// Try and allocate buffers to read data into
	BYTE *pbData1 = new BYTE[BUFFER_SIZE];
	if( pbData1 == NULL )
	{
		return false;
	}
	BYTE *pbData2 = new BYTE[BUFFER_SIZE];
	if( pbData2 == NULL )
	{
		delete []pbData1;
		return false;
	}

	// Initialize the amount of data to compare
	long lStreamSize = min( statStg1.cbSize.LowPart, LONG_MAX );

	while( lStreamSize > 0 )
	{
		// Try and read in a chunk of data
		ULONG cbRead1, cbRead2;
		const ULONG lReadSize = min( lStreamSize, BUFFER_SIZE );
		if( FAILED( pIStream1->Read( pbData1, lReadSize, &cbRead1 ) )
		||	FAILED( pIStream2->Read( pbData2, lReadSize, &cbRead2 ) )
		||	(lReadSize != cbRead1)
		||	(lReadSize != cbRead2) )
		{
			// If either read failed, or we didn't read in
			// the correct amount of data, return false
			break;
		}

		// Check if the data we read is identical
		if( memcmp( pbData1, pbData2, lReadSize ) != 0 )
		{
			// Nope
			break;
		}

		lStreamSize -= lReadSize;
	}

	delete []pbData1;
	delete []pbData2;

	// If we read all the data, return true.  Otherwise, return false
	return lStreamSize <= 0;
}

/////////////////////////////////////////////////////////////////////////////
// EffectInfo

EffectInfo::EffectInfo()
{
	m_pIStream = NULL;
	Empty();
}

EffectInfo::EffectInfo( CString strNewName, CString strNewInstanceName, REFCLSID rclsidNewObject, REFCLSID rclsidSendBuffer )
{
	m_pIStream = NULL;
	Empty();
	m_strName = strNewName;
	m_strInstanceName = strNewInstanceName;
	m_clsidObject = rclsidNewObject;
	m_clsidSendBuffer = rclsidSendBuffer;
}

void EffectInfo::Empty( void )
{
	ZeroMemory( &m_clsidObject, sizeof(CLSID) );
	ZeroMemory( &m_clsidSendBuffer, sizeof(CLSID) );

	if( m_pIStream )
	{
		m_pIStream->Release();
		m_pIStream= NULL;
	}
	m_dwFlags = 0;
	m_strInstanceName.Empty();
	m_strName.Empty();
	m_pSendDestinationMixGroup = NULL;
}

EffectInfo::~EffectInfo()
{
	if( m_pIStream )
	{
		m_pIStream->Release();
		m_pIStream = NULL;
	}
}

void EffectInfo::Copy( const EffectInfo &effectInfo )
{
	if( m_pIStream )
	{
		m_pIStream->Release();
		m_pIStream = NULL;
	}

	m_strInstanceName = effectInfo.m_strInstanceName;
	m_strName = effectInfo.m_strName;
	m_clsidObject = effectInfo.m_clsidObject;
	m_clsidSendBuffer = effectInfo.m_clsidSendBuffer;
	m_dwFlags = effectInfo.m_dwFlags;
	m_pSendDestinationMixGroup = effectInfo.m_pSendDestinationMixGroup;

	if( effectInfo.m_pIStream )
	{
		effectInfo.m_pIStream->Clone( &m_pIStream );
	}
}

void EffectInfo::Copy( const ioDSoundEffect *pioDSoundEffect )
{
	// Validate pioDSoundEffect
	ASSERT( pioDSoundEffect );
	if( NULL == pioDSoundEffect )
	{
		return;
	}

	// Remove our existing stream pointer
	if( m_pIStream )
	{
		m_pIStream->Release();
		m_pIStream = NULL;
	}

	// Copy the effect's data stream, if available
	if( pioDSoundEffect->pStreamData )
	{
		pioDSoundEffect->pStreamData->Clone( &m_pIStream );
	}

	// Set the effect's flags
	m_dwFlags = pioDSoundEffect->ioFXHeader.dwEffectFlags;

	// Clear our pointer to the dest. mix group
	m_pSendDestinationMixGroup = NULL;

	// Copy the rest of the effect's information
	m_strName = pioDSoundEffect->strEffectName;
	m_strInstanceName = pioDSoundEffect->strInstanceName;
	m_clsidObject = pioDSoundEffect->ioFXHeader.guidDSFXClass;
	m_clsidSendBuffer = pioDSoundEffect->ioFXHeader.guidSendBuffer;
}

bool EffectInfo::IsEqualExceptName( const EffectInfo *pEffectInfo ) const
{
	if( (m_dwFlags != pEffectInfo->m_dwFlags)
	||	(m_clsidObject != pEffectInfo->m_clsidObject)
	||	(m_clsidSendBuffer != pEffectInfo->m_clsidSendBuffer)
	||	!StreamsAreEqual( m_pIStream, pEffectInfo->m_pIStream ) )
	{
		return false;
	}
	return true;
}

HRESULT EffectInfo::Write( IDMUSProdRIFFStream *pIRiffStream ) const
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

	// Save the effect in a LIST chunk
	MMCKINFO ckEffect;
	ckEffect.fccType = DMUSPROD_FOURCC_EFFECTINFO_LIST;
	if( 0 == pIRiffStream->CreateChunk( &ckEffect, MMIO_CREATELIST ) )
	{
		// Save the instance name in a UNAM chunk
		MMCKINFO ckSubChunk;
		ckSubChunk.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, 0 ) )
		{
			SaveMBStoWCS( pStream, &m_strInstanceName );
			pIRiffStream->Ascend( &ckSubChunk, 0 );
		}

		// Save the normal name in a EFFECTINFO_NAME chunk
		ckSubChunk.ckid = DMUSPROD_FOURCC_EFFECTINFO_NAME;
		if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, 0 ) )
		{
			SaveMBStoWCS( pStream, &m_strName );
			pIRiffStream->Ascend( &ckSubChunk, 0 );
		}

		// Save the CLSID in an EFFECTINFO_HEADER chunk
		ckSubChunk.ckid = DMUSPROD_FOURCC_EFFECTINFO_HEADER;
		if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, 0 ) )
		{
			pStream->Write( &m_clsidObject, sizeof(CLSID), NULL );
			pStream->Write( &m_clsidSendBuffer, sizeof(CLSID), NULL );
			pStream->Write( &m_dwFlags, sizeof(DWORD), NULL );
			pIRiffStream->Ascend( &ckSubChunk, 0 );
		}

		// If there is any custom data, save it in an EFFECTINFO_DATA chunk
		if( m_pIStream )
		{
			STATSTG statStg;
			if( SUCCEEDED( m_pIStream->Stat( &statStg, STATFLAG_NONAME ) )
			&&	(statStg.cbSize.QuadPart > 0)  )
			{
				// Seek back to the start of the stream
				if( SUCCEEDED( StreamSeek( m_pIStream, 0 , STREAM_SEEK_SET ) ) )
				{
					ckSubChunk.ckid = DMUSPROD_FOURCC_EFFECTINFO_DATA;
					if( 0 == pIRiffStream->CreateChunk( &ckSubChunk, 0 ) )
					{
						ULARGE_INTEGER cbRead, cbWritten;
						m_pIStream->CopyTo( pStream, statStg.cbSize, &cbRead, &cbWritten );
						if( (statStg.cbSize.QuadPart != cbRead.QuadPart)
						||	(statStg.cbSize.QuadPart != cbWritten.QuadPart) )
						{
							ASSERT(FALSE);
						}

						pIRiffStream->Ascend( &ckSubChunk, 0 );
					}
				}
			}
		}

		pIRiffStream->Ascend( &ckEffect, 0 );
	}

	pStream->Release();

	return S_OK;
}

HRESULT EffectInfo::Read( IStream *pIStream )
{
	if( !pIStream )
	{
		return E_POINTER;
	}

	// Allocate a RIFF stream to use to parse this stream
	IDMUSProdRIFFStream *pRiffStream = NULL;
	AllocRIFFStream( pIStream, &pRiffStream );
	if( !pRiffStream )
	{
		return E_POINTER;
	}

	// Initialize ourself to a clean state
	Empty();

	// Initialize our result code
	HRESULT hr = E_FAIL;

	// Descend into the main EffectInfo LIST chunk
	MMCKINFO ckMain;
	ckMain.fccType = DMUSPROD_FOURCC_EFFECTINFO_LIST;
    if( pRiffStream->Descend( &ckMain, NULL, MMIO_FINDLIST ) == 0 )
	{
		// Now, descend into each of the subchunks
		MMCKINFO ck;
		while( pRiffStream->Descend( &ck, &ckMain, 0 ) == 0 )
		{
			switch( ck.ckid )
			{
			case DMUS_FOURCC_UNAM_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &m_strInstanceName );
				break;
			case DMUSPROD_FOURCC_EFFECTINFO_NAME:
				ReadMBSfromWCS( pIStream, ck.cksize, &m_strName );
				break;
			case DMUSPROD_FOURCC_EFFECTINFO_HEADER:
				if( ck.cksize >= sizeof(CLSID) )
				{
					pIStream->Read( &m_clsidObject, sizeof(CLSID), NULL );
					if( ck.cksize >= sizeof(CLSID) * 2 )
					{
						pIStream->Read( &m_clsidSendBuffer, sizeof(CLSID), NULL );
						if( ck.cksize >= sizeof(CLSID) * 2 + sizeof(DWORD) )
						{
							pIStream->Read( &m_dwFlags, sizeof(DWORD), NULL );
						}
					}
					hr = S_OK;
				}
				break;
			case DMUSPROD_FOURCC_EFFECTINFO_DATA:
				// Read in the effect's data

				// Delete any existing data
				if( m_pIStream )
				{
					m_pIStream->Release();
					m_pIStream = NULL;
				}

				// Allocate a stream for the new data
				if( SUCCEEDED( ::CreateStreamOnHGlobal( NULL, TRUE, &m_pIStream ) ) )
				{
					// Read in the new data
					ULARGE_INTEGER cbWrite, cbRead, cbWritten;
					cbWrite.QuadPart = ck.cksize;
					pIStream->CopyTo( m_pIStream, cbWrite, &cbRead, &cbWritten );
					ASSERT( cbWrite.QuadPart == cbRead.QuadPart );
					ASSERT( cbWrite.QuadPart == cbWritten.QuadPart );
				}
				break;
			}
			pRiffStream->Ascend( &ck, 0 );
		}
		pRiffStream->Ascend( &ckMain, 0 );
	}

	// Release the RIFF stream
	pRiffStream->Release();

	return hr;
}

HRESULT EffectInfo::Read( IDMUSProdRIFFStream *pIRiffStream, MMCKINFO *pckParent )
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
			// Read in the effect's instance name
			ReadMBSfromWCS( pStream, ckChunk.cksize, &m_strInstanceName );
			break;

		case DMUSPROD_FOURCC_EFFECTINFO_NAME:
			// Read in the effect's standard name
			ReadMBSfromWCS( pStream, ckChunk.cksize, &m_strName );
			break;

		case DMUSPROD_FOURCC_EFFECTINFO_HEADER:
			// Read in the effect's CLSID
			if( ckChunk.cksize >= sizeof(CLSID) )
			{
				hr = pStream->Read( &m_clsidObject, sizeof(CLSID), &cbRead );

				// If we didn't read in the correct amount, set hr to a failure code
				if( SUCCEEDED(hr)
				&&	(cbRead != sizeof(CLSID)) )
				{
					hr = E_FAIL;
				}

				// Read in the effect's send CLSID
				if( ckChunk.cksize >= sizeof(CLSID) * 2 )
				{
					hr = pStream->Read( &m_clsidSendBuffer, sizeof(CLSID), &cbRead );

					// If we didn't read in the correct amount, set hr to a failure code
					if( SUCCEEDED(hr)
					&&	(cbRead != sizeof(CLSID)) )
					{
						hr = E_FAIL;
					}

					// Read in the effect's flags
					if( ckChunk.cksize >= sizeof(CLSID) * 2 + sizeof(DWORD) )
					{
						hr = pStream->Read( &m_dwFlags, sizeof(DWORD), &cbRead );

						// If we didn't read in the correct amount, set hr to a failure code
						if( SUCCEEDED(hr)
						&&	(cbRead != sizeof(DWORD)) )
						{
							hr = E_FAIL;
						}
					}
				}
			}
			else
			{
				hr = E_FAIL;
			}
			break;

		case DMUSPROD_FOURCC_EFFECTINFO_DATA:
			// Read in the effect's data

			// Delete any existing data
			if( m_pIStream )
			{
				m_pIStream->Release();
				m_pIStream = NULL;
			}

			// Allocate a stream for the new data
			if( SUCCEEDED( ::CreateStreamOnHGlobal( NULL, TRUE, &m_pIStream ) ) )
			{
				// Read in the new data
				ULARGE_INTEGER cbWrite, cbRead, cbWritten;
				cbWrite.QuadPart = ckChunk.cksize;
				hr = pStream->CopyTo( m_pIStream, cbWrite, &cbRead, &cbWritten );

				// If we didn't read in the correct amount, set hr to a failure code
				if( SUCCEEDED(hr)
				&&	((cbWrite.QuadPart != cbRead.QuadPart) ||
					 (cbWrite.QuadPart != cbWritten.QuadPart)) )
				{
					hr = E_FAIL;
				}

				// If the read failed
				if( FAILED(hr) )
				{
					// Delete the new data
					m_pIStream->Release();
					m_pIStream = NULL;
				}
			}
			break;
		}

		// Ascend out of the child chunks
		pIRiffStream->Ascend( &ckChunk, 0 );
	}

	// Release the IStream
	pStream->Release();

	// Return the result
	return hr;
}
