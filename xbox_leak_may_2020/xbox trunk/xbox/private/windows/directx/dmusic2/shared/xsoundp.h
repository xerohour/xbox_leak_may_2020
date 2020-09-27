//
// xsoundp.h
//
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//
// Private interfaces

#ifndef _XSOUNDP_DOT_H_
#define _XSOUNDP_DOT_H_

#ifdef XBOX

#include <xtl.h>
#include <dmusicc.h>

// Temporary defines until the "8"s go back

#define IDirectSound8 IDirectSound
#define LPDIRECTSOUND8 LPDIRECTSOUND
#define IDirectSoundBuffer8 IDirectSoundBuffer
#define DirectSoundCreate8 DirectSoundCreate

// This probably needs to go back into dsound.h


// Dummy reverberation GUIDs, until DSound gets its act together.

// I3DL2 Environmental Reverberation: Source Effect {EFBA364A-E606-451C-8E97-07D508119C65}
DEFINE_GUID(GUID_DSFX_STANDARD_I3DL2SOURCE, 0xefba364a, 0xe606, 0x451c, 0x8e, 0x97, 0x07, 0xd5, 0x08, 0x11, 0x9c, 0x65);

// I3DL2 Environmental Reverberation: Reverb (Listener) Effect {EF985E71-D5C7-42D4-BA4D-2D073E2E96F4}
DEFINE_GUID(GUID_DSFX_STANDARD_I3DL2REVERB, 0xef985e71, 0xd5c7, 0x42d4, 0xba, 0x4d, 0x2d, 0x07, 0x3e, 0x2e, 0x96, 0xf4);

// Waves Reverberation {87FC0268-9A55-4360-95AA-004A1D9DE26C}
DEFINE_GUID(GUID_DSFX_WAVES_REVERB, 0x87fc0268, 0x9a55, 0x4360, 0x95, 0xaa, 0x00, 0x4a, 0x1d, 0x9d, 0xe2, 0x6c);


// Private classes that used to be in dsound.x, but which are only used by DirectMusic.

// Forward declarations

struct IReferenceClock;

//
// Private DirectSound interfaces used by DMusic, DPlayVoice, etc.
// Eventually much of the MSINTERNAL stuff above should migrate here.
//

#ifdef __cplusplus
struct IDirectSoundPrivate;
struct IDirectSoundSynthSink;
struct IDirectSoundConnect;
struct IDirectSoundSinkSync;
struct IDirectSoundSource;
struct IDirectSoundWave;
class CBuffer;
#endif // __cplusplus

#define IDirectSoundPrivate8          IDirectSoundPrivate
#define IDirectSoundSynthSink8        IDirectSoundSynthSink
#define IDirectSoundConnect8          IDirectSoundConnect
#define IDirectSoundSinkSync8         IDirectSoundSinkSync
#define IDirectSoundSource8           IDirectSoundSource
#define IDirectSoundWave8             IDirectSoundWave

#define IID_IDirectSoundPrivate8      IID_IDirectSoundPrivate
#define IID_IDirectSoundSynthSink8    IID_IDirectSoundSynthSink
#define IID_IDirectSoundConnect8      IID_IDirectSoundConnect
#define IID_IDirectSoundSinkSync8     IID_IDirectSoundSinkSync
#define IID_IDirectSoundSource8       IID_IDirectSoundSource
#define IID_IDirectSoundWave8         IID_IDirectSoundWave

typedef struct IDirectSoundPrivate    *LPDIRECTSOUNDPRIVATE;
typedef struct IDirectSoundSynthSink  *LPDIRECTSOUNDSYNTHSINK;
typedef struct IDirectSoundConnect    *LPDIRECTSOUNDCONNECT;
typedef struct IDirectSoundSinkSync   *LPDIRECTSOUNDSINKSYNC;
typedef struct IDirectSoundSource     *LPDIRECTSOUNDSOURCE;
typedef struct IDirectSoundWave       *LPDIRECTSOUNDWAVE;

