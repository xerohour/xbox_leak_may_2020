#include <dsoundtest.h>

CDSoundTest::CDSoundTest( void )
	: m_pDSound( NULL ),
	  m_pDSoundBuffer( NULL ), 
	  m_pds3dl( NULL ),
	  m_pds3db( NULL ),
	  m_pdsCaps( NULL ),
	  m_pDsbd( NULL ),
	  m_pWaveFormat( NULL ),
	  m_pWaveFile( NULL ),
	  m_pad3dVal( NULL ),
	  m_pControllingUnknown( NULL ),
	  m_pvLock1( NULL ),
	  m_pvLock2( NULL ),
	  m_pvBufferData( NULL ),
	  m_paNotifies( NULL ),
	  m_rtTimeStamp( 0 ),
#ifdef SILVER
	  m_pdscv( NULL ),
	  m_dwDeviceId( DSDEVID_MCPX ),
#endif SILVER
	  m_dwDuration( 0 ),
	  m_dwBufferSize( 0 ),
	  m_dwBufferFlags( 0 ),
	  m_dwLockSize1( 0 ),
	  m_dwLockSize2( 0 ),
	  m_dwLockFlags( 0 ),
	  m_dwWriteCursor( 0 ),
	  m_dwWriteBytes( 0 ),
	  m_dwReadPosition( 0 ),
      m_dwLoopStart( 0 ),
      m_dwLoopLength( 0 ),
      m_dwSleepTime( PLAY_TO_COMPLETION ),
	  m_dwBufferStatus( 0 ),
	  m_dwPlayCursor( 0 ),
	  m_dwFrequency( 0 ),
	  m_dwChannelCount( 0 ),
	  m_dwStatus( DSBSTATUS_PLAYING ),
	  m_dwApply( 0 ),
	  m_dwInsideConeAngle( 0 ),
	  m_dwOutsideConeAngle( 0 ),
	  m_dwMode( 0 ),
	  m_dwNumBuffers( NUM_DEFAULT_BUFFERS ),
	  m_dwPlayFlags( 0 ),
	  m_dwReserved1( 0 ),
	  m_dwReserved2( 0 ),
	  m_dwBufferBytes( 0 ),
	  m_dwSpeakerConfig( 0 ),
	  m_dwNotifyCount( 0 ),
	  m_dwFlags( 0 ),
	  m_lConeOutsideVolume( 0 ),
	  m_lVolume( 0 ),
	  m_fBufferDuration( CREATE_COMPLETE_BUFFER ),
	  m_bStress( false )
{
	// NO CODE HERE
}

CDSoundTest::~CDSoundTest( void )
{
	Close();
	DestroyDSound();
}

HRESULT CDSoundTest::ReAllocate( void )
{
	HRESULT hr = S_OK;

	Close();

	ALLOCATEANDCHECKSTRUCT( m_pWaveFormat, WAVEFORMATEX );
	ALLOCATEANDCHECKSTRUCT( m_pDsbd, DSBUFFERDESC );
	ALLOCATEANDCHECKSTRUCT( m_pdsCaps, DSCAPS );
	ALLOCATEANDCHECKSTRUCT( m_pds3dl, DS3DLISTENER );
	ALLOCATEANDCHECKSTRUCT( m_pds3db, DS3DBUFFER );
#ifdef    SILVER
	ALLOCATEANDCHECKSTRUCT( m_pdscv, DSCHANNELVOLUME );
#endif // SILVER
	ALLOCATEANDCHECK( m_pWaveFile, CWaveFile );

	if ( FAILED( hr ) ) 
	{
		Close();
	}

	return hr;
}

HRESULT CDSoundTest::CheckMemory( void )
{
	LPBYTE pBuffer = new BYTE[m_dwBufferSize * BUFFER_ALLOC_PAD];
	HRESULT hr = NULL == pBuffer ? E_OUTOFMEMORY : S_OK;

	delete [] pBuffer;

	return hr;
}	

