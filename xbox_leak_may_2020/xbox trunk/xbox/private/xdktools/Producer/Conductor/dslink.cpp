#include "stdafx.h"
#include <mmsystem.h>
#include "dsoundp.h"
#include "dmusici.h"
#include "dslink.h"
#include "audiosink.h"
#include "float.h"
#include "effects.h"
#include "phoneyds.h"
#include "cconduct.h"
#include "fstream.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DWORD g_dwLatency = DEFAULT_PHONEY_DS_LATENCY;

#ifdef _DEBUG
LogClass g_publicLogClass;
#endif

//#define _FILE_DEBUG 1
#ifdef _FILE_DEBUG
ofstream ofOutput;

void FTRACE( LPSTR pszFormat, ... )
{
	char strText[4096];
	va_list va;

	va_start(va, pszFormat);
	vsprintf(strText, pszFormat, va);
	va_end(va);

	ofOutput.write(strText, strlen( strText ));
}
#endif

bool g_fKslInitialized = false;
TCHAR g_szPhoneyDSoundFilterName[MAX_PATH];       // Filter path

#ifdef _DEBUG
UINT g_nLogLevel = 5;

int LogClass::Log( UINT nLevel, LPSTR pszFormat, ...)
{
	if( nLevel > g_nLogLevel )
	{
		return 0;
	}

	char strText[4096];
	va_list va;

	va_start(va, pszFormat);
	vsprintf(strText, pszFormat, va);
	va_end(va);
	TRACE("%d: %s\n", nLevel, strText);
	return 0;
}
int LogClass::LogEx( DWORD dwType, UINT nLevel, LPSTR pszFormat, ...)
{
	char strText[4096];
	va_list va;

	va_start(va, pszFormat);
	vsprintf(strText, pszFormat, va);
	va_end(va);

	TRACE("%d %d: %s\n", dwType, nLevel, strText);
	return 0;
}
int LogClass::LogSummary( UINT nLevel, LPSTR pszFormat, ...)
{
	char strText[4096];
	va_list va;

	va_start(va, pszFormat);
	vsprintf(strText, pszFormat, va);
	va_end(va);

	TRACE("%d: %s\n", nLevel, strText);
	return 0;
}
int LogClass::LogStatusBar( LPSTR pszFormat, ...)
{
	char strText[4096];
	va_list va;

	va_start(va, pszFormat);
	vsprintf(strText, pszFormat, va);
	va_end(va);

	TRACE("%s\n", strText);
	return 0;
}
void LogClass::LogStdErrorString(DWORD dwErrorCode)
{
	TRACE("Standard error %d\n", dwErrorCode);
	return;
}
#endif // _DEBUG

#ifndef USE_LOOPING_BUFFERS
void MyLock( LONGLONG llWriteStart, DWORD dwWriteCount, BYTE **ppbBuffer, DWORD dwBufferSize, LPVOID *ppStart, DWORD *pdwStart, LPVOID *ppEnd, DWORD *pdwEnd )
{
	int nBufferToUse = int((llWriteStart / dwBufferSize) % NUM_FRAMES);

	DWORD dwWriteStart = DWORD(llWriteStart % dwBufferSize);

	*pdwStart = min( dwWriteCount, dwBufferSize - dwWriteStart );
	*pdwEnd = dwWriteCount - *pdwStart;

	*ppEnd = ppbBuffer[(nBufferToUse + 1) % NUM_FRAMES];
	*ppStart = &(ppbBuffer[nBufferToUse][dwWriteStart]);

	//TRACE("MyLock: %d %I64d, %d, %x:%d, %x:%d\n", nBufferToUse, llWriteStart, dwWriteCount, *ppStart, *pdwStart, *ppEnd, *pdwEnd );
}
#endif //USE_LOOPING_BUFFERS

