#include <dsutil.h>
#include "retVals.h"

struct MixerTest {
	DWORD dwInputBufferSize;
	DWORD dwAccumBufferSize;
	DWORD dwOutputBufferSize;
	DWORD dwInputStart;
	LONG lAccumStart;
	DWORD dwOutputStart;

	WORD wLeft;
	WORD wRight;

	WORD wSourceFormatTag;
	WORD nSourceChannels;
	DWORD nSourceSamplesPerSec;
	DWORD nSourceAvgBytesPerSec;
	WORD nSourceBlockAlign;
	WORD wSourceBitsPerSample;
	WORD cbSourceSize;

	WORD wDestFormatTag;
	WORD nDestChannels;
	DWORD nDestSamplesPerSec;
	DWORD nDestAvgBytesPerSec;
	WORD nDestBlockAlign;
	WORD wDestBitsPerSample;
	WORD cbDestSize;

	DWORD dwRetVal;
};

static WAVEFORMATEX g_wfx[] = {
	{ WAVE_FORMAT_PCM, 1, 1, 1, 1, 8,  0 },  // 8 bit, mono
	{ WAVE_FORMAT_PCM, 1, 1, 2, 2, 16, 0 }, // 16 bit, mono
	{ WAVE_FORMAT_PCM, 2, 1, 2, 2, 8,  0 }, // 8bit stereo
	{ WAVE_FORMAT_PCM, 2, 1, 4, 4, 16, 0 }, // 16 bit stereo
};

static WORD g_wVolumes[] = {
	0x0000,
	0x7FFF,
	0xFFFF,
};

static DWORD g_dwStartAccumValues[] = {
 //	0x80000000 /*        -2^31 */, 
 // 	0x80000001 /*      -2^31+1 */,
 // 	0x80008000 /*   -2^31+2^15 */, 
 // 	0x80008001 /* -2^31+2^15+1 */, 
 // 	0x80010000 /*   -2^31+2^16 */, 
 // 	0x80010001 /* -2^31+2^16+1 */,
 // 	0xFFFF0000 /*       -65536 */,
 // 	0xFFFF0001 /*       -65535 */,
 // 	0xFFFF1000 /*       -61440 */,
  	0xFFFF8000 /*       -32768 */,
  	0xFFFF8001 /*       -32767 */,
  	0xFFFFF000 /*        -4096 */,
  	0xFFFFF001 /*        -4095 */,
  	0xFFFFFF00 /*         -256 */,
  	0xFFFFFF01 /*         -255 */,
  	0xFFFFFF80 /*         -128 */,
  	0xFFFFFF81 /*         -127 */,
  	0xFFFFFFFF /*           -1 */,
  	0x00000000 /*            0 */,
  	0x00000001 /*            1 */,
  	0x0000007F /*          127 */,
  	0x00000080 /*          128 */,
  	0x000000FF /*          255 */,
  	0x00000100 /*          256 */,
  	0x00000FFF /*         4095 */,
  	0x00001000 /*         4096 */,
  	0x00007FFF /*        32767 */,
  	0x00008000 /*        32768 */,
  //	0x0000F000 /*        61440 */,
  //	0x00010000 /*        65536 */,
  //	0x7FFEFFFF /*  2^31-2^16-1 */,
  //	0x7FFF0000 /*    2^31-2^16 */,
  //	0x7FFF7FFF /*  2^31-2^15-1 */,
  //	0x7FFF8000 /*    2^31-2^15 */,
  //	0x7FFFFFFF /*       2^31-1 */,
};

static DWORD g_dwStartValues[] = {
	0,
	127,
	128,
	255,
};

