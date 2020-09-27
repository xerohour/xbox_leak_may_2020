// Copyright Microsoft, Inc. All Rights Reserved.

#ifndef __DSOUNDSEQUENCER_INCLUDED__
#define __DSOUNDSEQUENCER_INCLUDED__

#include <dsound.h>

typedef LONGLONG SEQUENCE_NUMBER, *PSEQUENCE_NUMBER, *LPSEQUENCE_NUMBER;

typedef struct IDirectSoundSequencer IDirectSoundSequencer;
typedef IDirectSoundSequencer *LPDIRECTSOUNDSEQUENCER;

typedef struct IDirectSoundSequencerBuffer IDirectSoundSequencerBuffer;
typedef IDirectSoundSequencerBuffer *LPDIRECTSOUNDSEQUENCERBUFFER;

typedef struct IDirectSoundSequencerStream IDirectSoundSequencerStream;
typedef IDirectSoundSequencerStream *LPDIRECTSOUNDSEQUENCERSTREAM;

//
// Buffer status flags
//

#define DSSBSTATUS_PLAYING           0x00000001      // The buffer is playing
#define DSSBSTATUS_LOOPING           0x00000004      // The buffer is playing in a loop
#define DSSBSTATUS_PENDINGPLAYEVENTS 0x80000000      // There are play events waiting to be processed

//
// Stream status flags
//

#define DSSSSTATUS_PENDINGEVENTS     0x80000000      // There are events waiting to be processed

                                                                            
EXTERN_C HRESULT WINAPI DirectSoundCreateSequencer(LPGUID pguidDeviceId, DWORD maxEvents, LPDIRECTSOUNDSEQUENCER *ppSequencer);

#undef INTERFACE
#define INTERFACE IDirectSoundSequencer

DECLARE_INTERFACE(IDirectSoundSequencer)
{
public:
    // IUnknown methods
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    STDMETHOD(GetTime)(THIS_ LPREFERENCE_TIME prtCurrent) PURE;
    STDMETHOD(SetTime)(THIS_ REFERENCE_TIME rtCurrent) PURE;
    STDMETHOD(SetAbsoluteTime)(THIS_ REFERENCE_TIME rtSystemTime, REFERENCE_TIME rtCurrent) PURE;

    STDMETHOD(CreateBuffer)(THIS_ LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDSEQUENCERBUFFER* ppBuffer) PURE;
    STDMETHOD(CreateStream)(THIS_ LPCDSSTREAMDESC pdssd, LPDIRECTSOUNDSEQUENCERSTREAM* ppStream) PURE;
};

#define IDirectSoundSequencer_AddRef             IUnknown_AddRef
#define IDirectSoundSequencer_Release            IUnknown_Release

#if defined(__cplusplus) && !defined(CINTERFACE)

#define IDirectSoundSequencer_GetTime(p, a)                              p->GetTime(a)
#define IDirectSoundSequencer_SetTime(p, a)                              p->SetTime(a)
#define IDirectSoundSequencer_SetAbsoluteTime(p, a, b)                   p->SetAbsoluteTime(a, b)
#define IDirectSoundSequencer_CreateBuffer(p, a, b)                      p->CreateBuffer(a, b)
#define IDirectSoundSequencer_CreateStream(p, a, b)                      p->CreateStream(a, b)

#else

#define IDirectSoundSequencer_GetTime(p, a)                              p->lpVtbl->GetTime(a)
#define IDirectSoundSequencer_SetTime(p, a)                              p->lpVtbl->SetTime(a)
#define IDirectSoundSequencer_SetAbsoluteTime(p, a, b)                   p->lpVtbl->SetAbsoluteTime(a, b)
#define IDirectSoundSequencer_CreateBuffer(p, a, b)                      p->lpVtbl->CreateBuffer(a, b)
#define IDirectSoundSequencer_CreateStream(p, a, b)                      p->lpVtbl->CreateStream(a, b)

#endif

