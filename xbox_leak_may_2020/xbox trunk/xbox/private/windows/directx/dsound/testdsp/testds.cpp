/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       testds.cpp
 *  Content:    DirectSound test functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/08/00    dereks  Created.
 *  04/2001   georgioc  cut and paste/delete from testds, for testing dsp stuff
 *
 ****************************************************************************/

#include "dsoundi.h"
#include "c:\xboxbins\dsp\reverb_2ch.h"
#include "C:\xbox\private\windows\directx\dsound\tools\internal\DSP\rvbparam\rvbformat.h"

// #define FIX_BUFFER_SIZE 0x1004
#define STALL_ON_START
#define BREAK_ON_START
// #define ALLOC_ALIGNED
#define ALLOC_CONTIGUOUS

#ifdef STALL_ON_START

BOOL fTestMe;

#endif // STALL_ON_START

BOOL fBreakTest;

static const float pi = 3.1415f;


#define DPF_TEST DPF_ABSOLUTE

__inline HRESULT
ValidateResult
(
    HRESULT                 hr
)
{
    ASSERT(SUCCEEDED(hr));

    return hr;
}


__inline HRESULT
ValidateBool
(
    BOOL                    fSuccess
)
{
    ASSERT(fSuccess);

    return fSuccess ? S_OK : E_FAIL;
}


__inline HRESULT
ValidatePtr
(
    LPVOID                  pv
)
{
    ASSERT(pv);

    return pv ? S_OK : E_OUTOFMEMORY;
}


LPVOID
__Malloc
(
    DWORD                   dwSize
)
{

#ifdef ALLOC_CONTIGUOUS

#ifdef ALLOC_ALIGNED

    return MmAllocateContiguousMemoryEx(dwSize, 0, 0xFFFFFFFF, 4, PAGE_READWRITE);

#else // ALLOC_ALIGNED

    return MmAllocateContiguousMemoryEx(dwSize, 0, 0xFFFFFFFF, PAGE_SIZE, PAGE_READWRITE);

#endif // ALLOC_ALIGNED

#else // ALLOC_CONTIGUOUS

    DWORD                   dwAllocSize;
    DWORD                   dwOffset;
    LPVOID                  pvBuffer;
    DWORD                   dwOldProtect;
    BOOL                    fProtect;

    dwAllocSize = dwSize + PAGE_SIZE + PAGE_SIZE - 1;

#ifndef ALLOC_ALIGNED

    dwAllocSize += sizeof(DWORD);

#endif // ALLOC_ALIGNED

    dwAllocSize /= PAGE_SIZE;
    dwAllocSize *= PAGE_SIZE;

    if(pvBuffer = VirtualAlloc(NULL, dwAllocSize, MEM_COMMIT, PAGE_READWRITE))
    {
        fProtect = VirtualProtect((LPBYTE)pvBuffer + dwAllocSize - PAGE_SIZE, PAGE_SIZE, PAGE_NOACCESS, &dwOldProtect);
        ASSERT(fProtect);

#ifndef ALLOC_ALIGNED

        dwOffset = dwAllocSize - dwSize - PAGE_SIZE;

        pvBuffer = (LPBYTE)pvBuffer + dwOffset;

        *((LPDWORD)pvBuffer - 1) = dwOffset;

#endif // ALLOC_ALIGNED

    }

    return pvBuffer;

#endif // ALLOC_CONTIGUOUS

}

#define MALLOC(a) \
    __Malloc(a)


void
__Free
(
    LPVOID *                ppvBuffer
)
{

    LPVOID                  pvBuffer    = *ppvBuffer;
    DWORD                   dwOffset;

    *ppvBuffer = NULL;

    if(pvBuffer)
    {

#ifdef ALLOC_CONTIGUOUS

        MmFreeContiguousMemory(pvBuffer);

#else // ALLOC_CONTIGUOUS

#ifdef ALLOC_ALIGNED

        dwOffset = 0;

#else // ALLOC_ALIGNED

        dwOffset = *((LPDWORD)pvBuffer - 1);

#endif // ALLOC_ALIGNED
    
        VirtualFree((LPBYTE)pvBuffer - dwOffset, 0, MEM_RELEASE);

#endif // ALLOC_CONTIGUOUS

    }
}

