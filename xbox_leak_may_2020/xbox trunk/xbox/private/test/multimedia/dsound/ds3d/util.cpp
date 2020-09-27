#include "globals.h"

/********************************************************************************
FILE: UTIL.CPP

PURPOSE:    
    Contains simple helper functions for 3D tests.
********************************************************************************/

char *WAVEFILESDIR = "t:\\media\\audio\\pcm\\";
char *WAVEFILES    = "t:\\media\\audio\\pcm\\*.wav";

/********************************************************************************
Temporary stub for xDebugStringA until console output works!!!
********************************************************************************/
#define CONSOLE xDebugStringA

typedef signed short int SWORD;

void xDebugStringA
(
    LPSTR                   szFormat,
    ...
)
{
    va_list va;
    static char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);

    //A carriage return goes here when we've decided not to output to VC anymore!!
    DbgPrint("CONSOLE OUTPUT --------->%s\n",szBuffer);

};



/********************************************************************************
Logs to both the debugger and the log file.
********************************************************************************/
void LogBoth
(
    DWORD                   dwPassOrFail,
    LPSTR                   szFormat,
    ...
)
{
    va_list va;

    if (XLL_FAIL != dwPassOrFail ||
        XLL_PASS != dwPassOrFail)
        _asm int 3

    va_start(va, szFormat);
    DbgPrint(szFormat, va);                
	xLog( g_hLog, dwPassOrFail, szFormat, va);
    va_end(va);
 
} // dprintf()


/********************************************************************************
BUGBUG: These are crap functions cuz the D3DOVERLOADS don't work.
BUG 2371 Overloaded functions unusable on _D3DVECTOR due to inclusion of less functional _D3DVECTOR class in D3D8TYPES.H
********************************************************************************/
_D3DVECTOR operator - (const _D3DVECTOR& v, const _D3DVECTOR& w)
{
    _D3DVECTOR a;
    a.x = v.x - w.x;
    a.y = v.y - w.y;
    a.z = v.z - w.z;
    return a;    
};

_D3DVECTOR operator -= (_D3DVECTOR& v, const _D3DVECTOR& w)
{
    v.x -= w.x;
    v.y -= w.y;
    v.z -= w.z;
    return v;    
};

BOOL operator == (_D3DVECTOR& v, const _D3DVECTOR& w)
{
    return
    (
    v.x == w.x &&
    v.y == w.y &&
    v.z == w.z
    );
};


//constructor
D3DVECTOR x_D3DVECTOR(FLOAT _x, FLOAT _y, FLOAT _z)
{
D3DVECTOR v;

v.x = _x;
v.y = _y;
v.z = _z;
return v;
}