typedef struct IDirectSoundPrivate8   *LPDIRECTSOUNDPRIVATE8;
typedef struct IDirectSoundSynthSink8 *LPDIRECTSOUNDSYNTHSINK8;
typedef struct IDirectSoundConnect8   *LPDIRECTSOUNDCONNECT8;
typedef struct IDirectSoundSinkSync8  *LPDIRECTSOUNDSINKSYNC8;
typedef struct IDirectSoundSource8    *LPDIRECTSOUNDSOURCE8;
typedef struct IDirectSoundWave8      *LPDIRECTSOUNDWAVE8;

//
// IDirectSoundPrivate: used by DirectMusic to create DirectSoundSink objects
//

DEFINE_GUID(IID_IDirectSoundPrivate, 0xd6e525ae, 0xb125, 0x4ec4, 0xbe, 0x13, 0x12, 0x6d, 0x0c, 0xf7, 0xaf, 0xb6);

// There is no IID for dsound buffer, so make one for now...

DEFINE_GUID(IID_IDirectSoundBuffer,0x1b13ff5e, 0x981b, 0x4ca9, 0x92, 0x6a, 0xcd, 0x7b, 0xb6, 0x68, 0xc8, 0x5d);


#undef INTERFACE
#define INTERFACE IDirectSoundPrivate

DECLARE_INTERFACE_(IDirectSoundPrivate, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    // IDirectSoundPrivate methods
    STDMETHOD(AllocSink)            (THIS_ LPWAVEFORMATEX pwfxFormat, LPDIRECTSOUNDCONNECT *ppSinkConnect) PURE;
};

#define IDirectSoundPrivate_QueryInterface(p,a,b)       IUnknown_QueryInterface(p,a,b)
#define IDirectSoundPrivate_AddRef(p)                   IUnknown_AddRef(p)
#define IDirectSoundPrivate_Release(p)                  IUnknown_Release(p)

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundPrivate_AllocSink(p,a,b)            (p)->lpVtbl->AllocSink(p,a,b)
#else // !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundPrivate_AllocSink(p,a,b)            (p)->AllocSink(a,b)
#endif // !defined(__cplusplus) || defined(CINTERFACE)

//
// IDirectSoundSynthSink: used by the DirectMusic synthesiser
//

DEFINE_GUID(IID_IDirectSoundSynthSink, 0x73a6a85a, 0x493e, 0x4c87, 0xb4, 0xa5, 0xbe, 0x53, 0xeb, 0x92, 0x74, 0x4b);

#undef INTERFACE
#define INTERFACE IDirectSoundSynthSink

DECLARE_INTERFACE_(IDirectSoundSynthSink, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    // IDirectSoundSynthSink methods
    STDMETHOD(GetLatencyClock)      (THIS_ IReferenceClock **ppClock) PURE;
    STDMETHOD(Activate)             (THIS_ BOOL fEnable) PURE;
    STDMETHOD(SampleToRefTime)      (THIS_ LONGLONG llSampleTime, REFERENCE_TIME *prtTime) PURE;
    STDMETHOD(RefToSampleTime)      (THIS_ REFERENCE_TIME rtTime, LONGLONG *pllSampleTime) PURE;
    STDMETHOD(GetFormat)            (THIS_ LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten) PURE;
};

#define IDirectSoundSynthSink_QueryInterface(p,a,b)     IUnknown_QueryInterface(p,a,b)
#define IDirectSoundSynthSink_AddRef(p)                 IUnknown_AddRef(p)
#define IDirectSoundSynthSink_Release(p)                IUnknown_Release(p)

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundSynthSink_GetLatencyClock(p,a)      (p)->lpVtbl->GetLatencyClock(p,a)
#define IDirectSoundSynthSink_Activate(p,a)             (p)->lpVtbl->Activate(p,a)
#define IDirectSoundSynthSink_SampleToRefTime(p,a,b)    (p)->lpVtbl->SampleToRefTime(p,a,b)
#define IDirectSoundSynthSink_RefToSampleTime(p,a,b)    (p)->lpVtbl->RefToSampleTime(p,a,b)
#define IDirectSoundSynthSink_GetFormat(p,a,b)          (p)->lpVtbl->GetFormat(p,a,b)
#else // !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundSynthSink_GetLatencyClock(p,a)      (p)->GetLatencyClock(a)
#define IDirectSoundSynthSink_Activate(p,a)             (p)->Activate(a)
#define IDirectSoundSynthSink_SampleToRefTime(p,a,b)    (p)->SampleToRefTime(a,b)
#define IDirectSoundSynthSink_RefToSampleTime(p,a,b)    (p)->RefToSampleTime(a,b)
#define IDirectSoundSynthSink_GetFormat(p,a,b)          (p)->GetFormat(a,b)
#endif // !defined(__cplusplus) || defined(CINTERFACE)

