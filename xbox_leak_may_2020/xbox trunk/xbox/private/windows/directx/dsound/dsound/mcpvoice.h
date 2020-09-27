/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mcpvoice.h
 *  Content:    MCP-X audio device objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/09/01    dereks  Created based on NVidia/georgioc code.
 *
 ****************************************************************************/

#ifndef __MCPVOICE_H__
#define __MCPVOICE_H__

//
// Voice status
//

#define MCPX_VOICESTATUS_ALLOCATED          0x0001
#define MCPX_VOICESTATUS_ACTIVE             0x0002
#define MCPX_VOICESTATUS_PAUSED             0x0004
#define MCPX_VOICESTATUS_NOTEOFF            0x0008
#define MCPX_VOICESTATUS_PERSIST            0x0010
#define MCPX_VOICESTATUS_VOICEOFF           0x8000

//
// Buffer status
//

#define MCPX_VOICESTATUS_BUFFERMAPPED       0x0100
#define MCPX_VOICESTATUS_LOOPING            0x0200

//
// Stream status
//

#define MCPX_VOICESTATUS_STARVED            0x0400
#define MCPX_VOICESTATUS_DISCONTINUITY      0x0800

//
// Status masks
//
                                            
#define MCPX_VOICESTATUS_ACTIVEMASK         (MCPX_VOICESTATUS_ALLOCATED | MCPX_VOICESTATUS_ACTIVE)
#define MCPX_VOICESTATUS_PAUSEDMASK         (MCPX_VOICESTATUS_ACTIVEMASK | MCPX_VOICESTATUS_PAUSED)
#define MCPX_VOICESTATUS_NOTEOFFMASK        (MCPX_VOICESTATUS_ACTIVEMASK | MCPX_VOICESTATUS_NOTEOFF)
#define MCPX_VOICESTATUS_VOICEOFFMASK       (MCPX_VOICESTATUS_ALLOCATED | MCPX_VOICESTATUS_VOICEOFF)
#define MCPX_VOICESTATUS_ACTIVEORVOICEOFF   (MCPX_VOICESTATUS_ACTIVE | MCPX_VOICESTATUS_VOICEOFF)

#define MCPX_VOICESTATUS_BUFFERMAPPEDMASK   (MCPX_VOICESTATUS_ALLOCATED | MCPX_VOICESTATUS_BUFFERMAPPED)
#define MCPX_VOICESTATUS_LOOPINGMASK        (MCPX_VOICESTATUS_ACTIVEMASK | MCPX_VOICESTATUS_LOOPING)

#define MCPX_VOICESTATUS_DISCONTINUITYMASK  (MCPX_VOICESTATUS_ACTIVEMASK | MCPX_VOICESTATUS_DISCONTINUITY)

#define MCPX_VOICESTATUS_ALLPAUSEDMASK      (MCPX_VOICESTATUS_PAUSED | MCPX_VOICESTATUS_STARVED)
#define MCPX_VOICESTATUS_DEACTIVATEMASK     (MCPX_VOICESTATUS_ALLOCATED | MCPX_VOICESTATUS_BUFFERMAPPED | MCPX_VOICESTATUS_PERSIST)
#define MCPX_VOICESTATUS_RELEASEMASK        MCPX_VOICESTATUS_PERSIST

//
// Cached voice registers
//

BEGIN_DEFINE_STRUCT()
    DWORD   CfgFMT;
    DWORD   CfgMISC;
    DWORD   CfgENV0;
    DWORD   CfgENVA;
    DWORD   CfgENV1;
    DWORD   CfgENVF;
    DWORD   CfgLFODLY;
    DWORD   LFOENV;
    DWORD   LFOMOD;
    DWORD   TarFCA;
    DWORD   TarFCB;
    DWORD   VoiceOn;
END_DEFINE_STRUCT(MCPX_VOICE_REGCACHE);

//
// Volume data
//

BEGIN_DEFINE_STRUCT()
    DWORD   TarVOLA[MCPX_MAX_VOICE_CHANNELS];
    DWORD   TarVOLB[MCPX_MAX_VOICE_CHANNELS];
    DWORD   TarVOLC[MCPX_MAX_VOICE_CHANNELS];
