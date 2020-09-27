/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsapi.h
 *  Content:    DirectSound API objects and entry points.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/06/00    dereks  Created.
 *
 ****************************************************************************/

#ifndef __DSAPI_H__
#define __DSAPI_H__

// #define USE_KEEPALIVE_BUFFERS 1
#define USE_KEEPALIVE_BUFFERS 4

// #define DSAPI_DISABLE_SUBMIX
// #define DSAPI_DISABLE_EFFECTS
// #define DSAPI_DISABLE_LISTENER
// #define DSAPI_DISABLE_3DSOURCE
// #define DSAPI_DISABLE_MIXBINS
// #define DSAPI_DISABLE_VOLUME
// #define DSAPI_DISABLE_PITCH
// #define DSAPI_DISABLE_FILTER
// #define DSAPI_DISABLE_LFO
// #define DSAPI_DISABLE_EG
// #define DSAPI_DISABLE_NOTEOFF

#ifdef DEBUG
#define DS3D_ALLOW_ALGORITHM_SWAP
#endif // DEBUG

//
// Global critical section
//

__inline BOOL DirectSoundEnterCriticalSection(void)
{
    if(PASSIVE_LEVEL != KeGetCurrentIrql())
    {
        return FALSE;
    }

    EnterCriticalSection(&g_DirectSoundCriticalSection); 

    return TRUE;
}

__inline void DirectSoundLeaveCriticalSection(void)
{
    LeaveCriticalSection(&g_DirectSoundCriticalSection);
}

#ifdef __cplusplus

//
// Forward declarations
//

namespace DirectSound
{
    class CMcpxAPU;
    class CMcpxVoiceClient;
    class CMcpxBuffer;
    class CMcpxStream;
}

//
// Validation object and corresponding macros
//

#ifdef VALIDATE_PARAMETERS

namespace DirectSound
{
    template <DWORD dwSignature> class CValidObject
    {
    private:
        const DWORD             m_dwSignature;      // Object signature

    public:
        CValidObject(void);
        virtual ~CValidObject(void);

    public:
        void __AssertValidObject(LPCSTR pszFile, DWORD dwLine, LPCSTR pszFunction);
    };

    template <DWORD dwSignature> CValidObject<dwSignature>::CValidObject(void)
        : m_dwSignature(dwSignature)
    {
    }

    template <DWORD dwSignature> CValidObject<dwSignature>::~CValidObject(void)
    {
    }

    template <DWORD dwSignature> void CValidObject<dwSignature>::__AssertValidObject(LPCSTR pszFile, DWORD dwLine, LPCSTR pszFunction)
    {
        if(!this)
        {
            DirectSound::CDebug::SetContext(DPF_FLAGS_DEFAULT, DPFLVL_ERROR, pszFile, dwLine, pszFunction, DPF_LIBRARY);
            DirectSound::CDebug::Print("NULL \"this\" pointer (%x)", this);
        }
        else if(m_dwSignature != dwSignature)
        {
            DirectSound::CDebug::SetContext(DPF_FLAGS_DEFAULT, DPFLVL_ERROR, pszFile, dwLine, pszFunction, DPF_LIBRARY);
            DirectSound::CDebug::Print("Invalid \"this\" pointer (%x != %x)", m_dwSignature, dwSignature);
        }
    }
}

#define _AssertValidObject(fname) \
    __AssertValidObject(__FILE__, __LINE__, fname)

#define AssertValidObject() \
    _AssertValidObject(DPF_FNAME)

#else // VALIDATE_PARAMETERS

#define _AssertValidObject(fname)
#define AssertValidObject()

#endif // VALIDATE_PARAMETERS

//
// Automatic (functon-scope) locking mechanism
//

namespace DirectSound
{
    class CAutoLock
    {
    private:
        BOOL                    m_fLocked;

    public:
        CAutoLock(void);
        ~CAutoLock(void);
    };

    __inline CAutoLock::CAutoLock(void)
    {
        m_fLocked = (BOOLEAN)DirectSoundEnterCriticalSection();
    }

    __inline CAutoLock::~CAutoLock(void)
    {
        if(m_fLocked)
        {
            DirectSoundLeaveCriticalSection();
        }
    }
}

#define AutoLock() \
    DirectSound::CAutoLock __AutoLock

//
// API helper macros
//

#define CHECK_FINAL_RELEASE_HRESULT() \
    if(g_fDirectSoundInFinalRelease) \
    { \
        DPF_WARNING("DirectSound is in the final stages of shutting down.  No more API calls are allowed."); \
        return DSERR_GENERIC; \
    }

