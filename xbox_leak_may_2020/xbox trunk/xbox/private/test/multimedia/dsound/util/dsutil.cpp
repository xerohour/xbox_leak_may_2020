/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	dsutil.cpp

Abstract:

	DSound Test Utilities

Author:

	Robert Heitkamp (robheit) 24-Oct-2001

Revision History:

	24-Oct-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes
//------------------------------------------------------------------------------
#include <dsutil.h>
#include <srcxmo.h>
#include <waveout.h>
#include <time.h>

//------------------------------------------------------------------------------
//	Constants
//------------------------------------------------------------------------------
static const double cPi	= 3.1415926535;

//------------------------------------------------------------------------------
//	Reboot Code
//------------------------------------------------------------------------------
typedef enum _FIRMWARE_REENTRY 
{
    HalHaltRoutine,
    HalRebootRoutine,
    HalQuickRebootRoutine,
    HalKdRebootRoutine,
    HalMaximumRoutine
} FIRMWARE_REENTRY, *PFIRMWARE_REENTRY;

extern "C" VOID HalReturnToFirmware(IN FIRMWARE_REENTRY Routine);

//------------------------------------------------------------------------------
//	PlaySequential
//------------------------------------------------------------------------------
HRESULT 
PlaySequential( const LPCSTR* pszFileNames, ULONG ulNumFiles, DWORD dwSleepTime )
{
	HRESULT hr = S_OK;
	CDSoundTest testFile;

	if ( 0 == ulNumFiles && NULL == pszFileNames )
	{
		hr = E_INVALIDARG;
	}

	if ( SUCCEEDED( hr ) )
	{
		hr = testFile.CreateDSound();
	}

	if ( SUCCEEDED( hr ) )
	{
		testFile.SetSleepTime( dwSleepTime );
	}

	for ( ULONG ul = 0; ul < ulNumFiles && SUCCEEDED( hr ); ul++ ) 
	{
		xLog( g_hLog, XLL_INFO, "Opening: %s", pszFileNames[ul] );
		EXECUTE( testFile.OpenAndPlay( NULL == pszFileNames ? NULL : pszFileNames[ul] ) );
	}

	return hr;
}

//------------------------------------------------------------------------------
//	PlaySimultaneous
//------------------------------------------------------------------------------
HRESULT 
PlaySimultaneous( const LPCSTR* pszFileNames, ULONG ulNumFiles, DWORD dwSleepTime, DWORD dwIncrementTime )
{

	HRESULT hr = S_OK;

	DWORD dwMaxTime = 0;
	DWORD dwCurTime = 0;

	ULONG i = 0;

	CDSoundTest* paTestFiles = new CDSoundTest[ulNumFiles];

	if ( NULL == paTestFiles )
	{
		hr = E_OUTOFMEMORY;
	}

	if ( SUCCEEDED( hr ) && 0 == ulNumFiles && NULL == pszFileNames )
	{
		hr = E_INVALIDARG;
	}

	for ( i = 0; i < ulNumFiles && SUCCEEDED( hr ); i++ ) 
	{
		paTestFiles[i].SetSleepTime( dwIncrementTime );
		paTestFiles[i].SetBufferDuration( DEFAULT_BUFFER_DURATION );
		paTestFiles[i].SetPlayFlags( DSBPLAY_LOOPING );
		xLog( g_hLog, XLL_INFO, "Opening: %s", pszFileNames[i] );
		hr = NULL != pszFileNames ? paTestFiles[i].Open( pszFileNames[i] ) : paTestFiles[i].OpenRandomFile( MEDIA_DIR );

		if ( SUCCEEDED( hr ) )
			hr = paTestFiles[i].Play();

		if  ( SUCCEEDED( hr ) )
		{
			dwCurTime = paTestFiles[i].CalculateSleepTime();

			if ( dwCurTime > dwMaxTime )
			{
				dwMaxTime = dwCurTime;
			}
		}		
	}

	if ( SUCCEEDED( hr ) )
	{
	//Sleep( ( PLAY_TO_COMPLETION == dwSleepTime ? dwMaxTime : dwSleepTime ) - ( dwMaxTime < dwIncrementTime ? 0 : dwIncrementTime ) );
		Sleep( 1000 );
	}

	delete [] paTestFiles;

	return hr;
}

