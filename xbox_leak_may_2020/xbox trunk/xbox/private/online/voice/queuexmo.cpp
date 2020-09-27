//-----------------------------------------------------------------------------
// File: QueueXMO.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <stddef.h>
#include <ntos.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#define  NODSOUND
#include <xtl.h>
#include <dsound.h>

#include "xvocver.h"
#include "xvoice.h"
#include "QueueXMOp.h"
#include "voiceencoder.h"

//
// The interface
//
//////////////////////////////////////////////////////////////////////////////
XBOXAPI 
HRESULT 
WINAPI
XVoiceQueueCreateMediaObject(LPQUEUE_XMO_CONFIG pConfig , LPXVOICEQUEUEMEDIAOBJECT *ppVoiceQueueMediaObject)
{   
    
    CQueueXMO *pQueueXmo;

	HRESULT hr = QUEUE_S_OK;
    
    do
    {
		*ppVoiceQueueMediaObject = NULL;

		//
		// Create Queue XMO
		//
		pQueueXmo = new CQueueXMO;

		if ( NULL == pQueueXmo )
		{
			hr = QUEUE_E_OUTOFMEMORY;
			break;
		}

		//
        // Initialize with the given parameters
        //
		hr = pQueueXmo->Initialize(pConfig);

		if (SUCCEEDED(hr)) 
		{
			*ppVoiceQueueMediaObject = pQueueXmo;
		}
		else
		{
			delete pQueueXmo;
		}
     
    }
    while( FALSE );
    
    return( hr );

}

//
// Public Method: Get output buffer size for the encoder depending on the PCM buffer size to be encoded 
// The same buffer sizes apply to the decoder , too
//
XBOXAPI 
HRESULT 
WINAPI
XVoiceGetCodecBufferSize( const LPXMEDIAOBJECT pCodecMediaObject, WORD wPCMBufferSize , WORD *pwCodecBufferSize)
{
	DOUBLE dCompressionRate;

    if( NULL !=  pCodecMediaObject)
    {
        XMEDIAINFO info;
        pCodecMediaObject->GetInfo(&info);

        //
        // The codec has a fixed compression rate and the compressed packet contains a header
        //
		if( info.dwInputSize >  info.dwOutputSize)
        {
			//
            // Encoder
            //
			dCompressionRate = (DOUBLE)info.dwInputSize / (DOUBLE)(info.dwOutputSize - sizeof(VOICE_ENCODER_HEADER));
        }
		else
		{
            //
            // Decoder
            //
			dCompressionRate = (DOUBLE)info.dwOutputSize / (DOUBLE)(info.dwInputSize - sizeof(VOICE_ENCODER_HEADER));
		}

        *pwCodecBufferSize = (WORD)((DOUBLE)wPCMBufferSize / dCompressionRate) + sizeof(VOICE_ENCODER_HEADER);
    }
    else
    {
        *pwCodecBufferSize = 0;
    }

	return (QUEUE_S_OK);
}


//
// Class CQueueXMO implementation
//
//////////////////////////////////////////////////////////////////////////////
void CQueueXMO::Cleanup()
{
	if( NULL != m_pdHighWaterMarkQuality )
    {
		delete [] m_pdHighWaterMarkQuality;
    }

	if( NULL != m_pBufferPool )
    {
		delete [] m_pBufferPool;
    }

	if( NULL != m_pHeapForBuffers )
    {
		delete [] m_pHeapForBuffers;
    }

	if( NULL != m_pSavedBuffer )
    {
		delete [] m_pSavedBuffer;
    }

    if(m_pDecoder)
    {
        m_pDecoder->Release();
        m_pDecoder = NULL;
    }
}