void InitTestMixer( MixerTest* pmt, LPWAVEFORMATEX pwfxSrc, LPWAVEFORMATEX pwfxDest, WORD wLeftVol, WORD wRightVol, DWORD dwInputStart, DWORD dwAccumStart, DWORD dwOutputStart )
{
	if ( NULL == pmt || NULL == pwfxSrc || NULL == pwfxDest )
		return;

	pmt->dwInputBufferSize = pwfxSrc->wBitsPerSample * pwfxSrc->nChannels / 8;
	pmt->dwOutputBufferSize = pwfxDest->wBitsPerSample * pwfxDest->nChannels / 8;
	pmt->dwAccumBufferSize = 4 * max( pwfxSrc->nChannels, pwfxDest->nChannels );

	pmt->dwInputStart = dwInputStart;
	pmt->lAccumStart = dwAccumStart;
	pmt->dwOutputStart = dwOutputStart;

	pmt->wLeft = wLeftVol;
	pmt->wRight = wRightVol;

	pmt->wSourceFormatTag = pwfxSrc->wFormatTag;
	pmt->nSourceChannels = pwfxSrc->nChannels;
	pmt->nSourceSamplesPerSec = pwfxSrc->nSamplesPerSec;
	pmt->nSourceAvgBytesPerSec = pwfxSrc->nAvgBytesPerSec;
	pmt->nSourceBlockAlign = pwfxSrc->nBlockAlign; 
	pmt->wSourceBitsPerSample = pwfxSrc->wBitsPerSample;
	pmt->cbSourceSize = pwfxSrc->cbSize;

	pmt->wDestFormatTag = pwfxDest->wFormatTag;
	pmt->nDestChannels = pwfxDest->nChannels;
	pmt->nDestSamplesPerSec = pwfxDest->nSamplesPerSec;
	pmt->nDestAvgBytesPerSec = pwfxDest->nAvgBytesPerSec;
	pmt->nDestBlockAlign = pwfxDest->nBlockAlign; 
	pmt->wDestBitsPerSample = pwfxDest->wBitsPerSample;
	pmt->cbDestSize = pwfxDest->cbSize;
}

void InitWaveFormatEx( LPWAVEFORMATEX pWfx, WORD wFormatTag, WORD nChannels, DWORD nSamplesPerSec, DWORD nAvgBytesPerSec, WORD nBlockAlign, WORD wBitsPerSample, WORD cbSize )
{
	if ( NULL == pWfx )
		return;

	pWfx->wFormatTag = wFormatTag;
	pWfx->nChannels = nChannels;
	pWfx->nSamplesPerSec = nSamplesPerSec;
	pWfx->nAvgBytesPerSec = nAvgBytesPerSec;
	pWfx->nBlockAlign = nBlockAlign;
	pWfx->wBitsPerSample = wBitsPerSample;
	pWfx->cbSize = cbSize;

}

void InitXMEDIAPACKET( LPXMEDIAPACKET pXBuffer, LPVOID pvBuffer, DWORD dwMaxSize, PDWORD pdwCompletedSize, PDWORD pdwStatus, HANDLE hCompletionEvent, REFERENCE_TIME *prtTimestamp )
{
	if ( NULL == pXBuffer )
		return;

    pXBuffer->pvBuffer = pvBuffer;
    pXBuffer->dwMaxSize = dwMaxSize;
    pXBuffer->pdwCompletedSize = pdwCompletedSize;
    pXBuffer->pdwStatus = pdwStatus;
    pXBuffer->hCompletionEvent = hCompletionEvent;
    pXBuffer->prtTimestamp = prtTimestamp;
}

void InitAccumBuffer( LPXMEDIAPACKET pXBuffer, LONG lVal )
{
	if ( NULL == pXBuffer || NULL == pXBuffer->pvBuffer )
		return;

	*((LPLONG) pXBuffer->pvBuffer) = lVal;
}

/*void InitVolume( LPDSMXVOLUME pVolume, WORD wLeft, WORD wRight )
{
	if ( NULL == pVolume )
		return;

	pVolume->wLeft = wLeft;
	pVolume->wRight = wRight;
}
*/

void PrintBuffer( LPCSTR szName, DWORD dwSwitchVal, LPXMEDIAPACKET pBuffer )
{
	switch ( dwSwitchVal )
	{
		case 8:
			DbgPrint( "%s: %d (0x%x)\n", szName, *((LPBYTE) pBuffer->pvBuffer), *((LPBYTE) pBuffer->pvBuffer) );
			break;

		case 16:
			DbgPrint( "%s: %d (0x%x)\n", szName, *((LPWORD) pBuffer->pvBuffer), *((LPWORD) pBuffer->pvBuffer) );
			break;

		case 32:
			DbgPrint( "%s: %d (0x%x)\n", szName, *((LPDWORD) pBuffer->pvBuffer), *((LPDWORD) pBuffer->pvBuffer) );
			break;

		default:
			break;
	}
}