#ifdef USE_LOOPING_BUFFERS
void MyLoopingLock( LONGLONG llWriteStart, DWORD dwWriteCount, BYTE *pbBuffer, DWORD dwBufferSize, LPVOID *ppStart, DWORD *pdwStart, LPVOID *ppEnd, DWORD *pdwEnd )
{
	DWORD dwWriteStart = DWORD(llWriteStart % dwBufferSize);

	*pdwStart = min( dwWriteCount, dwBufferSize - dwWriteStart );
	*pdwEnd = dwWriteCount - *pdwStart;

	*ppEnd = pbBuffer;
	*ppStart = &(pbBuffer[dwWriteStart]);

	//TRACE("MyLoopingLock: %I64d, %d, %x:%d, %x:%d\n", llWriteStart, dwWriteCount, *ppStart, *pdwStart, *ppEnd, *pdwEnd );
}
#endif //USE_LOOPING_BUFFERS

void CDSLink::ReadBuffer()
{
    ::EnterCriticalSection(&m_CriticalSection);

    if (!m_fActive || !m_pPCMAudioPin /*|| !m_pIMasterClock*/)
    {
        TRACE("Warning: SynthSink - Thread in invalid state\n");
        ::LeaveCriticalSection(&m_CriticalSection);
        return;
    }

	KSSTATE ksState;
	if( m_pPCMAudioPin->GetState( &ksState )
	&&	ksState != KSSTATE_RUN )
	{
        TRACE("Warning: SynthSink - Pin not running\n");
        ::LeaveCriticalSection(&m_CriticalSection);
        return;
	}

	KSAUDIO_POSITION ksAudioPosition;
	if( m_pPCMAudioPin->GetPosition( &ksAudioPosition ) )
	{
		//TRACE("Position: %I64d %I64d %I64d\n", ksAudioPosition.PlayOffset, ksAudioPosition.WriteOffset, m_llAbsWrite );
#ifdef _FILE_DEBUG
		FTRACE("Position: %d %I64d %I64d %I64d ", timeGetTime(), ksAudioPosition.PlayOffset, ksAudioPosition.WriteOffset, m_llAbsWrite );
#endif

#ifdef USE_PINGPONG_BUFFERS
		if( ksAudioPosition.PlayOffset == ksAudioPosition.WriteOffset )
		{
			// We looped through all our buffers!!!
			//m_dwFrame = int(((ksAudioPosition.WriteOffset) / m_pPCMAudioPin->m_cbStreamData) % NUM_FRAMES);
			TRACE("Looped through all buffers\n");
		}

		while( m_pPCMAudioPin->IsSignaled( m_dwFrame ) )
		{
			// Write the next frame
			m_pPCMAudioPin->m_pbStreamData = m_apbData[m_dwFrame];
			m_pPCMAudioPin->WriteData( m_dwFrame, FALSE );

			// Switch to the next frame
			m_dwFrame++;
			m_dwFrame %= NUM_FRAMES;

			//TRACE("Switched to Frame %d at %I64d\n", m_dwFrame, ksAudioPosition.PlayOffset);
		}
#endif

		// Calculate the distance from the play cursor to the write cursor
        DWORD dwCursorDelta;
#ifdef USE_LOOPING_BUFFERS
		if( ksAudioPosition.PlayOffset > ksAudioPosition.WriteOffset )
		{
			ksAudioPosition.WriteOffset += m_pPCMAudioPin->m_cbStreamData;
		}
#endif //USE_LOOPING_BUFFERS
        dwCursorDelta = DWORD(ksAudioPosition.WriteOffset - ksAudioPosition.PlayOffset);

		// Check if the cursor delta is greater than anything we've seen
        if (dwCursorDelta > m_dwWriteFromMax)
        {
            //TRACE( "Warning: SynthSink - Play to Write cursor distance increased from %lu to %lu\n", m_dwWriteFromMax,dwCursorDelta);
            m_dwWriteFromMax = dwCursorDelta;
        }
        else
        {
			DWORD dwOldWriteFromMax = m_dwWriteFromMax;

			// Decrease m_dwWriteFromMax by 1% of the distance to dwCursorDelta
            m_dwWriteFromMax -= ((m_dwWriteFromMax - dwCursorDelta) / 100);
            m_dwWriteFromMax = SampleAlign(m_dwWriteFromMax);
            dwCursorDelta = m_dwWriteFromMax;

			if( dwCursorDelta != dwOldWriteFromMax )
			{
                //TRACE( "Warning: SynthSink - Play to Write cursor distance shrunk from %lu to %lu\n", dwOldWriteFromMax,m_dwWriteFromMax);
			}
        }

        if (m_llAbsWrite == 0)
        {
            // we just started
            m_llAbsPlay = ksAudioPosition.PlayOffset;
#ifdef USE_LOOPING_BUFFERS
			m_dwLastPlay = m_llAbsPlay;
#endif //USE_LOOPING_BUFFERS
            m_llAbsWrite = ksAudioPosition.WriteOffset;
        }

        ULONGLONG llAbsWriteFrom;

		HANDLE hCaptureFile = g_pconductor->m_pPhoneyDSound->GetFileHandle();

        DWORD dwPlayed;

		// Compute the length of the buffer that was played
#ifndef USE_LOOPING_BUFFERS
        dwPlayed = DWORD(ksAudioPosition.PlayOffset - m_llAbsPlay);

		m_llAbsPlay = ksAudioPosition.PlayOffset;
#else //USE_LOOPING_BUFFERS
		// Check if play position looped
		if( ksAudioPosition.PlayOffset < m_dwLastPlay )
		{
			// Yes - add in a copy of the buffer length
			dwPlayed = m_pPCMAudioPin->m_cbStreamData;
		}
		else
		{
			dwPlayed = 0;
		}
		dwPlayed += ksAudioPosition.PlayOffset - m_dwLastPlay;
		m_dwLastPlay = ksAudioPosition.PlayOffset;

		m_llAbsPlay += dwPlayed;
#endif //USE_LOOPING_BUFFERS

		 // Must start writing at least from the write cursor
        llAbsWriteFrom = m_llAbsPlay + DWORD(ksAudioPosition.WriteOffset - ksAudioPosition.PlayOffset);
#ifdef _FILE_DEBUG
		FTRACE("%I64d %d %I64d\n", m_llAbsPlay, dwPlayed, llAbsWriteFrom );
#endif

        // Are we ahead of the write head? (m_llAbsWrite is the position we wrote to the _last_ time)
        if (llAbsWriteFrom > m_llAbsWrite)
        {
            DWORD dwWriteMissed;

            // we are behind-- let's catch up
            dwWriteMissed = DWORD(llAbsWriteFrom - m_llAbsWrite);

            TRACE("Warning: SynthSink - Write underrun, missed %lu bytes\n", dwWriteMissed);
#ifdef _FILE_DEBUG
			FTRACE("Warning: SynthSink - Write underrun, missed %lu bytes\n", dwWriteMissed);
#endif

            m_llAbsWrite = llAbsWriteFrom;

			// Write out zeros for the time that we missed
			if( hCaptureFile != INVALID_HANDLE_VALUE )
			{
				static DWORD adwEmptyArray[16];
				while( dwWriteMissed > 0 )
				{
					DWORD dwWritten;
					WriteFile(hCaptureFile, adwEmptyArray, min( 16 * sizeof(DWORD ), dwWriteMissed ), &dwWritten, NULL);
					dwWriteMissed -= min( 16 * sizeof(DWORD ), dwWriteMissed );
				}
			}
        }

        // how much to write?
        ULONGLONG llAbsWriteTo;
        DWORD dwBytesToFill;

        llAbsWriteTo = llAbsWriteFrom + m_dwWriteTo;
		
#ifdef _FILE_DEBUG
		FTRACE("Write to: %I64d\n", llAbsWriteTo );
#endif

        if( m_pAudioSink
		&&	(llAbsWriteTo > m_llAbsWrite) )
        {
            dwBytesToFill = DWORD(llAbsWriteTo - m_llAbsWrite);
        }
        else
        {
            dwBytesToFill = 0;
        }

#ifdef USE_STREAMING_BUFFERS
		while( dwBytesToFill )
		{
			// Switch to the next frame
			m_dwFrame++;
			m_dwFrame %= NUM_FRAMES;
			m_pPCMAudioPin->m_pbStreamData = m_apbData[m_dwFrame];

			// Calculate the amount of data we're going to write into this frame
			DWORD dwBytesToFillNow = min( dwBytesToFill, m_pPCMAudioPin->m_cbStreamData );

			// Render the data into the buffer
            m_pAudioSink->Render((short*)m_pPCMAudioPin->m_pbStreamData, ByteToSample(dwBytesToFillNow), ByteToSample(m_llAbsWrite),ByteToSample(m_llAbsPlay));

            m_llAbsWrite += dwBytesToFillNow; 

			// Write out the samples we just rendered
			if( hCaptureFile != INVALID_HANDLE_VALUE )
			{
				DWORD dwWritten;
				WriteFile(hCaptureFile, m_pPCMAudioPin->m_pbStreamData, dwBytesToFillNow, &dwWritten, NULL);
			}

			// Write the frame
			DWORD dwTmpCbData = m_pPCMAudioPin->m_cbStreamData;
			m_pPCMAudioPin->m_cbStreamData = dwBytesToFillNow;
			m_pPCMAudioPin->WriteData( m_dwFrame, FALSE );
			m_pPCMAudioPin->m_cbStreamData = dwTmpCbData;

			dwBytesToFill -= dwBytesToFillNow;
		}
#endif
#ifdef USE_LOOPING_BUFFERS
        if (dwBytesToFill)
        {
            LPVOID lpStart, lpEnd;      // Buffer pointers, filled by Lock command.
            DWORD dwStart, dwEnd;       // For Lock.

			MyLoopingLock( m_llAbsWrite, dwBytesToFill, m_pPCMAudioPin->m_pbStreamData, m_pPCMAudioPin->m_cbStreamData, &lpStart, &dwStart, &lpEnd, &dwEnd );
            if (dwStart)
            {
//              TRACE("Clearing from %lx, %ld bytes\n",lpStart,dwStart);
				// No need to clear memory - Render() overwrites it

                m_pAudioSink->Render((short*)lpStart, ByteToSample(dwStart), ByteToSample(m_llAbsWrite),ByteToSample(m_llAbsPlay));

                m_llAbsWrite += dwStart; 

				// Write out the samples we just rendered
				if( hCaptureFile != INVALID_HANDLE_VALUE )
				{
					DWORD dwWritten;
					WriteFile(hCaptureFile, lpStart, dwStart, &dwWritten, NULL);
				}
            }
            if (dwEnd)
            {
//              TRACE("Clearing from %lx, %ld bytes\n",lpEnd,dwEnd);
				ZeroMemory( lpEnd, dwEnd );

                m_pAudioSink->Render((short*)lpEnd, ByteToSample(dwEnd), ByteToSample(m_llAbsWrite),ByteToSample(m_llAbsPlay));

                m_llAbsWrite += dwEnd;

				// Write out the samples we just rendered
				if( hCaptureFile != INVALID_HANDLE_VALUE )
				{
					DWORD dwWritten;
					WriteFile(hCaptureFile, lpEnd, dwEnd, &dwWritten, NULL);
				}
            }

            // write silence into unplayed buffer(s)
            //dwBytesToFill = m_pPCMAudioPin->m_cbStreamData - m_llAbsWrite + dwPlayCursor;

			//MyLock( m_llAbsWrite, dwBytesToFill, m_apbData, m_pPCMAudioPin->m_cbStreamData, &lpStart, &dwStart, &lpEnd, &dwEnd );
            //if (dwStart)
            //{
			//	ZeroMemory( lpStart, dwStart );
            //}
            //if (dwEnd)
            //{
			//	ZeroMemory( lpEnd, dwEnd );
            //}
        }
#endif //USE_PINGPONG_BUFFERS
#ifdef USE_PINGPONG_BUFFERS
        if (dwBytesToFill)
        {
            LPVOID lpStart, lpEnd;      // Buffer pointers, filled by Lock command.
            DWORD dwStart, dwEnd;       // For Lock.

			MyLock( m_llAbsWrite, dwBytesToFill, m_apbData, m_pPCMAudioPin->m_cbStreamData, &lpStart, &dwStart, &lpEnd, &dwEnd );
            if (dwStart)
            {
//              TRACE("Clearing from %lx, %ld bytes\n",lpStart,dwStart);
				// No need to clear memory - Render() overwrites it

                m_pAudioSink->Render((short*)lpStart, ByteToSample(dwStart), ByteToSample(m_llAbsWrite),ByteToSample(m_llAbsPlay));

                m_llAbsWrite += dwStart; 

				// Write out the samples we just rendered
				if( hCaptureFile != INVALID_HANDLE_VALUE )
				{
					DWORD dwWritten;
					WriteFile(hCaptureFile, lpStart, dwStart, &dwWritten, NULL);
				}
            }
            if (dwEnd)
            {
//              TRACE("Clearing from %lx, %ld bytes\n",lpEnd,dwEnd);
				ZeroMemory( lpEnd, dwEnd );

                m_pAudioSink->Render((short*)lpEnd, ByteToSample(dwEnd), ByteToSample(m_llAbsWrite),ByteToSample(m_llAbsPlay));

                m_llAbsWrite += dwEnd;

				// Write out the samples we just rendered
				if( hCaptureFile != INVALID_HANDLE_VALUE )
				{
					DWORD dwWritten;
					WriteFile(hCaptureFile, lpEnd, dwEnd, &dwWritten, NULL);
				}
            }

            // write silence into unplayed buffer(s)
            //dwBytesToFill = m_pPCMAudioPin->m_cbStreamData - m_llAbsWrite + dwPlayCursor;

			//MyLock( m_llAbsWrite, dwBytesToFill, m_apbData, m_pPCMAudioPin->m_cbStreamData, &lpStart, &dwStart, &lpEnd, &dwEnd );
            //if (dwStart)
            //{
			//	ZeroMemory( lpStart, dwStart );
            //}
            //if (dwEnd)
            //{
			//	ZeroMemory( lpEnd, dwEnd );
            //}
        }
#endif //USE_PINGPONG_BUFFERS
    }
    else
    {
        TRACE("Error: SynthSink - Failed to get DS buffer position.\n");
    }

    ::LeaveCriticalSection(&m_CriticalSection);
}