void CQueueXMO::SetConfigDefaults(LPQUEUE_XMO_CONFIG pConfig)
{
    //
    // In case pConfig members set to 0 -> use defaults
    //
	if( 0 == pConfig->wVoiceSamplingRate )
    {
		pConfig->wVoiceSamplingRate = 8000;
    }
	RIP_ON_NOT_TRUE("XVoiceQueueCreateMediaObject()", ( 8000 == pConfig->wVoiceSamplingRate));

	if( 0 ==  pConfig->dwCodecTag)
    {
		pConfig->dwCodecTag = WAVE_FORMAT_VOXWARE_SC06;
    }
	RIP_ON_NOT_TRUE("XVoiceQueueCreateMediaObject()", ((WAVE_FORMAT_VOXWARE_SC06 == pConfig->dwCodecTag) ||
		                                               (WAVE_FORMAT_VOXWARE_SC03 == pConfig->dwCodecTag)));

	if( 0 ==  pConfig->wMsOfDataPerPacket)
    {
		pConfig->wMsOfDataPerPacket = 40;
    }
	RIP_ON_NOT_TRUE("XVoiceQueueCreateMediaObject()", ( 0 == pConfig->wMsOfDataPerPacket % 20 ));

	if( 0 == pConfig->wMinDelay )
    {
		pConfig->wMinDelay = 80;
    }

	if( 0 ==  pConfig->wMaxDelay)
    {
		pConfig->wMaxDelay = 1000;
    }
	RIP_ON_NOT_TRUE("XVoiceQueueCreateMediaObject()", ( pConfig->wMaxDelay >= pConfig->wMinDelay ));

	if( 0 == pConfig->wInitialHighWaterMark )
    {
		pConfig->wInitialHighWaterMark = 80;
    }
	RIP_ON_NOT_TRUE("XVoiceQueueCreateMediaObject()", ( pConfig->wInitialHighWaterMark >= pConfig->wMinDelay ));

	if( 0.0 == pConfig->dIdealQuality )
    {
		pConfig->dIdealQuality = 0.005;
    }
	RIP_ON_NOT_TRUE("XVoiceQueueCreateMediaObject()", (( 0 <= pConfig->dIdealQuality ) && (1 >= pConfig->dIdealQuality)));

	pConfig->dInitialFrameStrength = (DOUBLE)(pConfig->wMsOfDataPerPacket)/(DOUBLE)30000;

}

