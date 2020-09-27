/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       DirectSoundCreateBuffer.cpp
 *  Content:    DirectSoundCreateBuffer tests
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  04/11/01    danrose Created to test Xbox DirectSoundCreateBuffer
 *	10/12/01	robheit	Modified MaxBuffersPCMTest to use only 252 buffers
 *	01/02/02	robheit	Modified all tests to query the device for number
 *						of free voices instead of using hard coded values.
 *
 ****************************************************************************/

#include "DirectSoundCreateBuffer.h"

/****************************************************************************
 *
 * The Global Logging Handle
 *
 ****************************************************************************/

extern HANDLE g_hLog;

/****************************************************************************
 *
 *  CalculateXboxAdpcmAlignment
 *
 *  Description:
 *      calculates the proper ADPCM alignment
 *
 *  Arguments:
 *      nChannels - the number of channels
 *      nSamplesPerBlock - the number of samples per block
 *
 *  Returns:  
 *      the calculated alignment
 *
 ****************************************************************************/

WORD CalculateXboxAdpcmAlignment
(
    WORD                    nChannels,
    WORD                    nSamplesPerBlock
)
{
    const WORD              nEncodedSampleBits  = nChannels * 4;
    const WORD              nHeaderBytes        = nChannels * 4;
    WORD                    nBlockAlign;

    //
    // Calculate the raw block alignment that nSamplesPerBlock dictates.  This
    // value may include a partial encoded sample, so be sure to round up.
    //
    // Start with the samples-per-block, minus 1.  The first sample is actually
    // stored in the header.
    //

    nBlockAlign = nSamplesPerBlock - 1;

    //
    // Convert to encoded sample size
    //

    nBlockAlign *= nEncodedSampleBits;
    nBlockAlign += 7;
    nBlockAlign /= 8;

    //
    // The stereo encoder requires that there be at least two DWORDs to process
    //

    nBlockAlign += 7;
    nBlockAlign /= 8;
    nBlockAlign *= 8;

    //
    // Add the header
    //

    nBlockAlign += nHeaderBytes;

    return nBlockAlign;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer
 *
 *  Description:
 *      Create and Return a DirectSoundBuffer Interface Pointer
 *
 *  Arguments:
 *      ppDirectSoundBuffer - a double pointer to a DSBuffer Interface
 *      pWfx - a pointer to WAVEFORMATEX, can be NULL
 *		pDsbd - a pointer to a DSBUFFERDESC, can be NULL
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateBuffer( LPDIRECTSOUNDBUFFER* ppBuffer, LPWAVEFORMATEX pWfx, LPDSBUFFERDESC pDsbd )
{
    HRESULT             hr              = S_OK;
	LPDIRECTSOUNDBUFFER pBuffer			= NULL;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;

	//
	// check the incoming pointers
	//

	CHECKPTR( ppBuffer );

	//
	// Zero out the DSound buffer description
	//

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

	//
	// Fill in the WaveFormatEx
	//

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.wBitsPerSample = 8;
	wfx.nBlockAlign = 1;

	//
	// Set the Waveformat of the DSBUFFERDESC to my wfx or the passed in wfx
	//

	dsbd.lpwfxFormat = NULL == pWfx ? &wfx : pWfx;

	//
	// call DirectSoundCreateBuffer
	//

	CHECKRUN( DirectSoundCreateBuffer( NULL == pDsbd ? &dsbd : pDsbd, ppBuffer ) );

	//
	// return the Hresult
	//

	return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_ParameterTest
 *
 *  Description:
 *      Validate different parameter combinations for  DirectSoundCreateBuffer
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateBuffer_ParameterTest( void )
{
	HRESULT				hr					= S_OK;
	LPDIRECTSOUNDBUFFER pBuffer				= NULL;
	DSBUFFERDESC		dsbd                = { 0 };
	XBOXADPCMWAVEFORMAT	wfx;

	//
	// Create a test matrix of DSBUFFERDESC sub components
	//

	DWORD				adwSize[]			= { sizeof( DSBUFFERDESC ) };
	DWORD				adwFlags[]			= { 0,
												DSBCAPS_CTRL3D, 
												DSBCAPS_CTRLFREQUENCY,
												DSBCAPS_CTRLVOLUME,
												DSBCAPS_CTRLPOSITIONNOTIFY,
												DSBCAPS_MIXIN,
												DSBCAPS_LOCDEFER,
												DSBCAPS_FXIN };
	DWORD				adwMixBins[]		= { 0,
												DSMIXBIN_FRONT_LEFT,
												DSMIXBIN_FRONT_RIGHT,        
												DSMIXBIN_FRONT_CENTER,     
												DSMIXBIN_LOW_FREQUENCY,
												DSMIXBIN_BACK_LEFT,        
												DSMIXBIN_BACK_RIGHT,      
												DSMIXBIN_XTLK_FRONT_LEFT,   
												DSMIXBIN_XTLK_FRONT_RIGHT,
												DSMIXBIN_XTLK_BACK_LEFT,   
												DSMIXBIN_XTLK_BACK_RIGHT, 
												DSMIXBIN_I3DL2,     
												DSMIXBIN_FXSEND_0,
												DSMIXBIN_FXSEND_1,
												DSMIXBIN_FXSEND_2,
												DSMIXBIN_FXSEND_3,
												DSMIXBIN_FXSEND_4,
												DSMIXBIN_FXSEND_5,
												DSMIXBIN_FXSEND_6,
												DSMIXBIN_FXSEND_7,
												DSMIXBIN_FXSEND_8,
												DSMIXBIN_FXSEND_9,
												DSMIXBIN_FXSEND_10,
												DSMIXBIN_FXSEND_11,
												DSMIXBIN_FXSEND_12,
												DSMIXBIN_FXSEND_13,
												DSMIXBIN_FXSEND_14,
												DSMIXBIN_FXSEND_15,
												DSMIXBIN_FXSEND_16,
												DSMIXBIN_FXSEND_17,
												DSMIXBIN_FXSEND_18,
												DSMIXBIN_FXSEND_19 };
	DWORD				adwSecs[]			= { 1, 2 };
												
	//
	// Create a test matrix of WAVEFORMATEX sub components
	//

    WORD				awFormatTag[]		= { WAVE_FORMAT_PCM,
												WAVE_FORMAT_XBOX_ADPCM };             
    WORD				anChannels[]		= { 1, 
												2 }; 
	DWORD				anSamplesPerSec[]   = { 8000, 
												11025,
												16000,
												22050,
												32000,
												44100,
												48000 };
	WORD				awBitsPerSample[]	= { 8,
												16 };
             

	
	//
	// Cycle through each param, try to create a DirectSoundBuffer
	//

	for ( DWORD a = 0; a < NUMELEMS( adwSize ); a++ )
	{
		for ( DWORD b = 0; b < NUMELEMS( adwFlags ); b++ )
		{
			for ( DWORD c = 0; c < NUMELEMS( anSamplesPerSec ); c++ )
			{
				for ( DWORD d = 0; d < NUMELEMS( adwMixBins ); d++ )
				{
					for ( DWORD e = 0; e < NUMELEMS( awFormatTag ); e++ )
					{
						for ( DWORD f = 0; f < NUMELEMS( anChannels ); f++ )
						{
							for ( DWORD g = 0; g < NUMELEMS( adwSecs ); g++ )
							{
								for ( DWORD h = 0; h < NUMELEMS( awBitsPerSample ); h++ )
								{	
									//
									// fill in wfx
									//

									wfx.wfx.wFormatTag = awFormatTag[e];                                      
									wfx.wfx.nSamplesPerSec = anSamplesPerSec[c];  

									//
									// special case out certain values
									//

									//
									// 3d buffers must be mono
									//

								    if ( ( adwFlags[b] & DSBCAPS_CTRL3D ) )
									{
										wfx.wfx.nChannels = 1;
									}
									else
									{
										wfx.wfx.nChannels = anChannels[f];
									}

									//
									// ADPCM buffers have set following data
									//

									if ( WAVE_FORMAT_XBOX_ADPCM == wfx.wfx.wFormatTag )
									{
										wfx.wfx.cbSize = 2;
										wfx.wfx.wBitsPerSample = 4;
										wfx.wSamplesPerBlock = 64;
										wfx.wfx.nBlockAlign = CalculateXboxAdpcmAlignment( wfx.wfx.nChannels, wfx.wSamplesPerBlock );
									}  
									else
									{
										wfx.wfx.cbSize = 0;
										wfx.wfx.wBitsPerSample = awBitsPerSample[h];
										wfx.wSamplesPerBlock = 0;
										wfx.wfx.nBlockAlign = wfx.wfx.nChannels * wfx.wfx.wBitsPerSample / 8;
									}

									//
									// Finish filling in WFX
									//

									wfx.wfx.nAvgBytesPerSec = wfx.wfx.nSamplesPerSec * wfx.wfx.nBlockAlign;    
								

									//
									// fill in DSBD
									//

									dsbd.dwSize = adwSize[a];
									dsbd.dwFlags = adwFlags[b];
									
									//
									// update the dsbd for special cases
									//

									//
									// mixin buffers do not have buffers or formats
									//

									if ( dsbd.dwFlags & DSBCAPS_MIXIN ) 
									{
										dsbd.lpwfxFormat = NULL;
										dsbd.dwBufferBytes = 0;
									}
									else
									{
										dsbd.lpwfxFormat = (LPWAVEFORMATEX) &wfx;
										dsbd.dwBufferBytes = wfx.wfx.nAvgBytesPerSec * adwSecs[g];
									}

									//
									// make sure to enable 3d if necessary
									//

									if ( DSBCAPS_CTRL3D == adwFlags[b] )
									{
										if ( rand() % 2 )
										{
											DirectSoundUseFullHRTF();
										}
										else
										{
											DirectSoundUseLightHRTF();
										}
									}

									//
									// actualy make the call
									//

								    EXECUTE( DirectSoundCreateBuffer( &pBuffer, NULL, &dsbd ) );
								    RELEASE( pBuffer );
								}
							}
						}
					}
				}
			}
		}
	}

	//
	// Release the buffer
	//

	RELEASE( pBuffer );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_BasicTest
 *
 *  Description:
 *      Basic Test for DirectSoundCreateBuffer
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateBuffer_BasicTest( void )
{
    HRESULT             hr              = S_OK;
	LPDIRECTSOUNDBUFFER pBuffer			= NULL;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;

	//
	// Zero out the DSound buffer description and the WaveFormatEx
	//

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

	//
	// Fill in the WaveFormatEx
	//

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.wBitsPerSample = 8;
	wfx.nBlockAlign = 1;
	wfx.nSamplesPerSec = 48000;

	//
	// Set the Waveformat of the DSBUFFERDESC to my wfx
	//

	dsbd.lpwfxFormat = &wfx;

	//
	// Call DirectSoundCreateBuffer and check return code and
	// resultant pointer
	//

	CHECKRUN( DirectSoundCreateBuffer( &dsbd, &pBuffer ) );
	CHECKALLOC( pBuffer );

	//
	// Destroy the DirectSoundBuffer
	//

	RELEASE( pBuffer );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_BasicADPCMTest
 *
 *  Description:
 *      Basic Test for DirectSoundCreateBuffer init using ADPCM
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateBuffer_BasicADPCMTest( void )
{
    HRESULT             hr              = S_OK;
	LPDIRECTSOUNDBUFFER pBuffer			= NULL;
	DSBUFFERDESC		dsbd;
	XBOXADPCMWAVEFORMAT	wfx;

	//
	// Zero out the DSound buffer description and the WaveFormatEx
	//

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( XBOXADPCMWAVEFORMAT ) );

	//
	// Fill in the WaveFormatEx
	//

	wfx.wfx.wFormatTag = WAVE_FORMAT_XBOX_ADPCM;
	wfx.wfx.nChannels = 1;
	wfx.wfx.wBitsPerSample = 4;
	wfx.wfx.nBlockAlign = 36;
	wfx.wfx.cbSize = 2;
	wfx.wfx.nSamplesPerSec = 48000;
	wfx.wSamplesPerBlock = 64;

	//
	// Set the Waveformat of the DSBUFFERDESC to my wfx
	//

	dsbd.lpwfxFormat = (LPWAVEFORMATEX) &wfx;

	//
	// Call DirectSoundCreateBuffer and check return code and
	// resultant pointer
	//

	CHECKRUN( DirectSoundCreateBuffer( &dsbd, &pBuffer ) );
	CHECKALLOC( pBuffer );

	//
	// Destroy the DirectSoundBuffer
	//

	RELEASE( pBuffer );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_OutOfMemoryTest
 *
 *  Description:
 *      Out of memory Test for DirectSoundCreateBuffer
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateBuffer_OutOfMemoryTest( void )
{
    HRESULT             hr              = S_OK;
	LPDIRECTSOUNDBUFFER pBuffer			= NULL;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;

	//
	// Memory variables
	//

    LPBYTE              pData           = NULL;
	DWORD				i				= 0;
	DWORD				dwPageSize      = 0x1000;
    MEMORYSTATUS        memStatus;

	//
	// Zero out the Memory Status Structure
	//

    ZeroMemory( &memStatus, sizeof( MEMORYSTATUS ) );

	//
	// Get the Memory available on the Xbox
	//

    GlobalMemoryStatus( &memStatus );

	//
	// Keep trying to allocate available memory. If the allocation
	// fails, subtract 4K bytes (1 page) of memory and try again
	//

	do {

		pData = new BYTE[ memStatus.dwAvailPhys - dwPageSize * i++ ];
	
	} while ( NULL == pData );

	//
	// Zero out the DSound buffer description and the WaveFormatEx
	//

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

	//
	// Fill in the WaveFormatEx
	//

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.wBitsPerSample = 8;
	wfx.nBlockAlign = 1;

	//
	// Set the Waveformat ofthe DSBUFFERDESC to my wfx
	//

	dsbd.lpwfxFormat = &wfx;

	//
	// Call DirectSoundCreateBuffer and check return code and
	// resultant pointer
	//

	CHECKRUN( DirectSoundCreateBuffer( &dsbd, &pBuffer ) );
	
	//
	// If the large allocation was successful, the DsoundBuffer pointer
	// was null, and The call to DirectSoundCreate returned
	// the correct error code, then set hr to S_OK
	//

	if ( E_OUTOFMEMORY == hr && NULL == pBuffer && NULL != pData )
	{
		hr = S_OK;
	} else {
		hr = E_FAIL;
	}

	//
	// Destroy the DirectSoundBuffer, clena up memory
	//

	delete [] pData;
	RELEASE( pBuffer );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_NullDSBDTest
 *
 *  Description:
 *      Test DirectSoundCreateBuffer with a null DSBD
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateBuffer_NullDSBDTest( void )
{
    HRESULT             hr              = S_OK;
	LPDIRECTSOUNDBUFFER pBuffer			= NULL;

	//
	// Call DirectSoundCreateBuffer and check return code and
	// resultant pointer
	//

	CHECKRUN( DirectSoundCreateBuffer( NULL, &pBuffer ) );
	CHECKALLOC( pBuffer );

	//
	// Destroy the DirectSoundBuffer
	//

	RELEASE( pBuffer );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_NullBufferTest
 *
 *  Description:
 *      Test DirectSoundCreateBuffer with a null Buffer
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateBuffer_NullBufferTest( void )
{
    HRESULT             hr              = S_OK;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;

	//
	// Zero out the DSound buffer description and the WaveFormatEx
	//

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

	//
	// Fill in the WaveFormatEx
	//

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.wBitsPerSample = 8;
	wfx.nBlockAlign = 1;

	//
	// Set the Waveformat ofthe DSBUFFERDESC to my wfx
	//

	dsbd.lpwfxFormat = &wfx;
	//
	// Call DirectSoundCreateBuffer and check return code
	//

	CHECKRUN( DirectSoundCreateBuffer( &dsbd, NULL ) );

    //
    // Return the Hresult
    //

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_MaxBuffersPCMTest
 *
 *  Description:
 *      Test DirectSoundCreateBuffer and make sure the max number of PCM
 *      buffers can be created
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateBuffer_MaxBuffersPCMTest( void )
{
    HRESULT             hr              = S_OK;
	DWORD				i				= 0;
	LPDIRECTSOUND8		pDSound			= NULL;
	LPDIRECTSOUNDBUFFER apBuffers[256];
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;
	DSCAPS				dsCaps;

	// zero out the structs
	ZeroMemory( &apBuffers, sizeof( apBuffers ) );
	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

	// setup the WaveFormatEx
	wfx.wFormatTag		= WAVE_FORMAT_PCM;
    wfx.nChannels		= 1;  
    wfx.nSamplesPerSec	= 16000;     
    wfx.nBlockAlign		= 1;   
    wfx.wBitsPerSample	= 8;   
    wfx.nAvgBytesPerSec	= wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize			= 0;

	// Setup the DSBufferDesc
	dsbd.dwBufferBytes = wfx.nAvgBytesPerSec;
	dsbd.lpwfxFormat = &wfx;

	// Determine the max number of buffers available
	DirectSoundCreate(NULL, &pDSound, NULL);
	pDSound->GetCaps(&dsCaps);

	// Create the Max number of PCM buffers
	for ( i = 0; SUCCEEDED( hr ) && i < dsCaps.dwFree2DBuffers; ++i)
	{
		CHECKRUN( DirectSoundCreateBuffer( &dsbd, &apBuffers[i] ) );
		CHECKALLOC( apBuffers[i] );
	}

	// Release all the buffers
	for ( i = 0; i < dsCaps.dwFree2DBuffers; i++ )
	{
		RELEASE( apBuffers[i] );
	}

	// Release the DSound object
	pDSound->Release();

    // Return the Hresult
    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_ThreadFunction
 *
 *  Description:
 *      thread function for multiple thread test
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		0 on failure
 *      a pointer cast to a DWORD representing the Dsound Buffer Interface
 *
 ****************************************************************************/

DWORD WINAPI DirectSoundCreateBuffer_ThreadFunction( LPVOID lpParameter )
{
    HRESULT             hr              = S_OK;
	LPDIRECTSOUNDBUFFER	pBuffer			= NULL;
	DWORD				dwRet			= 0;
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfx;

	//
	// Zero out the DSound buffer description and the WaveFormatEx
	//

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

	//
	// Fill in the WaveFormatEx
	//

	wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;  
    wfx.nSamplesPerSec = 32000;     
    wfx.nBlockAlign = 1;   
    wfx.wBitsPerSample = 8;   
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

	//
	// Set up the DSBUFFERDESC
	//

	dsbd.dwBufferBytes = wfx.nAvgBytesPerSec;
	dsbd.lpwfxFormat = &wfx;

	//
	// Call DirectSoundCreateBuffer and check return code and
	// resultant pointer
	//

	CHECKRUN( DirectSoundCreateBuffer( &dsbd, &pBuffer ) );	

	//
	// Cast the pointer value
	//

	dwRet = (DWORD) pBuffer;

	//
	// Release the IDirectSoundBuffer pointer
	// 

    RELEASE( pBuffer );

	//
	// return the pBuffer pointer value
	//

	return dwRet;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_MaxBuffersPCMThreadedTest
 *
 *  Description:
 *      Test DirectSoundCreateBuffer and make sure the max number of PCM
 *      buffers can be created on multiple threads
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      S_OK on success
 *      E_XX on failure
 *
 ****************************************************************************/

HRESULT DirectSoundCreateBuffer_MaxBuffersPCMThreadedTest( void )
{
    HRESULT             hr              = S_OK;
	DWORD				i				= 0;
	DWORD				dwWait			= 0;
	DWORD				adwRetVals[256];
	HANDLE				ahThreads[256];
	LPDIRECTSOUND8		pDSound			= NULL;
	DSCAPS				dsCaps;
	DWORD				nCount;

	// Determine the max free buffers
	DirectSoundCreate(NULL, &pDSound, NULL);
	pDSound->GetCaps(&dsCaps);

	//
	// Zero out the thread array and the ret vals
	//

	ZeroMemory( &ahThreads, sizeof( ahThreads ) );
	ZeroMemory( &adwRetVals, sizeof( adwRetVals ) );

	//
	// cycle through the thread array, and create new threads
	// error out as appropriate
	//

	for ( i = 0; i < dsCaps.dwFree2DBuffers && SUCCEEDED( hr ); i++ )
	{
		ahThreads[i] = CreateThread( NULL, 0, &DirectSoundCreateBuffer_ThreadFunction, NULL, 0, NULL );

		if ( NULL == ahThreads[i] )
			hr = E_OUTOFMEMORY;
	}

	//
	// Wait on the threads to finish, check return code
	//

	for(i=0; i<dsCaps.dwFree2DBuffers && SUCCEEDED(hr); i += nCount)
	{
		nCount = MAXIMUM_WAIT_OBJECTS;
		if(nCount > (dsCaps.dwFree2DBuffers - i))
			nCount = (dsCaps.dwFree2DBuffers - i);
		dwWait = WaitForMultipleObjects(nCount, ahThreads + i, TRUE, INFINITE);

		if ( WAIT_FAILED == dwWait )
			hr = E_FAIL;
	}

	//
	// Get the return values from the threads
	//

	for ( i = 0; i < dsCaps.dwFree2DBuffers && SUCCEEDED( hr ); i++ )
	{
		if ( ! GetExitCodeThread( ahThreads[i], adwRetVals + i ) )
			hr = E_FAIL;
	}

	//
	// Check the values and make sure they are equal and no zero
	//

	for ( i = 0; i < dsCaps.dwFree2DBuffers && SUCCEEDED( hr ); i++ )
	{
		FAIL_ON_CONDITION( adwRetVals[i] == 0 );
	}

	//
	// close each thread handle
	//

	for ( i = 0; i < dsCaps.dwFree2DBuffers; i++ )
	{
		if ( NULL != ahThreads[i] )
			CloseHandle( ahThreads[i] );
	}

	//
	// Return the Hresult
	//

    return hr;
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_StartTest
 *
 *  Description:
 *		The Harness Entry into the DirectSoundCreateBuffer tests
 *
 *  Arguments:
 *      LogHandle - a handle to a logging object
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI DirectSoundCreateBuffer_StartTest( HANDLE LogHandle )
{
    HRESULT             hr              = S_OK;

    //
    // the following tests will Assert (by design) in debug builds
    // to turn these tests on (they are off by default) define
    // CODE_COVERAGE when this file is compiled
    //

#ifdef CODE_COVERAGE

	//
	// Test DirectSoundCreateBuffer in an out of memory situation
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateBuffer", "OutOfMemory" );
    EXECUTE( DirectSoundCreateBuffer_OutOfMemoryTest() );

	//
	// Test DirectSoundCreateBuffer with a NULL DSBD
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateBuffer", "NullDSBD" );
    EXECUTE( DirectSoundCreateBuffer_NullDSBDTest() );

	//
	// Test DirectSoundCreateBuffer with a NULL DirectSoundBuffer
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateBuffer", "NullBuffer" );
    EXECUTE( DirectSoundCreateBuffer_NullBufferTest() );

	//
	// Test DirectSoundCreateBuffer using all combinations of valid parameters 
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateBuffer", "Parameter" );
	EXECUTE( DirectSoundCreateBuffer_ParameterTest() );

#endif // CODE_COVERAGE

    //
    // Test DirectSoundCreateBuffer in the mannor it was meant to be called
    //

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateBuffer", "Basic" );
    EXECUTE( DirectSoundCreateBuffer_BasicTest() );

	//
	// Test DirectSoundCreateBuffer using basic ADPCM
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateBuffer", "BasicADPCM" );
    EXECUTE( DirectSoundCreateBuffer_BasicADPCMTest() );

	//
	// Test DirectSoundCreateBuffer using the max number of PCM buffers
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateBuffer", "MaxBuffersPCM" );
    EXECUTE( DirectSoundCreateBuffer_MaxBuffersPCMTest() );

	//
	// Test DirectSoundCreateBuffer using the max number of PCM buffers on multiple threads
	//

    SETLOG( LogHandle, "danrose", "MCPX", "DirectSoundCreateBuffer", "MaxBuffersPCMThreaded" );
    EXECUTE( DirectSoundCreateBuffer_MaxBuffersPCMThreadedTest() );

	
}

/****************************************************************************
 *
 *  DirectSoundCreateBuffer_EndTest
 *
 *  Description:
 *		The Exit function for the test harness
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *		(void)
 *
 ****************************************************************************/

VOID WINAPI DirectSoundCreateBuffer_EndTest( VOID )
{
}

/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      the exe entry point
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#ifdef NOLOGGING
void __cdecl main( void )
{
    DirectSoundCreateBuffer_StartTest( NULL );
    DirectSoundCreateBuffer_EndTest();
}
#endif // NOLOGGING

/****************************************************************************
 *
 * Export Function Pointers for StartTest and EndTest
 *
 ****************************************************************************/

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( DirectSoundCreateBuffer )
#pragma data_seg()

BEGIN_EXPORT_TABLE( DirectSoundCreateBuffer )
    EXPORT_TABLE_ENTRY( "StartTest", DirectSoundCreateBuffer_StartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DirectSoundCreateBuffer_EndTest )
END_EXPORT_TABLE( DirectSoundCreateBuffer )
