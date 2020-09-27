#define DSOUND_INCLUDE_XHAWK

#include "playback.h"

#if TEST_HAWK_SAMPLING_RATE == 8000
#pragma message ("Playback built for 8 ksamples/second")
#endif

#if TEST_HAWK_SAMPLING_RATE == 11025
#pragma message ("Playback built for 11.025 ksamples/second")
#endif

#if TEST_HAWK_SAMPLING_RATE == 16000
#pragma message ("Playback built for 16 ksamples/second")
#endif

#if TEST_HAWK_SAMPLING_RATE == 22050
#pragma message ("Playback built for 22.05 ksamples/second")
#endif

#if TEST_HAWK_SAMPLING_RATE == 24000
#pragma message ("Playback built for 24 ksamples/second")
#endif

// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}


/*
**
**  Implementation of CHawkUnit that does play back test
**  on a single unit.
**
*/

void CHawkUnit::Init(DWORD dwPortNumber, HANDLE hEvent)
{
    m_dwPortNumber = dwPortNumber;
    m_hEvent = hEvent;
}

CHawkUnit::~CHawkUnit()
{
    if(m_pHeadphone)
    {
        m_pHeadphone->Release();
    }
    if(m_pMicrophone)
    {
        m_pMicrophone->Release();
    }
}

void CHawkUnit::Inserted()
{
    Log(L"Hawk Inserted");
     HRESULT hr = 0;

    WAVEFORMATEX wfxFormat;
    wfxFormat.wFormatTag = WAVE_FORMAT_PCM;
    wfxFormat.nChannels = 1;
    wfxFormat.nSamplesPerSec = TEST_HAWK_SAMPLING_RATE;
    wfxFormat.nAvgBytesPerSec = 2*TEST_HAWK_SAMPLING_RATE;
    wfxFormat.nBlockAlign = 2;
    wfxFormat.wBitsPerSample = 16;
    wfxFormat.cbSize = 0;

    //
    //  Create a microphone and a headphone.
    //
    hr = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, m_dwPortNumber , 2, &wfxFormat, &m_pMicrophone);
    if(SUCCEEDED(hr))
    {
       hr = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, m_dwPortNumber , 2, &wfxFormat, &m_pHeadphone);
       if(SUCCEEDED(hr))
       {
            m_dwMicrophoneRegionIndex = 0;
            m_dwHeadphoneRegionIndex = 0;
            m_dwMicrophoneContextIndex = 0;
            m_dwHeadphoneContextIndex = 0;
            m_fConnected = TRUE;
            m_fPreBuffering = TRUE;
            m_dwPreBufferRegions = 0;
            m_dwBuffersReceived = 0;
            m_dwBytesReceived = 0;
            m_dwBytesSent = 0;
            m_dwErrorCount = 0;
    
            
       } else
       {
            m_pMicrophone->Release();
            m_pMicrophone = NULL;
       }
    }
}


void CHawkUnit::Removed()
{
    if(m_pMicrophone)
    {
        m_pMicrophone->Release();
    }
    if(m_pHeadphone)
    {
        m_pHeadphone->Release();
    }
    m_fConnected = FALSE;
	m_dwBytesSent = 0;
	m_dwBytesReceived  = 0;
	m_dwBuffersReceived = 0;
	m_dwErrorCount = 0;

}



int CHawkUnit::ProcessMicrophone()
/*++
    
    This routine is called because this XMO's event
    was signaled.  Since we pass the same event with
    every buffer, we do not know whether the microphone or
    headphone completed.

--*/
{
   
    //
    //  Check for a completed microphone context first
    //
	if(m_dwBytesReceived >= XHAWK_PLAYBACK_BUFFER_SIZE)
			return 0;
    if(XMEDIAPACKET_STATUS_PENDING != m_MicrophoneContexts[m_dwMicrophoneContextIndex].dwStatus)
    {
        if(SUCCEEDED(m_MicrophoneContexts[m_dwMicrophoneContextIndex].dwStatus))
        {
            m_dwBuffersReceived++;
        } else
        {
            m_dwErrorCount++;
        }

        m_dwBytesReceived += m_MicrophoneContexts[m_dwMicrophoneContextIndex].dwCompletedSize;
        
        m_MicrophoneContexts[m_dwMicrophoneContextIndex].dwStatus = XMEDIAPACKET_STATUS_PENDING;
		
		if(m_dwBytesReceived >= XHAWK_PLAYBACK_BUFFER_SIZE)
			return 0;
        //
        //  If we are still connected, reattach the context
        //

        if(m_fConnected)
        {
            //
            //  Increment m_dwMicrophoneContextIndex
            //

            DWORD index = m_dwMicrophoneContextIndex;
            m_dwMicrophoneContextIndex = (m_dwMicrophoneContextIndex+1)%2;
            
            //
            //  Reattach the microphone context that just completed.
            //

            AttachMicrophoneContext(index);
            
            //
            //  If we prebuffer is complete, switch off the prebuffer flag
            //  and attach two headphone contexts.
            //

            
        } //m_fConnected
    }   //if microphone context no longer pending
	return 1;

}    