void CDSLink::SynthProc()
{
    for (;;)
    {
        if (m_fPleaseDie)
        {
            m_fPleaseDie = FALSE;
            break;
        }
        
        if (m_fActive)
        {
            ReadBuffer();
        }

		const DWORD dwLatency = g_dwLatency;

		m_dwWriteTo = SampleAlign((500 + (m_wfSynth.nAvgBytesPerSec * dwLatency)) / 1000);
		m_dwResolution = dwLatency/3;
        if (m_dwResolution < 2) m_dwResolution = 2; 
        if (m_dwResolution > 100) m_dwResolution = 100;
        WaitForSingleObject(m_hEvent, m_dwResolution);
    }
}


DWORD WINAPI CDSLink::SynthThread (LPVOID lpThreadParameter)
{
    CDSLink *pLink = (CDSLink *) lpThreadParameter;
    pLink->SynthProc();
    return 0;
}

void TestRender(short *pnBuffer,DWORD dwLength,DWORD dwPosition)
{
    DWORD dwIndex;
    //TRACE("Mixing %ld samples from %ld to %ld on buffer %lx\n",
    //    dwLength,dwPosition,dwLength+dwPosition,pnBuffer);
    for (dwIndex = 0;dwIndex < dwLength;dwIndex++)
    {
        pnBuffer[dwIndex] = (short) (16000 * sin( 3.1415926 * (dwPosition + dwIndex) / 100));
    }
}