/********************************************************************************
PURPOSE:
    Helper function to load a wave file off the hard drive for testing.
    Since 3D files can only be mono, it will mix stereo files into mono.
********************************************************************************/
HRESULT LoadWaveFile(LPDSBUFFERDESC *ppdsbd, LPVOID *ppvSoundData, LPDIRECTSOUNDBUFFER *ppBuffer, char *szFileName)
{
HRESULT hr = S_OK;
LPWAVEFORMATEX pWfx = NULL;
DWORD dwWFXSize = 0;
DWORD dwDuration = 0;
LPDIRECTSOUNDBUFFER pBuffer = NULL;
LPVOID pvSoundData = NULL;
BOOL bDownMixedToMono = FALSE;

LPDSBUFFERDESC pdsbd = NULL;
CWaveFile *pWaveFile = NULL;

//Open the wave file.
ALLOCATEANDCHECK(pWaveFile, CWaveFile);
CHECKRUN(pWaveFile->Open(szFileName));

//Get format and length.
CHECKRUN(pWaveFile->GetFormat(NULL, 0, &dwWFXSize));
CHECK(pWfx = (LPWAVEFORMATEX)malloc(dwWFXSize));
CHECKALLOC(pWfx);
CHECKRUN(pWaveFile->GetFormat(pWfx, dwWFXSize, NULL));
CHECKRUN(pWaveFile->GetDuration(&dwDuration));

//Allocate space and read in the file.
#ifndef DVTSNOOPBUG
CHECK(pvSoundData = malloc(dwDuration));
#else
CHECK(pvSoundData = XPhysicalAlloc( dwDuration, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE ) );
#endif
CHECKALLOC(pvSoundData);
CHECKRUN(pWaveFile->ReadSample(0, pvSoundData, dwDuration, NULL));


//Mix it into a mono buffer if it's stereo.
//If we're mono we can play back in 3D, period.
if (SUCCEEDED(hr))
{
    if (pWfx->nChannels != 1)
    {
        //If we're stereo and PCM, then just mix the data together into the 1st half of the buffer and only play that.
        if (WAVE_FORMAT_PCM == pWfx->wFormatTag && pWfx->nChannels == 2)
        {
            //If we're 8-bits, then map range 0-256-511 to 0-128-255
            if (8 == pWfx->wBitsPerSample)
            {
                BYTE *pbData = (BYTE *)pvSoundData;
                DWORD i;
                for (i=0; i<dwDuration / 2; i++)
                {
                    DWORD dwTotal = 0;

                    //Clip this sample
                    dwTotal = (DWORD)pbData[i*2] + (DWORD)pbData[i*2+1];                    
                    if (dwTotal > (256 + 127))
                        dwTotal = (256 + 127);
                    if (dwTotal < (256 - 128))
                        dwTotal = (256 - 128);
                    dwTotal -= 128;
                    pbData[i] = (BYTE)dwTotal;
                }           
                bDownMixedToMono = TRUE;
            }
            //If we're 16 bits, map -65536 -> 0 -> 65535 to -32768 -> 0 -> 32767
            else if (16 == pWfx->wBitsPerSample)
            {

                SWORD *pwData = (SWORD *)pvSoundData;
                DWORD i;
                for (i=0; i<dwDuration / 4; i++)
                {
                    LONG lTotal = 0;

                    //Clip this sample
                    lTotal = (int)pwData[i*2] + (int)pwData[i*2+1];
                    if (lTotal > 32767)
                        lTotal = 32767;
                    if (lTotal < -32767)
                        lTotal = -32767;
                    pwData[i] = (SWORD)lTotal;
                }           
                bDownMixedToMono = TRUE;
            }

            //If we're not 8 or 16 bits, we just leave it (should we make it silence)?
            else
            {
                CONSOLE("%s has non-standard bits-per-sample (%d), can't mix to mono.", szFileName, pWfx->wBitsPerSample);
            }
        }//END if (wFormatTag == WAVE_FORMAT_PCM)
        //If we're not stereo or mono, and/or non-PCM, then we'll make do with silence.  Yippee.
        else
        {
            CONSOLE("Can't play back %s in 3D: nChannels = 0x%d and wFormatTag = 0x%X.", szFileName, pWfx->nChannels, pWfx->wFormatTag);
            ZeroMemory(pvSoundData, dwDuration);
        }
    }
}


//Fill out the bufferdesc.
if (SUCCEEDED(hr))
{
    ALLOCATEANDCHECK(pdsbd, DSBUFFERDESC);
}


if (SUCCEEDED(hr))
{
    ZeroMemory(pdsbd, sizeof(DSBUFFERDESC));
    pdsbd->dwSize = sizeof(DSBUFFERDESC);
    pdsbd->dwBufferBytes = dwDuration;
    pdsbd->dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
    pdsbd->lpwfxFormat = pWfx;

    //If we've downmixed this data, then change the dsbd a little bit.
    if (bDownMixedToMono)
    {
        dwDuration /= 2;
        pdsbd->dwBufferBytes /= 2;
        pdsbd->lpwfxFormat->nChannels /= 2;
        pdsbd->lpwfxFormat->nAvgBytesPerSec /= 2;
        pdsbd->lpwfxFormat->nBlockAlign /= 2;
    }

    //Make the bufferbytes even.
    pdsbd->dwBufferBytes = (pdsbd->dwBufferBytes / pdsbd->lpwfxFormat->nBlockAlign) * pdsbd->lpwfxFormat->nBlockAlign;
}

//Create a buffer.

#ifdef SILVER
CHECKRUN(Help_DirectSoundCreateBuffer(DSDEVID_DEVICEID_MCPX, pdsbd, &pBuffer, NULL));
#else // SILVER
CHECKRUN(Help_DirectSoundCreateBuffer(0, pdsbd, &pBuffer, NULL));
#endif // SILVER

CHECKALLOC(pBuffer);

FAIL_ON_CONDITION( ( SUCCEEDED( hr ) && NULL == pBuffer ) || ( FAILED( hr ) && NULL != pBuffer ) );

//Point the buffer to the sound data.
CHECKRUN(pBuffer->SetBufferData(pvSoundData, pdsbd->dwBufferBytes));

//Here's stuff we may want to return.
//if we failed, release it, otherwise return it.
if (FAILED(hr))
{
    FreeDSBD(pdsbd);
    if (pvSoundData)
#ifndef DVTSNOOPBUG
	    free(pvSoundData);
#else
      XPhysicalFree( pvSoundData );
#endif
	RELEASE(pBuffer);
}
else
{
    //This parameter is optional.
    if (ppdsbd)
        *ppdsbd			= pdsbd;
    else 
        FreeDSBD(pdsbd);

	*ppvSoundData	= pvSoundData;    

	*ppBuffer		= pBuffer;
}

//Here's the stuff we'll release anyway.  
pWaveFile->Close();
SAFEDELETE(pWaveFile);
return hr;
}