HRESULT CDSoundTest::OpenWaveFile( LPCSTR szFileName )
{
	HRESULT hr = S_OK;

	if ( NULL == m_pWaveFile )
	{
		hr = E_UNEXPECTED;
	}

	if ( SUCCEEDED( hr ) )
	{
		hr = m_pWaveFile->Open( szFileName );
	}

    if ( SUCCEEDED( hr ) )
    {   
        hr = m_pWaveFile->GetFormat( m_pWaveFormat, sizeof( WAVEFORMATEX ), NULL );
    }

    if ( SUCCEEDED( hr ) )
    {   
        hr = m_pWaveFile->GetDuration( &m_dwDuration );
    }

	if ( SUCCEEDED( hr ) )
	{
		m_dwBufferSize = CreateCompleteBufferSet() ? m_dwDuration : CalculateBufferSize();

		if ( true == m_bStress )
		{
			hr = CheckMemory();
		}
	}

	return hr;
}

HRESULT CDSoundTest::PopulateDSBD( void )
{
	HRESULT hr = S_OK;

	if ( NULL == m_pDsbd )
	{
		hr = E_UNEXPECTED;
	}

	if ( SUCCEEDED( hr ) )
	{
		memset( m_pDsbd, 0, sizeof( DSBUFFERDESC ) );
			
		m_pDsbd->dwSize = sizeof( DSBUFFERDESC );
		m_pDsbd->dwFlags = m_dwBufferFlags;
	
		m_pDsbd->dwBufferBytes = 0 == m_pWaveFormat->nBlockAlign ? m_dwBufferSize : m_dwBufferSize - ( m_dwBufferSize % m_pWaveFormat->nBlockAlign );
		m_pDsbd->lpwfxFormat = m_pWaveFormat;

		m_dwWriteBytes = m_pDsbd->dwBufferBytes;
	}

	return hr;
}

// create a sound buffer using the DSOUND api

HRESULT CDSoundTest::CreateBuffer( void ) 
{
	HRESULT hr = S_OK;

	if ( NULL != m_pDSound )
	{
		return CreateSoundBuffer();
	}
	
	hr = PopulateDSBD();

	if ( SUCCEEDED( hr ) )
	{	
#ifdef    SILVER
		hr = DirectSoundCreateBuffer( m_dwDeviceId, m_pDsbd, &m_pDSoundBuffer, m_pControllingUnknown );
#else  // SILVER
		hr = DirectSoundCreateBuffer( m_pDsbd, &m_pDSoundBuffer );
#endif // SILVER
	}
	
	return hr;
}

// create the dsound object

HRESULT CDSoundTest::CreateDSound( void )
{
#ifdef    SILVER
	return DirectSoundCreate( m_dwDeviceId, &m_pDSound, m_pControllingUnknown );
#else  // SILVER
	return DirectSoundCreate( NULL, &m_pDSound, m_pControllingUnknown );
#endif // SILVER
}

// create a sound buffer using the existing DSound object

HRESULT CDSoundTest::CreateSoundBuffer( void )
{
	HRESULT hr = S_OK;

	if ( NULL == m_pDSound )
	{
		hr = E_UNEXPECTED;
	}

	if ( SUCCEEDED( hr ) )
	{
		hr = PopulateDSBD();
	}

	if ( SUCCEEDED( hr ) )
	{
		hr = m_pDSound->CreateSoundBuffer( m_pDsbd, &m_pDSoundBuffer, m_pControllingUnknown );
	}

	return hr;
}

