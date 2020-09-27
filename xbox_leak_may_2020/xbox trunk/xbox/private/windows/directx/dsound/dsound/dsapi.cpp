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

//
// Hacks to enable or disable certain API features
//

#ifdef DSAPI_DISABLE_SUBMIX
#pragma HACKHACK("Submix API currently disabled")
#endif // DSAPI_DISABLE_SUBMIX

#ifdef DSAPI_DISABLE_EFFECTS
#pragma HACKHACK("Effects API currently disabled")
#endif // DSAPI_DISABLE_EFFECTS

#ifdef DSAPI_DISABLE_LISTENER
#pragma HACKHACK("3D listener API currently disabled")
#endif // DSAPI_DISABLE_LISTENER

#ifdef DSAPI_DISABLE_3DSOURCE
#pragma HACKHACK("3D source API currently disabled")
#endif // DSAPI_DISABLE_3DSOURCE

#ifdef DSAPI_DISABLE_MIXBINS
#pragma HACKHACK("MixBin API currently disabled")
#endif // DSAPI_DISABLE_MIXBINS

#ifdef DSAPI_DISABLE_VOLUME
#pragma HACKHACK("Volume API currently disabled")
#endif // DSAPI_DISABLE_VOLUME

#ifdef DSAPI_DISABLE_PITCH
#pragma HACKHACK("Pitch/frequency API currently disabled")
#endif // DSAPI_DISABLE_PITCH

#ifdef DSAPI_DISABLE_FILTER
#pragma HACKHACK("DLS filter API currently disabled")
#endif // DSAPI_DISABLE_FILTER

#ifdef DSAPI_DISABLE_LFO
#pragma HACKHACK("LFO API currently disabled")
#endif // DSAPI_DISABLE_LFO

#ifdef DSAPI_DISABLE_EG
#pragma HACKHACK("EG API currently disabled")
#endif // DSAPI_DISABLE_EG

#ifdef DSAPI_DISABLE_NOTEOFF
#pragma HACKHACK("NoteOff API currently disabled")
#endif // DSAPI_DISABLE_NOTEOFF

//
// Merge segments
//

#pragma comment(linker, "/merge:DSOUND_RW=DSOUND")
#pragma comment(linker, "/merge:DSOUND_URW=DSOUND")
#pragma comment(linker, "/merge:DSOUND_RD=DSOUND")
#pragma comment(linker, "/section:DSOUND,ERW")

//
// Turn off floating-point normalization in retail builds
//

#ifndef VALIDATE_PARAMETERS

#define NormalizeFloat(a) (a)
#define NormalizeDouble(a) (a)

#endif // VALIDATE_PARAMETERS

//
// Method entry points
//

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

#ifndef MCPX_BOOT_LIB

STDAPI IDirectSound_CreateSoundStream(LPDIRECTSOUND pDirectSound, LPCDSSTREAMDESC pdssd, LPDIRECTSOUNDSTREAM *ppStream, LPUNKNOWN pUnkOuter)
{
    return ((CDirectSound *)pDirectSound)->CreateSoundStream(pdssd, ppStream, pUnkOuter);
}

#endif // MCPX_BOOT_LIB

STDAPI IDirectSound_GetSpeakerConfig(LPDIRECTSOUND pDirectSound, LPDWORD pdwSpeakerConfig)
{
    return ((CDirectSound *)pDirectSound)->GetSpeakerConfig(pdwSpeakerConfig);
}

#ifndef MCPX_BOOT_LIB

STDAPI IDirectSound_DownloadEffectsImage(LPDIRECTSOUND pDirectSound, LPCVOID pvImageBuffer, DWORD dwImageSize, LPCDSEFFECTIMAGELOC pImageLoc, LPDSEFFECTIMAGEDESC *ppImageDesc)
{

#ifdef DSAPI_DISABLE_EFFECTS

    return DS_OK;

#else // DSAPI_DISABLE_EFFECTS

    return ((CDirectSound *)pDirectSound)->DownloadEffectsImage(pvImageBuffer, dwImageSize, pImageLoc, ppImageDesc);

#endif // DSAPI_DISABLE_EFFECTS
}

#endif // MCPX_BOOT_LIB

STDAPI IDirectSound_GetEffectData(LPDIRECTSOUND pDirectSound, DWORD dwEffectIndex, DWORD dwOffset, LPVOID pvData, DWORD dwDataSize)
{

#ifdef DSAPI_DISABLE_EFFECTS

    ZeroMemory(pvData, dwDataSize);

    return DS_OK;

#else // DSAPI_DISABLE_EFFECTS

    return ((CDirectSound *)pDirectSound)->GetEffectData(dwEffectIndex, dwOffset, pvData, dwDataSize);

#endif // DSAPI_DISABLE_EFFECTS

}

STDAPI IDirectSound_SetEffectData(LPDIRECTSOUND pDirectSound, DWORD dwEffectIndex, DWORD dwOffset, LPCVOID pvData, DWORD dwDataSize, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_EFFECTS

    return DS_OK;

#else // DSAPI_DISABLE_EFFECTS

    return ((CDirectSound *)pDirectSound)->SetEffectData(dwEffectIndex, dwOffset, pvData, dwDataSize, dwFlags);

#endif // DSAPI_DISABLE_EFFECTS

}

