#include "stdafx.h"
#include "OutputTool.h"
#include "cconduct.h"

extern CMIDIInputContainer* g_pMIDIInputContainer;

void UpdateLatency( DWORD dwEchoMIDIID, IDirectMusicPort **pIBasePort, DWORD *pdwBaseOutputGroup, REFERENCE_TIME rtTimeBuffer )
{
	static int nLatencyRecomputeCounter = 0;

	// Initialize the variables to return
	*pIBasePort = NULL;
	*pdwBaseOutputGroup = 0;

	IDirectMusicPort *pOutputPort;
	DWORD dwRealPChannel, dwBaseOutputGroup;
	for( DWORD dwPChannel = g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_dwPChannelBase; dwPChannel < g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_dwPChannelBase + 16; dwPChannel++ )
	{
		pOutputPort = NULL;
		if( SUCCEEDED( g_pconductor->m_pDMAudiopath->ConvertPChannel( dwPChannel, &dwRealPChannel ) )
		&&	SUCCEEDED( g_pconductor->m_pDMPerformance->PChannelInfo( dwRealPChannel,
				&pOutputPort, &dwBaseOutputGroup, NULL ) )
		&&	pOutputPort )
		{
			// If this is the information for PChannel 0, save it
			if( dwPChannel == 0 )
			{
				*pdwBaseOutputGroup = dwBaseOutputGroup;
				*pIBasePort = pOutputPort;
				pOutputPort->AddRef();
			}

			// If the latency is not set, or if it is time to check the latency
			if( (g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_rtLatencyOffset <= 0) ||
				(nLatencyRecomputeCounter <= 0) )
			{
				// Get the latency clock
				IReferenceClock *pRefClock;
				if( SUCCEEDED( pOutputPort->GetLatencyClock( &pRefClock ) ) )
				{
					// Get the latency time
					REFERENCE_TIME rtLatencyTime;
					if( SUCCEEDED( pRefClock->GetTime( &rtLatencyTime ) ) )
					{
						// Set the offset to 9/8 (112.5%) of the latency, with a minimum of the existing offset
						REFERENCE_TIME rtNewOffset = ((rtLatencyTime - rtTimeBuffer) * 9) >> 3;
						if( rtNewOffset > g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_rtLatencyOffset )
						{
							// Why is this check here?
							if( g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_rtLatencyOffset <= 10000 )
							{
								g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_rtLatencyOffset = rtNewOffset;
							}
							else
							{
								// Only allow jumps of up to 50%.
								g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_rtLatencyOffset = min( rtNewOffset, (g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_rtLatencyOffset * 3) >> 1 );
							}
							TRACE("Echo MIDI In Latency is: %I64d\n", g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_rtLatencyOffset);
						}
					}
					pRefClock->Release();
				}
			}

			// Release our pointer to the port
			pOutputPort->Release();
		}
	}

	nLatencyRecomputeCounter--;
	if( nLatencyRecomputeCounter <= 0 )
	{
		nLatencyRecomputeCounter = 10;
	}

}