//------------------------------------------------------------------------------
//	PlayOneFileStream
//------------------------------------------------------------------------------
HRESULT 
PlayOneFileStream( LPCSTR tszFile )
{
    CHAR pszFileName[MAX_PATH];

	if ( NULL == tszFile ) 
	{
		sprintf( pszFileName, "%s\\%u.wav", DVD_FILES, rand() % MAX_FILE_NUM + MIN_FILE_NUM );
		xLog( g_hLog, XLL_INFO, "Opening: %s", pszFileName );
	}
	else
		sprintf( pszFileName, "%s", tszFile );
	
    static const DWORD      dwPacketCount                       = 4;
    static const DWORD      dwPacketSize                        = 0x2000;
    LPXFILEMEDIAOBJECT      pSourceFilter                       = NULL;
    LPXWAVEFILEMEDIAOBJECT	pWaveSourceFilter                   = NULL;
    LPDIRECTSOUNDSTREAM     pRenderFilter                       = NULL;
	LPVOID                  pvBuffer                            = NULL;
    HANDLE                  hFile								= 0;
    HANDLE                  ahPacketEvents[dwPacketCount];
    XMEDIAPACKET            xmp;
	DWORD                   dwCompletedSize;
    DWORD                   dwPacketIndex;
	DWORD					dwWait;
    LPCWAVEFORMATEX         pwfxFile;
    DSSTREAMDESC            dssd;
    HRESULT                 hr;

	DWORD adwStatus[dwPacketCount];

	LPBYTE pBuffer = new BYTE[dwPacketSize * BUFFER_ALLOC_PAD];
	
	if ( NULL == pBuffer )
		return E_OUTOFMEMORY;

	delete [] pBuffer;

	// Randomly select the create method
	switch(rand() % 3)
	{
	case 0:
		hr = XWaveFileCreateMediaObjectEx(pszFileName, 0, &pWaveSourceFilter);
		if(SUCCEEDED(hr) && pWaveSourceFilter)
		{
			hr = pWaveSourceFilter->GetFormat(&pwfxFile);
			pSourceFilter = pWaveSourceFilter;
		}
		break;
	case 1:
	    hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile)
		{
			hr = XWaveFileCreateMediaObjectEx(NULL, hFile, &pWaveSourceFilter);
			if(SUCCEEDED(hr) && pWaveSourceFilter)
			{
				hr = pWaveSourceFilter->GetFormat(&pwfxFile);
				pSourceFilter = pWaveSourceFilter;
			}
		}
		else
			hr = E_FAIL;
		break;
	default:
		hr = XWaveFileCreateMediaObject(pszFileName, &pwfxFile, &pSourceFilter);
		break;
	}

    if (SUCCEEDED(hr))
    {
	    if(!pSourceFilter)
        {
    	    DbgPrint("XWaveFileCreateMediaObject returned %08X, but the outparam pSourceFilter was %08X\n", hr, pSourceFilter);
            hr = E_FAIL;
        }
    }

    //
    // Create the render (DirectSound stream) filter
    //
    if(SUCCEEDED(hr))
    {   
        ZeroMemory(&dssd, sizeof(dssd));
        dssd.dwMaxAttachedPackets = dwPacketCount;
        dssd.lpwfxFormat = (LPWAVEFORMATEX) pwfxFile;
		hr = DirectSoundCreateStream( &dssd, &pRenderFilter );
        if (SUCCEEDED(hr))
        {
		    if(!pRenderFilter)
		    {
    	        DbgPrint("DirectSoundCreateStream returned %08X, but the outparam "
					     "pRenderFilter was %08X\n", hr, pRenderFilter);
                hr = E_FAIL;
		    }
        }
    }

    //
    // Allocate a buffer large enough to store packet data for all the 
    // renderer's packets.  Because he's asynchronous, we have to keep the
    // data around until he's done processing it.
    //

    if(SUCCEEDED(hr))
    {
#ifndef DVTSNOOPBUG
        if(!(pvBuffer = LocalAlloc(LPTR, dwPacketSize * dwPacketCount)))
#else
		if (! ( pvBuffer = XPhysicalAlloc( dwPacketSize * dwPacketCount, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE ) ) )
#endif
        {
            hr = E_OUTOFMEMORY;
        }
    }

    //
    // Create render packet completion events
    //

	for ( dwPacketIndex = 0; dwPacketIndex < dwPacketCount; dwPacketIndex++ )
	{
		ahPacketEvents[dwPacketIndex] = 0;
		adwStatus[dwPacketIndex] = XMEDIAPACKET_STATUS_PENDING;
	}

    if(SUCCEEDED(hr))
    {
        for(dwPacketIndex = 0; dwPacketIndex < dwPacketCount; dwPacketIndex++)
        {
            if(!(ahPacketEvents[dwPacketIndex] = CreateEvent(NULL, TRUE, TRUE, NULL)))
            {
                hr = E_OUTOFMEMORY;
                break;
            }
        }
    }

    //
    // Enter the main loop
    //

    while(SUCCEEDED(hr))
    {

        //
        // Wait for a packet completion event to be signalled.  If this is our
        // first time through the loop, all the packets are free, so all the
        // events are signalled.
        //

		while ( SUCCEEDED( hr ) )
		{
			//
			// Have direct sound process
			//
			DirectSoundDoWork();

		    dwPacketIndex = WaitForMultipleObjects(dwPacketCount, ahPacketEvents, FALSE, 0) - WAIT_OBJECT_0;
			if ( WAIT_TIMEOUT != ( dwPacketIndex + WAIT_OBJECT_0 ) )
			{
				break;
			}
		}

		if ( WAIT_FAILED == dwPacketIndex + WAIT_OBJECT_0 )
		{
			hr = E_OUTOFMEMORY;
			break;
		}
        
        //
        // Read some data
        //

        if ( SUCCEEDED( hr ) )
		{
			ZeroMemory(&xmp, sizeof(xmp));
        
			xmp.pvBuffer = (LPBYTE) pvBuffer + (dwPacketSize * dwPacketIndex);
			xmp.dwMaxSize = dwPacketSize;
			xmp.pdwCompletedSize = &dwCompletedSize;
			xmp.hCompletionEvent = NULL;
        
			hr = pSourceFilter->Process(NULL, &xmp);
		}

		if(SUCCEEDED(hr) && !dwCompletedSize)
		{
            break;
        }

		if ( SUCCEEDED( hr ) && dwCompletedSize < dwPacketSize )
		{
			dwCompletedSize -= dwCompletedSize % pwfxFile->nBlockAlign;
		}

        if(SUCCEEDED(hr))
        {
            xmp.pdwCompletedSize = NULL;
			xmp.dwMaxSize = dwCompletedSize;
            xmp.hCompletionEvent = ahPacketEvents[dwPacketIndex];
			xmp.pdwStatus = adwStatus + dwPacketIndex;

            ResetEvent(xmp.hCompletionEvent);
	//		*xmp.pdwStatus = XMEDIAPACKET_STATUS_PENDING;

            hr = pRenderFilter->Process(&xmp, NULL);
        }
	}

	while ( SUCCEEDED( hr ) )
	{
		//
		// Have direct sound process
		//

		DirectSoundDoWork();

		dwWait = WaitForMultipleObjects(dwPacketCount, ahPacketEvents, TRUE, 0) - WAIT_OBJECT_0;
		if ( WAIT_TIMEOUT != ( dwWait + WAIT_OBJECT_0 ) )
		{
			break;
		}
	}

	if ( WAIT_FAILED == dwWait )
		hr = E_OUTOFMEMORY;

    //
    // Free events
    //

    for(dwPacketIndex = 0; dwPacketIndex < dwPacketCount; dwPacketIndex++)
    {
        if(ahPacketEvents[dwPacketIndex])
        {
            CloseHandle(ahPacketEvents[dwPacketIndex]);
        }
    }

	//
	// flush
	//

    if(pSourceFilter)
    {
        pSourceFilter->Flush();
    }

    if(pRenderFilter)
    {
        pRenderFilter->Flush();
    }

    //
    // Free objects
    //
	if(pWaveSourceFilter)
		pWaveSourceFilter->Release();
    else if(pSourceFilter)
        pSourceFilter->Release();

    if(pRenderFilter)
    {
        pRenderFilter->Release();
    }

    //
    // Free memory
    //

    if(pvBuffer)
    {
#ifndef DVTSNOOPBUG
        LocalFree(pvBuffer);
#else
		XPhysicalFree( pvBuffer );
#endif
    }

	// Close file
	if(hFile)
		CloseHandle(hFile);

	return hr;
}