HRESULT CDSLink::Connect()
{
/*    if (!m_pSynth)
    {
        TRACE( "Error: SynthSink - Activation failed, SynthSink not initialized\n");
        return DMUS_E_SYNTHNOTCONFIGURED;
    }*/


	/*
    if (!IsValidFormat(&m_wfSynth))
    {
        TRACE( "Error: SynthSink - Activation failed, format not initialized/valid\n");
        return DMUS_E_SYNTHNOTCONFIGURED;
    }
	*/
	/*
    if (!m_pIMasterClock)
    {
        TRACE( "Error: SynthSink - Activation failed, master clock not set\n");
        return DMUS_E_NO_MASTER_CLOCK;
    }*/

#ifdef _FILE_DEBUG
	if( !ofOutput.is_open() )
	{
		ofOutput.open( "\\output.txt" );
	}
#endif

	if( !m_pPCMAudioPin )
	{
		return DMUS_E_DSOUND_NOT_SET;
	}

    if (m_fActive)
    {
        TRACE( "Error: SynthSink - Activation failed, already active\n");
        return DMUS_E_SYNTHACTIVE;
    }

    HRESULT hr = E_FAIL;

    m_dwWriteTo = SampleAlign((500 + (m_wfSynth.nAvgBytesPerSec * g_dwLatency)) / 1000);

    m_llAbsPlay = 0;
	m_llAbsWrite = 0;

	// Create a DSBUFFER_LENGTH_MS buffer
	if( m_pPCMAudioPin->CreateDataBufferMS( DSBUFFER_LENGTH_MS, 0 ) )
	{
		m_pPCMAudioPin->m_cFramesUsed = NUM_FRAMES;
		m_apbData[0] = m_pPCMAudioPin->m_pbStreamData;
		for( int i=1; i < NUM_FRAMES; i++ )
		{
			m_apbData[i] = new BYTE[m_pPCMAudioPin->m_cbStreamData];
		}

		// fill initial buffer with silence
		for( i=0; i < NUM_FRAMES; i++ )
		{
			ZeroMemory(m_apbData[i], m_pPCMAudioPin->m_cbStreamData);
		}

		m_dwFrame = 0;
#ifdef USE_STREAMING_BUFFERS
		if( m_pPCMAudioPin->SetState( KSSTATE_RUN ) )
		{
			Activate();
			hr = S_OK;
		}
#endif //USE_STREAMING_BUFFERS
#ifdef USE_LOOPING_BUFFERS
		if( m_pPCMAudioPin->WriteData( 0, TRUE )
		&&	m_pPCMAudioPin->SetState( KSSTATE_RUN ) )
		{
			Activate();
			hr = S_OK;
		}
#endif //USE_LOOPING_BUFFERS
#ifdef USE_PINGPONG_BUFFERS
		bool fContinue = true;
		for( i=0; i < NUM_FRAMES; i++ )
		{
			m_pPCMAudioPin->m_pbStreamData = m_apbData[i];
			if( !m_pPCMAudioPin->WriteData( i, TRUE ) )
			{
				fContinue = false;
				break;
			}
		}

		if( fContinue
		&&	m_pPCMAudioPin->SetState( KSSTATE_RUN ) )
		{
			Activate();
			hr = S_OK;
		}
#endif //USE_PINGPONG_BUFFERS
	}

	if (FAILED(hr))
    {
        // Clean up
        //

		if( m_pPCMAudioPin )
		{
			m_pPCMAudioPin->SetState( KSSTATE_STOP );
			m_pPCMAudioPin->m_pbStreamData = NULL;
			m_pPCMAudioPin->ClosePin();
			m_pPCMAudioPin = NULL;
		}

        Clear();
    }

    if (SUCCEEDED(hr))
    {
	    ::LeaveCriticalSection(&m_CriticalSection);

        // wait until the pump is primed
        for (WORD wRetry = 0; wRetry < 10 && !m_llAbsWrite; wRetry++)
        {
            Sleep(10);
        }

	    ::EnterCriticalSection(&m_CriticalSection);

        if (m_llAbsWrite)
        {
            TRACE( "Warning: SynthSink - Pump is primed\n");
        }
        else
        {
            TRACE( "Error: SynthSink - Pump is NOT primed\n");
        }
    }

    return hr;
}