END_DEFINE_STRUCT(MCPX_VOICE_VOLUME);

//
// Deferred voice commands
//

BEGIN_DEFINE_ENUM()
    MCPX_DEFERREDCMD_VOICE_CHECKSTUCK = 0,
    MCPX_DEFERREDCMD_VOICE_COUNT,
END_DEFINE_ENUM_();

#ifdef __cplusplus

//
// MCPX voice notifier wrapper class
//

namespace DirectSound
{
    class CMcpxVoiceNotifier
        : public CMcpxNotifier
    {
    public:
        // Initialization
        void Initialize(MCPX_VOICE_HANDLE hVoice);
    };

    __inline void CMcpxVoiceNotifier::Initialize(MCPX_VOICE_HANDLE hVoice)
    {
        CMcpxNotifier::Initialize(MCPX_NOTIFIER_BASE_OFFSET + ((DWORD)hVoice * MCPX_NOTIFIER_COUNT), MCPX_NOTIFIER_COUNT);
    }
}

//
// Base class for buffers and streams
//

namespace DirectSound
{
    class CMcpxVoiceClient
        : public CRefCount
    {
        friend class CMcpxAPU;

    public:
        static DWORD                m_dwStuckVoiceCount;                    // Number of times a voice has had to be un-stuck

    protected:
        CMcpxAPU *                  m_pMcpxApu;                             // Pointer back to the core MCPX object
        MCPX_VOICE_HANDLE           m_ahVoices[MCPX_MAX_VOICE_CHANNELS];    // Hardware voice array
        volatile WORD               m_dwStatus;                             // Voice status (WORD for alignment)
        MCPX_VOICE_REGCACHE         m_RegCache;                             // Cached voice registers
        LIST_ENTRY                  m_lstSourceVoices;                      // Source voice list for mix-in voices
        LIST_ENTRY                  m_leActiveVoice;                        // Active voice list entry
        LIST_ENTRY                  m_lePendingInactiveVoice;               // Pending inactive voice list entry
        LIST_ENTRY                  m_leSourceVoice;                        // Source voice list entry
        BYTE                        m_bVoiceCount;                          // Hardware voice count
        BYTE                        m_bVoiceList;                           // Which voice list is the voice in?
        BYTE                        m_bAvailable3dFilter;                   // Available 3D filter index
        BYTE                        m_bAlign0;                              // 32-bit alignment
        CMcpxVoiceNotifier          m_Notifier;                             // Voice notifier wrapper
        CHrtfSource *               m_pHrtfSource;                          // HRTF source object
        CI3dl2Source *              m_pI3dl2Source;                         // I3DL2 source object
                                                                                    
    private:                                                                            
        CDirectSoundVoiceSettings * m_pSettings;                            // Shared voice settings
        DWORD                       m_dw3dMode;                             // Cached 3D mode
        REFERENCE_TIME              m_rtVoiceOff;                           // Time VoiceOff method was called

#ifdef MCPX_DEBUG_STUCK_VOICES

        DWORD                       m_dwIgnoredTraps;                       // Number of times we've had to ignore SE2FE_IDLE_VOICE

#endif // MCPX_DEBUG_STUCK_VOICES

    public:
        CMcpxVoiceClient(CMcpxAPU *pAPU, CDirectSoundVoiceSettings *pSettings);
        virtual ~CMcpxVoiceClient(void);

    public:
        // Initialization
        virtual HRESULT Initialize(BOOL fStream);
        virtual HRESULT SetFormat(void);
    
        // Voice properties
        HRESULT SetMixBins(void);
        HRESULT SetVolume(void);
        HRESULT SetPitch(void);
        HRESULT SetLFO(LPCDSLFODESC pLfo);
        HRESULT SetEG(LPCDSENVELOPEDESC pEnv);
        HRESULT SetFilter(LPCDSFILTERDESC pFilterDesc);
        HRESULT ConnectVoice(void);
        HRESULT DisconnectVoice(void);

#ifndef MCPX_BOOT_LIB

        // 3D object properties
        DWORD Commit3dSettings(void);

#endif // MCPX_BOOT_LIB

    protected:
        // Voice state
        void ActivateVoice(void);
        void DeactivateVoice(BOOL fBlock = FALSE);
        void ReleaseVoice(void);
        void PauseVoice(DWORD dwStatus);
        void WaitForVoiceOff(void);
        void CheckStuckVoice(void);
    
        // Register formatting
        void ConvertMixBinValues(LPDWORD pdwVBin, LPDWORD pdwVoiceFormat);
        void ConvertVolumeValues(LPMCPX_VOICE_VOLUME pVolume);
        void ConvertPitchValue(LPDWORD pdwPitch);
        DWORD GetVoiceCfgFMT(DWORD dwVoiceIndex);

        // Hardware voice resources
        HRESULT AllocateVoiceResources(void);
        void ReleaseVoiceResources(void);
    
#ifndef MCPX_BOOT_LIB
    
        // 3D
        void Apply3dSettings(DWORD dw3dChangeMask, DWORD dwMixBinChangeMask, DWORD dwI3dl2ChangeMask);
        void LoadHrtfFilter(void);

#endif // MCPX_BOOT_LIB

	    // Voice Format
	    DWORD BytesToSamples(DWORD dwBytes);
        DWORD SamplesToBytes(DWORD dwSamples);

        // Voice SSL position
        DWORD GetSslPosition(void);

        // Submixing
        CMcpxBuffer *GetSubMixDestination(void);

        // Interrupt handler
        virtual BOOL ServiceVoiceInterrupt(void) = 0;

        // Deferred commands
        virtual BOOL ScheduleDeferredCommand(DWORD dwCommand, REFERENCE_TIME rtTimeStamp, DWORD dwContext) = 0;
        virtual void RemoveDeferredCommand(DWORD dwCommand) = 0;
        virtual void ServiceDeferredCommand(DWORD dwCommand, DWORD dwContext) = 0;

    private:
        // Voice state
        void GetAntecedentVoice(LPBYTE pbVoiceList, CMcpxVoiceClient **ppAntecedentVoice);
        void RemoveIdleVoice(BOOL fScheduleDpc = TRUE);
        void RemoveStuckVoice(void);
    };