//------------------------------------------------------------------------------
//	PlayWmaFile
//------------------------------------------------------------------------------
HRESULT 
PlayWmaFile( LPCSTR szFile, bool bSave )
{
    static const DWORD      dwPlaybackFrequency                 = 48000;
    static const DWORD      dwPacketCount                       = 16;
    static DWORD            dwPacketSize                        = 0x2000;
    LPXFILEMEDIAOBJECT      pSourceFilter                       = NULL;
    LPDIRECTSOUNDSTREAM     pRenderFilter                       = NULL;
	LPXMEDIAOBJECT          pOutputFile                         = NULL;
    LPVOID                  pvSourceBuffer                      = NULL;
    HANDLE                  ahPacketEvents[dwPacketCount];
	HANDLE                  hEvent                              = NULL;

    XMEDIAPACKET            xmp;
	XMEDIAINFO              xmi;
    DWORD                   dwSourceCompletedSize = 0;
    DWORD                   dwPacketIndex;
	DWORD					dwWait;
    WAVEFORMATEX            wfx;
    DSSTREAMDESC            dssd;
    HRESULT                 hr;

	DWORD					dwRand = rand() % 2;
	CHAR*                   szOutFileName                       = new CHAR[ strlen( szFile ) + 5 ];

	if ( NULL == szOutFileName )
		return E_OUTOFMEMORY;

	DbgPrint( "Playing %s using %s\n", szFile, 0 == dwRand ? "sync" : "async" );
	xLog( g_hLog, XLL_INFO, "Opening: %s", szFile );

	LPBYTE pBuffer = new BYTE[dwPacketSize * BUFFER_ALLOC_PAD];
	
	if ( NULL == pBuffer )
		return E_OUTOFMEMORY;

	delete [] pBuffer;

	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

    hr = WmaCreateDecoder( szFile, NULL, (BOOL) dwRand, (4096*16), dwPacketCount, 0, &wfx, &pSourceFilter );

    if(SUCCEEDED(hr))
    {   
        ZeroMemory(&dssd, sizeof(dssd));

        dssd.dwMaxAttachedPackets = dwPacketCount;
        dssd.lpwfxFormat = &wfx;

		hr = DirectSoundCreateStream( &dssd, &pRenderFilter );
    }

	if ( SUCCEEDED( hr ) && true == bSave )
	{
		sprintf( szOutFileName, "%s.wav", szFile );

		hr = XWaveOutCreateMediaObject( szOutFileName, &wfx, &pOutputFile );
	}

    if(SUCCEEDED(hr))
    {
#ifndef DVTSNOOPBUG
        if(!(pvSourceBuffer = LocalAlloc(LPTR, dwPacketSize * dwPacketCount)))
#else
		if (! ( pvSourceBuffer = XPhysicalAlloc( dwPacketSize * dwPacketCount, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE ) ) )
#endif
        {
            hr = E_OUTOFMEMORY;
        }
    }

	for ( dwPacketIndex = 0; dwPacketIndex < dwPacketCount; dwPacketIndex++ )
	{
		ahPacketEvents[dwPacketIndex] = 0;
	}

    if(SUCCEEDED(hr))
    {
        for(dwPacketIndex = 0; dwPacketIndex < dwPacketCount; dwPacketIndex++)
        {
            if(!(ahPacketEvents[dwPacketIndex] = CreateEvent(NULL, TRUE, TRUE, NULL)))
            {
                hr = E_OUTOFMEMORY;
                break;
            }
        }
    }

	if ( SUCCEEDED( hr ) )
	{
		hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

		if ( ! hEvent )
		{
			hr = E_OUTOFMEMORY;
		}
	}

    while(SUCCEEDED(hr))
    {
		while ( SUCCEEDED( hr ) )
		{
			//
			// Have direct sound process
			//
			DirectSoundDoWork();

		    dwPacketIndex = WaitForMultipleObjects(dwPacketCount, ahPacketEvents, FALSE, 0) - WAIT_OBJECT_0;
			if ( WAIT_TIMEOUT != ( dwPacketIndex + WAIT_OBJECT_0 ) )
			{
				break;
			}
		}

		if ( WAIT_FAILED == dwPacketIndex + WAIT_OBJECT_0 )
		{
			hr = E_OUTOFMEMORY;
			break;
		}
        
        if ( SUCCEEDED( hr ) )
		{
			ZeroMemory(&xmp, sizeof(xmp));
        
			xmp.pvBuffer = (LPBYTE) pvSourceBuffer + (dwPacketSize * dwPacketIndex);
			xmp.dwMaxSize = dwPacketSize;
			xmp.pdwCompletedSize = &dwSourceCompletedSize;
			xmp.hCompletionEvent = hEvent;
        
			hr = pSourceFilter->Process(NULL, &xmp);
		}

		dwWait = WaitForSingleObject( hEvent, INFINITE );

		if(SUCCEEDED(hr) && !dwSourceCompletedSize)
		{
            break;
        }

		if ( WAIT_FAILED == dwWait )
			hr = E_OUTOFMEMORY;

		if ( SUCCEEDED( hr ) && true == bSave )
		{
			xmp.pdwCompletedSize = NULL;
			xmp.dwMaxSize = dwSourceCompletedSize;

			hr = pOutputFile->Process( &xmp, NULL );
		}

		if(SUCCEEDED(hr) && !dwSourceCompletedSize)
		{
            break;
        }

        if(SUCCEEDED(hr))
        {
            xmp.pdwCompletedSize = NULL;
			xmp.dwMaxSize = dwSourceCompletedSize;
            xmp.hCompletionEvent = ahPacketEvents[dwPacketIndex];

            ResetEvent(xmp.hCompletionEvent);

            hr = pRenderFilter->Process(&xmp, NULL);
        }

		if ( SUCCEEDED( hr ) && dwSourceCompletedSize < dwPacketSize )
		{
			break;
		}

    }

	while ( SUCCEEDED( hr ) )
	{
		//
		// Have direct sound process
		//
		DirectSoundDoWork();

		dwWait = WaitForMultipleObjects(dwPacketCount, ahPacketEvents, TRUE, 0) - WAIT_OBJECT_0;
		if ( WAIT_TIMEOUT != ( dwWait + WAIT_OBJECT_0 ) )
		{
			break;
		}
	}

	if ( WAIT_FAILED == dwWait )
		hr = E_OUTOFMEMORY;

    if(pSourceFilter)
    {
        pSourceFilter->Release();
    }

    if(pRenderFilter)
    {
        pRenderFilter->Release();
    }

	if ( pOutputFile )
	{
		pOutputFile->Release();
	}

    for(dwPacketIndex = 0; dwPacketIndex < dwPacketCount; dwPacketIndex++)
    {
        if(ahPacketEvents[dwPacketIndex])
        {
            CloseHandle(ahPacketEvents[dwPacketIndex]);
        }
    }

	if ( hEvent )
	{
		CloseHandle( hEvent );
	}

    if(pvSourceBuffer)
    {
#ifndef DVTSNOOPBUG
        LocalFree(pvSourceBuffer);
#else
		XPhysicalFree( pvSourceBuffer );
#endif
    }

	delete [] szOutFileName;

	return hr;
}
	
