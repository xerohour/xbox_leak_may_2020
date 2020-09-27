#include <xtl.h>
#include <xdbg.h>
#include <macros.h>
#include <waveldr.h>
#include <srcxmo.h>
#include <netxmo.h>

static const DWORD XMO_NULL           = 0x00000000;
static const DWORD XMO_HAWK_MIC       = 0x00000001;
static const DWORD XMO_NET_RECV       = 0x00000002;
static const DWORD XMO_WAVE_FILE      = 0x00000004;
static const DWORD XMO_SRC_16_8       = 0x00000008;
static const DWORD XMO_VOX_ENC        = 0x00000010;
static const DWORD XMO_VOX_DEC        = 0x00000020;
static const DWORD XMO_ADPCM_ENC      = 0x00000040;
static const DWORD XMO_ADPCM_DEC      = 0x00000080;
static const DWORD XMO_SRC_8_16       = 0x00000100;
static const DWORD XMO_HAWK_HEAD      = 0x00000200;
static const DWORD XMO_NET_SEND       = 0x00000400;
static const DWORD XMO_MCPX           = 0x00000800;

static const DWORD FREE_SRC           = 0x00000000;
static const DWORD FREE_DEST          = 0x00000001;
static const DWORD PENDING_SRC        = 0x00000002;
static const DWORD PENDING_DEST       = 0x00000004;

static const WORD SAMPLES_PER_BLOCK   = 0x0800;
static const ULONG PACKET_COUNT       = 3;
static const ULONG PACKET_SIZE        = 1024;
static const ULONG NUM_QUEUES         = 2;
static const ULONG NUM_EVENTS         = NUM_QUEUES * PACKET_COUNT;
static const ULONG TEST_PORT          = 600;
static const DWORD INVALID_PORT       = 0xFFFFFFFF;
static const LPCWSTR FILE_NAME        = L"t:\\media\\audio\\pcm\\1306.wav";
static const LPCSTR TEST_ADDR_LEFT    = "157.56.10.245";
static const LPCSTR TEST_ADDR_RIGHT   = "157.56.11.66";

static LPSTR XMO_NAME_NET_SEND = "XMO_NET_SEND";
static LPSTR XMO_NAME_NET_RECV = "XMO_NET_RECV";
static LPSTR XMO_NAME_HAWK_MIC = "XMO_HAWK_MIC";
static LPSTR XMO_NAME_HAWK_HEAD = "XMO_HAWK_HEAD";

struct List {
	XMediaObject* pXMO;
	char* szName;
	List* pNext;
};

static HANDLE g_hEvent = NULL;

struct ProcessContext {
	List* pXMOList;
	List* pLastXMO;

//	DWORD dwNumDest[PACKET_COUNT];
//	DWORD dwNumQueued;
//	DWORD dwStartQueue;
	DWORD dwBytesPerQueue;
	DWORD dwPacketSize;

	DWORD pdwStatus[NUM_EVENTS];
	DWORD pdwContextStatus[NUM_EVENTS];
	DWORD pdwCompletedSize[NUM_EVENTS];

//	DWORD dwInStatus;
//	DWORD dwOutStatus;
//	DWORD dwInCompletedSize;
//	DWORD dwOutCompletedSize;

	LPBYTE pQueueBuffer;

	HANDLE hEvent;

//	HANDLE pPacketEvents[NUM_EVENTS];
	XMEDIAPACKET pPackets[NUM_EVENTS];
};

union ENCODEDFORMATEX {

	WAVEFORMATEX wfx;
	IMAADPCMWAVEFORMAT wfxAdpcm;
	VOICECODECWAVEFORMAT wfxVox;
};

HRESULT OpenNet( void )
{
	HRESULT hr = S_OK;
	WSADATA wsadata;

	CHECKEXECUTE( XnetInitialize( NULL, TRUE ) );
	CHECKEXECUTE( WSAStartup( WINSOCK_VERSION, &wsadata ) );

	return hr;
}