HRESULT STDMETHODCALLTYPE CQueueXMO::Initialize( LPQUEUE_XMO_CONFIG pConfig )
{
	HRESULT hr = QUEUE_S_OK;
	int i;

	do
	{
		SetConfigDefaults(pConfig);

		//
        // Compute dimensions for the queue
        //
		m_wMsOfDataPerBuffer = pConfig->wMsOfDataPerPacket;
		m_wBufferPoolSize = pConfig->wMaxDelay / m_wMsOfDataPerBuffer;

		WORD wXVoiceBytesPerMs = ( pConfig->wVoiceSamplingRate / 1000 ) * 2 ;
		m_wXVoiceBufferSize = wXVoiceBytesPerMs * pConfig->wMsOfDataPerPacket;
				
		m_wHighWaterMark = pConfig->wInitialHighWaterMark;
		m_PreviousTime = 0;

		m_dIdealQuality = pConfig->dIdealQuality;
		m_dFrameStrength = pConfig->dInitialFrameStrength;
		m_dCurrentTalkspurtWeighting = 0;
		m_wNumberOfFramesInTalkspurt = 0;
		m_bCurrentTalkspurtID = 0;
		m_wErrorCount = 0;

		m_wMaxHighWaterMark = pConfig->wMaxDelay;
	    m_wMinHighWaterMark = pConfig->wMinDelay;

		m_wHighWaterMarkQualitySize = (m_wMaxHighWaterMark - m_wMinHighWaterMark) / m_wMsOfDataPerBuffer + 1;

		//
        // Create voice decoder
        //
		WAVEFORMATEX wfxFormat;
		wfxFormat.wFormatTag = WAVE_FORMAT_PCM;
		wfxFormat.nChannels = 1;
		wfxFormat.nSamplesPerSec = pConfig->wVoiceSamplingRate;
		wfxFormat.nAvgBytesPerSec = 2 * pConfig->wVoiceSamplingRate;
		wfxFormat.nBlockAlign = 2;
		wfxFormat.wBitsPerSample = 16;
		wfxFormat.cbSize = 0;

        hr = XVoiceDecoderCreateMediaObject( pConfig->dwCodecTag,
                                             &wfxFormat,
                                             &m_pDecoder );
        if( FAILED( hr ) )
        {

#ifdef _DEBUG_PRINT
            DebugPrint("CQueueXMO::Initialize( )...Voice decoder failed with %x", hr);
#endif
            break;
        }

		//
        // Get the codec buffer size for this decoder
        //
		XVoiceGetCodecBufferSize( m_pDecoder, m_wXVoiceBufferSize , &m_wCodecBufferSize);
		
		//
        // Allocate memory for the array of HWM quality history
        //
		m_pdHighWaterMarkQuality = new DOUBLE[m_wHighWaterMarkQualitySize];
		if ( NULL == m_pdHighWaterMarkQuality )
		{
			hr = QUEUE_E_OUTOFMEMORY;
			break;
		}

		//
        // Allocate memory for saved buffer for lost packets
        //
		m_pSavedBuffer = new BYTE[m_wXVoiceBufferSize];
		if( NULL ==  m_pSavedBuffer)
        {	
			hr = QUEUE_E_OUTOFMEMORY;
			break;
        }
				
		//
        // Allocate memory for the buffers
        //
		m_pHeapForBuffers = new BYTE[ m_wBufferPoolSize * m_wCodecBufferSize];		
		if ( NULL == m_pHeapForBuffers )
		{
			hr = QUEUE_E_OUTOFMEMORY;
			break;
		}

		//
        // Allocate memory for the list of buffers
        //
		m_pBufferPool = new BufferList[m_wBufferPoolSize];
		if ( NULL == m_pBufferPool )
		{
			Cleanup();
			hr = QUEUE_E_OUTOFMEMORY;
			break;
		}

		//
        // Initialize free buffers list 
        //
		for( i = 1 ; i < m_wBufferPoolSize - 1; i++ )
        {
			m_pBufferPool[i].m_pbBuffer = m_pHeapForBuffers + i * m_wCodecBufferSize;
			m_pBufferPool[i].m_pPrev = &m_pBufferPool[i - 1];
			m_pBufferPool[i].m_pNext = &m_pBufferPool[i + 1];
        }
		m_pFreeEntriesList = &m_pBufferPool[0];
		m_pBufferPool[0].m_pNext = &m_pBufferPool[1];
		m_pBufferPool[0].m_pbBuffer = m_pHeapForBuffers;
		m_pBufferPool[m_wBufferPoolSize - 1].m_pPrev = &m_pBufferPool[m_wBufferPoolSize - 2];
		m_pBufferPool[m_wBufferPoolSize - 1].m_pbBuffer = m_pHeapForBuffers + (m_wBufferPoolSize - 1) * m_wCodecBufferSize;

		//
		// Initialize the array of HWM quality history
		//
		for( i = 0 ; i < m_wHighWaterMarkQualitySize ; i++ )
        {
			m_pdHighWaterMarkQuality[i] = m_dIdealQuality;
        }
	}
	while( FALSE );

	return( hr );
}

HRESULT STDMETHODCALLTYPE CQueueXMO::Process( LPCXMEDIAPACKET pSrcBuffer, LPCXMEDIAPACKET pDstBuffer )
{
	HRESULT hr = QUEUE_S_OK;
    
    if( NULL != pSrcBuffer )
    {
    	hr = InputProcess(pSrcBuffer);
    }

	if( NULL != pDstBuffer )
    {
		hr = OutputProcess(pDstBuffer);
    }
    
    return( hr );
}