HRESULT CDSLink::Disconnect()
{
    // stop the buffer right away!
	if( m_pPCMAudioPin )
	{
		m_pPCMAudioPin->SetState( KSSTATE_STOP );

        // write silence to prevent blip bug if reactivated
		ZeroMemory( m_pPCMAudioPin->m_pbStreamData, m_pPCMAudioPin->m_cbStreamData );
		m_pPCMAudioPin->m_pbStreamData = m_apbData[0];
		ZeroMemory( m_pPCMAudioPin->m_pbStreamData, m_pPCMAudioPin->m_cbStreamData );
	}

    Deactivate();

	m_apbData[0] = NULL;
	for( int i=1; i < NUM_FRAMES; i++ )
	{
		delete []m_apbData[i];
		m_apbData[i] = NULL;
	}

    Clear();

    return S_OK;
}

void CDSLink::Clear()
{
    m_llAbsPlay = 0;        // Absolute point where play head is.
    m_llAbsWrite = 0;		// Absolute point we've written up to.
    m_dwWriteTo = 1000;     // Distance between write head and where we are writing.
}

CDSLink::CDSLink()
{
    ::InitializeCriticalSection(&m_CriticalSection);
	ZeroMemory( &m_wfSynth, sizeof(m_wfSynth) ); 
    m_pAudioSink = NULL;
    m_cRef = 0;
	m_pPCMAudioPin = NULL;
    m_dwWriteFromMax = 0;
    Clear();
    m_fActive = FALSE;
    m_fOpened = FALSE;
    m_fPleaseDie = FALSE;
    m_hThread = NULL;           // Handle for synth thread.
    m_dwThread = 0;             // ID for thread.
    m_hEvent = NULL;            // Used to signal thread.
    m_dwResolution = g_dwLatency/3;
	m_dwFrame = 0;
	for( int i=0; i < NUM_FRAMES; i++ )
	{
		m_apbData[i] = 0;
	}
	m_llAbsPlay = 0;
	m_llAbsWrite = 0;
#ifdef USE_LOOPING_BUFFERS
	m_dwLastPlay = 0;
#endif

    m_wfSynth.cbSize = 0;
    m_wfSynth.nAvgBytesPerSec = 22050 * 2 * 2;
    m_wfSynth.nBlockAlign = 4;
    m_wfSynth.nChannels = 2;
    m_wfSynth.nSamplesPerSec = 22050;
    m_wfSynth.wBitsPerSample = 16;
    m_wfSynth.wFormatTag = WAVE_FORMAT_PCM;
}