#undef INTERFACE
#define INTERFACE IDirectSoundSequencerBuffer

DECLARE_INTERFACE(IDirectSoundSequencerBuffer)
{
public:
    // IUnknown methods
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    STDMETHOD(Clear)(THIS); // Clear all pending events
    STDMETHOD(ClearAtOrAfter)(THIS_ REFERENCE_TIME prtClearTime);

    STDMETHOD(GetSequenceNumber)(THIS_ LPSEQUENCE_NUMBER pSequenceNumber) PURE; // Associated with next event to be queued
    STDMETHOD(GetStatus)        (THIS_ LPDWORD pdwStatus) PURE;

    // Errors are reported asynchronously. Only one error is recorded at a time for a given buffer.
    // Once an error is recorded, no further errors are recorded for that buffer until ClearError is called.

    STDMETHOD(GetError)(THIS_ HRESULT* pError) PURE; // returns S_OK if no error is recorded
    STDMETHOD(GetErrorSequenceNumber)(THIS_ LPSEQUENCE_NUMBER pSequenceNumber) PURE;
    STDMETHOD(GetErrorTime)(THIS_ LPREFERENCE_TIME prtErrorTime) PURE;
    STDMETHOD(ClearError)(THIS) PURE;

    // DirectSoundBuffer methods

    STDMETHOD(Play)                (THIS_ REFERENCE_TIME timeStamp, DWORD dwReserved1, DWORD dwReserved2, DWORD dwFlags) PURE;
    STDMETHOD(Stop)                (THIS_ REFERENCE_TIME timeStamp) PURE;
    STDMETHOD(StopEx)              (THIS_ REFERENCE_TIME timeStamp, DWORD dwFlags) PURE;
    STDMETHOD(SetPlayRegion)       (THIS_ REFERENCE_TIME timeStamp, DWORD dwPlayStart, DWORD dwPlayLength) PURE;
    STDMETHOD(SetLoopRegion)       (THIS_ REFERENCE_TIME timeStamp, DWORD dwLoopStart, DWORD dwLoopLength) PURE;
    STDMETHOD(SetCurrentPosition)  (THIS_ REFERENCE_TIME timeStamp, DWORD dwPlayCursor) PURE;
    STDMETHOD(SetBufferData)       (THIS_ REFERENCE_TIME timeStamp, LPVOID pvBufferData, DWORD dwBufferBytes) PURE;
    STDMETHOD(SetFrequency)        (THIS_ REFERENCE_TIME timeStamp, DWORD dwFrequency) PURE;
    STDMETHOD(SetPitch)            (THIS_ REFERENCE_TIME timeStamp, LONG lPitch) PURE;
    STDMETHOD(SetVolume)           (THIS_ REFERENCE_TIME timeStamp, LONG lVolume) PURE;
    STDMETHOD(SetLFO)              (THIS_ REFERENCE_TIME timeStamp, LPCDSLFODESC pLFODesc) PURE;
    STDMETHOD(SetEG)               (THIS_ REFERENCE_TIME timeStamp, LPCDSENVELOPEDESC pEnvelopeDesc) PURE;
    STDMETHOD(SetFilter)           (THIS_ REFERENCE_TIME timeStamp, LPCDSFILTERDESC pFilterDesc) PURE;
    STDMETHOD(SetOutputBuffer)     (THIS_ REFERENCE_TIME timeStamp, LPDIRECTSOUNDBUFFER pOutputBuffer) PURE;
    STDMETHOD(SetMixBins)          (THIS_ REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins) PURE;
    STDMETHOD(SetMixBinVolumes)    (THIS_ REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins) PURE;

    // IDirectSound3DBuffer methods
    STDMETHOD(SetAllParameters)    (THIS_ REFERENCE_TIME timeStamp, LPCDS3DBUFFER pds3db, DWORD dwApply) PURE;
    STDMETHOD(SetConeAngles)       (THIS_ REFERENCE_TIME timeStamp, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply) PURE;
    STDMETHOD(SetConeOrientation)  (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) PURE;
    STDMETHOD(SetConeOutsideVolume)(THIS_ REFERENCE_TIME timeStamp, LONG lConeOutsideVolume, DWORD dwApply) PURE;
    STDMETHOD(SetMaxDistance)      (THIS_ REFERENCE_TIME timeStamp, FLOAT flMaxDistance, DWORD dwApply) PURE;
    STDMETHOD(SetMinDistance)      (THIS_ REFERENCE_TIME timeStamp, FLOAT flMinDistance, DWORD dwApply) PURE;
    STDMETHOD(SetMode)             (THIS_ REFERENCE_TIME timeStamp, DWORD dwMode, DWORD dwApply) PURE;
    STDMETHOD(SetPosition)         (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) PURE;
    STDMETHOD(SetVelocity)         (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) PURE;

    // IDirectSoundNotify methods
    STDMETHOD(SetNotificationPositions)(THIS_ REFERENCE_TIME sampleTime, DWORD dwNotifyCount, LPCDSBPOSITIONNOTIFY paNotifies) PURE;
};