HRESULT CQueueXMO::InputProcess(LPCXMEDIAPACKET pInMediaPacket)
{
	HRESULT hr = QUEUE_S_OK;
    
    do
    {
		if( VOICE_MESSAGE_TYPE_VOICE != ((PVOICE_ENCODER_HEADER)(pInMediaPacket->pvBuffer))->bType)
        {
			if(m_bStartTiming)
			{
#ifdef _DEBUG_PRINT
			    DebugPrint("CQueueXMO::InputProcess...Drop silence packet!\n");
#endif
			    break;
			}
        }

		//
        // Get an empty buffer for putting the new data in
        //
		BufferList *pWorkingBuffer = GetInputWorkingBuffer();
		if( NULL == pWorkingBuffer )
        {
			hr = QUEUE_E_FULL;
#ifdef _DEBUG_PRINT
			DebugPrint("CQueueXMO::InputProcess...The queue is full!\n");
#endif
			break;
        }
     

		//
        // Fill the buffer with the contents of input media packet
        //
		WORD wInSeqNo = ((PVOICE_ENCODER_HEADER)(pInMediaPacket->pvBuffer))->wSeqNum;
		BYTE bMsgNum = ((PVOICE_ENCODER_HEADER)(pInMediaPacket->pvBuffer))->bMsgNum;

		if( !m_bStartTiming )
	    {
			//
			// Start measuring the real time
			//
			m_RealStartOutputTime = GetTickCount();
			m_LogicalOutputTime = 0;
			m_wCurrentSeqNo = wInSeqNo;
		    m_bCurrentTalkspurtID = bMsgNum;

			//
            // Up to 1 second of difference between arrived sequences 
			// After that, we do some computation to recover from the "discontinuity"
            // that came up on the other end
			//
			m_wSafeDeltaSeq = 1000 / m_wMsOfDataPerBuffer;
			
			m_bStartTiming = TRUE;
		}

		
		if( wInSeqNo + m_wSafeDeltaSeq < m_wCurrentSeqNo )
        {
			// Assuming reset sequence numbers on the other side
		    // GetTickCount() - m_RealStartOutputTime + m_wMsOfDataPerBuffer -> fair guess of this packet's output time
		    // round to complete packet time 
		    TimeStamp outputTime = GetTickCount() - m_RealStartOutputTime + m_wMsOfDataPerBuffer;
		    outputTime -= (outputTime % m_wMsOfDataPerBuffer);
			
			//
            // Make sure 2 packets don't have the same output time
            //
			pWorkingBuffer->m_timestamp = max(outputTime , m_PreviousTime + m_wMsOfDataPerBuffer);

        }
		else
		{
			//
            // Normal sequencing
            //
			LONG nCandidateTimestamp = (LONG)m_PreviousTime + ((LONG)wInSeqNo - (LONG)m_wCurrentSeqNo) * m_wMsOfDataPerBuffer; 
			if(nCandidateTimestamp >= 0)
			{
				pWorkingBuffer->m_timestamp = nCandidateTimestamp;
			}
			else
			{
#ifdef _DEBUG_PRINT
			DebugPrint("Drop buffer created before we started counting time: pWorkingBuffer->m_timestamp = %d\n", nCandidateTimestamp);
#endif
				//
                // This packet came before we started counting time; drop it
                //
				AddFreeBuffer(pWorkingBuffer);
				break;
			}
		}

		//
        // Check for a new talkspurt
        //
		if( bMsgNum !=  m_bCurrentTalkspurtID )
        {
			//
            // Get into account the completed talkspurt - adapt to new network conditions
            //
			RecomputeQueueParameters();
			m_bCurrentTalkspurtID = bMsgNum;
        }

		if( 0 == m_wNumberOfFramesInTalkspurt  )
        {
			//
			// Resync the local clock with the time given by the input
			//
#ifdef _DEBUG_PRINT
			DebugPrint("***Resync clock - Before: %d\n" , m_RealStartOutputTime);
#endif

			m_RealStartOutputTime = GetTickCount() - pWorkingBuffer->m_timestamp;

#ifdef _DEBUG_PRINT
			DebugPrint("***Resync clock - After: %d\n" , m_RealStartOutputTime);
#endif
        }
		m_wNumberOfFramesInTalkspurt++;

#ifdef _DEBUG_PRINT
		DebugPrint("wInSeqNo=%d bCurrentTalkspurtID=%d\n", wInSeqNo , m_bCurrentTalkspurtID);
#endif

		if( m_LogicalOutputTime > pWorkingBuffer->m_timestamp)
        {
			//
            // This buffer is late
            //
#ifdef _DEBUG_PRINT
			DebugPrint("Buffer late: pWorkingBuffer->m_timestamp = %d\n", pWorkingBuffer->m_timestamp);
#endif
			if( bMsgNum ==  m_bCurrentTalkspurtID )
            {
           		//
				// Count it as an error only if it's part of the current talkspurt
				//
				IncrementErrorCount();
			}
		    AddFreeBuffer(pWorkingBuffer);
        }
		else
		{
			memcpy(pWorkingBuffer->m_pbBuffer , pInMediaPacket->pvBuffer , m_wCodecBufferSize);

			//
			// Add the buffer to the list of in use entries
			//
			hr = AddInUseBuffer(pWorkingBuffer);
			
			if( SUCCEEDED( hr ) )
            {
				//
				// Update the current sequence number and previous time
				//
				m_wCurrentSeqNo = wInSeqNo;
				m_PreviousTime = pWorkingBuffer->m_timestamp;

#ifdef _DEBUG_PRINT
			DebugPrint("Insert buffer: pWorkingBuffer->m_timestamp = %d\n", pWorkingBuffer->m_timestamp);
#endif

            }
			else
			{
				AddFreeBuffer(pWorkingBuffer);
#ifdef _DEBUG_PRINT
			DebugPrint("Drop duplicated buffer: pWorkingBuffer->m_timestamp = %d\n", pWorkingBuffer->m_timestamp);
#endif
			}
		}			

    }
    while( FALSE );

	//
    // Update output parameters
    //
	if(pInMediaPacket->pdwCompletedSize) 
	{
        *pInMediaPacket->pdwCompletedSize = SUCCEEDED(hr) ? pInMediaPacket->dwMaxSize : 0;
    }

    if(pInMediaPacket->pdwStatus) 
	{
        *pInMediaPacket->pdwStatus = SUCCEEDED(hr) ? XMEDIAPACKET_STATUS_SUCCESS : XMEDIAPACKET_STATUS_FAILURE;
    }

    if (pInMediaPacket->hCompletionEvent)
	{
        SetEvent(pInMediaPacket->hCompletionEvent);
    }
    
    return( hr );

}