CDSLink::~CDSLink()
{
	CloseDown();
        
    ::DeleteCriticalSection(&m_CriticalSection);
}

void CDSLink::SetWaveFormat(WAVEFORMATEX *pWaveFormat)

{
    m_wfSynth.nSamplesPerSec = pWaveFormat->nSamplesPerSec;
    m_wfSynth.nAvgBytesPerSec = m_wfSynth.nSamplesPerSec * 2 * 2;
}

BOOL CDSLink::OpenUp(CAudioSink *pSink)
{
    if (m_fOpened)
    {
        TRACE( "Warning: SynthSink - Already opened\n");
        return TRUE;
    }

    ::EnterCriticalSection(&m_CriticalSection);

    m_fOpened = TRUE;
    m_pAudioSink = pSink;
    
	if( !g_fKslInitialized )
	{
#ifdef _DEBUG
		KslRegisterLog( &g_publicLogClass );
#endif
		KslInitKsLib();
		g_fKslInitialized = true;
	}
	CList<CKsFilter> lstFilters;
	GUID *pGuid = new GUID;
	*pGuid = KSCATEGORY_AUDIO_DEVICE;
	KslEnumFilters( &lstFilters, ePCMAudio, &pGuid, 1, TRUE, TRUE, FALSE );
	CNode<CKsFilter> *pNode = lstFilters.GetHead();
	while( pNode && !m_pPCMAudioPin )
	{
		CPCMAudioFilter *pPCMAudioFilter = (CPCMAudioFilter *)pNode->pData;

		if( g_szPhoneyDSoundFilterName[0] == 0
		||	strcmp( pPCMAudioFilter->m_szFilterName, g_szPhoneyDSoundFilterName ) == 0 )
		{
			CPCMAudioPin *pCPCMAudioPin = pPCMAudioFilter->FindViablePin( &(pPCMAudioFilter->m_listRenderSinkPins), &m_wfSynth );
			if( pCPCMAudioPin )
			{
				pCPCMAudioPin->SetFormat( &m_wfSynth );
#ifdef USE_LOOPING_BUFFERS
				if( pCPCMAudioPin->Instantiate( TRUE ) ) // TRUE - looped, FALSE - not looped
#else //USE_LOOPING_BUFFERS
				if( pCPCMAudioPin->Instantiate( FALSE ) ) // TRUE - looped, FALSE - not looped
#endif //USE_LOOPING_BUFFERS
				{
					CNode<CKsPin> *pPinNode = pPCMAudioFilter->m_listPins.Find( pCPCMAudioPin );
					pPCMAudioFilter->m_listPins.Remove( pPinNode );
					m_pPCMAudioPin = pCPCMAudioPin;
					break;
				}
			}
		}

		pNode = lstFilters.GetNext( pNode );
	}

	pNode = lstFilters.GetHead();
    while(pNode)
    {
        if (pNode->pData)
        {
            delete pNode->pData;
        }
        pNode = lstFilters.GetNext(pNode);
    }
	lstFilters.Empty();

	delete pGuid;

    Connect();

    ::LeaveCriticalSection(&m_CriticalSection);

   return TRUE;
}