    __inline DWORD CMcpxVoiceClient::BytesToSamples(DWORD dwBytes)
    {
        if(WAVE_FORMAT_XBOX_ADPCM == m_pSettings->m_fmt.wFormatTag)
        {
            return (dwBytes / m_pSettings->m_fmt.nBlockAlign) << 6;
        }
        else
        {
            ASSERT(WAVE_FORMAT_PCM == m_pSettings->m_fmt.wFormatTag);
        
            return dwBytes / m_pSettings->m_fmt.nBlockAlign;
        }
    }

    __inline DWORD CMcpxVoiceClient::SamplesToBytes(DWORD dwSamples)
    {
	    if(WAVE_FORMAT_XBOX_ADPCM == m_pSettings->m_fmt.wFormatTag)
        {
            return (dwSamples >> 6) * m_pSettings->m_fmt.nBlockAlign;
        }
        else
        {
            ASSERT(WAVE_FORMAT_PCM == m_pSettings->m_fmt.wFormatTag);
        
            return dwSamples * m_pSettings->m_fmt.nBlockAlign;
        }
    }

    __inline DWORD CMcpxVoiceClient::GetVoiceCfgFMT(DWORD dwVoiceIndex)
    {
        DWORD                   dwCfgFMT    = m_RegCache.CfgFMT;

        if(dwVoiceIndex)
        {
            dwCfgFMT |= MCPX_MAKE_REG_VALUE(NV_PAVS_VOICE_CFG_FMT_LINKED_VOICE_ON, NV1BA0_PIO_SET_VOICE_CFG_FMT_LINKED_VOICE);
        }

        return dwCfgFMT;
    }

    __inline CMcpxBuffer *CMcpxVoiceClient::GetSubMixDestination(void)
    {
        if(!m_pSettings->m_pMixinBuffer)
        {
            return NULL;
        }

        ASSERT(!(m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK));
        ASSERT(m_pSettings->m_pMixinBuffer->m_pSettings->m_dwFlags & DSBCAPS_SUBMIXMASK);

        return m_pSettings->m_pMixinBuffer->m_pBuffer;
    }
}

#endif // __cplusplus

#endif // __MCPVOICE_H__
