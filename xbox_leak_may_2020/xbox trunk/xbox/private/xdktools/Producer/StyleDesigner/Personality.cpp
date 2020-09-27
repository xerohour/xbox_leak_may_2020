// Personality.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"
#include "Personality.h"
#include "RiffStrm.h"
#include <dmusicf.h>
#include "RiffStructs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPersonality constructor/destructor

CPersonality::CPersonality()
{
	m_fDefault = FALSE;

    memset( &m_guid, 0, sizeof( GUID ) );
}

CPersonality::~CPersonality()
{
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality::DM_SavePersonalityRefChunk

HRESULT CPersonality::DM_SavePersonalityRefChunk( IDMUSProdRIFFStream* pIRiffStream )
{
    IStream* pIStream;
    HRESULT hr = S_OK;
    MMCKINFO ckMain;
    MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( pIRiffStream == NULL )
	{
		hr = E_INVALIDARG;
		goto ON_ERROR;
	}

	// Write REF LIST header
	ckMain.fccType = DMUS_FOURCC_REF_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write REF chunk
	{
		DMUS_IO_REFERENCE dmusReference;

		ck.ckid = DMUS_FOURCC_REF_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare DMUS_IO_REFERENCE structure
		memset( &dmusReference, 0, sizeof(DMUS_IO_REFERENCE) );
		dmusReference.guidClassID = CLSID_DirectMusicChordMap;
		dmusReference.dwValidData = (DMUS_OBJ_CLASS | DMUS_OBJ_NAME | DMUS_OBJ_FILENAME);

		// Write REF chunk data 
		hr = pIStream->Write( &dmusReference, sizeof(DMUS_IO_REFERENCE), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(DMUS_IO_REFERENCE) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Name chunk
	{
		ck.ckid = DMUS_FOURCC_NAME_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strName );
		if( FAILED( hr ) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Filename chunk
	{
		ck.ckid = DMUS_FOURCC_FILE_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strFileName );
		if( FAILED( hr ) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality::DM_LoadPersonalityRef

HRESULT CPersonality::DM_LoadPersonalityRef( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pck )
{
    IStream* pIStream;
    HRESULT hr;
	MMCKINFO ck;
    DWORD dwSize;
    DWORD dwByteCount;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	while( pIRiffStream->Descend( &ck, pck, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case DMUS_FOURCC_REF_CHUNK:
			{
				DMUS_IO_REFERENCE dmusReference;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_REFERENCE ) );
				hr = pIStream->Read( &dmusReference, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Nothing to store at this time.....
				break;
			}

			case DMUS_FOURCC_NAME_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &m_strName );
				break;

			case DMUS_FOURCC_FILE_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &m_strFileName );
				break;
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
    pIStream->Release();

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CPersonality::IMA25_LoadPersonalityRef

HRESULT CPersonality::IMA25_LoadPersonalityRef( IStream* pIStream, MMCKINFO* pck )
{
    ioPersonalityRef iPersonalityRef;
    DWORD dwSize;
    WORD wStructSize;

    ASSERT( pIStream != NULL );

	// Read size of ioPersonalityRef structure
    dwSize = pck->cksize;
    pIStream->Read( &wStructSize, sizeof( wStructSize ), NULL );
    dwSize -= sizeof( wStructSize );

	// Read ioPersonalityRef structure
    if( wStructSize > sizeof(iPersonalityRef) )
    {
        pIStream->Read( &iPersonalityRef, sizeof(iPersonalityRef), NULL );
        StreamSeek( pIStream, wStructSize - sizeof(iPersonalityRef), STREAM_SEEK_CUR );
    }
    else
    {
        pIStream->Read( &iPersonalityRef, wStructSize, NULL );
    }
    dwSize -= wStructSize;

    m_strName = iPersonalityRef.wstrName;
    m_fDefault = iPersonalityRef.fDefault;
	memcpy( &m_guid, &iPersonalityRef.guid, sizeof( m_guid ) );

	// Read PersonalityRef filename (unicode format)
	if( dwSize > 0 )
	{
		ReadMBSfromWCS( pIStream, dwSize, &m_strFileName );
		m_strFileName += _T(".PER");
	}

    return S_OK;
}