#define IDirectSoundSequencerBuffer_AddRef             IUnknown_AddRef
#define IDirectSoundSequencerBuffer_Release            IUnknown_Release

#if defined(__cplusplus) && !defined(CINTERFACE)

#define IDirectSoundSequencerBuffer_Clear(p)                             p->Clear()
#define IDirectSoundSequencerBuffer_ClearAtOrAfter(p, a)                 p->ClearAtOrAfter(a)
#define IDirectSoundSequencerBuffer_GetSequenceNumber(p, a)              p->GetSequenceNumber(a)
#define IDirectSoundSequencerBuffer_GetStatus(p, a)                      p->GetStatus(a)
#define IDirectSoundSequencerBuffer_GetError(p, a)                       p->GetError(a)
#define IDirectSoundSequencerBuffer_GetErrorSequenceNumber(p, a)         p->GetErrorSequenceNumber(a)
#define IDirectSoundSequencerBuffer_GetErrorSampleTime(p, a)             p->GetErrorSampleTime(a)
#define IDirectSoundSequencerBuffer_ClearError(p)                        p->ClearError()
#define IDirectSoundSequencerBuffer_Play(p, a, b, c, d)                  p->Play(a, b, c, d)
#define IDirectSoundSequencerBuffer_Stop(p, a)                           p->Stop(a)
#define IDirectSoundSequencerBuffer_StopEx(p, a, b)                      p->StopEx(a, b)
#define IDirectSoundSequencerBuffer_SetPlayRegion(p, a, b, c)            p->SetPlayRegion(a, b, c)
#define IDirectSoundSequencerBuffer_SetLoopRegion(p, a, b, c)            p->SetLoopRegion(a, b, c)
#define IDirectSoundSequencerBuffer_SetCurrentPosition(p, a, b)          p->SetCurrentPosition(a, b)
#define IDirectSoundSequencerBuffer_SetBufferData(p, a, b, c)            p->SetBufferData(a, b, c)
#define IDirectSoundSequencerBuffer_SetFrequency(p, a, b)                p->SetFrequency(a, b)
#define IDirectSoundSequencerBuffer_SetPitch(p, a, b)                    p->SetPitch(a, b)
#define IDirectSoundSequencerBuffer_SetVolume(p, a, b)                   p->SetVolume(a, b)
#define IDirectSoundSequencerBuffer_SetLFO(p, a, b)                      p->SetLFO(a, b)
#define IDirectSoundSequencerBuffer_SetEG(p, a, b)                       p->SetEG(a, b)
#define IDirectSoundSequencerBuffer_SetFilter(p, a, b)                   p->SetFilter(a, b)
#define IDirectSoundSequencerBuffer_SetOutputBuffer(p, a, b)             p->SetOutputBuffer(a, b)
#define IDirectSoundSequencerBuffer_SetMixBins(p, a, b)                  p->SetMixBins(a, b)
#define IDirectSoundSequencerBuffer_SetMixBinVolumes(p, a, b, c)         p->SetMixBinVolumes(a, b, c)
#define IDirectSoundSequencerBuffer_SetAllParameters(p, a, b, c)         p->SetAllParameters(a, b, c)
#define IDirectSoundSequencerBuffer_SetConeAngles(p, a, b, c, d)         p->SetConeAngles(a, b, c, d)
#define IDirectSoundSequencerBuffer_SetConeOrientation(p, a, b, c, d, e) p->SetConeOrientation(a, b, c, d, e)
#define IDirectSoundSequencerBuffer_SetConeOutsideVolume(p, a, b, c)     p->SetConeOutsideVolume(a, b, c)
#define IDirectSoundSequencerBuffer_SetMaxDistance(p, a, b, c)           p->SetMaxDistance(a, b, c)
#define IDirectSoundSequencerBuffer_SetMinDistance(p, a, b, c)           p->SetMinDistance(a, b, c)
#define IDirectSoundSequencerBuffer_SetMode(p, a, b, c)                  p->SetMode(a, b, c)
#define IDirectSoundSequencerBuffer_SetPosition(p, a, b, c, d, e)        p->SetPosition(a, b, c, d, e)
#define IDirectSoundSequencerBuffer_SetVelocity(p, a, b, c, d, e)        p->SetVelocity(a, b, c, d, e)
#define IDirectSoundSequencerBuffer_SetNotificationPositions(p, a, b, c) p->SetNotificationPositions(a, b, c)

