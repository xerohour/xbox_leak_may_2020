//#define DSOUND_INCLUDE_XHAWK
//#include <xtl.h>
//#include <dsound.h>


extern "C" {
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntos.h>
}


#include <xtl.h>
#include <dsound.h>
//#include <dsoundp.h>
#include <xvoice.h>
#include <xtestlib.h>
#include <xlog.h>
#include <xlogconio.h>
#include "hawk.h"

#define VERIFY_SUCCESS(xprsn) \
{\
    HRESULT hrTemp = (xprsn);\
    if(!SUCCEEDED(hrTemp))\
    {\
        DebugPrint("Call failed: %s(%d): 0x%0.8x\n", __FILE__, __LINE__, hrTemp);\
    }\
}

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


#if !defined(HARNESSLIB)
BOOL g_RunForever = TRUE;
BOOL g_Pause = FALSE;
#else
extern BOOL g_RunForever;
extern BOOL g_Pause;
#endif


// 
// Pink noise generator
//
PinkNumber::PinkNumber() {
	// Zero vars
	m_Index = 0;
	m_RunningSum = 0;
	ZeroMemory(m_Rows, sizeof(m_Rows));

	// Mask used to determin the number of trailing zeros in Index
	m_IndexMask = (1<<PINK_NOISE_ROWS) - 1;

	// Seed random number generator
	srand(timeGetTime());
}


PinkNumber::~PinkNumber() {
}


WORD PinkNumber::GetNextValue() {
	int NumZeros, n;
	WORD RandNum;

	// Imcrement and mask index
	m_Index++;
	m_Index &= m_IndexMask;

	// Update random values for Index !=0
	if(m_Index != 0) {
		// Determine the number of trailing zeros in Index
		NumZeros = 0;
		n = m_Index;
		while((n & 1) == 0) {
			n = n >> 1;
			NumZeros++;
		}

		// Use NumZeros to determine which row to update
		m_RunningSum = m_RunningSum - m_Rows[NumZeros];
		RandNum = (WORD)rand();
		m_RunningSum = m_RunningSum + RandNum;
		m_Rows[NumZeros] = RandNum;
	}

	// Add white noise and scale
	RandNum = (WORD)rand();
	return (m_RunningSum + RandNum)%PINK_MAX;
}

 
/*
**
**  Implementation of CHawkUnit that does loop back test
**  on a single unit.
**
*/
CHawkUnit::CHawkUnit():
        m_dwPortNumber(0),
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


BOOL CHawkUnit::IsConnected() {
	return m_fConnected;
}


void CHawkUnit::Init(DWORD dwPortNumber, HANDLE hEvent)
{
    m_dwPortNumber = dwPortNumber;

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

void CHawkUnit::Inserted(HANDLE LogHandle)
{
    int i;
	WAVEFORMATEX    wfxFormat;

	m_LogHandle = LogHandle;

    //
    // Headset initialization
    //
    wfxFormat.wFormatTag = WAVE_FORMAT_PCM;
    wfxFormat.nChannels = 1;
    wfxFormat.nSamplesPerSec = XHAWK_VOICE_SAMPLING_RATE;
    wfxFormat.nAvgBytesPerSec = 2*XHAWK_VOICE_SAMPLING_RATE;
    wfxFormat.nBlockAlign = 2;
    wfxFormat.wBitsPerSample = 16;
    wfxFormat.cbSize = 0;

    //
    //  Create a microphone and a headphone.
    //
    VERIFY_SUCCESS(XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, m_dwPortNumber , 
												XHAWK_TEST_BUFFER_COUNT, &wfxFormat, &m_pMicrophone));

    VERIFY_SUCCESS(XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, m_dwPortNumber , 
												XHAWK_TEST_BUFFER_COUNT, &wfxFormat, &m_pHeadphone));

    m_fConnected = TRUE;
    m_NextMicrophoneBufferIndex = 0;
    m_NextHeadphoneBufferIndex = 0;
    m_dwBuffersSent = 0;
    m_dwBuffersReceived = 0;
    m_dwLoopbackErrors = 0;
    
    //
    //  Initialize the status of all the buffer
    //
    for(i=0; i < XHAWK_TEST_BUFFER_COUNT; i++) {
        HeadphoneBuffers[i].dwStatus = (DWORD)XMEDIAPACKET_STATUS_PENDING;
        MicrophoneBuffers[i].dwStatus = (DWORD)XMEDIAPACKET_STATUS_PENDING;
    }

    //
    //  Start up the streaming
    //
    for(i=0; i < XHAWK_TEST_BUFFER_COUNT; i++) {
        //  Fill the headphone buffer with a test pattern
        for(int bufferOffset=0 ; bufferOffset < XHAWK_TEST_BUFFER_SIZE/sizeof(WORD); bufferOffset++) {
			((PWORD)HeadphoneBuffers[i].xMediaPacket.pvBuffer)[bufferOffset] = m_PinkNoise.GetNextValue();
        }
        
        //  Wipe the microphone buffer clean
        memset(MicrophoneBuffers[i].xMediaPacket.pvBuffer, 0, XHAWK_TEST_BUFFER_SIZE);
                
        //  Attach media buffer for headphone XMO's
        VERIFY_SUCCESS(m_pHeadphone->Process(&HeadphoneBuffers[i].xMediaPacket, NULL));
                
        //  For the first buffer, introduce a 10 ms delay, to give the loop back device
        //  a few cycles to process the first data.
        if(i==0) Sleep(25);

        //
        //  Attach media buffer for microphone
        VERIFY_SUCCESS(m_pMicrophone->Process(NULL, &MicrophoneBuffers[i].xMediaPacket));
    }

	m_fStarted = TRUE;
	xLog(gbConsoleOut, m_LogHandle, XLL_INFO, "Added Hawk: Port: %ld", m_dwPortNumber);
}


