#define DSOUND_INCLUDE_XHAWK

#include "loopback.h"


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
**  Implementation of CHawkUnit that does loop back test
**  on a single unit.
**
*/

CHawkUnit::CHawkUnit():
        m_dwPortNumber(0),
        m_iPosX(0),
        m_iPosY(0),
        m_pHeadphone(NULL),
        m_pMicrophone(NULL),
        m_dwBuffersSent(0),
        m_dwBuffersReceived(0),
        m_dwLoopbackErrors(0),
        m_fConnected(FALSE),
        m_fStarted(FALSE),
        m_dwPatternPosition(0),
        m_NextMicrophoneBufferIndex(0),
        m_NextHeadphoneBufferIndex(0)
{
}

void CHawkUnit::Init(DWORD dwPortNumber, INT iPosX, INT iPosY, HANDLE hEvent)
{
    m_dwPortNumber = dwPortNumber;
    m_iPosX = iPosX;
    m_iPosY = iPosY;
    
    //
    // Initialize the media buffers
    //

    for(int i=0; i <3; i++)
    {
        
        HeadphoneBuffers[i].xMediaPacket.dwMaxSize  = XHAWK_TEST_BUFFER_SIZE;
        HeadphoneBuffers[i].xMediaPacket.pvBuffer = new BYTE[XHAWK_TEST_BUFFER_SIZE];
        HeadphoneBuffers[i].xMediaPacket.pdwCompletedSize = &HeadphoneBuffers[i].dwCompletedSize;
        HeadphoneBuffers[i].xMediaPacket.pdwStatus = &HeadphoneBuffers[i].dwStatus;
        HeadphoneBuffers[i].xMediaPacket.hCompletionEvent = hEvent;
        

        MicrophoneBuffers[i].xMediaPacket.dwMaxSize  = XHAWK_TEST_BUFFER_SIZE;
        MicrophoneBuffers[i].xMediaPacket.pvBuffer = new BYTE[XHAWK_TEST_BUFFER_SIZE];
        MicrophoneBuffers[i].xMediaPacket.pdwCompletedSize = &MicrophoneBuffers[i].dwCompletedSize;
        MicrophoneBuffers[i].xMediaPacket.pdwStatus = &MicrophoneBuffers[i].dwStatus;
        MicrophoneBuffers[i].xMediaPacket.hCompletionEvent = hEvent;

        if(NULL == HeadphoneBuffers[i].xMediaPacket.pvBuffer)
        {
            DebugPrint("Buffer allocation Failed (Headphone Buffer #%d)\n", i);
        }
        if(NULL == MicrophoneBuffers[i].xMediaPacket.pvBuffer)
        {
            DebugPrint("Buffer allocation Failed (Microphone Buffer #%d)\n", i);
        }
    }
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
    
    for(int i=0; i <3; i++)
    {
        delete MicrophoneBuffers[i].xMediaPacket.pvBuffer;
        delete HeadphoneBuffers[i].xMediaPacket.pvBuffer;
    }
}