//
// IDirectSoundConnect: used by DirectMusic to implement
// audio paths and connect the synthesiser to the sink
//

DEFINE_GUID(IID_IDirectSoundConnect, 0x5bee1fe0, 0x60d5, 0x4ef9, 0x88, 0xbc, 0x33, 0x67, 0xb9, 0x75, 0xc6, 0x11);

#undef INTERFACE
#define INTERFACE IDirectSoundConnect

DECLARE_INTERFACE_(IDirectSoundConnect, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    // IDirectSoundConnect methods
    STDMETHOD(AddSource)            (THIS_ LPDIRECTSOUNDSOURCE pDSSource) PURE;
    STDMETHOD(RemoveSource)         (THIS_ LPDIRECTSOUNDSOURCE pDSSource) PURE;
    STDMETHOD(SetMasterClock)       (THIS_ IReferenceClock *pClock) PURE;
    STDMETHOD(CreateSoundBuffer)    (THIS_ LPCDSBUFFERDESC pcDSBufferDesc, LPDWORD pdwFuncID, DWORD dwBusIDCount,
                                           REFGUID guidBufferID, LPDIRECTSOUNDBUFFER *ppDSBuffer) PURE;
    STDMETHOD(CreateSoundBufferFromConfig) (THIS_ LPUNKNOWN pConfig, LPDIRECTSOUNDBUFFER *ppDSBuffer) PURE;
    STDMETHOD(GetSoundBuffer)       (THIS_ DWORD dwBusID, LPDIRECTSOUNDBUFFER *ppDSBuffer) PURE;
    STDMETHOD(GetBusCount)          (THIS_ LPDWORD pdwCount) PURE;
    STDMETHOD(GetBusIDs)            (THIS_ LPDWORD pdwBusIDs, LPDWORD pdwFuncIDs, DWORD dwBusCount) PURE;
    STDMETHOD(GetFunctionalID)      (THIS_ DWORD dwBusID, LPDWORD pdwFuncID) PURE;
    STDMETHOD(GetSoundBufferBusIDs) (THIS_ LPDIRECTSOUNDBUFFER pDSBuffer, LPDWORD pdwBusIDs, LPDWORD pdwFuncIDs, LPDWORD pdwBusCount) PURE;
};