#else // defined(__cplusplus) && !defined(CINTERFACE)

#define IDirectSoundSequencerBuffer_Clear(p)                             p->lpVtbl->Clear()
#define IDirectSoundSequencerBuffer_ClearAtOrAfter(p, a)                 p->lpVtbl->ClearAtOrAfter(a)
#define IDirectSoundSequencerBuffer_GetSequenceNumber(p, a)              p->lpVtbl->GetSequenceNumber(a)
#define IDirectSoundSequencerBuffer_GetStatus(p, a)                      p->lpVtbl->GetStatus(a)
#define IDirectSoundSequencerBuffer_GetError(p, a)                       p->lpVtbl->GetError(a)
#define IDirectSoundSequencerBuffer_GetErrorSequenceNumber(p, a)         p->lpVtbl->GetErrorSequenceNumber(a)
#define IDirectSoundSequencerBuffer_GetErrorSampleTime(p, a)             p->lpVtbl->GetErrorSampleTime(a)
#define IDirectSoundSequencerBuffer_ClearError(p)                        p->lpVtbl->ClearError()
#define IDirectSoundSequencerBuffer_Play(p, a, b, c, d)                  p->lpVtbl->Play(a, b, c, d)
#define IDirectSoundSequencerBuffer_Stop(p, a)                           p->lpVtbl->Stop(a)
#define IDirectSoundSequencerBuffer_StopEx(p, a, b)                      p->lpVtbl->StopEx(a, b)
#define IDirectSoundSequencerBuffer_SetPlayRegion(p, a, b, c)            p->lpVtbl->SetPlayRegion(a, b, c)
#define IDirectSoundSequencerBuffer_SetLoopRegion(p, a, b, c)            p->lpVtbl->SetLoopRegion(a, b, c)
#define IDirectSoundSequencerBuffer_SetCurrentPosition(p, a, b)          p->lpVtbl->SetCurrentPosition(a, b)
#define IDirectSoundSequencerBuffer_SetBufferData(p, a, b, c)            p->lpVtbl->SetBufferData(a, b, c)
#define IDirectSoundSequencerBuffer_SetFrequency(p, a, b)                p->lpVtbl->SetFrequency(a, b)
#define IDirectSoundSequencerBuffer_SetPitch(p, a, b)                    p->lpVtbl->SetPitch(a, b)
#define IDirectSoundSequencerBuffer_SetVolume(p, a, b)                   p->lpVtbl->SetVolume(a, b)
#define IDirectSoundSequencerBuffer_SetLFO1(p, a, b)                     p->lpVtbl->SetLFO(a, b)
#define IDirectSoundSequencerBuffer_SetEG(p, a, b)                       p->lpVtbl->SetEG(a, b)
#define IDirectSoundSequencerBuffer_SetFilter(p, a, b)                   p->lpVtbl->SetFilter(a, b)
#define IDirectSoundSequencerBuffer_SetOutputBuffer(p, a, b)             p->lpVtbl->SetOutputBuffer(a, b)
#define IDirectSoundSequencerBuffer_SetMixBins(p, a, b)                  p->lpVtbl->SetMixBins(a, b)
#define IDirectSoundSequencerBuffer_SetMixBinVolumes(p, a, b, c)         p->lpVtbl->SetMixBinVolumes(a, b, c)
#define IDirectSoundSequencerBuffer_SetAllParameters(p, a, b, c)         p->lpVtbl->SetAllParameters(a, b, c)
#define IDirectSoundSequencerBuffer_SetConeAngles(p, a, b, c, d)         p->lpVtbl->SetConeAngles(a, b, c, d)
#define IDirectSoundSequencerBuffer_SetConeOrientation(p, a, b, c, d, e) p->lpVtbl->SetConeOrientation(a, b, c, d, e)
#define IDirectSoundSequencerBuffer_SetConeOutsideVolume(p, a, b, c)     p->lpVtbl->SetConeOutsideVolume(a, b, c)
#define IDirectSoundSequencerBuffer_SetMaxDistance(p, a, b, c)           p->lpVtbl->SetMaxDistance(a, b, c)
#define IDirectSoundSequencerBuffer_SetMinDistance(p, a, b, c)           p->lpVtbl->SetMinDistance(a, b, c)
#define IDirectSoundSequencerBuffer_SetMode(p, a, b, c)                  p->lpVtbl->SetMode(a, b, c)
#define IDirectSoundSequencerBuffer_SetPosition(p, a, b, c, d, e)        p->lpVtbl->SetPosition(a, b, c, d, e)
#define IDirectSoundSequencerBuffer_SetVelocity(p, a, b, c, d, e         p->lpVtbl->SetVelocity(a, b, c, d, e)
#define IDirectSoundSequencerBuffer_SetNotificationPositions(p, a, b, c) p->lpVtbl->SetNotificationPositions(a, b, c)

