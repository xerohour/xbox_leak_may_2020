#include <dsutil.h>
#include <netXmo.h>
#include <echo.h>

#include <xnetref.h>

static const ULONG PACKET_COUNT       = 1;
static const ULONG PACKET_SIZE        = 1;
static const DWORD INVALID_PORT       = 0xFFFFFFFF;
static const WORD  SAMPLES_PER_BLOCK  = 1024;
static const ULONG TEST_PORT          = 600;
static const LPCSTR TEST_ADDRESS      = "157.56.11.66";

#define IMAADPCM_BITS_PER_SAMPLE            4
#define IMAADPCM_HEADER_LENGTH              4

struct PacketContext {
	XMEDIAPACKET axmp[2];
	DWORD adwCompletedSize[2];
	DWORD adwStatus[2];
	HANDLE ahEvent[2];

	XMEDIAINFO xmi;
};

enum XMO { 
//	XMO_FILE_READ = 0,
//	XMO_FILE_WRITE,
//	XMO_FILE_READ_WRITE,
	XMO_WAVE_FILE = 0,
	XMO_WMA_FILE,
//	XMO_ECHO_FILTER,
//	XMO_HAWK_HEAD,
//	XMO_HAWK_MIC,
//	XMO_VOX_ENC,
//	XMO_VOX_DEC,
//	XMO_NET,
//	XMO_MIXER_SRC,
//	XMO_MIXER_DEST,
//	XMO_WMA_MEMORY,
	XMO_AC97_ANALOG,
	XMO_AC97_DIGITAL,
	XMO_DSOUND
};

DWORD WmaCallback( LPVOID pvContext, DWORD dwOffset, DWORD dwByteCount, LPVOID *ppvData )
{
	return dwByteCount;
}

HRESULT OpenNet( void )
{
	HRESULT hr = S_OK;
	WSADATA wsadata;

	CHECK( XNetAddRef() );
	CHECK( WSAStartup( WINSOCK_VERSION, &wsadata ) );

	return hr;
}

HRESULT CloseNet( void )
{
	HRESULT hr = S_OK;

	CHECK( WSACleanup() );
	CHECK( XNetRelease() );

	return hr;
}

HRESULT InitWaveFormat( LPWAVEFORMATEX* ppFormat )
{
	if ( NULL == ppFormat )
		return E_INVALIDARG;

	LPWAVEFORMATEX pFormat = new WAVEFORMATEX;
	
	if ( NULL == pFormat )
		return E_OUTOFMEMORY;

	ZeroMemory( pFormat, sizeof( WAVEFORMATEX ) );

    pFormat->wFormatTag          = WAVE_FORMAT_PCM;
    pFormat->nSamplesPerSec      = 16000;
    pFormat->nChannels           = 1;
    pFormat->wBitsPerSample      = (WORD) 16;
    pFormat->nBlockAlign         = pFormat->wBitsPerSample * pFormat->nChannels / 8;
    pFormat->nAvgBytesPerSec     = pFormat->nSamplesPerSec * pFormat->nBlockAlign;
    pFormat->cbSize              = 0;

	*ppFormat = pFormat;

	return S_OK;
}