/********************************************************************************
PURPOSE:
    Calls LOADWAVEFILE with a random file name.
********************************************************************************/
HRESULT LoadRandomWaveFile(LPDSBUFFERDESC *ppdsbd, LPVOID *ppvSoundData, LPDIRECTSOUNDBUFFER *ppBuffer)
{
    HRESULT hr = S_OK;
    static char szFullPath[1000];
    WIN32_FIND_DATA Data = {0};
    DWORD dwCurrentFile     = 0;
    DWORD dwChosenFile      = 0;
    HANDLE hSearch          = NULL;
    BOOL bRes               = FALSE;

    //Count up the number of wav files we have.
    dwCurrentFile = 0;
    CHECK(hSearch = FindFirstFile(WAVEFILES, &Data));
    if (hSearch == INVALID_HANDLE_VALUE)
    {
        hr = E_FAIL;
        goto END;
    }
    dwCurrentFile++; 
    while (FindNextFile(hSearch, &Data))            
    {
        dwCurrentFile++;
    }

    dwChosenFile = rand() % dwCurrentFile;
    FindClose(hSearch);
    hSearch = NULL;

    //Do the same thing over again.
    dwCurrentFile = 0;
    CHECK(hSearch = FindFirstFile(WAVEFILES, &Data));
    if (hSearch == INVALID_HANDLE_VALUE)
    {
        hr = E_FAIL;
        goto END;
    }
    
    //Go through all the files again and choose the one with the index you selected.
    while (dwCurrentFile != dwChosenFile)            
    {
        bRes = FindNextFile(hSearch, &Data);
        if (FALSE == bRes)
        {
            hr = E_FAIL;
            goto END;
        }
        dwCurrentFile++;
    }

    sprintf(szFullPath, "%s%s", WAVEFILESDIR, Data.cFileName);
    CHECKRUN(LoadWaveFile(NULL, ppvSoundData, ppBuffer, szFullPath));

END:
    if (hSearch)
    {
        FindClose(hSearch);
        hSearch = NULL;
    }

return hr;
};




/********************************************************************************
PURPOSE:
    Helper function.
    This frees up a DSBD created by LoadRandomWaveFile.  This MUST be used to
    free the DSBD.
********************************************************************************/
HRESULT FreeDSBD(LPDSBUFFERDESC &pdsbd)
{
HRESULT hr = S_OK;

if (pdsbd)
{
    
    if (pdsbd->lpwfxFormat)
    {
        free(pdsbd->lpwfxFormat);
        pdsbd->lpwfxFormat = NULL;
    }
    SAFEDELETE(pdsbd);
    pdsbd = NULL;
}
return hr;
};


/********************************************************************************
PURPOSE:
    Sets the x, y, or z component of a vector where x, y, z are indexed by
    dwComponent values 0-2.  Is nice for loops that test all 3 axes.
********************************************************************************/
void SetComponent(D3DVECTOR *pVector, DWORD dwComponent, FLOAT fValue)
{
    switch (dwComponent)
    {
        case 0: 
            pVector->x = fValue;
            break;
        case 1: 
            pVector->y = fValue;
            break;
        case 2: 
            pVector->z = fValue;
            break;
        default:
            CONSOLE("Test Error, see danhaff!!!!");
            break;
    }

}



