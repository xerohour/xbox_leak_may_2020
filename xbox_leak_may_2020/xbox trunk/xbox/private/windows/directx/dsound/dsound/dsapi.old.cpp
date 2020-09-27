//depot/xbox/private/windows/directx/dsound/dsound/dsapi.cpp#72 - edit change 20916 (text)
/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsapi.cpp
 *  Content:    DirectSound API objects and entry points.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/06/00    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"

#pragma comment(linker, "/merge:DSOUND_RW=DSOUND")
#pragma comment(linker, "/merge:DSOUND_URW=DSOUND")
#pragma comment(linker, "/merge:DSOUND_RD=DSOUND")
#pragma comment(linker, "/section:DSOUND,ERW")

STDAPI IDirectSound_QueryInterfaceC(LPDIRECTSOUND pDirectSound, const IID *iid, LPVOID *ppvInterface)
{
    ASSERT(pDirectSound);
    ASSERT(ppvInterface);

    IDirectSound_AddRef(pDirectSound);
    *ppvInterface = pDirectSound;

    return DS_OK;
}

STDAPI IDirectSound_QueryInterface(LPDIRECTSOUND pDirectSound, REFIID iid, LPVOID *ppvInterface)
{
    ASSERT(pDirectSound);
    ASSERT(ppvInterface);

    IDirectSound_AddRef(pDirectSound);
    *ppvInterface = pDirectSound;

    return DS_OK;
}

STDAPI IDirectSound_SetCooperativeLevel(LPDIRECTSOUND pDirectSound, HWND hWnd, DWORD dwLevel)
{
    UNREFERENCED_PARAMETER(pDirectSound);
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(dwLevel);
    return DS_OK;
}

STDAPI IDirectSound_Compact(LPDIRECTSOUND pDirectSound)
{
    UNREFERENCED_PARAMETER(pDirectSound);
    return DS_OK;
}

STDAPI_(ULONG) IDirectSound_AddRef(LPDIRECTSOUND pDirectSound)
{
    return ((CDirectSound *)pDirectSound)->AddRef();
}

STDAPI_(ULONG) IDirectSound_Release(LPDIRECTSOUND pDirectSound)
{
    return ((CDirectSound *)pDirectSound)->Release();
}

STDAPI IDirectSound_GetCaps(LPDIRECTSOUND pDirectSound, LPDSCAPS pdsc)
{
    return ((CDirectSound *)pDirectSound)->GetCaps(pdsc);
}

STDAPI IDirectSound_CreateSoundBuffer(LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDBUFFER *ppBuffer, LPUNKNOWN pUnkOuter)
{
    return ((CDirectSound *)pDirectSound)->CreateSoundBuffer(pdsbd, ppBuffer, pUnkOuter);
}

STDAPI IDirectSound_CreateSoundBuffer_v1(LPDIRECTSOUND pDirectSound, LPCDSBUFFERDESC1 pdsbd, LPDIRECTSOUNDBUFFER *ppBuffer, LPUNKNOWN pUnkOuter)
{
    return ((CDirectSound *)pDirectSound)->CreateSoundBuffer_v1(pdsbd, ppBuffer, pUnkOuter);
}

#ifndef MCPX_BOOT_LIB

STDAPI IDirectSound_CreateSoundStream(LPDIRECTSOUND pDirectSound, LPCDSSTREAMDESC pdssd, LPDIRECTSOUNDSTREAM *ppStream, LPUNKNOWN pUnkOuter)
{
    return ((CDirectSound *)pDirectSound)->CreateSoundStream(pdssd, ppStream, pUnkOuter);
}

STDAPI IDirectSound_CreateSoundStream_v1(LPDIRECTSOUND pDirectSound, LPCDSSTREAMDESC1 pdssd, LPDIRECTSOUNDSTREAM *ppStream, LPUNKNOWN pUnkOuter)
{
    return ((CDirectSound *)pDirectSound)->CreateSoundStream_v1(pdssd, ppStream, pUnkOuter);
}

#endif // MCPX_BOOT_LIB

STDAPI IDirectSound_GetSpeakerConfig(LPDIRECTSOUND pDirectSound, LPDWORD pdwSpeakerConfig)
{
    return ((CDirectSound *)pDirectSound)->GetSpeakerConfig(pdwSpeakerConfig);
}

#ifndef MCPX_BOOT_LIB

STDAPI IDirectSound_DownloadEffectsImage(LPDIRECTSOUND pDirectSound, LPCVOID pvImageBuffer, DWORD dwImageSize, LPCDSEFFECTIMAGELOC pImageLoc, LPDSEFFECTIMAGEDESC *ppImageDesc)
{
    return ((CDirectSound *)pDirectSound)->DownloadEffectsImage(pvImageBuffer, dwImageSize, pImageLoc, ppImageDesc);
}

#endif // MCPX_BOOT_LIB

STDAPI IDirectSound_GetEffectData(LPDIRECTSOUND pDirectSound, DWORD dwEffectIndex, DWORD dwOffset, LPVOID pvData, DWORD dwDataSize)
{
    return ((CDirectSound *)pDirectSound)->GetEffectData(dwEffectIndex, dwOffset, pvData, dwDataSize);
}

STDAPI IDirectSound_SetEffectData(LPDIRECTSOUND pDirectSound, DWORD dwEffectIndex, DWORD dwOffset, LPCVOID pvData, DWORD dwDataSize, DWORD dwFlags)
{
    return ((CDirectSound *)pDirectSound)->SetEffectData(dwEffectIndex, dwOffset, pvData, dwDataSize, dwFlags);
}

STDAPI IDirectSound_CommitEffectData(LPDIRECTSOUND pDirectSound)
{
    return ((CDirectSound *)pDirectSound)->CommitEffectData();
}

#ifndef MCPX_BOOT_LIB

STDAPI IDirectSound_EnableHeadphones(LPDIRECTSOUND pDirectSound, BOOL fEnabled)
{
    return ((CDirectSound *)pDirectSound)->EnableHeadphones(fEnabled);
}

#endif // MCPX_BOOT_LIB

STDAPI IDirectSound_SetMixBinHeadroom(LPDIRECTSOUND pDirectSound, DWORD dwMixBin, DWORD dwHeadroom)
{
    return ((CDirectSound *)pDirectSound)->SetMixBinHeadroom(dwMixBin, dwHeadroom);
}

STDAPI IDirectSound_SetMixBinHeadroom_v1(LPDIRECTSOUND pDirectSound, DWORD dwMixBinMask, DWORD dwHeadroom)
{
    return ((CDirectSound *)pDirectSound)->SetMixBinHeadroom_v1(dwMixBinMask, dwHeadroom);
}

#ifndef MCPX_BOOT_LIB

STDAPI IDirectSound_SetAllParameters(LPDIRECTSOUND pDirectSound, LPCDS3DLISTENER pds3dl, DWORD dwFlags)
{
    return ((CDirectSound *)pDirectSound)->SetAllParameters(pds3dl, dwFlags);
}

STDAPI IDirectSound_SetDistanceFactor(LPDIRECTSOUND pDirectSound, FLOAT flDistanceFactor, DWORD dwFlags)
{
    return ((CDirectSound *)pDirectSound)->SetDistanceFactor(flDistanceFactor, dwFlags);
}

STDAPI IDirectSound_SetDopplerFactor(LPDIRECTSOUND pDirectSound, FLOAT flDopplerFactor, DWORD dwFlags)
{
    return ((CDirectSound *)pDirectSound)->SetDopplerFactor(flDopplerFactor, dwFlags);
}

STDAPI IDirectSound_SetOrientation(LPDIRECTSOUND pDirectSound, FLOAT xFront, FLOAT yFront, FLOAT zFront, FLOAT xTop, FLOAT yTop, FLOAT zTop, DWORD dwFlags)
{
    return ((CDirectSound *)pDirectSound)->SetOrientation(xFront, yFront, zFront, xTop, yTop, zTop, dwFlags);
}

STDAPI IDirectSound_SetPosition(LPDIRECTSOUND pDirectSound, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{
    return ((CDirectSound *)pDirectSound)->SetPosition(x, y, z, dwFlags);
}

STDAPI IDirectSound_SetRolloffFactor(LPDIRECTSOUND pDirectSound, FLOAT flRolloffFactor, DWORD dwFlags)
{
    return ((CDirectSound *)pDirectSound)->SetRolloffFactor(flRolloffFactor, dwFlags);
}

STDAPI IDirectSound_SetVelocity(LPDIRECTSOUND pDirectSound, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{
    return ((CDirectSound *)pDirectSound)->SetVelocity(x, y, z, dwFlags);
}

STDAPI IDirectSound_CommitDeferredSettings(LPDIRECTSOUND pDirectSound)
{
    return ((CDirectSound *)pDirectSound)->CommitDeferredSettings();
}

STDAPI IDirectSound_SetI3DL2Listener(LPDIRECTSOUND pDirectSound, LPCDSI3DL2LISTENER pds3dl, DWORD dwFlags)
{
    return ((CDirectSound *)pDirectSound)->SetI3DL2Listener(pds3dl, dwFlags);
}

#endif // MCPX_BOOT_LIB

STDAPI IDirectSound_GetTime(LPDIRECTSOUND pDirectSound, REFERENCE_TIME *prtCurrent)
{
    return ((CDirectSound *)pDirectSound)->GetTime(prtCurrent);
}

STDAPI IDirectSoundBuffer_QueryInterface(LPDIRECTSOUNDBUFFER pBuffer, REFIID iid, LPVOID *ppvInterface)
{
    ASSERT(pBuffer);
    ASSERT(ppvInterface);

    IDirectSoundBuffer_AddRef(pBuffer);
    *ppvInterface = pBuffer;

    return DS_OK;
}

STDAPI IDirectSoundBuffer_QueryInterfaceC(LPDIRECTSOUNDBUFFER pBuffer, const IID *iid, LPVOID *ppvInterface)
{
    ASSERT(pBuffer);
    ASSERT(ppvInterface);

    IDirectSoundBuffer_AddRef(pBuffer);
    *ppvInterface = pBuffer;

    return DS_OK;
}

STDAPI IDirectSoundBuffer_Unlock(LPDIRECTSOUNDBUFFER pBuffer, LPVOID pvLock1, DWORD dwLockSize1, LPVOID pvLock2, DWORD dwLockSize2)
{
    return DS_OK;
}

STDAPI IDirectSoundBuffer_Restore(LPDIRECTSOUNDBUFFER pBuffer)
{
    return DS_OK;
}

STDAPI_(ULONG) IDirectSoundBuffer_AddRef(LPDIRECTSOUNDBUFFER pBuffer)
{
    return ((CDirectSoundBuffer *)pBuffer)->AddRef();
}

STDAPI_(ULONG) IDirectSoundBuffer_Release(LPDIRECTSOUNDBUFFER pBuffer)
{
    return ((CDirectSoundBuffer *)pBuffer)->Release();
}

STDAPI IDirectSoundBuffer_SetFormat(LPDIRECTSOUNDBUFFER pBuffer, LPCWAVEFORMATEX pwfxFormat)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetFormat(pwfxFormat);
}

STDAPI IDirectSoundBuffer_SetFrequency(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwFrequency)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetFrequency(dwFrequency);
}

STDAPI IDirectSoundBuffer_SetVolume(LPDIRECTSOUNDBUFFER pBuffer, LONG lVolume)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetVolume(lVolume);
}

STDAPI IDirectSoundBuffer_SetPitch(LPDIRECTSOUNDBUFFER pBuffer, LONG lPitch)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetPitch(lPitch);
}

STDAPI IDirectSoundBuffer_SetLFO(LPDIRECTSOUNDBUFFER pBuffer, LPCDSLFODESC pLFODesc)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetLFO(pLFODesc);
}

STDAPI IDirectSoundBuffer_SetEG(LPDIRECTSOUNDBUFFER pBuffer, LPCDSENVELOPEDESC pEnvelopeDesc)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetEG(pEnvelopeDesc);
}

STDAPI IDirectSoundBuffer_SetFilter(LPDIRECTSOUNDBUFFER pBuffer, LPCDSFILTERDESC pFilter)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetFilter(pFilter);
}

STDAPI IDirectSoundBuffer_SetHeadroom(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwHeadroom)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetHeadroom(dwHeadroom);
}

STDAPI IDirectSoundBuffer_SetOutputBuffer(LPDIRECTSOUNDBUFFER pBuffer, LPDIRECTSOUNDBUFFER pOutputBuffer)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetOutputBuffer(pOutputBuffer);
}

STDAPI IDirectSoundBuffer_SetMixBins(LPDIRECTSOUNDBUFFER pBuffer, LPCDSMIXBINS pMixBins)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetMixBins(pMixBins);
}

STDAPI IDirectSoundBuffer_SetMixBinVolumes(LPDIRECTSOUNDBUFFER pBuffer, LPCDSMIXBINS pMixBins)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetMixBinVolumes(pMixBins);
}

STDAPI IDirectSoundBuffer_SetOutputBuffer_v1(LPDIRECTSOUNDBUFFER pBuffer, LPDIRECTSOUNDBUFFER pOutputBuffer)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetOutputBuffer_v1(pOutputBuffer);
}

STDAPI IDirectSoundBuffer_SetMixBins_v1(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwMixBinMask)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetMixBins_v1(dwMixBinMask);
}

STDAPI IDirectSoundBuffer_SetMixBinVolumes_v1(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwMixBinMask, const LONG *palVolumes)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetMixBinVolumes_v1(dwMixBinMask, palVolumes);
}

#ifndef MCPX_BOOT_LIB

STDAPI IDirectSoundBuffer_SetAllParameters(LPDIRECTSOUNDBUFFER pBuffer, LPCDS3DBUFFER pds3db, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetAllParameters(pds3db, dwFlags);
}

STDAPI IDirectSoundBuffer_SetConeAngles(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetConeAngles(dwInsideConeAngle, dwOutsideConeAngle, dwFlags);
}

STDAPI IDirectSoundBuffer_SetConeOrientation(LPDIRECTSOUNDBUFFER pBuffer, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetConeOrientation(x, y, z, dwFlags);
}

STDAPI IDirectSoundBuffer_SetConeOutsideVolume(LPDIRECTSOUNDBUFFER pBuffer, LONG lConeOutsideVolume, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetConeOutsideVolume(lConeOutsideVolume, dwFlags);
}

STDAPI IDirectSoundBuffer_SetMaxDistance(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flMaxDistance, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetMaxDistance(flMaxDistance, dwFlags);
}

STDAPI IDirectSoundBuffer_SetMinDistance(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flMinDistance, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetMinDistance(flMinDistance, dwFlags);
}

STDAPI IDirectSoundBuffer_SetMode(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwMode, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetMode(dwMode, dwFlags);
}

STDAPI IDirectSoundBuffer_SetPosition(LPDIRECTSOUNDBUFFER pBuffer, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetPosition(x, y, z, dwFlags);
}

STDAPI IDirectSoundBuffer_SetVelocity(LPDIRECTSOUNDBUFFER pBuffer, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetVelocity(x, y, z, dwFlags);
}

STDAPI IDirectSoundBuffer_SetI3DL2Source(LPDIRECTSOUNDBUFFER pBuffer, LPCDSI3DL2BUFFER pds3db, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetI3DL2Source(pds3db, dwFlags);
}

#endif // MCPX_BOOT_LIB

STDAPI IDirectSoundBuffer_Play(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwReserved1, DWORD dwReserved2, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->Play(dwReserved1, dwReserved2, dwFlags);
}

STDAPI IDirectSoundBuffer_PlayEx(LPDIRECTSOUNDBUFFER pBuffer, REFERENCE_TIME rtTimeStamp, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->PlayEx(rtTimeStamp, dwFlags);
}

STDAPI IDirectSoundBuffer_Stop(LPDIRECTSOUNDBUFFER pBuffer)
{
    return ((CDirectSoundBuffer *)pBuffer)->Stop();
}

STDAPI IDirectSoundBuffer_StopEx(LPDIRECTSOUNDBUFFER pBuffer, REFERENCE_TIME rtTimeStamp, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->StopEx(rtTimeStamp, dwFlags);
}

STDAPI IDirectSoundBuffer_SetBufferOffsets(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwPlayStart, DWORD dwPlayLength, DWORD dwLoopStart, DWORD dwLoopLength)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetBufferOffsets(dwPlayStart, dwPlayLength, dwLoopStart, dwLoopLength);
}

STDAPI IDirectSoundBuffer_SetLoopRegion_v1(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwLoopStart, DWORD dwLoopLength)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetLoopRegion_v1(dwLoopStart, dwLoopLength);
}

STDAPI IDirectSoundBuffer_GetStatus(LPDIRECTSOUNDBUFFER pBuffer, LPDWORD pdwStatus)
{
    return ((CDirectSoundBuffer *)pBuffer)->GetStatus(pdwStatus);
}

STDAPI IDirectSoundBuffer_GetCurrentPosition(LPDIRECTSOUNDBUFFER pBuffer, LPDWORD pdwPlayCursor, LPDWORD pdwWriteCursor)
{
    return ((CDirectSoundBuffer *)pBuffer)->GetCurrentPosition(pdwPlayCursor, pdwWriteCursor);
}

STDAPI IDirectSoundBuffer_SetCurrentPosition(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwPlayCursor)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetCurrentPosition(dwPlayCursor);
}

STDAPI IDirectSoundBuffer_SetBufferData(LPDIRECTSOUNDBUFFER pBuffer, LPVOID pvBufferData, DWORD dwBufferBytes)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetBufferData(pvBufferData, dwBufferBytes);
}

STDAPI IDirectSoundBuffer_Lock(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags)
{
    return ((CDirectSoundBuffer *)pBuffer)->Lock(dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags);
}

STDAPI IDirectSoundBuffer_SetNotificationPositions(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwNotifyCount, LPCDSBPOSITIONNOTIFY paNotifies)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetNotificationPositions(dwNotifyCount, paNotifies);
}

#ifndef MCPX_BOOT_LIB

STDAPI IDirectSoundStream_QueryInterface(LPDIRECTSOUNDSTREAM pStream, REFIID iid, LPVOID *ppvInterface)
{
    ASSERT(pStream);
    ASSERT(ppvInterface);

    IDirectSoundStream_AddRef(pStream);
    *ppvInterface = pStream;

    return DS_OK;
}

STDAPI IDirectSoundStream_QueryInterfaceC(LPDIRECTSOUNDSTREAM pStream, const IID *iid, LPVOID *ppvInterface)
{
    ASSERT(pStream);
    ASSERT(ppvInterface);

    IDirectSoundStream_AddRef(pStream);
    *ppvInterface = pStream;

    return DS_OK;
}

STDAPI IDirectSoundStream_SetFormat(LPDIRECTSOUNDSTREAM pStream, LPCWAVEFORMATEX pwfxFormat)
{
    return ((CDirectSoundStream *)pStream)->SetFormat(pwfxFormat);
}

STDAPI IDirectSoundStream_SetFrequency(LPDIRECTSOUNDSTREAM pStream, DWORD dwFrequency)
{
    return ((CDirectSoundStream *)pStream)->SetFrequency(dwFrequency);
}

STDAPI IDirectSoundStream_SetVolume(LPDIRECTSOUNDSTREAM pStream, LONG lVolume)
{
    return ((CDirectSoundStream *)pStream)->SetVolume(lVolume);
}

STDAPI IDirectSoundStream_SetPitch(LPDIRECTSOUNDSTREAM pStream, LONG lPitch)
{
    return ((CDirectSoundStream *)pStream)->SetPitch(lPitch);
}