void CHawkUnit::Inserted()
{
    Log(L"Hawk Inserted");
    int i;
    //
    //  Create a microphone and a headphone.
    //
    VERIFY_SUCCESS(XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, m_dwPortNumber , XHAWK_TEST_BUFFER_COUNT, NULL, &m_pMicrophone));
    VERIFY_SUCCESS(XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, m_dwPortNumber , XHAWK_TEST_BUFFER_COUNT, NULL, &m_pHeadphone));
    m_fConnected = TRUE;

    m_NextMicrophoneBufferIndex = 0;
    m_NextHeadphoneBufferIndex = 0;
    m_dwBuffersSent = 0;
    m_dwBuffersReceived = 0;
    m_dwLoopbackErrors = 0;
    
    //
    //  Initialize the status of all the buffer
    //
    for(i=0; i < XHAWK_TEST_BUFFER_COUNT; i++)
    {
        HeadphoneBuffers[i].dwStatus = XMEDIAPACKET_STATUS_PENDING;
        MicrophoneBuffers[i].dwStatus = XMEDIAPACKET_STATUS_PENDING;
    }

    //
    //  Start up the streaming
    //
    for(i=0; i < XHAWK_TEST_BUFFER_COUNT; i++)
    {
        //
        //  Fill the headphone buffer with a test pattern
        //
        for(int bufferOffset=0 ; bufferOffset < XHAWK_TEST_BUFFER_SIZE/sizeof(DWORD); bufferOffset++)
        {
            ((PDWORD)HeadphoneBuffers[i].xMediaPacket.pvBuffer)[bufferOffset] = m_dwPatternPosition++;
        }
        
        //
        //  Wipe the microphone buffer clean
        //
        memset(MicrophoneBuffers[i].xMediaPacket.pvBuffer, 0, XHAWK_TEST_BUFFER_SIZE);
                
        //
        //  Attach media buffer for headphone XMO's
        //
        VERIFY_SUCCESS(m_pHeadphone->Process(&HeadphoneBuffers[i].xMediaPacket, NULL));
                
        //
        //  For the first buffer, introduce a 10 ms delay, to give the loop back device
        //  a few cycles to process the first data.
        //
        if(i==0) Sleep(25);

        //
        //  Attach media buffer for microphone
        //
        VERIFY_SUCCESS(m_pMicrophone->Process(NULL, &MicrophoneBuffers[i].xMediaPacket));
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
    every buffer, we do not know whether th microphone or
    headphone completed.

--*/
{
    
    //
    //  Handle a headphone completions
    //
    while(XMEDIAPACKET_STATUS_PENDING != HeadphoneBuffers[m_NextHeadphoneBufferIndex].dwStatus)
    {
        if(XMEDIAPACKET_STATUS_SUCCESS == HeadphoneBuffers[m_NextHeadphoneBufferIndex].dwStatus)
        {
            m_dwBuffersSent++;
        }
        HeadphoneBuffers[m_NextHeadphoneBufferIndex].dwStatus = XMEDIAPACKET_STATUS_PENDING;
        m_NextHeadphoneBufferIndex = (m_NextHeadphoneBufferIndex+1)%XHAWK_TEST_BUFFER_COUNT;
    }

    //
    //  Handle a microphone completions
    //
    while(XMEDIAPACKET_STATUS_PENDING != MicrophoneBuffers[m_NextMicrophoneBufferIndex].dwStatus)
    {
        int i = m_NextMicrophoneBufferIndex; //save some typing

        //
        //  only do the rest on success
        //
        if(XMEDIAPACKET_STATUS_SUCCESS == MicrophoneBuffers[i].dwStatus)
        {
            //
            //  Record that the data was receieved
            //
            m_dwBuffersReceived++;
            if(m_dwBuffersReceived > m_dwBuffersSent)
            {
                DebugPrint("ERROR: m_dwBuffersReceived > m_dwBuffersSent\n");
                _asm int 3;
            }

            //
            //  Check for loopback errors
            //
            if(0!=memcmp(
                    MicrophoneBuffers[i].xMediaPacket.pvBuffer,
                    HeadphoneBuffers[i].xMediaPacket.pvBuffer,
                    XHAWK_TEST_BUFFER_SIZE)
            ) {
                m_dwLoopbackErrors++;
            }
        } else
        {
            DebugPrint("ERROR: Buffer transmission failure\n");
            m_dwLoopbackErrors++;
        }

        //
        //  Reset the status to pending so we don't accidentaly
        //  process twice.
        //

        MicrophoneBuffers[i].dwStatus = XMEDIAPACKET_STATUS_PENDING;

        //
        //  If the device is still connected, resubmit the buffers.
        //

        if(m_fConnected)
        {
            //
            //  Fill the headphone buffer with a test pattern
            //
            for(int bufferOffset=0 ; bufferOffset < XHAWK_TEST_BUFFER_SIZE/sizeof(DWORD); bufferOffset++)
            {
                ((PDWORD)HeadphoneBuffers[i].xMediaPacket.pvBuffer)[bufferOffset] = m_dwPatternPosition++;
            }
    
            //
            //  Wipe the microphone buffer clean
            //
            memset(MicrophoneBuffers[i].xMediaPacket.pvBuffer, 0, XHAWK_TEST_BUFFER_SIZE);
            
            //
            //  Attach media buffer for headphone XMO's
            //
            VERIFY_SUCCESS(m_pHeadphone->Process(&HeadphoneBuffers[i].xMediaPacket, NULL));
            
            //
            //  Attach media buffer for microphone
            //
            VERIFY_SUCCESS(m_pMicrophone->Process(NULL, &MicrophoneBuffers[i].xMediaPacket));
        }

        //
        //  Process the next microphone buffer, if it is ready.
        //

        m_NextMicrophoneBufferIndex = (m_NextMicrophoneBufferIndex+1)%XHAWK_TEST_BUFFER_COUNT;
    }
}    

void CHawkUnit::Draw(CDraw& draw)
{
    WCHAR wcBuffer[80];
    DWORD dwColor;
    
    if(m_fConnected)
    {
        //dwColor = CONNECTED_YELLOW;
        dwColor = 0xFFFFFF;
    }else
    {
        //dwColor = DISCONNECTED_BLUE;
        dwColor = 0xFFFFFF;
    }

    for(int i=0; i<2; i++)
    {
        draw.FillRect(m_iPosX, m_iPosY, 150, 150, BACKDROP_BLUE);
        wsprintf(wcBuffer, L"Port %d: %ws", m_dwPortNumber, (m_fConnected ? L"Connected" : L"Disconnected") );
        draw.DrawText(wcBuffer, m_iPosX, m_iPosY, dwColor);
        wsprintf(wcBuffer, L"Buffer Sent = %d", m_dwBuffersSent);
        draw.DrawText(wcBuffer, m_iPosX+10, m_iPosY+15, dwColor);
        wsprintf(wcBuffer, L"Buffer Received = %d", m_dwBuffersReceived);
        draw.DrawText(wcBuffer, m_iPosX+10, m_iPosY+30, dwColor);
        wsprintf(wcBuffer, L"Loopback Errors = %d", m_dwLoopbackErrors);
        draw.DrawText(wcBuffer, m_iPosX+10, m_iPosY+45, dwColor);
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
                          (i<2) ? 15 : 245,
                          m_pProcessEvents[i]
                          );
    }        
}
CHawkUnits::~CHawkUnits(){}

void CHawkUnits::Run(CDraw& draw)
{
    int i;
    draw.FillRectNow(0,0, 480,640, BACKDROP_BLUE);
    DebugPrint("XInput Test Application.\nBuilt on %s at %s\n", __DATE__, __TIME__);
    draw.FillRectNow(0, 0, 640, 480, BACKDROP_BLUE); 
    draw.DrawTextNow(L"xHawk Audio Loopback Test", 300,  0, LABEL_WHITE);
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
