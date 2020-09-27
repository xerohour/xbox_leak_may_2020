#pragma once

#include <dsoundtest.h>

extern HANDLE g_hLog;
extern "C" LPSTR g_szComponent;
extern "C" LPSTR g_szSubComp;
extern "C" LPSTR g_szFunction;
extern "C" LPSTR g_szVariation;

HRESULT PlaySequential( const LPCSTR* pszFileNames, ULONG ulNumFiles, DWORD dwSleepTime );
HRESULT PlaySimultaneous( const LPCSTR* pszFileNames, ULONG ulNumFiles, DWORD dwSleepTime, DWORD dwIncrementTime );
HRESULT PlayOneFile( LPCSTR szFile, PLAY_MODE mode, bool bStress = false );
HRESULT PlayWmaFile( LPCSTR szFile, bool bSave = false );
HRESULT TestSequentialPlayback( void );
HRESULT TestSimultaneousPlayback( void );
HRESULT TestSimplePlayback( void );
HRESULT TestBVTPlayback( void );
HRESULT TestBufferedPlayback( void );
HRESULT TestPollingPlayback( void );
HRESULT TestOutOfMemory( void );
HRESULT TestSimpleConstruction( void );
DWORD WINAPI ThreadTest( LPVOID lpParam );
DWORD WINAPI WMAThreadTest( LPVOID lpParam );
HRESULT TestMultiThreaded( LPTHREAD_START_ROUTINE pRoutine );


//------------------------------------------------------------------------------
//	DSUtilMakeSineWave
//------------------------------------------------------------------------------
LPDIRECTSOUNDBUFFER 
DSUtilMakeSineWave(IN LPDIRECTSOUND8	pDSound, 
				   IN DSMIXBINS*		pMixbins, 
				   IN double			frequency, 
				   IN WORD				numChannels,
				   IN DWORD				nSamplesPerSec,
				   IN WORD				wBitsPerSample);
/*------------------------------------------------------------------------------
Routine Description:

    Makes a multichannel sine wave    

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

------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
//	DSUtilReboot
//------------------------------------------------------------------------------
void
DSUtilReboot(void); 
/*------------------------------------------------------------------------------
Routine Description:

    Reboots the Xbox

Arguments:

	None

Return Value:

	This function does not return.

------------------------------------------------------------------------------*/
