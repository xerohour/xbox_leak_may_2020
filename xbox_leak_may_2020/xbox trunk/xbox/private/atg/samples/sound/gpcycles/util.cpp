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
    LPCSTR pszFile,
    LPDIRECTSOUNDBUFFER     *pBuffer
)
{

    HRESULT hr = S_OK;
    LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
    DWORD dwDuration;

    hr = CreateBufferFromFile(pszFile,
                              0,
                              &pDSBuffer,
                              &dwDuration);
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
DownloadEffectsImage(PCHAR pszScratchFile)
{

    LPDIRECTSOUND           pDirectSound;
    HANDLE hFile;
    DWORD dwSize;
    PVOID pBuffer=NULL;
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
    
    if (pBuffer) {
        delete [] pBuffer;
    }

    return hr;
}