#endif // defined(__cplusplus) && !defined(CINTERFACE)

#define XMEDIAPACKET_STATUS_NOT_YET_SUBMITTED 42 // The status of a queued, but not yet submitted, packet

#undef INTERFACE
#define INTERFACE IDirectSoundSequencerStream

DECLARE_INTERFACE(IDirectSoundSequencerStream)
{
public:
    // IUnknown methods
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    STDMETHOD(Clear)(THIS); // Clear all pending events
    STDMETHOD(ClearAtOrAfter)(THIS_ REFERENCE_TIME rtTimeStamp);

    STDMETHOD(GetSequenceNumber)(THIS_ LPSEQUENCE_NUMBER pSequenceNumber) PURE; // Associated with next event to be queued
    STDMETHOD(GetStatus)        (THIS_ LPDWORD pdwStatus) PURE;
    STDMETHOD(GetPacketStatus)  (THIS_ DWORD dwPacketIndex, LPDWORD pdwStatus) PURE;

    // Errors are reported asynchronously. Only one error is recorded at a time for a given buffer.
    // Once an error is recorded, no further errors are recorded for that buffer until ClearError is called.

    STDMETHOD(GetError)(THIS_ HRESULT* pError) PURE; // returns S_OK if no error is recorded
    STDMETHOD(GetErrorSequenceNumber)(THIS_ LPSEQUENCE_NUMBER pSequenceNumber) PURE;
    STDMETHOD(GetErrorTime)(THIS_ LPREFERENCE_TIME prtErrorTime) PURE;
    STDMETHOD(ClearError)(THIS) PURE;

    // XMediaObject methods
    STDMETHOD(Process)(THIS_ REFERENCE_TIME timeStamp, LPCXMEDIAPACKET pInputPacket, LPDWORD pdwPacketIndex) PURE;
    STDMETHOD(Discontinuity)(THIS_ REFERENCE_TIME timeStamp) PURE;
    STDMETHOD(Flush)(THIS_ REFERENCE_TIME timeStamp) PURE;

    // IDirectSoundStream methods

    STDMETHOD(SetFrequency)        (THIS_ REFERENCE_TIME timeStamp, DWORD dwFrequency) PURE;
    STDMETHOD(SetPitch)            (THIS_ REFERENCE_TIME timeStamp, LONG lPitch) PURE;
    STDMETHOD(SetVolume)           (THIS_ REFERENCE_TIME timeStamp, LONG lVolume) PURE;
    STDMETHOD(SetLFO)              (THIS_ REFERENCE_TIME timeStamp, LPCDSLFODESC pLFODesc) PURE;
    STDMETHOD(SetEG)               (THIS_ REFERENCE_TIME timeStamp, LPCDSENVELOPEDESC pEnvelopeDesc) PURE;
    STDMETHOD(SetFilter)           (THIS_ REFERENCE_TIME timeStamp, LPCDSFILTERDESC pFilterDesc) PURE;
    STDMETHOD(SetOutputBuffer)     (THIS_ REFERENCE_TIME timeStamp, LPDIRECTSOUNDBUFFER pOutputBuffer) PURE;
    STDMETHOD(SetMixBins)          (THIS_ REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins) PURE;
    STDMETHOD(SetMixBinVolumes)    (THIS_ REFERENCE_TIME timeStamp, LPCDSMIXBINS pMixBins) PURE;
    STDMETHOD(Pause)               (THIS_ REFERENCE_TIME timeStamp, DWORD dwPause) PURE;

    // IDirectSound3DBuffer methods
    STDMETHOD(SetAllParameters)    (THIS_ REFERENCE_TIME timeStamp, LPCDS3DBUFFER pds3db, DWORD dwApply) PURE;
    STDMETHOD(SetConeAngles)       (THIS_ REFERENCE_TIME timeStamp, DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwApply) PURE;
    STDMETHOD(SetConeOrientation)  (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) PURE;
    STDMETHOD(SetConeOutsideVolume)(THIS_ REFERENCE_TIME timeStamp, LONG lConeOutsideVolume, DWORD dwApply) PURE;
    STDMETHOD(SetMaxDistance)      (THIS_ REFERENCE_TIME timeStamp, FLOAT flMaxDistance, DWORD dwApply) PURE;
    STDMETHOD(SetMinDistance)      (THIS_ REFERENCE_TIME timeStamp, FLOAT flMinDistance, DWORD dwApply) PURE;
    STDMETHOD(SetMode)             (THIS_ REFERENCE_TIME timeStamp, DWORD dwMode, DWORD dwApply) PURE;
    STDMETHOD(SetPosition)         (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) PURE;
    STDMETHOD(SetVelocity)         (THIS_ REFERENCE_TIME timeStamp, FLOAT x, FLOAT y, FLOAT z, DWORD dwApply) PURE;

};