void CHawkUnit::StartStreaming() {
	int i;

	// Start streaming if connected and not started
	if(g_Pause) {
		// Applicatioin is paused, do not start streaming and clear Started flag
		m_fStarted = FALSE;
	} else {
		// Application is not paused, start streaming if connected and not started
		if(m_fConnected && !m_fStarted) {
			//  Start up the streaming
			for(i=0; i < XHAWK_TEST_BUFFER_COUNT; i++) {
				//  Fill the headphone buffer with a test pattern
				for(int bufferOffset=0 ; bufferOffset < XHAWK_TEST_BUFFER_SIZE/sizeof(WORD); bufferOffset++) {
					((PWORD)HeadphoneBuffers[i].xMediaPacket.pvBuffer)[bufferOffset] = m_PinkNoise.GetNextValue();
				}

				//  Wipe the microphone buffer clean
				memset(MicrophoneBuffers[i].xMediaPacket.pvBuffer, 0, XHAWK_TEST_BUFFER_SIZE);
                
				//  Attach media buffer for headphone XMO's
				VERIFY_SUCCESS(m_pHeadphone->Process(&HeadphoneBuffers[i].xMediaPacket, NULL));
                
				if(i==0) Sleep(25);

				//  Attach media buffer for microphone
				VERIFY_SUCCESS(m_pMicrophone->Process(NULL, &MicrophoneBuffers[i].xMediaPacket));
			}

			m_fStarted = TRUE;
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
	m_fStarted = FALSE;
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
        HeadphoneBuffers[m_NextHeadphoneBufferIndex].dwStatus = (DWORD)XMEDIAPACKET_STATUS_PENDING;
        m_NextHeadphoneBufferIndex = (m_NextHeadphoneBufferIndex+1)%XHAWK_TEST_BUFFER_COUNT;
    }

    //
    //  Handle a microphone completions
    //
    while(XMEDIAPACKET_STATUS_PENDING != MicrophoneBuffers[m_NextMicrophoneBufferIndex].dwStatus) {
		switch(MicrophoneBuffers[m_NextMicrophoneBufferIndex].dwStatus) {
		case XMEDIAPACKET_STATUS_SUCCESS:
			//  Record that the data was receieved
			m_dwBuffersReceived++;
			if(m_dwBuffersReceived > m_dwBuffersSent) {
				DebugPrint("ERROR: m_dwBuffersReceived > m_dwBuffersSent\n");
				_asm int 3;
			}
			//  Check for loopback errors
			/*
			if(0!=memcmp(MicrophoneBuffers[m_NextMicrophoneBufferIndex].xMediaPacket.pvBuffer, HeadphoneBuffers[m_NextMicrophoneBufferIndex].xMediaPacket.pvBuffer, XHAWK_TEST_BUFFER_SIZE)) {
				m_dwLoopbackErrors++;
				xLog(gbConsoleOut, m_LogHandle, XLL_FAIL, "Hawk: Port: %ld Microphone Loopback Error %d", m_dwPortNumber, m_dwLoopbackErrors);
			}
			*/
			break;

		case XMEDIAPACKET_STATUS_FLUSHED:
	        m_dwLoopbackErrors++;
			xLog(gbConsoleOut, m_LogHandle, XLL_FAIL, "Hawk: Port: %ld XMEDIAPACKET_STATUS_FLUSHED", m_dwPortNumber);
			break;

		case XMEDIAPACKET_STATUS_FAILURE:
	        m_dwLoopbackErrors++;
			xLog(gbConsoleOut, m_LogHandle, XLL_FAIL, "Hawk: Port: %ld XMEDIAPACKET_STATUS_FAILURE", m_dwPortNumber);
			break;
		}

        //
        //  Reset the status to pending so we don't accidentaly
        //  process twice.
        //
        MicrophoneBuffers[m_NextMicrophoneBufferIndex].dwStatus = (DWORD)XMEDIAPACKET_STATUS_PENDING;

        //
        //  If the device is still connected, resubmit the buffers.
		//  and not paused
        //
        if(m_fConnected && !g_Pause)
        {
            //  Fill the headphone buffer with a test pattern
            for(int bufferOffset=0 ; bufferOffset < XHAWK_TEST_BUFFER_SIZE/sizeof(WORD); bufferOffset++) {
				((PWORD)HeadphoneBuffers[m_NextMicrophoneBufferIndex].xMediaPacket.pvBuffer)[bufferOffset] = m_PinkNoise.GetNextValue();
            }
    
            //  Wipe the microphone buffer clean
            memset(MicrophoneBuffers[m_NextMicrophoneBufferIndex].xMediaPacket.pvBuffer, 0, XHAWK_TEST_BUFFER_SIZE);
            
            //  Attach media buffer for headphone XMO's
            VERIFY_SUCCESS(m_pHeadphone->Process(&HeadphoneBuffers[m_NextMicrophoneBufferIndex].xMediaPacket, NULL));
            
            //  Attach media buffer for microphone
            VERIFY_SUCCESS(m_pMicrophone->Process(NULL, &MicrophoneBuffers[m_NextMicrophoneBufferIndex].xMediaPacket));
        }

        //  Process the next microphone buffer, if it is ready.
        m_NextMicrophoneBufferIndex = (m_NextMicrophoneBufferIndex+1)%XHAWK_TEST_BUFFER_COUNT;
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
CHawkUnits::CHawkUnits() {   
    for(int i=0; i<4; i++) {
        m_pProcessEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        m_pHawkUnits[i].Init(i, m_pProcessEvents[i]);
    }        
}


CHawkUnits::~CHawkUnits(){
}


void CHawkUnits::Run(HANDLE LogHandle)
{
    int i;
    
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
            m_pHawkUnits[i].Inserted(LogHandle);
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "Added Hawk: Port: %ld", i);
        }
    }

    //
    //  Now loop forever
    //
    while(g_RunForever)
    {
		//  Start streaming (does nothing if: not paused, connected, and started
		for(i=0; i<4; i++) {
			m_pHawkUnits[i].StartStreaming();
		}
        
        //
        //  Wait until something happens, or 16 ms, whichever is first
        //
        DWORD dwReason = WaitForMultipleObjects(4, this->m_pProcessEvents, FALSE, 16);

        //
        //  If the wait expired due to one of the events, then call
        //  the appropriate hawk unit.
		//
		//	Do not .Process() if global Pause flag set.
        //
        if((dwReason >= WAIT_OBJECT_0) && (dwReason <= WAIT_OBJECT_0+3) && (!g_Pause)) {
		    int i = dwReason-WAIT_OBJECT_0;
            m_pHawkUnits[i].Process();
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
					xLog(gbConsoleOut, LogHandle, XLL_INFO, "Removed Hawk: Port: %ld", i);
                }

                //
                //  Handle Insertion, if there is one.
                //

                if(dwMicrophoneInsertions&(1<<i))
                {
                    m_pHawkUnits[i].Inserted(LogHandle);
                }
            } //end of loop over ports
        }  //end of check for getting device changes
    } //end of infinite loop
}