int CHawkUnit::ProcessHeadphones()
/*++
    
    This routine is called because this XMO's event
    was signaled.  Since we pass the same event with
    every buffer, we do not know whether the microphone or
    headphone completed.

--*/
{
   
	if(m_dwBytesSent >= XHAWK_PLAYBACK_BUFFER_SIZE)
			return 0;
    //
    //  Check for a completed headphone context first
    //

    if(XMEDIAPACKET_STATUS_PENDING != m_HeadphoneContexts[m_dwHeadphoneContextIndex].dwStatus)
    {
        if(SUCCEEDED(m_HeadphoneContexts[m_dwHeadphoneContextIndex].dwStatus))
        {
            m_dwBuffersSent++;
        } else
        {
            m_dwErrorCount++;
        }
        
        m_dwBytesSent += m_HeadphoneContexts[m_dwHeadphoneContextIndex].dwCompletedSize;

        m_HeadphoneContexts[m_dwHeadphoneContextIndex].dwStatus = XMEDIAPACKET_STATUS_PENDING;
		if(m_dwBytesSent >= XHAWK_PLAYBACK_BUFFER_SIZE)
			return 0;
        //
        //  If we are still connected, reattach the context
        //
        if(m_fConnected)
        {
            //
            //  Increment m_dwHeadphoneContextIndex
            //

            DWORD index = m_dwHeadphoneContextIndex;
            m_dwHeadphoneContextIndex = (m_dwHeadphoneContextIndex+1)%2;
            
            //
            //  Reattach the microphone context that just completed.
            //

            AttachHeadphoneContext(index);
        }
    }
	return 1;
}    



void CHawkUnit::AttachMicrophoneContext(DWORD dwIndex)
{
    XMEDIAPACKET MediaPacket;

     //
    //  Seup the size and pointer into our buffer
    //

    MediaPacket.dwMaxSize = XHAWK_PLAYBACK_BUFFER_REGION_SIZE;
    MediaPacket.pvBuffer = (PVOID)m_Buffer[m_dwMicrophoneRegionIndex].Data;

    //
    //  Adjust the region index for the next call
    //

    m_dwMicrophoneRegionIndex = (m_dwMicrophoneRegionIndex+1)%XHAWK_PLAYBACK_TOTAL_REGIONS;

    //
    //  Setup the pointers to our status and completed size pointers
    //

    MediaPacket.pdwCompletedSize = &m_MicrophoneContexts[dwIndex].dwCompletedSize;
    MediaPacket.pdwStatus = &m_MicrophoneContexts[dwIndex].dwStatus;

    //
    //  Initialize the event and timestamp
    //

    MediaPacket.hCompletionEvent = m_hEvent;

    //
    //  Submit the buffer
    //
    
    VERIFY_SUCCESS(m_pMicrophone->Process(NULL, &MediaPacket));
}

void CHawkUnit::AttachHeadphoneContext(DWORD dwIndex)
{
    XMEDIAPACKET MediaPacket;
    
    //
    //  Seup the size and pointer into our buffer
    //

    MediaPacket.dwMaxSize = XHAWK_PLAYBACK_BUFFER_REGION_SIZE;
    MediaPacket.pvBuffer = (PVOID)m_Buffer[m_dwHeadphoneRegionIndex].Data;

    //
    //  Adjust the region index for the next call
    //

    m_dwHeadphoneRegionIndex = (m_dwHeadphoneRegionIndex+1)%XHAWK_PLAYBACK_TOTAL_REGIONS;

    
    //
    //  Setup the pointers to our status and completed size pointers
    //

    MediaPacket.pdwCompletedSize = &m_HeadphoneContexts[dwIndex].dwCompletedSize;
    MediaPacket.pdwStatus = &m_HeadphoneContexts[dwIndex].dwStatus;

    //
    //  Initialize the event and timestamp
    //

    MediaPacket.hCompletionEvent = m_hEvent;

    //
    //  Submit the buffer
    //
    
    VERIFY_SUCCESS(m_pHeadphone->Process(&MediaPacket, NULL));
}
    


void CHawkUnit::Log(WCHAR *pwstrText)
{
    DebugPrint("%ws\n", pwstrText);
}

/*
**
**  Implementation of CHawkUnits that tracks multiple hawk units.
**
*/