HRESULT CQueueXMO::OutputProcess(LPCXMEDIAPACKET pOutMediaPacket)
{
	HRESULT hr = QUEUE_S_OK;
	BOOL bReturnSilence;
	BOOL bIsNextPacketLost;
    
	assert(pOutMediaPacket->dwMaxSize % m_wXVoiceBufferSize == 0);

    do
    {
		if( !m_bStartTiming )
        {
			hr = QUEUE_E_NOINPUTYET;
#ifdef _DEBUG_PRINT
			DebugPrint("CQueueXMO::OutputProcess...No input yet!\n");
#endif
			break;
        }
		//
        // Get the next buffer to be decoded and returned
        //
		BufferList *pWorkingBuffer = GetOutputWorkingBuffer(&bReturnSilence , &bIsNextPacketLost);	        
		memset(pOutMediaPacket->pvBuffer , 0 , pOutMediaPacket->dwMaxSize);

		if( NULL == pWorkingBuffer )
        {
			if(!bReturnSilence)
			{
				//
                // The high water mark is not reached
                //
				hr = QUEUE_E_HWMNOTREACHED;
#ifdef _DEBUG_PRINT
				DebugPrint("CQueueXMO::OutputProcess...High water mark not reached!\n");
#endif
				break;
			}
			else
			{
				//
                // Packet lost or late
                //
			    if(m_bVoiceReturned)
				{
    				//
                    // Mark the starting moment for rendering previous packet
                    //
				    m_bVoiceReturned = FALSE;
			        m_wAttenuatedBackupMs = 0;
				}

			    if(m_wAttenuatedBackupMs < MAX_MILISECONDS_OF_ATTENUATED_VOICE)
				{
					m_wAttenuatedBackupMs += m_wMsOfDataPerBuffer;

					CopyAttenuatedBuffer(pOutMediaPacket , 1.0);

					// m_LastOutputInfo.nMaxPower is the same as the previous

#ifdef _DEBUG_PRINT
					DebugPrint("CQueueXMO::OutputProcess...Return attenuated(1.0) previous packet!\n");
#endif
				}
				else
				{
				    //
                    // Silence returned for some time now
                    //
#ifdef _DEBUG_PRINT
				    DebugPrint("CQueueXMO::OutputProcess...Return silence!\n");
#endif
					m_LastOutputInfo.nMaxPower = 0;
				}
			}
        }
		else
		{
			m_LastOutputInfo.nMaxPower = (((PVOICE_ENCODER_HEADER)(pWorkingBuffer->m_pbBuffer))->bType);

			//
            // Decompress 
            //
			hr = DecompressMediaBuffer(pWorkingBuffer, pOutMediaPacket);
#ifdef _DEBUG_PRINT
			DebugPrint("CQueueXMO::Return buffer pWorkingBuffer->m_timestamp = %d\n", pWorkingBuffer->m_timestamp);
#endif

			//
			// Free the working buffer
			//
			AddFreeBuffer(pWorkingBuffer);

			m_bVoiceReturned = TRUE;
			if( bIsNextPacketLost )
            {
				//
                // Save this buffer in case next one doesn't show up until its output time
                //
				memcpy(m_pSavedBuffer , pOutMediaPacket->pvBuffer , pOutMediaPacket->dwMaxSize); 
            }

			
		}
    
		//
        // Update the logical output time
        //
		m_LogicalOutputTime += m_wMsOfDataPerBuffer;

        hr = QUEUE_S_OK;
    }
    while( FALSE );

	//
    // Update output parameters
    //
	if(pOutMediaPacket->pdwCompletedSize) 
	{
	    *pOutMediaPacket->pdwCompletedSize = SUCCEEDED(hr) ? pOutMediaPacket->dwMaxSize : 0;
    }

    if(pOutMediaPacket->pdwStatus) 
	{
        *pOutMediaPacket->pdwStatus = SUCCEEDED(hr) ? XMEDIAPACKET_STATUS_SUCCESS : XMEDIAPACKET_STATUS_FAILURE;
    }

    if (pOutMediaPacket->hCompletionEvent)
	{
        SetEvent(pOutMediaPacket->hCompletionEvent);
    }
    
    return( hr );
}

