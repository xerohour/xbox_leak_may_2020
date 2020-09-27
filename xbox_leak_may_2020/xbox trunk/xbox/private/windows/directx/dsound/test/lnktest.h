#include <xtl.h>

#ifdef __cplusplus
#define REF_GUID_NULL GUID_NULL
#else // __cplusplus
#define REF_GUID_NULL 0
#endif // __cplusplus

static void
LinkTest
(
    void
)
{
    DirectSoundCreate(0, 0, 0);
    DirectSoundCreateBuffer(0, 0);
    DirectSoundCreateStream(0, 0);
    DirectSoundDoWork();
    DirectSoundUseFullHRTF();
    DirectSoundUseLightHRTF();
    DirectSoundUsePan3D();
    DirectSoundOverrideSpeakerConfig(0);
    DirectSoundGetSampleTime();
    DirectSoundDumpMemoryUsage(0);
    XAudioCreatePcmFormat(0, 0, 0, 0);
    XAudioCreateAdpcmFormat(0, 0, 0);
    XAudioCalculatePitch(0);
    WmaCreateDecoder(0, 0, 0, 0, 0, 0, 0, 0);
    WmaCreateInMemoryDecoder(0, 0, 0, 0, 0);
    WmaCreateDecoderEx(0, 0, 0, 0, 0, 0, 0, 0);
    WmaCreateInMemoryDecoderEx(0, 0, 0, 0, 0);
    Ac97CreateMediaObject(0, 0, 0, 0);
    XFileCreateMediaObject(0, 0, 0, 0, 0, 0);
    XFileCreateMediaObjectEx(0, 0);
    XWaveFileCreateMediaObject(0, 0, 0);
    XWaveFileCreateMediaObjectEx(0, 0, 0);
    XAudioDownloadEffectsImage(0, 0, 0, 0);
    IDirectSound_QueryInterface(0, REF_GUID_NULL, 0);
    IDirectSound_AddRef(0);
    IDirectSound_Release(0);
    IDirectSound_GetCaps(0, 0);
    IDirectSound_CreateSoundBuffer(0, 0, 0, 0);
    IDirectSound_CreateSoundStream(0, 0, 0, 0);
    IDirectSound_GetSpeakerConfig(0, 0);
    IDirectSound_SetCooperativeLevel(0, 0, 0);
    IDirectSound_Compact(0);
    IDirectSound_DownloadEffectsImage(0, 0, 0, 0, 0);
    IDirectSound_SetEffectData(0, 0, 0, 0, 0, 0);
    IDirectSound_CommitEffectData(0);
    IDirectSound_EnableHeadphones(0, 0);
    IDirectSound_SetMixBinHeadroom(0, 0, 0);
    IDirectSound_SetAllParameters(0, 0, 0);
    IDirectSound_SetDistanceFactor(0, 0, 0);
    IDirectSound_SetDopplerFactor(0, 0, 0);
    IDirectSound_SetOrientation(0, 0, 0, 0, 0, 0, 0, 0);
    IDirectSound_SetPosition(0, 0, 0, 0, 0);
    IDirectSound_SetRolloffFactor(0, 0, 0);
    IDirectSound_SetVelocity(0, 0, 0, 0, 0);
    IDirectSound_SetI3DL2Listener(0, 0, 0);
    IDirectSound_CommitDeferredSettings(0);
    IDirectSound_GetTime(0, 0);
    IDirectSoundBuffer_QueryInterface(0, REF_GUID_NULL, 0);
    IDirectSoundBuffer_AddRef(0);
    IDirectSoundBuffer_Release(0);
    IDirectSoundBuffer_SetFrequency(0, 0);
    IDirectSoundBuffer_SetVolume(0, 0);
    IDirectSoundBuffer_SetPitch(0, 0);
    IDirectSoundBuffer_SetLFO(0, 0);
    IDirectSoundBuffer_SetEG(0, 0);
    IDirectSoundBuffer_SetFilter(0, 0);
    IDirectSoundBuffer_SetOutputBuffer(0, 0);
    IDirectSoundBuffer_SetMixBins(0, 0);
    IDirectSoundBuffer_SetMixBinVolumes(0, 0);
    IDirectSoundBuffer_SetAllParameters(0, 0, 0);
    IDirectSoundBuffer_SetConeAngles(0, 0, 0, 0);
    IDirectSoundBuffer_SetConeOrientation(0, 0, 0, 0, 0);
    IDirectSoundBuffer_SetConeOutsideVolume(0, 0, 0);
    IDirectSoundBuffer_SetMaxDistance(0, 0, 0);
    IDirectSoundBuffer_SetMinDistance(0, 0, 0);
    IDirectSoundBuffer_SetMode(0, 0, 0);
    IDirectSoundBuffer_SetPosition(0, 0, 0, 0, 0);
    IDirectSoundBuffer_SetVelocity(0, 0, 0, 0, 0);
    IDirectSoundBuffer_SetRolloffFactor(0, 0, 0);
    IDirectSoundBuffer_SetRolloffCurve(0, 0, 0, 0);
    IDirectSoundBuffer_SetI3DL2Source(0, 0, 0);
    IDirectSoundBuffer_Play(0, 0, 0, 0);
    IDirectSoundBuffer_PlayEx(0, 0, 0);
    IDirectSoundBuffer_Stop(0);
    IDirectSoundBuffer_StopEx(0, 0, 0);
    IDirectSoundBuffer_SetPlayRegion(0, 0, 0);
    IDirectSoundBuffer_SetLoopRegion(0, 0, 0);
    IDirectSoundBuffer_GetStatus(0, 0);
    IDirectSoundBuffer_GetCurrentPosition(0, 0, 0);
    IDirectSoundBuffer_SetCurrentPosition(0, 0);
    IDirectSoundBuffer_SetBufferData(0, 0, 0);
    IDirectSoundBuffer_Lock(0, 0, 0, 0, 0, 0, 0, 0);
    IDirectSoundBuffer_Unlock(0, 0, 0, 0, 0);
    IDirectSoundBuffer_Restore(0);
    IDirectSoundBuffer_SetNotificationPositions(0, 0, 0);
    IDirectSoundStream_QueryInterface(0, REF_GUID_NULL, 0);
    IDirectSoundStream_AddRef(((LPDIRECTSOUNDSTREAM)0));
    IDirectSoundStream_Release(((LPDIRECTSOUNDSTREAM)0));
    IDirectSoundStream_GetInfo(((LPDIRECTSOUNDSTREAM)0), 0);
    IDirectSoundStream_GetStatus(((LPDIRECTSOUNDSTREAM)0), 0);
    IDirectSoundStream_Process(((LPDIRECTSOUNDSTREAM)0), 0, 0);
    IDirectSoundStream_Discontinuity(((LPDIRECTSOUNDSTREAM)0));
    IDirectSoundStream_Flush(((LPDIRECTSOUNDSTREAM)0));
    IDirectSoundStream_SetFrequency(0, 0);
    IDirectSoundStream_SetVolume(0, 0);
    IDirectSoundStream_SetPitch(0, 0);
    IDirectSoundStream_SetLFO(0, 0);
    IDirectSoundStream_SetEG(0, 0);
    IDirectSoundStream_SetFilter(0, 0);
    IDirectSoundStream_SetOutputBuffer(0, 0);
    IDirectSoundStream_SetMixBins(0, 0);
    IDirectSoundStream_SetMixBinVolumes(0, 0);
    IDirectSoundStream_SetAllParameters(0, 0, 0);
    IDirectSoundStream_SetConeAngles(0, 0, 0, 0);
    IDirectSoundStream_SetConeOrientation(0, 0, 0, 0, 0);
    IDirectSoundStream_SetConeOutsideVolume(0, 0, 0);
    IDirectSoundStream_SetMaxDistance(0, 0, 0);
    IDirectSoundStream_SetMinDistance(0, 0, 0);
    IDirectSoundStream_SetMode(0, 0, 0);
    IDirectSoundStream_SetPosition(0, 0, 0, 0, 0);
    IDirectSoundStream_SetVelocity(0, 0, 0, 0, 0);
    IDirectSoundStream_SetRolloffFactor(0, 0, 0);
    IDirectSoundStream_SetRolloffCurve(0, 0, 0, 0);
    IDirectSoundStream_SetI3DL2Source(0, 0, 0);
    IDirectSoundStream_Pause(0, 0);
    IDirectSoundStream_FlushEx(0, 0, 0);
}