//------------------------------------------------------------------------------
//	PlayOneFile
//------------------------------------------------------------------------------
HRESULT 
PlayOneFile( LPCSTR szFile, PLAY_MODE mode, bool bStress )
{
	CDSoundTest testFile;
	testFile.SetStress( bStress );

	switch ( mode )
	{
		case PLAY_MODE_POLL:
		case PLAY_MODE_WAIT:
		case PLAY_MODE_BUFFERED:
			DbgPrint( "Playing %s using BUFFERING\n", szFile );
			SETLOG( g_hLog, "danrose", "DSOUND", "PlayOneFile", "BUFFERING" );
			testFile.SetBufferDuration( DEFAULT_BUFFER_DURATION );
			testFile.SetPlayFlags( DSBPLAY_LOOPING );
			break;

		case PLAY_MODE_STREAM:
			DbgPrint( "Playing %s using STREAMING\n", szFile );
			SETLOG( g_hLog, "danrose", "XMO", "PlayOneFile", "STREAMING" );
			xLog( g_hLog, XLL_INFO, "Opening: %s", szFile );

			return PlayOneFileStream( szFile );
			break;

		default:
			break;
	}

	xLog( g_hLog, XLL_INFO, "Opening: %s", szFile );

	return testFile.OpenAndPlay( szFile );
}