#define IDirectSoundSequencerStream_AddRef             IUnknown_AddRef
#define IDirectSoundSequencerStream_Release            IUnknown_Release

#if defined(__cplusplus) && !defined(CINTERFACE)

#define IDirectSoundSequencerStream_Clear(p)                             p->Clear()
#define IDirectSoundSequencerStream_ClearAtOrAfter(p, a)                 p->ClearAtOrAfter(p, a)
#define IDirectSoundSequencerStream_GetSequenceNumber(p, a)              p->GetSequenceNumber(a)
#define IDirectSoundSequencerStream_GetStatus(p, a)                      p->GetStatus(a)
#define IDirectSoundSequencerStream_GetPacketStatus(p, a, b)             p->GetStatus(a, b)
#define IDirectSoundSequencerStream_GetError(p, a)                       p->GetError(a)
#define IDirectSoundSequencerStream_GetErrorSequenceNumber(p, a)         p->GetErrorSequenceNumber(a)
#define IDirectSoundSequencerStream_GetErrorSampleTime(p, a)             p->GetErrorSampleTime(a)
#define IDirectSoundSequencerStream_ClearError(p)                        p->ClearError()
#define IDirectSoundSequencerStream_Process(p, a, b, c)                  p->Process(a, b, c)
#define IDirectSoundSequencerStream_Discontinuity(p)                     p->Discontinuity()
#define IDirectSoundSequencerStream_Flush(p)                             p->Flush()
#define IDirectSoundSequencerStream_SetFrequency(p, a, b)                p->SetFrequency(a, b)
#define IDirectSoundSequencerStream_SetPitch(p, a, b)                    p->SetPitch(a, b)
#define IDirectSoundSequencerStream_SetVolume(p, a, b)                   p->SetVolume(a, b)
#define IDirectSoundSequencerStream_SetLFO(p, a, b)                      p->SetLFO(a, b)
#define IDirectSoundSequencerStream_SetEG(p, a, b)                       p->SetEG(a, b)
#define IDirectSoundSequencerStream_SetFilter(p, a, b)                   p->SetFilter(a, b)
#define IDirectSoundSequencerStream_SetOutputBuffer(p, a, b)             p->SetOutputBuffer(a, b)
#define IDirectSoundSequencerStream_SetMixBins(p, a, b)                  p->SetMixBins(a, b)
#define IDirectSoundSequencerStream_SetMixBinVolumes(p, a, b, c)         p->SetMixBinVolumes(a, b, c)
#define IDirectSoundSequencerStream_Pause(p, a)                          p->Pause(a)
#define IDirectSoundSequencerStream_SetAllParameters(p, a, b, c)         p->SetAllParameters(a, b, c)
#define IDirectSoundSequencerStream_SetConeAngles(p, a, b, c, d)         p->SetConeAngles(a, b, c, d)
#define IDirectSoundSequencerStream_SetConeOrientation(p, a, b, c, d, e) p->SetConeOrientation(a, b, c, d, e)
#define IDirectSoundSequencerStream_SetConeOutsideVolume(p, a, b, c)     p->SetConeOutsideVolume(a, b, c)
#define IDirectSoundSequencerStream_SetMaxDistance(p, a, b, c)           p->SetMaxDistance(a, b, c)
#define IDirectSoundSequencerStream_SetMinDistance(p, a, b, c)           p->SetMinDistance(a, b, c)
#define IDirectSoundSequencerStream_SetMode(p, a, b, c)                  p->SetMode(a, b, c)
#define IDirectSoundSequencerStream_SetPosition(p, a, b, c, d, e)        p->SetPosition(a, b, c, d, e)
#define IDirectSoundSequencerStream_SetVelocity(p, a, b, c, d, e)        p->SetVelocity(a, b, c, d, e)
#define IDirectSoundSequencerStream_SetNotificationPositions(p, a, b, c) p->SetNotificationPositions(a, b, c)