void PrintBuffers( LPXMEDIAPACKET pInputBuffer, LPXMEDIAPACKET pAccumBuffer, LPXMEDIAPACKET pOutputBuffer, LPWAVEFORMATEX pSrcWfx, LPWAVEFORMATEX pDestWfx )
{
	if ( NULL == pInputBuffer || NULL == pAccumBuffer || NULL == pOutputBuffer || NULL == pInputBuffer->pvBuffer || NULL == pOutputBuffer->pvBuffer || NULL == pAccumBuffer->pvBuffer || NULL == pSrcWfx || NULL == pDestWfx )
		return;

	PrintBuffer( "Input", pSrcWfx->wBitsPerSample * pSrcWfx->nChannels, pInputBuffer );
	PrintBuffer( "Accum", 32, pAccumBuffer );
	PrintBuffer( "Output", pDestWfx->wBitsPerSample * pDestWfx->nChannels, pOutputBuffer );
}

HRESULT VerifyResult( MixerTest* pMT, LPWAVEFORMATEX pwfx, LPXMEDIAPACKET pXBuffer, DWORD dwCount )
{
	if ( NULL == pMT || NULL == pwfx || NULL == pXBuffer )
		return E_POINTER;

	HRESULT hr = S_OK;

	switch ( pwfx->wBitsPerSample * pwfx->nChannels )
	{
		case 8:
			hr = g_dwRetVal[dwCount] == *((LPBYTE)pXBuffer->pvBuffer) ? S_OK : E_FAIL;
		//	hr = *((LPBYTE)pXBuffer->pvBuffer);
			break;

		case 16:
			hr = g_dwRetVal[dwCount] == *((LPWORD)pXBuffer->pvBuffer) ? S_OK : E_FAIL;
		//	hr = *((LPWORD)pXBuffer->pvBuffer);
			break;

		case 32:
			hr = g_dwRetVal[dwCount] == *((LPDWORD)pXBuffer->pvBuffer) ? S_OK : E_FAIL;
		//	hr = *((LPDWORD)pXBuffer->pvBuffer);
			break;

		default:
			break;
	};

	return hr;
}
/*
HRESULT TestMixer( MixerTest* pMT, DWORD dwCount )
{
	if ( NULL == pMT )
		return E_POINTER;

	HRESULT hr = S_OK;

	LPWAVEFORMATEX pwfxSrc = NULL;
	LPWAVEFORMATEX pwfxDest = NULL;

//	LPDSMXVOLUME pVolume = NULL;

	LPXMEDIAPACKET pInputBuffer = NULL;
	LPXMEDIAPACKET pAccumBuffer = NULL;
	LPXMEDIAPACKET pOutputBuffer = NULL;

//	LPDIRECTSOUNDMIXERSOURCE pMixSrc = NULL;
//	LPDIRECTSOUNDMIXERDESTINATION pMixDest = NULL;

	LPVOID pvInputBuffer = NULL;
	LPVOID pvOutputBuffer = NULL;
	LPVOID pvAccumBuffer = NULL;

	ALLOCATEANDCHECKSTRUCT( pwfxSrc, WAVEFORMATEX );
	ALLOCATEANDCHECKSTRUCT( pwfxDest, WAVEFORMATEX );

//	ALLOCATEANDCHECKSTRUCT( pVolume, DSMXVOLUME );

	ALLOCATEANDCHECKSTRUCT( pInputBuffer, XMEDIAPACKET );
	ALLOCATEANDCHECKSTRUCT( pAccumBuffer, XMEDIAPACKET );
	ALLOCATEANDCHECKSTRUCT( pOutputBuffer, XMEDIAPACKET );

	CHECK( pvInputBuffer = new BYTE[pMT->dwInputBufferSize] );
	CHECKALLOC( pvInputBuffer );	
	CHECK( memset( pvInputBuffer, pMT->dwInputStart, sizeof( BYTE ) * pMT->dwInputBufferSize ) );

	CHECK( pvAccumBuffer = new LONG[pMT->dwAccumBufferSize] );
	CHECKALLOC( pvAccumBuffer );
	CHECK( memset( pvAccumBuffer, 0, sizeof( LONG ) * pMT->dwAccumBufferSize ) );

	CHECK( pvOutputBuffer = new BYTE[pMT->dwOutputBufferSize] );
	CHECKALLOC( pvOutputBuffer );
	CHECK( memset( pvOutputBuffer, pMT->dwOutputStart, sizeof( BYTE ) * pMT->dwOutputBufferSize ) );

//	CHECK( InitVolume( pVolume, pMT->wLeft, pMT->wRight ) );

	CHECK( InitWaveFormatEx( pwfxSrc, pMT->wSourceFormatTag, pMT->nSourceChannels, pMT->nSourceSamplesPerSec, pMT->nSourceAvgBytesPerSec, pMT->nSourceBlockAlign, pMT->wSourceBitsPerSample, pMT->cbSourceSize ) );
	CHECK( InitWaveFormatEx( pwfxDest, pMT->wDestFormatTag, pMT->nDestChannels, pMT->nDestSamplesPerSec, pMT->nDestAvgBytesPerSec, pMT->nDestBlockAlign, pMT->wDestBitsPerSample, pMT->cbDestSize ) );

	CHECK( InitXMEDIAPACKET( pInputBuffer, pvInputBuffer, pMT->dwInputBufferSize, NULL, NULL, NULL, 0 ) );
	CHECK( InitXMEDIAPACKET( pAccumBuffer, pvAccumBuffer, pMT->dwAccumBufferSize, NULL, NULL, NULL, 0 ) );
	CHECK( InitXMEDIAPACKET( pOutputBuffer, pvOutputBuffer, pMT->dwOutputBufferSize, NULL, NULL, NULL, 0 ) );

	CHECK( InitAccumBuffer( pAccumBuffer, pMT->lAccumStart ) );

	CHECKEXECUTE( DirectSoundCreateMixerSource( pwfxSrc, pwfxDest, pVolume, &pMixSrc ) );
	CHECKALLOC( pMixSrc );

	CHECKEXECUTE( DirectSoundCreateMixerDestination( pwfxDest, &pMixDest ) );
	CHECKALLOC( pMixDest );

//	CHECK( PrintBuffers( pInputBuffer, pAccumBuffer, pOutputBuffer, pwfxSrc, pwfxDest ) );	
	CHECKEXECUTE( pMixSrc->Process( pInputBuffer, pAccumBuffer ) );
//	CHECK( PrintBuffers( pInputBuffer, pAccumBuffer, pOutputBuffer, pwfxSrc, pwfxDest ) );	
	CHECKEXECUTE( pMixDest->Process( pAccumBuffer, pOutputBuffer ) );
//	CHECK( PrintBuffers( pInputBuffer, pAccumBuffer, pOutputBuffer, pwfxSrc, pwfxDest ) );

	CHECKEXECUTE( VerifyResult( pMT, pwfxDest, pOutputBuffer, dwCount ) ); 

	RELEASE( pMixSrc );
	RELEASE( pMixDest );

	delete [] pvInputBuffer;
	delete [] pvAccumBuffer;
	delete [] pvOutputBuffer;

	return hr;
}
*/
void RunTest( void )
{
	HRESULT hr = S_OK;
	MixerTest mt;
	ZeroMemory( &mt, sizeof( MixerTest ) );

	DWORD dw = 0;

	for ( DWORD i = 0; i < NUMELEMS( g_wfx ); i++ )
	{
		for ( DWORD j = 0; j < NUMELEMS( g_wfx ); j++ )
		{
			for ( DWORD k = 0; k < NUMELEMS( g_wVolumes ); k++ )
			{
		//		for ( DWORD l = 0; l < NUMELEMS( g_wVolumes ); l++ )
		//		{
					for ( DWORD m = 0; m < NUMELEMS( g_dwStartValues ); m++ )
					{
						for ( DWORD n = 0; n < NUMELEMS( g_dwStartAccumValues ); n++ )
						{
							for ( DWORD o = 0; o < NUMELEMS( g_dwStartValues ); o++ )
							{
	//							InitTestMixer( &mt, &g_wfx[i], &g_wfx[j], g_wVolumes[k], g_wVolumes[/*l*/k], g_dwStartValues[m], g_dwStartValues[n], g_dwStartValues[o] );
	//							EXECUTE( TestMixer( &mt , dw++ ) );
							}
						}
					}
			//	}
			}
		}
	}
}

VOID WINAPI MixerStartTest( HANDLE LogHandle )
{
	SETLOG( LogHandle, "danrose", "DSOUND", "Mixer", "MixerTests" );

//	RunTest();
}

VOID WINAPI MixerEndTest( VOID )
{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( mixer )
#pragma data_seg()

BEGIN_EXPORT_TABLE( mixer )
    EXPORT_TABLE_ENTRY( "StartTest", MixerStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", MixerEndTest )
END_EXPORT_TABLE( mixer )