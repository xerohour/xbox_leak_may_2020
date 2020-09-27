/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xactapi.cpp
 *  Content:    XACT runtime Engine API objects and entry points.
 *  History:
 *  Date        By        Reason
 *  ====        ==        ======
 *  1/22/2002   georgioc  Created.
 *
 ****************************************************************************/

#include "xacti.h"
#include "xboxdbg.h"

#pragma comment(linker, "/merge:XACTENG_RW=XACTENG")
#pragma comment(linker, "/merge:XACTENG_URW=XACTENG")
#pragma comment(linker, "/merge:XACTENG_RD=XACTENG")
#pragma comment(linker, "/section:XACTENG,ERW")

    
INITIALIZED_CRITICAL_SECTION(g_XACTCriticalSection);

STDAPI_(ULONG) IXACTEngine_AddRef(PXACTENGINE pEngine)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->AddRef();
}

STDAPI_(ULONG) IXACTEngine_Release(PXACTENGINE pEngine)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->Release();
}


STDAPI IXACTEngine_LoadDspImage(PXACTENGINE pEngine, PVOID pvBuffer, DWORD dwSize, LPCDSEFFECTIMAGELOC pEffectLoc)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->LoadDspImage(pvBuffer, dwSize, pEffectLoc);
}

STDAPI IXACTEngine_CreateSoundSource(PXACTENGINE pEngine, DWORD dwFlags, PXACTSOUNDSOURCE *ppSoundSource)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->CreateSoundSource(dwFlags, ppSoundSource);
}

STDAPI IXACTEngine_CreateSoundBank(PXACTENGINE pEngine, PVOID pvBuffer, DWORD dwSize, PXACTSOUNDBANK *ppSoundBank)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->CreateSoundBank(pvBuffer, dwSize, ppSoundBank);
}

STDAPI IXACTEngine_RegisterWaveBank(PXACTENGINE pEngine, PVOID pvData, DWORD dwSize, PXACTWAVEBANK *ppWaveBank)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->RegisterWaveBank(pvData, dwSize, ppWaveBank);
}

STDAPI IXACTEngine_RegisterStreamedWaveBank(PXACTENGINE pEngine, PVOID pvStreamingBuffer, DWORD dwSize, HANDLE hFileHandle, DWORD dwOffset, PXACTWAVEBANK *ppWaveBank)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->RegisterStreamedWaveBank(pvStreamingBuffer, dwSize, hFileHandle, dwOffset, ppWaveBank);
}

STDAPI IXACTEngine_UnRegisterWaveBank(PXACTENGINE pEngine, PXACTWAVEBANK pWaveBank)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->UnRegisterWaveBank(pWaveBank);
}

STDAPI IXACTEngine_SetMasterVolume(PXACTENGINE pEngine, LONG lVolume)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->SetMasterVolume(lVolume);
}

STDAPI IXACTEngine_SetListenerParameters(PXACTENGINE pEngine, LPCDS3DLISTENER pcDs3dListener, LPCDSI3DL2LISTENER pds3dl, DWORD dwApply)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->SetListenerParameters(pcDs3dListener, pds3dl, dwApply);
}

STDAPI IXACTEngine_GlobalPause(PXACTENGINE pEngine, BOOL bPause)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->GlobalPause(bPause);
}

STDAPI IXACTEngine_RegisterNotification(PXACTENGINE pEngine, PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->RegisterNotification(pNotificationDesc);
}

STDAPI IXACTEngine_UnRegisterNotification(PXACTENGINE pEngine, PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->UnRegisterNotification(pNotificationDesc);
}

STDAPI IXACTEngine_GetNotification(PXACTENGINE pEngine, PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc, PXACT_NOTIFICATION pNotification)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->GetNotification(pNotificationDesc, pNotification);
}

STDAPI IXACTEngine_FlushNotification(PXACTENGINE pEngine, PXACT_NOTIFICATION_DESCRIPTION pNotificationDesc)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->FlushNotification(pNotificationDesc);
}

STDAPI IXACTEngine_ScheduleEvent(PXACTENGINE pEngine, XACT_TRACK_EVENT *pEventDesc, PXACTSOUNDCUE pSoundCue, DWORD dwTrackIndex)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->ScheduleEvent(pEventDesc, pSoundCue, dwTrackIndex);
}