#define IDirectSoundConnect_QueryInterface(p,a,b)               IUnknown_QueryInterface(p,a,b)
#define IDirectSoundConnect_AddRef(p)                           IUnknown_AddRef(p)
#define IDirectSoundConnect_Release(p)                          IUnknown_Release(p)

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundConnect_AddSource(p,a)                      (p)->lpVtbl->AddSource(p,a)
#define IDirectSoundConnect_RemoveSource(p,a)                   (p)->lpVtbl->RemoveSource(p,a)
#define IDirectSoundConnect_SetMasterClock(p,a)                 (p)->lpVtbl->SetMasterClock(p,a)
#define IDirectSoundConnect_CreateSoundBuffer(p,a,b,c,d)        (p)->lpVtbl->CreateSoundBuffer(p,a,b,c,d)
#define IDirectSoundConnect_CreateSoundBufferFromConfig(p,a,b)  (p)->lpVtbl->CreateSoundBufferFromConfig(p,a,b)
#define IDirectSoundConnect_GetSoundBuffer(p,a,b)               (p)->lpVtbl->GetSoundBuffer(p,a,b)
#define IDirectSoundConnect_GetBusCount(p,a)                    (p)->lpVtbl->GetBusCount(p,a)
#define IDirectSoundConnect_GetBusIDs(p,a,b,c)                  (p)->lpVtbl->GetBusIDs(p,a,b,c)
#define IDirectSoundConnect_GetFunctionalID(p,a,b)              (p)->lpVtbl->GetFunctionalID(p,a,b)
#define IDirectSoundConnect_GetSoundBufferBusIDs(p,a,b,c,d)     (p)->lpVtbl->GetSoundBufferBusIDs(p,a,b,c,d)
#else // !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundConnect_AddSource(p,a)                      (p)->AddSource(a)
#define IDirectSoundConnect_RemoveSource(p,a)                   (p)->RemoveSource(a)
#define IDirectSoundConnect_SetMasterClock(p,a)                 (p)->SetMasterClock(a)
#define IDirectSoundConnect_CreateSoundBuffer(p,a,b,c,d)        (p)->CreateSoundBuffer(a,b,c,d)
#define IDirectSoundConnect_CreateSoundBufferFromConfig(p,a,b)  (p)->CreateSoundBufferFromConfig(a,b)
#define IDirectSoundConnect_GetSoundBuffer(p,a,b)               (p)->GetSoundBuffer(a,b)
#define IDirectSoundConnect_GetBusCount(p,a)                    (p)->GetBusCount(a)
#define IDirectSoundConnect_GetBusIDs(p,a,b,c)                  (p)->GetBusIDs(a,b,c)
#define IDirectSoundConnect_GetFunctionalID(p,a,b)              (p)->GetFunctionalID(a,b)
#define IDirectSoundConnect_GetSoundBufferBusIDs(p,a,b,c,d)     (p)->GetSoundBufferBusIDs(a,b,c,d)
#endif // !defined(__cplusplus) || defined(CINTERFACE)

//
// IDirectSoundSinkSync: provided by the DirectMusic master clock
// to allow DirectSound to synchronize with it
//

DEFINE_GUID(IID_IDirectSoundSinkSync, 0xd28de0d0, 0x2794, 0x492f, 0xa3, 0xff, 0xe2, 0x41, 0x80, 0xd5, 0x43, 0x79);

#undef INTERFACE
#define INTERFACE IDirectSoundSinkSync

DECLARE_INTERFACE_(IDirectSoundSinkSync, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    // IDirectSoundSinkSync methods
    STDMETHOD(SetClockOffset)       (THIS_ LONGLONG llOffset) PURE;
};

#define IDirectSoundSinkSync_QueryInterface(p,a,b)  IUnknown_QueryInterface(p,a,b)
#define IDirectSoundSinkSync_AddRef(p)              IUnknown_AddRef(p)
#define IDirectSoundSinkSync_Release(p)             IUnknown_Release(p)

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundSinkSync_SetClockOffset(p,a)    (p)->lpVtbl->SetClockOffset(p,a)
#else // !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundSinkSync_SetClockOffset(p,a)    (p)->SetClockOffset(a)
#endif // !defined(__cplusplus) || defined(CINTERFACE)

//
// IDirectSoundSource (currently private)
//

DEFINE_GUID(IID_IDirectSoundSource, 0x536f7af3, 0xdb03, 0x4888, 0x93, 0x66, 0x54, 0x48, 0xb1, 0x1d, 0x4a, 0x19);

#undef INTERFACE
#define INTERFACE IDirectSoundSource

DECLARE_INTERFACE_(IDirectSoundSource, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    // IDirectSoundSource methods
    STDMETHOD(SetSink)              (THIS_ LPDIRECTSOUNDCONNECT pSinkConnect) PURE;
    STDMETHOD(GetFormat)            (THIS_ LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten) PURE;
    STDMETHOD(Seek)                 (THIS_ ULONGLONG ullPosition) PURE;
    STDMETHOD(Read)                 (THIS_ LPVOID *ppvBusBuffers, LPDWORD pdwIDs, LPDWORD pdwFuncIDs, LPLONG plPitchAdjust, DWORD dwBufferCount, ULONGLONG *ullLength) PURE;
    STDMETHOD(GetSize)              (THIS_ ULONGLONG *pullStreamSize) PURE;
};

