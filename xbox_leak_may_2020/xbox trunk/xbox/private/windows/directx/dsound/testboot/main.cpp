/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       main.cpp
 *  Content:    insert content description here.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/19/01    dereks  Created.
 *
 ****************************************************************************/

#include "xtl.h"
#include "xdbg.h"

EXTERN_C PVOID InitBootSound();
EXTERN_C VOID ReleaseBootSound(PVOID pCore);

DWORD g_dwBreak = 0;

///
/// Set up necessary types
/// 

typedef short* LPSHORT;

///
/// Setup the frequency, and define PI
///

static const double FREQ = 2.55;
static const double PI = 3.1415926535;

///
/// Scaling factors
///
/// ISCALE is used to convert an interger [0, 44099] to a double [0, 2PI]
/// DATASCALE is used to conver a double [-1.0, 1.0] to an integer [-32767, 32767]

static const double ISCALE = 2.0 * PI / 48000.0;
static const double DATASCALE = 32767.0 / 1.0;


HRESULT PlaySineWave(double Frequency, DWORD dwMixBins,LPDIRECTSOUNDBUFFER     *pBuffer )
{

	///
	/// create a dsound buffer and a data buffer
	///

    HRESULT hr = S_OK;
	LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
	LPSHORT pData = NULL;

	///
	/// place to hold the number of bytes allocated by dsound
	///

	DWORD dwBytes = 0;

	///
	/// allocate space for calculate argument to sin, return value from sine, and actual integer value
	///

	double dArg = 0.0;
	double dSinVal = 0.0;
	SHORT sVal = 0;
   
	///
	/// allocate the dsound buffer description and the wave format
	///

	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;

	///
	/// It's always safe to zero out stack allocated structs
	///

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( DSBUFFERDESC ) );

	///
	/// setup the waveformat description to be PCM, 44100hz, 16bit, mono
    ///

	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 48000;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = 2;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	///
	/// setup the dsound buffer description with the waveformatex
	///

	dsbd.dwBufferBytes = wfx.nAvgBytesPerSec;
	dsbd.dwFlags = 0;
	dsbd.dwSize = sizeof( DSBUFFERDESC );
	dsbd.lpwfxFormat = &wfx;
    dsbd.dwMixBinMask = dwMixBins;

	/// 
	/// Create the dsound buffer
	///

	hr = DirectSoundCreateBuffer( &dsbd, &pDSBuffer );

	///
	/// continue forever (or until an error occurs)
	///

	if ( SUCCEEDED( hr ) )
	{

		///
		/// get a buffer from dsound using lock
		///

		hr = pDSBuffer->Lock( 0, dsbd.dwBufferBytes, (LPVOID*) &pData, &dwBytes, NULL, NULL, 0 );

		///
		/// go through the buffer 2 bytes (1 short) at a time
		///

		for( DWORD i = 0; SUCCEEDED( hr ) && i < dsbd.dwBufferBytes / 2; i++ )
		{	
			///
			/// calculate the sin value
			///

			dArg = (double) i * ISCALE;
			dSinVal = sin( Frequency * dArg );
			sVal = (SHORT) ( dSinVal * DATASCALE );

			///
			/// copy the value into the dsound buffer
			///

			pData[i] = sVal;
		}

    }
    
	///
	/// start the dsound buffer playing in a loop mode
	///

	if ( SUCCEEDED( hr ) )
	{
		hr = pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );
        *pBuffer = pDSBuffer;
	}



    //
    // leave with the buffer still playing..
    //

    return hr;
}

void __cdecl
main
(
    void
)
{
    HRESULT                 hr;
    LPDIRECTSOUNDBUFFER pBuffer;
    LPDIRECTSOUNDBUFFER pOscBuffer;

    hr = PlaySineWave(1000,DSMIXBIN_FXSEND_1, &pBuffer);

    if (FAILED(hr)) {
        _asm int 3;
    }

    hr = PlaySineWave(3,DSMIXBIN_FXSEND_2, &pOscBuffer);

    if (FAILED(hr)) {
        _asm int 3;
    }

    DWORD i =0,dwValue;
    while(TRUE) {

        dwValue = i;
        if (i%410000 == 0) {
            DbgPrint("dwValue %d\n",dwValue);
        }
        
        i++;

        if (g_dwBreak == 1) {
            break;
        }

        if (g_dwBreak == 2) {

            *(PDWORD)(0xFE830000+0x14) = 1;
        }

    }

    pBuffer->Release();
    pOscBuffer->Release();
    while(TRUE);
}