BufferList* CQueueXMO::GetInputWorkingBuffer()
{
	BufferList *pWorkingBuffer;

	if( NULL == m_pFreeEntriesList )
    {
		//
        // All the buffers are in use: drop this packet
        //
		pWorkingBuffer = NULL;
    }
	else
	{
		//
        // Get the first free buffer
        //
		pWorkingBuffer = RemoveFirstBuffer(&m_pFreeEntriesList);
	}

	return pWorkingBuffer;

}


BufferList* CQueueXMO::RemoveFirstBuffer(BufferList **pHead , BufferList **pTail)
{
	assert(*pHead);

	//
    // Remove the head of the double linked list and return it
    //
	BufferList *pTemp = *pHead;
	*pHead = pTemp->m_pNext;

	if( NULL !=  *pHead)
    {
		(*pHead)->m_pPrev = NULL;
    }
	pTemp->m_pNext = NULL;

	if(( NULL != pTail ) && (NULL == *pHead))
    {
		*pTail = NULL;
    }

	return pTemp;

}

HRESULT CQueueXMO::AddInUseBuffer(BufferList *pBuffer)
{
	HRESULT hr = S_OK;

	m_wInUseEntriesCount++;

	if( NULL == m_pInUseEntriesListHead )
    {
		//
        // Empty in use list
        //
		assert(NULL == m_pInUseEntriesListTail);
		m_pInUseEntriesListHead = m_pInUseEntriesListTail = pBuffer;
    }
	else
	{
		//
        // At least one element, insert in sorted list
        //
		BufferList *pTmpBuffer = m_pInUseEntriesListTail;
		while((NULL != pTmpBuffer) && (pBuffer->m_timestamp < pTmpBuffer->m_timestamp))
		{		
			pTmpBuffer = pTmpBuffer->m_pPrev;
		}

		if( NULL == pTmpBuffer )
        {
			//
            // We are at the beginning of the list
            //
			m_pInUseEntriesListHead->m_pPrev = pBuffer;
			pBuffer->m_pNext = m_pInUseEntriesListHead;
			m_pInUseEntriesListHead = pBuffer;
        }
		else
		{
			if( pBuffer->m_timestamp != pTmpBuffer->m_timestamp )
            {
			    //
                // Insert after pTmpBuffer
			    //
			    pBuffer->m_pNext = pTmpBuffer->m_pNext;
			    pBuffer->m_pPrev = pTmpBuffer;

			    pTmpBuffer->m_pNext = pBuffer;
			    if( NULL == pBuffer->m_pNext )
				{
				    m_pInUseEntriesListTail = pBuffer;
				}
			    else
				{
				    (pBuffer->m_pNext)->m_pPrev = pBuffer;
				}	
            }
			else
			{
				//
                // Drop duplicate
                //
				m_wInUseEntriesCount--;
				hr = E_FAIL;
			}
		}
	}

	return (hr);
}