#define FREE(p) \
    __Free((LPVOID *)&(p))


HRESULT TestMe(void);


/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      Application entry point.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void __cdecl
main
(
    void
)
{
    HRESULT                 hr;
    
    XInitDevices(0, NULL);

    while(TRUE)
    {

#ifdef STALL_ON_START

        fTestMe = FALSE;

        DPF_TEST("");
        DPF_TEST("******************************************************************************");
        DPF_TEST("");
        DPF_TEST("Enter the following into the debugger in order to start testing:");
        DPF_TEST("ed testds!fTestMe 1;g");
        DPF_TEST("");
        DPF_TEST("******************************************************************************");
        DPF_TEST("");

        while(!fTestMe);

#endif // STALL_ON_START

#ifdef BREAK_ON_START

        BREAK();

#endif // BREAK_ON_START

        fBreakTest = FALSE;

        hr = TestMe();

        if(SUCCEEDED(hr))
        {
            DPF_TEST("Test successful");
        }
        else
        {
            DPF_TEST("Test failed");
        }
    }
}


/****************************************************************************
 *
 *  GenerateTone
 *
 *  Description:
 *      Generates a 16-bit sine-wave tone.
 *
 *  Arguments:
 *      LPVOID [out]: audio data buffer.
 *      DWORD [in]: audio data buffer size, in bytes.
 *      DWORD [in]: playback frequency.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
void
GenerateTone
(
    LPVOID                  pvBuffer,
    DWORD                   dwBufferSize,
    DWORD                   dwFrequency
)
{
    static const double     freq            = 750.0;
    static const double     datascale       = 32767.0;
    const double            iscale          = 2.0 * pi * (double)(dwFrequency - 1);
    short * const           psBuffer        = (short *)pvBuffer;
    const DWORD             dwSampleCount   = dwBufferSize / 2;
    DWORD                   i;

    for(i = 0; i < dwSampleCount; i++)
    {
        psBuffer[i] = (short)(sin(freq * ((double)i * iscale)) * datascale);
    }
}


/****************************************************************************
 *
 *  GenerateNoise
 *
 *  Description:
 *      Generates white noise.
 *
 *  Arguments:
 *      WORD [in]: channel count.
 *      LPVOID [out]: audio data buffer.
 *      DWORD [in]: audio data buffer size, in bytes.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
void
GenerateNoise
(
    DWORD                   dwChannels,
    LPVOID                  pvBuffer,
    DWORD                   dwBufferSize
)
{
    short *                 psBuffer    = (short *)pvBuffer;
    DWORD                   dwSamples   = dwBufferSize / 2;

    while(dwSamples--)
    {
        *psBuffer++ = (short)((rand() % 0xFFFF) - 0x7FFF);
    }
}


/****************************************************************************
 *
 *  ReadXMO
 *
 *  Description:
 *      Reads data from an XMO.
 *
 *  Arguments:
 *      XMediaObject * [in]: XMO.
 *      LPVOID [out]: data buffer.
 *      DWORD [in]: data buffer size.
 *      LPDWORD [out]: amount read.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
ReadXMO
(
    XMediaObject *          pMediaObject,
    LPVOID                  pvBuffer,
    DWORD                   dwBufferSize,
    LPDWORD                 pdwRead
)
{
    HANDLE                  hCompletionEvent    = NULL;
    XMEDIAINFO              xmi;
    XMEDIAPACKET            xmp;
    HRESULT                 hr;

    //
    // Check to see if the XMO is ASYNC.  It's massive overhead to do this
    // every time we want to read, but hey, it's test code.
    //

    ZeroMemory(&xmi, sizeof(xmi));

    hr = ValidateResult(pMediaObject->GetInfo(&xmi));

    //
    // If we are ASYNC, create an event so we can simulate SYNC.
    //
    
    if(SUCCEEDED(hr) && (xmi.dwFlags & XMO_STREAMF_OUTPUT_ASYNC))
    {
        hCompletionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    //
    // Read from the XMO
    //

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&xmp, sizeof(xmp));

        xmp.pvBuffer = pvBuffer;
        xmp.dwMaxSize = dwBufferSize,
        xmp.pdwCompletedSize = pdwRead;
        xmp.hCompletionEvent = hCompletionEvent;

        hr = ValidateResult(pMediaObject->Process(NULL, &xmp));
    }

    //
    // Wait for processing to complete
    //

    if(SUCCEEDED(hr) && hCompletionEvent)
    {
        WaitForSingleObject(hCompletionEvent, INFINITE);
    }

    //
    // Clean up
    //

    CLOSE_HANDLE(hCompletionEvent);

    return hr;
}

/****************************************************************************
 *
 *  LoadWaveFile
 *
 *  Description:
 *      Loads a wave file into an XMO object.
 *
 *  Arguments:
 *      LPCSTR [in]: file name.
 *      LPCWAVEFORMATEX [out]: wave file format.
 *      XMediaObject ** [out]: wave file XMO.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
LoadWaveFile
(
    LPCSTR                  pszFileName,
    LPCWAVEFORMATEX *       ppwfxFormat,
    XFileMediaObject **     ppMediaObject
)
{
    DPF_TEST("Loading %s", pszFileName);

    return ValidateResult(XWaveFileCreateMediaObject(pszFileName, ppwfxFormat, ppMediaObject));
}


/****************************************************************************
 *
 *  LoadStdWaveFile
 *
 *  Description:
 *      Loads a "standard" wave file into a CWaveFile object.
 *
 *  Arguments:
 *      CWaveFile * [in]: wave file.
 *      DWORD [in]: sample rate, in Hz
 *      DWORD [in]: bit resolution.
 *      DWORD [in]: channel count.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
LoadStdWaveFile
(
    WORD                    wFormatTag,
    DWORD                   dwSampleRate,
    DWORD                   dwBitResolution,
    DWORD                   dwChannelCount,
    LPCWAVEFORMATEX *       ppwfxFormat,
    XFileMediaObject **     ppMediaObject
)
{
    static const LPCSTR     pszParent               = "d:\\media\\audio\\";
    static const LPCSTR     pszPcm                  = "pcm\\";
    static const LPCSTR     pszXboxAdpcm            = "xbadpcm\\";
    static const LPCSTR     pszFileTemplate         = "std\\%uk%ub%ls.wav";
    static const LPCSTR     pszMono                 = "m";
    static const LPCSTR     pszStereo               = "s";
    CHAR                    szFileName[MAX_PATH];

    strcpy(szFileName, pszParent);

    switch(wFormatTag)
    {
        case WAVE_FORMAT_PCM:
            strcat(szFileName, pszPcm);
            break;

        case WAVE_FORMAT_XBOX_ADPCM:
            strcat(szFileName, pszXboxAdpcm);
            break;

        default:
            DPF_TEST("Unsupported format");
            return E_FAIL;
    }

    sprintf(szFileName + strlen(szFileName), pszFileTemplate, dwSampleRate / 1000, dwBitResolution, (1 == dwChannelCount) ? pszMono : pszStereo);

    return LoadWaveFile(szFileName, ppwfxFormat, ppMediaObject);
}


/****************************************************************************
 *
 *  CreateBufferFromFile
 *
 *  Description:
 *      Creates a sound buffer based on a given wave file.
 *
 *  Arguments:
 *      DWORD [in]: buffer creation flags.
 *      LPCWAVEFORMATEX [in]: buffer format.
 *      XFileMediaObject * [in]: file XMO.
 *      LPDIRECTSOUNDBUFFER * [out]: buffer object.
 *      LPDWORD [out]: buffer length, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
CreateBufferFromFile
(
    DWORD                   dwFlags,
    LPCWAVEFORMATEX         pwfxFormat,
    XFileMediaObject *      pMediaObject,
    LPDIRECTSOUNDBUFFER *   ppBuffer,
    LPDWORD                 pdwDuration     = NULL
)
{
    LPDIRECTSOUNDBUFFER     pBuffer         = NULL;
    HRESULT                 hr              = DS_OK;
    DSBUFFERDESC            dsbd;
    LPVOID                  pvLock;
    DWORD                   dwLockSize = 0;

    ZeroMemory(&dsbd, sizeof(dsbd));

    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = dwFlags;
    dsbd.lpwfxFormat = (LPWAVEFORMATEX)pwfxFormat;

#ifdef FIX_BUFFER_SIZE

    dsbd.dwBufferBytes = FIX_BUFFER_SIZE;

#else // FIX_BUFFER_SIZE

    hr = ValidateResult(pMediaObject->GetLength(&dsbd.dwBufferBytes));

#endif // FIX_BUFFER_SIZE

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(DirectSoundCreateBuffer(&dsbd, &pBuffer));
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(pBuffer->Lock(0, 0, &pvLock, &dwLockSize, NULL, NULL, DSBLOCK_ENTIREBUFFER));
    }

    if(SUCCEEDED(hr))
    {
        hr = ReadXMO(pMediaObject, pvLock, dwLockSize, NULL);
    }

    if(SUCCEEDED(hr))
    {
        *ppBuffer = pBuffer;

        if(pdwDuration)
        {
            *pdwDuration = dsbd.dwBufferBytes;
        }
    }
    else
    {
        RELEASE(pBuffer);
    }

    return hr;
}


/****************************************************************************
 *
 *  CreateBufferFromFile
 *
 *  Description:
 *      Creates a sound buffer based on a given wave file.
 *
 *  Arguments:
 *      LPCSTR [in]: file path.
 *      DWORD [in]: buffer creation flags.
 *      REFGUID [in]: 3D algorithm.
 *      LPDIRECTSOUNDBUFFER * [out]: buffer object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
CreateBufferFromFile
(
    LPCSTR                  pszFile,
    DWORD                   dwFlags,
    LPDIRECTSOUNDBUFFER *   ppBuffer,
    LPDWORD                 pdwDuration     = NULL
)
{
    XFileMediaObject *      pMediaObject    = NULL;
    LPCWAVEFORMATEX         pwfxFormat      = NULL;
    HRESULT                 hr;

    hr = LoadWaveFile(pszFile, &pwfxFormat, &pMediaObject);

    if(SUCCEEDED(hr))
    {
        hr = CreateBufferFromFile(dwFlags, pwfxFormat, pMediaObject, ppBuffer, pdwDuration);
    }

    RELEASE(pMediaObject);

    return hr;
}


/****************************************************************************
 *
 *  CreateStdSoundBuffer
 *
 *  Description:
 *      Creates a "standard" sound buffer using one of the "standard" wave
 *      files.
 *
 *  Arguments:
 *      DWORD [in]: buffer creation flags.
 *      DWORD [in]: format tag.
 *      DWORD [in]: sample rate, in Hz
 *      DWORD [in]: bit resolution.
 *      DWORD [in]: channel count.
 *      LPDIRECTSOUNDBUFFER * [out]: buffer object.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

HRESULT
CreateStdSoundBuffer
(
    DWORD                   dwFlags,
    WORD                    wFormatTag,
    DWORD                   dwSampleRate,
    DWORD                   dwBitResolution,
    DWORD                   dwChannelCount,
    LPDIRECTSOUNDBUFFER *   ppBuffer,
    LPCWAVEFORMATEX *       ppwfxFormat     = NULL,
    LPDWORD                 pdwDuration     = NULL
)
{
    XFileMediaObject *      pMediaObject    = NULL;
    LPCWAVEFORMATEX         pwfxFormat      = NULL;
    HRESULT                 hr;

    hr = LoadStdWaveFile(wFormatTag, dwSampleRate, dwBitResolution, dwChannelCount, &pwfxFormat, &pMediaObject);

    if(SUCCEEDED(hr))
    {
        hr = CreateBufferFromFile(dwFlags, pwfxFormat, pMediaObject, ppBuffer, pdwDuration);
    }

    if(SUCCEEDED(hr) && ppwfxFormat)
    {
        *ppwfxFormat = pwfxFormat;
    }

    RELEASE(pMediaObject);

    return hr;
}

#define IDLE_BUFFER_SIZE    100
#define IDLE_BUFFER_BASE    (0x36000+0x800*sizeof(DWORD)-IDLE_BUFFER_SIZE*sizeof(DWORD)-sizeof(DWORD))
#define DSP_COUNTER_OVERHEAD 5

VOID
DisplayGPDspStats()
{
    int i;
    DWORD dwMin=-1;
    DWORD dwMax=0;
    DWORD dwAvg=0;
    DWORD dwValue;

    PDWORD  pDspYramIdleBuffer = (PDWORD)(0xFE800000+IDLE_BUFFER_BASE);  // BASE mcpx BAR + yram offset

    //
    // get some DSP usage statistics straight form Y-ram and print them out
    //

    dwAvg = 0;
    for (i=0;i<IDLE_BUFFER_SIZE/sizeof(DWORD);i++) {

        dwValue = *(pDspYramIdleBuffer+i)*DSP_COUNTER_OVERHEAD;
        dwAvg += dwValue;
        if (dwValue>dwMax) {
            dwMax=dwValue;
        }

        if (dwValue<dwMin) {
            dwMin=dwValue;
        }
    }

    dwAvg = dwAvg/(IDLE_BUFFER_SIZE/sizeof(DWORD));

    DPF_TEST("GPIdle Cycles: Avg = %d, min = %d, max = %d",dwAvg,dwMin,dwMax);

}

VOID
WaitForAnyButton()
{
    BOOL fButtonPressed = FALSE;
    static DWORD dwPads = 0;
    DWORD dwInsertions, dwRemovals;
    static HANDLE hPads[XGetPortCount()] = { 0 };
    int i;

    dwPads = XGetDevices(XDEVICE_TYPE_GAMEPAD);

    while (!fButtonPressed)
    {

        if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
        {
            DPF_TEST("GetDevicesReturned %x %x",dwInsertions, dwRemovals);
            dwPads |= dwInsertions;
            dwPads &= ~dwRemovals;

            for (i = 0; i < XGetPortCount(); i++)
            {
                if ((1 << i) & dwRemovals)
                {
                    if (NULL != hPads[i])
                    {
                        XInputClose(hPads[i]);
                        hPads[i] = NULL;
                    }
                }

                if ((1 << i) & dwInsertions)
                {
                    if (NULL == hPads[i])
                    {
                        hPads[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + i, XDEVICE_NO_SLOT, NULL);;
                    }
                }
            }
        }

        for (i = 0; i < XGetPortCount(); i++)
        {
            if ((1 << i) & dwPads)
            {
                if (NULL != hPads[i])
                {
                    XINPUT_STATE State;

                    if (ERROR_SUCCESS == XInputGetState(hPads[i], &State))
                    {
                        if ((State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 28) &&
                            (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 28) &&
                            (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB))
                        {
                            DPF_TEST("DPAD pressed");
                            DisplayGPDspStats();           
                        }
                        else if ((0 != ((XINPUT_GAMEPAD_START | XINPUT_GAMEPAD_BACK) & State.Gamepad.wButtons)) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > 28) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > 28) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] > 28) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] > 28) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 28) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > 28))
                        {
                            fButtonPressed = TRUE;
                            DPF_TEST("Button pressed");
                            DisplayGPDspStats();           

                        }
                    }
                }
            }

        }
    }
}

///
/// Set up necessary types
/// 

typedef short* LPSHORT;

///
/// Setup the frequency, and define PI
///

static const double FREQ = 2.55;
static const double PI = 3.1415926535;

///
/// Scaling factors
///
/// ISCALE is used to convert an interger [0, 44099] to a double [0, 2PI]
/// DATASCALE is used to conver a double [-1.0, 1.0] to an integer [-32767, 32767]

static const double ISCALE = 2.0 * PI / 48000.0;
static const double DATASCALE = 32767.0 / 1.0;

///
/// the main sine player code
///

    
HRESULT PlaySineWave(double Frequency, DWORD dwMixBins,LPDIRECTSOUNDBUFFER     *pBuffer )
{

	///
	/// create a dsound buffer and a data buffer
	///

    HRESULT hr = S_OK;
	LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
	LPSHORT pData = NULL;

	///
	/// place to hold the number of bytes allocated by dsound
	///

	DWORD dwBytes = 0;

	///
	/// allocate space for calculate argument to sin, return value from sine, and actual integer value
	///

	double dArg = 0.0;
	double dSinVal = 0.0;
	SHORT sVal = 0;
   
	///
	/// allocate the dsound buffer description and the wave format
	///

	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;

	///
	/// It's always safe to zero out stack allocated structs
	///

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( DSBUFFERDESC ) );

	///
	/// setup the waveformat description to be PCM, 44100hz, 16bit, mono
    ///

	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 48000;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = 2;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	///
	/// setup the dsound buffer description with the waveformatex
	///

	dsbd.dwBufferBytes = wfx.nAvgBytesPerSec;
	dsbd.dwFlags = 0;
	dsbd.dwSize = sizeof( DSBUFFERDESC );
	dsbd.lpwfxFormat = &wfx;
    dsbd.dwMixBinMask = dwMixBins;

	/// 
	/// Create the dsound buffer
	///

	hr = DirectSoundCreateBuffer( &dsbd, &pDSBuffer );

	///
	/// continue forever (or until an error occurs)
	///

	if ( SUCCEEDED( hr ) )
	{

		///
		/// get a buffer from dsound using lock
		///

		hr = pDSBuffer->Lock( 0, dsbd.dwBufferBytes, (LPVOID*) &pData, &dwBytes, NULL, NULL, 0 );

		///
		/// go through the buffer 2 bytes (1 short) at a time
		///

		for( DWORD i = 0; SUCCEEDED( hr ) && i < dsbd.dwBufferBytes / 2; i++ )
		{	
			///
			/// calculate the sin value
			///

			dArg = (double) i * ISCALE;
			dSinVal = sin( Frequency * dArg );
			sVal = (SHORT) ( dSinVal * DATASCALE );

			///
			/// copy the value into the dsound buffer
			///

			pData[i] = sVal;
		}

    }
    
	///
	/// start the dsound buffer playing in a loop mode
	///

	if ( SUCCEEDED( hr ) )
	{
		hr = pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );
        *pBuffer = pDSBuffer;
	}



    //
    // leave with the buffer still playing..
    //

    return hr;
}

HRESULT
PlayLoopingBuffer
(
    BOOL                    fUseCliffWaves,
    WORD                    wFormatTag,
    DWORD                   dwSampleRate,
    WORD                    wBitsPerSample,
    WORD                    wChannelCount,
    LPDIRECTSOUNDBUFFER     *pBuffer
)
{

    HRESULT hr = S_OK;
    LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
    DWORD dwDuration;

    if (fUseCliffWaves) {

        hr = CreateBufferFromFile("d:\\media\\audio\\pcm\\CleanGuitarArpeggios.wav",
                                  0,
                                  &pDSBuffer,
                                  &dwDuration);

    } else {

        hr = CreateStdSoundBuffer(0, wFormatTag, dwSampleRate, wBitsPerSample, wChannelCount, &pDSBuffer);

    }

    if (SUCCEEDED(hr)) {

        hr = pDSBuffer->SetLoopRegion( 0, 0);

    }

    if (SUCCEEDED(hr)) {

        hr = pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    }

    *pBuffer = pDSBuffer;

    return hr;
}

HRESULT
DownloadScratch(PCHAR pszScratchFile)
{

    LPDIRECTSOUND           pDirectSound;
    HANDLE hFile;
    DWORD dwSize;
    PVOID pBuffer;
    DWORD err;
    HRESULT hr=S_OK;

    UCHAR data[4] = {0,1,2,3};
    LPDSEFFECTIMAGEDESC pDesc;

    //
    // open scratch image file generated by xps2 tool
    //


    hFile = CreateFile(
        pszScratchFile,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE) {

        err = GetLastError();
        fprintf(stderr,"\n Failed to open the dsp image file.Error 0x%x\n", err);
        hr = HRESULT_FROM_WIN32(err);

    }

    if (SUCCEEDED(hr)) {

        dwSize = SetFilePointer(hFile, 0, NULL, FILE_END);              
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    
        pBuffer = new BYTE[dwSize];
        ASSERT(pBuffer);

        DWORD dwBytesRead;
        BOOL bResult = ReadFile(hFile,
            pBuffer,
            dwSize,
            &dwBytesRead,
            0);
        
        if (!bResult) {
            
            err = GetLastError();
            fprintf(stderr,"\n Failed to open the dsp image file.Error 0x%x\n", err);
            hr = HRESULT_FROM_WIN32(err);
    
        }

    }

    if (SUCCEEDED(hr)) {

        //
        // call dsound api to download the image..
        //

        hr = ValidateResult(DirectSoundCreate(NULL, &pDirectSound, NULL));

        if (SUCCEEDED(hr)) {

            hr = ValidateResult(pDirectSound->DownloadEffectsImage(pBuffer,
                                               dwSize,
                                               NULL,
                                               &pDesc));
        }

    }

    if (hFile) {
        CloseHandle(hFile);
    }
    
    delete [] pBuffer;


    return hr;
}

HRESULT LoadReverbParameters()
{

    LPDIRECTSOUND           pDirectSound;
    HANDLE hFile;
    DWORD dwSize;
    PVOID pBuffer=NULL;
    DWORD err;
    HRESULT hr=S_OK;

    UCHAR data[4] = {0,1,2,3};
    LPDSEFFECTIMAGEDESC pDesc;
    PREVERB_EFFECT_PARAMS pRvbData;

    hFile = CreateFile(
        "d:\\media\\hangar.bin",
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE) {

        err = GetLastError();
        hr = HRESULT_FROM_WIN32(err);
        _asm int 3;

    }

    if (SUCCEEDED(hr)) {

        dwSize = SetFilePointer(hFile, 0, NULL, FILE_END);              
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    
        pBuffer = new BYTE[dwSize];

        DWORD dwBytesRead;
        BOOL bResult = ReadFile(hFile,
            pBuffer,
            dwSize,
            &dwBytesRead,
            0);
        
        if (!bResult) {
            
            err = GetLastError();
            _asm int 3;
            hr = HRESULT_FROM_WIN32(err);
    
        }

        pRvbData = (PREVERB_EFFECT_PARAMS) pBuffer;
    }

    if (SUCCEEDED(hr)) {

        hr = ValidateResult(DirectSoundCreate(NULL, &pDirectSound, NULL));

    }

    if (SUCCEEDED(hr)) {

        //
        // call dsound api to download the new reverb parameters..
        // defer commiting so they can only get DMAed at once
        // We only update the non-scratch related values
        //

        hr = ValidateResult(pDirectSound->SetEffectData(
                                    REVERB_CHAIN_FX0_REVERB, 
                                    sizeof(REVERB_CHAIN_FX0_REVERB_STATE)+FIELD_OFFSET(REVERB_EFFECT_PARAMS,dwReflectionsInputDelay0),
                                    &pRvbData->dwReflectionsInputDelay0,
                                    dwSize-FIELD_OFFSET(REVERB_EFFECT_PARAMS,dwReflectionsInputDelay0),
                                    DSFX_DEFERRED));
    }

    //
    // commit FX changes
    //

    if (SUCCEEDED(hr)) {
        hr = pDirectSound->CommitEffectData();
    }

    if (hFile) {
        CloseHandle(hFile);
    }
    
    if (pBuffer) {
        delete [] pBuffer;
    }    

    return hr;

}

HRESULT
TestSurround(LPDIRECTSOUNDBUFFER pBuffer)
{

    typedef struct
    {
        LPCSTR                  pszDescription;
        DWORD                   dwMixBins;
    } TESTMIXBINS;

    static const TESTMIXBINS    aMixBins[] =
    {
        { "All speakers",   DSMIXBIN_SPEAKER_MASK },
        { "Front left",     DSMIXBIN_FRONT_LEFT },
        { "Front right",    DSMIXBIN_FRONT_RIGHT },
        { "Back left",      DSMIXBIN_BACK_LEFT },
        { "Back right",     DSMIXBIN_BACK_RIGHT },
        { "Center",         DSMIXBIN_FRONT_CENTER },
        { "LFE",            DSMIXBIN_LOW_FREQUENCY },
        { "All speakers",   DSMIXBIN_SPEAKER_MASK },

    };
    
    HRESULT                     hr = S_OK;
    DWORD                       i;

    for(i = 0; (i < NUMELMS(aMixBins)) && SUCCEEDED(hr); i++)
    {
        DPF_TEST(aMixBins[i].pszDescription);

        hr = ValidateResult(pBuffer->SetMixBins(aMixBins[i].dwMixBins));

        Sleep(5000);

    }

    return hr;
}

/****************************************************************************
 *
 *  TestMe
 *
 *  Description:
 *      Main test function.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

HRESULT
TestMe
(
    void
)
{
    HRESULT                 hr  = DS_OK;
    LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
    LPDIRECTSOUNDBUFFER pOscillatorBuffer0 = NULL;
    LPDIRECTSOUNDBUFFER pOscillatorBuffer1 = NULL;
    LPDIRECTSOUNDBUFFER pOscillatorBuffer2 = NULL;
    DWORD dwMask = 0;
    

    hr = PlaySineWave(3,DSMIXBIN_FXSEND_0,&pOscillatorBuffer0);
    if(SUCCEEDED(hr)) hr = PlaySineWave(1,DSMIXBIN_FXSEND_1,&pOscillatorBuffer1);
    if(SUCCEEDED(hr)) hr = PlaySineWave(1005,DSMIXBIN_FXSEND_2,&pOscillatorBuffer2);

    //
    // create audio buffer
    //

    if(SUCCEEDED(hr)) hr = PlayLoopingBuffer(TRUE, WAVE_FORMAT_PCM, 44100, 16, 1, &pDSBuffer);

    //
    // set mixbins
    //

    //dwMask = DSMIXBIN_XTLK_FRONT_LEFT | DSMIXBIN_XTLK_FRONT_RIGHT | DSMIXBIN_XTLK_BACK_LEFT | DSMIXBIN_XTLK_BACK_RIGHT;
    //hr = ValidateResult(pDSBuffer->SetMixBins(dwMask));

    //
    // test surround config
    //

    if(SUCCEEDED(hr)) hr = TestSurround(pDSBuffer);
    

    while(TRUE) {

        WaitForAnyButton();
        if(SUCCEEDED(hr)) hr = DownloadScratch("d:\\media\\audio\\scratchimg.bin");
        WaitForAnyButton();   
        if(SUCCEEDED(hr)) hr = LoadReverbParameters();

    }

    return hr;
}