CHawkUnits::CHawkUnits()
{  
	m_iNumConnected = 0;
	m_fRecording = FALSE;
	m_fPlaying = FALSE;
    for(int i=0; i<4; i++)
    {
        m_pProcessEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        m_pHawkUnits[i].Init(i,m_pProcessEvents[i]);
    }        
}
CHawkUnits::~CHawkUnits(){}


int CHawkUnits::FindConnections()
{
	int NumConnected = 0;
	int i;
	//
	//  Always check for hot-plugs
	//
	DWORD dwMicrophoneInsertions, dwHeadphoneInsertions, dwMicrophoneRemovals, dwHeadphoneRemovals;
	if(XGetDeviceChanges(XDEVICE_TYPE_VOICE_MICROPHONE, &dwMicrophoneInsertions, &dwMicrophoneRemovals))
	{
		if(!XGetDeviceChanges(XDEVICE_TYPE_VOICE_HEADPHONE, &dwHeadphoneInsertions, &dwHeadphoneRemovals))
		{
			DebugPrint("GetDeviceChanges does not match\n");
			_asm int 3;
		}
		if((dwMicrophoneInsertions != dwHeadphoneInsertions) ||
			(dwMicrophoneRemovals != dwHeadphoneRemovals))
		{
			DebugPrint("GetDeviceChanges does not match, different\n");
			_asm int 3;
		}
		//
		//  Loop over all the ports
		//
		for(i=0; i<4; i++)
		{
			//
			//  Handle Removal, if there is one.
			//
			if(dwMicrophoneRemovals&(1<<i))
			{
				m_pHawkUnits[i].Removed();
				NumConnected++;
				m_iNumConnected--;
				m_fRecording = FALSE;
				m_fPlaying = FALSE;           
			}
			//
			//  Handle Insertion, if there is one.
			//
			if(dwMicrophoneInsertions&(1<<i))
			{
				m_pHawkUnits[i].Inserted();
				NumConnected++;
				m_iNumConnected++;
				m_fRecording = FALSE;
				m_fPlaying = FALSE;
			}
		} //end of loop over ports
	}  //end of check for getting device changes
	return NumConnected;
}


int CHawkUnits::RecordSound(int iTime)
{
		if(!m_fRecording)
		{
			//
            //  Initialize the status of all the contexts
            //
			for(int i=0;i<4;i++)
			{
				if(m_pHawkUnits[i].IsConnected())
				{
					for(DWORD j=0;j<2;j++)
					{
						m_pHawkUnits[i].m_MicrophoneContexts[j].dwStatus = XMEDIAPACKET_STATUS_PENDING;
						m_pHawkUnits[i].m_HeadphoneContexts[j].dwStatus = XMEDIAPACKET_STATUS_PENDING;
					}
					//
					//  Attach two microphone contexts off the bat.
					//

					for(j=0; j < 2; j++)
					{
						m_pHawkUnits[i].AttachMicrophoneContext(j);
					}
					m_fRecording = TRUE;
				}
			}
		}
		if(!m_fRecording)
			return 0;
        //
        //  Wait until something happens, or 16 ms, whichever is first
        //
        
        DWORD dwReason = WaitForMultipleObjects(4, this->m_pProcessEvents, FALSE, 16);

        //
        //  If the wait expired due to one of the events, then call
        //  the appropriate hawk unit.
        //
        if((dwReason >= WAIT_OBJECT_0) && (dwReason <= WAIT_OBJECT_0+3))
        {
            int i = dwReason-WAIT_OBJECT_0;
            if(!m_pHawkUnits[i].ProcessMicrophone())
			{
				m_fRecording = FALSE;
				return 0;
			}			
        }
		return 1;
}

int CHawkUnits::PlaySound()
{
		if(!m_fPlaying)
		{
			for(int i=0;i<4;i++)
			{
				if(m_pHawkUnits[i].IsConnected())
				{
					for(DWORD j=0;j<2;j++)
					{
					   m_pHawkUnits[i].AttachHeadphoneContext(j);
					}
					m_fPlaying = TRUE;
				}
			}
		}
		if(!m_fPlaying)
			return 0;
        //
        //  Wait until something happens, or 16 ms, whichever is first
        //       
        DWORD dwReason = WaitForMultipleObjects(4, this->m_pProcessEvents, FALSE, 16);
        //
        //  If the wait expired due to one of the events, then call
        //  the appropriate hawk unit.
        //
        if((dwReason >= WAIT_OBJECT_0) && (dwReason <= WAIT_OBJECT_0+3))
        {
            int i = dwReason-WAIT_OBJECT_0;
            if(!m_pHawkUnits[i].ProcessHeadphones())
			{
				m_fPlaying = FALSE;
				return 0;
			}		
        }
		return 1;
}