WORD CalculateEncodeAlignment
(
    WORD                    nChannels,
    WORD                    nSamplesPerBlock
)
{
    const WORD              nEncodedSampleBits  = nChannels * IMAADPCM_BITS_PER_SAMPLE;
    const WORD              nHeaderBytes        = nChannels * IMAADPCM_HEADER_LENGTH;
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

HRESULT InitAdpcmWaveFormat
(
	LPXBOXADPCMWAVEFORMAT*   ppwfx
)

{
	if ( NULL == ppwfx )
		return E_INVALIDARG;

	HRESULT hr = S_OK;

	LPXBOXADPCMWAVEFORMAT pwfx = new XBOXADPCMWAVEFORMAT;
	CHECKALLOC( pwfx );

	if ( SUCCEEDED( hr ) )
	{

		pwfx->wfx.wFormatTag = WAVE_FORMAT_XBOX_ADPCM;
		pwfx->wfx.nChannels = 1;
		pwfx->wfx.nSamplesPerSec = 16000;
		pwfx->wfx.nBlockAlign = CalculateEncodeAlignment(pwfx->wfx.nChannels, SAMPLES_PER_BLOCK );
		pwfx->wfx.nAvgBytesPerSec = pwfx->wfx.nSamplesPerSec * pwfx->wfx.nBlockAlign / SAMPLES_PER_BLOCK;
		pwfx->wfx.wBitsPerSample = IMAADPCM_BITS_PER_SAMPLE;
		pwfx->wfx.cbSize = sizeof(*pwfx) - sizeof(pwfx->wfx);
		pwfx->wSamplesPerBlock = SAMPLES_PER_BLOCK;

		*ppwfx = pwfx;
	}

	return hr;
}
/*
HRESULT InitVolume( LPDSMXVOLUME* ppVol )
{
	if ( NULL == ppVol )
		return E_INVALIDARG;

	LPDSMXVOLUME pVol = new DSMXVOLUME;

	if ( NULL == pVol )
		return E_OUTOFMEMORY;

	ZeroMemory( pVol, sizeof( DSMXVOLUME ) );

	pVol->wLeft = 0x8000;
	pVol->wRight = 0x8000;

	*ppVol = pVol;

	return S_OK;
}
*/
HRESULT InitStream( LPDSSTREAMDESC* ppDesc, LPCWAVEFORMATEX pFormat )
{
	if ( NULL == ppDesc || NULL == pFormat )
		return E_INVALIDARG;

	LPDSSTREAMDESC pDesc = new DSSTREAMDESC;

	if ( NULL == pDesc )
		return E_OUTOFMEMORY;

	ZeroMemory( pDesc, sizeof( DSSTREAMDESC ) );

    pDesc->dwFlags = 0;
    pDesc->dwMaxAttachedPackets = PACKET_COUNT;
    pDesc->lpwfxFormat = (LPWAVEFORMATEX) pFormat;

	*ppDesc = pDesc;

	return S_OK;
}

DWORD GetHawkPort( void )
{
	DWORD dwCount = 0;
	DWORD dwInsertions = 0;
    DWORD dwSlot = 0;
    DWORD dwPort = INVALID_PORT;

    while ( dwCount++ < 2 ) {

        dwInsertions = XGetDevices( XDEVICE_TYPE_VOICE_MICROPHONE );

        if( dwInsertions )
        {
            for( DWORD i = 0; i < 32; i++ )
            {
                if( dwInsertions & ( 1 << i ) )
                {
                    if( i < 16 )
                    {
                        dwPort = i;
                        dwSlot = XDEVICE_TOP_SLOT;
                    } else
                    {
                        dwPort = i-16;
                        dwSlot = XDEVICE_BOTTOM_SLOT;
                    }

                    dwCount = 2;
                    break;
                }   
            }
        }

        Sleep( 1000 );
    }

	return dwPort;
}

HRESULT InitPacketContext( PacketContext* pContext, bool bEmpty )
{
	if ( NULL == pContext )
		return E_INVALIDARG;

	HRESULT hr = S_OK;

	ZeroMemory( pContext, sizeof( PacketContext ) );

	ZeroMemory( pContext->axmp, sizeof( XMEDIAPACKET ) * 2 );
	ZeroMemory( pContext->adwCompletedSize, sizeof( DWORD ) * 2 );
	ZeroMemory( pContext->adwStatus, sizeof( DWORD ) * 2 );
	ZeroMemory( pContext->ahEvent, sizeof( HANDLE ) * 2 );
	ZeroMemory( &pContext->xmi, sizeof( XMEDIAINFO ) );

	for ( DWORD i = 0; i < 2 && SUCCEEDED( hr ) && false == bEmpty; i++ )
	{
		pContext->ahEvent[i] = CreateEvent( NULL, FALSE, FALSE, NULL );
		CHECKALLOC( pContext->ahEvent[i] );

		if ( SUCCEEDED( hr ) )
		{
			pContext->axmp[i].pdwStatus = &pContext->adwStatus[i];
			pContext->axmp[i].pdwCompletedSize = &pContext->adwCompletedSize[i];
			pContext->axmp[i].hCompletionEvent = pContext->ahEvent[i];
		}
	}

	return hr;
}

void DestroyPacketContext( PacketContext* pContext )
{
	for ( DWORD i = 0; i < 2; i++ )
	{
		if ( pContext->axmp[i].pvBuffer )
//#ifndef DVTSNOOPBUG
//			delete [] pContext->axmp[i].pvBuffer;
//#else // DVTSNOOPBUG
			XPhysicalFree( pContext->axmp[i].pvBuffer );
//#endif // DVTSNOOPBUG

		CloseHandle( pContext->ahEvent[i] );
	}
}

HRESULT GetStatus( XMediaObject* pXMO )
{
	HRESULT hr = S_OK;
	DWORD dwStatus = 0;
	char szTemp[512];

	CHECKEXECUTE( pXMO->GetStatus( &dwStatus ) );

	if ( SUCCEEDED( hr ) )
	{
		DbgPrint( "GetStatus:\n" );
		sprintf( szTemp, "  dwStatus: " );

		if ( 0 == dwStatus )
		{
			strcat( szTemp, "0x0" );
		}

		for ( DWORD dwMask = 0x00000001; 0 != dwMask; dwMask <<= 1 )
		{
			switch ( dwMask & dwStatus )
			{
				case XMO_STATUSF_ACCEPT_INPUT_DATA:
					strcat( szTemp, "XMO_STATUSF_ACCEPT_INPUT_DATA | " );
					break;

				case XMO_STATUSF_ACCEPT_OUTPUT_DATA:
					strcat( szTemp, "XMO_STATUSF_ACCEPT_OUTPUT_DATA | " );
					break;
			}
		}

		DbgPrint( "%s\n\n", szTemp );
	}

	return hr;
}

HRESULT InitBuffers( PacketContext* pContext )
{
	HRESULT hr = S_OK;

	for ( DWORD i = 0; i < 2 && SUCCEEDED( hr ); i++ )
	{
		pContext->axmp[i].dwMaxSize = 0 == i ? pContext->xmi.dwInputSize : pContext->xmi.dwOutputSize;

		if ( ( 0 == i && 0 == pContext->xmi.dwInputSize ) || ( 1 == i && 0 == pContext->xmi.dwOutputSize ) )
		{
			pContext->axmp[i].pvBuffer = NULL;
		}
		else
		{
//#ifndef DVTSNOOPBUG
//			pContext->axmp[i].pvBuffer = new BYTE[ 0 == i ? pContext->xmi.dwInputSize : pContext->xmi.dwOutputSize ];
//#else
			pContext->axmp[i].pvBuffer = XPhysicalAlloc( 0 == i ? pContext->xmi.dwInputSize : pContext->xmi.dwOutputSize, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE );
//#endif
			CHECKALLOC( pContext->axmp[i].pvBuffer );

			ZeroMemory( pContext->axmp[i].pvBuffer, 0 == i ? pContext->xmi.dwInputSize : pContext->xmi.dwOutputSize );
		}
	}

	return hr;
}

HRESULT GetInfo( XMediaObject* pXMO, PacketContext* pContext )
{
	HRESULT hr = S_OK;
	char szTemp[512];

	CHECKEXECUTE( pXMO->GetInfo( &pContext->xmi ) );

	if ( SUCCEEDED( hr ) )
	{
		DbgPrint( "GetInfo:\n" );
		DbgPrint( "  xmi: InputSize: %u OutputSize: %u MaxLookAhead: %u\n", pContext->xmi.dwInputSize, pContext->xmi.dwOutputSize, pContext->xmi.dwMaxLookahead ); 
		sprintf( szTemp, "       Flags: " );

		if ( 0 == pContext->xmi.dwFlags )
		{
			strcat( szTemp, "0x0" );
		}

		for ( DWORD dwMask = 0x000000001; 0 != dwMask; dwMask <<= 1 )
		{
			switch ( pContext->xmi.dwFlags & dwMask )
			{
				case XMO_STREAMF_FIXED_SAMPLE_SIZE:
					strcat( szTemp, "XMO_STREAMF_FIXED_SAMPLE_SIZE | " );
					break;

				case XMO_STREAMF_FIXED_PACKET_ALIGNMENT:
					strcat( szTemp, "XMO_STREAMF_FIXED_PACKET_ALIGNMENT | " );
					break;

				case XMO_STREAMF_INPUT_ASYNC:
					strcat( szTemp, "XMO_STREAMF_INPUT_ASYNC | " );
					break;

				case XMO_STREAMF_OUTPUT_ASYNC:
					strcat( szTemp, "XMO_STREAMF_OUTPUT_ASYNC | " );
					break;

				case XMO_STREAMF_IN_PLACE:
					strcat( szTemp, "XMO_STREAMF_IN_PLACE | " );
					break;
			}
		}
		
		DbgPrint( "%s\n\n", szTemp );
	}

	CHECKEXECUTE( InitBuffers( pContext ) );

	return hr;
}

HRESULT Process( XMediaObject* pXMO, PacketContext* pContext )
{
	HRESULT hr = S_OK;
	char szTemp[512];
	char szSize[64];

	CHECKEXECUTE( pXMO->Process( NULL == pContext->axmp[0].pvBuffer ? NULL : &pContext->axmp[0], NULL == pContext->axmp[1].pvBuffer ? NULL : &pContext->axmp[1] ) );
	
	if ( SUCCEEDED( hr ) )
	{
		DbgPrint( "Process:\n" );

		for ( DWORD i = 0; i < 2; i++ )
		{
			sprintf( szTemp, "  packet[%u]: pBuffer: 0x%x MaxSize: %u ", i, pContext->axmp[i].pvBuffer, pContext->axmp[i].dwMaxSize );
			
			if ( NULL != pContext->axmp[i].pdwCompletedSize )
			{
				sprintf( szSize, "Completed: %u ", *pContext->axmp[i].pdwCompletedSize );
				strcat( szTemp, szSize );
			}

			if ( NULL != pContext->axmp[i].pdwStatus )
			{
				strcat( szTemp, "Status: " );
				switch ( *pContext->axmp[i].pdwStatus )
				{
					case XMEDIAPACKET_STATUS_SUCCESS:
						strcat( szTemp, "XMEDIAPACKET_STATUS_SUCCESS" );
						break;

					case XMEDIAPACKET_STATUS_PENDING:
						strcat( szTemp, "XMEDIAPACKET_STATUS_PENDING" );
						break;

					case XMEDIAPACKET_STATUS_FLUSHED:
						strcat( szTemp, "XMEDIAPACKET_STATUS_FLUSHED" );
						break;

					case XMEDIAPACKET_STATUS_FAILURE:
						strcat( szTemp, "XMEDIAPACKET_STATUS_FAILURE" );
						break;
				}
			}

			DbgPrint( "%s\n", szTemp );
			
		}

		DbgPrint( "\n" );
	}

	return hr;
}

HRESULT Callback( PacketContext* pContext )
{
	HRESULT hr = S_OK;

	DWORD i = 0;
	DWORD dwNumToWait = 2;
	
	do {

		i = WaitForMultipleObjects( 2, pContext->ahEvent, FALSE, 10000 );

		if ( WAIT_OBJECT_0 == i )
		{
			DbgPrint( "Recieved callback on input\n" );
		}
		else if ( WAIT_TIMEOUT != i )
		{
			DbgPrint( "Recieved callback on output\n" );
		}
		else
		{
			DbgPrint( "Wait timed out on callback\n" );
		}

		if ( NULL == pContext->axmp[0].pvBuffer || NULL == pContext->axmp[1].pvBuffer )
		{
			dwNumToWait = 0;
		}
		else
		{
			dwNumToWait--;
		}

	} while ( 0 != dwNumToWait );

	DbgPrint( "\n" );

	return hr;
}

HRESULT CreateXMO( XMO xmo, XMediaObject** ppXMO )
{
	HRESULT hr = S_OK;
	LPCWAVEFORMATEX pWfxRaw = NULL;
	LPXBOXADPCMWAVEFORMAT pWfxAdpcm = NULL;
	LPWAVEFORMATEX pWfx = NULL;
//	LPDSMXVOLUME pVol = NULL;
	LPDSSTREAMDESC pdsDesc = NULL;

	DWORD dwOffset = 0;

	char* szName = NULL;

	switch ( xmo )
	{
/*		case XMO_FILE_READ:
			CHECKEXECUTE( XFileCreateMediaObject( "t:\\media\\audio\\pcm\\2000.wav", GENERIC_READ, FILE_SHARE_READ, OPEN_ALWAYS, 0, (XFileMediaObject**) ppXMO ) );
			szName = "XFileCreateMediaObject - Read";
			break;

		case XMO_FILE_WRITE:
			CHECKEXECUTE( XFileCreateMediaObject( "t:\\media\\audio\\pcm\\dumb.wav", GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS, 0, (XFileMediaObject**) ppXMO ) );
			szName = "XFileCreateMediaObject - Write";
			break;

		case XMO_FILE_READ_WRITE:
			CHECKEXECUTE( XFileCreateMediaObject( "t:\\media\\audio\\pcm\\dumb.wav", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, CREATE_ALWAYS, 0, (XFileMediaObject**) ppXMO ) );
			szName = "XFileCreateMediaObject - Read + Write";
			break;
*/
		case XMO_WAVE_FILE:
			CHECKEXECUTE( XWaveFileCreateMediaObject( "t:\\media\\audio\\pcm\\2111.wav", &pWfxRaw, (XFileMediaObject**) ppXMO ) );
			szName = "XWaveFileCreateMediaObject";
			break;

		case XMO_WMA_FILE:
			CHECKEXECUTE( InitWaveFormat( &pWfx ) );
			CHECKEXECUTE( WmaCreateDecoder( "t:\\media\\audio\\wma\\test.wma", NULL, FALSE, (4096*16), 16, 0, pWfx, (XFileMediaObject**) ppXMO ) );
			szName = "WmaCreateDecoder";
			break;

/*		case XMO_ECHO_FILTER:
			CHECKEXECUTE( XCreateEchoFxMediaObject( PACKET_SIZE, 16 >> 3, ppXMO ) );
			szName = "XCreateEchoFxMediaObject";
			break;
*/
/*		case XMO_HAWK_HEAD:
			CHECKEXECUTE( InitWaveFormat( &pWfx ) );
			CHECKEXECUTE( XVoiceCreateMediaObject( XDEVICE_TYPE_VOICE_HEADPHONE, GetHawkPort(), PACKET_COUNT, pWfx, ppXMO ) );
			szName = "XVoiceCreateMediaObject - Head";
			break;

		case XMO_HAWK_MIC:
			CHECKEXECUTE( InitWaveFormat( &pWfx ) );
			CHECKEXECUTE( XVoiceCreateMediaObject( XDEVICE_TYPE_VOICE_MICROPHONE, GetHawkPort(), PACKET_COUNT, pWfx, ppXMO ) );
			szName = "XVoiceCreateMediaObject - Mic";
			break;
*/
//		case XMO_VOX_ENC:
//		case XMO_VOX_DEC:
//			break;

/*		case XMO_NET:
			CHECKEXECUTE( XnetCreateMediaObject( ppXMO, INVALID_SOCKET, TEST_ADDRESS, TEST_PORT, PACKET_SIZE, 0 ) );
			szName = "XNetCreateMediaObject";
			break;
*/
/*
		case XMO_MIXER_SRC:
			CHECKEXECUTE( InitWaveFormat( &pWfx ) );
			CHECKEXECUTE( InitVolume( &pVol ) );
			CHECKEXECUTE( DirectSoundCreateMixerSource( pWfx, pWfx, pVol, (IDirectSoundMixerSource**) ppXMO ) );
			szName = "DirectSoundCreateMixerSource";
			break;


		case XMO_MIXER_DEST:
			CHECKEXECUTE( InitWaveFormat( &pWfx ) );
			CHECKEXECUTE( DirectSoundCreateMixerDestination( pWfx, (IDirectSoundMixerDestination**) ppXMO ) );
			szName = "DirectSoundCreateMixerDestination";
			break;
*/
/*		case XMO_WMA_MEMORY:
			CHECKEXECUTE( InitWaveFormat( &pWfx ) );
			CHECKEXECUTE( WmaCreateInMemoryDecoder( WmaCallback, NULL, pWfx, ppXMO ) );
			szName = "WmaCreateInMemoryDecoder";
			break;
*/

		case XMO_AC97_ANALOG:
			CHECKEXECUTE( Ac97CreateMediaObject( DSAC97_CHANNEL_ANALOG, NULL, NULL, (LPAC97MEDIAOBJECT *) ppXMO ) );
			szName = "Ac97CreateMediaObject - ANALOG";
			break;

		case XMO_AC97_DIGITAL:
			CHECKEXECUTE( Ac97CreateMediaObject( DSAC97_CHANNEL_DIGITAL, NULL, NULL, (LPAC97MEDIAOBJECT *) ppXMO ) );
			szName = "Ac97CreateMediaObject - DIGITAL";
			break;

		case XMO_DSOUND:
			CHECKEXECUTE( InitWaveFormat( &pWfx ) );
			CHECKEXECUTE( InitStream( &pdsDesc, pWfx ) );
			CHECKEXECUTE( DirectSoundCreateStream( pdsDesc, (LPDIRECTSOUNDSTREAM*) ppXMO ) );
			szName = "DirectSoundCreateStream";
			break;
	}

	CHECKALLOC( *ppXMO );

	if ( SUCCEEDED( hr ) )
	{
		DbgPrint( "\n" );
		DbgPrint( "%s:\n", szName );
		
		if ( NULL != pWfxRaw )
		{
			DbgPrint( "  WaveFormat: FormatTag: %u Channels: %u SamplesPerSec: %u AvgBytesPerSec: %u BlockAlign: %u BitsPerSample: %u Size %u\n", pWfxRaw->wFormatTag, pWfxRaw->nChannels, pWfxRaw->nSamplesPerSec, pWfxRaw->nAvgBytesPerSec, pWfxRaw->nBlockAlign, pWfxRaw->wBitsPerSample, pWfxRaw->cbSize );
		}

		if ( NULL != pWfx )
		{
			DbgPrint( "  WaveFormat: FormatTag: %u Channels: %u SamplesPerSec: %u AvgBytesPerSec: %u BlockAlign: %u BitsPerSample: %u Size %u\n", pWfx->wFormatTag, pWfx->nChannels, pWfx->nSamplesPerSec, pWfx->nAvgBytesPerSec, pWfx->nBlockAlign, pWfx->wBitsPerSample, pWfx->cbSize );
		}

		if ( NULL != pWfxAdpcm )
		{
			DbgPrint( "  ADPCMWaveFormat: FormatTag: %u Channels: %u SamplesPerSec: %u AvgBytesPerSec: %u BlockAlign: %u BitsPerSample: %u Size %u SamplesPerBlock: 0x%x\n", pWfxAdpcm->wfx.wFormatTag, pWfxAdpcm->wfx.nChannels, pWfxAdpcm->wfx.nSamplesPerSec, pWfxAdpcm->wfx.nAvgBytesPerSec, pWfxAdpcm->wfx.nBlockAlign, pWfxAdpcm->wfx.wBitsPerSample, pWfxAdpcm->wfx.cbSize, pWfxAdpcm->wSamplesPerBlock );
		}

/*		if ( NULL != pVol )
		{
			DbgPrint( "  Volume: Left: 0x%x Right 0x%x\n", pVol->wLeft, pVol->wRight );
		}
*/
		if ( NULL != pdsDesc )
		{
			DbgPrint( "  DSDESC: Flags: 0x%x\n", pdsDesc->dwFlags );
		}

		DbgPrint( "\n" );
	}

	delete pWfx;
	delete pWfxAdpcm;
//	delete pVol;
	delete pdsDesc;

	return hr;
}

HRESULT GetCompletedValue( PacketContext* pContext )
{
	if ( NULL == pContext )
		return E_INVALIDARG;

	HRESULT hr = S_OK;

	DbgPrint( "GetCompletedValue:\n" );
	DbgPrint( "  Completed Value[0]: %u\n", pContext->adwCompletedSize[0] );
	DbgPrint( "  Completed Value[1]: %u\n", pContext->adwCompletedSize[1] );
	DbgPrint( "  Status[0]: 0x%x\n", pContext->adwStatus[0] );
	DbgPrint( "  Status[1]: 0x%x\n", pContext->adwStatus[1] );
	DbgPrint( "\n" );

	return hr;
}

HRESULT TestXMO( XMO xmo )
{
	HRESULT hr = S_OK;

	PacketContext context;
	XMediaObject* pXMO = NULL;

	ZeroMemory( &context, sizeof( PacketContext ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "CreateXMO" );
	CHECKEXECUTE( CreateXMO( xmo, &pXMO ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "InitPacketContext" );
	CHECKEXECUTE( InitPacketContext( &context, false ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "GetStatus" );
	CHECKEXECUTE( GetStatus( pXMO ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "GetInfo" );
	CHECKEXECUTE( GetInfo( pXMO, &context ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "Process" );
	CHECKEXECUTE( Process( pXMO, &context ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "GetStatus" );
	CHECKEXECUTE( GetStatus( pXMO ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "Callback" );
	CHECKEXECUTE( Callback( &context ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "GetCompletedValue" );
	CHECKEXECUTE( GetCompletedValue( &context ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "GetStatus" );
	CHECKEXECUTE( GetStatus( pXMO ) );

	DestroyPacketContext( &context );
	RELEASE( pXMO );

	return hr;
}

HRESULT TestXMOEmptyPacket( XMO xmo )
{
	HRESULT hr = S_OK;

	PacketContext context;
	XMediaObject* pXMO = NULL;

	ZeroMemory( &context, sizeof( PacketContext ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "CreateXMO" );
	CHECKEXECUTE( CreateXMO( xmo, &pXMO ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "InitPacketContext" );
	CHECKEXECUTE( InitPacketContext( &context, true ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "GetInfo" );
	CHECKEXECUTE( GetInfo( pXMO, &context ) );

	SETLOG( g_hLog, "danrose", "XMO", "Regression", "Process" );
	CHECKEXECUTE( Process( pXMO, &context ) );

	DestroyPacketContext( &context );
	RELEASE( pXMO );

	return hr;
}

VOID WINAPI XMORegStartTest( HANDLE LogHandle )
{
	SETLOG( LogHandle, "danrose", "XMO", "Regression", "StartTest" );

	HRESULT hr = S_OK;

//	XMO Regression currently does nothing with the network.
//	However, it may need to in the future.
//	At such time, OpenNet and CloseNet should be uncommented.
//	CHECK( OpenNet() );

	for ( DWORD xmo = XMO_WAVE_FILE; xmo <= XMO_DSOUND; xmo++ )
//	for ( DWORD xmo = XMO_WMA_FILE; xmo <= XMO_WMA_FILE; xmo++ )
	{
		EXECUTE( TestXMO( (XMO) xmo ) );
		EXECUTE( TestXMOEmptyPacket( (XMO) xmo ) );
	}

//	XMO Regression currently does nothing with the network.
//	However, it may need to in the future.
//	At such time, OpenNet and CloseNet should be uncommented.
//	CHECK( CloseNet() );
}

VOID WINAPI XMORegEndTest( VOID )
{
}

//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( xmoReg )
#pragma data_seg()

BEGIN_EXPORT_TABLE( xmoReg )
    EXPORT_TABLE_ENTRY( "StartTest", XMORegStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XMORegEndTest )
END_EXPORT_TABLE( xmoReg )
