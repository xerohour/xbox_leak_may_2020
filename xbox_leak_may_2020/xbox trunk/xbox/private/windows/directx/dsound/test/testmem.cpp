/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       testmem.cpp
 *  Content:    Memory usage tests.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  10/18/01    dereks  Created.
 *
 ****************************************************************************/

#include "testds.h"

typedef struct
{
    DWORD   dwPool;
    DWORD   dwPhysical;
    DWORD   dwSlop;
    DWORD   dwRecovered;
} DSMEMUSAGE, *LPDSMEMUSAGE;

typedef const DSMEMUSAGE *LPCDSMEMUSAGE;
typedef const DSMEMUSAGE &REFDSMEMUSAGE;


/****************************************************************************
 *
 *  GetCurrentMemoryUsage
 *
 *  Description:
 *      Saves the current memory usage numbers.
 *
 *  Arguments:
 *      LPDSMEMUSAGE [out]: memory usage data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "GetCurrentMemoryUsage"

__inline void
GetCurrentMemoryUsage
(
    LPDSMEMUSAGE            pMemory
)
{
    pMemory->dwPool = g_dwDirectSoundPoolMemoryUsage;
    pMemory->dwPhysical = g_dwDirectSoundPhysicalMemoryUsage;
    pMemory->dwSlop = g_dwDirectSoundPhysicalMemorySlop;
    pMemory->dwRecovered = g_dwDirectSoundPhysicalMemoryRecovered;
}


/****************************************************************************
 *
 *  DumpCurrentMemoryUsage
 *
 *  Description:
 *      Dumps the amount of memory used by a test.
 *
 *  Arguments:
 *      LPCSTR [in]: test name.
 *      LPDSMEMUSAGE [in/out]: on entry, "before" usage.  On exit, "after"
 *                             usage.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DumpCurrentMemoryUsage"

void
DumpCurrentMemoryUsage
(
    LPCSTR                  pszTest,
    LPDSMEMUSAGE            pBefore
)
{
    DSMEMUSAGE              After;
    DSMEMUSAGE              Diff;

    GetCurrentMemoryUsage(&After);

    Diff.dwPool = After.dwPool - pBefore->dwPool;
    Diff.dwPhysical = After.dwPhysical - pBefore->dwPhysical;
    Diff.dwSlop = After.dwSlop - pBefore->dwSlop;
    Diff.dwRecovered = After.dwRecovered - pBefore->dwRecovered;

    DPF_TEST("%s: %ld pool, %ld physical (%ld slop), %ld recovered, %ld total", pszTest, Diff.dwPool, Diff.dwPhysical, Diff.dwSlop, Diff.dwRecovered, Diff.dwPool + Diff.dwPhysical + Diff.dwRecovered);

    *pBefore = After;
}


/****************************************************************************
 *
 *  TestMemoryUsage
 *
 *  Description:
 *      Determines the amount of memory used by each object type.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "TestMemoryUsage"

HRESULT
TestMemoryUsage
(
    void
)
{
    WAVEFORMATEX            wfxFormat       = INIT_PCM_WAVEFORMAT(1, 48000, 16);
    LPDIRECTSOUND           pDirectSound    = NULL;
    LPDIRECTSOUNDBUFFER     pBuffer         = NULL;
    LPDIRECTSOUNDSTREAM     pStream         = NULL;
    LPAC97MEDIAOBJECT       pAnalogAc97     = NULL;
    LPAC97MEDIAOBJECT       pDigitalAc97    = NULL;
    DSMEMUSAGE              Memory;
    DSBUFFERDESC            dsbd;
    DSSTREAMDESC            dssd;
    HRESULT                 hr;

    DirectSoundUseLightHRTF();

    GetCurrentMemoryUsage(&Memory);

    hr = ValidateResult(DirectSoundCreate(NULL, &pDirectSound, NULL));

    if(SUCCEEDED(hr))
    {
        DumpCurrentMemoryUsage("DirectSoundCreate", &Memory);
    }

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(LoadEffectsImage("dsstdfx", XAUDIO_DOWNLOADFX_XBESECTION));
    }

    if(SUCCEEDED(hr))
    {
        DumpCurrentMemoryUsage("DownloadEffectsImage (dsstdfx)", &Memory);
    }

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&dsbd, sizeof(dsbd));

        dsbd.dwSize = sizeof(dsbd);
        dsbd.lpwfxFormat = &wfxFormat;

        hr = ValidateResult(pDirectSound->CreateSoundBuffer(&dsbd, &pBuffer, NULL));
    }

    if(SUCCEEDED(hr))
    {
        DumpCurrentMemoryUsage("CreateSoundBuffer (2D)", &Memory);
    }

    RELEASE(pBuffer);

    GetCurrentMemoryUsage(&Memory);

    if(SUCCEEDED(hr))
    {
        dsbd.dwFlags |= DSBCAPS_CTRL3D;
        
        hr = ValidateResult(pDirectSound->CreateSoundBuffer(&dsbd, &pBuffer, NULL));
    }

    if(SUCCEEDED(hr))
    {
        DumpCurrentMemoryUsage("CreateSoundBuffer (3D)", &Memory);
    }
        
    RELEASE(pBuffer);

    GetCurrentMemoryUsage(&Memory);

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&dssd, sizeof(dssd));

        dssd.lpwfxFormat = &wfxFormat;
        dssd.dwMaxAttachedPackets = 4;

        hr = ValidateResult(pDirectSound->CreateSoundStream(&dssd, &pStream, NULL));
    }

    if(SUCCEEDED(hr))
    {
        DumpCurrentMemoryUsage("CreateSoundStream (2D, 4 packets)", &Memory);
    }

    RELEASE(pStream);

    GetCurrentMemoryUsage(&Memory);

    if(SUCCEEDED(hr))
    {
        dssd.dwFlags |= DSBCAPS_CTRL3D;

        hr = ValidateResult(pDirectSound->CreateSoundStream(&dssd, &pStream, NULL));
    }

    if(SUCCEEDED(hr))
    {
        DumpCurrentMemoryUsage("CreateSoundStream (3D, 4 packets)", &Memory);
    }

    RELEASE(pStream);
    RELEASE(pDirectSound);

    GetCurrentMemoryUsage(&Memory);

    ASSERT(!Memory.dwPool);
    ASSERT(!Memory.dwPhysical);
    ASSERT(!Memory.dwSlop);
    ASSERT(!Memory.dwRecovered);

    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(Ac97CreateMediaObject(DSAC97_CHANNEL_ANALOG, NULL, NULL, &pAnalogAc97));
    }

    if(SUCCEEDED(hr))
    {
        DumpCurrentMemoryUsage("Ac97CreateMediaObject (first)", &Memory);
    }
        
    if(SUCCEEDED(hr))
    {
        hr = ValidateResult(Ac97CreateMediaObject(DSAC97_CHANNEL_DIGITAL, NULL, NULL, &pDigitalAc97));
    }

    if(SUCCEEDED(hr))
    {
        DumpCurrentMemoryUsage("Ac97CreateMediaObject (second)", &Memory);
    }

    RELEASE(pAnalogAc97);
    RELEASE(pDigitalAc97);
        
    DirectSoundDumpMemoryUsage(TRUE);

    return hr;
}