//------------------------------------------------------------------------------
//	TestSequentialPlayback
//------------------------------------------------------------------------------
HRESULT 
TestSequentialPlayback( void )
{
	return PlaySequential( BVT_FILES, NUMELEMS( BVT_FILES ), PLAY_TO_COMPLETION );
}


//------------------------------------------------------------------------------
//	TestSimultaneousPlayback
//------------------------------------------------------------------------------
HRESULT 
TestSimultaneousPlayback( void )
{
	return PlaySimultaneous( SIM_FILES, NUMELEMS( SIM_FILES ), PLAY_TO_COMPLETION, INCREMENTAL_AMOUNT );
}

//------------------------------------------------------------------------------
//	TestSimplePlayback
//------------------------------------------------------------------------------
HRESULT 
TestSimplePlayback( void )
{
	return PlayOneFile( SIMPLE_FILE, PLAY_MODE_WAIT );
}

//------------------------------------------------------------------------------
//	TestBVTPlayback
//------------------------------------------------------------------------------
HRESULT 
TestBVTPlayback( void )
{
	return PlayOneFile( BVT_FILE, PLAY_MODE_WAIT );
}

//------------------------------------------------------------------------------
//	TestBufferedPlayback
//------------------------------------------------------------------------------
HRESULT 
TestBufferedPlayback( void )
{
	return PlayOneFile( SIMPLE_FILE, PLAY_MODE_BUFFERED );
}