STDAPI IXACTEngine_CommitDeferredSettings(PXACTENGINE pEngine)
{
    using namespace XACT;
    return ((CEngine *)pEngine)->CommitDeferredSettings();
}

//
// soundbank apis
//

STDAPI_(ULONG) IXACTSoundBank_AddRef(PXACTSOUNDBANK pBank)
{
    using namespace XACT;
    return ((CSoundBank *)pBank)->AddRef();
}
STDAPI_(ULONG) IXACTSoundBank_Release(PXACTSOUNDBANK pBank)
{
    using namespace XACT;
    return ((CSoundBank *)pBank)->Release();
}

STDAPI IXACTSoundBank_GetSoundCueIndexFromFriendlyName(PXACTSOUNDBANK pBank, LPCSTR lpFriendlyName, PDWORD pdwCueIndex)
{
    using namespace XACT;
    return ((CSoundBank *)pBank)->GetSoundCueIndexFromFriendlyName(lpFriendlyName, pdwCueIndex);
}

STDAPI IXACTSoundBank_Play(PXACTSOUNDBANK pBank, DWORD dwCueIndex, PXACTSOUNDSOURCE pSoundSource, DWORD dwFlags, PXACTSOUNDCUE *ppCue)
{
    using namespace XACT;
    return ((CSoundBank *)pBank)->Play(dwCueIndex, pSoundSource, dwFlags, ppCue);
}

STDAPI IXACTSoundBank_Stop(PXACTSOUNDBANK pBank, DWORD dwCueIndex, DWORD dwFlags, PXACTSOUNDCUE pCue)
{
    using namespace XACT;
    return ((CSoundBank *)pBank)->Stop(dwCueIndex, dwFlags, pCue);
}


//
// SoundSource apis
//

STDAPI_(ULONG) IXACTSoundSource_AddRef(PXACTSOUNDSOURCE pSoundSource)
{
    using namespace XACT;
    return ((CSoundSource *)pSoundSource)->AddRef();
}

STDAPI_(ULONG) IXACTSoundSource_Release(PXACTSOUNDSOURCE pSoundSource)
{
    using namespace XACT;
    return ((CSoundSource *)pSoundSource)->Release();
}

STDAPI IXACTSoundSource_SetPosition(PXACTSOUNDSOURCE pSoundSource, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply)
{
    using namespace XACT;
    return ((CSoundSource *)pSoundSource)->SetPosition(x, y, z, dwApply);
}

STDAPI IXACTSoundSource_SetAllParameters(PXACTSOUNDSOURCE pSoundSource, LPCDS3DBUFFER pcDs3dBuffer, DWORD dwApply)
{
    using namespace XACT;
    return ((CSoundSource *)pSoundSource)->SetAllParameters(pcDs3dBuffer, dwApply);
}

STDAPI IXACTSoundSource_SetConeOrientation(PXACTSOUNDSOURCE pSoundSource,FLOAT x, FLOAT y, FLOAT z, DWORD dwApply)
{
    using namespace XACT;
    return ((CSoundSource *)pSoundSource)->SetConeOrientation(x, y, z, dwApply);
}

STDAPI IXACTSoundSource_SetI3DL2Source(PXACTSOUNDSOURCE pSoundSource,LPCDSI3DL2BUFFER pds3db, DWORD dwApply)
{
    using namespace XACT;
    return ((CSoundSource *)pSoundSource)->SetI3DL2Source(pds3db,dwApply);
}

STDAPI IXACTSoundSource_SetVelocity(PXACTSOUNDSOURCE pSoundSource,FLOAT x, FLOAT y, FLOAT z, DWORD dwApply)
{
    using namespace XACT;
    return ((CSoundSource *)pSoundSource)->SetVelocity(x, y, z, dwApply);
}

STDAPI IXACTSoundSource_SetMixBins(PXACTSOUNDSOURCE pSoundSource, LPCDSMIXBINS pMixBins)
{
    using namespace XACT;
    return ((CSoundSource *)pSoundSource)->SetMixBins(pMixBins);
}

STDAPI IXACTSoundSource_SetMixBinVolumes(PXACTSOUNDSOURCE pSoundSource, LPCDSMIXBINS pMixBins)
{
    using namespace XACT;
    return ((CSoundSource *)pSoundSource)->SetMixBinVolumes(pMixBins);
}


