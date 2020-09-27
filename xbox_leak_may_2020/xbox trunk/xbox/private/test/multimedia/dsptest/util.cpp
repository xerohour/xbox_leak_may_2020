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

#include "xtl.h"
#include "stdio.h"


static const float pi = 3.1415f;
#define PAGE_SIZE 4096

__inline HRESULT
ValidateResult
(
    HRESULT                 hr
)
{
    if(FAILED(hr)) {
        _asm int 3;
    }

    return hr;
}


__inline HRESULT
ValidateBool
(
    BOOL                    fSuccess
)
{
    return fSuccess ? S_OK : E_FAIL;
}


__inline HRESULT
ValidatePtr
(
    LPVOID                  pv
)
{
    return pv ? S_OK : E_OUTOFMEMORY;
}


LPVOID
__Malloc
(
    DWORD                   dwSize
)
{

    DWORD                   dwAllocSize;
    DWORD                   dwOffset;
    LPVOID                  pvBuffer;
    DWORD                   dwOldProtect;
    BOOL                    fProtect;

    dwAllocSize = dwSize + PAGE_SIZE + PAGE_SIZE - 1;

    dwAllocSize += sizeof(DWORD);

    dwAllocSize /= PAGE_SIZE;
    dwAllocSize *= PAGE_SIZE;

    if(pvBuffer = VirtualAlloc(NULL, dwAllocSize, MEM_COMMIT, PAGE_READWRITE | PAGE_NOCACHE))
    {
        fProtect = VirtualProtect((LPBYTE)pvBuffer + dwAllocSize - PAGE_SIZE, PAGE_SIZE, PAGE_READWRITE | PAGE_NOCACHE, &dwOldProtect);

        dwOffset = dwAllocSize - dwSize - PAGE_SIZE;

        pvBuffer = (LPBYTE)pvBuffer + dwOffset;

        *((LPDWORD)pvBuffer - 1) = dwOffset;

    }

    return pvBuffer;

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

    CloseHandle(hCompletionEvent);

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
    return ValidateResult(XWaveFileCreateMediaObject(pszFileName, ppwfxFormat, ppMediaObject));
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

    hr = ValidateResult(pMediaObject->GetLength(&dsbd.dwBufferBytes));

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
        pBuffer->Release();
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

    pMediaObject->Release();

    return hr;
}

///
/// Set up necessary types
/// 

typedef short* LPSHORT;

///
/// Setup the frequency, and define PI
///

static const double PI = 3.1415926535;


HRESULT
CreateSineWaveBuffer( double dFrequency, LPDIRECTSOUNDBUFFER8 * ppBuffer )
{
    HRESULT hr = S_OK;
    LPDIRECTSOUNDBUFFER8 pBuffer = NULL;
    WORD * pData = NULL;
    DWORD dwBytes = 0;
    double dArg = 0.0;
    double dSinVal = 0.0;
    WORD wVal = 0;

    // Check arguments
    if( !ppBuffer || dFrequency < 0 )
        return E_INVALIDARG;

    *ppBuffer = NULL;

    //
    // Initialize a wave format structure
    //
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof( WAVEFORMATEX ) );

    wfx.wFormatTag      = WAVE_FORMAT_PCM;      // PCM data
    wfx.nChannels       = 1;                    // Mono
    wfx.nSamplesPerSec  = 48000;                 // 48kHz
    wfx.nAvgBytesPerSec = 96000;                 // 48kHz * 2 bytes / sample
    wfx.nBlockAlign     = 2;                    // sample size in bytes
    wfx.wBitsPerSample  = 16;                   // 16 bit samples
    wfx.cbSize          = 0;                    // No extra data

    //
    // Intialize the buffer description
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );

    dsbd.dwSize = sizeof( DSBUFFERDESC );
    dsbd.dwBufferBytes = wfx.nAvgBytesPerSec;
    dsbd.lpwfxFormat = &wfx;

    // Create the buffer
    hr = DirectSoundCreateBuffer( &dsbd, &pBuffer );
    if( FAILED( hr ) )
        return hr;

    // Get a pointer to buffer data to fill
    hr = pBuffer->Lock( 0, dsbd.dwBufferBytes, (VOID **)&pData, &dwBytes, NULL, NULL, 0 );
    if( FAILED( hr ) )
        return hr;

    // Now fill the buffer, 1 16-bit sample at a time
    for( DWORD i = 0; i < dwBytes/sizeof(WORD); i++ )
    {
        // Convert sample offset to radians
        dArg = (double)i / wfx.nSamplesPerSec * D3DX_PI * 2;

        // Calculate the sin
        dSinVal = sin( dFrequency * dArg );

        // Scale to sample format
        wVal = WORD( dSinVal * 32767 );

        // Store the sample
        pData[i] = wVal;
    }

    // Start the sine wave looping
    hr = pBuffer->Play( 0, 0, DSBPLAY_LOOPING );
    if( FAILED( hr ) )
        return hr;

    // return the buffer
    *ppBuffer = pBuffer;

    return hr;
}

HRESULT
PlayLoopingBuffer
(
    LPCSTR pszFile,
    LPDIRECTSOUNDBUFFER     *pBuffer,
    DWORD dwFlags
)
{

    HRESULT hr = S_OK;
    LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
    DWORD dwDuration;

    DirectSoundUseFullHRTF();
    hr = CreateBufferFromFile(pszFile,
                              dwFlags,
                              &pDSBuffer,
                              &dwDuration);

    if (SUCCEEDED(hr)) {

        hr = pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    }

    *pBuffer = pDSBuffer;

    return hr;
}


HRESULT SetFXOscillatorParameters(LPDIRECTSOUND pDirectSound,DWORD dwEffectIndex,FLOAT Frequency)
{
    DWORD dwValue = 0;
    HRESULT hr = S_OK;

    //
    // convert frequency to 1/10s of Hz as an integer
    //

    dwValue = (DWORD)(Frequency*10.0);

    hr = pDirectSound->SetEffectData(dwEffectIndex,FIELD_OFFSET( DSFX_OSCILLATOR_PARAMS,adwFrequency), &dwValue, sizeof( DWORD ), DSFX_IMMEDIATE);

    return hr;

}