void EchoBuffer( DWORD dwEchoMIDIID, IDirectMusicBuffer *pDMBuffer )
{
	ASSERT( pDMBuffer && g_pMIDIInputContainer);
	if( !pDMBuffer || !g_pMIDIInputContainer )
	{
		return;
	}

	REFERENCE_TIME rtTimeBuffer;
	pDMBuffer->GetStartTime( &rtTimeBuffer );
#ifdef VERBOSE_MIDI_INPUT_SPEW
	TRACE("Echo MIDI In Buffer start: %I64x\n", rtTimeBuffer);
#endif

	if( g_pconductor->m_pDMPerformance
	&&	g_pconductor->m_pDMAudiopath )
	{
		IDirectMusicPort *pIBasePort;
		DWORD dwBaseOutputGroup;
		UpdateLatency( dwEchoMIDIID, &pIBasePort, &dwBaseOutputGroup, rtTimeBuffer );

		// Reset the start time of the buffer to account for the worst latency
		pDMBuffer->SetStartTime( rtTimeBuffer + g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_rtLatencyOffset );

		// Reset the read pointer to the start of the buffer
		pDMBuffer->ResetReadPtr();

		DMUS_BUFFERDESC bufDesc;
		bufDesc.dwSize = sizeof(DMUS_BUFFERDESC);
		bufDesc.dwFlags = 0;
		bufDesc.guidBufferFormat = GUID_AllZeros;

		if( FAILED( pDMBuffer->GetUsedBytes( &bufDesc.cbBuffer ) ) )
		{
			bufDesc.cbBuffer = 256;
		}

		// Need to repack the buffer to play on the correct group and port
		IDirectMusicBuffer *pDMOutputBuffer = NULL;
		if( SUCCEEDED( g_pconductor->m_pDMusic->CreateMusicBuffer( &bufDesc, &pDMOutputBuffer, NULL ) ) )
		{
			DWORD dwAvailableBufferSize = bufDesc.cbBuffer;
			BYTE *pbMsg;
			REFERENCE_TIME rtTime;
			DWORD dwLength;
			DWORD dwRealPChannel;

			// The port that the current buffer is destined for
			IDirectMusicPort *pIBufferPort = NULL;

			// Reset the read pointer again (TODO: is this really necesary?)
			pDMBuffer->ResetReadPtr();

			while(  pDMOutputBuffer
				&& (pDMBuffer->GetNextEvent( &rtTime, NULL, &dwLength, &pbMsg ) == S_OK) )
			{
				// Skip empty messages
				if( dwLength == 0 )
				{
					continue;
				}

				// Initialize the group and output port
				DWORD dwOutputGroup = dwBaseOutputGroup;
				IDirectMusicPort *pIOutputPort = pIBasePort;
				if( pIOutputPort )
				{
					pIOutputPort->AddRef();
				}

				const BYTE bStatus = pbMsg[0];
				if( (dwLength < 4)
				&&	(bStatus & 0x80)
				&&	(bStatus & 0xF0) != 0xF0 )
				{
					DWORD dwMChannel;
					if( SUCCEEDED( g_pconductor->m_pDMAudiopath->ConvertPChannel( g_pMIDIInputContainer->m_aMIDIInputContainer[dwEchoMIDIID].m_dwPChannelBase + (bStatus & 0x0F), &dwRealPChannel ) )
					&&	SUCCEEDED( g_pconductor->m_pDMPerformance->PChannelInfo( dwRealPChannel,
							&pIOutputPort, &dwOutputGroup, &dwMChannel ) )
					&&	pIOutputPort )
					{
						pbMsg[0] = BYTE((bStatus & 0xF0) | BYTE(dwMChannel & 0xF));

						// If there is a base port, release it
						if( pIBasePort )
						{
							// Release the reference to the base port we added above
							pIBasePort->Release();
						}
					}
				}

				// If the current output port is NULL
				if( !pIOutputPort )
				{
					// Skip this event, since we have nowhere to send it
					continue;
				}

				// If no buffer port is assigned yet
				if( !pIBufferPort )
				{
					// Assign the current output port to the buffer port
					pIBufferPort = pIOutputPort;
				}

				// If this event needs to go to a port other than the one the buffer is
				// set up to use, send this buffer, create another one, and reset pIBufferPort
				if( pIBufferPort != pIOutputPort )
				{
					// Play pDMOutputBuffer and create a new buffer
					pIBufferPort->PlayBuffer( pDMOutputBuffer );
					pDMOutputBuffer->Release();

					// Try and create a new, empty pDMOutputBuffer
					if( FAILED( g_pconductor->m_pDMusic->CreateMusicBuffer( &bufDesc, &pDMOutputBuffer, NULL ) ) )
					{
						// Creation failed - set pDMOutputBuffer to NULL and break out of the loop
						pDMOutputBuffer = NULL;

						// Set the buffer port to NULL
						pIBufferPort = NULL;

						// Release the output port
						pIOutputPort->Release();
						break;
					}
					else
					{
						// Reset dwAvailableBufferSize
						dwAvailableBufferSize = bufDesc.cbBuffer;
					}

					pIBufferPort = pIOutputPort;
				}

				// Check if there's enough space left in pDMOutputBuffer
				if( dwAvailableBufferSize >= DMUS_EVENT_SIZE(dwLength) )
				{
					// Yes - Pack it into the output buffer
					pDMOutputBuffer->PackUnstructured( rtTime, dwOutputGroup, dwLength, pbMsg );

					// Update dwAvailableBufferSize
					dwAvailableBufferSize -= DMUS_EVENT_SIZE(dwLength);
				}
				// No - check if the message will fit in an empty buffer
				else if( DMUS_EVENT_SIZE(dwLength) <= bufDesc.cbBuffer )
				{
					// Yes - play pDMOutputBuffer and create a new buffer
					pIBufferPort->PlayBuffer( pDMOutputBuffer );
					pDMOutputBuffer->Release();

					// Try and create a new, empty pDMOutputBuffer
					if( FAILED( g_pconductor->m_pDMusic->CreateMusicBuffer( &bufDesc, &pDMOutputBuffer, NULL ) ) )
					{
						// Creation failed - set pDMOutputBuffer to NULL and break out of the loop
						pDMOutputBuffer = NULL;

						// Set the buffer port to NULL
						pIBufferPort = NULL;

						// Release the output port
						pIOutputPort->Release();
						break;
					}
					else
					{
												// Creation succeeded - pack the event into the output buffer
						pDMOutputBuffer->PackUnstructured( rtTime, dwOutputGroup, dwLength, pbMsg );

						// Reset dwAvailableBufferSize
						dwAvailableBufferSize = bufDesc.cbBuffer - DMUS_EVENT_SIZE(dwLength);
					}
				}
				else
				{
					// Message won't fit - drop it
					TRACE("EchoMIDIInThreadProc: Dropping message that is too big to fit in buffer!\n");
				}

				// Release the output port
				pIOutputPort->Release();
			}

			if( pDMOutputBuffer )
			{
				// If there is a buffer port
				if( pIBufferPort )
				{
					pIBufferPort->PlayBuffer( pDMOutputBuffer );
				}

				pDMOutputBuffer->Release();
			}
		}

		pDMBuffer->SetStartTime( rtTimeBuffer );

		if( pIBasePort )
		{
			pIBasePort->Release();
		}
	}
}