VOID
WINAPI
HawkStartTest(HANDLE LogHandle) {

	xSetOwnerAlias(LogHandle, "a-emebac");
    xSetComponent( LogHandle, "EMC", "Hawk" );
    xSetFunctionName( LogHandle, "LoopBack" );

    CHawkUnits hawkUnits;
    hawkUnits.Run(LogHandle);
}


VOID
WINAPI
HawkEndTest( VOID) {
}

#if !defined(HARNESSLIB)
int __cdecl main() {
	HANDLE	LogHandle = NULL;

	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);
	gbConsoleOut = TRUE;
	g_RunForever = TRUE;
	g_Pause = FALSE;

	// xlog
	LogHandle = xCreateLog_W(L"t:\\hawk.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("HAWK: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		return 0;
    }

	// Start Test
	xDebugStringA("All attached Hawk Units will be exercised.\n");
	XInitDevices(0, NULL);
	HawkStartTest(LogHandle);
	

	// End Test
	HawkEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Do the de-initialize that the harness does
	// dxconio
	xReleaseConsole();

	// Return to launcher
	XLaunchNewImage(NULL, NULL);
}
#endif

//
// Export function pointers of StartTest and EndTest
//
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( hawk )
#pragma data_seg()

BEGIN_EXPORT_TABLE( hawk )
    EXPORT_TABLE_ENTRY( "StartTest", HawkStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", HawkEndTest )
END_EXPORT_TABLE( hawk )
