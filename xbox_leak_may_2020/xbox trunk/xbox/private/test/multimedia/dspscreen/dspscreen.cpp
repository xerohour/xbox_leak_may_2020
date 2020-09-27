/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

	dspscreen.cpp

Abstract:

Revision History:


--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------

#include "dspscreen.h"
#include "dspscver.h"

EXTERN_C DWORD g_dwDirectSoundOverrideSpeakerConfig;

STDAPI
DirectSoundCreateTest
(
    LPGUID                  pguidDeviceId,
    LPDIRECTSOUND *         ppDirectSound,
    LPUNKNOWN               pControllingUnknown
);


const USHORT gCRCTable[256] = {
      0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
      0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
      0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
      0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
      0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
      0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
      0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
      0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
      0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
      0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
      0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
      0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
      0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
      0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
      0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
      0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
      0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
      0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
      0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
      0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
      0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
      0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
      0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
      0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
      0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
      0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
      0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
      0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
      0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
      0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
      0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
      0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/***************************************************************************\
* CalcCRC
*
\***************************************************************************/

USHORT CalcCRC(PBYTE pb, DWORD cb)
{
    USHORT crc = 0xFFFF;

    while (cb--) {
        crc = (crc >> 8) ^ gCRCTable[(crc ^ *pb++) & 0xFF];
    }

    return ~crc;
}

///
/// Set up necessary types
/// 

typedef short* LPSHORT;

///
/// Setup the frequency, and define PI
///

static const double PI = 3.1415926535;

extern HRESULT LoadDolbyCode();

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


BOOL XTestAudioDspScreeningCRC(WORD *pwCrc,WORD *pwXramCrc,WORD *pwCopyXramCrc)
{

    LPDIRECTSOUND8 pDSound;
    LPDIRECTSOUNDBUFFER8 pDSBuffer;
    DWORD dwError = 0;
    HRESULT hr = S_OK;
    BOOL bPass = TRUE;
    PBYTE pBuffer;

    *pwCrc = 0;

    pBuffer = new BYTE[EP_ROM_SIZE*sizeof(DWORD)];
    if (pBuffer == NULL) {
        return FALSE;
    }

    //
    // create a dsound object
    //

    DirectSoundCreateTest(NULL, &pDSound, NULL);
    if (pDSound == NULL) {

        return -1;

    }

    Sleep(64);

    //
    // this test compares the contents of the EP rom from X-ram, to the contents the EP dma-ed
    // into the base offset of FIFO0
    //

    //
    // to get the VA of the scratch space we need to look into the EP SGE register first
    // retrieve scatter gather entry table first

    PDWORD pTemp = (PDWORD) ((PBYTE)0xfe800000+0x2048);

    // now get first page
    pTemp = (PDWORD) (*pTemp | 0x80000000);

    // now make physical address into virtual
    PDWORD pSysMem = (PDWORD) (*pTemp | 0x80000000);
    pTemp = pSysMem;

    PDWORD pDspXMem = (PDWORD) (0xfe851000);
    for (DWORD i=0;i<EP_ROM_SIZE;i++) {

        if (*pTemp != *pDspXMem) {

            bPass = FALSE;
            return bPass;
        }

        pTemp++;
        pDspXMem++;
    }

    pDspXMem = (PDWORD) (0xfe851000);

    //
    // make a copy of xmem contents
    //

    memcpy(pBuffer,pDspXMem,EP_ROM_SIZE*sizeof(DWORD));

    //
    // now calculate the crcs
    //

    *pwXramCrc = 0;
    *pwCopyXramCrc = 0;

    //
    // first calculate system memory
    //

    *pwCrc = CalcCRC((PBYTE)pSysMem,EP_ROM_SIZE*sizeof(DWORD));

    if (*pwCrc != EP_ROM_CRC) {
        bPass = FALSE;
    }

    //
    // now calculate xram straight of the EP
    //

    *pwXramCrc = CalcCRC((PBYTE)pDspXMem,EP_ROM_SIZE*sizeof(DWORD));

    if (*pwXramCrc != EP_ROM_CRC) {
        bPass = FALSE;
    }

    *pwCopyXramCrc = CalcCRC((PBYTE)pBuffer,EP_ROM_SIZE*sizeof(DWORD));

    if (*pwCopyXramCrc != EP_ROM_CRC) {
        bPass = FALSE;
    }
    
    pDSound->Release();

    delete [] pBuffer;
    return bPass;

}

BOOL XTestAudioDspScreening(LPDIRECTSOUND pDSound,DOUBLE maxTime)
{

    LPDIRECTSOUND8 pDS;
    LPDIRECTSOUNDBUFFER8 pDSBuffer;
    BOOL bPass = TRUE;
    HRESULT hr = S_OK;
    DSMIXBINS               dsMixBins;
    DSMIXBINVOLUMEPAIR      dsMixBinArray[8];

    memset(dsMixBinArray,0,sizeof(dsMixBinArray));

    //
    // tell dsound to use DD as the speaker config
    //

    g_dwDirectSoundOverrideSpeakerConfig = DSSPEAKER_ENABLE_AC3 | DSSPEAKER_SURROUND;

    if (pDSound == NULL) {

        //
        // create a dsound object
        //

        DirectSoundCreate(NULL, &pDS, NULL);
        if (pDS == NULL) {

            return FALSE;

        }

        pDSound = pDS;
    }

    //
    // start a sound...
    //

    hr = CreateSineWaveBuffer(220,&pDSBuffer);
    if (FAILED(hr)) {
        return FALSE;
    }

    dsMixBins.dwMixBinCount = 6;
    dsMixBins.lpMixBinVolumePairs = dsMixBinArray;

    if (dsMixBins.dwMixBinCount > 1) {

        for (DWORD i=0;i<dsMixBins.dwMixBinCount;i++) {
    
            dsMixBinArray[i].dwMixBin = i;
            dsMixBinArray[i].lVolume =0;
            pDSound->SetMixBinHeadroom(i, 0);
    
        }

    }

    pDSBuffer->SetMixBins(&dsMixBins);
    pDSBuffer->SetHeadroom(0);


    //
    // the dsps are now running.
    // all we need to do now is check if the EP elapsed time variable
    // gets updated every frame and the block number variable moves
    //

    DWORD dwValue, dwOldValue = -1; 
    LARGE_INTEGER startTime,currentTime,lastUpdateTime,deltaTime,updateTime;
    LARGE_INTEGER freq,temp;
    DOUBLE elapsedTime;

    deltaTime.QuadPart = 0;

    Sleep(32);

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&startTime);

    updateTime.QuadPart = 0;
    lastUpdateTime.QuadPart = startTime.QuadPart;

    //dwOldValue = MAX_EP_IDLE_CYCLES-*(PDWORD)(0xFE800000+0x5a000+0x4*sizeof(DWORD)); 
    dwOldValue = *(PDWORD)(0xFE800000+0x403c);


    while (bPass) {

        //
        // before we do anything fancy check if the box run of cycles
        // if it did the test will fail right away since the box is hang
        //
    
        dwValue = *(PDWORD)(0xFE800000+0x5a000+0x6*sizeof(DWORD));
        if (dwValue == 0xbbbbbb) {
    
            bPass = FALSE;
            break;

        }

        //dwValue = MAX_EP_IDLE_CYCLES-*(PDWORD)(0xFE800000+0x5a000+0x4*sizeof(DWORD)); 
        dwValue = *(PDWORD)(0xFE800000+0x403c);
        QueryPerformanceCounter(&currentTime);

        elapsedTime = (DOUBLE)deltaTime.QuadPart/(DOUBLE)freq.QuadPart;
        if (elapsedTime > STUCK_THRESHOLD) {

            //
            // the values did not change for 500ms. the box is bad
            //

            bPass = FALSE;
            break;
        }

        if (dwValue != dwOldValue) {

            //
            // the value moved, the dsp seems alive
            // track how often the value changes
            //
            
            temp.QuadPart = currentTime.QuadPart - lastUpdateTime.QuadPart;
            if (temp.QuadPart > updateTime.QuadPart) {
                updateTime = temp;
            }

            lastUpdateTime.QuadPart = currentTime.QuadPart;
            deltaTime.QuadPart = 0;

            dwOldValue = dwValue;        

    
        } else {

            deltaTime.QuadPart = currentTime.QuadPart - lastUpdateTime.QuadPart;

        }

        //
        // after maxTime seconds, bail
        //

        elapsedTime = ((DOUBLE)currentTime.QuadPart - (DOUBLE)startTime.QuadPart)/(DOUBLE)freq.QuadPart;
        if (elapsedTime > maxTime) {

            break;

        }


    }

    pDSBuffer->Release();
    pDSound->Release();

    //
    // tell dsound to use the default speaker config again
    //

    g_dwDirectSoundOverrideSpeakerConfig = -1;

    return bPass;


}