STDAPI IDirectSoundStream_SetLFO(LPDIRECTSOUNDSTREAM pStream, LPCDSLFODESC pLFODesc)
{
    return ((CDirectSoundStream *)pStream)->SetLFO(pLFODesc);
}

STDAPI IDirectSoundStream_SetEG(LPDIRECTSOUNDSTREAM pStream, LPCDSENVELOPEDESC pEnvelopeDesc)
{
    return ((CDirectSoundStream *)pStream)->SetEG(pEnvelopeDesc);
}

STDAPI IDirectSoundStream_SetFilter(LPDIRECTSOUNDSTREAM pStream, LPCDSFILTERDESC pFilter)
{
    return ((CDirectSoundStream *)pStream)->SetFilter(pFilter);
}

STDAPI IDirectSoundStream_SetHeadroom(LPDIRECTSOUNDSTREAM pStream, DWORD dwHeadroom)
{
    return ((CDirectSoundStream *)pStream)->SetHeadroom(dwHeadroom);
}

STDAPI IDirectSoundStream_SetOutputBuffer(LPDIRECTSOUNDSTREAM pStream, LPDIRECTSOUNDBUFFER pOutputBuffer)
{
    return ((CDirectSoundStream *)pStream)->SetOutputBuffer(pOutputBuffer);
}

STDAPI IDirectSoundStream_SetMixBins(LPDIRECTSOUNDSTREAM pStream, LPCDSMIXBINS pMixBins)
{
    return ((CDirectSoundStream *)pStream)->SetMixBins(pMixBins);
}

STDAPI IDirectSoundStream_SetMixBinVolumes(LPDIRECTSOUNDSTREAM pStream, LPCDSMIXBINS pMixBins)
{
    return ((CDirectSoundStream *)pStream)->SetMixBinVolumes(pMixBins);
}

STDAPI IDirectSoundStream_SetOutputBuffer_v1(LPDIRECTSOUNDSTREAM pStream, LPDIRECTSOUNDBUFFER pOutputBuffer)
{
    return ((CDirectSoundStream *)pStream)->SetOutputBuffer_v1(pOutputBuffer);
}

STDAPI IDirectSoundStream_SetMixBins_v1(LPDIRECTSOUNDSTREAM pStream, DWORD dwMixBinMask)
{
    return ((CDirectSoundStream *)pStream)->SetMixBins_v1(dwMixBinMask);
}

STDAPI IDirectSoundStream_SetMixBinVolumes_v1(LPDIRECTSOUNDSTREAM pStream, DWORD dwMixBinMask, const LONG *palVolumes)
{
    return ((CDirectSoundStream *)pStream)->SetMixBinVolumes_v1(dwMixBinMask, palVolumes);
}

STDAPI IDirectSoundStream_SetAllParameters(LPDIRECTSOUNDSTREAM pStream, LPCDS3DBUFFER pds3db, DWORD dwFlags)
{
    return ((CDirectSoundStream *)pStream)->SetAllParameters(pds3db, dwFlags);
}

STDAPI IDirectSoundStream_SetConeAngles(LPDIRECTSOUNDSTREAM pStream, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwFlags)
{
    return ((CDirectSoundStream *)pStream)->SetConeAngles(dwInsideConeAngle, dwOutsideConeAngle, dwFlags);
}