bool IsLeftMachine( void )
{
	char* szAddress = NULL;

    struct in_addr address;
	XnetGetIpAddress( &address );

	szAddress = inet_ntoa( address );

	return 0 == strcmp( TEST_ADDR_LEFT, szAddress );
}


HRESULT CloseNet( void )
{
	HRESULT hr = S_OK;

	CHECKEXECUTE( WSACleanup() );
	CHECKEXECUTE( XnetCleanup() );

	return hr;
}

void DeleteList( List* pList )
{
	List* pTemp = NULL;

	for ( ; NULL != pList; pList = pList->pNext )
	{
		if ( NULL != pTemp )
			delete pTemp;

		RELEASE( pList->pXMO );
		pTemp = pList;
	}

	delete pTemp;
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



HRESULT InsertList( List** ppList, XMediaObject* pXMO, char* szName )
{
	if ( NULL == ppList || NULL == pXMO )
		return E_INVALIDARG;

	HRESULT hr = S_OK;

	List* pTemp = NULL;
	List* pLast = *ppList;

	pTemp = new List;
	CHECKALLOC( pTemp );

	if ( SUCCEEDED( hr ) )
	{
		pTemp->pXMO = pXMO;
		pTemp->pNext = NULL;
		pTemp->szName = szName;
	}

	if ( SUCCEEDED( hr ) && NULL == pLast )
	{
		*ppList = pTemp;
	}

	else if ( SUCCEEDED( hr ) )
	{
		for( ; NULL != pLast->pNext; pLast = pLast->pNext )
		{
			// NO CODE HERE
		}

		pLast->pNext = pTemp;
	}

	return hr;
}

HRESULT InitWaveFormat( LPWAVEFORMATEX* ppFormat )
{
	if ( NULL == ppFormat )
		return E_INVALIDARG;

	LPWAVEFORMATEX pFormat = new WAVEFORMATEX;
	
	if ( NULL == pFormat )
		return E_OUTOFMEMORY;

    pFormat->wFormatTag          = WAVE_FORMAT_PCM;
    pFormat->nSamplesPerSec      = 16000;
    pFormat->nChannels           = 1;
    pFormat->wBitsPerSample      = (WORD) 16;
    pFormat->nBlockAlign         = (16 * pFormat->nChannels / 8);
    pFormat->nAvgBytesPerSec     = pFormat->nSamplesPerSec * pFormat->nBlockAlign;
    pFormat->cbSize              = 0;

	*ppFormat = pFormat;

	return S_OK;
}

HRESULT InitEncodedFormat( ENCODEDFORMATEX* pFormat, DWORD dwFlags )
{
	if ( NULL == pFormat )
		return E_INVALIDARG;

	if ( dwFlags & XMO_ADPCM_ENC || dwFlags & XMO_ADPCM_DEC )
	{
		IMAADPCMWAVEFORMAT* pwfx = &pFormat->wfxAdpcm;

		pwfx->wfx.wFormatTag = WAVE_FORMAT_IMA_ADPCM;
		pwfx->wfx.nChannels = 1;
		pwfx->wfx.nSamplesPerSec = 16000;
		pwfx->wfx.wBitsPerSample = 4;
		pwfx->wfx.nBlockAlign = (((SAMPLES_PER_BLOCK * 4) + 7) >> 3) + 4;
		pwfx->wfx.nAvgBytesPerSec = pwfx->wfx.nSamplesPerSec * pwfx->wfx.nBlockAlign / SAMPLES_PER_BLOCK;
		pwfx->wfx.cbSize = sizeof(*pwfx) - sizeof(pwfx->wfx);
		pwfx->wSamplesPerBlock = SAMPLES_PER_BLOCK;
	}

	// add code for vox

	return S_OK;
}



HRESULT InitStream( LPDSSTREAMDESC pDesc, LPCWAVEFORMATEX pFormat )
{
	if ( NULL == pDesc || NULL == pFormat )
		return E_INVALIDARG;

    pDesc->dwSize = sizeof( *pDesc );
    pDesc->dwFlags = 0;
    pDesc->dwMaxAttachedPackets = PACKET_COUNT;
    pDesc->lpwfxFormat = (LPWAVEFORMATEX) pFormat;

	return S_OK;
}

HRESULT CreateNetXMO( XMediaObject** pXMO )
{
	if ( NULL == pXMO )
		return E_INVALIDARG;

	HRESULT hr = S_OK;

	static XMediaObject* pNetXMO = NULL;

	if ( pNetXMO )
	{
		pNetXMO->AddRef();
	}

	else
	{
		CHECKEXECUTE( XnetCreateMediaObject( &pNetXMO, INVALID_SOCKET, IsLeftMachine() ? TEST_ADDR_RIGHT : TEST_ADDR_LEFT, TEST_PORT, PACKET_SIZE, 0 ) );
	}

	if ( SUCCEEDED( hr ) )
	{
		*pXMO = pNetXMO;
	}

	return hr;
}

HANDLE GetEvent( void )
{
	if ( NULL == g_hEvent )
	{
		g_hEvent = CreateEvent( NULL, FALSE, TRUE, NULL );
	}

	return g_hEvent;
}

void DestroyEvent( void )
{
	if ( NULL != g_hEvent )
	{
		CloseHandle( g_hEvent );
		g_hEvent = NULL;
	}
}

HRESULT CreateList( List** ppList, DWORD dwFlags )
{
	if ( NULL != *ppList )
		return E_INVALIDARG;

	HRESULT hr = S_OK;

	XMediaObject* pXMO = NULL;
	DWORD dwIndex = 0x00000001;

	LPCWAVEFORMATEX pwfxRaw = NULL;
	ENCODEDFORMATEX wfxEncoded;
	DSSTREAMDESC dsDesc;

	char * szName = NULL;

	ZeroMemory( &wfxEncoded, sizeof( ENCODEDFORMATEX ) );
	ZeroMemory( &dsDesc, sizeof( DSSTREAMDESC ) );

	CHECKEXECUTE( InitEncodedFormat( &wfxEncoded, dwFlags ) );

	while ( SUCCEEDED( hr ) && XMO_NULL != dwIndex )
	{
		switch ( dwFlags & dwIndex )
		{
			case XMO_HAWK_HEAD:
				szName = XMO_NAME_HAWK_HEAD;
				CHECKEXECUTE( XVoiceCreateMediaObject( XDEVICE_TYPE_VOICE_HEADPHONE, GetHawkPort(), PACKET_COUNT, (LPWAVEFORMATEX) pwfxRaw, &pXMO ) );
				break;

			case XMO_MCPX:
				CHECKEXECUTE( InitStream( &dsDesc, pwfxRaw ) );
				CHECKEXECUTE( DirectSoundCreateStream( DSDEVID_MCPX, &dsDesc, (LPDIRECTSOUNDSTREAM*) &pXMO, NULL ) );
				break;

			case XMO_HAWK_MIC:
				szName = XMO_NAME_HAWK_MIC;
				CHECKEXECUTE( InitWaveFormat( (LPWAVEFORMATEX*) &pwfxRaw ) );
				CHECKEXECUTE( XVoiceCreateMediaObject( XDEVICE_TYPE_VOICE_MICROPHONE, GetHawkPort(), PACKET_COUNT, (LPWAVEFORMATEX) pwfxRaw, &pXMO ) );
				break;

			case XMO_NET_SEND:
				szName = XMO_NAME_NET_SEND;
			case XMO_NET_RECV:
				szName = NULL == szName ? XMO_NAME_NET_RECV : szName;
				CHECKEXECUTE( InitWaveFormat( (LPWAVEFORMATEX*) &pwfxRaw ) );
				CHECKEXECUTE( CreateNetXMO( &pXMO ) );
				break;

			case XMO_WAVE_FILE:
				CHECKEXECUTE( XWaveFileCreateMediaObject( FILE_NAME, &pwfxRaw, (XFileMediaObject**) &pXMO ) );
				break;

			case XMO_SRC_16_8:
			case XMO_SRC_8_16:
			//	CHECKEXECUTE( XSrcCreateMediaObject( &pXMO, pwfxRaw ) );
				break;

			case XMO_VOX_ENC:
				break;

			case XMO_VOX_DEC:
				break;

			case XMO_ADPCM_ENC:
				CHECKEXECUTE( AdpcmCreateEncoder( (LPWAVEFORMATEX) pwfxRaw, SAMPLES_PER_BLOCK, &wfxEncoded.wfxAdpcm, &pXMO ) );
				break;

			case XMO_ADPCM_DEC:
				CHECKEXECUTE( AdpcmCreateDecoder( &wfxEncoded.wfxAdpcm, (LPWAVEFORMATEX) pwfxRaw, &pXMO ) );
				break;

			default:
				pXMO = NULL;
				szName = NULL;
				break;
		}

		if ( SUCCEEDED( hr ) && NULL != pXMO )
		{
			CHECKEXECUTE( InsertList( ppList, pXMO, szName ) );
		}	

		dwIndex <<= 1;
	}

	return hr;
}

DWORD CountList( List* pList )
{
	DWORD dwCount = 0;

	for ( ; NULL != pList; pList = pList->pNext )
	{
		dwCount++;
	}

	return dwCount;
}

HRESULT ConnectXMOs( List* pXMOList, LPDWORD pdwPacketSize )
{
	if ( NULL == pXMOList )
		return S_OK;

	if ( NULL == pdwPacketSize )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	XMEDIAINFO xMediaInfo;
	ZeroMemory( &xMediaInfo, sizeof( XMEDIAINFO ) );

	DWORD dwNumXMOs = CountList( pXMOList );
	DWORD dwMask = 1UL << (dwNumXMOs - 1);
	DWORD dwDone = 0x00000000;
	DWORD dwOldInputSize = 0;
	DWORD dwOldOutputSize = 0;
	DWORD dwCompleted = 0xFFFFFFFF << dwNumXMOs;

	*pdwPacketSize = PACKET_SIZE;

	while ( 0xFFFFFFFF != ( dwDone | dwCompleted ) ) 
	{
		for ( List* pList = pXMOList ; SUCCEEDED( hr ) && NULL != pList; pList = pList->pNext )
		{
			xMediaInfo.dwInputSize = xMediaInfo.dwOutputSize;
			xMediaInfo.dwOutputSize = pList == pXMOList ? *pdwPacketSize : 0;

			dwOldInputSize = xMediaInfo.dwInputSize;
			dwOldOutputSize = xMediaInfo.dwOutputSize;

			CHECKEXECUTE( pList->pXMO->GetInfo( &xMediaInfo ) );

			if ( ( pList == pXMOList && xMediaInfo.dwOutputSize == dwOldOutputSize ) || 
				 ( NULL == pList->pNext && xMediaInfo.dwInputSize == dwOldInputSize ) || 
				 ( xMediaInfo.dwInputSize == dwOldInputSize /*&& xMediaInfo.dwOutputSize == dwOldOutputSize*/ ) )
			{
				dwDone |= dwMask;
			}

			*pdwPacketSize = max( xMediaInfo.dwInputSize, max( xMediaInfo.dwOutputSize, *pdwPacketSize ) );

			dwMask >>= 1UL;
		}

		dwMask = 1UL << (dwNumXMOs - 1);
	}
	
	DbgPrint( "Agreed size: %u\n", *pdwPacketSize );

	return hr;
}

HRESULT InitPacket( LPXMEDIAPACKET pPacket, DWORD dwPacketSize, LPVOID pBuffer, LPDWORD dwStatus, LPDWORD dwCompletedSize, HANDLE hEvent )
{
	if ( NULL == pPacket )
		return E_INVALIDARG;

	ZeroMemory( pPacket, sizeof ( XMEDIAPACKET ) );

	pPacket->dwMaxSize = dwPacketSize;
	pPacket->pvBuffer = pBuffer;

	pPacket->pdwStatus = dwStatus;
	pPacket->pdwCompletedSize = dwCompletedSize;

	pPacket->hCompletionEvent = hEvent;

	return S_OK;
}

HRESULT InitProcessContext( List* pXMOList, DWORD dwPacketSize, ProcessContext** ppContext )
{
	if ( NULL == ppContext )
		return E_INVALIDARG;

	if ( NULL == pXMOList )
	{
		*ppContext = NULL;
		return S_OK;
	}

	HRESULT hr = S_OK;
	DWORD i = 0;

	ProcessContext* pContext = new ProcessContext;
	CHECKALLOC( pContext );

	if ( SUCCEEDED( hr ) )
	{
		ZeroMemory( pContext, sizeof( ProcessContext ) );
		pContext->pXMOList = pXMOList;

		for ( pContext->pLastXMO = pXMOList; NULL != pContext->pLastXMO->pNext; pContext->pLastXMO = pContext->pLastXMO->pNext ); 

		pContext->dwPacketSize = dwPacketSize;
		pContext->dwBytesPerQueue = pContext->dwPacketSize * PACKET_COUNT;

//		pContext->dwInStatus = XMEDIAPACKET_STATUS_PENDING;
//		pContext->dwOutStatus = XMEDIAPACKET_STATUS_PENDING;

//		pContext->dwInCompletedSize = 0;
//		pContext->dwOutCompletedSize = 0;

//		pContext->dwNumQueued = 0;
//		pContext->dwStartQueue = 0;

//		for ( i = 0; i < PACKET_COUNT; i++ )
//		{
//			pContext->dwNumDest[i] = 0;
//		}

/*		for ( i = 0; SUCCEEDED( hr ) && i < NUM_EVENTS; i++ )
		{
			pContext->pPacketEvents[i] = CreateEvent( NULL, FALSE, FALSE, NULL );
			CHECKALLOC( pContext->pPacketEvents[i] );
		}
*/
		pContext->hEvent = GetEvent();
		CHECKALLOC( pContext->hEvent );
	}

	if ( SUCCEEDED( hr ) )
	{
		pContext->pQueueBuffer = new BYTE[pContext->dwBytesPerQueue * NUM_QUEUES];
		CHECKALLOC( pContext->pQueueBuffer );
	}

	if ( SUCCEEDED( hr ) )
	{
		ZeroMemory( pContext->pQueueBuffer, pContext->dwBytesPerQueue * NUM_QUEUES );
	}

	for ( i = 0; SUCCEEDED( hr ) && i < NUM_EVENTS; i++ )
	{
		pContext->pdwStatus[i] = 0;
		pContext->pdwContextStatus[i] = FREE_SRC;
		pContext->pdwCompletedSize[i] = 0;

		CHECKEXECUTE( InitPacket( &pContext->pPackets[i], 
			                      pContext->dwPacketSize, 
								  pContext->pQueueBuffer + i * pContext->dwPacketSize, 
								  &pContext->pdwStatus[i], 
								  &pContext->pdwCompletedSize[i], 
								  pContext->hEvent ) );	
	}
		
	if ( SUCCEEDED( hr ) )
	{
		*ppContext = pContext;
	}

	return hr;
}

void DestroyProcessContext( ProcessContext* pContext )
{
	if ( NULL == pContext )
		return;

	delete [] pContext->pQueueBuffer;

	delete pContext;
}
/*
HRESULT PrimeContext( ProcessContext* pContext )
{
	if ( NULL == pContext )
		return S_OK;

	HRESULT hr = S_OK;

	for ( DWORD i = 0; SUCCEEDED( hr ) && i < PACKET_COUNT; i++ )
	{
		pContext->dwNumDest[i] = i + PACKET_COUNT;
		*pContext->pPackets[i + PACKET_COUNT].pdwCompletedSize = 0;
		CHECKEXECUTE( pContext->pXMOList->pXMO->Process( NULL, &pContext->pPackets[i + PACKET_COUNT] ) );
	}

	pContext->dwNumQueued = PACKET_COUNT;
	pContext->dwStartQueue = 0;

	return hr;
}
*/
/*HRESULT Process( ProcessContext* pContext )
{
	if ( NULL == pContext )
		return S_OK;

	HRESULT hr = S_OK;

	List* pCurrent = pContext->pXMOList;

	DWORD dwWait = WaitForMultipleObjects( NUM_EVENTS, pContext->pPacketEvents, FALSE, INFINITE );
	dwWait -= WAIT_OBJECT_0;

	if ( PACKET_COUNT <= dwWait )
	{
		if ( 0 != pContext->dwNumQueued && dwWait == pContext->dwNumDest[pContext->dwStartQueue] )
		{
			pContext->dwNumQueued--;
			pContext->dwStartQueue = ( pContext->dwStartQueue + 1 ) % PACKET_COUNT;

			pCurrent = pCurrent->pNext;

			for ( ; SUCCEEDED( hr ) && NULL != pCurrent; pCurrent = pCurrent->pNext )
			{
				*pContext->pPackets[dwWait - PACKET_COUNT].pdwCompletedSize = 0;
				//memcpy( pContext->pPackets[dwWait - PACKET_COUNT].pvBuffer, pContext->pPackets[dwWait].pvBuffer, pContext->dwPacketSize );
				pContext->pPackets[dwWait - PACKET_COUNT].pvBuffer = pContext->pQueueBuffer + dwWait * pContext->dwPacketSize;
				CHECKEXECUTE( pCurrent->pXMO->Process( &pContext->pPackets[dwWait - PACKET_COUNT], NULL == pCurrent->pNext ? NULL : &pContext->pPackets[dwWait] ) );
			}

		}
		else
		{
			SetEvent( pContext->pPacketEvents[dwWait] );
		}
	}
	else
	{
		if ( PACKET_COUNT != pContext->dwNumQueued )
		{
			pContext->dwNumDest[pContext->dwStartQueue] = dwWait + PACKET_COUNT;
			pContext->dwNumQueued++;
			pContext->dwStartQueue = 0 == pContext->dwStartQueue ? PACKET_COUNT - 1 : pContext->dwStartQueue - 1;

			*pContext->pPackets[dwWait + PACKET_COUNT].pdwCompletedSize = 0;
			CHECKEXECUTE( pCurrent->pXMO->Process( NULL, &pContext->pPackets[dwWait + PACKET_COUNT] ) );
		}
		else
		{
			SetEvent( pContext->pPacketEvents[dwWait] );
		}
	}

	return hr;
}
*/



HRESULT Process( ProcessContext* pContext )
{
	if ( NULL == pContext )
		return S_OK;

	HRESULT hr = S_OK;
	DWORD i = 0;
	DWORD dwStatusSrc = 0;
	DWORD dwStatusDest = 0;
	
	static DWORD c = 0;

	for ( i = 0; i < NUM_EVENTS && SUCCEEDED( hr ); i++ )
	{
		if ( PENDING_SRC == pContext->pdwContextStatus[i] && XMEDIAPACKET_STATUS_SUCCESS == pContext->pdwStatus[i] )
		{
			pContext->pdwContextStatus[i] = FREE_DEST;
		}

		if ( PENDING_DEST == pContext->pdwContextStatus[i] && XMEDIAPACKET_STATUS_SUCCESS == pContext->pdwStatus[i] )
		{
			pContext->pdwContextStatus[i] = FREE_SRC;
		}
	}

	for ( i = 0; i < NUM_EVENTS && SUCCEEDED( hr ); i++ )
	{
		CHECKEXECUTE( pContext->pXMOList->pXMO->GetStatus( &dwStatusSrc ) );
		CHECKEXECUTE( pContext->pLastXMO->pXMO->GetStatus( &dwStatusDest ) );

		if ( SUCCEEDED( hr ) && FREE_SRC == pContext->pdwContextStatus[i] && XMO_STATUSF_ACCEPT_OUTPUT_DATA & dwStatusSrc )
		{
			pContext->pPackets[i].dwMaxSize = pContext->dwPacketSize;
			*pContext->pPackets[i].pdwCompletedSize = 0;
			pContext->pdwContextStatus[i] = PENDING_SRC;
			/*CHECKEXECUTE(*/ hr = pContext->pXMOList->pXMO->Process( NULL, &pContext->pPackets[i] /*)*/ );

			if ( ( SUCCEEDED( hr ) && 
				   XMEDIAPACKET_STATUS_SUCCESS == pContext->pdwStatus[i] && 
				   0 == *pContext->pPackets[i].pdwCompletedSize ) || FAILED( hr ) )
			{
				__asm int 3;
				DbgPrint( "Warning case source\n" );
			//	hr = S_OK;
				break;
			}

		}

		if ( SUCCEEDED( hr ) && FREE_DEST == pContext->pdwContextStatus[i] && XMO_STATUSF_ACCEPT_INPUT_DATA & dwStatusDest )
		{
		//	if ( c++ % 100 == 0 )
		//	DbgPrint( "******************** %u PENDING_DEST %s\n", i, pContext->pLastXMO->szName );

			pContext->pPackets[i].dwMaxSize = *pContext->pPackets[i].pdwCompletedSize;
			*pContext->pPackets[i].pdwCompletedSize = 0;
			pContext->pdwContextStatus[i] = PENDING_DEST;
			/*CHECKEXECUTE(*/ hr = pContext->pLastXMO->pXMO->Process( &pContext->pPackets[i], NULL /*)*/ );

			if ( ( SUCCEEDED( hr ) && XMEDIAPACKET_STATUS_SUCCESS == pContext->pdwStatus[i] && 0 == *pContext->pPackets[i].pdwCompletedSize ) || FAILED( hr ) )
			{
				DbgPrint( "waring case dest\n" );
				__asm int 3;
			//	hr = S_OK;
				break;
			}

		}
	}

	return hr;
}

HRESULT ProcessXMOs( List* pSendXMOs, DWORD dwSendPacketSize, List* pRecvXMOs, DWORD dwRecvPacketSize )
{
	HRESULT hr = S_OK;

	ProcessContext* pSendContext = NULL;
	ProcessContext* pRecvContext = NULL;

	CHECKEXECUTE( InitProcessContext( pSendXMOs, dwSendPacketSize, &pSendContext ) );
	CHECKEXECUTE( InitProcessContext( pRecvXMOs, dwRecvPacketSize, &pRecvContext ) );

//	CHECKEXECUTE( PrimeContext( pSendContext ) );
//	CHECKEXECUTE( PrimeContext( pRecvContext ) );

	while ( SUCCEEDED( hr ) )
	{
		WaitForSingleObject( g_hEvent, INFINITE );

		CHECKEXECUTE( Process( pSendContext ) );
		CHECKEXECUTE( Process( pRecvContext ) );
	}
	
	DestroyProcessContext( pSendContext );
	DestroyProcessContext( pRecvContext );

	return hr;
}

void RunTest( void )
{
	HRESULT hr = S_OK;

	List* pSendXMOs = NULL;
	List* pRecvXMOs = NULL;

	DWORD dwSendPacketSize = 0;
	DWORD dwRecvPacketSize = 0;

	CHECKEXECUTE( OpenNet() );

	CHECKEXECUTE( CreateList( &pSendXMOs, XMO_HAWK_MIC | XMO_NET_SEND ) );
	CHECKEXECUTE( CreateList( &pRecvXMOs, XMO_NET_RECV | XMO_HAWK_HEAD ) );

	CHECKEXECUTE( ConnectXMOs( pSendXMOs, &dwSendPacketSize ) );
	CHECKEXECUTE( ConnectXMOs( pRecvXMOs, &dwRecvPacketSize ) );

	CHECKEXECUTE( ProcessXMOs( pSendXMOs, dwSendPacketSize, pRecvXMOs, dwRecvPacketSize ) );

	DeleteList( pSendXMOs );
    DeleteList( pRecvXMOs );

	CHECKEXECUTE( CloseNet() );

}

EXTERN_C DWORD WINAPI VoiceThreadProc( LPVOID lpParameter )
{
	RunTest();

	return 0;
}