#define IDirectSoundSource_QueryInterface(p,a,b)    IUnknown_QueryInterface(p,a,b)
#define IDirectSoundSource_AddRef(p)                IUnknown_AddRef(p)
#define IDirectSoundSource_Release(p)               IUnknown_Release(p)

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundSource_SetSink(p,a)             (p)->lpVtbl->SetSink(p,a)
#define IDirectSoundSource_GetFormat(p,a,b)         (p)->lpVtbl->GetFormat(p,a,b)
#define IDirectSoundSource_Seek(p,a)                (p)->lpVtbl->Seek(p,a)
#define IDirectSoundSource_Read(p,a,b,c,d,e)        (p)->lpVtbl->Read(p,a,b,c,d,e)
#define IDirectSoundSource_GetSize(p,a)             (p)->lpVtbl->GetSize(p,a)
#else // !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundSource_SetSink(p,a)             (p)->SetSink(a)
#define IDirectSoundSource_GetFormat(p,a,b)         (p)->GetFormat(a,b)
#define IDirectSoundSource_Seek(p,a)                (p)->Seek(a)
#define IDirectSoundSource_Read(p,a,b,c,d,e)        (p)->Read(a,b,c,d,e)
#define IDirectSoundSource_GetSize(p,a)             (p)->GetSize(a)
#endif // !defined(__cplusplus) || defined(CINTERFACE)

//
// IDirectSoundWave (currently private)
//

DEFINE_GUID(IID_IDirectSoundWave, 0x69e934e4, 0x97f1, 0x4f1d, 0x88, 0xe8, 0xf2, 0xac, 0x88, 0x67, 0x13, 0x27);

#define DSWCS_F_DEINTERLEAVED   0x00000001
#define DSWCS_F_TOGGLE_8BIT     0x00000002

#undef INTERFACE
#define INTERFACE IDirectSoundWave

DECLARE_INTERFACE_(IDirectSoundWave, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)       (THIS_ REFIID, LPVOID *) PURE;
    STDMETHOD_(ULONG,AddRef)        (THIS) PURE;
    STDMETHOD_(ULONG,Release)       (THIS) PURE;

    // IDirectSoundWave methods
    STDMETHOD(CreateSource)         (THIS_ LPDIRECTSOUNDSOURCE *ppDSSource, LPWAVEFORMATEX pwfxFormat, DWORD dwFlags) PURE;
    STDMETHOD(GetFormat)            (THIS_ LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten) PURE;
    STDMETHOD(GetStreamingParms)    (THIS_ LPDWORD pdwFlags, REFERENCE_TIME *prtReadAhead) PURE;
};

#define IDirectSoundWave_QueryInterface(p,a,b)      IUnknown_QueryInterface(p,a,b)
#define IDirectSoundWave_AddRef(p)                  IUnknown_AddRef(p)
#define IDirectSoundWave_Release(p)                 IUnknown_Release(p)

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundWave_CreateSource(p,a,b,c)      (p)->lpVtbl->CreateSource(p,a,b,c)
#define IDirectSoundWave_GetFormat(p,a,b)           (p)->lpVtbl->GetFormat(p,a,b)
#define IDirectSoundWave_GetStreamingParms(p,a,b)   (p)->lpVtbl->GetStreamingParms(p,a,b)
#else // !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectSoundWave_CreateSource(p,a,b,c)      (p)->CreateSource(a,b,c)
#define IDirectSoundWave_GetFormat(p,a,b)           (p)->GetFormat(a,b)
#define IDirectSoundWave_GetStreamingParms(p,a,b)   (p)->GetStreamingParms(a,b)
#endif // !defined(__cplusplus) || defined(CINTERFACE)

#endif // XBOX

#endif // _XSOUNDP_DOT_H_