#define CHECK_FINAL_RELEASE_VOID() \
    if(g_fDirectSoundInFinalRelease) \
    { \
        DPF_WARNING("DirectSound is in the final stages of shutting down.  No more API calls are allowed."); \
        return; \
    }

#define ENTER_EXTERNAL_FUNCTION() \
    AutoLock()

#define _ENTER_EXTERNAL_METHOD(name) \
    AutoLock(); \
    CHECK_FINAL_RELEASE_HRESULT(); \
    _AssertValidObject(name)

#define _ENTER_EXTERNAL_METHOD_VOID(name) \
    AutoLock(); \
    CHECK_FINAL_RELEASE_VOID(); \
    _AssertValidObject(name)

#define ENTER_EXTERNAL_METHOD() \
    _ENTER_EXTERNAL_METHOD(DPF_FNAME)

#define ENTER_EXTERNAL_METHOD_VOID() \
    _ENTER_EXTERNAL_METHOD_VOID(DPF_FNAME)

//
// DirectSound settings
//

namespace DirectSound
{
    class CDirectSoundSettings
        : public CRefCount
    {
    public:
        DWORD               m_dwSpeakerConfig;                      // Speaker configuration
        DSEFFECTIMAGELOC    m_EffectLocations;                      // Effect locations
        BYTE                m_abMixBinHeadroom[DSMIXBIN_COUNT];     // MixBin headroom

    #ifndef MCPX_BOOT_LIB                                           
                                                                
        DS3DLISTENERPARAMS  m_3dParams;                             // 3D parameters

    #endif // MCPX_BOOT_LIB

    public:
        CDirectSoundSettings(void);
        virtual ~CDirectSoundSettings(void);

    public:
        void SetEffectImageLocations(LPCDSEFFECTIMAGELOC pImageLoc);
    };
}

//
// DirectSound implementation
//

namespace DirectSound
{

#ifdef VALIDATE_PARAMETERS

    class CDirectSound
        : public IDirectSound, public CValidObject<'DSND'>, public CRefCount

#else // VALIDATE_PARAMETERS

    class CDirectSound
        : public IDirectSound, public CRefCount

#endif // VALIDATE_PARAMETERS