BufferList* CQueueXMO::GetOutputWorkingBuffer(BOOL *pbReturnSilence , BOOL *pbIsNextPacketLost)
{
	BufferList *pOutBuffer = NULL;
	*pbReturnSilence = FALSE;
	*pbIsNextPacketLost = TRUE;

	assert(((0 == m_wInUseEntriesCount) && (NULL == m_pInUseEntriesListHead)) ||
		   ((0 != m_wInUseEntriesCount) && (NULL != m_pInUseEntriesListHead)));

	DWORD realOutputTime = GetTickCount() - m_RealStartOutputTime;

#ifdef _DEBUG_PRINT
	DebugPrint("CQueueXMO::LogicalOutputTime = %d InUseEntriesCount = %d RealOutputTime=%d HWM=%d\n" , 
		        m_LogicalOutputTime , m_wInUseEntriesCount , realOutputTime , m_wHighWaterMark);
#endif


	//
    // 1. Because a packet can be asked for output just before the high water mark is reached, 
	// allow a window of 5 ms around HWM for output
    //
	//
    // 2. If more packets in the queue than HWM enforces, let them go
    //
	if( ((LONG)realOutputTime - (LONG)m_LogicalOutputTime >= (LONG)m_wHighWaterMark - 5) ||
		(m_wInUseEntriesCount > m_wHighWaterMark / m_wMsOfDataPerBuffer))
	{
		//
		// We send to output packet either silence or decompressed data
		//
		if(( m_wInUseEntriesCount > 0 ) && (m_LogicalOutputTime == m_pInUseEntriesListHead->m_timestamp))
		{
			//
			// The buffer is in the list
			//
			pOutBuffer = RemoveFirstBuffer(&m_pInUseEntriesListHead , &m_pInUseEntriesListTail); 
			m_wInUseEntriesCount--;
			if( (m_wInUseEntriesCount > 0 ) && (m_pInUseEntriesListHead->m_timestamp == pOutBuffer->m_timestamp + m_wMsOfDataPerBuffer) )
            {
				*pbIsNextPacketLost = FALSE;
            }
		}
		else
		{
			*pbReturnSilence = TRUE;
		}
	}
	
	return pOutBuffer;
}

void CQueueXMO::AddFreeBuffer(BufferList *pBuffer)
{
	
	//
    // Attach buffer to the head of free buffers list
    //
	if( NULL == m_pFreeEntriesList )
    {
		m_pFreeEntriesList = pBuffer;
    }
	else
	{
		m_pFreeEntriesList->m_pPrev = pBuffer;
		pBuffer->m_pNext = m_pFreeEntriesList;
		m_pFreeEntriesList = pBuffer;
	}
}

HRESULT CQueueXMO::DecompressMediaBuffer(BufferList *pSrc, LPCXMEDIAPACKET pDst)
{
	HRESULT hr;
        
	memset(pDst->pvBuffer , 0 , pDst->dwMaxSize);

	XMEDIAPACKET xmb;

	memset(&xmb,0,sizeof(xmb));
	xmb.dwMaxSize = m_wCodecBufferSize;
	xmb.pvBuffer = pSrc->m_pbBuffer;
	xmb.hCompletionEvent = NULL;

		
	//
	// Call decoding xmo
	//
	hr = m_pDecoder->Process( &xmb , pDst);

	assert(!FAILED(hr));

    return( hr );
}