UINT AFX_CDECL EchoMIDIInThreadProc( LPVOID pParam )
{
	UNREFERENCED_PARAMETER(pParam);
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

    if ( !g_pconductor || !g_pMIDIInputContainer )
	{
		ASSERT(FALSE);
		return UINT_MAX;    // illegal parameter
	}

	ASSERT( g_pconductor->m_pDMusic != NULL );
	//try {
	while (1)
	{
		DWORD dwRes;
		dwRes = WaitForMultipleObjects( g_pMIDIInputContainer->m_dwValidInputHandles, g_pMIDIInputContainer->m_ahMIDIInputHandles, FALSE, INFINITE );

		// If our conductor, MIDI input container, or DirectMusic pointers are invalid
		// or if the Conductor is shutting down
		// or if the MIDI input container is waiting for us to exit
		if( !g_pconductor
		||	!g_pMIDIInputContainer
		||	!g_pconductor->m_pDMusic
		||	g_pconductor->m_fShutdown
		||	g_pMIDIInputContainer->m_hEchoMIDIInputExitEvent )
		{
			// Exit the thread
			break;
		}

		// If we woke up because we were signaled
		if( (dwRes >= WAIT_OBJECT_0)
		&&	(dwRes <= WAIT_OBJECT_0 + MIDI_IN_PORTS - 1) )
		{
			// Create a music buffer
			DMUS_BUFFERDESC bufDesc;
			bufDesc.dwSize = sizeof(DMUS_BUFFERDESC);
			bufDesc.dwFlags = 0;
			bufDesc.guidBufferFormat = GUID_AllZeros;
			bufDesc.cbBuffer = 256;

			IDirectMusicBuffer *pDMBuffer = NULL;
			if( SUCCEEDED( g_pconductor->m_pDMusic->CreateMusicBuffer( &bufDesc, &pDMBuffer, NULL ) ) )
			{
				HRESULT hr = E_FAIL;

				// Enter the MIDI input critical section
				::EnterCriticalSection( &g_pMIDIInputContainer->m_csEchoMIDI );

				// Iterate through all MIDI input ports
				POSITION pos = g_pMIDIInputContainer->m_lstMIDIInPorts.GetHeadPosition();
				while( pos )
				{
					// Get the next port
					PortContainer *pPortContainer = g_pMIDIInputContainer->m_lstMIDIInPorts.GetNext( pos );

					// If there is a valid port pointer
					if( pPortContainer->m_pPort )
					{
						// Try and read from the port
						hr = pPortContainer->m_pPort->Read( pDMBuffer );

						// If there is data
						if ( hr == S_OK )
						{
							// Find which "echo" containers use this port
							for( int i=1; i < MIDI_IN_PORTS; i++)
							{
								if( (pPortContainer == g_pMIDIInputContainer->m_aMIDIInputContainer[i].m_pPortContainer)
								&&	(g_pMIDIInputContainer->m_aMIDIInputContainer[i].m_fEchoMIDI) )
								{
									// Echo the MIDI data
									EchoBuffer( i, pDMBuffer );
								}
							}

							// If this port is used for MIDI input also
							if( pPortContainer == g_pMIDIInputContainer->m_aMIDIInputContainer[0].m_pPortContainer )
							{
								// Broadcast the MIDI data
								BYTE *pbMsg;
								REFERENCE_TIME rtTime;
								DWORD dwLength;
								pDMBuffer->ResetReadPtr();
								while ( pDMBuffer->GetNextEvent( &rtTime, NULL, &dwLength, &pbMsg ) == S_OK )
								{
									// process it
									IUnknown **pp;
									BYTE bStatus = pbMsg[0];
									BYTE bData1 = pbMsg[1];
									BYTE bData2 = pbMsg[2];

									if( bStatus & 0x80 && (bStatus & 0xF0) != 0xF0 )
									{
										if( (bStatus & 0xF0) == 0x90 && bData2 == 0 )
										{
											bStatus -= 0x10;
										}
										for( pp = g_pconductor->m_vec.begin();
										pp < g_pconductor->m_vec.end(); pp++ )
										{
											if( *pp )
											{
												((IDMUSProdMidiInCPt*)(*pp))->OnMidiMsg( rtTime, bStatus, bData1, bData2 );
											}
										}
									}
								}
							}
						}
					}
				}

				// Leave the MIDI input critical section
				::LeaveCriticalSection( &g_pMIDIInputContainer->m_csEchoMIDI );

				pDMBuffer->Release();

				if ( FAILED(hr) )
				{
					TRACE("EchoMIDIInThreadProc: Read failed with %x\n", hr);
					break;
				}
			}
		}
		else if ( dwRes != WAIT_TIMEOUT )
		{
			break;
		}
	}
	/*}
	catch( ... )
	{
		TRACE("Caught exception in EchoMIDIInThreadProc. Exiting.\n");
	}*/

	if( g_pMIDIInputContainer && g_pMIDIInputContainer->m_hEchoMIDIInputExitEvent )
	{
		//TRACE("EchoMIDIInThreadProc setting event.\n");
		::SetEvent( g_pMIDIInputContainer->m_hEchoMIDIInputExitEvent );
	}

	//TRACE("EchoMIDIInThreadProc exiting.\n");
	return 0;
}
