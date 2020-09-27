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

void CHawkUnit::Init(DWORD dwPortNumber, INT iPosX, INT iPosY, HANDLE hEvent)
{
    m_dwPortNumber = dwPortNumber;
    m_iPosX = iPosX;
    m_iPosY = iPosY;
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
    DWORD i;
    HRESULT hr;

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
    
            //
            //  Initialize the status of all the contexts
            //

            for(i=0; i < 2; i++)
            {
                m_MicrophoneContexts[i].dwStatus = XMEDIAPACKET_STATUS_PENDING;
                m_HeadphoneContexts[i].dwStatus = XMEDIAPACKET_STATUS_PENDING;
            }

            //
            //  Attach two microphone contexts off the bat.
            //

            for(i=0; i < 2; i++)
            {
                AttachMicrophoneContext(i);
            }
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
}

void CHawkUnit::Process()
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

            if(m_fPreBuffering)
            {
                if(XHAWK_PLAYBACK_PREBUFFER_REGIONS==++m_dwPreBufferRegions)
                {
                    m_fPreBuffering = FALSE;
                    for(DWORD i=0;i<2;i++)
                    {
                        AttachHeadphoneContext(i);
                    }
                }
            } //m_fPreBuffering
        } //m_fConnected
    }   //if microphone context no longer pending

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
    
void CHawkUnit::Draw(CDraw& draw)
{
    WCHAR wcBuffer[80];
    DWORD dwColor;
    
    if(m_fConnected)
    {
        dwColor = CONNECTED_YELLOW;
    }else
    {
        dwColor = DISCONNECTED_BLUE;
    }

    for(int i=0; i<2; i++)
    {
        draw.FillRect(m_iPosX, m_iPosY, 250, 150, BACKDROP_BLUE);
        
        wsprintf(wcBuffer, L"Port %d: %ws", m_dwPortNumber, (m_fConnected ? L"Connected" : L"Disconnected") );
        draw.DrawText(wcBuffer, m_iPosX, m_iPosY, dwColor);
        
        if(m_fPreBuffering)
        {
            wsprintf(wcBuffer, L"pre-buffering data: %d ms remaining", (XHAWK_PLAYBACK_PREBUFFER_REGIONS - m_dwPreBufferRegions)*XHAWK_PLAYBACK_BUFFER_REGION_TIME);
        } else
        {
            wsprintf(wcBuffer, L"playing with %d ms delay", XHAWK_PLAYBACK_PREBUFFER_REGIONS*XHAWK_PLAYBACK_BUFFER_REGION_TIME);
        }
        draw.DrawText(wcBuffer, m_iPosX+10, m_iPosY+15, dwColor);

        wsprintf(wcBuffer, L"Buffers Received = %d", m_dwBuffersReceived);
        draw.DrawText(wcBuffer, m_iPosX+10, m_iPosY+30, dwColor);

        wsprintf(wcBuffer, L"Bytes Received = %d", m_dwBytesReceived);
        draw.DrawText(wcBuffer, m_iPosX+10, m_iPosY+45, dwColor);

        wsprintf(wcBuffer, L"Buffers Sent = %d", m_dwBuffersSent);
        draw.DrawText(wcBuffer, m_iPosX+10, m_iPosY+60, dwColor);

        wsprintf(wcBuffer, L"Bytes Sent = %d", m_dwBytesSent);
        draw.DrawText(wcBuffer, m_iPosX+10, m_iPosY+75, dwColor);

        wsprintf(wcBuffer, L"Errors = %d", m_dwErrorCount);
        draw.DrawText(wcBuffer, m_iPosX+10, m_iPosY+90, dwColor);
        
        draw.Present();
    } 
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
    for(int i=0; i<4; i++)
    {
        m_pProcessEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        m_pHawkUnits[i].Init(
                          i,
                          (0==i%2) ? 5 : 325,
                          (i<2) ? 45 : 245,
                          m_pProcessEvents[i]
                          );
    }        
}
CHawkUnits::~CHawkUnits(){}

void CHawkUnits::Run(CDraw& draw)
{
    int i;
    WCHAR FormatBuffer[255];

    draw.FillRectNow(0,0,640,480, BACKDROP_BLUE);
    DebugPrint("XHawk Test Application.\nBuilt on %s at %s\n", __DATE__, __TIME__);
    wsprintf(FormatBuffer,L"Hawk Audio Play-back Test.");
    draw.DrawTextNow(FormatBuffer, 0,  0, LABEL_WHITE);
    wsprintf(FormatBuffer,L"Rate = %0.3f ksamples/second.", TEST_HAWK_SAMPLING_RATE/1000.0);
    draw.DrawTextNow(FormatBuffer, 2,  14, LABEL_WHITE);
    wsprintf(FormatBuffer,L"Built on %hs at %hs", __DATE__, __TIME__);
    draw.DrawTextNow(FormatBuffer, 2,  28, LABEL_WHITE);
    draw.Present();
    
    DWORD dwConnectedMicrophones;
    DWORD dwConnectedHeadphones;
    

    //
    //  Get the initial connected state of the devices
    //

    dwConnectedMicrophones = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);
    dwConnectedHeadphones = XGetDevices(XDEVICE_TYPE_VOICE_HEADPHONE);
    if( dwConnectedMicrophones != dwConnectedHeadphones)
    {
        DebugPrint("dwConnectedMicrophones != dwConnectedHeadphone\n");
        _asm int 3;
    }
    for(i=0; i < 4; i++)
    {
        if(dwConnectedMicrophones&(1<<i))
        {
            m_pHawkUnits[i].Inserted();
        }
        m_pHawkUnits[i].Draw(draw);
    }

    //
    //  Now loop forever
    //
    while(1)
    {
        
        //
        //  Wait until something happens, or 16 ms, whichever is first
        //
        
        DWORD dwReason = WaitForMultipleObjects(4, this->m_pProcessEvents, FALSE, 16);

        //
        //  If the wait expired due to one of the events, then call
        //  the appropriate hawk unit.
        //

        if(
            (dwReason >= WAIT_OBJECT_0) &&
            (dwReason <= WAIT_OBJECT_0+3)
          )
        {
            int i = dwReason-WAIT_OBJECT_0;
            m_pHawkUnits[i].Process();
            m_pHawkUnits[i].Draw(draw);
        }

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
            if(
                (dwMicrophoneInsertions != dwHeadphoneInsertions) ||
                (dwMicrophoneRemovals != dwHeadphoneRemovals)
            ){
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
                    m_pHawkUnits[i].Draw(draw);
                }

                //
                //  Handle Insertion, if there is one.
                //

                if(dwMicrophoneInsertions&(1<<i))
                {
                    m_pHawkUnits[i].Inserted();
                    m_pHawkUnits[i].Draw(draw);
                }
            } //end of loop over ports
        }  //end of check for getting device changes
    } //end of infinite loop
}