STDAPI IDirectSoundStream_SetConeOrientation(LPDIRECTSOUNDSTREAM pStream, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{
    return ((CDirectSoundStream *)pStream)->SetConeOrientation(x, y, z, dwFlags);
}

STDAPI IDirectSoundStream_SetConeOutsideVolume(LPDIRECTSOUNDSTREAM pStream, LONG lConeOutsideVolume, DWORD dwFlags)
{
    return ((CDirectSoundStream *)pStream)->SetConeOutsideVolume(lConeOutsideVolume, dwFlags);
}

STDAPI IDirectSoundStream_SetMaxDistance(LPDIRECTSOUNDSTREAM pStream, FLOAT flMaxDistance, DWORD dwFlags)
{
    return ((CDirectSoundStream *)pStream)->SetMaxDistance(flMaxDistance, dwFlags);
}

STDAPI IDirectSoundStream_SetMinDistance(LPDIRECTSOUNDSTREAM pStream, FLOAT flMinDistance, DWORD dwFlags)
{
    return ((CDirectSoundStream *)pStream)->SetMinDistance(flMinDistance, dwFlags);
}

STDAPI IDirectSoundStream_SetMode(LPDIRECTSOUNDSTREAM pStream, DWORD dwMode, DWORD dwFlags)
{
    return ((CDirectSoundStream *)pStream)->SetMode(dwMode, dwFlags);
}

STDAPI IDirectSoundStream_SetPosition(LPDIRECTSOUNDSTREAM pStream, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{
    return ((CDirectSoundStream *)pStream)->SetPosition(x, y, z, dwFlags);
}

STDAPI IDirectSoundStream_SetVelocity(LPDIRECTSOUNDSTREAM pStream, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{
    return ((CDirectSoundStream *)pStream)->SetVelocity(x, y, z, dwFlags);
}

STDAPI IDirectSoundStream_SetI3DL2Source(LPDIRECTSOUNDSTREAM pStream, LPCDSI3DL2BUFFER pds3db, DWORD dwFlags)
{
    return ((CDirectSoundStream *)pStream)->SetI3DL2Source(pds3db, dwFlags);
}

STDAPI IDirectSoundStream_Pause(LPDIRECTSOUNDSTREAM pStream, DWORD dwPause)
{
    return ((CDirectSoundStream *)pStream)->Pause(dwPause);
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  DirectSoundCreate
 *
 *  Description:
 *      Creates and initializes a DirectSound object.
 *
 *  Arguments:
 *      CDirectSound ** [out]: DirectSound object.  The caller is responbile
 *                             for freeing this object with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundCreateInternal"

HRESULT
DirectSoundCreateInternal
(
    CDirectSound **         ppDirectSound
)
{
    CDirectSound *          pDirectSound    = NULL;
    HRESULT                 hr              = DS_OK;
    
    DPF_ENTER();

    //
    // Check to see if the DirectSound object already exists
    //

    if(CDirectSound::m_pDirectSound)
    {
        *ppDirectSound = ADDREF(CDirectSound::m_pDirectSound);
    }
    else
    {
        hr = HRFROMP(pDirectSound = NEW(CDirectSound));

        if(SUCCEEDED(hr))
        {
            hr = pDirectSound->Initialize();
        }

        if(SUCCEEDED(hr))
        {
            *ppDirectSound = pDirectSound;
        }
        else
        {
            RELEASE(pDirectSound);
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  DirectSoundCreate
 *
 *  Description:
 *      Creates and initializes a DirectSound object.
 *
 *  Arguments:
 *      LPGUID [in]: unused.  Must be NULL.
 *      LPDIRECTSOUND * [out]: DirectSound object.  The caller is responbile
 *                             for freeing this object with Release.
 *      LPUNKNOWN [in]: unused.  Must be NULL.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundCreate"

HRESULT
DirectSoundCreate
(
    LPGUID                  pguidDeviceId,
    LPDIRECTSOUND *         ppDirectSound,
    LPUNKNOWN               pControllingUnknown
)
{
    CDirectSound *          pDirectSound;
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

#ifdef VALIDATE_PARAMETERS

    if(!IS_NULL_GUID(*pguidDeviceId))
    {
        DPF_ERROR("NULL or GUID_NULL is the only supported device identifier");
    }

    if(!ppDirectSound)
    {
        DPF_ERROR("Failed to supply an LPDIRECTSOUND *");
    }
    
    if(pControllingUnknown)
    {
        DPF_ERROR("Aggregation is not supported");
    }

#endif // VALIDATE_PARAMETERS

    hr = DirectSoundCreateInternal(&pDirectSound);

    if(SUCCEEDED(hr))
    {
        *ppDirectSound = pDirectSound;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  DirectSoundCreateBuffer
 *
 *  Description:
 *      Creates and initializes a DirectSound Buffer object.
 *
 *  Arguments:
 *      LPCDSBUFFERDESC [in]: buffer description.
 *      LPDIRECTSOUNDBUFFER * [out]: DirectSound Buffer object.  The caller 
 *                                   is responbile for freeing this object 
 *                                   with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundCreateBuffer"

HRESULT
DirectSoundCreateBuffer
(
    LPCDSBUFFERDESC         pdsbd,
    LPDIRECTSOUNDBUFFER *   ppBuffer
)
{
    CDirectSound *          pDirectSound    = NULL;
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    hr = DirectSoundCreateInternal(&pDirectSound);

    if(SUCCEEDED(hr))
    {
        hr = pDirectSound->CreateSoundBuffer(pdsbd, ppBuffer, NULL);
    }

    RELEASE(pDirectSound);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  DirectSoundCreateBuffer_v1
 *
 *  Description:
 *      Creates and initializes a DirectSound Buffer object.
 *
 *  Arguments:
 *      LPCDSBUFFERDESC [in]: buffer description.
 *      LPDIRECTSOUNDBUFFER * [out]: DirectSound Buffer object.  The caller 
 *                                   is responbile for freeing this object 
 *                                   with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundCreateBuffer_v1"

HRESULT
DirectSoundCreateBuffer_v1
(
    LPCDSBUFFERDESC1        pdsbd,
    LPDIRECTSOUNDBUFFER *   ppBuffer
)
{
    CDirectSound *          pDirectSound    = NULL;
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    hr = DirectSoundCreateInternal(&pDirectSound);

    if(SUCCEEDED(hr))
    {
        hr = pDirectSound->CreateSoundBuffer_v1(pdsbd, ppBuffer, NULL);
    }

    RELEASE(pDirectSound);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  DirectSoundCreateStream
 *
 *  Description:
 *      Creates and initializes a DirectSound Stream object.
 *
 *  Arguments:
 *      LPCDSSTREAMDESC [in]: stream description.
 *      LPDIRECTSOUNDSTREAM * [out]: DirectSound Stream object.  The caller 
 *                                   is responbile for freeing this object 
 *                                   with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundCreateStream"

HRESULT
DirectSoundCreateStream
(
    LPCDSSTREAMDESC         pdssd,
    LPDIRECTSOUNDSTREAM *   ppStream
)
{
    CDirectSound *          pDirectSound    = NULL;
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    hr = DirectSoundCreateInternal(&pDirectSound);

    if(SUCCEEDED(hr))
    {
        hr = pDirectSound->CreateSoundStream(pdssd, ppStream, NULL);
    }

    RELEASE(pDirectSound);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  DirectSoundCreateStream_v1
 *
 *  Description:
 *      Creates and initializes a DirectSound Stream object.
 *
 *  Arguments:
 *      LPCDSSTREAMDESC [in]: stream description.
 *      LPDIRECTSOUNDSTREAM * [out]: DirectSound Stream object.  The caller 
 *                                   is responbile for freeing this object 
 *                                   with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundCreateStream_v1"

HRESULT
DirectSoundCreateStream_v1
(
    LPCDSSTREAMDESC1        pdssd,
    LPDIRECTSOUNDSTREAM *   ppStream
)
{
    CDirectSound *          pDirectSound    = NULL;
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    hr = DirectSoundCreateInternal(&pDirectSound);

    if(SUCCEEDED(hr))
    {
        hr = pDirectSound->CreateSoundStream_v1(pdssd, ppStream, NULL);
    }

    RELEASE(pDirectSound);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  DirectSoundDoWork
 *
 *  Description:
 *      Does some work.  Leave me alone.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundDoWork"

void
DirectSoundDoWork
(
    void
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

#ifdef DEBUG

    //
    // Check for a deferred breakpoint
    //

    if(INTERLOCKED_EXCHANGE(g_fDirectSoundDebugBreak, FALSE))
    {
        DPF_ABSOLUTE("A breakpoint occurred at raised IRQL.  If you're running the Visual C debugger, you probably missed it.");
        BREAK();
    }

#endif // DEBUG

    //
    // Workie, workie
    //

    if(CDirectSound::m_pDirectSound)
    {
        CDirectSound::m_pDirectSound->DoWork();
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  DirectSoundGetSampleTime
 *
 *  Description:
 *      Gets the current sample clock time.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: sample time.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundGetSampleTime"

DWORD
DirectSoundGetSampleTime
(
    void
)
{
    DWORD                   dwSampleTime;
    
    DPF_ENTER();    

    //
    // MASSIVE HACK
    //
    // In order to keep latency and overhead to an absolute minimum, we're 
    // going to read the MCPX sample clock register from here.  This will 
    // prevent us from having to take the critical section or raise IRQL.
    //

    MCPX_REG_READ(NV_PAPU_XGSCNT, &dwSampleTime);

    DPF_LEAVE(dwSampleTime);

    return dwSampleTime;
}


#ifndef MCPX_BOOT_LIB

/****************************************************************************
 *
 *  DirectSoundUseFullHRTF
 *
 *  Description:
 *      Sets up the HRTF 3D filter loader to use the full HRTF table.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundUseFullHRTF"

void
DirectSoundUseFullHRTF
(
    void
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    CHrtfSource::SetFullHrtfQuality();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  DirectSoundUseLightHRTF
 *
 *  Description:
 *      Sets up the HRTF 3D filter loader to use the light HRTF table.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundUseLightHRTF"

void
DirectSoundUseLightHRTF
(
    void
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    CHrtfSource::SetLightHrtfQuality();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  DirectSoundOverrideSpeakerConfig
 *
 *  Description:
 *      Allows a game to override the default speaker configuration.
 *
 *  Arguments:
 *      DWORD [in]: speaker configuration.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundOverrideSpeakerConfig"

void
DirectSoundOverrideSpeakerConfig
(
    DWORD                   dwSpeakerConfig
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

#ifdef VALIDATE_PARAMETERS

    if(!DSSPEAKER_IS_VALID(dwSpeakerConfig))
    {
        DPF_ERROR("Invalid speaker configuration");
    }

    if(!DSSPEAKER_IS_VALID_BASIC(dwSpeakerConfig))
    {
        DPF_ERROR("Can't set encoder bits in the speaker config");
    }

    if(CDirectSound::m_pDirectSound)
    {
        DPF_ERROR("Can't call " DPF_FNAME " after the DirectSound object has been created");
    }

#endif // VALIDATE_PARAMETERS

    g_dwDirectSoundOverrideSpeakerConfig = DSSPEAKER_BASIC(dwSpeakerConfig);

    DPF_LEAVE_VOID();
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  XAudioCalculatePitch
 *
 *  Description:
 *      Converts frequency to an MCPX pitch value.
 *
 *  Arguments:
 *      DWORD [in]: frequency, in Hz.
 *
 *  Returns:  
 *      LONG: pitch.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "XAudioCalculatePitch"

LONG
XAudioCalculatePitch
(
    DWORD                   dwFrequency
)
{
    LONG                    lPitch;
   
    DPF_ENTER();
    AutoFpState();

#ifdef VALIDATE_PARAMETERS

    if((dwFrequency < DSBFREQUENCY_MIN) || (dwFrequency > DSBFREQUENCY_MAX))
    {
        DPF_ERROR("Invalid frequency value");
    }

#endif // VALIDATE_PARAMETERS

    if(MCPX_BASEFREQ == dwFrequency)
    {
        lPitch = 0;
    }
    else
    {
        lPitch = RatioToPitch((FLOAT)dwFrequency * MCPX_ONEOVERBASEFREQ);
    }

    DPF_LEAVE(lPitch);

    return lPitch;
}


/****************************************************************************
 *
 *  XAudioDownloadEffectsImage
 *
 *  Description:
 *      Downloads an effects image to the DSP.
 *
 *  Arguments:
 *      LPCSTR [in]: file or section name.
 *      LPCDSEFFECTIMAGELOC [in]: effect locations.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "XAudioDownloadEffectsImage"

STDAPI
XAudioDownloadEffectsImage
(
    LPCSTR                  pszImageName,
    LPCDSEFFECTIMAGELOC     pEffectLoc,
    DWORD                   dwFlags,
    LPDSEFFECTIMAGEDESC *   ppImageDesc
)
{
    HANDLE                  hImageFile      = NULL;
    LPVOID                  pvImageData     = NULL;
    HRESULT                 hr              = DS_OK;
    DWORD                   dwImageSize;

    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

#ifdef VALIDATE_PARAMETERS

    if(!pszImageName)
    {
        DPF_ERROR("No image name supplied");
    }

    if(dwFlags & ~XAUDIO_DOWNLOADFX_VALID)
    {
        DPF_ERROR("Invalid flags");
    }

#endif // VALIDATE_PARAMETERS

    if(!CDirectSound::m_pDirectSound)
    {
        DPF_ERROR("The DirectSound object must have already been created before calling " DPF_FNAME);
        hr = DSERR_INVALIDCALL;
    }

    //
    // Open the image
    //

    if(SUCCEEDED(hr))
    {
        if(dwFlags & XAUDIO_DOWNLOADFX_XBESECTION)
        {
            if(INVALID_HANDLE_VALUE == (hImageFile = XGetSectionHandle(pszImageName)))
            {
                DPF_ERROR("Can't find image section");
                hr = HRESULT_FROM_WIN32(GetLastError());
            }

            if(SUCCEEDED(hr))
            {
                if(!(dwImageSize = XGetSectionSize(hImageFile)))
                {
                    DPF_ERROR("Can't get image size");
                    hr = HRESULT_FROM_WIN32(GetLastError());
                }
            }

            if(SUCCEEDED(hr))
            {
                if(!(pvImageData = XLoadSectionByHandle(hImageFile)))
                {
                    DPF_ERROR("Can't read image section");
                    hr = HRESULT_FROM_WIN32(GetLastError());
                }
            }
        }
        else
        {
            if(INVALID_HANDLE_VALUE == (hImageFile = CreateFile(pszImageName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
            {
                DPF_ERROR("Can't find image file");
                hr = HRESULT_FROM_WIN32(GetLastError());
            }

            if(SUCCEEDED(hr))
            {
                if(!(dwImageSize = GetFileSize(hImageFile, NULL)))
                {
                    DPF_ERROR("Can't get image size");
                    hr = HRESULT_FROM_WIN32(GetLastError());
                }
            }

            if(SUCCEEDED(hr))
            {
                hr = HRFROMP(pvImageData = MEMALLOC_NOINIT(BYTE, dwImageSize));
            }

            if(SUCCEEDED(hr))
            {
                if(!ReadFile(hImageFile, pvImageData, dwImageSize, &dwImageSize, NULL))
                {
                    DPF_ERROR("Can't read image file");
                    hr = HRESULT_FROM_WIN32(GetLastError());
                }
            }
        }
    }

    //
    // Download the image
    //
    
    if(SUCCEEDED(hr))
    {
        hr = CDirectSound::m_pDirectSound->DownloadEffectsImage(pvImageData, dwImageSize, pEffectLoc, ppImageDesc);
    }

    //
    // Clean up
    //

    if(dwFlags & XAUDIO_DOWNLOADFX_XBESECTION)
    {
        if(IS_VALID_HANDLE_VALUE(hImageFile))
        {
            XFreeSectionByHandle(hImageFile);
        }
    }
    else
    {
        MEMFREE(pvImageData);
        CLOSE_HANDLE(hImageFile);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#endif // MCPX_BOOT_LIB

/****************************************************************************
 *
 *  CDirectSoundSettings
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundSettings::CDirectSoundSettings"

CDirectSoundSettings::CDirectSoundSettings
(
    void
)
{
    DWORD                   i;

    DPF_ENTER();

#ifndef MCPX_BOOT_LIB

    m_3dParams = DirectSoundDefault3DListener;
    m_I3dl2Params = DirectSoundI3DL2ListenerPreset_Default;

#endif // MCPX_BOOT_LIB

    //
    // Read the speaker configuration from the EEPROM
    //

#ifndef MCPX_BOOT_LIB

    if(DSSPEAKER_IS_VALID(g_dwDirectSoundOverrideSpeakerConfig))
    {
        m_dwSpeakerConfig = g_dwDirectSoundOverrideSpeakerConfig;
    }
    else
    {
        m_dwSpeakerConfig = XAudioGetSpeakerConfig();
    }

#else // MCPX_BOOT_LIB

    m_dwSpeakerConfig = DSSPEAKER_STEREO;

#endif // MCPX_BOOT_LIB

    //
    // Set default mixbin headroom
    //

    for(i = 0; i < DSMIXBIN_SUBMIX; i++)
    {
        m_adwMixBinHeadroom[i] = DSHEADROOM_DEFAULT;
    }

    m_adwMixBinHeadroom[DSMIXBIN_SUBMIX] = 0;

    //
    // Set default effect locations
    //

    SetEffectImageLocations(NULL);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CDirectSoundSettings
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundSettings::~CDirectSoundSettings"

CDirectSoundSettings::~CDirectSoundSettings
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetEffectImageLocations
 *
 *  Description:
 *      Sets effect indeces.
 *
 *  Arguments:
 *      LPCDSEFFECTIMAGELOC [in]: effect locations.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundSettings::SetEffectImageLocations"

void
CDirectSoundSettings::SetEffectImageLocations
(
    LPCDSEFFECTIMAGELOC     pImageLoc
)
{
    DPF_ENTER();

    if(pImageLoc)
    {
        CopyMemory(&m_EffectLocations, pImageLoc, sizeof(*pImageLoc));
    }
    else
    {
        memset(&m_EffectLocations, 0xFF, sizeof(m_EffectLocations));
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CDirectSound
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::CDirectSound"

CDirectSound *CDirectSound::m_pDirectSound;

CDirectSound::CDirectSound
(
    void
)

#ifdef USE_KEEPALIVE_BUFFERS

:   CRefCount(2)

#endif // USE_KEEPALIVE_BUFFER

{
    DPF_ENTER();

    //
    // Set the global DirectSound object pointer
    //

    m_pDirectSound = this;

    //
    // Initialize defaults
    //

    InitializeListHead(&m_lst3dVoices);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CDirectSound
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::~CDirectSound"

CDirectSound::~CDirectSound
(
    void
)
{
    DPF_ENTER();

    //
    // Release performance counters
    //

    CDirectSoundPerformanceMonitor::UnregisterCounters();

    //
    // Release the global DirectSound object pointer
    //

    m_pDirectSound = NULL;

    //
    // Free owned objects
    //

    RELEASE(m_pDevice);
    RELEASE(m_pSettings);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Release
 *
 *  Description:
 *      Decrements the object reference count.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: reference count.
 *
 ****************************************************************************/

#ifdef USE_KEEPALIVE_BUFFERS

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::Release"

DWORD
CDirectSound::Release
(
    void
)
{
    DWORD                   dwRefCount;
    DWORD                   i;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    if((dwRefCount = CRefCount::Release()) == NUMELMS(m_apKeepAliveBuffers) + 1)
    {
        //
        // The only references we have left are the keepalive buffers and the
        // artificial one we added at creation.  Go ahead and shut down.
        //

        for(i = 0; i < NUMELMS(m_apKeepAliveBuffers); i++)
        {
            ASSERT(m_apKeepAliveBuffers[i]);
            RELEASE(m_apKeepAliveBuffers[i]);
        }

        dwRefCount = CRefCount::Release();
        ASSERT(!dwRefCount);
    }

    DPF_LEAVE(dwRefCount);

    return dwRefCount;
}

#endif // USE_KEEPALIVE_BUFFERS


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::Initialize"

HRESULT
CDirectSound::Initialize
(
    void
)
{

#ifdef USE_KEEPALIVE_BUFFERS

    static const WAVEFORMATEX   wfxSilence              = INIT_PCM_WAVEFORMAT(1, 48000, 16);
    static const SHORT          asSilence[32]           = { 0 };
    DSBUFFERDESC                dsbd;
    DSMIXBINVOLUMEPAIR          MixBinVolumePairs[8];
    DSMIXBINS                   MixBins;
    DWORD                       i, z;

#endif // USE_KEEPALIVE_BUFFERS

    HRESULT                     hr;

    DPF_ENTER();

    //
    // Create the shared settings object
    //

    hr = HRFROMP(m_pSettings = NEW(CDirectSoundSettings));

    //
    // Create the device object
    //

    if(SUCCEEDED(hr))
    {
        hr = HRFROMP(m_pDevice = NEW(CMcpxAPU(m_pSettings)));
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pDevice->Initialize();
    }

#ifdef USE_KEEPALIVE_BUFFERS

    //
    // Create dummy buffers.  The MCPX refuses to output any data at least 
    // one voice writes to the speaker mixbins.  We'll burn voices in order 
    // to write silence to all the mixbins all the time.
    //

    if(SUCCEEDED(hr))
    {
        ZeroMemory(&dsbd, sizeof(dsbd));
        ZeroMemory(&MixBinVolumePairs, sizeof(MixBinVolumePairs));

        dsbd.dwSize = sizeof(dsbd);
        dsbd.lpwfxFormat = (LPWAVEFORMATEX)&wfxSilence;
        dsbd.lpMixBins = &MixBins;

        MixBins.dwMixBinCount = NUMELMS(MixBinVolumePairs);
        MixBins.lpMixBinVolumePairs = MixBinVolumePairs;

        for(i = 0; SUCCEEDED(hr) && (i < NUMELMS(m_apKeepAliveBuffers)); i++)
        {
            for(z = 0; z < NUMELMS(MixBinVolumePairs); z++)
            {
                MixBinVolumePairs[z].dwMixBin = (i * NUMELMS(MixBinVolumePairs)) + z;
            }

            if(DSMIXBIN_SUBMIX == MixBinVolumePairs[NUMELMS(MixBinVolumePairs) - 1].dwMixBin)
            {
                MixBins.dwMixBinCount--;
            }

            hr = CreateSoundBuffer(&dsbd, &m_apKeepAliveBuffers[i], NULL);

            if(SUCCEEDED(hr))
            {
                hr = m_apKeepAliveBuffers[i]->SetBufferData((LPVOID)asSilence, sizeof(asSilence));
            }

            if(SUCCEEDED(hr))
            {
                hr = m_apKeepAliveBuffers[i]->Play(0, 0, DSBPLAY_LOOPING);
            }
        }
    }

#endif // USE_KEEPALIVE_BUFFERS

    //
    // Set up performance counters
    //

    if(SUCCEEDED(hr))
    {
        CDirectSoundPerformanceMonitor::RegisterCounters();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetCaps
 *
 *  Description:
 *      Gets the capabilities of the object.
 *
 *  Arguments:
 *      LPDSCAPS [in/out]: capabilities.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::GetCaps"

HRESULT
CDirectSound::GetCaps
(
    LPDSCAPS                pdsc
)
{
    HRESULT                 hr;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pdsc)
    {
        DPF_ERROR("DSCAPS not supplied");
    }

#endif // VALIDATE_PARAMETERS
    
    hr = m_pDevice->GetCaps(&pdsc->dwFree2DBuffers, &pdsc->dwFree3DBuffers, &pdsc->dwFreeBufferSGEs);

    if(SUCCEEDED(hr))
    {
        pdsc->dwMemoryAllocated = g_dwDirectSoundPoolMemoryUsage + g_dwDirectSoundPhysicalMemoryUsage;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  CreateSoundBuffer
 *
 *  Description:
 *      Creates a sound buffer object.
 *
 *  Arguments:
 *      LPCDSBUFFERDESC [in]: buffer description.
 *      LPDIRECTSOUNDBUFFER * [out]: buffer object.  The caller is expected
 *                                   to free this object with Release.
 *      LPUNKNOWN [in]: controlling unknown.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::CreateSoundBuffer"

HRESULT
CDirectSound::CreateSoundBuffer
(
    LPCDSBUFFERDESC         pdsbd,
    LPDIRECTSOUNDBUFFER *   ppBuffer,
    LPUNKNOWN               pControllingUnknown
)
{
    CDirectSoundBuffer *    pBuffer;
    HRESULT                 hr;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!ppBuffer)
    {
        DPF_ERROR("Failed to supply an LPDIRECTSOUNDBUFFER *");
    }
    
    if(pControllingUnknown)
    {
        DPF_ERROR("Aggregation is not supported");
    }

#endif // VALIDATE_PARAMETERS

    hr = HRFROMP(pBuffer = NEW(CDirectSoundBuffer(this)));

    if(SUCCEEDED(hr))
    {
        hr = pBuffer->Initialize(pdsbd);
    }

    if(SUCCEEDED(hr))
    {
        *ppBuffer = pBuffer;
    }
    else
    {
        RELEASE(pBuffer);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  CreateSoundBuffer_v1
 *
 *  Description:
 *      Creates a sound buffer object.
 *
 *  Arguments:
 *      LPCDSBUFFERDESC [in]: buffer description.
 *      LPDIRECTSOUNDBUFFER * [out]: buffer object.  The caller is expected
 *                                   to free this object with Release.
 *      LPUNKNOWN [in]: controlling unknown.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::CreateSoundBuffer_v1"

HRESULT
CDirectSound::CreateSoundBuffer_v1
(
    LPCDSBUFFERDESC1        pdsbd,
    LPDIRECTSOUNDBUFFER *   ppBuffer,
    LPUNKNOWN               pControllingUnknown
)
{
    DSMIXBINS               MixBins;
    DSMIXBINVOLUMEPAIR      MixBinVolumePairs[DSMIXBIN_ASSIGNMENT_MAX];
    DSBUFFERDESC            dsbd;
    DWORD                   dwMixBinMask;
    HRESULT                 hr;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = pdsbd->dwFlags;
    dsbd.dwBufferBytes = pdsbd->dwBufferBytes;
    dsbd.lpwfxFormat = pdsbd->lpwfxFormat;
    
    if(dwMixBinMask = pdsbd->dwMixBinMask)
    {
        dsbd.lpMixBins = &MixBins;
        
        MixBins.dwMixBinCount = 0;
        MixBins.lpMixBinVolumePairs = MixBinVolumePairs;

        while(dwMixBinMask)
        {
            if(MixBins.dwMixBinCount >= NUMELMS(MixBinVolumePairs))
            {
                DPF_ERROR("Too many mixbins in the mask");
                break;
            }
            
            MixBinVolumePairs[MixBins.dwMixBinCount].dwMixBin = lsb(dwMixBinMask);
            MixBinVolumePairs[MixBins.dwMixBinCount].lVolume = 0;

            dwMixBinMask &= ~(1UL << MixBinVolumePairs[MixBins.dwMixBinCount].dwMixBin);

            MixBins.dwMixBinCount++;
        }
    }
    else
    {
        dsbd.lpMixBins = NULL;
    }

    if(pdsbd->dwInputMixBinMask)
    {
        dsbd.dwInputMixBin = lsb(pdsbd->dwInputMixBinMask);
    }
    else
    {
        dsbd.dwInputMixBin = 0;
    }

    hr = CreateSoundBuffer(&dsbd, ppBuffer, pControllingUnknown);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  CreateSoundStream
 *
 *  Description:
 *      Creates a sound buffer object.
 *
 *  Arguments:
 *      LPCDSSTREAMDESC [in]: buffer description.
 *      LPDIRECTSOUNDSTREAM * [out]: buffer object.  The caller is expected
 *                                   to free this object with Release.
 *      LPUNKNOWN [in]: controlling unknown.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::CreateSoundStream"

HRESULT
CDirectSound::CreateSoundStream
(
    LPCDSSTREAMDESC         pdssd,
    LPDIRECTSOUNDSTREAM *   ppStream,
    LPUNKNOWN               pControllingUnknown
)
{
    CDirectSoundStream *    pStream;
    HRESULT                 hr;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!ppStream)
    {
        DPF_ERROR("Failed to supply an LPDIRECTSOUNDSTREAM *");
    }
    
    if(pControllingUnknown)
    {
        DPF_ERROR("Aggregation is not supported");
    }

#endif // VALIDATE_PARAMETERS

    hr = HRFROMP(pStream = NEW(CDirectSoundStream(this)));

    if(SUCCEEDED(hr))
    {
        hr = pStream->Initialize(pdssd);
    }

    if(SUCCEEDED(hr))
    {
        *ppStream = pStream;
    }
    else
    {
        RELEASE(pStream);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  CreateSoundStream_v1
 *
 *  Description:
 *      Creates a sound buffer object.
 *
 *  Arguments:
 *      LPCDSSTREAMDESC [in]: buffer description.
 *      LPDIRECTSOUNDSTREAM * [out]: buffer object.  The caller is expected
 *                                   to free this object with Release.
 *      LPUNKNOWN [in]: controlling unknown.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::CreateSoundStream_v1"

HRESULT
CDirectSound::CreateSoundStream_v1
(
    LPCDSSTREAMDESC1        pdssd,
    LPDIRECTSOUNDSTREAM *   ppStream,
    LPUNKNOWN               pControllingUnknown
)
{
    DSMIXBINS               MixBins;
    DSMIXBINVOLUMEPAIR      MixBinVolumePairs[DSMIXBIN_ASSIGNMENT_MAX];
    DSSTREAMDESC            dssd;
    DWORD                   dwMixBinMask;
    HRESULT                 hr;
    DWORD                   i;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    dssd.dwFlags = pdssd->dwFlags;
    dssd.dwMaxAttachedPackets = pdssd->dwMaxAttachedPackets;
    dssd.lpwfxFormat = pdssd->lpwfxFormat;
    dssd.lpfnCallback = pdssd->lpfnCallback;
    dssd.lpvContext = pdssd->lpvContext;

    if(dwMixBinMask = pdssd->dwMixBinMask)
    {
        dssd.lpMixBins = &MixBins;
        
        MixBins.dwMixBinCount = 0;
        MixBins.lpMixBinVolumePairs = MixBinVolumePairs;

        while(dwMixBinMask)
        {
            if(MixBins.dwMixBinCount >= NUMELMS(MixBinVolumePairs))
            {
                DPF_ERROR("Too many mixbins in the mask");
                break;
            }

            MixBinVolumePairs[MixBins.dwMixBinCount].dwMixBin = lsb(dwMixBinMask);
            MixBinVolumePairs[MixBins.dwMixBinCount].lVolume = 0;

            dwMixBinMask &= ~(1UL << MixBinVolumePairs[MixBins.dwMixBinCount].dwMixBin);

            MixBins.dwMixBinCount++;
        }
    }
    else
    {
        dssd.lpMixBins = NULL;
    }

    hr = CreateSoundStream(&dssd, ppStream, pControllingUnknown);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  GetSpeakerConfig
 *
 *  Description:
 *      Gets the current speaker config.
 *
 *  Arguments:
 *      LPDWORD [out]: speaker config.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::GetSpeakerConfig"

HRESULT
CDirectSound::GetSpeakerConfig
(
    LPDWORD                 pdwSpeakerConfig
)
{
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!pdwSpeakerConfig)
    {
        DPF_ERROR("Failed to supply speaker config buffer");
    }

#endif // VALIDATE_PARAMETERS

    *pdwSpeakerConfig = m_pSettings->m_dwSpeakerConfig & ~DSSPEAKER_ENABLE_HEADPHONES;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  DownloadEffectsImage
 *
 *  Description:
 *      Downloads an effects image to the effects processor.
 *
 *  Arguments:
 *      LPCVOID [in]: image data.
 *      DWORD [in]: image data size, in bytes.
 *      LPCDSEFFECTIMAGELOC [in]: effect locations.
 *      LPVOID * [out]: image scratch data.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::DownloadEffectsImage"

HRESULT
CDirectSound::DownloadEffectsImage
(
    LPCVOID                 pvImageBuffer, 
    DWORD                   dwImageSize,
    LPCDSEFFECTIMAGELOC     pImageLoc, 
    LPDSEFFECTIMAGEDESC *   ppImageDesc
)
{
    HRESULT                 hr;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pvImageBuffer)
    {
        DPF_ERROR("Failed to supply an image buffer");
    }

    if(dwImageSize <= (DSP_COMMANDBLOCK_SCRATCHOFFSET + sizeof(HOST_TO_DSP_COMMANDBLOCK)))
    {
        DPF_ERROR("Invalid image size");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Save the effect locations
    //

    m_pSettings->SetEffectImageLocations(pImageLoc);

    //
    // Download the image
    //

    hr = m_pDevice->DownloadEffectsImage(pvImageBuffer, dwImageSize, ppImageDesc);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  SetEffectData
 *
 *  Description:
 *      Updates part or all of an effect's data.
 *
 *  Arguments:
 *      DWORD [in]: absolute effect index.
 *      DWORD [in]: offset, in bytes, of the start of the update region.
 *      LPCVOID [in]: data buffer.
 *      DWORD [in]: data buffer size, in bytes.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetEffectData"

HRESULT
CDirectSound::SetEffectData
(
    DWORD                   dwEffectIndex, 
    DWORD                   dwOffset, 
    LPCVOID                 pvData, 
    DWORD                   dwDataSize,
    DWORD                   dwFlags
)
{
    HRESULT                 hr;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(dwOffset & 3)
    {
        DPF_ERROR("Offset must be DWORD-aligned");
    }

    if(!pvData)
    {
        DPF_ERROR("Failed to supply a data buffer");
    }

    if(!dwDataSize)
    {
        DPF_ERROR("Invalid data size");
    }

    if(dwDataSize & 3)
    {
        DPF_ERROR("Data size must be DWORD-aligned");
    }

    if(dwFlags & ~DSFX_VALID)
    {
        DPF_ERROR("Invalid flags");
    }

#endif // VALIDATE_PARAMETERS

    hr = m_pDevice->SetEffectData(dwEffectIndex, dwOffset, pvData, dwDataSize, dwFlags);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetEffectData
 *
 *  Description:
 *      Retrieves part or all of an effect's data.
 *
 *  Arguments:
 *      DWORD [in]: absolute effect index.
 *      DWORD [in]: offset, in bytes, of the start of the update region.
 *      LPVOID [out]: data buffer.
 *      DWORD [in]: data buffer size, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::GetEffectData"

HRESULT
CDirectSound::GetEffectData
(
    DWORD                   dwEffectIndex, 
    DWORD                   dwOffset, 
    LPVOID                  pvData, 
    DWORD                   dwDataSize
)
{
    HRESULT                 hr;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(dwOffset & 3)
    {
        DPF_ERROR("Offset must be DWORD-aligned");
    }

    if(!pvData)
    {
        DPF_ERROR("Failed to supply a data buffer");
    }

    if(!dwDataSize)
    {
        DPF_ERROR("Invalid data size");
    }

    if(dwDataSize & 3)
    {
        DPF_ERROR("Data size must be DWORD-aligned");
    }

#endif // VALIDATE_PARAMETERS

    hr = m_pDevice->GetEffectData(dwEffectIndex, dwOffset, pvData, dwDataSize);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  CommitEffectData
 *
 *  Description:
 *      Commits deferred effects settings.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::CommitEffectData"

HRESULT 
CDirectSound::CommitEffectData
(
    void
)
{
    HRESULT                 hr;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    hr = m_pDevice->CommitEffectData();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetMixBinHeadroom
 *
 *  Description:
 *      Sets the headroom for a mixbin.
 *
 *  Arguments:
 *      DWORD [in]: mixbin identifier.
 *      DWORD [in]: headroom amount.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetMixBinHeadroom"

HRESULT
CDirectSound::SetMixBinHeadroom
(
    DWORD                   dwMixBin,
    DWORD                   dwHeadroom
)
{
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(dwMixBin >= DSMIXBIN_COUNT)
    {
        DPF_ERROR("Invalid mixbin");
    }

    if((dwHeadroom < DSHEADROOM_MIN) || (dwHeadroom > DSHEADROOM_MAX))
    {
        DPF_ERROR("Invalid headroom value");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Update the settings object
    //

    m_pSettings->m_adwMixBinHeadroom[dwMixBin] = dwHeadroom;
    
    //
    // Update the implementation object
    //

    hr = m_pDevice->SetMixBinHeadroom(dwMixBin);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetMixBinHeadroom_v1
 *
 *  Description:
 *      Sets the headroom for one or more mixbins.
 *
 *  Arguments:
 *      DWORD [in]: mixbin mask.
 *      DWORD [in]: headroom amount.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetMixBinHeadroom_v1"

HRESULT
CDirectSound::SetMixBinHeadroom_v1
(
    DWORD                   dwMixBinMask,
    DWORD                   dwHeadroom
)
{
    HRESULT                 hr          = DS_OK;
    DWORD                   dwMixBin;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    while(dwMixBinMask && SUCCEEDED(hr))
    {
        dwMixBin = lsb(dwMixBinMask);
        dwMixBinMask &= ~(1UL << dwMixBin);

        hr = SetMixBinHeadroom(dwMixBin, dwHeadroom);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  SetAllParameters
 *
 *  Description:
 *      Sets all listener properties.
 *
 *  Arguments:
 *      LPDS3DLISTENER [in]: properties.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetAllParameters"

HRESULT 
CDirectSound::SetAllParameters
(
    LPCDS3DLISTENER         pParams, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pParams)
    {
        DPF_ERROR("Buffer not supplied");
    }

    if(pParams->dwSize < sizeof(*pParams))
    {
        DPF_ERROR("Structure size too small");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if((pParams->flDistanceFactor < DS3D_MINDISTANCEFACTOR) || (pParams->flDistanceFactor > DS3D_MAXDISTANCEFACTOR))
    {
        DPF_ERROR("Distance factor out of bounds");
    }

    if((pParams->flDopplerFactor < DS3D_MINDOPPLERFACTOR) || (pParams->flDopplerFactor > DS3D_MAXDOPPLERFACTOR))
    {
        DPF_ERROR("Doppler factor out of bounds");
    }

    if((pParams->flRolloffFactor < DS3D_MINROLLOFFFACTOR) || (pParams->flRolloffFactor > DS3D_MAXROLLOFFFACTOR))
    {
        DPF_ERROR("Rolloff factor out of bounds");
    }

    if((!pParams->vOrientFront.x && !pParams->vOrientFront.y && !pParams->vOrientFront.z) || (!pParams->vOrientTop.x && !pParams->vOrientTop.y && !pParams->vOrientTop.z))
    {
        DPF_ERROR("Invalid orientation");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams = *pParams;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_LISTENER_MASK;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetDistanceFactor
 *
 *  Description:
 *      Sets the world's distance factor.
 *
 *  Arguments:
 *      FLOAT [in]: distance factor.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetDistanceFactor"

HRESULT 
CDirectSound::SetDistanceFactor
(
    FLOAT                   flDistanceFactor, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if((flDistanceFactor < DS3D_MINDISTANCEFACTOR) || (flDistanceFactor > DS3D_MAXDISTANCEFACTOR))
    {
        DPF_ERROR("Distance factor out of bounds");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.flDistanceFactor = flDistanceFactor;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_LISTENER_DISTANCEFACTOR;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetDopplerFactor
 *
 *  Description:
 *      Sets the world's Doppler factor.
 *
 *  Arguments:
 *      FLOAT [in]: Doppler factor.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetDopplerFactor"

HRESULT 
CDirectSound::SetDopplerFactor
(
    FLOAT                   flDopplerFactor, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if((flDopplerFactor < DS3D_MINDOPPLERFACTOR) || (flDopplerFactor > DS3D_MAXDOPPLERFACTOR))
    {
        DPF_ERROR("Doppler factor out of bounds");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.flDopplerFactor = flDopplerFactor;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_LISTENER_DOPPLERFACTOR;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetRolloffFactor
 *
 *  Description:
 *      Sets the world's rolloff factor.
 *
 *  Arguments:
 *      FLOAT [in]: rolloff factor.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetRolloffFactor"

HRESULT 
CDirectSound::SetRolloffFactor
(
    FLOAT                   flRolloffFactor, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if((flRolloffFactor < DS3D_MINROLLOFFFACTOR) || (flRolloffFactor > DS3D_MAXROLLOFFFACTOR))
    {
        DPF_ERROR("Rolloff factor out of bounds");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.flRolloffFactor = flRolloffFactor;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_LISTENER_ROLLOFFFACTOR;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetOrientation
 *
 *  Description:
 *      Sets the listener's orientation.
 *
 *  Arguments:
 *      REFD3DXVECTOR3 [in]: front orientation.
 *      REFD3DXVECTOR3 [in]: top orientation.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetOrientation"

HRESULT 
CDirectSound::SetOrientation
(
    FLOAT                   flFrontX, 
    FLOAT                   flFrontY, 
    FLOAT                   flFrontZ, 
    FLOAT                   flTopX, 
    FLOAT                   flTopY, 
    FLOAT                   flTopZ, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if((!flFrontX && !flFrontY && !flFrontZ) || (!flTopX && !flTopY && !flTopZ))
    {
        DPF_ERROR("Invalid orientation");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.vOrientFront.x = flFrontX;
    m_pSettings->m_3dParams.vOrientFront.y = flFrontY;
    m_pSettings->m_3dParams.vOrientFront.z = flFrontZ;
    m_pSettings->m_3dParams.vOrientTop.x = flTopX;
    m_pSettings->m_3dParams.vOrientTop.y = flTopY;
    m_pSettings->m_3dParams.vOrientTop.z = flTopZ;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_LISTENER_ORIENTATION;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetPosition
 *
 *  Description:
 *      Sets the listener's position.
 *
 *  Arguments:
 *      REFD3DXVECTOR3 [in]: position.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetPosition"

HRESULT 
CDirectSound::SetPosition
(
    FLOAT                   flPositionX,
    FLOAT                   flPositionY,
    FLOAT                   flPositionZ,
    DWORD                   dwFlags
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.vPosition.x = flPositionX;
    m_pSettings->m_3dParams.vPosition.y = flPositionY;
    m_pSettings->m_3dParams.vPosition.z = flPositionZ;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_LISTENER_POSITION;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetVelocity
 *
 *  Description:
 *      Sets the listener's velocity.
 *
 *  Arguments:
 *      REFD3DXVECTOR3 [in]: velocity.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetVelocity"

HRESULT 
CDirectSound::SetVelocity
(
    FLOAT                   flVelocityX,
    FLOAT                   flVelocityY,
    FLOAT                   flVelocityZ,
    DWORD                   dwFlags
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.vVelocity.x = flVelocityX;
    m_pSettings->m_3dParams.vVelocity.y = flVelocityY;
    m_pSettings->m_3dParams.vVelocity.z = flVelocityZ;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_LISTENER_VELOCITY;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  SetI3DL2Listener
 *
 *  Description:
 *      Sets I3DL2 listener properties.
 *
 *  Arguments:
 *      LPCDSI3DL2LISTENER [in]: listener properties.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::SetI3DL2Listener"

HRESULT
CDirectSound::SetI3DL2Listener
(
    LPCDSI3DL2LISTENER      pProperties,
    DWORD                   dwFlags
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();    
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pProperties)
    {
        DPF_ERROR("Failed to supply properties");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if((pProperties->lRoom < DSI3DL2LISTENER_MINROOM) || (pProperties->lRoom > DSI3DL2LISTENER_MAXROOM))
    {
        DPF_ERROR("Invalid lRoom value");
    }

    if((pProperties->lRoomHF < DSI3DL2LISTENER_MINROOMHF) || (pProperties->lRoomHF > DSI3DL2LISTENER_MAXROOMHF))
    {
        DPF_ERROR("Invalid lRoomHF value");
    }

    if((pProperties->flRoomRolloffFactor < DSI3DL2LISTENER_MINROOMROLLOFFFACTOR) || (pProperties->flRoomRolloffFactor > DSI3DL2LISTENER_MAXROOMROLLOFFFACTOR))
    {
        DPF_ERROR("Invalid flRoomRolloffFactor value");
    }

    if((pProperties->flDecayTime < DSI3DL2LISTENER_MINDECAYTIME) || (pProperties->flDecayTime > DSI3DL2LISTENER_MAXDECAYTIME))
    {
        DPF_ERROR("Invalid flDecayTime value");
    }

    if((pProperties->flDecayHFRatio < DSI3DL2LISTENER_MINDECAYHFRATIO) || (pProperties->flDecayHFRatio > DSI3DL2LISTENER_MAXDECAYHFRATIO))
    {
        DPF_ERROR("Invalid flDecayHFRatio value");
    }

    if((pProperties->lReflections < DSI3DL2LISTENER_MINREFLECTIONS) || (pProperties->lReflections > DSI3DL2LISTENER_MAXREFLECTIONS))
    {
        DPF_ERROR("Invalid lReflections value");
    }

    if((pProperties->flReflectionsDelay < DSI3DL2LISTENER_MINREFLECTIONSDELAY) || (pProperties->flReflectionsDelay > DSI3DL2LISTENER_MAXREFLECTIONSDELAY))
    {
        DPF_ERROR("Invalid flReflectionsDelay value");
    }

    if((pProperties->lReverb < DSI3DL2LISTENER_MINREVERB) || (pProperties->lReverb > DSI3DL2LISTENER_MAXREVERB))
    {
        DPF_ERROR("Invalid lReverb value");
    }

    if((pProperties->flReverbDelay < DSI3DL2LISTENER_MINREVERBDELAY) || (pProperties->flReverbDelay > DSI3DL2LISTENER_MAXREVERBDELAY))
    {
        DPF_ERROR("Invalid flReverbDelay value");
    }

    if((pProperties->flDiffusion < DSI3DL2LISTENER_MINDIFFUSION) || (pProperties->flDiffusion > DSI3DL2LISTENER_MAXDIFFUSION))
    {
        DPF_ERROR("Invalid flDiffusion value");
    }

    if((pProperties->flDensity < DSI3DL2LISTENER_MINDENSITY) || (pProperties->flDensity > DSI3DL2LISTENER_MAXDENSITY))
    {
        DPF_ERROR("Invalid flDensity value");
    }

    if((pProperties->flHFReference < DSI3DL2LISTENER_MINHFREFERENCE) || (pProperties->flHFReference > DSI3DL2LISTENER_MAXHFREFERENCE))
    {
        DPF_ERROR("Invalid flHFReference value");
    }

#endif // VALIDATE_PARAMETERS

    if(DSFX_IMAGELOC_UNUSED == m_pSettings->m_EffectLocations.dwI3DL2ReverbIndex)
    {
        DPF_ERROR("Can't set I3DL2 listener properties without loading the I3DL2 reverb effect");
        hr = DSERR_INVALIDCALL;
    }

    if(SUCCEEDED(hr))
    {
        m_pSettings->m_I3dl2Params = *pProperties;
        m_pSettings->m_dw3dParameterMask |= DS3DPARAM_LISTENER_I3DL2;

        if(!(dwFlags & DS3D_DEFERRED))
        {
            CommitDeferredSettings();
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  CommitDeferredSettings
 *
 *  Description:
 *      Commits deferred settings.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::CommitDeferredSettings"

HRESULT 
CDirectSound::CommitDeferredSettings
(
    void
)
{
    PLIST_ENTRY             pleEntry;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    //
    // Recalculate changed 3D listener parameters
    //

    m_pDevice->Commit3dSettings();

    //
    // Recalculate all 3D voices
    //

    for(pleEntry = m_lst3dVoices.Flink; pleEntry != &m_lst3dVoices; pleEntry = pleEntry->Flink)
    {
        CONTAINING_RECORD(pleEntry, CDirectSoundVoice, m_le3dVoice)->CommitDeferredSettings();
    }

    //
    // Reset the 3D listener parameter mask
    //

    m_pSettings->m_dw3dParameterMask = 0;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  GetTime
 *
 *  Description:
 *      Gets the current time.
 *
 *  Arguments:
 *      REFERENCE_TIME * [out]: current time.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::GetTime"

HRESULT
CDirectSound::GetTime
(
    REFERENCE_TIME *        prtCurrent
)
{
    DPF_ENTER();    

#ifdef VALIDATE_PARAMETERS

    if(!prtCurrent)
    {
        DPF_ERROR("Failed to specify a time buffer");
    }

#endif // VALIDATE_PARAMETERS

    KeQuerySystemTime((PLARGE_INTEGER)prtCurrent);

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  DoWork
 *
 *  Description:
 *      Uh... does some work?
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::DoWork"

void
CDirectSound::DoWork
(
    void
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD_VOID();

    m_pDevice->DoWork();

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  EnableHeadphones
 *
 *  Description:
 *      Enables or disables headphone support.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to enable.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#ifndef MCPX_BOOT_LIB

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::EnableHeadphones"

HRESULT
CDirectSound::EnableHeadphones
(
    BOOL                    fEnable
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    if(MAKEBOOL(m_pSettings->m_dwSpeakerConfig & DSSPEAKER_ENABLE_HEADPHONES) != MAKEBOOL(fEnable))
    {
        //
        // Set the headphone bit in the speaker config
        //

        if(fEnable)
        {
            m_pSettings->m_dwSpeakerConfig |= DSSPEAKER_ENABLE_HEADPHONES;
        }
        else
        {
            m_pSettings->m_dwSpeakerConfig &= ~DSSPEAKER_ENABLE_HEADPHONES;
        }

        //
        // Update the APU
        //

        hr = m_pDevice->SetSpeakerConfig();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  CDirectSoundVoiceSettings
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoiceSettings::CDirectSoundVoiceSettings"

CDirectSoundVoiceSettings::CDirectSoundVoiceSettings
(
    void
)
{
    DPF_ENTER();

#ifndef MCPX_BOOT_LIB

    m_3dParams = DirectSoundDefault3DBuffer;
    m_I3dl2Params = DirectSoundDefaultI3DL2Buffer;
    m_dw3dParameterMask = DS3DPARAM_BUFFER_MASK;

#endif // MCPX_BOOT_LIB

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CDirectSoundVoiceSettings
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoiceSettings::~CDirectSoundVoiceSettings"

CDirectSoundVoiceSettings::~CDirectSoundVoiceSettings
(
    void
)
{
    DPF_ENTER();
    
    //
    // Release the output buffer
    //

    if(m_pMixinBuffer)
    {
        SetOutputBuffer(NULL);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      DWORD [in]: voice creation flags.
 *      LPCWAVEFORMATEX [in]: voice format.
 *      LPCDSMIXBINS [in]: mix bin assignments.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoiceSettings::Initialize"

void
CDirectSoundVoiceSettings::Initialize
(
    DWORD                   dwFlags,
    LPCWAVEFORMATEX         pwfxFormat,
    LPCDSMIXBINS            pMixBins
)
{
    BOOL                    fSetMixBins;

    DPF_ENTER();

    ASSERT(pwfxFormat);

    //
    // Save flags
    //
    
    m_dwFlags = dwFlags;

    //
    // Set default headroom
    //

    if(m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        m_dwHeadroom = DSBHEADROOM_DEFAULT_SUBMIX;
    }
    else if(m_dwFlags & DSBCAPS_CTRL3D)
    {
        m_dwHeadroom = DSBHEADROOM_DEFAULT_3D;
    }
    else
    {
        m_dwHeadroom = DSBHEADROOM_DEFAULT_2D;
    }

    //
    // Set format.  If mixbins were specified, we won't allow the format
    // to override them.
    //

    fSetMixBins = SetFormat(pwfxFormat, !pMixBins);

    //
    // If SetFormat didn't set the mixbins for us, we'll need to do it now
    //

    if(!fSetMixBins)
    {
        SetMixBins(pMixBins);
    }

    //
    // Set default volume
    //

    SetVolume(DSBVOLUME_MAX);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetFormat
 *
 *  Description:
 *      Sets the voice format.
 *
 *  Arguments:
 *      LPCWAVEFORMATEX [in]: voice format.
 *      BOOL [in]: TRUE to allow the format to define the mixbins.
 *
 *  Returns:  
 *      BOOL: TRUE if the voice format changed the mixbin assignments.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoiceSettings::SetFormat"

BOOL
CDirectSoundVoiceSettings::SetFormat
(
    LPCWAVEFORMATEX         pwfxFormat,
    BOOL                    fAllowMixBins
)
{
    BOOL                    fSetMixBins                                     = FALSE;
    DWORD                   dwChannelMask;
    DSMIXBINS               MixBins;
    DSMIXBINVOLUMEPAIR      MixBinVolumePairs[DSMIXBIN_ASSIGNMENT_MAX];
    
    DPF_ENTER();

    ASSERT(pwfxFormat);

    //
    // Save format.  If MixBins were specified by the format, we'll use those
    // assuming no mixbins were passed to us and the voice is 2D.
    //

    if(dwChannelMask = FmtCreateInternal(&m_fmt, pwfxFormat))
    {
        if(!(m_dwFlags & DSBCAPS_CTRL3D) && fAllowMixBins)
        {
            MixBins.dwMixBinCount = 0;
            MixBins.lpMixBinVolumePairs = MixBinVolumePairs;
            
            while(dwChannelMask && (MixBins.dwMixBinCount < NUMELMS(MixBinVolumePairs)))
            {
                MixBinVolumePairs[MixBins.dwMixBinCount].dwMixBin = lsb(dwChannelMask);
                MixBinVolumePairs[MixBins.dwMixBinCount].lVolume = 0;

                MixBins.dwMixBinCount++;

                dwChannelMask &= ~(1UL << MixBinVolumePairs[MixBins.dwMixBinCount].dwMixBin);
            }

            SetMixBins(&MixBins);

            fSetMixBins = TRUE;
        }
    }

    //
    // Set default pitch
    //

    m_lPitch = XAudioCalculatePitch(m_fmt.nSamplesPerSec);

    DPF_LEAVE(fSetMixBins);

    return fSetMixBins;
}


/****************************************************************************
 *
 *  SetVolume
 *
 *  Description:
 *      Sets voice volume.
 *
 *  Arguments:
 *      LONG [in]: volume.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoiceSettings::SetVolume"

void
CDirectSoundVoiceSettings::SetVolume
(
    LONG                    lVolume
)
{
    DPF_ENTER();

    //
    // Apply headroom
    //

    m_lVolume = lVolume - m_dwHeadroom;
    ASSERT(m_lVolume <= DSBVOLUME_MAX);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetMixBinVolumes
 *
 *  Description:
 *      Sets the volume of some number of mixbins.
 *
 *  Arguments:
 *      LPCDSMIXBINS [in]: mixbin/volume pairs.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoiceSettings::SetMixBinVolumes"

void
CDirectSoundVoiceSettings::SetMixBinVolumes
(
    LPCDSMIXBINS            pMixBins
)
{
    DWORD                   i;
    
    DPF_ENTER();

    ASSERT(pMixBins);
    ASSERT(pMixBins->dwMixBinCount <= NUMELMS(m_alMixBinVolumes));

    for(i = 0; i < pMixBins->dwMixBinCount; i++)
    {
        ASSERT(pMixBins->lpMixBinVolumePairs[i].dwMixBin < NUMELMS(m_alMixBinVolumes));
        
        m_alMixBinVolumes[pMixBins->lpMixBinVolumePairs[i].dwMixBin] = pMixBins->lpMixBinVolumePairs[i].lVolume;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetHeadroom
 *
 *  Description:
 *      Sets voice headroom.
 *
 *  Arguments:
 *      DWORD [in]: headroom.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoiceSettings::SetHeadroom"

void
CDirectSoundVoiceSettings::SetHeadroom
(
    DWORD                   dwHeadroom
)
{
    LONG                    lDiff;
    
    DPF_ENTER();

    //
    // Calculate the difference between the old headroom and the new one
    //

    lDiff = m_dwHeadroom - dwHeadroom;

    //
    // Save the new headroom
    //

    m_dwHeadroom = dwHeadroom;
    
    //
    // Update volume
    //

    m_lVolume += lDiff;
    ASSERT(m_lVolume <= DSBVOLUME_MAX);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetMixBins
 *
 *  Description:
 *      Sets the mixbins used by the voice.
 *
 *  Arguments:
 *      LPCDSMIXBINS [in]: mixbin assignments.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoiceSettings::SetMixBins"

void
CDirectSoundVoiceSettings::SetMixBins
(
    LPCDSMIXBINS            pMixBins
)
{
    DPF_ENTER();

    //
    // If no mixbins were supplied, use defaults
    //

    if(!pMixBins)
    {
        if(m_dwFlags & DSBCAPS_CTRL3D)
        {
            pMixBins = &DirectSoundDefaultMixBins_3D;
        }
        else
        {
            switch(m_fmt.nChannels)
            {
                case 1:
                    pMixBins = &DirectSoundDefaultMixBins_Mono;
                    break;

                case 2:
                    pMixBins = &DirectSoundDefaultMixBins_Stereo;
                    break;

                case 4:
                    pMixBins = &DirectSoundDefaultMixBins_4Channel;
                    break;

                case 6:
                    pMixBins = &DirectSoundDefaultMixBins_6Channel;
                    break;

                default:
                    ASSERTMSG("Unexpected channel count");
                    break;
            }
        }
    }

    //
    // Set mixbins and volume
    //

    ASSERT(pMixBins);
    ASSERT(pMixBins->dwMixBinCount <= NUMELMS(m_adwMixBins));
    
    for(m_dwMixBinCount = 0; m_dwMixBinCount < pMixBins->dwMixBinCount; m_dwMixBinCount++)
    {
        ASSERT(pMixBins->lpMixBinVolumePairs[m_dwMixBinCount].dwMixBin < NUMELMS(m_alMixBinVolumes));
        
        m_adwMixBins[m_dwMixBinCount] = pMixBins->lpMixBinVolumePairs[m_dwMixBinCount].dwMixBin;
        m_alMixBinVolumes[m_adwMixBins[m_dwMixBinCount]] = pMixBins->lpMixBinVolumePairs[m_dwMixBinCount].lVolume;
    }

    //
    // If we're submixing, the submix bin must be included
    //

    if(m_pMixinBuffer)
    {
        ASSERT(m_pMixinBuffer->m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK);
        ASSERT(m_dwMixBinCount < NUMELMS(m_adwMixBins) - 1);

        m_adwMixBins[m_dwMixBinCount++] = m_pMixinBuffer->m_pSettings->m_dwInputMixBin;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetOutputBuffer
 *
 *  Description:
 *      Assignes the output of this voice to another voice instead of the
 *      standard mixbins.
 *
 *  Arguments:
 *      CDirectSoundBuffer * [in]: mixin buffer.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoiceSettings::SetOutputBuffer"

void
CDirectSoundVoiceSettings::SetOutputBuffer
(
    CDirectSoundBuffer *    pOutputBuffer
)
{
    DWORD                   dwMixBin;
    
    DPF_ENTER();

    ASSERT(pOutputBuffer != m_pMixinBuffer);

    //
    // Release the old buffer and update the mixbins
    //

    if(m_pMixinBuffer)
    {
        dwMixBin = m_pMixinBuffer->m_pSettings->m_dwInputMixBin;

        ASSERT(m_dwMixBinCount);
        ASSERT(m_adwMixBins[m_dwMixBinCount - 1] == dwMixBin);

        m_dwMixBinCount--;

        RELEASE(m_pMixinBuffer);
    }

    //
    // Save a reference to the new buffer and update the mixbins
    //

    if(m_pMixinBuffer = ADDREF(pOutputBuffer))
    {
        dwMixBin = m_pMixinBuffer->m_pSettings->m_dwInputMixBin;

        ASSERT(m_pMixinBuffer->m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK);
        ASSERT(m_dwMixBinCount < NUMELMS(m_adwMixBins) - 1);

        m_adwMixBins[m_dwMixBinCount++] = dwMixBin;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CDirectSoundVoice
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CDirectSound * [in]: parent DirectSound object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::CDirectSoundVoice"

CDirectSoundVoice::CDirectSoundVoice
(
    CDirectSound *          pDirectSound
)
{
    DPF_ENTER();

    //
    // Initialize defaults
    //

    InitializeListHead(&m_le3dVoice);

    //
    // Save a reference to the DirectSound object
    //

    m_pDirectSound = ADDREF(pDirectSound);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CDirectSoundVoice
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::~CDirectSoundVoice"

CDirectSoundVoice::~CDirectSoundVoice
(
    void
)
{
    DPF_ENTER();

    //
    // Remove ourselves from the 3D list
    //

    RemoveEntryList(&m_le3dVoice);

    //
    // Release the reference to the DirectSound object
    //

    RELEASE(m_pDirectSound);

    //
    // Release the voice implementation object
    //

    RELEASE(m_pVoice);

    //
    // Release the shared settings object
    //

    RELEASE(m_pSettings);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      CMcpxVoiceClient * [in]: voice implementation object.
 *      CDirectSoundVoiceSettings * [in]: shared settings object.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::Initialize"

void
CDirectSoundVoice::Initialize
(
    CMcpxVoiceClient *          pVoice, 
    CDirectSoundVoiceSettings * pSettings
)
{
    DPF_ENTER();

    //
    // Just save a reference to each
    //

    m_pVoice = ADDREF(pVoice);
    m_pSettings = ADDREF(pSettings);

    //
    // If we're a 3D voice, add ourselves to the 3D voice list
    //

    if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
    {
        InsertTailList(&m_pDirectSound->m_lst3dVoices, &m_le3dVoice);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetFormat
 *
 *  Description:
 *      Sets the voice format.
 *
 *  Arguments:
 *      LPCWAVEFORMATEX [in]: new format.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetFormat"

HRESULT 
CDirectSoundVoice::SetFormat
(
    LPCWAVEFORMATEX         pwfxFormat
)
{
    BOOL                    fSetMixBins;
    HRESULT                 hr;

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!pwfxFormat)
    {
        DPF_ERROR("Failed to specify a buffer format");
    }

    if(!FmtIsValid(pwfxFormat))
    {
        DPF_ERROR("Invalid voice format");
    }

    if((m_pSettings->m_dwFlags & DSBCAPS_CTRL3D) && (1 != pwfxFormat->nChannels))
    {
        DPF_ERROR("3D voices must be mono");
    }

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't SetFormat on MIXIN or FXIN buffers");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Update the settings object
    //

    fSetMixBins = m_pSettings->SetFormat(pwfxFormat, TRUE);

    //
    // Update the implementation object
    //

    hr = m_pVoice->SetFormat();

    //
    // In some cases, the mixbins are defined by the format structure
    //
    
    if(SUCCEEDED(hr) && fSetMixBins)
    {
        hr = m_pVoice->SetMixBins();
    }

    //
    // Make sure the pitch is updated as well
    //

    if(SUCCEEDED(hr))
    {
        m_pVoice->SetPitch();
    }
    
    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetFrequency
 *
 *  Description:
 *      Sets the buffer playback frequency.
 *
 *  Arguments:
 *      DWORD [in]: frequency in Hz.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetFrequency"

HRESULT 
CDirectSoundVoice::SetFrequency
(
    DWORD                   dwFrequency
)
{
    HRESULT                 hr;

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(dwFrequency)
    {
        if((dwFrequency < DSBFREQUENCY_MIN) || (dwFrequency > DSBFREQUENCY_MAX))
        {
            DPF_ERROR("Invalid frequency value");
        }
    }

#endif // VALIDATE_PARAMETERS

    //
    // Convert frequency to pitch.  0 frequency means use the default.
    //

    if(!dwFrequency)
    {
        dwFrequency = m_pSettings->m_fmt.nSamplesPerSec;
    }

    hr = SetPitch(XAudioCalculatePitch(dwFrequency));

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetPitch
 *
 *  Description:
 *      Sets the buffer pitch.
 *
 *  Arguments:
 *      LONG [in]: pitch, in octaves (s3.12).
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetPitch"

HRESULT 
CDirectSoundVoice::SetPitch
(
    LONG                    lPitch
)
{
    HRESULT                 hr;

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if((lPitch < DSBPITCH_MIN) || (lPitch > DSBPITCH_MAX))
    {
        DPF_ERROR("Invalid pitch value");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Update the settings object
    //

    m_pSettings->m_lPitch = lPitch;

    //
    // Notify the implementation object of the change
    //

    hr = m_pVoice->SetPitch();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetVolume
 *
 *  Description:
 *      Sets the buffer playback volume.
 *
 *  Arguments:
 *      LONG [in]: volume.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetVolume"

HRESULT
CDirectSoundVoice::SetVolume
(
    LONG                    lVolume
)
{
    HRESULT                 hr;

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if((lVolume < DSBVOLUME_MIN) || (lVolume > DSBVOLUME_MAX))
    {
        DPF_ERROR("Volume value out-of-bounds");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Save the volume to the settings
    //

    m_pSettings->SetVolume(lVolume);
    
    //
    // Notify the implementation object
    //

    hr = m_pVoice->SetVolume();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetLFO
 *
 *  Description:
 *      Sets parameters for one of the low-frequency occilator.
 *
 *  Arguments:
 *      LPCDSLFODESC [in]: common LFO settings.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetLFO"

HRESULT
CDirectSoundVoice::SetLFO
(
    LPCDSLFODESC            pLfo
)
{
    HRESULT                 hr;

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!pLfo)
    {
        DPF_ERROR("LFO description not provided");
    }

    if((pLfo->dwLFO < DSLFO_FIRST) && (pLfo->dwLFO > DSLFO_LAST))
    {
        DPF_ERROR("Invalid LFO identifier");
    }

    if(pLfo->dwDelay > 0x7FFF)
    {
        DPF_ERROR("Invalid LFO delay value");
    }

    if(pLfo->dwDelta > 0x3FF)
    {
        DPF_ERROR("Invalid LFO delta value");
    }

    if((pLfo->lPitchModulation < -128) || (pLfo->lPitchModulation > 127))
    {
        DPF_ERROR("Invalid LFO pitch modulation value");
    }

    if((pLfo->lFilterCutOffRange < -128) || (pLfo->lFilterCutOffRange > 127))
    {
        DPF_ERROR("Invalid LFO filter cutoff value");
    }

    if((pLfo->lAmplitudeModulation < -128) || (pLfo->lAmplitudeModulation > 127))
    {
        DPF_ERROR("Invalid LFO amplitude modulation value");
    }

    if(pLfo->dwLFO != DSLFO_MULTI)
    {
        if(pLfo->lFilterCutOffRange || pLfo->lAmplitudeModulation)
        {
            DPF_WARNING("The pitch LFO doesn not support filter cut-off range or amplitude modulation");
        }
    }

#endif // VALIDATE_PARAMETERS

    //
    // Hand off to the implementation object
    //

    hr = m_pVoice->SetLFO(pLfo);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetEG
 *
 *  Description:
 *      Sets parameters for one of the envelope generators.
 *
 *  Arguments:
 *      LPCDSENVELOPEDESC [in]: common envelope settings.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetEG"

HRESULT
CDirectSoundVoice::SetEG
(
    LPCDSENVELOPEDESC       pEnv
)
{
    HRESULT                 hr;

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!pEnv)
    {
        DPF_ERROR("Envelope description not provided");
    }

    if((pEnv->dwEG < DSEG_FIRST) || (pEnv->dwEG > DSEG_LAST))
    {
        DPF_ERROR("Invalid envelope generator identifier");
    }

    if((pEnv->dwMode < DSEG_MODE_FIRST) || (pEnv->dwMode > DSEG_MODE_LAST))
    {
        DPF_ERROR("Invalid envelope generator mode");
    }

    if(pEnv->dwDelay > 0x7FFF)
    {
        DPF_ERROR("Invalid envelope delay value");
    }

    if(pEnv->dwAttack > 0x7FFF)
    {
        DPF_ERROR("Invalid envelope attack value");
    }

    if(pEnv->dwHold > 0x7FFF)
    {
        DPF_ERROR("Invalid envelope hold value");
    }

    if(pEnv->dwDecay > 0x7FFF)
    {
        DPF_ERROR("Invalid envelope decay value");
    }

    if(pEnv->dwRelease > 0x7FFF)
    {
        DPF_ERROR("Invalid envelope release value");
    }

    if(pEnv->dwSustain > 0xFF)
    {
        DPF_ERROR("Invalid envelope sustain value");
    }

    if((pEnv->lPitchScale < -128) || (pEnv->lPitchScale > 127))
    {
        DPF_ERROR("Invalid envelope pitch scale value");
    }

    if((pEnv->lFilterCutOff < -128) || (pEnv->lFilterCutOff > 127))
    {
        DPF_ERROR("Invalid envelope filter cutoff value");
    }

    if(pEnv->dwEG != DSEG_MULTI)
    {
        if(pEnv->lPitchScale || pEnv->lFilterCutOff)
        {
            DPF_WARNING("The amplitude envelope doesn not support pitch scale or filter cutoff");
        }
    }

#endif // VALIDATE_PARAMETERS

    //
    // Hand off to the implementation object
    //

    hr = m_pVoice->SetEG(pEnv);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetFilter
 *
 *  Description:
 *      Sets parameters for the filter.
 *
 *  Arguments:
 *      LPCDSFILTERDESC [in]: filter settings.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetFilter"

HRESULT
CDirectSoundVoice::SetFilter
(
    LPCDSFILTERDESC         pFilter
)
{
    HRESULT                 hr;

#ifdef VALIDATE_PARAMETERS

    DWORD                   i;

#endif // VALIDATE_PARAMETERS

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!pFilter)
    {
        DPF_ERROR("Filter description not provided");
    }

    if((pFilter->dwMode < DSFILTER_MODE_FIRST) || (pFilter->dwMode > DSFILTER_MODE_LAST))
    {
        DPF_ERROR("Invalid filter mode");
    }

    if(pFilter->dwQCoefficient > 7)
    {
        DPF_ERROR("Invalid Q-coefficient value");
    }

    for(i = 0; i < NUMELMS(pFilter->adwCoefficients); i++)
    {
        if(pFilter->adwCoefficients[i] > 0xFFFF)
        {
            DPF_ERROR("Invalid coefficient value (index %lu)", i);
        }
    }

    if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
    {
        if(DSFILTER_MODE_BYPASS == pFilter->dwMode)
        {
            DPF_ERROR("The filter can't be bypassed on a 3D voice");
        }

        for(i = 2; i < NUMELMS(pFilter->adwCoefficients); i++)
        {
            if(pFilter->adwCoefficients[i])
            {
                DPF_ERROR("Only the first 2 coefficients are valid on a 3D voice");
            }
        }
    }

#endif // VALIDATE_PARAMETERS

    //
    // Hand off to the implementation object
    //

    hr = m_pVoice->SetFilter(pFilter);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetHeadroom
 *
 *  Description:
 *      Sets voice headroom.
 *
 *  Arguments:
 *      DWORD [in]: headroom, in Millibels.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetHeadroom"

HRESULT
CDirectSoundVoice::SetHeadroom
(
    DWORD                   dwHeadroom
)
{
    HRESULT                 hr;

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if((dwHeadroom < DSBHEADROOM_MIN) || (dwHeadroom > DSBHEADROOM_MAX))
    {
        DPF_ERROR("Headroom value out-of-bounds");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Save the headroom to the settings
    //

    m_pSettings->SetHeadroom(dwHeadroom);
    
    //
    // Headroom affects voice volume, so notify the implementation object
    //

    hr = m_pVoice->SetVolume();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}    


/****************************************************************************
 *
 *  SetOutputBuffer
 *
 *  Description:
 *      Assignes the output of this voice to another voice instead of the
 *      standard mixbins.
 *
 *  Arguments:
 *      LPDIRECTSOUNDBUFFER [in]: mixin buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetOutputBuffer"

HRESULT
CDirectSoundVoice::SetOutputBuffer
(
    LPDIRECTSOUNDBUFFER     pOutputBuffer
)
{
    CDirectSoundBuffer *    pMixinBuffer    = (CDirectSoundBuffer *)pOutputBuffer;
    HRESULT                 hr              = DS_OK;

#ifdef VALIDATE_PARAMETERS
    
    static BOOL             fSubMixHeadroom = FALSE;
    static BOOL             fExtraMixBins   = FALSE;
    DWORD                   i;

#endif // VALIDATE_PARAMETERS

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(pMixinBuffer)
    {
        //
        // Make sure the output buffer is really MIXIN
        //

        if(!(pMixinBuffer->m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK))
        {
            DPF_ERROR("Output buffer specified is not a MIXIN/FXIN buffer");
        }

        //
        // Only supporting a single submix
        //

        if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
        {
            DPF_ERROR("Can't set the output buffer on MIXIN/FXIN buffers");
        }

        //
        // Can't submix 3D voices
        //

        if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
        {
            DPF_ERROR("Can't submix a 3D buffer.  Create a 2D source buffer and a 3D destination buffer.");
        }

        //
        // Can't submix anything but mono
        //

        if(1 != m_pSettings->m_fmt.nChannels)
        {
            DPF_ERROR("Can only submix mono source voices");
        }

        //
        // Must have one mixbin slot free for the submix bin
        //

        if(m_pSettings->m_dwMixBinCount >= NUMELMS(m_pSettings->m_adwMixBins))
        {
            DPF_ERROR("Submixed voices must leave one mixbin slot free");
        }

        //
        // Reduce API ignorance
        //

        if(!fExtraMixBins)
        {
            if(m_pSettings->m_dwMixBinCount)
            {
                DPF_WARNING("The voice you're adding to the submix chain is assigned to mixbins.  You will hear the voice both through the submix output and the mixbins it's assigned to.  In order to prevent this, set the mixbin count to 0 before calling SetOutputBuffer.  You will only see this warning once.");
                fExtraMixBins = TRUE;
            }
        }

        //
        // Make sure we're not double-assigned to the submix bin
        //

        for(i = 0; i < m_pSettings->m_dwMixBinCount; i++)
        {
            if(m_pSettings->m_adwMixBins[i] == pMixinBuffer->m_pSettings->m_dwInputMixBin)
            {
                DPF_WARNING("The mixbin your submix destination is reading from is already in your mixbin list");
                break;
            }
        }

        //
        // Check the headroom for the destination voice
        //

        if(!fSubMixHeadroom)
        {
            if(m_pDirectSound->m_pSettings->m_adwMixBinHeadroom[pMixinBuffer->m_pSettings->m_dwInputMixBin])
            {
                DPF_WARNING("The headroom for the mixbin your submix destination is reading from is non-zero.  This will be additive with the headroom that your submix destination is reading to, possibly giving you more headroom that you want.  You will only see this warning once.");
                fSubMixHeadroom = TRUE;
            }
        }
    }

#endif // VALIDATE_PARAMETERS

    if(pMixinBuffer != m_pSettings->m_pMixinBuffer)
    {
        //
        // Disconnect from the current buffer
        //

        if(m_pSettings->m_pMixinBuffer)
        {
            hr = m_pVoice->DisconnectVoice();
        }
        
        //
        // Update the settings object
        //

        if(SUCCEEDED(hr))
        {
            m_pSettings->SetOutputBuffer(pMixinBuffer);
        }

        //
        // Hand off to the implementation object.  ConnectVoice handles
        // resetting the mixbins and mixbin volumes as well as voice list
        // management.
        //

        if(SUCCEEDED(hr) && pMixinBuffer)
        {
            hr = m_pVoice->ConnectVoice();
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetOutputBuffer_v1
 *
 *  Description:
 *      Assignes the output of this voice to another voice instead of the
 *      standard mixbins.
 *
 *  Arguments:
 *      LPDIRECTSOUNDBUFFER [in]: mixin buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetOutputBuffer_v1"

HRESULT
CDirectSoundVoice::SetOutputBuffer_v1
(
    LPDIRECTSOUNDBUFFER     pOutputBuffer
)
{
    DSMIXBINS               MixBins;
    HRESULT                 hr;

    DPF_ENTER();

    //
    // Assign the voice to 0 mixbins
    //

    MixBins.dwMixBinCount = 0;
    
    hr = SetMixBins(&MixBins);

    //
    // Set the output buffer
    //

    if(SUCCEEDED(hr))
    {
        hr = SetOutputBuffer(pOutputBuffer);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetMixBins
 *
 *  Description:
 *      Assigns the output of this voice to specific mixbins.
 *
 *  Arguments:
 *      LPCDSMIXBINS [in]: mixbin assignments.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetMixBins"

HRESULT
CDirectSoundVoice::SetMixBins
(
    LPCDSMIXBINS            pMixBins
)
{
    HRESULT                 hr;

#ifdef VALIDATE_PARAMETERS  

    DWORD                   i;
   
#endif // VALIDATE_PARAMETERS    

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(pMixBins->dwMixBinCount)
    {
        if(pMixBins->dwMixBinCount > DSMIXBIN_ASSIGNMENT_MAX)
        {
            DPF_ERROR("No voice can be assigned to more than %lu mix bins", DSMIXBIN_ASSIGNMENT_MAX);
        }
        
        if(pMixBins->dwMixBinCount % m_pSettings->m_fmt.nChannels)
        {
            DPF_ERROR("The mixbin count must be a multiple of the channel count");
        }

        for(i = 0; i < pMixBins->dwMixBinCount; i++)
        {
            if(pMixBins->lpMixBinVolumePairs[i].dwMixBin > DSMIXBIN_LAST)
            {
                DPF_ERROR("Specified an invalid mixbin");
            }

            if(DSMIXBIN_SUBMIX == pMixBins->lpMixBinVolumePairs[i].dwMixBin)
            {
                DPF_ERROR("Don't SetMixBins to DSMIXBIN_SUBMIX.  Instead, call SetOutputBuffer");
            }

            if(m_pSettings->m_pMixinBuffer)
            {
                if(pMixBins->lpMixBinVolumePairs[i].dwMixBin == m_pSettings->m_pMixinBuffer->m_pSettings->m_dwInputMixBin)
                {
                    DPF_WARNING("Don't use SetMixBins to redirect output to an FXIN buffer.  Instead, call SetOutputBuffer");
                }
            }
        }

        if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
        {
            if(pMixBins->dwMixBinCount < DirectSoundRequiredMixBins_3D.dwMixBinCount)
            {
                DPF_ERROR("MixBin count doesn't include enough mixbins to hold all required for 3D");
            }
            
            for(i = 0; i < DirectSoundRequiredMixBins_3D.dwMixBinCount; i++)
            {
                if(pMixBins->lpMixBinVolumePairs[i].dwMixBin != DirectSoundRequiredMixBins_3D.lpMixBinVolumePairs[i].dwMixBin)
                {
                    DPF_ERROR("MixBin array does not include the required 3D bins");
                }
            }
        }

        if(m_pSettings->m_pMixinBuffer)
        {
            if(pMixBins->dwMixBinCount >= DSMIXBIN_ASSIGNMENT_MAX)
            {
                DPF_ERROR("Submixed voices need to keep one mixbin slot free");
            }
        }
    }

#endif // VALIDATE_PARAMETERS

    //
    // Save settings
    //

    m_pSettings->SetMixBins(pMixBins);

    //
    // Hand off to the implementation object
    //

    hr = m_pVoice->SetMixBins();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetMixBins_v1
 *
 *  Description:
 *      Assigns the output of this voice to specific mixbins.
 *
 *  Arguments:
 *      DWORD [in]: mixbin assignment mask.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetMixBins_v1"

HRESULT
CDirectSoundVoice::SetMixBins_v1
(
    DWORD                   dwMixBinMask
)
{
    DSMIXBINS               MixBins;
    DSMIXBINVOLUMEPAIR      MixBinVolumePairs[DSMIXBIN_ASSIGNMENT_MAX];
    DWORD                   dwMixBin;
    HRESULT                 hr;

    DPF_ENTER();

    MixBins.dwMixBinCount = 0;
    MixBins.lpMixBinVolumePairs = MixBinVolumePairs;

    while(dwMixBinMask)
    {
        if(MixBins.dwMixBinCount >= NUMELMS(MixBinVolumePairs))
        {
            DPF_ERROR("Too many mixbins in the mask");
            break;
        }

        MixBinVolumePairs[MixBins.dwMixBinCount].dwMixBin = lsb(dwMixBinMask);
        MixBinVolumePairs[MixBins.dwMixBinCount].lVolume = 0;

        dwMixBinMask &= ~(1UL << MixBinVolumePairs[MixBins.dwMixBinCount].dwMixBin);

        MixBins.dwMixBinCount++;
    }

    hr = SetMixBins(&MixBins);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetMixBinVolumes
 *
 *  Description:
 *      Sets the volume of each channel of the voice as it applies to it's
 *      assigned mixbins.
 *
 *  Arguments:
 *      LPCDSMIXBINS [in]: mixbin volume data.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetMixBinVolumes"

HRESULT
CDirectSoundVoice::SetMixBinVolumes
(
    LPCDSMIXBINS            pMixBins
)
{
    HRESULT                 hr;

#ifdef VALIDATE_PARAMETERS

    DWORD                   i, z;

#endif // VALIDATE_PARAMETERS

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!pMixBins)
    {
        DPF_ERROR("Failed to supply any volume data");
    }

    if(pMixBins->dwMixBinCount > DSMIXBIN_ASSIGNMENT_MAX)
    {
        DPF_ERROR("Invalid mixbin count");
    }
    
    for(i = 0; i < pMixBins->dwMixBinCount; i++)
    {
        for(z = 0; z < m_pSettings->m_dwMixBinCount; z++)
        {
            if(pMixBins->lpMixBinVolumePairs[i].dwMixBin == m_pSettings->m_adwMixBins[z])
            {
                break;
            }
        }

        if(z >= m_pSettings->m_dwMixBinCount)
        {
            DPF_ERROR("Specified a mixbin the voice is not assigned to at index %lu (mixbin %lu)", i, pMixBins->lpMixBinVolumePairs[i].dwMixBin);
        }
        
        if((pMixBins->lpMixBinVolumePairs[i].lVolume < DSBVOLUME_MIN) || (pMixBins->lpMixBinVolumePairs[i].lVolume > DSBVOLUME_MAX))
        {
            DPF_ERROR("Invalid volume at index %lu", i);
        }
    }

#endif // VALIDATE_PARAMETERS

    //
    // Update the volume
    //

    m_pSettings->SetMixBinVolumes(pMixBins);

    //
    // Hand off to the implementation object
    //

    hr = m_pVoice->SetVolume();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetMixBinVolumes_v1
 *
 *  Description:
 *      Sets the volume of each channel of the voice as it applies to it's
 *      assigned mixbins.
 *
 *  Arguments:
 *      DWORD [in]: mask of mixbins to set volume on.
 *      const LONG * [in]: volume array.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetMixBinVolumes_v1"

HRESULT
CDirectSoundVoice::SetMixBinVolumes_v1
(
    DWORD                   dwMixBinMask,
    const LONG *            palVolumes
)
{
    DSMIXBINS               MixBins;
    DSMIXBINVOLUMEPAIR      MixBinVolumePairs[DSMIXBIN_ASSIGNMENT_MAX];
    HRESULT                 hr;
    DWORD                   i;

    DPF_ENTER();

    MixBins.dwMixBinCount = 0;
    MixBins.lpMixBinVolumePairs = MixBinVolumePairs;

    while(dwMixBinMask)
    {
        if(MixBins.dwMixBinCount >= NUMELMS(MixBinVolumePairs))
        {
            DPF_ERROR("Too many mixbins in the mask");
            break;
        }

        MixBinVolumePairs[MixBins.dwMixBinCount].dwMixBin = lsb(dwMixBinMask);
        MixBinVolumePairs[MixBins.dwMixBinCount].lVolume = palVolumes[MixBins.dwMixBinCount];

        dwMixBinMask &= ~(1UL << MixBinVolumePairs[MixBins.dwMixBinCount].dwMixBin);

        MixBins.dwMixBinCount++;
    }

    hr = SetMixBinVolumes(&MixBins);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


#ifndef MCPX_BOOT_LIB

/***************************************************************************
 *
 *  SetAllParameters
 *
 *  Description:
 *      Sets all object properties.
 *
 *  Arguments:
 *      LPDS3DBUFFER [in]: object parameters.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetAllParameters"

HRESULT 
CDirectSoundVoice::SetAllParameters
(
    LPCDS3DBUFFER           pParams, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!pParams)
    {
        DPF_ERROR("Parameters not supplied");
    }

    if(pParams->dwSize < sizeof(*pParams))
    {
        DPF_ERROR("Invalid structure size");
    }

    if((pParams->dwInsideConeAngle < DS3D_MINCONEANGLE) || (pParams->dwInsideConeAngle > DS3D_MAXCONEANGLE) || (pParams->dwOutsideConeAngle < DS3D_MINCONEANGLE) || (pParams->dwOutsideConeAngle > DS3D_MAXCONEANGLE))
    {
        DPF_ERROR("Invalid cone angle value");
    }

    if(pParams->dwInsideConeAngle > pParams->dwOutsideConeAngle)
    {
        DPF_ERROR("Inside cone angle can't be greater than outside");
    }

    if(!pParams->vConeOrientation.x && !pParams->vConeOrientation.y && !pParams->vConeOrientation.z)
    {
        DPF_ERROR("Invalid cone orientation");
    }
    
    if((pParams->lConeOutsideVolume < DSBVOLUME_MIN) || (pParams->lConeOutsideVolume > DSBVOLUME_MAX))
    {
        DPF_ERROR("Invalid volume level");
    }

    if(!pParams->flMinDistance || !pParams->flMaxDistance)
    {
        DPF_ERROR("Minimum and maximum distance must be non-zero");
    }

    if((pParams->dwMode < DS3DMODE_FIRST) || (pParams->dwMode > DS3DMODE_LAST))
    {
        DPF_ERROR("Invalid mode");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams = *pParams;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_BUFFER_MASK;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetConeAngles
 *
 *  Description:
 *      Sets the sound cone's angles.
 *
 *  Arguments:
 *      DWORD [in]: inside angle.
 *      DWORD [in]: outside angle.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetConeAngles"

HRESULT 
CDirectSoundVoice::SetConeAngles
(
    DWORD                   dwInside, 
    DWORD                   dwOutside, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if((dwInside < DS3D_MINCONEANGLE) || (dwInside > DS3D_MAXCONEANGLE) || (dwOutside < DS3D_MINCONEANGLE) || (dwOutside > DS3D_MAXCONEANGLE))
    {
        DPF_ERROR("Invalid cone angle value");
    }

    if(dwInside > dwOutside)
    {
        DPF_ERROR("Inside cone angle can't be greater than outside");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.dwInsideConeAngle = dwInside;
    m_pSettings->m_3dParams.dwOutsideConeAngle = dwOutside;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_BUFFER_CONEANGLES;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetConeOrientation
 *
 *  Description:
 *      Sets the sound cone's orientation.
 *
 *  Arguments:
 *      REFD3DXVECTOR3 [in]: orientation.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetConeOrientation"

HRESULT 
CDirectSoundVoice::SetConeOrientation
(
    FLOAT                   flOrientationX,
    FLOAT                   flOrientationY,
    FLOAT                   flOrientationZ,
    DWORD                   dwFlags
)
{
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!flOrientationX && !flOrientationY && !flOrientationZ)
    {
        DPF_ERROR("Invalid cone orientation");
    }
    
    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.vConeOrientation.x = flOrientationX;
    m_pSettings->m_3dParams.vConeOrientation.y = flOrientationY;
    m_pSettings->m_3dParams.vConeOrientation.z = flOrientationZ;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_BUFFER_CONEORIENTATION;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetConeOutsideVolume
 *
 *  Description:
 *      Sets the sound cone's outside volume.
 *
 *  Arguments:
 *      LONG [in]: volume.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetConeOutsideVolume"

HRESULT 
CDirectSoundVoice::SetConeOutsideVolume
(
    LONG                    lVolume, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if((lVolume < DSBVOLUME_MIN) || (lVolume > DSBVOLUME_MAX))
    {
        DPF_ERROR("Invalid volume level");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.lConeOutsideVolume = lVolume;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_BUFFER_CONEOUTSIDEVOLUME;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetMaxDistance
 *
 *  Description:
 *      Sets the objects maximum distance from the listener.
 *
 *  Arguments:
 *      FLOAT [in]: maximum distance.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetMaxDistance"

HRESULT 
CDirectSoundVoice::SetMaxDistance
(
    FLOAT                   flMaxDistance, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if((flMaxDistance < DS3D_MINMAXDISTANCE) || (flMaxDistance > DS3D_MAXMAXDISTANCE))
    {
        DPF_ERROR("Invalid max distance value");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

    if(flMaxDistance < m_pSettings->m_3dParams.flMinDistance)
    {
        DPF_ERROR("flMaxDistance must be >= flMinDistance");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.flMaxDistance = flMaxDistance;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_BUFFER_MAXDISTANCE;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetMinDistance
 *
 *  Description:
 *      Sets the objects minimum distance from the listener.
 *
 *  Arguments:
 *      FLOAT [in]: minimum distance.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetMinDistance"

HRESULT 
CDirectSoundVoice::SetMinDistance
(
    FLOAT                   flMinDistance, 
    DWORD                   dwFlags
)
{
    HRESULT                 hr;

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if((flMinDistance < DS3D_MINMINDISTANCE) || (flMinDistance > DS3D_MAXMINDISTANCE))
    {
        DPF_ERROR("Invalid min distance value");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

    if(flMinDistance > m_pSettings->m_3dParams.flMaxDistance)
    {
        DPF_ERROR("MinDistance must be <= MaxDistance");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.flMinDistance = flMinDistance;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_BUFFER_MINDISTANCE;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetMode
 *
 *  Description:
 *      Sets the objects mode.
 *
 *  Arguments:
 *      DWORD [in]: mode.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetMode"

HRESULT 
CDirectSoundVoice::SetMode
(
    DWORD                   dwMode, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if((dwMode < DS3DMODE_FIRST) || (dwMode > DS3DMODE_LAST))
    {
        DPF_ERROR("Invalid mode");
    }
    
    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

#endif // VALIDATE_PARAMETERS

    //
    // There is no parameter bit for mode since what's recalculated is based
    // on the mode change
    //
    
    m_pSettings->m_3dParams.dwMode = dwMode;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetPosition
 *
 *  Description:
 *      Sets the objects position.
 *
 *  Arguments:
 *      REFD3DXVECTOR3 [in]: position.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetPosition"

HRESULT 
CDirectSoundVoice::SetPosition
(
    FLOAT                   flPositionX,
    FLOAT                   flPositionY,
    FLOAT                   flPositionZ,
    DWORD                   dwFlags
)
{
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.vPosition.x = flPositionX;
    m_pSettings->m_3dParams.vPosition.y = flPositionY;
    m_pSettings->m_3dParams.vPosition.z = flPositionZ;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_BUFFER_POSITION;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetVelocity
 *
 *  Description:
 *      Sets the objects velocity.
 *
 *  Arguments:
 *      REFD3DXVECTOR3 [in]: velocity.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetVelocity"

HRESULT 
CDirectSoundVoice::SetVelocity
(
    FLOAT                   flVelocityX,
    FLOAT                   flVelocityY,
    FLOAT                   flVelocityZ,
    DWORD                   dwFlags
)
{
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_3dParams.vVelocity.x = flVelocityX;
    m_pSettings->m_3dParams.vVelocity.y = flVelocityY;
    m_pSettings->m_3dParams.vVelocity.z = flVelocityZ;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_BUFFER_VELOCITY;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  SetI3DL2Source
 *
 *  Description:
 *      Sets I3DL2 source parameters.
 *
 *  Arguments:
 *      LPCDSI3DL2BUFFER [in]: source parameters.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetI3DL2Source"

HRESULT
CDirectSoundVoice::SetI3DL2Source
(
    LPCDSI3DL2BUFFER        pProperties,
    DWORD                   dwFlags
)
{
    HRESULT                 hr;
    
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!pProperties)
    {
        DPF_ERROR("Failed to supply parameters");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if((pProperties->lDirect < DSI3DL2BUFFER_MINDIRECT) || (pProperties->lDirect > DSI3DL2BUFFER_MAXDIRECT))
    {
        DPF_ERROR("Invalid lDirect value");
    }

    if((pProperties->lDirectHF < DSI3DL2BUFFER_MINDIRECTHF) || (pProperties->lDirectHF > DSI3DL2BUFFER_MAXDIRECTHF))
    {
        DPF_ERROR("Invalid lDirectHF value");
    }

    if((pProperties->lRoom < DSI3DL2BUFFER_MINROOM) || (pProperties->lRoom > DSI3DL2BUFFER_MAXROOM))
    {
        DPF_ERROR("Invalid lRoom value");
    }

    if((pProperties->lRoomHF < DSI3DL2BUFFER_MINROOMHF) || (pProperties->lRoomHF > DSI3DL2BUFFER_MAXROOMHF))
    {
        DPF_ERROR("Invalid lRoomHF value");
    }

    if((pProperties->flRoomRolloffFactor < DSI3DL2BUFFER_MINROOMROLLOFFFACTOR) || (pProperties->flRoomRolloffFactor > DSI3DL2BUFFER_MAXROOMROLLOFFFACTOR))
    {
        DPF_ERROR("Invalid flRoomRolloffFactor value");
    }

    if((pProperties->Obstruction.lHFLevel < DSI3DL2BUFFER_MINOBSTRUCTION) || (pProperties->Obstruction.lHFLevel > DSI3DL2BUFFER_MAXOBSTRUCTION))
    {
        DPF_ERROR("Invalid Obstruction.lHFLevel value");
    }

    if((pProperties->Obstruction.flLFRatio < DSI3DL2BUFFER_MINOBSTRUCTIONLFRATIO) || (pProperties->Obstruction.flLFRatio > DSI3DL2BUFFER_MAXOBSTRUCTIONLFRATIO))
    {
        DPF_ERROR("Invalid Obstruction.flLFRatio value");
    }

    if((pProperties->Occlusion.lHFLevel < DSI3DL2BUFFER_MINOCCLUSION) || (pProperties->Occlusion.lHFLevel > DSI3DL2BUFFER_MAXOCCLUSION))
    {
        DPF_ERROR("Invalid Occlusion.lHFLevel value");
    }

    if((pProperties->Occlusion.flLFRatio < DSI3DL2BUFFER_MINOCCLUSIONLFRATIO) || (pProperties->Occlusion.flLFRatio > DSI3DL2BUFFER_MAXOCCLUSIONLFRATIO))
    {
        DPF_ERROR("Invalid Occlusion.flLFRatio value");
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_I3dl2Params = *pProperties;
    m_pSettings->m_dw3dParameterMask |= DS3DPARAM_BUFFER_I3DL2;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  CommitDeferredSettings
 *
 *  Description:
 *      Commits deferred settings.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::CommitDeferredSettings"

HRESULT 
CDirectSoundVoice::CommitDeferredSettings
(
    void
)
{
    DPF_ENTER();

    //
    // Include the changed listener parameters in the 3D parameter mask
    //
    
    m_pSettings->m_dw3dParameterMask |= m_pDirectSound->m_pSettings->m_dw3dParameterMask;

    //
    // Apply changes to the voice.  The voice implementation object will 
    // properly reset m_pSettings->m_dw3dParameterMask for us.
    //
    
    m_pVoice->Commit3dSettings();

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  CDirectSoundBufferSettings
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBufferSettings::CDirectSoundBufferSettings"

const WAVEFORMATEX CDirectSoundBufferSettings::m_wfxMixDest = INIT_PCM_WAVEFORMAT_EX(1, 48000, 24, 32);

CDirectSoundBufferSettings::CDirectSoundBufferSettings
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CDirectSoundBufferSettings
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBufferSettings::~CDirectSoundBufferSettings"

CDirectSoundBufferSettings::~CDirectSoundBufferSettings
(
    void
)
{
    DPF_ENTER();
    
    //
    // Free the data buffer
    //
    
    SetBufferData(NULL, 0);

    //
    // Free notifications
    //

    SetNotificationPositions(0, NULL);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      LPCDSBUFFERDESC [in]: buffer description.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBufferSettings::Initialize"

HRESULT
CDirectSoundBufferSettings::Initialize
(
    LPCDSBUFFERDESC         pdsbd
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();

    //
    // Hand off to the base class
    //

    CDirectSoundVoiceSettings::Initialize(pdsbd->dwFlags, (pdsbd->dwFlags & DSBCAPS_SUBMIXMASK) ? &m_wfxMixDest : pdsbd->lpwfxFormat, pdsbd->lpMixBins);

    //
    // Save the input mixbin
    //

    if(pdsbd->dwFlags & DSBCAPS_FXIN)
    {
        m_dwInputMixBin = pdsbd->dwInputMixBin;
    }
    else if(pdsbd->dwFlags & DSBCAPS_MIXIN)
    {
        m_dwInputMixBin = DSMIXBIN_SUBMIX;
    }
    
    //
    // Initialize buffer data
    //

    if(pdsbd->dwBufferBytes)
    {
        hr = SetBufferData(NULL, pdsbd->dwBufferBytes);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetBufferData
 *
 *  Description:
 *      Sets buffer data pointers.
 *
 *  Arguments:
 *      LPVOID [in]: data buffer.
 *      LPVOID [in]: buffer size, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBufferSettings::SetBufferData"

HRESULT
CDirectSoundBufferSettings::SetBufferData
(
    LPVOID                  pvDataBuffer,
    DWORD                   dwBufferBytes
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();
    
    //
    // Free the existing buffer
    //

    if(m_dwFlags & DSBCAPS_APPALLOCBUFFER)
    {
        m_pvBufferData = NULL;
    }
    else
    {
        MEMFREE(m_pvBufferData);
    }

    //
    // Allocate the new buffer (or just save the pointer)
    //

    if(m_dwBufferSize = dwBufferBytes)
    {
        if(pvDataBuffer)
        {
            m_dwFlags |= DSBCAPS_APPALLOCBUFFER;

            m_pvBufferData = pvDataBuffer;
        }
        else
        {
            m_dwFlags &= ~DSBCAPS_APPALLOCBUFFER;
        
            hr = HRFROMP(m_pvBufferData = MEMALLOC_NOINIT(BYTE, m_dwBufferSize));
        }
    }

    //
    // Reset the buffer offsets
    //

    SetBufferOffsets(0, m_dwBufferSize, 0, m_dwBufferSize);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetBufferOffsets
 *
 *  Description:
 *      Sets buffer play and loop regions.
 *
 *  Arguments:
 *      DWORD [in]: starting position of the play region.
 *      DWORD [in]: length, in bytes, of the play region.
 *      DWORD [in]: starting position of the loop region.
 *      DWORD [in]: length, in bytes, of the loop region.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBufferSettings::SetBufferOffsets"

void
CDirectSoundBufferSettings::SetBufferOffsets
(
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength,
    DWORD                   dwLoopStart,
    DWORD                   dwLoopLength
)
{
    DPF_ENTER();

    ASSERT(dwPlayStart + dwPlayLength <= m_dwBufferSize);
    ASSERT(dwLoopStart + dwLoopLength <= m_dwBufferSize);
    ASSERT(dwLoopStart + dwLoopLength <= dwPlayLength);
    
    m_dwPlayStart = dwPlayStart;
    m_dwPlayLength = dwPlayLength;
    m_dwLoopStart = dwLoopStart;
    m_dwLoopLength = dwLoopLength;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetNotificationPositions
 *
 *  Description:
 *      Sets notification positions.
 *
 *  Arguments:
 *      DWORD [in]: notification count.
 *      LPCDSBPOSITIONNOTIFY [in]: notification array.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBufferSettings::SetNotificationPositions"

HRESULT
CDirectSoundBufferSettings::SetNotificationPositions
(
    DWORD                   dwNotifyCount,
    LPCDSBPOSITIONNOTIFY    paNotifies
)
{
    HRESULT                 hr      = DS_OK;
    DWORD                   i       = 0;
    DSBPOSITIONNOTIFY       NotifyT;

    DPF_ENTER();

    //
    // Free any existing notifications
    //

    MEMFREE(m_paNotifies);

    //
    // Save the new notification count
    //

    m_dwNotifyCount = dwNotifyCount;

    //
    // Make a copy of the notification array and put it into ascending order
    //

    if(dwNotifyCount)
    {
        hr = HRFROMP(m_paNotifies = MEMALLOC_NOINIT(DSBPOSITIONNOTIFY, dwNotifyCount));

        if(SUCCEEDED(hr))
        {
            CopyMemory(m_paNotifies, paNotifies, sizeof(*paNotifies) * dwNotifyCount);
        }

        if(SUCCEEDED(hr))
        {
            while(i < m_dwNotifyCount - 1)
            {
                if(m_paNotifies[i].dwOffset > m_paNotifies[i + 1].dwOffset)
                {
                    NotifyT = m_paNotifies[i];
                    m_paNotifies[i] = m_paNotifies[i + 1];
                    m_paNotifies[i + 1] = NotifyT;

                    i = 0;
                }
                else
                {
                    i++;
                }
            }
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  CDirectSoundBuffer
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CDirectSound * [in]: parent DirectSound object
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::CDirectSoundBuffer"

CDirectSoundBuffer::CDirectSoundBuffer
(
    CDirectSound *      pDirectSound
)
:   CDirectSoundVoice(pDirectSound)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CDirectSoundBuffer
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::~CDirectSoundBuffer"

CDirectSoundBuffer::~CDirectSoundBuffer
(
    void
)
{
    DPF_ENTER();

    //
    // Free the buffer implementation
    //

    RELEASE(m_pBuffer);

    //
    // Free settings
    //

    RELEASE(m_pSettings);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      LPCDSBUFFERDESC [in]: buffer description.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::Initialize"

HRESULT
CDirectSoundBuffer::Initialize
(
    LPCDSBUFFERDESC         pdsbd
)
{                                   
    HRESULT                 hr;

#ifdef VALIDATE_PARAMETERS

    DWORD                   i;

#endif // VALIDATE_PARAMETERS

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
#ifdef VALIDATE_PARAMETERS

    if(!pdsbd)
    {
        DPF_ERROR("DSBUFFERDESC not supplied");
    }

    if(sizeof(*pdsbd) < pdsbd->dwSize)
    {
        DPF_ERROR("DSBUFFERDESC size not valid");
    }

    if(pdsbd->dwFlags & ~DSBCAPS_VALID)
    {
        DPF_ERROR("Specified invalid flags (%lx)", pdsbd->dwFlags & ~DSBCAPS_VALID);
    }

    if(pdsbd->dwFlags & DSBCAPS_SUBMIXMASK)
    {
        if(pdsbd->dwFlags & DSBCAPS_LOCDEFER)
        {
            DPF_ERROR("MIXIN/FXIN buffers can't be LOCDEFER");
        }

        if(pdsbd->dwFlags & DSBCAPS_CTRLPOSITIONNOTIFY)
        {
            DPF_ERROR("MIXIN/FXIN buffers can't have CTRLPOSITIONNOTIFY");
        }

        if(pdsbd->lpwfxFormat)
        {
            DPF_ERROR("lpwfxFormat for MIXIN/FXIN buffers must be NULL");
        }

        if(pdsbd->dwBufferBytes)
        {
            DPF_ERROR("dwBufferBytes for MIXIN/FXIN buffers must be 0");
        }
    }
    else
    {
        if(!pdsbd->lpwfxFormat)
        {
            DPF_ERROR("Failed to specify a buffer format");
        }

        if(!FmtIsValid(pdsbd->lpwfxFormat))
        {
            DPF_ERROR("Invalid buffer format");
        }

        if(pdsbd->dwBufferBytes && ((pdsbd->dwBufferBytes < DSBSIZE_MIN) || (pdsbd->dwBufferBytes > DSBSIZE_MAX)))
        {
            DPF_ERROR("Buffer size out-of-bounds");
        }
    
        if(pdsbd->dwBufferBytes && (pdsbd->dwBufferBytes % pdsbd->lpwfxFormat->nBlockAlign))
        {
            DPF_ERROR("Buffer size not a multiple of the sample size");
        }

        if((pdsbd->dwFlags & DSBCAPS_CTRL3D) && (1 != pdsbd->lpwfxFormat->nChannels))
        {
            DPF_ERROR("3D voices must be mono");
        }
    }

    if((pdsbd->dwFlags & DSBCAPS_CTRL3D) && !CHrtfSource::IsValidHrtfQuality())
    {
        DPF_ERROR("You must call DirectSoundUseLight/FullHRTF before creating a 3D buffer");
    }

    if(pdsbd->lpMixBins)
    {
        if(pdsbd->lpMixBins->dwMixBinCount > DSMIXBIN_ASSIGNMENT_MAX)
        {
            DPF_ERROR("No voice can be assigned to more than %lu mix bins", DSMIXBIN_ASSIGNMENT_MAX);
        }

        if(pdsbd->lpwfxFormat)
        {
            if(pdsbd->lpMixBins->dwMixBinCount % pdsbd->lpwfxFormat->nChannels)
            {
                DPF_ERROR("The mixbin count must be a multiple of the channel count");
            }

            if(WAVE_FORMAT_EXTENSIBLE == pdsbd->lpwfxFormat->wFormatTag)
            {
                if(((LPCWAVEFORMATEXTENSIBLE)pdsbd->lpwfxFormat)->dwChannelMask)
                {
                    DPF_WARNING("Specifying mixbins will override the channel mask specified in the format");
                }
            }
        }

        for(i = 0; i < pdsbd->lpMixBins->dwMixBinCount; i++)
        {
            if(pdsbd->lpMixBins->lpMixBinVolumePairs[i].dwMixBin > DSMIXBIN_LAST)
            {
                DPF_ERROR("Specified an invalid mixbin");
            }

            if(DSMIXBIN_SUBMIX == pdsbd->lpMixBins->lpMixBinVolumePairs[i].dwMixBin)
            {
                DPF_ERROR("Don't assign a voice to DSMIXBIN_SUBMIX.  Instead, call SetOutputBuffer");
            }
        }

        if(pdsbd->dwFlags & DSBCAPS_CTRL3D)
        {
            if(pdsbd->lpMixBins->dwMixBinCount < DirectSoundRequiredMixBins_3D.dwMixBinCount)
            {
                DPF_ERROR("MixBin count doesn't include enough mixbins to hold all required for 3D");
            }
            
            for(i = 0; i < DirectSoundRequiredMixBins_3D.dwMixBinCount; i++)
            {
                if(pdsbd->lpMixBins->lpMixBinVolumePairs[i].dwMixBin != DirectSoundRequiredMixBins_3D.lpMixBinVolumePairs[i].dwMixBin)
                {
                    DPF_ERROR("MixBin array does not include the required 3D bins");
                }
            }
        }
    }

    if(pdsbd->dwFlags & DSBCAPS_FXIN)
    {
        if((pdsbd->dwInputMixBin < DSMIXBIN_FXSEND_FIRST) || (pdsbd->dwInputMixBin > DSMIXBIN_FXSEND_LAST))
        {
            DPF_ERROR("The input mixbin for an FXIN buffer can only be one of the FX sends");
        }
    }

#endif // VALIDATE_PARAMETERS

    //
    // Create the settings object
    //

    hr = HRFROMP(m_pSettings = NEW(CDirectSoundBufferSettings));

    if(SUCCEEDED(hr))
    {
        hr = m_pSettings->Initialize(pdsbd);
    }

    //
    // Create the implementation object
    //
    
    if(SUCCEEDED(hr))
    {
        hr = HRFROMP(m_pBuffer = NEW(CMcpxBuffer(m_pDirectSound->m_pDevice, m_pSettings)));
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pBuffer->Initialize();
    }

    //
    // Initialize the base class
    //

    if(SUCCEEDED(hr))
    {
        CDirectSoundVoice::Initialize(m_pBuffer, m_pSettings);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetBufferData
 *
 *  Description:
 *      Sets buffer data pointers.
 *
 *  Arguments:
 *      LPVOID [in]: data buffer.
 *      LPVOID [in]: buffer size, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::SetBufferData"

HRESULT
CDirectSoundBuffer::SetBufferData
(
    LPVOID                  pvDataBuffer,
    DWORD                   dwBufferBytes
)
{
    const BOOL              fChanging   = pvDataBuffer || dwBufferBytes;
    HRESULT                 hr          = DS_OK;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
#ifdef VALIDATE_PARAMETERS

    if(pvDataBuffer && dwBufferBytes)
    {
        if(dwBufferBytes && (dwBufferBytes % m_pSettings->m_fmt.nBlockAlign))
        {
            DPF_ERROR("Buffer size not block-aligned");
        }

        if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
        {
            DPF_ERROR("Can't SetBufferData on MIXIN/FXIN buffers");
        }
    }
    else if(pvDataBuffer)
    {
        DPF_ERROR("0-sized buffer with non-NULL buffer pointer");
    }
    else if(dwBufferBytes)
    {
        DPF_ERROR("NULL buffer with non-zero buffer size");
    }

#endif // VALIDATE_PARAMETERS

    if((pvDataBuffer != m_pSettings->m_pvBufferData) || (dwBufferBytes != m_pSettings->m_dwBufferSize))
    {
        //
        // Give the implementation object a chance to clean up before we
        // stomp on the shared data members.
        //

        hr = m_pBuffer->ReleaseBufferData(fChanging);

        //
        // Hand off to the settings object
        //

        if(SUCCEEDED(hr))
        {
            hr = m_pSettings->SetBufferData(pvDataBuffer, dwBufferBytes);
        }

        //
        // Notify the implementation object of the change
        //

        if(SUCCEEDED(hr) && fChanging)
        {
            hr = m_pBuffer->SetBufferData();
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Play
 *
 *  Description:
 *      Starts the buffer playing.
 *
 *  Arguments:
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::Play"

HRESULT
CDirectSoundBuffer::Play
(
    DWORD                   dwReserved1,
    DWORD                   dwReserved2,
    DWORD                   dwFlags
)
{
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
#ifdef VALIDATE_PARAMETERS

    if(dwReserved1 || dwReserved2)
    {
        DPF_ERROR("Reserved parameters must be 0");
    }
    
    if(dwFlags & ~DSBPLAY_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DSBPLAY_VALID);
    }

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't call Play on a MIXIN/FXIN buffer");
    }
    
    if(!m_pSettings->m_pvBufferData || !m_pSettings->m_dwBufferSize)
    {
        DPF_ERROR("No data to play!");
    }

#endif // VALIDATE_PARAMETERS

    hr = m_pBuffer->Play(MAKEBOOL(dwFlags & DSBPLAY_LOOPING));

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  PlayEx
 *
 *  Description:
 *      Starts the buffer playing.
 *
 *  Arguments:
 *      REFERENCE_TIME [in]: time to start playing.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::PlayEx"

HRESULT
CDirectSoundBuffer::PlayEx
(
    REFERENCE_TIME          rtTimeStamp,
    DWORD                   dwFlags
)
{
    HRESULT                 hr;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
#ifdef VALIDATE_PARAMETERS

    if(dwFlags & ~DSBPLAY_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DSBPLAY_VALID);
    }

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't call Play on a MIXIN/FXIN buffer");
    }
    
    if(!m_pSettings->m_pvBufferData || !m_pSettings->m_dwBufferSize)
    {
        DPF_ERROR("No data to play!");
    }

#endif // VALIDATE_PARAMETERS

    hr = m_pBuffer->Play(rtTimeStamp, MAKEBOOL(dwFlags & DSBPLAY_LOOPING));

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Stop
 *
 *  Description:
 *      Stops buffer playback.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::Stop"

HRESULT
CDirectSoundBuffer::Stop
(
    void
)
{
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
#ifdef VALIDATE_PARAMETERS

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't call Stop on a MIXIN/FXIN buffer");
    }
    
#endif // VALIDATE_PARAMETERS

    hr = m_pBuffer->Stop();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  StopEx
 *
 *  Description:
 *      Stops buffer playback.
 *
 *  Arguments:
 *      REFERENCE_TIME [in]: timestamp.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::StopEx"

HRESULT
CDirectSoundBuffer::StopEx
(
    REFERENCE_TIME          rtTimeStamp,
    DWORD                   dwFlags
)
{
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
#ifdef VALIDATE_PARAMETERS

    if(dwFlags & ~DSBSTOPEX_VALID)
    {
        DPF_ERROR("Invalid flags");
    }

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't call Stop on a MIXIN/FXIN buffer");
    }
    
#endif // VALIDATE_PARAMETERS

    if(dwFlags & DSBSTOPEX_VALID)
    {
        hr = m_pBuffer->NoteOff(rtTimeStamp, MAKEBOOL(dwFlags & DSBSTOPEX_RELEASEWAVEFORM));
    }
    else
    {
        hr = m_pBuffer->Stop(rtTimeStamp);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetStatus
 *
 *  Description:
 *      Gets the current buffer status.
 *
 *  Arguments:
 *      LPDWORD [out]: buffer status.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::GetStatus"

HRESULT
CDirectSoundBuffer::GetStatus
(
    LPDWORD                 pdwStatus
)
{
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pdwStatus)
    {
        DPF_ERROR("Failed to specify a status buffer");
    }

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't call GetStatus on a MIXIN/FXIN buffer");
    }
    
#endif // VALIDATE_PARAMETERS
    
    hr = m_pBuffer->GetStatus(pdwStatus);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetBufferOffsets
 *
 *  Description:
 *      Sets buffer play and loop regions.
 *
 *  Arguments:
 *      DWORD [in]: starting position of the play region.
 *      DWORD [in]: length, in bytes, of the play region.
 *      DWORD [in]: starting position of the loop region.
 *      DWORD [in]: length, in bytes, of the loop region.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::SetBufferOffsets"

HRESULT
CDirectSoundBuffer::SetBufferOffsets
(
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength,
    DWORD                   dwLoopStart,
    DWORD                   dwLoopLength
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't SetBufferOffsets on MIXIN/FXIN buffers");
    }

    if(dwPlayStart % m_pSettings->m_fmt.nBlockAlign)
    {
        DPF_ERROR("Play offset not block-aligned");
    }

    if(dwLoopStart % m_pSettings->m_fmt.nBlockAlign)
    {
        DPF_ERROR("Loop offset not block-aligned");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Munge the lengths so that they're all valid
    //

    if(dwPlayLength)
    {

#ifdef VALIDATE_PARAMETERS

        if(dwPlayStart + dwPlayLength > m_pSettings->m_dwBufferSize)
        {
            DPF_ERROR("Play region extends past the end of the buffer");
        }
    
        if(dwPlayLength % m_pSettings->m_fmt.nBlockAlign)
        {
            DPF_ERROR("Play length not block-aligned");
        }

#endif // VALIDATE_PARAMETERS

    }
    else
    {
        dwPlayLength = m_pSettings->m_dwBufferSize - dwPlayStart;
    }

    if(dwLoopLength)
    {

#ifdef VALIDATE_PARAMETERS

        if(dwLoopStart + dwLoopLength > dwPlayLength)
        {
            DPF_ERROR("Loop region extends past the end of the play region");
        }
    
        if(dwLoopLength % m_pSettings->m_fmt.nBlockAlign)
        {
            DPF_ERROR("Loop length not block-aligned");
        }

#endif // VALIDATE_PARAMETERS

    }
    else
    {
        dwLoopLength = dwPlayLength - dwLoopStart;
    }

    //
    // Update the settings object
    //

    m_pSettings->SetBufferOffsets(dwPlayStart, dwPlayLength, dwLoopStart, dwLoopLength);

    //
    // Update the buffer implementation
    //

    hr = m_pBuffer->SetBufferOffsets();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetCurrentPosition
 *
 *  Description:
 *      Gets the current play and write cursor positions.
 *
 *  Arguments:
 *      LPDWORD [out]: play cursor position, in bytes.
 *      LPDWORD [out]: write cursor position, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::GetCurrentPosition"

HRESULT
CDirectSoundBuffer::GetCurrentPosition
(
    LPDWORD                 pdwPlayPosition,
    LPDWORD                 pdwWritePosition
)
{
    HRESULT                 hr;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
#ifdef VALIDATE_PARAMETERS

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't GetCurrentPosition on MIXIN/FXIN buffers");
    }

#endif // VALIDATE_PARAMETERS

    hr = m_pBuffer->GetCurrentPosition(pdwPlayPosition, pdwWritePosition);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Lock
 *
 *  Description:
 *      Locks the buffer data for writing.
 *
 *  Arguments:
 *      DWORD [in]: lock position.
 *      DWORD [in]: lock size.
 *      LPVOID * [out]: lock pointer 1.
 *      LPDWORD [out]: lock size 1.
 *      LPVOID * [out]: lock pointer 2.
 *      LPDWORD [out]: lock size 2.
 *      DWORD [in]: flags.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::Lock"

HRESULT
CDirectSoundBuffer::Lock
(
    DWORD                   dwLockPosition, 
    DWORD                   dwLockSize, 
    LPVOID *                ppvLock1, 
    LPDWORD                 pdwLockSize1, 
    LPVOID *                ppvLock2, 
    LPDWORD                 pdwLockSize2, 
    DWORD                   dwFlags
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!ppvLock1 || !pdwLockSize1)
    {
        DPF_ERROR("Missing first set of lock parameters");
    }

    if((ppvLock2 && !pdwLockSize2) || (!ppvLock2 && pdwLockSize2))
    {
        DPF_ERROR("Missing one of the second set of lock parameters");
    }

    if(dwFlags & ~DSBLOCK_VALID)
    {
        DPF_ERROR("Specified unsupported or invalid flags (%lx)", dwFlags & ~DSBLOCK_VALID);
    }

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't Lock MIXIN/FXIN buffers");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Validate the lock position
    //

    if(dwFlags & DSBLOCK_FROMWRITECURSOR)
    {
        hr = GetCurrentPosition(NULL, &dwLockPosition);
    }
    
#ifdef VALIDATE_PARAMETERS

    else if(dwLockPosition >= m_pSettings->m_dwBufferSize)
    {
        DPF_ERROR("Lock position out-of-bounds");
    }
    else if(dwLockPosition % m_pSettings->m_fmt.nBlockAlign)
    {
        DPF_ERROR("Lock position not block aligned");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Validate the lock region size
    //

    if(SUCCEEDED(hr))
    {
        if(dwFlags & DSBLOCK_ENTIREBUFFER)
        {
            dwLockSize = m_pSettings->m_dwBufferSize;
        }
            
#ifdef VALIDATE_PARAMETERS

        else if(!dwLockSize)
        {
            DPF_ERROR("Lock size can't be 0 without specifying DSBLOCK_ENTIREBUFFER");
        }
        else if(dwLockSize > m_pSettings->m_dwBufferSize)
        {
            DPF_ERROR("Lock size out-of-bounds");
        }
        else if(dwLockSize % m_pSettings->m_fmt.nBlockAlign)
        {
            DPF_ERROR("Lock size not block aligned");
        }

#endif // VALIDATE_PARAMETERS

    }

    //
    // We're not really locking anything.  Instead, we're just returning
    // pointers to the data buffer.
    //

    if(SUCCEEDED(hr))
    {
        *ppvLock1 = (LPBYTE)m_pSettings->m_pvBufferData + dwLockPosition;
        *pdwLockSize1 = min(dwLockSize, m_pSettings->m_dwBufferSize - dwLockPosition);

        if(ppvLock2 && pdwLockSize2)
        {
            if(*pdwLockSize1 < dwLockSize)
            {
                *ppvLock2 = m_pSettings->m_pvBufferData;
                *pdwLockSize2 = dwLockSize - *pdwLockSize1;
            }
            else
            {
                *ppvLock2 = NULL;
                *pdwLockSize2 = 0;
            }
        }
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetCurrentPosition
 *
 *  Description:
 *      Gets the current play cursor position.
 *
 *  Arguments:
 *      DWORD [in]: play cursor position, in bytes.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::SetCurrentPosition"

HRESULT
CDirectSoundBuffer::SetCurrentPosition
(
    DWORD                   dwPlayPosition
)
{
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
#ifdef VALIDATE_PARAMETERS

    if(dwPlayPosition % m_pSettings->m_fmt.nBlockAlign)
    {
        DPF_ERROR("Position not sample-aligned");
    }
    
    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't SetCurrentPosition on MIXIN/FXIN buffers");
    }

    if(dwPlayPosition >= m_pSettings->m_dwPlayLength)
    {
        DPF_ERROR("Play cursor position out-of-bounds");
    }
    
#endif // VALIDATE_PARAMETERS

    hr = m_pBuffer->SetCurrentPosition(dwPlayPosition);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetNotificationPositions
 *
 *  Description:
 *      Sets notification positions.
 *
 *  Arguments:
 *      DWORD [in]: notification count.
 *      LPCDSBPOSITIONNOTIFY [in]: notification array.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::SetNotificationPositions"

HRESULT
CDirectSoundBuffer::SetNotificationPositions
(
    DWORD                   dwNotifyCount,
    LPCDSBPOSITIONNOTIFY    paNotifies
)
{
    HRESULT                 hr  = DS_OK;

#ifdef VALIDATE_PARAMETERS

    DWORD                   i;

#endif // VALIDATE_PARAMETERS

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRLPOSITIONNOTIFY))
    {
        DPF_ERROR("Buffer does not have CTRLPOSITIONNOTIFY");
    }

    for(i = 0; i < dwNotifyCount; i++)
    {
        if(DSBPN_OFFSETSTOP == paNotifies[i].dwOffset)
        {
            continue;
        }
        
        if(paNotifies[i].dwOffset % m_pSettings->m_fmt.nBlockAlign)
        {
            DPF_ERROR("Offset at index %lu not block-aligned", i);
        }

        if(paNotifies[i].dwOffset >= m_pSettings->m_dwPlayLength)
        {
            DPF_ERROR("Offset at index %lu past the end of the play region", i);
        }
        
        if(!IS_VALID_HANDLE_VALUE(paNotifies[i].hEventNotify))
        {
            DPF_ERROR("Invalid event at index %lu", i);
        }
    }

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't SetNotificationPositions on MIXIN/FXIN buffers");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Update the settings object
    //

    hr = m_pSettings->SetNotificationPositions(dwNotifyCount, paNotifies);

    //
    // Update the implementation object
    //

    if(SUCCEEDED(hr))
    {
        hr = m_pBuffer->SetNotificationPositions();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


#ifndef MCPX_BOOT_LIB

/****************************************************************************
 *
 *  CDirectSoundStreamSettings
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStreamSettings::CDirectSoundStreamSettings"

CDirectSoundStreamSettings::CDirectSoundStreamSettings
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CDirectSoundStreamSettings
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStreamSettings::~CDirectSoundStreamSettings"

CDirectSoundStreamSettings::~CDirectSoundStreamSettings
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      LPCDSSTREAMDESC [in]: stream description.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStreamSettings::Initialize"

HRESULT
CDirectSoundStreamSettings::Initialize
(
    LPCDSSTREAMDESC         pdssd
)
{
    DPF_ENTER();

    //
    // Hand off to the base class
    //

    CDirectSoundVoiceSettings::Initialize(pdssd->dwFlags, pdssd->lpwfxFormat, pdssd->lpMixBins);

    //
    // Save stream-specific settings
    //

    m_dwMaxAttachedPackets = pdssd->dwMaxAttachedPackets;
    m_pfnCallback = pdssd->lpfnCallback;
    m_pvContext = pdssd->lpvContext;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  CDirectSoundStream
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CDirectSound * [in]: parent DirectSound object
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStream::CDirectSoundStream"

CDirectSoundStream::CDirectSoundStream
(
    CDirectSound *      pDirectSound
)
:   CDirectSoundVoice(pDirectSound)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CDirectSoundStream
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStream::~CDirectSoundStream"

CDirectSoundStream::~CDirectSoundStream
(
    void
)
{
    DPF_ENTER();

    //
    // Free the stream implementation
    //

    RELEASE(m_pStream);

    //
    // Free settings
    //

    RELEASE(m_pSettings);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      LPCDSSTREAMDESC [in]: stream description.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStream::Initialize"

HRESULT
CDirectSoundStream::Initialize
(
    LPCDSSTREAMDESC         pdssd
)
{                                   
    HRESULT                 hr;

#ifdef VALIDATE_PARAMETERS

    DWORD                   i;

#endif // VALIDATE_PARAMETERS

    DPF_ENTER();
    
#ifdef VALIDATE_PARAMETERS

    if(!pdssd)
    {
        DPF_ERROR("DSSTREAMDESC not supplied");
    }

    if(pdssd->dwFlags & ~DSSTREAMCAPS_VALID)
    {
        DPF_ERROR("Specified invalid flags (%lx)", pdssd->dwFlags & ~DSSTREAMCAPS_VALID);
    }
        
    if(!pdssd->lpwfxFormat)
    {
        DPF_ERROR("Failed to specify a stream format");
    }

    if(!FmtIsValid(pdssd->lpwfxFormat))
    {
        DPF_ERROR("Invalid stream format");
    }

    if((pdssd->dwFlags & DSBCAPS_CTRL3D) && (1 != pdssd->lpwfxFormat->nChannels))
    {
        DPF_ERROR("3D streams must be mono");
    }

    if((pdssd->dwFlags & DSSTREAMCAPS_CTRL3D) && !CHrtfSource::IsValidHrtfQuality())
    {
        DPF_ERROR("You must call DirectSoundUseLight/FullHrtf before creating a 3D stream");
    }

    if(pdssd->lpMixBins)
    {
        if(pdssd->lpMixBins->dwMixBinCount > DSMIXBIN_ASSIGNMENT_MAX)
        {
            DPF_ERROR("No voice can be assigned to more than %lu mix bins", DSMIXBIN_ASSIGNMENT_MAX);
        }

        if(pdssd->lpwfxFormat)
        {
            if(pdssd->lpMixBins->dwMixBinCount % pdssd->lpwfxFormat->nChannels)
            {
                DPF_ERROR("The mixbin count must be a multiple of the channel count");
            }

            if(WAVE_FORMAT_EXTENSIBLE == pdssd->lpwfxFormat->wFormatTag)
            {
                if(((LPCWAVEFORMATEXTENSIBLE)pdssd->lpwfxFormat)->dwChannelMask)
                {
                    DPF_WARNING("Specifying mixbins will override the channel mask specified in the format");
                }
            }
        }

        for(i = 0; i < pdssd->lpMixBins->dwMixBinCount; i++)
        {
            if(pdssd->lpMixBins->lpMixBinVolumePairs[i].dwMixBin > DSMIXBIN_LAST)
            {
                DPF_ERROR("Specified an invalid mixbin");
            }

            if(DSMIXBIN_SUBMIX == pdssd->lpMixBins->lpMixBinVolumePairs[i].dwMixBin)
            {
                DPF_ERROR("Don't assign a voice to DSMIXBIN_SUBMIX.  Instead, call SetOutputBuffer");
            }
        }

        if(pdssd->dwFlags & DSBCAPS_CTRL3D)
        {
            if(pdssd->lpMixBins->dwMixBinCount < DirectSoundRequiredMixBins_3D.dwMixBinCount)
            {
                DPF_ERROR("MixBin count doesn't include enough mixbins to hold all required for 3D");
            }
            
            for(i = 0; i < DirectSoundRequiredMixBins_3D.dwMixBinCount; i++)
            {
                if(pdssd->lpMixBins->lpMixBinVolumePairs[i].dwMixBin != DirectSoundRequiredMixBins_3D.lpMixBinVolumePairs[i].dwMixBin)
                {
                    DPF_ERROR("MixBin array does not include the required 3D bins");
                }
            }
        }
    }

    if(!pdssd->dwMaxAttachedPackets)
    {
        DPF_ERROR("dwMaxAttachedPackets must be at least 1");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Create the settings object
    //

    hr = HRFROMP(m_pSettings = NEW(CDirectSoundStreamSettings));

    if(SUCCEEDED(hr))
    {
        hr = m_pSettings->Initialize(pdssd);
    }

    //
    // Create the implementation object
    //
    
    if(SUCCEEDED(hr))
    {
        hr = HRFROMP(m_pStream = NEW(CMcpxStream(m_pDirectSound->m_pDevice, m_pSettings)));
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pStream->Initialize();
    }

    //
    // Initialize the base class
    //

    if(SUCCEEDED(hr))
    {
        CDirectSoundVoice::Initialize(m_pStream, m_pSettings);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetInfo
 *
 *  Description:
 *      Gets information about the data the object supports.
 *
 *  Arguments:
 *      LPXMEDIAINFO [in/out]: info data.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStream::GetInfo"

HRESULT
CDirectSoundStream::GetInfo
(
    LPXMEDIAINFO            pInfo
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pInfo)
    {
        DPF_ERROR("No XMEDIAINFO buffer supplied");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Flags are easy
    //
    
    pInfo->dwFlags = XMO_STREAMF_WHOLE_SAMPLES | XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC;

    //
    // The mixer will break up if you queue up less than the mix buffer size
    //

    pInfo->dwMaxLookahead = m_pStream->GetLowWatermark();

    //
    // Input size is just sample-aligment, since that's the smallest amount of
    // data we require to process.
    //

    pInfo->dwInputSize = m_pSettings->m_fmt.nBlockAlign;

    //
    // There is no output
    //

    pInfo->dwOutputSize = 0;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Discontinuity
 *
 *  Description:
 *      Signals a discontinuity in the stream data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStream::Discontinuity"

HRESULT
CDirectSoundStream::Discontinuity
(
    void
)                                       
{                                           
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
    hr = m_pStream->Discontinuity();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Flush
 *
 *  Description:
 *      Resets the stream to it's default state.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStream::Flush"

HRESULT
CDirectSoundStream::Flush
(
    void
)                                       
{                                           
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
    hr = m_pStream->Flush();

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetStatus
 *
 *  Description:
 *      Gets stream status.
 *
 *  Arguments:
 *      LPDWORD [out]: stream status.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStream::GetStatus"

HRESULT
CDirectSoundStream::GetStatus
(
    LPDWORD                 pdwStatus
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pdwStatus)
    {
        DPF_ERROR("Status buffer not supplied");
    }

#endif // VALIDATE_PARAMETERS

    if(m_pStream->IsReady())
    {
        *pdwStatus = XMO_STATUSF_ACCEPT_INPUT_DATA;
    }
    else
    {
        *pdwStatus = 0;
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Process
 *
 *  Description:
 *      Submits buffers to the stream.
 *
 *  Arguments:
 *      LPXMEDIAPACKET  [in]: input buffer.
 *      LPXMEDIAPACKET  [in]: output buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStream::Process"

HRESULT
CDirectSoundStream::Process
(
    LPCXMEDIAPACKET         pxmbSource, 
    LPCXMEDIAPACKET         pxmbDest 
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pxmbSource)
    {
        DPF_ERROR("No input buffer supplied");
    }

    if(!pxmbSource->pvBuffer)
    {
        DPF_ERROR("No input data buffer supplied");
    }

    if(!pxmbSource->dwMaxSize)
    {
        DPF_ERROR("No input buffer size");
    }

    if(pxmbSource->dwMaxSize % m_pSettings->m_fmt.nBlockAlign)
    {
        DPF_ERROR("Input buffer size not block-aligned");
    }

    if(pxmbDest)
    {
        DPF_ERROR("DirectSound streams are input only.  No destination packet should be supplied.");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Make sure the stream is accepting data
    //

    if(!m_pStream->IsReady())
    {
        DPF_ERROR("The stream is not ready to accept more data");
        hr = DSERR_INVALIDCALL;
    }
    
    //
    // Submit the packet to the stream
    //

    if(SUCCEEDED(hr))
    {
        hr = m_pStream->SubmitPacket(*pxmbSource);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Pause
 *
 *  Description:
 *      Pauses or resumes a stream.
 *
 *  Arguments:
 *      DWORD [in]: stream pause state.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStream::Pause"

HRESULT
CDirectSoundStream::Pause
(
    DWORD                   dwPause
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if((dwPause < DSSTREAMPAUSE_FIRST) || (dwPause > DSSTREAMPAUSE_LAST))
    {
        DPF_ERROR("Invalid stream pause state");
    }

#endif // VALIDATE_PARAMETERS

    hr = m_pStream->Pause(dwPause);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#endif // MCPX_BOOT_LIB