//------------------------------------------------------------------------------
//	TestPollingPlayback
//------------------------------------------------------------------------------
HRESULT 
TestPollingPlayback( void )
{
	return PlayOneFile( SIMPLE_FILE, PLAY_MODE_POLL );
}

//------------------------------------------------------------------------------
//	TestOutOfMemory
//------------------------------------------------------------------------------
HRESULT 
TestOutOfMemory( void )
{
	HRESULT hr = PlayOneFile( VERY_LARGE_FILE, PLAY_MODE_WAIT );
	return SUCCEED_ON_FAIL( hr, E_OUTOFMEMORY );
}

//------------------------------------------------------------------------------
//	TestSimpleConstruction
//------------------------------------------------------------------------------
HRESULT 
TestSimpleConstruction( void )
{
	CDSoundTest testFile;
	return S_OK;
}

// ThreadTest returns an hr converted to a DWORD

//------------------------------------------------------------------------------
//	ThreadTest
//------------------------------------------------------------------------------
DWORD WINAPI 
ThreadTest( LPVOID lpParam )
{
	HRESULT hr = S_OK;
	DWORD dwParam = (DWORD) lpParam;

	hr = PlayOneFile( THREAD_FILES[dwParam], (PLAY_MODE) (dwParam % 4) );

	EXECUTE( hr );

	return (DWORD) hr;
}

//------------------------------------------------------------------------------
//	WMAThreadTest
//------------------------------------------------------------------------------
DWORD WINAPI 
WMAThreadTest( LPVOID lpParam )
{
	HRESULT hr = S_OK;
	DWORD dwParam = (DWORD) lpParam;

	hr = PlayWmaFile( WMA_THREAD_FILES[dwParam], true );

	EXECUTE( hr );

	return (DWORD) hr;
}

//------------------------------------------------------------------------------
//	TestMultiThreaded
//------------------------------------------------------------------------------
HRESULT 
TestMultiThreaded( LPTHREAD_START_ROUTINE pRoutine )
{
	HANDLE ahThread[NUM_THREADS];
	HRESULT hr = S_OK;
	DWORD dwWait = 0;

	
	for ( DWORD i = 0; i < NUM_THREADS && SUCCEEDED( hr ); i++ )
	{
		ahThread[i] = CreateThread( NULL, 0, pRoutine, (VOID*) i, 0, NULL );

		if ( NULL == ahThread[i] )
		{
			hr = E_OUTOFMEMORY;
		}
	}

	if ( SUCCEEDED( hr ) )
	{
		dwWait = WaitForMultipleObjects( NUM_THREADS, ahThread, TRUE, INFINITE );

		hr = WAIT_FAILED == dwWait ? E_OUTOFMEMORY : S_OK;
	}

	for ( i = 0; i < NUM_THREADS; i++ )
	{
		SAFECLOSEHANDLE( ahThread[i] );
	}

	return hr;

}