#else // defined(__cplusplus) && !defined(CINTERFACE)

#define IDirectSoundSequencerStream_Clear(p)                             p->lpVtbl->Clear()
#define IDirectSoundSequencerStream_ClearAtOrAfter(p)                    p->lpVtbl->ClearAtOrAfter(p, a)
#define IDirectSoundSequencerStream_GetSequenceNumber(p, a)              p->lpVtbl->GetSequenceNumber(a)
#define IDirectSoundSequencerStream_GetPacketStatus(p, a, b)             p->lpVtbl->GetStatus(a, b)
#define IDirectSoundSequencerStream_GetError(p, a)                       p->lpVtbl->GetError(a)
#define IDirectSoundSequencerStream_GetErrorSequenceNumber(p, a)         p->lpVtbl->GetErrorSequenceNumber(a)
#define IDirectSoundSequencerStream_GetErrorSampleTime(p, a)             p->lpVtbl->GetErrorSampleTime(a)
#define IDirectSoundSequencerStream_ClearError(p)                        p->lpVtbl->ClearError()
#define IDirectSoundSequencerStream_Process(p, a, b, c)                  p->lpVtbl->Process(a, b, c)
#define IDirectSoundSequencerStream_Discontinuity(p)                     p->lpVtbl->Discontinuity()
#define IDirectSoundSequencerStream_Flush(p)                             p->lpVtbl->Flush()
#define IDirectSoundSequencerStream_SetFrequency(p, a, b)                p->lpVtbl->SetFrequency(a, b)
#define IDirectSoundSequencerStream_SetPitch(p, a, b)                    p->lpVtbl->SetPitch(a, b)
#define IDirectSoundSequencerStream_SetVolume(p, a, b)                   p->lpVtbl->SetVolume(a, b)
#define IDirectSoundSequencerStream_SetLFO1(p, a, b)                     p->lpVtbl->SetLFO(a, b)
#define IDirectSoundSequencerStream_SetEG(p, a, b)                       p->lpVtbl->SetEG(a, b)
#define IDirectSoundSequencerStream_SetFilter(p, a, b)                   p->lpVtbl->SetFilter(a, b)
#define IDirectSoundSequencerStream_SetOutputBuffer(p, a, b)             p->lpVtbl->SetOutputBuffer(a, b)
#define IDirectSoundSequencerStream_SetMixBins(p, a, b)                  p->lpVtbl->SetMixBins(a, b)
#define IDirectSoundSequencerStream_SetMixBinVolumes(p, a, b, c)         p->lpVtbl->SetMixBinVolumes(a, b, c)
#define IDirectSoundSequencerStream_Pause(p, a)                          p->lpVtbl->Pause(a)
#define IDirectSoundSequencerStream_SetAllParameters(p, a, b, c)         p->lpVtbl->SetAllParameters(a, b, c)
#define IDirectSoundSequencerStream_SetConeAngles(p, a, b, c, d)         p->lpVtbl->SetConeAngles(a, b, c, d)
#define IDirectSoundSequencerStream_SetConeOrientation(p, a, b, c, d, e) p->lpVtbl->SetConeOrientation(a, b, c, d, e)
#define IDirectSoundSequencerStream_SetConeOutsideVolume(p, a, b, c)     p->lpVtbl->SetConeOutsideVolume(a, b, c)
#define IDirectSoundSequencerStream_SetMaxDistance(p, a, b, c)           p->lpVtbl->SetMaxDistance(a, b, c)
#define IDirectSoundSequencerStream_SetMinDistance(p, a, b, c)           p->lpVtbl->SetMinDistance(a, b, c)
#define IDirectSoundSequencerStream_SetMode(p, a, b, c)                  p->lpVtbl->SetMode(a, b, c)
#define IDirectSoundSequencerStream_SetPosition(p, a, b, c, d, e)        p->lpVtbl->SetPosition(a, b, c, d, e)
#define IDirectSoundSequencerStream_SetVelocity(p, a, b, c, d, e         p->lpVtbl->SetVelocity(a, b, c, d, e)
#define IDirectSoundSequencerStream_SetNotificationPositions(p, a, b, c) p->lpVtbl->SetNotificationPositions(a, b, c)

#endif // defined(__cplusplus) && !defined(CINTERFACE)

#endif // __DSOUNDSEQUENCER_INCLUDED__