void CQueueXMO::RecomputeQueueParameters()
{
	WORD wCurrentIndex = (m_wHighWaterMark - m_wMinHighWaterMark) / m_wMsOfDataPerBuffer;

	DOUBLE dTalkspurtQuality = min( ((DOUBLE)m_wErrorCount) / ((DOUBLE)m_wNumberOfFramesInTalkspurt) , 1.0);

	assert(wCurrentIndex < m_wHighWaterMarkQualitySize);

#ifdef _DEBUG_PRINT
	DebugPrint("***CurrentTalkspurt = %d NumberOfFramesInTalkspurt = %d Errors = %d m_dFrameStrength = %f\n",
		       m_bCurrentTalkspurtID , m_wNumberOfFramesInTalkspurt , m_wErrorCount , m_dFrameStrength);
#endif

	//
    // Compute new parameters
    //
	m_dCurrentTalkspurtWeighting = min( m_dFrameStrength * m_wNumberOfFramesInTalkspurt , 1.0);
	m_pdHighWaterMarkQuality[wCurrentIndex] = m_pdHighWaterMarkQuality[wCurrentIndex] * (1 - m_dCurrentTalkspurtWeighting) +
		                                      dTalkspurtQuality * m_dCurrentTalkspurtWeighting;

	//
    // Reset counters for the new talkspurt
    //
	m_wNumberOfFramesInTalkspurt = 0;
	m_wErrorCount = 0;

	if( m_pdHighWaterMarkQuality[wCurrentIndex] < m_dIdealQuality )
    {
		//
        // Better quality --> try to decrease the highwatermark
        //
		if( wCurrentIndex > 0 )
        {
			//
            // See if wCurrentIndex - 1 is closer to ideal quality
            //
			if( fabs( m_pdHighWaterMarkQuality[ wCurrentIndex - 1 ] - m_dIdealQuality) < 
				fabs( m_pdHighWaterMarkQuality[ wCurrentIndex ] - m_dIdealQuality))
            {
				m_wHighWaterMark -= m_wMsOfDataPerBuffer;

				//
                // When we decrease the HWM, we already render packets with the delay introduced by the previous HWM
				// By skipping one packet(if the queue was empty, we don't lose anything), we reduce the delay 
				// between LOT and ROT
                //
				m_LogicalOutputTime += m_wMsOfDataPerBuffer;

		        if(( m_wInUseEntriesCount > 0 ) && 
				   (m_LogicalOutputTime > m_pInUseEntriesListHead->m_timestamp))
                {
					BufferList *pDroppedBuffer = RemoveFirstBuffer(&m_pInUseEntriesListHead , &m_pInUseEntriesListTail); 
					m_wInUseEntriesCount--;
					AddFreeBuffer(pDroppedBuffer);
                }
            }
        }

    }
	else
	{
		//
        // Worse quality --> try to increase the highwatermark 
        //
		if( wCurrentIndex + 1 < m_wHighWaterMarkQualitySize  )
        {
			//
            // See if wCurrentIndex + 1 is closer to ideal quality
            //
			if( fabs( m_pdHighWaterMarkQuality[ wCurrentIndex + 1 ] - m_dIdealQuality) < 
				fabs( m_pdHighWaterMarkQuality[ wCurrentIndex ] - m_dIdealQuality))
            {
				m_wHighWaterMark += m_wMsOfDataPerBuffer;

				//
                // In order for the queueXMO to behave continuous, and the hwm stuff to not affect the user of the queue,
                // we will "play again" one packet to give time to the HWM to be reached
				// By changing the LOT, there will be silence(attenuated packet) returned
				//
				assert(0 != m_LogicalOutputTime);
				m_LogicalOutputTime -= m_wMsOfDataPerBuffer;
            }
        }

	}

#ifdef _DEBUG_PRINT
	DebugPrint("***TalkspurtQuality = %f CurrentTalkspurtWeighting = %f HighWaterMarkQuality[%d] = %f HighWaterMark = %d\n" ,
		       dTalkspurtQuality , m_dCurrentTalkspurtWeighting , wCurrentIndex , 
			   m_pdHighWaterMarkQuality[wCurrentIndex] , m_wHighWaterMark);
#endif



}

void CQueueXMO::CopyAttenuatedBuffer(LPCXMEDIAPACKET pOutMediaPacket , DOUBLE dAttenuationFactor)
{
	assert(pOutMediaPacket);

	//
    // TBD : take into account attenuation factor
    //
	memcpy(pOutMediaPacket->pvBuffer , m_pSavedBuffer , pOutMediaPacket->dwMaxSize);

#ifdef _DEBUG_PRINT
	DebugPrint("Copy attenuated buffer = %d\n", pOutMediaPacket->dwMaxSize);
#endif
}

HRESULT STDMETHODCALLTYPE CQueueXMO::GetLastOutputInfo(LPPCM_INFO pPCMInfo)
{
	if( NULL != pPCMInfo )
    {
		pPCMInfo->cbSize = sizeof(PCM_INFO);
		pPCMInfo->nMaxPower = m_LastOutputInfo.nMaxPower;
    }

	return (QUEUE_S_OK);
}