HRESULT CDSoundTest::PopulateBuffer( void )
{
	HRESULT hr = S_OK;
	DWORD dwAmount = 0;

	if ( NULL == m_pWaveFile )
	{
		hr = E_UNEXPECTED;
	}

	if ( SUCCEEDED( hr ) )
	{
		hr = Lock();
	}

    if ( SUCCEEDED( hr ) )
    {
        hr = m_pWaveFile->ReadSample( m_dwReadPosition, m_pvLock1, m_dwLockSize1, &dwAmount );
    }

    if ( SUCCEEDED( hr ) )
	{
		m_dwReadPosition += dwAmount;

		if ( dwAmount != m_dwLockSize1 )
		{
			memset( (BYTE*) m_pvLock1 + dwAmount, 0, m_dwLockSize1 - dwAmount );
		}
    }

	return hr;
}

HRESULT CDSoundTest::Open( const LPCSTR szFileName )
{
	HRESULT hr = S_OK;

	if ( NULL == szFileName )
	{
		hr = E_INVALIDARG;
	}

	if ( SUCCEEDED( hr ) )
	{
		hr = ReAllocate();
    }

	if ( SUCCEEDED( hr ) )
	{
		hr = OpenWaveFile( szFileName );
	}

	if ( SUCCEEDED( hr ) )
	{
		hr = CreateBuffer();
	}

	if ( SUCCEEDED( hr ) )
	{
		hr = PopulateBuffer();
	}

    if ( FAILED( hr ) )
    {
        Close();
    }

	return hr;

}

HRESULT CDSoundTest::OpenRandomFile( LPCSTR szPath ) 
{
	HRESULT hr = S_OK;
	CHAR* szFile = NULL;

	if ( NULL == szPath )
	{
		hr = E_INVALIDARG;
	}

	if ( SUCCEEDED( hr ) )
	{
		szFile = new CHAR[ strlen( szPath ) + MAX_FILE_NAME_LEN ];

		if ( NULL == szFile )
		{
			hr = E_OUTOFMEMORY;
		}
	}

	if ( SUCCEEDED( hr ) ) 
	{
		sprintf( szFile, FILE_FORMAT_STRING, szPath, ( rand() % MAX_FILE_NUM ) + MIN_FILE_NUM );
		hr = Open( szFile );
	} 

	delete [] szFile;

	return hr;
}

HRESULT CDSoundTest::OpenAndPlay( LPCSTR szFileName )
{
	HRESULT hr = S_OK;

	hr = NULL == szFileName ? OpenRandomFile( MEDIA_DIR ) : Open( szFileName );

	if ( SUCCEEDED( hr ) )
	{
		hr = PlayAndWait();
	}

	return hr;
}

void CDSoundTest::DestroyDSound( void )
{
	RELEASE( m_pDSound );
}


void CDSoundTest::Close( void )
{
	RELEASE( m_pDSoundBuffer );

	SAFEDELETE( m_pDsbd );
	SAFEDELETE( m_pWaveFormat );
	SAFEDELETE( m_pWaveFile );
	SAFEDELETE( m_pdsCaps );
	SAFEDELETE( m_pds3dl );
	SAFEDELETE( m_pds3db );

	SetStatus( DSBSTATUS_PLAYING );
	
	m_dwDuration = 0;
	m_dwBufferSize = 0;
	m_dwReadPosition = 0;
}

DWORD CDSoundTest::CalculateSleepTime( void )
{

	if ( NULL == m_pWaveFormat )
	{
		return 0;
	}

	DWORD dw = (DWORD) ( (float) m_dwDuration * (float) BITS_PER_BYTE / ( (float) m_pWaveFormat->wBitsPerSample * (float) m_pWaveFormat->nSamplesPerSec * (float) m_pWaveFormat->nChannels ) * (float) MILLISEC_PER_SEC );

	return dw;
}