/********************************************************************************
Makes it easy to print out which mode you're in.
********************************************************************************/
char *pszApplyStrings[] = {"TESTAPPLY_DEFERRED_NOUPDATE", "TESTAPPLY_DEFERRED_UPDATE", "TESTAPPLY_IMMEDIATE"};
char *String(TestApplies eTestApply)
{
    return pszApplyStrings[eTestApply - TESTAPPLY_DEFERRED_NOUPDATE];
}



/********************************************************************************
If you just want to make this an API values test (no listening) then you may
set g_bWait to FALSE somewhere in your code or in the debugger.
********************************************************************************/
BOOL g_bWait = TRUE;
void SleepEx(DWORD dwWait)
{
    if (g_bWait)
        Sleep(dwWait);
}



/********************************************************************************
********************************************************************************/
HRESULT Help_DirectSoundCreate(DWORD dwDeviceId, LPDIRECTSOUND *ppDirectSound, LPUNKNOWN pUnkOuter)
{
HRESULT hr = S_OK;
#ifdef SILVER
    CHECKRUN(DirectSoundCreate(dwDeviceId,    ppDirectSound, pUnkOuter));
#else
    CHECKRUN(DirectSoundCreate(0, ppDirectSound, pUnkOuter));
#endif
return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT Help_DirectSoundCreateBuffer(DWORD dwDeviceId, LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDBUFFER *ppBuffer, LPUNKNOWN pUnkOuter)
{
HRESULT hr = S_OK;
#ifdef SILVER
    CHECKRUN(DirectSoundCreateBuffer(dwDeviceId, pdsbd, ppBuffer, pUnkOuter));
#else
    CHECKRUN(DirectSoundCreateBuffer(pdsbd, ppBuffer));
#endif
return hr;

}
/*
//-----------------------------------------------------------------------------
// Name: DownloadScratch
// Desc: Downloads a DSP scratch image to the DSP
//-----------------------------------------------------------------------------
HRESULT DownloadScratch(IDirectSound *pDSound, PCHAR pszScratchFile)
{
    HANDLE hFile;
    DWORD dwSize;
    PVOID pBuffer = NULL;
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc = {0};
    DSEFFECTIMAGELOC        EffectLoc = {0};

    EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
    EffectLoc.dwCrosstalkIndex   = I3DL2_CHAIN_XTALK;

    // open scratch image file generated by xps2 tool
    hFile = CreateFile( pszScratchFile,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL );

    if( hFile == INVALID_HANDLE_VALUE )
    {
        DWORD err;

        err = GetLastError();
        hr = HRESULT_FROM_WIN32(err);
    }

    if( SUCCEEDED(hr) )
    {
        // Determine the size of the scratch image by seeking to
        // the end of the file
        dwSize = SetFilePointer( hFile, 0, NULL, FILE_END );
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    
        // Allocate memory to read the scratch image from disk
        pBuffer = new BYTE[dwSize];

        // Read the image in
        DWORD dwBytesRead;
        BOOL bResult = ReadFile( hFile,
                                 pBuffer,
                                 dwSize,
                                 &dwBytesRead,
                                 0 );
        
        if (!bResult)
        {
            DWORD err;

            err = GetLastError();
            hr = HRESULT_FROM_WIN32(err);
        }

    }

    if( SUCCEEDED(hr) )
    {
        // call dsound api to download the image..
        if (SUCCEEDED(hr))
		{
                  hr = pDSound->DownloadEffectsImage( pBuffer,
                                                  dwSize,
                                                  &EffectLoc,
                                                  &pDesc );
        }
    }

    delete[] pBuffer;

    if( hFile != INVALID_HANDLE_VALUE ) 
    {
        CloseHandle( hFile );
    }

    
    return hr;
}
*/


//-----------------------------------------------------------------------------
// Name: DownloadLinkedDSPImage
// Desc: Downloads the DSP image linked to the app.
//-----------------------------------------------------------------------------
HRESULT DownloadLinkedDSPImage(IDirectSound *pDSound)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwSize = 0;
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc;
    DSEFFECTIMAGELOC EffectLoc = {0};
    EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
    EffectLoc.dwCrosstalkIndex   = I3DL2_CHAIN_XTALK;
    
    CHECKRUN(XAudioDownloadEffectsImage("dsstdfx", &EffectLoc, XAUDIO_DOWNLOADFX_XBESECTION, &pDesc));

    return hr;
}