void CDSLink::CloseDown()
{
    if (!m_fOpened)
    {
        TRACE("Warning: SynthSink - Process Detach, ports all deactivated\n");
    }
    else
    {
	   ::EnterCriticalSection(&m_CriticalSection);

        Disconnect();
		if( m_pPCMAudioPin )
		{
			m_pPCMAudioPin->SetState( KSSTATE_STOP );
			m_pPCMAudioPin->m_pbStreamData = NULL;
			m_pPCMAudioPin->ClosePin();
			m_pPCMAudioPin = NULL;
		}
        m_fOpened = FALSE;
		if( g_fKslInitialized )
		{
			KslCloseKsLib();
			g_fKslInitialized = false;
		}

	    ::LeaveCriticalSection(&m_CriticalSection);
    }
}

void CDSLink::Activate()
{
    if (!m_fActive)
    {
        m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        m_hThread = CreateThread(NULL, 0, CDSLink::SynthThread, this, 0, &m_dwThread);

        if (m_hThread)
        {
            if (!SetThreadPriority(m_hThread, THREAD_PRIORITY_TIME_CRITICAL))
            {
                TRACE( "Error: SynthSink - Activate couldn't set thread priority\n");
            }
        }
        else
        {
            TRACE( "Error: SynthSink - Activate couldn't create thread\n");
        }
        
        m_fActive = TRUE;
    }
}