DWORD CDSoundTest::CalculateBufferSize( void )
{
	float fSleepTime = 0.0f;

	if ( NULL == m_pWaveFormat )
	{
		return 0;
	}

	fSleepTime = (float) CalculateSleepTime() / (float) MILLISEC_PER_SEC;

	if ( fSleepTime < m_fBufferDuration )
	{
		m_fBufferDuration = fSleepTime;
		return m_dwDuration;
	}

	DWORD dw = (DWORD) ( m_fBufferDuration * (float) m_pWaveFormat->wBitsPerSample * (float) m_pWaveFormat->nSamplesPerSec * (float) m_pWaveFormat->nChannels / (float) BITS_PER_BYTE );

	return dw;
}

HRESULT CDSoundTest::Wait( void )
{

	HRESULT hr = S_OK;

	DWORD dwLastWritePos = 0;
	DWORD dwAmount = 0;
	DWORD dwSmallBufferSize = m_dwBufferSize / m_dwNumBuffers;

	// polled playback

	while ( SUCCEEDED( hr ) && PlayPollingSet() && StillPlaying() )
    {	
        hr = GetStatus();
    }

	// complete buffer load

	if ( SUCCEEDED( hr ) && ! PlayPollingSet() && CreateCompleteBufferSet() )
	{
		Sleep( PlayToCompletionSet() ? CalculateSleepTime() : m_dwSleepTime );
	}

	// buffered playback

	while ( SUCCEEDED( hr ) && ! PlayPollingSet() && ! CreateCompleteBufferSet() && dwAmount <= m_dwReadPosition )
	{
		hr = GetCurrentPosition();

		if ( SUCCEEDED( hr ) )
		{
			if ( m_dwPlayCursor < dwLastWritePos )
			{
				
				m_dwWriteBytes = m_dwBufferSize - dwLastWritePos;
				m_dwWriteCursor = dwLastWritePos;
				PopulateBuffer();
				
				dwAmount += m_dwWriteBytes;

				if ( 0 != m_dwPlayCursor )
				{
					m_dwWriteBytes = m_dwPlayCursor;
					m_dwWriteCursor = 0;

					PopulateBuffer();

					dwAmount += m_dwWriteBytes;

				}

				dwLastWritePos = m_dwPlayCursor;
			}

			else if ( m_dwPlayCursor - dwLastWritePos >= dwSmallBufferSize )
			{
				m_dwWriteBytes = m_dwPlayCursor - dwLastWritePos;
				m_dwWriteCursor = dwLastWritePos;
				PopulateBuffer();

				dwAmount += m_dwWriteBytes;

				dwLastWritePos = m_dwPlayCursor;
			}
		}
	}

	return hr;
}

HRESULT CDSoundTest::PlayAndWait( void ) 
{
	HRESULT hr = S_OK;

	if ( NULL == m_pDSoundBuffer )
	{
		hr = E_UNEXPECTED;
	}	

	if ( SUCCEEDED( hr ) )
	{
		hr = Play();
	}

	if ( SUCCEEDED( hr ) )
	{
		hr = Wait();
	}

	return hr;
}

void CDSoundTest::SetWaveFormat( LPWAVEFORMATEX pWFX )
{
	if ( NULL == pWFX || NULL == m_pWaveFormat )
	{
		return;
	}

	memcpy( m_pWaveFormat, pWFX, sizeof( WAVEFORMATEX ) );

}

void CDSoundTest::SetDSCaps( LPDSCAPS pdsCaps )
{
	if ( NULL == pdsCaps || NULL == m_pdsCaps )
	{
		return;
	}

	memcpy( m_pdsCaps, pdsCaps, sizeof( DSCAPS ) );
}

void CDSoundTest::SetListener( LPDS3DLISTENER pListener )
{
	if ( NULL == pListener || NULL == m_pds3dl )
	{
		return;
	}

	memcpy( m_pds3dl, pListener, sizeof( DS3DLISTENER ) );
}


void CDSoundTest::SetBuffer( LPDS3DBUFFER pBuffer )
{
	if ( NULL == pBuffer || NULL == m_pds3db )
	{
		return;
	}

	memcpy( m_pds3db, pBuffer, sizeof( DS3DBUFFER ) );
}