    {
        friend class CDirectSoundVoice;
        friend class CDirectSoundBuffer;
        friend class CDirectSoundStream;
        friend class CPerfMon;

    public:
        static CDirectSound *       m_pDirectSound;                                 // The one-and-only DirectSound object
                                                                                
    #ifdef DEBUG                                                                    
                                                                                
        static LPVOID               m_pvEncoderImageData;                           // Encoder image data
        static DWORD                m_dwEncoderImageSize;                           // Encoder image size, in bytes
                                                                                
    #endif // DEBUG                                                                 
                                                                                
    protected:                                                                      
        CDirectSoundSettings *      m_pSettings;                                    // Shared settings object
        CMcpxAPU *                  m_pDevice;                                      // Device implementation
        LIST_ENTRY                  m_lst3dVoices;                                  // 3D voice list

    #ifdef USE_KEEPALIVE_BUFFERS

        LPDIRECTSOUNDBUFFER         m_apKeepAliveBuffers[USE_KEEPALIVE_BUFFERS];    // Dummy buffers used to keep the mixbins working

    #endif // USE_KEEPALIVE_BUFFERS

    public:
        CDirectSound(void);
        virtual ~CDirectSound(void);

    public:
        // Initialization
        HRESULT STDMETHODCALLTYPE Initialize(void);

        // IUnknown methods
        ULONG STDMETHODCALLTYPE AddRef(void);
        ULONG STDMETHODCALLTYPE Release(void);

        // IDirectSound methods
        HRESULT STDMETHODCALLTYPE GetCaps(LPDSCAPS pdsc);
        HRESULT STDMETHODCALLTYPE CreateSoundBuffer(LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDBUFFER *ppBuffer, LPUNKNOWN pUnkOuter);
        HRESULT STDMETHODCALLTYPE CreateSoundStream(LPCDSSTREAMDESC pdssd, LPDIRECTSOUNDSTREAM *ppStream, LPUNKNOWN pUnkOuter);
        HRESULT STDMETHODCALLTYPE GetSpeakerConfig(LPDWORD pdwSpeakerConfig);
        HRESULT STDMETHODCALLTYPE DownloadEffectsImage(LPCVOID pvImageBuffer, DWORD dwImageSize, LPCDSEFFECTIMAGELOC pImageLoc, LPDSEFFECTIMAGEDESC *ppImageDesc);
        HRESULT STDMETHODCALLTYPE GetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPVOID pvData, DWORD dwDataSize);
        HRESULT STDMETHODCALLTYPE SetEffectData(DWORD dwEffectIndex, DWORD dwOffset, LPCVOID pvData, DWORD dwDataSize, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE CommitEffectData(void);
        HRESULT STDMETHODCALLTYPE EnableHeadphones(BOOL fEnable);
        HRESULT STDMETHODCALLTYPE SetMixBinHeadroom(DWORD dwMixBin, DWORD dwHeadroom);
        HRESULT STDMETHODCALLTYPE GetOutputLevels(DSOUTPUTLEVELS *pOutputLevels, BOOL bReset);

        // IDirectSound3dListener methods
        HRESULT STDMETHODCALLTYPE SetAllParameters(LPCDS3DLISTENER pds3dl, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetDistanceFactor(FLOAT flDistanceFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetDopplerFactor(FLOAT flDopplerFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetOrientation(FLOAT xFront, FLOAT yFront, FLOAT zFront, FLOAT xTop, FLOAT yTop, FLOAT zTop, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetRolloffFactor(FLOAT flRolloffFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetI3DL2Listener(LPCDSI3DL2LISTENER pProperties, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE CommitDeferredSettings(void);

        // IReferenceClock methods
        HRESULT STDMETHODCALLTYPE GetTime(REFERENCE_TIME *prtCurrent);


        // Work, work, work
        void STDMETHODCALLTYPE DoWork(void);

        // 3D
        void STDMETHODCALLTYPE Force3dRecalc(DWORD dwParameterMask = -1);
    };

    __inline ULONG CDirectSound::AddRef(void)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSound::AddRef");
        return CRefCount::AddRef();
    }

#ifndef USE_KEEPALIVE_BUFFERS

    __inline ULONG CDirectSound::Release(void)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSound::Release");
        return CRefCount::Release();
    }

#endif // USE_KEEPALIVE_BUFFERS

#ifndef MCPX_BOOT_LIB

    __inline void CDirectSound::Force3dRecalc(DWORD dwParameterMask)
    {
        m_pSettings->m_3dParams.dwParameterMask |= dwParameterMask;

        CommitDeferredSettings();
    }

#endif // MCPX_BOOT_LIB

}

//
// DirectSound Voice settings
//

namespace DirectSound
{
    class CDirectSoundVoiceSettings
        : public CRefCount
    {
    public:
        DWORD                       m_dwFlags;                          // Voice creation flags
        DSWAVEFORMAT                m_fmt;                              // Voice format
        LONG                        m_lPitch;                           // Voice pitch
        LONG                        m_lVolume;                          // Voice volume
        DWORD                       m_dwHeadroom;                       // Voice headroom
        DWORD                       m_dwMixBinCount;                    // MixBin count
        BYTE                        m_abMixBins[8];                     // MixBin assignments
        LONG                        m_alMixBinVolumes[DSMIXBIN_COUNT];  // MixBin volumes
        CDirectSoundBuffer *        m_pMixinBuffer;                     // Destination buffer for a submix operation
                                                                    
#ifndef MCPX_BOOT_LIB                                               
                                                                    
        LPDS3DSOURCEPARAMS          m_p3dParams;                        // 3D parameters

#endif // MCPX_BOOT_LIB

    public:
        CDirectSoundVoiceSettings(void);
        virtual ~CDirectSoundVoiceSettings(void);

    public:
        // Initialization
        HRESULT Initialize(DWORD dwFlags, LPCWAVEFORMATEX pwfxFormat, LPCDSMIXBINS pMixBins);

        // Format
        BOOL SetFormat(LPCWAVEFORMATEX pwfxFormat, BOOL fAllowMixBins);
    
        // Volume
        void SetVolume(LONG lVolume);
        void SetMixBinVolumes(LPCDSMIXBINS pMixBins);
        void SetHeadroom(DWORD dwHeadroom);

        // Mixbins
        void SetMixBins(LPCDSMIXBINS pMixBins);

        // Submixing
        void SetOutputBuffer(CDirectSoundBuffer *pMixinBuffer);
    };
}

//
// DirectSound Voice base class
//

namespace DirectSound
{
    class CDirectSoundVoice
        : public CRefCount
    {
    public:
        CDirectSound *              m_pDirectSound;     // Parent object
        CMcpxVoiceClient *          m_pVoice;           // Voice implementation
        CDirectSoundVoiceSettings * m_pSettings;        // Voice settings
        LIST_ENTRY                  m_le3dVoice;        // 3D voice list entry

    public:
        CDirectSoundVoice(CDirectSound *pDirectSound);
        virtual ~CDirectSoundVoice(void);

    public:
        // Initialization
        void STDMETHODCALLTYPE Initialize(CMcpxVoiceClient *pVoice, CDirectSoundVoiceSettings *pSettings);
    
        // IDirectSoundBuffer methods
        HRESULT STDMETHODCALLTYPE SetFormat(LPCWAVEFORMATEX pwfxFormat);
        HRESULT STDMETHODCALLTYPE SetVolume(LONG lVolume);
        HRESULT STDMETHODCALLTYPE SetFrequency(DWORD dwFrequency);
        HRESULT STDMETHODCALLTYPE SetPitch(LONG lPitch);
        HRESULT STDMETHODCALLTYPE SetLFO(LPCDSLFODESC pLFODesc);
        HRESULT STDMETHODCALLTYPE SetEG(LPCDSENVELOPEDESC pEnvelopeDesc);
        HRESULT STDMETHODCALLTYPE SetFilter(LPCDSFILTERDESC pFilterDesc);
        HRESULT STDMETHODCALLTYPE SetHeadroom(DWORD dwHeadroom);
        HRESULT STDMETHODCALLTYPE SetOutputBuffer(LPDIRECTSOUNDBUFFER pOutputBuffer);
        HRESULT STDMETHODCALLTYPE SetMixBins(LPCDSMIXBINS pMixBins);
        HRESULT STDMETHODCALLTYPE SetMixBinVolumes(LPCDSMIXBINS pMixBins);

        // IDirectSound3dBuffer methods
        HRESULT STDMETHODCALLTYPE SetAllParameters(LPCDS3DBUFFER pds3db, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetConeOrientation(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetConeOutsideVolume(LONG lConeOutsideVolume, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetMaxDistance(FLOAT flMaxDistance, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetMinDistance(FLOAT flMinDistance, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetMode(DWORD dwMode, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetDistanceFactor(FLOAT flDistanceFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetDopplerFactor(FLOAT flDopplerFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetRolloffFactor(FLOAT flRolloffFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetRolloffCurve(const FLOAT *pflPoints, DWORD dwPointCount, DWORD dwApply);
        HRESULT STDMETHODCALLTYPE SetI3DL2Source(LPCDSI3DL2BUFFER pds3db, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE CommitDeferredSettings(void);
    };
}

//
// DirectSound Buffer settings
//

namespace DirectSound
{
    class CDirectSoundBufferSettings
        : public CDirectSoundVoiceSettings
    {
    public:
        static const WAVEFORMATEX   m_wfxMixDest;           // Submix destination format

    public:
        LPVOID                      m_pvBufferData;         // Buffer audio data
        DWORD                       m_dwBufferSize;         // Buffer audio data size, in bytes
        DWORD                       m_dwPlayStart;          // Play region starting position, in bytes
        DWORD                       m_dwPlayLength;         // Play region length, in bytes
        DWORD                       m_dwLoopStart;          // Loop region starting position, in bytes
        DWORD                       m_dwLoopLength;         // Loop region length, in bytes
        DWORD                       m_dwInputMixBin;        // Submix input mixbin
        LPDSBPOSITIONNOTIFY         m_paNotifies;           // Notification array
        DWORD                       m_dwNotifyCount;        // Count of items in the above array

    public:
        CDirectSoundBufferSettings(void);
        virtual ~CDirectSoundBufferSettings(void);

    public:
        // Initialization
        HRESULT Initialize(LPCDSBUFFERDESC pdsbd);

        // Buffer data
        HRESULT SetBufferData(LPVOID pvBufferData, DWORD dwBufferSize);
        void SetPlayRegion(DWORD dwPlayStart, DWORD dwPlayLength);
        void SetLoopRegion(DWORD dwLoopStart, DWORD dwLoopLength);

        // Notifications
        HRESULT SetNotificationPositions(DWORD dwNotifyCount, LPCDSBPOSITIONNOTIFY paNotifies);
    };
}

//
// DirectSound Buffer
//

namespace DirectSound
{

#ifdef VALIDATE_PARAMETERS

    class CDirectSoundBuffer
        : public IDirectSoundBuffer, public CValidObject<'DSB '>, public CDirectSoundVoice

#else // VALIDATE_PARAMETERS

    class CDirectSoundBuffer
        : public IDirectSoundBuffer, public CDirectSoundVoice

#endif // VALIDATE_PARAMETERS

    {
        friend class CDirectSound;
        friend class CDirectSoundVoice;

    public:
        CDirectSoundBufferSettings *    m_pSettings;            // Buffer settings
        CMcpxBuffer *                   m_pBuffer;              // Buffer implementation

    public:
        CDirectSoundBuffer(CDirectSound *pDirectSound);
        virtual ~CDirectSoundBuffer(void);

    public:
        // Initialization
        HRESULT STDMETHODCALLTYPE Initialize(LPCDSBUFFERDESC pdsbd);
    
        // IUnknown methods
        ULONG STDMETHODCALLTYPE AddRef(void);
        ULONG STDMETHODCALLTYPE Release(void);

        // IDirectSoundBuffer methods
        HRESULT STDMETHODCALLTYPE SetFormat(LPCWAVEFORMATEX pwfxFormat);
        HRESULT STDMETHODCALLTYPE Play(DWORD dwReserved1, DWORD dwReserved2, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE PlayEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE Stop(void);
        HRESULT STDMETHODCALLTYPE StopEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetPlayRegion(DWORD dwPlayStart, DWORD dwPlayLength);
        HRESULT STDMETHODCALLTYPE SetLoopRegion(DWORD dwLoopStart, DWORD dwLoopLength);
        HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus);
        HRESULT STDMETHODCALLTYPE GetCurrentPosition(LPDWORD pdwPlayCursor, LPDWORD pdwWriteCursor);
        HRESULT STDMETHODCALLTYPE SetCurrentPosition(DWORD dwPlayCursor);
        HRESULT STDMETHODCALLTYPE SetBufferData(LPVOID pvBufferData, DWORD dwBufferBytes);
        HRESULT STDMETHODCALLTYPE Lock(DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetFrequency(DWORD dwFrequency);
        HRESULT STDMETHODCALLTYPE SetVolume(LONG lVolume);
        HRESULT STDMETHODCALLTYPE SetPitch(LONG lPitch);
        HRESULT STDMETHODCALLTYPE SetLFO(LPCDSLFODESC pLFODesc);
        HRESULT STDMETHODCALLTYPE SetEG(LPCDSENVELOPEDESC pEnvelopeDesc);
        HRESULT STDMETHODCALLTYPE SetFilter(LPCDSFILTERDESC pFilterDesc);
        HRESULT STDMETHODCALLTYPE SetHeadroom(DWORD dwHeadroom);
        HRESULT STDMETHODCALLTYPE SetOutputBuffer(LPDIRECTSOUNDBUFFER pOutputBuffer);
        HRESULT STDMETHODCALLTYPE SetMixBins(LPCDSMIXBINS pMixBins);
        HRESULT STDMETHODCALLTYPE SetMixBinVolumes(LPCDSMIXBINS pMixBins);

        // IDirectSound3dBuffer methods
        HRESULT STDMETHODCALLTYPE SetAllParameters(LPCDS3DBUFFER pds3db, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetConeOrientation(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetConeOutsideVolume(LONG lConeOutsideVolume, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetMaxDistance(FLOAT flMaxDistance, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetMinDistance(FLOAT flMinDistance, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetMode(DWORD dwMode, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetDistanceFactor(FLOAT flDistanceFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetDopplerFactor(FLOAT flDopplerFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetRolloffFactor(FLOAT flRolloffFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetRolloffCurve(const FLOAT *pflPoints, DWORD dwPointCount, DWORD dwApply);
        HRESULT STDMETHODCALLTYPE SetI3DL2Source(LPCDSI3DL2BUFFER pds3db, DWORD dwFlags);

        // IDirectSoundNotify methods
        HRESULT STDMETHODCALLTYPE SetNotificationPositions(DWORD dwNotifyCount, LPCDSBPOSITIONNOTIFY paNotifies);
    };    

    __inline ULONG CDirectSoundBuffer::AddRef(void)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::AddRef");
        return CRefCount::AddRef();
    }

    __inline ULONG CDirectSoundBuffer::Release(void)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::Release");
        return CRefCount::Release();
    }

    __inline HRESULT CDirectSoundBuffer::SetFormat(LPCWAVEFORMATEX pwfxFormat)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetFormat");
        return CDirectSoundVoice::SetFormat(pwfxFormat);
    }

    __inline HRESULT CDirectSoundBuffer::SetFrequency(DWORD dwFrequency)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetFrequency");
        return CDirectSoundVoice::SetFrequency(dwFrequency);
    }

    __inline HRESULT CDirectSoundBuffer::SetVolume(LONG lVolume)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetVolume");
        return CDirectSoundVoice::SetVolume(lVolume);
    }

    __inline HRESULT CDirectSoundBuffer::SetPitch(LONG lPitch)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetPitch");
        return CDirectSoundVoice::SetPitch(lPitch);
    }

    __inline HRESULT CDirectSoundBuffer::SetLFO(LPCDSLFODESC pLFODesc)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetLFO");
        return CDirectSoundVoice::SetLFO(pLFODesc);
    }

    __inline HRESULT CDirectSoundBuffer::SetEG(LPCDSENVELOPEDESC pEnvelopeDesc)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetEG");
        return CDirectSoundVoice::SetEG(pEnvelopeDesc);
    }

    __inline HRESULT CDirectSoundBuffer::SetFilter(LPCDSFILTERDESC pFilterDesc)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetFilter");
        return CDirectSoundVoice::SetFilter(pFilterDesc);
    }

    __inline HRESULT CDirectSoundBuffer::SetHeadroom(DWORD dwHeadroom)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetHeadroom");
        return CDirectSoundVoice::SetHeadroom(dwHeadroom);
    }

    __inline HRESULT CDirectSoundBuffer::SetOutputBuffer(LPDIRECTSOUNDBUFFER pOutputBuffer)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetOutputBuffer");
        return CDirectSoundVoice::SetOutputBuffer(pOutputBuffer);
    }

    __inline HRESULT CDirectSoundBuffer::SetMixBins(LPCDSMIXBINS pMixBins)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetMixBins");
        return CDirectSoundVoice::SetMixBins(pMixBins);
    }

    __inline HRESULT CDirectSoundBuffer::SetMixBinVolumes(LPCDSMIXBINS pMixBins)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetMixBinVolumes");
        return CDirectSoundVoice::SetMixBinVolumes(pMixBins);
    }

    __inline HRESULT CDirectSoundBuffer::SetAllParameters(LPCDS3DBUFFER pds3db, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetAllParameters");
        return CDirectSoundVoice::SetAllParameters(pds3db, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetConeAngles");
        return CDirectSoundVoice::SetConeAngles(dwInsideConeAngle, dwOutsideConeAngle, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetConeOrientation(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetConeOrientation");
        return CDirectSoundVoice::SetConeOrientation(x, y, z, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetConeOutsideVolume(LONG lConeOutsideVolume, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetConeOutsideVolume");
        return CDirectSoundVoice::SetConeOutsideVolume(lConeOutsideVolume, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetMaxDistance(FLOAT flMaxDistance, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetMaxDistance");
        return CDirectSoundVoice::SetMaxDistance(flMaxDistance, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetMinDistance(FLOAT flMinDistance, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetMinDistance");
        return CDirectSoundVoice::SetMinDistance(flMinDistance, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetMode(DWORD dwMode, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetMode");
        return CDirectSoundVoice::SetMode(dwMode, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetPosition");
        return CDirectSoundVoice::SetPosition(x, y, z, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetVelocity");
        return CDirectSoundVoice::SetVelocity(x, y, z, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetDistanceFactor(FLOAT flDistanceFactor, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetDistanceFactor");
        return CDirectSoundVoice::SetDistanceFactor(flDistanceFactor, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetDopplerFactor(FLOAT flDopplerFactor, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetDopplerFactor");
        return CDirectSoundVoice::SetDopplerFactor(flDopplerFactor, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetRolloffFactor(FLOAT flRolloffFactor, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetRolloffFactor");
        return CDirectSoundVoice::SetRolloffFactor(flRolloffFactor, dwFlags);
    }

    __inline HRESULT CDirectSoundBuffer::SetRolloffCurve(const FLOAT *pflPoints, DWORD dwPointCount, DWORD dwApply)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetRolloffCurve");
        return CDirectSoundVoice::SetRolloffCurve(pflPoints, dwPointCount, dwApply);
    }

    __inline HRESULT CDirectSoundBuffer::SetI3DL2Source(LPCDSI3DL2BUFFER pds3db, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundBuffer::SetI3DL2Source");
        return CDirectSoundVoice::SetI3DL2Source(pds3db, dwFlags);
    }
}

//
// DirectSound Stream settings
//

namespace DirectSound
{
    class CDirectSoundStreamSettings
        : public CDirectSoundVoiceSettings
    {
    public:
        DWORD                       m_dwMaxAttachedPackets; // Maximum number of packets in the queue
        LPFNXMEDIAOBJECTCALLBACK    m_pfnCallback;          // Packet completion callback routine
        LPVOID                      m_pvContext;            // Packet completion callback routine context
   
    public:
        CDirectSoundStreamSettings(void);
        virtual ~CDirectSoundStreamSettings(void);

    public:
        // Initialization
        HRESULT Initialize(LPCDSSTREAMDESC pdssd);
    };
}

//
// DirectSound Stream
//

namespace DirectSound
{

#ifdef VALIDATE_PARAMETERS

    class CDirectSoundStream
        : public IDirectSoundStream, public CValidObject<'DSS '>, public CDirectSoundVoice

#else // VALIDATE_PARAMETERS

    class CDirectSoundStream
        : public IDirectSoundStream, public CDirectSoundVoice

#endif // VALIDATE_PARAMETERS

    {
    public:
        CDirectSoundStreamSettings *    m_pSettings;        // Stream settings
        CMcpxStream *                   m_pStream;          // Stream implementation

    public:
        CDirectSoundStream(CDirectSound *pDirectSound);
        virtual ~CDirectSoundStream(void);

    public:
        // Initialization
        HRESULT STDMETHODCALLTYPE Initialize(LPCDSSTREAMDESC pdssd);
    
        // IUnknown methods
        virtual ULONG STDMETHODCALLTYPE AddRef(void);
        virtual ULONG STDMETHODCALLTYPE Release(void);

        // XMediaObject methods
        virtual HRESULT STDMETHODCALLTYPE GetInfo(LPXMEDIAINFO pInfo);
        virtual HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus);
        virtual HRESULT STDMETHODCALLTYPE Process(LPCXMEDIAPACKET pInputPacket, LPCXMEDIAPACKET pOutputPacket);
        virtual HRESULT STDMETHODCALLTYPE Discontinuity(void);
        virtual HRESULT STDMETHODCALLTYPE Flush(void);

        // IDirectSoundStream methods
        HRESULT STDMETHODCALLTYPE SetFormat(LPCWAVEFORMATEX pwfxFormat);
        HRESULT STDMETHODCALLTYPE SetFrequency(DWORD dwFrequency);
        HRESULT STDMETHODCALLTYPE SetPitch(LONG lPitch);
        HRESULT STDMETHODCALLTYPE SetVolume(LONG lVolume);
        HRESULT STDMETHODCALLTYPE SetLFO(LPCDSLFODESC pLFODesc);
        HRESULT STDMETHODCALLTYPE SetEG(LPCDSENVELOPEDESC pEnvelopeDesc);
        HRESULT STDMETHODCALLTYPE SetFilter(LPCDSFILTERDESC pFilterDesc);
        HRESULT STDMETHODCALLTYPE SetHeadroom(DWORD dwHeadroom);
        HRESULT STDMETHODCALLTYPE SetOutputBuffer(LPDIRECTSOUNDBUFFER pOutputBuffer);
        HRESULT STDMETHODCALLTYPE SetMixBins(LPCDSMIXBINS pMixBins);
        HRESULT STDMETHODCALLTYPE SetMixBinVolumes(LPCDSMIXBINS pMixBins);
        HRESULT STDMETHODCALLTYPE Pause(DWORD dwPause);
        HRESULT STDMETHODCALLTYPE PauseEx(REFERENCE_TIME rtTimeStamp, DWORD dwPause);
        HRESULT STDMETHODCALLTYPE FlushEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags);

        // IDirectSound3dBuffer methods
        HRESULT STDMETHODCALLTYPE SetAllParameters(LPCDS3DBUFFER pds3db, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetConeOrientation(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetConeOutsideVolume(LONG lConeOutsideVolume, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetMaxDistance(FLOAT flMaxDistance, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetMinDistance(FLOAT flMinDistance, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetMode(DWORD dwMode, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetDistanceFactor(FLOAT flDistanceFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetDopplerFactor(FLOAT flDopplerFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetRolloffFactor(FLOAT flRolloffFactor, DWORD dwFlags);
        HRESULT STDMETHODCALLTYPE SetRolloffCurve(const FLOAT *pflPoints, DWORD dwPointCount, DWORD dwApply);
        HRESULT STDMETHODCALLTYPE SetI3DL2Source(LPCDSI3DL2BUFFER pds3db, DWORD dwFlags);
    };

    __inline ULONG CDirectSoundStream::AddRef(void)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::AddRef");
        return CRefCount::AddRef();
    }

    __inline ULONG CDirectSoundStream::Release(void)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::Release");
        return CRefCount::Release();
    }

    __inline HRESULT CDirectSoundStream::SetFormat(LPCWAVEFORMATEX pwfxFormat)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetFormat");
        return CDirectSoundVoice::SetFormat(pwfxFormat);
    }

    __inline HRESULT CDirectSoundStream::SetFrequency(DWORD dwFrequency)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetFrequency");
        return CDirectSoundVoice::SetFrequency(dwFrequency);
    }

    __inline HRESULT CDirectSoundStream::SetPitch(LONG lPitch)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetPitch");
        return CDirectSoundVoice::SetPitch(lPitch);
    }

    __inline HRESULT CDirectSoundStream::SetVolume(LONG lVolume)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetVolume");
        return CDirectSoundVoice::SetVolume(lVolume);
    }

    __inline HRESULT CDirectSoundStream::SetLFO(LPCDSLFODESC pLFODesc)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetLFO");
        return CDirectSoundVoice::SetLFO(pLFODesc);
    }

    __inline HRESULT CDirectSoundStream::SetEG(LPCDSENVELOPEDESC pEnvelopeDesc)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetEG");
        return CDirectSoundVoice::SetEG(pEnvelopeDesc);
    }

    __inline HRESULT CDirectSoundStream::SetFilter(LPCDSFILTERDESC pFilterDesc)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetFilter");
        return CDirectSoundVoice::SetFilter(pFilterDesc);
    }

    __inline HRESULT CDirectSoundStream::SetHeadroom(DWORD dwHeadroom)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetHeadroom");
        return CDirectSoundVoice::SetHeadroom(dwHeadroom);
    }

    __inline HRESULT CDirectSoundStream::SetOutputBuffer(LPDIRECTSOUNDBUFFER pOutputBuffer)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetOutputBuffer");
        return CDirectSoundVoice::SetOutputBuffer(pOutputBuffer);
    }

    __inline HRESULT CDirectSoundStream::SetMixBins(LPCDSMIXBINS pMixBins)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetMixBins");
        return CDirectSoundVoice::SetMixBins(pMixBins);
    }

    __inline HRESULT CDirectSoundStream::SetMixBinVolumes(LPCDSMIXBINS pMixBins)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetMixBinVolumes");
        return CDirectSoundVoice::SetMixBinVolumes(pMixBins);
    }

    __inline HRESULT CDirectSoundStream::SetAllParameters(LPCDS3DBUFFER pds3db, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetAllParameters");
        return CDirectSoundVoice::SetAllParameters(pds3db, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetConeAngles");
        return CDirectSoundVoice::SetConeAngles(dwInsideConeAngle, dwOutsideConeAngle, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetConeOrientation(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetConeOrientation");
        return CDirectSoundVoice::SetConeOrientation(x, y, z, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetConeOutsideVolume(LONG lConeOutsideVolume, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetConeOutsideVolume");
        return CDirectSoundVoice::SetConeOutsideVolume(lConeOutsideVolume, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetMaxDistance(FLOAT flMaxDistance, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetMaxDistance");
        return CDirectSoundVoice::SetMaxDistance(flMaxDistance, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetMinDistance(FLOAT flMinDistance, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetMinDistance");
        return CDirectSoundVoice::SetMinDistance(flMinDistance, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetMode(DWORD dwMode, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetMode");
        return CDirectSoundVoice::SetMode(dwMode, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetPosition(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetPosition");
        return CDirectSoundVoice::SetPosition(x, y, z, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetVelocity(FLOAT x, FLOAT y, FLOAT z, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetVelocity");
        return CDirectSoundVoice::SetVelocity(x, y, z, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetDistanceFactor(FLOAT flDistanceFactor, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetDistanceFactor");
        return CDirectSoundVoice::SetDistanceFactor(flDistanceFactor, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetDopplerFactor(FLOAT flDopplerFactor, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetDopplerFactor");
        return CDirectSoundVoice::SetDopplerFactor(flDopplerFactor, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetRolloffFactor(FLOAT flRolloffFactor, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetRolloffFactor");
        return CDirectSoundVoice::SetRolloffFactor(flRolloffFactor, dwFlags);
    }

    __inline HRESULT CDirectSoundStream::SetRolloffCurve(const FLOAT *pflPoints, DWORD dwPointCount, DWORD dwApply)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetRolloffCurve");
        return CDirectSoundVoice::SetRolloffCurve(pflPoints, dwPointCount, dwApply);
    }

    __inline HRESULT CDirectSoundStream::SetI3DL2Source(LPCDSI3DL2BUFFER pds3db, DWORD dwFlags)
    {
        _ENTER_EXTERNAL_METHOD("CDirectSoundStream::SetI3DL2Source");
        return CDirectSoundVoice::SetI3DL2Source(pds3db, dwFlags);
    }
}

#endif // __cplusplus

#endif // __DSAPI_H__