//------------------------------------------------------------------------------
//	DSUtilMakeSineWave
//------------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER 
DSUtilMakeSineWave(
				   IN LPDIRECTSOUND8	pDSound, 
				   IN DSMIXBINS*		pMixbins, 
				   IN double			frequency, 
				   IN WORD				numChannels,
				   IN DWORD				nSamplesPerSec,
				   IN WORD				wBitsPerSample
				   )
/*++

Routine Description:

    Plays a multichannel sine wave    

Arguments:

	IN pDSound -		DSound object, 
	IN pMixbins -		Mixbins (can be NULL), 
	IN frequency -		Frequency of sine wave in hertz, 
	IN numChannels -	Number of channels: 1, 2, 4 or 6
	IN nSamplesPerSec -	Samples per sec (22000, 41000, 48000, etc.)
	IN wBitsPerSample -	Bits per sample (8 or 16)

Return Value:

	If the buffer is created successfully, a pointer to the DSound Buffer
	otherwise NULL.

--*/
{
	DWORD				i;
	DWORD				index;
	DWORD				size;
	double				dF;
	double				dInc;
    HRESULT				hr			= S_OK;
	void*				pData		= NULL;
	DWORD				dwBytes;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;
	long				value;
	DWORD				c;
	LPDIRECTSOUNDBUFFER	pBuffer;
	
	// Basic error checking
	if((numChannels != 1) && (numChannels != 2) && (numChannels != 4) &&
	   (numChannels != 6))
	   return NULL;
	if((frequency > DSBFREQUENCY_MAX) || (frequency < DSBFREQUENCY_MIN))
		return NULL;

	// It's always safe to zero out stack allocated structs
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));

	// Setup the waveformat description
	wfx.cbSize			= 0;
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= numChannels;
	wfx.nSamplesPerSec	= nSamplesPerSec;
	wfx.wBitsPerSample	= wBitsPerSample;
	wfx.nBlockAlign		= numChannels * wBitsPerSample / 8;
	wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;

	// Setup the dsound buffer description with the waveformatex
	dsbd.dwBufferBytes	= (DWORD)((double)wfx.nAvgBytesPerSec / frequency);
	dsbd.dwBufferBytes	+= (dsbd.dwBufferBytes % wfx.nBlockAlign);
	dsbd.dwFlags		= 0;
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.lpwfxFormat	= &wfx;
    dsbd.lpMixBins		= pMixbins;

	// Create the dsound buffer
	hr = pDSound->CreateSoundBuffer(&dsbd, &pBuffer, NULL);
	if(SUCCEEDED(hr))
	{
		// Get a buffer from dsound using lock
		hr = pBuffer->Lock(0, dsbd.dwBufferBytes, &pData, &dwBytes, NULL, NULL, 0);
		if(SUCCEEDED(hr))
		{
			size	= (dsbd.dwBufferBytes / wfx.nBlockAlign) / numChannels;
			dInc	= (2.0 * cPi) / (double)size;
			for(dF=0.0, index=0, i=0;  i<size; ++i, dF+=dInc)
			{
				switch(wfx.wBitsPerSample)
				{
				case 8:
					value = (long)((sin(dF) + 1.0) / 2.0 * 255.0);
					for(c=0; c<numChannels; ++c)
						((unsigned char*)pData)[index++] = (unsigned char)value;
					break;

				case 16:
					value = (long)(sin(dF) * 32767.0);
					for(c=0; c<numChannels; ++c)
						((short*)pData)[index++] = (short)value;
					break;

				}

			}
			return pBuffer;
		}
    }
    
	// Error
	if(pBuffer)
		pBuffer->Release();
	return NULL;
}

//------------------------------------------------------------------------------
//	DSUtilReboot
//------------------------------------------------------------------------------
void
DSUtilReboot(void)
/*++

Routine Description:

	Reboots the Xbox

Arguments:

	None

Return Value:

	None

--*/
{
	HalReturnToFirmware(HalQuickRebootRoutine);
}