void CDSLink::Deactivate()
{
    if (m_fActive)
    {
        m_fActive = FALSE;

        if (m_hThread && m_hEvent)
        {
            m_fPleaseDie = TRUE;

			// Need to leave the critical section to that the sink thread can run
			::LeaveCriticalSection( &m_CriticalSection );

            SetEvent(m_hEvent);
            if (WaitForSingleObject(m_hThread, 10000) == WAIT_TIMEOUT)
            {
                TRACE( "Error: SynthSink - Deactivate, thread did not exit\n");
            }

			// Now, re-enter the critical section
			::EnterCriticalSection( &m_CriticalSection );
        }
        if (m_hEvent)
        {
            CloseHandle(m_hEvent);
            m_hEvent = NULL;
        }
        if(m_hThread)
        {
            CloseHandle(m_hThread);
            m_hThread = NULL;
        }
    }
}

ULONGLONG CDSLink::GetAbsPlaySample(void)
{
	KSAUDIO_POSITION ksAudioPosition;
	if( m_pPCMAudioPin
	&&	m_pPCMAudioPin->GetPosition( &ksAudioPosition ) )
	{
#ifdef USE_LOOPING_BUFFERS
		if( ksAudioPosition.PlayOffset < m_dwLastPlay )
		{
			ksAudioPosition.PlayOffset += m_pPCMAudioPin->m_cbStreamData;
		}
		ksAudioPosition.PlayOffset += m_llAbsPlay - m_dwLastPlay;
#else //USE_LOOPING_BUFFERS
		/*
		if( ksAudioPosition.PlayOffset != m_llAbsPlay )
		{
			TRACE("PlayOffset; %I64d\n", ksAudioPosition.PlayOffset - m_llAbsPlay);
		}
		*/
#endif //USE_LOOPING_BUFFERS
		return ByteToSample(ksAudioPosition.PlayOffset);
	}
	else
	{
		return ByteToSample(m_llAbsPlay);
	}
}