STDAPI IDirectSound_CommitEffectData(LPDIRECTSOUND pDirectSound)
{

#ifdef DSAPI_DISABLE_EFFECTS

    return DS_OK;

#else // DSAPI_DISABLE_EFFECTS

    return ((CDirectSound *)pDirectSound)->CommitEffectData();

#endif // DSAPI_DISABLE_EFFECTS

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

#ifndef MCPX_BOOT_LIB

STDAPI IDirectSound_SetAllParameters(LPDIRECTSOUND pDirectSound, LPCDS3DLISTENER pds3dl, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_LISTENER

    return DS_OK;

#else // DSAPI_DISABLE_LISTENER

    return ((CDirectSound *)pDirectSound)->SetAllParameters(pds3dl, dwFlags);

#endif // DSAPI_DISABLE_LISTENER

}

STDAPI IDirectSound_SetDistanceFactor(LPDIRECTSOUND pDirectSound, FLOAT flDistanceFactor, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_LISTENER

    return DS_OK;

#else // DSAPI_DISABLE_LISTENER

    return ((CDirectSound *)pDirectSound)->SetDistanceFactor(flDistanceFactor, dwFlags);

#endif // DSAPI_DISABLE_LISTENER

}

STDAPI IDirectSound_SetDopplerFactor(LPDIRECTSOUND pDirectSound, FLOAT flDopplerFactor, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_LISTENER

    return DS_OK;

#else // DSAPI_DISABLE_LISTENER

    return ((CDirectSound *)pDirectSound)->SetDopplerFactor(flDopplerFactor, dwFlags);

#endif // DSAPI_DISABLE_LISTENER

}

STDAPI IDirectSound_SetOrientation(LPDIRECTSOUND pDirectSound, FLOAT xFront, FLOAT yFront, FLOAT zFront, FLOAT xTop, FLOAT yTop, FLOAT zTop, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_LISTENER

    return DS_OK;

#else // DSAPI_DISABLE_LISTENER

    return ((CDirectSound *)pDirectSound)->SetOrientation(xFront, yFront, zFront, xTop, yTop, zTop, dwFlags);

#endif // DSAPI_DISABLE_LISTENER

}

STDAPI IDirectSound_SetPosition(LPDIRECTSOUND pDirectSound, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_LISTENER

    return DS_OK;

#else // DSAPI_DISABLE_LISTENER

    return ((CDirectSound *)pDirectSound)->SetPosition(x, y, z, dwFlags);

#endif // DSAPI_DISABLE_LISTENER

}

STDAPI IDirectSound_SetRolloffFactor(LPDIRECTSOUND pDirectSound, FLOAT flRolloffFactor, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_LISTENER

    return DS_OK;

#else // DSAPI_DISABLE_LISTENER

    return ((CDirectSound *)pDirectSound)->SetRolloffFactor(flRolloffFactor, dwFlags);

#endif // DSAPI_DISABLE_LISTENER

}

STDAPI IDirectSound_SetVelocity(LPDIRECTSOUND pDirectSound, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_LISTENER

    return DS_OK;

#else // DSAPI_DISABLE_LISTENER

    return ((CDirectSound *)pDirectSound)->SetVelocity(x, y, z, dwFlags);

#endif // DSAPI_DISABLE_LISTENER

}

STDAPI IDirectSound_CommitDeferredSettings(LPDIRECTSOUND pDirectSound)
{

#ifdef DSAPI_DISABLE_LISTENER

    return DS_OK;

#else // DSAPI_DISABLE_LISTENER

    return ((CDirectSound *)pDirectSound)->CommitDeferredSettings();

#endif // DSAPI_DISABLE_LISTENER

}

STDAPI IDirectSound_SetI3DL2Listener(LPDIRECTSOUND pDirectSound, LPCDSI3DL2LISTENER pds3dl, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_LISTENER

    return DS_OK;

#else // DSAPI_DISABLE_LISTENER

    return ((CDirectSound *)pDirectSound)->SetI3DL2Listener(pds3dl, dwFlags);

#endif // DSAPI_DISABLE_LISTENER

}

#endif // MCPX_BOOT_LIB

STDAPI IDirectSound_GetTime(LPDIRECTSOUND pDirectSound, REFERENCE_TIME *prtCurrent)
{
    return ((CDirectSound *)pDirectSound)->GetTime(prtCurrent);
}


STDAPI IDirectSound_GetOutputLevels(LPDIRECTSOUND pDirectSound, DSOUTPUTLEVELS *pOutputLevels, BOOL bReset)
{
    return ((CDirectSound *)pDirectSound)->GetOutputLevels(pOutputLevels,bReset);
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

#ifdef DSAPI_DISABLE_PITCH

    return DS_OK;

#else // DSAPI_DISABLE_PITCH

    return ((CDirectSoundBuffer *)pBuffer)->SetFrequency(dwFrequency);

#endif // DSAPI_DISABLE_PITCH

}

STDAPI IDirectSoundBuffer_SetVolume(LPDIRECTSOUNDBUFFER pBuffer, LONG lVolume)
{

#ifdef DSAPI_DISABLE_VOLUME

    return DS_OK;

#else // DSAPI_DISABLE_VOLUME

    return ((CDirectSoundBuffer *)pBuffer)->SetVolume(lVolume);

#endif // DSAPI_DISABLE_VOLUME

}

STDAPI IDirectSoundBuffer_SetPitch(LPDIRECTSOUNDBUFFER pBuffer, LONG lPitch)
{

#ifdef DSAPI_DISABLE_PITCH

    return DS_OK;

#else // DSAPI_DISABLE_PITCH

    return ((CDirectSoundBuffer *)pBuffer)->SetPitch(lPitch);

#endif // DSAPI_DISABLE_PITCH

}

STDAPI IDirectSoundBuffer_SetLFO(LPDIRECTSOUNDBUFFER pBuffer, LPCDSLFODESC pLFODesc)
{

#ifdef DSAPI_DISABLE_LFO

    return DS_OK;

#else // DSAPI_DISABLE_LFO

    return ((CDirectSoundBuffer *)pBuffer)->SetLFO(pLFODesc);

#endif // DSAPI_DISABLE_LFO

}

STDAPI IDirectSoundBuffer_SetEG(LPDIRECTSOUNDBUFFER pBuffer, LPCDSENVELOPEDESC pEnvelopeDesc)
{

#ifdef DSAPI_DISABLE_EG

    return DS_OK;

#else // DSAPI_DISABLE_EG

    return ((CDirectSoundBuffer *)pBuffer)->SetEG(pEnvelopeDesc);

#endif // DSAPI_DISABLE_EG

}

STDAPI IDirectSoundBuffer_SetFilter(LPDIRECTSOUNDBUFFER pBuffer, LPCDSFILTERDESC pFilter)
{

#ifdef DSAPI_DISABLE_FILTER

    return DS_OK;

#else // DSAPI_DISABLE_FILTER

    return ((CDirectSoundBuffer *)pBuffer)->SetFilter(pFilter);

#endif // DSAPI_DISABLE_FILTER

}

STDAPI IDirectSoundBuffer_SetHeadroom(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwHeadroom)
{

#ifdef DSAPI_DISABLE_VOLUME

    return DS_OK;

#else // DSAPI_DISABLE_VOLUME

    return ((CDirectSoundBuffer *)pBuffer)->SetHeadroom(dwHeadroom);

#endif // DSAPI_DISABLE_VOLUME

}

STDAPI IDirectSoundBuffer_SetOutputBuffer(LPDIRECTSOUNDBUFFER pBuffer, LPDIRECTSOUNDBUFFER pOutputBuffer)
{

#ifdef DSAPI_DISABLE_SUBMIX

    return DS_OK;

#else // DSAPI_DISABLE_SUBMIX

    return ((CDirectSoundBuffer *)pBuffer)->SetOutputBuffer(pOutputBuffer);

#endif // DSAPI_DISABLE_SUBMIX

}

STDAPI IDirectSoundBuffer_SetMixBins(LPDIRECTSOUNDBUFFER pBuffer, LPCDSMIXBINS pMixBins)
{

#ifdef DSAPI_DISABLE_MIXBINS

    return DS_OK;

#else // DSAPI_DISABLE_MIXBINS

    return ((CDirectSoundBuffer *)pBuffer)->SetMixBins(pMixBins);

#endif // DSAPI_DISABLE_SUBMIX

}

STDAPI IDirectSoundBuffer_SetMixBinVolumes(LPDIRECTSOUNDBUFFER pBuffer, LPCDSMIXBINS pMixBins)
{

#ifdef DSAPI_DISABLE_VOLUME

    return DS_OK;

#else // DSAPI_DISABLE_VOLUME

    return ((CDirectSoundBuffer *)pBuffer)->SetMixBinVolumes(pMixBins);

#endif // DSAPI_DISABLE_VOLUME

}

#ifndef MCPX_BOOT_LIB

STDAPI IDirectSoundBuffer_SetAllParameters(LPDIRECTSOUNDBUFFER pBuffer, LPCDS3DBUFFER pds3db, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetAllParameters(pds3db, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetConeAngles(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetConeAngles(dwInsideConeAngle, dwOutsideConeAngle, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetConeOrientation(LPDIRECTSOUNDBUFFER pBuffer, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetConeOrientation(x, y, z, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetConeOutsideVolume(LPDIRECTSOUNDBUFFER pBuffer, LONG lConeOutsideVolume, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetConeOutsideVolume(lConeOutsideVolume, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetMaxDistance(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flMaxDistance, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetMaxDistance(flMaxDistance, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetMinDistance(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flMinDistance, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetMinDistance(flMinDistance, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetMode(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwMode, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetMode(dwMode, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetPosition(LPDIRECTSOUNDBUFFER pBuffer, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetPosition(x, y, z, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetVelocity(LPDIRECTSOUNDBUFFER pBuffer, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetVelocity(x, y, z, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetDistanceFactor(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flDistanceFactor, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetDistanceFactor(flDistanceFactor, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetDopplerFactor(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flDopplerFactor, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetDopplerFactor(flDopplerFactor, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetRolloffFactor(LPDIRECTSOUNDBUFFER pBuffer, FLOAT flRolloffFactor, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetRolloffFactor(flRolloffFactor, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetRolloffCurve(LPDIRECTSOUNDBUFFER pBuffer, const FLOAT *pflPoints, DWORD dwPointCount, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetRolloffCurve(pflPoints, dwPointCount, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundBuffer_SetI3DL2Source(LPDIRECTSOUNDBUFFER pBuffer, LPCDSI3DL2BUFFER pds3db, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundBuffer *)pBuffer)->SetI3DL2Source(pds3db, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

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

#ifdef DSAPI_DISABLE_NOTEOFF

    dwFlags = 0;

#endif // DSAPI_DISABLE_NOTEOFF

    return ((CDirectSoundBuffer *)pBuffer)->StopEx(rtTimeStamp, dwFlags);
}

STDAPI IDirectSoundBuffer_SetPlayRegion(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwPlayStart, DWORD dwPlayLength)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetPlayRegion(dwPlayStart, dwPlayLength);
}

STDAPI IDirectSoundBuffer_SetLoopRegion(LPDIRECTSOUNDBUFFER pBuffer, DWORD dwLoopStart, DWORD dwLoopLength)
{
    return ((CDirectSoundBuffer *)pBuffer)->SetLoopRegion(dwLoopStart, dwLoopLength);
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

#ifdef DSAPI_DISABLE_PITCH

    return DS_OK;

#else // DSAPI_DISABLE_PITCH

    return ((CDirectSoundStream *)pStream)->SetFrequency(dwFrequency);

#endif // DSAPI_DISABLE_PITCH

}

STDAPI IDirectSoundStream_SetVolume(LPDIRECTSOUNDSTREAM pStream, LONG lVolume)
{

#ifdef DSAPI_DISABLE_VOLUME

    return DS_OK;

#else // DSAPI_DISABLE_VOLUME

    return ((CDirectSoundStream *)pStream)->SetVolume(lVolume);

#endif // DSAPI_DISABLE_VOLUME

}

STDAPI IDirectSoundStream_SetPitch(LPDIRECTSOUNDSTREAM pStream, LONG lPitch)
{

#ifdef DSAPI_DISABLE_PITCH

    return DS_OK;

#else // DSAPI_DISABLE_PITCH

    return ((CDirectSoundStream *)pStream)->SetPitch(lPitch);

#endif // DSAPI_DISABLE_PITCH

}

STDAPI IDirectSoundStream_SetLFO(LPDIRECTSOUNDSTREAM pStream, LPCDSLFODESC pLFODesc)
{

#ifdef DSAPI_DISABLE_LFO

    return DS_OK;

#else // DSAPI_DISABLE_LFO

    return ((CDirectSoundStream *)pStream)->SetLFO(pLFODesc);

#endif // DSAPI_DISABLE_LFO

}

STDAPI IDirectSoundStream_SetEG(LPDIRECTSOUNDSTREAM pStream, LPCDSENVELOPEDESC pEnvelopeDesc)
{

#ifdef DSAPI_DISABLE_EG

    return DS_OK;

#else // DSAPI_DISABLE_EG

    return ((CDirectSoundStream *)pStream)->SetEG(pEnvelopeDesc);

#endif // DSAPI_DISABLE_EG

}

STDAPI IDirectSoundStream_SetFilter(LPDIRECTSOUNDSTREAM pStream, LPCDSFILTERDESC pFilter)
{

#ifdef DSAPI_DISABLE_FILTER

    return DS_OK;

#else // DSAPI_DISABLE_FILTER

    return ((CDirectSoundStream *)pStream)->SetFilter(pFilter);

#endif // DSAPI_DISABLE_FILTER

}

STDAPI IDirectSoundStream_SetHeadroom(LPDIRECTSOUNDSTREAM pStream, DWORD dwHeadroom)
{

#ifdef DSAPI_DISABLE_VOLUME

    return DS_OK;

#else // DSAPI_DISABLE_VOLUME

    return ((CDirectSoundStream *)pStream)->SetHeadroom(dwHeadroom);

#endif // DSAPI_DISABLE_VOLUME

}

STDAPI IDirectSoundStream_SetOutputBuffer(LPDIRECTSOUNDSTREAM pStream, LPDIRECTSOUNDBUFFER pOutputBuffer)
{

#ifdef DSAPI_DISABLE_SUBMIX

    return DS_OK;

#else // DSAPI_DISABLE_SUBMIX

    return ((CDirectSoundStream *)pStream)->SetOutputBuffer(pOutputBuffer);

#endif // DSAPI_DISABLE_SUBMIX

}

STDAPI IDirectSoundStream_SetMixBins(LPDIRECTSOUNDSTREAM pStream, LPCDSMIXBINS pMixBins)
{

#ifdef DSAPI_DISABLE_MIXBINS

    return DS_OK;

#else // DSAPI_DISABLE_MIXBINS

    return ((CDirectSoundStream *)pStream)->SetMixBins(pMixBins);

#endif // DSAPI_DISABLE_MIXBINS

}

STDAPI IDirectSoundStream_SetMixBinVolumes(LPDIRECTSOUNDSTREAM pStream, LPCDSMIXBINS pMixBins)
{

#ifdef DSAPI_DISABLE_VOLUME

    return DS_OK;

#else // DSAPI_DISABLE_VOLUME

    return ((CDirectSoundStream *)pStream)->SetMixBinVolumes(pMixBins);

#endif // DSAPI_DISABLE_VOLUME

}

STDAPI IDirectSoundStream_SetAllParameters(LPDIRECTSOUNDSTREAM pStream, LPCDS3DBUFFER pds3db, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetAllParameters(pds3db, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetConeAngles(LPDIRECTSOUNDSTREAM pStream, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetConeAngles(dwInsideConeAngle, dwOutsideConeAngle, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetConeOrientation(LPDIRECTSOUNDSTREAM pStream, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetConeOrientation(x, y, z, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetConeOutsideVolume(LPDIRECTSOUNDSTREAM pStream, LONG lConeOutsideVolume, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetConeOutsideVolume(lConeOutsideVolume, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetMaxDistance(LPDIRECTSOUNDSTREAM pStream, FLOAT flMaxDistance, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetMaxDistance(flMaxDistance, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetMinDistance(LPDIRECTSOUNDSTREAM pStream, FLOAT flMinDistance, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetMinDistance(flMinDistance, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetMode(LPDIRECTSOUNDSTREAM pStream, DWORD dwMode, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetMode(dwMode, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetPosition(LPDIRECTSOUNDSTREAM pStream, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetPosition(x, y, z, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetVelocity(LPDIRECTSOUNDSTREAM pStream, FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetVelocity(x, y, z, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetDistanceFactor(LPDIRECTSOUNDSTREAM pStream, FLOAT flDistanceFactor, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetDistanceFactor(flDistanceFactor, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetDopplerFactor(LPDIRECTSOUNDSTREAM pStream, FLOAT flDopplerFactor, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetDopplerFactor(flDopplerFactor, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetRolloffFactor(LPDIRECTSOUNDSTREAM pStream, FLOAT flRolloffFactor, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetRolloffFactor(flRolloffFactor, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetRolloffCurve(LPDIRECTSOUNDSTREAM pStream, const FLOAT *pflPoints, DWORD dwPointCount, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetRolloffCurve(pflPoints, dwPointCount, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_SetI3DL2Source(LPDIRECTSOUNDSTREAM pStream, LPCDSI3DL2BUFFER pds3db, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_3DSOURCE

    return DS_OK;

#else // DSAPI_DISABLE_3DSOURCE

    return ((CDirectSoundStream *)pStream)->SetI3DL2Source(pds3db, dwFlags);

#endif // DSAPI_DISABLE_3DSOURCE

}

STDAPI IDirectSoundStream_Pause(LPDIRECTSOUNDSTREAM pStream, DWORD dwPause)
{
    return ((CDirectSoundStream *)pStream)->Pause(dwPause);
}

STDAPI IDirectSoundStream_PauseEx(LPDIRECTSOUNDSTREAM pStream, REFERENCE_TIME rtTimestamp, DWORD dwPause)
{
    return ((CDirectSoundStream *)pStream)->PauseEx(rtTimestamp, dwPause);
}

STDAPI IDirectSoundStream_FlushEx(LPDIRECTSOUNDSTREAM pStream, REFERENCE_TIME rtTimeStamp, DWORD dwFlags)
{

#ifdef DSAPI_DISABLE_NOTEOFF

    dwFlags = 0;

#endif // DSAPI_DISABLE_NOTEOFF

    return ((CDirectSoundStream *)pStream)->FlushEx(rtTimeStamp, dwFlags);
}

#endif // MCPX_BOOT_LIB


/****************************************************************************
 *
 *  DirectSoundCreateInternal
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
    LPGUID                          pguidDeviceId,
    LPDIRECTSOUND *                 ppDirectSound,
    LPUNKNOWN                       pControllingUnknown
)
{
    CDirectSound *  pDirectSound;
    HRESULT                         hr;
    
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
    LPCDSBUFFERDESC                 pdsbd,
    LPDIRECTSOUNDBUFFER *           ppBuffer
)
{
    CDirectSound *  pDirectSound    = NULL;
    HRESULT                         hr;
    
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
    LPCDSSTREAMDESC                 pdssd,
    LPDIRECTSOUNDSTREAM *           ppStream
)
{
    CDirectSound *  pDirectSound    = NULL;
    HRESULT                         hr;
    
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
    // If the DirectSound object hasn't been created, the sample counter
    // can't be trusted.
    //

    if(CDirectSound::m_pDirectSound)
    {
        MCPX_REG_READ(NV_PAPU_XGSCNT, &dwSampleTime);
    }
    else
    {
        dwSampleTime = 0;
    }

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

#ifndef DS3D_ALLOW_ALGORITHM_SWAP

    if(CHrtfSource::IsValidAlgorithm())
    {
        DPF_ERROR("3D algorithms can't be changed once set");
    }
    else

#endif // DS3D_ALLOW_ALGORITHM_SWAP

    {
        DPF_ABSOLUTE("Using full HRTF 3D algorithm");
    
        CHrtfSource::SetAlgorithm_FullHrtf();

#ifdef DS3D_ALLOW_ALGORITHM_SWAP

        if(CDirectSound::m_pDirectSound)
        {
            CDirectSound::m_pDirectSound->Force3dRecalc();
        }

#endif // DS3D_ALLOW_ALGORITHM_SWAP

    }

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

#ifndef DS3D_ALLOW_ALGORITHM_SWAP

    if(CHrtfSource::IsValidAlgorithm())
    {
        DPF_ERROR("3D algorithms can't be changed once set");
    }
    else

#endif // DS3D_ALLOW_ALGORITHM_SWAP

    {
        DPF_ABSOLUTE("Using light HRTF 3D algorithm");
    
        CHrtfSource::SetAlgorithm_LightHrtf();

#ifdef DS3D_ALLOW_ALGORITHM_SWAP

        if(CDirectSound::m_pDirectSound)
        {
            CDirectSound::m_pDirectSound->Force3dRecalc();
        }

#endif // DS3D_ALLOW_ALGORITHM_SWAP

    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  DirectSoundUsePan3D
 *
 *  Description:
 *      Sets up the 3D engine to use panning.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#if 0

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundUsePan3D"

void
DirectSoundUsePan3D
(
    void
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

#ifndef DS3D_ALLOW_ALGORITHM_SWAP

    if(CHrtfSource::IsValidAlgorithm())
    {
        DPF_ERROR("3D algorithms can't be changed once set");
    }
    else

#endif // DS3D_ALLOW_ALGORITHM_SWAP

    {
        DPF_ABSOLUTE("Using pan 3D algorithm");
    
        CHrtfSource::SetAlgorithm_Pan();

#ifdef DS3D_ALLOW_ALGORITHM_SWAP

        if(CDirectSound::m_pDirectSound)
        {
            CDirectSound::m_pDirectSound->Force3dRecalc();
        }

#endif // DS3D_ALLOW_ALGORITHM_SWAP

    }

    DPF_LEAVE_VOID();
}

#endif


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

    if(DSSPEAKER_USE_DEFAULT != dwSpeakerConfig)
    {
        if(!DSSPEAKER_IS_VALID(dwSpeakerConfig))
        {
            DPF_ERROR("Invalid speaker configuration");
        }

        if(!DSSPEAKER_IS_VALID_BASIC(dwSpeakerConfig))
        {
            DPF_ERROR("Can't set encoder bits in the speaker config");
        }
    }

    if(CDirectSound::m_pDirectSound)
    {
        DPF_ERROR("Can't call " DPF_FNAME " after the DirectSound object has been created");
    }

#endif // VALIDATE_PARAMETERS

    g_dwDirectSoundOverrideSpeakerConfig = (DSSPEAKER_USE_DEFAULT == dwSpeakerConfig) ? DSSPEAKER_USE_DEFAULT : DSSPEAKER_BASIC(dwSpeakerConfig);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  DirectSoundLoadEncoder
 *
 *  Description:
 *      Creates and initializes a DirectSound object.
 *
 *  Arguments:
 *      LPCVOID [in]: image data.
 *      DWORD [in]: image data size, in bytes.
 *      LPVOID * [out]: image scratch data.
 *      LPDIRECTSOUND * [out]: DirectSound object.  The caller is responbile
 *                             for freeing this object with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#ifdef DEBUG

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundLoadEncoder"

HRESULT
DirectSoundLoadEncoder
(
    LPCVOID                 pvImageBuffer, 
    DWORD                   dwImageSize, 
    LPVOID *                ppvScratchData, 
    LPDIRECTSOUND *         ppDirectSound
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    //
    // Make sure the DirectSound object doesn't already exist
    //

    if(CDirectSound::m_pDirectSound)
    {
        DPF_ERROR("You must call DirectSoundLoadEncoder before the DirectSound object has been created");
        hr = DSERR_INVALIDCALL;
    }

    //
    // Set the global encoder image pointer
    //

    if(SUCCEEDED(hr))
    {
        CDirectSound::m_pvEncoderImageData = (LPVOID)pvImageBuffer;
        CDirectSound::m_dwEncoderImageSize = dwImageSize;
    }

    //
    // Create the DirectSound object
    //

    if(SUCCEEDED(hr))
    {
        hr = DirectSoundCreate(NULL, ppDirectSound, NULL);
    }

    //
    // Retrieve the scratch image pointer
    //

    if(SUCCEEDED(hr) && ppvScratchData)
    {
        *ppvScratchData = CDirectSound::m_pvEncoderImageData;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}

#endif // DEBUG

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
 *  XAudioCreatePcmFormat
 *
 *  Description:
 *      Creates a PCM wave format structure.
 *
 *  Arguments:
 *      WORD [in]: channel count.
 *      DWORD [in]: sampling rate.
 *      WORD [in]: count of bits per sample.
 *      LPWAVEFORMATEX [out]: format data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "XAudioCreatePcmFormat"

void 
XAudioCreatePcmFormat
(
    WORD                    nChannels, 
    DWORD                   nSamplesPerSec, 
    WORD                    wBitsPerSample, 
    LPWAVEFORMATEX          pwfx
)
{
    DPF_ENTER();
    
    CreatePcmFormat(nChannels, nSamplesPerSec, wBitsPerSample, pwfx);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  XAudioCreateAdpcmFormat
 *
 *  Description:
 *      Creates an Xbox ADPCM wave format structure.
 *
 *  Arguments:
 *      WORD [in]: channel count.
 *      DWORD [in]: sampling rate.
 *      LPWAVEFORMATEX [out]: format data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "XAudioCreateAdpcmFormat"

void 
XAudioCreateAdpcmFormat
(
    WORD                    nChannels, 
    DWORD                   nSamplesPerSec, 
    LPXBOXADPCMWAVEFORMAT   pwfx
)
{
    DPF_ENTER();
    
    CreateXboxAdpcmFormat(nChannels, nSamplesPerSec, pwfx);

    DPF_LEAVE_VOID();
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
 *  XWaveFileCreateMediaObject
 *
 *  Description:
 *      Creates and initializes a WaveLoader XMO.
 *
 *  Arguments:
 *      LPCSTR [in]: file path.
 *      LPCWAVEFORMATEX * [out]: receives a pointer to the file format.  The
 *                               file XMO maintains this data, so the caller
 *                               should not try to free this pointer.
 *      XMediaObject ** [out]: media object.  The caller is responsible for
 *                             for freeing this object with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "XWaveFileCreateMediaObject"

HRESULT
XWaveFileCreateMediaObject
(
    LPCSTR                  pszFileName, 
    LPCWAVEFORMATEX *       ppwfxFormat, 
    XFileMediaObject **     ppMediaObject
)
{
    CWaveFileMediaObject *  pMediaObject;
    HRESULT                 hr;

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!pszFileName)
    {
        DPF_ERROR("No file name specified");
    }

    if(!ppMediaObject)
    {
        DPF_ERROR("No media object pointer supplied");
    }

#endif // VALIDATE_PARAMETERS
    
    hr = HRFROMP(pMediaObject = NEW(CWaveFileMediaObject));

    if(SUCCEEDED(hr))
    {
        hr = pMediaObject->Initialize(pszFileName);
    }

    if(SUCCEEDED(hr) && ppwfxFormat)
    {
        hr = pMediaObject->GetFormat(ppwfxFormat);
    }

    if(SUCCEEDED(hr))
    {
        *ppMediaObject = ADDREF(pMediaObject);
    }

    RELEASE(pMediaObject);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  XWaveFileCreateMediaObjectEx
 *
 *  Description:
 *      Creates and initializes a WaveLoader XMO.
 *
 *  Arguments:
 *      LPCSTR [in]: file path.
 *      LPCWAVEFORMATEX * [out]: receives a pointer to the file format.  The
 *                               file XMO maintains this data, so the caller
 *                               should not try to free this pointer.
 *      XMediaObject ** [out]: media object.  The caller is responsible for
 *                             for freeing this object with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "XWaveFileCreateMediaObjectEx"

HRESULT
XWaveFileCreateMediaObjectEx
(
    LPCSTR                  pszFileName, 
    HANDLE                  hFile,
    XWaveFileMediaObject ** ppMediaObject
)
{
    CWaveFileMediaObject *  pMediaObject;
    HRESULT                 hr;

    DPF_ENTER();
    
#ifdef VALIDATE_PARAMETERS

    if(!pszFileName && !IS_VALID_HANDLE_VALUE(hFile))
    {
        DPF_ERROR("No file name or handle specified");
    }

    if(pszFileName && IS_VALID_HANDLE_VALUE(hFile))
    {
        DPF_ERROR("Both file name and handle specified");
    }

    if(!ppMediaObject)
    {
        DPF_ERROR("No media object pointer supplied");
    }

#endif // VALIDATE_PARAMETERS
    
    hr = HRFROMP(pMediaObject = NEW(CWaveFileMediaObject));

    if(SUCCEEDED(hr))
    {
        if(pszFileName)
        {
            hr = pMediaObject->Initialize(pszFileName);
        }
        else
        {
            hr = pMediaObject->Initialize(hFile);
        }
    }

    if(SUCCEEDED(hr))
    {
        *ppMediaObject = ADDREF(pMediaObject);
    }

    RELEASE(pMediaObject);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  XFileCreateMediaObject
 *
 *  Description:
 *      Creates and initializes a File XMO.
 *
 *  Arguments:
 *      LPCSTR [in]: file name.
 *      DWORD [in]: access mask.
 *      DWORD [in]: share mask.
 *      DWORD [in]: creation distribution.
 *      DWORD [in]: attribute mask.
 *      XMediaObject ** [out]: media object.  The caller is responsible for
 *                             for freeing this object with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "XFileCreateMediaObject"

HRESULT
XFileCreateMediaObject
(
    LPCSTR                  pszFileName, 
    DWORD                   dwDesiredAccess, 
    DWORD                   dwShareMode, 
    DWORD                   dwCreationDisposition, 
    DWORD                   dwFlagsAndAttributes,
    XFileMediaObject **     ppMediaObject
)
{
    CFileMediaObject *      pMediaObject;
    HRESULT                 hr;

    DPF_ENTER();
    
    hr = HRFROMP(pMediaObject = NEW(CFileMediaObject));

    if(SUCCEEDED(hr))
    {
        hr = pMediaObject->Initialize(pszFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, dwFlagsAndAttributes);
    }

    if(SUCCEEDED(hr))
    {
        *ppMediaObject = ADDREF(pMediaObject);
    }

    RELEASE(pMediaObject);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  XFileCreateMediaObject
 *
 *  Description:
 *      Creates and initializes a File XMO.
 *
 *  Arguments:
 *      HANDLE [in]: file handle.
 *      XMediaObject ** [out]: media object.  The caller is responsible for
 *                             for freeing this object with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "XFileCreateMediaObjectEx"

HRESULT
XFileCreateMediaObjectEx
(
    HANDLE                  hFile,
    XFileMediaObject **     ppMediaObject
)
{
    CFileMediaObject *      pMediaObject;
    HRESULT                 hr;

    DPF_ENTER();
    
    hr = HRFROMP(pMediaObject = NEW(CFileMediaObject));

    if(SUCCEEDED(hr))
    {
        hr = pMediaObject->Initialize(hFile);
    }

    if(SUCCEEDED(hr))
    {
        *ppMediaObject = ADDREF(pMediaObject);
    }

    RELEASE(pMediaObject);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  Ac97CreateMediaObject
 *
 *  Description:
 *      Creates and initializes an AC97 Media Object.
 *
 *  Arguments:
 *      DWORD [in]: channel identifier.
 *      XMediaObject ** [out]: receives channel object.  The caller is
 *                             responsible for freeing this object with
 *                             Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "Ac97CreateMediaObject"

HRESULT
Ac97CreateMediaObject
(
    DWORD                       dwChannel,
    LPFNXMEDIAOBJECTCALLBACK    pfnCallback, 
    LPVOID                      pvCallbackContext,
    XAc97MediaObject **         ppMediaObject
)
{
    CAc97MediaObject *          pMediaObject    = NULL;
    HRESULT                     hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    //
    // Create the AC97 media object
    //

    hr = HRFROMP(pMediaObject = NEW(CAc97MediaObject));
   
    if(SUCCEEDED(hr))
    {
        hr = pMediaObject->Initialize(dwChannel, pfnCallback, pvCallbackContext);
    }

    if(SUCCEEDED(hr))
    {
        *ppMediaObject = ADDREF(pMediaObject);
    }

    RELEASE(pMediaObject);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  DirectSoundDumpMemoryUsage
 *
 *  Description:
 *      Dumps current memory usage to the debugger.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to assert that there is no memory allocated.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "DirectSoundDumpMemoryUsage"

void
DirectSoundDumpMemoryUsage
(
    BOOL                    fAssertNone
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    CMemoryManager::DumpMemoryUsage(fAssertNone);

    DPF_LEAVE_VOID();
}


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

    m_3dParams.HrtfParams = DirectSoundDefault3DListener;
    m_3dParams.I3dl2Params = DirectSoundI3DL2ListenerPreset_Default;

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
        m_abMixBinHeadroom[i] = DSHEADROOM_DEFAULT;
    }

    m_abMixBinHeadroom[DSMIXBIN_SUBMIX] = 0;

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

#ifdef DEBUG

LPVOID CDirectSound::m_pvEncoderImageData;
DWORD CDirectSound::m_dwEncoderImageSize;

#endif // DEBUG

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

    CPerfMon::UnregisterCounters();

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
    DWORD                   dwStatus;
    HRESULT                 hr;
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

            m_apKeepAliveBuffers[i]->Stop();
        }

        for(i = 0; i < NUMELMS(m_apKeepAliveBuffers); i++)
        {
            ASSERT(m_apKeepAliveBuffers[i]);

            do
            {
                DoWork();
                
                hr = m_apKeepAliveBuffers[i]->GetStatus(&dwStatus);
            }
            while(SUCCEEDED(hr) && (dwStatus & DSBSTATUS_PLAYING));

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
        CPerfMon::RegisterCounters();
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
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pdsc)
    {
        DPF_ERROR("DSCAPS not supplied");
    }

#endif // VALIDATE_PARAMETERS
    
    pdsc->dwFree2DBuffers = g_dwDirectSoundFree2dVoices;
    pdsc->dwFree3DBuffers = g_dwDirectSoundFree3dVoices;
    pdsc->dwFreeBufferSGEs = g_dwDirectSoundFreeBufferSGEs;
    pdsc->dwMemoryAllocated = g_dwDirectSoundPoolMemoryUsage + g_dwDirectSoundPhysicalMemoryUsage;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
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
    ENTER_EXTERNAL_METHOD();

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

    m_pSettings->m_abMixBinHeadroom[dwMixBin] = (BYTE)dwHeadroom;
    
    //
    // Update the implementation object
    //

    hr = m_pDevice->SetMixBinHeadroom(dwMixBin);

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

    m_pSettings->m_3dParams.HrtfParams.vPosition.x = NormalizeFloat(pParams->vPosition.x);
    m_pSettings->m_3dParams.HrtfParams.vPosition.y = NormalizeFloat(pParams->vPosition.y);
    m_pSettings->m_3dParams.HrtfParams.vPosition.z = NormalizeFloat(pParams->vPosition.z);

    m_pSettings->m_3dParams.HrtfParams.vVelocity.x = NormalizeFloat(pParams->vVelocity.x);
    m_pSettings->m_3dParams.HrtfParams.vVelocity.y = NormalizeFloat(pParams->vVelocity.y);
    m_pSettings->m_3dParams.HrtfParams.vVelocity.z = NormalizeFloat(pParams->vVelocity.z);

    m_pSettings->m_3dParams.HrtfParams.vOrientFront.x = NormalizeFloat(pParams->vOrientFront.x);
    m_pSettings->m_3dParams.HrtfParams.vOrientFront.y = NormalizeFloat(pParams->vOrientFront.y);
    m_pSettings->m_3dParams.HrtfParams.vOrientFront.z = NormalizeFloat(pParams->vOrientFront.z);
    
    m_pSettings->m_3dParams.HrtfParams.vOrientTop.x = NormalizeFloat(pParams->vOrientTop.x);
    m_pSettings->m_3dParams.HrtfParams.vOrientTop.y = NormalizeFloat(pParams->vOrientTop.y);
    m_pSettings->m_3dParams.HrtfParams.vOrientTop.z = NormalizeFloat(pParams->vOrientTop.z);

    m_pSettings->m_3dParams.HrtfParams.flDistanceFactor = NormalizeFloat(pParams->flDistanceFactor);
    m_pSettings->m_3dParams.HrtfParams.flRolloffFactor = NormalizeFloat(pParams->flRolloffFactor);
    m_pSettings->m_3dParams.HrtfParams.flDopplerFactor = NormalizeFloat(pParams->flDopplerFactor);

    m_pSettings->m_3dParams.dwParameterMask |= DS3DPARAM_LISTENER_MASK;

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

    m_pSettings->m_3dParams.HrtfParams.flDistanceFactor = NormalizeFloat(flDistanceFactor);

    m_pSettings->m_3dParams.dwParameterMask |= DS3DPARAM_LISTENER_DISTANCEFACTOR;

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

    m_pSettings->m_3dParams.HrtfParams.flDopplerFactor = NormalizeFloat(flDopplerFactor);

    m_pSettings->m_3dParams.dwParameterMask |= DS3DPARAM_LISTENER_DOPPLERFACTOR;

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

    m_pSettings->m_3dParams.HrtfParams.flRolloffFactor = NormalizeFloat(flRolloffFactor);

    m_pSettings->m_3dParams.dwParameterMask |= DS3DPARAM_LISTENER_ROLLOFFFACTOR;

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

    m_pSettings->m_3dParams.HrtfParams.vOrientFront.x = NormalizeFloat(flFrontX);
    m_pSettings->m_3dParams.HrtfParams.vOrientFront.y = NormalizeFloat(flFrontY);
    m_pSettings->m_3dParams.HrtfParams.vOrientFront.z = NormalizeFloat(flFrontZ);

    m_pSettings->m_3dParams.HrtfParams.vOrientTop.x = NormalizeFloat(flTopX);
    m_pSettings->m_3dParams.HrtfParams.vOrientTop.y = NormalizeFloat(flTopY);
    m_pSettings->m_3dParams.HrtfParams.vOrientTop.z = NormalizeFloat(flTopZ);

    m_pSettings->m_3dParams.dwParameterMask |= DS3DPARAM_LISTENER_ORIENTATION;

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

    m_pSettings->m_3dParams.HrtfParams.vPosition.x = NormalizeFloat(flPositionX);
    m_pSettings->m_3dParams.HrtfParams.vPosition.y = NormalizeFloat(flPositionY);
    m_pSettings->m_3dParams.HrtfParams.vPosition.z = NormalizeFloat(flPositionZ);

    m_pSettings->m_3dParams.dwParameterMask |= DS3DPARAM_LISTENER_POSITION;

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

    m_pSettings->m_3dParams.HrtfParams.vVelocity.x = NormalizeFloat(flVelocityX);
    m_pSettings->m_3dParams.HrtfParams.vVelocity.y = NormalizeFloat(flVelocityY);
    m_pSettings->m_3dParams.HrtfParams.vVelocity.z = NormalizeFloat(flVelocityZ);

    m_pSettings->m_3dParams.dwParameterMask |= DS3DPARAM_LISTENER_VELOCITY;

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
    LPCDSI3DL2LISTENER      pParams,
    DWORD                   dwFlags
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();    
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pParams)
    {
        DPF_ERROR("Failed to supply properties");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if((pParams->lRoom < DSI3DL2LISTENER_MINROOM) || (pParams->lRoom > DSI3DL2LISTENER_MAXROOM))
    {
        DPF_ERROR("Invalid lRoom value");
    }

    if((pParams->lRoomHF < DSI3DL2LISTENER_MINROOMHF) || (pParams->lRoomHF > DSI3DL2LISTENER_MAXROOMHF))
    {
        DPF_ERROR("Invalid lRoomHF value");
    }

    if((pParams->flRoomRolloffFactor < DSI3DL2LISTENER_MINROOMROLLOFFFACTOR) || (pParams->flRoomRolloffFactor > DSI3DL2LISTENER_MAXROOMROLLOFFFACTOR))
    {
        DPF_ERROR("Invalid flRoomRolloffFactor value");
    }

    if((pParams->flDecayTime < DSI3DL2LISTENER_MINDECAYTIME) || (pParams->flDecayTime > DSI3DL2LISTENER_MAXDECAYTIME))
    {
        DPF_ERROR("Invalid flDecayTime value");
    }

    if((pParams->flDecayHFRatio < DSI3DL2LISTENER_MINDECAYHFRATIO) || (pParams->flDecayHFRatio > DSI3DL2LISTENER_MAXDECAYHFRATIO))
    {
        DPF_ERROR("Invalid flDecayHFRatio value");
    }

    if((pParams->lReflections < DSI3DL2LISTENER_MINREFLECTIONS) || (pParams->lReflections > DSI3DL2LISTENER_MAXREFLECTIONS))
    {
        DPF_ERROR("Invalid lReflections value");
    }

    if((pParams->flReflectionsDelay < DSI3DL2LISTENER_MINREFLECTIONSDELAY) || (pParams->flReflectionsDelay > DSI3DL2LISTENER_MAXREFLECTIONSDELAY))
    {
        DPF_ERROR("Invalid flReflectionsDelay value");
    }

    if((pParams->lReverb < DSI3DL2LISTENER_MINREVERB) || (pParams->lReverb > DSI3DL2LISTENER_MAXREVERB))
    {
        DPF_ERROR("Invalid lReverb value");
    }

    if((pParams->flReverbDelay < DSI3DL2LISTENER_MINREVERBDELAY) || (pParams->flReverbDelay > DSI3DL2LISTENER_MAXREVERBDELAY))
    {
        DPF_ERROR("Invalid flReverbDelay value");
    }

    if((pParams->flDiffusion < DSI3DL2LISTENER_MINDIFFUSION) || (pParams->flDiffusion > DSI3DL2LISTENER_MAXDIFFUSION))
    {
        DPF_ERROR("Invalid flDiffusion value");
    }

    if((pParams->flDensity < DSI3DL2LISTENER_MINDENSITY) || (pParams->flDensity > DSI3DL2LISTENER_MAXDENSITY))
    {
        DPF_ERROR("Invalid flDensity value");
    }

    if((pParams->flHFReference < DSI3DL2LISTENER_MINHFREFERENCE) || (pParams->flHFReference > DSI3DL2LISTENER_MAXHFREFERENCE))
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
        m_pSettings->m_3dParams.I3dl2Params.lRoom = pParams->lRoom;
        m_pSettings->m_3dParams.I3dl2Params.lRoomHF = pParams->lRoomHF;
        m_pSettings->m_3dParams.I3dl2Params.flRoomRolloffFactor = NormalizeFloat(pParams->flRoomRolloffFactor);;
        m_pSettings->m_3dParams.I3dl2Params.flDecayTime = NormalizeFloat(pParams->flDecayTime);
        m_pSettings->m_3dParams.I3dl2Params.flDecayHFRatio = NormalizeFloat(pParams->flDecayHFRatio);
        m_pSettings->m_3dParams.I3dl2Params.lReflections = pParams->lReflections;
        m_pSettings->m_3dParams.I3dl2Params.flReflectionsDelay = NormalizeFloat(pParams->flReflectionsDelay);
        m_pSettings->m_3dParams.I3dl2Params.lReverb = pParams->lReverb;
        m_pSettings->m_3dParams.I3dl2Params.flReverbDelay = NormalizeFloat(pParams->flReverbDelay);
        m_pSettings->m_3dParams.I3dl2Params.flDiffusion = NormalizeFloat(pParams->flDiffusion);
        m_pSettings->m_3dParams.I3dl2Params.flDensity = NormalizeFloat(pParams->flDensity);
        m_pSettings->m_3dParams.I3dl2Params.flHFReference = NormalizeFloat(pParams->flHFReference);

        m_pSettings->m_3dParams.dwParameterMask |= DS3DPARAM_LISTENER_I3DL2;

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
    DWORD                   dwParameterMask;
    PLIST_ENTRY             pleEntry;

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    //
    // Recalculate changed 3D listener parameters
    //

    dwParameterMask = m_pDevice->Commit3dSettings();

    //
    // Recalculate all 3D voices
    //

    for(pleEntry = m_lst3dVoices.Flink; pleEntry != &m_lst3dVoices; pleEntry = pleEntry->Flink)
    {
        CONTAINING_RECORD(pleEntry, CDirectSoundVoice, m_le3dVoice)->CommitDeferredSettings();
    }

    //
    // Clear the parameter mask
    //

    m_pSettings->m_3dParams.dwParameterMask &= ~dwParameterMask;

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
 *  GetTime
 *
 *  Description:
 *      Gets the current EP output levels.
 *
 *  Arguments:
 *      DSOUTPUTLEVELS * [out]: current output levels.
 *      BOOL bReset [in]: Reset peak values
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSound::GetOutputLevels"

HRESULT
CDirectSound::GetOutputLevels
(
    DSOUTPUTLEVELS *        pLevels,
    BOOL bReset
)
{
    PDSOUTPUTLEVELS pDspLevels;
    DPF_ENTER();    

#ifdef VALIDATE_PARAMETERS

    if(!pLevels)
    {
        DPF_ERROR("Failed to specify an output level buffer");
    }

#endif // VALIDATE_PARAMETERS

    //
    // map a data struct on top the of EP dsp P ram location where
    // the output levels get cached, twice every 5.33ms
    //

    pDspLevels = (PDSOUTPUTLEVELS) ((PUCHAR) XPCICFG_APU_MEMORY_REGISTER_BASE_0 + 
        NV_PAPU_EPPMEM(0) +
        EP_OFFSET_OUTPUT_LEVELS_ANALOG_PEAK*sizeof(DWORD));

    //
    // copy levels to caller supplied buffer
    //

    memcpy(pLevels, pDspLevels, sizeof(DSOUTPUTLEVELS));

    //
    // if the caller wants to reset the historical max values
    // reset them now
    //

    if (bReset) {

        pDspLevels->dwAnalogLeftTotalPeak = 0;          // analog peak
        pDspLevels->dwAnalogRightTotalPeak = 0;
        pDspLevels->dwDigitalFrontLeftPeak = 0;         // digital peak levels
        pDspLevels->dwDigitalFrontCenterPeak = 0;
        pDspLevels->dwDigitalFrontRightPeak = 0;
        pDspLevels->dwDigitalBackLeftPeak = 0;
        pDspLevels->dwDigitalBackRightPeak = 0;
        pDspLevels->dwDigitalLowFrequencyPeak = 0;

    }

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

        //
        // Recalculate the whole 3D world
        //

        if(SUCCEEDED(hr))
        {
            m_pSettings->m_3dParams.dwParameterMask = ~0UL;
            
            hr = CommitDeferredSettings();
        }
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

#ifndef MCPX_BOOT_LIB

    //
    // Free memory
    //

    MEMFREE(m_p3dParams);

#endif // MCPX_BOOT_LIB

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
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoiceSettings::Initialize"

HRESULT
CDirectSoundVoiceSettings::Initialize
(
    DWORD                   dwFlags,
    LPCWAVEFORMATEX         pwfxFormat,
    LPCDSMIXBINS            pMixBins
)
{
    HRESULT                 hr              = DS_OK;
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

#ifndef MCPX_BOOT_LIB

    //
    // Allocate 3D parameter data
    //

    if(m_dwFlags & DSBCAPS_CTRL3D)
    {
        hr = HRFROMP(m_p3dParams = MEMALLOC(DS3DSOURCEPARAMS, 1));

        if(SUCCEEDED(hr))
        {
            m_p3dParams->HrtfParams = DirectSoundDefault3DBuffer;
            m_p3dParams->I3dl2Params = DirectSoundDefaultI3DL2Buffer;
            m_p3dParams->dwParameterMask = DS3DPARAM_BUFFER_MASK;
        }
    }

#endif // MCPX_BOOT_LIB

    DPF_LEAVE_HRESULT(hr);

    return hr;
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

    if(dwChannelMask = CreateInternalFormat(&m_fmt, pwfxFormat))
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
    LPCDSMIXBINS                pMixBins
)
{
    static const LPCDSMIXBINS   apDefaultMixBins[] =
    {
        &DirectSoundDefaultMixBins_Mono,
        &DirectSoundDefaultMixBins_Stereo,
        &DirectSoundDefaultMixBins_4Channel,
        &DirectSoundDefaultMixBins_6Channel,
    };

    DWORD                       i;
    
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
            ASSERT(m_fmt.nChannels / 2 < NUMELMS(apDefaultMixBins));
            pMixBins = apDefaultMixBins[m_fmt.nChannels / 2];
        }
    }

    //
    // Set mixbins and volume
    //

    ASSERT(pMixBins);
    ASSERT(pMixBins->dwMixBinCount <= NUMELMS(m_abMixBins));
    
    for(m_dwMixBinCount = 0; m_dwMixBinCount < (BYTE)pMixBins->dwMixBinCount; m_dwMixBinCount++)
    {
        ASSERT(pMixBins->lpMixBinVolumePairs[m_dwMixBinCount].dwMixBin < NUMELMS(m_alMixBinVolumes));
        
        m_abMixBins[m_dwMixBinCount] = (BYTE)pMixBins->lpMixBinVolumePairs[m_dwMixBinCount].dwMixBin;
        m_alMixBinVolumes[m_abMixBins[m_dwMixBinCount]] = pMixBins->lpMixBinVolumePairs[m_dwMixBinCount].lVolume;
    }

    //
    // If we're submixing, the submix bin must be included.  If it wasn't
    // included in the array, we'll add it automatically for "convenience"
    //

    if(m_pMixinBuffer)
    {
        ASSERT(m_pMixinBuffer->m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK);

        for(i = 0; i < m_dwMixBinCount; i++)
        {
            if(m_abMixBins[i] == (BYTE)m_pMixinBuffer->m_pSettings->m_dwInputMixBin)
            {
                break;
            }
        }

        if(i >= m_dwMixBinCount)
        {
            if(m_dwMixBinCount >= NUMELMS(m_abMixBins))
            {
                DPF_ERROR("One mixbin slot must be kept free for submixing.  The last mixbin specified will be lost");
                m_dwMixBinCount = NUMELMS(m_abMixBins) - 1;
            }

            m_abMixBins[m_dwMixBinCount] = (BYTE)m_pMixinBuffer->m_pSettings->m_dwInputMixBin;
            m_alMixBinVolumes[m_abMixBins[m_dwMixBinCount]] = DSBVOLUME_MAX;

            m_dwMixBinCount++;
        }
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
        ASSERT(m_abMixBins[m_dwMixBinCount - 1] == dwMixBin);

        m_dwMixBinCount--;

        RELEASE(m_pMixinBuffer);
    }

    //
    // Save a reference to the new buffer and update the mixbins.  We're 
    // automatically resetting the mixbin count to 0 before dropping the
    // submix bin in for "convienence."  If you call SetOutputBuffer(NULL)
    // after this, you'll have lost whatever mixbins you used to be assigned
    // to.
    //

    if(m_pMixinBuffer = ADDREF(pOutputBuffer))
    {
        dwMixBin = m_pMixinBuffer->m_pSettings->m_dwInputMixBin;

        ASSERT(m_pMixinBuffer->m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK);

        m_dwMixBinCount = 1;
        m_abMixBins[0] = (BYTE)dwMixBin;
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
    // Release the voice implementation object
    //

    RELEASE(m_pVoice);

    //
    // Release the shared settings object
    //

    RELEASE(m_pSettings);

    //
    // Release the reference to the DirectSound object
    //

    RELEASE(m_pDirectSound);

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

    if(!IsValidFormat(pwfxFormat))
    {
        DPF_ERROR("Invalid voice format");
    }

    if((m_pSettings->m_dwFlags & DSBCAPS_CTRL3D) && (1 != pwfxFormat->nChannels))
    {
        DPF_ERROR("3D voices must be mono");
    }

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't call SetFormat on MIXIN or FXIN buffers");
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

    if((pLfo->dwDelay < DSLFO_DELAY_MIN) || (pLfo->dwDelay > DSLFO_DELAY_MAX))
    {
        DPF_ERROR("Invalid LFO delay value");
    }

    if((pLfo->dwDelta < DSLFO_DELTA_MIN) || (pLfo->dwDelta > DSLFO_DELTA_MAX))
    {
        DPF_ERROR("Invalid LFO delta value");
    }

    if((pLfo->lPitchModulation < DSLFO_PITCHMOD_MIN) || (pLfo->lPitchModulation > DSLFO_PITCHMOD_MAX))
    {
        DPF_ERROR("Invalid LFO pitch modulation value");
    }

    if((pLfo->lFilterCutOffRange < DSLFO_FCRANGE_MIN) || (pLfo->lFilterCutOffRange > DSLFO_FCRANGE_MAX))
    {
        DPF_ERROR("Invalid LFO filter cutoff value");
    }

    if((pLfo->lAmplitudeModulation < DSLFO_AMPMOD_MIN) || (pLfo->lAmplitudeModulation > DSLFO_AMPMOD_MAX))
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

    if((pEnv->dwDelay < DSEG_DELAY_MIN) || (pEnv->dwDelay > DSEG_DELAY_MAX))
    {
        DPF_ERROR("Invalid envelope delay value");
    }

    if((pEnv->dwAttack < DSEG_ATTACK_MIN) || (pEnv->dwAttack > DSEG_ATTACK_MAX))
    {
        DPF_ERROR("Invalid envelope attack value");
    }

    if((pEnv->dwHold < DSEG_HOLD_MIN) || (pEnv->dwHold > DSEG_HOLD_MAX))
    {
        DPF_ERROR("Invalid envelope hold value");
    }

    if((pEnv->dwDecay < DSEG_DECAY_MIN) || (pEnv->dwDecay > DSEG_DECAY_MAX))
    {
        DPF_ERROR("Invalid envelope decay value");
    }

    if((pEnv->dwRelease < DSEG_RELEASE_MIN) || (pEnv->dwRelease > DSEG_RELEASE_MAX))
    {
        DPF_ERROR("Invalid envelope release value");
    }

    if((pEnv->dwSustain < DSEG_SUSTAIN_MIN) || (pEnv->dwSustain > DSEG_SUSTAIN_MAX))
    {
        DPF_ERROR("Invalid envelope sustain value");
    }

    if((pEnv->lPitchScale < DSEG_PITCHSCALE_MIN) || (pEnv->lPitchScale > DSEG_PITCHSCALE_MAX))
    {
        DPF_ERROR("Invalid envelope pitch scale value");
    }

    if((pEnv->lFilterCutOff < DSEG_FILTERCUTOFF_MIN) || (pEnv->lFilterCutOff > DSEG_FILTERCUTOFF_MAX))
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
    DWORD                   i;

#endif // VALIDATE_PARAMETERS

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(pMixinBuffer)
    {
        if(!(pMixinBuffer->m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK))
        {
            DPF_ERROR("Output buffer specified is not a MIXIN/FXIN buffer");
        }

        if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
        {
            DPF_ERROR("Can't set the output buffer on MIXIN/FXIN buffers");
        }

        if(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D)
        {
            DPF_ERROR("Can't submix a 3D buffer.  Create a 2D source buffer and a 3D destination buffer.");
        }

        if(1 != m_pSettings->m_fmt.nChannels)
        {
            DPF_ERROR("Can only submix mono source voices");
        }

        if(m_pSettings->m_dwMixBinCount >= NUMELMS(m_pSettings->m_abMixBins))
        {
            DPF_ERROR("Submixed voices must leave one mixbin slot free");
        }

        if(!fSubMixHeadroom)
        {
            if(m_pDirectSound->m_pSettings->m_abMixBinHeadroom[pMixinBuffer->m_pSettings->m_dwInputMixBin])
            {
                DPF_WARNING("The headroom for the mixbin your submix destination is reading from is");
                DPF_WARNING("non-zero.  This will be additive with the headroom that your submix");
                DPF_WARNING("destination is reading to, possibly giving you more headroom that you want.");
                DPF_WARNING("You will only see this warning once.");
                
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

    if(pMixBins)
    {
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
            if(pMixBins->lpMixBinVolumePairs[i].dwMixBin == m_pSettings->m_abMixBins[z])
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

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_p3dParams->HrtfParams.vPosition.x = NormalizeFloat(pParams->vPosition.x);
    m_pSettings->m_p3dParams->HrtfParams.vPosition.y = NormalizeFloat(pParams->vPosition.y);
    m_pSettings->m_p3dParams->HrtfParams.vPosition.z = NormalizeFloat(pParams->vPosition.z);

    m_pSettings->m_p3dParams->HrtfParams.vVelocity.x = NormalizeFloat(pParams->vVelocity.x);
    m_pSettings->m_p3dParams->HrtfParams.vVelocity.y = NormalizeFloat(pParams->vVelocity.y);
    m_pSettings->m_p3dParams->HrtfParams.vVelocity.z = NormalizeFloat(pParams->vVelocity.z);

    m_pSettings->m_p3dParams->HrtfParams.dwInsideConeAngle = pParams->dwInsideConeAngle;
    m_pSettings->m_p3dParams->HrtfParams.dwOutsideConeAngle = pParams->dwOutsideConeAngle;

    m_pSettings->m_p3dParams->HrtfParams.vConeOrientation.x = NormalizeFloat(pParams->vConeOrientation.x);
    m_pSettings->m_p3dParams->HrtfParams.vConeOrientation.y = NormalizeFloat(pParams->vConeOrientation.y);
    m_pSettings->m_p3dParams->HrtfParams.vConeOrientation.z = NormalizeFloat(pParams->vConeOrientation.z);

    m_pSettings->m_p3dParams->HrtfParams.lConeOutsideVolume = pParams->lConeOutsideVolume;
    
    m_pSettings->m_p3dParams->HrtfParams.flMinDistance = NormalizeFloat(pParams->flMinDistance);
    m_pSettings->m_p3dParams->HrtfParams.flMaxDistance = NormalizeFloat(pParams->flMaxDistance);
    
    m_pSettings->m_p3dParams->HrtfParams.dwMode = pParams->dwMode;
    
    m_pSettings->m_p3dParams->HrtfParams.flDistanceFactor = NormalizeFloat(pParams->flDistanceFactor);
    m_pSettings->m_p3dParams->HrtfParams.flRolloffFactor = NormalizeFloat(pParams->flRolloffFactor);
    m_pSettings->m_p3dParams->HrtfParams.flDopplerFactor = NormalizeFloat(pParams->flDopplerFactor);

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_MASK;

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

    m_pSettings->m_p3dParams->HrtfParams.dwInsideConeAngle = dwInside;
    m_pSettings->m_p3dParams->HrtfParams.dwOutsideConeAngle = dwOutside;

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_CONEANGLES;

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

    m_pSettings->m_p3dParams->HrtfParams.vConeOrientation.x = NormalizeFloat(flOrientationX);
    m_pSettings->m_p3dParams->HrtfParams.vConeOrientation.y = NormalizeFloat(flOrientationY);
    m_pSettings->m_p3dParams->HrtfParams.vConeOrientation.z = NormalizeFloat(flOrientationZ);

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_CONEORIENTATION;

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

    m_pSettings->m_p3dParams->HrtfParams.lConeOutsideVolume = lVolume;

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_CONEOUTSIDEVOLUME;

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

    if(flMaxDistance < m_pSettings->m_p3dParams->HrtfParams.flMinDistance)
    {
        DPF_ERROR("flMaxDistance must be >= flMinDistance");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_p3dParams->HrtfParams.flMaxDistance = NormalizeFloat(flMaxDistance);

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_MAXDISTANCE;

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

    if(flMinDistance > m_pSettings->m_p3dParams->HrtfParams.flMaxDistance)
    {
        DPF_ERROR("MinDistance must be <= MaxDistance");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_p3dParams->HrtfParams.flMinDistance = NormalizeFloat(flMinDistance);

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_MINDISTANCE;

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
    
    m_pSettings->m_p3dParams->HrtfParams.dwMode = dwMode;

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

    m_pSettings->m_p3dParams->HrtfParams.vPosition.x = NormalizeFloat(flPositionX);
    m_pSettings->m_p3dParams->HrtfParams.vPosition.y = NormalizeFloat(flPositionY);
    m_pSettings->m_p3dParams->HrtfParams.vPosition.z = NormalizeFloat(flPositionZ);

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_POSITION;

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

    m_pSettings->m_p3dParams->HrtfParams.vVelocity.x = NormalizeFloat(flVelocityX);
    m_pSettings->m_p3dParams->HrtfParams.vVelocity.y = NormalizeFloat(flVelocityY);
    m_pSettings->m_p3dParams->HrtfParams.vVelocity.z = NormalizeFloat(flVelocityZ);

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_VELOCITY;

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
#define DPF_FNAME "CDirectSoundVoice::SetDistanceFactor"

HRESULT 
CDirectSoundVoice::SetDistanceFactor
(
    FLOAT                   flDistanceFactor, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();

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

    m_pSettings->m_p3dParams->HrtfParams.flDistanceFactor = NormalizeFloat(flDistanceFactor);

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_DISTANCEFACTOR;

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
#define DPF_FNAME "CDirectSoundVoice::SetDopplerFactor"

HRESULT 
CDirectSoundVoice::SetDopplerFactor
(
    FLOAT                   flDopplerFactor, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();

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

    m_pSettings->m_p3dParams->HrtfParams.flDopplerFactor = NormalizeFloat(flDopplerFactor);

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_DOPPLERFACTOR;

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
 *      Sets the source's rolloff factor.
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
#define DPF_FNAME "CDirectSoundVoice::SetRolloffFactor"

HRESULT 
CDirectSoundVoice::SetRolloffFactor
(
    FLOAT                   flRolloffFactor, 
    DWORD                   dwFlags
)
{
    DPF_ENTER();

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

    m_pSettings->m_p3dParams->HrtfParams.flRolloffFactor = NormalizeFloat(flRolloffFactor);

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_ROLLOFFFACTOR;

    if(!(dwFlags & DS3D_DEFERRED))
    {
        CommitDeferredSettings();
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/***************************************************************************
 *
 *  SetRolloffCurve
 *
 *  Description:
 *      Sets the source's rolloff curve.
 *
 *  Arguments:
 *      const FLOAT * [in]: point array.
 *      DWORD [in]: point count.
 *      DWORD [in]: flags.
 *
 *  Returns:
 *      HRESULT: DirectSound/COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundVoice::SetRolloffCurve"

HRESULT 
CDirectSoundVoice::SetRolloffCurve
(
    const FLOAT *           pflPoints, 
    DWORD                   dwPointCount, 
    DWORD                   dwFlags
)
{

#ifdef VALIDATE_PARAMETERS

    DWORD                   i;

#endif // VALIDATE_PARAMETERS

    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(pflPoints && dwPointCount)
    {
        for(i = 0; i < dwPointCount; i++)
        {
            if((pflPoints[i] < 0.0f) || (pflPoints[i] > 1.0f))
            {
                DPF_ERROR("Rolloff curve points must be between 0 and 1");
            }
        }
    }
    else if((!pflPoints && dwPointCount) || (pflPoints && !dwPointCount))
    {
        DPF_ERROR("Point array and count most both be valid or NULL");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_p3dParams->paflRolloffPoints = pflPoints;
    m_pSettings->m_p3dParams->dwRolloffPointCount = dwPointCount;

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_ROLLOFFFACTOR;

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
    LPCDSI3DL2BUFFER        pParams,
    DWORD                   dwFlags
)
{
    HRESULT                 hr;
    
    DPF_ENTER();

#ifdef VALIDATE_PARAMETERS

    if(!pParams)
    {
        DPF_ERROR("Failed to supply parameters");
    }

    if(dwFlags & ~DS3D_VALID)
    {
        DPF_ERROR("Specified invalid or unsupported flags (%lx)", dwFlags & ~DS3D_VALID);
    }

    if((pParams->lDirect < DSI3DL2BUFFER_MINDIRECT) || (pParams->lDirect > DSI3DL2BUFFER_MAXDIRECT))
    {
        DPF_ERROR("Invalid lDirect value");
    }

    if((pParams->lDirectHF < DSI3DL2BUFFER_MINDIRECTHF) || (pParams->lDirectHF > DSI3DL2BUFFER_MAXDIRECTHF))
    {
        DPF_ERROR("Invalid lDirectHF value");
    }

    if((pParams->lRoom < DSI3DL2BUFFER_MINROOM) || (pParams->lRoom > DSI3DL2BUFFER_MAXROOM))
    {
        DPF_ERROR("Invalid lRoom value");
    }

    if((pParams->lRoomHF < DSI3DL2BUFFER_MINROOMHF) || (pParams->lRoomHF > DSI3DL2BUFFER_MAXROOMHF))
    {
        DPF_ERROR("Invalid lRoomHF value");
    }

    if((pParams->flRoomRolloffFactor < DSI3DL2BUFFER_MINROOMROLLOFFFACTOR) || (pParams->flRoomRolloffFactor > DSI3DL2BUFFER_MAXROOMROLLOFFFACTOR))
    {
        DPF_ERROR("Invalid flRoomRolloffFactor value");
    }

    if((pParams->Obstruction.lHFLevel < DSI3DL2BUFFER_MINOBSTRUCTION) || (pParams->Obstruction.lHFLevel > DSI3DL2BUFFER_MAXOBSTRUCTION))
    {
        DPF_ERROR("Invalid Obstruction.lHFLevel value");
    }

    if((pParams->Obstruction.flLFRatio < DSI3DL2BUFFER_MINOBSTRUCTIONLFRATIO) || (pParams->Obstruction.flLFRatio > DSI3DL2BUFFER_MAXOBSTRUCTIONLFRATIO))
    {
        DPF_ERROR("Invalid Obstruction.flLFRatio value");
    }

    if((pParams->Occlusion.lHFLevel < DSI3DL2BUFFER_MINOCCLUSION) || (pParams->Occlusion.lHFLevel > DSI3DL2BUFFER_MAXOCCLUSION))
    {
        DPF_ERROR("Invalid Occlusion.lHFLevel value");
    }

    if((pParams->Occlusion.flLFRatio < DSI3DL2BUFFER_MINOCCLUSIONLFRATIO) || (pParams->Occlusion.flLFRatio > DSI3DL2BUFFER_MAXOCCLUSIONLFRATIO))
    {
        DPF_ERROR("Invalid Occlusion.flLFRatio value");
    }

    if(!(m_pSettings->m_dwFlags & DSBCAPS_CTRL3D))
    {
        DPF_ERROR("3D not enabled");
    }

#endif // VALIDATE_PARAMETERS

    m_pSettings->m_p3dParams->I3dl2Params.lDirect = pParams->lDirect;
    m_pSettings->m_p3dParams->I3dl2Params.lDirectHF = pParams->lDirectHF;
    m_pSettings->m_p3dParams->I3dl2Params.lRoom = pParams->lRoom;
    m_pSettings->m_p3dParams->I3dl2Params.lRoomHF = pParams->lRoomHF;
    m_pSettings->m_p3dParams->I3dl2Params.flRoomRolloffFactor = NormalizeFloat(pParams->flRoomRolloffFactor);
    
    m_pSettings->m_p3dParams->I3dl2Params.Obstruction.lHFLevel = pParams->Obstruction.lHFLevel;
    m_pSettings->m_p3dParams->I3dl2Params.Obstruction.flLFRatio = NormalizeFloat(pParams->Obstruction.flLFRatio);

    m_pSettings->m_p3dParams->I3dl2Params.Occlusion.lHFLevel = pParams->Occlusion.lHFLevel;
    m_pSettings->m_p3dParams->I3dl2Params.Occlusion.flLFRatio = NormalizeFloat(pParams->Occlusion.flLFRatio);

    m_pSettings->m_p3dParams->dwParameterMask |= DS3DPARAM_BUFFER_I3DL2;

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
    DWORD                   dwParameterMask;

    DPF_ENTER();

    //
    // Include the changed listener parameters in the 3D parameter mask
    //
    
    m_pSettings->m_p3dParams->dwParameterMask |= m_pDirectSound->m_pSettings->m_3dParams.dwParameterMask;

    //
    // Apply changes to the voice
    //
    
    dwParameterMask = m_pVoice->Commit3dSettings();

    //
    // Clear the parameter mask
    //

    m_pSettings->m_p3dParams->dwParameterMask &= ~dwParameterMask;

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
    HRESULT                 hr;
    
    DPF_ENTER();

    //
    // Hand off to the base class
    //

    hr = CDirectSoundVoiceSettings::Initialize(pdsbd->dwFlags, (pdsbd->dwFlags & DSBCAPS_SUBMIXMASK) ? &m_wfxMixDest : pdsbd->lpwfxFormat, pdsbd->lpMixBins);

    //
    // Save the input mixbin
    //

    if(SUCCEEDED(hr))
    {
        if(pdsbd->dwFlags & DSBCAPS_FXIN)
        {
            m_dwInputMixBin = pdsbd->dwInputMixBin;
        }
        else if(pdsbd->dwFlags & DSBCAPS_MIXIN)
        {
            m_dwInputMixBin = DSMIXBIN_SUBMIX;
        }
    }
    
    //
    // Initialize buffer data
    //

    if(SUCCEEDED(hr))
    {
        if(pdsbd->dwBufferBytes)
        {
            hr = SetBufferData(NULL, pdsbd->dwBufferBytes);
        }
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

    SetPlayRegion(0, m_dwBufferSize);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetPlayRegion
 *
 *  Description:
 *      Sets buffer play region.
 *
 *  Arguments:
 *      DWORD [in]: starting position of the play region.
 *      DWORD [in]: length, in bytes, of the play region.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBufferSettings::SetPlayRegion"

void
CDirectSoundBufferSettings::SetPlayRegion
(
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength
)
{
    DPF_ENTER();

    ASSERT(dwPlayStart + dwPlayLength <= m_dwBufferSize);
    
    m_dwPlayStart = dwPlayStart;
    m_dwPlayLength = dwPlayLength;

    SetLoopRegion(0, dwPlayLength);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetLoopRegion
 *
 *  Description:
 *      Sets buffer loop region.
 *
 *  Arguments:
 *      DWORD [in]: starting position of the loop region.
 *      DWORD [in]: length, in bytes, of the loop region.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBufferSettings::SetLoopRegion"

void
CDirectSoundBufferSettings::SetLoopRegion
(
    DWORD                   dwLoopStart,
    DWORD                   dwLoopLength
)
{
    DPF_ENTER();

    ASSERT(dwLoopStart + dwLoopLength <= m_dwBufferSize);
    ASSERT(dwLoopStart + dwLoopLength <= m_dwPlayLength);
    
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

        if(!IsValidFormat(pdsbd->lpwfxFormat))
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

    if((pdsbd->dwFlags & DSBCAPS_CTRL3D) && !CHrtfSource::IsValidAlgorithm())
    {
        DPF_ERROR("You must call one of the 3D algorithm initialization functions (DirectSoundUseLightHRTF, DirectSoundUseFullHRTF, DirectSoundUsePan3D) before creating a 3D buffer");
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
            DPF_ERROR("Can't call SetBufferData on MIXIN/FXIN buffers");
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

    hr = m_pBuffer->Play(dwFlags);

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

    hr = m_pBuffer->Play(rtTimeStamp, dwFlags);

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

    hr = m_pBuffer->Stop(rtTimeStamp, dwFlags);

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
 *  SetPlayRegion
 *
 *  Description:
 *      Sets buffer play region.
 *
 *  Arguments:
 *      DWORD [in]: starting position of the play region.
 *      DWORD [in]: length, in bytes, of the play region.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::SetPlayRegion"

HRESULT
CDirectSoundBuffer::SetPlayRegion
(
    DWORD                   dwPlayStart,
    DWORD                   dwPlayLength
)
{
    HRESULT                 hr  = DS_OK;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK)
    {
        DPF_ERROR("Can't call SetPlayRegion on MIXIN/FXIN buffers");
    }

    if(dwPlayStart % 4)
    {
        DPF_ERROR("Play offset not block-aligned");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Munge the length so it's valid
    //

    if(dwPlayLength)
    {

#ifdef VALIDATE_PARAMETERS

        if(dwPlayLength % m_pSettings->m_fmt.nBlockAlign)
        {
            DPF_ERROR("Play length not block-aligned");
        }

#endif // VALIDATE_PARAMETERS

        if(dwPlayStart + dwPlayLength > m_pSettings->m_dwBufferSize)
        {
            DPF_ERROR("Play region extends past the end of the buffer");
            hr = DSERR_INVALIDCALL;
        }
    }
    else
    {
        dwPlayLength = m_pSettings->m_dwBufferSize - dwPlayStart;
    }

    //
    // Update the settings object
    //

    if(SUCCEEDED(hr))
    {
        m_pSettings->SetPlayRegion(dwPlayStart, dwPlayLength);
    }

    //
    // Update the buffer implementation
    //

    if(SUCCEEDED(hr))
    {
        hr = m_pBuffer->SetPlayRegion();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  SetLoopRegion
 *
 *  Description:
 *      Sets buffer loop region.
 *
 *  Arguments:
 *      DWORD [in]: starting position of the loop region.
 *      DWORD [in]: length, in bytes, of the loop region.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundBuffer::SetLoopRegion"

HRESULT
CDirectSoundBuffer::SetLoopRegion
(
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
        DPF_ERROR("Can't call SetLoopRegion on MIXIN/FXIN buffers");
    }

    if(dwLoopStart % m_pSettings->m_fmt.nBlockAlign)
    {
        DPF_ERROR("Loop offset not block-aligned");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Munge the length so it's valid
    //

    if(dwLoopLength)
    {

#ifdef VALIDATE_PARAMETERS

        if(dwLoopLength % m_pSettings->m_fmt.nBlockAlign)
        {
            DPF_ERROR("Loop length not block-aligned");
        }

#endif // VALIDATE_PARAMETERS

        if(dwLoopStart + dwLoopLength > m_pSettings->m_dwPlayLength)
        {
            DPF_ERROR("Loop region extends past the end of the play region");
            hr = DSERR_INVALIDCALL;
        }
    }
    else
    {
        dwLoopLength = m_pSettings->m_dwPlayLength - dwLoopStart;
    }

    //
    // Update the settings object
    //

    if(SUCCEEDED(hr))
    {
        m_pSettings->SetLoopRegion(dwLoopStart, dwLoopLength);
    }

    //
    // Update the buffer implementation
    //

    if(SUCCEEDED(hr))
    {
        hr = m_pBuffer->SetLoopRegion();
    }

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
        DPF_ERROR("Can't call GetCurrentPosition on MIXIN/FXIN buffers");
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
        DPF_ERROR("Can't lock MIXIN/FXIN buffers");
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
        DPF_ERROR("Can't call SetCurrentPosition on MIXIN/FXIN buffers");
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
    ENTER_EXTERNAL_METHOD();

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
        DPF_ERROR("Can't call SetNotificationPositions on MIXIN/FXIN buffers");
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
    HRESULT                 hr;

    DPF_ENTER();

    //
    // Hand off to the base class
    //

    hr = CDirectSoundVoiceSettings::Initialize(pdssd->dwFlags, pdssd->lpwfxFormat, pdssd->lpMixBins);

    //
    // Save stream-specific settings
    //

    if(SUCCEEDED(hr))
    {
        m_dwMaxAttachedPackets = pdssd->dwMaxAttachedPackets;
        m_pfnCallback = pdssd->lpfnCallback;
        m_pvContext = pdssd->lpvContext;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
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

    if(!IsValidFormat(pdssd->lpwfxFormat))
    {
        DPF_ERROR("Invalid stream format");
    }

    if((pdssd->dwFlags & DSBCAPS_CTRL3D) && (1 != pdssd->lpwfxFormat->nChannels))
    {
        DPF_ERROR("3D streams must be mono");
    }

    if((pdssd->dwFlags & DSSTREAMCAPS_CTRL3D) && !CHrtfSource::IsValidAlgorithm())
    {
        DPF_ERROR("You must call one of the 3D algorithm initialization functions (DirectSoundUseLightHRTF, DirectSoundUseFullHRTF, DirectSoundUsePan3D) before creating a 3D stream");
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
    
    pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC;

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
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pdwStatus)
    {
        DPF_ERROR("Status buffer not supplied");
    }

#endif // VALIDATE_PARAMETERS

    hr = m_pStream->GetStatus(pdwStatus);

    DPF_LEAVE_HRESULT(hr);

    return hr;
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
    DWORD                   dwStatus;
    HRESULT                 hr;
    
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

    if(pxmbSource->prtTimestamp)
    {
        if(*pxmbSource->prtTimestamp)
        {
            DPF_ERROR("Timestamps are not supported on DirectSound streams");
        }
    }

    if(pxmbDest)
    {
        DPF_ERROR("DirectSound streams are input only.  No destination packet should be supplied.");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Make sure the stream is accepting data
    //

    hr = m_pStream->GetStatus(&dwStatus);

    if(SUCCEEDED(hr) && !(dwStatus & DSSTREAMSTATUS_READY))
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


/****************************************************************************
 *
 *  PauseEx
 *
 *  Description:
 *      Pauses or resumes a stream.
 *
 *  Arguments:
 *      REFERENCE_TIME [in]: timestamp.
 *      DWORD [in]: stream pause state.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundStream::PauseEx"

HRESULT
CDirectSoundStream::PauseEx
(
    REFERENCE_TIME          rtTimestamp,
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

    hr = m_pStream->Pause(rtTimestamp, dwPause);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  FlushEx
 *
 *  Description:
 *      Flushes the stream.
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
#define DPF_FNAME "CDirectSoundStream::FlushEx"

HRESULT
CDirectSoundStream::FlushEx
(
    REFERENCE_TIME          rtTimeStamp, 
    DWORD                   dwFlags
)
{
    HRESULT                 hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();
    
#ifdef VALIDATE_PARAMETERS

    if(dwFlags & ~DSSTREAMFLUSHEX_VALID)
    {
        DPF_ERROR("Invalid flags");
    }

    if((dwFlags & DSSTREAMFLUSHEX_ENVELOPE) && !(dwFlags & DSSTREAMFLUSHEX_ASYNC))
    {
        DPF_ERROR("ENVELOPE flag requires ASYNC flag");
    }

    if(rtTimeStamp && !(dwFlags & DSSTREAMFLUSHEX_ASYNC))
    {
        DPF_ERROR("Timestamped flush requires ASYNC flag");
    }

#endif // VALIDATE_PARAMETERS

    if(dwFlags)
    {
        hr = m_pStream->Stop(rtTimeStamp, dwFlags);
    }
    else
    {
        hr = m_pStream->Flush();
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


#endif // MCPX_BOOT_LIB


